#include "stdafx.h"
#include "SampleDialogSettingTestRequest.h"
#include "ExtensionSample.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

class MessageItem
{
public:
	MessageItem(const unsigned char& connectionType, const unsigned short& type, const unsigned short& length, const bool& outgoing):
		m_timestamp(TL_GetCurrentMillisecond()),
		m_type(type),//message->m_type),
		m_length(length),//message->m_length),
		m_connectionType(connectionType),
		m_outgoing(outgoing)
	{
	}
	const unsigned int& GetTimestamp() const{return m_timestamp;}
	const unsigned short& GetType() const{return m_type;}
	const unsigned short& GetLength() const{return m_length;}
	const unsigned char& GetConnectionType() const{return m_connectionType;}
	const bool& isOutgoing() const{return m_outgoing;}
protected:
	unsigned int m_timestamp;
	unsigned short m_type;
	unsigned short m_length;
	unsigned char m_connectionType;
	bool m_outgoing;
};

class RingMessageItem : public MessageItem, public RingVectorBaseItem
{
public:
	RingMessageItem(const unsigned char& connectionType, const unsigned short& type, const unsigned short& length, const bool& outgoing):
		MessageItem(connectionType, type, length, outgoing)
	{
	}
};

class LbMessageHorizontalScroller : public LbHorizontalScroller
{
public:
	LbMessageHorizontalScroller(ListBoxMessage* listBox);
	virtual void AddColumns() override;
protected:
	ListBoxMessage* m_listBoxMessage;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LbMessageHorizontalScroller::LbMessageHorizontalScroller(ListBoxMessage* listBox):
	LbHorizontalScroller(listBox),
	m_listBoxMessage(listBox)
{
}

void LbMessageHorizontalScroller::AddColumns()
{

	const int itemHeight = m_listBox->GetMyItemHeight();
	const int doubleHeight = itemHeight + itemHeight;
	const int tripleHeight = doubleHeight + itemHeight;
	const int fourHeight = tripleHeight + itemHeight;
	const int fiveHeight = fourHeight + itemHeight;
	const int sixHeight = fiveHeight + itemHeight;

	AddColumn("Time", MI_TIMESTAMP, fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Timestamp");
	AddColumn("Type", MI_TYPE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Message Type");
	AddColumn("Length", MI_LENGTH, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Message Length");
	AddColumn("Src", MI_SRC, doubleHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Data Destination or Source: 0 - MD, 1 - EXE, 2 - MS, 3 - HistPr, 4 - Option");
	AddColumn("Out", MI_OUT, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Outgoing Message");
}
/*
int ListBoxMessage::Compare(const void* item1, const void* item2) const
{
	const unsigned int& id1 = ((const MessageItem*)item1)->GetId();
	const unsigned int& id2 = ((const MessageItem*)item2)->GetId();
	return id1 > id2 ? -1 : id1 < id2 ? 1 : 0;
}
*/
void ListBoxMessage::RepaintLineCount() const
{
	m_owner->RepaintLineCount();
}

void ListBoxMessage::DestroyItem(const void* item) const
{
	delete (MessageItem*)item;
}
/*
void ListBoxMessage::ListBoxCreated()
{
	ColumnedListBox::ListBoxCreated();
//	UpdateSymbolColumnRight();

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
}
*/
void ListBoxMessage::DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->itemID)
	{
		case TMID_CLEAR:
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
		break;

		default:
		ColumnedListBox::DrawMenuItem(lpDrawItemStruct);
		break;
	}
}

bool ListBoxMessage::MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	switch(lpMeasureItemStruct->itemID)
	{
		case TMID_CLEAR:
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
		lpMeasureItemStruct->itemWidth = m_owner->GetMainWnd()->GetClearMenuWidth();// m_menuWidth;// mainMessageLoopWnd->GetAddMenuWidth();// + lpMeasureItemStruct->itemHeight;
		break;

		default:
		return ColumnedListBox::MeasureMenuItem(lpMeasureItemStruct);
	}
	return true;
}

void ListBoxMessage::AddMenuItems(CMenu& menu)
{
	bool hasItems = m_itemCount > 0;//GetCount() > 0;
/*
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
*/
//	menu.AppendMenu(MF_SEPARATOR);
//	menu.AppendMenu(MF_STRING, TMID_CLIPBOARD, "Copy");
	menu.AppendMenu(MF_OWNERDRAW|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLEAR, "Clear");
}

BOOL ListBoxMessage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD loword = (DWORD)wParam;
    switch(loword)
    {
		case TMID_CLEAR:
		ClearLines();//ClearItemsWithRedraw();
		break;

		default:
		return CListBox::OnCommand(wParam, lParam);
	}
	return TRUE;
}

LbHorizontalScroller* ListBoxMessage::CreateHorizontalScroller()
{
	return new LbMessageHorizontalScroller(this);
}

void ListBoxMessage::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
	const MessageItem* messageItem = (const MessageItem*)item;
	static const char y = 'Y';
	switch(columnId)
	{
		case MI_TIMESTAMP:
		{
			std::string info;
			U_AppendMillisecond(info, messageItem->GetTimestamp(), true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;


		case MI_TYPE:
		{
			char num[33];
			DrawText(hDC, U_UnsignedNumberToStrNoDecDigits(messageItem->GetType(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case MI_LENGTH:
		{
			char num[33];
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(messageItem->GetLength(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case MI_SRC:
		{
			char num[33];
			DrawText(hDC, U_SignedNumberToStrNoDecDigits(messageItem->GetConnectionType(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case MI_OUT:
		if(messageItem->isOutgoing())
		{
			DrawText(hDC, &y, 1, &r, alignment);
		}
		break;
	}
}

enum SampleSettingTestRequestIds
{
	listBoxMessageId = 10000,
	staticMessageCountId,
	spinMessageCountId,
	checkBoxSuspendedId,
	staticMessageRequestFilterId,
	staticMessageResponseFilterId,
	listBoxMessageRequestFilterId,
	listBoxMessageResponseFilterId,

	messageIdsCount
};

SampleDialogSettingTestRequest::SampleDialogSettingTestRequest(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSetting(mainWnd, parentTab, "WatchMessage"),
	m_SpinMessageCount(999999, 0, 1000, 1000, 3, TakionSpin::charComma),
	m_ListBoxMessage(this),
	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),
	m_leftOffsetResponse(0),
	m_topOffsetResponse(0),
	m_widthResponse(0),
	m_requestFilter(0),
	m_responseFilter(0),
	m_messageCount(0),
	m_messageCountCurrent(0),
	m_suspended(false),
	m_itemsNotInListBox(nullptr),
	m_totalLineCountRect(0, 0, 0, 0)
{
	m_SpinMessageCount.AddSpinCell(1, false);
	m_SpinMessageCount.AddSpinCell(100, false);
	m_SpinMessageCount.AddSpinCell(1000, true);

	AddListBox(&m_ListBoxMessage);

	int w = 80;
	int x = 0;
	int y = 0;//vgap;
	const int xc = x + hgap;
	int width = 44;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, y += vgap, width, spinHeight, spinMessageCountId);
	AddStaticIndirect("MaxCount", WS_VISIBLE | SS_NOPREFIX, 0, xc + width, y + (spinHeight - staticHeight) / 2, w - width, staticHeight, staticMessageCountId);

	AddButtonIndirect("Suspend", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc, y += spinHeight + vgap, width, checkBoxHeight, checkBoxSuspendedId);

	AddStaticIndirect("RequestFilter", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += checkBoxHeight + vgap, w, staticHeight, staticMessageRequestFilterId);
	int lbHeight = 5 * checkBoxHeight + 2 * vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y += staticHeight, w, lbHeight, listBoxMessageRequestFilterId);

	AddStaticIndirect("ResponseFilter", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += lbHeight + vgap, w, staticHeight, staticMessageResponseFilterId);
	lbHeight = 6 * checkBoxHeight + 2 * vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y += staticHeight, w, lbHeight, listBoxMessageResponseFilterId);

	y = staticHeight;
	x += w + hgap + hgap;
	w = m_width - x;
//	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, w, m_height - staticHeight - y, listBoxMessageId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, w, m_height - staticHeight - y, listBoxMessageId);

	TD_SetExtensionRequestFilter(theApp.m_hInstance, extensionConfig->m_watchRequestFilter.GetValue());
	TD_SetExtensionResponseFilter(theApp.m_hInstance, extensionConfig->m_watchResponseFilter.GetValue());
	m_messageCountCurrent = m_messageCount = extensionConfig->m_watchMessageCount.GetValue();
	m_itemsNotInListBox = U_CreateTakionRingVector(m_messageCount, true, true);
	m_suspended = extensionConfig->m_watchSuspended.GetValue();
}

SampleDialogSettingTestRequest::~SampleDialogSettingTestRequest()
{
	TD_SetExtensionRequestFilter(theApp.m_hInstance, 0);
	TD_SetExtensionResponseFilter(theApp.m_hInstance, 0);
	DestroyItemsNotInListBox();
}

void SampleDialogSettingTestRequest::RepaintLineCount() const
{
	if(m_hWnd)
	{
		::InvalidateRect(m_hWnd, &m_totalLineCountRect, FALSE);
	}
}

void SampleDialogSettingTestRequest::UpdateTotalRects()
{
	DoUpdateTotalRects();
}

void SampleDialogSettingTestRequest::DoUpdateTotalRects()
{
	CRect titleRect;
	GetColumnRects(listBoxMessageId, 0, titleRect, m_totalLineCountRect);
	RepaintTotals();
}

void SampleDialogSettingTestRequest::TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff)
{
	m_totalLineCountRect.left += diff;
	m_totalLineCountRect.right += diff;
}

void SampleDialogSettingTestRequest::RepaintTotals() const
{
	::InvalidateRect(m_hWnd, &m_totalLineCountRect, FALSE);
}

void SampleDialogSettingTestRequest::DoPaint(const RECT& rcPaint, CDC& dc)
{
	BaseDialogSetting::DoPaint(rcPaint, dc);

	HGDIOBJ oldFont = dc.SelectObject(GetFont());

	CRect intersectRect;
	std::string info;
	if(intersectRect.IntersectRect(&m_totalLineCountRect, &rcPaint))
	{
		char num[33];
		dc.DrawText(U_UnsignedNumberToStrWithCommas(m_ListBoxMessage.GetLineCount(), num, sizeof(num)), -1, &m_totalLineCountRect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP|DT_NOPREFIX);//|DT_END_ELLIPSIS);
	}
	dc.SelectObject(oldFont);
}

HWND SampleDialogSettingTestRequest::GetFirstTabControl() const
{
	return m_ListBoxMessage.m_hWnd;
}

void SampleDialogSettingTestRequest::Entering()
{
	EnableApplyButton(m_modified);
	BaseDialogSetting::Entering();
}

void SampleDialogSettingTestRequest::Leaving()
{
	EnableApplyButton(true);
	BaseDialogSetting::Leaving();
}

void SampleDialogSettingTestRequest::UpdateSettings()
{
//	BaseDialogSetting::UpdateSettings();
	if(extensionConfig->m_watchRequestFilter.SetValue(m_requestFilter))
	{
		TD_SetExtensionRequestFilter(theApp.m_hInstance, m_requestFilter);
	}
	if(extensionConfig->m_watchResponseFilter.SetValue(m_responseFilter))
	{
		TD_SetExtensionResponseFilter(theApp.m_hInstance, m_responseFilter);
	}
	if(extensionConfig->m_watchMessageCount.SetValue(m_SpinMessageCount.GetValue()))
	{
		SetMessageCount(extensionConfig->m_watchMessageCount.GetValue());
//		if(m_itemsNotInListBox)m_itemsNotInListBox->reserve(m_messageCount);
	}
	extensionConfig->m_watchSuspended.SetValue(m_suspended);

	EnableApplyButton(false);
}

void SampleDialogSettingTestRequest::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
}

void SampleDialogSettingTestRequest::DestroyItemsNotInListBox()
{
	m_itemsNotInListBoxLock.Lock();
	if(m_itemsNotInListBox)
	{
		delete m_itemsNotInListBox;
		m_itemsNotInListBox = nullptr;
	}
	m_itemsNotInListBoxLock.Unlock();
}

void SampleDialogSettingTestRequest::BeforeDestroy()
{
	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;

	m_leftOffsetResponse = 0;
	m_topOffsetResponse = 0;
	m_widthResponse = 0;

	m_requestFilter = 0;
	m_responseFilter = 0;

	if(!m_itemsNotInListBox)
	{
		m_itemsNotInListBox = U_CreateTakionRingVector(m_messageCount, true, true);
	}

	BaseDialogSetting::BeforeDestroy();
}

void SampleDialogSettingTestRequest::UpdateModified()
{
	bool changed = m_requestFilter != extensionConfig->m_watchRequestFilter.GetValue()
		|| m_responseFilter != extensionConfig->m_watchResponseFilter.GetValue()
		|| m_messageCountCurrent != m_messageCount//extensionConfig->m_watchMessageCount.GetValue()
//		|| m_suspended != extensionConfig->m_watchSuspended.GetValue()
		;
	if(m_modified != changed)
	{
		m_modified = changed;
		EnableApplyButton(m_modified);
	}
}
//		m_oneSecondStates(U_CreateTakionRingVector(600, true, true))

void SampleDialogSettingTestRequest::UpdateRequestFilter()
{
	unsigned int mask = GetRequestFilter();
	if(m_requestFilter != mask)
	{
//		unsigned char removeFilter = m_requestFilter & ~mask;
//		unsigned char addFilter = mask & ~m_requestFilter;
		m_requestFilter = mask;
//		TD_SetExtensionRequestFilter(theApp.m_hInstance, m_requestFilter);
//		RemoveOrderStatus(removeFilter);
//		AddOrderByFilter(addFilter, m_orderSideMask);
//		m_modified = true;
		UpdateModified();
	}
}

void SampleDialogSettingTestRequest::UpdateResponseFilter()
{
	unsigned int mask = GetResponseFilter();
	if(m_responseFilter != mask)
	{
//		unsigned char removeFilter = m_requestFilter & ~mask;
//		unsigned char addFilter = mask & ~m_requestFilter;
		m_responseFilter = mask;
//		TD_SetExtensionResponseFilter(theApp.m_hInstance, m_responseFilter);
//		RemoveOrderStatus(removeFilter);
//		AddOrderByFilter(addFilter, m_orderSideMask);
//		m_modified = true;
		UpdateModified();
	}
}

LRESULT SampleDialogSettingTestRequest::OnListBoxChecked(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case listBoxMessageRequestFilterId:
		m_ListBoxRequestFilter.InvalidateClient();
		UpdateRequestFilter();
		break;

		case listBoxMessageResponseFilterId:
		m_ListBoxResponseFilter.InvalidateClient();
		UpdateResponseFilter();
		break;
/*
		default:
		BaseDialogSetting::OnListBoxChecked(w, l);
		break;
*/
	}
	return 0;
}

unsigned int SampleDialogSettingTestRequest::GetRequestFilter() const
{
	return m_ListBoxRequestFilter.GetFilter();
}

unsigned int SampleDialogSettingTestRequest::GetResponseFilter() const
{
	return m_ListBoxResponseFilter.GetFilter();
}

void SampleDialogSettingTestRequest::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
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

void SampleDialogSettingTestRequest::Resize(bool widthChanged, bool heightChanged)
{
	BaseDialogSetting::Resize(widthChanged, heightChanged);
	if(m_ListBoxMessage.m_hWnd)
	{
		if(!m_bottomOffset)
		{
			CRect rect;
			m_ListBoxMessage.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_ListBoxMessage.GetClientRect(&clientRect);
			m_listBoxBorderSize = (rect.Width() - clientRect.Width()) / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;

			m_titleOffset = m_leftOffset + m_listBoxBorderSize;
/*			
			m_EditSymbol.GetWindowRect(&rect);
			m_EditSymbol.GetClientRect(&clientRect);
			int editBoxBorderSize = (rect.Width() - clientRect.Width()) / 2 + LOWORD(m_EditSymbol.GetMargins());

			m_StaticSymbol.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_StaticSymbol.SetWindowPos(nullptr, rect.left + editBoxBorderSize, rect.top, rect.Width() - editBoxBorderSize, rect.Height(), SWP_NOZORDER|SWP_NOACTIVATE);
*/
			m_ListBoxResponseFilter.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffsetResponse = rect.left;
			m_topOffsetResponse = rect.top;
			m_widthResponse = rect.right - m_leftOffsetResponse;
		}
		int bottom = m_clientRect.bottom - m_bottomOffset;
		m_ListBoxMessage.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
		if(heightChanged)
		{
			m_ListBoxResponseFilter.SetWindowPos(nullptr, m_leftOffsetResponse, m_topOffsetResponse, m_widthResponse, m_clientRect.bottom - m_topOffsetResponse, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
			UpdateTotalRects();
		}
/*
		if(heightChanged)
		{
			UpdateTotalRects();//m_leftOffset + m_listBoxBorderSize, bottom);
		}
*/
	}
}

/*
void SampleDialogSettingTestRequest::Leaving()
{
//	TD_GetNewAccountObservable().RemoveObserver(this);
//	TD_GetAdminObservable().RemoveObserver(this);
}
*/

void SampleDialogSettingTestRequest::ToolTipsEnabled(bool enable)
{
	BaseDialogSetting::ToolTipsEnabled(enable);
	m_ListBoxMessage.EnableTooltips(enable, m_toolTip);
}

BOOL SampleDialogSettingTestRequest::OnInitDialog()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;
	m_leftOffsetResponse = 0;
	m_topOffsetResponse = 0;
	m_widthResponse = 0;
	m_totalLineCountRect.left = m_totalLineCountRect.top = m_totalLineCountRect.right = m_totalLineCountRect.bottom = 0;

	BOOL ret = BaseDialogSetting::OnInitDialog();

	SetToolTipsEnabled(true);
//	m_ListBoxMessage.EnableTooltips(true, m_toolTip);//CreateToolTip();

	AddTitle(&m_ListBoxMessage);

//	RestoreLayout();
/*
	UpdateTotalRects();

	if(m_itemsNotInListBoxLock.Lock())
	{
		if(m_itemsNotInListBox)
		{
			RingMessageItem* item;
			TakionIterator* it = U_CreateReverseTakionRingVectorIterator(m_itemsNotInListBox);
			it->Reset();
			while(item = (RingMessageItem*)U_GetNextReverseTakionRingVectorItem(it))
			{
				m_ListBoxMessage.AddLine(new MessageItem(*item));
			}
			delete it;
			DestroyItemsNotInListBox();
		}
		m_itemsNotInListBoxLock.Unlock();
	}
	if(m_ListBoxMessage.GetCount())
	{
		m_ListBoxMessage.SetCurSel(0);
		OnSelchangeMessage();
	}

	RepaintLineCount();
*/
	UpdateTotalRects();
	return ret;
}

void SampleDialogSettingTestRequest::SetupControls()
{
	UpdateTotalRects();

	if(m_itemsNotInListBoxLock.Lock())
	{
		if(m_itemsNotInListBox)
		{
			RingMessageItem* item;
			TakionIterator* it = U_CreateReverseTakionRingVectorIterator(m_itemsNotInListBox);
			it->Reset();
			while(item = (RingMessageItem*)U_GetNextReverseTakionRingVectorItem(it))
			{
				m_ListBoxMessage.AddLine(new MessageItem(*item));
			}
			delete it;
			DestroyItemsNotInListBox();
		}
		m_itemsNotInListBoxLock.Unlock();
	}
	if(m_ListBoxMessage.GetItemCount())
	{
		m_ListBoxMessage.SetCurSel(0);
		OnSelchangeMessage();
	}

	RepaintLineCount();
}

void SampleDialogSettingTestRequest::SetMessageCount(const unsigned int& messageCount)
{
	if(messageCount != m_messageCount)
	{
		m_messageCount = m_messageCountCurrent = messageCount;
		if(m_itemsNotInListBox)m_itemsNotInListBox->reserve(m_messageCount);
		if(m_ListBoxMessage.m_hWnd)
		{
			unsigned int count = m_ListBoxMessage.GetItemCount();
			if(count > m_messageCount)
			{
				m_ListBoxMessage.SetRedraw(FALSE);
				for(--count; count > m_messageCount; --count)
				{
					m_ListBoxMessage.DeleteLine(count);
				}
				m_ListBoxMessage.DeleteLine(count);

				m_ListBoxMessage.SetRedraw(TRUE);
				m_ListBoxMessage.Invalidate(FALSE);
			}
		}
	}
}

void SampleDialogSettingTestRequest::RestoreLayout()
{
	m_ListBoxRequestFilter.SetFilter(extensionConfig->m_watchRequestFilter.GetValue());
	UpdateRequestFilter();
	m_ListBoxResponseFilter.SetFilter(extensionConfig->m_watchResponseFilter.GetValue());
	UpdateResponseFilter();

	TD_SetExtensionRequestFilter(theApp.m_hInstance, m_requestFilter);
	TD_SetExtensionResponseFilter(theApp.m_hInstance, m_responseFilter);

	SetMessageCount(extensionConfig->m_watchMessageCount.GetValue());
	m_SpinMessageCount.SetValue(m_messageCount);

	m_suspended = extensionConfig->m_watchSuspended.GetValue();
	m_CheckBoxSuspended.SetCheck(m_suspended ? BST_CHECKED : BST_UNCHECKED);

	m_modified = false;
//	UpdateModified();
}

void SampleDialogSettingTestRequest::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
	DDX_Control(pDX, spinMessageCountId, m_SpinMessageCount);
	DDX_Control(pDX, checkBoxSuspendedId, m_CheckBoxSuspended);
	DDX_Control(pDX, listBoxMessageRequestFilterId, m_ListBoxRequestFilter);
	DDX_Control(pDX, listBoxMessageResponseFilterId, m_ListBoxResponseFilter);
	DDX_Control(pDX, listBoxMessageId, m_ListBoxMessage);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingTestRequest, BaseDialogSetting)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(checkBoxSuspendedId, OnSuspended)
	ON_LBN_SELCHANGE(listBoxMessageId, OnSelchangeMessage)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)
	ON_MESSAGE(WM_USER + 940, OnRequestSent)
	ON_MESSAGE(WM_USER + 941, OnMessageReceived)
END_MESSAGE_MAP()

void SampleDialogSettingTestRequest::OnSuspended()
{
	const bool suspended = m_CheckBoxSuspended.GetCheck() == BST_CHECKED;
	if(suspended != m_suspended)
	{
		m_suspended = suspended;
		m_CheckBoxSuspended.Invalidate(FALSE);
	}
}

void SampleDialogSettingTestRequest::OnSelchangeMessage()
{
//	int sel = m_ListBoxMessage.GetCurSel();
}

void SampleDialogSettingTestRequest::AddLine(unsigned int wparam, unsigned char connectionType, bool outgoing)
{
	m_ListBoxMessage.SetRedraw(FALSE);

	m_ListBoxMessage.AddLine(new MessageItem(connectionType, (unsigned short)(wparam >> 16), (unsigned short)(wparam & 0xFFFF), outgoing));
//	m_ListBoxMessage.InsertString(0, (const char*)new MessageItem(connectionType, (unsigned short)(wparam >> 16), (unsigned short)(wparam & 0xFFFF), outgoing));
	unsigned int count = m_ListBoxMessage.GetItemCount();
	if(m_messageCount < count)
	{
		for(--count; count > m_messageCount; --count)
		{
//			m_ListBoxMessage.DeleteString(count);
			m_ListBoxMessage.DeleteLine(count);
		}
//		m_ListBoxMessage.DeleteString(count);
		m_ListBoxMessage.DeleteLine(count);
	}
	m_ListBoxMessage.SetCurSel(0);

	m_ListBoxMessage.SetRedraw(TRUE);
	m_ListBoxMessage.Invalidate(FALSE);

	OnSelchangeMessage();
}

LRESULT SampleDialogSettingTestRequest::OnRequestSent(WPARAM w, LPARAM l)
{
	AddLine((unsigned int)w, (unsigned char)l, true);
	return 0;
}

LRESULT SampleDialogSettingTestRequest::OnMessageReceived(WPARAM w, LPARAM l)
{
	AddLine((unsigned int)w, (unsigned char)l, false);
	return 0;
}

LRESULT SampleDialogSettingTestRequest::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinMessageCountId:
		m_messageCountCurrent = m_SpinMessageCount.GetValue();
//		SetMessageCount(m_SpinMessageCount.GetValue());
		UpdateModified();
		break;

		default:
		break;
	}
	return 0;
}

void SampleDialogSettingTestRequest::AddMessageReport(unsigned char connectionType, const Message* message, bool outgoing)
{
	if(!m_suspended)
	{
		if(m_hWnd)PostMessage(outgoing ? WM_USER + 940 : WM_USER + 941, ((unsigned int)message->m_type << 16) | message->m_length, connectionType);
		else
		{
			RingMessageItem* item = new RingMessageItem(connectionType, message->m_type, message->m_length, outgoing);
			if(m_itemsNotInListBoxLock.Lock())
			{
				if(m_itemsNotInListBox)m_itemsNotInListBox->push_back(item);
				m_itemsNotInListBoxLock.Unlock();
			}
			else
			{
				delete item;
			}
		}
	}
}

/*
void SampleDialogSettingTestRequest::OnSelchangeConnection()
{
}
*/

HBRUSH SampleDialogSettingTestRequest::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case checkBoxSuspendedId:
		if(m_suspended)
		{
			pDC->SetBkMode(TRANSPARENT);
//			pDC->SetTextColor(m_takionVisual->GetWarningTextColor());
			pDC->SetTextColor(m_visualBase->GetWarningColor());
			return *m_bkBrush;
		}
		break;

	}
	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}

