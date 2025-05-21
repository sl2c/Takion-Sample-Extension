#pragma once

#include "BaseDialogSettingSimulateMessage.h"

enum BaseDialogSettingSimulateAccountIds
{
	comboBoxAccountAccountId = baseSimulateMessageIdsCount,
	groupAccountAccountId,

	baseSimulateAccountIdsCount
};

class BaseDialogSettingSimulateAccount : public BaseDialogSettingSimulateMessage, public Observer
{
public:
	void CurrentAccountSet(Account* account);
	Account* GetAccount(){return m_account;}
	const bool& isHistoryLoaded() const{return m_historyLoaded;}
	const bool& isUntradeableStatusLoaded() const{return m_untradeableStatusLoaded;}
	const bool& isRestrictedSymbolListLoaded() const{return m_restrictedSymbolListLoaded;}
	const bool& isExecutorConnected() const{return m_executorConnected;}
	const bool& isExecutorLogged() const{return m_executorLogged;}
	virtual HWND GetFirstTabControl() const{return m_ComboBoxAccount.m_hWnd;}
	virtual bool isSendMessageButtonEnabled() const;
	void UpdateAccountInConfig(StringSetting& accountId) const;
	void SetAccountById(const char* accountId);
protected:
	BaseDialogSettingSimulateAccount(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label,
		bool accountControlNextToSend = false);
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;

	afx_msg void OnSelchangeAccount();
//	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	virtual void AccountChanged(){}
	ComboBoxAccount m_ComboBoxAccount;

	void UpdateComboBoxAccount();
	void SetAccount(Account* account);
	void AddAccount(Account* account);
	void SetHistoryLoaded(const bool loaded);
	void SetUntradeableStatusLoaded(const bool loaded);
	void SetRestrictedSymbolListLoaded(const bool loaded);
	void SetExecutorConnected(const bool connected);
	void SetExecutorLogged(const bool logged);

	Account* m_account;
	char m_accountId[MAX_ACCOUNT_SIZE];
	int m_accountSel;

	int m_comboBoxAccountWidth;

	bool m_historyLoaded;
	bool m_untradeableStatusLoaded;
	bool m_restrictedSymbolListLoaded;
	bool m_executorConnected;
	bool m_executorLogged;

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
