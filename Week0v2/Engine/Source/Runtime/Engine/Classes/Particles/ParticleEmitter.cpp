#include "ParticleEmitter.h"

#include "ParticleEmitterInstance.h"
#include "ParticleHelper.h"
#include "ParticleLODLevel.h"
#include "ParticleMeshEmitterInstance.h"
#include "ParticleModuleRequired.h"
#include "ParticleSpriteEmitterInstance.h"
#include "TypeData/ParticleModuleTypeDataBase.h"

UParticleLODLevel* UParticleEmitter::GetCurrentLODLevel(FParticleEmitterInstance* Instance) const
{
    return Instance->CurrentLODLevel;
}

FParticleEmitterInstance* UParticleEmitter::CreateInstance(UParticleSystemComponent* InComponent)
{
    FParticleEmitterInstance* Instance = 0;

    UParticleLODLevel* LODLevel = LODLevels[0];


    if (!InComponent)
        return nullptr;

    if (LODLevel[0].RequiredModule->ParticleEmitterType == EParticleEmitterType::Sprite)
    {
        Instance = new FParticleSpriteEmitterInstance;
    }
    else if (LODLevel[0].RequiredModule->ParticleEmitterType == EParticleEmitterType::Mesh)
    {
        Instance = new FParticleMeshEmitterInstance;
    }
    if (!Instance)
        return nullptr;
    Instance->InitParameters(this, InComponent);
    

    if (Instance)
    {
        Instance->CurrentLODLevelIndex =0;
        Instance->CurrentLODLevel = LODLevels[Instance->CurrentLODLevelIndex];
        Instance->Init();

        
    }
    return Instance;
}

void UParticleEmitter::CacheEmitterModuleInfo()
{
    ParticleSize = sizeof(FBaseParticle);
    
    UParticleLODLevel* LODLevel = LODLevels[0];
    if (LODLevel == nullptr) return;
    UParticleModuleTypeDataBase* TypeData = LODLevel->TypeDataModule;
    if (TypeData)
    {
        
    }

    UParticleModuleRequired* RequiredModule = LODLevel->RequiredModule;
    
}
