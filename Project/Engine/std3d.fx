#ifndef _STD3D
#define _STD3D

#include "value.fx"

//// Test Light 3D
//static float3 LightPos = float3(0.f, 0.f, 0.f);
//static float3 LightDir = float3(1.f, -1.f, 0.f);
//static float3 LightColor = float3(1.f, 1.f, 1.f);

//static float3 Ambient = float3(0.15f, 0.15f, 0.15f);

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD; 
    float3 vWorldNormal : NORMAL;
};

VS_OUT VS_Std3D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    // 광원 정보
    tLightInfo LightInfo = g_Light3D[0];
    
    // 방향성 벡터는 이동에 영향을 받으면 안됨. 그래서 동차좌표는 0으로 (4차원 좌표는 이동좌표)
    // 물체의 월드상에서의 표면의 수직 방향 계산
    output.vWorldNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWorld));
    
    // 정점에서 광원 연산하는 작업은 앞으로 진행 X (픽셀에서 진행)
    
    return output;
}

float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float3 vObjectColor = float3(1.f, 0.f, 1.f);
    
    // 광원 정보
    tLightInfo LightInfo = g_Light3D[0];
    
    
    // 램버트 코사인 법칙
    float LightPower = saturate(dot(normalize(_in.vWorldNormal), -normalize(LightInfo.WorldDir.xyz)));
    
    
    // 반사 Reflect
    float3 vReflect = LightInfo.WorldDir.xyz + dot(-LightInfo.WorldDir.xyz, _in.vWorldNormal) * 2.f * _in.vWorldNormal;
    
    // HLSL 내부 함수 => reflect()
    
    // 각 꼭짓점에 위치한 정점의 광원 세기를 바탕으로 보간해서 색을 지정한다.
    // 최종 색상 = ObjectColor * LightColor * LightPower + (ObjectColor * Ambient);
    float3 vOut = vObjectColor * LightInfo.Light.vDiffuse.rgb * LightPower
                 + (vObjectColor * LightInfo.Light.vAmbient.rgb);
    
    
    return float4(vOut, 1.f);
}

#endif