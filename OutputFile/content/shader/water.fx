#ifndef _WATER
#define _WATER

#include "value.fx"
#include "struct.fx"

Texture2D reflectionTex     : register(t0);     // Reflection Texture
Texture2D waterNormalTex    : register(t1);     // Water Normal Texture

#define time                Time
#define cameraPos           g_vec4_0
#define objectScale         g_vec2_0
#define fresnelPower        g_float_3       // Fresnel 효과의 강도
//#define normalStrength      g_float_1       // 물결 왜곡 강도

#define waveSpeed           g_float_0       // 파도 속도
#define waveAmplitude       g_float_1       // 파도 높이
#define waveFrequency       g_float_2       // 파도 빈도 (주기)

struct VS_INPUT
{
    float4 pos      : POSITION;
    float2 tex      : TEXCOORD;
    float3 normal   : NORMAL;
};

struct PS_INPUT
{
    float4 pos      : SV_POSITION;
    float2 tex      : TEXCOORD;
    float3 worldPos : TEXCOORD1;
    float3 normal   : NORMAL;
};

PS_INPUT VS_Water(VS_INPUT input)
{
    PS_INPUT output;

    // Z축을 기준으로 파동 효과 계산
    float waveOffset = sin(input.pos.x * waveFrequency + time * waveSpeed)
                     + cos(input.pos.y * waveFrequency + time * waveSpeed);
    waveOffset *= waveAmplitude;

    // 정점의 Z 위치를 파동에 따라 조정
    input.pos.z += waveOffset;

    // 변환된 위치와 텍스처 좌표 출력
    float4 worldPos = mul(input.pos, g_matWorld);
    output.pos = mul(worldPos, g_matView);
    output.pos = mul(output.pos, g_matProj);
    output.worldPos = worldPos.xyz;
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) g_matWorld); // Normal을 월드 공간으로 변환

    return output;
}

float4 PS_Water(PS_INPUT input) : SV_TARGET
{   
    // 1. 카메라 방향 및 물 표면 노멀 계산
    float3 viewDir = normalize(cameraPos.xyz - input.worldPos);
    float3 normal = normalize(input.normal);

    // 2. Water Normal Texture 샘플링으로 물결 왜곡 추가
    float2 animatedUV = input.tex + float2(time * 0.02, time * 0.03);
    float3 waterNormal = waterNormalTex.Sample(g_sam_0, animatedUV).xyz * 2.0 - 1.0;
    
    // 3. finalNormal 계산 시 더 낮은 강도로 설정
    float3 finalNormal = normalize(normal + waterNormal * 0.1f); // 0.1로 낮춰서 테스트

    // 3. Fresnel 효과 계산 (반사와 기본 색상 혼합 강도 조절)
    float fresnel = pow(1.0 - dot(viewDir, finalNormal), fresnelPower);

    // 4. 반사 텍스처 샘플링
    float3 reflectDir = reflect(-viewDir, finalNormal);
    float2 reflectionCoords = reflectDir.xy * 0.5 + 0.5;
    reflectionCoords.x = 1.0 - reflectionCoords.x;
    reflectionCoords = saturate(reflectionCoords);
    float4 reflectionColor = reflectionTex.Sample(g_sam_0, reflectionCoords);

    // 5. 기본 색상과 반사 색상 혼합
    float4 baseColor = float4(0.0, 0.3, 0.5, 1.0); // 물 기본 색상
    float4 finalColor = lerp(baseColor, reflectionColor, fresnel); // Fresnel로 혼합

    return finalColor;
}

#endif