#pragma once
#include "Particles/ParticleModule.h"

class UMaterial;

class UParticleModuleSnow : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSnow, UParticleModule)
public:
    UParticleModuleSnow();
    ~UParticleModuleSnow() = default;

    UMaterial* MyMatarial;

    void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
};
