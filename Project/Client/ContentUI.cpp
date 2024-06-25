#include "pch.h"
#include "ContentUI.h"

#include <Engine\\CAssetManager.h>
#include <Engine\\CPathManager.h>

#include "Inspector.h"
#include "TreeUI.h"

ContentUI::ContentUI()
	: EditorUI("Content", "##Content")
{
	// 자식 UI 로 Tree UI 생성
	m_Tree = new TreeUI("Content Tree");
	AddChildUI(m_Tree);

	// Tree UI Root Node 보이지 않게 설정
	m_Tree->ShowRoot(false);

	// Tree UI 의 Drag 기능 On
	m_Tree->UseDrag(true);

	// Node 항목의 File Name 만 보이게 설정
	m_Tree->ShowFileNameOnly(true);

	// 항목 선택 시 동작할 CallBack or Delegate 등록
	m_Tree->RegisterSelectDelegate(this, (UI_DELEGATE1)&ContentUI::SelectAsset);

	// Content 폴더에 있는 모든 Asset 을 메모리에 로딩
	ReloadContent();

	// 현재 Asset 상태들에 따라 Data Tree 설정
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
	// 무언가 Data 변경점이 생겼을 때 기존의 Tree 를 해체하고 새로운 Data 로 Tree 를 재생성
	m_Tree->Clear();

	// Dummy Root Node
	TreeNode* pRootNode = m_Tree->AddTreeNode(nullptr, "Dummy Root");

	// Asset Manager 로 부터 Asset 의 정보를 받아와서 Tree 에 반영
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
	// 이미 있는 Asset 들은 다시 Loading 하지 않는다.

	// Content 폴도 하위에 존재하는 모든 파일명을 알아낸다.
	FindFileName(CPathManager::GetInst()->GetContentPath());

	// 알아낸 파일의 상대경로명을 통해서, 해당 Asset 을 로딩
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
			//CAssetManager::GetInst()->Load<CMeshData>(m_vecContentName[i], m_vecContentName[i]); _미구현
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

	// Content 폴더내에 있는 Asset 의 상대경로 정보 삭제 (사용 끝)
	m_vecContentName.clear();

	// 로딩된 Asset 과 실제 파일 존재 여부를 체크
	wstring strContent = CPathManager::GetInst()->GetContentPath();

	for (UINT i = 0; i < (UINT)ASSET_TYPE::END; i++)
	{
		// Shader 의 경우는 제외
		if ((UINT)ASSET_TYPE::GRAPHICS_SHADER == i || (UINT)ASSET_TYPE::COMPUTE_SHADER == i)
		{
			continue;
		}

		const map<wstring, Ptr<CAsset>>& mapAsset = CAssetManager::GetInst()->GetAssets((ASSET_TYPE)i);

		for (const auto& pair : mapAsset)
		{
			// Engine 용 Asset 은 원본 파일이 존재하지 않는다.
			if (pair.second->IsEngineAsset())
				continue;

			// 해당 파일이 존재하는지 체크
			if (!exists(strContent + pair.second->GetRelativePath()))
			{
				DeleteAsset(pair.second);
			}
		}
	}
}

void ContentUI::FindFileName(const wstring& strFolderPath)
{
	// *.* : 파일 전체 ,  *.mtrl : mtrl 파일만
	wstring strFindPath = strFolderPath + L"*.*";

	// 탐색 Flag
	WIN32_FIND_DATA FindData = {};

	// 파일 탐색기 - 지정된 폴더 및 필터링에 해당하는 파일을 탐색하는 객체(핸들값)
	HANDLE hFind = FindFirstFile(strFindPath.c_str(), &FindData);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		MessageBox(nullptr, strFindPath.c_str(), L"파일 탐색 실패", MB_OK);
	}

	// 해당 폴더에 더이상 파일 및 폴더가 존재하지 않을때까지 탐색
	while (FindNextFile(hFind, &FindData))
	{
		// 만약 해당 Data 가 폴더 형식(DIRECTORY) 이라면 재귀 탐색
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// 만약 해당 폴더가 .. (상위 폴더 이동) 이라면 continue
			if (!wcscmp(FindData.cFileName, L".."))
				continue;

			FindFileName(strFolderPath + FindData.cFileName + L"\\");
		}
		// 해당 Data 가 파일 형식 이라면 vector 에 파일의 상대경로 저장
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
