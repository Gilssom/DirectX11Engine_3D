#ifndef _REFLECTION
#define _REFLECTION

#include "value.fx"
#include "struct.fx"

struct VS_INPUT
{
    float3 pos      : POSITION; // ���� ��ġ
    float3 normal   : NORMAL; // ���� ���
};

struct PS_INPUT
{
    float4 pos      : SV_POSITION; // Ŭ�� ���� ��ġ
    float3 worldPos : TEXCOORD0; // ���� ���� ��ġ
    float3 normal   : TEXCOORD1; // ���� ���� ���
};

// Reflection Vertex Shader
PS_INPUT VS_Reflection(VS_INPUT input)
{
    PS_INPUT output;

    // ���� ���� ��ġ ���
    float4 worldPos = mul(float4(input.pos, 1.0), g_matWorld);
    output.worldPos = worldPos.xyz;

    // ���� ���� ��� ���
    output.normal = mul((float3x3) g_matWorld, input.normal);

    // Ŭ�� ���� ��ġ ��� (view-projection ����)
    output.pos = mul(worldPos, g_matView);
    output.pos = mul(output.pos, g_matProj);

    return output;
}

// Reflection Pixel Shader
float4 PS_Reflection(PS_INPUT input) : SV_Target
{
    // Ŭ���� ��鿡 ���� �Ÿ� ���
    float distance = dot(float4(input.worldPos, 1.0), clipPlane);

    // �� ǥ�� �Ʒ��� �ִ� �ȼ��� ���������� ����
    if (distance < 0)
    {
        discard;
    }

    // �⺻ �ݻ� ���� ���� (������ ���÷� ����)
    // �����δ� ��鿡 �´� �ؽ�ó ���ø��̳� ������ ����� �ʿ��� �� ����
    float4 color = float4(0.8, 0.9, 1.0, 1.0); // ���÷� ���� �Ķ��� ����
    
    return color;
}

#endif