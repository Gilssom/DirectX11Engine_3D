#ifndef _POSTPROCESS
#define _POSTPROCESS

#include "value.fx"

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

// mesh : RectMesh
VS_OUT VS_PostProcess(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_PostProcess(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0)
    {
        // ��ü ������ ������ �ش�.
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        vOutColor = (vOutColor.r + vOutColor.g + vOutColor.b) / 3.f; // ��ü ���� ���
    }
    
    return vOutColor;
}

float4 PS_Bloom(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(0.f, 0.f, 0.f, 1.f);

    if (g_btex_0) // Copy Render Target Texture
    {
        float4 renderColor = g_tex_0.Sample(g_sam_0, _in.vUV);

        if (g_btex_1) // Bright Pass Target Texture�� �ִ� ���
        {
            float4 brightColor = g_tex_1.Sample(g_sam_0, _in.vUV);

            // Bloom Threshold ����
            float luminance = dot(brightColor.rgb, float3(0.299, 0.587, 0.114));
            if (luminance < g_float_3)
            {
                brightColor = float4(0, 0, 0, 0);
            }

            // Bloom Power ����
            brightColor *= g_float_0;

            // Bloom ȿ���� ���� Gaussian Blur ����
            float2 texelSize = g_float_1 / vResolution; // Blur Radius�� �̿��� �ݰ� ũ�� ����

            float weight[5] = { 0.227027027f, 0.194594595f, 0.121621622f, 0.054054054f, 0.016216216f };

            // �߽ɰ� ����
            vOutColor = brightColor * weight[0];

            // �翷�� �ȼ��� ����ġ ����
            for (int i = 1; i < 5; ++i)
            {
                vOutColor += g_tex_1.Sample(g_sam_0, _in.vUV + texelSize * float2(i, 0)) * weight[i];
                vOutColor += g_tex_1.Sample(g_sam_0, _in.vUV - texelSize * float2(i, 0)) * weight[i];
                vOutColor += g_tex_1.Sample(g_sam_0, _in.vUV + texelSize * float2(0, i)) * weight[i];
                vOutColor += g_tex_1.Sample(g_sam_0, _in.vUV - texelSize * float2(0, i)) * weight[i];
            }

            vOutColor += renderColor; // ���� ������ ����� �ջ��Ͽ� Bloom ȿ�� ����
        }
        else // Bright Pass�� ���� ���
        {
            vOutColor = renderColor; // ���� ���� �״�� ��ȯ
        }

        // Brightness Scale ����
        vOutColor.rgb *= g_float_2;
    }

    return vOutColor;
}


// Vignette Pixel Shader
float4 PS_Vignette(VS_OUT _in) : SV_Target
{
    float4 vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);

    // Vignette parameters
    float2 center = float2(0.5, 0.5);
    float radius = g_float_0;
    float softness = g_float_1;

    // Calculate distance from the center of the screen
    float2 position = _in.vUV - center;
    float distance = length(position);

    // Apply vignette effect based on distance from the center
    float vignette = smoothstep(radius, radius - softness, distance);

    // Blend original color with the vignette effect
    vOutColor.rgb *= vignette;

    return vOutColor;
}


// mesh : RectMesh
VS_OUT VS_Distortion(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Distortion(VS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    
    if (g_btex_0)
    {
        // �ش� �ȼ��� �ȼ� ��ǥ /  ȭ�� ��ü �ػ� ����
        float2 vScreenUV = _in.vPosition.xy / vResolution;
        
        float2 vOffsetUV = float2(0.f, 0.f);
        if (g_btex_1)
        {
            _in.vUV.x += Time * 0.1f;
            
            vOffsetUV = g_tex_1.Sample(g_sam_0, _in.vUV);
            vOffsetUV -= 0.5f;
            vOffsetUV *= 0.02f;
        }
        
        vOutColor = g_tex_0.Sample(g_sam_0, vScreenUV + vOffsetUV);
    }
    
    //vOutColor = float4(1.f, 0.f, 0.f, 1.f);
    
    return vOutColor;
}



#endif