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

    EDistributionType GetDistributionType() 
    {
        return DistributionType;
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

    FSimpleVectorDistribution(const FVector StartVector, const FVector EndVector, EDistributionType Type) 
    {
        DistX = FSimpleFloatDistribution(StartVector.X, EndVector.X, Type);
        DistY = FSimpleFloatDistribution(StartVector.Y, EndVector.Y, Type);
        DistZ = FSimpleFloatDistribution(StartVector.Z, EndVector.Z, Type);
    }

    EDistributionType GetDistributionType()
    {
        // 현재 로직 상 DistributionType이 바뀌지 않으므로
        return DistX.DistributionType;
    }

    void GetDistributionVector(FVector& OutConstantVector) 
    {
        OutConstantVector.X = DistX.Constant;
        OutConstantVector.Y = DistY.Constant;
        OutConstantVector.Z = DistZ.Constant;
    }

    void GetDistributionVectors(FVector& OutStartVector, FVector& OutEndVector) 
    {
        OutStartVector.X = DistX.Min;
        OutStartVector.Y = DistY.Min;
        OutStartVector.Z = DistZ.Min;

        OutEndVector.X = DistX.Max;
        OutEndVector.Y = DistY.Max;
        OutEndVector.Z = DistZ.Max;
    }

    void SetDistributionVector(FVector& InConstantVector) 
    {
        DistX.Constant = InConstantVector.X;
        DistY.Constant = InConstantVector.Y;
        DistZ.Constant = InConstantVector.Z;
    }

    void SetDistributionVectors(FVector& InStartVector, FVector& InEndVector) 
    {
        DistX.Min = InStartVector.X;
        DistY.Min = InStartVector.Y;
        DistZ.Min = InStartVector.Z;

        DistX.Max = InEndVector.X;
        DistY.Max = InEndVector.Y;
        DistZ.Max = InEndVector.Z;
    }

    FVector GetValue(float t) const 
    {
        return FVector(DistX.GetValue(t), DistY.GetValue(t), DistZ.GetValue(t));
    }

    void GetOutRange(float& MinSize, float& MaxSize)
    {
        float min = DistX.Min;
        if (min > DistX.Max) 
            min = DistX.Max;
        if (min > DistY.Min)
            min = DistY.Min;
        if (min > DistY.Max)
            min = DistY.Max;
        if (min > DistZ.Min)
            min = DistZ.Min;
        if (min > DistZ.Max)
            min = DistZ.Max;

        float max = DistX.Min;
        if (max < DistX.Max)
            max = DistX.Max;
        if (max < DistY.Min)
            max = DistY.Min;
        if (max < DistY.Max)
            max = DistY.Max;
        if (max < DistZ.Min)
            max = DistZ.Min;
        if (max < DistZ.Max)
            max = DistZ.Max;

        MinSize = min;
        MaxSize = max;
    }
};