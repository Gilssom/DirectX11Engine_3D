#include "pch.h"
#include "TransformUI.h"

#include <Engine\\CTransform.h>

TransformUI::TransformUI()
	: ComponentUI("Transform", "##TransformUI", COMPONENT_TYPE::TRANSFORM)
{
	//SIZE_SET(1);
}

TransformUI::~TransformUI()
{
}

void TransformUI::Render_Tick()
{
	// Title 출력
	Render_Title();

	if (GetTarget() == nullptr)
		return;

	ImGui::SeparatorText("Object Name Change");
	char buffer[256];
	string name = ToString(GetTarget()->GetName());
	strncpy_s(buffer, name.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';

	ImGui::Text("Object Name"); SAME;
	if (ImGui::InputText("##Object Name", buffer, sizeof(buffer)))
	{
		wstring newName = ToWString(buffer);
		GetTarget()->ChangeName(newName);
	}

	ImGui::Text("");

	ImGui::SeparatorText("Object Transform");
	// 해당 UI에 출력할 정보 입력
	// Vector3 에 Vector 의 포인터를 넘겨주는 operator 가 구현이 되어 있음
	Vec3 vPos = GetTarget()->Transform()->GetRelativePos();
	Vec3 vScale = GetTarget()->Transform()->GetRelativeScale();
	Vec3 vRotation = GetTarget()->Transform()->GetRelativeRotation();

	ImGui::Text("Position"); SAME;
	if (ImGui::DragFloat3("##Position", vPos, 0.1f))
		GetTarget()->Transform()->SetRelativePos(vPos);

	ImGui::Text("Scale"); SAME;
	if (ImGui::DragFloat3("##Scale", vScale, 0.1f))
		GetTarget()->Transform()->SetRelativeScale(vScale);

	ImGui::Text("Rotation"); SAME;
	vRotation = (vRotation / XM_PI) * 180.f;
	if (ImGui::DragFloat3("##Rotation", vRotation, 0.1f))
	{
		vRotation = (vRotation / 180.f) * XM_PI;
		GetTarget()->Transform()->SetRelativeRotation(vRotation);
	}

	bool Absolute = GetTarget()->Transform()->IsAbsolute();
	ImGui::Text("Is Absolute"); SAME;
	if (ImGui::Checkbox("##Absolute", &Absolute))
	{
		GetTarget()->Transform()->SetAbsolute(Absolute);
	}
}