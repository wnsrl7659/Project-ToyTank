/***************************************************************************/
/*!
\file   ToScreenPixel.hlsl
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
Texture2D FinalTexture : register(t0);
Texture2D BloomTexture : register(t1);
Texture2D Vignette : register(t2);
SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float2 texCoords : TEXCOORDS;
    float4 position : SV_Position;
};

/*===========================================================================*\
|| ------------------------- UTILITY FUNCTIONS ----------------------------- ||
\*===========================================================================*/
float3 tonemap(float3 C)
{
	// Filmic -- model film properties
    C = max(0, C - 0.004);
    return (C * (6.2 * C + 0.5)) / (C * (6.2 * C + 1.7) + 0.06);
}

/*===========================================================================*\
|| --------------------------- MAIN FUNCTION ------------------------------- ||
\*===========================================================================*/
float4 main(PS_INPUT IN) : SV_TARGET
{
    float4 hdrColor = FinalTexture.Sample(Sampler, IN.texCoords);
    float4 bloomColor = BloomTexture.Sample(Sampler, IN.texCoords);
    float3 combined = hdrColor.rgb * hdrColor.a + bloomColor.rgb * bloomColor.a * 0.3f;
    
      // Exposure tone mapping
   // float exposure = 1.7f;
   // float3 mapped = float3(1.0f, 1.0f, 1.0f) - exp(-hdrColor * exposure);

      // Gamma correction 
   // float gamma = 2.2f; //2.2f;
   // float oneOverGam = 1.0 / gamma;
   // mapped = pow(mapped, float3(oneOverGam, oneOverGam, oneOverGam));
    
    return float4(tonemap(combined), 1.0) * Vignette.Sample(Sampler, IN.texCoords);
}
