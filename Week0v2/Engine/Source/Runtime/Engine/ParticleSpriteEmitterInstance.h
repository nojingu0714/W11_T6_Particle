#pragma once
#include "ParticleEmitterInstance.h"

struct FParticleSpriteEmitterInstance : public FParticleEmitterInstance
{
    FParticleSpriteEmitterInstance();
    virtual ~FParticleSpriteEmitterInstance();
public:
    virtual FDynamicEmitterReplayDataBase* GetReplayData() override;
    virtual FDynamicEmitterDataBase* GetDynamicData(bool bSelected, ERHIFeatureLevel::Type InFeatureLevel) override;
    virtual bool FillReplayData(FDynamicEmitterReplayDataBase& DynamicEmitterReplayDataBase) override;

};
