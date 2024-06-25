#include "pch.h"
#include "CLevelSaveLoad.h"

#include <Engine\\CLevel.h>
#include <Engine\\CLevelManager.h>
#include <Engine\\CLayer.h>
#include <Engine\\CGameObject.h>
#include <Engine\\components.h>
#include <Engine\\CCollisionManager.h>
#include <Engine\\CScript.h>

#include <Scripts\\CScriptManager.h>

void CLevelSaveLoad::SaveLevel(CLevel* level, const wstring& filePath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, filePath.c_str(), L"wb");

	if (pFile == nullptr)
	{
		MessageBox(nullptr, L"파일 저장 실패", L"레벨 저장 실패", MB_OK);
		return;
	}

	// Level 의 이름 저장
	SaveWString(level->GetName(), pFile);

	// Level 이 소유한 모든 Layer 정보를 저장
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// Layer 이름 저장
		CLayer* pLayer = level->GetLayer(i);
		SaveWString(pLayer->GetName(), pFile);

		// Layer 가 보유한 오브젝트들을 저장(부모 오브젝트 기준)
		const vector<CGameObject*>& vecParents = pLayer->GetParentObjects();

		// ParentObject 개수 저장
		size_t objectCount = vecParents.size();
		fwrite(&objectCount, sizeof(size_t), 1, pFile);

		// 해당 Layer 에 소속된 각각의 GameObject 정보들을 저장
		for (size_t i = 0; i < objectCount; i++)
		{
			SaveGameObject(vecParents[i], pFile);
		}
	}

	// 충돌체 정보 저장
	CCollisionManager::GetInst()->SaveToLevelFile(pFile);

	fclose(pFile);
}

void CLevelSaveLoad::SaveGameObject(CGameObject* object, FILE* file)
{
	// Name
	SaveWString(object->GetName(), file);

	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		CComponent* pComponent = object->GetComponent((COMPONENT_TYPE)i);

		if (pComponent == nullptr)
			continue;

		// 제일 먼저 Component Type 을 일단 저장
		COMPONENT_TYPE type = pComponent->GetComponentType();
		fwrite(&type, sizeof(UINT), 1, file);

		// Component Name
		SaveWString(pComponent->GetName(), file);

		// Component Type 에 따라 각자의 정보들을 저장
		pComponent->SaveToLevelFile(file);
	}

	COMPONENT_TYPE end = COMPONENT_TYPE::END;
	fwrite(&end, sizeof(UINT), 1, file);

	// Script
	const vector<CScript*> vecScripts = object->GetScripts();
	size_t scriptCount = vecScripts.size();

	// Script 개수 정보
	fwrite(&scriptCount, sizeof(size_t), 1, file);

	// 모든 Script 정보 저장
	for (size_t i = 0; i < vecScripts.size(); i++)
	{
		wstring scriptName = CScriptManager::GetScriptName(vecScripts[i]);
		SaveWString(scriptName, file);
		vecScripts[i]->SaveToLevelFile(file);
	}

	// Layer Index 저장
	//size_t curIndex = object->GetLayerIdx();
	//fwrite(&curIndex, sizeof(size_t), 1, file);

	// Child Object
	const vector<CGameObject*>& vecChild = object->GetChildren();

	// 자식 오브젝트 개수 저장
	size_t childCount = vecChild.size();
	fwrite(&childCount, sizeof(size_t), 1, file);

	// 자식 오브젝트 개수 만큼 재귀적으로 저장
	for (size_t i = 0; i < childCount; i++)
	{
		SaveGameObject(vecChild[i], file);
	}
}

CLevel* CLevelSaveLoad::LoadLevel(const wstring& filePath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, filePath.c_str(), L"rb");

	if (pFile == nullptr)
	{
		MessageBox(nullptr, L"파일 불러오기 실패", L"레벨 불러오기 실패", MB_OK);
		return nullptr;
	}

	CLevel* pLevel = new CLevel;

	// Level 의 이름 로드
	wstring strLevelName;
	LoadWString(strLevelName, pFile);
	pLevel->SetName(strLevelName);

	// Level 이 소유한 모든 Layer 정보를 로딩
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// Layer 이름 로드
		CLayer* pLayer = pLevel->GetLayer(i);
		wstring strLayerName();
		LoadWString(strLevelName, pFile);
		pLayer->SetName(strLevelName);

		// ParentObject 개수 로드
		size_t objectCount = 0;
		fread(&objectCount, sizeof(size_t), 1, pFile);

		// 저장된 오브젝트 개수만큼 오브젝트들을 로드 후 Layer 에 재등록(부모 오브젝트 기준)
		for (size_t i = 0; i < objectCount; i++)
		{
			CGameObject* pParentObject = LoadGameObject(pFile);
			pLayer->AddObject(pParentObject);
		}
	}

	// 충돌체 정보 로드
	CCollisionManager::GetInst()->LoadToLevelFile(pFile);

	fclose(pFile);

	return pLevel;
}

CGameObject* CLevelSaveLoad::LoadGameObject(FILE* file)
{
	CGameObject* pObject = new CGameObject;

	// Name
	wstring Name;
	LoadWString(Name, file);
	pObject->SetName(Name);

	// Component
	COMPONENT_TYPE type = COMPONENT_TYPE::END;
	CComponent* pComponent = nullptr;

	while (true)
	{
		// 저장된 Component Type 을 먼저 확인
		fread(&type, sizeof(UINT), 1, file);

		if (type == COMPONENT_TYPE::END)
			break;

		switch (type)
		{
		case COMPONENT_TYPE::TRANSFORM:
			pComponent = new CTransform;
			break;
		case COMPONENT_TYPE::CAMERA:
			pComponent = new CCamera;
			break;
		case COMPONENT_TYPE::COLLIDER2D:
			pComponent = new CCollider2D;
			break;
		case COMPONENT_TYPE::COLLIDER3D:
			//pComponent = new CCollider3D;
			break;
		case COMPONENT_TYPE::ANIMATOR2D:
			pComponent = new CAnimator2D;
			break;
		case COMPONENT_TYPE::ANIMATOR3D:
			//pComponent = new CAnimator3D;
			break;
		case COMPONENT_TYPE::LIGHT2D:
			pComponent = new CLight2D;
			break;
		case COMPONENT_TYPE::LIGHT3D:
			//pComponent = new CLight3D;
			break;
		case COMPONENT_TYPE::MESHRENDER:
			pComponent = new CMeshRender;
			break;
		case COMPONENT_TYPE::DECAL:
			//pComponent = new CDecal;
			break;
		case COMPONENT_TYPE::PARTICLESYSTEM:
			pComponent = new CParticleSystem;
			break;
		case COMPONENT_TYPE::TILEMAP:
			pComponent = new CTileMap;
			break;
		case COMPONENT_TYPE::LANDSCAPE:
			//pComponent = new CLandScape;
			break;
		}

		// 저장된 Component Name 확인
		LoadWString(Name, file);
		pComponent->SetName(Name);

		pObject->AddComponent(pComponent);
		pComponent->LoadFromLevelFile(file);
	}

	// Script
	// 저장된 Script 개수 확인
	size_t scriptCount = 0;
	fread(&scriptCount, sizeof(size_t), 1, file);

	// 저장된 Script 개수만큼 저장된 모든 정보 확인
	for (size_t i = 0; i < scriptCount; i++)
	{
		wstring scriptName;
		LoadWString(scriptName, file);
		CScript* pScript = CScriptManager::GetScript(scriptName);
		pObject->AddComponent(pScript);
		pScript->LoadFromLevelFile(file);
	}

	// Layer Index 
	//size_t curIndex = 0;
	//fread(&curIndex, sizeof(size_t), 1, file);
	//pObject->ChangeLayerIdx(curIndex);

	// Child Object
	// 저장된 자식 오브젝트 개수 확인
	size_t childCount = 0;
	fread(&childCount, sizeof(size_t), 1, file);

	// 확인된 자식 오브젝트 개수 만큼 로드(자식의 자식도 순차적으로 로드)
	for (size_t i = 0; i < childCount; i++)
	{
		CGameObject* pChild = LoadGameObject(file);
		pObject->AddChild(pChild);
	}

	return pObject;
}
