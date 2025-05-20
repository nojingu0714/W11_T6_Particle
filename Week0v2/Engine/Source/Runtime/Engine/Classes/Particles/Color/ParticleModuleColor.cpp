#include "ParticleModuleColor.h"
#include "Engine/Source/Runtime/Engine/ParticleEmitterInstance.h"
#include "Engine/Source/Runtime/Engine/ParticleHelper.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"

UParticleModuleColor::UParticleModuleColor()
{
    bSpawnModule = true;
    bUpdateModule = true;
    bCurvesAsColor = true;
}

UParticleModuleColor::~UParticleModuleColor()
{

}

void UParticleModuleColor::InitializeDefaults()
{
    ColorScaleOverLife = FSimpleVectorDistribution(FVector(1.0f, 1.0f, 1.0f),
        FVector(1.0f, 1.0f, 1.0f), EDistributionType::EaseInOut);
    AlphaScaleOverLife = FSimpleFloatDistribution(1.0f, 1.0f, EDistributionType::EaseInOut);
}

void UParticleModuleColor::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    SPAWN_INIT;
    FVector ColorVec;
    float	fAlpha;

    ColorVec = ColorScaleOverLife.GetValue(Particle.RelativeTime);
    fAlpha = AlphaScaleOverLife.GetValue(Particle.RelativeTime);
 
    Particle.Color.R = ColorVec.X;
    Particle.Color.G = ColorVec.Y;
    Particle.Color.B = ColorVec.Z;
    Particle.Color.A = fAlpha;
}

void UParticleModuleColor::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    FVector ColorVec;
    float	fAlpha;
    
    BEGIN_UPDATE_LOOP;
    ColorVec = ColorScaleOverLife.GetValue(Particle.RelativeTime);
    fAlpha = AlphaScaleOverLife.GetValue(Particle.RelativeTime);
    Particle.Color.R = ColorVec.X;
    Particle.Color.G = ColorVec.Y;
    Particle.Color.B = ColorVec.Z;
    Particle.Color.A = fAlpha;
    END_UPDATE_LOOP;
}
