#pragma once

#include "BaseDialogSetting.h"

class SampleDialogSettingStockMarketSorter;
class StockMarketSorter;
//class ListBoxStockMarketSorter;

//#define SAMPLE_MARKET_SORTER_TIMERS
//#define STOCK_PROPERTY

//#define DEBUG_LOCK

enum StockMarketSorterColumnId : unsigned short
{
	MSCI_SYMBOL,
#ifdef DEBUG_LOCK
	MSCI_LOCKED_INQUIRY,
	MSCI_LOCKED_MODIFICATION,
#endif
	MSCI_LEVEL1_BID,
	MSCI_LEVEL1_ASK,
	MSCI_LEVEL1_SPREAD,
	MSCI_PRINT,
	MSCI_LOW,
	MSCI_HIGH,
	MSCI_LAST_SIZE,
	MSCI_VOLUME,
	MSCI_PE_OPEN,
	MSCI_PE_TODAYS_CLOSE,
	MSCI_YEST_CLOSE,
	MSCI_POS_SIZE,
	MSCI_BUY_PENDING,
	MSCI_SELL_PENDING,
	MSCI_POS_TIMESTAMP,
	MSCI_POS_PRICE,
	MSCI_LAST_EXEC_PRICE,
	MSCI_CLOSED_PNL,
	MSCI_OPEN_PNL_LEVEL1,
	MSCI_OPEN_PNL_PRINT,
#ifdef STOCK_PROPERTY
	MSCI_STOCK_PROPERTY,
#endif
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	MSCI_RANGE,
	MSCI_ONE_SECOND,
#endif
	MSCI_Count,
};

class ListBoxStockMarketSorter : public ColumnedListBox//ListBoxOwnerDraw
{
public:
	ListBoxStockMarketSorter(SampleDialogSettingStockMarketSorter* owner, Account* account):
		ColumnedListBox(0, true, true),
		m_loadedColor(0),
		m_unloadedColor(RGB(128, 0, 0)),
		m_updateOnTimer(false),
		m_inThread(false),
		m_positionInThread(false),
		m_marketSorterConnected(false),
		m_marketSorterLoaded(false),
		m_owner(owner),
		m_currentAccount(account),

		m_level1BidLeft(0),
		m_level1BidRight(0),

		m_volumeLeft(0),
		m_volumeRight(0),

		m_openPnlPrintLeft(0),
		m_openPnlPrintRight(0),

		m_positionSizeLeft(0),
		m_positionSizeRight(0),

#ifdef DEBUG_LOCK
		m_lockInquiryCount(0),
		m_lockModificationCount(0),

		m_lockInquiryLeft(0),
		m_lockInquiryRight(0),

		m_lockModificationLeft(0),
		m_lockModificationRight(0),
#endif
		m_menuWidth(0)

#ifdef SAMPLE_MARKET_SORTER_TIMERS
		,m_rangeStockCount(0),
		m_rangeLeft(0),
		m_rangeRight(0),

		m_oneSecondPrintLeft(0),
		m_oneSecondPrintRight(0),

		m_oneSecondRect(0, 0, 0, 0),
		m_secondBack(0)
#endif
	{
	}
	virtual void DestroyItem(const void* item) const override;
	virtual const char* GetItemName(const void* item) const override;
	virtual COLORREF GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	virtual COLORREF GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	void CopySymbols(std::string& info) const;
	void RefreshOrdinals();
	typedef std::map<std::string, std::string> StringMap;
	void PasteStocks(StringMap& stringMap, bool replace);

	void SetInThread(const bool inThread);
	void SetPositionInThread(const bool inThread);

	void SetUpdateOnTimer(const bool updateOnTimer);

	void SetMarketSorterConnected(bool connected);
	bool isMarketSorterConnected() const{return m_marketSorterConnected;}

	void SetMarketSorterLoaded(bool loaded);
	bool isMarketSorterLoaded() const{return m_marketSorterLoaded;}

	void MarketSorterCleanup();

	void CurrentAccountSet(Account* account);
//	Account* GetCurrentAccount(){return m_currentAccount;}
	Account* GetCurrentAccount() const{return m_currentAccount;}

	virtual bool HasItemToolTips() const override{return false;}

	void Trigger(unsigned int millisecondFromMidnight);
	void TriggerOneSecond(unsigned int count);

	const int& GetLevel1BidLeft() const{return m_level1BidLeft;}
	const int& GetLevel1BidRight() const{return m_level1BidRight;}

	const int& GetVolumeLeft() const{return m_volumeLeft;}
	const int& GetVolumeRight() const{return m_volumeRight;}

	const int& GetOpenPnlPrintLeft() const{return m_openPnlPrintLeft;}
	const int& GetOpenPnlPrintRight() const{return m_openPnlPrintRight;}

	const int& GetPositionSizeLeft() const{return m_positionSizeLeft;}
	const int& GetPositionSizeRight() const{return m_positionSizeRight;}

#ifdef DEBUG_LOCK
	const int& GetLockInquiryLeft() const{return m_lockInquiryLeft;}
	const int& GetLockInquiryRight() const{return m_lockInquiryRight;}

	const int& GetLockModificationLeft() const{return m_lockModificationLeft;}
	const int& GetLockModificationRight() const{return m_lockModificationRight;}
#endif
	unsigned char AddSymbol(const std::string& textResult);
	bool RemoveSelectedSymbol();
	bool ClearSymbols();

	unsigned char AddSecurity(const Security* security, bool updateOrdinals);
#ifdef SAMPLE_MARKET_SORTER_TIMERS
//Special Timer
	void ResetStockTimer();

	int GetRangeLeft() const{return m_rangeLeft;}
	int GetRangeRight() const{return m_rangeRight;}

	int GetOneSecondPrintLeft() const{return m_oneSecondPrintLeft;}
	int GetOneSecondPrintRight() const{return m_oneSecondPrintRight;}
	const Price& GetRangeAtFirstTimer() const{return m_rangeAtFirstTimer;}
	const Price& GetAverageRange() const{return m_averageRange;}
#endif
//totals
	const Money& GetClosedPnl() const{return m_closedPnl;}
	const Money& GetOpenLevel1Pnl() const{return m_openLevel1Pnl;}
	const Money& GetOpenPrintPnl() const{return m_openPrintPnl;}

#ifdef DEBUG_LOCK
	const unsigned int& GetLockInquiryCount() const{return m_lockInquiryCount;}
	const unsigned int& GetLockModificationCount() const{return m_lockModificationCount;}
	void UpdateLockInquiryCountTotal(const bool lock);
	void UpdateLockModificationCountTotal(const bool lock);
	void RepaintLockInquiryCount() const;
	void RepaintLockModificationCount() const;
#endif
	void IncrementLoadedStocks();
	void DecrementLoadedStocks();
	void UpdatePositionTotals(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff);
	void UpdatePositionOpenPrintPnlTotal(const Money& openPrintPnlDiff);
	void UpdatePositionOpenLevel1PnlTotal(const Money& openLevel1PnlDiff);
	void UpdatePositionClosedPnlTotal(const Money& closedPnlDiff);
	void RepaintCount() const;
	void RepaintClosedPnl() const;
	void RepaintOpenLevel1Pnl() const;
	void RepaintOpenPrintPnl() const;

//	static bool GetOrderErrorMessage(unsigned char errorCode, std::string& errorMessage);
	void LogOrderError(unsigned char errorCode, const char* prefix) const;

//	static bool GetCancelErrorMessage(unsigned char errorCode, std::string& errorMessage);
	void LogCancelError(unsigned char errorCode, const char* prefix) const;

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	void UpdateTimerTotals(const SignedPrice& rangeDiff);
	void RepaintRangeAtFirstTimer() const;
	const unsigned int& GetSecondBack() const{return m_secondBack;}
	bool SetSecondBack(unsigned int secondBack);

	const unsigned int& GetRangeStockCount() const{return m_rangeStockCount;}
	void UpdateAverageRange();
#endif

	DialogTextEntry& DisplayStockEntryField();
	SampleDialogSettingStockMarketSorter* GetOwnerDialog(){return m_owner;}
	const SampleDialogSettingStockMarketSorter* GetOwnerDialog() const{return m_owner;}
protected:
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
	virtual void HorizontalScrollingDone(int diff) override;
//	virtual void DoCreateColumnToolTips() override;

	void ResetTotals();
	void UpdateStockAccount(StockMarketSorter* stock);

	virtual void SizeChanged(bool horizontalChange, bool verticalChange) override;
//	virtual void ListBoxCreated() override;
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;
//	virtual void DoDrawItem(const DRAWITEMSTRUCT* lpDrawItemStruct, const CRect& drawItemRect) override;

	virtual void ListBoxCreated() override;

	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;
	virtual void AddMenuItems(CMenu& menu) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	virtual void BeforeDestroy() override
	{
		ColumnedListBox::BeforeDestroy();
		m_updateOnTimer = false;
		m_inThread = false;
		m_positionInThread = false;

		m_marketSorterConnected = false;
		m_marketSorterLoaded = false;
//		m_owner = nullptr;
		m_currentAccount = nullptr;

//totals
		m_closedPnl.SetZero();
		m_openLevel1Pnl.SetZero();
		m_openPrintPnl.SetZero();

		m_level1BidLeft = 0;
		m_level1BidRight = 0;

		m_volumeLeft = 0;
		m_volumeRight = 0;

		m_openPnlPrintLeft = 0;
		m_openPnlPrintRight = 0;

		m_positionSizeLeft = 0;
		m_positionSizeRight = 0;

#ifdef DEBUG_LOCK
		m_lockInquiryCount = 0;
		m_lockModificationCount = 0;

		m_lockInquiryLeft = 0;
		m_lockInquiryRight = 0;

		m_lockModificationLeft = 0;
		m_lockModificationRight = 0;
#endif
		m_menuWidth = 0;

#ifdef SAMPLE_MARKET_SORTER_TIMERS
		m_rangeAtFirstTimer.SetZero();
		m_averageRange.SetZero();

		m_rangeStockCount = 0;

		m_rangeLeft = 0;
		m_rangeRight = 0;

		m_oneSecondPrintLeft = 0;
		m_oneSecondPrintRight = 0;

		m_oneSecondRect.left = m_oneSecondRect.top = m_oneSecondRect.right = m_oneSecondRect.bottom = 0;
		m_secondBack = 0;
#endif
	}

	afx_msg LRESULT OnStockAdd(WPARAM spinId, LPARAM l);
	DECLARE_MESSAGE_MAP()

	COLORREF m_loadedColor;
	COLORREF m_unloadedColor;

	bool m_updateOnTimer;

	bool m_inThread;
	bool m_positionInThread;

	bool m_marketSorterConnected;
	bool m_marketSorterLoaded;
	SampleDialogSettingStockMarketSorter* m_owner;
	Account* m_currentAccount;

//totals
	Money m_closedPnl;
	Money m_openLevel1Pnl;
	Money m_openPrintPnl;

	int m_level1BidLeft;
	int m_level1BidRight;

	int m_volumeLeft;
	int m_volumeRight;

	int m_openPnlPrintLeft;
	int m_openPnlPrintRight;

	int m_positionSizeLeft;
	int m_positionSizeRight;

#ifdef DEBUG_LOCK
	unsigned int m_lockInquiryCount;
	unsigned int m_lockModificationCount;

	int m_lockInquiryLeft;
	int m_lockInquiryRight;

	int m_lockModificationLeft;
	int m_lockModificationRight;
#endif
	int m_menuWidth;

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	void ResetTimerTotals();
	Price m_rangeAtFirstTimer;
	Price m_averageRange;

	unsigned int m_rangeStockCount;

	int m_rangeLeft;
	int m_rangeRight;

	int m_oneSecondPrintLeft;
	int m_oneSecondPrintRight;

	CRect m_oneSecondRect;
	unsigned int m_secondBack;
#endif
};

#ifdef SAMPLE_MARKET_SORTER_TIMERS

class ListBoxTimer : public ListBoxOwnerDraw
{
public:
	ListBoxTimer(bool rbuttonSelection = false, bool rbuttonMenu = false, bool draggable = false):
		ListBoxOwnerDraw(0, rbuttonSelection, rbuttonMenu, draggable),
		m_triggeredColor(RGB(0, 128, 0)),
		m_oldColor(RGB(128, 0, 0)),
		m_untriggeredColor(0)
	{
		m_hAlignment = DT_RIGHT;
	}
	virtual int Compare(const void* item1, const void* item2) const override;
	virtual const char* GetItemName(const void* item) const override;
	virtual COLORREF GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	virtual COLORREF GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	virtual void DestroyItem(const void* item) const override;
//	void RemoveTimer(StockTimer* timer);
	void RefreshOrdinals();
protected:
//	virtual void DoDrawItem(const DRAWITEMSTRUCT* lpDrawItemStruct, const CRect& drawItemRect) override;
	COLORREF m_triggeredColor;
	COLORREF m_oldColor;
	COLORREF m_untriggeredColor;
};

class StockTimer : public TakionTimerOnce
{
public:
	StockTimer(unsigned int millisecondFromMidnight, ListBoxStockMarketSorter* owner, ListBoxTimer* timerBox);
	bool isTriggered() const{return m_triggered;}
	bool isOld() const{return m_old;}
	unsigned int GetMillisecondFromMidnight() const{return m_millisecondFromMidnight;}
	virtual void Triggered(unsigned int count) override;

	void SetOrdinal(unsigned int ordinal){m_ordinal = ordinal;}
	unsigned int GetOrdinal() const{return m_ordinal;}
	void IncrementOrdinal(){++m_ordinal;}
	void IncrementOrdinal(unsigned int number){m_ordinal += number;}
	void DecrementOrdinal(){if(m_ordinal)--m_ordinal;}
	void DecrementOrdinal(unsigned int number){if(m_ordinal > number)m_ordinal -= number; else m_ordinal = 0;}

protected:
	ListBoxStockMarketSorter* m_owner;
	ListBoxTimer* m_timerBox;
	unsigned int m_ordinal;//ordinal number in the list box.
	unsigned int m_millisecondFromMidnight;
	bool m_triggered;
	bool m_old;
};

class OneSecondTimer : public TakionTimerRepeat
{
public:
	OneSecondTimer(unsigned int cycle, ListBoxStockMarketSorter* owner);
	virtual void Triggered(unsigned int count) override;
protected:
	ListBoxStockMarketSorter* m_owner;
};
#endif
/*
enum TotalsCode : unsigned char
{
	TC_CLOSED_PNL,
	TC_OPEN_PNL,
	TC_OPEN_PRINT_PNL,

	TC_Count
};
*/
class SampleDialogSettingStockMarketSorter : public BaseDialogSetting, public Observer
{
friend class ListBoxStockMarketSorter;
public:
	SampleDialogSettingStockMarketSorter(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, Account* account);
	virtual ~SampleDialogSettingStockMarketSorter();
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	virtual ~SampleDialogSettingStockMarketSorter(){DestroyOneSecondTimer();}
	void RepaintRangeAtFirstTimer() const;
	void SetSecondBack(unsigned int secondBack);
	const std::string& GetOneSecondPrintTitle() const{return m_oneSecondPrintTitle;}
#endif
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;
//	virtual void UpdateControls() override;
//	virtual void Leaving() override;

	void RemoveStock(Security* stock);
	void ClearStocks();

	bool isMarketSorterConnected() const{return m_marketSorterConnected;}
	bool isMarketSorterLoaded() const{return m_marketSorterLoaded;}

	void CurrentAccountSet(Account* account);
//	Account* GetCurrentAccount(){return m_currentAccount;}
	Account* GetCurrentAccount() const{return m_currentAccount;}

	void RepaintTotals() const;
//	void RepaintTotals(unsigned char which);
	void RepaintCount() const;
	void RepaintClosedPnl() const;
	void RepaintOpenLevel1Pnl() const;
	void RepaintOpenPrintPnl() const;
#ifdef DEBUG_LOCK
	void RepaintLockInquiryCount() const;
	void RepaintLockModificationCount() const;
#endif
	virtual void RestoreLayout() override;

	const int& GetListBoxBorderSize() const{return m_listBoxBorderSize;}
	const int& GetLeftOffset() const{return m_leftOffset;}
	const int& GetTopOffset() const{return m_topOffset;}

	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

protected:
//	virtual void TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff) override;
//	virtual void InitControls() override;
	virtual void ToolTipsEnabled(bool enable) override;

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	Account* m_currentAccount;

	CheckBoxOwnerDraw m_CheckBoxLoadOnStart;
	CButton m_ButtonLoadAdd;
	CButton m_ButtonLoadReplace;
	CButton m_ButtonViewStocks;
	CButton m_ButtonBrowse;
	CEdit m_EditPath;

	CheckBoxOwnerDraw m_CheckBoxInThread;
	CheckBoxOwnerDraw m_CheckBoxPositionInThread;
	CheckBoxOwnerDraw m_CheckBoxDblClkSendSymbolToMm;

	CustomGroupBoxAdjustable m_GroupBoxSymbol;
	CStatic m_StaticSymbol;
	ValidEdit m_EditSymbol;
	CButton m_ButtonFind;
	CButton m_ButtonAdd;
	CButton m_ButtonRemove;
	CButton m_ButtonClear;

	CheckBoxOwnerDraw m_CheckBoxAddAll;

	CustomGroupBoxAdjustable m_GroupBoxUpdateOnTimer;
	CheckBoxOwnerDraw m_CheckBoxUpdateOnTimer;
	TimeSpin m_SpinUpdateTimer;
	CButton m_ButtonUpdateTimerSet;
	CButton m_ButtonUpdateTimerReset;

	ListBoxStockMarketSorter m_ListBoxStock;

	static void SetTimeSpinCells(TimeSpin& spin);

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLoadAdd();
	afx_msg void OnLoadReplace();
	afx_msg void OnViewStocks();

	afx_msg void OnBrowse();
	afx_msg void OnPathChange();

	afx_msg void OnInThread();
	afx_msg void OnPositionInThread();
	afx_msg void OnSendSymbolToMm();
	afx_msg void OnAddAll();

	afx_msg void OnUpdateOnTimer();
	afx_msg void OnUpdateTimerSet();
	afx_msg void OnUpdateTimerReset();

	afx_msg void OnFind();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnClear();
//	afx_msg void OnAll();
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();

	afx_msg void OnSelchangeStock();
	afx_msg void OnDblClkStock();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	afx_msg void OnAddTimer();
	afx_msg void OnRemoveTimer();
	afx_msg void OnClearTimer();
	afx_msg void OnSelchangeTimer();
	void ClearTimers();
	TimeSpin m_SpinSecondBack;
	TimeSpin m_SpinTimer;
	CButton m_ButtonAddTimer;
	ListBoxTimer m_ListBoxTimer;
	CButton m_ButtonRemoveTimer;
	CButton m_ButtonClearTimer;

	void UpdateOneSecondPrintTitle();
	std::string m_oneSecondPrintTitle;

	void DestroyOneSecondTimer();
	OneSecondTimer* m_oneSecondTimer;
	int m_timerLeftOffset;
	int m_timerTopOffset;
	bool m_timersChanged;
#endif

	DECLARE_MESSAGE_MAP()

	void SetMarketSorterConnected(bool connected);
	void SetMarketSorterLoaded(bool loaded);
	bool m_marketSorterConnected;
	bool m_marketSorterLoaded;

	int m_editLeftOffset;
	int m_editTopOffset;
	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;

	std::string m_symbol;

	void ReadStocks(bool replace);
	void DoReadStocks(bool replace, const char* currentPath);

	void SetInThread(const bool inThread);
	bool m_inThread;

	void SetPositionInThread(const bool inThread);
	bool m_positionInThread;

	void SetSendToMm(const bool sendToMm);
	bool m_sendToMm;

	void SetAddAll(const bool addAll);
	void AddAll();
	bool m_addAll;

	void SetUpdateOnTimer(const bool updateOnTimer);
	bool m_updateOnTimer;

	void UpdateTimerInControl();
	void DoUpdateTimerSet();
	unsigned int m_updateTimerMillisecond;
	unsigned int m_updateTimerMillisecondInControl;

	TakionIterator* m_allStockterator;

	void EnableAddButton();
	void EnableFindButton();
	void EnableClearButton();
	bool Find();
	unsigned char AddStock();//0 symbol is empty, 1 = already exists, 2 - new stock added

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
