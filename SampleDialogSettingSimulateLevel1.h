#pragma once

#include "BaseDialogSettingSimulateOptionSource.h"

enum SampleSettingSimulateLevel1Ids
{
	spinSimLevel1BidId = baseSimulateOptionSourceIdsCount,
	staticSimLevel1BidId,

	spinSimLevel1AskId,
	staticSimLevel1AskId,

	spinSimLevel1BidSizeId,
	staticSimLevel1BidSizeId,

	spinSimLevel1AskSizeId,
	staticSimLevel1AskSizeId,

	checkBoxSideId,

	comboLevel1ConditionId,
	staticLevel1ConditionId,

	comboLevel1BidMarketCenterId,
	staticLevel1BidMarketCenterId,

	comboLevel1AskMarketCenterId,
	staticLevel1AskMarketCenterId,

	comboLevel1MarketListingId,
	staticLevel1MarketListingId,

	simulateLevel1IdsCount
};

class SampleDialogSettingSimulateLevel1 : public BaseDialogSettingSimulateOptionSource
{
public:
	SampleDialogSettingSimulateLevel1(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
/*
#ifndef TAKION_NO_OPTIONS
	virtual bool isOptionModeUsed() const;
#endif
*/
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual bool isSendMessageButtonEnabled() const override;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinBid;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinAsk;
	UnsignedNumberTypeSpin<unsigned int> m_SpinBidSize;
	UnsignedNumberTypeSpin<unsigned int> m_SpinAskSize;

	ComboBoxChar m_ComboBoxQuoteCondition;
	ComboBoxChar m_ComboBoxBidMarketCenter;
	ComboBoxChar m_ComboBoxAskMarketCenter;
	ComboBoxChar m_ComboBoxMarketListing;

	CButton m_CheckBoxSide;

	afx_msg void OnSide();
	afx_msg void OnSelchangeQuoteCondition();
	afx_msg void OnSelchangeBidMarketCenter();
	afx_msg void OnSelchangeAskMarketCenter();
	afx_msg void OnSelchangeMarketListing();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);

	DECLARE_MESSAGE_MAP()

	unsigned char m_quoteCondition;
	unsigned char m_marketListing;
	unsigned char m_bidMarketCenter;
	unsigned char m_askMarketCenter;
	Price m_bid;
	Price m_ask;
	unsigned int m_bidSize;
	unsigned int m_askSize;
	bool m_side;

#ifndef TAKION_NO_OPTIONS
	TMsgOptLevel1 m_messageOptionLevel1;
	TMsgFutLevel1 m_messageFutureLevel1;
#endif
	TMsgLevel1 m_messageStockLevel1;
	TMsgMsLevel1Long m_messageEquityLevel1Long;
	TMsgMsLevel1Short m_messageEquityLevel1Short;

	TSmMsgLevel1 m_messageStockLevel1Sm;
	TSmMsgMsLevel1Long m_messageEquityLevel1LongSm;
	TSmMsgMsLevel1Short m_messageEquityLevel1ShortSm;
};