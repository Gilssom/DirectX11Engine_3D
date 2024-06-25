#ifndef _FUNC
#define _FUNC

#include "value.fx"


float3 CalLight2D(int lightIdx, float3 vWorldPos)
{
    // ==================
    // ���� ó��
    // ==================
    
    tLightInfo info = g_Light2D[lightIdx];
   
    float3 vLightPower = (float3) 0.f;
    
    // Directional Light
    if (info.LightType == 0)
    {
        vLightPower = info.Light.vDiffuse.rgb + info.Light.vAmbient.rgb;
    }
    
    // Point Light
    else if (info.LightType == 1)
    {
        // Pixel World Space
        float fDist = distance(info.WorldPos.xy, vWorldPos.xy);
        
        float fRatio = 0.f;
        
        // �Ÿ��� ���� ����ġ
        if(g_int_1)
        {
            // saturate : ������ 0 , 1�� �ʰ��ϸ� 1�� �����ش�
            // ������ ������ �������� ���� ���
            fRatio = 1.f - saturate(fDist / info.Range);
        }
        else
        {
            // cos �׷����� Ȱ���� �Ÿ��� ���� ����ġ
            // �־��� ���� �ް��ϰ� Light Power �� ������
            fRatio = cos((fDist / info.Range) * (PI / 2.f));
        }
        
        if (fDist < info.Range)
        {
            vLightPower = info.Light.vDiffuse * fRatio;
        }
    }
    
    // Spot Light (���� ����)
    else
    {
        // Pixel World Space
        float fDist = distance(info.WorldPos.xy, vWorldPos.xy);
        
        float fRatio = 1.f;
        
        if (fDist < info.Range)
        {
            vLightPower = (info.Light.vDiffuse.rgb + info.Light.vAmbient.rgb) * fRatio;
        }
    }
    
    return vLightPower;
}


int IsBinding(in Texture2D _tex)
{
    uint width = 0;
    uint height = 0;
    g_tex_0.GetDimensions(width, height);
    
    if(width && height)
        return 1;
    else
        return 0;
}


float3 GetRandom(in Texture2D _Noise, float _NormalizedThreadID)
{
    // ��ƼŬ�� �������� Random Position ���� ����
    // ���� cos �׷����� Ȱ���ؼ� ������ UV ��ǥ���� ���� ����
    float2 vUV = (float2) 0.f;
                
    // x �� y �� �ӵ��� �ٸ��� �ξ���Ѵ�.   x�� ���������� �̵���
    vUV.x = _NormalizedThreadID.x + (Time * 0.1f); // 0 ~ 1 ����ȭ
                
                
    // sin �׷����� ���ļ� Ƚ���� �ø���. or sin �׷����� ���� ������ ���δ�.
    // �׸��� �׷����� ��� ���������� ������.
    //                                  Frequency     ����   y�������� ��ǥ �̵���
    vUV.y = (sin((vUV.x - Time) * PI * 20.f) * 0.5f) + (Time * 0.2f);
                
                
    // Compute Shader ������ Sample Level ���
    // Sampler Wrap Mode : ��ǥ�� 0�� 1�� �ʰ��ϸ� �ʰ��� ��ŭ �ٽ� ��ǥ�� �������ȴ�.
    float3 vNoise = _Noise.SampleLevel(g_sam_0, vUV, 0).xyz;
    
    return vNoise;
}


#endif