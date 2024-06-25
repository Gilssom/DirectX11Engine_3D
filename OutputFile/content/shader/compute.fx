#ifndef _COMPUTE
#define _COMPUTE

#include "value.fx"

#ifndef Compute_Shader_Info
// thread ���� ��� (HLSL 5.0 ver Max = 1,024)
// 3���� ���� ���� ��� ex) 2, 2, 2 = 8�� / Max = 32, 32, 1 or 1024, 1, 1
// = Group �� Thread ����

// int  SV_GroupIndex        : �׷� �������� Thread 1���� ��ǥ
// int3 SV_GroupThreadID     : �׷� �������� Thread 3���� ��ǥ
// int3 SV_GroupID           : Thread�� ���� �׷��� 3���� ��ǥ
// int3 SV_DispatchThreadID  : ��� �׷� ������, ��, ��ü ���� 3���� ��ǥ (Pixel ��ǥ�� ��ȯ�ؼ� ����� �� �ִ�.)

// �б⸸ ����
//Texture2D TargetTex : register(t21);

// �б� �� ���� ���� (register �� u �� ����) = unordered register
#endif

RWTexture2D<float4> TargetTex : register(u0);


#define Width   g_int_0
#define Height  g_int_1
#define Color   g_vec4_0


[numthreads(32, 32, 1)]
void CS_Test(int3 _ThreadID : SV_DispatchThreadID)
{
    // ������ Texture �� ������ ��� Thread ���� return ó��
    if (Width <= _ThreadID.x || Height <= _ThreadID.y)
        return;
    
    TargetTex[_ThreadID.xy] = Color;
}


#endif