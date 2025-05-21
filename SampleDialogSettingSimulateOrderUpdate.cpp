#include "stdafx.h"
#include "SampleDialogSettingSimulateOrderUpdate.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char* const traderIdExcludeChars = "\\/:*?\"<>|";

void InitializeOrderHeader(TMessageOrderHeader& header)
{
	header.AccountName[sizeof(header.AccountName) - 1] = '\0';
	header.FirmID[sizeof(header.FirmID) - 1] = '\0';
	header.Ip = U_GetIpAddressNum();
	header.Tracking = TR_ON_MARKET;

	*(TraderIdType*)header.TraderID = TD_GetNumericTraderIdPrintable();
	header.TraderID[sizeof(header.TraderID) - 1] = '\0';

	header.Symbol[sizeof(header.Symbol) - 1] = '\0';
	header.MarketReferenceID[sizeof(header.MarketReferenceID) - 1] = '\0';
}

void SampleDialogSettingSimulateOrderUpdate::FillOrderHeader(TMessageOrderHeader& header) const
{
#ifndef TAKION_NO_OPTIONS
	if(isOptionUsed())
	{
		OptionKey optionKey(m_strikePrice.GetCompactPrice(),
			m_optionCall,
			m_optionSuffix,
			m_expirationDay,
			m_expirationMonth,
			m_expirationYear);

		if(m_optionSuffix)U_CopyPlusOneCharAndPad(header.Symbol, sizeof(header.Symbol), m_symbol.c_str(), m_optionSuffix, '\0');
		else U_CopyAndPad(header.Symbol, sizeof(m_msgOrderReject.Symbol), m_symbol.c_str(), '\0');

		*(unsigned __int64*)header.ConfirmationID = optionKey.CalculateOptionBlock();

		header.InstrumentType = 1;
	}
	else
	{
#endif
		U_CopyAndPad(header.Symbol, sizeof(header.Symbol), m_symbol.c_str(), '\0');
//		header.InstrumentType = 0;
		header.InstrumentType =
#ifndef TAKION_NO_OPTIONS
			TU_IsFutureSymbol(m_symbol.c_str()) ? ST_FUTURE:
#endif
			ST_STOCK;
#ifndef TAKION_NO_OPTIONS
	}
#endif
	
	header.AcceptedTime = 
	header.Time = 
	header.m_time = TL_GetCurrentMillisecond();

	U_CopyAndPad(header.AccountName, sizeof(header.AccountName), m_accountId, '\0');
	
	header.Quantity = m_size;
	header.RemainingQuantity = m_remaining;
	header.ClOrdID = m_clientId;
	header.SrOrdID = m_serverId;

	switch(m_side)
	{
		case 'B':
		header.Side = ORDER_SIDE_BUY;
		break;

		case 'S':
		header.Side = ORDER_SIDE_SELL;
		break;

		default:
		header.Side = ORDER_SIDE_SELL_SHORT;
		break;
	}

	header.RealTimeOrRecovered = m_historical ? 'R': '\0';
}

void SampleDialogSettingSimulateOrderUpdate::FillReasonHeader(TMsgOrderReason& header) const
{
	FillOrderHeader(header);
	header.ReasonCode = m_reasonCode;
}

SampleDialogSettingSimulateOrderUpdate::SampleDialogSettingSimulateOrderUpdate(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab) :
	BaseDialogSettingSimulateAccountOption(mainWnd, parentTab, "Order Update"),
	m_SpinSize(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinRemaining(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinClientId(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinServerId(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinBid(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinAsk(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinLast(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_ComboBoxSide(false),
	m_ComboBoxReRoute(false),
	m_EditMmid("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", false),
	m_SpinReserveSize(100, 0, 0, 1000, 3, TakionSpin::charComma),
	m_ComboBoxTifType(false),
	m_SpinTif(true, false),
	m_EditMarketRefId(traderIdExcludeChars, true),
	m_CheckBoxHistorical("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxMarket("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_ComboBoxMessageType(false),
	m_SpinReasonCode(0xFFFF, 0, 0, 1000, 3, TakionSpin::charComma),
//	m_CheckBoxBorrowAccept("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_ListBoxExecutionInstructions(32, false, "Execution Instructions", true),

	m_ComboBoxExecutionLiquidity1(m_visualBase->GetGrayColor()),
	m_ComboBoxExecutionLiquidity2(m_visualBase->GetGrayColor()),
	m_SpinExecutionFilled(999999, 0, 0, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Execution Size"),
	m_SpinExecutionCancelled(999999, 0, 0, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Total Cancelled Size"),
	m_SpinExecutionRemaining(999999, 0, 0, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Total Remaining Size"),
	m_SpinExecutionPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4, TakionSpin::charDot, 1, true, true, false, 0, true, "Execution Price"),
	m_EditExecutionMmid("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", false),
	m_EditExecutionRefId(traderIdExcludeChars, true),
	m_ListBoxExecutionReportFlags(9, false, "Execution Report Type Flags", true),
	m_historical(false),
//	m_reroutedToBaml(false),
	m_reRoute(0),
	m_messageType(0),
	m_market(false),
//	m_borrowAccept(false),
	m_side('B'),
	m_size(0),
	m_reserveSize(0),
	m_remaining(0),
	m_clientId(0),
	m_serverId(0),
	m_reasonCode(0),
	m_currentDestination(nullptr),
	m_liquidity1('\0'),
	m_liquidity2('\0'),
	m_executionFilled(0),
	m_executionCancelled(0),
	m_executionRemaining(0),
	m_executionDestination(nullptr)
{
	m_ComboBoxDestination.SetItemToolTips(true);
	m_ComboBoxRouting.SetItemToolTips(true);

	m_ComboBoxDestination.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxRouting.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxTifType.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_ComboBoxExecutionLiquidity1.SetColorInvalid(m_visualBase->GetGrayColor());
	m_ComboBoxExecutionLiquidity2.SetColorInvalid(m_visualBase->GetGrayColor());

	m_ComboBoxExecutionDestination.SetItemToolTips(true);
//	m_ComboBoxExecutionDestination.SetNoSelectionColor(m_visualBase->GetWarningColor());

	unsigned int i = 0;
	m_ListBoxExecutionInstructions.AddStringItem("StayOnOfferSide", i);
	m_ListBoxExecutionInstructions.AddStringItem("NotHeld", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("Work", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("GoAlong", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("OverTheDay", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("Held", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("ParticipateNoInitiate", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("StrictScale", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("TryToScale", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("StayOnBidSide", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("NoCross", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("OkToCross", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("CallFirst", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("PercentOfVolume", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("DNI", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("DNR", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("AON", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("InstitutionsOnly", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("PegLast", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("PegMidPrice", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("NonNegotiable", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("PegOpening", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("PegMarket", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("PegPrimary", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("Suspend", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("InterMarketSweep", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("PegFixed", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("CustomerDisplay", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("Netting", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("PegToWap", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("ImbalanceOnly", ++i);
	m_ListBoxExecutionInstructions.AddStringItem("PegMidPointDiscretionary", ++i);
/*
REPORT_FILLED = 0x01,	// Part or all of order filled
REPORT_CANCELED = 0x02,	// Part or all of order canceled
REPORT_NOFILLED = 0x04,	// Part or all of order not filled
REPORT_REJECTED = 0x08,	// Part or all of order was rejected
REPORT_TIMEOUT = 0x10,	// Order timed out
REPORT_CANCEL_SOLICITED = 0x20,	// Part or all of order canceled due to trader request
REPORT_BUSTED = 0x40,	// Execution busted
REPORT_FOREIGN = 0x80,	// Execution foreign
REPORT_REJECTED_NO_OATS = 0x100,
*/
	i = 0;
	m_ListBoxExecutionReportFlags.AddStringItem("Filled", i);
	m_ListBoxExecutionReportFlags.AddStringItem("Cancelled", ++i);
	m_ListBoxExecutionReportFlags.AddStringItem("NoFilled", ++i);
	m_ListBoxExecutionReportFlags.AddStringItem("Rejected", ++i);
	m_ListBoxExecutionReportFlags.AddStringItem("TimedOut", ++i);
	m_ListBoxExecutionReportFlags.AddStringItem("CancelSolicited", ++i);
	m_ListBoxExecutionReportFlags.AddStringItem("Busted", ++i);
	m_ListBoxExecutionReportFlags.AddStringItem("Foreign", ++i);
	m_ListBoxExecutionReportFlags.AddStringItem("RejectedNoOats", ++i);

	InitializeOrderHeader(m_msg);
/*
	m_msg.AccountName[sizeof(m_msg.AccountName) - 1] = '\0';
	m_msg.FirmID[sizeof(m_msg.FirmID) - 1] = '\0';
	m_msg.Ip = U_GetIpAddressNum();
	m_msg.Tracking = TR_ON_MARKET;

	*(TraderIdType*)m_msg.TraderID = TD_GetNumericTraderIdPrintable();

	m_msg.TraderID[sizeof(m_msg.TraderID) - 1] = '\0';

	m_msg.Symbol[sizeof(m_msg.Symbol) - 1] = '\0';
	m_msg.MarketReferenceID[sizeof(m_msg.MarketReferenceID) - 1] = '\0';
*/
	m_msg.market_maker[sizeof(m_msg.market_maker) - 1] = '\0';
	m_borrowAcceptRequest.Tracking = TR_ON_MARKET;

	m_msgOrderExecute.MMID[sizeof(m_msgOrderExecute.MMID) - 1] = '\0';
/*
	m_borrowAcceptRequest.AccountName[sizeof(m_borrowAcceptRequest.AccountName) - 1] = '\0';
	m_borrowAcceptRequest.FirmID[sizeof(m_borrowAcceptRequest.FirmID) - 1] = '\0';
	m_borrowAcceptRequest.Ip = U_GetIpAddressNum();

	*(TraderIdType*)m_borrowAcceptRequest.TraderID = TD_GetNumericTraderIdPrintable();
//	U_CopyAndPad(m_borrowAcceptRequest.TraderID, sizeof(m_borrowAcceptRequest.TraderID) - 1, TD_GetTraderId(), '\0');
	m_borrowAcceptRequest.TraderID[sizeof(m_borrowAcceptRequest.TraderID) - 1] = '\0';

	m_borrowAcceptRequest.Symbol[sizeof(m_borrowAcceptRequest.Symbol) - 1] = '\0';
	m_borrowAcceptRequest.MarketReferenceID[sizeof(m_borrowAcceptRequest.MarketReferenceID) - 1] = '\0';
*/
	InitializeOrderHeader(m_borrowAcceptRequest);
	InitializeOrderHeader(m_msgOrderReject);
	InitializeOrderHeader(m_msgCancelReject);
	InitializeOrderHeader(m_msgKillOrder);
	InitializeOrderHeader(m_msgOrderExecute);

//	m_msgOrderReject.Tracking = TR_ON_MARKET;
	*m_msgOrderReject.ReasonText = '\0';
	*m_msgCancelReject.ReasonText = '\0';
	*m_msgKillOrder.ReasonText = '\0';
	*m_msgOrderExecute.ReasonText = '\0';


	*(unsigned int*)m_mmid = 0;
	m_mmid[sizeof(unsigned int)] = '\0';

	*(unsigned int*)m_executionMmid = 0;
	m_executionMmid[sizeof(unsigned int)] = '\0';

	U_CopyAndPad(m_marketReferenceId, sizeof(m_marketReferenceId) - 1, "", '\0', true);
	U_CopyAndPad(m_executionReferenceId, sizeof(m_executionReferenceId) - 1, "", '\0', true);

	SetSizeSpinCells(m_SpinSize);
	SetSizeSpinCells(m_SpinRemaining);
	SetSizeSpinCells(m_SpinClientId);
	SetSizeSpinCells(m_SpinServerId);

	SetPriceSpinCells(m_SpinPrice);
	SetPriceSpinCells(m_SpinBid);
	SetPriceSpinCells(m_SpinAsk);
	SetPriceSpinCells(m_SpinLast);

	SetSizeSpinCells(m_SpinExecutionFilled);
	SetSizeSpinCells(m_SpinExecutionCancelled);
	SetSizeSpinCells(m_SpinExecutionRemaining);
	SetPriceSpinCells(m_SpinExecutionPrice);

	m_SpinReserveSize.AddSpinCell(1, false);
	m_SpinReserveSize.AddSpinCell(100, false);

	m_SpinReasonCode.AddSpinCell(1, false);
	m_SpinReasonCode.AddSpinCell(100, false);
	m_SpinReasonCode.AddSpinCell(10000, true);

	const int doubleWidth = m_firstColumnWidth + m_firstColumnWidth;
//	const int priceWidth = m_firstColumnWidth + spinHeight * 11 / 8;
	const int priceWidth = m_firstColumnWidth + spinHeight * 15 / 8;
	const int priceNameWidth = doubleWidth - priceWidth;
//	const int moneyWidth = m_firstColumnWidth + spinHeight * 24 / 8;
//	const int moneyNameWidth = doubleWidth - moneyWidth;

	m_bottomControl += vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinOrderUpdateSizeId);
	AddStaticIndirect("Quantity", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth + 1, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateSizeId);

	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinOrderUpdateRemainingId);
	AddStaticIndirect("Remaining", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth + 1, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateRemainingId);

	AddButtonIndirect("Market", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, m_bottomControl += spinHeight + vgap, m_firstColumnWidth, checkBoxHeight, checkBoxOrderUpdateMarketId);

	m_bottomControl += checkBoxHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinOrderUpdatePriceId);
	AddStaticIndirect("Price", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticOrderUpdatePriceId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinOrderUpdateBidId);
	AddStaticIndirect("Bid", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticOrderUpdateBidId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinOrderUpdateAskId);
	AddStaticIndirect("Ask", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticOrderUpdateAskId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinOrderUpdateLastId);
	AddStaticIndirect("Last", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticOrderUpdateLastId);

	m_bottomControl += spinHeight + vgap;

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 400, comboBoxOrderUpdateSideId);
	AddStaticIndirect("Side", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateSideId);

	m_bottomControl += comboBoxHeight + vgap;

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_SORT|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 400, comboBoxOrderUpdateDestinationId);
	AddStaticIndirect("Venue", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateDestinationId);

	m_bottomControl += comboBoxHeight + vgap;

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_SORT|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 400, comboBoxOrderUpdateRoutingId);
	AddStaticIndirect("Routing", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateRoutingId);

	m_bottomControl += comboBoxHeight + vgap;

	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL|ES_UPPERCASE, 0, m_xc, m_bottomControl, m_firstColumnWidth, editHeight, editOrderUpdateMmidId);
	AddStaticIndirect("MMID", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateMmidId);

	m_bottomControl += editHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinOrderUpdateReserveSizeId);
	AddStaticIndirect("Reserve", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticOrderUpdateReserveSizeId);

	m_bottomControl += spinHeight + vgap;

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 400, comboBoxOrderUpdateReRouteId);
	AddStaticIndirect("ReRoute", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateReRouteId);

	m_bottomControl += comboBoxHeight + vgap;

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 100, comboBoxOrderUpdateTifId);
	AddStaticIndirect("TIF", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateTifId);

	m_bottomControl += comboBoxHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinOrderUpdateMillisecondId);
	AddStaticIndirect("Millisecond", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticOrderUpdateMillisecondId);

	m_bottomControl += spinHeight + vgap;

	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL|ES_UPPERCASE, 0, m_xc, m_bottomControl, priceWidth, editHeight, editOrderUpdateMarketRefIdId);
	AddStaticIndirect("RefId", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (editHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticOrderUpdateMarketRefIdId);

	m_bottomControl += editHeight + vgap;

	AddButtonIndirect("Historical", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, m_bottomControl, m_firstColumnWidth, checkBoxHeight, checkBoxOrderUpdateHistoricalId);

	m_bottomControl += checkBoxHeight + vgap;

//	AddButtonIndirect("ReRoute BAML", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | WS_GROUP, 0, m_xc, m_bottomControl, m_firstColumnWidth, checkBoxHeight, checkBoxOrderUpdateReroutedToBamlId);

//	m_bottomControl += checkBoxHeight + vgap;

	int y = vgap;
	m_xc = m_firstColumnWidth + m_firstColumnWidth + 4 * hgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y, m_firstColumnWidth, spinHeight, spinOrderUpdateClientIdId);
	AddStaticIndirect("Client ID", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, y + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateClientIdId);

	y += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y, m_firstColumnWidth, spinHeight, spinOrderUpdateServerIdId);
	AddStaticIndirect("Server ID", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, y + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateServerIdId);

	y += spinHeight + spinHeight + vgap;
//	AddButtonIndirect("BorrowAccept", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | WS_GROUP, 0, m_xc, y, m_firstColumnWidth, checkBoxHeight, checkBoxBorrowAcceptId);

	const int messageTypeY = y;

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL, 0, m_xc, y, m_firstColumnWidth, 400, comboBoxMessageTypeId);
	AddStaticIndirect("Message Type", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, y + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticMessageTypeId);

	y += comboBoxHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y, m_firstColumnWidth, spinHeight, spinOrderUpdateReasonCodeId);
	AddStaticIndirect("Reason Code", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth + 1, y + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateReasonCodeId);
/*
	const int itemHeight = 8;
	const unsigned int itemCount = 32;
	const int borderSize = 10;//2;
	const int execInstLbHeight = itemHeight * itemCount + borderSize;
*/
	y += spinHeight + vgap;
	const int lbHeight = m_height - y;
	const int execInstWidth = m_firstColumnWidth * 2;
	AddStaticIndirect("Execution Instructions", WS_VISIBLE|SS_NOPREFIX, 0, m_xc, y, execInstWidth, staticHeight, staticExecutionInstructionsId);
	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_OWNERDRAWFIXED|WS_VSCROLL|LBS_NOINTEGRALHEIGHT, 0, m_xc,
		y += staticHeight, execInstWidth, m_height - lbHeight, listBoxExecutionInstructionsId);

	m_xc += execInstWidth + 4 * hgap;
	y = messageTypeY;

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_SORT|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL|WS_GROUP, 0, m_xc, y, m_firstColumnHalfWidth, 400, comboBoxExecutionLiquidity1Id);
	AddStaticIndirect("Liquidity1", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnHalfWidth, y + (comboBoxHeight - staticHeight) / 2, m_firstColumnHalfWidth + 2, staticHeight, staticExecutionLiquidity1Id);
	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_SORT|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL|WS_GROUP, 0, m_xc, y += comboBoxHeight + vgap, m_firstColumnHalfWidth, 400, comboBoxExecutionLiquidity2Id);
	AddStaticIndirect("Liquidity2", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnHalfWidth, y + (comboBoxHeight - staticHeight) / 2, m_firstColumnHalfWidth + 2, staticHeight, staticExecutionLiquidity2Id);

	const int xLabel = m_xc + m_firstColumnWidth + 1;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y += comboBoxHeight + vgap, m_firstColumnWidth, spinHeight, spinExecutionFilledId);
	AddStaticIndirect("Filled", WS_VISIBLE|SS_NOPREFIX, 0, xLabel, y + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticExecutionFilledId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y += spinHeight + vgap, m_firstColumnWidth, spinHeight, spinExecutionCancelledId);
	AddStaticIndirect("Cancelled", WS_VISIBLE|SS_NOPREFIX, 0, xLabel, y + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticExecutionCancelledId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y += spinHeight + vgap, m_firstColumnWidth, spinHeight, spinExecutionRemainingId);
	AddStaticIndirect("Remaining", WS_VISIBLE|SS_NOPREFIX, 0, xLabel, y + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticExecutionRemainingId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += spinHeight + vgap, priceWidth, spinHeight, spinOrderUpdateExecutionPriceId);
	AddStaticIndirect("ExecPrice", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + priceWidth, y + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticOrderUpdateExecutionPriceId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_SORT|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL, 0, m_xc, y += spinHeight + vgap, m_firstColumnWidth, 400, comboBoxOrderUpdateExecutionDestinationId);
	AddStaticIndirect("ExecVenue", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, y + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateExecutionDestinationId);

	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL|ES_UPPERCASE, 0, m_xc, y += comboBoxHeight + vgap, m_firstColumnWidth, editHeight, editOrderUpdateExecutionMmidId);
	AddStaticIndirect("Exec MMID", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, y + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticOrderUpdateExecutionMmidId);

	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL|ES_UPPERCASE, 0, m_xc, y += editHeight + vgap, priceWidth, editHeight, editOrderUpdateExecutionRefIdId);
	AddStaticIndirect("Exec RefId", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + priceWidth, y + (editHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticOrderUpdateExecutionRefIdId);

	AddStaticIndirect("Execution Report Flags", WS_VISIBLE|SS_NOPREFIX, 0, m_xc, y += comboBoxHeight + vgap, execInstWidth, staticHeight, staticExecutionReportFlagsId);
	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_OWNERDRAWFIXED|WS_VSCROLL|LBS_NOINTEGRALHEIGHT, 0, m_xc,
		y += staticHeight, execInstWidth, m_height - lbHeight, listBoxExecutionReportFlagsId);

	AddResizeControl(new VerticalResizeControl(this, listBoxExecutionInstructionsId));
	AddListBox(&m_ListBoxExecutionInstructions);
	AddResizeControl(new VerticalResizeControl(this, listBoxExecutionReportFlagsId));
	AddListBox(&m_ListBoxExecutionReportFlags);
}

unsigned int SampleDialogSettingSimulateOrderUpdate::GetReserveQuantity() const
{
	return m_SpinReserveSize.GetValue();
}

/*
void SampleDialogSettingSimulateOrderUpdate::SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000, true);
}

void SampleDialogSettingSimulateOrderUpdate::SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}
*/
/*
void SampleDialogSettingSimulateOrderUpdate::SetMoneySpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(100000000, true);
	SetPriceSpinCells(spin);
}
*/

void SampleDialogSettingSimulateOrderUpdate::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingSimulateAccountOption::ToolTipsEnabled(enable);

	m_SpinSize.EnableTooltips(enable, m_toolTip);
	m_SpinRemaining.EnableTooltips(enable, m_toolTip);
	m_SpinPrice.EnableTooltips(enable, m_toolTip);
	m_SpinBid.EnableTooltips(enable, m_toolTip);
	m_SpinAsk.EnableTooltips(enable, m_toolTip);
	m_SpinLast.EnableTooltips(enable, m_toolTip);

	m_SpinClientId.EnableTooltips(enable, m_toolTip);
	m_SpinServerId.EnableTooltips(enable, m_toolTip);

	m_ComboBoxDestination.EnableTooltips(enable, m_toolTip);
	m_ComboBoxRouting.EnableTooltips(enable, m_toolTip);

	m_EditMmid.EnableTooltips(enable, m_toolTip);
	m_EditMarketRefId.EnableTooltips(enable, m_toolTip);

	m_CheckBoxHistorical.EnableTooltips(enable, m_toolTip);
	m_CheckBoxMarket.EnableTooltips(enable, m_toolTip);
//	m_CheckBoxBorrowAccept.EnableTooltips(enable, m_toolTip);
	m_ComboBoxMessageType.EnableTooltips(enable, m_toolTip);

	m_ListBoxExecutionInstructions.EnableTooltips(enable, m_toolTip);

	m_ComboBoxExecutionLiquidity1.EnableTooltips(enable, m_toolTip);
	m_ComboBoxExecutionLiquidity2.EnableTooltips(enable, m_toolTip);
	m_ListBoxExecutionReportFlags.EnableTooltips(enable, m_toolTip);

	m_SpinExecutionFilled.EnableTooltips(enable, m_toolTip);
	m_SpinExecutionCancelled.EnableTooltips(enable, m_toolTip);
	m_SpinExecutionRemaining.EnableTooltips(enable, m_toolTip);
	m_SpinExecutionPrice.EnableTooltips(enable, m_toolTip);
	m_ComboBoxExecutionDestination.EnableTooltips(enable, m_toolTip);
	m_EditExecutionMmid.EnableTooltips(enable, m_toolTip);
	m_EditExecutionRefId.EnableTooltips(enable, m_toolTip);
}

unsigned int SampleDialogSettingSimulateOrderUpdate::GetTifMilliseconds() const
{
	return m_SpinTif.GetMillisecond();
}

unsigned char SampleDialogSettingSimulateOrderUpdate::GetTifType() const
{
	const int sel = m_ComboBoxTifType.GetCurSel();
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
//			return TIF_DAY;

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
	return 0xFF;
}

void SampleDialogSettingSimulateOrderUpdate::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateAccountOption::DoDataExchange(pDX);

	DDX_Control(pDX, spinOrderUpdateSizeId, m_SpinSize);
	DDX_Control(pDX, spinOrderUpdateRemainingId, m_SpinRemaining);
	DDX_Control(pDX, spinOrderUpdateClientIdId, m_SpinClientId);
	DDX_Control(pDX, spinOrderUpdateServerIdId, m_SpinServerId);
	DDX_Control(pDX, spinOrderUpdatePriceId, m_SpinPrice);
	DDX_Control(pDX, spinOrderUpdateBidId, m_SpinBid);
	DDX_Control(pDX, spinOrderUpdateAskId, m_SpinAsk);
	DDX_Control(pDX, spinOrderUpdateLastId, m_SpinLast);
	DDX_Control(pDX, comboBoxOrderUpdateSideId, m_ComboBoxSide);
	DDX_Control(pDX, comboBoxOrderUpdateReRouteId, m_ComboBoxReRoute);

	DDX_Control(pDX, comboBoxOrderUpdateDestinationId, m_ComboBoxDestination);
	DDX_Control(pDX, comboBoxOrderUpdateRoutingId, m_ComboBoxRouting);
	DDX_Control(pDX, editOrderUpdateMmidId, m_EditMmid);
	DDX_Control(pDX, spinOrderUpdateReserveSizeId, m_SpinReserveSize);
	DDX_Control(pDX, staticOrderUpdateReserveSizeId, m_StaticReserveSize);

	DDX_Control(pDX, comboBoxOrderUpdateTifId, m_ComboBoxTifType);
	DDX_Control(pDX, spinOrderUpdateMillisecondId, m_SpinTif);

	DDX_Control(pDX, editOrderUpdateMarketRefIdId, m_EditMarketRefId);

	DDX_Control(pDX, checkBoxOrderUpdateHistoricalId, m_CheckBoxHistorical);
//	DDX_Control(pDX, checkBoxOrderUpdateReroutedToBamlId, m_CheckBoxReroutedToBaml);
	DDX_Control(pDX, checkBoxOrderUpdateMarketId, m_CheckBoxMarket);

//	DDX_Control(pDX, checkBoxBorrowAcceptId, m_CheckBoxBorrowAccept);
	DDX_Control(pDX, comboBoxMessageTypeId, m_ComboBoxMessageType);
	DDX_Control(pDX, spinOrderUpdateReasonCodeId, m_SpinReasonCode);

	DDX_Control(pDX, listBoxExecutionInstructionsId, m_ListBoxExecutionInstructions);

	DDX_Control(pDX, comboBoxExecutionLiquidity1Id, m_ComboBoxExecutionLiquidity1);
	DDX_Control(pDX, comboBoxExecutionLiquidity2Id, m_ComboBoxExecutionLiquidity2);

	DDX_Control(pDX, spinExecutionFilledId, m_SpinExecutionFilled);
	DDX_Control(pDX, spinExecutionCancelledId, m_SpinExecutionCancelled);
	DDX_Control(pDX, spinExecutionRemainingId, m_SpinExecutionRemaining);
	DDX_Control(pDX, spinOrderUpdateExecutionPriceId, m_SpinExecutionPrice);
	DDX_Control(pDX, comboBoxOrderUpdateExecutionDestinationId, m_ComboBoxExecutionDestination);
	DDX_Control(pDX, editOrderUpdateExecutionMmidId, m_EditExecutionMmid);
	DDX_Control(pDX, editOrderUpdateExecutionRefIdId, m_EditExecutionRefId);

	DDX_Control(pDX, listBoxExecutionReportFlagsId, m_ListBoxExecutionReportFlags);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateOrderUpdate, BaseDialogSettingSimulateAccountOption)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(checkBoxOrderUpdateHistoricalId, OnHistorical)
//	ON_BN_CLICKED(checkBoxOrderUpdateReroutedToBamlId, OnReroutedToBaml)
	ON_BN_CLICKED(checkBoxOrderUpdateMarketId, OnMarket)
//	ON_BN_CLICKED(checkBoxBorrowAcceptId, OnBorrowAccept)
	ON_CBN_SELCHANGE(comboBoxMessageTypeId, OnSelchangeMessageType)
	ON_CBN_SELCHANGE(comboBoxOrderUpdateSideId, OnSelchangeSide)
	ON_CBN_SELCHANGE(comboBoxOrderUpdateReRouteId, OnSelchangeReRoute)
	ON_CBN_SELCHANGE(comboBoxOrderUpdateDestinationId, OnSelchangeDestination)
	ON_CBN_SELCHANGE(comboBoxOrderUpdateRoutingId, OnSelchangeRouting)
	ON_CBN_SELCHANGE(comboBoxOrderUpdateTifId, OnSelchangeTif)
	ON_EN_CHANGE(editOrderUpdateMmidId, OnMmidChange)
	ON_EN_UPDATE(editOrderUpdateMmidId, OnMmidUpdate)
	ON_EN_CHANGE(editOrderUpdateMarketRefIdId, OnMarketRefIdChange)
	ON_EN_UPDATE(editOrderUpdateMarketRefIdId, OnMarketRefIdUpdate)
	ON_CBN_SELCHANGE(comboBoxExecutionLiquidity1Id, OnSelchangeLiquidity1)
	ON_CBN_SELCHANGE(comboBoxExecutionLiquidity2Id, OnSelchangeLiquidity2)
	ON_CBN_SELCHANGE(comboBoxOrderUpdateExecutionDestinationId, OnSelchangeExecutionDestination)
	ON_EN_CHANGE(editOrderUpdateExecutionMmidId, OnExecutionMmidChange)
	ON_EN_UPDATE(editOrderUpdateExecutionMmidId, OnExecutionMmidUpdate)
	ON_EN_CHANGE(editOrderUpdateExecutionRefIdId, OnExecutionRefIdChange)
	ON_EN_UPDATE(editOrderUpdateExecutionRefIdId, OnExecutionRefIdUpdate)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateOrderUpdate::LiquiditySelected(ComboBoxChar& comboBox, char& liquidity)
{
	const int sel = comboBox.GetCurSel();
	const char liq = sel >= 0 ? (char)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		comboBox.GetItemDataPtr(sel) : '\0';
	if(liq != liquidity)
	{
		liquidity = liq;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnSelchangeLiquidity1()
{
	LiquiditySelected(m_ComboBoxExecutionLiquidity1, m_liquidity1);
}

void SampleDialogSettingSimulateOrderUpdate::OnSelchangeLiquidity2()
{
	LiquiditySelected(m_ComboBoxExecutionLiquidity2, m_liquidity2);
}

void SampleDialogSettingSimulateOrderUpdate::UpdateTifType()
{
	m_SpinTif.SetBkColorEnabled(GetTifType() == TIF_GTD
//		&& !m_borrowAccept ?
		&& !m_messageType ?
		GetSysColor(COLOR_WINDOW):
		m_visualBase->GetGrayColor());//m_SpinTif.GetBkColorDisabled());//GetSysColor(COLOR_BTNFACE));
}

void SampleDialogSettingSimulateOrderUpdate::OnSelchangeTif()
{
	UpdateTifType();
}

void SampleDialogSettingSimulateOrderUpdate::OnMmidUpdate()
{
	m_EditMmid.TryUpdate();
}

void SampleDialogSettingSimulateOrderUpdate::OnMmidChange()
{
	CString mmid;
	m_EditMmid.GetWindowText(mmid);
	if(strcmp(mmid, m_mmid))
	{
		strncpy_s(m_mmid, sizeof(m_mmid), mmid, sizeof(unsigned int));
		m_modified = true;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnExecutionMmidUpdate()
{
	m_EditExecutionMmid.TryUpdate();
}

void SampleDialogSettingSimulateOrderUpdate::OnExecutionMmidChange()
{
	CString mmid;
	m_EditExecutionMmid.GetWindowText(mmid);
	if(strcmp(mmid, m_executionMmid))
	{
		strncpy_s(m_executionMmid, sizeof(m_executionMmid), mmid, sizeof(unsigned int));
		m_modified = true;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnMarketRefIdUpdate()
{
	m_EditMarketRefId.TryUpdate();
}

void SampleDialogSettingSimulateOrderUpdate::OnMarketRefIdChange()
{
	CString marketRefId;
	m_EditMarketRefId.GetWindowText(marketRefId);
	if(strcmp(marketRefId, m_marketReferenceId))
	{
		strncpy_s(m_marketReferenceId, sizeof(m_marketReferenceId), marketRefId, sizeof(m_marketReferenceId) - 1);
		m_modified = true;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnExecutionRefIdUpdate()
{
	m_EditExecutionRefId.TryUpdate();
}

void SampleDialogSettingSimulateOrderUpdate::OnExecutionRefIdChange()
{
	CString executionRefId;
	m_EditExecutionRefId.GetWindowText(executionRefId);
	if(strcmp(executionRefId, m_executionReferenceId))
	{
		strncpy_s(m_executionReferenceId, sizeof(m_executionReferenceId), executionRefId, sizeof(m_executionReferenceId) - 1);
		m_modified = true;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnSelchangeExecutionDestination()
{
//	const Destination* controlDestination;

//	bool ret = false;
	const int sel = m_ComboBoxExecutionDestination.GetCurSel();
	if(sel >= 0)
	{
		const Destination* dest = (const Destination*)m_ComboBoxExecutionDestination.GetItemDataPtr(sel);
		if(m_executionDestination != dest)
		{
			m_executionDestination = dest;
		}
	}
	else
	{
		m_executionDestination = nullptr;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnSelchangeDestination()
{
	const Destination* controlDestination;
//	if(UpdateDestinationRoutingStatic(m_currentDestination, m_currentDestination, m_ComboBoxDestination, m_ComboBoxRouting, &m_EditMmid))
	if(UpdateDestinationRoutingStatic(controlDestination, m_currentDestination, m_ComboBoxDestination, m_ComboBoxRouting, &m_EditMmid))
	{
//		m_ComboBoxDestination.SetInvalid(controlDestination != m_currentDestination);
		OnSelchangeRouting();

//		const int sel = m_ComboBoxDestination.GetCurSel();
//		const Destination* const dest = sel >= 0 ? (const Destination*)m_ComboBoxDestination.GetItemDataPtr(sel) : nullptr;
//		if(dest != m_currentDestination)
		if(controlDestination != m_currentDestination)
		{
			m_currentDestination = controlDestination;// dest;
			EnableSendMessageButton();
		}
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnSelchangeRouting()
{
	OnSelchangeRoutingStatic(m_ComboBoxRouting, m_SpinReserveSize, m_StaticReserveSize);
	m_modified = true;
}

/*
void SampleDialogSettingSimulateOrderUpdate::OnBorrowAccept()
{
	const bool borrowAccept = m_CheckBoxBorrowAccept.GetCheck() == BST_CHECKED;
	if(borrowAccept != m_borrowAccept)
	{
		m_borrowAccept = borrowAccept;
		UpdateSecurityPriceColor();
		UpdateTifType();
		m_ComboBoxTifType.SetInvalid(borrowAccept);
		m_ComboBoxRouting.SetInvalid(borrowAccept);
		m_ComboBoxReRoute.SetInvalid(borrowAccept);
		m_EditMmid.Invalidate(FALSE);
		m_EditMarketRefId.Invalidate(FALSE);
		m_modified = true;
	}
}
*/

void SampleDialogSettingSimulateOrderUpdate::OnHistorical()
{
	const bool historical = m_CheckBoxHistorical.GetCheck() == BST_CHECKED;
	if(historical != m_historical)
	{
		m_historical = historical;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnMarket()
{
	const bool market = m_CheckBoxMarket.GetCheck() == BST_CHECKED;
	if(market != m_market)
	{
		m_market = market;
		m_SpinPrice.SetBkColorEnabled(m_market ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));
		m_modified = true;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnSelchangeSide()
{
//	const int sel = m_ComboBoxSide.GetCurSel();
	char side;
	switch(m_ComboBoxSide.GetCurSel())
	{
		case 1:
		side = 'S';
		break;

		case 2:
		side = 'T';
		break;

		default:
		side = 'B';
		break;
	}
	if(side != m_side)
	{
		m_side = side;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnSelchangeMessageType()
{
	unsigned char messageType = 0;
	const int sel = m_ComboBoxMessageType.GetCurSel();
	if(sel >= 0)
	{
		messageType = (unsigned char)sel;
		if(messageType >= OMT_Count)messageType = 0;
	}
	if(messageType != m_messageType)
	{
		m_messageType = messageType;

		UpdateSecurityPriceColor();
		UpdateTifType();
		const bool invalid = messageType != 0;
		m_ComboBoxTifType.SetInvalid(invalid);
		m_ComboBoxRouting.SetInvalid(invalid);
		m_ComboBoxReRoute.SetInvalid(invalid);
		m_ListBoxExecutionInstructions.SetInvalid(invalid);

		const bool exec = OMT_EXECUTE_ORDER == m_messageType;
		const COLORREF color = OMT_REJECT_ORDER > m_messageType || exec ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW);
		m_SpinReasonCode.SetBkColorEnabled(color);

		m_ComboBoxExecutionLiquidity1.SetInvalid(!exec);
		m_ComboBoxExecutionLiquidity2.SetInvalid(!exec);
		m_ListBoxExecutionReportFlags.SetInvalid(!exec);

		m_SpinExecutionFilled.SetBkColorEnabled(exec ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
		m_SpinExecutionCancelled.SetBkColorEnabled(exec ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
		m_SpinExecutionRemaining.SetBkColorEnabled(exec ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
		m_SpinExecutionPrice.SetBkColorEnabled(exec ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
		m_ComboBoxExecutionDestination.SetInvalid(!exec);

		m_EditMmid.Invalidate(FALSE);
		m_EditMarketRefId.Invalidate(FALSE);
		m_EditExecutionMmid.Invalidate(FALSE);
		m_EditExecutionRefId.Invalidate(FALSE);
		m_modified = true;

		m_modified = true;
	}
}

void SampleDialogSettingSimulateOrderUpdate::OnSelchangeReRoute()
{
	unsigned char reRoute = 0;
	const int sel = m_ComboBoxReRoute.GetCurSel();
	if(sel >= 0)
	{
		reRoute = (unsigned char)sel;
		if(reRoute >= RER_Count)reRoute = 0;
	}
	if(reRoute != m_reRoute)
	{
		m_reRoute = reRoute;
		m_modified = true;
	}
}

LRESULT SampleDialogSettingSimulateOrderUpdate::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinOrderUpdateSizeId:
		{
			const unsigned int size = m_SpinSize.GetValue();
			if(size != m_size)
			{
				if(size == 0)
				m_SpinSize.SetBkColorEnabled(m_visualBase->GetWarningColor());
				else if(m_size == 0)
				m_SpinSize.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));

				m_size = size;
				m_modified = true;
				EnableSendMessageButton();
			}
		}
		break;

		case spinExecutionFilledId:
		{
			const unsigned int size = m_SpinExecutionFilled.GetValue();
			if(size != m_executionFilled)
			{
//				if(size == 0)
//				m_SpinExecutionFilled.SetBkColorEnabled(m_visualBase->GetWarningColor());
//				else if(m_size == 0)
//				m_SpinExecutionFilled.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));

				m_executionFilled = size;
				m_modified = true;
//				EnableSendMessageButton();
			}
		}
		break;

		case spinExecutionCancelledId:
		{
			const unsigned int size = m_SpinExecutionCancelled.GetValue();
			if(size != m_executionCancelled)
			{
				m_executionCancelled = size;
				m_modified = true;
			}
		}
		break;

		case spinExecutionRemainingId:
		{
			const unsigned int size = m_SpinExecutionRemaining.GetValue();
			if(size != m_executionRemaining)
			{
				m_executionRemaining = size;
				m_modified = true;
			}
		}
		break;

		case spinOrderUpdateReasonCodeId:
		{
			const unsigned short code = m_SpinReasonCode.GetValue();
			if(code != m_reasonCode)
			{
				m_reasonCode = code;
				m_modified = true;
//				EnableSendMessageButton();
			}
		}
		break;

		case spinOrderUpdateRemainingId:
		{
			const unsigned int size = m_SpinRemaining.GetValue();
			if(size != m_remaining)
			{
				m_remaining = size;
				m_modified = true;
//				EnableSendMessageButton();
			}
		}
		break;

		case spinOrderUpdateClientIdId:
		{
			const unsigned int size = m_SpinClientId.GetValue();
			if(size != m_clientId)
			{
				if(size == 0)
				m_SpinClientId.SetBkColorEnabled(m_visualBase->GetWarningColor());
				else if(m_clientId == 0)
				m_SpinClientId.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));

				m_clientId = size;
				m_modified = true;
				EnableSendMessageButton();
			}
		}
		break;

		case spinOrderUpdateServerIdId:
		{
			const unsigned int size = m_SpinServerId.GetValue();
			if(size != m_serverId)
			{
				m_serverId = size;
				m_modified = true;
//				EnableSendMessageButton();
			}
		}
		break;

		case spinOrderUpdatePriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPrice, 10000);
			if(price != m_price)
			{
/*
				if(price.isZero())
				{
					m_SpinPrice.SetBkColorEnabled(m_visualBase->GetWarningColor());
				}
				else if(m_price.isZero())
				{
					m_SpinPrice.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
				}
*/
				m_price = price;
				m_modified = true;
//				EnableSendMessageButton();
			}
		}
		break;

		case spinOrderUpdateExecutionPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinExecutionPrice, 10000);
			if(price != m_executionPrice)
			{
				m_executionPrice = price;
				m_modified = true;
			}
		}
		break;

		case spinOrderUpdateBidId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinBid, 10000);
			if(price != m_bid)
			{
				m_bid = price;
				m_modified = true;
//				EnableSendMessageButton();
			}
		}
		break;

		case spinOrderUpdateAskId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinAsk, 10000);
			if(price != m_ask)
			{
				m_ask = price;
				m_modified = true;
//				EnableSendMessageButton();
			}
		}
		break;

		case spinOrderUpdateLastId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinLast, 10000);
			if(price != m_last)
			{
				m_last = price;
				m_modified = true;
//				EnableSendMessageButton();
			}
		}
		break;

		case spinOrderUpdateReserveSizeId:
		{
			const unsigned int size = m_SpinReserveSize.GetValue();
			if(size != m_reserveSize)
			{
				m_reserveSize = size;
				m_modified = true;
//				EnableSendMessageButton();
			}
		}
		break;

		case spinOrderUpdateMillisecondId:
		m_modified = true;
		break;

#ifndef TAKION_NO_OPTIONS
		default:
		BaseDialogSettingSimulateAccountOption::OnSpinChange(w, l);
		break;
#endif
	}
	return 0;
}

bool SampleDialogSettingSimulateOrderUpdate::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateAccountOption::isSendMessageButtonEnabled()
//		&& !m_price.isZero()
		&& m_currentDestination != nullptr
		&& m_size != 0
		&& m_clientId != 0;
}

const Message* SampleDialogSettingSimulateOrderUpdate::CreateMessage(WSABUF& wsaBuf)
{
//	if(m_borrowAccept)
	switch(m_messageType)
	{
		case OMT_BORROW_ACCEPT:
		FillOrderHeader(m_borrowAcceptRequest);
/*
#ifndef TAKION_NO_OPTIONS
		if(isOptionUsed())//m_optionMode)
		{
			OptionKey optionKey(m_strikePrice.GetCompactPrice(),
				m_optionCall,
				m_optionSuffix,
				m_expirationDay,
				m_expirationMonth,
				m_expirationYear);

			if(m_optionSuffix)U_CopyPlusOneCharAndPad(m_borrowAcceptRequest.Symbol, sizeof(m_borrowAcceptRequest.Symbol), m_symbol.c_str(), m_optionSuffix, '\0');
			else U_CopyAndPad(m_borrowAcceptRequest.Symbol, sizeof(m_borrowAcceptRequest.Symbol), m_symbol.c_str(), '\0');

			*(unsigned __int64*)m_borrowAcceptRequest.ConfirmationID = optionKey.CalculateOptionBlock();

			m_borrowAcceptRequest.InstrumentType = 1;
		}
		else
		{
#endif
			U_CopyAndPad(m_borrowAcceptRequest.Symbol, sizeof(m_borrowAcceptRequest.Symbol), m_symbol.c_str(), '\0');
			m_borrowAcceptRequest.InstrumentType = 0;
#ifndef TAKION_NO_OPTIONS
		}
#endif
	
		m_borrowAcceptRequest.m_time = TL_GetCurrentMillisecond();

		U_CopyAndPad(m_borrowAcceptRequest.AccountName, sizeof(m_borrowAcceptRequest.AccountName), m_accountId, '\0');

		m_borrowAcceptRequest.Quantity = m_size;//m_SpinSize.GetValue();
		m_borrowAcceptRequest.RemainingQuantity = m_remaining;
		m_borrowAcceptRequest.ClOrdID = m_clientId;
		m_borrowAcceptRequest.SrOrdID = m_serverId;

		switch(m_side)
		{
			case 'B':
			m_borrowAcceptRequest.Side = ORDER_SIDE_BUY;
			break;

			case 'S':
			m_borrowAcceptRequest.Side = ORDER_SIDE_SELL;
			break;

			default:
			m_borrowAcceptRequest.Side = ORDER_SIDE_SELL_SHORT;
			break;
		}

		m_borrowAcceptRequest.RealTimeOrRecovered = m_historical ? 'R': '\0';
*/
		m_borrowAcceptRequest.m_availableQuantity = m_remaining;

//		m_msg.Type = m_market ? ORDER_TYPE_MARKET : ORDER_TYPE_LIMIT;

		if(m_market)
		{
			m_borrowAcceptRequest.PriceDollar = m_borrowAcceptRequest.PriceFraction = 0;
		}
		else
		{
			m_borrowAcceptRequest.PriceDollar = m_price.GetDollars();
			m_borrowAcceptRequest.PriceFraction = m_price.GetDollarFraction();
		}


		return &m_borrowAcceptRequest;

		case OMT_UPDATE_ORDER:
//	}
//	else
//	{
		{
			FillOrderHeader(m_msg);
/*
#ifndef TAKION_NO_OPTIONS
		if(isOptionUsed())//m_optionMode)
		{
			OptionKey optionKey(m_strikePrice.GetCompactPrice(),
				m_optionCall,
				m_optionSuffix,
				m_expirationDay,
				m_expirationMonth,
				m_expirationYear);

			if(m_optionSuffix)U_CopyPlusOneCharAndPad(m_msg.Symbol, sizeof(m_msg.Symbol), m_symbol.c_str(), m_optionSuffix, '\0');
			else U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol), m_symbol.c_str(), '\0');

			*(unsigned __int64*)m_msg.ConfirmationID = optionKey.CalculateOptionBlock();

			m_msg.InstrumentType = ST_OPTION;
		}
		else
		{
#endif
			U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol), m_symbol.c_str(), '\0');
			m_msg.InstrumentType =
#ifndef TAKION_NO_OPTIONS
				TU_IsFutureSymbol(m_symbol.c_str()) ? ST_FUTURE:
#endif
				ST_STOCK;
#ifndef TAKION_NO_OPTIONS
		}
#endif
	
		m_msg.m_time = TL_GetCurrentMillisecond();

		U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName), m_accountId, '\0');

		m_msg.Quantity = m_size;//m_SpinSize.GetValue();
		m_msg.RemainingQuantity = m_remaining;
		m_msg.ClOrdID = m_clientId;
		m_msg.SrOrdID = m_serverId;

		switch(m_side)
		{
			case 'B':
			m_msg.Side = ORDER_SIDE_BUY;
			break;

			case 'S':
			m_msg.Side = ORDER_SIDE_SELL;
			break;

			default:
			m_msg.Side = ORDER_SIDE_SELL_SHORT;
			break;
		}

		m_msg.RealTimeOrRecovered = m_historical ? 'R': '\0';
*/
			switch(m_reRoute)
			{
				case RER_BAML:
				m_msg.DefaultRoutesHigh = (unsigned int)(m_currentDestination->GetId() >> 32);
				m_msg.DefaultRoutesLow = (unsigned int)(m_currentDestination->GetId() & 0xFFFFFFFFui64);
				m_msg.Route = BAML_ROUTE;
				m_msg.Status = OS_NEW | OS_REROUTED;
				break;

				case RER_VELO:
				m_msg.DefaultRoutesHigh = (unsigned int)(m_currentDestination->GetId() >> 32);
				m_msg.DefaultRoutesLow = (unsigned int)(m_currentDestination->GetId() & 0xFFFFFFFFui64);
				m_msg.Route = VELOCITY_ROUTE;
				m_msg.Status = OS_NEW | OS_REROUTED;
				break;

				default:
				m_msg.Route = m_currentDestination ? m_currentDestination->GetId() : 0;
				m_msg.DefaultRoutesHigh = m_msg.DefaultRoutesLow = 0;
				m_msg.Status = OS_NEW;
				break;
			}

			unsigned int minReserve = 0;
			unsigned int maxReserve = 0;
			const int sel = m_ComboBoxRouting.GetCurSel();
			if(sel < 0)
			{
				m_msg.RoutingInstructions = RI_NONE;
				m_msg.SubType = 0;
			}
			else
			{
				const DestinationRouting* routing = (const DestinationRouting*)m_ComboBoxRouting.GetItemDataPtr(sel);
				if(routing)
				{
					minReserve = routing->GetMinReserveQuantity();
					maxReserve = routing->GetMaxReserveQuantity();
					m_msg.RoutingInstructions = routing->GetId();
					m_msg.SubType = routing->GetSubType();
				}
				else
				{
					m_msg.RoutingInstructions = RI_NONE;
					m_msg.SubType = 0;
				}
			}

			if(minReserve < maxReserve)
			{
				unsigned int reserveSize = m_reserveSize;// m_SpinReserveSize.GetValue();
				if(reserveSize < minReserve)
				{
					reserveSize = minReserve;
				}
				else if(reserveSize > maxReserve)
				{
					reserveSize = maxReserve;
				}
				m_msg.ReserveQuantity = reserveSize;
			}
			else
			{
				m_msg.ReserveQuantity = 0;
			}

			m_msg.ExecInst = m_ListBoxExecutionInstructions.GetFilter();
			*(unsigned int*)m_msg.market_maker = U_RepresentStringAsUnsignedNumber<unsigned int>(m_mmid);//*(unsigned int*)"XXXX";//execution->GetMmid();

			U_CopyAndPad(m_msg.MarketReferenceID, sizeof(m_msg.MarketReferenceID) - 1, m_marketReferenceId, '\0', true);

			m_msg.TIF = GetTifType();
			m_msg.ExpireTime = GetTifMilliseconds();

			m_msg.Type = m_market ? ORDER_TYPE_MARKET : ORDER_TYPE_LIMIT;

			if(m_market)
			{
				m_msg.LimitPriceDollar = 
					m_msg.LimitPriceFraction = 
					m_msg.DisplayPriceDollar = 
					m_msg.DisplayPriceFraction = 0;
			}
			else
			{
				m_msg.LimitPriceDollar = m_msg.DisplayPriceDollar = m_price.GetDollars();
				m_msg.LimitPriceFraction = m_msg.DisplayPriceFraction = m_price.GetDollarFraction();
			}

			m_msg.BidPriceDollar = m_bid.GetDollars();
			m_msg.BidPriceFraction = m_bid.GetDollarFraction();

			m_msg.AskPriceDollar = m_ask.GetDollars();
			m_msg.AskPriceFraction = m_ask.GetDollarFraction();

			m_msg.LastPriceDollar = m_last.GetDollars();
			m_msg.LastPriceFraction = m_last.GetDollarFraction();

		}
		return &m_msg;

		case OMT_EXECUTE_ORDER:
		FillOrderHeader(m_msgOrderExecute);
		m_msgOrderExecute.LiquidityFlag = m_liquidity1;
		m_msgOrderExecute.BillableFlag = m_liquidity2;
		m_msgOrderExecute.ReportType = m_ListBoxExecutionReportFlags.GetFilter();
		m_msgOrderExecute.FillQuantity = m_executionFilled;
		m_msgOrderExecute.CancelQuantity = m_executionCancelled;
		m_msgOrderExecute.m_orderRemainingQuantity = m_executionRemaining;
		m_msgOrderExecute.FillPriceDollar = m_executionPrice.GetDollars();
		m_msgOrderExecute.FillPriceFraction = m_executionPrice.GetDollarFraction();
		*(unsigned int*)m_msgOrderExecute.MMID = U_RepresentStringAsUnsignedNumber<unsigned int>(m_executionMmid);
		U_CopyAndPad(m_msgOrderExecute.ExecutionReferenceID, sizeof(m_msgOrderExecute.ExecutionReferenceID) - 1, m_executionReferenceId, '\0', true);
		m_msgOrderExecute.Route = m_executionDestination ? m_executionDestination->GetId() : 0;
		return &m_msgOrderExecute;

		case OMT_REJECT_ORDER:
		FillReasonHeader(m_msgOrderReject);
		return &m_msgOrderReject;

		case OMT_REJECT_CANCEL:
		FillReasonHeader(m_msgCancelReject);
		return &m_msgCancelReject;

		case OMT_KILL_ORDER:
		FillReasonHeader(m_msgKillOrder);
		return &m_msgKillOrder;

		default:
		return nullptr;
	}
}

void SampleDialogSettingSimulateOrderUpdate::ResetValues()
{
	m_historical = false;
//	m_reroutedToBaml = false;
	m_reRoute = 0;
	m_messageType = 0;
	m_market = false;
//	m_borrowAccept = false;
	m_side = 'B';
	m_size = 0;
	m_reserveSize = 0;
	m_remaining = 0;
	m_clientId = 0;
	m_serverId = 0;
	m_reasonCode = 0;
	m_price.SetZero();
	m_bid.SetZero();
	m_ask.SetZero();
	m_last.SetZero();
	*(unsigned int*)m_mmid = 0;
	m_mmid[sizeof(unsigned int)] = '\0';
	U_CopyAndPad(m_marketReferenceId, sizeof(m_marketReferenceId) - 1, "", '\0', true);
	m_currentDestination = nullptr;
	m_liquidity1 = '\0';
	m_liquidity2 = '\0';
	m_executionFilled = 0;
	m_executionCancelled = 0;
	m_executionRemaining = 0;
	m_executionPrice.SetZero();
	*(unsigned int*)m_executionMmid = 0;
	m_executionMmid[sizeof(unsigned int)] = '\0';
	m_executionDestination = nullptr;
}

void SampleDialogSettingSimulateOrderUpdate::BeforeDestroy()
{
	ResetValues();
	BaseDialogSettingSimulateAccountOption::BeforeDestroy();
}


BOOL SampleDialogSettingSimulateOrderUpdate::OnInitDialog()
{
	ResetValues();

	m_ListBoxExecutionInstructions.SetFilter(0);
	m_ListBoxExecutionReportFlags.SetFilter(0);

	BOOL ret = BaseDialogSettingSimulateAccountOption::OnInitDialog();

	const COLORREF color = OMT_REJECT_ORDER > m_messageType || OMT_EXECUTE_ORDER == m_messageType ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW);
	m_SpinReasonCode.SetBkColorEnabled(color);

	m_ComboBoxExecutionLiquidity1.AddString(nullptr);
	m_ComboBoxExecutionLiquidity1.FillCharRange('A', 'Z', false);
	m_ComboBoxExecutionLiquidity1.FillCharRange('0', '9', false);
	m_ComboBoxExecutionLiquidity1.SetCurSel(0);
	m_ComboBoxExecutionLiquidity1.SetInvalid(true);

	m_ComboBoxExecutionLiquidity2.AddString(nullptr);
	m_ComboBoxExecutionLiquidity2.FillCharRange('A', 'Z', false);
	m_ComboBoxExecutionLiquidity2.FillCharRange('0', '9', false);
	m_ComboBoxExecutionLiquidity2.SetCurSel(0);
	m_ComboBoxExecutionLiquidity2.SetInvalid(true);

	m_CheckBoxHistorical.ResizeToFitText(0, false);
	m_CheckBoxMarket.ResizeToFitText(0, false);
//	m_CheckBoxBorrowAccept.ResizeToFitText(0, false);

	m_ComboBoxSide.AddString("Buy");
	m_ComboBoxSide.AddString("Sell");
	m_ComboBoxSide.AddString("Short");
	m_ComboBoxSide.SetCurSel(0);

	m_ComboBoxReRoute.AddString("");
	m_ComboBoxReRoute.AddString("BAML");
	m_ComboBoxReRoute.AddString("VELO");
	m_ComboBoxReRoute.SetCurSel(0);

	m_ComboBoxMessageType.AddString("UpdateOrder");
	m_ComboBoxMessageType.AddString("AcceptBorrow");
	m_ComboBoxMessageType.AddString("RejectOrder");
	m_ComboBoxMessageType.AddString("RejectCancel");
	m_ComboBoxMessageType.AddString("KillOrder");
	m_ComboBoxMessageType.AddString("ExecuteOrder");

	FillTifComboBox(m_ComboBoxTifType);

	m_EditMmid.SetLimitText(sizeof(unsigned int));
	m_EditMarketRefId.SetLimitText(sizeof(m_marketReferenceId) - 1);
	m_EditExecutionMmid.SetLimitText(sizeof(unsigned int));
	m_EditExecutionRefId.SetLimitText(sizeof(m_executionReferenceId) - 1);

	TakionIterator* iterator = TD_CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		m_ComboBoxDestination.AddString((const char*)destination);
		m_ComboBoxExecutionDestination.AddString((const char*)destination);
	}
	delete iterator;

	m_ComboBoxDestination.SetCurSel(0);
	OnSelchangeDestination();

	m_ComboBoxExecutionDestination.SetCurSel(0);
	OnSelchangeExecutionDestination();

	m_SpinTif.SetLeftmostMaxDigitValue(8);

	m_ComboBoxTifType.SetCurSel(0);
	OnSelchangeTif();

	m_ComboBoxMessageType.SetCurSel(0);
	OnSelchangeMessageType();

//	m_ComboBoxSide.SetInvalid(m_ref);

	m_SpinSize.SetBkColorEnabled(m_visualBase->GetWarningColor());
	m_SpinClientId.SetBkColorEnabled(m_visualBase->GetWarningColor());
//	m_SpinPrice.SetBkColorEnabled(m_visualBase->GetWarningColor());

	m_SpinExecutionFilled.SetBkColorEnabled(m_visualBase->GetGrayColor());
	m_SpinExecutionCancelled.SetBkColorEnabled(m_visualBase->GetGrayColor());
	m_SpinExecutionRemaining.SetBkColorEnabled(m_visualBase->GetGrayColor());

//	RestoreLayout();

	UpdateSpinColor();

	return ret;
}

void SampleDialogSettingSimulateOrderUpdate::UpdateSettings()
{
	extensionConfig->m_simOrderUpdateSymbol.SetValue(m_symbol);
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_simOrderUpdateOption.SetValue(m_optionMode);
	extensionConfig->m_simOrderUpdateOptionCall.SetValue(m_optionCall);
	extensionConfig->m_simOrderUpdateOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_simOrderUpdateOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_simOrderUpdateOptionExpiration.SetValue(GetExpirationDate());
#endif
	UpdateAccountInConfig(extensionConfig->m_simOrderUpdateAccountId);

	Price price;
//	Money money;

	extensionConfig->m_simOrderUpdateSize.SetValue(m_size);//m_SpinSize.GetValue());
	extensionConfig->m_simOrderUpdateRemaining.SetValue(m_remaining);
	extensionConfig->m_simOrderUpdateClientId.SetValue(m_clientId);
	extensionConfig->m_simOrderUpdateServerId.SetValue(m_serverId);
	extensionConfig->m_simOrderUpdateReasonCode.SetValue(m_reasonCode);
	extensionConfig->m_simOrderUpdatePrice.SetValue(m_price);
	extensionConfig->m_simOrderUpdateBid.SetValue(m_bid);
	extensionConfig->m_simOrderUpdateAsk.SetValue(m_ask);
	extensionConfig->m_simOrderUpdateLast.SetValue(m_last);
	extensionConfig->m_simOrderUpdateSide.SetValue(m_side);
	extensionConfig->m_simOrderUpdateReserveSize.SetValue(m_reserveSize);

	extensionConfig->m_simOrderUpdateVenue.SetValue(m_currentDestination ? m_currentDestination->GetMainNumericName() : 0);

	const int sel = m_ComboBoxRouting.GetCurSel();
	const DestinationRouting* routing = sel < 0 ? nullptr : (const DestinationRouting*)m_ComboBoxRouting.GetItemDataPtr(sel);
	extensionConfig->m_simOrderUpdateRouting.SetValue(routing ? routing->GetNumericName() : 0);

	extensionConfig->m_simOrderUpdateMmid.SetValue(U_RepresentStringAsUnsignedNumber<unsigned int>(m_mmid));

	extensionConfig->m_simOrderUpdateMarketReferenceId.SetStrValue(m_marketReferenceId);

	extensionConfig->m_simOrderUpdateHistorical.SetValue(m_historical);
//	extensionConfig->m_simOrderUpdateReroutedToBaml.SetValue(m_reroutedToBaml);
	extensionConfig->m_simOrderUpdateReRoute.SetValue(m_reRoute);
	extensionConfig->m_simOrderUpdateMarket.SetValue(m_market);

	extensionConfig->m_simOrderUpdateMessageType.SetValue(m_messageType);

//	extensionConfig->m_simOrderUpdateBorrowAccept.SetValue(m_borrowAccept);

	extensionConfig->m_simOrderUpdateExecutionInstructions.SetValue(m_ListBoxExecutionInstructions.GetFilter());

	extensionConfig->m_simOrderUpdateLiquidity1.SetValue(m_liquidity1);
	extensionConfig->m_simOrderUpdateLiquidity2.SetValue(m_liquidity2);

	extensionConfig->m_simOrderUpdateExecutionFilled.SetValue(m_executionFilled);
	extensionConfig->m_simOrderUpdateExecutionCancelled.SetValue(m_executionCancelled);
	extensionConfig->m_simOrderUpdateExecutionRemaining.SetValue(m_executionRemaining);
	extensionConfig->m_simOrderUpdateExecutionPrice.SetValue(m_executionPrice);

	extensionConfig->m_simOrderUpdateExecutionVenue.SetValue(m_executionDestination ? m_executionDestination->GetMainNumericName() : 0);
	extensionConfig->m_simOrderUpdateExecutionMmid.SetValue(U_RepresentStringAsUnsignedNumber<unsigned int>(m_executionMmid));
	extensionConfig->m_simOrderUpdateExecutionReferenceId.SetStrValue(m_executionReferenceId);

	extensionConfig->m_simOrderUpdateExecutionReportFlags.SetValue(m_ListBoxExecutionReportFlags.GetFilter());
}

void SampleDialogSettingSimulateOrderUpdate::RestoreUInt(const unsigned int& src, unsigned int& dst, UnsignedNumberTypeSpin<unsigned int>& spin)
{
	if(src != dst)
	{
		dst = src;
		FillSpinControlFromUInt(src, spin, false);
	}
}

void SampleDialogSettingSimulateOrderUpdate::RestoreUShort(const unsigned short& src, unsigned short& dst, UnsignedNumberTypeSpin<unsigned short>& spin)
{
	if(src != dst)
	{
		dst = src;
		FillSpinControlFromUShort(src, spin, false);
	}
}

void SampleDialogSettingSimulateOrderUpdate::RestorePrice(const Price& src, Price& dst, UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	if(src != dst)
	{
		dst = src;
		FillSpinControlFromPrice(src, spin, 10000, false);
	}
}

void SampleDialogSettingSimulateOrderUpdate::RestoreLayout()
{
	m_EditSymbol.SetWindowText(extensionConfig->m_simOrderUpdateSymbol.GetValue().c_str());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_simOrderUpdateOption.GetValue(),
		extensionConfig->m_simOrderUpdateOptionSuffix.GetValue(),
		extensionConfig->m_simOrderUpdateOptionCall.GetValue(),
		extensionConfig->m_simOrderUpdateOptionStrikePrice.GetValue(),
		extensionConfig->m_simOrderUpdateOptionExpiration.GetValue());
#endif
	SetAccountById(extensionConfig->m_simOrderUpdateAccountId.GetValue().c_str());

	FillSpinControlFromUInt(extensionConfig->m_simOrderUpdateSize.GetValue(), m_SpinSize, false);
	FillSpinControlFromUInt(extensionConfig->m_simOrderUpdateRemaining.GetValue(), m_SpinRemaining, false);
	FillSpinControlFromUInt(extensionConfig->m_simOrderUpdateClientId.GetValue(), m_SpinClientId, false);
	FillSpinControlFromUInt(extensionConfig->m_simOrderUpdateServerId.GetValue(), m_SpinServerId, false);
	FillSpinControlFromPrice(extensionConfig->m_simOrderUpdatePrice.GetValue(), m_SpinPrice, 10000, false);
	FillSpinControlFromPrice(extensionConfig->m_simOrderUpdateBid.GetValue(), m_SpinBid, 10000, false);
	FillSpinControlFromPrice(extensionConfig->m_simOrderUpdateAsk.GetValue(), m_SpinAsk, 10000, false);
	FillSpinControlFromPrice(extensionConfig->m_simOrderUpdateLast.GetValue(), m_SpinLast, 10000, false);
	FillSpinControlFromUInt(extensionConfig->m_simOrderUpdateReserveSize.GetValue(), m_SpinReserveSize, false);
	FillSpinControlFromUShort(extensionConfig->m_simOrderUpdateReasonCode.GetValue(), m_SpinReasonCode, false);
/*
	const bool borrowAccept = extensionConfig->m_simOrderUpdateBorrowAccept.GetValue();
	if(borrowAccept != m_borrowAccept)
	{
		m_CheckBoxBorrowAccept.SetCheck(borrowAccept ? BST_CHECKED : BST_UNCHECKED);
		m_borrowAccept = borrowAccept;
	}
*/
	unsigned char messageType = extensionConfig->m_simOrderUpdateMessageType.GetValue();
	if(messageType != m_messageType)
	{
//		m_messageType = messageType;
		if(messageType >= OMT_Count)messageType = 0;
		m_ComboBoxMessageType.SetCurSel(messageType);
		OnSelchangeMessageType();
	}

	const bool historical = extensionConfig->m_simOrderUpdateHistorical.GetValue();
	if(historical != m_historical)
	{
		m_CheckBoxHistorical.SetCheck(historical ? BST_CHECKED : BST_UNCHECKED);
		m_historical = historical;
	}

	const unsigned char reRoute = extensionConfig->m_simOrderUpdateReRoute.GetValue();
	if(reRoute != m_reRoute)
	{
		m_reRoute = reRoute;
		if(m_reRoute >= RER_Count)m_reRoute = 0;
		m_ComboBoxReRoute.SetCurSel(m_reRoute);
	}

	const bool market = extensionConfig->m_simOrderUpdateMarket.GetValue();
	if(market != m_market)
	{
		m_CheckBoxMarket.SetCheck(market ? BST_CHECKED : BST_UNCHECKED);
		OnMarket();
//		m_market = market;
	}

	RestoreUInt(extensionConfig->m_simOrderUpdateSize.GetValue(), m_size, m_SpinSize);
	RestoreUInt(extensionConfig->m_simOrderUpdateReserveSize.GetValue(), m_reserveSize, m_SpinReserveSize);

	RestoreUInt(extensionConfig->m_simOrderUpdateRemaining.GetValue(), m_remaining, m_SpinRemaining);
	RestoreUInt(extensionConfig->m_simOrderUpdateClientId.GetValue(), m_clientId, m_SpinClientId);
	RestoreUInt(extensionConfig->m_simOrderUpdateServerId.GetValue(), m_serverId, m_SpinServerId);
	RestoreUShort(extensionConfig->m_simOrderUpdateReasonCode.GetValue(), m_reasonCode, m_SpinReasonCode);

	RestorePrice(extensionConfig->m_simOrderUpdatePrice.GetValue(), m_price, m_SpinPrice);

	RestorePrice(extensionConfig->m_simOrderUpdateBid.GetValue(), m_bid, m_SpinBid);
	RestorePrice(extensionConfig->m_simOrderUpdateAsk.GetValue(), m_ask, m_SpinAsk);
	RestorePrice(extensionConfig->m_simOrderUpdateLast.GetValue(), m_last, m_SpinLast);

	RestoreUInt(extensionConfig->m_simOrderUpdateExecutionFilled.GetValue(), m_executionFilled, m_SpinExecutionFilled);
	RestoreUInt(extensionConfig->m_simOrderUpdateExecutionCancelled.GetValue(), m_executionCancelled, m_SpinExecutionCancelled);
	RestoreUInt(extensionConfig->m_simOrderUpdateExecutionRemaining.GetValue(), m_executionRemaining, m_SpinExecutionRemaining);
	RestorePrice(extensionConfig->m_simOrderUpdateExecutionPrice.GetValue(), m_executionPrice, m_SpinExecutionPrice);

	UpdateSpinColor();

	const char side = extensionConfig->m_simOrderUpdateSide.GetValue();
	if(side != m_side)
	{
		switch(side)
		{
			case 'B':
			m_side = side;
			m_ComboBoxSide.SetCurSel(0);
			break;

			case 'S':
			m_side = side;
			m_ComboBoxSide.SetCurSel(1);
			break;

			case 'T':
			m_side = side;
			m_ComboBoxSide.SetCurSel(2);
			break;

			default:
			if('B' != m_side)
			{
				m_side = 'B';
				m_ComboBoxSide.SetCurSel(0);
			}
			break;
		}
	}

//
	char buf[sizeof(unsigned int) + 1];
	buf[sizeof(unsigned int)] = '\0';

	unsigned int mmid = extensionConfig->m_simOrderUpdateMmid.GetValue();
	unsigned int oldMmid = U_RepresentStringAsUnsignedNumber<unsigned int>(m_mmid);
	if(oldMmid != mmid)
	{
		*(unsigned int*)buf = mmid;
		m_EditMmid.SetWindowText(buf);
	}

	const std::string& marketRefId = extensionConfig->m_simOrderUpdateMarketReferenceId.GetValue();
	if(strcmp(marketRefId.c_str(), m_marketReferenceId))
	{
		m_EditMarketRefId.SetWindowText(marketRefId.c_str());
	}

	int found;
	unsigned int venueNumericName = extensionConfig->m_simOrderUpdateVenue.GetValue();
	if(venueNumericName != (m_currentDestination ? m_currentDestination->GetMainNumericName() : 0))
	{
		found = m_ComboBoxDestination.FindItemByNumericName(venueNumericName);
		if(found >= 0)
		{
			m_ComboBoxDestination.SetCurSel(found);
			OnSelchangeDestination();
		}
		else if(m_ComboBoxDestination.GetCurSel() != 0)
		{
			m_ComboBoxDestination.SetCurSel(0);
			OnSelchangeDestination();
		}
	}
//
	const int sel = m_ComboBoxRouting.FindItemByNumericName(extensionConfig->m_simOrderUpdateRouting.GetValue());
	m_ComboBoxRouting.SetCurSel(sel < 0 ? 0 : sel);
	OnSelchangeRouting();

	m_ListBoxExecutionInstructions.SetFilter(extensionConfig->m_simOrderUpdateExecutionInstructions.GetValue());

	char liquidity = extensionConfig->m_simOrderUpdateLiquidity1.GetValue();
	found = m_ComboBoxExecutionLiquidity1.FindItem((const void*)liquidity);//extensionConfig->m_mdOptionSuffix.GetValue());
	m_ComboBoxExecutionLiquidity1.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeLiquidity1();
	liquidity = extensionConfig->m_simOrderUpdateLiquidity2.GetValue();
	found = m_ComboBoxExecutionLiquidity2.FindItem((const void*)liquidity);//extensionConfig->m_mdOptionSuffix.GetValue());
	m_ComboBoxExecutionLiquidity2.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeLiquidity2();
	mmid = extensionConfig->m_simOrderUpdateExecutionMmid.GetValue();
	oldMmid = U_RepresentStringAsUnsignedNumber<unsigned int>(m_executionMmid);
	if(oldMmid != mmid)
	{
		*(unsigned int*)buf = mmid;
		m_EditExecutionMmid.SetWindowText(buf);
	}
	const std::string& executionRefId = extensionConfig->m_simOrderUpdateExecutionReferenceId.GetValue();
	if(strcmp(executionRefId.c_str(), m_executionReferenceId))
	{
		m_EditExecutionRefId.SetWindowText(executionRefId.c_str());
	}

	venueNumericName = extensionConfig->m_simOrderUpdateExecutionVenue.GetValue();
	if(venueNumericName != (m_executionDestination ? m_executionDestination->GetMainNumericName() : 0))
	{
		found = m_ComboBoxExecutionDestination.FindItemByNumericName(venueNumericName);
		if(found >= 0)
		{
			m_ComboBoxExecutionDestination.SetCurSel(found);
			OnSelchangeExecutionDestination();
		}
		else if(m_ComboBoxExecutionDestination.GetCurSel() != 0)
		{
			m_ComboBoxExecutionDestination.SetCurSel(0);
			OnSelchangeExecutionDestination();
		}
	}

	m_ListBoxExecutionReportFlags.SetFilter(extensionConfig->m_simOrderUpdateExecutionReportFlags.GetValue());

	EnableSendMessageButton();

	m_modified = false;
}

void SampleDialogSettingSimulateOrderUpdate::UpdateSecurityPriceColor()
{
//	const COLORREF color = m_borrowAccept ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW);
	const COLORREF color = m_messageType ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW);
	m_SpinBid.SetBkColorEnabled(color);
	m_SpinAsk.SetBkColorEnabled(color);
	m_SpinLast.SetBkColorEnabled(color);
}

void SampleDialogSettingSimulateOrderUpdate::UpdateSpinColor()
{
	m_SpinSize.SetBkColorEnabled(m_size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
	m_SpinClientId.SetBkColorEnabled(m_clientId ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
//	m_SpinPrice.SetBkColorEnabled(m_price.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
	m_SpinPrice.SetBkColorEnabled(m_market ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));
	UpdateSecurityPriceColor();
}

HBRUSH SampleDialogSettingSimulateOrderUpdate::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editOrderUpdateMmidId:
		case editOrderUpdateMarketRefIdId:
//		if(m_borrowAccept)
		if(OMT_BORROW_ACCEPT == m_messageType)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		return nullptr;

		case editOrderUpdateExecutionMmidId:
		case editOrderUpdateExecutionRefIdId:
		if(OMT_EXECUTE_ORDER != m_messageType)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		return nullptr;
	}
	return BaseDialogSettingSimulateAccountOption::OnCtlColor(pDC, pWnd, nCtlColor);
}
