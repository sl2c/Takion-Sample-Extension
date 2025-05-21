#include "stdafx.h"
#include "SampleDialogSettingWindow.h"
#include "ExtensionDerivedConfig.h"
#include "StockComparator.h"
#include "MyMarketSorter.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int ListBoxExtensionWindow::Compare(const void* item1, const void* item2) const
{
	unsigned int ordinal1 = ((const TakionTool*)item1)->GetToolOrdinal();
	unsigned int ordinal2 = ((const TakionTool*)item2)->GetToolOrdinal();
	return ordinal1 < ordinal2 ? -1:
		ordinal2 < ordinal1 ? 1:
		0;
}

COLORREF ListBoxExtensionWindow::GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	const TakionTool* tool = (const TakionTool*)lpDrawItemStruct->itemData;
	return tool->m_hWnd ? tool->isHidden() ? m_hiddenColor : m_visibleColor : m_destroyedColor;
}

COLORREF ListBoxExtensionWindow::GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	return m_selectable && (lpDrawItemStruct->itemState & ODS_SELECTED) ? RGB(255, 255, 255) : GetSelectedBkColor(lpDrawItemStruct);
}

int ListBoxExtensionWindow::FindItemByOrdinal(const unsigned int ordinal) const
{
	unsigned int count = m_itemCount;//GetCount();
	if(count == 0)
	{
		return -1;
	}
	unsigned int ord;
/*
	if((GetStyle() & LBS_SORT) == 0 || m_doNotSort)//unsorted
	{
		for(int i = 0; i < count; ++i)
		{
//			if(Compare(toFind, GetItemDataPtr(i)) == 0)
			if(((const ConnectionItem*)GetItemDataPtr(middle))->GetDataType() == dataType)
			{
				return i;
			}
		}
		return -1;
	}
	else
	{
*/
        int low = 0;
        int high = count - 1;
        int middle;
//        int ret;
        while(high >= low)
        {
            middle = (low + high) >> 1;
			ord = ((const TakionTool*)GetItemDataPtr(middle))->GetToolOrdinal();
//            ret = Compare(toFind, );
            if(ordinal < ord)
            {
                high = middle - 1;
            }
            else if(ordinal > ord)
            {
                low = middle + 1;
            }
			else
			{
				if(middle > 0)
				{
					for(--middle; middle >= 0; --middle)
					{
						if(ordinal != ((const TakionTool*)GetItemDataPtr(middle))->GetToolOrdinal())
						{
							break;
						}
					}
	                return middle + 1;
				}
				return middle;
			}
        }
        return -1;
//	}
}

int ListBoxExtensionWindow::TakionWindowAdded(TakionTool* tool)
{
	int found = FindItemByOrdinal(tool->GetToolOrdinal());
	if(found >= 0)
	{
		if(found == GetCurSel())
		{
			found = -1;
		}
	}
	else
	{
		found = AddString((const char*)tool);
	}
	if(found >= 0)
	{
		SetCurSel(found);
	}
	return found;
}

int ListBoxExtensionWindow::TakionWindowRepaintTool(TakionTool* tool)
{
	int found = FindItemByOrdinal(tool->GetToolOrdinal());
	if(found >= 0)
	{
		InvalidateItem(found);
	}
	return found;
}

int ListBoxExtensionWindow::TakionWindowRemoved(TakionTool* tool)
{
	int found = -1;
	unsigned int count = m_itemCount;//GetCount();
	if(count > 0)
	{
		found = FindItemByOrdinal(tool->GetToolOrdinal());
		if(found >= 0)
		{
			bool selected = found == GetCurSel();
			DeleteString(found);
			if(selected)
			{
				--count;
				if((unsigned int)found >= count)
				{
					found = (int)count - 1;
				}

				if(found >= 0)
				{
					SetCurSel(found);
				}
			}
			else
			{
				found = -1;
			}
		}
	}
	return found;
}

const char* ListBoxExtensionWindow::GetItemName(const void* item) const
{
	return ((const TakionTool*)item)->GetToolId();
}

void ListBoxExtensionWindow::DrawNumber(const DRAWITEMSTRUCT* lpDrawItemStruct, const CRect& r)
{
	const TakionTool* tool = (const TakionTool*)lpDrawItemStruct->itemData;
	char num[33];
	DrawText(lpDrawItemStruct->hDC, U_UnsignedNumberToStrNoDecDigits(tool->GetToolOrdinal(), num, sizeof(num)), -1, &(RECT&)r, DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP|DT_NOPREFIX);//|DT_END_ELLIPSIS);
}

void ListBoxExtensionWindow::ListBoxCreated()
{
	ListBoxOwnerDraw::ListBoxCreated();
	m_menuWidth = MeasureText("Corner all", GetMenuFont());
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
		TakionBaseWnd::CalculateTextWidth(rect, hdc, "Corner all", m_menuWidth);

		if(oldFont)
		{
			::SelectObject(hdc, oldFont);
		}
		::ReleaseDC(m_hWnd, hdc);
	}
*/
}

void ListBoxExtensionWindow::DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->itemID)
	{
		case TMID_REMOVE:
		case TMID_CORNER_ACTIVE:
		case TMID_ACTIVATE_SELECTED:
		case TMID_HIDE:
		case TMID_SHOW:
		case TMID_CORNER:
		case TMID_CLEAR:
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
		break;

		default:
		ListBoxOwnerDraw::DrawMenuItem(lpDrawItemStruct);
		break;
	}
}

bool ListBoxExtensionWindow::MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	switch(lpMeasureItemStruct->itemID)
	{
		case TMID_REMOVE:
		case TMID_CORNER_ACTIVE:
		case TMID_ACTIVATE_SELECTED:
		case TMID_HIDE:
		case TMID_SHOW:
		case TMID_CORNER:
		case TMID_CLEAR:

		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
		lpMeasureItemStruct->itemWidth = m_menuWidth;// mainMessageLoopWnd->GetAddMenuWidth();// + lpMeasureItemStruct->itemHeight;
		break;

		default:
		return ListBoxOwnerDraw::MeasureMenuItem(lpMeasureItemStruct);
	}
	return true;
}


void ListBoxExtensionWindow::AddMenuItems(CMenu& menu)
{
	const bool hasItems = m_itemCount > 0;//GetCount() > 0;
	const int sel = GetCurSel();
	const bool selected = sel >= 0;
	const TakionTool* selectedTool = selected ? (const TakionTool*)GetItemDataPtr(sel) : nullptr;

	menu.AppendMenu(MF_OWNERDRAW|(selected ? MF_ENABLED : MF_GRAYED), TMID_REMOVE, "Close");
	menu.AppendMenu(MF_OWNERDRAW|(selected ? MF_ENABLED : MF_GRAYED), TMID_CORNER_ACTIVE, "Corner");
	menu.AppendMenu(MF_OWNERDRAW|(selected ? MF_ENABLED : MF_GRAYED), TMID_ACTIVATE_SELECTED, "Activate");
	menu.AppendMenu(MF_OWNERDRAW|(selected && !selectedTool->isHidden() ? MF_ENABLED : MF_GRAYED), TMID_HIDE, "Hide");
	menu.AppendMenu(MF_OWNERDRAW|(selected && selectedTool->isHidden() ? MF_ENABLED : MF_GRAYED), TMID_SHOW, "UnHide");
	
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CORNER, "Corner all");
	menu.AppendMenu(MF_OWNERDRAW|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLEAR, "Clear");
}

BOOL ListBoxExtensionWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD loword = (DWORD)wParam;//LOWORD(wParam);
    switch(loword)
    {
		case TMID_REMOVE:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				((TakionTool*)GetItemDataPtr(sel))->DestroyWindow();
			}
		}
		break;

		case TMID_CORNER_ACTIVE:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				((TakionTool*)GetItemDataPtr(sel))->Corner();
			}
		}
		break;

		case TMID_ACTIVATE_SELECTED:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				::SetActiveWindow(((TakionTool*)GetItemDataPtr(sel))->m_hWnd);
			}
		}
		break;

		case TMID_HIDE:
		m_dlg->HideSelectedWindow();
		break;

		case TMID_SHOW:
		m_dlg->UnhideSelectedWindow();
		break;

		case TMID_CORNER:
		CornerAll();
		break;

		case TMID_CLEAR:
		DestroyTools();
		break;

		default:
		return CListBox::OnCommand(wParam, lParam);
	}
	return TRUE;
}

void ListBoxExtensionWindow::CornerAll()
{
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		((TakionTool*)GetItemDataPtr(i))->Corner();
	}
}

void ListBoxExtensionWindow::DestroyTools()
{
	typedef std::list<TakionTool*> ToolList;
	ToolList toolList;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		toolList.push_back((TakionTool*)GetItemDataPtr(i));
	}
	TakionTool* tool;
	ResetContent();
	for(ToolList::iterator it = toolList.begin(), itend = toolList.end(); it != itend; ++it)
	{
		tool = *it;
		if(tool->m_hWnd)
		{
			tool->DestroyWindow();
		}
		else
		{
			delete tool;
		}
	}
}

enum SampleSettingWindowIds
{
	listBoxWindowTypeId = 10000,
	buttonNewId,
	staticWindowTypeId,

	listBoxWindowId,

	windowIdsCount
};

SampleDialogSettingWindow::SampleDialogSettingWindow(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSetting(mainWnd, parentTab, "Window"),
	m_ListBoxWindowType(false),
	m_ListBoxExtensionWindow(this),
	m_leftTypeOffset(0),
	m_topTypeOffset(0),
	m_typeWidth(0),
	m_leftOffset(0),
	m_topOffset(0),
	m_currentType(0xFF)
{
	int w = 24;
	int x = hgap;
	int y = vgap;

	int width = 80;

	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX, 0, x, y, width, staticHeight, staticWindowTypeId);
	AddButtonIndirect("New", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x, y += staticHeight + vgap, w, editHeight, buttonNewId);
	y += editHeight + vgap;
	int h = m_height - y;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, width, h, listBoxWindowTypeId);

	x += width + hgap;
	y = 0;
	int listWidth = m_width - x;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, listWidth, m_height, listBoxWindowId);
}

HWND SampleDialogSettingWindow::GetFirstTabControl() const
{
	return m_ListBoxWindowType.m_hWnd;
}

void SampleDialogSettingWindow::TakionWindowAdded(TakionTool* tool)
{
	if(m_ListBoxExtensionWindow.TakionWindowAdded(tool) >= 0)
	{
		OnSelchangeWindow();
	}
}

void SampleDialogSettingWindow::TakionWindowRemoved(TakionTool* tool)
{
	if(m_ListBoxExtensionWindow.TakionWindowRemoved(tool) >= 0)
	{
		OnSelchangeWindow();
	}
}

void SampleDialogSettingWindow::TakionWindowRepaintTool(TakionTool* tool)
{
	m_ListBoxExtensionWindow.TakionWindowRepaintTool(tool);
}

void SampleDialogSettingWindow::Resize(bool widthChanged, bool heightChanged)
{
	BaseDialogSetting::Resize(widthChanged, heightChanged);
	if(m_ListBoxWindowType.m_hWnd)
	{
		CRect rect;
		m_ListBoxWindowType.GetWindowRect(&rect);
		ScreenToClient(&rect);
		if(!m_typeWidth)
		{
			m_leftTypeOffset = rect.left;
			m_topTypeOffset = rect.top;
			m_typeWidth = rect.right - m_leftTypeOffset;

			m_ListBoxExtensionWindow.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_leftOffset = rect.left;
			m_topOffset = rect.top;
		}
		if(heightChanged)
		{
			m_ListBoxWindowType.SetWindowPos(nullptr, m_leftTypeOffset, m_topTypeOffset, m_typeWidth, m_clientRect.bottom - m_topTypeOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
		}
		m_ListBoxExtensionWindow.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, m_clientRect.bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
	}
}

void SampleDialogSettingWindow::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
}

void SampleDialogSettingWindow::HideSelectedWindow()
{
	int sel = m_ListBoxExtensionWindow.GetCurSel();
	if(sel >= 0)
	{
		TGUI_GetTakionMainWnd()->HideWindow((TakionTool*)m_ListBoxExtensionWindow.GetItemDataPtr(sel));
	}
}

void SampleDialogSettingWindow::UnhideSelectedWindow()
{
	int sel = m_ListBoxExtensionWindow.GetCurSel();
	if(sel >= 0)
	{
		TGUI_GetTakionMainWnd()->UnhideWindow((TakionTool*)m_ListBoxExtensionWindow.GetItemDataPtr(sel));
	}
}

void SampleDialogSettingWindow::BeforeDestroy()
{
	m_ListBoxExtensionWindow.DestroyTools();
	BaseDialogSetting::BeforeDestroy();
}

void SampleDialogSettingWindow::Leaving()
{
//	TD_GetNewAccountObservable().RemoveObserver(this);
//	TD_GetAdminObservable().RemoveObserver(this);
}

BOOL SampleDialogSettingWindow::OnInitDialog()
{
	m_currentType = 0xFF;
	BOOL ret = BaseDialogSetting::OnInitDialog();
	m_ListBoxWindowType.AddString(StockComparator::stockComparatorId);
	m_ListBoxWindowType.AddString(MyMarketSorter::myMarketSorterId);
//	RestoreLayout();
	return ret;
}

void SampleDialogSettingWindow::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
	DDX_Control(pDX, listBoxWindowTypeId, m_ListBoxWindowType);
	DDX_Control(pDX, buttonNewId, m_ButtonNew);
	DDX_Control(pDX, staticWindowTypeId, m_StaticWindowType);
	DDX_Control(pDX, listBoxWindowId, m_ListBoxExtensionWindow);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingWindow, BaseDialogSetting)
	ON_LBN_SELCHANGE(listBoxWindowTypeId, OnSelchangeWindowType)
	ON_BN_CLICKED(buttonNewId, OnNewWindow)
	ON_LBN_SELCHANGE(listBoxWindowId, OnSelchangeWindow)
	ON_LBN_DBLCLK(listBoxWindowId, OnDblclkWindow)
	ON_LBN_DBLCLK(listBoxWindowTypeId, OnDblclkWindowType)
END_MESSAGE_MAP()

TakionTool* SampleDialogSettingWindow::CreateTakionWindow() const
{
	if(m_currentType != 0xFF)
	{
		TakionTool* tool;
		CRect rect(m_leftOffset, m_topOffset, m_leftOffset, m_topOffset);
		ClientToScreen(&rect);
		switch(m_currentType)
		{
			case SWT_STOCK_COMPARATOR:
			tool = TGUI_GetTakionMainWnd()->CreateTool(StockComparator::stockComparatorId, nullptr, &rect, nullptr);//, bool clone = false, unsigned int contentModifier = 0, unsigned __int64 version = 0);
			break;

			case SWT_MY_MARKET_SORTER:
			tool = TGUI_GetTakionMainWnd()->CreateTool(MyMarketSorter::myMarketSorterId, nullptr, &rect, nullptr);//, bool clone = false, unsigned int contentModifier = 0, unsigned __int64 version = 0);
			break;

			default:
			tool = nullptr;
			break;
		}
		if(tool)
		{
			tool->ShowWindow(SW_SHOW);
		}
		return tool;
	}
	return nullptr;
}

void SampleDialogSettingWindow::CreateTakionWindowOrDisplayError()
{
	if(m_currentType != 0xFF && !CreateTakionWindow())
	{
		TakionMessageBox mb(m_mainWnd);
		char num[33];
		mb.AddMessage(U_SignedNumberToStrNoDecDigits(m_currentType, num, sizeof(num)));
		mb.DisplayModal(this, "Unknown Window Type", MB_OK|MB_ICONERROR);//, RGB(0, 0, 128), RGB(255, 255, 255));
	}
}

void SampleDialogSettingWindow::OnNewWindow()
{
	CreateTakionWindowOrDisplayError();
}

bool SampleDialogSettingWindow::GetWindowsLayout(UIntStrMap& windowsMap, unsigned int contentModifier) const
{
	if(m_ListBoxExtensionWindow.m_hWnd)
	{
		const TakionTool* tool;
		std::string value;
		for(unsigned int i = 0, end = m_ListBoxExtensionWindow.GetItemCount(); i < end; ++i)
		{
			value.clear();
			tool = (const TakionTool*)m_ListBoxExtensionWindow.GetItemDataPtr(i);
			tool->toString(value, contentModifier);
			windowsMap.insert(UIntStrMap::value_type(i, value));
		}
		return true;
	}
	return false;
}

void SampleDialogSettingWindow::OnDblclkWindowType()
{
	CreateTakionWindowOrDisplayError();
}

void SampleDialogSettingWindow::OnDblclkWindow()
{
	int sel = m_ListBoxExtensionWindow.GetCurSel();
	if(sel >= 0)
	{
		TakionTool* tool = (TakionTool*)m_ListBoxExtensionWindow.GetItemDataPtr(sel);
		TGUI_GetTakionMainWnd()->UnhideWindow(tool);
		::SetActiveWindow(tool->m_hWnd);
	}
}

void SampleDialogSettingWindow::OnSelchangeWindow()
{
	int sel = m_ListBoxExtensionWindow.GetCurSel();
	if(sel >= 0)
	{
	}
	else
	{
	}
}

void SampleDialogSettingWindow::OnSelchangeWindowType()
{
	int sel = m_ListBoxWindowType.GetCurSel();
	unsigned int type = sel >= 0 ? (unsigned char)sel : 0xFF;
	if(m_currentType != type)
	{
		m_currentType = type;
		if(m_currentType == 0xFF)
		{
			m_ButtonNew.EnableWindow(FALSE);
			m_StaticWindowType.SetWindowText("");
		}
		else
		{
			m_ButtonNew.EnableWindow(TRUE);
			m_StaticWindowType.SetWindowText((const char*)m_ListBoxWindowType.GetItemDataPtr(sel));
		}
		m_modified = true;
	}
}

void SampleDialogSettingWindow::UpdateSettings()
{
//	BaseDialogSetting::UpdateSettings();
//	int sel = m_ListBoxWindowType.GetCurSel();
	extensionConfig->m_winType.SetValue(m_currentType);

}

void SampleDialogSettingWindow::RestoreLayout()
{
	unsigned char sel = extensionConfig->m_winType.GetValue();
	if(sel < (unsigned char)m_ListBoxWindowType.GetItemCount())
	{
		m_ListBoxWindowType.SetCurSel(sel);
	}
	else
	{
		m_ListBoxWindowType.SetCurSel(-1);
	}
	OnSelchangeWindowType();

	sel = m_ListBoxExtensionWindow.GetCurSel();
	const TakionTool* selectedTool = sel >= 0 ? (const TakionTool*)m_ListBoxExtensionWindow.GetItemDataPtr(sel) : nullptr;
	TakionTool* tool;
	ToolPool pool;
	for(unsigned int i = 0, end = m_ListBoxExtensionWindow.GetItemCount(); i < end; ++i)
	{
		tool = (TakionTool*)m_ListBoxExtensionWindow.GetItemDataPtr(i);
		tool->ClearConfig();
		tool->BeforeDestroy();
		pool.Insert(tool);
	}

	m_ListBoxExtensionWindow.ResetContent();

	const ExtensionDerivedConfig::ExtToolMap& collection = extensionConfig->m_extTools;
	ExtensionDerivedConfig::ExtToolMap::ConstIterator its = collection.GetConstBegin();
	ExtensionDerivedConfig::ExtToolMap::ConstIterator itsend = collection.GetConstEnd();
	std::string toolId;
	const char* cursor;
//	unsigned int workspace;
//	const ToolCreationButton* button;
	unsigned __int64 savedVersion = extensionConfig->GetSavedVersion();
	for(; its != itsend; ++its)
	{
		cursor = its->second.GetValue().c_str();
		toolId.clear();
		U_ReadStrToken(toolId, cursor, settingDel);
		tool = pool.GetTool(toolId);
		TakionMainWnd* frame = TGUI_GetTakionMainWnd();
		if(!tool)
		{
//			button = m_toolboxRow.FindButton(toolId);
//			tool = CreateTool(toolId.c_str(), button ? button->GetBitmap() : nullptr, nullptr, cursor, false, 0, savedVersion);
			tool = frame->CreateTool(toolId.c_str(), nullptr, nullptr, cursor, false, 0, savedVersion);
			if(tool)
			{
				frame->UpdateWorkspaceFont(tool->GetWorkspace());
			}
			else
			{
TD_Log(toolId.c_str(), "ERROR Extension Cannot create tool");
			}
		}
		else
		{
			tool->fromString(cursor, savedVersion, false, 0);

			tool->MoveToFrameRect(true, false);
			tool->SetCommandToolOrdinal(m_mainWnd->GetCommandToolCount());

			frame->AddToolToCollection(tool);

			sel = m_ListBoxExtensionWindow.AddString((const char*)tool);
			if(tool == selectedTool)
			{
				m_ListBoxExtensionWindow.SetCurSel(sel);
			}
		}

		frame->AddHiddenTool(tool);
		frame->AddPinnedTool(tool);
		frame->UpdateWorkspace();
		
	}

	OnSelchangeWindow();
/*
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

	CString oldPath;
	m_EditPath.GetWindowText(oldPath);
//	bool oldLoadOnStart = m_CheckBoxLoadOnStart.GetCheck() == BST_CHECKED;
	bool loadOnStart = extensionConfig->m_msLoadOnStart.GetValue();
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
	EnableControl(m_ButtonClearTimer.m_hWnd, m_ListBoxTimer.GetCount() > 0, m_ListBoxTimer.m_hWnd);

	m_timersChanged = false;
*/
	m_modified = false;
}


/*
HBRUSH SampleDialogSettingWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
//
	switch(pWnd->GetDlgCtrlID())
	{
		case editSymbolId:
		if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_takionVisual->GetWarningBrush();
		}
		break;

	}
//
	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/
