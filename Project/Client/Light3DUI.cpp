#include "pch.h"
#include "Light3DUI.h"

#include <Engine\\CLight3D.h>

Light3DUI::Light3DUI()
	: ComponentUI("Light3D", "##Light3DUI", COMPONENT_TYPE::LIGHT3D)
{
	SIZE;
}

Light3DUI::~Light3DUI()
{
}

void Light3DUI::Render_Tick()
{
	Render_Title();

	CLight3D* pLight3D = GetTarget()->Light3D();
	const tLightInfo& Info = pLight3D->GetLightInfo();

	// Light Type : Directional , Point , Spot
	ImGui::Text("Light Type"); SAME;
	const char* arrLightType[] = { "Directional", "Point", "Spot" };

	if (ImGui::BeginCombo("##Light3DType", arrLightType[Info.LightType], 0))
	{
		for (int i = 0; i < 3; i++)
		{
			const bool is_selected = Info.LightType == i;
			if (ImGui::Selectable(arrLightType[i], is_selected))
			{
				pLight3D->SetLightType((LIGHT_TYPE)i);
			}
		}

		ImGui::EndCombo();
	}

	// Light Color
	ImGui::Text("Light Color"); SAME;
	ImGui::ColorEdit3("##Light3D Color", (float*)&Info.Light.vDiffuse, 0);

	// Light Ambient
	ImGui::Text("Light Ambient"); SAME;
	ImGui::ColorEdit3("##Light3D Ambient", (float*)&Info.Light.vAmbient, 0);

	// Range : Point or Spot Light
	bool IsDirLight = Info.LightType == (UINT)LIGHT_TYPE::DIRECTIONAL;
	float fRange = Info.Range;
	ImGui::BeginDisabled(IsDirLight);

	ImGui::Text("Light Range"); SAME;
	if (ImGui::DragFloat("##Light Range", &fRange, 0.5f, 0.f));
	{
		pLight3D->SetRange(fRange);
	}

	ImGui::EndDisabled();

	// Angle : Only Spot Light
	bool IsSpotLight = Info.LightType == (UINT)LIGHT_TYPE::SPOT;
	float fAngle = Info.Angle;
	ImGui::BeginDisabled(!IsSpotLight);

	ImGui::Text("Spot Light Angle"); SAME;
	if (ImGui::DragFloat("##Spot Light Angle", &fAngle, 0.1f, 0.f, 180.f))
	{
		pLight3D->SetAngle(fAngle);
	}

	ImGui::EndDisabled();
}