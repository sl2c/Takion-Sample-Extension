#include "stdafx.h"
#include "SampleDialogSettingSimulateMdLogon.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateMdLogon::SampleDialogSettingSimulateMdLogon(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateMessage(mainWnd, parentTab, "MD Logon"),
	m_EditUserId("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", false),
	m_ComboBoxCompression(false),
	m_ComboBoxInstrument(false),
	m_ComboBoxResult(false),
//	m_ListBoxEntitlements(31, true),
//	m_ListBoxExtEntitlements(31, true),
	m_ListBoxEntitlements(63, true),
	m_ListBoxExtEntitlements(63, true),
	m_version(0),//U_Version64To32(TD_GetMarketDataVersion())),
	m_versionValid(false)
//	m_msg(TD_GetTraderId())
{
	m_bottomControl += vgap;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editMdLogonUserId);
	AddStaticIndirect("User ID", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticMdLogonUserId);

	AddComponentIndirect("SysIPAddress32", nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP | WS_GROUP, 0, m_xc, m_bottomControl += editHeight + vgap, m_firstColumnWidth + 2 * editHeight, editHeight, ipMdLogonVersionId);
	AddStaticIndirect("MD Version", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth + 2 * editHeight, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth - editHeight, staticHeight, staticMdLogonVersionId);

	AddButtonIndirect("Initialize", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, m_xc, m_bottomControl += editHeight + vgap, m_firstColumnWidth, buttonHeight, buttonMdLogonInitializeId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += buttonHeight + vgap, m_firstColumnWidth, 200, comboBoxMdLogonCompressionId);
	AddStaticIndirect("Compression", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticMdLogonCompressionId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += editHeight + vgap, m_firstColumnWidth, 200, comboBoxMdLogonInstrumentId);
	AddStaticIndirect("Instrument", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticMdLogonInstrumentId);

	const int oneAndHalf = m_firstColumnWidth * 3 / 2;
	AddButtonIndirect("SubscriptionOnly", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | WS_GROUP, 0, m_xc, m_bottomControl += editHeight + vgap, oneAndHalf, checkBoxHeight, checkBoxMdLogonSubscriptionOnlyId);
	AddButtonIndirect("ForceSubscriptionOnly", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | WS_GROUP, 0, m_xc, m_bottomControl += checkBoxHeight + vgap, oneAndHalf + editHeight, checkBoxHeight, checkBoxMdLogonForseSubscriptionOnlyId);
	AddButtonIndirect("Disable Extensions", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | WS_GROUP, 0, m_xc, m_bottomControl += checkBoxHeight + vgap, oneAndHalf + editHeight, checkBoxHeight, checkBoxMdLogonDisableExtensionsId);

//	AddButtonIndirect("Enforce", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, combobBoxHeight, comboBoxMarketStatusId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += checkBoxHeight + vgap, m_firstColumnWidth, 200, comboBoxMdLogonResultId);
	AddStaticIndirect("Result", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticMdLogonResultId);

	m_bottomControl += editHeight + vgap;
	const int borderSize = 2;
	const int lbHeight = MDE_Count * checkBoxHeight + borderSize;
	const int xx = m_xc + m_firstColumnWidth + hgap;
	AddStaticIndirect("Entitlements", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, m_bottomControl, m_firstColumnWidth, staticHeight, staticMdLogonEntitlementsId);
	AddStaticIndirect("ExtEntitlements", WS_VISIBLE | SS_NOPREFIX, 0, xx, m_bottomControl, m_firstColumnWidth, staticHeight, staticMdLogonExtEntitlementsId);

	m_bottomControl += staticHeight;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, lbHeight, listBoxMdLogonEntitlementsId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xx, m_bottomControl, m_firstColumnWidth, lbHeight, listBoxMdLogonExtEntitlementsId);

	m_bottomControl += lbHeight;// + vgap;

	AddResizeControl(new VerticalResizeControl(this, listBoxMdLogonEntitlementsId));
	AddResizeControl(new VerticalResizeControl(this, listBoxMdLogonExtEntitlementsId));

	AddListBox(&m_ListBoxEntitlements);
	AddListBox(&m_ListBoxExtEntitlements);

	FillListBoxEntitlements(m_ListBoxEntitlements);
	FillListBoxEntitlements(m_ListBoxExtEntitlements);
}

void SampleDialogSettingSimulateMdLogon::FillListBoxEntitlements(ListBoxFilterStringSameColor<unsigned __int64>& listBox)
{
/*
	listBox.AddStringItem("NSDQ", MDE_NSDQ, nullptr);
	listBox.AddStringItem("ARCA", MDE_ARCA, nullptr);
	listBox.AddStringItem("BATS", MDE_BATS, nullptr);
	listBox.AddStringItem("BATY", MDE_BATY, nullptr);
	listBox.AddStringItem("EDGA", MDE_EDGA, nullptr);
	listBox.AddStringItem("EDGX", MDE_EDGX, nullptr);
	listBox.AddStringItem("UQDF", MDE_UQDF, nullptr);
	listBox.AddStringItem("UTDF", MDE_UTDF, nullptr);
	listBox.AddStringItem("CQS", MDE_CQS, nullptr);
	listBox.AddStringItem("CTS", MDE_CTS, nullptr);
	listBox.AddStringItem("NYSE", MDE_NYSE, nullptr);
	listBox.AddStringItem("AMEX", MDE_AMEX, nullptr);
	listBox.AddStringItem("OPRA", MDE_OPRA, nullptr);
*/
	for(unsigned char entitlementId = 0; entitlementId < MDE_Count; ++entitlementId)
	{
		listBox.AddStringItem(TU_GetEntitlementName(entitlementId), entitlementId);
	}
}

void SampleDialogSettingSimulateMdLogon::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateMessage::DoDataExchange(pDX);

	DDX_Control(pDX, editMdLogonUserId, m_EditUserId);
	DDX_Control(pDX, ipMdLogonVersionId, m_IpVersion);
	DDX_Control(pDX, comboBoxMdLogonCompressionId, m_ComboBoxCompression);
	DDX_Control(pDX, comboBoxMdLogonInstrumentId, m_ComboBoxInstrument);
	DDX_Control(pDX, comboBoxMdLogonResultId, m_ComboBoxResult);
	DDX_Control(pDX, checkBoxMdLogonSubscriptionOnlyId, m_CheckBoxSubscriptionOnly);
	DDX_Control(pDX, checkBoxMdLogonForseSubscriptionOnlyId, m_CheckBoxForceSubscriptionOnly);
	DDX_Control(pDX, checkBoxMdLogonDisableExtensionsId, m_CheckBoxDisableExtensions);
	DDX_Control(pDX, listBoxMdLogonEntitlementsId, m_ListBoxEntitlements);
	DDX_Control(pDX, listBoxMdLogonExtEntitlementsId, m_ListBoxExtEntitlements);
}

BOOL SampleDialogSettingSimulateMdLogon::OnInitDialog()
{
	m_userId.clear();// = TD_GetTraderId();
	m_version = 0;//U_Version64To32(TD_GetMarketDataVersion());
	m_versionValid = false;
	m_ListBoxEntitlements.SetFilter(0);
	m_ListBoxExtEntitlements.SetFilter(0);

	BOOL ret = BaseDialogSettingSimulateMessage::OnInitDialog();
/*
	NoCompression		= 0x00,
	FromClientToServer	= 0x01,
	FromServerToClient	= 0x02,
	FullCompression		= 0x03
*/
	m_ComboBoxCompression.AddString("None");
	m_ComboBoxCompression.AddString("ClientServer");
	m_ComboBoxCompression.AddString("ServerClient");
	m_ComboBoxCompression.AddString("Full");
	m_ComboBoxCompression.SetCurSel(0);

	m_ComboBoxInstrument.AddString("Stocks(MD)");
	m_ComboBoxInstrument.AddString("Options");
	m_ComboBoxInstrument.AddString("MS");
	m_ComboBoxInstrument.AddString("Futures");
	m_ComboBoxInstrument.SetCurSel(0);

	m_ComboBoxResult.AddString("Succeeded");
	m_ComboBoxResult.AddString("TimedOut");
	m_ComboBoxResult.AddString("Failed");
	m_ComboBoxResult.SetCurSel(0);

//	OnInitialize();
	char buf[sizeof(TraderIdType) + 1];
	*(TraderIdType*)buf = TD_GetNumericTraderIdPrintable();
	buf[sizeof(TraderIdType)] = '\0';
	m_EditUserId.SetWindowText(buf);
//	m_EditUserId.SetWindowText(TD_GetTraderId());

	return ret;
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateMdLogon, BaseDialogSettingSimulateMessage)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(buttonMdLogonInitializeId, OnInitialize)
	ON_EN_CHANGE(ipMdLogonVersionId, OnIpVersionChange)
	ON_EN_CHANGE(editMdLogonUserId, OnUserIdChange)
	ON_EN_UPDATE(editMdLogonUserId, OnUserIdUpdate)

	ON_CBN_SELCHANGE(comboBoxMdLogonCompressionId, OnModified)
	ON_CBN_SELCHANGE(comboBoxMdLogonInstrumentId, OnModified)
	ON_CBN_SELCHANGE(comboBoxMdLogonResultId, OnModified)
	ON_BN_CLICKED(checkBoxMdLogonSubscriptionOnlyId, OnModified)
	ON_BN_CLICKED(checkBoxMdLogonForseSubscriptionOnlyId, OnModified)
	ON_BN_CLICKED(checkBoxMdLogonDisableExtensionsId, OnModified)

	ON_MESSAGE(WM_USER + 17, OnWmUserModified)
	ON_MESSAGE(WM_USER + 18, OnWmUserModified)//OnListBoxRangeChecked)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateMdLogon::OnUserIdChange()
{
	bool empty = m_userId.empty();
	CString text;
	m_EditUserId.GetWindowText(text);
	m_userId = text;
	if(m_userId.empty() != empty)
	{
		m_EditUserId.Invalidate(FALSE);
		EnableSendMessageButton();
	}
	m_modified = true;
}

void SampleDialogSettingSimulateMdLogon::OnUserIdUpdate()
{
	m_EditUserId.TryUpdate();
}

void SampleDialogSettingSimulateMdLogon::OnInitialize()
{
//	SetIpVersion(U_UIntSwapBytes(U_Version64To32(TD_GetMarketDataVersion())));
	SetIpVersion(U_Version64To32(TD_GetMarketDataVersion()));

	char buf[sizeof(TraderIdType) + 1];
	*(TraderIdType*)buf = TD_GetNumericTraderIdPrintable();
	buf[sizeof(TraderIdType)] = '\0';
	m_EditUserId.SetWindowText(buf);
//	m_EditUserId.SetWindowText(TD_GetTraderId());

	if(m_ListBoxEntitlements.SetFilter(TD_GetMarketDataEntitlements()))
	{
		m_modified = true;
	}
	if(m_ListBoxExtEntitlements.SetFilter(TD_GetExtMarketDataEntitlements()))
	{
		m_modified = true;
	}
}

void SampleDialogSettingSimulateMdLogon::IpChange(ColorIPAddrCtrl& ipControl, bool& valid, DWORD& ipAddress)
{
//	m_modified = true;
	const bool val = ipControl.GetAddress(ipAddress) == 4 && ipAddress != 0;
//	if(valid != (ipControl.GetAddress(ipAddress) == 4))
	if(val != valid)
	{
		valid = val;//!valid;
		ipControl.Invalidate(FALSE);
//		UpdateConnectionIpColor();
//		EnableConnectButton();
	}
//	ClearFailed();
}

void SampleDialogSettingSimulateMdLogon::OnIpVersionChange()
{
	m_modified = true;
	bool wasValid = m_versionValid;
	IpChange(m_IpVersion, m_versionValid, m_version);
	if(wasValid != m_versionValid)
	{
		m_IpVersion.SetBkColor(m_versionValid ? 0xFFFFFFFF : m_visualBase->GetWarningColor());
		EnableSendMessageButton();
	}
}

void SampleDialogSettingSimulateMdLogon::SetIpVersion(const unsigned int version)
{
	if(m_IpVersion.m_hWnd)
	{
		if(version != m_version)
		{
			m_IpVersion.SetAddress(version);
			OnIpVersionChange();
		}
	}
}

const Message* SampleDialogSettingSimulateMdLogon::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
		U_CopyAndPad(m_msg.m_userId, sizeof(m_msg.m_userId), m_userId.c_str(), '\0');
		m_msg.m_marketDataEntitlements = m_ListBoxEntitlements.GetFilter();
		m_msg.m_extMarketDataEntitlements = m_ListBoxExtEntitlements.GetFilter();
//		U_ConvertIpToString(U_UIntSwapBytes(m_version), m_msg.m_serverVersion, sizeof(m_msg.m_serverVersion));
		U_ConvertIpToString(m_version, m_msg.m_serverVersion, sizeof(m_msg.m_serverVersion));

		m_msg.m_compression = m_ComboBoxCompression.GetCurSel();
		if(m_msg.m_compression > FullCompression)m_msg.m_compression = FullCompression;

		switch(m_ComboBoxInstrument.GetCurSel())
		{
			case 1:
			m_msg.m_requestId = MDLR_MARKET_DATA_OPTION;//2;
			break;

			case 2:
			m_msg.m_requestId = MDLR_MARKET_SORTER_STOCK;//3;
			break;

			case 3:
			m_msg.m_requestId = MDLR_MARKET_DATA_FUTURE;//4;
			break;

			default:
			m_msg.m_requestId = MDLR_MARKET_DATA_STOCK;//0;
			break;
		}

		switch(m_ComboBoxResult.GetCurSel())
		{
			case 0:
			m_msg.m_flags = TMsgMdLogonResponse::SUCCEEDED;
			break;

			case 1:
			m_msg.m_flags = TMsgMdLogonResponse::TIMED_OUT;
			break;

			default:
			m_msg.m_flags = TMsgMdLogonResponse::FAILED;
			break;
		}

		if(m_CheckBoxSubscriptionOnly.GetCheck() == BST_CHECKED)
		{
			m_msg.m_flags |= TMsgMdLogonResponse::SUBSCRIPTION_ONLY;
		}
		if(m_CheckBoxForceSubscriptionOnly.GetCheck() == BST_CHECKED)
		{
			m_msg.m_flags |= TMsgMdLogonResponse::FORCE_MS_SUBSCRIPTION_ONLY;
		}
		if(m_CheckBoxDisableExtensions.GetCheck() == BST_CHECKED)
		{
			m_msg.m_flags |= TMsgMdLogonResponse::DISABLE_EXTENSIONS;
		}
		return &m_msg;
	}
	return nullptr;
}

void SampleDialogSettingSimulateMdLogon::SaveComboBoxSelection(const ComboBoxString& comboBox, UIntSetting& uintSetting)
{
	const int sel = comboBox.GetCurSel();
	uintSetting.SetValue(sel >= 0 ? sel : 0);
}

void SampleDialogSettingSimulateMdLogon::RestoreComboBoxSelection(ComboBoxString& comboBox, const UIntSetting& uintSetting)
{
	const unsigned int sel = uintSetting.GetValue();
	comboBox.SetCurSel(sel < comboBox.GetItemCount() ? sel : -1);
}

void SampleDialogSettingSimulateMdLogon::UpdateSettings()
{
	extensionConfig->m_simMdLogonMdVersion.SetValue(m_versionValid ? m_version : 0);
	SaveComboBoxSelection(m_ComboBoxCompression, extensionConfig->m_simMdLogonCompression);
	SaveComboBoxSelection(m_ComboBoxInstrument, extensionConfig->m_simMdLogonInstrument);
	SaveComboBoxSelection(m_ComboBoxResult, extensionConfig->m_simMdLogonResult);

	extensionConfig->m_simMdLogonSubscriptionOnly.SetValue(m_CheckBoxSubscriptionOnly.GetCheck() == BST_CHECKED);
	extensionConfig->m_simMdLogonForceSubscriptionOnly.SetValue(m_CheckBoxForceSubscriptionOnly.GetCheck() == BST_CHECKED);
	extensionConfig->m_simMdLogonDisableExtensions.SetValue(m_CheckBoxDisableExtensions.GetCheck() == BST_CHECKED);

	extensionConfig->m_simMdLogonEntitlements.SetValue(m_ListBoxEntitlements.GetFilter());
	extensionConfig->m_simMdLogonExtEntitlements.SetValue(m_ListBoxExtEntitlements.GetFilter());
}

void SampleDialogSettingSimulateMdLogon::RestoreLayout()
{
/*
	m_marketStatus = extensionConfig->m_simMarketStatus;
	char name[2];
	*name = m_marketStatus;
	*(name + 1) = '\0';
	int found = m_ComboBoxMarketStatus.FindItemByName(name);
	if(found < 0)
	{
		if(*name != 'O')
		{
			*name = 'O';
			found = m_ComboBoxMarketStatus.FindItemByName(name);
			if(found < 0)found = 0;
		}
		else
		{
			found = 0;
		}
	}
	m_ComboBoxMarketStatus.SetCurSel(found);

	m_symbolMapping = extensionConfig->m_simMarketStatusSymbolMapping.GetValue();
	m_CheckBoxSymbolMapping.SetCheck(m_symbolMapping ? BST_CHECKED : BST_UNCHECKED);

	OnSelchangeMarketStatus();
*/
	SetIpVersion(extensionConfig->m_simMdLogonMdVersion.GetValue());
	extensionConfig->m_simMdLogonMdVersion.SetValue(m_versionValid ? m_version : 0);

	RestoreComboBoxSelection(m_ComboBoxCompression, extensionConfig->m_simMdLogonCompression);
	RestoreComboBoxSelection(m_ComboBoxInstrument, extensionConfig->m_simMdLogonInstrument);
	RestoreComboBoxSelection(m_ComboBoxResult, extensionConfig->m_simMdLogonResult);

	m_CheckBoxSubscriptionOnly.SetCheck(extensionConfig->m_simMdLogonSubscriptionOnly.GetValue());
	m_CheckBoxForceSubscriptionOnly.SetCheck(extensionConfig->m_simMdLogonForceSubscriptionOnly.GetValue());
	m_CheckBoxDisableExtensions.SetCheck(extensionConfig->m_simMdLogonDisableExtensions.GetValue());

	m_ListBoxEntitlements.SetFilter(extensionConfig->m_simMdLogonEntitlements.GetValue());
	m_ListBoxExtEntitlements.SetFilter(extensionConfig->m_simMdLogonExtEntitlements.GetValue());

	EnableSendMessageButton();

	m_modified = false;

}

HBRUSH SampleDialogSettingSimulateMdLogon::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editMdLogonUserId:
		if(m_userId.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		break;
	}
	return BaseDialogSettingSimulateMessage::OnCtlColor(pDC, pWnd, nCtlColor);
}
