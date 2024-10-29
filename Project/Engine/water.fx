#ifndef _WATER
#define _WATER

#include "value.fx"
#include "struct.fx"

Texture2D reflectionTex     : register(t0);     // Reflection Texture
Texture2D waterNormalTex    : register(t1);     // Water Normal Texture
Texture2D emissiveTex       : register(t2);     // Emissive Texture (Optional)

#define time                Time
#define cameraPos           g_vec4_0
#define objectScale         g_vec2_0
//#define fresnelPower        g_float_0       // Fresnel 효과의 강도
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
    // 노멀 텍스처 샘플링 및 애니메이션을 위한 좌표 계산
    float2 animatedUV = input.tex + float2(time * 0.02, time * 0.03);
    float3 waterNormal = waterNormalTex.Sample(g_sam_0, animatedUV).xyz * 2.0 - 1.0;

    // 기본 색상과 노멀 효과
    float4 baseColor = float4(0.0, 0.3, 0.5, 1.0); // 물의 기본 색상
    float3 finalNormal = normalize(input.normal + waterNormal * 0.3);

    // 간단한 라이트 계산 (빛 반사 효과)
    float lightIntensity = saturate(dot(finalNormal, normalize(float3(0.0, 1.0, 0.5))));
    float4 finalColor = baseColor * lightIntensity;

    // ======= Emissive 처리 =======
    float4 vEmissive = MtrlData.vEmv; // 기본 발광값

    // Emissive 텍스처가 있다면 텍스처 샘플링하여 Emissive 효과 추가
    if (g_btex_2)
    {
        vEmissive += emissiveTex.Sample(g_sam_0, input.tex) * MtrlData.vEmv;
    }

    // Emissive 효과를 적용한 최종 색상
    finalColor.rgb += vEmissive.rgb;

    return finalColor;
}

#endif