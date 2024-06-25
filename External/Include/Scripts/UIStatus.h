#pragma once
#include "UIElement.h"

class UIStatus : public UIElement
{
private:

public:
	virtual void Render() override;

public:
	UIStatus(const string& name);
	~UIStatus();
};