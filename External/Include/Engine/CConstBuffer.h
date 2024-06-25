#pragma once
#include "CEntity.h"

class CConstBuffer : public CEntity
{
private:
	// 상수 버퍼 ( Constant Buffer ) 물체의 위치, 크기, 회전 정보를 전달하는 용도의 Buffer
	ComPtr<ID3D11Buffer>	m_CB;
	D3D11_BUFFER_DESC		m_Desc;
	CB_TYPE					m_Type;

public:
	// 몇번 레지스터에 옮길것인지 설정해야함
	int Create(size_t bufferSize, CB_TYPE type);
	void SetData(void* data);
	void Binding();
	void Binding_CS();
	void Clear();


	CLONE_DISABLE(CConstBuffer);

public:
	CConstBuffer();
	~CConstBuffer();
};

