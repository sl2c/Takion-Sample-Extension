#pragma once

#include "BaseDialogSettingAccount.h"
//#include "ConfigEnums.h"

class BaseDialogSettingStrategy;
class SecurityObserver;

class ListBoxSymbol : public ColumnedListBox
{
public:
	virtual void DestroyItem(const void* item) const override;
	virtual const char* GetItemName(const void* item) const override;

	int AddItem(SecurityObserver* item);
	int DoAddItem(SecurityObserver* item, bool updateOrdinals);
	bool RemoveRowAt(int found);
	bool RemoveRow(const SecurityObserver* item);
	bool RemoveSecurity(const Security* security);
	void RemoveItemAt(int found, SecurityObserver* item);
	void DoRemoveItemAt(int found, SecurityObserver* item);
	void Clear();

//	void SetExecutorConnected(bool connected);
	bool isExecutorConnected() const{return m_executorConnected;}
//	void SetHistoryLoaded(bool historyLoaded);
	bool isHistoryLoaded() const{return m_historyLoaded;}
//	void SetAccount(const Account* account);
	Account* GetAccount() const{return m_account;}

	void SetHistoryLoaded(const bool& loaded)
	{
		if(loaded != m_historyLoaded)
		{
			m_historyLoaded = loaded;
		}
	}

	void SetExecutorConnected(const bool& connected)
	{
		if(connected != m_executorConnected)
		{
			m_executorConnected = connected;
			if(!m_executorConnected)
			{
				SetHistoryLoaded(false);
			}
		}
	}

	void SetAccount(Account* const& account);

	BaseDialogSettingStrategy* GetOwnerDialog(){return m_owner;}
	const BaseDialogSettingStrategy* GetOwnerDialog() const{return m_owner;}

	virtual bool HasItemToolTips() const override{return false;}

	void LogOrderError(unsigned char errorCode, const char* prefix) const;

	void LogCancelError(unsigned char errorCode, const char* prefix) const;
	virtual COLORREF GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	virtual COLORREF GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	void GetPrefix(std::string& prefix) const;

	void RepaintCount() const;

	void IncrementOrdinals(unsigned int startFrom);
	void DecrementOrdinals(unsigned int startFrom);

	SecurityObserver* AddSymbolAndUpdateOrdinals(const char* symbol, const char* data, bool* added);
	SecurityObserver* AddSymbol(const unsigned __int64& numericSymbol, const char* data, bool* added);
	SecurityObserver* AddSecurityObserver(const unsigned __int64& numericSymbol, const Security* security, const char* data, bool* added);//, const Position* position);
	SecurityObserver* AddSecurity(const Security* security, const char* data, bool updateOrdinals, bool* added);

	void MsCleanup();

	void CopySymbols(std::string& info) const;
	void RefreshOrdinals();
	typedef std::map<std::string, std::string> StringMap;
	void PasteStocks(StringMap& stringMap, bool replace);

	void RepaintPosSize() const;
	void RepaintSharesTraded() const;
	void RepaintOpenPnl() const;
	void RepaintClosedPnl() const;

	const Money& GetClosedPnl() const{return m_closedPnl;}
	bool UpdateClosedPnl(const Money& oldValue, const Money& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_closedPnl))
		{
			RepaintClosedPnl();
			return true;
		}
		return false;
	}

	const Money& GetOpenPnl() const{return m_openPnl;}
	bool UpdateOpenPnl(const Money& oldValue, const Money& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_openPnl))
		{
			RepaintOpenPnl();
			return true;
		}
		return false;
	}

	const unsigned int& GetPosSize() const{return m_posSize;}
	bool UpdatePosSize(const int& oldValue, const int& newValue)
	{
		unsigned int uOld = oldValue < 0 ? -oldValue : oldValue;
		unsigned int uNew = newValue < 0 ? -newValue : newValue;
		if(U_UpdateValueByDifference(uOld, uNew, m_posSize))
		{
			RepaintPosSize();
			return true;
		}
		return false;
	}
	const unsigned int& GetSharesTraded() const{return m_sharesTraded;}
	bool UpdateSharesTraded(const unsigned int& oldValue, const unsigned int& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_sharesTraded))
		{
			RepaintSharesTraded();
			return true;
		}
		return false;
	}
	virtual bool CanTrade() const;
protected:
	ListBoxSymbol(BaseDialogSettingStrategy* owner, Account* account):
		ColumnedListBox(0, true, true),
		m_mdColor(RGB(64, 64, 0)),
		m_msColor(RGB(0, 64, 64)),
		m_refreshedMdColor(RGB(0, 128, 0)),
		m_refreshedMsColor(RGB(0, 0, 128)),
		m_failedColor(RGB(128, 0, 0)),
		m_executorConnected(false),
		m_historyLoaded(false),
		m_owner(owner),
		m_account(account),
		m_menuWidth(0),
		m_posSize(0),
		m_sharesTraded(0)
	{
	}
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;

	virtual void ListBoxCreated() override;

	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;
	virtual void AddMenuItems(CMenu& menu) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;

	afx_msg LRESULT OnStockAdd(WPARAM spinId, LPARAM l);
	DECLARE_MESSAGE_MAP()

//	COLORREF m_refreshedColor;
	COLORREF m_mdColor;
	COLORREF m_msColor;
	COLORREF m_refreshedMdColor;
	COLORREF m_refreshedMsColor;
	COLORREF m_failedColor;
	bool m_executorConnected;
	bool m_historyLoaded;
	BaseDialogSettingStrategy* m_owner;
	Account* m_account;

//totals
	void ResetTotals();

	virtual void BeforeDestroy() override
	{
		ColumnedListBox::BeforeDestroy();
		m_menuWidth = 0;
		m_posSize = 0;
		m_sharesTraded = 0;
		m_openPnl.SetZero();
		m_closedPnl.SetZero();
	}

	int m_menuWidth;

	unsigned int m_posSize;
	unsigned int m_sharesTraded;
	Money m_openPnl;
	Money m_closedPnl;

	DialogTextEntry& DisplayStockEntryField();
	virtual SecurityObserver* CreateSecurityObserver(const unsigned __int64& numericSymbol);//, const Security* security, const Position* position);
private:
	int AddString(LPCTSTR item){return ColumnedListBox::AddString(item);}
	int InsertString(int i, LPCTSTR item){return ColumnedListBox::InsertString(i, item);}
	int DeleteString(int i){return ColumnedListBox::DeleteString(i);}
	bool ClearItemsWithRedraw(){return ListBoxOwnerDraw::ClearItemsWithRedraw();}
	void ResetContent(){return ColumnedListBox::ResetContent();}
};

enum SymbolColumnId : unsigned short
{
	SO_SYMBOL,
	SO_POS_SIZE,
	SO_OPEN_PNL,
	SO_CLOSED_PNL,
	SO_SHARES_TRADED,

	SO_Count,
};

class LbSymbolHorizontalScroller : public LbHorizontalScroller
{
public:
	LbSymbolHorizontalScroller(ListBoxSymbol* listBox):
		LbHorizontalScroller(listBox),
		m_listBoxSymbol(listBox),
		m_itemHeight(0),
		m_doubleHeight(0),
		m_tripleHeight(0),
		m_fourHeight(0),
		m_fiveHeight(0),
		m_sixHeight(0)
	{
	}
	virtual void AddColumns() override;
protected:
	ListBoxSymbol* m_listBoxSymbol;
	int m_itemHeight;
	int m_doubleHeight;
	int m_tripleHeight;
	int m_fourHeight;
	int m_fiveHeight;
	int m_sixHeight;
};

class SecurityObserver : public Observer
{
public:
	virtual ~SecurityObserver()
	{
		SetSecurity(nullptr);
		SetPosition(nullptr);
	}
	const Security* GetSecurity() const{return m_security;}
	void SetSecurity(const Security* const& security)
	{
		if(security != m_security)
		{
			if(m_security)
			{
				m_security->RemoveObserver(this);
				TD_ReleaseStock(m_security);//the same as m_security->DecrementUnsubscribable();
			}
			m_security = security;
			if(m_security)
			{
				m_security->IncrementUnsubscribable();
				m_security->AddObserver(this);
			}
			UpdateSecurityConstValues();
			UpdateSecurity();
		}
	}
	const Position* GetPosition() const{return m_position;}
	void SetPosition(const Position* const& position)
	{
		if(position != m_position)
		{
			if(m_position)
			{
				m_position->RemoveObserver(this);
			}
			m_position = position;
			if(m_position)
			{
				m_position->AddObserver(this);
			}
			UpdatePositionConstValues();
			UpdatePosition();
		}
	}
	const int& GetPosSize() const{return m_posSize;}
	const unsigned int& GetSharesTraded() const{return m_sharesTraded;}
	const Money& GetOpenPnl() const{return m_openPnl;}
	const Money& GetClosedPnl() const{return m_closedPnl;}
	const Price& GetLevel1BidPrice() const{return m_level1BidPrice;}
	const Price& GetLevel1AskPrice() const{return m_level1AskPrice;}

	const char* GetSymbol() const{return (const char*)&m_numericSymbol;}
	const unsigned __int64& GetNumericSymbol() const{return m_numericSymbol;}

	virtual void SetData(const char* data){}

	const unsigned int& GetOrdinal() const{return m_ordinal;}
	void SetOrdinal(const unsigned int& ordinal) const{m_ordinal = ordinal;}
	void IncrementOrdinal() const{++m_ordinal;}
	void DecrementOrdinal() const{--m_ordinal;}
	SecurityObserver(const unsigned __int64& numericSymbol, ListBoxSymbol* listBoxOrder)://, const Security* security, const Position* position, ListBoxSymbol* listBoxOrder):
		m_numericSymbol(numericSymbol),
		m_security(nullptr),
		m_position(nullptr),
		m_listBoxSymbol(listBoxOrder),
		m_ordinal(0),
		m_posSize(0),
		m_sharesTraded(0),
		m_level1Bid(0),
		m_level1Ask(0),
		m_lastPrint(0),
		m_posSizeNew(0),
		m_sharesTradedNew(0),
		m_level1BidNew(0),
		m_level1AskNew(0),
		m_lastPrintNew(0)
	{
	}
	bool UpdateSecurity();
	bool UpdateSecurityConstValues();
	bool UpdatePosition();
	bool UpdatePositionConstValues();
	bool Update()
	{
		bool ret = UpdateSecurity();
		if(UpdatePosition())
		{
			ret = true;
		}
		return ret;
	}
	bool UpdateConstValues()
	{
		bool ret = UpdateSecurityConstValues();
		if(UpdatePositionConstValues())
		{
			ret = true;
		}
		return ret;
	}
	void Invalidate()
	{
		m_listBoxSymbol->InvalidateItem(m_ordinal);
	}

	void InvalidateData()//except symbol
	{
		m_listBoxSymbol->InvalidateItemColumnsFrom(m_ordinal, SO_SYMBOL + 1);
	}
	virtual bool CanTrade() const
	{
		return m_security && m_listBoxSymbol->CanTrade();
	}
protected:
	virtual void NullifySecurityValues();
	virtual void NullifySecurityConstValues(){}
	virtual void NullifyPositionValues();
	virtual void NullifyPositionConstValues(){}
	virtual void RememberSecurityValues();
	virtual void RememberSecurityConstValues(){}
	virtual void RememberPositionValues();
	virtual void RememberPositionConstValues(){}
	virtual bool DoUpdateSecurity();
	virtual bool DoUpdateSecurityConstValues(){return false;}
	virtual bool DoUpdatePosition();
	virtual bool DoUpdatePositionConstValues(){return false;}

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
	unsigned __int64 m_numericSymbol;
	const Security* m_security;
	const Position* m_position;
	ListBoxSymbol* m_listBoxSymbol;

	mutable unsigned int m_ordinal;

	virtual void OnPosSizeChanged(){}
//Current Values
	int m_posSize;
	unsigned int m_sharesTraded;
	Money m_openPnl;
	Money m_closedPnl;
	unsigned int m_level1Bid;
	unsigned int m_level1Ask;
	unsigned int m_lastPrint;
	Price m_level1BidPrice;
	Price m_level1AskPrice;
	Price m_lastPrintPrice;
//New Values
	int m_posSizeNew;
	unsigned int m_sharesTradedNew;
	Money m_openPnlNew;
	Money m_closedPnlNew;
	unsigned int m_level1BidNew;
	unsigned int m_level1AskNew;
	unsigned int m_lastPrintNew;
private:
	SecurityObserver(const SecurityObserver& other){}
	SecurityObserver& operator=(const SecurityObserver& other){return *this;}
};

enum BaseDialogSettingStrategyIds
{
	editStrategySymbolId = baseDialogSettingAccountIdsCount,//1000,
	groupStrategySymbolId,
	buttonSymbolAddId,
	buttonSymbolFindId,

	groupLoadFromId,
	radioLoadFromFileId,
	radioLoadFromMarketId,

	strategyLoadOnStartCheckBoxId,
	strategyButtonLoadAddStockId,
	strategyButtonLoadReplaceStockId,
	strategyButtonViewStocksId,
	strategyButtonBrowseId,
	strategyComboBoxSymbolDelimiterId,
	strategyEditPathId,

	strategyTradeOnStartCheckBoxId,
	strategyButtonTradeId,
	strategyButtonStopId,

	listBoxSecurityId,

	baseStrategyIdsCount
};

class BaseDialogSettingStrategy : public BaseDialogSettingAccount
{
friend class ListBoxSymbol;
public:
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;

	virtual BoolSetting* GetSettingTradeOnStart(){return nullptr;}
	virtual BoolSetting* GetSettingLoadOnStart(){return nullptr;}
	virtual BoolSetting* GetSettingLoadFromFile(){return nullptr;}
	virtual StringSetting* GetSettingStockFilePath(){return nullptr;}
	virtual UCharSetting* GetSettingSymbolDelimiter(){return nullptr;}

	void RepaintCount() const;
	void RepaintPosSize() const;
	void RepaintSharesTraded() const;
	void RepaintOpenPnl() const;
	void RepaintClosedPnl() const;

	virtual void RepaintTotals() const;

	int GetListBoxBorderSize() const{return m_listBoxBorderSize;}
	int GetLeftOffset() const{return m_leftOffset;}
	int GetTopOffset() const{return m_topOffset;}

	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

	const bool& isHistoryLoaded() const{return m_historyLoaded;}
	const bool& isExecutorConnected() const{return m_executorConnected;}

	const bool& isMarketSorterConnected() const{return m_marketSorterConnected;}
	const bool& isMarketSorterLoaded() const{return m_marketSorterLoaded;}

	const bool& isSubscribe() const{return m_subscribe;}
	const bool& isDisconnectFromMarketSorter() const{return m_disconnectFromMarketSorter;}

	virtual bool SecurityBelongs(const Security* security) const{return !security->isTest();}

	bool GetLoadFromFileFromControls() const;
	const bool& isLoadFromFile() const{return m_loadFromFile;}

	virtual bool CanTrade() const{return m_trading && m_account;}
protected:
	BaseDialogSettingStrategy(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label,
		ListBoxSymbol* listBoxSymbol, bool subscribe, bool disconnectFromMarketSorter, int controlColumnWidth, int listBoxHOffset);

	bool ReadStocks(bool replace);
	void DoReadStocks(bool replace, const char* currentPath);

	virtual void ToolTipsEnabled(bool enable) override;

	virtual BOOL OnInitDialog() override;

	void EnableFindSymbolButton();
	virtual bool CanApplySymbol() const;
	virtual bool ApplySymbol();
	void FindSymbol();
	std::string m_controlSymbol;

	virtual void OnHistoryLoaded();

	void SetHistoryLoaded(const bool& loaded)
	{
		if(loaded != m_historyLoaded)
		{
			m_historyLoaded = loaded;
			OnHistoryLoaded();
		}
	}

	virtual void OnExecutorConnected();
	void SetExecutorConnected(const bool& connected)
	{
		if(connected != m_executorConnected)
		{
			m_executorConnected = connected;
			OnExecutorConnected();
			if(!m_executorConnected)
			{
				SetHistoryLoaded(false);
			}
		}
	}

	void SetMarketSorterConnected(const bool& connected)
	{
		if(connected != m_marketSorterConnected)
		{
			m_marketSorterConnected = connected;
			if(!m_marketSorterConnected)SetMarketSorterLoaded(false);
		}
	}

	void SetMarketSorterLoaded(const bool& loaded)
	{
		if(loaded != m_marketSorterLoaded)
		{
			m_marketSorterLoaded = loaded;
		}
	}

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	afx_msg void OnLoadOnStart();

	afx_msg void OnLoadAdd();
	afx_msg void OnLoadReplace();
	afx_msg void OnViewStocks();

	afx_msg void OnBrowse();
	afx_msg void OnPathChange();

	afx_msg void OnLoadFromFile();
	afx_msg void OnLoadFromMarket();

	afx_msg void OnTradeOnStart();
	afx_msg void OnTrade();
	afx_msg void OnStop();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnSelchangeSymbol();
	afx_msg void OnDblclkSymbol();
	afx_msg void OnAddSymbol();
	afx_msg void OnFindSymbol();

	CButton m_CheckBoxLoadOnStart;
	CButton m_ButtonLoadAdd;
	CButton m_ButtonLoadReplace;
	CButton m_ButtonViewStocks;
	CButton m_ButtonBrowse;
	ComboBoxString m_ComboBoxSymbolDelimiter;
	CEdit m_EditPath;

	CButton m_RadioLoadFromFile;
	CButton m_RadioLoadFromMarket;

	CButton m_CheckBoxTradeOnStart;
	CButton m_ButtonTrade;
	CButton m_ButtonStop;

	ValidEdit m_EditSymbol;
	CButton m_ButtonSymbolAdd;
	CButton m_ButtonSymbolFind;

	ListBoxSymbol* m_listBoxSymbol;

	virtual void AccountChanged() override;

	int m_editLeftOffset;
	int m_editTopOffset;

	int m_topLineY;
	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;

	bool m_historyLoaded;
	bool m_executorConnected;
	bool m_marketSorterConnected;
	bool m_marketSorterLoaded;

	void SetTrading(const bool trading);
	void StartTrading();
	void StopTrading();
	bool m_trading;
	bool m_tradeOnStart;

	bool m_subscribe;
	bool m_disconnectFromMarketSorter;

	bool m_layoutLoaded;

	bool m_loadOnStart;

	bool OnLoadFrom();
	bool m_loadFromFile;
	bool m_loadingFromMarket;

	bool m_loadFilePathCurrent;
	std::string m_loadFilePath;
	std::string m_controlLoadFilePath;
	void EnableLoadButton();
	void UpdateSymbolDelimiterColor();

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
