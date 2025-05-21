#pragma once

#include "BaseDialogSetting.h"

enum BaseDialogSettingAccountIds
{
	comboBoxAccountId = 1000,
//	groupAccountId,

	baseDialogSettingAccountIdsCount,
};

class BaseDialogSettingAccount : public BaseDialogSetting, public Observer
{
public:
//	virtual ~BaseDialogSettingAccount();
	virtual void UpdateSettings() override;
	virtual HWND GetFirstTabControl() const override{return m_ComboBoxAccount.m_hWnd;}
	virtual void RestoreLayout() override;
	virtual StringSetting* GetAccountIdSetting(){return nullptr;}

	void CurrentAccountSet(Account* account);

	Account* GetAccount() const{return m_account;}
	const char* GetAccountId() const{return m_accountId;}

	const bool& isTradingAllowed() const{return m_tradingAllowed;}
	const bool& isSuspended() const{return m_suspended;}
	const bool& isHistoryLoaded() const{return m_historyLoaded;}
	const bool& isUntradeableStatusLoaded() const{return m_untradeableStatusLoaded;}
	const bool& isRestrictedSymbolListLoaded() const{return m_restrictedSymbolListLoaded;}
	const bool& isExecutorConnected() const{return m_executorConnected;}
	const bool& isExecutorLogged() const{return m_executorLogged;}

#ifndef TAKION_NO_OPTIONS
	const bool& isFutureExecutorConnected() const{return m_futureExecutorConnected;}
	const bool& isFutureExecutorLogged() const{return m_futureExecutorLogged;}
#endif

	const bool& isTraderIdEnabled() const{return m_traderIdEnabled;}
	const bool& isTradingLockedByTrader() const{return m_tradingLockedByTrader;}
	const bool& isTradingDefaultAccountOnlyByTrader() const{return m_tradingDefaultAccountOnlyByTrader;}
	const bool& isTradingTestStocksOnlyByTrader() const{return m_tradingTestStocksOnlyByTrader;}

	void TradingLocked(bool locked);
	void TradingDefaultAccountOnly(bool defaultAccountOnly);
	void TradingTestStocksOnly(bool testStocksOnly);
protected:
	BaseDialogSettingAccount(TakionMainWnd* mainWnd,
		TakionSettingTabDialog* parentTab,
		bool useAccount,
		const char* label,
		bool includeCurrentAccount,
		int accountWidth,
		int topOffset = 0,
		bool displayAccountChoice = false);
	virtual BOOL OnInitDialog() override;
//	void AccountDestroyed(const Account* account);

	virtual void AddAccountObserver();
	virtual void RemoveAccountObserver();

	void SetAccount(Account* account);
	void AddAccount(Account* account);
//	void SetPosition(Position* position);

	void UpdateComboBoxAccount();

	virtual void AccountChanged(){}
	virtual void OnTradingAllowed(){}
	virtual void OnSuspended(){}
	virtual void OnHistoryLoaded(){}
	virtual void OnUntradeableStatusLoaded(){}
	virtual void OnRestrictedSymbolListLoaded(){}

	virtual void OnExecutorConnected(){}
	virtual void OnExecutorLogged(){}
	virtual void OnTraderIdEnabledChanged(){}
	virtual void OnTradingLockedByTrader(){}
	virtual void OnTradingDefaultAccountOnlyByTrader(){}
	virtual void OnTradingTestStocksOnlyByTrader(){}

	virtual bool ShouldAddAccountObserver() const{return false;}
	void SetAccountId(const char* accountId);
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;
	bool m_useAccount;
	Account* m_account;
	StringArea m_accountChoice;
//	const Position* m_position;
	char m_accountId[MAX_ACCOUNT_SIZE];
	bool m_includeCurrentAccount;
	bool m_displayAccountChoice;
/*
	std::string m_symbol;
	std::string m_controlSymbol;
	bool m_symbolCurrent;
*/
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
//	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
/*
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnSymbol();
	afx_msg void OnSelchangeOrder();
*/
	afx_msg void OnSelchangeAccount();

	DECLARE_MESSAGE_MAP()
/*
	ValidEdit m_EditSymbol;
	CButton m_ButtonSymbol;
	CStatic m_StaticSymbol;
*/
	CustomGroupBox m_groupBoxAccount;
	ComboBoxAccount m_ComboBoxAccount;

	int m_accountGroupHeight;
	int m_accountWidth;
	int m_bottomControl;
	int m_accountBottom;

	int m_xc;

	void SetExecutorConnected(const bool connected);
	void SetExecutorLogged(const bool logged);
	void SetTraderIdEnabled(const bool traderIdEnabled);
	void SetTradingAllowed(const bool tradingAllowed);
	void SetSuspended(const bool suspended);
	void SetHistoryLoaded(const bool loaded);
	void SetUntradeableStatusLoaded(const bool loaded);
	void SetRestrictedSymbolListLoaded(const bool loaded);
#ifndef TAKION_NO_OPTIONS
	void SetFutureExecutorConnected(const bool connected);
	void SetFutureExecutorLogged(const bool logged);
	virtual void OnFutureExecutorConnected(){}
	virtual void OnFutureExecutorLogged(){}
	bool m_futureExecutorConnected;
	bool m_futureExecutorLogged;
#endif
	bool m_executorConnected;
	bool m_executorLogged;
	bool m_tradingAllowed;
	bool m_suspended;
	bool m_historyLoaded;
	bool m_untradeableStatusLoaded;
	bool m_restrictedSymbolListLoaded;

	bool m_traderIdEnabled;
	bool m_tradingLockedByTrader;
	bool m_tradingDefaultAccountOnlyByTrader;
	bool m_tradingTestStocksOnlyByTrader;

	virtual void ProcessConnectDisconnect(const Connection* connection){}

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
