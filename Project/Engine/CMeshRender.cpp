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
	// ���� Mesh �� ���ٸ�
	if (GetMesh() == nullptr)
	{
		return;
	}

	// ������Ʈ�� ��ġ���� ������۸� ���ؼ� ���ε�
	Transform()->Binding();

	// �޽��� ��� ������ ������ �Ѵ�.
	UINT iMtrlCount = GetMaterialCount();
	for (int i = 0; i < iMtrlCount; ++i)
	{
		if (nullptr == GetMaterial(i))
			continue;

		// ������Ʈ�� Animator ������Ʈ�� �ִٸ�..
		if (Animator2D())
		{
			Animator2D()->Binding();
		}
		else
		{
			CAnim2D::Clear();
		}

		// ����� ���̴� ���ε�
		GetMaterial(i)->Binding();

		// �޽� ���ε� �� ������
		GetMesh()->Render(i);
	}
}