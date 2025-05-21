#pragma once

//class for handling the Price Difference row cell
class SecurityPriceAsUIntRowValueDiffNull : public SignedPriceRowValue
{
public:
	typedef const unsigned int& (Security::*PriceFunction)() const;
	SecurityPriceAsUIntRowValueDiffNull(const Security* const& security, const Security* const& security2, const bool* comma, const unsigned char* decDigits, const bool* truncate, PriceFunction priceFunction, bool useEmptyValue = true, bool useTick = true, bool useNeutralColor = false):
		SignedPriceRowValue(comma, decDigits, truncate, useEmptyValue, useTick, useNeutralColor),
		m_security(security),
		m_security2(security2),
		m_priceFunction(priceFunction),
		m_price(0),
		m_price2(0)
	{
	}

	static void CalculatePrice(unsigned int price, unsigned int price2, SignedPrice& value);

	virtual bool isValueChanged() const override;
	virtual short CompareAhead(const RowValue& other) const override;
	virtual bool UpdateValue() override;
	virtual bool isObjectValueEmpty() const override;
	virtual bool isValueEmpty() const override;
	DECLARE_NED_NEW
protected:
	virtual void InitializeValue() override;
	const Security* const& m_security;
	const Security* const& m_security2;
	PriceFunction m_priceFunction;
	unsigned int m_price;
	unsigned int m_price2;
};

//Stock Pair identifier
class PairKey
{
public:
	PairKey(const unsigned __int64& first = 0, const unsigned __int64& second = 0)
	{
		if(U_CompareUIntAsString(first, second) < 0)
		{
			m_first = first;
			m_second = second;
		}
		else
		{
			m_first = second;
			m_second = first;
		}
	}
	bool operator==(const PairKey& other) const{return m_first == other.m_first && m_second == other.m_second;}
	bool operator<(const PairKey& other) const{return m_first < other.m_first || m_first == other.m_first && m_second < other.m_second;}
	bool operator>(const PairKey& other) const{return m_first > other.m_first || m_first == other.m_first && m_second > other.m_second;}
	bool operator<=(const PairKey& other) const{return !operator>(other);}
	bool operator>=(const PairKey& other) const{return !operator<(other);}
	bool operator!=(const PairKey& other) const{return !operator==(other);}
	
	size_t GetHashValue() const
	{
#ifdef _WIN64
		return ((unsigned __int64)(unsigned int)m_second << 32) | (unsigned int)m_first;
#else
		return (unsigned int)m_second + (unsigned int)m_first;
#endif
	}

	const unsigned __int64& GetFirst() const{return m_first;}
	const unsigned __int64& GetSecond() const{return m_second;}

	void SetFirst(const unsigned __int64& first){m_first = first;}
	void SetSecond(const unsigned __int64& second){m_second = second;}

	bool isValid() const{return m_first && m_second;}
protected:
	unsigned __int64 m_first;
	unsigned __int64 m_second;
};

template<> AFX_INLINE UINT AFXAPI HashKey<PairKey&>(PairKey& key)
{
#ifdef _WIN64
	size_t val = key.GetHashValue();
	// (algorithm copied from STL hash in xfunctional)
	return (HashKey<DWORD>((DWORD)(val & 0xffffffffUL)) ^ HashKey<DWORD>((DWORD)(val >> 32)));
#else
	return key.GetHashValue();
#endif
}

template<> inline size_t stdext::hash_value<PairKey>(const PairKey& key)
{	// hash _Keyval to size_t value one-to-one
	return key.GetHashValue() ^ _HASH_SEED;
}

class StockPair : public PairKey
{
public:
	StockPair(const unsigned __int64& first, const unsigned __int64& second, const SignedPrice& value):
		PairKey(first, second),
		m_security1(TD_FindSortableEquityByNumericSymbol(m_first)),
		m_security2(TD_FindSortableEquityByNumericSymbol(m_second)),
		m_value(value)
	{
	}
	StockPair(const PairKey& key, const SignedPrice& value):
		PairKey(key),
		m_security1(TD_FindSortableEquityByNumericSymbol(m_first)),
		m_security2(TD_FindSortableEquityByNumericSymbol(m_second)),
		m_value(value)
	{
	}
	StockPair(const Security* security1, const Security* security2, const SignedPrice& value):
		PairKey(security1->GetNumericSymbol(), security2->GetNumericSymbol()),
		m_security1(security1),
		m_security2(security2),
		m_value(value)
	{
	}
	virtual ~StockPair()
	{
		if(m_security1)TD_ReleaseStock(m_security1);
		if(m_security2)TD_ReleaseStock(m_security2);
	}
	const Security* GetSecurity1() const{return m_security1;}
	const Security* GetSecurity2() const{return m_security2;}
	const Security* const& GetSecurityRef1() const{return m_security1;}
	const Security* const& GetSecurityRef2() const{return m_security2;}
	const Security* GetSecurity(bool first) const{return first ? m_security1 : m_security2;}
	const Security* const& GetSecurityRef(bool first) const{return first ? m_security1 : m_security2;}

	unsigned char WhichSecurity(const unsigned __int64& numericSymbol) const
	{
		return m_first == numericSymbol ? 1:
			m_second == numericSymbol ? 2:
			0;
	}
	unsigned char SetSecurity(const Security* const& security)
	{
		unsigned __int64 numericSymbol = security->GetNumericSymbol();
		if(m_first == numericSymbol)
		{
			if(security != m_security1)
			{
				if(m_security1)TD_ReleaseStock(m_security1);
				m_security1 = security;
				if(m_security1)m_security1->IncrementUnsubscribable();
			}
			return 1;
		}
		else if(m_second == numericSymbol)
		{
			if(security != m_security2)
			{
				if(m_security2)TD_ReleaseStock(m_security2);
				m_security2 = security;
				if(m_security2)m_security2->IncrementUnsubscribable();
			}
			return 2;
		}
		return 0;
	}
	bool SetValue(const SignedPrice& value)
	{
		if(m_value != value)
		{
			m_value = value;
			return true;
		}
		return false;
	}
	const SignedPrice& GetValue() const{return m_value;}
protected:
	const Security* m_security1;
	const Security* m_security2;
	SignedPrice m_value;
};

class StockComparatorRow;

class SecurityChartRow2 : public SecurityChartRow
{
public:
	SecurityChartRow2(StockComparatorRow* row, const Security* security, Position* position, unsigned short timeFrame, unsigned short timeFrameInSeconds):
		SecurityChartRow(security, position, timeFrame, timeFrameInSeconds),
		m_row(row)
	{
	}
protected:
	virtual void Notify(const Message* message, const Observable* from, const Message* info = nullptr) override;
	StockComparatorRow* m_row;
};

class ScUnappliedMessageReader : public LineBufferArrayReader
{
public:
	ScUnappliedMessageReader(SecurityChartRow* row, const LineBufferArray* bufferArray = nullptr):LineBufferArrayReader(bufferArray), m_row(row){}
	virtual bool ProcessDataToken(const char* cursor, unsigned short length) const override;
protected:
	SecurityChartRow* m_row;
};

class StockPairCollection
{
public:
	StockPairCollection(){}
#if (_MSC_VER > 1600)
	typedef std::unordered_set<StockPair*> StockPairSet;
#else
	typedef std::hash_set<StockPair*> StockPairSet;
#endif
	StockPairSet& GetStockPairSet(){return m_stockPairSet;}
	const StockPairSet& GetStockPairSet() const{return m_stockPairSet;}
	bool AddStockPair(StockPair* stockPair){return m_stockPairSet.insert(stockPair).second;}
	bool RemoveStockPair(StockPair* stockPair){return m_stockPairSet.erase(stockPair) != 0;}
	void Clear(){m_stockPairSet.clear();}
	bool isEmpty() const{return m_stockPairSet.empty();}
protected:
	StockPairSet m_stockPairSet;
};

class StockComparator : public RowWindowSortableAccount
{
public:
	StockComparator(const CBitmap* bitmap,
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
	static const char* const stockComparatorId;
	static const char* const stockFileExtension;
	virtual const char* GetToolId() const override{return stockComparatorId;}
	virtual TakionTool* Clone() const override;
	virtual COLORREF GetBkColor() const override;
	enum ColumnId : unsigned short
	{
		SC_SYMBOL_1,
		SC_SYMBOL_2,
		SC_PRICE_1,
		SC_PRICE_2,
		SC_PRICE_DIFF,
		SC_VALUE,
		SC_POS_SIZE_1,
		SC_POS_SIZE_2,

		SC_COLUMN_COUNT
	};
	virtual ~StockComparator();
	virtual unsigned short GetBuiltInColumnCount() const override{return SC_COLUMN_COUNT;}
//	virtual unsigned short GetAdditionalColumnCount() const override;
	virtual void toString(std::string& str, unsigned int contentModifier) const override;
	virtual void fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier) override;
	virtual void fromScratch() override;
	virtual Row* CreateRow(const void* object, bool potential) override;
	virtual void SetAccount(const Account* account) override;

//	virtual bool CanPopulateSelectedIntoActiveCommandWindow() const override{return true;}

//	virtual void AccountDeleted(const Account* account) override;//defined in RowWindowSortableAccount 
//	virtual void NewAccount(Account* account) override;//defined in RowWindowSortableAccount 
//	virtual void CurrentAccountConstraintsLoaded() override;//defined in RowWindowSortableAccount
	virtual void AllAccountsDeleted() override;
	virtual void CurrentAccountLoaded() override;
	virtual void CurrentAccountConstraintsLoaded() override;

	unsigned short GetTimeFrame() const{return m_timeFrame;}

	void ApplyValue(RowSet& rowSet, const SignedPrice& value);
	void ApplyValue(StockComparatorRow* row, const SignedPrice& value);

	bool isSortByValue() const;
	bool isSortBySecurity(bool firstSecurity) const;
	bool isSortByPosition(bool firstSecurity) const;

	void RemoveSelected();
	void ClearAll();

	typedef CMap<PairKey, PairKey&, StockPair*, StockPair*> StockPairMap;
	typedef CMap<unsigned __int64, unsigned __int64, StockPairCollection*, StockPairCollection*> SymbolStockPairMap;
	StockPairCollection* FindStockPairCollection(const unsigned __int64& symbol){SymbolStockPairMap::CPair* found = m_symbolStockPairMap.PLookup(symbol); return found ? found->value : nullptr;}

	typedef CMap<PairKey, PairKey&, SignedPrice, SignedPrice&> StockPairValueMap;
	void AddRows(const StockPairValueMap& stockPairValueMap, bool replace);
	virtual void PasteStocksFromString(const char* cursor, size_t size, bool replace) override;

	const std::string& GetOpenFileName() const{return m_openFileName;}
	const std::string& GetSaveFileName() const{return m_saveFileName;}
protected:
	void ApplyNewSecurity(StockComparatorRow* row, const Security* security, unsigned char whichSecurity);
	void ApplyNewPosition(StockComparatorRow* row, const Position* position, unsigned char whichSecurity);

	void UpdateAccountInfo();
	void UpdatePositionValues(RowSortable* row);
	void RepaintPositionValues();

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

	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;

	afx_msg LRESULT OnAddStockPair(WPARAM startMinute, LPARAM lengthMinute);
	DECLARE_MESSAGE_MAP()

	void ClearComparatorRowsTemporarily(RowSortableHashSet& rowHashSet);

	unsigned short m_timeFrame;

	void SetValueToSelectedRows(const SignedPrice& value);

	void AddStockPair(const PairKey& key, const SignedPrice& value);
	StockPair* RemoveStockPair(StockPair* stockPair);
	void DestroyStockPairMap();
	StockPairMap m_stockPairMap;
	SymbolStockPairMap m_symbolStockPairMap;

	std::string m_openFileName;
	std::string m_saveFileName;
	std::string m_fileFilterString;
	UShortList m_saveColumnIdList;
	UShortListConstIterator m_saveColumnIdListBegin;
	UShortListConstIterator m_saveColumnIdListEnd;
	size_t m_saveColumnIdListSize;

	void SetMarketSorterConnected(bool connected);
	void SetMarketSorterLoaded(bool loaded);
	bool m_marketSorterConnected;
	COLORREF m_colorDisconnected;
	bool m_marketSorterLoaded;
	COLORREF m_colorNotLoaded;

	int m_menuWidth;
};

class StockComparatorRow : public RowSortable, public SecurityChartRow
{
friend class SecurityChartRow2;
public:
	StockComparatorRow(StockComparator* parent, StockPair* stockPair, Position* position1, Position* position2, bool potential = false);
	virtual ~StockComparatorRow();
	virtual const void* GetBaseObject() const override{return m_stockPair;}
//	virtual bool PaintCell(CDC& dc, const Column* column, const CRect& rect, UINT verticalJustification, int horizontalOffset) const override;//, unsigned char horizontalAlignment) const;
	virtual bool PaintCell(CDC& dc, const Column* column, CRect& rect, bool bottomBase, UINT verticalJustification, int horizontalOffset, int verticalOffset) const override;//, unsigned char horizontalAlignment) const;
	virtual bool CellToString(std::string& info, unsigned short columnId) const override;
	virtual COLORREF GetRowColor() const override;
	virtual COLORREF GetCellColor(unsigned short columnId) const override;
	virtual COLORREF GetCellBkColor(unsigned short columnId) const override;
	virtual COLORREF GetRowBkColor() const override;

	virtual RowValue* CreateCell(unsigned short id) override;

	StockPair* GetStockPair(){return m_stockPair;}
	const StockPair* GetStockPair() const{return m_stockPair;}

	virtual bool SetPosition(const Position* position) override;
	bool SetPosition1(const Position* position);
	bool SetPosition2(const Position* position);

	virtual bool SetSecurity(const Security* security) override;
	bool SetSecurity1(const Security* security);
	bool SetSecurity2(const Security* security);

	bool SetValue(const SignedPrice& value)
	{
		return m_stockPair->SetValue(value);
	}
	const SignedPrice& GetValue() const{return m_stockPair->GetValue();}

	const unsigned __int64& GetFirstSymbol() const{return m_stockPair->GetFirst();}
	const unsigned __int64& GetSecondSymbol() const{return m_stockPair->GetSecond();}

	virtual bool Lock(bool wait) const override;
//	virtual void LockWait() const override;
	virtual void Unlock() const override;

	bool Lock1(bool wait) const;
	bool Lock2(bool wait) const;
//	void LockWait1() const;
//	void LockWait2() const;
	void Unlock1() const;
	void Unlock2() const;

	virtual bool isSortValueChanged() const override;
	virtual bool isChainSortValueChanged() const override;

	virtual short Compare(const RowSortable& other) const override;
	virtual short CompareAhead(const RowSortable& other) const override;
	virtual short CompareChain(const RowSortable& other) const override;
	virtual short CompareChainAhead(const RowSortable& other) const override;
	virtual short DefaultCompare(const RowSortable& other) const override;
	virtual void Update(bool dynamicOnly) override;
	virtual bool UpdateRowColor() override;//should be called only inside a lock
	virtual void UpdateAndInvalidate(CRect& rowRect, bool dynamicOnly) override;

	virtual void Initialize() override;//should be called only inside a lock

	virtual const char* GetRowSecuritySymbol(const CPoint* point = nullptr) const override;

	DECLARE_NED_NEW

	virtual const Security* GetSelectedSecurity() const override;

	StockComparator* GetStockComparatorWindow(){return m_stockComparatorWindow;}
	const StockComparator* GetStockComparatorWindow() const{return m_stockComparatorWindow;}

protected:
	void UpdateSecurity();

	bool UpdatePrints(bool lock);
	virtual void Notify(const Message* message, const Observable* from, const Message* info = nullptr) override;

	SecurityChartRow2 m_securityChartRow;
	StockPair* m_stockPair;
	StockComparator* m_stockComparatorWindow;
	COLORREF m_securityColor;
	ScUnappliedMessageReader m_unappliedMessageReader;
	ScUnappliedMessageReader m_unappliedMessageReader2;
};

