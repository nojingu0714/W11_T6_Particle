#pragma once
#include "Particles/ParticleModule.h"

class UParticleModuleSnow : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSnow, UParticleModule)
public:
    UParticleModuleSnow();
    ~UParticleModuleSnow() = default;


    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
};
