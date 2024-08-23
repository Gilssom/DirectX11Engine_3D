#pragma once

class CEntity
{
private:
	static UINT g_NextID;


private:
	wstring m_Name;
	UINT	m_ID;


public:
	const wstring& GetName() const { return m_Name; }
	void SetName(const wstring& name) { m_Name = name; }
	void ChangeName(const wstring& newName) { m_Name = newName; }
	UINT GetID() { return m_ID; }


	// �ڱ� �ڽ��� ������ ��ü�� ��ȯ
	virtual CEntity* Clone() = 0;

public:
	CEntity();
	CEntity(const CEntity& _Origin); // ���� ������ ���� ����
	virtual ~CEntity();
};

