#pragma once

#include "BaseDialogSettingSimulateOptionSource.h"

enum BaseDialogSettingSimulateClosingPriceIds
{
	spinSimClosingPriceId = baseSimulateOptionSourceIdsCount,
	staticSimClosingPriceId,

	simulateClosingPriceIdsCount
};

class SampleDialogSettingSimulateClosingPrice : public BaseDialogSettingSimulateOptionSource
{
public:
	SampleDialogSettingSimulateClosingPrice(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPrice;

//	afx_msg void OnSelchangeMessageType();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

//	virtual void SymbolTypeChanged() override;
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	TMsgPreviousDayClose m_closeMd;
	TSmMsgPreviousDayClose m_closeMdSm;

#ifndef TAKION_NO_OPTIONS
	TMsgOptPreviousDayClose m_closeOptionMd;
	TMsgFutPreviousDayClose m_closeFutureMd;
#endif	
	TMsgMsPreviousDayClose m_closeMs;
	TSmMsgMsPreviousDayClose m_closeMsSm;

	void AddPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
	Price m_price;
};
