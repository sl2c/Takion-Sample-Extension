#pragma once

#include "BaseDialogSettingSimulateAccount.h"

enum BaseDialogSettingSimulateAccountSymbolIds
{
	staticSimAccountSymbolId = baseSimulateAccountIdsCount,
	editSimAccountSymbolId,
//	radioSimAccountSymbolId,
/*
	radioSimSymbolMapId,// = baseSimulateSymbolIdsCount,
	spinSimSymbolMapId,
#ifndef TAKION_NO_OPTIONS
	groupOptionId,// = baseSimulateSymbolMapIdsCount,
	checkBoxOptionId,
	radioOptionCallId,
	radioOptionPutId,
	comboBoxOptionSuffixId,
	staticOptionSuffixId,
	spinOptionStrikePriceId,
	staticOptionStrikePriceId,
	datetimeOptionExpirationId,
	staticOptionExpirationId,
//	baseSimulateOptionIdsCount
#endif
*/
	baseSimulateAccountSymbolIdsCount
};
/*
enum SimSymbolType : unsigned char
{
	SYMT_SYMBOL_STRING,
	SYMT_SYMBOL_MAPPING,

	SYMT_SYMBOL_Count
};
*/
class BaseDialogSettingSimulateAccountSymbol : public BaseDialogSettingSimulateAccount
{
public:
	virtual HWND GetFirstTabControl() const override;
//	virtual void UpdateSettings() override;
//	virtual void UpdateControls() override;
//	virtual void Leaving() override;
	virtual bool isSymbolUsed() const{return true;}
protected:
	BaseDialogSettingSimulateAccountSymbol(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
//	virtual void InitControls() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
//	virtual void BeforeDestroy() override;

	ValidEdit m_EditSymbol;
//	CButton m_RadioSymbol;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
//	afx_msg void OnSymbolType();
	DECLARE_MESSAGE_MAP()

//	virtual void SymbolTypeChanged();

	virtual void SymbolChanged(){}

	virtual bool isSendMessageButtonEnabled() const override;
	std::string m_symbol;
//	unsigned char m_symbolType;

//	bool m_emptySymbolValid;
//	bool m_symbolNotUsed;
};
