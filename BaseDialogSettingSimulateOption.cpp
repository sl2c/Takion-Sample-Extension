#include "stdafx.h"
#include "BaseDialogSettingSimulateOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulateOption::BaseDialogSettingSimulateOption(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseDialogSettingSimulateSymbolMap(mainWnd, parentTab, label)
#ifndef TAKION_NO_OPTIONS
	,m_CheckBoxOption("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_ComboBoxOptionSuffix(m_visualBase->GetGrayColor()),
	m_SpinOptionStrikePrice(99999999, 0, 0, 1000, 3, TakionSpin::charComma, 2),
	m_DateTimeOptionExpiration(448, 0xFFFFFFFF),
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

	const int optionGroupHeight = 56;
//	m_bottomControl += spinHeight + vgap;
	m_bottomControl += vgap;
	AddButtonIndirect("", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, m_xc, m_bottomControl, m_firstColumnWidth + m_firstColumnWidth, optionGroupHeight, groupOptionId);

	int x = m_xc + hgap;
	int y = m_bottomControl + groupOffset;
	AddButtonIndirect("Option", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, x, y, m_firstColumnHalfWidth + 5, checkBoxHeight, checkBoxOptionId);

	AddButtonIndirect("Call", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, x + m_firstColumnHalfWidth + 15 + hgap, y, m_firstColumnHalfWidth, checkBoxHeight, radioOptionCallId);
	AddButtonIndirect("Put", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, x + m_firstColumnHalfWidth + 15 + hgap + m_firstColumnHalfWidth + hgap, y, m_firstColumnHalfWidth, checkBoxHeight, radioOptionPutId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL|WS_GROUP, 0, x, y += checkBoxHeight + vgap, m_firstColumnHalfWidth, 400, comboBoxOptionSuffixId);
	AddStaticIndirect("Suffix", WS_VISIBLE | SS_NOPREFIX, 0, x + m_firstColumnHalfWidth, y + (comboBoxHeight - staticHeight) / 2, m_firstColumnHalfWidth, staticHeight, staticOptionSuffixId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x, y += comboBoxHeight + vgap, m_firstColumnWidth, spinHeight, spinOptionStrikePriceId);
	AddStaticIndirect("StrikePrice", WS_VISIBLE | SS_NOPREFIX, 0, x + m_firstColumnWidth, y + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOptionStrikePriceId);

	AddComponentIndirect("SysDateTimePick32", nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP | DTS_RIGHTALIGN, 0, x, y += spinHeight + vgap, m_firstColumnWidth, comboBoxHeight, datetimeOptionExpirationId);
	AddStaticIndirect("Expiration", WS_VISIBLE | SS_NOPREFIX, 0, x + m_firstColumnWidth, y + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOptionExpirationId);
	m_bottomControl += optionGroupHeight;
/*
#else
	m_bottomControl += spinHeight;
#endif
*/
	const int controlWidth = m_xc + m_firstColumnWidth + hgap + m_firstColumnWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
#endif
}

#ifndef TAKION_NO_OPTIONS

void BaseDialogSettingSimulateOption::SetCallPut(bool call)
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

void BaseDialogSettingSimulateOption::SetStrikePrice(const Price& strikePrice)
{
	if(strikePrice != m_strikePrice)
	{
		m_strikePrice = strikePrice;
		FillSpinControlFromPrice(m_strikePrice, m_SpinOptionStrikePrice, 100, false);
	}
}

void BaseDialogSettingSimulateOption::SetExpirationDate(unsigned int date)
{
//	unsigned int date = extensionConfig->m_positionActionOptionExpiration.GetValue();
	if(!date)
	{
		unsigned short year;
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

void BaseDialogSettingSimulateOption::SetOptionValues(bool useOption, char suffix, bool call, const Price& strikePrice, unsigned int expirationDate)
{
	SetCallPut(call);//extensionConfig->m_mdOptionCall.GetValue());
	
	int found = m_ComboBoxOptionSuffix.FindItem((const void*)suffix);//extensionConfig->m_mdOptionSuffix.GetValue());
	m_ComboBoxOptionSuffix.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeOptionSuffix();

	SetStrikePrice(strikePrice);//extensionConfig->m_mdOptionStrikePrice.GetValue());

	SetExpirationDate(expirationDate);//extensionConfig->m_mdOptionExpiration.GetValue());

	m_CheckBoxOption.SetCheck(useOption ? BST_CHECKED : BST_UNCHECKED);//extensionConfig->m_mdOption.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnOption();
}

void BaseDialogSettingSimulateOption::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingSimulateSymbolMap::ToolTipsEnabled(enable);
	m_CheckBoxOption.EnableTooltips(enable, m_toolTip);
}

BOOL BaseDialogSettingSimulateOption::OnInitDialog()
{
	m_optionMode = false;
	m_optionSuffix = '\0';
	m_optionCall = false;
	m_expirationYear = m_expirationMonth = m_expirationDay = 0;
	m_strikePrice.SetZero();

	BOOL ret = BaseDialogSettingSimulateSymbolMap::OnInitDialog();

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

void BaseDialogSettingSimulateOption::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSymbolMap::DoDataExchange(pDX);
	DDX_Control(pDX, checkBoxOptionId, m_CheckBoxOption);
	DDX_Control(pDX, radioOptionCallId, m_RadioOptionCall);
	DDX_Control(pDX, radioOptionPutId, m_RadioOptionPut);
	DDX_Control(pDX, comboBoxOptionSuffixId, m_ComboBoxOptionSuffix);
	DDX_Control(pDX, spinOptionStrikePriceId, m_SpinOptionStrikePrice);
	DDX_Control(pDX, datetimeOptionExpirationId, m_DateTimeOptionExpiration);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateOption, BaseDialogSettingSimulateSymbolMap)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(checkBoxOptionId, OnOption)
	ON_BN_CLICKED(radioOptionCallId, OnOptionCall)
	ON_BN_CLICKED(radioOptionPutId, OnOptionPut)
	ON_CBN_SELCHANGE(comboBoxOptionSuffixId, OnSelchangeOptionSuffix)
	ON_NOTIFY(DTN_DROPDOWN, datetimeOptionExpirationId, OnCalendarDropdown)
	ON_NOTIFY(DTN_CLOSEUP, datetimeOptionExpirationId, OnCalendarCloseup)
	ON_NOTIFY(DTN_DATETIMECHANGE, datetimeOptionExpirationId, OnExpirationDateChange)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

//!m_optionMode || SYMT_SYMBOL_STRING != m_symbolType
bool BaseDialogSettingSimulateOption::isOptionModeUsed() const
{
	return SYMT_SYMBOL_STRING == m_symbolType;
}

void BaseDialogSettingSimulateOption::OnSelchangeOptionSuffix()
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

void BaseDialogSettingSimulateOption::OnCalendarDropdown(NMHDR* pNMHDR, LRESULT* pResult)
{
	UGUI_IdleInterrupt(true, 4);
	*pResult = 0;
}

void BaseDialogSettingSimulateOption::OnCalendarCloseup(NMHDR* pNMHDR, LRESULT* pResult)
{
	UGUI_IdleInterrupt(false, 4);
	*pResult = 0;
}

void BaseDialogSettingSimulateOption::OnExpirationDateChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(UpdateOptionExpirationValue())
	{
		m_modified = true;
	}
	*pResult = 0;
}

bool BaseDialogSettingSimulateOption::UpdateOptionExpirationValue()
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
	return ret;
}

void BaseDialogSettingSimulateOption::SetOptionCall(const bool call)
{
	if(call != m_optionCall)
	{
		m_optionCall = call;
		m_modified = true;
	}
}

void BaseDialogSettingSimulateOption::OnOptionCall()
{
	SetOptionCall(m_RadioOptionCall.GetCheck() == BST_CHECKED);
}

void BaseDialogSettingSimulateOption::OnOptionPut()
{
	SetOptionCall(m_RadioOptionPut.GetCheck() == BST_UNCHECKED);
}

void BaseDialogSettingSimulateOption::OnOption()
{
	const bool optionMode = m_CheckBoxOption.GetCheck() == BST_CHECKED;
	if(optionMode != m_optionMode)
	{
		m_optionMode = optionMode;
		m_modified = true;
//		if(SYMT_SYMBOL_STRING == m_symbolType)
//		{
			OptionModeChanged();
//		}
	}
}

bool BaseDialogSettingSimulateOption::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateSymbolMap::isSendMessageButtonEnabled()
//		&& (!m_optionMode || !m_strikePrice.isZero());
		&& (!isOptionUsed() || !m_strikePrice.isZero());// || m_symbol.empty());
}

bool BaseDialogSettingSimulateOption::OptionModeChanged()
{
	const bool option = isOptionUsed();//m_optionMode && SYMT_SYMBOL_STRING == m_symbolType;
	m_ComboBoxOptionSuffix.SetInvalid(!option);
	m_SpinOptionStrikePrice.SetBkColorEnabled(option ? m_strikePrice.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
	m_DateTimeOptionExpiration.SetBkColor(option ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
	m_RadioOptionCall.Invalidate(FALSE);
	m_RadioOptionPut.Invalidate(FALSE);
	m_CheckBoxOption.SetForegroundColor(isOptionModeUsed() ? 0xFFFFFFFF : m_visualBase->GetGrayColor());
	UpdateMarketCenter();
	EnableSendMessageButton();
	return option;
}

LRESULT BaseDialogSettingSimulateOption::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinOptionStrikePriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinOptionStrikePrice, 100);
			bool changed = isOptionUsed() && m_strikePrice.isZero() != price.isZero();
			if(changed)m_SpinOptionStrikePrice.SetBkColorEnabled(m_strikePrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(price != m_strikePrice)
			{
				m_strikePrice = price;
				m_modified = true;
			}
			if(changed)EnableSendMessageButton();
		}
		break;

		default:
		BaseDialogSettingSimulateSymbolMap::OnSpinChange(w, l);
		break;
	}
	return 0;
}


void BaseDialogSettingSimulateOption::SymbolNotUsedChanged()
{
	BaseDialogSettingSimulateSymbolMap::SymbolNotUsedChanged();
	BaseDialogSettingSimulateOption::OptionModeChanged();
}

void BaseDialogSettingSimulateOption::SymbolTypeChanged()
{
	BaseDialogSettingSimulateSymbolMap::SymbolTypeChanged();
//	if(m_optionMode)
//	{
		OptionModeChanged();
//	}
//	m_CheckBoxOption.Invalidate(FALSE);
}

HBRUSH BaseDialogSettingSimulateOption::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case radioOptionCallId:
		case radioOptionPutId:
		if(!isOptionUsed())//!m_optionMode || SYMT_SYMBOL_STRING != m_symbolType)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;
/*
		case checkBoxOptionId:
		if(!isOptionModeUsed())//SYMT_SYMBOL_STRING != m_symbolType || )
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;
*/
	}
	return BaseDialogSettingSimulateSymbolMap::OnCtlColor(pDC, pWnd, nCtlColor);
}
#endif
