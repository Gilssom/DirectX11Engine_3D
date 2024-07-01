#ifndef _STD3D
#define _STD3D

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD; 
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
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
    output.vViewPos     = mul(float4(_in.vNormal, 1.f), g_matWV);
    output.vViewNormal  = normalize(mul(float4(_in.vNormal, 0.f), g_matWV));
    
    // 정점에서 광원 연산하는 작업은 앞으로 진행 X (픽셀에서 진행)
    
    return output;
}

float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    // 기본적인 색상
    float3 vObjectColor = float3(1.f, 0.f, 1.f);
    float Alpha = 1.f;
    
    if(g_tex_0)
    {
        float4 vColor   = g_tex_0.Sample(g_sam_0, _in.vUV);
        vObjectColor    = vColor.rgb;
        Alpha           = vColor.a;
    }
    
    
    // Normal Map
    float3 vViewNormal = _in.vViewNormal;
    
    if(g_tex_1)
    {
        // Sample 로 얻은 값의 범위는 0 ~ 1, 실제 기록된 값의 의미는 -1 ~ 1
        float3 vNormal = g_tex_1.Sample(g_sam_0, _in.vUV);
        
        // 범위 보정
        vNormal = vNormal * 2.f - 1.f;
        
        float3x3 Rot =
        {
            // tangent
            // binormal
            // normal
        };

        vViewNormal = mul(vNormal, Rot);
    }
    
    
    // 광원의 세기 계산
    tLight Light = (tLight) 0.f;
  
    for (int i = 0; i < Light3DCount; i++)
    {
        CalLight3D(i, _in.vViewPos, vViewNormal, Light);
    }
    
    // 오브젝트 광원 적용
    float4 vOut = float4(0.f, 0.f, 0.f, 1.f);
    
    vOut.rgb =  vObjectColor.rgb * Light.vDiffuse.rgb 
                + vObjectColor.rgb * Light.vAmbient.rgb
                + vObjectColor.rgb * Light.vMaxSpecular.rgb;
    
    return vOut;
}

#endif