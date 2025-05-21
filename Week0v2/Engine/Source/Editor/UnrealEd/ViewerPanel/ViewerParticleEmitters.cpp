#include "ViewerParticleEmitters.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleModule.h"

#include "ImGUI/imgui.h"
#include <Particles/Lifetime/ParticleModuleLifetime.h>
#include <Particles/Size/ParticleModuleSize.h>
#include <Particles/Color/ParticleModuleColor.h>
#include <Particles/Velocity/ParticleModuleVelocity.h>
#include <Particles/Snow/ParticleModuleSnow.h>
#include "Particles/ParticleModuleRequired.h"
#include "Particles/Spawn/ParticleModuleSpawn.h"
#include "ParticleEmitterInstance.h"

void ViewerParticleEmitters::SetParticleSystemComponent(UParticleSystemComponent* InParticleSystemComponent)
{
    ParticleSystemComponent = InParticleSystemComponent;
}

void ViewerParticleEmitters::Render()
{
    //--- 창 설정 ---
    ImGui::SetNextWindowPos(ImVec2(Width * 0.3f, 70));
    ImGui::SetNextWindowSize(ImVec2(Width * 0.7f, Height * 0.6f - 70));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_HorizontalScrollbar;
    if (!ImGui::Begin("Emitters", nullptr, flags))
    {
        ImGui::End();
        return;
    }

    //--- Emitter 추가 버튼 ---
    if (ParticleSystemComponent && ParticleSystemComponent->Template)
    {
        if (ImGui::Button("Add Emitter"))
        {
            UParticleEmitter* newEmitter = FObjectFactory::ConstructObject<UParticleEmitter>(nullptr);
            UParticleLODLevel* newLOD = FObjectFactory::ConstructObject<UParticleLODLevel>(nullptr);
            newEmitter->LODLevels.Add(newLOD);

            auto* RequiredModule = FObjectFactory::ConstructObject<UParticleModuleRequired>(nullptr);
            newLOD->Modules.Add(RequiredModule);

            auto* SpawnModule = FObjectFactory::ConstructObject<UParticleModuleSpawn>(nullptr);
            newLOD->Modules.Add(SpawnModule);

            ParticleSystemComponent->Template->Emitters.Add(newEmitter);
        }
        ImGui::Separator();

        const auto& Emitters = ParticleSystemComponent->Template->Emitters;
        int emitterCount = Emitters.Num();
        if (emitterCount > 0)
        {
            //--- 테이블 시작 ---
            if (ImGui::BeginTable("EmitterTable", emitterCount,
                ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoBordersInBody))
            {
                // 헤더 행
                ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                for (int col = 0; col < emitterCount; ++col)
                {
                    ImGui::TableSetColumnIndex(col);
                    FString headerText = FString::Printf(TEXT("Emitter %d"), col);
                    ImGui::TableHeader(*headerText);
                }

                // 콘텐츠 행
                ImGui::TableNextRow();
                for (int col = 0; col < emitterCount; ++col)
                {
                    ImGui::TableSetColumnIndex(col);
                    ImGui::BeginGroup();

                    UParticleLODLevel* lod = Emitters[col]->LODLevels[0];
                    if (lod)
                    {
                        // 모듈 리스트
                        for (int m = 0; m < lod->Modules.Num(); ++m)
                        {
                            UParticleModule* module = lod->Modules[m];
                            FString className = module->GetClass()->GetName();
                            FString selectableId = FString::Printf(TEXT("%s##%d_%d"),
                                *className, col, m);
                            if (ImGui::Selectable(*selectableId,
                                SelectedModule == module))
                            {
                                SelectedModule = module;
                            }
                        }

                        ImGui::Separator();

                        // Add Module 버튼 & 팝업
                        FString addBtnId = FString::Printf(TEXT("Add Module##%d"), col);
                        if (ImGui::Button(*addBtnId))
                        {
                            FString popupId = FString::Printf(TEXT("ModulePopup##%d"), col);
                            ImGui::OpenPopup(*popupId);
                        }

                        FString popupId = FString::Printf(TEXT("ModulePopup##%d"), col);
                        if (ImGui::BeginPopup(*popupId))
                        {
                            if (ImGui::MenuItem("UParticleModuleLifetime"))
                            {
                                auto* nm = FObjectFactory::ConstructObject<UParticleModuleLifetime>(nullptr);
                                lod->Modules.Add(nm);
                                SelectedModule = nm;
                            }
                            if (ImGui::MenuItem("UParticleModuleSize"))
                            {
                                auto* nm = FObjectFactory::ConstructObject<UParticleModuleSize>(nullptr);
                                lod->Modules.Add(nm);
                                SelectedModule = nm;
                            }
                            if (ImGui::MenuItem("UParticleModuleColor"))
                            {
                                auto* nm = FObjectFactory::ConstructObject<UParticleModuleColor>(nullptr);
                                lod->Modules.Add(nm);
                                SelectedModule = nm;
                            }
                            if (ImGui::MenuItem("UParticleModuleVelocity"))
                            {
                                auto* nm = FObjectFactory::ConstructObject<UParticleModuleVelocity>(nullptr);
                                lod->Modules.Add(nm);
                                SelectedModule = nm;
                            }
                            if (ImGui::MenuItem("UParticleModuleSnow"))
                            {
                                auto* nm = FObjectFactory::ConstructObject<UParticleModuleSnow>(nullptr);
                                lod->Modules.Add(nm);
                                SelectedModule = nm;
                            }

                            // 모든 인스턴스 모듈 재분류
                            for (auto* inst : ParticleSystemComponent->EmitterInstances)
                                inst->ClassifyModules();

                            ImGui::EndPopup();
                        }
                    }

                    ImGui::EndGroup();
                }

                ImGui::EndTable();
            }
        }
    }

    ImGui::End();

    // 선택된 모듈 상세 패널
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
