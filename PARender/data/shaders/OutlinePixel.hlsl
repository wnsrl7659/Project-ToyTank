/***************************************************************************/
/*!
\file   GBufferPixel.hlsl
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
Texture2D Outline : register(t10);
SamplerState Sampler : register(s0);

struct PS_INPUT
{
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
    nointerpolation bool discardVertex : DISCARDEDVERTEX;
    nointerpolation float3 positionModel : POSITIONMODEL;
    float4 position : SV_Position;
};

struct PS_OUTPUT
{
    float4 Positon : SV_TARGET0;
    float4 Albedo : SV_TARGET1;
    float4 Normal : SV_TARGET2;
    float4 MetalRoughAO : SV_TARGET3;
    float4 ViewPosition : SV_TARGET4;
    float4 MVPPosition : SV_TARGET5;
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
    
    float x = IN.position.x - (widthHeight.x / 2) + widthHeight.z;
    float y = IN.position.y - (widthHeight.y / 1.9) + widthHeight.w;
    float c = sqrt(x * x + y * y);
    if ((c <= 9 && c >= 7) || c <= 3)
    {
        discard;
    }
    
    if (IN.discardVertex || Outline.Sample(Sampler, IN.texCoords).r == 1)
        discard;
    
    OUT.Positon = float4(IN.worldPosition, 1);
    OUT.MVPPosition = float4(IN.mvpPosition, 1);

    OUT.Albedo = float4(0, 0, 0, 1);

    OUT.Normal = float4(0, 0, 0, 1);
    OUT.MetalRoughAO = float4(0, 0, 0, 1);

    OUT.ViewPosition = float4(IN.eyePosition, 1);

    return OUT;
}
