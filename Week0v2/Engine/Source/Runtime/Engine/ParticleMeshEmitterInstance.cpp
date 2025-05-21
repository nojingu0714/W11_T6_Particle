#include "ParticleMeshEmitterInstance.h"

#include "ParticleHelper.h"
#include "Engine/FLoaderOBJ.h"
#include "Particles/ParticleEmitter.h"

FParticleMeshEmitterInstance::FParticleMeshEmitterInstance()
{
    // TODO 테스트 코드

    FManagerOBJ::CreateStaticMesh("Assets/apple_mid.obj");
    SetStaticMesh(FManagerOBJ::GetStaticMesh(L"apple_mid.obj"));
    // FManagerOBJ::CreateStaticMesh("Assets/Primitives/Capsule.obj");
    // SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Capsule.obj"));
}

FParticleMeshEmitterInstance::~FParticleMeshEmitterInstance()
{
}

FDynamicEmitterReplayDataBase* FParticleMeshEmitterInstance::GetReplayData()
{
    if (ActiveParticles <= 0 || !bEnabled)
    {
        return nullptr;
    }
    FDynamicMeshEmitterReplayData* NewEmitterReplayData = new FDynamicMeshEmitterReplayData();
    if( NewEmitterReplayData == nullptr )
        return nullptr; 

    if( !FillReplayData( *NewEmitterReplayData ) )
    {
        delete NewEmitterReplayData;
        return NULL;
    }

    return NewEmitterReplayData;
}

FDynamicEmitterDataBase* FParticleMeshEmitterInstance::GetDynamicData(bool bSelected)
{
    // It is valid for the LOD level to be NULL here!
    UParticleLODLevel* LODLevel = SpriteTemplate->GetCurrentLODLevel(this);

    if (IsDynamicDataRequired(LODLevel) == false || !bEnabled)
    {
        return NULL;
    }


    FDynamicMeshEmitterData* NewEmitterData = new FDynamicMeshEmitterData();

    // Now fill in the source data
    if (!FillReplayData(NewEmitterData->Source))
    {
        delete NewEmitterData;
        return NULL;
    }

    NewEmitterData->StaticMesh = StaticMesh;
    NewEmitterData->bSelected = bSelected;

    // Setup dynamic render data.  Only call this AFTER filling in source data for the emitter.
    // NewEmitterData->Init(bSelected);

    return NewEmitterData;
}

bool FParticleMeshEmitterInstance::FillReplayData(FDynamicEmitterReplayDataBase& DynamicEmitterReplayDataBase)
{
    if (ActiveParticles <= 0)
    {
        return false;
    }

    // Call parent implementation first to fill in common particle source data
    if( !FParticleEmitterInstance::FillReplayData( DynamicEmitterReplayDataBase ) )
    {
        return false;
    }

    DynamicEmitterReplayDataBase.eEmitterType = DET_Mesh;

    FDynamicMeshEmitterReplayData* NewReplayData =
    static_cast< FDynamicMeshEmitterReplayData* >( &DynamicEmitterReplayDataBase );

    NewReplayData->Material = GetCurrentMaterial();

    return true;
}
