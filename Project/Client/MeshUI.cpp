#include "pch.h"
#include "MeshUI.h"

MeshUI::MeshUI()
	: AssetUI("MeshUI", "##MeshUI", ASSET_TYPE::MESH)
{

}

MeshUI::~MeshUI()
{

}

void MeshUI::Render_Tick()
{
	Render_Title();

	string strName = ToString(GetTarget()->GetKey());

	Ptr<CMesh> pMesh = dynamic_cast<CMesh*>(GetTarget().Get());
	assert(pMesh.Get());

	// Mesh Name
	ImGui::Text("Mesh Name"); SAME_SET(110);
	ImGui::InputText("##MeshNameMeshUI", (char*)strName.c_str(), strName.capacity(), ImGuiInputTextFlags_ReadOnly);

	// Vertex Count
	int VertexCount = pMesh->GetVertexCount();
	ImGui::Text("Vertex Count"); SAME_SET(110);
	ImGui::InputInt("##VertexCountMeshUI", &VertexCount, 0, 100, ImGuiInputTextFlags_ReadOnly);

	// Triangle Count
	int IndexCount = pMesh->GetIndexCount() / 3;
	ImGui::Text("Triangle Count"); SAME_SET(110);
	ImGui::InputInt("##TriangleCountMeshUI", &IndexCount, 0, 100, ImGuiInputTextFlags_ReadOnly);
}