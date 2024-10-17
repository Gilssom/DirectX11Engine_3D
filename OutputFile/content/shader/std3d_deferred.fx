#ifndef _STD3D_DEFERRED
#define _STD3D_DEFERRED

#include "value.fx"
#include "func.fx"

// ============================
// Std 3D Deferred Shader
// MRT      : Deferred
// g_tex_0  : Color Texture
// g_tex_1  : Normal Texture
// g_tex_2  : Specualr Texture
// g_tex_3  : Emissive Texture
// g_tex_4  : Bright Texture
// ============================

struct VS_IN
{
    float3 vPos         : POSITION;
    float2 vUV          : TEXCOORD;
    
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    
    float4 vWeights     : BLENDWEIGHT;
    float4 vIndices     : BLENDINDICES;
};

struct VS_OUT
{
    float4 vPosition    : SV_Position;
    float2 vUV          : TEXCOORD;
    
    float3 vViewPos     : POSITION;
    float3 vViewNormal  : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal: BINORMAL;
};

VS_OUT VS_Std3D_Deferred(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    if (g_iAnim)
    {
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal
              , _in.vWeights, _in.vIndices, 0);
    }
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV);
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV));
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV));
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV));
    
    return output;
}

struct VS_IN_Inst
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float4 vWeights : BLENDWEIGHT;
    float4 vIndices : BLENDINDICES;
    
    // Per Instance Data
    row_major matrix matWorld   : WORLD;
    row_major matrix matWV      : WV;
    row_major matrix matWVP     : WVP;
    uint iRowIndex              : ROWINDEX; // 자신의 애니메이션 최종 행렬 데이터가 몇번째 행에 있는지
};

VS_OUT VS_Std3D_Deferred_Inst(VS_IN_Inst _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    if(g_iAnim)
    {
        // 마지막에 자신의 행렬 위치 전달
        Skinning(_in.vPos, _in.vTangent, _in.vBinormal, _in.vNormal
              , _in.vWeights, _in.vIndices, _in.iRowIndex);
    }
    
    // 그 후 입력으로 들어온 Instance Data 에서 받은 정보를 기반으로 행렬 계산
    output.vPosition = mul(float4(_in.vPos, 1.f), _in.matWVP);
    output.vUV = _in.vUV;
	
    output.vViewPos = mul(float4(_in.vPos, 1.f), _in.matWV);
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), _in.matWV));
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), _in.matWV));
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), _in.matWV));
    
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

PS_OUT PS_Std3D_Deferred(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float4 vObjectColor = float4(1.f, 0.f, 1.f, 1.f);
    
    // ======= Color Texture =======
    if (g_btex_0)
    {
        vObjectColor = g_tex_0.Sample(g_sam_0, _in.vUV) * MtrlData.vDiff;
    }

     // ======= Hit 이벤트 처리 =======
    if (g_int_0)
    {
        // g_int_0 값이 1이면 빨간색으로 변경
        vObjectColor = float4(0.7f, 0.f, 0.f, 1.f); // 빨간색 (Hit 상태)
    }
    
    // ======= Normal Texture =======    
    float3 vViewNormal = _in.vViewNormal;
    if (g_btex_1)
    {
        // Sample 로 얻은 값의 범위는 0 ~ 1, 실제 기록된 값의 의미는 -1 ~ 1
        float3 vNormal = g_tex_1.Sample(g_sam_0, _in.vUV);
        
        // 범위 보정
        vNormal = vNormal * 2.f - 1.f;
        
        float3x3 Rot =
        {
            _in.vViewTangent,
            _in.vViewBinormal,
            _in.vViewNormal
        };

        vViewNormal = normalize(mul(vNormal, Rot));
    }
    
    // ======= Emissive 처리 =======
    float4 vEmissive = (float4) 0.f;
    if (g_int_1)
    {
        // g_int_1 값이 1이면 발광 효과 추가
        vEmissive = float4(1.f, 1.f, 0.5f, 1.f) * g_float_0; // 노란색 발광 효과
    }
    
    // ======= Emissive Texture =======    
    // float4 vEmissive = (float4) 0.f;
    if(g_btex_3)
    {
        vEmissive += g_tex_3.Sample(g_sam_0, _in.vUV) * MtrlData.vEmv;
    }
    
    // ======== Data Texture ======
    float4 vBrightColor = (float4) 0.f;
    if (g_int_1)
    {
        vBrightColor = vEmissive;

        // 밝은 부분만 통과시키기 위한 임계값
        float brightnessThreshold = 0.7f;
        float luminance = dot(vBrightColor.rgb, float3(0.299, 0.587, 0.114));

        if (luminance < brightnessThreshold)
        {
            vBrightColor = (float4) 0.f; // 밝기 임계값을 만족하지 않으면 원래 색상 유지
        }
    }
    
    output.vColor       = float4(vObjectColor);
    output.vNormal      = float4(vViewNormal, 1.f);
    output.vPosition    = float4(_in.vViewPos, 1.f);
    output.vEmissive    = vEmissive;
    output.vData        = vBrightColor;
    
    return output;
}


#endif