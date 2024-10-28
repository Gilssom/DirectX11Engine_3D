#ifndef _WATER
#define _WATER

#include "value.fx"
#include "struct.fx"

Texture2D reflectionTex     : register(t0);     // Reflection Texture
Texture2D waterNormalTex    : register(t1);     // Water Normal Texture

#define time                DeltaTime_Engine
#define cameraPos           g_vec4_0
#define fresnelPower        g_float_0       // Fresnel 효과의 강도
#define normalStrength      g_float_1       // 물결 왜곡 강도

struct VS_INPUT
{
    float4 pos      : POSITION;
    float2 tex      : TEXCOORD;
    float3 normal   : NORMAL;
};

struct PS_INPUT
{
    float4 pos          : SV_POSITION;
    float2 tex          : TEXCOORD;
    float3 worldPos     : TEXCOORD1;
    float3 normal       : NORMAL;
};

PS_INPUT VS_Water(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = mul(input.pos, g_matWorld);
    
    output.pos = mul(worldPos, g_matView);
    output.pos = mul(output.pos, g_matProj);
    output.worldPos = worldPos.xyz;
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) g_matWorld); // Normal 변환
    
    return output;
}

float4 PS_Water(PS_INPUT input) : SV_TARGET
{
   // 1. 뷰 방향과 법선 벡터 계산
    float3 viewDir = normalize(cameraPos.xyz - input.worldPos);
    float3 normal = normalize(input.normal);

    // 2. Reflection Texture에서 색상 샘플링
    float3 reflectDir = reflect(-viewDir, normal);
    reflectDir.y = -reflectDir.y; // 반사 축 조정

    float2 reflectionCoords = reflectDir.xy * 0.5 + 0.5;
    float4 reflectionColor = reflectionTex.Sample(g_sam_0, reflectionCoords);

    // 3. 최종 색상은 반사된 텍스처 색상 그대로 사용
    return reflectionColor;
}

#endif