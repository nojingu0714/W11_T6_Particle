#pragma once

#include "Engine/Classes/Particles/ParticleModule.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"

class UParticleModuleLocation : public UParticleModule 
{
    DECLARE_CLASS(UParticleModuleLocation, UParticleModule)

public:
    UParticleModuleLocation();
    ~UParticleModuleLocation();

    FSimpleVectorDistribution StartLocation;

    void InitializeDefaults();
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
};