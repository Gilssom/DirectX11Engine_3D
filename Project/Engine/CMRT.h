#pragma once
#include "CEntity.h"

class CMRT : public CEntity
{
private:
	Ptr<CTexture>	m_RTTex[8];	// �ִ� 8���� Render Tareget
	Vec4			m_ClearColor[8];
	UINT			m_RTCount;

	Ptr<CTexture>	m_DSTex;

public:
	void Create(Ptr<CTexture>* _RTTex, UINT _RTCount, Ptr<CTexture> _DSTex);
	void ClearTarget();
	void ClearDepthStencil(); // ���� �ؽ�ó�� �����ؼ� ����ϱ� ������ ���������� �и�
	void OMSet(); // ���� ��� ����

	void SetClearColor(UINT _Idx, Vec4 _Color) { m_ClearColor[_Idx] = _Color; }

public:
	CLONE_DISABLE(CMRT);

public:
	CMRT();
	~CMRT();
};