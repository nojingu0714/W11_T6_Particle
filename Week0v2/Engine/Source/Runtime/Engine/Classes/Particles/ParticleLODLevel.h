#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleModule;

class UParticleLODLevel : public UObject
{
    DECLARE_CLASS(UParticleLODLevel, UObject)
public:
    UParticleLODLevel() = default;
    ~UParticleLODLevel() = default;


    int32 Level;
    bool bEnabled;

    // UParticleModuleRequired* RequiredModule;    
    // TArray<UParticleModule*> Modules;
    // UParticleModuleTypeDataBase* TypeDataModule;
};
