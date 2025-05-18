#pragma once

#include "Engine/Source/Runtime/Core/Math/Vector.h"
#include <algorithm>


enum class EDistributionType
{
    Constant,
    Unitform,
};

struct FSimpleFloatDistribution 
{
    EDistributionType DistributionType;
    float Constant = 0.0f;
    float Min = 0.0f, Max = 1.0f;

    FSimpleFloatDistribution(float InConstant = 0.0f)
        : DistributionType(EDistributionType::Constant), Constant(InConstant) { }

    FSimpleFloatDistribution(float InMin, float InMax)
        : DistributionType(EDistributionType::Unitform), Min(InMin), Max(InMax) { }

    float GetValue() const 
    {
        switch (DistributionType) 
        {
        case EDistributionType::Constant:
            return Constant;
            break;
        case EDistributionType::Unitform:
            return Min + (Max - Min) * Random01();
            break;
        default:
            return 0.0f;
            break;
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

    FVector GetValue() const 
    {
        return FVector(DistX.GetValue(), DistY.GetValue(), DistZ.GetValue());
    }
};