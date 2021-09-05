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
TextureCube cubeMap : register(t0);
SamplerState samTriLinearSam
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct PS_INPUT
{
    float3 texCoords : TEXCOORDS;
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
    float x = IN.position.x - (widthHeight.x / 2) + widthHeight.z;
    float y = IN.position.y - (widthHeight.y / 1.9) + widthHeight.w;
    float c = sqrt(x * x + y * y);
    if ((c <= 9 && c >= 7) || c <= 3)
    {
        OUT.Color = float4(0, 1, 0, 1);
    }
    else
    {
        OUT.Color = cubeMap.Sample(samTriLinearSam, IN.texCoords);
    }   
    
    OUT.Bloom = float4(0, 0, 0, 1);
    
    return OUT;
}
