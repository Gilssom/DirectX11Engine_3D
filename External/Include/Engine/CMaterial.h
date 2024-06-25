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

	void* GetScalarParam(SCALAR_PARAM param);
	Ptr<CTexture>& GetTexParam(TEX_PARAM texParam) { return m_arrTex[texParam]; }

	bool IsDynamic() { return m_bDynamic; }

	Ptr<CMaterial> GetDynamicMaterial();

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
	// 어떤 타입이던 간에 무조건 주소를 받는다.
	void* pValue = (void*)&value;

	switch (param)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		m_Const.iArr[param - INT_0] = *((int*)pValue); // 주소를 받은 값을 강제 형변환
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