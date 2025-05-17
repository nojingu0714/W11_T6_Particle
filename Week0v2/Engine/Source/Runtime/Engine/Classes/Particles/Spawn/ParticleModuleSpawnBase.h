#pragma once
#include "Particles/ParticleModule.h"

class UParticleModuleSpawnBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSpawnBase, UParticleModule)
public:
    UParticleModuleSpawnBase() = default;
    ~UParticleModuleSpawnBase() = default;

private:
    /**
     *	If true, the SpawnRate of the SpawnModule of the emitter will be processed.
     *	If mutliple Spawn modules are 'stacked' in an emitter, if ANY of them
     *	have this set to false, it will not process the SpawnModule SpawnRate.
     */
    uint32 bProcessSpawnRate : 1;

    /**
     *	If true, the BurstList of the SpawnModule of the emitter will be processed.
     *	If mutliple Spawn modules are 'stacked' in an emitter, if ANY of them
     *	have this set to false, it will not process the SpawnModule BurstList.
     */
    uint32 bProcessBurstList : 1;

    virtual EModuleType	GetModuleType() const override { return EPMT_Spawn; }

    virtual bool GetSpawnAmount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover,
        float DeltaTime, int32& Number, float& Rate)
    {
        return bProcessSpawnRate;
    }

    virtual bool GetBurstCount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover,
        float DeltaTime, int32& Number)
    {
        Number = 0;
        return bProcessBurstList;
    }

    virtual float GetMaximumSpawnRate() { return 0.0f; }
    virtual float GetEstimatedSpawnRate() { return 0.0f; }
    virtual int32 GetMaximumBurstCount() { return 0; }
};
