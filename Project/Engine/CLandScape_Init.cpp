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
}

void CLandScape::CreateMesh()
{
	
}