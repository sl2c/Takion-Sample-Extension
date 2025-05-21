#pragma once

#include "BaseSampleDialogSettingSymbol.h"
//#include "ConfigEnums.h"

class HistPrintItem
{
public:
	HistPrintItem(const unsigned __int64& referenceNumber,
		const Price& price,
		const unsigned int& size,
		const unsigned int millisecond,
		const unsigned int& mmid,
		const unsigned char& source,
		const char& saleCondition1,
		const char& saleCondition2,
		const char& saleCondition3,
		const char& saleCondition4):
		m_referenceNumber(referenceNumber),
		m_price(price),
		m_money(price * size),
		m_size(size),
		m_time(millisecond),
		m_mmid(mmid),
		m_source(source),
		m_sourceChar('A' + source),
		m_saleCondition1(saleCondition1),
		m_saleCondition2(saleCondition2),
		m_saleCondition3(saleCondition3),
		m_saleCondition4(saleCondition4)
	{
	}
	const unsigned __int64& GetReferenceNumber() const{return m_referenceNumber;}
	const unsigned int& GetTime() const{return m_time;}
	const Price& GetPrice() const{return m_price;}
	const Money& GetMoney() const{return m_money;}
	const unsigned int& GetSize() const{return m_size;}
	const unsigned int& GetMmid() const{return m_mmid;}
	const unsigned char& GetSource() const{return m_source;}
	const char& GetSourceChar() const{return m_sourceChar;}
	const char& GetSaleCondition1() const{return m_saleCondition1;}
	const char& GetSaleCondition2() const{return m_saleCondition2;}
	const char& GetSaleCondition3() const{return m_saleCondition3;}
	const char& GetSaleCondition4() const{return m_saleCondition4;}
protected:
	unsigned __int64 m_referenceNumber;
	Price m_price;
	Money m_money;
	unsigned int m_size;
	unsigned int m_time;
	unsigned int m_mmid;
	unsigned char m_source;
	char m_sourceChar;
	char m_saleCondition1;
	char m_saleCondition2;
	char m_saleCondition3;
	char m_saleCondition4;
};

class SampleDialogSettingHistPrint;

enum HistPrintColumnId : unsigned short
{
	HPCI_SYMBOL,
	HPCI_TIME,
	HPCI_PRICE,
	HPCI_SIZE,
	HPCI_SALE_CONDITION_1,
	HPCI_SALE_CONDITION_2,
	HPCI_SALE_CONDITION_3,
	HPCI_SALE_CONDITION_4,
	HPCI_EXCHANGE,
	HPCI_SOURCE,
	HPCI_REF_NUMBER,

	HPCI_Count,
};

class ListBoxHistPrint : public ColumnedListBox
{
friend class SampleDialogSettingHistPrint;
public:
	ListBoxHistPrint(SampleDialogSettingHistPrint* owner):
		ColumnedListBox(0, true, true),
		m_diconnectColor(RGB(255, 0, 0)),
		m_loadingColor(RGB(255, 255, 0)),
		m_diconnectBrush(TakionDialogInterface::m_visualBase->GetSolidBrush(m_diconnectColor)),
		m_loadingBrush(TakionDialogInterface::m_visualBase->GetSolidBrush(m_loadingColor)),
		m_histPrintConnected(false),
		m_histPrintLoaded(true),
		m_owner(owner),
		m_totalSize(0)
	{
	}
//	virtual void DestroyItem(const void* item) const override;
	virtual const char* GetItemName(const void* item) const override;
/*
	bool RemoveRowAt(int found);
	bool RemoveRow(const OrderObserver* item);
	bool RemoveOrder(const Order* order);
	void RemoveItemAt(int found, const OrderObserver* item);
*/
	bool SetHistPrintConnected(const bool connected);
	const bool& isHistPrintConnected() const{return m_histPrintConnected;}
	void SetHistPrintLoaded(const bool historyLoaded);
	const bool& isHistoryLoaded() const{return m_histPrintLoaded;}

	SampleDialogSettingHistPrint* GetOwnerDialog(){return m_owner;}
	const SampleDialogSettingHistPrint* GetOwnerDialog() const{return m_owner;}

	virtual bool HasItemToolTips() const override{return false;}

	const COLORREF& GetDisconnectColor() const{return m_diconnectColor;}
//	const COLORREF& GetLoadedColor() const{return m_loadedColor;}
	const COLORREF& GetLoadingColor() const{return m_loadingColor;}

	virtual CBrush* GetBkBrush() const override
	{
		return m_histPrintConnected ?
			m_histPrintLoaded ? nullptr : m_loadingBrush:
			m_diconnectBrush;
	}
//	virtual COLORREF GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	virtual COLORREF GetBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override
	{
		return m_histPrintConnected ?
			m_histPrintLoaded ? ColumnedListBox::GetBkColor(lpDrawItemStruct) : m_loadingColor:
			m_diconnectColor;
	}

	void RepaintCount() const;
	void RepaintTotalSize() const;
	void RepaintVWAP() const;
//Totals
	const unsigned __int64& GetTotalSize() const{return m_totalSize;}
	const Money& GetTotalMoney() const{return m_totalMoney;}
	const Money& GetVWAP() const{return m_vwap;}
	bool UpdateSize(const unsigned __int64& oldValue, const unsigned __int64& newValue)//In case there is a correction to process
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_totalSize))
		{
			RepaintTotalSize();
			return true;
		}
		return false;
	}
	bool UpdateMoney(const Money& oldValue, const Money& newValue)//In case there is a correction to process
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_totalMoney))
		{
			if(m_totalSize)
			{
				m_vwap = m_totalMoney / m_totalSize;
			}
			else
			{
				m_vwap.SetZero();
			}
			RepaintVWAP();
			return true;
		}
		return false;
	}
	virtual int Compare(const void* item1, const void* item2) const override;
//	int FindHistPrintByRefNumber(const unsigned __int64 refNumber) const;
	typedef CMap<unsigned __int64, const unsigned __int64&, const HistPrintItem*, const HistPrintItem* const&> HistoricalPrintItemMap;
	const HistPrintItem* FindItemByRefNumber(const unsigned __int64& ref) const
	{
		const HistoricalPrintItemMap::CPair* found = m_historicalPrintItemMapVisible.PLookup(ref);
		return found ? found->value : nullptr;
	}
	bool ClearItemsWithRedraw()
	{
		ClearAndDestroyPrints();
		ResetTotals();
		return m_hWnd && ColumnedListBox::ClearItemsWithRedraw();
	}
	bool AddItem(const HistPrintItem* const& item)
	{
		if(!m_historicalPrintItemMapVisible.PLookup(item->GetReferenceNumber()))
		{
			DoAddItem(item);
			return true;
		}
		return false;
	}
	int FindLbItemByRefNumber(const unsigned __int64& refNumber) const
	{
		const HistPrintItem* item;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			item = (const HistPrintItem*)GetItemDataPtr(i);
			if(item->GetReferenceNumber() == refNumber)
			{
				return i;
			}
		}
		return -1;
	}
	const HistPrintItem* RemoveItem(const unsigned __int64& refNumber)
	{
		HistoricalPrintItemMap::CPair* found = m_historicalPrintItemMapVisible.PLookup(refNumber);
		if(found)
		{
			int foundOrdinal = FindLbItemByRefNumber(refNumber);
			if(foundOrdinal >= 0)
			{
				DoRemoveItem(foundOrdinal, refNumber);
			}
			return found->value;
		}
		return nullptr;
	}
protected:
	void DoAddItem(const HistPrintItem* const& item)
	{
		m_historicalPrintItemMapVisible.SetAt(item->GetReferenceNumber(), item);
		AddString((const char*)item);
		UpdateSize(0, item->GetSize());
		UpdateMoney(Money::moneyZero, item->GetMoney());
		RepaintCount();
	}
	void DoRemoveItem(const unsigned int& i, const unsigned __int64& refNumber)
	{
		m_historicalPrintItemMapVisible.RemoveKey(refNumber);
		HistPrintItem* item = (HistPrintItem*)GetItemDataPtr(i);
		UpdateSize(item->GetSize(), 0);
		UpdateMoney(item->GetMoney(), Money::moneyZero);
		DeleteString(i);
		RepaintCount();
		delete item;
	}
//	HistoricalPrintItemMap m_historicalPrintItemMap;
	HistoricalPrintItemMap m_historicalPrintItemMapVisible;
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
//	virtual void HorizontalScrollingDone(int diff) override;
//	virtual void DoCreateColumnToolTips() override
	void ClearAndDestroyPrints();

	virtual void BeforeDestroy()
	{
		ClearAndDestroyPrints();
	}
//	virtual void SizeChanged(bool horizontalChange, bool verticalChange) override;
//	virtual void ListBoxCreated() override;
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;
//	virtual void AddMenuItems(CMenu& menu) override;
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;

	COLORREF m_diconnectColor;
//	COLORREF m_loadedColor;
	COLORREF m_loadingColor;
	CBrush* m_diconnectBrush;
	CBrush* m_loadingBrush;
	bool m_histPrintConnected;
	bool m_histPrintLoaded;
	SampleDialogSettingHistPrint* m_owner;

//totals
	void ResetTotals();

	unsigned __int64 m_totalSize;
	Money m_totalMoney;
	Money m_vwap;
};

class ListBoxHistPrintSource : public ListBoxFilterString<unsigned int>//, 8>
{
public:
	ListBoxHistPrintSource()
//		:ListBoxSimpleFilter((1 << FOST_Count) - 1, FOST_Count)
	{
	}
//	virtual const char* GetItemName(const void* item) const override;
};

class SampleDialogSettingHistPrint : public BaseSampleDialogSettingSymbol//, public Observer
{
public:
	SampleDialogSettingHistPrint(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
//	virtual ~SampleDialogSettingOrder(){}
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;

	void RepaintCount() const;
	void RepaintTotalSize() const;
	void RepaintVWAP() const;

	void RepaintTotals() const;

	virtual void RestoreLayout() override;

	int GetListBoxBorderSize() const{return m_listBoxBorderSize;}
	int GetLeftOffset() const{return m_leftOffset;}
	int GetTopOffset() const{return m_topOffset;}

	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

//	unsigned int GetHistPrintFilter() const;
	bool HistPrintBelongs(const HistoricalPrint* print) const;
	bool HistPrintItemBelongs(const HistPrintItem* print) const;
//	static unsigned int CalculateTakionOrderStatusMask(unsigned char orderStatusMask);
//	static unsigned int CalculateTakionOrderSideMask(unsigned char orderSideMask);
	void ReloadCurrent()
	{
		if(!m_symbol.empty())
		{
			Reload(m_symbol.c_str(), false);
		}
	}
protected:
	virtual void ToolTipsEnabled(bool enable) override;

	virtual BOOL OnInitDialog() override;

	void ClearHistPrints();

	void EnableLoadButton();
	void EnableStopButton();
	bool ApplySymbol();
	bool CanApplySymbol() const;

//	void AddNewOrder(Order* order);
//	void AddOrder(Order* order);

//	const Position* m_position;
//	std::string m_symbol;
	std::string m_controlSymbol;
	bool m_symbolCurrent;
/*
	void AddOrderByFilter(unsigned char addStatusFilter, unsigned char addSideFilter);

	void UpdateOrderStatusMask();
	void RemoveOrderStatus(unsigned char removeFilter);
	unsigned char m_orderStatusMask;//enum FilterOrderStatus in ConfigEnums.h

	void UpdateOrderSideMask();
	void RemoveOrderSide(unsigned char removeFilter);
	unsigned char m_orderSideMask;//enum FilterOrderSide in ConfigEnums.h

	void RemoveOrdersButSymbol();
*/
//	virtual void TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff) override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnLoad();
	afx_msg void OnStop();
	afx_msg void OnOddLot();
//	afx_msg void OnSelchangeOrder();
	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);

	DECLARE_MESSAGE_MAP()

//	ValidEdit m_EditSymbol;
	CButton m_ButtonLoad;
	CButton m_ButtonStop;
	CStatic m_StaticSymbol;
//	ComboBoxAccount m_ComboBoxAccount;

	CheckBoxOwnerDraw m_CheckBoxOddLot;

	ListBoxHistPrintSource m_ListBoxHistPrintSource;

	ListBoxHistPrint m_ListBoxHistPrint;

	bool m_printShowOddLot;

	bool UpdateFilter();
	unsigned int m_filter;

	unsigned int m_requestId;
	unsigned __int64 m_nextRef;
	unsigned short m_nextMinute;

	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;

	int m_topFilter;
	int m_widthFilter;

	bool AddPrint(const unsigned __int64& referenceNumber,
		const Price& price,
		unsigned int size,
		unsigned int millisecond,
		unsigned char source,
		char saleCondition1,
		char saleCondition2,
		char saleCondition3,
		char saleCondition4);
//		bool newPrint);

	void PopulatePrints();
	void PopulatePrints(const Security* security);
	void RemoveNotBelongingPrints();
	void UpdatePrints(const bool remove, const bool add);
	void StopLoadingPrints();
	void Reload(const char* symbol, bool changed);
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
