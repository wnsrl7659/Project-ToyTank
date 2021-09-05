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
static const float PI = 3.14159265359f;
static const float Epsilon = 0.001f;

/*===========================================================================*\
|| -------------------------- INPUT VARIABLES ------------------------------ ||
\*===========================================================================*/
Texture2D AlbedoTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MetalicTexture : register(t2);
Texture2D RoughTexture : register(t3);
Texture2D AOTexture : register(t4);
Texture2D HeightTexture : register(t5);
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

#define MAX_LIGHT_COUNT 64
const int LIGHT_TYPE_POINT = 0;
const int LIGHT_TYPE_SPOT = 1;
const int LIGHT_TYPE_DIR = 2;

struct Light
{
    float3 position;
    float pad0;
    float3 direction;
    float pad1;
    float3 color;
    float pad2;
    float innerAngle; //spotlight use only
    float outerAngle; //spotlight use only
    float intensity;
    int type; //point, spot, dir
};

cbuffer PerApplication : register(b0)
{
    matrix projectionMatrix;
}

cbuffer Camera : register(b1)
{
    matrix cameraMatrix;
    float4 cameraWorldPosition;
    float4 widthHeight;
}

cbuffer SceneLighting : register(b2)
{
    Light lights[MAX_LIGHT_COUNT];
    int light_count;
};

/*===========================================================================*\
|| ------------------------- UTILITY FUNCTIONS ----------------------------- ||
\*===========================================================================*/

/*===========================================================================*\
|| --------------------------- MAIN FUNCTION ------------------------------- ||
\*===========================================================================*/
PS_OUTPUT main(PS_INPUT IN) : SV_TARGET
{
    PS_OUTPUT OUT;
    OUT.Color = float4(AlbedoTexture.Sample(Sampler, IN.texCoords).rgb * IN.color.rgb, IN.color.a);
    OUT.Bloom = float4(0, 0, 0, 1);
    return OUT;
}