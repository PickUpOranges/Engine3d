#include "Transform.hlsli"
#include "VShadow.hlsli"

struct VSOut
{
    float3 camPos  : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 shadowHomoPos : ShadowPosition;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord)
{
    VSOut vso;
    vso.camPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.normal = mul(n, (float3x3) modelView);
    vso.tan = mul(tan, (float3x3) modelView);
    vso.bitan = mul(bitan, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
    vso.shadowHomoPos = ToShadowHomoSpace(pos, model);
    return vso;
}