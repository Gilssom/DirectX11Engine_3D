#pragma once
#include "CEntity.h"

class CMRT : public CEntity
{
private:
	Ptr<CTexture>	m_RTTex[8];	// �ִ� 8���� Render Tareget
	Vec4			m_ClearColor[8];
	UINT			m_RTCount;
	Ptr<CTexture>	m_DSTex;

	D3D11_VIEWPORT	m_ViewPort;

public:
	void Create(Ptr<CTexture>* _RTTex, UINT _RTCount, Ptr<CTexture> _DSTex);
	void ClearTarget();
	void ClearDepthStencil(); // ���� �ؽ�ó�� �����ؼ� ����ϱ� ������ ���������� �и�
	void OMSet(); // ���� ��� ����

	Ptr<CTexture> GetRenderTarget(UINT idx) { return m_RTTex[idx]; }

	ID3D11DepthStencilView* GetDSV() const 
	{
		if (m_DSTex != nullptr)
			return m_DSTex->GetDSV().Get();
		else
			return nullptr;
	}

	void SetViewPort(const D3D11_VIEWPORT& viewPort) { m_ViewPort = viewPort; }
	const D3D11_VIEWPORT& GetViewPort() { return m_ViewPort; }

	void SetClearColor(UINT _Idx, Vec4 _Color) { m_ClearColor[_Idx] = _Color; }

public:
	CLONE_DISABLE(CMRT);

public:
	CMRT();
	~CMRT();
};