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
    
    // ���� ����
    tLightInfo LightInfo = g_Light3D[0];
  
    
    // ���⼺ ���ʹ� �̵��� ������ ������ �ȵ�. �׷��� ������ǥ�� 0���� (4���� ��ǥ�� �̵���ǥ)
    // ��ü�� ����󿡼��� ǥ���� ���� ���� ���
    float3 vWorldNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWorld));
    float3 vLightDir = normalize(LightInfo.WorldDir);
    
    // �븻 ����� ���� ������ ���ϱ� ���ؼ��� ������ ������ ������� �Ѵ�.
    // �� ���� ������ ������ �� ���� �޴� ������ ����
    
    // ������ �����, ǥ���� ���� ���Ͱ� �̷�� ������ ���� ���� ���ͷ�,
    // ������ �����ϸ� �ᱹ Cos ���� ������ ������ �̷�� ������ ���� ���Ѵ�.
    output.LightPower = saturate(dot(vWorldNormal, -vLightDir));
    
    
    // ������ ���� ���� �븻 ������ �ȼ� ���̴��� �����ؼ� �ѱ��.
    output.vWorldNormal = vWorldNormal;
    
    
    return output;
}

float4 PS_Std3D(VS_OUT _in) : SV_Target
{
    float3 vObjectColor = float3(1.f, 0.f, 1.f);
    
    // ���� ����
    tLightInfo LightInfo = g_Light3D[0];
    
    float LightPower = saturate(dot(normalize(_in.vWorldNormal), normalize(LightInfo.WorldDir.xyz)));
    
    // �� �������� ��ġ�� ������ ���� ���⸦ �������� �����ؼ� ���� �����Ѵ�.
    // ���� ���� = ObjectColor * LightColor * LightPower + (ObjectColor * Ambient);
    
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