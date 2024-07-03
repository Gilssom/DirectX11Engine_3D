#pragma once
#include "EditorUI.h"

class ListUI : public EditorUI
{
private:
	vector<string>	m_vecList;
	int				m_SelectedIdx;

	// ��ȯ Ÿ���� void, ���ڵ� void �� �Լ� ������
	UI_CALLBACK		m_CallBack;

	// ��ȯ Ÿ���� UINT, EditorUI �����̿��� �ϸ�, ���ڴ� ���� �Լ� ������
	EditorUI*		m_Inst;
	UI_DELEGATE1	m_Delegate;
	// ���ϴ� �Լ��� ����� ���ְڴ� ��� ����


public:
	const string& GetSelectedString() { return m_vecList[m_SelectedIdx]; }

	void AddItem(const string& item) { m_vecList.push_back(item); }
	void AddItem(const vector<string>& vecItem) { m_vecList.insert(m_vecList.end(), vecItem.begin(), vecItem.end()); }

	void RegisterDoubleClickCallBack(UI_CALLBACK callBack) { m_CallBack = callBack; }
	void RegisterDoubleClickDelegate(EditorUI* inst, UI_DELEGATE1 func) { m_Inst = inst; m_Delegate = func; }

public:
	virtual void Render_Tick() override;
	virtual void Activate() override;
	virtual void Deactivate() override;

public:
	ListUI();
	~ListUI();
};

