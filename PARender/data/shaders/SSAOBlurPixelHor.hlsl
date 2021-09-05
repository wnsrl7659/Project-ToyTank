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
    const float offset[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
    const float weight[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };
    float width = 0;
    float height = 0;
    SSAOTexture.GetDimensions(width, height);
    float2 texelSize = 1.0 / float2(width, height);
    float3 result = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 3; ++i)
    {
        float2 off = float2(offset[i], 0) * texelSize;
        result += SSAOTexture.Sample(Sampler, IN.texCoords + off).rgb * weight[i];
        
        off = float2(offset[i], 0) * texelSize;
        result += SSAOTexture.Sample(Sampler, IN.texCoords - off).rgb * weight[i];
    }

    return float4(result, 1);
}
