#include "pch.h"
#include "ComputeShaderUI.h"

#include <Engine\\CComputeShader.h>

ComputeShaderUI::ComputeShaderUI()
	: AssetUI("ComputeShaderUI", "##ComputeShaderUI", ASSET_TYPE::COMPUTE_SHADER)
{

}

ComputeShaderUI::~ComputeShaderUI()
{

}

void ComputeShaderUI::Render_Tick()
{
	Render_Title();

	Ptr<CComputeShader> pComputeShader = dynamic_cast<CComputeShader*>(GetTarget().Get());
	assert(pComputeShader.Get());

	// Texture ÀÌ¸§
	ImGui::Text("Name"); SAME;
	string strComputeShaderKey = ToString(pComputeShader->GetKey());
	ImGui::InputText("##ComputeUIName", (char*)strComputeShaderKey.c_str(), strComputeShaderKey.capacity(), ImGuiInputTextFlags_ReadOnly);
}