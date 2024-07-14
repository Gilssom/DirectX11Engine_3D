#include "pch.h"
#include "CFrustum.h"

#include "CCamera.h"

CFrustum::CFrustum(CCamera* owner)
	: m_Camera(owner)
{
	// Frustum �ʱ� �� ����
	//    4 --- 5
	//   /|  \  |  Far
	//  / 7 --/ 6     
	// 0 -- 1/ /
	// | \  | / 
	// 3 -- 2  Near

	m_arrProjPos[0] = Vec3(-1.f, 1.f, 0.f);
	m_arrProjPos[1] = Vec3(1.f, 1.f, 0.f);
	m_arrProjPos[2] = Vec3(1.f, -1.f, 0.f);
	m_arrProjPos[3] = Vec3(-1.f, -1.f, 0.f);

	m_arrProjPos[4] = Vec3(-1.f, 1.f, 1.f);
	m_arrProjPos[5] = Vec3(1.f, 1.f, 1.f);
	m_arrProjPos[6] = Vec3(1.f, -1.f, 1.f);
	m_arrProjPos[7] = Vec3(-1.f, -1.f, 1.f);
}

CFrustum::~CFrustum()
{

}

void CFrustum::FinalTick()
{
	const Matrix& matViewInv = m_Camera->GetViewInvMat();
	const Matrix& matProjInv = m_Camera->GetProjInvMat();
	Matrix matInv = matProjInv * matViewInv;

	// �������� ����ǥ�� -> ViewSpace -> WorldSpcae
	Vec3 arrWorld[8] = {};

	for (int i = 0; i < 8; ++i)
	{
		arrWorld[i] = XMVector2TransformCoord(m_arrProjPos[i], matInv);
	}

	// 6���� ī�޶� �þ߹��� ����� ������� �������� ������.


}