#include "stdafx.h"
#include "DialogStockPair.h"
//#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DialogStockPair::DialogStockPair(TakionMainWnd* mainWnd, bool toDestroyOnFocus, TakionFrame* receipient, CWnd* parent):
	BaseDialogValue(mainWnd, &m_EditStock1, toDestroyOnFocus, receipient, parent),//, nullptr, WS_BORDER|WS_POPUP | WS_CLIPCHILDREN, 0, 0, 0, 0, 0, nullptr, 0),
	m_EditStock1(U_GetValidStockChars(), false),
	m_EditStock2(U_GetValidStockChars(), false)
{
	unsigned int y = 0;
	AddStaticIndirect("Symbol1", WS_VISIBLE | SS_NOPREFIX | SS_RIGHT | SS_CENTERIMAGE, 0, 0, y, staticWidth, spinHeight, staticStock1Id);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, staticWidth + buttonGapH, y, spinWidth, spinHeight, editStock1Id);

	y += spinHeight + buttonGapV;
	AddStaticIndirect("Symbol2", WS_VISIBLE | SS_NOPREFIX | SS_RIGHT | SS_CENTERIMAGE, 0, 0, y, staticWidth, spinHeight, staticStock2Id);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, staticWidth + buttonGapH, y, spinWidth, spinHeight, editStock2Id);

	y += spinHeight + buttonGapV;
	AddStaticIndirect("Value", WS_VISIBLE | SS_NOPREFIX | SS_RIGHT | SS_CENTERIMAGE, 0, 0, y, staticWidth, spinHeight, staticValueId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, staticWidth + buttonGapH, y, spinWidth, spinHeight, spinValueId);

	y += spinHeight + buttonGapV;

	AddButtons(y, buttonWidth, buttonHeight, buttonGapH, buttonGapV);
}

bool DialogStockPair::isValid() const
{
	return !m_currentSymbol1.empty() && !m_currentSymbol2.empty();
}
/*
void DialogStockPair::DoEnableOkButton(bool enable)
{
	bool valid = isValid();
	EnableControl(m_ButtonOk.m_hWnd, valid, m_ButtonCancel.m_hWnd);
	EnableControl(m_ButtonApply.m_hWnd, valid, m_ButtonCancel.m_hWnd);
	EnableControl(m_ButtonReset.m_hWnd, enable, m_ButtonCancel.m_hWnd);
}
*/
bool DialogStockPair::isChanged() const
{
	return BaseDialogValue::isChanged() || m_currentSymbol1 != m_symbol1 || m_currentSymbol2 != m_symbol2;
}

bool DialogStockPair::DoReset()
{
	bool ret = BaseDialogValue::DoReset();
/*	
	if(m_currentValue != m_value)
	{
		ret = true;
		m_currentValue = m_value;
		FillSpinControlFromSignedPrice(m_currentValue, m_SpinValue, m_multiplier);
	}
*/
	if(m_currentSymbol1 != m_symbol1)
	{
		ret = true;
//		m_currentSymbol1 = m_symbol1;
		m_EditStock1.SetWindowText(m_symbol1.c_str());
	}
	if(m_currentSymbol2 != m_symbol2)
	{
		ret = true;
//		m_currentSymbol2 = m_symbol2;
		m_EditStock2.SetWindowText(m_symbol2.c_str());
	}
	return ret;
}

void DialogStockPair::Display(TakionFrame* receipient, unsigned int code, const char* symbol1, const char* symbol2, const SignedPrice& value, CWnd* parent, const CPoint& mousePoint)
{
	m_currentSymbol1 = m_symbol1 = symbol1;
	m_currentSymbol2 = m_symbol2 = symbol2;
//	m_currentValue = m_value = value;

	BaseDialogValue::Display(receipient, code, value, parent, mousePoint, 60, 100);

//	FillSpinControlFromSignedPrice(m_currentValue, m_SpinValue, m_multiplier);
	m_EditStock1.SetWindowText(m_currentSymbol1.c_str());
	m_EditStock2.SetWindowText(m_currentSymbol2.c_str());
}

void DialogStockPair::DoApply()
{
	m_symbol1 = m_currentSymbol1;
	m_symbol2 = m_currentSymbol2;
	BaseDialogValue::DoApply();
/*
	m_value = m_currentValue;
	if(m_receipient)
	{
		m_receipient->PostMessage(WM_USER + 77, 0, 0);
	}
	else if(m_wndReceipient)
	{
		m_wndReceipient->PostMessage(WM_USER + 77, 0, 0);
	}
*/
}

BEGIN_MESSAGE_MAP(DialogStockPair, BaseDialogValue)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editStock1Id, OnSymbol1Change)
	ON_EN_UPDATE(editStock1Id, OnSymbol1Update)
	ON_EN_CHANGE(editStock2Id, OnSymbol2Change)
	ON_EN_UPDATE(editStock2Id, OnSymbol2Update)
//	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void DialogStockPair::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogValue::DoDataExchange(pDX);
	DDX_Control(pDX, staticStock1Id, m_StaticStock1);
	DDX_Control(pDX, editStock1Id, m_EditStock1);
	DDX_Control(pDX, staticStock2Id, m_StaticStock2);
	DDX_Control(pDX, editStock2Id, m_EditStock2);
//	DDX_Control(pDX, staticValueId, m_StaticValue);
//	DDX_Control(pDX, spinValueId, m_SpinValue);
}

BOOL DialogStockPair::OnInitDialog()
{
	BOOL ret = BaseDialogValue::OnInitDialog();

	m_EditStock1.SetLimitText(7);
	m_EditStock2.SetLimitText(7);

	return ret;
}

void DialogStockPair::OnSymbol1Update()
{
	m_EditStock1.TryUpdate();
}

void DialogStockPair::OnSymbol2Update()
{
	m_EditStock2.TryUpdate();
}

void DialogStockPair::OnSymbol1Change()
{
	bool empty = m_currentSymbol1.empty();
//	bool symbolCurrent = m_security && !strcmp(m_security->GetSymbol(), m_symbol.c_str());
	CString text;
	m_EditStock1.GetWindowText(text);
	m_currentSymbol1 = text;
	if(m_currentSymbol1.empty() != empty)
	{
		m_EditStock1.Invalidate(FALSE);
		EnableOkButton();
	}
}

void DialogStockPair::OnSymbol2Change()
{
	bool empty = m_currentSymbol2.empty();
//	bool symbolCurrent = m_security && !strcmp(m_security->GetSymbol(), m_symbol.c_str());
	CString text;
	m_EditStock2.GetWindowText(text);
	m_currentSymbol2 = text;
	if(m_currentSymbol2.empty() != empty)
	{
		m_EditStock2.Invalidate(FALSE);
		EnableOkButton();
	}
}

HBRUSH DialogStockPair::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editStock1Id:
		if(m_currentSymbol1.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
/*
		else if(!m_security || strcmp(m_security->GetSymbol(), m_symbol.c_str()))
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_takionVisual->GetPendingBrush();
		}
*/
		break;

		case editStock2Id:
		if(m_currentSymbol2.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		break;
	}
	return BaseDialogValue::OnCtlColor(pDC, pWnd, nCtlColor);
}

