struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORDS;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.position = float4(IN.position, 1.0f);
    OUT.color = OUT.position;

    return OUT;
}