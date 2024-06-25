#pragma once

#include "..\\Client\\ImGui\\imgui.h"

class UIElement
{
private:
	string	m_Name;
	ImVec2	m_Position;
	bool	m_Dragging;

	Ptr<CTexture>	m_Tex;
	ImTextureID		m_TexID;
	float			m_Width;
	float			m_Height;

public:
	string GetName() { return m_Name; }
	ImVec2 GetPosition() const { return m_Position; }
	void SetPosition(const ImVec2& pos) { m_Position = pos; }
	bool IsDragging() { return m_Dragging; }
	void SetDragging(bool drag) { m_Dragging = drag; }

	void ChangeTexture(const wstring& texPath);

	ImTextureID GetTexID() { return m_TexID; }
	float GetWidth() { return m_Width; }
	float GetHeight() { return m_Height; }

public:
	void Draw();
	virtual void Render() = 0;

public:
	UIElement(const string& name, const wstring& texPath);
	virtual ~UIElement() = default;
};