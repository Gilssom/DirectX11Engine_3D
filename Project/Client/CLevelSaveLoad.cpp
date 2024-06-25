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
		MessageBox(nullptr, L"���� ���� ����", L"���� ���� ����", MB_OK);
		return;
	}

	// Level �� �̸� ����
	SaveWString(level->GetName(), pFile);

	// Level �� ������ ��� Layer ������ ����
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// Layer �̸� ����
		CLayer* pLayer = level->GetLayer(i);
		SaveWString(pLayer->GetName(), pFile);

		// Layer �� ������ ������Ʈ���� ����(�θ� ������Ʈ ����)
		const vector<CGameObject*>& vecParents = pLayer->GetParentObjects();

		// ParentObject ���� ����
		size_t objectCount = vecParents.size();
		fwrite(&objectCount, sizeof(size_t), 1, pFile);

		// �ش� Layer �� �Ҽӵ� ������ GameObject �������� ����
		for (size_t i = 0; i < objectCount; i++)
		{
			SaveGameObject(vecParents[i], pFile);
		}
	}

	// �浹ü ���� ����
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

		// ���� ���� Component Type �� �ϴ� ����
		COMPONENT_TYPE type = pComponent->GetComponentType();
		fwrite(&type, sizeof(UINT), 1, file);

		// Component Name
		SaveWString(pComponent->GetName(), file);

		// Component Type �� ���� ������ �������� ����
		pComponent->SaveToLevelFile(file);
	}

	COMPONENT_TYPE end = COMPONENT_TYPE::END;
	fwrite(&end, sizeof(UINT), 1, file);

	// Script
	const vector<CScript*> vecScripts = object->GetScripts();
	size_t scriptCount = vecScripts.size();

	// Script ���� ����
	fwrite(&scriptCount, sizeof(size_t), 1, file);

	// ��� Script ���� ����
	for (size_t i = 0; i < vecScripts.size(); i++)
	{
		wstring scriptName = CScriptManager::GetScriptName(vecScripts[i]);
		SaveWString(scriptName, file);
		vecScripts[i]->SaveToLevelFile(file);
	}

	// Layer Index ����
	//size_t curIndex = object->GetLayerIdx();
	//fwrite(&curIndex, sizeof(size_t), 1, file);

	// Child Object
	const vector<CGameObject*>& vecChild = object->GetChildren();

	// �ڽ� ������Ʈ ���� ����
	size_t childCount = vecChild.size();
	fwrite(&childCount, sizeof(size_t), 1, file);

	// �ڽ� ������Ʈ ���� ��ŭ ��������� ����
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
		MessageBox(nullptr, L"���� �ҷ����� ����", L"���� �ҷ����� ����", MB_OK);
		return nullptr;
	}

	CLevel* pLevel = new CLevel;

	// Level �� �̸� �ε�
	wstring strLevelName;
	LoadWString(strLevelName, pFile);
	pLevel->SetName(strLevelName);

	// Level �� ������ ��� Layer ������ �ε�
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// Layer �̸� �ε�
		CLayer* pLayer = pLevel->GetLayer(i);
		wstring strLayerName();
		LoadWString(strLevelName, pFile);
		pLayer->SetName(strLevelName);

		// ParentObject ���� �ε�
		size_t objectCount = 0;
		fread(&objectCount, sizeof(size_t), 1, pFile);

		// ����� ������Ʈ ������ŭ ������Ʈ���� �ε� �� Layer �� ����(�θ� ������Ʈ ����)
		for (size_t i = 0; i < objectCount; i++)
		{
			CGameObject* pParentObject = LoadGameObject(pFile);
			pLayer->AddObject(pParentObject);
		}
	}

	// �浹ü ���� �ε�
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
		// ����� Component Type �� ���� Ȯ��
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

		// ����� Component Name Ȯ��
		LoadWString(Name, file);
		pComponent->SetName(Name);

		pObject->AddComponent(pComponent);
		pComponent->LoadFromLevelFile(file);
	}

	// Script
	// ����� Script ���� Ȯ��
	size_t scriptCount = 0;
	fread(&scriptCount, sizeof(size_t), 1, file);

	// ����� Script ������ŭ ����� ��� ���� Ȯ��
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
	// ����� �ڽ� ������Ʈ ���� Ȯ��
	size_t childCount = 0;
	fread(&childCount, sizeof(size_t), 1, file);

	// Ȯ�ε� �ڽ� ������Ʈ ���� ��ŭ �ε�(�ڽ��� �ڽĵ� ���������� �ε�)
	for (size_t i = 0; i < childCount; i++)
	{
		CGameObject* pChild = LoadGameObject(file);
		pObject->AddChild(pChild);
	}

	return pObject;
}
