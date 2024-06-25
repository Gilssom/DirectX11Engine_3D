#pragma once
#include "EditorUI.h"

// �߻� Ŭ���� (��� �뵵)
class AssetUI : public EditorUI
{
private:
	Ptr<CAsset>			m_TargetAsset;
	const ASSET_TYPE	m_Type;


public:
	void SetTarget(Ptr<CAsset> target);
	Ptr<CAsset> GetTarget() { return m_TargetAsset; }
	ASSET_TYPE GetAssetType() { return m_Type; }

	// Asset UI �� �����ְ� �ִ� ����� ����� ���
	virtual void TargetChanged() {};

public:
	virtual void Render_Tick() = 0;

protected:
	void Render_Title();

public:
	AssetUI(const string& name, const  string& id, ASSET_TYPE type);
	~AssetUI();
};