#pragma once
#include "ComponentUI.h"

class CLandScapeUI : public ComponentUI
{
private:

public:
	virtual void Render_Tick() override;

	void WeightMap(CLandScape* landscape);
	void GrassBrush(CLandScape* landscape);

	void CreateWeightMapSRVs(Ptr<CTexture> texture, std::vector<ComPtr<ID3D11ShaderResourceView>>& srvList, int sliceCount);

public:
	CLandScapeUI();
	~CLandScapeUI();
};