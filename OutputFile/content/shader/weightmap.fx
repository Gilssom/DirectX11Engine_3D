#ifndef _WEIGHTMAP
#define _WEIGHTMAP

#include "value.fx"
#include "struct.fx"

RWStructuredBuffer<tWeight>     WEIGHT_MAP : register(u0);
StructuredBuffer<tRaycastOut>   Raycast : register(t20);

#define WIDTH                   g_int_0
#define HEIGHT                  g_int_1
#define WEIGHT_IDX              g_int_2

#define BRUSH_TEX               g_tex_0
#define BRUSH_SCALE             g_vec2_0

[numthreads(32, 32, 1)]
void CS_WeightMap(int3 _ID : SV_DispatchThreadID)
{
    if (WIDTH <= _ID.x || HEIGHT <= _ID.y || !Raycast[0].Success)
        return;
    
    // ���콺 ��ŷ��ġ�� 2���� Index �� ��ȯ
    int2 CenterPos = float2(WIDTH, HEIGHT) * Raycast[0].Location;
    
    // Brush ������ �ػ� ������ ����
    int2 Scale = float2(WIDTH, HEIGHT) * BRUSH_SCALE;
    
    // ���콺 ��ŷ��ġ�� �߽����� Brush ������ ��� ������ ���� ��������� ����
    if (_ID.x < CenterPos.x - Scale.x / 2 || _ID.x > CenterPos.x + Scale.x / 2
        || _ID.y < CenterPos.y - Scale.y / 2 || _ID.y > CenterPos.y + Scale.y / 2)
    {
        return;
    }
    
    // (Thread ��ǥ - LeftTop ��ǥ) / Brush Scale 
    // = �ش� Thread �� �����ؾ��ϴ� Brush Texture �� UV ��ġ
    int2 LeftTop = CenterPos - Scale / 2.f;
    float2 vBrushUV = (float2)(_ID.xy - LeftTop) / (float2)Scale;
    float Alpha = BRUSH_TEX.SampleLevel(g_sam_0, vBrushUV, 0).a;
    
    // Thread �� ����� Weight Map ������ 1���� Index �� ���Ѵ�.
    int WeightMapIdx = WIDTH * _ID.y + _ID.x;
    
    // ����ġ �� Ȯ��
    float weight = WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX];
    float Add = 2.f * DeltaTime_Engine * Alpha;
    
    // ���� ����ġ �߰�
    WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX] = saturate(weight + Add);
    
    // ����ġ �ʰ��� Ȯ��
    float Total = 0.f;
    for (int i = 0; i < 8; i++)
    {
        Total += WEIGHT_MAP[WeightMapIdx].Weight[i];
    }
    
    if (Total > 1.f)
    {
        float Over = Total - 1.f;

        for (int i = 0; i < 8; i++)
        {
            if(i == WEIGHT_IDX)
                continue;
            
            float Ratio = WEIGHT_MAP[WeightMapIdx].Weight[i] / (Total - WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX]);
            
            WEIGHT_MAP[WeightMapIdx].Weight[i] -= Ratio * Over;
            
            if (WEIGHT_MAP[WeightMapIdx].Weight[i] < 0.f)
                WEIGHT_MAP[WeightMapIdx].Weight[i] = 0.f;
        }
    }
}
#endif