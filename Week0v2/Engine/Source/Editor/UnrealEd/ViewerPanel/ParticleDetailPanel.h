#pragma once
#include "Actors/ADodge.h"
#include "Core/Math/Distribution.h"

class UParticleModuleRequired;
class UParticleModule;

class ParticleDetailPanel
{
public:
    void Render(UParticleModule* SelectedModule);
    void OnResize(HWND hWnd);

    void RenderFSimpleFloatDistribution(FSimpleFloatDistribution& RenderDistribution, float Tvalue, FString DistributionName);
    void RenderFSimpleVectorDistribution(FSimpleVectorDistribution& RenderDistribution, float Tvalue, FString DistributionName);
    void EditParticleModuleRequired(UParticleModuleRequired* Required);
private:
    float Width = 800, Height = 600;
};
