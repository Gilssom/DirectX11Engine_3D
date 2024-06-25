#include "pch.h"
#include "MeshDataUI.h"

MeshDataUI::MeshDataUI()
	: AssetUI("MeshDataUI", "##MeshDataUI", ASSET_TYPE::MESH_DATA)
{

}

MeshDataUI::~MeshDataUI()
{

}

void MeshDataUI::Render_Tick()
{
	Render_Title();
}