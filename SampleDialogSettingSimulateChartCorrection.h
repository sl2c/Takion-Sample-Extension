#pragma once

#include "BaseDialogSettingSimulateSymbolMap.h"//"BaseDialogSettingSimulateSource.h"

enum SampleSettingSimulateChartCorrectionIds
{
	spinSimChartCorrectionPriceStartId = baseSimulateSymbolMapIdsCount,//baseSimulateSourceIdsCount,
	staticSimChartCorrectionPriceStartId,

	spinSimChartCorrectionPriceHighId,
	staticSimChartCorrectionPriceHighId,

	spinSimChartCorrectionPriceLowId,
	staticSimChartCorrectionPriceLowId,

	spinSimChartCorrectionPriceEndId,
	staticSimChartCorrectionPriceEndId,

	spinSimChartCorrectionMoneyTradedId,
	staticSimChartCorrectionMoneyTradedId,

	spinSimChartCorrectionVolumeId,
	staticSimChartCorrectionVolumeId,

	spinSimChartCorrectionMinuteId,
	staticSimChartCorrectionMinuteId,

	simulateChartCorrectionIdsCount
};

class SampleDialogSettingSimulateChartCorrection : public BaseDialogSettingSimulateSymbolMap//BaseDialogSettingSimulateSource
{
public:
	SampleDialogSettingSimulateChartCorrection(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
//	static void SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
//	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceStart;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceHigh;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceLow;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceEnd;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinMoneyTraded;

	UnsignedNumberTypeSpin<unsigned int> m_SpinVolume;

	MinuteSpin m_SpinMinute;

	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	TMsgTickCorrection m_msgMs;
	TSmMsgTickCorrection m_msgMsSm;

//	void AddPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);


	Price m_priceStart;
	Price m_priceHigh;
	Price m_priceLow;
	Price m_priceEnd;
	Price m_moneyTraded;

	unsigned int m_volume;
	unsigned short m_minute;
};
