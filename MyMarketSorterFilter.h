#pragma once

//Filters

enum MyFilterType : unsigned char
{
	MY_STOCK,
	MY_MARKET_SORTER,
	MyFilterTypeCount
};


class MySymbolFilter : public TakionFilter
{
public:
	virtual unsigned char isA() const = 0;
	virtual bool isPriceFilter() const{return false;}
	virtual bool isStockFilter() const{return false;}
	virtual bool isMarketSorterFilter() const{return true;}
	virtual bool isBaseOptionSorterFilter() const{return false;}

	const char* GetFilterTypeName() const
	{
		switch(isA())
		{
			case MY_STOCK:
			return "pr";

			case MY_MARKET_SORTER:
			return "ms";

			default:
			return nullptr;
		}
	}

	virtual void Copy(const TakionFilter& other) override;
	virtual void CopyFilterOfOtherKind(const TakionFilter& other, unsigned int doNotCopyFlags) override
	{
		CopySymbolFilterOfOtherKind((const MySymbolFilter&)other, doNotCopyFlags);
	}
	void CopyMyFields(const MySymbolFilter& other, unsigned int doNotCopyFlags);
	virtual void CopySymbolFilterOfOtherKind(const MySymbolFilter& other, unsigned int doNotCopyFlags)
	{
		CopyMyFields(other, doNotCopyFlags);
	}
	bool MyFieldsEqual(const MySymbolFilter& other, unsigned int doNotCopyFlags) const;
	virtual bool isIntersectionEqual(const TakionFilter& other) const override//, unsigned int doNotCopyFlags) const
	{
		return isSymbolIntersectionEqual((const MySymbolFilter&)other, 1 << FDNC_SYMBOL);
	}
	virtual bool isSymbolIntersectionEqual(const MySymbolFilter& other, unsigned int doNotCopyFlags) const
	{
		return MyFieldsEqual(other, doNotCopyFlags);
	}
	virtual bool operator==(const TakionFilter& other) const override
	{
		return MyFieldsEqual((const MySymbolFilter&)other, 0);
	}

	virtual void toString(std::string& str) const override;
	virtual void fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier) override;
	virtual unsigned char isPotentialFilterSame(const TakionFilter& filter) const override;//0 - same; 1 - to add; 2 - to remove; 3 - to add and remove
	virtual bool isFilterSame(const TakionFilter& filter) const override;

	virtual bool isFiltering() const override;
	virtual bool isEmpty() const override;

#if (_MSC_VER > 1600)
	typedef std::unordered_set<char> CharSet;
#else
	typedef std::hash_set<char> CharSet;
#endif
	const CharSet& GetFirstCharSet() const{return m_firstCharSet;}
	const CharSet& GetNoFirstCharSet() const{return m_noFirstCharSet;}
	const CharSet& GetCharSet() const{return m_charSet;}
	const CharSet& GetNoCharSet() const{return m_noCharSet;}

	const std::string& GetFirstCharString() const{return m_firstCharString;}
	const std::string& GetNoFirstCharString() const{return m_noFirstCharString;}
	const std::string& GetCharString() const{return m_charString;}
	const std::string& GetNoCharString() const{return m_noCharString;}

	void SetFirstCharString(const std::string& str);
	void SetNoFirstCharString(const std::string& str);
	void SetCharString(const std::string& str);
	void SetNoCharString(const std::string& str);

	const bool& isUseFirstCharSet() const{return m_useFirstCharSet;}
	const bool& isUseNoFirstCharSet() const{return m_useNoFirstCharSet;}
	const bool& isUseCharSet() const{return m_useCharSet;}
	const bool& isUseNoCharSet() const{return m_useNoCharSet;}
	void SetUseFirstCharSet(const bool& use){m_useFirstCharSet = use;}
	void SetUseNoFirstCharSet(const bool& use){m_useNoFirstCharSet = use;}
	void SetUseCharSet(const bool& use){m_useCharSet = use;}
	void SetUseNoCharSet(const bool& use){m_useNoCharSet = use;}
#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
	typedef CMap<unsigned __int64, const unsigned __int64&, unsigned char, const unsigned char&> UInt64Set;
	void ClearExcludeIncludeSymbolSet()
	{
		m_excludeSymbolSet.RemoveAll();
		m_includeSymbolSet.RemoveAll();
		m_excludeIncludeSymbolMap.clear();
	}
	void ClearExcludeSymbolSet()
	{
		for(const UInt64Set::CPair* p = m_excludeSymbolSet.PGetFirstAssoc(); p; p = m_excludeSymbolSet.PGetNextAssoc(p))
		{
			m_excludeIncludeSymbolMap.erase(p->key);
		}
		m_excludeSymbolSet.RemoveAll();
	}
	void ClearIncludeSymbolSet()
	{
		for(const UInt64Set::CPair* p = m_includeSymbolSet.PGetFirstAssoc(); p; p = m_includeSymbolSet.PGetNextAssoc(p))
		{
			m_excludeIncludeSymbolMap.erase(p->key);
		}
		m_includeSymbolSet.RemoveAll();
	}
	void EraseExcludeSymbol(const unsigned __int64& symbol)
	{
		m_excludeSymbolSet.RemoveKey(symbol);
		m_excludeIncludeSymbolMap.erase(symbol);
	}
	void EraseIncludeSymbol(const unsigned __int64& symbol)
	{
		m_includeSymbolSet.RemoveKey(symbol);
		m_excludeIncludeSymbolMap.erase(symbol);
	}
	bool UnExcludeSymbol(const unsigned __int64& symbol)
	{
		if(m_useExclude)
		{
			const UInt64Set::CPair* found = m_excludeSymbolSet.PLookup(symbol);
			if(found)
			{
				m_excludeSymbolSet.RemoveKey(symbol);
				m_excludeIncludeSymbolMap.erase(symbol);
				return true;
			}
		}
		else
		{
			m_excludeSymbolSet.RemoveKey(symbol);
			m_excludeIncludeSymbolMap.erase(symbol);
		}
		return false;
	}
	bool UnIncludeSymbol(const unsigned __int64& symbol)//{return m_includeSymbolSet.erase(symbol) != 0 && m_useInclude;}
	{
		if(m_useInclude)
		{
			const UInt64Set::CPair* found = m_includeSymbolSet.PLookup(symbol);
			if(found)
			{
				m_includeSymbolSet.RemoveKey(symbol);
				m_excludeIncludeSymbolMap.erase(symbol);
				return true;
			}
		}
		else
		{
			m_includeSymbolSet.RemoveKey(symbol);
			m_excludeIncludeSymbolMap.erase(symbol);
		}
		return false;
	}
	bool isSymbolInExcludeSet(const unsigned __int64& symbol) const{return m_excludeSymbolSet.PLookup(symbol) != nullptr;}
	bool isSymbolInIncludeSet(const unsigned __int64& symbol) const{return m_includeSymbolSet.PLookup(symbol) != nullptr;}
	bool isExcludeSetEmpty() const{return m_excludeSymbolSet.IsEmpty() == TRUE;}
	bool isIncludeSetEmpty() const{return m_includeSymbolSet.IsEmpty() == TRUE;}
#else
#ifdef _DEBUG
	typedef std::set<unsigned __int64> UInt64Set;
#else
#if (_MSC_VER > 1600)
	typedef std::unordered_set<unsigned __int64> UInt64Set;
#else
	typedef std::hash_set<unsigned __int64> UInt64Set;
#endif
#endif
	void ClearExcludeIncludeSymbolSet()
	{
		m_excludeSymbolSet.clear();
		m_includeSymbolSet.clear();
		m_excludeIncludeSymbolMap.clear();
	}
	void ClearExcludeSymbolSet()
	{
		for(UInt64Set::const_iterator it = m_excludeSymbolSet.cbegin(); it != m_excludeSymbolSetEnd; ++it)
		{
			m_excludeIncludeSymbolMap.erase(*it);
		}
		m_excludeSymbolSet.clear();
	}
	void ClearIncludeSymbolSet()
	{
		for(UInt64Set::const_iterator it = m_includeSymbolSet.cbegin(); it != m_includeSymbolSetEnd; ++it)
		{
			m_excludeIncludeSymbolMap.erase(*it);
		}
		m_includeSymbolSet.clear();
	}
	void EraseExcludeSymbol(const unsigned __int64& symbol)
	{
		m_excludeSymbolSet.erase(symbol);
		m_excludeIncludeSymbolMap.erase(symbol);
	}
	void EraseIncludeSymbol(const unsigned __int64& symbol)
	{
		m_includeSymbolSet.erase(symbol);
		m_excludeIncludeSymbolMap.erase(symbol);
	}
	bool UnExcludeSymbol(const unsigned __int64& symbol)
	{
		if(m_excludeSymbolSet.erase(symbol) != 0)
		{
			m_excludeIncludeSymbolMap.erase(symbol);
			return m_useExclude;
		}
		return false;
	}
	bool UnIncludeSymbol(const unsigned __int64& symbol)
	{
		if(m_includeSymbolSet.erase(symbol) != 0)
		{
			m_excludeIncludeSymbolMap.erase(symbol);
			return m_useInclude;
		}
		return false;
	}
	bool isSymbolInExcludeSet(const unsigned __int64& symbol) const{return m_excludeSymbolSet.find(symbol) != m_excludeSymbolSetEnd;}
	bool isSymbolInIncludeSet(const unsigned __int64& symbol) const{return m_includeSymbolSet.find(symbol) != m_includeSymbolSetEnd;}
	bool isExcludeSetEmpty() const{return m_excludeSymbolSet.empty();}
	bool isIncludeSetEmpty() const{return m_includeSymbolSet.empty();}
#endif
	bool isSymbolExcluded(const unsigned __int64& symbol) const{return m_useExclude && isSymbolInExcludeSet(symbol);}
	bool isSymbolIncluded(const unsigned __int64& symbol) const{return m_useInclude && isSymbolInIncludeSet(symbol);}
	const UInt64Set& GetExcludeSymbolSet() const{return m_excludeSymbolSet;}
	const UInt64Set& GetIncludeSymbolSet() const{return m_includeSymbolSet;}
	UInt64Set& GetExcludeSymbolSet(){return m_excludeSymbolSet;}
	UInt64Set& GetIncludeSymbolSet(){return m_includeSymbolSet;}
	unsigned char ExcludeSymbol(unsigned __int64 symbol);
	unsigned char IncludeSymbol(unsigned __int64 symbol);

	typedef std::map<unsigned __int64, bool, lessUIntAsStr<unsigned __int64> > ExcludeIncludeSymbolMap;
	const ExcludeIncludeSymbolMap& GetExcludeIncludeSymbolMap() const{return m_excludeIncludeSymbolMap;}//Union of m_excludeSymbolSet & m_includeSymbolSet
	ExcludeIncludeSymbolMap& GetExcludeIncludeSymbolMap(){return m_excludeIncludeSymbolMap;}//Union of m_excludeSymbolSet & m_includeSymbolSet

	const bool& isUseExclude() const{return m_useExclude;}
	const bool& isUseInclude() const{return m_useInclude;}
	void SetUseExclude(const bool& use){m_useExclude = use;}
	void SetUseInclude(const bool& use){m_useInclude = use;}

	MySymbolFilter& operator=(const MySymbolFilter& other);
	virtual void Reset();

protected:
	MySymbolFilter();
	MySymbolFilter(const MySymbolFilter& other);
	bool m_useExclude;
	bool m_useInclude;
	UInt64Set m_excludeSymbolSet;
	UInt64Set m_includeSymbolSet;

#ifndef USE_CMAP_FOR_FILTER_SYMBOLS
	UInt64Set::const_iterator m_excludeSymbolSetEnd;
	UInt64Set::const_iterator m_includeSymbolSetEnd;
#endif

	ExcludeIncludeSymbolMap m_excludeIncludeSymbolMap;//Union of m_excludeSymbolSet & m_includeSymbolSet
	ExcludeIncludeSymbolMap::const_iterator m_excludeIncludeSymbolMapEnd;

	bool m_useFirstCharSet;
	bool m_useNoFirstCharSet;
	bool m_useCharSet;
	bool m_useNoCharSet;
	std::string m_firstCharString;
	std::string m_noFirstCharString;
	std::string m_charString;
	std::string m_noCharString;
	CharSet m_firstCharSet;
	CharSet m_noFirstCharSet;
	CharSet m_charSet;
	CharSet m_noCharSet;
	CharSet::const_iterator m_firstCharSetEnd;
	CharSet::const_iterator m_noFirstCharSetEnd;
	CharSet::const_iterator m_charSetEnd;
	CharSet::const_iterator m_noCharSetEnd;
};

class MyPriceFilter : public MySymbolFilter
{
public:
	virtual void Copy(const TakionFilter& other) override;

	virtual bool isPriceFilter() const override{return true;}
	void CopyMyFields(const MyPriceFilter& other, unsigned int doNotCopyFlags);
	virtual void CopySymbolFilterOfOtherKind(const MySymbolFilter& other, unsigned int doNotCopyFlags) override
	{
		if(other.isPriceFilter())CopyMyFields((const MyPriceFilter&)other, doNotCopyFlags);
		MySymbolFilter::CopySymbolFilterOfOtherKind(other, doNotCopyFlags);
	}
	bool MyFieldsEqual(const MyPriceFilter& other, unsigned int doNotCopyFlags) const;
	virtual bool isSymbolIntersectionEqual(const MySymbolFilter& other, unsigned int doNotCopyFlags) const override
	{
		return MySymbolFilter::isSymbolIntersectionEqual(other, doNotCopyFlags) && (!other.isPriceFilter() || MyFieldsEqual((const MyPriceFilter&)other, doNotCopyFlags));
	}
	virtual bool operator==(const TakionFilter& other) const override
	{
		return MySymbolFilter::operator==(other) && MyFieldsEqual((const MyPriceFilter&)other, 0);
	}

	virtual bool isFiltering() const override;
	virtual bool isEmpty() const override;

	TakionRangePositive<Price>& GetPriceRange(){return m_priceRange;}
	TakionRangePositive<Price>& GetOpenPriceRange(){return m_openPriceRange;}
	TakionRangePositiveNegative<Price>& GetDayNetChangeRange(){return m_dayNetChangeRange;}
	TakionRangePositive<Price>& GetDayPriceRangeRange(){return m_dayPriceRangeRange;}
	TakionRangePositiveNegative<Price>& GetDayNetChangePercentRange(){return m_dayNetChangePercentRange;}
	TakionRangePositiveNegative<Price>& GetDayNetOpenChangePercentRange(){return m_dayNetOpenChangePercentRange;}
	TakionRangePositive<Price>& GetDayPriceRangePercentRange(){return m_dayPriceRangePercentRange;}

	const TakionRangePositive<Price>& GetPriceRange() const{return m_priceRange;}
	const TakionRangePositive<Price>& GetOpenPriceRange() const{return m_openPriceRange;}
	const TakionRangePositiveNegative<Price>& GetDayNetChangeRange() const{return m_dayNetChangeRange;}
	const TakionRangePositive<Price>& GetDayPriceRangeRange() const{return m_dayPriceRangeRange;}
	const TakionRangePositiveNegative<Price>& GetDayNetChangePercentRange() const{return m_dayNetChangePercentRange;}
	const TakionRangePositiveNegative<Price>& GetDayNetOpenChangePercentRange() const{return m_dayNetOpenChangePercentRange;}
	const TakionRangePositive<Price>& GetDayPriceRangePercentRange() const{return m_dayPriceRangePercentRange;}

	virtual void toString(std::string& str) const override;
	virtual void fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier) override;

	virtual unsigned char isPotentialFilterSame(const TakionFilter& filter) const override;//0 - same; 1 - to add; 2 - to remove; 3 - to add and remove
	virtual bool isFilterSame(const TakionFilter& filter) const override;
	virtual void Reset() override;

	bool SecBelongsByValues(const Security* security) const;
protected:
	MyPriceFilter();
	TakionRangePositive<Price> m_priceRange;
	TakionRangePositive<Price> m_openPriceRange;

	TakionRangePositiveNegative<Price> m_dayNetChangeRange;
	TakionRangePositive<Price> m_dayPriceRangeRange;

	TakionRangePositiveNegative<Price> m_dayNetChangePercentRange;
	TakionRangePositiveNegative<Price> m_dayNetOpenChangePercentRange;
	TakionRangePositive<Price> m_dayPriceRangePercentRange;
};

class MyStockFilter : public MyPriceFilter
{
public:
	MyStockFilter();
	virtual void Copy(const TakionFilter& other);

	virtual TakionFilter* CreateFilterCopy() const override;
	virtual unsigned char isA() const override{return MY_STOCK;}
	virtual bool isStockFilter() const override{return true;}
	void CopyMyFields(const MyStockFilter& other, unsigned int doNotCopyFlags);
	virtual void CopySymbolFilterOfOtherKind(const MySymbolFilter& other, unsigned int doNotCopyFlags) override
	{
		if(other.isStockFilter())CopyMyFields((const MyStockFilter&)other, doNotCopyFlags);
		MyPriceFilter::CopySymbolFilterOfOtherKind(other, doNotCopyFlags);
	}
	bool MyFieldsEqual(const MyStockFilter& other, unsigned int doNotCopyFlags) const;
	virtual bool isSymbolIntersectionEqual(const MySymbolFilter& other, unsigned int doNotCopyFlags) const override
	{
		return MyPriceFilter::isSymbolIntersectionEqual(other, doNotCopyFlags) && (!other.isStockFilter() || MyFieldsEqual((const MyStockFilter&)other, doNotCopyFlags));
	}
	virtual bool operator==(const TakionFilter& other) const override
	{
		return MyPriceFilter::operator==(other) && MyFieldsEqual((const MyStockFilter&)other, 0);
	}

	virtual bool isFiltering() const override;
	virtual bool isEmpty() const override;

	TakionRangePositive<unsigned __int64>& GetYesterdaysVolumeRange(){return m_yesterdaysVolumeRange;}
	TakionRangePositive<unsigned __int64>& GetTodaysVolumeRange(){return m_todaysVolumeRange;}
	TakionRangePositive<unsigned __int64>& GetTodaysPxMhVolumeRange(){return m_todaysPxMhVolumeRange;}

	TakionRangePositive<unsigned __int64>& GetAvgDailyVolumeRange(){return m_avgDailyVolumeRange;}
	TakionRangePositive<Money>& GetRelativeVolumeRange(){return m_relativeVolumeRange;}
	TakionRangePositive<Money>& GetPreMhVolRelRange(){return m_preMhVolRelRange;}
	TakionRangePositive<Price>& GetYesterdaysPriceRangeRange(){return m_yesterdaysPriceRangeRange;}
	TakionRangePositive<Price>& GetYesterdaysPriceRangePercentRange(){return m_yesterdaysPriceRangePercentRange;}

	TakionRangePositiveNegative<unsigned int>& GetImbalanceNyseRange(){return m_imbalanceNyseRange;}
	TakionRangePositiveNegative<unsigned int>& GetImbalanceNyseRegRange(){return m_imbalanceNyseRegRange;}
	TakionRangePositiveNegative<unsigned int>& GetImbalanceNsdqRange(){return m_imbalanceNsdqRange;}
	TakionRangePositiveNegative<unsigned int>& GetImbalanceArcaRange(){return m_imbalanceArcaRange;}
	TakionRangePositiveNegative<unsigned int>& GetImbalanceExchRange(){return m_imbalanceExchRange;}
	TakionRangePositiveNegative<Price>& GetRateImbalanceRange(){return m_rateImbalanceRange;}
	TakionRangePositiveNegative<Price>& GetImbNyDeltaInfRange(){return m_imbNyDeltaInfRange;}
	TakionRangePositiveNegative<Price>& GetImbExFarPriceDeltaRange(){return m_imbExFarPriceDeltaRange;}
	TakionRangePositiveNegative<Price>& GetImbNqFarPriceDeltaRange(){return m_imbNqFarPriceDeltaRange;}
	TakionRangePositiveNegative<Price>& GetImbNqThruRange(){return m_imbNqThruRange;}
	TakionRangePositiveNegative<Money>& GetImbExValueRange(){return m_imbExValueRange;}

	TakionRangePositive<Price>& GetImbExNearRange(){return m_imbExNearRange;}
	TakionRangePositive<Price>& GetImbExFarRange(){return m_imbExFarRange;}

	TakionRangePositive<unsigned int>& GetImbExMatchRange(){return m_imbExMatchRange;}
	TakionRangePositive<Money>& GetImbExVolPercentRange(){return m_imbExVolPercentRange;}
	TakionRangePositive<Money>& GetImbExYestVolPercentRange(){return m_imbExYestVolPercentRange;}

	TakionRangePositive<Price>& GetDeltaPvRange(){return m_deltaPvRange;}
//	TakionRangePositive<Price>& GetRsiMhRange(){return m_rsiMhRange;}
//	TakionRangePositive<Price>& GetRsiDaysRange(){return m_rsiDaysRange;}
	TakionRangePositive<Price>& GetBorrowPriceRange(){return m_borrowPriceRange;}
	TakionRangePositive<Price>& GetPmiSpreadRange(){return m_pmiSpreadRange;}
	TakionRangePositive<Price>& GetTodaysClosePriceRange(){return m_todaysClosePriceRange;}

	TakionRangePositiveNegative<Price>& GetBidNetRange(){return m_bidNetRange;}
	TakionRangePositiveNegative<Price>& GetAskNetRange(){return m_askNetRange;}

//Bid/Ask as % of Today's Close Price
	TakionRangePositiveNegative<Price>& GetTBidNetRange(){return m_tBidNetRange;}
	TakionRangePositiveNegative<Price>& GetTAskNetRange(){return m_tAskNetRange;}

	TakionRangePositiveNegative<Price>& GetSpreadRange(){return m_spreadRange;}

	TakionRangePositiveNegative<Price>& GetBidNetPercentRange(){return m_bidNetPercentRange;}
	TakionRangePositiveNegative<Price>& GetAskNetPercentRange(){return m_askNetPercentRange;}

//Bid/Ask as % of Today's Close Price
	TakionRangePositiveNegative<Price>& GetTBidNetPercentRange(){return m_tBidNetPercentRange;}
	TakionRangePositiveNegative<Price>& GetTAskNetPercentRange(){return m_tAskNetPercentRange;}

	TakionRangePositiveNegative<SignedPrice>& GetLuldRange(){return m_luldRange;}
	TakionRangePositiveNegative<Money>& GetLuldPercentRange(){return m_luldPercentRange;}
/*
//RSI Minutes
	const bool& isRsiMhCalculate() const{return m_rsiMhCalculate;}
	const unsigned short& GetRsiMhPriorPointCount() const{return m_rsiMhPriorPointCount;}
	const unsigned short& GetRsiMhPeriodLength() const{return m_rsiMhPeriodLength;}
	const unsigned short& GetRsiMhPeriodCount() const{return m_rsiMhPeriodCount;}
	const unsigned short& GetRsiMhStartMinute() const{return m_rsiMhStartMinute;}
	const bool& isRsiMhTodayOnly() const{return m_rsiMhTodayOnly;}
	const bool& isRsiMhPostMarket() const{return m_rsiMhPostMarket;}

	void SetRsiMhCalculate(const bool& calculate){m_rsiMhCalculate = calculate;}
	void SetRsiMhPriorPointCount(const unsigned short& count){m_rsiMhPriorPointCount = count;}
	void SetRsiMhPeriodLength(const unsigned short& length){m_rsiMhPeriodLength = length;}
	void SetRsiMhPeriodCount(const unsigned short& count){m_rsiMhPeriodCount = count;}
	void SetRsiMhStartMinute(const unsigned short& minute){m_rsiMhStartMinute = minute;}
	void SetRsiMhTodayOnly(const bool& todayOnly){m_rsiMhTodayOnly = todayOnly;}
	void SetRsiMhPostMarket(const bool& postMarket){m_rsiMhPostMarket = postMarket;}
//RSI Days
	const bool& isRsiDaysCalculate() const{return m_rsiDaysCalculate;}
	const unsigned short& GetRsiDaysPriorPointCount() const{return m_rsiDaysPriorPointCount;}
	const unsigned short& GetRsiDaysPeriodLength() const{return m_rsiDaysPeriodLength;}
	const unsigned short& GetRsiDaysPeriodCount() const{return m_rsiDaysPeriodCount;}
	const unsigned short& GetRsiDaysStartMinute() const{return m_rsiDaysStartMinute;}
	const bool& isRsiDaysPostMarket() const{return m_rsiDaysPostMarket;}

	void SetRsiDaysCalculate(const bool& calculate){m_rsiDaysCalculate = calculate;}
	void SetRsiDaysPriorPointCount(const unsigned short& count){m_rsiDaysPriorPointCount = count;}
	void SetRsiDaysPeriodLength(const unsigned short& length){m_rsiDaysPeriodLength = length;}
	void SetRsiDaysPeriodCount(const unsigned short& count){m_rsiDaysPeriodCount = count;}
	void SetRsiDaysStartMinute(const unsigned short& minute){m_rsiDaysStartMinute = minute;}
	void SetRsiDaysPostMarket(const bool& postMarket){m_rsiDaysPostMarket = postMarket;}
*/
//
	const TakionRangePositive<unsigned __int64>& GetYesterdaysVolumeRange() const{return m_yesterdaysVolumeRange;}
	const TakionRangePositive<unsigned __int64>& GetTodaysVolumeRange() const{return m_todaysVolumeRange;}
	const TakionRangePositive<unsigned __int64>& GetTodaysPxMhVolumeRange() const{return m_todaysPxMhVolumeRange;}

	const TakionRangePositive<unsigned __int64>& GetAvgDailyVolumeRange() const{return m_avgDailyVolumeRange;}
	const TakionRangePositive<Money>& GetRelativeVolumeRange() const{return m_relativeVolumeRange;}
	const TakionRangePositive<Money>& GetPreMhVolRelRange() const{return m_preMhVolRelRange;}
	const TakionRangePositive<Price>& GetYesterdaysPriceRangeRange() const{return m_yesterdaysPriceRangeRange;}
	const TakionRangePositive<Price>& GetYesterdaysPriceRangePercentRange() const{return m_yesterdaysPriceRangePercentRange;}

	const TakionRangePositiveNegative<unsigned int>& GetImbalanceNyseRange() const{return m_imbalanceNyseRange;}
	const TakionRangePositiveNegative<unsigned int>& GetImbalanceNyseRegRange() const{return m_imbalanceNyseRegRange;}
	const TakionRangePositiveNegative<unsigned int>& GetImbalanceNsdqRange() const{return m_imbalanceNsdqRange;}
	const TakionRangePositiveNegative<unsigned int>& GetImbalanceArcaRange() const{return m_imbalanceArcaRange;}
	const TakionRangePositiveNegative<unsigned int>& GetImbalanceExchRange() const{return m_imbalanceExchRange;}
	const TakionRangePositiveNegative<Price>& GetRateImbalanceRange() const{return m_rateImbalanceRange;}
	const TakionRangePositiveNegative<Price> & GetImbNyDeltaInfRange() const{return m_imbNyDeltaInfRange;}
	const TakionRangePositiveNegative<Price>& GetImbExFarPriceDeltaRange() const{return m_imbExFarPriceDeltaRange;}
	const TakionRangePositiveNegative<Price>& GetImbNqFarPriceDeltaRange() const{return m_imbNqFarPriceDeltaRange;}
	const TakionRangePositiveNegative<Price>& GetImbNqThruRange() const{return m_imbNqThruRange;}
	const TakionRangePositiveNegative<Money>& GetImbExValueRange() const{return m_imbExValueRange;}
	const TakionRangePositive<Price>& GetImbExNearRange() const{return m_imbExNearRange;}
	const TakionRangePositive<Price>& GetImbExFarRange() const{return m_imbExFarRange;}
	const TakionRangePositive<unsigned int>& GetImbExMatchRange() const{return m_imbExMatchRange;}
	const TakionRangePositive<Money>& GetImbExVolPercentRange() const{return m_imbExVolPercentRange;}
	const TakionRangePositive<Money>& GetImbExYestVolPercentRange() const{return m_imbExYestVolPercentRange;}
	const TakionRangePositive<Price>& GetDeltaPvRange() const{return m_deltaPvRange;}
//	const TakionRangePositive<Price>& GetRsiMhRange() const{return m_rsiMhRange;}
//	const TakionRangePositive<Price>& GetRsiDaysRange() const{return m_rsiDaysRange;}
	const TakionRangePositive<Price>& GetBorrowPriceRange() const{return m_borrowPriceRange;}
	const TakionRangePositive<Price>& GetPmiSpreadRange() const{return m_pmiSpreadRange;}
	const TakionRangePositive<Price>& GetTodaysClosePriceRange() const{return m_todaysClosePriceRange;}

	const TakionRangePositiveNegative<Price>& GetBidNetRange() const{return m_bidNetRange;}
	const TakionRangePositiveNegative<Price>& GetAskNetRange() const{return m_askNetRange;}

//Bid/Ask as % of Today's Close Price
	const TakionRangePositiveNegative<Price>& GetTBidNetRange() const{return m_tBidNetRange;}
	const TakionRangePositiveNegative<Price>& GetTAskNetRange() const{return m_tAskNetRange;}

	const TakionRangePositiveNegative<Price>& GetSpreadRange() const{return m_spreadRange;}

	const TakionRangePositiveNegative<Price>& GetBidNetPercentRange() const{return m_bidNetPercentRange;}
	const TakionRangePositiveNegative<Price>& GetAskNetPercentRange() const{return m_askNetPercentRange;}

//Bid/Ask as % of Today's Close Price
	const TakionRangePositiveNegative<Price>& GetTBidNetPercentRange() const{return m_tBidNetPercentRange;}
	const TakionRangePositiveNegative<Price>& GetTAskNetPercentRange() const{return m_tAskNetPercentRange;}

	const TakionRangePositiveNegative<SignedPrice>& GetLuldRange() const{return m_luldRange;}
	const TakionRangePositiveNegative<Money>& GetLuldPercentRange() const{return m_luldPercentRange;}

	const unsigned int& GetMarketCategoryFilter() const{return m_marketCategoryFilter;}
	void SetMarketCategoryFilter(const unsigned int& filter){m_marketCategoryFilter = filter;}

	const unsigned int& GetTestStockFilter() const{return m_testStockFilter;}
	void SetTestStockFilter(const unsigned int& filter){m_testStockFilter = filter;}

	const unsigned int& GetSecurityTypeFilter() const{return m_securityTypeFilter;}
	void SetSecurityTypeFilter(const unsigned int& filter){m_securityTypeFilter = filter;}

	const unsigned int& GetHtbFilter() const{return m_htbFilter;}
	void SetHtbFilter(const unsigned int& filter){m_htbFilter = filter;}

	const unsigned int& GetTradingStateFilter() const{return m_tradingStateFilter;}
	void SetTradingStateFilter(const unsigned int& filter){m_tradingStateFilter = filter;}

	const unsigned int& GetCircuitBreakerFilter() const{return m_circuitBreakerFilter;}
	void SetCircuitBreakerFilter(const unsigned int& filter){m_circuitBreakerFilter = filter;}

	const unsigned int& GetNasdaqQuoteConditionFilter() const{return m_nasdaqQuoteConditionFilter;}
	void SetNasdaqQuoteConditionFilter(const unsigned int& filter){m_nasdaqQuoteConditionFilter = filter;}
	const unsigned int& GetListedQuoteConditionFilter() const{return m_listedQuoteConditionFilter;}
	void SetListedQuoteConditionFilter(const unsigned int& filter){m_listedQuoteConditionFilter = filter;}

	virtual void toString(std::string& str) const override;
	virtual void fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier) override;

	virtual bool SecurityBelongs(const SecurityChartRow* securityChartRow) const;
	bool SecurityBelongsByCalcValues(const SecurityChartRow* securityChartRow) const;
//	bool SecurityBelongsByRsiMh(const SecurityChartRow* securityChartRow) const;
//	bool SecurityBelongsByRsiDays(const SecurityChartRow* securityChartRow) const;

	bool SecBelongs(const Security* security) const;
	bool SecBelongsBySymbolInclude(const Security* security) const{return isSymbolIncluded(security->GetNumericSymbol());}//m_useInclude && isSymbolInIncludeSet(security->GetNumericSymbol());}
	bool SecBelongsBySymbolExclude(const Security* security) const{return !isSymbolExcluded(security->GetNumericSymbol());}//!m_useExclude || !isSymbolInExcludeSet(security->GetNumericSymbol());}
	bool SecBelongsByValues(const Security* security) const;
	bool SecBelongsByCalcValues(const Security* security) const;

	virtual bool SecurityPotentiallyBelongs(const Security* security) const;
	virtual unsigned char isPotentialFilterSame(const TakionFilter& filter) const override;//0 - same; 1 - to add; 2 - to remove; 3 - to add and remove
	virtual bool isFilterSame(const TakionFilter& filter) const override;
	virtual void Reset() override;
protected:
	unsigned int m_marketCategoryFilter;
	unsigned int m_testStockFilter;//1 - real; 2 - test; 3 - both
	unsigned int m_securityTypeFilter;
	unsigned int m_htbFilter;//1 - ETB; 2 - HTB available; 4 - htb available
	unsigned int m_tradingStateFilter;
	unsigned int m_circuitBreakerFilter;
	unsigned int m_nasdaqQuoteConditionFilter;
	unsigned int m_listedQuoteConditionFilter;
	TakionRangePositive<unsigned __int64> m_yesterdaysVolumeRange;
	TakionRangePositive<unsigned __int64> m_todaysVolumeRange;
	TakionRangePositive<unsigned __int64> m_todaysPxMhVolumeRange;
//	TakionRangePositive<unsigned int> m_volumeRateRange;
//	TakionRangePositive<Price> m_percentTVolByYVolRange;
//	TakionRangePositiveNegative<unsigned int, int> m_positionRange;

	TakionRangePositive<unsigned __int64> m_avgDailyVolumeRange;
	TakionRangePositive<Money> m_relativeVolumeRange;
	TakionRangePositive<Money> m_preMhVolRelRange;
	TakionRangePositive<Price> m_yesterdaysPriceRangeRange;
	TakionRangePositive<Price> m_yesterdaysPriceRangePercentRange;

	TakionRangePositiveNegative<unsigned int> m_imbalanceNyseRange;
	TakionRangePositiveNegative<unsigned int> m_imbalanceNyseRegRange;
	TakionRangePositiveNegative<unsigned int> m_imbalanceNsdqRange;
	TakionRangePositiveNegative<unsigned int> m_imbalanceArcaRange;
	TakionRangePositiveNegative<unsigned int> m_imbalanceExchRange;
	TakionRangePositiveNegative<Price> m_rateImbalanceRange;
	TakionRangePositiveNegative<Price> m_imbNyDeltaInfRange;
	TakionRangePositiveNegative<Price> m_imbExFarPriceDeltaRange;
	TakionRangePositiveNegative<Price> m_imbNqFarPriceDeltaRange;
	TakionRangePositiveNegative<Price> m_imbNqThruRange;
	TakionRangePositiveNegative<Money> m_imbExValueRange;
	TakionRangePositive<Price> m_imbExNearRange;
	TakionRangePositive<Price> m_imbExFarRange;
	TakionRangePositive<unsigned int> m_imbExMatchRange;
	TakionRangePositive<Money> m_imbExVolPercentRange;
	TakionRangePositive<Money> m_imbExYestVolPercentRange;
	TakionRangePositive<Price> m_deltaPvRange;
//	TakionRangePositive<Price> m_rsiMhRange;
//	TakionRangePositive<Price> m_rsiDaysRange;
	TakionRangePositive<Price> m_borrowPriceRange;
	TakionRangePositive<Price> m_pmiSpreadRange;
	TakionRangePositive<Price> m_todaysClosePriceRange;

	TakionRangePositiveNegative<Price> m_bidNetRange;
	TakionRangePositiveNegative<Price> m_askNetRange;

//Bid/Ask as % of Today's Close Price
	TakionRangePositiveNegative<Price> m_tBidNetRange;
	TakionRangePositiveNegative<Price> m_tAskNetRange;

	TakionRangePositiveNegative<Price> m_spreadRange;

	TakionRangePositiveNegative<Price> m_bidNetPercentRange;
	TakionRangePositiveNegative<Price> m_askNetPercentRange;

//Bid/Ask as % of Today's Close Price
	TakionRangePositiveNegative<Price> m_tBidNetPercentRange;
	TakionRangePositiveNegative<Price> m_tAskNetPercentRange;

	TakionRangePositiveNegative<SignedPrice> m_luldRange;
	TakionRangePositiveNegative<Money> m_luldPercentRange;
/*
//RSI Minutes
	unsigned short m_rsiMhPriorPointCount;
	unsigned short m_rsiMhPeriodLength;
	unsigned short m_rsiMhPeriodCount;
	unsigned short m_rsiMhStartMinute;
	bool m_rsiMhCalculate;
	bool m_rsiMhTodayOnly;
	bool m_rsiMhPostMarket;
//RSI Days
	unsigned short m_rsiDaysPriorPointCount;
	unsigned short m_rsiDaysPeriodLength;
	unsigned short m_rsiDaysPeriodCount;
	unsigned short m_rsiDaysStartMinute;
	bool m_rsiDaysCalculate;
	bool m_rsiDaysPostMarket;
*/
};

//Dialogs

class MyDialogSymbolFilter : public DialogFilter
{
public:
	void Display(const TakionFilter& filter, TakionFrame* receipient, unsigned int code, CWnd* parent, int x, int y, int w, int h, bool show = true);
	virtual bool isFilterChanged() const override;
	virtual bool isFilterValid() const override;
	virtual bool isFilterEmpty() const override;
	virtual void SetFilterEmpty() override;
//	unsigned short GetTimeFrameMinutes() const;

	static const int topMost = 0;//3;
	static const int hgap = 4;
	static const int vgap = 0;//1;
	static const int vgroupgap = 6;
	static const int vtitlegroupgap = 10;
	static const int checkBoxHeight = 10;
	static const int staticHeight = 8;
	static const int spinHeight = 12;
	static const int groupHeight = checkBoxHeight + 2 * spinHeight + 4 * vgap + vgroupgap + 3;
	static const int groupWidth = 85;
	static const int controlWidth = groupWidth - hgap - hgap;
	static const int firstColumnOffset = 5;
	static const int firstColumnWidth = 65;
	static const int firstColumnControlWidth = firstColumnWidth - hgap - hgap;

	void SetShowToolTips(bool showToolTips);
	bool UpdateGlobalToolTipsEnabled();

	virtual void UpdateFilterOfOtherKind(TakionFilter& other) const override
	{
		UpdateSymbolFilterOfOtherKind((MySymbolFilter&)other);
	}
	void UpdateMyFields(MySymbolFilter& other) const;// , unsigned int doNotCopyFlags);
	virtual void UpdateSymbolFilterOfOtherKind(MySymbolFilter& other) const
	{
		UpdateMyFields(other);
	}
	virtual bool isFilterOfOtherKindIntersectionDifferent(const TakionFilter& other) const override
	{
		return isSymbolFilterOfOtherKindIntersectionDifferent((const MySymbolFilter&)other, 0);
	}
	bool MyFieldsIntersectionDifferent(const MySymbolFilter& other, unsigned int doNotCopyFlags) const;
	virtual bool isSymbolFilterOfOtherKindIntersectionDifferent(const MySymbolFilter& other, unsigned int doNotCopyFlags) const
	{
		return MyFieldsIntersectionDifferent(other, doNotCopyFlags);
	}

	virtual void CopyFilterOfOtherKindToControls(const TakionFilter& filter) override
	{
		CopySymbolFilterOfOtherKindToControls((const MySymbolFilter&)filter, 0);
	}
	void MyFieldsToControls(const MySymbolFilter& other, unsigned int doNotCopyFlags);
	virtual void CopySymbolFilterOfOtherKindToControls(const MySymbolFilter& filter, unsigned int doNotCopyFlags)
	{
		MyFieldsToControls(filter, doNotCopyFlags);
	}
	void PopulateControls(const MySymbolFilter& filter, unsigned int doNotCopyFlags)
	{
		CopySymbolFilterOfOtherKindToControls(filter, doNotCopyFlags);
		EnableOkButton();
	}

	const COLORREF& GetEnabledColor() const{return m_enabledColor;}
	const COLORREF& GetEnabledTextColor() const{return m_enabledTextColor;}
	const COLORREF& GetDisabledColor() const{return m_disabledColor;}
	const COLORREF& GetInvalidColor() const{return m_invalidColor;}
	const COLORREF& GetCheckBoxColor() const{return m_checkBoxColor;}
	const COLORREF& GetPressedCheckColor() const{return m_pressedCheckColor;}

protected:
	MyDialogSymbolFilter(TakionMainWnd* mainWnd, int xSymbolOffset, int xSymbolHeight, bool toDestroyOnFocus = false, TakionFrame* receipient = nullptr, CWnd* parent = nullptr, LPCSTR caption = nullptr, DWORD style = 0, DWORD exstendedStyle = 0, int x = 0, int y = 0, int w = 0, int h = 0,
		LPCSTR font = nullptr, WORD fontSize = 8, bool executeCommands = true, bool closeOnCommandExecution = true);
	void DoAddSymbol(const char* symbol, bool checked);
	void DoRemoveSymbol(const char* symbol);
	int AddRangeComponent(int groupLeft, int groupTop, int groupWidth, const char* useCheckBoxName, const RangeGroupBase& groupBase, const char* positiveCheckBoxName = nullptr, const char* negativeCheckBoxName = nullptr, int additionalHeight = 0, int yOffset = 0);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void BeforeShow();
	virtual void AfterClose() override;
	virtual void UpdateFilterValues();

//	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

//	virtual bool EnableToolTips(bool enable) override;
	virtual void ToolTipsEnabled(bool enable) override;
	virtual bool InitializeToolTips() override;

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckBoxToolTips();

	afx_msg void OnSymbolUseInclude();
	afx_msg void OnSymbolUseExclude();
	afx_msg void OnSelchangeSymbol();
	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);
//	afx_msg LRESULT OnListBoxRangeChecked(WPARAM w, LPARAM l);

	afx_msg LRESULT OnListBoxIncludeExclude(WPARAM w, LPARAM l);
	afx_msg LRESULT OnListBoxFind(WPARAM w, LPARAM l);
	afx_msg LRESULT OnListBoxRemove(WPARAM w, LPARAM l);
	afx_msg LRESULT OnListBoxClear(WPARAM w, LPARAM l);
	afx_msg LRESULT OnListBoxCopy(WPARAM w, LPARAM l);
	afx_msg LRESULT OnListBoxPaste(WPARAM w, LPARAM l);
	afx_msg LRESULT OnListBoxUnPaste(WPARAM w, LPARAM l);

//	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnSymbolFind();
	afx_msg void OnSymbolInclude();
	afx_msg void OnSymbolExclude();
	afx_msg void OnSymbolRemove();
	afx_msg void OnSymbolClear();
	afx_msg void OnSymbolCopy();
	afx_msg void OnSymbolPaste();
	afx_msg void OnSymbolUnPaste();

	afx_msg void OnSymbolUseFirstChars();
	afx_msg void OnSymbolUseNoFirstChars();
	afx_msg void OnSymbolUseChars();
	afx_msg void OnSymbolUseNoChars();
//	afx_msg void OnSymbolFirstCharsUpdate();
	afx_msg void OnSymbolFirstCharsChange();
//	afx_msg void OnSymbolNoFirstCharsUpdate();
	afx_msg void OnSymbolNoFirstCharsChange();
//	afx_msg void OnSymbolCharsUpdate();
	afx_msg void OnSymbolCharsChange();
//	afx_msg void OnSymbolNoCharsUpdate();
	afx_msg void OnSymbolNoCharsChange();

	void DoDataExchangeRange(RangeGroupBase& cg, CDataExchange* pDX);
	void EnableFindButton();
	void EnableAddSymbolButtons();
	void AddSymbol(bool checked);

	COLORREF m_enabledColor;
	COLORREF m_enabledTextColor;
	COLORREF m_disabledColor;
	COLORREF m_invalidColor;
	COLORREF m_checkBoxColor;
	COLORREF m_pressedCheckColor;

	CheckBoxOwnerDraw m_CheckBoxToolTips;

	CheckBoxOwnerDraw m_CheckBoxSymbolInclude;
	CheckBoxOwnerDraw m_CheckBoxSymbolExclude;
	ValidEdit m_EditSymbol;
	ListBoxSymbolChecked m_ListBoxSymbol;
	CButton m_ButtonSymbolInclude;
	CButton m_ButtonSymbolExclude;
	CButton m_ButtonSymbolFind;
	CButton m_ButtonSymbolRemove;
	CButton m_ButtonSymbolClear;

	CButton m_GroupBoxSymbolChars;
	CheckBoxOwnerDraw m_CheckBoxFirstChars;
	CheckBoxOwnerDraw m_CheckBoxNoFirstChars;
	CheckBoxOwnerDraw m_CheckBoxChars;
	CheckBoxOwnerDraw m_CheckBoxNoChars;
	ValidEdit m_EditFirstChars;
	ValidEdit m_EditNoFirstChars;
	ValidEdit m_EditChars;
	ValidEdit m_EditNoChars;


	virtual void DoEnableOkButton(bool enable) override;

	void UpdateListBoxSymbolColor(bool enable);
	
	std::string m_firstChars;
	std::string m_noFirstChars;
	std::string m_chars;
	std::string m_noChars;

	bool m_useFirstChars;
	bool m_useNoFirstChars;
	bool m_useChars;
	bool m_useNoChars;

//	typedef std::map<unsigned __int64, bool, lessUIntAsStr<unsigned __int64> > SymbolMap;
//	SymbolMap m_symbolMap;

	bool m_lastUsedSymbolInclude;
};

class MyDialogPriceFilter : public MyDialogSymbolFilter
{
public:
	void Display(const TakionFilter& filter, TakionFrame* receipient, unsigned int code, CWnd* parent, int x, int y, int w, int h, bool show = true);
	virtual bool isFilterChanged() const override;
	virtual bool isFilterValid() const override;
	virtual bool isFilterEmpty() const override;
	virtual void SetFilterEmpty() override;
//	unsigned short GetTimeFrameMinutes() const;

//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void UpdateMyFields(MyPriceFilter& other) const;// , unsigned int doNotCopyFlags);
	virtual void UpdateSymbolFilterOfOtherKind(MySymbolFilter& other) const override
	{
		MyDialogSymbolFilter::UpdateSymbolFilterOfOtherKind(other);
		if(other.isPriceFilter())UpdateMyFields((MyPriceFilter&)other);
	}
	bool MyFieldsIntersectionDifferent(const MyPriceFilter& other, unsigned int doNotCopyFlags) const;
	virtual bool isSymbolFilterOfOtherKindIntersectionDifferent(const MySymbolFilter& other, unsigned int doNotCopyFlags) const override
	{
		return MyDialogSymbolFilter::isSymbolFilterOfOtherKindIntersectionDifferent(other, doNotCopyFlags) || other.isPriceFilter() && MyFieldsIntersectionDifferent((const MyPriceFilter&)other, doNotCopyFlags);
	}

	void MyFieldsToControls(const MyPriceFilter& other, unsigned int doNotCopyFlags);
	virtual void CopySymbolFilterOfOtherKindToControls(const MySymbolFilter& other, unsigned int doNotCopyFlags) override
	{
		MyDialogSymbolFilter::CopySymbolFilterOfOtherKindToControls(other, doNotCopyFlags);
		if(other.isPriceFilter())MyFieldsToControls((const MyPriceFilter&)other, doNotCopyFlags);
	}
protected:
	MyDialogPriceFilter(TakionMainWnd* mainWnd, int xPriceOffset, int priceTop, int xSymbolOffset, int xSymbolHeight, bool toDestroyOnFocus = false, TakionFrame* receipient = nullptr, CWnd* parent = nullptr, LPCSTR caption = nullptr, DWORD style = 0, DWORD exstendedStyle = 0, int x = 0, int y = 0, int w = 0, int h = 0,
		LPCSTR font = nullptr, WORD fontSize = 8, bool executeCommands = true, bool closeOnCommandExecution = true);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void BeforeShow() override;
	virtual void UpdateFilterValues() override;

//	virtual bool EnableToolTips(bool enable) override;
	virtual void ToolTipsEnabled(bool enable) override;

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnUsePrice();
	afx_msg void OnUseOpenPrice();
	afx_msg void OnUseDayNetChange();
	afx_msg void OnUseDayPriceRange();
	afx_msg void OnUseDayNetChangePercent();
	afx_msg void OnUseDayNetOpenChangePercent();
	afx_msg void OnUseDayPriceRangePercent();

	afx_msg void OnPositiveDayNetChange();
	afx_msg void OnNegativeDayNetChange();
	afx_msg void OnPositiveDayNetChangePercent();
	afx_msg void OnNegativeDayNetChangePercent();
	afx_msg void OnPositiveDayNetOpenChangePercent();
	afx_msg void OnNegativeDayNetOpenChangePercent();

	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);

	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	RangeGroup<unsigned int> m_price;
	RangeGroup<unsigned int> m_openPrice;
	RangeGroupPositiveNegative<unsigned int> m_dayNetChange;
	RangeGroup<unsigned int> m_dayPriceRange;
	RangeGroupPositiveNegative<unsigned int> m_dayNetChangePercent;
	RangeGroupPositiveNegative<unsigned int> m_dayNetOpenChangePercent;
	RangeGroup<unsigned int> m_dayPriceRangePercent;

	int m_secondColumnBottom;
};

class MyDialogStockFilter : public MyDialogPriceFilter
{
public:
	void Display(const TakionFilter& filter, TakionFrame* receipient, unsigned int code, CWnd* parent, int x, int y, int w, int h, bool show = true);
	virtual bool isFilterChanged() const override;
	virtual bool isFilterValid() const override;
	virtual bool isFilterEmpty() const override;
	virtual void SetFilterEmpty() override;
//	static const char zeroChar;
//	static const unsigned int securityTypeFilterAll;
//	static const unsigned int nasdaqQuoteConditionFilterAll;
//	static const unsigned int listedQuoteConditionFilterAll;

	void UpdateMyFields(MyStockFilter& other) const;
	virtual void UpdateSymbolFilterOfOtherKind(MySymbolFilter& other) const override
	{
		MyDialogPriceFilter::UpdateSymbolFilterOfOtherKind(other);
		if(other.isStockFilter())UpdateMyFields((MyStockFilter&)other);
	}
	bool MyFieldsIntersectionDifferent(const MyStockFilter& other, unsigned int doNotCopyFlags) const;
	virtual bool isSymbolFilterOfOtherKindIntersectionDifferent(const MySymbolFilter& other, unsigned int doNotCopyFlags) const override
	{
		return MyDialogPriceFilter::isSymbolFilterOfOtherKindIntersectionDifferent(other, doNotCopyFlags) || other.isStockFilter() && MyFieldsIntersectionDifferent((const MyStockFilter&)other, doNotCopyFlags);
	}

	void MyFieldsToControls(const MyStockFilter& other, unsigned int doNotCopyFlags);
	virtual void CopySymbolFilterOfOtherKindToControls(const MySymbolFilter& other, unsigned int doNotCopyFlags) override
	{
		MyDialogPriceFilter::CopySymbolFilterOfOtherKindToControls(other, doNotCopyFlags);
		if(other.isStockFilter())MyFieldsToControls((const MyStockFilter&)other, doNotCopyFlags);
	}
protected:
	MyDialogStockFilter(TakionMainWnd* mainWnd,
		int xPriceOffset, int priceTop, int xSymbolOffset, int xSymbolHeight, bool useTimeFrame,
		bool toDestroyOnFocus = false, TakionFrame* receipient = nullptr, CWnd* parent = nullptr, LPCSTR caption = nullptr, DWORD style = 0, DWORD exstendedStyle = 0, int x = 0, int y = 0, int w = 0, int h = 0,
		LPCSTR font = nullptr, WORD fontSize = 8, bool executeCommands = true, bool closeOnCommandExecution = true);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void BeforeShow() override;
	virtual void UpdateFilterValues() override;
	
	virtual void ToolTipsEnabled(bool enable) override;

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnUseYesterdaysVolume();
	afx_msg void OnUseTodaysVolume();
	afx_msg void OnUseTodaysPxMhVolume();
	afx_msg void OnUseAvgDailyVolume();
	afx_msg void OnUseRelativeVolume();
	afx_msg void OnUsePreMhVolRel();
	afx_msg void OnUseYesterdaysPriceRange();
	afx_msg void OnUseYesterdaysPriceRangePercent();
	afx_msg void OnUseImbalanceNyse();
	afx_msg void OnUseImbalanceNyseReg();
	afx_msg void OnUseImbalanceNsdq();
	afx_msg void OnUseImbalanceArca();
	afx_msg void OnUseImbalanceExch();
	afx_msg void OnUseRateImbalance();
	afx_msg void OnUseImbNyDeltaInf();
	afx_msg void OnUseImbExFarPriceDelta();
	afx_msg void OnUseImbNqFarPriceDelta();
	afx_msg void OnUseImbNqThru();
	afx_msg void OnUseImbExValue();
	afx_msg void OnUseImbExNear();
	afx_msg void OnUseImbExFar();
	afx_msg void OnUseImbExMatch();
	afx_msg void OnUseImbExVolPercent();
	afx_msg void OnUseImbExYestVolPercent();
	afx_msg void OnUseDeltaPv();
//	afx_msg void OnUseRsiMh();
//	afx_msg void OnUseRsiDays();
	afx_msg void OnUseBorrowPrice();
	afx_msg void OnUsePmiSpread();
	afx_msg void OnUseTodaysClosePrice();

	afx_msg void OnUseBidNet();
	afx_msg void OnUseAskNet();

	afx_msg void OnUseTBidNet();
	afx_msg void OnUseTAskNet();

	afx_msg void OnUseSpread();

	afx_msg void OnUseBidNetPercent();
	afx_msg void OnUseAskNetPercent();

	afx_msg void OnUseTBidNetPercent();
	afx_msg void OnUseTAskNetPercent();

	afx_msg void OnUseLuldDiff();
	afx_msg void OnPositiveLuldDiff();
	afx_msg void OnNegativeLuldDiff();

	afx_msg void OnUseLuldDiffPercent();
	afx_msg void OnPositiveLuldDiffPercent();
	afx_msg void OnNegativeLuldDiffPercent();

//	afx_msg void OnLongPosition();
//	afx_msg void OnShortPosition();

	afx_msg void OnPositiveImbalanceNyse();
	afx_msg void OnNegativeImbalanceNyse();
	afx_msg void OnPositiveImbalanceNyseReg();
	afx_msg void OnNegativeImbalanceNyseReg();
	afx_msg void OnPositiveImbalanceNsdq();
	afx_msg void OnNegativeImbalanceNsdq();
	afx_msg void OnPositiveImbalanceArca();
	afx_msg void OnNegativeImbalanceArca();
	afx_msg void OnPositiveImbalanceExch();
	afx_msg void OnNegativeImbalanceExch();
	afx_msg void OnPositiveRateImbalance();
	afx_msg void OnNegativeRateImbalance();
	afx_msg void OnPositiveImbNyDeltaInf();
	afx_msg void OnNegativeImbNyDeltaInf();
	afx_msg void OnPositiveImbExFarPriceDelta();
	afx_msg void OnNegativeImbExFarPriceDelta();
	afx_msg void OnPositiveImbNqFarPriceDelta();
	afx_msg void OnNegativeImbNqFarPriceDelta();
	afx_msg void OnPositiveImbNqThru();
	afx_msg void OnNegativeImbNqThru();
	afx_msg void OnPositiveImbExValue();
	afx_msg void OnNegativeImbExValue();

	afx_msg void OnPositiveBidNet();
	afx_msg void OnNegativeBidNet();
	afx_msg void OnPositiveAskNet();
	afx_msg void OnNegativeAskNet();

	afx_msg void OnPositiveTBidNet();
	afx_msg void OnNegativeTBidNet();
	afx_msg void OnPositiveTAskNet();
	afx_msg void OnNegativeTAskNet();

	afx_msg void OnPositiveSpread();
	afx_msg void OnNegativeSpread();

	afx_msg void OnPositiveBidNetPercent();
	afx_msg void OnNegativeBidNetPercent();
	afx_msg void OnPositiveAskNetPercent();
	afx_msg void OnNegativeAskNetPercent();

	afx_msg void OnPositiveTBidNetPercent();
	afx_msg void OnNegativeTBidNetPercent();
	afx_msg void OnPositiveTAskNetPercent();
	afx_msg void OnNegativeTAskNetPercent();
/*
	afx_msg void OnRsiMhCalculate();
	afx_msg void OnRsiMhTodayOnly();
	afx_msg void OnRsiMhPostMarket();

	afx_msg void OnRsiDaysCalculate();
	afx_msg void OnRsiDaysPostMarket();
*/
	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);

	CStatic m_StaticMarketCategory;
	ListBoxMarketCategory m_ListBoxMarketCategory;

	CStatic m_StaticTestOrReal;
	ListBoxFilterString<unsigned int> m_ListBoxTestOrReal;

	CStatic m_StaticSecurityType;
	ListBoxFilterString<unsigned int> m_ListBoxSecurityType;

	CStatic m_StaticHtb;
	ListBoxFilterString<unsigned int> m_ListBoxHtb;

	CStatic m_StaticTradingState;
	ListBoxFilterString<unsigned int> m_ListBoxTradingState;

	CStatic m_StaticCircuitBreaker;
	ListBoxFilterString<unsigned int> m_ListBoxCircuitBreaker;

	CStatic m_StaticNasdaqQc;
	ListBoxFilterString<unsigned int> m_ListBoxNasdaqQc;

	CStatic m_StaticListedQc;
	ListBoxFilterString<unsigned int> m_ListBoxListedQc;

	RangeGroup<unsigned int> m_yesterdaysVolume;
	RangeGroup<unsigned int> m_todaysVolume;
	RangeGroup<unsigned int> m_todaysPxMhVolume;
//	RangeGroup<unsigned int> m_volumeRate;
//	RangeGroup<unsigned int> m_percentTVolByYVol;
//	RangeGroupPositiveNegative<unsigned int> m_positionSize;
	RangeGroup<unsigned int> m_avgDailyVolume;
	RangeGroup<unsigned int> m_relativeVolume;
	RangeGroup<unsigned int> m_preMhVolRel;
	RangeGroup<unsigned int> m_yesterdaysPriceRange;
	RangeGroup<unsigned int> m_yesterdaysPriceRangePercent;

	RangeGroupPositiveNegative<unsigned int> m_imbalanceNyse;
	RangeGroupPositiveNegative<unsigned int> m_imbalanceNyseReg;
	RangeGroupPositiveNegative<unsigned int> m_imbalanceNsdq;
	RangeGroupPositiveNegative<unsigned int> m_imbalanceArca;
	RangeGroupPositiveNegative<unsigned int> m_imbalanceExch;
	RangeGroupPositiveNegative<unsigned int> m_rateImbalance;
	RangeGroupPositiveNegative<unsigned int> m_imbNyDeltaInf;
	RangeGroupPositiveNegative<unsigned int> m_imbExFarPriceDelta;
	RangeGroupPositiveNegative<unsigned int> m_imbNqFarPriceDelta;
	RangeGroupPositiveNegative<unsigned int> m_imbNqThru;
	RangeGroupPositiveNegative<unsigned __int64> m_imbExValue;

	RangeGroup<unsigned int> m_imbExNear;
	RangeGroup<unsigned int> m_imbExFar;

	RangeGroup<unsigned int> m_imbExMatch;
	RangeGroup<unsigned int> m_imbExVolPercent;
	RangeGroup<unsigned int> m_imbExYestVolPercent;

	RangeGroup<unsigned int> m_deltaPv;
	RangeGroup<unsigned int> m_borrowPrice;
	RangeGroup<unsigned int> m_pmiSpread;
	RangeGroup<unsigned int> m_todaysClosePrice;

	RangeGroupPositiveNegative<unsigned int> m_bidNet;
	RangeGroupPositiveNegative<unsigned int> m_askNet;

	RangeGroupPositiveNegative<unsigned int> m_tBidNet;
	RangeGroupPositiveNegative<unsigned int> m_tAskNet;

	RangeGroupPositiveNegative<unsigned int> m_spread;

	RangeGroupPositiveNegative<unsigned int> m_bidNetPercent;
	RangeGroupPositiveNegative<unsigned int> m_askNetPercent;

	RangeGroupPositiveNegative<unsigned int> m_tBidNetPercent;
	RangeGroupPositiveNegative<unsigned int> m_tAskNetPercent;
/*
	RangeGroup<unsigned int> m_rsiMh;
	RangeGroup<unsigned int> m_rsiDays;

	CButton m_GroupBoxRsiMhCalculation;
	CheckBoxOwnerDraw m_CheckBoxRsiMhCalculate;
	CheckBoxOwnerDraw m_CheckBoxRsiMhTodayOnly;
	CStatic m_StaticRsiMhPriorPointCount;
	UnsignedNumberTypeSpin<unsigned short> m_SpinRsiMhPriorPointCount;
	CStatic m_StaticRsiMhPeriodLength;
	UnsignedNumberTypeSpin<unsigned short> m_SpinRsiMhPeriodLength;
	CStatic m_StaticRsiMhPeriodCount;
	UnsignedNumberTypeSpin<unsigned short> m_SpinRsiMhPeriodCount;
	CStatic m_StaticRsiMhStartMinute;
	MinuteSpin m_SpinRsiMhStartMinute;
	CheckBoxOwnerDraw m_CheckBoxRsiMhPostMarket;
	bool m_calculateRsiMh;

	CButton m_GroupBoxRsiDaysCalculation;
	CheckBoxOwnerDraw m_CheckBoxRsiDaysCalculate;
	CStatic m_StaticRsiDaysPriorPointCount;
	UnsignedNumberTypeSpin<unsigned short> m_SpinRsiDaysPriorPointCount;
	CStatic m_StaticRsiDaysPeriodLength;
	UnsignedNumberTypeSpin<unsigned short> m_SpinRsiDaysPeriodLength;
	CStatic m_StaticRsiDaysPeriodCount;
	UnsignedNumberTypeSpin<unsigned short> m_SpinRsiDaysPeriodCount;
	CStatic m_StaticRsiDaysStartMinute;
	MinuteSpin m_SpinRsiDaysStartMinute;
	CheckBoxOwnerDraw m_CheckBoxRsiDaysPostMarket;
	bool m_calculateRsiDays;
*/
	RangeGroupPositiveNegative<unsigned int> m_luldDiff;
	RangeGroupPositiveNegative<unsigned int> m_luldDiffPercent;

	int m_fourthColumnBottom;
	int m_fifthColumnBottom;
	int m_sixthColumnBottom;
	int m_seventhColumnBottom;
	int m_eighthColumnBottom;

	int m_timeFrameHeight;
	int m_leftMostColumnBottom;

	int m_leftMostColumnBottomPixel;
	int m_leftMostColumnLeftPixel;
	int m_leftMostColumnWidthPixel;
	int m_staticHeightPixel;

	int m_fifthColumnLeftPixel;
	int m_fifthColumnWidthPixel;
	int m_fifthColumnBottomPixel;

	unsigned int m_marketCategoryVisibleItemCount;
	unsigned int m_testVisibleItemCount;
	unsigned int m_securityTypeVisibleItemCount;
	unsigned int m_htbVisibleItemCount;
	unsigned int m_tradingStateVisibleItemCount;
	unsigned int m_quoteConditionVisibleItemCount;
};

class MyMarketSorterFilter : public MyStockFilter
{
public:
	MyMarketSorterFilter()
	{}
	virtual bool isEmpty() const override
	{
		return MyStockFilter::isEmpty();
	}
	virtual void Copy(const TakionFilter& other) override;

	virtual TakionFilter* CreateFilterCopy() const override;
	virtual unsigned char isA() const override{return MY_MARKET_SORTER;}
	virtual bool isMarketSorterFilter() const override{return true;}
	void CopyMyFields(const MyMarketSorterFilter& other, unsigned int doNotCopyFlags);
	virtual void CopySymbolFilterOfOtherKind(const MySymbolFilter& other, unsigned int doNotCopyFlags) override
	{
		if(other.isMarketSorterFilter())CopyMyFields((const MyMarketSorterFilter&)other, doNotCopyFlags);
		MyStockFilter::CopySymbolFilterOfOtherKind(other, doNotCopyFlags);
	}
	bool MyFieldsEqual(const MyMarketSorterFilter& other, unsigned int doNotCopyFlags) const;
	virtual bool isSymbolIntersectionEqual(const MySymbolFilter& other, unsigned int doNotCopyFlags) const override
	{
		return MyStockFilter::isSymbolIntersectionEqual(other, doNotCopyFlags) && (!other.isMarketSorterFilter() || MyFieldsEqual((const MyMarketSorterFilter&)other, doNotCopyFlags));
	}
	virtual bool operator==(const TakionFilter& other) const override
	{
		return MyStockFilter::operator==(other) && MyFieldsEqual((const MyMarketSorterFilter&)other, 0);
	}

	virtual void toString(std::string& str) const override;
	virtual void fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier) override;
	virtual bool SecurityBelongs(const SecurityChartRow* securityChartRow) const override;
	virtual bool SecurityPotentiallyBelongs(const Security* security) const override;
	virtual unsigned char isPotentialFilterSame(const TakionFilter& filter) const override;//0 - same; 1 - to add; 2 - to remove; 3 - to add and remove
	virtual bool isFilterSame(const TakionFilter& filter) const override;
	virtual void Reset() override
	{
		MyStockFilter::Reset();
	}
};

class MyMarketSorter;

class MyDialogMarketSorterFilter : public MyDialogStockFilter
{
public:
	MyDialogMarketSorterFilter(TakionMainWnd* mainWnd, bool toDestroyOnFocus = false, MyMarketSorter* receipient = nullptr, CWnd* parent = nullptr);
	void Display(const TakionFilter& filter, MyMarketSorter* receipient, unsigned int code, CWnd* parent, int x, int y);// , int w, int h, bool show = true);
protected:
	virtual void ToolTipsEnabled(bool enable) override;
};

