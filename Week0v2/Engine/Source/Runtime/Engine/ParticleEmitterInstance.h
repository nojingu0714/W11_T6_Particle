#pragma once
#include "Container/Array.h"
#include "HAL/PlatformType.h"
#include "Math/Vector.h"
#include "UserInterface/Debug/DebugViewModeHelpers.h"

class UMaterial;
struct FDynamicEmitterDataBase;
struct FDynamicEmitterReplayDataBase;
class UParticleModule;
class UParticleModuleRequired;
class UParticleModuleSpawn;
struct FParticleEventInstancePayload;
struct FBaseParticle;
class UParticleSystemComponent;
class UParticleLODLevel;
class UParticleEmitter;

struct FParticleEmitterInstance
{
    UParticleEmitter* SpriteTemplate;
    // Owner
    UParticleSystemComponent* Component;

    int32 CurrentLODLevelIndex;
    UParticleLODLevel* CurrentLODLevel;

    TArray<FBaseParticle*> BaseParticles;
    TArray<UParticleModuleSpawn*> SpawnModules;
    TArray<UParticleModule*> UpdateModules;
    TArray<FBaseParticle*> DeadParticles;
    
    UParticleModuleRequired* RequiredModule;
    /** Pointer to the particle data array.                             */
    uint8* ParticleData;
    /** Pointer to the particle index array.                            */
    uint16* ParticleIndices;
    /** Pointer to the instance data array.                             */
    uint8* InstanceData;
    /** The size of the Instance data array.                            */
    int32 InstancePayloadSize;
    /** The offset to the particle data.                                */
    int32 PayloadOffset;
    /** The total size of a particle (in bytes).                        */
    int32 ParticleSize;
    /** The stride between particles in the ParticleData array.         */
    int32 ParticleStride;
    /** The number of particles currently active in the emitter.        */
    int32 ActiveParticles;
    /** Monotonically increasing counter. */
    uint32 ParticleCounter;
    /** The maximum number of active particles that can be held in 
     *  the particle data array.
     */
    int32 MaxActiveParticles;
    /** The fraction of time left over from spawning.                   */

    /** The previous location of the instance.							*/
    FVector OldLocation;
    // Emitter 시작 후 총 누적 시간 
    float EmitterTime = 0.0f;
    bool bEnabled= true;

    /** The sort mode to use for this emitter as specified by artist.	*/
    int32 SortMode;
    
    FParticleEmitterInstance();
    
    void InitParameters(UParticleEmitter* InEmitter, UParticleSystemComponent* InComponent);
    void Init();
    virtual void Tick(float DeltaTime);
    void SetupEmitterDuration();
    void SpawnParticles( int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity, FParticleEventInstancePayload* EventPayload );

    void PreSpawn(FBaseParticle& Particle, const FVector& InitLocation, const FVector& InitVelocity);
    void PostSpawn(FBaseParticle* Particle, float InterpolationPercentage, float SpawnTime);
    void KillParticle(int32 Index);
    void KilParticles();

    virtual FDynamicEmitterReplayDataBase* GetReplayData();
    virtual FDynamicEmitterDataBase* GetDynamicData(bool bSelected, ERHIFeatureLevel::Type InFeatureLevel)
    {
        return NULL;
    }
    // Replyay Data를 채워주는 역할
    virtual bool FillReplayData(FDynamicEmitterReplayDataBase& DynamicEmitterReplayDataBase);
    virtual UMaterial* GetCurrentMaterial();

};



