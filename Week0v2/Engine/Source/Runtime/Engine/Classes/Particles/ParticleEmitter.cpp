#include "ParticleEmitter.h"

#include "ParticleEmitterInstances.h"
#include "ParticleHelper.h"
#include "ParticleLODLevel.h"
#include "TypeData/ParticleModuleTypeDataBase.h"

FParticleEmitterInstances* UParticleEmitter::CreateInstance(UParticleSystemComponent* InComponent)
{
    FParticleEmitterInstances* Instance = 0;

    UParticleLODLevel* LODLevel = LODLevels[0];

    if (LODLevel == nullptr)
        return nullptr;
    if (LODLevel->TypeDataModule)
    {
        Instance = LODLevel->TypeDataModule->CreateInstance(this, InComponent);
    }
    else
    {
        if (!InComponent)
            return nullptr;
        Instance = new FParticleEmitterInstances;
        if (!Instance)
            return nullptr;
        Instance->InitParameters(this, InComponent);
    }

    if (Instance)
    {
        Instance->CurrentLODLevelIndex =0;
        Instance->CurrentLODLevel = LODLevels[Instance->CurrentLODLevelIndex];
        Instance->Init();
    }
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
