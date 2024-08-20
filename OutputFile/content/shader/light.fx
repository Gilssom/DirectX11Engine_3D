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

#define SHADOW_MAP      g_tex_2
#define LIGHT_VP        g_mat_0
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
    
    
    // 빛을 받을 위치를 월드 좌표계 기준으로 변경한다.
    float3 vWorldPos = mul(float4(vPosition.xyz, 1.f), g_matViewInv);
    
    float4 vLightProjPos = mul(float4(vWorldPos, 1.f), LIGHT_VP);

    float2 vShadowMapUV = vLightProjPos.xy / vLightProjPos.w;
    
    // 광원 시점에서 바라본 최종 UV 좌표
    vShadowMapUV.x = vShadowMapUV.x / 2.f + 0.5f;
    vShadowMapUV.y = 1.f - (vShadowMapUV.y / 2.f + 0.5f);
    
    // 광원 시점에서 그림자를 판별할 부분의 기존 깊이 값
    float Depth = vLightProjPos.z / vLightProjPos.w;
    
    // 그림자 맵에 기록되어 있는 깊이 값
    float fShadowPower = 0.f;
    float fShadowDepth = SHADOW_MAP.Sample(g_sam_0, vShadowMapUV);

    // fShadowDepth 가 기존 Depth 값 보다 작으면 그림자 영역이 된다.
    // 기존 지점까지 가는 도중에 더 가까운 무언가의 깊이 값이 잡혔기 때문이다.
    if (vShadowMapUV.x >= 0.f && vShadowMapUV.x <= 1.f
        && vShadowMapUV.y >= 0.f && vShadowMapUV.y <= 1.f)
    {
        // 그림자 판정
        if (fShadowDepth + 0.001f < Depth)
        {
            fShadowPower = 0.9f;
        }
    }
    
    // 빛 계산 과정
    // 호출된 픽셀과 동일한 위치에 기록된 Normal 값을 가져온다.
    float4 vNormal = NormalTarget.Sample(g_sam_0, _in.vUV);
    
    tLight light = (tLight) 0.f;
    CalLight3D(LightIdx, vPosition.xyz, vNormal.xyz, light);
    
    output.vDiffuse = (light.vDiffuse + light.vAmbient) * (1.f - fShadowPower);
    output.vSpecular = light.vMaxSpecular * (1.f - fShadowPower);
    
    output.vDiffuse.a = vPosition.z;
    output.vSpecular.a = vPosition.z;
    
    return output;
}


// =============================
//  PointLightShader
//  Domain  : DOMAIN_LIGHTING
//  MRT     : Light MRT
//  Mesh    : SphereMesh
//#define LightIdx        g_int_0
//#define PositionTarget  g_tex_0
//#define NormalTarget    g_tex_1
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
    
    if (length(vLocalPos.xyz) > 0.5f)
    {
        discard;
    }
    
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


// =============================
//  SpotLightShader
//  Domain  : DOMAIN_LIGHTING
//  MRT     : Light MRT
//  Mesh    : ContMesh
//#define LightIdx        g_int_0
//#define PositionTarget  g_tex_0
//#define NormalTarget    g_tex_1
// =============================

VS_OUT VS_SpotLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // Spot Light 는 Volume Mesh 를 실제 광원 위치에 배치함
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

PS_OUT PS_SpotLight(VS_OUT _in)
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
    
    if (length(vLocalPos.xyz) > 0.5f)
    {
        discard;
    }
    
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