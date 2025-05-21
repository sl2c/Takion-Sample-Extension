#pragma once

#include "BaseDialogSettingSimulateSymbol.h"

enum BaseDialogSettingSimulateSymbolMapIds
{
/*
	radioSimSymbolMapId = baseSimulateSymbolIdsCount,
	spinSimSymbolMapId,

#ifndef TAKION_NO_OPTIONS
	groupOptionId,
	checkBoxOptionId,
	radioOptionCallId,
	radioOptionPutId,
	comboBoxOptionSuffixId,
	staticOptionSuffixId,
	spinOptionStrikePriceId,
	staticOptionStrikePriceId,
	datetimeOptionExpirationId,
	staticOptionExpirationId,
#endif
*/
	baseSimulateSymbolMapIdsCount = baseSimulateSymbolIdsCount
};

class BaseDialogSettingSimulateSymbolMap : public BaseDialogSettingSimulateSymbol
{
public:
//	virtual void UpdateSettings() override;
//	virtual void UpdateControls() override;
protected:
	BaseDialogSettingSimulateSymbolMap(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
//	virtual void BeforeDestroy() override;

	UnsignedNumberTypeSpin<unsigned short> m_SpinSymbolMap;
	CButton m_RadioSymbolMap;

	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	void UpdateSymbolTypeColor();

	virtual void SymbolTypeChanged() override;
	unsigned short m_symbolMap;
	virtual void SymbolNotUsedChanged() override;

	virtual void UpdateMarketCenter(){}
};
