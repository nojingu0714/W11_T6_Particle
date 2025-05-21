#include "MyParticleSystem.h"
#include "ParticleEmitter.h"
#include "ParticleLODLevel.h"
#include "ParticleModuleRequired.h"
#include "Collision/ParticleModuleCollision.h"
#include "Size/ParticleModuleSizeScaleBySpeed.h"
#include "Spawn/ParticleModuleSpawn.h"
#include "Lifetime/ParticleModuleLifetime.h"
#include "Location/ParticleModuleLocation.h"
#include "Velocity/ParticleModuleVelocity.h"
#include "Color/ParticleModuleColor.h"
#include "Size/ParticleModuleSize.h"
#include "Snow/ParticleModuleSnow.h"

UMyParticleSystem::UMyParticleSystem()
{
    UParticleEmitter* emitter = FObjectFactory::ConstructObject<UParticleEmitter>(this);
    Emitters.Add(emitter);
    UParticleLODLevel* LODLevel = FObjectFactory::ConstructObject<UParticleLODLevel>(this);
    Emitters[0]->LODLevels.Add(LODLevel);
    UParticleModuleRequired* Required = FObjectFactory::ConstructObject<UParticleModuleRequired>(this);
    LODLevel->AddModule(Required);
    LODLevel->RequiredModule = Required;
    UParticleModuleSpawn* Spawn = FObjectFactory::ConstructObject<UParticleModuleSpawn>(this);
    Spawn->InitializeDefaults();
    LODLevel->AddModule(Spawn);
    UParticleModuleLifetime* Lifetime = FObjectFactory::ConstructObject<UParticleModuleLifetime>(this);
    Lifetime->InitializeDefaults();
    LODLevel->AddModule(Lifetime);
   /* UParticleModuleSizeScaleBySpeed* SizeScaleBySpeed = FObjectFactory::ConstructObject<UParticleModuleSizeScaleBySpeed>(this);
    LODLevel->AddModule(SizeScaleBySpeed);
    UParticleModuleSize* Size = FObjectFactory::ConstructObject<UParticleModuleSize>(this);
    Size->InitializeDefaults();
    LODLevel->AddModule(Size);
    UParticleModuleLocation* Location = FObjectFactory::ConstructObject<UParticleModuleLocation>(this);
    Location->InitializeDefaults();
    LODLevel->AddModule(Location);
    UParticleModuleVelocity* Velocity = FObjectFactory::ConstructObject<UParticleModuleVelocity>(this);
    Velocity->InitializeDefaults();
    LODLevel->AddModule(Velocity);
    UParticleModuleColor* Color = FObjectFactory::ConstructObject<UParticleModuleColor>(this);
    Color->InitializeDefaults();
    LODLevel->AddModule(Color);

    UParticleModuleCollision* Collision = FObjectFactory::ConstructObject<UParticleModuleCollision>(this);
    Collision->InitializeDefaults();
    LODLevel->AddModule(Collision);
    
    LODLevel->AddModule(Color);*/

    UParticleModuleSnow* Snow = FObjectFactory::ConstructObject<UParticleModuleSnow>(this);
    LODLevel->AddModule(Snow);
}

UMyParticleSystem::~UMyParticleSystem()
{
    
}


