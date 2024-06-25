#pragma once
#include "TileMapSubUI.h"

// �ڽ� UI �� �ƴ�, ������ ������ �����ִ� �������� UI
// (��, �ش� â �� ������ Tile Map Editor �� ���������� ������)

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

