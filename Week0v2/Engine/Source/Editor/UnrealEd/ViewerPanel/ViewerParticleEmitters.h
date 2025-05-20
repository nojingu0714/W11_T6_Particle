#pragma once
#include "ParticleDetailPanel.h"
#include "UnrealEd/EditorPanel.h"
#include "Particles/ParticleSystemComponent.h"

class ViewerParticleEmitters : public UEditorPanel
{
public:
    void SetParticleSystemComponent(UParticleSystemComponent* InParticleSystemComponent);

    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

    float Width = 300, Height = 100;

    ParticleDetailPanel DetailPanel;
    UParticleSystemComponent* ParticleSystemComponent;
    UParticleModule* SelectedModule;
};
