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
	// Prefab 제작 및 Asset Manager 에 등록
	//CreatePrefab();

	CLevel* m_CurLevel = new CLevel;
	m_CurLevel->GetLayer(0)->SetName(L"Default");
	m_CurLevel->GetLayer(1)->SetName(L"Player");
	m_CurLevel->GetLayer(2)->SetName(L"Monster");

	// Camera Object 생성
	CGameObject* pCamObject = new CGameObject;
	pCamObject->SetName(L"MainCamera");
	pCamObject->AddComponent(new CTransform);
	pCamObject->AddComponent(new CCamera);
	pCamObject->AddComponent(new CCameraMoveScript);

	pCamObject->Camera()->LayerCheckAll(1);
	pCamObject->Camera()->SetCameraPriority(0); // 메인 카메라로 설정
	pCamObject->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);

	m_CurLevel->AddObject(0, pCamObject);

	// Light 3D
	CGameObject* pLight = new CGameObject;
	pLight->SetName(L"Light 1");
	pLight->AddComponent(new CTransform);
	pLight->AddComponent(new CLight3D);
	
	pLight->Transform()->SetRelativePos(500.f, 100.f, 500.f);
	pLight->Transform()->SetRelativeRotation(XM_PI / 4.f, XM_PI / 4.f, 0.f);

	pLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pLight->Light3D()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
	pLight->Light3D()->SetSpecular(Vec3(0.2f, 0.2f, 0.2f));
	pLight->Light3D()->SetRange(400.f);
	//pLight->Light3D()->SetAngle(500.f);
	pLight->Light3D()->SetAmbient(Vec3(0.15f, 0.15f, 0.15f));

	m_CurLevel->AddObject(0, pLight);


	// SkyBox
	CGameObject* pSkyBox = new CGameObject;
	pSkyBox->SetName(L"SkyBox");

	pSkyBox->AddComponent(new CTransform);
	pSkyBox->AddComponent(new CSkyBox);

	pSkyBox->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));

	pSkyBox->SkyBox()->SetSkyBoxType(SPHERE);
	pSkyBox->SkyBox()->SetSkyBoxTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\skybox\\Sky02.jpg"));

	//pSkyBox->SkyBox()->SetSkyBoxType(CUBE);
	//pSkyBox->SkyBox()->SetSkyBoxTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\skybox\\SkyDawn.dds"));

	m_CurLevel->AddObject(0, pSkyBox);


	// Player
	CGameObject* Player = new CGameObject;
	Player->SetName(L"Player");

	Player->AddComponent(new CTransform);
	Player->AddComponent(new CMeshRender);
	Player->AddComponent(new CBoundingBox);

	Player->Transform()->SetRelativePos(Vec3(0.f, 0.f, 500.f));
	Player->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1000.f));
	//Player->Transform()->SetRelativeRotation(Vec3(XM_PI / 2.f, 0.f, 0.f));

	Player->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
	Player->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"Std3DDeferredMaterial"));
	Player->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\Background.jpg"));

	Player->BoundingBox()->SetAbsolute(true);
	Player->BoundingBox()->SetOffset(Vec3(0.f, 0.f, 0.f));
	Player->BoundingBox()->SetRadius(600.f);

	m_CurLevel->AddObject(0, Player);


	// Decal
	CGameObject* pDecal = new CGameObject;
	pDecal->SetName(L"Decal");

	pDecal->AddComponent(new CTransform);
	pDecal->AddComponent(new CDecal);

	pDecal->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 200.f));
	pDecal->Decal()->SetOutputTexture(CAssetManager::GetInst()->FindAsset<CTexture>(L"texture\\Character.png"));
	pDecal->Decal()->SetAsLight(true);
	pDecal->Decal()->SetDecalPower(0.8f);

	m_CurLevel->AddObject(0, pDecal);

	// Level Change System 을 이용해서 Level 을 전달해줄 것 (Task Manager)
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
