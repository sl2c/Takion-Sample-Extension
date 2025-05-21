#pragma once

#include "BaseDialogSetting.h"
#include "StockBase.h"

class SampleDialogSettingLevel2InThread;
class StockLevel2;
//class ListBoxStockMarketSorter;

#define L2T_SHOW_ORDINALS

enum Level2InThreadColumnId : unsigned short
{
	L2T_SYMBOL,

#ifdef L2T_SHOW_ORDINALS
	L2T_LINE_ORDINAL,
	L2T_STOCK_ORDINAL,
#endif

	L2T_CLOSE_PRICE,

	L2T_LEVEL2_SPREAD,

	L2T_LEVEL2_BID,
	L2T_LEVEL2_BID_SIZE,

	L2T_TOTAL_PE_BID_SIZE_ABOVE_CLOSE_PRICE,
	L2T_MAX_PE_BID_SIZE_ABOVE_CLOSE_PRICE,
	L2T_PE_BID_ABOVE_CLOSE_PRICE_WITH_MAX_SIZE,

	L2T_LEVEL2_ASK,
	L2T_LEVEL2_ASK_SIZE,

	L2T_TOTAL_PE_ASK_SIZE_BELOW_CLOSE_PRICE,
	L2T_MAX_PE_ASK_SIZE_BELOW_CLOSE_PRICE,
	L2T_PE_ASK_BELOW_CLOSE_PRICE_WITH_MAX_SIZE,

	L2T_Count,
};

class ListBoxStockLevel2 : public ColumnedListBox//ListBoxOwnerDraw
{
public:
	ListBoxStockLevel2(SampleDialogSettingLevel2InThread* const& owner, Account* const& account, const bool& calculateL2):
		ColumnedListBox(0, true, true),
		m_loadedColor(0),
		m_unloadedColor(RGB(128, 0, 0)),
		m_marketDataConnected(false),
		m_marketSorterLoaded(false),
		m_calculateL2(calculateL2),
		m_owner(owner),
		m_currentAccount(account),
		m_loadedCount(0),
		m_menuWidth(0)
	{
	}
	virtual void DestroyItem(const void* item) const override;
	virtual const char* GetItemName(const void* item) const override;
	virtual COLORREF GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	virtual COLORREF GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	void CopySymbols(std::string& info) const;
	void RefreshOrdinals() const;
	typedef std::map<std::string, std::string> StringMap;
	void PasteStocks(StringMap& stringMap, bool replace);
//	void LoadAllStocks(bool replace);

	void SetMarketDataConnected(const bool connected);
	const bool& isMarketDataConnected() const{return m_marketDataConnected;}

	void SetMarketSorterLoaded(const bool loaded);
	const bool& isMarketSorterLoaded() const{return m_marketSorterLoaded;}

	void CurrentAccountSet(Account* account);
	Account* GetCurrentAccount() const{return m_currentAccount;}

	const unsigned int& GetLoadedCount() const{return m_loadedCount;}

	virtual bool HasItemToolTips() const override{return false;}
/*
//totals
	const Money& GetClosedPnl() const{return m_closedPnl;}
	const Money& GetOpenPnl() const{return m_openPnl;}
	const Money& GetOpenPrintPnl() const{return m_openPrintPnl;}
	const Price& GetRangeAtFirstTimer() const{return m_rangeAtFirstTimer;}
	const Price& GetAverageRange() const{return m_averageRange;}
*/
	void IncrementLoadedStocks();
	void DecrementLoadedStocks();

	void RepaintCount() const;
//	void UpdatePositionTotals(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff);
/*
	void RepaintClosedPnl();
	void RepaintOpenPnl();
	void RepaintOpenPrintPnl();
*/

	DialogTextEntry& DisplayStockEntryField(unsigned int code);
	SampleDialogSettingLevel2InThread* GetOwnerDialog(){return m_owner;}
	const SampleDialogSettingLevel2InThread* GetOwnerDialog() const{return m_owner;}

	const bool& isCalculateL2() const{return m_calculateL2;}
	void SetCalculateL2(const bool& calculateL2)
	{
		if(calculateL2 != m_calculateL2)
		{
			m_calculateL2 = calculateL2;
		}
	}
protected:
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
//	virtual void HorizontalScrollingDone(int diff) override;
//	virtual void DoCreateColumnToolTips() override;

	void ResetTotals();
	void UpdateStockAccount(StockLevel2* stock);

//	virtual void SizeChanged(bool horizontalChange, bool verticalChange) override;
	virtual void ListBoxCreated() override;
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;

	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;
	virtual void AddMenuItems(CMenu& menu) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;

	virtual void HorizontalScrollDone() override{Invalidate(FALSE);}
	virtual void VerticalScrollDone() override{Invalidate(FALSE);}
	virtual void TopItemChanged(unsigned int prevTop, unsigned char cause){if(TICC_RESIZE == cause)Invalidate(FALSE);}

	afx_msg LRESULT OnStockAdd(WPARAM spinId, LPARAM l);
	DECLARE_MESSAGE_MAP()

	COLORREF m_loadedColor;
	COLORREF m_unloadedColor;
	bool m_marketDataConnected;
	bool m_marketSorterLoaded;

	bool m_calculateL2;

//	bool m_marketDataLoaded;
	SampleDialogSettingLevel2InThread* m_owner;
	Account* m_currentAccount;

	unsigned int m_loadedCount;

	int m_menuWidth;
/*
//totals
	Money m_closedPnl;
	Money m_openPnl;
	Money m_openPrintPnl;
*/
};

class SampleDialogSettingLevel2InThread : public BaseDialogSetting, public Observer
{
friend class ListBoxStockLevel2;
public:
	SampleDialogSettingLevel2InThread(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, Account* account);
//	virtual ~SampleDialogSettingLevel2InThread(){DestroyOneSecondTimer();}
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;
//	virtual void UpdateControls() override;
//	virtual void Leaving() override;

	void RemoveStock(Security* stock);
	void ClearStocks();

	const bool& isMarketDataConnected() const{return m_marketDataConnected;}
	const bool& isMarketSorterLoaded() const{return m_marketSorterLoaded;}

	void CurrentAccountSet(Account* account);
	Account* GetCurrentAccount() const{return m_currentAccount;}

	void RepaintTotals() const;
	void RepaintCount() const;
/*
	void RepaintClosedPnl();
	void RepaintOpenPnl();
	void RepaintOpenPrintPnl();
*/
	virtual void RestoreLayout() override;

	const int& GetListBoxBorderSize() const{return m_listBoxBorderSize;}
	const int& GetLeftOffset() const{return m_leftOffset;}
	const int& GetTopOffset() const{return m_topOffset;}

	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

	bool MainThreadIdleState(unsigned int idleCount, bool takionIdleDone);
protected:
	virtual void ToolTipsEnabled(bool enable) override;

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	bool m_calculateL2;

	Account* m_currentAccount;

	CheckBoxOwnerDraw m_CheckBoxLoadOnStart;
	CheckBoxOwnerDraw m_CheckBoxCalculateL2;
	CButton m_ButtonLoadAll;
	CButton m_ButtonLoadAdd;
	CButton m_ButtonLoadReplace;
	CButton m_ButtonViewStocks;
	CButton m_ButtonBrowse;
	CEdit m_EditPath;

	ListBoxStockLevel2 m_ListBoxStock;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLoadAll();
	afx_msg void OnLoadAdd();
	afx_msg void OnLoadReplace();
	afx_msg void OnViewStocks();

	afx_msg void OnBrowse();
	afx_msg void OnPathChange();

	afx_msg void OnCalculateL2();

	afx_msg void OnSelchangeStock();
	afx_msg void OnDblclkStock();

//	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);

	DECLARE_MESSAGE_MAP()

	void SetMarketDataConnected(const bool connected);
	bool m_marketDataConnected;

	void SetMarketSorterLoaded(const bool loaded);
	bool m_marketSorterLoaded;

	int m_editLeftOffset;
	int m_editTopOffset;
	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;

	void ReadStocks(bool replace);
	void DoReadStocks(bool replace, const char* currentPath);

	void LoadAllStocks(bool replace);

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
