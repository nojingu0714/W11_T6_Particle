#pragma once

#include "Engine/Classes/Particles/ParticleModule.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"

class UParticleModuleLifetime : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleLifetime, UParticleModule)

    UParticleModuleLifetime();
    ~UParticleModuleLifetime();

    FSimpleFloatDistribution Lifetime;

    virtual void CompileModule(struct FParticleEmitterBuildInfo& EmitterInfo) override;
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    virtual void SetToSensibleDefaults(UParticleEmitter* Owner) override;

    virtual float GetMaxLifetime();
    virtual float GetLifetimeValue(FParticleEmitterInstance* Owner, float InTime, UObject* Data = NULL);
};