#include "ParticleSpriteEmitterInstance.h"

#include "ParticleHelper.h"

FDynamicEmitterReplayDataBase* FParticleSpriteEmitterInstance::GetReplayData()
{
    if (ActiveParticles <= 0 || !bEnabled)
    {
        return nullptr;
    }
    FDynamicEmitterReplayDataBase* NewEmitterReplayData = new FDynamicSpriteEmitterReplayData();
    if( NewEmitterReplayData == nullptr )
         return nullptr; 

    if( !FillReplayData( *NewEmitterReplayData ) )
    {
        delete NewEmitterReplayData;
        return NULL;
    }

    return NewEmitterReplayData;
}

FDynamicEmitterDataBase* FParticleSpriteEmitterInstance::GetDynamicData(bool bSelected, ERHIFeatureLevel::Type InFeatureLevel)
{
    return FParticleEmitterInstance::GetDynamicData(bSelected, InFeatureLevel);
}

bool FParticleSpriteEmitterInstance::FillReplayData(FDynamicEmitterReplayDataBase& OutData)
{
    if (ActiveParticles <= 0)
    {
        return false;
    }

    // Call parent implementation first to fill in common particle source data
    if( !FParticleEmitterInstance::FillReplayData( OutData ) )
    {
        return false;
    }

    OutData.eEmitterType = DET_Sprite;

    FDynamicSpriteEmitterReplayData* NewReplayData =
    static_cast< FDynamicSpriteEmitterReplayData* >( &OutData );

    NewReplayData->Material = GetCurrentMaterial();

    return true;
}
