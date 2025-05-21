#include "stdafx.h"
#include "SampleDialogSettingSimulateChartCorrection.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateChartCorrection::SampleDialogSettingSimulateChartCorrection(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateSymbolMap(mainWnd, parentTab, "ChartCorrection"),
//	m_SpinImbalance(99999999, 0, 0, 1000, 3, TakionSpin::charComma),
//	m_SpinMarketImbalance(99999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPriceStart(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPriceHigh(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPriceLow(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPriceEnd(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinMoneyTraded(99999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinVolume(99999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinMinute(1439, 0, 0, 10),
	m_volume(0),
	m_minute(0)
{
	SetBigSizeSpinCells(m_SpinVolume);
	SetPriceSpinCells(m_SpinPriceStart);
	SetPriceSpinCells(m_SpinPriceHigh);
	SetPriceSpinCells(m_SpinPriceLow);
	SetPriceSpinCells(m_SpinPriceEnd);
	SetPriceSpinCells(m_SpinMoneyTraded);

	m_bottomControl += vgap;
	const int sideWidth = 44;
	const int priceWidth = 82;// 75;

	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	const int w = controlWidth;

//	controlWidth += sideWidth;
//	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + priceWidth + hgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartCorrectionPriceStartId);
	AddStaticIndirect("StartPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartCorrectionPriceStartId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartCorrectionPriceHighId);
	AddStaticIndirect("HighPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartCorrectionPriceHighId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartCorrectionPriceLowId);
	AddStaticIndirect("LowPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartCorrectionPriceLowId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartCorrectionPriceEndId);
	AddStaticIndirect("EndPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartCorrectionPriceEndId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartCorrectionMoneyTradedId);
	AddStaticIndirect("MoneyTraded", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartCorrectionMoneyTradedId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartCorrectionVolumeId);
	AddStaticIndirect("Volume", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartCorrectionVolumeId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartCorrectionMinuteId);
	AddStaticIndirect("Minute", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartCorrectionMinuteId);
	m_bottomControl += spinHeight + vgap;

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}
/*
void SampleDialogSettingSimulateChartCorrection::SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}

void SampleDialogSettingSimulateChartCorrection::SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}
*/
BOOL SampleDialogSettingSimulateChartCorrection::OnInitDialog()
{
	m_priceStart.SetZero();
	m_priceHigh.SetZero();
	m_priceLow.SetZero();
	m_priceEnd.SetZero();
	m_moneyTraded.SetZero();
	m_volume = 0;
	m_minute = 0;

	BOOL ret = BaseDialogSettingSimulateSymbolMap::OnInitDialog();
	
//	m_SpinPriceLow.SetBkColorEnabled(m_takionVisual->GetWarningColor());
//	m_SpinPriceHigh.SetBkColorEnabled(m_takionVisual->GetWarningColor());


	return ret;
}

void SampleDialogSettingSimulateChartCorrection::UpdateSettings()
{

	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simChartCorrectionSymbolType.SetValue(symbolType);

	extensionConfig->m_simChartCorrectionSymbol.SetValue(m_symbol);
	extensionConfig->m_simChartCorrectionSymbolMap.SetValue(m_symbolMap);
/*
	unsigned char source = m_source;
	if(source >= ST_Count)source = ST_Count - 1;
	extensionConfig->m_simChartCorrectionSource.SetValue(source);
*/

	extensionConfig->m_simChartCorrectionPriceStart.SetValue(m_priceStart);
	extensionConfig->m_simChartCorrectionPriceHigh.SetValue(m_priceHigh);
	extensionConfig->m_simChartCorrectionPriceLow.SetValue(m_priceLow);
	extensionConfig->m_simChartCorrectionPriceEnd.SetValue(m_priceEnd);
	extensionConfig->m_simChartCorrectionMoneyTraded.SetValue(m_moneyTraded);
	extensionConfig->m_simChartCorrectionVolume.SetValue(m_volume);
	extensionConfig->m_simChartCorrectionMinute.SetValue(m_minute);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateChartCorrection::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simChartCorrectionSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simChartCorrectionSymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simChartCorrectionSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}
/*
	unsigned char source = extensionConfig->m_simChartCorrectionSource.GetValue();
	if(source >= ST_Count)source = ST_Count - 1;
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}
*/
	const Price& priceStart = extensionConfig->m_simChartCorrectionPriceStart.GetValue();
	if(priceStart != m_priceStart)
	{
		m_priceStart = priceStart;
		FillSpinControlFromPrice(m_priceStart, m_SpinPriceStart, 10000, false);
	}

	const Price& priceHigh = extensionConfig->m_simChartCorrectionPriceHigh.GetValue();
	if(priceHigh != m_priceHigh)
	{
		m_priceHigh = priceHigh;
		FillSpinControlFromPrice(m_priceHigh, m_SpinPriceHigh, 10000, false);
	}

	const Price& priceLow = extensionConfig->m_simChartCorrectionPriceLow.GetValue();
	if(priceLow != m_priceLow)
	{
		m_priceLow = priceLow;
		FillSpinControlFromPrice(m_priceLow, m_SpinPriceLow, 10000, false);
	}

	const Price& priceEnd = extensionConfig->m_simChartCorrectionPriceEnd.GetValue();
	if(priceEnd != m_priceEnd)
	{
		m_priceEnd = priceEnd;
		FillSpinControlFromPrice(m_priceEnd, m_SpinPriceEnd, 10000, false);
	}

	const Price& moneyTraded = extensionConfig->m_simChartCorrectionMoneyTraded.GetValue();
	if(moneyTraded != m_moneyTraded)
	{
		m_moneyTraded = moneyTraded;
		FillSpinControlFromPrice(m_moneyTraded, m_SpinMoneyTraded, 10000, false);
	}

	const unsigned int& volume = extensionConfig->m_simChartCorrectionVolume.GetValue();
	if(volume != m_volume)
	{
		m_volume = volume;
		m_SpinVolume.SetValue(m_volume, false, true, false);
	}

	const unsigned short& minute = extensionConfig->m_simChartCorrectionMinute.GetValue();
	if(minute != m_minute)
	{
		m_minute = minute;
		m_SpinMinute.SetMinute(m_minute, false);
	}

	EnableApplyButton(false);
	EnableSendMessageButton();

	m_modified = false;
}

const Message* SampleDialogSettingSimulateChartCorrection::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
//		static const unsigned int maxDollars = (1 << 18) - 1;
		if(m_symbolType == SYMT_SYMBOL_STRING)
		{
			U_CopyAndPad(m_msgMs.m_symbol, sizeof(m_msgMs.m_symbol), m_symbol.c_str(), '\0');
//			m_msgMs.m_time = TL_GetCurrentMillisecond();
			m_msgMs.m_first = m_priceStart.GetCompactPrice();
			m_msgMs.m_last = m_priceEnd.GetCompactPrice();
			m_msgMs.m_high = m_priceHigh.GetCompactPrice();
			m_msgMs.m_low = m_priceLow.GetCompactPrice();
			m_msgMs.m_moneyDollars = m_moneyTraded.GetDollars();
			m_msgMs.m_moneyFraction = m_moneyTraded.GetDollarFraction();
			m_msgMs.m_volume = m_volume;
			m_msgMs.m_minute = m_minute;

			return &m_msgMs;
		}
		else
		{
			m_msgMsSm.m_symbol = m_symbolMap;
//			m_msgMsSm.m_time = TL_GetCurrentMillisecond();
			m_msgMsSm.m_first = m_priceStart.GetCompactPrice();
			m_msgMsSm.m_last = m_priceEnd.GetCompactPrice();
			m_msgMsSm.m_high = m_priceHigh.GetCompactPrice();
			m_msgMsSm.m_low = m_priceLow.GetCompactPrice();
			m_msgMsSm.m_moneyDollars = m_moneyTraded.GetDollars();
			m_msgMsSm.m_moneyFraction = m_moneyTraded.GetDollarFraction();
			m_msgMsSm.m_volume = m_volume;
			m_msgMsSm.m_minute = m_minute;

			return &m_msgMsSm;
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulateChartCorrection::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSymbolMap::DoDataExchange(pDX);

	DDX_Control(pDX, spinSimChartCorrectionPriceStartId, m_SpinPriceStart);
	DDX_Control(pDX, spinSimChartCorrectionPriceHighId, m_SpinPriceHigh);
	DDX_Control(pDX, spinSimChartCorrectionPriceLowId, m_SpinPriceLow);
	DDX_Control(pDX, spinSimChartCorrectionPriceEndId, m_SpinPriceEnd);
	DDX_Control(pDX, spinSimChartCorrectionMoneyTradedId, m_SpinMoneyTraded);

	DDX_Control(pDX, spinSimChartCorrectionVolumeId, m_SpinVolume);
	DDX_Control(pDX, spinSimChartCorrectionMinuteId, m_SpinMinute);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateChartCorrection, BaseDialogSettingSimulateSymbolMap)
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

LRESULT SampleDialogSettingSimulateChartCorrection::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimChartCorrectionPriceStartId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceStart, 10000);
//			bool changed = m_priceStart.isZero() != price.isZero();
			if(price != m_priceStart)
			{
				m_priceStart = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartCorrectionPriceHighId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceHigh, 10000);
//			bool changed = m_priceHigh.isZero() != price.isZero();
			if(price != m_priceHigh)
			{
				m_priceHigh = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartCorrectionPriceLowId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceLow, 10000);
//			bool changed = m_priceHigh.isZero() != price.isZero();
			if(price != m_priceLow)
			{
				m_priceLow = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartCorrectionPriceEndId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceEnd, 10000);
//			bool changed = m_priceEnd.isZero() != price.isZero();
			if(price != m_priceEnd)
			{
				m_priceEnd = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartCorrectionMoneyTradedId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinMoneyTraded, 10000);
//			bool changed = m_priceEnd.isZero() != price.isZero();
			if(price != m_moneyTraded)
			{
				m_moneyTraded = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartCorrectionVolumeId:
		{
			const unsigned int& volume = m_SpinVolume.GetValue();
			if(volume != m_volume)
			{
				m_volume = volume;
				m_modified = true;
			}
		}
		break;

		case spinSimChartCorrectionMinuteId:
		{
			const unsigned short& minute = m_SpinMinute.GetMinute();
			if(minute != m_minute)
			{
				m_minute = minute;
				m_modified = true;
			}
		}
		break;

		default:
		BaseDialogSettingSimulateSymbolMap::OnSpinChange(w, l);
		break;
	}
	return 0;
}

