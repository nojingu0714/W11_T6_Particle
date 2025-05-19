#pragma once

#include "Engine/Classes/Particles/ParticleModule.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"

class UParticleModuleColor : public UParticleModule 
{
    DECLARE_CLASS(UParticleModuleColor, UParticleModule)

public:
    UParticleModuleColor();
    ~UParticleModuleColor();

    // 언리얼에서의 UParticleModuleColor는 시작 Color만 정하므로 
    // 코스에서 요구한 시간이 변하는 것에 따른 색상 변화를 위해
    // UParticleModuleColorScaleOverLife을 참고했음

    FSimpleVectorDistribution ColorScaleOverLife;
    FSimpleFloatDistribution AlphaScaleOverLife;

    void InitializeDefaults();

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, FBaseParticle* ParticleBase, float DeltaTime) override;
};