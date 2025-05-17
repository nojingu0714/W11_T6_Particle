#pragma once
#include "Container/Array.h"
#include "HAL/PlatformType.h"
#include "Math/Vector.h"

struct FBaseParticle;
class UParticleSystemComponent;
class UParticleLODLevel;
class UParticleEmitter;

struct FParticleEmitterInstances
{
    UParticleEmitter* SpriteTemplate;

    // Owner
    UParticleSystemComponent* Component;

    int32 CurrentLODLevelIndex;
    UParticleLODLevel* CurrentLODLevel;

    TArray<FBaseParticle*> BaseParticles;
    
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
    void InitParameters(UParticleEmitter* InEmitter, UParticleSystemComponent* InComponent);
    void Init();
    void Tick(float DeltaTime);
    void SetupEmitterDuration();
    void SpawnParticles( int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity, struct FParticleEventInstancePayload* EventPayload );
    void SpawnParticles( int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity);

    void KillParticle(int32 Index);
};


