#include "ParticleSystemComponent.h"

#include "ParticleEmitter.h"
#include "ParticleEmitterInstance.h"
#include "ParticleHelper.h"
#include "ParticleSystem.h"

void UParticleSystemComponent::TickComponent(float DeltaTime)
{
    UFXSystemComponent::TickComponent(DeltaTime);

    for (auto Instance : EmitterInstances)
    {
        Instance->Tick(DeltaTime);
    }
}

UParticleSystemComponent::UParticleSystemComponent()
{
    
}

void UParticleSystemComponent::BeginPlay()
{
    UFXSystemComponent::BeginPlay();
    InitParticles();

}

void UParticleSystemComponent::UpdateDynamicData()
{
}

void UParticleSystemComponent::InitParticles()
{
    ResetSystem();

    if (Template != nullptr)
    {
        int32 NumInstances = EmitterInstances.Num();
        int32 NumEmiiters = Template->Emitters.Num();
        const bool bIsFirstCreate = NumInstances == 0;

        int32 PreferredLODLevel = 0;

        for (int32 Idx = 0; Idx < NumEmiiters; Idx++)
        {
            UParticleEmitter* Emitter = Template->Emitters[Idx];
            if (Emitter)
            {
                FParticleEmitterInstance* Instance = NumInstances == 0 ? nullptr : EmitterInstances[Idx];

                if (Instance)
                {
                    
                }
                else
                {
                    Instance = Emitter->CreateInstance(this);
                    // EmitterInstances[Idx] = Instance;
                    EmitterInstances.Add(Instance);
                }
                if (Instance)
                {
                    Instance->bEnabled = true;
                    Instance->InitParameters(Emitter, this);
                    Instance->Init();
                }
            }
        }
    }
}
inline void UParticleSystemComponent::ResetSystem()
{
    for (auto instance : EmitterInstances)
    {
        delete instance;
    }
    EmitterInstances.Empty();
    for (auto database : EmitterRenderData)
    {
        delete database;
    }
    EmitterRenderData.Empty();
}
