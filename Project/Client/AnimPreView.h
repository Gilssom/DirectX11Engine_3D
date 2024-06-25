#pragma once
#include "AnimSubUI.h"
#include "SpriteView.h"

class AnimPreView : public AnimSubUI
{
private:
	SpriteView* m_SpriteView;

	float		m_PlaySpeed;
	bool		m_IsPlaying;
	int			m_CurFrameIndex;
	float		m_Timer;

public:
	void SetSpriteView(SpriteView* spriteView) { m_SpriteView = spriteView; }

	void Play();
	void Stop();
	void UpdateAnimation();
	void RenderPreView();
	void SelectedFrame();

public:
	float GetSpeed() { return m_PlaySpeed; }

public:
	virtual void Render_Tick() override;

	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	AnimPreView();
	~AnimPreView();
};

