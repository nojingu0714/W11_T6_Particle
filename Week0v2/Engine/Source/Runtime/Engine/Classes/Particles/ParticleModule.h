#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

struct FBaseParticle;
struct FParticleEmitterInstance;
class UParticleEmitter;

enum EModuleType : int
{
    /** General - all emitter types can use it			*/
    EPMT_General,
    /** TypeData - TypeData modules						*/
    EPMT_TypeData,
    /** Beam - only applied to beam emitters			*/
    EPMT_Beam ,
    /** Trail 	*/
    EPMT_Trail,
    /** Spawn 	*/
    EPMT_Spawn,
    /** Requir	*/
    EPMT_Requi,
    /** Event 	*/
    EPMT_Event,
    /** Light	*/
    EPMT_Light,
    /** SubUV	*/
    EPMT_SubUV,
    EPMT_MAX,
};

class UParticleModule : public UObject
{
    DECLARE_CLASS(UParticleModule, UObject)
public:
    UParticleModule();
    ~UParticleModule();

private:    // 8비트 중 마지막 1비트만을 활용하여 bool 변수처럼 활용
    // 8비트짜리 변수라는 점에 대하여 유의 필요
    // 아래 변수를 통해 Module Pool 관리하여 최적화하는 것으로 보임
    
    /** If true, the module performs operations on particles during Spawning		*/
    uint8 bSpawnModule : 1;

    /** If true, the module performs operations on particles during Updating		*/
    uint8 bUpdateModule : 1;

    /** If true, the module performs operations on particles during final update	*/
    uint8 bFinalUpdateModule : 1;

    /** If true, the module performs operations on particles during update and/or final update for GPU emitters*/
    uint8 bUpdateForGPUEmitter : 1;

    /** If true, the module displays FVector curves as colors						*/
    uint8 bCurvesAsColor : 1;

    /** If true, the module should render its 3D visualization helper				*/
    uint8 b3DDrawMode : 1;

    /** If true, the module supports rendering a 3D visualization helper			*/
    uint8 bSupported3DDrawMode : 1;

    /** If true, the module is enabled												*/
    uint8 bEnabled : 1;

    /** If true, the module has had editing enabled on it							*/
    uint8 bEditable : 1;

    /** If true, the module supports RandomSeed setting */
    uint8 bSupportsRandomSeed : 1;

    // TODO: 아래 함수는 프로퍼티가 변경되었을 때 호출되는 요소 UI 구현할 때 구현 필요
    //virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;


    virtual void CompileModule(struct FParticleEmitterBuildInfo& EmitterInfo);
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase);
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    virtual void FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    // TODO UParticleModuleTypeDataBase 구현하면 활성화
    //virtual uint32 RequiredBytes(UParticleModuleTypeDataBase* TypeData);
    virtual uint32	RequiredBytesPerInstance();
    virtual uint32	PrepPerInstanceBlock(FParticleEmitterInstance* Owner, void* InstData);
    virtual void SetToSensibleDefaults(UParticleEmitter* Owner);
    virtual EModuleType	GetModuleType() const { return EPMT_General; }
};

/*
UParticleModuleRequired: Emitter에 필수적인 설정을 포함하며, 파티클의 기본 속성들을 정의합니다.
UParticleModuleSpawn: 파티클의 생성 빈도와 수량을 제어합니다.
UParticleModuleLifetime: 파티클의 수명을 설정합니다.
UParticleModuleLocation: 파티클의 초기 위치를 결정합니다.
UParticleModuleVelocity: 파티클의 초기 속도와 방향을 설정합니다.
UParticleModuleColor: 파티클의 색상을 정의하며, 시간에 따른 색상 변화를 설정할 수 있습니다.
UParticleModuleSize: 파티클의 크기를 설정합니다.
*/
