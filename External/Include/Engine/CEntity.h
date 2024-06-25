#pragma once

class CEntity
{
private:
	static UINT g_NextID;


private:
	wstring m_Name;
	UINT	m_ID;


public:
	const wstring& GetName() { return m_Name; }
	void SetName(const wstring& name) { m_Name = name; }
	void ChangeName(const wstring& newName) { m_Name = newName; }
	UINT GetID() { return m_ID; }


	// 자기 자신을 복제한 객체를 반환
	virtual CEntity* Clone() = 0;

public:
	CEntity();
	CEntity(const CEntity& _Origin); // 복사 생성자 직접 구현
	virtual ~CEntity();
};

