#include "ParticleActor.h"

#include "Particles/MyParticleSystem.h"

AParticleActor::AParticleActor()
{
    ParticleSystemComponent = AddComponent<UParticleSystemComponent>(EComponentOrigin::Constructor);
    RootComponent = ParticleSystemComponent;
    
    ParticleSystemComponent->Template = FObjectFactory::ConstructObject<UMyParticleSystem>(this);
    ParticleSystemComponent->InitParticles();
}

AParticleActor::~AParticleActor()
{
}
