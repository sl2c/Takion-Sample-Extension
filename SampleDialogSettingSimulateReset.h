#pragma once

#include "BaseDialogSettingSimulateOptionSource.h"

enum SampleDialogSettingSimulateResetIds
{
	comboBoxSimResetMessageId = baseSimulateOptionSourceIdsCount,
	staticSimResetMessageId,
	comboBoxSimBookId,
	staticSimBookId,

	simulateResetIdsCount
};

class SampleDialogSettingSimulateReset : public BaseDialogSettingSimulateOptionSource
{
public:
	SampleDialogSettingSimulateReset(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
#ifndef TAKION_NO_OPTIONS
	virtual bool isOptionModeUsed() const override;
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

//	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPrice;
	ComboBoxString m_ComboBoxMessageType;
	ComboBoxEcn m_ComboBoxBook;

	afx_msg void OnSelchangeMessageType();
	afx_msg void OnSelchangeBook();
//	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

//	virtual void SymbolTypeChanged() override;
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

#ifndef TAKION_NO_OPTIONS
	virtual bool OptionModeChanged() override;
#endif

	TMsgResetSymbol m_resetSymbol;
	TSmMsgResetSymbol m_resetSymbolSm;

	TMsgResetBook m_resetBook;
	TSmMsgResetBook m_resetBookSm;

	TMsgResetImbalances m_resetImbalancesMd;
	TSmMsgResetImbalances m_resetImbalancesMdSm;

	TMsgMsResetImbalances m_resetImbalancesMs;
	TSmMsgMsResetImbalances m_resetImbalancesMsSm;
	enum MessageType : unsigned char
	{
		MTR_QUOTE,
		MTR_IMBALANCE,

		MTR_Count
	};
	unsigned char m_messageType;
	unsigned char m_book;
	unsigned char m_allBooksOrdinal;

	virtual bool isSendMessageButtonEnabled() const override;
	virtual void SourceChanged();
};
