#pragma once
#include "CComponent.h"

class CStructuredBuffer;

class CAnimator3D : public CComponent
{
private:
    const vector<tMTBone>*      m_pVecBones;
    const vector<tMTAnimClip>*  m_pVecClip;

    vector<float>				m_vecClipUpdateTime;
    vector<Matrix>				m_vecFinalBoneMat;      // 텍스쳐에 전달할 최종 행렬정보
    int							m_iFrameCount;          // 30
    double						m_dCurTime;
    int							m_iCurClip;             // 클립 인덱스	

    int							m_iFrameIdx;            // 클립의 현재 프레임
    int							m_iNextFrameIdx;        // 클립의 다음 프레임
    float						m_fRatio;	            // 프레임 사이 비율

    CStructuredBuffer*          m_pBoneFinalMatBuffer;  // 특정 프레임의 최종 행렬
    bool						m_bFinalMatUpdate;      // 최종행렬 연산 수행여부

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