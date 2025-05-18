#include "Components/Material/Material.h"
#include "Math/Color.h"
#include "Math/Vector.h"
#include "UserInterface/Debug/DebugViewModeHelpers.h"

#define DECLARE_PARTICLE_PTR(Name,Address)		\
    FBaseParticle* Name = (FBaseParticle*) (Address);
#define BEGIN_UPDATE_LOOP																								\
{																													\
    check((Owner != NULL) && (Owner->Component != NULL));															\
    int32&			ActiveParticles = Owner->ActiveParticles;														\
    uint32			CurrentOffset	= Offset;																		\
    const uint8*		ParticleData	= Owner->ParticleData;															\
    const uint32		ParticleStride	= Owner->ParticleStride;														\
    uint16*			ParticleIndices	= Owner->ParticleIndices;														\
    for(int32 i=ActiveParticles-1; i>=0; i--)																			\
    {																												\
    const int32	CurrentIndex	= ParticleIndices[i];															\
    const uint8* ParticleBase	= ParticleData + CurrentIndex * ParticleStride;									\
    FBaseParticle& Particle		= *((FBaseParticle*) ParticleBase);												\
    if ((Particle.Flags & STATE_Particle_Freeze) == 0)															\
{																											\

#define END_UPDATE_LOOP																									\
}																											\
CurrentOffset				= Offset;																		\
}																												\
}

class UMaterialInterface;

enum EDynamicEmitterType
{
    DET_Unknown = 0,
    DET_Sprite,
    DET_Mesh,
    DET_Beam2,
    DET_Ribbon,
    DET_AnimTrail,
    DET_Custom
};

struct FBaseParticle // 파티클 하나의 완전한 상태를 저장하는 POD 구조체 
{
    // 48 bytes
    FVector		OldLocation;			// Last frame's location, used for collision
    FVector		Location;				// Current location

    // 16 bytes
    FVector		BaseVelocity;			// Velocity = BaseVelocity at the start of each frame.
    float			Rotation;				// Rotation of particle (in Radians)

    // 16 bytes
    FVector		Velocity;				// Current velocity, gets reset to BaseVelocity each frame to allow 
    float			BaseRotationRate;		// Initial angular velocity of particle (in Radians per second)

    // 16 bytes
    FVector		BaseSize;				// Size = BaseSize at the start of each frame
    float			RotationRate;			// Current rotation rate, gets reset to BaseRotationRate each frame

    // 16 bytes
    FVector		Size;					// Current size, gets reset to BaseSize each frame
    int32			Flags;					// Flags indicating various particle states

    // 16 bytes
    FLinearColor	Color;					// Current color of particle.

    // 16 bytes
    FLinearColor	BaseColor;				// Base color of the particle

    // 16 bytes
    float			RelativeTime;			// Relative time, range is 0 (==spawn) to 1 (==death)
    float			OneOverMaxLifetime;		// Reciprocal of lifetime
    float			Placeholder0;
    float			Placeholder1;

    FBaseParticle()
    : OldLocation(FVector::ZeroVector)
    , Location(FVector::ZeroVector)
    , BaseVelocity(FVector::ZeroVector)
    , Rotation(0.0f)
    , Velocity(FVector::ZeroVector)
    , BaseRotationRate(0.0f)
    , BaseSize(FVector(1.0f))           // 기본 크기를 (1,1,1)로
    , RotationRate(0.0f)
    , Size(BaseSize)
    , Flags(0)
    , Color(FLinearColor::White)
    , BaseColor(FLinearColor::White)
    , RelativeTime(0.0f)
    , OneOverMaxLifetime(1.0f)          // 기본 수명 1초로 가정
    , Placeholder0(0.0f)
    , Placeholder1(0.0f)
    {}
};

struct FParticleSpriteVertex // GPU로 전달되는 스프라이트 파티클용 정점 데이터 
{
    /** The position of the particle. */
    FVector Position;
    /** The relative time of the particle. */
    float RelativeTime;
    /** The previous position of the particle. */
    FVector	OldPosition;
    /** Value that remains constant over the lifetime of a particle. */
    float ParticleId;
    /** The size of the particle. */
    FVector2D Size;
    /** The rotation of the particle. */
    float Rotation;
    /** The sub-image index for the particle. */
    float SubImageIndex;
    /** The color of the particle. */
    FLinearColor Color;
};
struct FMeshParticleInstanceVertex // GPU로 전송되는 메시 인스텅신 파티클용 정점 데이터
{
    /** The color of the particle. */
    FLinearColor Color;

    /** The instance to world transform of the particle. Translation vector is packed into W components. */
    FVector4 Transform[3];

    /** The velocity of the particle, XYZ: direction, W: speed. */
    FVector4 Velocity;

    /** The sub-image texture offsets for the particle. */
    int16 SubUVParams[4];

    /** The sub-image lerp value for the particle. */
    float SubUVLerp;

    /** The relative time of the particle. */
    float RelativeTime;
};

struct FParticleDataContainer // 파티클 데이터 용 메모리 블록
{
    int32 MemBlockSize;
    int32 ParticleDataNumBytes;
    int32 ParticleIndicesNumShorts;
    uint8* ParticleData; // FBaseParticle를 배열 형태로 저장 
    uint16* ParticleIndices; // 실제로 렌더링할 활성 파티클의 인덱스

    FParticleDataContainer()
        : MemBlockSize(0)
        , ParticleDataNumBytes(0)
        , ParticleIndicesNumShorts(0)
        , ParticleData(nullptr)
        , ParticleIndices(nullptr)
    {
    }
    ~FParticleDataContainer()
    {
        Free();
    }
    void Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts);
    void Free();
};

inline void FParticleDataContainer::Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts)
{
}

inline void FParticleDataContainer::Free()
{
}

// Replay Data Base 
struct FDynamicEmitterReplayDataBase // 재생 모드에서 Emitter 상태를 저장 복원 
{
    /** The type of emitter. */
    EDynamicEmitterType eEmitterType;

    /** The number of particles currently active in this emitter. */
    int32 ActiveParticleCount;

    int32 ParticleStride;
    FParticleDataContainer DataContainer;

    FVector Scale;

    int32 SortMode;
};

struct FDynamicSpriteEmitterReplayDataBase : public FDynamicEmitterReplayDataBase
{
    //원레 머터리얼 인터페이스를 사용하나 UMaterial로 일단 퉁치기
    //UMaterialInterface*             MaterialInterface;
    UMaterial*                      Material;
    UParticleModuleRequired*        RequiredModule;
};

struct FDynamicSpriteEmitterReplayData : public FDynamicSpriteEmitterReplayDataBase
{
    /** Constructor */
    FDynamicSpriteEmitterReplayData()
    {
    }
    
    /** Serialization */
    virtual void Serialize( FArchive& Ar )
    {
        // // Call parent implementation
        // FDynamicSpriteEmitterReplayDataBase::Serialize( Ar );
        //
        // // ...
    }
};
// Emitter Data Base 
struct FDynamicEmitterDataBase
{
    int32 EmitterIndex;
    
    virtual const FDynamicEmitterReplayDataBase& GetSource() const = 0;
};

struct FDynamicSpriteEmitterDataBase : public FDynamicEmitterDataBase
{
    void SortSpriteParticles();
    virtual int32 GetDynamicVertexStride(ERHIFeatureLevel::Type /*InFeatureLevel*/) const = 0;
    const FDynamicEmitterReplayDataBase& GetSource() const
    {
    }
};

struct FDynamicSpriteEmitterData : public FDynamicSpriteEmitterDataBase
{
    virtual int32 GetDynamicVertexStride(ERHIFeatureLevel::Type InFeatureLevel) const override
    {
        return sizeof(FParticleSpriteVertex);
    }
    const FDynamicEmitterReplayDataBase& GetSource() const
    {
        return Source;
    }
    FDynamicSpriteEmitterReplayData Source;
};


struct FDynamicMeshEmitterData : public FDynamicSpriteEmitterDataBase
{
    virtual int32 GetDynamicVertexStride(ERHIFeatureLevel::Type /*InFeatureLevel*/) const override
    {
        return sizeof(FMeshParticleInstanceVertex);
    }
};
