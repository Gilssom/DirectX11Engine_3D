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

	// 자식으로 넣기만 하지 말고 포인터를 편리하게 하나 더 들고 있자. (다형성)
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