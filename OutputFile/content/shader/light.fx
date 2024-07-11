#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"

// =============================
//  DirLightShader
//  Domain  : DOMAIN_LIGHTING
//  MRT     : Light MRT
//  Mesh    : RectMesh
#define LightIdx        g_int_0
#define PositionTarget  g_tex_0
#define NormalTarget    g_tex_1
// =============================

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_DirLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // Directional Light 는 화면의 모든 픽셀에 대해서 픽셀쉐이더가 호출되도록 한다.
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vSpecular : SV_Target1;
};

PS_OUT PS_DirLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // 호출된 픽셀과 동일한 위치에 기록된 Position 값을 가져온다.
    float4 vPosition = PositionTarget.Sample(g_sam_0, _in.vUV);
    
    // Position 값이 존재하지 않으면, 빛을 받을 물체가 존재하지 않는다.
    if (vPosition.a == 0.f)
    {
        discard;
    }
    
    // 호출된 픽셀과 동일한 위치에 기록된 Normal 값을 가져온다.
    float4 vNormal = NormalTarget.Sample(g_sam_0, _in.vUV);
    
    tLight light = (tLight) 0.f;
    CalLight3D(LightIdx, vPosition.xyz, vNormal.xyz, light);
    
    output.vDiffuse = light.vDiffuse + light.vAmbient;
    output.vSpecular = light.vMaxSpecular;
    
    output.vDiffuse.a = vPosition.z;
    output.vSpecular.a = vPosition.z;
    
    return output;
}


// =============================
//  PointLightShader
//  Domain  : DOMAIN_LIGHTING
//  MRT     : Light MRT
//  Mesh    : RectMesh
#define LightIdx        g_int_0
#define PositionTarget  g_tex_0
#define NormalTarget    g_tex_1
// =============================

VS_OUT VS_PointLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // Point Light 는 Volume Mesh 를 실제 광원 위치에 배치함
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

PS_OUT PS_PointLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // 호출될 픽셀 쉐이더의 픽셀 좌표 / Render Target 해상도
    float2 vScreenUV = _in.vPosition.xy / vResolution;
    
    // 호출될 픽셀과 동일한 위치에 기록된 Position 값을 가져온다.
    float4 vPosition = PositionTarget.Sample(g_sam_0, vScreenUV);

    // Position 값이 존재하지 않으면, 빛을 받을 물체가 존재하지 않는다.
    if (vPosition.a == 0.f)
    {
        discard;
    }
    
    // 기록되어있는 Position 값을 Volume Mesh 의 Local Space 로 이동시킨다.
    float4 vLocalPos = mul(mul(float4(vPosition.xyz, 1.f), g_matViewInv), g_matWorldInv);
    
    if (length(vLocalPos.xyz) < 0.5f)
    {
        output.vDiffuse = float4(0.f, 0.f, 1.f, 1.f);
        return output;
    }
    else
    {
        output.vDiffuse = float4(1.f, 0.f, 0.f, 1.f);
        return output;
    }
    
    //if (length(vLocalPos.xyz) > 0.5f)
    //{
    //    discard;
    //}
    //else
    //{
    //    output.vDiffuse = float4(1.f, 0.f, 0.f, 1.f);
    //    return output;
    //}
    
    // 최종적으로 모든 과정을 통과를 했다면,
    float4 vNormal = NormalTarget.Sample(g_sam_0, vScreenUV);
    
    tLight light = (tLight) 0.f;
    
    CalLight3D(LightIdx, vPosition.xyz, vNormal.xyz, light);
      
    output.vDiffuse = light.vDiffuse + light.vAmbient;
    output.vSpecular = light.vMaxSpecular;
      
    output.vDiffuse.a = vPosition.z;
    output.vSpecular.a = vPosition.z;
    
    return output;
}

#endif