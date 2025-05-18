#pragma once
#include "Container/Array.h"
#include "HAL/PlatformType.h"
#include "Math/Vector.h"

class UMaterialInterface;
struct FDynamicEmitterReplayDataBase;
struct FDynamicEmitterDataBase;
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

    /** The material to render this instance with.						*/
    UMaterialInterface* CurrentMaterial;
    
    // Emitter 시작 후 총 누적 시간 
    float EmitterTime = 0.0f;
    bool bEnabled= true;
    FVector2D PivotOffset;

    FParticleEmitterInstance();
    
    void InitParameters(UParticleEmitter* InEmitter, UParticleSystemComponent* InComponent);
    void Init();
    void Tick(float DeltaTime);
    void SetupEmitterDuration();
    void SpawnParticles( int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity, FParticleEventInstancePayload* EventPayload );

    void PreSpawn(FBaseParticle& Particle, const FVector& InitLocation, const FVector& InitVelocity);
    void PostSpawn(FBaseParticle* Particle, float InterpolationPercentage, float SpawnTime);
    void KillParticle(int32 Index);
    void KillParticle(FBaseParticle* Particle);
    void KilParticles();

    /**
    * Get the current material to render with.
    */
     UMaterialInterface* GetCurrentMaterial();
    
    /**
    * GetDynamicData 유효성에 대한 몇 가지 일반적인 값을 확인합니다.
    *
    * GetDynamicData가 계속되어야 하는 경우 true를 반환하고, NULL을 반환해야 하는 경우 false를 반환합니다.
    */
    virtual bool IsDynamicDataRequired(UParticleLODLevel* InCurrentLODLevel);
    /**
    *	Retrieves the dynamic data for the emitter
    */
    virtual FDynamicEmitterDataBase* GetDynamicData(bool bSelected)
    {
        return NULL;
    }

    /**
    * Captures dynamic replay data for this particle system.
    *
    * @param	OutData		[Out] Data will be copied here
    *
    * @return Returns true if successful
    */
    virtual bool FillReplayData( FDynamicEmitterReplayDataBase& OutData );

};


/*-----------------------------------------------------------------------------
    ParticleSpriteEmitterInstance
-----------------------------------------------------------------------------*/
struct FParticleSpriteEmitterInstance : public FParticleEmitterInstance
{
    /** Constructor	*/
    FParticleSpriteEmitterInstance();

    /** Destructor	*/
    virtual ~FParticleSpriteEmitterInstance();

    /**
     *	Retrieves the dynamic data for the emitter
     */
    virtual FDynamicEmitterDataBase* GetDynamicData(bool bSelected) override;

    
    virtual bool FillReplayData( FDynamicEmitterReplayDataBase& OutData );
    
};



