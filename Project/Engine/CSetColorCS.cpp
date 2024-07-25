#include "pch.h"
#include "CSetColorCS.h"

CSetColorCS::CSetColorCS()
	: CComputeShader(32, 32, 1, L"shader\\compute.fx", "CS_Test") // Thread ���� ���� �ʼ��� ����ȭ
{

}

CSetColorCS::~CSetColorCS()
{

}

int CSetColorCS::Binding()
{
	if (m_TargetTex == nullptr || m_TargetTex->GetUAV() == nullptr)
		return E_FAIL;

	m_TargetTex->Binding_CS_UAV(0);

	m_Const.v4Arr[0] = m_ClearColor;
	m_Const.iArr[0] = (UINT)m_TargetTex->GetWidth();
	m_Const.iArr[1] = (UINT)m_TargetTex->GetHeight();

	return S_OK;
}

void CSetColorCS::CalculateGroupNum()
{
	// Thread �� ������ Texture �� �ػ󵵰� �� ������ ���������� ���� ����.

	m_GroupX = (UINT)m_TargetTex->GetWidth() / m_ThreadPerGroupX;
	(UINT)m_TargetTex->GetWidth() % m_ThreadPerGroupX ? m_GroupX += 1 : m_GroupX;

	m_GroupY = (UINT)m_TargetTex->GetHeight() / m_ThreadPerGroupY;
	(UINT)m_TargetTex->GetHeight() % m_ThreadPerGroupY ? m_GroupY += 1 : m_GroupY;

	m_GroupZ = 1;
}

void CSetColorCS::Clear()
{
	m_TargetTex->Clear_CS_UAV(0);
}