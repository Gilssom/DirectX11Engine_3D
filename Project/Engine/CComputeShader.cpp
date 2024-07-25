#include "pch.h"
#include "CComputeShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CComputeShader::CComputeShader(UINT threadPerGroupX, UINT threadPerGroupY, UINT threadPerGroupZ
						, const wstring& relativePath, const string& funcName)
	: CShader(ASSET_TYPE::COMPUTE_SHADER)
	, m_ThreadPerGroupX(threadPerGroupX)
	, m_ThreadPerGroupY(threadPerGroupY)
	, m_ThreadPerGroupZ(threadPerGroupZ)
	, m_GroupX(1)
	, m_GroupY(1)
	, m_GroupZ(1)
{
	wstring strPath = CPathManager::GetInst()->GetContentPath();
	if (FAILED(CreateComputeShader(strPath + relativePath, funcName)))
	{
		assert(nullptr);
	}
}

CComputeShader::~CComputeShader()
{

}

int CComputeShader::CreateComputeShader(const wstring& strFilePath, const string& csFuncName)
{
	// Compute Shader
	if (FAILED(D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, csFuncName.c_str(), "cs_5_0", D3DCOMPILE_DEBUG, 0
		, m_CSBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		if (m_ErrBlob != nullptr)
		{
			MessageBoxA(nullptr
				, (char*)m_ErrBlob->GetBufferPointer()
				, "Compute Shader Compile ����", MB_OK);
		}
		else
		{
			MessageBox(nullptr
				, L"������ ã�� �� �����ϴ�."
				, L"Compute Shader Compile ����", MB_OK);
		}

		return E_FAIL;
	}

	// Compute Shader ��ü ����
	if (FAILED(DEVICE->CreateComputeShader(m_CSBlob->GetBufferPointer()
		, m_CSBlob->GetBufferSize()
		, nullptr, m_CS.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CComputeShader::Execute()
{
	// ��� �޾ư� Class �� Binding �Լ���
	if (FAILED(Binding()))
	{
		return E_FAIL;
	}

	// ��� �޾ư� Class �� CalculateGroupNum �Լ���
	CalculateGroupNum();

	// ��� �����͸� Compute Shader �� ����
	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->Binding_CS();

	// Compute Shader ����
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);

	// Group Count (�ִ� ���� ���� X) (�׷� �� Thread ������ 1,024��)
	CONTEXT->Dispatch(m_GroupX, m_GroupY, m_GroupZ);

	// ��� �޾ư� Class �� Clear �Լ���
	Clear();

	return S_OK;
}
