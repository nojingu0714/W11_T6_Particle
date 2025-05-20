#include "Container/Array.h"
#include "Components/Material/Material.h"
#include "Math/Color.h"
#include "Math/Vector.h"
#include "UserInterface/Debug/DebugViewModeHelpers.h"

/*-----------------------------------------------------------------------------
    Helper macros.
-----------------------------------------------------------------------------*/
//	Macro fun.

// if 문의 오류를 막기 위해 do while 사용
#define SPAWN_INIT																										\
    do{   \
        if ((Owner != NULL) && (Owner->Component != NULL))  \
        {   \
            UE_LOG(LogLevel::Error, "SPAWN_INIT NULL"); \
        }   \
    } while(0);  \
    const int32		ActiveParticles = Owner->ActiveParticles;															\
    const uint32		ParticleStride = Owner->ParticleStride;														\
    uint32			CurrentOffset = Offset;																	\
    FBaseParticle& Particle = *(ParticleBase);

#define DECLARE_PARTICLE(Name,Address)		\
FBaseParticle& Name = *((FBaseParticle*) (Address));

#define DECLARE_PARTICLE_CONST(Name,Address)		\
const FBaseParticle& Name = *((const FBaseParticle*) (Address));

#define DECLARE_PARTICLE_PTR(Name,Address)		\
FBaseParticle* Name = (FBaseParticle*) (Address);

struct FDynamicEmitterDataBase;
struct FMatrix;
#define BEGIN_UPDATE_LOOP																								\
{			\
    do{   \
        if ((Owner != NULL) && (Owner->Component != NULL))  \
        {   \
            UE_LOG(LogLevel::Error, "BEGINE_UPDATE_LOOP NULL"); \
        }   \
    } while(0); \
    int32&			ActiveParticles = Owner->ActiveParticles;										\
    uint32			CurrentOffset	= Offset;					    								\
    const uint8*		ParticleData	= Owner->ParticleData;										\
    const uint32		ParticleStride	= Owner->ParticleStride;									\
    uint16*			ParticleIndices	= Owner->ParticleIndices;										\
    for(int32 i=ActiveParticles-1; i>=0; i--)								    					\
    {																								\
    const int32	CurrentIndex	= ParticleIndices[i];				          						\
    const uint8* ParticleBase	= ParticleData + CurrentIndex * ParticleStride;						\
    FBaseParticle& Particle		= *((FBaseParticle*) ParticleBase);								    \
    if ((Particle.Flags & STATE_Particle_Freeze) == 0)	   							                \
        {
#define END_UPDATE_LOOP																									\
       }																											\
CurrentOffset				= Offset;																		\
}																												\
}

class UMaterialInterface;
class UParticleModuleRequired;

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

/*-----------------------------------------------------------------------------
    Particle State Flags
-----------------------------------------------------------------------------*/
enum EParticleStates
{
    /** Ignore updates to the particle						*/
    STATE_Particle_JustSpawned = 0x02000000,
    /** Ignore updates to the particle						*/
    STATE_Particle_Freeze = 0x04000000,
    /** Ignore collision updates to the particle			*/
    STATE_Particle_IgnoreCollisions = 0x08000000,
    /**	Stop translations of the particle					*/
    STATE_Particle_FreezeTranslation = 0x10000000,
    /**	Stop rotations of the particle						*/
    STATE_Particle_FreezeRotation = 0x20000000,
    /** Combination for a single check of 'ignore' flags	*/
    STATE_Particle_CollisionIgnoreCheck = STATE_Particle_Freeze | STATE_Particle_IgnoreCollisions | STATE_Particle_FreezeTranslation |
    STATE_Particle_FreezeRotation,
    /** Delay collision updates to the particle				*/
    STATE_Particle_DelayCollisions = 0x40000000,
    /** Flag indicating the particle has had at least one collision	*/
    STATE_Particle_CollisionHasOccurred = 0x80000000,
    /** State mask. */
    STATE_Mask = 0xFE000000,
    /** Counter mask. */
    STATE_CounterMask = (~STATE_Mask)
};

struct FParticleOrder
{
    // 1. 원래 파티클 배열에서의 인덱스 (정렬 후에도 어떤 파티클이었는지 알기 위함)
    int32 ParticleIndex;

    // 2. 정렬 기준 값 (예: 카메라로부터의 거리, 뷰 프로젝션 후의 W값 등)
    //    float과 uint32를 공용체(union)로 사용하는 것은 특정 최적화나
    //    다른 정렬 모드(예: 색상 기반 정렬)를 위한 것일 수 있습니다.
    //    제공된 코드에서는 Z (float)만 사용되고 있습니다.
    union
    {
        float Z; // 주로 깊이 값 (더 큰 값이 더 멀리 있는 것을 의미하도록 사용될 수 있음)
        uint32 C; // 다른 정렬 기준을 위한 값 (예: 색상 값의 정수 표현)
    };

    // 생성자 1: ParticleIndex와 float 타입의 Z값을 받아 초기화
    FParticleOrder(int32 InParticleIndex, float InZ)
        : ParticleIndex(InParticleIndex)
        , Z(InZ)
    {
    }

    // 생성자 2: ParticleIndex와 uint32 타입의 C값을 받아 초기화
    FParticleOrder(int32 InParticleIndex, uint32 InC)
        : ParticleIndex(InParticleIndex)
        , C(InC)
    {
    }
};

struct FBaseParticle // 파티클 하나의 완전한 상태를 저장하는 POD 구조체 
{
    // 48 bytes
    FVector OldLocation; // Last frame's location, used for collision
    FVector Location; // Current location

    // 16 bytes
    FVector BaseVelocity; // Velocity = BaseVelocity at the start of each frame.
    float Rotation; // Rotation of particle (in Radians)

    // 16 bytes
    FVector Velocity; // Current velocity, gets reset to BaseVelocity each frame to allow 
    float BaseRotationRate; // Initial angular velocity of particle (in Radians per second)

    // 16 bytes
    FVector BaseSize; // Size = BaseSize at the start of each frame
    float RotationRate; // Current rotation rate, gets reset to BaseRotationRate each frame

    // 16 bytes
    FVector Size; // Current size, gets reset to BaseSize each frame
    int32 Flags; // Flags indicating various particle states

    // 16 bytes
    FLinearColor Color; // Current color of particle.

    // 16 bytes
    FLinearColor BaseColor; // Base color of the particle

    // 16 bytes
    float RelativeTime; // Relative time, range is 0 (==spawn) to 1 (==death)
    float OneOverMaxLifetime; // Reciprocal of lifetime
    float Placeholder0;
    float Placeholder1;

    FBaseParticle()
        : OldLocation(FVector::ZeroVector)
        , Location(FVector::ZeroVector)
        , BaseVelocity(FVector::ZeroVector)
        , Rotation(0.0f)
        , Velocity(FVector::ZeroVector)
        , BaseRotationRate(0.0f)
        , BaseSize(FVector(1.0f)) // 기본 크기를 (1,1,1)로
        , RotationRate(0.0f)
        , Size(BaseSize)
        , Flags(0)
        , Color(FLinearColor::White)
        , BaseColor(FLinearColor::White)
        , RelativeTime(0.0f)
        , OneOverMaxLifetime(1.0f) // 기본 수명 1초로 가정
        , Placeholder0(0.0f)
        , Placeholder1(0.0f)
    {
    }
};

struct FParticleSpriteVertex // GPU로 전달되는 스프라이트 파티클용 정점 데이터 
{
    /** The position of the particle. */
    FVector Position = FVector::ZeroVector;
    /** The relative time of the particle. */
    float RelativeTime = 0.0f;
    /** The previous position of the particle. */
    FVector OldPosition = FVector::ZeroVector;
    /** Value that remains constant over the lifetime of a particle. */
    // 보통 난수의 시드값으로 사용
    float ParticleId = 0.0f;
    /** The size of the particle. */
    FVector2D Size = FVector2D::OneVector;
    /** The rotation of the particle. */
    float Rotation = 0.0f;
    /** The sub-image index for the particle. */
    float SubImageIndex = 0.0f;
    /** The color of the particle. */
    FLinearColor Color = FLinearColor::White;
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


    /** Constructor */
    FDynamicEmitterReplayDataBase()
        : eEmitterType(DET_Unknown)
        , ActiveParticleCount(0)
        , ParticleStride(0)
        , Scale(FVector(1.0f))
    {
    }

    virtual ~FDynamicEmitterReplayDataBase()
    {
    }
};


struct FDynamicSpriteEmitterReplayDataBase : public FDynamicEmitterReplayDataBase
{
    //원레 머터리얼 인터페이스를 사용하나 UMaterial로 일단 퉁치기
    //UMaterialInterface*             MaterialInterface;
    UMaterial* Material;
    UParticleModuleRequired* RequiredModule;
    FVector2D PivotOffset;
    int32 MaxDrawCount;
    bool bUseLocalSpace;
};

struct FDynamicSpriteEmitterReplayData : public FDynamicSpriteEmitterReplayDataBase
{
    /** Constructor */
    FDynamicSpriteEmitterReplayData()
    {
    }

    /** Serialization */
    virtual void Serialize(FArchive& Ar)
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
    FDynamicEmitterDataBase() = default;

    virtual ~FDynamicEmitterDataBase()
    {
    }

    /** true if this emitter is currently selected */
    uint32 bSelected : 1;
    /** true if this emitter has valid rendering data */
    uint32 bValid : 1;

    int32 EmitterIndex;

    virtual const FDynamicEmitterReplayDataBase& GetSource() const = 0;
};

struct FDynamicSpriteEmitterDataBase : public FDynamicEmitterDataBase
{
    virtual int32 GetDynamicVertexStride() const = 0;

    void SortSpriteParticles();

    /**
 *	Sort the given sprite particles
 *
 *	@param	SorceMode			The sort mode to utilize (EParticleSortMode)
 *	@param	bLocalSpace			true if the emitter is using local space
 *	@param	ParticleCount		The number of particles
 *	@param	ParticleData		The actual particle data
 *	@param	ParticleStride		The stride between entries in the ParticleData array
 *	@param	ParticleIndices		Indirect index list into ParticleData
 *	@param	ViewProjection				The scene view being rendered
 *	@param	LocalToWorld		The local to world transform of the component rendering the emitter
 *	@param	ParticleOrder		The array to fill in with ordered indices
 */
    void SortSpriteParticles(int32 SortMode, bool bLocalSpace,
                             int32 ParticleCount, const uint8* ParticleData, int32 ParticleStride, const uint16* ParticleIndices,
                             const FMatrix* ViewProjection, const FMatrix& LocalToWorld, TArray<FParticleOrder>& ParticleOrder) const;

    void SortSpriteParticles(int32 SortMode, bool bLocalSpace,
                             TArray<FBaseParticle>& ParticleData,
                             const FMatrix* ViewProjection, const FMatrix& LocalToWorld, TArray<FParticleOrder>& ParticleOrder) const;
};

struct FDynamicSpriteEmitterData : public FDynamicSpriteEmitterDataBase
{
    virtual ~FDynamicSpriteEmitterData()
    {
        if (VertexBuffer) { VertexBuffer->Release(); VertexBuffer = nullptr; }
        if (IndexBuffer)  { IndexBuffer->Release();  IndexBuffer  = nullptr; }
    }
    virtual int32 GetDynamicVertexStride() const override
    {
        return sizeof(FParticleSpriteVertex);
    }

    /** Returns the source data for this particle system */
    virtual const FDynamicEmitterReplayDataBase& GetSource() const override
    {
        return Source;
    }

    /**
 *	Retrieve the vertex and (optional) index required to render this emitter.
 *	Render-thread only
 *
 *	@param	VertexData			The memory to fill the vertex data into
 *	@param	FillIndexData		The index data to fill in
 *	@param	ParticleOrder		The (optional) particle ordering to use
 *	@param	InCameraPosition	The position of the camera in world space.
 *	@param	InLocalToWorld		Transform from local to world space.
 *	@param	InstanceFactor		The factor to duplicate instances by.
 *
 *	@return	bool			true if successful, false if failed
 */
    bool GetVertexAndIndexData(void* VertexData, void* FillIndexData, TArray<FParticleOrder>* ParticleOrder,
                               const FVector& InCameraPosition, const FMatrix& InLocalToWorld, uint32 InstanceFactor) const;

    bool GetVertexAndIndexData(void* VertexData, void* FillIndexData, TArray<FParticleOrder>* ParticleOrder, TArray<FBaseParticle>& ParticleData,
                               const FVector& InCameraPosition, const FMatrix& InLocalToWorld, uint32 InstanceFactor) const;
    void Init(bool bInSelected);
    
    ID3D11Buffer* VertexBuffer   = nullptr;
    ID3D11Buffer* IndexBuffer    = nullptr;
    int32         VertexBufferSize = 0;
    int32         IndexBufferSize  = 0;

    FDynamicSpriteEmitterReplayData Source;
};


struct FDynamicMeshEmitterData : public FDynamicSpriteEmitterDataBase
{
    virtual int32 GetDynamicVertexStride() const override
    {
        return sizeof(FMeshParticleInstanceVertex);
    }
};

/*-----------------------------------------------------------------------------
 *	Particle dynamic data
 *	This is a copy of the particle system data needed to render the system in
 *	another thread.
 ----------------------------------------------------------------------------*/
class FParticleDynamicData
{
public:
    FParticleDynamicData()
        : DynamicEmitterDataArray()
    {
    }

    ~FParticleDynamicData()
    {
        ClearEmitterDataArray();
    }

    void ClearEmitterDataArray()
    {
        for (int32 Index = 0; Index < DynamicEmitterDataArray.Num(); Index++)
        {
            FDynamicEmitterDataBase* Data = DynamicEmitterDataArray[Index];
            delete Data;
        }
        DynamicEmitterDataArray.Empty();
    }

    /** The Current Emmitter we are rendering **/
    uint32 EmitterIndex;

    // Variables
    TArray<FDynamicEmitterDataBase*> DynamicEmitterDataArray;
};


FORCEINLINE FVector2D GetParticleSizeWithUVFlipInSign(const FBaseParticle& Particle, const FVector2D& ScaledSize)
{
    return FVector2D(
        Particle.BaseSize.X >= 0.0f ? ScaledSize.X : -ScaledSize.X,
        Particle.BaseSize.Y >= 0.0f ? ScaledSize.Y : -ScaledSize.Y);
}
