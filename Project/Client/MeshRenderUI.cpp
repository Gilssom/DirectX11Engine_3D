#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine\CAsset.h>
#include <Engine\CMeshRender.h>

#include "CImGuiManager.h"
#include "inspector.h"
#include "ListUI.h"

MeshRenderUI::MeshRenderUI()
	: ComponentUI("MeshRender", "##MeshRenderUI", COMPONENT_TYPE::MESHRENDER)
{
	SIZE;
}

MeshRenderUI::~MeshRenderUI()
{
}

void MeshRenderUI::Render_Tick()
{
	Render_Title();

	CMeshRender* pMeshRender = GetTarget()->MeshRender();
	Ptr<CMesh> pMesh = pMeshRender->GetMesh();
	Ptr<CMaterial> pMaterial = pMeshRender->GetMaterial();

	// ImGuiInputTextFlags_ : ImGui �� Flag �� ��� _ �� ���̰�, ������ �� �� �ִ�. (��Ʈ)
	string strMeshKey;
	if (pMesh != nullptr)
	{
		strMeshKey = string(pMesh->GetKey().begin(), pMesh->GetKey().end());
	}

	ImGui::Text("Mesh      "); SAME;
	ImGui::PushItemWidth(200.f);
	ImGui::InputText("##MeshName", (char*)strMeshKey.c_str(), strMeshKey.capacity(), ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	ImGui::PopItemWidth();

	if (ImGui::BeginDragDropTarget())
	{
		// Content �κ��� ����� ��ü���� üũ
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content");

		// ���� ������� �ùٸ� ��ü���
		if (payload != nullptr)
		{
			// payload �� ����� Data �� �츮 ������� �����;� �Ѵ�.
			DWORD_PTR dwData = 0;
			memcpy(&dwData, payload->Data, payload->DataSize);

			// �ش� ��ü�� Type �� �ڽ��� Type �� ��
			Ptr<CAsset> pAsset = (CAsset*)dwData;
			if (pAsset->GetAssetType() == ASSET_TYPE::MESH)
			{
				pMeshRender->SetMesh((CMesh*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (ImGui::Button("##MeshButton", ImVec2(24, 24)))
	{
		// List UI Ȱ��ȭ
		ListUI* pListUI = CImGuiManager::GetInst()->FindEditorUI<ListUI>("List");

		if (pListUI != nullptr && !pListUI->IsActive())
		{
			pListUI->SetModal(true);
			pListUI->SetActive(true);
			pListUI->SetFocus();

			vector<string> vecNames;
			CAssetManager::GetInst()->GetAssetNames(ASSET_TYPE::MESH, vecNames);
			pListUI->AddItem(vecNames);

			UI_DELEGATE1 dele = (UI_DELEGATE1)&MeshRenderUI::SelectMesh;
			pListUI->RegisterDoubleClickDelegate(this, dele);
		}
	}

	string strMaterialKey;
	if (pMaterial != nullptr)
	{
		strMaterialKey = string(pMaterial->GetKey().begin(), pMaterial->GetKey().end());
	}

	ImGui::Text("Material  "); SAME;
	ImGui::PushItemWidth(200.f);
	ImGui::InputText("##MaterialName", (char*)strMaterialKey.c_str(), strMaterialKey.capacity(), ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	ImGui::PopItemWidth();

	if (ImGui::BeginDragDropTarget())
	{
		// Content �κ��� ����� ��ü���� üũ
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content");

		// ���� ������� �ùٸ� ��ü���
		if (payload != nullptr)
		{
			// payload �� ����� Data �� �츮 ������� �����;� �Ѵ�.
			DWORD_PTR dwData = 0;
			memcpy(&dwData, payload->Data, payload->DataSize);

			// �ش� ��ü�� Type �� �ڽ��� Type �� ��
			Ptr<CAsset> pAsset = (CAsset*)dwData;
			if (pAsset->GetAssetType() == ASSET_TYPE::MATERIAL)
			{
				pMeshRender->SetMaterial((CMaterial*)pAsset.Get());
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (ImGui::Button("##MaterialButton", ImVec2(24, 24)))
	{
		// List UI Ȱ��ȭ
		ListUI* pListUI = CImGuiManager::GetInst()->FindEditorUI<ListUI>("List");

		if (pListUI != nullptr && !pListUI->IsActive())
		{
			pListUI->SetModal(true);
			pListUI->SetActive(true);
			pListUI->SetFocus();

			vector<string> vecNames;
			CAssetManager::GetInst()->GetAssetNames(ASSET_TYPE::MATERIAL, vecNames);
			pListUI->AddItem(vecNames);
			pListUI->RegisterDoubleClickDelegate(this, (UI_DELEGATE1)&MeshRenderUI::SelectMaterial);
		}
	}
}

UINT MeshRenderUI::SelectMesh(DWORD_PTR selected)
{
	string* pStr = (string*)selected;

	wstring strMeshKey = ToWString(*pStr);

	Ptr<CMesh> pMesh = CAssetManager::GetInst()->FindAsset<CMesh>(strMeshKey);

	assert(GetTarget() != nullptr && pMesh != nullptr);

	GetTarget()->GetRenderComponent()->SetMesh(pMesh);

	SetFocus();

	return 0;
}

UINT MeshRenderUI::SelectMaterial(DWORD_PTR selected)
{
	string* pStr = (string*)selected;

	wstring strMaterialKey = ToWString(*pStr);

	Ptr<CMaterial> pMaterial = CAssetManager::GetInst()->FindAsset<CMaterial>(strMaterialKey);

	assert(GetTarget() != nullptr && pMaterial != nullptr);

	GetTarget()->GetRenderComponent()->SetMaterial(pMaterial);

	SetFocus();

	return 0;
}
