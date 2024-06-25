#pragma once
#include "AnimSubUI.h"

struct Frame 
{
	ImVec2 topLeft;
	ImVec2 bottomRight;
	ImVec2 offset;

	ImVec2 UI_topLeft;
	ImVec2 UI_bottomRight;
};

class SpriteView : public AnimSubUI
{
private:
	float			m_DefaultSize;
	float			m_SpriteScale;

	ImVec2			m_ImageSize;
	ImVec2			m_TextureOffset;
	Vec2			m_ImageRectMin;
	Vec2			m_MouseFinalPos;

	Ptr<CTexture>	m_SpriteTexture;

	vector<Frame>	m_Frames;
	ImVec2			m_UIStartPos;
	ImVec2			m_UIEndPos;
	ImVec2			m_StartPos;
	ImVec2			m_EndPos;
	bool			isSelecting = false;
	bool			isSelectFrame;
	int				m_SelectedFrameIndex = -1;

	float			m_ScrollX;
	float			m_ScrollY;
	float			m_MaxScrollX;
	float			m_MaxScrollY;
	ImVec2			m_Offset;

public:
	void SetSpriteTexture(Ptr<CTexture> tex) { m_SpriteTexture = tex; m_Frames.clear(); }
	Ptr<CTexture> GetSpriteTexture() const { return m_SpriteTexture; }

	void MouseCheck();
	void AddFrame(const ImVec2& topLeft, const ImVec2& bottomRight, const ImVec2& ui_TopLeft, const ImVec2& ui_BottomRight);
	void DrawFrame(ImDrawList* drawList);
	void SetOffset();
	void DeleteFrame();

public:
	vector<Frame>& GetFrames() { return m_Frames; }

public:
	virtual void Render_Tick() override;

	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	SpriteView();
	~SpriteView();
};