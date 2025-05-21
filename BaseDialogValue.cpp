#include "stdafx.h"
#include "BaseDialogValue.h"
//#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogValue::BaseDialogValue(TakionMainWnd* mainWnd, CWnd* rightmostControl, bool toDestroyOnFocus, TakionFrame* receipient, CWnd* parent):
	DialogValueReset(mainWnd, rightmostControl, toDestroyOnFocus, receipient, parent, nullptr, WS_BORDER|WS_POPUP | WS_CLIPCHILDREN, 0, 0, 0, 0, 0, nullptr, 0),
	m_decDigits(4),
	m_multiplier(1),
	m_divider(1),
	m_SpinValue(999999, -999999, 0, 10000, 3, TakionSpin::charComma, m_decDigits)
{
	for(unsigned int i = 0; i < m_decDigits; ++i)
	{
		m_multiplier *= 10;
	}
	m_divider = Price::divider / m_multiplier;

	m_SpinValue.AddSpinCell(1, false);
	m_SpinValue.AddSpinCell(100, false);
	m_SpinValue.AddSpinCell(10000, true);
/*
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

	AddButtons(y);
*/
}

LRESULT BaseDialogValue::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinValueId:
		m_SpinValue.UpdateValueNum();
		FillSignedPriceFromSpinControl(m_currentValue, m_SpinValue, m_multiplier);
		if(m_currentValue != m_value)DoEnableOkButton(true);
		else EnableOkButton();
		break;
	}
	return 0;
}
/*
bool BaseDialogValue::isValid() const
{
	return !m_currentSymbol1.empty() && !m_currentSymbol2.empty();
}
*/
void BaseDialogValue::DoEnableOkButton(bool enable)
{
	bool valid = isValid();
	EnableControl(m_ButtonOk.m_hWnd, valid, m_ButtonCancel.m_hWnd);
	EnableControl(m_ButtonApply.m_hWnd, valid, m_ButtonCancel.m_hWnd);
	EnableControl(m_ButtonReset.m_hWnd, enable, m_ButtonCancel.m_hWnd);
}

bool BaseDialogValue::isChanged() const
{
	return m_currentValue != m_value;// || m_currentSymbol1 != m_symbol1 || m_currentSymbol2 != m_symbol2;
}

bool BaseDialogValue::DoReset()
{
	bool ret = false;
	if(m_currentValue != m_value)
	{
		ret = true;
		m_currentValue = m_value;
		FillSpinControlFromSignedPrice(m_currentValue, m_SpinValue, m_multiplier, false);
	}
/*
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
*/
	return ret;
}

void BaseDialogValue::Display(TakionFrame* receipient, unsigned int code, const SignedPrice& value, CWnd* parent, const CPoint& mousePoint, int w, int h)
{
//	m_currentSymbol1 = m_symbol1 = symbol1;
//	m_currentSymbol2 = m_symbol2 = symbol2;
	m_currentValue = m_value = value;

	DialogValueReset::Display(receipient, code, parent, mousePoint, w, h);

	FillSpinControlFromSignedPrice(m_currentValue, m_SpinValue, m_multiplier, false);
//	m_EditStock1.SetWindowText(m_currentSymbol1.c_str());
//	m_EditStock2.SetWindowText(m_currentSymbol2.c_str());
}

void BaseDialogValue::DoApply()
{
//	m_symbol1 = m_currentSymbol1;
//	m_symbol2 = m_currentSymbol2;
	m_value = m_currentValue;
	if(m_receipient)
	{
		m_receipient->PostMessage(WM_USER + 77, m_code, 0);
	}
	else if(m_wndReceipient)
	{
		m_wndReceipient->PostMessage(WM_USER + 77, m_code, 0);
	}
}

BEGIN_MESSAGE_MAP(BaseDialogValue, DialogValueReset)
/*
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editStock1Id, OnSymbol1Change)
	ON_EN_UPDATE(editStock1Id, OnSymbol1Update)
	ON_EN_CHANGE(editStock2Id, OnSymbol2Change)
	ON_EN_UPDATE(editStock2Id, OnSymbol2Update)
*/
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void BaseDialogValue::DoDataExchange(CDataExchange* pDX)
{
	DialogValueReset::DoDataExchange(pDX);
/*
	DDX_Control(pDX, staticStock1Id, m_StaticStock1);
	DDX_Control(pDX, editStock1Id, m_EditStock1);
	DDX_Control(pDX, staticStock2Id, m_StaticStock2);
	DDX_Control(pDX, editStock2Id, m_EditStock2);
*/
	DDX_Control(pDX, staticValueId, m_StaticValue);
	DDX_Control(pDX, spinValueId, m_SpinValue);
}
