#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleLODLevel;

class UParticleEmitter : public UObject
{
    DECLARE_CLASS(UParticleEmitter, UObject)
public:
    UParticleEmitter() = default;
    ~UParticleEmitter() = default;

    TArray<UParticleLODLevel*> LODLevels;

    void CacheEmitterModuleInfo();
};
