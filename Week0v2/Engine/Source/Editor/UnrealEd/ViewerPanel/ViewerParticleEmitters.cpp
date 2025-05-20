#include "ViewerParticleEmitters.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleModule.h"

#include "ImGUI/imgui.h"

void ViewerParticleEmitters::SetParticleSystemComponent(UParticleSystemComponent* InParticleSystemComponent)
{
    ParticleSystemComponent = InParticleSystemComponent;
}

void ViewerParticleEmitters::Render()
{
    const ImGuiIO& IO = ImGui::GetIO();
    ImFont* IconFont = IO.Fonts->Fonts.size() == 1 ? IO.FontDefault : IO.Fonts->Fonts[FEATHER_FONT];
    constexpr ImVec2 IconSize = ImVec2(32, 32);
    ImVec2 WinSize = ImVec2(Width, Height);
    
    ImGui::SetNextWindowPos(ImVec2(Width*0.3, 70));

    ImGui::SetNextWindowSize(ImVec2(Width * 0.7, Height * 0.6 - 70));

    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;
    //UE_LOG(LogLevel::Error, "ViewerControlPanel::Render() called");

    if (ImGui::Begin("Emitters", nullptr, PanelFlags))
    {
        if (ImGui::Begin("Emitters", nullptr, PanelFlags))
        {
            if (ParticleSystemComponent && ParticleSystemComponent->Template)
            {
                const TArray<UParticleEmitter*>& Emitters = ParticleSystemComponent->Template->Emitters;
                int EmitterCount = Emitters.Num();
                if (EmitterCount > 0)
                {
                    // Create one column per emitter
                    ImGui::Columns(EmitterCount, "EmitterColumns", true);

                    for (int E = 0; E < EmitterCount; ++E)
                    {
                        UParticleEmitter* Emitter = Emitters[E];
                        // Header for this emitter
                        ImGui::Text("Emitter %d", E);
                        ImGui::Separator();

                        // List modules in LOD0 in their defined order
                        UParticleLODLevel* LOD = Emitter->LODLevels[0];
                        if (LOD)
                        {
                            for (int M = 0; M < LOD->Modules.Num(); ++M)
                            {
                                UParticleModule* Module = LOD->Modules[M];

                                // 클래스 이름을 FString으로 가져오기
                                FString ClassName = Module->GetClass()->GetName();
                                // 또는, 로컬라이즈된 이름: GetDisplayNameText().ToString()

                                // 고유 ID 생성: 보이는 레이블은 ClassName, 내부 ID는 인덱스 조합
                                FString Id = FString::Printf(TEXT("%s##%d_%d"), *ClassName, E, M);

                                if (ImGui::Selectable(*Id, SelectedModule == Module))
                                {
                                    SelectedModule = Module;
                                }
                            }
                        }

                        ImGui::NextColumn();
                    }

                    ImGui::Columns(1);
                }
            }
            ImGui::End();
        }
        ImGui::End();
    }
    DetailPanel.Render(SelectedModule);
}

void ViewerParticleEmitters::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = ClientRect.right - ClientRect.left;
    Height = ClientRect.bottom - ClientRect.top;
    DetailPanel.OnResize(hWnd);
}
