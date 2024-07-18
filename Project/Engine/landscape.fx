#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "value.fx"

// =============================
// LandScpae Shader
// MRT      : Deferred
// DOMAIN   : Deferred
// Mesh     : 자체 Mesh 
// Parameter
#define FACE_X          g_int_0
#define FACE_Z          g_int_1

#define Tess_Level      g_float_0

#define HEIGHT_MAP      g_tex_0
#define HasHeightMap    g_btex_0
// =============================

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_LandScape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    // 정점 Shader 는 어떠한 역할도 수행하지 않는다.
    // 입력값을 Hull Shader 에 그대로 넘겨준다.
    
    // 하지만 없어서는 안된다. Input Assembler 단계에서 입력 받은 정보를
    // 다음 단계로 넘겨주는 필수 과정이기 때문이다.
    
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    
    return output;
}

// ===========
// Hull Shader
// ===========
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
        
    Level.arrEdge[0]    = Tess_Level;
    Level.arrEdge[1]    = Tess_Level;
    Level.arrEdge[2]    = Tess_Level;
    Level.Inside        = Tess_Level;
    
    return Level;
}

struct HS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

// 정점 당 호출되는 함수
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[maxtessfactor(64)]
[patchconstantfunc("PatchConstFunc")]
HS_OUT HS_LandScape(InputPatch<VS_OUT, 3> _Patch
                    , uint _VtxIdx : SV_OutputControlPointID
                    , uint _PatchID : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT) 0.f;
    
    
    output.vPos = _Patch[_VtxIdx].vPos;
    output.vUV = _Patch[_VtxIdx].vUV;
    
    return output;
}


// =============
// Domain Shader
// =============
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBinormal : BINORMAL;
};

[domain("tri")]
DS_OUT DS_LandScape(const OutputPatch<HS_OUT, 3> _Patch
             , float3 _Weight : SV_DomainLocation
             , TessLevel _TessLevel)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    float3 vLocalPos = (float3) 0.f;
    float2 vUV = (float2) 0.f;
    
    float3 vTangent = float3(1.f, 0.f, 0.f);
    float3 vNormal = float3(0.f, 1.f, 0.f);
    float3 vBinormal = float3(0.f, 0.f, -1.f);
    
    for (int i = 0; i < 3; ++i)
    {
        vLocalPos += _Patch[i].vPos * _Weight[i];
        vUV += _Patch[i].vUV * _Weight[i];
        
        //vTangent += _Patch[i].vTangent * _Weight[i];
        //vNormal += _Patch[i].vNormal * _Weight[i];
        //vBinormal += _Patch[i].vBinormal * _Weight[i];
    }
    
    // 지형 전체 기준 UV 좌표
    float2 vFullUV = vUV / float2((float) FACE_X, (float) FACE_Z);
    
    // Height Map 적용
    if (HasHeightMap)
    {
        vLocalPos.y = HEIGHT_MAP.SampleLevel(g_sam_0, vFullUV, 0).x;
    
        // Normal Vector 재계산
        // 주변 정점의 좌표와 UV 좌표를 알아낸다.
        float fStep         = 1.f / _TessLevel.Inside;        
        float2 vUVStep      = fStep / float2(FACE_X, FACE_Z);
        
        float2 vLeftUV      = vFullUV + float2(-vUVStep.x, 0.f);
        float2 vRightUV     = vFullUV + float2(vUVStep.x, 0.f);
        float2 vUpUV        = vFullUV + float2(0.f, -vUVStep.y);
        float2 vDownUV      = vFullUV + float2(0.f, vUVStep.y);
        
        float3 vLeftPos     = float3(vLocalPos.x - fStep, HEIGHT_MAP.SampleLevel(g_sam_0, vLeftUV , 0).x, vLocalPos.z);
        float3 vRightPos    = float3(vLocalPos.x + fStep, HEIGHT_MAP.SampleLevel(g_sam_0, vRightUV, 0).x, vLocalPos.z);
        float3 vUpPos       = float3(vLocalPos.x        , HEIGHT_MAP.SampleLevel(g_sam_0, vUpUV   , 0).x, vLocalPos.z + fStep);
        float3 vDownPos     = float3(vLocalPos.x        , HEIGHT_MAP.SampleLevel(g_sam_0, vDownUV , 0).x, vLocalPos.z - fStep);
        
        vLeftPos    = mul(float4(vLeftPos  , 1.f), g_matWorld).xyz;
        vRightPos   = mul(float4(vRightPos , 1.f), g_matWorld).xyz;
        vUpPos      = mul(float4(vUpPos    , 1.f), g_matWorld).xyz;
        vDownPos    = mul(float4(vDownPos  , 1.f), g_matWorld).xyz;
        
        vTangent    = normalize(vRightPos - vLeftPos);
        vBinormal   = normalize(vDownPos - vUpPos);
        vNormal     = normalize(cross(vTangent, vBinormal));
        
        output.vViewTangent     = normalize(mul(float4(vTangent, 0.f), g_matWV)).xyz;
        output.vViewBinormal    = normalize(mul(float4(vBinormal, 0.f), g_matWV)).xyz;
        output.vViewNormal      = normalize(mul(float4(vNormal, 0.f), g_matWV)).xyz;
    }
    else
    {
        output.vViewTangent     = normalize(mul(float4(vTangent, 0.f), g_matWV)).xyz;
        output.vViewBinormal    = normalize(mul(float4(vBinormal, 0.f), g_matWV)).xyz;
        output.vViewNormal      = normalize(mul(float4(vNormal, 0.f), g_matWV)).xyz;
    }
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
        
    output.vViewPos = mul(float4(vLocalPos, 1.f), g_matWV).xyz;
    //output.vViewTangent = normalize(mul(float4(vTangent, 0.f), g_matWV)).xyz;
    //output.vViewBinormal = normalize(mul(float4(vBinormal, 0.f), g_matWV)).xyz;
    //output.vViewNormal = normalize(mul(float4(vNormal, 0.f), g_matWV)).xyz;
    
    return output;
}


struct PS_OUT
{
    float4 vColor       : SV_Target0;
    float4 vNormal      : SV_Target1;
    float4 vPosition    : SV_Target2;
    float4 vEmissive    : SV_Target3;
    float4 vData        : SV_Target4;
};


PS_OUT PS_LandScape(DS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    output.vColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    output.vNormal = float4(_in.vViewNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    
    return output;
}



#endif