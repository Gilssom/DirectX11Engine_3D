#pragma once
#include "CComponent.h"

#include "CTexture.h"

class CAnim2D;

class CAnimator2D : public CComponent
{
private:
	map<wstring, CAnim2D*>	m_mapAnim; // Animation Sprite 관리
	CAnim2D*				m_CurAnim; // 재생되고 있는 Animation
	bool					m_Repeat;  // 반복 설정


public:
	void CreateAnimation(const wstring& animName, Ptr<CTexture> atlasTex, Vec2 leftTopPixelPos, Vec2 vSlicePixelSize, Vec2 backGroundSize, int frameCount, UINT fps);
	CAnim2D* FindAnimation(const wstring& animName);
	CAnim2D* FindAnimFile(const wstring& strRelativePath);
	void LoadAnimation(const wstring& strRelativePath);

	void Play(const wstring& strAnimName, bool repeat);

	void SetRepeat(bool repeat) { m_Repeat = repeat; }
	bool GetRepeat() { return m_Repeat; }
	CAnim2D* GetCurAnim() { return m_CurAnim; }

public:
	virtual void FinalTick() override;
	void Binding();

	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CAnimator2D);

public:
	CAnimator2D();
	CAnimator2D(const CAnimator2D& other);
	~CAnimator2D();
};

