#ifndef _GRASS_RENDER
#define _GRASS_RENDER

#include "value.fx"

cbuffer GRASS_CONSTANT_BUFFER : register(b0)
{
    //row_major float4x4 g_matWVP;
    float time; // �ð��� ������� Ǯ ��鸲 ȿ�� ����
}

Texture2D GrassTexture : register(t0);
SamplerState samLinear : register(s0);

struct VS_IN
{
    float3 Position : POSITION;
    float Rotation : ROTATION;
    float TimeOffset : TIMEOFFSET;
};

struct VS_OUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VS_OUT VS_Grass(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // ��鸲 ȿ���� ������ ��ġ ���
    float offset = sin(time + input.TimeOffset) * 0.05f;
    float3 pos = input.Position;
    pos.x += offset;

    float4 worldPos = float4(pos, 1.0f);
    output.Position = mul(worldPos, g_matWVP);

    // �⺻ �ؽ�ó ��ǥ ����
    output.TexCoord = float2(0.5f, 0.5f);

    return output;
}

float4 PS_Grass(VS_OUT input) : SV_Target
{
    // Grass Texture ����
    float4 color = GrassTexture.Sample(samLinear, input.TexCoord);

    // ���� ������ ���� ó��
    if (color.a < 0.1f)
        discard;

    return color;
}

#endif