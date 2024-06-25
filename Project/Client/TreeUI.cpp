#include "pch.h"
#include "TreeUI.h"

// ===========
//  Tree Node
// ===========
UINT TreeNode::g_ID = 0;

TreeNode::~TreeNode()
{
	Safe_Del_Vector(m_vecChildNode);
}

void TreeNode::Render_Tick()
{
	// Tree Node �� ��� �̸� ����
	string name;

	if (m_Name.empty())
	{
		name = string("None") + "##" + m_ID;
	}
	else
	{
		// ���� �̸��� ���� Node �� ������ ID �� �ֱ� ������ Tree �� ������ �� �ְ� �ȴ�.
		name = m_Name + "##" + m_ID;
	}


	// ȭ��ǥ�� �����ų�, ����Ŭ���� ��쿡�� �ڽ� ��带 ��ģ��.
	UINT iFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	// Frame ����
	if (m_bFrame)
		iFlag |= ImGuiTreeNodeFlags_Framed;

	// Selected ����
	if (m_bSelected)
		iFlag |= ImGuiTreeNodeFlags_Selected;

	// File Name ����
	if (m_Owner->m_bShowFileName)
	{
		char buffer[256] = {};
		_splitpath_s(name.c_str(), 0, 0, 0, 0, buffer, 256, 0, 0);
		name = buffer;
	}

	// Arrow ����
	if (m_vecChildNode.empty())
	{
		iFlag |= ImGuiTreeNodeFlags_Leaf;

		if(m_bFrame)
			name = "       " + name;
	}

	if (ImGui::TreeNodeEx(name.c_str(), iFlag))
	{
		// Node �� ������(Tree UI) �� Drag ����� ����ϱ�� �ߴٸ� �Ʒ� ���� üũ
		if (m_Owner->m_bUseDrag)
		{
			if (ImGui::BeginDragDropSource())
			{
				// Tree UI �� ������ Drag Node ���� ���
				m_Owner->SetDragNode(this);

				// ù��° ���� ID �� Node �� ������(Tree UI) (ex Outliner, Content)
				// �� ���� �����ϰ��� �ϴ� Data �� ���� �ּҿ� ũ�⸦ ���ڷ�
				ImGui::SetDragDropPayload(m_Owner->GetParentUI()->GetName().c_str(), &m_Data, sizeof(DWORD_PTR));

				// PayLoad ��, EndDragDrop ���� ���� Text �� ToolTip ���� ����ȴ�.
				ImGui::Text(m_Name.c_str());

				ImGui::EndDragDropSource();
			}
		}

		// Node �� ������(Tree UI) �� Drop ��ɵ� ���� ����ϱ�� �ߴٸ� �Ʒ� ���� üũ
		if (m_Owner->m_bUseDrag && m_Owner->m_bUseDrop)
		{
			if (ImGui::BeginDragDropTarget())
			{
				// �ڽ��� Tree �κ��� ����� ��ü���� üũ
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_Owner->GetParentUI()->GetName().c_str());

				// ���� ������� �ùٸ� ��ü���
				if (payload != nullptr)
				{
					// Tree UI �� ������ Drop Node ���� ���
					m_Owner->SetDropNode(this);
				}

				ImGui::EndDragDropTarget();
			}
		}

		// �ش� Node ������ ���콺 Ŭ�� ������ �߻��ϸ� �ش� ��带 ���õ� ���·� �����.
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			// �����ִ� �θ� Tree Ŭ�� �� �Ʒ� �Լ� ȣ���� �ȵ�
			if (ImGui::IsItemHovered())
			{
				// Frame Node �� ����
				if (!m_bFrame)
					m_Owner->SetSelectedNode(this);
			}
		}

		// �ڽ� Node Render
		for (size_t i = 0; i < m_vecChildNode.size(); i++)
		{
			m_vecChildNode[i]->Render_Tick();
		}

		ImGui::TreePop();
	}
}




// ===========
//   Tree UI
// ===========
TreeUI::TreeUI(const string& name)
	: EditorUI(name, "##Tree")
	, m_RootNode(nullptr)
	, m_bShowRoot(false)
	, m_bShowFileName(false)
	, m_SelectedNode(nullptr)
	, m_DragNode(nullptr)
	, m_DropNode(nullptr)
	, m_bUseDrag(false)
	, m_bUseDrop(false)
	, m_SelectedCallBack(nullptr)
	, m_SelectedInst(nullptr)
	, m_SelectedDelegate(nullptr)
	, m_DragDropInst(nullptr)
	, m_DragDropDelegate(nullptr)
{

}

TreeUI::~TreeUI()
{
	if (m_RootNode != nullptr)
	{
		delete m_RootNode;
	}
}

TreeNode* TreeUI::AddTreeNode(TreeNode* parent, const string& nodeName, DWORD_PTR dwData)
{
	TreeNode* pNewNode = new TreeNode(nodeName, dwData);

	// Node �� Owner �� Tree ���� ����
	pNewNode->m_Owner = this;

	// Parent Node �� �������� ������, Root Node �� �߰��ϰڴٴ� �ǹ�
	if (parent == nullptr)
	{
		// ������ �̹� Root Node �� �����Ѵٸ� assert
		assert(!m_RootNode);

		m_RootNode = pNewNode;
	}
	else
	{
		parent->AddChildNode(pNewNode);
	}

	return pNewNode;
}

void TreeUI::Render_Tick()
{
	// Tree �� Node �� �ϳ��� ���� ���
	if (m_RootNode == nullptr)
		return;

	if (m_bShowRoot)
	{
		m_RootNode->Render_Tick();
	}
	else
	{
		const vector<TreeNode*>& vecChildNode = m_RootNode->GetChildNode();

		for (size_t i = 0; i < vecChildNode.size(); i++)
		{
			vecChildNode[i]->Render_Tick();
		}
	}

	if (m_DragNode && (m_DropNode || ImGui::IsMouseReleased(ImGuiMouseButton_Left)))
	{
		// ȣ���� Instance �� Delegate �� �����ؾ���
		if (m_DragDropInst && m_DragDropDelegate)
		{
			(m_DragDropInst->*m_DragDropDelegate)((DWORD_PTR)m_DragNode, (DWORD_PTR)m_DropNode);
		}
			

		m_DragNode = nullptr;
		m_DropNode = nullptr;
	}
}

void TreeUI::SetSelectedNode(TreeNode* node)
{
	// ������ ���õ� Node �� ������ �ش� Node �� ����
	if (m_SelectedNode)
		m_SelectedNode->m_bSelected = false;

	m_SelectedNode = node;

	// ���� ���õ� Node �� Selected �缳��
	if (m_SelectedNode)
	{
		m_SelectedNode->m_bSelected = true;

		// ��ϵǾ��ִ� Call Back �̳�,
		if (m_SelectedCallBack)
		{
			m_SelectedCallBack();
		}

		// Delegate �� ������ ȣ��
		if (m_SelectedInst && m_SelectedDelegate)
		{
			(m_SelectedInst->*m_SelectedDelegate)((DWORD_PTR)m_SelectedNode);
		}
	}
}