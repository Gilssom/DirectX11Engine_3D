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
    
    //// ���콺 ��ŷ��ġ�� 2���� Index �� ��ȯ
    //int2 CenterPos = float2(WIDTH, HEIGHT) * Raycast[0].Location;
    //
    //// Brush ������ �ػ� ������ ����
    //int2 Scale = float2(WIDTH, HEIGHT) * BRUSH_SCALE;
    //
    //// ���콺 ��ŷ��ġ�� �߽����� Brush ������ ��� ������ ���� ��������� ����
    //if (_ID.x < CenterPos.x - Scale.x / 2 || _ID.x > CenterPos.x + Scale.x / 2
    //    || _ID.y < CenterPos.y - Scale.y / 2 || _ID.y > CenterPos.y + Scale.y / 2)
    //{
    //    return;
    //}
    //
    //// (Thread ��ǥ - LeftTop ��ǥ) / Brush Scale 
    //// = �ش� Thread �� �����ؾ��ϴ� Brush Texture �� UV ��ġ
    //int2 LeftTop = CenterPos - Scale / 2.f;
    //float2 vBrushUV = (float2)(_ID.xy - LeftTop) / (float2)Scale;
    //float Alpha = BRUSH_TEX.SampleLevel(g_sam_0, vBrushUV, 0).a;
    //
    //// Thread �� ����� Weight Map ������ 1���� Index �� ���Ѵ�.
    //int WeightMapIdx = WIDTH * _ID.y + _ID.x;
    //
    //WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX / 4][WEIGHT_IDX % 4] += 1.f * DeltaTime_Engine * Alpha;
    //saturate(WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX / 4][WEIGHT_IDX % 4]);

    
    
    
    // ���콺 ��ŷ ��ġ �� �귯�� ���� ���
    float2 CenterPosF = float2(WIDTH, HEIGHT) * Raycast[0].Location;
    int2 CenterPos = int2(round(CenterPosF)); // �Ҽ��� �ݿø� �� ��ȯ

    float2 ScaleF = float2(WIDTH, HEIGHT) * BRUSH_SCALE;
    int2 Scale = int2(round(ScaleF)); // �Ҽ��� �ݿø� �� ��ȯ

    // �귯�� ���� �˻�
    int halfScaleX = int(floor(Scale.x / 2.0));
    int halfScaleY = int(floor(Scale.y / 2.0));

    if (_ID.x < CenterPos.x - halfScaleX || _ID.x > CenterPos.x + halfScaleX
        || _ID.y < CenterPos.y - halfScaleY || _ID.y > CenterPos.y + halfScaleY)
    {
        return;
    }

    // �귯�� �ؽ�ó UV ��ġ �� ���� �� ���ø�
    int2 LeftTop = CenterPos - Scale / 2;
    float2 vBrushUV = (float2(_ID.xy) - float2(LeftTop)) / float2(Scale);
    
    // Boundary check for UV coordinates
    if (vBrushUV.x < 0.0 || vBrushUV.x > 1.0 || vBrushUV.y < 0.0 || vBrushUV.y > 1.0)
    {
        return;
    }

    float Alpha = BRUSH_TEX.SampleLevel(g_sam_0, vBrushUV, 0).a;

    // ����ġ �� ������Ʈ
    int WeightMapIdx = WIDTH * _ID.y + _ID.x;
    float newWeight = WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX] + DeltaTime_Engine * Alpha;
    WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX] = saturate(newWeight); // �� ����ġ�� Ŭ���� �� ����
}
#endif