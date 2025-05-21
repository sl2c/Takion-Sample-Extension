#pragma once

#include "BaseDialogSettingSimulateAccount.h"

enum BaseDialogSettingSimulateAccountRiskIds
{
//	comboBoxRiskAccountId = baseSimulateMessageIdsCount,
	listBoxConstraintId = baseSimulateAccountIdsCount,
	editSearchConstraintId,
	buttonSearchConstraintId,
	buttonNullifyConstraintId,
	buttonResetConstraintId,

	spinConstraintAccountBalanceId,
	staticConstraintAccountBalanceId,

	baseSimulateAccountRiskIdsCount
};

class AccountConstraint;

class SampleDialogSettingSimulateAccountRisk : public BaseDialogSettingSimulateAccount//public BaseDialogSettingSimulateMessage, public Observer
{
public:
	SampleDialogSettingSimulateAccountRisk(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);//, const char* label);
//	virtual ~SampleDialogSettingSimulateAccountRisk(){DestroyZeroAccountConstraint();}
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;
	virtual void UpdateControls() override;
//	virtual void Leaving() override;
	bool isAccountConstraintModified(const AccountConstraint* ac) const;
/*
	void CurrentAccountSet(const Account* account);
	const Account* GetAccount() const{return m_currentAccount;}
	bool isHistoryLoaded() const{return m_historyLoaded;}
	bool isExecutorConnected() const{return m_executorConnected;}
*/
protected:
//	virtual void InitControls() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
//	virtual void SettingPageDestroyed() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void BeforeDestroy() override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	virtual void ToolTipsEnabled(bool enable) override;

//	ComboBoxAccount m_ComboBoxAccount;

	SignedNumberTypeSpin<__int64> m_SpinAccountBalance;

	ListBoxConstraint m_ListBoxConstraint;

	EditNoContext m_EditSearch;
	CButton m_ButtonSearch;
	CButton m_ButtonNullify;
	CButton m_ButtonResetConstraint;

	void EnableResetCustomButton();
	bool SearchItems();
	unsigned char ProcessKeyInSearch(MSG* pMsg);
	CString m_searchStr;
	bool m_searchDone;

//	void DestroyZeroAccountConstraint();
	ConstraintCollection m_zeroAccountConstraint;

//	afx_msg void OnSelchangeAccount();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSearchChange();
	afx_msg void OnSearch();
	afx_msg void OnNullify();
	afx_msg void OnResetConstraint();
	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);
	afx_msg LRESULT OnListBoxValueSpinned(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()
/*
	void AccountAdded(const Account* account);
	void AccountSelected();
*/
	virtual void AccountChanged();
	void SetItems(bool reset);

	int m_leftOffset;
	int m_topOffset;
	int m_listBoxBorderSize;
/*
	void SetHistoryLoaded(const bool loaded);
	void SetExecutorConnected(const bool connected);

	Account* m_currentAccount;
	char m_accountId[MAX_ACCOUNT_SIZE];
	int m_accountSel;
	bool m_historyLoaded;
	bool m_executorConnected;
*/
	virtual void RestoreLayout() override;
	virtual bool isSendMessageButtonEnabled() const override;
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	TMsgNewAccount m_messageAccount;

//	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
