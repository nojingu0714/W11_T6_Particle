#pragma once
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"

class AParticleActor : public AActor
{
    DECLARE_CLASS(AParticleActor, AActor)
public:
    AParticleActor();
    ~AParticleActor();

    void Tick(float DeltaTime) override;
    UParticleSystemComponent* ParticleSystemComponent;
};
