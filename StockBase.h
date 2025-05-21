#pragma once

//class Security;

class StockBase : public Observer
{
public:
	virtual ~StockBase()
	{
		if(m_security)
		{
			TD_ReleaseStock(m_security);//the same as m_security->DecrementUnsubscribable();
		}
	}
	bool isLoaded() const{return m_loaded;}

	virtual void RemoveSecurityObserver(const Security* security){security->RemoveObserver(this);}
	virtual void AddSecurityObserver(const Security* security){security->AddObserver(this);}
	virtual void RemovePositionObserver(const Position* position){position->RemoveObserver(this);}
	virtual void AddPositionObserver(const Position* position){position->AddObserver(this);}
	virtual bool LockPositionInquiry(){return m_position->LockInquiry();}
	virtual void LockPositionInquiryWait(){m_position->LockInquiryWait();}
	virtual void UnlockPositionInquiry(){return m_position->UnlockInquiry();}

	const Security* GetSecurity() const{return m_security;}
	void SetSecurity(const Security* security)
	{
		if(m_security != security)
		{
			if(m_security)
			{
				RemoveSecurityObserver(m_security);
				TD_ReleaseStock(m_security);//the same as m_security->DecrementUnsubscribable();
			}
			m_security = security;
			m_initialized = false;

			SecuritySet();

			if(m_security)
			{
				m_security->IncrementUnsubscribable();
				AddSecurityObserver(security);
				m_loaded = m_security->isLoaded();
				if(m_loaded)
				{
					Initialize();
				}
			}
			else
			{
				Reset();
//				m_loaded = false;
			}
		}
	}
	bool isInitialized() const{return m_initialized;}

	const unsigned int& GetRoundLot() const{return m_roundLot;}
	const int& GetPositionSize() const{return m_positionSize;}
	const unsigned int& GetBuyPendingShares() const{return m_buyPendingShares;}
	const unsigned int& GetSellPendingShares() const{return m_sellPendingShares;}
	const unsigned int& GetPositionEntryTimestamp() const{return m_positionEntryTimestamp;}
	const unsigned int& GetLastCloseTimestamp() const{return m_lastCloseTimestamp;}
	const Price& GetPositionPrice() const{return m_positionPrice;}
	const Price& GetLastExecutionPrice() const{return m_lastExecutionPrice;}
	const Money& GetClosedPnl() const{return m_closedPnl;}
	const Money& GetOpenPnlLevel1() const{return m_openPnlLevel1;}
	const Money& GetOpenPnlPrint() const{return m_openPnlPrint;}

	bool LockPaintPositionInquiry() const{return m_positionPaintLock.LockInquiry();}
	void LockPaintPositionInquiryWait() const{m_positionPaintLock.LockInquiryWait();}
	void UnlockPaintPositionInquiry() const{m_positionPaintLock.UnlockInquiry();}
	bool LockPaintPositionModification() const{return m_positionPaintLock.LockModification();}
	void UnlockPaintPositionModification() const{m_positionPaintLock.UnlockModification();}

	bool UpdatePositionValues(const bool& inThread = false, Money* closedPnlDiff = nullptr, Money* openPnlDiff = nullptr, Money* openPrintPnlDiff = nullptr)
	{
		if(m_position)
		{
			if(inThread || LockPositionInquiry())
			{
				const int positionSize = m_position->GetSize();
				const unsigned int buyPendingShares = m_position->GetPendingBuyShares();
				const unsigned int sellPendingShares = m_position->GetPendingSellAndShortShares();
				const unsigned int entryTimestamp = m_position->GetPositionEnterTime();
				const Price positionPrice = m_position->GetAveragePrice();
				const Price lastExecutionPrice = Price(m_position->GetLastExecPrice());
				const Money closedPnl = m_position->GetClosedPnl();
				const Money openPnlLevel1 = m_position->GetOpenPnlLevel1();
				const Money openPnlPrint = m_position->GetOpenPnlNbboPrint();

				if(!inThread)UnlockPositionInquiry();

				bool flat = false;
				bool flipped = false;
				bool entered = false;
				bool changed = false;

				if(closedPnlDiff)*closedPnlDiff = closedPnl - m_closedPnl;
				if(openPnlDiff)*openPnlDiff = openPnlLevel1 - m_openPnlLevel1;
				if(openPrintPnlDiff)*openPrintPnlDiff = openPnlPrint - m_openPnlPrint;

				bool unlock = inThread && LockPaintPositionModification();

				if(m_positionSize != positionSize)
				{
					changed = true;
					if(m_positionSize < 0)
					{
						if(positionSize >= 0)//flipped or flat
						{
							m_lastCloseTimestamp = 0;
							if(positionSize > 0)//flipped
							{
								flipped = true;
							}
							else
							{
								flat = true;
							}
						}
						else if(m_positionSize < positionSize)//position partially closed
						{
							m_lastCloseTimestamp = TL_GetCurrentMillisecond();
						}
					}
					else if(m_positionSize > 0)
					{
						if(positionSize <= 0)//flipped or flat
						{
							m_lastCloseTimestamp = 0;
							if(positionSize < 0)//flipped
							{
								flipped = true;
							}
							else
							{
								flat = true;
							}
						}
						else if(m_positionSize > positionSize)//position partially closed
						{
							m_lastCloseTimestamp = TL_GetCurrentMillisecond();
						}
					}
					else
					{
						entered = true;
					}
					m_positionSize = positionSize;
				}
				if(m_buyPendingShares != buyPendingShares)
				{
					changed = true;
					m_buyPendingShares = buyPendingShares;
				}
				if(m_sellPendingShares != sellPendingShares)
				{
					changed = true;
					m_sellPendingShares = sellPendingShares;
				}

				if(m_positionEntryTimestamp != entryTimestamp)
				{
					changed = true;
					m_positionEntryTimestamp = entryTimestamp;
				}
				if(m_positionPrice != positionPrice)
				{
					changed = true;
					m_positionPrice = positionPrice;
				}
				if(m_lastExecutionPrice != lastExecutionPrice)
				{
					changed = true;
					m_lastExecutionPrice = lastExecutionPrice;
				}
				if(m_closedPnl != closedPnl)
				{
					changed = true;
					m_closedPnl = closedPnl;
				}
				if(m_openPnlLevel1 != openPnlLevel1)
				{
					changed = true;
					m_openPnlLevel1 = openPnlLevel1;
				}
				if(m_openPnlPrint != openPnlPrint)
				{
					changed = true;
					m_openPnlPrint = openPnlPrint;
				}

				if(unlock)UnlockPaintPositionModification();

				if(entered)
				{
					PositionEntered();
				}
				else if(flat)
				{
					PositionFlat();
				}
				else if(flipped)
				{
					PositionFlipped();
				}
				return changed;
			}
			return false;
		}
		else
		{
			if(closedPnlDiff)*closedPnlDiff = -m_closedPnl;
			if(openPnlDiff)*openPnlDiff = -m_openPnlLevel1;
			if(openPrintPnlDiff)*openPrintPnlDiff = -m_openPnlPrint;
			return ResetPositionValues(inThread);
		}
	}

	bool UpdatePositionPrintOpenPnl(const bool& inThread = false, Money* openPrintPnlDiff = nullptr)
	{
		if(m_position)
		{
			if(inThread || LockPositionInquiry())
			{
				const Money openPnlPrint = m_position->GetOpenPnlNbboPrint();

				if(!inThread)UnlockPositionInquiry();

				bool changed = m_openPnlPrint != openPnlPrint;

				if(openPrintPnlDiff)*openPrintPnlDiff = openPnlPrint - m_openPnlPrint;

				if(changed)
				{
					bool unlock = inThread && LockPaintPositionModification();
					m_openPnlPrint = openPnlPrint;
					if(unlock)UnlockPaintPositionModification();
				}

				return changed;
			}
		}
		else
		{
			if(openPrintPnlDiff)*openPrintPnlDiff = -m_openPnlPrint;
			if(!m_openPnlPrint.isZero())
			{
				bool unlock = inThread && LockPaintPositionModification();
				m_openPnlPrint.SetZero();
				if(unlock)UnlockPaintPositionModification();
				return true;
			}
		}
		return false;
	}

	bool UpdatePositionLevel1OpenPnl(const bool& inThread = false, Money* openLevel1PnlDiff = nullptr)
	{
		if(m_position)
		{
			if(inThread || LockPositionInquiry())
			{
				const Money openPnlLevel1 = m_position->GetOpenPnlLevel1();

				if(!inThread)UnlockPositionInquiry();

				bool changed = m_openPnlLevel1 != openPnlLevel1;

				if(openLevel1PnlDiff)*openLevel1PnlDiff = openPnlLevel1 - m_openPnlLevel1;

				if(changed)
				{
					bool unlock = inThread && LockPaintPositionModification();
					m_openPnlLevel1 = openPnlLevel1;
					if(unlock)UnlockPaintPositionModification();
				}

				return changed;
			}
		}
		else
		{
			if(openLevel1PnlDiff)*openLevel1PnlDiff = -m_openPnlLevel1;
			if(!m_openPnlLevel1.isZero())
			{
				bool unlock = inThread && LockPaintPositionModification();
				m_openPnlLevel1.SetZero();
				if(unlock)UnlockPaintPositionModification();
				return true;
			}
		}
		return false;
	}

	bool UpdatePositionClosedPnl(const bool& inThread = false, Money* closedPnlDiff = nullptr)
	{
		if(m_position)
		{
			if(inThread || LockPositionInquiry())
			{
				const Money closedPnl = m_position->GetClosedPnl();

				if(!inThread)UnlockPositionInquiry();

				bool changed = m_closedPnl != closedPnl;

				if(closedPnlDiff)*closedPnlDiff = closedPnl - m_closedPnl;

				if(changed)
				{
					bool unlock = inThread && LockPaintPositionModification();
					m_closedPnl = closedPnl;
					if(unlock)UnlockPaintPositionModification();
				}

				return changed;
			}
		}
		else
		{
			if(closedPnlDiff)*closedPnlDiff = -m_closedPnl;
			if(!m_closedPnl.isZero())
			{
				bool unlock = inThread && LockPaintPositionModification();
				m_closedPnl.SetZero();
				if(unlock)UnlockPaintPositionModification();
				return true;
			}
		}
		return false;
	}

	bool ResetPositionValues(const bool& inThread)
	{
		bool changed = false;
		bool unlock = inThread && LockPaintPositionModification();
		if(m_positionSize)
		{
			changed = true;
			m_positionSize = 0;
		}
		if(m_buyPendingShares)
		{
			changed = true;
			m_buyPendingShares = 0;
		}
		if(m_sellPendingShares)
		{
			changed = true;
			m_sellPendingShares = 0;
		}
		if(m_positionEntryTimestamp)
		{
			changed = true;
			m_positionEntryTimestamp = 0;
		}
		if(!m_positionPrice.isZero())
		{
			changed = true;
			m_positionPrice.SetZero();
		}
		if(!m_lastExecutionPrice.isZero())
		{
			changed = true;
			m_lastExecutionPrice.SetZero();
		}
		if(!m_closedPnl.isZero())
		{
			changed = true;
			m_closedPnl.SetZero();
		}
		if(!m_openPnlLevel1.isZero())
		{
			changed = true;
			m_openPnlLevel1.SetZero();
		}
		if(!m_openPnlPrint.isZero())
		{
			changed = true;
			m_openPnlPrint.SetZero();
		}
		m_lastCloseTimestamp = 0;

		if(unlock)UnlockPaintPositionModification();

		return changed;
	}

	const Position* GetPosition() const{return m_position;}
//	Position* GetPosition(){return m_position;}
	bool SetPosition(const Position* position)
	{
		if(m_position != position)
		{
			if(m_position)
			{
				RemovePositionObserver(m_position);
			}
			m_position = position;
			if(m_position)
			{
				AddPositionObserver(m_position);
				UpdatePositionOnAccountChange();
			}
			return true;
		}
		return false;
	}

	const Account* GetAccount() const{return m_account;}
//	Account* GetAccount(){return m_account;}
	bool SetAccount(const Account* account)
	{
		if(m_account != account)
		{
//			m_account->WriteMessageToAccountThread();
//			if(m_account)NotifyAccountRemoveInThread();
			m_account = account;
//			if(m_account)NotifyAccountAddInThread();
			return SetPosition(m_account ? m_account->FindStockPositionWithLock(m_numericSymbol) : nullptr);
		}
		return false;
	}

	const unsigned __int64& GetNumericSymbol() const{return m_numericSymbol;}
	const char* GetSymbol() const{return (const char*)&m_numericSymbol;}
	const char& GetMarketCategory() const{return m_marketCategory;}
	const char& GetPrimaryExchange() const{return m_primaryExchange;}
	const bool& isNasdaq() const{return m_nasdaq;}

	void Initialize()
	{
		if(!m_initialized)
		{
			if(m_security)
			{
				m_roundLot = m_security->GetRoundLot();
				m_marketCategory = m_security->GetMarketCategory();
				m_primaryExchange = m_security->GetPrimaryExchange();
				m_nasdaq = m_security->isNasdaq();
			}
			if(DoInitialize())
			{
				m_initialized = true;
			}
		}
	}
	void Reset()
	{
		if(m_initialized)
		{
			DoReset();
			m_initialized = false;
		}
	}
	virtual void MarketSorterConnected(bool connected)
	{
/*
		if(!connected)
		{
			SetSecurity(nullptr);
		}
*/
	}
	virtual void MarketSorterCleanup()
	{
		SetSecurity(nullptr);
	}
	virtual void PositionValuesChanged(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff){}
//Special Timer
	virtual void ResetTimer()
	{
		m_buyPendingShares = 0;
		m_sellPendingShares = 0;
		m_positionEntryTimestamp = 0;
		m_lastCloseTimestamp = 0;
	}
protected:
	StockBase(const char* symbol):
		m_account(nullptr),
		m_position(nullptr),
		m_security(nullptr),
		m_numericSymbol(U_RepresentStringAsUnsignedNumber<unsigned __int64>(symbol, sizeof(unsigned __int64) - 1)),//U_StringToUInt64(symbol)),
		m_marketCategory('\0'),
		m_primaryExchange('\0'),
		m_nasdaq(false),
		m_loaded(false),
		m_initialized(false),
		m_roundLot(100),
		m_positionSize(0),
		m_buyPendingShares(0),
		m_sellPendingShares(0),
		m_positionEntryTimestamp(0),
		m_lastCloseTimestamp(0)
	{}
	virtual void UpdatePositionOnAccountChange()
	{
		Money closedPnlDiff, openPnlDiff, openPrintPnlDiff;
		if(UpdatePositionValues(false, &closedPnlDiff, &openPnlDiff, &openPrintPnlDiff))
		{
			PositionValuesChanged(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
		}
	}
	virtual bool DoInitialize() = 0;
	virtual void DoReset(){m_loaded = false;}// m_marketCategory = '\0';}
	virtual void SecuritySet(){}

	TakionLockMultithread m_positionPaintLock;

	const Account* m_account;
	const Position* m_position;
	const Security* m_security;
	unsigned __int64 m_numericSymbol;
	char m_marketCategory;
	char m_primaryExchange;
	bool m_nasdaq;
	bool m_loaded;
	bool m_initialized;

	unsigned int m_roundLot;

	int m_positionSize;
	unsigned int m_buyPendingShares;
	unsigned int m_sellPendingShares;
	unsigned int m_positionEntryTimestamp;
	unsigned int m_lastCloseTimestamp;
	Price m_positionPrice;
	Price m_lastExecutionPrice;
	Money m_closedPnl;
	Money m_openPnlLevel1;
	Money m_openPnlPrint;

	virtual void PositionEntered(){}
	virtual void PositionFlat(){}
	virtual void PositionFlipped(){}
};