#pragma once
#include "CEntity.h"

struct tAnim2DFrame
{
	// 0 ���� 1 ������ UV ��ǥ��� ����ؾ��� (GPU �޸𸮷� ������ ����)
	Vec2	LeftTop;	// ù �ִϸ��̼��� ���� �� ��ǥ
	Vec2	SliceSize;	// Sprite �� ũ��
	Vec2	Offset;

	float	Duration;	// �ִϸ��̼��� ��� �� �ӵ�
	int		padding[3];	// ��� ���� ũ�⸦ ���߱� ���� ���� ( 16byte )
};

class CTexture;
class CAnimator2D;

class CAnim2D : public CEntity
{
private:
	CAnimator2D*					m_Animator; // ������ �����ϰ��ִ� Animator
	vector<tAnim2DFrame>			m_vecFrame; // ������ �������� Animation ����
	Vec2							m_BackGroundSize;

	int								m_CurIdx;	// ����ǰ� �ִ� Index
	bool							m_Finish;	// �ִϸ��̼� ����

	float							m_Time;

	Ptr<CTexture>					m_AtlasTex;	// Texture ����

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

