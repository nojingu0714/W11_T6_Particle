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
    ReqInstanceBytes = sizeof(FCollsionParticleInfo);
    bUpdateModule = true;
}

void UParticleModuleCollision::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{

}

void UParticleModuleCollision::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    BEGIN_UPDATE_LOOP

    FCollsionParticleInfo* CollisionParticleBase = reinterpret_cast<FCollsionParticleInfo*>(const_cast<uint8*>(ParticleBase) + Owner->ModulePayloadOffsetMap[this]);
        
    FVector OldLoc = Particle.OldLocation;
    FVector NewLoc = Particle.OldLocation + Particle.Velocity * DeltaTime;

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
            Particle.BaseVelocity = Ref * BounceFactor + Particle.Velocity * (1.f - FrictionFactor);
            CollisionParticleBase->Collisions++;
            if (CollisionParticleBase->Collisions>= MaxCollisions.GetValue(0))
            {
                if (Response == EPCC_Kill)
                    Particle.RelativeTime = 1.0f;
                else if (Response == EPCC_Freez)
                {
                    Particle.Velocity = FVector(0, 0, 0);
                    Particle.BaseVelocity = FVector(0, 0, 0);
                }
                else
                {
                    Particle.RelativeTime = 1.0f;
                }
            }
             break; // 여러 AABB 중 하나만 충돌 처리
         }
        
    }
    END_UPDATE_LOOP
}

void UParticleModuleCollision::InitializeDefaults()
{
    
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
