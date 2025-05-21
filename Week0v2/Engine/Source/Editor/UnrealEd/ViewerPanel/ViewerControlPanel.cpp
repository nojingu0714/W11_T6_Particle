#include "ViewerControlPanel.h"

#include "Engine/AssetManager.h"
#include "LevelEditor/SLevelEditor.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include "Particles/ParticleSystemComponent.h"


class FDrawer;

void ViewerControlPanel::Render()
{
    /* Pre Setup */
    // Menu bar

    const ImGuiIO& IO = ImGui::GetIO();
    ImFont* IconFont = IO.Fonts->Fonts.size() == 1 ? IO.FontDefault : IO.Fonts->Fonts[FEATHER_FONT];
    constexpr ImVec2 IconSize = ImVec2(32, 32);
    ImVec2 WinSize = ImVec2(Width, Height);
    
    ImGui::SetNextWindowPos(ImVec2(0, 20));

    ImGui::SetNextWindowSize(ImVec2(Width, 50));
    /* Panel Flags */
    ImGuiWindowFlags PanelFlags =ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;
    UE_LOG(LogLevel::Error, "ViewerControlPanel::Render() called");
    /* Render Start */
    if (ImGui::Begin("Control Panel", nullptr, PanelFlags))
    {
        ImGui::PushFont(IconFont);
        
        if (ImGui::Button("\ue9d6",IconSize))
        {
            UE_LOG(LogLevel::Warning, "Save Particle");
            ParticleSystemComponent->InitParticles();
        }
        ImGui::SameLine();
        if (ImGui::Button("\ue950",IconSize))
        {
            UE_LOG(LogLevel::Warning, "Content Drawer");
        }
        ImGui::PopFont();
        
        ImGui::End();
    }
}

void ViewerControlPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = ClientRect.right - ClientRect.left;
    Height = ClientRect.bottom - ClientRect.top;
}

void ViewerControlPanel::SetParticleSystemComponent(UParticleSystemComponent* InParticleSystemComponent)
{
    ParticleSystemComponent = InParticleSystemComponent;
}


