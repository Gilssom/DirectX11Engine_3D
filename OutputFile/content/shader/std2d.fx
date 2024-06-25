// 중복 헤더 파일 참조 방지 = #progma once
#ifndef _STD2D
#define _STD2D

#include "value.fx"
#include "func.fx"


StructuredBuffer<float4> g_TestBuffer : register(t20);

// 정점 버퍼의 정보 변화는 필요 없다.
// 물체의 좌표만 가져오면 된다.

// 정점의 모든 정보가 필요 없다.
// 전달 받은 정점의 필요한 정보들만 가져오면 된다. = Input Layout

struct VS_IN // 입력 타입
{
    float3 vPos : POSITION;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};


struct VS_OUT // 반환 타입
{
    float4 vPosition : SV_Position;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
    
    float3 vWorldPos : POSITION;
};


VS_OUT VS_Std2D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // 행렬을 곱할때 3차원 좌표를 4차원으로 확장 (동차좌표)
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    // 1. 월드 위치로 물체를 배치 시킨다.
    float4 vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    
    // 2. 카메라 위치를 기준으로 View Space 좌표 재배치
    //   모든 물체가 카메라가 기준이 되는 좌표계로 다 넘어온다.
    //float4 vViewPos = mul(vWorldPos, g_matView);
    
    // 3. 투영 좌표도 곱해준다. ( 최종 좌표 X )
    //float4 vProjPos = mul(vViewPos, g_matProj);
    
    // 4. Rasterizer 단계에서 행렬의 4번째 w 값을 알아서 나눠서 최종 좌표를 계산을 해준다.
    // 직접 투영 좌표를 나눠서 계산을해서 넘겨주고 할 필요가 없다.
    // 이전에 1 을 넣은 이유는 기존 그대로의 형태를 그리기 위해서.
    // 투영 행렬을 각각의 모든 물체가 가지고 있는 형태는 존재할 수 없다고 판단.
    // 그래서 내부에서 w 값을 나눠주는 시스템이 존재함.
    output.vWorldPos = vWorldPos;
    //output.vPosition = vProjPos;
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_Std2D(VS_OUT _in) : SV_Target // 반환 타입
{
    float4 vColor = (float4) 0.f;
    
    if (UseAnim2D)
    {
        float2 vBackGroundLeftTop = vLeftTop - ((vBackGround - vSliceSize) * 0.5f);
        float2 vUV = (vBackGroundLeftTop + _in.vUV * vBackGround) - vOffset;
        
        if (vUV.x < vLeftTop.x || vUV.x > vSliceSize.x + vLeftTop.x
            || vUV.y < vLeftTop.y || vUV.y > vSliceSize.y + vLeftTop.y)
        {
            // 해당 영역은 무시
            discard;
        }
        else
        {
            vColor = g_Atlas.Sample(g_sam_0, vUV);
        }
    }
    else
    {
        // 샘플링 ( 각 픽셀마다의 색상을 가져와야함 )
        if (g_int_1)
            vColor = g_tex_1.Sample(g_sam_0, _in.vUV);
        else if (g_btex_0)
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        else
            vColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    // Global Data 에 들어있는 광원의 개수 가져와서 반복문 돌리기
    float3 vLightPower = 0.f;
    for (int i = 0; i < Light2DCount; i++)
    {
        vLightPower += CalLight2D(i, _in.vWorldPos);
    }
    
    vColor.rgb *= vLightPower;
    
    // Noise Test
    if(g_btex_2)
    {
        float4 vNoise = g_tex_2.Sample(g_sam_0, _in.vUV);
        
        float fIntence = g_float_0 + vNoise.r;
        
        // 선형 그래프 활용 ( Dissolve Effect 효과 같은 느낌 )
        vColor.r += pow(fIntence * 10.f, 5) / pow(10, 5);
        vColor.g += pow(fIntence * 10.f, 5) / pow(10, 5) * 0.5f;
        vColor.b += pow(fIntence * 10.f, 5) / pow(10, 5) * 0.5f;
        
        if (1.f <= fIntence)
        {
            discard;
        }
    }
    
    // 보간 개념이 들어간 Color
    // 각 정점이 Color 값을 들고 있기 때문에
    // 가중치 보간 개념이 들어가 픽셀 마다의 색상 값을 정해준다. ( Rasterize Stage )
    
    //if (vColor.r == 0.f && vColor.g == 0.f && vColor.b == 0.f)
    //    vColor.a = 0.f;
    
    if(vColor.a == 0.f)
        discard;
    
    if(g_int_0)
        vColor.r = 0.8f; 
    
    return vColor;
}


float4 PS_Std2D_AB(VS_OUT _in) : SV_Target // 반환 타입
{
    float4 vColor = (float4) 0.f;
    
    if (UseAnim2D)
    {
        float2 vBackGroundLeftTop = vLeftTop - ((vBackGround - vSliceSize) * 0.5f);
        float2 vUV = (vBackGroundLeftTop + _in.vUV * vBackGround) - vOffset;
        
        if(vUV.x < vLeftTop.x || vUV.x > vSliceSize.x + vLeftTop.x
            || vUV.y < vLeftTop.y || vUV.y > vSliceSize.y + vLeftTop.y)
        {
            // 해당 영역은 무시
            discard;
        }
        else
        {
            vColor = g_Atlas.Sample(g_sam_0, vUV);
        }
    }
    else
    {
        // 샘플링 ( 각 픽셀마다의 색상을 가져와야함 )
        
        if(g_int_1)
            vColor = g_tex_1.Sample(g_sam_0, _in.vUV);
        else      
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    if (g_float_1)
    {
        if (vColor.r <= 0.01f && vColor.g <= 0.01f && vColor.b <= 0.01f)
            vColor.a = 0.f;
        else
            vColor.a = g_float_1;
    }
    
    // Global Data 에 들어있는 광원의 개수 가져와서 반복문 돌리기
    float3 vLightPower = 0.f;
    for (int i = 0; i < Light2DCount; i++)
    {
        vLightPower += CalLight2D(i, _in.vWorldPos);
    }
    
    vColor.rgb *= vLightPower;
    
    if (g_int_0)
        vColor.r = 0.8f;
    
    // 보간 개념이 들어간 Color
    // 각 정점이 Color 값을 들고 있기 때문에
    // 가중치 보간 개념이 들어가 픽셀 마다의 색상 값을 정해준다. ( Rasterize Stage )
    return vColor;
}

#endif