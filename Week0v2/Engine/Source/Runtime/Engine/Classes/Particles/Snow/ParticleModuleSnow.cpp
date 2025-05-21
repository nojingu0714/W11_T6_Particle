#include "ParticleModuleSnow.h"
#include "Engine/Source/Runtime/Engine/ParticleHelper.h"
#include "Engine/Source/Runtime/Engine/ParticleEmitterInstance.h"
#include "Particles/ParticleModuleRequired.h"

UParticleModuleSnow::UParticleModuleSnow() :
    SpawnXMin(-80.0f),
    SpawnXMax(80.0f),
    SpawnYMin(-40.0f),
    SpawnYMax(40.0f),
    SpawnZMin(30.0f),
    SpawnZMax(40.0f)
{

    bUpdateModule = true;
    bSpawnModule=true;
}

void UParticleModuleSnow::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    ParticleBase->RelativeTime = 0.0f;
    // Min/Max 값을 사용한 랜덤 위치 계산
    float randX = SpawnXMin.GetValue(0) + static_cast<float>(rand()) / RAND_MAX * (SpawnXMax.GetValue(0) - SpawnXMin.GetValue(0));
    float randY = SpawnYMin.GetValue(0) + static_cast<float>(rand()) / RAND_MAX * (SpawnYMax.GetValue(0) - SpawnYMin.GetValue(0));
    float randZ = SpawnZMin.GetValue(0) + static_cast<float>(rand()) / RAND_MAX * (SpawnZMax.GetValue(0) - SpawnZMin.GetValue(0));
    ParticleBase->Location = FVector(randX, randY, randZ);

    // 낙하 속도 (-Z 방향으로)
    float fallSpeed = 1.5f + (static_cast<float>(rand()) / RAND_MAX * 1.0f);  // 1.5~2.5 범위
    ParticleBase->BaseVelocity = FVector(0.0f, 0.0f, -fallSpeed);  // Z축 아래 방향으로
    ParticleBase->Velocity = FVector(0.0f, 0.0f, -fallSpeed);  // Z축 아래 방향으로

    // 각 파티클의 흔들림 특성 저장
    ParticleBase->Flags = rand();
}

void UParticleModuleSnow::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    BEGIN_UPDATE_LOOP;

    // 기본 Z축 낙하 - GravityScale 적용
    Particle.Velocity.Z = Particle.BaseVelocity.Z * GravityScale.GetValue(0);

    // 파티클별 고유 특성 계산
    float timeOffset = static_cast<float>(Particle.Flags % 1000) / 200.0f;
    float currentTime = Owner->EmitterTime + timeOffset;

    // 사인 함수로 부드러운 좌우 흔들림 (시간에 따라 변화)
    float swayAmount = SwayMinAmount.GetValue(0) + ((Particle.Flags % 500) / 500.0f) *
        (SwayMaxAmount.GetValue(0) - SwayMinAmount.GetValue(0));
    float swayValue = sinf(currentTime * SwayFrequency.GetValue(0)) * swayAmount;
    Particle.Velocity.X = swayValue;
    Particle.BaseVelocity.X = swayValue;

    // 회전 속도 추가 (파티클별로 다른 속도와 방향)
    float rotSpeed = RotationSpeedMin.GetValue(0) + ((Particle.Flags % 400) / 400.0f) *
        (RotationSpeedMax.GetValue(0) - RotationSpeedMin.GetValue(0));

    // 회전 방향 결정 (RotationDirectionBias 사용)
    // Flags의 홀수/짝수로 회전 방향 결정하되, RotationDirectionBias 값으로 방향 결정 로직 수정
    bool clockwise = (Particle.Flags % 2 == 0);

    // RotationDirectionBias 값에 따라 회전 방향 결정
    float dirBias = RotationDirectionBias.GetValue(0);
    if (dirBias != 0.0f) {
        if (dirBias > 0.0f && Particle.Flags % 100 < dirBias * 100) {
            clockwise = true;  // 양수: 시계방향 선호
        }
        else if (dirBias < 0.0f && Particle.Flags % 100 < -dirBias * 100) {
            clockwise = false;  // 음수: 반시계방향 선호
        }
    }

    if (!clockwise) {
        rotSpeed = -rotSpeed;
    }

    Particle.RotationRate = rotSpeed;

    END_UPDATE_LOOP;
}