#include "stdafx.h"
#include "BaseDialogSettingSimulateFirm.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulateFirm::BaseDialogSettingSimulateFirm(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseDialogSettingSimulateAccountSymbol(mainWnd, parentTab, label),
	m_SpinRequestId(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_EditStockLoanFirm("", true)// U_GetValidStockChars(), false),
{
	SetSizeSpinCells(m_SpinRequestId);

	const int doubleWidth = m_firstColumnWidth + m_firstColumnWidth;
//	const int priceWidth = m_firstColumnWidth + spinHeight * 3 / 2;
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
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinRequestIdId);
	AddStaticIndirect("RequestId", WS_VISIBLE | SS_NOPREFIX, 0, xs, m_bottomControl + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticRequestIdId);

	m_bottomControl += spinHeight + vgap;
	AddButtonIndirect("Fill", WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | WS_DISABLED, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, buttonHeight, buttonFillId);

	m_bottomControl += buttonHeight + vgap;
//	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editSlFirmId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editFirmId);
	AddStaticIndirect("Firm", WS_VISIBLE | SS_NOPREFIX, 0, xs, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticFirmId);

	m_bottomControl += editHeight;// +vgap;
}

void BaseDialogSettingSimulateFirm::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateAccountSymbol::DoDataExchange(pDX);

	DDX_Control(pDX, spinRequestIdId, m_SpinRequestId);

	DDX_Control(pDX, buttonFillId, m_ButtonFill);
	DDX_Control(pDX, editFirmId, m_EditStockLoanFirm);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateFirm, BaseDialogSettingSimulateAccountSymbol)
//	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(buttonFillId, OnFill)
	ON_EN_CHANGE(editFirmId, OnStockLoanFirmChange)
	ON_EN_UPDATE(editFirmId, OnStockLoanFirmUpdate)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void BaseDialogSettingSimulateFirm::SymbolChanged()
{
	EnableControl(m_ButtonFill.m_hWnd, !m_symbol.empty(), m_EditSymbol.m_hWnd);
}

void BaseDialogSettingSimulateFirm::FillFirmInfo()
{
	m_EditStockLoanFirm.SetWindowText(m_account->GetStockLoanFirmId());
}

void BaseDialogSettingSimulateFirm::OnStockLoanFirmUpdate()
{
	m_EditStockLoanFirm.TryUpdate();
}

void BaseDialogSettingSimulateFirm::OnStockLoanFirmChange()
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


void BaseDialogSettingSimulateFirm::OnFill()
{
	if(m_account)
	{
		FillFirmInfo();
	}
}

LRESULT BaseDialogSettingSimulateFirm::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinRequestIdId:
		m_modified = true;
		break;

//		default:
//		BaseDialogSettingSimulateAccountSymbol::OnSpinChange(w, l);
//		break;
	}
	return 0;
}

bool BaseDialogSettingSimulateFirm::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateAccountSymbol::isSendMessageButtonEnabled() && !m_stockLoanFirm.empty();
}
/*
void BaseDialogSettingSimulateFirm::SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000, true);
}
*/
BOOL BaseDialogSettingSimulateFirm::OnInitDialog()
{
	m_stockLoanFirm.clear();

	BOOL ret = BaseDialogSettingSimulateAccountSymbol::OnInitDialog();

	m_EditStockLoanFirm.SetLimitText(4);
//	RestoreLayout();

	return ret;
}

void BaseDialogSettingSimulateFirm::UpdateSettings()
{
	GetSettingFirm()->SetValue(m_stockLoanFirm);
	GetSettingRequestId()->SetValue(m_SpinRequestId.GetValue());
}

void BaseDialogSettingSimulateFirm::RestoreLayout()
{
//	m_EditSymbol.SetWindowText(extensionConfig->m_simSlSymbol.GetValue().c_str());

//	SetAccountById(extensionConfig->m_simSlAccountId.GetValue().c_str());

	m_EditStockLoanFirm.SetWindowText(GetSettingFirm()->GetValue().c_str());
	FillSpinControlFromUInt(GetSettingRequestId()->GetValue(), m_SpinRequestId, false);

	EnableSendMessageButton();//??

	m_modified = false;
}

