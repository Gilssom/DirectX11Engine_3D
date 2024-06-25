#pragma once
#include "CEntity.h"

// ��� ���� register �� ū ������ �޸� ũ�Ⱑ �����Ǿ� �ִ�.
// �ſ� ������, �������� �Ҵ��� �Ұ�����

// DX11 ���� �߰��� ����ȭ ����
// Texture register �� ���ε��� �ؼ� �����͸� ������ �� �ְ�
// �������� �����ͷ� Ȱ���� �� ����
// Texture �� �ƴѵ� �������� �з��� ũ�ų�, �������� ��Ȳ�� �� ����ϸ� ����.
// 16Byte ������ �� �������� �Ǵ°� �����ϴ�.

class CStructruedBuffer : public CEntity
{
private:
	ComPtr<ID3D11Buffer>				m_SB;		// ����
	D3D11_BUFFER_DESC					m_Desc;		// ���� ���� ����ü
	ComPtr<ID3D11ShaderResourceView>	m_SRV;		// Shader Resource View

	UINT								m_ElementSize;	// ���� �� ��� ������
	UINT								m_ElementCount;	// ������ ��� ����
	
public:
	UINT GetElementSize() { return m_ElementSize; }
	UINT GetElementCount() { return m_ElementCount; }

	int Create(UINT elementSize, UINT elementCount, void* initialData = nullptr);

public:
	CStructruedBuffer();
	~CStructruedBuffer();
};