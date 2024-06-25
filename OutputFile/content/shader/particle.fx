#ifndef _PARTICLE
#define _PARTICLE

#include "value.fx"
#include "func.fx"

StructuredBuffer<tParticle>         ParticleBuffer  : register(t17);
StructuredBuffer<tParticleModule>   ModuleBuffer    : register(t18);

#define Particle    ParticleBuffer[_in[0].InstID]
#define Module      ModuleBuffer[0]


struct VS_PARTICLE_IN
{
    float3 vPos     : POSITION;
    float2 vUV      : TEXCOORD;
    uint   InstID   : SV_InstanceID; // Instance 대상의 ID
};

struct VS_PARTICLE_OUT
{
    float3 vPos     : POSITION;
    float2 vUV      : TEXCOORD;
    uint   InstID   : FOG;
};

// 1. VS 가 딱히 하는 일이 없어졌지만, 파이프라인 단계에서 VS 없이 호출될 수 없으므로,
//    GS 로 단순히 데이터 전달하는 기능이라도 구현해서 세팅해줘야 함.
VS_PARTICLE_OUT VS_Particle(VS_PARTICLE_IN _in)
{
    VS_PARTICLE_OUT output = (VS_PARTICLE_OUT) 0.f;
    
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    output.InstID = _in.InstID;
    
    return output;
}


// Geometry Shader : 정점 추가 생성 가능
// SV_Position 을 이제 Geometry 에서 받아야한다. (원래는 바로 Rasterizer - NDC 좌표계)
// 정점에서 온 정보를 Geometry Shader -> outStream 에다가 넘겼다가 Rasterizer 에 넘기는 구조
// 만약 outStream 에 정보를 넘기지 않으면 최종 정보들이 없어지기 때문에 그대로 중단되버림. (정점이 없기 때문에)
// 불필요한 단계가 있을 경우 outStream 에 정보를 넘기지만 않으면 된다.

// Bill Board 처리
// 정점 하나를 받아와서 활용하게 되면 해당 정점이 원점이자 중점이니까
// 해당 정점을 통해서 확장시켜서 Mesh 를 그리면 편하다. (카메라가 회전을 해도) ( ex) 이펙트가 회전되면 안된다.)
// 복잡한 월드 회전 행렬 계산이 필요없게됨.

struct GS_OUT
{
    float4 vPosition    : SV_Position;
    float2 vUV          : TEXCOORD;
    uint   InstID       : FOG;
};

// 정점 사용 최대 개수
[maxvertexcount(12)] // inout : Reference (HLSL 원본)
void GS_Particle(point VS_PARTICLE_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    GS_OUT output_cross[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    // 1. 비활성화된 파티클의 경우, GS 단계에서 파이프라인을 중단시킬 수 있다.
    
    // 2. Point Mesh 를 사용해서 Rect Mesh 로 분할 할 예정. 
    //    중심점에 해당하는 Point Mesh 를 View Space 에서 확장시켜서,
    //    자연스럽게 Bill Board 처리가 되도록 함.
   
    
    // 비활성화인 Particle 은 파이프라인 단계 실행 X
    if (Particle.Active == 0)
    {
        return;
    }
    
    // Paticle 중심 위치를 Local(원점) -> World -> View
    float4 vViewPos = mul(float4(Particle.vWorldPos, 1.f), g_matView);
    
    // View Space 에서 4개의 중점으로 분할
    // 0 -- 1
    // |    |
    // 3 -- 2
    output[0].vPosition = float4(-Particle.vWorldCurrentScale.x * 0.5f,  Particle.vWorldCurrentScale.y * 0.5f, 0.f, 0.f);
    output[1].vPosition = float4( Particle.vWorldCurrentScale.x * 0.5f,  Particle.vWorldCurrentScale.y * 0.5f, 0.f, 0.f);
    output[2].vPosition = float4( Particle.vWorldCurrentScale.x * 0.5f, -Particle.vWorldCurrentScale.y * 0.5f, 0.f, 0.f);
    output[3].vPosition = float4(-Particle.vWorldCurrentScale.x * 0.5f, -Particle.vWorldCurrentScale.y * 0.5f, 0.f, 0.f);
    
    if(Module.VelocityAlignment)
    {
        output_cross[0].vPosition = float4(-Particle.vWorldCurrentScale.x * 0.5f, 0.f,  Particle.vWorldCurrentScale.y * 0.5f, 0.f);
        output_cross[1].vPosition = float4( Particle.vWorldCurrentScale.x * 0.5f, 0.f,  Particle.vWorldCurrentScale.y * 0.5f, 0.f);
        output_cross[2].vPosition = float4( Particle.vWorldCurrentScale.x * 0.5f, 0.f, -Particle.vWorldCurrentScale.y * 0.5f, 0.f);
        output_cross[3].vPosition = float4(-Particle.vWorldCurrentScale.x * 0.5f, 0.f, -Particle.vWorldCurrentScale.y * 0.5f, 0.f);

        float3 vR = normalize(mul(float4(Particle.vVelocity, 0.f), g_matView).xyz);
        float3 vF = normalize(cross(vR, float3(0.f, 1.f, 0.f)));
        float3 vU = normalize(cross(vF, vR));

        float3x3 vRot =
        {
            vR,
            vU,
            vF,
        };
        
        for (int i = 0; i < 4; ++i)
        {
            output[i].vPosition.xyz = mul(output[i].vPosition.xyz, vRot);
            output_cross[i].vPosition.xyz = mul(output_cross[i].vPosition.xyz, vRot);
        }
    }
    
    for (int i = 0; i < 4; i++)
    {
        output[i].vPosition += vViewPos;
        output[i].vPosition = mul(output[i].vPosition, g_matProj);
    }
    
    // 생성시킨 정점의 UV 세팅
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
    
    // 생성시킨 정점의 Instance ID 세팅
    output[0].InstID = output[1].InstID = output[2].InstID = output[3].InstID = _in[0].InstID;
    
    // 출력 OutStream 에 Rect Mesh 를 만들기 위해 총 6개의 출력을 지정한다.
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();

    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
    
    if(Module.VelocityAlignment)
    {
        for (int i = 0; i < 4; ++i)
        {
            output_cross[i].vPosition += vViewPos;
            output_cross[i].vPosition = mul(output_cross[i].vPosition, g_matProj);
        }
    
        // 생성시킨 정점의 UV 세팅
        output_cross[0].vUV = float2(0.f, 0.f);
        output_cross[1].vUV = float2(1.f, 0.f);
        output_cross[2].vUV = float2(1.f, 1.f);
        output_cross[3].vUV = float2(0.f, 1.f);
    
        // 생성시킨 정점의 Instance ID 세팅
        output_cross[0].InstID = output_cross[1].InstID = output_cross[2].InstID = output_cross[3].InstID = _in[0].InstID;
       
        // OutStream 에 RectMesh 를 만들기 위해서 총 6개의 출력을 지정한다.
        _OutStream.Append(output_cross[0]);
        _OutStream.Append(output_cross[1]);
        _OutStream.Append(output_cross[2]);
        _OutStream.RestartStrip();
    
        _OutStream.Append(output_cross[0]);
        _OutStream.Append(output_cross[2]);
        _OutStream.Append(output_cross[3]);
        _OutStream.RestartStrip();
    }
    
    return;
}

float4 PS_Particle(GS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    // 대부분의 Shader 는 재질을 통해서 전달을 받으니까
    // 예외처리 같은 부분은 재질을 통해서 받는게 효율적이다. (픽셀 당은 효율이 너무 안좋음)
    if (g_btex_0)
    {
        vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        vOutColor *= ParticleBuffer[_in.InstID].vColor;
    }
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    return vOutColor;
}

#endif