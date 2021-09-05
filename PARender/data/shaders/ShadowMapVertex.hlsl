cbuffer PerApplication : register(b0)
{
    matrix projectionMatrix;
}

cbuffer Camera : register(b1)
{
    matrix cameraMatrix;
    float4 cameraWorldPosition;
}

cbuffer PerObject : register(b3)
{
    float4 texScaleNHeight; // texture scale, parralax Height
    bool useHeightMap;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoords : TEXCOORDS;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    matrix instanceMat : TRANSFORM;
    matrix instanceNormalMat : NORMALMATRIX;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
};

VS_OUTPUT main(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    matrix mvp = mul(IN.instanceMat, mul(cameraMatrix, projectionMatrix));
    OUT.position = mul(float4(IN.position, 1.0f), mvp);
    
    return OUT;
}