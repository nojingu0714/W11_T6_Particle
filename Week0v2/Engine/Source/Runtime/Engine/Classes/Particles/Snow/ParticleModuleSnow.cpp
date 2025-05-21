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

  
    MyMatarial = FObjectFactory::ConstructObject<UMaterial>(this);
    FObjMaterialInfo MatInfo;
    MatInfo.MTLName = "Snow";
    MatInfo.DiffuseTextureName = "Snow.png";
    MatInfo.DiffuseTexturePath = L"Assets/Texture/Snow.png";
    MyMatarial->SetMaterialInfo(MatInfo);
}

void UParticleModuleSnow::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    ParticleBase->RelativeTime = 0.0f;
    // Min/Max 값을 사용한 랜덤 위치 계산
    float randX = SpawnXMin + static_cast<float>(rand()) / RAND_MAX * (SpawnXMax - SpawnXMin);
    float randY = SpawnYMin + static_cast<float>(rand()) / RAND_MAX * (SpawnYMax - SpawnYMin);
    float randZ = SpawnZMin + static_cast<float>(rand()) / RAND_MAX * (SpawnZMax - SpawnZMin);
    ParticleBase->Location = FVector(randX, randY, randZ);

    // 낙하 속도 (-Z 방향으로)
    float fallSpeed = 1.5f + (static_cast<float>(rand()) / RAND_MAX * 1.0f);  // 1.5~2.5 범위
    ParticleBase->BaseVelocity = FVector(0.0f, 0.0f, -fallSpeed);  // Z축 아래 방향으로
    ParticleBase->Velocity = FVector(0.0f, 0.0f, -fallSpeed);  // Z축 아래 방향으로

    // 각 파티클의 흔들림 특성 저장
    ParticleBase->Flags = rand();

    if (!Owner->RequiredModule->SpriteTexture)
    {
        Owner->RequiredModule->SpriteTexture = MyMatarial;
    }
}

void UParticleModuleSnow::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    BEGIN_UPDATE_LOOP;

    // 기본 Z축 낙하
    Particle.Velocity.Z = Particle.BaseVelocity.Z;

    // 파티클별 고유 특성 계산
    float timeOffset = static_cast<float>(Particle.Flags % 1000) / 200.0f;
    float currentTime = Owner->EmitterTime + timeOffset;
    float swayAmount = 0.3f + ((Particle.Flags % 500) / 500.0f) * 0.2f;  // 0.3~0.5 범위

    // 사인 함수로 부드러운 좌우 흔들림 (시간에 따라 변화)
    Particle.Velocity.X = sinf(currentTime * 0.5f) * swayAmount;
    Particle.BaseVelocity.X = sinf(currentTime * 0.5f) * swayAmount;

    // 1. 사이즈 맥동 효과 추가 (파티클별로 다른 주기)
    float sizeFreq = 0.5f + ((Particle.Flags % 300) / 300.0f);  // 0.5~1.5 범위
    float sizeAmp = 0.1f + ((Particle.Flags % 200) / 200.0f) * 0.1f;  // 0.1~0.2 범위

    // 기본 크기에서 시간에 따라 10~20% 범위로 맥동
    float sizeFactor = 1.0f + sinf(currentTime * sizeFreq) * sizeAmp;
    Particle.Size = Particle.BaseSize * sizeFactor;

    // 2. 회전 속도 추가 (파티클별로 다른 속도와 방향)
    // 파티클별 고유 회전 방향과 속도 결정
    float rotSpeed = 0.2f + ((Particle.Flags % 400) / 400.0f) * 0.3f;  // 0.2~0.5 범위

    // Flags의 홀수/짝수로 회전 방향 결정
    if (Particle.Flags % 2 == 0) {
        rotSpeed = -rotSpeed;  // 짝수 ID는 시계 반대 방향
    }

    Particle.RotationRate = rotSpeed; 

    END_UPDATE_LOOP;
}