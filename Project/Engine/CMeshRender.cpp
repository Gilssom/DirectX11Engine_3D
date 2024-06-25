#include "pch.h"
#include "CMeshRender.h"

#include "CTransform.h"
#include "CMaterial.h"
#include "CAnimator2D.h"
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
	// 만약 Mesh 나 Material 이 없다면 (동적 재질이 있으면 동적 재질을 반환함)
	if (GetMesh() == nullptr || GetMaterial() == nullptr)
	{
		return;
	}

	// 오브젝트의 위치값을 상수버퍼를 통해서 바인딩
	Transform()->Binding();

	// 오브젝트가 Animator 컴포넌트가 있다면
	if (Animator2D())
	{
		Animator2D()->Binding();
	}
	else
	{
		// 이전 오브젝트의 상수 버퍼 영향을 안받게 Clear 를 해야함
		CAnim2D::Clear();
	}

	// 사용할 머테리얼(쉐이더, 텍스처) 바인딩
	GetMaterial()->Binding();

	// Mesh , Shader Render
	GetMesh()->Render();
}