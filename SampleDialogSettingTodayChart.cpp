#include "stdafx.h"
#include "SampleDialogSettingTodayChart.h"
#include "ExtensionDerivedConfig.h"
#include "ExtensionSample.h"

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool UnappliedChartMessageReader::ProcessDataToken(const char* cursor, unsigned short length) const
{
	return m_window->ProcessUnappliedMessage((const Message*)cursor);
}

SampleDialogSettingTodayChart::SampleDialogSettingTodayChart(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab)://, const Account* account):
	BaseDialogSettingChart(mainWnd, parentTab, "TodayChart", 55),
	m_security(nullptr),
	m_unappliedMessageReader(this),
	m_currentMinute(0),
	m_chartMinute(0)
//	m_historicalChart(nullptr),
{
	int x = 0;
	int y = m_firstColumnBottom;
	int xc = x + hgap;
	AddButtonIndirect("Load", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, xc, y, m_width, comboBoxHeight, buttonLoadId);

	m_firstColumnBottom += comboBoxHeight + vgap;
}

bool SampleDialogSettingTodayChart::ProcessUnappliedMessage(const Message* message)
{
	switch(message->m_type)
	{
		case TM_MS_TRADE_SHORT_INTERNAL:
		{
			const TMsgMsTradeShortInternal* msg = (const TMsgMsTradeShortInternal*)message;
			AddPrint(Price(msg->m_price), msg->m_size, msg->m_marketCenterID, msg->m_printFlags, msg->m_priceApplicableToChart);
		}
		return true;

		case TM_MS_TRADE_LONG_INTERNAL:
		{
			const TMsgMsTradeLongInternal* msg = (const TMsgMsTradeLongInternal*)message;
			AddPrint(Price(msg->m_priceDollars, msg->m_priceFraction), msg->m_size, msg->m_marketCenterID, msg->m_printFlags, msg->m_priceApplicableToChart);
		}
		return true;
/*
		case TM_STOCK_ADD_PRINT:
		{
			const TMsgStockAddPrint* msg = (const TMsgStockAddPrint*)message;
			PropagateTodaysChartToMinute((unsigned short)(msg->m_time / 60000));
			AddPrint(Price(msg->m_dollars, msg->m_dollarFraction), msg->m_quantity, msg->m_time, msg->m_source, 31 + 64, true);//msg->m_printFlags, msg->m_priceApplicableToChart);
		}
		return true;
*/
		case TM_EQUITY_NEW_MINUTE:
		{
			const unsigned short minute = ((const TMsgNewMinute*)message)->m_minute;
			PropagateTodaysChartToMinute(minute);
		}
		return false;

		default:
		return false;

	}
}

void SampleDialogSettingTodayChart::AddPrint(const Price& price, unsigned int size, char marketCenterId, unsigned char printFlags, bool priceApplicableToChart)//printFlags: 4 - price; 8 - volume, 16 - high/low
{
//priceApplicableToChart = true if the price was close enough to NBBO
//The reason why (printFlags & 4) == 0 (price is not applicable to charts) is that the sale condition dictates it. One such sale condition is 'I' - odd lot - size less than the round lot.
	bool volumeApplicable = (printFlags & 8) != 0;
	bool priceApplicable = priceApplicableToChart && (printFlags & 4) != 0;
	if(priceApplicable || volumeApplicable)
	{
		unsigned int volume = 0;
		if(volumeApplicable)
		{
			volume = size;
			m_totalVolume += volume;
			InvalidateTotalValueRect(listBoxHistChartId, HCI_VOLUME);
		}
		Price chartPrice;//zero
		if(priceApplicable)
		{
			chartPrice = price;
		}
		if(m_chartMinute < m_currentMinute)
		{
			m_chartMinute = m_currentMinute;
			ChartPoint* cp = new ChartPoint(m_chartMinute);//(chartPrice, chartPrice, chartPrice, chartPrice, chartPrice * size, m_chartMinute, 0, size);
			cp->AddPrint(chartPrice, volume, printFlags);
			m_ListBoxChart.AddString((const char*)cp);
			InvalidateTotalValueRect(listBoxHistChartId, HCI_DATE);
		}
		else if(m_chartMinute)
		{
			unsigned int count = m_ListBoxChart.GetItemCount();
			if(count)
			{
				--count;
				ChartPoint* cp = (ChartPoint*)m_ListBoxChart.GetItemDataPtr(count);
				cp->AddPrint(chartPrice, volume, printFlags);
				m_ListBoxChart.InvalidateItem(count);
			}
			else
			{
				ChartPoint* cp = new ChartPoint(m_chartMinute);//(chartPrice, chartPrice, chartPrice, chartPrice, chartPrice * size, m_chartMinute, 0, size);
				cp->AddPrint(chartPrice, volume, printFlags);
				m_ListBoxChart.AddString((const char*)cp);
				InvalidateTotalValueRect(listBoxHistChartId, HCI_DATE);
			}
		}
	}
}

void SampleDialogSettingTodayChart::PropagateTodaysChartToMinute(const unsigned short minute)
{
	if(minute > m_currentMinute)
	{
		m_currentMinute = minute;
	}
}

void SampleDialogSettingTodayChart::RestoreLayout()
{
	m_symbolInitial = extensionConfig->m_tcSymbol.GetValue();
	m_StaticSymbol.SetWindowText(m_symbolInitial.c_str());

	m_decDigit = extensionConfig->m_tcDecDigit.GetValue();
	if(m_decDigit > 9)m_decDigit = 9;
	if(m_SpinDecDigit.SetValue(m_decDigit, true, true, false))
	{
		m_ListBoxChart.SetDecDigit(m_decDigit);
	}

	DoReset();
	DoLoad();
//	UpdateOpenAllEnabled();
	m_modified = false;
/*
	if(m_security && m_security->isChartLoaded())
	{
		FillListBox();
	}
*/
}

void SampleDialogSettingTodayChart::UpdateSettings()
{
	extensionConfig->m_tcSymbol.SetValue(m_symbolInitial);
	extensionConfig->m_tcDecDigit.SetValue(m_decDigit);
}

void SampleDialogSettingTodayChart::BeforeDestroy()
{
	m_security = nullptr;
	BaseDialogSettingChart::BeforeDestroy();
}

BOOL SampleDialogSettingTodayChart::OnInitDialog()
{
	m_security = nullptr;

	BOOL ret = BaseDialogSettingChart::OnInitDialog();

	return ret;
}

void SampleDialogSettingTodayChart::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingChart::DoDataExchange(pDX);
}

bool SampleDialogSettingTodayChart::CanFill() const
{
	return m_security && m_security->isChartLoaded();
}

void SampleDialogSettingTodayChart::SetSecurity(const Security* security)
{
	if(security != m_security)
	{
		if(m_security)
		{
			m_security->RemoveObserver(this);
			TD_ReleaseStock(m_security);//the same as m_security->DecrementUnsubscribable();
		}
		m_security = security;
		if(m_security)
		{
			m_security->IncrementUnsubscribable();
			m_unappliedMessageReader.SetLineBufferArray(m_security->GetUnappliedMessages());
			m_security->AddObserver(this);
		}
		FillListBox();
	}
}

bool SampleDialogSettingTodayChart::DoLoad()
{
	if(canLoad())
	{
		const unsigned __int64 numericSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(m_symbol.c_str(), sizeof(unsigned __int64) - 1);
		UpdateCurrentLoadSettingsAndSymbol();
		SetSecurity(TD_FindSortableEquityByNumericSymbolWithNoIncrementUnsubscribe(numericSymbol));
		return true;
	}
	return false;
}

void SampleDialogSettingTodayChart::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_EQUITY_UPDATE:
		m_unappliedMessageReader.ProcessAllData();
		break;

		case TM_EQUITY_CHART_LOADED:
		{
			const TMsgEquityChartLoaded* msg = (const TMsgEquityChartLoaded*)message;
			if(!strcmp(msg->m_symbol, m_symbol.c_str()))
			{
				FillListBox();
			}
		}
		break;

		default:
		BaseDialogSettingChart::Notify(message, from, info);
		break;
	}
}
