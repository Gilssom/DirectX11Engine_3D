#include "pch.h"
#include "CMRT.h"

#include "CDevice.h"
#include "CTexture.h"

CMRT::CMRT()
	: m_RTTex{}
	, m_RTCount(0)
	, m_DSTex(nullptr)
	, m_ViewPort{}
{

}

CMRT::~CMRT()
{

}

void CMRT::Create(Ptr<CTexture>* _RTTex, UINT _RTCount, Ptr<CTexture> _DSTex)
{
	for (UINT i = 0; i < _RTCount; i++)
	{
		m_RTTex[i] = _RTTex[i];
	}

	m_RTCount = _RTCount;
	m_DSTex = _DSTex;

	// ViewPort 설정
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = _RTTex[0]->GetWidth();
	m_ViewPort.Height = _RTTex[0]->GetHeight();

	// 깊이 텍스쳐에 저장되는 깊이 Min, Max 지정
	m_ViewPort.MinDepth = 0;
	m_ViewPort.MaxDepth = 1;
}

void CMRT::OMSet()
{
	ID3D11RenderTargetView* m_RTView[8] = {};
	for (UINT i = 0; i < m_RTCount; i++)
	{
		m_RTView[i] = m_RTTex[i]->GetRTV().Get();
	}

	ID3D11DepthStencilView* DSView = nullptr;
	if (m_DSTex.Get())
		DSView = m_DSTex->GetDSV().Get();

	CONTEXT->OMSetRenderTargets(m_RTCount, m_RTView, DSView);

	// ViewPort 정보 세팅
	CONTEXT->RSSetViewports(1, &m_ViewPort);
}

void CMRT::ClearTarget()
{
	for (UINT i = 0; i < m_RTCount; i++)
	{
		CONTEXT->ClearRenderTargetView(m_RTTex[i]->GetRTV().Get(), m_ClearColor[i]);
	}
}

void CMRT::ClearDepthStencil()
{
	if (m_DSTex == nullptr)
		return;

	// Depth Stencil 에서 자신의 깊이 값보다 작은 값들의 물체들만 그려줌
	// 그래서 Depth Stencil View 의 초기값은 1로 초기화 해야 함
	CONTEXT->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}