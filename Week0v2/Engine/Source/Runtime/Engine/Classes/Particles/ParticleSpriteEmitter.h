#pragma once

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "ParticleEmitter.h"

class UParticleSystemComponent;

enum EParticleScreenAlignment : int
{
    PSA_FacingCameraPosition,
    PSA_Square,
    PSA_Rectangle,
    PSA_Velocity,
    PSA_AwayFromCenter,
    PSA_TypeSpecific,
    PSA_FacingCameraDistanceBlend,
    PSA_MAX,
};

class UParticleSpriteEmitter : public UParticleEmitter
{

    
};