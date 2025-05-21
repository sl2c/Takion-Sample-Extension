#pragma once

#include "BaseDialogSettingSimulateSource.h"

enum BaseDialogSettingSimulateTradingActionIds
{
	staticSimTradingActionId = baseSimulateSourceIdsCount,
	comboBoxSimTradingActionId,

	staticSimTradingActionReasonId,
	editSimTradingActionReasonId,

	simulateTradingActionIdsCount
};

class SampleDialogSettingSimulateTradingAction : public BaseDialogSettingSimulateSource
{
public:
	SampleDialogSettingSimulateTradingAction(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	ComboBoxNameDescription m_ComboBoxTradingState;
	CEdit m_EditTradingActionReason;

	afx_msg void OnSelchangeTradingState();
	afx_msg void OnTradingActionReasonChange();
	DECLARE_MESSAGE_MAP()

//	virtual void SymbolTypeChanged() override;
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	TMsgStockTradingAction m_tradingActionMd;
	TSmMsgStockTradingAction m_tradingActionMdSm;
	TMsgMsStockTradingAction m_tradingActionMs;
	TSmMsgMsStockTradingAction m_tradingActionMsSm;
//#ifndef TAKION_NO_OPTIONS
//	TMsgFutTradingAction m_futureTradingActionMd;
//#endif
	unsigned int m_tradingActionReason;
	char m_tradingState;
};

