#pragma once
#include "AssetUI.h"

class MaterialUI : public AssetUI
{
private:
	UINT		m_ParamDescMaxWidth;

	TEX_PARAM	m_TargetParam;

public:
	virtual void Render_Tick() override;
	virtual void TargetChanged() override;

	UINT SelectTexture(DWORD_PTR selected);

public:
	MaterialUI();
	~MaterialUI();
};

