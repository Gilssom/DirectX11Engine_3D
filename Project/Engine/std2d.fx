// �ߺ� ��� ���� ���� ���� = #progma once
#ifndef _STD2D
#define _STD2D

#include "value.fx"
#include "func.fx"


StructuredBuffer<float4> g_TestBuffer : register(t20);

// ���� ������ ���� ��ȭ�� �ʿ� ����.
// ��ü�� ��ǥ�� �������� �ȴ�.

// ������ ��� ������ �ʿ� ����.
// ���� ���� ������ �ʿ��� �����鸸 �������� �ȴ�. = Input Layout

struct VS_IN // �Է� Ÿ��
{
    float3 vPos : POSITION;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};


struct VS_OUT // ��ȯ Ÿ��
{
    float4 vPosition : SV_Position;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
    
    float3 vWorldPos : POSITION;
};


VS_OUT VS_Std2D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    // ����� ���Ҷ� 3���� ��ǥ�� 4�������� Ȯ�� (������ǥ)
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    // 1. ���� ��ġ�� ��ü�� ��ġ ��Ų��.
    float4 vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    
    // 2. ī�޶� ��ġ�� �������� View Space ��ǥ ���ġ
    //   ��� ��ü�� ī�޶� ������ �Ǵ� ��ǥ��� �� �Ѿ�´�.
    //float4 vViewPos = mul(vWorldPos, g_matView);
    
    // 3. ���� ��ǥ�� �����ش�. ( ���� ��ǥ X )
    //float4 vProjPos = mul(vViewPos, g_matProj);
    
    // 4. Rasterizer �ܰ迡�� ����� 4��° w ���� �˾Ƽ� ������ ���� ��ǥ�� ����� ���ش�.
    // ���� ���� ��ǥ�� ������ ������ؼ� �Ѱ��ְ� �� �ʿ䰡 ����.
    // ������ 1 �� ���� ������ ���� �״���� ���¸� �׸��� ���ؼ�.
    // ���� ����� ������ ��� ��ü�� ������ �ִ� ���´� ������ �� ���ٰ� �Ǵ�.
    // �׷��� ���ο��� w ���� �����ִ� �ý����� ������.
    output.vWorldPos = vWorldPos;
    //output.vPosition = vProjPos;
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_Std2D(VS_OUT _in) : SV_Target // ��ȯ Ÿ��
{
    float4 vColor = (float4) 0.f;
    
    if (UseAnim2D)
    {
        float2 vBackGroundLeftTop = vLeftTop - ((vBackGround - vSliceSize) * 0.5f);
        float2 vUV = (vBackGroundLeftTop + _in.vUV * vBackGround) - vOffset;
        
        if (vUV.x < vLeftTop.x || vUV.x > vSliceSize.x + vLeftTop.x
            || vUV.y < vLeftTop.y || vUV.y > vSliceSize.y + vLeftTop.y)
        {
            // �ش� ������ ����
            discard;
        }
        else
        {
            vColor = g_Atlas.Sample(g_sam_0, vUV);
        }
    }
    else
    {
        // ���ø� ( �� �ȼ������� ������ �����;��� )
        if (g_int_1)
            vColor = g_tex_1.Sample(g_sam_0, _in.vUV);
        else if (g_btex_0)
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        else
            vColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    // Global Data �� ����ִ� ������ ���� �����ͼ� �ݺ��� ������
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
        
        // ���� �׷��� Ȱ�� ( Dissolve Effect ȿ�� ���� ���� )
        vColor.r += pow(fIntence * 10.f, 5) / pow(10, 5);
        vColor.g += pow(fIntence * 10.f, 5) / pow(10, 5) * 0.5f;
        vColor.b += pow(fIntence * 10.f, 5) / pow(10, 5) * 0.5f;
        
        if (1.f <= fIntence)
        {
            discard;
        }
    }
    
    // ���� ������ �� Color
    // �� ������ Color ���� ��� �ֱ� ������
    // ����ġ ���� ������ �� �ȼ� ������ ���� ���� �����ش�. ( Rasterize Stage )
    
    //if (vColor.r == 0.f && vColor.g == 0.f && vColor.b == 0.f)
    //    vColor.a = 0.f;
    
    if(vColor.a == 0.f)
        discard;
    
    if(g_int_0)
        vColor.r = 0.8f; 
    
    return vColor;
}


float4 PS_Std2D_AB(VS_OUT _in) : SV_Target // ��ȯ Ÿ��
{
    float4 vColor = (float4) 0.f;
    
    if (UseAnim2D)
    {
        float2 vBackGroundLeftTop = vLeftTop - ((vBackGround - vSliceSize) * 0.5f);
        float2 vUV = (vBackGroundLeftTop + _in.vUV * vBackGround) - vOffset;
        
        if(vUV.x < vLeftTop.x || vUV.x > vSliceSize.x + vLeftTop.x
            || vUV.y < vLeftTop.y || vUV.y > vSliceSize.y + vLeftTop.y)
        {
            // �ش� ������ ����
            discard;
        }
        else
        {
            vColor = g_Atlas.Sample(g_sam_0, vUV);
        }
    }
    else
    {
        // ���ø� ( �� �ȼ������� ������ �����;��� )
        
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
    
    // Global Data �� ����ִ� ������ ���� �����ͼ� �ݺ��� ������
    float3 vLightPower = 0.f;
    for (int i = 0; i < Light2DCount; i++)
    {
        vLightPower += CalLight2D(i, _in.vWorldPos);
    }
    
    vColor.rgb *= vLightPower;
    
    if (g_int_0)
        vColor.r = 0.8f;
    
    // ���� ������ �� Color
    // �� ������ Color ���� ��� �ֱ� ������
    // ����ġ ���� ������ �� �ȼ� ������ ���� ���� �����ش�. ( Rasterize Stage )
    return vColor;
}

#endif