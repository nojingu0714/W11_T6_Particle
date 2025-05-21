#pragma once
#include "ParticleEmitterInstance.h"

class UStaticMesh;

struct FParticleMeshEmitterInstance : public FParticleEmitterInstance
{
    FParticleMeshEmitterInstance();
    virtual ~FParticleMeshEmitterInstance();
public:
    virtual FDynamicEmitterReplayDataBase* GetReplayData() override;
    virtual FDynamicEmitterDataBase* GetDynamicData(bool bSelected) override;
    virtual bool FillReplayData(FDynamicEmitterReplayDataBase& DynamicEmitterReplayDataBase) override;

    void SetStaticMesh(UStaticMesh* InStaticMesh) { StaticMesh = InStaticMesh; }

    UStaticMesh* StaticMesh;
};
