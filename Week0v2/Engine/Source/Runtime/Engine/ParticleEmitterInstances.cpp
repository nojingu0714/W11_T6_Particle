#include "ParticleEmitterInstances.h"

#include "Particles/ParticleEmitter.h"
#include "ParticleHelper.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/Spawn/ParticleModuleSpawn.h"
#include "UObject/Casts.h"

void FParticleEmitterInstances::InitParameters(UParticleEmitter* InEmitter, UParticleSystemComponent* InComponent)
{
    SpriteTemplate = InEmitter;
    Component = InComponent;
    SetupEmitterDuration();
}

void FParticleEmitterInstances::Init()
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
    }
}

void FParticleEmitterInstances::Tick(float DeltaTime)
{
    EmitterTime += DeltaTime;

    // 2) 초기 위치·속도 계산
    FVector CurrentLocation = Component->GetWorldLocation();
    FVector InitialLocation = CurrentLocation;
    FVector InitialVelocity = (CurrentLocation - OldLocation) / DeltaTime;
    OldLocation = CurrentLocation;

    // 3) 이벤트 페이로드 (이 예시는 일반 Rate Spawn 이므로 nullptr)
    FParticleEventInstancePayload* EventPayload = nullptr;

    // 4) Spawn Phase
    if (!SpriteTemplate)
    {
        return;
    }

    //   4-1) 이 LODLevel 의 Spawn 모듈 수집
    TArray<UParticleModuleSpawn*> SpawnModules;
    for (UParticleModule* Module : CurrentLODLevel->Modules)
    {
        if (UParticleModuleSpawn* Spawn = Cast<UParticleModuleSpawn>(Module))
        {
            SpawnModules.Add(Spawn);
        }
    }

    //   4-2) 모듈별로 생성 개수 계산 후 SpawnParticles 호출
    for (UParticleModuleSpawn* SpawnModule : SpawnModules)
    {
        // SpawnRate 분포에서 실제 값 가져오기
        float Rate = SpawnModule->Rate;
        int32 Count = Rate * DeltaTime;
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
}

void FParticleEmitterInstances::SetupEmitterDuration()
{
    
}

void FParticleEmitterInstances::SpawnParticles(int32 Count, float StartTime, float Increment, const FVector& InitialLocation,
    const FVector& InitialVelocity, struct FParticleEventInstancePayload* EventPayload)
{
    // for (int32 i = 0; i < Count; i++)
    // {
    //     DECLARE_PARTICLE_PTR
    //     PreSpawn(Particle, InitialLocation, InitialVelocity);
    //
    //     for (int32 ModuleIndex = 0; ModuleIndex < LODLevel->SpawnModules.Num(); ModuleIndex++)
    //     {
    //         ...
    //     }
    //
    //     PostSpawn(Particle, Interp, SpawnTime);
    // }
}

void FParticleEmitterInstances::SpawnParticles(int32 Count, float StartTime, float Increment, const FVector& InitialLocation,
    const FVector& InitialVelocity)
{
}


void FParticleEmitterInstances::KillParticle(int32 Index)
{
}
