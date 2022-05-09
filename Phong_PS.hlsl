#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
// fragment to light vector data
const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
// attenuation
const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
// diffuse
const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
// specular
const float3 specular = Speculate(
    //specularColor.rgb, 1.0f, viewNormal,
    //lv.vToL, viewFragPos, att, specularPower
    diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal,
    lv.vToL, viewFragPos, att, specularGloss
);
// final color
//return float4(saturate((diffuse + ambient) * materialColor.rgb + specular), 1.0f);
return float4(saturate((diffuse + ambient) * materialColor + specular), 1.0f);
}