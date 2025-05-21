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

void FParticleEmitterInstance::InitParameters(UParticleEmitter* InEmitter, UParticleSystemComponent* InComponent, float InEmitterDuration)
{
    SpriteTemplate = InEmitter;
    Component = InComponent;
    SetupEmitterDuration(InEmitterDuration); // 기본적으로 1 
}

void FParticleEmitterInstance::Init()
{
    if (SpriteTemplate== nullptr)
        return;

    // 1) RequiredModule 과 Spawn/Update 모듈은 이미 ctor 또는 InitParameters()에서 수집했다고 가정

    // 2) ParticleSize, ParticleStride 계산 (FBaseParticle만 저장할 경우)
    const int32 BaseSize        = sizeof(FBaseParticle);
    const int32 RequiredPayload = SpriteTemplate->ReqInstanceBytes;

    // 2) 전체 파티클당 사이즈 (페이로드 포함)
    ParticleSize       = BaseSize + RequiredPayload;
    // 16바이트 정렬 (SSE-friendly)
    ParticleSize       = (ParticleSize + 15) & ~15;

    
    // 3) 최대 파티클 수 설정
    MaxActiveParticles = 2048;

    // 3) 스트라이드 계산 (패딩 포함)
    ParticleStride     = ParticleSize;
    
    // 4) 시뮬레이션용 메모리 블록 할당
    {
        int32 DataBytes   = ParticleStride * MaxActiveParticles;
        int32 IndexCount  = MaxActiveParticles;
        int32 BlockSize   = DataBytes + sizeof(uint16) * IndexCount;
        

        ParticleData      = static_cast<uint8*>(std::malloc(BlockSize));
        if (!ParticleData)
        {
            UE_LOG(LogLevel::Warning, "Particle Data Malloc Failed");
        }
        ParticleIndices   = reinterpret_cast<uint16*>(ParticleData + DataBytes);
    }

    ParticleCounter =0;
    if (ParticleData == nullptr)
    {
        MaxActiveParticles	= 0;
        ActiveParticles		= 0;
        return;
    }
    // 5) 초기 카운터
    ActiveParticles     = 0;
    ParticleCounter     = 0;
    SpawnFraction       = 0.0f;

    // 6) 타이밍 및 위치 초기화
    EmitterTime         = 0.0f;
    SecondsSinceCreation= 0.0f;
    Location            = Component->GetWorldLocation();
    OldLocation         = Location;

    // // 7) 바운딩 초기화 (필요하다면)
    // ParticleBoundingBox.Init();

    // 8) 렌더 데이터 갱신 플래그
    IsRenderDataDirty    = true;

    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    ParticleEmitterRenderData.VertexBuffer = renderResourceManager->CreateEmptyDynamicVertexBuffer(sizeof(FParticleSpriteVertex) * MaxActiveParticles);
}

void FParticleEmitterInstance::Tick(float DeltaTime)
{
    SecondsSinceCreation += DeltaTime;
    if (SecondsSinceCreation > EmitterDuration) 
    {
        SecondsSinceCreation -= EmitterDuration;
    }

    EmitterTime = SecondsSinceCreation / EmitterDuration;

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

    // 변화가 생긴 경우, 다시 Module 재분배
    if (CurrentLODLevel->bIsModuleDirty) 
    {
        ClassifyModules();
        CurrentLODLevel->bIsModuleDirty = false;
    }

    //   4-2) 모듈별로 생성 개수 계산 후 SpawnParticles 호출
    if (SpawnModule != nullptr)
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
    
    for (int32 i = 0; i < ActiveParticles; ++i)
    {
        const uint16 SlotIndex = ParticleIndices[i];
        // 버퍼 오프셋 계산
        uint8* DataPtr = ParticleData + SlotIndex * ParticleStride;
        FBaseParticle& P = *reinterpret_cast<FBaseParticle*>(DataPtr);

        // (a) 프레임 초기값 리셋
        P.OldLocation    = P.Location;
        P.Velocity       = P.BaseVelocity;
        P.RotationRate   = P.BaseRotationRate;
        P.Size           = P.BaseSize;
        P.Color          = P.BaseColor;


        // (c) 물리적 통합
        P.Location      += P.Velocity * DeltaTime;
        P.Rotation      += P.RotationRate * DeltaTime;
        P.RelativeTime  += DeltaTime * P.OneOverMaxLifetime;

        // (d) 수명 초과 시 슬롯 제거
        if (P.RelativeTime >= 1.0f)
        {
            KillParticle(i);
            --i;  // ActiveParticles가 하나 줄어들었으니 인덱스 보정
        }
    }

    // (b) 모든 Update 모듈 실행, 전달해준 this를 통해 목록에 접근하므로 
    //  offset은 딱히 무슨 값 넣어도 상관 없음
    for (UParticleModule* Mod : UpdateModules)
    {
        Mod->Update(this, /*Offset=*/0 * ParticleStride, DeltaTime);
    }

    UE_LOG(LogLevel::Warning, "Particles : %d", ActiveParticles);
}

void FParticleEmitterInstance::ClassifyModules()
{
    // LODLevel이 소유하고 있는 Modules 분류하기

    SpawnModules.Empty();
    UpdateModules.Empty();

    for (UParticleModule* Module : CurrentLODLevel->Modules)
    {
        if (UParticleModuleSpawn* Spawn = Cast<UParticleModuleSpawn>(Module))
        {
            SpawnModule = Spawn;
        }
        else if (UParticleModuleRequired* Required = Cast<UParticleModuleRequired>(Module))
        {
            RequiredModule = Required;
        }

        if (Module->bSpawnModule)
        {
            SpawnModules.Add(Module);
        }

        if (Module->bUpdateModule)
        {
            UpdateModules.Add(Module);
        }

    }
}

void FParticleEmitterInstance::SetupEmitterDuration(float InEmitterDuration)
{
    EmitterDuration = InEmitterDuration;
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
        // // 새 파티클 슬롯 확보, Particle 포인터 생성, ActiveParticles++ & 인덱스 추가
        // // DECLARE_PARTICLE_PTR(Particle, );
        // FBaseParticle* Particle = new FBaseParticle();
        //
        // // 1) PreSpawn: 기본값(BaseLocation, BaseVelocity 등) 세팅
        // PreSpawn(*Particle, InitialLocation, InitialVelocity);
        //
        // // 2) SpawnTime 설정 (이 배치의 스폰 시점)
        // const float SpawnTime = StartTime + Increment * i;
        //
        // // 3) 각 Spawn 모듈 적용
        //
        // // 4) PostSpawn: 모듈간 보간, 이벤트 페이로드 적용 등 최종 처리
        // const float Interp = ComputeInterp(i);
        // PostSpawn(Particle, Interp, SpawnTime);
        // BaseParticles.Add(Particle);

        // // 테스트를 위한 static 변수들 (함수 내부에 선언하여 해당 함수 범위로 제한)
        // static FVector CurrentTestDirection = FVector(1.0f, 0.0f, 0.0f); // 초기 방향: +X (Right)
        // static int directionIndex = 0; // 현재 방향 인덱스
        //
        // // 6가지 방향 정의: Right, Left, Up, Down, Forward, Backward
        // const FVector directions[] = {
        //     FVector(1.0f, 0.0f, 0.0f),  // Right (+X)
        //     FVector(-1.0f, 0.0f, 0.0f), // Left (-X)
        //     FVector(0.0f, 1.0f, 0.0f),  // Up (+Y)
        //     FVector(0.0f, -1.0f, 0.0f), // Down (-Y)
        //     FVector(0.0f, 0.0f, 1.0f),  // Forward (+Z)
        //     FVector(0.0f, 0.0f, -1.0f)  // Backward (-Z)
        // };
        // const int numDirections = sizeof(directions) / sizeof(directions[0]);
        // const float testVelocityMagnitude = 1.0f; // 테스트용 속도 크기 (원하는 값으로 조절)
        // // 테스트용 방향 벡터 설정
        // CurrentTestDirection = directions[directionIndex];
        // FVector TestParticleVelocity = CurrentTestDirection * testVelocityMagnitude;
        // //FVector TestParticleVelocity = FVector(1.0f, 0.0f, 0.0f); 
        // // 다음 방향으로 업데이트 (순환)
        // directionIndex = (directionIndex + 1) % numDirections;
        
        // --- 1) 슬롯 확보 ---
        // 단순히 ActiveParticles를 인덱스로 쓰되, 최대까지 체크
        const int32 NewIndex = (ActiveParticles < MaxActiveParticles)
            ? ActiveParticles
            : (MaxActiveParticles - 1);  // 꽉 찼을 때 마지막 슬롯 재사용
        // 파티클 데이터 블록에서 해당 슬롯의 위치 계산
        uint8* SlotPtr = ParticleData + NewIndex * ParticleStride;
        // FBaseParticle 구조체 포인터 얻기
        FBaseParticle* Particle = reinterpret_cast<FBaseParticle*>(SlotPtr);
        // 활성 인덱스 배열에도 기록
        ParticleIndices[ActiveParticles++] = static_cast<uint16>(NewIndex);
        

        // --- 2) PreSpawn: 기본 속성 초기화 ---
        // (위에서 언급한 PreSpawn 함수: RequiredModule 기반 초기화)
        PreSpawn(*Particle, InitialLocation, InitialVelocity);
        //PreSpawn(*Particle, InitialLocation, TestParticleVelocity);

        // --- 3) 각 Spawn 모듈 적용 ---
        const float SpawnTime = StartTime + Increment * i;
        for (auto* SpawnModule : SpawnModules)
        {
            SpawnModule->Spawn(this,
                /*Offset=*/NewIndex * ParticleStride,
                SpawnTime,
                Particle);
        }

        // --- 4) PostSpawn: 보간 및 이벤트 페이로드 ---
        const float Interp = ComputeInterp(i);
        PostSpawn(Particle, Interp, SpawnTime);
    }
}

void FParticleEmitterInstance::PreSpawn(FBaseParticle& Particle, const FVector& InitLocation, const FVector& InitVelocity)
{
    // 위치
    Particle.Location        = InitLocation;
    Particle.OldLocation     = InitLocation;

    // 속도
    Particle.BaseVelocity    = InitVelocity;
    Particle.Velocity        = InitVelocity;

    // 수명
     float Lifetime    =  1.0f; //RequiredModule
    //     ? RequiredModule->Lifetime.GetValue(EmitterTime, Component)
    //     : 1.0f;
    Particle.OneOverMaxLifetime = (Lifetime > 0.0f) 
        ? (1.0f / Lifetime) 
        : 1.0f;
    Particle.RelativeTime    = 0.0f;

    // 회전

    Particle.Rotation        = 0.0f;
    Particle.BaseRotationRate = 0.0f;
    
    Particle.RotationRate    = Particle.BaseRotationRate;

    // 크기
    
    Particle.BaseSize       = FVector(1.0f);
    Particle.Size            = Particle.BaseSize;

    // 색상
    Particle.BaseColor      = FLinearColor::White;
    
    Particle.Color           = Particle.BaseColor;

    // 기타 플래그 및 플레이스홀더
    Particle.Flags           = 0;
    Particle.Placeholder0    = 0.0f;
    Particle.Placeholder1    = 0.0f;
}

void FParticleEmitterInstance::PostSpawn(FBaseParticle* Particle, float InterpolationPercentage, float SpawnTime)
{
}


void FParticleEmitterInstance::KillParticle(int32 Index)
{
    // 1) 유효성 체크
    if(!(Index >= 0 && Index < ActiveParticles)) return;

    // 2) 마지막 활성 슬롯 인덱스 가져오기
    int32 LastArrayIndex = ActiveParticles - 1;
    uint16 LastSlotIndex = ParticleIndices[LastArrayIndex];
    

    // 3) 제거할 위치에 마지막 슬롯 복사
    //    → O(1)로 중간 삭제
    ParticleIndices[Index] = LastSlotIndex;

    // 4) 활성 개수 하나 줄이기
    --ActiveParticles;
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
    // UParticleLODLevel* LODLevel = SpriteTemplate->GetCurrentLODLevel(this);
    // if ((LODLevel == NULL) || (LODLevel->bEnabled == false))
    // {
    //     return false;
    // }
    // Must be filled in by implementation in derived class
    OutData.eEmitterType = DET_Unknown;
    
    OutData.ComponentLocalToWorld = Component->GetWorldMatrix();

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

    
    // — 메모리 블록(ParticleData + ParticleIndices) 복사
    int32 DataBytes   = MaxActiveParticles * ParticleStride;
    int32 IndexCount  = MaxActiveParticles;
    OutData.DataContainer.Alloc(DataBytes, IndexCount);
    std::memcpy(OutData.DataContainer.ParticleData,
                ParticleData,
                DataBytes);
    std::memcpy(OutData.DataContainer.ParticleIndices,
                ParticleIndices,
                sizeof(uint16) * ActiveParticles);

    
    //언리얼에서도 모든 emitter가 스프라이트인 걸로 가정하고 한다.
    FDynamicSpriteEmitterReplayDataBase* NewReplayData =
    static_cast< FDynamicSpriteEmitterReplayDataBase* >( &OutData );

    NewReplayData->RequiredModule = RequiredModule;
    NewReplayData->Material = RequiredModule->SpriteTexture;	// 파생된 구현에서 설정해야 합니다.
    
    NewReplayData->MaxDrawCount =ActiveParticles;
    
    NewReplayData->PivotOffset =   (PivotOffset);
    NewReplayData->bUseLocalSpace = false;

    OutData.ComponentLocalToWorld = Component->GetWorldMatrix();
    OutData.ParticleEmitterRenderData = GetRenderData();

    return true;
}

UMaterial* FParticleEmitterInstance::GetCurrentMaterial()
{
    return RequiredModule->SpriteTexture;
}
