#pragma once
#include "UIElement.h"

class UIMiniMap : public UIElement
{
private:

public:
	virtual void Render() override;

public:
	UIMiniMap(const string& name);
	~UIMiniMap();
};