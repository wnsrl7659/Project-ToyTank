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
Texture2D AlbedoTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MetalicTexture : register(t2);
Texture2D RoughTexture : register(t3);
Texture2D AOTexture : register(t4);
Texture2D HeightTexture : register(t5);
SamplerState Sampler : register(s0);

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

//float2 ParallaxMapping(const float2 texCoords, const float3 viewDir, const float parallaxHeight)
//{

//    //// number of depth layers
//    const float minLayers = 8;
//    const float maxLayers = 32;
//    const float2 dx = ddx(texCoords);
//    const float2 dy = ddy(texCoords);
//    float numLayers = lerp(maxLayers, minLayers, abs(dot(float3(0.0, 0.0, 1.0), viewDir)));

//    // calculate the size of each layer
//    float layerDepth = 1.0 / numLayers;

//    // depth of current layer
//    float currentLayerDepth = 0.0;

//    // the amount to shift the texture coordinates per layer (from vector P)
//    float2 P = normalize(float2(viewDir.x, viewDir.y) / viewDir.z) * parallaxHeight;
//    float2 deltaTexCoords = P / numLayers;
  
//    // get initial values
//    float2 currentTexCoords = texCoords;
//    float currentDepthMapValue = ((HeightTexture.SampleGrad(Sampler, currentTexCoords, dx, dy).r * -1) + 1);
      

//    while (currentLayerDepth < currentDepthMapValue)
//    {
//        // shift texture coordinates along direction of P
//        currentTexCoords -= deltaTexCoords;
//        // get depthmap value at current texture coordinates
//        currentDepthMapValue = ((HeightTexture.SampleGrad(Sampler, currentTexCoords, dx, dy).r * -1) + 1);
//        // get depth of next layer
//        currentLayerDepth += layerDepth;
//    }
    
//    // get texture coordinates before collision
//    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

//    // get depth after and before collision for linear interpolation
//    float afterDepth = currentDepthMapValue - currentLayerDepth;
//    float beforeDepth = ((HeightTexture.SampleGrad(Sampler, prevTexCoords, dx, dy).r * -1) + 1) - currentLayerDepth + layerDepth;
 
//    // interpolation of texture coordinates
//    float weight = afterDepth / (afterDepth - beforeDepth);
//    return (prevTexCoords * weight + currentTexCoords * (1.0 - weight));
//}//float2 ParallaxMapping(const float2 texCoords, const float3 viewDir, const float parallaxHeight)
//{

//    //// number of depth layers
//    const float minLayers = 8;
//    const float maxLayers = 32;
//    const float2 dx = ddx(texCoords);
//    const float2 dy = ddy(texCoords);
//    float numLayers = lerp(maxLayers, minLayers, abs(dot(float3(0.0, 0.0, 1.0), viewDir)));

//    // calculate the size of each layer
//    float layerDepth = 1.0 / numLayers;

//    // depth of current layer
//    float currentLayerDepth = 0.0;

//    // the amount to shift the texture coordinates per layer (from vector P)
//    float2 P = normalize(float2(viewDir.x, viewDir.y) / viewDir.z) * parallaxHeight;
//    float2 deltaTexCoords = P / numLayers;
  
//    // get initial values
//    float2 currentTexCoords = texCoords;
//    float currentDepthMapValue = ((HeightTexture.SampleGrad(Sampler, currentTexCoords, dx, dy).r * -1) + 1);
      

//    while (currentLayerDepth < currentDepthMapValue)
//    {
//        // shift texture coordinates along direction of P
//        currentTexCoords -= deltaTexCoords;
//        // get depthmap value at current texture coordinates
//        currentDepthMapValue = ((HeightTexture.SampleGrad(Sampler, currentTexCoords, dx, dy).r * -1) + 1);
//        // get depth of next layer
//        currentLayerDepth += layerDepth;
//    }
    
//    // get texture coordinates before collision
//    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

//    // get depth after and before collision for linear interpolation
//    float afterDepth = currentDepthMapValue - currentLayerDepth;
//    float beforeDepth = ((HeightTexture.SampleGrad(Sampler, prevTexCoords, dx, dy).r * -1) + 1) - currentLayerDepth + layerDepth;
 
//    // interpolation of texture coordinates
//    float weight = afterDepth / (afterDepth - beforeDepth);
//    return (prevTexCoords * weight + currentTexCoords * (1.0 - weight));
//}

/*===========================================================================*\
|| --------------------------- MAIN FUNCTION ------------------------------- ||
\*===========================================================================*/
PS_OUTPUT main(PS_INPUT IN) : SV_TARGET
{
    PS_OUTPUT OUT;
    
    float2 texCoords;
    float3x3 TBN = float3x3(IN.tangent, IN.bitangent, IN.normal);
    float3 viewDir = mul(IN.eyePosition - IN.worldPosition, transpose(TBN));
    viewDir = -normalize(viewDir);
    
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

          
 //   if (IN.useHeightMap)
  //  {
 //       texCoords = ParallaxMapping(IN.texCoords, viewDir, 0.1);
 //   }
  //  else
    texCoords = IN.texCoords;

    OUT.Positon = float4(IN.worldPosition, 1);
    OUT.MVPPosition = float4(IN.mvpPosition, 1);
    
    float x = IN.position.x - (widthHeight.x / 2) + widthHeight.z;
    float y = IN.position.y - (widthHeight.y / 1.9) + widthHeight.w;
    float c = sqrt(x * x + y * y);
    if ((c <= 9 && c >= 7) || c <= 3)
    {
        OUT.Albedo = float4(0, 1, 0, 1);
    }
    else
    {
        OUT.Albedo = float4(AlbedoTexture.Sample(Sampler, texCoords).rgb * IN.color.rgb, IN.color.a);
    }
   
    OUT.Normal = float4(normalize(mul(normalize(NormalTexture.Sample(Sampler, texCoords).rgb * 2.0f - 1.0), TBN)), 1);
   // OUT.Normal = normalize(mul(OUT.Normal, IN.toWorld));

    OUT.MetalRoughAO = float4(MetalicTexture.Sample(Sampler, texCoords).r,
    RoughTexture.Sample(Sampler, texCoords).r, AOTexture.Sample(Sampler, texCoords).r, 1);

    OUT.ViewPosition = float4(IN.eyePosition, 1);

    return OUT;
}
