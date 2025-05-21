#include "ParticleModuleCollision.h"

#include "LaunchEngineLoop.h"
#include "ParticleHelper.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"
#include "Engine/FEditorStateManager.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/Casts.h"
#include "UObject/UObjectIterator.h"

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

    for (UStaticMeshComponent* StaticComp : TObjectRange<UStaticMeshComponent>())
    {
        if (Cast<UGizmoBaseComponent>(StaticComp))
            continue;
        
        FBoundingBox Box = StaticComp->GetBoundingBox();
        FVector BoxMin = Box.min, BoxMax = Box.max;
        FMatrix WMat = StaticComp->GetWorldMatrix();
        BoxMin = WMat.TransformPosition(BoxMin);
        BoxMax = WMat.TransformPosition(BoxMax);
        
        if (SphereIntersectsAABB(NewLoc,CollisionRadius, BoxMin,BoxMax))
        {
            FVector Closest(
                FMath::Clamp(NewLoc.X, BoxMin.X, BoxMax.X),
                FMath::Clamp(NewLoc.Y, BoxMin.Y, BoxMax.Y),
                FMath::Clamp(NewLoc.Z, BoxMin.Z, BoxMax.Z)
            );
             // (d) 법선 계산
             FVector Normal = ComputeBoxNormalAtPoint(Closest, BoxMin, BoxMax);

             // (e) 위치 보정: 박스 밖으로 꺼내주기
             Particle.Location = Particle.OldLocation;

             // (f) 속도 반사 & 감쇠
             FVector Ref = Particle.Velocity - Normal * 2.f * (Particle.Velocity.Dot(Normal));
             Particle.BaseVelocity = Ref;

             break; // 여러 AABB 중 하나만 충돌 처리
         }
        
    }
    
    // if (RaycastGround(OldLoc, NewLoc, HitPoint, HitNormal))
    // {
    //     Particle.Location = Particle.OldLocation;
    //     // 속도 반사 & 감쇠
    //     FVector Ref = Particle.Velocity - HitNormal * 2.f * (Particle.Velocity.Dot(HitNormal));
    //     Particle.BaseVelocity = Ref;//* BounceFactor + Particle.Velocity * (1.f - FrictionFactor);
    //     // Particle.BaseVelocity = Particle.Velocity;
    //     // Particle.BaseVelocity.Z =2.0f;
    // }
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

bool UParticleModuleCollision::SphereIntersectsAABB(const FVector& Center, float Radius, const FVector& BoxMin, const FVector& BoxMax)
{
    // AABB 에서 구의 중심에 가장 가까운 점을 찾는다.
    float x = FMath::Clamp(Center.X, BoxMin.X, BoxMax.X);
    float y = FMath::Clamp(Center.Y, BoxMin.Y, BoxMax.Y);
    float z = FMath::Clamp(Center.Z, BoxMin.Z, BoxMax.Z);

    // 그 점과 구 중심 사이의 거리 제곱이 반지름^2 이하이면 교차
    FVector ClosestPoint(x, y, z);
    FVector Delta = ClosestPoint - Center;
    float dist2 = Delta.X*Delta.X + Delta.Y*Delta.Y + Delta.Z*Delta.Z;
    return dist2 <= Radius*Radius;
}

FVector UParticleModuleCollision::ComputeBoxNormalAtPoint(const FVector& Point, const FVector& BoxMin, const FVector& BoxMax)
{
    // 각 축별로 얼마나 가까운지 비교해서 면을 고른다.
    float dxMin = FMath::Abs(Point.X - BoxMin.X);
    float dxMax = FMath::Abs(Point.X - BoxMax.X);
    float dyMin = FMath::Abs(Point.Y - BoxMin.Y);
    float dyMax = FMath::Abs(Point.Y - BoxMax.Y);
    float dzMin = FMath::Abs(Point.Z - BoxMin.Z);
    float dzMax = FMath::Abs(Point.Z - BoxMax.Z);

    float minDist = dxMin;
    FVector normal(-1,0,0);

    if (dxMax < minDist) { minDist = dxMax; normal = FVector(1,0,0); }
    if (dyMin < minDist) { minDist = dyMin; normal = FVector(0,-1,0); }
    if (dyMax < minDist) { minDist = dyMax; normal = FVector(0,1,0); }
    if (dzMin < minDist) { minDist = dzMin; normal = FVector(0,0,-1); }
    if (dzMax < minDist) { minDist = dzMax; normal = FVector(0,0,1); }

    return normal;
}
