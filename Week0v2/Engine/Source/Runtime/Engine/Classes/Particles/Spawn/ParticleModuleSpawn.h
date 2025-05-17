#pragma once
#include "ParticleModuleSpawnBase.h"

class UParticleModuleSpawn : public UParticleModuleSpawnBase
{
    DECLARE_CLASS(UParticleModuleSpawn, UParticleModule)
public:
    UParticleModuleSpawn()= default;
    ~UParticleModuleSpawn()= default;

    float Rate = 1.0f;
    float RateScale;
    
private:
};
