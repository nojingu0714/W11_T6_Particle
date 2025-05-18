#pragma once
#include "ParticleModuleSpawnBase.h"

struct FParticleEmitterInstance;
struct FBaseParticle;

class UParticleModuleSpawn : public UParticleModuleSpawnBase
{
    DECLARE_CLASS(UParticleModuleSpawn, UParticleModule)
public:
    UParticleModuleSpawn()= default;
    ~UParticleModuleSpawn()= default;

    float Rate = 1.0f;
    float RateScale;

    void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle& Particle);
private:
};
