#include "ParticlePreviewUI.h"

#include "Define.h"

#include "PropertyEditor/OutlinerEditorPanel.h"
#include "PropertyEditor/PreviewControlEditorPanel.h"
#include "ViewerPanel/ParticleCurvePanel.h"
#include "ViewerPanel/ViewerControlPanel.h"
#include "ViewerPanel/ViewerMenuPanel.h"
#include "ViewerPanel/ViewerParticleEmitters.h"
#include "Engine/World.h"
#include "Engine/Source/Runtime/Launch/EditorEngine.h"
#include "Engine/Source/Runtime/CoreUObject/UObject/Casts.h"

extern UEngine* GEngine;

void FParticlePreviewUI::Initialize(SLevelEditor* LevelEditor, float Width, float Height)
{
    auto ControlPanel = std::make_shared<ViewerControlPanel>();
    Panels["ViewerControlPanel"] = ControlPanel;
    
    auto MenuPanel = std::make_shared<ViewerMenuPanel>();
    Panels["ViewerMenuPanel"] = MenuPanel;

    auto EmitterPanel  = std::make_shared<ViewerParticleEmitters>();
    Panels["ViewerParticleEmitters"] = EmitterPanel;

    auto CurvePanel = std::make_shared<ParticleCurvePanel>();
    Panels["ParticleCurvePanel"] = CurvePanel;
}

void FParticlePreviewUI::SetParticleSystemComponent(UParticleSystemComponent* InParticleSystemComponent)
{
    Panels["ViewerControlPanel"].get()->SetParticleSystemComponent(InParticleSystemComponent);
    Panels["ViewerMenuPanel"].get()->SetParticleSystemComponent(InParticleSystemComponent);
    Panels["ViewerParticleEmitters"].get()->SetParticleSystemComponent(InParticleSystemComponent);
    Panels["ParticleCurvePanel"].get()->SetParticleSystemComponent(InParticleSystemComponent);
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
