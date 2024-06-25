#pragma once
#include "ComponentUI.h"

class CameraUI : public ComponentUI
{
private:
	int		m_SelectedProjType;

public:
	virtual void Render_Tick() override;

public:
	CameraUI();
	~CameraUI();
};

