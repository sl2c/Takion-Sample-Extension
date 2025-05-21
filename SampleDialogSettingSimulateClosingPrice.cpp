#include "stdafx.h"
#include "SampleDialogSettingSimulateClosingPrice.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateClosingPrice::SampleDialogSettingSimulateClosingPrice(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateOptionSource(mainWnd, parentTab, "ClsPrice"),
	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4)
{
	AddPriceSpinCells(m_SpinPrice);

	m_bottomControl += vgap;
	int sideWidth = 35;
	const int priceWidth = 75;

	int controlWidth = m_xc + priceWidth + hgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimClosingPriceId);
	AddStaticIndirect("ClosePrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimClosingPriceId);

	m_bottomControl += spinHeight;
	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}

void SampleDialogSettingSimulateClosingPrice::AddPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}

BOOL SampleDialogSettingSimulateClosingPrice::OnInitDialog()
{
//	m_messageType = MT_LRP;

	BOOL ret = BaseDialogSettingSimulateOptionSource::OnInitDialog();
	
//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateClosingPrice::UpdateSettings()
{
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simClsPriceSymbolType.SetValue(symbolType);

	extensionConfig->m_simClsPriceSymbol.SetValue(m_symbol);
	extensionConfig->m_simClsPriceSymbolMap.SetValue(m_symbolMap);

//	unsigned char source = m_source;
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(m_source);

	extensionConfig->m_simClsPriceSource.SetValue(source);
/*
	unsigned char messageType = m_messageType;
	if(messageType >= MT_Count)messageType = MT_Count - 1;
	extensionConfig->m_simClsPriceMessageType.SetValue(messageType);
*/
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_simClsPriceOption.SetValue(m_optionMode);
	extensionConfig->m_simClsPriceOptionCall.SetValue(m_optionCall);
	extensionConfig->m_simClsPriceOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_simClsPriceOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_simClsPriceOptionExpiration.SetValue(GetExpirationDate());
#endif

	extensionConfig->m_simClsPrice.SetValue(m_price);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateClosingPrice::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simClsPriceSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simClsPriceSymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simClsPriceSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

//	unsigned char source = extensionConfig->m_simClsPriceSource.GetValue();
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(extensionConfig->m_simClsPriceSource.GetValue());
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}
/*
	unsigned char messageType = extensionConfig->m_simClsPriceMessageType.GetValue();
	if(messageType >= MT_Count)messageType = MT_Count - 1;
	if(messageType != m_messageType)
	{
		m_messageType = messageType;
		m_ComboBoxMessageType.SetCurSel(m_messageType);
	}
*/
	const Price& price = extensionConfig->m_simClsPrice.GetValue();
	if(price != m_price)
	{
		m_price = price;
		m_price.Canonicalize();
		FillSpinControlFromPrice(m_price, m_SpinPrice, 10000, false);
		FillPriceFromSpinControl(m_price, m_SpinPrice, 10000);
//		m_SpinPriceLow.SetBkColorEnabled(m_priceLow.isZero() ? GetSysColor(COLOR_WINDOW) : m_takionVisual->GetWarningColor());
	}

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_simClsPriceOption.GetValue(),
		extensionConfig->m_simClsPriceOptionSuffix.GetValue(),
		extensionConfig->m_simClsPriceOptionCall.GetValue(),
		extensionConfig->m_simClsPriceOptionStrikePrice.GetValue(),
		extensionConfig->m_simClsPriceOptionExpiration.GetValue());
/*
	SetCallPut(extensionConfig->m_simClsPriceOptionCall.GetValue());
	
	int found = m_ComboBoxOptionSuffix.FindItem((const void*)extensionConfig->m_simClsPriceOptionSuffix.GetValue());
	m_ComboBoxOptionSuffix.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeOptionSuffix();

	SetStrikePrice(extensionConfig->m_simClsPriceOptionStrikePrice.GetValue());

	SetExpirationDate(extensionConfig->m_simClsPriceOptionExpiration.GetValue());

	m_CheckBoxOption.SetCheck(extensionConfig->m_simClsPriceOption.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnOption();
*/
#endif

	EnableApplyButton(false);
	EnableSendMessageButton();

	m_modified = false;

	if(price != m_price)
	{
		m_modified = true;
	}
}

const Message* SampleDialogSettingSimulateClosingPrice::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
		const char* symbol = m_symbol.c_str();
#ifndef TAKION_NO_OPTIONS
		const bool optionUsed = isOptionUsed();
		unsigned __int64 futureNumericSymbol;
		bool future = !optionUsed && *symbol == '/';
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

		if(optionUsed)
		{
			OptionKey optionKey(m_strikePrice.GetCompactPrice(),
				m_optionCall,
				m_optionSuffix,
				m_expirationDay,
				m_expirationMonth,
				m_expirationYear);
			m_closeOptionMd.m_closePriceDollar = m_price.GetDollars();
			m_closeOptionMd.m_closePriceFraction = m_price.GetDollarFraction();
			U_CopyAndPad(m_closeOptionMd.m_symbol, sizeof(m_closeOptionMd.m_symbol), symbol, '\0');
			m_closeOptionMd.m_time = TL_GetCurrentMillisecond();
			m_closeOptionMd.m_optionBlock = OptionBlock(optionKey.CalculateOptionBlock());
			return &m_closeOptionMd;
		}
		else if(future)
		{
			m_closeFutureMd.m_closePriceDollar = m_price.GetDollars();
			m_closeFutureMd.m_closePriceFraction = m_price.GetDollarFraction();
			U_CopyAndPad(m_closeFutureMd.m_symbol, sizeof(m_closeFutureMd.m_symbol), symbol, '\0');
			m_closeFutureMd.m_time = TL_GetCurrentMillisecond();
			return &m_closeFutureMd;
		}
		else
#endif
		switch(m_source)
		{
			case ST_MARKET_DATA:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				m_closeMd.m_closePriceDollar = m_price.GetDollars();
				m_closeMd.m_closePriceFraction = m_price.GetDollarFraction();
				U_CopyAndPad(m_closeMd.m_symbol, sizeof(m_closeMd.m_symbol), symbol, '\0');
				m_closeMd.m_time = TL_GetCurrentMillisecond();
				return &m_closeMd;
			}
			else
			{
				m_closeMdSm.m_closePriceDollar = m_price.GetDollars();
				m_closeMdSm.m_closePriceFraction = m_price.GetDollarFraction();
				m_closeMdSm.m_symbol = m_symbolMap;
				m_closeMdSm.m_time = TL_GetCurrentMillisecond();
				return &m_closeMdSm;
			}
			break;

			case ST_MARKET_SORTER:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				m_closeMs.m_closePriceDollar = m_price.GetDollars();
				m_closeMs.m_closePriceFraction = m_price.GetDollarFraction();
				U_CopyAndPad(m_closeMs.m_symbol, sizeof(m_closeMs.m_symbol), symbol, '\0');
				m_closeMs.m_time = TL_GetCurrentMillisecond();
				return &m_closeMs;
			}
			else
			{
				m_closeMsSm.m_closePriceDollar = m_price.GetDollars();
				m_closeMsSm.m_closePriceFraction = m_price.GetDollarFraction();
				m_closeMsSm.m_symbol = m_symbolMap;
				m_closeMsSm.m_time = TL_GetCurrentMillisecond();
				return &m_closeMsSm;
			}
			break;
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulateClosingPrice::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateOptionSource::DoDataExchange(pDX);
//	DDX_Control(pDX, comboBoxMessageId, m_ComboBoxMessageType);
	DDX_Control(pDX, spinSimClosingPriceId, m_SpinPrice);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateClosingPrice, BaseDialogSettingSimulateOptionSource)
//	ON_CBN_SELCHANGE(comboBoxMessageId, OnSelchangeMessageType)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

LRESULT SampleDialogSettingSimulateClosingPrice::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimClosingPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPrice, 10000);
			if(price != m_price)
			{
				m_price = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		default:
		BaseDialogSettingSimulateOptionSource::OnSpinChange(w, l);
		break;
	}
	return 0;
}
/*
void SampleDialogSettingSimulateClosingPrice::OnSelchangeMessageType()
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
*/