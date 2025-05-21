#pragma once

#include "BaseDialogSettingSimulateAccountOption.h"

enum SampleSettingSimulateOrderUpdateIds
{
//	spinSimQuotePriceId = baseSimulatePositionIdsCount,
	spinOrderUpdateSizeId = baseSimulateAccountOptionIdsCount,
	staticOrderUpdateSizeId,

	spinOrderUpdateRemainingId,
	staticOrderUpdateRemainingId,

	spinOrderUpdateClientIdId,
	staticOrderUpdateClientIdId,

	spinOrderUpdateServerIdId,
	staticOrderUpdateServerIdId,

	spinOrderUpdatePriceId,
	staticOrderUpdatePriceId,

	spinOrderUpdateBidId,
	staticOrderUpdateBidId,

	spinOrderUpdateAskId,
	staticOrderUpdateAskId,

	spinOrderUpdateLastId,
	staticOrderUpdateLastId,

	comboBoxOrderUpdateSideId,
	staticOrderUpdateSideId,

	comboBoxOrderUpdateReRouteId,
	staticOrderUpdateReRouteId,

	comboBoxOrderUpdateDestinationId,
	staticOrderUpdateDestinationId,

	comboBoxOrderUpdateRoutingId,
	staticOrderUpdateRoutingId,

	editOrderUpdateMmidId,
	staticOrderUpdateMmidId,

	spinOrderUpdateReserveSizeId,
	staticOrderUpdateReserveSizeId,

	comboBoxOrderUpdateTifId,
	staticOrderUpdateTifId,

	spinOrderUpdateMillisecondId,
	staticOrderUpdateMillisecondId,

	editOrderUpdateMarketRefIdId,
	staticOrderUpdateMarketRefIdId,

	checkBoxOrderUpdateHistoricalId,
	checkBoxOrderUpdateMarketId,

//	checkBoxOrderUpdateReroutedToBamlId,

//	checkBoxBorrowAcceptId,
	comboBoxMessageTypeId,
	staticMessageTypeId,

	spinOrderUpdateReasonCodeId,
	staticOrderUpdateReasonCodeId,

	staticExecutionInstructionsId,
	listBoxExecutionInstructionsId,
//Execution
	comboBoxExecutionLiquidity1Id,
	staticExecutionLiquidity1Id,

	comboBoxExecutionLiquidity2Id,
	staticExecutionLiquidity2Id,

	spinExecutionFilledId,
	staticExecutionFilledId,

	spinExecutionCancelledId,
	staticExecutionCancelledId,

	spinExecutionRemainingId,
	staticExecutionRemainingId,

	spinOrderUpdateExecutionPriceId,
	staticOrderUpdateExecutionPriceId,

	comboBoxOrderUpdateExecutionDestinationId,
	staticOrderUpdateExecutionDestinationId,

	editOrderUpdateExecutionMmidId,
	staticOrderUpdateExecutionMmidId,

	editOrderUpdateExecutionRefIdId,
	staticOrderUpdateExecutionRefIdId,

	staticExecutionReportFlagsId,
	listBoxExecutionReportFlagsId,
//
	simulateOrderUpdateIdsCount
};

enum EnumReRoute : unsigned char
{
	RER_NONE,
	RER_BAML,
	RER_VELO,

	RER_Count
};

enum EnumOrderMessageType : unsigned char
{
	OMT_UPDATE_ORDER,
	OMT_BORROW_ACCEPT,
	OMT_REJECT_ORDER,
	OMT_REJECT_CANCEL,
	OMT_KILL_ORDER,
	OMT_EXECUTE_ORDER,

	OMT_Count
};


class SampleDialogSettingSimulateOrderUpdate : public BaseDialogSettingSimulateAccountOption
{
public:
	SampleDialogSettingSimulateOrderUpdate(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual bool isSendMessageButtonEnabled() const override;
//	static void SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
//	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
	unsigned int GetReserveQuantity() const;
	unsigned int GetTifMilliseconds() const;
	unsigned char GetTifType() const;
	void FillOrderHeader(TMessageOrderHeader& header) const;
	void FillReasonHeader(TMsgOrderReason& header) const;
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
	virtual void ToolTipsEnabled(bool enable) override;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeDestination();
	afx_msg void OnSelchangeRouting();
	afx_msg void OnMmidUpdate();
	afx_msg void OnMmidChange();
	afx_msg void OnMarketRefIdUpdate();
	afx_msg void OnMarketRefIdChange();

	afx_msg void OnHistorical();
//	afx_msg void OnReroutedToBaml();
	afx_msg void OnMarket();
//	afx_msg void OnBorrowAccept();
	afx_msg void OnSelchangeSide();
	afx_msg void OnSelchangeReRoute();
	afx_msg void OnSelchangeTif();
	afx_msg void OnSelchangeMessageType();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);

	afx_msg void OnSelchangeLiquidity1();
	afx_msg void OnSelchangeLiquidity2();
	afx_msg void OnExecutionMmidUpdate();
	afx_msg void OnExecutionMmidChange();
	afx_msg void OnExecutionRefIdUpdate();
	afx_msg void OnExecutionRefIdChange();
	afx_msg void OnSelchangeExecutionDestination();

	DECLARE_MESSAGE_MAP()

	void UpdateSpinColor();
	void UpdateSecurityPriceColor();

	void RestorePrice(const Price& src, Price& dst, UnsignedNumberTypeSpin<unsigned __int64>& spin);
	void RestoreUInt(const unsigned int& src, unsigned int& dst, UnsignedNumberTypeSpin<unsigned int>& spin);
	void RestoreUShort(const unsigned short& src, unsigned short& dst, UnsignedNumberTypeSpin<unsigned short>& spin);

	UnsignedNumberTypeSpin<unsigned int> m_SpinSize;
	UnsignedNumberTypeSpin<unsigned int> m_SpinRemaining;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPrice;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinBid;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinAsk;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinLast;

	UnsignedNumberTypeSpin<unsigned int> m_SpinClientId;
	UnsignedNumberTypeSpin<unsigned int> m_SpinServerId;

	ComboBoxString m_ComboBoxSide;
	ComboBoxString m_ComboBoxReRoute;

	ComboBoxDestination m_ComboBoxDestination;
	ComboBoxRouting m_ComboBoxRouting;
	ValidEdit m_EditMmid;
	UnsignedNumberTypeSpin<unsigned int> m_SpinReserveSize;
	CStatic m_StaticReserveSize;

	ComboBoxString m_ComboBoxTifType;
	TifSpin m_SpinTif;

	ValidEdit m_EditMarketRefId;

	CheckBoxOwnerDraw m_CheckBoxHistorical;
	CheckBoxOwnerDraw m_CheckBoxMarket;

	ComboBoxString m_ComboBoxMessageType;
//	CheckBoxOwnerDraw m_CheckBoxBorrowAccept;
	UnsignedNumberTypeSpin<unsigned short> m_SpinReasonCode;

	ListBoxFilterString<unsigned int> m_ListBoxExecutionInstructions;

//Executions
	ComboBoxChar m_ComboBoxExecutionLiquidity1;
	ComboBoxChar m_ComboBoxExecutionLiquidity2;

	UnsignedNumberTypeSpin<unsigned int> m_SpinExecutionFilled;
	UnsignedNumberTypeSpin<unsigned int> m_SpinExecutionCancelled;
	UnsignedNumberTypeSpin<unsigned int> m_SpinExecutionRemaining;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinExecutionPrice;
	ComboBoxDestination m_ComboBoxExecutionDestination;
	ValidEdit m_EditExecutionMmid;
	ValidEdit m_EditExecutionRefId;

	ListBoxFilterString<unsigned short> m_ListBoxExecutionReportFlags;
////

	void ResetValues();

	void UpdateTifType();

	bool m_historical;
//	bool m_reroutedToBaml;
	unsigned char m_reRoute;
	unsigned char m_messageType;
	bool m_market;
//	bool m_borrowAccept;
	char m_side;
	unsigned int m_size;
	unsigned int m_reserveSize;
	unsigned int m_remaining;
	unsigned int m_clientId;
	unsigned int m_serverId;
	unsigned short m_reasonCode;
	Price m_price;
	Price m_bid;
	Price m_ask;
	Price m_last;

	const Destination* m_currentDestination;
	char m_mmid[sizeof(unsigned int) + 1];
	char m_marketReferenceId[MAX_MARKET_REFERENCE_ID];

	void LiquiditySelected(ComboBoxChar& comboBox, char& liquidity);
	char m_liquidity1;
	char m_liquidity2;

	unsigned int m_executionFilled;
	unsigned int m_executionCancelled;
	unsigned int m_executionRemaining;
	Price m_executionPrice;
	const Destination* m_executionDestination;
	char m_executionMmid[sizeof(unsigned int) + 1];
	char m_executionReferenceId[MAX_MARKET_REFERENCE_ID];

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;
	TMsgOrderUpdate m_msg;
	TMessageBorrowAcceptRequest m_borrowAcceptRequest;

	TMsgOrderReject m_msgOrderReject;
	TMsgCancelReject m_msgCancelReject;
	TMsgOrderKill m_msgKillOrder;
	TMsgOrderReport m_msgOrderExecute;
};
