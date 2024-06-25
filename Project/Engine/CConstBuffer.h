#pragma once
#include "CEntity.h"

class CConstBuffer : public CEntity
{
private:
	// ��� ���� ( Constant Buffer ) ��ü�� ��ġ, ũ��, ȸ�� ������ �����ϴ� �뵵�� Buffer
	ComPtr<ID3D11Buffer>	m_CB;
	D3D11_BUFFER_DESC		m_Desc;
	CB_TYPE					m_Type;

public:
	// ��� �������Ϳ� �ű������ �����ؾ���
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

