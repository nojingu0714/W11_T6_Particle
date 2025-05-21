#pragma once

#include "ParticleModuleCollisionBase.h"
#include "Engine/Source/Runtime/Core/Math/Distribution.h"

struct FCollsionParticleInfo
{
    int32 Collisions = 0;
};

class UParticleModuleCollision : public UParticleModuleCollisionBase
{
    DECLARE_CLASS(UParticleModuleCollision, UParticleModuleCollisionBase)
public:
    UParticleModuleCollision();
    ~UParticleModuleCollision() = default;


    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase);
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    /**
     *	How much to `slow' the velocity of the particle after a collision.
     *	Value is obtained using the EmitterTime at particle spawn.
     */
    FSimpleVectorDistribution DampingFactor;

    /**
     *	How much to `slow' the rotation of the particle after a collision.
     *	Value is obtained using the EmitterTime at particle spawn.
     */
     FSimpleVectorDistribution DampingFactorRotation;

    /**
     *	The maximum number of collisions a particle can have. 
     *  Value is obtained using the EmitterTime at particle spawn. 
     */
     FSimpleFloatDistribution MaxCollisions= 3;

    // 파티클 크기
    float CollisionRadius= 1.0f;

    // 충돌시 반사 여부
    bool bBounce;

    // 마지막 충돌에 대한 응답
    EParticleCollisionComplete Response;


    /** 반사 강도 (0=완전흡수, 1=완전반사) */
    float BounceFactor = 1.0f;
    /** 마찰 감쇠 (0=무감쇠, 1=완전정지) */
    float FrictionFactor = 0.1f;
    void InitializeDefaults();
    bool SphereIntersectsAABB(
    const FVector& Center,
    float Radius,
    const FVector& BoxMin,
    const FVector& BoxMax);
    FVector ComputeBoxNormalAtPoint(
    const FVector& Point,
    const FVector& BoxMin,
    const FVector& BoxMax);
};
