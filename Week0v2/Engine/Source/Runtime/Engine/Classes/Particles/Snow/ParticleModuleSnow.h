#pragma once
#include "Particles/ParticleModule.h"
#include "Define.h"

class UMaterial;

class UParticleModuleSnow : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSnow, UParticleModule)

private:

    float SpawnXMin;
    float SpawnXMax;

    // Y축 스폰 범위
    float SpawnYMin;
    float SpawnYMax;

    // Z축 스폰 높이 범위
    float SpawnZMin;
    float SpawnZMax;


public:
    UParticleModuleSnow();
    ~UParticleModuleSnow() = default;

    void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
};
