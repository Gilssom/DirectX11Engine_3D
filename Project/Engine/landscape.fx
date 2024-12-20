#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "value.fx"
#include "func.fx"

// =============================
// LandScpae Shader
// MRT      : Deferred
// DOMAIN   : Deferred
// Mesh     : 자체 Mesh 
// Parameter
#define FACE_X              g_int_0
#define FACE_Z              g_int_1
#define MODE                g_int_2
#define TEXTURE_ARRSIZE     g_int_3

#define Tess_Level      g_float_0

#define MIN_LEVEL       g_vec4_0.x
#define MAX_LEVEL       g_vec4_0.y
#define MIN_RANGE       g_vec4_0.z
#define MAX_RANGE       g_vec4_0.w

#define CAM_POS         g_vec4_1.xyz

#define HEIGHT_MAP      g_tex_0
#define HasHeightMap    g_btex_0

#define HasColorTex     g_btexarr_0
#define HasNormalTex    g_btexarr_1

#define COLOR_TEX       g_texarr_0
#define NORMAL_TEX      g_texarr_1
#define GRASS_TEX       g_tex_2

#define IsShowBrush     g_btex_1
#define BRUSH_TEX       g_tex_1
#define BrushScale      g_vec2_0
#define BrushPos        g_vec2_1

StructuredBuffer<tWeight>       WEIGHT_MAP : register(t20);
StructuredBuffer<tGrassWeight>  GRASS_MAP : register(t21);
#define WEIGHT_RESOLUTION   g_vec2_2
#define GRASS_RESOLUTION    g_vec2_3
// =============================

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float3 vWorldPos : POSITION1;
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
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld).xyz;
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
    
    
    // Edge 0 :: 위, 아래
    Level.arrEdge[0] = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                    , MIN_RANGE, MAX_RANGE, CAM_POS
                                    , (_Patch[1].vWorldPos + _Patch[2].vWorldPos) * 0.5f);
    
    // Edge 1 :: 좌, 우
    Level.arrEdge[1] = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                    , MIN_RANGE, MAX_RANGE, CAM_POS
                                    , (_Patch[0].vWorldPos + _Patch[2].vWorldPos) * 0.5f);
    
    // Edge 2 :: 빗변 (서로 마주보는 변)
    Level.arrEdge[2] = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                    , MIN_RANGE, MAX_RANGE, CAM_POS
                                    , (_Patch[0].vWorldPos + _Patch[1].vWorldPos) * 0.5f);
    
    // Inside :: 각 삼각형의 중심 (가중치의 중심)
    Level.Inside     = GetTessFactor(MIN_LEVEL, MAX_LEVEL
                                    , MIN_RANGE, MAX_RANGE, CAM_POS
                                    , (_Patch[0].vWorldPos + _Patch[1].vWorldPos + _Patch[2].vWorldPos) / 3.f);
    
    return Level;
}

struct HS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

// 정점 당 호출되는 함수
[domain("tri")]
[partitioning("fractional_odd")]
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
    float2 vFullUV : TEXCOORD1;
    
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
    output.vFullUV = vUV / float2((float) FACE_X, (float) FACE_Z);
    
    // Height Map 적용
    if (HasHeightMap)
    {
        vLocalPos.y = HEIGHT_MAP.SampleLevel(g_sam_2, output.vFullUV, 0).x;
    
        // Normal Vector 재계산
        // 주변 정점의 좌표와 UV 좌표를 알아낸다.
        float fStep         = 1.f / _TessLevel.Inside;        
        float2 vUVStep      = fStep / float2(FACE_X, FACE_Z);
        
        // 주변 정점의 UV 좌표
        float2 vLeftUV      = output.vFullUV + float2(-vUVStep.x, 0.f);
        float2 vRightUV     = output.vFullUV + float2(vUVStep.x, 0.f);
        float2 vUpUV        = output.vFullUV + float2(0.f, -vUVStep.y);
        float2 vDownUV      = output.vFullUV + float2(0.f, vUVStep.y);
        
        // 주변 정점의 Local 좌표
        float3 vLeftPos     = float3(vLocalPos.x - fStep, HEIGHT_MAP.SampleLevel(g_sam_2, vLeftUV , 0).x, vLocalPos.z);
        float3 vRightPos    = float3(vLocalPos.x + fStep, HEIGHT_MAP.SampleLevel(g_sam_2, vRightUV, 0).x, vLocalPos.z);
        float3 vUpPos       = float3(vLocalPos.x        , HEIGHT_MAP.SampleLevel(g_sam_2, vUpUV   , 0).x, vLocalPos.z + fStep);
        float3 vDownPos     = float3(vLocalPos.x        , HEIGHT_MAP.SampleLevel(g_sam_2, vDownUV , 0).x, vLocalPos.z - fStep);
        
        // 주변 정점의 Local -> World 좌표 변환
        vLeftPos    = mul(float4(vLeftPos  , 1.f), g_matWorld).xyz;
        vRightPos   = mul(float4(vRightPos , 1.f), g_matWorld).xyz;
        vUpPos      = mul(float4(vUpPos    , 1.f), g_matWorld).xyz;
        vDownPos    = mul(float4(vDownPos  , 1.f), g_matWorld).xyz;
        
        // 현재 정점의 T, B, N Vector 방향
        vTangent    = normalize(vRightPos - vLeftPos);
        vBinormal   = normalize(vDownPos - vUpPos);
        vNormal     = normalize(cross(vTangent, vBinormal));
        
        // 최종 T, B, N Vector World View 좌표
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
    
    return output;
}


// =================
//  Geometry Shader
// =================
struct GS_OUT
{
    float4 vPosition    : SV_Position;
    float2 vUV          : TEXCOORD;
    float3 vWorldPos    : POSITION1;
};

[maxvertexcount(6)]
void GS_LandScape(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    int2 vColRow = _in[0].vUV * GRASS_RESOLUTION;
    int GrassMapIdx = GRASS_RESOLUTION.x * vColRow.y + vColRow.x;
    float GrassWeight = GRASS_MAP[GrassMapIdx].GrassWeight;

    // GrassWeight 조건 확인
    if (GrassWeight >= 0.95f)
    {
        GS_OUT output[4];

        // 풀 배치할 위치를 중심으로 사각형 생성
        float4 vViewPos = mul(float4(_in[0].vWorldPos, 1.f), g_matView);

        output[0].vPosition = float4(-0.5f, 0.5f, 0.f, 1.f); // 좌상단
        output[1].vPosition = float4(0.5f, 0.5f, 0.f, 1.f); // 우상단
        output[2].vPosition = float4(0.5f, -0.5f, 0.f, 1.f); // 우하단
        output[3].vPosition = float4(-0.5f, -0.5f, 0.f, 1.f); // 좌하단

        // 카메라를 향하도록 Billboarding 처리
        for (int i = 0; i < 4; ++i)
        {
            output[i].vPosition.xyz += vViewPos.xyz;
            output[i].vPosition = mul(output[i].vPosition, g_matProj);
            output[i].vWorldPos = _in[0].vWorldPos; // 세계 좌표를 Pixel Shader로 전달
            output[i].vUV = (i == 0 || i == 3) ? float2(0.f, (i == 0) ? 0.f : 1.f) : float2(1.f, (i == 1) ? 0.f : 1.f);
        }

        // 사각형 출력 (삼각형 두 개로 구성)
        _OutStream.Append(output[0]);
        _OutStream.Append(output[1]);
        _OutStream.Append(output[2]);
        _OutStream.RestartStrip();

        _OutStream.Append(output[0]);
        _OutStream.Append(output[2]);
        _OutStream.Append(output[3]);
        _OutStream.RestartStrip();
    }
}


// =============
//  Pixel Shader
// =============
struct PS_OUT
{
    float4 vColor       : SV_Target0;
    float4 vNormal      : SV_Target1;
    float4 vPosition    : SV_Target2;
    float4 vEmissive    : SV_Target3;
    float4 vData        : SV_Target4;
};

//PS_OUT PS_LandScape(GS_OUT _in) // Geometry Shader에서 데이터를 받음
//{
//    PS_OUT output = (PS_OUT) 0.f;
//
//    // 풀 텍스처 샘플링하여 사각형에 적용
//    output.vColor = GRASS_TEX.Sample(g_sam_0, _in.vUV);
//    output.vPosition = float4(_in.vWorldPos, 1.f); // 월드 좌표에 해당하는 포지션 적용
//
//    return output;
//}

PS_OUT PS_LandScape(DS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    float4 vBrush = (float4) 0.f;
    
    if (IsShowBrush && MODE)
    {
        // Brush Left Top 좌표
        float2 BrushLT = BrushPos - (BrushScale * 0.5f);
        
        // 지형 기준, Pixel 위치 구하기
        float2 vBrushUV = _in.vUV / float2(FACE_X, FACE_Z);
        vBrushUV = (vBrushUV - BrushLT) / BrushScale;
        
        if (0.f <= vBrushUV.x && vBrushUV.x <= 1.f && 0.f <= vBrushUV.y && vBrushUV.y <= 1.f)
        {
            float BrushAlpha = BRUSH_TEX.Sample(g_sam_2, vBrushUV).a;
            float3 BrushColor = float3(0.8f, 0.8f, 0.f);
            
            vBrush.rgb = (vBrush.rgb * (1 - BrushAlpha)) + (BrushColor * BrushAlpha);
        }   
    }
    
    float4 vColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    float3 vViewNormal = _in.vViewNormal;
    
    if(HasColorTex)
    {
        float2 derivX = ddx(_in.vUV);
        float2 derivY = ddy(_in.vUV);
        
        //vColor = COLOR_TEX.Sample(g_sam_0, float3(_in.vUV, 1.f));
        int2 vColRow = _in.vFullUV * WEIGHT_RESOLUTION;
        int WeightMapIdx = WEIGHT_RESOLUTION.x * vColRow.y + vColRow.x;
        int GrassMapIdx = GRASS_RESOLUTION.x * vColRow.y + vColRow.x;

        vColor = (float4) 0.f;
        
        int WeightMaxIdx = -1;
        float WeightMax = 0.f;
        
        // Weight Map Check
        for (int i = 0; i < TEXTURE_ARRSIZE; ++i)
        {
            float Weight = WEIGHT_MAP[WeightMapIdx].Weight[i];
            
            if(Weight != 0.f)
            {
                //vColor += COLOR_TEX.SampleLevel(g_sam_0, float3(_in.vUV, i), 3) * Weight;
                vColor += COLOR_TEX.SampleGrad(g_sam_0, float3(_in.vUV, i), derivX * 0.25f, derivY * 0.25f) * Weight;
            }
            
            // 제일 높았던 가중치를 기록
            if (WeightMax < Weight)
            {
                WeightMax = Weight;
                WeightMaxIdx = i;
            }
        }
        
        if (WeightMaxIdx != -1)
        {
            float3 vNormal = NORMAL_TEX.Sample(g_sam_0, float3(_in.vUV, WeightMaxIdx));
            vNormal = vNormal * 2.f - 1.f;
        
            float3x3 Rot =
            {
                _in.vViewTangent,
                _in.vViewBinormal,
                _in.vViewNormal
            };
        
            vViewNormal = normalize(mul(vNormal, Rot));
        }
        
        // Grass Map Check
        int GrassMaxIdx = -1;
        float GrassMax = 0.f;
        
        float GrassWeight = GRASS_MAP[GrassMapIdx].GrassWeight;
        
        if (GrassWeight >= 0.95f)
        {
            //vColor += COLOR_TEX.SampleLevel(g_sam_0, float3(_in.vUV, i), 3) * Weight;
            //vColor += GRASS_TEX.SampleGrad(g_sam_0, float3(_in.vUV, i), derivX * 0.25f, derivY * 0.25f) * GrassWeight;
            //vColor = GRASS_TEX.Sample(g_sam_0, _in.vUV);
            //vColor = float4(0.f, 0.f, 0.f, 1.f);
            vColor = GRASS_TEX.Sample(g_sam_0, _in.vUV);
        }
    }
    
    output.vColor = vColor;
    output.vEmissive = float4(vBrush.rgb, 1.f);
    output.vNormal = float4(vViewNormal, 1.f);
    output.vPosition = float4(_in.vViewPos, 1.f);
    
    return output;
}



#endif