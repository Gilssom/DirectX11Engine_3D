//#pragma once
//#include "CComponent.h"
//
//class CPostProcess : public CComponent
//{
//private:
//	vector<Ptr<CMaterial>>	m_PostMaterial;
//
//	float m_BloomPower;
//	float m_BlurRadius;
//	float m_BrightnessScale;
//	float m_BloomThreshold;
//
//	float m_Radius;
//	float m_Sofeness;
//
//	float m_FocusDistance;
//	float m_FocusRange;
//
//public:
//	void SetBloomPower(float value)			{ m_BloomPower = value; }
//	void SetBlurRadius(float value)			{ m_BlurRadius = value; }
//	void SetBrightnessScale(float value)	{ m_BrightnessScale = value; }
//	void SetBloomThreshold(float value)		{ m_BloomThreshold = value; }
//	void SetRadius(float value)				{ m_Radius = value; }
//	void SetSofeness(float value)			{ m_Sofeness = value; }
//	void SetFocusDistance(float value)		{ m_FocusDistance = value; }
//	void SetFocusRange(float value)			{ m_FocusRange = value; }
//
//	float GetBloomPower()		const { return m_BloomPower; }
//	float GetBlurRadius()		const { return m_BlurRadius; }
//	float GetBrightnessScale()	const { return m_BrightnessScale; }
//	float GetBloomThreshold()	const { return m_BloomThreshold; }
//	float GetRadius()			const { return m_Radius; }
//	float GetSofeness()			const { return m_Sofeness; }
//	float GetFocusDistance()	const { return m_FocusDistance; }
//	float GetFocusRange()		const { return m_FocusRange; }
//
//private:
//	void Binding();
//
//public:
//	virtual void Begin() override;
//	virtual void FinalTick() override;
//
//	virtual void SaveToLevelFile(FILE* file) override;
//	virtual void LoadFromLevelFile(FILE* file) override;
//
//	CLONE(CPostProcess);
//
//public:
//	CPostProcess();
//	CPostProcess(const CPostProcess& origin);
//	~CPostProcess();
//};