#include "pch.h"
#include "OutlinerUI.h"

#include <Engine\\CLevelManager.h>
#include <Engine\\CLevel.h>
#include <Engine\\CLayer.h>
#include <Engine\\CGameObject.h>
#include <Engine\\CTaskManager.h>

#include "Inspector.h"
#include "TreeUI.h"

OutlinerUI::OutlinerUI()
	: EditorUI("Outliner", "##Outliner")
{
	// �ڽ� UI �� Tree UI ����
	m_Tree = new TreeUI("Outliner Tree");
	AddChildUI(m_Tree);

	// Tree UI Root Node ������ �ʰ� ����
	m_Tree->ShowRoot(false);

	// Tree UI �� Drag & Drop ��� On
	m_Tree->UseDragDrop(true);

	// Drag & Drop �� ������ Delegate ���
	m_Tree->RegisterDragDropDelegate(this, (UI_DELEGATE2)&OutlinerUI::DragDropFunc);

	// �׸� ���� �� ������ CallBack or Delegate ���
	m_Tree->RegisterSelectDelegate(this, (UI_DELEGATE1)&OutlinerUI::SelectGameObject);

	// ���� Asset ���µ鿡 ���� Data Tree ����
	RenewGameObject();
}

OutlinerUI::~OutlinerUI()
{

}

void OutlinerUI::Render_Tick()
{
	if (CTaskManager::GetInst()->IsObjectsChanged())
	{
		RenewGameObject();
	}
}

void OutlinerUI::RenewGameObject()
{
	// Tree Clear
	m_Tree->Clear();
	TreeNode* pRootNode = m_Tree->AddTreeNode(nullptr, "Dummy Root");

	// ���� Level �� �˾Ƴ���.
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

	if (pCurLevel == nullptr)
		return;

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		CLayer* pLayer = pCurLevel->GetLayer(i);

		const vector<CGameObject*>& vecParent = pLayer->GetParentObjects();

		for (size_t j = 0; j < vecParent.size(); j++)
		{
			AddGameObject(pRootNode, vecParent[j]);
		}
	}
}

void OutlinerUI::AddGameObject(TreeNode* node, CGameObject* object)
{
	TreeNode* pNewNode = m_Tree->AddTreeNode(node, ToString(object->GetName()), (DWORD_PTR)object);

	// ���� �ش� Object �� �ڽ� Object �� �����ϰ� �ִٸ�
	const vector<CGameObject*>& vecChildren = object->GetChildren();

	// �ڽ� Object �� �θ� Object �� ������ ������� ��� ������ �ٽ� �߰� (��� ����)
	for (size_t i = 0; i < vecChildren.size(); i++)
	{
		AddGameObject(pNewNode, vecChildren[i]);
	}
}

UINT OutlinerUI::SelectGameObject(DWORD_PTR data)
{
	TreeNode* pSelectedNode = (TreeNode*)data;

	pSelectedNode->GetName();
	CGameObject* pSelectObject = (CGameObject*)pSelectedNode->GetData();

	Inspector* pInspector = CImGuiManager::GetInst()->FindEditorUI<Inspector>("Inspector");

	pInspector->SetTargetObject(pSelectObject);

	return 0;
}

// Tree ���� Drag & Drop �� �߻��ϸ� ȣ��Ǵ� �Լ�(Delegate)
UINT OutlinerUI::DragDropFunc(DWORD_PTR dragNode, DWORD_PTR dropNode)
{
	TreeNode* pDragNode = (TreeNode*)dragNode;
	TreeNode* pDropNode = (TreeNode*)dropNode;

	CGameObject* pDragObject = (CGameObject*)(pDragNode->GetData());
	CGameObject* pDropObject = pDropNode ? (CGameObject*)pDropNode->GetData() : nullptr;

	if (pDropObject)
	{
		// Drag Object �� Drop Object �� Parent ����� �ȵȴ�.
		if (pDropObject->IsAncestor(pDragObject) == false)
		{
			// Drag Object �� Drop Object �� �ڽ����� ������
			// ������ Add Event �� Task Manager �� ���ؼ� ���� Frame �� ����ǵ��� �ؾ� �ϴµ�,
			// �츮�� �����ӿ�ũ Ư����, Editor - UI �� ������ �� �������� ���� ���� ó���� �Ǳ� ������
			// ���� Task �� �Ű�Ⱦ��� Ȱ���ص� �����ϴ�. (��ǻ� ������ Task Manager �ܰ�)
			pDropObject->AddChild(pDragObject);
		}
	}

	// ����� ���� ����̸�,
	else
	{
		// ���� Drag �� Object �� �ڽ� ������Ʈ���
		if (pDragObject->GetParent() != nullptr)
		{
			// �θ�κ��� ���ͼ�, �ֻ��� ������Ʈ�� �Ǿ�� �Ѵ�.
			pDragObject->DisconnectWithParent();
			pDragObject->RegisterAsParentObject();
		}
	}

	// Obejct �� ���� ����(�θ�-�ڽ�) �� ������ �Ǿ��� ������
	// Outliner �� Tree �� �籸���Ѵ�.
	RenewGameObject();

	return 0;
}
