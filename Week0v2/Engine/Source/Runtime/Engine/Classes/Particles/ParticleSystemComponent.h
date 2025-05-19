#pragma once
#include "FXSystemComponent.h"

struct FParticleEmitterInstance;
struct FDynamicEmitterDataBase;
class UParticleSystem;

class UParticleSystemComponent : public UFXSystemComponent
{
    DECLARE_CLASS(UParticleSystemComponent, UFXSystemComponent)
public:
    UParticleSystemComponent();
    ~UParticleSystemComponent() = default;


    void BeginPlay() override;
    void TickComponent(float DeltaTime) override;
    void InitParticles();
    void ResetSystem();
    
    TArray<FParticleEmitterInstance*> EmitterInstances;
    UParticleSystem* Template;

    void UpdateDynamicData();
    TArray<FDynamicEmitterDataBase*> EmitterRenderData;
};

