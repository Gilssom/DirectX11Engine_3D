#pragma once
#include "CAsset.h"

#include "assets.h"

class CMaterial : public CAsset
{
private:
	Ptr<CGraphicShader>	m_Shader;
	tMtrlConst			m_Const;
	Ptr<CTexture>		m_arrTex[TEX_PARAM::END];

	bool				m_bDynamic;

public:
	void SetShader(Ptr<CGraphicShader> shader);
	Ptr<CGraphicShader> GetShader() { return m_Shader; }

	template<typename T>
	void SetScalarParam(SCALAR_PARAM param, const T& value);
	void SetTexParam(TEX_PARAM param, Ptr<CTexture> tex);

	void SetMaterialCoefficient(Vec4 vDiff, Vec4 vSpec, Vec4 vAmb, Vec4 vEmis)
	{
		m_Const.mtrl.vDiff = vDiff;
		m_Const.mtrl.vAmb = vAmb;
		m_Const.mtrl.vSpec = vSpec;
		m_Const.mtrl.vEmv = vEmis;
	}

	void* GetScalarParam(SCALAR_PARAM param);
	Ptr<CTexture>& GetTexParam(TEX_PARAM texParam) { return m_arrTex[texParam]; }

	bool IsDynamic() { return m_bDynamic; }

	Ptr<CMaterial> GetDynamicMaterial();

	void operator = (const CMaterial& otherMtrl)
	{
		SetName(otherMtrl.GetName());

		m_Const = otherMtrl.m_Const;

		for (UINT i = 0; i < (UINT)TEX_PARAM::END; ++i)
		{
			m_arrTex[i] = otherMtrl.m_arrTex[i];
		}

		m_Shader = otherMtrl.m_Shader;
	}

public:
	void Binding();

	virtual int Load(const wstring& FilePath);
	virtual int Save(const wstring& FilePath);


	CLONE(CMaterial);

public:
	CMaterial(bool bEngine = false);
	~CMaterial();
};

template<typename T>
inline void CMaterial::SetScalarParam(SCALAR_PARAM param, const T& value)
{
	// � Ÿ���̴� ���� ������ �ּҸ� �޴´�.
	void* pValue = (void*)&value;

	switch (param)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		m_Const.iArr[param - INT_0] = *((int*)pValue); // �ּҸ� ���� ���� ���� ����ȯ
		break;

	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		m_Const.fArr[param - FLOAT_0] = *((float*)pValue);
		break;

	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		m_Const.v2Arr[param - VEC2_0] = *((Vec2*)pValue);
		break;

	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		m_Const.v4Arr[param - VEC4_0] = *((Vec4*)pValue);
		break;

	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
		m_Const.matArr[param - MAT_0] = *((Matrix*)pValue);
		break;
	}
}