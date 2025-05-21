#pragma once

#include "BaseDialogSettingSimulateSymbolMap.h"

enum BaseDialogSettingSimulateOptionIds
{
/*
#ifndef TAKION_NO_OPTIONS
	groupOptionId = baseSimulateSymbolMapIdsCount,
	checkBoxOptionId,
	radioOptionCallId,
	radioOptionPutId,
	comboBoxOptionSuffixId,
	staticOptionSuffixId,
	spinOptionStrikePriceId,
	staticOptionStrikePriceId,
	datetimeOptionExpirationId,
	staticOptionExpirationId,
	baseSimulateOptionIdsCount
#else
*/
	baseSimulateOptionIdsCount = baseSimulateSymbolMapIdsCount,
//#endif
};

class BaseDialogSettingSimulateOption : public BaseDialogSettingSimulateSymbolMap
{
public:
#ifndef TAKION_NO_OPTIONS
	unsigned int GetExpirationDate() const{return (unsigned int)m_expirationYear * 10000 + (unsigned int)m_expirationMonth * 100 + m_expirationDay;}
	virtual bool isOptionModeUsed() const;
	virtual bool isOptionUsed() const{return m_optionMode && isOptionModeUsed();}
	void SetExpirationDate(unsigned int date);
	void SetCallPut(bool call);
	void SetStrikePrice(const Price& strikePrice);
	void SetOptionValues(bool useOption, char suffix, bool call, const Price& strikePrice, unsigned int expirationDate);
#endif
protected:
	BaseDialogSettingSimulateOption(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
#ifndef TAKION_NO_OPTIONS
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void ToolTipsEnabled(bool enable) override;

	CheckBoxOwnerDraw m_CheckBoxOption;
	CButton m_RadioOptionCall;
	CButton m_RadioOptionPut;
	ComboBoxChar m_ComboBoxOptionSuffix;
	UnsignedNumberTypeSpin<unsigned int> m_SpinOptionStrikePrice;
	ColorDateTimeCtrl m_DateTimeOptionExpiration;
	afx_msg void OnCalendarDropdown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCalendarCloseup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExpirationDateChange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOption();
	afx_msg void OnOptionCall();
	afx_msg void OnOptionPut();
	afx_msg void OnSelchangeOptionSuffix();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	bool m_optionMode;
	char m_optionSuffix;
	bool m_optionCall;
	unsigned char m_expirationYear;
	unsigned char m_expirationMonth;
	unsigned char m_expirationDay;
	Price m_strikePrice;
	bool UpdateOptionExpirationValue();
	void SetOptionCall(const bool call);
	virtual bool isSendMessageButtonEnabled() const override;
	virtual bool OptionModeChanged();

	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	virtual void SymbolTypeChanged() override;
	virtual void SymbolNotUsedChanged() override;
#endif
};
