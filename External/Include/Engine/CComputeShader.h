#pragma once
#include "CShader.h"

#pragma region GPGPU - Compute Shader Info
// General Purpose GPU = GPGPU = 연산 및 계산 Shader = Compute Shader _ DX11
// NVIDIA CUDA : C++ 과 상당히 유사한 문법으로 GPU 를 제어할 수 있음 ( NVIDIA GPU 만 사용 가능 )
// 대신 그래픽카드는 NVIDIA - AMD 등 여러 가지로 사용하기 때문에
// 범용성이 충분히 있게 제작을 해야한다. ( = Compute Shader )

// CPU 와 GPU 의 역할과 작업의 성질에 따른 차이
// CPU : 석사 박사 2~3명 (성능)
// GPU : 노동자 1~10만명 (효율) - 단순 작업 (Rendering) - GPU Thread_병렬 처리

// Compute Shader 는 어디에서 사용하나
// - Particle System - 개별적인 입자
// 오브젝트 하나가 엄청난 개수의 개별 입자를 관리할 수 있게 된다.
// GPU 병렬 연산을 통해 한 프레임에서 한번에 Tick 을 돌릴 수 있게 되기 때문.
#pragma endregion

// 연산 작업 GPU
class CComputeShader : public CShader
{
private:
    ComPtr<ID3DBlob>                m_CSBlob;
    ComPtr<ID3D11ComputeShader>     m_CS;


protected:
    tMtrlConst                      m_Const;


protected:
    // 1개의 그룹 당 스레드 개수
    UINT                            m_ThreadPerGroupX;
    UINT                            m_ThreadPerGroupY;
    UINT                            m_ThreadPerGroupZ;

    // 그룹 개수
    UINT                            m_GroupX;
    UINT                            m_GroupY;
    UINT                            m_GroupZ;


public:
    virtual int Binding() = 0;
    virtual void CalculateGroupNum() = 0;
    virtual void Clear() = 0;


public:
    int Execute();


public:
    int CreateComputeShader(const wstring& strFilePath, const string& psFuncName);


public:
    CComputeShader(UINT threadPerGroupX, UINT threadPerGroupY, UINT threadPerGroupZ);
    ~CComputeShader();
};

