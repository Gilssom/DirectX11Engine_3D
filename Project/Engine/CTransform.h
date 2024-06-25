#pragma once
#include "CComponent.h"

class CTransform : public CComponent
{
private:
	// Relative : �θ�κ��� ������� ��ǥ�� ����
	//			  �θ� ������ ���忡 ������� ��ǥ�� ����
	Vec3	m_RelativePos;
	Vec3	m_RelativeScale;
	Vec3	m_RelativeRotation;
	Vec3	m_RelativeDir[3]; // ���� ����

	Vec3	m_WorldDir[3];
	Matrix	m_matWorld;

	// �θ���� �������� ��ǥ ���� ���� ������
	bool	m_Absolute;

	// ����
	bool	m_IsLeft;
	bool	m_IsRight;

public:
	virtual void FinalTick() override;
	void Binding();


public:
	Vec3 GetRelativePos() { return m_RelativePos; }
	Vec3 GetRelativeScale() { return m_RelativeScale; }
	Vec3 GetRelativeRotation() { return m_RelativeRotation; }

	Vec3 GetWorldPos();
	Vec3 GetWorldScale();
	Vec3 GetWorldRotation();

	void SetRelativePos(const Vec3& vPos) { m_RelativePos = vPos; }
	void SetRelativeScale(const Vec3& vScale) { m_RelativeScale = vScale; }
	void SetRelativeRotation(const Vec3& vRotation) { m_RelativeRotation = vRotation; }

	void SetRelativePos(float x, float y, float z) { m_RelativePos = Vec3(x, y, z); }
	void SetRelativeScale(float x, float y, float z) { m_RelativeScale = Vec3(x, y, z); }
	void SetRelativeRotation(float x, float y, float z) { m_RelativeRotation = Vec3(x, y, z); }

	void SetWorldMat(const Matrix& mat) { m_matWorld = mat; }

	Vec3 GetRelativeDir(DIR_TYPE type) { return m_RelativeDir[(UINT)type]; }
	Vec3 GetWorldDir(DIR_TYPE type) { return m_WorldDir[(UINT)type]; }
	const Matrix& GetWorldMat() { return m_matWorld; }

	void SetAbsolute(bool absolute) { m_Absolute = absolute; }
	bool IsAbsolute() { return m_Absolute; }

	void SetLeft(bool isLeft) { m_IsLeft = isLeft; }
	void SetRight(bool isRight) { m_IsRight = isRight; }

	bool GetLeft() { return m_IsLeft; }
	bool GetRight() { return m_IsRight; }

	virtual void SaveToLevelFile(FILE* file) override;
	virtual void LoadFromLevelFile(FILE* file) override;

	CLONE(CTransform);

public:
	CTransform();
	~CTransform();
};

