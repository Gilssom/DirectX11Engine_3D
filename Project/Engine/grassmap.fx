#ifndef _GRASSMAP
#define _GRASSMAP

#include "value.fx"
#include "struct.fx"

RWStructuredBuffer<tGrassWeight>    GRASSWEIGHT_MAP : register(u0);
StructuredBuffer<tRaycastOut>       Raycast         : register(t20);

#define WIDTH                   g_int_0
#define HEIGHT                  g_int_1

#define BRUSH_TEX               g_tex_0
#define BRUSH_SCALE             g_vec2_0

[numthreads(32, 32, 1)]
void CS_GrassMap(int3 _ID : SV_DispatchThreadID)
{
    if (WIDTH <= _ID.x || HEIGHT <= _ID.y || !Raycast[0].Success)
        return;
    
    // 마우스 피킹위치를 2차원 Index 로 변환
    int2 CenterPos = float2(WIDTH, HEIGHT) * Raycast[0].Location;
    
    // Brush 범위를 해상도 단위로 변경
    int2 Scale = float2(WIDTH, HEIGHT) * BRUSH_SCALE;
    
    // 마우스 피킹위치를 중심으로 Brush 범위를 벗어난 영역에 대한 스레드들은 종료
    if (_ID.x < CenterPos.x - Scale.x / 2 || _ID.x > CenterPos.x + Scale.x / 2
        || _ID.y < CenterPos.y - Scale.y / 2 || _ID.y > CenterPos.y + Scale.y / 2)
    {
        return;
    }
    
    // (Thread 좌표 - LeftTop 좌표) / Brush Scale 
    // = 해당 Thread 가 참조해야하는 Brush Texture 의 UV 위치
    int2 LeftTop = CenterPos - Scale / 2.f;
    float2 vBrushUV = (float2) (_ID.xy - LeftTop) / (float2) Scale;
    float Alpha = BRUSH_TEX.SampleLevel(g_sam_0, vBrushUV, 0).a;
    
    // Thread 가 담당한 Grass Map 에서의 1차원 Index 를 구한다.
    int GrassMapIdx = WIDTH * _ID.y + _ID.x;
    
    // 가중치 값 확인
    float weight = GRASSWEIGHT_MAP[GrassMapIdx].GrassWeight;
    float Add = 2.f * DeltaTime_Engine * Alpha;
    
    // 최종 가중치 추가
    GRASSWEIGHT_MAP[GrassMapIdx].GrassWeight = saturate(weight + Add);
    
    // 가중치 초과량 확인
    float Total = 0.f;
    
    Total += GRASSWEIGHT_MAP[GrassMapIdx].GrassWeight;
    
    if (Total > 1.f)
    {
        float Over = Total - 1.f;

        float Ratio = GRASSWEIGHT_MAP[GrassMapIdx].GrassWeight / (Total - GRASSWEIGHT_MAP[GrassMapIdx].GrassWeight);
        
        GRASSWEIGHT_MAP[GrassMapIdx].GrassWeight -= Ratio * Over;
        
        if (GRASSWEIGHT_MAP[GrassMapIdx].GrassWeight < 0.f)
            GRASSWEIGHT_MAP[GrassMapIdx].GrassWeight = 0.f;
    }
}

#endif