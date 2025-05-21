#include "stdafx.h"
#include "SampleDialogSettingStockMarketSorter.h"
#include "StockBase.h"
//#include "StockMarketSorter.h"
#include "ExtensionSample.h"
#include "ExtensionDerivedConfig.h"
#include <fstream>

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

#ifdef SAMPLE_MARKET_SORTER_TIMERS
class OneSecondStockState : public RingVectorBaseItem
{
public:
	OneSecondStockState(const Price& last):m_last(last)
	{
	}
	const Price& GetLast() const{return m_last;}
	DECLARE_NED_NEW
protected:
	Price m_last;
};

class StockState
{
public:
	StockState(unsigned int millisecondFromMidnight, const Price& high, const Price& low):
		m_millisecondFromMidnight(millisecondFromMidnight),
		m_high(high),
		m_low(low)
	{
		UpdateRange();
	}
	StockState(unsigned int millisecondFromMidnight):
		m_millisecondFromMidnight(millisecondFromMidnight)
	{}
	const unsigned int& GetMillisecondFromMidnight() const{return m_millisecondFromMidnight;}
	const Price& GetHigh() const{return m_high;}
	const Price& GetLow() const{return m_low;}
	const Price& GetRange() const{return m_range;}
	void SetHighAndLow(const Price& high, const Price& low)
	{
		m_high = high;
		m_low = low;
		UpdateRange();
	}
	DECLARE_NED_NEW
protected:
	void UpdateRange()
	{
		m_range = m_high.GetPercentChange(m_high - m_low) * 100;
	}
	unsigned int m_millisecondFromMidnight;
	Price m_high;
	Price m_low;
	Price m_range;
};

#endif

class StockMarketSorter : public StockBase
{
public:
	StockMarketSorter(const char* symbol,
#ifdef STOCK_PROPERTY
		const Price& stockProperty,
#endif
		const bool& updateOnTimer,
		const bool& stockInThread,
		const bool& positionInThread,
		ListBoxStockMarketSorter* owner):
		StockBase(symbol),
		m_ordinal(0xFFFFFFFF),//ordinal number in the list box.
		m_owner(owner),
#ifdef STOCK_PROPERTY
		m_stockProperty(stockProperty),
#endif
		m_updateOnTimer(updateOnTimer),
		m_inThread(stockInThread),
		m_positionInThread(positionInThread),
		m_level1Bid(0),
		m_level1Ask(0),
		m_lastPrint(0),
		m_peOpen(0),
		m_peTodaysClose(0),
		m_yestClose(0),
		m_volume(0),
		m_lastSize(0)
#ifdef DEBUG_LOCK
		,m_inquryLocked(false),
		m_modificationLocked(false)
#endif
#ifdef SAMPLE_MARKET_SORTER_TIMERS
		,m_stateMapEnd(m_stateMap.end()),
		m_oneSecondStates(U_CreateTakionRingVector(600, true, true))
#endif
	{
		SetSecurity(TD_FindSortableEquityWithNoIncrementUnsubscribe(symbol));
	}
	StockMarketSorter(const Security* security,
#ifdef STOCK_PROPERTY
		const Price& stockProperty,
#endif
		const bool& updateOnTimer,
		const bool& stockInThread,
		const bool& positionInThread,
		ListBoxStockMarketSorter* owner):
		StockBase(security->GetSymbol()),
		m_ordinal(0xFFFFFFFF),//ordinal number in the list box.
		m_owner(owner),
#ifdef STOCK_PROPERTY
		m_stockProperty(stockProperty),
#endif
		m_updateOnTimer(updateOnTimer),
		m_inThread(stockInThread),
		m_positionInThread(positionInThread),
		m_level1Bid(0),
		m_level1Ask(0),
		m_lastPrint(0),
		m_peOpen(0),
		m_peTodaysClose(0),
		m_yestClose(0),
		m_volume(0),
		m_lastSize(0)
#ifdef DEBUG_LOCK
		,m_inquryLocked(false),
		m_modificationLocked(false)
#endif
#ifdef SAMPLE_MARKET_SORTER_TIMERS
		,m_stateMapEnd(m_stateMap.end()),
		m_oneSecondStates(U_CreateTakionRingVector(600, true, true))
#endif
	{
		SetSecurity(security);
	}
	virtual ~StockMarketSorter();

#ifdef DEBUG_LOCK
	const bool& isInquryLocked() const{return m_inquryLocked;}
	const bool& isModificationLocked() const{return m_modificationLocked;}

	bool LockInquiry()
	{
		if(!m_inquryLocked && !m_inThread)
		{
			if(m_security)
			{
				m_security->LockInquiryWaitLevel1();
				m_inquryLocked = true;
				return true;
			}
			else if(m_numericSymbol)
			{
				const Security* security = TD_FindStockByNumericSymbol(m_numericSymbol);
//				const Security* security = TD_FindSortableEquityByNumericSymbol(m_numericSymbol);
				if(security)
				{
					security->LockInquiryWaitLevel1();
					m_inquryLocked = true;
//					TD_ReleaseStock(security);
					return true;
				}
			}
		}
		return false;
	}
	bool UnlockInquiry()
	{
		if(m_inquryLocked && !m_inThread)
		{
			bool ret = false;
			if(m_security)
			{
				ret = true;
				m_security->UnlockInquiryLevel1();
			}
			else if(m_numericSymbol)
			{
				const Security* security = TD_FindStockByNumericSymbolNoUseCount(m_numericSymbol);
//				const Security* security = TD_FindSortableEquityByNumericSymbol(m_numericSymbol);
				if(security)
				{
					ret = true;
					security->UnlockInquiryLevel1();
					m_inquryLocked = true;
					TD_ReleaseStock(security);
				}
			}

			m_inquryLocked = false;
			return ret;
		}
		return false;
	}

	bool LockModification()
	{
		if(!m_modificationLocked && m_inThread && m_numericSymbol)
		{
//			TD_LockModificationMsStock(m_numericSymbol);//m_security->GetNumericSymbol());
			TD_LockModificationMdStock(m_numericSymbol);
			m_modificationLocked = true;
			return true;
		}
		return false;
	}
	bool UnlockModification()
	{
		if(m_modificationLocked && m_inThread && m_numericSymbol)
		{
//			if(m_security)
//			TD_UnlockModificationMsStock(m_numericSymbol);//m_security->GetNumericSymbol());
			TD_UnlockModificationMdStock(m_numericSymbol);

			m_modificationLocked = false;
			return true;
		}
		return false;
	}
#endif
	void SetUpdateOnTimer(const bool& updateOnTimer)
	{
		if(updateOnTimer != m_updateOnTimer)
		{
			m_updateOnTimer = updateOnTimer;
			if(m_security)
			{
				if(m_updateOnTimer)
				{
					if(m_inThread)
					{
						m_security->RemoveInThreadObserver(this);
					}
					else
					{
						m_security->RemoveObserver(this);
					}
				}
				else
				{
					if(m_inThread)
					{
						m_security->AddInThreadObserver(this);
					}
					else
					{
						m_security->AddObserver(this);
					}
				}
			}
			if(m_position)
			{
				if(m_updateOnTimer)
				{
					if(m_positionInThread)
					{
						m_position->RemoveInThreadObserver(this);
					}
					else
					{
						m_position->RemoveObserver(this);
					}
				}
				else
				{
					if(m_positionInThread)
					{
						m_position->AddInThreadObserver(this);
					}
					else
					{
						m_position->AddObserver(this);
					}
				}
			}
		}
	}

	void SetInThread(const bool& inThread)
	{
		if(inThread != m_inThread)
		{
			m_inThread = inThread;
			if(m_security && !m_updateOnTimer)
			{
				if(m_inThread)
				{
					m_security->AddInThreadObserver(this);
					m_security->RemoveObserver(this);
				}
				else
				{
					m_security->AddObserver(this);
					m_security->RemoveInThreadObserver(this);
				}
			}
		}
	}

	void SetPositionInThread(const bool& inThread)
	{
		if(inThread != m_positionInThread)
		{
			m_positionInThread = inThread;
			if(m_position && !m_updateOnTimer)
			{
				if(m_positionInThread)
				{
					m_position->AddInThreadObserver(this);
					m_position->RemoveObserver(this);
				}
				else
				{
					m_position->AddObserver(this);
					m_position->RemoveInThreadObserver(this);
				}
			}
		}
	}

#ifdef STOCK_PROPERTY
	const Price& GetStockProperty() const{return m_stockProperty;}
	void SetStockProperty(const Price& stockProperty){m_stockProperty = stockProperty;}
#endif
//	const Price& GetLevel1Bid() const{return m_level1Bid;}
//	const Price& GetLevel1Ask() const{return m_level1Ask;}
	const unsigned int& GetLevel1Bid() const{return m_level1Bid;}
	const unsigned int& GetLevel1Ask() const{return m_level1Ask;}
	const unsigned int& GetLastPrint() const{return m_lastPrint;}
	const SignedPrice& GetLevel1Spread() const{return m_level1Spread;}
	const Price& GetHigh() const{return m_high;}
	const Price& GetLow() const{return m_low;}

	const unsigned int& GetPeOpen() const{return m_peOpen;}
	const unsigned int& GetPeTodaysClose() const{return m_peTodaysClose;}
	const unsigned int& GetYestClose() const{return m_yestClose;}

	const unsigned __int64& GetVolume() const{return m_volume;}
	const unsigned int& GetLastSize() const{return m_lastSize;}

	void SetOrdinal(const unsigned int& ordinal){m_ordinal = ordinal;}
	const unsigned int& GetOrdinal() const{return m_ordinal;}
	void IncrementOrdinal(){++m_ordinal;}
	void IncrementOrdinal(unsigned int number){m_ordinal += number;}
	void DecrementOrdinal(){if(m_ordinal)--m_ordinal;}
	void DecrementOrdinal(unsigned int number){if(m_ordinal > number)m_ordinal -= number; else m_ordinal = 0;}

	void Repaint(int left = -1, int right = -1);
	void RepaintColumnRange(const unsigned short& idFrom, const unsigned short& idTo)
	{
		m_owner->InvalidateItemColumnRange(m_ordinal, idFrom, idTo);
	}
	void RepaintColumn(const unsigned short& id)
	{
		m_owner->InvalidateItemColumn(m_ordinal, id);
	}

	void UpdatePositionTotals(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff);
	virtual void PositionValuesChanged(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff) override;

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	const Price& GetRangeAtFirstTimer() const{return m_rangeAtFirstTimer;}
	typedef std::hash_map<unsigned int, StockState> StateMap;
	const StateMap& GetStateMap() const{return m_stateMap;}
	void UpdateTimerTotals(const SignedPrice& rangeAtFirstTimer);
	const OneSecondStockState* GetOneSecondState(unsigned int countBack) const{return (const OneSecondStockState*)m_oneSecondStates->fromBack(countBack);}
	virtual void ResetTimer() override
	{
		StockBase::ResetTimer();
		m_stateMap.clear();
		m_rangeAtFirstTimer.SetZero();
	}

	void Trigger(unsigned int millisecondFromMidnight);
	void TriggerOneSecond(unsigned int count);

	unsigned int CalculateOrderSize() const;
	unsigned int CalculatePositionPercent(unsigned int percent) const;

	unsigned char SendMarketOrder(char side, unsigned int orderSize, bool close);
	void ClosePositionPercent(unsigned int percent, const Price& away);
	void ClosePosition();
	void CancelOpeningOrders();
#endif
	DECLARE_NED_NEW

	virtual void RemoveSecurityObserver(const Security* security) override
	{
		if(!m_updateOnTimer)
		{
			if(m_inThread)
			{
				security->RemoveInThreadObserver(this);
			}
			else
			{
				security->RemoveObserver(this);
			}
		}
	}
	virtual void AddSecurityObserver(const Security* security) override
	{
		if(!m_updateOnTimer)
		{
			if(m_inThread)
			{
				security->AddInThreadObserver(this);
			}
			else
			{
				security->AddObserver(this);
			}
		}
	}
	virtual void RemovePositionObserver(const Position* position) override
	{
		if(!m_updateOnTimer)
		{
			if(m_positionInThread)
			{
				position->RemoveInThreadObserver(this);
			}
			else
			{
				position->RemoveObserver(this);
			}
		}
	}
	virtual void AddPositionObserver(const Position* position) override
	{
		if(!m_updateOnTimer)
		{
			if(m_positionInThread)
			{
				position->AddInThreadObserver(this);
			}
			else
			{
				position->AddObserver(this);
			}
		}
	}
//Since the imbalances are updated in the worker thread, we need to LockInquiry when we paint in the main thread, and LockModification when we change the values in the worker thread.
	bool LockPaintStockInquiry() const{return m_paintLock.LockInquiry();}
	void LockPaintStockInquiryWait() const{m_paintLock.LockInquiryWait();}
	void UnlockPaintStockInquiry() const{m_paintLock.UnlockInquiry();}
	bool LockPaintStockModification() const{return m_paintLock.LockModification();}
	void UnlockPaintStockModification() const{m_paintLock.UnlockModification();}
	bool UpdateValuesInMainThread(bool lockWait);
protected:
//Update separate fields. When processing InThread it's cheaper to do than updating all fields.
	bool UpdateLevel1Bid();
	bool UpdateLevel1Ask();
	void UpdateLevel1AndInvalidate(const bool& bid)
	{
		if(bid)
		{
			if(UpdateLevel1Bid())
			{
				m_owner->InvalidateItemColumn(m_ordinal, MSCI_LEVEL1_BID);
			}
		}
		else
		{
			if(UpdateLevel1Ask())
			{
				m_owner->InvalidateItemColumn(m_ordinal, MSCI_LEVEL1_ASK);
			}
		}
	}
	bool UpdateLevel1Spread()
	{
		SignedPrice spread;
		if(m_level1Bid && m_level1Ask)
		{
			spread = Price(m_level1Ask) - Price(m_level1Bid);
		}
		if(spread != m_level1Spread)
		{
			m_level1Spread = spread;
			return true;
		}
		return false;
	}

	bool UpdateYesterdaysClosePrice();
	void UpdateYesterdaysClosePriceAndInvalidate()
	{
		if(UpdateYesterdaysClosePrice())
		{
			m_owner->InvalidateItemColumn(m_ordinal, MSCI_YEST_CLOSE);
		}
	}

	bool UpdatePeOpenPrice();
	void UpdatePeOpenPriceAndInvalidate()
	{
		if(UpdatePeOpenPrice())
		{
			m_owner->InvalidateItemColumn(m_ordinal, MSCI_PE_OPEN);
		}
	}

	bool UpdatePeTodaysClosePrice();
	void UpdatePeTodaysClosePriceAndInvalidate()
	{
		if(UpdatePeTodaysClosePrice())
		{
			m_owner->InvalidateItemColumn(m_ordinal, MSCI_PE_TODAYS_CLOSE);
		}
	}

	void CheckSaleConditionUpdatePrintAndInvalidate(const char marketCenterId, const char saleCondition);
	void CheckSaleConditionUpdatePrintAndInvalidate(const char marketCenterId, const char saleCondition2, const char saleCondition4);

	void UpdateAggregatedOpeningAndClosingPrice(const unsigned char flags);

	bool UpdatePrint();
	void UpdatePrintAndInvalidate()
	{
		if(UpdatePrint())
		{
			m_owner->InvalidateItemColumnRange(m_ordinal, MSCI_PRINT, MSCI_VOLUME);
		}
	}
//
	virtual bool DoInitialize() override;
	void DoInitializeWithoutLocking();
	void InitializeInThread();
	virtual void DoReset() override;
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
	unsigned int m_ordinal;//ordinal number in the list box.
	TakionLockMultithread m_paintLock;//Since the imbalances are updated in the worker thread, we need to LockInquiry when we paint in the main thread, and LockModification when we change the values in the worker thread.
	ListBoxStockMarketSorter* m_owner;

	bool m_updateOnTimer;
	bool m_inThread;
	bool m_positionInThread;

#ifdef STOCK_PROPERTY
	Price m_stockProperty;
#endif
	unsigned int m_level1Bid;
	unsigned int m_level1Ask;
	unsigned int m_lastPrint;
	SignedPrice m_level1Spread;
	Price m_high;
	Price m_low;
	unsigned int m_peOpen;
	unsigned int m_peTodaysClose;
	unsigned int m_yestClose;
	unsigned __int64 m_volume;
	unsigned int m_lastSize;

#ifdef DEBUG_LOCK
	bool m_inquryLocked;
	bool m_modificationLocked;
#endif
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	Price m_rangeAtFirstTimer;
	StateMap m_stateMap;
	StateMap::const_iterator m_stateMapEnd;

	TakionRingVectorBase* m_oneSecondStates;
#endif
	virtual void PositionEntered() override;
	virtual void PositionFlat() override;
	virtual void PositionFlipped() override;
};


class LbStockMarketSorterHorizontalScroller : public LbHorizontalScroller
{
public:
	LbStockMarketSorterHorizontalScroller(ListBoxStockMarketSorter* listBox);
	virtual void AddColumns() override;
protected:
	ListBoxStockMarketSorter* m_listBoxMS;
};

class TMsgExtensionSamplePositionValues : public TMsgCustomExternal
{
public:
	TMsgExtensionSamplePositionValues(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff):
		TMsgCustomExternal(POSITION_VALUES, sizeof(TMsgExtensionSamplePositionValues)),
		m_closedPnlDiff(closedPnlDiff),
		m_openPnlDiff(openPnlDiff),
		m_openPrintPnlDiff(openPrintPnlDiff)
	{}
	Money m_closedPnlDiff;
	Money m_openPnlDiff;
	Money m_openPrintPnlDiff;
};

class TMsgExtensionSamplePnl : public TMsgCustomExternal
{
public:
	Money m_pnl;
protected:
	TMsgExtensionSamplePnl(const Money& pnl, const unsigned short& type, const unsigned short& length):
		TMsgCustomExternal(type, length),
		m_pnl(pnl)
	{}
};

class TMsgExtensionSampleOpenPrintPnlDiff : public TMsgExtensionSamplePnl
{
public:
	TMsgExtensionSampleOpenPrintPnlDiff(const Money& openPrintPnlDiff):
		TMsgExtensionSamplePnl(openPrintPnlDiff, POSITION_OPEN_PRINT_PNL_DIFF, sizeof(TMsgExtensionSampleOpenPrintPnlDiff))
	{}
};

class TMsgExtensionSampleOpenLevel1PnlDiff : public TMsgExtensionSamplePnl
{
public:
	TMsgExtensionSampleOpenLevel1PnlDiff(const Money& openLevel1PnlDiff):
		TMsgExtensionSamplePnl(openLevel1PnlDiff, POSITION_OPEN_LEVEL1_PNL_DIFF, sizeof(TMsgExtensionSampleOpenLevel1PnlDiff))
	{}
};

class TMsgExtensionSampleClosedPnlDiff : public TMsgExtensionSamplePnl
{
public:
	TMsgExtensionSampleClosedPnlDiff(const Money& closedPnlDiff):
		TMsgExtensionSamplePnl(closedPnlDiff, POSITION_CLOSED_PNL_DIFF, sizeof(TMsgExtensionSampleClosedPnlDiff))
	{}
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LbStockMarketSorterHorizontalScroller::LbStockMarketSorterHorizontalScroller(ListBoxStockMarketSorter* listBox):
	LbHorizontalScroller(listBox),
	m_listBoxMS(listBox)
{
}

void LbStockMarketSorterHorizontalScroller::AddColumns()
{
	const int itemHeight = m_listBox->GetMyItemHeight();
	const int doubleHeight = itemHeight + itemHeight;
	const int tripleHeight = doubleHeight + itemHeight;
	const int fourHeight = tripleHeight + itemHeight;
	const int fiveHeight = fourHeight + itemHeight;
	const int sixHeight = fiveHeight + itemHeight;
	AddColumn("Symbol", MSCI_SYMBOL, fiveHeight, DT_LEFT, DT_LEFT, DT_RIGHT, "Symbol");//, 0xFFFFFFFF, itemHeight, itemHeight);
#ifdef DEBUG_LOCK
	AddColumn("Lock?", MSCI_LOCKED_INQUIRY, tripleHeight, DT_CENTER, DT_CENTER, DT_RIGHT, "Locked for Inquiry");
	AddColumn("Lock!", MSCI_LOCKED_MODIFICATION, tripleHeight, DT_CENTER, DT_CENTER, DT_RIGHT, "Locked for Modification");
#endif
	AddColumn("Bid", MSCI_LEVEL1_BID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level1 Bid");
	AddColumn("Ask", MSCI_LEVEL1_ASK, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level1 Ask");
	AddColumn("Spread", MSCI_LEVEL1_SPREAD, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level1 Spread (Level1 Ask - Level1 Bid)");
	AddColumn("Last", MSCI_PRINT, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Last Print Price");
	AddColumn("Low", MSCI_LOW, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Low Price");
	AddColumn("High", MSCI_HIGH, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "High Price");
	AddColumn("Size", MSCI_LAST_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Last Print Size");
	AddColumn("Volume", MSCI_VOLUME, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Volume");

	AddColumn("PeOpn", MSCI_PE_OPEN, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Primary Exchange Open Price");
	AddColumn("PeTCls", MSCI_PE_TODAYS_CLOSE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Primary Exchange Today's Close Price");
	AddColumn("YCls", MSCI_YEST_CLOSE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Yesterday's Close Price");

	AddColumn("pSize", MSCI_POS_SIZE, tripleHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Position Size (Negative for Short)");
	AddColumn("peBuy", MSCI_BUY_PENDING, tripleHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Buy Pending Shares");
	AddColumn("peSell", MSCI_SELL_PENDING, tripleHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Sell Pending Shares");
	AddColumn("pEntry", MSCI_POS_TIMESTAMP, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Position Entry Timestamp");
	AddColumn("pPrice", MSCI_POS_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Average Position Price");
	AddColumn("ePrice", MSCI_LAST_EXEC_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Last Execution Price");
	AddColumn("ClosedP&L", MSCI_CLOSED_PNL, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Closed P&&L");
	AddColumn("OpenP&L", MSCI_OPEN_PNL_LEVEL1, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Open P&&L based on Level1");
	AddColumn("OpenPrP&L", MSCI_OPEN_PNL_PRINT, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Open P&&L based on Last NBBO Print");
#ifdef STOCK_PROPERTY
	AddColumn("Property", MSCI_STOCK_PROPERTY, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Stock Property");
#endif
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	AddColumn("Range", MSCI_RANGE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Range at first timer");
	AddColumn(m_listBoxMS->GetOwnerDialog()->GetOneSecondPrintTitle().c_str(), MSCI_ONE_SECOND, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "NBBO Print Price N seconds ago");
#endif
}

///////////////////////////

#ifdef SAMPLE_MARKET_SORTER_TIMERS

OneSecondTimer::OneSecondTimer(unsigned int cycle, ListBoxStockMarketSorter* owner):
	TakionTimerRepeat(cycle, 0, false),
	m_owner(owner)
{
}

void OneSecondTimer::Triggered(unsigned int count)
{
	m_owner->TriggerOneSecond(count);
}

StockTimer::StockTimer(unsigned int millisecondFromMidnight, ListBoxStockMarketSorter* owner, ListBoxTimer* timerBox):
	TakionTimerOnce(TL_GetCurrentMillisecond(), false),
	m_owner(owner),
	m_timerBox(timerBox),
	m_ordinal(0xFFFFFFFF),//ordinal number in the list box.
	m_millisecondFromMidnight(millisecondFromMidnight),
	m_triggered(false),
	m_old(m_milliseconds > millisecondFromMidnight)
{
	if(!m_old)
	{
		m_milliseconds = millisecondFromMidnight - m_milliseconds;
	}
}

void StockTimer::Triggered(unsigned int count)
{
//	m_timerBox->RemoveTimer(this);
	m_triggered = true;
	m_owner->Trigger(m_millisecondFromMidnight);
	m_timerBox->InvalidateItem(m_ordinal);
}

void ListBoxTimer::DestroyItem(const void* item) const
{
	delete (StockTimer*)item;
}

void ListBoxTimer::RefreshOrdinals()
{
	StockTimer* timer;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		timer = (StockTimer*)GetItemDataPtr(i);
		timer->SetOrdinal(i);
	}
}

int ListBoxTimer::Compare(const void* item1, const void* item2) const
{
	const StockTimer* stockTimer1 = (const StockTimer*)item1;
	const StockTimer* stockTimer2 = (const StockTimer*)item2;
	unsigned int n1 = stockTimer1->GetMillisecondFromMidnight();
	unsigned int n2 = stockTimer2->GetMillisecondFromMidnight();
	return n1 < n2 ? -1 : n1 > n2 ? 1 : 0;
}

const char* ListBoxTimer::GetItemName(const void* item) const
{
	static char num[33];
	U_FormatMillisecond(((const StockTimer*)item)->GetMillisecondFromMidnight(), num, sizeof(num), '\0', true);//m_milliseconds);
	return num;
}

COLORREF ListBoxTimer::GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	const StockTimer* stockTimer = (const StockTimer*)lpDrawItemStruct->itemData;
	return stockTimer->isTriggered() ? m_triggeredColor : stockTimer->isOld() ? m_oldColor : m_untriggeredColor;
}

COLORREF ListBoxTimer::GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	const StockTimer* stockTimer = (const StockTimer*)lpDrawItemStruct->itemData;
	return m_selectable && (lpDrawItemStruct->itemState & ODS_SELECTED) != 0 ?
		RGB(255, 255, 255)://GetSysColor(COLOR_HIGHLIGHTTEXT):
		stockTimer->isTriggered() ? m_triggeredColor : stockTimer->isOld() ? m_oldColor : m_untriggeredColor;
}

void StockMarketSorter::TriggerOneSecond(unsigned int count)
{
#ifdef _DEBUG
if(count != 1)
{
printf("X");
}
#endif
	for(; count; --count)
	{
		m_oneSecondStates->push_back(new OneSecondStockState(Price(m_lastPrint)));
	}
}

void StockMarketSorter::Trigger(unsigned int millisecondFromMidnight)
{
	StateMap::iterator found = m_stateMap.find(millisecondFromMidnight);
	if(found == m_stateMapEnd)
	{
		found = m_stateMap.insert(StateMap::value_type(millisecondFromMidnight, StockState(millisecondFromMidnight))).first;
	}
//Remember High and Low
	found->second.SetHighAndLow(m_high, m_low);
	if(found == m_stateMap.begin())
	{
		const Price& range = found->second.GetRange();
		if(m_rangeAtFirstTimer != range)
		{
			UpdateTimerTotals(SignedPrice(range) - SignedPrice(m_rangeAtFirstTimer));
			m_rangeAtFirstTimer = range;
			Repaint(m_owner->GetRangeLeft(), m_owner->GetRangeRight());
		}
	}
}

void StockMarketSorter::UpdateTimerTotals(const SignedPrice& rangeAtFirstTimerDiff)
{
	m_owner->UpdateTimerTotals(rangeAtFirstTimerDiff);
}

#endif

StockMarketSorter::~StockMarketSorter()
{
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	if(m_oneSecondStates)
	{
		delete m_oneSecondStates;
	}
#endif
}


void StockMarketSorter::PositionEntered()
{
}

void StockMarketSorter::PositionFlat()
{
}

void StockMarketSorter::PositionFlipped()
{
}

#ifdef SAMPLE_MARKET_SORTER_TIMERS
unsigned int StockMarketSorter::CalculateOrderSize() const
{
//	if(!m_lastPrint.isZero())
	if(m_lastPrint)
	{
		Price lastPrint(m_lastPrint);
		unsigned int orderSize = 3000000 / (lastPrint.GetDollars() * 100 + lastPrint.GetDollarFraction() / (Price::divider / 100));
		if(orderSize < m_roundLot)
		{
			orderSize = m_roundLot;
		}
		else
		{
			orderSize /= m_roundLot;
			orderSize *= m_roundLot;
		}
		return orderSize;
	}
	return 0;
}

unsigned char StockMarketSorter::SendMarketOrder(char side, unsigned int orderSize, bool close)
{
	if(orderSize)
	{
		if(m_account)
		{
			if(m_account->isSimulation())//SIMULATION only
			{
				unsigned int destinationId = SPEED_ROUTE;
				unsigned int destinationName = qstkNum;
//				const Price& zeroPrice = TU_GetPriceZero();
//				const SignedPrice& zeroSignedPrice = TU_GetSignedPriceZero();
				bool preBorrow = false;
				bool imbalanceOnly = false;
				const Price& preBorrowPrice = Price::priceZero;//TU_GetPriceZero();
				unsigned char errorCode = ((Account*)m_account)->SendOrder(
					nullptr,//unsigned int* orderClientId - output parameter
					nullptr,//Order* orderToreplace
					true,//replaceNative
					0,//int replaceSizeOffset
					m_security,
					0,//timeToActivate
					PE_NOT_POSTPONED,
					destinationName,
					destinationId,
					side,
					ORDER_TYPE_MARKET,
					0,//userOrderType
					nullptr,//userOrderInfo
					Price::priceZero,//limitPrice,
					Price::priceZero,//discretionaryPrice,
					Price::priceZero,//signedStopPrice,
					OST_NONE,//venueOrderParams.GetStopType(),
					OSB_BID,//venueOrderParams.GetStopBase(),
					Price(m_level1Bid),
					Price(m_level1Ask),
					Price(m_lastPrint),
					orderSize,
					orderSize,//displaySize,
					0,//venueOrderParams.GetReserveSize(),
					RI_PROACTIVE,//routingId,
					0,//routingSubType,
					TD_GetDefaultProActiveRoutingNumericName(),//*(unsigned int*)"ProA",//venueOrderParams.GetRoutingName(),
					0,//venueOrderParams.GetMnemonic(),
					false,//iso,
					PEG_NONE,//pegType,
					SignedPrice::oneCent,//pegOffset,
					TIF_IOC,//tifType,
					0,//tifMillisecond,
					WOP_CANCEL,//TD_GetWashOrderPolicy();
					OOP_RESIZE,//TD_GetOversellSplitPolicy()
					!preBorrow,//TD_IsResizeShortToBorrowed(),
					preBorrow,
					preBorrowPrice,
					false,//noRedirect,
					close,//false,//closeOnly,
					close,//false,//resizeToClose,
					imbalanceOnly,
//					true,//proAts
					!close,//true,//blockAggressiveOrders,
					RLP_ROUND_DOWN,//TD_GetRoundLotPolicy(),
					nullptr,//iso ? &isoAttachment : nullptr,
					0,//venueOrderParams.GetUserId());
					0,//parentId
					0);//ocoId

				if(errorCode != OE_OK)
				{
					m_owner->LogOrderError(errorCode, "Send Order");
				}
				return errorCode;
			}
			return OE_WRONG_ACCOUNT;
		}
		return OE_NO_CURRENT_ACCOUNT;
	}
	return OE_SIZE_ZERO;
}

#endif

bool StockMarketSorter::UpdateLevel1Bid()
{
	if(m_security)// && m_security->LockInquiryLevel1())
	{
//We don't have to LockModification because only one 4 byte (atomic) value is modified
		const unsigned int level1Bid = m_security->GetL1Bid();
//		m_security->UnlockInquiryLevel1();
		if(level1Bid != m_level1Bid)
		{
			m_level1Bid = level1Bid;
			if(UpdateLevel1Spread())
			{
				m_owner->InvalidateItemColumn(m_ordinal, MSCI_LEVEL1_SPREAD);
			}
			return true;
		}
	}
	return false;
}

bool StockMarketSorter::UpdateLevel1Ask()
{
	if(m_security)// && m_security->LockInquiryLevel1())
	{
//We don't have to LockModification because only one 4 byte (atomic) value is modified
		const unsigned int level1Ask = m_security->GetL1Ask();
//		m_security->UnlockInquiryLevel1();
		if(level1Ask != m_level1Ask)
		{
			m_level1Ask = level1Ask;
			if(UpdateLevel1Spread())
			{
				m_owner->InvalidateItemColumn(m_ordinal, MSCI_LEVEL1_SPREAD);
			}
			return true;
		}
	}
	return false;
}

bool StockMarketSorter::UpdateYesterdaysClosePrice()//InThread
{
	if(m_security)
	{
//We don't have to LockModification because only one 4 byte (atomic) value is modified
		const unsigned int& yestClose = m_security->GetClosePriceCompact();

		if(yestClose != m_yestClose)
		{
			m_yestClose = yestClose;
			return true;
		}
	}
	return false;
}

bool StockMarketSorter::UpdatePeOpenPrice()//InThread
{
	if(m_security)
	{
//We don't have to LockModification because only one 4 byte (atomic) value is modified
		const unsigned int& peOpen = m_security->GetPrimaryQOpenPriceCompact();

		if(peOpen != m_peOpen)
		{
			m_peOpen = peOpen;
			return true;
		}
	}
	return false;
}

bool StockMarketSorter::UpdatePeTodaysClosePrice()//InThread
{
	if(m_security)
	{
//We don't have to LockModification because only one 4 byte (atomic) value is modified
		const unsigned int& peTodaysClose = m_security->GetTodaysClosePriceCompact();

		if(peTodaysClose != m_peTodaysClose)
		{
			m_peTodaysClose = peTodaysClose;
			return true;
		}
	}
	return false;
}

void StockMarketSorter::CheckSaleConditionUpdatePrintAndInvalidate(const char marketCenterId, const char saleCondition)
{
//We can just call UpdatePeOpenPriceAndInvalidate() and UpdatePeTodaysClosePriceAndInvalidate();
//Because if the Open or Close Price changed in Takion, we will get it inside UpdatePeOpenPriceAndInvalidate() and UpdatePeTodaysClosePriceAndInvalidate and display it
//The code below if there just in case you want to add functionality to keep the Open Prices and Close Prices from other exchanges but the primary exchange
//	if(m_nasdaq)//UTDF
//	{
	switch(saleCondition)
	{
		case 'O'://Open Price. Level1 values are changed
		if(marketCenterId == m_primaryExchange)//PrimaryExchange Print
		{
			UpdatePeOpenPriceAndInvalidate();
		}
		break;

		case 'Q'://Open Price. No Level1 values are changed
		if(marketCenterId == m_primaryExchange)//PrimaryExchange Print
		{
			UpdatePeOpenPriceAndInvalidate();
			return;//No need to change other Level1 values
		}
		break;

		case '6'://Todays Close Price. Level1 values are changed
		if(marketCenterId == m_primaryExchange)//PrimaryExchange Print
		{
			UpdatePeTodaysClosePriceAndInvalidate();
		}
		break;

		case 'M'://Todays Close Price. No Level1 values are changed
		if(marketCenterId == m_primaryExchange)//PrimaryExchange Print
		{
			UpdatePeTodaysClosePriceAndInvalidate();
			return;//No need to change other Level1 values
		}
		break;
	}
/*
	}
	else//CTS - the same as UTDF
	{
	}
*/
/////////////////////
	UpdatePrintAndInvalidate();
}

void StockMarketSorter::CheckSaleConditionUpdatePrintAndInvalidate(const char marketCenterId, const char saleCondition2, const char saleCondition4)
{
	bool specialPrice = false;
	switch(saleCondition2)
	{
		case 'O'://Open Price. Level1 values are changed
		if(marketCenterId == m_primaryExchange)//PrimaryExchange Print
		{
			UpdatePeOpenPriceAndInvalidate();
		}
		specialPrice = true;
		break;

		case '6'://Todays Close Price. Level1 values are changed
		if(marketCenterId == m_primaryExchange)//PrimaryExchange Print
		{
			UpdatePeTodaysClosePriceAndInvalidate();
		}
		specialPrice = true;
		break;
	}
	if(!specialPrice)
	{
		switch(saleCondition4)
		{
			case 'Q'://Open Price. No Level1 values are changed
			if(marketCenterId == m_primaryExchange)//PrimaryExchange Print
			{
				UpdatePeOpenPriceAndInvalidate();
				return;//No need to change other Level1 values
			}
			break;

			case 'M'://Todays Close Price. No Level1 values are changed
			if(marketCenterId == m_primaryExchange)//PrimaryExchange Print
			{
				UpdatePeTodaysClosePriceAndInvalidate();
				return;//No need to change other Level1 values
			}
			break;
		}
	}
	UpdatePrintAndInvalidate();
}

void StockMarketSorter::UpdateAggregatedOpeningAndClosingPrice(const unsigned char flags)
{
	if(flags & APF_OpeningPrice)
	{
		UpdatePeOpenPriceAndInvalidate();
	}
	else if(flags & APF_ClosingPrice)
	{
		UpdatePeTodaysClosePriceAndInvalidate();
	}
	UpdatePrintAndInvalidate();
}

bool StockMarketSorter::UpdatePrint()//InThread
{
	if(m_security)
	{
//		const Price lastTrade = Price(m_security->GetLastNbboPrintPrice());//Last Trade
//		const Price lastTrade = Price(m_security->GetLastPrice());//Last Trade
		const unsigned int lastTrade = m_security->GetLastPrice();//Last Trade
//		const Price high = m_security->GetMarketHoursHighPrice();
//		const Price low = m_security->GetMarketHoursLowPrice();
		const Price high = m_security->GetHighPrice();
		const Price low = m_security->GetLowPrice();
/*
		const Price peOpen = Price(m_security->GetPrimaryQOpenPriceCompact());
		const Price peTodaysClose = Price(m_security->GetTodaysClosePriceCompact());
		const Price yestClose = Price(m_security->GetClosePriceCompact());
*/	
		const unsigned int lastSize = m_security->GetLastTradeSize();
		const unsigned __int64 volume = m_security->GetVolume();

		bool changed = false;

		bool lastPrintChanged = m_lastPrint != lastTrade;
		bool sizeChanged = m_lastSize != lastSize;
		bool volumeChanged = m_volume != volume;
		bool lowChanged = m_low != low;
		bool highChanged = m_high != high;

//We don't have to lock m_security, because we are in the worker thread, but we need to lock the values that we are updating, so that they are not painted in the main thread while updating
		bool unlock = LockPaintStockModification();
		if(lastPrintChanged)
		{
			m_lastPrint = lastTrade;
			changed = true;
		}

		if(sizeChanged)
		{
			m_lastSize = lastSize;
			changed = true;
		}

		if(volumeChanged)
		{
			m_volume = volume;
			changed = true;
		}

		if(lowChanged)
		{
			m_low = low;
			changed = true;
		}
		if(highChanged)
		{
			m_high = high;
			changed = true;
		}
		if(unlock)UnlockPaintStockModification();
/*
		bool peOpenChanged = m_peOpen != peOpen;
		if(peOpenChanged)
		{
			m_peOpen = peOpen;
			changed = true;
		}

		bool peTodaysCloseChanged = m_peTodaysClose != peTodaysClose;
		if(peTodaysCloseChanged)
		{
			m_peTodaysClose = peTodaysClose;
			changed = true;
		}

		bool peYestCloseChanged = m_yestClose != yestClose;
		if(peYestCloseChanged)
		{
			m_yestClose = yestClose;
			changed = true;
		}
*/
#ifdef SAMPLE_MARKET_SORTER_TIMERS
		bool closeSent = false;
		if(changed)
		{
			if(!m_positionSize && !m_buyPendingShares && !m_sellPendingShares)
			{
				StateMap::const_iterator tenAMIterator=m_stateMap.begin();//find(36000000+(3600000*2));//12PM
				if(tenAMIterator != m_stateMap.end() )
				{
					const StockState& tenAM=tenAMIterator->second;

					unsigned int orderSize = 0;
					char side = '\0';
					Price lastPrint(m_lastPrint);
					if(lastPrint>tenAM.GetHigh())
					{
						// $30,000/lastPrint and round lot down  if 0 make 100 shares
						orderSize = CalculateOrderSize();
						side = 'B';
					}
					else if(lastPrint<tenAM.GetLow())
					{
						// $30,000/lastPrint and round lot down if 0 make 100 shares
						orderSize = CalculateOrderSize();
						side = 'S';
					}
					if(orderSize)
					{
						closeSent = true;
						SendMarketOrder(side, orderSize, false);
					}
				}
			}
		}
		if(m_positionSize && lastPrintChanged && !closeSent)
		{
			static const Price away(0, Price::divider / 10);//10 cents;//away(0, 100000000);
			ClosePositionPercent(10, away);
		}
#endif
		return changed;
	}
	return false;
}

bool StockMarketSorter::UpdateValuesInMainThread(bool lockWait)
{
	if(m_security)
	{
		if(lockWait)
		{
			m_security->LockInquiryWaitLevel1();
		}
		else if(!m_security->LockInquiryLevel1())
		{
			return false;
		}
		if(m_initialized)
		{
			const unsigned int level1Bid = m_security->GetL1Bid();
			const unsigned int level1Ask = m_security->GetL1Ask();

			const unsigned int lastTrade = m_security->GetLastPrice();//Last Trade
			const Price high = m_security->GetHighPrice();
			const Price low = m_security->GetLowPrice();
	
			const unsigned int lastSize = m_security->GetLastTradeSize();
			const unsigned __int64 volume = m_security->GetVolume();

			m_security->UnlockInquiryLevel1();

			bool changed = false;
			const bool bidChanged = m_level1Bid != level1Bid;
	
			if(bidChanged)
			{
				m_level1Bid = level1Bid;
				changed = true;
			}

			const bool askChanged = m_level1Ask != level1Ask;
			if(askChanged)
			{
				m_level1Ask = level1Ask;
				UpdateLevel1Spread();
				changed = true;
			}
			else if(bidChanged)
			{
				UpdateLevel1Spread();
			}

			const bool lastPrintChanged = m_lastPrint != lastTrade;
			if(lastPrintChanged)
			{
				m_lastPrint = lastTrade;
				changed = true;
			}

			const bool lastSizeChanged = m_lastSize != lastSize;
			if(lastSizeChanged)
			{
				m_lastSize = lastSize;
				changed = true;
			}

			const bool volumeChanged = m_volume != volume;
			if(volumeChanged)
			{
				m_volume = volume;
				changed = true;
			}

			const bool lowChanged = m_low != low;
			if(lowChanged)
			{
				m_low = low;
				changed = true;
			}
			const bool highChanged = m_high != high;
			if(highChanged)
			{
				m_high = high;
				changed = true;
			}
#ifdef SAMPLE_MARKET_SORTER_TIMERS
			bool closeSent = false;
			if(changed)
			{
				if(!m_positionSize && !m_buyPendingShares && !m_sellPendingShares)
				{
					StateMap::const_iterator tenAMIterator=m_stateMap.begin();//find(36000000+(3600000*2));//12PM
					if(tenAMIterator != m_stateMap.end() )
					{
						const StockState& tenAM=tenAMIterator->second;

						unsigned int orderSize = 0;
						char side = '\0';
						Price lastPrint(m_lastPrint);
						if(lastPrint>tenAM.GetHigh())
						{
							// $30,000/lastPrint and round lot down  if 0 make 100 shares
							orderSize = CalculateOrderSize();
							side = 'B';
						}
						else if(lastPrint<tenAM.GetLow())
						{
							// $30,000/lastPrint and round lot down if 0 make 100 shares
							orderSize = CalculateOrderSize();
							side = 'S';
						}
						if(orderSize)
						{
							closeSent = true;
							SendMarketOrder(side, orderSize, false);
						}
					}
				}
			}
			if(m_positionSize && lastPrintChanged && !closeSent)
			{
				static const Price away(0, Price::divider / 10);//10 cents;//away(0, 100000000);
				ClosePositionPercent(10, away);
			}
#endif
			return changed;
		}
		else
		{
			DoInitializeWithoutLocking();
			m_initialized = true;
			m_security->UnlockInquiryLevel1();
			return true;
		}
	}
	return false;
}

void StockMarketSorter::DoInitializeWithoutLocking()
{
	m_level1Bid = m_security->GetL1Bid();
	m_level1Ask = m_security->GetL1Ask();

//	m_lastPrint = Price(m_security->GetLastNbboPrintPrice());
//	m_lastPrint = Price(m_security->GetLastPrice());
	m_lastPrint = m_security->GetLastPrice();
//	m_high = m_security->GetMarketHoursHighPrice();
//	m_low = m_security->GetMarketHoursLowPrice();
	m_high = m_security->GetHighPrice();
	m_low = m_security->GetLowPrice();

	m_peOpen = m_security->GetPrimaryQOpenPriceCompact();
	m_peTodaysClose = m_security->GetTodaysClosePriceCompact();
	m_yestClose = m_security->GetClosePriceCompact();

	m_lastSize = m_security->GetLastTradeSize();
	m_volume = m_security->GetVolume();

	if(m_level1Bid && m_level1Ask)
	{
		m_level1Spread = Price(m_level1Ask) - Price(m_level1Bid);
	}
	else
	{
		m_level1Spread.SetZero();
	}
}

void StockMarketSorter::InitializeInThread()
{
	m_roundLot = m_security->GetRoundLot();
	m_marketCategory = m_security->GetMarketCategory();
	m_primaryExchange = m_security->GetPrimaryExchange();
	m_nasdaq = m_security->isNasdaq();

	const bool unlock = LockPaintStockModification();
	DoInitializeWithoutLocking();
	if(unlock)UnlockPaintStockModification();

	m_initialized = true;
}

bool StockMarketSorter::DoInitialize()
{
	if(m_security && m_security->LockInquiryLevel1())
	{
		DoInitializeWithoutLocking();
		m_security->UnlockInquiryLevel1();
		return true;
	}
	return false;
}

void StockMarketSorter::DoReset()
{
	StockBase::DoReset();
	m_level1Bid = 0;//.SetZero();
	m_level1Ask = 0;//.SetZero();
	m_lastPrint = 0;//.SetZero();
	m_level1Spread.SetZero();
	m_high.SetZero();
	m_low.SetZero();
	m_lastSize = 0;
	m_volume = 0;
	m_peOpen = 0;
	m_peTodaysClose = 0;
	m_yestClose = 0;
	Repaint();
}

void StockMarketSorter::UpdatePositionTotals(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff)
{
	m_owner->UpdatePositionTotals(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
}

void ListBoxStockMarketSorter::LogCancelError(unsigned char errorCode, const char* prefix) const
{
	if(errorCode != CE_OK)
	{
		std::string errorMessage;
		TU_GetCancelErrorMessage(errorCode, errorMessage);
		std::string extensionPrefix("ExtSample MS Cancel ");
		if(prefix)
		{
			extensionPrefix += ' ';
			extensionPrefix += prefix;
		}
		TD_Log(errorMessage.c_str(), extensionPrefix.c_str());
	}
}

void ListBoxStockMarketSorter::LogOrderError(unsigned char errorCode, const char* prefix) const
{
	if(errorCode != OE_OK)
	{
		std::string errorMessage;
		TU_GetOrderErrorMessage(errorCode, errorMessage);
		std::string extensionPrefix("ExtSample MS");
		if(prefix)
		{
			extensionPrefix += ' ';
			extensionPrefix += prefix;
		}
		TD_Log(errorMessage.c_str(), extensionPrefix.c_str());
	}
}
void ListBoxStockMarketSorter::ResetTotals()
{
	m_owner->RepaintCount();
	if(!m_closedPnl.isZero())
	{
		m_closedPnl.SetZero();
		m_owner->RepaintClosedPnl();
	}
	if(!m_openLevel1Pnl.isZero())
	{
		m_openLevel1Pnl.SetZero();
		m_owner->RepaintOpenLevel1Pnl();
	}
	if(!m_openPrintPnl.isZero())
	{
		m_openPrintPnl.SetZero();
		m_owner->RepaintOpenPrintPnl();
	}
#ifdef DEBUG_LOCK
	if(m_lockInquiryCount)
	{
		m_lockInquiryCount = 0;
		m_owner->RepaintLockInquiryCount();
	}
	if(m_lockModificationCount)
	{
		m_lockModificationCount = 0;
		m_owner->RepaintLockModificationCount();
	}
#endif
}

void ListBoxStockMarketSorter::UpdatePositionTotals(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff)
{
	if(!closedPnlDiff.isZero())
	{
		m_closedPnl += closedPnlDiff;
		m_owner->RepaintClosedPnl();
	}
	if(!openPnlDiff.isZero())
	{
		m_openLevel1Pnl += openPnlDiff;
		m_owner->RepaintOpenLevel1Pnl();
	}
	if(!openPrintPnlDiff.isZero())
	{
		m_openPrintPnl += openPrintPnlDiff;
		m_owner->RepaintOpenPrintPnl();
	}
}

#ifdef DEBUG_LOCK
void ListBoxStockMarketSorter::UpdateLockInquiryCountTotal(const bool lock)
{
	if(lock)
	{
		++m_lockInquiryCount;
		m_owner->RepaintLockInquiryCount();
	}
	else if(m_lockInquiryCount)
	{
		--m_lockInquiryCount;
		m_owner->RepaintLockInquiryCount();
	}
}

void ListBoxStockMarketSorter::UpdateLockModificationCountTotal(const bool lock)
{
	if(lock)
	{
		++m_lockModificationCount;
		m_owner->RepaintLockModificationCount();
	}
	else if(m_lockModificationCount)
	{
		--m_lockModificationCount;
		m_owner->RepaintLockModificationCount();
	}
}
#endif

void ListBoxStockMarketSorter::UpdatePositionOpenPrintPnlTotal(const Money& openPrintPnlDiff)
{
	if(!openPrintPnlDiff.isZero())
	{
		m_openPrintPnl += openPrintPnlDiff;
		m_owner->RepaintOpenPrintPnl();
	}
}

void ListBoxStockMarketSorter::UpdatePositionOpenLevel1PnlTotal(const Money& openLevel1PnlDiff)
{
	if(!openLevel1PnlDiff.isZero())
	{
		m_openLevel1Pnl += openLevel1PnlDiff;
		m_owner->RepaintOpenLevel1Pnl();
	}
}

void ListBoxStockMarketSorter::UpdatePositionClosedPnlTotal(const Money& closedPnlDiff)
{
	if(!closedPnlDiff.isZero())
	{
		m_closedPnl += closedPnlDiff;
		m_owner->RepaintClosedPnl();
	}
}

#ifdef SAMPLE_MARKET_SORTER_TIMERS

void ListBoxStockMarketSorter::ResetTimerTotals()
{
	if(!m_rangeAtFirstTimer.isZero())
	{
		m_rangeAtFirstTimer.SetZero();
		m_averageRange.SetZero();
		m_rangeStockCount = 0;
		m_owner->RepaintRangeAtFirstTimer();
	}
}

//Special Timer
void ListBoxStockMarketSorter::ResetStockTimer()
{
	m_rangeAtFirstTimer.SetZero();
	m_averageRange.SetZero();
	m_owner->RepaintRangeAtFirstTimer();
	StockMarketSorter* stock;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (StockMarketSorter*)GetItemDataPtr(i);
		stock->ResetTimer();
	}
}

void ListBoxStockMarketSorter::UpdateAverageRange()
{
	Price averageRange;
	if(m_rangeStockCount)
	{
		averageRange = m_rangeAtFirstTimer / m_rangeStockCount;
	}
	if(m_averageRange != averageRange)
	{
		m_averageRange = averageRange;
//		Repaint(m_owner->GetRangeLeft(), m_owner->GetRangeRight());
		m_owner->RepaintRangeAtFirstTimer();
	}
}

void ListBoxStockMarketSorter::UpdateTimerTotals(const SignedPrice& rangeDiff)
{
	if(!rangeDiff.isZero())
	{
		if(rangeDiff.isPositive())
		{
			m_rangeAtFirstTimer += Price(rangeDiff);
		}
		else
		{
			m_rangeAtFirstTimer -= Price(-rangeDiff);
		}
//		m_owner->RepaintRangeAtFirstTimer();
	}
}

void ListBoxStockMarketSorter::RepaintRangeAtFirstTimer() const
{
	m_owner->RepaintRangeAtFirstTimer();
}

unsigned int StockMarketSorter::CalculatePositionPercent(unsigned int percent) const
{
	unsigned int size = 0;
	if(m_positionSize)
	{
		const unsigned int absPosSize = m_positionSize > 0 ? (unsigned int)m_positionSize : (unsigned int)-m_positionSize;
		size = absPosSize * percent / 100;
		if(size)
		{
			unsigned int lotCount = size / m_roundLot;
			unsigned int s = lotCount * m_roundLot;
			if(s != size)
			{
				size = s + m_roundLot;
			}
		}
		else
		{
			size = m_roundLot;
		}
		if(size > absPosSize)
		{
			size = absPosSize;
		}
	}
	return size;
}

void StockMarketSorter::ClosePositionPercent(unsigned int percent, const Price& away)
{
	if(m_positionSize)
	{
		unsigned int currentMillisecond = TL_GetCurrentMillisecond();
		if(!m_lastCloseTimestamp || m_lastCloseTimestamp + 60000 < currentMillisecond)//close only if at least 60000 milliseconds passed since last close.
		{
			Price lastPrint(m_lastPrint);
			if(m_positionSize > 0)//long position
			{
				if(lastPrint > m_lastExecutionPrice)//m_positionPrice)//gain
				{
					if(lastPrint >= m_lastExecutionPrice + away)//m_positionPrice + away)
					{
						unsigned int size = CalculatePositionPercent(percent);
						if(m_sellPendingShares < size)
						{
							SendMarketOrder('S', size - m_sellPendingShares, true);
							m_lastCloseTimestamp = currentMillisecond;
						}
					}
				}
				else//loss
				{
	//				if(m_positionPrice >= lastPrint + away)
					if(m_lastExecutionPrice >= lastPrint + away)
					{
						unsigned int size = CalculatePositionPercent(percent);
						if(m_sellPendingShares < size)
						{
							SendMarketOrder('S', size - m_sellPendingShares, true);
							m_lastCloseTimestamp = currentMillisecond;
						}
					}
				}
			}
			else//short position
			{
				if(lastPrint > m_lastExecutionPrice)//m_positionPrice)//loss
				{
					if(lastPrint >= m_lastExecutionPrice + away)//m_positionPrice + away)
					{
						unsigned int size = CalculatePositionPercent(percent);
						if(m_buyPendingShares < size)
						{
							SendMarketOrder('B', size - m_buyPendingShares, true);
							m_lastCloseTimestamp = currentMillisecond;
						}
					}
				}
				else//gain
				{
	//				if(m_positionPrice >= lastPrint + away)
					if(m_lastExecutionPrice >= lastPrint + away)
					{
						unsigned int size = CalculatePositionPercent(percent);
						if(m_buyPendingShares < size)
						{
							SendMarketOrder('B', size - m_buyPendingShares, true);
							m_lastCloseTimestamp = currentMillisecond;
						}
					}
				}
			}
		}
	}
}

void StockMarketSorter::CancelOpeningOrders()
{
	if(m_position)
	{
		unsigned char sideFilter;
		if(m_positionSize > 0)
		{
			sideFilter = 1 << TU_ORDER_SIDE_BUY;
		}
		else if(m_positionSize < 0)
		{
			sideFilter = (1 << TU_ORDER_SIDE_SELL) | (1 << TU_ORDER_SIDE_SHORT);
		}
		else
		{
			sideFilter = (1 << TU_ORDER_SIDE_BUY) | (1 << TU_ORDER_SIDE_SELL) | (1 << TU_ORDER_SIDE_SHORT);
		}
		((Position*)m_position)->CancelOrders(sideFilter,
			TU_ORDER_CANCEL_ALL,
			TIFMASK_ALL,
			ALGORITHMMASK_ALL,
			YNM_ALL,
			YNM_ALL,
			true,
			nullptr);//uintSet);
	}
}

void StockMarketSorter::ClosePosition()
{
	if(m_positionSize)
	{
		unsigned int absSize = (unsigned int)m_positionSize;
		if(m_positionSize > 0)//long position
		{
			if(absSize > m_sellPendingShares)
			{
				SendMarketOrder('S', absSize - m_sellPendingShares, true);
			}
		}
		else//short position
		{
			if(absSize > m_buyPendingShares)
			{
				SendMarketOrder('B', absSize - m_buyPendingShares, true);
			}
		}
	}
}

#endif

void StockMarketSorter::PositionValuesChanged(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff)
{
//	Repaint(m_owner->GetPositionSizeLeft(), m_owner->GetOpenPnlPrintRight());
	RepaintColumnRange(MSCI_POS_SIZE, MSCI_OPEN_PNL_PRINT);
	UpdatePositionTotals(closedPnlDiff, openPnlDiff, openPrintPnlDiff);

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	if(m_positionSize)
	{
		static const SignedPrice stopLoss(0, SignedPrice::divider / 10);//10 cents;
		Price lastPrint(m_lastPrint);
		if(m_positionSize > 0)//long position
		{
			if(m_sellPendingShares < (unsigned int)m_positionSize)
			{
				StateMap::const_iterator tenAMIterator=m_stateMap.begin() ;//first time
				if(tenAMIterator != m_stateMap.end() )
				{
					const StockState& tenAM=tenAMIterator->second;

					if(lastPrint<tenAM.GetLow())
					{
						SendMarketOrder('S', (unsigned int)m_positionSize - m_sellPendingShares, true);
					}
				}
			}
		}
		else//short position
		{
			if(m_buyPendingShares < (unsigned int)-m_positionSize)
			{
				StateMap::const_iterator tenAMIterator=m_stateMap.begin() ;//first time
				if(tenAMIterator != m_stateMap.end() )
				{
					const StockState& tenAM=tenAMIterator->second;

					if(lastPrint>tenAM.GetHigh())
					{
						SendMarketOrder('B', (unsigned int)-m_positionSize - m_buyPendingShares, true);
					}
				}
			}
		}
	}
#endif
}

void StockMarketSorter::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
//InThread

//Level1
		case M_MS_LEVEL1_SHORT:
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
//		if(m_inThread)
		UpdateLevel1AndInvalidate(((const TMsgMsLevel1Short*)message)->m_side == 'B');
		break;

		case M_MS_LEVEL1_LONG:
//		if(m_inThread)
		UpdateLevel1AndInvalidate(((const TMsgMsLevel1Long*)message)->m_side == 'B');
		break;

		case SM_MS_LEVEL1_SHORT:
//		if(m_inThread)
		UpdateLevel1AndInvalidate(((const TSmMsgMsLevel1Short*)message)->m_side == 'B');
		break;

		case SM_MS_LEVEL1_LONG:
//		if(m_inThread)
		UpdateLevel1AndInvalidate(((const TSmMsgMsLevel1Long*)message)->m_side == 'B');
		break;

//Prints
		case M_MS_TRADE_REPORT_SHORT:
		{
//Check if there are sale conditions for Open or Close price
			const TMsgMsTradeShort* msg = (const TMsgMsTradeShort*)message;
//			Price printPrice(msg->m_price >> 14, (msg->m_price & Price::compactFractionMask) * (Price::divider / 10000));
			CheckSaleConditionUpdatePrintAndInvalidate(msg->m_marketCenterID, msg->m_saleCondition);
		}
		break;

		case M_MS_TRADE_REPORT_LONG:
		{
//Check if there are sale conditions for Open or Close price
			const TMsgMsTradeLong* msg = (const TMsgMsTradeLong*)message;
//			Price printPrice(msg->m_priceDollars, msg->m_priceFraction);
			CheckSaleConditionUpdatePrintAndInvalidate(msg->m_marketCenterID, msg->m_saleCondition[1], msg->m_saleCondition[3]);
		}
		break;

		case SM_MS_TRADE_REPORT_SHORT:
		{
//Check if there are sale conditions for Open or Close price
			const TSmMsgMsTradeShort* msg = (const TSmMsgMsTradeShort*)message;
//			Price printPrice(msg->m_price >> 14, (msg->m_price & Price::compactFractionMask) * (Price::divider / 10000));
			CheckSaleConditionUpdatePrintAndInvalidate(msg->m_marketCenterID, msg->m_saleCondition);
		}
		break;

		case SM_MS_TRADE_REPORT_LONG:
		{
//Check if there are sale conditions for Open or Close price
			const TSmMsgMsTradeLong* msg = (const TSmMsgMsTradeLong*)message;
//			Price printPrice(msg->m_priceDollars, msg->m_priceFraction);
			CheckSaleConditionUpdatePrintAndInvalidate(msg->m_marketCenterID, msg->m_saleCondition[1], msg->m_saleCondition[3]);
		}
		break;

//When you are connected to some remote Market Sorter servers, they send "aggegated prints" to you which create less traffic 
		case M_MS_AGGREGATED_PRINT_SHORT:
		UpdateAggregatedOpeningAndClosingPrice(((const TMsgMsAggregatedPrintShort*)message)->m_flags);
		break;

		case M_MS_AGGREGATED_PRINT_LONG:
		UpdateAggregatedOpeningAndClosingPrice(((const TMsgMsAggregatedPrintLong*)message)->m_flags);
		break;

		case SM_MS_AGGREGATED_PRINT_SHORT:
		UpdateAggregatedOpeningAndClosingPrice(((const TSmMsgMsAggregatedPrintShort*)message)->m_flags);
		break;

		case SM_MS_AGGREGATED_PRINT_LONG:
		UpdateAggregatedOpeningAndClosingPrice(((const TSmMsgMsAggregatedPrintLong*)message)->m_flags);
		break;
//
		case M_MS_TRADE_CORRECTION:
		UpdatePrintAndInvalidate();
		break;

		case M_MS_TRADE_CANCEL_ERROR:
		UpdatePrintAndInvalidate();
		break;

		case SM_MS_TRADE_CORRECTION:
		UpdatePrintAndInvalidate();
		break;

		case SM_MS_TRADE_CANCEL_ERROR:
		UpdatePrintAndInvalidate();
		break;

		case M_MS_EOD_SUMMARY_REPORT:
		UpdatePeTodaysClosePriceAndInvalidate();
		break;

		case SM_MS_EOD_SUMMARY_REPORT:
		UpdatePeTodaysClosePriceAndInvalidate();
		break;

//Yesterdays Close
		case M_MS_PREVIOUS_DAY_CLOSE:
		if(m_inThread)
		UpdateYesterdaysClosePriceAndInvalidate();
		break;

		case SM_MS_PREVIOUS_DAY_CLOSE:
		if(m_inThread)
		UpdateYesterdaysClosePriceAndInvalidate();
		break;
///

		case M_RESP_REFRESH_SYMBOL://In worker thread
//		if(m_inThread)
		{
			const TMsgRespRefreshSymbol* msg = (const TMsgRespRefreshSymbol*)message;
			if(msg->m_flags & REFRESH_RefreshFailed)
			{
				SetSecurity(nullptr);
			}
			else
			{
				if(m_loaded)
				{
//stock reloaded for whatever reason. Should not happen
				}
				else
				{
					m_loaded = true;
					m_owner->IncrementLoadedStocks();
				}
//				Initialize();
				InitializeInThread();
//				Repaint();//m_owner->GetLevel1BidLeft(), m_owner->GetVolumeRight());
				RepaintColumnRange(MSCI_LEVEL1_BID, MSCI_YEST_CLOSE);
			}
		}
		break;

		case SM_M_RESP_REFRESH_SYMBOL://InThread
//		if(m_inThread)
		{
			const TSmMsgRespRefreshSymbol* msg = (const TSmMsgRespRefreshSymbol*)message;
			if(msg->m_flags & REFRESH_RefreshFailed)
			{
				SetSecurity(nullptr);
			}
			else
			{
				if(m_loaded)
				{
//stock reloaded for whatever reason. Should not happen
				}
				else
				{
					m_loaded = true;
					m_owner->IncrementLoadedStocks();
				}
//				Initialize();
				InitializeInThread();
//				Repaint();//m_owner->GetLevel1BidLeft(), m_owner->GetVolumeRight());
				RepaintColumnRange(MSCI_LEVEL1_BID, MSCI_YEST_CLOSE);
			}
		}
		break;

//Position
		case TS_ACCOUNT_RECOVERY_RESP://InThread Position loaded from the database
//		if(m_positionInThread)
		{
			Money closedPnlDiff, openPnlDiff, openPrintPnlDiff;
			if(UpdatePositionValues(true, &closedPnlDiff, &openPnlDiff, &openPrintPnlDiff))
			{
				TD_WriteCustomExternalMessageToMainThread(&TMsgExtensionSamplePositionValues(closedPnlDiff, openPnlDiff, openPrintPnlDiff));
//				PositionValuesChanged(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
			}
		}
		break;
//Closed PNL is updated in case TS_ORDER_REPORT:
		case TM_POSITION_OPEN_PNL_PRINT://InThread Position's Open PNL changed (based on Print or NBBO Print)
		{
			Money openPrintPnlDiff;
			if(UpdatePositionPrintOpenPnl(true, &openPrintPnlDiff))
			{
				RepaintColumn(MSCI_OPEN_PNL_PRINT);
				TD_WriteCustomExternalMessageToMainThread(&TMsgExtensionSampleOpenPrintPnlDiff(openPrintPnlDiff));
			}
		}
		break;

		case TM_POSITION_OPEN_PNL_LEVEL1://InThread Position's Open PNL changed (based on Print or NBBO Print)
		{
			Money openLevel1PnlDiff;
			if(UpdatePositionLevel1OpenPnl(true, &openLevel1PnlDiff))
			{
				RepaintColumn(MSCI_OPEN_PNL_LEVEL1);
				TD_WriteCustomExternalMessageToMainThread(&TMsgExtensionSampleOpenLevel1PnlDiff(openLevel1PnlDiff));
			}
		}
		break;

		case TS_ORDER://InThread New Order created
		if(UpdatePositionValues(true))//Pending Shares change
		{
			RepaintColumnRange(MSCI_POS_SIZE, MSCI_OPEN_PNL_PRINT);
		}
		break;

		case TS_ORDER_ACK://InThread Order acknowledged by the server
		break;
	
		case TS_ORDER_UPDATE://InThread Order state updated (Canceled, Expired)
		break;

		case TS_ORDER_REJECT://InThread Order rejected by the server
//		if(m_positionInThread)
		if(UpdatePositionValues(true))//Pending Shares change
		{
			RepaintColumnRange(MSCI_POS_SIZE, MSCI_OPEN_PNL_PRINT);
		}
		break;

		case TS_ORDER_REPORT://InThread Order Executed
//		if(m_positionInThread)
		{
/*You can use more detaile info about the Execution
			if(info && info->m_type == TM_NEW_EXECUTION)
			{
				const TMsgNewExecution* msgNewExecution = (const TMsgNewExecution*)info;
			}
*/
			Money closedPnlDiff, openPnlDiff, openPrintPnlDiff;
			if(UpdatePositionValues(true, &closedPnlDiff, &openPnlDiff, &openPrintPnlDiff))
			{
				RepaintColumnRange(MSCI_POS_SIZE, MSCI_OPEN_PNL_PRINT);
				TD_WriteCustomExternalMessageToMainThread(&TMsgExtensionSamplePositionValues(closedPnlDiff, openPnlDiff, openPrintPnlDiff));
//				PositionValuesChanged(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
			}
		}
		break;

		case TS_ORDER_CANCEL_REQ://InThread Cancel Requested
		break;

		case TS_ORDER_REPLACE_REQ://InThread Replace Requested
		break;

		case TS_ORDER_CANCEL_ACK://InThread Cancel Acknowledged
		break;

		case TS_ORDER_KILL://InThread Order Killed
//		if(m_positionInThread)
		if(UpdatePositionValues(true))//Pending Shares change
		{
			RepaintColumnRange(MSCI_POS_SIZE, MSCI_OPEN_PNL_PRINT);
		}
		break;

		case TS_ORDER_REPLACE_ACK://InThread Replace Acknowledged
		break;

		case TS_ORDER_REPLACED://InThread Order Replaced
//		if(m_positionInThread)
		if(UpdatePositionValues(true))//Pending Shares change
		{
			RepaintColumnRange(MSCI_POS_SIZE, MSCI_OPEN_PNL_PRINT);
		}
		break;

		case TS_CANCEL_REJECT://InThread Cancel Rejected
		break;

		case TM_EQUITY_YEST_CLOSE_PRICE_CHANGED://main thread
		if(!m_inThread)
		UpdateYesterdaysClosePriceAndInvalidate();
		break;

//LULD messages come in the Worker Thread and in Main Thread
		case TM_EQUITY_CLOSED:
		case TM_EQUITY_OPENED:
		break;

		case TM_SET_EQUITY_TIER_SIZE://Main Thread
		case TM_SET_EQUITY_CUSTOM_PRICE_BASE:
		case TM_SET_EQUITY_CUSTOM_DATE:
		case TM_EQUITY_BORROW_PRICE:
		case TM_EQUITY_BETA_AND_RETAIL_INFO:
		case TM_EQUITY_BORROW_INFO:
		case TM_EQUITY_HTB:
		case M_MS_DIVIDENDS_AND_SPLITS:
		case SM_MS_DIVIDENDS_AND_SPLITS:
		case M_MS_52WEEK_30DAY_HIGH_LOW:
		case SM_MS_52WEEK_30DAY_HIGH_LOW:
		case M_MS_SHORT_SALE_INDICATORS:
		case SM_MS_SHORT_SALE_INDICATORS:
		case M_MS_LULD:
		case SM_MS_LULD:
		case M_MS_PMI:
		case SM_MS_PMI:

		case M_MS_AVERAGE_DAILY_VOLUME:
		case SM_MS_AVERAGE_DAILY_VOLUME:
		case M_MS_PREVIOUS_DAY_HIGH_LOW:
		case SM_MS_PREVIOUS_DAY_HIGH_LOW:
		case M_MS_SECURITY_TYPE:
		case SM_MS_SECURITY_TYPE:
		case M_MS_TICK_PILOT:
		case SM_MS_TICK_PILOT:

		if(m_inThread)
		{
		}
		else
		{
		}
		break;

//MainThread
		case TM_EQUITY_REFRESHED://MainThread
		if(!m_inThread)
		{
			if(m_loaded)
			{
//stock reloaded for whatever reason. Should not happen
			}
			else
			{
				m_loaded = true;
				m_owner->IncrementLoadedStocks();
			}
			Initialize();
//			Repaint();//m_owner->GetLevel1BidLeft(), m_owner->GetVolumeRight());
			RepaintColumnRange(MSCI_LEVEL1_BID, MSCI_YEST_CLOSE);
		}
		break;

		case TM_EQUITY_UPDATE://MainThread
		case TM_EQUITY_RESET://MainThread
		case TM_SORTABLE_SECURITY_ENTITLEMENTS_CHANGED:
		if(!m_inThread)
		{
			if(UpdateValuesInMainThread(false))
			{
//				Repaint(m_owner->GetLevel1BidLeft(), m_owner->GetVolumeRight());
				RepaintColumnRange(MSCI_LEVEL1_BID, MSCI_YEST_CLOSE);
			}
		}
		break;

//Position
		case TM_UPDATED_POSITION://MainThread
		if(!m_positionInThread)
		{
			Money closedPnlDiff, openPnlDiff, openPrintPnlDiff;
			if(UpdatePositionValues(false, &closedPnlDiff, &openPnlDiff, &openPrintPnlDiff))
			{
				PositionValuesChanged(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
			}
		}
		break;

		case TM_EQUITY_IMBALANCE_UPDATE:
		break;

		case TM_EQUITY_NEW_MINUTE:
		break;

		case TM_EQUITY_CHART_LOADED:
		break;

		case TM_EQUITY_NEW_TIME_FRAME:
		break;

		default:
		break;
	}
}

void StockMarketSorter::Repaint(int left, int right)
{
	m_owner->InvalidateItem(m_ordinal, left, right);
}

void ListBoxStockMarketSorter::RepaintCount() const
{
	m_owner->RepaintCount();
}

void ListBoxStockMarketSorter::RepaintClosedPnl() const
{
	m_owner->RepaintClosedPnl();
}

void ListBoxStockMarketSorter::RepaintOpenLevel1Pnl() const
{
	m_owner->RepaintOpenLevel1Pnl();
}

void ListBoxStockMarketSorter::RepaintOpenPrintPnl() const
{
	m_owner->RepaintOpenPrintPnl();
}

#ifdef DEBUG_LOCK
void ListBoxStockMarketSorter::RepaintLockInquiryCount() const
{
	m_owner->RepaintLockInquiryCount();
}

void ListBoxStockMarketSorter::RepaintLockModificationCount() const
{
	m_owner->RepaintLockModificationCount();
}
#endif

const char* ListBoxStockMarketSorter::GetItemName(const void* item) const
{
	return ((const StockMarketSorter*)item)->GetSymbol();
}

COLORREF ListBoxStockMarketSorter::GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	return ((const StockMarketSorter*)lpDrawItemStruct->itemData)->isLoaded() ? m_loadedColor : m_unloadedColor;
}

COLORREF ListBoxStockMarketSorter::GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	if(lpDrawItemStruct->itemState & ODS_SELECTED)return RGB(255, 255, 255);
	return GetSelectedBkColor(lpDrawItemStruct);
//	return m_selectable && (lpDrawItemStruct->itemState & ODS_SELECTED) != 0 ?
//		RGB(255, 255, 255)://GetSysColor(COLOR_HIGHLIGHTTEXT):
//		((const StockMarketSorter*)lpDrawItemStruct->itemData)->isLoaded() ? m_loadedColor : m_unloadedColor;
}

void ListBoxStockMarketSorter::DestroyItem(const void* item) const
{
	delete (StockMarketSorter*)item;
}

void ListBoxStockMarketSorter::DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->itemID)
	{
		case TMID_ADD:
		case TMID_REMOVE:
		case TMID_CLEAR:

//		case TMID_FIND:
//		case TMID_UPDATE:

		case TMID_PASTE:
		case TMID_PASTE_STOCKS:
		case TMID_CLIPBOARD:
		case TMID_OPEN:
		case TMID_SAVE_AS:
#ifdef DEBUG_LOCK
		case TMID_LOCK:
		case TMID_UNLOCK:
		case TMID_LAST:
		case TMID_LAST + 1:
#endif
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
		break;

		default:
		ColumnedListBox::DrawMenuItem(lpDrawItemStruct);
		break;
	}
}

bool ListBoxStockMarketSorter::MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	switch(lpMeasureItemStruct->itemID)
	{
		case TMID_ADD:
		case TMID_REMOVE:
		case TMID_CLEAR:

//		case TMID_FIND:
//		case TMID_UPDATE:

		case TMID_PASTE:
		case TMID_PASTE_STOCKS:
		case TMID_CLIPBOARD:
		case TMID_OPEN:
		case TMID_SAVE_AS:
#ifdef DEBUG_LOCK
		case TMID_LOCK:
		case TMID_UNLOCK:
		case TMID_LAST:
		case TMID_LAST + 1:
#endif
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
		lpMeasureItemStruct->itemWidth = m_menuWidth;// mainMessageLoopWnd->GetAddMenuWidth();// + lpMeasureItemStruct->itemHeight;
		break;

		default:
		return ColumnedListBox::MeasureMenuItem(lpMeasureItemStruct);
	}
	return true;
}

void ListBoxStockMarketSorter::AddMenuItems(CMenu& menu)
{
	const bool hasItems = m_itemCount > 0;//GetCount() > 0;
	const int sel = GetCurSel();
	const bool selected = sel >= 0;
	const bool clipboardAvailable = IsClipboardFormatAvailable(CF_TEXT) == TRUE;
	menu.AppendMenu(MF_OWNERDRAW|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLIPBOARD, "Copy");
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW, TMID_ADD, "Add");
	menu.AppendMenu(MF_OWNERDRAW|(clipboardAvailable ? MF_ENABLED : MF_GRAYED), TMID_PASTE, "Paste Add");
	menu.AppendMenu(MF_OWNERDRAW|(clipboardAvailable ? MF_ENABLED : MF_GRAYED), TMID_PASTE_STOCKS, "Paste Replace");
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW|(selected ? MF_ENABLED : MF_GRAYED), TMID_REMOVE, "Remove");
	menu.AppendMenu(MF_OWNERDRAW|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLEAR, "Clear");
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW, TMID_OPEN, "Open...");
	menu.AppendMenu(MF_OWNERDRAW, TMID_SAVE_AS, "Save as...");
#ifdef DEBUG_LOCK
	const StockMarketSorter* stock = selected ? (const StockMarketSorter*)GetItemDataPtr(sel) : nullptr;
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW|(!stock || stock->isInquryLocked() || m_inThread ? MF_GRAYED : MF_ENABLED), TMID_LOCK, "LockInq");
	menu.AppendMenu(MF_OWNERDRAW|(stock && stock->isInquryLocked() && !m_inThread ? MF_ENABLED : MF_GRAYED), TMID_UNLOCK, "UnlockInq");
	menu.AppendMenu(MF_OWNERDRAW|(!stock || stock->isModificationLocked() || !m_inThread ? MF_GRAYED : MF_ENABLED), TMID_LAST, "LockMod");
	menu.AppendMenu(MF_OWNERDRAW|(stock && stock->isModificationLocked() && m_inThread ? MF_ENABLED : MF_GRAYED), TMID_LAST + 1, "UnlockMod");
#endif
}

void ListBoxStockMarketSorter::CopySymbols(std::string& info) const
{
	bool first = true;
	const StockMarketSorter* item;
//	for(unsigned int i = 0, end = (unsigned int)GetCount(); i < end; ++i)
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		item = (const StockMarketSorter*)GetItemDataPtr(i);
		if(first)
		{
			first = false;
		}
		else
		{
//			info += '\r';
			info += '\n';
		}
		info += item->GetSymbol();
	}
}

void ListBoxStockMarketSorter::RefreshOrdinals()
{
	StockMarketSorter* stock;
//	for(unsigned int i = 0, end = GetCount(); i < end; ++i)
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (StockMarketSorter*)GetItemDataPtr(i);
		stock->SetOrdinal(i);
	}
}

void ListBoxStockMarketSorter::IncrementLoadedStocks()
{
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	++m_rangeStockCount;
	UpdateAverageRange();
#endif
}

void ListBoxStockMarketSorter::DecrementLoadedStocks()
{
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	if(m_rangeStockCount)
	{
		--m_rangeStockCount;
		UpdateAverageRange();
	}
#endif
}

void ListBoxStockMarketSorter::SetUpdateOnTimer(const bool updateOnTimer)
{
	if(updateOnTimer != m_updateOnTimer)
	{
		m_updateOnTimer = updateOnTimer;
		StockMarketSorter* stock;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			stock = (StockMarketSorter*)GetItemDataPtr(i);
			stock->SetUpdateOnTimer(updateOnTimer);
		}
	}
}

void ListBoxStockMarketSorter::SetInThread(const bool inThread)
{
	if(inThread != m_inThread)
	{
		m_inThread = inThread;
		StockMarketSorter* stock;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			stock = (StockMarketSorter*)GetItemDataPtr(i);
			stock->SetInThread(m_inThread);
		}
	}
}

void ListBoxStockMarketSorter::SetPositionInThread(const bool inThread)
{
	if(inThread != m_positionInThread)
	{
		m_positionInThread = inThread;
		StockMarketSorter* stock;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			stock = (StockMarketSorter*)GetItemDataPtr(i);
			stock->SetPositionInThread(m_positionInThread);
		}
	}
}

void ListBoxStockMarketSorter::PasteStocks(StringMap& stringMap, bool replace)
{
	std::string symbol;
	StringMap::const_iterator it;
	StringMap::const_iterator itend = stringMap.end();
	StockMarketSorter* stock;
	const char* cursor;
	std::string token;
#ifdef STOCK_PROPERTY
	Price stockProperty;
#endif
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	SignedPrice diff;
	bool loaded;
#endif
	SetRedraw(FALSE);

	for(unsigned int i = 0, end = m_itemCount; i < end;)
	{
		stock = (StockMarketSorter*)GetItemDataPtr(i);
		symbol = stock->GetSymbol();
		it = stringMap.find(symbol);
		if(it == itend)
		{
			if(replace)
			{
				if(m_positionInThread)stock->LockPaintPositionInquiryWait();
				UpdatePositionTotals(-stock->GetClosedPnl(), -stock->GetOpenPnlLevel1(), -stock->GetOpenPnlPrint());
				if(m_positionInThread)stock->UnlockPaintPositionInquiry();
#ifdef SAMPLE_MARKET_SORTER_TIMERS
				diff = -SignedPrice(stock->GetRangeAtFirstTimer());
				loaded = stock->isLoaded();
#endif
				DeleteString(i);
#ifdef SAMPLE_MARKET_SORTER_TIMERS
				UpdateTimerTotals(diff);
				if(loaded && m_rangeStockCount)
				{
					--m_rangeStockCount;
				}
#endif
				RepaintCount();
				--end;
			}
			else
			{
				++i;
			}
		}
		else
		{
			cursor = it->second.c_str();
			U_ReadStrToken(token, cursor, '\t');
#ifdef STOCK_PROPERTY
			token.clear();
			U_ReadStrToken(token, cursor, '\t');
			if(!token.empty())
			{
				cursor = token.c_str();
				U_StrToPrice(stockProperty, cursor, 9);
			}
			else
			{
				stockProperty.SetZero();
			}
			stock->SetStockProperty(stockProperty);
#endif
			++i;
			stringMap.erase(it);
		}
	}
	for(it = stringMap.begin(); it != itend; ++it)
	{
		const std::string& symbol = it->first;
		if(!symbol.empty())
		{
			cursor = it->second.c_str();
			U_ReadStrToken(token, cursor, '\t');
#ifdef STOCK_PROPERTY
			token.clear();
			U_ReadStrToken(token, cursor, '\t');
			if(!token.empty())
			{
				cursor = token.c_str();
				U_StrToPrice(stockProperty, cursor, 9);
			}
			else
			{
				stockProperty.SetZero();
			}
#endif

			stock = new StockMarketSorter(it->first.c_str(),
#ifdef STOCK_PROPERTY
				stockProperty,
#endif
				m_updateOnTimer,
				m_inThread,
				m_positionInThread,
				this);

#ifdef SAMPLE_MARKET_SORTER_TIMERS
			if(stock->isLoaded())
			{
				++m_rangeStockCount;
			}
#endif
			AddString((const char*)stock);
			UpdateStockAccount(stock);
		}
	}
	m_owner->EnableFindButton();
	m_owner->EnableClearButton();
	RefreshOrdinals();
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	UpdateAverageRange();
#endif
	RepaintCount();

	SetRedraw(TRUE);
	Invalidate(FALSE);
}

DialogTextEntry& ListBoxStockMarketSorter::DisplayStockEntryField()
{
	CFont* font = GetFont();//m_visual->GetFont("Arial", -14, true);
	DialogTextEntry& symbolEntryDialog = TGUI_GetTakionMainWnd()->GetSymbolEntryDialog();
	CPoint point;
//	int GetListBoxBorderSize() const{return m_listBoxBorderSize;}
	point.x = m_owner->GetLeftOffset();
	point.y = m_owner->GetTopOffset();
	m_owner->ClientToScreen(&point);
	symbolEntryDialog.Display(true, true, nullptr, this, 0, this,
		U_GetValidStockChars(), false, true, U_GetValidStockFirstChars(), false, ".",
		RGB(255, 0, 255), 0, 7, font, point.x, point.y, 1, 1);
	return symbolEntryDialog;
}

BEGIN_MESSAGE_MAP(ListBoxStockMarketSorter, ColumnedListBox)
	ON_MESSAGE(WM_USER + 999, OnStockAdd)
END_MESSAGE_MAP()

bool ListBoxStockMarketSorter::RemoveSelectedSymbol()
{
	const int sel = GetCurSel();
	if(sel >= 0)
	{
		StockMarketSorter* stock = (StockMarketSorter*)GetItemDataPtr(sel);
#ifdef DEBUG_LOCK
		if(stock->UnlockInquiry())
		{
			UpdateLockInquiryCountTotal(false);
			InvalidateItemColumn(sel, MSCI_LOCKED_INQUIRY);
		}
		if(stock->UnlockModification())
		{
			UpdateLockModificationCountTotal(false);
			InvalidateItemColumn(sel, MSCI_LOCKED_MODIFICATION);
		}
#endif
		if(m_positionInThread)stock->LockPaintPositionInquiryWait();
		UpdatePositionTotals(-stock->GetClosedPnl(), -stock->GetOpenPnlLevel1(), -stock->GetOpenPnlPrint());
		if(m_positionInThread)stock->UnlockPaintPositionInquiry();
#ifdef SAMPLE_MARKET_SORTER_TIMERS
		SignedPrice diff = -SignedPrice(stock->GetRangeAtFirstTimer());
		bool loaded = stock->isLoaded();
#endif
		DeleteString(sel);
		const unsigned int uSel = (unsigned int)sel;
		for(unsigned int i = uSel, end = m_itemCount; i < end; ++i)
		{
			stock = (StockMarketSorter*)GetItemDataPtr(i);
			stock->DecrementOrdinal();
		}
#ifdef SAMPLE_MARKET_SORTER_TIMERS
		UpdateTimerTotals(diff);
		if(loaded && m_rangeStockCount)
		{
			--m_rangeStockCount;
		}
		UpdateAverageRange();
#endif
		if(m_itemCount)
		{
			SetCurSel(uSel < m_itemCount ? uSel : m_itemCount - 1);
		}
		m_owner->EnableFindButton();
		m_owner->EnableClearButton();
		m_owner->OnSelchangeStock();
		RepaintCount();
		return true;
	}
	return false;
}

bool ListBoxStockMarketSorter::ClearSymbols()
{
#ifdef DEBUG_LOCK
	const unsigned int end = GetItemCount();
	if(end)
	{
		StockMarketSorter* stock;
		for(unsigned int i = 0; i < end; ++i)
		{
			stock = (StockMarketSorter*)GetItemDataPtr(i);
			stock->UnlockInquiry();
			stock->UnlockModification();
		}
		if(m_lockInquiryCount)
		{
			m_lockInquiryCount = 0;
			m_owner->RepaintLockInquiryCount();
		}
		if(m_lockModificationCount)
		{
			m_lockModificationCount = 0;
			m_owner->RepaintLockModificationCount();
		}
	}
#endif
	const bool ret = ClearItemsWithRedraw();
	m_owner->EnableFindButton();
	m_owner->EnableClearButton();
	m_owner->OnSelchangeStock();
	ResetTotals();
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	ResetTimerTotals();
#endif
	return ret;
}

unsigned char ListBoxStockMarketSorter::AddSecurity(const Security* security, bool updateOrdinals)
{
	unsigned char ret = 0;
	if(security)
	{
		int sel = FindItemByName(security->GetSymbol());
		if(sel < 0)
		{
			ret = 2;
			StockMarketSorter* stock = new StockMarketSorter(security,
#ifdef STOCK_PROPERTY
				Price::priceZero,
#endif
				m_updateOnTimer,
				m_inThread,
				m_positionInThread,
				this);
#ifdef SAMPLE_MARKET_SORTER_TIMERS
			if(stock->isLoaded())
			{
				++m_rangeStockCount;
			}
#endif
			sel = AddString((const char*)stock);
			if(updateOrdinals)
			{
				unsigned int i = (unsigned int)sel;
				stock->SetOrdinal(i);
				++i;
				for(unsigned int end = m_itemCount; i < end; ++i)
				{
					((StockMarketSorter*)GetItemDataPtr(i))->IncrementOrdinal();
				}
				RepaintCount();
			}
			UpdateStockAccount(stock);
			if(m_itemCount == 1)
			{
				m_owner->EnableFindButton();
				m_owner->EnableClearButton();
			}
		}
		else
		{
			ret = 1;
		}
		if(updateOrdinals)
		{
			SetCurSel(sel);
			m_owner->OnSelchangeStock();
//		::SetFocus(m_hWnd);
			m_owner->GotoDlgCtrl(this);
		}
	}
	return ret;
}

unsigned char ListBoxStockMarketSorter::AddSymbol(const std::string& textResult)
{
	unsigned char ret = 0;
	if(!textResult.empty())
	{
		int sel = FindItemByName(textResult.c_str());
		if(sel < 0)
		{
			ret = 2;
			StockMarketSorter* stock = new StockMarketSorter(textResult.c_str(),
#ifdef STOCK_PROPERTY
				Price::priceZero,
#endif
				m_updateOnTimer,
				m_inThread,
				m_positionInThread,
				this);
#ifdef SAMPLE_MARKET_SORTER_TIMERS
			if(stock->isLoaded())
			{
				++m_rangeStockCount;
			}
#endif
			sel = AddString((const char*)stock);
			unsigned int i = (unsigned int)sel;
			stock->SetOrdinal(i);
			++i;
			for(unsigned int end = m_itemCount; i < end; ++i)
			{
				((StockMarketSorter*)GetItemDataPtr(i))->IncrementOrdinal();
			}
			UpdateStockAccount(stock);
			RepaintCount();
			if(m_itemCount == 1)
			{
				m_owner->EnableFindButton();
				m_owner->EnableClearButton();
			}
		}
		else
		{
			ret = 1;
		}
		SetCurSel(sel);
		m_owner->OnSelchangeStock();
//		::SetFocus(m_hWnd);
		m_owner->GotoDlgCtrl(this);
	}
	return ret;
}

LRESULT ListBoxStockMarketSorter::OnStockAdd(WPARAM spinId, LPARAM l)
{
	const std::string& textResult = TGUI_GetTakionMainWnd()->GetSymbolEntryDialog().GetTextResult();
	AddSymbol(textResult);
	return 0;
}

BOOL ListBoxStockMarketSorter::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD loword = (DWORD)wParam;//LOWORD(wParam);
    switch(loword)
    {
		case TMID_CLIPBOARD:
		{
			std::string info;
			CopySymbols(info);
			TU_CopyStringToClipboard(info, m_hWnd);
		}
		break;

		case TMID_ADD:
		DisplayStockEntryField().ExpandForChar('A');
		break;

		case TMID_PASTE_STOCKS://replace
		{
			StringMap stringMap;
			unsigned int duplicateCount;
			unsigned int emptyCount = TU_CopyClipboardToStringMap(stringMap, m_hWnd, '\n', &duplicateCount);
			PasteStocks(stringMap, true);
		}
		break;

		case TMID_PASTE:
		{
			StringMap stringMap;
			unsigned int duplicateCount;
			unsigned int emptyCount = TU_CopyClipboardToStringMap(stringMap, m_hWnd, '\n', &duplicateCount);
			PasteStocks(stringMap, false);
		}
		break;

		case TMID_REMOVE:
		RemoveSelectedSymbol();
		break;

		case TMID_CLEAR:
		ClearSymbols();
		break;

#ifdef DEBUG_LOCK
		case TMID_LOCK:
		{
			const int sel = GetCurSel();
			if(sel >= 0)
			{
				StockMarketSorter* stock = (StockMarketSorter*)GetItemDataPtr(sel);
				if(stock->LockInquiry())
				{
					UpdateLockInquiryCountTotal(true);
					InvalidateItemColumn(sel, MSCI_LOCKED_INQUIRY);
				}
			}
		}
		break;

		case TMID_UNLOCK:
		{
			const int sel = GetCurSel();
			if(sel >= 0)
			{
				StockMarketSorter* stock = (StockMarketSorter*)GetItemDataPtr(sel);
				if(stock->UnlockInquiry())
				{
					UpdateLockInquiryCountTotal(false);
					InvalidateItemColumn(sel, MSCI_LOCKED_INQUIRY);
				}
			}
		}
		break;

		case TMID_LAST:
		{
			const int sel = GetCurSel();
			if(sel >= 0)
			{
				StockMarketSorter* stock = (StockMarketSorter*)GetItemDataPtr(sel);
				if(stock->LockModification())
				{
					UpdateLockModificationCountTotal(true);
					InvalidateItemColumn(sel, MSCI_LOCKED_MODIFICATION);
				}
			}
		}
		break;

		case TMID_LAST + 1:
		{
			const int sel = GetCurSel();
			if(sel >= 0)
			{
				StockMarketSorter* stock = (StockMarketSorter*)GetItemDataPtr(sel);
				if(stock->UnlockModification())
				{
					UpdateLockModificationCountTotal(false);
					InvalidateItemColumn(sel, MSCI_LOCKED_MODIFICATION);
				}
			}
		}
		break;
#endif
		case TMID_SAVE_AS:
		{
			char buf[MAX_PATH];
			strcpy_s(buf, sizeof(buf), theApp.GetFilePathAndName().c_str());
			char* p = strrchr(buf, '\\');
			if(p)
			{
				*++p = '*';
				*++p = '.';
				*++p = 't';
				*++p = 'x';
				*++p = 't';
				*++p = '\0';
			}
			TakionFileDialog fileDlg(m_owner->GetMainWnd(), false, "txt", buf,
				OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_EXTENSIONDIFFERENT,
				"Stock Files (*.txt)|*.txt|All Files (*.*)|*.*||", "Save Stock File", this);
			if(fileDlg.DoModal() == IDOK)
			{
				std::string pathName = fileDlg.GetPathName();
				std::string info;
				CopySymbols(info);
				U_WriteStringToFile(pathName.c_str(), info);
			}
		}
		break;

		case TMID_OPEN:
		{
			char buf[MAX_PATH];
			strcpy_s(buf, sizeof(buf), theApp.GetFilePathAndName().c_str());
			char* p = strrchr(buf, '\\');
			if(p)
			{
				*++p = '*';
				*++p = '.';
				*++p = 't';
				*++p = 'x';
				*++p = 't';
				*++p = '\0';
			}
			TakionFileDialog fileDlg(m_owner->GetMainWnd(), true, "txt", buf,
				OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
				"Stock Files (*.txt)|*.txt|All Files (*.*)|*.*||", "Load Stock File", this);
			if(fileDlg.DoModal() == IDOK)
			{
				std::string pathName = fileDlg.GetPathName();
				m_owner->DoReadStocks(true, pathName.c_str());
			}
		}
		break;

		default:
		return ColumnedListBox::OnCommand(wParam, lParam);
	}
	return TRUE;
}

void ListBoxStockMarketSorter::SetMarketSorterLoaded(bool loaded)
{
	if(m_marketSorterLoaded != loaded)
	{
		m_marketSorterLoaded = loaded;
		if(m_hWnd)
		{
			StockMarketSorter* stock;
			unsigned int i = 0, end = m_itemCount;
			for(; i < end; ++i)
			{
				stock = (StockMarketSorter*)GetItemDataPtr(i);
				stock->MarketSorterConnected(m_marketSorterConnected);
			}
			if(m_marketSorterLoaded)
			{
				m_closedPnl.SetZero();
				m_openLevel1Pnl.SetZero();
				m_openPrintPnl.SetZero();

				for(i = 0; i < end; ++i)
				{
					stock = (StockMarketSorter*)GetItemDataPtr(i);
//08/23/2014
//					stock->MarketSorterConnected(m_marketSorterConnected);
					if(m_positionInThread)stock->LockPaintPositionInquiryWait();
					m_closedPnl += stock->GetClosedPnl();
					m_openLevel1Pnl += stock->GetOpenPnlLevel1();
					m_openPrintPnl += stock->GetOpenPnlPrint();
					if(m_positionInThread)stock->UnlockPaintPositionInquiry();
				}
			}
		}
	}
}

void ListBoxStockMarketSorter::MarketSorterCleanup()
{
	StockMarketSorter* stock;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (StockMarketSorter*)GetItemDataPtr(i);
		stock->MarketSorterCleanup();
	}
}

void ListBoxStockMarketSorter::SetMarketSorterConnected(bool connected)
{
	if(m_marketSorterConnected != connected)
	{
		m_marketSorterConnected = connected;
		if(!m_marketSorterConnected)
		{
			SetMarketSorterLoaded(false);
		}
	}
}

void ListBoxStockMarketSorter::UpdateStockAccount(StockMarketSorter* stock)
{
	if(stock->SetAccount(m_currentAccount))
	{
		Money closedPnlDiff, openPnlDiff, openPrintPnlDiff;
		if(stock->UpdatePositionValues(false, &closedPnlDiff, &openPnlDiff, &openPrintPnlDiff))
		{
			stock->PositionValuesChanged(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
		}
	}
}

void ListBoxStockMarketSorter::CurrentAccountSet(Account* account)
{
	if(m_currentAccount != account)
	{
		m_currentAccount = account;
		if(m_hWnd)
		{
			StockMarketSorter* stock;
			for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
			{
				stock = (StockMarketSorter*)GetItemDataPtr(i);
				UpdateStockAccount(stock);
			}
		}
	}
}

void ListBoxStockMarketSorter::SizeChanged(bool horizontalChange, bool verticalChange)
{
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	if(verticalChange)
	{
		m_oneSecondRect.bottom = m_listBoxHeight;
	}
#endif
}

#ifdef SAMPLE_MARKET_SORTER_TIMERS

void ListBoxStockMarketSorter::TriggerOneSecond(unsigned int count)
{
	StockMarketSorter* stock;
	const OneSecondStockState* oneSecondStatePrev;
	const OneSecondStockState* oneSecondState;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (StockMarketSorter*)GetItemDataPtr(i);
		oneSecondStatePrev = stock->GetOneSecondState(m_secondBack);
		stock->TriggerOneSecond(count);
		oneSecondState = stock->GetOneSecondState(m_secondBack);
		if(oneSecondStatePrev)
		{
			if(oneSecondState)
			{
				if(oneSecondState->GetLast() != oneSecondStatePrev->GetLast())
				{
					stock->Repaint(m_oneSecondPrintLeft, m_oneSecondPrintRight);
				}
			}
			else
			{
				stock->Repaint(m_oneSecondPrintLeft, m_oneSecondPrintRight);
			}
		}
		else if(oneSecondState)
		{
			stock->Repaint(m_oneSecondPrintLeft, m_oneSecondPrintRight);
		}
	}
}

void ListBoxStockMarketSorter::Trigger(unsigned int millisecondFromMidnight)
{
	StockMarketSorter* stock;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (StockMarketSorter*)GetItemDataPtr(i);
		stock->Trigger(millisecondFromMidnight);
	}
	UpdateAverageRange();
}

#endif

LbHorizontalScroller* ListBoxStockMarketSorter::CreateHorizontalScroller()
{
	return new LbStockMarketSorterHorizontalScroller(this);
}

void ListBoxStockMarketSorter::ListBoxCreated()
{
	ColumnedListBox::ListBoxCreated();
	m_menuWidth = MeasureText("Paste Replace", GetMenuFont());
/*
	HDC hdc = ::GetDC(m_hWnd);
	if(hdc)
	{
		HGDIOBJ oldFont = nullptr;
		CFont* menuFont = GetMenuFont();
		if(menuFont)
		{
			oldFont = ::SelectObject(hdc, menuFont->m_hObject);
		}
		CRect rect(0, 0, 0, 0);
		TakionBaseWnd::CalculateTextWidth(rect, hdc, "Paste Replace", m_menuWidth);

		if(oldFont)
		{
			::SelectObject(hdc, oldFont);
		}
		::ReleaseDC(m_hWnd, hdc);
	}
*/
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	m_horizontalScroller->GetColumnBounds(MSCI_ONE_SECOND, m_oneSecondPrintLeft, m_oneSecondPrintRight);
	m_horizontalScroller->GetColumnBounds(MSCI_RANGE, m_rangeLeft, m_rangeRight);
#endif
	m_horizontalScroller->GetColumnBounds(MSCI_POS_SIZE, m_positionSizeLeft, m_positionSizeRight);
	m_horizontalScroller->GetColumnBounds(MSCI_OPEN_PNL_PRINT, m_openPnlPrintLeft, m_openPnlPrintRight);
	m_horizontalScroller->GetColumnBounds(MSCI_LEVEL1_BID, m_level1BidLeft, m_level1BidRight);
	m_horizontalScroller->GetColumnBounds(MSCI_VOLUME, m_volumeLeft, m_volumeRight);
#ifdef DEBUG_LOCK
	m_horizontalScroller->GetColumnBounds(MSCI_LOCKED_INQUIRY, m_lockInquiryLeft, m_lockInquiryRight);
	m_horizontalScroller->GetColumnBounds(MSCI_LOCKED_MODIFICATION, m_lockModificationLeft, m_lockModificationRight);
#endif

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	m_oneSecondRect.left = m_oneSecondPrintLeft;
	m_oneSecondRect.right = m_oneSecondPrintRight;
	m_oneSecondRect.bottom = m_listBoxHeight;
#endif
}

#ifdef SAMPLE_MARKET_SORTER_TIMERS
bool ListBoxStockMarketSorter::SetSecondBack(unsigned int secondBack)
{
	if(m_secondBack != secondBack)
	{
		m_secondBack = secondBack;
		::InvalidateRect(m_hWnd, &m_oneSecondRect, FALSE);
		return true;
	}
	return false;
}
#endif

void ListBoxStockMarketSorter::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
	const StockMarketSorter* stock = (const StockMarketSorter*)item;
	std::string info;
	char num[33];
	static const char y = 'Y';
	switch(columnId)
	{
		case MSCI_SYMBOL:
		DrawText(hDC, stock->GetSymbol(), -1, &r, alignment);
		break;

#ifdef DEBUG_LOCK
		case MSCI_LOCKED_INQUIRY:
		if(stock->isInquryLocked())
		DrawText(hDC, &y, 1, &r, alignment);
		break;

		case MSCI_LOCKED_MODIFICATION:
		if(stock->isModificationLocked())
		DrawText(hDC, &y, 1, &r, alignment);
		break;
#endif

		case MSCI_LEVEL1_BID:
		info.clear();
//		if(!m_inThread || stock->LockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
//		{
			U_CompactPriceToStr(info, stock->GetLevel1Bid(), 4, -1, true, '\0', true);
//			U_PriceToStr(info, Price(stock->GetLevel1Bid()), 4, -1, true, '\0', true);
//			if(m_inThread)stock->UnlockInquiry();
	
			DrawText(hDC, info.c_str(), -1, &r, alignment);
//		}
		break;

		case MSCI_LEVEL1_ASK:
		info.clear();
//		if(!m_inThread || stock->LockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
//		{
			U_CompactPriceToStr(info, stock->GetLevel1Ask(), 4, -1, true, '\0', true);
//			U_PriceToStr(info, Price(stock->GetLevel1Ask()), 4, -1, true, '\0', true);
//			if(m_inThread)stock->UnlockInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
//		}
		break;

		case MSCI_LEVEL1_SPREAD:
		if(stock->GetLevel1Bid() && stock->GetLevel1Ask())
		{
			const SignedPrice& spread = stock->GetLevel1Spread();
			info.clear();
			U_SignedPriceToStr(info, spread, 4, '\0', true, '\0', true);
			if(!spread.isPositive())
			{
				UGUI_FillSolidRect(hDC, r, spread.isZero() ? RGB(192, 192, 0) : RGB(255, 0, 0));
			}
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case MSCI_PRINT:
		info.clear();
//		if(!m_inThread || stock->LockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
//		{
			U_CompactPriceToStr(info, stock->GetLastPrint(), 4, -1, true, '\0', true);
//			U_PriceToStr(info, stock->GetLastPrint(), 4, -1, true, '\0', true);
//			if(m_inThread)stock->UnlockInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
//		}
		break;

		case MSCI_LOW:
		info.clear();
		if(!m_inThread || stock->LockPaintStockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
		{
			U_PriceToStr(info, stock->GetLow(), 4, -1, true, '\0', true);
			if(m_inThread)stock->UnlockPaintStockInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case MSCI_HIGH:
		info.clear();
		if(!m_inThread || stock->LockPaintStockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
		{
			U_PriceToStr(info, stock->GetHigh(), 4, -1, true, '\0', true);
			if(m_inThread)stock->UnlockPaintStockInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case MSCI_PE_OPEN:
		info.clear();
//		if(!m_inThread || stock->LockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
//		{
			U_CompactPriceToStr(info, stock->GetPeOpen(), 4, -1, true, '\0', true);
//			U_PriceToStr(info, Price(stock->GetPeOpen()), 4, -1, true, '\0', true);
//			if(m_inThread)stock->UnlockInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
//		}
		break;

		case MSCI_PE_TODAYS_CLOSE:
		info.clear();
//		if(!m_inThread || stock->LockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
//		{
			U_CompactPriceToStr(info, stock->GetPeTodaysClose(), 4, -1, true, '\0', true);
//			if(m_inThread)stock->UnlockInquiry();
			DrawText(hDC, info.c_str(), -1, &r, alignment);
//		}
		break;

		case MSCI_YEST_CLOSE:
		info.clear();
//		if(!m_inThread || stock->LockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
//		{
			U_CompactPriceToStr(info, stock->GetYestClose(), 4, -1, true, '\0', true);
//			if(m_inThread)stock->UnlockInquiry();
			DrawText(hDC, info.c_str(), -1, &r, alignment);
//		}
		break;

		case MSCI_LAST_SIZE:
//		if(!m_inThread || stock->LockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
//		{
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetLastSize(), num, sizeof(num)), -1, &r, alignment);
//			if(m_inThread)stock->UnlockInquiry();
//		}
		break;

		case MSCI_VOLUME:
#ifndef _WIN64//m_volume is unsigned __int64. For 32 bit applications it's not an atomic value
		if(!m_inThread || stock->LockPaintStockInquiry())//If LockInquiry returns false, it means that currently the StockMarketSorter object is being modified and it will be signaled to repaint after the modification is done
#endif
		{
			const char* ptr = U_UnsignedNumberToStrWithCommas(stock->GetVolume(), num, sizeof(num));
#ifndef _WIN64
			if(m_inThread)stock->UnlockPaintStockInquiry();
#endif
			DrawText(hDC, ptr, -1, &r, alignment);
		}
		break;

		case MSCI_POS_SIZE:
		DrawText(hDC, U_SignedNumberToStrWithCommas(stock->GetPositionSize(), num, sizeof(num)), -1, &r, alignment);
		break;

		case MSCI_BUY_PENDING:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetBuyPendingShares(), num, sizeof(num)), -1, &r, alignment);
		break;

		case MSCI_SELL_PENDING:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetSellPendingShares(), num, sizeof(num)), -1, &r, alignment);
		break;

		case MSCI_POS_TIMESTAMP:
		info.clear();
		U_AppendMillisecond(info, stock->GetPositionEntryTimestamp(), false);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;

		case MSCI_POS_PRICE:
		info.clear();
		if(!m_positionInThread || stock->LockPaintPositionInquiry())//If LockPaintPositionInquiry returns false, it means that currently the StockMarketSorter Position object is being modified and it will be signaled to repaint after the modification is done
		{
			U_PriceToStr(info, stock->GetPositionPrice(), 4, 0, true, '\0', true);
			if(m_positionInThread)stock->UnlockPaintPositionInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case MSCI_LAST_EXEC_PRICE:
		info.clear();
		if(!m_positionInThread || stock->LockPaintPositionInquiry())//If LockPaintPositionInquiry returns false, it means that currently the StockMarketSorter Position object is being modified and it will be signaled to repaint after the modification is done
		{
			U_PriceToStr(info, stock->GetLastExecutionPrice(), 4, -1, true, '\0', true);
			if(m_positionInThread)stock->UnlockPaintPositionInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case MSCI_CLOSED_PNL:
		info.clear();
		if(!m_positionInThread || stock->LockPaintPositionInquiry())//If LockPaintPositionInquiry returns false, it means that currently the StockMarketSorter Position object is being modified and it will be signaled to repaint after the modification is done
		{
			U_MoneyToStr(info, stock->GetClosedPnl(), 2);
			if(m_positionInThread)stock->UnlockPaintPositionInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case MSCI_OPEN_PNL_LEVEL1:
		info.clear();
		if(!m_positionInThread || stock->LockPaintPositionInquiry())//If LockPaintPositionInquiry returns false, it means that currently the StockMarketSorter Position object is being modified and it will be signaled to repaint after the modification is done
		{
			U_MoneyToStr(info, stock->GetOpenPnlLevel1(), 2);
			if(m_positionInThread)stock->UnlockPaintPositionInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case MSCI_OPEN_PNL_PRINT:
		info.clear();
		if(!m_positionInThread || stock->LockPaintPositionInquiry())//If LockPaintPositionInquiry returns false, it means that currently the StockMarketSorter Position object is being modified and it will be signaled to repaint after the modification is done
		{
			U_MoneyToStr(info, stock->GetOpenPnlPrint(), 2);
			if(m_positionInThread)stock->UnlockPaintPositionInquiry();

			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

#ifdef STOCK_PROPERTY
		case MSCI_STOCK_PROPERTY:
		info.clear();
		U_PriceToStr(info, stock->GetStockProperty(), 9);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;
#endif
#ifdef SAMPLE_MARKET_SORTER_TIMERS
		case MSCI_RANGE:
		info.clear();
		U_PriceToStr(info, stock->GetRangeAtFirstTimer(), 2, 0, true, '\0', false);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;

		case MSCI_ONE_SECOND:
		{
			const OneSecondStockState* oneSecondState = stock->GetOneSecondState(m_secondBack);
			if(oneSecondState)
			{
				info.clear();
				U_PriceToStr(info, oneSecondState->GetLast(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
		}
		break;
#endif
	}
}

enum SampleSettingStockMarketSorterIds
{
	listBoxStockId = 1000,

	loadOnStartCheckBoxId,
	buttonLoadAddStockId,
	buttonLoadReplaceStockId,
	buttonViewStocksId,
	buttonBrowseId,
	editPathId,

	inThreadCheckBoxId,
	positionInThreadCheckBoxId,
	sendToMmCheckBoxId,

	staticSymbolId,
	editSymbolId,
	buttonFindId,
	buttonAddId,
	buttonRemoveId,
	buttonClearId,
	addAllCheckBoxId,

	onTimerCheckBoxId,
	spinUpdateTimerId,
	buttonUpdateTimerSetId,
	buttonUpdateTimerResetId,

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	staticSecondBackId,
	spinSecondBackId,

	staticTimerId,
	spinTimerId,
	buttonAddTimerId,
	listBoxTimerId,
	buttonRemoveTimerId,
	buttonClearTimerId,
#endif
	stockMarketSorterIdsCount,
};


SampleDialogSettingStockMarketSorter::SampleDialogSettingStockMarketSorter(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, Account* account):
	BaseDialogSetting(mainWnd, parentTab, "MarketSorter"),
	m_currentAccount(account),
	m_CheckBoxLoadOnStart("Load Stocks when this tab is created", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxInThread("Process Stock Updates in the Worker Thread", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxPositionInThread("Process Position Updates in the Worker Thread", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxDblClkSendSymbolToMm("Double clicking on a Stock in the ListBox populates Takion's active MmBox", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_GroupBoxSymbol(1, 1, 2, 3),
	m_EditSymbol(U_GetValidStockChars(), false),
	m_CheckBoxAddAll("Load All Stocks", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_GroupBoxUpdateOnTimer(1, 1, 2, 3),
	m_CheckBoxUpdateOnTimer("Update All Stocks on Timer", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_SpinUpdateTimer(true, 5959999, 0),
	m_ListBoxStock(this, m_currentAccount),
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	m_SpinSecondBack(false, 80000, 0, 0),
	m_SpinTimer(false, 235959, 1, 1),
	m_oneSecondTimer(nullptr),
	m_timerLeftOffset(0),
	m_timerTopOffset(0),
	m_timersChanged(false),
#endif
	m_marketSorterConnected(false),
	m_marketSorterLoaded(false),
	m_editLeftOffset(0),
	m_editTopOffset(0),
	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),

	m_inThread(false),
	m_positionInThread(false),
	m_sendToMm(false),
	m_addAll(false),
	m_updateOnTimer(false),
	m_updateTimerMillisecond(0),
	m_updateTimerMillisecondInControl(0),
	m_allStockterator(TD_CreateEquityIterator())
{
	AddListBox(&m_ListBoxStock);

	m_GroupBoxSymbol.SetWindows(&m_StaticSymbol, nullptr, &m_ButtonClear, &m_ButtonClear);
	m_GroupBoxUpdateOnTimer.SetWindows(&m_CheckBoxUpdateOnTimer, nullptr, &m_SpinUpdateTimer, &m_ButtonUpdateTimerReset);

	AddCustomGroupBox(&m_GroupBoxSymbol);
	AddCustomGroupBox(&m_GroupBoxUpdateOnTimer);

	SetTimeSpinCells(m_SpinUpdateTimer);

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	AddListBox(&m_ListBoxTimer);
	UpdateOneSecondPrintTitle();
	m_SpinSecondBack.AddSpinCell(1, false);
	m_SpinSecondBack.AddSpinCell(100, false);
	m_SpinSecondBack.AddSpinCell(10000, true);

	m_SpinTimer.AddSpinCell(1, false);
	m_SpinTimer.AddSpinCell(100, false);
	m_SpinTimer.AddSpinCell(10000, true);
#endif

	int w = 58;
	int x = hgap;
	int y = vgap;
	AddButtonIndirect("Load on Start", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y + (editHeight - checkBoxHeight) / 2, w, checkBoxHeight, loadOnStartCheckBoxId);
	AddButtonIndirect("Load Add", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x += w + hgap, y, w, editHeight, buttonLoadAddStockId);
	AddButtonIndirect("Load Replace", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x += w + hgap, y, w, editHeight, buttonLoadReplaceStockId);
	AddButtonIndirect("View", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x += w + hgap, y, w / 2, editHeight, buttonViewStocksId);
	AddButtonIndirect("Browse", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x += w / 2 + hgap, y, w, editHeight, buttonBrowseId);
	x += w + hgap;
	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL, 0, x, y, m_width - x, editHeight, editPathId);

	x = hgap;
	y += editHeight + vgap;

	AddButtonIndirect("In Thread", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y, w, checkBoxHeight, inThreadCheckBoxId);
	AddButtonIndirect("Pos In Thread", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y += checkBoxHeight + vgap, w, checkBoxHeight, positionInThreadCheckBoxId);

	AddButtonIndirect("Dbl Clk to MM", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y += checkBoxHeight + vgap + vgap, w, checkBoxHeight, sendToMmCheckBoxId);

	AddStaticIndirect("Symbol", WS_VISIBLE|SS_NOPREFIX, 0, x, y += checkBoxHeight + 4 * vgap, w, staticHeight, staticSymbolId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL|ES_UPPERCASE, 0, x, y += staticHeight, w, editHeight, editSymbolId);
	AddButtonIndirect("Find", WS_DISABLED|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += editHeight + vgap, w, comboBoxHeight, buttonFindId);
	AddButtonIndirect("Add", WS_DISABLED|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += editHeight + vgap, w, comboBoxHeight, buttonAddId);
	AddButtonIndirect("Remove", WS_DISABLED|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += editHeight + vgap, w, comboBoxHeight, buttonRemoveId);
	AddButtonIndirect("Clear", WS_DISABLED|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += editHeight + vgap, w, comboBoxHeight, buttonClearId);

	AddButtonIndirect("All Stocks", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y += editHeight + editHeight + vgap, w, checkBoxHeight, addAllCheckBoxId);

	AddButtonIndirect("On Timer", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y += editHeight + editHeight + vgap, w, checkBoxHeight, onTimerCheckBoxId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, x, y += checkBoxHeight, w, spinHeight, spinUpdateTimerId);
	AddButtonIndirect("Set", WS_DISABLED|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += editHeight + vgap, w, comboBoxHeight, buttonUpdateTimerSetId);
	AddButtonIndirect("Reset", WS_DISABLED|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += editHeight + vgap, w, comboBoxHeight, buttonUpdateTimerResetId);

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	y += editHeight + spinHeight;// + vgap;

	AddStaticIndirect("Seconds Back", WS_VISIBLE | SS_NOPREFIX, 0, x, y, w, staticHeight, staticSecondBackId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, x, y += staticHeight, w, spinHeight, spinSecondBackId);

	y += spinHeight + spinHeight;// + vgap;

	AddStaticIndirect("Timer", WS_VISIBLE|SS_NOPREFIX, 0, x, y, w, staticHeight, staticTimerId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, x, y += staticHeight, w, spinHeight, spinTimerId);
	AddButtonIndirect("Add", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += spinHeight + vgap, w, editHeight, buttonAddTimerId);
	AddButtonIndirect("Remove", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x, y += editHeight + vgap, w, editHeight, buttonRemoveTimerId);
	AddButtonIndirect("Clear", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x, y += editHeight + vgap, w, editHeight, buttonClearTimerId);

	y += editHeight + vgap;
//	int h = m_height - y - 2 * (editHeight + vgap);
	int h = m_height - y;
	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_SORT|LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT|WS_VSCROLL, 0, x, y, w, h, listBoxTimerId);

//	AddButtonIndirect("Remove", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x, y += h + vgap, w, editHeight, buttonRemoveTimerId);
//	AddButtonIndirect("Clear", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x, y += editHeight + vgap, w, editHeight, buttonClearTimerId);
#endif
	x += w + hgap;

	y = editHeight + vgap + vgap + staticHeight;
	w = m_width - x;
	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_SORT|LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT|WS_VSCROLL, 0, x, y, w, m_height - staticHeight - y, listBoxStockId);

	SetMarketSorterConnected(TD_IsMarketSorterConnected());
	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());

}

SampleDialogSettingStockMarketSorter::~SampleDialogSettingStockMarketSorter()
{
	if(m_allStockterator)
	{
		delete m_allStockterator;
	}
}

void SampleDialogSettingStockMarketSorter::SetTimeSpinCells(TimeSpin& spin)
{
	spin.AddSpinCell(1, false, 1);
	spin.AddSpinCell(100, false, 100);
	spin.AddSpinCell(1000, false, 1000);
//	spin.AddSpinCell(10000000, true, 10000000);
//	spin.AddSpinCell(100000, true, 100000);
}


void SampleDialogSettingStockMarketSorter::CurrentAccountSet(Account* account)
{
	if(m_currentAccount != account)
	{
		m_currentAccount = account;
		m_ListBoxStock.CurrentAccountSet(m_currentAccount);
	}
}

void SampleDialogSettingStockMarketSorter::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void SampleDialogSettingStockMarketSorter::OnSymbolChange()
{
	bool empty = m_symbol.empty();
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_symbol = text;
	if(m_symbol.empty() != empty)
	{
		EnableFindButton();
		EnableAddButton();
/*
//		m_EditSymbol.Invalidate(FALSE);
		if(m_ListBoxStock.GetItemCount() > 0)
		{
			EnableControl(m_ButtonFind.m_hWnd, empty, m_ListBoxStock.m_hWnd);
		}
*/
	}
}

void SampleDialogSettingStockMarketSorter::EnableFindButton()
{
	EnableControl(m_ButtonFind.m_hWnd, !m_symbol.empty() && m_ListBoxStock.GetItemCount() > 0, m_ListBoxStock.m_hWnd);
}

void SampleDialogSettingStockMarketSorter::EnableAddButton()
{
	EnableControl(m_ButtonAdd.m_hWnd, !m_symbol.empty(), m_ListBoxStock.m_hWnd);
}

void SampleDialogSettingStockMarketSorter::EnableClearButton()
{
	EnableControl(m_ButtonClear.m_hWnd, m_ListBoxStock.GetItemCount() > 0, m_ListBoxStock.m_hWnd);
}

void SampleDialogSettingStockMarketSorter::OnFind()
{
	Find();
}

bool SampleDialogSettingStockMarketSorter::Find()
{
	if(!m_symbol.empty())
	{
		int found = m_ListBoxStock.FindItemByName(m_symbol.c_str());
		m_ListBoxStock.SetCurSel(found);
		OnSelchangeStock();
		return true;
	}
	return false;
}

void SampleDialogSettingStockMarketSorter::OnRemove()
{
	m_ListBoxStock.RemoveSelectedSymbol();
}

void SampleDialogSettingStockMarketSorter::OnClear()
{
	m_ListBoxStock.ClearSymbols();
}

void SampleDialogSettingStockMarketSorter::OnAdd()
{
	AddStock();
}

void SampleDialogSettingStockMarketSorter::AddAll()
{
//	AddStock();
	const Security* security;
	m_ListBoxStock.SetRedraw(FALSE);
	TD_LockEquityStorage();
	m_allStockterator->Reset();
	while(security = TD_GetNextEquity(m_allStockterator))
	{
		m_ListBoxStock.AddSecurity(security, false);
	}
	TD_UnlockEquityStorage();
	
	m_ListBoxStock.RefreshOrdinals();

	m_ListBoxStock.SetRedraw(TRUE);
	m_ListBoxStock.Invalidate(FALSE);

	RepaintCount();
}

unsigned char SampleDialogSettingStockMarketSorter::AddStock()//0 symbol is empty, 1 = already exists, 2 - new stock added
{
	return m_ListBoxStock.AddSymbol(m_symbol);
}

BOOL SampleDialogSettingStockMarketSorter::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)// || pMsg->message == WM_SYSKEYDOWN)
	{
		unsigned char ascii = (unsigned char)pMsg->wParam;
		HWND focus = ::GetFocus();
		if(focus == m_ListBoxStock.m_hWnd)
		{
			if(ascii >= 'A' && ascii <= 'Z')
			{
				DialogTextEntry& dlg = m_ListBoxStock.DisplayStockEntryField();//.ExpandForChar('A');
				dlg.AddChar(ascii);
				return TRUE;
			}
		}
	}

	if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		unsigned char ascii = (unsigned char)pMsg->wParam;
		switch(ascii)
		{
			case VK_RETURN:
			case VK_INSERT:
			if(AddStock())
			{
				return TRUE;
			}
			break;
		}
	}
	return BaseDialogSetting::PreTranslateMessage(pMsg);
}

HWND SampleDialogSettingStockMarketSorter::GetFirstTabControl() const
{
	return m_ListBoxStock.m_hWnd;
}
/*
void SampleDialogSettingStockMarketSorter::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
}
void BaseSampleDialogSettingSymbol::Leaving()
{
//	TD_GetNewAccountObservable().RemoveObserver(this);
//	TD_GetAdminObservable().RemoveObserver(this);
}
*/

void SampleDialogSettingStockMarketSorter::RemoveStock(Security* stock)
{
/*
	PendingStockMap::iterator found = m_pendingStockMap.find(stock);
	if(found != m_pendingStockMap.end())
	{
		delete found->second;
		m_pendingStockMap.erase(found);
	}
*/
}

void SampleDialogSettingStockMarketSorter::ClearStocks()
{
/*
	for(PendingStockMap::iterator it = m_pendingStockMap.begin(), itend = m_pendingStockMap.end(); it != itend; ++it)
	{
		delete it->second;
	}
	m_pendingStockMap.clear();
*/
}

#ifdef SAMPLE_MARKET_SORTER_TIMERS

void SampleDialogSettingStockMarketSorter::UpdateOneSecondPrintTitle()
{
	m_oneSecondPrintTitle = "1sP-";
	char num[33];
	m_oneSecondPrintTitle += U_UnsignedNumberToStrNoDecDigits(m_ListBoxStock.GetSecondBack(), num, sizeof(num));
	CRect titleRect;
	CRect totalRect;
	LbColumn* lbColumn = GetColumnRects(listBoxStockId, MSCI_ONE_SECOND, titleRect, totalRect);
	if(lbColumn)
	{
		lbColumn->SetName(m_oneSecondPrintTitle);
	}
	if(m_hWnd)
	{
		::InvalidateRect(m_hWnd, &titleRect, FALSE);
	}
}

void SampleDialogSettingStockMarketSorter::ClearTimers()
{
	unsigned int end = m_ListBoxTimer.GetItemCount();
	if(end)
	{
		StockTimer* timer;
		for(unsigned int i = 0; i < end; ++i)
		{
			timer = (StockTimer*)m_ListBoxTimer.GetItemDataPtr(i);
			TL_RemoveTimer(timer);
		}
//Special Timer
		m_ListBoxStock.ResetStockTimer();
	}
}

void SampleDialogSettingStockMarketSorter::DestroyOneSecondTimer()
{
	if(m_oneSecondTimer)
	{
		TL_RemoveTimer(m_oneSecondTimer);
		delete m_oneSecondTimer;
		m_oneSecondTimer = nullptr;
	}
}

void SampleDialogSettingStockMarketSorter::RepaintRangeAtFirstTimer() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_ONE_SECOND);
}

#endif

void SampleDialogSettingStockMarketSorter::BeforeDestroy()
{
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	DestroyOneSecondTimer();
	ClearTimers();
	m_timerLeftOffset = 0;
	m_timerTopOffset = 0;
#endif
	m_editLeftOffset = 0;
	m_editTopOffset = 0;
	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;

	m_inThread = false;
	m_positionInThread = false;
	m_sendToMm = false;
	m_addAll = false;
	m_updateOnTimer = false;
	m_updateTimerMillisecond = 0;
	m_updateTimerMillisecondInControl = 0;

	ClearStocks();
	TD_GetNewEquityObservable().RemoveObserver(this);
	TD_GetNewPositionObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
	TD_GetCustomExternalObservable().RemoveObserver(this);
	BaseDialogSetting::BeforeDestroy();
}

void SampleDialogSettingStockMarketSorter::ToolTipsEnabled(bool enable)
{
	BaseDialogSetting::ToolTipsEnabled(enable);
	m_ListBoxStock.EnableTooltips(enable, m_toolTip);
	m_CheckBoxLoadOnStart.EnableTooltips(enable, m_toolTip);
	m_CheckBoxInThread.EnableTooltips(enable, m_toolTip);
	m_CheckBoxPositionInThread.EnableTooltips(enable, m_toolTip);
	m_CheckBoxDblClkSendSymbolToMm.EnableTooltips(enable, m_toolTip);
	m_EditSymbol.EnableTooltips(enable, m_toolTip);
	m_CheckBoxAddAll.EnableTooltips(enable, m_toolTip);
	m_CheckBoxUpdateOnTimer.EnableTooltips(enable, m_toolTip);
	m_SpinUpdateTimer.EnableTooltips(enable, m_toolTip);
}

BOOL SampleDialogSettingStockMarketSorter::OnInitDialog()
{
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	m_oneSecondTimer = new OneSecondTimer(1000, &m_ListBoxStock);
	TL_AddTimer(m_oneSecondTimer);
#endif
	TD_GetNewEquityObservable().AddObserver(this);//to get notified about new Securities created
	TD_GetNewPositionObservable().AddObserver(this);//to get notified about new Positions created
	TD_GetAdminObservable().AddObserver(this);//to get notified about connect / disconnect
	TD_GetCustomExternalObservable().AddObserver(this);//to get notified about total values changed

	m_inThread = false;
	m_positionInThread = false;
	m_sendToMm = false;
	m_addAll = false;
	m_updateOnTimer = false;
	m_updateTimerMillisecond = 0;
	m_updateTimerMillisecondInControl = 0;

	m_CheckBoxLoadOnStart.SetInvalid(true);
//	m_CheckBoxInThread.SetInvalid(true);
//	m_CheckBoxPositionInThread.SetInvalid(true);

	BOOL ret = BaseDialogSetting::OnInitDialog();

//	m_EditSymbol.SetLimitText(7);

	m_CheckBoxLoadOnStart.ResizeToFitText(0);
	m_CheckBoxInThread.ResizeToFitText(0);
	m_CheckBoxPositionInThread.ResizeToFitText(0);
	m_CheckBoxDblClkSendSymbolToMm.ResizeToFitText(0);
	m_CheckBoxAddAll.ResizeToFitText(0);
	m_CheckBoxUpdateOnTimer.ResizeToFitText(0);

	SetToolTipsEnabled(true);
//	m_ListBoxStock.EnableTooltips(true, m_toolTip);

	AddTitle(&m_ListBoxStock);

//	RestoreLayout();
	m_GroupBoxSymbol.Adjust();
	m_GroupBoxUpdateOnTimer.Adjust();

	return ret;
}

void SampleDialogSettingStockMarketSorter::RestoreLayout()
{
	const bool inThread = extensionConfig->m_msInThread.GetValue();
	m_CheckBoxInThread.SetCheck(inThread ? BST_CHECKED : BST_UNCHECKED);
	SetInThread(inThread);

	const bool positionInThread = extensionConfig->m_msPositionInThread.GetValue();
	m_CheckBoxPositionInThread.SetCheck(positionInThread ? BST_CHECKED : BST_UNCHECKED);
	SetPositionInThread(positionInThread);

	const bool sendToMm = extensionConfig->m_msSendToMm.GetValue();
	m_CheckBoxDblClkSendSymbolToMm.SetCheck(sendToMm ? BST_CHECKED : BST_UNCHECKED);
	SetSendToMm(sendToMm);

	const bool addAll = extensionConfig->m_msAddAll.GetValue();
	m_CheckBoxAddAll.SetCheck(addAll ? BST_CHECKED : BST_UNCHECKED);
	SetAddAll(addAll);

	const bool updateOnTimer = extensionConfig->m_msUpdateOnTimer.GetValue();
	m_CheckBoxUpdateOnTimer.SetCheck(updateOnTimer ? BST_CHECKED : BST_UNCHECKED);
	SetUpdateOnTimer(updateOnTimer);

	m_updateTimerMillisecondInControl = extensionConfig->m_msUpdateOnTimerMillisecond.GetValue();
	m_SpinUpdateTimer.SetMillisecond(m_updateTimerMillisecondInControl, false);
	UpdateTimerInControl();
	DoUpdateTimerSet();

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	unsigned int secondBack = extensionConfig->m_msSecondBack.GetValue();
	m_ListBoxStock.SetSecondBack(secondBack);
	bool initializing = m_SpinSecondBack.isInitializing();
	if(!initializing)
	{
		m_SpinSecondBack.EnableNotification(false);
	}
	m_SpinSecondBack.SetSecond(secondBack);
	if(!initializing)
	{
		m_SpinSecondBack.EnableNotification(true);
	}
	UpdateOneSecondPrintTitle();
#endif
	CString oldPath;
	m_EditPath.GetWindowText(oldPath);
//	bool oldLoadOnStart = m_CheckBoxLoadOnStart.GetCheck() == BST_CHECKED;
	const bool loadOnStart = extensionConfig->m_msLoadOnStart.GetValue();
	m_CheckBoxLoadOnStart.SetCheck(loadOnStart ? BST_CHECKED : BST_UNCHECKED);
	const std::string& path = extensionConfig->m_msStockFilePath.GetValue();
	if(strcmp(oldPath, path.c_str()))
//	if(!path.empty())
	{
		m_EditPath.SetWindowText(path.c_str());
		OnPathChange();
		if(loadOnStart && !path.empty())
		{
			DoReadStocks(true, path.c_str());
		}
	}

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	ClearTimers();
	m_ListBoxTimer.ResetContent();
	StockTimer* timer;
	for(ExtensionDerivedConfig::TimerMap::ConstIterator it = extensionConfig->m_msTimerMap.GetConstBegin(), itend = extensionConfig->m_msTimerMap.GetConstEnd(); it != itend; ++it)
	{
		timer = new StockTimer(it->first.GetValue(), &m_ListBoxStock, &m_ListBoxTimer);
		m_ListBoxTimer.AddString((const char*)timer);
		TL_AddTimer(timer);
	}
	m_ListBoxTimer.RefreshOrdinals();
	EnableControl(m_ButtonClearTimer.m_hWnd, m_ListBoxTimer.GetItemCount() > 0, m_ListBoxTimer.m_hWnd);

	m_SpinTimer.SetSecond(TL_GetCurrentMillisecond() / 1000);
	m_timersChanged = false;
#endif

	m_modified = false;

}

void SampleDialogSettingStockMarketSorter::UpdateSettings()
{
	extensionConfig->m_msLoadOnStart.SetValue(m_CheckBoxLoadOnStart.GetCheck() == BST_CHECKED);
	CString path;
	m_EditPath.GetWindowText(path);
	extensionConfig->m_msStockFilePath.SetValue(std::string(path));

	extensionConfig->m_msInThread.SetValue(m_inThread);// m_CheckBoxInThread.GetCheck() == BST_CHECKED);
	extensionConfig->m_msPositionInThread.SetValue(m_positionInThread);// m_CheckBoxPositionInThread.GetCheck() == BST_CHECKED);
	extensionConfig->m_msSendToMm.SetValue(m_sendToMm);// m_CheckBoxDblClkSendSymbolToMm.GetCheck() == BST_CHECKED);
	extensionConfig->m_msAddAll.SetValue(m_addAll);// m_CheckBoxAddAll.GetCheck() == BST_CHECKED);
	extensionConfig->m_msUpdateOnTimer.SetValue(m_updateOnTimer);// m_CheckBoxUpdateOnTimer.GetCheck() == BST_CHECKED);
	extensionConfig->m_msUpdateOnTimerMillisecond.SetValue(m_updateTimerMillisecondInControl);

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	extensionConfig->m_msSecondBack.SetValue(m_ListBoxStock.GetSecondBack());

	if(m_timersChanged)
	{
		m_timersChanged = false;
		extensionConfig->m_msTimerMap.Clear();
		const StockTimer* timer;
		for(unsigned int i = 0, end = m_ListBoxTimer.GetItemCount(); i < end; ++i)
		{
			timer = (const StockTimer*)m_ListBoxTimer.GetItemDataPtr(i);
			extensionConfig->m_msTimerMap.AddValue(USecondSetting(timer->GetMillisecondFromMidnight()), UIntSetting(0));
		}
	}
#endif
}

void SampleDialogSettingStockMarketSorter::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
{
	titleRect.bottom = m_topOffset;
	titleRect.top = titleRect.bottom - m_bottomOffset;

	totalRect.bottom = m_clientRect.bottom;
	totalRect.top = totalRect.bottom - m_bottomOffset;

	const LbHorizontalScroller* scroller = listBox->GetHorizontalScroller();
	if(scroller)
	{
		titleRect.left = totalRect.left = scroller->GetColumnLeftOffset() + m_titleOffset;
		titleRect.right = totalRect.right = m_clientRect.right;
	}
}

const char* SampleDialogSettingStockMarketSorter::GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const
{
	std::string info;
	switch(columnId)
	{
		case MSCI_SYMBOL:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxStock.GetItemCount(), buf, buflen);

#ifdef DEBUG_LOCK
		case MSCI_LOCKED_INQUIRY:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxStock.GetLockInquiryCount(), buf, buflen);

		case MSCI_LOCKED_MODIFICATION:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxStock.GetLockModificationCount(), buf, buflen);
#endif

		case MSCI_CLOSED_PNL:
		U_MoneyToStr(info, m_ListBoxStock.GetClosedPnl(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		case MSCI_OPEN_PNL_LEVEL1:
		U_MoneyToStr(info, m_ListBoxStock.GetOpenLevel1Pnl(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		case MSCI_OPEN_PNL_PRINT:
		U_MoneyToStr(info, m_ListBoxStock.GetOpenPrintPnl(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

#ifdef SAMPLE_MARKET_SORTER_TIMERS
		case MSCI_ONE_SECOND:
		U_PriceToStr(info, m_ListBoxStock.GetAverageRange(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;
#endif
		default:
		return nullptr;
	}
}

void SampleDialogSettingStockMarketSorter::Resize(bool widthChanged, bool heightChanged)
{
	BaseDialogSetting::Resize(widthChanged, heightChanged);
	if(m_ListBoxStock.m_hWnd)
	{
		if(!m_bottomOffset)
		{
			CRect rect;
			m_ListBoxStock.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_ListBoxStock.GetClientRect(&clientRect);
			m_listBoxBorderSize = (rect.Width() - clientRect.Width()) / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;

#ifdef SAMPLE_MARKET_SORTER_TIMERS
			m_ListBoxTimer.GetWindowRect(&clientRect);
			ScreenToClient(&clientRect);
			m_timerLeftOffset = clientRect.left;
			m_timerTopOffset = clientRect.top;
#endif
			m_EditPath.GetWindowRect(&clientRect);
			ScreenToClient(&clientRect);
			m_editLeftOffset = clientRect.left;
			m_editTopOffset = clientRect.top;

			m_titleOffset = m_leftOffset + m_listBoxBorderSize;
			
		}
		int bottom = m_clientRect.bottom - m_bottomOffset;
		m_ListBoxStock.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);

#ifdef SAMPLE_MARKET_SORTER_TIMERS
		m_ListBoxTimer.SetWindowPos(nullptr, m_timerLeftOffset, m_timerTopOffset, m_leftOffset - m_timerLeftOffset - m_timerLeftOffset, m_clientRect.bottom - m_timerTopOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
#endif
		m_EditPath.SetWindowPos(nullptr, m_editLeftOffset, m_editTopOffset, m_clientRect.right - m_editLeftOffset, m_topOffset - m_editTopOffset - m_editTopOffset - m_bottomOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);

		if(heightChanged)InvalidateAllTotalValueRects(listBoxStockId);
//		if(heightChanged)UpdateTotalRects();//m_leftOffset + m_listBoxBorderSize, bottom);
	}
}

void ListBoxStockMarketSorter::HorizontalScrollingDone(int diff)
{

	m_level1BidLeft += diff;
	m_level1BidRight += diff;

	m_volumeLeft += diff;
	m_volumeRight += diff;

	m_openPnlPrintLeft += diff;
	m_openPnlPrintRight += diff;

	m_positionSizeLeft += diff;
	m_positionSizeRight += diff;

#ifdef DEBUG_LOCK
	m_lockInquiryLeft += diff;
	m_lockInquiryRight += diff;

	m_lockModificationLeft += diff;
	m_lockModificationRight += diff;
#endif

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	m_rangeLeft += diff;
	m_rangeRight += diff;

	m_oneSecondPrintLeft += diff;
	m_oneSecondPrintRight += diff;

	m_oneSecondRect.left += diff;
	m_oneSecondRect.right += diff;
#endif
}

void SampleDialogSettingStockMarketSorter::RepaintTotals() const
{
	InvalidateAllTotalValueRects(listBoxStockId);
}

void SampleDialogSettingStockMarketSorter::RepaintCount() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_SYMBOL);
}

void SampleDialogSettingStockMarketSorter::RepaintClosedPnl() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_CLOSED_PNL);
}

void SampleDialogSettingStockMarketSorter::RepaintOpenLevel1Pnl() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_OPEN_PNL_LEVEL1);
}

void SampleDialogSettingStockMarketSorter::RepaintOpenPrintPnl() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_OPEN_PNL_PRINT);
}

#ifdef DEBUG_LOCK
void SampleDialogSettingStockMarketSorter::RepaintLockInquiryCount() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_LOCKED_INQUIRY);
}

void SampleDialogSettingStockMarketSorter::RepaintLockModificationCount() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_LOCKED_MODIFICATION);
}
#endif

void SampleDialogSettingStockMarketSorter::OnInThread()
{
	SetInThread(m_CheckBoxInThread.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingStockMarketSorter::OnPositionInThread()
{
	SetPositionInThread(m_CheckBoxPositionInThread.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingStockMarketSorter::OnSendSymbolToMm()
{
	SetSendToMm(m_CheckBoxDblClkSendSymbolToMm.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingStockMarketSorter::OnAddAll()
{
	SetAddAll(m_CheckBoxAddAll.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingStockMarketSorter::OnUpdateOnTimer()
{
	SetUpdateOnTimer(m_CheckBoxUpdateOnTimer.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingStockMarketSorter::OnUpdateTimerSet()
{
	if(m_updateTimerMillisecond != m_updateTimerMillisecondInControl)
	{
		DoUpdateTimerSet();
	}
}

void SampleDialogSettingStockMarketSorter::DoUpdateTimerSet()
{
	m_updateTimerMillisecond = m_updateTimerMillisecondInControl;

	if(m_updateOnTimer && m_updateTimerMillisecond)
	SetTimer(1, m_updateTimerMillisecond, nullptr);
	else
	KillTimer(1);

	UpdateTimerInControl();
}

void SampleDialogSettingStockMarketSorter::OnUpdateTimerReset()
{
	if(m_updateTimerMillisecond != m_updateTimerMillisecondInControl)
	{
		m_SpinUpdateTimer.SetMillisecond(m_updateTimerMillisecond);
	}
}

void SampleDialogSettingStockMarketSorter::SetSendToMm(const bool sendToMm)
{
	if(sendToMm != m_sendToMm)
	{
		m_sendToMm = sendToMm;
		m_modified = true;
	}
}

void SampleDialogSettingStockMarketSorter::SetAddAll(const bool addAll)
{
	if(addAll != m_addAll)
	{
		m_addAll = addAll;
		m_modified = true;
		if(addAll)
		{
			AddAll();
		}
		m_CheckBoxLoadOnStart.SetInvalid(!m_EditPath.GetWindowTextLength() && !addAll);
	}
}

void SampleDialogSettingStockMarketSorter::SetUpdateOnTimer(const bool updateOnTimer)
{
	if(updateOnTimer != m_updateOnTimer)
	{
		m_updateOnTimer = updateOnTimer;
		m_ListBoxStock.SetUpdateOnTimer(updateOnTimer);

		if(m_updateTimerMillisecond)
		if(updateOnTimer)
		SetTimer(1, m_updateTimerMillisecond, nullptr);
		else
		KillTimer(1);

		m_modified = true;

		m_CheckBoxInThread.SetInvalid(updateOnTimer);
		m_CheckBoxPositionInThread.SetInvalid(updateOnTimer);
		UpdateTimerInControl();
	}
}

void SampleDialogSettingStockMarketSorter::OnTimer(UINT_PTR nIDEvent)
{
	if(1 == nIDEvent)
	{
		StockMarketSorter* stock;
		const unsigned int end = m_ListBoxStock.GetItemCount();
		for(unsigned int i = 0; i < end; ++i)
		{
			stock = (StockMarketSorter*)m_ListBoxStock.GetItemDataPtr(i);
			stock->UpdateValuesInMainThread(true);
		}
		::InvalidateRect(m_ListBoxStock.m_hWnd, nullptr, FALSE);
	}
}

void SampleDialogSettingStockMarketSorter::SetInThread(const bool inThread)
{
	if(inThread != m_inThread)
	{
		m_inThread = inThread;
		m_ListBoxStock.SetInThread(inThread);
		m_modified = true;
	}
}

void SampleDialogSettingStockMarketSorter::SetPositionInThread(const bool inThread)
{
	if(inThread != m_positionInThread)
	{
		m_positionInThread = inThread;
		m_ListBoxStock.SetPositionInThread(m_positionInThread);
		m_modified = true;
	}
}

void SampleDialogSettingStockMarketSorter::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
	DDX_Control(pDX, loadOnStartCheckBoxId, m_CheckBoxLoadOnStart);
	DDX_Control(pDX, buttonLoadAddStockId, m_ButtonLoadAdd);
	DDX_Control(pDX, buttonLoadReplaceStockId, m_ButtonLoadReplace);
	DDX_Control(pDX, buttonViewStocksId, m_ButtonViewStocks);
	DDX_Control(pDX, buttonBrowseId, m_ButtonBrowse);
	DDX_Control(pDX, editPathId, m_EditPath);

	DDX_Control(pDX, inThreadCheckBoxId, m_CheckBoxInThread);
	DDX_Control(pDX, positionInThreadCheckBoxId, m_CheckBoxPositionInThread);
	DDX_Control(pDX, sendToMmCheckBoxId, m_CheckBoxDblClkSendSymbolToMm);
	DDX_Control(pDX, staticSymbolId, m_StaticSymbol);
	DDX_Control(pDX, editSymbolId, m_EditSymbol);
	DDX_Control(pDX, buttonFindId, m_ButtonFind);
	DDX_Control(pDX, buttonAddId, m_ButtonAdd);
	DDX_Control(pDX, buttonRemoveId, m_ButtonRemove);
	DDX_Control(pDX, buttonClearId, m_ButtonClear);
	DDX_Control(pDX, addAllCheckBoxId, m_CheckBoxAddAll);

	DDX_Control(pDX, onTimerCheckBoxId, m_CheckBoxUpdateOnTimer);
	DDX_Control(pDX, spinUpdateTimerId, m_SpinUpdateTimer);
	DDX_Control(pDX, buttonUpdateTimerSetId, m_ButtonUpdateTimerSet);
	DDX_Control(pDX, buttonUpdateTimerResetId, m_ButtonUpdateTimerReset);

	DDX_Control(pDX, listBoxStockId, m_ListBoxStock);
#ifdef SAMPLE_MARKET_SORTER_TIMERS
	DDX_Control(pDX, spinSecondBackId, m_SpinSecondBack);
	DDX_Control(pDX, spinTimerId, m_SpinTimer);
	DDX_Control(pDX, buttonAddTimerId, m_ButtonAddTimer);
	DDX_Control(pDX, listBoxTimerId, m_ListBoxTimer);
	DDX_Control(pDX, buttonRemoveTimerId, m_ButtonRemoveTimer);
	DDX_Control(pDX, buttonClearTimerId, m_ButtonClearTimer);
#endif
}

BEGIN_MESSAGE_MAP(SampleDialogSettingStockMarketSorter, BaseDialogSetting)
//	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(loadOnStartCheckBoxId, OnModified)
	ON_BN_CLICKED(buttonLoadAddStockId, OnLoadAdd)
	ON_BN_CLICKED(buttonLoadReplaceStockId, OnLoadReplace)
	ON_BN_CLICKED(buttonViewStocksId, OnViewStocks)
	ON_BN_CLICKED(buttonBrowseId, OnBrowse)
	ON_EN_CHANGE(editPathId, OnPathChange)

	ON_BN_CLICKED(inThreadCheckBoxId, OnInThread)
	ON_BN_CLICKED(positionInThreadCheckBoxId, OnPositionInThread)
	ON_BN_CLICKED(sendToMmCheckBoxId, OnSendSymbolToMm)
	ON_BN_CLICKED(buttonFindId, OnFind)
	ON_BN_CLICKED(buttonAddId, OnAdd)
	ON_BN_CLICKED(buttonRemoveId, OnRemove)
	ON_BN_CLICKED(buttonClearId, OnClear)
	ON_BN_CLICKED(addAllCheckBoxId, OnAddAll)
	ON_EN_CHANGE(editSymbolId, OnSymbolChange)
	ON_EN_UPDATE(editSymbolId, OnSymbolUpdate)

	ON_BN_CLICKED(onTimerCheckBoxId, OnUpdateOnTimer)
	ON_BN_CLICKED(buttonUpdateTimerSetId, OnUpdateTimerSet)
	ON_BN_CLICKED(buttonUpdateTimerResetId, OnUpdateTimerReset)

	ON_LBN_SELCHANGE(listBoxStockId, OnSelchangeStock)
	ON_LBN_DBLCLK(listBoxStockId, OnDblClkStock)

#ifdef SAMPLE_MARKET_SORTER_TIMERS
	ON_BN_CLICKED(buttonAddTimerId, OnAddTimer)
	ON_BN_CLICKED(buttonRemoveTimerId, OnRemoveTimer)
	ON_BN_CLICKED(buttonClearTimerId, OnClearTimer)
	ON_LBN_SELCHANGE(listBoxTimerId, OnSelchangeTimer)
#endif
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

#ifdef SAMPLE_MARKET_SORTER_TIMERS

void SampleDialogSettingStockMarketSorter::SetSecondBack(unsigned int secondBack)
{
	if(m_ListBoxStock.SetSecondBack(secondBack))
	{
		UpdateOneSecondPrintTitle();
		m_modified = true;
	}
}
#endif

void SampleDialogSettingStockMarketSorter::UpdateTimerInControl()
{
	const bool different = m_updateTimerMillisecondInControl != m_updateTimerMillisecond;
	m_SpinUpdateTimer.SetBkColorEnabled(m_updateOnTimer ?
		m_updateTimerMillisecondInControl ? 
		different ? m_visualBase->GetPendingColor() : 0xFFFFFFFF:
		m_visualBase->GetWarningColor():
		m_visualBase->GetGrayColor());
	EnableControl(m_ButtonUpdateTimerSet.m_hWnd, different && m_updateOnTimer, m_SpinUpdateTimer.m_hWnd);
	EnableControl(m_ButtonUpdateTimerReset.m_hWnd, different, m_SpinUpdateTimer.m_hWnd);
}

LRESULT SampleDialogSettingStockMarketSorter::OnSpinChange(WPARAM spinId, LPARAM l)
{
	switch(spinId)
	{
		case spinUpdateTimerId:
		{
			const unsigned int millisecond = m_SpinUpdateTimer.GetMillisecond();
			if(millisecond != m_updateTimerMillisecondInControl)
			{
				m_updateTimerMillisecondInControl = millisecond;
				UpdateTimerInControl();
				m_modified = true;
			}
//			m_updateTimerMillisecond;
		}
		break;

#ifdef SAMPLE_MARKET_SORTER_TIMERS
		case spinSecondBackId:
		SetSecondBack(m_SpinSecondBack.GetSecond());
		break;
#endif
	}
	return 0;
}


void SampleDialogSettingStockMarketSorter::OnPathChange()
{
	m_modified = true;
	const bool enable = m_EditPath.GetWindowTextLength() != 0;
	EnableControl(m_ButtonLoadAdd.m_hWnd, enable, m_EditPath.m_hWnd);
	EnableControl(m_ButtonLoadReplace.m_hWnd, enable, m_EditPath.m_hWnd);
	EnableControl(m_ButtonViewStocks.m_hWnd, enable, m_EditPath.m_hWnd);
//	m_CheckBoxLoadOnStart.Invalidate(FALSE);
	m_CheckBoxLoadOnStart.SetInvalid(!enable && !m_addAll);
}

void SampleDialogSettingStockMarketSorter::DoReadStocks(bool replace, const char* currentPath)
{
	ListBoxStockMarketSorter::StringMap stringMap;
	unsigned int duplicateCount;
	unsigned int emptyCount;
	if(U_ReadFileToStringMap(currentPath, stringMap, '\t', ';', true, &duplicateCount, &emptyCount))
	{
		m_ListBoxStock.PasteStocks(stringMap, replace);
	}
	else
	{
		TakionMessageBox mb(m_mainWnd);
		mb.AddMessage(currentPath);
		mb.DisplayModal(this, "File does not exist", MB_OK|MB_ICONERROR);//, RGB(0, 0, 128), RGB(255, 255, 255));
	}
}

void SampleDialogSettingStockMarketSorter::ReadStocks(bool replace)
{
	CString currentPath;
	m_EditPath.GetWindowText(currentPath);
	if(!currentPath.IsEmpty())
	{
		DoReadStocks(replace, currentPath);
	}
}

void SampleDialogSettingStockMarketSorter::OnViewStocks()
{
	CString currentPath;
	m_EditPath.GetWindowText(currentPath);
	if(!currentPath.IsEmpty())
	{
		const char* error = U_ShellExecute(m_hWnd, currentPath);
		if(error)
		{
			TakionMessageBox mb(m_mainWnd);
			mb.AddMessage(currentPath);
			mb.AddMessage(error);
			mb.DisplayModal(this, "Cannot open file", MB_OK | MB_ICONERROR);//, RGB(0, 0, 128), RGB(255, 255, 255));
		}
	}
}

void SampleDialogSettingStockMarketSorter::OnLoadAdd()
{
	ReadStocks(false);
}

void SampleDialogSettingStockMarketSorter::OnLoadReplace()
{
	ReadStocks(true);
}

void SampleDialogSettingStockMarketSorter::OnBrowse()
{
	char buf[MAX_PATH];
	CString currentPath;
	m_EditPath.GetWindowText(currentPath);

//	const std::string& theApp.GetFilePathAndName()
	if(currentPath.IsEmpty())
	{
		strcpy_s(buf, sizeof(buf), theApp.GetFilePathAndName().c_str());
		char* p = strrchr(buf, '\\');
		if(p)
		{
			*++p = '*';
			*++p = '.';
			*++p = 't';
			*++p = 'x';
			*++p = 't';
			*++p = '\0';
		}
	}
	else
	{
		strcpy_s(buf, sizeof(buf), currentPath);
	}

	TakionFileDialog fileDlg(m_mainWnd, true, "txt", buf,
		OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
		"Stock Files (*.txt)|*.txt|All Files (*.*)|*.*||", "Load Stock File", this);
	if(fileDlg.DoModal() == IDOK)
	{
		std::string pathName = fileDlg.GetPathName();
		if(strcmp(pathName.c_str(), currentPath))
		{
			m_EditPath.SetWindowText(pathName.c_str());
			OnPathChange();
		}
	}
}

void SampleDialogSettingStockMarketSorter::OnSelchangeStock()
{
	EnableControl(m_ButtonRemove.m_hWnd, m_ListBoxStock.GetCurSel() >= 0, m_ListBoxStock.m_hWnd);
}

void SampleDialogSettingStockMarketSorter::OnDblClkStock()
{
	int sel = m_ListBoxStock.GetCurSel();
	if(sel >= 0)
	{
		const char* symbol = m_ListBoxStock.GetItemNameAt(sel);
		m_EditSymbol.SetWindowText(symbol);
		EnableControl(m_ButtonFind.m_hWnd, true, m_ListBoxStock.m_hWnd);

		if(m_sendToMm)
		{
			TD_SetSymbolInCommandTool(symbol, true);//, unsigned int commandToolOrdinal = 0xFFFFFFFF);
		}
	}
}

#ifdef SAMPLE_MARKET_SORTER_TIMERS

void SampleDialogSettingStockMarketSorter::OnSelchangeTimer()
{
	EnableControl(m_ButtonRemoveTimer.m_hWnd, m_ListBoxTimer.GetCurSel() >= 0, m_ListBoxTimer.m_hWnd);
}

void SampleDialogSettingStockMarketSorter::OnAddTimer()
{
	StockTimer* timer = new StockTimer(m_SpinTimer.GetSecond() * 1000, &m_ListBoxStock, &m_ListBoxTimer);
	int sel = m_ListBoxTimer.FindItem(timer);
	if(sel < 0)
	{
		if(TL_AddTimer(timer))
		{
			sel = m_ListBoxTimer.AddString((const char*)timer);
			const unsigned int count = m_ListBoxTimer.GetItemCount();
			unsigned int i = (unsigned int)sel;
			timer->SetOrdinal(i);
			for(++i; i < count; ++i)
			{
				timer = (StockTimer*)m_ListBoxTimer.GetItemDataPtr(i);
				timer->IncrementOrdinal();
			}

//			if(m_ListBoxTimer.GetItemCount() == 1)
			if(count == 1)
			{
				EnableControl(m_ButtonClearTimer.m_hWnd, true, m_ListBoxTimer.m_hWnd);
			}
			m_timersChanged = true;
			m_modified = true;
		}
		else
		{
			TakionMessageBox mb(TGUI_GetTakionMainWnd());
			mb.AddMessage("Timer is not added");
			mb.DisplayModal(this, "Timer", MB_OK|MB_ICONERROR);//, RGB(0, 0, 128), RGB(255, 255, 255));
		}
	}
//T
	else
	{
		delete timer;
	}
	if(sel >= 0)
	{
		m_ListBoxTimer.SetCurSel(sel);
		EnableControl(m_ButtonRemoveTimer.m_hWnd, true, m_ListBoxTimer.m_hWnd);
	}
}

void SampleDialogSettingStockMarketSorter::OnRemoveTimer()
{
	int sel = m_ListBoxTimer.GetCurSel();
	if(sel >= 0)
	{
		StockTimer* timer = (StockTimer*)m_ListBoxTimer.GetItemDataPtr(sel);
//Special Timer
		switch(timer->GetMillisecondFromMidnight())
		{
			case 50400000://14:00:00
			case 55800000://15:30:00
			break;

			default:
			m_ListBoxStock.ResetStockTimer();
			break;
		}

		TL_RemoveTimer(timer);
		m_ListBoxTimer.DeleteString(sel);

		const unsigned int count = m_ListBoxTimer.GetItemCount();
		for(unsigned int i = (unsigned int)sel; i < count; ++i)
		{
			timer = (StockTimer*)m_ListBoxTimer.GetItemDataPtr(i);
			timer->DecrementOrdinal();
		}

		if((unsigned int)sel < count)
		{
			m_ListBoxTimer.SetCurSel(sel);
		}
		else
		{
			if(count)
			{
				m_ListBoxTimer.SetCurSel(count - 1);
			}
			else
			{
				EnableControl(m_ButtonRemoveTimer.m_hWnd, false, m_ListBoxTimer.m_hWnd);
				EnableControl(m_ButtonClearTimer.m_hWnd, false, m_ListBoxTimer.m_hWnd);
			}
		}
		m_timersChanged = true;
		m_modified = true;
	}
}

void SampleDialogSettingStockMarketSorter::OnClearTimer()
{
	unsigned int end = m_ListBoxTimer.GetItemCount();
	if(end)
	{
		ClearTimers();
		m_ListBoxTimer.ResetContent();
		EnableControl(m_ButtonRemoveTimer.m_hWnd, false, m_ListBoxTimer.m_hWnd);
		EnableControl(m_ButtonClearTimer.m_hWnd, false, m_ListBoxTimer.m_hWnd);
		m_timersChanged = true;
		m_modified = true;
	}
}

#endif
/*
HBRUSH SampleDialogSettingStockMarketSorter::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case loadOnStartCheckBoxId:
		if(m_EditPath.GetWindowTextLength() == 0)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
			return *m_bkBrush;
		}
		break;
	}
	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/
void SampleDialogSettingStockMarketSorter::SetMarketSorterLoaded(bool loaded)
{
	if(m_marketSorterLoaded != loaded)
	{
		m_marketSorterLoaded = loaded;
		m_ListBoxStock.SetMarketSorterLoaded(m_marketSorterLoaded);
		if(m_addAll && m_marketSorterLoaded)
		{
			AddAll();
			RepaintCount();
		}
	}
}

void SampleDialogSettingStockMarketSorter::SetMarketSorterConnected(bool connected)
{
	if(m_marketSorterConnected != connected)
	{
		m_marketSorterConnected = connected;
		m_ListBoxStock.SetMarketSorterConnected(m_marketSorterConnected);

		if(!m_marketSorterConnected)
		{
			RepaintTotals();
		}
	}
}

void SampleDialogSettingStockMarketSorter::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_CUSTOM_EXTERNAL:
		switch(((const TMsgCustom*)message)->m_customId)
		{
			case POSITION_VALUES:
			{
				const TMsgExtensionSamplePositionValues* msg = (const TMsgExtensionSamplePositionValues*)message;
				m_ListBoxStock.UpdatePositionTotals(msg->m_closedPnlDiff, msg->m_openPnlDiff, msg->m_openPrintPnlDiff);
			}
			break;

			case POSITION_OPEN_PRINT_PNL_DIFF:
			{
				const TMsgExtensionSamplePnl* msg = (const TMsgExtensionSamplePnl*)message;
				m_ListBoxStock.UpdatePositionOpenPrintPnlTotal(msg->m_pnl);
			}
			break;

			case POSITION_OPEN_LEVEL1_PNL_DIFF:
			{
				const TMsgExtensionSamplePnl* msg = (const TMsgExtensionSamplePnl*)message;
				m_ListBoxStock.UpdatePositionOpenLevel1PnlTotal(msg->m_pnl);
			}
			break;

			case POSITION_CLOSED_PNL_DIFF:
			{
				const TMsgExtensionSamplePnl* msg = (const TMsgExtensionSamplePnl*)message;
				m_ListBoxStock.UpdatePositionClosedPnlTotal(msg->m_pnl);
			}
			break;
		}
		break;

		case TM_NEW_EQUITY:
		case TM_REFRESHED_EQUITY:
		{
			const Security* security = ((const TMsgSecurity*)message)->m_security;
			if(security->isLoaded())
			{
				int found = m_ListBoxStock.FindItemByName(security->GetSymbol());
				if(found >= 0)
				{
					StockMarketSorter* stock = (StockMarketSorter*)m_ListBoxStock.GetItemDataPtr(found);
					bool loaded = stock->isLoaded();
					stock->SetSecurity(security);
					stock->Repaint();//m_ListBoxStock.GetLevel1BidLeft(), m_ListBoxStock.GetVolumeRight());
					if(loaded != stock->isLoaded())
					{
						if(loaded)
						{
							m_ListBoxStock.DecrementLoadedStocks();
						}
						else
						{
							m_ListBoxStock.IncrementLoadedStocks();
						}
					}
				}
				else if(m_addAll && m_marketSorterLoaded)
				{
					m_ListBoxStock.AddSecurity(security, true);
//					m_ListBoxStock.RefreshOrdinals();
					RepaintCount();
				}
			}
		}
		break;

		case TM_NEW_POSITION:
		{
			const Position* position = ((TMsgNewPosition*)message)->m_position;
			if(m_currentAccount == position->GetAccount())
			{
				int sel = m_ListBoxStock.FindItemByName(position->GetSymbol());
				if(sel >= 0)
				{
					StockMarketSorter* stock = (StockMarketSorter*)m_ListBoxStock.GetItemDataPtr(sel);
					stock->SetPosition(position);
//					stock->Repaint(m_ListBoxStock.GetPositionSizeLeft(), m_ListBoxStock.GetOpenPnlPrintRight());
					stock->RepaintColumnRange(MSCI_POS_SIZE, MSCI_OPEN_PNL_PRINT);
//					m_ListBoxStock.InvalidateItemColumnRange(stock->GetOrdinal(), MSCI_POS_SIZE, MSCI_OPEN_PNL_PRINT);
				}
			}
		}
		break;


		case TM_CONNECTED:
		case TM_DISCONNECTED:
		SetMarketSorterConnected(TD_IsMarketSorterConnected());
		break;

		case TM_MARKET_SORTER_LOADED:
		SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
		break;

		case TM_MARKET_SORTER_CLEANUP:
		if(from == &TD_GetAdminObservable())
		{
			m_ListBoxStock.MarketSorterCleanup();
		}
		break;

		case TM_MARKET_STATUS:
		{
		}
		break;

		case TM_NEXT_DAY_STARTED:
		{
//			NextDayStarted(((const TMsgNextDayStarted*)message)->m_date);
		}
		break;

		case TM_ORDER_ERROR:
		if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
		{
//			Account* account = ((TMsgUpdatedAccountAndPosition*)info)->m_account;
//			if(account == m_account || !m_account)
//			{
				const TMsgOrderError* msg = (const TMsgOrderError*)message;
				m_ListBoxStock.LogOrderError(msg->m_errorCode, "OrderError");
		}
		break;

		case TM_CANCEL_ERROR:
		if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
		{
//			Account* account = ((TMsgUpdatedAccountAndPosition*)info)->m_account;
			const TMsgCancelError* msg = (const TMsgCancelError*)message;
			m_ListBoxStock.LogCancelError(msg->m_errorCode, "CancelError");
		}
	}
/*
	switch(message->m_type)
	{
		case TM_NEW_ORDER:
//		DisplayNewOrder(((const TMsgOrder*)message)->m_order);
		break;

//		case TM_ORDER_UPDATE:
//		break;

		case TM_DEAD_ORDER:
//		DisplayCanceledOrder(((const TMsgOrder*)message)->m_order);
		break;

		case TM_NEW_EXECUTION:
//		DisplayExecution(((const TMsgExecution*)message)->m_execution);
		break;

		case TM_ORDER_ERROR:
		if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
		{
			Account* account = ((TMsgUpdatedAccountAndPosition*)info)->m_account;
//			if(account == m_account || !m_account)
//			{
				const TMsgOrderError* msg = (const TMsgOrderError*)message;
				char side;
				switch(msg->Side)
				{
					case ORDER_SIDE_BUY://					'1'		// = Buy
					side = 'B';
					break;

					case ORDER_SIDE_SELL://					'2'		// = Sell
					side = 'S';
					break;

					case ORDER_SIDE_SELL_SHORT://			'5'		// = Sell short
					side = 'T';
					break;

					case ORDER_SIDE__BUY_MINUS://			'3'		// = Buy minus
					side = 'B';
					break;

					case ORDER_SIDE_SELL_PLUS://			'4'		// = Sell plus
					side = 'S';
					break;

					case ORDER_SIDE_SELL_SHORT_EXEMPT://	'6'		// = Sell short exempt
					side = 'T';
					break;

					case ORDER_SIDE_UNDISCLOSED://			'7'		// = Undisclosed (valid for IOI and List Order messages only)
					case ORDER_SIDE_CROSS://				'8'		// = Cross (orders where counterparty is an exchange, valid for all messages except IOIs)
					case ORDER_SIDE_CROSS_SHORT://			'9'		// = Cross short
					case ORDER_SIDE_SHORT_EXEMPT://			'A'		//= Cross short exxmpt
					case ORDER_SIDE_AS_DEFINED://			'B'		//"As Defined" (for use with multileg instruments)
					case ORDER_SIDE_OPPOSITE://				'C'		// = "Opposite" (for use with multileg instruments)
					side = msg->Side;
					break;

					default:
					side = 'U';
					break;
				}
//				DisplayOrderError(account, msg->Symbol, side, msg->m_errorCode, msg->m_moneyValue, msg->m_moneyConstraint, msg->m_uintValue, msg->m_uintConstraint);
//			}
		}
		break;

		case TM_CANCEL_ERROR:
		if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
		{
			Account* account = ((TMsgUpdatedAccountAndPosition*)info)->m_account;
			const TMsgCancelError* msg = (const TMsgCancelError*)message;
			char side;
			switch(msg->Side)
			{
				case ORDER_SIDE_BUY://					'1'		// = Buy
				side = 'B';
				break;

				case ORDER_SIDE_SELL://					'2'		// = Sell
				side = 'S';
				break;

				case ORDER_SIDE_SELL_SHORT://			'5'		// = Sell short
				side = 'T';
				break;

				case ORDER_SIDE__BUY_MINUS://			'3'		// = Buy minus
				side = 'B';
				break;

				case ORDER_SIDE_SELL_PLUS://			'4'		// = Sell plus
				side = 'S';
				break;

				case ORDER_SIDE_SELL_SHORT_EXEMPT://	'6'		// = Sell short exempt
				side = 'T';
				break;

				case ORDER_SIDE_UNDISCLOSED://			'7'		// = Undisclosed (valid for IOI and List Order messages only)
				case ORDER_SIDE_CROSS://				'8'		// = Cross (orders where counterparty is an exchange, valid for all messages except IOIs)
				case ORDER_SIDE_CROSS_SHORT://			'9'		// = Cross short
				case ORDER_SIDE_SHORT_EXEMPT://			'A'		//= Cross short exxmpt
				case ORDER_SIDE_AS_DEFINED://			'B'		//"As Defined" (for use with multileg instruments)
				case ORDER_SIDE_OPPOSITE://				'C'		// = "Opposite" (for use with multileg instruments)
				side = msg->Side;
				break;

				default:
				side = 'U';
				break;
			}
//			DisplayCancelError(account, msg->Symbol, side, msg->m_errorCode, msg->m_moneyValue, msg->m_moneyConstraint, msg->m_uintValue, msg->m_uintConstraint);
		}
		break;

		case M_TEXT:
		{
//			const TMsgText* msg = (const TMsgText*)message;
//			DisplayAdminBroadcastMessage((const char*)msg + sizeof(TMsgText));
		}
		break;

		case TS_NEW_ACCOUNT:
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			Account* account = msg->m_account;
			m_ComboBoxAccount.AddString((const char*)account);
		}
		break;

		case TM_NEW_ACCOUNT:
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)message;
			Account* account = msg->m_account;
			m_ComboBoxAccount.AddString((const char*)account);
		}
		break;

		case TM_UPDATED_ACCOUNT://TM_ACCOUNT_UPDATE:
//		UpdateAccount(((TMsgAccount*)message)->m_account);
		break;

		case TM_ACCOUNT_DELETE:
		{
			Account* account = ((const TMsgAccountDelete*)message)->m_account;
			ClearAccountOrders(account);
			int found = m_ComboBoxAccount.FindItem(account);
			if(found >= 0)
			{
				const int sel = m_ComboBoxAccount.GetCurSel();
				m_ComboBoxAccount.DeleteString(found);
				if(sel >= 0)
				{
					const unsigned int count = m_ComboBoxAccount.GetItemCount();
					if(count > 0)
					{
						if((unsigned int)sel >= count)
						{
							m_ComboBoxAccount.SetCurSel(count - 1);
						}
					}
				}
				OnSelchangeAccount();
			}
		}
		break;

		case TM_DELETE_ALL_ACCOUNTS:
		ClearStocks();
		m_ComboBoxAccount.ResetContent();
		m_ComboBoxAccount.AddString(nullptr);
		break;

		case M_RESP_MD_LOGON:
//		UpdateMarketDataEntitlements();
		break;

		case TS_LOGIN_RESP:
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			Account* account = msg->m_account;
			if(m_ComboBoxAccount.FindItem(account) < 0)
			{
				m_ComboBoxAccount.AddString((const char*)account);
			}

		}
		break;

		case TS_ACCOUNT_POSITION_RESP:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;

//			SendAccountPositionCost(account);
//			if(account == m_currentAccount)
//			{
//				CurrentAccountLoaded();
//			}
//			if(from != (Observable*)account)
//			{
//				AccountLoaded(account);
//			}
		}
		break;

		case TS_ACCOUNT_EXEC_HISTORY_COMPLETE:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;
			int sel = m_ComboBoxAccount.GetCurSel();
			if(sel >= 0)
			{
				if(account == (Account*)m_ComboBoxAccount.GetItemDataPtr(sel))
				{
				}
			}
		}
		break;

		case TM_DISCONNECTED:

		if(info && info->m_type == TM_CONNECTION_OBJECT)
		{
			const TMsgConnectionObject* msg = (const TMsgConnectionObject*)info;
			Connection* connection = msg->m_connection;
			if(connection == TD_GetMarketSorterConnection())
			{
			}
			else if(connection == TD_GetMarketDataConnection())
			{
			}
			else if(connection == TD_GetExecutorConnection())
			{
			}
		}
		break;

		case TM_CONNECTED:
		if(info && info->m_type == TM_CONNECTION_OBJECT)
		{
//			const TMsgConnected* msg = (const TMsgConnected*)message;
//			ConnectionConnected(msg->m_ipAddress, msg->m_port, msg->m_ipBindAddress, msg->m_bindPort, msg->m_connectionName);
			const TMsgConnectionObject* msg = (const TMsgConnectionObject*)info;
			Connection* connection = msg->m_connection;
			if(connection == TD_GetMarketSorterConnection())
			{
			}
			else if(connection == TD_GetMarketDataConnection())
			{
			}
			else if(connection == TD_GetExecutorConnection())
			{
			}
		}
		break;

		case TM_MARKET_STATUS:
		{
		}
		break;

		case TM_NEXT_DAY_STARTED:
		{
//			NextDayStarted(((const TMsgNextDayStarted*)message)->m_date);
		}
		break;

		case TM_MARKET_SORTER_LOADED:
//		MarketSorterLoaded();
		break;

		case TM_MARKET_SORTER_CLEANUP:
//		MarketSorterCleanup();
		break;

		case TM_DATA_THREAD_BUFFER_OVERFLOW:
//		PostMessage(WM_USER + 500, ((const TMsgDataThreadBufferOverflow*)message)->m_threadId, 0);
		break;
	}
*/
}
