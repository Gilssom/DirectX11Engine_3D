#pragma once
#include "UIElement.h"

class UILobby : public UIElement
{
private:
	bool m_IsShow;

public:
	virtual void Render() override;

public:
	void SetGetStart(bool isShow) { m_IsShow = isShow; }

public:
	UILobby(const string& name);
	~UILobby();
};

