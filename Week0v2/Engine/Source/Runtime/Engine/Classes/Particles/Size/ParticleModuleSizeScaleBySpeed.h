#pragma once
#include "ParticleModuleSizeBase.h"
#include "Particles/ParticleLODLevel.h"
#include "UObject/Object.h"

class UParticleModuleSizeScaleBySpeed : public UParticleModuleSizeBase
{
    DECLARE_CLASS(UParticleModuleSizeScaleBySpeed, UParticleModuleSizeBase)
public:
    UParticleModuleSizeScaleBySpeed() = default;
    ~UParticleModuleSizeScaleBySpeed() = default;
};
