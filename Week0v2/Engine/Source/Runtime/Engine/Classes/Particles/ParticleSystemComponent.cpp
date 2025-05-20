#include "ParticleSystemComponent.h"

#include "ParticleEmitter.h"
#include "ParticleEmitterInstance.h"
#include "ParticleHelper.h"
#include "ParticleModuleRequired.h"
#include "ParticleSystem.h"
#include "UserInterface/Debug/DebugViewModeHelpers.h"

void UParticleSystemComponent::TickComponent(float DeltaTime)
{
    UFXSystemComponent::TickComponent(DeltaTime);

    for (FParticleEmitterInstance* Instance : EmitterInstances)
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
    for (FDynamicEmitterDataBase* OldData : EmitterRenderData)
    {
        delete OldData;
    }
    EmitterRenderData.Empty();
    
    // 2) 각 EmitterInstance에 대해 렌더 데이터 생성
    int32 EmitterIndex = 0;
    for (FParticleEmitterInstance* Instance : EmitterInstances)
    {
        if (!Instance || !Instance->bEnabled || Instance->ActiveParticles == 0)
        {
            ++EmitterIndex;
            continue;
        }
    
        // 2-1) ReplayDataBase를 얻어오거나 바로 BuildDynamicData 호출
        FDynamicEmitterReplayDataBase* ReplayData = Instance->GetReplayData();
    
        // 2-2) 타입에 따라 적절한 FDynamicEmitterDataBase 파생형 생성
        FDynamicSpriteEmitterData* SpriteData = new FDynamicSpriteEmitterData();
        SpriteData->EmitterIndex      = EmitterIndex;
        SpriteData->Source.Material = Instance->RequiredModule->SpriteTexture; // 예시
        SpriteData->Source.RequiredModule = Instance->RequiredModule;
    
        // 2-3) 버텍스 버퍼 / 인덱스 버퍼에 FParticleSpriteVertex 채우기
        // (앞서 설명한 대로 ParticleDataContainer를 순회해 VertexBufferRHI에 업로드)
    
        // 2-4) 카운트·스트라이드·VF 설정
        // SpriteData->DynamicVertexStride = SpriteData->GetDynamicVertexStride();
        // SpriteData->NumVertices   = Instance->ActiveParticles * 4;
        // SpriteData->NumPrimitives = Instance->ActiveParticles * 2;
        // SpriteData->VertexFactory = &GParticleSpriteVertexFactory;
        // SpriteData->IndexBufferRHI= GParticleIndexBuffer.IndexBufferRHI;
        //
        // 2-5) 배열에 추가
        EmitterRenderData.Add(SpriteData);
    
        ++EmitterIndex;
    }
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
