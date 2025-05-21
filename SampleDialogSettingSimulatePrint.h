#pragma once

//#include "BaseDialogSettingSimulateOptionSource.h"
#include "BaseDialogSettingSimulatePrint.h"
/*
enum SampleSettingSimulatePrintIds
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

	simulatePrintIdsCount
};
*/
enum PrintMessageType : unsigned char
{
	PMT_PRINT,
	PMT_ERROR,
	PMT_CORRECTION,

	PMT_Count
};

enum SampleSettingSimulatePrintIds
{
	radioTypePrintId = basePrintIdsCount,
	radioTypeErrorId,
	radioTypeCorrectionId,

	spinSimPrintLastPriceId,
	staticSimPrintLastPriceId,

	spinSimPrintHighPriceId,
	staticSimPrintHighPriceId,

	spinSimPrintLowPriceId,
	staticSimPrintLowPriceId,

	spinSimPrintVolumeId,
	staticSimPrintVolumeId,

	editSimPrintCorrRefId,
	staticSimPrintCorrRefId,

	spinSimPrintCorrPriceId,
	staticSimPrintCorrPriceId,
	spinSimPrintCorrSizeId,
	staticSimPrintCorrSizeId,
	comboPrintCorrCondition1Id,
	staticPrintCorrCondition1Id,
	comboPrintCorrCondition2Id,
	staticPrintCorrCondition2Id,
	comboPrintCorrCondition3Id,
	staticPrintCorrCondition3Id,
	comboPrintCorrCondition4Id,
	staticPrintCorrCondition4Id,

	simulatePrintIdsCount
};

class SampleDialogSettingSimulatePrint : public BaseDialogSettingSimulatePrint
{
public:
	SampleDialogSettingSimulatePrint(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
	virtual bool isModified() const override{return BaseDialogSettingSimulatePrint::isModified() || m_EditRefCorr.isModified();}

//	virtual UIntSetting* GetSettingMillisecondEntry(){return nullptr;}

	virtual UCharSetting* GetSettingSymbolType() override;
	virtual StringSetting* GetSettingSymbol() override;
	virtual UShortSetting* GetSettingSymbolMap() override;

	virtual UCharSetting* GetSettingPrintSource() override;

#ifndef TAKION_NO_OPTIONS
	virtual BoolSetting* GetSettingUseOption() override;
	virtual BoolSetting* GetSettingOptionCall() override;
	virtual CharSetting* GetSettingOptionSuffix() override;
	virtual PriceSetting* GetSettingOptionStrikePrice() override;
	virtual UIntSetting* GetSettingOptionExpiration() override;
#endif
	virtual UInt64HexSetting* GetSettingRefNumber() override;
	virtual PriceSetting* GetSettingPrintPrice() override;
	virtual UIntSetting* GetSettingPrintSize() override;
	virtual CharSetting* GetSettingPrintSaleCondition1() override;
	virtual CharSetting* GetSettingPrintSaleCondition2() override;
	virtual CharSetting* GetSettingPrintSaleCondition3() override;
	virtual CharSetting* GetSettingPrintSaleCondition4() override;
	virtual CharSetting* GetSettingPrintMarketCenter() override;
	virtual CharSetting* GetSettingStockMarketListing() override;
	virtual BoolSetting* GetSettingPrintRefAutoIncrement() override;
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
/*
	ValidEdit m_EditRef;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPrice;
	UnsignedNumberTypeSpin<unsigned int> m_SpinSize;

	ComboBoxChar m_ComboBoxSaleCondition1;
	ComboBoxChar m_ComboBoxSaleCondition2;
	ComboBoxChar m_ComboBoxSaleCondition3;
	ComboBoxChar m_ComboBoxSaleCondition4;

	ComboBoxChar m_ComboBoxMarketCenter;
	ComboBoxChar m_ComboBoxMarketListing;

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRefUpdate();
	afx_msg void OnRefChange();
	afx_msg void OnSelchangeSaleCondition1();
	afx_msg void OnSelchangeSaleCondition2();
	afx_msg void OnSelchangeSaleCondition3();
	afx_msg void OnSelchangeSaleCondition4();
	afx_msg void OnSelchangeMarketCenter();
	afx_msg void OnSelchangeMarketListing();

	std::string m_refNumber;

	Price m_price;
	unsigned int m_size;

	unsigned char m_saleCondition1;
	unsigned char m_saleCondition2;
	unsigned char m_saleCondition3;
	unsigned char m_saleCondition4;
	unsigned char m_marketCenter;
	unsigned char m_marketListing;
*/
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnTypePrint();
	afx_msg void OnTypeError();
	afx_msg void OnTypeCorrection();

	afx_msg void OnRefUpdateCorr();
	afx_msg void OnRefChangeCorr();
	afx_msg void OnSelchangeSaleCondition1Corr();
	afx_msg void OnSelchangeSaleCondition2Corr();
	afx_msg void OnSelchangeSaleCondition3Corr();
	afx_msg void OnSelchangeSaleCondition4Corr();

	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	CButton m_RadioTypePrint;
	CButton m_RadioTypeError;
	CButton m_RadioTypeCorrection;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinLastPrice;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinHighPrice;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinLowPrice;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinVolume;

	virtual void MessageSent() override;
	IncrementalHexEdit m_EditRefCorr;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceCorr;
	UnsignedNumberTypeSpin<unsigned int> m_SpinSizeCorr;

	ComboBoxChar m_ComboBoxSaleCondition1Corr;
	ComboBoxChar m_ComboBoxSaleCondition2Corr;
	ComboBoxChar m_ComboBoxSaleCondition3Corr;
	ComboBoxChar m_ComboBoxSaleCondition4Corr;

	Price m_lastPrice;
	Price m_highPrice;
	Price m_lowPrice;
	unsigned __int64 m_volume;

	void OnPrintMessageType();
	bool SetMessageType(const unsigned char messageType);
	unsigned char m_printMessageType;

	std::string m_refNumberCorr;

	Price m_priceCorr;
	unsigned int m_sizeCorr;

	unsigned char m_saleCondition1Corr;
	unsigned char m_saleCondition2Corr;
	unsigned char m_saleCondition3Corr;
	unsigned char m_saleCondition4Corr;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	virtual bool isSendMessageButtonEnabled() const override;

#ifndef TAKION_NO_OPTIONS
	TMsgOptTrade m_messageOptionPrint;
	TMsgFutTrade m_messageFuturePrint;
#endif
	TMsgTrade m_messageStockPrint;
	TMsgMsTradeLong m_messageEquityPrintLong;
	TMsgMsTradeShort m_messageEquityPrintShort;

	TSmMsgTrade m_messageStockPrintSm;
	TSmMsgMsTradeLong m_messageEquityPrintLongSm;
	TSmMsgMsTradeShort m_messageEquityPrintShortSm;

	TMsgTradeError m_messageStockPrintError;
	TMsgMsTradeError m_messageEquityPrintError;

	TSmMsgTradeError m_messageStockPrintErrorSm;
	TSmMsgMsTradeError m_messageEquityPrintErrorSm;

	TMsgTradeCorrection m_messageStockPrintCorrection;
	TMsgMsTradeCorrection m_messageEquityPrintCorrection;

	TSmMsgTradeCorrection m_messageStockPrintCorrectionSm;
	TSmMsgMsTradeCorrection m_messageEquityPrintCorrectionSm;
};
