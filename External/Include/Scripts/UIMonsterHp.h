#pragma once
#include "UIElement.h"

struct HpBarImage
{
	ImTextureID		textureID;
	float			width;
	float			height;
	ImVec4			color;
};

struct NumberImage
{
	ImTextureID		textureID;
	float			width;
	float			height;
};

class UIMonsterHp : public UIElement
{
public:
	vector<HpBarImage>		m_HpBars;
	vector<HpBarImage>		m_BossHpBars;
	vector<NumberImage>		m_NumberImages;
	NumberImage				m_MultiplyImage;

	int						m_LineMaxHp;
	bool					m_IsShow;

public:
	void Init();

public:
	virtual void Render() override;

public:
	void RenderHealthBar();

public:
	UIMonsterHp(const string& name);
	~UIMonsterHp();
};