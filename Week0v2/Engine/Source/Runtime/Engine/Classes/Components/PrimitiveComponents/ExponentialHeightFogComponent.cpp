﻿#include "ExponentialHeightFogComponent.h"

UExponentialHeightFogComponent::UExponentialHeightFogComponent()
    : Super()
{
    FogDensity = 0.2f;
    FogHeightFalloff = 0.02f;
    StartDistance = 0.0f;
    FogMaxOpacity = 1.0f;
    FogCutOffDistance = 0.0f;
    FogInscatteringColor = FLinearColor::Black;
}

UExponentialHeightFogComponent::UExponentialHeightFogComponent(const UExponentialHeightFogComponent& Other)
    : Super(Other)
    , FogDensity(Other.FogDensity)
    , FogHeightFalloff(Other.FogHeightFalloff)
    , StartDistance(Other.StartDistance)
    , FogMaxOpacity(Other.FogMaxOpacity)
    , FogCutOffDistance(Other.FogCutOffDistance)
    , FogInscatteringColor(Other.FogInscatteringColor)
{
}

void UExponentialHeightFogComponent::SetFogDensity(float Density)
{
    FogDensity = Density;
}

void UExponentialHeightFogComponent::SetFogHeightFalloff(float HeightFalloff)
{
    FogHeightFalloff = HeightFalloff;
}

void UExponentialHeightFogComponent::SetFogMaxOpacity(float MaxOpacity)
{
    FogMaxOpacity = MaxOpacity;
}

void UExponentialHeightFogComponent::SetCutOffDistance(float CutOffDistance)
{
    FogCutOffDistance = CutOffDistance;
}

void UExponentialHeightFogComponent::SetStartDistance(float InStartDistance)
{
    StartDistance = InStartDistance;
}

void UExponentialHeightFogComponent::SetFogInscatteringColor(FLinearColor Color)
{
    FogInscatteringColor = Color;
}

