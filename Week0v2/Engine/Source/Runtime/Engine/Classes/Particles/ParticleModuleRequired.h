#pragma once
#include "ParticleModule.h"
#include "Engine/Source/Runtime/Core/Math/Vector.h"
#include "Core/Math/Rotator.h"
#include "ParticleSpriteEmitter.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"
#include "SubUVAnimation.h"
#include "Engine/Classes/Components/PrimitiveComponents/UTexture.h"

enum class EParticleUVFlipMode : uint8
{
    /** Flips UV on all particles. */
    None,
    /** Flips UV on all particles. */
    FlipUV,
    /** Flips U only on all particles. */
    FlipUOnly,
    /** Flips V only on all particles. */
    FlipVOnly,
    /** Flips UV randomly for each particle on spawn. */
    RandomFlipUV,
    /** Flips U only randomly for each particle on spawn. */
    RandomFlipUOnly,
    /** Flips V only randomly for each particle on spawn. */
    RandomFlipVOnly,
    /** Flips U and V independently at random for each particle on spawn. */
    RandomFlipUVIndependent,
};

/** Flips the sign of a particle's base size based on it's UV flip mode. */
//void AdjustParticleBaseSizeForUVFlipping(FVector& OutSize, EParticleUVFlipMode FlipMode)
//{
//    static const float HalfRandMax = 0.5f;
//
//    switch (FlipMode)
//    {
//    case EParticleUVFlipMode::None:
//        return;
//
//    case EParticleUVFlipMode::FlipUV:
//        OutSize = -OutSize;
//        return;
//
//    case EParticleUVFlipMode::FlipUOnly:
//        OutSize.X = -OutSize.X;
//        return;
//
//    case EParticleUVFlipMode::FlipVOnly:
//        OutSize.Y = -OutSize.Y;
//        return;
//
//    /*case EParticleUVFlipMode::RandomFlipUV:
//        OutSize = InRandomStream.FRand() > HalfRandMax ? -OutSize : OutSize;
//        return;
//
//    case EParticleUVFlipMode::RandomFlipUOnly:
//        OutSize.X = InRandomStream.FRand() > HalfRandMax ? -OutSize.X : OutSize.X;
//        return;
//
//    case EParticleUVFlipMode::RandomFlipVOnly:
//        OutSize.Y = InRandomStream.FRand() > HalfRandMax ? -OutSize.Y : OutSize.Y;
//        return;
//
//    case EParticleUVFlipMode::RandomFlipUVIndependent:
//        OutSize.X = InRandomStream.FRand() > HalfRandMax ? -OutSize.X : OutSize.X;
//        OutSize.Y = InRandomStream.FRand() > HalfRandMax ? -OutSize.Y : OutSize.Y;
//        return;*/
//
//    default:
//        /*checkNoEntry();*/
//        break;
//    }
//}



enum EParticleSortMode : int
{
    PSORTMODE_None,
    PSORTMODE_ViewProjDepth,
    PSORTMODE_DistanceToView,
    PSORTMODE_Age_OldestFirst,
    PSORTMODE_Age_NewestFirst,
    PSORTMODE_MAX,
};

enum EEmitterNormalsMode : int
{
    /** Default mode, normals are based on the camera facing geometry. */
    ENM_CameraFacing,
    /** Normals are generated from a sphere centered at NormalsSphereCenter. */
    ENM_Spherical,
    /** Normals are generated from a cylinder going through NormalsSphereCenter, in the direction NormalsCylinderDirection. */
    ENM_Cylindrical,
    ENM_MAX,
};

struct FParticleRequiredModule
{
    uint32 NumFrames;
    uint32 NumBoundingVertices;
    uint32 NumBoundingTriangles;
    float AlphaThreshold;
    TArray<FVector2D> FrameData;
    //FRHIShaderResourceView* BoundingGeometryBufferSRV;    TODO SRV 연결 필요할듯?
    uint8 bCutoutTexureIsValid : 1;
    uint8 bUseVelocityForMotionBlur : 1;
};

class UMaterial;
class UTexture;

class UParticleModuleRequired : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleRequired, UParticleModule)
public:
    UParticleModuleRequired();
    ~UParticleModuleRequired();
    
    //class UMaterialInterface Material;

    FVector EmitterOrigin;
    FRotator EmitterRotation;
    EParticleScreenAlignment ScreenAlignment;
    uint8 bKillOnDeactivate : 1;
    uint8 bKillOnCompleted : 1;
    EParticleSortMode SortMode;
    float EmitterDuration;
    struct FSimpleFloatDistribution SpawnRate;
    TArray<FParticleBurst> BurstList;
    float EmitterDelay;
    uint8 bDelayFirstLoopOnly : 1;

    EParticleSubUVInterpMethod InterpolationMethod;
    uint8 bScaleUV : 1;
    int32 SubImages_Horizontal;
    int32 SubImages_Vertical;
    /**
     *	The amount of time (particle-relative, 0.0 to 1.0) to 'lock' on a random sub image
     *	    0.0 = change every frame
     *      1.0 = select a random image at spawn and hold for the life of the particle
     */
    float RandomImageTime;
    int32 RandomImageChanges;

    FVector MacroUVPosition;
    float MacroUVRadius;
    EParticleUVFlipMode UVFlippingMode;

    EParticleBurstMethod ParticleBurstMethod;

    EEmitterNormalsMode EmitterNormalsMode;
    ESubUVBoundingVertexCount BoundingMode;

    uint8 bDurationRecalcEachLoop : 1;
    FVector NormalsSphereCenter;
    float AlphaThreshold;
    int32 EmitterLoops;

    UTexture* CutoutTexture;
    UMaterial* SpriteTexture;
    uint8 bUseMaxDrawCount = false;
    int32 MaxDrawCount;
    uint8 bUseLocalSpace = true;

    FVector NormalsCylinderDirection;

    TArray<FName> NamedMaterialOverrides;


    void InitializeDefulat();

    virtual void PostLoad();

    // 아래 함수들은 BoundingGemotryBuffer에 관한 것임, 이후 필요하다면 구현해야함
    /*virtual void PostInitProperties();
    virtual void BeginDestroy() override;
    virtual bool IsReadyForFinishDestroy() override;
    virtual void FinishDestroy() override;*/

    virtual EModuleType	GetModuleType() const override { return EPMT_Required; }

    inline int32 GetNumFrames() const
    {
        return SubImages_Vertical * SubImages_Horizontal;
    }

    inline bool IsBoundingGeometryValid() const
    {
        return CutoutTexture != nullptr;
    }

    /*inline FRHIShaderResourceView* GetBoundingGeometrySRV() const
    {
        return BoundingGeometryBuffer->ShaderResourceView;
    }*/

    inline int32 GetNumBoundingVertices() const
    {
        return BoundingMode == BVC_FourVertices ? 4 : 8;
    }

    inline int32 GetNumBoundingTriangles() const
    {
        return BoundingMode == BVC_FourVertices ? 2 : 6;
    }

    /*inline const FVector2f* GetFrameData(int32 FrameIndex) const
    {
        return &DerivedData.BoundingGeometry[FrameIndex * GetNumBoundingVertices()];
    }*/

    /*FParticleRequiredModule* CreateRendererResource()
    {
        FParticleRequiredModule* FReqMod = new FParticleRequiredModule();
        FReqMod->bCutoutTexureIsValid = IsBoundingGeometryValid();
        FReqMod->bUseVelocityForMotionBlur = ShouldUseVelocityForMotionBlur();
        FReqMod->NumFrames = GetNumFrames();
        FReqMod->FrameData = DerivedData.BoundingGeometry;
        FReqMod->NumBoundingVertices = GetNumBoundingVertices();
        FReqMod->NumBoundingTriangles = GetNumBoundingTriangles();
        check(FReqMod->NumBoundingTriangles == 2 || FReqMod->NumBoundingTriangles == 6);
        FReqMod->AlphaThreshold = AlphaThreshold;
        FReqMod->BoundingGeometryBufferSRV = GetBoundingGeometrySRV();
        return FReqMod;
    }*/

    protected:
        friend class FParticleModuleRequiredDetails;
        friend struct FParticleEmitterInstance;

private:
    /** Derived data for this asset, generated off of SubUVTexture. */
    //FSubUVDerivedData DerivedData;

    /** Tracks progress of BoundingGeometryBuffer release during destruction. */
    //FRenderCommandFence ReleaseFence;

    /** Used on platforms that support instancing, the bounding geometry is fetched from a vertex shader instead of on the CPU. */
    //FSubUVBoundingGeometryBuffer* BoundingGeometryBuffer;

    /*void CacheDerivedData();
    void InitBoundingGeometryBuffer();
    void GetDefaultCutout();*/
};
