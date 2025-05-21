#include "stdafx.h"
#include "SampleDialogSettingSimulateEtbList.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateEtbList::SampleDialogSettingSimulateEtbList(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab) :
	BaseDialogSettingSimulateFirm(mainWnd, parentTab, "EtbList"),
	m_ListBoxFlags(2, false, "Flags of the ETB List Message", true),
	m_CheckBoxDone("Last Message in a series of ETB List Messages", 0xFFFFFFFF, 0xFFFFFFFF, checkBoxColor, textCheckColor, pressedCheckColor),
	m_done(false),
	m_msg(0, 0, 0, sizeof(EtbNode), 1, false, 0)
{
	m_ListBoxFlags.AddStringItem("First", 0);
	m_ListBoxFlags.AddStringItem("Reload", 1);

	const int doubleWidth = m_firstColumnWidth + m_firstColumnWidth;
	const int priceWidth = m_firstColumnWidth + spinHeight * 11 / 8;
	const int priceNameWidth = doubleWidth - priceWidth;
	const int moneyWidth = m_firstColumnWidth + spinHeight * 24 / 8;
	const int moneyNameWidth = doubleWidth - moneyWidth;

	const int staticSpinYOffset = (spinHeight - staticHeight) / 2;
	const int staticComboYOffset = (comboBoxHeight - staticHeight) / 2;
	m_bottomControl += vgap;
	int y = m_bottomControl - spinHeight - vgap;
	int xs = m_xc + m_firstColumnWidth + 1;
	int xs2 = m_xc + m_firstColumnHalfWidth + 1;
	int xPriceS = m_xc + priceWidth + 1;

	const int itemHeight = 8;
	const int borderSize = 4;
	const unsigned int itemCountVisible = 2;
	const int flagsLbHeight = itemHeight * itemCountVisible + borderSize;

	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_OWNERDRAWFIXED|WS_VSCROLL, 0, m_xc, m_bottomControl += vgap, m_firstColumnWidth, flagsLbHeight, listBoxFlagsId);
	AddButtonIndirect("Done", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, m_xc, m_bottomControl += flagsLbHeight + vgap, m_firstColumnHalfWidth, checkBoxHeight, checkBoxDoneId);

	m_bottomControl += checkBoxHeight;
//	y += comboBoxHeight;
//	if(m_bottomControl < y)m_bottomControl = y;
}

const Message* SampleDialogSettingSimulateEtbList::CreateMessage(WSABUF& wsaBuf)
{
	m_msg.m_firmId = U_RepresentStringAsUnsignedNumber<unsigned int>(m_stockLoanFirm.c_str());
	m_msg.m_flags = m_ListBoxFlags.GetFilter();
	m_msg.m_done = m_done;
//	m_msg.m_nodes = 1;
//	m_msg.m_nodeSize = sizeof(EtbNode);
	m_msg.m_requestId = m_SpinRequestId.GetValue();
	m_msg.m_time = TL_GetCurrentMillisecond();

	U_CopyAndPad(m_etbNode.m_symbol, sizeof(m_etbNode.m_symbol), m_symbol.c_str(), '\0');

	wsaBuf.buf = (char*)&m_etbNode;
	wsaBuf.len = sizeof(m_etbNode);

	return &m_msg;
}

void SampleDialogSettingSimulateEtbList::RestoreLayout()
{
	m_EditSymbol.SetWindowText(extensionConfig->m_simEtbListSymbol.GetValue().c_str());

	SetAccountById(extensionConfig->m_simEtbListAccountId.GetValue().c_str());

	BaseDialogSettingSimulateFirm::RestoreLayout();

	m_ListBoxFlags.SetFilter(extensionConfig->m_simEtbListFlags.GetValue());

	const bool done = extensionConfig->m_simEtbListDone.GetValue();
	if(done != m_done)
	{
		m_done = done;
		m_CheckBoxDone.SetCheck(m_done ? BST_CHECKED : BST_UNCHECKED);
	}

	EnableSendMessageButton();

	m_modified = false;
}

BOOL SampleDialogSettingSimulateEtbList::OnInitDialog()
{
	m_done = false;
	m_ListBoxFlags.SetFilter(0);

	BOOL ret = BaseDialogSettingSimulateFirm::OnInitDialog();

//	RestoreLayout();

	return ret;
}

StringSetting* SampleDialogSettingSimulateEtbList::GetSettingFirm(){return &extensionConfig->m_simEtbListFirm;}
UIntSetting* SampleDialogSettingSimulateEtbList::GetSettingRequestId(){return &extensionConfig->m_simEtbListRequestId;}

void SampleDialogSettingSimulateEtbList::UpdateSettings()
{
	extensionConfig->m_simEtbListSymbol.SetValue(m_symbol);
	UpdateAccountInConfig(extensionConfig->m_simEtbListAccountId);

	BaseDialogSettingSimulateFirm::UpdateSettings();
	extensionConfig->m_simEtbListFlags.SetValue(m_ListBoxFlags.GetFilter());
	extensionConfig->m_simEtbListDone.SetValue(m_done);
}

void SampleDialogSettingSimulateEtbList::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateFirm::DoDataExchange(pDX);
	DDX_Control(pDX, listBoxFlagsId, m_ListBoxFlags);
	DDX_Control(pDX, checkBoxDoneId, m_CheckBoxDone);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateEtbList, BaseDialogSettingSimulateFirm)
//	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(checkBoxDoneId, OnDone)

//	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateEtbList::OnDone()
{
	const bool done = m_CheckBoxDone.GetCheck() == BST_CHECKED;
	if(done != m_done)
	{
		m_done = done;
		m_modified = true;
	}
}


/*
LRESULT SampleDialogSettingSimulateEtbList::OnSpinChange(WPARAM w, LPARAM l)
{

	switch(w)
	{
		case spinSlBorrowPriceId:
		m_modified = true;
		break;

		default:
		BaseDialogSettingSimulateFirm::OnSpinChange(w, l);
		break;
	}

	BaseDialogSettingSimulateFirm::OnSpinChange(w, l);
	return 0;
}
*/
bool SampleDialogSettingSimulateEtbList::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateFirm::isSendMessageButtonEnabled();// && m_ComboBoxHtb.GetCurSel() >= 0;
}
