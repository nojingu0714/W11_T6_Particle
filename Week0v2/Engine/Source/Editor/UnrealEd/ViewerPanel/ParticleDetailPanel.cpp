#include "ParticleDetailPanel.h"

#include "ImGUI/imgui.h"

void ParticleDetailPanel::Render(UParticleModule* InSkeletalMesh)
{
    ImVec2 WinSize = ImVec2(Width, Height);
    
    ImGui::SetNextWindowPos(ImVec2(0, Height * 0.5f));

    ImGui::SetNextWindowSize(ImVec2(WinSize.x * 0.3f, Height * 0.5f));
    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;

    if (ImGui::Begin("Details", nullptr, PanelFlags))
    {

    }
    ImGui::End();
}

void ParticleDetailPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = ClientRect.right - ClientRect.left;
    Height = ClientRect.bottom - ClientRect.top;
}
