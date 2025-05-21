#include "stdafx.h"
#include "OrderDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum ExtensionOrderDialogIds
{
	editId = 10000,
//	orderButtonId,
//	commandListBoxId,
};

OrderDialog::OrderDialog():
	DraggableDialog(TGUI_GetTakionMainWnd(), "ExtensionOrder", WS_VISIBLE|WS_CLIPCHILDREN|WS_POPUP|WS_CAPTION|WS_THICKFRAME|WS_SYSMENU, 0, 10, 10, 100, 100)//LPCSTR font = NULL, WORD fontSize = 8
{
	int left = 10;
	int height = 12;
	int width = 80;
	int vgap = 4;
	int y = 10;
	AddEditBoxIndirect(NULL, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_UPPERCASE | ES_AUTOHSCROLL, 0, left, y, width, height, editId);
//	AddButtonIndirect("Order", WS_VISIBLE|WS_DISABLED|WS_TABSTOP|BS_PUSHBUTTON, 0, left, y += height + vgap, width, height, orderButtonId);
}

void OrderDialog::DoDataExchange(CDataExchange* pDX)
{
	DraggableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, editId, m_edit);
}

BEGIN_MESSAGE_MAP(OrderDialog, DraggableDialog)
//	ON_BN_CLICKED(orderButtonId, OnOrder)
/*
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(searchEditId, OnSearchChange)
	ON_LBN_SELCHANGE(commandListBoxId, OnSelchangeCommand)
	ON_LBN_SELCHANGE(keyListBoxId, OnSelchangeKey)
	ON_LBN_DBLCLK(commandListBoxId, OnDblclkCommand)
	ON_LBN_DBLCLK(keyListBoxId, OnDblclkKey)
	ON_BN_CLICKED(checkBoxMappedOnTopKeys, OnMappedOnTopKeys)
	ON_BN_CLICKED(checkBoxMappedOnTopCommands, OnMappedOnTopCommands)
	ON_MESSAGE(WM_USER + 1, OnMapCommand)
*/
END_MESSAGE_MAP()
