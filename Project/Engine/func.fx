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
            vLightPower = info.Light.vDiffuse.xyz * fRatio;
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

void CalLight3D(int _LightIdx, float3 _ViewPos, float3 _ViewNormal, inout tLight _Light)
{
    // 광원 정보
    tLightInfo LightInfo = g_Light3D[_LightIdx];
    
    // 빛의 세기
    float LightPower = (float) 0.f;
    
    // 빛의 방향
    float3 vLight = (float3) 0.f;
    
    // 빛을 적용받을 픽셀(물체) 에서의 법선 벡터 (노말 벡터)
    _ViewNormal = normalize(_ViewNormal);
    
    // 거리에 따른 빛의 세기 (Directional 은 무관)
    float DistancePower = 1.f;
    
    
    // Directional Light
    if(LightInfo.LightType == 0)
    {
        // View Space 에서의 광원 방향
        vLight = normalize(mul(float4(LightInfo.WorldDir, 0.f), g_matView).xyz);
        
        // 램버트 코사인 법칙으로 구한 빛의 세기    
        LightPower = saturate(dot(_ViewNormal, -vLight));
    }
    
    // Point Light
    else if (LightInfo.LightType == 1)
    {
        // 광원의 위치
        float3 vLightPos = mul(float4(LightInfo.WorldPos.xyz, 1.f), g_matView).xyz;
        
        // View Space 에서의 물체를 향한 광원의 방향
        vLight = normalize(_ViewPos - vLightPos);
        
        // 램버트 코사인 법칙으로 구한 빛의 세기   
        LightPower = saturate(dot(_ViewNormal, -vLight));
        
        // 광원과 물체 사이의 거리
        float Distance = distance(_ViewPos, vLightPos);
        
        // 거리에 따른 빛의 세기 감소율
        //DistancePower = saturate(1.f - Distance / LightInfo.Range);                        (일정한 간격)
        DistancePower = saturate(cos(saturate(Distance / LightInfo.Range) * (PI / 2.f))); // (cos 그래프에 의한 간격)
    }
    
    // Spot Light (주말 과제)
    else if (LightInfo.LightType == 2)
    {
        // 광원의 위치
        float3 vLightPos = mul(float4(LightInfo.WorldPos.xyz, 1.f), g_matView);
        
        // View Space 에서의 물체를 향한 광원의 방향
        vLight = normalize(_ViewPos - vLightPos);
        
        // 램버트 코사인 법칙으로 구한 빛의 세기   
        LightPower = saturate(dot(_ViewNormal, -vLight));
        
        // 광원과 물체 사이의 거리
        float Distance = distance(_ViewPos, vLightPos);
        
        // 거리에 따른 빛의 세기 감소율
        DistancePower = saturate(cos(saturate(Distance / LightInfo.Range) * (PI / 2.f)));
        
        // 스포트라이트의 방향
        float3 vSpotDir = normalize(mul(float4(LightInfo.WorldDir, 0.f), g_matView).xyz);
        
        // 스포트라이트 각도에 따른 빛의 세기 감소율 계산
        float SpotEffect = dot(-vLight, vSpotDir);
        float SpotAngle = cos(radians(LightInfo.Angle));
        float SpotPower = saturate((SpotEffect - SpotAngle) / (1.0f - SpotAngle));
        LightPower *= SpotPower;
    }
    
    
    // 반사 Reflect (HLSL 내부 함수 => reflect())
    float3 vReflect = normalize(vLight + dot(-vLight, _ViewNormal) * 2.f * _ViewNormal);
    
    // 시선 벡터 (픽셀이 카메라를 향한 방향)
    float3 vEye = normalize(_ViewPos);
    
    // 반사된 빛이 카메라로 들어오는 각도를 cos 으로 환산
    float ReflectPower = saturate(dot(-vEye, vReflect));
    ReflectPower = pow(ReflectPower, 20);
    
    
    // 나(픽셀)의 색상과 광원 세기, 반사광을 바탕으로 보간해서 색 및 빛을 지정한다.
    // 빛의 크기를 계산 (광원이 여러개일 수도 있기 때문에 + 를 해준다.
    _Light.vDiffuse.rgb     += LightInfo.Light.vDiffuse.rgb * LightPower * DistancePower;
    _Light.vAmbient.rgb     += LightInfo.Light.vAmbient.rgb;
    _Light.vMaxSpecular.rgb += LightInfo.Light.vDiffuse.rgb * LightInfo.Light.vMaxSpecular.rgb * ReflectPower * DistancePower;
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

int IntersectsRay(float3 _Pos[3], float3 _vStart, float3 _vDir, out float3 _CrossPos, out float _Dist)
{
        
    
    return 0;
}

#endif