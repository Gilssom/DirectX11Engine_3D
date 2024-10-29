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
//#define fresnelPower        g_float_0       // Fresnel ȿ���� ����
//#define normalStrength      g_float_1       // ���� �ְ� ����

#define waveSpeed           g_float_0       // �ĵ� �ӵ�
#define waveAmplitude       g_float_1       // �ĵ� ����
#define waveFrequency       g_float_2       // �ĵ� �� (�ֱ�)

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

    // Z���� �������� �ĵ� ȿ�� ���
    float waveOffset = sin(input.pos.x * waveFrequency + time * waveSpeed)
                     + cos(input.pos.y * waveFrequency + time * waveSpeed);
    waveOffset *= waveAmplitude;

    // ������ Z ��ġ�� �ĵ��� ���� ����
    input.pos.z += waveOffset;

    // ��ȯ�� ��ġ�� �ؽ�ó ��ǥ ���
    float4 worldPos = mul(input.pos, g_matWorld);
    output.pos = mul(worldPos, g_matView);
    output.pos = mul(output.pos, g_matProj);
    output.worldPos = worldPos.xyz;
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) g_matWorld); // Normal�� ���� �������� ��ȯ

    return output;
}

float4 PS_Water(PS_INPUT input) : SV_TARGET
{
    // ��� �ؽ�ó ���ø� �� �ִϸ��̼��� ���� ��ǥ ���
    float2 animatedUV = input.tex + float2(time * 0.02, time * 0.03);
    float3 waterNormal = waterNormalTex.Sample(g_sam_0, animatedUV).xyz * 2.0 - 1.0;

    // �⺻ ����� ��� ȿ��
    float4 baseColor = float4(0.0, 0.3, 0.5, 1.0); // ���� �⺻ ����
    float3 finalNormal = normalize(input.normal + waterNormal * 0.3);

    // ������ ����Ʈ ��� (�� �ݻ� ȿ��)
    float lightIntensity = saturate(dot(finalNormal, normalize(float3(0.0, 1.0, 0.5))));
    float4 finalColor = baseColor * lightIntensity;

    // ======= Emissive ó�� =======
    float4 vEmissive = MtrlData.vEmv; // �⺻ �߱���

    // Emissive �ؽ�ó�� �ִٸ� �ؽ�ó ���ø��Ͽ� Emissive ȿ�� �߰�
    if (g_btex_2)
    {
        vEmissive += emissiveTex.Sample(g_sam_0, input.tex) * MtrlData.vEmv;
    }

    // Emissive ȿ���� ������ ���� ����
    finalColor.rgb += vEmissive.rgb;

    return finalColor;
}

#endif