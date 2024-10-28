#ifndef _WATER
#define _WATER

#include "value.fx"
#include "struct.fx"

Texture2D reflectionTex     : register(t0);     // Reflection Texture
Texture2D waterNormalTex    : register(t1);     // Water Normal Texture

#define time                DeltaTime_Engine
#define cameraPos           g_vec4_0
#define fresnelPower        g_float_0       // Fresnel ȿ���� ����
#define normalStrength      g_float_1       // ���� �ְ� ����

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
    output.normal = mul(input.normal, (float3x3) g_matWorld); // Normal ��ȯ
    
    return output;
}

float4 PS_Water(PS_INPUT input) : SV_TARGET
{
   // 1. �� ����� ���� ���� ���
    float3 viewDir = normalize(cameraPos.xyz - input.worldPos);
    float3 normal = normalize(input.normal);

    // 2. Reflection Texture���� ���� ���ø�
    float3 reflectDir = reflect(-viewDir, normal);
    reflectDir.y = -reflectDir.y; // �ݻ� �� ����

    float2 reflectionCoords = reflectDir.xy * 0.5 + 0.5;
    float4 reflectionColor = reflectionTex.Sample(g_sam_0, reflectionCoords);

    // 3. ���� ������ �ݻ�� �ؽ�ó ���� �״�� ���
    return reflectionColor;
}

#endif