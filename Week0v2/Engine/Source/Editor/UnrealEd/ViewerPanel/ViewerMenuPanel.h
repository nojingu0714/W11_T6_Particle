#pragma once
#include "GameFramework/Actor.h"
#include "ImGUI/imgui.h"
#include "UnrealEd/EditorPanel.h"

class UParticleSystemComponent;

class ViewerMenuPanel : public UEditorPanel
{
public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

    void SetParticleSystemComponent(UParticleSystemComponent* InParticleSystemComponent);

    float Width = 300, Height = 100;

    UParticleSystemComponent* ParticleSystemComponent;
};
