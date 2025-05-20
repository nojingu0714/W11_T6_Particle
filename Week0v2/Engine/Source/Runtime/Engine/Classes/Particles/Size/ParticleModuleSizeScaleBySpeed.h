#pragma once
#include "ParticleModuleSizeBase.h"
#include "Particles/ParticleLODLevel.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Core/Math/Vector.h"

class UParticleModuleSizeScaleBySpeed : public UParticleModuleSizeBase
{
    DECLARE_CLASS(UParticleModuleSizeScaleBySpeed, UParticleModuleSizeBase)
public:
    UParticleModuleSizeScaleBySpeed();
    ~UParticleModuleSizeScaleBySpeed() = default;

    FVector2D SpeedScale;
    FVector2D MaxScale;


    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    virtual void CompileModule(struct FParticleEmitterBuildInfo& EmitterInfo) override;
};
