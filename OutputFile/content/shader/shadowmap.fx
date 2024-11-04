#ifndef _SHADOWMAP
#define _SHADOWMAP

#include "value.fx"
#include "func.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float4 vProjPos : POSITION;
};

VS_OUT VS_ShadowMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    if (g_int_0)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal
              , _in.vWeights, _in.vIndices, 0);
    }
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vProjPos = output.vPosition;
    
    return output;
}

float PS_ShadowMap(VS_OUT _in) : SV_Target
{
    // ���� ��ǥ�� �����ؼ� ���� z �� / w =>> ��¥ ���� ��ǥ���� z ��
    float ProjZ = _in.vProjPos.z / _in.vProjPos.w;
    ProjZ = saturate(ProjZ); // 0~1 ���̷� �����Ͽ� Ȯ���� �մϴ�.

    return ProjZ;
}

#endif