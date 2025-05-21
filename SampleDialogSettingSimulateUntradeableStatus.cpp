#include "stdafx.h"
#include "SampleDialogSettingSimulateUntradeableStatus.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateUntradeablStatus::SampleDialogSettingSimulateUntradeablStatus(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab) :
	BaseDialogSettingSimulateAccountSymbol(mainWnd, parentTab, "Untradeable"),
	m_EditStockLoanFirm("`~!@#$%^&*(),<>.?/'\";:|\\][{}", true),
	m_EditTraderId(U_GetValidStockChars(), false),
	m_ComboBoxUntradeableStatus(false),
	m_untradeableStatus(0)
//	m_msg(0, 0, 0, sizeof(BorrowNode), 1, true)
{
//	const int doubleWidth = m_firstColumnWidth + m_firstColumnWidth;
	const int priceWidth = m_firstColumnWidth + spinHeight * 11 / 8;
//	const int priceNameWidth = doubleWidth - priceWidth;
//	const int moneyWidth = m_firstColumnWidth + spinHeight * 24 / 8;
//	const int moneyNameWidth = doubleWidth - moneyWidth;

//	m_msg.m_firmId[sizeof(m_msg.m_firmId) - 1] = '\0';

	m_bottomControl += vgap;
	AddButtonIndirect("Fill", WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, buttonHeight, buttonUsFillId);

	m_bottomControl += buttonHeight + vgap;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editUsFirmId);
	AddStaticIndirect("Firm", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticUsFirmId);

	m_bottomControl += editHeight + vgap;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editUsTraderId);
	AddStaticIndirect("TraderId", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticUsTraderId);

	m_bottomControl += editHeight + vgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL | WS_GROUP, 0, m_xc, m_bottomControl, priceWidth, 400, comboboxUsStatusId);
	AddStaticIndirect("Status", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, m_firstColumnHalfWidth, staticHeight, staticUsStatusId);

}

void SampleDialogSettingSimulateUntradeablStatus::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateAccountSymbol::DoDataExchange(pDX);

	DDX_Control(pDX, buttonUsFillId, m_ButtonFill);
	DDX_Control(pDX, editUsFirmId, m_EditStockLoanFirm);

	DDX_Control(pDX, editUsTraderId, m_EditTraderId);

	DDX_Control(pDX, comboboxUsStatusId, m_ComboBoxUntradeableStatus);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateUntradeablStatus, BaseDialogSettingSimulateAccountSymbol)
//	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(buttonUsFillId, OnFill)
	ON_EN_CHANGE(editUsFirmId, OnStockLoanFirmChange)
	ON_EN_UPDATE(editUsFirmId, OnStockLoanFirmUpdate)
	ON_EN_CHANGE(editUsTraderId, OnTraderIdChange)
	ON_EN_UPDATE(editUsTraderId, OnTraderIdUpdate)
	ON_CBN_SELCHANGE(comboboxUsStatusId, OnSelchangeStatus)
//	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateUntradeablStatus::OnSelchangeStatus()
{
	int sel = m_ComboBoxUntradeableStatus.GetCurSel();
	if(sel < 0)sel = 0;
	const unsigned char untradeableStatus = (unsigned char)sel;
	if(untradeableStatus != m_untradeableStatus)
	{
		m_untradeableStatus = untradeableStatus;
	}
}

void SampleDialogSettingSimulateUntradeablStatus::SymbolChanged()
{
//	EnableControl(m_ButtonFill.m_hWnd, !m_symbol.empty(), m_EditSymbol.m_hWnd);
}

void SampleDialogSettingSimulateUntradeablStatus::OnFill()
{
	if(m_account)
	{
		m_EditStockLoanFirm.SetWindowText(m_account->GetStockLoanFirmId());
	}
	char buf[sizeof(TraderIdType) + 1];
	*(TraderIdType*)buf = TD_GetNumericTraderIdPrintable();
	buf[sizeof(TraderIdType)] = '\0';
	m_EditTraderId.SetWindowText(buf);
//	m_EditTraderId.SetWindowText(TD_GetNumericTraderIdPrintable());
}

void SampleDialogSettingSimulateUntradeablStatus::OnStockLoanFirmUpdate()
{
	m_EditStockLoanFirm.TryUpdate();
}

void SampleDialogSettingSimulateUntradeablStatus::OnStockLoanFirmChange()
{
	const bool empty = m_stockLoanFirm.empty();
	CString text;
	m_EditStockLoanFirm.GetWindowText(text);
	m_stockLoanFirm = text;
	if(m_stockLoanFirm.empty() != empty)
	{
		m_EditStockLoanFirm.Invalidate(FALSE);
		EnableSendMessageButton();
	}
	m_modified = true;
}

void SampleDialogSettingSimulateUntradeablStatus::OnTraderIdUpdate()
{
	m_EditTraderId.TryUpdate();
}

void SampleDialogSettingSimulateUntradeablStatus::OnTraderIdChange()
{
	const bool empty = m_traderId.empty();
	CString text;
	m_EditTraderId.GetWindowText(text);
	m_traderId = text;
	if(m_traderId.empty() != empty)
	{
		m_EditTraderId.Invalidate(FALSE);
		EnableSendMessageButton();
	}
	m_modified = true;
}


bool SampleDialogSettingSimulateUntradeablStatus::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateAccountSymbol::isSendMessageButtonEnabled()
		&& !m_stockLoanFirm.empty()
		&& !m_traderId.empty()
		&& m_ComboBoxUntradeableStatus.GetCurSel() >= 0;
}

const Message* SampleDialogSettingSimulateUntradeablStatus::CreateMessage(WSABUF& wsaBuf)
{
	if(m_account)
	{
		m_msg.m_time = TL_GetCurrentMillisecond();

		U_CopyAndPad(m_msg.m_traderId, sizeof(m_msg.m_traderId), m_traderId.c_str(), '\0', true);

		U_CopyAndPad(m_msg.m_clearingAccountName, sizeof(m_msg.m_clearingAccountName), m_account->GetClearingAccountName(), '\0', true);
		U_CopyAndPad(m_msg.m_symbol, sizeof(m_msg.m_symbol), m_symbol.c_str(), '\0');
		U_CopyAndPad(m_msg.m_firmId, sizeof(m_msg.m_firmId), m_stockLoanFirm.c_str(), '\0', true);

		m_msg.m_untradeable = m_untradeableStatus;
		return &m_msg;
	}
	return nullptr;
}

BOOL SampleDialogSettingSimulateUntradeablStatus::OnInitDialog()
{
	m_stockLoanFirm.clear();
	m_traderId.clear();
	m_untradeableStatus = 0;
	BOOL ret = BaseDialogSettingSimulateAccountSymbol::OnInitDialog();

	m_EditStockLoanFirm.SetLimitText(4);
	m_EditTraderId.SetLimitText(4);

	m_ComboBoxUntradeableStatus.AddString("Tradeable");
	m_ComboBoxUntradeableStatus.AddString("FullUntradeable");
	m_ComboBoxUntradeableStatus.AddString("PartUntradeable");
	m_ComboBoxUntradeableStatus.SetCurSel(m_untradeableStatus);
//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateUntradeablStatus::UpdateSettings()
{
	extensionConfig->m_simUsSymbol.SetValue(m_symbol);
	UpdateAccountInConfig(extensionConfig->m_simUsAccountId);

	extensionConfig->m_simUsFirm.SetValue(m_stockLoanFirm);
	extensionConfig->m_simUsTraderId.SetValue(m_traderId);

	extensionConfig->m_simUsStatus.SetValue(m_untradeableStatus);
}

void SampleDialogSettingSimulateUntradeablStatus::RestoreLayout()
{
	m_EditSymbol.SetWindowText(extensionConfig->m_simUsSymbol.GetValue().c_str());

	SetAccountById(extensionConfig->m_simUsAccountId.GetValue().c_str());

	m_EditStockLoanFirm.SetWindowText(extensionConfig->m_simUsFirm.GetValue().c_str());
	m_EditTraderId.SetWindowText(extensionConfig->m_simUsTraderId.GetValue().c_str());

	m_untradeableStatus = extensionConfig->m_simUsStatus.GetValue();
	if((unsigned int)m_untradeableStatus > m_ComboBoxUntradeableStatus.GetItemCount())m_untradeableStatus = 0;
	m_ComboBoxUntradeableStatus.SetCurSel(m_untradeableStatus);
//	OnSelchangeStatus();

	EnableSendMessageButton();

	m_modified = false;
}

