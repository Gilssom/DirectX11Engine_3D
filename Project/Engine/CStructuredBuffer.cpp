#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer()
	: m_Desc{}
	, m_ElementSize(0)
	, m_ElementCount(0)
	, m_Type(SB_TYPE::SRV_ONLY)
	, m_SRV_Register(0)
	, m_UAV_Register(0)
{

}

CStructuredBuffer::CStructuredBuffer(const CStructuredBuffer& other)
	: CEntity(other)
	, m_SB_Main(nullptr)
	, m_SB_Write(nullptr)
	, m_SB_Read(nullptr)
	, m_Desc(other.m_Desc)
	, m_SRV(nullptr)
	, m_UAV(nullptr)
	, m_ElementSize(other.m_ElementSize)
	, m_ElementCount(other.m_ElementCount)
	, m_Type(other.m_Type)
	, m_SysMemMove(other.m_SysMemMove)
	, m_SRV_Register(0)
	, m_UAV_Register(0)
{ 
	Create(m_ElementSize, m_ElementCount, m_Type, m_SysMemMove);

	if (other.m_SB_Main != nullptr)
	{
		CONTEXT->CopyResource(m_SB_Main.Get(), other.m_SB_Main.Get());
	}
}

CStructuredBuffer::~CStructuredBuffer()
{

}

int CStructuredBuffer::Create(UINT elementSize, UINT elementCount, SB_TYPE type, bool sysMemMove, void* initialData)
{
	// ������ �ִ� �޸𸮰� �����ִ� ������ ���
	// ���ο� Create �� ������ ������ ����Ű�� ���� �� SRV �� ����Ű�� �ʰ� �ʱ�ȭ.
	m_SB_Main = nullptr;
	m_SB_Read = nullptr;
	m_SB_Write = nullptr;

	m_SRV = nullptr;
	m_UAV = nullptr;

	m_Type = type;
	m_SysMemMove = sysMemMove;

	// ������ ũ�⸦ 16�� ����� �����ؾ� �Ѵ�.
	assert(!(elementSize % 16));
	
	m_ElementSize = elementSize;
	m_ElementCount = elementCount;

	m_Desc.ByteWidth = m_ElementSize * m_ElementCount; // ������ ������ ũ��

	// �⺻�� Default. ���� Type �� ���� ������
	m_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Desc.CPUAccessFlags = 0;

	// View Type
	if (m_Type == SB_TYPE::SRV_ONLY)
	{
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else
	{
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	}

	// ���� ������� ���۰� Texture ���۰� �ƴ�, ����ȭ ���۶��� �˷��ֱ� ���� �뵵
	m_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	// ����ȭ ���۾ȿ� �ִ� ��� �ϳ��ϳ��� ũ��
	m_Desc.StructureByteStride = elementSize;

	HRESULT hr = E_FAIL;

	if (initialData)
	{
		D3D11_SUBRESOURCE_DATA tSub = {};
		tSub.pSysMem = initialData;
		hr = DEVICE->CreateBuffer(&m_Desc, &tSub, m_SB_Main.GetAddressOf());
	}
	else
	{
		hr = DEVICE->CreateBuffer(&m_Desc, nullptr, m_SB_Main.GetAddressOf());
	}

	if (FAILED(hr))
		return E_FAIL;

	// Shader Resource View ���� �� ����
	D3D11_SHADER_RESOURCE_VIEW_DESC tViewDesc = {};
	tViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tViewDesc.BufferEx.NumElements = m_ElementCount;
	hr = DEVICE->CreateShaderResourceView(m_SB_Main.Get(), &tViewDesc, m_SRV.GetAddressOf());

	// Unordered Access View ���� �� ����
	if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC tViewDesc = {};
		tViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		tViewDesc.Buffer.NumElements = m_ElementCount;
		if (FAILED(DEVICE->CreateUnorderedAccessView(m_SB_Main.Get(), &tViewDesc, m_UAV.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}
	}

	// �߰� ���� ����
	if (m_SysMemMove)
	{
		// Write (Binding X) System -> GPU
		D3D11_BUFFER_DESC writeDesc = {};
		writeDesc = m_Desc;
		writeDesc.Usage = D3D11_USAGE_DYNAMIC;
		writeDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		writeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (FAILED(DEVICE->CreateBuffer(&writeDesc, nullptr, m_SB_Write.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}

		// Read (Binding X) GPU -> System
		D3D11_BUFFER_DESC readDesc = {};
		readDesc = m_Desc;
		readDesc.Usage = D3D11_USAGE_DEFAULT;
		readDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		readDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (FAILED(DEVICE->CreateBuffer(&readDesc, nullptr, m_SB_Read.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}
	}

	return hr;
}

// �������̱� ������ Count �� ���ϴ� �� ��ŭ �޾ƾߵ�
void CStructuredBuffer::SetData(void* data, UINT elementCount)
{
	// Count �� �ȵ����� ����ȭ ������ ũ�⸸ŭ ����
	if (!elementCount)
	{
		elementCount = m_ElementCount;
	}

	D3D11_MAPPED_SUBRESOURCE tSub = {};
	// ������ ����
	CONTEXT->Map(m_SB_Write.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);

	// ������ �ֱ�
	memcpy(tSub.pData, data, elementCount * m_ElementSize);

	// ������ ���� �Ϸ�
	CONTEXT->Unmap(m_SB_Write.Get(), 0);

	// Write Buffer -> Main Buffer ������ �ű��
	CONTEXT->CopyResource(m_SB_Main.Get(), m_SB_Write.Get());
}

void CStructuredBuffer::GetData(void* pDest, UINT elementCount)
{
	// Count �� �ȵ����� ����ȭ ������ ũ�⸸ŭ ����
	if (!elementCount)
	{
		elementCount = m_ElementCount;
	}

	// Main Buffer -> Read Buffer ������ �ű��
	CONTEXT->CopyResource(m_SB_Read.Get(), m_SB_Main.Get());

	D3D11_MAPPED_SUBRESOURCE tSub = {};
	// ������ ����
	CONTEXT->Map(m_SB_Read.Get(), 0, D3D11_MAP_READ, 0, &tSub);

	// Dest �� tSub Data �ֱ�
	memcpy(pDest, tSub.pData, elementCount * m_ElementSize);

	// ������ ���� �Ϸ�
	CONTEXT->Unmap(m_SB_Read.Get(), 0);
}

void CStructuredBuffer::Binding(UINT registerNum)
{
	m_SRV_Register = registerNum;

	CONTEXT->VSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
}

void CStructuredBuffer::Binding_CS_SRV(UINT registerNum)
{
	m_SRV_Register = registerNum;

	CONTEXT->CSSetShaderResources(registerNum, 1, m_SRV.GetAddressOf());
}

void CStructuredBuffer::Binding_CS_UAV(UINT registerNum)
{
	m_UAV_Register = registerNum;

	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(registerNum, 1, m_UAV.GetAddressOf(), &i);
}

void CStructuredBuffer::Clear_SRV()
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(m_SRV_Register, 1, &pSRV);
	CONTEXT->HSSetShaderResources(m_SRV_Register, 1, &pSRV);
	CONTEXT->DSSetShaderResources(m_SRV_Register, 1, &pSRV);
	CONTEXT->GSSetShaderResources(m_SRV_Register, 1, &pSRV);
	CONTEXT->PSSetShaderResources(m_SRV_Register, 1, &pSRV);
	CONTEXT->CSSetShaderResources(m_SRV_Register, 1, &pSRV);
}

void CStructuredBuffer::Clear_UAV()
{
	ID3D11UnorderedAccessView* pUAV = nullptr;

	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(m_UAV_Register, 1, &pUAV, &i);
}