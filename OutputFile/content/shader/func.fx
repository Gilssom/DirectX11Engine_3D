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
            vLightPower = info.Light.vDiffuse.xyz * fRatio;
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

void CalLight3D(int _LightIdx, float3 _ViewPos, float3 _ViewNormal, inout tLight _Light)
{
    // ���� ����
    tLightInfo LightInfo = g_Light3D[_LightIdx];
    
    // ���� ����
    float LightPower = (float) 0.f;
    
    // ���� ����
    float3 vLight = (float3) 0.f;
    
    // ���� ������� �ȼ�(��ü) ������ ���� ���� (�븻 ����)
    _ViewNormal = normalize(_ViewNormal);
    
    // �Ÿ��� ���� ���� ���� (Directional �� ����)
    float DistancePower = 1.f;
    
    
    // Directional Light
    if(LightInfo.LightType == 0)
    {
        // View Space ������ ���� ����
        vLight = normalize(mul(float4(LightInfo.WorldDir, 0.f), g_matView).xyz);
        
        // ����Ʈ �ڻ��� ��Ģ���� ���� ���� ����    
        LightPower = saturate(dot(_ViewNormal, -vLight));
    }
    
    // Point Light
    else if (LightInfo.LightType == 1)
    {
        // ������ ��ġ
        float3 vLightPos = mul(float4(LightInfo.WorldPos.xyz, 1.f), g_matView).xyz;
        
        // View Space ������ ��ü�� ���� ������ ����
        vLight = normalize(_ViewPos - vLightPos);
        
        // ����Ʈ �ڻ��� ��Ģ���� ���� ���� ����   
        LightPower = saturate(dot(_ViewNormal, -vLight));
        
        // ������ ��ü ������ �Ÿ�
        float Distance = distance(_ViewPos, vLightPos);
        
        // �Ÿ��� ���� ���� ���� ������
        //DistancePower = saturate(1.f - Distance / LightInfo.Range);                        (������ ����)
        DistancePower = saturate(cos(saturate(Distance / LightInfo.Range) * (PI / 2.f))); // (cos �׷����� ���� ����)
    }
    
    // Spot Light (�ָ� ����)
    else if (LightInfo.LightType == 2)
    {
        // ������ ��ġ
        float3 vLightPos = mul(float4(LightInfo.WorldPos.xyz, 1.f), g_matView);
        
        // View Space ������ ��ü�� ���� ������ ����
        vLight = normalize(_ViewPos - vLightPos);
        
        // ����Ʈ �ڻ��� ��Ģ���� ���� ���� ����   
        LightPower = saturate(dot(_ViewNormal, -vLight));
        
        // ������ ��ü ������ �Ÿ�
        float Distance = distance(_ViewPos, vLightPos);
        
        // �Ÿ��� ���� ���� ���� ������
        DistancePower = saturate(cos(saturate(Distance / LightInfo.Range) * (PI / 2.f)));
        
        // ����Ʈ����Ʈ�� ����
        float3 vSpotDir = normalize(mul(float4(LightInfo.WorldDir, 0.f), g_matView).xyz);
        
        // ����Ʈ����Ʈ ������ ���� ���� ���� ������ ���
        float SpotEffect = dot(-vLight, vSpotDir);
        float SpotAngle = cos(radians(LightInfo.Angle));
        float SpotPower = saturate((SpotEffect - SpotAngle) / (1.0f - SpotAngle));
        LightPower *= SpotPower;
    }
    
    
    // �ݻ� Reflect (HLSL ���� �Լ� => reflect())
    float3 vReflect = normalize(vLight + dot(-vLight, _ViewNormal) * 2.f * _ViewNormal);
    
    // �ü� ���� (�ȼ��� ī�޶� ���� ����)
    float3 vEye = normalize(_ViewPos);
    
    // �ݻ�� ���� ī�޶�� ������ ������ cos ���� ȯ��
    float ReflectPower = saturate(dot(-vEye, vReflect));
    ReflectPower = pow(ReflectPower, 20);
    
    
    // ��(�ȼ�)�� ����� ���� ����, �ݻ籤�� �������� �����ؼ� �� �� ���� �����Ѵ�.
    // ���� ũ�⸦ ��� (������ �������� ���� �ֱ� ������ + �� ���ش�.
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

int IntersectsRay(float3 _Pos[3], float3 _vStart, float3 _vDir, out float3 _CrossPos, out float _Dist)
{
        
    
    return 0;
}

#endif