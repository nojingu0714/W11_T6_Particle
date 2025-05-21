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
            // 1) 먼저 모든 머티리얼을 수집
            TArray<UMaterial*> MaterialList;
            TArray<FString> MaterialNames;
            bool bInitialized = false;
            if (!bInitialized)
            {
                for (UMaterial* Mat : TObjectRange<UMaterial>())
                {
                    MaterialList.Add(Mat);
                    // GetName() 반환값을 UTF-8 문자열로 변환
                    MaterialNames.Add(Mat->GetMaterialInfo().MTLName);
                }
                bInitialized = true;
            }

            // 2) 선택 인덱스
            static int32 CurrentIndex = 0;
            CurrentIndex = FMath::Clamp(CurrentIndex, 0, MaterialList.Num() - 1);

            // 3) ImGui UI
            ImGui::Text("Particle Module Required");
            if (ImGui::BeginCombo("Select Material", GetData(MaterialNames[CurrentIndex])))
            {
                for (int32 i = 0; i < MaterialList.Num(); ++i)
                {
                    bool bIsSelected = (CurrentIndex == i);
                    if (ImGui::Selectable(GetData(MaterialNames[i]), bIsSelected))
                    {
                        CurrentIndex = i;
                    }
                    if (bIsSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            // 4) 선택된 머티리얼을 할당
            UMaterial* SelectedMat = MaterialList.IsValidIndex(CurrentIndex) ? MaterialList[CurrentIndex] : nullptr;
            if (SelectedMat)
            {
                // UParticleModuleRequired::SpriteTexture 는 UTexture* 타입이므로,
                // 머티리얼을 텍스처로 사용하려면 머티리얼 인스턴스의 텍스처 파라미터를 꺼내거나
                // 필요한 UTexture2D* 를 직접 나열해야 합니다.
                // 예시로, 만약 머티리얼 이름 그대로 텍스처 에셋이 있다면:
                Required->SpriteTexture = SelectedMat;
            }
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

