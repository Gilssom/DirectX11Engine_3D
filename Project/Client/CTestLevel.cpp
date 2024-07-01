#include "pch.h"

#include "CTestLevel.h"

#include <Engine\\CLevel.h>
#include <Engine\\CLayer.h>
#include <Engine\\CGameObject.h>
#include <Engine\\components.h>
#include <Engine\\CAnim2D.h>

#include <Scripts\\CCameraMoveScript.h>
#include <Scripts\\CMissileScript.h>

#include <Engine\\CCollisionManager.h>
#include <Engine\\CSetColorCS.h>

#include <Engine\\CStructuredBuffer.h>
#include <Engine\\CPrefab.h>

#include "CLevelSaveLoad.h"

void CTestLevel::CreateTestLevel()
{
	// Prefab ���� �� Asset Manager �� ���
	//CreatePrefab();

	CLevel* m_CurLevel = new CLevel;
	m_CurLevel->GetLayer(0)->SetName(L"Default");
	m_CurLevel->GetLayer(1)->SetName(L"Player");
	m_CurLevel->GetLayer(2)->SetName(L"Monster");

	// Camera Object ����
	CGameObject* pCamObject = new CGameObject;
	pCamObject->SetName(L"MainCamera");
	pCamObject->AddComponent(new CTransform);
	pCamObject->AddComponent(new CCamera);
	pCamObject->AddComponent(new CCameraMoveScript);

	pCamObject->Camera()->LayerCheckAll(1);
	pCamObject->Camera()->SetCameraPriority(0); // ���� ī�޶�� ����
	pCamObject->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);

	m_CurLevel->AddObject(0, pCamObject);

	// Light 3D
	CGameObject* pLight = new CGameObject;
	pLight->SetName(L"Light");
	pLight->AddComponent(new CTransform);
	pLight->AddComponent(new CLight3D);
	
	pLight->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	pLight->Light3D()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
	pLight->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pLight->Light3D()->SetRange(500.f);
	//pLight->Light3D()->SetAngle(50.f);
	//pLight->Light3D()->SetAmbient(Vec3(0.15f, 0.15f, 0.15f))

	m_CurLevel->AddObject(0, pLight);

	// Player
	CGameObject* Player = new CGameObject;
	Player->SetName(L"Player");

	Player->AddComponent(new CTransform);
	Player->AddComponent(new CMeshRender);
	Player->Transform()->SetRelativePos(Vec3(0.f, 0.f, 500.f));
	Player->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

	Player->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	Player->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"Std3DMaterial"));

	m_CurLevel->AddObject(0, Player);

	// Level Change System �� �̿��ؼ� Level �� �������� �� (Task Manager)
	ChangeLevelRegister(m_CurLevel, LEVEL_STATE::STOP);
}

void CTestLevel::CreatePrefab()
{
	//// Particle Prefab
	//CGameObject* pParticleObject = new CGameObject;
	//pParticleObject->SetName(L"Particle");
	//pParticleObject->AddComponent(new CTransform);
	//pParticleObject->AddComponent(new CParticleSystem);
	//pParticleObject->Transform()->SetRelativePos(Vec3(-675.f, 0.f, 500.f));
	//pParticleObject->ParticleSystem()->SetParticleTexture(CAssetManager::GetInst()->Load<CTexture>(L"texture\\particle\\SmokeParticleTest.png", L"texture\\particle\\SmokeParticleTest.png"));

	//Ptr<CPrefab> prefab = new CPrefab(pParticleObject);
	//CAssetManager::GetInst()->AddAsset<CPrefab>(L"ParticlePrefab", prefab);

	//wstring filePath = CPathManager::GetInst()->GetContentPath();
	//prefab->Save(filePath + L"Prefab\\Particle.pref");

	//// Missile Prefab
	//CGameObject* pNewObj = new CGameObject;
	//pNewObj->SetName(L"Missile");
	//pNewObj->AddComponent(new CTransform);
	//pNewObj->AddComponent(new CMeshRender);
	//pNewObj->AddComponent(new CMissileScript);
	//pNewObj->Transform()->SetRelativeScale(Vec3(40.f, 40.f, 40.f));
	//pNewObj->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	//pNewObj->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"Std2DMaterial"));

	//prefab = new CPrefab(pNewObj);
	//CAssetManager::GetInst()->AddAsset<CPrefab>(L"MissilePrefab", prefab);
	//prefab->Save(filePath + L"Prefab\\Missile.pref");
}
