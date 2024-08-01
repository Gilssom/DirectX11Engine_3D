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
    
    //// 마우스 피킹위치를 2차원 Index 로 변환
    //int2 CenterPos = float2(WIDTH, HEIGHT) * Raycast[0].Location;
    //
    //// Brush 범위를 해상도 단위로 변경
    //int2 Scale = float2(WIDTH, HEIGHT) * BRUSH_SCALE;
    //
    //// 마우스 피킹위치를 중심으로 Brush 범위를 벗어난 영역에 대한 스레드들은 종료
    //if (_ID.x < CenterPos.x - Scale.x / 2 || _ID.x > CenterPos.x + Scale.x / 2
    //    || _ID.y < CenterPos.y - Scale.y / 2 || _ID.y > CenterPos.y + Scale.y / 2)
    //{
    //    return;
    //}
    //
    //// (Thread 좌표 - LeftTop 좌표) / Brush Scale 
    //// = 해당 Thread 가 참조해야하는 Brush Texture 의 UV 위치
    //int2 LeftTop = CenterPos - Scale / 2.f;
    //float2 vBrushUV = (float2)(_ID.xy - LeftTop) / (float2)Scale;
    //float Alpha = BRUSH_TEX.SampleLevel(g_sam_0, vBrushUV, 0).a;
    //
    //// Thread 가 담당한 Weight Map 에서의 1차원 Index 를 구한다.
    //int WeightMapIdx = WIDTH * _ID.y + _ID.x;
    //
    //WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX / 4][WEIGHT_IDX % 4] += 1.f * DeltaTime_Engine * Alpha;
    //saturate(WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX / 4][WEIGHT_IDX % 4]);

    
    
    
    // 마우스 피킹 위치 및 브러시 범위 계산
    float2 CenterPosF = float2(WIDTH, HEIGHT) * Raycast[0].Location;
    int2 CenterPos = int2(round(CenterPosF)); // 소수점 반올림 후 변환

    float2 ScaleF = float2(WIDTH, HEIGHT) * BRUSH_SCALE;
    int2 Scale = int2(round(ScaleF)); // 소수점 반올림 후 변환

    // 브러시 범위 검사
    int halfScaleX = int(floor(Scale.x / 2.0));
    int halfScaleY = int(floor(Scale.y / 2.0));

    if (_ID.x < CenterPos.x - halfScaleX || _ID.x > CenterPos.x + halfScaleX
        || _ID.y < CenterPos.y - halfScaleY || _ID.y > CenterPos.y + halfScaleY)
    {
        return;
    }

    // 브러시 텍스처 UV 위치 및 알파 값 샘플링
    int2 LeftTop = CenterPos - Scale / 2;
    float2 vBrushUV = (float2(_ID.xy) - float2(LeftTop)) / float2(Scale);
    
    // Boundary check for UV coordinates
    if (vBrushUV.x < 0.0 || vBrushUV.x > 1.0 || vBrushUV.y < 0.0 || vBrushUV.y > 1.0)
    {
        return;
    }

    float Alpha = BRUSH_TEX.SampleLevel(g_sam_0, vBrushUV, 0).a;

    // 가중치 맵 업데이트
    int WeightMapIdx = WIDTH * _ID.y + _ID.x;
    float newWeight = WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX] + DeltaTime_Engine * Alpha;
    WEIGHT_MAP[WeightMapIdx].Weight[WEIGHT_IDX] = saturate(newWeight); // 새 가중치를 클램프 후 적용
}
#endif