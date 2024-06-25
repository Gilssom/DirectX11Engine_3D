#include "pch.h"
#include "ContentUI.h"

#include <Engine\\CAssetManager.h>
#include <Engine\\CPathManager.h>

#include "Inspector.h"
#include "TreeUI.h"

ContentUI::ContentUI()
	: EditorUI("Content", "##Content")
{
	// �ڽ� UI �� Tree UI ����
	m_Tree = new TreeUI("Content Tree");
	AddChildUI(m_Tree);

	// Tree UI Root Node ������ �ʰ� ����
	m_Tree->ShowRoot(false);

	// Tree UI �� Drag ��� On
	m_Tree->UseDrag(true);

	// Node �׸��� File Name �� ���̰� ����
	m_Tree->ShowFileNameOnly(true);

	// �׸� ���� �� ������ CallBack or Delegate ���
	m_Tree->RegisterSelectDelegate(this, (UI_DELEGATE1)&ContentUI::SelectAsset);

	// Content ������ �ִ� ��� Asset �� �޸𸮿� �ε�
	ReloadContent();

	// ���� Asset ���µ鿡 ���� Data Tree ����
	RenewContent();
}

ContentUI::~ContentUI()
{

}

void ContentUI::Render_Tick()
{
	if (CAssetManager::GetInst()->IsAssetChanged())
	{
		RenewContent();
	}
}

void ContentUI::RenewContent()
{
	// ���� Data �������� ������ �� ������ Tree �� ��ü�ϰ� ���ο� Data �� Tree �� �����
	m_Tree->Clear();

	// Dummy Root Node
	TreeNode* pRootNode = m_Tree->AddTreeNode(nullptr, "Dummy Root");

	// Asset Manager �� ���� Asset �� ������ �޾ƿͼ� Tree �� �ݿ�
	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; i++)
	{
		// Category Setting
		TreeNode* pCategoryNode = m_Tree->AddTreeNode(pRootNode, ASSET_TYPE_STRING[i]);
		pCategoryNode->SetFrame(true);

		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetManager::GetInst()->GetAssets((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			m_Tree->AddTreeNode(pCategoryNode, ToString(pair.first).c_str(), (DWORD_PTR)pair.second.Get());
		}
	}
}

void ContentUI::ReloadContent()
{
	// �̹� �ִ� Asset ���� �ٽ� Loading ���� �ʴ´�.

	// Content ���� ������ �����ϴ� ��� ���ϸ��� �˾Ƴ���.
	FindFileName(CPathManager::GetInst()->GetContentPath());

	// �˾Ƴ� ������ ����θ��� ���ؼ�, �ش� Asset �� �ε�
	for (size_t i = 0; i < m_vecContentName.size(); i++)
	{
		ASSET_TYPE type = GetAssetTypeFromExt(m_vecContentName[i]);

		switch (type)
		{
		case ASSET_TYPE::PREFAB:
			CAssetManager::GetInst()->Load<CPrefab>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::MESH:
			CAssetManager::GetInst()->Load<CMesh>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::MESH_DATA:
			//CAssetManager::GetInst()->Load<CMeshData>(m_vecContentName[i], m_vecContentName[i]); _�̱���
			break;
		case ASSET_TYPE::MATERIAL:
			CAssetManager::GetInst()->Load<CMaterial>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::TEXTURE:
			CAssetManager::GetInst()->Load<CTexture>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::SOUND:
			CAssetManager::GetInst()->Load<CSound>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case ASSET_TYPE::END:
			break;
		}
	}

	// Content �������� �ִ� Asset �� ����� ���� ���� (��� ��)
	m_vecContentName.clear();

	// �ε��� Asset �� ���� ���� ���� ���θ� üũ
	wstring strContent = CPathManager::GetInst()->GetContentPath();

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; i++)
	{
		// Shader �� ���� ����
		if ((UINT)ASSET_TYPE::GRAPHICS_SHADER == i || (UINT)ASSET_TYPE::COMPUTE_SHADER == i)
		{
			continue;
		}

		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetManager::GetInst()->GetAssets((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			// Engine �� Asset �� ���� ������ �������� �ʴ´�.
			if (pair.second->IsEngineAsset())
				continue;

			// �ش� ������ �����ϴ��� üũ
			if (!exists(strContent + pair.second->GetRelativePath()))
			{
				DeleteAsset(pair.second);
			}
		}
	}
}

void ContentUI::FindFileName(const wstring& strFolderPath)
{
	// *.* : ���� ��ü ,  *.mtrl : mtrl ���ϸ�
	wstring strFindPath = strFolderPath + L"*.*";

	// Ž�� Flag
	WIN32_FIND_DATA FindData = {};

	// ���� Ž���� - ������ ���� �� ���͸��� �ش��ϴ� ������ Ž���ϴ� ��ü(�ڵ鰪)
	HANDLE hFind = FindFirstFile(strFindPath.c_str(), &FindData);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		MessageBox(nullptr, strFindPath.c_str(), L"���� Ž�� ����", MB_OK);
	}

	// �ش� ������ ���̻� ���� �� ������ �������� ���������� Ž��
	while (FindNextFile(hFind, &FindData))
	{
		// ���� �ش� Data �� ���� ����(DIRECTORY) �̶�� ��� Ž��
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// ���� �ش� ������ .. (���� ���� �̵�) �̶�� continue
			if (!wcscmp(FindData.cFileName, L".."))
				continue;

			FindFileName(strFolderPath + FindData.cFileName + L"\\");
		}
		// �ش� Data �� ���� ���� �̶�� vector �� ������ ����� ����
		else
		{
			wstring RelativePath = CPathManager::GetInst()->GetRelativePath(strFolderPath + FindData.cFileName);
			m_vecContentName.push_back(RelativePath);
		}
	}

	FindClose(hFind);
}

UINT ContentUI::SelectAsset(DWORD_PTR data)
{
	TreeNode* pSelectedNode = (TreeNode*)data;

	pSelectedNode->GetName();
	Ptr<CAsset> pAsset = (CAsset*)pSelectedNode->GetData();

	Inspector* pInspector = CImGuiManager::GetInst()->FindEditorUI<Inspector>("Inspector");

	pInspector->SetTargetAsset(pAsset);

	return 0;
}

ASSET_TYPE ContentUI::GetAssetTypeFromExt(const path& assetName)
{
	path ext = assetName.extension();

	if (ext == L".pref" || ext == L".PREF")
	{
		return ASSET_TYPE::PREFAB;
	}

	else if (ext == L".png" || ext == L".PNG" ||
			 ext == L".jpg" || ext == L".JPG" ||
			 ext == L".bmp" || ext == L".BMP" ||
			 ext == L".tga" || ext == L".TGA" ||
			 ext == L".dds" || ext == L".DDS")
	{
		return ASSET_TYPE::TEXTURE;
	}

	else if (ext == L".mesh" || ext == L".MESH")
	{
		return ASSET_TYPE::MESH;
	}

	else if (ext == L".mdat" || ext == L".MDAT")
	{
		return ASSET_TYPE::MESH_DATA;
	}

	else if (ext == L".mtrl" || ext == L".MTRL")
	{
		return ASSET_TYPE::MATERIAL;
	}

	else if (ext == L".mp3" || ext == L".MP3" ||
			 ext == L".wav" || ext == L".WAV" ||
			 ext == L".ogg" || ext == L".OGG")
	{
		return ASSET_TYPE::SOUND;
	}

	return ASSET_TYPE::END;
}
