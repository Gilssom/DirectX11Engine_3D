#include "pch.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CGraphicShader.h"

#include "CLevelManager.h"
#include "CLevel.h"

CMaterial::CMaterial(bool bEngine)
	:CAsset(ASSET_TYPE::MATERIAL, bEngine)
	, m_bDynamic(false)
{

}

CMaterial::~CMaterial()
{

}

void CMaterial::SetShader(Ptr<CGraphicShader> shader)
{
	bool bSave = false;

	// 1. 지금 같은 Shader 가 아니고, 2. Engine 전용이 아니며, 3. 동적도 아니고, 
	// 4. 현재 Stop 모드일 경우에만 저장 가능
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();
	if (m_Shader != shader && !IsEngineAsset() && !m_bDynamic && pCurLevel->GetState() == LEVEL_STATE::STOP)
	{
		bSave = true;
	}

	m_Shader = shader;

	// 변경점이 생기면 새로 저장
	if (bSave)
	{
		Save(CPathManager::GetInst()->GetContentPath() + GetRelativePath());
	}
}

void CMaterial::SetTexParam(TEX_PARAM param, Ptr<CTexture> tex)
{
	m_arrTex[param] = tex;
}

void* CMaterial::GetScalarParam(SCALAR_PARAM param)
{
	switch (param)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		return &m_Const.iArr[param];
		break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		return &m_Const.fArr[param - FLOAT_0];
		break;
	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		return &m_Const.v2Arr[param - VEC2_0];
		break;
	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		return &m_Const.v4Arr[param - VEC4_0];
		break;
	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
		return &m_Const.matArr[param - MAT_0];
		break;
	}

	return nullptr;
}

Ptr<CMaterial> CMaterial::GetDynamicMaterial()
{
	// 본인을 이용해서(복사) 새로운 재질 생성 (Clone 복제 사용)
	Ptr<CMaterial> pDynamicMtrl = Clone();
	pDynamicMtrl->m_bDynamic = true;

	return pDynamicMtrl;
}

void CMaterial::Binding()
{
	// 텍스처 바인딩 - 상수 데이터에 Texture Binding 여부까지 포함
	for (UINT i = 0; i < TEX_PARAM::END; i++)
	{
		if (m_arrTex[i] == nullptr)
		{
			CTexture::Clear(i);
			m_Const.btex[i] = false;
			continue;
		}

		m_arrTex[i]->Binding(i);
		m_Const.btex[i] = true;
	}

	// 상수 데이터 바인딩
	if (m_Shader.Get() != nullptr)
		m_Shader->Binding();

	CConstBuffer* pMtrlCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pMtrlCB->SetData(&m_Const);
	pMtrlCB->Binding();
}

int CMaterial::Save(const wstring& FilePath)
{
	// 동적 재질 및 Engine 용 재질은 저장이 될 수 없다.
	assert(!m_bDynamic && !IsEngineAsset());

	SetRelativePath(CPathManager::GetInst()->GetRelativePath(FilePath));

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, FilePath.c_str(), L"wb");

	// 상수 데이터 저장
	fwrite(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// 참조하고 있던 Shader 저장 - Asset의 Key 값과 Relative Path 모두 함께 저장.
	SaveAssetRef(m_Shader, pFile);

	// Texture 저장
	for (UINT i = 0; i < (UINT)TEX_PARAM::END; i++)
	{
		SaveAssetRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}

int CMaterial::Load(const wstring& FilePath)
{
	// 동적 재질 및 Engine 용 재질은 불러올 수 없다.
	assert(!m_bDynamic && !IsEngineAsset());

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, FilePath.c_str(), L"rb");
	
	// 경로가 잘못되었거나 해당 Asset 이 없을 경우
	if (pFile == nullptr)
	{
		return E_FAIL;
	}

	// 상수 데이터 로드
	fread(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// 저장해놓은 참조정보 로드
	LoadAssetRef(m_Shader, pFile);

	// Texture 로드
	for (UINT i = 0; i < (UINT)TEX_PARAM::END; i++)
	{
		LoadAssetRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}
