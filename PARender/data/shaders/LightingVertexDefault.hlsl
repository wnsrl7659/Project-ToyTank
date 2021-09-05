
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORDS;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    matrix instanceMat : TRANSFORM;
    matrix instanceNormalMat : NORMALMATRIX;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float2 texCoords : TEXCOORDS;
    float4 position : SV_Position;
};

VS_OUTPUT main(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    OUT.texCoords = IN.texCoords;
    OUT.position = float4(IN.position, 1);

    return OUT;
}