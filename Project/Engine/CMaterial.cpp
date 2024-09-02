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

	// 1. ���� ���� Shader �� �ƴϰ�, 2. Engine ������ �ƴϸ�, 3. ������ �ƴϰ�, 
	// 4. ���� Stop ����� ��쿡�� ���� ����
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurrentLevel();

	if (m_Shader != shader && !IsEngineAsset() && !m_bDynamic && (!pCurLevel || pCurLevel->GetState() == LEVEL_STATE::STOP))
	{
		bSave = true;
	}

	m_Shader = shader;

	// �������� ����� ���� ����
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
	// ������ �̿��ؼ�(����) ���ο� ���� ���� (Clone ���� ���)
	Ptr<CMaterial> pDynamicMtrl = Clone();
	pDynamicMtrl->m_bDynamic = true;

	return pDynamicMtrl;
}

void CMaterial::Binding()
{
	// �ؽ�ó ���ε� - ��� �����Ϳ� Texture Binding ���α��� ����
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

	// ��� ������ ���ε�
	if (m_Shader.Get() != nullptr)
		m_Shader->Binding();

	CConstBuffer* pMtrlCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pMtrlCB->SetData(&m_Const);
	pMtrlCB->Binding();
}

void CMaterial::Binding_Inst()
{
	// �ؽ��� ���ε�
	for (UINT i = 0; i < TEX_PARAM::END; ++i)
	{
		if (nullptr == m_arrTex[i])
		{
			CTexture::Clear(i);
			m_Const.btex[i] = false;
			continue;
		}

		m_arrTex[i]->Binding(i);
		m_Const.btex[i] = true;
	}


	// ��� ������ ���ε�
	if (nullptr != m_Shader.Get())
		m_Shader->Binding_Inst();

	CConstBuffer* pMtrlCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pMtrlCB->SetData(&m_Const);
	pMtrlCB->Binding();
}

int CMaterial::Save(const wstring& FilePath)
{
	// ���� ���� �� Engine �� ������ ������ �� �� ����.
	assert(!m_bDynamic && !IsEngineAsset());

	SetRelativePath(CPathManager::GetInst()->GetRelativePath(FilePath));

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, FilePath.c_str(), L"wb");

	// ��� ������ ����
	fwrite(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// �����ϰ� �ִ� Shader ���� - Asset�� Key ���� Relative Path ��� �Բ� ����.
	SaveAssetRef(m_Shader, pFile);

	// Texture ����
	for (UINT i = 0; i < (UINT)TEX_PARAM::END; i++)
	{
		SaveAssetRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}

int CMaterial::Load(const wstring& FilePath)
{
	// ���� ���� �� Engine �� ������ �ҷ��� �� ����.
	assert(!m_bDynamic && !IsEngineAsset());

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, FilePath.c_str(), L"rb");
	
	// ��ΰ� �߸��Ǿ��ų� �ش� Asset �� ���� ���
	if (pFile == nullptr)
	{
		return E_FAIL;
	}

	// ��� ������ �ε�
	fread(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// �����س��� �������� �ε�
	LoadAssetRef(m_Shader, pFile);

	// Texture �ε�
	for (UINT i = 0; i < (UINT)TEX_PARAM::END; i++)
	{
		LoadAssetRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}
