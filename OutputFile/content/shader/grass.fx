#ifndef _GRASS
#define _GRASS

#include "value.fx"
#include "struct.fx"

#define GrassWidth          300.f
#define GrassHeight         300.f

#define time                Time
#define windSpeed           5.f
#define windStrength        5.f

// Grass ��ġ�� ũ�� ������ ���� ��� ����
//cbuffer GrassData : register(b0)
//{
//    float GrassWidth;       // Grass ��ü�� �ʺ�
//    float GrassHeight;      // Grass ��ü�� ����
//}

struct VS_GRASS_IN
{
    float3 vPos : POSITION;
};

struct VS_GRASS_IN_Inst
{
    float3 vPos : POSITION;
    
    // Per Instance Data
    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
};

struct VS_GRASS_OUT
{
    float4 vPosition    : SV_Position;
    
    float3 vViewPos     : POSITION;
};

// Vertex Shader: Geometry Shader�� ������ ����
VS_GRASS_OUT VS_Grass(VS_GRASS_IN _in)
{
    VS_GRASS_OUT output;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV);
    
    return output;
}

//VS_GRASS_OUT VS_Grass_Inst(VS_GRASS_IN_Inst _in)
//{
//    VS_GRASS_OUT output = (VS_GRASS_OUT) 0.f;
//    
//    // �ν��Ͻ��� ���� ��ǥ ��ȯ
//    float4 worldPos = mul(float4(_in.vPos, 1.f), _in.matWorld);
//
//    // ȭ�� ���� ��ǥ�� ��ȯ
//    output.vPosition = mul(worldPos, _in.matWVP); // g_matWVP�� ���� ���
//
//    // �� ���� ��ǥ ���
//    output.vViewPos = mul(worldPos, _in.matWV); // g_matWV�� ���� ���
//    
//    return output;
//}

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

// Geometry Shader: Point Mesh�� ���� Grass ��ü�� Ȯ��
[maxvertexcount(6)]
void GS_Grass(point VS_GRASS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    // Grass ��ġ�� ���� �������� �� �������� ��ȯ
    float4 vViewPos = float4(_in[0].vViewPos, 1.f);

    // Grass ��ü�� �����ϴ� �簢���� �� ���� ����
    GS_OUT output[4];
    
    // ��� ���� (���� ���, ���� ���)
    output[0].vPosition = float4(-GrassWidth * 0.5f, GrassHeight, 0.f, 1.f); // ���� ���
    output[1].vPosition = float4(GrassWidth * 0.5f, GrassHeight, 0.f, 1.f); // ���� ���
    
    // �ϴ� ���� (���� �ϴ�, ���� �ϴ�) - �ٶ��� ������ ���� ����
    output[2].vPosition = float4(GrassWidth * 0.5f, 0.f, 0.f, 1.f); // ���� �ϴ�
    output[3].vPosition = float4(-GrassWidth * 0.5f, 0.f, 0.f, 1.f); // ���� �ϴ�

    // ��� �� �������� �ٶ��� ���� ��鸲 ���� (X�� �������θ�)
    output[0].vPosition.x += sin(time * windSpeed + _in[0].vViewPos.z * 0.1f) * windStrength; // ���� ��� ��鸲
    output[1].vPosition.x += sin(time * windSpeed + _in[0].vViewPos.z * 0.1f) * windStrength; // ���� ��� ��鸲

    // ���� ��ǥ���� ������������ ��ȯ
    for (int j = 0; j < 4; ++j)
    {
        output[j].vPosition.xyz += vViewPos.xyz;
        output[j].vPosition = mul(output[j].vPosition, g_matProj);
    }

    // UV ��ǥ ����
    output[0].vUV = float2(0.0f, 0.0f); // ���� ���
    output[1].vUV = float2(1.0f, 0.0f); // ���� ���
    output[2].vUV = float2(1.0f, 1.0f); // ���� �ϴ�
    output[3].vUV = float2(0.0f, 1.0f); // ���� �ϴ�

    // �簢���� �����ϴ� �� �ﰢ���� ��� ��Ʈ���� �߰�
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();

    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
}

float4 PS_Grass(GS_OUT _in) : SV_Target
{
    float4 color;
    if (g_btex_0)
    {
        color = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    else
    {
        color = float4(0.0f, 1.0f, 0.0f, 1.0f); // �ؽ�ó�� ���� �� �ʷϻ� �⺻ ����
    }
    return color;
}

#endif