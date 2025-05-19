#pragma once
#include "FXSystemComponent.h"

class FParticleDynamicData;
struct FParticleEmitterInstance;
struct FDynamicEmitterDataBase;
class UParticleSystem;

class UParticleSystemComponent : public UFXSystemComponent
{
    DECLARE_CLASS(UParticleSystemComponent, UFXSystemComponent)
public:
    UParticleSystemComponent();
    ~UParticleSystemComponent() = default;


    void BeginPlay() override;
    void TickComponent(float DeltaTime) override;
    void InitParticles();
    void ResetSystem();

    /**
 * 이 프레임에서 파티클 시스템을 렌더링하기 위한 동적 파티클 데이터를 생성합니다.  이 함수는
 *는 정기적으로 시뮬레이션되는 파티클의 동적 데이터 생성을 처리하지만, 파티클 재생 데이터의 캡처
 * 및 파티클 리플레이 데이터의 재생도 처리합니다.
 * @param InFeatureLevel - 관련 셰이더 피처 레벨입니다.
 * 반환 - 이 프레임을 렌더링할 동적 데이터를 반환합니다.
 */
    void CreateDynamicData();

    void ClearEmitterDataArray();

    
    TArray<FParticleEmitterInstance*> EmitterInstances;
    UParticleSystem* Template;

    void UpdateDynamicData();
    TArray<FDynamicEmitterDataBase*> EmitterRenderData;

    
};

