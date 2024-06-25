#pragma once
#include "AssetUI.h"

class MeshUI : public AssetUI
{
private:


public:
	virtual void Render_Tick() override;

public:
	MeshUI();
	~MeshUI();
};