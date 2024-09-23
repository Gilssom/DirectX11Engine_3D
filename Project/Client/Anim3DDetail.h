#pragma once
#include "Anim3DSubUI.h"

#include <Engine\CAnimator3D.h>

class Anim3DDetail : public Anim3DSubUI
{
private:
	CAnimator3D*	m_Animator3D;

	float			m_ZoomLevel;
	float			m_ScrollOffset;

public:
	void DrawTimeLineView(int& curFrame, int maxFrame, float& zoomLevel, float& scrollOffset);

public:
	virtual void Render_Tick() override;

	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	Anim3DDetail();
	~Anim3DDetail();
};