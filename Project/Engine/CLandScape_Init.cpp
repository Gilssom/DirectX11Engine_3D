#include "pch.h"
#include "CLandScape.h"

#include "CAssetManager.h"

void CLandScape::Init()
{
	// Land Scape 전용 Mesh 생성
	CreateMesh();

	// Land Scape 전용 재질 참조
	Ptr<CMaterial> pMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"LandScapeMaterial");
	SetMaterial(pMaterial);
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

	pMaterial->SetScalarParam(FLOAT_0, 1.f);
	pMaterial->GetShader()->AddScalarParam("Tess Factor", FLOAT_0);
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