#pragma once

#include "BaseDialogSettingSimulateFirm.h"

enum SampleSettingSimulateStockLoanInfoIds
{
/*
	spinSlRequestIdId = baseSimulateAccountSymbolIdsCount,
	staticSlRequestIdId,

	buttonSlFillId,

	editSlFirmId,
	staticSlFirmId,
*/
	spinSlBorrowPriceId = baseSimulateFirmIdsCount,
	staticSlBorrowPriceId,

	spinSlBorrowSizeId,
	staticSlBorrowSizeId,

	spinSlBorrowDiscountId,
	staticSlBorrowDiscountId,

	comboboxSlHtbId,
	staticSlHtbId,

	checkBoxSlOnTheFlyId,
	checkBoxBorrowId,

	spinSlBetaId,
	staticSlBetaId,

	spinSlMarginId,
	staticSlMarginId,


	spinMePmElRateId,
	staticMePmElRateId,

	spinMePmEsRateId,
	staticMePmEsRateId,

	spinMePmHlRateId,
	staticMePmHlRateId,

	spinMePmHsRateId,
	staticMePmHsRateId,

	spinMeElRateId,
	staticMeElRateId,

	spinMeEsRateId,
	staticMeEsRateId,

	spinMeHlRateId,
	staticMeHlRateId,

	spinMeHsRateId,
	staticMeHsRateId,

	spinMeFedReqId,
	staticMeFedReqId,

	comboboxMeAssetTypeId,
	staticMeAssetTypeId,

	checkBoxMeMarginableId,

	comboboxMeSecTypeId,
	staticMeSecTypeId,

	comboboxMeStatusId,
	staticMeStatusId,

	simulateStockLoanInfoIdsCount
};

class SampleDialogSettingSimulateStockLoanInfo : public BaseDialogSettingSimulateFirm
{
public:
	SampleDialogSettingSimulateStockLoanInfo(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;

//	static void SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
//	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
//	static void SetMoneySpinCells(SignedNumberTypeSpin<__int64>& spin);

	virtual StringSetting* GetSettingFirm() override;
	virtual UIntSetting* GetSettingRequestId() override;
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	virtual bool isSendMessageButtonEnabled() const override;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg void OnFill();
//	afx_msg void OnStockLoanFirmUpdate();
//	afx_msg void OnStockLoanFirmChange();
	afx_msg void OnOnTheFly();
	afx_msg void OnOnTheFlyBorrow();
	afx_msg void OnMarginable();
	afx_msg void OnSelchangeHtb();
	afx_msg void OnSelchangeAssetType();
	afx_msg void OnSelchangeSecType();
	afx_msg void OnSelchangeStatus();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

//	UnsignedNumberTypeSpin<unsigned int> m_SpinRequestId;
//	CButton m_ButtonFill;
//	ValidEdit m_EditStockLoanFirm;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinBorrowPrice;
	UnsignedNumberTypeSpin<unsigned int> m_SpinBorrowSize;
	UnsignedNumberTypeSpin<unsigned char> m_SpinBorrowDiscount;
	ComboBoxChar m_ComboBoxHtb;

	CButton m_CheckBoxOnTheFly;
	CButton m_CheckBoxBorrow;

	SignedNumberTypeSpin<__int64> m_SpinBeta;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMargin;

	UnsignedNumberTypeSpin<unsigned char> m_SpinMePmElRate;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMePmEsRate;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMePmHlRate;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMePmHsRate;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMeElRate;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMeEsRate;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMeHlRate;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMeHsRate;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMeFedReq;

	ComboBoxChar m_ComboBoxMeAssetType;
	CButton m_CheckBoxMeMarginable;
	ComboBoxChar m_ComboBoxMeSecType;
	ComboBoxChar m_ComboBoxMeStatus;

//	std::string m_stockLoanFirm;

	void UpdateOnTheFly();
	void UpdateBorrow();
	bool m_onTheFly;
//	bool m_onTheFlyBorrow;
	bool m_borrow;
	bool m_marginable;
	unsigned char m_htb;
	unsigned char m_assetType;
	unsigned char m_secType;
	unsigned char m_status;

//	virtual void SymbolChanged() override;
	virtual void FillFirmInfo() override;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;
	TMsgRespLocateInfo m_msg;
	BorrowNode m_borrowNode;

	TMsgLocateInfo m_msgOnTheFlyBorrow;
	TMsgBetaInfo  m_msgOnTheFlyBetaAndMargin;

	TMsgRespHtbSymbolList m_msgHtb;
	HtbNode m_htbNode;
};
