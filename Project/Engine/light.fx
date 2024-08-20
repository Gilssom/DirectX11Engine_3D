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
    
    // Directional Light �� ȭ���� ��� �ȼ��� ���ؼ� �ȼ����̴��� ȣ��ǵ��� �Ѵ�.
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
    
    // ȣ��� �ȼ��� ������ ��ġ�� ��ϵ� Position ���� �����´�.
    float4 vPosition = PositionTarget.Sample(g_sam_0, _in.vUV);
    
    // Position ���� �������� ������, ���� ���� ��ü�� �������� �ʴ´�.
    if (vPosition.a == 0.f)
    {
        discard;
    }
    
    
    // ���� ���� ��ġ�� ���� ��ǥ�� �������� �����Ѵ�.
    float3 vWorldPos = mul(float4(vPosition.xyz, 1.f), g_matViewInv);
    
    float4 vLightProjPos = mul(float4(vWorldPos, 1.f), LIGHT_VP);

    float2 vShadowMapUV = vLightProjPos.xy / vLightProjPos.w;
    
    // ���� �������� �ٶ� ���� UV ��ǥ
    vShadowMapUV.x = vShadowMapUV.x / 2.f + 0.5f;
    vShadowMapUV.y = 1.f - (vShadowMapUV.y / 2.f + 0.5f);
    
    // ���� �������� �׸��ڸ� �Ǻ��� �κ��� ���� ���� ��
    float Depth = vLightProjPos.z / vLightProjPos.w;
    
    // �׸��� �ʿ� ��ϵǾ� �ִ� ���� ��
    float fShadowPower = 0.f;
    float fShadowDepth = SHADOW_MAP.Sample(g_sam_0, vShadowMapUV);

    // fShadowDepth �� ���� Depth �� ���� ������ �׸��� ������ �ȴ�.
    // ���� �������� ���� ���߿� �� ����� ������ ���� ���� ������ �����̴�.
    if (vShadowMapUV.x >= 0.f && vShadowMapUV.x <= 1.f
        && vShadowMapUV.y >= 0.f && vShadowMapUV.y <= 1.f)
    {
        // �׸��� ����
        if (fShadowDepth + 0.001f < Depth)
        {
            fShadowPower = 0.9f;
        }
    }
    
    // �� ��� ����
    // ȣ��� �ȼ��� ������ ��ġ�� ��ϵ� Normal ���� �����´�.
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
    
    // Point Light �� Volume Mesh �� ���� ���� ��ġ�� ��ġ��
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

PS_OUT PS_PointLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // ȣ��� �ȼ� ���̴��� �ȼ� ��ǥ / Render Target �ػ�
    float2 vScreenUV = _in.vPosition.xy / vResolution;
    
    // ȣ��� �ȼ��� ������ ��ġ�� ��ϵ� Position ���� �����´�.
    float4 vPosition = PositionTarget.Sample(g_sam_0, vScreenUV);

    // Position ���� �������� ������, ���� ���� ��ü�� �������� �ʴ´�.
    if (vPosition.a == 0.f)
    {
        discard;
    }
    
    // ��ϵǾ��ִ� Position ���� Volume Mesh �� Local Space �� �̵���Ų��.
    float4 vLocalPos = mul(mul(float4(vPosition.xyz, 1.f), g_matViewInv), g_matWorldInv);
    
    if (length(vLocalPos.xyz) > 0.5f)
    {
        discard;
    }
    
    // ���������� ��� ������ ����� �ߴٸ�,
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
    
    // Spot Light �� Volume Mesh �� ���� ���� ��ġ�� ��ġ��
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

PS_OUT PS_SpotLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // ȣ��� �ȼ� ���̴��� �ȼ� ��ǥ / Render Target �ػ�
    float2 vScreenUV = _in.vPosition.xy / vResolution;
    
    // ȣ��� �ȼ��� ������ ��ġ�� ��ϵ� Position ���� �����´�.
    float4 vPosition = PositionTarget.Sample(g_sam_0, vScreenUV);

    // Position ���� �������� ������, ���� ���� ��ü�� �������� �ʴ´�.
    if (vPosition.a == 0.f)
    {
        discard;
    }
    
    // ��ϵǾ��ִ� Position ���� Volume Mesh �� Local Space �� �̵���Ų��.
    float4 vLocalPos = mul(mul(float4(vPosition.xyz, 1.f), g_matViewInv), g_matWorldInv);
    
    if (length(vLocalPos.xyz) > 0.5f)
    {
        discard;
    }
    
    // ���������� ��� ������ ����� �ߴٸ�,
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