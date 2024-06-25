#pragma once
#include "ComponentUI.h"

class MeshRenderUI : public ComponentUI
{
private:

public:
	virtual void Render_Tick() override;

	UINT SelectMesh(DWORD_PTR selected);
	UINT SelectMaterial(DWORD_PTR selected);

public:
	MeshRenderUI();
	~MeshRenderUI();
};

