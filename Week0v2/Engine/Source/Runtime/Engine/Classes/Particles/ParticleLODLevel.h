#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleModuleTypeDataBase;
class UParticleModuleRequired;
class UParticleModule;

class UParticleLODLevel : public UObject
{
    DECLARE_CLASS(UParticleLODLevel, UObject)
public:
    UParticleLODLevel() = default;
    ~UParticleLODLevel() = default;

    

    int32 Level = 0;
    bool bEnabled;

    UParticleModuleRequired* RequiredModule;    
    TArray<UParticleModule*> Modules;
    UParticleModuleTypeDataBase* TypeDataModule;
};
