#include "ParticleCurvePanel.h"

#include "ImGUI/imgui.h"

void ParticleCurvePanel::Render()
{
    ImVec2 WinSize = ImVec2(Width, Height);
    
    ImGui::SetNextWindowPos(ImVec2(WinSize.x * 0.3f, Height * 0.6f));

    ImGui::SetNextWindowSize(ImVec2(WinSize.x * 0.7f, Height * 0.4f));
    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;

    if (ImGui::Begin("Curve Editor", nullptr, PanelFlags))
    {

    }
    ImGui::End();
}

void ParticleCurvePanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = ClientRect.right - ClientRect.left;
    Height = ClientRect.bottom - ClientRect.top;
}
