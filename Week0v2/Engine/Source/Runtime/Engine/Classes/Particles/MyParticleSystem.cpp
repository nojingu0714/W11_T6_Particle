#include "MyParticleSystem.h"
#include "ParticleEmitter.h"
#include "ParticleLODLevel.h"
#include "ParticleModuleRequired.h"
#include "Size/ParticleModuleSizeScaleBySpeed.h"
#include "Spawn/ParticleModuleSpawn.h"
#include "Lifetime/ParticleModuleLifetime.h"
#include "Location/ParticleModuleLocation.h"
#include "Velocity/ParticleModuleVelocity.h"
#include "Color/ParticleModuleColor.h"
#include "Size/ParticleModuleSize.h"

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
    UParticleModuleSize* Size = FObjectFactory::ConstructObject<UParticleModuleSize>(this);
    Size->InitializeDefaults();
    LODLevel->Modules.Add(Size);
    UParticleModuleLocation* Location = FObjectFactory::ConstructObject<UParticleModuleLocation>(this);
    Location->InitializeDefaults();
    LODLevel->Modules.Add(Location);
    UParticleModuleVelocity* Velocity = FObjectFactory::ConstructObject<UParticleModuleVelocity>(this);
    Velocity->InitializeDefaults();
    LODLevel->Modules.Add(Velocity);
    UParticleModuleColor* Color = FObjectFactory::ConstructObject<UParticleModuleColor>(this);
    Color->InitializeDefaults();
    LODLevel->Modules.Add(Color);
}

UMyParticleSystem::~UMyParticleSystem()
{
    
}


