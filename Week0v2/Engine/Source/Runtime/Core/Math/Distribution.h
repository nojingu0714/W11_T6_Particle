#pragma once

#include "Engine/Source/Runtime/Core/Math/Vector.h"
#include <algorithm>


enum class EDistributionType
{
    Constant,
    Uniform,
    Linear,
    EaseInOut,
    SinWave,
};

struct FSimpleFloatDistribution 
{
    EDistributionType DistributionType;
    float Constant = 0.0f;
    float Min = 0.0f, Max = 1.0f;

    FSimpleFloatDistribution(float InConstant = 0.0f)
        : DistributionType(EDistributionType::Constant), Constant(InConstant) { }

    // Uniform, Linear, EaseInOut, SinWave 용 생성자
    FSimpleFloatDistribution(float InMin, float InMax, EDistributionType DistType = EDistributionType::Uniform)
        : DistributionType(DistType), Min(InMin), Max(InMax) {
    }

    float GetValue(float t) const
    {
        switch (DistributionType)
        {
        case EDistributionType::Constant:
            return Constant;

        case EDistributionType::Uniform:
            // 범위 내 랜덤
            return Min + (Max - Min) * Random01();

        case EDistributionType::Linear:
            // 선형 보간
            return Min + (Max - Min) * t;

        case EDistributionType::EaseInOut:
        {
            // Hermite easing: 3t^2 - 2t^3
            float s = t * t * (3.0f - 2.0f * t);
            return Min + (Max - Min) * s;
        }

        case EDistributionType::SinWave:
        {
            float sine = std::sin(PI * t);
            return Min + (Max - Min) * sine;
        }
        default:
            return 0.0f;
        }
    }

    static float Random01() 
    {
        return float(std::rand()) / float(RAND_MAX);
    }
};

struct FSimpleVectorDistribution 
{
    FSimpleFloatDistribution DistX, DistY, DistZ;

    FSimpleVectorDistribution() = default;

    FSimpleVectorDistribution(const FSimpleFloatDistribution& InX,
                              const FSimpleFloatDistribution& InY,
                              const FSimpleFloatDistribution& InZ)
        : DistX(InX), DistY(InY), DistZ(InZ) { }

    FVector GetValue(float t) const 
    {
        return FVector(DistX.GetValue(t), DistY.GetValue(t), DistZ.GetValue(t));
    }
};