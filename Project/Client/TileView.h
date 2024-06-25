#pragma once
#include "TileMapSubUI.h"

// 자식 UI 가 아닌, 각자의 내용을 보여주는 독립적인 UI
// (단, 해당 창 및 정보는 Tile Map Editor 가 최종적으로 관리함)

class TileView : public TileMapSubUI
{
private:
	float	m_DefaultSize;
	float	m_CaptureScale;

	int		m_SelectedRow;
	int		m_SelectedCol;

	Vec2	m_ImageRectMin;

public:
	virtual void Render_Tick() override;
	virtual void Deactivate() override;

private:
	void MouseCheck();

public:
	TileView();
	~TileView();
};

