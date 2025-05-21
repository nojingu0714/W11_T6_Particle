#include "ParticleModule.h"
#include "Engine/ParticleEmitterInstance.h"
#include "Engine/ParticleHelper.h"
#include "Engine/Classes/Particles/ParticleEmitter.h"

UParticleModule::UParticleModule()
{
    bSupported3DDrawMode = false;
    b3DDrawMode = false;
    bEnabled = true;
    bEditable = true;
    bSupportsRandomSeed = false;
    bUpdateForGPUEmitter = false;
}

UParticleModule::~UParticleModule()
{
}

void UParticleModule::CompileModule(FParticleEmitterBuildInfo& EmitterInfo)
{
    if (bSpawnModule) 
    {
        EmitterInfo.SpawnModules.Add(this);
    }

}

void UParticleModule::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{

}

void UParticleModule::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{

}

void UParticleModule::Update(FParticleEmitterInstance* Owner, int32 Offset, FBaseParticle* Particle, float DeltaTime)
{
    
}

void UParticleModule::FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{

}

//uint32 UParticleModule::RequiredBytes(UParticleModuleTypeDataBase* TypeData)
//{
//    return 0;
//}

uint32 UParticleModule::RequiredBytesPerInstance()
{
    return 0;
}

uint32 UParticleModule::PrepPerInstanceBlock(FParticleEmitterInstance* Owner, void* InstData)
{
    return 0xffffffff;
}

void UParticleModule::SetToSensibleDefaults(UParticleEmitter* Owner)
{
    // The default implementation does nothing...
}

uint32 UParticleModule::RequiredBytes()
{
    return ReqInstanceBytes;
}
