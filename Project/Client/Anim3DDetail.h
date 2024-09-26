#pragma once
#include "Anim3DSubUI.h"

#include <Engine\CAnimator3D.h>

class Anim3DDetail : public Anim3DSubUI
{
private:
	CAnimator3D*	m_Animator3D;

	int				m_StartFrame;
	int				m_EndFrame;
	int				m_CurFrame;
	bool			m_HasCurFrameEvent;
	string			m_CurFrameEventName;

	float			m_ZoomLevel;
	float			m_ScrollOffset;

public:
	void DrawTimeLineView(int& curFrame, int startFrame, int endFrame, float& zoomLevel, float& scrollOffset);
	bool CheckFrameEvent(int curFrame);
	void SetCurFrameInfo(int curFrame);

public:
	virtual void Render_Tick() override;

	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	Anim3DDetail();
	~Anim3DDetail();
};