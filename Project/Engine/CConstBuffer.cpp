#include "pch.h"
#include "CConstBuffer.h"

#include "CDevice.h"

CConstBuffer::CConstBuffer()
	: m_Type(CB_TYPE::TRANSFORM)
	, m_Desc{}
{

}

CConstBuffer::~CConstBuffer()
{

}

int CConstBuffer::Create(size_t bufferSize, CB_TYPE type)
{
	m_Type = type;

	// Constant Buffer 생성
	m_Desc.ByteWidth = (UINT)bufferSize;
	m_Desc.MiscFlags = 0;

	// 버퍼 용도 설정
	m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// 한번 생성한 이후에 읽기, 쓰기 가능 ( 물체의 위치값 전달 위해 )
	m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_Desc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(DEVICE->CreateBuffer(&m_Desc, nullptr, m_CB.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CConstBuffer::SetData(void* data)
{
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	// 데이터 수정
	CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);

	// 데이터 넣기
	memcpy(tSub.pData, data, m_Desc.ByteWidth);

	// 데이터 수정 완료
	CONTEXT->Unmap(m_CB.Get(), 0);
}

void CConstBuffer::Binding()
{
	// 상수 -> std2d.fx - b0 에 데이터 복사
	CONTEXT->VSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->DSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->HSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->GSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->PSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
}

void CConstBuffer::Binding_CS()
{
	CONTEXT->CSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
}

void CConstBuffer::Clear()
{
	ID3D11Buffer* pBuffer = nullptr;
	CONTEXT->VSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, &pBuffer);
	CONTEXT->DSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, &pBuffer);
	CONTEXT->HSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, &pBuffer);
	CONTEXT->GSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, &pBuffer);
	CONTEXT->PSSetConstantBuffers(/*레지스터 번호*/ (UINT)m_Type, 1, &pBuffer);
}