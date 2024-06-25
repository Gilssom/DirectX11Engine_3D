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

	// Layer 설정
	m_CurLevel->GetLayer(0)->SetName(L"Default");
	m_CurLevel->GetLayer(1)->SetName(L"Player");
	m_CurLevel->GetLayer(2)->SetName(L"Monster");
	m_CurLevel->GetLayer(2)->SetName(L"Back Ground");

	CCollisionManager::GetInst()->LayerCheck(1, 2);

	// Light Object
	CGameObject* pLightObject = new CGameObject;
	pLightObject->SetName(L"Directional Light");
	pLightObject->AddComponent(new CTransform);
	pLightObject->AddComponent(new CLight2D);

	pLightObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));

	pLightObject->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pLightObject->Light2D()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
	pLightObject->Light2D()->SetAmbient(Vec3(0.f, 0.f, 0.f));
	pLightObject->Light2D()->SetRange(500.f);

	m_CurLevel->AddObject(0, pLightObject);

	// Clone Test
	//pLightObject = pLightObject->Clone();
	//pLightObject->Transform()->SetRelativePos(Vec3(300.f, 0.f, 0.f));
	//m_CurLevel->AddObject(0, pLightObject);

	// Clone Test
	/*pPlayer = pPlayer->Clone();
	pPlayer->Transform()->SetRelativePos(Vec3(100.f, 0.f, 100.f));
	m_CurLevel->AddObject(0, pPlayer, false);*/

	// Monster
	CGameObject* pMonster = new CGameObject;
	pMonster->SetName(L"Monster");
	pMonster->AddComponent(new CTransform);
	pMonster->AddComponent(new CMeshRender);
	pMonster->AddComponent(new CCollider2D);

	pMonster->Transform()->SetRelativePos(Vec3(200.f, 100.f, 0.f));
	pMonster->Transform()->SetRelativeScale(100.f, 100.f, 0.f);
	pMonster->Transform()->SetAbsolute(true);

	pMonster->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pMonster->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"Std2DMaterial"));

	pMonster->Collider2D()->SetAbsolute(false);
	pMonster->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	pMonster->Collider2D()->SetScale(Vec3(1.f, 1.f, 1.f));

	// Player 와 Monster 를 부모-자식 관계로 연결
	//pPlayer->AddChild(pMonster);
	m_CurLevel->AddObject(1, pMonster, false);

	//pPlayer->GetScript<CPlayerScript>()->SetTarget(pMonster);

	// Back Ground Object
	CGameObject* pBackGround = new CGameObject;
	pBackGround->SetName(L"BackGround_0");
	pBackGround->AddComponent(new CTransform);
	pBackGround->AddComponent(new CMeshRender);

	pBackGround->Transform()->SetRelativePos(Vec3(0.f, 0.f, 1000.f));
	pBackGround->Transform()->SetRelativeScale(Vec3(1600.f, 900.f, 1.f));

	pBackGround->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pBackGround->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"Std2DMaterial"));
	pBackGround->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CAssetManager::GetInst()->Load<CTexture>(L"texture\\Background.jpg", L"texture\\Background.jpg"));

	m_CurLevel->AddObject(3, pBackGround, false);

	// PostProcee Filter 추가
	CGameObject* pFilterObject = new CGameObject;
	pFilterObject->SetName(L"Post Process");
	pFilterObject->AddComponent(new CTransform);
	pFilterObject->AddComponent(new CMeshRender);

	pFilterObject->Transform()->SetRelativePos(200.f, 0.f, 500.f);
	pFilterObject->Transform()->SetRelativeScale(100.f, 100.f, 1.f);

	pFilterObject->MeshRender()->SetMesh(CAssetManager::GetInst()->FindAsset<CMesh>(L"RectMesh"));
	pFilterObject->MeshRender()->SetMaterial(CAssetManager::GetInst()->FindAsset<CMaterial>(L"DistortionMaterial"));
	pFilterObject->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CAssetManager::GetInst()->FindAsset<CTexture>(L"RenderTargetCopyTex"));
	pFilterObject->MeshRender()->GetMaterial()->SetTexParam(TEX_1, CAssetManager::GetInst()->Load<CTexture>(L"texture\\noise\\noise_03.jpg", L"texture\\noise\\noise_03.jpg"));

	m_CurLevel->AddObject(0, pFilterObject, false);

	// 레벨 시작
	m_CurLevel->Init();

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
