
#include "LaunchEngineLoop.h"
#include "ParticleHelper.h"
#include "Components/Mesh/StaticMesh.h"
#include "Container/Array.h"
#include "Math/Matrix.h"
#include "Renderer/Renderer.h"
#include "Renderer/VBIBTopologyMapping.h"
#include "Renderer/RenderPass/ParticleMeshRenderPass.h"
#include "Renderer/RenderPass/ParticleRenderPass.h"

FVector2D GetParticleSize(const FBaseParticle& Particle, const FDynamicSpriteEmitterReplayDataBase& Source)
{
    FVector2D Size;
    Size.X = FMath::Abs(Particle.Size.X * Source.Scale.X);
    Size.Y = FMath::Abs(Particle.Size.Y * Source.Scale.Y);
    // if (Source.ScreenAlignment == PSA_Square || Source.ScreenAlignment == PSA_FacingCameraPosition || Source.ScreenAlignment == PSA_FacingCameraDistanceBlend)
    // {
    //     Size.Y = Size.X;
    // }

    return Size;
}

void FParticleDataContainer::Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts)
{
    ParticleDataNumBytes     = InParticleDataNumBytes;
    ParticleIndicesNumShorts = InParticleIndicesNumShorts;
    // 전체 바이트 = 데이터 영역 + 인덱스 배열 크기
    MemBlockSize = ParticleDataNumBytes + sizeof(uint16) * ParticleIndicesNumShorts;

    // malloc 으로 한 번에 할당
    ParticleData = static_cast<uint8*>(std::malloc(MemBlockSize));
    if (!ParticleData)
    {
        // 할당 실패 처리 (로그, 예외 등)
        throw std::bad_alloc();
    }

    // 인덱스 배열은 데이터 블록 끝에 위치
    ParticleIndices = reinterpret_cast<uint16*>(ParticleData + ParticleDataNumBytes);
}

void FParticleDataContainer::Free()
{
    if (ParticleData)
    {
        std::free(ParticleData);
        ParticleData    = nullptr;
        ParticleIndices = nullptr;
    }
    MemBlockSize             = 0;
    ParticleDataNumBytes     = 0;
    ParticleIndicesNumShorts = 0;
}

void FDynamicEmitterDataBase::ExecuteRender(const FMatrix& ViewProj) const
{
    

}

void FDynamicSpriteEmitterDataBase::SortSpriteParticles(int32 SortMode, bool bLocalSpace, 
                                                        int32 ParticleCount, const uint8* ParticleData, int32 ParticleStride, const uint16* ParticleIndices,
                                                        const FMatrix* ViewProjection, const FMatrix& LocalToWorld, TArray<FParticleOrder>& ParticleOrder) const
{

    //원본 코드대로 이미 ParticleOrder 크기 가 ParticleCount로 설정되어 있다고 가정
    //ParticleOrder.SetNumUninitialized(ParticleCount);
    
    for (int32 ParticleIndex = 0; ParticleIndex < ParticleCount; ParticleIndex++)
    {
        FBaseParticle& Particle = *((FBaseParticle*)(ParticleData + ParticleStride * ParticleIndices[ParticleIndex]));
            
        float InZ;
        if (bLocalSpace)
        {
            InZ = ViewProjection->TransformPositionFVector4(LocalToWorld.TransformPositionFVector4(Particle.Location).xyz()).W;
        }
        else
        {
            InZ = ViewProjection->TransformPositionFVector4(Particle.Location).W;
        }
        ParticleOrder[ParticleIndex].ParticleIndex = ParticleIndex;

        ParticleOrder[ParticleIndex].Z = InZ;
    }
    ParticleOrder.Sort([](const FParticleOrder& A, const FParticleOrder& B) {
    // Z 값이 큰 것이 앞으로 오도록 (내림차순)
    return A.Z > B.Z;
    });
}

void FDynamicSpriteEmitterDataBase::SortSpriteParticles(int32 SortMode, bool bLocalSpace, TArray<FBaseParticle>& ParticleData,
    const FMatrix* ViewProjection, const FMatrix& LocalToWorld, TArray<FParticleOrder>& ParticleOrder) const
{
    for (int32 ParticleIndex = 0; ParticleIndex < ParticleData.Num(); ParticleIndex++)
    {
        FBaseParticle& Particle = ParticleData[ParticleIndex];
            
        float InZ;
        if (bLocalSpace)
        {
            InZ = ViewProjection->TransformPositionFVector4(LocalToWorld.TransformPositionFVector4(Particle.Location).xyz()).W;
        }
        else
        {
            InZ = ViewProjection->TransformPositionFVector4(Particle.Location).W;
        }
        ParticleOrder[ParticleIndex].ParticleIndex = ParticleIndex;

        ParticleOrder[ParticleIndex].Z = InZ;
    }
    ParticleOrder.Sort([](const FParticleOrder& A, const FParticleOrder& B) {
    // Z 값이 큰 것이 앞으로 오도록 (내림차순)
    return A.Z > B.Z;
    });
}

bool FDynamicSpriteEmitterData::GetVertexAndIndexData(void* VertexData,  void* FillIndexData,
                                                      TArray<FParticleOrder>* ParticleOrder, uint32 InstanceFactor) const
{
    int32 ParticleCount = Source.ActiveParticleCount;
    if ((Source.MaxDrawCount >= 0) && (ParticleCount > Source.MaxDrawCount))
    {
        ParticleCount = Source.MaxDrawCount;
    }

    
    int32	ParticleIndex;
    
    int32 VertexStride = sizeof(FParticleSpriteVertex);
	uint8* TempVert = (uint8*)VertexData;
	FParticleSpriteVertex* FillVertex;
    
    FVector ParticlePosition;
    FVector ParticleOldPosition;
    float SubImageIndex = 0.0f;

    const uint8* ParticleData = Source.DataContainer.ParticleData;
    const uint16* ParticleIndices = Source.DataContainer.ParticleIndices;
    const TArray<FParticleOrder>* OrderedIndices = ParticleOrder;


	for (int32 i = 0; i < ParticleCount; i++)
	{
		ParticleIndex = OrderedIndices ? (*OrderedIndices)[i].ParticleIndex : i;
		DECLARE_PARTICLE_CONST(Particle, ParticleData + Source.ParticleStride * ParticleIndices[ParticleIndex]);

		const FVector2D Size = GetParticleSize(Particle, Source);

		ParticlePosition = Particle.Location;
		ParticleOldPosition = Particle.OldLocation;
	    

		for (uint32 Factor = 0; Factor < InstanceFactor; Factor++)
		{
			FillVertex = (FParticleSpriteVertex*)TempVert;
			FillVertex->Position = ParticlePosition;
			FillVertex->RelativeTime = Particle.RelativeTime;
			FillVertex->OldPosition = ParticleOldPosition;
			// Create a floating point particle ID from the counter, map into approximately 0-1
			//FillVertex->ParticleId = (Particle.Flags & STATE_CounterMask) / 10000.0f;
		    FillVertex->ParticleId = (float)(Particle.Flags) / 10000.0f;
			FillVertex->Size = FVector2D(GetParticleSizeWithUVFlipInSign(Particle, Size));
			FillVertex->Rotation = Particle.Rotation;
			FillVertex->SubImageIndex = SubImageIndex;
			FillVertex->Color = Particle.Color;

			TempVert += VertexStride;
		}
	}
	return true;
}

bool FDynamicSpriteEmitterData::GetVertexAndIndexData(void* VertexData, void* FillIndexData, TArray<FParticleOrder>* ParticleOrder,
    TArray<FBaseParticle>& ParticleData, const FVector& InCameraPosition, const FMatrix& InLocalToWorld, uint32 InstanceFactor) const
{
        int32 ParticleCount = Source.ActiveParticleCount;
    if ((Source.MaxDrawCount >= 0) && (ParticleCount > Source.MaxDrawCount))
    {
        ParticleCount = Source.MaxDrawCount;
    }

    
    int32	ParticleIndex;
    
    int32 VertexStride = sizeof(FParticleSpriteVertex);
	uint8* TempVert = (uint8*)VertexData;
	FParticleSpriteVertex* FillVertex;
    
    FVector ParticlePosition;
    FVector ParticleOldPosition;
    float SubImageIndex = 0.0f;

    //const uint8* ParticleData = Source.DataContainer.ParticleData;
    //const uint16* ParticleIndices = Source.DataContainer.ParticleIndices;
    const TArray<FParticleOrder>* OrderedIndices = ParticleOrder;


	for (int32 i = 0; i < ParticleCount; i++)
	{
		ParticleIndex = OrderedIndices ? (*OrderedIndices)[i].ParticleIndex : i;
	    FBaseParticle& Particle = ParticleData[ParticleIndex];
		//DECLARE_PARTICLE_CONST(Particle, ParticleData + Source.ParticleStride * ParticleIndices[ParticleIndex]);

		const FVector2D Size = GetParticleSize(Particle, Source);

		ParticlePosition = Particle.Location;
		ParticleOldPosition = Particle.OldLocation;

	    // 공전 모듈 계산
		//ApplyOrbitToPosition(Particle, Source, InLocalToWorld, ParticlePosition, ParticleOldPosition);

	    // 카메라 오프셋 용
	    
		// if (Source.CameraPayloadOffset != 0)
		// {
		// 	FVector CameraOffset = GetCameraOffsetFromPayload(Source.CameraPayloadOffset, Particle, ParticlePosition, CameraPosition);
		// 	ParticlePosition += CameraOffset;
		// 	ParticleOldPosition += CameraOffset;
		// }
	    
	    // 아틀라스 용

		// if (Source.SubUVDataOffset > 0)
		// {
		// 	FFullSubUVPayload* SubUVPayload = (FFullSubUVPayload*)(((uint8*)&Particle) + Source.SubUVDataOffset);
		// 	SubImageIndex = SubUVPayload->ImageIndex;
		// }


		for (uint32 Factor = 0; Factor < InstanceFactor; Factor++)
		{
			FillVertex = (FParticleSpriteVertex*)TempVert;
			FillVertex->Position = ParticlePosition;
			FillVertex->RelativeTime = Particle.RelativeTime;
			FillVertex->OldPosition = ParticleOldPosition;
			// Create a floating point particle ID from the counter, map into approximately 0-1
			//FillVertex->ParticleId = (Particle.Flags & STATE_CounterMask) / 10000.0f;
		    FillVertex->ParticleId = (float)(Particle.Flags) / 10000.0f;
			FillVertex->Size = FVector2D(GetParticleSizeWithUVFlipInSign(Particle, Size));
			FillVertex->Rotation = Particle.Rotation;
			FillVertex->SubImageIndex = SubImageIndex;
			FillVertex->Color = Particle.Color;

			TempVert += VertexStride;
		}
	}

	return true;
}

void FDynamicSpriteEmitterData::ExecuteRender(const FMatrix& ViewProj) const
{
    // 업데이트 전에 sorting
    // Sort and generate particles for this view.
    const FDynamicSpriteEmitterReplayDataBase* SourceData = GetSourceData();
    if (!SourceData || SourceData->ActiveParticleCount == 0)
    {
        return; // 렌더링할 파티클 없음
    }
    
    int32 ParticleCount = SourceData->ActiveParticleCount;
    if ((SourceData->MaxDrawCount >= 0) && (ParticleCount > SourceData->MaxDrawCount))
    {
        ParticleCount = SourceData->MaxDrawCount;
    }
    
    if (ParticleCount == 0)
    {
        return;
    }
    
    if (UMaterial* Material = SourceData->Material)
    {
        FParticleRenderPass::UpdateMaterialConstants(Material->GetMaterialInfo());
        //MaterialResource = MaterialInterface->GetRenderProxy();
    }
    
    TArray<FParticleOrder> ParticleOrders;

    ParticleOrders.SetNum(ParticleCount);
    SortSpriteParticles(true, SourceData->bUseLocalSpace,ParticleCount, SourceData->DataContainer.ParticleData,
        SourceData->ParticleStride, SourceData->DataContainer.ParticleIndices, &ViewProj, SourceData->ComponentLocalToWorld,ParticleOrders);

    
    ID3D11Buffer* VB = SourceData->ParticleEmitterRenderData.DynamicInstanceVertexBuffer;

    D3D11_MAPPED_SUBRESOURCE sub = {};
    const HRESULT hr = FEngineLoop::GraphicDevice.DeviceContext->Map(VB, 0,D3D11_MAP_WRITE_DISCARD,0, &sub);
    if (FAILED(hr))
    {
        // 실패 시 메시지 박스 표시
        std::string errorMessage = "Map failed for Dynamic DynamicInstanceVertexBuffer type: ";
        MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
        return;
    }
    GetVertexAndIndexData(sub.pData, nullptr, &ParticleOrders,  1);
    FEngineLoop::GraphicDevice.DeviceContext->Unmap(VB, 0);

    const UINT Stride = sizeof(FParticleSpriteVertex);
    const UINT Offset = 0;
    FEngineLoop::GraphicDevice.DeviceContext->IASetVertexBuffers(0, 1, &VB, &Stride, &Offset);
    if (Source.Material)
    {
        auto texture = GEngineLoop.ResourceManager
                           .GetTexture(Source
                                           .Material
                                           ->GetMaterialInfo()
                                           .DiffuseTexturePath);
        if (texture)
             FEngineLoop::GraphicDevice.DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
    }
    FEngineLoop::GraphicDevice.DeviceContext->DrawInstanced(4, ParticleCount, 0, 0);
}

void FDynamicMeshEmitterData::GetInstanceData(void* InstanceData, uint32 InstanceFactor, const TArray<FParticleOrder>* ParticleOrder) const
{
    int32 ParticleCount = Source.ActiveParticleCount;
    if ((Source.MaxDrawCount >= 0) && (ParticleCount > Source.MaxDrawCount))
    {
        ParticleCount = Source.MaxDrawCount;
    }

    
    int32	ParticleIndex;
    
    int32 VertexStride = sizeof(FParticleSpriteVertex);
    uint8* TempVert = (uint8*)InstanceData;
    FParticleSpriteVertex* FillVertex;
    
    FVector ParticlePosition;
    FVector ParticleOldPosition;
    float SubImageIndex = 0.0f;

    const uint8* ParticleData = Source.DataContainer.ParticleData;
    const uint16* ParticleIndices = Source.DataContainer.ParticleIndices;
    const TArray<FParticleOrder>* OrderedIndices = ParticleOrder;


    for (int32 i = 0; i < ParticleCount; i++)
    {
        ParticleIndex = OrderedIndices ? (*OrderedIndices)[i].ParticleIndex : i;
        DECLARE_PARTICLE_CONST(Particle, ParticleData + Source.ParticleStride * ParticleIndices[ParticleIndex]);

        const FVector2D Size = GetParticleSize(Particle, Source);

        ParticlePosition = Particle.Location;
        ParticleOldPosition = Particle.OldLocation;
	    

        for (uint32 Factor = 0; Factor < InstanceFactor; Factor++)
        {
            FillVertex = (FParticleSpriteVertex*)TempVert;
            FillVertex->Position = ParticlePosition;
            FillVertex->RelativeTime = Particle.RelativeTime;
            FillVertex->OldPosition = ParticleOldPosition;
            // Create a floating point particle ID from the counter, map into approximately 0-1
            //FillVertex->ParticleId = (Particle.Flags & STATE_CounterMask) / 10000.0f;
            FillVertex->ParticleId = (float)(Particle.Flags) / 10000.0f;
            FillVertex->Size = FVector2D(GetParticleSizeWithUVFlipInSign(Particle, Size));
            FillVertex->Rotation = Particle.Rotation;
            FillVertex->SubImageIndex = SubImageIndex;
            FillVertex->Color = Particle.Color;

            TempVert += VertexStride;
        }
    }
    return ;
    
}

void FDynamicMeshEmitterData::ExecuteRender(const FMatrix& ViewProj) const
{
    // 업데이트 전에 sorting
    // Sort and generate particles for this view.
    const FDynamicMeshEmitterReplayData* SourceData = &static_cast<const FDynamicMeshEmitterReplayData&>(GetSource());
    if (!SourceData || SourceData->ActiveParticleCount == 0)
    {
        return; // 렌더링할 파티클 없음
    }
    
    int32 ParticleCount = SourceData->ActiveParticleCount;
    if ((SourceData->MaxDrawCount >= 0) && (ParticleCount > SourceData->MaxDrawCount))
    {
        ParticleCount = SourceData->MaxDrawCount;
    }
    
    if (ParticleCount == 0)
    {
        return;
    }
    
    if (UMaterial* Material = SourceData->Material)
    {
        FParticleMeshRenderPass::UpdateMaterialConstants(Material->GetMaterialInfo());
        //MaterialResource = MaterialInterface->GetRenderProxy();
    }
    
    TArray<FParticleOrder> ParticleOrders;

    ParticleOrders.SetNum(ParticleCount);
    SortSpriteParticles(true, SourceData->bUseLocalSpace,ParticleCount, SourceData->DataContainer.ParticleData,
        SourceData->ParticleStride, SourceData->DataContainer.ParticleIndices, &ViewProj, SourceData->ComponentLocalToWorld,ParticleOrders);

    
    ID3D11Buffer* InstanceVB = SourceData->ParticleEmitterRenderData.DynamicInstanceVertexBuffer;

    D3D11_MAPPED_SUBRESOURCE sub = {};
    const HRESULT hr = FEngineLoop::GraphicDevice.DeviceContext->Map(InstanceVB, 0,D3D11_MAP_WRITE_DISCARD,0, &sub);
    if (FAILED(hr))
    {
        // 실패 시 메시지 박스 표시
        std::string errorMessage = "Map failed for Dynamic DynamicInstanceVertexBuffer type: ";
        MessageBoxA(nullptr, errorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
        return;
    }
    GetInstanceData(sub.pData,  1 , &ParticleOrders);
    FEngineLoop::GraphicDevice.DeviceContext->Unmap(InstanceVB, 0);


    const OBJ::FStaticMeshRenderData* renderData = StaticMesh->GetRenderData();

    // VIBuffer Bind
    const std::shared_ptr<FVBIBTopologyMapping> VBIBTopMappingInfo = FEngineLoop::Renderer.GetVBIBTopologyMapping(renderData->DisplayName);
    VBIBTopMappingInfo->Bind();

    


    


    const UINT Stride = sizeof(FParticleSpriteVertex);
    const UINT Offset = 0;
    FEngineLoop::GraphicDevice.DeviceContext->IASetVertexBuffers(1, 1, &InstanceVB, &Stride, &Offset);

    if (Source.Material)
    {
        auto texture = GEngineLoop.ResourceManager
                           .GetTexture(Source
                                           .Material
                                           ->GetMaterialInfo()
                                           .DiffuseTexturePath);
        if (texture)
             FEngineLoop::GraphicDevice.DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
    }
    
    UINT IndexCount = VBIBTopMappingInfo->GetNumIndices();
    
    // // SubSet마다 Material Update 및 Draw
    for (int subMeshIndex = 0; subMeshIndex < renderData->MaterialSubsets.Num(); ++subMeshIndex)
    {
        const int materialIndex = renderData->MaterialSubsets[subMeshIndex].MaterialIndex;
            
        FParticleMeshRenderPass::UpdateMaterialConstants(StaticMesh->GetMaterials()[subMeshIndex]->Material->GetMaterialInfo());
    
        // index draw
        const uint64 startIndex = renderData->MaterialSubsets[subMeshIndex].IndexStart;
        const uint64 indexCount = renderData->MaterialSubsets[subMeshIndex].IndexCount;
        if (IndexCount > 0)
        {
            FEngineLoop::GraphicDevice.DeviceContext->DrawIndexedInstanced(indexCount, ParticleCount, startIndex, 0, 0);
        }
        //FEngineLoop::GraphicDevice.DeviceContext->DrawIndexed(indexCount, startIndex, 0);
    }
    

}

/**  소스 데이터가 채워진 후 호출되는 이 이미터의 다이내믹 렌더링 데이터를 초기화합니다.*/
void FDynamicSpriteEmitterData::Init(bool bInSelected)
{
    bSelected = bInSelected;

    UMaterial const* Material = const_cast<UMaterial const*>(Source.Material);
    //MaterialResource = MaterialInterface->GetRenderProxy();

    // We won't need this on the render thread
    //Source.MaterialInterface = NULL;
}
