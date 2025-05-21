#pragma once

#include "BaseDialogSettingSimulateOptionSource.h"
#include "IncrementalHexEdit.h"

enum BaseSettingSimulatePrintIds
{
	editSimPrintRefId = baseSimulateOptionSourceIdsCount,
	staticSimPrintRefId,

	spinSimPrintPriceId,
	staticSimPrintPriceId,
	spinSimPrintSizeId,
	staticSimPrintSizeId,
	comboPrintCondition1Id,
	staticPrintCondition1Id,
	comboPrintCondition2Id,
	staticPrintCondition2Id,
	comboPrintCondition3Id,
	staticPrintCondition3Id,
	comboPrintCondition4Id,
	staticPrintCondition4Id,

	comboPrintMarketCenterId,
	staticPrintMarketCenterId,

	comboPrintMarketListingId,
	staticPrintMarketListingId,

	basePrintIdsCount
};

class BaseDialogSettingSimulatePrint : public BaseDialogSettingSimulateOptionSource
{
public:
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;

	virtual bool isModified() const override{return BaseDialogSettingSimulateOptionSource::isModified() || m_EditRef.isModified();}
	virtual UIntSetting* GetSettingMillisecondEntry(){return nullptr;}

	virtual UCharSetting* GetSettingSymbolType() = 0;//{return nullptr;}
	virtual StringSetting* GetSettingSymbol() = 0;//{return nullptr;}
	virtual UShortSetting* GetSettingSymbolMap() = 0;//{return nullptr;}

	virtual UCharSetting* GetSettingPrintSource() = 0;//{return nullptr;}

#ifndef TAKION_NO_OPTIONS
	virtual BoolSetting* GetSettingUseOption() = 0;//{return nullptr;}
	virtual BoolSetting* GetSettingOptionCall() = 0;//{return nullptr;}
	virtual CharSetting* GetSettingOptionSuffix() = 0;//{return nullptr;}
	virtual PriceSetting* GetSettingOptionStrikePrice() = 0;//{return nullptr;}
	virtual UIntSetting* GetSettingOptionExpiration() = 0;//{return nullptr;}
#endif
	virtual UInt64HexSetting* GetSettingRefNumber() = 0;//{return nullptr;}
	virtual PriceSetting* GetSettingPrintPrice() = 0;//{return nullptr;}
	virtual UIntSetting* GetSettingPrintSize() = 0;//{return nullptr;}
	virtual CharSetting* GetSettingPrintSaleCondition1() = 0;//{return nullptr;}
	virtual CharSetting* GetSettingPrintSaleCondition2() = 0;//{return nullptr;}
	virtual CharSetting* GetSettingPrintSaleCondition3() = 0;//{return nullptr;}
	virtual CharSetting* GetSettingPrintSaleCondition4() = 0;//{return nullptr;}
	virtual CharSetting* GetSettingPrintMarketCenter() = 0;//{return nullptr;}
	virtual CharSetting* GetSettingStockMarketListing() = 0;//{return nullptr;}
	virtual BoolSetting* GetSettingPrintRefAutoIncrement() = 0;//{return nullptr;}
protected:
	BaseDialogSettingSimulatePrint(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	IncrementalHexEdit m_EditRef;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPrice;
	UnsignedNumberTypeSpin<unsigned int> m_SpinSize;

	ComboBoxChar m_ComboBoxSaleCondition1;
	ComboBoxChar m_ComboBoxSaleCondition2;
	ComboBoxChar m_ComboBoxSaleCondition3;
	ComboBoxChar m_ComboBoxSaleCondition4;

	ComboBoxChar m_ComboBoxMarketCenter;
	ComboBoxChar m_ComboBoxMarketListing;

	afx_msg void OnRefUpdate();
	afx_msg void OnRefChange();
	afx_msg void OnSelchangeSaleCondition1();
	afx_msg void OnSelchangeSaleCondition2();
	afx_msg void OnSelchangeSaleCondition3();
	afx_msg void OnSelchangeSaleCondition4();
	afx_msg void OnSelchangeMarketCenter();
	afx_msg void OnSelchangeMarketListing();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	virtual void MessageSent() override;
	std::string m_refNumber;

	Price m_price;
	unsigned int m_size;

	unsigned char m_saleCondition1;
	unsigned char m_saleCondition2;
	unsigned char m_saleCondition3;
	unsigned char m_saleCondition4;
	unsigned char m_marketCenter;
	unsigned char m_marketListing;

	int m_refNumberY;

//	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	virtual bool isSendMessageButtonEnabled() const override;
/*
#ifndef TAKION_NO_OPTIONS
	TMsgOptTrade m_messageOptionPrint;
#endif
	TMsgTrade m_messageStockPrint;
	TMsgMsTradeLong m_messageEquityPrintLong;
	TMsgMsTradeShort m_messageEquityPrintShort;

	TSmMsgTrade m_messageStockPrintSm;
	TSmMsgMsTradeLong m_messageEquityPrintLongSm;
	TSmMsgMsTradeShort m_messageEquityPrintShortSm;
*/
};