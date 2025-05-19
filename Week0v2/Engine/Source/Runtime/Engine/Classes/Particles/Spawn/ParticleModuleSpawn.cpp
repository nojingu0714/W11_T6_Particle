#include "ParticleModuleSpawn.h"
#include "Engine/ParticleEmitterInstance.h"

void UParticleModuleSpawn::InitializeDefaults()
{
    Rate = FSimpleFloatDistribution(5.0f);          //  기본적인 초당 스폰 파티클 갯수
    RateScale = FSimpleFloatDistribution(1.0f);     // Rate에 곱해져서 상대적인 비율을 반영
    BurstScale = FSimpleFloatDistribution(1.0f);
}

bool UParticleModuleSpawn::GetSpawnAmount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover, float DeltaTime, int32& Number, float& OutRate)
{
    // Rate와 RateScale으로 현재 Spawn해야하는 Particle 갯수 (float)
    // 지난 프레임에 남아있던 Particle 조각에 합해서
    // 이번에 Spawn해야 하는 Particle 갯수 (int)를 구하고 남은 부분은 따로 저장
    float CurRateValue = DeltaTime * Rate.GetValue(Owner->EmitterTime) * RateScale.GetValue(Owner->EmitterTime);
    float CurParticle = CurRateValue + LeftOverParticle;
    Number = static_cast<int>(CurParticle);
    LeftOverParticle = CurParticle - Number;

    return false;
}

float UParticleModuleSpawn::GetMaximumSpawnRate()
{
    float MinSpawn, MaxSpawn;
    float MinScale, MaxScale;

    MinSpawn = Rate.Min;
    MaxSpawn = Rate.Max;

    MinScale = RateScale.Min;
    MaxScale = RateScale.Max;

    return (MaxSpawn * MaxScale);
}

float UParticleModuleSpawn::GetEstimatedSpawnRate()
{
    float MinSpawn, MaxSpawn;
    float MinScale, MaxScale;

    MinSpawn = Rate.Min;
    MaxSpawn = Rate.Max;

    MinScale = RateScale.Min;
    MaxScale = RateScale.Max;

    // 언리얼에서 성능 예측을 위해 사용하는 값이라고 함
    // Curve를 사용하지 않으므로 최대값을 보내주는 것으로 처리

    return (MaxSpawn * MaxScale);
}

int32 UParticleModuleSpawn::GetMaximumBurstCount()
{
    int32 MaxBurst = 0;
    for (int32 BurstIndex = 0; BurstIndex < BurstList.Num(); BurstIndex++)
    {
        MaxBurst += BurstList[BurstIndex].Count;
    }
    return MaxBurst;
}

float UParticleModuleSpawn::GetGlobalRateScale() const
{
    // TODO GlobalSpawnRate를 사용하는 경우 추가 처리하기

    //bApplyGlobalSpawnRateScale

    return 0.0f;
}

void UParticleModuleSpawn::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle& Particle)
{
}
