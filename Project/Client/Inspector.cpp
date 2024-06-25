#include "pch.h"
#include "Inspector.h"

#include <Engine\\CLevelManager.h>
#include <Engine\\CLevel.h>
#include <Engine\\CGameObject.h>
#include <Engine\\CTransform.h>

#include "ComponentUI.h"
#include "AssetUI.h"
#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "Animator2DUI.h"
#include "Collider2DUI.h"
#include "CameraUI.h"
#include "Light2DUI.h"
#include "ParticleSystemUI.h"
#include "TileMapUI.h"
#include "ScriptUI.h"

#include "TileMapEditor.h"
#include "AnimationEditor.h"

Inspector::Inspector()
	: EditorUI("Inspector", "##Inspector")
	, m_TargetObject(nullptr)
	, m_arrComUI{}
{
	CreateComponentUI();

	CreateAssetUI();
}

Inspector::~Inspector()
{
	
}

void Inspector::Render_Tick()
{
	if (m_TargetObject == nullptr)
	{
		CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

		if (pCurLevel == nullptr)
			return;

		//CGameObject* pCurTileMap = pCurLevel->FindObjectByName(L"Anima");

		//if (!pCurTileMap)
			//return;

		//AnimationEditor* pEditor = CImGuiManager::GetInst()->FindEditorUI<AnimationEditor>("AnimationEditor");
		//pEditor->SetTileMap(pCurTileMap->TileMap());
		//pEditor->SetActive(true);
	}
}

void Inspector::SetTargetObject(CGameObject* target)
{
	// ���� Target Asset �� �ִٸ� �����ϴ� Asset UI ��Ȱ��ȭ
	if (m_TargetAsset != nullptr)
	{
		m_arrAssetUI[(UINT)m_TargetAsset->GetAssetType()]->SetActive(false);
		m_TargetAsset = nullptr;
	}

	m_TargetObject = target;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		// �ش� ������Ʈ�� ����� UI �� �����Ǿ����� �ʱ� ������ nullptr üũ ���־�� ��.
		if (m_arrComUI[i] == nullptr)
			continue;

		m_arrComUI[i]->SetTarget(m_TargetObject);
	}

	// ���� Target Object �� ���ٸ� Script UI ��ü ��Ȱ��ȭ
	if (m_TargetObject == nullptr)
	{
		for (size_t i = 0; i < m_vecScriptUI.size(); i++)
		{
			m_vecScriptUI[i]->SetActive(false);
		}
	}
	else
	{
		// Object �� Script �� �ִ��� Ȯ��, ������ Script UI Ȱ��ȭ
		const vector<CScript*>& vecScripts = m_TargetObject->GetScripts();

		// Object Script �� ���� Script UI �� ������ �� ���ٸ�, Script UI �� �߰��� ����
		if (m_vecScriptUI.size() < vecScripts.size())
		{
			size_t iAddCount = vecScripts.size() - m_vecScriptUI.size();

			for (size_t i = 0; i < iAddCount; i++)
			{
				// Script UI �߰� ����
				ScriptUI* pScriptUI = nullptr;
				pScriptUI = new ScriptUI;
				pScriptUI->SetActive(false);
				pScriptUI->SetSeperate(true);
				pScriptUI->SetComponentType(true);

				m_vecScriptUI.push_back(pScriptUI);
				AddChildUI(pScriptUI);
			}
		}

		// �� Script UI �� ������ Script �� �����Ѵ�.
		for (size_t i = 0; i < vecScripts.size(); i++)
		{
			m_vecScriptUI[i]->SetScript(vecScripts[i]);
			m_vecScriptUI[i]->SetActive(true);
		}

		// ���� Script �� �ϳ��� ������ ��ü ��Ȱ��ȭ
		if (vecScripts.size() == 0)
		{
			for (size_t i = 0; i < m_vecScriptUI.size(); i++)
			{
				m_vecScriptUI[i]->SetActive(false);
			}
		}
	}
}

void Inspector::SetTargetAsset(Ptr<CAsset> asset)
{
	// ��� Component UI ��Ȱ��ȭ
	SetTargetObject(nullptr);

	// Target Asset Setting
	m_TargetAsset = asset;

	// ��� Asset UI ��Ȱ��ȭ
	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; i++)
	{
		if (m_arrAssetUI[i] == nullptr)
			continue;

		m_arrAssetUI[i]->SetActive(false);
	}

	// Target Asset �� null �̸� ����
	if (m_TargetAsset == nullptr)
		return;

	// Target Asset �� �����ϴ� Asset UI �� Ȱ��ȭ �� Target Setting
	ASSET_TYPE AssetType = m_TargetAsset->GetAssetType();
	m_arrAssetUI[(UINT)AssetType]->SetActive(true);
	m_arrAssetUI[(UINT)AssetType]->SetTarget(m_TargetAsset);
}

void Inspector::CreateComponentUI()
{
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM] = new TransformUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER] = new MeshRenderUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D] = new Collider2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D] = new Animator2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA] = new CameraUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D] = new Light2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP] = new TileMapUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM] = new ParticleSystemUI;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (m_arrComUI[i] == nullptr)
			continue;

		m_arrComUI[i]->SetActive(false);
		m_arrComUI[i]->SetSeperate(true);
		m_arrComUI[i]->SetComponentType(true);
		AddChildUI(m_arrComUI[i]);
	}

	// Script UI
	/*ScriptUI* pScriptUI = nullptr;
	pScriptUI = new ScriptUI;
	pScriptUI->SetActive(false);
	pScriptUI->SetSeperate(true);
	pScriptUI->SetComponentType(true);

	m_vecScriptUI.push_back(pScriptUI);
	AddChildUI(pScriptUI);*/
}

#include "MeshUI.h"
#include "MeshDataUI.h"
#include "MaterialUI.h"
#include "TextureUI.h"
#include "PrefabUI.h"
#include "SoundUI.h"
#include "GraphicShaderUI.h"
#include "ComputeShaderUI.h"

void Inspector::CreateAssetUI()
{
	m_arrAssetUI[(UINT)ASSET_TYPE::MESH] = new MeshUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::MESH_DATA] = new MeshDataUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::MATERIAL] = new MaterialUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::TEXTURE] = new TextureUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::PREFAB] = new PrefabUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::SOUND] = new SoundUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::GRAPHICS_SHADER] = new GraphicShaderUI;
	m_arrAssetUI[(UINT)ASSET_TYPE::COMPUTE_SHADER] = new ComputeShaderUI;

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; i++)
	{
		if (m_arrAssetUI[i] == nullptr)
			continue;

		m_arrAssetUI[i]->SetActive(false);
		m_arrAssetUI[i]->SetSeperate(true);
		AddChildUI(m_arrAssetUI[i]);
	}
}