#pragma once
#include "EditorUI.h"

enum ANIM_SUB
{
	SPRITEVIEW,
	PREVIEW,
	ANIMDETAIL,
};

class AnimationEditor : public EditorUI
{
private:
	class AnimSubUI*	m_Sub[3];
	bool				m_PrevActive[3];

public:
	AnimSubUI* GetSubUI(ANIM_SUB sub) { return m_Sub[sub]; }

public:
	virtual void Render_Tick() override;

	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	AnimationEditor();
	~AnimationEditor();
};