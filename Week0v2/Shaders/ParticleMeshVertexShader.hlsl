cbuffer FMatrixConstants : register(b1)
{
    row_major float4x4 Model;
    row_major float4x4 ViewProj;
    row_major float4x4 MInverseTranspose;
    bool isSelected;
    float3 pad0;
};

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

struct VertexInput_MeshData
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float3 normal : NORMAL; // 버텍스 노멀
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
};

struct VertexInput_InstanceData
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

// 정점 셰이더 출력 구조체 (이전과 동일)
struct VSOutput
{
    float4 PositionPS    : SV_Position;
    float4 Color         : COLOR0;
    float2 TexCoord      : TEXCOORD0;
    float3 NormalWS      : NORMAL0;
    float3 PositionWS    : TEXCOORD1;
    float  RelativeTimePS: TEXCOORD2;
};

// 정점 셰이더 함수
VSOutput mainVS(VertexInput_MeshData meshInput, VertexInput_InstanceData instanceInput)
{
    VSOutput output = (VSOutput)0;

    // 1. 인스턴스별 변환 행렬 구성
    //    (위치, 크기, Z축 회전만 사용)

    // 크기 변환 행렬 (X, Y 스케일, Z 스케일은 1.0으로 가정)
    // 또는 instanceInput.Size.x를 균일 스케일로 사용할 수도 있음.
    // 여기서는 instanceInput.Size를 X, Y 스케일로 사용.
    float particleScaleX = instanceInput.Size.x;
    float particleScaleY = instanceInput.Size.y;
    // UV 뒤집기 처리 (만약 Size의 부호를 사용한다면)
    // float2 baseUV = meshInput.TexCoordOS; // SubUV 전에 기본 UV
    // if (particleScaleX < 0.0f) { baseUV.x = 1.0f - baseUV.x; particleScaleX = abs(particleScaleX); }
    // if (particleScaleY < 0.0f) { baseUV.y = 1.0f - baseUV.y; particleScaleY = abs(particleScaleY); }
    // 위 UV 뒤집기는 SubUV 계산 시 함께 고려해야 함. 여기서는 크기만 양수로.
    particleScaleX = abs(particleScaleX);
    particleScaleY = abs(particleScaleY);

    matrix scaleMatrix = matrix(
        particleScaleX, 0, 0, 0,
        0, particleScaleY, 0, 0,
        0, 0, particleScaleX, 0,          // Z 스케일은 1.0
        0, 0, 0, 1
    );

    // Z축 회전 행렬
    float s = sin(instanceInput.Rotation);
    float c = cos(instanceInput.Rotation);
    matrix rotationMatrix = matrix(
        c,  s, 0, 0,
       -s,  c, 0, 0,
        0,  0, 1, 0,
        0,  0, 0, 1
    );

    // 이동 변환 행렬
    // instanceInput.Position이 이미 월드 좌표라면 EmitterWorldMatrix는 Identity
    // instanceInput.Position이 이미터 로컬 좌표라면 EmitterWorldMatrix를 곱해야 함.
    // 여기서는 instanceInput.Position을 최종 월드 중심으로 사용한다고 가정.
    // (만약 instanceInput.Position이 이미터 로컬이면, 먼저 EmitterWorldMatrix로 월드 변환 필요)
    matrix translationMatrix = matrix(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        instanceInput.Position.x, instanceInput.Position.y, instanceInput.Position.z, 1
    );

    // 최종 인스턴스 월드 변환 행렬: Scale -> Rotate -> Translate
    // (오브젝트 로컬 -> 월드)
    // HLSL에서 행렬 곱셈은 오른쪽부터 적용되므로 mul(mul(Scale, Rotate), Translate)
    // 또는, 수학적 순서대로 (v * S * R * T)
    // 여기서는 v * S * R * T (v는 행벡터, 행렬은 row-major)
    matrix instanceWorldMatrix = mul(rotationMatrix, scaleMatrix); // 로컬 회전 후 스케일 (또는 스케일 후 회전)
                                                                  // 보통 Scale -> Rotate 순서가 일반적: mul(scaleMatrix, rotationMatrix)
    instanceWorldMatrix = mul(instanceWorldMatrix, translationMatrix); // 그 다음 이동

    // 만약 instanceInput.Position이 이미터 로컬 공간이고, EmitterWorldMatrix가 있다면:
    // matrix instanceLocalMatrix = mul(scaleMatrix, rotationMatrix);
    // instanceLocalMatrix = mul(instanceLocalMatrix, translationMatrixForLocalOffset); // 만약 로컬 오프셋 이동이 있다면
    // instanceWorldMatrix = mul(instanceLocalMatrix, EmitterWorldMatrix);


    // --- 빌보딩 또는 카메라 페이싱 (선택적, 메시 특성에 따라) ---
    // 만약 메시 파티클도 스프라이트처럼 항상 카메라를 향해야 한다면 (예: 얇은 판자 모양 메시)
    // instanceWorldMatrix를 빌보드 행렬로 대체하거나 수정해야 합니다.
    // 이 경우, 위에서 만든 Scale, Rotation(Z축)은 빌보드 평면 내에서의 크기와 회전이 됩니다.
    // 예시: 카메라 페이싱 (Y축만 카메라를 향하도록, X축은 월드 Up과 수직)
    // {
    //     float3 lookDir = normalize(instanceInput.Position - CameraWorldPosition); // 파티클 -> 카메라
    //     float3 camRight = normalize(cross(float3(0,1,0)/*WorldUp*/, lookDir)); // 월드 Up과 수직인 Right
    //     float3 camUp = normalize(cross(lookDir, camRight)); // 실제 Up
    //
    //     // 위에서 만든 Scale, Rotation(Z축)을 이 새로운 축에 적용
    //     matrix billboardRotMatrix = matrix(
    //         camRight.x, camRight.y, camRight.z, 0,
    //         camUp.x,    camUp.y,    camUp.z,    0,
    //         lookDir.x,  lookDir.y,  lookDir.z,  0, // 메시의 로컬 Z가 카메라를 향하도록
    //         0,          0,          0,          1
    //     );
    //     // Z축 중심 회전 적용
    //     billboardRotMatrix = mul(rotationMatrix, billboardRotMatrix); // 로컬 Z축 회전 후 빌보드
    //     instanceWorldMatrix = mul(scaleMatrix, billboardRotMatrix);
    //     instanceWorldMatrix = mul(instanceWorldMatrix, translationMatrix); // 최종 이동
    // }


    // 2. 정점 위치 변환
    float4 positionOS = float4(meshInput.position);
    float4 positionWS = mul(positionOS, instanceWorldMatrix); // 로컬 정점 -> 월드 정점

    float4 positionVS = mul(positionWS, ViewMatrix);
    output.PositionPS = mul(positionVS, ProjectionMatrix);
    output.PositionWS = positionWS.xyz;

    // 3. 법선 변환
    float4 normalOS = float4(meshInput.normal,0.0f);
    // 법선 변환에는 instanceWorldMatrix의 회전/스케일 부분만 사용
    // (이동 성분 제거, 비균등 스케일 시 역전치 필요)
    // 간단히는 3x3 부분만 사용
    matrix instanceWorldRotScaleMatrix = instanceWorldMatrix;
    instanceWorldRotScaleMatrix._m30_m31_m32 = 0; // 이동 성분 제거 (또는 _41_42_43)
    instanceWorldRotScaleMatrix._m03_m13_m23_m33 = float4(0,0,0,1); // W 성분 정리

    float4 normalWS_H = mul(normalOS, instanceWorldRotScaleMatrix);
    output.NormalWS = normalize(normalWS_H.xyz);

    // 4. SubUV 좌표 계산
    float2 baseMeshUV = meshInput.texcoord;
    // UV 뒤집기 (만약 Size 부호를 사용한다면, 여기서 baseMeshUV를 수정)
    if (instanceInput.Size.x < 0.0f) { baseMeshUV.x = 1.0f - baseMeshUV.x; }
    if (instanceInput.Size.y < 0.0f) { baseMeshUV.y = 1.0f - baseMeshUV.y; }
    
    // if (NumSubUVColumns > 0 && NumSubUVRows > 0)
    // {
    //     float subImageActualIndex = floor(instanceInput.SubImageIndex);
    //     float subImageLerp = frac(instanceInput.SubImageIndex);
    //
    //     float colA = subImageActualIndex % NumSubUVColumns;
    //     float rowA = floor(subImageActualIndex / NumSubUVColumns);
    //     float2 uv_A_Start = float2(colA * InvNumSubUV.x, rowA * InvNumSubUV.y);
    //     float2 uv_A = uv_A_Start + (baseMeshUV * InvNumSubUV);
    //
    //     // 보간을 위한 다음 프레임 인덱스 (간단히 +1, 실제로는 더 복잡한 로직 가능)
    //     float colB = (subImageActualIndex + 1) % NumSubUVColumns;
    //     float rowB = floor((subImageActualIndex + 1) / NumSubUVColumns);
    //     // (만약 애니메이션 끝에 도달하면 어떻게 할지 정의 필요 - 예: 루프, 정지)
    //     // 여기서는 간단히 다음 인덱스로 가정
    //     if (subImageActualIndex + 1 >= NumSubUVColumns * NumSubUVRows) { // 애니메이션 끝이면
    //         colB = colA; // 현재 프레임 유지 (또는 첫 프레임으로 루프)
    //         rowB = rowA;
    //     }
    //
    //     float2 uv_B_Start = float2(colB * InvNumSubUV.x, rowB * InvNumSubUV.y);
    //     float2 uv_B = uv_B_Start + (baseMeshUV * InvNumSubUV);
    //
    //     output.TexCoord = lerp(uv_A, uv_B, subImageLerp);
    // }
    // else
    {
        output.TexCoord = baseMeshUV;
    }

    // 5. 기타 데이터 전달
    output.Color = instanceInput.Color;
    output.RelativeTimePS = instanceInput.RelativeTime;
    // output.ParticleIdPS = instanceInput.ParticleId;

    return output;
}