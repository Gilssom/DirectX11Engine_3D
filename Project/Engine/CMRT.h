#pragma once
#include "CEntity.h"

class CMRT : public CEntity
{
private:
	Ptr<CTexture>	m_RTTex[8];	// 최대 8개의 Render Tareget
	Vec4			m_ClearColor[8];
	UINT			m_RTCount;

	Ptr<CTexture>	m_DSTex;

public:
	void Create(Ptr<CTexture>* _RTTex, UINT _RTCount, Ptr<CTexture> _DSTex);
	void ClearTarget();
	void ClearDepthStencil(); // 깊이 텍스처는 공유해서 사용하기 때문에 독립적으로 분리
	void OMSet(); // 최종 출력 지정

	void SetClearColor(UINT _Idx, Vec4 _Color) { m_ClearColor[_Idx] = _Color; }

public:
	CLONE_DISABLE(CMRT);

public:
	CMRT();
	~CMRT();
};