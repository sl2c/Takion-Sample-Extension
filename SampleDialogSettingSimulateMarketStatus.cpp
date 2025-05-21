#include "stdafx.h"
#include "SampleDialogSettingSimulateMarketStatus.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateMarketStatus::SampleDialogSettingSimulateMarketStatus(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateMessage(mainWnd, parentTab, "Market Status"),
	m_ComboBoxMarketStatus(DT_CENTER),
	m_marketStatus('O'),
	m_symbolMapping(false),
	m_msg(m_marketStatus, ECNBOOK_NSDQ),
	m_msgSymbolMapping(m_marketStatus, ECNBOOK_NSDQ)
{
	m_bottomControl += vgap;
//	AddButtonIndirect("Enforce", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, combobBoxHeight, comboBoxMarketStatusId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, 200, comboBoxMarketStatusId);
	AddStaticIndirect("MarketStatus", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnHalfWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticMarketStatusId);

	m_bottomControl += editHeight + vgap;

	AddButtonIndirect("Symbol Mapping", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, m_bottomControl, m_firstColumnWidth + m_firstColumnHalfWidth, checkBoxHeight, checkBoxSymbolMappingId);
	m_bottomControl += checkBoxHeight;
}

void SampleDialogSettingSimulateMarketStatus::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateMessage::DoDataExchange(pDX);

	DDX_Control(pDX, comboBoxMarketStatusId, m_ComboBoxMarketStatus);
	DDX_Control(pDX, checkBoxSymbolMappingId, m_CheckBoxSymbolMapping);
}

BOOL SampleDialogSettingSimulateMarketStatus::OnInitDialog()
{
	m_marketStatus = 'O';
	m_symbolMapping = false;
	m_msg.m_eventCode = m_marketStatus;
	m_msgSymbolMapping.m_eventCode = m_marketStatus;

	BOOL ret = BaseDialogSettingSimulateMessage::OnInitDialog();

//	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("", "Uninitialized"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("O", "Message Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("S", "System Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("Q", "Market Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("M", "Post Market Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("E", "Post System Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("C", "Post Message Hours"));

//	RestoreLayout();

	return ret;
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateMarketStatus, BaseDialogSettingSimulateMessage)
	ON_CBN_SELCHANGE(comboBoxMarketStatusId, OnSelchangeMarketStatus)
	ON_BN_CLICKED(checkBoxSymbolMappingId, OnSymbolMapping)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateMarketStatus::OnSymbolMapping()
{
	const bool symbolMapping = m_CheckBoxSymbolMapping.GetCheck() == BST_CHECKED;
	if(symbolMapping != m_symbolMapping)
	{
		m_symbolMapping = symbolMapping;
	}
}

void SampleDialogSettingSimulateMarketStatus::OnSelchangeMarketStatus()
{
	int sel = m_ComboBoxMarketStatus.GetCurSel();
	const char marketStatus = sel >= 0 ?
		*((const NameDescriptionItem*)m_ComboBoxMarketStatus.GetItemDataPtr(sel))->GetName().c_str():
		'U';
	if(marketStatus != m_marketStatus)
	{
		m_marketStatus = marketStatus;
		m_modified = true;
	}
}

const Message* SampleDialogSettingSimulateMarketStatus::CreateMessage(WSABUF& wsaBuf)
{
	if(m_symbolMapping)
	{
		m_msgSymbolMapping.m_eventCode = m_marketStatus;
		return &m_msgSymbolMapping;
	}
	else
	{
		m_msg.m_eventCode = m_marketStatus;
		return &m_msg;
	}
}

void SampleDialogSettingSimulateMarketStatus::UpdateSettings()
{
	extensionConfig->m_simMarketStatus.SetValue(m_marketStatus);
	extensionConfig->m_simMarketStatusSymbolMapping.SetValue(m_symbolMapping);
}

void SampleDialogSettingSimulateMarketStatus::RestoreLayout()
{
	m_marketStatus = extensionConfig->m_simMarketStatus.GetValue();
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

	EnableSendMessageButton();

	m_modified = false;
}
//