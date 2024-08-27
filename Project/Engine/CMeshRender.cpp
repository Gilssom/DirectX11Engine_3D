#include "pch.h"
#include "CMeshRender.h"

#include "CTransform.h"
#include "CMaterial.h"
#include "CAnimator2D.h"
#include "CAnimator3D.h"
#include "CAnim2D.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{

}

CMeshRender::~CMeshRender()
{

}

void CMeshRender::FinalTick()
{

}

void CMeshRender::Render()
{
	// 만약 Mesh 가 없다면
	if (GetMesh() == nullptr)
	{
		return;
	}

	// 오브젝트의 위치값을 상수버퍼를 통해서 바인딩
	Transform()->Binding();

	// 오브젝트가 Animator 컴포넌트가 있다면..
	if (Animator2D())
	{
		Animator2D()->Binding();
	}
	else
	{
		CAnim2D::Clear();
	}

	// Animator3D Binding
	if (Animator3D())
	{
		Animator3D()->Binding();

		for (UINT i = 0; i < GetMaterialCount(); ++i)
		{
			if (nullptr == GetMaterial(i))
				continue;

			GetMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
			GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	// 메쉬의 모든 부위를 렌더링 한다.
	UINT iMtrlCount = GetMaterialCount();
	for (int i = 0; i < iMtrlCount; ++i)
	{
		if (nullptr == GetMaterial(i))
			continue;

		// 사용할 쉐이더 바인딩
		GetMaterial(i)->Binding();

		// 메시 바인딩 및 렌더링
		GetMesh()->Render(i);
	}

	if (Animator3D())
	{
		Animator3D()->ClearData();
	}
}