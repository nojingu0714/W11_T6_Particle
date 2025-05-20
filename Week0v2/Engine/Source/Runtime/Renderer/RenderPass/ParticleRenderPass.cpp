#include "ParticleRenderPass.h"

#include "Define.h"
#include "LaunchEngineLoop.h"
#include "ParticleHelper.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Engine/World.h"
#include "Particles/ParticleModuleRequired.h"
#include "Particles/ParticleSystemComponent.h"
#include "Renderer/Renderer.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"


FParticleRenderPass::FParticleRenderPass(const FName& InShaderName) : FBaseRenderPass(InShaderName)
{
}

FParticleRenderPass::~FParticleRenderPass()
{
    
    ParticleSystemComponents.Empty();
}

void FParticleRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
{
    for (UParticleSystemComponent* ParticleSystemComponent : TObjectRange<UParticleSystemComponent>())
    {
        if (((ParticleSystemComponent->GetWorld()->WorldType != EWorldType::Editor && ParticleSystemComponent->GetWorld()->WorldType != EWorldType::EditorPreview)) || ParticleSystemComponent->GetWorld() != World)
        {
            continue;
        }
        
        ParticleSystemComponents.Add(ParticleSystemComponent);
    }
}

void FParticleRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);
    
    const FRenderer& Renderer = GEngineLoop.Renderer;
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);
    Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 정정 연결 방식 설정
    Graphics.DeviceContext->RSSetState(Renderer.GetRasterizerState(ERasterizerState::SolidBack));
    Graphics.DeviceContext->OMSetBlendState(Renderer.GetBlendState(EBlendState::AlphaBlend), nullptr, 0xffffffff); // 블렌딩 상태 설정, 기본 블렌딩 상태임

    // RTVs 배열의 유효성을 확인합니다.
    const auto CurRTV = Graphics.GetCurrentRenderTargetView();
    if (CurRTV != nullptr)
    {
        Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, Graphics.GetCurrentWindowData()->DepthStencilView); // 렌더 타겟 설정
    }
    else
    {
        // RTVs[0]이 nullptr인 경우에 대한 처리
        // 예를 들어, 기본 렌더 타겟을 설정하거나 오류를 기록할 수 있습니다.
        // Graphics.DeviceContext->OMSetRenderTargets(1, &Graphics.FrameBufferRTV, Graphics.DepthStencilView);
    }

    ID3D11SamplerState* linearSampler = Renderer.GetSamplerState(ESamplerType::Linear);
    Graphics.DeviceContext->PSSetSamplers(0, 1, &linearSampler);
}

void FParticleRenderPass::Execute(std::shared_ptr<FViewportClient> InViewportClient)
{
    // 1) 뷰·프로젝션·카메라 상수 업데이트
    FMatrix View = FMatrix::Identity;
    FMatrix Proj = FMatrix::Identity;
    auto EditorVC = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
    if (EditorVC)
    {
        View = EditorVC->GetViewMatrix();
        Proj = EditorVC->GetProjectionMatrix();
    }

    FSceneConstant SceneCB;
    SceneCB.ViewMatrix = View;
    SceneCB.ProjMatrix = Proj;
    if (EditorVC->GetOverrideComponent())
    {
        SceneCB.CameraPos    = EditorVC->GetOverrideComponent()->GetWorldLocation();
        SceneCB.CameraLookAt = EditorVC->ViewTransformPerspective.GetLookAt();
    }
    else
    {
        SceneCB.CameraPos    = EditorVC->ViewTransformPerspective.GetLocation();
        SceneCB.CameraLookAt = EditorVC->ViewTransformPerspective.GetLookAt();
    }
    GEngineLoop.Renderer.GetResourceManager()
        ->UpdateConstantBuffer(TEXT("FSceneConstant"), &SceneCB);

    auto* DC = GEngineLoop.GraphicDevice.DeviceContext;

    for (UParticleSystemComponent* PSC : ParticleSystemComponents)
    {
        for (FDynamicEmitterDataBase* Base : PSC->EmitterRenderData)
        {
            auto* SpriteData = static_cast<FDynamicSpriteEmitterData*>(Base);
            if (SpriteData->Source.eEmitterType != DET_Sprite) 
                continue;

            int32 Count = SpriteData->Source.ActiveParticleCount;
            if (Count <= 0) 
                continue;

            // 1) 버퍼 사이즈 계산
            const int32 NumVerts     = Count * 4;
            const int32 NumIndices   = Count * 6;         // 1 파티클당 2 tri → 6 인덱스
            const UINT  VertStride   = sizeof(FParticleSpriteVertex);
            const int32 VBSize       = VertStride * NumVerts;
            const int32 IBSize       = sizeof(uint16) * NumIndices;

            // 2) 버퍼 생성(또는 재할당)
            if (!SpriteData->VertexBuffer || SpriteData->VertexBufferSize < VBSize)
            {
                if (SpriteData->VertexBuffer) SpriteData->VertexBuffer->Release();
                D3D11_BUFFER_DESC desc = {};
                desc.Usage          = D3D11_USAGE_DYNAMIC;
                desc.ByteWidth      = VBSize;
                desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                GEngineLoop.GraphicDevice.Device
                    ->CreateBuffer(&desc, nullptr, &SpriteData->VertexBuffer);
                SpriteData->VertexBufferSize = VBSize;
            }
            if (!SpriteData->IndexBuffer || SpriteData->IndexBufferSize < IBSize)
            {
                if (SpriteData->IndexBuffer) SpriteData->IndexBuffer->Release();
                D3D11_BUFFER_DESC desc = {};
                desc.Usage          = D3D11_USAGE_DYNAMIC;
                desc.ByteWidth      = IBSize;
                desc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                GEngineLoop.GraphicDevice.Device
                    ->CreateBuffer(&desc, nullptr, &SpriteData->IndexBuffer);
                SpriteData->IndexBufferSize = IBSize;
            }

            // 3) 정점 데이터 채우기
            D3D11_MAPPED_SUBRESOURCE vbMapped;
            DC->Map(SpriteData->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vbMapped);
            SpriteData->GetVertexAndIndexData(
                /*VertexData=*/ vbMapped.pData,
                /*IndexData=*/ nullptr,  // 인덱스는 다음 단계에서 채우기
                /*ParticleOrder=*/ nullptr,
                /*CameraPos=*/ SceneCB.CameraPos,
                /*LocalToWorld=*/ FMatrix::CreateScaleMatrix(
                                      PSC->GetWorldScale().X,
                                      PSC->GetWorldScale().Y,
                                      PSC->GetWorldScale().Z),
                /*InstanceFactor=*/ 1
            );
            DC->Unmap(SpriteData->VertexBuffer, 0);

            // 4) 인덱스 데이터 채우기
            D3D11_MAPPED_SUBRESOURCE ibMapped;
            DC->Map(SpriteData->IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ibMapped);
            {
                uint16* indices = reinterpret_cast<uint16*>(ibMapped.pData);
                for (int32 i = 0; i < Count; ++i)
                {
                    int32 vBase = i * 4;
                    int32 iBase = i * 6;
                    // quad: (0,1,2) + (0,2,3)
                    indices[iBase + 0] = vBase + 0;
                    indices[iBase + 1] = vBase + 1;
                    indices[iBase + 2] = vBase + 2;
                    indices[iBase + 3] = vBase + 0;
                    indices[iBase + 4] = vBase + 2;
                    indices[iBase + 5] = vBase + 3;
                }
            }
            DC->Unmap(SpriteData->IndexBuffer, 0);

            // 5) IA 셋업
            UINT offset = 0;
            // DC->IASetInputLayout(  YourParticleInputLayout );
            DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            DC->IASetVertexBuffers(0, 1, &SpriteData->VertexBuffer, &VertStride, &offset);
            DC->IASetIndexBuffer(  SpriteData->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

            // 6) 텍스처 바인딩
            // 6) 셰이더 리소스 바인딩
            if (SpriteData->Source.Material)
            {
                auto texture = GEngineLoop.ResourceManager
                                   .GetTexture(SpriteData->Source
                                                   .Material
                                                   ->GetMaterialInfo()
                                                   .DiffuseTexturePath);
                if (texture)
                    DC->PSSetShaderResources(0, 1, &texture->TextureSRV);
            }

            // 7) 드로우 호출 (인덱스 개수 = Count*6)
            DC->DrawIndexed(NumIndices, 0, 0);
        }
    }
}

void FParticleRenderPass::ClearRenderObjects()
{
    FBaseRenderPass::ClearRenderObjects();
    
    ParticleSystemComponents.Empty();
}
