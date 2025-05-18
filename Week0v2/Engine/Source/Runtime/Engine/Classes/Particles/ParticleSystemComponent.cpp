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
    CreateDynamicData();
    
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
    
    ClearEmitterDataArray();
}

void UParticleSystemComponent::CreateDynamicData()
{
    // Only proceed if we have any live particles or if we're actively replaying/capturing
    if (EmitterInstances.Num() > 0)
    {
        int32 LiveCount = 0;
        for (int32 EmitterIndex = 0; EmitterIndex < EmitterInstances.Num(); EmitterIndex++)
        {
            FParticleEmitterInstance* EmitInst = EmitterInstances[EmitterIndex];
            if (EmitInst)
            {
                if (EmitInst->ActiveParticles > 0)
                {
                    LiveCount++;
                }
            }
        }

        if (LiveCount == 0)
        {
            return ;
        }
    }

    
    //FParticleDynamicData* ParticleDynamicData = new FParticleDynamicData();
    //
    //ParticleDynamicData->DynamicEmitterDataArray.Empty();
    //ParticleDynamicData->DynamicEmitterDataArray.Reserve(EmitterInstances.Num());

    
    ClearEmitterDataArray();
    EmitterRenderData.Reserve(EmitterInstances.Num());
    
    for (int32 EmitterIndex = 0; EmitterIndex < EmitterInstances.Num(); EmitterIndex++)
    {
        
        FDynamicEmitterDataBase* NewDynamicEmitterData = NULL;
        FParticleEmitterInstance* EmitterInst = EmitterInstances[EmitterIndex];
        if (EmitterInst)
        {
            // Create a new dynamic data object for this emitter instance
            NewDynamicEmitterData = EmitterInst->GetDynamicData(false);
            
            if( NewDynamicEmitterData != NULL )
            {
                //NewDynamicEmitterData->StatID = EmitterInst->SpriteTemplate->GetStatIDRT();
                NewDynamicEmitterData->bValid = true;
                NewDynamicEmitterData->EmitterIndex = EmitterIndex;
                NewDynamicEmitterData->EmitterIndex = EmitterIndex;
                EmitterRenderData.Add(NewDynamicEmitterData);

                
                // FDynamicEmitterReplayDataBase* NewEmitterReplayData = EmitterInst->GetReplayData();
                // if (NewEmitterReplayData != NULL)
                // {
                //     NewDynamicEmitterData->ReplayData = NewEmitterReplayData;
                // }
                //
            }
            
        }
    }
    //DynamicData = ParticleDynamicData;
    //return ParticleDynamicData;

    
}

void UParticleSystemComponent::ClearEmitterDataArray()
{    
    for (int32 Index = 0; Index < EmitterRenderData.Num(); Index++)
    {
        FDynamicEmitterDataBase* Data =	EmitterRenderData[Index];
        delete Data;
    }
    EmitterRenderData.Empty();    
}
