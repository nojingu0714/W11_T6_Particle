#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleSystemComponent;
struct FParticleEmitterInstance;
class UParticleLODLevel;

class UParticleEmitter : public UObject
{
    DECLARE_CLASS(UParticleEmitter, UObject)
public:
    UParticleEmitter() = default;
    ~UParticleEmitter() = default;

    TArray<UParticleLODLevel*> LODLevels;

    int32 ParticleSize;
    
    FParticleEmitterInstance* CreateInstance(UParticleSystemComponent* InComponent);
    
    void CacheEmitterModuleInfo();
};
