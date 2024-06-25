#pragma once
#include "UIElement.h"

class UIButton : public UIElement
{
private:
	string			m_Label;
	Ptr<CTexture>	m_Texture;
	ImVec2			m_Size;

	function<void()> onClick;

public:
	virtual void Render() override;

public:
	UIButton(const string& label, function<void()> onClick);
	UIButton(Ptr<CTexture> texture, ImVec2 size, function<void()> onClick);
	~UIButton();
};