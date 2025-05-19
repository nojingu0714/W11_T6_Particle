#include "ParticleEmitterInstance.h"

#include "Particles/ParticleEmitter.h"
#include "ParticleHelper.h"
#include "Engine/AssetManager.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleModuleRequired.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/Spawn/ParticleModuleSpawn.h"
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
    }

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
        float Rate = SpawnModule->Rate.GetValue(0.0f); // 만약 t 값을 활용해야 하는경우 수정 필요 
        int32 Count = Rate;
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

    UE_LOG(LogLevel::Warning, "Particles : %d", BaseParticles.Num());
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

FDynamicEmitterReplayDataBase* FParticleEmitterInstance::GetReplayData()
{
    return nullptr;
}

bool FParticleEmitterInstance::FillReplayData(FDynamicEmitterReplayDataBase& OutData)
{
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
	UParticleLODLevel* LODLevel = SpriteTemplate->LODLevels[0];
	if ((LODLevel == NULL) || (LODLevel->bEnabled == false))
	{
		return false;
	}

	// Make sure we will not be allocating enough memory
        if (!(MaxActiveParticles >= ActiveParticles)) return false;

	// Must be filled in by implementation in derived class
	OutData.eEmitterType = DET_Unknown;

	OutData.ActiveParticleCount = ActiveParticles;
	OutData.ParticleStride = ParticleStride;
	OutData.SortMode = SortMode;

	// Take scale into account
	OutData.Scale = FVector::OneVector;
	if (Component)
	{
		OutData.Scale = FVector(Component->GetWorldScale());
	}

	int32 ParticleMemSize = MaxActiveParticles * ParticleStride;

	// Allocate particle memory

	OutData.DataContainer.Alloc(ParticleMemSize, MaxActiveParticles);
	// INC_DWORD_STAT_BY(STAT_RTParticleData, OutData.DataContainer.MemBlockSize);
	//
	// FMemory::BigBlockMemcpy(OutData.DataContainer.ParticleData, ParticleData, ParticleMemSize);
	// FMemory::Memcpy(OutData.DataContainer.ParticleIndices, ParticleIndices, OutData.DataContainer.ParticleIndicesNumShorts * sizeof(uint16));

	// All particle emitter types derived from sprite emitters, so we can fill that data in here too!
	{
		FDynamicSpriteEmitterReplayDataBase* NewReplayData =
			static_cast< FDynamicSpriteEmitterReplayDataBase* >( &OutData );

		NewReplayData->RequiredModule = LODLevel->RequiredModule;
//		NewReplayData->Material = nullptr;	// Must be set by derived implementation
    	    
        //원레 하위 인스턴스 클래스에서 정하는거라는데 그냥 여기서 정해버리기 
    	NewReplayData->Material = GetCurrentMaterial();
    	    

    	    
    	    //  ################## 이하 언리얼 코드인데 머하는지 모르겠어서 일단 보류############################
		// NewReplayData->InvDeltaSeconds = (LastDeltaTime > UE_KINDA_SMALL_NUMBER) ? (1.0f / LastDeltaTime) : 0.0f;
		// NewReplayData->LWCTile = ((Component == nullptr) || LODLevel->RequiredModule->bUseLocalSpace) ? FVector3f::Zero() : Component->GetLWCTile();
		//
		// NewReplayData->MaxDrawCount =
		// 	(LODLevel->RequiredModule->bUseMaxDrawCount == true) ? LODLevel->RequiredModule->MaxDrawCount : -1;
		// NewReplayData->ScreenAlignment	= LODLevel->RequiredModule->ScreenAlignment;
		// NewReplayData->bUseLocalSpace = LODLevel->RequiredModule->bUseLocalSpace;
		// NewReplayData->EmitterRenderMode = SpriteTemplate->EmitterRenderMode;
		// NewReplayData->DynamicParameterDataOffset = DynamicParameterDataOffset;
		// NewReplayData->LightDataOffset = LightDataOffset;
		// NewReplayData->LightVolumetricScatteringIntensity = LightVolumetricScatteringIntensity;
		// NewReplayData->CameraPayloadOffset = CameraPayloadOffset;
		//
		// NewReplayData->SubUVDataOffset = SubUVDataOffset;
		// NewReplayData->SubImages_Horizontal = LODLevel->RequiredModule->SubImages_Horizontal;
		// NewReplayData->SubImages_Vertical = LODLevel->RequiredModule->SubImages_Vertical;
		//
		// NewReplayData->MacroUVOverride.bOverride = LODLevel->RequiredModule->bOverrideSystemMacroUV;
		// NewReplayData->MacroUVOverride.Radius = LODLevel->RequiredModule->MacroUVRadius;
		// NewReplayData->MacroUVOverride.Position = FVector3f(LODLevel->RequiredModule->MacroUVPosition);
  //       
		// NewReplayData->bLockAxis = false;
		// if (bAxisLockEnabled == true)
		// {
		// 	NewReplayData->LockAxisFlag = LockAxisFlags;
		// 	if (LockAxisFlags != EPAL_NONE)
		// 	{
		// 		NewReplayData->bLockAxis = true;
		// 	}
		// }
		//
		// // If there are orbit modules, add the orbit module data
		// if (LODLevel->OrbitModules.Num() > 0)
		// {
		// 	UParticleLODLevel* HighestLODLevel = SpriteTemplate->LODLevels[0];
		// 	UParticleModuleOrbit* LastOrbit = HighestLODLevel->OrbitModules[LODLevel->OrbitModules.Num() - 1];
		// 	check(LastOrbit);
		//
		// 	uint32* LastOrbitOffset = SpriteTemplate->ModuleOffsetMap.Find(LastOrbit);
		// 	NewReplayData->OrbitModuleOffset = *LastOrbitOffset;
		// }
		//
		// NewReplayData->EmitterNormalsMode = LODLevel->RequiredModule->EmitterNormalsMode;
		// NewReplayData->NormalsSphereCenter = (FVector3f)LODLevel->RequiredModule->NormalsSphereCenter;
		// NewReplayData->NormalsCylinderDirection = (FVector3f)LODLevel->RequiredModule->NormalsCylinderDirection;
		//
		// NewReplayData->PivotOffset = FVector2f(PivotOffset);
		//
		// NewReplayData->bUseVelocityForMotionBlur = LODLevel->RequiredModule->ShouldUseVelocityForMotionBlur();
		// NewReplayData->bRemoveHMDRoll = LODLevel->RequiredModule->bRemoveHMDRoll;
		// NewReplayData->MinFacingCameraBlendDistance = LODLevel->RequiredModule->MinFacingCameraBlendDistance;
		// NewReplayData->MaxFacingCameraBlendDistance = LODLevel->RequiredModule->MaxFacingCameraBlendDistance;
	}
}

UMaterial* FParticleEmitterInstance::GetCurrentMaterial()
{
    return RequiredModule->SpriteTexture;
}
