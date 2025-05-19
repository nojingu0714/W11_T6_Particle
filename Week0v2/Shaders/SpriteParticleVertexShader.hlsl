cbuffer FSceneConstant : register(b0)
{
    matrix ViewMatrix;
    matrix ProjMatrix;
    float3 CameraPos;
    float Padding0;
    float3 CameraLookAt;
    float Padding1;
};


struct VSInput
{
    float3 Position : POSITION0; // Center of sprite
    float RelativeTime : TEXCOORD0;
    float3 OldPosition : TEXCOORD1; // Not used for static billboard
    float ParticleId : TEXCOORD2;
    float2 Size : TEXCOORD3; // X=half‐width, Y=half‐height
    float Rotation : TEXCOORD4; // Radians
    float SubImageIdx : TEXCOORD5; // UV‐offset index, if any
    float4 Color : COLOR0; // RGBA multiplier
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
};

VSOutput mainVS(VSInput IN, uint SV_VertexID : SV_VertexID)
{
    VSOutput OUT;

    // SV_VertexID mod 4 gives which corner: 0,1,2,3
    int corner = SV_VertexID & 3;

    // Define unit quad in NDC space around (0,0):
    float2 quadVerts[4] =
    {
        float2(-1, -1),
        float2(1, -1),
        float2(1, 1),
        float2(-1, 1)
    };
    float2 uvCoords[4] =
    {
        float2(0, 0),
        float2(1, 0),
        float2(1, 1),
        float2(0, 1)
    };

    float2 cornerPos = quadVerts[corner] * IN.Size; // scale quad by particle size
    float s = sin(IN.Rotation), c = cos(IN.Rotation);
    // rotate corner
    float2 rotated = float2(
        cornerPos.x * c - cornerPos.y * s,
        cornerPos.x * s + cornerPos.y * c
    );

    // world‐space position of quad corner
    float3 worldPos = float3(IN.Position.xy + rotated.xy,0.0f); // billboard in XY plane
    worldPos.z = IN.Position.z;

    // transform to clip space
    float4 viewPos = mul(float4(worldPos, 1), ViewMatrix);
    OUT.Position = mul(viewPos, ProjMatrix);

    // pass UV (with subimage offset if using atlas)
    OUT.UV = uvCoords[corner];
    OUT.Color = IN.Color;

    return OUT;
}