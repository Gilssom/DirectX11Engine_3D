#pragma once

#include "ImGui\\\imgui.h"
#include "CImGuiManager.h"

#define SAME ImGui::SameLine(150)
#define SAME_SET(spacing) ImGui::SameLine(spacing)
#define SIZE SetSize(ImVec2(0, 150))
#define SIZE_SET(height) SetSize(ImVec2(0, height))

class EditorUI
{
private:
	string				m_Name;			// UI �̸� (��ġ�� �ʰ� ����)
	const string		m_ID;		

	EditorUI*			m_ParentUI;		// Editor UI �� �θ� UI �� ����Ŵ
	vector<EditorUI*>	m_vecChildUI;	// Editor UI �� �����ϰ� �ִ� �ڽ� UI �� ����Ŵ

	ImVec2				m_vChildSize;	// �ڽ� UI Size

	bool				m_Menu;			// ���� Menu �� �ʿ�����
	bool				m_Active;		// Ȱ��ȭ - ��Ȱ��ȭ ����
	bool				m_Seperate;		// ���м� �ɼ�
	bool				m_Modal;
	bool				m_Move;			// UI â�� ������ ������

	bool				m_IsComponent;

public:
	void SetName(const string& name) { m_Name = name; }
	const string& GetName() { return m_Name; }

	// ImVec2(0, 0) = Parent Default Size
	void SetSize(ImVec2 size) { m_vChildSize = size; }
	void SetSeperate(bool seperate) { m_Seperate = seperate; }
	bool SetActive(bool active);
	void SetModal(bool modal) { m_Modal = modal; }
	void SetComponentType(bool isComponent) { m_IsComponent = isComponent; }
	void SetMenu(bool menu) { m_Menu = menu; }
	void SetUIMove(bool move) { m_Move = move; }

	EditorUI* GetParentUI() { return m_ParentUI; }
	const vector<EditorUI*> GetChildUI() { return m_vecChildUI; }

	void AddChildUI(EditorUI* child) 
	{ 
		child->m_ParentUI = this;
		m_vecChildUI.push_back(child); 
	}

	void SetFocus();

	bool IsActive() { return m_Active; }
	// �ڽ��� Ÿ���� ��ȯ
	bool IsRootUI() { return !m_ParentUI; }

public:
	virtual void Tick();
	virtual void Render_Tick() = 0;

	virtual void Activate() {}
	virtual void Deactivate() {}

public:
	// ��ӹ޾ư� ������  Name �� ������ ������ �� �ְ�
	EditorUI(const string& name, const  string& id);
	virtual ~EditorUI();
};