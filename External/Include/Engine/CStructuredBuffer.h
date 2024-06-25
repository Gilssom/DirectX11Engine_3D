#pragma once
#include "CEntity.h"

#pragma region Structure Buffer Info
// ��� ���� register �� ū ������ �޸� ũ�Ⱑ �����Ǿ� �ִ�.
// �ſ� ������, �������� �Ҵ��� �Ұ�����

// DX11 ���� �߰��� ����ȭ ����
// Texture register �� ���ε��� �ؼ� �����͸� ������ �� �ְ�
// �������� �����ͷ� Ȱ���� �� ����
// Texture �� �ƴѵ� �������� �з��� ũ�ų�, �������� ��Ȳ�� �� ����ϸ� ����.
// 16Byte ������ �� �������� �Ǵ°� �����ϴ�.
#pragma endregion

enum class SB_TYPE
{
	SRV_ONLY,
	SRV_UAV,
};

class CStructuredBuffer : public CEntity
{
private:
	// ** ���۸� 3���� �δ� ���� **
	// GPU �� System �޸𸮸� �ѳ���� �ִ� ������ �־�� �� ��.
	// System Memory -> GPU : GPU Write Buffer �� �̵� �� Main ���� Copy, ���� Binding ���� (�ٷ� Main ���� �Ұ���)
	// GPU -> System Memory : GPU Main ���۸� Read ���۷� Copy, ���� System Memory �� �̵�.
	// ����� �б� ���� ���۸� �����ν� System Memory �� ������ ����. (������ �Ұ���)
	// GPU �� ���۳����� ���� �ӵ��� ���� �ְ��ɱ�
	// System Memory Move Bool ���� true �� Write �� Read Buffer �� ������ ����

	ComPtr<ID3D11Buffer>				m_SB_Main;		// Main ����	  (Binding)
	ComPtr<ID3D11Buffer>				m_SB_Write;		// ���� ���� ���� (System -> GPU)
	ComPtr<ID3D11Buffer>				m_SB_Read;		// �б� ���� ���� (GPU -> System)

	D3D11_BUFFER_DESC					m_Desc;			// ���� ���� ����ü

	ComPtr<ID3D11ShaderResourceView>	m_SRV;			// Shader Resource View
	ComPtr<ID3D11UnorderedAccessView>	m_UAV;			// Unordered Access View
	
	UINT								m_ElementSize;	// ���� �� ��� ������
	UINT								m_ElementCount;	// ������ ��� ����
	
	SB_TYPE								m_Type;			// SRV Only or SRV + UAV
	bool								m_SysMemMove;	// System Memory Move Possible
	
	UINT								m_SRV_Register;
	UINT								m_UAV_Register;


public:
	UINT GetElementSize() { return m_ElementSize; }
	UINT GetElementCount() { return m_ElementCount; }

	int Create(UINT elementSize, UINT elementCount, SB_TYPE type, bool sysMemMove, void* initialData = nullptr);
	void SetData(void* data, UINT elementCount = 0);
	void GetData(void* pDest, UINT elementCount = 0);

	void Binding(UINT registerNum);
	void Binding_CS_SRV(UINT registerNum);
	void Binding_CS_UAV(UINT registerNum);

	void Clear_SRV();
	void Clear_UAV();


	CLONE(CStructuredBuffer);

public:
	CStructuredBuffer();
	CStructuredBuffer(const CStructuredBuffer& other);
	~CStructuredBuffer();
};