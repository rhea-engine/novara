// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
    
struct Light
{
    float3 Direction;
    float Type;
    float4 Position;
    float4 Color;
    float4 Diffuse;
    float4 Specular;
    
    float SpotAngle;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
};

cbuffer cbPerFrame : register(b0)
{
    float4x4 V;
    float4x4 P;
    float4x4 VP;
    float4 cameraPosition;
    float4 frameTime;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 worldMatrix;
    float4x4 invModel;
};

cbuffer cbLight : register(b2)
{
    Light lights[8];
    float4 ambient;
    float lightCount;
};

cbuffer cbMaterial : register(b3)
{
    float4 mat_diffuse;
    float4 mat_ambient;
    float4 mat_specular;
    float4 mat_emissive;
    float mat_shininess;
    float has_normal_map;
}

Texture2D BaseColorTexture;
Texture2D NormalMap;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    float4 positionWS : TEXCOORD1;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

VS_OUTPUT VShader(float4 position : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD, float3 tangent : TANGENT)
{
    VS_OUTPUT output;

    float3 worldPos = mul(position, worldMatrix);
    output.normal = normalize(mul(normal, (float3x3) transpose(invModel)));
    output.position = mul(float4(worldPos, 1.0), VP);
    output.tex = tex;
    output.positionWS = float4(worldPos, 1.0);
    
    output.tangent = normalize(mul(tangent, (float3x3) worldMatrix)).xyz;
    // Compute binormal (cross product of normal and tangent)
    output.bitangent = normalize(cross(output.tangent, output.normal));
    
    return output;
}

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

float4 DoSpecularPhong(Light light, float3 V, float3 L, float3 N)
{
    // Phong lighting.
    float3 R = normalize(reflect(-L, N));
    float RdotV = max(0, dot(R, V));

    return light.Color * pow(RdotV, mat_shininess);
}

float4 DoSpecularBlinnPhong(Light light, float3 V, float3 L, float3 N)
{
    // Blinn-Phong lighting
    float3 H = normalize(L + V);
    float NdotH = max(0, dot(N, H));

    return light.Color * pow(NdotH, mat_shininess);
}

float4 DoDiffuse(Light light, float3 L, float3 N)
{
    float NdotL = max(0, dot(N, L));
    return light.Color * NdotL;
}

LightingResult DoDirectionalLight(Light light, float3 View, float4 P, float3 N)
{
    LightingResult result;
 
    float3 L = normalize(-light.Direction.xyz);
    N = normalize(N);
    View = normalize(View);
 
    result.Diffuse = DoDiffuse(light, L, N);
    result.Specular = DoSpecularPhong(light, View, L, N);
 
    return result;
}

float DoAttenuation(Light light, float distance)
{
    float att = (light.ConstantAttenuation + light.LinearAttenuation * distance + light.QuadraticAttenuation * distance * distance);
    if (att == 0.0f)
        return 0.0f;
    float result = 1.0f / att;
    return result;
}

LightingResult DoPointLight(Light light, float3 V, float4 P, float3 N)
{
    LightingResult result;
 
    float3 L = (light.Position - P).xyz;
    float distance = length(L);
    L = L / distance;
 
    float attenuation = DoAttenuation(light, distance);
 
    result.Diffuse = DoDiffuse(light, L, N) * attenuation;
    result.Specular = DoSpecularPhong(light, V, L, N) * attenuation;
    
    return result;
}

float DoSpotCone(Light light, float3 L)
{
    float minCos = cos(light.SpotAngle);
    float maxCos = (minCos + 1.0f) / 2.0f;
    float cosAngle = dot(light.Direction.xyz, -L);
    return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoSpotLight(Light light, float3 V, float4 P, float3 N)
{
    LightingResult result;
 
    //float3 L = normalize(light.Position - P).xyz;
    //float distance = length(L);
    //L = L / distance;
    float3 L = (light.Position.xyz - P.xyz);
    float distance = length(L);
    L = L / distance;
 
    float attenuation = DoAttenuation(light, distance);
    float spotIntensity = DoSpotCone(light, L);
 
    result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity;
    result.Specular = DoSpecularPhong(light, V, L, N) * attenuation * spotIntensity;
    
    return result;
}

LightingResult ComputeLighting(float4 P, float3 N)
{
    LightingResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
    
    float3 View = normalize(cameraPosition - P).xyz;
    
    for (int i = 0; i < lightCount; i++)
    {
        LightingResult single_result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
        
        switch (lights[i].Type)
        {
            case DIRECTIONAL_LIGHT:
                single_result = DoDirectionalLight(lights[i], View, P, N);
                break;
            case POINT_LIGHT:
                single_result = DoPointLight(lights[i], View, P, N);
                break;
            case SPOT_LIGHT:
                single_result = DoSpotLight(lights[i], View, P, N);
                break;
        }
        result.Diffuse += single_result.Diffuse;
        result.Specular += single_result.Specular;
    }
    
    result.Diffuse = saturate(result.Diffuse);
    result.Specular = saturate(result.Specular);
    
    return result;
}

float4 PShader(VS_OUTPUT input) : SV_TARGET
{
    float3 V = normalize(cameraPosition.xyz - input.positionWS.xyz);
    float waveSize = 4.0;
    float2 worldUV = input.positionWS.xz / waveSize;
    float waterTime = frameTime.x;

    float2 uv1 = worldUV + float2(waterTime * 0.10, waterTime * 0.06);
    float2 uv2 = worldUV + float2(-waterTime * 0.07, waterTime * 0.11);

  //  float3 n1 = NormalMap.Sample(ObjSamplerState, uv1).xyz * 2.0 - 1.0;
  //  float3 n2 = NormalMap.Sample(ObjSamplerState, uv2).xyz * 2.0 - 1.0;
      float3 n1 = BaseColorTexture.Sample(ObjSamplerState, uv1).xyz * 2.0 - 1.0;
    float3 n2 = BaseColorTexture.Sample(ObjSamplerState, uv2).xyz * 2.0 - 1.0;

    float3 tangentNormal = normalize(float3(
        n1.x + n2.x,
        n1.y + n2.y,
        n1.z * n2.z
    ));
    
    tangentNormal.xy *= 1.5;
    tangentNormal = normalize(tangentNormal);
    
    float3x3 TBN = float3x3(
        normalize(input.tangent),
        normalize(input.bitangent),
        normalize(input.normal)
    );

    float3 N = normalize(mul(tangentNormal, TBN));

    float fresnel = pow(
        1.0 - saturate(dot(V, N)),
        5.0
    );

    float3 waterColor = lerp(
        float3(0.06, 0.25, 0.28),
        float3(0.30, 0.48, 0.55),
        fresnel
    );

    LightingResult lit = ComputeLighting(input.positionWS, N);

    float3 color = waterColor * (0.35 + lit.Diffuse.rgb * 0.65);
    color += lit.Specular.rgb * 0.10;
    
   // float3 color =
   //     waterColor * (ambient.rgb + lit.Diffuse.rgb * 0.35);
      //  + lit.Specular.rgb;

    float wavePattern =
    (n1.x + n2.y) * 0.5;

    color *= 1.0 + wavePattern * 0.25;
    
    float alpha = lerp(0.68, 0.80, fresnel);

    return float4(saturate(color), alpha);
}