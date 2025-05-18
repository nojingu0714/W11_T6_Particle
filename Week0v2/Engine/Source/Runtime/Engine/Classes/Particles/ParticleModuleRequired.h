#pragma once
#include "ParticleModule.h"

class UMaterial;
class UTexture;

class UParticleModuleRequired : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleRequired, UParticleModule)
public:
    UParticleModuleRequired() = default;
    ~UParticleModuleRequired() = default;
    
    UMaterial* SpriteTexture;

    
    uint8 bUseMaxDrawCount = false;
    int32 MaxDrawCount;
    
    uint8 bUseLocalSpace = true;
};
