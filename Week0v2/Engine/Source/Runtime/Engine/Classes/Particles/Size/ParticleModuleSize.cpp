#include "ParticleModuleSize.h"
#include "Engine/Source/Runtime/Engine/ParticleEmitterInstances.h"
#include "Engine/Source/Runtime/Engine/ParticleHelper.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"

UParticleModuleSize::UParticleModuleSize()
{
    bSpawnModule = true;
    bUpdateModule = false;
}

UParticleModuleSize::~UParticleModuleSize()
{

}

void UParticleModuleSize::InitializeDefaults()
{
    StartSize = FSimpleVectorDistribution(FVector(1.0f, 1.0f, 1.0f),
                                          FVector(1.0f, 1.0f, 1.0f), EDistributionType::Constant);
}

void UParticleModuleSize::CompileModule(FParticleEmitterBuildInfo& EmitterInfo)
{
    float MinSize = 0.0f;
    float MaxSize = 0.0f;
    StartSize.GetOutRange(MinSize, MaxSize);
    EmitterInfo.MaxSize.X *= MaxSize;
    EmitterInfo.MaxSize.Y *= MaxSize;
    EmitterInfo.SpawnModules.Add(this);
}

void UParticleModuleSize::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    SPAWN_INIT;
    FVector Size = StartSize.GetValue(Owner->EmitterTime);
    Particle.Size += Size;

    //AdjustParticleBaseSizeForUVFlipping(Size, Owner->CurrentLODLevel->RequiredModule->UVFlippingMode, *InRandomStream);
    Particle.BaseSize += Size;
}
