#pragma once
#include "CEntity.h"

struct tAnim2DFrame
{
	// 0 에서 1 사이의 UV 좌표계로 계산해야함 (GPU 메모리로 보내기 위한)
	Vec2	LeftTop;	// 첫 애니메이션의 왼쪽 위 좌표
	Vec2	SliceSize;	// Sprite 의 크기
	Vec2	Offset;

	float	Duration;	// 애니메이션이 재생 될 속도
	int		padding[3];	// 상수 버퍼 크기를 맞추기 위한 변수 ( 16byte )
};

class CTexture;
class CAnimator2D;

class CAnim2D : public CEntity
{
private:
	CAnimator2D*					m_Animator; // 본인을 소유하고있는 Animator
	vector<tAnim2DFrame>			m_vecFrame; // 프레임 간격으로 Animation 관리
	Vec2							m_BackGroundSize;

	int								m_CurIdx;	// 재생되고 있는 Index
	bool							m_Finish;	// 애니메이션 종료

	float							m_Time;

	Ptr<CTexture>					m_AtlasTex;	// Texture 정보

	std::function<void()>			m_StartCallBack;
	std::function<void()>			m_EventCallBack;
	std::function<void()>			m_EndCallBack;

public:
	bool IsFinish() { return m_Finish; }
	void Reset() { m_CurIdx = 0; m_Time = 0.f; m_Finish = false; }
	int GetCurIndex() { return m_CurIdx; }

	void SetStartCallBack(std::function<void()> callback) { m_StartCallBack = callback; }
	void SetEventCallBack(std::function<void()> callback) { m_EventCallBack = callback; }
	void SetEndCallBack(std::function<void()> callback) { m_EndCallBack = callback; }

public:
	void Create(Ptr<CTexture> atlasTex, Vec2 leftTopPixelPos, Vec2 vSlicePixelSize, Vec2 backGroundSize, int frameCount, UINT fps);
	void Create(Ptr<CTexture> atlasTex, vector<tAnim2DFrame>& animFrames, int frameCount);

	void SetAnimator(CAnimator2D* animator) { m_Animator = animator; }

	void Save(const wstring& relativeFolderPath);
	void Load(const wstring& relativeFolderPath);

public:
	void FinalTick();
	void Binding();
	static void Clear();

	void SaveToLevelFile(FILE* file);
	void LoadFromLevelFile(FILE* file);

	CLONE(CAnim2D);

public:
	CAnim2D();
	CAnim2D(const CAnim2D& other);
	~CAnim2D();

	friend class CAnimator2D;
};

