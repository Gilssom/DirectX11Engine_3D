#include "pch.h"
#include "CLandScape.h"

#include "CAssetManager.h"

void CLandScape::Init()
{
	// Land Scape ���� Mesh ����
	CreateMesh();

	// Land Scape ���� ���� ����
	Ptr<CMaterial> pMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"LandScapeMaterial");
	SetMaterial(pMaterial);
}

void CLandScape::SetFace(UINT x, UINT z)
{
	m_FaceX = x;
	m_FaceZ = z;

	// ����� Face ������ ���� �����ϰ� ����
	CreateMesh();

	// Land Scape ���� ���� ����
	Ptr<CMaterial> pMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(L"LandScapeMaterial");
	SetMaterial(pMaterial);
}

void CLandScape::CreateMesh()
{
	
}