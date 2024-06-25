#pragma once
#include "EditorUI.h"

class TreeUI;

class TreeNode
{
private:
	static	UINT	g_ID;

private:
	TreeUI*				m_Owner;
	TreeNode*			m_ParentNode;
	vector<TreeNode*>	m_vecChildNode;
	string				m_Name;
	string				m_ID;

	DWORD_PTR			m_Data;

	bool				m_bFrame;
	bool				m_bSelected;

public:
	void SetName(const string& name) { m_Name = name; }
	void SetData(DWORD_PTR data) { m_Data = data; }
	void SetFrame(bool frame) { m_bFrame = frame; }
	void AddChildNode(TreeNode* child)
	{
		m_vecChildNode.push_back(child);
		child->m_ParentNode = this;
	}

	const string& GetName() { return m_Name; }
	DWORD_PTR GetData() { return m_Data; }
	const vector<TreeNode*>& GetChildNode() { return m_vecChildNode; }

public:
	void Render_Tick();

public:
	TreeNode()
		: m_Owner(nullptr)
		, m_ParentNode(nullptr)
		, m_Data(0)
		, m_bFrame(false)
		, m_bSelected(false)
	{
		// ��� ������ ������ ID �� �����ؾ� �Ѵ�.
		char buffer[50] = {};

		_itoa_s(g_ID++, buffer, 10);
		m_ID = buffer;
	}

	TreeNode(const string& name, DWORD_PTR data)
		: m_Owner(nullptr)
		, m_ParentNode(nullptr)
		, m_Name(name)
		, m_Data(data)
		, m_bFrame(false)
		, m_bSelected(false)
	{
		char buffer[50] = {};

		_itoa_s(g_ID++, buffer, 10);
		m_ID = buffer;
	}

	~TreeNode();

	friend class TreeUI;
};

// ������ Data �� Tree ������ ��Ÿ���� ���� ���� ����
class TreeUI : public EditorUI
{
private:
	TreeNode*		m_RootNode;			// �ֻ��� Root Node
	TreeNode*		m_SelectedNode;		// ���õ� Node

	TreeNode*		m_DragNode;			// Drag Target Node
	TreeNode*		m_DropNode;			// Drop Target Node

	bool			m_bShowRoot;		// Root Node �� �������� �Ⱥ�������
	bool			m_bShowFileName;	// File �̸��� �������� �Ⱥ�������
	bool			m_bUseDrag;			// Drag
	bool			m_bUseDrop;			// Drop

	UI_CALLBACK		m_SelectedCallBack;

	EditorUI*		m_SelectedInst;
	UI_DELEGATE1	m_SelectedDelegate;

	EditorUI*		m_DragDropInst;
	UI_DELEGATE2	m_DragDropDelegate;


public:
	void ShowRoot(bool show) { m_bShowRoot = show; }
	void ShowFileNameOnly(bool showFileName) { m_bShowFileName = showFileName; }

	void UseDrag(bool drag) { m_bUseDrag = drag; }
	void UseDragDrop(bool dragdrop) { m_bUseDrag = dragdrop; m_bUseDrop = dragdrop; }

	TreeNode* AddTreeNode(TreeNode* parent, const string& nodeName, DWORD_PTR dwData = 0);

	void RegisterSelectCallBack(UI_CALLBACK callBack) { m_SelectedCallBack = callBack; }
	void RegisterSelectDelegate(EditorUI* inst, UI_DELEGATE1 func) { m_SelectedInst = inst; m_SelectedDelegate = func; }
	void RegisterDragDropDelegate(EditorUI* inst, UI_DELEGATE2 func) { m_DragDropInst = inst; m_DragDropDelegate = func; }

	void Clear() { if (m_RootNode != nullptr) { delete m_RootNode; m_RootNode = nullptr; }}


private:
	void SetSelectedNode(TreeNode* node);
	void SetDragNode(TreeNode* node) { m_DragNode = node; }
	void SetDropNode(TreeNode* node) { m_DropNode = node; }


public:
	virtual void Render_Tick() override;


public:
	TreeUI(const string& name);
	~TreeUI();

	friend class TreeNode;
};

