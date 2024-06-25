#include "pch.h"
#include "GraphicShaderUI.h"

#include <Engine\\CGraphicShader.h>

GraphicShaderUI::GraphicShaderUI()
	: AssetUI("GraphicShaderUI", "##GraphicShaderUI", ASSET_TYPE::GRAPHICS_SHADER)
{

}

GraphicShaderUI::~GraphicShaderUI()
{

}

void GraphicShaderUI::Render_Tick()
{
	Render_Title();

	Ptr<CGraphicShader> pGraphicShader = dynamic_cast<CGraphicShader*>(GetTarget().Get());
	assert(pGraphicShader.Get());

	// Texture ÀÌ¸§
	ImGui::Text("Name"); SAME;
	string strGraphicShaderKey = ToString(pGraphicShader->GetKey());
	ImGui::InputText("##GraphicUIName", (char*)strGraphicShaderKey.c_str(), strGraphicShaderKey.capacity(), ImGuiInputTextFlags_ReadOnly);
}