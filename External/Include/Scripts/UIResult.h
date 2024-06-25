#pragma once
#include "UIElement.h"

class UIResult : public UIElement
{
private:
	bool m_IsShow;

public:
	virtual void Render() override;

public:
	void SetGetEnd(bool isShow) { m_IsShow = isShow; }

public:
	UIResult(const string& name);
	~UIResult();
};