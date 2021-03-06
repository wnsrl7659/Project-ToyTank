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
Texture2D OutTexture : register(t0);
SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float2 texCoords : TEXCOORDS;
    float4 position : SV_Position;
};

/*===========================================================================*\
|| --------------------------- MAIN FUNCTION ------------------------------- ||
\*===========================================================================*/
float4 main(PS_INPUT IN) : SV_TARGET
{  
    float2 uv = IN.texCoords.xy;
    uv *= 1.0 - uv.yx;
    float vig = (uv.x * uv.y * 15.0);
    vig = pow(vig, 0.25);
  
    return float4(vig, vig, vig, 1.0);
}