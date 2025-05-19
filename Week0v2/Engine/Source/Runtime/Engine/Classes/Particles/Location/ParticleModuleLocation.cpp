#include "ParticleModuleLocation.h"
#include "Engine/Source/Runtime/Engine/ParticleEmitterInstances.h"
#include "Engine/Source/Runtime/Engine/ParticleHelper.h"

UParticleModuleLocation::UParticleModuleLocation()
{
    bSpawnModule = true;
    bSupported3DDrawMode = true;    // 일단 3DDrawMode는 지원하지 않고 있는 것으로 보임, Render3DPreview를 구현하지 않았기 때문에
}

UParticleModuleLocation::~UParticleModuleLocation()
{

}

void UParticleModuleLocation::InitializeDefaults()
{
    StartLocation = FSimpleVectorDistribution(FSimpleFloatDistribution(-10.0f, 10.0f),
                                              FSimpleFloatDistribution(-10.0f, 10.0f), 
                                              FSimpleFloatDistribution(-10.0f, 10.0f));
}

void UParticleModuleLocation::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    SPAWN_INIT;

    Particle.Location += StartLocation.GetValue(SpawnTime);  // 기본은 Uniform이므로 SpawnTime에 영향 받지 않음
}
