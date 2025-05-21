#pragma once

#include "MyMarketSorterFilter.h"
/*
enum MyMarketSorterMenuIDs : unsigned int
{
	MMSID_REAL = TMID_LAST,
	MMSID_TEST,

	MMSID_MARKET_CATEGORY_ALL,
	MMSID_MARKET_CATEGORY_NONE,

	MMSID_Last
};

enum MyRealTestStockEnum : unsigned char
{
	MRTS_REAL = 0,
	MRTS_TEST,

	MRTS_Last
};
*/

//Use this class to access current prints
class MmsUnappliedMessageReader : public LineBufferArrayReader
{
public:
	MmsUnappliedMessageReader(SecurityChartRow* row, const LineBufferArray* bufferArray = nullptr):LineBufferArrayReader(bufferArray), m_row(row){}
	virtual bool ProcessDataToken(const char* cursor, unsigned short length) const override;
protected:
	SecurityChartRow* m_row;
};

//classes representing row cells.
//They keep the old value, and when a new value is applied, it can be determined if the row ha to be removed from the sorted collection and inserted back with the new value at a new location.
//CompareAhead compares the new value of "this" row with the current value of neighboring rows and determines if the removing / inserting is necessary (for efficiency)

class MySecurityPriceRowValue : public PriceRowValue
{
public:
	typedef const ValueType& (Security::*PriceFunction)() const;
	MySecurityPriceRowValue(const Security* const& security, const bool* comma, const unsigned char* decDigits, const bool* truncate, PriceFunction priceFunction, bool useEmptyValue = true, bool useTick = true):
		PriceRowValue(comma, decDigits, truncate, useEmptyValue, useTick),
		m_security(security),
		m_priceFunction(priceFunction){}
	virtual const Price& GetObjectValue() const{return (m_security->*m_priceFunction)();}
	DECLARE_NED_NEW
protected:
	const Security* m_security;
	PriceFunction m_priceFunction;
};

class MySecurityCompactPriceRowValue : public CompactPriceRowValue
{
public:
	typedef const ValueType& (Security::*PriceFunction)() const;
	MySecurityCompactPriceRowValue(const Security* const& security, const bool* comma, const unsigned char* decDigits, const bool* truncate, PriceFunction priceFunction, bool useEmptyValue = true, bool useTick = true):
		CompactPriceRowValue(comma, decDigits, truncate, useEmptyValue, useTick),
		m_security(security),
		m_priceFunction(priceFunction){}
	virtual const ValueType& GetObjectValue() const{return (m_security->*m_priceFunction)();}
	DECLARE_NED_NEW
protected:
	const Security* m_security;
	PriceFunction m_priceFunction;
};

class MySecurityPriceRowValueDiffAsUInt : public SignedPriceRowValue
{
public:
	typedef const Price& (Security::*PriceFunction)() const;
	typedef const unsigned int& (Security::*PriceAsUIntFunction)() const;
	MySecurityPriceRowValueDiffAsUInt(const Security* const& security, const bool* comma, const unsigned char* decDigits, const bool* truncate, PriceFunction priceFunctionFirst, PriceAsUIntFunction priceFunctionLast, bool useEmptyValue = false, bool useTick = true, bool useNeutralColor = false):
		SignedPriceRowValue(comma, decDigits, truncate, useEmptyValue, useTick, useNeutralColor),
		m_security(security),
		m_last(0),
		m_priceFunctionFirst(priceFunctionFirst),
		m_priceFunctionLast(priceFunctionLast)
	{
	}
	SignedPrice CalculateValue() const
	{
		unsigned int last = (m_security->*m_priceFunctionLast)();
		if(last)
		{
			const Price& first = (m_security->*m_priceFunctionFirst)();
			if(!first.isZero())
			{
				return Price(last) - first;
			}
		}
		return SignedPrice::signedPriceZero;
	}
	SignedPrice DeriveValue() const
	{
		if(m_last && !m_first.isZero())
		{
			return Price(m_last) - m_first;
		}
		return SignedPrice::signedPriceZero;
	}
	virtual bool isValueChanged() const
	{
		return m_last != (m_security->*m_priceFunctionLast)() || m_first != (m_security->*m_priceFunctionFirst)();
	}
	virtual short CompareAhead(const RowValue& other) const
	{
		return -((const TypedRowValue&)other).CompareValue(CalculateValue());
	}

	virtual bool UpdateValue()
	{
		m_last = (m_security->*m_priceFunctionLast)();
		m_first = (m_security->*m_priceFunctionFirst)();
		return SetValue(DeriveValue());
	}
	virtual bool isObjectValueZero() const
	{
		return !(m_security->*m_priceFunctionLast)() || (m_security->*m_priceFunctionFirst)().isZero();
	}
	virtual bool isValueZero() const
	{
		return !m_last || m_first.isZero();
	}

	DECLARE_NED_NEW
protected:
	virtual void InitializeValue()
	{
		m_last = (m_security->*m_priceFunctionLast)();
		m_first = (m_security->*m_priceFunctionFirst)();
		m_value = DeriveValue();
	}
	const Security* m_security;
	unsigned int m_last;
	Price m_first;
	PriceFunction m_priceFunctionFirst;
	PriceAsUIntFunction m_priceFunctionLast;
};

class MySecurityPriceRowValueDiffAsUIntDividedByFirst : public SignedPriceRowValue
{
public:
	typedef const Price& (Security::*PriceFunction)() const;
	typedef const unsigned int& (Security::*PriceAsUIntFunction)() const;
	MySecurityPriceRowValueDiffAsUIntDividedByFirst(const Security* const& security, const bool* comma, const unsigned char* decDigits, const bool* truncate, PriceFunction priceFunctionFirst, PriceAsUIntFunction priceFunctionLast, bool useEmptyValue = false, bool useTick = true, bool useNeutralColor = false):
		SignedPriceRowValue(comma, decDigits, truncate, useEmptyValue, useTick, useNeutralColor),
		m_security(security),
		m_last(0),
		m_priceFunctionFirst(priceFunctionFirst),
		m_priceFunctionLast(priceFunctionLast)
	{
	}
	SignedPrice CalculateValue() const
	{
		unsigned int last = (m_security->*m_priceFunctionLast)();
		if(last)
		{
			const Price& first = (m_security->*m_priceFunctionFirst)();
			if(!first.isZero())
			{
				return first.GetPercentChange(Price(last) - first);
			}
		}
		return SignedPrice::signedPriceZero;
	}
	SignedPrice DeriveValue() const
	{
		if(m_last && !m_first.isZero())
		{
			return m_first.GetPercentChange(Price(m_last) - m_first);
		}
		return SignedPrice::signedPriceZero;
	}
	virtual bool isValueChanged() const
	{
		return m_last != (m_security->*m_priceFunctionLast)() || m_first != (m_security->*m_priceFunctionFirst)();
	}
	virtual short CompareAhead(const RowValue& other) const
	{
		return -((const TypedRowValue&)other).CompareValue(CalculateValue());
	}
	virtual bool UpdateValue()
	{
		m_last = (m_security->*m_priceFunctionLast)();
		m_first = (m_security->*m_priceFunctionFirst)();
		return SetValue(DeriveValue());
	}
	virtual bool isObjectValueZero() const
	{
		return !(m_security->*m_priceFunctionLast)() || (m_security->*m_priceFunctionFirst)().isZero();
	}
	virtual bool isValueZero() const
	{
		return !m_last || m_first.isZero();
	}

	DECLARE_NED_NEW
protected:
	virtual void InitializeValue()
	{
		m_last = (m_security->*m_priceFunctionLast)();
		m_first = (m_security->*m_priceFunctionFirst)();
		m_value = DeriveValue();
	}
	const Security* m_security;
	unsigned int m_last;
	Price m_first;
	PriceFunction m_priceFunctionFirst;
	PriceAsUIntFunction m_priceFunctionLast;
};

class MyPositionIntInventoryRowValue : public IntRowValue
{
public:
	typedef const ValueType& (PositionData::*IntFunction)(const bool& inventoryView) const;
	MyPositionIntInventoryRowValue(const PositionData* position, const bool& inventoryView, const bool* comma, IntFunction intFunction, bool useEmptyValue = false, bool useTick = true):
		IntRowValue(comma, useEmptyValue, useTick),
		m_position(position),
		m_intFunction(intFunction),
		m_inventoryView(inventoryView)
		{}
	virtual const ValueType& GetObjectValue() const{return (m_position->*m_intFunction)(m_inventoryView);}
	virtual void SetPosition(const PositionData* position){m_position = position;}
	DECLARE_NED_NEW
protected:
	const PositionData* m_position;
	IntFunction m_intFunction;
	const bool& m_inventoryView;
};

class MyPositionIntInventoryRowValueNull : public MyPositionIntInventoryRowValue
{
public:
	MyPositionIntInventoryRowValueNull(const PositionData* position, const bool& inventoryView, const bool* comma, IntFunction intFunction, bool useEmptyValue = false, bool useTick = true):
		MyPositionIntInventoryRowValue(position, inventoryView, comma, intFunction, useEmptyValue, useTick){}
	virtual const ValueType& GetObjectValue() const{return m_position ? (m_position->*m_intFunction)(m_inventoryView) : zero;}
	DECLARE_NED_NEW
};
/////////////

class MyMarketSorterRow;

class MyMarketSorter : public RowWindowSortableAccount
{
public:
	MyMarketSorter(const CBitmap* bitmap,
		unsigned int workspace,
		bool pinned = false,
		unsigned short builtInColumns = 0,//MS_COLUMN_COUNT,
		int minVisibleSize = 15,
		int minWidth = 20,
		int maxWidth = 0,
		int minHeight = 20,
		int maxHeight = 0,
		int minBorderWidth = 2,
		int maxBorderWidth = 2,
		const char* className = "TakionTool",
		int minTitleHeight = 0,
		int maxTitleHeight = 15,
		int titleHeight = 15);
	static const char* const myMarketSorterId;
	virtual const char* GetToolId() const override{return myMarketSorterId;}
	virtual TakionTool* Clone() const override;
	virtual COLORREF GetBkColor() const override;
	enum ColumnId : unsigned short
	{
		MMS_SYMBOL,
		MMS_EXCHANGE,
		MMS_OPEN_PRICE,
		MMS_CLOSE_PRICE,
		MMS_LAST_TRADE,
		MMS_DAY_HIGH_PRICE,
		MMS_DAY_LOW_PRICE,
		MMS_BID,
		MMS_ASK,
		MMS_MARKET_HOURS_NET_CHANGE,
		MMS_MARKET_HOURS_NET_PERCENT_CHANGE,
		MMS_POS_SIZE,
		MMS_YESTERDAYS_HIGH_PRICE,
		MMS_YESTERDAYS_LOW_PRICE,
		MMS_PREMARKET_HIGH_PRICE,
		MMS_PREMARKET_LOW_PRICE,

		MMS_COLUMN_COUNT
	};
	virtual ~MyMarketSorter();

	virtual void toString(std::string& str, unsigned int contentModifier) const override;
	virtual void fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier) override;
	virtual void fromScratch() override;

	const Security* GetSecurityFromRow(const MyMarketSorterRow* row) const;

	void SelectSymbol(const char* symbol);
//Usually you should not call the following functions. Takion calls them when it needs to.
//Filtering
	bool SecurityPotentiallyBelongs(const Security* security) const;
	bool SecurityRowBelongs(const MyMarketSorterRow* row) const;

	virtual bool ObjectPotentiallyBelongs(const void* object) const override
	{
		return SecurityPotentiallyBelongs((const Security*)object);
	}
	virtual bool RowFiltersIn(const RowSortable* row) const override//should be called inside lock
	{
		return SecurityRowBelongs((const MyMarketSorterRow*)row);
	}
	virtual unsigned char RowBelongs(const RowSortable* row) const override//2 - belongs; 1 - does not belong but can update to become belonging; 0 - does not belong and cannot update to become belonging
	{
		const MyMarketSorterRow* myRow = (const MyMarketSorterRow*)row;
		return SecurityPotentiallyBelongs(GetSecurityFromRow(myRow)) ? SecurityRowBelongs(myRow) ? 2 : 1 : 0;
	}
	virtual unsigned char ExistingRowBelongs(const RowSortable* row) const override//2 - belongs; 1 - does not belong but can update to become belonging; 0 - does not belong and cannot update to become belonging (can't return 0 because the row already exists)
	{
		return SecurityRowBelongs((const MyMarketSorterRow*)row) ? 2 : 1;
	}
//
	virtual void SetAccount(const Account* account) override;
	virtual void AllAccountsDeleted() override;
	virtual void CurrentAccountLoaded() override;
	virtual void CurrentAccountConstraintsLoaded() override;

	virtual unsigned short GetBuiltInColumnCount() const override{return MMS_COLUMN_COUNT;}
	virtual Row* CreateRow(const void* object, bool potential) override;
///////

	const unsigned short& GetTimeFrame() const{return m_timeFrame;}//This value is there only for initialization and passing to the SecurityChartRow subclass

	typedef void (MyMarketSorterRow::*FnPaintInfo)(std::string& text, COLORREF& color) const;//, UINT& horizontalJustification) const;
	typedef short (MyMarketSorterRow::*FnCompare)(const MyMarketSorterRow& other) const;

	bool AddMyMarketSorterFunctions(unsigned short id, FnPaintInfo fnPaint, FnCompare fnCompare, FnCompare fnCompareAhead);

	const FnPaintInfo& GetFnPaintInfo(const unsigned short& id) const{return id < MMS_COLUMN_COUNT ? m_paintInfoFunction[id] : nullptr;}
	const FnCompare& GetFnCompare(const unsigned short& id) const{return id < MMS_COLUMN_COUNT ? m_compareFunction[id] : nullptr;}
	const FnCompare& GetFnCompareAhead(const unsigned short& id) const{return id < MMS_COLUMN_COUNT ? m_compareAheadFunction[id] : nullptr;}

	virtual bool isRowActiveSymbol(const RowSortable* row) const override;
	virtual RowSortable* FindRowActiveSymbol(const unsigned __int64& activeSymbol
#ifndef TAKION_NO_OPTIONS
			,const unsigned __int64& activeOptionBlock
#endif
		) const override;

//	virtual void AddSymbol(const char* symbol) override;
	virtual bool CanClearSymbols() const override;
	virtual void ClearSymbols() override;
	virtual unsigned short GetSymbolColumn() const override{return MMS_SYMBOL;}
protected:

	virtual bool ProcessKeyDown(unsigned char key, unsigned char modifier, bool repeat, TakionFrame* fromWindow) override;
	virtual void AcceptText(const char* text, unsigned int code);

	virtual void PasteStocksFromString(const char* cursor, size_t size, bool replace) override;
	virtual void RemoveSelectedStocks() override;

	void UpdateAccountInfo();
	void UpdatePositionValues(RowSortable* row);
	void RepaintPositionValues();

	void Repopulate();

	virtual void OnSuccessfulCreate() override;
	virtual void ToolAdded() override;
	virtual void OnHidden() override;

	virtual void NextDayStarted() override;
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;

	virtual void LineSelected() override;
	virtual void BeforeDestroy() override;
	virtual void AccountChanged() override;
	virtual void AddMenuItems(CMenu& menu) override;
	virtual void PostAddMenuItems(CMenu& menu) override;
//	virtual void AddRowPropertiesMenuItems(CMenu& popupMenu) override;
//	virtual void PostAddRowPropertiesMenuItems(CMenu& popupMenu) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	virtual void AddDoubleClickActionMenuItems(CMenu& menu) override;

	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;

//	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
//	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
//	DECLARE_MESSAGE_MAP()

	FnPaintInfo m_paintInfoFunction[MMS_COLUMN_COUNT];
	FnCompare m_compareFunction[MMS_COLUMN_COUNT];
	FnCompare m_compareAheadFunction[MMS_COLUMN_COUNT];

	MyMarketSorterFilter m_filter;
	virtual void DoApplyFilter(const TakionFilter& filter, bool forceFilterChanged) override;

	unsigned short m_timeFrame;
/*
	void SwitchMarketCategoryFilter(const unsigned int marketCategoryFlag);
	void AddMarketCategoryFilter(const unsigned int marketCategoryMask);
	void RemoveMarketCategoryFilter(const unsigned int marketCategoryMask);
	void SetMarketCategoryFilter(const unsigned int marketCategoryFilter);
	unsigned int m_marketCategoryFilter;

	void SwitchRealTestFilter(const MyRealTestStockEnum realTest);
	void AddRealTestFilter(const unsigned char realTestMask);
	void RemoveRealTestFilter(const unsigned char realTestMask);
	unsigned char m_realTestFilter;
*/
	void SetMarketSorterConnected(bool connected);
	void SetMarketSorterLoaded(bool loaded);
	bool m_marketSorterConnected;
	COLORREF m_colorDisconnected;
	bool m_marketSorterLoaded;
	COLORREF m_colorNotLoaded;

	virtual void ActiveSymbolAboutToChange(const unsigned __int64& newActiveSymbol
#ifndef TAKION_NO_OPTIONS
		,const unsigned __int64& newActiveOptionBlock
#endif
		) override;
	virtual bool ActiveSymbolChanged() override;
	RowSortable* m_prevTopRow;
	RowSortable* m_nextTopRow;
};

//Row object
class MyMarketSorterRow : public RowSortable, public SecurityChartRow
{
public:
	MyMarketSorterRow(MyMarketSorter* parent, Security* security, const Position* position, bool potential = false);
	virtual ~MyMarketSorterRow();
	virtual const void* GetBaseObject() const override{return m_security;}
//	virtual bool PaintCell(CDC& dc, const Column* column, const CRect& rect, UINT verticalJustification, int horizontalOffset) const override;//, unsigned char horizontalAlignment) const;
//	virtual void CellToString(std::string& info, unsigned short columnId) const override;

	virtual COLORREF GetRowColor() const override;
	virtual COLORREF GetCellColor(unsigned short columnId) const override;
	virtual COLORREF GetCellBkColor(unsigned short columnId) const override;
	virtual COLORREF GetRowBkColor() const override;

	virtual RowValue* CreateCell(unsigned short id) override;

	virtual bool Lock(bool wait) const override;
//	virtual void LockWait() const override;
	virtual void Unlock() const override;

	virtual bool SetPosition(const Position* position);

//There may be some columns that require special handling. Otherwise you don't have to redifine these functions
//	virtual bool isSortValueChanged() const override;
//	virtual bool isChainSortValueChanged() const override;
//	virtual short Compare(const RowSortable& other) const override;
//	virtual short CompareAhead(const RowSortable& other) const override;
//	virtual short CompareChain(const RowSortable& other) const override;
//	virtual short CompareChainAhead(const RowSortable& other) const override;
//

	virtual short PreCompare(const RowSortable& other) const;//provide this function if you want to override the column sorting (Takion uses it for "TopActive" functionality)

	virtual short DefaultCompare(const RowSortable& other) const override;//Should return 0 for an object compared to itself only. Otherwise it must return -1 or 1.
//You can compare using functions, or Cells (created through SetRowValue)
//Using functions is convenient if the value in a column does not change (like "Symbol"). If the value changes, using Cells is easier.
//If the value in a column does not, then CompareFunction and CompareFunctionAhead is the same for the column.
//You can provide Cells for some columns and Functions for othres.
	virtual bool CompareFunction(const RowSortable& other, unsigned short sortId, short& ret) const override;
	virtual bool CompareFunctionAhead(const RowSortable& other, unsigned short sortId, short& ret) const override;
	virtual bool GetPaintTextInfo(unsigned short columnId, std::string& text, COLORREF& color) const override;//, UINT& horizontalJustification) const override;

	virtual void Update(bool dynamicOnly) override;
	virtual bool UpdateRowColor() override;//should be called only inside a lock
	virtual void UpdateAndInvalidate(CRect& rowRect, bool dynamicOnly) override;

	virtual void Initialize() override;//should be called only inside a lock

//	virtual const char* GetRowSecuritySymbol(const CPoint* point = nullptr) const override;

	DECLARE_NED_NEW

	virtual const Security* GetSelectedSecurity() const override;

	MyMarketSorter* GetMyMarketSorterWindow(){return m_myMarketSorterWindow;}
	const MyMarketSorter* GetMyMarketSorterWindow() const{return m_myMarketSorterWindow;}

	void PaintSymbol(std::string& text, COLORREF& color) const;//, UINT& horizontalJustification) const;
	short CompareSymbol(const MyMarketSorterRow& other) const;
	void PaintExchange(std::string& text, COLORREF& color) const;//, UINT& horizontalJustification) const;
	short CompareExchange(const MyMarketSorterRow& other) const;

	virtual const char* GetRowSecuritySymbol(const CPoint* point = nullptr) const override{return m_security ? m_security->GetSymbol() : nullptr;}
	virtual const unsigned __int64& GetRowNumericSymbol() const{return m_security ? m_security->GetNumericSymbol() : uint64Zero;}
	virtual const PositionData* GetRowPosition(const CPoint* point = nullptr) const override{return m_position;}

	const unsigned __int64& GetNumericSymbol() const{return m_security ? m_security->GetNumericSymbol() : uint64Zero;}
protected:
	void UpdateSecurity();

	bool UpdatePrints(bool lock);
	virtual void Notify(const Message* message, const Observable* from, const Message* info = nullptr) override;

	MyMarketSorter* m_myMarketSorterWindow;
	COLORREF m_securityColor;
	MmsUnappliedMessageReader m_unappliedMessageReader;
};

