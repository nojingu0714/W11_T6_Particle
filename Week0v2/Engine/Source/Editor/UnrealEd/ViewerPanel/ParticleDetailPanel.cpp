#include "ParticleDetailPanel.h"

#include "ImGUI/imgui.h"
#include "Runtime/CoreUObject/UObject/UObjectIterator.h"
#include "Particles/ParticleModuleRequired.h"
#include "Particles/Lifetime/ParticleModuleLifetime.h"
#include "Particles/Location/ParticleModuleLocation.h"
#include "Particles/Color/ParticleModuleColor.h"
#include "Particles/Size/ParticleModuleSize.h"
#include "Particles/Size/ParticleModuleSizeScaleBySpeed.h"
#include "Particles/Spawn/ParticleModuleSpawn.h"
#include "Particles/Velocity/ParticleModuleVelocity.h"
#include "Engine/Classes/Components/Material/Material.h"
#include <Particles/Snow/ParticleModuleSnow.h>

#include "Components/Mesh/StaticMesh.h"
#include "Particles/Collision/ParticleModuleCollision.h"


void ParticleDetailPanel::Render(UParticleModule* SelectedModule)
{
    ImVec2 WinSize = ImVec2(Width, Height);
    
    ImGui::SetNextWindowPos(ImVec2(0, Height * 0.5f));

    ImGui::SetNextWindowSize(ImVec2(WinSize.x * 0.3f, Height * 0.5f));
    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;

    if (ImGui::Begin("Details", nullptr, PanelFlags))
    {
        if (UParticleModuleRequired* Required = Cast<UParticleModuleRequired>(SelectedModule))
        {
            EditParticleModuleRequired(Required);
        }
        else if (UParticleModuleSpawn* Spawn = Cast<UParticleModuleSpawn>(SelectedModule))
        {
            ImGui::Text("Particle Module Spawn");
            RenderFSimpleFloatDistribution(Spawn->Rate, 0.0f, "Spawn Rate");
            ImGui::Separator();
        }
        else if (UParticleModuleSize* Size = Cast<UParticleModuleSize>(SelectedModule))
        {
            ImGui::Text("Particle Module Size");
            RenderFSimpleVectorDistribution(Size->StartSize, 0.0f, "StartSize");
            ImGui::Separator();
        }
        else if (UParticleModuleSizeScaleBySpeed* ScaleBySpeed = Cast<UParticleModuleSizeScaleBySpeed>(SelectedModule))
        {
            ImGui::Text("Particle Module ScaleBySpeed");
            ImGui::Text("SpeedScale");
            ImGui::PushItemWidth(50.0f);
            ImGui::InputFloat("X##SpeedScale", &ScaleBySpeed->SpeedScale.X);
            ImGui::SameLine();
            ImGui::InputFloat("Y##SpeedScale", &ScaleBySpeed->SpeedScale.Y);
            ImGui::Text("MaxScale");
            ImGui::InputFloat("X##MaxScale", &ScaleBySpeed->MaxScale.X);
            ImGui::SameLine();
            ImGui::InputFloat("Y##MaxScale", &ScaleBySpeed->MaxScale.Y);
            ImGui::PopItemWidth();
            ImGui::Separator();
        }
        else if (Cast<UParticleModuleSizeBase>(SelectedModule))
        {
            ImGui::Text("Particle Module Size");
            ImGui::Separator();
        }
        else if (UParticleModuleLifetime* Lifetime = Cast<UParticleModuleLifetime>(SelectedModule))
        {
            ImGui::Text("Particle Module Lifetime");
            RenderFSimpleFloatDistribution(Lifetime->Lifetime, 0.0f, "Lifetime");
            ImGui::Separator();
        }
        else if (UParticleModuleLocation* Location = Cast<UParticleModuleLocation>(SelectedModule))
        {
            ImGui::Text("Particle Module Location");
            RenderFSimpleVectorDistribution(Location->StartLocation, 0.0f, "Location");
            ImGui::Separator();
        }
        else if (UParticleModuleVelocity* Velocity = Cast<UParticleModuleVelocity>(SelectedModule))
        {
            ImGui::Text("Particle Module Velocity");
            RenderFSimpleVectorDistribution(Velocity->StartVelocity, 0.0f, "StartVelocity");
            RenderFSimpleVectorDistribution(Velocity->StartVelocityRadial, 0.0f, "StartVelocityRadial");
            ImGui::Separator();
        }
        else if (UParticleModuleColor* Color = Cast<UParticleModuleColor>(SelectedModule))
        {
            ImGui::Text("Particle Module Color");
            RenderFSimpleVectorDistribution(Color->ColorScaleOverLife, 0.0f, "ColorOverLife");
            RenderFSimpleFloatDistribution(Color->AlphaScaleOverLife, 0.0f, "FloatOverLife");
            ImGui::Separator();
        }
        else if (UParticleModuleSnow* Snow = Cast<UParticleModuleSnow>(SelectedModule))
        {
            ImGui::Text("Particle Module Snow");
            ImGui::Separator();

            // 좌표 범위
            ImGui::Text("Spawn Range X:");
            RenderFSimpleFloatDistribution(Snow->SpawnXMin, 0.0f, "X_Min");
            ImGui::SameLine();
            RenderFSimpleFloatDistribution(Snow->SpawnXMax, 0.0f, "X_Max");
            if (Snow->SpawnXMin.GetValue(0) > Snow->SpawnXMax.GetValue(0)) {
                Snow->SpawnXMax = Snow->SpawnXMin;
            }

            ImGui::Text("Spawn Range Y:");
            RenderFSimpleFloatDistribution(Snow->SpawnYMin, 0.0f, "Y_Min");
            ImGui::SameLine();
            RenderFSimpleFloatDistribution(Snow->SpawnYMax, 0.0f, "Y_Max");
            if (Snow->SpawnYMin.GetValue(0) > Snow->SpawnYMax.GetValue(0)) {
                Snow->SpawnYMax = Snow->SpawnYMin;
            }

            ImGui::Text("Spawn Range Z:");
            RenderFSimpleFloatDistribution(Snow->SpawnZMin, 0.0f, "Z_Min");
            ImGui::SameLine();
            RenderFSimpleFloatDistribution(Snow->SpawnZMax, 0.0f, "Z_Max");
            if (Snow->SpawnZMin.GetValue(0) > Snow->SpawnZMax.GetValue(0)) {
                Snow->SpawnZMax = Snow->SpawnZMin;
            }

            ImGui::Separator();

            // 중력 스케일
            ImGui::Text("Gravity Scale:");
            RenderFSimpleFloatDistribution(Snow->GravityScale, 1.0f, "GravityScale");

            // 좌우 흔들림
            ImGui::Text("Sway Settings:");
            RenderFSimpleFloatDistribution(Snow->SwayFrequency, 0.5f, "SwayFreq");

            ImGui::Text("Sway Amount:");
            RenderFSimpleFloatDistribution(Snow->SwayMinAmount, 0.3f, "SwayMin");
            ImGui::SameLine();
            RenderFSimpleFloatDistribution(Snow->SwayMaxAmount, 0.5f, "SwayMax");
            if (Snow->SwayMinAmount.GetValue(0) > Snow->SwayMaxAmount.GetValue(0)) {
                Snow->SwayMaxAmount = Snow->SwayMinAmount;
            }

            ImGui::Separator();

      

            // 회전 설정
            ImGui::Text("Rotation:");

            ImGui::Text("Speed:");
            RenderFSimpleFloatDistribution(Snow->RotationSpeedMin, 0.2f, "RotSpeedMin");
            ImGui::SameLine();
            RenderFSimpleFloatDistribution(Snow->RotationSpeedMax, 0.5f, "RotSpeedMax");
            if (Snow->RotationSpeedMin.GetValue(0) > Snow->RotationSpeedMax.GetValue(0)) {
                Snow->RotationSpeedMax = Snow->RotationSpeedMin;
            }

            ImGui::Text("Direction Bias:");
            RenderFSimpleFloatDistribution(Snow->RotationDirectionBias, 0.0f, "DirBias");

            ImGui::Separator();
         }
        else if (UParticleModuleCollision* Collision = Cast<UParticleModuleCollision>(SelectedModule))
        {
            ImGui::Text("Particle Module Collision");
            ImGui::InputFloat("Bounce Factor", &Collision->BounceFactor);
            ImGui::InputFloat("Friction Factor", &Collision->FrictionFactor);
            ImGui::InputFloat("Collision Radius", &Collision->CollisionRadius);
            RenderFSimpleFloatDistribution(Collision->MaxCollisions, 0.0f, "MaxCollisions");
            const char* EPCC_Names[] = {
                "Kill",
                "Freeze",
                "Halt Collisions",
                "Freeze Translation",
                "Freeze Rotation",
                "Freeze Movement"
            };

            // Combo를 이용한 선택 UI
            int currentItem = static_cast<int>(Collision->Response);
            if (ImGui::Combo("Collision Response", &currentItem, EPCC_Names, EPCC_MAX))
            {
                // 사용자가 선택을 변경했을 경우 처리
                Collision->Response = static_cast<EParticleCollisionComplete>(currentItem);
            }
            ImGui::Separator();
        }
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

void ParticleDetailPanel::RenderFSimpleFloatDistribution(FSimpleFloatDistribution& RenderDistribution, float Tvalue, FString DistributionName)
{
    switch (RenderDistribution.GetDistributionType())
    {
    case EDistributionType::Constant: {
        ImGui::PushItemWidth(50.0f);
        float NewRate = RenderDistribution.GetValue(Tvalue);
        ImGui::InputFloat(*DistributionName, &NewRate);
        RenderDistribution.Constant = NewRate;
        ImGui::PopItemWidth();
        return;
        break;
    }

    case EDistributionType::Uniform: {
        ImGui::PushItemWidth(50.0f);
        float& MinValue = RenderDistribution.Min;
        float& MaxValue = RenderDistribution.Max;
        ImGui::InputFloat("Min", &MinValue);
        ImGui::SameLine();
        ImGui::InputFloat("Max", &MaxValue);
        ImGui::PopItemWidth();
        return;
        break;
    }

    case EDistributionType::Linear:
    case EDistributionType::EaseInOut:
    case EDistributionType::SinWave: {
        ImGui::PushItemWidth(50.0f);
        float& StartValue = RenderDistribution.Min;
        float& EndValue = RenderDistribution.Max;
        ImGui::InputFloat("Start", &StartValue);
        ImGui::SameLine();
        ImGui::InputFloat("End", &EndValue);
        ImGui::PopItemWidth();
        return;
        break;
    }

    default: {
        UE_LOG(LogLevel::Error, "EDistribution Error");
        return;
        break;
    }
    }
}

void ParticleDetailPanel::RenderFSimpleVectorDistribution(FSimpleVectorDistribution& RenderDistribution, float Tvalue, FString DistributionName)
{
    switch (RenderDistribution.GetDistributionType())
    {
    case EDistributionType::Constant: {
        FVector NewVector;
        RenderDistribution.GetDistributionVector(NewVector);

        ImGui::Text(*DistributionName);
        FString DistributionNameWithX = "X##" + DistributionName;
        FString DistributionNameWithY = "Y##" + DistributionName;
        FString DistributionNameWithZ = "Z##" + DistributionName;

        ImGui::PushItemWidth(50.0f);
        ImGui::InputFloat(*DistributionNameWithX, &NewVector.X);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithY, &NewVector.Y);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithZ, &NewVector.Z);
        ImGui::PopItemWidth();

        RenderDistribution.SetDistributionVector(NewVector);

        return;
        break;
    }

    case EDistributionType::Uniform: {
        FVector StartVector;
        FVector EndVector;
        RenderDistribution.GetDistributionVectors(StartVector, EndVector);

        ImGui::Text(*DistributionName);
        FString DistributionNameWithXMin = "X##" + DistributionName + "Min";
        FString DistributionNameWithYMin = "Y##" + DistributionName + "Min";
        FString DistributionNameWithZMin = "Z##" + DistributionName + "Min";
        FString DistributionNameWithXMax = "X##" + DistributionName + "Max";
        FString DistributionNameWithYMax = "Y##" + DistributionName + "Max";
        FString DistributionNameWithZMax = "Z##" + DistributionName + "Max";

        ImGui::PushItemWidth(50.0f);
        ImGui::Text("MinVector");
        ImGui::InputFloat(*DistributionNameWithXMin, &StartVector.X);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithYMin, &StartVector.Y);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithZMin, &StartVector.Z);

        ImGui::Text("MaxVector");
        ImGui::InputFloat(*DistributionNameWithXMax, &EndVector.X);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithYMax, &EndVector.Y);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithZMax, &EndVector.Z);
        ImGui::PopItemWidth();

        RenderDistribution.SetDistributionVectors(StartVector, EndVector);

        return;
        break;
    }

    case EDistributionType::Linear:
    case EDistributionType::EaseInOut:
    case EDistributionType::SinWave: {
        FVector StartVector;
        FVector EndVector;
        RenderDistribution.GetDistributionVectors(StartVector, EndVector);


        FString DistributionNameWithXStart = "X##" + DistributionName + "Start";
        FString DistributionNameWithYStart = "Y##" + DistributionName + "Start";
        FString DistributionNameWithZStart = "Z##" + DistributionName + "Start";
        FString DistributionNameWithXEnd = "X##" + DistributionName + "End";
        FString DistributionNameWithYEnd = "Y##" + DistributionName + "End";
        FString DistributionNameWithZEnd = "Z##" + DistributionName + "End";

        ImGui::PushItemWidth(50.0f);
        ImGui::Text("StartVector");
        ImGui::InputFloat(*DistributionNameWithXStart, &StartVector.X);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithYStart, &StartVector.Y);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithZStart, &StartVector.Z);

        ImGui::Text("EndVector");
        ImGui::InputFloat(*DistributionNameWithXEnd, &EndVector.X);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithYEnd, &EndVector.Y);
        ImGui::SameLine();
        ImGui::InputFloat(*DistributionNameWithZEnd, &EndVector.Z);
        ImGui::PopItemWidth();

        RenderDistribution.SetDistributionVectors(StartVector, EndVector);

        return;
        break;
    }

    default: {
        UE_LOG(LogLevel::Error, "EDistribution Error");
        return;
        break;
    }
    }
}

void ParticleDetailPanel::EditParticleModuleRequired(UParticleModuleRequired* Required)
{
    if (!Required)
    {
        return;
    }

    ImGui::Text("Particle Module Required");
    ImGui::Separator();

    // --- 머티리얼 선택 UI (스프라이트용) ---
    ImGui::Text("Sprite Material:");

    // 1) 모든 UMaterial 수집 (매번 호출 시 다시 수집하거나, 캐싱 전략 사용 가능)
    //    여기서는 간단히 매번 수집합니다. 성능이 중요하다면 최적화 필요.
    static TArray<UMaterial*> SpriteMaterialList; // static으로 하여 이전 선택 유지 시도
    static TArray<FString> SpriteMaterialNames;
    static bool bSpriteMaterialsInitialized = false; // 초기화 플래그

    // UI가 처음 그려지거나, 특정 조건에서 머티리얼 목록을 다시 로드할 수 있도록 함
    // (예: 에디터에서 새 머티리얼이 추가되었을 때)
    // 여기서는 간단히 bSpriteMaterialsInitialized 플래그 사용
    if (ImGui::Button("Refresh Material List") || !bSpriteMaterialsInitialized)
    {
        SpriteMaterialList.Empty();
        SpriteMaterialNames.Empty();
        for (UMaterial* Mat : TObjectRange<UMaterial>()) // 실제 프로젝트에서는 애셋 레지스트리 사용 권장
        {
            if (Mat) // 유효한 머티리얼만 추가
            {
                SpriteMaterialList.Add(Mat);
                SpriteMaterialNames.Add(Mat->GetMaterialInfo().MTLName); // 또는 Mat->GetName()
            }
        }
        bSpriteMaterialsInitialized = true;
    }

    // 2) 현재 선택된 머티리얼 인덱스 찾기 (Required->SpriteMaterial 기준으로)
    //    UParticleModuleRequired에 UMaterial* SpriteMaterial 멤버가 있다고 가정합니다.
    //    (원래 코드에서는 Required->SpriteTexture 였으나, 머티리얼을 직접 다루는 것이 더 일반적)
    int32 CurrentSpriteMaterialIndex = -1;
    if (Required->SpriteTexture != nullptr && SpriteMaterialList.Num() > 0)
    {
        for (int32 i = 0; i < SpriteMaterialList.Num(); ++i)
        {
            if (SpriteMaterialList[i] == Required->SpriteTexture)
            {
                CurrentSpriteMaterialIndex = i;
                break;
            }
        }
    }

    // 3) ImGui 콤보 박스 UI
    const char* previewSpriteMaterialName = (CurrentSpriteMaterialIndex != -1) ? GetData(SpriteMaterialNames[CurrentSpriteMaterialIndex]) : "None";
    if (ImGui::BeginCombo("##SpriteMaterialCombo", previewSpriteMaterialName)) // ID에 ##를 붙여 라벨 숨김
    {
        // "None" 옵션 추가
        bool bIsNoneSelected = (Required->SpriteTexture == nullptr);
        if (ImGui::Selectable("None", bIsNoneSelected))
        {
            Required->SpriteTexture = nullptr;
            Required->MeshData = nullptr; // 스프라이트 머티리얼을 "None"으로 하면 메시도 해제 
            CurrentSpriteMaterialIndex = -1;
        }
        if (bIsNoneSelected) ImGui::SetItemDefaultFocus();

        for (int32 i = 0; i < SpriteMaterialList.Num(); ++i)
        {
            bool bIsSelected = (CurrentSpriteMaterialIndex == i);
            if (ImGui::Selectable(GetData(SpriteMaterialNames[i]), bIsSelected))
            {
                CurrentSpriteMaterialIndex = i;
                Required->SpriteTexture = SpriteMaterialList[i];
                // 스프라이트 머티리얼을 선택하면, 메시는 사용하지 않도록 설정
                Required->MeshData = nullptr;
                Required->ParticleEmitterType = EParticleEmitterType::Sprite; // 스프라이트 파티클로 설정
            }
            if (bIsSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Separator();

    // --- 스태틱 메시 선택 UI (메시 파티클용) ---
    ImGui::Text("Static Mesh:");

    // 1) 모든 UStaticMesh 수집
    static TArray<UStaticMesh*> StaticMeshList;
    static TArray<FString> StaticMeshNames;
    static bool bStaticMeshesInitialized = false;

    if (ImGui::Button("Refresh Mesh List") || !bStaticMeshesInitialized)
    {
        StaticMeshList.Empty();
        StaticMeshNames.Empty();
        for (UStaticMesh* MeshAsset : TObjectRange<UStaticMesh>()) // 실제 프로젝트에서는 애셋 레지스트리 사용 권장
        {
            if (MeshAsset) // 유효한 메시만 추가
            {
                StaticMeshList.Add(MeshAsset);
                StaticMeshNames.Add(MeshAsset->GetRenderData()->DisplayName); // UStaticMesh에 GetName()이 있다고 가정
            }
        }
        bStaticMeshesInitialized = true;
    }

    // 2) 현재 선택된 스태틱 메시 인덱스 찾기 (Required->Mesh 기준으로)
    int32 CurrentStaticMeshIndex = -1;
    if (Required->MeshData != nullptr && StaticMeshList.Num() > 0)
    {
        for (int32 i = 0; i < StaticMeshList.Num(); ++i)
        {
            if (StaticMeshList[i] == Required->MeshData)
            {
                CurrentStaticMeshIndex = i;
                break;
            }
        }
    }

    // 3) ImGui 콤보 박스 UI
    const char* previewStaticMeshName = (CurrentStaticMeshIndex != -1) ? GetData(StaticMeshNames[CurrentStaticMeshIndex]) : "None";
    if (ImGui::BeginCombo("##StaticMeshCombo", previewStaticMeshName))
    {
        // "None" 옵션 추가
        bool bIsNoneSelected = (Required->MeshData == nullptr);
        if (ImGui::Selectable("None", bIsNoneSelected))
        {
            Required->MeshData = nullptr;
            // 메시를 "None"으로 하면 스프라이트 머티리얼 선택에 영향을 주지 않음 (또는 특정 규칙 적용 가능)
            CurrentStaticMeshIndex = -1;
        }
        if (bIsNoneSelected) ImGui::SetItemDefaultFocus();

        for (int32 i = 0; i < StaticMeshList.Num(); ++i)
        {
            bool bIsSelected = (CurrentStaticMeshIndex == i);
            if (ImGui::Selectable(GetData(StaticMeshNames[i]), bIsSelected))
            {
                CurrentStaticMeshIndex = i;
                Required->MeshData = StaticMeshList[i];
                // 스태틱 메시를 선택하면, 스프라이트 머티리얼은 사용하지 않도록 설정
                Required->SpriteTexture = nullptr;
                Required->ParticleEmitterType = EParticleEmitterType::Mesh; // 메시 파티클로 설정
            }
            if (bIsSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

