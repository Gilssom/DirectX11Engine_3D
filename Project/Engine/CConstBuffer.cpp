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

	// Constant Buffer ����
	m_Desc.ByteWidth = (UINT)bufferSize;
	m_Desc.MiscFlags = 0;

	// ���� �뵵 ����
	m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// �ѹ� ������ ���Ŀ� �б�, ���� ���� ( ��ü�� ��ġ�� ���� ���� )
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
	// ������ ����
	CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);

	// ������ �ֱ�
	memcpy(tSub.pData, data, m_Desc.ByteWidth);

	// ������ ���� �Ϸ�
	CONTEXT->Unmap(m_CB.Get(), 0);
}

void CConstBuffer::Binding()
{
	// ��� -> std2d.fx - b0 �� ������ ����
	CONTEXT->VSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->DSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->HSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->GSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->PSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
}

void CConstBuffer::Binding_CS()
{
	CONTEXT->CSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, m_CB.GetAddressOf());
}

void CConstBuffer::Clear()
{
	ID3D11Buffer* pBuffer = nullptr;
	CONTEXT->VSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, &pBuffer);
	CONTEXT->DSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, &pBuffer);
	CONTEXT->HSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, &pBuffer);
	CONTEXT->GSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, &pBuffer);
	CONTEXT->PSSetConstantBuffers(/*�������� ��ȣ*/ (UINT)m_Type, 1, &pBuffer);
}