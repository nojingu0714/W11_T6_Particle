Texture2D DiffuseTexture : register(t0);
SamplerState LinearSampler : register(s0);

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
};
float4 mainPS(VSOutput IN) : SV_TARGET
{
    float4 tex = DiffuseTexture.Sample(LinearSampler, IN.UV);
    return tex * IN.Color;
}