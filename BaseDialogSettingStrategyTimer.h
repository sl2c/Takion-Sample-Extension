#pragma once

#include "BaseDialogSettingStrategy.h"

enum TimerTradingState : unsigned char
{
	TS_BEFORE_ENTRY,
	TS_ENTRY,
	TS_STOP,
	TS_EXIT,
	TS_STOP_EXIT,

	TS_Count
};

class BaseDialogSettingStrategyTimer;

class ListBoxSecurityTimer : public ListBoxSymbol
{
public:
	ListBoxSecurityTimer(BaseDialogSettingStrategy* owner, Account* account):
		ListBoxSymbol(owner, account),
		m_tradingState(TS_STOP_EXIT)
	{
	}
	const unsigned char& GetTradingState() const{return m_tradingState;}
	bool SetTradingState(const unsigned char& tradingState);
protected:
/*
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
	virtual SecurityObserver* CreateSecurityObserver(const unsigned __int64& numericSymbol, const Security* security, const Position* position) override;
*/
	unsigned char m_tradingState;
};

class TimerSecurityObserver : public SecurityObserver
{
friend BaseDialogSettingStrategyTimer;
public:
	TimerSecurityObserver(const unsigned __int64& numericSymbol, ListBoxSecurityTimer* listBox):
		SecurityObserver(numericSymbol, listBox),
		m_tradingState(listBox->GetTradingState())
	{
	}
	const unsigned char& GetTradingState() const{return m_tradingState;}
	bool SetTradingState(const unsigned char& tradingState)
	{
		if(tradingState != m_tradingState)
		{
			m_tradingState = tradingState;
			OnTradingStateChange();
			return true;
		}
		return false;
	}
protected:
	virtual void OnTradingStateChange() = 0;
	unsigned char m_tradingState;
};

enum TimerStrategyIds
{
	groupTimerId = baseStrategyIdsCount,
	spinTimeEntryId,
	staticTimeEntryId,

	spinTimeStopId,
	staticTimeStopId,

	spinTimeExitId,
	staticTimeExitId,

	spinTimeStopExitId,
	staticTimeStopExitId,

	timerStrategyIdsCount
};

class BaseDialogSettingStrategyTimer : public BaseDialogSettingStrategy
{
public:
	BaseDialogSettingStrategyTimer(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label,
		ListBoxSecurityTimer* listBox, bool subscribe, bool disconnectFromMarketSorter, int controlColumnWidth, int listBoxHOffset);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;

	virtual UIntSetting* GetSettingMillisecondEntry(){return nullptr;}
	virtual UIntSetting* GetSettingMillisecondStop(){return nullptr;}
	virtual UIntSetting* GetSettingMillisecondExit(){return nullptr;}
	virtual UIntSetting* GetSettingMillisecondStopExit(){return nullptr;}
/*
	virtual void RepaintTotals() const;

//	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
//	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

	virtual bool SecurityBelongs(const Security* security) const override
	{
		return BaseDialogSettingStrategy::SecurityBelongs(security);
	}
//	virtual bool CanTrade() const{return m_trading && m_account;}
*/
	TimeSpin* GetTimeSpinByTradingState(const unsigned char tradingState);
	CStatic* GetTimeStaticByTradingState(const unsigned char tradingState);
	void InvalidateTimerName(const unsigned char tradingState);
	void Invalidate2TimerNames(const unsigned char tradingState1, const unsigned char tradingState2);
protected:
	virtual BOOL OnInitDialog() override;

	virtual void OnHistoryLoaded();
	virtual void OnExecutorConnected();

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
//	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
//	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

	static void SetTimeSpinCells(TimeSpin& spin);
	TimeSpin m_SpinTimeEntry;
	TimeSpin m_SpinTimeStop;
	TimeSpin m_SpinTimeExit;
	TimeSpin m_SpinTimeStopExit;

	CStatic m_StaticTimeEntry;
	CStatic m_StaticTimeStop;
	CStatic m_StaticTimeExit;
	CStatic m_StaticTimeStopExit;

	ListBoxSecurityTimer* m_listBoxSecurityTimer;

	unsigned int m_millisecondEntry;
	unsigned int m_millisecondStop;
	unsigned int m_millisecondExit;
	unsigned int m_millisecondStopExit;

	void OnTradingStateTimeChange(const unsigned char tradingState, const unsigned char nextTradingState, unsigned int& controlMillisecond, TimeSpin& spin);
	void ClearTimer(const unsigned char tradingState, TimeSpin& spin);
	void CreateTimer(const unsigned char tradingState, const unsigned int millisecond, TimeSpin& spin);
	void UpdateTimer(const unsigned char tradingState, const unsigned int currentMillisecond, const unsigned int endMillisecond, TimeSpin& spin);
	void InitializeTradingState(const unsigned int millisecond);
	bool SetTradingState(const unsigned int millisecond, const unsigned char tradingState);
	unsigned char m_tradingState;
	unsigned char m_timerCreatedMask;
/*
	virtual void AccountChanged() override;
	virtual void OnHistoryLoaded() override;
	virtual void OnExecutorConnected() override;
	virtual bool ShouldAddAccountObserver() const override;
*/
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
