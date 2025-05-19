#include "ParticleEmitterInstance.h"

#include "LaunchEngineLoop.h"
#include "Particles/ParticleEmitter.h"
#include "ParticleHelper.h"
#include "Engine/AssetManager.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleModuleRequired.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/Spawn/ParticleModuleSpawn.h"
#include "Renderer/Renderer.h"
#include "UObject/Casts.h"

FParticleEmitterInstance::FParticleEmitterInstance()
{


}

void FParticleEmitterInstance::InitParameters(UParticleEmitter* InEmitter, UParticleSystemComponent* InComponent)
{
    SpriteTemplate = InEmitter;
    Component = InComponent;
    SetupEmitterDuration();
}

void FParticleEmitterInstance::Init()
{
    if (SpriteTemplate== nullptr)
        return;

    bool bNeedsInit = (ParticleSize ==0);
    if (bNeedsInit)
    {
        ParticleSize = SpriteTemplate->ParticleSize;
        PayloadOffset = ParticleSize;

        ParticleStride = ParticleSize;
    }

    ParticleCounter =0;
    if (ParticleData == nullptr)
    {
        MaxActiveParticles	= 0;
        ActiveParticles		= 0;
        return;
    }

    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    ParticleEmitterRenderData.VertexBuffer = renderResourceManager->CreateEmptyDynamicVertexBuffer(sizeof(FParticleSpriteVertex) * MaxActiveParticles);
}

void FParticleEmitterInstance::Tick(float DeltaTime)
{
    EmitterTime += DeltaTime;

    FVector CurrentLocation = Component->GetWorldLocation();
    FVector InitialLocation = CurrentLocation;
    FVector InitialVelocity = (CurrentLocation - OldLocation) / DeltaTime;
    OldLocation = CurrentLocation;

    // 충돌이나 데스 이벤트 기반 스폰시 필요 
    FParticleEventInstancePayload* EventPayload = nullptr;
 
    // 4) Spawn Phase
    if (!SpriteTemplate)
    {
        return;
    }
    //   4-1) 이 LODLevel 의 Spawn 모듈 수집
    for (UParticleModule* Module : CurrentLODLevel->Modules)
    {
        if (UParticleModuleSpawn* Spawn = Cast<UParticleModuleSpawn>(Module))
        {
            SpawnModules.Add(Spawn);
        }
        else if (UParticleModuleRequired* Required = Cast<UParticleModuleRequired>(Module))
        {
            RequiredModule = Required;
        }
        else
        {
            UpdateModules.Add(Module);
        }
    }


    //   4-2) 모듈별로 생성 개수 계산 후 SpawnParticles 호출
    for (UParticleModuleSpawn* SpawnModule : SpawnModules)
    {
        // SpawnRate 분포에서 실제 값 가져오기
        float Rate;
        int32 Count;
        SpawnModule->GetSpawnAmount(this, 0, 0.0f, DeltaTime, Count, Rate); // Owner에 this를 주었기 때문에 EmitterTime 가져다 쓸거임
        if (Count > 0)
        {
            float Increment = DeltaTime / (float)Count;
            SpawnParticles(
                Count,
                /* StartTime */    EmitterTime,
                /* Increment */    Increment,
                /* Location */     InitialLocation,
                /* Velocity */     InitialVelocity,
                /* EventPayload */ EventPayload
            );
        }
    }

    for (auto Particle : BaseParticles)
    {
        // 활성 파티클의 버퍼 오프셋 계산
        // int32 Index = ParticleIndices[i];
        // FBaseParticle& P = *reinterpret_cast<FBaseParticle*>(ParticleData + Index * ParticleStride);

        Particle->OldLocation     = Particle->Location;
        Particle->Velocity        = Particle->BaseVelocity;
        Particle->RotationRate    = Particle->BaseRotationRate;
        Particle->Size            = Particle->BaseSize;
        Particle->Color           = Particle->BaseColor;

        for (UParticleModule* UpdateModule : UpdateModules)
        {
            UpdateModule->Update(this, Particle, DeltaTime);
        }

        Particle->Location      += Particle->Velocity     * DeltaTime;
        Particle->Rotation      += Particle->RotationRate * DeltaTime;
        Particle->RelativeTime  += DeltaTime * Particle->OneOverMaxLifetime;

        // 수명 만료 시 파티클 제거
        if (Particle->RelativeTime >= 1.0f)
        {
            // KillParticle(Particle);
            DeadParticles.Add(Particle);
        }
    }
    KilParticles();

    //UE_LOG(LogLevel::Warning, "Particles : %d", BaseParticles.Num());

    SpawnModules.Empty();
    UpdateModules.Empty();
}

void FParticleEmitterInstance::SetupEmitterDuration()
{
    
}

void FParticleEmitterInstance::SpawnParticles(int32 Count, float StartTime, float Increment, const FVector& InitialLocation,
    const FVector& InitialVelocity, struct FParticleEventInstancePayload* EventPayload)
{
    if (Count <= 0)
    {
        return;
    }

    // 분할(interpolation) 계산: 파티클마다 보간 비율
    auto ComputeInterp = [Count](int32 i)
    {
        return (Count > 1) ? (static_cast<float>(i) / (Count - 1)) : 1.0f;
    };

    for (int32 i = 0; i < Count; ++i)
    {
        // 새 파티클 슬롯 확보, Particle 포인터 생성, ActiveParticles++ & 인덱스 추가
        // DECLARE_PARTICLE_PTR(Particle, );
        FBaseParticle* Particle = new FBaseParticle();
        
        // 1) PreSpawn: 기본값(BaseLocation, BaseVelocity 등) 세팅
        PreSpawn(*Particle, InitialLocation, InitialVelocity);

        // 2) SpawnTime 설정 (이 배치의 스폰 시점)
        const float SpawnTime = StartTime + Increment * i;

        // 3) 각 Spawn 모듈 적용

        // 4) PostSpawn: 모듈간 보간, 이벤트 페이로드 적용 등 최종 처리
        const float Interp = ComputeInterp(i);
        PostSpawn(Particle, Interp, SpawnTime);
        BaseParticles.Add(Particle);
    }
}

void FParticleEmitterInstance::PreSpawn(FBaseParticle& Particle, const FVector& InitLocation, const FVector& InitVelocity)
{

    Particle.Location     = InitLocation;
    Particle.OldLocation  = InitLocation;
    
    Particle.BaseVelocity    = InitVelocity;
    Particle.Velocity        = InitVelocity;
}

void FParticleEmitterInstance::PostSpawn(FBaseParticle* Particle, float InterpolationPercentage, float SpawnTime)
{
    // Offset caused by any velocity
    Particle->OldLocation = Particle->Location;
    Particle->Location   +=  FVector(Particle->Velocity) * SpawnTime;
}


void FParticleEmitterInstance::KillParticle(int32 Index)
{
}

void FParticleEmitterInstance::KilParticles()
{
    for (auto Dead : DeadParticles)
    {
        BaseParticles.Remove(Dead);
        delete Dead;
    }
    DeadParticles.Empty();
}

bool FParticleEmitterInstance::IsDynamicDataRequired(UParticleLODLevel* InCurrentLODLevel)
{
    if ((ActiveParticles <= 0) || 
    (SpriteTemplate && (SpriteTemplate->EmitterRenderMode == ERM_None)))
    {
        return false;
    }

    if ((InCurrentLODLevel == NULL) || (InCurrentLODLevel->bEnabled == false)
        /*||
        ((InCurrentLODLevel->RequiredModule->bUseMaxDrawCount == true) && (InCurrentLODLevel->RequiredModule->MaxDrawCount == 0))*/)
    {
        return false;
    }

    if (Component == NULL)
    {
        return false;
    }
    return true;
}

bool FParticleEmitterInstance::FillReplayData(FDynamicEmitterReplayDataBase& OutData)
{
    // Make sure there is a template present
    if (!SpriteTemplate)
    {
        return false;
    }
    
    // Allocate it for now, but we will want to change this to do some form
    // of caching
    if (ActiveParticles <= 0 || !bEnabled)
    {
        return false;
    }

    // If the template is disabled, don't return data.
    UParticleLODLevel* LODLevel = SpriteTemplate->GetCurrentLODLevel(this);
    if ((LODLevel == NULL) || (LODLevel->bEnabled == false))
    {
        return false;
    }
    // Must be filled in by implementation in derived class
    OutData.eEmitterType = DET_Unknown;
    
    OutData.LocalToWorld = Component->GetWorldMatrix();

    OutData.ParticleEmitterRenderData = ParticleEmitterRenderData;

    OutData.ActiveParticleCount = ActiveParticles;
    OutData.ParticleStride = ParticleStride;
    //OutData.SortMode = SortMode;

    // Take scale into account
    OutData.Scale = FVector::OneVector;
    if (Component)
    {
        OutData.Scale = FVector(Component->GetWorldScale());
    }

    int32 ParticleMemSize = MaxActiveParticles * ParticleStride;

    //언리얼에서도 모든 emitter가 스프라이트인 걸로 가정하고 한다.
    FDynamicSpriteEmitterReplayDataBase* NewReplayData =
    static_cast< FDynamicSpriteEmitterReplayDataBase* >( &OutData );

    //NewReplayData->RequiredModule = LODLevel->RequiredModule->CreateRendererResource();
    NewReplayData->Material = NULL;	// 파생된 구현에서 설정해야 합니다.
    
    NewReplayData->MaxDrawCount =
        (LODLevel->RequiredModule->bUseMaxDrawCount == true) ? LODLevel->RequiredModule->MaxDrawCount : -1;
    
    NewReplayData->PivotOffset =   (PivotOffset);
    NewReplayData->bUseLocalSpace = LODLevel->RequiredModule->bUseLocalSpace;

    return true;
}

UMaterial* FParticleEmitterInstance::GetCurrentMaterial()
{
    return RequiredModule->SpriteTexture;
}
