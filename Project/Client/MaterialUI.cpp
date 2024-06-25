#include "pch.h"
#include "MaterialUI.h"

#include <Engine\CMaterial.h>
#include <Engine\CGraphicShader.h>

#include "ParamUI.h"

MaterialUI::MaterialUI()
	: AssetUI("MaterialUI", "##MaterialUI", ASSET_TYPE::MATERIAL)
	, m_ParamDescMaxWidth(0)
{

}

MaterialUI::~MaterialUI()
{

}

void MaterialUI::Render_Tick()
{
	Render_Title();

	string strName = ToString(GetTarget()->GetKey());

	Ptr<CMaterial> pMaterial = dynamic_cast<CMaterial*>(GetTarget().Get());
	assert(pMaterial.Get());

	// Mesh Name
	ImGui::Text("Material Name"); SAME_SET(110);
	ImGui::InputText("##MaterialNameMaterialUI", (char*)strName.c_str(), strName.capacity(), ImGuiInputTextFlags_ReadOnly);

	// Shader Info
	string strShaderName = nullptr == pMaterial->GetShader() ? "null" : ToString(pMaterial->GetShader()->GetKey());

	ImGui::Text("Shader Name"); SAME_SET(110);
	ImGui::InputText("##ShaderNameMaterialUI", (char*)strShaderName.c_str(), strShaderName.capacity(), ImGuiInputTextFlags_ReadOnly);

	// Drag Drop
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content");

		if (nullptr != payload)
		{
			DWORD_PTR dwData = 0;
			memcpy(&dwData, payload->Data, payload->DataSize);

			Ptr<CAsset> pAsset = (CAsset*)dwData;
			if (pAsset->GetAssetType() == ASSET_TYPE::GRAPHICS_SHADER)
			{
				Ptr<CGraphicShader> pShader = (CGraphicShader*)pAsset.Get();
				pMaterial->SetShader(pShader);
			}
		}

		ImGui::EndDragDropTarget();
	}

	// Shader 의 내부 수치까지 조절 가능하게 구현할 예정

	// Material 이 참조하는 Shader 가 있다면
	if (pMaterial->GetShader() == nullptr)
		return;

	// Shader 가 요청한 Parameter 목록 확인
	Ptr<CGraphicShader> pShader = pMaterial->GetShader();
	vector<tShaderScalarParam> ScalarParam = pShader->GetScalarParam();

	if(!ScalarParam.empty())
		ImGui::SeparatorText("Shader Parmeter");

	ParamUI::SetDescWidth(m_ParamDescMaxWidth);

	for (size_t i = 0; i < ScalarParam.size(); i++)
	{
		// Parameter 의 Desc 의 최대 가로 길이를 계산한다.
		ImVec2 vDescWidth = ImGui::CalcTextSize(ScalarParam[i].strDesc.c_str());
		if (m_ParamDescMaxWidth < vDescWidth.x)
		{
			m_ParamDescMaxWidth = vDescWidth.x;
		}

		switch (ScalarParam[i].Param)
		{
		case INT_0:
		case INT_1:
		case INT_2:
		case INT_3:
			ParamUI::Param_DragInt(ScalarParam[i].strDesc, (int*)pMaterial->GetScalarParam(ScalarParam[i].Param));
			break;
		case FLOAT_0:
		case FLOAT_1:
		case FLOAT_2:
		case FLOAT_3:
			ParamUI::Param_DragFloat(ScalarParam[i].strDesc, (float*)pMaterial->GetScalarParam(ScalarParam[i].Param), 0.01f);
			break;
		case VEC2_0:
		case VEC2_1:
		case VEC2_2:
		case VEC2_3:
			ParamUI::Param_DragVec2(ScalarParam[i].strDesc, (Vec2*)pMaterial->GetScalarParam(ScalarParam[i].Param), 0.01f);
			break;
		case VEC4_0:
		case VEC4_1:
		case VEC4_2:
		case VEC4_3:
			ParamUI::Param_DragVec4(ScalarParam[i].strDesc, (Vec4*)pMaterial->GetScalarParam(ScalarParam[i].Param), 0.01f);
			break;
		case MAT_0:
		case MAT_1:
		case MAT_2:
		case MAT_3:
			ParamUI::Param_DragMat(ScalarParam[i].strDesc, (Matrix*)pMaterial->GetScalarParam(ScalarParam[i].Param), 0.01f);
			break;
		}
	}


	// Shader 가 요청한 Texture Parameter 목록 확인
	vector<tShaderTexParam> TexParam = pShader->GetTexParam();

	if (!TexParam.empty())
		ImGui::SeparatorText("Texture Parmeter");
	for (size_t i = 0; i < TexParam.size(); i++)
	{
		// Parameter 의 Desc 의 최대 가로 길이를 계산한다.
		ImVec2 vDescWidth = ImGui::CalcTextSize(TexParam[i].strDesc.c_str());
		if (m_ParamDescMaxWidth < vDescWidth.x)
		{
			m_ParamDescMaxWidth = vDescWidth.x;
		}

		// ListUI 가 더블클릭됐을때 호출 될 Delegate 등록
		ParamUI::RegisterTextureDelegate(this, (UI_DELEGATE1)&MaterialUI::SelectTexture);

		// 텍스쳐 파라미터 정보 출력
		if (ParamUI::Param_Texture(TexParam[i].strDesc, pMaterial->GetTexParam(TexParam[i].Param)))
		{
			m_TargetParam = TexParam[i].Param;
		}

		ImGui::Text("");
	}
}

void MaterialUI::TargetChanged()
{
	m_ParamDescMaxWidth = 0;
}

UINT MaterialUI::SelectTexture(DWORD_PTR selected)
{
	string* pStr = (string*)selected;

	wstring strTexKey = ToWString(*pStr);

	Ptr<CTexture> pTexture = CAssetManager::GetInst()->FindAsset<CTexture>(strTexKey);

	Ptr<CMaterial> pMtrl = dynamic_cast<CMaterial*>(GetTarget().Get());

	pMtrl->SetTexParam(m_TargetParam, pTexture);

	SetFocus();

	return 0;
}
