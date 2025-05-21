#include "stdafx.h"
#include "SampleDialogSettingVenue.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum SampleSettingVenueIds
{
	groupStopId = baseOrderIdsCount,
	comboBoxStopTypeId,
	spinStopPriceOffsetId,
	comboBoxStopBaseId,

	groupDiscretionaryId,
	spinDiscretionaryOffsetId,

	venueIdsCount
};

SampleDialogSettingVenue::SampleDialogSettingVenue(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab) :
	BaseSampleDialogSettingOrder(mainWnd, parentTab, "Venue"),
	//	m_ComboBoxPegType(false),
	m_ComboBoxStopType(false),
	m_ComboBoxStopBase(false),
	//	m_SpinPegOffset(999999, -999999, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinDiscretionaryOffset(999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinStopOffset(999999, -999999, 0, 10000, 3, TakionSpin::charComma, 4),
//	m_SpinMinQuantity(mainWnd->GetOrderSizeAbsoluteCap() / 100, 0, 0, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Minimum Quantity to fill in Round Lots"),
//	m_CheckBoxAllOrNone("Fill all Shares or none", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_stopType(0),
	m_stopBase(0)
{
	m_SpinDiscretionaryOffset.AddSpinCell(1, false);
	m_SpinDiscretionaryOffset.AddSpinCell(100, false);
	m_SpinDiscretionaryOffset.AddSpinCell(10000, true);

	m_SpinStopOffset.AddSpinCell(1, false);
	m_SpinStopOffset.AddSpinCell(100, false);
	m_SpinStopOffset.AddSpinCell(10000, true);

	int width = 47;//47 + comboBoxHeight;
	int x = m_horizontalOffset;//m_tifHorizontalOffset;
	int xc = x + hgap;
	int y = 0;//m_displaySizeVerticalOffset;
	const int vgap1 = 2;
	AddButtonIndirect("Stop Price", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap, 50, groupStopId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y += groupOffset, width, 100, comboBoxStopTypeId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += comboBoxHeight + vgap, width, spinHeight, spinStopPriceOffsetId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y += spinHeight + vgap1, width, 100, comboBoxStopBaseId);
/*
	x = m_horizontalOffset;
	xc = x + hgap;
	y = 0;
	int yAbsolute = y;
	AddButtonIndirect("Peg", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap, 40, groupPegId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y += groupOffset, width, 100, comboBoxPegTypeId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += comboBoxHeight + vgap, width, spinHeight, spinPegPriceOffsetId);
*/
	y = 50;//m_displaySizeVerticalOffset + 50;//yAbsolute + 40;
	const int groupHeight = 24;
	AddButtonIndirect("Discretionary", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap, groupHeight, groupDiscretionaryId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y + groupOffset, width, spinHeight, spinDiscretionaryOffsetId);
/*
	xc += width + 4 * hgap;
	y = groupOffset;// += groupHeight + vgap + vgap;
	AddButtonIndirect("AON", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, xc, y, width, checkBoxHeight, checkboxAllOrNoneId);
	AddStaticIndirect("Min Qty", WS_VISIBLE|SS_NOPREFIX|SS_LEFTNOWORDWRAP, 0, xc, y += checkBoxHeight + vgap + vgap, width, staticHeight, staticMinQuantityId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, xc, y += staticHeight, width, spinHeight, spinMinQuantityId);
*/
}

StringSetting* SampleDialogSettingVenue::GetAccountIdSetting()
{
	return &extensionConfig->m_venueAccountId;
}

void SampleDialogSettingVenue::UpdateSettings()
{
	BaseSampleDialogSettingOrder::UpdateSettings();

	extensionConfig->m_venueSymbol.SetValue(m_symbol);
	extensionConfig->m_venueVenue.SetValue(m_currentDestination ? m_currentDestination->GetMainNumericName() : 0);

	const int sel = m_ComboBoxRouting.GetCurSel();
	const DestinationRouting* routing = sel < 0 ? nullptr : (const DestinationRouting*)m_ComboBoxRouting.GetItemDataPtr(sel);
	extensionConfig->m_venueRouting.SetValue(routing ? routing->GetNumericName() : 0);

	CString mnemonic;
	m_EditMnemonic.GetWindowText(mnemonic);
	extensionConfig->m_venueMnemonic.SetValue(U_RepresentStringAsUnsignedNumber<unsigned int>(mnemonic));//U_StringToUInt(mnemonic));

	extensionConfig->m_venueReserveSize.SetValue(m_SpinReserveSize.GetValue());
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_venueOption.SetValue(m_optionMode);
	extensionConfig->m_venueOptionCall.SetValue(m_optionCall);
	extensionConfig->m_venueOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_venueOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_venueOptionExpiration.SetValue(GetExpirationDate());
#endif
}

void SampleDialogSettingVenue::UpdateControls()
{
	BaseSampleDialogSettingOrder::UpdateControls();
}

BOOL SampleDialogSettingVenue::OnInitDialog()
{
	m_stopType = 0;
	m_stopBase = 0;
	BOOL ret = BaseSampleDialogSettingOrder::OnInitDialog();

/*
	for(unsigned char pegType = PEG_NONE; pegType < PEG_TYPE_COUNT; ++pegType)
	{
		switch(pegType)
		{
			case PEG_NONE:
			m_ComboBoxPegType.AddString("");
			break;

			case PEG_PRIMARY:
			m_ComboBoxPegType.AddString("Primary");
			break;

			case PEG_MIDPOINT:
			m_ComboBoxPegType.AddString("Midpoint");
			break;

			case PEG_MARKET:
			m_ComboBoxPegType.AddString("Market");
			break;

			default:
			m_ComboBoxPegType.AddString("ERROR");
			break;
		}
	}
	m_ComboBoxPegType.SetCurSel(0);
*/
	for(unsigned char stopType = OST_NONE; stopType < OST_COUNT; ++stopType)
	{
		switch(stopType)
		{
			case OST_NONE:
			m_ComboBoxStopType.AddString("");
			break;

			case OST_NATIVE:
			m_ComboBoxStopType.AddString("Native");
			break;

			case OST_TRAILING:
			m_ComboBoxStopType.AddString("Trailing");
			break;

			case OST_SYNTHETIC:
			m_ComboBoxStopType.AddString("Synthetic");
			break;

			default:
			m_ComboBoxStopType.AddString("ERROR");
			break;
		}
	}
	m_ComboBoxStopType.SetCurSel(0);

	for(unsigned char stopBase = OSB_BID; stopBase < OSB_COUNT; ++stopBase)
	{
		switch(stopBase)
		{
			case OSB_BID:
			m_ComboBoxStopBase.AddString("Bid");
			break;

			case OSB_ASK:
			m_ComboBoxStopBase.AddString("Ask");
			break;

			case OSB_MIDPOINT:
			m_ComboBoxStopBase.AddString("MidPoint");
			break;

			case OSB_PRINT:
			m_ComboBoxStopBase.AddString("Print");
			break;

			default:
			m_ComboBoxStopBase.AddString("ERROR");
			break;
		}
	}
	m_ComboBoxStopBase.SetCurSel(0);

	UpdateAllOrNone();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingVenue::RestoreLayout()
{
	BaseSampleDialogSettingOrder::RestoreLayout();
//	SetAccountId(extensionConfig->m_venueAccountId.GetValue().c_str());

	const std::string& symbol = extensionConfig->m_venueSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

//	extensionConfig->m_venueVenue.GetValue();
	int sel = m_ComboBoxDestination.FindItemByNumericName(extensionConfig->m_venueVenue.GetValue());
	if(sel < 0)sel = 0;
	m_ComboBoxDestination.SetCurSel(sel);
	OnSelchangeDestination();

	sel = m_ComboBoxRouting.FindItemByNumericName(extensionConfig->m_venueRouting.GetValue());
	if(sel < 0)sel = 0;
	m_ComboBoxRouting.SetCurSel(sel);
	OnSelchangeRouting();

	CString mnemonic;
	m_EditMnemonic.SetTextAsUInt(extensionConfig->m_venueMnemonic.GetValue());

	m_SpinReserveSize.SetValue(extensionConfig->m_venueReserveSize.GetValue());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_venueOption.GetValue(),
		extensionConfig->m_venueOptionSuffix.GetValue(),
		extensionConfig->m_venueOptionCall.GetValue(),
		extensionConfig->m_venueOptionStrikePrice.GetValue(),
		extensionConfig->m_venueOptionExpiration.GetValue());
#endif

//	EnableOkAndApplyButton(false);
//	EnableSendMessageButton();

	m_modified = false;
}

void SampleDialogSettingVenue::UpdateAllOrNone()
{
	if(m_stopType)//m_ComboBoxStopType.GetCurSel() >= 1)
	{
		m_CheckBoxAllOrNone.SetInvalid(true);
		m_SpinMinQuantity.SetBkColorEnabled(m_visualBase->GetGrayColor());
	}
	else
	{
		if(m_currentDestination
			&& TD_IsAllOrNoneValid(m_currentDestination->GetId(), m_currentRouting ? m_currentRouting->GetId() : 0)
			&& m_pegType != 3)
		{
			m_CheckBoxAllOrNone.SetInvalid(false);
			m_SpinMinQuantity.SetBkColorEnabled(0xFFFFFFFF);
		}
		else
		{
			m_CheckBoxAllOrNone.SetInvalid(true);
			m_SpinMinQuantity.SetBkColorEnabled(m_visualBase->GetGrayColor());
		}
//		BaseSampleDialogSettingOrder::UpdateAllOrNone();
	}
}

/*
void SampleDialogSettingVenue::InitControls()
{
	m_ComboBoxSide.AddString("Buy");
	m_ComboBoxSide.AddString("Sell");
}
*/
void SampleDialogSettingVenue::DoDataExchange(CDataExchange* pDX)
{
	BaseSampleDialogSettingOrder::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxStopTypeId, m_ComboBoxStopType);
	DDX_Control(pDX, spinStopPriceOffsetId, m_SpinStopOffset);
	DDX_Control(pDX, comboBoxStopBaseId, m_ComboBoxStopBase);

//	DDX_Control(pDX, comboBoxPegTypeId, m_ComboBoxPegType);
//	DDX_Control(pDX, spinPegPriceOffsetId, m_SpinPegOffset);
	DDX_Control(pDX, spinDiscretionaryOffsetId, m_SpinDiscretionaryOffset);

//	DDX_Control(pDX, spinMinQuantityId, m_SpinMinQuantity);
//	DDX_Control(pDX, checkboxAllOrNoneId, m_CheckBoxAllOrNone);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingVenue, BaseSampleDialogSettingOrder)
//	ON_CBN_SELCHANGE(comboBoxPegTypeId, OnSelchangePegType)
	ON_CBN_SELCHANGE(comboBoxStopTypeId, OnSelchangeStopType)
	ON_CBN_SELCHANGE(comboBoxStopBaseId, OnSelchangeStopBase)

//	ON_BN_CLICKED(checkboxAllOrNoneId, OnAllOrNone)
END_MESSAGE_MAP()

void SampleDialogSettingVenue::OnSelchangeStopType()
{
	const unsigned char stopType = (unsigned char)m_ComboBoxStopType.GetCurSel();
	if(stopType != m_stopType)
	{
		m_stopType = stopType;
		m_modified = true;
		UpdatePegValid();
		UpdateStopTypeValid();
		UpdateStopBaseValid();
		UpdateAllOrNone();
	}
}

void SampleDialogSettingVenue::OnSelchangeStopBase()
{
	const unsigned char stopBase = (unsigned char)m_ComboBoxStopBase.GetCurSel();
	if(stopBase != m_stopBase)
	{
		m_stopBase = stopBase;
		m_modified = true;
	}
}

void SampleDialogSettingVenue::VenueChanged()
{
	UpdateAllOrNone();
}

void SampleDialogSettingVenue::RoutingChanged()
{
	UpdateAllOrNone();
}

void SampleDialogSettingVenue::UpdateStopTypeValid()
{
//	bool invalid = m_CheckBoxStopOrder.GetCheck() == BST_UNCHECKED;
//	m_ComboBoxStopType.SetInvalid(invalid);
	const bool invalid = m_stopType < 1;
	m_SpinStopOffset.SetBkColorEnabled(invalid ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));
}

void SampleDialogSettingVenue::UpdateStopBaseValid()
{
//	bool invalid = m_CheckBoxStopOrder.GetCheck() == BST_UNCHECKED || m_ComboBoxStopType.GetCurSel() < 2;
//	bool invalid = m_ComboBoxStopType.GetCurSel() < 2;
	const bool invalid = m_stopType < 2 || m_stopType >= OST_COUNT;
	m_ComboBoxStopBase.SetInvalid(invalid);
}

void SampleDialogSettingVenue::UpdatePegValid()
{
	const bool invalid = m_RadioPriceMarket.GetCheck() == BST_CHECKED || m_stopType > 0;//m_CheckBoxStopOrder.GetCheck() == BST_CHECKED;
	m_SpinDiscretionaryOffset.SetBkColorEnabled(invalid ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));
	m_ComboBoxPegType.SetInvalid(invalid);
	UpdatePegType();
}

void SampleDialogSettingVenue::UpdatePegType()
{
	const bool invalid = m_RadioPriceMarket.GetCheck() == BST_CHECKED || m_stopType > 0 || m_pegType < 1;//m_CheckBoxStopOrder.GetCheck() == BST_CHECKED;
	m_SpinPegOffset.SetBkColorEnabled(invalid ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));//m_SpinTif.GetBkColorDisabled());//GetSysColor(COLOR_BTNFACE));

	UpdateAllOrNone();
}
/*
void SampleDialogSettingVenue::OnSelchangePegType()
{
//	EnableObjectUpdateButton();
	UpdatePegType();
}
*/
unsigned char SampleDialogSettingVenue::SendOrder(Security* stock, const BaseOrderParams& orderParams)
{
	unsigned char ret = OE_OK;
//	bool valid = true;
	VenueOrderParams& venueOrderParams = (VenueOrderParams&)orderParams;
	Account* account = venueOrderParams.GetAccount();
	unsigned char sideId = venueOrderParams.GetSide();
	char side = '\0';
	int positionSize = 0;
	Price positionPrice;
	Price positionCost;
	const Position* position;

//	account->LockInquiryWait();
//	position = account->FindPosition(stock);
//	account->UnlockInquiry();

	position = account->FindPositionWithLock(stock);

	if(position)
	{
		positionSize = position->GetSize();
		positionPrice = Price(position->GetCompactAveragePrice());
		positionCost = Price(position->GetCompactAverageCost());
	}

	switch(sideId)
	{
		case OCS_BUY:
		side = 'B';
		break;

		case OCS_SELL:
		side = 'S';
		break;

		case OCS_CLOSE:
		if(positionSize > 0)
		{
			side = 'S';
		}
		else if(positionSize < 0)
		{
			side = 'B';
		}
		break;

		case OCS_ADD:
		if(positionSize > 0)
		{
			side = 'B';
		}
		else if(positionSize < 0)
		{
			side = 'S';
		}
		break;
	}
	if(!side)
	{
		return OE_SIZE_ZERO;
	}

	Price level1Bid(stock->GetL1Bid());
	Price level1Ask(stock->GetL1Ask());
	Price lastPrint(stock->GetLastNbboPrintPrice());

/*	
	bool sortBySize = false;
	bool hideSlowQuotes = false;
	const unsigned int* lineCount = nullptr;//ALL lines for all books;
	unsigned int exchangeMask = 0xFFFFFFFF;//ALL

	Price level2Bid;
	Price level2Ask;
	TakionIterator* iteratorBid = TU_CreateMmAggregatedIterator(true, sortBySize, hideSlowQuotes, lineCount, exchangeMask, 0xFFFFFFFF, nullptr, stock);
	TakionIterator* iteratorAsk = TU_CreateMmAggregatedIterator(false, sortBySize, hideSlowQuotes, lineCount, exchangeMask, 0xFFFFFFFF, nullptr, stock);

	if(iteratorBid && iteratorAsk)
	{
		stock->LockInquiryWaitLevel2();

		iteratorBid->Reset();
		iteratorAsk->Reset();
		const Quote* quote = TU_GetNextQuote(iteratorBid);
		if(quote)level2Bid = *quote;
		quote = TU_GetNextQuote(iteratorAsk);
		if(quote)level2Ask = *quote;

		stock->UnlockInquiryLevel2();
	}
	Price insideBid = level2Bid.isZero() ? level1Bid : level2Bid;
	Price insideAsk = level2Ask.isZero() ? level1Ask : level2Ask;

	delete iteratorBid;
	delete iteratorAsk;
*/

	Price insideBid = level1Bid;
	Price insideAsk = level1Ask;

	const unsigned char stopType = venueOrderParams.GetStopType();
	unsigned char orderType;
	Price limitPrice;
	SignedPrice signedPrice;
	switch(venueOrderParams.GetPriceType())
	{
		case PT_MARKET:
		orderType = stopType == OST_NONE ? ORDER_TYPE_MARKET : ORDER_TYPE_STOP;
		break;

		case PT_OFFSET:
		orderType = stopType == OST_NONE ? ORDER_TYPE_LIMIT : ORDER_TYPE_STOP_LIMIT;
		ret = CalculatePriceByOffset(side,
			stock,
			insideBid,
			insideAsk,
			positionPrice,
			positionCost,
			venueOrderParams.GetPriceBase(),
			venueOrderParams.GetPriceOffset(),
			venueOrderParams.isPriceOffsetPercent(),
			limitPrice);
		if(ret != OE_OK)
		{
			return ret;
		}
		break;

		default://case PT_ABSOLUTE:
		orderType = stopType == OST_NONE ? ORDER_TYPE_LIMIT : ORDER_TYPE_STOP_LIMIT;
		limitPrice = venueOrderParams.GetPriceAbsolute();
		break;
	}

	Price stopPrice;
//	SignedPrice signedLimitPrice = SignedPrice(limitPrice);
//	const SignedPrice& onePenny = SignedPrice::oneCent;//TU_GetSignedPriceOnePenny();
	Price discretionaryPrice;
	unsigned char pegType = PEG_NONE;
	SignedPrice pegOffset;
	if(stopType == OST_NONE)
	{
		if(!limitPrice.isZero())
		{
			pegType = venueOrderParams.GetPegType();
			if(pegType == PEG_NONE)
			{
//				SignedPrice signedDiscretionaryPrice;
				const SignedPrice discretionaryOffset = SignedPrice(venueOrderParams.GetDiscretionaryOffset());
//				const Price discretionaryOffset = venueOrderParams.GetDiscretionaryOffset();
				if(discretionaryOffset.isPositive())
				{
//					signedDiscretionaryPrice = signedLimitPrice;
					AdjustPrice(discretionaryOffset, false, discretionaryPrice, side);
					if(discretionaryPrice < Price::oneCent)
					{
						discretionaryPrice = Price::oneCent;
						if(discretionaryPrice >= limitPrice)
						{
							discretionaryPrice.SetZero();
						}
					}
				}
			}
			else
			{
				pegOffset = venueOrderParams.GetPegOffset();
			}
		}
	}
	else
	{
//		if(venueOrderParams.GetStopType() != OST_TRAILING)
//		{
			stopPrice = limitPrice;
			AdjustPrice(venueOrderParams.GetStopPriceOffset(), false, stopPrice, side);
/*
		}
		else
		{
			Price priceCalculatedByPriceOffset(limitPrice);
			if(priceCalculatedByPriceOffset.isZero())
			{
				ret = CalculatePriceByOffset(side,
					stock,
					insideBid,
					insideAsk,
					positionPrice,
					positionCost,
					venueOrderParams.GetPriceBase(),
					venueOrderParams.GetPriceOffset(),
					venueOrderParams.isPriceOffsetPercent(),
					priceCalculatedByPriceOffset);
				if(ret != OE_OK)
				{
					return ret;
				}
			}
			signedStopPrice = SignedPrice(priceCalculatedByPriceOffset);
			AdjustPrice(venueOrderParams.GetStopPriceOffset(), false, signedStopPrice, side);
			if(!signedStopPrice.isPositive())
			{
				return OE_STOP_PRICE_ZERO;
			}
		}
*/
	}

	unsigned int orderSize = 0;
	switch(venueOrderParams.GetSizeType())
	{
		case SIT_POSITION:
		orderSize = positionSize < 0 ? (unsigned int)-positionSize : (unsigned int)positionSize;
		break;

		case SIT_SHARES:
		orderSize = venueOrderParams.GetSizeShares();
		break;
	}
	if(!orderSize)
	{
		return OE_SIZE_ZERO;
	}
	unsigned int displaySize = 0;
	switch(venueOrderParams.GetDisplaySizeType())
	{
		case DST_ALL:
		displaySize = orderSize;
		break;

		case DST_SHARES:
		displaySize = venueOrderParams.GetDisplaySizeShares();
		break;
	}

	unsigned char tifType = venueOrderParams.GetTifType();
	unsigned int tifMillisecond = tifType == TIF_GTD ? venueOrderParams.GetTifMilliseconds() : 0;

	unsigned short routingId = venueOrderParams.GetRoutingId();
	unsigned short routingSubType = venueOrderParams.GetRoutingSubType();
	bool iso = stock->isMarketDataStock()
		&& orderType == ORDER_TYPE_LIMIT
		&& !limitPrice.isZero()
		&& !routingId
		&& tifType == TIF_IOC;
//		&& (side == 'B' ? limitPrice == level1Ask : limitPrice == level1Bid);

	std::string isoAttachment;
	if(iso)
	{
		const bool bidSide = side != 'B';

		const bool sortBySize = false;
		const bool hideSlowQuotes = false;
		unsigned int exchangeMask = 0xFFFFFFFF;//ALL

		const unsigned int* lineCount = nullptr;//ALL lines for all books;
		unsigned int lineCountWithoutNyseAmexBook[ECNBOOK_COUNT];
		if(!stock->isOpen())
		{
			unsigned int* lineCountCursor = lineCountWithoutNyseAmexBook;
			for(unsigned char ic = 0; ic < ECNBOOK_COUNT; ++ic, ++lineCountCursor)
			{
				*lineCountCursor = 0xFFFFFFFF;
			}
			lineCountWithoutNyseAmexBook[ECNBOOK_NYS] = 0;
			lineCountWithoutNyseAmexBook[ECNBOOK_AMEX] = 0;

			lineCount = lineCountWithoutNyseAmexBook;
		}
		Price topPrice;

		TakionIterator* allQuotesIterator = TU_CreateMmAggregatedIterator(bidSide, false, false, (1 << CQSF_Count) - 1, lineCount, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, stock);

		stock->LockInquiryWaitLevel2();

		if(TU_GetIsoSnapshot(bidSide, allQuotesIterator, limitPrice, isoAttachment, topPrice, true))
		{
			isoAttachment += '/';
			stock->GetProtectedMarketSnapshot(bidSide, isoAttachment);
		}

		stock->UnlockInquiryLevel2();

		delete allQuotesIterator;

		if(topPrice.isZero() || topPrice != limitPrice)
		{
			iso = false;
		}
	}

	unsigned char cancelWashOrders = WOP_CANCEL;//TD_GetWashOrderPolicy();
	unsigned char oversellSplitPolicy = OOP_SPLIT;//OOP_RESIZE, OOP_SPLIT, OOP_REJECT; TD_GetOversellSplitPolicy()
	bool resizeShortToBorrowed = true;//TD_IsResizeShortToBorrowed()
	bool noRedirect = false;
	bool closeOnly = false;
	bool resizeToClose = false;
	bool imbalanceOnly = false;
	bool syntheticIgnoreFinra = false;
	bool syntheticWaitNext = false;

	bool preBorrow = false;
	Price preBorrowPrice(0, 0);
	bool blockAggressiveOrders = false;//true;
	unsigned char roundLotPolicy = RLP_ROUND_DOWN;//RLP_REJECT, RLP_ROUND_DOWN, RLP_ROUND_UP; TD_GetRoundLotPolicy(),

	ret = account->SendOrder(nullptr,
		nullptr,
		true,
		0,
		stock,
		PE_NOT_POSTPONED,
		0,//timeToActivate
		venueOrderParams.GetDestinationName(),
		venueOrderParams.GetDestinationId(),
		side,
		0,//unsigned char repriceFlags
		orderType,
		venueOrderParams.GetUserType(),
		nullptr,//userOrderInfo
		nullptr,//commandName
		limitPrice,
		discretionaryPrice,
		stopPrice,
		stopType,//venueOrderParams.GetStopType(),
		venueOrderParams.GetStopBase(),
		level1Bid,
		level1Ask,
		lastPrint,
		orderSize,
		venueOrderParams.GetMinQuantity(),
		displaySize,
		venueOrderParams.GetReserveSize(),
		routingId,
		routingSubType,
		venueOrderParams.GetRoutingName(),
		venueOrderParams.GetMnemonic(),
		iso,
		pegType,
		pegOffset,
		tifType,
		tifMillisecond,
		cancelWashOrders,
		oversellSplitPolicy,
		resizeShortToBorrowed,
		preBorrow,
		preBorrowPrice,
		noRedirect,
		closeOnly,
		resizeToClose,
		venueOrderParams.isAllOrNone(),
		syntheticIgnoreFinra,
		syntheticWaitNext,
		imbalanceOnly,
		blockAggressiveOrders,
		roundLotPolicy,
		iso ? &isoAttachment : nullptr,
		venueOrderParams.GetUserId(),
		0,//parentId
		0,//ocoId
		0,//renewMillisecond
		nullptr,//userData
		0,//const unsigned int& maxPositionSize
		venueOrderParams.GetExtensionId(),
		false);//bool openOnly

	return ret;
}

BaseOrderParams* SampleDialogSettingVenue::CreateOrderParams() const
{
	return new VenueOrderParams;
}

unsigned char SampleDialogSettingVenue::FillOrderParams(BaseOrderParams& orderParams) const
{
	unsigned char ret = BaseSampleDialogSettingOrder::FillOrderParams(orderParams);
	if(ret == SPE_OK)
	{
		VenueOrderParams& venueOrderParams = (VenueOrderParams&)orderParams;

//		int sel = m_ComboBoxStopType.GetCurSel();
//		if(sel < 0 || sel >= OST_COUNT)venueOrderParams.SetStopType(OST_NONE);
//		else venueOrderParams.SetStopType((unsigned char)sel);

		venueOrderParams.SetStopType(m_stopType >= OST_COUNT ? OST_NONE : m_stopType);

//		sel = m_ComboBoxStopBase.GetCurSel();
//		if(sel < 0 || sel >= OSB_COUNT)venueOrderParams.SetStopBase(OSB_PRINT);
//		else venueOrderParams.SetStopBase((unsigned char)sel);

		venueOrderParams.SetStopBase(m_stopBase >= OSB_COUNT ? OSB_PRINT : m_stopBase);

		SignedPrice signedPrice;
		FillSignedPriceFromSpinControl(signedPrice, m_SpinStopOffset, 10000);
		venueOrderParams.SetStopPriceOffset(signedPrice);

		Price price;
		FillPriceFromSpinControl(price, m_SpinDiscretionaryOffset, 10000);
		venueOrderParams.SetDiscretionaryOffset(price);

		venueOrderParams.SetMinQuantity(m_SpinMinQuantity.GetValue());
		venueOrderParams.SetAllOrNone(m_allOrNone);

	}
	return ret;
}
