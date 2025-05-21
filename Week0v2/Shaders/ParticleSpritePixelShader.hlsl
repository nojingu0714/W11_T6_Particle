
#include "ShaderHeaders/GSamplers.hlsli"

Texture2D gTexture : register(t0);
struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float4 Color         : COLOR0;        // 파티클 색상
    float  RelativeTimePS: TEXCOORD1;
    float  ParticleIdPS  : TEXCOORD2;
};

// 픽셀 셰이더
float4 mainPS(PS_INPUT input) : SV_Target
{
    float4 col = gTexture.Sample(linearSampler, input.TexCoord);
    // 알파 테스트 - 낮은 알파값 픽셀 제거
    const float threshold = 0.1;
    if (col.a < threshold)
        clip(-1);
    
    return col;
}