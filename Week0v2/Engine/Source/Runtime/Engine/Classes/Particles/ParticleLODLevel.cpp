#include "ParticleLODLevel.h"

void UParticleLODLevel::AddModule(UParticleModule* InParticleModule)
{
    Modules.Add(InParticleModule);
    bIsModuleDirty = true;
}

void UParticleLODLevel::DeleteModule(UParticleModule* InParticleModule)
{
    for (UParticleModule* Module : Modules) 
    {
        if (Module == InParticleModule) 
        {
            Modules.Remove(InParticleModule);
            break;
        }
    }
    bIsModuleDirty = true;
}
