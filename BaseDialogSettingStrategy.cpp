#include "stdafx.h"
#include "BaseDialogSettingStrategy.h"
#include "ExtensionSample.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void SecurityObserver::RememberPositionValues()
{
	m_posSizeNew = m_position->GetSizeInv(false);
	m_sharesTradedNew = m_position->GetSharesTraded();
	m_openPnlNew = m_position->GetOpenPnlLevel1();
	m_closedPnlNew = m_position->GetClosedPnl();
}

void SecurityObserver::NullifyPositionValues()
{
	m_posSizeNew = 0;
	m_sharesTradedNew = 0;
	m_openPnlNew.SetZero();
	m_closedPnlNew.SetZero();
}

bool SecurityObserver::DoUpdatePosition()
{
	bool ret = false;
	LbHorizontalScroller* scroller = m_listBoxSymbol->GetHorizontalScroller();
	if(m_sharesTraded != m_sharesTradedNew)
	{
		ret = true;
		if(scroller)
		{
			if(!m_sharesTraded || !m_sharesTradedNew)scroller->InvalidateItemColumn(m_ordinal, SO_CLOSED_PNL);
			scroller->InvalidateItemColumn(m_ordinal, SO_SHARES_TRADED);
		}
		m_listBoxSymbol->UpdateSharesTraded(m_sharesTraded, m_sharesTradedNew);
		m_sharesTraded = m_sharesTradedNew;
	}
	if(m_posSize != m_posSizeNew)
	{
		ret = true;
		m_listBoxSymbol->UpdatePosSize(m_posSize, m_posSizeNew);
		m_posSize = m_posSizeNew;
		if(scroller)scroller->InvalidateItemColumn(m_ordinal, SO_POS_SIZE);
		OnPosSizeChanged();
	}
	if(m_openPnl != m_openPnlNew)
	{
		ret = true;
		m_listBoxSymbol->UpdateOpenPnl(m_openPnl, m_openPnlNew);
		m_openPnl = m_openPnlNew;
		if(scroller)scroller->InvalidateItemColumn(m_ordinal, SO_OPEN_PNL);
	}
	if(m_closedPnl != m_closedPnlNew)
	{
		ret = true;
		m_listBoxSymbol->UpdateClosedPnl(m_closedPnl, m_closedPnlNew);
		m_closedPnl = m_closedPnlNew;
		if(scroller)scroller->InvalidateItemColumn(m_ordinal, SO_CLOSED_PNL);
	}
	return ret;
}

bool SecurityObserver::UpdatePosition()
{
	if(m_position)
	{
		if(m_position->LockInquiry())//have to lock because the position may be being modified in a worker thread
		{
			RememberPositionValues();
			m_position->UnlockInquiry();
			return DoUpdatePosition();
		}
	}
	else
	{
		NullifyPositionValues();
		return DoUpdatePosition();
	}
	return false;
}

bool SecurityObserver::UpdatePositionConstValues()
{
	if(m_position)
	{
		m_position->LockInquiryWait();//have to lock because the position may be being modified in a worker thread
		RememberPositionConstValues();
		m_position->UnlockInquiry();
		return DoUpdatePositionConstValues();
	}
	else
	{
		NullifyPositionConstValues();
		return DoUpdatePositionConstValues();
	}
	return false;
}

void SecurityObserver::RememberSecurityValues()
{
	m_level1BidNew = m_security->GetL1Bid();
	m_level1AskNew = m_security->GetL1Ask();
	m_lastPrintNew = m_security->GetLastNbboPrintPrice();//m_security->GetLastPrice();
}

void SecurityObserver::NullifySecurityValues()
{
	m_level1BidNew = 0;
	m_level1AskNew = 0;
	m_lastPrintNew = 0;
}

bool SecurityObserver::DoUpdateSecurity()
{
	bool ret = false;
	if(m_level1Bid != m_level1BidNew)
	{
		m_level1Bid = m_level1BidNew;
		m_level1BidPrice.SetCompactValue(m_level1Bid);
		ret = true;
	}
	if(m_level1Ask != m_level1AskNew)
	{
		m_level1Ask = m_level1AskNew;
		m_level1AskPrice.SetCompactValue(m_level1Ask);
		ret = true;
	}
	if(m_lastPrint != m_lastPrintNew)
	{
		m_lastPrint = m_lastPrintNew;
		m_lastPrintPrice.SetCompactValue(m_lastPrint);
		ret = true;
	}
	return ret;
}

bool SecurityObserver::UpdateSecurity()//returns true if order status changed. If returns true, then we need to check if the order needs to be removed from the list box because it no longer belongs there.
{
	if(m_security)
	{
		if(m_security->LockInquiryLevel1())//have to lock because the security may be being modified in a worker thread
		{
//Remember the values that can change. Other values don't change and can be safely taken from the order object itself at any time even if the order is being modified in a different thread.
			RememberSecurityValues();
//Unlock as soon as you don't need to access the order data.
			m_security->UnlockInquiryLevel1();
			return DoUpdateSecurity();
		}
//else - LockInquiry failed which means that the security is being updated and a new notification will come soon and the function will be called again.
	}
	else
	{
		NullifySecurityValues();
		return DoUpdateSecurity();
	}
	return false;
}

bool SecurityObserver::UpdateSecurityConstValues()//returns true if order status changed. If returns true, then we need to check if the order needs to be removed from the list box because it no longer belongs there.
{
	if(m_security)
	{
		m_security->LockInquiryWaitLevel1();//have to lock because the security may be being modified in a worker thread
		RememberSecurityConstValues();
//Unlock as soon as you don't need to access the order data.
		m_security->UnlockInquiryLevel1();
		return DoUpdateSecurityConstValues();
//else - LockInquiry failed which means that the security is being updated and a new notification will come soon and the function will be called again.
	}
	else
	{
		NullifySecurityConstValues();
		return DoUpdateSecurityConstValues();
	}
	return false;
}

void SecurityObserver::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_STOCK_LIGHT_SNAPSHOT:

#ifndef TAKION_NO_OPTIONS
		case TM_FUTURE_UPDATE_LEVEL1:
		case TM_OPTION_UPDATE_LEVEL1:
		case TM_FUTURE_ENTITLEMENTS_CHANGED:
		case TM_OPTION_ENTITLEMENTS_CHANGED:
#endif
		case TM_STOCK_IMBALANCE_UPDATE:
		case TM_STOCK_UPDATE_LEVEL1:
		case TM_EQUITY_IMBALANCE_UPDATE:
		case TM_EQUITY_UPDATE:
		case TM_EQUITY_RESET:
		case TM_STOCK_ENTITLEMENTS_CHANGED:
		case TM_SORTABLE_SECURITY_ENTITLEMENTS_CHANGED:
		UpdateSecurity();
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_FUTURE_REFRESHED:
		case TM_OPTION_REFRESHED:
#endif
		case TM_STOCK_REFRESHED:
		UpdateSecurity();
		break;

		case TM_EQUITY_REFRESHED:
		UpdateSecurity();
		break;

		case TM_STOCK_INVALID:
		if(m_security && m_security->GetNumericSymbol() == *(unsigned __int64*)((const TMsgStockInvalid*)message)->m_symbol)
		{
			SetSecurity(nullptr);
			Invalidate();
		}
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_FUTURE_DELETE:
#endif
		case TM_STOCK_DELETE:
		{
			const Security* const& security = ((const TMsgSecurity*)message)->m_security;
			if(security == m_security)
			{
				SetSecurity(nullptr);
			}
		}
		break;

		case TM_UPDATED_POSITION:
		UpdatePosition();
		break;

		case TM_POSITION_DELETE:
		SetPosition(nullptr);
		break;
	}
}

void LbSymbolHorizontalScroller::AddColumns()
{
	m_itemHeight = m_listBox->GetMyItemHeight();
	m_doubleHeight = m_itemHeight + m_itemHeight;
	m_tripleHeight = m_doubleHeight + m_itemHeight;
	m_fourHeight = m_tripleHeight + m_itemHeight;
	m_fiveHeight = m_fourHeight + m_itemHeight;
	m_sixHeight = m_fiveHeight + m_itemHeight;

	AddColumn("Symbol", SO_SYMBOL, m_fiveHeight, DT_LEFT, DT_LEFT, DT_RIGHT, "Symbol");//, 0xFFFFFFFF, itemHeight, itemHeight);
	AddColumn("PosSize", SO_POS_SIZE, m_fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "PosSize");
	AddColumn("OpenP&L", SO_OPEN_PNL, m_fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Open P&L");
	AddColumn("ClosedP&L", SO_CLOSED_PNL, m_fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Closed P&L");
	AddColumn("Traded", SO_SHARES_TRADED, m_fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Shares Traded");
/*
	AddColumn("Side", OCI_SIDE, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Side");
	AddColumn("$Limit", OCI_LIMIT_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Limit Price");
	AddColumn("$Stop", OCI_STOP_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Stop Price");
	AddColumn("$Exec", OCI_EXECUTED_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Executed Price");
	AddColumn("Size", OCI_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Order Size");
	AddColumn("Pending", OCI_PENDING_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Pending Size");
	AddColumn("Executed", OCI_EXECUTED_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Executed Size");
	AddColumn("Visible", OCI_VISIBLE_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Visible Size");
	AddColumn("Status", OCI_STATUS, fourHeight, DT_LEFT, DT_LEFT, 0xFFFFFFFF, "Visible Size", 0xFFFFFFFF, 0xFFFFFFFF, 5, 5, 5);
	AddColumn("Venue", OCI_VENUE, tripleHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Venue");
	AddColumn("Route", OCI_ROUTING, tripleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Routing");
	AddColumn("TIF", OCI_TIF, fourHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Time in Force");
	AddColumn("Created", OCI_TIME_CREATED, fiveHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Time Created");
	AddColumn("CancelSent", OCI_TIME_CANCEL_SENT, fiveHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Cancel Sent");
	AddColumn("Dead", OCI_TIME_DEAD, fiveHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Time Dead");
	AddColumn("ClientId", OCI_CLIENT_ID, tripleHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Client Id");
	AddColumn("ServerId", OCI_SERVER_ID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Server Id");
	AddColumn("ISO", OCI_ISO, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Inter Market Sweep Order");
	AddColumn("ClosedP&L", OCI_CLOSED_PNL, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Closed P&&L");
	AddColumn("BP Used", OCI_BP_USED, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Buying Power used by the Order");
	AddColumn("Rej", OCI_REJECT_CODE, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Reject Code");
	AddColumn("Reject Text", OCI_REJECT_TEXT, sixHeight, DT_LEFT, DT_LEFT, 0xFFFFFFFF, "Reject Text");
*/
}

bool ListBoxSymbol::CanTrade() const
{
	return m_owner->CanTrade();
}

BEGIN_MESSAGE_MAP(ListBoxSymbol, ColumnedListBox)
	ON_MESSAGE(WM_USER + 999, OnStockAdd)
END_MESSAGE_MAP()

SecurityObserver* ListBoxSymbol::AddSymbolAndUpdateOrdinals(const char* symbol, const char* data, bool* added)
{
	bool a;
	SecurityObserver* stock = AddSymbol(U_RepresentStringAsUnsignedNumber<unsigned __int64>(symbol, sizeof(unsigned __int64) - 1), data, &a);
	if(added)*added = a;
	if(stock)
	{
		unsigned int i = stock->GetOrdinal();
		SetCurSel(i);
		if(a)
		{
			IncrementOrdinals(++i);
		}
	}
	return stock;
}

LRESULT ListBoxSymbol::OnStockAdd(WPARAM spinId, LPARAM l)
{
	const std::string& textResult = TGUI_GetTakionMainWnd()->GetSymbolEntryDialog().GetTextResult();
	if(!textResult.empty())
	{
		AddSymbolAndUpdateOrdinals(textResult.c_str(), nullptr, nullptr);
	}
	return 0;
}

SecurityObserver* ListBoxSymbol::CreateSecurityObserver(const unsigned __int64& numericSymbol)
{
	return new SecurityObserver(numericSymbol, this);
}

COLORREF ListBoxSymbol::GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	const SecurityObserver* securityObserver = (const SecurityObserver*)lpDrawItemStruct->itemData;
	const Security* security = securityObserver->GetSecurity();
	if(security)
	{
		return security->isMarketDataStock() ?
			security->isLoaded() ? m_refreshedMdColor : m_mdColor:
			security->isLoaded() ? m_refreshedMsColor : m_msColor;
/*
		if(m_position)
		{
		}
		else
*/
	}
	else
	{
		return m_failedColor;
	}
}

void ListBoxSymbol::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
	const SecurityObserver* securityObserver = (const SecurityObserver*)item;
	static char num[33];
	static std::string info;
	switch(columnId)
	{
		case SO_SYMBOL:
		DrawText(hDC, securityObserver->GetSymbol(), -1, &r, alignment);
		break;

		case SO_POS_SIZE:
		if(securityObserver->GetPosSize())
		{
			DrawText(hDC, U_SignedNumberToStrWithCommas(securityObserver->GetPosSize(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case SO_SHARES_TRADED:
		if(securityObserver->GetSharesTraded())
		{
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(securityObserver->GetSharesTraded(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case SO_OPEN_PNL:
		if(securityObserver->GetPosSize())
		{
			info.clear();
			U_MoneyToStr(info, securityObserver->GetOpenPnl(), 4, 0, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case SO_CLOSED_PNL:
		if(securityObserver->GetSharesTraded())
		{
			info.clear();
			U_MoneyToStr(info, securityObserver->GetClosedPnl(), 4, 0, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;
/*
		default:
		ListBoxSymbol::DrawLine(columnId, item, itemOrdinal, hDC, r, alignment);
		break;
*/
	}
}

COLORREF ListBoxSymbol::GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	if(lpDrawItemStruct->itemState & ODS_SELECTED)return RGB(255, 255, 255);
	return GetSelectedBkColor(lpDrawItemStruct);
}

void ListBoxSymbol::DestroyItem(const void* item) const
{
	delete (SecurityObserver*)item;
}

const char* ListBoxSymbol::GetItemName(const void* item) const
{
	return ((const SecurityObserver*)item)->GetSymbol();
}

void ListBoxSymbol::GetPrefix(std::string& prefix) const
{
	prefix += "ExtStrategy ";
	prefix += m_owner->GetLabel();
}

void ListBoxSymbol::LogCancelError(unsigned char errorCode, const char* prefix) const
{
	if(errorCode != CE_OK)
	{
		std::string errorMessage;
		TU_GetCancelErrorMessage(errorCode, errorMessage);
		std::string extensionPrefix;
		GetPrefix(extensionPrefix);
		extensionPrefix += " Order Cancel ";
		if(prefix)
		{
			extensionPrefix += ' ';
			extensionPrefix += prefix;
		}
		TD_Log(errorMessage.c_str(), extensionPrefix.c_str());
	}
}

void ListBoxSymbol::LogOrderError(unsigned char errorCode, const char* prefix) const
{
	if(errorCode != OE_OK)
	{
		std::string errorMessage;
		TU_GetOrderErrorMessage(errorCode, errorMessage);
		std::string extensionPrefix;
		GetPrefix(extensionPrefix);
		extensionPrefix += " ORDER ";
		if(prefix)
		{
			extensionPrefix += ' ';
			extensionPrefix += prefix;
		}
		TD_Log(errorMessage.c_str(), extensionPrefix.c_str());
	}
}

void ListBoxSymbol::RepaintPosSize() const
{
	m_owner->RepaintPosSize();
}

void ListBoxSymbol::RepaintSharesTraded() const
{
	m_owner->RepaintSharesTraded();
}

void ListBoxSymbol::RepaintOpenPnl() const
{
	m_owner->RepaintOpenPnl();
}

void ListBoxSymbol::RepaintClosedPnl() const
{
	m_owner->RepaintClosedPnl();
}

void ListBoxSymbol::ResetTotals()
{
	m_owner->RepaintCount();
	if(m_posSize)
	{
		m_posSize = 0;
		m_owner->RepaintPosSize();
	}
	if(m_sharesTraded)
	{
		m_sharesTraded = 0;
		m_owner->RepaintSharesTraded();
	}
	if(!m_openPnl.isZero())
	{
		m_openPnl.SetZero();
		m_owner->RepaintOpenPnl();
	}
	if(!m_closedPnl.isZero())
	{
		m_closedPnl.SetZero();
		m_owner->RepaintClosedPnl();
	}
}

void ListBoxSymbol::RepaintCount() const
{
	m_owner->RepaintCount();
}
	
void ListBoxSymbol::RefreshOrdinals()
{
	SecurityObserver* stock;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (SecurityObserver*)GetItemDataPtr(i);
		stock->SetOrdinal(i);
	}
}

void ListBoxSymbol::PasteStocks(StringMap& stringMap, bool replace)
{
	std::string symbol;
	StringMap::const_iterator it;
	StringMap::const_iterator itend = stringMap.end();
	SecurityObserver* stock;
	for(unsigned int i = 0, end = m_itemCount; i < end;)
	{
		stock = (SecurityObserver*)GetItemDataPtr(i);
		symbol = stock->GetSymbol();
		it = stringMap.find(symbol);
		if(it == itend)
		{
			if(replace)
			{
				DeleteString(i);
				--end;
			}
			else
			{
				++i;
			}
		}
		else
		{
			++i;
			stringMap.erase(it);
		}
	}

	for(it = stringMap.begin(); it != itend; ++it)
	{
		const std::string& symbol = it->first;
		if(!symbol.empty())
		{
			stock = AddSymbol(U_RepresentStringAsUnsignedNumber<unsigned __int64>(it->first.c_str(), sizeof(unsigned __int64) - 1), it->second.c_str(), nullptr);//, security);
		}
	}
	RefreshOrdinals();
	RepaintCount();
}

void ListBoxSymbol::ListBoxCreated()
{
	ColumnedListBox::ListBoxCreated();
//	UpdateSymbolColumnRight();
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
}

void ListBoxSymbol::DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
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
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
		break;

		default:
		ColumnedListBox::DrawMenuItem(lpDrawItemStruct);
		break;
	}
}

bool ListBoxSymbol::MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
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
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
		lpMeasureItemStruct->itemWidth = m_menuWidth;// mainMessageLoopWnd->GetAddMenuWidth();// + lpMeasureItemStruct->itemHeight;
		break;

		default:
		return ColumnedListBox::MeasureMenuItem(lpMeasureItemStruct);
	}
	return true;
}

void ListBoxSymbol::AddMenuItems(CMenu& menu)
{
	const bool hasItems = m_itemCount > 0;
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
}

void ListBoxSymbol::CopySymbols(std::string& info) const
{
	bool first = true;
	const SecurityObserver* item;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		item = (const SecurityObserver*)GetItemDataPtr(i);
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

DialogTextEntry& ListBoxSymbol::DisplayStockEntryField()
{
	CFont* font = GetFont();//m_visualBase->GetFont("Arial", -14, true);
	DialogTextEntry& symbolEntryDialog = TGUI_GetTakionMainWnd()->GetSymbolEntryDialog();
	CPoint point;
	point.x = m_owner->GetLeftOffset();
	point.y = m_owner->GetTopOffset();
	m_owner->ClientToScreen(&point);
	symbolEntryDialog.Display(true, true, nullptr, this, 0, this,
		U_GetValidStockChars(), false, true, U_GetValidStockFirstChars(), false, ".",
		RGB(255, 0, 255), 0, 7, font, point.x, point.y, 1, 1);
	return symbolEntryDialog;
}

BOOL ListBoxSymbol::OnCommand(WPARAM wParam, LPARAM lParam)
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
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				SecurityObserver* stock = (SecurityObserver*)GetItemDataPtr(sel);
				RemoveItemAt(sel, stock);
				if(m_itemCount)
				{
					const unsigned int uSel = (unsigned int)sel;
					SetCurSel(uSel < m_itemCount ? uSel : m_itemCount - 1);
				}
			}
		}
		break;

		case TMID_CLEAR:
		ResetContent();
		ResetTotals();
		break;

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
		return CListBox::OnCommand(wParam, lParam);
	}
	return TRUE;
}

void ListBoxSymbol::IncrementOrdinals(unsigned int startFrom)
{
	for(const unsigned int end = m_itemCount; startFrom < end; ++startFrom)
	{
		((SecurityObserver*)GetItemDataPtr(startFrom))->IncrementOrdinal();
	}
}

void ListBoxSymbol::DecrementOrdinals(unsigned int startFrom)
{
	for(const unsigned int end = m_itemCount; startFrom < end; ++startFrom)
	{
		((SecurityObserver*)GetItemDataPtr(startFrom))->DecrementOrdinal();
	}
}

SecurityObserver* ListBoxSymbol::AddSecurity(const Security* security, const char* data, bool updateOrdinals, bool* added)//ordinals are not refreshed
{
	if(m_owner->SecurityBelongs(security))
	{
//		const Position* position = m_account ? m_account->FindPosition(security) : nullptr;
		const Position* position = m_account ? m_account->FindStockPositionWithLock(security->GetNumericSymbol()) : nullptr;
		int found = FindItemByName(security->GetSymbol());
		if(found < 0)
		{
			if(m_owner->isSubscribe())
			{
				security = TD_ObtainStock(security->GetSymbol(), true);
				if(security)
				{
					SecurityObserver* securityObserver = CreateSecurityObserver(security->GetNumericSymbol());
					securityObserver->SetSecurity(security);
					securityObserver->SetPosition(position);
					securityObserver->SetData(data);
					DoAddItem(securityObserver, updateOrdinals);
					if(added)*added = true;
					return securityObserver;
				}
				else
				{
					if(added)*added = false;
				}
			}
			else
			{
				SecurityObserver* securityObserver = CreateSecurityObserver(security->GetNumericSymbol());
				securityObserver->SetSecurity(security);
				securityObserver->SetPosition(position);
				securityObserver->SetData(data);

				DoAddItem(securityObserver, updateOrdinals);
				if(added)*added = true;
				return securityObserver;
			}
		}
		else
		{
			if(added)*added = false;
			SecurityObserver* securityObserver = (SecurityObserver*)GetItemDataPtr(found);
			if(m_owner->isSubscribe())
			{
				const Security* msecurity = securityObserver->GetSecurity();
				if(!msecurity || !msecurity->isMarketDataStock())
				{
					security = TD_ObtainStock(security->GetSymbol(), true);
					securityObserver->SetSecurity(security);
					TD_ReleaseStock(security);
				}
			}
			else
			{
				securityObserver->SetSecurity(security);
			}
			securityObserver->SetPosition(position);
			return securityObserver;
		}
	}
	return nullptr;
}

SecurityObserver* ListBoxSymbol::AddSymbol(const unsigned __int64& numericSymbol, const char* data, bool* added)
{
	const Security* security = TD_FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(numericSymbol);
	SecurityObserver* so = AddSecurityObserver(numericSymbol, security, data, added);

	return so;
}

SecurityObserver* ListBoxSymbol::AddSecurityObserver(const unsigned __int64& numericSymbol, const Security* security, const char* data, bool* added)
{
	if(security)
	{
		return AddSecurity(security, data, false, added);
	}
	else
	{
		SecurityObserver* securityObserver;
		const Position* position = m_account ? m_account->FindStockPositionWithLock(numericSymbol) : nullptr;
		int found = FindItemByName((const char*)&numericSymbol);
		if(found >= 0)
		{
			securityObserver = (SecurityObserver*)GetItemDataPtr(found);
			securityObserver->SetData(data);
			securityObserver->SetSecurity(nullptr);
			securityObserver->SetPosition(position);
			if(added)*added = false;
		}
		else
		{
			securityObserver = CreateSecurityObserver(numericSymbol);
//			securityObserver->SetSecurity(nullptr);
			securityObserver->SetPosition(position);
			securityObserver->SetData(data);
			if(DoAddItem(securityObserver, false) >= 0)
			{
				if(added)*added = true;
			}
			else
			{
				if(added)*added = false;
			}
		}
		return securityObserver;
	}
}

int ListBoxSymbol::DoAddItem(SecurityObserver* item, bool updateOrdinals)
{
	int found = AddString((const char*)item);
	if(found >= 0)
	{
		item->SetOrdinal(found);
		if(updateOrdinals)IncrementOrdinals(found + 1);
		RepaintCount();
		item->Update();
	}
	return found;
}

int ListBoxSymbol::AddItem(SecurityObserver* item)
{
	int found = FindItemByName(item->GetSymbol());
	if(found < 0)
	{
		return DoAddItem(item, true);
	}
	return found;
}

void ListBoxSymbol::DoRemoveItemAt(int found, SecurityObserver* item)
{
/*
	UpdateSizePending(item->GetSizePending(), 0);
	UpdateSizeExecuted(item->GetSizeExecuted(), 0);
	UpdateClosedPnl(item->GetClosedPnl(), Money::moneyZero);//TU_GetMoneyZero());
	UpdateBpUsed(item->GetBpUsed(), Money::moneyZero);//TU_GetMoneyZero());
*/
	item->SetPosition(nullptr);
	item->SetSecurity(nullptr);
	DeleteString(found);
}

void ListBoxSymbol::RemoveItemAt(int found, SecurityObserver* item)
{
/*
	UpdateSizePending(item->GetSizePending(), 0);
	UpdateSizeExecuted(item->GetSizeExecuted(), 0);
	UpdateClosedPnl(item->GetClosedPnl(), Money::moneyZero);//TU_GetMoneyZero());
	UpdateBpUsed(item->GetBpUsed(), Money::moneyZero);//TU_GetMoneyZero());
*/
/*
	item->SetPosition(nullptr);
	item->SetSecurity(nullptr);
	DeleteString(found);
*/
	DoRemoveItemAt(found, item);
	DecrementOrdinals(found);
	RepaintCount();
}

bool ListBoxSymbol::RemoveRowAt(int found)
{
	if(found >= 0 && (unsigned int)found < m_itemCount)
	{
		RemoveItemAt(found, (SecurityObserver*)GetItemDataPtr(found));
		return true;
	}
	return false;
}

bool ListBoxSymbol::RemoveRow(const SecurityObserver* item)
{
	return RemoveRowAt(item->GetOrdinal());
}

bool ListBoxSymbol::RemoveSecurity(const Security* security)
{
	int found = FindItemByName(security->GetSymbol());
	if(found >= 0)
	{
		RemoveItemAt(found, (SecurityObserver*)GetItemDataPtr(found));
		return true;
	}
	return false;
}

void ListBoxSymbol::Clear()
{
	ClearItemsWithRedraw();
	RepaintCount();
}

void ListBoxSymbol::MsCleanup()
{
	SecurityObserver* securityObserver;
	const Security* security;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		securityObserver = (SecurityObserver*)GetItemDataPtr(i);
		security = securityObserver->GetSecurity();
		if(security && !security->isMarketDataStock())
		{
			securityObserver->SetSecurity(nullptr);
		}
	}
}

void ListBoxSymbol::SetAccount(Account* const& account)
{
	if(account != m_account)
	{
		m_account = account;
		m_historyLoaded = m_account && m_account->isHistoryLoaded();

		SecurityObserver* securityObserver;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			securityObserver = (SecurityObserver*)GetItemDataPtr(i);
			securityObserver->SetPosition(m_account ? m_account->FindStockPositionWithLock(securityObserver->GetNumericSymbol()) : nullptr);
		}
	}
}

void BaseDialogSettingStrategy::AccountChanged()
{
	m_listBoxSymbol->SetAccount(m_account);
}

///////
void BaseDialogSettingStrategy::DoReadStocks(bool replace, const char* currentPath)
{
	ListBoxSymbol::StringMap stringMap;
	unsigned int duplicateCount;
	unsigned int emptyCount;

	char delimiter;
	switch(m_ComboBoxSymbolDelimiter.GetCurSel())
	{
		case 0:
		delimiter = ' ';
		break;

		case 1:
		delimiter = '\t';
		break;

		default:
		delimiter = '|';
		break;
	}

	if(U_ReadFileToStringMap(currentPath, stringMap, delimiter, ';', true, &duplicateCount, &emptyCount))
	{
		m_listBoxSymbol->PasteStocks(stringMap, replace);
	}
	else
	{
		TakionMessageBox mb(m_mainWnd);
		mb.AddMessage(currentPath);
		mb.DisplayModal(this, "File does not exist", MB_OK|MB_ICONERROR);//, RGB(0, 0, 128), RGB(255, 255, 255));
	}
}

bool BaseDialogSettingStrategy::ReadStocks(bool replace)
{
//	CString currentPath;
//	m_EditPath.GetWindowText(currentPath);
//	if(!currentPath.IsEmpty())
	if(!m_controlLoadFilePath.empty() && m_loadFromFile)
	{
		if(!m_loadFilePathCurrent)
		{
			m_loadFilePath = m_controlLoadFilePath;
			m_loadFilePathCurrent = true;
			m_EditPath.Invalidate(FALSE);
		}
		DoReadStocks(replace, m_controlLoadFilePath.c_str());
		return true;
	}
	return false;
}

void BaseDialogSettingStrategy::OnViewStocks()
{
	if(!m_controlLoadFilePath.empty())
	{
		const char* error = U_ShellExecute(m_hWnd, m_controlLoadFilePath.c_str());
		if(error)
		{
			TakionMessageBox mb(m_mainWnd);
			mb.AddMessage(m_controlLoadFilePath);
			mb.AddMessage(error);
			mb.DisplayModal(this, "Cannot open file", MB_OK | MB_ICONERROR);//, RGB(0, 0, 128), RGB(255, 255, 255));
		}
	}
}

void BaseDialogSettingStrategy::OnLoadAdd()
{
	if(m_loadFromFile)
	{
		ReadStocks(false);
	}
	else
	{
		m_loadingFromMarket = true;
		TakionIterator* iterator = TD_CreateEquityIterator();
		if(iterator)
		{
			const Security* security;
			TD_LockEquityStorage();
			iterator->Reset();
			while(security = TD_GetNextEquity(iterator))
			{
				m_listBoxSymbol->AddSecurity(security, nullptr, false, nullptr);
			}
			TD_UnlockEquityStorage();
			delete iterator;
		}
		m_listBoxSymbol->RefreshOrdinals();
		RepaintCount();
	}
}

void BaseDialogSettingStrategy::OnLoadReplace()
{
	ReadStocks(true);
}

void BaseDialogSettingStrategy::OnBrowse()
{
	char buf[MAX_PATH];
//	CString currentPath;
//	m_EditPath.GetWindowText(currentPath);

//	const std::string& theApp.GetFilePathAndName()
//	if(currentPath.IsEmpty())
	if(m_controlLoadFilePath.empty())
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
		strcpy_s(buf, sizeof(buf), m_controlLoadFilePath.c_str());
	}

	TakionFileDialog fileDlg(m_mainWnd, true, "txt", buf,
		OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
		"Stock Files (*.txt)|*.txt|All Files (*.*)|*.*||", "Load Stock File", this);
	if(fileDlg.DoModal() == IDOK)
	{
		std::string pathName = fileDlg.GetPathName();
		if(strcmp(pathName.c_str(), m_controlLoadFilePath.c_str()))
		{
			m_EditPath.SetWindowText(pathName.c_str());
//			OnPathChange();
		}
	}
}

BaseDialogSettingStrategy::BaseDialogSettingStrategy(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label,
	ListBoxSymbol* listBoxSymbol, bool subscribe, bool disconnectFromMarketSorter, int controlColumnWidth, int listBoxHOffset)://, const Account* account):
	BaseDialogSettingAccount(mainWnd, parentTab, true, label, false, controlColumnWidth, editHeight + groupOffset + 2 * checkBoxHeight + hgap + 3 * vgap),
	m_ComboBoxSymbolDelimiter(false),
	m_EditSymbol(U_GetValidStockChars(), false),
	m_listBoxSymbol(listBoxSymbol),

	m_editLeftOffset(0),
	m_editTopOffset(0),

	m_topLineY(0),

	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),

	m_historyLoaded(false),
	m_executorConnected(false),
	m_marketSorterConnected(false),
	m_marketSorterLoaded(false),

	m_trading(false),
	m_tradeOnStart(false),

	m_subscribe(subscribe),
	m_disconnectFromMarketSorter(disconnectFromMarketSorter),

	m_layoutLoaded(false),
	m_loadOnStart(false),
	m_loadFromFile(false),
	m_loadingFromMarket(false),
	m_loadFilePathCurrent(true)
{
	AddListBox(m_listBoxSymbol);

	m_ComboBoxSymbolDelimiter.SetTooltipText("Symbol Delimiter in the file that lists stocks");// , nullptr);
	m_ComboBoxSymbolDelimiter.SetInvalid(true);//for coloring purpose only
	m_ComboBoxSymbolDelimiter.SetColorInvalid(m_visualBase->GetGrayColor());

	int w = controlColumnWidth;//52;
	int x = hgap;

	const int w34 = w * 3 / 4;
	const int w58 = w * 5 / 8;
	int w2 = w / 2;

	int y = vgap;
	AddButtonIndirect("Load on Start", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y, w34, editHeight, strategyLoadOnStartCheckBoxId);
	AddButtonIndirect("Load Add", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x += w + hgap, y, w58, editHeight, strategyButtonLoadAddStockId);
	AddButtonIndirect("Load Replace", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x += w58 + hgap, y, w34, editHeight, strategyButtonLoadReplaceStockId);
	AddButtonIndirect("View", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x += w34 + hgap, y, w2, editHeight, strategyButtonViewStocksId);
	AddButtonIndirect("Browse", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x += w2 + hgap, y, w2, editHeight, strategyButtonBrowseId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, x += w2 + hgap, y, w2, 100, strategyComboBoxSymbolDelimiterId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, x += w2 + hgap, y, m_width - x, editHeight, strategyEditPathId);

	m_topLineY = y + editHeight + vgap;

	x = 0;
	const int xc = x + hgap;

	const int loadGroupHeight = groupOffset + 2 * checkBoxHeight + hgap;//30;
	y = m_topLineY;
	AddButtonIndirect("Load From", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, controlColumnWidth, loadGroupHeight, groupLoadFromId);
	AddButtonIndirect("File", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += groupOffset, w34, checkBoxHeight, radioLoadFromFileId);
	AddButtonIndirect("Market", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight, w34, checkBoxHeight, radioLoadFromMarketId);

/////

	m_bottomControl += vgap;

	w = controlColumnWidth - hgap - hgap;
	w2 = w / 2;
	y = m_bottomControl;

	const int positionGroupHeight = groupOffset + editHeight + hgap + editHeight + vgap;//30;
	AddButtonIndirect("Symbol", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, controlColumnWidth, positionGroupHeight, groupStrategySymbolId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xc, y += groupOffset, w - editHeight, editHeight, editStrategySymbolId);
	AddButtonIndirect("->", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc + w - editHeight, y, editHeight, editHeight, buttonSymbolAddId);
	AddButtonIndirect("Find", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc, y += editHeight + vgap, w2, editHeight, buttonSymbolFindId);

	AddButtonIndirect("Trade on Start", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y += editHeight + vgap + vgap, w, checkBoxHeight, strategyTradeOnStartCheckBoxId);

	AddButtonIndirect("Trade", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += checkBoxHeight + vgap, w2 - hgap, editHeight, strategyButtonTradeId);
	AddButtonIndirect("Stop", WS_DISABLED|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x + w2 + hgap, y, w2 - hgap, editHeight, strategyButtonStopId);

	m_bottomControl = y + editHeight;

//	m_topLineY += vgap;
	y = m_topLineY + staticHeight;
//	x += w + hgap + hgap;
//	w = m_width - x;
	w = m_width - listBoxHOffset;
//	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, w, m_height - staticHeight - y, listBoxSecurityId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, listBoxHOffset, y, w, m_height - staticHeight - y, listBoxSecurityId);

	SetExecutorConnected(TD_IsExecutorConnected());
	SetMarketSorterConnected(TD_IsMarketSorterConnected());
	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());

}

HWND BaseDialogSettingStrategy::GetFirstTabControl() const
{
	return m_listBoxSymbol->m_hWnd;
}

void BaseDialogSettingStrategy::BeforeDestroy()
{
	m_editLeftOffset = 0;
	m_editTopOffset = 0;

	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;
	m_controlSymbol.clear();

	m_loadFilePath.clear();
	m_controlLoadFilePath.clear();
	m_loadFilePathCurrent = true;

	m_trading = false;
	m_tradeOnStart = false;

	m_layoutLoaded = false;
	m_loadOnStart = false;
	m_loadFromFile = false;
	m_loadingFromMarket = false;

	TD_GetNewEquityObservable().RemoveObserver(this);
	TD_GetNewPositionObservable().RemoveObserver(this);
	BaseDialogSettingAccount::BeforeDestroy();
}

void BaseDialogSettingStrategy::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingAccount::ToolTipsEnabled(enable);
	m_ComboBoxSymbolDelimiter.EnableTooltips(enable, m_toolTip);
	m_listBoxSymbol->EnableTooltips(enable, m_toolTip);
}

BOOL BaseDialogSettingStrategy::OnInitDialog()
{
//	TD_GetAdminObservable().AddObserver(this);//to get notified about connect / disconnect //Done in BaseDialogSettingAccount
	TD_GetNewEquityObservable().AddObserver(this);
	TD_GetNewPositionObservable().AddObserver(this);

	SetExecutorConnected(TD_IsExecutorConnected());
	SetMarketSorterConnected(TD_IsMarketSorterConnected());
	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());

	BOOL ret = BaseDialogSettingAccount::OnInitDialog();

	m_EditSymbol.SetLimitText(7);
	EnableFindSymbolButton();

	m_ComboBoxSymbolDelimiter.AddString("Space");
	m_ComboBoxSymbolDelimiter.AddString("Tab");
	m_ComboBoxSymbolDelimiter.AddString("Pipe");
	m_ComboBoxSymbolDelimiter.SetColorInvalid(m_visualBase->GetGrayColor());

	if(m_loadOnStart)
	{
		m_CheckBoxLoadOnStart.SetCheck(BST_CHECKED);
	}
	if(m_loadFromFile)m_RadioLoadFromFile.SetCheck(BST_CHECKED);
	else m_RadioLoadFromMarket.SetCheck(BST_CHECKED);

	if(m_tradeOnStart)
	{
		m_CheckBoxTradeOnStart.SetCheck(BST_CHECKED);
//		SetTrading(m_tradeOnStart);
	}
/*
	const bool enableToolTips = true;
	EnableToolTips(enableToolTips);
	m_ComboBoxSymbolDelimiter.EnableTooltips(enableToolTips, m_toolTip);
*/
	SetToolTipsEnabled(true);
//	m_listBoxSymbol->EnableTooltips(enableToolTips, m_toolTip);
	AddTitle(m_listBoxSymbol);

	return ret;
}

void BaseDialogSettingStrategy::RepaintCount() const
{
	InvalidateTotalValueRect(listBoxSecurityId, SO_SYMBOL);
}

void BaseDialogSettingStrategy::RepaintPosSize() const
{
	InvalidateTotalValueRect(listBoxSecurityId, SO_POS_SIZE);
}

void BaseDialogSettingStrategy::RepaintSharesTraded() const
{
	InvalidateTotalValueRect(listBoxSecurityId, SO_SHARES_TRADED);
}

void BaseDialogSettingStrategy::RepaintOpenPnl() const
{
	InvalidateTotalValueRect(listBoxSecurityId, SO_OPEN_PNL);
}

void BaseDialogSettingStrategy::RepaintClosedPnl() const
{
	InvalidateTotalValueRect(listBoxSecurityId, SO_CLOSED_PNL);
}

void BaseDialogSettingStrategy::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingAccount::DoDataExchange(pDX);

	DDX_Control(pDX, strategyLoadOnStartCheckBoxId, m_CheckBoxLoadOnStart);
	DDX_Control(pDX, strategyButtonLoadAddStockId, m_ButtonLoadAdd);
	DDX_Control(pDX, strategyButtonLoadReplaceStockId, m_ButtonLoadReplace);
	DDX_Control(pDX, strategyButtonViewStocksId, m_ButtonViewStocks);
	DDX_Control(pDX, strategyButtonBrowseId, m_ButtonBrowse);
	DDX_Control(pDX, strategyComboBoxSymbolDelimiterId, m_ComboBoxSymbolDelimiter);
	DDX_Control(pDX, strategyEditPathId, m_EditPath);

	DDX_Control(pDX, radioLoadFromFileId, m_RadioLoadFromFile);
	DDX_Control(pDX, radioLoadFromMarketId, m_RadioLoadFromMarket);

	DDX_Control(pDX, strategyTradeOnStartCheckBoxId, m_CheckBoxTradeOnStart);
	DDX_Control(pDX, strategyButtonTradeId, m_ButtonTrade);
	DDX_Control(pDX, strategyButtonStopId, m_ButtonStop);

	DDX_Control(pDX, editStrategySymbolId, m_EditSymbol);
	DDX_Control(pDX, buttonSymbolAddId, m_ButtonSymbolAdd);
	DDX_Control(pDX, buttonSymbolFindId, m_ButtonSymbolFind);

	DDX_Control(pDX, listBoxSecurityId, *m_listBoxSymbol);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingStrategy, BaseDialogSettingAccount)
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(strategyLoadOnStartCheckBoxId, OnLoadOnStart)
	ON_BN_CLICKED(strategyButtonLoadAddStockId, OnLoadAdd)
	ON_BN_CLICKED(strategyButtonLoadReplaceStockId, OnLoadReplace)
	ON_BN_CLICKED(strategyButtonViewStocksId, OnViewStocks)
	ON_BN_CLICKED(strategyButtonBrowseId, OnBrowse)
	ON_EN_CHANGE(strategyEditPathId, OnPathChange)

	ON_BN_CLICKED(radioLoadFromFileId, OnLoadFromFile)
	ON_BN_CLICKED(radioLoadFromMarketId, OnLoadFromMarket)

	ON_BN_CLICKED(strategyTradeOnStartCheckBoxId, OnTradeOnStart)
	ON_BN_CLICKED(strategyButtonTradeId, OnTrade)
	ON_BN_CLICKED(strategyButtonStopId, OnStop)

	ON_EN_CHANGE(editStrategySymbolId, OnSymbolChange)
	ON_EN_UPDATE(editStrategySymbolId, OnSymbolUpdate)
	ON_BN_CLICKED(buttonSymbolAddId, OnAddSymbol)
	ON_BN_CLICKED(buttonSymbolFindId, OnFindSymbol)
	ON_LBN_SELCHANGE(listBoxSecurityId, OnSelchangeSymbol)
	ON_LBN_DBLCLK(listBoxSecurityId, OnDblclkSymbol)

//	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
//	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)
END_MESSAGE_MAP()

void BaseDialogSettingStrategy::EnableLoadButton()
{
	const bool enable = !m_controlLoadFilePath.empty();
	EnableControl(m_ButtonLoadAdd.m_hWnd, !m_loadFromFile || enable, m_EditPath.m_hWnd);
//	EnableControl(m_ButtonLoadReplace.m_hWnd, m_loadFromFile && enable, m_EditPath.m_hWnd);
	EnableControl(m_ButtonLoadReplace.m_hWnd, !m_loadFromFile || enable, m_EditPath.m_hWnd);
}


void BaseDialogSettingStrategy::UpdateSymbolDelimiterColor()
{
	m_ComboBoxSymbolDelimiter.SetColorInvalid(m_controlLoadFilePath.empty() || !m_loadFromFile ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));
}

void BaseDialogSettingStrategy::OnPathChange()
{
	CString text;
	m_EditPath.GetWindowText(text);
	if(strcmp(m_controlLoadFilePath.c_str(), text))
	{
		bool emptyChanged = m_controlLoadFilePath.empty() || text.IsEmpty();

		m_controlLoadFilePath = text;
		const bool current = m_controlLoadFilePath == m_loadFilePath;
		if(current != m_loadFilePathCurrent)
		{
			m_loadFilePathCurrent = current;
			m_EditPath.Invalidate(FALSE);
		}
		m_modified = true;
		const bool enable = !m_controlLoadFilePath.empty();
		EnableLoadButton();
//		EnableControl(m_ButtonLoadReplace.m_hWnd, enable, m_EditPath.m_hWnd);
		EnableControl(m_ButtonViewStocks.m_hWnd, enable, m_EditPath.m_hWnd);
//		m_CheckBoxLoadOnStart.Invalidate(FALSE);

		if(emptyChanged)
		{
			m_EditPath.Invalidate(FALSE);
			UpdateSymbolDelimiterColor();
		}
	}
}

bool BaseDialogSettingStrategy::GetLoadFromFileFromControls() const
{
	return m_RadioLoadFromFile.GetCheck() == BST_CHECKED;
}

void BaseDialogSettingStrategy::OnLoadOnStart()
{
	const bool loadOnStart = m_CheckBoxLoadOnStart.GetCheck() == BST_CHECKED;
	if(loadOnStart != m_loadOnStart)
	{
		m_loadOnStart = loadOnStart;
		m_modified = true;
	}
}

void BaseDialogSettingStrategy::OnTradeOnStart()
{
	const bool tradeOnStart = m_CheckBoxTradeOnStart.GetCheck() == BST_CHECKED;
	if(tradeOnStart != m_tradeOnStart)
	{
		m_tradeOnStart = tradeOnStart;
		m_modified = true;
	}
}

bool BaseDialogSettingStrategy::OnLoadFrom()
{
	if(GetLoadFromFileFromControls() != m_loadFromFile)
	{
		m_loadFromFile = !m_loadFromFile;
		if(m_loadFromFile)
		{
			m_loadingFromMarket = false;
		}
		EnableLoadButton();
		if(!m_controlLoadFilePath.empty())
		{
//			m_EditPath.Invalidate(FALSE);
			UpdateSymbolDelimiterColor();
		}
		m_modified = true;
		return true;
	}
	return false;
}

void BaseDialogSettingStrategy::OnLoadFromFile()
{
	OnLoadFrom();
}

void BaseDialogSettingStrategy::OnLoadFromMarket()
{
	OnLoadFrom();
}

void BaseDialogSettingStrategy::OnFindSymbol()
{
	FindSymbol();
}

void BaseDialogSettingStrategy::OnAddSymbol()
{
	ApplySymbol();
}

void BaseDialogSettingStrategy::FindSymbol()
{
	if(!m_controlSymbol.empty())
	{
		int found = m_listBoxSymbol->FindItemByName(m_controlSymbol.c_str());
		m_listBoxSymbol->SetCurSel(found);
		OnSelchangeSymbol();
	}
}

void BaseDialogSettingStrategy::OnSelchangeSymbol()
{
}

void BaseDialogSettingStrategy::OnDblclkSymbol()
{
	int sel = m_listBoxSymbol->GetCurSel();
	if(sel >= 0)
	{
		const SecurityObserver* securityObserver = (const SecurityObserver*)m_listBoxSymbol->GetItemDataPtr(sel);
		TD_SetSymbolInCommandTool(securityObserver->GetSymbol(), true);//, unsigned int commandToolOrdinal = 0xFFFFFFFF);
	}
}

/*
LRESULT BaseDialogSettingStrategy::OnListBoxChecked(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case listBoxSecurityId:
		m_ListBoxOrderStatus.InvalidateClient();
		UpdateOrderStatusMask();
		break;

		case listBoxOrderSideId:
		m_ListBoxOrderSide.InvalidateClient();
		UpdateOrderSideMask();
		break;
	}
	return 0;
}
*/
void BaseDialogSettingStrategy::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
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

const char* BaseDialogSettingStrategy::GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const
{
	static std::string info;
	switch(columnId)
	{
		case SO_SYMBOL:
		return U_UnsignedNumberToStrWithCommas(m_listBoxSymbol->GetItemCount(), buf, buflen);

		case SO_POS_SIZE:
		return U_UnsignedNumberToStrWithCommas(m_listBoxSymbol->GetPosSize(), buf, buflen);

		case SO_SHARES_TRADED:
		return U_UnsignedNumberToStrWithCommas(m_listBoxSymbol->GetSharesTraded(), buf, buflen);

		case SO_OPEN_PNL:
		info.clear();
		U_MoneyToStr(info, m_listBoxSymbol->GetOpenPnl(), 2);
		return info.c_str();
//		strcpy_s(buf, buflen, info.c_str());
//		return buf;

		case SO_CLOSED_PNL:
		info.clear();
		U_MoneyToStr(info, m_listBoxSymbol->GetClosedPnl(), 2);
		return info.c_str();
//		strcpy_s(buf, buflen, info.c_str());
//		return buf;

		default:
		return nullptr;
	}
}

void BaseDialogSettingStrategy::Resize(bool widthChanged, bool heightChanged)
{
	if(m_listBoxSymbol->m_hWnd)
	{
		if(!m_bottomOffset)
		{
			CRect rect;
			m_listBoxSymbol->GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_listBoxSymbol->GetClientRect(&clientRect);
			m_listBoxBorderSize = (rect.Width() - clientRect.Width()) / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;

			m_EditPath.GetWindowRect(&clientRect);
			ScreenToClient(&clientRect);
			m_editLeftOffset = clientRect.left;
			m_editTopOffset = clientRect.top;

			m_titleOffset = m_leftOffset + m_listBoxBorderSize;
			
			m_EditSymbol.GetWindowRect(&rect);
			m_EditSymbol.GetClientRect(&clientRect);
			int editBoxBorderSize = (rect.Width() - clientRect.Width()) / 2 + LOWORD(m_EditSymbol.GetMargins());
		}
		int bottom = m_clientRect.bottom - m_bottomOffset;
		m_listBoxSymbol->SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
		m_EditPath.SetWindowPos(nullptr, m_editLeftOffset, m_editTopOffset, m_clientRect.right - m_editLeftOffset, m_topOffset - m_editTopOffset - m_editTopOffset - m_bottomOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
	}
	BaseDialogSettingAccount::Resize(widthChanged, heightChanged);
	if(heightChanged)InvalidateAllTotalValueRects(listBoxSecurityId);
}

void BaseDialogSettingStrategy::RepaintTotals() const
{
	InvalidateAllTotalValueRects(listBoxSecurityId);
}

void BaseDialogSettingStrategy::UpdateSettings()
{
	BaseDialogSettingAccount::UpdateSettings();

	BoolSetting* settingLoadOnStart = GetSettingLoadOnStart();
	if(settingLoadOnStart)
	{
		settingLoadOnStart->SetValue(m_loadOnStart);
	}
	BoolSetting* settingLoadFromFile = GetSettingLoadFromFile();
	if(settingLoadFromFile)
	{
		settingLoadFromFile->SetValue(m_loadFromFile);
	}

	UCharSetting* settingSymbolDelimiter = GetSettingSymbolDelimiter();
	if(settingSymbolDelimiter)
	{
		int sel = m_ComboBoxSymbolDelimiter.GetCurSel();
		if(sel < 0)sel = m_ComboBoxSymbolDelimiter.GetItemCount() + 1;
		settingSymbolDelimiter->SetValue((unsigned char)sel);
	}

	StringSetting* settingStockFilePath = GetSettingStockFilePath();
	if(settingStockFilePath)
	{
		settingStockFilePath->SetValue(m_controlLoadFilePath);
	}

	BoolSetting* settingTradeOnStart = GetSettingTradeOnStart();
	if(settingTradeOnStart)
	{
		settingTradeOnStart->SetValue(m_loadOnStart);
	}
}

void BaseDialogSettingStrategy::RestoreLayout()
{
	BaseDialogSettingAccount::RestoreLayout();

	BoolSetting* settingLoadOnStart = GetSettingLoadOnStart();
	const bool loadOnStart = settingLoadOnStart && settingLoadOnStart->GetValue();//extensionConfig->m_msLoadOnStart.GetValue();
	if(loadOnStart != m_loadOnStart)
	{
		m_loadOnStart = loadOnStart;
		m_CheckBoxLoadOnStart.SetCheck(loadOnStart ? BST_CHECKED : BST_UNCHECKED);
	}

	bool loadChanged = !m_layoutLoaded;

	BoolSetting* settingLoadFromFile = GetSettingLoadFromFile();
	const bool loadFromFile = settingLoadFromFile && settingLoadFromFile->GetValue();
	if(loadFromFile != m_loadFromFile)
	{
		loadChanged = true;
		m_loadFromFile = loadFromFile;
		if(loadFromFile)
		{
			m_RadioLoadFromMarket.SetCheck(BST_UNCHECKED);
			m_RadioLoadFromFile.SetCheck(BST_CHECKED);
		}
		else
		{
			m_RadioLoadFromFile.SetCheck(BST_UNCHECKED);
			m_RadioLoadFromMarket.SetCheck(BST_CHECKED);
		}
		EnableLoadButton();
	}
	UCharSetting* settingSymbolDelimiter = GetSettingSymbolDelimiter();
	m_ComboBoxSymbolDelimiter.SetCurSel(settingSymbolDelimiter ? settingSymbolDelimiter->GetValue() : -1);

	StringSetting* settingStockFilePath = GetSettingStockFilePath();
	if(settingStockFilePath)
	{
		const std::string& path = settingStockFilePath->GetValue();
		if(strcmp(m_controlLoadFilePath.c_str(), path.c_str()))
		{
			if(m_loadFromFile)
			{
				loadChanged = true;
			}
			m_EditPath.SetWindowText(path.c_str());
/*
//			OnPathChange();
			if(loadOnStart && !path.empty())
			{
				DoReadStocks(true, path.c_str());
			}
*/
		}
	}
	else if(!m_controlLoadFilePath.empty())
	{
		if(m_loadFromFile)
		{
			loadChanged = true;
		}
		m_EditPath.SetWindowText("");
//		OnPathChange();
/*
		if(loadOnStart && !path.empty())
		{
			DoReadStocks(true, path.c_str());
		}
*/
	}
//	RefreshOrdinals();
	if(m_loadOnStart && loadChanged)
	{
		m_listBoxSymbol->Clear();
//		m_loadingFromMarket = !m_loadFromFile;
		OnLoadAdd();
	}

	BoolSetting* settingTradeOnStart = GetSettingTradeOnStart();
	const bool tradeOnStart = settingTradeOnStart && settingTradeOnStart->GetValue();//extensionConfig->m_msLoadOnStart.GetValue();
	if(tradeOnStart != m_tradeOnStart)
	{
		m_tradeOnStart = tradeOnStart;
		m_CheckBoxTradeOnStart.SetCheck(tradeOnStart ? BST_CHECKED : BST_UNCHECKED);

		SetTrading(tradeOnStart);
	}

	m_layoutLoaded = true;

	EnableLoadButton();

	m_modified = false;
}

void BaseDialogSettingStrategy::StartTrading()
{
}

void BaseDialogSettingStrategy::StopTrading()
{
}

void BaseDialogSettingStrategy::OnTrade()
{
	SetTrading(true);
}

void BaseDialogSettingStrategy::OnStop()
{
	SetTrading(false);
}

void BaseDialogSettingStrategy::SetTrading(const bool trading)
{
	if(trading != m_trading)
	{
		m_trading = trading;
		EnableControl(m_ButtonTrade.m_hWnd, !trading, m_listBoxSymbol->m_hWnd);
		EnableControl(m_ButtonStop.m_hWnd, trading, m_listBoxSymbol->m_hWnd);
		if(trading)StartTrading();
		else StopTrading();
	}
}
/*
LRESULT BaseDialogSettingStrategy::OnSpinChange(WPARAM spinId, LPARAM l)
{
	switch(spinId)
	{
		case spinSecondBackId:
		SetSecondBack(m_SpinSecondBack.GetSecond());
		break;
	}
	return 0;
}

*/
bool BaseDialogSettingStrategy::CanApplySymbol() const
{
	return !m_controlSymbol.empty();
}

void BaseDialogSettingStrategy::EnableFindSymbolButton()
{
	EnableControl(m_ButtonSymbolAdd.m_hWnd, CanApplySymbol(), m_EditSymbol.m_hWnd);
	EnableControl(m_ButtonSymbolFind.m_hWnd, !m_controlSymbol.empty(), m_EditSymbol.m_hWnd);
}

void BaseDialogSettingStrategy::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void BaseDialogSettingStrategy::OnSymbolChange()
{
	CString text;
	m_EditSymbol.GetWindowText(text);
	if(strcmp(m_controlSymbol.c_str(), text))
	{
		m_controlSymbol = text;
//		m_modified = true;
		EnableFindSymbolButton();
	}
}

bool BaseDialogSettingStrategy::ApplySymbol()
{
	m_listBoxSymbol->AddSymbolAndUpdateOrdinals(m_controlSymbol.c_str(), nullptr, nullptr);
	return true;
}

BOOL BaseDialogSettingStrategy::PreTranslateMessage(MSG* pMsg)
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
//				case VK_INSERT:
				if(ApplySymbol())
				{
					return TRUE;
				}
				break;
			}
//		}
	}
	return BaseDialogSettingAccount::PreTranslateMessage(pMsg);
}

HBRUSH BaseDialogSettingStrategy::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case strategyEditPathId:
		if(!m_loadFromFile)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		else if(m_loadFilePathCurrent)
		{
			if(m_controlLoadFilePath.empty())
			{
				pDC->SetBkMode(TRANSPARENT);
				return *m_visualBase->GetGrayBrush();
			}
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;
	}

	return BaseDialogSettingAccount::OnCtlColor(pDC, pWnd, nCtlColor);
}

void BaseDialogSettingStrategy::OnHistoryLoaded()
{
	m_listBoxSymbol->SetHistoryLoaded(m_historyLoaded);
}
/*
void SampleDialogSettingOrder::SetExecutorConnected(bool connected)
{
	if(m_executorConnected != connected)
	{
		m_executorConnected = connected;
		OnExecutorConnected();
	}
}
*/
void BaseDialogSettingStrategy::OnExecutorConnected()
{
	m_listBoxSymbol->SetExecutorConnected(m_executorConnected);

	if(!m_executorConnected)
	{
		RepaintTotals();
	}
}

void BaseDialogSettingStrategy::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
/*
		case M_MS_RESET_IMBALANCES://main thread notification, usually comes at noon to reset opening imbalances and get ready for closing imbalances
		case SM_MS_RESET_IMBALANCES://main thread notification, symbol mapping, usually comes at noon to reset opening imbalances and get ready for closing imbalances
		m_ListBoxImbalance.ResetImbalances();
		break;
*/
		case TM_NEW_EQUITY:
		case TM_REFRESHED_EQUITY:
		if(m_loadingFromMarket)
		{
			const Security* security = ((const TMsgSecurity*)message)->m_security;
			if(security->isLoaded())
			{
				m_listBoxSymbol->AddSecurity(security, nullptr, true, nullptr);
			}
		}
		break;

		case TM_NEW_POSITION:
		{
			const Position* position = ((TMsgNewPosition*)message)->m_position;
			if(position->GetAccount() == m_account)
			{
				int found = m_listBoxSymbol->FindItemByName(position->GetSymbol());
				if(found >= 0)
				{
					((SecurityObserver*)m_listBoxSymbol->GetItemDataPtr(found))->SetPosition(position);
				}
			}
		}
		break;

		case TM_CONNECTED:
		case TM_DISCONNECTED:
		SetMarketSorterConnected(TD_IsMarketSorterConnected());
		BaseDialogSettingAccount::Notify(message, from, info);
		break;

		case TM_MARKET_SORTER_LOADED:
		SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
		if(m_subscribe && m_disconnectFromMarketSorter)
		{
			TD_DisconnectFromMarketSorter();
		}
		break;

		case TM_MARKET_SORTER_CLEANUP:
		if(from == &TD_GetAdminObservable())
		{
//			m_ListBoxImbalance.MarketSorterCleanup();
			m_listBoxSymbol->MsCleanup();
		}
		break;
/*
		case TM_MARKET_STATUS:
		{
		}
		break;

		case TM_NEXT_DAY_STARTED:
		{
//			NextDayStarted(((const TMsgNextDayStarted*)message)->m_date);
		}
		break;
*/
		default:
		BaseDialogSettingAccount::Notify(message, from, info);
		break;
	}
}
