#pragma once
#include "UIElement.h"

class UIDamage : public UIElement
{
private:
	vector<ImTextureID> m_UseImage;

	ImVec2 m_Position;
	float m_StartTime;
	float m_LifeTime;
	float m_Alpha;

public:
	virtual void Render() override;

public:
	void LoadNumberImage();
	bool IsExpired() const { return ImGui::GetTime() - m_StartTime >= m_LifeTime; }

public:
	UIDamage(int damage, const ImVec2& position, float startTime);
	~UIDamage();
};