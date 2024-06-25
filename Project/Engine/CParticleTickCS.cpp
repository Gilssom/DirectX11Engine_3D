#include "pch.h"
#include "CParticleTickCS.h"

#include "CStructuredBuffer.h"

CParticleTickCS::CParticleTickCS()
    : CComputeShader(32, 1, 1)
    , m_ParticleBuffer(nullptr)
    , m_ModuleBuffer(nullptr)
    , m_SpawnCountBuffer(nullptr)
{
    // 사용할 Noise Texture 미리 참조
    m_NoiseTex = CAssetManager::GetInst()->Load<CTexture>(L"texture\\noise\\noise_03.jpg", L"texture\\noise\\noise_03.jpg");
}

CParticleTickCS::~CParticleTickCS()
{

}

int CParticleTickCS::Binding()
{
    if (m_ParticleBuffer == nullptr || m_NoiseTex == nullptr)
        return E_FAIL;

    m_ParticleBuffer->Binding_CS_UAV(0);
    m_SpawnCountBuffer->Binding_CS_UAV(1);

    m_NoiseTex->Binding_CS_SRV(20);
    m_ModuleBuffer->Binding_CS_SRV(21);

    // 파티클 개수를 재질의 g_int_0 번에 넣어두기
    m_Const.iArr[0] = m_ParticleBuffer->GetElementCount();
    m_Const.v4Arr[0] = m_vParticleWorldPos;

    return S_OK;
}

void CParticleTickCS::CalculateGroupNum()
{
    // 계산보다 초과된 개수가 있다면 추가로 그룹 하나 더 지정
    // ex) 만약 파티클이 64개이면 그룹 2개 지정, 66개면 2개 지정 후 1개 더 지정
    m_GroupX = m_ParticleBuffer->GetElementCount() / m_ThreadPerGroupX;
    m_ParticleBuffer->GetElementCount() % m_ThreadPerGroupX ? m_GroupX += 1 : m_GroupX;

    m_GroupY = 1;
    m_GroupZ = 1;
}

void CParticleTickCS::Clear()
{
    m_ParticleBuffer->Clear_UAV();
    m_ParticleBuffer = nullptr;

    m_SpawnCountBuffer->Clear_UAV();
    m_SpawnCountBuffer = nullptr;

    m_NoiseTex->Clear_CS_SRV(20);

    m_ModuleBuffer->Clear_SRV();
    m_ModuleBuffer = nullptr;
}
