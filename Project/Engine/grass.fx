#ifndef _GRASS
#define _GRASS

#include "value.fx"
#include "struct.fx"

#define GrassWidth          300.f
#define GrassHeight         300.f

#define time                Time
#define windSpeed           5.f
#define windStrength        5.f

// Grass 위치와 크기 설정을 위한 상수 버퍼
//cbuffer GrassData : register(b0)
//{
//    float GrassWidth;       // Grass 객체의 너비
//    float GrassHeight;      // Grass 객체의 높이
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

// Vertex Shader: Geometry Shader로 데이터 전달
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
//    // 인스턴스의 월드 좌표 변환
//    float4 worldPos = mul(float4(_in.vPos, 1.f), _in.matWorld);
//
//    // 화면 공간 좌표로 변환
//    output.vPosition = mul(worldPos, _in.matWVP); // g_matWVP를 직접 사용
//
//    // 뷰 공간 좌표 계산
//    output.vViewPos = mul(worldPos, _in.matWV); // g_matWV를 직접 사용
//    
//    return output;
//}

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

// Geometry Shader: Point Mesh를 단일 Grass 객체로 확장
[maxvertexcount(6)]
void GS_Grass(point VS_GRASS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    // Grass 위치를 월드 공간에서 뷰 공간으로 변환
    float4 vViewPos = float4(_in[0].vViewPos, 1.f);

    // Grass 객체를 구성하는 사각형의 각 정점 설정
    GS_OUT output[4];
    
    // 상단 정점 (좌측 상단, 우측 상단)
    output[0].vPosition = float4(-GrassWidth * 0.5f, GrassHeight, 0.f, 1.f); // 좌측 상단
    output[1].vPosition = float4(GrassWidth * 0.5f, GrassHeight, 0.f, 1.f); // 우측 상단
    
    // 하단 정점 (좌측 하단, 우측 하단) - 바람에 영향을 받지 않음
    output[2].vPosition = float4(GrassWidth * 0.5f, 0.f, 0.f, 1.f); // 우측 하단
    output[3].vPosition = float4(-GrassWidth * 0.5f, 0.f, 0.f, 1.f); // 좌측 하단

    // 상단 두 정점에만 바람에 의한 흔들림 적용 (X축 방향으로만)
    output[0].vPosition.x += sin(time * windSpeed + _in[0].vViewPos.z * 0.1f) * windStrength; // 좌측 상단 흔들림
    output[1].vPosition.x += sin(time * windSpeed + _in[0].vViewPos.z * 0.1f) * windStrength; // 우측 상단 흔들림

    // 월드 좌표에서 프로젝션으로 변환
    for (int j = 0; j < 4; ++j)
    {
        output[j].vPosition.xyz += vViewPos.xyz;
        output[j].vPosition = mul(output[j].vPosition, g_matProj);
    }

    // UV 좌표 설정
    output[0].vUV = float2(0.0f, 0.0f); // 좌측 상단
    output[1].vUV = float2(1.0f, 0.0f); // 우측 상단
    output[2].vUV = float2(1.0f, 1.0f); // 우측 하단
    output[3].vUV = float2(0.0f, 1.0f); // 좌측 하단

    // 사각형을 구성하는 두 삼각형을 출력 스트림에 추가
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
        color = float4(0.0f, 1.0f, 0.0f, 1.0f); // 텍스처가 없을 때 초록색 기본 색상
    }
    return color;
}

#endif