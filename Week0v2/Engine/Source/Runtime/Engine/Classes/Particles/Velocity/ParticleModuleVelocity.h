#pragma once

#include "Engine/Classes/Particles/ParticleModule.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"

class UParticleModuleVelocity : public UParticleModule 
{
    DECLARE_CLASS(UParticleModuleVelocity, UParticleModule)
    
public:
    UParticleModuleVelocity();
    ~UParticleModuleVelocity();

    FSimpleVectorDistribution StartVelocity;

    // Particle이 Emitter로부터 떨어진 방향으로 얼마나 나아갈련지 == 방사할련지
    FSimpleVectorDistribution StartVelocityRadial;

    void InitializeDefaults();

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
};