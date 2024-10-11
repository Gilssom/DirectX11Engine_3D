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

    // ��ƼŬ �ý��� �ν��Ͻ��� �����ɴϴ�.
    CParticleSystem* pParticleSystem = GetTarget()->ParticleSystem();
    if (pParticleSystem == nullptr)
        return;

    // ��ƼŬ ��� ��������
    tParticleModule& module = pParticleSystem->GetModule();

    bool bUpdated = false; // ���� �����ߴ��� Ȯ���ϴ� �÷���

    // 0. Texture Change
     // Texture UI �߰�
    Ptr<CTexture> pTexture = pParticleSystem->GetParticleTexture();
    ImTextureID TexID = 0;
    std::string strTexKey;

    if (pTexture != nullptr)
    {
        TexID = pTexture->GetSRV().Get();
        strTexKey = ToString(pTexture->GetKey());
    }
    else
    {
        strTexKey = "None";
    }

    // Texture �̸� ���
    ImGui::Text("Particle Texture");
    ImGui::InputText("##TextureUIName", (char*)strTexKey.c_str(), strTexKey.capacity(), ImGuiInputTextFlags_ReadOnly);

    // Texture �̹��� ���
    ImGui::SeparatorText("Texture");
    ImVec2 uv_min = ImVec2(0.0f, 0.0f);
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);
    ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

    ImGui::Image(TexID, ImVec2(150, 150), uv_min, uv_max, ImGui::GetStyleColorVec4(ImGuiCol_Text), border_col);

    // Drag & Drop���� Texture ���� �����ϵ��� ����
    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content");
        if (payload != nullptr)
        {
            // payload���� ������ ��������
            DWORD_PTR dwData = 0;
            memcpy(&dwData, payload->Data, payload->DataSize);

            // �巡�׵� Asset�� Type Ȯ��
            Ptr<CAsset> pAsset = (CAsset*)dwData;
            if (pAsset->GetAssetType() == ASSET_TYPE::TEXTURE)
            {
                // Texture ����
                pParticleSystem->SetParticleTexture((CTexture*)pAsset.Get());
                bUpdated = true;
            }
        }
        ImGui::EndDragDropTarget();
    }

    // 1. Spawn Module
    ImGui::SeparatorText("Spawn Settings");
    if (ImGui::Checkbox("Spawn", (bool*)&module.Module[(UINT)PARTICLE_MODULE::SPAWN])) bUpdated = true;
    if (ImGui::SliderInt("Spawn Rate", (int*)&module.vSpawnRate, 1, 1000)) bUpdated = true; // �ʴ� ���� ����
    if (ImGui::ColorEdit4("Spawn Color", (float*)&module.vSpawnColor)) bUpdated = true;     // ���� ����
    if (ImGui::SliderFloat3("Min Scale", (float*)&module.vSpawnMinScale, 1.0f, 100.0f)) bUpdated = true; // �ּ� ũ��
    if (ImGui::SliderFloat3("Max Scale", (float*)&module.vSpawnMaxScale, 1.0f, 100.0f)) bUpdated = true; // �ִ� ũ��
    if (ImGui::SliderFloat("Min Life", &module.MinLife, 0.1f, 10.0f)) bUpdated = true;     // �ּ� ����
    if (ImGui::SliderFloat("Max Life", &module.MaxLife, 0.1f, 10.0f)) bUpdated = true;     // �ִ� ����

    // 2. Spawn Area ����
    ImGui::SeparatorText("Spawn Area Settings");
    if (ImGui::Combo("Spawn Shape", (int*)&module.SpawnShape, "Box\0Sphere\0")) bUpdated = true; // ���� ��� (Box, Sphere)
    if (ImGui::SliderFloat3("Spawn Shape Scale", (float*)&module.SpawnShapeScale, 0.1f, 1000.0f)) bUpdated = true; // ���� ũ��

    ImGui::SeparatorText("Block Spawn Area");
    if (ImGui::Combo("Block Shape", (int*)&module.BlockSpawnShape, "Box\0Sphere\0")) bUpdated = true; // ���� ���� ��� (Box, Sphere)
    if (ImGui::SliderFloat3("Block Shape Scale", (float*)&module.BlockSpawnShapeScale, 0.1f, 1000.0f)) bUpdated = true; // ���� ���� ũ��

    // 3. ���� Ÿ�� ����
    ImGui::SeparatorText("Space Type");
    if (ImGui::Combo("Space Type", (int*)&module.SpaceType, "Local Space\0World Space\0")) bUpdated = true;

    // 4. Burst Module
    ImGui::SeparatorText("Spawn Burst Settings");
    if (ImGui::Checkbox("Spawn Burst", (bool*)&module.Module[(UINT)PARTICLE_MODULE::SPAWN_BURST])) bUpdated = true;
    if (ImGui::SliderInt("Burst Count", (int*)&module.SpawnBurstCount, 1, 1000)) bUpdated = true;  // �� ���� ������ ��ƼŬ ����
    if (ImGui::Checkbox("Burst Repeat", (bool*)&module.SpawnBurstRepeat)) bUpdated = true;         // �ݺ� ����
    if (ImGui::SliderFloat("Burst Repeat Time", &module.SpawnBurstRepeatTime, 0.1f, 10.0f)) bUpdated = true; // ����� �ð�

    // 5. Add Velocity Module
    ImGui::SeparatorText("Add Velocity Settings");
    if (ImGui::Checkbox("Add Velocity", (bool*)&module.Module[(UINT)PARTICLE_MODULE::ADD_VELOCITY])) bUpdated = true;
    if (ImGui::Combo("Velocity Type", (int*)&module.AddVelocityType, "Random\0From Center\0To Center\0Fixed\0")) bUpdated = true; // ����
    if (ImGui::SliderFloat3("Fixed Direction", (float*)&module.AddVelocityFixedDir, -1.0f, 1.0f)) bUpdated = true; // ���� ���� ����
    if (ImGui::SliderFloat("Min Speed", &module.AddMinSpeed, 0.0f, 1000.0f)) bUpdated = true; // �ּ� �ӵ�
    if (ImGui::SliderFloat("Max Speed", &module.AddMaxSpeed, 0.0f, 1000.0f)) bUpdated = true; // �ִ� �ӵ�

    // 6. Scale Module
    ImGui::SeparatorText("Scale Settings");
    if (ImGui::Checkbox("Scale", (bool*)&module.Module[(UINT)PARTICLE_MODULE::SCALE])) bUpdated = true;
    if (ImGui::SliderFloat("Start Scale", &module.StartScale, 0.1f, 10.0f)) bUpdated = true;  // ���� ũ��
    if (ImGui::SliderFloat("End Scale", &module.EndScale, 0.1f, 10.0f)) bUpdated = true;      // ���� ũ��

    // 7. Drag Module
    ImGui::SeparatorText("Drag Settings");
    if (ImGui::Checkbox("Drag", (bool*)&module.Module[(UINT)PARTICLE_MODULE::DRAG])) bUpdated = true;
    if (ImGui::SliderFloat("Drag End Normalized Age", &module.DestNormalizedAge, 0.0f, 1.0f)) bUpdated = true;  // �巡�װ� ������ ��ֶ������ ����
    if (ImGui::SliderFloat("Speed Limit", &module.LimitSpeed, 0.0f, 1000.0f)) bUpdated = true;  // �ӵ� ����

    // 8. Noise Force Module
    ImGui::SeparatorText("Noise Force Settings");
    if (ImGui::Checkbox("Noise Force", (bool*)&module.Module[(UINT)PARTICLE_MODULE::NOISE_FORCE])) bUpdated = true;
    if (ImGui::SliderFloat("Noise Force Term", &module.NoiseForceTerm, 0.0f, 10.0f)) bUpdated = true;  // ������ �� ���� ����
    if (ImGui::SliderFloat("Noise Force Scale", &module.NoiseForceScale, 0.0f, 100.0f)) bUpdated = true; // ������ �� ũ��

    // 9. Render Module
    ImGui::SeparatorText("Render Settings");
    if (ImGui::Checkbox("Render", (bool*)&module.Module[(UINT)PARTICLE_MODULE::RENDER])) bUpdated = true;
    if (ImGui::ColorEdit3("End Color", (float*)&module.EndColor)) bUpdated = true; // ���� ��ȯ ����
    if (ImGui::Checkbox("Fade Out", (bool*)&module.FadeOut)) bUpdated = true;      // ���̵� �ƿ� ����
    if (ImGui::SliderFloat("Fade Out Start Ratio", &module.FadeOutStartRatio, 0.0f, 1.0f)) bUpdated = true; // ���̵� �ƿ� ���� ����
    if (ImGui::Checkbox("Velocity Alignment", (bool*)&module.VelocityAlignment)) bUpdated = true;  // �ӵ� ���� ����

    // ���� ������Ʈ�Ǿ��� ���, ���۸� ������Ʈ
    if (bUpdated)
    {
        pParticleSystem->UpdateModuleBuffer();
    }

    // ������ �̸��� �Է��� �� �ִ� UI
    ImGui::SeparatorText("Save Particle System");
    static char prefabName[128] = "Slash_Effect";  // �⺻��
    ImGui::InputText("Prefab Name", prefabName, IM_ARRAYSIZE(prefabName));

    // ���� ��� �Է��� �� �ִ� UI (��θ� ���� �����ϰ� ��)
    static char filePath[256] = "Prefab\\Slash_Effect.pref";  // �⺻ ���
    ImGui::InputText("File Path", filePath, IM_ARRAYSIZE(filePath));

    // ������ ���� ��ư
    if (ImGui::Button("Save Prefab"))
    {
        SaveParticle(pParticleSystem->GetOwner(), prefabName, filePath);
    }
}

void ParticleSystemUI::SaveParticle(CGameObject* pParticleObject, const std::string& prefabName, const std::string& filePath)
{
    // ������ ����
    Ptr<CPrefab> prefab = new CPrefab(pParticleObject);

    // �������� ���� �Ŵ����� �߰�
    CAssetManager::GetInst()->AddAsset<CPrefab>(std::wstring(prefabName.begin(), prefabName.end()), prefab);

    // ���� ��� ���� �� ����
    std::wstring fullFilePath = CPathManager::GetInst()->GetContentPath() + std::wstring(filePath.begin(), filePath.end());
    prefab->Save(fullFilePath);

    // ������ �Ϸ�Ǿ����� �˸��� �޽��� ���
    ImGui::Text("Prefab saved to: %s", fullFilePath.c_str());
}