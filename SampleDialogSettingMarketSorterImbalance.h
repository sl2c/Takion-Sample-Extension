#pragma once

#include "BaseDialogSetting.h"

class SampleDialogSettingMarketSorterImbalance;
class StockImbalance;

enum MarketSorterImbalanceColumnId : unsigned short
{
	MSI_SYMBOL,
	MSI_EXCHANGE,
	MSI_ORDINAL,
	MSI_LOG,

	MSI_NYSE_IMBALANCE_DELAY,
	MSI_NYSE_IMBALANCE_INFO,
	MSI_NYSE_IMBALANCE_INFO_TIME,
	MSI_NYSE_FAR_PRICE,
	MSI_NYSE_NEAR_PRICE,
	MSI_NYSE_REF_PRICE,
	MSI_NYSE_PAIRED_SHARES,

	MSI_NYSE_CROSS_TYPE,

	MSI_NYSE_IMBALANCE_DELAY_REGULATORY,
	MSI_NYSE_IMBALANCE_REGULATORY,
	MSI_NYSE_IMBALANCE_REGULATORY_TIME,
	MSI_NYSE_FAR_PRICE_REGULATORY,
	MSI_NYSE_NEAR_PRICE_REGULATORY,
	MSI_NYSE_REF_PRICE_REGULATORY,
	MSI_NYSE_PAIRED_SHARES_REGULATORY,

//NSDQ
	MSI_NSDQ_IMBALANCE_DELAY,
	MSI_NSDQ_IMBALANCE_INFO,
	MSI_NSDQ_IMBALANCE_INFO_TIME,
	MSI_NSDQ_FAR_PRICE,
	MSI_NSDQ_NEAR_PRICE,
	MSI_NSDQ_REF_PRICE,
	MSI_NSDQ_PAIRED_SHARES,
	MSI_NSDQ_CROSS_TYPE,
	MSI_NSDQ_PRICE_VARIATION_INDICATOR,
//ARCA
	MSI_ARCA_IMBALANCE_DELAY,
	MSI_ARCA_IMBALANCE_MARKET,
	MSI_ARCA_IMBALANCE_INFO,
	MSI_ARCA_IMBALANCE_INFO_TIME,
	MSI_ARCA_FAR_PRICE,
	MSI_ARCA_NEAR_PRICE,
	MSI_ARCA_REF_PRICE,
	MSI_ARCA_PAIRED_SHARES,
	MSI_ARCA_CROSS_TYPE,

	MSI_MAIN_THREAD_IDLE_IMBALANCE_DELAY,

	MSI_YEST_VOLUME,

	MSI_Count,
};

enum StockTestOrReal : unsigned char
{
	STTR_REAL,
	STTR_TEST,

	STTR_Count
};

class ListBoxImbalance : public ColumnedListBox
{
public:
	ListBoxImbalance(SampleDialogSettingMarketSorterImbalance* owner, Account* account):
		ColumnedListBox(0, true, true),
		m_owner(owner),
//		m_loadedColor(0),
//		m_unloadedColor(RGB(128, 0, 0)),
		m_inThread(false),
		m_marketSorterConnected(false),
		m_marketSorterLoaded(false),
		m_currentAccount(account),
		m_logCount(0),
		m_menuWidth(0)
	{}
	virtual bool HasItemToolTips() const override{return false;}

	void SetInThread(const bool inThread);

	void ResetImbalances();

	void SetMarketSorterConnected(const bool connected);
	bool isMarketSorterConnected() const{return m_marketSorterConnected;}

	void SetMarketSorterLoaded(const bool loaded);
	bool isMarketSorterLoaded() const{return m_marketSorterLoaded;}

	void MarketSorterCleanup();

	void CurrentAccountSet(Account* account);
	Account* GetCurrentAccount() const{return m_currentAccount;}

	virtual void DestroyItem(const void* item) const override;
	virtual const char* GetItemName(const void* item) const override;

	void RepaintLogCount() const;
	void RepaintTotalCount() const;
	int RemoveItem(unsigned int ordinal);
	bool ClearItems()
	{
		if(m_logCount)
		{
			m_logCount = 0;
			RepaintLogCount();
		}
		RepaintTotalCount();
		return ClearItemsWithRedraw();
	}
	int AddItem(const StockImbalance* item);
	int AddSecurity(const Security* security);

	const unsigned int& GetLogCount() const{return m_logCount;}

	void RefreshOrdinals();
	void IncrementOrdinals(unsigned int i);

	bool CheckOrdinalIntegrity() const;
protected:
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;

	void UpdateStockAccount(StockImbalance* stock);

	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;
	virtual void AddMenuItems(CMenu& menu) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;

	virtual void ListBoxCreated() override;

	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;
	SampleDialogSettingMarketSorterImbalance* m_owner;

//	COLORREF m_loadedColor;
//	COLORREF m_unloadedColor;
	bool m_inThread;
	bool m_marketSorterConnected;
	bool m_marketSorterLoaded;
	Account* m_currentAccount;

	unsigned int m_logCount;

	int m_menuWidth;

	afx_msg LRESULT OnItemRepaint(WPARAM ordinal, LPARAM columns);
	DECLARE_MESSAGE_MAP()

private:
	int AddString(LPCTSTR lpszItem){return ColumnedListBox::AddString(lpszItem);}
	int DeleteString(unsigned int index){return ColumnedListBox::DeleteString(index);}
	void ResetContent(){ColumnedListBox::ResetContent();}
	bool ClearItemsWithRedraw(){return ColumnedListBox::ClearItemsWithRedraw();}
};

class SampleDialogSettingMarketSorterImbalance : public BaseDialogSetting, public Observer
{
friend class ListBoxImbalance;
public:
	SampleDialogSettingMarketSorterImbalance(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, Account* account);
	virtual ~SampleDialogSettingMarketSorterImbalance();
	virtual HWND GetFirstTabControl() const override;

	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;

	bool isFilterChanged() const;

	bool SecurityBelongs(const Security* security) const;

	void CurrentAccountSet(Account* account);
	Account* GetCurrentAccount() const{return m_currentAccount;}

	void RepaintTotalCount() const;
	void RepaintLogCount() const;
protected:
	virtual void ToolTipsEnabled(bool enable) override;

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	ListBoxImbalance m_ListBoxImbalance;
	CheckBoxOwnerDraw m_CheckBoxInThread;
	CheckBoxOwnerDraw m_CheckBoxDblClkSendSymbolToMm;
	ValidEdit m_EditSymbol;
	CButton m_ButtonFind;
	CButton m_ButtonReset;
	CButton m_ButtonLoad;

	ComboBoxChar m_ComboBoxCharFrom;
	ComboBoxChar m_ComboBoxCharTo;

	void AddVolumeSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinYestVolumeFrom;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinYestVolumeTo;

	ListBoxMarketCategory m_ListBoxMarketCategory;

	CStatic m_StaticTestOrReal;
	ListBoxFilterString<unsigned int> m_ListBoxTestOrReal;

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeImbalance();
	afx_msg void OnDblClkImbalance();
	afx_msg void OnInThread();
	afx_msg void OnSendSymbolToMm();
	afx_msg void OnFind();
	afx_msg void OnReset();
	afx_msg void OnLoad();
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnSelchangeCharFrom();
	afx_msg void OnSelchangeCharTo();
	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	afx_msg LRESULT OnListBoxChecked(WPARAM listBoxId, LPARAM l);

	DECLARE_MESSAGE_MAP()

	Account* m_currentAccount;

	void SetMarketSorterConnected(const bool connected);
	void SetMarketSorterLoaded(const bool loaded);
	bool m_marketSorterConnected;
	bool m_marketSorterLoaded;

	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

	int AddSecurityIfBelongs(const Security* security);
	void AddSecurityIfBelongsAndUpdateOrdinals(const Security* security);

//	static const int spinWidth = 32;
	static const int itemHeight = 8;

	void ResetOffsets();
	int m_listBoxBorderSize2;
	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;

	std::string m_symbol;

	void EnableLoadButton();
	void DoEnableLoadButton(bool enable);

	void Load();

	unsigned int m_marketCategoryFilter;
	unsigned int m_testStockFilter;//enum StockTestOrReal; 1 - real; 2 - test; 3 - both

	void YestVolumeChange();
	unsigned __int64 m_yestVolumeFrom;
	unsigned __int64 m_yestVolumeTo;

	void SelchangeChar();
	char GetCtrlCharFrom() const;
	char GetCtrlCharTo() const;
	char m_charFrom;
	char m_charTo;

	bool m_initialSearchDone;

	void SetInThread(const bool inThread);
	bool m_inThread;

	void SetSendToMm(const bool sendToMm);
	bool m_sendToMm;

	TakionIterator* m_allStockterator;

	bool DoReset();

	void EnableFindButton();
	bool Find();

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};