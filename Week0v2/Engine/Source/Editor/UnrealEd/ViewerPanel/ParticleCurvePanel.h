#pragma once
#include "Actors/ADodge.h"
#include "UnrealEd/EditorPanel.h"

class ParticleCurvePanel : public UEditorPanel
{
public:
    void Render();
    void OnResize(HWND hWnd);

private:
    float Width = 800, Height = 600;
};
