#include "stdafx.h"
#include "DialogSignedPriceValue.h"
//#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DialogSignedPriceValue::DialogSignedPriceValue(TakionMainWnd* mainWnd, bool toDestroyOnFocus, TakionFrame* receipient, CWnd* parent):
	BaseDialogValue(mainWnd, &m_SpinValue, toDestroyOnFocus, receipient, parent)//, nullptr, WS_BORDER|WS_POPUP | WS_CLIPCHILDREN, 0, 0, 0, 0, 0, nullptr, 0),
{
	unsigned int y = 0;
	AddStaticIndirect("Value", WS_VISIBLE | SS_NOPREFIX | SS_RIGHT | SS_CENTERIMAGE, 0, 0, y, staticWidth, spinHeight, staticValueId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, staticWidth + buttonGapH, y, spinWidth, spinHeight, spinValueId);

	y += spinHeight + buttonGapV;

	AddButtons(y, buttonWidth, buttonHeight, buttonGapH, buttonGapV);
}
/*
bool DialogSignedPriceValue::isValid() const
{
	return !m_currentSymbol1.empty() && !m_currentSymbol2.empty();
}

void DialogSignedPriceValue::DoEnableOkButton(bool enable)
{
	bool valid = isValid();
	EnableControl(m_ButtonOk.m_hWnd, valid, m_ButtonCancel.m_hWnd);
	EnableControl(m_ButtonApply.m_hWnd, valid, m_ButtonCancel.m_hWnd);
	EnableControl(m_ButtonReset.m_hWnd, enable, m_ButtonCancel.m_hWnd);
}

bool DialogSignedPriceValue::isChanged() const
{
	return BaseDialogValue::isChanged() || m_currentSymbol1 != m_symbol1 || m_currentSymbol2 != m_symbol2;
}

bool DialogSignedPriceValue::DoReset()
{
	bool ret = BaseDialogValue::DoReset();
	
	if(m_currentValue != m_value)
	{
		ret = true;
		m_currentValue = m_value;
		FillSpinControlFromSignedPrice(m_currentValue, m_SpinValue, m_multiplier);
	}
	return ret;
}
*/
void DialogSignedPriceValue::Display(TakionFrame* receipient, unsigned int code, const SignedPrice& value, CWnd* parent, const CPoint& mousePoint)
{
	BaseDialogValue::Display(receipient, code, value, parent, mousePoint, 60, 100);
}

void DialogSignedPriceValue::DoApply()
{
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

BEGIN_MESSAGE_MAP(DialogSignedPriceValue, BaseDialogValue)
/*
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editStock1Id, OnSymbol1Change)
	ON_EN_UPDATE(editStock1Id, OnSymbol1Update)
	ON_EN_CHANGE(editStock2Id, OnSymbol2Change)
	ON_EN_UPDATE(editStock2Id, OnSymbol2Update)
//	ON_MESSAGE(WM_USER + 15, OnSpinChange)
*/
END_MESSAGE_MAP()

void DialogSignedPriceValue::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogValue::DoDataExchange(pDX);
/*
	DDX_Control(pDX, staticStock1Id, m_StaticStock1);
	DDX_Control(pDX, editStock1Id, m_EditStock1);
	DDX_Control(pDX, staticStock2Id, m_StaticStock2);
	DDX_Control(pDX, editStock2Id, m_EditStock2);
//	DDX_Control(pDX, staticValueId, m_StaticValue);
//	DDX_Control(pDX, spinValueId, m_SpinValue);
*/
}
/*
BOOL DialogSignedPriceValue::OnInitDialog()
{
	BOOL ret = BaseDialogValue::OnInitDialog();

	return ret;
}
*/
