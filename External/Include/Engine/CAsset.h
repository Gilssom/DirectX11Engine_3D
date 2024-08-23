#pragma once
#include "CEntity.h"

#include "ptr.h"

class CAsset : public CEntity
{
private:
	wstring				m_Key;			// �ε��� Ű��
	wstring				m_RelativePath; // ��� ���
	const ASSET_TYPE	m_Type;			// Asset �� Type
	int					m_RefCount;		// ���� ī��Ʈ
	bool				m_bEngine;		// Engine ���� ���� ������Ų Asset ����

public:
	const wstring& GetKey() { return m_Key; }
	const wstring& GetRelativePath() { return m_RelativePath; }
	ASSET_TYPE GetAssetType() { return m_Type; }
	bool IsEngineAsset() { return m_bEngine; }

	// �ּ� "1" �� Counting �Ǿ� ����(���� ���� Asset Manager �� ����)
	int GetRefCount() { return m_RefCount; }

protected:
	void SetKey(const wstring& key) { m_Key = key; }
	void SetRelativePath(const wstring& strPath) { m_RelativePath = strPath; }

private:
	virtual int Load(const wstring& FilePath) = 0;
	virtual int Save(const wstring& FilePath) = 0;

private:
	void AddRef() { m_RefCount++; }
	void Release() 
	{ 
		m_RefCount--; 

		// �ƹ��� �� Asset �� �������� �ʴ´ٸ�
		if (m_RefCount <= 0)
		{
			delete this;
		}
	}

public:
	virtual CAsset* Clone() = 0;

public:
	CAsset(ASSET_TYPE type, bool bEngine = false); // �Ǽ��� type �� ���� ������ �� �ִ� ���� ������ ( �⺻ ������ ���� )
	CAsset(const CAsset& _Origin);
	~CAsset();

	// Ptr ���� ����Ű�� T �� Private �� ���� �����ϰ� �Ѵ�.
	template<typename T>
	friend class Ptr;

	friend class CAssetManager;
	friend class CFBXLoader;
};