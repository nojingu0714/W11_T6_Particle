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
    bool bEnabled = true;

    void AddModule(UParticleModule* InParticleModule);
    void DeleteModule(UParticleModule* InParticleModule);

    UParticleModuleRequired* RequiredModule;    
    TArray<UParticleModule*> Modules;
    bool bIsModuleDirty = false;    // 현재는 Module 추가 삭제에 관해서 dirty 플래그가 켜짐
    UParticleModuleTypeDataBase* TypeDataModule;
};
