#include "MyParticleSystem.h"
#include "ParticleEmitter.h"
#include "ParticleLODLevel.h"
#include "ParticleModuleRequired.h"
#include "Size/ParticleModuleSizeScaleBySpeed.h"
#include "Spawn/ParticleModuleSpawn.h"

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
    LODLevel->Modules.Add(Spawn);
    UParticleModuleSizeScaleBySpeed* SizeScale = FObjectFactory::ConstructObject<UParticleModuleSizeScaleBySpeed>(this);
    LODLevel->Modules.Add(SizeScale);
}

UMyParticleSystem::~UMyParticleSystem()
{
    
}


