#include "pch.h"
#include "Collider2DUI.h"

#include <Engine\\CCollider2D.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>

Collider2DUI::Collider2DUI()
	: ComponentUI("Collider2D", "##Collider2DUI", COMPONENT_TYPE::COLLIDER2D)
{
	SIZE;
}

Collider2DUI::~Collider2DUI()
{
}

void Collider2DUI::Render_Tick()
{
	Render_Title();

	CCollider2D* pCollider = GetTarget()->Collider2D();
	assert(pCollider);

	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

	if (pCurLevel == nullptr)
		return;

	// 임시삭제
	if (ImGui::Button("Destroy Object##GameObject"))
	{
		CLayer* pLayer = pCurLevel->GetLayer(GetTarget()->GetLayerIdx());
		pLayer->DeRegisterParentObject(GetTarget());
		return;
	}

	Vec3 vOffsetPos = pCollider->GetOffset();
	Vec3 vScale = pCollider->GetScale();
	bool bAbsolute = pCollider->IsAbsolute();
	int  iLayer = GetTarget()->GetLayerIdx();

	float ScaleDragSpeed = 1.f;
	if (!bAbsolute)
	{
		ScaleDragSpeed = 0.01f;
	}

	ImGui::Text("Offset Position"); SAME;
	if (ImGui::DragFloat3("##Collider2DOffsetPos", vOffsetPos, ScaleDragSpeed))
		pCollider->SetOffset(vOffsetPos);

	ImGui::Text("Scale          "); SAME;
	if (ImGui::DragFloat3("##Collider2DScale", vScale, ScaleDragSpeed))
		pCollider->SetScale(vScale);

	ImGui::Text("Is Absolute    "); SAME;
	if (ImGui::Checkbox("##Collider2DAbsolute", &bAbsolute))
	{
		pCollider->SetAbsolute(bAbsolute);
	}

	vector<string> vecLayerName;

	for (size_t i = 0; i < MAX_LAYER; i++)
	{
		string curLayerName = ToString(pCurLevel->GetLayer(i)->GetName());
		string strLayerName;
		char buffer[256] = {};
		sprintf_s(buffer, 256, "Layer Index %d : ", i);
		strLayerName = buffer;

		if (!curLayerName.empty())
		{
			strLayerName += curLayerName;
			vecLayerName.push_back(strLayerName);
		}
		else
		{
			strLayerName += "null";
			vecLayerName.push_back(strLayerName);
		}
	}

	string strCurLayerName;
	char buffer[256] = {};
	sprintf_s(buffer, 256, "Layer Index %d : ", iLayer);
	strCurLayerName = buffer;
	strCurLayerName += ToString(pCurLevel->GetLayer(iLayer)->GetName());

	CLayer* pCurLayer = pCurLevel->GetLayer(GetTarget()->GetLayerIdx());

	ImGui::Text("Layer          "); SAME;
	if (ImGui::BeginCombo("##LayerSelect", strCurLayerName.c_str(), 0))
	{
		for (size_t i = 0; i < MAX_LAYER; i++)
		{
			const bool is_selected = iLayer == i;
			if (ImGui::Selectable(vecLayerName[i].c_str(), is_selected))
			{
				pCurLayer->DeRegisterParentObject(GetTarget());

				GetTarget()->ChangeLayerIdx(i);
				CLayer* pNewLayer = pCurLevel->GetLayer(i);
				pNewLayer->AddObject(GetTarget(), true);
			}
		}

		ImGui::EndCombo();
	}
}