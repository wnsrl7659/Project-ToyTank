cbuffer PerApplication : register(b0)
{
    matrix projectionMatrix;
}

cbuffer Camera : register(b1)
{
    matrix cameraMatrix;
    float4 cameraWorldPosition;
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
    float3 texCoords : TEXCOORDS;
    float4 position : SV_Position;
};

VS_OUTPUT main(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    matrix cam = cameraMatrix;
    cam[0][3] = 0;
    cam[1][3] = 0;
    cam[2][3] = 0;
    cam[3][3] = 0;
    cam[3][0] = 0;
    cam[3][1] = 0;
    cam[3][2] = 0;
    cam[3][3] = 0;
    matrix mvp = mul(cam, projectionMatrix);
    OUT.position = mul(float4(IN.position, 1.0f), mvp).xyww;
    OUT.texCoords = IN.position.rgb;

    return OUT;
}