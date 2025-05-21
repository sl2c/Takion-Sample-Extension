#include "stdafx.h"
#include "SampleDialogSettingSimulateLrpLuld.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateLrpLuld::SampleDialogSettingSimulateLrpLuld(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateSource(mainWnd, parentTab, "PMI / LULD"),
	m_ComboBoxMessageType(false),
	m_SpinPriceLow(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPriceHigh(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
#ifdef LRPS
	m_messageType(MT_LRP)
#else
	m_messageType(MT_LULD)
#endif
{
	m_ComboBoxMessageType.SetNoSelectionColor(m_visualBase->GetWarningColor());

	AddPriceSpinCells(m_SpinPriceLow);
	AddPriceSpinCells(m_SpinPriceHigh);

	m_pmiMd.m_adjustment = 0;
	m_pmiMdSm.m_adjustment = 0;
	m_pmiMs.m_adjustment = 0;
	m_pmiMsSm.m_adjustment = 0;

	m_pmiMd.m_securityStatus = 0;
	m_pmiMdSm.m_securityStatus = 0;
	m_pmiMs.m_securityStatus = 0;
	m_pmiMsSm.m_securityStatus = 0;

	m_bottomControl += vgap;
	int sideWidth = 32;
	const int priceWidth = 75;

	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 50, comboBoxMessageId);
	AddStaticIndirect("Message", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticMessageId);

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	m_bottomControl += comboBoxHeight + vgap;

	controlWidth = m_xc + priceWidth + hgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimLrpLuldPriceLowId);
	AddStaticIndirect("Low", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimLrpLuldPriceLowId);

	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimLrpLuldPriceHighId);
	AddStaticIndirect("High", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimLrpLuldPriceHighId);

	m_bottomControl += spinHeight;

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}

void SampleDialogSettingSimulateLrpLuld::AddPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}

BOOL SampleDialogSettingSimulateLrpLuld::OnInitDialog()
{
	m_messageType =
#ifdef LRPS
		MT_LRP;
#else
		MT_LULD;
#endif
	BOOL ret = BaseDialogSettingSimulateSource::OnInitDialog();
	
//	m_SpinPriceLow.SetBkColorEnabled(m_takionVisual->GetWarningColor());
//	m_SpinPriceHigh.SetBkColorEnabled(m_takionVisual->GetWarningColor());

#ifdef LRPS
	m_ComboBoxMessageType.AddString("LRP");
#endif
	m_ComboBoxMessageType.AddString("LULD");
	m_ComboBoxMessageType.AddString("PMI");
	m_ComboBoxMessageType.AddString("YestHiLo");

	m_ComboBoxMessageType.SetCurSel(0);
	OnSelchangeMessageType();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateLrpLuld::UpdateSettings()
{
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simLrpLuldSymbolType.SetValue(symbolType);

	extensionConfig->m_simLrpLuldSymbol.SetValue(m_symbol);
	extensionConfig->m_simLrpLuldSymbolMap.SetValue(m_symbolMap);

//	unsigned char source = m_source;
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(m_source);
	extensionConfig->m_simLrpLuldSource.SetValue(source);

	unsigned char messageType = m_messageType;
	if(messageType >= MT_Count)messageType = MT_Count - 1;
	extensionConfig->m_simLrpLuldMessageType.SetValue(messageType);

	extensionConfig->m_simLrpLuldPriceLow.SetValue(m_priceLow);
	extensionConfig->m_simLrpLuldPriceHigh.SetValue(m_priceHigh);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateLrpLuld::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simLrpLuldSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simLrpLuldSymbolType.GetValue();
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	if(symbolType != m_symbolType)
	{
		m_symbolType = symbolType;
		switch(m_symbolType)
		{
			case SYMT_SYMBOL_STRING:
			m_RadioSymbolMap.SetCheck(BST_UNCHECKED);
			m_RadioSymbol.SetCheck(BST_CHECKED);
			break;

			case SYMT_SYMBOL_MAPPING:
			m_RadioSymbol.SetCheck(BST_UNCHECKED);
			m_RadioSymbolMap.SetCheck(BST_CHECKED);
			break;
		}
		SymbolTypeChanged();
	}
	const unsigned short& symbolMap = extensionConfig->m_simLrpLuldSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

//	unsigned char source = extensionConfig->m_simLrpLuldSource.GetValue();
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(extensionConfig->m_simLrpLuldSource.GetValue());
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}

	unsigned char messageType = extensionConfig->m_simLrpLuldMessageType.GetValue();
	if(messageType >= MT_Count)messageType = MT_Count - 1;
	if(messageType != m_messageType)
	{
		m_messageType = messageType;
		m_ComboBoxMessageType.SetCurSel(m_messageType);
	}

	const Price& priceLow = extensionConfig->m_simLrpLuldPriceLow.GetValue();
	if(priceLow != m_priceLow)
	{
		m_priceLow = priceLow;
		FillSpinControlFromPrice(m_priceLow, m_SpinPriceLow, 10000, false);
//		m_SpinPriceLow.SetBkColorEnabled(m_priceLow.isZero() ? GetSysColor(COLOR_WINDOW) : m_takionVisual->GetWarningColor());
	}

	const Price& priceHigh = extensionConfig->m_simLrpLuldPriceHigh.GetValue();
	if(priceHigh != m_priceHigh)
	{
		m_priceHigh = priceHigh;
		FillSpinControlFromPrice(m_priceHigh, m_SpinPriceHigh, 10000, false);
//		m_SpinPriceHigh.SetBkColorEnabled(m_priceHigh.isZero() ? GetSysColor(COLOR_WINDOW) : m_takionVisual->GetWarningColor());
	}
	EnableApplyButton(false);
	EnableSendMessageButton();

	m_modified = false;
}

const Message* SampleDialogSettingSimulateLrpLuld::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
		const char* symbol = m_symbol.c_str();
#ifndef TAKION_NO_OPTIONS
		unsigned __int64 futureNumericSymbol;
		bool future = *symbol == '/';
		if(future)
		{
			futureNumericSymbol = TU_GetCurrentFutureSymbol(symbol + 1);
			if(futureNumericSymbol)
			symbol = (const char*)&futureNumericSymbol;
		}
		else
		{
			future = TU_IsFutureSymbol(symbol) != 0;
		}
#endif

		switch(m_messageType)
		{
#ifdef LRPS
			case MT_LRP:
			switch(m_source)
			{
				case ST_MARKET_DATA:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					m_lrpMd.m_lowDollar = m_priceLow.GetDollars();
					m_lrpMd.m_lowFraction = m_priceLow.GetDollarFraction();
					m_lrpMd.m_highDollar = m_priceHigh.GetDollars();
					m_lrpMd.m_highFraction = m_priceHigh.GetDollarFraction();
					U_CopyAndPad(m_lrpMd.m_symbol, sizeof(m_lrpMd.m_symbol), symbol, '\0');
					m_lrpMd.m_time = TL_GetCurrentMillisecond();
					return &m_lrpMd;
				}
				else
				{
					m_lrpMdSm.m_lowDollar = m_priceLow.GetDollars();
					m_lrpMdSm.m_lowFraction = m_priceLow.GetDollarFraction();
					m_lrpMdSm.m_highDollar = m_priceHigh.GetDollars();
					m_lrpMdSm.m_highFraction = m_priceHigh.GetDollarFraction();
					m_lrpMdSm.m_symbol = m_symbolMap;
					m_lrpMdSm.m_time = TL_GetCurrentMillisecond();
					return &m_lrpMdSm;
				}
				break;

				case ST_MARKET_SORTER:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					m_lrpMs.m_lowDollar = m_priceLow.GetDollars();
					m_lrpMs.m_lowFraction = m_priceLow.GetDollarFraction();
					m_lrpMs.m_highDollar = m_priceHigh.GetDollars();
					m_lrpMs.m_highFraction = m_priceHigh.GetDollarFraction();
					U_CopyAndPad(m_lrpMs.m_symbol, sizeof(m_lrpMs.m_symbol), symbol, '\0');
					m_lrpMs.m_time = TL_GetCurrentMillisecond();
					return &m_lrpMs;
				}
				else
				{
					m_lrpMsSm.m_lowDollar = m_priceLow.GetDollars();
					m_lrpMsSm.m_lowFraction = m_priceLow.GetDollarFraction();
					m_lrpMsSm.m_highDollar = m_priceHigh.GetDollars();
					m_lrpMsSm.m_highFraction = m_priceHigh.GetDollarFraction();
					m_lrpMsSm.m_symbol = m_symbolMap;
					m_lrpMsSm.m_time = TL_GetCurrentMillisecond();
					return &m_lrpMsSm;
				}
				break;
			}
			break;
#endif
			case MT_LULD:
#ifndef TAKION_NO_OPTIONS
			if(future)
			{
				m_futureLuldMd.m_lowDollar = m_priceLow.GetDollars();
				m_futureLuldMd.m_lowFraction = m_priceLow.GetDollarFraction();
				m_futureLuldMd.m_highDollar = m_priceHigh.GetDollars();
				m_futureLuldMd.m_highFraction = m_priceHigh.GetDollarFraction();
				U_CopyAndPad(m_futureLuldMd.m_symbol, sizeof(m_futureLuldMd.m_symbol), symbol, '\0');
				m_futureLuldMd.m_time = TL_GetCurrentMillisecond();
				return &m_futureLuldMd;
			}
			else
#endif
			switch(m_source)
			{
				case ST_MARKET_DATA:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					m_luldMd.m_lowDollar = m_priceLow.GetDollars();
					m_luldMd.m_lowFraction = m_priceLow.GetDollarFraction();
					m_luldMd.m_highDollar = m_priceHigh.GetDollars();
					m_luldMd.m_highFraction = m_priceHigh.GetDollarFraction();
					U_CopyAndPad(m_luldMd.m_symbol, sizeof(m_luldMd.m_symbol), symbol, '\0');
					m_luldMd.m_time = TL_GetCurrentMillisecond();
					return &m_luldMd;
				}
				else
				{
					m_luldMdSm.m_lowDollar = m_priceLow.GetDollars();
					m_luldMdSm.m_lowFraction = m_priceLow.GetDollarFraction();
					m_luldMdSm.m_highDollar = m_priceHigh.GetDollars();
					m_luldMdSm.m_highFraction = m_priceHigh.GetDollarFraction();
					m_luldMdSm.m_symbol = m_symbolMap;
					m_luldMdSm.m_time = TL_GetCurrentMillisecond();
					return &m_luldMdSm;
				}
				break;

				case ST_MARKET_SORTER:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					m_luldMs.m_lowDollar = m_priceLow.GetDollars();
					m_luldMs.m_lowFraction = m_priceLow.GetDollarFraction();
					m_luldMs.m_highDollar = m_priceHigh.GetDollars();
					m_luldMs.m_highFraction = m_priceHigh.GetDollarFraction();
					U_CopyAndPad(m_luldMs.m_symbol, sizeof(m_luldMs.m_symbol), symbol, '\0');
					m_luldMs.m_time = TL_GetCurrentMillisecond();
					return &m_luldMs;
				}
				else
				{
					m_luldMsSm.m_lowDollar = m_priceLow.GetDollars();
					m_luldMsSm.m_lowFraction = m_priceLow.GetDollarFraction();
					m_luldMsSm.m_highDollar = m_priceHigh.GetDollars();
					m_luldMsSm.m_highFraction = m_priceHigh.GetDollarFraction();
					m_luldMsSm.m_symbol = m_symbolMap;
					m_luldMsSm.m_time = TL_GetCurrentMillisecond();
					return &m_luldMsSm;
				}
				break;
			}
			break;

			case MT_PMI:
			switch(m_source)
			{
				case ST_MARKET_DATA:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					m_pmiMd.m_lowDollar = m_priceLow.GetDollars();
					m_pmiMd.m_lowFraction = m_priceLow.GetDollarFraction();
					m_pmiMd.m_highDollar = m_priceHigh.GetDollars();
					m_pmiMd.m_highFraction = m_priceHigh.GetDollarFraction();
					U_CopyAndPad(m_pmiMd.m_symbol, sizeof(m_pmiMd.m_symbol), symbol, '\0');
					m_pmiMd.m_time = TL_GetCurrentMillisecond();
					return &m_pmiMd;
				}
				else
				{
					m_pmiMdSm.m_lowDollar = m_priceLow.GetDollars();
					m_pmiMdSm.m_lowFraction = m_priceLow.GetDollarFraction();
					m_pmiMdSm.m_highDollar = m_priceHigh.GetDollars();
					m_pmiMdSm.m_highFraction = m_priceHigh.GetDollarFraction();
					m_pmiMdSm.m_symbol = m_symbolMap;
					m_pmiMdSm.m_time = TL_GetCurrentMillisecond();
					return &m_pmiMdSm;
				}
				break;

				case ST_MARKET_SORTER:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					m_pmiMs.m_lowDollar = m_priceLow.GetDollars();
					m_pmiMs.m_lowFraction = m_priceLow.GetDollarFraction();
					m_pmiMs.m_highDollar = m_priceHigh.GetDollars();
					m_pmiMs.m_highFraction = m_priceHigh.GetDollarFraction();
					U_CopyAndPad(m_pmiMs.m_symbol, sizeof(m_pmiMs.m_symbol), symbol, '\0');
					m_pmiMs.m_time = TL_GetCurrentMillisecond();
					return &m_pmiMs;
				}
				else
				{
					m_pmiMsSm.m_lowDollar = m_priceLow.GetDollars();
					m_pmiMsSm.m_lowFraction = m_priceLow.GetDollarFraction();
					m_pmiMsSm.m_highDollar = m_priceHigh.GetDollars();
					m_pmiMsSm.m_highFraction = m_priceHigh.GetDollarFraction();
					m_pmiMsSm.m_symbol = m_symbolMap;
					m_pmiMsSm.m_time = TL_GetCurrentMillisecond();
					return &m_pmiMsSm;
				}
				break;
			}
			break;

			case MT_YESTERDAY_HILO:
			switch(m_source)
			{
				case ST_MARKET_DATA:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					m_yesterdaysHighLowMd.m_lowDollar = m_priceLow.GetDollars();
					m_yesterdaysHighLowMd.m_lowFraction = m_priceLow.GetDollarFraction();
					m_yesterdaysHighLowMd.m_highDollar = m_priceHigh.GetDollars();
					m_yesterdaysHighLowMd.m_highFraction = m_priceHigh.GetDollarFraction();
					U_CopyAndPad(m_yesterdaysHighLowMd.m_symbol, sizeof(m_yesterdaysHighLowMd.m_symbol), symbol, '\0');
					m_yesterdaysHighLowMd.m_time = TL_GetCurrentMillisecond();
					return &m_yesterdaysHighLowMd;
				}
				else
				{
					m_yesterdaysHighLowMdSm.m_lowDollar = m_priceLow.GetDollars();
					m_yesterdaysHighLowMdSm.m_lowFraction = m_priceLow.GetDollarFraction();
					m_yesterdaysHighLowMdSm.m_highDollar = m_priceHigh.GetDollars();
					m_yesterdaysHighLowMdSm.m_highFraction = m_priceHigh.GetDollarFraction();
					m_yesterdaysHighLowMdSm.m_symbol = m_symbolMap;
					m_yesterdaysHighLowMdSm.m_time = TL_GetCurrentMillisecond();
					return &m_yesterdaysHighLowMdSm;
				}
				break;

				case ST_MARKET_SORTER:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					m_yesterdaysHighLowMs.m_lowDollar = m_priceLow.GetDollars();
					m_yesterdaysHighLowMs.m_lowFraction = m_priceLow.GetDollarFraction();
					m_yesterdaysHighLowMs.m_highDollar = m_priceHigh.GetDollars();
					m_yesterdaysHighLowMs.m_highFraction = m_priceHigh.GetDollarFraction();
					U_CopyAndPad(m_yesterdaysHighLowMs.m_symbol, sizeof(m_yesterdaysHighLowMs.m_symbol), symbol, '\0');
					m_yesterdaysHighLowMs.m_time = TL_GetCurrentMillisecond();
					return &m_yesterdaysHighLowMs;
				}
				else
				{
					m_yesterdaysHighLowMsSm.m_lowDollar = m_priceLow.GetDollars();
					m_yesterdaysHighLowMsSm.m_lowFraction = m_priceLow.GetDollarFraction();
					m_yesterdaysHighLowMsSm.m_highDollar = m_priceHigh.GetDollars();
					m_yesterdaysHighLowMsSm.m_highFraction = m_priceHigh.GetDollarFraction();
					m_yesterdaysHighLowMsSm.m_symbol = m_symbolMap;
					m_yesterdaysHighLowMsSm.m_time = TL_GetCurrentMillisecond();
					return &m_yesterdaysHighLowMsSm;
				}
				break;
			}
			break;
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulateLrpLuld::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSource::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxMessageId, m_ComboBoxMessageType);
	DDX_Control(pDX, spinSimLrpLuldPriceLowId, m_SpinPriceLow);
	DDX_Control(pDX, spinSimLrpLuldPriceHighId, m_SpinPriceHigh);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateLrpLuld, BaseDialogSettingSimulateSource)
	ON_CBN_SELCHANGE(comboBoxMessageId, OnSelchangeMessageType)
/*
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(checkBoxOptionId, OnOption)
	ON_BN_CLICKED(radioOptionCallId, OnOptionCall)
	ON_BN_CLICKED(radioOptionPutId, OnOptionPut)
	ON_NOTIFY(DTN_DROPDOWN, datetimeOptionExpirationId, OnCalendarDropdown)
	ON_NOTIFY(DTN_CLOSEUP, datetimeOptionExpirationId, OnCalendarCloseup)
	ON_NOTIFY(DTN_DATETIMECHANGE, datetimeOptionExpirationId, OnExpirationDateChange)
*/
	ON_MESSAGE(WM_USER + 15, OnSpinChange)

END_MESSAGE_MAP()

LRESULT SampleDialogSettingSimulateLrpLuld::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimLrpLuldPriceLowId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceLow, 10000);
//			bool changed = m_priceLow.isZero() != price.isZero();
//			if(changed)m_SpinPriceLow.SetBkColorEnabled(m_priceLow.isZero() ? GetSysColor(COLOR_WINDOW) : m_takionVisual->GetWarningColor());
			if(price != m_priceLow)
			{
				m_priceLow = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimLrpLuldPriceHighId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceHigh, 10000);
//			bool changed = m_priceHigh.isZero() != price.isZero();
//			if(changed)m_SpinPriceHigh.SetBkColorEnabled(m_priceHigh.isZero() ? GetSysColor(COLOR_WINDOW) : m_takionVisual->GetWarningColor());
			if(price != m_priceHigh)
			{
				m_priceHigh = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		default:
		BaseDialogSettingSimulateSource::OnSpinChange(w, l);
		break;
	}
	return 0;
}

void SampleDialogSettingSimulateLrpLuld::OnSelchangeMessageType()
{
	int sel = m_ComboBoxMessageType.GetCurSel();
	if(sel < 0)sel = 0;//MT_LRP;
	else if(sel >= MT_Count)sel = MT_Count - 1;
	const unsigned char messageType = (unsigned char)sel;
	if(messageType != m_messageType)
	{
		m_messageType = messageType;
		m_modified = true;
	}
}
