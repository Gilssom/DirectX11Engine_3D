#ifndef _GRASS_RENDER
#define _GRASS_RENDER

#include "value.fx"

cbuffer GRASS_CONSTANT_BUFFER : register(b0)
{
    //row_major float4x4 g_matWVP;
    float time; // 시간을 기반으로 풀 흔들림 효과 적용
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

    // 흔들림 효과를 적용한 위치 계산
    float offset = sin(time + input.TimeOffset) * 0.05f;
    float3 pos = input.Position;
    pos.x += offset;

    float4 worldPos = float4(pos, 1.0f);
    output.Position = mul(worldPos, g_matWVP);

    // 기본 텍스처 좌표 설정
    output.TexCoord = float2(0.5f, 0.5f);

    return output;
}

float4 PS_Grass(VS_OUT input) : SV_Target
{
    // Grass Texture 적용
    float4 color = GrassTexture.Sample(samLinear, input.TexCoord);

    // 알파 값으로 투명도 처리
    if (color.a < 0.1f)
        discard;

    return color;
}

#endif