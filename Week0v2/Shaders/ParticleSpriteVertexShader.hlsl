cbuffer FMatrixConstants : register(b1)
{
    row_major float4x4 Model;
    row_major float4x4 ViewProj;
    row_major float4x4 MInverseTranspose;
    bool isSelected;
    float3 pad0;
};

// cbuffer FSceneConstant : register(b2)
// {
//     row_major matrix ViewMatrix;
//     row_major matrix ProjMatrix;
//     float3 CameraPos;
//     float3 CameraLookAt;
// };

cbuffer FPerFrameConstants : register(b2) // b0는 상수 버퍼 슬롯 번호
{
    row_major matrix ViewMatrix;
    row_major matrix ProjectionMatrix;
    float3 CameraWorldPosition;
    float  Padding0; // 정렬을 위한 패딩
    float3 CameraUpVector;    // 카메라의 Up 벡터 (빌보딩 시 필요)
    float  Padding1;
    float3 CameraRightVector; // 카메라의 Right 벡터 (빌보딩 시 필요)
    float  Padding2;
};

// cbuffer PerEmitterConstants : register(b3) 
// {
//     // SubUV 애니메이션 관련 정보
//     uint  NumSubUVColumns; // 가로 SubImage 개수
//     uint  NumSubUVRows;    // 세로 SubImage 개수
//     float2 InvNumSubUV;    // (1.0f / NumSubUVColumns, 1.0f / NumSubUVRows)
//     // 기타 이미터별 공통 설정 (예: bUseLocalSpace 플래그 등)
// };


struct VSOutput
{
    float4 Position    : SV_Position;   // 클립 공간 위치 (필수)
    float2 TexCoord      : TEXCOORD0;     // UV 좌표
    float4 Color         : COLOR0;        // 파티클 색상
    float  RelativeTimePS: TEXCOORD1;
    float  ParticleIdPS  : TEXCOORD2;
};


struct ParticleVSInput
{
    float3 Position      : INSTANCED_POSITION0;     // 파티클 중심 월드 (또는 로컬) 위치
    float  RelativeTime  : INSTANCED_TEXCOORD0;   // 상대적 수명 (0~1)
    float3 OldPosition   : INSTANCED_TEXCOORD1;   // 이전 프레임 위치 (모션 블러용)
    float  ParticleId    : INSTANCED_TEXCOORD2;   // 파티클 고유 ID
    float2 Size          : INSTANCED_TEXCOORD3;   // 파티클 2D 크기 (UV 뒤집기 정보 포함 가능)
    float  Rotation      : INSTANCED_TEXCOORD4;   // 파티클 Z축 회전 (라디안)
    float  SubImageIndex : INSTANCED_TEXCOORD5;   // SubUV 애니메이션 이미지 인덱스
    float4 Color         : INSTANCED_COLOR0;      // 파티클 색상 및 알파
};


// 정점 셰이더 함수
VSOutput mainVS(ParticleVSInput input, uint vertexID : SV_VertexID) // SV_VertexID: 0, 1, 2, 3 (Quad의 각 코너)
{
    VSOutput output = (VSOutput)0; // 출력 구조체 초기화

    // 1. Quad 코너 오프셋 및 기본 UV 결정 (SV_VertexID 사용)
    //    TriangleStrip 순서 (예: 좌하단, 좌상단, 우하단, 우상단 - 반시계 방향 컬링 기준)
    //    또는 (좌상단, 우상단, 좌하단, 우하단) - 시계 방향 컬링 기준
    //    여기서는 반시계 방향 컬링을 가정하고, 일반적인 Quad 정점 순서와 다를 수 있음에 유의.
    //    DirectX 기본값은 시계방향이 앞면(Front-facing)이므로, 그에 맞추거나 Rasterizer State에서 변경.
    //    여기서는 일반적인 좌상단부터 시작하는 순서로 하고, 컬링은 Rasterizer State에서 맞춘다고 가정.
    //    또는, TriangleStrip에 맞는 순서로 정점 생성:
    //    0: 좌하, 1: 좌상, 2: 우하, 3: 우상 (이 순서로 하면 두 삼각형 (0,1,2), (1,2,3)이 반시계방향)
    float2 localCorners[4] = {
        float2(-0.5f, -0.5f), // 0: 좌하단 (TriangleStrip 시작점)
        float2(-0.5f,  0.5f), // 1: 좌상단
        float2( 0.5f, -0.5f), // 2: 우하단
        float2( 0.5f,  0.5f)  // 3: 우상단 (TriangleStrip 끝점)
    };
    // 해당 UV 좌표
    float2 localUVs[4] = {
        float2(0.0f, 1.0f), // 0: 좌하단
        float2(0.0f, 0.0f), // 1: 좌상단
        float2(1.0f, 1.0f), // 2: 우하단
        float2(1.0f, 0.0f)  // 3: 우상단
    };

    // SV_VertexID는 직접 0, 1, 2, 3 코너 인덱스를 나타냄
    uint offset = vertexID; 

    float2 cornerOffset = localCorners[offset];
    float2 baseUV = localUVs[offset];

    // 2. 파티클 크기 및 UV 뒤집기 적용
    //float2 particleSize = input.Size;
    float2 particleSize = float2(1.f,1.f); // 임시 크기 (나중에 실제 크기로 대체)
    
    // UV 뒤집기 처리 (크기 부호 확인)
    if (particleSize.x < 0.0f)
    {
        baseUV.x = 1.0f - baseUV.x;
        particleSize.x = abs(particleSize.x);
    }
    if (particleSize.y < 0.0f)
    {
        baseUV.y = 1.0f - baseUV.y;
        particleSize.y = abs(particleSize.y);
    }
    
    // 코너 오프셋에 실제 크기 적용
    cornerOffset *= particleSize;

    // 3. 파티클 회전 적용 (2D 회전)
    float s = sin(input.Rotation);
    float c = cos(input.Rotation);
    float2 rotatedOffset = float2(
        cornerOffset.x * c - cornerOffset.y * s,
        cornerOffset.x * s + cornerOffset.y * c
    );

    // 4. 빌보딩 (카메라를 향하도록)
    // 파티클 중심 위치 (input.Position)
    // 빌보딩을 위한 카메라 상대적 로컬 축 생성
    // (카메라의 Right와 Up 벡터를 사용하여 파티클의 로컬 X, Y 축을 구성)
    // 주의: CameraRightVector와 CameraUpVector는 월드 공간 기준이어야 함.
    // input.Position이 로컬 공간이면, 먼저 월드 공간으로 변환해야 함.
    // 여기서는 input.Position이 이미 월드 공간이라고 가정.
    // (만약 input.Position이 로컬이면, ModelMatrix * input.Position으로 월드 변환 후 사용)

    // 카메라를 향하는 축 정렬 빌보딩 (Cylindrical/Spherical 빌보딩은 더 복잡)
    // 파티클의 로컬 X축은 카메라의 Right 벡터, Y축은 카메라의 Up 벡터를 사용
    //float3 billboardedPosition = mul(float4(input.Position, 1.0f), Model).xyz;
    float3 billboardedPosition = input.Position.xyz;
    billboardedPosition += CameraRightVector * rotatedOffset.x;
    billboardedPosition += CameraUpVector * rotatedOffset.y;
    // (만약 완벽한 구형 빌보딩(Spherical Billboarding)을 원한다면,
    //  파티클 위치에서 카메라 위치로의 벡터를 Z축으로 하고,
    //  이 Z축과 월드 Up 벡터를 외적하여 X축, 다시 Z,X 외적으로 Y축을 구해야 함)

    // 5. 최종 월드-뷰-프로젝션 변환
    float4 worldPosition = float4(billboardedPosition, 1.0f);
    float4 viewPosition = mul(worldPosition, ViewMatrix);
    output.Position = mul(viewPosition, ProjectionMatrix);

    // 6. SubUV 좌표 계산
    // input.SubImageIndex는 float (정수부: 이미지 인덱스, 소수부: 보간 가중치)
    //float subImageActualIndex = floor(input.SubImageIndex); // 정수 인덱스
    // float subImageLerp = frac(input.SubImageIndex); // 보간 값 (여기서는 사용 안 함)

    // // 현재 SubImage의 좌상단 UV 좌표 계산
    // float col = subImageActualIndex % NumSubUVColumns;
    // float row = floor(subImageActualIndex / NumSubUVColumns);
    //
    // float2 subUVStart = float2(col * InvNumSubUV.x, row * InvNumSubUV.y);

    // 최종 UV 좌표 = SubImage 시작 UV + (Quad 내 상대 UV * SubImage 하나의 크기)
    //output.TexCoord = subUVStart + (baseUV * InvNumSubUV);
    output.TexCoord = (baseUV);

    // 7. 기타 데이터 전달
    output.Color = input.Color;
    output.RelativeTimePS = input.RelativeTime;
    output.ParticleIdPS = input.ParticleId;

    return output;
    
}