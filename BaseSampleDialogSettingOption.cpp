#include "stdafx.h"
#include "BaseSampleDialogSettingOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseSampleDialogSettingOption::BaseSampleDialogSettingOption(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, bool useAccount, const char* label, int accountWidth, int accountTopOffset):
	BaseSampleDialogSettingSymbol(mainWnd, parentTab, useAccount, label, accountWidth, accountTopOffset)
#ifndef TAKION_NO_OPTIONS
	,m_CheckBoxOption("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_RadioOptionCall("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),//4, -3),
	m_RadioOptionPut("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),//4, -3),
	m_ComboBoxOptionSuffix(m_visualBase->GetGrayColor()),
	m_SpinOptionStrikePrice(99999999, 0, 0, 1000, 3, TakionSpin::charComma, 2),
	m_DateTimeOptionExpiration(447, 0xFFFFFFFF),
	m_optionMode(false),
	m_optionSuffix('\0'),
	m_optionCall(false),
	m_expirationYear(0),
	m_expirationMonth(0),
	m_expirationDay(0)
#endif
{
#ifndef TAKION_NO_OPTIONS
	m_ComboBoxOptionSuffix.SetColorInvalid(m_visualBase->GetGrayColor());

	m_SpinOptionStrikePrice.AddSpinCell(1, false);
	m_SpinOptionStrikePrice.AddSpinCell(100, false);
	m_SpinOptionStrikePrice.AddSpinCell(10000, true);

//	int width = 52;
//	int halfWidth = width / 2;

	const int optionGroupHeight = 82;//56;

	m_bottomControl += vgap;
//	int x = m_xc - hgap;
	AddButtonIndirect("", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, m_xc - hgap, m_bottomControl, m_accountWidth, optionGroupHeight, groupSymbolOptionId);

	int y = m_bottomControl + groupOffset;
	int width = m_accountWidth - hgap - hgap;
	int halfWidth = width / 2;
	AddButtonIndirect("Option", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, y, width, checkBoxHeight, checkBoxSymbolOptionId);

	AddButtonIndirect("Call", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, m_xc, y += checkBoxHeight + vgap, halfWidth, checkBoxHeight, radioSymbolOptionCallId);
	AddButtonIndirect("Put", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, m_xc + halfWidth, y, halfWidth, checkBoxHeight, radioSymbolOptionPutId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_SORT|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL|WS_GROUP, 0, m_xc, y += checkBoxHeight + vgap, halfWidth, 400, comboBoxSymbolOptionSuffixId);
	AddStaticIndirect("Suffix", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + halfWidth, y + (comboBoxHeight - staticHeight) / 2, halfWidth, staticHeight, staticSymbolOptionSuffixId);

	AddStaticIndirect("StrikePrice", WS_VISIBLE|SS_NOPREFIX, 0, m_xc, y += comboBoxHeight + vgap, width, staticHeight, staticSymbolOptionStrikePriceId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += staticHeight, width, spinHeight, spinSymbolOptionStrikePriceId);

	AddStaticIndirect("Expiration", WS_VISIBLE|SS_NOPREFIX, 0, m_xc, y += spinHeight + vgap, width, staticHeight, staticSymbolOptionExpirationId);
	AddComponentIndirect("SysDateTimePick32", nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP|DTS_RIGHTALIGN, 0, m_xc, y += staticHeight, width, comboBoxHeight, datetimeSymbolOptionExpirationId);

	m_bottomControl += optionGroupHeight;

	m_RadioOptionCall.SetRadioGroupOwner(&m_RadioGroupOption);
	m_RadioOptionPut.SetRadioGroupOwner(&m_RadioGroupOption);

//	const int controlWidth = m_xc + m_firstColumnWidth + hgap + m_firstColumnWidth;
//	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
#endif
}

#ifndef TAKION_NO_OPTIONS

void BaseSampleDialogSettingOption::ToolTipsEnabled(bool enable)
{
	BaseSampleDialogSettingSymbol::ToolTipsEnabled(enable);
	m_CheckBoxOption.EnableTooltips(enable, m_toolTip);
	m_RadioOptionCall.EnableTooltips(enable, m_toolTip);
	m_RadioOptionPut.EnableTooltips(enable, m_toolTip);
}

void BaseSampleDialogSettingOption::HideOptionControls(bool hide)
{
	const int intHide = hide ? SW_HIDE : SW_SHOW;
	m_CheckBoxOption.ShowWindow(intHide);
	m_RadioOptionCall.ShowWindow(intHide);
	m_RadioOptionPut.ShowWindow(intHide);
	m_ComboBoxOptionSuffix.ShowWindow(intHide);
	m_SpinOptionStrikePrice.ShowWindow(intHide);
	m_DateTimeOptionExpiration.ShowWindow(intHide);

	m_GroupOption.ShowWindow(intHide);
	m_StaticOptionSuffix.ShowWindow(intHide);
	m_StaticOptionStrikePrice.ShowWindow(intHide);
	m_StaticOptionExpiration.ShowWindow(intHide);
}

BOOL BaseSampleDialogSettingOption::OnInitDialog()
{
	m_optionMode = false;
	m_optionSuffix = '\0';
	m_optionCall = false;
	m_expirationYear = m_expirationMonth = m_expirationDay = 0;
	m_strikePrice.SetZero();
	m_RadioOptionCall.SetInvalid(true);
	m_RadioOptionPut.SetInvalid(true);

	BOOL ret = BaseSampleDialogSettingSymbol::OnInitDialog();

	m_CheckBoxOption.ResizeToFitText(0);
	m_RadioOptionCall.ResizeToFitText(0);
	m_RadioOptionPut.ResizeToFitText(0);

	m_ComboBoxOptionSuffix.AddString(nullptr);
	m_ComboBoxOptionSuffix.FillCharRange('A', 'Z', false);
	m_ComboBoxOptionSuffix.FillCharRange('0', '9', false);
	m_ComboBoxOptionSuffix.SetCurSel(0);
	
	m_RadioOptionPut.SetCheck(BST_CHECKED);

	m_ComboBoxOptionSuffix.SetInvalid(true);
	m_SpinOptionStrikePrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
	m_DateTimeOptionExpiration.SetBkColor(m_visualBase->GetGrayColor());

	UpdateOptionExpirationValue();
	return ret;
}

void BaseSampleDialogSettingOption::DoDataExchange(CDataExchange* pDX)
{
	BaseSampleDialogSettingSymbol::DoDataExchange(pDX);
	DDX_Control(pDX, checkBoxSymbolOptionId, m_CheckBoxOption);
	DDX_Control(pDX, radioSymbolOptionCallId, m_RadioOptionCall);
	DDX_Control(pDX, radioSymbolOptionPutId, m_RadioOptionPut);
	DDX_Control(pDX, comboBoxSymbolOptionSuffixId, m_ComboBoxOptionSuffix);
	DDX_Control(pDX, spinSymbolOptionStrikePriceId, m_SpinOptionStrikePrice);
	DDX_Control(pDX, datetimeSymbolOptionExpirationId, m_DateTimeOptionExpiration);

	DDX_Control(pDX, groupSymbolOptionId, m_GroupOption);
	DDX_Control(pDX, staticSymbolOptionSuffixId, m_StaticOptionSuffix);
	DDX_Control(pDX, staticSymbolOptionStrikePriceId, m_StaticOptionStrikePrice);
	DDX_Control(pDX, staticSymbolOptionExpirationId, m_StaticOptionExpiration);
}

BEGIN_MESSAGE_MAP(BaseSampleDialogSettingOption, BaseSampleDialogSettingSymbol)
//	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(checkBoxSymbolOptionId, OnOption)
	ON_BN_CLICKED(radioSymbolOptionCallId, OnOptionCall)
	ON_BN_CLICKED(radioSymbolOptionPutId, OnOptionPut)
	ON_CBN_SELCHANGE(comboBoxSymbolOptionSuffixId, OnSelchangeOptionSuffix)
	ON_NOTIFY(DTN_DROPDOWN, datetimeSymbolOptionExpirationId, OnCalendarDropdown)
	ON_NOTIFY(DTN_CLOSEUP, datetimeSymbolOptionExpirationId, OnCalendarCloseup)
	ON_NOTIFY(DTN_DATETIMECHANGE, datetimeSymbolOptionExpirationId, OnExpirationDateChange)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void BaseSampleDialogSettingOption::OnSelchangeOptionSuffix()
{
	int sel = m_ComboBoxOptionSuffix.GetCurSel();
	const char optionSuffix = sel >= 0 ? (char)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		m_ComboBoxOptionSuffix.GetItemDataPtr(sel) : '\0';
	if(optionSuffix != m_optionSuffix)
	{
		m_optionSuffix = optionSuffix;
		m_modified = true;
	}
}

void BaseSampleDialogSettingOption::OnCalendarDropdown(NMHDR* pNMHDR, LRESULT* pResult)
{
	UGUI_IdleInterrupt(true, 4);
	*pResult = 0;
}

void BaseSampleDialogSettingOption::OnCalendarCloseup(NMHDR* pNMHDR, LRESULT* pResult)
{
	UGUI_IdleInterrupt(false, 4);
	*pResult = 0;
}

bool BaseSampleDialogSettingOption::GetOptionValues(unsigned __int64& rootSymbol, unsigned __int64& underlierSymbol, unsigned __int64& optionBlock, unsigned short& optionType) const
{
	if(AreSettingsValid())
	{
		optionType = 0;
//		rootSymbol = m_numericSymbol;
		rootSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(m_symbol.c_str(), sizeof(unsigned __int64) - 1);
		if(isOptionUsed())
		{
			if(m_optionSuffix)
			{
				underlierSymbol = rootSymbol;
				OptionKey::AddSuffixToSymbol(rootSymbol, m_optionSuffix);
			}
			else
			{
				underlierSymbol = 0;
			}
			OptionKey optionKey(m_strikePrice.GetCompactPrice(),
				m_optionCall,
				m_optionSuffix,
				m_expirationDay,
				m_expirationMonth,
				m_expirationYear);
			optionBlock = optionKey.CalculateOptionBlock();
		}
		else
		{
			underlierSymbol = 0;
			optionBlock = 0;
		}
		return true;
	}
	return false;
}

void BaseSampleDialogSettingOption::OnExpirationDateChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(UpdateOptionExpirationValue())
	{
		m_modified = true;
	}
	*pResult = 0;
}

bool BaseSampleDialogSettingOption::UpdateOptionExpirationValue()
{
	bool ret = false;
	SYSTEMTIME d;
	m_DateTimeOptionExpiration.GetTime(&d);
	unsigned char token = (unsigned char)d.wMonth;
	if(token != m_expirationMonth)
	{
		m_expirationMonth = token;
		ret = true;
	}
	token = (unsigned char)d.wDay;
	if(token != m_expirationDay)
	{
		m_expirationDay = token;
		ret = true;
	}
	token = (unsigned char)(d.wYear % 100);
	if(token != m_expirationYear)
	{
		m_expirationYear = token;
		ret = true;
	}
	if(ret)OptionValuesChanged();
	return ret;
}

void BaseSampleDialogSettingOption::SetOptionCall(const bool call)
{
	if(call != m_optionCall)
	{
		m_optionCall = call;
		m_modified = true;
		OptionValuesChanged();
	}
}

void BaseSampleDialogSettingOption::OnOptionCall()
{
	SetOptionCall(m_RadioOptionCall.GetCheck() == BST_CHECKED);
}

void BaseSampleDialogSettingOption::OnOptionPut()
{
	SetOptionCall(m_RadioOptionPut.GetCheck() == BST_UNCHECKED);
}

void BaseSampleDialogSettingOption::OnOption()
{
	const bool optionMode = m_CheckBoxOption.GetCheck() == BST_CHECKED;
	if(optionMode != m_optionMode)
	{
		m_optionMode = optionMode;
		m_modified = true;
		OptionModeChanged();
	}
}

void BaseSampleDialogSettingOption::SetCallPut(bool call)
{
	if(m_RadioOptionCall.m_hWnd)
	{
		if(call)
		{
			m_RadioOptionPut.SetCheck(BST_UNCHECKED);
			m_RadioOptionCall.SetCheck(BST_CHECKED);
		}
		else
		{
			m_RadioOptionCall.SetCheck(BST_UNCHECKED);
			m_RadioOptionPut.SetCheck(BST_CHECKED);
		}
		OnOptionCall();
	}
}

void BaseSampleDialogSettingOption::SetStrikePrice(const Price& strikePrice)
{
	if(strikePrice != m_strikePrice)
	{
		m_strikePrice = strikePrice;
		FillSpinControlFromPrice(m_strikePrice, m_SpinOptionStrikePrice, 100, false);
	}
}

void BaseSampleDialogSettingOption::SetExpirationDate(unsigned int date)
{
//	unsigned int date = extensionConfig->m_positionActionOptionExpiration.GetValue();
	if(!date)
	{
		unsigned short year;
//		unsigned char month;
//		unsigned char day;
		U_GetDateTimeTokensDateOnly(TL_GetTodaysDateInShiftFormat(), year, m_expirationMonth, m_expirationDay);
		m_expirationYear = year % 100;
	}
	else
	{
		unsigned int token = date / 100;
		const unsigned char expirationDay = (unsigned char)(date - token * 100);
		date = token;
		token = date / 100;
		const unsigned char expirationMonth = (unsigned char)(date - token * 100);
		const unsigned char expirationYear = (unsigned char)token;
//	if(expirationDay != m_expirationDay || expirationMonth != m_expirationMonth || expirationYear != m_expirationYear)
//	{
		m_expirationDay = expirationDay;
		m_expirationMonth = expirationMonth;
		m_expirationYear = expirationYear;
	}
	if(m_DateTimeOptionExpiration.m_hWnd)
	{
		SYSTEMTIME d;
		d.wDay = m_expirationDay;
		d.wMonth = m_expirationMonth;
		d.wYear = 2000 + m_expirationYear;
		d.wDayOfWeek = d.wHour = d.wMilliseconds = d.wMinute = d.wSecond = 0;
		m_DateTimeOptionExpiration.SetTime(&d);
	}
}

//void BaseSampleDialogSettingOption::SetOptionValues(unsigned int date)
void BaseSampleDialogSettingOption::SetOptionValues(bool useOption, char suffix, bool call, const Price& strikePrice, unsigned int expirationDate)
{
	SetCallPut(call);//extensionConfig->m_mdOptionCall.GetValue());
	
	const int found = m_ComboBoxOptionSuffix.FindItem((const void*)suffix);//extensionConfig->m_mdOptionSuffix.GetValue());
	m_ComboBoxOptionSuffix.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeOptionSuffix();

	SetStrikePrice(strikePrice);//extensionConfig->m_mdOptionStrikePrice.GetValue());

	SetExpirationDate(expirationDate);//extensionConfig->m_mdOptionExpiration.GetValue());

	m_CheckBoxOption.SetCheck(useOption ? BST_CHECKED : BST_UNCHECKED);//extensionConfig->m_mdOption.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnOption();
}

bool BaseSampleDialogSettingOption::isExpirationValid() const
{
	return !TL_IsDatePassed((unsigned int)m_expirationYear + 2000, m_expirationMonth, m_expirationDay);
}

bool BaseSampleDialogSettingOption::AreOptionSettingsValid() const
{
//	return !m_optionMode || (!m_strikePrice.isZero() && isExpirationValid());
	return !m_optionMode || !m_strikePrice.isZero();
}

bool BaseSampleDialogSettingOption::AreSettingsValid() const
{
	return BaseSampleDialogSettingSymbol::AreSettingsValid() && AreOptionSettingsValid();
}

bool BaseSampleDialogSettingOption::OptionModeChanged()
{
	const bool option = isOptionUsed();
	m_ComboBoxOptionSuffix.SetInvalid(!option);
	m_SpinOptionStrikePrice.SetBkColorEnabled(option ? m_strikePrice.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
	m_DateTimeOptionExpiration.SetBkColor(option ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
//	m_RadioOptionCall.Invalidate(FALSE);
//	m_RadioOptionPut.Invalidate(FALSE);
	m_RadioOptionCall.SetInvalid(!option);
	m_RadioOptionPut.SetInvalid(!option);
	OptionValuesChanged();
	return option;
}

LRESULT BaseSampleDialogSettingOption::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSymbolOptionStrikePriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinOptionStrikePrice, 100);
			const bool priceZero = price.isZero();
			const bool changed = isOptionUsed() && m_strikePrice.isZero() != priceZero;
			if(changed)m_SpinOptionStrikePrice.SetBkColorEnabled(priceZero ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
			if(price != m_strikePrice)
			{
				m_strikePrice = price;
				m_modified = true;
			}
			if(changed)OptionValuesChanged();
		}
		break;
/*
		default:
		BaseSampleDialogSettingSymbol::OnSpinChange(w, l);
		break;
*/
	}
	return 0;
}
/*
HBRUSH BaseSampleDialogSettingOption::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case radioSymbolOptionCallId:
		case radioSymbolOptionPutId:
		if(!isOptionUsed())//!m_optionMode || SYMT_SYMBOL_STRING != m_symbolType)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;
	}
	return BaseSampleDialogSettingSymbol::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/
#endif
