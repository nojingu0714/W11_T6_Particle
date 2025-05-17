#pragma once
#include "FXSystemComponent.h"

struct FParticleEmitterInstances;
struct FDynamicEmitterDataBase;
class UParticleSystem;

class UParticleSystemComponent : public UFXSystemComponent
{
    DECLARE_CLASS(UParticleSystemComponent, UFXSystemComponent)
public:
    UParticleSystemComponent();
    ~UParticleSystemComponent() = default;


    void BeginPlay() override;
    void UpdateDynamicData();
    void TickComponent(float DeltaTime) override;
    void InitParticles();
    void ResetSystem();
    
    TArray<FParticleEmitterInstances*> EmitterInstances;
    UParticleSystem* Template;

    TArray<FDynamicEmitterDataBase*> EmitterRenderData;
};

