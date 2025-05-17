#pragma once
#include "FBaseRenderPass.h"
#include "Container/Array.h"

class UParticleSystemComponent;

class FParticleRenderPass : public FBaseRenderPass
{
public:
    explicit FParticleRenderPass(const FName& InShaderName)
        : FBaseRenderPass(InShaderName) {}
    
    ~FParticleRenderPass() override;
    
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void ClearRenderObjects() override;

private:
    TArray<UParticleSystemComponent*> ParticleSystemComponents;
};

