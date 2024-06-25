#include "pch.h"
#include "CComputeShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CComputeShader::CComputeShader(UINT threadPerGroupX, UINT threadPerGroupY, UINT threadPerGroupZ)
	: CShader(ASSET_TYPE::COMPUTE_SHADER)
	, m_ThreadPerGroupX(threadPerGroupX)
	, m_ThreadPerGroupY(threadPerGroupY)
	, m_ThreadPerGroupZ(threadPerGroupZ)
	, m_GroupX(1)
	, m_GroupY(1)
	, m_GroupZ(1)
{

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
				, "Compute Shader Compile 오류", MB_OK);
		}
		else
		{
			MessageBox(nullptr
				, L"파일을 찾을 수 없습니다."
				, L"Compute Shader Compile 오류", MB_OK);
		}

		return E_FAIL;
	}

	// Compute Shader 객체 생성
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
	// 상속 받아간 Class 의 Binding 함수로
	if (FAILED(Binding()))
	{
		return E_FAIL;
	}

	// 상속 받아간 Class 의 CalculateGroupNum 함수로
	CalculateGroupNum();

	// 상수 데이터를 Compute Shader 로 전달
	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->Binding_CS();

	// Compute Shader 실행
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);

	// Group Count (최대 개수 제한 X) (그룹 당 Thread 개수는 1,024개)
	CONTEXT->Dispatch(m_GroupX, m_GroupY, m_GroupZ);

	// 상속 받아간 Class 의 Clear 함수로
	Clear();

	return S_OK;
}
