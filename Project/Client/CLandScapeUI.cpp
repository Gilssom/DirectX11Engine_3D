#include "pch.h"
#include "CLandScapeUI.h"

#include <Engine\CLandScape.h>

CLandScapeUI::CLandScapeUI()
	: ComponentUI("LandScape", "##LandScapeUI", COMPONENT_TYPE::LANDSCAPE)
{
	SIZE;
}

CLandScapeUI::~CLandScapeUI()
{

}

void CLandScapeUI::Render_Tick()
{
	Render_Title();

    // LandScape �ν��Ͻ��� �����ɴϴ�.
    CLandScape* pLandScape = GetTarget()->LandScape();
    if (pLandScape == nullptr)
        return;

    // ���� ��� ���� UI
    ImGui::SeparatorText("LandScape Mode");
    const char* modes[] = { "None", "Height Map", "Splating", "Grass Brush"};
    int currentMode = (int)pLandScape->GetMode();
    if (ImGui::Combo("Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
    {
        pLandScape->SetMode(static_cast<LANDSCAPE_MODE>(currentMode));
    }

    // �귯�� �ε��� ���� UI
    ImGui::SeparatorText("Brush Settings");
    std::vector<Ptr<CTexture>>& vecBrush = pLandScape->GetBrushTextures();
    int brushIndex = pLandScape->GetBrushIndex();

    // �귯�� �ؽ�ó ��� ǥ�� �� ���� (���η� �ִ� 4����, ���� ����)
    ImGui::Text("Brush Texture List");
    int count = 0;
    ImGui::BeginTable("BrushTable", 4, ImGuiTableFlags_SizingFixedFit);
    for (size_t i = 0; i < vecBrush.size(); ++i)
    {
        Ptr<CTexture> brushTex = vecBrush[i];
        if (brushTex != nullptr)
        {
            ImGui::TableNextColumn();
            ImGui::PushID(static_cast<int>(i));

            ImTextureID texID = brushTex->GetSRV().Get();
            ImVec2 size = ImVec2(50, 50);
            ImVec2 uv0 = ImVec2(0, 0);
            ImVec2 uv1 = ImVec2(1, 1);

            // �ؽ�ó�� ��ư���� ǥ��
            if (ImGui::ImageButton(texID, size, uv0, uv1))
            {
                // �귯�� �ε����� ������ �ؽ�ó�� ����
                pLandScape->SetBrushIndex(static_cast<int>(i));
            }
            ImGui::PopID();

            count++;
        }
    }
    ImGui::EndTable();

    // ���� ���õ� �귯�� �ؽ�ó ���� ǥ��
    if (brushIndex >= 0 && brushIndex < vecBrush.size())
    {
        Ptr<CTexture> currentBrush = vecBrush[brushIndex];
        if (currentBrush != nullptr)
        {
            ImGui::SeparatorText("Current Brush Texture");
            ImTextureID texID = currentBrush->GetSRV().Get();
            ImGui::Image(texID, ImVec2(100, 100));
        }
    }

    // �귯�� ������ ���� UI
    ImGui::SeparatorText("Brush Scale");
    Vec2 brushScale = pLandScape->GetBrushScale();
    if (ImGui::SliderFloat2("Brush Scale", (float*)&brushScale, 0.1f, 10.0f))
    {
        pLandScape->SetBrushScale(brushScale);
    }

    switch (pLandScape->GetMode())
    {
        case LANDSCAPE_MODE::SPLATING:
        {
            WeightMap(pLandScape);

            break;
        }
        case LANDSCAPE_MODE::GRASS_MAP:
        {
            GrassBrush(pLandScape);

            break;
        }
    }
    

    // �׼����̼� ���� ���� UI
    ImGui::SeparatorText("Tessellation Settings");
    float minLevel = pLandScape->GetMinTessLevel();
    float maxLevel = pLandScape->GetMaxTessLevel();
    if (ImGui::SliderFloat("Min Tessellation Level", &minLevel, 0.0f, 10.0f))
    {
        pLandScape->SetMinTessLevel(minLevel);
    }
    if (ImGui::SliderFloat("Max Tessellation Level", &maxLevel, 0.0f, 10.0f))
    {
        pLandScape->SetMaxTessLevel(maxLevel);
    }

    // ���̾� ������ ��� ���
    ImGui::SeparatorText("Wireframe Mode");
    bool isWireframe = pLandScape->IsWireframeMode();
    if (ImGui::Checkbox("Wireframe", &isWireframe))
    {
        pLandScape->SetWireframeMode(isWireframe);
    }
}

void CLandScapeUI::WeightMap(CLandScape* landscape)
{
    // Weight Map Texture ��������
    Ptr<CTexture> weightMapTex = landscape->GetWeightMapTexture();  // m_ColorTex ��������
    if (weightMapTex == nullptr)
        return;

    // Weight Texture ��� ���� �� ����
    static std::vector<ComPtr<ID3D11ShaderResourceView>> srvList;
    static bool srvInitialized = false;
    if (!srvInitialized)
    {
        CreateWeightMapSRVs(weightMapTex, srvList, 6); // 6���� ���긮�ҽ� ����
        srvInitialized = true;
    }

    // Weight Map �ؽ�ó ǥ��
    ImGui::SeparatorText("Weight Texture Settings");
    ImGui::BeginTable("WeightTable", 4, ImGuiTableFlags_SizingFixedFit);
    for (int i = 0; i < srvList.size(); ++i)
    {
        ImGui::TableNextColumn();
        ImGui::PushID(i + 100); // Brush �ؽ�ó�� ID �浹 ����

        ImTextureID texID = srvList[i].Get();  // ���������� ������ SRV ���
        ImVec2 size = ImVec2(50, 50);
        ImVec2 uv0 = ImVec2(0, 0);
        ImVec2 uv1 = ImVec2(1, 1);

        if (ImGui::ImageButton(texID, size, uv0, uv1))
        {
            // Weight �ؽ�ó �ε����� ������ ������ ����
            landscape->SetWeightIndex(i);
        }
        ImGui::PopID();
    }
    ImGui::EndTable();

    // ���� ���õ� Weight �ؽ�ó ǥ��
    int weightIndex = landscape->GetWeightIndex();
    if (weightIndex >= 0 && weightIndex < srvList.size())
    {
        ImGui::SeparatorText("Current Weight Texture");
        ImTextureID texID = srvList[weightIndex].Get();
        ImGui::Image(texID, ImVec2(100, 100));
    }
}

void CLandScapeUI::GrassBrush(CLandScape* landscape)
{
    // Grass �ؽ�ó ���� UI
    ImGui::SeparatorText("Grass Texture Selection");

    // �ؽ�ó ����� ���η� ���� (�ִ� 4����)
    std::vector<Ptr<CTexture>>& vecGrassTexture = landscape->GetGrassTextures();
    int selectedTextureIndex = landscape->GetSelectedGrassTextureIndex();

    ImGui::BeginTable("GrassTextureTable", 4, ImGuiTableFlags_SizingFixedFit);
    for (size_t i = 0; i < vecGrassTexture.size(); ++i)
    {
        Ptr<CTexture> grassTexture = vecGrassTexture[i];
        if (grassTexture != nullptr)
        {
            ImGui::TableNextColumn();
            ImGui::PushID(static_cast<int>(i));

            ImTextureID texID = grassTexture->GetSRV().Get();
            ImVec2 size = ImVec2(50, 50);
            ImVec2 uv0 = ImVec2(0, 0);
            ImVec2 uv1 = ImVec2(1, 1);

            // ���õ� �ؽ�ó�� ���� ǥ�ø� ���� ���� ����
            bool isSelected = (i == selectedTextureIndex);
            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f)); // ��� ������
            }

            // �ؽ�ó�� ��ư���� ǥ��
            if (ImGui::ImageButton(texID, size, uv0, uv1))
            {
                // �ؽ�ó�� ���õǸ� �ε����� ����
                selectedTextureIndex = static_cast<int>(i);
                landscape->SetSelectedGrassTextureIndex(selectedTextureIndex); // ���õ� �ε��� ������Ʈ
            }

            if (isSelected)
            {
                ImGui::PopStyleColor(); // ���� ���� ����
            }

            ImGui::PopID();
        }
    }
    ImGui::EndTable();

    // ���õ� �ؽ�ó�� �ε����� ������� CMeshData ���� �� �ؽ�ó ����
    if (selectedTextureIndex != -1)
    {
        // ���õ� �ؽ�ó�� �ε����� ���� CMeshData �̸� ����
        std::wstring meshName = L"meshdata\\Grass_" + std::to_wstring(selectedTextureIndex) + L".mdat";
        Ptr<CMeshData> selectedMesh = CAssetManager::GetInst()->FindAsset<CMeshData>(meshName);

        if (selectedMesh != nullptr)
        {
            landscape->SetGrassMeshData(selectedMesh);

            // ���õ� �ؽ�ó�� �ش� CMeshData�� Material�� ����
            selectedMesh->GetMaterial(0)->SetTexParam(TEX_0, vecGrassTexture[selectedTextureIndex]);
        }
    }

    // Grass ���� ����
    ImGui::SeparatorText("Grass Settings");

    UINT grassCount = landscape->GetGrassCount();
    if (ImGui::InputInt("Grass Count", (int*)&grassCount))
    {
        grassCount = max(1, grassCount);  // �ּ� 1���� ����
        landscape->SetGrassCount(grassCount);
    }
}

void CLandScapeUI::CreateWeightMapSRVs(Ptr<CTexture> texture, std::vector<ComPtr<ID3D11ShaderResourceView>>& srvList, int sliceCount)
{
    if (texture == nullptr || texture->GetTex2D() == nullptr)
        return;

    ID3D11Device* device = nullptr;
    texture->GetTex2D()->GetDevice(&device);
    if (!device)
        return;

    srvList.clear();
    srvList.resize(sliceCount);

    for (int i = 0; i < sliceCount; ++i)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ���� �ؽ�ó ���� ���
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
        srvDesc.Texture2DArray.MipLevels = 1;
        srvDesc.Texture2DArray.FirstArraySlice = i;
        srvDesc.Texture2DArray.ArraySize = 1;

        HRESULT hr = device->CreateShaderResourceView(texture->GetTex2D().Get(), &srvDesc, &srvList[i]);
        if (FAILED(hr))
        {
            // ���� ó��
            continue;
        }
    }
}