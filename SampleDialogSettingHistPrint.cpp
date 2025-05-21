#include "stdafx.h"
#include "SampleDialogSettingHistPrint.h"
#include "ExtensionSample.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

class LbHistPrintHorizontalScroller : public LbHorizontalScroller
{
public:
	LbHistPrintHorizontalScroller(ListBoxHistPrint* listBox);
	virtual void AddColumns() override;
protected:
	ListBoxHistPrint* m_listBoxHistPrint;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LbHistPrintHorizontalScroller::LbHistPrintHorizontalScroller(ListBoxHistPrint* listBox):
	LbHorizontalScroller(listBox),
	m_listBoxHistPrint(listBox)
{
}

void LbHistPrintHorizontalScroller::AddColumns()
{

	const int itemHeight = m_listBox->GetMyItemHeight();
	const int doubleHeight = itemHeight + itemHeight;
	const int tripleHeight = doubleHeight + itemHeight;
	const int fourHeight = tripleHeight + itemHeight;
	const int fiveHeight = fourHeight + itemHeight;
	const int sixHeight = fiveHeight + itemHeight;

	AddColumn("Symbol", HPCI_SYMBOL, fiveHeight, DT_LEFT, DT_LEFT, DT_RIGHT, "Symbol");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("Time", HPCI_TIME, fiveHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Time");
	AddColumn("Price", HPCI_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Price");
	AddColumn("Size", HPCI_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Size");
	AddColumn("SC1", HPCI_SALE_CONDITION_1, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Sale Condition 1");
	AddColumn("SC2", HPCI_SALE_CONDITION_2, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Sale Condition 2");
	AddColumn("SC3", HPCI_SALE_CONDITION_3, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Sale Condition 3");
	AddColumn("SC4", HPCI_SALE_CONDITION_4, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Sale Condition 4");
	AddColumn("Exch", HPCI_EXCHANGE, fourHeight, DT_LEFT, DT_LEFT, 0xFFFFFFFF, "Exchange");
	AddColumn("Src", HPCI_SOURCE, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Source");
	AddColumn("Ref#", HPCI_REF_NUMBER, sixHeight, DT_LEFT, DT_LEFT, 0xFFFFFFFF, "Reference Number");//, 0xFFFFFFFF, 0xFFFFFFFF, 5, 5, 5);
}


int ListBoxHistPrint::Compare(const void* item1, const void* item2) const
{
	const HistPrintItem* print1 = (const HistPrintItem*)item1;
	const HistPrintItem* print2 = (const HistPrintItem*)item2;
	return print1->GetTime() != print2->GetTime() ? print1->GetTime() < print2->GetTime():
		print1->GetReferenceNumber() < print2->GetReferenceNumber();
}

void ListBoxHistPrint::ResetTotals()
{
	m_owner->RepaintCount();
	if(m_totalSize)
	{
		m_totalSize = 0;
		m_owner->RepaintTotalSize();
	}
	if(!m_totalMoney.isZero())
	{
		m_totalMoney.SetZero();
		m_vwap.SetZero();
		m_owner->RepaintVWAP();
	}
}

void ListBoxHistPrint::RepaintCount() const
{
	m_owner->RepaintCount();
}
	
void ListBoxHistPrint::RepaintTotalSize() const
{
	m_owner->RepaintTotalSize();
}

void ListBoxHistPrint::RepaintVWAP() const
{
	m_owner->RepaintVWAP();
}

const char* ListBoxHistPrint::GetItemName(const void* item) const
{
	return m_owner->GetSymbol().c_str();
}
/*
COLORREF ListBoxHistPrint::GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
}

COLORREF ListBoxHistPrint::GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	if(lpDrawItemStruct->itemState & ODS_SELECTED)return RGB(255, 255, 255);
//	return GetSelectedBkColor(lpDrawItemStruct);
}
*/
/*
void ListBoxHistPrint::DestroyItem(const void* item) const//Destruction is done in ClearAndDestroyPrints
{
//	delete (HistPrintItem*)item;
}
*/
/*
void ListBoxHistPrint::AddMenuItems(CMenu& menu)
{
	bool hasItems = GetCount() > 0;
	int sel = GetCurSel();
	bool selected = sel >= 0;

	if(selected)
	{
		menu.AppendMenu(MF_STRING|(((const OrderObserver*)GetItemDataPtr(sel))->isDead() ? MF_GRAYED : MF_ENABLED), TMID_CANCEL_ORDER, "Cancel");
	}
	else
	{
		menu.AppendMenu(MF_STRING, TMID_CANCEL_ORDER, "Cancel");
	}
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, TMID_CLIPBOARD, "Copy");
}
*/
/*
BOOL ListBoxHistPrint::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD loword = (DWORD)wParam;//LOWORD(wParam);
    switch(loword)
    {
		case TMID_CANCEL_ORDER:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				Order* order = ((OrderObserver*)GetItemDataPtr(sel))->GetOrder();
				order->Cancel();
			}
		}
		break;

		default:
		return CListBox::OnCommand(wParam, lParam);
	}
	return TRUE;

}
*/
void ListBoxHistPrint::SetHistPrintLoaded(const bool loaded)
{
	if(loaded != m_histPrintLoaded)
	{
		m_histPrintLoaded = loaded;
		if(m_hWnd)
		{
			Invalidate(FALSE);
		}
	}
}

bool ListBoxHistPrint::SetHistPrintConnected(const bool connected)
{
	if(connected != m_histPrintConnected)
	{
		m_histPrintConnected = connected;
		if(!connected)
		{
			SetHistPrintLoaded(true);
		}
		if(m_hWnd)
		{
			Invalidate(FALSE);
		}
		return true;
	}
	return false;
}
/*
void ListBoxHistPrint::SizeChanged(bool horizontalChange, bool verticalChange)
{
}
*/
LbHorizontalScroller* ListBoxHistPrint::CreateHorizontalScroller()
{
	return new LbHistPrintHorizontalScroller(this);
}

void ListBoxHistPrint::ClearAndDestroyPrints()
{
	for(HistoricalPrintItemMap::CPair* p = m_historicalPrintItemMapVisible.PGetFirstAssoc(); p; p = m_historicalPrintItemMapVisible.PGetNextAssoc(p))
	{
		delete p->value;
	}
	m_historicalPrintItemMapVisible.RemoveAll();
/*
	m_historicalPrintItemMapVisible.RemoveAll();
	for(HistoricalPrintItemMap::CPair* p = m_historicalPrintItemMap.PGetFirstAssoc(); p; p = m_historicalPrintItemMap.PGetNextAssoc(p))
	{
		delete p->value;
	}
	m_historicalPrintItemMap.RemoveAll();
*/
}

void ListBoxHistPrint::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
/*
	HPCI_SYMBOL,
	HPCI_TIME,
	HPCI_PRICE,
	HPCI_SIZE,
	HPCI_SALE_CONDITION_1,
	HPCI_SALE_CONDITION_2,
	HPCI_SALE_CONDITION_3,
	HPCI_SALE_CONDITION_4,
	HPCI_EXCHANGE,
	HPCI_SOURCE,
	HPCI_REF_NUMBER,
*/
	const HistPrintItem* histPrint = (const HistPrintItem*)item;
	std::string info;
	char num[33];
	static const unsigned int al = DT_VCENTER|DT_NOCLIP|DT_SINGLELINE|DT_NOPREFIX;
	switch(columnId)
	{
		case HPCI_SYMBOL:
		DrawText(hDC, m_owner->GetSymbol().c_str(), -1, &r, alignment);
		break;

		case HPCI_TIME:
		U_FormatMillisecond(histPrint->GetTime(), num, sizeof(num), '\0', true, ':', '-');
		DrawText(hDC, num, -1, &r, alignment);
		break;

		case HPCI_PRICE:
		info.clear();
		U_PriceToStr(info, histPrint->GetPrice(), 4, -1, true, '\0', true);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;

		case HPCI_SIZE:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(histPrint->GetSize(), num, sizeof(num)), -1, &r, alignment);
		break;

		case HPCI_SALE_CONDITION_1:
		if(histPrint->GetSaleCondition1())
		{
			DrawText(hDC, &histPrint->GetSaleCondition1(), 1, &r, alignment);
		}
		break;

		case HPCI_SALE_CONDITION_2:
		if(histPrint->GetSaleCondition2())
		{
			DrawText(hDC, &histPrint->GetSaleCondition2(), 1, &r, alignment);
		}
		break;

		case HPCI_SALE_CONDITION_3:
		if(histPrint->GetSaleCondition3())
		{
			DrawText(hDC, &histPrint->GetSaleCondition3(), 1, &r, alignment);
		}
		break;

		case HPCI_SALE_CONDITION_4:
		if(histPrint->GetSaleCondition4())
		{
			DrawText(hDC, &histPrint->GetSaleCondition4(), 1, &r, alignment);
		}
		break;

		case HPCI_EXCHANGE:
		info.clear();
		U_AppendUnsignedNumberAsString(info, histPrint->GetMmid());
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;

		case HPCI_SOURCE:
		if(histPrint->GetSource())
		{
			DrawText(hDC, (char*)&histPrint->GetSourceChar(), 1, &r, alignment);
		}
		break;

		case HPCI_REF_NUMBER:
		DrawText(hDC, U_UnsignedNumberHexToStr(histPrint->GetReferenceNumber(), num, sizeof(num)), -1, &r, alignment);
		break;
	}
}
/*
void ListBoxHistPrint::HorizontalScrollingDone(int diff)
{
}
*/

enum SampleSettingHistPrintIds
{
	buttonLoadId = baseSymbolIdsCount,
	buttonStopId,
	staticSymbolId,

	checkBoxOddLotId,

	staticSourceId,
	listBoxSourceId,

	listBoxHistPrintId,

	histPrintIdsCount,
};

//////////////////
SampleDialogSettingHistPrint::SampleDialogSettingHistPrint(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab)://, const Account* account):
	BaseSampleDialogSettingSymbol(mainWnd, parentTab, false, "HistPrint", 56),
//	m_position(NULL),
	m_symbolCurrent(true),
//	m_orderStatusMask(0),
//	m_orderSideMask(0),
//	m_EditSymbol(U_GetValidStockChars(), false),
	m_CheckBoxOddLot("Show Odd Lot Prints", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_ListBoxHistPrint(this),
	m_printShowOddLot(false),
	m_filter(0),

	m_requestId(0),
	m_nextRef(0),
	m_nextMinute(0),

	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),

	m_topFilter(0),
	m_widthFilter(0)
{
	AddListBox(&m_ListBoxHistPrintSource);
	AddListBox(&m_ListBoxHistPrint);

//	U_CopyAndPad(m_accountId, sizeof(m_accountId), "", '\0');

	int w = m_accountWidth - hgap - hgap;//52;
	int w2 = w / 2;
	int w2h = w2 - hgap;
	int x = 0;
	int y = m_bottomControl + vgap;
	const int xc = x + hgap;

	AddButtonIndirect("Load", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, xc, y, w2h, editHeight, buttonLoadId);
	AddButtonIndirect("Stop", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, xc + w2 + hgap, y, w2h, editHeight, buttonStopId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += editHeight, w, staticHeight, staticSymbolId);

	AddButtonIndirect("Odd Lot", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, xc, y += staticHeight + vgap, w, checkBoxHeight, checkBoxOddLotId);

	AddStaticIndirect("Source", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += checkBoxHeight + vgap, w, staticHeight, staticSourceId);
	y += staticHeight;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y, w, m_height - y, listBoxSourceId);


	y = staticHeight;
	x += w + hgap + hgap;
	w = m_width - x;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, w, m_height - staticHeight - y, listBoxHistPrintId);

//	SetExecutorConnected(TD_IsExecutorConnected());
//	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
}

HWND SampleDialogSettingHistPrint::GetFirstTabControl() const
{
	return m_ListBoxHistPrint.m_hWnd;
}

bool SampleDialogSettingHistPrint::HistPrintItemBelongs(const HistPrintItem* hp) const
{
	return (m_filter & (1 << hp->GetSource())) && (m_printShowOddLot || hp->GetSaleCondition1() != 'I' && hp->GetSaleCondition4() != 'I');
}

bool SampleDialogSettingHistPrint::HistPrintBelongs(const HistoricalPrint* hp) const
{
	char mcid = hp->GetMarketCenterId();
	if(mcid >= 'a' && mcid <= 'z')mcid -= 0x20;
	if(mcid >= 'A' && mcid <= 'Z')
	{
		unsigned char source = mcid - 'A';
//		return (m_ListBoxHistPrintSource.GetFilter() & (1 << source)) && (m_printShowOddLot || hp->GetSaleCondition1() != 'I' && hp->GetSaleCondition4() != 'I');
		return (m_filter & (1 << source)) && (m_printShowOddLot || hp->GetSaleCondition1() != 'I' && hp->GetSaleCondition4() != 'I');
	}
	return false;
}

void SampleDialogSettingHistPrint::UpdateSettings()
{
	BaseSampleDialogSettingSymbol::UpdateSettings();

	extensionConfig->m_histPrintSymbol.SetValue(m_symbol);

	extensionConfig->m_histPrintOddLot.SetValue(m_printShowOddLot);
	extensionConfig->m_histPrintFilter.SetValue(m_ListBoxHistPrintSource.GetFilter());
}

void SampleDialogSettingHistPrint::BeforeDestroy()
{
	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;

	m_topFilter = 0;
	m_widthFilter = 0;

	m_printShowOddLot = false;
	StopLoadingPrints();
	m_filter = 0;
	m_ListBoxHistPrintSource.SetFilter(0);

//	TD_GetAdminObservable().RemoveObserver(this);
	TD_GetHistoricalPrintObservable().RemoveObserver(this);

	BaseSampleDialogSettingSymbol::BeforeDestroy();
}

void SampleDialogSettingHistPrint::ToolTipsEnabled(bool enable)
{
	BaseSampleDialogSettingSymbol::ToolTipsEnabled(enable);
	m_ListBoxHistPrint.EnableTooltips(enable, m_toolTip);
	m_ListBoxHistPrintSource.EnableTooltips(enable, m_toolTip);
	m_CheckBoxOddLot.EnableTooltips(enable, m_toolTip);
}

BOOL SampleDialogSettingHistPrint::OnInitDialog()
{
//	TD_GetAdminObservable().AddObserver(this);//to get notified about connect / disconnect

	m_ListBoxHistPrint.SetHistPrintConnected(TD_IsHistoricalPrintConnected()
#ifndef TAKION_NO_OPTIONS
		|| TD_IsFutureMarketDataConnected()
#endif
		);

	BOOL ret = BaseSampleDialogSettingSymbol::OnInitDialog();

	m_CheckBoxOddLot.ResizeToFitText(0);

	TakionIterator* iterator = TU_CreateMarketCenterIterator();
	iterator->Reset();
	const MarketCenter* mc;
#if (_MSC_VER > 1600)
	typedef std::unordered_set<unsigned char> UCharSet;
#else
	typedef std::hash_set<unsigned char> UCharSet;
#endif
	UCharSet idSet;
	while(mc = TU_GetNextMarketCenter(iterator))
	{
		if(idSet.insert(mc->GetId()).second)
		{
			m_ListBoxHistPrintSource.AddStringItem(mc->GetLongName(), mc->GetId());
		}
//		++i;
	}
	delete iterator;

	EnableLoadButton();

	SetToolTipsEnabled(true);
//	m_ListBoxHistPrint.EnableTooltips(true, m_toolTip);
	AddTitle(&m_ListBoxHistPrint);

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingHistPrint::RepaintCount() const
{
	InvalidateTotalValueRect(listBoxHistPrintId, HPCI_SYMBOL);
}
	
void SampleDialogSettingHistPrint::RepaintTotalSize() const
{
	InvalidateTotalValueRect(listBoxHistPrintId, HPCI_SIZE);
}

void SampleDialogSettingHistPrint::RepaintVWAP() const
{
	InvalidateTotalValueRect(listBoxHistPrintId, HPCI_PRICE);
}

void SampleDialogSettingHistPrint::DoDataExchange(CDataExchange* pDX)
{
	BaseSampleDialogSettingSymbol::DoDataExchange(pDX);
	DDX_Control(pDX, buttonLoadId, m_ButtonLoad);
	DDX_Control(pDX, buttonStopId, m_ButtonStop);
	DDX_Control(pDX, staticSymbolId, m_StaticSymbol);

	DDX_Control(pDX, checkBoxOddLotId, m_CheckBoxOddLot);
	DDX_Control(pDX, listBoxSourceId, m_ListBoxHistPrintSource);

	DDX_Control(pDX, listBoxHistPrintId, m_ListBoxHistPrint);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingHistPrint, BaseSampleDialogSettingSymbol)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(buttonLoadId, OnLoad)
	ON_BN_CLICKED(buttonStopId, OnStop)
	ON_EN_CHANGE(editSymbolId, OnSymbolChange)
//	ON_LBN_SELCHANGE(listBoxHistPrintId, OnSelchangeHistPrint)

	ON_BN_CLICKED(checkBoxOddLotId, OnOddLot)

	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)
END_MESSAGE_MAP()

void SampleDialogSettingHistPrint::OnOddLot()
{
	const bool showOddLot = m_CheckBoxOddLot.GetCheck() == BST_CHECKED;
	if(showOddLot != m_printShowOddLot)
	{
		m_printShowOddLot = showOddLot;
		UpdatePrints(!m_printShowOddLot, m_printShowOddLot);
		m_modified = true;
	}
}

void SampleDialogSettingHistPrint::RemoveNotBelongingPrints()
{
	const HistPrintItem* item;
	for(unsigned int i = 0, end = m_ListBoxHistPrint.GetItemCount(); i < end; )
	{
		item = (const HistPrintItem*)m_ListBoxHistPrint.GetItemDataPtr(i);
		if(HistPrintItemBelongs(item))
		{
			++i;
		}
		else
		{
			m_ListBoxHistPrint.DoRemoveItem(i, item->GetReferenceNumber());
			--end;
		}
	}
}

void SampleDialogSettingHistPrint::UpdatePrints(const bool remove, const bool add)
{
	if(remove)RemoveNotBelongingPrints();
	if(add)PopulatePrints();//Reload(m_symbol.c_str(), true);
}

bool SampleDialogSettingHistPrint::UpdateFilter()
{
	const unsigned int filter = m_ListBoxHistPrintSource.GetFilter();
	if(filter != m_filter)
	{
		const unsigned int inter = m_filter & filter;
		const bool remove = inter != m_filter;
		const bool add = inter != filter;
		m_filter = filter;
		UpdatePrints(remove, add);
		return true;
	}
	return false;
}

LRESULT SampleDialogSettingHistPrint::OnListBoxChecked(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case listBoxSourceId:
//		m_ListBoxHistPrintSource.InvalidateClient();
		if(UpdateFilter())
		{
			m_modified = true;
		}
		break;
	}
	return 0;
}

void SampleDialogSettingHistPrint::ClearHistPrints()
{
	m_ListBoxHistPrint.ClearItemsWithRedraw();
	RepaintCount();
}

void SampleDialogSettingHistPrint::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
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

const char* SampleDialogSettingHistPrint::GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const
{
	std::string info;
	switch(columnId)
	{
		case HPCI_SYMBOL:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxHistPrint.GetItemCount(), buf, buflen);

		case HPCI_SIZE:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxHistPrint.GetTotalSize(), buf, buflen);

		case HPCI_PRICE:
		U_MoneyToStr(info, m_ListBoxHistPrint.GetVWAP(), 4);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		default:
		return nullptr;
	}
}

void SampleDialogSettingHistPrint::Resize(bool widthChanged, bool heightChanged)
{
	if(m_ListBoxHistPrint.m_hWnd)
	{
		if(!m_bottomOffset)
		{
			CRect rect;
			m_ListBoxHistPrint.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_ListBoxHistPrint.GetClientRect(&clientRect);
			m_listBoxBorderSize = (rect.Width() - clientRect.Width()) / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;
			m_titleOffset = m_leftOffset + m_listBoxBorderSize;
			
			m_EditSymbol.GetWindowRect(&rect);
			m_EditSymbol.GetClientRect(&clientRect);
			int editBoxBorderSize = (rect.Width() - clientRect.Width()) / 2 + LOWORD(m_EditSymbol.GetMargins());

			m_StaticSymbol.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_StaticSymbol.SetWindowPos(nullptr, rect.left + editBoxBorderSize, rect.top, rect.Width() - editBoxBorderSize, rect.Height(), SWP_NOZORDER|SWP_NOACTIVATE);

			m_ListBoxHistPrint.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_ListBoxHistPrintSource.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_topFilter = rect.top;
			m_widthFilter = rect.right - rect.left;
		}
		int bottom = m_clientRect.bottom - m_bottomOffset;
		m_ListBoxHistPrint.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);

		if(heightChanged)
		{
			m_ListBoxHistPrintSource.SetWindowPos(nullptr, 0, 0, m_widthFilter, m_clientRect.bottom - m_topFilter, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
//			UpdateTotalRects();//m_leftOffset + m_listBoxBorderSize, bottom);
		}
	}
	BaseSampleDialogSettingSymbol::Resize(widthChanged, heightChanged);
	if(heightChanged)InvalidateAllTotalValueRects(listBoxHistPrintId);
}

void SampleDialogSettingHistPrint::RepaintTotals() const
{
	InvalidateAllTotalValueRects(listBoxHistPrintId);
}
/*
StringSetting* SampleDialogSettingOrder::GetAccountIdSetting()//{return m_orderAccountId;}
{
	return &extensionConfig->m_orderAccountId;
}
*/
void SampleDialogSettingHistPrint::RestoreLayout()
{
	m_ListBoxHistPrint.ClearItemsWithRedraw();
	BaseSampleDialogSettingSymbol::RestoreLayout();
	m_EditSymbol.SetWindowText(extensionConfig->m_histPrintSymbol.GetValue().c_str());

	m_CheckBoxOddLot.SetCheck(extensionConfig->m_histPrintOddLot.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnOddLot();

	m_ListBoxHistPrintSource.SetFilter(extensionConfig->m_histPrintFilter.GetValue());

	UpdateFilter();

	ApplySymbol();

	m_modified = false;
}

void SampleDialogSettingHistPrint::EnableLoadButton()
{
	EnableControl(m_ButtonLoad.m_hWnd, CanApplySymbol(), m_EditSymbol.m_hWnd);
}

void SampleDialogSettingHistPrint::EnableStopButton()
{
	EnableControl(m_ButtonStop.m_hWnd, m_requestId != 0, m_EditSymbol.m_hWnd);
}

bool SampleDialogSettingHistPrint::AddPrint(const unsigned __int64& referenceNumber,
	const Price& price,
	unsigned int size,
	unsigned int millisecond,
	unsigned char source,
	char saleCondition1,
	char saleCondition2,
	char saleCondition3,
	char saleCondition4)
//	bool newPrint)
{
	if(!m_ListBoxHistPrint.FindItemByRefNumber(referenceNumber))
	{
		const MarketCenter* marketCenter = TU_GetMarketCenter(source);
		unsigned int mmid = marketCenter ? marketCenter->GetNumericLongName() : 0;
		HistPrintItem *item = new HistPrintItem(referenceNumber,
			price,
			size,
			millisecond,
			mmid,
			source,
			saleCondition1,
			saleCondition2,
			saleCondition3,
			saleCondition4);
		m_ListBoxHistPrint.DoAddItem(item);
		return true;
	}
/*
#ifdef _DEBUG
	else
	{
		char num[33];
		std::string info(m_symbol);
		info += ' ';
		info += U_UnsignedNumberHexToStr(referenceNumber, num, sizeof(num));
		TD_Log(info.c_str(), "ERROR Hist Print duplicate Ref#");
	}
#endif
*/
	return false;
}

void SampleDialogSettingHistPrint::StopLoadingPrints()
{
	if(m_requestId)
	{
		TD_StopHistoricalPrints(m_requestId);
		m_requestId = 0;
		EnableStopButton();
		EnableLoadButton();
		if(m_hWnd)
		{
			m_ListBoxHistPrint.Invalidate(FALSE);
		}
	}
}

void SampleDialogSettingHistPrint::PopulatePrints()
{
	const Security* security = TD_FindSortableEquityWithNoIncrementUnsubscribe(m_symbol.c_str());
	if(security)
	{
		PopulatePrints(security);
	}
}

void SampleDialogSettingHistPrint::PopulatePrints(const Security* security)
{
	const Security::HistoricalPrintMap* printList = security->GetHistoricalPrintMap();
//					const Security::TimeHistoricalPrintMap* printList = security->GetTimeHistoricalPrintMap();
	if(printList)
	{
		char mcid;
		unsigned char source;
		m_ListBoxHistPrint.SetRedraw(FALSE);
		security->LockInquiryWaitHistPrints();
//						for(Security::TimeHistoricalPrintMap::const_iterator it = printList->begin(), itend = printList->end(); it != itend; ++it)//, ++ref)
		for(Security::HistoricalPrintMap::const_iterator it = printList->begin(), itend = printList->end(); it != itend; ++it)//, ++ref)
		{
			const HistoricalPrint& hp = it->second->second;
			mcid = hp.GetMarketCenterId();
			if(mcid >= 'a' && mcid <= 'z')mcid -= 0x20;
			if(mcid >= 'A' && mcid <= 'Z')
			{
				source = mcid - 'A';
				if((m_filter & (1 << source)) && (m_printShowOddLot || hp.GetSaleCondition1() != 'I' && hp.GetSaleCondition4() != 'I'))// && !FindRowByRefNumber(it->first))
				{

//									if(
					AddPrint(it->first,//refNumber
						Price(hp.GetCompactPrice()),
						hp.GetSize(),//unsigned int printSize,
						it->second->first,//millisecond
						source,
						hp.GetSaleCondition1(),
						hp.GetSaleCondition2(),
						hp.GetSaleCondition3(),
						hp.GetSaleCondition4());
//						false);
/*
					{
						changed = true;
					}
*/
				}
			}
		}
		security->UnlockInquiryHistPrints();
		m_ListBoxHistPrint.SetRedraw(TRUE);
		m_ListBoxHistPrint.Invalidate(FALSE);
	}
}

void SampleDialogSettingHistPrint::Reload(const char* symbol, bool changed)
{
/*
	if(changed)
	{
		StopLoadingPrints();
		m_nextMinute = 0;
		m_nextRef = 0;
	}
*/
	if(!m_requestId)
	{
		const Security* security = TD_FindSortableEquity(symbol);
		if(security)
		{
			if(changed)
			{
				PopulatePrints(security);
			}
			if(m_ListBoxHistPrint.isHistPrintConnected())
			{
				security->LoadHistoricalPrints(m_requestId);
			}
			TD_ReleaseStock(security);
		}
		else if(m_ListBoxHistPrint.isHistPrintConnected())
		{
			TD_RequestHistoricalPrints(symbol, m_nextMinute, m_nextRef, m_nextRef ? ExcludeReferencedPrint : 0, m_requestId);
		}
		if(m_requestId)
		{
			TD_GetHistoricalPrintObservable().AddObserver(this);
			EnableStopButton();
			EnableLoadButton();
			m_ListBoxHistPrint.SetHistPrintLoaded(false);
/*
			if(m_ListBoxHistPrint.m_hWnd)
			{
				m_ListBoxHistPrint.Invalidate(FALSE);
			}
*/
		}
	}
/*
	}
	else
	{
		if(m_requestId)
		{
			m_nextMinute = 0;
			m_nextRef = 0;
			m_requestId = 0;
		}
		m_ListBoxHistPrint.SetHistPrintLoaded(true);
	}
*/
}

void SampleDialogSettingHistPrint::OnSymbolChange()
{
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_controlSymbol = text;
	if(m_controlSymbol == m_symbol)
	{
		if(!m_symbolCurrent)
		{
			m_symbolCurrent = true;
			m_EditSymbol.Invalidate(FALSE);
			EnableLoadButton();
		}
	}
	else
	{
		if(m_symbolCurrent)
		{
			m_symbolCurrent = false;
			m_EditSymbol.Invalidate(FALSE);
			EnableLoadButton();
		}
	}
}

BOOL SampleDialogSettingHistPrint::PreTranslateMessage(MSG* pMsg)
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
				if(ApplySymbol())
				{
					return TRUE;
				}
				break;
			}
//		}
	}
	return BaseSampleDialogSettingSymbol::PreTranslateMessage(pMsg);
}

void SampleDialogSettingHistPrint::OnLoad()
{
	ApplySymbol();
}

void SampleDialogSettingHistPrint::OnStop()
{
	StopLoadingPrints();
	EnableStopButton();
	EnableLoadButton();
}

bool SampleDialogSettingHistPrint::CanApplySymbol() const
{
	return !m_requestId && (!m_symbolCurrent || !m_symbol.empty());
}

bool SampleDialogSettingHistPrint::ApplySymbol()
{
	if(CanApplySymbol())
	{
		bool changed = !m_symbolCurrent;
		if(changed)
		{
			m_symbol = m_controlSymbol;
			m_symbolCurrent = true;
			m_ListBoxHistPrint.ClearItemsWithRedraw();
		}
		if(!m_symbol.empty())
		{
			Reload(m_symbol.c_str(), changed);
		}
/*
		bool wasEmpty = m_symbol.empty();
		m_symbol = m_controlSymbol;
		bool becameEmpty = m_symbol.empty();
		m_symbolCurrent = true;
		SetPosition(becameEmpty ? nullptr : FindPosition());
		if(m_account)
		{
			if(becameEmpty)
			{
				m_account->AddObserver(this);
			}
			else if(wasEmpty)
			{
				m_account->RemoveObserver(this);
			}
		}
		if(becameEmpty)//whole account
		{
			AddOrderByFilter(m_orderStatusMask, m_orderSideMask);
		}
		else if(wasEmpty)
		{
			RemoveOrdersButSymbol();
		}
		else
		{
			ClearOrders();
			AddOrderByFilter(m_orderStatusMask, m_orderSideMask);
		}
*/
		m_EditSymbol.Invalidate(FALSE);
		m_StaticSymbol.SetWindowText(m_symbol.c_str());
		EnableLoadButton();
//		m_StaticSymbol.Invalidate(FALSE);
		m_modified = true;
		return true;
	}
	return false;
}

HBRUSH SampleDialogSettingHistPrint::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSymbolId:
		if(!m_symbolCurrent)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;

		case staticSymbolId:
		if(!m_symbol.empty() && !m_ListBoxHistPrint.isHistPrintConnected())
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetWarningTextColor());
			return *m_bkBrush;
		}
		break;
	}
	return BaseSampleDialogSettingSymbol::OnCtlColor(pDC, pWnd, nCtlColor);
}
/*
void SampleDialogSettingOrder::OnHistoryLoaded()
{
	m_ListBoxOrder.SetHistoryLoaded(m_historyLoaded);
}

void SampleDialogSettingOrder::OnExecutorConnected()
{
	m_ListBoxOrder.SetExecutorConnected(m_executorConnected);

	if(!m_executorConnected)
	{
		RepaintTotals();
	}
}
*/
void SampleDialogSettingHistPrint::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case M_RESP_PRINTS:
		{
			const TMsgPrints* msg = (const TMsgPrints*)message;
			if(msg->m_requestId == m_requestId)
			{
				unsigned short length = msg->m_length;
				unsigned int lastTime = 0;
				if(length > sizeof(TMsgPrints))
				{
					length -= sizeof(TMsgPrints);
					const char* cursor = (const char*)msg + sizeof(TMsgPrints);
					const PrintData* data;
					unsigned char tickSize = msg->m_tickSize;
					char mcid;
					unsigned char source;
					m_ListBoxHistPrint.SetRedraw(FALSE);
					for(; length >= tickSize; length -= tickSize, cursor += tickSize)
					{
						data = (const PrintData*)cursor;
						mcid = data->m_marketCenterID;
						if(mcid >= 'a' && mcid <= 'z')mcid -= 0x20;
						if(mcid >= 'A' && mcid <= 'Z')
						{
							source = mcid - 'A';
							if((m_filter & (1 << source)) && (m_printShowOddLot || data->m_saleCondition1 != 'I' && data->m_saleCondition4 != 'I'))
							{
								m_nextRef = data->m_referenceNumber;
								lastTime = data->m_time;
								AddPrint(m_nextRef,
									Price(data->m_priceDollars, data->m_priceFraction),
									data->m_size,
									lastTime,
									source,
									data->m_saleCondition1,
									data->m_saleCondition2,
									data->m_saleCondition3,
									data->m_saleCondition4);
//									true);
							}
						}
					}
					m_ListBoxHistPrint.SetRedraw(TRUE);
					m_ListBoxHistPrint.Invalidate(FALSE);
					if(lastTime)
					{
						m_nextMinute = (unsigned short)(lastTime / 60000);
					}
				}
				if(!(msg->m_flags & More))
				{
					m_ListBoxHistPrint.SetHistPrintLoaded(true);
					TD_GetHistoricalPrintObservable().RemoveObserver(this);
					m_requestId = 0;
					EnableStopButton();
					EnableLoadButton();
/*
					if(m_hWnd)
					{
						m_ListBoxHistPrint.Invalidate(FALSE);
					}
*/
				}
			}
		}
		break;
#ifndef TAKION_NO_OPTIONS
		case M_RESP_FUT_PRINTS:
		{
			const TMsgFutPrints* msg = (const TMsgFutPrints*)message;
			if(msg->m_requestId == m_requestId)
			{
				unsigned short length = msg->m_length;
				unsigned int lastTime = 0;
				if(length > sizeof(TMsgFutPrints))
				{
					length -= sizeof(TMsgFutPrints);
					const char* cursor = (const char*)msg + sizeof(TMsgFutPrints);
					const FuturePrintData* data;
					unsigned char tickSize = msg->m_tickSize;
					char mcid;
					unsigned char source;
					m_ListBoxHistPrint.SetRedraw(FALSE);
					for(; length >= tickSize; length -= tickSize, cursor += tickSize)
					{
						data = (const FuturePrintData*)cursor;
						mcid = data->m_marketCenterID;
						if(mcid >= 'a' && mcid <= 'z')mcid -= 0x20;
						if(mcid >= 'A' && mcid <= 'Z')
						{
							source = mcid - 'A';
							if((m_filter & (1 << source)))// && (m_printShowOddLot || data->m_saleCondition1 != 'I' && data->m_saleCondition4 != 'I'))
							{
								m_nextRef = data->m_referenceNumber;
								lastTime = data->m_time;
								AddPrint(m_nextRef,
									Price(data->m_priceDollars, data->m_priceFraction),
									data->m_size,
									lastTime,
									source,
									data->m_tradeCondition,
									0,//data->m_saleCondition2,
									0,//data->m_saleCondition3,
									0);// data->m_saleCondition4);
//									true);
							}
						}
					}
					m_ListBoxHistPrint.SetRedraw(TRUE);
					m_ListBoxHistPrint.Invalidate(FALSE);
					if(lastTime)
					{
						m_nextMinute = (unsigned short)(lastTime / 60000);
					}
				}
				if(!(msg->m_flags & More))
				{
					m_ListBoxHistPrint.SetHistPrintLoaded(true);
					TD_GetHistoricalPrintObservable().RemoveObserver(this);
					m_requestId = 0;
					EnableStopButton();
					EnableLoadButton();
				}
			}
		}
		break;
#endif
		case TM_MARKET_SORTER_CLEANUP:
//		MarketSorterCleanup();
		break;

		case TM_CONNECTED:
		case TM_DISCONNECTED:
		if(info && info->m_type == TM_CONNECTION_OBJECT)
		{
			const TMsgConnectionObject* msg = (const TMsgConnectionObject*)info;
			Connection* connection = msg->m_connection;
			if(connection == TD_GetHistoricalPrintConnection())
			{
				const bool connected = TD_IsHistoricalPrintConnected()
#ifndef TAKION_NO_OPTIONS
					|| TD_IsFutureMarketDataConnected()
#endif
					;
				if(m_ListBoxHistPrint.SetHistPrintConnected(connected))
				{
					m_StaticSymbol.Invalidate(FALSE);
					if(connected)
					{
						ReloadCurrent();
					}
				}
			}
#ifndef TAKION_NO_OPTIONS
			else if(connection == TD_GetFutureMarketDataConnection())
			{
				const bool connected = TD_IsFutureMarketDataConnected() || TD_GetHistoricalPrintConnection();
				if(m_ListBoxHistPrint.SetHistPrintConnected(connected))
				{
					m_StaticSymbol.Invalidate(FALSE);
					if(connected)
					{
						ReloadCurrent();
					}
				}
			}
#endif
		}
//		break;
//fall through

		default:
		BaseSampleDialogSettingSymbol::Notify(message, from, info);
		break;
	}
}
