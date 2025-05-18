#pragma once
#include "Particles/ParticleModule.h"

struct FParticleEmitterInstance;
class UParticleSystemComponent;
class UParticleEmitter;

class UParticleModuleTypeDataBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleTypeDataBase, UParticleModule)
public:
    UParticleModuleTypeDataBase() = default;
    ~UParticleModuleTypeDataBase() = default;

    FParticleEmitterInstance* CreateInstance(UParticleEmitter* InEmitter, UParticleSystemComponent* InComponent);
};
