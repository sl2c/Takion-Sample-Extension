#pragma once

#include "BaseDialogSettingAccount.h"

class SampleDialogSettingAccountAction : public BaseDialogSettingAccount//, public Observer
{
public:
	SampleDialogSettingAccountAction(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
	virtual StringSetting* GetAccountIdSetting() override;
	void UpdateAccountReadyToTrade();

	bool CanSubscribeMarketSorter() const;
	bool MarketSorterSymbolDifferent() const;
	bool MarketSorterSymbolValid() const;

	bool CanSubscribeMarketData() const;
	bool MarketDataSymbolDifferent() const;
	bool MarketDataSymbolValid() const;

#ifndef TAKION_NO_OPTIONS
	unsigned int GetExpirationDate() const{return (unsigned int)m_expirationYear * 10000 + (unsigned int)m_expirationMonth * 100 + m_expirationDay;}
	unsigned int GetCurrentExpirationDate() const{return (unsigned int)m_currentExpirationYear * 10000 + (unsigned int)m_currentExpirationMonth * 100 + m_currentExpirationDay;}
//	virtual bool isOptionModeUsed() const{return true;}
//	virtual
	const bool& isOptionUsed() const{return m_optionMode;}
//	virtual 
	bool AreSettingsValid() const;// override;
	bool AreOptionSettingsValid() const;
	bool isExpirationValid() const;
	bool GetOptionValues(unsigned __int64& rootSymbol, unsigned __int64& underlierSymbol, unsigned __int64& optionBlock, unsigned short& optionType) const;
//	void HideOptionControls(bool hide);
	void SetExpirationDate(unsigned int date);
	void SetCallPut(bool call);
	void SetStrikePrice(const Price& strikePrice);
	void SetOptionValues(bool useOption, char suffix, bool call, const Price& strikePrice, unsigned int expirationDate);
#endif
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
//	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;
//	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);
	virtual void ToolTipsEnabled(bool enable) override;

	afx_msg void OnDestroyUntradedPositions();
	afx_msg void OnSetPhantomPositionsFlat();

	afx_msg void OnDestroyAllUntradedPositions();
	afx_msg void OnSetAllPhantomPositionsFlat();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSymbolMarketSorterUpdate();
	afx_msg void OnSymbolMarketSorterChange();
	afx_msg void OnSubscribeMarketSorter();
	afx_msg void OnResetMarketSorter();

	afx_msg void OnSymbolMarketDataUpdate();
	afx_msg void OnSymbolMarketDataChange();
	afx_msg void OnSubscribeMarketData();
	afx_msg void OnResetMarketData();

	DECLARE_MESSAGE_MAP()

	CButton m_ButtonDestroyUntradedPositions;
	CButton m_ButtonDestroyAllUntradedPositions;

	CButton m_ButtonSetPhantomPositionsFlat;
	CButton m_ButtonSetAllPhantomPositionsFlat;

//MarketSorter
	bool DoSubscribeMarketSorter();
	void EnableSubscribeMarketSorterButton();
	CStatic m_StaticSymbolMarketSorter;
	ValidEdit m_EditSymbolMarketSorter;
	CButton m_ButtonSubscribeMarketSorter;
	CButton m_ButtonResetMarketSorter;
	void MakeMarketSorterSymbolCurrent();
	void ResetMarketSorterSymbolCurrent();
	std::string m_symbolMarketSorter;
	std::string m_currentSymbolMarketSorter;
	const Security* m_securityMarketSorter;
	const Position* m_positionMarketSorter;
	bool SetSecurityMarketSorter(const Security* security);
	bool SetPositionMarketSorter(const Position* position);
//	void SecurityRefreshedMarketSorter();
////
//MarketData
	bool DoSubscribeMarketData();
	void EnableSubscribeMarketDataButton();
	CStatic m_StaticSymbolMarketData;
	ValidEdit m_EditSymbolMarketData;
	CButton m_ButtonSubscribeMarketData;
	CButton m_ButtonResetMarketData;

#ifndef TAKION_NO_OPTIONS
	CheckBoxOwnerDraw m_CheckBoxOption;
	RadioGroup m_RadioGroupOption;
	RadioOwnerDraw m_RadioOptionCall;
	RadioOwnerDraw m_RadioOptionPut;
	ComboBoxChar m_ComboBoxOptionSuffix;
	CStatic m_StaticOptionSuffix;
	CStatic m_StaticOptionStrikePrice;
	UnsignedNumberTypeSpin<unsigned int> m_SpinOptionStrikePrice;
	CStatic m_StaticOptionExpiration;
	ColorDateTimeCtrl m_DateTimeOptionExpiration;
	afx_msg void OnCalendarDropdown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCalendarCloseup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExpirationDateChange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOption();
	afx_msg void OnOptionCall();
	afx_msg void OnOptionPut();
	afx_msg void OnSelchangeOptionSuffix();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);

	bool m_optionMode;
	char m_optionSuffix;
	bool m_optionCall;
	unsigned char m_expirationYear;
	unsigned char m_expirationMonth;
	unsigned char m_expirationDay;
	Price m_strikePrice;

	bool m_currentOptionMode;
	char m_currentOptionSuffix;
	bool m_currentOptionCall;
	unsigned char m_currentExpirationYear;
	unsigned char m_currentExpirationMonth;
	unsigned char m_currentExpirationDay;
	Price m_currentStrikePrice;

	void UpdateCallPutBkColor();
	void UpdateStrikePriceColor();
	void UpdateExpirationColor();

	void MakeExpirationCurrent();

	bool UpdateOptionExpirationValue();
	void SetOptionCall(const bool call);
	bool OptionModeChanged();
	void OptionValuesChanged();
#endif

	void MakeMarketDataSymbolCurrent();
	void ResetMarketDataSymbolCurrent();
	std::string m_symbolMarketData;
	std::string m_currentSymbolMarketData;
	std::string m_realSymbolMarketData;//Can be different for Futures
	const Security* m_securityMarketData;
	const Position* m_positionMarketData;
	bool SetSecurityMarketData(const Security* security);
	bool SetPositionMarketData(const Position* position);

	static void SetStringValueNoStatic(StringArea& stringArea, COLORREF color);
	static void SetStringValueStatic(StringArea& stringArea, bool yes, COLORREF colorYes, COLORREF colorNo);

	void SetStringValue(StringArea& stringArea, bool yes) const;
	void SetStringValueNo(StringArea& stringArea) const;
	COLORREF m_colorGreen;// Yes;
	COLORREF m_colorRed;// No;

	void ResetStrings();
	virtual void AccountChanged() override;
	virtual void OnTradingAllowed() override;
	virtual void OnSuspended() override;
	virtual void OnHistoryLoaded() override;
	virtual void OnUntradeableStatusLoaded() override;
	virtual void OnRestrictedSymbolListLoaded() override;
	virtual void OnExecutorConnected() override;
	virtual void OnExecutorLogged() override;
#ifndef TAKION_NO_OPTIONS
	virtual void OnFutureExecutorConnected() override;
	virtual void OnFutureExecutorLogged() override;
#endif
	virtual void OnTraderIdEnabledChanged() override;
	virtual void OnTradingLockedByTrader() override;
	virtual void OnTradingDefaultAccountOnlyByTrader() override;
	virtual void OnTradingTestStocksOnlyByTrader() override;

//Executor and Account
	CustomGroupBox m_groupBoxAccountReady;

	StringArea m_labelAccountReady;
	StringArea m_valueAccountReady;

	StringArea m_labelHistoryLoaded;
	StringArea m_valueHistoryLoaded;

	StringArea m_labelUntradeableStatusLoaded;
	StringArea m_valueUntradeableStatusLoaded;

	StringArea m_labelRestrictedSymbolListLoaded;
	StringArea m_valueRestrictedSymbolListLoaded;

	StringArea m_labelTradingAllowed;
	StringArea m_valueTradingAllowed;

	StringArea m_labelSuspended;
	StringArea m_valueSuspended;

	StringArea m_labelExecutorConnected;
	StringArea m_valueExecutorConnected;

	StringArea m_labelExecutorLogged;
	StringArea m_valueExecutorLogged;

#ifndef TAKION_NO_OPTIONS
	StringArea m_labelFutureExecutorConnected;
	StringArea m_valueFutureExecutorConnected;

	StringArea m_labelFutureExecutorLogged;
	StringArea m_valueFutureExecutorLogged;
#endif

	StringArea m_labelTraderIdEnabled;
	StringArea m_valueTraderIdEnabled;

	StringArea m_labelTradingLockedByTrader;
	StringArea m_valueTradingLockedByTrader;

	StringArea m_labelTradingDefaultAccountOnlyByTrader;
	StringArea m_valueTradingDefaultAccountOnlyByTrader;

	StringArea m_labelTradingTestStocksOnlyByTrader;
	StringArea m_valueTradingTestStocksOnlyByTrader;

//MarketSorter
	CustomGroupBox m_groupBoxMarketSorterReady;

	StringArea m_valueMarketSorterCurrentStock;

	StringArea m_labelMarketSorterAllStocksLoaded;
	StringArea m_valueMarketSorterAllStocksLoaded;

	StringArea m_labelMarketSorterAllIndicesLoaded;
	StringArea m_valueMarketSorterAllIndicesLoaded;

	StringArea m_labelMarketSorterAllStockChartsLoaded;
	StringArea m_valueMarketSorterAllStockChartsLoaded;

	StringArea m_labelMarketSorterAllIndexChartsLoaded;
	StringArea m_valueMarketSorterAllIndexChartsLoaded;

	StringArea m_labelMarketSorterConnected;
	StringArea m_valueMarketSorterConnected;

	StringArea m_labelMarketSorterLogged;
	StringArea m_valueMarketSorterLogged;

	StringArea m_labelMarketSorterStockLoaded;
	StringArea m_valueMarketSorterStockLoaded;

	StringArea m_labelMarketSorterStockInvalid;
	StringArea m_valueMarketSorterStockInvalid;

	StringArea m_labelMarketSorterPositionSize;
	StringArea m_valueMarketSorterPositionSize;

	void SetMarketSorterAllStocksLoaded(const bool loaded);
	void SetMarketSorterAllIndicesLoaded(const bool loaded);
	void SetMarketSorterAllStockChartsLoaded(const bool loaded);
	void SetMarketSorterAllIndexChartsLoaded(const bool loaded);
	void SetMarketSorterConnected(const bool connected);
	void SetMarketSorterLogged(const bool logged);
	void SetMarketSorterStockLoaded(const bool loaded);
	void SetMarketSorterStockInvalid(const bool invalid);
	void SetMarketSorterPositionSize(const int positionSize, const bool noPosition);// = false);
	bool m_marketSorterAllStocksLoaded;
	bool m_marketSorterAllIndicesLoaded;
	bool m_marketSorterAllStockChartsLoaded;
	bool m_marketSorterAllIndexChartsLoaded;
	bool m_marketSorterConnected;
	bool m_marketSorterLogged;

	bool m_marketSorterStockLoaded;
	bool m_marketSorterStockInvalid;
	int m_marketSorterPositionSize;
	char m_marketSorterPositionSizeText[33];

//MarketData
	CustomGroupBox m_groupBoxMarketDataReady;

	StringArea m_valueMarketDataCurrentStock;

#ifndef TAKION_NO_OPTIONS
	CustomGroupBox m_groupBoxOption;
	StringArea m_labelOptionMarketDataConnected;
	StringArea m_valueOptionMarketDataConnected;
	StringArea m_labelOptionMarketDataLogged;
	StringArea m_valueOptionMarketDataLogged;
	void SetOptionMarketDataConnected(const bool connected);
	void SetOptionMarketDataLogged(const bool logged);
	bool m_optionMarketDataConnected;
	bool m_optionMarketDataLogged;

	StringArea m_labelFutureMarketDataConnected;
	StringArea m_valueFutureMarketDataConnected;
	StringArea m_labelFutureMarketDataLogged;
	StringArea m_valueFutureMarketDataLogged;
	void SetFutureMarketDataConnected(const bool connected);
	void SetFutureMarketDataLogged(const bool logged);
	bool m_futureMarketDataConnected;
	bool m_futureMarketDataLogged;
#endif

	StringArea m_labelStockMarketDataConnected;
	StringArea m_valueStockMarketDataConnected;
	StringArea m_labelStockMarketDataLogged;
	StringArea m_valueStockMarketDataLogged;

	StringArea m_labelMarketDataStockLoaded;
	StringArea m_valueMarketDataStockLoaded;
	StringArea m_labelMarketDataStockInvalid;
	StringArea m_valueMarketDataStockInvalid;
	StringArea m_labelMarketDataPositionSize;
	StringArea m_valueMarketDataPositionSize;

	void SetStockMarketDataConnected(const bool connected);
	void SetStockMarketDataLogged(const bool logged);
	bool m_stockMarketDataConnected;
	bool m_stockMarketDataLogged;

	void SetMarketDataStockLoaded(const bool loaded);
	void SetMarketDataStockInvalid(const bool invalid);
	void SetMarketDataPositionSize(const int positionSize, const bool noPosition);
	bool m_marketDataStockLoaded;
	bool m_marketDataStockInvalid;
	int m_marketDataPositionSize;
	char m_marketDataPositionSizeText[33];

	bool m_resizeInitialized;

	virtual void ProcessConnectDisconnect(const Connection* connection) override;

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
