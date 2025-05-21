#include "ParticleActor.h"

#include "Particles/MyParticleSystem.h"

AParticleActor::AParticleActor()
{

}

AParticleActor::~AParticleActor()
{
}

void AParticleActor::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
}

void AParticleActor::SetDefaultParticleSystem()
{
    ParticleSystemComponent = AddComponent<UParticleSystemComponent>(EComponentOrigin::Constructor);
    RootComponent = ParticleSystemComponent;

    ParticleSystemComponent->Template = FObjectFactory::ConstructObject<UMyParticleSystem>(this);
    ParticleSystemComponent->InitParticles();
}
