#pragma once
#include "UIElement.h"

class UIMpBar : public UIElement
{
private:

public:
	virtual void Render() override;

public:
	void RenderManaBar();

public:
	UIMpBar(const string& name);
	~UIMpBar();
};

