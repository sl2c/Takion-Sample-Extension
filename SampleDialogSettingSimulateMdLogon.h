#pragma once

#include "BaseDialogSettingSimulateMessage.h"

enum SampleSettingSimulateMdLogonIds
{
	editMdLogonUserId = baseSimulateMessageIdsCount,
	staticMdLogonUserId,
	ipMdLogonVersionId,
	staticMdLogonVersionId,

	buttonMdLogonInitializeId,

	comboBoxMdLogonCompressionId,
	staticMdLogonCompressionId,
	comboBoxMdLogonInstrumentId,
	staticMdLogonInstrumentId,

	checkBoxMdLogonSubscriptionOnlyId,
	checkBoxMdLogonForseSubscriptionOnlyId,
	checkBoxMdLogonDisableExtensionsId,

	comboBoxMdLogonResultId,
	staticMdLogonResultId,

	staticMdLogonEntitlementsId,
	listBoxMdLogonEntitlementsId,

	staticMdLogonExtEntitlementsId,
	listBoxMdLogonExtEntitlementsId,

	simulateMdLogonIdsCount
};

class SampleDialogSettingSimulateMdLogon : public BaseDialogSettingSimulateMessage
{
public:
	SampleDialogSettingSimulateMdLogon(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual bool isSendMessageButtonEnabled() const{return m_versionValid && !m_userId.empty();}
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	void SetIpVersion(const unsigned int version);

//	afx_msg void OnSelchangeMarketStatus();
//	afx_msg void OnSymbolMapping();

	afx_msg void OnIpVersionChange();
	afx_msg void OnInitialize();
	afx_msg void OnUserIdChange();
	afx_msg void OnUserIdUpdate();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

	ValidEdit m_EditUserId;
	ColorIPAddrCtrl m_IpVersion;

	ComboBoxString m_ComboBoxCompression;
	ComboBoxString m_ComboBoxInstrument;
	ComboBoxString m_ComboBoxResult;

	CButton m_CheckBoxSubscriptionOnly;
	CButton m_CheckBoxForceSubscriptionOnly;
	CButton m_CheckBoxDisableExtensions;

//	CButton m_CheckBoxSymbolMapping;

//	CStatic m_StaticTradingState;
	ListBoxFilterStringSameColor<unsigned __int64> m_ListBoxEntitlements;
	ListBoxFilterStringSameColor<unsigned __int64> m_ListBoxExtEntitlements;

	static void SaveComboBoxSelection(const ComboBoxString& comboBox, UIntSetting& uintSetting);
	static void RestoreComboBoxSelection(ComboBoxString& comboBox, const UIntSetting& uintSetting);

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;

	static void FillListBoxEntitlements(ListBoxFilterStringSameColor<unsigned __int64>& listBox);

//	char m_marketStatus;
//	bool m_symbolMapping;

	void IpChange(ColorIPAddrCtrl& ipControl, bool& valid, DWORD& ipAddress);
	std::string m_userId;
	DWORD m_version;
	bool m_versionValid;

	TMsgMdLogonResponse m_msg;
};
