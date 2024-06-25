#pragma once
#include "AnimSubUI.h"
#include "SpriteView.h"

class AnimDetail : public AnimSubUI
{
private:
	SpriteView*	m_SpriteView;
	Frame*		m_SelectedFrame;

public:
	void SetSpriteView(SpriteView* spriteView) { m_SpriteView = spriteView; }
	void SetSelectFrame(Frame* frame) { m_SelectedFrame = frame; }

public:
	virtual void Render_Tick() override;

	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	AnimDetail();
	~AnimDetail();
};