#ifndef _COMPUTE
#define _COMPUTE

#include "value.fx"

#ifndef Compute_Shader_Info
// thread 개수 명시 (HLSL 5.0 ver Max = 1,024)
// 3차원 공간 개수 계산 ex) 2, 2, 2 = 8개 / Max = 32, 32, 1 or 1024, 1, 1
// = Group 당 Thread 개수

// int  SV_GroupIndex        : 그룹 내에서의 Thread 1차원 좌표
// int3 SV_GroupThreadID     : 그룹 내에서의 Thread 3차원 좌표
// int3 SV_GroupID           : Thread가 속한 그룹의 3차원 좌표
// int3 SV_DispatchThreadID  : 모든 그룹 내에서, 즉, 전체 기준 3차원 좌표 (Pixel 좌표로 변환해서 사용할 수 있다.)

// 읽기만 가능
//Texture2D TargetTex : register(t21);

// 읽기 및 수정 가능 (register 는 u 로 시작) = unordered register
#endif

RWTexture2D<float4> TargetTex : register(u0);


#define Width   g_int_0
#define Height  g_int_1
#define Color   g_vec4_0


[numthreads(32, 32, 1)]
void CS_Test(int3 _ThreadID : SV_DispatchThreadID)
{
    // 지정된 Texture 의 범위를 벗어난 Thread 들은 return 처리
    if (Width <= _ThreadID.x || Height <= _ThreadID.y)
        return;
    
    TargetTex[_ThreadID.xy] = Color;
}


#endif