#include "stdafx.h"
#include "SampleDialogSettingHistChart.h"
#include "ExtensionDerivedConfig.h"
#include "ExtensionSample.h"

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum SampleSettingHistChartIds
{
	radioDayCountId = chartIdsCount,
	spinDayCountId,
	staticDayCountId,

	radioDateFromId,
	dateTimeDateFromId,
	staticDateFromId,

	staticFrequencyNameId,
	comboBoxFrequencyId,
	staticFrequencyId,

	buttonResetId,
	buttonSaveAllId,
	buttonOpenAllId,
/*
	staticSymbolSubscribedId,
	groupAccountId,
	comboBoxAccountId,
*/
	histChartIdsCount
};


SampleDialogSettingHistChart::SampleDialogSettingHistChart(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab)://, const Account* account):
	BaseDialogSettingChart(mainWnd, parentTab, "HistChart", 55),

	m_SpinDayCount(9999, 0, 0, 100, 3, TakionSpin::charComma),
	m_RadioDayCount("Chart duration measured by Business Days counted back from Today", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),//4, -3),
	m_RadioDateFrom("Chart duration measured from a Calendar Date counted forward to Today", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),//4, -3),
	m_DateFrom(333, 0xFFFFFFFF),
	m_ComboBoxFrequency(m_visualBase->GetWarningColor(), m_visualBase->GetPendingColor(), 0, 0xFFFFFFFF, false),

	m_dayCount(0),
	m_frequency(0),
	m_dateFrom(m_today),

	m_dayCountInitial(0),
	m_frequencyInitial(0),
	m_dateFromInitial(m_today),

	m_dayCountMode(false),
	m_dayCountModeInitial(false),

	m_historicalChart(nullptr),

//	m_marketSorterConnected(false),
//	m_marketSorterLoaded(false),
	m_historicalPrintConnected(false),
//	m_totalVolume(0),
/*
	m_listBoxBorderSize2(0),
	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),
*/
	m_historicalChartLoadingToSave(nullptr),
	m_dateFromLoadingToSave(0)
{
	m_ComboBoxFrequency.SetColorInvalid(m_visualBase->GetWarningColor());
	m_ComboBoxFrequency.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_SpinDayCount.AddSpinCell(1, false);
	m_SpinDayCount.AddSpinCell(100, true);

	m_SpinDayCount.SetBkColorEnabled(m_visualBase->GetGrayColor());

	int x = 0;
	int y = m_firstColumnBottom;
	int xc = x + hgap;
//	int width = 55;
/*
	AddStaticIndirect("Symbol", WS_VISIBLE | SS_NOPREFIX, 0, xc, y, width, staticHeight, staticSymbolNameId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xc, y += staticHeight, width, editHeight, editChartSymbolId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += editHeight, width, staticHeight, staticChartSymbolId);
*/
	AddButtonIndirect("Business Days", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += staticHeight + vgap, m_width, checkBoxHeight, radioDayCountId);
	AddButtonIndirect("From", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y + checkBoxHeight + spinHeight + staticHeight + vgap + vgap, m_width / 2, checkBoxHeight, radioDateFromId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_GROUP, 0, xc, y += checkBoxHeight, m_width, spinHeight, spinDayCountId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX | SS_RIGHT, 0, xc, y += spinHeight, m_width, staticHeight, staticDayCountId);

	AddComponentIndirect("SysDateTimePick32", nullptr, DTS_RIGHTALIGN|WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += checkBoxHeight + staticHeight + vgap + vgap, m_width, comboBoxHeight, dateTimeDateFromId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX | SS_RIGHT, 0, xc, y += comboBoxHeight, m_width, staticHeight, staticDateFromId);

	AddStaticIndirect("Frequency", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += staticHeight + vgap, m_width, staticHeight, staticFrequencyNameId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y += staticHeight, m_width, spinHeight * 10, comboBoxFrequencyId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += comboBoxHeight, m_width, staticHeight, staticFrequencyId);

	AddButtonIndirect("Reset", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, xc, y += staticHeight + vgap, m_width, comboBoxHeight, buttonResetId);
	AddButtonIndirect("Load", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, xc, y += comboBoxHeight + vgap, m_width, comboBoxHeight, buttonLoadId);
	AddButtonIndirect("Save All", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, xc, y += comboBoxHeight + vgap, m_width, comboBoxHeight, buttonSaveAllId);
	AddButtonIndirect("Open All", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, xc, y += comboBoxHeight + vgap, m_width, comboBoxHeight, buttonOpenAllId);

	m_firstColumnBottom = y + comboBoxHeight + vgap;

	m_RadioDayCount.SetRadioGroupOwner(&m_RadioGroupChartDuration);
	m_RadioDateFrom.SetRadioGroupOwner(&m_RadioGroupChartDuration);

/*
	y = vgap + staticHeight;
	xc += width + hgap;
	width = m_width - xc;

	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y, width, m_height - staticHeight - y, listBoxHistChartId);
//InvalidateAllTotalValueRects
	AddListBox(&m_ListBoxChart);
	SetMarketSorterConnected(TD_IsMarketSorterConnected());
	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
*/
	SetHistoricalPrintConnected(TD_IsHistoricalPrintConnected());

//	TD_GetNewEquityObservable().AddObserver(this);//to get notified about new Securities created
//	TD_GetAdminObservable().AddObserver(this);//to get notified about connect / disconnect
	TD_GetHistoricalChartObservable().AddObserver(this);//to get notified about loading the historical charts
}

SampleDialogSettingHistChart::~SampleDialogSettingHistChart()
{
//	TD_GetNewEquityObservable().RemoveObserver(this);
//	TD_GetAdminObservable().RemoveObserver(this);
	TD_GetHistoricalChartObservable().RemoveObserver(this);
}

void SampleDialogSettingHistChart::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingChart::ToolTipsEnabled(enable);
	m_SpinDayCount.EnableTooltips(enable, m_toolTip);
	m_RadioDayCount.EnableTooltips(enable, m_toolTip);
	m_RadioDateFrom.EnableTooltips(enable, m_toolTip);
	m_DateFrom.EnableTooltips(enable, m_toolTip);
	m_ComboBoxFrequency.EnableTooltips(enable, m_toolTip);
}


void SampleDialogSettingHistChart::RestoreLayout()
{
/*
	m_allChartFileName = extensionConfig->m_hcAllChartFileName.GetValue();
	m_symbol = extensionConfig->m_hcSymbol.GetValue();
	m_dayCountMode = extensionConfig->m_hcDayCountMode.GetValue();
	m_dayCount = extensionConfig->m_hcDayCount.GetValue();
	m_dateFrom = extensionConfig->m_hcDateFrom.GetValue();
	m_frequency = extensionConfig->m_hcFrequency.GetValue();
//	m_ComboBoxFrequency.SetCurrentItemOrdinal(m_frequencyInitial);
	if(!m_dateFrom)m_dateFrom = m_today;
*/

	m_allChartFileName = extensionConfig->m_hcAllChartFileName.GetValue();

	m_symbolInitial = extensionConfig->m_hcSymbol.GetValue();
	m_StaticSymbol.SetWindowText(m_symbolInitial.c_str());

	m_decDigit = extensionConfig->m_hcDecDigit.GetValue();
	if(m_decDigit > 9)m_decDigit = 9;
	if(m_SpinDecDigit.SetValue(m_decDigit, true, true, false))
	{
		m_ListBoxChart.SetDecDigit(m_decDigit);
	}

	m_dayCountModeInitial = extensionConfig->m_hcDayCountMode.GetValue();

	char num[33];
	m_dayCountInitial = extensionConfig->m_hcDayCount.GetValue();
	m_StaticDayCount.SetWindowText(U_UnsignedNumberToStrNoDecDigits(m_dayCountInitial, num, sizeof(num)));

	m_dateFromInitial = extensionConfig->m_hcDateFrom.GetValue();
	char* cursor = U_FormatDate(m_dateFromInitial, num, sizeof(num), '-', '\0', false, true);
	*cursor = '\0';
	m_StaticDateFrom.SetWindowText(num);
	m_StaticDateFrom.Invalidate(FALSE);
	m_DateFrom.SetBkColor(m_dateFrom >= m_today ? m_visualBase->GetWarningColor() : 0xFFFFFFFF);

	m_frequencyInitial = extensionConfig->m_hcFrequency.GetValue();
	m_ComboBoxFrequency.SetCurrentItemOrdinal(m_frequencyInitial);
	m_StaticFrequency.SetWindowText(m_frequencyInitial < (unsigned char)m_ComboBoxFrequency.GetItemCount() ? (const char*)m_ComboBoxFrequency.GetItemDataPtr(m_frequencyInitial) : "");

	if(!m_dateFromInitial)m_dateFromInitial = m_today;
	DoReset();

	DoLoad();
	UpdateOpenAllEnabled();
	m_modified = false;

//	if(m_historicalChart && m_historicalChart->isHistoricalChartLoaded())
//	{
		FillListBox();
//	}
}

bool SampleDialogSettingHistChart::CanFill() const
{
	return m_historicalChart && m_historicalChart->isHistoricalChartLoaded();
}

void SampleDialogSettingHistChart::UpdateSettings()
{
	extensionConfig->m_hcAllChartFileName.SetValue(m_allChartFileName);
//	extensionConfig->m_hcSymbol.SetValue(m_symbolInitial);
	extensionConfig->m_hcSymbol.SetValue(m_symbol);
	extensionConfig->m_hcDecDigit.SetValue(m_decDigit);
//	extensionConfig->m_hcDayCountMode.SetValue(m_dayCountModeInitial);
	extensionConfig->m_hcDayCountMode.SetValue(m_dayCountMode);
//	extensionConfig->m_hcDayCount.SetValue(m_dayCountInitial);
	extensionConfig->m_hcDayCount.SetValue(m_dayCount);
//	extensionConfig->m_hcDateFrom.SetValue(m_dateFromInitial);
	extensionConfig->m_hcDateFrom.SetValue(m_dateFrom);
//	extensionConfig->m_hcFrequency.SetValue(m_frequencyInitial);
	extensionConfig->m_hcFrequency.SetValue(m_frequency);
}

void SampleDialogSettingHistChart::BeforeDestroy()
{
	m_dayCount = 0;
	m_frequency = 0;
	m_dateFrom = m_today;

	m_dayCountInitial = 0;
	m_frequencyInitial = 0;
	m_dateFromInitial = m_today;
	m_dayCountMode = false;
	m_dayCountModeInitial = false;

	m_historicalChart = nullptr;
	m_historicalChartLoadingToSave = nullptr;
	m_allChartFile.close();
	m_allChartFileName.clear();
	m_symbolSet.clear();
	m_dateFromLoadingToSave = 0;

	m_ComboBoxFrequency.SetCurrentItemOrdinal(0xFFFFFFFF);

	BaseDialogSettingChart::BeforeDestroy();
}

BOOL SampleDialogSettingHistChart::OnInitDialog()
{
	m_dayCount = 0;
	m_frequency = 0;
	m_dateFrom = m_today;

	m_dayCountInitial = 0;
	m_frequencyInitial = 0;
	m_ComboBoxFrequency.SetCurrentItemOrdinal(0xFFFFFFFF);
	m_dateFromInitial = m_today;
	m_dayCountMode = false;
	m_dayCountModeInitial = false;

	m_historicalChart = nullptr;
	m_historicalChartLoadingToSave = nullptr;
	m_allChartFile.close();
	m_allChartFileName.clear();
	m_symbolSet.clear();
	m_dateFromLoadingToSave = 0;

	BOOL ret = BaseDialogSettingChart::OnInitDialog();

	m_RadioDayCount.ResizeToFitText(0);
	m_RadioDateFrom.ResizeToFitText(0);

	m_ComboBoxFrequency.AddString("");
	m_ComboBoxFrequency.AddString("Month");
	m_ComboBoxFrequency.AddString("Week");
	m_ComboBoxFrequency.AddString("Day");
	m_ComboBoxFrequency.AddString("Hour");
	m_ComboBoxFrequency.AddString("30 Min");
	m_ComboBoxFrequency.AddString("15 Min");
	m_ComboBoxFrequency.AddString("5 Min");
	m_ComboBoxFrequency.AddString("1 Min");

	m_ComboBoxFrequency.SetCurSel(0);
	OnSelchangeFrequency();

	SYSTEMTIME sysTime;
	memset(&sysTime, 0, sizeof(sysTime));
	unsigned int date = m_dateFrom ? m_dateFrom : m_today;
	sysTime.wYear = (WORD)(date / 10000);
	date -= (unsigned int)sysTime.wYear * 10000;
	sysTime.wMonth = (WORD)(date / 100);
	sysTime.wDay = date - (unsigned int)sysTime.wMonth * 100;
	m_DateFrom.SetTime(&sysTime);

	m_RadioDateFrom.SetCheck(BST_CHECKED);

	EnableLoadButton();

	UpdateSaveAllEnabled();

	UpdateDayCountMode();

	return ret;
}

void SampleDialogSettingHistChart::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingChart::DoDataExchange(pDX);

	DDX_Control(pDX, buttonResetId, m_ButtonReset);
	DDX_Control(pDX, buttonSaveAllId, m_ButtonSaveAll);
	DDX_Control(pDX, buttonOpenAllId, m_ButtonOpenAll);

	DDX_Control(pDX, radioDayCountId, m_RadioDayCount);
	DDX_Control(pDX, radioDateFromId, m_RadioDateFrom);

	DDX_Control(pDX, spinDayCountId, m_SpinDayCount);
	DDX_Control(pDX, dateTimeDateFromId, m_DateFrom);
	DDX_Control(pDX, comboBoxFrequencyId, m_ComboBoxFrequency);

	DDX_Control(pDX, staticDayCountId, m_StaticDayCount);
	DDX_Control(pDX, staticDateFromId, m_StaticDateFrom);
	DDX_Control(pDX, staticFrequencyId, m_StaticFrequency);

}

BEGIN_MESSAGE_MAP(SampleDialogSettingHistChart, BaseDialogSettingChart)
	ON_WM_CTLCOLOR()
//	ON_EN_CHANGE(editChartSymbolId, OnSymbolChange)
//	ON_EN_UPDATE(editChartSymbolId, OnSymbolUpdate)

	ON_BN_CLICKED(buttonResetId, OnReset)
	ON_BN_CLICKED(buttonSaveAllId, OnSaveAll)
	ON_BN_CLICKED(buttonOpenAllId, OnOpenAll)

	ON_BN_CLICKED(radioDayCountId, OnDayCountMode)
	ON_BN_CLICKED(radioDateFromId, OnDayCountMode)

	ON_CBN_SELCHANGE(comboBoxFrequencyId, OnSelchangeFrequency)
	ON_NOTIFY_EX(DTN_DROPDOWN, dateTimeDateFromId, OnCalendarDropdown)
	ON_NOTIFY_EX(DTN_CLOSEUP, dateTimeDateFromId, OnCalendarCloseup)
	ON_NOTIFY(DTN_DATETIMECHANGE, dateTimeDateFromId, OnDateChange)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingHistChart::OnDayCountMode()
{
	const bool dayCountMode = m_RadioDayCount.GetCheck() == BST_CHECKED;
	if(dayCountMode != m_dayCountMode)
	{
		m_dayCountMode = dayCountMode;
		UpdateDayCountMode();
		EnableLoadButton();
		UpdateSaveAllEnabled();
	}
}

LRESULT SampleDialogSettingHistChart::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinDayCountId:
		m_SpinDayCount.UpdateValueNum();
		{
			const unsigned int dayCount = m_SpinDayCount.GetValue();
//			if(!m_dayCount != !dayCount)
//			{
			if(m_dayCountMode)
			{
				m_SpinDayCount.SetBkColorEnabled(dayCount ? dayCount == m_dayCountInitial ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetPendingColor() : m_visualBase->GetWarningColor());
			}
			m_dayCount = dayCount;
		}
		EnableLoadButton();
		if(m_dayCountMode)UpdateSaveAllEnabled();

		break;

		default:
		BaseDialogSettingChart::OnSpinChange(w, l);
		break;
	}
	return 0;
}

void SampleDialogSettingHistChart::OnDateChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	unsigned int date = GetControlDateFrom();
	if(m_dateFrom != date)
	{
		if(!m_dayCountMode)
		{
/*
			bool today = date >= m_today;
			if((m_dateFrom >= m_today) != today)
			{
				m_DateFrom.SetBkColor(today ? m_takionVisual->GetWarningColor() : 0xFFFFFFFF);
			}
*/
			m_DateFrom.SetBkColor(date >= m_today ? m_visualBase->GetWarningColor() : date == m_dateFromInitial ? 0xFFFFFFFF : m_visualBase->GetPendingColor());
		}
		m_dateFrom = date;
		EnableLoadButton();
		if(!m_dayCountMode)UpdateSaveAllEnabled();
	}
	*pResult = 0;
}

BOOL SampleDialogSettingHistChart::OnCalendarDropdown(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
//	PretendNotDisplayed();
	UGUI_IdleInterrupt(true, 3);
	*pResult = 0;
	return TRUE;
}

BOOL SampleDialogSettingHistChart::OnCalendarCloseup(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
//	UnpretendNotDisplayed();
	UGUI_IdleInterrupt(false, 3);
	*pResult = 0;
	::SetFocus(m_DateFrom.m_hWnd);
	return TRUE;
}

unsigned char SampleDialogSettingHistChart::GetControlFrequency() const
{
	if(m_ComboBoxFrequency.m_hWnd)
	{
		int sel = m_ComboBoxFrequency.GetCurSel();
		return sel >= 0 ? (unsigned char)sel : 0xFF;
	}
	return 0xFF;
}

unsigned int SampleDialogSettingHistChart::GetStaticFrequencyFromCode(char frequencyCode)
{
	switch(frequencyCode)
	{
		case CHF_MONTH:
		return 1;

		case CHF_WEEK:
		return 2;
	
		case CHF_DAY:
		return 3;

		case CHF_HOUR:
		return 4;

		case CHF_30MIN:
		return 5;

		case CHF_15MIN:
		return 6;

		case CHF_5MIN:
		return 7;

		case CHF_1MIN:
		return 8;

		default:
		return 0xFFFFFFFF;
	}
}

char SampleDialogSettingHistChart::GetStaticFrequencyCode(unsigned int frequency)
{
	switch(frequency)
	{
		case 1:
		return CHF_MONTH;

		case 2:
		return CHF_WEEK;

		case 3:
		return CHF_DAY;

		case 4:
		return CHF_HOUR;

		case 5:
		return CHF_30MIN;

		case 6:
		return CHF_15MIN;

		case 7:
		return CHF_5MIN;

		case 8:
		return CHF_1MIN;

		default:
		return CHF_INVALID;
	}
}

unsigned int SampleDialogSettingHistChart::GetControlDateFrom() const
{
	SYSTEMTIME d;
	m_DateFrom.GetTime(&d);
	return (unsigned int)d.wYear * 10000 + (unsigned int)d.wMonth *100 + d.wDay;
}

void SampleDialogSettingHistChart::OnSelchangeFrequency()
{
	unsigned char cf = GetControlFrequency();
	if(m_frequency != cf)
	{
		m_frequency = cf;
//		m_ComboBoxFrequency.SetInvalid(!m_frequency);
		EnableLoadButton();
		UpdateSaveAllEnabled();
	}
}
/*
void SampleDialogSettingHistChart::UpdateCurrentLoadSettingsAndSymbol()
{
	if(m_symbolInitial != m_symbol)
	{
		m_symbolInitial = m_symbol;
//		m_EditSymbol.SetWindowText(m_symbolInitial.c_str());
		m_StaticSymbol.SetWindowText(m_symbolInitial.c_str());
		m_EditSymbol.Invalidate(FALSE);
		m_modified = true;
	}
	UpdateCurrentLoadSettings();

	UpdateDayCountMode();
	EnableLoadButton();
}
*/
void SampleDialogSettingHistChart::UpdateCurrentLoadSettings()
{
	static char num[33];
	if(m_dayCountInitial != m_dayCount)
	{
		m_dayCountInitial = m_dayCount;
//		m_SpinDayCount.SetValue(m_dayCountInitial);
		m_StaticDayCount.SetWindowText(U_UnsignedNumberToStrNoDecDigits(m_dayCountInitial, num, sizeof(num)));
//		m_StaticDayCount.Invalidate(FALSE);
		m_modified = true;
	}
	if(m_frequencyInitial != m_frequency)
	{
		m_frequencyInitial = m_frequency;
		m_ComboBoxFrequency.SetCurrentItemOrdinal(m_frequencyInitial);

//		m_ComboBoxFrequency.SetCurSel(m_frequencyInitial);
//		OnSelchangeFrequency();
		m_StaticFrequency.SetWindowText(m_frequencyInitial < (unsigned char)m_ComboBoxFrequency.GetItemCount() ? (const char*)m_ComboBoxFrequency.GetItemDataPtr(m_frequencyInitial) : "");
//		m_StaticDayCount.Invalidate(FALSE);
		m_modified = true;
	}
	if(m_dateFromInitial != m_dateFrom)
	{
		m_dateFromInitial = m_dateFrom;
		char* cursor = U_FormatDate(m_dateFromInitial, num, sizeof(num), '-', '\0', false, true);
		*cursor = '\0';
		m_StaticDateFrom.SetWindowText(num);
		m_StaticDateFrom.Invalidate(FALSE);
		m_DateFrom.SetBkColor(m_dateFrom >= m_today ? m_visualBase->GetWarningColor() : 0xFFFFFFFF);
		m_modified = true;
	}
	if(m_dayCountModeInitial != m_dayCountMode)
	{
		m_dayCountModeInitial = m_dayCountMode;
		m_modified = true;
	}
}

bool SampleDialogSettingHistChart::DoReset()
{
	bool ret = BaseDialogSettingChart::DoReset();
	if(m_dayCount != m_dayCountInitial)
	{
		m_dayCount = m_dayCountInitial;
		m_SpinDayCount.SetValue(m_dayCountInitial);
		ret = true;
	}

	if(m_dayCountMode != m_dayCountModeInitial)
	{
		m_dayCountMode = m_dayCountModeInitial;
		m_RadioDayCount.SetCheck(m_dayCountMode ? BST_CHECKED : BST_UNCHECKED);
		m_RadioDateFrom.SetCheck(m_dayCountMode ? BST_UNCHECKED : BST_CHECKED);
		ret = true;
	}
	if(m_frequency != m_frequencyInitial)
	{
		m_ComboBoxFrequency.SetCurSel(m_frequencyInitial == 0xFF ? -1 : m_frequencyInitial);
		OnSelchangeFrequency();
		ret = true;
	}
	if(m_dateFrom != m_dateFromInitial)
	{
		m_dateFrom = m_dateFromInitial;
		SYSTEMTIME sysTime;
		memset(&sysTime, 0, sizeof(sysTime));
		unsigned int date = m_dateFromInitial ? m_dateFromInitial : m_today;

		sysTime.wYear = (WORD)(date / 10000);
		date -= (unsigned int)sysTime.wYear * 10000;
		sysTime.wMonth = (WORD)(date / 100);
		sysTime.wDay = date - (unsigned int)sysTime.wMonth * 100;
		m_DateFrom.SetTime(&sysTime);
		ret = true;
	}
	if(ret)
	{
		UpdateDayCountMode();
		EnableLoadButton();
		UpdateSaveAllEnabled();
	}
	return ret;
}

void SampleDialogSettingHistChart::UpdateDayCountMode()
{
	if(m_dayCountMode)
	{
		m_SpinDayCount.SetBkColorEnabled(m_dayCount ? m_dayCount == m_dayCountInitial ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetPendingColor() : m_visualBase->GetWarningColor());
		m_DateFrom.SetBkColor(m_visualBase->GetGrayColor());
	}
	else
	{
		m_SpinDayCount.SetBkColorEnabled(m_visualBase->GetGrayColor());
		m_DateFrom.SetBkColor(m_dateFrom >= m_today ? m_visualBase->GetWarningColor() : m_dateFrom == m_dateFromInitial ? 0xFFFFFFFF : m_visualBase->GetPendingColor());
	}
}

void SampleDialogSettingHistChart::NextDayStarted()
{
	m_historicalChart = nullptr;
	m_historicalChartLoadingToSave = nullptr;
}

bool SampleDialogSettingHistChart::DoLoad()
{
	if(canLoad())
	{
		unsigned __int64 numericSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(m_symbol.c_str(), sizeof(unsigned __int64) - 1);
/*
		const FrequencyChartCollection* chartCollection = TD_FindHistoricalChartSymbol(numericSymbol);
		if(chartCollection)
		{
			UpdateCurrentLoadSettings();
		}
		else
		{
*/
			unsigned int dateFrom = 0;
			if(m_dayCountMode)
			{
				if(m_today)
				{
					unsigned short todayYear, todayMonth, todayDay;
					U_GetDateTokens(m_today, todayYear, todayMonth, todayDay);
					unsigned int y = todayYear;
					unsigned int m = todayMonth;
					unsigned int d = todayDay;
					TL_DateSubtractBusinessDays(y, m, d, m_dayCount);
					dateFrom = U_CombineTokensIntoDate((unsigned short)y, (unsigned short)m, (unsigned short)d);
				}			
			}
			else
			{
				dateFrom = m_dateFrom;
			}
			if(dateFrom && dateFrom < m_today)
			{
				UpdateCurrentLoadSettingsAndSymbol();

				bool includePrePostMarket = false;
				unsigned int reqId = m_historicalChart ? m_historicalChart->GetHistoricalChartRequestId() : 0;
				HistoricalChart* hc = TD_RequestAndKeepStockHistoricalChart(m_symbol.c_str(), dateFrom, GetFrequencyCode(), includePrePostMarket);
				if(hc && m_historicalChart != hc)
				{
					m_historicalChart = hc;
					if(m_historicalChart->GetHistoricalChartRequestId())
					{
						m_ListBoxChart.ClearItemsWithRedraw();
						m_totalVolume = 0;
					}
					else
					{
						FillListBox();
					}
					InvalidateAllTotalValueRects(listBoxHistChartId);
				}
				else if(m_historicalChart)
				{
					if(m_historicalChart->GetHistoricalChartRequestId())
					{
						m_ListBoxChart.ClearItemsWithRedraw();
						m_totalVolume = 0;
						InvalidateAllTotalValueRects(listBoxHistChartId);
					}
					else
					{
						FillListBox();
					}
				}
			}
//		}
		return true;
	}
	return false;
}

void SampleDialogSettingHistChart::DoFillListBox(const Security::ChartPointVector& chartPointVector)
{
	const unsigned short nodeMinutes = GetNodeMinuteCount();
	if(nodeMinutes)
	{
		ChartPoint* cp = nullptr;
		unsigned int currentDate = 0;
		unsigned int date = 0;
		unsigned short minute = 0;
		unsigned short currentMinute = 0;
		unsigned short nextMinute = 0;
		unsigned short overfillMinutes;
		unsigned int volume;
		for(Security::ChartPointVector::const_iterator it = chartPointVector.cbegin(), itend = chartPointVector.cend(); it != itend; ++it)
		{
			const ChartPoint& chartPoint = *it;
			minute = chartPoint.GetMinute();
			date = chartPoint.GetDate();
			if(date != currentDate)
			{
				currentDate = date;
				nextMinute = 0;
			}
			volume = chartPoint.GetVolume();
			m_totalVolume += volume;
			if(nextMinute)
			{
				if(minute >= nextMinute)
				{
//					if(volume || !chartPoint.GetStartPrice().isZero())
//					{
						cp = new ChartPoint(chartPoint);
						m_ListBoxChart.AddString((const char*)cp);
						currentMinute = minute;
						nextMinute = currentMinute + nodeMinutes;
						overfillMinutes = nextMinute % nodeMinutes;
						if(overfillMinutes)nextMinute -= overfillMinutes;
//					}
				}
				else
				{
					if(cp)
					{
						cp->AddPoint(chartPoint);
					}
				}
			}
			else// if(volume || !chartPoint.GetStartPrice().isZero())
			{
				cp = new ChartPoint(chartPoint);
				m_ListBoxChart.AddString((const char*)cp);
				currentMinute = minute;
				nextMinute = currentMinute + nodeMinutes;
				overfillMinutes = nextMinute % nodeMinutes;
				if(overfillMinutes)nextMinute -= overfillMinutes;
			}
		}
	}
	else
	{
		BaseDialogSettingChart::DoFillListBox(chartPointVector);
	}
}

void SampleDialogSettingHistChart::OnOpenAll()
{
	if(CanOpenAll())U_ShellExecute(m_hWnd, m_allChartFileName.c_str());
}

void SampleDialogSettingHistChart::OnSaveAll()
{
	if(CanSaveAll())
	{
		if(m_allChartFileName.empty())
		{
			char buf[MAX_PATH];
			strcpy_s(buf, sizeof(buf), theApp.GetFilePathAndName().c_str());
			char* p = strrchr(buf, '\\');
			if(p)
			{
				*++p = '*';
				*++p = '.';
				*++p = 't';
				*++p = 'x';
				*++p = 't';
				*++p = '\0';
				m_allChartFileName = buf;
			}
		}

		TakionFileDialog fileDlg(m_mainWnd, false, "txt", m_allChartFileName.c_str(),
			OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_EXTENSIONDIFFERENT,
			"All Chart Files (*.txt)|*.txt|All Files (*.*)|*.*||", "Save All Chart File", this);
		if(fileDlg.DoModal() == IDOK)
		{
			m_allChartFileName = fileDlg.GetPathName();
			m_allChartFile.open(m_allChartFileName, std::ios::out);
			if(m_allChartFile.is_open())
			{
				UpdateCurrentLoadSettings();

//Collect symbols of all the stocks on the Market (except test stocks)
				m_symbolSet.clear();
				TakionIterator* iterator = TD_CreateEquityIterator();
				TD_LockEquityStorage();
				iterator->Reset();

				std::string symbol;
				Security* security;
				while(security = TD_GetNextEquity(iterator))
				{
					if(!security->isTest())
					{
						symbol = security->GetSymbol();
						m_symbolSet.insert(symbol);
					}
				}

				TD_UnlockEquityStorage();
				delete iterator;
//Figure out Date From
				m_dateFromLoadingToSave = 0;
				if(m_dayCountMode)
				{
					if(m_today)
					{
						unsigned short todayYear, todayMonth, todayDay;
						U_GetDateTokens(m_today, todayYear, todayMonth, todayDay);
						unsigned int y = todayYear;
						unsigned int m = todayMonth;
						unsigned int d = todayDay;
						TL_DateSubtractBusinessDays(y, m, d, m_dayCount);
						m_dateFromLoadingToSave = U_CombineTokensIntoDate((unsigned short)y, (unsigned short)m, (unsigned short)d);
					}			
				}
				else
				{
					m_dateFromLoadingToSave = m_dateFrom;
				}

				if(m_dateFromLoadingToSave && m_dateFromLoadingToSave < m_today)
				{
					EnableAllControls(false);
					const char delimiter = '\t';
					m_allChartFile << "Date" << delimiter
						<< "Time" << delimiter
						<< "Start" << delimiter
						<< "Low" << delimiter
						<< "High" << delimiter
						<< "End" << delimiter
						<< "Volume" << '\n' << std::flush;
//Start loading
					LoadNextChart();
				}
				else
				{
					m_allChartFile.close();
					m_symbolSet.clear();
				}
			}
		}
	}
}

void SampleDialogSettingHistChart::LoadNextChart()
{
	for(StringSet::iterator itbegin = m_symbolSet.begin(), itend = m_symbolSet.end(); itbegin != itend; itbegin = m_symbolSet.erase(itbegin))
	{
		const std::string& symbol = *itbegin;
		m_historicalChartLoadingToSave = TD_RequestAndKeepStockHistoricalChart(symbol.c_str(), m_dateFromLoadingToSave, GetFrequencyCode(), false);//includePrePostMarket);
		if(m_historicalChartLoadingToSave)
		{
			if(m_historicalChartLoadingToSave->GetHistoricalChartRequestId())
			{
				m_EditSymbol.SetWindowText(symbol.c_str());
				m_symbolSet.erase(itbegin);
				break;//wait for the server to respond with data
			}
			else
			{
				SaveCurrentHistoricalChartLoading();
			}
		}
	}
	if(m_symbolSet.empty())
	{
		m_allChartFile.close();
		m_historicalChartLoadingToSave = nullptr;
		if(m_EditSymbol.m_hWnd)
		{
			EnableAllControls(true);
			m_EditSymbol.SetWindowText(m_symbol.c_str());
		}
	}
}

void SampleDialogSettingHistChart::SaveCurrentHistoricalChartLoading()
{
	if(m_historicalChartLoadingToSave && m_allChartFile.is_open())
	{
		const char delimiter = '\t';

		m_allChartFile << m_historicalChartLoadingToSave->GetSymbol() << '\n';

		char num[33];
		char* cursor;
		const Security::ChartPointVector& chartPointVector = m_historicalChartLoadingToSave->GetHistoricalChart();
		for(Security::ChartPointVector::const_iterator it = chartPointVector.begin(), itend = chartPointVector.end(); it != itend; ++it)
		{
			const ChartPoint& cp = *it;

			cursor = U_FormatDate(cp.GetDate(), num, sizeof(num), '-', '\0', false, false);
			*cursor = '\0';
			m_allChartFile << num << delimiter;

			if(cp.GetMinute())
			{
				U_FormatMinute(cp.GetMinute(), num, sizeof(num));
				m_allChartFile << num;
			}
			m_allChartFile << delimiter;

			m_allChartFile << U_UnsignedMonetaryToBuffer(cp.GetStartPrice(), num, sizeof(num), m_decDigit, true, '\0', 2) << delimiter;
			m_allChartFile << U_UnsignedMonetaryToBuffer(cp.GetLowPrice(), num, sizeof(num), m_decDigit, true, '\0', 2) << delimiter;
			m_allChartFile << U_UnsignedMonetaryToBuffer(cp.GetHighPrice(), num, sizeof(num), m_decDigit, true, '\0', 2) << delimiter;
			m_allChartFile << U_UnsignedMonetaryToBuffer(cp.GetEndPrice(), num, sizeof(num), m_decDigit, true, '\0', 2) << delimiter;
//			m_allChartFile << cp.GetVolume() << '\r' << '\n';
			m_allChartFile << cp.GetVolume() << '\n';
/*
#ifdef _DEBUG
if(cp.GetHighPrice() < cp.GetLowPrice())
{
printf("X");
}
const char* low = U_UnsignedMonetaryToBuffer(cp.GetLowPrice(), num, sizeof(num), m_decDigit, true, '\0', 2);
Price lowPrice;
U_StrToPrice(lowPrice, low, m_decDigit);
const char* high = U_UnsignedMonetaryToBuffer(cp.GetHighPrice(), num, sizeof(num), m_decDigit, true, '\0', 2);
Price highPrice;
U_StrToPrice(highPrice, high, m_decDigit);
if(highPrice < lowPrice)
{
low = U_UnsignedMonetaryToBuffer(cp.GetLowPrice(), num, sizeof(num), m_decDigit, true, '\0', 2);
high = U_UnsignedMonetaryToBuffer(cp.GetHighPrice(), num, sizeof(num), m_decDigit, true, '\0', 2);
printf("X");
}
#endif
*/
		}
		m_allChartFile << std::flush;
	}
}

void SampleDialogSettingHistChart::EnableAllControls(bool enable)
{
	if(enable)
	{
		EnableLoadButton();
		UpdateSaveAllEnabled();
		UpdateOpenAllEnabled();
	}
	else
	{
		EnableControl(m_ButtonReset.m_hWnd, enable, m_EditSymbol.m_hWnd);
		EnableControl(m_ButtonLoad.m_hWnd, enable, m_EditSymbol.m_hWnd);
		EnableControl(m_ButtonSaveAll.m_hWnd, enable, m_EditSymbol.m_hWnd);
		EnableControl(m_ButtonOpenAll.m_hWnd, enable, m_EditSymbol.m_hWnd);
	}
	m_EditSymbol.SetReadOnly(enable ? FALSE : TRUE);
	EnableControl(m_SpinDayCount.m_hWnd, enable, m_EditSymbol.m_hWnd);
	EnableControl(m_RadioDayCount.m_hWnd, enable, m_EditSymbol.m_hWnd);
	EnableControl(m_RadioDateFrom.m_hWnd, enable, m_EditSymbol.m_hWnd);
	EnableControl(m_DateFrom.m_hWnd, enable, m_EditSymbol.m_hWnd);
	EnableControl(m_ComboBoxFrequency.m_hWnd, enable, m_EditSymbol.m_hWnd);
	EnableControl(m_SpinDecDigit.m_hWnd, enable, m_EditSymbol.m_hWnd);
}

void SampleDialogSettingHistChart::OnReset()
{
	DoReset();
}

bool SampleDialogSettingHistChart::CanReset() const
{
	return m_dayCountMode != m_dayCountModeInitial || m_dayCount != m_dayCountInitial || m_dateFrom != m_dateFromInitial || m_frequency != m_frequencyInitial || m_symbol != m_symbolInitial;
}

bool SampleDialogSettingHistChart::canLoad() const
{
	return !m_symbol.empty()//BaseDialogSettingChart::CanLoad()
		&& !m_allChartFile.is_open()
		&& m_historicalPrintConnected
		&& (m_dayCountMode ? m_dayCount != 0 : m_dateFrom < m_today) && m_frequency != 0xFFFFFFFF && m_frequency != 0
		;
}

bool SampleDialogSettingHistChart::CanLoad() const
{
	return canLoad()
		&& (m_dayCountMode != m_dayCountModeInitial || (m_dayCountMode ? m_dayCount != m_dayCountInitial : m_dateFrom != m_dateFromInitial) || m_frequency != m_frequencyInitial || m_symbol != m_symbolInitial)
		;
}

bool SampleDialogSettingHistChart::CanSaveAll() const
{
	return !m_allChartFile.is_open() && m_marketSorterLoaded && m_historicalPrintConnected &&
		(m_dayCountMode ? m_dayCount != 0 : m_dateFrom < m_today) && m_frequency != 0xFFFFFFFF && m_frequency != 0;
}

void SampleDialogSettingHistChart::UpdateSaveAllEnabled()
{
	EnableControl(m_ButtonSaveAll.m_hWnd, CanSaveAll(), m_EditSymbol.m_hWnd);
}

bool SampleDialogSettingHistChart::CanOpenAll() const
{
	return !m_allChartFile.is_open() && !m_allChartFileName.empty();
}

void SampleDialogSettingHistChart::UpdateOpenAllEnabled()
{
	EnableControl(m_ButtonOpenAll.m_hWnd, CanOpenAll(), m_EditSymbol.m_hWnd);
}

void SampleDialogSettingHistChart::EnableLoadButton()
{
	BaseDialogSettingChart::EnableLoadButton();
	EnableControl(m_ButtonReset.m_hWnd, CanReset(), m_EditSymbol.m_hWnd);
}

void SampleDialogSettingHistChart::SetHistoricalPrintConnected(bool connected)
{
	if(m_historicalPrintConnected != connected)
	{
		m_historicalPrintConnected = connected;
		EnableLoadButton();
		UpdateSaveAllEnabled();
		if(m_historicalPrintConnected)
		{
			DoLoad();
		}
		else if(m_allChartFile.is_open())
		{
			m_symbolSet.clear();
			m_historicalChartLoadingToSave = nullptr;
			if(m_EditSymbol.m_hWnd)m_EditSymbol.SetWindowText(m_symbol.c_str());
			m_allChartFile.close();
			EnableAllControls(true);
		}
	}
}

void SampleDialogSettingHistChart::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case M_RESP_CANDLE_HISTORICAL_DATA:
		{
			const TMsgCandleHistoricalData* msg = (const TMsgCandleHistoricalData*)message;
			if(info && !(msg->m_flags & More))
			{
				const TMsgHistoricalChartPackLoaded* chartLoaded = (const TMsgHistoricalChartPackLoaded*)info;
				if(m_historicalChartLoadingToSave)
				{
					if(m_historicalChartLoadingToSave == chartLoaded->m_historicalChart)
					{
						SaveCurrentHistoricalChartLoading();
						LoadNextChart();
					}
				}
				else if(!m_historicalChart || m_historicalChart == chartLoaded->m_historicalChart)
				{
					FillListBox();
				}
			}
		}
		break;

		case M_RESP_CANDLE_INTRADAY_DATA:
		{
			const TMsgCandleIntradayData* msg = (const TMsgCandleIntradayData*)message;
			if(info && !(msg->m_flags & More))
			{
				const TMsgHistoricalChartPackLoaded* chartLoaded = (const TMsgHistoricalChartPackLoaded*)info;
				if(m_historicalChartLoadingToSave)
				{
					if(m_historicalChartLoadingToSave == chartLoaded->m_historicalChart)
					{
						SaveCurrentHistoricalChartLoading();
						LoadNextChart();
					}
				}
				else if(!m_historicalChart || m_historicalChart == chartLoaded->m_historicalChart)
				{
					FillListBox();
				}
			}
		}
		break;
#ifndef TAKION_NO_OPTIONS
		case M_RESP_FUT_HISTORICAL_DATA:
		{
			const TMsgFutCandleHistoricalData* msg = (const TMsgFutCandleHistoricalData*)message;
			if(info && !(msg->m_flags & More))
			{
				const TMsgHistoricalChartPackLoaded* chartLoaded = (const TMsgHistoricalChartPackLoaded*)info;
				if(m_historicalChartLoadingToSave)
				{
					if(m_historicalChartLoadingToSave == chartLoaded->m_historicalChart)
					{
						SaveCurrentHistoricalChartLoading();
						LoadNextChart();
					}
				}
				else if(!m_historicalChart || m_historicalChart == chartLoaded->m_historicalChart)
				{
					FillListBox();
				}
			}
		}
		break;

		case M_RESP_FUT_INTRADAY_DATA:
		{
			const TMsgFutCandleIntradayData* msg = (const TMsgFutCandleIntradayData*)message;
			if(info && !(msg->m_flags & More))
			{
				const TMsgHistoricalChartPackLoaded* chartLoaded = (const TMsgHistoricalChartPackLoaded*)info;
				if(m_historicalChartLoadingToSave)
				{
					if(m_historicalChartLoadingToSave == chartLoaded->m_historicalChart)
					{
						SaveCurrentHistoricalChartLoading();
						LoadNextChart();
					}
				}
				else if(!m_historicalChart || m_historicalChart == chartLoaded->m_historicalChart)
				{
					FillListBox();
				}
			}
		}
		break;

		case M_RESP_FUT_HISTORICAL_PRICE_VOLUME:
		{
			const TMsgRespFutHistoricalPriceVolume* msg = (const TMsgRespFutHistoricalPriceVolume*)message;
			const unsigned int requestId = msg->m_requestId;
			const unsigned char pointSize = msg->m_tickSize;
			const unsigned short pointCount = (msg->m_length - sizeof(*msg)) / pointSize;
/*
char num[33];
std::string info;
info.reserve(64);
info = msg->m_symbol;
info += ' ';
//info += U_UnsignedNumberToStrNoDecDigits(minute, num, sizeof(num), ' ');
info += U_UnsignedNumberToStrNoDecDigits(requestId, num, sizeof(num), ' ');
info += U_UnsignedNumberToStrNoDecDigits(pointSize, num, sizeof(num), ' ');
info += U_UnsignedNumberToStrNoDecDigits(msg->m_length, num, sizeof(num), ' ');
info += U_UnsignedNumberToStrNoDecDigits(pointCount, num, sizeof(num));
takion.Log(info.c_str(), "FuturePriceVolumeSnapshotProcessing");
*/
		}
		break;

#endif
		default:
		BaseDialogSettingChart::Notify(message, from, info);
		break;
	}
}
