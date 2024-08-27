#pragma once
#include "CComponent.h"

class CStructuredBuffer;

class CAnimator3D : public CComponent
{
private:
    const vector<tMTBone>*      m_pVecBones;
    const vector<tMTAnimClip>*  m_pVecClip;

    vector<float>				m_vecClipUpdateTime;
    vector<Matrix>				m_vecFinalBoneMat;      // �ؽ��Ŀ� ������ ���� �������
    int							m_iFrameCount;          // 30
    double						m_dCurTime;
    int							m_iCurClip;             // Ŭ�� �ε���	

    int							m_iFrameIdx;            // Ŭ���� ���� ������
    int							m_iNextFrameIdx;        // Ŭ���� ���� ������
    float						m_fRatio;	            // ������ ���� ����

    CStructuredBuffer*          m_pBoneFinalMatBuffer;  // Ư�� �������� ���� ���
    bool						m_bFinalMatUpdate;      // ������� ���� ���࿩��

public:
    void SetBones(const vector<tMTBone>* vecBones) { m_pVecBones = vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
    void SetAnimClip(const vector<tMTAnimClip>* vecAnimClip);
    void SetClipTime(int iClipIdx, float fTime) { m_vecClipUpdateTime[iClipIdx] = fTime; }

    CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMatBuffer; }
    UINT GetBoneCount() { return (UINT)m_pVecBones->size(); }
    void ClearData();

private:
    void check_mesh(Ptr<CMesh> pMesh);

public:
    virtual void FinalTick() override;
    void Binding();
    virtual void SaveToLevelFile(FILE* file) override;
    virtual void LoadFromLevelFile(FILE* file) override;

public:
    CLONE(CAnimator3D)

public:
    CAnimator3D();
    CAnimator3D(const CAnimator3D& other);
    ~CAnimator3D();
};