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

    // LandScape 인스턴스를 가져옵니다.
    CLandScape* pLandScape = GetTarget()->LandScape();
    if (pLandScape == nullptr)
        return;

    // 현재 모드 변경 UI
    ImGui::SeparatorText("LandScape Mode");
    const char* modes[] = { "None", "Height Map", "Splating", "Grass Brush"};
    int currentMode = (int)pLandScape->GetMode();
    if (ImGui::Combo("Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
    {
        pLandScape->SetMode(static_cast<LANDSCAPE_MODE>(currentMode));
    }

    // 브러시 인덱스 변경 UI
    ImGui::SeparatorText("Brush Settings");
    std::vector<Ptr<CTexture>>& vecBrush = pLandScape->GetBrushTextures();
    int brushIndex = pLandScape->GetBrushIndex();

    // 브러시 텍스처 목록 표시 및 선택 (가로로 최대 4개씩, 간격 조정)
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

            // 텍스처를 버튼으로 표시
            if (ImGui::ImageButton(texID, size, uv0, uv1))
            {
                // 브러시 인덱스를 선택한 텍스처로 변경
                pLandScape->SetBrushIndex(static_cast<int>(i));
            }
            ImGui::PopID();

            count++;
        }
    }
    ImGui::EndTable();

    // 현재 선택된 브러시 텍스처 정보 표시
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

    // 브러시 스케일 조정 UI
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
    

    // 테셀레이션 레벨 조정 UI
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

    // 와이어 프레임 모드 토글
    ImGui::SeparatorText("Wireframe Mode");
    bool isWireframe = pLandScape->IsWireframeMode();
    if (ImGui::Checkbox("Wireframe", &isWireframe))
    {
        pLandScape->SetWireframeMode(isWireframe);
    }
}

void CLandScapeUI::WeightMap(CLandScape* landscape)
{
    // Weight Map Texture 가져오기
    Ptr<CTexture> weightMapTex = landscape->GetWeightMapTexture();  // m_ColorTex 가져오기
    if (weightMapTex == nullptr)
        return;

    // Weight Texture 목록 생성 및 저장
    static std::vector<ComPtr<ID3D11ShaderResourceView>> srvList;
    static bool srvInitialized = false;
    if (!srvInitialized)
    {
        CreateWeightMapSRVs(weightMapTex, srvList, 6); // 6개의 서브리소스 가정
        srvInitialized = true;
    }

    // Weight Map 텍스처 표시
    ImGui::SeparatorText("Weight Texture Settings");
    ImGui::BeginTable("WeightTable", 4, ImGuiTableFlags_SizingFixedFit);
    for (int i = 0; i < srvList.size(); ++i)
    {
        ImGui::TableNextColumn();
        ImGui::PushID(i + 100); // Brush 텍스처와 ID 충돌 방지

        ImTextureID texID = srvList[i].Get();  // 개별적으로 생성된 SRV 사용
        ImVec2 size = ImVec2(50, 50);
        ImVec2 uv0 = ImVec2(0, 0);
        ImVec2 uv1 = ImVec2(1, 1);

        if (ImGui::ImageButton(texID, size, uv0, uv1))
        {
            // Weight 텍스처 인덱스를 선택한 것으로 변경
            landscape->SetWeightIndex(i);
        }
        ImGui::PopID();
    }
    ImGui::EndTable();

    // 현재 선택된 Weight 텍스처 표시
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
    // Grass 텍스처 선택 UI
    ImGui::SeparatorText("Grass Texture Selection");

    // 텍스처 목록을 가로로 나열 (최대 4개씩)
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

            // 선택된 텍스처는 강조 표시를 위해 배경색 변경
            bool isSelected = (i == selectedTextureIndex);
            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f)); // 녹색 강조색
            }

            // 텍스처를 버튼으로 표시
            if (ImGui::ImageButton(texID, size, uv0, uv1))
            {
                // 텍스처가 선택되면 인덱스를 저장
                selectedTextureIndex = static_cast<int>(i);
                landscape->SetSelectedGrassTextureIndex(selectedTextureIndex); // 선택된 인덱스 업데이트
            }

            if (isSelected)
            {
                ImGui::PopStyleColor(); // 강조 색상 해제
            }

            ImGui::PopID();
        }
    }
    ImGui::EndTable();

    // 선택된 텍스처의 인덱스를 기반으로 CMeshData 설정 및 텍스처 적용
    if (selectedTextureIndex != -1)
    {
        // 선택된 텍스처의 인덱스를 통해 CMeshData 이름 결정
        std::wstring meshName = L"meshdata\\Grass_" + std::to_wstring(selectedTextureIndex) + L".mdat";
        Ptr<CMeshData> selectedMesh = CAssetManager::GetInst()->FindAsset<CMeshData>(meshName);

        if (selectedMesh != nullptr)
        {
            landscape->SetGrassMeshData(selectedMesh);

            // 선택된 텍스처를 해당 CMeshData의 Material에 설정
            selectedMesh->GetMaterial(0)->SetTexParam(TEX_0, vecGrassTexture[selectedTextureIndex]);
        }
    }

    // Grass 개수 설정
    ImGui::SeparatorText("Grass Settings");

    UINT grassCount = landscape->GetGrassCount();
    if (ImGui::InputInt("Grass Count", (int*)&grassCount))
    {
        grassCount = max(1, grassCount);  // 최소 1개로 설정
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
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 원본 텍스처 형식 사용
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
        srvDesc.Texture2DArray.MipLevels = 1;
        srvDesc.Texture2DArray.FirstArraySlice = i;
        srvDesc.Texture2DArray.ArraySize = 1;

        HRESULT hr = device->CreateShaderResourceView(texture->GetTex2D().Get(), &srvDesc, &srvList[i]);
        if (FAILED(hr))
        {
            // 에러 처리
            continue;
        }
    }
}