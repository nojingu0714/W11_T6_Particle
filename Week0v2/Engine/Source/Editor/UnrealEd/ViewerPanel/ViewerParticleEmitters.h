#pragma once
#include "ParticleDetailPanel.h"
#include "UnrealEd/EditorPanel.h"

class ViewerParticleEmitters : public UEditorPanel
{
public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

    float Width = 300, Height = 100;

    ParticleDetailPanel DetailPanel;
};
