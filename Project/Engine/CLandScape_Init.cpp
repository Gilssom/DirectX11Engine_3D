#include "pch.h"
#include "CLandScape.h"

#include "CAssetManager.h"
#include "CTexture.h"

void CLandScape::Init()
{
	// Land Scape 전용 Mesh 생성
	CreateMesh();

	// Land Scape 전용 재질 참조
	Ptr<CMaterial> pMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"LandScapeMaterial");
	SetMaterial(pMaterial);

	pMaterial->SetScalarParam(FLOAT_0, 1.f);
	pMaterial->GetShader()->AddScalarParam("Tess Factor", FLOAT_0);

	// LandScape 전용 Compute Shader 제작
	CreateComputeShader();

	// BrushTexture 추가	
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\TX_GlowScene_2.png"));
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\TX_HitFlash_0.png"));
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\TX_HitFlash02.png"));
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\TX_Twirl02.png"));
	AddBrushTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\FX_Flare.png"));
	m_BrushIdx = 0;
}

void CLandScape::SetFace(UINT x, UINT z)
{
	m_FaceX = x;
	m_FaceZ = z;

	// 변경된 Face 개수로 새로 제작하고 세팅
	CreateMesh();

	// Land Scape 전용 재질 참조
	Ptr<CMaterial> pMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"LandScapeMaterial");
	SetMaterial(pMaterial);
}

void CLandScape::CreateMesh()
{
	Vtx v;
	vector<Vtx> vecVtx;

	for (int Row = 0; Row < m_FaceZ + 1; ++Row)
	{
		for (int Col = 0; Col < m_FaceX + 1; ++Col)
		{
			v.vPos = Vec3((float)Col, 0.f, (float)Row);
			v.vUv = Vec2((float)Col, (float)m_FaceZ - Row);

			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

			vecVtx.push_back(v);
		}
	}


	vector<UINT> vecIdx;

	for (int Row = 0; Row < m_FaceZ; ++Row)
	{
		for (int Col = 0; Col < m_FaceX; ++Col)
		{
			// 0
			// | \
			// 2--1
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col);
			vecIdx.push_back((m_FaceX + 1) * (Row)+Col + 1);
			vecIdx.push_back((m_FaceX + 1) * (Row)+Col);

			// 1--2
			//  \ |
			//    0
			vecIdx.push_back((m_FaceX + 1) * (Row)+Col + 1);
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col);
			vecIdx.push_back((m_FaceX + 1) * (Row + 1) + Col + 1);
		}
	}

	Ptr<CMesh> pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	SetMesh(pMesh);
}

void CLandScape::CreateComputeShader()
{
	// Height Map CS 가 있으면 찾아오고, 없으면 컴파일해서 등록한다.
	m_HeightMapCS = (CHeightMapCS*)CAssetManager::GetInst()->FindAsset<CComputeShader>(L"HeightMapCS").Get();

	if (m_HeightMapCS == nullptr)
	{
		m_HeightMapCS = new CHeightMapCS;
		CAssetManager::GetInst()->AddAsset<CComputeShader>(L"HeightMapCS", m_HeightMapCS.Get());
	}

	// RaycastCS 생성
	m_RaycastCS = (CRaycastCS*)CAssetManager::GetInst()->FindAsset<CComputeShader>(L"RaycastCS").Get();
	if (nullptr == m_RaycastCS)
	{
		m_RaycastCS = new CRaycastCS;
		CAssetManager::GetInst()->AddAsset<CComputeShader>(L"RaycastCS", m_RaycastCS.Get());
	}
}

void CLandScape::CreateHeightMap(UINT width, UINT height)
{
	m_IsHeightMapCreated = true;

	m_HeightMap = CAssetManager::GetInst()->CreateTexture(L"LandScapeHeightMap", width, height
													, DXGI_FORMAT_R32_FLOAT
													, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
}