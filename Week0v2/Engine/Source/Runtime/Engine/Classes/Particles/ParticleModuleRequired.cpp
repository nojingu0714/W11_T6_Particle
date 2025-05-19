#include "ParticleModuleRequired.h"

UParticleModuleRequired::UParticleModuleRequired()
    :Super()
{
    ScreenAlignment = PSA_Square;
    bSpawnModule = true;
    bUpdateModule = true;
    EmitterDuration = 1.0f;
    EmitterDelay = 0.0f;
    EmitterLoops = 0;
    SubImages_Horizontal = 1;
    SubImages_Vertical = 1;
    bUseMaxDrawCount = true;
    MaxDrawCount = 500;
    NormalsSphereCenter = FVector(0.0f, 0.0f, 100.0f);
    NormalsCylinderDirection = FVector(0.0f, 0.0f, 1.0f);
    UVFlippingMode = EParticleUVFlipMode::None;
    BoundingMode = BVC_EightVertices;
    AlphaThreshold = 0.1f;
}

UParticleModuleRequired::~UParticleModuleRequired()
{

}

void UParticleModuleRequired::InitializeDefulat()
{
    // 현재 SpawnRate의 경우 LookUpTable을 구성할 필욘 없으므로
}

void UParticleModuleRequired::PostLoad()
{
    SubImages_Horizontal = FMath::Max(SubImages_Horizontal, 1);
    SubImages_Vertical = FMath::Max(SubImages_Vertical, 1);

  
    if (CutoutTexture != nullptr)
    {
        /*CutoutTexture->ConditionalPostLoad();
        CacheDerivedData();*/
    }

    //InitBoundingGeometryBuffer();
}

