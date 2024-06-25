#pragma once
#include "EditorUI.h"

class CGameObject;
class ComponentUI;

class CAsset;
class AssetUI;
class ScriptUI;

class Inspector : public EditorUI
{
private:
	CGameObject*		m_TargetObject;

	// �ڽ����� �ֱ⸸ ���� ���� �����͸� ���ϰ� �ϳ� �� ��� ����. (������)
	ComponentUI*		m_arrComUI[(UINT)COMPONENT_TYPE::END];
	vector<ScriptUI*>	m_vecScriptUI;

	Ptr<CAsset>			m_TargetAsset;
	AssetUI*			m_arrAssetUI[(UINT)ASSET_TYPE::END];

public:
	virtual void Render_Tick() override;

public:
	void SetTargetObject(CGameObject* target);
	CGameObject* GetTargetObject() { return m_TargetObject; }

	void SetTargetAsset(Ptr<CAsset> asset);
	Ptr<CAsset> GetTargetAsset() { return m_TargetAsset; }

private:
	void CreateComponentUI();
	void CreateAssetUI();

public:
	Inspector();
	~Inspector();
};