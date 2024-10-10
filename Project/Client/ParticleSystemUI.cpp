#include "pch.h"
#include "ParticleSystemUI.h"
#include <Engine\CParticleSystem.h>

ParticleSystemUI::ParticleSystemUI()
	: ComponentUI("ParticleSystem", "##ParticleSystemUI", COMPONENT_TYPE::PARTICLESYSTEM)
{
	SIZE;
}

ParticleSystemUI::~ParticleSystemUI()
{
}

void ParticleSystemUI::Render_Tick()
{
	Render_Title();

    // 파티클 시스템 인스턴스를 가져옵니다.
    CParticleSystem* pParticleSystem = GetTarget()->ParticleSystem();
    if (pParticleSystem == nullptr)
        return;

    // 파티클 모듈 가져오기
    tParticleModule& module = pParticleSystem->GetModule();

    bool bUpdated = false; // 값을 변경했는지 확인하는 플래그

    // 1. Spawn Module
    ImGui::SeparatorText("Spawn Settings");
    if (ImGui::Checkbox("Spawn", (bool*)&module.Module[(UINT)PARTICLE_MODULE::SPAWN])) bUpdated = true;
    if (ImGui::SliderInt("Spawn Rate", (int*)&module.vSpawnRate, 1, 1000)) bUpdated = true; // 초당 생성 개수
    if (ImGui::ColorEdit4("Spawn Color", (float*)&module.vSpawnColor)) bUpdated = true;     // 생성 색상
    if (ImGui::SliderFloat3("Min Scale", (float*)&module.vSpawnMinScale, 1.0f, 100.0f)) bUpdated = true; // 최소 크기
    if (ImGui::SliderFloat3("Max Scale", (float*)&module.vSpawnMaxScale, 1.0f, 100.0f)) bUpdated = true; // 최대 크기
    if (ImGui::SliderFloat("Min Life", &module.MinLife, 0.1f, 10.0f)) bUpdated = true;     // 최소 수명
    if (ImGui::SliderFloat("Max Life", &module.MaxLife, 0.1f, 10.0f)) bUpdated = true;     // 최대 수명

    // 2. Spawn Area 설정
    ImGui::SeparatorText("Spawn Area Settings");
    if (ImGui::Combo("Spawn Shape", (int*)&module.SpawnShape, "Box\0Sphere\0")) bUpdated = true; // 스폰 모양 (Box, Sphere)
    if (ImGui::SliderFloat3("Spawn Shape Scale", (float*)&module.SpawnShapeScale, 0.1f, 1000.0f)) bUpdated = true; // 스폰 크기

    ImGui::SeparatorText("Block Spawn Area");
    if (ImGui::Combo("Block Shape", (int*)&module.BlockSpawnShape, "Box\0Sphere\0")) bUpdated = true; // 스폰 금지 모양 (Box, Sphere)
    if (ImGui::SliderFloat3("Block Shape Scale", (float*)&module.BlockSpawnShapeScale, 0.1f, 1000.0f)) bUpdated = true; // 금지 영역 크기

    // 3. 공간 타입 설정
    ImGui::SeparatorText("Space Type");
    if (ImGui::Combo("Space Type", (int*)&module.SpaceType, "Local Space\0World Space\0")) bUpdated = true;

    // 4. Burst Module
    ImGui::SeparatorText("Spawn Burst Settings");
    if (ImGui::Checkbox("Spawn Burst", (bool*)&module.Module[(UINT)PARTICLE_MODULE::SPAWN_BURST])) bUpdated = true;
    if (ImGui::SliderInt("Burst Count", (int*)&module.SpawnBurstCount, 1, 1000)) bUpdated = true;  // 한 번에 생성할 파티클 개수
    if (ImGui::Checkbox("Burst Repeat", (bool*)&module.SpawnBurstRepeat)) bUpdated = true;         // 반복 여부
    if (ImGui::SliderFloat("Burst Repeat Time", &module.SpawnBurstRepeatTime, 0.1f, 10.0f)) bUpdated = true; // 재생성 시간

    // 5. Add Velocity Module
    ImGui::SeparatorText("Add Velocity Settings");
    if (ImGui::Checkbox("Add Velocity", (bool*)&module.Module[(UINT)PARTICLE_MODULE::ADD_VELOCITY])) bUpdated = true;
    if (ImGui::Combo("Velocity Type", (int*)&module.AddVelocityType, "Random\0From Center\0To Center\0Fixed\0")) bUpdated = true; // 방향
    if (ImGui::SliderFloat3("Fixed Direction", (float*)&module.AddVelocityFixedDir, -1.0f, 1.0f)) bUpdated = true; // 고정 방향 설정
    if (ImGui::SliderFloat("Min Speed", &module.AddMinSpeed, 0.0f, 1000.0f)) bUpdated = true; // 최소 속도
    if (ImGui::SliderFloat("Max Speed", &module.AddMaxSpeed, 0.0f, 1000.0f)) bUpdated = true; // 최대 속도

    // 6. Scale Module
    ImGui::SeparatorText("Scale Settings");
    if (ImGui::Checkbox("Scale", (bool*)&module.Module[(UINT)PARTICLE_MODULE::SCALE])) bUpdated = true;
    if (ImGui::SliderFloat("Start Scale", &module.StartScale, 0.1f, 10.0f)) bUpdated = true;  // 시작 크기
    if (ImGui::SliderFloat("End Scale", &module.EndScale, 0.1f, 10.0f)) bUpdated = true;      // 최종 크기

    // 7. Drag Module
    ImGui::SeparatorText("Drag Settings");
    if (ImGui::Checkbox("Drag", (bool*)&module.Module[(UINT)PARTICLE_MODULE::DRAG])) bUpdated = true;
    if (ImGui::SliderFloat("Drag End Normalized Age", &module.DestNormalizedAge, 0.0f, 1.0f)) bUpdated = true;  // 드래그가 끝나는 노멀라이즈된 나이
    if (ImGui::SliderFloat("Speed Limit", &module.LimitSpeed, 0.0f, 1000.0f)) bUpdated = true;  // 속도 제한

    // 8. Noise Force Module
    ImGui::SeparatorText("Noise Force Settings");
    if (ImGui::Checkbox("Noise Force", (bool*)&module.Module[(UINT)PARTICLE_MODULE::NOISE_FORCE])) bUpdated = true;
    if (ImGui::SliderFloat("Noise Force Term", &module.NoiseForceTerm, 0.0f, 10.0f)) bUpdated = true;  // 노이즈 힘 적용 간격
    if (ImGui::SliderFloat("Noise Force Scale", &module.NoiseForceScale, 0.0f, 100.0f)) bUpdated = true; // 노이즈 힘 크기

    // 9. Render Module
    ImGui::SeparatorText("Render Settings");
    if (ImGui::Checkbox("Render", (bool*)&module.Module[(UINT)PARTICLE_MODULE::RENDER])) bUpdated = true;
    if (ImGui::ColorEdit3("End Color", (float*)&module.EndColor)) bUpdated = true; // 최종 변환 색상
    if (ImGui::Checkbox("Fade Out", (bool*)&module.FadeOut)) bUpdated = true;      // 페이드 아웃 여부
    if (ImGui::SliderFloat("Fade Out Start Ratio", &module.FadeOutStartRatio, 0.0f, 1.0f)) bUpdated = true; // 페이드 아웃 시작 지점
    if (ImGui::Checkbox("Velocity Alignment", (bool*)&module.VelocityAlignment)) bUpdated = true;  // 속도 정렬 여부

    // 값이 업데이트되었을 경우, 버퍼를 업데이트
    if (bUpdated)
    {
        pParticleSystem->UpdateModuleBuffer();
    }
}