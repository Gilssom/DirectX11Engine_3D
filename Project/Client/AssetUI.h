#pragma once
#include "EditorUI.h"

// 추상 클래스 (상속 용도)
class AssetUI : public EditorUI
{
private:
	Ptr<CAsset>			m_TargetAsset;
	const ASSET_TYPE	m_Type;


public:
	void SetTarget(Ptr<CAsset> target);
	Ptr<CAsset> GetTarget() { return m_TargetAsset; }
	ASSET_TYPE GetAssetType() { return m_Type; }

	// Asset UI 가 보여주고 있는 대상이 변경된 경우
	virtual void TargetChanged() {};

public:
	virtual void Render_Tick() = 0;

protected:
	void Render_Title();

public:
	AssetUI(const string& name, const  string& id, ASSET_TYPE type);
	~AssetUI();
};