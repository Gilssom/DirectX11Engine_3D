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
	string				m_Name;			// UI 이름 (겹치지 않게 주의)
	const string		m_ID;		

	EditorUI*			m_ParentUI;		// Editor UI 의 부모 UI 를 가리킴
	vector<EditorUI*>	m_vecChildUI;	// Editor UI 가 소유하고 있는 자식 UI 를 가리킴

	ImVec2				m_vChildSize;	// 자식 UI Size

	bool				m_Menu;			// 개별 Menu 가 필요한지
	bool				m_Active;		// 활성화 - 비활성화 여부
	bool				m_Seperate;		// 구분선 옵션
	bool				m_Modal;
	bool				m_Move;			// UI 창을 움직일 것인지

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
	// 자신의 타입을 반환
	bool IsRootUI() { return !m_ParentUI; }

public:
	virtual void Tick();
	virtual void Render_Tick() = 0;

	virtual void Activate() {}
	virtual void Deactivate() {}

public:
	// 상속받아간 곳에선  Name 을 무조건 지정할 수 있게
	EditorUI(const string& name, const  string& id);
	virtual ~EditorUI();
};