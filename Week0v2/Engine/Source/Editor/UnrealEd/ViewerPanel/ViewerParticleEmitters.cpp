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
#include "ParticleEmitterInstance.h"

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
        if (ParticleSystemComponent && ParticleSystemComponent->Template)
        {
            const TArray<UParticleEmitter*>& Emitters = ParticleSystemComponent->Template->Emitters;
            int EmitterCount = Emitters.Num();

            // ✅ Emitter 추가 버튼 (가장 위에 따로)
            if (ImGui::Button("Emitter 추가"))
            {
                UParticleEmitter* NewEmitter = FObjectFactory::ConstructObject<UParticleEmitter>(nullptr);
                UParticleLODLevel* NewLOD = FObjectFactory::ConstructObject<UParticleLODLevel>(nullptr);
                NewEmitter->LODLevels.Add(NewLOD);
                ParticleSystemComponent->Template->Emitters.Add(NewEmitter);
            }

            ImGui::Separator(); // Emitter UI들과 구분


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
                            // 고유 ID 생성: 보이는 레이블은 ClassName, 내부 ID는 인덱스 조합
                            FString Id = FString::Printf(TEXT("%s##%d_%d"), *ClassName, E, M);
                            if (ImGui::Selectable(*Id, SelectedModule == Module))
                            {
                                SelectedModule = Module;
                            }
                        }

                        // 모듈 추가 버튼 및 팝업 메뉴
                        ImGui::Separator();
                        FString AddButtonId = FString::Printf(TEXT("Add Module##%d"), E);
                        if (ImGui::Button(*AddButtonId))
                        {
                            ImGui::OpenPopup(*FString::Printf(TEXT("ModuleTypesPopup##%d"), E));
                        }

                        // 모듈 유형 선택 팝업
                        if (ImGui::BeginPopup(*FString::Printf(TEXT("ModuleTypesPopup##%d"), E)))
                        {
                            // 기본 모듈 유형들
                            if (ImGui::MenuItem("UParticleModuleLifetime"))
                            {
                                // 새 모듈 생성 및 추가
                                UParticleModuleLifetime* NewModule = FObjectFactory::ConstructObject<UParticleModuleLifetime>(
                                   nullptr);
                                LOD->Modules.Add(NewModule);
                                SelectedModule = NewModule;
                            }

                            if (ImGui::MenuItem("UParticleModuleSize"))
                            {
                                UParticleModuleSize* NewModule = FObjectFactory::ConstructObject<UParticleModuleSize>(
                                    nullptr);
                                LOD->Modules.Add(NewModule);
                                SelectedModule = NewModule;
                            }

                            if (ImGui::MenuItem("UParticleModuleColor"))
                            {
                                UParticleModuleColor* NewModule = FObjectFactory::ConstructObject<UParticleModuleColor>(
                                    nullptr);
                                LOD->Modules.Add(NewModule);
                                SelectedModule = NewModule;
                            }

                            if (ImGui::MenuItem("UParticleModuleVelocity"))
                            {
                                UParticleModuleVelocity* NewModule = FObjectFactory::ConstructObject<UParticleModuleVelocity>(
                                    nullptr);
                                LOD->Modules.Add(NewModule);
                                SelectedModule = NewModule;
                            }

                            // 방금 만든 Snow 모듈 추가
                            if (ImGui::MenuItem("UParticleModuleSnow"))
                            {
                                UParticleModuleSnow* NewModule = FObjectFactory::ConstructObject<UParticleModuleSnow>(
                                    nullptr);
                                // 기본값 초기화
                              
                              //  ;
                                LOD->Modules.Add(NewModule);
                                SelectedModule = NewModule;


                            }

                            for (FParticleEmitterInstance* Instance : ParticleSystemComponent->EmitterInstances)
                            {
                                Instance->ClassifyModules();
                            }
                            ImGui::EndPopup();
                        }
                    }
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
            }
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
