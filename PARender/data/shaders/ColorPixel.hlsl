/***************************************************************************/
/*!
\file   LightingPixelDefault.hlsl
\author Ryan Dugie
\brief  Copyright (c) Digipen 2019
*/
/***************************************************************************/

/*===========================================================================*\
|| ------------------------- STATIC VARIABLES ------------------------------ ||
\*===========================================================================*/

/*===========================================================================*\
|| -------------------------- INPUT VARIABLES ------------------------------ ||
\*===========================================================================*/
Texture2D PositonTexture : register(t0);
Texture2D AlbedoTexture : register(t1);
Texture2D NormalTexture : register(t2);
Texture2D MetalRoughAOTexture : register(t3);
Texture2D ViewportTexture : register(t4);
Texture2D MVPTexture : register(t5);
Texture2D SSAOTexture : register(t6);
Texture2D NoiseTexture : register(t7);
TextureCube skyBox : register(t8);
Texture2D ShadowMap : register(t9);
SamplerState Sampler : register(s0);
SamplerComparisonState shadowSampler : register(s1);

struct PS_INPUT
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

struct PS_OUTPUT
{
    float4 Color : SV_TARGET0;
    float4 Bloom : SV_TARGET1;
};

cbuffer Camera : register(b1)
{
    matrix cameraMatrix;
    float4 cameraWorldPosition;
    float4 widthHeight;
}

/*===========================================================================*\
|| ------------------------- UTILITY FUNCTIONS ----------------------------- ||
\*===========================================================================*/

/*===========================================================================*\
|| --------------------------- MAIN FUNCTION ------------------------------- ||
\*===========================================================================*/
PS_OUTPUT main(PS_INPUT IN) : SV_TARGET
{
    PS_OUTPUT OUT;
    
    if (IN.color.a < 0.01f)
        discard;
    
    const float4x4 thresholdMTX =
    {
        1, 5, 2, 6,
        6, 3, 5, 4,
        4, 5, 2, 6,
        6, 2, 5, 1
    };
    
    if (IN.canBeSeenThrough == true)
    {
        float dist = distance(IN.worldPosition, IN.eyePosition);
        if (IN.worldPosition.y < -2.99f || dist < thresholdMTX[IN.position.x % 4][IN.position.y % 4] + 3.5f)
            discard;
    }
    
    OUT.Color = IN.color;
    OUT.Bloom = IN.color;
      
    return OUT;
}