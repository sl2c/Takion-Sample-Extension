#include "stdafx.h"
#include "SampleDialogSettingMarketData.h"
#include "ExtensionDerivedConfig.h"

//extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool UnappliedMessageReader::ProcessDataToken(const char* cursor, unsigned short length) const
{
	return m_window->ProcessUnappliedMessage((const Message*)cursor);
}

void MyQuote::SetQuote(const Quote& quote, unsigned int roundLot)
{
	static char num[33];
	unsigned int quantity = quote.GetQuantity();
	if(m_quantity != quantity)
	{
		m_quantity = quantity;
		m_sizeStr = U_UnsignedNumberToStrNoDecDigits(m_quantity / (roundLot ? roundLot : 100), num, sizeof(num));
		::InvalidateRect(m_wnd->m_hWnd, &m_sizeRect, FALSE);
	}
	unsigned int mmid = quote.GetMmid();
	if(m_mmid != mmid)
	{
		m_mmid = mmid;
		*(unsigned int*)m_venueStr = m_mmid;
		::InvalidateRect(m_wnd->m_hWnd, &m_venueRect, FALSE);
	}

	bool priceChanged = false;
	unsigned int dollars = quote.GetDollars();
	if(m_dollars != dollars)
	{
		m_dollars = dollars;
		priceChanged = true;
	}
	unsigned int dollarFraction = quote.GetDollarFraction();
	if(m_dollarFraction != dollarFraction)
	{
		m_dollarFraction = dollarFraction;
		priceChanged = true;
	}
	if(priceChanged)
	{
		m_priceStr.clear();
		U_PriceToStr(m_priceStr, *this, m_dollars >= 1 ? 2 : 4);
		::InvalidateRect(m_wnd->m_hWnd, &m_priceRect, FALSE);
	}

	char qc = quote.GetCondition();
	if(qc == 'R')//regular
	{
		qc = '\0';
	}
	if(m_conditionChar != qc)
	{
		m_conditionChar = qc;
		::InvalidateRect(m_wnd->m_hWnd, &m_conditionRect, FALSE);
	}

	unsigned int timestamp = quote.GetMillisecond();
	if(m_millisecond != timestamp)
	{
		m_millisecond = timestamp;
		U_FormatMillisecond(m_millisecond, num, sizeof(num), '\0', true);//m_showQuoteMilliseconds);
		m_timeStr = num;
		::InvalidateRect(m_wnd->m_hWnd, &m_timeRect, FALSE);
	}
	if(quote.isHistorical())
	{
		m_delay = 0;
		if(m_delayStr.empty() || m_delayStr[0] != 'H')
		{
			m_delayStr = 'H';
			::InvalidateRect(m_wnd->m_hWnd, &m_delayRect, FALSE);
		}
	}
	else
	{
		short delay = quote.GetDelay();
		if(m_delay != delay)
		{
			m_delay = delay;
			m_delayStr = U_SignedNumberToStrNoDecDigits(m_delay, num, sizeof(num));
			::InvalidateRect(m_wnd->m_hWnd, &m_delayRect, FALSE);
		}
	}
}

void MyQuote::SetEmpty()
{
	RectCollection::SetEmpty();
	SetZero();
	m_quantity = 0;
	m_mmid = 0;
	m_millisecond = 0;
	m_delay = 0;
	if(m_conditionChar)
	{
		m_conditionChar = '\0';
		::InvalidateRect(m_wnd->m_hWnd, &m_conditionRect, FALSE);
	}
}

void MyPrint::SetPrint(const NamedPrint& print)
{
	static char num[33];
	unsigned int quantity = print.GetQuantity();
	if(m_quantity != quantity)
	{
		m_quantity = quantity;
		m_sizeStr = U_UnsignedNumberToStrNoDecDigits(m_quantity / 100, num, sizeof(num));
		::InvalidateRect(m_wnd->m_hWnd, &m_sizeRect, FALSE);
	}
	unsigned int mmid = print.GetMmid();
	if(m_mmid != mmid)
	{
		m_mmid = mmid;
		*(unsigned int*)m_venueStr = m_mmid;
		::InvalidateRect(m_wnd->m_hWnd, &m_venueRect, FALSE);
	}

	bool priceChanged = false;
	unsigned int dollars = print.GetDollars();
	if(m_dollars != dollars)
	{
		m_dollars = dollars;
		priceChanged = true;
	}
	unsigned int dollarFraction = print.GetDollarFraction();
	if(m_dollarFraction != dollarFraction)
	{
		m_dollarFraction = dollarFraction;
		priceChanged = true;
	}
	if(priceChanged)
	{
		m_priceStr.clear();
		U_PriceToStr(m_priceStr, *this, m_dollars >= 1 ? 2 : 4);
		::InvalidateRect(m_wnd->m_hWnd, &m_priceRect, FALSE);
	}

	char qc = print.GetSaleCondition1();
	if(qc == ' ')
	{
		qc = '\0';
	}
	if(m_saleCondition1 != qc)
	{
		m_saleCondition1 = qc;
		::InvalidateRect(m_wnd->m_hWnd, &m_conditionRect, FALSE);
	}

	qc = print.GetSaleCondition2();
	if(qc == ' ')
	{
		qc = '\0';
	}
	if(m_saleCondition2 != qc)
	{
		m_saleCondition2 = qc;
		::InvalidateRect(m_wnd->m_hWnd, &m_conditionRect2, FALSE);
	}

	qc = print.GetSaleCondition3();
	if(qc == ' ')
	{
		qc = '\0';
	}
	if(m_saleCondition3 != qc)
	{
		m_saleCondition3 = qc;
		::InvalidateRect(m_wnd->m_hWnd, &m_conditionRect3, FALSE);
	}

	qc = print.GetSaleCondition4();
	if(qc == ' ')
	{
		qc = '\0';
	}
	if(m_saleCondition4 != qc)
	{
		m_saleCondition4 = qc;
		::InvalidateRect(m_wnd->m_hWnd, &m_conditionRect4, FALSE);
	}

	unsigned int timestamp = print.GetMillisecond();
	if(m_millisecond != timestamp)
	{
		m_millisecond = timestamp;
		U_FormatMillisecond(m_millisecond, num, sizeof(num), '\0', true);//m_showQuoteMilliseconds);
		m_timeStr = num;
		::InvalidateRect(m_wnd->m_hWnd, &m_timeRect, FALSE);
	}
	if(print.isHistorical())
	{
		m_delay = 0;
		if(m_delayStr.empty() || m_delayStr[0] != 'H')
		{
			m_delayStr = 'H';
			::InvalidateRect(m_wnd->m_hWnd, &m_delayRect, FALSE);
		}
	}
	else
	{
		short delay = print.GetDelay();
		if(m_delay != delay)
		{
			m_delay = delay;
			m_delayStr = U_SignedNumberToStrNoDecDigits(m_delay, num, sizeof(num));
			::InvalidateRect(m_wnd->m_hWnd, &m_delayRect, FALSE);
		}
	}
}

void MyPrint::SetEmpty()
{
	RectCollection::SetEmpty();
	SetZero();
	m_quantity = 0;
	m_mmid = 0;
	m_millisecond = 0;
	m_delay = 0;
	m_saleCondition1 = '\0';
	m_saleCondition2 = '\0';
	m_saleCondition3 = '\0';
	m_saleCondition4 = '\0';
	m_bgColor = 0xFFFFFFFF;
	m_fgColor = 0;
}

void RectCollection::SetEmpty()
{
	if(!m_sizeStr.empty())
	{
		m_sizeStr.clear();
		::InvalidateRect(m_wnd->m_hWnd, &m_sizeRect, FALSE);
	}
	unsigned int& numVenue = *(unsigned int*)m_venueStr;
	if(numVenue)
	{
		numVenue = 0;
		::InvalidateRect(m_wnd->m_hWnd, &m_venueRect, FALSE);
	}
	if(!m_priceStr.empty())
	{
		m_priceStr.clear();
		::InvalidateRect(m_wnd->m_hWnd, &m_priceRect, FALSE);
	}
	if(!m_timeStr.empty())
	{
		m_timeStr.clear();
		::InvalidateRect(m_wnd->m_hWnd, &m_timeRect, FALSE);
	}
	if(!m_delayStr.empty())
	{
		m_delayStr.clear();
		::InvalidateRect(m_wnd->m_hWnd, &m_delayRect, FALSE);
	}
}

void MyQuote::Draw(HDC dc, const RECT& paintRect) const
{
	CRect intersectRect;
	if(intersectRect.IntersectRect(&paintRect, &m_sizeRect))
	{
		DrawText(dc, m_sizeStr.c_str(), -1, &(RECT&)m_sizeRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_venueRect))
	{
		DrawText(dc, m_venueStr, -1, &(RECT&)m_venueRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_priceRect))
	{
		DrawText(dc, m_priceStr.c_str(), -1, &(RECT&)m_priceRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_conditionRect) && m_conditionChar)
	{
		DrawText(dc, &m_conditionChar, 1, &(RECT&)m_conditionRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_timeRect))
	{
		DrawText(dc, m_timeStr.c_str(), -1, &(RECT&)m_timeRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_delayRect))
	{
		DrawText(dc, m_delayStr.c_str(), -1, &(RECT&)m_delayRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
}

void MyPrint::Draw(HDC dc, const RECT& paintRect) const
{
	CRect intersectRect;
	if(intersectRect.IntersectRect(&paintRect, &m_sizeRect))
	{
		DrawText(dc, m_sizeStr.c_str(), -1, &(RECT&)m_sizeRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_venueRect))
	{
		DrawText(dc, m_venueStr, -1, &(RECT&)m_venueRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_priceRect))
	{
		DrawText(dc, m_priceStr.c_str(), -1, &(RECT&)m_priceRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_conditionRect) && m_saleCondition1)
	{
		DrawText(dc, &m_saleCondition1, 1, &(RECT&)m_conditionRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_conditionRect2) && m_saleCondition2)
	{
		DrawText(dc, &m_saleCondition2, 1, &(RECT&)m_conditionRect2, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_conditionRect3) && m_saleCondition3)
	{
		DrawText(dc, &m_saleCondition3, 1, &(RECT&)m_conditionRect3, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_conditionRect4) && m_saleCondition4)
	{
		DrawText(dc, &m_saleCondition4, 1, &(RECT&)m_conditionRect4, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_timeRect))
	{
		DrawText(dc, m_timeStr.c_str(), -1, &(RECT&)m_timeRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
	if(intersectRect.IntersectRect(&paintRect, &m_delayRect))
	{
		DrawText(dc, m_delayStr.c_str(), -1, &(RECT&)m_delayRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	}
}

enum SampleSettingMarketDataIds
{
//	staticSymbolId = 10000,
//	editSymbolId,
	buttonSubscribeId = baseSymbolOptionIdsCount,//baseSymbolIdsCount,
	staticSymbolSubscribedId,

//	groupAccountId,
//	comboBoxAccountId,

	marketDataIdsCount
};

SampleDialogSettingMarketData::SampleDialogSettingMarketData(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseSampleDialogSettingOption(mainWnd, parentTab, true, "Level1, Level2", 66, 0),
//	m_EditSymbol(U_GetValidStockChars(), false),
//	m_ComboBoxAccount(0xFFFFFFFF, "Current"),

	m_quoteFont(m_visualBase->GetFont("Arial", -11, false, false, false, false, nullptr)),
	m_quoteCount(0),
	m_roundLot(100),
	m_sizesCalculated(false),
	m_quoteHeight(16),
	m_quoteSpace(m_quoteHeight + quoteVgap),
	m_sizeWidth(40),
	m_venueWidth(40),
	m_priceWidth(80),
	m_conditionWidth(m_quoteHeight),
	m_timeWidth(80),
	m_delayWidth(40),
	
	m_bidRect(0, 0, 0, 0),
	m_bidTitleRect(0, quoteVgap, 0, 0),
	m_bidSizeNameRect(0, quoteVgap, 0, 0),
	m_bidVenueNameRect(0, quoteVgap, 0, 0),
	m_bidPriceNameRect(0, quoteVgap, 0, 0),
	m_bidConditionNameRect(0, quoteVgap, 0, 0),
	m_bidTimeNameRect(0, quoteVgap, 0, 0),
	m_bidDelayNameRect(0, quoteVgap, 0, 0),

	m_askRect(0, 0, 0, 0),
	m_askTitleRect(0, quoteVgap, 0, 0),
	m_askSizeNameRect(0, quoteVgap, 0, 0),
	m_askVenueNameRect(0, quoteVgap, 0, 0),
	m_askPriceNameRect(0, quoteVgap, 0, 0),
	m_askConditionNameRect(0, quoteVgap, 0, 0),
	m_askTimeNameRect(0, quoteVgap, 0, 0),
	m_askDelayNameRect(0, quoteVgap, 0, 0),

	m_printRect(0, 0, 0, 0),
	m_printRectMinusOneLine(0, 0, 0, 0),
	m_printTitleRect(0, quoteVgap, 0, 0),
	m_printSizeNameRect(0, quoteVgap, 0, 0),
	m_printVenueNameRect(0, quoteVgap, 0, 0),
	m_printPriceNameRect(0, quoteVgap, 0, 0),
	m_printConditionNameRect1(0, quoteVgap, 0, 0),
	m_printConditionNameRect2(0, quoteVgap, 0, 0),
	m_printConditionNameRect3(0, quoteVgap, 0, 0),
	m_printConditionNameRect4(0, quoteVgap, 0, 0),
	m_printTimeNameRect(0, quoteVgap, 0, 0),
	m_printDelayNameRect(0, quoteVgap, 0, 0),

	m_l1TitleRect(0, 0, 0, 0),
	m_l1BidNameRect(0, 0, 0, 0),
	m_l1AskNameRect(0, 0, 0, 0),
	m_l1SpreadNameRect(0, 0, 0, 0),
	m_l2BidNameRect(0, 0, 0, 0),
	m_l2AskNameRect(0, 0, 0, 0),
	m_l2SpreadNameRect(0, 0, 0, 0),
#ifdef LRPS
	m_l1LrpBidNameRect(0, 0, 0, 0),
	m_l1LrpAskNameRect(0, 0, 0, 0),
#endif
	m_l1LuldBidNameRect(0, 0, 0, 0),
	m_l1LuldAskNameRect(0, 0, 0, 0),
	m_l1StatusNameRect(0, 0, 0, 0),
	m_positionSizeNameRect(0, 0, 0, 0),
	m_positionPriceNameRect(0, 0, 0, 0),
	m_positionClosedPnlNameRect(0, 0, 0, 0),
	m_positionOpenPnlNameRect(0, 0, 0, 0),

	m_l1ValueRect(0, 0, 0, 0),
	m_l1BidRect(0, 0, 0, 0),
	m_l1AskRect(0, 0, 0, 0),
	m_l1SpreadRect(0, 0, 0, 0),
	m_l2BidRect(0, 0, 0, 0),
	m_l2AskRect(0, 0, 0, 0),
	m_l2SpreadRect(0, 0, 0, 0),
#ifdef LRPS
	m_l1LrpBidRect(0, 0, 0, 0),
	m_l1LrpAskRect(0, 0, 0, 0),
#endif
	m_l1LuldBidRect(0, 0, 0, 0),
	m_l1LuldAskRect(0, 0, 0, 0),
	m_l1StatusRect(0, 0, 0, 0),
	m_positionSizeRect(0, 0, 0, 0),
	m_positionPriceRect(0, 0, 0, 0),
	m_positionClosedPnlRect(0, 0, 0, 0),
	m_positionOpenPnlRect(0, 0, 0, 0),

	m_bidVectorBegin(m_bidVector.begin()),
	m_bidVectorEnd(m_bidVector.end()),
	m_askVectorBegin(m_askVector.begin()),
	m_askVectorEnd(m_askVector.end()),

	m_printVectorBegin(m_printVector.begin()),
	m_printVectorEnd(m_printVector.end()),

	m_bidColor(RGB(0, 128, 0)),
	m_askColor(RGB(128, 0, 0)),
	m_printColor(RGB(96, 96, 0)),

	m_colorPrintBuyAggressiveText(RGB(255, 255, 255)),
	m_colorPrintBuyAggressive(RGB(0, 128, 0)),
	m_colorPrintBuy(RGB(0, 128, 0)),
	m_colorPrintNeutral(RGB(0, 0, 0)),
	m_colorPrintSell(RGB(128, 0, 0)),
	m_colorPrintSellAggressive(RGB(128, 0, 0)),
	m_colorPrintSellAggressiveText(RGB(255, 255, 255)),
/*
	m_accountGroupVerticalOffset(0),
	m_accountGroupHorizontalOffset(0),
	m_accountComboVerticalOffset(0),
	m_accountComboHorizontalOffset(0),
*/
	m_statusChar('\0'),
	m_positionSize(0),

	m_security(nullptr),
	m_position(nullptr),
//	m_account(nullptr),

	m_unappliedMessageReader(this)
{

	int x = 0;
//	int y = m_bottomControl;// + vgap;
	int width = m_accountWidth;//52;
	int xc = x + hgap;
//	AddStaticIndirect("Symbol", WS_VISIBLE | SS_NOPREFIX, 0, xc, y, width, staticHeight, staticSymbolId);
//	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xc, y += staticHeight, width, editHeight, editSymbolId);
	m_bottomControl += vgap;
//	AddButtonIndirect("Subscribe", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc, y += editHeight, width, comboBoxHeight, buttonSubscribeId);
	AddButtonIndirect("Subscribe", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc, m_bottomControl, width, comboBoxHeight, buttonSubscribeId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX, 0, xc, m_bottomControl += comboBoxHeight, width, staticHeight, staticSymbolSubscribedId);

	m_bottomControl += staticHeight;
/*
	y = m_height - tabHeight - 20;//25;
	AddButtonIndirect("Account", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap, 24, groupAccountId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y + groupOffset, width, 400, comboBoxAccountId);
*/
	bool sortBySize = true;
	bool hideSlowQuotes = false;
	unsigned int hideClosedQuotesMask = (1 << CQSF_Count) - 1;

	const unsigned int* lineCount = nullptr;//all
/*lineCountWithoutNyseAmexBook
	unsigned int lineCount[ECNBOOK_COUNT];
	unsigned int* lineCountCursor = lineCount;
	for(unsigned char ic = 0; ic < ECNBOOK_COUNT; ++ic, ++lineCountCursor)
	{
		*lineCountCursor = 0xFFFFFFFF;
	}
	lineCount[ECNBOOK_NYS] = 0;
	lineCount[ECNBOOK_AMEX] = 0;
*/
	unsigned int exchangeMask = 0xFFFFFFFF;//all;
//	unsigned int exchangeMask = ~((1 << NYS_NYSE)|(1 << NYS_AMEX));//No NYSE and AMEX

	unsigned int attributionMask = 0;
	bool oddSize = false;
	m_bidIterator = TU_CreateMmAggregatedIterator(true, sortBySize, hideSlowQuotes, hideClosedQuotesMask, lineCount, exchangeMask, attributionMask, nullptr, nullptr, oddSize);
	m_askIterator = TU_CreateMmAggregatedIterator(false, sortBySize, hideSlowQuotes, hideClosedQuotesMask, lineCount, exchangeMask, attributionMask, nullptr, nullptr, oddSize);
	m_printIterator = TU_CreateMultiPrintIterator();// AllMarketCenterFilter, 0, nullptr);

//	TU_MmAggregatedIteratorSetSortBySize(m_bidIterator, sortBySize);
//	TU_MmAggregatedIteratorSetSortBySize(m_askIterator, sortBySize);
//	TU_MmAggregatedIteratorSetHideSlowQuotes(m_bidIterator, hideSlowQuotes);
//	TU_MmAggregatedIteratorSetHideSlowQuotes(m_askIterator, hideSlowQuotes);
//	TU_MmAggregatedIteratorSetBookFilter(m_bidIterator, lineCount, exchangeMask);
//	TU_MmAggregatedIteratorSetBookFilter(m_askIterator, lineCount, exchangeMask);
//	TU_MmAggregatedIteratorSetAttributionMask(m_bidIterator, attributionMask);
//	TU_MmAggregatedIteratorSetAttributionMask(m_askIterator, attributionMask);
//	TU_MmAggregatedIteratorSetSecurity(m_bidIterator, m_security);
//	TU_MmAggregatedIteratorSetSecurity(m_askIterator, m_security);
//	TU_MultiPrintIteratorSetSecurity(m_printIterator, m_security);
}

SampleDialogSettingMarketData::~SampleDialogSettingMarketData()
{
	if(m_bidIterator)
	{
		delete m_bidIterator;
	}
	if(m_askIterator)
	{
		delete m_askIterator;
	}
	if(m_printIterator)
	{
		delete m_printIterator;
	}
}
/*
HWND SampleDialogSettingMarketData::GetFirstTabControl() const
{
	return m_EditSymbol.m_hWnd;
}
*/
/*
int SampleDialogSettingMarketData::CalculateWidth(const char* str)
{
	CRect rect(0, 0, 0, 0);
	m_dc.DrawText(str, -1, &rect, DT_SINGLELINE | DT_CALCRECT);
	return rect.right;
}
*/
void SampleDialogSettingMarketData::CalculateSizes()
{
	if(!m_sizesCalculated && m_dc.m_hDC)
	{
		m_sizesCalculated = true;
		CRect rect(0, 0, 0, 0);
		int width;
		m_quoteHeight = TakionBaseWnd::CalculateTextWidthAndHeight(rect, m_dc.m_hDC, "bgylGMN", width);
		m_quoteSpace = m_quoteHeight + quoteVgap;
		TakionBaseWnd::CalculateTextWidth(rect, m_dc.m_hDC, "999999", m_sizeWidth);
		TakionBaseWnd::CalculateTextWidth(rect, m_dc.m_hDC, "WWWW", m_venueWidth);
		TakionBaseWnd::CalculateTextWidth(rect, m_dc.m_hDC, "106,999.67", m_priceWidth);
		TakionBaseWnd::CalculateTextWidth(rect, m_dc.m_hDC, "W", m_conditionWidth);
		TakionBaseWnd::CalculateTextWidth(rect, m_dc.m_hDC, "10:28:30-234", m_timeWidth);
		TakionBaseWnd::CalculateTextWidth(rect, m_dc.m_hDC, "-32768", m_delayWidth);
/*
		m_quoteHeight = m_dc.DrawText("bgylGMN", -1, &rect, DT_SINGLELINE | DT_CALCRECT);
		m_quoteSpace = m_quoteHeight + quoteVgap;
		m_sizeWidth = CalculateWidth("999999");
		m_venueWidth = CalculateWidth("WWWW");
		m_priceWidth = CalculateWidth("106,999.67");
		m_conditionWidth = CalculateWidth("W");
		m_timeWidth = CalculateWidth("10:28:30-234");
		m_delayWidth = CalculateWidth("-32768");
*/
	}
}

void SampleDialogSettingMarketData::Resize(bool widthChanged, bool heightChanged)
{
//	CalculateSizes();
	if(m_EditSymbol.m_hWnd)
	{
		::InvalidateRect(m_hWnd, &m_printTitleRect, FALSE);
		::InvalidateRect(m_hWnd, &m_printRect, FALSE);

		m_EditSymbol.GetWindowRect(&m_bidRect);
		ScreenToClient(&m_bidRect);

		m_StaticSymbolSubscribed.GetWindowRect(&m_l1BidRect);
		ScreenToClient(&m_l1BidRect);

		m_l1TitleRect.top = m_l1ValueRect.top = m_l1BidRect.top = m_l1BidNameRect.top = m_l1BidRect.bottom + quoteVgap;
		m_l1BidRect.bottom = m_l1BidNameRect.bottom = m_l1BidRect.top + m_quoteHeight;

		m_l1AskRect.top = m_l1AskNameRect.top = m_l1BidRect.bottom + quoteVgap;
		m_l1AskRect.bottom = m_l1AskNameRect.bottom = m_l1AskRect.top + m_quoteHeight;

		m_l1SpreadRect.top = m_l1SpreadNameRect.top = m_l1AskRect.bottom + quoteVgap;
		m_l1SpreadRect.bottom = m_l1SpreadNameRect.bottom = m_l1SpreadRect.top + m_quoteHeight;

		m_l2BidRect.top = m_l2BidNameRect.top = m_l1SpreadRect.bottom + quoteVgap;
		m_l2BidRect.bottom = m_l2BidNameRect.bottom = m_l2BidRect.top + m_quoteHeight;

		m_l2AskRect.top = m_l2AskNameRect.top = m_l2BidRect.bottom + quoteVgap;
		m_l2AskRect.bottom = m_l2AskNameRect.bottom = m_l2AskRect.top + m_quoteHeight;

		m_l2SpreadRect.top = m_l2SpreadNameRect.top = m_l2AskRect.bottom + quoteVgap;
		m_l2SpreadRect.bottom = m_l2SpreadNameRect.bottom = m_l2SpreadRect.top + m_quoteHeight;

		m_l1LuldBidRect.top = m_l1LuldBidNameRect.top = m_l2SpreadRect.bottom + quoteVgap;
		m_l1LuldBidRect.bottom = m_l1LuldBidNameRect.bottom = m_l1LuldBidRect.top + m_quoteHeight;

		m_l1LuldAskRect.top = m_l1LuldAskNameRect.top = m_l1LuldBidRect.bottom + quoteVgap;
		m_l1LuldAskRect.bottom = m_l1LuldAskNameRect.bottom = m_l1LuldAskRect.top + m_quoteHeight;

#ifdef LRPS
		m_l1LrpBidRect.top = m_l1LrpBidNameRect.top = m_l1LuldAskRect.bottom + quoteVgap;
		m_l1LrpBidRect.bottom = m_l1LrpBidNameRect.bottom = m_l1LrpBidRect.top + m_quoteHeight;

		m_l1LrpAskRect.top = m_l1LrpAskNameRect.top = m_l1LrpBidRect.bottom + quoteVgap;
		m_l1LrpAskRect.bottom = m_l1LrpAskNameRect.bottom = m_l1LrpAskRect.top + m_quoteHeight;
#endif

		m_l1StatusRect.top = m_l1StatusNameRect.top =
#ifdef LRPS
			m_l1LrpAskRect.bottom
#else
			m_l1LuldAskRect.bottom
#endif
			+ quoteVgap;
		m_l1StatusRect.bottom = m_l1StatusNameRect.bottom = m_l1StatusRect.top + m_quoteHeight;

		m_positionSizeRect.top = m_positionSizeNameRect.top = m_l1StatusRect.bottom + quoteVgap;
		m_positionSizeRect.bottom = m_positionSizeNameRect.bottom = m_positionSizeRect.top + m_quoteHeight;

		m_positionPriceRect.top = m_positionPriceNameRect.top = m_positionSizeRect.bottom + quoteVgap;
		m_positionPriceRect.bottom = m_positionPriceNameRect.bottom = m_positionPriceRect.top + m_quoteHeight;

		m_positionClosedPnlRect.top = m_positionClosedPnlNameRect.top = m_positionPriceRect.bottom + quoteVgap;
		m_positionClosedPnlRect.bottom = m_positionClosedPnlNameRect.bottom = m_positionClosedPnlNameRect.top + m_quoteHeight;

		m_positionOpenPnlRect.top = m_positionOpenPnlNameRect.top = m_positionClosedPnlNameRect.bottom + quoteVgap;
		m_l1TitleRect.bottom = m_l1ValueRect.bottom = m_positionOpenPnlRect.bottom = m_positionOpenPnlNameRect.bottom = m_positionOpenPnlRect.top + m_quoteHeight;

		m_l1ValueRect.right
			= m_l1BidRect.right
			= m_l1AskRect.right
			= m_l1SpreadRect.right
			= m_l2BidRect.right
			= m_l2AskRect.right
			= m_l2SpreadRect.right
#ifdef LRPS
			= m_l1LrpBidRect.right
			= m_l1LrpAskRect.right
#endif
			= m_l1LuldBidRect.right
			= m_l1LuldAskRect.right
			= m_l1StatusRect.right
			= m_positionSizeRect.right
			= m_positionPriceRect.right
			= m_positionClosedPnlRect.right
			= m_positionOpenPnlRect.right
			= m_bidRect.right;

		m_l1ValueRect.left
			= m_l1BidRect.left
			= m_l1AskRect.left
			= m_l1SpreadRect.left
			= m_l2BidRect.left
			= m_l2AskRect.left
			= m_l2SpreadRect.left
#ifdef LRPS
			= m_l1LrpBidRect.left
			= m_l1LrpAskRect.left
#endif
			= m_l1LuldBidRect.left
			= m_l1LuldAskRect.left
			= m_l1StatusRect.left
			= m_positionSizeRect.left
			= m_positionPriceRect.left
			= m_positionClosedPnlRect.left
			= m_positionOpenPnlRect.left
			= m_bidRect.left
			= m_bidRect.right - m_priceWidth;

		m_l1TitleRect.right
			= m_l1BidNameRect.right
			= m_l1AskNameRect.right
			= m_l1SpreadNameRect.right
			= m_l2BidNameRect.right
			= m_l2AskNameRect.right
			= m_l2SpreadNameRect.right
#ifdef LRPS
			= m_l1LrpBidNameRect.right
			= m_l1LrpAskNameRect.right
#endif
			= m_l1LuldBidNameRect.right
			= m_l1LuldAskNameRect.right
			= m_l1StatusNameRect.right
			= m_positionSizeNameRect.right
			= m_positionPriceNameRect.right
			= m_positionClosedPnlNameRect.right
			= m_positionOpenPnlNameRect.right
			= m_l1BidRect.left - quoteHgap - quoteHgap;

		m_bidTitleRect.bottom
			= m_bidSizeNameRect.bottom
			= m_bidVenueNameRect.bottom
			= m_bidPriceNameRect.bottom
			= m_bidConditionNameRect.bottom
			= m_bidTimeNameRect.bottom
			= m_bidDelayNameRect.bottom

			= m_askTitleRect.bottom
			= m_askSizeNameRect.bottom
			= m_askVenueNameRect.bottom
			= m_askPriceNameRect.bottom
			= m_askConditionNameRect.bottom
			= m_askTimeNameRect.bottom
			= m_askDelayNameRect.bottom
			
			= m_printTitleRect.bottom
			= m_printSizeNameRect.bottom
			= m_printVenueNameRect.bottom
			= m_printPriceNameRect.bottom
			= m_printConditionNameRect1.bottom
			= m_printConditionNameRect2.bottom
			= m_printConditionNameRect3.bottom
			= m_printConditionNameRect4.bottom
			= m_printTimeNameRect.bottom
			= m_printDelayNameRect.bottom

			= m_bidTitleRect.top + m_quoteHeight;

		m_bidRect.top = m_askRect.top = m_printRect.top = m_printRectMinusOneLine.top = m_bidSizeNameRect.bottom + quoteVgap;

		m_bidRect.left = m_bidTitleRect.left = m_bidSizeNameRect.left = m_bidRect.right + quoteHgap + quoteHgap;

		m_bidSizeNameRect.right = m_bidSizeNameRect.left + m_sizeWidth;
		m_bidVenueNameRect.left = m_bidSizeNameRect.right + quoteHgap;
		m_bidVenueNameRect.right = m_bidVenueNameRect.left + m_venueWidth;
		m_bidPriceNameRect.left = m_bidVenueNameRect.right + quoteHgap;
		m_bidPriceNameRect.right = m_bidPriceNameRect.left + m_priceWidth;
		m_bidConditionNameRect.left = m_bidPriceNameRect.right + quoteHgap;
		m_bidConditionNameRect.right = m_bidConditionNameRect.left + m_conditionWidth;
		m_bidTimeNameRect.left = m_bidConditionNameRect.right + quoteHgap;
		m_bidTimeNameRect.right = m_bidTimeNameRect.left + m_timeWidth;
		m_bidDelayNameRect.left = m_bidTimeNameRect.right + quoteHgap;
		m_bidDelayNameRect.right = m_bidRect.right = m_bidTitleRect.right = m_bidDelayNameRect.left + m_delayWidth;

		m_askSizeNameRect.left = m_askRect.left = m_askTitleRect.left = m_bidDelayNameRect.right + 6 * quoteHgap;
		m_askSizeNameRect.right = m_askSizeNameRect.left + m_sizeWidth;
		m_askVenueNameRect.left = m_askSizeNameRect.right + quoteHgap;
		m_askVenueNameRect.right = m_askVenueNameRect.left + m_venueWidth;
		m_askPriceNameRect.left = m_askVenueNameRect.right + quoteHgap;
		m_askPriceNameRect.right = m_askPriceNameRect.left + m_priceWidth;
		m_askConditionNameRect.left = m_askPriceNameRect.right + quoteHgap;
		m_askConditionNameRect.right = m_askConditionNameRect.left + m_conditionWidth;
		m_askTimeNameRect.left = m_askConditionNameRect.right + quoteHgap;
		m_askTimeNameRect.right = m_askTimeNameRect.left + m_timeWidth;
		m_askDelayNameRect.left = m_askTimeNameRect.right + quoteHgap;
		m_askDelayNameRect.right = m_askRect.right = m_askTitleRect.right = m_askDelayNameRect.left + m_delayWidth;

		CRect windowRect;
		GetWindowRect(&windowRect);
		const int windowBottom = windowRect.Height();

		m_printDelayNameRect.right = m_printRect.right = m_printRectMinusOneLine.right = m_printTitleRect.right = m_clientRect.right;//m_frameRect.right;//windowRect.Width();//m_askDelayNameRect.left + m_delayWidth;
		m_printDelayNameRect.left = m_printDelayNameRect.right - m_delayWidth;//+ quoteHgap;
		m_printTimeNameRect.right = m_printDelayNameRect.left - quoteHgap;
		m_printTimeNameRect.left = m_printTimeNameRect.right - m_timeWidth;
		m_printConditionNameRect4.right = m_printTimeNameRect.left - quoteHgap;
		m_printConditionNameRect4.left = m_printConditionNameRect4.right - m_conditionWidth;
		m_printConditionNameRect3.right = m_printConditionNameRect4.left - quoteHgap;
		m_printConditionNameRect3.left = m_printConditionNameRect3.right - m_conditionWidth;
		m_printConditionNameRect2.right = m_printConditionNameRect3.left - quoteHgap;
		m_printConditionNameRect2.left = m_printConditionNameRect2.right - m_conditionWidth;
		m_printConditionNameRect1.right = m_printConditionNameRect2.left - quoteHgap;
		m_printConditionNameRect1.left = m_printConditionNameRect1.right - m_conditionWidth;
		m_printPriceNameRect.right = m_printConditionNameRect1.left - quoteHgap;
		m_printPriceNameRect.left = m_printPriceNameRect.right - m_priceWidth;
		m_printVenueNameRect.right = m_printPriceNameRect.left - quoteHgap;
		m_printVenueNameRect.left = m_printVenueNameRect.right - m_venueWidth;
		m_printSizeNameRect.right = m_printVenueNameRect.left - quoteHgap;
		m_printSizeNameRect.left = m_printRect.left = m_printRectMinusOneLine.left = m_printTitleRect.left = m_printSizeNameRect.right - m_sizeWidth;

		CRect rect;
		rect.top = m_bidRect.top;
		rect.bottom = rect.top + m_quoteHeight;

		const unsigned int quoteCount = windowBottom > rect.top ? (unsigned int)((windowBottom - rect.top) / m_quoteSpace) : 0;
//		unsigned int prevQuoteCount = (unsigned int)m_bidVector.size();
		bool verticalSizeIncreased = false;
		bool verticalSizeChanged = quoteCount != m_quoteCount;
		if(verticalSizeChanged)
		{
			verticalSizeIncreased = quoteCount > m_quoteCount;
			m_quoteCount = quoteCount;
			m_bidVector.resize(m_quoteCount);
			m_askVector.resize(m_quoteCount);
			m_printVector.resize(m_quoteCount);

			m_bidVectorBegin = m_bidVector.begin();
			m_bidVectorEnd = m_bidVector.end();
			m_askVectorBegin = m_askVector.begin();
			m_askVectorEnd = m_askVector.end();

			m_printVectorBegin = m_printVector.begin();
			m_printVectorEnd = m_printVector.end();
		}

		m_bidRect.bottom = m_askRect.bottom = m_printRect.bottom = m_bidRect.top + quoteCount * m_quoteSpace - quoteVgap;
		m_printRectMinusOneLine.bottom = m_printRect.bottom - m_quoteSpace;
		PrintVector::iterator itPrint = m_printVectorBegin;
		for(QuoteVector::iterator itBid = m_bidVectorBegin, itBidEnd = m_bidVectorEnd, itAsk = m_askVectorBegin; itBid != itBidEnd; ++itBid, ++itAsk, ++itPrint, rect.top += m_quoteSpace, rect.bottom += m_quoteSpace)
		{
			MyQuote& bidQuote = *itBid;
			bidQuote.SetWindow(this);
			rect.left = m_bidSizeNameRect.left;
			rect.right = m_bidSizeNameRect.right;
			bidQuote.SetSizeRect(rect);
			rect.left = m_bidVenueNameRect.left;
			rect.right = m_bidVenueNameRect.right;
			bidQuote.SetVenueRect(rect);
			rect.left = m_bidPriceNameRect.left;
			rect.right = m_bidPriceNameRect.right;
			bidQuote.SetPriceRect(rect);
			rect.left = m_bidConditionNameRect.left;
			rect.right = m_bidConditionNameRect.right;
			bidQuote.SetConditionRect(rect);
			rect.left = m_bidTimeNameRect.left;
			rect.right = m_bidTimeNameRect.right;
			bidQuote.SetTimeRect(rect);
			rect.left = m_bidDelayNameRect.left;
			rect.right = m_bidDelayNameRect.right;
			bidQuote.SetDelayRect(rect);

			MyQuote& askQuote = *itAsk;
			askQuote.SetWindow(this);
			rect.left = m_askSizeNameRect.left;
			rect.right = m_askSizeNameRect.right;
			askQuote.SetSizeRect(rect);
			rect.left = m_askVenueNameRect.left;
			rect.right = m_askVenueNameRect.right;
			askQuote.SetVenueRect(rect);
			rect.left = m_askPriceNameRect.left;
			rect.right = m_askPriceNameRect.right;
			askQuote.SetPriceRect(rect);
			rect.left = m_askConditionNameRect.left;
			rect.right = m_askConditionNameRect.right;
			askQuote.SetConditionRect(rect);
			rect.left = m_askTimeNameRect.left;
			rect.right = m_askTimeNameRect.right;
			askQuote.SetTimeRect(rect);
			rect.left = m_askDelayNameRect.left;
			rect.right = m_askDelayNameRect.right;
			askQuote.SetDelayRect(rect);

			MyPrint& print = *itPrint;
			print.SetWindow(this);
			rect.left = m_printSizeNameRect.left;
			rect.right = m_printSizeNameRect.right;
			print.SetSizeRect(rect);
			rect.left = m_printVenueNameRect.left;
			rect.right = m_printVenueNameRect.right;
			print.SetVenueRect(rect);
			rect.left = m_printPriceNameRect.left;
			rect.right = m_printPriceNameRect.right;
			print.SetPriceRect(rect);
			rect.left = m_printConditionNameRect1.left;
			rect.right = m_printConditionNameRect1.right;
			print.SetConditionRect(rect);
			rect.left = m_printConditionNameRect2.left;
			rect.right = m_printConditionNameRect2.right;
			print.SetConditionRect2(rect);
			rect.left = m_printConditionNameRect3.left;
			rect.right = m_printConditionNameRect3.right;
			print.SetConditionRect3(rect);
			rect.left = m_printConditionNameRect4.left;
			rect.right = m_printConditionNameRect4.right;
			print.SetConditionRect4(rect);
			rect.left = m_printTimeNameRect.left;
			rect.right = m_printTimeNameRect.right;
			print.SetTimeRect(rect);
			rect.left = m_printDelayNameRect.left;
			rect.right = m_printDelayNameRect.right;
			print.SetDelayRect(rect);
		}

		if(verticalSizeChanged)
		{
//			SecurityLevel1Updated();
			if(verticalSizeIncreased)
			{
				SecurityLevel2Updated();
				UpdatePrints();
			}
			::InvalidateRect(m_hWnd, &m_printTitleRect, FALSE);
			::InvalidateRect(m_hWnd, &m_printRect, FALSE);
		}
/*
		if(m_accountGroupVerticalOffset)
		{
			if(heightChanged)
			{
				m_GroupAccount.SetWindowPos(nullptr, m_accountGroupHorizontalOffset, m_clientRect.bottom - m_accountGroupVerticalOffset, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
				m_ComboBoxAccount.SetWindowPos(nullptr, m_accountComboHorizontalOffset, m_clientRect.bottom - m_accountComboVerticalOffset, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
				m_ComboBoxAccount.Invalidate(FALSE);
			}
		}
		else
		{
			CRect clientRect;
			GetClientRect(clientRect);
			int bottom = clientRect.bottom;

			CRect rect;
			m_GroupAccount.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_accountGroupHorizontalOffset = rect.left;
			m_accountGroupVerticalOffset = bottom - rect.top;

			m_ComboBoxAccount.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_accountComboHorizontalOffset = rect.left;
			m_accountComboVerticalOffset = bottom - rect.top;
		}
*/
	}
}

CFont* SampleDialogSettingMarketData::GetQuoteFont() const
{
//	return GetFont();
	return m_quoteFont;
}

void SampleDialogSettingMarketData::DoPaint(const RECT& rcPaint, CDC& dc)
{
	BaseSampleDialogSettingOption::DoPaint(rcPaint, dc);
	CRect intersectRect;

	COLORREF oldColor = dc.GetTextColor();

//	dc.SelectObject(GetFont());
	if(intersectRect.IntersectRect(&m_l1TitleRect, &rcPaint))
	{
		if(intersectRect.IntersectRect(&m_l1BidNameRect, &rcPaint))
		{
			DrawText(dc, "L1Bid", -1, &m_l1BidNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l1AskNameRect, &rcPaint))
		{
			DrawText(dc, "L1Ask", -1, &m_l1AskNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l1SpreadNameRect, &rcPaint))
		{
			DrawText(dc, "L1Spread", -1, &m_l1SpreadNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l2BidNameRect, &rcPaint))
		{
			DrawText(dc, "L2Bid", -1, &m_l2BidNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l2AskNameRect, &rcPaint))
		{
			DrawText(dc, "L2Ask", -1, &m_l2AskNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l2SpreadNameRect, &rcPaint))
		{
			DrawText(dc, "L2Spread", -1, &m_l2SpreadNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l1LuldBidNameRect, &rcPaint))
		{
			DrawText(dc, "LuldBid", -1, &m_l1LuldBidNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l1LuldAskNameRect, &rcPaint))
		{
			DrawText(dc, "LuldAsk", -1, &m_l1LuldAskNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
#ifdef LRPS
		if(intersectRect.IntersectRect(&m_l1LrpBidNameRect, &rcPaint))
		{
			DrawText(dc, "LrpBid", -1, &m_l1LrpBidNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l1LrpAskNameRect, &rcPaint))
		{
			DrawText(dc, "LrpAsk", -1, &m_l1LrpAskNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
#endif
		if(intersectRect.IntersectRect(&m_l1StatusNameRect, &rcPaint))
		{
			DrawText(dc, "Status", -1, &m_l1StatusNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_positionSizeNameRect, &rcPaint))
		{
			DrawText(dc, "pSize", -1, &m_positionSizeNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_positionPriceNameRect, &rcPaint))
		{
			DrawText(dc, "pPrice", -1, &m_positionPriceNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_positionClosedPnlNameRect, &rcPaint))
		{
			DrawText(dc, "pClosP&&L", -1, &m_positionClosedPnlNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_positionOpenPnlNameRect, &rcPaint))
		{
			DrawText(dc, "pOpP&&L", -1, &m_positionOpenPnlNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
	}
	if(intersectRect.IntersectRect(&m_l1ValueRect, &rcPaint))
	{
		if(intersectRect.IntersectRect(&m_l1BidRect, &rcPaint))
		{
			DrawText(dc, m_l1BidStr.c_str(), -1, &m_l1BidRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l1AskRect, &rcPaint))
		{
			DrawText(dc, m_l1AskStr.c_str(), -1, &m_l1AskRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(!m_l1SpreadStr.empty() && intersectRect.IntersectRect(&m_l1SpreadRect, &rcPaint))
		{
			UGUI_FillSolidRect(dc, m_l1SpreadRect, m_l1Spread.isPositive() ? GetBkColor() : m_l1Spread.isZero() ? RGB(192, 192, 0) : RGB(255, 0, 0));
			DrawText(dc, m_l1SpreadStr.c_str(), -1, &m_l1SpreadRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l2BidRect, &rcPaint))
		{
			DrawText(dc, m_l2BidStr.c_str(), -1, &m_l2BidRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l2AskRect, &rcPaint))
		{
			DrawText(dc, m_l2AskStr.c_str(), -1, &m_l2AskRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(!m_l2SpreadStr.empty() && intersectRect.IntersectRect(&m_l2SpreadRect, &rcPaint))
		{
			UGUI_FillSolidRect(dc, m_l2SpreadRect, m_l2Spread.isPositive() ? GetBkColor() : m_l2Spread.isZero() ? RGB(192, 192, 0) : RGB(255, 0, 0));
			DrawText(dc, m_l2SpreadStr.c_str(), -1, &m_l2SpreadRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l1LuldBidRect, &rcPaint))
		{
			DrawText(dc, m_luldBidStr.c_str(), -1, &m_l1LuldBidRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l1LuldAskRect, &rcPaint))
		{
			DrawText(dc, m_luldAskStr.c_str(), -1, &m_l1LuldAskRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
#ifdef LRPS
		if(intersectRect.IntersectRect(&m_l1LrpBidRect, &rcPaint))
		{
			DrawText(dc, m_lrpBidStr.c_str(), -1, &m_l1LrpBidRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_l1LrpAskRect, &rcPaint))
		{
			DrawText(dc, m_lrpAskStr.c_str(), -1, &m_l1LrpAskRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
#endif
		if(intersectRect.IntersectRect(&m_l1StatusRect, &rcPaint))
		{
			DrawText(dc, &m_statusChar, 1, &m_l1StatusRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_positionSizeRect, &rcPaint))
		{
			DrawText(dc, m_positionSizeStr.c_str(), -1, &m_positionSizeRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_positionPriceRect, &rcPaint))
		{
			DrawText(dc, m_positionPriceStr.c_str(), -1, &m_positionPriceRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_positionClosedPnlRect, &rcPaint))
		{
			DrawText(dc, m_positionClosedPnlStr.c_str(), -1, &m_positionClosedPnlRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_positionOpenPnlRect, &rcPaint))
		{
			DrawText(dc, m_positionOpenPnlStr.c_str(), -1, &m_positionOpenPnlRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
	}

	if(intersectRect.IntersectRect(&m_bidTitleRect, &rcPaint))
	{
		dc.SetTextColor(m_bidColor);
		if(intersectRect.IntersectRect(&m_bidSizeNameRect, &rcPaint))
		{
			DrawText(dc, "Size", -1, &m_bidSizeNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_bidVenueNameRect, &rcPaint))
		{
			DrawText(dc, "Venue", -1, &m_bidVenueNameRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_bidPriceNameRect, &rcPaint))
		{
			DrawText(dc, "Price", -1, &m_bidPriceNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_bidConditionNameRect, &rcPaint))
		{
			DrawText(dc, "C", -1, &m_bidConditionNameRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_bidTimeNameRect, &rcPaint))
		{
			DrawText(dc, "Time", -1, &m_bidTimeNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_bidDelayNameRect, &rcPaint))
		{
			DrawText(dc, "Delay", -1, &m_bidDelayNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
	}
	if(intersectRect.IntersectRect(&m_askTitleRect, &rcPaint))
	{
		dc.SetTextColor(m_askColor);
		if(intersectRect.IntersectRect(&m_askSizeNameRect, &rcPaint))
		{
			DrawText(dc, "Size", -1, &m_askSizeNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_askVenueNameRect, &rcPaint))
		{
			DrawText(dc, "Venue", -1, &m_askVenueNameRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_askPriceNameRect, &rcPaint))
		{
			DrawText(dc, "Price", -1, &m_askPriceNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_askConditionNameRect, &rcPaint))
		{
			DrawText(dc, "C", -1, &m_askConditionNameRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_askTimeNameRect, &rcPaint))
		{
			DrawText(dc, "Time", -1, &m_askTimeNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_askDelayNameRect, &rcPaint))
		{
			DrawText(dc, "Delay", -1, &m_askDelayNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
	}
	if(intersectRect.IntersectRect(&m_printTitleRect, &rcPaint))
	{
		dc.SetTextColor(m_printColor);
		if(intersectRect.IntersectRect(&m_printSizeNameRect, &rcPaint))
		{
			DrawText(dc, "Size", -1, &m_printSizeNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_printVenueNameRect, &rcPaint))
		{
			DrawText(dc, "Venue", -1, &m_printVenueNameRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_printPriceNameRect, &rcPaint))
		{
			DrawText(dc, "Price", -1, &m_printPriceNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_printConditionNameRect1, &rcPaint))
		{
			DrawText(dc, "c1", -1, &m_printConditionNameRect1, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_printConditionNameRect2, &rcPaint))
		{
			DrawText(dc, "c2", -1, &m_printConditionNameRect2, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_printConditionNameRect3, &rcPaint))
		{
			DrawText(dc, "c3", -1, &m_printConditionNameRect3, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_printConditionNameRect4, &rcPaint))
		{
			DrawText(dc, "c4", -1, &m_printConditionNameRect4, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_printTimeNameRect, &rcPaint))
		{
			DrawText(dc, "Time", -1, &m_printTimeNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		if(intersectRect.IntersectRect(&m_printDelayNameRect, &rcPaint))
		{
			DrawText(dc, "Delay", -1, &m_printDelayNameRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
		}
	}

	dc.SetTextColor(oldColor);
//	int h = m_quoteHeight + quoteVgap;
	if(intersectRect.IntersectRect(&m_bidRect, &rcPaint))
	{
		int start = (intersectRect.top - m_bidRect.top) / m_quoteSpace;
		int end = (intersectRect.bottom - m_bidRect.top) / m_quoteSpace;
		if(end * m_quoteSpace != intersectRect.bottom - m_bidRect.top)
		{
			++end;
		}
		if(start < end)
		{
//			for(QuoteVector::const_iterator it = m_bidVector.begin() + start, itend = m_bidVector.end(); it != itend && start < end; ++start, ++it)
			for(QuoteVector::iterator it = m_bidVectorBegin + start, itend = m_bidVectorEnd; it != itend && start < end; ++start, ++it)
			{
				it->Draw(dc, rcPaint);
			}
		}
	}
	if(intersectRect.IntersectRect(&m_askRect, &rcPaint))
	{
		int start = (intersectRect.top - m_askRect.top) / m_quoteSpace;
		int end = (intersectRect.bottom - m_askRect.top) / m_quoteSpace;
		if(end * m_quoteSpace != intersectRect.bottom - m_askRect.top)
		{
			++end;
		}
		if(start < end)
		{
//			for(QuoteVector::const_iterator it = m_askVector.begin() + start, itend = m_askVector.end(); it != itend && start < end; ++start, ++it)
			for(QuoteVector::iterator it = m_askVectorBegin + start, itend = m_askVectorEnd; it != itend && start < end; ++start, ++it)
			{
				it->Draw(dc, rcPaint);
			}
		}
	}

	if(intersectRect.IntersectRect(&m_printRect, &rcPaint))
	{
		int start = (intersectRect.top - m_printRect.top) / m_quoteSpace;
		int end = (intersectRect.bottom - m_printRect.top) / m_quoteSpace;
		if(end * m_quoteSpace != intersectRect.bottom - m_printRect.top)
		{
			++end;
		}
		if(start < end)
		{
			COLORREF bgColor;
			CRect rect(m_printRect.left, m_printRect.top, m_printRect.right, m_printRect.top + m_quoteHeight);
//			for(PrintVector::const_iterator it = m_printVector.begin() + start, itend = m_printVector.end(); it != itend && start < end; ++start, ++it, rect.top += m_quoteSpace, rect.bottom += m_quoteSpace)
			for(PrintVector::iterator it = m_printVectorBegin + start, itend = m_printVectorEnd; it != itend && start < end; ++start, ++it, rect.top += m_quoteSpace, rect.bottom += m_quoteSpace)
			{
				const MyPrint& print = *it;
				bgColor = print.GetBgColor();
				if(bgColor != 0xFFFFFFFF)
				{
					UGUI_FillSolidRect(dc, rect, bgColor);
				}
				dc.SetTextColor(print.GetFgColor());
				print.Draw(dc, rcPaint);
			}
		}
	}
	dc.SetTextColor(oldColor);
//	dc.SelectObject(oldFont);
}

StringSetting* SampleDialogSettingMarketData::GetAccountIdSetting()
{
	return &extensionConfig->m_mdAccountId;
}

void SampleDialogSettingMarketData::UpdateSettings()
{
	BaseSampleDialogSettingOption::UpdateSettings();
	extensionConfig->m_mdSymbol.SetValue(m_symbol);
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_mdOption.SetValue(m_optionMode);
	extensionConfig->m_mdOptionCall.SetValue(m_optionCall);
	extensionConfig->m_mdOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_mdOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_mdOptionExpiration.SetValue(GetExpirationDate());
#endif
}

void SampleDialogSettingMarketData::UpdateControls()
{
}

void SampleDialogSettingMarketData::BeforeDestroy()
{
//	SetAccount(nullptr);
//	TD_GetAdminObservable().RemoveObserver(this);
	TD_GetNewAccountObservable().RemoveObserver(this);
	TD_GetNewPositionObservable().RemoveObserver(this);
	Cleanup();
	BaseSampleDialogSettingOption::BeforeDestroy();
}

void SampleDialogSettingMarketData::Entering()
{
	DoSubscribe();
	BaseSampleDialogSettingOption::Entering();
}

void SampleDialogSettingMarketData::Leaving()
{
	Cleanup();
	BaseSampleDialogSettingOption::Leaving();
}

void SampleDialogSettingMarketData::DCCreated()
{
	m_dc.SelectObject(GetQuoteFont());
	if(!m_sizesCalculated)
	{
		CalculateSizes();
		Resize(true, true);
	}
}

BOOL SampleDialogSettingMarketData::OnInitDialog()
{
	m_sizesCalculated = false;

//	TD_GetAdminObservable().AddObserver(this);
	TD_GetNewAccountObservable().AddObserver(this);
	TD_GetNewPositionObservable().AddObserver(this);

	BOOL ret = BaseSampleDialogSettingOption::OnInitDialog();

//HideOptionControls(true);

	CFont* font = GetQuoteFont();
	if(m_dc.m_hDC)
	{
		m_dc.SelectObject(font);
		CalculateSizes();
		Resize(true, true);
		Invalidate(FALSE);
	}


//	m_EditSymbol.SetLimitText(7);

	EnableSubscribeButton();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingMarketData::RestoreLayout()
{
	BaseSampleDialogSettingOption::RestoreLayout();
//	SetAccountId(extensionConfig->m_mdAccountId.GetValue().c_str());

	m_EditSymbol.SetWindowText(extensionConfig->m_mdSymbol.GetValue().c_str());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_mdOption.GetValue(),
		extensionConfig->m_mdOptionSuffix.GetValue(),
		extensionConfig->m_mdOptionCall.GetValue(),
		extensionConfig->m_mdOptionStrikePrice.GetValue(),
		extensionConfig->m_mdOptionExpiration.GetValue());
#endif

	m_modified = false;
}

void SampleDialogSettingMarketData::DoDataExchange(CDataExchange* pDX)
{
	BaseSampleDialogSettingOption::DoDataExchange(pDX);
//	DDX_Control(pDX, editSymbolId, m_EditSymbol);
	DDX_Control(pDX, buttonSubscribeId, m_ButtonSubscribe);
	DDX_Control(pDX, staticSymbolSubscribedId, m_StaticSymbolSubscribed);
//	DDX_Control(pDX, groupAccountId, m_GroupAccount);
//	DDX_Control(pDX, comboBoxAccountId, m_ComboBoxAccount);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingMarketData, BaseSampleDialogSettingOption)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editSymbolId, OnSymbolChange)
//	ON_EN_UPDATE(editSymbolId, OnSymbolUpdate)
//	ON_LBN_SELCHANGE(listBoxConnectionId, OnSelchangeConnection)
	ON_BN_CLICKED(buttonSubscribeId, OnSubscribe)
//	ON_CBN_SELCHANGE(comboBoxAccountId, OnSelchangeAccount)
END_MESSAGE_MAP()

void SampleDialogSettingMarketData::AccountChanged()
{
	if(m_security)
	{
		SetPosition(m_account ? m_account->FindPositionWithLock(m_security) : nullptr);
	}
}

bool SampleDialogSettingMarketData::CanSubscribe() const
{
	return
#ifndef TAKION_NO_OPTIONS
		AreOptionSettingsValid() &&
#endif
//		(m_security ? strcmp(m_security->GetSymbol(), m_symbol.c_str()) != 0 : !m_symbol.empty());
		(m_security || !m_symbol.empty());
}

void SampleDialogSettingMarketData::Cleanup()
{
	if(m_security)
	{
		SecurityFailed();
	}
	SetPosition(nullptr);
}

BOOL SampleDialogSettingMarketData::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		unsigned char ascii = (unsigned char)pMsg->wParam;
//		HWND focus = ::GetFocus();
//		if(focus == m_EditSymbol.m_hWnd)
//		{
			switch(ascii)
			{
				case VK_RETURN:
				case VK_INSERT:
				if(DoSubscribe())
				{
					return TRUE;
				}
				break;
			}
//		}
	}
	return BaseSampleDialogSettingOption::PreTranslateMessage(pMsg);
}

void SampleDialogSettingMarketData::OnSubscribe()
{
	DoSubscribe();
//	EnableSubscribeButton();
}

bool SampleDialogSettingMarketData::DoSubscribe()
{
	if(CanSubscribe())
	{
		Security* security;
#ifndef TAKION_NO_OPTIONS
		unsigned __int64 rootSymbol;
		unsigned __int64 underlierSymbol;
		unsigned __int64 optionBlock;
		unsigned short optionType;
		bool option = false;
#endif
		if(m_symbol.empty())
		{
			security = nullptr;
		}
		else
		{
#ifndef TAKION_NO_OPTIONS
			if(isOptionUsed())
			{
				option = GetOptionValues(rootSymbol, underlierSymbol, optionBlock, optionType);
				if(option)
				{
					security = TD_ObtainOptionByRootAndOptionBlock(rootSymbol, optionBlock, false);//When the underlier symbol is not known
				}
				else
				{
					security = nullptr;
				}
			}
			else
#endif
				security = TD_ObtainStock(m_symbol.c_str(), false);
		}
		if(security != m_security)
		{
			if(m_security)
			{
				m_security->RemoveObserver(this);
			}
			m_security = security;
			if(m_security)
			{
				m_security->AddObserver(this);
#ifndef TAKION_NO_OPTIONS
				m_security->AddOptionLevel2Observer(this);
#endif
				m_security->Subscribe();
				m_StaticSymbolSubscribed.SetWindowText(m_security->GetSymbol());
				m_roundLot = m_security->GetRoundLot();
				if(!m_roundLot)m_roundLot = 100;
				TD_ReleaseStock(m_security);
				m_unappliedMessageReader.SetLineBufferArray(m_security->GetUnappliedMessages());
				if(m_account)
				{
					SetPosition(m_account->FindPositionWithLock(m_security));
				}
				else
				{
					Account* account = TD_GetCurrentAccount();
					if(account)
					{
						SetPosition(account->FindPositionWithLock(m_security));
					}
					else
					{
						m_StaticSymbolSubscribed.SetWindowText("");
						SetPosition(nullptr);
//						m_unappliedMessageReader.SetLineBufferArray(nullptr);
					}
				}
				SetIteratorSecurity();
			}
			return true;
		}
	}
	return false;
}

void SampleDialogSettingMarketData::SetIteratorSecurity()
{
	TU_MmAggregatedIteratorSetSecurity(m_bidIterator, m_security);
	TU_MmAggregatedIteratorSetSecurity(m_askIterator, m_security);
	TU_MultiPrintIteratorSetSecurity(m_printIterator, m_security);
	SecurityLevel2Updated();
	SecurityLevel1Updated();
	UpdatePosition();
	UpdatePrints();

	EnableSubscribeButton();

	m_EditSymbol.Invalidate(FALSE);
}

void SampleDialogSettingMarketData::SecurityFailed()
{
	if(m_security)
	{
		m_security->RemoveObserver(this);
		m_security = nullptr;
		m_roundLot = 100;
		m_unappliedMessageReader.SetLineBufferArray(nullptr);
	}
	m_StaticSymbolSubscribed.SetWindowText("");

	SetIteratorSecurity();
}

void SampleDialogSettingMarketData::SecurityLevel1Updated()
{
	if(m_security)
	{
		if(m_security->LockInquiryLevel1())
		{
			const Price l1Bid(m_security->GetL1Bid());
			const Price l1Ask(m_security->GetL1Ask());
//			const Price l2Bid(m_security->GetL2Bid());
//			const Price l2Ask(m_security->GetL2Ask());
#ifdef LRPS
			const Price lrpBid(m_security->GetLrpLow());
			const Price lrpAsk(m_security->GetLrpHigh());
#endif
			const Price luldBid(m_security->GetLuldLow());
			const Price luldAsk(m_security->GetLuldHigh());

			const char tradeStatus = m_security->GetTradingState();

			m_security->UnlockInquiryLevel1();

			const bool spreadValid = !l1Ask.isZero() && !l1Bid.isZero();
			SignedPrice l1Spread;
			if(spreadValid)
			{
				l1Spread = l1Ask - l1Bid;
			}

			if(m_l1Bid != l1Bid)
			{
				m_l1Bid = l1Bid;
				m_l1BidStr.clear();
				U_PriceToStr(m_l1BidStr, m_l1Bid, m_l1Bid.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l1BidRect, FALSE);
			}
			if(m_l1Ask != l1Ask)
			{
				m_l1Ask = l1Ask;
				m_l1AskStr.clear();
				U_PriceToStr(m_l1AskStr, m_l1Ask, m_l1Ask.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l1AskRect, FALSE);
			}
			if(m_l1Spread != l1Spread)
			{
				m_l1Spread = l1Spread;
				m_l1SpreadStr.clear();
				if(spreadValid)
				{
					U_SignedPriceToStr(m_l1SpreadStr, m_l1Spread, m_l1Bid.GetDollars() >= 1 ? 2 : 4, '\0', true, '\0', true);
#ifdef _DEBUG
					if(!l1Spread.isPositive())
					{
						printf("%s", m_l1SpreadStr.c_str());
					}
#endif
				}
				::InvalidateRect(m_hWnd, &m_l1SpreadRect, FALSE);
			}
/*
			if(m_l2Bid != l2Bid)
			{
				m_l2Bid = l2Bid;
				m_l2BidStr.clear();
				U_PriceToStr(m_l2BidStr, m_l2Bid, m_l2Bid.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l2BidRect, FALSE);
			}

			if(m_l2Ask != l2Ask)
			{
				m_l2Ask = l2Ask;
				m_l2AskStr.clear();
				U_PriceToStr(m_l2AskStr, m_l2Ask, m_l2Ask.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l2AskRect, FALSE);
			}
*/
#ifdef LRPS
			if(m_l1LrpBid != lrpBid)
			{
				m_l1LrpBid = lrpBid;
				m_lrpBidStr.clear();
				U_PriceToStr(m_lrpBidStr, m_l1LrpBid, m_l1LrpBid.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l1LrpBidRect, FALSE);
			}

			if(m_l1LrpAsk != lrpAsk)
			{
				m_l1LrpAsk = lrpAsk;
				m_lrpAskStr.clear();
				U_PriceToStr(m_lrpAskStr, m_l1LrpAsk, m_l1LrpAsk.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l1LrpAskRect, FALSE);
			}
#endif

			if(m_l1LuldBid != luldBid)
			{
				m_l1LuldBid = luldBid;
				m_luldBidStr.clear();
				U_PriceToStr(m_luldBidStr, m_l1LuldBid, m_l1LuldBid.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l1LuldBidRect, FALSE);
			}

			if(m_l1LuldAsk != luldAsk)
			{
				m_l1LuldAsk = luldAsk;
				m_luldAskStr.clear();
				U_PriceToStr(m_luldAskStr, m_l1LuldAsk, m_l1LuldAsk.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l1LuldAskRect, FALSE);
			}

			if(m_statusChar != tradeStatus)
			{
				m_statusChar = tradeStatus;
				::InvalidateRect(m_hWnd, &m_l1StatusRect, FALSE);
			}

		}
//		ScrollPrintLines(m_unappliedMessageReader.ProcessAllData());
/*
else
{
don't do anything. Stock was not able to lock which means it is being updated in the worker thread. This function will be called again when the stock update is completed.
}
*/
	}
	else
	{
		m_l1Bid.SetZero();
		m_l1Ask.SetZero();
		m_l1Spread.SetZero();
//		m_l2Bid.SetZero();
//		m_l2Ask.SetZero();
		m_l1LuldBid.SetZero();
		m_l1LuldAsk.SetZero();
		if(!m_l1BidStr.empty())
		{
			m_l1BidStr.clear();
			::InvalidateRect(m_hWnd, &m_l1BidRect, FALSE);
		}
		if(!m_l1AskStr.empty())
		{
			m_l1AskStr.clear();
			::InvalidateRect(m_hWnd, &m_l1AskRect, FALSE);
		}
		if(!m_l1SpreadStr.empty())
		{
			m_l1SpreadStr.clear();
			::InvalidateRect(m_hWnd, &m_l1SpreadRect, FALSE);
		}
		if(!m_l2BidStr.empty())
		{
			m_l2BidStr.clear();
			::InvalidateRect(m_hWnd, &m_l2BidRect, FALSE);
		}
		if(!m_l2AskStr.empty())
		{
			m_l2AskStr.clear();
			::InvalidateRect(m_hWnd, &m_l2AskRect, FALSE);
		}
		if(!m_l2SpreadStr.empty())
		{
			m_l2SpreadStr.clear();
			::InvalidateRect(m_hWnd, &m_l2SpreadRect, FALSE);
		}
#ifdef LRPS
		m_l1LrpBid.SetZero();
		m_l1LrpAsk.SetZero();
		if(!m_lrpBidStr.empty())
		{
			m_lrpBidStr.clear();
			::InvalidateRect(m_hWnd, &m_l1LrpBidRect, FALSE);
		}
		if(!m_lrpAskStr.empty())
		{
			m_lrpAskStr.clear();
			::InvalidateRect(m_hWnd, &m_l1LrpAskRect, FALSE);
		}
#endif
		m_l1LuldBid.SetZero();
		m_l1LuldAsk.SetZero();
		if(!m_luldBidStr.empty())
		{
			m_luldBidStr.clear();
			::InvalidateRect(m_hWnd, &m_l1LuldBidRect, FALSE);
		}
		if(!m_luldAskStr.empty())
		{
			m_luldAskStr.clear();
			::InvalidateRect(m_hWnd, &m_l1LuldAskRect, FALSE);
		}
		if(m_statusChar)
		{
			m_statusChar = '\0';
			::InvalidateRect(m_hWnd, &m_l1StatusRect, FALSE);
		}
	}
}

void SampleDialogSettingMarketData::UpdatePosition()
{
	if(m_position)
	{
		if(m_position->LockInquiry())
		{
			Price positionPrice(m_position->GetAveragePrice());
			int size = m_position->GetSize();
			Money pnlClosed = m_position->GetClosedPnl();
			Money pnlOpen = m_position->GetOpenPnlNbboPrint();

			m_position->UnlockInquiry();

			static char num[33];

			if(m_positionPrice != positionPrice || m_positionPriceStr.empty())
			{
				m_positionPrice = positionPrice;
				m_positionPriceStr.clear();
				U_PriceToStr(m_positionPriceStr, m_positionPrice, 2);
				::InvalidateRect(m_hWnd, &m_positionPriceRect, FALSE);
			}
			if(m_positionSize != size || m_positionSizeStr.empty())
			{
				m_positionSize = size;
				m_positionSizeStr = U_SignedNumberToStrNoDecDigits(m_positionSize, num, sizeof(num));
				::InvalidateRect(m_hWnd, &m_positionSizeRect, FALSE);
			}
			if(m_positionClosedPnl != pnlClosed || m_positionClosedPnlStr.empty())
			{
				m_positionClosedPnl = pnlClosed;
				m_positionClosedPnlStr.clear();
				U_MoneyToStr(m_positionClosedPnlStr, m_positionClosedPnl, 2);
				::InvalidateRect(m_hWnd, &m_positionClosedPnlRect, FALSE);
			}
			if(m_positionOpenPnl != pnlOpen || m_positionOpenPnlStr.empty())
			{
				m_positionOpenPnl = pnlOpen;
				m_positionOpenPnlStr.clear();
				U_MoneyToStr(m_positionOpenPnlStr, m_positionOpenPnl, 2);
				::InvalidateRect(m_hWnd, &m_positionOpenPnlRect, FALSE);
			}

		}
	}
	else
	{
		m_positionSize = 0;
		m_positionPrice.SetZero();
		m_positionClosedPnl.SetZero();
		m_positionOpenPnl.SetZero();
		if(!m_positionSizeStr.empty())
		{
			m_positionSizeStr.clear();
			::InvalidateRect(m_hWnd, &m_positionSizeRect, FALSE);
		}
		if(!m_positionPriceStr.empty())
		{
			m_positionPriceStr.clear();
			::InvalidateRect(m_hWnd, &m_positionPriceRect, FALSE);
		}
		if(!m_positionClosedPnlStr.empty())
		{
			m_positionClosedPnlStr.clear();
			::InvalidateRect(m_hWnd, &m_positionClosedPnlRect, FALSE);
		}
		if(!m_positionOpenPnlStr.empty())
		{
			m_positionOpenPnlStr.clear();
			::InvalidateRect(m_hWnd, &m_positionOpenPnlRect, FALSE);
		}
	}
}

bool SampleDialogSettingMarketData::ProcessUnappliedMessage(const Message* message)
{
	switch(message->m_type)
	{
		case TM_STOCK_ADD_PRINT:
		return DoAddPrint((const TMsgStockAddPrint*)message);
/*
		case TM_STOCK_ADD_BOOK_EXECUTION:
		return DoAddBookExecution((const TMsgStockAddBookExecution*)message);
*/
		default:
		return false;
	}
}

void MyPrint::SetValues(unsigned int mmid,
	unsigned short shortMmid,
	unsigned int dollars,
	unsigned int dollarFraction,
	unsigned int quantity,
	unsigned int roundLot,
	unsigned char source,
	unsigned char level1,
	char saleCondition1,
	char saleCondition2,
	char saleCondition3,
	char saleCondition4,
	unsigned int millisecond,
	short delay,
	bool historical,
	COLORREF bgColor, COLORREF fgColor)
{
	NamedPrint::SetValues(
		mmid,
		shortMmid,
		dollars,
		dollarFraction,
		quantity,
		source,
		level1,
		saleCondition1,
		saleCondition2,
		saleCondition3,
		saleCondition4,
		millisecond,
		delay,
		historical);
	m_bgColor = bgColor;
	m_fgColor = fgColor;

	static char num[33];

	m_sizeStr = U_UnsignedNumberToStrNoDecDigits(m_quantity / (roundLot ? roundLot : 100), num, sizeof(num));

	*(unsigned int*)m_venueStr = m_mmid;

	m_priceStr.clear();
	U_PriceToStr(m_priceStr, *this, 4);//true, '\0', true);

	U_FormatMillisecond(m_millisecond, num, sizeof(num), '\0', true);//m_showQuoteMilliseconds);
	m_timeStr = num;

	if(historical)
	{
		m_delayStr = 'H';

	}
	else
	{
		m_delayStr = U_SignedNumberToStrNoDecDigits(m_delay, num, sizeof(num));
	}
}

bool SampleDialogSettingMarketData::DoAddPrint(const TMsgStockAddPrint* msg)
{
//	if(m_printFilter & (1 << msg->m_source))
//	{

		COLORREF bgColor;// = 0xFFFFFFFF;
		COLORREF textColor;
		switch(msg->m_level1)
		{
			case TL1_LESS_BID:
			bgColor = m_colorPrintSellAggressive;
			textColor = m_colorPrintSellAggressiveText;
			break;

			case TL1_EQUAL_BID:
			bgColor = 0xFFFFFFFF;
			textColor = m_colorPrintSell;
			break;

			case TL1_EQUAL_ASK:
			bgColor = 0xFFFFFFFF;
			textColor = m_colorPrintBuy;
			break;

			case TL1_GREATER_ASK:
			bgColor = m_colorPrintBuyAggressive;
			textColor = m_colorPrintBuyAggressiveText;
			break;

			default://case TL1_BETWEEN_BIDASK:
			bgColor = 0xFFFFFFFF;
			textColor = m_colorPrintNeutral;
			break;
		}

		if(m_quoteCount)
		{
			if(m_quoteCount > 1)
			{
				PrintVector::reverse_iterator it = m_printVector.rbegin();
				PrintVector::reverse_iterator itPrev = it;
				++itPrev;
				for(PrintVector::reverse_iterator itend = m_printVector.rend(); itPrev != itend; ++it, ++itPrev)
				{
					it->CopyData(*itPrev);
				}
			}

//		m_printVector.push_front(DisplayPrint(true,
			MyPrint& print = m_printVector[0];
			print.SetValues(
				msg->m_mmid,
				msg->m_shortMmid,
				msg->m_dollars,
				msg->m_dollarFraction,
				msg->m_quantity,
				m_roundLot,
				msg->m_source,
				msg->m_level1,
				msg->m_saleCondition1,
				msg->m_saleCondition2,
				msg->m_saleCondition3,
				msg->m_saleCondition4,
				msg->m_time,
				msg->m_delay,
				false,
				bgColor,
				textColor);
			ScrollWindowEx(0, m_quoteSpace, &m_printRectMinusOneLine, nullptr, nullptr, nullptr, SW_INVALIDATE);
			return true;
		}

//	}
	return false;
}

void SampleDialogSettingMarketData::UpdatePrints()
{
	PrintVector::iterator it = m_printVectorBegin, itend = m_printVectorEnd;
	if(m_security)
	{
//		m_printVector.clear();

		m_security->LockInquiryWaitPrints();

		m_printIterator->Reset();
		const Trade* trade = TU_GetMultiPrintNextTrade(m_printIterator);
		COLORREF bgColor;// = 0xFFFFFFFF;
		COLORREF textColor;
		for(unsigned int i = 0; trade && i < m_quoteCount && it != itend; ++it, ++i, trade = TU_GetMultiPrintNextTrade(m_printIterator))
		{
			switch(trade->GetLevel1())
			{
				case TL1_LESS_BID:
				bgColor = m_colorPrintSellAggressive;
				textColor = m_colorPrintSellAggressiveText;
				break;

				case TL1_EQUAL_BID:
				bgColor = 0xFFFFFFFF;
				textColor = m_colorPrintSell;
				break;

				case TL1_EQUAL_ASK:
				bgColor = 0xFFFFFFFF;
				textColor = m_colorPrintBuy;
				break;

				case TL1_GREATER_ASK:
				bgColor = m_colorPrintBuyAggressive;
				textColor = m_colorPrintBuyAggressiveText;
				break;

				default://case TL1_BETWEEN_BIDASK:
				bgColor = 0xFFFFFFFF;
				textColor = m_colorPrintNeutral;
				break;
			}
			MyPrint& print = *it;
			print.SetValues(
				trade->GetMmid(),
				trade->GetShortMmid(),
				trade->GetDollars(),
				trade->GetDollarFraction(),
				trade->GetQuantity(),
				m_roundLot,
				trade->GetSource(),
				trade->GetLevel1(),
				trade->GetSaleCondition1(),
				trade->GetSaleCondition2(),
				trade->GetSaleCondition3(),
				trade->GetSaleCondition4(),
				trade->GetMillisecond(),
				trade->GetDelay(),
				trade->isHistorical(),
				bgColor,
				textColor);
		}

		m_security->UnlockInquiryPrints();
/*
		for(; it != itend; ++it)
		{
			MyPrint& print = *it;
			print.SetEmpty();
		}
*/
	}
//	else
//	{
		for(; it != itend; ++it)
		{
			MyPrint& print = *it;
			print.SetEmpty();
		}
//	}
	::InvalidateRect(m_hWnd, &m_printRect, FALSE);
}

void SampleDialogSettingMarketData::ScrollPrintLines(int count)
{
	if(count > 0)
	{
		if(count < (int)m_printVector.capacity())
		{
/*
	//				ScrollWindowEx(0, count * m_lineHeightPrint, &m_printRectMinusOneLine, nullptr, nullptr, nullptr, SW_INVALIDATE);
			if(count == 1)
			{
				ScrollWindowEx(0, m_lineHeightPrint, &m_printRectMinusOneLine, nullptr, nullptr, nullptr, SW_INVALIDATE);
			}
			else
			{
				CRect r(m_printRect);
				int dy = count * m_lineHeightPrint;
				r.bottom -= dy;
				ScrollWindowEx(0, dy, &r, nullptr, nullptr, nullptr, SW_INVALIDATE);
				if(r.top + dy > r.bottom)
				{
					int b = r.bottom;
					r.bottom = r.top + dy;
					r.top = b;
					::InvalidateRect(m_hWnd, &r, FALSE);//can cause a ptoblem if it scrolls again before repainting
				}
			}
*/
		}
		else
		{
			::InvalidateRect(m_hWnd, &m_printRect, FALSE);
		}
	}
}

void SampleDialogSettingMarketData::SecurityLevel2Updated()
{
	QuoteVector::iterator itBid = m_bidVectorBegin;
	QuoteVector::iterator itBidEnd = m_bidVectorEnd;
	QuoteVector::iterator itAsk = m_askVectorBegin;
	QuoteVector::iterator itAskEnd = m_askVectorEnd;
	if(m_security)
	{
		const Quote* quote;
		if(m_security->LockInquiryLevel2())
		{
			m_bidIterator->Reset();
			quote = TU_GetNextQuote(m_bidIterator);
			for(; itBid != itBidEnd && quote; ++itBid, quote = TU_GetNextQuote(m_bidIterator))
			{
				itBid->SetQuote(*quote, m_roundLot);
			}

			m_askIterator->Reset();
			quote = TU_GetNextQuote(m_askIterator);
			for(; itAsk != itAskEnd && quote; ++itAsk, quote = TU_GetNextQuote(m_askIterator))
			{
				itAsk->SetQuote(*quote, m_roundLot);
			}

			m_security->UnlockInquiryLevel2();

			for(; itBid != itBidEnd; ++itBid)
			{
				itBid->SetEmpty();//There were too few quotes. Set the rest of the display quotes empty.
			}
			for(; itAsk != itAskEnd; ++itAsk)
			{
				itAsk->SetEmpty();//There were too few quotes. Set the rest of the display quotes empty.
			}
		}
/*
else
{
don't do anything. Stock was not able to lock which means it is being updated in the worker thread. This function will be called again when the stock update is completed.
}
*/
		if(m_security->LockInquiryLevel1())
		{
			const Price l2Bid(m_security->GetL2Bid());
			const Price l2Ask(m_security->GetL2Ask());
			m_security->UnlockInquiryLevel1();
			
			const bool spreadValid = !l2Ask.isZero() && !l2Bid.isZero();

			SignedPrice l2Spread;
			if(spreadValid)
			{
				l2Spread = l2Ask - l2Bid;
			}

			if(m_l2Bid != l2Bid)
			{
				m_l2Bid = l2Bid;
				m_l2BidStr.clear();
				U_PriceToStr(m_l2BidStr, m_l2Bid, m_l2Bid.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l2BidRect, FALSE);
			}

			if(m_l2Ask != l2Ask)
			{
				m_l2Ask = l2Ask;
				m_l2AskStr.clear();
				U_PriceToStr(m_l2AskStr, m_l2Ask, m_l2Ask.GetDollars() >= 1 ? 2 : 4);
				::InvalidateRect(m_hWnd, &m_l2AskRect, FALSE);
			}

			if(m_l2Spread != l2Spread)
			{
				m_l2Spread = l2Spread;
				m_l2SpreadStr.clear();
				if(spreadValid)
				{
					U_SignedPriceToStr(m_l2SpreadStr, m_l2Spread, m_l2Bid.GetDollars() >= 1 ? 2 : 4, '\0', true, '\0', true);
#ifdef _DEBUG
					if(!m_l2Spread.isPositive())
					{
						printf("%s", m_l2SpreadStr.c_str());
					}
#endif
				}
				::InvalidateRect(m_hWnd, &m_l2SpreadRect, FALSE);
			}
		}
	}
	else
	{
		for(; itBid != itBidEnd; ++itBid)
		{
			itBid->SetEmpty();//There were too few quotes. Set the rest of the display quotes empty.
		}
		for(; itAsk != itAskEnd; ++itAsk)
		{
			itAsk->SetEmpty();//There were too few quotes. Set the rest of the display quotes empty.
		}
		if(!m_l2Bid.isZero())
		{
			m_l2Bid.SetZero();
			::InvalidateRect(m_hWnd, &m_l2BidRect, FALSE);
		}
		if(!m_l2Ask.isZero())
		{
			m_l2Ask.SetZero();
			::InvalidateRect(m_hWnd, &m_l2AskRect, FALSE);
		}
		if(!m_l2Spread.isZero())
		{
			m_l2Spread.SetZero();
			::InvalidateRect(m_hWnd, &m_l2SpreadRect, FALSE);
		}
	}
}
/*
void SampleDialogSettingMarketData::UpdateExtensionEntitlements()
{
	const unsigned int extEntitlements = TU_GetExtensionEntitlementBookFilter();
	TU_MmAggregatedIteratorSetExtensionEntitlements(m_bidIterator, extEntitlements);
	TU_MmAggregatedIteratorSetExtensionEntitlements(m_askIterator, extEntitlements);
	TU_MultiPrintIteratorUpdateAllEntitlements(m_printIterator);
}
*/
void SampleDialogSettingMarketData::UpdateEntitlements()
{
	TU_MmAggregatedIteratorUpdateEntitlements(m_bidIterator);
	TU_MmAggregatedIteratorUpdateEntitlements(m_askIterator);
	TU_MultiPrintIteratorUpdateAllEntitlements(m_printIterator);
	SecurityLevel2Updated();
	SecurityLevel1Updated();
}

void SampleDialogSettingMarketData::SecurityRefreshed()
{
	TU_MmAggregatedIteratorSecurityRefreshed(m_bidIterator, m_security);
	TU_MmAggregatedIteratorSecurityRefreshed(m_askIterator, m_security);
	TU_MultiPrintIteratorSetSecurity(m_printIterator, m_security);

	SecurityLevel2Updated();
	SecurityLevel1Updated();
	UpdatePrints();
}

void SampleDialogSettingMarketData::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_STOCK_UPDATE_LEVEL2:
#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_UPDATE_LEVEL2:
		case TM_FUTURE_UPDATE_LEVEL2:
#endif
		SecurityLevel2Updated();
		break;

		case TM_STOCK_UPDATE_LEVEL1:
		case TM_STOCK_IMBALANCE_UPDATE:
#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_UPDATE_LEVEL1:
		case TM_FUTURE_UPDATE_LEVEL1:
#endif
		SecurityLevel1Updated();
		ScrollPrintLines(m_unappliedMessageReader.ProcessAllData());
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_OPENED:
		case TM_FUTURE_OPENED:
		case TM_OPTION_CLOSED:
		case TM_FUTURE_CLOSED:
		case M_FUT_EOD_SUMMARY_REPORT:
		case M_OPT_EOD_SUMMARY_REPORT:
#endif
		case M_EOD_SUMMARY_REPORT:
		case SM_M_EOD_SUMMARY_REPORT:
		case TM_STOCK_CLOSED:
		case TM_STOCK_OPENED:
		case TM_STOCK_YEST_CLOSE_PRICE_CHANGED:
		case M_SHORT_SALE_INDICATORS:
		case SM_M_SHORT_SALE_INDICATORS:
		case M_DIVIDENDS_AND_SPLITS:
		case SM_M_DIVIDENDS_AND_SPLITS:
		case M_52WEEK_30DAY_HIGH_LOW:
		case SM_M_52WEEK_30DAY_HIGH_LOW:
		case M_LULD:
		case SM_M_LULD:
		case M_PMI:
		case SM_M_PMI:
		case TM_EQUITY_REFRESHED:
		case TM_EQUITY_RESET:
		case TM_SORTABLE_SECURITY_SYMBOL_MAPPING_CHANGED:
		case M_AVERAGE_DAILY_VOLUME:
		case SM_M_AVERAGE_DAILY_VOLUME:
		case M_PREVIOUS_DAY_HIGH_LOW:
		case SM_M_PREVIOUS_DAY_HIGH_LOW:
		case M_SECURITY_TYPE:
		case SM_M_SECURITY_TYPE:
		case M_TICK_PILOT:
		case SM_M_TICK_PILOT:
		case TM_SET_STOCK_TIER_SIZE:
		case TM_SET_STOCK_CUSTOM_PRICE_BASE:
		case TM_SET_STOCK_CUSTOM_DATE:
		case TM_STOCK_BORROW_PRICE:
		case TM_STOCK_BETA_AND_RETAIL_INFO:
		case TM_STOCK_BORROW_INFO:
		case TM_STOCK_HTB:
		SecurityLevel1Updated();
		break;

		case TM_STOCK_LIGHT_SNAPSHOT:
		SecurityLevel2Updated();
		SecurityLevel1Updated();
//		UpdatePrints();
		break;

		case TM_UPDATED_POSITION:
		UpdatePosition();
		break;

		case TM_STOCK_ADD_BOOK_EXECUTION:
//		AddBookExecution((const TMsgStockAddBookExecution*)message);
		break;

		case TM_STOCK_ENTITLEMENTS_CHANGED:
		case TM_STOCK_EXT_ENTITLEMENTS_CHANGED:
#ifndef TAKION_NO_OPTIONS
		case TM_FUTURE_ENTITLEMENTS_CHANGED:
		case TM_FUTURE_EXT_ENTITLEMENTS_CHANGED:
		case TM_OPTION_ENTITLEMENTS_CHANGED:
		case TM_OPTION_EXT_ENTITLEMENTS_CHANGED:
#endif
		UpdateEntitlements();

//fall through
		case TM_STOCK_REFRESHED:
#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_REFRESHED:
		case TM_FUTURE_REFRESHED:
#endif

		SecurityRefreshed();
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_INVALID:
		{
			const OptionInfo* optionInfo = m_security->GetOptionInfo();
			if(optionInfo)
			{
				const TMsgOptionSymbol* msg = (const TMsgOptionSymbol*)message;
				if(m_security->GetNumericSymbol() == *(unsigned __int64*)msg->m_symbol && m_security->GetOptionBlock() == msg->m_optionBlock.GetOptionBlock())
				{
					SecurityFailed();
				}
			}
		}
		break;

		case TM_UNDERLIER_DELETE:
		break;

		case TM_FUTURE_INVALID:
#endif
		case TM_STOCK_INVALID:
		if(m_security && m_security->GetNumericSymbol() == *(unsigned __int64*)((const TMessageSymbol*)message)->m_symbol)
		{
			SecurityFailed();
		}
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_FUTURE_DELETE:
#endif
		case TM_STOCK_DELETE:
		if(m_security == ((const TMsgSecurity*)message)->m_security)
		{
			SecurityFailed();
		}
		break;

		case TM_NEW_POSITION:
		if(m_security)
		{
			const Position* position = ((TMsgNewPosition*)message)->m_position;
			if(position->GetAccount() == m_account && !strcmp(position->GetSymbol(), m_security->GetSymbol()))
			{
				SetPosition(position);
			}
		}
		break;
/*
		case TS_NEW_ACCOUNT:
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			Account* account = msg->m_account;
			int sel = m_ComboBoxAccount.AddString((const char*)account);
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
		m_ComboBoxAccount.ResetContent();
		m_ComboBoxAccount.AddString(nullptr);
		m_ComboBoxAccount.SetCurSel(0);
//		SetPosition(nullptr);
		OnSelchangeAccount();
		break;


//		case M_RESP_MD_LOGON:
//		break;

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
*/
		case TS_ACCOUNT_POSITION_RESP:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;
			if(m_security)
			{
				SetPosition(account->FindPositionWithLock(m_security));
			}
		}
		break;

		default:
		BaseSampleDialogSettingOption::Notify(message, from, info);
		break;
	}
}

void SampleDialogSettingMarketData::SetPosition(const Position* position)
{
	if(m_position != position)
	{
		if(m_position)
		{
			m_position->RemoveObserver(this);
		}
		m_position = position;
		if(m_position)
		{
			m_position->AddObserver(this);
		}
		UpdatePosition();
	}
}

#ifndef TAKION_NO_OPTIONS
bool SampleDialogSettingMarketData::OptionModeChanged()
{
	bool ret = BaseSampleDialogSettingOption::OptionModeChanged();
	EnableSubscribeButton();
	return ret;
}

void SampleDialogSettingMarketData::OptionValuesChanged()
{
	EnableSubscribeButton();
}

#endif

void SampleDialogSettingMarketData::EnableSubscribeButton()
{
	EnableControl(m_ButtonSubscribe.m_hWnd, CanSubscribe(), m_EditSymbol.m_hWnd);
}
/*
void SampleDialogSettingMarketData::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}
*/
void SampleDialogSettingMarketData::OnSymbolChange()
{
	const bool empty = m_symbol.empty();
	const bool symbolCurrent = m_security && !strcmp(m_security->GetSymbol(), m_symbol.c_str());
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_symbol = text;
	if(m_symbol.empty() != empty || (m_security && !strcmp(m_security->GetSymbol(), m_symbol.c_str())) != symbolCurrent)
	{
		m_EditSymbol.Invalidate(FALSE);
		EnableSubscribeButton();
//		m_modified = true;
	}
}


HBRUSH SampleDialogSettingMarketData::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSymbolId:
		if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		else if(!m_security || strcmp(m_security->GetSymbol(), m_symbol.c_str()))
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;

	}
	return BaseSampleDialogSettingOption::OnCtlColor(pDC, pWnd, nCtlColor);
}

