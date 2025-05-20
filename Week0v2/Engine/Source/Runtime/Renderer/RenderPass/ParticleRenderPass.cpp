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
    Graphics.DeviceContext->PSSetSamplers(static_cast<uint32>(ESamplerType::Linear), 1, &linearSampler);
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

    // 2) 모든 파티클 시스템 컴포넌트 순회
    for (UParticleSystemComponent* PSC : ParticleSystemComponents)
    {
        for (FDynamicEmitterDataBase* Base : PSC->EmitterRenderData)
        {
            // 스프라이트만 처리

            auto* SpriteData = static_cast<FDynamicSpriteEmitterData*>(Base);
            if (SpriteData->Source.eEmitterType != DET_Sprite) 
                continue;
            const auto& Src  = SpriteData->Source;

            int32 Count    = Src.ActiveParticleCount;
            int32 NumVerts = Count * 4;
            int32 NumPrims = Count * 2;
            int32 VertStride = sizeof(FParticleSpriteVertex);
            int32 VBSize     = VertStride * NumVerts;
            int32 IBSize     = sizeof(uint16) * NumPrims * 3;

            // 3) GPU 버퍼가 없다면 생성
            if (!SpriteData->VertexBuffer || SpriteData->VertexBufferSize < VBSize)
            {
                if (SpriteData->VertexBuffer)
                    { SpriteData->VertexBuffer->Release(); }
                D3D11_BUFFER_DESC vbDesc = {};
                vbDesc.Usage          = D3D11_USAGE_DYNAMIC;
                vbDesc.ByteWidth      = VBSize;
                vbDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
                vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                GEngineLoop.GraphicDevice.Device->CreateBuffer(&vbDesc, nullptr, &SpriteData->VertexBuffer);
                SpriteData->VertexBufferSize = VBSize;
            }
            if (!SpriteData->IndexBuffer || SpriteData->IndexBufferSize < IBSize)
            {
                if (SpriteData->IndexBuffer)
                    { SpriteData->IndexBuffer->Release(); }
                D3D11_BUFFER_DESC ibDesc = {};
                ibDesc.Usage          = D3D11_USAGE_DYNAMIC;
                ibDesc.ByteWidth      = IBSize;
                ibDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
                ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                GEngineLoop.GraphicDevice.Device->CreateBuffer(&ibDesc, nullptr, &SpriteData->IndexBuffer);
                SpriteData->IndexBufferSize = IBSize;
            }

            // 4) Map → 데이터 채우기 → Unmap
            D3D11_MAPPED_SUBRESOURCE Mapped;
            auto* DC = GEngineLoop.GraphicDevice.DeviceContext;

            // 정점
            void* VertexPtr = nullptr;
            DC->Map(SpriteData->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
            VertexPtr = Mapped.pData;
            DC->Unmap(SpriteData->VertexBuffer, 0);

            // 2. Index Buffer
            void* IndexPtr = nullptr;
            DC->Map(SpriteData->IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
            IndexPtr = Mapped.pData;
            DC->Unmap(SpriteData->IndexBuffer, 0);

            // 3. 한 번에 호출 (메모리는 따로 맵핑해도, 호출은 한번만 가능)
            SpriteData->GetVertexAndIndexData(
                VertexPtr,
                IndexPtr,
                /*ParticleOrder*/ nullptr,
                SceneCB.CameraPos,
                FMatrix::CreateScaleMatrix(
                    PSC->GetWorldScale().X,
                    PSC->GetWorldScale().Y,
                    PSC->GetWorldScale().Z
                ),
                /*InstanceFactor*/ 1
            );

            // 5) IA 셋업
            UINT stride = VertStride, offset = 0;
            DC->IASetVertexBuffers(0, 1, &SpriteData->VertexBuffer, &stride, &offset);
            DC->IASetIndexBuffer(SpriteData->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

            if (SpriteData->Source.Material == nullptr)
                return;
            // 6) 셰이더 & 리소스 바인딩
            std::shared_ptr<FTexture> texture = GEngineLoop.ResourceManager.GetTexture(SpriteData->Source.Material->GetMaterialInfo().DiffuseTexturePath);
            if (texture)
            {
                GEngineLoop.GraphicDevice.DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
            }
            // 인스턴스 상수
            // GEngineLoop.Renderer.GetResourceManager()
            //     ->UpdateConstantBuffer(TEXT("FParticleInstanceConstant"),
            //                            &Src.RequiredModule->ParticleConstantBuffer);
            // ID3D11Buffer* PCB = GEngineLoop.Renderer.GetResourceManager()
            //                        ->GetConstantBuffer(TEXT("FParticleInstanceConstant"));
            // DC->VSSetConstantBuffers(1, 1, &PCB);
            // DC->PSSetConstantBuffers(1, 1, &PCB);
            
            // 7) DrawIndexed
            DC->DrawIndexed(NumPrims * 3, 0, 0);
        }
    }
}

void FParticleRenderPass::ClearRenderObjects()
{
    FBaseRenderPass::ClearRenderObjects();
    
    ParticleSystemComponents.Empty();
}
