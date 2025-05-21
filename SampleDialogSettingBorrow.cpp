#include "stdafx.h"
#include "SampleDialogSettingBorrow.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingBorrow::SampleDialogSettingBorrow(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseSampleDialogSettingSendOrder(mainWnd, parentTab, "Borrow"),
	m_SpinPrice(999999, 0, 0, 1000, 3, TakionSpin::charComma, 4),
	m_priceType(0xFF),
	m_SpinSizeShare(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_ComboBoxTifType(false)
{
	m_ComboBoxTifType.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_SpinPrice.AddSpinCell(1, false);
	m_SpinPrice.AddSpinCell(100, false);
	m_SpinPrice.AddSpinCell(10000, true);

	m_SpinSizeShare.AddSpinCell(1, false);
	m_SpinSizeShare.AddSpinCell(100, false);
	m_SpinSizeShare.AddSpinCell(1000, true);

	int x = m_accountWidth + hgap;//0;
	int y = 0;//m_bottomControl
	int width = m_accountWidth;// + 2;//52;
	int xc = x + hgap;

	m_bottomControl += vgap;

	int yPrice = y;//m_bottomControl;
	AddButtonIndirect("Price", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap + comboBoxHeight, 40, groupBorrowPriceId);

	AddButtonIndirect("Market Order", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += groupOffset, width, checkBoxHeight, radioBorrowPriceMarketId);
	AddButtonIndirect("Limit Order in cents", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap + vgap, width + comboBoxHeight, checkBoxHeight, radioBorrowPriceAbsoluteId);

	int xOffset = xc + comboBoxHeight;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xOffset, y += checkBoxHeight + vgap, width, spinHeight, spinBorrowPriceId);

//	width += hgap + hgap;
	const int cWidth = width - hgap - hgap;
	int ySize = yPrice + 42;
//	m_bottomControl = ySize;
	AddButtonIndirect("Size", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, ySize, width, 24, groupBorrowSizeId);
//	AddButtonIndirect("Position", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += groupOffset, width, checkBoxHeight, radioSizePositionId);
//	AddButtonIndirect("Share", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap + vgap, width, checkBoxHeight, radioSizeShareId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, ySize + groupOffset, cWidth, spinHeight, spinBorrowSizeShareId);

	int yTif = ySize + 26;
//	m_bottomControl = yTif;
	AddButtonIndirect("TIF", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, yTif, width, 24, groupBorrowTifId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, yTif + groupOffset, cWidth, 100, comboBoxBorrowTifId);

//	m_bottomControl += 24;
}

unsigned char SampleDialogSettingBorrow::SendOrder(Security* stock, const BaseOrderParams& orderParams)
{
	unsigned char ret = OE_OK;
	BorrowOrderParams& borrowOrderParams = (BorrowOrderParams&)orderParams;
	Account* account = borrowOrderParams.GetAccount();
	Price level1Bid(stock->GetL1Bid());
	Price level1Ask(stock->GetL1Ask());
	Price lastPrint(stock->GetLastNbboPrintPrice());
	unsigned char orderType;
	Price limitPrice;
//	SignedPrice signedPrice;
	switch(borrowOrderParams.GetPriceType())
	{
		case BPT_MARKET:
		orderType = ORDER_TYPE_MARKET;
		break;

		default://case BPT_ABSOLUTE:
		orderType = ORDER_TYPE_LIMIT;
		limitPrice = borrowOrderParams.GetPriceAbsolute();

		if(limitPrice.isZero())
		{
			return OE_PRICE_ZERO;
		}

		break;
	}

	unsigned int orderSize = borrowOrderParams.GetSizeShares();
	if(!orderSize)
	{
		return OE_SIZE_ZERO;
	}
	unsigned char tifType = borrowOrderParams.GetTifType();

	ret = account->SendOrder(nullptr,//&order,
		nullptr,
		true,
		0,
		stock,
		PE_NOT_POSTPONED,
		0,//timeToActivate
		qstkNum,
		LSTK_ROUTE,
		'B',
		0,//unsigned char repriceFlags
		orderType,
		borrowOrderParams.GetUserType(),//userOrderType
		nullptr,//userOrderInfo
		nullptr,//commandName
		limitPrice,

		Price::priceZero,//TU_GetPriceZero(),//Price(discretionaryPrice),
		Price::priceZero,//TU_GetSignedPriceZero(),//Price(stopPrice),
		OST_NONE,
		OSB_PRINT,

		level1Bid,
		level1Ask,
		lastPrint,

		orderSize,
		0,//minQuantity
		orderSize,//displayQuantity,
		0,//reserveQuantity,

		0,//routingId
		0,//routingSubType
		0,//routingName

		account->GetStockLoanFirmIdNum(),//mnemonic

		false,//iso,

		PEG_NONE,//m_pegType,
		SignedPrice::signedPriceZero,//TU_GetSignedPriceZero(),//m_pegOffset,

		tifType,
		0,//tif milliseconds
		WOP_KEEP,//TD_GetWashOrderPolicy();
		OOP_RESIZE,//TD_GetOversellSplitPolicy()
		true,//TD_IsResizeShortToBorrowed(),
		false,//preBorrow
		Price::priceZero,//TU_GetPriceZero(),//preBorrowPrice
		false,//noRedirect
		false,//closeOnly
		false,//resizeToClose
		false,//allOrNone
		false,//syntheticIgnoreFinra,
		false,//syntheticWaitNext,
		false,//imbalanceOnly
//		true,//proAts
		false,//blockAggressiveOrders
		RLP_ROUND_DOWN,//TD_GetRoundLotPolicy(),
		nullptr,//const std::string* const& isoAttachment
		borrowOrderParams.GetUserId(),
		0,//parentId
		0,//ocoId
		0,//renewMillisecond
		nullptr,//userData
		0,//const unsigned int& maxPositionSize
		borrowOrderParams.GetExtensionId(),
		false);//bool openOnly
	return ret;
}

BaseOrderParams* SampleDialogSettingBorrow::CreateOrderParams() const
{
	return new BorrowOrderParams;
}

unsigned char SampleDialogSettingBorrow::GetTifType() const
{
	int sel = m_ComboBoxTifType.GetCurSel();
	if(sel >= 0)
	{
		if(sel == 0)
		{
			return TIF_DAY;
		}
		else
		{
			return TIF_FOK;
		}
	}
	return 0xFF;
}

unsigned char SampleDialogSettingBorrow::FillOrderParams(BaseOrderParams& orderParams) const
{
	unsigned char ret = BaseSampleDialogSettingSendOrder::FillOrderParams(orderParams);

	if(ret == SPE_OK)
	{
		BorrowOrderParams& borrowOrderParams = (BorrowOrderParams&)orderParams;

		if(m_priceType >= BPT_Count)return SPE_NO_PRICE_TYPE;
		borrowOrderParams.SetPriceType(m_priceType);

		Price price;
		FillPriceFromSpinControl(price, m_SpinPrice, 10000);
		borrowOrderParams.SetPriceAbsolute(price);

		borrowOrderParams.SetSizeShares(((UnsignedNumberTypeSpin<unsigned int>&)m_SpinSizeShare).GetValue());

		unsigned char tifType = GetTifType();
		if(tifType == 0xFF)return SPE_NO_TIF_TYPE;
		borrowOrderParams.SetTifType(tifType);
	}

	return ret;
}

BOOL SampleDialogSettingBorrow::OnInitDialog()
{
	BOOL ret = BaseSampleDialogSettingSendOrder::OnInitDialog();

//	HideOptionControls(true);

	m_RadioPriceMarket.SetCheck(BST_CHECKED);
	m_priceType = BPT_MARKET;
	UpdatePriceType();

	m_ComboBoxTifType.AddString("Day");
	m_ComboBoxTifType.AddString("FillOrKill");
	m_ComboBoxTifType.SetCurSel(0);

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingBorrow::RestoreLayout()
{
	BaseSampleDialogSettingSendOrder::UpdateSettings();
//	SetAccountId(extensionConfig->m_borrowAccountId.GetValue().c_str());

	const std::string& symbol = extensionConfig->m_borrowSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_borrowOption.GetValue(),
		extensionConfig->m_borrowOptionSuffix.GetValue(),
		extensionConfig->m_borrowOptionCall.GetValue(),
		extensionConfig->m_borrowOptionStrikePrice.GetValue(),
		extensionConfig->m_borrowOptionExpiration.GetValue());
#endif

//	EnableApplyButton(false);
//	EnableSendMessageButton();

	m_modified = false;
}

StringSetting* SampleDialogSettingBorrow::GetAccountIdSetting()
{
	return &extensionConfig->m_borrowAccountId;
}

void SampleDialogSettingBorrow::UpdateSettings()
{
	BaseSampleDialogSettingSendOrder::UpdateSettings();
/*
	int sel = m_ComboBoxAccount.GetCurSel();
	if(sel > 0 && m_account)
	{
		extensionConfig->m_borrowAccountId.SetValue(m_account->GetId());
	}
	else
	{
		extensionConfig->m_borrowAccountId.SetValue(std::string());
	}
*/
	extensionConfig->m_borrowSymbol.SetValue(m_symbol);
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_borrowOption.SetValue(m_optionMode);
	extensionConfig->m_borrowOptionCall.SetValue(m_optionCall);
	extensionConfig->m_borrowOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_borrowOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_borrowOptionExpiration.SetValue(GetExpirationDate());
#endif
}

void SampleDialogSettingBorrow::DoDataExchange(CDataExchange* pDX)
{
	BaseSampleDialogSettingSendOrder::DoDataExchange(pDX);

	DDX_Control(pDX, radioBorrowPriceMarketId, m_RadioPriceMarket);
	DDX_Control(pDX, radioBorrowPriceAbsoluteId, m_RadioPriceAbsolute);

	DDX_Control(pDX, spinBorrowPriceId, m_SpinPrice);

	DDX_Control(pDX, spinBorrowSizeShareId, m_SpinSizeShare);

	DDX_Control(pDX, comboBoxBorrowTifId, m_ComboBoxTifType);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingBorrow, BaseSampleDialogSettingSendOrder)
//	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(radioBorrowPriceMarketId, OnPriceType)
	ON_BN_CLICKED(radioBorrowPriceAbsoluteId, OnPriceType)
//	ON_CBN_SELCHANGE(comboBoxTifId, OnSelchangeTif)
END_MESSAGE_MAP()

void SampleDialogSettingBorrow::UpdatePriceType()
{
	switch(m_priceType)
	{
		case BPT_ABSOLUTE:
		m_SpinPrice.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
		break;

		default:
		m_SpinPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
		break;
	}
}

void SampleDialogSettingBorrow::OnPriceType()
{
	unsigned char priceType = m_RadioPriceMarket.GetCheck() == BST_CHECKED ? BPT_MARKET:
		m_RadioPriceAbsolute.GetCheck() == BST_CHECKED ? BPT_ABSOLUTE:
		0xFF;
	if(m_priceType != priceType)
	{
		m_priceType = priceType;
		UpdatePriceType();
	}
}
/*
HBRUSH SampleDialogSettingBorrow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return BaseSampleDialogSettingSendOrder::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/