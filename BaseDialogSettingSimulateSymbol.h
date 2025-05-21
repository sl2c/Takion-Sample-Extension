#pragma once

#include "BaseDialogSettingSimulateMessage.h"

enum BaseDialogSettingSimulateSymbolIds
{
	editSimSymbolId = baseSimulateMessageIdsCount,
	radioSimSymbolId,

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

	baseSimulateSymbolIdsCount
};

enum SimSymbolType : unsigned char
{
	SYMT_SYMBOL_STRING,
	SYMT_SYMBOL_MAPPING,

	SYMT_SYMBOL_Count
};

class BaseDialogSettingSimulateSymbol : public BaseDialogSettingSimulateMessage
{
public:
//	virtual ~BaseDialogSettingSimulateSymbol(){ClearStocks();}
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;
	virtual void UpdateControls() override;
//	virtual void Leaving() override;
protected:
	BaseDialogSettingSimulateSymbol(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
//	virtual void InitControls() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;

	ValidEdit m_EditSymbol;
	CButton m_RadioSymbol;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnSymbolType();
	DECLARE_MESSAGE_MAP()

	virtual void SymbolTypeChanged();

	virtual bool isSendMessageButtonEnabled() const override;
	std::string m_symbol;
	unsigned __int64 m_numericSymbol;
	unsigned char m_symbolType;

	bool m_emptySymbolValid;
	bool m_symbolNotUsed;
	void SetSymbolNotUsed(bool symbolNotUsed);
	virtual void SymbolNotUsedChanged();
};
