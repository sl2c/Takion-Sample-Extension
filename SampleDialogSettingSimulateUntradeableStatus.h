#pragma once

#include "BaseDialogSettingSimulateAccountSymbol.h"

enum SampleSettingSimulateUntradeablStatusIds
{
	//	spinSimQuotePriceId = baseSimulatePositionIdsCount,
	buttonUsFillId = baseSimulateAccountSymbolIdsCount,
//	staticSlRequestIdId,

	editUsFirmId,
	staticUsFirmId,

	editUsTraderId,
	staticUsTraderId,

	comboboxUsStatusId,
	staticUsStatusId,

/*
	spinSlBorrowPriceId,
	staticSlBorrowPriceId,

	spinSlBorrowSizeId,
	staticSlBorrowSizeId,

	spinSlBorrowDiscountId,
	staticSlBorrowDiscountId,

	comboboxSlHtbId,
	staticSlHtbId,

	spinSlBetaId,
	staticSlBetaId,

	spinSlMarginId,
	staticSlMarginId,
*/
	simulateUntradeablStatusIdsCount
};

class SampleDialogSettingSimulateUntradeablStatus : public BaseDialogSettingSimulateAccountSymbol
{
public:
	SampleDialogSettingSimulateUntradeablStatus(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	virtual bool isSendMessageButtonEnabled() const override;

	//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnFill();
	afx_msg void OnStockLoanFirmUpdate();
	afx_msg void OnStockLoanFirmChange();
	afx_msg void OnTraderIdUpdate();
	afx_msg void OnTraderIdChange();
	afx_msg void OnSelchangeStatus();
	//	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

//	UnsignedNumberTypeSpin<unsigned int> m_SpinRequestId;

	CButton m_ButtonFill;

	ValidEdit m_EditStockLoanFirm;
	ValidEdit m_EditTraderId;

	ComboBoxString m_ComboBoxUntradeableStatus;

/*
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinBorrowPrice;
	UnsignedNumberTypeSpin<unsigned int> m_SpinBorrowSize;
	UnsignedNumberTypeSpin<unsigned char> m_SpinBorrowDiscount;
	ComboBoxChar m_ComboBoxHtb;

	SignedNumberTypeSpin<__int64> m_SpinBeta;
	UnsignedNumberTypeSpin<unsigned char> m_SpinMargin;
*/
	std::string m_stockLoanFirm;
	std::string m_traderId;

	unsigned char m_untradeableStatus;

	virtual void SymbolChanged() override;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;
	TMsgFirmUntradeableStatusUpdate m_msg;
//	BorrowNode m_borrowNode;
};
