#include "stdafx.h"
#include "BaseDialogSettingSimulateAccountSymbol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulateAccountSymbol::BaseDialogSettingSimulateAccountSymbol(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseDialogSettingSimulateAccount(mainWnd, parentTab, label),
	m_EditSymbol(U_GetValidStockChars(), false)
//	m_symbolType(SYMT_SYMBOL_STRING),
//	m_emptySymbolValid(false),
//	m_symbolNotUsed(false)
{
	m_bottomControl += vgap;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editSimAccountSymbolId);
	AddStaticIndirect("Symbol", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticSimAccountSymbolId);
//	AddButtonIndirect("Symbol", WS_DISABLED|WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, m_xc + m_firstColumnWidth + hgap, m_bottomControl + (editHeight - checkBoxHeight) / 2, m_firstColumnWidth, checkBoxHeight, radioSimSymbolId);

	const int controlWidth = m_xc + m_firstColumnWidth + hgap + m_firstColumnWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	m_bottomControl += editHeight;
}

HWND BaseDialogSettingSimulateAccountSymbol::GetFirstTabControl() const
{
	return m_EditSymbol.m_hWnd;
}

bool BaseDialogSettingSimulateAccountSymbol::isSendMessageButtonEnabled() const
{
//	return m_symbolType == SYMT_SYMBOL_MAPPING || !m_symbol.empty() || m_emptySymbolValid;
	return BaseDialogSettingSimulateAccount::isSendMessageButtonEnabled() && !m_symbol.empty();
}
/*
void BaseDialogSettingSimulateAccountSymbol::UpdateSettings()
{
	BaseDialogSettingSimulateAccount::UpdateSettings();
}

void BaseDialogSettingSimulateAccountSymbol::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
	BaseDialogSettingSimulateAccount::UpdateControls();
}

void BaseDialogSettingSimulateAccountSymbol::BeforeDestroy()
{
	BaseDialogSettingSimulateAccount::BeforeDestroy();
}
*/
/*
void BaseDialogSettingSimulateAccountSymbol::Leaving()
{
	TD_GetNewAccountObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
}
*/

BOOL BaseDialogSettingSimulateAccountSymbol::OnInitDialog()
{
	m_symbol.clear();
//	m_symbolType = SYMT_SYMBOL_STRING;

	BOOL ret = BaseDialogSettingSimulateAccount::OnInitDialog();

	m_EditSymbol.SetLimitText(7);
//	m_RadioSymbol.SetCheck(BST_CHECKED);
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

void BaseDialogSettingSimulateAccountSymbol::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateAccount::DoDataExchange(pDX);
	DDX_Control(pDX, editSimAccountSymbolId, m_EditSymbol);
//	DDX_Control(pDX, radioSimSymbolId, m_RadioSymbol);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateAccountSymbol, BaseDialogSettingSimulateAccount)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editSimAccountSymbolId, OnSymbolChange)
	ON_EN_UPDATE(editSimAccountSymbolId, OnSymbolUpdate)
//	ON_BN_CLICKED(radioSimSymbolId, OnSymbolType)
END_MESSAGE_MAP()
/*
void BaseDialogSettingSimulateAccountSymbol::SymbolTypeChanged()
{
	m_EditSymbol.Invalidate(FALSE);
}

void BaseDialogSettingSimulateAccountSymbol::OnSymbolType()
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
*/
void BaseDialogSettingSimulateAccountSymbol::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void BaseDialogSettingSimulateAccountSymbol::OnSymbolChange()
{
	const bool empty = m_symbol.empty();
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_symbol = text;
	if(m_symbol.empty() != empty)
	{
		m_EditSymbol.Invalidate(FALSE);
		EnableSendMessageButton();
	}
	SymbolChanged();
	m_modified = true;
}

HBRUSH BaseDialogSettingSimulateAccountSymbol::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSimAccountSymbolId:
/*
		if(m_symbolType != SYMT_SYMBOL_STRING || m_symbolNotUsed)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_takionVisual->GetGrayBrush();
		}
		else if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return m_emptySymbolValid ? *m_takionVisual->GetBlueBrush() : *m_takionVisual->GetWarningBrush();
		}
*/
		if(!isSymbolUsed())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		else if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		break;
	}
	return BaseDialogSettingSimulateAccount::OnCtlColor(pDC, pWnd, nCtlColor);
}
