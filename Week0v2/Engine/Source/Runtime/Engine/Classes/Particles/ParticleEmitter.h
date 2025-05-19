#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

enum EEmitterRenderMode : int
{
    ERM_Normal ,
    ERM_Point ,
    ERM_Cross ,
    ERM_LightsOnly ,
    ERM_None,
    ERM_MAX,
};

class UParticleSystemComponent;
struct FParticleEmitterInstance;
class UParticleLODLevel;

class UParticleEmitter : public UObject
{
    DECLARE_CLASS(UParticleEmitter, UObject)
public:
    UParticleEmitter() = default;
    ~UParticleEmitter() = default;

    UParticleLODLevel* GetCurrentLODLevel(FParticleEmitterInstance* Instance) const;
    
    TArray<UParticleLODLevel*> LODLevels;

    int32 ParticleSize;
    
    FParticleEmitterInstance* CreateInstance(UParticleSystemComponent* InComponent);
    
    enum EEmitterRenderMode EmitterRenderMode = ERM_MAX;
    
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

enum EParticleSubUVInterpMethod : int
{
	PSUVIM_None,
	PSUVIM_Linear,
	PSUVIM_Random,
	PSUVIM_MAX,
};