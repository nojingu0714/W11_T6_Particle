#include "ParticleSpriteEmitterInstance.h"
#include "Particles/ParticleEmitter.h"
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

FDynamicEmitterDataBase* FParticleSpriteEmitterInstance::GetDynamicData(bool bSelected)
{
    // It is valid for the LOD level to be NULL here!
    UParticleLODLevel* LODLevel = SpriteTemplate->GetCurrentLODLevel(this);

    if (IsDynamicDataRequired(LODLevel) == false || !bEnabled)
    {
        return NULL;
    }


    FDynamicSpriteEmitterData* NewEmitterData = new FDynamicSpriteEmitterData();

    // Now fill in the source data
    if (!FillReplayData(NewEmitterData->Source))
    {
        delete NewEmitterData;
        return NULL;
    }

    // Setup dynamic render data.  Only call this AFTER filling in source data for the emitter.
    NewEmitterData->Init(bSelected);

    return NewEmitterData;
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
