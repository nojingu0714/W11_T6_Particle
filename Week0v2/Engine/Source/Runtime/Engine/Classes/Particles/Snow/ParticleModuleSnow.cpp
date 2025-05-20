#include "ParticleModuleSnow.h"
#include "Engine/Source/Runtime/Engine/ParticleHelper.h"
#include "Engine/Source/Runtime/Engine/ParticleEmitterInstance.h"

UParticleModuleSnow::UParticleModuleSnow()
{
    bUpdateModule = true;
}

void UParticleModuleSnow::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    BEGIN_UPDATE_LOOP;

    // 초기화 (첫 프레임)
    if (Particle.RelativeTime < 0.01f)
    {
        // 넓은 범위에서 시작 (XY 평면에 분포)
        float randX = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * 80.0f;  // -80~80 범위
        float randY = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * 40.0f;  // -40~40 범위 (깊이)

        // Z축이 이제 높이 (위쪽)
        float startZ = 30.0f + (static_cast<float>(rand()) / RAND_MAX * 10.0f);  // 30~40 범위
        Particle.Location = FVector(randX, randY, startZ);

        // 낙하 속도 (-Z 방향으로)
        float fallSpeed = 1.5f + (static_cast<float>(rand()) / RAND_MAX * 1.0f);  // 1.5~2.5 범위
        Particle.BaseVelocity = FVector(0.0f, 0.0f, -fallSpeed);  // Z축 아래 방향으로

        // 각 파티클의 흔들림 특성 저장
        Particle.Flags = rand();
    }

    // 기본 Z축 낙하
    Particle.Velocity.Z = Particle.BaseVelocity.Z;

    // 약간의 좌우(X축) 흔들림 추가
    float timeOffset = static_cast<float>(Particle.Flags % 1000) / 200.0f;
    float currentTime = Owner->EmitterTime + timeOffset;
    float swayAmount = 0.3f + ((Particle.Flags % 500) / 500.0f) * 0.2f;  // 0.3~0.5 범위

    // 사인 함수로 부드러운 좌우 흔들림
    Particle.Velocity.X = sinf(currentTime * 0.5f) * swayAmount;

    // 화면 밖으로 나갔을 때 재활용
    if (Particle.Location.Z < -40.0f)
    {
        float randX = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * 80.0f;
        float randY = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * 40.0f;
        float startZ = 30.0f + (static_cast<float>(rand()) / RAND_MAX * 10.0f);

        Particle.Location = FVector(randX, randY, startZ);
        Particle.RelativeTime = 0.0f;

        float fallSpeed = 1.5f + (static_cast<float>(rand()) / RAND_MAX * 1.0f);
        Particle.BaseVelocity = FVector(0.0f, 0.0f, -fallSpeed);
    }

    END_UPDATE_LOOP;
}