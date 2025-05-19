#include "ViewerControlPanel.h"

#include "Engine/AssetManager.h"
#include "LevelEditor/SLevelEditor.h"
#include "tinyfiledialogs/tinyfiledialogs.h"


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

void ViewerControlPanel::CreateMenuButton(ImVec2 ButtonSize, ImFont* IconFont)
{
    if (ImGui::MenuItem("New Level"))
    {
        if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        {
            // EditorEngine->NewLevel();
        }
    }

    if (ImGui::MenuItem("Load Level"))
    {
        char const* lFilterPatterns[1] = { "*.scene" };
        const char* FileName = tinyfd_openFileDialog("Open Scene File", "", 1, lFilterPatterns, "Scene(.scene) file", 0);

        if (FileName == nullptr)
        {
            tinyfd_messageBox("Error", "파일을 불러올 수 없습니다.", "ok", "error", 1);
            return;
        }
        if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        {
            // EditorEngine->NewLevel();
            // EditorEngine->LoadLevel(FileName);
        }
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Save Level"))
    {
        char const* lFilterPatterns[1] = { "*.scene" };
        const char* FileName = tinyfd_saveFileDialog("Save Scene File", "", 1, lFilterPatterns, "Scene(.scene) file");

        if (FileName == nullptr)
        {
            return;
        }
        if (const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        {
            // EditorEngine->SaveLevel(FileName);
        }

        tinyfd_messageBox("알림", "저장되었습니다.", "ok", "info", 1);
    }

    ImGui::Separator();

    if (ImGui::BeginMenu("Import"))
    {
        if (ImGui::MenuItem("Wavefront (.obj)"))
        {
            char const* lFilterPatterns[1] = { "*.obj" };
            const char* FileName = tinyfd_openFileDialog("Open OBJ File", "", 1, lFilterPatterns, "Wavefront(.obj) file", 0);

            if (FileName != nullptr)
            {
                std::cout << FileName << '\n';

                // if (UAssetManager::Get().GetStaticMesh(FileName) == nullptr)
                // {
                //     tinyfd_messageBox("Error", "파일을 불러올 수 없습니다.", "ok", "error", 1);
                // }
            }
        }

        ImGui::EndMenu();
    }
    
    ImGui::Separator();

    if (ImGui::MenuItem("Quit"))
    {
        bOpenModal = true;
    }
}
