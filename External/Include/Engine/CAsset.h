#pragma once
#include "CEntity.h"

#include "ptr.h"

class CAsset : public CEntity
{
private:
	wstring				m_Key;			// 로딩된 키값
	wstring				m_RelativePath; // 상대 경로
	const ASSET_TYPE	m_Type;			// Asset 의 Type
	int					m_RefCount;		// 참조 카운트
	bool				m_bEngine;		// Engine 에서 직접 생성시킨 Asset 여부

public:
	const wstring& GetKey() { return m_Key; }
	const wstring& GetRelativePath() { return m_RelativePath; }
	ASSET_TYPE GetAssetType() { return m_Type; }
	bool IsEngineAsset() { return m_bEngine; }

	// 최소 "1" 이 Counting 되어 있음(최초 생성 Asset Manager 로 인해)
	int GetRefCount() { return m_RefCount; }

protected:
	void SetRelativePath(const wstring& strPath) { m_RelativePath = strPath; }

private:
	virtual int Load(const wstring& FilePath) = 0;
	virtual int Save(const wstring& FilePath) = 0;

private:
	void AddRef() { m_RefCount++; }
	void Release() 
	{ 
		m_RefCount--; 

		// 아무도 이 Asset 을 참조하지 않는다면
		if (m_RefCount <= 0)
		{
			delete this;
		}
	}

public:
	virtual CAsset* Clone() = 0;

public:
	CAsset(ASSET_TYPE type, bool bEngine = false); // 실수로 type 을 지정 못해줄 수 있는 것을 방지함 ( 기본 생성자 삭제 )
	CAsset(const CAsset& _Origin);
	~CAsset();

	// Ptr 에서 가르키는 T 의 Private 에 접근 가능하게 한다.
	template<typename T>
	friend class Ptr;

	friend class CAssetManager;
};

