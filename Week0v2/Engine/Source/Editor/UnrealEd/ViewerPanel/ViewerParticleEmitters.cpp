#include "ViewerParticleEmitters.h"

#include "ImGUI/imgui.h"

void ViewerParticleEmitters::Render()
{
    const ImGuiIO& IO = ImGui::GetIO();
    ImFont* IconFont = IO.Fonts->Fonts.size() == 1 ? IO.FontDefault : IO.Fonts->Fonts[FEATHER_FONT];
    constexpr ImVec2 IconSize = ImVec2(32, 32);
    ImVec2 WinSize = ImVec2(Width, Height);
    
    ImGui::SetNextWindowPos(ImVec2(Width*0.3, 70));

    ImGui::SetNextWindowSize(ImVec2(Width * 0.7, Height * 0.6 - 70));

    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;
    UE_LOG(LogLevel::Error, "ViewerControlPanel::Render() called");

    if (ImGui::Begin("Emitters", nullptr, PanelFlags))
    {

    }
    ImGui::End();
    DetailPanel.Render(nullptr);
}

void ViewerParticleEmitters::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = ClientRect.right - ClientRect.left;
    Height = ClientRect.bottom - ClientRect.top;
    DetailPanel.OnResize(hWnd);
}
