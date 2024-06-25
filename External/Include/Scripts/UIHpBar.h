#pragma once
#include "UIElement.h"

class UIHpBar : public UIElement
{
private:

public:
	virtual void Render() override;

public:
	void RenderHealthBar();

public:
	UIHpBar(const string& name);
	~UIHpBar();
};