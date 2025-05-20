#include "ParticleModuleSizeScaleBySpeed.h"
#include "Engine/Source/Runtime/Engine/ParticleHelper.h"
#include "Engine/Source/Runtime/Engine/ParticleEmitterInstance.h"

void UParticleModuleSizeScaleBySpeed::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    FVector Scale(SpeedScale.X, SpeedScale.Y, 1.0f);
    FVector ScaleMax(MaxScale.X, MaxScale.Y, 1.0f);

    // BEGIN_UPDATE_LOOP;
    // FVector Size = Scale * Particle.Velocity.Magnitude();
    // Size = Size.Max(FVector(1.0f));
    // Size = Size.Min(ScaleMax);
    // Particle.Size = Particle.BaseSize * Size;
    // END_UPDATE_LOOP;
}

void UParticleModuleSizeScaleBySpeed::CompileModule(FParticleEmitterBuildInfo& EmitterInfo)
{
    EmitterInfo.SizeScaleBySpeed = SpeedScale;
    EmitterInfo.MaxSizeScaleBySpeed = MaxScale;
}
