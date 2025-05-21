#pragma once

#include "BaseDialogSettingSimulateSource.h"

enum BaseDialogSettingSimulateImbalanceIds
{
	comboBoxImbalanceBookId = baseSimulateSourceIdsCount,
	staticImbalanceBookId,

	comboBoxImbalanceTypeId,
	staticImbalanceTypeId,

	comboBoxImbalanceDirectionId,
	staticImbalanceDirectionId,

	checkBoxSimRegulatoryId,
	checkBoxSimFreezeStatusId,

	spinSimImbalanceId,
	staticSimImbalanceId,
	spinSimMarketImbalanceId,
	staticSimMarketImbalanceId,
	spinSimPairedSharesId,
	staticSimPairedSharesId,

	spinSimImbalancePriceFarId,
	staticSimImbalancePriceFarId,
	spinSimImbalancePriceNearId,
	staticSimImbalancePriceNearId,
	spinSimImbalancePriceRefId,
	staticSimImbalancePriceRefId,

	comboBoxPriceVariationIndicatorId,
	staticPriceVariationIndicatorId,

	spinAuctionTimeId,
	staticAuctionTimeId,

	simulateImbalanceIdsCount
};

class SampleDialogSettingSimulateImbalance : public BaseDialogSettingSimulateSource
{
public:
	SampleDialogSettingSimulateImbalance(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
//	static void SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
//	static void SetSignedSizeSpinCells(SignedNumberTypeSpin<int>& spin);
//	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	ComboBoxString m_ComboBoxBookId;
	ComboBoxString m_ComboBoxImbalanceType;
	ComboBoxString m_ComboBoxImbalanceDirection;

	CButton m_CheckBoxRegulatory;
	CButton m_CheckBoxFreezeStatus;

//	SignedNumberTypeSpin<int> m_SpinImbalance;
//	SignedNumberTypeSpin<int> m_SpinMarketImbalance;
	UnsignedNumberTypeSpin<unsigned int> m_SpinImbalance;
	UnsignedNumberTypeSpin<unsigned int> m_SpinMarketImbalance;
	UnsignedNumberTypeSpin<unsigned int> m_SpinPairedShares;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceFar;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceNear;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceRef;

	ComboBoxChar m_ComboBoxPriceVariationIndicator;
	TimeSpin m_SpinAuctionTime;

	afx_msg void OnSelchangeBookId();
	afx_msg void OnSelchangeImbalanceType();
	afx_msg void OnSelchangeImbalanceDirection();
	afx_msg void OnSelchangePriceVariationIndicator();
	afx_msg void OnRegulatory();
	afx_msg void OnFreezeStatus();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

//	virtual void SymbolTypeChanged() override;
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	TMsgImbalance m_msgMd;
	TSmMsgImbalance m_msgMdSm;
	TMsgMsImbalanceShort m_msgShortMs;
	TSmMsgMsImbalanceShort m_msgShortMsSm;
	TMsgMsImbalanceLong m_msgLongMs;
	TSmMsgMsImbalanceLong m_msgLongMsSm;

//	void AddPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);

	enum BookType : unsigned char
	{
		BT_NYSE,
		BT_AMEX,
		BT_ARCA,
		BT_NSDQ,
		BT_BATS,
		BT_BATY,
//		BT_IEX,

		BT_Count
	};

	enum ImbalanceType : unsigned char
	{
		IT_OPENING,
		IT_CLOSING,
		IT_HALT_OR_IPO,
		IT_MARKET,
		IT_UNDEFINED,

		IT_Count
	};

	enum ImbalanceDirection : unsigned char
	{
		ID_BUY,
		ID_SELL,
		ID_NONE,
		ID_UNDEFINED,

		ID_Count
	};

	static unsigned char GetImbalanceDirectionStatic(unsigned char imbalanceDirection)
	{
		switch(imbalanceDirection)
		{
			case ID_BUY:
			return ImbalanceBuy;

			case ID_SELL:
			return ImbalanceSell;

			case ID_NONE:
			return ImbalanceNone;

			default:
			return ImbalanceUndefined;
		}
	}
	unsigned char GetImbalanceDirection() const
	{
		return GetImbalanceDirectionStatic(m_imbalanceDirection);
	}

	static unsigned char GetImbalanceTypeStatic(unsigned char imbalanceType)
	{
		switch(imbalanceType)
		{
			case IT_OPENING:
			return Opening;

			case IT_CLOSING:
			return Closing;

			case IT_HALT_OR_IPO:
			return HaltOrIPO;

			case IT_MARKET:
			return Market;

			default:
			return CrossUndefined;
		}
	}

	unsigned char GetImbalanceType() const
	{
		return GetImbalanceTypeStatic(m_imbalanceType);
	}

	static unsigned char GetImbalanceBookIdStatic(unsigned char bookId)
	{
		switch(bookId)
		{
			case BT_NYSE:
			return ECNBOOK_NYS;

			case BT_AMEX:
			return ECNBOOK_AMEX;

			case BT_ARCA:
			return ECNBOOK_ARCA;

			case BT_NSDQ:
			return ECNBOOK_NSDQ;

			case BT_BATS:
			return ECNBOOK_BATS;

			case BT_BATY:
			return ECNBOOK_BATY;

//			case BT_IEX:
//			return ECNBOOK_IEX;

			default:
			return 0;
		}
	}

	unsigned char GetImbalanceBookId() const
	{
		return GetImbalanceBookIdStatic(m_bookId);
	}

	unsigned char m_bookId;
	unsigned char m_imbalanceType;
	unsigned char m_imbalanceDirection;
	bool m_regulatory;
	bool m_freezeStatus;
	char m_priceVariationIndicator;
	Price m_priceFar;
	Price m_priceNear;
	Price m_priceRef;

	unsigned int m_imbalance;
	unsigned int m_marketImbalance;
	unsigned int m_pairedShares;
	unsigned int m_auctionTimeSeconds;
};
