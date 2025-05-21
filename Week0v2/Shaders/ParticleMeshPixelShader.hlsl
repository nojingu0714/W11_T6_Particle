
#include "ShaderHeaders/GSamplers.hlsli"



Texture2D gTexture : register(t0);

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

// 픽셀 셰이더
float4 mainPS(VSOutput input) : SV_Target
{
    float4 col = gTexture.Sample(linearSampler, input.TexCoord);
     float threshold = 0.01; // 필요한 경우 임계값을 조정
     if (col.a < threshold)
         clip(-1); // 픽셀 버리기

    //float4 col = float4(1.f,1.f,1.f,1.f);
    
    return col;
}