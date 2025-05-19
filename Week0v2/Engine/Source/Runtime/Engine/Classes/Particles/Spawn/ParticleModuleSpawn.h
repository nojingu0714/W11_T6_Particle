#pragma once
#include "ParticleModuleSpawnBase.h"
#include "Engine/Source/Runtime/Core/Container/Array.h"
#include "Engine/Source/Runtime/Engine/Classes/Particles/ParticleEmitter.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"


class UParticleModuleSpawn : public UParticleModuleSpawnBase
{
    DECLARE_CLASS(UParticleModuleSpawn, UParticleModule)
public:
    UParticleModuleSpawn()= default;
    ~UParticleModuleSpawn()= default;

    FSimpleFloatDistribution Rate;
    FSimpleFloatDistribution RateScale;
    TArray<FParticleBurst> BurstList;
    FSimpleFloatDistribution BurstScale;
    EParticleBurstMethod ParticleBurstMethod;
    uint32 bApplyGlobalSpawnRateScale : 1;

    void InitializeDefaults();

    // TODO 에디터 관련 작업 하기
    /*virtual void	PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void	PostInitProperties() override;
    virtual void	PostLoad() override;*/

    virtual bool GetSpawnAmount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover,
        float DeltaTime, int32& Number, float& Rate) override;
    virtual float GetMaximumSpawnRate() override;
    virtual float GetEstimatedSpawnRate() override;
    virtual int32 GetMaximumBurstCount() override;
    float GetGlobalRateScale()const;

    void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle& Particle);
};
