#include "pch.h"
#include "CGraphicShader.h"

#include "CDevice.h"

CGraphicShader::CGraphicShader()
	: CShader(ASSET_TYPE::GRAPHICS_SHADER)
	, m_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_RSType(RS_TYPE::CULL_BACK)
	, m_DSType(DS_TYPE::LESS)
	, m_BSType(BS_TYPE::DEFAULT)
	, m_Domain(SHADER_DOMAIN::DOMAIN_MASKED)
{

}

CGraphicShader::~CGraphicShader()
{

}

int CGraphicShader::CreateVertexShader(const wstring& strFilePath, const string& vsFuncName)
{
	// Vertex Shader 생성
	if (FAILED(D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, vsFuncName.c_str(), "vs_5_0", D3DCOMPILE_DEBUG, 0
		, m_VSBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		if (m_ErrBlob != nullptr)
		{
			MessageBoxA(nullptr
				, (char*)m_ErrBlob->GetBufferPointer()
				, "Vertex Shader Compile 오류", MB_OK);
		}
		else
		{
			MessageBox(nullptr
				, L"파일을 찾을 수 없습니다."
				, L"Vertex Shader Compile 오류", MB_OK);
		}

		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer()
										, m_VSBlob->GetBufferSize()
										, nullptr, m_VS.GetAddressOf())))
	{
		return E_FAIL;
	}

	// Vertex Layout 정보 생성
	D3D11_INPUT_ELEMENT_DESC Layoutdesc[3] = {};

	Layoutdesc[0].AlignedByteOffset = 0; // 정점 정보의 시작 위치
	Layoutdesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // 그에 맞는 픽셀 크기
	Layoutdesc[0].InputSlot = 0; // Vertex Buffer 의 위치
	Layoutdesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // 정점 정보
	Layoutdesc[0].InstanceDataStepRate = 0;
	Layoutdesc[0].SemanticName = "POSITION";
	Layoutdesc[0].SemanticIndex = 0;

	Layoutdesc[1].AlignedByteOffset = 12;
	Layoutdesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[1].InputSlot = 0;
	Layoutdesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Layoutdesc[1].InstanceDataStepRate = 0;
	Layoutdesc[1].SemanticName = "COLOR";
	Layoutdesc[1].SemanticIndex = 0;

	Layoutdesc[2].AlignedByteOffset = 28;
	Layoutdesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	Layoutdesc[2].InputSlot = 0;
	Layoutdesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Layoutdesc[2].InstanceDataStepRate = 0;
	Layoutdesc[2].SemanticName = "TEXCOORD";
	Layoutdesc[2].SemanticIndex = 0;

	if (FAILED(DEVICE->CreateInputLayout(Layoutdesc, 3
		, m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize()
		, m_Layout.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CGraphicShader::CreateHullShader(const wstring& strFilePath, const string& hsFuncName)
{
	// Hull Shader
	if (FAILED(D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, hsFuncName.c_str(), "hs_5_0", D3DCOMPILE_DEBUG, 0
		, m_HSBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		if (m_ErrBlob != nullptr)
		{
			MessageBoxA(nullptr
				, (char*)m_ErrBlob->GetBufferPointer()
				, "Hull Shader Compile 오류", MB_OK);
		}
		else
		{
			MessageBox(nullptr
				, L"파일을 찾을 수 없습니다."
				, L"Hull Shader Compile 오류", MB_OK);
		}

		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateHullShader(m_HSBlob->GetBufferPointer()
									  , m_HSBlob->GetBufferSize()
									  , nullptr, m_HS.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CGraphicShader::CreateDomainShader(const wstring& strFilePath, const string& dsFuncName)
{
	// Domain Shader
	if (FAILED(D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, dsFuncName.c_str(), "ds_5_0", D3DCOMPILE_DEBUG, 0
		, m_DSBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		if (m_ErrBlob != nullptr)
		{
			MessageBoxA(nullptr
				, (char*)m_ErrBlob->GetBufferPointer()
				, "Domain Shader Compile 오류", MB_OK);
		}
		else
		{
			MessageBox(nullptr
				, L"파일을 찾을 수 없습니다."
				, L"Domain Shader Compile 오류", MB_OK);
		}

		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateDomainShader(m_DSBlob->GetBufferPointer()
										, m_DSBlob->GetBufferSize()
										, nullptr, m_DS.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CGraphicShader::CreateGeometryShader(const wstring& strFilePath, const string& gsFuncName)
{
	// Geometry Shader
	if (FAILED(D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, gsFuncName.c_str(), "gs_5_0", D3DCOMPILE_DEBUG, 0
		, m_GSBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		if (m_ErrBlob != nullptr)
		{
			MessageBoxA(nullptr
				, (char*)m_ErrBlob->GetBufferPointer()
				, "Geometry Shader Compile 오류", MB_OK);
		}
		else
		{
			MessageBox(nullptr
				, L"파일을 찾을 수 없습니다."
				, L"Geometry Shader Compile 오류", MB_OK);
		}

		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateGeometryShader(m_GSBlob->GetBufferPointer()
										  , m_GSBlob->GetBufferSize()
										  , nullptr, m_GS.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CGraphicShader::CreatePixelShader(const wstring& strFilePath, const string& psFuncName)
{
	// Pixel Shader
	if (FAILED(D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, psFuncName.c_str(), "ps_5_0", D3DCOMPILE_DEBUG, 0
		, m_PSBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		if (m_ErrBlob != nullptr)
		{
			MessageBoxA(nullptr
				, (char*)m_ErrBlob->GetBufferPointer()
				, "Pixel Shader Compile 오류", MB_OK);
		}
		else
		{
			MessageBox(nullptr
				, L"파일을 찾을 수 없습니다."
				, L"Pixel Shader Compile 오류", MB_OK);
		}

		return E_FAIL;
	}

	if (FAILED(DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer()
									   , m_PSBlob->GetBufferSize()
									   , nullptr, m_PS.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CGraphicShader::Binding()
{
	CONTEXT->IASetInputLayout(m_Layout.Get());
	CONTEXT->IASetPrimitiveTopology(m_Topology); // 도형의 구조를 알려주는 함수

	CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);
	CONTEXT->HSSetShader(m_HS.Get(), nullptr, 0);
	CONTEXT->DSSetShader(m_DS.Get(), nullptr, 0);
	CONTEXT->GSSetShader(m_GS.Get(), nullptr, 0);

	CONTEXT->RSSetState(CDevice::GetInst()->GetRS(m_RSType).Get());

	CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

	CONTEXT->OMSetDepthStencilState(CDevice::GetInst()->GetDS(m_DSType).Get(), 0);

	CONTEXT->OMSetBlendState(CDevice::GetInst()->GetBS(m_BSType).Get(), Vec4(0.f, 0.f, 0.f, 0.f), 0xffffffff); // 0xffffffff 모든 마스킹 비트가 출력 될 수 있게

	return S_OK;
}