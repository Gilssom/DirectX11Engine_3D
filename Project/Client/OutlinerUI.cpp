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
	// 자식 UI 로 Tree UI 생성
	m_Tree = new TreeUI("Outliner Tree");
	AddChildUI(m_Tree);

	// Tree UI Root Node 보이지 않게 설정
	m_Tree->ShowRoot(false);

	// Tree UI 의 Drag & Drop 기능 On
	m_Tree->UseDragDrop(true);

	// Drag & Drop 시 동작할 Delegate 등록
	m_Tree->RegisterDragDropDelegate(this, (UI_DELEGATE2)&OutlinerUI::DragDropFunc);

	// 항목 선택 시 동작할 CallBack or Delegate 등록
	m_Tree->RegisterSelectDelegate(this, (UI_DELEGATE1)&OutlinerUI::SelectGameObject);

	// 현재 Asset 상태들에 따라 Data Tree 설정
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

	// 현재 Level 을 알아낸다.
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

	// 만약 해당 Object 가 자식 Object 를 보유하고 있다면
	const vector<CGameObject*>& vecChildren = object->GetChildren();

	// 자식 Object 를 부모 Object 의 정보로 만들어진 노드 밑으로 다시 추가 (재귀 형식)
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

// Tree 에서 Drag & Drop 이 발생하면 호출되는 함수(Delegate)
UINT OutlinerUI::DragDropFunc(DWORD_PTR dragNode, DWORD_PTR dropNode)
{
	TreeNode* pDragNode = (TreeNode*)dragNode;
	TreeNode* pDropNode = (TreeNode*)dropNode;

	CGameObject* pDragObject = (CGameObject*)(pDragNode->GetData());
	CGameObject* pDropObject = pDropNode ? (CGameObject*)pDropNode->GetData() : nullptr;

	if (pDropObject)
	{
		// Drag Object 가 Drop Object 의 Parent 관계면 안된다.
		if (pDropObject->IsAncestor(pDragObject) == false)
		{
			// Drag Object 가 Drop Object 의 자식으로 들어가야함
			// 보통의 Add Event 는 Task Manager 를 통해서 다음 Frame 에 적용되도록 해야 하는데,
			// 우리의 프레임워크 특성상, Editor - UI 는 게임의 한 프레임이 지난 이후 처리가 되기 때문에
			// 굳이 Task 를 신경안쓰고 활용해도 무방하다. (사실상 지금이 Task Manager 단계)
			pDropObject->AddChild(pDragObject);
		}
	}

	// 드랍된 곳이 허공이면,
	else
	{
		// 만약 Drag 한 Object 가 자식 오브젝트라면
		if (pDragObject->GetParent() != nullptr)
		{
			// 부모로부터 나와서, 최상위 오브젝트가 되어야 한다.
			pDragObject->DisconnectWithParent();
			pDragObject->RegisterAsParentObject();
		}
	}

	// Obejct 의 계층 구조(부모-자식) 가 변경이 되었기 때문에
	// Outliner 의 Tree 를 재구성한다.
	RenewGameObject();

	return 0;
}
