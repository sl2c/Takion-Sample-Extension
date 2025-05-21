#pragma once

#include "BaseDialogSettingSimulateQuote.h"

enum SampleSettingSimulateQuoteIds
{
	spinSimQuotePriceId = baseSimulateQuoteIdsCount,
	staticSimQuotePriceId,
	spinSimQuoteSizeId,
	staticSimQuoteSizeId,
	comboQuoteConditionId,
	staticQuoteConditionId,
	editSimAttributionId,
	staticSimAttributionId,
	spinSimQuoteRemainingSizeId,
	checkBoxSimQuoteRemainingSizeId,
	checkBoxSimQuotePrintableId,
	checkBoxSimQuoteRetailLiquidityId,

	comboSimQuoteMessageId,
	staticSimQuoteMessageId,

	simulateQuoteIdsCount
};

class SampleDialogSettingSimulateQuote : public BaseDialogSettingSimulateQuote
{
public:
	SampleDialogSettingSimulateQuote(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
#ifndef TAKION_NO_OPTIONS
	virtual bool isOptionModeUsed() const;
#endif
	bool ModifyMessageAcceptsSizeZero() const;
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPrice;
	UnsignedNumberTypeSpin<unsigned int> m_SpinSize;

	ComboBoxChar m_ComboBoxQuoteCondition;
	ValidEdit m_EditAttribution;

	UnsignedNumberTypeSpin<unsigned int> m_SpinRemainingSize;
	CButton m_CheckBoxUseRemainingSize;
	CButton m_CheckBoxPrintable;
	CButton m_CheckBoxRetailLiquidity;

	ComboBoxStringUnsignedNumber<unsigned short> m_ComboBoxMessageId;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnAttributionUpdate();
	afx_msg void OnAttributionChange();
	afx_msg void OnRemainingSize();
	afx_msg void OnPrintable();
	afx_msg void OnRetailLiquidity();
	afx_msg void OnSelchangeQuoteCondition();
	afx_msg void OnSelchangeMessageId();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	Price m_price;
	unsigned int m_size;
	unsigned int m_remainingSize;
	bool m_useRemainingSize;
	bool m_printable;
	std::string m_attribution;
	unsigned int m_numericAttribution;

	bool isPriceUsed() const;
	bool isSizeUsed() const;
	bool isQuoteConditionUsed() const;
	virtual bool isRefUsed() const override;
	void UpdateMessageId();
	unsigned short m_messageId;
	std::string m_messageName;

	unsigned char m_quoteCondition;
	bool m_retailLiquidity;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	void FillQuote(MsgQuote& quote) const;

	virtual bool isSendMessageButtonEnabled() const override;

	virtual void Level2Changed() override;
	const MarketCenter* m_marketCenter;

#ifndef TAKION_NO_OPTIONS
	TMsgOptLevel2 m_messageModifyOptionQuote;
	TMsgFutBookModifyQuote m_messageModifyFutureQuote;
	TMsgFutBookDeleteQuote m_messageDeleteFutureQuote;
#endif
	TMsgBookNewQuote m_messageNewQuote;
	TMsgBookModifyQuote m_messageModifyQuote;
	TMsgBookDeleteQuote m_messageDeleteQuote;
	TMsgBookOrderExecuted m_messageExecuteQuote;
	TMsgBookOrderExecutedWithPrice m_messageExecuteWithPriceQuote;
	TMsgBookOrderCanceled m_messageCancelQuote;
	TMsgBookTrade m_messageExecuteHiddenQuote;
	TMsgBookCrossTrade m_messageCrossTradeQuote;

	TSmMsgBookNewQuote m_messageNewQuoteSm;
	TSmMsgBookModifyQuote m_messageModifyQuoteSm;
	TSmMsgBookDeleteQuote m_messageDeleteQuoteSm;
	TSmMsgBookOrderExecuted m_messageExecuteQuoteSm;
	TSmMsgBookOrderExecutedWithPrice m_messageExecuteWithPriceQuoteSm;
	TSmMsgBookOrderCanceled m_messageCancelQuoteSm;
	TSmMsgBookTrade m_messageExecuteHiddenQuoteSm;
	TSmMsgBookCrossTrade m_messageCrossTradeQuoteSm;

	virtual void UpdateMarketCenter() override;
};