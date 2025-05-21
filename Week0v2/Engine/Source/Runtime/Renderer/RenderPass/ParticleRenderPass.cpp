#include "ParticleRenderPass.h"

#include "Define.h"
#include "EditorEngine.h"
#include "LaunchEngineLoop.h"
#include "ParticleEmitterInstance.h"
#include "ParticleHelper.h"
#include "ShowFlags.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Engine/FEditorStateManager.h"
#include "Engine/World.h"
#include "LevelEditor/SLevelEditor.h"
#include "Particles/ParticleModuleRequired.h"
#include "Particles/ParticleSystemComponent.h"
#include "Renderer/Renderer.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"


FParticleRenderPass::FParticleRenderPass(const FName& InShaderName)
    : FBaseRenderPass(InShaderName)
{

    FRenderer& Renderer = GEngineLoop.Renderer;
    FRenderResourceManager* RenderResourceManager = Renderer.GetResourceManager();
    
    PerFrameConstantBuffer = RenderResourceManager->CreateConstantBuffer(sizeof(FPerFrameConstants));
}

FParticleRenderPass::~FParticleRenderPass()
{
    
    ParticleSystemComponents.Empty();
}

void FParticleRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
{
    for (UParticleSystemComponent* ParticleSystemComponent : TObjectRange<UParticleSystemComponent>())
    {
        if (((ParticleSystemComponent->GetWorld()->WorldType != EWorldType::Editor && ParticleSystemComponent->GetWorld()->WorldType != EWorldType::EditorParticlePreview)) || ParticleSystemComponent->GetWorld() != World)
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

    Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::TranslucentNoDepthWrite), 0);
    Graphics.DeviceContext->RSSetState(Renderer.GetRasterizerState(ERasterizerState::SolidNone));
    Graphics.DeviceContext->OMSetBlendState(Renderer.GetBlendState(EBlendState::AlphaBlend), nullptr, 0xffffffff); // 블렌딩 상태 설정, 기본 블렌딩 상태임

    FEngineLoop::GraphicDevice.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    

    
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
    const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;
    }
    const auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

    if (!(ActiveViewport->ShowFlag & EEngineShowFlags::SF_Particles))
    {
        return;
    }
    for (const UParticleSystemComponent* ParticleSystemComponent : ParticleSystemComponents)
    {
            FMatrix View = FMatrix::Identity;
            FMatrix Proj = FMatrix::Identity;
            FMatrix ViewProj = FMatrix::Identity;
        
            FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
            FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();

            std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
            if (curEditorViewportClient != nullptr)
            {
                View = curEditorViewportClient->GetViewMatrix();
                Proj = curEditorViewportClient->GetProjectionMatrix();
                ViewProj = curEditorViewportClient->GetViewProjectionMatrix();
            }

            FPerFrameConstants PerFrameConstants;
        
            PerFrameConstants.ViewMatrix = View;
            PerFrameConstants.ProjectionMatrix = Proj;
        
            USceneComponent* overrideComp = curEditorViewportClient->GetOverrideComponent();
            if (overrideComp)
            {
                PerFrameConstants.CameraWorldPosition = overrideComp->GetWorldLocation();
            }
            else
            {
                PerFrameConstants.CameraWorldPosition = curEditorViewportClient->ViewTransformPerspective.GetLocation();
            }

        
            PerFrameConstants.CameraUpVector = curEditorViewportClient->ViewTransformPerspective.GetUpVector();
            PerFrameConstants.CameraRightVector = curEditorViewportClient->ViewTransformPerspective.GetRightVector();
            renderResourceManager->UpdateConstantBuffer("FPerFrameConstants", &PerFrameConstants);
            for (const FDynamicEmitterDataBase* EmitterDataBase : ParticleSystemComponent->EmitterRenderData)
            {
                const FDynamicSpriteEmitterData* SpriteEmitterData = dynamic_cast<const FDynamicSpriteEmitterData*>(EmitterDataBase);
                if (SpriteEmitterData)
                {
                    SpriteEmitterData->ExecuteRender(ViewProj);
                }
            }
    }

    // for (const UBillboardComponent* item : BillboardComponents)
    // {
    //     FDebugIconConstant DebugConstant;
    //     DebugConstant.IconPosition = item->GetWorldLocation();
    //     DebugConstant.IconScale = 0.2f; //TODO: 게임잼용 임시 스케일 변경
    //
    //     renderResourceManager->UpdateConstantBuffer(TEXT("FDebugIconConstant"), &DebugConstant);
    //
    //     Graphics.DeviceContext->PSSetShaderResources(0, 1, &item->Texture->TextureSRV);
    //     
    //     //TOD 이거 고쳐야함 Warning의 이유
    //     //D3D11 WARNING: ID3D11DeviceContext::Draw: Vertex Buffer at the input vertex slot 0 is not big enough for what the Draw*() call expects to traverse. This is OK, as reading off the end of the Buffer is defined to return 0. However the developer probably did not intend to make use of this behavior.  [ EXECUTION WARNING #356: DEVICE_DRAW_VERTEX_BUFFER_TOO_SMALL]
    //     Graphics.DeviceContext->Draw(6, 0); // 내부에서 버텍스 사용중
    // }
}

void FParticleRenderPass::ClearRenderObjects()
{
    FBaseRenderPass::ClearRenderObjects();
    
    ParticleSystemComponents.Empty();
}

void FParticleRenderPass::UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo)
{
        FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
        FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    
        FMaterialConstants MaterialConstants;
        MaterialConstants.DiffuseColor = MaterialInfo.Diffuse;
        MaterialConstants.TransparencyScalar = MaterialInfo.TransparencyScalar;
        MaterialConstants.MatAmbientColor = MaterialInfo.Ambient;
        MaterialConstants.DensityScalar = MaterialInfo.DensityScalar;
        MaterialConstants.SpecularColor = MaterialInfo.Specular;
        MaterialConstants.SpecularScalar = MaterialInfo.SpecularScalar;
        MaterialConstants.EmissiveColor = MaterialInfo.Emissive;
        //normalScale값 있는데 parse만 하고 constant로 넘기고 있진 않음
        MaterialConstants.bHasNormalTexture = false;
    
        if (MaterialInfo.bHasTexture == true)
        {
            const std::shared_ptr<FTexture> texture = GEngineLoop.ResourceManager.GetTexture(MaterialInfo.DiffuseTexturePath);
            const std::shared_ptr<FTexture> NormalTexture = GEngineLoop.ResourceManager.GetTexture(MaterialInfo.NormalTexturePath);
            if (texture)
            {
                Graphics.DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
            }
            if (NormalTexture)
            {
                Graphics.DeviceContext->PSSetShaderResources(1, 1, &NormalTexture->TextureSRV);
                MaterialConstants.bHasNormalTexture = true;
            }
        
            ID3D11SamplerState* linearSampler = renderResourceManager->GetSamplerState(ESamplerType::Linear);
            Graphics.DeviceContext->PSSetSamplers(static_cast<uint32>(ESamplerType::Linear), 1, &linearSampler);
        }
        else
        {
            ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
            Graphics.DeviceContext->PSSetShaderResources(0, 1, nullSRV);
        }
        renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FMaterialConstants")), &MaterialConstants);
    
}

