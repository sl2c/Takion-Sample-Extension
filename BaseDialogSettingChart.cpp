#include "stdafx.h"
#include "BaseDialogSettingChart.h"
#include "ExtensionDerivedConfig.h"
#include "ExtensionSample.h"

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int ListBoxHistChart::Compare(const void* item1, const void* item2) const
{
	const ChartPoint* c1 = (const ChartPoint*)item1;
	const ChartPoint* c2 = (const ChartPoint*)item2;
	return c1->GetDate() < c2->GetDate() ? -1:
		c1->GetDate() > c2->GetDate() ? 1:
		c1->GetMinute() < c2->GetMinute() ? -1:
		c1->GetMinute() > c2->GetMinute() ? 1:
		0;
}

void ListBoxHistChart::DestroyItem(const void* item) const
{
	delete (ChartPoint*)item;
}

void* ListBoxHistChart::CloneItem(const void* item) const
{
	return new ChartPoint(*(const ChartPoint*)item);
}

void ListBoxHistChart::DoDrawItem(const DRAWITEMSTRUCT* lpDrawItemStruct, const CRect& drawItemRect)
{
	static const unsigned int minDecDigits = 2;

	const ChartPoint* chartPoint = (const ChartPoint*)lpDrawItemStruct->itemData;

	SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);
	COLORREF oldTextColor = SetTextColor(lpDrawItemStruct->hDC, GetStringColor(lpDrawItemStruct));

	CRect r(drawItemRect);
	int h = r.Height();

	int right = drawItemRect.right;
	LbHorizontalScroller::ColumnConstIterator it = m_horizontalScroller->GetColumnBegin();
	LbHorizontalScroller::ColumnConstIterator itend = m_horizontalScroller->GetColumnEnd();
	if(it != itend)
	{
		std::string info;
		char num[33];
		char* cursor;
//		unsigned int val;
		const LbColumn* column;
		HDC hDC = lpDrawItemStruct->hDC;
		int scrollOffset = m_horizontalScroller->GetColumnScrollOffset();
		static const unsigned int al = DT_VCENTER|DT_NOCLIP|DT_SINGLELINE|DT_NOPREFIX;
		unsigned int alignment;
		int textOffset;
		for(; it != itend; ++it)
		{
			const PointLocatorInt& pointLocator = it->first;
			r.left = pointLocator.GetStart() - scrollOffset;
			if(r.left >= right)
			{
				break;
			}
			r.right = pointLocator.GetEnd() - scrollOffset;

			column = it->second;
			alignment = column->GetDataAlignment();
			textOffset = column->GetDataOffset();

			if(textOffset)
			{
				switch(alignment)
				{
					case DT_LEFT:
					r.left += textOffset;
					break;

					case DT_RIGHT:
					r.right -= textOffset;
					break;

					case DT_CENTER:
					r.left += textOffset;
					r.right -= textOffset;
					break;
				}
			}
			alignment |= al;
			switch(column->GetId())
			{
				case HCI_DATE:
				cursor = U_FormatDate(chartPoint->GetDate(), num, sizeof(num), '-', '\0', false, false);
				*cursor = '\0';
				DrawText(hDC, num, -1, &r, alignment);
				break;

				case HCI_TIME:
				if(chartPoint->GetMinute())
				{
					U_FormatMinute(chartPoint->GetMinute(), num, sizeof(num));
					DrawText(hDC, num, -1, &r, alignment);
				}
				break;

				case HCI_START:
				DrawText(hDC, U_UnsignedMonetaryToBuffer(chartPoint->GetStartPrice(), num, sizeof(num), m_decDigit, true, '\0', minDecDigits), -1, &r, alignment);
				break;

				case HCI_LOW:
/*
if(chartPoint->GetMinute() == 917)
{
	printf("X");
}
*/
				DrawText(hDC, U_UnsignedMonetaryToBuffer(chartPoint->GetLowPrice(), num, sizeof(num), m_decDigit, true, '\0', minDecDigits), -1, &r, alignment);
				break;

				case HCI_HIGH:
				DrawText(hDC, U_UnsignedMonetaryToBuffer(chartPoint->GetHighPrice(), num, sizeof(num), m_decDigit, true, '\0', minDecDigits), -1, &r, alignment);
				break;

				case HCI_END:
				DrawText(hDC, U_UnsignedMonetaryToBuffer(chartPoint->GetEndPrice(), num, sizeof(num), m_decDigit, true, '\0', minDecDigits), -1, &r, alignment);
				break;

				case HCI_VOLUME:
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(chartPoint->GetVolume(), num, sizeof(num)), -1, &r, alignment);
				break;

			}
		}
	}
	SetTextColor(lpDrawItemStruct->hDC, oldTextColor);

}
LbHorizontalScroller* ListBoxHistChart::CreateHorizontalScroller()
{
	return new LbHistChartHorizontalScroller(this);
}

void LbHistChartHorizontalScroller::AddColumns()
{
	int itemHeight = m_listBox->GetMyItemHeight();
	int doubleHeight = itemHeight + itemHeight;
	int tripleHeight = doubleHeight + itemHeight;
	int fourHeight = tripleHeight + itemHeight;
	int fiveHeight = fourHeight + itemHeight;
//	int sixHeight = fiveHeight + itemHeight;
//	int sevenHeight = sixHeight + itemHeight;
//	int eightHeight = sevenHeight + itemHeight;

	AddColumn("Date", HCI_DATE, fiveHeight, DT_RIGHT, DT_CENTER, DT_RIGHT, "Candle Date", 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, itemHeight, itemHeight);
	AddColumn("Time", HCI_TIME, fiveHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Port");//, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
	AddColumn("Start", HCI_START, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Candle Start Price");//, 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, 0);
	AddColumn("Low", HCI_LOW, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Candle Low Price");
	AddColumn("High", HCI_HIGH, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Candle High Price");
	AddColumn("End", HCI_END, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Candle End Price");
	AddColumn("Volume", HCI_VOLUME, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Volume");//, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0);
}

BaseDialogSettingChart::BaseDialogSettingChart(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label, int width)://, const Account* account):
	BaseDialogSetting(mainWnd, parentTab, label),
	m_EditSymbol(U_GetValidStockChars(), false),
//	m_SpinDecDigit(9, 0, 4, 100, 3, TakionSpin::charComma),
	m_SpinDecDigit(9, 0, 4, 1, 3, TakionSpin::charComma),

	m_today(TL_GetYear() * 10000 + TL_GetMonth() * 100 + TL_GetDay()),

	m_decDigit(4),

	m_marketSorterConnected(false),
	m_marketSorterLoaded(false),
//	m_historicalPrintConnected(false),
	m_totalVolume(0),

	m_listBoxBorderSize2(0),
	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),
	m_width(width),
	m_firstColumnBottom(0)

//	m_historicalChartLoadingToSave(nullptr),
//	m_dateFromLoadingToSave(0)
{
	m_SpinDecDigit.AddSpinCell(1, false);

	int x = 0;
	int y = 0;
//	int width = 55;
	int xc = x + hgap;

	AddStaticIndirect("Symbol", WS_VISIBLE | SS_NOPREFIX, 0, xc, y, m_width, staticHeight, staticChartSymbolNameId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xc, y += staticHeight, m_width, editHeight, editChartSymbolId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += editHeight, m_width, staticHeight, staticChartSymbolId);

	m_firstColumnBottom = y + staticHeight + vgap;

	const int decWidth = spinHeight + spinHeight;
//	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_GROUP, 0, xc, m_firstColumnBottom, m_width, spinHeight, spinChartDecDigitId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_GROUP, 0, xc, m_firstColumnBottom, decWidth, spinHeight, spinChartDecDigitId);
	AddStaticIndirect("Dec Digits", WS_VISIBLE | SS_NOPREFIX, 0, xc + decWidth, m_firstColumnBottom + (spinHeight - staticHeight) / 2, m_width - decWidth, staticHeight, staticChartDecDigitId);

	m_firstColumnBottom += spinHeight + vgap;

	y = vgap + staticHeight;
	xc += width + hgap;
	width = m_width - xc;

	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y, width, m_height - staticHeight - y, listBoxHistChartId);
//InvalidateAllTotalValueRects
	AddListBox(&m_ListBoxChart);

	SetMarketSorterConnected(TD_IsMarketSorterConnected());
	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
//	SetHistoricalPrintConnected(TD_IsHistoricalPrintConnected());

	TD_GetNewEquityObservable().AddObserver(this);//to get notified about new Securities created
	TD_GetAdminObservable().AddObserver(this);//to get notified about connect / disconnect
//	TD_GetHistoricalChartObservable().AddObserver(this);//to get notified about loading the historical charts
}

BaseDialogSettingChart::~BaseDialogSettingChart()
{
	TD_GetNewEquityObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
//	TD_GetHistoricalChartObservable().RemoveObserver(this);
}

HWND BaseDialogSettingChart::GetFirstTabControl() const
{
	return m_EditSymbol.m_hWnd;
}

void BaseDialogSettingChart::BeforeDestroy()
{
	m_symbol.clear();
	m_symbolInitial.clear();

	m_decDigit = 4;
	m_ListBoxChart.SetDecDigit(m_decDigit);

	m_listBoxBorderSize2 = 0;
	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;

	BaseDialogSetting::BeforeDestroy();
}

void BaseDialogSettingChart::ToolTipsEnabled(bool enable)
{
	BaseDialogSetting::ToolTipsEnabled(enable);
	m_ListBoxChart.EnableTooltips(enable, m_toolTip);
}

BOOL BaseDialogSettingChart::OnInitDialog()
{
	m_symbol.clear();
	m_symbolInitial.clear();

	m_decDigit = 4;
	m_ListBoxChart.SetDecDigit(m_decDigit);

	m_listBoxBorderSize2 = 0;
	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;

	BOOL ret = BaseDialogSetting::OnInitDialog();

	m_EditSymbol.SetLimitText(7);

	m_SpinDecDigit.SetValue(m_decDigit, true, true, false);

	SetToolTipsEnabled(true);
//	m_ListBoxChart.EnableTooltips(true, m_toolTip);
	AddTitle(&m_ListBoxChart);

	return ret;
}

void BaseDialogSettingChart::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
	DDX_Control(pDX, listBoxHistChartId, m_ListBoxChart);
	DDX_Control(pDX, editChartSymbolId, m_EditSymbol);
	DDX_Control(pDX, staticChartSymbolId, m_StaticSymbol);
	DDX_Control(pDX, spinChartDecDigitId, m_SpinDecDigit);
	DDX_Control(pDX, buttonLoadId, m_ButtonLoad);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingChart, BaseDialogSetting)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editChartSymbolId, OnSymbolChange)
	ON_EN_UPDATE(editChartSymbolId, OnSymbolUpdate)
	ON_BN_CLICKED(buttonLoadId, OnLoad)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

LRESULT BaseDialogSettingChart::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinChartDecDigitId:
		m_SpinDecDigit.UpdateValueNum();
		{
			const unsigned char decDigit = m_SpinDecDigit.GetValue();
/*
			if(m_dayCountMode)
			{
				m_SpinDayCount.SetBkColorEnabled(dayCount ? dayCount == m_dayCountInitial ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetPendingColor() : m_visualBase->GetWarningColor());
			}
*/
			if(decDigit != m_decDigit)
			{
				m_decDigit = decDigit;
				m_ListBoxChart.SetDecDigit(decDigit);
			}
		}
		break;
	}
	return 0;
}

void BaseDialogSettingChart::UpdateToday()
{
	m_today = TL_GetYear() * 10000 + TL_GetMonth() * 100 + TL_GetDay();
}

void BaseDialogSettingChart::UpdateCurrentLoadSettingsAndSymbol()
{
	if(m_symbolInitial != m_symbol)
	{
		m_symbolInitial = m_symbol;
//		m_EditSymbol.SetWindowText(m_symbolInitial.c_str());
		m_StaticSymbol.SetWindowText(m_symbolInitial.c_str());
		m_EditSymbol.Invalidate(FALSE);
		m_modified = true;
	}

	UpdateAllCurrentLoadSettings();
	EnableLoadButton();
}

void BaseDialogSettingChart::EnableLoadButton()
{
	EnableControl(m_ButtonLoad.m_hWnd, CanLoad(), m_EditSymbol.m_hWnd);
}

bool BaseDialogSettingChart::canLoad() const
{
	return !m_symbol.empty();
}

bool BaseDialogSettingChart::CanLoad() const
{
	return canLoad() && m_symbol != m_symbolInitial;
}

void BaseDialogSettingChart::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void BaseDialogSettingChart::OnSymbolChange()
{
//	bool empty = m_symbol.empty();
//	bool symbolCurrent = m_security && !strcmp(m_security->GetSymbol(), m_symbol.c_str());
	if(SymbolCanChange())//!m_historicalChartLoadingToSave)
	{
		CString text;
		m_EditSymbol.GetWindowText(text);
		m_symbol = text;
		m_EditSymbol.Invalidate(FALSE);
		EnableLoadButton();
//		m_modified = true;
	}
}

BOOL BaseDialogSettingChart::PreTranslateMessage(MSG* pMsg)
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
				if(DoLoad())
				{
					return TRUE;
				}
				break;
			}
//		}
	}
	return BaseDialogSetting::PreTranslateMessage(pMsg);
}

void BaseDialogSettingChart::OnLoad()
{
	DoLoad();
}

HBRUSH BaseDialogSettingChart::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editChartSymbolId:
		if(m_symbol.empty() || !SymbolCanChange())//m_historicalChartLoadingToSave)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		else if(strcmp(m_symbolInitial.c_str(), m_symbol.c_str()))
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;
	}
	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}

void BaseDialogSettingChart::Resize(bool widthChanged, bool heightChanged)
{
	if(m_ListBoxChart.m_hWnd)
	{
		CRect rect;
		m_ListBoxChart.GetWindowRect(&rect);
		ScreenToClient(&rect);
		if(!m_bottomOffset)
		{
			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_ListBoxChart.GetClientRect(&clientRect);
			m_listBoxBorderSize2 = rect.Width() - clientRect.Width();
			m_listBoxBorderSize = m_listBoxBorderSize2 / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;

			m_titleOffset = m_leftOffset + m_listBoxBorderSize;
		}
		int bottom = m_clientRect.bottom - m_bottomOffset;
		m_ListBoxChart.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
	}

	BaseDialogSetting::Resize(widthChanged, heightChanged);
	if(heightChanged)InvalidateAllTotalValueRects(listBoxHistChartId);
}

void BaseDialogSettingChart::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
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

const char* BaseDialogSettingChart::GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const
{
	switch(columnId)
	{
		case HCI_VOLUME:
		return U_UnsignedNumberToStrWithCommas(m_totalVolume, buf, buflen);

		case HCI_DATE:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxChart.GetItemCount(), buf, buflen);
	}
	return nullptr;
}

void BaseDialogSettingChart::TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff)
{
/*
	m_totalClosedPnlRect.left += diff;
	m_totalOpenPnlRect.left += diff;
	m_totalOpenPrintPnlRect.left += diff;
	m_totalRangeAtFirstTimerRect.left += diff;

	m_totalClosedPnlRect.right += diff;
	m_totalOpenPnlRect.right += diff;
	m_totalOpenPrintPnlRect.right += diff;
	m_totalRangeAtFirstTimerRect.right += diff;
*/
}

void BaseDialogSettingChart::SetMarketSorterConnected(bool connected)
{
	if(m_marketSorterConnected != connected)
	{
		m_marketSorterConnected = connected;
		if(!m_marketSorterConnected)SetMarketSorterLoaded(false);
//		m_ListBoxStock.SetMarketSorterConnected(m_marketSorterConnected);
//		if(!m_marketSorterConnected)RepaintTotals();
	}
}

void BaseDialogSettingChart::SetMarketSorterLoaded(bool loaded)
{
	if(m_marketSorterLoaded != loaded)
	{
		m_marketSorterLoaded = loaded;
		OnMarketSorterLoaded();
//		m_ListBoxStock.SetMarketSorterLoaded(m_marketSorterLoaded);
	}
}

void BaseDialogSettingChart::FillListBox()
{
	if(m_ListBoxChart.m_hWnd)
	{
		m_ListBoxChart.SetRedraw(FALSE);
		m_ListBoxChart.ResetContent();
		m_totalVolume = 0;
		if(CanFill())
		{
			const Security::ChartPointVector* chartPointVector = GetChartVector();
			if(chartPointVector)
			{
				DoFillListBox(*chartPointVector);
	/*
				const ChartPoint* cp;
				for(Security::ChartPointVector::const_iterator it = chartPointVector->begin(), itend = chartPointVector->end(); it != itend; ++it)
				{
					const ChartPoint& chartPoint = *it;
					if(chartPoint.GetVolume() || !chartPoint.GetStartPrice().isZero())
					{
						cp = new ChartPoint(chartPoint);
						m_ListBoxChart.AddString((const char*)cp);
						m_totalVolume += cp->GetVolume();
					}
				}
	*/
			}
		}
		m_ListBoxChart.SetRedraw(TRUE);
		m_ListBoxChart.Invalidate(FALSE);
		InvalidateAllTotalValueRects(listBoxHistChartId);
	}
}

void BaseDialogSettingChart::DoFillListBox(const Security::ChartPointVector& chartPointVector)
{
	const ChartPoint* cp;
	unsigned int volume;
	for(Security::ChartPointVector::const_iterator it = chartPointVector.cbegin(), itend = chartPointVector.cend(); it != itend; ++it)
	{
		const ChartPoint& chartPoint = *it;
		volume = chartPoint.GetVolume();
		if(volume || !chartPoint.GetStartPrice().isZero())
		{
			cp = new ChartPoint(chartPoint);
			m_ListBoxChart.AddString((const char*)cp);
			m_totalVolume += volume;
		}
	}
}
/*
void BaseDialogSettingChart::AddChartPoint(const ChartPoint& chartPoint)
{
	const ChartPoint* cp = new ChartPoint(chartPoint);
	m_ListBoxChart.AddString((const char*)cp);
}
*/
bool BaseDialogSettingChart::DoReset()
{
	bool ret = false;
	if(m_symbolInitial != m_symbol)
	{
		if(m_symbolInitial.empty() != m_symbol.empty())
		{
			m_EditSymbol.Invalidate(FALSE);
		}
		m_symbol = m_symbolInitial;
		m_EditSymbol.SetWindowText(m_symbol.c_str());
		ret = true;
	}
	return ret;
}

void BaseDialogSettingChart::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_CONNECTED:
		case TM_DISCONNECTED:
		UpdateConnection();
		break;

		case TM_MARKET_SORTER_LOADED://all stocks are initialized
		SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
		break;

		case TM_NEW_EQUITY:
		{
			const Security* security = ((const TMsgSecurity*)message)->m_security;
			if(security->isLoaded())
			{
				NewSecurity(security);
			}
		}
		break;

		case TM_MARKET_SORTER_CLEANUP:
/*
		if(from == &TD_GetAdminObservable())
		{
			m_ListBoxStock.MarketSorterCleanup();
		}
*/
		break;

		case TM_MARKET_STATUS:
		break;

		case TM_NEXT_DAY_STARTED:
		UpdateToday();
		if(ShouldReloadOnNewDay())//m_dayCountMode)
		{
			DoLoad();
		}
		break;
	}
}