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
	// Vertex Layout 정보 생성
	D3D11_INPUT_ELEMENT_DESC Layoutdesc[21] = {};

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

	Layoutdesc[3].AlignedByteOffset = 36;
	Layoutdesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	Layoutdesc[3].InputSlot = 0;
	Layoutdesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Layoutdesc[3].InstanceDataStepRate = 0;
	Layoutdesc[3].SemanticName = "TANGENT";
	Layoutdesc[3].SemanticIndex = 0;

	Layoutdesc[4].AlignedByteOffset = 48;
	Layoutdesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	Layoutdesc[4].InputSlot = 0;
	Layoutdesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Layoutdesc[4].InstanceDataStepRate = 0;
	Layoutdesc[4].SemanticName = "NORMAL";
	Layoutdesc[4].SemanticIndex = 0;

	Layoutdesc[5].AlignedByteOffset = 60;
	Layoutdesc[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	Layoutdesc[5].InputSlot = 0;
	Layoutdesc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Layoutdesc[5].InstanceDataStepRate = 0;
	Layoutdesc[5].SemanticName = "BINORMAL";
	Layoutdesc[5].SemanticIndex = 0;

	Layoutdesc[6].SemanticName = "BLENDWEIGHT";
	Layoutdesc[6].SemanticIndex = 0;
	Layoutdesc[6].AlignedByteOffset = 72;
	Layoutdesc[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[6].InputSlot = 0;
	Layoutdesc[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Layoutdesc[6].InstanceDataStepRate = 0;

	Layoutdesc[7].SemanticName = "BLENDINDICES";
	Layoutdesc[7].SemanticIndex = 0;
	Layoutdesc[7].AlignedByteOffset = 88;
	Layoutdesc[7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[7].InputSlot = 0;
	Layoutdesc[7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Layoutdesc[7].InstanceDataStepRate = 0;

	// 두번째 슬롯
	Layoutdesc[8].SemanticName = "WORLD";
	Layoutdesc[8].SemanticIndex = 0;
	Layoutdesc[8].AlignedByteOffset = 0;
	Layoutdesc[8].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[8].InputSlot = 1;
	Layoutdesc[8].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[8].InstanceDataStepRate = 1;

	Layoutdesc[9].SemanticName = "WORLD";
	Layoutdesc[9].SemanticIndex = 1;
	Layoutdesc[9].AlignedByteOffset = 16;
	Layoutdesc[9].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[9].InputSlot = 1;
	Layoutdesc[9].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[9].InstanceDataStepRate = 1;

	Layoutdesc[10].SemanticName = "WORLD";
	Layoutdesc[10].SemanticIndex = 2;
	Layoutdesc[10].AlignedByteOffset = 32;
	Layoutdesc[10].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[10].InputSlot = 1;
	Layoutdesc[10].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[10].InstanceDataStepRate = 1;

	Layoutdesc[11].SemanticName = "WORLD";
	Layoutdesc[11].SemanticIndex = 3;
	Layoutdesc[11].AlignedByteOffset = 48;
	Layoutdesc[11].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[11].InputSlot = 1;
	Layoutdesc[11].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[11].InstanceDataStepRate = 1;

	Layoutdesc[12].SemanticName = "WV";
	Layoutdesc[12].SemanticIndex = 0;
	Layoutdesc[12].AlignedByteOffset = 64;
	Layoutdesc[12].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[12].InputSlot = 1;
	Layoutdesc[12].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[12].InstanceDataStepRate = 1;

	Layoutdesc[13].SemanticName = "WV";
	Layoutdesc[13].SemanticIndex = 1;
	Layoutdesc[13].AlignedByteOffset = 80;
	Layoutdesc[13].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[13].InputSlot = 1;
	Layoutdesc[13].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[13].InstanceDataStepRate = 1;

	Layoutdesc[14].SemanticName = "WV";
	Layoutdesc[14].SemanticIndex = 2;
	Layoutdesc[14].AlignedByteOffset = 96;
	Layoutdesc[14].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[14].InputSlot = 1;
	Layoutdesc[14].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[14].InstanceDataStepRate = 1;

	Layoutdesc[15].SemanticName = "WV";
	Layoutdesc[15].SemanticIndex = 3;
	Layoutdesc[15].AlignedByteOffset = 112;
	Layoutdesc[15].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[15].InputSlot = 1;
	Layoutdesc[15].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[15].InstanceDataStepRate = 1;
	
	Layoutdesc[16].SemanticName = "WVP";
	Layoutdesc[16].SemanticIndex = 0;
	Layoutdesc[16].AlignedByteOffset = 128;
	Layoutdesc[16].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[16].InputSlot = 1;
	Layoutdesc[16].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[16].InstanceDataStepRate = 1;
	
	Layoutdesc[17].SemanticName = "WVP";
	Layoutdesc[17].SemanticIndex = 1;
	Layoutdesc[17].AlignedByteOffset = 144;
	Layoutdesc[17].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[17].InputSlot = 1;
	Layoutdesc[17].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[17].InstanceDataStepRate = 1;
	
	Layoutdesc[18].SemanticName = "WVP";
	Layoutdesc[18].SemanticIndex = 2;
	Layoutdesc[18].AlignedByteOffset = 160;
	Layoutdesc[18].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[18].InputSlot = 1;
	Layoutdesc[18].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[18].InstanceDataStepRate = 1;
	
	Layoutdesc[19].SemanticName = "WVP";
	Layoutdesc[19].SemanticIndex = 3;
	Layoutdesc[19].AlignedByteOffset = 176;
	Layoutdesc[19].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Layoutdesc[19].InputSlot = 1;
	Layoutdesc[19].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[19].InstanceDataStepRate = 1;
	
	Layoutdesc[20].SemanticName = "ROWINDEX";
	Layoutdesc[20].SemanticIndex = 0;
	Layoutdesc[20].AlignedByteOffset = 192;
	Layoutdesc[20].Format = DXGI_FORMAT_R32_UINT;
	Layoutdesc[20].InputSlot = 1;
	Layoutdesc[20].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	Layoutdesc[20].InstanceDataStepRate = 1;


	if (FAILED(D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, vsFuncName.c_str(), "vs_5_0", D3DCOMPILE_DEBUG, 0
		, m_VSBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		if (nullptr != m_ErrBlob)
		{
			MessageBoxA(nullptr
				, (char*)m_ErrBlob->GetBufferPointer()
				, "버텍스 쉐이더 컴파일 오류", MB_OK);
		}
		else
		{
			MessageBox(nullptr
				, L"파일을 찾을 수 없습니다."
				, L"버텍스 쉐이더 컴파일 오류", MB_OK);
		}

		return E_FAIL;
	}


	if (FAILED(DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize()
		, nullptr, m_VS.GetAddressOf())))
	{
		return E_FAIL;
	}


	if (FAILED(DEVICE->CreateInputLayout(Layoutdesc, 8
		, m_VSBlob->GetBufferPointer()
		, m_VSBlob->GetBufferSize()
		, m_Layout.GetAddressOf())))
	{
		return E_FAIL;
	}


	// 추가 버텍스쉐이더(Instancing 버전) 컴파일
	if (FAILED(D3DCompileFromFile(strFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, (vsFuncName + "_Inst").c_str(), "vs_5_0", D3DCOMPILE_DEBUG, 0
		, m_VSInstBlob.GetAddressOf()
		, m_ErrBlob.GetAddressOf())))
	{
		// Instancing 버전이 없어도 문제는 없다.
		return S_OK;
	}
	else
	{
		// 컴파일 된 코드로 Vertex Instancing Shader 객체 만들기
		DEVICE->CreateVertexShader(m_VSInstBlob->GetBufferPointer(), m_VSInstBlob->GetBufferSize(), nullptr, m_VSInst.GetAddressOf());

		DEVICE->CreateInputLayout(Layoutdesc, 21
			, m_VSInstBlob->GetBufferPointer()
			, m_VSInstBlob->GetBufferSize()
			, m_LayoutInst.GetAddressOf());
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

int CGraphicShader::Binding_Inst()
{
	CONTEXT->IASetInputLayout(m_LayoutInst.Get());
	CONTEXT->IASetPrimitiveTopology(m_Topology);

	CONTEXT->VSSetShader(m_VSInst.Get(), nullptr, 0);
	//CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);
	CONTEXT->HSSetShader(m_HS.Get(), nullptr, 0);
	CONTEXT->DSSetShader(m_DS.Get(), nullptr, 0);
	CONTEXT->GSSetShader(m_GS.Get(), nullptr, 0);

	CONTEXT->RSSetState(CDevice::GetInst()->GetRS(m_RSType).Get());

	CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

	CONTEXT->OMSetDepthStencilState(CDevice::GetInst()->GetDS(m_DSType).Get(), 0);
	CONTEXT->OMSetBlendState(CDevice::GetInst()->GetBS(m_BSType).Get(), Vec4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

	return S_OK;
}
