#include "ParticleSystemComponent.h"

#include "LaunchEngineLoop.h"
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
    // CreateDynamicData();
    UpdateDynamicData();
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
        
        // 그냥 리플레이 데이터에 다 박으면 안되나????????????????"??????
        FDynamicSpriteEmitterData* SpriteData = new FDynamicSpriteEmitterData();
        SpriteData->EmitterIndex      = EmitterIndex;
        auto& Source = SpriteData->Source;

        // — 기본 리플레이 필드
        Source.eEmitterType        = DET_Sprite;
        Source.ActiveParticleCount = Instance->ActiveParticles;
        Source.ParticleStride      = Instance->ParticleStride;
        Source.Scale               = Instance->Component->GetWorldScale();

        // — 메모리 블록(ParticleData + ParticleIndices) 복사
        int32 DataBytes   = Instance->MaxActiveParticles * Instance->ParticleStride;
        int32 IndexCount  = Instance->MaxActiveParticles;
        Source.DataContainer.Alloc(DataBytes, IndexCount);
        std::memcpy(Source.DataContainer.ParticleData,
                    Instance->ParticleData,
                    DataBytes);
        std::memcpy(Source.DataContainer.ParticleIndices,
                    Instance->ParticleIndices,
                    sizeof(uint16) * Instance->ActiveParticles);

        // — 파티클 시스템 세팅
        Source.Material         = Instance->RequiredModule->SpriteTexture;
        Source.RequiredModule   = Instance->RequiredModule;
        Source.PivotOffset      = Instance->PivotOffset;
        Source.MaxDrawCount     = Instance->ActiveParticles;
        //초기값을 멀로 줘야하지?????
        Source.bUseLocalSpace   = false;

        // 렌더용 데이터
        Source.ComponentLocalToWorld = Instance->Component->GetWorldMatrix();
        Source.ParticleEmitterRenderData = Instance->GetRenderData();

        // 3) 렌더러가 사용할 추가 세팅
        SpriteData->bSelected = false;
        SpriteData->bValid    = true;
        SpriteData->Init(SpriteData->bSelected);
        
        // 2-5) 배열에 추가
        EmitterRenderData.Add(SpriteData);
    
        ++EmitterIndex;
    }

    MarkRenderDynamicDataDirty();
}

void UParticleSystemComponent::MarkRenderDynamicDataDirty()
{
    // 1) 자체 플래그를 세팅
    bRenderDataDirty = true;
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
        if (EmitterInst || !EmitterInst->bEnabled || EmitterInst->ActiveParticles == 0)
        {
            // Create a new dynamic data object for this emitter instance
            NewDynamicEmitterData = EmitterInst->GetDynamicData(false);
            
            if( NewDynamicEmitterData != NULL )
            {
                //NewDynamicEmitterData->StatID = EmitterInst->SpriteTemplate->GetStatIDRT();
                NewDynamicEmitterData->bValid = true;
                NewDynamicEmitterData->bSelected    = false;
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
