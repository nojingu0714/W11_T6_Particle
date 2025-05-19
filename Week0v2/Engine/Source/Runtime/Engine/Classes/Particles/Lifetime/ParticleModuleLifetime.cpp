#include "ParticleModuleLifetime.h"
#include "Engine/Source/Runtime/Engine/ParticleEmitterInstance.h"
#include "Engine/Source/Runtime/Engine/ParticleHelper.h"

UParticleModuleLifetime::UParticleModuleLifetime()
{
    bSpawnModule = true;
}

UParticleModuleLifetime::~UParticleModuleLifetime()
{
}

void UParticleModuleLifetime::InitializeDefaults()
{
    Lifetime = FSimpleFloatDistribution(1.0f);
}

void UParticleModuleLifetime::CompileModule(FParticleEmitterBuildInfo& EmitterInfo)
{
    EmitterInfo.MaxLifetime = Lifetime.Max;
    EmitterInfo.SpawnModules.Add(this);
}

void UParticleModuleLifetime::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    SPAWN_INIT;

    // Lifetime 값이 실제로 있지는 않고 그의 역수 값인 OneOverMaxLifetime이 존재
    float MaxLifetime = Lifetime.GetValue(SpawnTime);
    if (Particle.OneOverMaxLifetime > 0.f)
    {
        // Another module already modified lifetime.
        Particle.OneOverMaxLifetime = 1.f / (MaxLifetime + 1.f / Particle.OneOverMaxLifetime);
    }
    else
    {
        // First module to modify lifetime.
        Particle.OneOverMaxLifetime = MaxLifetime > 0.f ? 1.f / MaxLifetime : 0.f;
    }
    //If the relative time is already > 1.0f then we don't want to be setting it. Some modules use this to mark a particle as dead during spawn.
    Particle.RelativeTime = Particle.RelativeTime > 1.0f ? Particle.RelativeTime : SpawnTime * Particle.OneOverMaxLifetime;
}

void UParticleModuleLifetime::SetToSensibleDefaults(UParticleEmitter* Owner)
{
    Lifetime.Min = 1.0f;
    Lifetime.Max = 1.0f;
}

float UParticleModuleLifetime::GetMaxLifetime()
{
    return Lifetime.Max;
}

float UParticleModuleLifetime::GetLifetimeValue(FParticleEmitterInstance* Owner, float InTime, UObject* Data)
{
    return Lifetime.GetValue(InTime);
}
