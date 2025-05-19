#pragma once
#include "GameFramework/Actor.h"
#include "ImGUI/imgui.h"
#include "UnrealEd/EditorPanel.h"

class ViewerControlPanel : public UEditorPanel
{
public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

private:
    float Width = 300, Height = 100;
    
};
