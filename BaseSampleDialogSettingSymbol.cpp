#include "stdafx.h"
#include "BaseSampleDialogSettingSymbol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseSampleDialogSettingSymbol::BaseSampleDialogSettingSymbol(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, bool useAccount, const char* label, int accountWidth, int accountTopOffset):
	BaseDialogSettingAccount(mainWnd, parentTab, useAccount, label, true, accountWidth, accountTopOffset),
	m_EditSymbol(U_GetValidStockChars(), false)
//	m_ComboBoxAccount(0xFFFFFFFF, "Current"),
//	m_SpinUserId(1000000000, 0, 0, 100000, 3, TakionSpin::charComma)
{
/*
	m_SpinUserId.AddSpinCell(1, false);
	m_SpinUserId.AddSpinCell(100, false);
	m_SpinUserId.AddSpinCell(1000000, true);
	m_SpinUserId.AddSpinCell(1000, true);
*/
//	int x = 0;
//	int y = m_bottomControl + vgap;
	m_bottomControl += vgap;
	int width = m_accountWidth - hgap - hgap;//52;
//	int xc = x + hgap;
	AddStaticIndirect("Symbol", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, m_bottomControl, width, staticHeight, staticSymbolNameId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, m_bottomControl += staticHeight, width, editHeight, editSymbolId);

	m_bottomControl += editHeight;
}

HWND BaseSampleDialogSettingSymbol::GetFirstTabControl() const
{
	return m_EditSymbol.m_hWnd;
}

BOOL BaseSampleDialogSettingSymbol::OnInitDialog()
{
	m_symbol.clear();

//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);

	BOOL ret = BaseDialogSettingAccount::OnInitDialog();

	m_EditSymbol.SetLimitText(7);
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

void BaseSampleDialogSettingSymbol::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingAccount::DoDataExchange(pDX);
	DDX_Control(pDX, editSymbolId, m_EditSymbol);
}

BEGIN_MESSAGE_MAP(BaseSampleDialogSettingSymbol, BaseDialogSettingAccount)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editSymbolId, OnSymbolChange)
	ON_EN_UPDATE(editSymbolId, OnSymbolUpdate)
END_MESSAGE_MAP()

void BaseSampleDialogSettingSymbol::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void BaseSampleDialogSettingSymbol::OnSymbolChange()
{
	const bool empty = m_symbol.empty();
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_symbol = text;
	if(m_symbol.empty() != empty)
	{
		m_EditSymbol.Invalidate(FALSE);
		SymbolEmptyChanged();
	}
}

HBRUSH BaseSampleDialogSettingSymbol::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSymbolId:
		if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		break;

	}
	return BaseDialogSettingAccount::OnCtlColor(pDC, pWnd, nCtlColor);
}

void BaseSampleDialogSettingSymbol::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case M_TEXT:
		{
//			const TMsgText* msg = (const TMsgText*)message;
//			DisplayAdminBroadcastMessage((const char*)msg + sizeof(TMsgText));
		}
		break;

		case TM_MARKET_SORTER_LOADED:
//		MarketSorterLoaded();
		break;

		case TM_MARKET_SORTER_CLEANUP:
//		MarketSorterCleanup();
		break;

		default:
		BaseDialogSettingAccount::Notify(message, from, info);
		break;
	}
}

