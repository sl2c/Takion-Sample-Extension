#pragma once

#include "BaseDialogSettingSimulateSource.h"

enum BaseDialogSettingSimulateLrpLuldIds
{
	comboBoxMessageId = baseSimulateSourceIdsCount,
	staticMessageId,
	spinSimLrpLuldPriceLowId,
	staticSimLrpLuldPriceLowId,
	spinSimLrpLuldPriceHighId,
	staticSimLrpLuldPriceHighId,

	simulateLrpLuldIdsCount
};

class SampleDialogSettingSimulateLrpLuld : public BaseDialogSettingSimulateSource
{
public:
	SampleDialogSettingSimulateLrpLuld(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	ComboBoxString m_ComboBoxMessageType;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceLow;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceHigh;

	afx_msg void OnSelchangeMessageType();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

//	virtual void SymbolTypeChanged() override;
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	TMsgLuld m_luldMd;
	TSmMsgLuld m_luldMdSm;
	TMsgMsLuld m_luldMs;
	TSmMsgMsLuld m_luldMsSm;
#ifndef TAKION_NO_OPTIONS
	TMsgFutLuld m_futureLuldMd;
#endif
#ifdef LRPS
	TMsgLrp m_lrpMd;
	TSmMsgLrp m_lrpMdSm;
	TMsgMsLrp m_lrpMs;
	TSmMsgMsLrp m_lrpMsSm;
#endif
	TMsgPmi m_pmiMd;
	TSmMsgPmi m_pmiMdSm;
	TMsgMsPmi m_pmiMs;
	TSmMsgMsPmi m_pmiMsSm;

	TMsgPreviousDayHighLow m_yesterdaysHighLowMd;
	TSmMsgPreviousDayHighLow m_yesterdaysHighLowMdSm;
	TMsgMsPreviousDayHighLow m_yesterdaysHighLowMs;
	TSmMsgMsPreviousDayHighLow m_yesterdaysHighLowMsSm;

	void AddPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
	enum MessageType : unsigned char
	{
#ifdef LRPS
		MT_LRP,
#endif
		MT_LULD,
		MT_PMI,
		MT_YESTERDAY_HILO,

		MT_Count
	};
	unsigned char m_messageType;
	Price m_priceLow;
	Price m_priceHigh;
};
