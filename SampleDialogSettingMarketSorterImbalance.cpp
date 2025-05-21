#include "stdafx.h"
#include "SampleDialogSettingMarketSorterImbalance.h"
#include "StockBase.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

class StockImbalance : public StockBase
{
public:
	StockImbalance(const Security* security, const bool& inThread, ListBoxImbalance* owner):
		StockBase(security->GetSymbol()),
		m_owner(owner),//for repainting of the changed values.
		m_ordinal(0xFFFFFFFF),//ordinal number in the list box.

		m_mainThreadDelay(0),

		m_inThread(inThread),
		m_yestVolume(0),
//NYSE
		m_nyseImbalanceReg(0),
		m_nyseImbalanceRegTime(0),
		m_nyseImbalanceRegTimeLogged(0),
		m_nyseImbalanceInfo(0),
		m_nyseImbalanceInfoTime(0),
		m_nyseImbalanceInfoTimeLogged(0),
		m_nysePairedShares(0),
		m_nyseInThreadDelay(0),
		m_nyseRegInThreadDelay(0),
		m_nyseCrossType('\0'),
//NSDQ
		m_nsdqImbalanceInfo(0),
		m_nsdqImbalanceInfoTime(0),
		m_nsdqImbalanceInfoTimeLogged(0),
		m_nsdqPairedShares(0),
		m_nsdqInThreadDelay(0),
		m_nsdqCrossType('\0'),
		m_nsdqPriceVariationIndicator('\0'),
//ARCA
		m_arcaImbalanceMarket(0),
		m_arcaImbalanceInfo(0),
		m_arcaImbalanceInfoTime(0),
		m_arcaImbalanceInfoTimeLogged(0),
		m_arcaPairedShares(0),
		m_arcaInThreadDelay(0),
		m_arcaCrossType('\0'),

		m_log(false)
	{
		SetSecurity(security);
//		InitializeAll();
	}
	void SetInThread(const bool& inThread)
	{
		if(inThread != m_inThread)
		{
			m_inThread = inThread;
			if(m_security)
			{
				if(m_inThread)
				{
					if(!m_log)
					{
						m_security->RemoveObserver(this);
					}
					m_security->AddInThreadObserver(this);
				}
				else
				{
					if(!m_log)
					{
						m_security->RemoveInThreadObserver(this);
					}
					m_security->AddObserver(this);
				}
			}
		}
	}
	void ResetImbalances()
	{
		bool unlock = m_inThread && LockModification();

		m_mainThreadDelay = 0;
//NYSE
		m_nyseInThreadDelay = 0;
		m_nyseImbalanceInfo = 0;
		m_nyseImbalanceInfoTime = 0;
		m_nyseImbalanceInfoTimeLogged = 0;
		m_nysePairedShares = 0;
		m_nyseCrossType = '\0';
		m_nyseFarPrice.SetZero();
		m_nyseNearPrice.SetZero();
		m_nyseRefPrice.SetZero();

		m_nyseRegInThreadDelay = 0;
		m_nyseImbalanceReg = 0;
		m_nyseImbalanceRegTime = 0;
		m_nyseImbalanceRegTimeLogged = 0;
		m_nyseRegFarPrice.SetZero();
		m_nyseRegNearPrice.SetZero();
		m_nyseRegRefPrice.SetZero();
		m_nyseRegPairedShares = 0;
//NSDQ
		m_nsdqInThreadDelay = 0;
		m_nsdqImbalanceInfo = 0;
		m_nsdqImbalanceInfoTime = 0;
		m_nsdqImbalanceInfoTimeLogged = 0;
		m_nsdqPairedShares = 0;
		m_nsdqCrossType = '\0';
		m_nsdqPriceVariationIndicator = '\0';
		m_nsdqFarPrice.SetZero();
		m_nsdqNearPrice.SetZero();
		m_nsdqRefPrice.SetZero();
//ARCA
		m_arcaInThreadDelay = 0;
		m_arcaImbalanceMarket = 0;
		m_arcaImbalanceInfo = 0;
		m_arcaImbalanceInfoTime = 0;
		m_arcaImbalanceInfoTimeLogged = 0;
		m_arcaPairedShares = 0;
		m_arcaCrossType = '\0';
		m_arcaFarPrice.SetZero();
		m_arcaNearPrice.SetZero();
		m_arcaRefPrice.SetZero();

		if(unlock)UnlockModification();
	}

	virtual void DoReset(){StockBase::DoReset(); ResetImbalances();}

	bool SetOrdinal(const unsigned int& ordinal)
	{
		if(ordinal != m_ordinal)
		{
			m_ordinal = ordinal;
			return true;
		}
		return false;
	}
	void IncrementOrdinal(){++m_ordinal;}
	const unsigned int& GetOrdinal() const{return m_ordinal;}
	void Repaint(int left = -1, int right = -1)
	{
		if(m_ordinal != 0xFFFFFFFF)
		{
			m_owner->InvalidateItem(m_ordinal, left, right);
		}
	}
	void RepaintColumn(unsigned short from)
	{
		if(m_ordinal != 0xFFFFFFFF)
		{
			m_owner->InvalidateItemColumn(m_ordinal, from);
		}
	}
	void RepaintColumns(unsigned short from, unsigned short to)
	{
		if(m_ordinal != 0xFFFFFFFF)
		{
			m_owner->InvalidateItemColumnRange(m_ordinal, from, to);
		}
	}
	DECLARE_NED_NEW

	virtual void RemoveSecurityObserver(const Security* security) override
	{
		if(m_inThread)
		{
			security->RemoveInThreadObserver(this);
			if(m_log)
			{
				security->RemoveObserver(this);
			}
		}
		else
		{
			security->RemoveObserver(this);
			if(m_log)
			{
				security->RemoveInThreadObserver(this);
			}
		}
	}
	virtual void AddSecurityObserver(const Security* security) override
	{
		if(m_inThread)
		{
			security->AddInThreadObserver(this);
			if(m_log)
			{
				security->AddObserver(this);
			}
		}
		else
		{
			security->AddObserver(this);
			if(m_log)
			{
				security->AddInThreadObserver(this);
			}
		}
	}

	const bool& isLog() const{return m_log;}

	bool SetLog(const bool& log)
	{
		if(log != m_log)
		{
			m_log = log;
			if(m_inThread)
			{
				if(log)
				{
					m_security->AddObserver(this);
				}
				else
				{
					m_security->RemoveObserver(this);
				}
			}
			else
			{
				if(log)
				{
					m_security->AddInThreadObserver(this);
				}
				else
				{
					m_security->RemoveInThreadObserver(this);
				}
			}
			return true;
		}
		return false;
	}
/*
	virtual void SecuritySet()
	{
		InitializeAll();
	}
*/
	void InitializeAll()//Main Thread
	{
		LockInquiryWait();

		m_security->LockInquiryWaitLevel1();

		m_yestVolume = m_security->GetYesterdaysVolume();

		UpdateNyseImbalance(false, 3);//3 - both regulatory and info
		UpdateNsdqImbalance(false);
		UpdateArcaImbalance(false);

		m_security->UnlockInquiryLevel1();

		UnlockInquiry();
	}
//NYSE
	const short& GetNyseInThreadDelay() const{return m_nyseInThreadDelay;}
	const int& GetNyseImbalanceInfo() const{return m_nyseImbalanceInfo;}
	const unsigned int& GetNyseImbalanceInfoTime() const{return m_nyseImbalanceInfoTime;}
	const Price& GetNyseFarPrice() const{return m_nyseFarPrice;}
	const Price& GetNyseNearPrice() const{return m_nyseNearPrice;}
	const Price& GetNyseCurrentReferencePrice() const{return m_nyseRefPrice;}
	const unsigned int& GetNysePairedShares() const{return m_nysePairedShares;}

	const short& GetNyseRegInThreadDelay() const{return m_nyseRegInThreadDelay;}
	const int& GetNyseImbalanceReg() const{return m_nyseImbalanceReg;}
	const unsigned int& GetNyseImbalanceRegTime() const{return m_nyseImbalanceRegTime;}
	const Price& GetNyseRegFarPrice() const{return m_nyseRegFarPrice;}
	const Price& GetNyseRegNearPrice() const{return m_nyseRegNearPrice;}
	const Price& GetNyseRegCurrentReferencePrice() const{return m_nyseRegRefPrice;}
	const unsigned int& GetNyseRegPairedShares() const{return m_nyseRegPairedShares;}

	const char& GetNyseCrossType() const{return m_nyseCrossType;}
//NSDQ
	const short& GetNsdqInThreadDelay() const{return m_nsdqInThreadDelay;}
	const int& GetNsdqImbalanceInfo() const{return m_nsdqImbalanceInfo;}
	const unsigned int& GetNsdqImbalanceInfoTime() const{return m_nsdqImbalanceInfoTime;}
	const Price& GetNsdqFarPrice() const{return m_nsdqFarPrice;}
	const Price& GetNsdqNearPrice() const{return m_nsdqNearPrice;}
	const Price& GetNsdqCurrentReferencePrice() const{return m_nsdqRefPrice;}
	const unsigned int& GetNsdqPairedShares() const{return m_nsdqPairedShares;}
	const char& GetNsdqCrossType() const{return m_nsdqCrossType;}
	const char& GetNsdqPriceVariationIndicator() const{return m_nsdqPriceVariationIndicator;}
//ARCA
	const short& GetArcaInThreadDelay() const{return m_arcaInThreadDelay;}
	const int& GetArcaImbalanceMarket() const{return m_arcaImbalanceMarket;}
	const int& GetArcaImbalanceInfo() const{return m_arcaImbalanceInfo;}
	const unsigned int& GetArcaImbalanceInfoTime() const{return m_arcaImbalanceInfoTime;}
	const Price& GetArcaFarPrice() const{return m_arcaFarPrice;}
	const Price& GetArcaNearPrice() const{return m_arcaNearPrice;}
	const Price& GetArcaCurrentReferencePrice() const{return m_arcaRefPrice;}
	const unsigned int& GetArcaPairedShares() const{return m_arcaPairedShares;}
	const char& GetArcaCrossType() const{return m_arcaCrossType;}

	const unsigned __int64& GetYestVolume() const{return m_yestVolume;}

	const short& GetMainThreadDelay() const{return m_mainThreadDelay;}

//When the imbalance is updated in the worker thread, we need to LockInquiry when we paint in the main thread, and LockModification when we change the values in the worker thread.
	bool LockInquiry() const{return m_paintLock.LockInquiry();}
	void LockInquiryWait() const{m_paintLock.LockInquiryWait();}
	void UnlockInquiry() const{m_paintLock.UnlockInquiry();}
	bool LockModification() const{return m_paintLock.LockModification();}
	void UnlockModification() const{m_paintLock.UnlockModification();}
protected:
	virtual bool DoInitialize() override;
	void DoInitializeWithoutLocking();
//	virtual void DoReset() override;
//This Observer adds directly to the Security observable (look at function SetSecurity(Security* security); in file StockBase.h
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;

	void UpdateImbalanceInThread(const unsigned char bookId, bool regulatory);
	void PostRepaintMessageFromWorkerThread(unsigned short startColumn, unsigned short endColumn) const;
/*
short WINAPI TU_GetQuoteDelay(unsigned int millisecond)
{
	unsigned int delay = TL_GetCurrentMillisecond();
	if(millisecond < delay)
	{
		delay -= millisecond;
		return delay > 0x7FFF ? 0x7FFF : (short)delay;
	}
	delay = millisecond - delay;
	return delay > 0x7FFF ? -0x7FFF : -(short)delay;
}
*/
	void UpdateNyseImbalance(bool onTheFly, unsigned char regInfo)//regInfo = 1 - info; regInfo = 2 - regulatory; regInfo = 3 - both
	{
		if(regInfo & 1)
		{
			m_nyseImbalanceInfo = m_security->GetImbalanceSharesNyse();
			m_nyseImbalanceInfoTime = m_security->GetImbalanceTimeNyse();
			m_nysePairedShares = m_security->GetPairedSharesNyse();
			m_nyseFarPrice = m_security->GetFarPriceNyse();
			m_nyseNearPrice = m_security->GetNearPriceNyse();
			m_nyseRefPrice = m_security->GetCurrentReferencePriceNyse();
//			if(m_inThread && onTheFly)m_nyseInThreadDelay = TU_GetQuoteDelay(m_nyseImbalanceInfoTime);
			if(m_inThread && onTheFly)m_nyseInThreadDelay = TL_GetDelay(m_nyseImbalanceInfoTime);
		}
		if(regInfo & 2)
		{
			m_nyseImbalanceReg = m_security->GetRegulatoryImbalanceSharesNyse();
			m_nyseImbalanceRegTime = m_security->GetRegulatoryImbalanceTimeNyse();
			m_nyseRegFarPrice = m_security->GetRegulatoryFarPriceNyse();
			m_nyseRegNearPrice = m_security->GetRegulatoryNearPriceNyse();
			m_nyseRegRefPrice = m_security->GetRegulatoryCurrentReferencePriceNyse();
			m_nyseRegPairedShares = m_security->GetRegulatoryPairedSharesNyse();
//			if(m_inThread && onTheFly)m_nyseRegInThreadDelay = TU_GetQuoteDelay(m_nyseImbalanceRegTime);
			if(m_inThread && onTheFly)m_nyseRegInThreadDelay = TL_GetDelay(m_nyseImbalanceRegTime);
		}
		m_nyseCrossType = m_security->GetCrossTypeNyse();
	}

	void UpdateNsdqImbalance(bool onTheFly)
	{
		m_nsdqImbalanceInfo = m_security->GetImbalanceSharesNsdq();
		m_nsdqImbalanceInfoTime = m_security->GetImbalanceTimeNsdq();
		m_nsdqPairedShares = m_security->GetPairedSharesNsdq();
		m_nsdqCrossType = m_security->GetCrossTypeNsdq();
		m_nsdqPriceVariationIndicator = m_security->GetPriceVariationIndicatorNsdq();
		m_nsdqFarPrice = m_security->GetFarPriceNsdq();
		m_nsdqNearPrice = m_security->GetNearPriceNsdq();
		m_nsdqRefPrice = m_security->GetCurrentReferencePriceNsdq();

//		if(m_inThread && onTheFly)m_nsdqInThreadDelay = TU_GetQuoteDelay(m_nsdqImbalanceInfoTime);
		if(m_inThread && onTheFly)m_nsdqInThreadDelay = TL_GetDelay(m_nsdqImbalanceInfoTime);
	}

	void UpdateArcaImbalance(bool onTheFly)
	{
		m_arcaImbalanceMarket = m_security->GetMarketImbalanceSharesArca();
		m_arcaImbalanceInfo = m_security->GetImbalanceSharesArca();
		m_arcaImbalanceInfoTime = m_security->GetImbalanceTimeArca();
		m_arcaPairedShares = m_security->GetPairedSharesArca();
		m_arcaCrossType = m_security->GetCrossTypeArca();
		m_arcaFarPrice = m_security->GetFarPriceArca();
		m_arcaNearPrice = m_security->GetNearPriceArca();
		m_arcaRefPrice = m_security->GetCurrentReferencePriceArca();

//		if(m_inThread && onTheFly)m_arcaInThreadDelay = TU_GetQuoteDelay(m_arcaImbalanceInfoTime);
		if(m_inThread && onTheFly)m_arcaInThreadDelay = TL_GetDelay(m_arcaImbalanceInfoTime);
	}

	ListBoxImbalance* m_owner;
	TakionLockMultithread m_paintLock;//When the imbalance is updated in the worker thread, we need to LockInquiry when we paint in the main thread, and LockModification when we change the values in the worker thread.
	unsigned int m_ordinal;//ordinal number in the list box. (for quick repaint, otherwise we would have to find the stock in the list box to repaint this specific stock).

	short m_mainThreadDelay;

	bool m_inThread;

	unsigned __int64 m_yestVolume;
//NYSE
	int m_nyseImbalanceReg;
	unsigned int m_nyseImbalanceRegTime;
	unsigned int m_nyseImbalanceRegTimeLogged;
	unsigned int m_nyseRegPairedShares;
	int m_nyseImbalanceInfo;
	unsigned int m_nyseImbalanceInfoTime;
	unsigned int m_nyseImbalanceInfoTimeLogged;
	unsigned int m_nysePairedShares;
	short m_nyseInThreadDelay;
	short m_nyseRegInThreadDelay;
	char m_nyseCrossType;
	Price m_nyseFarPrice;
	Price m_nyseNearPrice;
	Price m_nyseRefPrice;

	Price m_nyseRegFarPrice;
	Price m_nyseRegNearPrice;
	Price m_nyseRegRefPrice;
//NSDQ
	int m_nsdqImbalanceInfo;
	unsigned int m_nsdqImbalanceInfoTime;
	unsigned int m_nsdqImbalanceInfoTimeLogged;
	unsigned int m_nsdqPairedShares;
	short m_nsdqInThreadDelay;
	char m_nsdqCrossType;
	char m_nsdqPriceVariationIndicator;
	Price m_nsdqFarPrice;
	Price m_nsdqNearPrice;
	Price m_nsdqRefPrice;
//ARCA
	int m_arcaImbalanceMarket;
	int m_arcaImbalanceInfo;
	unsigned int m_arcaImbalanceInfoTime;
	unsigned int m_arcaImbalanceInfoTimeLogged;
	unsigned int m_arcaPairedShares;
	short m_arcaInThreadDelay;
	char m_arcaCrossType;
	Price m_arcaFarPrice;
	Price m_arcaNearPrice;
	Price m_arcaRefPrice;

	bool m_log;
};

class LbImbalanceHorizontalScroller : public LbHorizontalScroller
{
public:
	LbImbalanceHorizontalScroller(ListBoxOwnerDraw* listBox):LbHorizontalScroller(listBox){}
	virtual void AddColumns() override;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void StockImbalance::DoInitializeWithoutLocking()
{
	InitializeAll();
/*
	m_level1Bid = Price(m_security->GetL1Bid());
	m_level1Ask = Price(m_security->GetL1Ask());
	m_lastPrint = Price(m_security->GetLastNbboPrintPrice());
	m_high = Price(m_security->GetMarketHoursHighPrice());
	m_low = Price(m_security->GetMarketHoursLowPrice());
	m_volume = m_security->GetVolume();
*/
}

bool StockImbalance::DoInitialize()
{
	if(m_security && m_security->LockInquiryLevel1())
	{
		DoInitializeWithoutLocking();
		m_security->UnlockInquiryLevel1();
		return true;
	}
	return false;
}

void StockImbalance::PostRepaintMessageFromWorkerThread(unsigned short startColumn, unsigned short endColumn) const
{
	if(m_ordinal != 0xFFFFFFFF && startColumn <= endColumn)
	{
		m_owner->PostMessage(WM_USER + 444, m_ordinal, ((unsigned int)startColumn << 16) | endColumn);
	}
}

void StockImbalance::UpdateImbalanceInThread(const unsigned char bookId, bool regulatory)
{
	bool unlock;// = LockModification();
	switch(bookId)
	{
		case ECNBOOK_ARCA:
		if(m_log)
		{
			std::string info(m_security->GetSymbol());
			info += " ARCA ";
			m_security->GetArcaImbalance()->toString(info, 4);
TD_Log(info.c_str(), "EXT_IMB");
		}
		if(m_inThread)
		{
			unlock = LockModification();
			UpdateArcaImbalance(true);
			if(unlock)UnlockModification();
//Can't repaint from a thread. Post Windows Message
//			RepaintColumns(MSI_ARCA_IMBALANCE_DELAY, MSI_ARCA_CROSS_TYPE);
			PostRepaintMessageFromWorkerThread(MSI_ARCA_IMBALANCE_DELAY, MSI_ARCA_CROSS_TYPE);
		}
		break;

		case ECNBOOK_NSDQ:
		if(m_log)
		{
			std::string info(m_security->GetSymbol());
			info += " NSDQ ";
			m_security->GetNasdaqImbalance()->toString(info, 4);
TD_Log(info.c_str(), "EXT_IMB");
		}
		if(m_inThread)
		{
			unlock = LockModification();
			UpdateNsdqImbalance(true);
			if(unlock)UnlockModification();
//Can't repaint from a thread. Post Windows Message
//			RepaintColumns(MSI_NSDQ_IMBALANCE_DELAY, MSI_NSDQ_PRICE_VARIATION_INDICATOR);
			PostRepaintMessageFromWorkerThread(MSI_NSDQ_IMBALANCE_DELAY, MSI_NSDQ_PRICE_VARIATION_INDICATOR);
		}
		break;

		case ECNBOOK_NYS:
		case ECNBOOK_AMEX:
		case ECNBOOK_BATS:
		case ECNBOOK_BATY:
//		case ECNBOOK_IEX:
		if(m_log)
		{
			std::string info(m_security->GetSymbol());
			info += " NYSE ";
			m_security->GetNyseImbalance()->toString(info, 4);
TD_Log(info.c_str(), "EXT_IMB");
		}
		if(m_inThread)
		{
			unlock = LockModification();
			UpdateNyseImbalance(true, regulatory ? 2 : 1);
			if(unlock)UnlockModification();
//Can't repaint from a thread. Post Windows Message
//			if(regulatory)RepaintColumns(MSI_NYSE_CROSS_TYPE, MSI_NYSE_PAIRED_SHARES_REGULATORY);
//			else RepaintColumns(MSI_NYSE_IMBALANCE_DELAY, MSI_NYSE_CROSS_TYPE);
			if(regulatory)PostRepaintMessageFromWorkerThread(MSI_NYSE_CROSS_TYPE, MSI_NYSE_PAIRED_SHARES_REGULATORY);
			else PostRepaintMessageFromWorkerThread(MSI_NYSE_IMBALANCE_DELAY, MSI_NYSE_CROSS_TYPE);
		}
		break;
	}
//	if(unlock)UnlockModification();
}

void StockImbalance::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_UPDATED_POSITION:
		{
			Money closedPnlDiff, openPnlDiff, openPrintPnlDiff;
			if(UpdatePositionValues(false, &closedPnlDiff, &openPnlDiff, &openPrintPnlDiff))
			{
				PositionValuesChanged(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
			}
		}
		break;

//There are several messagse that come from the server with the imbalance notification.
//We don't care about the structure of the messages, because when we are notified, the stock m_security already has the updated values. (Security* m_security belongs to the parent class StockBase)
//Only member m_bookID of the messages is useful because it tells us which imbalance data changed - NYSE (AMEX, BATS), NSDQ, ARCA. It can save some CPU usage because we update data only for some fields that belong to a certain exchange imbalance (like NYSE, or ARCA), not all the fields.
//We know which stock it is, because object StockImbalance contains member Security* m_security to which we added StockImbalance as an Observer, and thus we can be notified only by m_security.
		
		case M_MS_IMBALANCE_SHORT://worker thread, short message
//		if(m_inThread)
		{
			const TMsgMsImbalanceShort* msg = (const TMsgMsImbalanceShort*)message;
			UpdateImbalanceInThread(msg->m_bookID & 15, msg->m_regulatoryImbalance_StockOpen == 1);
		}
		break;
			
		case M_MS_IMBALANCE_LONG://worker thread, long message
//		if(m_inThread)
		{
			const TMsgMsImbalanceLong* msg = (const TMsgMsImbalanceLong*)message;
			UpdateImbalanceInThread(msg->m_bookID & 15, msg->m_regulatoryImbalance_StockOpen == 1);
		}
		break;

		case SM_MS_IMBALANCE_SHORT://worker thread, symbol mapping, short message
//		if(m_inThread)
		{
			const TSmMsgMsImbalanceShort* msg = (const TSmMsgMsImbalanceShort*)message;
			UpdateImbalanceInThread(msg->m_bookID & 15, msg->m_regulatoryImbalance_StockOpen == 1);
		}
		break;

		case SM_MS_IMBALANCE_LONG://worker thread, symbol mapping, long message
//		if(m_inThread)
		{
			const TSmMsgMsImbalanceLong* msg = (const TSmMsgMsImbalanceLong*)message;
			UpdateImbalanceInThread(msg->m_bookID & 15, msg->m_regulatoryImbalance_StockOpen == 1);
		}
		break;

		case TM_SORTABLE_SECURITY_ENTITLEMENTS_CHANGED:
		case TM_SORTABLE_SECURITY_EXT_ENTITLEMENTS_CHANGED:
		if(m_inThread)
		{
			DoInitializeWithoutLocking();
		}
		else
		{
			DoInitialize();
		}
		Repaint();
		break;

		case TM_EQUITY_IMBALANCE_UPDATE://main thread notification
		if(!m_inThread)
		{
//Here we have to update all the fields because we don't know which Exchange imbalances updated in the worker thread.
//We don't have to lock (unlock = LockModification();) because the update message was posted to the main thread.
//We will lock m_security->LockInquiryWaitLevel1() so that while we update our imbalance values, the security values don't change in the worker thread
			std::string infoNyse("NYSE ");
			std::string infoNsdq("NSDQ ");
			std::string infoArca("ARCA ");
			std::string prefix;
			if(m_log)
			{
				prefix = "EXT_MT_IMB ";
				prefix += m_security->GetSymbol();
			}
			const Imbalance* nyseImbalance = m_security->GetNyseImbalance();
			bool nyseOk = nyseImbalance && nyseImbalance->GetImbalanceTime();
			const Imbalance* nsdqImbalance = m_security->GetNasdaqImbalance();
			bool nsdqOk = nsdqImbalance && nsdqImbalance->GetImbalanceTime();
			const Imbalance* arcaImbalance = m_security->GetArcaImbalance();
			bool arcaOk = arcaImbalance && arcaImbalance->GetImbalanceTime();

			m_security->LockInquiryWaitLevel1();

			if(m_log)
			{
				if(nyseOk)
				{
//In the Main Thread, Log only if we have not logged the imbalance info before.
					if(m_nyseImbalanceInfoTimeLogged < nyseImbalance->GetImbalanceTime() || m_nyseImbalanceRegTimeLogged < nyseImbalance->GetRegulatoryImbalanceTime())
					{
						nyseImbalance->toString(infoNyse, 4);
						m_nyseImbalanceInfoTimeLogged = nyseImbalance->GetImbalanceTime();
						m_nyseImbalanceRegTimeLogged = nyseImbalance->GetRegulatoryImbalanceTime();
					}
					else
					{
						nyseOk = false;
					}
				}
				if(nsdqOk)
				{
//In the Main Thread, Log only if we have not logged the imbalance info before.
					if(m_nsdqImbalanceInfoTimeLogged < nsdqImbalance->GetImbalanceTime())
					{
						nsdqImbalance->toString(infoNsdq, 4);
						m_nsdqImbalanceInfoTimeLogged = nsdqImbalance->GetImbalanceTime();
					}
					else
					{
						nsdqOk = false;
					}
				}
				if(arcaOk)
				{
//In the Main Thread, Log only if we have not logged the imbalance info before.
					if(m_arcaImbalanceInfoTimeLogged < arcaImbalance->GetImbalanceTime())
					{
						arcaImbalance->toString(infoArca, 4);
						m_arcaImbalanceInfoTimeLogged = arcaImbalance->GetImbalanceTime();
					}
					else
					{
						arcaOk = false;
					}
				}
			}

			UpdateNyseImbalance(true, 3);//3 - both regulatory and info
			UpdateNsdqImbalance(true);
			UpdateArcaImbalance(true);

			m_security->UnlockInquiryLevel1();

			if(m_log)
			{
				if(nyseOk)TD_Log(infoNyse.c_str(), prefix.c_str());
				if(nsdqOk)TD_Log(infoNsdq.c_str(), prefix.c_str());
				if(arcaOk)TD_Log(infoArca.c_str(), prefix.c_str());
			}

			unsigned int latestTimestamp = m_arcaImbalanceInfoTime;
			if(m_nsdqImbalanceInfoTime > latestTimestamp)latestTimestamp = m_nsdqImbalanceInfoTime;
			if(m_nyseImbalanceInfoTime > latestTimestamp)latestTimestamp = m_nyseImbalanceInfoTime;
			if(m_nyseImbalanceRegTime > latestTimestamp)latestTimestamp = m_nyseImbalanceRegTime;
			
//			m_mainThreadDelay = TU_GetQuoteDelay(latestTimestamp);
			m_mainThreadDelay = TL_GetDelay(latestTimestamp);

			Repaint();
		}
		else if(m_log)
		{
			std::string infoNyse("NYSE ");
			std::string infoNsdq("NSDQ ");
			std::string infoArca("ARCA ");
			std::string prefix("EXT_MT_IMB ");
			prefix += m_security->GetSymbol();

			const Imbalance* nyseImbalance = m_security->GetNyseImbalance();
			bool nyseOk = nyseImbalance && nyseImbalance->GetImbalanceTime();
			const Imbalance* nsdqImbalance = m_security->GetNasdaqImbalance();
			bool nsdqOk = nsdqImbalance && nsdqImbalance->GetImbalanceTime();
			const Imbalance* arcaImbalance = m_security->GetArcaImbalance();
			bool arcaOk = arcaImbalance && arcaImbalance->GetImbalanceTime();

			m_security->LockInquiryWaitLevel1();
//			if(nyseOk)nyseImbalance->toString(infoNyse, 4);
//			if(nsdqOk)nsdqImbalance->toString(infoNsdq, 4);
//			if(arcaOk)arcaImbalance->toString(infoArca, 4);
	
			if(nyseOk)
			{
//Log only if we have not logged the imbalance info before.
				if(m_nyseImbalanceInfoTimeLogged < nyseImbalance->GetImbalanceTime() || m_nyseImbalanceRegTimeLogged < nyseImbalance->GetRegulatoryImbalanceTime())
				{
					nyseImbalance->toString(infoNyse, 4);
					m_nyseImbalanceInfoTimeLogged = nyseImbalance->GetImbalanceTime();
					m_nyseImbalanceRegTimeLogged = nyseImbalance->GetRegulatoryImbalanceTime();
				}
				else
				{
					nyseOk = false;
				}
			}
			if(nsdqOk)
			{
				if(m_nsdqImbalanceInfoTimeLogged < nsdqImbalance->GetImbalanceTime())
				{
					nsdqImbalance->toString(infoNsdq, 4);
					m_nsdqImbalanceInfoTimeLogged = nsdqImbalance->GetImbalanceTime();
				}
				else
				{
					nsdqOk = false;
				}
			}
			if(arcaOk)
			{
				if(m_arcaImbalanceInfoTimeLogged < arcaImbalance->GetImbalanceTime())
				{
					arcaImbalance->toString(infoArca, 4);
					m_arcaImbalanceInfoTimeLogged = arcaImbalance->GetImbalanceTime();
				}
				else
				{
					arcaOk = false;
				}
			}
			m_security->UnlockInquiryLevel1();

			if(nyseOk)TD_Log(infoNyse.c_str(), prefix.c_str());
			if(nsdqOk)TD_Log(infoNsdq.c_str(), prefix.c_str());
			if(arcaOk)TD_Log(infoArca.c_str(), prefix.c_str());
		}
		break;

		case TM_EQUITY_NEW_MINUTE:
		break;

		case TM_EQUITY_CHART_LOADED:
		break;

		case TM_EQUITY_REFRESHED:
		if(m_loaded)
		{
//stock reloaded for whatever reason. Should not happen
		}
		else
		{
			m_loaded = true;
//			m_owner->IncrementLoadedStocks();
		}
		Initialize();
		Repaint();//m_owner->GetLevel1BidLeft(), m_owner->GetVolumeRight());
		break;
/*
		default://update message
		if(UpdateValues())
		{
			Repaint(m_owner->GetLevel1BidLeft(), m_owner->GetVolumeRight());
		}
		break;
*/
	}
}


//////
void LbImbalanceHorizontalScroller::AddColumns()
{
	const int itemHeight = m_listBox->GetMyItemHeight();
	const int doubleHeight = itemHeight + itemHeight;
	const int tripleHeight = doubleHeight + itemHeight;
	const int fourHeight = tripleHeight + itemHeight;
	const int fiveHeight = fourHeight + itemHeight;
	const int sixHeight = fiveHeight + itemHeight;
	const int sevenHeight = sixHeight + itemHeight;
	AddColumn("Symbol", MSI_SYMBOL, fourHeight, DT_LEFT, DT_LEFT, DT_RIGHT, "Symbol");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("Exch", MSI_EXCHANGE, fourHeight, DT_LEFT, DT_LEFT, 0xFFFFFFFF, "Market Category");
	AddColumn("#", MSI_ORDINAL, tripleHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Ordinal Number in the List Box");
	AddColumn("Log", MSI_LOG, tripleHeight, DT_CENTER, DT_CENTER, DT_RIGHT, "Log Stock's Imbalances");//, 0xFFFFFFFF, itemHeight, itemHeight);
//NYSE
	AddColumn("NyDelay", MSI_NYSE_IMBALANCE_DELAY, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Info Imbalance Delay In Thread");
	AddColumn("NyImbInfo", MSI_NYSE_IMBALANCE_INFO, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Info Imbalance");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("NyInfoTime", MSI_NYSE_IMBALANCE_INFO_TIME, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Info Imbalance Time");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("NyFar", MSI_NYSE_FAR_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Far Price");
	AddColumn("NyNear", MSI_NYSE_NEAR_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Near Price");
	AddColumn("NyRef", MSI_NYSE_REF_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Current Reference Price");
	AddColumn("NyPaired", MSI_NYSE_PAIRED_SHARES, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Paired Shares");
	
	AddColumn("NyType", MSI_NYSE_CROSS_TYPE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Cross Type");
//NYSE Reg
	AddColumn("NyRDelay", MSI_NYSE_IMBALANCE_DELAY_REGULATORY, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Regulatory Imbalance Delay In Thread");
	AddColumn("NyImbReg", MSI_NYSE_IMBALANCE_REGULATORY, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Regulatory Imbalance");
	AddColumn("NyRegTime", MSI_NYSE_IMBALANCE_REGULATORY_TIME, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Regulatory Imbalance Time");
	AddColumn("NyRFar", MSI_NYSE_FAR_PRICE_REGULATORY, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Regulatory Far Price");
	AddColumn("NyRNear", MSI_NYSE_NEAR_PRICE_REGULATORY, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Regulatory Near Price");
	AddColumn("NyRRef", MSI_NYSE_REF_PRICE_REGULATORY, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Regulatory Current Reference Price");
	AddColumn("NyRPaired", MSI_NYSE_PAIRED_SHARES_REGULATORY, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NYSE (or AMEX or BATS) Regulatory Paired Shares");

//NSDQ
	AddColumn("NqDelay", MSI_NSDQ_IMBALANCE_DELAY, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NSDQ Imbalance Delay In Thread");
	AddColumn("NqImbInfo", MSI_NSDQ_IMBALANCE_INFO, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NSDQ Imbalance");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("NqInfoTime", MSI_NSDQ_IMBALANCE_INFO_TIME, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NSDQ Imbalance Time");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("NqFar", MSI_NSDQ_FAR_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NSDQ Far Price");
	AddColumn("NqNear", MSI_NSDQ_NEAR_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NSDQ Near Price");
	AddColumn("NqRef", MSI_NSDQ_REF_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NSDQ Current Reference Price");
	AddColumn("NqPaired", MSI_NSDQ_PAIRED_SHARES, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NSDQ Paired Shares");
	AddColumn("NqType", MSI_NSDQ_CROSS_TYPE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NSDQ Cross Type");
	AddColumn("NqVar", MSI_NSDQ_PRICE_VARIATION_INDICATOR, tripleHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "NSDQ Price Variation Indicator");

//ARCA
	AddColumn("ArDelay", MSI_ARCA_IMBALANCE_DELAY, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "ARCA Imbalance Delay In Thread");
	AddColumn("ArImbMkt", MSI_ARCA_IMBALANCE_MARKET, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "ARCA Market Imbalance");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("ArImbInfo", MSI_ARCA_IMBALANCE_INFO, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "ARCA Info Imbalance");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("ArInfoTime", MSI_ARCA_IMBALANCE_INFO_TIME, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "ARCA Imbalance Time");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("ArFar", MSI_ARCA_FAR_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "ARCA Far Price");
	AddColumn("ArNear", MSI_ARCA_NEAR_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "ARCA Near Price");
	AddColumn("ArRef", MSI_ARCA_REF_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "ARCA Current Reference Price");
	AddColumn("ArPaired", MSI_ARCA_PAIRED_SHARES, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "ARCA Paired Shares");
	AddColumn("ArType", MSI_ARCA_CROSS_TYPE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "ARCA Cross Type");

	AddColumn("MtDelay", MSI_MAIN_THREAD_IDLE_IMBALANCE_DELAY, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Main Thread Idle Imbalance Delay");

	AddColumn("Yest Volume", MSI_YEST_VOLUME, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Yesterday's volume");
/*
	AddColumn("Bid", MSCI_LEVEL1_BID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level1 Bid");
	AddColumn("Ask", MSCI_LEVEL1_ASK, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level1 Ask");
*/
}

LbHorizontalScroller* ListBoxImbalance::CreateHorizontalScroller()
{
	return new LbImbalanceHorizontalScroller(this);
}

BEGIN_MESSAGE_MAP(ListBoxImbalance, ColumnedListBox)
	ON_MESSAGE(WM_USER + 444, OnItemRepaint)
END_MESSAGE_MAP()

void ListBoxImbalance::ListBoxCreated()
{
	ColumnedListBox::ListBoxCreated();
//	UpdateSymbolColumnRight();

	m_menuWidth = MeasureText("Log", GetMenuFont());
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
		TakionBaseWnd::CalculateTextWidth(rect, hdc, "Log", m_menuWidth);

		if(oldFont)
		{
			::SelectObject(hdc, oldFont);
		}
		::ReleaseDC(m_hWnd, hdc);
	}
*/
}

void ListBoxImbalance::DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->itemID)
	{
		case TMID_HELP_LOG_BOOKMARK:
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, true);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
		break;

		default:
		ColumnedListBox::DrawMenuItem(lpDrawItemStruct);
		break;
	}
}

bool ListBoxImbalance::MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	switch(lpMeasureItemStruct->itemID)
	{
		case TMID_HELP_LOG_BOOKMARK:
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
		lpMeasureItemStruct->itemWidth = m_menuWidth;// mainMessageLoopWnd->GetAddMenuWidth();// + lpMeasureItemStruct->itemHeight;
		break;

		default:
		return ColumnedListBox::MeasureMenuItem(lpMeasureItemStruct);
	}
	return true;
}

void ListBoxImbalance::AddMenuItems(CMenu& menu)
{
//	bool hasItems = m_itemCount >0;//GetCount() > 0;
	const int sel = GetCurSel();
	const bool selected = sel >= 0;
	const StockImbalance* stock = selected ? (const StockImbalance*)GetItemDataPtr(sel) : nullptr;
	if(selected)
	{

	}
//	bool clipboardAvailable = IsClipboardFormatAvailable(CF_TEXT) == TRUE;
//	menu.AppendMenu(MF_STRING|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLIPBOARD, "Copy");
//	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_OWNERDRAW|(stock != nullptr ? MF_ENABLED : MF_GRAYED)|(stock && stock->isLog() ? MF_CHECKED : MF_UNCHECKED), TMID_HELP_LOG_BOOKMARK, "Log");
}

BOOL ListBoxImbalance::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD loword = (DWORD)wParam;//LOWORD(wParam);
    switch(loword)
    {
/*
		case TMID_CLIPBOARD:
		{
			std::string info;
			CopySymbols(info);
			TU_CopyStringToClipboard(info, m_hWnd);
		}
		break;
*/
		case TMID_HELP_LOG_BOOKMARK:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				StockImbalance* stock = (StockImbalance*)GetItemDataPtr(sel);
				if(stock->isLog())
				{
					if(stock->SetLog(false))
					{
						InvalidateItemColumn(sel, MSI_LOG);
						--m_logCount;
						RepaintLogCount();
					}
				}
				else
				{
					if(stock->SetLog(true))
					{
						InvalidateItemColumn(sel, MSI_LOG);
						++m_logCount;
						RepaintLogCount();
					}
				}
			}
		}
		break;
/*
		case TMID_REMOVE:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				StockMarketSorter* stock = (StockMarketSorter*)GetItemDataPtr(sel);
				UpdatePositionTotals(-stock->GetClosedPnl(), -stock->GetOpenPnl(), -stock->GetOpenPnlPrint());
				SignedPrice diff = -SignedPrice(stock->GetRangeAtFirstTimer());
				bool loaded = stock->isLoaded();
				DeleteString(sel);
				for(unsigned int i = (unsigned int)sel, end = m_itemCount; i < end; ++i)
				{
					stock = (StockMarketSorter*)GetItemDataPtr(i);
					stock->DecrementOrdinal();
				}
				RepaintCount();
				UpdateTimerTotals(diff);
				if(loaded && m_rangeStockCount)
				{
					--m_rangeStockCount;
				}
				UpdateAverageRange();
			}
		}
		break;
*/

		default:
		return ColumnedListBox::OnCommand(wParam, lParam);
	}
	return TRUE;
}

LRESULT ListBoxImbalance::OnItemRepaint(WPARAM ordinal, LPARAM columns)
{
	InvalidateItemColumnRange((int)ordinal, (unsigned short)(columns >> 16), (unsigned short)columns);
	return 0;
}

void ListBoxImbalance::RefreshOrdinals()
{
	StockImbalance* stock;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (StockImbalance*)GetItemDataPtr(i);
		stock->SetOrdinal(i);
	}
}

void ListBoxImbalance::IncrementOrdinals(unsigned int i)
{
	unsigned int end = m_itemCount;
	if(i < end)
	{
		StockImbalance* stock;
		for(; i < end; ++i)
		{
			stock = (StockImbalance*)GetItemDataPtr(i);
			stock->IncrementOrdinal();
		}
	}
}

int ListBoxImbalance::AddItem(const StockImbalance* item)
{
	if(FindItemByName(item->GetSymbol()) < 0)
	{
		RepaintTotalCount();
		int ret = ColumnedListBox::AddString((const char*)item);
		if(ret >= 0)
		{
			if(item->isLog())
			{
				++m_logCount;
				RepaintLogCount();
			}
			if(m_itemCount == 1)
			{
				m_owner->EnableFindButton();
			}
		}
		return ret;
	}
	return -1;
}

int ListBoxImbalance::RemoveItem(unsigned int ordinal)
{
	RepaintTotalCount();
	if(ordinal < m_itemCount)
	{
		const StockImbalance* stock = (const StockImbalance*)GetItemDataPtr(ordinal);
		const bool log = stock->isLog();
		int ret = DeleteString(ordinal);
		if(ret != LB_ERR)
		{
			if(log)
			{
				--m_logCount;
				RepaintLogCount();
			}
			m_owner->EnableFindButton();
		}
		return ret;
	}
	return LB_ERR;
}

void ListBoxImbalance::ResetImbalances()
{
	StockImbalance* stock;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (StockImbalance*)GetItemDataPtr(i);
		stock->ResetImbalances();
	}
	Invalidate(FALSE);
}

void ListBoxImbalance::SetInThread(const bool inThread)
{
	if(inThread != m_inThread)
	{
		m_inThread = inThread;
		StockImbalance* stock;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			stock = (StockImbalance*)GetItemDataPtr(i);
			stock->SetInThread(m_inThread);
		}
	}
}

int ListBoxImbalance::AddSecurity(const Security* security)
{
	int found = FindItemByName(security->GetSymbol());
	if(found < 0)
	{
		RepaintTotalCount();
		int ret = ColumnedListBox::AddString((const char*)new StockImbalance(security, m_inThread, this));
		m_owner->EnableFindButton();
		return ret;
	}
	else
	{
		StockImbalance* item = (StockImbalance*)GetItemDataPtr(found);
		item->SetSecurity(security);
		return found;
	}
}

void ListBoxImbalance::RepaintTotalCount() const
{
	m_owner->RepaintTotalCount();
}

void ListBoxImbalance::RepaintLogCount() const
{
	m_owner->RepaintLogCount();
}

bool ListBoxImbalance::CheckOrdinalIntegrity() const
{
	if(m_hWnd)
	{
		StockImbalance* stock;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			stock = (StockImbalance*)GetItemDataPtr(i);
			if(stock->GetOrdinal() != i)
			{
				return false;
			}
		}
	}
	return true;
}

//void ListBoxImbalance::RepaintNyseImbalance(){m_owner->RepaintNyseImbalance();}
void ListBoxImbalance::SetMarketSorterLoaded(const bool loaded)
{
	if(m_marketSorterLoaded != loaded)
	{
		m_marketSorterLoaded = loaded;
		SetInvalid(!m_marketSorterLoaded);
		if(m_hWnd)
		{
			StockImbalance* stock;
			unsigned int i = 0, end = m_itemCount;
			for(; i < end; ++i)
			{
				stock = (StockImbalance*)GetItemDataPtr(i);
				stock->MarketSorterConnected(m_marketSorterConnected);
			}
			if(m_marketSorterLoaded)
			{
//				m_closedPnl.SetZero();
//				m_openPnl.SetZero();
//				m_openPrintPnl.SetZero();

				for(i = 0; i < end; ++i)
				{
					stock = (StockImbalance*)GetItemDataPtr(i);
//					stock->MarketSorterConnected(m_marketSorterConnected);

//					m_closedPnl += stock->GetClosedPnl();
//					m_openPnl += stock->GetOpenPnl();
//					m_openPrintPnl += stock->GetOpenPnlPrint();
				}
			}
		}
		if(m_marketSorterLoaded)
		{
//Sanity check
			CheckOrdinalIntegrity();
		}
	}
}

void ListBoxImbalance::MarketSorterCleanup()
{
	StockImbalance* stock;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (StockImbalance*)GetItemDataPtr(i);
		stock->MarketSorterCleanup();
	}
}

void ListBoxImbalance::SetMarketSorterConnected(const bool connected)
{
	if(connected != m_marketSorterConnected)
	{
		m_marketSorterConnected = connected;
		if(!m_marketSorterConnected)
		{
			SetMarketSorterLoaded(false);
		}
	}
}

void ListBoxImbalance::UpdateStockAccount(StockImbalance* stock)
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

void ListBoxImbalance::CurrentAccountSet(Account* account)
{
	if(m_currentAccount != account)
	{
		m_currentAccount = account;
		if(m_hWnd)
		{
			StockImbalance* stock;
			for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
			{
				stock = (StockImbalance*)GetItemDataPtr(i);
				UpdateStockAccount(stock);
			}
		}
	}
}

void ListBoxImbalance::DestroyItem(const void* item) const
{
	delete (const StockImbalance*)item;
}

const char* ListBoxImbalance::GetItemName(const void* item) const
{
	return ((const StockImbalance*)item)->GetSymbol();
}

void ListBoxImbalance::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
	const StockImbalance* imbalanceItem = (const StockImbalance*)item;
	static const char y = 'Y';
//We don't have to lock to paint permanent items
	switch(columnId)
	{
		case MSI_SYMBOL:
		DrawText(hDC, imbalanceItem->GetSymbol(), -1, &r, alignment);
		return;

		case MSI_EXCHANGE:
		{
			const char* text;
			switch(imbalanceItem->GetMarketCategory())
			{
				case MC_OTHR:
				text = "Other";
				break;

				case MC_AMEX:
				text = "AMEX";
				break;

				case MC_NYSE:
				text = "NYSE";
				break;

				case MC_ARCA:
				text = "ARCA";
				break;

				case MC_NSDQ_SELECT:
				text = "NSDQ Sel";
				break;

				case MC_NSDQ_GLOBAL:
				text = "NSDQ Glo";
				break;

				case MC_NSDQ_CAPITAL:
				text = "NSDQ Cap";
				break;

				case MC_OTCB:
				text = "OTCB";
				break;

				case MC_BATS:
				text = "BATS";
				break;

				case MC_IEX:
				text = "IEX";
				break;

				default:
				text = nullptr;
				break;
			}
			if(text)DrawText(hDC, text, -1, &r, alignment);
		}
		return;

		case MSI_ORDINAL:
		if(imbalanceItem->GetOrdinal() != 0xFFFFFFFF)
		{
			char num[33];
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(imbalanceItem->GetOrdinal(), num, sizeof(num)), -1, &r, alignment);
		}
		return;

		case MSI_LOG:
		if(imbalanceItem->isLog())
		{
			DrawText(hDC, &y, 1, &r, alignment);
		}
		return;

		case MSI_MAIN_THREAD_IDLE_IMBALANCE_DELAY:
		if(imbalanceItem->GetMainThreadDelay())
		{
			char num[33];
			DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetMainThreadDelay(), num, sizeof(num)), -1, &r, alignment);
		}
		return;

		case MSI_YEST_VOLUME:
		{
			char num[33];
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(imbalanceItem->GetYestVolume(), num, sizeof(num)), -1, &r, alignment);
		}
		return;
	}
	if(!m_inThread || imbalanceItem->LockInquiry())//If LockInquiry returns false, it means that currently the StockImbalance object is being modified and it will be signaled to repaint after the modification is done
	{
		std::string info;
		char num[33];
		switch(columnId)
		{
//NYSE
			case MSI_NYSE_IMBALANCE_DELAY:
			if(imbalanceItem->GetNyseInThreadDelay())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetNyseInThreadDelay(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_IMBALANCE_INFO://NYSE info imbalance (as opposed to regulatory imbalance).
			if(imbalanceItem->GetNyseImbalanceInfo())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetNyseImbalanceInfo(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_IMBALANCE_INFO_TIME:
			if(imbalanceItem->GetNyseImbalanceInfoTime())
			{
				info.clear();
				U_AppendMillisecond(info, imbalanceItem->GetNyseImbalanceInfoTime(), true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_FAR_PRICE:
			if(!imbalanceItem->GetNyseFarPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetNyseFarPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_NEAR_PRICE:
			if(!imbalanceItem->GetNyseNearPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetNyseNearPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_REF_PRICE:
			if(!imbalanceItem->GetNyseCurrentReferencePrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetNyseCurrentReferencePrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_PAIRED_SHARES:
			if(imbalanceItem->GetNysePairedShares())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(imbalanceItem->GetNysePairedShares(), num, sizeof(num)), -1, &r, alignment);
			}
			break;
//NYSE Reg
			case MSI_NYSE_IMBALANCE_DELAY_REGULATORY:
			if(imbalanceItem->GetNyseRegInThreadDelay())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetNyseRegInThreadDelay(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_IMBALANCE_REGULATORY:
			if(imbalanceItem->GetNyseImbalanceReg())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetNyseImbalanceReg(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_IMBALANCE_REGULATORY_TIME:
			if(imbalanceItem->GetNyseImbalanceRegTime())
			{
				info.clear();
				U_AppendMillisecond(info, imbalanceItem->GetNyseImbalanceRegTime(), true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_FAR_PRICE_REGULATORY:
			if(!imbalanceItem->GetNyseRegFarPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetNyseRegFarPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_NEAR_PRICE_REGULATORY:
			if(!imbalanceItem->GetNyseRegNearPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetNyseRegNearPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_REF_PRICE_REGULATORY:
			if(!imbalanceItem->GetNyseRegCurrentReferencePrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetNyseCurrentReferencePrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_PAIRED_SHARES_REGULATORY:
			if(imbalanceItem->GetNyseRegPairedShares())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(imbalanceItem->GetNyseRegPairedShares(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_NYSE_CROSS_TYPE:
			if(imbalanceItem->GetNyseCrossType())
			{
				DrawText(hDC, &(char&)(imbalanceItem->GetNyseCrossType()), 1, &r, alignment);
			}
			break;
//NSDQ
			case MSI_NSDQ_IMBALANCE_DELAY:
			if(imbalanceItem->GetNsdqInThreadDelay())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetNsdqInThreadDelay(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_NSDQ_IMBALANCE_INFO:
			if(imbalanceItem->GetNsdqImbalanceInfo())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetNsdqImbalanceInfo(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_NSDQ_IMBALANCE_INFO_TIME:
			if(imbalanceItem->GetNsdqImbalanceInfoTime())
			{
				info.clear();
				U_AppendMillisecond(info, imbalanceItem->GetNsdqImbalanceInfoTime(), true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NSDQ_FAR_PRICE:
			if(!imbalanceItem->GetNsdqFarPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetNsdqFarPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NSDQ_NEAR_PRICE:
			if(!imbalanceItem->GetNsdqNearPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetNsdqNearPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NSDQ_REF_PRICE:
			if(!imbalanceItem->GetNsdqCurrentReferencePrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetNsdqCurrentReferencePrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_NSDQ_PAIRED_SHARES:
			if(imbalanceItem->GetNsdqPairedShares())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(imbalanceItem->GetNsdqPairedShares(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_NSDQ_CROSS_TYPE:
			if(imbalanceItem->GetNsdqCrossType())
			{
				DrawText(hDC, &(char&)(imbalanceItem->GetNsdqCrossType()), 1, &r, alignment);
			}
			break;

			case MSI_NSDQ_PRICE_VARIATION_INDICATOR:
			if(imbalanceItem->GetNsdqPriceVariationIndicator())
			{
				DrawText(hDC, &(char&)(imbalanceItem->GetNsdqPriceVariationIndicator()), 1, &r, alignment);
			}
			break;
//ARCA
			case MSI_ARCA_IMBALANCE_DELAY:
			if(imbalanceItem->GetArcaInThreadDelay())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetArcaInThreadDelay(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_ARCA_IMBALANCE_MARKET:
			if(imbalanceItem->GetArcaImbalanceInfo())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetArcaImbalanceInfo(), num, sizeof(num)), -1, &r, alignment);
			}
			break;
			break;

			case MSI_ARCA_IMBALANCE_INFO:
			if(imbalanceItem->GetArcaImbalanceInfo())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(imbalanceItem->GetArcaImbalanceInfo(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_ARCA_IMBALANCE_INFO_TIME:
			if(imbalanceItem->GetArcaImbalanceInfoTime())
			{
				info.clear();
				U_AppendMillisecond(info, imbalanceItem->GetArcaImbalanceInfoTime(), true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_ARCA_FAR_PRICE:
			if(!imbalanceItem->GetArcaFarPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetArcaFarPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_ARCA_NEAR_PRICE:
			if(!imbalanceItem->GetArcaNearPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetArcaNearPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_ARCA_REF_PRICE:
			if(!imbalanceItem->GetArcaCurrentReferencePrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, imbalanceItem->GetArcaCurrentReferencePrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case MSI_ARCA_PAIRED_SHARES:
			if(imbalanceItem->GetArcaPairedShares())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(imbalanceItem->GetArcaPairedShares(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case MSI_ARCA_CROSS_TYPE:
			if(imbalanceItem->GetArcaCrossType())
			{
				DrawText(hDC, &(char&)(imbalanceItem->GetArcaCrossType()), 1, &r, alignment);
			}
			break;

		}
		if(m_inThread)imbalanceItem->UnlockInquiry();
	}
}


enum SampleSettingMarketSorterImbalanceIds
{
	listBoxImbalanceId = 10000,
//	loadOnStartCheckBoxId,
	inThreadCheckBoxId,
	sendToMmCheckBoxId,

	staticSymbolId,
	editSymbolId,
	buttonFindId,
	
	buttonResetId,

	buttonLoadId,

	groupFirstCharId,
	staticCharFromId,
	comboBoxCharFromId,
	staticCharToId,
	comboBoxCharToId,

	groupYestVolumeId,
	spinYestVolumeFromId,
	spinYestVolumeToId,

	staticExchangeId,
	listBoxExchangeId,

	staticTestId,
	listBoxTestId,

	marketSorterImbalanceIdsCount

};

SampleDialogSettingMarketSorterImbalance::SampleDialogSettingMarketSorterImbalance(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, Account* account):
	BaseDialogSetting(mainWnd, parentTab, "Imbalance"),
	m_currentAccount(account),
	m_ListBoxImbalance(this, m_currentAccount),
	m_CheckBoxInThread("Process Stock Updates in the Worker Thread", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxDblClkSendSymbolToMm("Double clicking on a Stock in the ListBox populates Takion's active MmBox", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_EditSymbol(U_GetValidStockChars(), false),
	m_ListBoxTestOrReal(2),

	m_SpinYestVolumeFrom(90000000000ui64, 0, 0, 1000, 3, TakionSpin::charComma), 
	m_SpinYestVolumeTo(90000000000ui64, 0, 0, 1000, 3, TakionSpin::charComma), 

	m_marketSorterConnected(false),
	m_marketSorterLoaded(false),

	m_listBoxBorderSize2(0),
	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),

	m_marketCategoryFilter((1 << MCF_COUNT) - 1),
	m_testStockFilter((1 << STTR_Count) - 1),//1 - real; 2 - test; 3 - both

	m_yestVolumeFrom(0),
	m_yestVolumeTo(0),

	m_charFrom('A'),
	m_charTo('Z'),

	m_initialSearchDone(false),
	m_inThread(false),
	m_sendToMm(false),

	m_allStockterator(TD_CreateEquityIterator())
{
	m_ListBoxTestOrReal.AddStringItem("Real", 0);
	m_ListBoxTestOrReal.AddStringItem("Test", 1);

	m_ComboBoxCharFrom.SetColorInvalid(m_visualBase->GetWarningColor());
	m_ComboBoxCharTo.SetColorInvalid(m_visualBase->GetWarningColor());

	AddVolumeSpinCells(m_SpinYestVolumeFrom);
	AddVolumeSpinCells(m_SpinYestVolumeTo);

	int x = hgap;
	int xc = x + hgap;
	int y = vgap;
	int width = 88;
	int width1 = 55;
	int width2 = (width - hgap) / 2;
	int widthG = width - hgap - hgap;
	int widthG2 = (widthG - hgap) / 2;

	AddButtonIndirect("In Thread", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y, width2, checkBoxHeight, inThreadCheckBoxId);

	AddButtonIndirect("Dbl Clk Send To MM", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y += checkBoxHeight + vgap, width, checkBoxHeight, sendToMmCheckBoxId);

	AddStaticIndirect("Symbol to find", WS_VISIBLE | SS_NOPREFIX, 0, x, y += checkBoxHeight + 4 * vgap, width, staticHeight, staticSymbolId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, x, y += staticHeight, width1, editHeight, editSymbolId);
	AddButtonIndirect("Find", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x + width1 + hgap, y, width - x - width1 - hgap, comboBoxHeight, buttonFindId);

	AddButtonIndirect("Reset", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += comboBoxHeight + 8 * vgap, width2, comboBoxHeight, buttonResetId);

	AddButtonIndirect("Load", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x + width - width2, y, width2, comboBoxHeight, buttonLoadId);

	const int charGroupHeight = 32;
	AddButtonIndirect("First Char Filter", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y += comboBoxHeight + vgap, width, charGroupHeight, groupFirstCharId);
	AddStaticIndirect("From", WS_VISIBLE | SS_NOPREFIX, 0, xc, y + groupOffset, widthG2, staticHeight, staticCharFromId);
	AddStaticIndirect("To", WS_VISIBLE | SS_NOPREFIX, 0, xc + widthG2 + hgap, y + groupOffset, widthG2, staticHeight, staticCharToId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y + groupOffset + staticHeight, widthG2, 400, comboBoxCharFromId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc + widthG2 + hgap, y + groupOffset + staticHeight, widthG2, 400, comboBoxCharToId);

	const int yestVolumeGroupHeight = 36;
	AddButtonIndirect("Yesterday Volume Filter", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y += charGroupHeight + vgap, width, yestVolumeGroupHeight, groupYestVolumeId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y + groupOffset, widthG, spinHeight, spinYestVolumeFromId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y + groupOffset + spinHeight + vgap, widthG, spinHeight, spinYestVolumeToId);

	const int itemHeight = 8;
	AddStaticIndirect("Exchange Filter", WS_VISIBLE | SS_NOPREFIX, 0, x, y += yestVolumeGroupHeight + vgap, width, staticHeight, staticExchangeId);
	const int exchangeLbHeight = 9 * itemHeight + 4;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y += staticHeight, width, exchangeLbHeight, listBoxExchangeId);

	AddStaticIndirect("Test Stock Filter", WS_VISIBLE | SS_NOPREFIX, 0, x, y += exchangeLbHeight + vgap, width, staticHeight, staticTestId);
	const int testLbHeight = 2 * itemHeight + 4;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y += staticHeight, width, testLbHeight, listBoxTestId);

	y = vgap + staticHeight;
	x += width + hgap;
	width = m_width - x;

	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, width, m_height - staticHeight - y, listBoxImbalanceId);
	AddListBox(&m_ListBoxImbalance);
	AddListBox(&m_ListBoxMarketCategory);
	AddListBox(&m_ListBoxTestOrReal);

	SetMarketSorterConnected(TD_IsMarketSorterConnected());
	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());

//	m_ListBoxImbalance.SetInvalid(!m_marketSorterLoaded);
}

SampleDialogSettingMarketSorterImbalance::~SampleDialogSettingMarketSorterImbalance()
{
	delete m_allStockterator;
}

//void SampleDialogSettingMarketSorterImbalance::RepaintNyseImbalance() const{InvalidateTotalValueRect(listBoxImbalanceId, MSI_NYSE_IMBALANCE);}
void SampleDialogSettingMarketSorterImbalance::RepaintTotalCount() const
{
	InvalidateTotalValueRect(listBoxImbalanceId, MSI_SYMBOL);
}

void SampleDialogSettingMarketSorterImbalance::RepaintLogCount() const
{
	InvalidateTotalValueRect(listBoxImbalanceId, MSI_LOG);
}

void SampleDialogSettingMarketSorterImbalance::AddVolumeSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(100000000, true);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}

void SampleDialogSettingMarketSorterImbalance::BeforeDestroy()
{
	TD_GetAdminObservable().RemoveObserver(this);
	TD_GetNewEquityObservable().RemoveObserver(this);
	TD_GetNewPositionObservable().RemoveObserver(this);

	ResetOffsets();

	BaseDialogSetting::BeforeDestroy();
}

void SampleDialogSettingMarketSorterImbalance::ResetOffsets()
{
	m_listBoxBorderSize2 = 0;
	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;

	m_initialSearchDone = false;

	m_inThread = false;
	m_sendToMm = false;
	m_charFrom = 'A';
	m_charTo = 'Z';

	m_yestVolumeFrom = 0;
	m_yestVolumeTo = 0;
/*
	m_stockWidth = 0;
	m_stockLeftOffset = 0;
	m_stockTopOffset = 0;
	m_maxStockHeight = 0;
	m_stockHeight = 0;
	m_storyWidth = 0;
	m_storyTopOffset = 0;
	m_stockCountRect.SetRectEmpty();

	m_currentSearchType = m_searchType = SEA_ALL;
	m_allEntitled = true;
	m_showStoryChain = false;
	m_selectedHeadlineStoryDisplayed = false;
//	m_currentDaysBack =
	m_daysBack = 0;
//	m_currentHeadlineCount =
	m_headlineCount = 100;
	m_currentTopicOrdinal = m_topicOrdinal = 0xFFFF;
*/
}

void SampleDialogSettingMarketSorterImbalance::ToolTipsEnabled(bool enable)
{
	BaseDialogSetting::ToolTipsEnabled(enable);
	m_ListBoxImbalance.EnableTooltips(enable, m_toolTip);
	m_CheckBoxInThread.EnableTooltips(enable, m_toolTip);
	m_CheckBoxDblClkSendSymbolToMm.EnableTooltips(enable, m_toolTip);
	m_EditSymbol.EnableTooltips(enable, m_toolTip);
	m_ListBoxTestOrReal.EnableTooltips(enable, m_toolTip);
}

BOOL SampleDialogSettingMarketSorterImbalance::OnInitDialog()
{
	TD_GetAdminObservable().AddObserver(this);
	TD_GetNewEquityObservable().AddObserver(this);
	TD_GetNewPositionObservable().AddObserver(this);

	SetMarketSorterConnected(TD_IsMarketSorterConnected());
	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());

	ResetOffsets();

	m_ListBoxImbalance.SetInvalid(!m_marketSorterLoaded);

	BOOL ret = BaseDialogSetting::OnInitDialog();

	m_CheckBoxInThread.ResizeToFitText(0);
	m_CheckBoxDblClkSendSymbolToMm.ResizeToFitText(0);

	m_EditSymbol.SetLimitText(7);

	m_ComboBoxCharFrom.FillCharRange('A', 'Z', false);
	m_ComboBoxCharTo.FillCharRange('A', 'Z', false);

	m_ComboBoxCharFrom.SetCurSel(m_ComboBoxCharFrom.FindItem((const void*)m_charFrom));
	m_ComboBoxCharTo.SetCurSel(m_ComboBoxCharTo.FindItem((const void*)m_charTo));

	SetToolTipsEnabled(true);
	AddTitle(&m_ListBoxImbalance);

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingMarketSorterImbalance::CurrentAccountSet(Account* account)
{
	if(m_currentAccount != account)
	{
		m_currentAccount = account;
		m_ListBoxImbalance.CurrentAccountSet(m_currentAccount);
	}
}

HWND SampleDialogSettingMarketSorterImbalance::GetFirstTabControl() const
{
	return m_ListBoxImbalance.m_hWnd;
}

void SampleDialogSettingMarketSorterImbalance::RestoreLayout()
{
//	bool loadOnStart = extensionConfig->m_miLoadOnStart.GetValue();
//	m_CheckBoxLoadOnStart.SetCheck(loadOnStart ? BST_CHECKED : BST_UNCHECKED);

	const bool inThread = extensionConfig->m_miInThread.GetValue();
	m_CheckBoxInThread.SetCheck(inThread ? BST_CHECKED : BST_UNCHECKED);
	SetInThread(inThread);

	const bool sendToMm = extensionConfig->m_miSendToMm.GetValue();
	m_CheckBoxDblClkSendSymbolToMm.SetCheck(sendToMm ? BST_CHECKED : BST_UNCHECKED);
	SetSendToMm(sendToMm);

	m_ListBoxMarketCategory.SetFilter(extensionConfig->m_miMarketCategoryFilter.GetValue());
	m_ListBoxTestOrReal.SetFilter(extensionConfig->m_miTestStockFilter.GetValue());

	char charFrom = extensionConfig->m_miCharFrom.GetValue();
	if(charFrom < 'A' || charFrom > 'Z')charFrom = 'A';
	m_ComboBoxCharFrom.SetCurSel(m_ComboBoxCharFrom.FindItem((const void*)charFrom));

	char charTo = extensionConfig->m_miCharTo.GetValue();
	if(charTo < 'A' || charTo > 'Z')charTo = 'Z';
	m_ComboBoxCharTo.SetCurSel(m_ComboBoxCharTo.FindItem((const void*)charTo));

	m_SpinYestVolumeFrom.SetValue(extensionConfig->m_miYestVolumeFrom.GetValue(), false, true, false);

	m_SpinYestVolumeTo.SetValue(extensionConfig->m_miYestVolumeTo.GetValue(), false, true, false);

	Load();

	EnableLoadButton();
	m_modified = false;
}

void SampleDialogSettingMarketSorterImbalance::UpdateSettings()
{
//	extensionConfig->m_miLoadOnStart.SetValue(m_CheckBoxLoadOnStart.GetCheck() == BST_CHECKED);
	extensionConfig->m_miInThread.SetValue(m_CheckBoxInThread.GetCheck() == BST_CHECKED);
	extensionConfig->m_miSendToMm.SetValue(m_CheckBoxDblClkSendSymbolToMm.GetCheck() == BST_CHECKED);
	extensionConfig->m_miMarketCategoryFilter.SetValue(m_ListBoxMarketCategory.GetFilter());
	extensionConfig->m_miTestStockFilter.SetValue(m_ListBoxTestOrReal.GetFilter());

	int sel = m_ComboBoxCharFrom.GetCurSel();
	extensionConfig->m_miCharFrom.SetValue(sel >= 0 ? (char)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		m_ComboBoxCharFrom.GetItemDataPtr(sel) : 'A');

	sel = m_ComboBoxCharTo.GetCurSel();
	extensionConfig->m_miCharTo.SetValue(sel >= 0 ? (char)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		m_ComboBoxCharTo.GetItemDataPtr(sel) : 'Z');

	extensionConfig->m_miYestVolumeFrom.SetValue(m_SpinYestVolumeFrom.GetValue());
	extensionConfig->m_miYestVolumeTo.SetValue(m_SpinYestVolumeTo.GetValue());
}

void SampleDialogSettingMarketSorterImbalance::OnInThread()
{
	SetInThread(m_CheckBoxInThread.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingMarketSorterImbalance::OnSendSymbolToMm()
{
	SetSendToMm(m_CheckBoxDblClkSendSymbolToMm.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingMarketSorterImbalance::SetSendToMm(const bool sendToMm)
{
	if(sendToMm != m_sendToMm)
	{
		m_sendToMm = sendToMm;
		m_modified = true;
	}
}

void SampleDialogSettingMarketSorterImbalance::SetInThread(const bool inThread)
{
	if(inThread != m_inThread)
	{
		m_inThread = inThread;
		m_ListBoxImbalance.SetInThread(m_inThread);
		m_modified = true;
	}
}

void SampleDialogSettingMarketSorterImbalance::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
	DDX_Control(pDX, listBoxImbalanceId, m_ListBoxImbalance);
//	DDX_Control(pDX, loadOnStartCheckBoxId, m_CheckBoxLoadOnStart);
	DDX_Control(pDX, inThreadCheckBoxId, m_CheckBoxInThread);
	DDX_Control(pDX, sendToMmCheckBoxId, m_CheckBoxDblClkSendSymbolToMm);
	DDX_Control(pDX, editSymbolId, m_EditSymbol);
	DDX_Control(pDX, buttonFindId, m_ButtonFind);
	DDX_Control(pDX, buttonResetId, m_ButtonReset);
	DDX_Control(pDX, buttonLoadId, m_ButtonLoad);

	DDX_Control(pDX, comboBoxCharFromId, m_ComboBoxCharFrom);
	DDX_Control(pDX, comboBoxCharToId, m_ComboBoxCharTo);

	DDX_Control(pDX, spinYestVolumeFromId, m_SpinYestVolumeFrom);
	DDX_Control(pDX, spinYestVolumeToId, m_SpinYestVolumeTo);

	DDX_Control(pDX, listBoxExchangeId, m_ListBoxMarketCategory);
	DDX_Control(pDX, staticTestId, m_StaticTestOrReal);

	DDX_Control(pDX, listBoxTestId, m_ListBoxTestOrReal);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingMarketSorterImbalance, BaseDialogSetting)
//	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(inThreadCheckBoxId, OnInThread)
	ON_BN_CLICKED(sendToMmCheckBoxId, OnSendSymbolToMm)
	ON_LBN_SELCHANGE(listBoxImbalanceId, OnSelchangeImbalance)
	ON_LBN_DBLCLK(listBoxImbalanceId, OnDblClkImbalance)
	ON_BN_CLICKED(buttonFindId, OnFind)
	ON_BN_CLICKED(buttonResetId, OnReset)
	ON_BN_CLICKED(buttonLoadId, OnLoad)
	ON_CBN_SELCHANGE(comboBoxCharFromId, OnSelchangeCharFrom)
	ON_CBN_SELCHANGE(comboBoxCharToId, OnSelchangeCharTo)
	ON_EN_CHANGE(editSymbolId, OnSymbolChange)
	ON_EN_UPDATE(editSymbolId, OnSymbolUpdate)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)//one item is checked/unchecked
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//multiple items checked/unchecked
END_MESSAGE_MAP()

void SampleDialogSettingMarketSorterImbalance::DoEnableLoadButton(bool enable)
{
	EnableControl(m_ButtonLoad.m_hWnd, enable, m_ListBoxImbalance.m_hWnd);
	EnableControl(m_ButtonReset.m_hWnd, enable, m_ListBoxImbalance.m_hWnd);
}

void SampleDialogSettingMarketSorterImbalance::EnableLoadButton()
{
	DoEnableLoadButton(isFilterChanged());
}

void SampleDialogSettingMarketSorterImbalance::OnSelchangeCharFrom()
{
	SelchangeChar();//m_ComboBoxCharFrom, m_charFrom);
}

void SampleDialogSettingMarketSorterImbalance::OnSelchangeCharTo()
{
	SelchangeChar();//m_ComboBoxCharTo, m_charTo);
}

void SampleDialogSettingMarketSorterImbalance::SelchangeChar()//const ComboBoxChar& comboBox, char& value)
{
	bool invalid = GetCtrlCharFrom() > GetCtrlCharTo();
	m_ComboBoxCharFrom.SetInvalid(invalid);
	m_ComboBoxCharTo.SetInvalid(invalid);
	EnableLoadButton();
}

LRESULT SampleDialogSettingMarketSorterImbalance::OnListBoxChecked(WPARAM listBoxId, LPARAM l)
{
	switch(listBoxId)
	{
		case listBoxExchangeId:
		m_ListBoxMarketCategory.InvalidateClient();
		if(m_marketCategoryFilter != m_ListBoxMarketCategory.GetFilter())DoEnableLoadButton(true);
		else EnableLoadButton();
//		if(((const StockFilter*)m_filter)->GetMarketCategoryFilter() != m_ListBoxMarketCategory.GetFilter())DoEnableOkButton(true);
//		else EnableOkButton();
		m_modified = true;
		break;

		case listBoxTestId:
		m_ListBoxTestOrReal.InvalidateClient();
		if(m_testStockFilter != m_ListBoxTestOrReal.GetFilter())DoEnableLoadButton(true);
		else EnableLoadButton();
//		if(((const StockFilter*)m_filter)->GetTestStockFilter() != m_ListBoxTestOrReal.GetFilter())DoEnableOkButton(true);
//		else EnableOkButton();
		m_modified = true;
		break;
	}
	return 0;
}

bool SampleDialogSettingMarketSorterImbalance::isFilterChanged() const//0 - not changed; 1 - requires add; 2 - requires remove; 3 - requires add and remove stocks
{
/*
	unsigned int m_marketCategoryFilter;
	unsigned int m_testStockFilter;

	unsigned __int64 m_yestVolumeFrom;
	unsigned __int64 m_yestVolumeTo;

	char m_charFrom;
	char m_charTo
*/
	return m_ListBoxMarketCategory.GetFilter() != m_marketCategoryFilter
		|| m_ListBoxTestOrReal.GetFilter() != m_testStockFilter
		|| GetCtrlCharFrom() != m_charFrom
		|| GetCtrlCharTo() != m_charTo
		|| m_SpinYestVolumeFrom.GetValue() != m_yestVolumeFrom
		|| m_SpinYestVolumeTo.GetValue() != m_yestVolumeTo
		;
}

char SampleDialogSettingMarketSorterImbalance::GetCtrlCharFrom() const
{
	int sel = m_ComboBoxCharFrom.GetCurSel();
	return sel >= 0 ? (char)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		m_ComboBoxCharFrom.GetItemDataPtr(sel) : 'A';
}

char SampleDialogSettingMarketSorterImbalance::GetCtrlCharTo() const
{
	int sel = m_ComboBoxCharTo.GetCurSel();
	return sel >= 0 ? (char)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		m_ComboBoxCharTo.GetItemDataPtr(sel) : 'Z';
}

void SampleDialogSettingMarketSorterImbalance::OnLoad()//0 - not changed; 1 - requires add; 2 - requires remove; 3 - requires add and remove stocks
{
	Load();
}

bool SampleDialogSettingMarketSorterImbalance::SecurityBelongs(const Security* security) const
{
	if(!(security->GetMarketCategoryFilter() & m_marketCategoryFilter))
	{
		return false;
	}
	if(m_testStockFilter != (1 << STTR_Count) - 1 && !(security->isTest() ? (m_testStockFilter & (1 << STTR_TEST)) : (m_testStockFilter & (1 << STTR_REAL))))
	{
		return false;
	}
	const char firstChar = *security->GetSymbol();
#ifdef _DEBUG
if(firstChar == 'R')
{
printf("X");
}
#endif
	if(firstChar < m_charFrom || firstChar > m_charTo)
	{
		return false;
	}

	const unsigned __int64& yestVolume = security->GetYesterdaysVolume();
	if(yestVolume < m_yestVolumeFrom || yestVolume > m_yestVolumeTo)
	{
		return false;
	}
	return true;
}

void SampleDialogSettingMarketSorterImbalance::Load()//0 - not changed; 1 - requires add; 2 - requires remove; 3 - requires add and remove stocks
{
	unsigned char changed = 0;
	const unsigned int marketCategoryFilter = m_ListBoxMarketCategory.GetFilter();
	if(marketCategoryFilter != m_marketCategoryFilter)
	{
		const unsigned int u = m_marketCategoryFilter | marketCategoryFilter;
		if(u != m_marketCategoryFilter)changed |= 1;//add
		if(u != marketCategoryFilter)changed |= 2;//remove

		m_marketCategoryFilter = marketCategoryFilter;
	}
	const unsigned int testFilter = m_ListBoxTestOrReal.GetFilter();
	if(testFilter != m_testStockFilter)
	{
		const unsigned int u = m_testStockFilter | testFilter;
		if(u != m_testStockFilter)changed |= 1;//add
		if(u != testFilter)changed |= 2;//remove

		m_testStockFilter = testFilter;
	}

	bool invalidOld = m_charFrom > m_charTo;

	const char charFrom = GetCtrlCharFrom();
	const char charTo = GetCtrlCharTo();
	bool invalidNew = charFrom > charTo;
	if(invalidOld)
	{
		changed &= ~2;//!remove
		if(invalidNew)changed = 0;
		else changed |= 1;//add
	}
	else if(invalidNew)
	{
		changed &= ~1;//!add
		changed |= 2;//remove
	}
	else
	{
		if(charFrom != m_charFrom)
		{
			if(charFrom < m_charFrom)changed |= 1;//add
			else if(charFrom > m_charFrom)changed |= 2;//remove
		}
		if(charTo != m_charTo)
		{
			if(charTo > m_charTo)changed |= 1;//add
			else if(charTo < m_charTo)changed |= 2;//remove
		}
	}
	m_charFrom = charFrom;
	m_charTo = charTo;

//	invalidOld = m_yestVolumeTo && m_yestVolumeTo < m_yestVolumeFrom;
	invalidOld = m_yestVolumeTo < m_yestVolumeFrom;
	const unsigned __int64 yestVolumeFrom = m_SpinYestVolumeFrom.GetValue();
	const unsigned __int64 yestVolumeTo = m_SpinYestVolumeTo.GetValue();
//	invalidNew = yestVolumeTo && yestVolumeTo < yestVolumeFrom;
	invalidNew = yestVolumeTo < yestVolumeFrom;
	if(invalidOld)
	{
		changed &= ~2;//!remove
		if(invalidNew)changed = 0;
		else changed |= 1;//add
	}
	else if(invalidNew)
	{
		changed &= ~1;//!add
		changed |= 2;//remove
	}
	else
	{
		if(yestVolumeFrom != m_yestVolumeFrom)
		{
			if(yestVolumeFrom < m_yestVolumeFrom)changed |= 1;//add
			else if(yestVolumeFrom > m_yestVolumeFrom)changed |= 2;//remove
		}
		if(yestVolumeTo != m_yestVolumeTo)
		{
			if(yestVolumeTo > m_yestVolumeTo)changed |= 1;//add
			else if(yestVolumeTo < m_yestVolumeTo)changed |= 2;//remove
		}
	}
	m_yestVolumeFrom = yestVolumeFrom;
	m_yestVolumeTo = yestVolumeTo;

	if(changed)
	{
		if(changed & 2)//remove
		{
			for(unsigned int i = 0, end = m_ListBoxImbalance.GetItemCount(); i < end;)
			{
				if(SecurityBelongs(((const StockImbalance*)m_ListBoxImbalance.GetItemDataPtr(i))->GetSecurity()))
				{
					++i;
				}
				else
				{
					m_ListBoxImbalance.RemoveItem(i);
					--end;
				}
			}
		}
		if((changed & 1) || !m_initialSearchDone)//add
		{
			m_initialSearchDone = true;

			TD_LockEquityStorage();
			m_allStockterator->Reset();
			Security* security;
			while(security = TD_GetNextEquity(m_allStockterator))
			{
				AddSecurityIfBelongs(security);
			}
			TD_UnlockEquityStorage();
		}
		m_ListBoxImbalance.RefreshOrdinals();

		EnableLoadButton();
	}
}

int SampleDialogSettingMarketSorterImbalance::AddSecurityIfBelongs(const Security* security)
{
	if(SecurityBelongs(security))
	{
		return m_ListBoxImbalance.AddSecurity(security);
	}
	return -1;
}

void SampleDialogSettingMarketSorterImbalance::AddSecurityIfBelongsAndUpdateOrdinals(const Security* security)
{
	int ins = AddSecurityIfBelongs(security);
	if(ins >= 0)
	{
		unsigned int i = (unsigned int)ins;
		StockImbalance* stockImbalance = (StockImbalance*)m_ListBoxImbalance.GetItemDataPtr(i);
		if(stockImbalance->SetOrdinal(i))
		{
			m_ListBoxImbalance.IncrementOrdinals(i + 1);
		}
	}
}

void SampleDialogSettingMarketSorterImbalance::YestVolumeChange()
{
//	const unsigned __int64 volTo = m_SpinYestVolumeTo.GetValue();
//	bool invalid = volTo != 0 && m_SpinYestVolumeFrom.GetValue() > volTo;
//	bool invalid = m_SpinYestVolumeFrom.GetValue() > m_SpinYestVolumeTo.GetValue();
//	COLORREF color = volTo != 0 && m_SpinYestVolumeFrom.GetValue() > volTo ? m_takionVisual->GetWarningColor() : GetSysColor(COLOR_WINDOW);
	COLORREF color = m_SpinYestVolumeFrom.GetValue() > m_SpinYestVolumeTo.GetValue() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW);
	m_SpinYestVolumeFrom.SetBkColorEnabled(color);
	m_SpinYestVolumeTo.SetBkColorEnabled(color);
	m_modified = true;
}

LRESULT SampleDialogSettingMarketSorterImbalance::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinYestVolumeFromId:
		m_SpinYestVolumeFrom.UpdateValueNum();
		if(m_SpinYestVolumeFrom.GetValue() != m_yestVolumeFrom)DoEnableLoadButton(true);
		else EnableLoadButton();
		YestVolumeChange();
		break;

		case spinYestVolumeToId:
		m_SpinYestVolumeTo.UpdateValueNum();
		if(m_SpinYestVolumeTo.GetValue() != m_yestVolumeTo)DoEnableLoadButton(true);
		else EnableLoadButton();
		YestVolumeChange();
		break;
	}
	return 0;
}
/*
HBRUSH SampleDialogSettingMarketSorterImbalance::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	switch(pWnd->GetDlgCtrlID())
	{
		case editStoryId:
		if(m_selectedHeadlineStoryDisplayed)
		{
//			pDC->SetBkMode(TRANSPARENT);
//			return *m_takionVisual->GetBlueBrush();
			pDC->SetTextColor(RGB(0, 0, 96));
			return *m_bkBrush;
		}
		break;

		case editSymbolId:
		if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_takionVisual->GetWarningBrush();
		}
		break;

}

	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/
void SampleDialogSettingMarketSorterImbalance::OnReset()
{
	DoReset();
}

bool SampleDialogSettingMarketSorterImbalance::DoReset()
{
	bool ret = false;//ResetControls();
/*
	if((m_CheckBoxAllEntitled.GetCheck() == BST_CHECKED) != m_allEntitled)
	{
		ret = true;
		m_CheckBoxAllEntitled.SetCheck(m_allEntitled ? BST_CHECKED : BST_UNCHECKED);
	}
*/
	int found = m_ComboBoxCharFrom.FindItem((const void*)m_charFrom);
	if(found >= 0 && found != m_ComboBoxCharFrom.GetCurSel())
	{
		m_ComboBoxCharFrom.SetCurSel(found);
		ret = true;
	}
	found = m_ComboBoxCharTo.FindItem((const void*)m_charTo);
	if(found >= 0 && found != m_ComboBoxCharTo.GetCurSel())
	{
		m_ComboBoxCharTo.SetCurSel(found);
		ret = true;
	}
	if(m_SpinYestVolumeFrom.GetValue() != m_yestVolumeFrom)
	{
		m_SpinYestVolumeFrom.SetValue(m_yestVolumeFrom);
		ret = true;
	}
	if(m_SpinYestVolumeTo.GetValue() != m_yestVolumeTo)
	{
		m_SpinYestVolumeTo.SetValue(m_yestVolumeTo);
		ret = true;
	}
	if(m_ListBoxMarketCategory.SetFilter(m_marketCategoryFilter))
	{
		ret = true;
	}
	if(m_ListBoxTestOrReal.SetFilter(m_testStockFilter))
	{
		ret = true;
	}
	if(ret)m_modified = true;
	return ret;
}

void SampleDialogSettingMarketSorterImbalance::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void SampleDialogSettingMarketSorterImbalance::OnSymbolChange()
{
	bool empty = m_symbol.empty();
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_symbol = text;
	if(m_symbol.empty() != empty)
	{
		EnableFindButton();
/*
//		m_EditSymbol.Invalidate(FALSE);
		if(m_ListBoxImbalance.GetItemCount() > 0)
		{
			EnableControl(m_ButtonFind.m_hWnd, empty , m_ListBoxImbalance.m_hWnd);
		}
*/
	}
}

void SampleDialogSettingMarketSorterImbalance::EnableFindButton()
{
	EnableControl(m_ButtonFind.m_hWnd, !m_symbol.empty() && m_ListBoxImbalance.GetItemCount() > 0, m_ListBoxImbalance.m_hWnd);
}

BOOL SampleDialogSettingMarketSorterImbalance::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		unsigned char ascii = (unsigned char)pMsg->wParam;
		switch(ascii)
		{
			case VK_RETURN:
			case VK_INSERT:
			if(Find())
			{
				return TRUE;
			}
			break;
		}
	}
	return BaseDialogSetting::PreTranslateMessage(pMsg);
}

void SampleDialogSettingMarketSorterImbalance::OnFind()
{
	Find();
}

bool SampleDialogSettingMarketSorterImbalance::Find()
{
//	if(m_EditSymbol.GetWindowTextLength())
	if(!m_symbol.empty())
	{
//		CString symbol;
//		m_EditSymbol.GetWindowText(symbol);
//		int found = m_ListBoxImbalance.FindItemByName(symbol);
		int found = m_ListBoxImbalance.FindItemByName(m_symbol.c_str());
		m_ListBoxImbalance.SetCurSel(found);
		OnSelchangeImbalance();
		return true;
	}
	return false;
}

void SampleDialogSettingMarketSorterImbalance::OnSelchangeImbalance()
{
}

void SampleDialogSettingMarketSorterImbalance::OnDblClkImbalance()
{
	int sel = m_ListBoxImbalance.GetCurSel();
	if(sel >= 0)
	{
		const char* symbol = m_ListBoxImbalance.GetItemNameAt(sel);
		m_EditSymbol.SetWindowText(symbol);
		EnableControl(m_ButtonFind.m_hWnd, true, m_ListBoxImbalance.m_hWnd);

		if(m_sendToMm)
		{
			TD_SetSymbolInCommandTool(symbol, true);//, unsigned int commandToolOrdinal = 0xFFFFFFFF);
		}
	}
}

void SampleDialogSettingMarketSorterImbalance::SetMarketSorterLoaded(const bool loaded)
{
	if(loaded != m_marketSorterLoaded)
	{
		m_marketSorterLoaded = loaded;
		m_ListBoxImbalance.SetMarketSorterLoaded(m_marketSorterLoaded);
	}
}

void SampleDialogSettingMarketSorterImbalance::SetMarketSorterConnected(const bool connected)
{
	if(connected != m_marketSorterConnected)
	{
		m_marketSorterConnected = connected;
		m_ListBoxImbalance.SetMarketSorterConnected(m_marketSorterConnected);
		if(!m_marketSorterConnected)SetMarketSorterLoaded(false);
/*
		if(!m_marketSorterConnected)
		{
			RepaintTotals();
		}
*/
	}
}

void SampleDialogSettingMarketSorterImbalance::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case M_MS_RESET_IMBALANCES://main thread notification, usually comes at noon to reset opening imbalances and get ready for closing imbalances
		case SM_MS_RESET_IMBALANCES://main thread notification, symbol mapping, usually comes at noon to reset opening imbalances and get ready for closing imbalances
		m_ListBoxImbalance.ResetImbalances();
		break;

		case TM_NEW_EQUITY:
		case TM_REFRESHED_EQUITY:
		{
			const Security* security = ((const TMsgSecurity*)message)->m_security;
			if(security->isLoaded())
			{
				AddSecurityIfBelongsAndUpdateOrdinals(security);
			}
		}
		break;

		case TM_NEW_POSITION:
		{
			const Position* position = ((TMsgNewPosition*)message)->m_position;
			if(m_currentAccount == position->GetAccount())
			{
				const int sel = m_ListBoxImbalance.FindItemByName(position->GetSymbol());
				if(sel >= 0)
				{
					StockImbalance* stock = (StockImbalance*)m_ListBoxImbalance.GetItemDataPtr(sel);
					stock->SetPosition(position);
//					stock->Repaint(m_ListBoxImbalance.GetPositionSizeLeft(), m_ListBoxImbalance.GetOpenPnlPrintRight());
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
			m_ListBoxImbalance.MarketSorterCleanup();
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
	}
}

void SampleDialogSettingMarketSorterImbalance::Resize(bool widthChanged, bool heightChanged)
{
	if(m_ListBoxImbalance.m_hWnd)
	{
		if(!m_bottomOffset)
		{
			CRect rect;
			m_ListBoxImbalance.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_ListBoxImbalance.GetClientRect(&clientRect);
			m_listBoxBorderSize2 = rect.Width() - clientRect.Width();
			m_listBoxBorderSize = m_listBoxBorderSize2 / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;

			m_titleOffset = m_leftOffset + m_listBoxBorderSize;

			m_ListBoxMarketCategory.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_StaticTestOrReal.GetWindowRect(&clientRect);
			ScreenToClient(&clientRect);
			int gap = clientRect.top - rect.bottom;
			int stHeight = clientRect.Height();
			int left = rect.left;
			int top = rect.top;
			int width = rect.Width();
			int height = m_ListBoxMarketCategory.GetMyItemHeight() * m_ListBoxMarketCategory.GetItemCount() + m_listBoxBorderSize2;
			m_ListBoxMarketCategory.SetWindowPos(nullptr, left, top, width, height, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);

			m_StaticTestOrReal.SetWindowPos(nullptr, left, top += height + gap, width, stHeight, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

			height = m_ListBoxTestOrReal.GetMyItemHeight() * m_ListBoxTestOrReal.GetItemCount() + m_listBoxBorderSize2;
			m_ListBoxTestOrReal.SetWindowPos(nullptr, left, top += stHeight, width, height, SWP_NOZORDER|SWP_NOACTIVATE);
		}
		int bottom = m_clientRect.bottom - m_bottomOffset;
		m_ListBoxImbalance.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
/*
		if(heightChanged)
		{
			const int stockHeight = m_clientRect.bottom - m_stockTopOffset;
			const bool stockHeightSmall = stockHeight < m_maxStockHeight;
			if(stockHeightSmall || m_stockHeight < m_maxStockHeight)
			{
				m_stockHeight = stockHeightSmall ? stockHeight : m_maxStockHeight;
				m_ListBoxStock.SetWindowPos(nullptr, m_stockLeftOffset, m_stockTopOffset, m_stockWidth, m_stockHeight, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
			}
			if(!stockHeightSmall)
			{
				m_EditStory.SetWindowPos(nullptr, 0, m_storyTopOffset, m_storyWidth, m_clientRect.bottom - m_storyTopOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
			}
		}
*/
	}

	BaseDialogSetting::Resize(widthChanged, heightChanged);
	if(heightChanged)InvalidateAllTotalValueRects(listBoxImbalanceId);
}

void SampleDialogSettingMarketSorterImbalance::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
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

const char* SampleDialogSettingMarketSorterImbalance::GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const
{
	switch(columnId)
	{
		case MSI_SYMBOL:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxImbalance.GetItemCount(), buf, buflen);

		case MSI_LOG:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxImbalance.GetLogCount(), buf, buflen);

		default:
		return nullptr;
	}
}
/*
void SampleDialogSettingMarketSorterImbalance::TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff)
{
	m_totalClosedPnlRect.left += diff;
	m_totalOpenPnlRect.left += diff;
	m_totalOpenPrintPnlRect.left += diff;
	m_totalRangeAtFirstTimerRect.left += diff;

	m_totalClosedPnlRect.right += diff;
	m_totalOpenPnlRect.right += diff;
	m_totalOpenPrintPnlRect.right += diff;
	m_totalRangeAtFirstTimerRect.right += diff;
}
*/