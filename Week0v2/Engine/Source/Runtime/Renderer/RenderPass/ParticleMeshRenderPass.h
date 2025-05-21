#pragma once

#include "FBaseRenderPass.h"
#include "Container/Array.h"
#include "Math/Matrix.h"
#include "Particles/ParticleSystem.h"

class UParticleSystemComponent;
struct FObjMaterialInfo;

class FParticleMeshRenderPass : public FBaseRenderPass
{
public:
    explicit FParticleMeshRenderPass(const FName& InShaderName);
    
    ~FParticleMeshRenderPass() override;
    
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void ClearRenderObjects() override;

    static void UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo);


private:
    TArray<UParticleSystemComponent*> ParticleSystemComponents;
    
    class ID3D11Buffer* PerFrameConstantBuffer = nullptr;

    ID3D11InputLayout* InputLayout = nullptr;
};
