#pragma once

#include "ParticleModuleSizeBase.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"

class UParticleModuleSize : public UParticleModuleSizeBase 
{
    DECLARE_CLASS(UParticleModuleSize, UParticleModuleSizeBase)

public:
    UParticleModuleSize();
    ~UParticleModuleSize();

    FSimpleVectorDistribution StartSize;

    void InitializeDefaults();
    virtual void CompileModule(struct FParticleEmitterBuildInfo& EmitterInfo) override;
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
};