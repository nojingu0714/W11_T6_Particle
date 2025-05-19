#pragma once
#include "Actors/ADodge.h"

class UParticleModule;

class ParticleDetailPanel
{
public:
    void Render(UParticleModule* InSkeletalMesh);
    void OnResize(HWND hWnd);

private:
    float Width = 800, Height = 600;
};
