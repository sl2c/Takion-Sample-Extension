#pragma once

#include "BaseSampleDialogSettingSymbol.h"

enum BaseSampleDialogSettingOptionIds
{
#ifndef TAKION_NO_OPTIONS
	groupSymbolOptionId = baseSymbolIdsCount,
	checkBoxSymbolOptionId,
	radioSymbolOptionCallId,
	radioSymbolOptionPutId,
	comboBoxSymbolOptionSuffixId,
	staticSymbolOptionSuffixId,
	spinSymbolOptionStrikePriceId,
	staticSymbolOptionStrikePriceId,
	datetimeSymbolOptionExpirationId,
	staticSymbolOptionExpirationId,
	baseSymbolOptionIdsCount
#else
	baseSymbolOptionIdsCount = baseSymbolIdsCount,
#endif
};

class BaseSampleDialogSettingOption : public BaseSampleDialogSettingSymbol
{
public:
#ifndef TAKION_NO_OPTIONS
	unsigned int GetExpirationDate() const{return (unsigned int)m_expirationYear * 10000 + (unsigned int)m_expirationMonth * 100 + m_expirationDay;}
	virtual bool isOptionModeUsed() const{return true;}
	virtual bool isOptionUsed() const{return m_optionMode;}
	virtual bool AreSettingsValid() const override;
	bool AreOptionSettingsValid() const;
	bool isExpirationValid() const;
	bool GetOptionValues(unsigned __int64& rootSymbol, unsigned __int64& underlierSymbol, unsigned __int64& optionBlock, unsigned short& optionType) const;
	void HideOptionControls(bool hide);
	void SetExpirationDate(unsigned int date);
	void SetCallPut(bool call);
	void SetStrikePrice(const Price& strikePrice);
	void SetOptionValues(bool useOption, char suffix, bool call, const Price& strikePrice, unsigned int expirationDate);
#endif
protected:
	BaseSampleDialogSettingOption(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, bool useAccount, const char* label, int accountWidth, int accountTopOffset);
#ifndef TAKION_NO_OPTIONS
	virtual void ToolTipsEnabled(bool enable) override;

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	CButton m_GroupOption;
	CStatic m_StaticOptionSuffix;
	CStatic m_StaticOptionStrikePrice;
	CStatic m_StaticOptionExpiration;

	CheckBoxOwnerDraw m_CheckBoxOption;
	RadioGroup m_RadioGroupOption;
	RadioOwnerDraw m_RadioOptionCall;
	RadioOwnerDraw m_RadioOptionPut;
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
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	bool m_optionMode;
	char m_optionSuffix;
	bool m_optionCall;
	unsigned char m_expirationYear;
	unsigned char m_expirationMonth;
	unsigned char m_expirationDay;
	Price m_strikePrice;
	bool UpdateOptionExpirationValue();
	void SetOptionCall(const bool call);
//	virtual bool isSendMessageButtonEnabled() const override;

	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	virtual bool OptionModeChanged();
	virtual void OptionValuesChanged(){}
//	virtual void SymbolTypeChanged() override;
#endif
};
