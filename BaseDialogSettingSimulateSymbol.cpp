#include "stdafx.h"
#include "BaseDialogSettingSimulateSymbol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulateSymbol::BaseDialogSettingSimulateSymbol(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseDialogSettingSimulateMessage(mainWnd, parentTab, label),
	m_EditSymbol(U_GetValidStockChars(), false),
	m_symbolType(SYMT_SYMBOL_STRING),
	m_emptySymbolValid(false),
	m_symbolNotUsed(false)
{
	m_bottomControl += vgap;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editSimSymbolId);
//	AddStaticIndirect("Symbol", WS_VISIBLE | SS_NOPREFIX, 0, xc + width, y + (editHeight - staticHeight) / 2, width, staticHeight, staticSimSymbolId);
	AddButtonIndirect("Symbol", WS_DISABLED|WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, m_xc + m_firstColumnWidth + hgap, m_bottomControl + (editHeight - checkBoxHeight) / 2, m_firstColumnWidth, checkBoxHeight, radioSimSymbolId);

	const int controlWidth = m_xc + m_firstColumnWidth + hgap + m_firstColumnWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	m_bottomControl += editHeight;
}

HWND BaseDialogSettingSimulateSymbol::GetFirstTabControl() const
{
	return m_EditSymbol.m_hWnd;
}

bool BaseDialogSettingSimulateSymbol::isSendMessageButtonEnabled() const
{
//	return m_symbolType == SYMT_SYMBOL_MAPPING || !m_symbol.empty() || m_emptySymbolValid;
	return m_symbolType == SYMT_SYMBOL_MAPPING || m_numericSymbol != 0 || m_emptySymbolValid;
}

void BaseDialogSettingSimulateSymbol::UpdateSettings()
{
	BaseDialogSettingSimulateMessage::UpdateSettings();
}

void BaseDialogSettingSimulateSymbol::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
	BaseDialogSettingSimulateMessage::UpdateControls();
}

void BaseDialogSettingSimulateSymbol::BeforeDestroy()
{
	m_symbol.clear();
	m_numericSymbol = 0;
	m_symbolType = SYMT_SYMBOL_STRING;

	BaseDialogSettingSimulateMessage::BeforeDestroy();
}

/*
void BaseDialogSettingSimulateSymbol::Leaving()
{
	TD_GetNewAccountObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
}
*/

BOOL BaseDialogSettingSimulateSymbol::OnInitDialog()
{
	m_symbol.clear();
	m_numericSymbol = 0;
	m_symbolType = SYMT_SYMBOL_STRING;
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);

	BOOL ret = BaseDialogSettingSimulateMessage::OnInitDialog();

	m_EditSymbol.SetLimitText(7);
	m_RadioSymbol.SetCheck(BST_CHECKED);
/*
	m_ComboBoxAccount.AddString(nullptr);

	TakionIterator* iterator = TD_CreateAccountIterator();
	Account* account;
	TD_LockAccountStorage();
	iterator->Reset();
	while(account = (Account*)TD_GetNextAccount(iterator))
	{
		m_ComboBoxAccount.AddString((const char*)account);
	}
	TD_UnlockAccountStorage();
	delete iterator;

	m_ComboBoxAccount.SetCurSel(0);
*/
	return ret;
}

void BaseDialogSettingSimulateSymbol::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateMessage::DoDataExchange(pDX);
	DDX_Control(pDX, editSimSymbolId, m_EditSymbol);
	DDX_Control(pDX, radioSimSymbolId, m_RadioSymbol);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateSymbol, BaseDialogSettingSimulateMessage)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editSimSymbolId, OnSymbolChange)
	ON_EN_UPDATE(editSimSymbolId, OnSymbolUpdate)
	ON_BN_CLICKED(radioSimSymbolId, OnSymbolType)
END_MESSAGE_MAP()

void BaseDialogSettingSimulateSymbol::SymbolTypeChanged()
{
	m_EditSymbol.Invalidate(FALSE);
}

void BaseDialogSettingSimulateSymbol::OnSymbolType()
{
	const unsigned char symbolType = m_RadioSymbol.GetCheck() == BST_CHECKED ? SYMT_SYMBOL_STRING : SYMT_SYMBOL_MAPPING;
	if(symbolType != m_symbolType)
	{
		m_symbolType = symbolType;
		SymbolTypeChanged();
		EnableSendMessageButton();
		m_modified = true;
	}
}

void BaseDialogSettingSimulateSymbol::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void BaseDialogSettingSimulateSymbol::OnSymbolChange()
{
	bool empty = !m_numericSymbol;// m_symbol.empty();
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_symbol = text;
	m_numericSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(m_symbol.c_str(), sizeof(unsigned __int64) - 1);
	if(!m_numericSymbol != empty)
	{
		m_EditSymbol.Invalidate(FALSE);
		EnableSendMessageButton();
	}
	m_modified = true;
}

void BaseDialogSettingSimulateSymbol::SetSymbolNotUsed(bool symbolNotUsed)
{
	if(symbolNotUsed != m_symbolNotUsed)
	{
		m_symbolNotUsed = symbolNotUsed;
		SymbolNotUsedChanged();
	}
}

void BaseDialogSettingSimulateSymbol::SymbolNotUsedChanged()
{
	m_EditSymbol.Invalidate(FALSE);
	EnableSendMessageButton();
	m_modified = true;
}

HBRUSH BaseDialogSettingSimulateSymbol::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSimSymbolId:
		if(m_symbolType != SYMT_SYMBOL_STRING || m_symbolNotUsed)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		else if(!m_numericSymbol)//m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return m_emptySymbolValid ? *m_visualBase->GetBlueBrush() : *m_visualBase->GetWarningBrush();
		}
		break;

	}
	return BaseDialogSettingSimulateMessage::OnCtlColor(pDC, pWnd, nCtlColor);
}
