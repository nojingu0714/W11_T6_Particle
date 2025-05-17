#include "ParticleModuleSpawn.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"

void UParticleModuleSpawn::InitializeDefaults()
{
    Rate = FSimpleFloatDistribution(20.0f);
    RateScale = FSimpleFloatDistribution(1.0f);
    BurstScale = FSimpleFloatDistribution(1.0f);
}

bool UParticleModuleSpawn::GetSpawnAmount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover, float DeltaTime, int32& Number, float& Rate)
{
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
