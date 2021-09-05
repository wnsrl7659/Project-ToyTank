/***************************************************************************/
/*!
\file   SSAOBLURPIXEL.hlsl
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
Texture2D SSAOTexture : register(t0);
SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float2 texCoords : TEXCOORDS;
    float4 position : SV_Position;
};

/*===========================================================================*\
|| ------------------------- UTILITY FUNCTIONS ----------------------------- ||
\*===========================================================================*/

/*===========================================================================*\
|| --------------------------- MAIN FUNCTION ------------------------------- ||
\*===========================================================================*/
float4 main(PS_INPUT IN) : SV_TARGET
{
    float width = 0;
    float height = 0;
    SSAOTexture.GetDimensions(width, height);
    float2 texelSize = 1.0 / float2(width, height);
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            float2 offset = float2(float(x), float(y)) * texelSize;
            result += SSAOTexture.Sample(Sampler, IN.texCoords + offset).r;
        }
    }

    result = result / 16;
    
    return float4(result, result, result, 1);
}
