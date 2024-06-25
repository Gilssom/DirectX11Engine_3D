#pragma once
#include "CComponent.h"

class CCollider2D : public CComponent
{
private:
	static bool		m_IsShow;

	Vec3	m_Offset;		// Offset ��ġ
	Vec3	m_FinalPos;		// �浹ü�� ���� ��ġ ( Object Position + Collider Offset )

	Vec3	m_Scale;		// Object ������� ����
	Vec3	m_Rotation;		// Object ������� ȸ�� (����)

	bool	m_Absolute;		// true �� ���, Object ũ�⿡ ���� ���� �ʴ� ���� ũ�� (����)

	Matrix	m_matColWorld;

	int		m_OverlapCount; // �浹ü�� �ٸ� �浹ü�� ���� �ִ� ����

	bool	m_Active;		// �浹ü�� Ȱ��ȭ ����
	bool	m_SemiDeactive; // ��Ȱ��ȭ ���� ����


public:
	static void SetShow() { m_IsShow = !m_IsShow; }

public:
	void SetOffset(Vec3 offset) { m_Offset = offset; }
	void SetScale(Vec3 scale) { m_Scale = scale; }
	void SetRotationZ(float angle) { m_Rotation.z = angle; }

	Vec3 GetOffset() { return m_Offset; }
	Vec3 GetScale() { return m_Scale; }
	Vec3 GetFinalPos() { return m_FinalPos; }
	float GetRotationZ() { return m_Rotation.z; }

	const Matrix& GetWorldMat() { return m_matColWorld; }

	void SetAbsolute(bool abs) { m_Absolute = abs; }
	bool IsAbsolute() { return m_Absolute; }

	void Activate();
	void Deactivate();

	bool IsActive() { return m_Active; }
	bool IsSemiDeactive() { return m_SemiDeactive; }

public:
	virtual void FinalTick() override;

public:
	void BeginOverlap(CCollider2D* otherCollider);
	void Overlap(CCollider2D* otherCollider);
	void EndOverlap(CCollider2D* otherCollider);

	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CCollider2D);

public:
	CCollider2D();
	CCollider2D(const CCollider2D& other);
	~CCollider2D();

	friend class CTaskManager;
};

