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
 //   bool useHeightMap;
    bool canBeSeenThrough;
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
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 color : COLOR;
    float parallaxHeight : PARRALAXHEIGHT;
    bool useHeightMap : HEIGHTMAPBOOL;
    bool canBeSeenThrough : ALPHABOOL;
    float2 texCoords : TEXCOORDS;
    float3 worldPosition : POSITION0;
    float3 eyePosition : POSITION1;
    float3 mvpPosition : POSITION2;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    matrix toWorld : MATRIX;
    float4 position : SV_Position;
};

VS_OUTPUT main(VS_INPUT IN)
{
    VS_OUTPUT OUT;

    matrix mvp = mul(IN.instanceMat, mul(cameraMatrix, projectionMatrix));
    OUT.position = mul(float4(IN.position, 1.0f), mvp);

    OUT.normal = normalize(mul(IN.normal, (float3x3) IN.instanceNormalMat));
    OUT.tangent = normalize(mul(IN.tangent, (float3x3) IN.instanceNormalMat));
    OUT.bitangent = normalize(mul(IN.bitangent, (float3x3) IN.instanceNormalMat));

    OUT.worldPosition = mul(float4(IN.position, 1.0f), IN.instanceMat).xyz;
    OUT.mvpPosition = mul(float4(OUT.worldPosition, 1.0f), mul(cameraMatrix, projectionMatrix)).xyz;
    OUT.eyePosition = cameraWorldPosition.xyz;
    OUT.texCoords = IN.texCoords / (texScaleNHeight.x);
    OUT.parallaxHeight = texScaleNHeight.y;
    OUT.useHeightMap = true;
   // useHeightMap;
    OUT.toWorld = IN.instanceNormalMat;
    OUT.canBeSeenThrough = canBeSeenThrough;
    
    OUT.color = IN.color;
    
    return OUT;
}