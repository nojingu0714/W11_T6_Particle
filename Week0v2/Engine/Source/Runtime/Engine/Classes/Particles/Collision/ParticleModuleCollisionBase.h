#pragma once
#include "Particles/ParticleModule.h"


enum EParticleCollisionComplete : int
{
    /**	Kill the particle when MaxCollisions is reached		*/
    EPCC_Kill,
    /**	Freeze the particle in place						*/
    EPCC_Freez,
    /**	Stop collision checks, but keep updating			*/
    EPCC_HaltCollisions,
    /**	Stop translations of the particle					*/
    EPCC_FreezeTranslation,
    /**	Stop rotations of the particle						*/
    EPCC_FreezeRotation,
    /**	Stop all movement of the particle					*/
    EPCC_FreezeMovement,
    EPCC_MAX,
};

class UParticleModuleCollisionBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleCollisionBase, UParticleModule)
public:
    UParticleModuleCollisionBase() = default;
    ~UParticleModuleCollisionBase() = default;
};
