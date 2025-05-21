#include "stdafx.h"
#include "SampleDialogSettingSimulateFirmPosition.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateFirmPosition::SampleDialogSettingSimulateFirmPosition(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateAccountSymbol(mainWnd, parentTab, "Firm Position"),
	m_SpinSize(999999, -999999, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPendingSellPassive(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPendingSellNonPassive(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPendingShortPassive(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPendingShortNonPassive(999999, 0, 0, 1000, 3, TakionSpin::charComma)
{
	SetSignedSizeSpinCells(m_SpinSize);
	SetSizeSpinCells(m_SpinPendingSellPassive);
	SetSizeSpinCells(m_SpinPendingSellNonPassive);
	SetSizeSpinCells(m_SpinPendingShortPassive);
	SetSizeSpinCells(m_SpinPendingShortNonPassive);

	const int staticX = m_xc + m_firstColumnWidth + hgap;
	m_bottomControl += vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinFirmPositionSizeId);
	AddStaticIndirect("Size", WS_VISIBLE | SS_NOPREFIX, 0, staticX, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticFirmPositionSizeId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinFirmPositionPendingSellPassiveId);
	AddStaticIndirect("SellPassive", WS_VISIBLE | SS_NOPREFIX, 0, staticX, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticFirmPositionPendingSellPassiveId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinFirmPositionPendingSellNonPassiveId);
	AddStaticIndirect("SellNonPassive", WS_VISIBLE | SS_NOPREFIX, 0, staticX, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticFirmPositionPendingSellNonPassiveId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinFirmPositionPendingShortPassiveId);
	AddStaticIndirect("ShortPassive", WS_VISIBLE | SS_NOPREFIX, 0, staticX, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticFirmPositionPendingShortPassiveId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinFirmPositionPendingShortNonPassiveId);
	AddStaticIndirect("ShortNonPassive", WS_VISIBLE | SS_NOPREFIX, 0, staticX, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticFirmPositionPendingShortNonPassiveId);
}
/*
void SampleDialogSettingSimulateFirmPosition::SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000, true);
}

void SampleDialogSettingSimulateFirmPosition::SetSignedSizeSpinCells(SignedNumberTypeSpin<int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000, true);
}
*/
void SampleDialogSettingSimulateFirmPosition::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateAccountSymbol::DoDataExchange(pDX);

	DDX_Control(pDX, spinFirmPositionSizeId, m_SpinSize);
	DDX_Control(pDX, spinFirmPositionPendingSellPassiveId, m_SpinPendingSellPassive);
	DDX_Control(pDX, spinFirmPositionPendingSellNonPassiveId, m_SpinPendingSellNonPassive);
	DDX_Control(pDX, spinFirmPositionPendingShortPassiveId, m_SpinPendingShortPassive);
	DDX_Control(pDX, spinFirmPositionPendingShortNonPassiveId, m_SpinPendingShortNonPassive);
}

BOOL SampleDialogSettingSimulateFirmPosition::OnInitDialog()
{
	BOOL ret = BaseDialogSettingSimulateAccountSymbol::OnInitDialog();

//	RestoreLayout();

	return ret;
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateFirmPosition, BaseDialogSettingSimulateAccountSymbol)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

LRESULT SampleDialogSettingSimulateFirmPosition::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinFirmPositionSizeId:
		case spinFirmPositionPendingSellPassiveId:
		case spinFirmPositionPendingSellNonPassiveId:
		case spinFirmPositionPendingShortPassiveId:
		case spinFirmPositionPendingShortNonPassiveId:

		m_modified = true;
		break;
	}
	return 0;
}

const Message* SampleDialogSettingSimulateFirmPosition::CreateMessage(WSABUF& wsaBuf)
{
	if(m_account)
	{
		U_CopyAndPad(m_msg.m_symbol, sizeof(m_msg.m_symbol), m_symbol.c_str(), '\0');
		m_msg.m_time = TL_GetCurrentMillisecond();

		U_CopyAndPad(m_msg.m_clearingAccountName, sizeof(m_msg.m_clearingAccountName), m_account->GetClearingAccountName(), '\0');
//		U_CopyAndPad(m_msg.m_firmId, sizeof(m_msg.m_firmId), m_account->GetClearingFirmId(), '\0');
		U_CopyAndPad(m_msg.m_firmId, sizeof(m_msg.m_firmId), m_account->GetStockLoanFirmId(), '\0');

		m_msg.m_firmPosition = m_SpinSize.GetValue();
		m_msg.m_firmPendingSellSharesPassive = m_SpinPendingSellPassive.GetValue();
		m_msg.m_firmPendingSellSharesNonPassive = m_SpinPendingSellNonPassive.GetValue();
		m_msg.m_firmPendingShortSharesPassive = m_SpinPendingShortPassive.GetValue();
		m_msg.m_firmPendingShortSharesNonPassive = m_SpinPendingShortNonPassive.GetValue();

		return &m_msg;
	}
	return nullptr;
}

void SampleDialogSettingSimulateFirmPosition::UpdateSettings()
{
	extensionConfig->m_simFirmPositionSymbol.SetValue(m_symbol);
	UpdateAccountInConfig(extensionConfig->m_simFirmPositionAccountId);

	extensionConfig->m_simFirmPositionSize.SetValue(m_SpinSize.GetValue());
	extensionConfig->m_simFirmPositionPendingSellPassive.SetValue(m_SpinPendingSellPassive.GetValue());
	extensionConfig->m_simFirmPositionPendingSellNonPassive.SetValue(m_SpinPendingSellNonPassive.GetValue());
	extensionConfig->m_simFirmPositionPendingShortPassive.SetValue(m_SpinPendingShortPassive.GetValue());
	extensionConfig->m_simFirmPositionPendingShortNonPassive.SetValue(m_SpinPendingShortNonPassive.GetValue());
}

void SampleDialogSettingSimulateFirmPosition::RestoreLayout()
{
	m_EditSymbol.SetWindowText(extensionConfig->m_simFirmPositionSymbol.GetValue().c_str());

	SetAccountById(extensionConfig->m_simFirmPositionAccountId.GetValue().c_str());

	m_SpinSize.SetValue(extensionConfig->m_simFirmPositionSize.GetValue(), false, true, false);
	m_SpinPendingSellPassive.SetValue(extensionConfig->m_simFirmPositionPendingSellPassive.GetValue(), false, true, false);
	m_SpinPendingSellNonPassive.SetValue(extensionConfig->m_simFirmPositionPendingSellNonPassive.GetValue(), false, true, false);
	m_SpinPendingShortPassive.SetValue(extensionConfig->m_simFirmPositionPendingShortPassive.GetValue(), false, true, false);
	m_SpinPendingShortNonPassive.SetValue(extensionConfig->m_simFirmPositionPendingShortNonPassive.GetValue(), false, true, false);

	EnableSendMessageButton();

	m_modified = false;
}
