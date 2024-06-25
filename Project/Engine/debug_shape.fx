#ifndef _DEBUG_SHAPE
#define _DEBUG_SHAPE

#include "value.fx"


struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};


VS_OUT VS_DebugShape(float3 vLocalPos : POSITION, float2 vUV : TEXCOORD)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(mul(mul(float4(vLocalPos, 1.f), g_matWorld), g_matView), g_matProj);
    output.vUV = vUV;
    
    return output;
}


float4 PS_DebugShape(VS_OUT _in) : SV_Target
{
    // Object Material Setting 에서 Color 값을 받아옴
    return g_vec4_0;
}


#endif