#pragma once
#include "CEntity.h"

#pragma region Structure Buffer Info
// 상수 버퍼 register 의 큰 단점은 메모리 크기가 한정되어 있다.
// 매우 수동적, 동적으로 할당이 불가능함

// DX11 에서 추가된 구조화 버퍼
// Texture register 에 바인딩을 해서 데이터를 꺼내갈 수 있게
// 가변적인 데이터로 활용할 수 있음
// Texture 는 아닌데 데이터의 분량이 크거나, 가변적인 상황일 때 사용하면 좋다.
// 16Byte 단위로 딱 떨어져야 되는건 동일하다.
#pragma endregion

enum class SB_TYPE
{
	SRV_ONLY,
	SRV_UAV,
};

class CStructuredBuffer : public CEntity
{
private:
	// ** 버퍼를 3개나 두는 이유 **
	// GPU 와 System 메모리를 넘나들수 있는 역할이 있어야 할 때.
	// System Memory -> GPU : GPU Write Buffer 로 이동 후 Main 으로 Copy, 이후 Binding 가능 (바로 Main 으로 불가능)
	// GPU -> System Memory : GPU Main 버퍼를 Read 버퍼로 Copy, 이후 System Memory 로 이동.
	// 쓰기와 읽기 전용 버퍼를 둠으로써 System Memory 와 소통이 가능. (원래는 불가능)
	// GPU 는 버퍼끼리의 소통 속도가 거의 최고성능급
	// System Memory Move Bool 값이 true 면 Write 와 Read Buffer 를 생성할 예정

	ComPtr<ID3D11Buffer>				m_SB_Main;		// Main 버퍼	  (Binding)
	ComPtr<ID3D11Buffer>				m_SB_Write;		// 쓰기 전용 버퍼 (System -> GPU)
	ComPtr<ID3D11Buffer>				m_SB_Read;		// 읽기 전용 버퍼 (GPU -> System)

	D3D11_BUFFER_DESC					m_Desc;			// 버퍼 정보 구조체

	ComPtr<ID3D11ShaderResourceView>	m_SRV;			// Shader Resource View
	ComPtr<ID3D11UnorderedAccessView>	m_UAV;			// Unordered Access View
	
	UINT								m_ElementSize;	// 버퍼 각 요소 사이즈
	UINT								m_ElementCount;	// 버퍼의 요소 개수
	
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