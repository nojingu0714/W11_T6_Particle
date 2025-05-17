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

struct FParticleBurst 
{
    int32 Count;
    int32 CountLow;
    float Time;

    FParticleBurst()
        : Count(0)
        , CountLow(-1)
        , Time(0.0f) { }
};

enum EParticleBurstMethod : int
{
    EPBM_Instant,
    EPBM_Interpolated,
    EPBM_MAX,
};