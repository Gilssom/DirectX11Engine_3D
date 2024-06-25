#pragma once
#include "CEntity.h"

// 상수 버퍼 register 의 큰 단점은 메모리 크기가 한정되어 있다.
// 매우 수동적, 동적으로 할당이 불가능함

// DX11 에서 추가된 구조화 버퍼
// Texture register 에 바인딩을 해서 데이터를 꺼내갈 수 있게
// 가변적인 데이터로 활용할 수 있음
// Texture 는 아닌데 데이터의 분량이 크거나, 가변적인 상황일 때 사용하면 좋다.
// 16Byte 단위로 딱 떨어져야 되는건 동일하다.

class CStructruedBuffer : public CEntity
{
private:
	ComPtr<ID3D11Buffer>				m_SB;		// 버퍼
	D3D11_BUFFER_DESC					m_Desc;		// 버퍼 정보 구조체
	ComPtr<ID3D11ShaderResourceView>	m_SRV;		// Shader Resource View

	UINT								m_ElementSize;	// 버퍼 각 요소 사이즈
	UINT								m_ElementCount;	// 버퍼의 요소 개수
	
public:
	UINT GetElementSize() { return m_ElementSize; }
	UINT GetElementCount() { return m_ElementCount; }

	int Create(UINT elementSize, UINT elementCount, void* initialData = nullptr);

public:
	CStructruedBuffer();
	~CStructruedBuffer();
};