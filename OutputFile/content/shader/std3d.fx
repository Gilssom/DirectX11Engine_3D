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
    float LightPower : FOG;
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
    float3 vWorldNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWorld));
    float3 vLightDir = normalize(LightInfo.WorldDir);
    
    // 노말 방향과 광원 방향을 구하기 위해서는 광원의 방향을 뒤집어야 한다.
    // 두 방향 사이의 각도가 즉 빛을 받는 광원의 세기
    
    // 광원의 방향과, 표면의 법선 벡터가 이루는 각도는 각각 단위 벡터로,
    // 내적을 진행하면 결국 Cos 값이 나오기 때문에 이루는 각도로 값을 구한다.
    output.LightPower = saturate(dot(vWorldNormal, -vLightDir));
    
    
    // 정점의 월드 상의 노말 방향을 픽셀 쉐이더에 보간해서 넘긴다.
    output.vWorldNormal = vWorldNormal;
    
    
    return output;
}

float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float3 vObjectColor = float3(1.f, 0.f, 1.f);
    
    // 광원 정보
    tLightInfo LightInfo = g_Light3D[0];
    
    float LightPower = saturate(dot(normalize(_in.vWorldNormal), normalize(LightInfo.WorldDir.xyz)));
    
    // 각 꼭짓점에 위치한 정점의 광원 세기를 바탕으로 보간해서 색을 지정한다.
    // 최종 색상 = ObjectColor * LightColor * LightPower + (ObjectColor * Ambient);
    
    if(g_int_0)
    {
        float3 vOut = vObjectColor * LightInfo.Light.vDiffuse.rgb * LightPower
                 + (vObjectColor * LightInfo.Light.vAmbient.rgb);
        
        return float4(vOut, 1.f);
    }
    else
    {
        float3 vOut = vObjectColor * LightInfo.Light.vDiffuse.rgb * _in.LightPower
                 + (vObjectColor * LightInfo.Light.vAmbient.rgb);
    
        return float4(vOut, 1.f);
    }
}

#endif