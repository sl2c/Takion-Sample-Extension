#include "stdafx.h"
#include "StockComparator.h"
//#include "ExtensionDerivedConfig.h"
#include "MainDialog.h"
#include "DialogStockPair.h"
#include "DialogSignedPriceValue.h"

//extern ExtensionDerivedConfig* extensionConfig;
extern MainDialog* mainDialog;
extern DialogStockPair* stockPairDialog;
extern DialogSignedPriceValue* signedPriceValueDialog;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char* const StockComparator::stockComparatorId = "StockComparator";
const char* const StockComparator::stockFileExtension = "txt";

void SecurityPriceAsUIntRowValueDiffNull::CalculatePrice(unsigned int price, unsigned int price2, SignedPrice& value)
{
	if(price && price2)
	{
		value = SignedPrice(price) - SignedPrice(price2);
	}
	else
	{
		value.SetZero();
	}
}

bool SecurityPriceAsUIntRowValueDiffNull::isValueChanged() const
{
	return m_price != (m_security ? (m_security->*m_priceFunction)() : 0) || m_price2 != (m_security2 ? (m_security2->*m_priceFunction)() : 0);
}
short SecurityPriceAsUIntRowValueDiffNull::CompareAhead(const RowValue& other) const
{
	SignedPrice value;
	CalculatePrice(m_security ? (m_security->*m_priceFunction)() : 0, m_security2 ? (m_security2->*m_priceFunction)() : 0, value);
	return -((const TypedRowValue&)other).CompareValue(value);
}
bool SecurityPriceAsUIntRowValueDiffNull::UpdateValue()
{
	m_price = m_security ? (m_security->*m_priceFunction)() : 0;
	m_price2 = m_security2 ? (m_security2->*m_priceFunction)() : 0;
	SignedPrice value;
	CalculatePrice(m_price, m_price2, value);
	return SetValue(value);
}
bool SecurityPriceAsUIntRowValueDiffNull::isObjectValueEmpty() const
{
	return m_useEmptyValue && (!m_security || !m_security2 || !(m_security->*m_priceFunction)() || !(m_security2->*m_priceFunction)());
}
bool SecurityPriceAsUIntRowValueDiffNull::isValueEmpty() const
{
	return m_useEmptyValue && (!m_price || !m_price2);
}
void SecurityPriceAsUIntRowValueDiffNull::InitializeValue()
{
	m_price = m_security ? (m_security->*m_priceFunction)() : 0;
	m_price2 = m_security2 ? (m_security2->*m_priceFunction)() : 0;
	CalculatePrice(m_price, m_price2, m_value);
}

bool ScUnappliedMessageReader::ProcessDataToken(const char* cursor, unsigned short length) const
{
	const Message* message = (const Message*)cursor;
	switch(message->m_type)
	{
		case TM_MS_TRADE_SHORT_INTERNAL:
		{
			const TMsgMsTradeShortInternal* msg = (const TMsgMsTradeShortInternal*)message;
//			const Security* security = m_row->GetSecurity();
			m_row->AddPrint(Price(msg->m_price), msg->m_size, msg->m_marketCenterID, msg->m_printFlags);
		}
		return true;

		case TM_MS_TRADE_LONG_INTERNAL:
		{
			const TMsgMsTradeLongInternal* msg = (const TMsgMsTradeLongInternal*)message;
//			const Security* security = m_row->GetSecurity();
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

void SecurityChartRow2::Notify(const Message* message, const Observable* from, const Message* info)
{
	m_row->Notify(message, from, info);
}

StockComparatorRow::StockComparatorRow(StockComparator* parent, StockPair* stockPair, Position* position1, Position* position2, bool potential):
//	RowSortable(MarketSorterWindow::MS_COLUMN_COUNT + msAdditionalColumnCount, parent, potential),
	RowSortable(parent->GetBuiltInColumnCount(), parent, potential),
	SecurityChartRow(stockPair->GetSecurity1(), position1, parent->GetTimeFrame(), parent->GetTimeFrameInSeconds()),
	m_securityChartRow(this, stockPair->GetSecurity2(), position2, parent->GetTimeFrame(), parent->GetTimeFrameInSeconds()),
	m_stockPair(stockPair),
	m_stockComparatorWindow(parent),
	m_securityColor(RGB(255, 255, 255)),
	m_unappliedMessageReader(this, m_security ? m_security->GetUnappliedMessages() : nullptr),
	m_unappliedMessageReader2(&m_securityChartRow, m_securityChartRow.GetSecurity() ? m_securityChartRow.GetSecurity()->GetUnappliedMessages() : nullptr)
{
	SetRowValue(StockComparator::SC_PRICE_1, new PositionSecurityCompactPriceRowValue(m_stockPair->GetSecurityRef1(), parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetLastPrice), true);
	SetRowValue(StockComparator::SC_PRICE_2, new PositionSecurityCompactPriceRowValue(m_stockPair->GetSecurityRef2(), parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetLastPrice), true);
	SetRowValue(StockComparator::SC_PRICE_DIFF, new SecurityPriceAsUIntRowValueDiffNull(m_stockPair->GetSecurityRef1(), m_stockPair->GetSecurityRef2(), parent->isCommaPrice(), parent->GetDecDigits(), parent->isTruncatePrice(), &Security::GetLastPrice), true);
	SetRowValue(StockComparator::SC_POS_SIZE_1, new PositionIntRowValueNull(m_position, parent->isCommaSize(), &Position::GetSize, true), true);
	SetRowValue(StockComparator::SC_POS_SIZE_2, new PositionIntRowValueNull(m_securityChartRow.GetPosition(), parent->isCommaSize(), &Position::GetSize, true), true);

#ifndef USE_DYNAMIC_ARRAY
	UpdateRowValueIterators();
#endif
	UpdateSecurity();
}

StockComparatorRow::~StockComparatorRow()
{
}

bool StockComparatorRow::SetSecurity1(const Security* security)
{
	bool ret = SecurityChartRow::SetSecurity(security);
	UpdateSecurity();
	return ret;
}

bool StockComparatorRow::SetSecurity2(const Security* security)
{
	bool ret = m_securityChartRow.SetSecurity(security);
	UpdateSecurity();
	return ret;
}

void StockComparatorRow::UpdateSecurity()
{
	Lock(true);
	Initialize();
	UpdateRowColor();
	Unlock();
}

bool StockComparatorRow::SetSecurity(const Security* security)
{
	switch(m_stockPair->SetSecurity(security))
	{
		case 1:
		SetSecurity1(security);
		return true;

		case 2:
		SetSecurity2(security);
		return true;

		default:
		return false;
	}
/*
	bool ret = false;
	unsigned __int64 numericSymbol = security->GetNumericSymbol();
	if(m_stockPair->GetFirst() == numericSymbol)
	{
		SetSecurity1(security);
	}
	else if(m_stockPair->GetSecond() == numericSymbol)
	{
		SetSecurity2(security);
	}
	return ret;
*/
}

bool StockComparatorRow::PaintCell(CDC& dc, const Column* column, CRect& rect, bool bottomBase, UINT verticalJustification, int horizontalOffset, int verticalOffset) const
//bool StockComparatorRow::PaintCell(CDC& dc, const Column* column, const CRect& rect, UINT verticalJustification, int horizontalOffset) const
{
	unsigned short columnId = column->GetId();
	COLORREF color = GetCellColor(columnId);
	if(color != 0xFFFFFFFF)
	{
		color = dc.SetTextColor(color);
	}
	CRect r(rect.left + horizontalOffset, rect.top, rect.right - horizontalOffset, rect.bottom);
	switch(column->GetAlignment())
	{
		case 0:
		drawHorizontalJustification = DT_RIGHT;
		break;

		case 1:
		drawHorizontalJustification = DT_LEFT;
		break;

		case 2:
		drawHorizontalJustification = DT_CENTER;
		break;

		default:
		drawHorizontalJustification = DT_RIGHT;
		break;
	}
	bool ret = true;
	switch(columnId)
	{
		case StockComparator::SC_SYMBOL_1:
		if(!column->GetAlignment())
		{
			drawHorizontalJustification = DT_LEFT;
		}
		dc.DrawText((const char*)&m_stockPair->GetFirst(), -1, &r, drawHorizontalJustification | verticalJustification | DT_SINGLELINE | DT_NOPREFIX);
		break;

		case StockComparator::SC_SYMBOL_2:
		if(!column->GetAlignment())
		{
			drawHorizontalJustification = DT_LEFT;
		}
		dc.DrawText((const char*)&m_stockPair->GetSecond(), -1, &r, drawHorizontalJustification | verticalJustification | DT_SINGLELINE | DT_NOPREFIX);
		break;

		case StockComparator::SC_VALUE:
		if(!m_stockPair->GetValue().isZero())
		{
			std::string text;
			U_SignedPriceToStr(text, m_stockPair->GetValue(), *m_windowSortable->GetDecDigits(), -1, *m_windowSortable->isCommaPrice(), '\0', *m_windowSortable->isTruncatePrice());
			dc.DrawText(text.c_str(), -1, &r, drawHorizontalJustification | verticalJustification | DT_SINGLELINE | DT_NOPREFIX);
		}
		break;

		default:
		ret = RowSortable::PaintCell(dc, column, rect, bottomBase, verticalJustification, horizontalOffset, verticalOffset);
		break;
	}
	if(color != 0xFFFFFFFF)
	{
		dc.SetTextColor(color);
	}
	return ret;
}

bool StockComparatorRow::CellToString(std::string& info, unsigned short columnId) const
{
	switch(columnId)
	{
		case StockComparator::SC_SYMBOL_1:
		info += (const char*)&m_stockPair->GetFirst();
		break;

		case StockComparator::SC_SYMBOL_2:
		info += (const char*)&m_stockPair->GetSecond();
		break;

		case StockComparator::SC_VALUE:
		U_SignedPriceToStr(info, m_stockPair->GetValue(), *m_windowSortable->GetDecDigits(), -1, *m_windowSortable->isCommaPrice(), '\0', *m_windowSortable->isTruncatePrice());
		break;

		default:
		return RowSortable::CellToString(info, columnId);
//		break;
	}
	return true;
}

COLORREF StockComparatorRow::GetRowColor() const
{
	return m_securityColor;//0xFFFFFFFF;//each cell has its own color
}

COLORREF StockComparatorRow::GetRowBkColor() const
{
	return 0xFFFFFFFF;//each cell has its own color
}

COLORREF StockComparatorRow::GetCellColor(unsigned short columnId) const
{
	switch(columnId)
	{
		case StockComparator::SC_SYMBOL_1:
		return m_stockPair->GetSecurity1() ? RGB(255, 255, 255) : RGB(255, 0, 0);

		case StockComparator::SC_SYMBOL_2:
		return m_stockPair->GetSecurity2() ? RGB(255, 255, 255) : RGB(255, 0, 0);

		case StockComparator::SC_VALUE:
		return m_stockPair->GetValue().isNegative() ? RGB(255, 0, 0) : RGB(0, 255, 0);
		break;

		default:
		return 0xFFFFFFFF;//RGB(255, 255, 255);
	}
}

COLORREF StockComparatorRow::GetCellBkColor(unsigned short columnId) const
{
	return 0xFFFFFFFF;//same as window bk color
}

RowValue* StockComparatorRow::CreateCell(unsigned short id)
{
	bool dynamic = true;
	RowValue* rowValue;
	switch(id)
	{
		case StockComparator::SC_PRICE_1:
		rowValue = new PositionSecurityCompactPriceRowValue(m_stockPair->GetSecurityRef1(), m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetLastPrice);
		break;

		case StockComparator::SC_PRICE_2:
		rowValue = new PositionSecurityCompactPriceRowValue(m_stockPair->GetSecurityRef2(), m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetLastPrice);
		break;

		case StockComparator::SC_PRICE_DIFF:
		rowValue = new SecurityPriceAsUIntRowValueDiffNull(m_stockPair->GetSecurityRef1(), m_stockPair->GetSecurityRef2(), m_windowSortable->isCommaPrice(), m_windowSortable->GetDecDigits(), m_windowSortable->isTruncatePrice(), &Security::GetLastPrice);
		break;

		case StockComparator::SC_POS_SIZE_1:
		rowValue = new PositionIntRowValueNull(m_position, m_windowSortable->isCommaSize(), &Position::GetSize, true);
		break;

		case StockComparator::SC_POS_SIZE_2:
		rowValue = new PositionIntRowValueNull(m_securityChartRow.GetPosition(), m_windowSortable->isCommaSize(), &Position::GetSize, true);
		break;

		default:
		return nullptr;
	}
	SetRowValue(id, rowValue, dynamic);
	return rowValue;
}

bool StockComparatorRow::UpdatePrints(bool lock)
{
//	unsigned short thermosecond = (unsigned short)(TL_GetCurrentMillisecond() >> 11);
//	m_thermographPrintsGreen.AddValue(thermosecond, m_security->GetMoveCountPrintGreenLocked());
//	m_thermographPrintsRed.AddValue(thermosecond, m_security->GetMoveCountPrintRedLocked());
//	m_thermographQuotesGreen.AddValue(thermosecond, m_security->GetMoveCountQuoteGreenLocked());
//	m_thermographQuotesRed.AddValue(thermosecond, m_security->GetMoveCountQuoteRedLocked());

	if(m_windowSortable->UpdateRow(this, lock, true))
	{
		return true;
	}
	return false;
}

void StockComparatorRow::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_EQUITY_IMBALANCE_UPDATE:
//		if(from == m_security)

		if(m_stockComparatorWindow->UpdateRow(this, true, true))
		{
			delete this;
		}
		break;

		case TM_UPDATED_POSITION:
		if(m_stockComparatorWindow->UpdateRow(this, true, true))
		{
			delete this;
		}
		break;

		case TM_POSITION_DELETE:
		if(SetPosition(nullptr) && m_stockComparatorWindow->UpdateRow(this, true, true))delete this;
		break;

		case TM_EQUITY_NEW_MINUTE:
		{
			const TMsgEquityNewMinute* msg = (const TMsgEquityNewMinute*)message;
			if(SetCurrentMinute(msg->m_minute, msg->m_fromServer))
			{
				if(m_stockComparatorWindow->UpdateRow(this, true, true))
				{
					delete this;
				}
			}
		}
		break;

		case TM_EQUITY_CHART_LOADED:
#ifndef TAKION_NO_OPTIONS
		case TM_FUTURE_CHART_LOADED:
#endif
		Lock(true);
		if(from == m_security)
		{
			m_chartLoaded = true;
			InitializeChart(2);
		}
		else if(from == m_securityChartRow.GetSecurity())
		{
			m_securityChartRow.SetChartLoaded(true);
			m_securityChartRow.InitializeChart(2);
		}
		if(m_stockComparatorWindow->UpdateRow(this, false, true))
		{
			Unlock();
			delete this;
		}
		else
		{
			Unlock();
		}
		break;

//		case TM_EQUITY_REFRESHED:
//		break;

		case TM_EQUITY_UPDATE:
		if(((const TMsgEquityUpdate*)message)->m_hasPrints)
		{
			Lock(true);
			m_unappliedMessageReader.ProcessAllData();
			m_unappliedMessageReader2.ProcessAllData();
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
		else if(m_stockComparatorWindow->UpdateRow(this, true, true))delete this;
		break;

		case TM_EQUITY_CLOSED:
		case TM_EQUITY_OPENED:
		case M_MS_EOD_SUMMARY_REPORT:
		case SM_MS_EOD_SUMMARY_REPORT:
		case TM_EQUITY_YEST_CLOSE_PRICE_CHANGED:
		case M_MS_SHORT_SALE_INDICATORS:
		case SM_MS_SHORT_SALE_INDICATORS:
		case M_MS_DIVIDENDS_AND_SPLITS:
		case SM_MS_DIVIDENDS_AND_SPLITS:
		case M_MS_52WEEK_30DAY_HIGH_LOW:
		case SM_MS_52WEEK_30DAY_HIGH_LOW:
		case M_MS_LULD:
		case SM_MS_LULD:
		case M_MS_PMI:
		case SM_MS_PMI:
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
		case TM_SET_EQUITY_TIER_SIZE:
		case TM_SET_EQUITY_CUSTOM_PRICE_BASE:
		case TM_SET_EQUITY_CUSTOM_DATE:
		case TM_EQUITY_BORROW_PRICE:
		case TM_EQUITY_BETA_AND_RETAIL_INFO:
		case TM_EQUITY_BORROW_INFO:
		case TM_EQUITY_HTB:
		if(m_stockComparatorWindow->UpdateRow(this, true, false))delete this;
		break;

		case TM_SORTABLE_SECURITY_INVALID:
		if(*(unsigned __int64*)((const TMessageSymbol*)message)->m_symbol == m_security->GetNumericSymbol())
		{
			RowSortable* row = m_stockComparatorWindow->RemoveRow(this);
			if(row)
			{
				delete row;
			}
		}
		break;

		case TM_SORTABLE_SECURITY_DELETE:
		if(m_security == ((const TMsgSecurity*)message)->m_security)
		{
			RowSortable* row = m_stockComparatorWindow->RemoveRow(this);
			if(row)
			{
				delete row;
			}
		}
		break;

		default:
		if(m_stockComparatorWindow->UpdateRow(this, true, true))delete this;
		break;
	}
}

bool StockComparatorRow::Lock1(bool wait) const
{
	if(m_security)
	{
		if(wait)
		{
			m_security->LockInquiryWaitLevel1();
			if(m_position)
			{
				m_position->LockInquiryWait();
			}
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

bool StockComparatorRow::Lock2(bool wait) const
{
	const Security* security = m_securityChartRow.GetSecurity();
	if(security)
	{
		if(wait)
		{
			security->LockInquiryWaitLevel1();
			const Position* position = m_securityChartRow.GetPosition();
			if(position)
			{
				position->LockInquiryWait();
			}
			return true;
		}
		else if(security->LockInquiryLevel1())
		{
			const Position* position = m_securityChartRow.GetPosition();
			if(!position || position->LockInquiry())
			{
				return true;
			}
			security->UnlockInquiryLevel1();
		}
		return false;
	}
	return true;
}
/*
void StockComparatorRow::LockWait1() const
{
	if(m_security)
	{
		m_security->LockInquiryWaitLevel1();
		if(m_position)
		{
			m_position->LockInquiryWait();
		}
	}
}

void StockComparatorRow::LockWait2() const
{
	const Security* security = m_securityChartRow.GetSecurity();
	if(security)
	{
		security->LockInquiryWaitLevel1();
		const Position* position = m_securityChartRow.GetPosition();
		if(position)
		{
			position->LockInquiryWait();
		}
	}
}
*/
void StockComparatorRow::Unlock1() const
{
	if(m_security)
	{
		m_security->UnlockInquiryLevel1();
		if(m_position)
		{
			m_position->UnlockInquiry();
		}
	}
}

void StockComparatorRow::Unlock2() const
{
	const Security* security = m_securityChartRow.GetSecurity();
	if(security)
	{
		security->UnlockInquiryLevel1();
		const Position* position = m_securityChartRow.GetPosition();
		if(position)
		{
			position->UnlockInquiry();
		}
	}
}

bool StockComparatorRow::Lock(bool wait) const
{
	if(Lock1(wait))
	{
		if(Lock2(wait))
		{
			return true;
		}
		Unlock1();
	}
	return false;
}
/*
void StockComparatorRow::LockWait() const
{
	LockWait1();
	LockWait2();
}
*/
void StockComparatorRow::Unlock() const
{
	Unlock1();
	Unlock2();
}

bool StockComparatorRow::SetPosition(const Position* position)
{
	bool ret = false;
	unsigned __int64 numericSymbol = position->GetNumericSymbol();
	if(m_stockPair->GetFirst() == numericSymbol)
	{
		ret = SetPosition1(position);
	}
	else if(m_stockPair->GetSecond() == numericSymbol)
	{
		ret = SetPosition2(position);
	}
	return ret;
}

bool StockComparatorRow::SetPosition1(const Position* position)
{
	if(SecurityChartRow::SetPosition(position))
	{
		ApplyPosition(StockComparator::SC_POS_SIZE_1, position);
		return true;
	}
	return false;
}

bool StockComparatorRow::SetPosition2(const Position* position)
{
	if(m_securityChartRow.SetPosition(position))
	{
		ApplyPosition(StockComparator::SC_POS_SIZE_2, position);
		return true;
	}
	return false;
}

bool StockComparatorRow::isSortValueChanged() const
{
	return RowSortable::isSortValueChanged();
}

bool StockComparatorRow::isChainSortValueChanged() const
{
	return RowSortable::isChainSortValueChanged();
}

short StockComparatorRow::Compare(const RowSortable& other) const
{
	short ret = 0;
	unsigned short sortId = m_windowSortable->GetSortId();
	switch(sortId)
	{
		case StockComparator::SC_SYMBOL_1:
		ret = U_CompareUIntAsString(m_stockPair->GetFirst(), ((const StockComparatorRow&)other).m_stockPair->GetFirst());
		break;

		case StockComparator::SC_SYMBOL_2:
		ret = U_CompareUIntAsString(m_stockPair->GetSecond(), ((const StockComparatorRow&)other).m_stockPair->GetSecond());
		break;

		case StockComparator::SC_VALUE:
		{
			const SignedPrice& value = m_stockPair->GetValue();
			const SignedPrice& valueOther = ((const StockComparatorRow&)other).m_stockPair->GetValue();
			ret = value < valueOther ? -1 : valueOther < value ? 1 : 0;
		}
		break;

		default:
		return RowSortable::Compare(other);
		break;
	}
	if(!ret)return CompareChain(other);
	if(!m_windowSortable->isSortAscending())ret = -ret;
	return ret;
}

short StockComparatorRow::CompareAhead(const RowSortable& other) const
{
	short ret = 0;
	switch(m_windowSortable->GetSortId())
	{
		case StockComparator::SC_SYMBOL_1:
		ret = U_CompareUIntAsString(m_stockPair->GetFirst(), ((const StockComparatorRow&)other).m_stockPair->GetFirst());
		break;

		case StockComparator::SC_SYMBOL_2:
		ret = U_CompareUIntAsString(m_stockPair->GetSecond(), ((const StockComparatorRow&)other).m_stockPair->GetSecond());
		break;

		case StockComparator::SC_VALUE:
		{
			const SignedPrice& value = m_stockPair->GetValue();
			const SignedPrice& valueOther = ((const StockComparatorRow&)other).m_stockPair->GetValue();
			ret = value < valueOther ? -1 : valueOther < value ? 1 : 0;
		}
		break;

		default:
		return RowSortable::CompareAhead(other);
	}
	if(!ret)return CompareChainAhead(other);
	if(!m_windowSortable->isSortAscending())ret = -ret;
	return ret;
}

short StockComparatorRow::CompareChain(const RowSortable& other) const
{
	short ret = 0;
	unsigned short sortId;
	const RowValue* rowValue;
	const Columns::UShortChain* chain = m_windowSortable->GetSortChain();
	const Columns::UShortChain::List& sortList = chain->GetList();
	for(Columns::UShortChain::List::const_iterator it = sortList.begin(), itend = sortList.end(); it != itend; ++it)
	{
		sortId = *it;

		switch(sortId)
		{
			case StockComparator::SC_SYMBOL_1:
			ret = U_CompareUIntAsString(m_stockPair->GetFirst(), ((const StockComparatorRow&)other).m_stockPair->GetFirst());
			break;

			case StockComparator::SC_SYMBOL_2:
			ret = U_CompareUIntAsString(m_stockPair->GetSecond(), ((const StockComparatorRow&)other).m_stockPair->GetSecond());
			break;

			case StockComparator::SC_VALUE:
			{
				const SignedPrice& value = m_stockPair->GetValue();
				const SignedPrice& valueOther = ((const StockComparatorRow&)other).m_stockPair->GetValue();
				ret = value < valueOther ? -1 : valueOther < value ? 1 : 0;
			}
			break;

			default:
			rowValue = GetRowValue(sortId);
			if(rowValue)
			{
				const RowValue& rowValueOther = *other.GetRowValue(sortId);
				if(rowValue->isValueEmpty())
				{
					if(!rowValueOther.isValueEmpty())
					{
						return 1;
					}
				}
				else if(rowValueOther.isValueEmpty())
				{
					return -1;
				}
				else
				{
					ret = rowValue->Compare(rowValueOther);
				}
			}
			break;
		}
		if(ret)
		{
			if(!m_windowSortable->isColumnSortAscending(sortId))ret = -ret;
			return ret;
		}
	}
	ret = DefaultCompare(other);
	if(!m_windowSortable->isSortAscending())ret = -ret;
	return ret;
}

short StockComparatorRow::CompareChainAhead(const RowSortable& other) const
{
	short ret = 0;
	unsigned short sortId;
	const RowValue* rowValue;
	const Columns::UShortChain::List& sortList = m_windowSortable->GetSortChain()->GetList();
	for(Columns::UShortChain::List::const_iterator it = sortList.begin(), itend = sortList.end(); it != itend; ++it)
	{
		sortId = *it;
		switch(sortId)
		{
			case StockComparator::SC_SYMBOL_1:
			ret = U_CompareUIntAsString(m_stockPair->GetFirst(), ((const StockComparatorRow&)other).m_stockPair->GetFirst());
			break;

			case StockComparator::SC_SYMBOL_2:
			ret = U_CompareUIntAsString(m_stockPair->GetSecond(), ((const StockComparatorRow&)other).m_stockPair->GetSecond());
			break;

			case StockComparator::SC_VALUE:
			{
				const SignedPrice& value = m_stockPair->GetValue();
				const SignedPrice& valueOther = ((const StockComparatorRow&)other).m_stockPair->GetValue();
				ret = value < valueOther ? -1 : valueOther < value ? 1 : 0;
			}
			break;

			default:
			rowValue = GetRowValue(sortId);
			if(rowValue)
			{
				const RowValue& rowValueOther = *other.GetRowValue(sortId);
				if(rowValue->isObjectValueEmpty())
				{
					if(!rowValueOther.isValueEmpty())
					{
						return 1;
					}
				}
				else if(rowValueOther.isValueEmpty())
				{
					return -1;
				}
				else
				{
					ret = rowValue->CompareAhead(rowValueOther);
				}
			}
			break;
		}
		if(ret)
		{
			if(!m_windowSortable->isColumnSortAscending(sortId))
			{
				ret = -ret;
			}
			return ret;
		}
	}
	ret = DefaultCompare(other);
	if(!m_windowSortable->isSortAscending())
	{
		ret = -ret;
	}
	return ret;
}

short StockComparatorRow::DefaultCompare(const RowSortable& other) const
{
	short ret = U_CompareUIntAsString(m_stockPair->GetFirst(), ((const StockComparatorRow&)other).m_stockPair->GetFirst());
	if(!ret)
	{
		ret = U_CompareUIntAsString(m_stockPair->GetSecond(), ((const StockComparatorRow&)other).m_stockPair->GetSecond());
		if(!ret)
		{
			ret = RowSortable::DefaultCompare(other);
		}
	}
	return ret;
}

void StockComparatorRow::Update(bool dynamicOnly)//don't call this function. It is called by Takion.
{
	RowSortable::Update(dynamicOnly);
}

bool StockComparatorRow::UpdateRowColor()//don't call this function. It is called by Takion. Return true if the color changes and you have to repaint the whole line.
{
	return RowSortable::UpdateRowColor();
}

void StockComparatorRow::UpdateAndInvalidate(CRect& rowRect, bool dynamicOnly)//don't call this function. It is called by Takion.
{
	RowSortable::UpdateAndInvalidate(rowRect, dynamicOnly);
}

void StockComparatorRow::Initialize()//should be called only inside a lock
{
	InitializeChart(true);
	m_securityChartRow.InitializeChart(true);
	RowSortable::Initialize();
}

const Security* StockComparatorRow::GetSelectedSecurity() const
{
	return m_security;
}

StockComparator::StockComparator(const CBitmap* bitmap,
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
	m_marketSorterConnected(TD_IsMarketSorterConnected()),
	m_colorDisconnected(RGB(128, 128, 128)),
	m_marketSorterLoaded(TD_IsMarketSorterLoaded()),
	m_colorNotLoaded(RGB(128, 128, 0)),
	m_menuWidth(0)
{
	m_rowMap.InitHashTable(9001);
	m_stockPairMap.InitHashTable(9001);
	m_symbolStockPairMap.InitHashTable(9001);

	m_fileFilterString = "StockComparator Files (*.";
	m_fileFilterString += stockFileExtension;
	m_fileFilterString += ")|*.";
	m_fileFilterString += stockFileExtension;
	m_fileFilterString += "|All Files (*.*)|*.*||";

	m_saveColumnIdList.push_back(SC_SYMBOL_1);
	m_saveColumnIdList.push_back(SC_SYMBOL_2);
	m_saveColumnIdList.push_back(SC_VALUE);
	m_saveColumnIdListBegin = m_saveColumnIdList.begin();
	m_saveColumnIdListEnd = m_saveColumnIdList.end();
	m_saveColumnIdListSize = m_saveColumnIdList.size();

	TD_GetNewEquityObservable().AddObserver(this);
	TD_GetNewPositionObservable().AddObserver(this);
	TD_GetAdminObservable().AddObserver(this);

//	memset(&m_paintInfoFunction, 0, sizeof(m_paintInfoFunction));
//	memset(&m_compareFunction, 0, sizeof(m_compareFunction));
//	memset(&m_compareAheadFunction, 0, sizeof(m_compareAheadFunction));

	mainDialog->TakionWindowAdded(this);
}

StockComparator::~StockComparator()
{
	ClearAll();
	mainDialog->TakionWindowRemoved(this);
}

COLORREF StockComparator::GetBkColor() const
{
	return m_marketSorterConnected ? m_marketSorterLoaded ? m_backgroundColor : m_colorNotLoaded : m_colorDisconnected;
}

void StockComparator::DestroyStockPairMap()
{
	for(SymbolStockPairMap::CPair* ssp = m_symbolStockPairMap.PGetFirstAssoc(); ssp; ssp = m_symbolStockPairMap.PGetNextAssoc(ssp))
	{
		delete ssp->value;
	}
	m_symbolStockPairMap.RemoveAll();
	for(StockPairMap::CPair* p = m_stockPairMap.PGetFirstAssoc(); p; p = m_stockPairMap.PGetNextAssoc(p))
	{
		delete p->value;
	}
	m_stockPairMap.RemoveAll();
}

void StockComparator::OnHidden()
{
	mainDialog->TakionWindowRepaintTool(this);
}

void StockComparator::OnSuccessfulCreate()
{
/*
	bool AddColumn(const char* name,
		unsigned short id,
		int width,
		unsigned char useTick,// = 0,
		bool sortable,// = true,
		bool defaultSortAscending,// = false,
		const char* tooltip,// = nullptr,
		bool use,// = true,
		bool globalUse,// = true,
		bool createsCell,// = true,
		unsigned char followable,// = 1,
		bool alwaysUsed,// = false,
		bool paintText,// = true,
		bool useDefaultColor);// = false);
*/
	HDC hdc = ::GetDC(m_hWnd);
	if(hdc)
	{
		HGDIOBJ oldFont = nullptr;
		CFont* menuFont = m_visualBase->GetMenuFont();
		if(menuFont)
		{
			oldFont = ::SelectObject(hdc, menuFont->m_hObject);
		}
		CRect rect(0, 0, 0, 0);
		TakionBaseWnd::CalculateTextWidth(rect, hdc, "Set Value", m_menuWidth);
		TakionBaseWnd::AdjustTextWidth(rect, hdc, "Save as...", m_menuWidth);

		if(oldFont)
		{
			::SelectObject(hdc, oldFont);
		}
		::ReleaseDC(m_hWnd, hdc);
	}

	AddColumn("Symbol1", SC_SYMBOL_1, 60, 0, true, true, "First Stock Symbol", true, true, true, 1, false, true, false, DT_LEFT);
	AddColumn("Symbol2", SC_SYMBOL_2, 60, 0, true, true, "Second Stock Symbol", true, true, true, 1, false, true, false, DT_LEFT);
	AddColumn("Price1", SC_PRICE_1, 80, 0, true, false, "First Stock Price", true, true, true, 1, false, true, false, DT_RIGHT);
	AddColumn("Price2", SC_PRICE_2, 80, 0, true, false, "Second Stock Price", true, true, true, 1, false, true, false, DT_RIGHT);
	AddColumn("PriceDiff", SC_PRICE_DIFF, 80, 0, true, false, "Price Difference", true, true, true, 1, false, true, false, DT_RIGHT);
	AddColumn("Value", SC_VALUE, 80, 0, true, false, "Value", true, true, true, 1, false, true, false, DT_RIGHT);
	AddColumn("PosSize1", SC_POS_SIZE_1, 60, 0, true, false, "First Position Size", true, true, true, 1, false, true, false, DT_RIGHT);
	AddColumn("PosSize2", SC_POS_SIZE_2, 60, 0, true, false, "Second Position Size", true, true, true, 1, false, true, false, DT_RIGHT);

	RowWindowSortableAccount::OnSuccessfulCreate();

	mainDialog->TakionWindowRepaintTool(this);
}

void StockComparator::ToolAdded()
{
	if(!m_fontRowCustom)
	{
		SetFontRow(m_fontDescriptionRow);
	}
	m_rowVerticalSpacing = SPACING_SMALL;//SPACING_MEDIUM;//graphics->GetVerticalSpacing();
	m_rowHorizontalSpacing = SPACING_EXTRA_LARGE;//SPACING_MEDIUM;//graphics->GetHorizontalSpacing();
	UpdateRowMetrics();
}

TakionTool* StockComparator::Clone() const
{
//	return new StockComparator(m_bitmap, m_workspace, m_pinned);
	return new StockComparator(nullptr, m_workspace, m_pinned);
}

void StockComparator::toString(std::string& str, unsigned int contentModifier) const
{
	RowWindowSortableAccount::toString(str, contentModifier);

	str += sectionDel;

	StockComparatorRow* row;

	for(const RowMap::CPair* p = m_rowMap.PGetFirstAssoc(); p; p = m_rowMap.PGetNextAssoc(p))
	{
		row = (StockComparatorRow*)p->value;
		str += (const char*)&row->GetFirstSymbol();
		str += settingSubdel;
		str += (const char*)&row->GetSecondSymbol();
		str += settingSubdel;
		U_SignedPriceToStr(str, row->GetValue(), 4, -1, false, settingDel);
	}
	str += sectionDel;
}

void StockComparator::fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier)
{
	RowWindowSortableAccount::fromString(cursor, version, clone, contentModifier);
	bool endChange;
	char* end = U_MarkSectionEnd((char*)cursor, sectionDel, endChange);

	std::string stockPairToken;
//	std::string token1;
//	std::string token2;
	SignedPrice value;
	const char* cur;

	if(endChange)*end = sectionDel;
	cursor = end + 1;

	if(endChange)
	{
		PairKey pairKey;
		end = U_MarkSectionEnd((char*)cursor, sectionDel, endChange);
		StockPairValueMap stockPairValueMap;
		while(*cursor)
		{
			stockPairToken.clear();
			U_ReadStrToken(stockPairToken, cursor, settingDel);
			cur = stockPairToken.c_str();
			pairKey.SetFirst(U_ReadStrAsUInt64Token(cur, settingSubdel));
			pairKey.SetSecond(U_ReadStrAsUInt64Token(cur, settingSubdel));
			U_StrToSignedPrice(value, cur, 4, settingSubdel);
			if(pairKey.isValid())
			{
				stockPairValueMap.SetAt(pairKey, value);
			}
		}
		AddRows(stockPairValueMap, true);

		if(endChange)*end = sectionDel;
		cursor = end + 1;
	}
}

void StockComparator::fromScratch()
{
	RowWindowSortableAccount::fromScratch();
}

void StockComparator::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_NEW_EQUITY:
		case TM_REFRESHED_EQUITY:
		{
			const Security* security = ((const TMsgSecurity*)message)->m_security;
			if(security->isLoaded())
			{
				unsigned __int64 numericSymbol = security->GetNumericSymbol();
				StockPairCollection* stockPairCollection = FindStockPairCollection(numericSymbol);
				if(stockPairCollection)
				{
					unsigned char whichSecurity;
					StockComparatorRow* row;
					StockPair* stockPair;
					StockPairCollection::StockPairSet& stockPairSet = stockPairCollection->GetStockPairSet();
					for(StockPairCollection::StockPairSet::iterator it = stockPairSet.begin(), itend = stockPairSet.end(); it != itend; ++it)
					{
						stockPair = *it;
						whichSecurity = stockPair->SetSecurity(security);
						if(whichSecurity)//should be always 1 or 2
						{
							row = (StockComparatorRow*)FindObject(stockPair);
							ApplyNewSecurity(row, security, whichSecurity);
						}
					}
				}
			}
		}
		break;

		case TM_NEW_POSITION:
		{
			const Position* position = ((TMsgNewPosition*)message)->m_position;
			if(m_account == position->GetAccount())
			{
				unsigned __int64 numericSymbol = position->GetNumericSymbol();
				StockPairCollection* stockPairCollection = FindStockPairCollection(numericSymbol);
				if(stockPairCollection)
				{
					unsigned char whichSecurity;
					StockComparatorRow* row;
					StockPair* stockPair;
					StockPairCollection::StockPairSet& stockPairSet = stockPairCollection->GetStockPairSet();
					for(StockPairCollection::StockPairSet::iterator it = stockPairSet.begin(), itend = stockPairSet.end(); it != itend; ++it)
					{
						stockPair = *it;
						whichSecurity = stockPair->WhichSecurity(numericSymbol);
						if(whichSecurity)//should be always 1 or 2
						{
							row = (StockComparatorRow*)FindObject(stockPair);
							ApplyNewPosition(row, position, whichSecurity);
						}
					}
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
		ClearAll();
		break;
	}
}

Row* StockComparator::CreateRow(const void* object, bool potential)
{
	StockPair* stockPair = (StockPair*)object;
	Position* position1 = nullptr;
	Position* position2 = nullptr;
	if(m_account)
	{
		position1 = m_account->FindStockPositionWithLock(stockPair->GetFirst());
		position2 = m_account->FindStockPositionWithLock(stockPair->GetSecond());
	}
	return new StockComparatorRow(this, stockPair, position1, position2, potential);
}

void StockComparator::SetAccount(const Account* account)
{
	if(m_account != account)
	{
		m_account = account;
		UpdateHistoryLoad();
		AccountChanged();
	}
}

void StockComparator::RepaintPositionValues()
{
	InvalidateDataAtColumn(SC_POS_SIZE_1);
	InvalidateDataAtColumn(SC_POS_SIZE_2);

	if(m_rowTotal)
	{
		CRect rowRect(m_totalRowRect);
		InvalidateCellIfVisible(rowRect, SC_POS_SIZE_1);
		InvalidateCellIfVisible(rowRect, SC_POS_SIZE_2);
	}
}

void StockComparator::UpdatePositionValues(RowSortable* row)
{
	row->UpdateRowValue(SC_POS_SIZE_1);
	row->UpdateRowValue(SC_POS_SIZE_2);

	if(m_rowTotal)
	{
		m_rowTotal->AdjustRowValue(row, SC_POS_SIZE_1);
		m_rowTotal->AdjustRowValue(row, SC_POS_SIZE_2);
	}
}

void StockComparator::AddRows(const StockPairValueMap& stockPairValueMap, bool replace)
{
	if(replace)
	{
		StockComparatorRow* row;
		Row* r;
		StockPair* stockPair;
		StockPair* sp;
		for(RowMap::CPair* p = m_rowMap.PGetFirstAssoc(); p; )
		{
			row = (StockComparatorRow*)p->value;

			p = m_rowMap.PGetNextAssoc(p);
			stockPair = row->GetStockPair();
			if(!stockPairValueMap.PLookup(*stockPair))
			{
				sp = RemoveStockPair(stockPair);
				r = RemoveObject(stockPair);
				if(r)delete r;
				if(sp)delete sp;
			}
		}
	}
	for(const StockPairValueMap::CPair* p = stockPairValueMap.PGetFirstAssoc(); p; p = stockPairValueMap.PGetNextAssoc(p))
	{
		AddStockPair(p->key, p->value);
	}
}

void StockComparator::UpdateAccountInfo()
{
	bool sortingChanged = false;
	unsigned short sortId = GetSortId();
	switch(sortId)
	{
		case SC_POS_SIZE_1:
		case SC_POS_SIZE_2:
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
					case SC_POS_SIZE_1:
					case SC_POS_SIZE_2:
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
	StockComparatorRow* row;
	Position* position1;
	Position* position2;
	unsigned char belongs;
	const StockPair* stockPair;
	size_t ord;
	bool rowChanged;

	if(m_account)
	{
		m_account->LockInquiryWait();
	}
	for(RowMap::CPair* p = m_rowMap.PGetFirstAssoc(); p; )
	{
		row = (StockComparatorRow*)p->value;

		p = m_rowMap.PGetNextAssoc(p);
		stockPair = row->GetStockPair();
		if(m_account)
		{
//No need to FindStockPositionWithLock because the account is locked above
			position1 = m_account->FindStockPosition(stockPair->GetFirst());
			position2 = m_account->FindStockPosition(stockPair->GetSecond());
		}
		else
		{
			position1 = nullptr;
			position2 = nullptr;
		}
		rowChanged = row->SetPosition1(position1);
		if(row->SetPosition2(position2))
		{
			rowChanged = true;
		}
		if(rowChanged)
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
			}
		}
	}
	if(m_account)
	{
		m_account->UnlockInquiry();
	}

	if(changed)
	{
		RepaintOnPopulation(false);
	}
	else
	{
		RepaintPositionValues();
	}
	::InvalidateRect(m_hWnd, &m_countRect, FALSE);
	::InvalidateRect(m_hWnd, &m_potentialCountRect, FALSE);
}

void StockComparator::CurrentAccountConstraintsLoaded()
{
	UpdateAccountInfo();
}

void StockComparator::CurrentAccountLoaded()
{
	RowWindowSortableAccount::CurrentAccountLoaded();
	UpdateAccountInfo();
}

void StockComparator::SetMarketSorterConnected(bool connected)
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
			::InvalidateRect(m_hWnd, &m_clientRect, FALSE);
		}
	}
}

void StockComparator::SetMarketSorterLoaded(bool loaded)
{
	if(m_marketSorterLoaded != loaded)
	{
		m_marketSorterLoaded = loaded;
		if(m_hWnd)
		{
			::InvalidateRect(m_hWnd, &m_clientRect, FALSE);
		}
	}
}

void StockComparator::NextDayStarted()
{
	RowWindowSortableAccount::NextDayStarted();
}

void StockComparator::LineSelected()
{
	RowWindowSortableAccount::LineSelected();
}

void StockComparator::BeforeDestroy()
{
	RowWindowSortableAccount::BeforeDestroy();
	mainDialog->TakionWindowRepaintTool(this);
}

void StockComparator::AllAccountsDeleted()
{
	SetAccount(nullptr);
}

void StockComparator::AccountChanged()
{
	RowWindowSortableAccount::AccountChanged();
	UpdateAccountInfo();
}

void StockComparator::ClearAll()
{
	DestroyRows();
	DestroyStockPairMap();
}

void StockComparator::RemoveSelected()
{
	StockPair* stockPair;
	StockPair* sp;
	Row* row;
	if(m_multiselect)
	{
		if(!m_selectedSet.empty())
		{
			for(RowSortableSet::const_iterator it = m_selectedSet.begin(); it != m_selectedSetEnd; )
			{
				stockPair = ((StockComparatorRow*)*it)->GetStockPair();
				++it;
				sp = RemoveStockPair(stockPair);
				row = RemoveObject(stockPair);
				if(row)delete row;
				if(sp)delete sp;
			}
		}
	}
	else
	{
		row = GetSelectedRow();
		if(row)
		{
			stockPair = ((StockComparatorRow*)row)->GetStockPair();
			sp = RemoveStockPair(stockPair);
			row = RemoveObject(stockPair);
			if(row)delete row;
			if(sp)delete sp;
		}
	}
}

void StockComparator::DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->itemID)
	{
		case TMID_ADD:
		case TMID_REMOVE:
		case TMID_CLEAR:
		case TMID_VALUE:
		case TMID_OPEN:
		case TMID_SAVE_AS:
		case TMID_PASTE:
		case TMID_COPY:
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
		break;

		default:
		RowWindowSortableAccount::DrawMenuItem(lpDrawItemStruct);
		break;
	}
}

bool StockComparator::MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	switch(lpMeasureItemStruct->itemID)
	{
		case TMID_ADD:
		case TMID_REMOVE:
		case TMID_CLEAR:
		case TMID_VALUE:
		case TMID_OPEN:
		case TMID_SAVE_AS:
		case TMID_PASTE:
		case TMID_COPY:

		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
		lpMeasureItemStruct->itemWidth = m_menuWidth;// m_mainWnd->GetAddMenuWidth();// + lpMeasureItemStruct->itemHeight;
		break;

		default:
		return RowWindowSortableAccount::MeasureMenuItem(lpMeasureItemStruct);
	}
	return true;
}

void StockComparator::AddMenuItems(CMenu& menu)
{
	menu.AppendMenu(MF_OWNERDRAW, TMID_ADD, "Add");
	menu.AppendMenu(MF_OWNERDRAW, TMID_REMOVE, "Remove");
	menu.AppendMenu(MF_OWNERDRAW, TMID_CLEAR, "Clear");
	menu.AppendMenu(MF_OWNERDRAW, TMID_VALUE, "Set Value");
//	menu.AppendMenu(MF_SEPARATOR);
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW, TMID_OPEN, "Open...");
//	menu.AppendMenu(MF_OWNERDRAW, TMID_SAVE, "Save");
	menu.AppendMenu(MF_OWNERDRAW, TMID_SAVE_AS, "Save as...");
	menu.AppendMenu(MF_OWNERDRAW, TMID_PASTE, "Paste");
	menu.AppendMenu(MF_OWNERDRAW, TMID_COPY, "Copy");

	RowWindowSortableAccount::AddMenuItems(menu);
}

void StockComparator::PostAddMenuItems(CMenu& menu)
{
	RowWindowSortableAccount::PostAddMenuItems(menu);
}
//	void AddRowPropertiesMenuItems(CMenu& popupMenu);
//	void PostAddRowPropertiesMenuItems(CMenu& popupMenu);

void StockComparator::PasteStocksFromString(const char* cursor, size_t size, bool replace)
{
	std::string symbol1;
	std::string symbol2;
	PairKey pairKey;

	char c;
	bool valid = true;

	unsigned int dollars = 0;
	unsigned int fraction = 0;
	bool fractionReached = false;
	bool negativeValue = false;

	StockPairValueMap stockPairValueMap;
///
	unsigned int multiplier = Price::divider / 10;

	unsigned int tokenCount = 0;

	unsigned int charCount = 0;
	const unsigned int maxCharCount = 7;
	
	for(size_t i = 0; i < size && *cursor; ++i, ++cursor)
	{
		c = *cursor;
		switch(c)
		{
			case '\r':
			break;

			case '\n':
			if(tokenCount < 2)
			{
				valid = false;
			}
			if(valid)
			{
				if(!symbol1.empty() && !symbol2.empty())
				{
					pairKey.SetFirst(U_RepresentStringAsUnsignedNumber<unsigned __int64>(symbol1.c_str(), sizeof(unsigned __int64) - 1));//U_StringToUInt64(symbol1.c_str()));
					pairKey.SetSecond(U_RepresentStringAsUnsignedNumber<unsigned __int64>(symbol2.c_str(), sizeof(unsigned __int64) - 1));//U_StringToUInt64(symbol2.c_str()));
					if(!stockPairValueMap.PLookup(pairKey))
					{
						stockPairValueMap.SetAt(pairKey, negativeValue ? -SignedPrice(dollars, fraction) : SignedPrice(dollars, fraction));
					}
				}
			}
			else
			{
				valid = true;
			}
			charCount = 0;
			symbol1.clear();
			symbol2.clear();
			dollars = 0;
			fraction = 0;
			multiplier = Price::divider / 10;
			fractionReached = false;
			negativeValue = false;
			tokenCount = 0;
			break;

			case '|':
			case '\t':
			++tokenCount;
			charCount = 0;
			break;

			default:
			if(valid)
			{
				switch(tokenCount)
				{
					case 0://symbol1
					c = toupper(c);
					if(U_IsStockCharValid(c, symbol1.empty()))
					{
						if(charCount >= maxCharCount)
						{
							valid = false;
						}
						else
						{
							symbol1 += c;
						}
					}
					else
					{
						valid = false;
					}
					break;

					case 1://symbol2
					c = toupper(c);
					if(U_IsStockCharValid(c, symbol2.empty()))
					{
						if(charCount >= maxCharCount)
						{
							valid = false;
						}
						else
						{
							symbol2 += c;
						}
					}
					else
					{
						valid = false;
					}
					break;

					case 2:
					switch(c)
					{
						case '-':
						if(charCount)
						{
							valid = false;
						}
						else
						{
							negativeValue = true;
						}
						break;

						case '.':
						if(fractionReached)
						{
							valid = false;
						}
						else
						{
							fractionReached = true;
						}
						break;

						default:
						if(isdigit(c))
						{
							if(fractionReached)
							{
								fraction += (c - '0') * multiplier;
								multiplier /= 10;
							}
							else
							{
								dollars *= 10;
								dollars += c - '0';
							}
						}
						else
						{
							valid = false;
						}
					}
					break;

					default:
					break;
				}
				++charCount;
			}
			break;
		}
	}
	if(valid && tokenCount > 1 && !symbol1.empty() && !symbol2.empty())
	{
		pairKey.SetFirst(U_RepresentStringAsUnsignedNumber<unsigned __int64>(symbol1.c_str(), sizeof(unsigned __int64) - 1));//U_StringToUInt64(symbol1.c_str()));
		pairKey.SetSecond(U_RepresentStringAsUnsignedNumber<unsigned __int64>(symbol2.c_str(), sizeof(unsigned __int64) - 1));//U_StringToUInt64(symbol2.c_str()));
		if(!stockPairValueMap.PLookup(pairKey))
		{
			stockPairValueMap.SetAt(pairKey, negativeValue ? -SignedPrice(dollars, fraction) : SignedPrice(dollars, fraction));
		}
	}
	AddRows(stockPairValueMap, replace);
	if(m_hWnd)
	{
		::InvalidateRect(m_hWnd, &m_titleRect, FALSE);
		RepaintOnPopulation(false);
	}
}

const char* StockComparatorRow::GetRowSecuritySymbol(const CPoint* point) const
{
	static std::string staticSymbol;
	RowWindow::ColumnIterator found = m_windowSortable->FindColumn(point->x + m_windowSortable->GetColumnScrollOffset());
	if(found != m_windowSortable->GetColumnEnd())
	{
		switch(found->second->GetId())
		{
			case StockComparator::SC_SYMBOL_1:
			staticSymbol = (const char*)&m_stockPair->GetFirst();
			return staticSymbol.c_str();

			case StockComparator::SC_SYMBOL_2:
			staticSymbol = (const char*)&m_stockPair->GetSecond();
			return staticSymbol.c_str();
		}
	}
	return nullptr;
}

BOOL StockComparator::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
    {
		case TMID_ADD:
		stockPairDialog->Display(this,
			0,
			"",
			"",
			SignedPrice::signedPriceZero,//TU_GetSignedPriceZero(),
			nullptr,
			m_rButtonDownPoint);
		break;
	
		case TMID_REMOVE:
		RemoveSelected();
		break;

		case TMID_CLEAR:
		ClearAll();
		break;

		case TMID_VALUE:
		{
			SignedPrice value;
			if(m_multiselect)
			{
				StockComparatorRow* row = (StockComparatorRow*)GetSelectedRow();
				if(row && row->isSelected())
				{
					value = row->GetValue();
				}
				else if(!m_selectedSet.empty())
				{
					value = ((StockComparatorRow*)*m_selectedSet.begin())->GetValue();
				}
			}
			else
			{
				StockComparatorRow* row = (StockComparatorRow*)GetSelectedRow();
				if(row)
				{
					value = row->GetValue();
				}
			}

			signedPriceValueDialog->Display(this,
				1,
				value,
				nullptr,
				m_rButtonDownPoint);
		}
		break;

		case TMID_OPEN:
		{
			bool replace = (GetKeyState(VK_CONTROL) & highBit) != 0;//CTRL key is pressed
			char buf[MAX_PATH];
			strcpy_s(buf, sizeof(buf), m_openFileName.c_str());

			TakionFileDialog fileDlg(m_mainWnd, true, stockFileExtension, buf,
				OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST,
				m_fileFilterString.c_str(),
				"Open StockComparator Stocks", this);

			if(fileDlg.DoModal() == IDOK)
			{
				std::string pathName = fileDlg.GetPathName();
				if(!pathName.empty())
				{
					m_openFileName = pathName;
					std::string info;
					U_ReadFileToString(pathName.c_str(), info);
					PasteStocksFromString(info.c_str(), info.length(), replace);
				}
			}
		}
		break;

		case TMID_SAVE_AS:
		{
			bool selectedOnly = (GetKeyState(VK_CONTROL) & highBit) != 0;//CTRL key is pressed
			char buf[MAX_PATH];
			strcpy_s(buf, sizeof(buf), m_saveFileName.c_str());
			TakionFileDialog fileDlg(m_mainWnd, false, stockFileExtension, buf,
				OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_EXTENSIONDIFFERENT,
				m_fileFilterString.c_str(),
				"Save StockComparator Stocks", this);

			if(fileDlg.DoModal() == IDOK)
			{
				std::string pathName = fileDlg.GetPathName();
				if(!pathName.empty())
				{
					size_t length = m_saveColumnIdListSize;
					if(length)
					{
						length <<= 4;
						length *= (selectedOnly && isMultiSelect() ? GetSelectedRowCount() : GetRowCount());// + (firstRowColumnNames ? 1 : 0));
						std::string info;
						info.reserve(length);
						ColumnListCopyRowsToString(info, selectedOnly, false, m_saveColumnIdListBegin, m_saveColumnIdListEnd, '|');

						m_saveFileName = pathName;
						U_WriteStringToFile(m_saveFileName.c_str(), info);
					}
				}
			}
		}
		break;

		case TMID_PASTE:
		PasteStocksFromClipboard((GetKeyState(VK_CONTROL) & highBit) != 0);//replace if CTRL key is down
		break;

		case TMID_COPY:
		ColumnListCopyRowsToClipboard((GetKeyState(VK_CONTROL) & highBit) != 0, false, m_saveColumnIdList, '|');//copy selected rows if CTRL key is down
		break;

		default:
		return RowWindowSortableAccount::OnCommand(wParam, lParam);
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(StockComparator, RowWindowSortableAccount)
	ON_MESSAGE(WM_USER + 77, OnAddStockPair)
END_MESSAGE_MAP()

void StockComparator::ClearComparatorRowsTemporarily(RowSortableHashSet& rowHashSet)
{
	GetVisibleRows(rowHashSet);
	ClearRowsTemporarily();
}

bool StockComparator::isSortByPosition(bool firstSecurity) const
{
	switch(GetSortId())
	{
		case SC_POS_SIZE_1:
		if(firstSecurity)
		{
			return true;
		}
		break;

		case SC_POS_SIZE_2:
		if(!firstSecurity)
		{
			return true;
		}
		break;
	}
	if(!m_sortChain.isEmpty())
	{
		const UShortChain::List& sortList = m_sortChain.GetList();
		for(UShortChain::ListConstIterator it = sortList.begin(), itend = sortList.end(); it != itend; ++it)
		{
			switch(*it)
			{
				case SC_POS_SIZE_1:
				if(firstSecurity)
				{
					return true;
				}
				break;

				case SC_POS_SIZE_2:
				if(!firstSecurity)
				{
					return true;
				}
				break;
			}
		}
	}

	return false;
}

bool StockComparator::isSortBySecurity(bool firstSecurity) const
{
	switch(GetSortId())
	{
		case SC_PRICE_DIFF:
		return true;

		case SC_PRICE_1:
//		case SC_POS_SIZE_1:
		if(firstSecurity)
		{
			return true;
		}
		break;

		case SC_PRICE_2:
//		case SC_POS_SIZE_2:
		if(!firstSecurity)
		{
			return true;
		}
		break;
	}
	if(!m_sortChain.isEmpty())
	{
		const UShortChain::List& sortList = m_sortChain.GetList();
		for(UShortChain::ListConstIterator it = sortList.begin(), itend = sortList.end(); it != itend; ++it)
		{
			switch(*it)
			{
				case SC_PRICE_DIFF:
				return true;

				case SC_PRICE_1:
//				case SC_POS_SIZE_1:
				if(firstSecurity)
				{
					return true;
				}
				break;

				case SC_PRICE_2:
//				case SC_POS_SIZE_2:
				if(!firstSecurity)
				{
					return true;
				}
				break;
			}
		}
	}

	return false;
}

bool StockComparator::isSortByValue() const
{
	if(GetSortId() == SC_VALUE)
	{
		return true;
	}
	if(!m_sortChain.isEmpty())
	{
		const UShortChain::List& sortList = m_sortChain.GetList();
		for(UShortChain::ListConstIterator it = sortList.begin(), itend = sortList.end(); it != itend; ++it)
		{
			if(*it == SC_VALUE)
			{
				return true;
			}
		}
	}
	return false;
}

void StockComparator::ApplyValue(RowSet& rowSet, const SignedPrice& value)
{
	bool sortingChanged = isSortByValue();
	RowSortableHashSet rowHashSet;
	if(sortingChanged)
	{
		ClearComparatorRowsTemporarily(rowHashSet);
	}
	for(RowSet::iterator it = rowSet.begin(), itend = rowSet.end(); it != itend; ++it)
	{
		((StockComparatorRow*)*it)->SetValue(value);
	}
	if(sortingChanged)
	{
		for(RowSortableHashSet::iterator it = rowHashSet.begin(), itend = rowHashSet.end(); it != itend; ++it)
		{
			ProcessRowBelonging(*it, 2, sortingChanged);
		}
		RepaintOnPopulation(false);
		::InvalidateRect(m_hWnd, &m_countRect, FALSE);
		::InvalidateRect(m_hWnd, &m_potentialCountRect, FALSE);
	}
	else
	{
		InvalidateDataAtColumn(SC_VALUE);
	}
}

void StockComparator::ApplyValue(StockComparatorRow* row, const SignedPrice& value)
{
	bool sortingChanged = isSortByValue();
	if(sortingChanged)
	{
		EraseRow(row);
	}
	row->SetValue(value);
	if(sortingChanged)
	{
		ProcessRowBelonging(row, 2, sortingChanged);
		RepaintOnPopulation(false);
		::InvalidateRect(m_hWnd, &m_countRect, FALSE);
		::InvalidateRect(m_hWnd, &m_potentialCountRect, FALSE);
	}
	else
	{
		InvalidateRowCellIfVisible(row, SC_VALUE);
	}
}

void StockComparator::ApplyNewPosition(StockComparatorRow* row, const Position* position, unsigned char whichSecurity)
{
	bool first = whichSecurity == 1;
	bool sorting = isSortByPosition(first);
	if(sorting)
	{
		EraseRow(row);
	}
	if(first)
	{
		row->SetPosition1(position);
	}
	else
	{
		row->SetPosition2(position);
	}
	if(sorting)
	{
		ProcessRowBelonging(row, 2, sorting);
		RepaintOnPopulation(false);
		::InvalidateRect(m_hWnd, &m_countRect, FALSE);
		::InvalidateRect(m_hWnd, &m_potentialCountRect, FALSE);
	}
	else
	{
		if(first)
		{
			InvalidateRowCellIfVisible(row, SC_POS_SIZE_1);
		}
		else
		{
			InvalidateRowCellIfVisible(row, SC_POS_SIZE_2);
		}
	}
}

//void StockComparator::ApplyNewSecurityAndPosition(StockComparatorRow* row, Security* security, Position* position, unsigned char whichSecurity)
void StockComparator::ApplyNewSecurity(StockComparatorRow* row, const Security* security, unsigned char whichSecurity)
{
	bool first = whichSecurity == 1;
	bool sorting = isSortBySecurity(first);
	if(sorting)
	{
		EraseRow(row);
	}
	if(first)
	{
		row->SetSecurity1(security);
//		row->SetPosition1(position);
	}
	else
	{
		row->SetSecurity2(security);
//		row->SetPosition2(position);
	}
	if(sorting)
	{
		ProcessRowBelonging(row, 2, sorting);
		RepaintOnPopulation(false);
		::InvalidateRect(m_hWnd, &m_countRect, FALSE);
		::InvalidateRect(m_hWnd, &m_potentialCountRect, FALSE);
	}
	else
	{
		InvalidateRowCellIfVisible(row, SC_PRICE_DIFF);
		if(first)
		{
			InvalidateRowCellIfVisible(row, SC_PRICE_1);
			InvalidateRowCellIfVisible(row, SC_SYMBOL_1);
//			InvalidateRowCellIfVisible(row, SC_POS_SIZE_1);
		}
		else
		{
			InvalidateRowCellIfVisible(row, SC_PRICE_2);
			InvalidateRowCellIfVisible(row, SC_SYMBOL_2);
//			InvalidateRowCellIfVisible(row, SC_POS_SIZE_2);
		}
	}
}

StockPair* StockComparator::RemoveStockPair(StockPair* stockPair)
{
	StockPairMap::CPair* found = m_stockPairMap.PLookup(*stockPair);
	if(found)
	{
		StockPair* ret = found->value;
		m_stockPairMap.RemoveKey(*stockPair);

		StockPairCollection* foundCollection;
		unsigned __int64 symbol = ret->GetFirst();
		SymbolStockPairMap::CPair* foundCollectionPair = m_symbolStockPairMap.PLookup(symbol);
		if(foundCollectionPair)
		{
			foundCollection = foundCollectionPair->value;
			if(foundCollection->RemoveStockPair(ret))
			{
				if(foundCollection->isEmpty())
				{
					m_symbolStockPairMap.RemoveKey(symbol);
					delete foundCollection;
				}
			}
		}

		symbol = ret->GetSecond();
		foundCollectionPair = m_symbolStockPairMap.PLookup(symbol);
		if(foundCollectionPair)
		{
			foundCollection = foundCollectionPair->value;
			if(foundCollection->RemoveStockPair(ret))
			{
				if(foundCollection->isEmpty())
				{
					m_symbolStockPairMap.RemoveKey(symbol);
					delete foundCollection;
				}
			}
		}
		return ret;
	}
	return nullptr;
}

void StockComparator::SetValueToSelectedRows(const SignedPrice& value)
{
	StockComparatorRow* row;
	if(m_multiselect)
	{
		if(!m_selectedSet.empty())
		{
			for(RowSortableSet::const_iterator it = m_selectedSet.begin(); it != m_selectedSetEnd; )
			{
				row = (StockComparatorRow*)*it;
				++it;
				ApplyValue(row, value);
			}
		}
	}
	else
	{
		row = (StockComparatorRow*)GetSelectedRow();
		if(row)
		{
			ApplyValue(row, value);
		}
	}
}

void StockComparator::AddStockPair(const PairKey& key, const SignedPrice& value)
{
	StockPairMap::CPair* found = m_stockPairMap.PLookup((PairKey&)key);
	StockPair* sp;
	if(found)
	{
		sp = found->value;
		RowSortable* row = FindObject(sp);
		if(row)
		{
			ApplyValue((StockComparatorRow*)row, value);
		}
		else//should not happen
		{
			NewRow(sp);
		}
	}
	else
	{
		sp = new StockPair(key, value);
		m_stockPairMap.SetAt((PairKey&)key, sp);

		StockPairCollection* spc;

		SymbolStockPairMap::CPair* foundCollection = m_symbolStockPairMap.PLookup(key.GetFirst());
		if(foundCollection)
		{
			spc = foundCollection->value;
		}
		else
		{
			spc = new StockPairCollection();
			m_symbolStockPairMap.SetAt(key.GetFirst(), spc);
		}
		spc->AddStockPair(sp);

		foundCollection = m_symbolStockPairMap.PLookup(key.GetSecond());
		if(foundCollection)
		{
			spc = foundCollection->value;
		}
		else
		{
			spc = new StockPairCollection();
			m_symbolStockPairMap.SetAt(key.GetSecond(), spc);
		}
		spc->AddStockPair(sp);

		NewRow(sp);
	}
}

LRESULT StockComparator::OnAddStockPair(WPARAM wparam, LPARAM lparam)
{
	switch(wparam)
	{
		case 0:
		AddStockPair(PairKey(U_RepresentStringAsUnsignedNumber<unsigned __int64>(stockPairDialog->GetSymbol1().c_str(), sizeof(unsigned __int64) - 1),//U_StringToUInt64(stockPairDialog->GetSymbol1().c_str()),
			U_RepresentStringAsUnsignedNumber<unsigned __int64>(stockPairDialog->GetSymbol2().c_str(), sizeof(unsigned __int64) - 1)),//U_StringToUInt64(stockPairDialog->GetSymbol2().c_str())),
			stockPairDialog->GetValue());
		break;

		case 1:
		SetValueToSelectedRows(signedPriceValueDialog->GetValue());
		break;
	}
	return 0;
}
