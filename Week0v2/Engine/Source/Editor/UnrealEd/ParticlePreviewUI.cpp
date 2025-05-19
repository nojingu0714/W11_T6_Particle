#include "ParticlePreviewUI.h"

#include "PropertyEditor/OutlinerEditorPanel.h"
#include "PropertyEditor/PreviewControlEditorPanel.h"
#include "ViewerPanel/ViewerControlPanel.h"
#include "ViewerPanel/ViewerMenuPanel.h"

void FParticlePreviewUI::Initialize(SLevelEditor* LevelEditor, float Width, float Height)
{
    auto ControlPanel = std::make_shared<ViewerControlPanel>();
    Panels["ViewerControlPanel"] = ControlPanel;
    
    auto MenuPanel = std::make_shared<ViewerMenuPanel>();
    Panels["ViewerMenuPanel"] = MenuPanel;
}

void FParticlePreviewUI::Render() const
{
    for (const auto& Panel : Panels)
    {
        if(Panel.Value->bIsVisible)
        {
            Panel.Value->Render();
        }
    }
}

void FParticlePreviewUI::OnResize(HWND hWnd) const
{
    for (auto& Panel : Panels)
    {
        Panel.Value->OnResize(hWnd);
    }
}

void FParticlePreviewUI::SetWorld(UWorld* InWorld)
{
    World = InWorld;
    for (auto& [_, Panel] : Panels)
    {
        Panel->SetWorld(World);
    } 
}

void FParticlePreviewUI::AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel)
{
    Panels[PanelId] = EditorPanel;
}

std::shared_ptr<UEditorPanel> FParticlePreviewUI::GetEditorPanel(const FString& PanelId)
{
    return Panels[PanelId];
}
