#include "MyParticleSystem.h"
#include "ParticleEmitter.h"
#include "ParticleLODLevel.h"
#include "ParticleModuleRequired.h"
#include "Size/ParticleModuleSizeScaleBySpeed.h"
#include "Spawn/ParticleModuleSpawn.h"
#include "Lifetime/ParticleModuleLifetime.h"

UMyParticleSystem::UMyParticleSystem()
{
    UParticleEmitter* emitter = FObjectFactory::ConstructObject<UParticleEmitter>(this);
    Emitters.Add(emitter);
    UParticleLODLevel* LODLevel = FObjectFactory::ConstructObject<UParticleLODLevel>(this);
    Emitters[0]->LODLevels.Add(LODLevel);
    UParticleModuleRequired* Required = FObjectFactory::ConstructObject<UParticleModuleRequired>(this);
    LODLevel->Modules.Add(Required);
    LODLevel->RequiredModule = Required;
    UParticleModuleSpawn* Spawn = FObjectFactory::ConstructObject<UParticleModuleSpawn>(this);
    Spawn->InitializeDefaults();
    LODLevel->Modules.Add(Spawn);
    UParticleModuleLifetime* Lifetime = FObjectFactory::ConstructObject<UParticleModuleLifetime>(this);
    Lifetime->InitializeDefaults();
    LODLevel->Modules.Add(Lifetime);
    UParticleModuleSizeScaleBySpeed* SizeScale = FObjectFactory::ConstructObject<UParticleModuleSizeScaleBySpeed>(this);
    LODLevel->Modules.Add(SizeScale);
}

UMyParticleSystem::~UMyParticleSystem()
{
    
}


