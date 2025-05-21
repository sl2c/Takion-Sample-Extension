#include "stdafx.h"
#include "SampleDialogSettingSimulateAdminBroadcast.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateAdminBroadcast::SampleDialogSettingSimulateAdminBroadcast(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateMessage(mainWnd, parentTab, "Admin Broadcast"),
//	m_textUpdated(false),
	m_msg(0)
{
	m_bottomControl += vgap;
	AddStaticIndirect("Admin Text", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, m_bottomControl, m_firstColumnWidth, staticHeight, staticTextId);
	m_bottomControl += staticHeight;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth + m_firstColumnWidth, editHeight, editTextId);
	m_bottomControl += editHeight + vgap;
	AddButtonIndirect("Reset", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, buttonHeight, buttonResetId);
	m_bottomControl += buttonHeight;
}

void SampleDialogSettingSimulateAdminBroadcast::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateMessage::DoDataExchange(pDX);

	DDX_Control(pDX, editTextId, m_EditText);
	DDX_Control(pDX, buttonResetId, m_ButtonReset);
}

BOOL SampleDialogSettingSimulateAdminBroadcast::OnInitDialog()
{
	m_text.clear();
	m_controlText.clear();
	m_modified = false;
	BOOL ret = BaseDialogSettingSimulateMessage::OnInitDialog();
	m_EditText.SetLimitText(0xFFFF - sizeof(TMsgText) - 1);
	EnableResetButton();
	return ret;
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateAdminBroadcast, BaseDialogSettingSimulateMessage)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editTextId, OnTextChange)
	ON_BN_CLICKED(buttonResetId, OnReset)
END_MESSAGE_MAP()

HBRUSH SampleDialogSettingSimulateAdminBroadcast::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editTextId:
		if(m_modified)//m_historicalChartLoadingToSave)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();//GetWarningBrush
		}
		break;
	}
	return BaseDialogSettingSimulateMessage::OnCtlColor(pDC, pWnd, nCtlColor);
}

void SampleDialogSettingSimulateAdminBroadcast::EnableResetButton()
{
	EnableControl(m_ButtonReset.m_hWnd, m_modified, m_EditText.m_hWnd);
}

void SampleDialogSettingSimulateAdminBroadcast::OnTextChange()
{
//	bool empty = m_symbol.empty();
//	bool symbolCurrent = m_security && !strcmp(m_security->GetSymbol(), m_symbol.c_str());
	CString text;
	m_EditText.GetWindowText(text);
	m_controlText = text;
	m_modified = m_controlText != m_text;
	m_EditText.Invalidate(FALSE);
	EnableResetButton();
}

void SampleDialogSettingSimulateAdminBroadcast::OnReset()
{
	DoReset();
}

bool SampleDialogSettingSimulateAdminBroadcast::DoReset()
{
	if(m_modified)
	{
		m_controlText = m_text;
		m_modified = false;
		m_EditText.SetWindowText(m_text.c_str());
		m_EditText.Invalidate(FALSE);
		EnableResetButton();
		return true;
	}
	return false;
}

const Message* SampleDialogSettingSimulateAdminBroadcast::CreateMessage(WSABUF& wsaBuf)
{
	if(m_modified)
	{
		m_text = m_controlText;
		m_modified = false;
		m_EditText.Invalidate(FALSE);
		EnableResetButton();
	}
	const unsigned short length = (unsigned short)m_text.length() + 1;
	m_msg.m_length = sizeof(m_msg) + length;
	wsaBuf.buf = (char*)m_text.c_str();
	wsaBuf.len = length;
	return &m_msg;
}

void SampleDialogSettingSimulateAdminBroadcast::UpdateSettings()
{
	extensionConfig->m_simAdminBroadcastText.SetValue(m_controlText);
}

void SampleDialogSettingSimulateAdminBroadcast::RestoreLayout()
{
	m_text = extensionConfig->m_simAdminBroadcastText.GetValue();
	m_EditText.SetWindowText(m_text.c_str());

	m_modified = false;
	EnableResetButton();
}
