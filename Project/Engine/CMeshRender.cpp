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
	// ���� Mesh �� Material �� ���ٸ� (���� ������ ������ ���� ������ ��ȯ��)
	if (GetMesh() == nullptr || GetMaterial() == nullptr)
	{
		return;
	}

	// ������Ʈ�� ��ġ���� ������۸� ���ؼ� ���ε�
	Transform()->Binding();

	// ������Ʈ�� Animator ������Ʈ�� �ִٸ�
	if (Animator2D())
	{
		Animator2D()->Binding();
	}
	else
	{
		// ���� ������Ʈ�� ��� ���� ������ �ȹް� Clear �� �ؾ���
		CAnim2D::Clear();
	}

	// ����� ���׸���(���̴�, �ؽ�ó) ���ε�
	GetMaterial()->Binding();

	// Mesh , Shader Render
	GetMesh()->Render();
}