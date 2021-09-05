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
    float2 texCoords : TEXCOORDS;
    float4 position : SV_Position;
};

struct PS_OUTPUT
{
    float4 Color : SV_TARGET0;
    float4 Bloom : SV_TARGET1;
};

#define MAX_LIGHT_COUNT 64
#define LIGHT_TYPE_POINT  0
#define LIGHT_TYPE_SPOT  1
#define LIGHT_TYPE_DIR  2

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

/****************************************************************************/
/*!
\brief
 GGX/Towbridge-Reitz normal distribution function
 approximates the relative surface area of microfacets
 exactly aligned to the (halfway) vector

NDFGGX(n,h,?)= ?^2 / ?((n?h)^2(?^2?1)+1)2

\param cosHalfVector
   n?h

\param roughness
  roughness = sqrt(?) in above equation

*/
/****************************************************************************/
float ndfGGX(float cosHalfVector, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosHalfVector * cosHalfVector) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

/****************************************************************************/
/*!
\brief
The Geometry function takes a material's roughness
parameter as input with rougher surfaces having a higher probability of
overshadowing microfacets. The geometry function we will use is a combination
of the GGX and Schlick-Beckmann approximation known as Schlick-GGX:

gaSchlickG1(n,v,k)=n?v / (n?v)(1?k)+k

\param cosTheta
   n?v

\param k
  roughness value

*/
/****************************************************************************/
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

/****************************************************************************/
/*!
\brief
"To effectively approximate the geometry we need to take account of both
the view direction (geometry obstruction) and the light direction vector
(geometry shadowing). We can take both into account using Smith's method:"

G(n,v,l,k)=Gsub(n,v,k)Gsub(n,l,k)

\param cosLightDir
   n?v

\param cosViewDir
   n?l

\param roughness
  roughness value

*/
/****************************************************************************/
float gaSchlickGGX(float cosLightDir, float cosViewDir, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return gaSchlickG1(cosLightDir, k) * gaSchlickG1(cosViewDir, k);
}

/****************************************************************************/
/*!
\brief
"The Fresnel equation (pronounced as Freh-nel) describes the ratio of
light that gets reflected over the light that gets refracted,
which varies over the angle we're looking at a surface. The moment
light hits a surface, based on the surface to view angle the Fresnel
equation tells us the percentage of light that gets reflected. From this
ratio of reflection and the energy conservation principle we can directly
obtain the refracted portion of light from its remaining energy."

The Fresnel equation can be approximated using the Fresnel-Schlick approximation:

FSchlick(h,v,F0) = F0 + (1?F0)(1?(h?v))^5


\param F0
  the surface reflection at zero incidence

\param cosTheta
  h?v

*/
/****************************************************************************/
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float depthCheck(float4 lightSpacePos, float NdotL)
{
    float2 shadowTexCoords;
    shadowTexCoords.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
    shadowTexCoords.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5f);
    float pixelDepth = lightSpacePos.z / lightSpacePos.w;

    float lighting = 1;

    if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
        (saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
        (pixelDepth > 0))
    {
        if (NdotL < -1)
        {
            float margin = acos(saturate(NdotL));
            float epsilon = 0.001 / margin;

            epsilon = clamp(epsilon, 0, 0.05);

            float2 texelSize;
            ShadowMap.GetDimensions(texelSize.x, texelSize.y);
            texelSize = 1 / texelSize;
        
            for (int x = -2; x <= 2; ++x)
            {
                for (int y = -2; y <= 2; ++y)
                {
                    lighting += float(ShadowMap.SampleCmpLevelZero(shadowSampler, shadowTexCoords + float2(x, y) * texelSize, pixelDepth).r) > 0 ? 1 : 0.45f;
                }
            }
        
            lighting /= 18;
        }
    }

    return lighting;
}

/*===========================================================================*\
|| --------------------------- MAIN FUNCTION ------------------------------- ||
\*===========================================================================*/
PS_OUTPUT main(PS_INPUT IN) : SV_TARGET
{  
    PS_OUTPUT OUT;
    
    
    float3 albedo = AlbedoTexture.Sample(Sampler, IN.texCoords).rgb;
    
    if (albedo.r == 0 && albedo.g == 1 && albedo.b == 0)
    {
        OUT.Color = float4(0, 1, 0, 1);
        OUT.Bloom = float4(0, 0, 0, 1);
        return OUT;
    }

    
    float3 result = float3(0, 0, 0);
    float SSAO = 1 - SSAOTexture.Sample(Sampler, IN.texCoords).r;
    float3 normal = normalize(NormalTexture.Sample(Sampler, IN.texCoords).rgb);
    float3 worldPosition = PositonTexture.Sample(Sampler, IN.texCoords).rgb;
    float4 lightSpacePosition = mul(float4(worldPosition, 1), mul(cameraMatrix, projectionMatrix));
    float3 lightRay = cameraMatrix[3].rgb - worldPosition;
    float3 viewPos = ViewportTexture.Sample(Sampler, IN.texCoords).rgb;
    float3 MRAO = MetalRoughAOTexture.Sample(Sampler, IN.texCoords).rgb;
    float roughness = MRAO.r;
    float metalness = MRAO.g;
    float AO = MRAO.b;
    

    
    // Outgoing light direction (vector from world-space fragment position to the "eye").
    float3 viewDir = normalize(viewPos - worldPosition);

    // Angle between surface normal and outgoing light direction.
    float cosViewDir = max(0.0, dot(normal, viewDir));

    // Specular reflection vector.
 //   float3 Lr = 2.0 * cosViewDir * normal - viewDir;

    // Fresnel reflectance at normal incidence (for metals use albedo color).
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metalness);

  //  matrix mvp = mul(float4(worldPosition, 1), MVPTexture.Sample(Sampler, IN.texCoords));
    
    for (int i = 0; i < light_count; ++i)
    {
        // direction vector between light and pixel position
        float3 lightDir = normalize(lights[i].position - worldPosition);

        if (lights[i].type > LIGHT_TYPE_SPOT)
        {
            lightDir = normalize(-lights[i].direction);
        }

        // calculate light attenuation -> radiance
        float dis = length(lights[i].position - worldPosition);
        float attenuation = 1.0 / ((dis * dis) / 10);
        
     //   float3 lightScreenPos = mul(float4(lights[i].position, 1), mvp).rgb;
     //  float disScreen = length(lightScreenPos - IN.position.xyz);
     //   float attenuationScreen = 1.0 / ((disScreen * disScreen) / (widthHeight.y * 50));

        if (lights[i].type > LIGHT_TYPE_SPOT)
        {
            attenuation = 1.0f;
        }

        //  pow((1 + 0.007f * dis + 0.0002 * (dis * dis)), 2);
     //   float3 lightScreenRadiance = lights[i].color * attenuationScreen;
        float3 lightRadiance = lights[i].color * attenuation;

        // Half-vector between lightDir and viewDir.
        float3 halfVector = normalize(lightDir + viewDir);

      // Calculate angles between surface normal and various light vectors.
        float cosLightDir = max(0.0, dot(normal, lightDir));
        float cosHalfVector = max(0.0, dot(normal, halfVector));

    // Calculate Fresnel term for direct lighting.
        float3 F = fresnelSchlick(F0, max(0.0, dot(halfVector, viewDir)));
  // Calculate normal distribution for specular BRDF.
        float D = ndfGGX(cosHalfVector, roughness);
// Calculate geometric attenuation for specular BRDF.
        float G = gaSchlickGGX(cosLightDir, cosViewDir, roughness);

// energy conservation --  we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
// float3 kd = float3(1, 1, 1) - F;
// kd *= 1.0 - metalness;

 // Lambert diffuse BRDF.
        float3 diffuseBRDF = kd * albedo / PI;

// Cook-Torrance specular microfacet BRDF.
        float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLightDir * cosViewDir);

        result += ((diffuseBRDF + specularBRDF) * lightRadiance * cosLightDir) /*+ lightScreenRadiance*/;
    }

    float3 R = normalize((2 * dot(-normal, viewDir) * -normal) - viewDir);
    float3 specular = metalness * skyBox.SampleLevel(Sampler, R, roughness * 12).rgb;

    float3 ambient = ((0.5 * albedo) + specular) * AO;

    result += ambient;
    result *= SSAO;
    
    if (normal.y > 0.8)
        result *= depthCheck(lightSpacePosition, dot(normalize(-normal), lightRay));
    else
        result *= depthCheck(lightSpacePosition, dot(normalize(normal), lightRay));
   
    OUT.Color = float4(result.r, result.g, result.b, 1.0);
    
    float brightness = dot(OUT.Color.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    if (brightness > 1.f)
        OUT.Bloom = OUT.Color;
    else
        OUT.Bloom = float4(0,0,0,1);
    
    return OUT;
}