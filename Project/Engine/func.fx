#ifndef _FUNC
#define _FUNC

#include "value.fx"


float3 CalLight2D(int lightIdx, float3 vWorldPos)
{
    // ==================
    // 광원 처리
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
        
        // 거리에 따른 기준치
        if(g_int_1)
        {
            // saturate : 음수는 0 , 1을 초과하면 1로 맞춰준다
            // 일정한 비율로 떨어지는 선형 방식
            fRatio = 1.f - saturate(fDist / info.Range);
        }
        else
        {
            // cos 그래프를 활용한 거리에 따른 기준치
            // 멀어질 수록 급격하게 Light Power 가 낮아짐
            fRatio = cos((fDist / info.Range) * (PI / 2.f));
        }
        
        if (fDist < info.Range)
        {
            vLightPower = info.Light.vDiffuse * fRatio;
        }
    }
    
    // Spot Light (과제 예정)
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
    // 파티클의 포지션을 Random Position 으로 지정
    // 추후 cos 그래프를 활용해서 랜덤한 UV 좌표값을 구할 예정
    float2 vUV = (float2) 0.f;
                
    // x 와 y 의 속도를 다르게 두어야한다.   x의 우측으로의 이동량
    vUV.x = _NormalizedThreadID.x + (Time * 0.1f); // 0 ~ 1 정규화
                
                
    // sin 그래프의 주파수 횟수를 늘린다. or sin 그래프의 진폭 범위를 줄인다.
    // 그리고 그래프를 계속 우하향으로 내린다.
    //                                  Frequency     진폭   y축으로의 좌표 이동량
    vUV.y = (sin((vUV.x - Time) * PI * 20.f) * 0.5f) + (Time * 0.2f);
                
                
    // Compute Shader 에서는 Sample Level 사용
    // Sampler Wrap Mode : 좌표가 0과 1을 초과하면 초과한 만큼 다시 좌표가 재지정된다.
    float3 vNoise = _Noise.SampleLevel(g_sam_0, vUV, 0).xyz;
    
    return vNoise;
}


#endif