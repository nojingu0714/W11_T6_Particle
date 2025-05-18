#include "ParticleModuleVelocity.h"
#include "Engine/Source/Runtime/Engine/ParticleEmitterInstances.h"
#include "Engine/Source/Runtime/Engine/ParticleHelper.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"

void UParticleModuleVelocity::InitializeDefaults()
{
    StartVelocity = FSimpleVectorDistribution(FSimpleFloatDistribution(0.0f),
                                              FSimpleFloatDistribution(0.0f),
                                              FSimpleFloatDistribution(10.0f));

    // 방사는 조심할것, 뷰어쪽에서 Owner의 Component 위치를 가져오는 것에 추가처리가 필요해보임
    StartVelocityRadial = FSimpleVectorDistribution(FSimpleFloatDistribution(0.0f),
                                                    FSimpleFloatDistribution(0.0f),
                                                    FSimpleFloatDistribution(0.0f));
}

void UParticleModuleVelocity::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    SPAWN_INIT;
    {
        FVector Vel = StartVelocity.GetValue(Owner->EmitterTime);
        // Particle의 Location이 World기준인지 Model 기준인지 확인 필요
        FVector FromOrigin = (Particle.Location - Owner->Component->GetWorldLocation());
        FVector OwnerScale(1.0f);
        if (Owner->Component) 
        {
            OwnerScale = Owner->Component->GetWorldScale();
        }

        Vel = Vel * OwnerScale;
        Vel += FromOrigin * StartVelocityRadial.GetValue(Owner->EmitterTime) * OwnerScale;
        Particle.Velocity += Vel;
        Particle.BaseVelocity += Vel;
    }
}
