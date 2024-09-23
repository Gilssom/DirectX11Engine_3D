#pragma once
#include "Anim3DSubUI.h"

class Anim3DPreView : public Anim3DSubUI
{
public:
	virtual void Render_Tick() override;

	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	Anim3DPreView();
	~Anim3DPreView();
};