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
	// ���� Mesh �� ���ٸ�
	if (GetMesh() == nullptr)
	{
		return;
	}

	// ������Ʈ�� ��ġ���� ������۸� ���ؼ� ���ε�
	Transform()->Binding();

	// ������Ʈ�� Animator ������Ʈ�� �ִٸ�..
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

			GetMaterial(i)->SetAnim3D(true); // Animation Mesh �˸���
			GetMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	// �޽��� ��� ������ ������ �Ѵ�.
	UINT iMtrlCount = GetMaterialCount();
	for (int i = 0; i < iMtrlCount; ++i)
	{
		if (nullptr == GetMaterial(i))
			continue;

		// ����� ���̴� ���ε�
		GetMaterial(i)->Binding();

		// �޽� ���ε� �� ������
		GetMesh()->Render(i);
	}

	if (Animator3D())
	{
		Animator3D()->ClearData();
	}
}

void CMeshRender::Render(UINT iSubset)
{
	if (nullptr == GetMesh() || nullptr == GetMaterial(iSubset))
		return;

	// Transform �� UpdateData ��û
	Transform()->Binding();

	// Animator2D ������Ʈ�� �ִٸ�
	if (Animator2D())
	{
		Animator2D()->Binding();
	}

	// Animator3D ������Ʈ
	if (Animator3D())
	{
		Animator3D()->Binding();
		GetMaterial(iSubset)->SetAnim3D(true); // Animation Mesh �˸���
		GetMaterial(iSubset)->SetBoneCount(Animator3D()->GetBoneCount());
	}

	// ����� ���� ������Ʈ
	GetMaterial(iSubset)->Binding();

	// ����� �޽� ������Ʈ �� ������
	GetMesh()->Render(iSubset);

	// Animation ���� ���� ����
	if (Animator2D())
		CAnim2D::Clear();

	if (Animator3D())
		Animator3D()->ClearData();
}
