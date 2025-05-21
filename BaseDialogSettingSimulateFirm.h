#pragma once

#include "BaseDialogSettingSimulateAccountSymbol.h"

enum BaseDialogSettingSimulateFirmIds
{
	spinRequestIdId = baseSimulateAccountSymbolIdsCount,
	staticRequestIdId,

	buttonFillId,

	editFirmId,
	staticFirmId,

	baseSimulateFirmIdsCount
};

class BaseDialogSettingSimulateFirm : public BaseDialogSettingSimulateAccountSymbol
{
public:
	virtual void UpdateSettings() override;

//	static void SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);

	virtual StringSetting* GetSettingFirm() = 0;//{return nullptr;}
	virtual UIntSetting* GetSettingRequestId() = 0;//{return nullptr;}
protected:
	BaseDialogSettingSimulateFirm(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);

	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	virtual bool isSendMessageButtonEnabled() const override;

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnFill();
	afx_msg void OnStockLoanFirmUpdate();
	afx_msg void OnStockLoanFirmChange();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	UnsignedNumberTypeSpin<unsigned int> m_SpinRequestId;

	CButton m_ButtonFill;

	ValidEdit m_EditStockLoanFirm;

	std::string m_stockLoanFirm;

	virtual void SymbolChanged() override;

	virtual void RestoreLayout() override;

	virtual void FillFirmInfo();
};
