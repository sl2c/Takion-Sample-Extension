#include "stdafx.h"
#include "SampleDialogSettingSwipe.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum SampleSettingVenueIds
{
	comboBoxSwipeTypeId = baseOrderIdsCount,
	checkBoxProactiveId,
	checkBoxSwipeExcludeId,
	listBoxDestinationId,

	checkBoxSkimOddLotId,
	checkBoxPostRemainderId,

	staticPostTifId,
	comboBoxPostTifTypeId,
	spinPostTifId,

	swipeIdsCount
};

SampleDialogSettingSwipe::SampleDialogSettingSwipe(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseSampleDialogSettingOrder(mainWnd, parentTab, "Swipe"),
	m_ComboBoxSwipeType(false),
	m_swipeType(0xFF),
	m_ComboBoxPostTifType(false),
	m_SpinPostTif(true, false)
{
	m_spiderOnlyControls.push_back(&m_GroupPeg);
	m_spiderOnlyControls.push_back(&m_ComboBoxPegType);
	m_spiderOnlyControls.push_back(&m_SpinPegOffset);

	m_nonSpiderOnlyControls.push_back(&m_CheckBoxPostRemainder);
	m_nonSpiderOnlyControls.push_back(&m_ComboBoxDestination);
	m_nonSpiderOnlyControls.push_back(&m_ComboBoxRouting);
	m_nonSpiderOnlyControls.push_back(&m_EditMnemonic);
	m_nonSpiderOnlyControls.push_back(&m_SpinReserveSize);
	m_nonSpiderOnlyControls.push_back(&m_ComboBoxPostTifType);
	m_nonSpiderOnlyControls.push_back(&m_SpinPostTif);
//	m_nonSpiderOnlyControls.push_back(&m_GroupPost);
	m_nonSpiderOnlyControls.push_back(&m_StaticDestination);
	m_nonSpiderOnlyControls.push_back(&m_StaticRouting);
	m_nonSpiderOnlyControls.push_back(&m_StaticMnemonic);
	m_nonSpiderOnlyControls.push_back(&m_StaticReserveSize);
	m_nonSpiderOnlyControls.push_back(&m_StaticPostTif);

	m_ComboBoxSwipeType.SetNoSelectionColor(m_visualBase->GetWarningColor());
	int x = m_horizontalOffset;
	int xc = x + hgap;
	int y = groupOffset / 2;
	int width = 44;
	int listBoxHeight = 58;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y, width, 100, comboBoxSwipeTypeId);
	AddButtonIndirect("Exclude", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc, y += comboBoxHeight, width, checkBoxHeight, checkBoxSwipeExcludeId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y += checkBoxHeight, width, listBoxHeight, listBoxDestinationId);
	AddButtonIndirect("Proactive", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc += width + 1, y, width, checkBoxHeight, checkBoxProactiveId);
	AddButtonIndirect("Skim Odd Lot", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc, y += checkBoxHeight, width, checkBoxHeight, checkBoxSkimOddLotId);

	y = m_displaySizeVerticalOffset + 2;
	x = m_priceHorizontalOffset;
	xc = x + hgap;
	width += comboBoxHeight + 6;
	AddButtonIndirect("Post Remainder", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc, y, width, checkBoxHeight, checkBoxPostRemainderId);

	x = m_destinationHorizontalalOffset + hgap;
	y = m_destinationVerticalOffset;
	const int tifStaticWidth = comboBoxHeight;
	const int tifY = y - comboBoxHeight - 2;
	AddStaticIndirect("TIF", WS_VISIBLE | SS_NOPREFIX, 0, x, tifY + (comboBoxHeight - staticHeight) / 2, tifStaticWidth, staticHeight, staticPostTifId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, x + tifStaticWidth, tifY, width - tifStaticWidth, 100, comboBoxPostTifTypeId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x, y, width + 2, spinHeight, spinPostTifId);
}

unsigned char SampleDialogSettingSwipe::SendOrder(Security* stock, const BaseOrderParams& orderParams)
{
	unsigned char ret = OE_OK;
	SwipeOrderParams& swipeOrderParams = (SwipeOrderParams&)orderParams;
	unsigned char swipeType = swipeOrderParams.GetSwipeType();
	Account* account = swipeOrderParams.GetAccount();
	unsigned char sideId = swipeOrderParams.GetSide();
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

	Price level1Bid = Price(stock->GetL1Bid());
	Price level1Ask = Price(stock->GetL1Ask());
	Price level2Bid;
	Price level2Ask;

	bool hideSlowQuotes = true;
	unsigned int hideClosedQuotesMask = (1 << CQSF_Count) - 1;

	bool sortBySize = false;
	const unsigned int* lineCount = nullptr;//ALL lines for all books;
	unsigned int exchangeMask = 0xFFFFFFFF;//ALL
	
	TakionIterator* iteratorBid = TU_CreateMmAggregatedIterator(true, sortBySize, hideSlowQuotes, hideClosedQuotesMask, lineCount, exchangeMask, 0xFFFFFFFF, nullptr, stock);
	TakionIterator* iteratorAsk = TU_CreateMmAggregatedIterator(false, sortBySize, hideSlowQuotes, hideClosedQuotesMask, lineCount, exchangeMask, 0xFFFFFFFF, nullptr, stock);

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

	Price limitPrice;
	SignedPrice signedPrice;
	switch(swipeOrderParams.GetPriceType())
	{
		case PT_OFFSET:
		ret = CalculatePriceByOffset(side,
			stock,
			insideBid,
			insideAsk,
			positionPrice,
			positionCost,
			swipeOrderParams.GetPriceBase(),
			swipeOrderParams.GetPriceOffset(),
			swipeOrderParams.isPriceOffsetPercent(),
			limitPrice);
		if(ret != OE_OK)
		{
			return ret;
		}
		break;

		default://case PT_ABSOLUTE:
		limitPrice = swipeOrderParams.GetPriceAbsolute();
		break;
	}

	if(limitPrice.isZero())
	{
		return OE_PRICE_ZERO;
	}

	unsigned char pegType = PEG_NONE;
	SignedPrice pegOffset;
	if(swipeType == SWIPE_SPIDER)
	{
		pegType = swipeOrderParams.GetPegType();
		if(pegType != PEG_NONE)
		{
			pegOffset = swipeOrderParams.GetPegOffset();
		}
	}

	unsigned int orderSize = 0;
	switch(swipeOrderParams.GetSizeType())
	{
		case SIT_POSITION:
		orderSize = positionSize < 0 ? (unsigned int)-positionSize : (unsigned int)positionSize;
		break;

		case SIT_SHARES:
		orderSize = swipeOrderParams.GetSizeShares();
		break;
	}
	if(!orderSize)
	{
		return OE_SIZE_ZERO;
	}
	unsigned int displaySize = 0;
	switch(swipeOrderParams.GetSizeType())
	{
		case DST_ALL:
		displaySize = orderSize;
		break;

		case DST_SHARES:
		displaySize = swipeOrderParams.GetDisplaySizeShares();
		break;
	}

	unsigned char tifType = swipeOrderParams.GetTifType();
	unsigned int tifMillisecond = tifType == TIF_GTD ? swipeOrderParams.GetTifMilliseconds() : 0;

	bool proactive = swipeOrderParams.isProactive();

	bool cancelWashOrders = true;
	unsigned char oversellSplitPolicy = OOP_SPLIT;//OOP_RESIZE, OOP_SPLIT, OOP_REJECT

	bool resizeShortToBorrowed = true;
	bool preBorrow = false;
	bool postRemainderOnIsoViolation = false;
	bool reiterate = true;
	bool forceAlgo = false;
	Price preBorrowPrice(0, 0);

	unsigned char postPegType = 0;
	SignedPrice postPegOffset = SignedPrice::signedPriceZero;
	unsigned int postDisplayQuantity = 0;
	SignedPrice postPriceOffset = SignedPrice::signedPriceZero;
	bool postPriceOffsetPercent = false;

	bool noRedirect = false;
	bool closeOnly = false;
	bool resizeToClose = false;
	unsigned char roundLotPolicy = RLP_ROUND_DOWN;//RLP_REJECT, RLP_ROUND_DOWN, RLP_ROUND_UP,

	Account::UIntSet quoteFilterSet;//empty set;

	unsigned char postTifType = swipeOrderParams.GetPostTifType();
	unsigned int postTifMilliseconds = postTifType == TIF_GTD ? swipeOrderParams.GetPostTifMilliseconds() : 0;

	unsigned int swipeClientId;

	ret = account->SendSwipeOrder(&swipeClientId,
		stock,
		PE_NOT_POSTPONED,
		0,//timeToActivate
		swipeType,
		swipeOrderParams.GetDestinationSet(),
		swipeOrderParams.isExcludeDestinations(),
		quoteFilterSet,//m_swipeType == SWIPE_MEGA ? DestSet() : mm ? mm->GetQuoteFilterSet(open) : frame->GetQuoteFilterSet(open),
		side,
		0,//unsigned char repriceFlags
		swipeOrderParams.GetUserType(),//userOrderType
		nullptr,//userOrderInfo
		nullptr,//commandName
		limitPrice,
		orderSize,
		displaySize,
		0,//reserveQuantity,
		proactive ? RI_PROACTIVE : 0,
		proactive ? TD_GetDefaultProActiveRoutingNumericName() : 0,//*(unsigned int*)"ProA" : 0,
		tifType,//not implemented yet, reserved for future use
		tifType == TIF_GTD ? tifMillisecond : 0,//not implemented yet, reserved for future use
		cancelWashOrders,
		oversellSplitPolicy,
		resizeShortToBorrowed,
		preBorrow,
		preBorrowPrice,
		noRedirect,
		closeOnly,
		resizeToClose,
		swipeOrderParams.isAllOrNone(),
		swipeOrderParams.GetMinQuantity(),
		false,//blockAggressiveOrders
		roundLotPolicy,
		hideSlowQuotes,
		hideClosedQuotesMask,

		swipeOrderParams.isSkimOddLot(),
		swipeOrderParams.isPostRemainder(),
		swipeOrderParams.GetDestinationId(),
		swipeOrderParams.GetDestinationName(),
		swipeOrderParams.GetRoutingName(),
		swipeOrderParams.GetRoutingId(),
		swipeOrderParams.GetRoutingSubType(),
		swipeOrderParams.GetMnemonic(),
		postTifType,
		tifType == TIF_GTD ? postTifMilliseconds : 0,

		postPegType,
		postPegOffset,
		postDisplayQuantity,
		postPriceOffset,
		postPriceOffsetPercent,

		postRemainderOnIsoViolation,

		reiterate,
		forceAlgo,

		swipeOrderParams.GetUserId(),
		0,//parent id
		account->GetCurrentOcoId(),
		pegType,
		pegOffset,
		nullptr,//const void* const& userData
		swipeOrderParams.GetExtensionId(),
		false);//const bool& openOnly
	return OE_OK;
}

BaseOrderParams* SampleDialogSettingSwipe::CreateOrderParams() const
{
	return new SwipeOrderParams;
}

unsigned char SampleDialogSettingSwipe::FillOrderParams(BaseOrderParams& orderParams) const
{
	unsigned char ret = BaseSampleDialogSettingOrder::FillOrderParams(orderParams);

	if(ret == SPE_OK)
	{
		SwipeOrderParams& swipeOrderParams = (SwipeOrderParams&)orderParams;
		if(m_swipeType >= SWIPE_TYPE_COUNT)
		{
			return SPE_NO_SWIPE_TYPE;
		}
		swipeOrderParams.SetSwipeType(m_swipeType);
		swipeOrderParams.SetProactive(m_CheckBoxProactive.GetCheck() == BST_CHECKED);

		swipeOrderParams.SetExcludeDestinations(m_CheckBoxSwipeExclude.GetCheck() == BST_CHECKED);
		swipeOrderParams.SetSkimOddLot(m_CheckBoxSkimOddLot.GetCheck() == BST_CHECKED);
		swipeOrderParams.SetPostRemainder(m_CheckBoxPostRemainder.GetCheck() == BST_CHECKED);
		swipeOrderParams.SetPostTifType(GetPostTifType());
		swipeOrderParams.SetPostTifMilliseconds(GetPostTifMilliseconds());

		swipeOrderParams.SetMinQuantity(m_SpinMinQuantity.GetValue());
		swipeOrderParams.SetAllOrNone(m_allOrNone);

		swipeOrderParams.ClearDestinationSet();

		const ListBoxCheckDestination::DestinationSet& checkSet = m_ListBoxDestination.GetCheckedSet();
		ListBoxCheckDestination::DestinationSet::const_iterator itc = checkSet.begin(), itcend = checkSet.end();

		for(; itc != itcend; ++itc)
		{
			swipeOrderParams.AddDestination((*itc)->GetMainNumericName());
		}
	}

	return ret;
}

void SampleDialogSettingSwipe::DoDataExchange(CDataExchange* pDX)
{
	BaseSampleDialogSettingOrder::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxSwipeTypeId, m_ComboBoxSwipeType);
	DDX_Control(pDX, checkBoxProactiveId, m_CheckBoxProactive);
	DDX_Control(pDX, checkBoxSwipeExcludeId, m_CheckBoxSwipeExclude);
	DDX_Control(pDX, listBoxDestinationId, m_ListBoxDestination);

	DDX_Control(pDX, checkBoxSkimOddLotId, m_CheckBoxSkimOddLot);
	DDX_Control(pDX, checkBoxPostRemainderId, m_CheckBoxPostRemainder);

	DDX_Control(pDX, staticPostTifId, m_StaticPostTif);
	DDX_Control(pDX, comboBoxPostTifTypeId, m_ComboBoxPostTifType);
	DDX_Control(pDX, spinPostTifId, m_SpinPostTif);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSwipe, BaseSampleDialogSettingOrder)
//	ON_CBN_SELCHANGE(comboBoxPegTypeId, OnSelchangePegType)
	ON_CBN_SELCHANGE(comboBoxSwipeTypeId, OnSelchangeSwipeType)
	ON_CBN_SELCHANGE(comboBoxPostTifTypeId, OnSelchangePostTifType)
	ON_BN_CLICKED(checkBoxSwipeExcludeId, OnExcludeDestinations)
//	ON_BN_CLICKED(IDB_ORDER_MODULE, OnOrderModule)
	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxRangeChecked)
END_MESSAGE_MAP()

LRESULT SampleDialogSettingSwipe::OnListBoxChecked(WPARAM w, LPARAM l)
{
	m_ListBoxDestination.Invalidate(FALSE);
	return 0;
}

LRESULT SampleDialogSettingSwipe::OnListBoxRangeChecked(WPARAM w, LPARAM l)
{
	m_ListBoxDestination.Invalidate(FALSE);
	return 0;
}

void SampleDialogSettingSwipe::OnExcludeDestinations()
{
	m_ListBoxDestination.SetExclude(m_CheckBoxSwipeExclude.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingSwipe::OnSelchangeSwipeType()
{
	int sel = m_ComboBoxSwipeType.GetCurSel();
	unsigned char swipeType = sel < 0 ? 0xFF : (unsigned char)sel;
	if(m_swipeType != swipeType)
	{
		DoSetSwipeType(swipeType);
	}
}

void SampleDialogSettingSwipe::DoSetSwipeType(unsigned char swipeType)
{
	bool oldSpider = m_swipeType == SWIPE_SPIDER;
	bool spider = swipeType == SWIPE_SPIDER;
	if(oldSpider != spider || m_swipeType == 0xFF)
	{
		SwitchSpider(spider);
	}
	else
	{
		m_EditMnemonic.Invalidate(FALSE);
		m_CheckBoxPostRemainder.Invalidate(FALSE);
	}
	m_swipeType = swipeType;
}

void SampleDialogSettingSwipe::SwitchSpider(bool spider)
{
	WindowList::iterator it;
	WindowList::iterator itend;
	int spiderShow;
	int nonSpiderShow;
	if(spider)
	{
		spiderShow = SW_SHOW;
		nonSpiderShow = SW_HIDE;
	}
	else
	{
		spiderShow = SW_HIDE;
		nonSpiderShow = SW_SHOW;
	}
	for(it = m_spiderOnlyControls.begin(), itend = m_spiderOnlyControls.end(); it != itend; ++it)
	{
		(*it)->ShowWindow(spiderShow);
	}
	for(it = m_nonSpiderOnlyControls.begin(), itend = m_nonSpiderOnlyControls.end(); it != itend; ++it)
	{
		(*it)->ShowWindow(nonSpiderShow);
	}
}

unsigned int SampleDialogSettingSwipe::GetPostTifMilliseconds() const
{
	return m_SpinPostTif.GetMillisecond();
}

unsigned char SampleDialogSettingSwipe::GetPostTifType() const
{
	int sel = m_ComboBoxPostTifType.GetCurSel();
	if(sel >= 0)
	{
/*
	TIF_DAY	= '0',		//Day (or session)
//	TIF_GTC = '1',		//Good Till Cancel (GTC)
	TIF_OPG = '2',		//At the Opening (OPG)
	TIF_IOC = '3',		//Immediate Or Cancel (IOC)
	TIF_FOK = '4',		//Fill Or Kill (FOK)
	TIF_GTX = '5',		//Good Till Crossing (GTX)
	TIF_GTD = '6',		//Good Till Date (GTD)
	TIF_ATC = '7',		//At the Close
*/
//		if(sel == 0)
//		return TIF_DAY;

		switch(sel)
		{
			case 0:
			return TIF_DAY;

			case 7:
			return TIF_ETC;

			default:
			return TIF_DAY + 1 + (unsigned char)sel;
		}
//		return TIF_DAY + 1 + (unsigned char)sel;
	}
	return 0;//0xFF;
}

void SampleDialogSettingSwipe::OnSelchangePostTifType()
{
	UpdatePostTifType();
}

void SampleDialogSettingSwipe::UpdatePostTifType()
{
	m_SpinPostTif.SetBkColorEnabled(GetPostTifType() == TIF_GTD ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());//m_SpinTif.GetBkColorDisabled());//GetSysColor(COLOR_BTNFACE));
}

BOOL SampleDialogSettingSwipe::OnInitDialog()
{
	BOOL ret = BaseSampleDialogSettingOrder::OnInitDialog();

	FillTifComboBox(m_ComboBoxPostTifType);

	m_SpinPostTif.SetLeftmostMaxDigitValue(8);

	m_RadioPriceMarket.EnableWindow(FALSE);

	for(unsigned char swipeType = SWIPE_MEGA; swipeType < SWIPE_TYPE_COUNT; ++swipeType)
	{
		switch(swipeType)
		{
			case SWIPE_MEGA:
			m_ComboBoxSwipeType.AddString("MEGA");
			break;

			case SWIPE_DUMB:
			m_ComboBoxSwipeType.AddString("SWIP");
			break;

			case SWIPE_SMART:
			m_ComboBoxSwipeType.AddString("SMRT");
			break;

			case SWIPE_SPIDER:
			m_ComboBoxSwipeType.AddString("SPDR");
			break;

			default:
			m_ComboBoxSwipeType.AddString("ERROR");
			break;
		}
	}
	m_ComboBoxSwipeType.SetCurSel(0);
	DoSetSwipeType(SWIPE_MEGA);

	m_ComboBoxPostTifType.SetCurSel(0);
	OnSelchangePostTifType();

	TakionIterator* iterator = TD_CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		if(!destination->isBorrow())
		{
			m_ListBoxDestination.AddString((const char*)destination);
		}
	}
	delete iterator;

	m_CheckBoxSwipeExclude.SetCheck(BST_CHECKED);
	m_ListBoxDestination.SetExclude(true);

	UpdateAllOrNone();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSwipe::RestoreLayout()
{
	BaseSampleDialogSettingOrder::RestoreLayout();
//	SetAccountId(extensionConfig->m_swipeAccountId.GetValue().c_str());

	const std::string& symbol = extensionConfig->m_swipeSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	int sel = m_ComboBoxDestination.FindItemByNumericName(extensionConfig->m_swipeVenue.GetValue());
	if(sel < 0)sel = 0;
	m_ComboBoxDestination.SetCurSel(sel);
	OnSelchangeDestination();

	sel = m_ComboBoxRouting.FindItemByNumericName(extensionConfig->m_swipeRouting.GetValue());
	if(sel < 0)sel = 0;
	m_ComboBoxRouting.SetCurSel(sel);
	OnSelchangeRouting();

	CString mnemonic;
	m_EditMnemonic.SetTextAsUInt(extensionConfig->m_swipeMnemonic.GetValue());

	m_SpinReserveSize.SetValue(extensionConfig->m_swipeReserveSize.GetValue());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_swipeOption.GetValue(),
		extensionConfig->m_swipeOptionSuffix.GetValue(),
		extensionConfig->m_swipeOptionCall.GetValue(),
		extensionConfig->m_swipeOptionStrikePrice.GetValue(),
		extensionConfig->m_swipeOptionExpiration.GetValue());
#endif

//	EnableApplyButton(false);
//	EnableSendMessageButton();

	m_modified = false;
}

StringSetting* SampleDialogSettingSwipe::GetAccountIdSetting()
{
	return &extensionConfig->m_swipeAccountId;
}

void SampleDialogSettingSwipe::UpdateSettings()
{
	BaseSampleDialogSettingOrder::UpdateSettings();
	extensionConfig->m_swipeSymbol.SetValue(m_symbol);

	extensionConfig->m_swipeVenue.SetValue(m_currentDestination ? m_currentDestination->GetMainNumericName() : 0);

	int sel = m_ComboBoxRouting.GetCurSel();
	const DestinationRouting* routing = sel < 0 ? nullptr : (const DestinationRouting*)m_ComboBoxRouting.GetItemDataPtr(sel);
	extensionConfig->m_swipeRouting.SetValue(routing ? routing->GetNumericName() : 0);

	CString mnemonic;
	m_EditMnemonic.GetWindowText(mnemonic);
	extensionConfig->m_swipeMnemonic.SetValue(U_RepresentStringAsUnsignedNumber<unsigned int>(mnemonic));//U_StringToUInt(mnemonic));

	extensionConfig->m_swipeReserveSize.SetValue(m_SpinReserveSize.GetValue());

#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_swipeOption.SetValue(m_optionMode);
	extensionConfig->m_swipeOptionCall.SetValue(m_optionCall);
	extensionConfig->m_swipeOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_swipeOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_swipeOptionExpiration.SetValue(GetExpirationDate());
#endif
}
