#include "stdafx.h"
#include "SampleDialogSettingSimulateStockLoanInfo.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateStockLoanInfo::SampleDialogSettingSimulateStockLoanInfo(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab) :
	BaseDialogSettingSimulateFirm(mainWnd, parentTab, "StockLoan"),
//	m_SpinRequestId(999999, 0, 0, 1000, 3, TakionSpin::charComma),
//	m_EditStockLoanFirm("", true),// U_GetValidStockChars(), false),

	m_SpinBorrowPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 6),
	m_SpinBorrowSize(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinBorrowDiscount(255, 0, 0),
//	m_ComboBoxHtb(),
	m_SpinBeta(9999999999, -9999999999, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinMargin(255, 0, 0),
	m_SpinMePmElRate(255, 0, 0),
	m_SpinMePmEsRate(255, 0, 0),
	m_SpinMePmHlRate(255, 0, 0),
	m_SpinMePmHsRate(255, 0, 0),
	m_SpinMeElRate(255, 0, 0),
	m_SpinMeEsRate(255, 0, 0),
	m_SpinMeHlRate(255, 0, 0),
	m_SpinMeHsRate(255, 0, 0),
	m_SpinMeFedReq(100, 0, 0),
	m_onTheFly(false),
	m_borrow(false),
	m_marginable(false),
	m_htb('\0'),
	m_assetType('\0'),
	m_secType('\0'),
	m_status('\0'),
	m_msg(0, 0, 0, sizeof(BorrowNode), 1, true)
{
	m_msg.m_length = sizeof(m_msg) + sizeof(m_borrowNode);

	m_msgHtb.m_length = sizeof(m_msgHtb) + sizeof(m_htbNode);

	m_msgOnTheFlyBorrow.m_locateMPID = *(unsigned int*)"QSTK";

//	SetSizeSpinCells(m_SpinRequestId);
	SetSizeSpinCells(m_SpinBorrowSize);

	SetPriceSpinCells(m_SpinBorrowPrice);

	SetSignedMoneySpinCells(m_SpinBeta);

	m_SpinBorrowDiscount.AddSpinCell(1, false);
	m_SpinMargin.AddSpinCell(1, false);

	m_SpinMePmElRate.AddSpinCell(1, false);
	m_SpinMePmEsRate.AddSpinCell(1, false);
	m_SpinMePmHlRate.AddSpinCell(1, false);
	m_SpinMePmHsRate.AddSpinCell(1, false);
	m_SpinMeElRate.AddSpinCell(1, false);
	m_SpinMeEsRate.AddSpinCell(1, false);
	m_SpinMeHlRate.AddSpinCell(1, false);
	m_SpinMeHsRate.AddSpinCell(1, false);
	m_SpinMeFedReq.AddSpinCell(1, false);

	const int doubleWidth = m_firstColumnWidth + m_firstColumnWidth;
//	const int priceWidth = m_firstColumnWidth + spinHeight * 3 / 2;
	const int priceWidth = m_firstColumnWidth + spinHeight * 11 / 8;
	const int priceNameWidth = doubleWidth - priceWidth;
	const int moneyWidth = m_firstColumnWidth + spinHeight * 24 / 8;// +18;
	const int moneyNameWidth = doubleWidth - moneyWidth;

	const int staticSpinYOffset = (spinHeight - staticHeight) / 2;
	const int staticComboYOffset = (comboBoxHeight - staticHeight) / 2;
	m_bottomControl += vgap;
	int y = m_bottomControl - spinHeight - vgap;
	int xs = m_xc + m_firstColumnWidth + 1;
	int xs2 = m_xc + m_firstColumnHalfWidth + 1;
	int xPriceS = m_xc + priceWidth + 1;
/*
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinSlRequestIdId);
	AddStaticIndirect("RequestId", WS_VISIBLE | SS_NOPREFIX, 0, xs, m_bottomControl + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticSlRequestIdId);

	m_bottomControl += spinHeight + vgap;
	AddButtonIndirect("Fill", WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP | WS_DISABLED, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, buttonHeight, buttonSlFillId);

	m_bottomControl += buttonHeight + vgap;
//	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editSlFirmId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editSlFirmId);
	AddStaticIndirect("Firm", WS_VISIBLE | SS_NOPREFIX, 0, xs, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticSlFirmId);
	m_bottomControl += editHeight + vgap;
*/

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSlBorrowPriceId);
	AddStaticIndirect("$Borrow", WS_VISIBLE | SS_NOPREFIX, 0, xPriceS, m_bottomControl + staticSpinYOffset, priceNameWidth, staticHeight, staticSlBorrowPriceId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinSlBorrowSizeId);
	AddStaticIndirect("AvailableSize", WS_VISIBLE | SS_NOPREFIX, 0, xs, m_bottomControl + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticSlBorrowSizeId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, spinHeight, spinSlBorrowDiscountId);
	AddStaticIndirect("Discount", WS_VISIBLE | SS_NOPREFIX, 0, xs2, m_bottomControl + staticSpinYOffset, priceNameWidth, staticHeight, staticSlBorrowDiscountId);

	m_bottomControl += spinHeight + vgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL | WS_GROUP, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, 400, comboboxSlHtbId);
	AddStaticIndirect("HTB", WS_VISIBLE | SS_NOPREFIX, 0, xs2, m_bottomControl + staticComboYOffset, m_firstColumnHalfWidth, staticHeight, staticSlHtbId);

	m_bottomControl += comboBoxHeight + vgap;
	AddButtonIndirect("On the Fly", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | WS_GROUP, 0, m_xc, m_bottomControl, m_firstColumnWidth, checkBoxHeight, checkBoxSlOnTheFlyId);

	m_bottomControl += checkBoxHeight + vgap;
	AddButtonIndirect("Borrow", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | WS_GROUP, 0, m_xc + checkBoxHeight, m_bottomControl, m_firstColumnWidth, checkBoxHeight, checkBoxBorrowId);
	
//	m_bottomControl += checkBoxHeight + vgap;
//	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL | WS_GROUP, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, 400, comboboxSlHtbId);
//	AddStaticIndirect("HTB", WS_VISIBLE | SS_NOPREFIX, 0, xs2, m_bottomControl + staticComboYOffset, m_firstColumnHalfWidth, staticHeight, staticSlHtbId);

	m_bottomControl += checkBoxHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, m_xc, m_bottomControl, moneyWidth, spinHeight, spinSlBetaId);
	AddStaticIndirect("Beta", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + moneyWidth + 1, m_bottomControl + staticSpinYOffset, moneyNameWidth, staticHeight, staticSlBetaId);
//	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinPosLoadOpenOvernightSizeId);
//	AddStaticIndirect("OpenOvnSize", WS_VISIBLE | SS_NOPREFIX, 0, xs, m_bottomControl + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticPosLoadOpenOvernightSizeId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnHalfWidth, spinHeight, spinSlMarginId);
	AddStaticIndirect("%Margin", WS_VISIBLE | SS_NOPREFIX, 0, xs2, m_bottomControl + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticSlMarginId);

	const int x = m_xc + m_firstColumnWidth + m_firstColumnWidth + hgap;
	xs = x + m_firstColumnHalfWidth + 1;

	y = vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y, m_firstColumnHalfWidth, spinHeight, spinMePmElRateId);
	AddStaticIndirect("%PM EL", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticMePmElRateId);

	y += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y, m_firstColumnHalfWidth, spinHeight, spinMePmEsRateId);
	AddStaticIndirect("%PM ES", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticMePmEsRateId);

	y += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y, m_firstColumnHalfWidth, spinHeight, spinMePmHlRateId);
	AddStaticIndirect("%PM HL", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticMePmHlRateId);

	y += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y, m_firstColumnHalfWidth, spinHeight, spinMePmHsRateId);
	AddStaticIndirect("%PM HS", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticMePmHsRateId);

	y += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y, m_firstColumnHalfWidth, spinHeight, spinMeElRateId);
	AddStaticIndirect("%EL", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticMeElRateId);

	y += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y, m_firstColumnHalfWidth, spinHeight, spinMeEsRateId);
	AddStaticIndirect("%ES", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticMeEsRateId);

	y += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y, m_firstColumnHalfWidth, spinHeight, spinMeHlRateId);
	AddStaticIndirect("%HL", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticMeHlRateId);

	y += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y, m_firstColumnHalfWidth, spinHeight, spinMeHsRateId);
	AddStaticIndirect("%HS", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticMeHsRateId);

	y += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y, m_firstColumnHalfWidth, spinHeight, spinMeFedReqId);
	AddStaticIndirect("%FedReq", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticSpinYOffset, m_firstColumnWidth, staticHeight, staticMeFedReqId);

//	m_bottomControl += checkBoxHeight + vgap;
	y += spinHeight + vgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL | WS_GROUP, 0, x, y, m_firstColumnHalfWidth, 400, comboboxMeAssetTypeId);
	AddStaticIndirect("AssetType", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticComboYOffset, m_firstColumnWidth, staticHeight, staticMeAssetTypeId);

	y += comboBoxHeight + vgap;
	AddButtonIndirect("Marginable", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | WS_GROUP, 0, x, y, m_firstColumnWidth, checkBoxHeight, checkBoxMeMarginableId);

	y += checkBoxHeight + vgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL | WS_GROUP, 0, x, y, m_firstColumnHalfWidth, 400, comboboxMeSecTypeId);
	AddStaticIndirect("SecType", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticComboYOffset, m_firstColumnWidth, staticHeight, staticMeSecTypeId);

	y += comboBoxHeight + vgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL | WS_GROUP, 0, x, y, m_firstColumnHalfWidth, 400, comboboxMeStatusId);
	AddStaticIndirect("Status", WS_VISIBLE | SS_NOPREFIX, 0, xs, y + staticComboYOffset, m_firstColumnWidth, staticHeight, staticMeStatusId);

	y += comboBoxHeight;
	if(m_bottomControl < y)m_bottomControl = y;
}

/*
void SampleDialogSettingSimulateStockLoanInfo::SetSignedSizeSpinCells(SignedNumberTypeSpin<int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000, true);
}
*/
/*
void SampleDialogSettingSimulateStockLoanInfo::SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}

void SampleDialogSettingSimulateStockLoanInfo::SetMoneySpinCells(SignedNumberTypeSpin<__int64>& spin)
{
	spin.AddSpinCell(100000000, true);
//	SetPriceSpinCells(spin);
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}
*/
void SampleDialogSettingSimulateStockLoanInfo::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateFirm::DoDataExchange(pDX);

//	DDX_Control(pDX, spinSlRequestIdId, m_SpinRequestId);

//	DDX_Control(pDX, buttonSlFillId, m_ButtonFill);
//	DDX_Control(pDX, editSlFirmId, m_EditStockLoanFirm);

	DDX_Control(pDX, spinSlBorrowPriceId, m_SpinBorrowPrice);
	DDX_Control(pDX, spinSlBorrowSizeId, m_SpinBorrowSize);
	DDX_Control(pDX, spinSlBorrowDiscountId, m_SpinBorrowDiscount);

	DDX_Control(pDX, checkBoxSlOnTheFlyId, m_CheckBoxOnTheFly);
	DDX_Control(pDX, checkBoxBorrowId, m_CheckBoxBorrow);

	DDX_Control(pDX, comboboxSlHtbId, m_ComboBoxHtb);
	DDX_Control(pDX, spinSlBetaId, m_SpinBeta);
	DDX_Control(pDX, spinSlMarginId, m_SpinMargin);

	DDX_Control(pDX, spinMePmElRateId, m_SpinMePmElRate);
	DDX_Control(pDX, spinMePmEsRateId, m_SpinMePmEsRate);
	DDX_Control(pDX, spinMePmHlRateId, m_SpinMePmHlRate);
	DDX_Control(pDX, spinMePmHsRateId, m_SpinMePmHsRate);
	DDX_Control(pDX, spinMeElRateId, m_SpinMeElRate);
	DDX_Control(pDX, spinMeEsRateId, m_SpinMeEsRate);
	DDX_Control(pDX, spinMeHlRateId, m_SpinMeHlRate);
	DDX_Control(pDX, spinMeHsRateId, m_SpinMeHsRate);
	DDX_Control(pDX, spinMeFedReqId, m_SpinMeFedReq);
	DDX_Control(pDX, comboboxMeAssetTypeId, m_ComboBoxMeAssetType);
	DDX_Control(pDX, checkBoxMeMarginableId, m_CheckBoxMeMarginable);
	DDX_Control(pDX, comboboxMeSecTypeId, m_ComboBoxMeSecType);
	DDX_Control(pDX, comboboxMeStatusId, m_ComboBoxMeStatus);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateStockLoanInfo, BaseDialogSettingSimulateFirm)
	ON_WM_CTLCOLOR()
//	ON_BN_CLICKED(buttonSlFillId, OnFill)
//	ON_EN_CHANGE(editSlFirmId, OnStockLoanFirmChange)
//	ON_EN_UPDATE(editSlFirmId, OnStockLoanFirmUpdate)
	ON_BN_CLICKED(checkBoxSlOnTheFlyId, OnOnTheFly)
	ON_BN_CLICKED(checkBoxBorrowId, OnOnTheFlyBorrow)
	ON_BN_CLICKED(checkBoxMeMarginableId, OnMarginable)

	ON_CBN_SELCHANGE(comboboxSlHtbId, OnSelchangeHtb)
	ON_CBN_SELCHANGE(comboboxMeAssetTypeId, OnSelchangeAssetType)
	ON_CBN_SELCHANGE(comboboxMeSecTypeId, OnSelchangeSecType)
	ON_CBN_SELCHANGE(comboboxMeStatusId, OnSelchangeStatus)

	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateStockLoanInfo::UpdateOnTheFly()
{
	const COLORREF colorHtb = m_onTheFly ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW);
	const bool betaValid = !m_onTheFly || !m_borrow;
	const bool borrowValid = !m_onTheFly || m_borrow;
	const COLORREF colorBeta = betaValid ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor();
	const COLORREF colorBorrow = borrowValid ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor();

//	m_CheckBoxBorrow.Invalidate(FALSE);

	m_SpinBorrowPrice.SetBkColorEnabled(colorBorrow);
	m_SpinBorrowSize.SetBkColorEnabled(colorBorrow);
	m_SpinBorrowDiscount.SetBkColorEnabled(colorBorrow);

	m_SpinBeta.SetBkColorEnabled(colorBeta);
	m_SpinMargin.SetBkColorEnabled(colorBeta);
	m_SpinMePmElRate.SetBkColorEnabled(colorBeta);
	m_SpinMePmEsRate.SetBkColorEnabled(colorBeta);
	m_SpinMePmHlRate.SetBkColorEnabled(colorBeta);
	m_SpinMePmHsRate.SetBkColorEnabled(colorBeta);
	m_SpinMeElRate.SetBkColorEnabled(colorBeta);
	m_SpinMeEsRate.SetBkColorEnabled(colorBeta);
	m_SpinMeHlRate.SetBkColorEnabled(colorBeta);
	m_SpinMeHsRate.SetBkColorEnabled(colorBeta);
	m_SpinMeFedReq.SetBkColorEnabled(colorBeta);

	m_ComboBoxMeAssetType.SetInvalid(!betaValid);
	m_CheckBoxMeMarginable.Invalidate(FALSE);
	m_ComboBoxMeSecType.SetInvalid(!betaValid);
	m_ComboBoxMeStatus.SetInvalid(!betaValid);

	m_SpinRequestId.SetBkColorEnabled(colorHtb);
	m_ComboBoxHtb.SetInvalid(m_onTheFly && !m_borrow);

//	m_modified = true;
}

void SampleDialogSettingSimulateStockLoanInfo::UpdateBorrow()
{
	if(m_onTheFly)
	{
		const bool betaValid = !m_borrow;
		const bool borrowValid = m_borrow;
		const COLORREF colorBeta = betaValid ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor();
		const COLORREF colorBorrow = borrowValid ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor();

		m_SpinBorrowPrice.SetBkColorEnabled(colorBorrow);
		m_SpinBorrowSize.SetBkColorEnabled(colorBorrow);
		m_SpinBorrowDiscount.SetBkColorEnabled(colorBorrow);

		m_SpinBeta.SetBkColorEnabled(colorBeta);
		m_SpinMargin.SetBkColorEnabled(colorBeta);
		m_SpinMePmElRate.SetBkColorEnabled(colorBeta);
		m_SpinMePmEsRate.SetBkColorEnabled(colorBeta);
		m_SpinMePmHlRate.SetBkColorEnabled(colorBeta);
		m_SpinMePmHsRate.SetBkColorEnabled(colorBeta);
		m_SpinMeElRate.SetBkColorEnabled(colorBeta);
		m_SpinMeEsRate.SetBkColorEnabled(colorBeta);
		m_SpinMeHlRate.SetBkColorEnabled(colorBeta);
		m_SpinMeHsRate.SetBkColorEnabled(colorBeta);
		m_SpinMeFedReq.SetBkColorEnabled(colorBeta);

		m_ComboBoxMeAssetType.SetInvalid(!betaValid);
		m_CheckBoxMeMarginable.Invalidate(FALSE);
		m_ComboBoxMeSecType.SetInvalid(!betaValid);
		m_ComboBoxMeStatus.SetInvalid(!betaValid);

		m_ComboBoxHtb.SetInvalid(!m_borrow);
	}
	else
	{
		m_ComboBoxHtb.SetInvalid(false);
	}
//	m_modified = true;
//	m_ComboBoxHtb.SetInvalid(m_onTheFly && !m_borrow);
}

void SampleDialogSettingSimulateStockLoanInfo::OnOnTheFly()
{
	const bool onTheFly = m_CheckBoxOnTheFly.GetCheck() == BST_CHECKED;
	if(onTheFly != m_onTheFly)
	{
		m_onTheFly = onTheFly;
		UpdateOnTheFly();
		m_modified = true;
	}
}

void SampleDialogSettingSimulateStockLoanInfo::OnOnTheFlyBorrow()
{
	const bool borrow = m_CheckBoxBorrow.GetCheck() == BST_CHECKED;
	if(borrow != m_borrow)
	{
		m_borrow = borrow;
		UpdateBorrow();
		m_modified = true;
	}
}

void SampleDialogSettingSimulateStockLoanInfo::OnMarginable()
{
	const bool marginable = m_CheckBoxMeMarginable.GetCheck() == BST_CHECKED;
	if(marginable != m_marginable)
	{
		m_marginable = marginable;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateStockLoanInfo::OnSelchangeHtb()
{
	const unsigned char htb = m_ComboBoxHtb.GetSelectedChar();
	if(htb != m_htb)
	{
		m_htb = htb;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateStockLoanInfo::OnSelchangeAssetType()
{
	const unsigned char assetType = m_ComboBoxMeAssetType.GetSelectedChar();
	if(assetType != m_assetType)
	{
		m_assetType = assetType;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateStockLoanInfo::OnSelchangeSecType()
{
	const unsigned char secType = m_ComboBoxMeSecType.GetSelectedChar();
	if(secType != m_secType)
	{
		m_secType = secType;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateStockLoanInfo::OnSelchangeStatus()
{
	const unsigned char status = m_ComboBoxMeStatus.GetSelectedChar();
	if(status != m_status)
	{
		m_status = status;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateStockLoanInfo::FillFirmInfo()
{
//	m_EditStockLoanFirm.SetWindowText(m_account->GetStockLoanFirmId());
	BaseDialogSettingSimulateFirm::FillFirmInfo();
	if(!m_symbol.empty())
	{
		const Security* security = TD_FindSortableEquity(m_symbol.c_str());
		if(security)
		{
			Price borPrice;
			Money betaMoney;
			unsigned int borSize;
			unsigned char discountFlag = 1;
			unsigned short margin;
			unsigned char htb;

			unsigned short pm_el_rate;
			unsigned short pm_es_rate;
			unsigned short pm_hl_rate;
			unsigned short pm_hs_rate;
			unsigned short el_rate;
			unsigned short es_rate;
			unsigned short hl_rate;
			unsigned short hs_rate;
			unsigned char fed_req;
			unsigned char assetType;
			bool marginable;
			unsigned char secType;
			unsigned char status;

			const unsigned int ordinal = m_account->GetStockLoanFirmOrdinal();
			const bool portfolioManaged = m_account->isPortfolioManaged();

			security->LockInquiryWaitLevel1();

			const unsigned int* borrowPrice = security->GetCompactBorrowPrice(ordinal);
			if(borrowPrice)borPrice = Price(Price(*borrowPrice) / 100);

//				const int* beta = security->GetCompactBeta(ordinal);
//				if(beta)betaMoney = Money(SignedPrice(*beta));

			borSize = security->ObtainBorrowSize(ordinal);
#ifdef BORROW_PRICE_DISCOUNT
			discountFlag = security->GetBorrowDiscountFlag(ordinal);
#endif
			margin = security->ObtainMargin(ordinal, portfolioManaged);
			htb = security->isHTB(ordinal);

			pm_el_rate = security->ObtainPmElRate(ordinal);
			pm_es_rate = security->ObtainPmEsRate(ordinal);
			pm_hl_rate = security->ObtainPmHlRate(ordinal);
			pm_hs_rate = security->ObtainPmHsRate(ordinal);
			el_rate = security->ObtainElRate(ordinal);
			es_rate = security->ObtainEsRate(ordinal);
			hl_rate = security->ObtainHlRate(ordinal);
			hs_rate = security->ObtainHsRate(ordinal);
			fed_req = 50;//security->ObtainFedReq(ordinal);
			assetType = 'E';//security->ObtainAssetType(ordinal);
			marginable = true;//security->ObtainMarginable(ordinal);
			secType = 'C';//security->ObtainSecType(ordinal);
			status = 0;//security->ObtainStatus(ordinal);

			security->UnlockInquiryLevel1();

			TD_ReleaseStock(security);

			FillSpinControlFromPrice(borPrice, m_SpinBorrowPrice, 1000000, false);
			FillSpinControlFromMoney(betaMoney, m_SpinBeta, 10000, false);
			FillSpinControlFromUInt(borSize, m_SpinBorrowSize, false);
			FillSpinControlFromUChar(discountFlag, m_SpinBorrowDiscount, false);
			FillSpinControlFromUChar((unsigned char)margin, m_SpinMargin, false);
			m_ComboBoxHtb.SelectChar(htb);

			FillSpinControlFromUChar((unsigned char)pm_el_rate, m_SpinMePmElRate, false);
			FillSpinControlFromUChar((unsigned char)pm_es_rate, m_SpinMePmEsRate, false);
			FillSpinControlFromUChar((unsigned char)pm_hl_rate, m_SpinMePmHlRate, false);
			FillSpinControlFromUChar((unsigned char)pm_hs_rate, m_SpinMePmHsRate, false);
			FillSpinControlFromUChar((unsigned char)el_rate, m_SpinMeElRate, false);
			FillSpinControlFromUChar((unsigned char)es_rate, m_SpinMeEsRate, false);
			FillSpinControlFromUChar((unsigned char)hl_rate, m_SpinMeHlRate, false);
			FillSpinControlFromUChar((unsigned char)hs_rate, m_SpinMeHsRate, false);
			FillSpinControlFromUChar(fed_req, m_SpinMeFedReq, false);
			m_ComboBoxMeAssetType.SelectChar(assetType);
			m_CheckBoxMeMarginable.SetCheck(marginable ? BST_CHECKED : BST_UNCHECKED);
			m_ComboBoxMeSecType.SelectChar(secType);
			m_ComboBoxMeStatus.SelectChar(status);

			OnSelchangeHtb();
			OnSelchangeAssetType();
			OnSelchangeSecType();
			OnSelchangeStatus();

		}
	}
}

LRESULT SampleDialogSettingSimulateStockLoanInfo::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
//		case spinSlRequestIdId:
		case spinSlBorrowPriceId:
		case spinSlBorrowSizeId:
		case spinSlBorrowDiscountId:
		case spinSlBetaId:
		case spinSlMarginId:

		case spinMePmElRateId:
		case spinMePmEsRateId:
		case spinMePmHlRateId:
		case spinMePmHsRateId:
		case spinMeElRateId:
		case spinMeEsRateId:
		case spinMeHlRateId:
		case spinMeHsRateId:
		case spinMeFedReqId:
		m_modified = true;
		break;

		default:
		BaseDialogSettingSimulateFirm::OnSpinChange(w, l);
		break;
	}
	return 0;
}

bool SampleDialogSettingSimulateStockLoanInfo::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateFirm::isSendMessageButtonEnabled() && m_ComboBoxHtb.GetCurSel() >= 0;
}

const Message* SampleDialogSettingSimulateStockLoanInfo::CreateMessage(WSABUF& wsaBuf)
{
	Price price;
	Money money;
	FillPriceFromSpinControl(price, m_SpinBorrowPrice, 1000000);

	if(m_onTheFly)
	{
		wsaBuf.buf = nullptr;
		wsaBuf.len = 0;
		if(m_borrow)
		{
			U_CopyAndPad(m_msgOnTheFlyBorrow.m_symbol, sizeof(m_msgOnTheFlyBorrow.m_symbol), m_symbol.c_str(), '\0');
			m_msgOnTheFlyBorrow.m_time = TL_GetCurrentMillisecond();

			m_msgOnTheFlyBorrow.m_firmId = U_RepresentStringAsUnsignedNumber<unsigned int>(m_stockLoanFirm.c_str());
			m_msgOnTheFlyBorrow.m_size = m_SpinBorrowSize.GetValue();
			m_msgOnTheFlyBorrow.m_discountFlag = m_SpinBorrowDiscount.GetValue();
			m_msgOnTheFlyBorrow.m_priceDollar = price.GetDollars();
			m_msgOnTheFlyBorrow.m_priceFraction = price.GetDollarFraction();
			return &m_msgOnTheFlyBorrow;
		}
		else
		{
			U_CopyAndPad(m_msgOnTheFlyBetaAndMargin.m_symbol, sizeof(m_msgOnTheFlyBetaAndMargin.m_symbol), m_symbol.c_str(), '\0');
			m_msgOnTheFlyBetaAndMargin.m_firmId = U_RepresentStringAsUnsignedNumber<unsigned int>(m_stockLoanFirm.c_str());
			m_msgOnTheFlyBetaAndMargin.m_time = TL_GetCurrentMillisecond();

			m_msgOnTheFlyBetaAndMargin.m_margin = m_SpinMargin.GetValue();
			FillMoneyFromSpinControl(money, m_SpinBeta, 10000);
			m_msgOnTheFlyBetaAndMargin.m_beta = money.toDouble();

			m_msgOnTheFlyBetaAndMargin.m_PM_EL_Rate = m_SpinMePmElRate.GetValue();
			m_msgOnTheFlyBetaAndMargin.m_PM_ES_Rate = m_SpinMePmEsRate.GetValue();
			m_msgOnTheFlyBetaAndMargin.m_PM_HL_Rate = m_SpinMePmHlRate.GetValue();
			m_msgOnTheFlyBetaAndMargin.m_PM_HS_Rate = m_SpinMePmHsRate.GetValue();

			m_msgOnTheFlyBetaAndMargin.m_EL_Rate = m_SpinMeElRate.GetValue();
			m_msgOnTheFlyBetaAndMargin.m_ES_Rate = m_SpinMeEsRate.GetValue();
			m_msgOnTheFlyBetaAndMargin.m_HL_Rate = m_SpinMeHlRate.GetValue();
			m_msgOnTheFlyBetaAndMargin.m_HS_Rate = m_SpinMeHsRate.GetValue();

			m_msgOnTheFlyBetaAndMargin.m_Fed_Req = m_SpinMeFedReq.GetValue();

			m_msgOnTheFlyBetaAndMargin.m_Asset_Type = m_assetType;//m_ComboBoxMeAssetType.GetSelectedChar();
			m_msgOnTheFlyBetaAndMargin.m_Sec_Type = m_secType;//m_ComboBoxMeSecType.GetSelectedChar();
			m_msgOnTheFlyBetaAndMargin.m_Status = m_status;//m_ComboBoxMeStatus.GetSelectedChar();
			m_msgOnTheFlyBetaAndMargin.m_Marginable = m_marginable;// m_CheckBoxMeMarginable.GetCheck() == BST_CHECKED;

			return &m_msgOnTheFlyBetaAndMargin;
		}
	}
	else
	{
		if(m_borrow)
		{
			U_CopyAndPad(m_htbNode.m_symbol, sizeof(m_htbNode.m_symbol), m_symbol.c_str(), '\0');
			m_msgHtb.m_requestId = m_SpinRequestId.GetValue();
			m_msgHtb.m_time = TL_GetCurrentMillisecond();
			m_msgHtb.m_firmId = U_RepresentStringAsUnsignedNumber<unsigned int>(m_stockLoanFirm.c_str());

			m_htbNode.m_quantity = m_SpinBorrowSize.GetValue();
			m_htbNode.m_discountFlag = m_SpinBorrowDiscount.GetValue();
			FillPriceFromSpinControl(price, m_SpinBorrowPrice, 1000000);
			m_htbNode.m_priceToBorrowDollar = price.GetDollars();
			m_htbNode.m_priceToBorrowFraction = price.GetDollarFraction();
			m_htbNode.m_htbType = m_htb;// m_ComboBoxHtb.GetSelectedChar();

			wsaBuf.buf = (char*)&m_htbNode;
			wsaBuf.len = sizeof(m_htbNode);
			return &m_msgHtb;
		}
		else
		{
			U_CopyAndPad(m_borrowNode.m_symbol, sizeof(m_borrowNode.m_symbol), m_symbol.c_str(), '\0');
			m_msg.m_requestId = m_SpinRequestId.GetValue();
			m_msg.m_time = TL_GetCurrentMillisecond();

			m_msg.m_firmId = U_RepresentStringAsUnsignedNumber<unsigned int>(m_stockLoanFirm.c_str());

			m_borrowNode.m_quantity = m_SpinBorrowSize.GetValue();
			m_borrowNode.m_discountFlag = m_SpinBorrowDiscount.GetValue();
			FillPriceFromSpinControl(price, m_SpinBorrowPrice, 1000000);
			m_borrowNode.m_priceToBorrowDollar = price.GetDollars();
			m_borrowNode.m_priceToBorrowFraction = price.GetDollarFraction();
			m_borrowNode.m_htbType = m_htb;// m_ComboBoxHtb.GetSelectedChar();

			m_borrowNode.m_margin = m_SpinMargin.GetValue();

			FillMoneyFromSpinControl(money, m_SpinBeta, 10000);
			m_borrowNode.m_beta = money.toDouble();

			m_borrowNode.m_PM_EL_Rate = m_SpinMePmElRate.GetValue();
			m_borrowNode.m_PM_ES_Rate = m_SpinMePmEsRate.GetValue();
			m_borrowNode.m_PM_HL_Rate = m_SpinMePmHlRate.GetValue();
			m_borrowNode.m_PM_HS_Rate = m_SpinMePmHsRate.GetValue();

			m_borrowNode.m_EL_Rate = m_SpinMeElRate.GetValue();
			m_borrowNode.m_ES_Rate = m_SpinMeEsRate.GetValue();
			m_borrowNode.m_HL_Rate = m_SpinMeHlRate.GetValue();
			m_borrowNode.m_HS_Rate = m_SpinMeHsRate.GetValue();

			m_borrowNode.m_Fed_Req = m_SpinMeFedReq.GetValue();

			m_borrowNode.m_Asset_Type = m_assetType;// m_ComboBoxMeAssetType.GetSelectedChar();
			m_borrowNode.m_Sec_Type = m_secType;// m_ComboBoxMeSecType.GetSelectedChar();
			m_borrowNode.m_Status = m_status;// m_ComboBoxMeStatus.GetSelectedChar();
			m_borrowNode.m_Marginable = m_marginable;// m_CheckBoxMeMarginable.GetCheck() == BST_CHECKED;

//			const unsigned short length = (unsigned short)sizeof(m_borrowNode) + 1;
//			m_msg.m_length = sizeof(m_msg) + sizeof(m_borrowNode);
			wsaBuf.buf = (char*)&m_borrowNode;
			wsaBuf.len = sizeof(m_borrowNode);
			return &m_msg;
		}
	}
}

BOOL SampleDialogSettingSimulateStockLoanInfo::OnInitDialog()
{
//	m_stockLoanFirm.clear();
	m_onTheFly = false;
	m_borrow = false;
	m_marginable = false;
	m_assetType = false;
	m_secType = false;
	m_status = false;
/*Done in UpdateOnTheFly();
	m_SpinBeta.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));//m_visualBase->GetGrayColor());
	m_SpinMargin.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));//m_visualBase->GetGrayColor());
	m_SpinRequestId.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
	m_ComboBoxHtb.SetInvalid(false);
*/
	BOOL ret = BaseDialogSettingSimulateFirm::OnInitDialog();

	UpdateOnTheFly();

//	m_EditStockLoanFirm.SetLimitText(4);
	m_ComboBoxHtb.FillAlphabet();
	m_ComboBoxHtb.AddString(nullptr);
	m_ComboBoxMeAssetType.FillAlphabet();
	m_ComboBoxMeAssetType.AddString(nullptr);
	m_ComboBoxMeSecType.FillAlphabet();
	m_ComboBoxMeSecType.AddString(nullptr);
	m_ComboBoxMeStatus.FillAlphabet();
	m_ComboBoxMeStatus.AddString(nullptr);
//	RestoreLayout();

	return ret;
}

StringSetting* SampleDialogSettingSimulateStockLoanInfo::GetSettingFirm(){return &extensionConfig->m_simSlFirm;}
UIntSetting* SampleDialogSettingSimulateStockLoanInfo::GetSettingRequestId(){return &extensionConfig->m_simSlRequestId;}

void SampleDialogSettingSimulateStockLoanInfo::UpdateSettings()
{
	extensionConfig->m_simSlSymbol.SetValue(m_symbol);
	UpdateAccountInConfig(extensionConfig->m_simSlAccountId);

	BaseDialogSettingSimulateFirm::UpdateSettings();
//	extensionConfig->m_simSlFirm.SetValue(m_stockLoanFirm);

	Price price;
	FillPriceFromSpinControl(price, m_SpinBorrowPrice, 1000000);
	extensionConfig->m_simSlBorrowPrice.SetValue(price);

	Money money;
	FillMoneyFromSpinControl(money, m_SpinBeta, 10000);
	extensionConfig->m_simSlBeta.SetValue(money);

	extensionConfig->m_simSlBorrowSize.SetValue(m_SpinBorrowSize.GetValue());

	extensionConfig->m_simSlDiscountFlag.SetValue(m_SpinBorrowDiscount.GetValue());

	extensionConfig->m_simSlOnTheFly.SetValue(m_onTheFly);
	extensionConfig->m_simSlOnTheFlyBorrow.SetValue(m_borrow);
	extensionConfig->m_simSlMarginable.SetValue(m_marginable);

	extensionConfig->m_simSlHtb.SetValue(m_htb);
	extensionConfig->m_simSlAssetType.SetValue(m_assetType);
	extensionConfig->m_simSlSecType.SetValue(m_secType);
	extensionConfig->m_simSlStatus.SetValue(m_status);

	extensionConfig->m_simSlMargin.SetValue(m_SpinMargin.GetValue());

	extensionConfig->m_simSlPmElRate.SetValue(m_SpinMeElRate.GetValue());
	extensionConfig->m_simSlPmEsRate.SetValue(m_SpinMeEsRate.GetValue());
	extensionConfig->m_simSlPmHlRate.SetValue(m_SpinMeHlRate.GetValue());
	extensionConfig->m_simSlPmHsRate.SetValue(m_SpinMeHsRate.GetValue());
	extensionConfig->m_simSlElRate.SetValue(m_SpinMeElRate.GetValue());
	extensionConfig->m_simSlEsRate.SetValue(m_SpinMeEsRate.GetValue());
	extensionConfig->m_simSlHlRate.SetValue(m_SpinMeHlRate.GetValue());
	extensionConfig->m_simSlHsRate.SetValue(m_SpinMeHsRate.GetValue());
	extensionConfig->m_simSlFedReq.SetValue(m_SpinMeFedReq.GetValue());
}

void SampleDialogSettingSimulateStockLoanInfo::RestoreLayout()
{
	m_EditSymbol.SetWindowText(extensionConfig->m_simSlSymbol.GetValue().c_str());

	SetAccountById(extensionConfig->m_simSlAccountId.GetValue().c_str());

	BaseDialogSettingSimulateFirm::RestoreLayout();
//	m_EditStockLoanFirm.SetWindowText(extensionConfig->m_simSlFirm.GetValue().c_str());

	FillSpinControlFromPrice(extensionConfig->m_simSlBorrowPrice.GetValue(), m_SpinBorrowPrice, 1000000, false);

	FillSpinControlFromMoney(extensionConfig->m_simSlBeta.GetValue(), m_SpinBeta, 10000, false);

	FillSpinControlFromUInt(extensionConfig->m_simSlBorrowSize.GetValue(), m_SpinBorrowSize, false);
	FillSpinControlFromUChar(extensionConfig->m_simSlDiscountFlag.GetValue(), m_SpinBorrowDiscount, false);

	const bool onTheFly = extensionConfig->m_simSlOnTheFly.GetValue();
	if(onTheFly != m_onTheFly)
	{
		m_onTheFly = onTheFly;
		m_CheckBoxOnTheFly.SetCheck(m_onTheFly ? BST_CHECKED : BST_UNCHECKED);
		UpdateOnTheFly();
	}

	const bool borrow = extensionConfig->m_simSlOnTheFlyBorrow.GetValue();
	if(borrow != m_borrow)
	{
		m_borrow = borrow;
		m_CheckBoxBorrow.SetCheck(m_borrow ? BST_CHECKED : BST_UNCHECKED);
		UpdateBorrow();
	}

	const bool marginable = extensionConfig->m_simSlMarginable.GetValue();
	if(marginable != m_marginable)
	{
		m_marginable = marginable;
		m_CheckBoxMeMarginable.SetCheck(m_marginable ? BST_CHECKED : BST_UNCHECKED);
	}

	FillSpinControlFromUChar(extensionConfig->m_simSlMargin.GetValue(), m_SpinMargin, false);

	FillSpinControlFromUChar(extensionConfig->m_simSlPmElRate.GetValue(), m_SpinMePmElRate, false);
	FillSpinControlFromUChar(extensionConfig->m_simSlPmEsRate.GetValue(), m_SpinMePmEsRate, false);
	FillSpinControlFromUChar(extensionConfig->m_simSlPmHlRate.GetValue(), m_SpinMePmHlRate, false);
	FillSpinControlFromUChar(extensionConfig->m_simSlPmHsRate.GetValue(), m_SpinMePmHsRate, false);
	FillSpinControlFromUChar(extensionConfig->m_simSlElRate.GetValue(), m_SpinMeElRate, false);
	FillSpinControlFromUChar(extensionConfig->m_simSlEsRate.GetValue(), m_SpinMeEsRate, false);
	FillSpinControlFromUChar(extensionConfig->m_simSlHlRate.GetValue(), m_SpinMeHlRate, false);
	FillSpinControlFromUChar(extensionConfig->m_simSlHsRate.GetValue(), m_SpinMeHsRate, false);
	FillSpinControlFromUChar(extensionConfig->m_simSlFedReq.GetValue(), m_SpinMeFedReq, false);

	const unsigned char htb = extensionConfig->m_simSlHtb.GetValue();
	if(htb != m_htb)
	{
		m_htb = htb;
		m_ComboBoxHtb.SelectChar(htb);
	}
	const unsigned char assetType = extensionConfig->m_simSlAssetType.GetValue();
	if(assetType != m_assetType)
	{
		m_assetType = assetType;
		m_ComboBoxMeAssetType.SelectChar(assetType);
	}
	const unsigned char secType = extensionConfig->m_simSlSecType.GetValue();
	if(secType != m_secType)
	{
		m_secType = secType;
		m_ComboBoxMeSecType.SelectChar(secType);
	}
	const unsigned char status = extensionConfig->m_simSlSecType.GetValue();
	if(status != m_status)
	{
		m_status = status;
		m_ComboBoxMeStatus.SelectChar(status);
	}

	EnableSendMessageButton();

	m_modified = false;
}

HBRUSH SampleDialogSettingSimulateStockLoanInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
/*
		case checkBoxBorrowId:
		if(!m_onTheFly)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;
*/
		case checkBoxMeMarginableId:
		if(m_onTheFly && m_borrow)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;
	}
	return BaseDialogSettingSimulateFirm::OnCtlColor(pDC, pWnd, nCtlColor);
}

