#include "stdafx.h"
#include "SampleDialogSettingSimulateNewDay.h"
//#include "ExtensionDerivedConfig.h"

//extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateNewDay::SampleDialogSettingSimulateNewDay(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateMessage(mainWnd, parentTab, "NewDay", 80),
#ifndef TAKION_NO_OPTIONS
	m_ButtonNewFutureSession("Simulate \"NewFuturesSession\" message. (Works only in simulation)"),
#endif
	m_ButtonShutdown("Shutdown Takion")
{
	m_ButtonSendMessage.SetTooltipText("Simulate \"NewDay\" message. (Works only in simulation)");

#ifndef TAKION_NO_OPTIONS
	AddButtonIndirect("New Future Session", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, m_xc, m_bottomControl += vgap, m_sendButtonWidth, buttonHeight, buttonNewFutureSessionId);
	m_bottomControl += buttonHeight;
#endif

	AddButtonIndirect("Shutdown", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, m_xc, m_bottomControl += vgap, m_sendButtonWidth, buttonHeight, buttonShutdownId);
	m_bottomControl += buttonHeight;
}

BOOL SampleDialogSettingSimulateNewDay::OnInitDialog()
{
	BOOL ret = BaseDialogSettingSimulateMessage::OnInitDialog();

	m_ButtonSendMessage.SetWindowText("New Day");

//	RestoreLayout();

	return ret;
}
/*
void SampleDialogSettingSimulateNewDay::UpdateSettings()
{
	EnableApplyButton(false);
}
*/
void SampleDialogSettingSimulateNewDay::RestoreLayout()
{
	EnableApplyButton(false);
	EnableSendMessageButton();
	m_modified = false;
}


void SampleDialogSettingSimulateNewDay::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateMessage::DoDataExchange(pDX);
#ifndef TAKION_NO_OPTIONS
	DDX_Control(pDX, buttonNewFutureSessionId, m_ButtonNewFutureSession);
#endif
	DDX_Control(pDX, buttonShutdownId, m_ButtonShutdown);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateNewDay, BaseDialogSettingSimulateMessage)
	ON_BN_CLICKED(buttonSendMessageId, OnSendMessage)
#ifndef TAKION_NO_OPTIONS
	ON_BN_CLICKED(buttonNewFutureSessionId, OnNewFutureSession)
#endif
	ON_BN_CLICKED(buttonShutdownId, OnShutdown)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateNewDay::OnSendMessage()
{
	TD_SimulateNextDayStarted();
}

#ifndef TAKION_NO_OPTIONS
void SampleDialogSettingSimulateNewDay::OnNewFutureSession()
{
	TD_SimulateFutureSessionEnded();
}
#endif

void SampleDialogSettingSimulateNewDay::OnShutdown()
{
/*
	OnSendMessage();
/**/
	::PostMessage(TGUI_GetTakionMainWnd()->m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
}

bool SampleDialogSettingSimulateNewDay::isSendMessageButtonEnabled() const
{
	return true;
}

