#pragma once
#include "FXSystemComponent.h"

struct FDynamicEmitterDataBase;
class UParticleSystem;

class UParticleSystemComponent : public UFXSystemComponent
{
    DECLARE_CLASS(UParticleSystemComponent, UFXSystemComponent)
public:
    UParticleSystemComponent() = default;
    ~UParticleSystemComponent() = default;

    TArray<struct FParticleEmitterInstance*> EmitterInstances;
    UParticleSystem* Template;

    TArray<FDynamicEmitterDataBase*> EmitterRenderData;
};
