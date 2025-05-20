#include "ParticleModuleCollision.h"

#include "ParticleHelper.h"

UParticleModuleCollision::UParticleModuleCollision()
{
    bUpdateModule = true;
}

void UParticleModuleCollision::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{

}

void UParticleModuleCollision::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    BEGIN_UPDATE_LOOP
        // BEGIN_UPDATE_LOOP 매크로 덕분에
        //   i, CurrentIndex, Particle, ActiveParticles 등 변수가 모두 준비되어 있습니다.

        // 1) OldLoc → NewLoc
    FVector OldLoc = Particle.OldLocation;
    FVector NewLoc = Particle.OldLocation + Particle.Velocity * DeltaTime;

    // 2) 씬에 맞춰 단순 Plane 충돌 검사 (예시)
    FVector HitPoint, HitNormal;
    if (RaycastGround(OldLoc, NewLoc, HitPoint, HitNormal))
    {
        Particle.Location = HitPoint;
        // 속도 반사 & 감쇠
        FVector Ref = Particle.Velocity - HitNormal * 2.f * (Particle.Velocity.Dot(HitNormal));
        Particle.Velocity = Ref * BounceFactor + Particle.Velocity * (1.f - FrictionFactor);
        Particle.BaseVelocity = Particle.Velocity;
    }
    END_UPDATE_LOOP
}

void UParticleModuleCollision::InitializeDefaults()
{
}

bool UParticleModuleCollision::RaycastGround(const FVector& Start, const FVector& End, FVector& OutImpactPoint, FVector& OutNormal)
{
    // Plane: z=0, normal = (0,0,1)
    // Ray: P(t) = Start + t*(End-Start), t in [0,1]
    float dzStart = Start.Z;
    float dzEnd   = End.Z;
    if ((dzStart > 0 && dzEnd > 0) || (dzStart < 0 && dzEnd < 0))
        return false;  // 둘 다 같은 쪽에 있으면 충돌 없다

    float t = dzStart / (dzStart - dzEnd);
    OutImpactPoint = Start + (End - Start) * t;
    OutNormal       = FVector(0,0,1);
    return true;
}
