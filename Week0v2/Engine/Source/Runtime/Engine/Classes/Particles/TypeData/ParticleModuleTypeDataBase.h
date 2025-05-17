#pragma once
#include "Particles/ParticleModule.h"

struct FParticleEmitterInstances;
class UParticleSystemComponent;
class UParticleEmitter;

class UParticleModuleTypeDataBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleTypeDataBase, UParticleModule)
public:
    UParticleModuleTypeDataBase() = default;
    ~UParticleModuleTypeDataBase() = default;

    FParticleEmitterInstances* CreateInstance(UParticleEmitter* InEmitter, UParticleSystemComponent* InComponent);
};
