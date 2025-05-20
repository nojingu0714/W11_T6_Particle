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

    for (auto Instance : EmitterInstances)
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
    // 1) 이전 데이터 해제
    for (FDynamicEmitterDataBase* OldData : EmitterRenderData)
    {
        delete OldData;
    }
    EmitterRenderData.Empty();
    
    // 2) 각 EmitterInstance별 리플레이 데이터 준비
    int32 EmitterIndex = 0;
    for (FParticleEmitterInstance* Instance : EmitterInstances)
    {
        if (!Instance || !Instance->bEnabled || Instance->ActiveParticles == 0)
        {
            ++EmitterIndex;
            continue;
        }
        
        auto* SpriteData = new FDynamicSpriteEmitterData();
        SpriteData->EmitterIndex = EmitterIndex;
        auto& Source = SpriteData->Source;

        // — 리플레이 필드 채우기
        Source.eEmitterType        = DET_Sprite;
        Source.ActiveParticleCount = Instance->ActiveParticles;
        // VertexShader 쪽에서 <Position,Size,Rotation,Color>만 쓰므로
        Source.ParticleStride      = Instance->ParticleStride;
        Source.Scale               = Instance->Component->GetWorldScale();

        // — 시뮬레이션용 메모리 데이터 복사(필요하다면)
        int32 DataBytes  = Instance->MaxActiveParticles * Instance->ParticleStride;
        int32 IndexCount = Instance->MaxActiveParticles;
        Source.DataContainer.Alloc(DataBytes, IndexCount);
        std::memcpy(Source.DataContainer.ParticleData,    Instance->ParticleData,   DataBytes);
        std::memcpy(Source.DataContainer.ParticleIndices, Instance->ParticleIndices,
                    sizeof(uint16) * Instance->ActiveParticles);

        // — 머티리얼·모듈 정보
        Source.Material       = Instance->RequiredModule->SpriteTexture;
        Source.RequiredModule = Instance->RequiredModule;
        Source.PivotOffset    = Instance->PivotOffset;
        Source.MaxDrawCount   = Instance->ActiveParticles;
        // Source.bUseLocalSpace = Instance->bUseLocalSpace;

        // — RenderData 초기화
        SpriteData->bSelected = false;
        SpriteData->bValid    = true;
        SpriteData->Init(SpriteData->bSelected);

        EmitterRenderData.Add(SpriteData);
        ++EmitterIndex;
    }

    // 3) RHI 버퍼 생성·업데이트 (VertexBuffer만)
    for (FDynamicEmitterDataBase* Base : EmitterRenderData)
    {
        auto* SpriteData = static_cast<FDynamicSpriteEmitterData*>(Base);
        auto& Src = SpriteData->Source;

        // 파티클당 1정점, 총 정점 수 = ActiveParticleCount
        int32 VertexCount = Src.ActiveParticleCount;
        int32 VertStride  = sizeof(FParticleSpriteVertex);
        int32 VBSize      = VertexCount * VertStride;

        // --- VertexBuffer 생성 또는 재할당 ---
        if (!SpriteData->VertexBuffer || SpriteData->VertexBufferSize < VBSize)
        {
            if (SpriteData->VertexBuffer)
            {
                SpriteData->VertexBuffer->Release();
                SpriteData->VertexBuffer = nullptr;
            }
            D3D11_BUFFER_DESC vbDesc = {};
            vbDesc.Usage          = D3D11_USAGE_DYNAMIC;
            vbDesc.ByteWidth      = VBSize;
            vbDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
            vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            GEngineLoop.GraphicDevice.Device->CreateBuffer(&vbDesc, nullptr, &SpriteData->VertexBuffer);
            SpriteData->VertexBufferSize = VBSize;
        }

        // --- VertexBuffer에 데이터 채우기 ---
        D3D11_MAPPED_SUBRESOURCE Mapped;
        auto* DC = GEngineLoop.GraphicDevice.DeviceContext;
        DC->Map(SpriteData->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);

        // 파티클 1개당 하나의 FParticleSpriteVertex
        SpriteData->GetVertexAndIndexData(
            Mapped.pData,        // VertexData
            nullptr,             // FillIndexData (인덱스 버퍼 사용 안 함)
            nullptr,             // ParticleOrder
            /*CameraPos=*/{},    // 필요한 값으로 교체
            /*LocalToWorld=*/FMatrix::Identity,
            /*InstanceFactor=*/1
        );
        {
            auto* VertArray = reinterpret_cast<FParticleSpriteVertex*>(Mapped.pData);
            int32 Count = SpriteData->Source.ActiveParticleCount;
            int32 LogMax = FMath::Min(Count, 10);  // 최대 10개까지만

            UE_LOG(LogLevel::Warning, TEXT("=== SpriteVertex After UpdateDynamicData (up to %d) ==="), LogMax);
            for (int i = 0; i < LogMax; ++i)
            {
                const auto& V = VertArray[i];
                UE_LOG(LogLevel::Warning, 
                    TEXT("V[%d]: Pos=(%.2f,%.2f,%.2f)  Size=(%.2f,%.2f)  Rot=%.2f  Color=(%.2f,%.2f,%.2f,%.2f)  RelT=%.2f"),
                    i,
                    V.Position.X,  V.Position.Y,  V.Position.Z,
                    V.Size.X,      V.Size.Y,
                    V.Rotation,
                    V.Color.R,     V.Color.G,     V.Color.B,     V.Color.A,
                    V.RelativeTime
                );
            }
        }
        DC->Unmap(SpriteData->VertexBuffer, 0);
    }

    // 다음 렌더 패스에서 UpdateDynamicData가 반영되도록 플래그
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
        if (EmitterInst)
        {
            // Create a new dynamic data object for this emitter instance
            NewDynamicEmitterData = EmitterInst->GetDynamicData(false);
            
            if( NewDynamicEmitterData != NULL )
            {
                //NewDynamicEmitterData->StatID = EmitterInst->SpriteTemplate->GetStatIDRT();
                NewDynamicEmitterData->bValid = true;
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
