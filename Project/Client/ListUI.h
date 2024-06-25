#pragma once
#include "EditorUI.h"

class ListUI : public EditorUI
{
private:
	vector<string>	m_vecList;
	int				m_SelectedIdx;

	// 반환 타입은 void, 인자도 void 인 함수 포인터
	UI_CALLBACK		m_CallBack;

	// 반환 타입은 UINT, EditorUI 형식이여야 하며, 인자는 없는 함수 포인터
	EditorUI*		m_Inst;
	UI_DELEGATE1	m_Delegate;
	// 원하는 함수의 기능을 해주겠다 라는 형식


public:
	const string& GetSelectedString() { return m_vecList[m_SelectedIdx]; }

	void AddItem(const string& item) { m_vecList.push_back(item); }
	void AddItem(const vector<string>& vecItem) { m_vecList.insert(m_vecList.end(), vecItem.begin(), vecItem.end()); }

	void RegisterDoubleClickCallBack(UI_CALLBACK callBack) { m_CallBack = callBack; }
	void RegisterDoubleClickDelegate(EditorUI* inst, UI_DELEGATE1 func) { m_Inst = inst; m_Delegate = func; }

public:
	virtual void Render_Tick() override;
	virtual void Deactivate() override;

public:
	ListUI();
	~ListUI();
};

