#ifndef _TESS
#define _TESS

#include "value.fx"
#include "func.fx"


struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV  : TEXCOORD;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float2 vUV  : TEXCOORD;
};

VS_OUT VS_Tess(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 정점 Shader 는 어떠한 역할도 수행하지 않는다.
    // 입력값을 Hull Shader 에 그대로 넘겨준다.
    
    // 하지만 없어서는 안된다. Input Assembler 단계에서 입력 받은 정보를
    // 다음 단계로 넘겨주는 필수 과정이기 때문이다.
    
    output.vPos = _in.vPos;
    output.vUV  = _in.vUV;
    
    return output;
}


// ==============
// Hull Shader
// ==============
struct TessLevel
{
    float arrEdge[3]    : SV_TessFactor;        // 제어점의 분할
    float Inside        : SV_InsideTessFactor;  // 내부의 분할
};

// Patch 상수 함수
// 하나의 Patch (도형) 당 호출되는 함수
TessLevel PatchConstFunc(InputPatch<VS_OUT, 3> _Patch, uint _PatchID : SV_PrimitiveID)
{
    TessLevel Level = (TessLevel) 0.f;
    
    Level.arrEdge[0] = 2.f;
    Level.arrEdge[1] = 2.f;
    Level.arrEdge[2] = 2.f;
    Level.Inside = 2.f;
    
    return Level;
}

struct HS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

// 정점 당 호출되는 함수
[domain("tir")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[maxtessfactor(64)]
[patchconstantfunc(PatchConstFunc)]
HS_OUT HS_Tess(InputPatch<VS_OUT, 3> _Patch
                , uint _VtxIdx : SV_OutputControlPointID
                , uint _PatchID : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT) 0.f;
    
    output.vPos = _Patch[_VtxIdx].vPos;
    output.vUV = _Patch[_VtxIdx].vUV;
    
    return output;
}


// ==================
// Domain Shader
// ==================
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

[domain("tri")]
DS_OUT DS_Tess(const OutputPatch<HS_OUT, 3> _Patch
                , float3 _Weight : SV_DomainLocation /* 가중치 */
                , TessLevel _TessLevel)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    float3 vLocalPos = (_Patch[0].vPos * _Weight[0])
                     + (_Patch[1].vPos * _Weight[1]) 
                     + (_Patch[2].vPos * _Weight[2]);

    float2 vUV = (_Patch[0].vUV * _Weight[0])
               + (_Patch[1].vUV * _Weight[1])
               + (_Patch[2].vUV * _Weight[2]);
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
    
    return output;
}

float4 PS_Tess(DS_OUT _in) : SV_Target // 반환 타입
{
    float4 vColor = (float4) 0.f;
    
    vColor = float4(1.f, 0.f, 1.f, 1.f);
    
    return vColor;
}

#endif