#pragma once
#include "UIElement.h"
#include "CSkillManager.h"

class UISkill : public UIElement
{
private:
	eSkillType m_SkillType;

public:
	virtual void Render() override;

public:
	void SkillIcon();

public:
	UISkill(const string& name, const wstring& texPath, eSkillType skillType);
	~UISkill();
};