/***************************************************************************/
/*!
\file   SSAOPixel.hlsl
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

Texture2D NoiseTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MVPTexture : register(t2);
SamplerState Sampler : register(s0);

cbuffer PerApplication : register(b0)
{
    matrix projectionMatrix;
}

cbuffer Camera : register(b1)
{
    matrix cameraMatrix;
    float4 cameraWorldPosition;
}

cbuffer SSAO : register(b5)
{
    float4 samples[16];
    float4 rbp;
    float4 widthHeight;
    //float radius;
    //float bias;
    //float power;
    //float screenWidth;
    //float screenHeight;
}

struct PS_INPUT
{
    float2 texCoords : TEXCOORDS;
    float4 position : SV_Position;
};

/*===========================================================================*\
|| ------------------------- UTILITY FUNCTIONS ----------------------------- ||
\*===========================================================================*/

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
    float3 diff = MVPTexture.Sample(Sampler, tcoord + uv).rgb - p;
    const float3 v = normalize(diff);
    const float d = length(diff) * rbp.r;
    return max(0.0, dot(cnorm, v) - rbp.b) * (1.0 / (1.0 + d)) * rbp.g;
}


/*===========================================================================*\
|| --------------------------- MAIN FUNCTION ------------------------------- ||
\*===========================================================================*/
float main(PS_INPUT IN) : SV_TARGET
{
    float3 p = MVPTexture.Sample(Sampler, IN.texCoords).rgb;
    float3 n = normalize(NormalTexture.Sample(Sampler, IN.texCoords) * 2.0f - 1.0f).rgb;
    float2 rand = normalize(NoiseTexture.Sample(Sampler, widthHeight.xy * IN.texCoords / 64).rg);
    
    float ao = 0.0f;
    float rad = rbp.r / p.z;
  
     /*SSAO Calculation*/ 
    int iterations = 16;
    for (int i = 0; i < iterations; ++i)
    {
        float2 coord1 = reflect(samples[i].xy, rand) * rad;
        float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);
    
        ao += doAmbientOcclusion(IN.texCoords, coord1 * 0.25, p, n);
        ao += doAmbientOcclusion(IN.texCoords, coord2 * 0.5, p, n);
        ao += doAmbientOcclusion(IN.texCoords, coord1 * 0.75, p, n);
        ao += doAmbientOcclusion(IN.texCoords, coord2, p, n);
    }
     
    ao /= (float) iterations * 4.f;
       
    return (ao);
}
