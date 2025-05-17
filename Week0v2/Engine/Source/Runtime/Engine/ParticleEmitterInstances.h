#pragma once
#include "HAL/PlatformType.h"
#include "Math/Vector.h"
#include "Engine/Source/Runtime/Core/Container/Array.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"
#include "Engine/Source/Runtime/Core/Math/Transform.h"

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

    void SpawnParticles( int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity, struct FParticleEventInstancePayload* EventPayload )
    {
        // for (int32 i = 0; i < Count; i++)
        // {
        //     DECLARE_PARTICLE_PTR
        //     PreSpawn(Particle, InitialLocation, InitialVelocity);
        //
        //     for (int32 ModuleIndex = 0; ModuleIndex < LODLevel->SpawnModules.Num(); ModuleIndex++)
        //     {
        //         ...
        //     }
        //
        //     PostSpawn(Particle, Interp, SpawnTime);
        // }
    }
    void KillParticle(int32 Index);
};


struct FParticleEmitterBuildInfo
{
    /** The required module. */
    class UParticleModuleRequired* RequiredModule;
    /** The spawn module. */
    class UParticleModuleSpawn* SpawnModule;
    /** The spawn-per-unit module. */
    class UParticleModuleSpawnPerUnit* SpawnPerUnitModule;
    /** List of spawn modules that need to be invoked at runtime. */
    TArray<class UParticleModule*> SpawnModules;

    // 언리얼의 Distribution은 LookUp Table을 필수적으로 활용해야해서
    // 일단 간단 버전 Distribution을 만들어서
    // 분포를 매번 계산하도록 하고 시간이 되면 LookUP Table을 사용하여 최적화 시키도록 하겠음

    /** The accumulated orbit offset. */
    FSimpleVectorDistribution OrbitOffset;
    /** The accumulated orbit initial rotation. */
    FSimpleVectorDistribution OrbitInitialRotation;
    /** The accumulated orbit rotation rate. */
    FSimpleVectorDistribution OrbitRotationRate;

    /** The color scale of a particle over time. */
    FSimpleVectorDistribution ColorScale;
    /** The alpha scale of a particle over time. */
    FSimpleFloatDistribution AlphaScale;

    /** An additional color scale for allowing parameters to be used for ColorOverLife modules. */
    FSimpleVectorDistribution DynamicColor;
    /** An additional alpha scale for allowing parameters to be used for ColorOverLife modules. */
    FSimpleFloatDistribution DynamicAlpha;

    /** An additional color scale for allowing parameters to be used for ColorScaleOverLife modules. */
    FSimpleVectorDistribution DynamicColorScale;
    /** An additional alpha scale for allowing parameters to be used for ColorScaleOverLife modules. */
    FSimpleFloatDistribution DynamicAlphaScale;

    /** How to scale a particle's size over time. */
    FSimpleVectorDistribution SizeScale;
    /** The maximum size of a particle. */
    FVector2D MaxSize;
    /** How much to scale a particle's size based on its speed. */
    FVector2D SizeScaleBySpeed;
    /** The maximum amount by which to scale a particle based on its speed. */
    FVector2D MaxSizeScaleBySpeed;

    /** The sub-image index over the particle's life time. */
    FSimpleFloatDistribution SubImageIndex;

    /** Drag coefficient. */
    FSimpleFloatDistribution DragCoefficient;
    /** Drag scale over life. */
    FSimpleFloatDistribution DragScale;

    /** Enable collision? */
    bool bEnableCollision;
    /** How particles respond to collision. */
    //uint8 CollisionResponse;
    //uint8 CollisionMode;
    ///** Radius scale applied to friction. */
    //float CollisionRadiusScale;
    ///** Bias applied to the collision radius. */
    //float CollisionRadiusBias;
    ///** Factor reflection spreading cone when colliding. */
    //float CollisionRandomSpread;
    ///** Random distribution across the reflection spreading cone when colliding. */
    //float CollisionRandomDistribution;
    ///** Friction. */
    //float Friction;
    ///** Collision damping factor. */
    //FSimpleFloatDistribution Resilience;
    ///** Collision damping factor scale over life. */
    //FSimpleFloatDistribution ResilienceScaleOverLife;

    /** Location of a point source attractor. */
    FVector PointAttractorPosition;
    /** Radius of the point source attractor. */
    float PointAttractorRadius;
    /** Strength of the point attractor. */
    FSimpleFloatDistribution PointAttractorStrength;

    /** The per-particle vector field scale. */
    FSimpleFloatDistribution VectorFieldScale;
    /** The per-particle vector field scale-over-life. */
    FSimpleFloatDistribution VectorFieldScaleOverLife;
    /** Global vector field scale. */
    float GlobalVectorFieldScale;
    /** Global vector field tightness. */
    float GlobalVectorFieldTightness;

    /** Local vector field. */
    class UVectorField* LocalVectorField;
    /** Local vector field transform. */
    FTransform LocalVectorFieldTransform;
    /** Local vector field intensity. */
    float LocalVectorFieldIntensity;
    /** Tightness tweak for local vector fields. */
    float LocalVectorFieldTightness;
    /** Minimum initial rotation applied to local vector fields. */
    FVector LocalVectorFieldMinInitialRotation;
    /** Maximum initial rotation applied to local vector fields. */
    FVector LocalVectorFieldMaxInitialRotation;
    /** Local vector field rotation rate. */
    FVector LocalVectorFieldRotationRate;

    /** Constant acceleration to apply to particles. */
    FVector ConstantAcceleration;

    /** The maximum lifetime of any particle that will spawn. */
    float MaxLifetime;
    /** The maximum rotation rate of particles. */
    float MaxRotationRate;
    /** The estimated maximum number of particles for this emitter. */
    int32 EstimatedMaxActiveParticleCount;

    int32 ScreenAlignment;

    /** An offset in UV space for the positioning of a sprites vertices. */
    FVector2D PivotOffset;

    /** If true, local vector fields ignore the component transform. */
    uint32 bLocalVectorFieldIgnoreComponentTransform : 1;
    /** Tile vector field in x axis? */
    uint32 bLocalVectorFieldTileX : 1;
    /** Tile vector field in y axis? */
    uint32 bLocalVectorFieldTileY : 1;
    /** Tile vector field in z axis? */
    uint32 bLocalVectorFieldTileZ : 1;
    /** Use fix delta time in the simulation? */
    uint32 bLocalVectorFieldUseFixDT : 1;

    uint32 bUseVelocityForMotionBlur : 1;

    /** Particle alignment overrides */
    uint32 bRemoveHMDRoll : 1;
    float MinFacingCameraBlendDistance;
    float MaxFacingCameraBlendDistance;

    /** Default constructor. */
    FParticleEmitterBuildInfo();
};