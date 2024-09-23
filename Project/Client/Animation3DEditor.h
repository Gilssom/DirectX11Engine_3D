#pragma once
#include "EditorUI.h"

#include <Engine\CAnimator3D.h>

enum ANIM3D_SUB
{
	SPRITEVIEW_3D,
	PREVIEW_3D,
	DETAIL_3D,
};

class Animation3DEditor : public EditorUI
{
private:
	CAnimator3D* m_Animator3D;

	class Anim3DSubUI*	m_Sub[3];
	bool				m_PrevActive[3];

public:
	void SetAnimator3D(CAnimator3D* animator) { m_Animator3D = animator; }

	Anim3DSubUI* GetSubUI(ANIM3D_SUB sub) { return m_Sub[sub]; }
	CAnimator3D* GetAnimator3D() { return m_Animator3D; }

public:
	virtual void Render_Tick() override;

	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	Animation3DEditor();
	~Animation3DEditor();
};