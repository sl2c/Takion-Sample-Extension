#include "stdafx.h"
#include "MyMarketSorter.h"
#include "MainDialog.h"
#include "ExtensionSample.h"

extern MainDialog* mainDialog;
extern ExtensionSample theApp;

const char* const MyMarketSorter::myMarketSorterId = "MyMarketSorter";
static const bool zeroPriceEmpty = true;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool MmsUnappliedMessageReader::ProcessDataToken(const char* cursor, unsigned short length) const
{
	const Message* message = (const Message*)cursor;
	switch(message->m_type)
	{
		case TM_MS_TRADE_SHORT_INTERNAL:
		{
			const TMsgMsTradeShortInternal* msg = (const TMsgMsTradeShortInternal*)message;
			m_row->AddPrint(Price(msg->m_price), msg->m_size, msg->m_marketCenterID, msg->m_printFlags);
		}
		return true;

		case TM_MS_TRADE_LONG_INTERNAL:
		{
			const TMsgMsTradeLongInternal* msg = (const TMsgMsTradeLongInternal*)message;
			m_row->AddPrint(Price(msg->m_priceDollars, msg->m_priceFraction), msg->m_size, msg->m_marketCenterID, msg->m_printFlags);
		}
		return true;

		case TM_EQUITY_NEW_MINUTE:
		{
			const TMsgEquityNewMinute* msg = (const TMsgEquityNewMinute*)message;
			m_row->SetEndChartMinute(msg->m_minute + 1, msg->m_fromServer);
		}
//		m_row->SetEndChartMinute(((const TMsgNewMinute*)message)->m_minute + 1);
		return false;

		default:
		return false;
	}
}

MyMarketSorterRow::MyMarketSorterRow(MyMarketSorter* parent, Security* security, const Position* position, bool potential):
	RowSortable(parent->GetBuiltInColumnCount(), parent, potential),
	SecurityChartRow(security, position, parent->GetTimeFrame(), parent->GetTimeFrameInSeconds()),
	m_myMarketSorterWindow(parent),
	m_securityColor(RGB(255, 255, 255)),
	m_unappliedMessageReader(this, m_security ? m_security->GetUnappliedMessages() : nullptr)
{
	SetRowValue(MyMarketSorter::MMS_OPEN_PRICE, new MySecurityPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetOpenPrice, zeroPriceEmpty, false), false);
	SetRowValue(MyMarketSorter::MMS_CLOSE_PRICE, new MySecurityPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetClosePrice, zeroPriceEmpty, false), false);
	SetRowValue(MyMarketSorter::MMS_LAST_TRADE, new MySecurityCompactPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetLastPrice, zeroPriceEmpty, false), true);
	SetRowValue(MyMarketSorter::MMS_DAY_HIGH_PRICE, new MySecurityPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetHighPrice, zeroPriceEmpty, false), true);
	SetRowValue(MyMarketSorter::MMS_DAY_LOW_PRICE, new MySecurityPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetLowPrice, zeroPriceEmpty, false), true);
	SetRowValue(MyMarketSorter::MMS_BID, new MySecurityCompactPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetL1Bid, zeroPriceEmpty, false), true);
	SetRowValue(MyMarketSorter::MMS_ASK, new MySecurityCompactPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetL1Ask, zeroPriceEmpty, false), true);

	SetRowValue(MyMarketSorter::MMS_MARKET_HOURS_NET_CHANGE, new MySecurityPriceRowValueDiffAsUInt(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetOpenPrice, &Security::GetLastPrice, true, true), true);
	SetRowValue(MyMarketSorter::MMS_MARKET_HOURS_NET_PERCENT_CHANGE, new MySecurityPriceRowValueDiffAsUIntDividedByFirst(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetOpenPrice, &Security::GetLastPrice, true, true), true);
	SetRowValue(MyMarketSorter::MMS_POS_SIZE, new PositionIntInventoryRowValueNull(m_position, parent->isInventoryView(), parent->isCommaSize(), &Position::GetSizeInv, true), true);

	SetRowValue(MyMarketSorter::MMS_YESTERDAYS_HIGH_PRICE, new MySecurityPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetYesterdaysHighPrice, zeroPriceEmpty, false), false);
	SetRowValue(MyMarketSorter::MMS_YESTERDAYS_LOW_PRICE, new MySecurityPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetYesterdaysLowPrice, zeroPriceEmpty, false), false);
	SetRowValue(MyMarketSorter::MMS_PREMARKET_HIGH_PRICE, new MySecurityPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetPreMarketHoursHighPrice, zeroPriceEmpty, false), true);
	SetRowValue(MyMarketSorter::MMS_PREMARKET_LOW_PRICE, new MySecurityPriceRowValue(m_security, parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetPreMarketHoursLowPrice, zeroPriceEmpty, false), true);

#ifndef USE_DYNAMIC_ARRAY
	UpdateRowValueIterators();
#endif
	Lock(true);
	Initialize();
	UpdateRowColor();
	Unlock();
}

MyMarketSorterRow::~MyMarketSorterRow()
{
}

COLORREF MyMarketSorterRow::GetRowColor() const
{
	return m_securityColor;//0xFFFFFFFF;//each cell has its own color
}

COLORREF MyMarketSorterRow::GetRowBkColor() const
{
	return 0xFFFFFFFF;//each cell has its own color
}

COLORREF MyMarketSorterRow::GetCellColor(unsigned short columnId) const
{
	switch(columnId)
	{
		case MarketSorterWindow::MS_SYMBOL:
		return m_myMarketSorterWindow->GetActiveSymbol() == m_security->GetNumericSymbol() ? m_myMarketSorterWindow->GetTopActiveColor() : 0xFFFFFFFF;

		default:
		return 0xFFFFFFFF;
	}
}

COLORREF MyMarketSorterRow::GetCellBkColor(unsigned short columnId) const
{
	return 0xFFFFFFFF;//same as window bk color
}

RowValue* MyMarketSorterRow::CreateCell(unsigned short id)
{
	bool dynamic = true;
	RowValue* rowValue;
	switch(id)
	{
		case MyMarketSorter::MMS_OPEN_PRICE:
		rowValue = new MySecurityPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetOpenPrice, zeroPriceEmpty, false);
		dynamic = false;
		break;

		case MyMarketSorter::MMS_CLOSE_PRICE:
		rowValue = new MySecurityPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetClosePrice, zeroPriceEmpty, false);
		dynamic = false;
		break;

		case MyMarketSorter::MMS_LAST_TRADE:
		rowValue = new MySecurityCompactPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetLastPrice, zeroPriceEmpty, false);

		case MyMarketSorter::MMS_DAY_HIGH_PRICE:
		rowValue = new MySecurityPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetHighPrice, zeroPriceEmpty, false);
		break;

		case MyMarketSorter::MMS_DAY_LOW_PRICE:
		rowValue = new MySecurityPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetLowPrice, zeroPriceEmpty, false);
		break;

		case MyMarketSorter::MMS_BID:
		rowValue = new MySecurityCompactPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetL1Bid, zeroPriceEmpty, false);
		break;

		case MyMarketSorter::MMS_ASK:
		rowValue = new MySecurityCompactPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetL1Ask, zeroPriceEmpty, false);
		break;

		case MyMarketSorter::MMS_MARKET_HOURS_NET_CHANGE:
		rowValue = new MySecurityPriceRowValueDiffAsUInt(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetOpenPrice, &Security::GetLastPrice, true, true);
		break;

		case MyMarketSorter::MMS_MARKET_HOURS_NET_PERCENT_CHANGE:
		rowValue = new MySecurityPriceRowValueDiffAsUIntDividedByFirst(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetOpenPrice, &Security::GetLastPrice, true, true);
		break;

		case MyMarketSorter::MMS_POS_SIZE:
		rowValue = new MyPositionIntInventoryRowValueNull(m_position, m_windowSortable->isInventoryView(), m_windowSortable->isCommaSize(), &PositionData::GetSizeInv, true);
		break;

		case MyMarketSorter::MMS_YESTERDAYS_HIGH_PRICE:
		rowValue = new MySecurityPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetYesterdaysHighPrice, zeroPriceEmpty, false);
		dynamic = false;
		break;

		case MyMarketSorter::MMS_YESTERDAYS_LOW_PRICE:
		rowValue = new MySecurityPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetYesterdaysLowPrice, zeroPriceEmpty, false);
		dynamic = false;
		break;

		case MyMarketSorter::MMS_PREMARKET_HIGH_PRICE:
		rowValue = new MySecurityPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetPreMarketHoursHighPrice, zeroPriceEmpty, false);
		break;

		case MyMarketSorter::MMS_PREMARKET_LOW_PRICE:
		rowValue = new MySecurityPriceRowValue(m_security, m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetPreMarketHoursLowPrice, zeroPriceEmpty, false);
		break;

		default:
		return nullptr;
	}
	SetRowValue(id, rowValue, dynamic);
	return rowValue;
}

bool MyMarketSorterRow::UpdatePrints(bool lock)
{
	if(m_windowSortable->UpdateRow(this, lock, true))
	{
		return true;
	}
	return false;
}

void MyMarketSorterRow::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_EQUITY_IMBALANCE_UPDATE:
		if(m_myMarketSorterWindow->UpdateRow(this, true, true))
		{
			delete this;
		}
		break;

		case TM_UPDATED_POSITION:
		if(m_myMarketSorterWindow->UpdateRow(this, true, true))
		{
			delete this;
		}
		break;

		case TM_POSITION_DELETE:
		if(SetPosition(nullptr) && m_myMarketSorterWindow->UpdateRow(this, true, true))delete this;
		break;

		case TM_EQUITY_NEW_MINUTE:
		{
			const TMsgEquityNewMinute* msg = (const TMsgEquityNewMinute*)message;
			if(SetCurrentMinute(msg->m_minute, msg->m_fromServer))
			{
				if(m_myMarketSorterWindow->UpdateRow(this, true, true))
				{
					delete this;
				}
			}
		}
		break;

		case TM_EQUITY_CHART_LOADED:
		Lock(true);
		if(!m_chartLoaded)
		{
			m_chartLoaded = true;
			InitializeChart(2);
		}
		if(m_myMarketSorterWindow->UpdateRow(this, false, true))
		{
			Unlock();
			delete this;
		}
		else
		{
			Unlock();
		}
		break;

		case TM_EQUITY_UPDATE:
		if(((const TMsgEquityUpdate*)message)->m_hasPrints)
		{
			Lock(true);
			m_unappliedMessageReader.ProcessAllData();
			CalculateValues();
			if(UpdatePrints(false))
			{
				Unlock();
				delete this;
			}
			else
			{
				Unlock();
			}
		}
		else if(m_myMarketSorterWindow->UpdateRow(this, true, true))delete this;
		break;

		case TM_EQUITY_REFRESHED:
		case TM_EQUITY_RESET:
		case TM_SORTABLE_SECURITY_SYMBOL_MAPPING_CHANGED:
		case M_MS_AVERAGE_DAILY_VOLUME:
		case SM_MS_AVERAGE_DAILY_VOLUME:
		case M_MS_PREVIOUS_DAY_HIGH_LOW:
		case SM_MS_PREVIOUS_DAY_HIGH_LOW:
		case M_MS_SECURITY_TYPE:
		case SM_MS_SECURITY_TYPE:
		case M_MS_TICK_PILOT:
		case SM_MS_TICK_PILOT:
		case TM_SORTABLE_SECURITY_ENTITLEMENTS_CHANGED:

		case TM_EQUITY_CLOSED:
		case TM_EQUITY_OPENED:
		case M_MS_EOD_SUMMARY_REPORT:
		case SM_MS_EOD_SUMMARY_REPORT:
		case TM_EQUITY_YEST_CLOSE_PRICE_CHANGED:
		case TM_SET_EQUITY_TIER_SIZE://Main Thread
		case TM_SET_EQUITY_CUSTOM_PRICE_BASE:
		case TM_SET_EQUITY_CUSTOM_DATE:
		case TM_EQUITY_BORROW_PRICE:
		case TM_EQUITY_BETA_AND_RETAIL_INFO:
		case TM_EQUITY_BORROW_INFO:
		case TM_EQUITY_HTB:
		case M_MS_DIVIDENDS_AND_SPLITS:
		case SM_MS_DIVIDENDS_AND_SPLITS:
		case M_MS_SHORT_SALE_INDICATORS:
		case SM_MS_SHORT_SALE_INDICATORS:
		case M_MS_LULD:
		case SM_MS_LULD:
		case M_MS_PMI:
		case SM_MS_PMI:
//		case M_52WEEK_30DAY_HIGH_LOW:
//		case SM_M_52WEEK_30DAY_HIGH_LOW:
		case M_MS_52WEEK_30DAY_HIGH_LOW:
		case SM_MS_52WEEK_30DAY_HIGH_LOW:

		if(m_myMarketSorterWindow->UpdateRow(this, true, false))delete this;
		break;

		case TM_SORTABLE_SECURITY_INVALID:
		if(*(unsigned __int64*)((const TMessageSymbol*)message)->m_symbol == m_security->GetNumericSymbol())
		{
			RowSortable* row = m_myMarketSorterWindow->RemoveRow(this);
			if(row)
			{
				delete row;
			}
		}
		break;

		case TM_SORTABLE_SECURITY_DELETE:
		if(m_security == ((const TMsgSecurity*)message)->m_security)
		{
			RowSortable* row = m_myMarketSorterWindow->RemoveRow(this);
			if(row)
			{
				delete row;
			}
		}
		break;

		default:
		if(m_myMarketSorterWindow->UpdateRow(this, true, true))delete this;
		break;
	}
}

bool MyMarketSorterRow::Lock(bool wait) const
{
	if(m_security)
	{
		if(wait)
		{
			m_security->LockInquiryWaitLevel1();
			if(m_position)m_position->LockInquiryWait();
			return true;
		}
		else if(m_security->LockInquiryLevel1())
		{
			if(!m_position || m_position->LockInquiry())
			{
				return true;
			}
			m_security->UnlockInquiryLevel1();
		}
		return false;
	}
	return true;
}
/*
void MyMarketSorterRow::LockWait() const
{
	m_security->LockInquiryWaitLevel1();
	if(m_position)m_position->LockInquiryWait();
}
*/
void MyMarketSorterRow::Unlock() const
{
	m_security->UnlockInquiryLevel1();
	if(m_position)m_position->UnlockInquiry();
}
/*
bool MyMarketSorterRow::isSortValueChanged() const
{
//There may be some columns that require special handling
	return RowSortable::isSortValueChanged();
}

bool MyMarketSorterRow::isChainSortValueChanged() const
{
//There may be some columns that require special handling
	return RowSortable::isChainSortValueChanged();
}

short MyMarketSorterRow::Compare(const RowSortable& other) const
{
//There may be some columns that require special handling
	return RowSortable::Compare(other);
}

short MyMarketSorterRow::CompareAhead(const RowSortable& other) const
{
//There may be some columns that require special handling
	return RowSortable::CompareAhead(other);
}

short MyMarketSorterRow::CompareChain(const RowSortable& other) const
{
//There may be some columns that require special handling
	return RowSortable::CompareChain(other);
}

short MyMarketSorterRow::CompareChainAhead(const RowSortable& other) const
{
//There may be some columns that require special handling
	return RowSortable::CompareChainAhead(other);
}
*/
short MyMarketSorterRow::PreCompare(const RowSortable& other) const
{
	const unsigned __int64& numericSymbol = m_myMarketSorterWindow->GetActiveSymbol();
	if(numericSymbol)
	{
		const MyMarketSorterRow& rowOther = (const MyMarketSorterRow&)other;
		if(m_security->GetNumericSymbol() == numericSymbol)
		{
			if(rowOther.m_security->GetNumericSymbol() != numericSymbol)
			{
				return -1;
			}
		}
		else
		{
			if(rowOther.m_security->GetNumericSymbol() == numericSymbol)
			{
				return 1;
			}
		}
	}
	return 0;
}

short MyMarketSorterRow::DefaultCompare(const RowSortable& other) const
{
	const Security* otherSecurity = ((const MyMarketSorterRow&)other).m_security;
	int res = strcmp(m_security->GetSymbol(), otherSecurity->GetSymbol());
	if(res)
	{
		return res < 0 ? -1 : 1;
	}
	return m_security < otherSecurity ? -1:
		m_security > otherSecurity ? 1:
		0;
}

bool MyMarketSorterRow::CompareFunction(const RowSortable& other, unsigned short sortId, short& ret) const
{
	MyMarketSorter::FnCompare fn = m_myMarketSorterWindow->GetFnCompare(sortId);
	if(fn)
	{
		ret = (this->*fn)((const MyMarketSorterRow&)other);
		return true;
	}
	return false;
}

bool MyMarketSorterRow::CompareFunctionAhead(const RowSortable& other, unsigned short sortId, short& ret) const
{
	MyMarketSorter::FnCompare fn = m_myMarketSorterWindow->GetFnCompareAhead(sortId);
	if(fn)
	{
		ret = (this->*fn)((const MyMarketSorterRow&)other);
		return true;
	}
	return false;
}

bool MyMarketSorterRow::GetPaintTextInfo(unsigned short columnId, std::string& text, COLORREF& color) const//, UINT& horizontalJustification) const
{
	const MyMarketSorter::FnPaintInfo fn = m_myMarketSorterWindow->GetFnPaintInfo(columnId);
	if(fn)
	{
		(this->*fn)(text, drawColor);//, drawHorizontalJustification);
		return true;
	}
	return false;
}

void MyMarketSorterRow::Update(bool dynamicOnly)//don't call this function. It is called by Takion.
{
//There may be some columns that require special handling
	RowSortable::Update(dynamicOnly);
}

bool MyMarketSorterRow::UpdateRowColor()//don't call this function. It is called by Takion. Return true if the color changes and you have to repaint the whole line.
{
//There may be some columns that require special handling
	return RowSortable::UpdateRowColor();
}

void MyMarketSorterRow::UpdateAndInvalidate(CRect& rowRect, bool dynamicOnly)//don't call this function. It is called by Takion.
{
//There may be some columns that require special handling
	RowSortable::UpdateAndInvalidate(rowRect, dynamicOnly);
}

void MyMarketSorterRow::Initialize()//should be called only inside a lock
{
	InitializeChart(true);
	RowSortable::Initialize();
}

const Security* MyMarketSorterRow::GetSelectedSecurity() const
{
	return m_security;
}

bool MyMarketSorterRow::SetPosition(const Position* position)
{
	if(SecurityChartRow::SetPosition(position))
	{
		ApplyPosition(MyMarketSorter::MMS_POS_SIZE, position);
		return true;
	}
	return false;
}

void MyMarketSorterRow::PaintSymbol(std::string& text, COLORREF& color) const//, UINT& horizontalJustification) const
{
	text += m_security->GetSymbol();
//	horizontalJustification = DT_LEFT;
}

short MyMarketSorterRow::CompareSymbol(const MyMarketSorterRow& other) const
{
	int ret = strcmp(m_security->GetSymbol(), other.m_security->GetSymbol());
	return ret < 0 ? -1:
		ret > 0 ? 1:
		0;
}

void MyMarketSorterRow::PaintExchange(std::string& text, COLORREF& color) const//, UINT& horizontalJustification) const
{
	U_AppendUnsignedNumberAsString(text, m_security->GetPrimaryExchangeNumericName());
//	horizontalJustification = DT_LEFT;
}

short MyMarketSorterRow::CompareExchange(const MyMarketSorterRow& other) const
{
	char ret = U_CompareUIntAsString(m_security->GetPrimaryExchangeNumericName(), other.m_security->GetPrimaryExchangeNumericName());
//	int ret = strcmp(m_security->GetPrimaryExchangeName(), other.m_security->GetPrimaryExchangeName());
	return ret < 0 ? -1:
		ret > 0 ? 1:
		0;
}

void MyMarketSorter::OnSuccessfulCreate()
{
	AddColumn("Symbol", MMS_SYMBOL, 60, 0, true, true, "Symbol", true, true, false, 0, false, true, false, DT_LEFT);
	AddColumn("Exch", MMS_EXCHANGE, 50, 0, true, true, "Exchange", true, true, false, 0, false, true, false, DT_LEFT);

	AddColumn("OpenPrice", MMS_OPEN_PRICE, 80, 0, true, false, "Open Price", true, true, true, 0, false, true, false, DT_RIGHT);
	AddColumn("ClosePrice", MMS_CLOSE_PRICE, 80, 0, true, false, "Close Price", true, true, true, 0, false, true, false, DT_RIGHT);
	AddColumn("LastPrice", MMS_LAST_TRADE, 80, 0, true, false, "Last Price", true, true, true, 0, false, true, false, DT_RIGHT);
	AddColumn("HiPrice", MMS_DAY_HIGH_PRICE, 80, 0, true, false, "High Price", true, true, true, 0, false, true, false, DT_RIGHT);
	AddColumn("LoPrice", MMS_DAY_LOW_PRICE, 80, 0, true, false, "Low Price", true, true, true, 0, false, true, false, DT_RIGHT);
	AddColumn("Bid", MMS_BID, 80, 0, true, false, "Bid Price", true, true, true, 0, false, true, false, DT_RIGHT);
	AddColumn("Ask", MMS_ASK, 80, 0, true, false, "Ask Price", true, true, true, 0, false, true, false, DT_RIGHT);

	AddColumn("OpenNet", MMS_MARKET_HOURS_NET_CHANGE, 80, 0, true, false, "Open Net Change", true, true, true, 1, false, true, false, DT_RIGHT);
	AddColumn("OpenNet%", MMS_MARKET_HOURS_NET_PERCENT_CHANGE, 80, 0, true, false, "Open Net Change %", true, true, true, 1, false, true, false, DT_RIGHT);
	AddColumn("PosSize", MMS_POS_SIZE, 60, 0, true, false, "Position Size", true, true, true, 1, false, true, false, DT_RIGHT);

	AddColumn("yHiPrice", MMS_YESTERDAYS_HIGH_PRICE, 80, 0, true, false, "Yesterday's High Price", true, true, true, 0, false, true, false, DT_RIGHT);
	AddColumn("yLoPrice", MMS_YESTERDAYS_LOW_PRICE, 80, 0, true, false, "Yesterday's Low Price", true, true, true, 0, false, true, false, DT_RIGHT);

	AddColumn("preHiPrice", MMS_PREMARKET_HIGH_PRICE, 80, 0, true, false, "Pre Market Hours High Price", true, true, true, 0, false, true, false, DT_RIGHT);
	AddColumn("preLoPrice", MMS_PREMARKET_LOW_PRICE, 80, 0, true, false, "Pre Market Hours Low Price", true, true, true, 0, false, true, false, DT_RIGHT);

	RowWindowSortableAccount::OnSuccessfulCreate();
	AddInventoryColumn(MMS_POS_SIZE, 1);

	mainDialog->TakionWindowRepaintTool(this);
}

void MyMarketSorter::AddDoubleClickActionMenuItems(CMenu& menu)
{
}

void MyMarketSorter::SetAccount(const Account* account)
{
	if(m_account != account)
	{
		m_account = account;
		UpdateHistoryLoad();
		AccountChanged();
	}
}

void MyMarketSorter::RepaintPositionValues()
{
	InvalidateDataAtColumn(MMS_POS_SIZE);
/*
	if(m_rowTotal)
	{
		CRect rowRect(m_totalRowRect);
		InvalidateCellIfVisible(rowRect, MS_POS_SIZE);
	}
*/
}

void MyMarketSorter::UpdatePositionValues(RowSortable* row)
{
	row->UpdateRowValue(MMS_POS_SIZE);
/*
	if(m_rowTotal)
	{
		m_rowTotal->AdjustRowValue(row, MMS_POS_SIZE);
	}
*/
}

void MyMarketSorter::CurrentAccountConstraintsLoaded()
{
	UpdateAccountInfo();
}

void MyMarketSorter::CurrentAccountLoaded()
{
	RowWindowSortableAccount::CurrentAccountLoaded();
	UpdateAccountInfo();
}

void MyMarketSorter::AccountChanged()
{
	RowWindowSortableAccount::AccountChanged();
	UpdateAccountInfo();
}

void MyMarketSorter::AllAccountsDeleted()
{
	SetAccount(nullptr);
}

void MyMarketSorter::Repopulate()
{
//	DestroyRows();
	TakionIterator* iterator = TD_CreateEquityIterator();
	TD_LockEquityStorage();
	iterator->Reset();
	Security* security;
	while(security = TD_GetNextEquity(iterator))
	{
		if(security->isLoaded())
		{
			NewRow(security);
		}
	}
	TD_UnlockEquityStorage();
	delete iterator;
}

void MyMarketSorter::UpdateAccountInfo()
{
	bool sortingChanged = false;
	unsigned short sortId = GetSortId();
	switch(sortId)
	{
		case MMS_POS_SIZE:
		sortingChanged = true;
		break;

		default:
		if(!m_sortChain.isEmpty())
		{
			const UShortChain::List& sortList = m_sortChain.GetList();
			for(UShortChain::ListConstIterator it = sortList.begin(), itend = sortList.end(); !sortingChanged && it != itend; ++it)
			{
				switch(*it)
				{
					case MMS_POS_SIZE:
					sortingChanged = true;
					break;
				}
			}
		}
		break;
	}

	if(sortingChanged)
	{
		ClearRowsTemporarily();
	}
	bool changed = sortingChanged;
	UnselectAllRows(true);
	MyMarketSorterRow* row;
	Position* position;
	unsigned char belongs;
	const Security* security;
	size_t ord;

	if(m_account)m_account->LockInquiryWait();

	for(RowMap::CPair* p = m_rowMap.PGetFirstAssoc(); p; )
	{
		row = (MyMarketSorterRow*)p->value;

		p = m_rowMap.PGetNextAssoc(p);

		if(m_account)
		{
			security = row->GetSecurity();
			position = m_account->FindPosition(security);
		}
		else
		{
			position = nullptr;
		}
		if(row->SetPosition(position))
		{
			row->Lock(true);
			UpdatePositionValues(row);
			belongs = RowBelongs(row);
			row->Unlock();
			if(ProcessRowBelonging(row, belongs, sortingChanged))
			{
				changed = true;
			}
		}
		else if(sortingChanged)
		{
			if(!row->isPotential())
			{
				ReInsertRow(row, ord);
//				RowInserted(row);
			}
		}
	}
	if(m_account)m_account->UnlockInquiry();

	if(changed)
	{
		RepaintOnPopulation(false);
	}
	else
	{
		RepaintPositionValues();
	}
	InvalidateRect(&m_countRect, FALSE);
	InvalidateRect(&m_potentialCountRect, FALSE);
}

Row* MyMarketSorter::CreateRow(const void* object, bool potential)
{
	Security* security = (Security*)object;
	Position* position = nullptr;
	if(m_account)
	{
		position = m_account->FindPositionWithLock(security);
	}
	return new MyMarketSorterRow(this, security, position, potential);
}

const Security* MyMarketSorter::GetSecurityFromRow(const MyMarketSorterRow* row) const
{
	return row->GetSecurity();
}

bool MyMarketSorter::SecurityPotentiallyBelongs(const Security* security) const
{
//Use security to figure out if it can get potentially displayed in the window.
//Sometimes you may want to exclude some stocks permanently and never show them. For example, you want to see only NYSE stocks.
//Security is not locked at this point, because the values you want to use for potential belonging are not changing.
//Do not use changing values, like LastPrintPrice here.

//	return (m_realTestFilter & (security->isTest() ? 1 << MRTS_TEST : 1 << MRTS_REAL)) != 0
//		&& (m_marketCategoryFilter & security->GetMarketCategoryFilter()) != 0;

	return m_filter.SecurityPotentiallyBelongs(security);
}

bool MyMarketSorter::SecurityRowBelongs(const MyMarketSorterRow* row) const
{
//	const Security* security = row->GetSecurity();
//Use security to figure out if it should be displayed in the window.
//Security is locked at this point.
//You should not call this function. If you do call it for any reason, Lock/Unlock row.

	return m_filter.SecurityBelongs(row);
}

void MyMarketSorter::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_NEW_EQUITY:
		{
			const Security* security = ((const TMsgSecurity*)message)->m_security;
			if(security->isLoaded())
			{
				NewRow(security);
			}
		}
		break;

		case TM_NEW_POSITION:
		{
			const Position* position = ((TMsgNewPosition*)message)->m_position;
			if(m_account == position->GetAccount())
			{
				const Security* security = TD_FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(position->GetNumericSymbol());
				if(security)
				{
					MyMarketSorterRow* row = (MyMarketSorterRow*)FindObject(security);
					if(row && row->SetPosition(position) && UpdateRow(row, true, true))delete row;
				}
			}
		}
		break;

		case TM_DISCONNECTED:
		if(((const TMsgDisconnected*)message)->m_finalDisconnect)
		{
			break;
		}
//fall through
		case TM_CONNECTED:
		SetMarketSorterConnected(TD_IsMarketSorterConnected());
		break;

		case TM_MARKET_SORTER_LOADED:
		SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
		break;

		case TM_MARKET_SORTER_CLEANUP:
		DestroyRows();
		break;
	}
}

MyMarketSorter::MyMarketSorter(const CBitmap* bitmap,
	unsigned int workspace,
	bool pinned,
	unsigned short builtInColumns,
	int minVisibleSize,
	int minWidth,
	int maxWidth,
	int minHeight,
	int maxHeight,
	int minBorderWidth,
	int maxBorderWidth,
	const char* className,
	int minTitleHeight,
	int maxTitleHeight,
	int titleHeight
	):
	RowWindowSortableAccount(true,
		nullptr,
		workspace,
		pinned,
		minVisibleSize,
		minWidth,
		maxWidth,
		minHeight,
		maxHeight,
		minBorderWidth,
		maxBorderWidth,
		className,
		minTitleHeight,
		maxTitleHeight,
		titleHeight
		),
	m_timeFrame(1440),
	
//	m_marketCategoryFilter((1 << MCF_COUNT) - 1),
//	m_realTestFilter((1 << MRTS_Last) - 1),

	m_marketSorterConnected(TD_IsMarketSorterConnected()),
	m_colorDisconnected(RGB(128, 128, 128)),
	m_marketSorterLoaded(TD_IsMarketSorterLoaded()),
	m_colorNotLoaded(RGB(128, 128, 0)),
	m_prevTopRow(nullptr),
	m_nextTopRow(nullptr)
{
	m_rowMap.InitHashTable(9001);

	TD_GetNewEquityObservable().AddObserver(this);
	TD_GetNewPositionObservable().AddObserver(this);
	TD_GetAdminObservable().AddObserver(this);

	memset(&m_paintInfoFunction, 0, sizeof(m_paintInfoFunction));
	memset(&m_compareFunction, 0, sizeof(m_compareFunction));
	memset(&m_compareAheadFunction, 0, sizeof(m_compareAheadFunction));

	AddMyMarketSorterFunctions(MMS_SYMBOL, &MyMarketSorterRow::PaintSymbol, &MyMarketSorterRow::CompareSymbol, &MyMarketSorterRow::CompareSymbol);
	AddMyMarketSorterFunctions(MMS_EXCHANGE, &MyMarketSorterRow::PaintExchange, &MyMarketSorterRow::CompareExchange, &MyMarketSorterRow::CompareExchange);

	mainDialog->TakionWindowAdded(this);
}

bool MyMarketSorter::AddMyMarketSorterFunctions(unsigned short id, FnPaintInfo fnPaint, FnCompare fnCompare, FnCompare fnCompareAhead)
{
	if(id < MMS_COLUMN_COUNT)
	{
		m_paintInfoFunction[id] = fnPaint;
		m_compareFunction[id] = fnCompare;
		m_compareAheadFunction[id] = fnCompareAhead;
		return true;
	}
	return false;
}

MyMarketSorter::~MyMarketSorter()
{
	DestroyRows();
	mainDialog->TakionWindowRemoved(this);
}

COLORREF MyMarketSorter::GetBkColor() const
{
	return m_marketSorterConnected ? m_marketSorterLoaded ? m_backgroundColor : m_colorNotLoaded : m_colorDisconnected;
}

void MyMarketSorter::ToolAdded()
{
	if(!m_fontRowCustom)
	{
		SetFontRow(m_fontDescriptionRow);
	}
	m_rowVerticalSpacing = SPACING_SMALL;//SPACING_MEDIUM;//graphics->GetVerticalSpacing();
	m_rowHorizontalSpacing = SPACING_EXTRA_LARGE;//SPACING_MEDIUM;//graphics->GetHorizontalSpacing();
	UpdateRowMetrics();
}

TakionTool* MyMarketSorter::Clone() const
{
	return new MyMarketSorter(nullptr, m_workspace, m_pinned);
}

void MyMarketSorter::OnHidden()
{
	mainDialog->TakionWindowRepaintTool(this);
}

void MyMarketSorter::SetMarketSorterConnected(bool connected)
{
	if(m_marketSorterConnected != connected)
	{
		m_marketSorterConnected = connected;
		if(!m_marketSorterConnected)
		{
			m_marketSorterLoaded = false;
//			ResetAll
		}
		if(m_hWnd)
		{
			InvalidateRect(&m_clientRect, FALSE);
		}
	}
}

void MyMarketSorter::SetMarketSorterLoaded(bool loaded)
{
	if(m_marketSorterLoaded != loaded)
	{
		m_marketSorterLoaded = loaded;
		if(m_hWnd)
		{
			InvalidateRect(&m_clientRect, FALSE);
		}
	}
}

void MyMarketSorter::DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	const unsigned int id = lpDrawItemStruct->itemID;
	switch(id)
	{
//		case MMSID_REAL:
//		case MMSID_TEST:
//		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, true, false, m_realTestFilter ? 0xFFFFFFFF : RGB(255, 0, 0));//nullptr, nullptr, 0xFFFFFFFF);
//		break;

		case TMID_COPY_ALL:
		case TMID_COPY:
		case TMID_PASTE_STOCKS:
		case TMID_REMOVE:
		case TMID_CLEAR:
		case TMID_FILTER:
//		case MMSID_MARKET_CATEGORY_ALL:
//		case MMSID_MARKET_CATEGORY_NONE:
		case TMID_FIND:
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false, false);//, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
		break;
/*
		case TMID_TIME_FRAME_IN_MINUTES:
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, true, false, 0xFFFFFFFF, nullptr, nullptr, takionConfig->m_marketSorterUseTimeFrame.GetValue() ? 0xFFFFFFFF : m_visualBase->GetGrayColor());
		break;
*/
		default:
//		if(lpDrawItemStruct->itemID >= MMSID_Last && lpDrawItemStruct->itemID < MMSID_Last + MCF_COUNT)
//			UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, true, false, m_marketCategoryFilter ? 0xFFFFFFFF : RGB(255, 0, 0));//, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, );
//		else
		RowWindowSortableAccount::DrawMenuItem(lpDrawItemStruct);
		break;
	}
}

bool MyMarketSorter::MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	const unsigned int id = lpMeasureItemStruct->itemID;
	switch(id)
	{
		case TMID_FIND:
//		case MMSID_REAL:
//		case MMSID_TEST:
//		case MMSID_MARKET_CATEGORY_ALL:
//		case MMSID_MARKET_CATEGORY_NONE:
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU) - 4;
		lpMeasureItemStruct->itemWidth = TGUI_GetTakionMainWnd()->GetTruncateMenuWidth() - lpMeasureItemStruct->itemHeight;
		break;

		case TMID_COPY_ALL:
		case TMID_COPY:
		case TMID_PASTE_STOCKS:
		case TMID_REMOVE:
		case TMID_CLEAR:
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);// -4;
		lpMeasureItemStruct->itemWidth = TGUI_GetTakionMainWnd()->GetRemoveAllMenuWidth();// -lpMeasureItemStruct->itemHeight;
		break;

		case TMID_FILTER:
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
		lpMeasureItemStruct->itemWidth = TGUI_GetTakionMainWnd()->GetFilterMenuWidth() - lpMeasureItemStruct->itemHeight;
		break;

		default:
//		if(lpMeasureItemStruct->itemID >= MMSID_Last && lpMeasureItemStruct->itemID < MMSID_Last + MCF_COUNT)
//		{
//			lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU) - 4;
//			lpMeasureItemStruct->itemWidth = TGUI_GetTakionMainWnd()->GetTruncateMenuWidth() - lpMeasureItemStruct->itemHeight;
//		}
//		else
		return RowWindowSortableAccount::MeasureMenuItem(lpMeasureItemStruct);
		break;
	}
	return true;
}
////////////

void MyMarketSorter::AddMenuItems(CMenu& menu)
{
	menu.AppendMenu(MF_OWNERDRAW|(GetRowCount() ? MF_ENABLED : MF_GRAYED), TMID_COPY_ALL, "Copy All");
	menu.AppendMenu(MF_OWNERDRAW |((m_multiselect ? m_selectedSet.empty() : !GetRowCount()) ? MF_GRAYED : MF_ENABLED), TMID_COPY, "Copy");

	menu.AppendMenu(MF_OWNERDRAW|(IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED), TMID_PASTE_STOCKS, "Paste");
	menu.AppendMenu(MF_OWNERDRAW|((m_multiselect ? m_selectedSet.empty() : !GetRowCount()) ? MF_GRAYED : MF_ENABLED), TMID_REMOVE, "Remove");
	menu.AppendMenu(MF_OWNERDRAW|(CanClearSymbols() ? MF_ENABLED : MF_GRAYED), TMID_CLEAR, "Clear");
	RowWindowSortableAccount::AddMenuItems(menu);

	CMenu popupMenu;
/*
	popupMenu.CreatePopupMenu();
	
	popupMenu.AppendMenu(MF_OWNERDRAW|((m_realTestFilter & (1 << MRTS_REAL)) ? MF_CHECKED : MF_UNCHECKED), MMSID_REAL, "Real");
	popupMenu.AppendMenu(MF_OWNERDRAW|((m_realTestFilter & (1 << MRTS_TEST)) ? MF_CHECKED : MF_UNCHECKED), MMSID_TEST, "Test");
	AddMenuSeparator(popupMenu);
	popupMenu.AppendMenu(MF_OWNERDRAW|((m_marketCategoryFilter & (1 << MCF_COUNT) - 1) != (1 << MCF_COUNT) - 1 ? MF_ENABLED : MF_GRAYED), MMSID_MARKET_CATEGORY_ALL, "All");
	popupMenu.AppendMenu(MF_OWNERDRAW|(m_marketCategoryFilter ? MF_ENABLED : MF_GRAYED), MMSID_MARKET_CATEGORY_NONE, "None");
	unsigned int id = MMSID_Last;
	for(unsigned char i = 0; i < MCF_COUNT; ++i, ++id)
	{
		popupMenu.AppendMenu(MF_OWNERDRAW|((m_marketCategoryFilter & (1 << i)) ? MF_CHECKED : MF_UNCHECKED), id, TU_GetMarketCategoryName(i));
	}
	menu.AppendMenu(MF_POPUP, (UINT_PTR)popupMenu.Detach(), "Filter");
*/
	menu.AppendMenu(MF_OWNERDRAW, TMID_FILTER, "Filter...");

	TakionTool* tool = TGUI_GetTakionMainWnd()->GetActiveCommandTool();
	if(tool && tool->GetSecurity())
	{
		unsigned char basket;
		const char* symbol = tool->GetStringSymbolPtr(basket);
		if(basket == EST_STOCK && symbol && *symbol)//!symbol->empty())
		{
			const unsigned __int64 numSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(symbol, sizeof(unsigned __int64) - 1);//U_StringToUInt64Term0(symbol);
			popupMenu.CreatePopupMenu();
			popupMenu.AppendMenu(MF_OWNERDRAW, TMID_FIND, "Find");
/*
			popupMenu.AppendMenu(MF_SEPARATOR);
			popupMenu.AppendMenu(MF_STRING|(included ? MF_GRAYED : MF_ENABLED), TMID_INCLUDE, "Include");
			popupMenu.AppendMenu(MF_STRING|(excluded ? MF_GRAYED : MF_ENABLED), TMID_EXCLUDE, "Exclude");
			popupMenu.AppendMenu(MF_STRING|(included ? MF_ENABLED : MF_GRAYED), TMID_UNINCLUDE, "UnInclude");
			popupMenu.AppendMenu(MF_STRING|(excluded ? MF_ENABLED : MF_GRAYED), TMID_UNEXCLUDE, "UnExclude");
*/
			AddMenuSeparator(menu);
			menu.AppendMenu(MF_POPUP, (UINT_PTR)popupMenu.Detach(), symbol);//->c_str());
		}
	}
}

void MyMarketSorter::PostAddMenuItems(CMenu& menu)
{
	RowWindowSortableAccount::PostAddMenuItems(menu);
}

void MyMarketSorter::NextDayStarted()
{
	RowWindowSortableAccount::NextDayStarted();
}

void MyMarketSorter::LineSelected()
{
	RowWindowSortableAccount::LineSelected();
}

void MyMarketSorter::BeforeDestroy()
{
	RowWindowSortableAccount::BeforeDestroy();
	mainDialog->TakionWindowRepaintTool(this);
}

bool MyMarketSorter::isRowActiveSymbol(const RowSortable* row) const
{
	const Security* security = ((MarketSorterRow*)row)->GetSecurity();
	return security->GetNumericSymbol() == m_activeSymbol;
}

RowSortable* MyMarketSorter::FindRowActiveSymbol(const unsigned __int64& newActiveSymbol
#ifndef TAKION_NO_OPTIONS
		,const unsigned __int64& newActiveOptionBlock
#endif
	) const
{
	const Security* security = TD_FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(newActiveSymbol);
	if(security)
	{
		const RowMap::CPair* found = m_rowMap.PLookup(security);
		return found ? found->value : nullptr;
	}
	return nullptr;
}

void MyMarketSorter::ActiveSymbolAboutToChange(const unsigned __int64& newActiveSymbol
#ifndef TAKION_NO_OPTIONS
		,const unsigned __int64& newActiveOptionBlock
#endif
	)
{
	RemoveActiveRowsTemporarily(m_prevTopRow, m_nextTopRow, newActiveSymbol
#ifndef TAKION_NO_OPTIONS
		,newActiveOptionBlock
#endif
		);
}

bool MyMarketSorter::ActiveSymbolChanged()
{
	return ReInsertActiveRows(m_prevTopRow, m_nextTopRow);
//	m_prevTopRow = m_nextTopRow = nullptr;
}

void MyMarketSorter::toString(std::string& str, unsigned int contentModifier) const
{
	RowWindowSortableAccount::toString(str, contentModifier);
//	U_AppendUInt(str, m_marketCategoryFilter, settingDel);
//	U_AppendUChar(str, m_realTestFilter, settingDel);

	m_filter.toString(str);

	str += sectionDel;
}

void MyMarketSorter::fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier)
{
	const Account* account = m_account;
	RowWindowSortableAccount::fromString(cursor, version, clone, contentModifier);
	if(account != m_account)AccountChanged();

	bool endChange;
	char* end = U_MarkSectionEnd((char*)cursor, sectionDel, endChange);
/*
	m_marketCategoryFilter = U_ReadUIntToken(cursor, settingDel, (1 << MCF_COUNT) - 1);
	m_marketCategoryFilter &= (1 << MCF_COUNT) - 1;

	m_realTestFilter = U_ReadUCharToken(cursor, settingDel, (1 << MRTS_Last) - 1);
	m_realTestFilter &= (1 << MRTS_Last) - 1;
*/
	if(version >= U_GetVersionFromTokens(1, 0, 5, 213))
	{
		MyMarketSorterFilter filter(m_filter);
		filter.fromString(cursor, version, clone, contentModifier);
		ApplyFilter(filter, true);
	}
	if(endChange)*end = sectionDel;
	cursor = end + 1;

/*
	RemoveNotBelongingRows(true);
	Repopulate();
	RepaintOnPopulation(true);
*/
}

void MyMarketSorter::fromScratch()
{
	RowWindowSortableAccount::fromScratch();

	AccountChanged();
//	UpdateTitle();
	Repopulate();
}

bool MyMarketSorter::CanClearSymbols() const
{
	return m_filter.isUseInclude() && !m_filter.isIncludeSetEmpty();//!m_filter.GetIncludeSymbolSet().empty();
}

void MyMarketSorter::ClearSymbols()
{
	if(m_filter.isUseInclude() && !m_filter.isIncludeSetEmpty())//!m_filter.GetIncludeSymbolSet().empty())
	{
		MySymbolFilter::UInt64Set toRemove = m_filter.GetIncludeSymbolSet();
		m_filter.ClearIncludeSymbolSet();
		bool changed = false;
		Security* security;

		const TakionMainWnd* takionMainWnd = TGUI_GetTakionMainWnd();
#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
		for(const MySymbolFilter::UInt64Set::CPair* p = toRemove.PGetFirstAssoc(); p; p = toRemove.PGetNextAssoc(p))
		{
			security = takionMainWnd->FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(p->key);
#else
		for(MySymbolFilter::UInt64Set::const_iterator it = toRemove.cbegin(), itend = toRemove.cend(); it != itend; ++it)
		{
			security = takionMainWnd->FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(*it);
#endif
			if(security)
			{
				NewRow(security);
				changed = true;
//				takionMainWnd->ReleaseStock(security);
			}
		}
		if(changed && m_hWnd)
		{
	//		InvalidateRect(&m_titleRect, FALSE);
			RepaintOnPopulation(false);
		}
	}
}

void MyMarketSorter::RemoveSelectedStocks()
{
	if(!m_filter.isUseExclude())
	{
		m_filter.ClearExcludeSymbolSet();
	}
	m_filter.SetUseExclude(true);
	const Security* security;
	if(m_multiselect)
	{
		if(!m_selectedSet.empty())
		{
			for(RowSortableSet::const_iterator it = m_selectedSet.cbegin(); it != m_selectedSetEnd; )
			{
				security = ((MarketSorterRow*)*it)->GetSecurity();
				++it;
				if(m_filter.ExcludeSymbol(security->GetNumericSymbol()))
				{
					NewRow(security);
				}
			}
		}
	}
	else
	{
		Row* row = GetSelectedRow();
		if(row)
		{
			security = ((MarketSorterRow*)row)->GetSecurity();
			if(m_filter.ExcludeSymbol(security->GetNumericSymbol()))
			{
				NewRow(security);
			}
		}
	}
}

void MyMarketSorter::PasteStocksFromString(const char* cursor, size_t size, bool replace)
{
	MySymbolFilter::UInt64Set toRemove;
	if(m_filter.isUseInclude())
	{
		if(replace)
		{
			toRemove = m_filter.GetIncludeSymbolSet();
		}
	}
	if(replace || !m_filter.isUseInclude())
	{
		m_filter.ClearIncludeSymbolSet();
	}
	m_filter.SetUseInclude(true);

	std::string symbol;
	char c;
	bool valid = true;
	unsigned __int64 key;

	unsigned int tokenCount = 0;
	unsigned int charCount = 0;
	bool spaceEncountered = false;
	const unsigned int maxCharCount = 7;
	Security* security;
	const TakionMainWnd* takionMainWnd = TGUI_GetTakionMainWnd();

	for(size_t i = 0; i < size && *cursor; ++i, ++cursor)
	{
		c = *cursor;
		switch(c)
		{
			case '\r':
			if(spaceEncountered)spaceEncountered = false;
			else ++tokenCount;
			break;

			case '\n':
			if(valid)
			{
				if(!symbol.empty())
				{
//add
					key = U_StringToUInt64Term0(symbol.c_str());
#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
					toRemove.RemoveKey(key);
#else
					toRemove.erase(key);
#endif
					security = takionMainWnd->FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(key);
					if(security)
					{
						if(m_filter.IncludeSymbol(key))
						{
							NewRow(security);
						}
					}
				}
			}
			else
			{
				valid = true;
			}
			charCount = 0;
			symbol.clear();
			tokenCount = 0;
			spaceEncountered = false;
			break;

			case '|':
			case '\t':
			if(spaceEncountered)spaceEncountered = false;
			else ++tokenCount;
			break;

			case ' ':
			if(!spaceEncountered && !symbol.empty())
			{
				spaceEncountered = true;
				++tokenCount;
			}
			break;

			default:
			if(valid && !spaceEncountered)
			{
				switch(tokenCount)
				{
					case 0://symbol
					c = toupper(c);
					if(U_IsStockCharValid(c, symbol.empty()))
					{
						++charCount;
						if(charCount > maxCharCount)
						{
							valid = false;
						}
						else
						{
							symbol += c;
						}
					}
					else
					{
						valid = false;
					}
					break;
					default:
					break;
				}
			}
			break;
		}
	}
	if(valid && !symbol.empty())
	{
//add
		key = U_StringToUInt64Term0(symbol.c_str());
#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
		toRemove.RemoveKey(key);
#else
		toRemove.erase(key);
#endif
		security = takionMainWnd->FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(key);
		if(security)
		{
			if(m_filter.IncludeSymbol(key))
			{
				NewRow(security);
			}
		}
	}
#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
	for(const SymbolFilter::UInt64Set::CPair* p = toRemove.PGetFirstAssoc(); p; p = toRemove.PGetNextAssoc(p))
	{
		security = takionMainWnd->FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(p->key);
#else
	for(SymbolFilter::UInt64Set::const_iterator it = toRemove.cbegin(), itend = toRemove.cend(); it != itend; ++it)
	{
		security = takionMainWnd->FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(*it);
#endif
		if(security)
		{
			NewRow(security);
		}
	}
	if(m_hWnd)
	{
		RepaintOnPopulation(false);
	}
}


bool MyMarketSorter::ProcessKeyDown(unsigned char key, unsigned char modifier, bool repeat, TakionFrame* fromWindow)
{
	if(!(modifier & (KM_SHIFT_LEFT|KM_CTRL_LEFT|KM_ALT_LEFT|KM_SHIFT_RIGHT|KM_CTRL_RIGHT|KM_ALT_RIGHT))
		&& (key >= 'A' && key <= 'Z' || key == VK_OEM_PERIOD))
	{
//		CFont* font = GetSymbolEntryFont();//m_visual->GetFont("Arial", -14, true);
		DialogTextEntry& symbolEntryDialog = TGUI_GetTakionMainWnd()->GetSymbolEntryDialog();
		CPoint point;
		GetEntryPoint(point);
		symbolEntryDialog.Display(true, true, this, nullptr, 0, this,
			U_GetValidStockChars(), false, true, U_GetValidStockFirstChars(), false, ".",
			RGB(255, 0, 255), RGB(0, 0, 192), 7, m_fontTitle, point.x, point.y, 1, 1);
		symbolEntryDialog.AddChar(key);

		return true;
	}
	return RowWindowSortableAccount::ProcessKeyDown(key, modifier, repeat, fromWindow);
}

void MyMarketSorter::AcceptText(const char* text, unsigned int code)
{
	switch(code)
	{
		case 0:
		if(GetKeyState(VK_CONTROL) & highBit)
		{
//			AddSymbol(text);
		}
		else
		{
			SelectSymbol(text);
		}
		break;

		default:
		RowWindowSortableAccount::AcceptText(text, code);
		break;
	}
}

void MyMarketSorter::SelectSymbol(const char* symbol)
{
	const Security* security = TGUI_GetTakionMainWnd()->FindSortableEquity(symbol);
	if(security)
	{
		SelectRowByObject(security);
	}
}

BOOL MyMarketSorter::OnCommand(WPARAM wParam, LPARAM lParam)
{
	const DWORD loword = (DWORD)wParam;//LOWORD(wParam);
	switch(loword)
    {
		case TMID_FIND:
		{
			TakionTool* tool = TGUI_GetTakionMainWnd()->GetActiveCommandTool();
			if(tool)
			{
				unsigned char basket;
				const char* symbol = tool->GetStringSymbolPtr(basket);
				if(basket == EST_STOCK && symbol && *symbol)//!symbol->empty())
				{
					SelectSymbol(symbol);
				}
			}
		}
		break;

		case TMID_COPY_ALL:
		CopyAllStocksToClipboard();
		break;

		case TMID_COPY:
		CopySelectedStocksToClipboard();
		break;

		case TMID_PASTE_STOCKS:
		PasteStocksFromClipboard((GetKeyState(VK_CONTROL) & highBit) != 0);//replace
		break;

		case TMID_REMOVE:
		RemoveSelectedStocks();
		break;

		case TMID_CLEAR:
		ClearSymbols();
		break;

/*
		case MMSID_REAL:
		SwitchRealTestFilter(MRTS_REAL);
		break;

		case MMSID_TEST:
		SwitchRealTestFilter(MRTS_TEST);
		break;

		case MMSID_MARKET_CATEGORY_ALL:
		SetMarketCategoryFilter((1 << MCF_COUNT) - 1);
		break;

		case MMSID_MARKET_CATEGORY_NONE:
		SetMarketCategoryFilter(0);
		break;
*/
		case TMID_FILTER:
		{
			MyDialogMarketSorterFilter* dlg = ExtensionSample::GetMarketSorterFilterDialog();
			const CRect& rect = dlg->GetDialogFrameRect();
			int w = rect.Width();//m_takionConfig->m_fontDlgSize.m_first.GetValue();
			int h = rect.Height();//m_takionConfig->m_fontDlgSize.m_second.GetValue();
			CRect r;
			GetPopupDlgRectFromDimensions(w, h, r);
			dlg->Display(m_filter, this, 0, this, r.left, r.top);
		}
		break;

		default:
//		if(loword >= MMSID_Last && loword < MMSID_Last + MCF_COUNT)
//			SwitchMarketCategoryFilter(loword - MMSID_Last);
//		else
		return RowWindowSortableAccount::OnCommand(wParam, lParam);
	}
    return TRUE;
}
/*
void MyMarketSorter::SwitchRealTestFilter(const MyRealTestStockEnum realTestFlag) 
{
	if(m_realTestFilter & (1 << realTestFlag))
	{
		RemoveRealTestFilter(1 << realTestFlag);
	}
	else
	{
		AddRealTestFilter(1 << realTestFlag);
	}
}

void MyMarketSorter::AddRealTestFilter(const unsigned char realTestMask)
{
	m_realTestFilter |= realTestMask;
	Repopulate();
	RepaintOnPopulation(true);
}

void MyMarketSorter::RemoveRealTestFilter(const unsigned char realTestMask)
{
	m_realTestFilter &= ~realTestMask;
	RemoveNotBelongingRows(true);
	RepaintOnPopulation(true);
}

void MyMarketSorter::SetMarketCategoryFilter(const unsigned int marketCategoryFilter)
{
	if(marketCategoryFilter != m_marketCategoryFilter)
	{
		const unsigned int intersection = m_marketCategoryFilter & marketCategoryFilter;
		bool add = intersection != marketCategoryFilter;
		bool remove = intersection != m_marketCategoryFilter;
		m_marketCategoryFilter = marketCategoryFilter;

		if(remove)RemoveNotBelongingRows(true);
		if(add)Repopulate();

		RepaintOnPopulation(true);
	}
}

void MyMarketSorter::SwitchMarketCategoryFilter(const unsigned int marketCategoryFlag)
{
	if(m_marketCategoryFilter & (1 << marketCategoryFlag))
	{
		RemoveMarketCategoryFilter(1 << marketCategoryFlag);
	}
	else
	{
		AddMarketCategoryFilter(1 << marketCategoryFlag);
	}
}

void MyMarketSorter::AddMarketCategoryFilter(const unsigned int marketCategoryMask)
{
	m_marketCategoryFilter |= marketCategoryMask;
	Repopulate();
	RepaintOnPopulation(true);
}

void MyMarketSorter::RemoveMarketCategoryFilter(const unsigned int marketCategoryMask)
{
	m_marketCategoryFilter &= ~marketCategoryMask;
	RemoveNotBelongingRows(true);
	RepaintOnPopulation(true);
}
*/
void MyMarketSorter::DoApplyFilter(const TakionFilter& filter, bool forceFilterChanged)
{
	const bool filteringDidNotChange = !forceFilterChanged && m_filter.isFilterSame(filter);
	const MarketSorterFilter& otherFilter = (const MarketSorterFilter&)filter;
/*
	const bool loadRsiNew = otherFilter.isRsiMhCalculate() && !otherFilter.isRsiMhTodayOnly() && otherFilter.GetRsiMhPriorPointCount() != 0;
	const bool loadRsiOld = m_filter.isRsiMhCalculate() && !m_filter.isRsiMhTodayOnly() && m_filter.GetRsiMhPriorPointCount() != 0;
	const bool startMinuteChanged = m_filter.GetRsiMhStartMinute() != otherFilter.GetRsiMhStartMinute();

	bool rsiMhChanged = m_filter.GetRsiMhPriorPointCount() != otherFilter.GetRsiMhPriorPointCount()
		|| m_filter.GetRsiMhPeriodCount() != otherFilter.GetRsiMhPeriodCount()
		|| m_filter.GetRsiMhPeriodLength() != otherFilter.GetRsiMhPeriodLength()
		|| startMinuteChanged//m_filter.GetRsiMhStartMinute() != otherFilter.GetRsiMhStartMinute()
		|| m_filter.isRsiMhCalculate() != otherFilter.isRsiMhCalculate()
		|| m_filter.isRsiMhTodayOnly() != otherFilter.isRsiMhTodayOnly()
		|| m_filter.isRsiMhPostMarket() != otherFilter.isRsiMhPostMarket();

	const bool loadRsiDaysNew = otherFilter.isRsiDaysCalculate() && otherFilter.GetRsiDaysPriorPointCount() != 0;
	const bool loadRsiDaysOld = m_filter.isRsiDaysCalculate() && m_filter.GetRsiDaysPriorPointCount() != 0;
	const bool startMinuteDaysChanged = m_filter.GetRsiDaysStartMinute() != otherFilter.GetRsiDaysStartMinute();

	bool rsiDaysChanged = m_filter.GetRsiDaysPriorPointCount() != otherFilter.GetRsiDaysPriorPointCount()
		|| m_filter.GetRsiDaysPeriodCount() != otherFilter.GetRsiDaysPeriodCount()
		|| m_filter.GetRsiDaysPeriodLength() != otherFilter.GetRsiDaysPeriodLength()
		|| startMinuteDaysChanged//m_filter.GetRsiMhStartMinute() != otherFilter.GetRsiMhStartMinute()
		|| m_filter.isRsiDaysCalculate() != otherFilter.isRsiDaysCalculate()
		|| m_filter.isRsiDaysPostMarket() != otherFilter.isRsiDaysPostMarket();
*/
//	const unsigned short sortId = GetSortId();
	bool sortingChanged = forceFilterChanged;
//		|| m_filter.isSortQuotesByActivity() != otherFilter.isSortQuotesByActivity() && (sortId == MS_THERMOGRAPH_QUOTES || m_sortChain.Contains(MS_THERMOGRAPH_QUOTES))
//		|| m_filter.isSortPrintsByActivity() != otherFilter.isSortPrintsByActivity() && (sortId == MS_THERMOGRAPH_PRINTS || m_sortChain.Contains(MS_THERMOGRAPH_PRINTS))
			
//		|| rsiMhChanged && (sortId == MS_RSI_MH || m_sortChain.Contains(MS_RSI_MH))
//		|| rsiDaysChanged && (sortId == MS_RSI_DAYS || m_sortChain.Contains(MS_RSI_DAYS))
//		;

	const unsigned char samePotential = m_filter.isPotentialFilterSame(filter);
	const unsigned char same = forceFilterChanged ? 3 : samePotential;
	const bool canUseContents = !(samePotential & 1);//no add
	m_filter.Copy(otherFilter);
/*
	if(rsiMhChanged)
	{
		if(loadRsiNew != loadRsiOld)
		{
			takionMainWnd->LoadRsiMhData(loadRsiNew);
		}
		UpdateRsiMhParams();
		if(startMinuteChanged)
		{
			InvalidateDataAtColumn(MS_RSI_MH);
		}
	}
	if(rsiDaysChanged)
	{
		if(loadRsiDaysNew != loadRsiDaysOld)
		{
			takionMainWnd->LoadRsiDaysData(loadRsiDaysNew);
		}
		UpdateRsiDaysParams();
		if(startMinuteDaysChanged)
		{
			InvalidateDataAtColumn(MS_RSI_DAYS);
		}
	}
	if(UpdateTimeFrame(m_filter.GetTimeFrameMinutes(), sortingChanged))
	{
		if(!filteringDidNotChange)
		{
			if(same & 1)
			{
				RepopulateOnFiltering();
			}
		}
	}
	else
*/
	if(!filteringDidNotChange)
	{
		if(!same || (same & 2))
		{
			UpdatePopulation(sortingChanged);
		}
		if(same & 1)
		{
			Repopulate();
		}
	}
	else if(sortingChanged)
	{
		SortData();
	}
	if(!canUseContents)
	{
		AddNewRows();
	}
}
