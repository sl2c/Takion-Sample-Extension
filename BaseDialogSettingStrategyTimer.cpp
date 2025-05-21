#include "stdafx.h"
#include "BaseDialogSettingStrategyTimer.h"
#include "ExtensionDerivedConfig.h"

//extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool ListBoxSecurityTimer::SetTradingState(const unsigned char& tradingState)
{
	if(tradingState != m_tradingState)
	{
		m_tradingState = tradingState;
		TimerSecurityObserver* securityObserver;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			securityObserver = (TimerSecurityObserver*)GetItemDataPtr(i);
			securityObserver->SetTradingState(tradingState);
		}
		return true;
	}
	return false;
}

BaseDialogSettingStrategyTimer::BaseDialogSettingStrategyTimer(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label,
		ListBoxSecurityTimer* listBox, bool subscribe, bool disconnectFromMarketSorter, int controlColumnWidth, int listBoxHOffset):
	BaseDialogSettingStrategy(mainWnd, parentTab, label, listBox, false, false, controlColumnWidth, listBoxHOffset),
	m_SpinTimeEntry(true, 200000000, 40000000),//max 20 o'clock, min 4 o'clock
	m_SpinTimeStop(true, 200000000, 40000000),
	m_SpinTimeExit(true, 200000000, 40000000),
	m_SpinTimeStopExit(true, 200000000, 40000000),
	m_listBoxSecurityTimer(listBox),
	m_millisecondEntry(0),
	m_millisecondStop(0),
	m_millisecondExit(0),
	m_millisecondStopExit(0),
	m_tradingState(TS_STOP_EXIT),
	m_timerCreatedMask(0)
{
	SetTimeSpinCells(m_SpinTimeEntry);
	SetTimeSpinCells(m_SpinTimeStop);
	SetTimeSpinCells(m_SpinTimeExit);
	SetTimeSpinCells(m_SpinTimeStopExit);

//	m_bottomControl += staticHeight;
	m_bottomControl += vgap + vgap;
	const int spinW = m_accountWidth - 1 - 2;// * 3 / 4;
	const int x = 0;//hgap;
	const int xc = x + 1;
//	const int staticX = x + spinW;
//	const int staticW = m_accountWidth - hgap - spinW - x;
	const int timerGroupHeight = groupOffset + 4 * (staticHeight + spinHeight + vgap) + hgap;// + vgap + editHeight + hgap;
	int y = m_bottomControl;

	AddButtonIndirect("Trading State", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, m_accountWidth, timerGroupHeight, groupTimerId);

	AddStaticIndirect("Start Entry", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += groupOffset, spinW, staticHeight, staticTimeEntryId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, y += staticHeight, spinW, spinHeight, spinTimeEntryId);

	AddStaticIndirect("Stop Entry", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += spinHeight + vgap, spinW, staticHeight, staticTimeStopId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, y += staticHeight, spinW, spinHeight, spinTimeStopId);

	AddStaticIndirect("Exit", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += spinHeight + vgap, spinW, staticHeight, staticTimeExitId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, y += staticHeight, spinW, spinHeight, spinTimeExitId);

	AddStaticIndirect("Don't Exit", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += spinHeight + vgap, spinW, staticHeight, staticTimeStopExitId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, y += staticHeight, spinW, spinHeight, spinTimeStopExitId);

	m_bottomControl += timerGroupHeight;//spinHeight;
}

void BaseDialogSettingStrategyTimer::SetTimeSpinCells(TimeSpin& spin)
{
	spin.AddSpinCell(1, false, 1);
	spin.AddSpinCell(100, false, 100);
	spin.AddSpinCell(1000, false, 1000);
//	spin.AddSpinCell(10000, false, 10000);
	spin.AddSpinCell(10000000, true, 10000000);
	spin.AddSpinCell(100000, true, 100000);
}

void BaseDialogSettingStrategyTimer::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingStrategy::DoDataExchange(pDX);
	DDX_Control(pDX, spinTimeEntryId, m_SpinTimeEntry);
	DDX_Control(pDX, spinTimeStopId, m_SpinTimeStop);
	DDX_Control(pDX, spinTimeExitId, m_SpinTimeExit);
	DDX_Control(pDX, spinTimeStopExitId, m_SpinTimeStopExit);

	DDX_Control(pDX, staticTimeEntryId, m_StaticTimeEntry);
	DDX_Control(pDX, staticTimeStopId, m_StaticTimeStop);
	DDX_Control(pDX, staticTimeExitId, m_StaticTimeExit);
	DDX_Control(pDX, staticTimeStopExitId, m_StaticTimeStopExit);

}

BEGIN_MESSAGE_MAP(BaseDialogSettingStrategyTimer, BaseDialogSettingStrategy)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

CStatic* BaseDialogSettingStrategyTimer::GetTimeStaticByTradingState(const unsigned char tradingState)
{
	switch(tradingState)
	{
		case TS_ENTRY:
		return &m_StaticTimeEntry;

		case TS_STOP:
		return &m_StaticTimeStop;

		case TS_EXIT:
		return &m_StaticTimeExit;

		case TS_STOP_EXIT:
		return &m_StaticTimeStopExit;

		default:
		return nullptr;
	}
}

TimeSpin* BaseDialogSettingStrategyTimer::GetTimeSpinByTradingState(const unsigned char tradingState)
{
	switch(tradingState)
	{
		case TS_ENTRY:
		return &m_SpinTimeEntry;

		case TS_STOP:
		return &m_SpinTimeStop;

		case TS_EXIT:
		return &m_SpinTimeExit;

		case TS_STOP_EXIT:
		return &m_SpinTimeStopExit;

		default:
		return nullptr;
	}
}

void BaseDialogSettingStrategyTimer::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent >= TS_ENTRY && nIDEvent < TS_Count)
	{
		const unsigned char tradingState = (const unsigned char)nIDEvent;
		SetTradingState(TL_GetCurrentMillisecond(), tradingState);
		TimeSpin* spin = GetTimeSpinByTradingState(tradingState);
		if(spin)ClearTimer(tradingState, *spin);
	}
}
/*
void BaseDialogSettingStrategyTimer::UpdateTimeSpinColor()
{
	m_SpinTimeEntry.SetBkColorEnabled(m_dlgConnection->isConnected() && m_repeatSecond ? m_repeatMode ? m_okColor : 0xFFFFFFFF : m_grayColor);
}
*/
void BaseDialogSettingStrategyTimer::OnTradingStateTimeChange(const unsigned char tradingState, const unsigned char nextTradingState, unsigned int& controlMillisecond, TimeSpin& spin)
{
	const unsigned int millisecond = spin.GetMillisecond();
	if(millisecond != controlMillisecond)
	{
		const unsigned int currentMillisecond = TL_GetCurrentMillisecond();
		bool changed = (currentMillisecond < millisecond) != (currentMillisecond < controlMillisecond);
		controlMillisecond = millisecond;
		m_modified = true;
		if(changed)
		{
			InitializeTradingState(currentMillisecond);
		}
		UpdateTimer(nextTradingState, currentMillisecond, millisecond, spin);
	}
}

LRESULT BaseDialogSettingStrategyTimer::OnSpinChange(WPARAM spinId, LPARAM l)
{
	switch(spinId)
	{
		case spinTimeEntryId:
		OnTradingStateTimeChange(TS_BEFORE_ENTRY, TS_ENTRY, m_millisecondEntry, m_SpinTimeEntry);
		break;

		case spinTimeStopId:
		OnTradingStateTimeChange(TS_ENTRY, TS_STOP, m_millisecondStop, m_SpinTimeStop);
		break;

		case spinTimeExitId:
		OnTradingStateTimeChange(TS_STOP, TS_EXIT, m_millisecondExit, m_SpinTimeExit);
		break;

		case spinTimeStopExitId:
		OnTradingStateTimeChange(TS_EXIT, TS_STOP_EXIT, m_millisecondStopExit, m_SpinTimeStopExit);
		break;
	}
	return 0;
}

BOOL BaseDialogSettingStrategyTimer::OnInitDialog()
{
	BOOL ret = BaseDialogSettingStrategy::OnInitDialog();

	return ret;
}

void BaseDialogSettingStrategyTimer::BeforeDestroy()
{
	BaseDialogSettingStrategy::BeforeDestroy();
	m_millisecondEntry = 0;
	m_millisecondStop = 0;
	m_millisecondExit = 0;
	m_millisecondStopExit = 0;
	m_tradingState = TS_STOP_EXIT;
	m_timerCreatedMask = 0;
}

HBRUSH BaseDialogSettingStrategyTimer::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case staticTimeEntryId:
		if(TS_ENTRY == m_tradingState)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetOkTextColor());
			return *m_bkBrush;
		}
		break;

		case staticTimeStopId:
		if(TS_STOP == m_tradingState)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetOkTextColor());
			return *m_bkBrush;
		}
		break;

		case staticTimeExitId:
		if(TS_EXIT == m_tradingState)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetOkTextColor());
			return *m_bkBrush;
		}
		break;

		case staticTimeStopExitId:
		if(TS_STOP_EXIT == m_tradingState)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetOkTextColor());
			return *m_bkBrush;
		}
		break;
	}
	return BaseDialogSettingStrategy::OnCtlColor(pDC, pWnd, nCtlColor);
}
/*
void BaseDialogSettingStrategyTimer::RepaintTotals() const
{
	BaseDialogSettingStrategy::RepaintTotals();
}
*/

void BaseDialogSettingStrategyTimer::OnHistoryLoaded()
{
	BaseDialogSettingStrategy::OnHistoryLoaded();
}

void BaseDialogSettingStrategyTimer::OnExecutorConnected()
{
	BaseDialogSettingStrategy::OnExecutorConnected();
}

void BaseDialogSettingStrategyTimer::ClearTimer(const unsigned char tradingState, TimeSpin& spin)
{
	if(m_timerCreatedMask & (1 << tradingState))
	{
		KillTimer(tradingState);
		m_timerCreatedMask &= ~(1 << tradingState);
		spin.SetBkColorEnabled(0xFFFFFFFF);
	}
}

void BaseDialogSettingStrategyTimer::CreateTimer(const unsigned char tradingState, const unsigned int millisecond, TimeSpin& spin)
{
	SetTimer(tradingState, millisecond, nullptr);
	if(!(m_timerCreatedMask & (1 << tradingState)))
	{
		m_timerCreatedMask |= 1 << tradingState;
		spin.SetBkColorEnabled(m_visualBase->GetOkColor());
	}
}

void BaseDialogSettingStrategyTimer::UpdateTimer(const unsigned char tradingState, const unsigned int currentMillisecond, const unsigned int endMillisecond, TimeSpin& spin)
{
	ClearTimer(tradingState, spin);
	if(currentMillisecond < endMillisecond)
	{
		CreateTimer(tradingState, endMillisecond - currentMillisecond, spin);
	}
}

void BaseDialogSettingStrategyTimer::InvalidateTimerName(const unsigned char tradingState)
{
	CStatic* timerName = GetTimeStaticByTradingState(tradingState);
	if(timerName)timerName->Invalidate(FALSE);
}

void BaseDialogSettingStrategyTimer::Invalidate2TimerNames(const unsigned char tradingState1, const unsigned char tradingState2)
{
	InvalidateTimerName(tradingState1);
	InvalidateTimerName(tradingState2);
}

bool BaseDialogSettingStrategyTimer::SetTradingState(const unsigned int millisecond, const unsigned char tradingState)
{
	if(tradingState != m_tradingState && tradingState < TS_Count)
	{
		bool change = false;
		switch(tradingState)
		{
			case TS_BEFORE_ENTRY:
			if(millisecond < m_millisecondEntry
				&& millisecond < m_millisecondStopExit
				&& millisecond < m_millisecondExit
				&& millisecond < m_millisecondStop)
			{
				change = true;
			}
			break;

			case TS_ENTRY:
			if(millisecond >= m_millisecondEntry
				&& millisecond < m_millisecondStopExit
				&& millisecond < m_millisecondExit
				&& millisecond < m_millisecondStop)
			{
				change = true;
			}
			break;

			case TS_STOP:
			if(millisecond < m_millisecondStopExit
				&& millisecond < m_millisecondExit)
			{
				change = true;
			}
			break;

			case TS_EXIT:
			if(millisecond < m_millisecondStopExit)
			{
				change = true;
			}
			break;

			case TS_STOP_EXIT:
			change = true;
			break;
		}
		if(change)
		{
			Invalidate2TimerNames(m_tradingState, tradingState);
			m_tradingState = tradingState;
			m_listBoxSecurityTimer->SetTradingState(tradingState);
			return true;
		}
	}
	return false;
}

void BaseDialogSettingStrategyTimer::InitializeTradingState(const unsigned int millisecond)
{
//	const unsigned int millisecond = TL_GetCurrentMillisecond();
	if(millisecond >= m_millisecondStopExit)
	{
		SetTradingState(millisecond, TS_STOP_EXIT);
	}
	else if(millisecond >= m_millisecondExit)
	{
		SetTradingState(millisecond, TS_EXIT);
	}
	else if(millisecond >= m_millisecondStop)
	{
		SetTradingState(millisecond, TS_STOP);
	}
	else if(millisecond >= m_millisecondEntry)
	{
		SetTradingState(millisecond, TS_ENTRY);
	}
	else
	{
		SetTradingState(millisecond, TS_BEFORE_ENTRY);
	}
	UpdateTimer(TS_STOP_EXIT, millisecond, m_millisecondStopExit, m_SpinTimeStopExit);
	UpdateTimer(TS_EXIT, millisecond, m_millisecondExit, m_SpinTimeExit);
	UpdateTimer(TS_STOP, millisecond, m_millisecondStop, m_SpinTimeStop);
	UpdateTimer(TS_ENTRY, millisecond, m_millisecondEntry, m_SpinTimeEntry);
}

void BaseDialogSettingStrategyTimer::RestoreLayout()
{
	BaseDialogSettingStrategy::RestoreLayout();

	unsigned int millisecond;
	const UIntSetting* settingMillisecond = GetSettingMillisecondEntry();
	if(settingMillisecond)
	{
		millisecond = settingMillisecond->GetValue();
		if(millisecond != m_millisecondEntry)
		{
			m_millisecondEntry = millisecond;
			m_SpinTimeEntry.SetMillisecond(millisecond, false);
		}
	}

	settingMillisecond = GetSettingMillisecondStop();
	if(settingMillisecond)
	{
		millisecond = settingMillisecond->GetValue();
		if(millisecond != m_millisecondStop)
		{
			m_millisecondStop = millisecond;
			m_SpinTimeStop.SetMillisecond(millisecond, false);
		}
	}

	settingMillisecond = GetSettingMillisecondExit();
	if(settingMillisecond)
	{
		millisecond = settingMillisecond->GetValue();
		if(millisecond != m_millisecondExit)
		{
			m_millisecondExit = millisecond;
			m_SpinTimeExit.SetMillisecond(millisecond, false);
		}
	}

	settingMillisecond = GetSettingMillisecondStopExit();
	if(settingMillisecond)
	{
		millisecond = settingMillisecond->GetValue();
		if(millisecond != m_millisecondStopExit)
		{
			m_millisecondStopExit = millisecond;
			m_SpinTimeStopExit.SetMillisecond(millisecond, false);
		}
	}
	InitializeTradingState(TL_GetCurrentMillisecond());
}

void BaseDialogSettingStrategyTimer::UpdateSettings()
{
	BaseDialogSettingStrategy::UpdateSettings();

	UIntSetting* settingMillisecond = GetSettingMillisecondEntry();
	if(settingMillisecond)settingMillisecond->SetValue(m_millisecondEntry);

	settingMillisecond = GetSettingMillisecondStop();
	if(settingMillisecond)settingMillisecond->SetValue(m_millisecondStop);

	settingMillisecond = GetSettingMillisecondExit();
	if(settingMillisecond)settingMillisecond->SetValue(m_millisecondExit);

	settingMillisecond = GetSettingMillisecondStopExit();
	if(settingMillisecond)settingMillisecond->SetValue(m_millisecondStopExit);
}

void BaseDialogSettingStrategyTimer::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_SYSTEM_TIME_CHANGED:
		InitializeTradingState(TL_GetCurrentMillisecond());
		break;

		default:
		BaseDialogSettingStrategy::Notify(message, from, info);
		break;
	}
}
