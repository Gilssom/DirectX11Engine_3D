#pragma once
#include "EditorUI.h"

class TreeUI;

class ContentUI : public EditorUI
{
private:
	TreeUI*				m_Tree;
	vector<wstring>		m_vecContentName;	// Content ������ �ִ� Asset �����

public:
	void RenewContent();
	void ReloadContent();

private:
	void FindFileName(const wstring& strFolderPath);
	UINT SelectAsset(DWORD_PTR data);
	ASSET_TYPE GetAssetTypeFromExt(const path& assetName);

public:
	virtual void Render_Tick() override;


public:
	ContentUI();
	~ContentUI();
};

