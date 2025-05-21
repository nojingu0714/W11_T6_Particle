#pragma once
#include "Particles/ParticleModule.h"
#include "Define.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"

class UMaterial;

class UParticleModuleSnow : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSnow, UParticleModule)

private:



public:
    FSimpleFloatDistribution SpawnXMin;
    FSimpleFloatDistribution SpawnXMax;

    // Y축 스폰 범위
    FSimpleFloatDistribution SpawnYMin;
    FSimpleFloatDistribution SpawnYMax;

    // Z축 스폰 높이 범위
    FSimpleFloatDistribution SpawnZMin;
    FSimpleFloatDistribution SpawnZMax;

    FSimpleFloatDistribution GravityScale = 1.0f;  // Z축 낙하 속도 스케일링 값

    FSimpleFloatDistribution SwayFrequency = 0.5f;  // 흔들림 주기 (값이 클수록 빠르게 흔들림)
    FSimpleFloatDistribution SwayMinAmount = 0.3f;  // 최소 흔들림 강도
    FSimpleFloatDistribution SwayMaxAmount = 0.5f;  // 최대 흔들림 강도
   
    FSimpleFloatDistribution RotationSpeedMin = 0.2f;  // 최소 회전 속도
    FSimpleFloatDistribution RotationSpeedMax = 0.5f;  // 최대 회전 속도
    FSimpleFloatDistribution RotationDirectionBias = 0.0f;  // 회전 방향 경향 (0: 랜덤, 양수: 시계방향 선호, 음수: 반시계방향 선호)


    UParticleModuleSnow();
    ~UParticleModuleSnow() = default;

    void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
};
