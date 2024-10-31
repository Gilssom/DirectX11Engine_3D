#ifndef _REFLECTION
#define _REFLECTION

#include "value.fx"
#include "struct.fx"

struct VS_INPUT
{
    float3 pos      : POSITION; // 정점 위치
    float3 normal   : NORMAL; // 정점 노멀
};

struct PS_INPUT
{
    float4 pos      : SV_POSITION; // 클립 공간 위치
    float3 worldPos : TEXCOORD0; // 월드 공간 위치
    float3 normal   : TEXCOORD1; // 월드 공간 노멀
};

// Reflection Vertex Shader
PS_INPUT VS_Reflection(VS_INPUT input)
{
    PS_INPUT output;

    // 월드 공간 위치 계산
    float4 worldPos = mul(float4(input.pos, 1.0), g_matWorld);
    output.worldPos = worldPos.xyz;

    // 월드 공간 노멀 계산
    output.normal = mul((float3x3) g_matWorld, input.normal);

    // 클립 공간 위치 계산 (view-projection 적용)
    output.pos = mul(worldPos, g_matView);
    output.pos = mul(output.pos, g_matProj);

    return output;
}

// Reflection Pixel Shader
float4 PS_Reflection(PS_INPUT input) : SV_Target
{
    // 클리핑 평면에 따른 거리 계산
    float distance = dot(float4(input.worldPos, 1.0), clipPlane);

    // 물 표면 아래에 있는 픽셀을 렌더링에서 제외
    if (distance < 0)
    {
        discard;
    }

    // 기본 반사 색상 적용 (간단한 예시로 설정)
    // 실제로는 장면에 맞는 텍스처 샘플링이나 라이팅 계산이 필요할 수 있음
    float4 color = float4(0.8, 0.9, 1.0, 1.0); // 예시로 연한 파란색 적용
    
    return color;
}

#endif