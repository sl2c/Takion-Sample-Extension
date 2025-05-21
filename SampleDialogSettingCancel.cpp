#include "stdafx.h"
#include "SampleDialogSettingCancel.h"
#include "ExtensionDerivedConfig.h"
#include "MainDialog.h"

extern MainDialog* mainDialog;
extern ExtensionDerivedConfig* extensionConfig;

static const char* const cancelOrderText = "Cancel Order";
static const char* const replaceOrderText = "Replace Order";
static const char* const cancelOrdersText = "Cancel Orders";
static const char* const replaceOrdersText = "Replace Orders";

SampleDialogSettingCancel::SampleDialogSettingCancel(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab) :
	BaseSampleDialogSettingOption(mainWnd, parentTab, true, "Cancel (Replace)", 66, buttonHeight + vgap),
	m_SpinClientId(0xFFFFFFFF, 0, 0, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Client ID of the Order to be cancelled (replaced)"),
	m_CheckBoxClientId("Cancel (Replace) Order with Client ID specified in the spin to the left", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_RadioAccountCurrent("Orders in the Current Account", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_RadioAccountAll("Orders in All Accounts", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_RadioPositionCurrent("Orders in the Position specified in Symbol and Option Controls", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_RadioPositionEach("Order Pick is done in each Position separately", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_RadioPositionAll("Order Pick is done among all Positions", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

#ifndef TAKION_NO_OPTIONS
	m_CheckBoxInstrumentStock("Stock Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxInstrumentOption("Option Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxInstrumentFuture("Future Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
#endif

	m_CheckBoxSideBuy("Buy Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset, false, nullptr, false, nullptr, false),
	m_CheckBoxSideSell("Sell Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset, false, nullptr, false, nullptr, false),
	m_CheckBoxSideShort("Short Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset, false, nullptr, false, nullptr, false),

	m_CheckBoxDirectionClose("Orders that close Positions (in full, partially or flipping)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxDirectionOpen("Orders that Open a Position", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxDirectionAdd("Orders that add to an existing Position", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_ListBoxOrder(false),

	m_CheckBoxTifOnOpen("Orders with TIF=OnOpen", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTifOnClose("Orders with TIF=OnClose", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTifDay("Orders with TIF=Day", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTifExtended("Orders with TIF=ExtendedDay (extending after Market Close)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTifTime("Orders with TIF=Time (dies when the Time expires)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTifIoc("Orders with TIF=IOC", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTifFok("Orders with TIF=FOK", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTifGtc("Orders with TIF=Good Till Cancelled", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTifGtt("Orders with TIF=Good Till Time", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTifExtendedTradingClose("Orders with TIF=ETC (Extended Trading Close)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_CheckBoxOrderTypeAlgo("Order Algorithms (like SPDR)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxOrderTypeLimit("Limit Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxOrderTypeMarket("Market Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxOrderTypeStop("Stop Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_CheckBoxPqOpenYes("Orders in Stocks that have the Primary Exchange Open Price", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxPqOpenNo("Orders in Stocks that don't have the Primary Exchange Open Price", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_CheckBoxTCloseYes("Orders in Stocks that have Today's Close Price", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxTCloseNo("Orders in Stocks that don't have Today's Close Price", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_CheckBoxExcludeDestinations("If checked, exclude Venues checked below; Otherwise cancel only Venues checked below", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_ListBoxDestination(1, true, true),

	m_CheckBoxExcludeMnemonics("If checked, exclude Mnemonics checked below; Otherwise cancel only Mnemonics checked below (Empty Mnemonic means Orders with no Mnemonic)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_ListBoxMnemonics(true, true, false, true, 0, true, true, false, nullptr),
	m_EditMnemonic("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", false),
//Replace
	m_groupBoxReplaceOrder(RangeGroupBase::groupBoxLeftMargin, RangeGroupBase::groupBoxTopMargin, RangeGroupBase::groupBoxRightMargin, 3 * RangeGroupBase::groupBoxBottomMargin),
	m_CheckBoxReplaceOrder("Replace Order with a different Price or Size", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxReplaceNative("Use a native Replace. Otherwise Algorithm \"Replacer\" is created which sends a request to cancel the Order(s), waits for the Cancel to confirm and sends the new Order(s)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxMarketOrder("Replace with Market Order", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_SpinReplacePriceOffset(10000000, -10000000, 0, 1000, 3, TakionSpin::charComma, 4),
	m_CheckBoxReplacePriceOffsetPercent("Price offset as percentage of the original Order Price", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxReplacePriceEnforceMinOffset("If Price Offset is not 0, enforce the price offset to be at least $0.01 for price over $1.00 and $0.0001 for price below $1.00", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxReplacePriceReverseForSell("Reverse Price Offset for Sell and Short Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_SpinReplaceSize(TD_GetOrderSizeAbsoluteCap(), -(int)TD_GetOrderSizeAbsoluteCap(), 0, 1000, 3, TakionSpin::charComma),
	m_CheckBoxReplaceSizeOffsetPercent("Size offset as percentage of the original Order Size", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxReplaceSizeReverseForSell("Reverse Size Offset for Sell and Short Orders", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_CheckBoxReplaceAdjustable("Display a Dialog to adjust replacing Order Values", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxReplaceAdjustableUseMmBox("Use MmBox to adjust replacing Order Values if there is only one Order to replace", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_CheckBoxPreBorrow("Borrow Stock if necessary before sending a replacing Order", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_SpinBorrowPrice(999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),

	m_SpinDisplaySizeFraction(999, 0, 10, 1, 3, TakionSpin::charComma, 1, TakionSpin::charSlash),
	m_SpinDisplaySizeRoundLot(999, 0, 0, 1),//, 3, TakionSpin::charComma),
////
	m_clientId(0),
	m_useClientId(false),
	m_accountAll(false),
	m_positionType(0),
#ifndef TAKION_NO_OPTIONS
	m_instrumentMask(0),
#endif
	m_sideMask(0),
	m_directionMask(0),
	m_orderPick(0),
	m_tifMask(0),
	m_orderTypeMask(0),
	m_pqOpenMask(0),
	m_tCloseMask(0),
	m_excludeDestinations(true),
	m_excludeMnemonics(true),
	m_replaceOrder(false),
	m_replaceNative(false),
	m_marketOrder(false),
	m_priceOffsetPercent(false),
	m_priceEnforceMinOffset(false),
	m_priceOffsetReverseForSell(false),
	m_sizeOffsetPercent(false),
	m_sizeOffsetReverseForSell(false),
	m_adjustable(false),
	m_adjustableUseMmBox(false),
	m_preBorrow(false),
	m_displaySizeMode(OCDSM_COUNT),
	m_mnemonic(0)
{
	m_groupBoxReplaceOrder.SetLeftWindow(&m_GroupDisplaySize);
	m_groupBoxReplaceOrder.SetTopWindow(&m_CheckBoxReplaceOrder);
	m_groupBoxReplaceOrder.SetRightWindow(&m_SpinReplaceSize);
	m_groupBoxReplaceOrder.SetBottomWindow(&m_GroupDisplaySize);

	m_SpinClientId.AddSpinCell(1, false);
	m_SpinClientId.AddSpinCell(100, false);
	m_SpinClientId.AddSpinCell(1000000, true);
	m_SpinClientId.AddSpinCell(1000, true);

	m_SpinReplacePriceOffset.AddSpinCell(1, false, 10);
	m_SpinReplacePriceOffset.AddSpinCell(100, false);
	m_SpinReplacePriceOffset.AddSpinCell(10000, false);
	m_SpinReplacePriceOffset.AddSpinCell(1000000, true);

	m_SpinReplaceSize.AddSpinCell(1, false);
	m_SpinReplaceSize.AddSpinCell(100, false);
	m_SpinReplaceSize.AddSpinCell(100000, true);
	m_SpinReplaceSize.AddSpinCell(1000, true);

	m_SpinBorrowPrice.AddSpinCell(1, false);
	m_SpinBorrowPrice.AddSpinCell(100, false);
	m_SpinBorrowPrice.AddSpinCell(10000, true);

	m_SpinDisplaySizeRoundLot.AddSpinCell(1, false);
	m_SpinDisplaySizeRoundLot.AddSpinCell(10, true);

	m_SpinDisplaySizeFraction.AddSpinCell(1, false, 10);
	m_SpinDisplaySizeFraction.AddSpinCell(10, true);

	m_RadioAccountCurrent.SetRadioGroupOwner(&m_RadioGroupAccount);
	m_RadioAccountAll.SetRadioGroupOwner(&m_RadioGroupAccount);

	m_RadioPositionCurrent.SetRadioGroupOwner(&m_RadioGroupPosition);
	m_RadioPositionEach.SetRadioGroupOwner(&m_RadioGroupPosition);
	m_RadioPositionAll.SetRadioGroupOwner(&m_RadioGroupPosition);

	m_ListBoxOrder.SetSelectionBkColor(RGB(0, 96, 0));
	m_ListBoxMnemonics.SetSelectionBkColor(0);
	AddListBox(&m_ListBoxOrder);

	AddListBox(&m_ListBoxDestination);
	AddListBox(&m_ListBoxMnemonics);

	m_radioDisplaySizeMode[OCDSM_COUNT].SetTooltipText("Do not change the Display Size");
	m_radioDisplaySizeMode[OCDSM_SIZE_FRACTION].SetTooltipText("Fraction of the Order Size");
	m_radioDisplaySizeMode[OCDSM_ROUND_LOT].SetTooltipText("Number of Round Lots");

	const COLORREF lightGrayColor = m_visualBase->GetLightGrayColor();
	RadioOwnerDraw* displaySizeCursor = m_radioDisplaySizeMode;
	for(unsigned int i = 0; i <= OCDSM_COUNT; ++i, ++displaySizeCursor)
	{
		displaySizeCursor->SetRadioGroupOwner(&m_RadioGroupDisplaySize);
		displaySizeCursor->SetPressedBkColor(lightGrayColor);
		displaySizeCursor->SetTextOffset(-3);
	}


	int y = vgap;
	const int buttonWidth = m_accountWidth - hgap - hgap;// 60;
	AddButtonIndirect(cancelOrderText, WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, hgap, y, buttonWidth, buttonHeight, buttonCancelOrderId);

	int x = m_accountWidth + hgap + hgap;
	const int clientIdWidth = 80;
//	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, x, y += buttonHeight + vgap, clientIdWidth, spinHeight, spinClientIdId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, x, y, clientIdWidth, spinHeight, spinClientIdId);
	AddButtonIndirect("ClientId", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, x + clientIdWidth + hgap, y + (spinHeight - checkBoxHeight) / 2, 40, checkBoxHeight, checkBoxClientIdId);

	int width = 41;//29, 37; (45,36)
	int xControl = x + hgap;
	int xWidth = width - hgap - hgap;
	int yGroup = buttonHeight + vgap;
	const int topGroup = yGroup;
	y = yGroup;
//	int groupHeight = 27;
	int groupHeight = checkBoxHeight * 2 + groupOffset + vgap;
	AddButtonIndirect("Account", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width, groupHeight, groupAccountAllId);
	AddButtonIndirect("Current", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xControl, y += groupOffset, xWidth, checkBoxHeight, radioAccountCurrentId);
//	AddButtonIndirect("ALL", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP|BS_LEFTTEXT|BS_RIGHT, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, radioAccountAll);
	AddButtonIndirect("ALL", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, radioAccountAllId);

	yGroup += groupHeight + vgap;
	y = yGroup;
	groupHeight = checkBoxHeight * 3 + groupOffset + vgap;
	AddButtonIndirect("Position", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width, groupHeight, groupPositionId);
	AddButtonIndirect("Current", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xControl, y += groupOffset, xWidth, checkBoxHeight, radioPositionCurrentId);
	AddButtonIndirect("Each", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, radioPositionEachId);
	AddButtonIndirect("ALL", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, radioPositionAllId);

#ifndef TAKION_NO_OPTIONS
	width = 45;
	xWidth = width - hgap - hgap;
	yGroup += groupHeight + vgap;
	y = yGroup;
	AddButtonIndirect("Instrument", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width, groupHeight, groupInstrumentId);
	AddButtonIndirect("Stock", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += groupOffset, xWidth, checkBoxHeight, checkBoxInstrumentStockId);
	AddButtonIndirect("Option", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxInstrumentOptionId);
	AddButtonIndirect("Future", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxInstrumentFutureId);
#endif
	const int xOffset = x + width + hgap;

	AddButtonIndirect("Check All", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += checkBoxHeight + vgap, width, buttonHeight, buttonCheckAllId);
	AddButtonIndirect("Uncheck All", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += buttonHeight + vgap, width, buttonHeight, buttonUncheckAllId);

	width = 41;
	xWidth = width - hgap - hgap;
	yGroup = topGroup;
	y = yGroup;
	x += width + hgap;
	xControl = x + hgap;
//	const int x2 = x;
	AddButtonIndirect("Side", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width, groupHeight, groupCancelSideId);
	AddButtonIndirect("Buy", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += groupOffset, xWidth, checkBoxHeight, checkBoxSideBuyId);
	AddButtonIndirect("Sell", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxSideSellId);
	AddButtonIndirect("Short", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxSideShortId);

	yGroup = topGroup;
	y = yGroup;
	x += width + hgap;
	xControl = x + hgap;
	const int xOffset2 = x + width + hgap;

	AddButtonIndirect("Direction", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width, groupHeight, groupDirectionId);
	AddButtonIndirect("Close", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += groupOffset, xWidth, checkBoxHeight, checkBoxDirectionCloseId);
	AddButtonIndirect("Open", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxDirectionOpenId);
	AddButtonIndirect("Add", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxDirectionAddId);

	yGroup += groupHeight + vgap;
	y = yGroup;
	x = xOffset;
	width = 59;
	const int listBoxHeight = 42;

	AddStaticIndirect("Order Pick", WS_VISIBLE|SS_NOPREFIX, 0, x, y, width, staticHeight, staticPickOrderId);
	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT|WS_VSCROLL, 0, x, y += staticHeight, width, listBoxHeight, listBoxPickOrderId);

	yGroup = topGroup;
	y = yGroup;
	x = xOffset2;
	xControl = x + hgap;
	width = 47;//93
	xWidth = width - hgap - hgap;
	groupHeight = checkBoxHeight * 10 + groupOffset + vgap;
	AddButtonIndirect("TIF", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width, groupHeight, groupCancelTifId);
	AddButtonIndirect("OnOpen", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += groupOffset, xWidth, checkBoxHeight, checkBoxTifOnOpenId);
	AddButtonIndirect("OnClose", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTifOnCloseId);
	AddButtonIndirect("Day", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTifDayId);
	AddButtonIndirect("Extended", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTifExtendedId);
	AddButtonIndirect("Time", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTifTimeId);
	AddButtonIndirect("IOC", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTifIocId);
	AddButtonIndirect("FOK", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTifFokId);
	AddButtonIndirect("GTC", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTifGtcId);
	AddButtonIndirect("GTT", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTifGttId);
	AddButtonIndirect("ETC", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTifExtendedTradingCloseId);

	yGroup = topGroup;
	y = yGroup;
	x += width + hgap;
	xControl = x + hgap;
	width = 47;
	xWidth = width - hgap - hgap;
	groupHeight = checkBoxHeight * 4 + groupOffset + vgap;
	AddButtonIndirect("Order Type", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width, groupHeight, groupCancelOrderTypeId);
	AddButtonIndirect("Algorithm", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += groupOffset, xWidth, checkBoxHeight, checkBoxOrderTypeAlgoId);
	AddButtonIndirect("Limit", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxOrderTypeLimitId);
	AddButtonIndirect("Market", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxOrderTypeMarketId);
	AddButtonIndirect("Stop", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxOrderTypeStopId);

	yGroup += groupHeight + vgap;
	y = yGroup;
	groupHeight = checkBoxHeight * 2 + groupOffset + vgap;
	AddButtonIndirect("PQ Open", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width, groupHeight, groupCancelPqOpenId);
	AddButtonIndirect("Yes", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += groupOffset, xWidth, checkBoxHeight, checkBoxPqOpenYesId);
	AddButtonIndirect("No", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxPqOpenNoId);

	yGroup += groupHeight + vgap;
	y = yGroup;
	width = 54;
	xWidth = width - hgap - hgap;
	AddButtonIndirect("Today's Close", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width, groupHeight, groupCancelTCloseId);
	AddButtonIndirect("Yes", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += groupOffset, xWidth, checkBoxHeight, checkBoxTCloseYesId);
	AddButtonIndirect("No", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, xControl, y += checkBoxHeight, xWidth, checkBoxHeight, checkBoxTCloseNoId);

	x += width + hgap;
	y = vgap;
	width = 61;
	const int countWidth = 8;
	const int excludeBoxWidth = width - countWidth;
	const int maxHeight = checkBoxHeight < staticHeight ? staticHeight : checkBoxHeight;
	AddButtonIndirect("Exclude Dest.", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, x, y, excludeBoxWidth, checkBoxHeight, checkBoxExcludeDestinationsId);

	AddStaticIndirect("0", WS_VISIBLE|SS_NOPREFIX|SS_RIGHT, 0, x + excludeBoxWidth, y + (checkBoxHeight - staticHeight) / 2, countWidth, staticHeight, staticDestinationCountId);

	const int yLb = y + maxHeight;
	y = yLb;
	int lbHeight = m_height - y;
	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_SORT|LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT|WS_VSCROLL, 0, x, y, width, lbHeight, listBoxDestinationsId);

	x += width + hgap;
	y = vgap;
	width = 59;// 61;

	AddButtonIndirect("Exclude Mnem.", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, x, y, width, checkBoxHeight, checkBoxExcludeMnemonicsId);
	y = yLb;
	lbHeight -= 3 * editHeight + vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_SORT|LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT|WS_VSCROLL, 0, x, y, width, lbHeight, listBoxMnemonicsId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL|ES_UPPERCASE, 0, x, y += lbHeight + vgap, width, editHeight, editCancelOrderMnemonicId);

	const int buttonWidthBig = width * 3 / 5;
	const int buttonWidthSmall = width - buttonWidthBig;
	const int xSmall = x + buttonWidthBig;
	AddButtonIndirect("Find", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += editHeight, buttonWidthBig, editHeight, buttonMnemonicFindId);
	AddButtonIndirect("Add", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, xSmall, y, buttonWidthSmall, editHeight, buttonMnemonicAddId);
	AddButtonIndirect("Remove", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += editHeight, buttonWidthBig, editHeight, buttonMnemonicRemoveId);
	AddButtonIndirect("Clear", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, xSmall, y, buttonWidthSmall, editHeight, buttonMnemonicClearId);

	x += width + 10 * hgap;
	y = yLb;
	AddButtonIndirect("Replace Order", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, x, y, 57, checkBoxHeight, checkBoxReplaceOrderId);
	AddButtonIndirect("Native", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, x + 100, y, 32, checkBoxHeight, checkBoxReplaceOrderNativeId);
	AddButtonIndirect("Market Order", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, x, y += checkBoxHeight + vgap, 54, checkBoxHeight, checkBoxReplaceOrderMarketId);

	const int replaceControlX = x;
	y += checkBoxHeight + 5 * vgap;

	const int priceOffsetStaticWidth = 38;
	const int spinPriceWidth = 70;
	const int spinSizeWidth = 63;//57;
	const int percentGap = 3;
	const int percentWidth = 16;
	AddStaticIndirect("Price Offset", WS_VISIBLE|SS_NOPREFIX, 0, x, y, priceOffsetStaticWidth, staticHeight, staticReplaceOrderPriceOffsetId);
	AddButtonIndirect("%", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, x + priceOffsetStaticWidth + percentGap, y, percentWidth, checkBoxHeight, checkBoxReplaceOrderPriceOffsetPercentId);

	const int sizeGap = 10;
	x += spinPriceWidth + sizeGap;
	const int sizeOffsetX = x;
	AddStaticIndirect("Size Offset", WS_VISIBLE|SS_NOPREFIX, 0, x, y, priceOffsetStaticWidth, staticHeight, staticReplaceOrderSizeOffsetId);
	AddButtonIndirect("%", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, x + priceOffsetStaticWidth + percentGap, y, percentWidth, checkBoxHeight, checkBoxReplaceOrderSizeOffsetPercentId);

	y += checkBoxHeight;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, replaceControlX, y, spinPriceWidth, spinHeight, spinReplaceOrderPriceOffsetId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, sizeOffsetX, y, spinSizeWidth, spinHeight, spinReplaceOrderSizeOffsetId);
	const int xRight = sizeOffsetX + spinSizeWidth;

	y += spinHeight + vgap;
	AddButtonIndirect("Reverse for Sell", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, replaceControlX, y, spinPriceWidth, checkBoxHeight, checkBoxReplaceOrderPriceOffsetReverseForSellId);
	AddButtonIndirect("Reverse for Sell", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, sizeOffsetX, y, spinSizeWidth, checkBoxHeight, checkBoxReplaceOrderSizeOffsetReverseForSellId);

	AddButtonIndirect("Enforce MinOffset", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, replaceControlX, y += checkBoxHeight, spinPriceWidth, checkBoxHeight, checkBoxReplaceOrderPriceEnforceMinOffsetId);

	y += checkBoxHeight + 6 * vgap;
	const int preBorrowTop = y;
	const int preBorrowWidth = 51;
	AddButtonIndirect("Pre Borrow", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, sizeOffsetX, y, preBorrowWidth, checkBoxHeight, checkBoxReplaceOrderPreBorrowId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, sizeOffsetX, y += checkBoxHeight, preBorrowWidth, spinHeight, spinReplaceOrderPreBorrowId);
	AddStaticIndirect("¢", WS_VISIBLE|SS_NOPREFIX|SS_RIGHT, 0, sizeOffsetX - staticHeight, y + (spinHeight - staticHeight) / 2, staticHeight, staticHeight, staticReplaceOrderPreBorrowCentId);
	const int preBorrowBottom = y + spinHeight;

	y = preBorrowTop;
	AddButtonIndirect("Adjustable", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, replaceControlX, y, 45, checkBoxHeight, checkBoxReplaceOrderAdjustableId);
	AddButtonIndirect("MmBox if 1 Order", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, replaceControlX, y += checkBoxHeight + vgap, 68, checkBoxHeight, checkBoxReplaceOrderAdjustableMmBoxId);

	const int displayGroupTop = preBorrowBottom + 4 * vgap;
	const int displayGroupLeft = replaceControlX;// -2 * hgap;
	y = displayGroupTop + groupOffset;
	const int displaySizeWidth = 52;
	x = replaceControlX + 2 * hgap;
//	x = displayGroupLeft + hgap;
//	AddStaticIndirect("Display Size", WS_VISIBLE|SS_NOPREFIX|SS_RIGHT, 0, replaceControlX + checkBoxHeight, y += spinHeight - staticHeight, displaySizeWidth - checkBoxHeight, staticHeight, staticReplaceOrderDisplaySizeId);
	AddButtonIndirect("No Change", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|BS_LEFTTEXT|WS_GROUP|WS_TABSTOP, 0, x, y, displaySizeWidth, checkBoxHeight, radioReplaceOrderDisplaySizeNoChangeId);
	AddButtonIndirect("Size Fraction", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|BS_LEFTTEXT, 0, x, y += checkBoxHeight + vgap + 2, displaySizeWidth, checkBoxHeight, radioReplaceOrderDisplaySizeFractionId);
	const int displaySizeFractionY = y;
	AddButtonIndirect("Round Lot", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|BS_LEFTTEXT, 0, x, y += checkBoxHeight + vgap + 2, displaySizeWidth, checkBoxHeight, radioReplaceOrderDisplaySizeRoundLotId);

//	x = replaceControlX + displaySizeWidth + hgap;
	x += displaySizeWidth + hgap;
	const int displaySizeSpinWidth = 32;
	const int yOff = (spinHeight - checkBoxHeight) / 2;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, x, displaySizeFractionY - yOff, displaySizeSpinWidth, spinHeight, spinReplaceOrderDisplaySizeFractionId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x, y - yOff, displaySizeSpinWidth, spinHeight, spinReplaceOrderDisplaySizeRoundLotId);
	AddButtonIndirect("Display Size", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, displayGroupLeft, displayGroupTop, x + displaySizeSpinWidth + hgap + hgap - displayGroupLeft, y - yOff + spinHeight + vgap + vgap - displayGroupTop, groupReplaceOrderDisplaySizeId);

	x = xRight + 4 * hgap;
//	width = buttonWidth;
	const int buttonHeightPlusGap = editHeight + vgap;

	const int groupTop = 3 * vgap;

	y = groupTop;
	const int buttonX = x + hgap;
//	const int buttonX2 = buttonX + buttonWidth + hgap;
	y += groupOffset;
	AddButtonIndirect("All", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, buttonX, y, buttonWidth, editHeight, buttonCancelReplaceOrdersAllId);
	AddButtonIndirect("Buy", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, buttonX, y += buttonHeightPlusGap, buttonWidth, editHeight, buttonCancelReplaceOrdersBuyId);
//	y += buttonHeightPlusGap;
	AddButtonIndirect("Sell+Short Limit", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, buttonX, y += buttonHeightPlusGap, buttonWidth, editHeight, buttonCancelReplaceOrdersSellShortLimitId);
	AddButtonIndirect("Short Stop", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, buttonX, y += buttonHeightPlusGap, buttonWidth, editHeight, buttonCancelReplaceOrdersShortStopId);
	AddButtonIndirect("NSDQ Day+Ext", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, buttonX, y += buttonHeightPlusGap, buttonWidth, editHeight, buttonCancelReplaceOrdersLimitNsdqDayExtId);
	AddButtonIndirect("All Opening", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, buttonX, y += buttonHeightPlusGap, buttonWidth, editHeight, buttonCancelReplaceOrdersAllOpeningId);
	AddButtonIndirect("Sell+Short Closing", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, buttonX, y += buttonHeightPlusGap, buttonWidth, editHeight, buttonCancelReplaceOrdersSellShortClosingId);

	groupHeight = y + buttonHeightPlusGap - groupTop + vgap;
	AddButtonIndirect(cancelOrdersText, WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, groupTop, buttonWidth + 2 * hgap, groupHeight, groupCancelReplaceOrdersAllId);

//	y += checkBoxHeight + 6 * vgap;
//	AddButtonIndirect("Adjustable", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, replaceControlX, y, 45, checkBoxHeight, checkBoxReplaceOrderAdjustableId);
//	AddButtonIndirect("MmBox if 1 Order", WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_TABSTOP, 0, x, y, 68, checkBoxHeight, checkBoxReplaceOrderAdjustableMmBoxId);

	AddResizeControl(new VerticalResizeControl(this, listBoxDestinationsId));
//	AddResizeControl(new VerticalResizeControl(this, listBoxMnemonicsId));
	AddResizeControl(new VerticalResizeControlBottomOffset(this, listBoxMnemonicsId));
	AddResizeControl(new VerticalMoveControl(this, editCancelOrderMnemonicId));
	AddResizeControl(new VerticalMoveControl(this, buttonMnemonicFindId));
	AddResizeControl(new VerticalMoveControl(this, buttonMnemonicAddId));
	AddResizeControl(new VerticalMoveControl(this, buttonMnemonicRemoveId));
	AddResizeControl(new VerticalMoveControl(this, buttonMnemonicClearId));
}
/*
HWND SampleDialogSettingCancel::GetFirstTabControl() const
{
	return m_SpinClientId.m_hWnd;
}
*/

StringSetting* SampleDialogSettingCancel::GetAccountIdSetting()
{
	return &extensionConfig->m_cancelOrderAccountId;
}

typedef SetSetting<StringSetting, std::set<StringSetting> > StringSetSetting;
typedef MapSetting<StringSetting, BoolSetting, std::map<StringSetting, BoolSetting> > StringBoolMapSetting;

void SampleDialogSettingCancel::RestoreLayout()
{
	BaseSampleDialogSettingOption::RestoreLayout();

	m_EditSymbol.SetWindowText(extensionConfig->m_cancelOrderSymbol.GetValue().c_str());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_cancelOrderOption.GetValue(),
		extensionConfig->m_cancelOrderOptionSuffix.GetValue(),
		extensionConfig->m_cancelOrderOptionCall.GetValue(),
		extensionConfig->m_cancelOrderOptionStrikePrice.GetValue(),
		extensionConfig->m_cancelOrderOptionExpiration.GetValue());
#endif

	m_SpinClientId.SetValue(extensionConfig->m_cancelOrderClientId.GetValue());

	m_CheckBoxClientId.SetCheck(extensionConfig->m_cancelOrderUseClientId.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnClientId();

	if(extensionConfig->m_cancelOrderAccountAll.GetValue())
	{
		m_RadioAccountCurrent.SetCheck(BST_UNCHECKED);
		m_RadioAccountAll.SetCheck(BST_CHECKED);
	}
	else
	{
		m_RadioAccountAll.SetCheck(BST_UNCHECKED);
		m_RadioAccountCurrent.SetCheck(BST_CHECKED);
	}
	OnAccount();

	switch(extensionConfig->m_cancelOrderPosition.GetValue())
	{
		case 0:
		m_RadioPositionEach.SetCheck(BST_UNCHECKED);
		m_RadioPositionAll.SetCheck(BST_UNCHECKED);
		m_RadioPositionCurrent.SetCheck(BST_CHECKED);
		break;

		case 1://each
		m_RadioPositionCurrent.SetCheck(BST_UNCHECKED);
		m_RadioPositionAll.SetCheck(BST_UNCHECKED);
		m_RadioPositionEach.SetCheck(BST_CHECKED);
		break;

		default://all
		m_RadioPositionCurrent.SetCheck(BST_UNCHECKED);
		m_RadioPositionEach.SetCheck(BST_UNCHECKED);
		m_RadioPositionAll.SetCheck(BST_CHECKED);
		break;
	}
	OnPosition();

#ifndef TAKION_NO_OPTIONS
	SetInstrumentMask(extensionConfig->m_cancelOrderInstrument.GetValue());
#endif
	SetSideMask(extensionConfig->m_cancelOrderSide.GetValue());

	SetDirectionMask(extensionConfig->m_cancelOrderDirection.GetValue());

	unsigned char orderPick = extensionConfig->m_cancelOrderPick.GetValue();
	if(orderPick >= TU_ORDER_CANCEL_COUNT)orderPick = TU_ORDER_CANCEL_ALL;
	if(orderPick != m_orderPick)
	{
		m_ListBoxOrder.SetCurSel(orderPick);
		OnSelchangeOrder();
	}

	SetTifMask(extensionConfig->m_cancelOrderTif.GetValue());

	SetOrderTypeMask(extensionConfig->m_cancelOrderType.GetValue());

	SetPqOpenMask(extensionConfig->m_cancelOrderPqOpen.GetValue());

	SetTCloseMask(extensionConfig->m_cancelOrderTClose.GetValue());

	m_CheckBoxExcludeDestinations.SetCheck(extensionConfig->m_cancelOrderExcludeDestinations.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnExcludeDestinations();

	const StringSetSetting& destinationSet = extensionConfig->m_cancelOrderDestinations;
	StringSetting stringSetting;
//	sestinationSet.FindValue()
	unsigned int i = 0, end = m_ListBoxDestination.GetItemCount();
	for(; i < end; ++i)
	{
		stringSetting.SetValue(((const Destination*)m_ListBoxDestination.GetItemDataPtr(i))->GetProperNameStr());
		m_ListBoxDestination.SetCheckAndInvalidate(i, destinationSet.FindValue(stringSetting) != nullptr, false);
	}
	ListBoxDestinationChecked();

	m_CheckBoxExcludeMnemonics.SetCheck(extensionConfig->m_cancelOrderExcludeMnemonics.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnExcludeMnemonics();

	unsigned char added;
	const StringBoolMapSetting& mnemonicSet = extensionConfig->m_cancelOrderMnemonics;
	m_ListBoxMnemonics.SetRedraw(FALSE);
	m_ListBoxMnemonics.ClearItems(false);
	MnemonicsCleared();
	for(StringBoolMapSetting::ConstIterator itm = mnemonicSet.GetConstBegin(), itmend = mnemonicSet.GetConstEnd(); itm != itmend; ++itm)
	{
		m_ListBoxMnemonics.AddItemChecked(U_RepresentStringAsUnsignedNumber<MnemonicType>(itm->first.GetValue().c_str()), itm->second.GetValue(), false, added);
	}
	m_ListBoxMnemonics.SetRedraw(TRUE);
	m_ListBoxMnemonics.Invalidate(FALSE);

	OnSelchangeMnemonic();
	EnableControl(m_ButtonMnemonicClear.m_hWnd, m_ListBoxMnemonics.GetItemCount() > 1, m_ListBoxMnemonics.m_hWnd);
/*
	bool m_priceOffsetPercent;
	bool m_priceOffsetReverseForSell;
	bool m_sizeOffsetPercent;
	bool m_sizeOffsetReverseForSell;
	bool m_adjustable;
	bool m_adjustableUseMmBox;
*/
	m_CheckBoxReplaceOrder.SetCheck(extensionConfig->m_cancelOrderReplace.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnReplaceOrder();
	m_CheckBoxReplaceNative.SetCheck(extensionConfig->m_cancelOrderReplaceNative.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnReplaceNative();
	m_CheckBoxMarketOrder.SetCheck(extensionConfig->m_cancelOrderReplaceMarket.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnMarketOrder();
	m_CheckBoxReplacePriceOffsetPercent.SetCheck(extensionConfig->m_cancelOrderReplacePriceOffsetPercent.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnReplacePriceOffsetPercent();
	m_CheckBoxReplacePriceEnforceMinOffset.SetCheck(extensionConfig->m_cancelOrderReplacePriceEnforceMinOffset.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnReplacePriceEnforceMinOffset();
	m_CheckBoxReplacePriceReverseForSell.SetCheck(extensionConfig->m_cancelOrderReplacePriceOffsetReverseForSell.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnReplacePriceOffsetReverseForSell();
	m_CheckBoxReplaceSizeOffsetPercent.SetCheck(extensionConfig->m_cancelOrderReplaceSizeOffsetPercent.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnReplaceSizeOffsetPercent();
	m_CheckBoxReplaceSizeReverseForSell.SetCheck(extensionConfig->m_cancelOrderReplaceSizeOffsetReverseForSell.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnReplaceSizeOffsetReverseForSell();
	m_CheckBoxReplaceAdjustable.SetCheck(extensionConfig->m_cancelOrderReplaceAdjustable.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnAdjustable();
	m_CheckBoxReplaceAdjustableUseMmBox.SetCheck(extensionConfig->m_cancelOrderReplaceAdjustableUseMmBox.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnAdjustableMmBox();
	m_CheckBoxPreBorrow.SetCheck(extensionConfig->m_cancelOrderReplacePreBorrow.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnPreBorrow();

	m_SpinReplacePriceOffset.SetValue(extensionConfig->m_cancelOrdeReplacePriceOffset.GetValue());
	m_SpinReplaceSize.SetValue(extensionConfig->m_cancelOrdeReplaceSizeOffset.GetValue());
	m_SpinBorrowPrice.SetValue(extensionConfig->m_cancelOrdeReplaceBorrowPrice.GetValue());
	m_SpinDisplaySizeFraction.SetValue(extensionConfig->m_cancelOrdeReplaceDisplaySizeFraction.GetValue());
	m_SpinDisplaySizeRoundLot.SetValue(extensionConfig->m_cancelOrdeReplaceDisplaySizeRoundLot.GetValue());

	switch(extensionConfig->m_cancelOrderDisplaySizeMode.GetValue())
	{
		case OCDSM_SIZE_FRACTION:
		m_radioDisplaySizeMode[OCDSM_COUNT].SetCheck(BST_UNCHECKED);
		m_radioDisplaySizeMode[OCDSM_ROUND_LOT].SetCheck(BST_UNCHECKED);
		m_radioDisplaySizeMode[OCDSM_SIZE_FRACTION].SetCheck(BST_CHECKED);
		DisplaySizeModeChange(OCDSM_SIZE_FRACTION);
		break;

		case OCDSM_ROUND_LOT:
		m_radioDisplaySizeMode[OCDSM_COUNT].SetCheck(BST_UNCHECKED);
		m_radioDisplaySizeMode[OCDSM_SIZE_FRACTION].SetCheck(BST_UNCHECKED);
		m_radioDisplaySizeMode[OCDSM_ROUND_LOT].SetCheck(BST_CHECKED);
		DisplaySizeModeChange(OCDSM_ROUND_LOT);
		break;

		default://OCDSM_COUNT
		m_radioDisplaySizeMode[OCDSM_SIZE_FRACTION].SetCheck(BST_UNCHECKED);
		m_radioDisplaySizeMode[OCDSM_ROUND_LOT].SetCheck(BST_UNCHECKED);
		m_radioDisplaySizeMode[OCDSM_COUNT].SetCheck(BST_CHECKED);
		DisplaySizeModeChange(OCDSM_COUNT);
		break;
	}
	m_CheckBoxReplacePriceEnforceMinOffset.SetInvalid(!m_replaceOrder || m_marketOrder || m_replacePriceOffset.isZero());

	m_modified = false;
}

void SampleDialogSettingCancel::UpdateSettings()
{
	BaseSampleDialogSettingOption::UpdateSettings();
	extensionConfig->m_cancelOrderSymbol.SetValue(m_symbol);
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_cancelOrderOption.SetValue(m_optionMode);
	extensionConfig->m_cancelOrderOptionCall.SetValue(m_optionCall);
	extensionConfig->m_cancelOrderOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_cancelOrderOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_cancelOrderOptionExpiration.SetValue(GetExpirationDate());
#endif

	extensionConfig->m_cancelOrderClientId.SetValue(m_clientId);
	extensionConfig->m_cancelOrderUseClientId.SetValue(m_useClientId);
	extensionConfig->m_cancelOrderAccountAll.SetValue(m_accountAll);
	extensionConfig->m_cancelOrderPosition.SetValue(m_positionType);
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_cancelOrderInstrument.SetValue(m_instrumentMask);
#endif
	extensionConfig->m_cancelOrderSide.SetValue(m_sideMask);
	extensionConfig->m_cancelOrderDirection.SetValue(m_directionMask);
	extensionConfig->m_cancelOrderPick.SetValue(m_orderPick);
	extensionConfig->m_cancelOrderTif.SetValue(m_tifMask);
	extensionConfig->m_cancelOrderType.SetValue(m_orderTypeMask);
	extensionConfig->m_cancelOrderPqOpen.SetValue(m_pqOpenMask);
	extensionConfig->m_cancelOrderTClose.SetValue(m_tCloseMask);
	extensionConfig->m_cancelOrderExcludeDestinations.SetValue(m_excludeDestinations);
	extensionConfig->m_cancelOrderExcludeMnemonics.SetValue(m_excludeMnemonics);

	const Destination* destination;
	StringSetting stringSetting;
	StringSetSetting& destinationSet = extensionConfig->m_cancelOrderDestinations;
	destinationSet.Clear();
	const ListBoxCheckDestination::DestinationSet& checkSet = m_ListBoxDestination.GetCheckedSet();
	for(ListBoxCheckDestination::DestinationSet::const_iterator it = checkSet.cbegin(), itend = checkSet.cend(); it != itend; ++it)
	{
		destination = *it;
		stringSetting.SetValue(destination->GetProperNameStr());
		destinationSet.AddValue(stringSetting);
	}

	BoolSetting boolSetting;
	StringBoolMapSetting& mnemonicsMap = extensionConfig->m_cancelOrderMnemonics;
	mnemonicsMap.Clear();

	const ListBoxStringAsUIntChecked<MnemonicType>::SortedStringMap& checkMap = m_ListBoxMnemonics.GetCheckedMap();
	ListBoxStringAsUIntChecked<MnemonicType>::SortedStringMap::const_iterator itm = checkMap.cbegin(), itmend = checkMap.cend();

	for(; itm != itmend; ++itm)
	{
		boolSetting.SetValue(itm->second);
		stringSetting.SetValue(U_GetStringFromNumber(itm->first));
		mnemonicsMap.AddValue(stringSetting, boolSetting);
	}

/*
	MnemonicType val;
	unsigned int i = 0, end = m_ListBoxMnemonics.GetItemCount();
	for(; i < end; ++i)
	{
		val = (MnemonicType)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			m_ListBoxMnemonics.GetItemDataPtr(i);
		stringSetting.SetValue(U_GetStringFromNumber(val));
		boolSetting.SetValue(m_ListBoxMnemonics.isItemAtChecked(i));
		mnemonicsMap.AddValue(stringSetting, boolSetting);
	}
*/
	extensionConfig->m_cancelOrderReplace.SetValue(m_replaceOrder);
	extensionConfig->m_cancelOrderReplaceNative.SetValue(m_replaceNative);
	extensionConfig->m_cancelOrderReplaceMarket.SetValue(m_marketOrder);
	extensionConfig->m_cancelOrderReplacePriceOffsetPercent.SetValue(m_priceOffsetPercent);
	extensionConfig->m_cancelOrderReplacePriceEnforceMinOffset.SetValue(m_priceEnforceMinOffset);
	extensionConfig->m_cancelOrderReplacePriceOffsetReverseForSell.SetValue(m_priceOffsetReverseForSell);
	extensionConfig->m_cancelOrderReplaceSizeOffsetPercent.SetValue(m_sizeOffsetPercent);
	extensionConfig->m_cancelOrderReplaceSizeOffsetReverseForSell.SetValue(m_sizeOffsetReverseForSell);
	extensionConfig->m_cancelOrderReplaceAdjustable.SetValue(m_adjustable);
	extensionConfig->m_cancelOrderReplaceAdjustableUseMmBox.SetValue(m_adjustableUseMmBox);
	extensionConfig->m_cancelOrderReplacePreBorrow.SetValue(m_preBorrow);
	extensionConfig->m_cancelOrderDisplaySizeMode.SetValue(m_displaySizeMode);
	extensionConfig->m_cancelOrdeReplacePriceOffset.SetValue(m_SpinReplacePriceOffset.GetValue());
	extensionConfig->m_cancelOrdeReplaceSizeOffset.SetValue(m_SpinReplaceSize.GetValue());
	extensionConfig->m_cancelOrdeReplaceBorrowPrice.SetValue(m_SpinBorrowPrice.GetValue());
	extensionConfig->m_cancelOrdeReplaceDisplaySizeFraction.SetValue(m_SpinDisplaySizeFraction.GetValue());
	extensionConfig->m_cancelOrdeReplaceDisplaySizeRoundLot.SetValue(m_SpinDisplaySizeRoundLot.GetValue());
}

void SampleDialogSettingCancel::Leaving()
{
}

void SampleDialogSettingCancel::UpdateControls()
{
}

void SampleDialogSettingCancel::BeforeDestroy()
{
	NullifyValues();
	BaseSampleDialogSettingOption::BeforeDestroy();
}

void SampleDialogSettingCancel::NullifyValues()
{
	m_clientId = 0;
	m_useClientId = false;
	m_accountAll = false;
	m_positionType = 0;
#ifndef TAKION_NO_OPTIONS
	m_instrumentMask = 0;
#endif
	m_sideMask = 0;
	m_directionMask = 0;
	m_orderPick = 0;
	m_tifMask = 0;
	m_orderTypeMask = 0;
	m_pqOpenMask = 0;
	m_tCloseMask = 0;
	m_excludeDestinations = true;
	m_excludeMnemonics = true;
//ReplaceOrder
	m_replaceOrder = false;
	m_replaceNative = false;
	m_marketOrder = false;
	m_priceOffsetPercent = false;
	m_priceEnforceMinOffset = false;
	m_priceOffsetReverseForSell = false;
	m_sizeOffsetPercent = false;
	m_sizeOffsetReverseForSell = false;
	m_adjustable = false;
	m_adjustableUseMmBox = false;
	m_preBorrow = false;
	m_displaySizeMode = OCDSM_COUNT;
////
	m_mnemonic = 0;
}

void SampleDialogSettingCancel::ToolTipsEnabled(bool enable)
{
	BaseSampleDialogSettingOption::ToolTipsEnabled(enable);
	m_ButtonCancelOrder.EnableTooltips(enable, m_toolTip);

	m_SpinClientId.EnableTooltips(enable, m_toolTip);
	m_CheckBoxClientId.EnableTooltips(enable, m_toolTip);

	m_RadioAccountCurrent.EnableTooltips(enable, m_toolTip);
	m_RadioAccountAll.EnableTooltips(enable, m_toolTip);

	m_RadioPositionCurrent.EnableTooltips(enable, m_toolTip);
	m_RadioPositionEach.EnableTooltips(enable, m_toolTip);
	m_RadioPositionAll.EnableTooltips(enable, m_toolTip);

#ifndef TAKION_NO_OPTIONS
	m_CheckBoxInstrumentStock.EnableTooltips(enable, m_toolTip);
	m_CheckBoxInstrumentOption.EnableTooltips(enable, m_toolTip);
	m_CheckBoxInstrumentFuture.EnableTooltips(enable, m_toolTip);
#endif

	m_CheckBoxSideBuy.EnableTooltips(enable, m_toolTip);
	m_CheckBoxSideSell.EnableTooltips(enable, m_toolTip);
	m_CheckBoxSideShort.EnableTooltips(enable, m_toolTip);

	m_CheckBoxDirectionClose.EnableTooltips(enable, m_toolTip);
	m_CheckBoxDirectionOpen.EnableTooltips(enable, m_toolTip);
	m_CheckBoxDirectionAdd.EnableTooltips(enable, m_toolTip);

	m_ListBoxOrder.EnableTooltips(enable, m_toolTip);

	m_CheckBoxTifOnOpen.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTifOnClose.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTifDay.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTifExtended.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTifTime.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTifIoc.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTifFok.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTifGtc.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTifGtt.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTifExtendedTradingClose.EnableTooltips(enable, m_toolTip);

	m_CheckBoxOrderTypeAlgo.EnableTooltips(enable, m_toolTip);
	m_CheckBoxOrderTypeLimit.EnableTooltips(enable, m_toolTip);
	m_CheckBoxOrderTypeMarket.EnableTooltips(enable, m_toolTip);
	m_CheckBoxOrderTypeStop.EnableTooltips(enable, m_toolTip);

	m_CheckBoxPqOpenYes.EnableTooltips(enable, m_toolTip);
	m_CheckBoxPqOpenNo.EnableTooltips(enable, m_toolTip);

	m_CheckBoxTCloseYes.EnableTooltips(enable, m_toolTip);
	m_CheckBoxTCloseNo.EnableTooltips(enable, m_toolTip);

	m_CheckBoxExcludeDestinations.EnableTooltips(enable, m_toolTip);
	m_ListBoxDestination.EnableTooltips(enable, m_toolTip);

	m_CheckBoxExcludeMnemonics.EnableTooltips(enable, m_toolTip);
	m_ListBoxMnemonics.EnableTooltips(enable, m_toolTip);
//ReplaceOrder
	m_CheckBoxReplaceOrder.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplaceNative.EnableTooltips(enable, m_toolTip);
	m_CheckBoxMarketOrder.EnableTooltips(enable, m_toolTip);

	m_SpinReplacePriceOffset.EnableTooltips(enable, m_toolTip);
	m_SpinReplaceSize.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplacePriceOffsetPercent.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplacePriceEnforceMinOffset.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplacePriceReverseForSell.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplaceSizeOffsetPercent.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplaceSizeReverseForSell.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplaceAdjustable.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplaceAdjustableUseMmBox.EnableTooltips(enable, m_toolTip);

	m_CheckBoxPreBorrow.EnableTooltips(enable, m_toolTip);
	m_SpinBorrowPrice.EnableTooltips(enable, m_toolTip);

	m_SpinDisplaySizeFraction.EnableTooltips(enable, m_toolTip);
	m_SpinDisplaySizeRoundLot.EnableTooltips(enable, m_toolTip);

	RadioOwnerDraw* displaySizeCursor = m_radioDisplaySizeMode;
	for(unsigned int i = 0; i <= OCDSM_COUNT; ++i, ++displaySizeCursor)
	{
		displaySizeCursor->EnableTooltips(enable, m_toolTip);
	}
////
}

void SampleDialogSettingCancel::InitControls()
{
	BaseSampleDialogSettingOption::InitControls();
	m_RadioAccountCurrent.SetCheck(BST_CHECKED);
	m_RadioPositionCurrent.SetCheck(BST_CHECKED);

	m_ListBoxOrder.AddString("ALL");
	m_ListBoxOrder.AddString("Oldest");
	m_ListBoxOrder.AddString("Youngest");
	m_ListBoxOrder.AddString("Most Aggressive");
	m_ListBoxOrder.AddString("Least Aggressive");

	m_ListBoxOrder.SetCurSel(0);

	InvalidateOnOrderTypeStop();

	const TakionMainWnd* takionMainWnd = TGUI_GetTakionMainWnd();
	const unsigned int stockLoanFirmId = takionMainWnd->GetCurrentStockLoanFirmId();
	TakionIterator* iterator = takionMainWnd->CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		if(!destination->isBorrow() && takionMainWnd->isDestinationValid(destination->GetId(), stockLoanFirmId))
		{
			m_ListBoxDestination.AddString((const char*)destination);
		}
	}
	delete iterator;

	m_CheckBoxExcludeDestinations.SetCheck(m_excludeDestinations ? BST_CHECKED : BST_UNCHECKED);
	OnExcludeDestinations();

	m_CheckBoxExcludeMnemonics.SetCheck(m_excludeMnemonics ? BST_CHECKED : BST_UNCHECKED);
	OnExcludeMnemonics();

	m_displaySizeMode = OCDSM_COUNT;
	m_radioDisplaySizeMode[m_displaySizeMode].SetCheck(BST_CHECKED);
}

BOOL SampleDialogSettingCancel::OnInitDialog()
{
/*
	TD_GetAdminObservable().AddObserver(this);
	Observable* storyObservable = m_newsDll->GetStoryObservable();
	if(storyObservable)
	{
		storyObservable->AddObserver(this);
	}

	ResetOffsets();
*/
	NullifyValues();

	m_SpinClientId.SetBkColorEnabled(m_visualBase->GetGrayColor());

	BOOL ret = BaseSampleDialogSettingOption::OnInitDialog();

	m_EditMnemonic.LimitText(sizeof(MnemonicType));

	m_CheckBoxClientId.ResizeToFitText(0, false);

	m_RadioAccountCurrent.ResizeToFitText(0, false);
	m_RadioAccountAll.ResizeToFitText(0, false);

	m_RadioPositionCurrent.ResizeToFitText(0, false);
	m_RadioPositionEach.ResizeToFitText(0, false);
	m_RadioPositionAll.ResizeToFitText(0, false);

#ifndef TAKION_NO_OPTIONS
	m_CheckBoxInstrumentStock.ResizeToFitText(0, false);
	m_CheckBoxInstrumentOption.ResizeToFitText(0, false);
	m_CheckBoxInstrumentFuture.ResizeToFitText(0, false);
#endif

	m_CheckBoxSideBuy.ResizeToFitText(0, false);
	m_CheckBoxSideSell.ResizeToFitText(0, false);
	m_CheckBoxSideShort.ResizeToFitText(0, false);

	m_CheckBoxDirectionClose.ResizeToFitText(0, false);
	m_CheckBoxDirectionOpen.ResizeToFitText(0, false);
	m_CheckBoxDirectionAdd.ResizeToFitText(0, false);

	m_CheckBoxTifOnOpen.ResizeToFitText(0, false);
	m_CheckBoxTifOnClose.ResizeToFitText(0, false);
	m_CheckBoxTifDay.ResizeToFitText(0, false);
	m_CheckBoxTifExtended.ResizeToFitText(0, false);
	m_CheckBoxTifTime.ResizeToFitText(0, false);
	m_CheckBoxTifIoc.ResizeToFitText(0, false);
	m_CheckBoxTifFok.ResizeToFitText(0, false);
	m_CheckBoxTifGtc.ResizeToFitText(0, false);
	m_CheckBoxTifGtt.ResizeToFitText(0, false);
	m_CheckBoxTifExtendedTradingClose.ResizeToFitText(0, false);

	m_CheckBoxOrderTypeAlgo.ResizeToFitText(0, false);
	m_CheckBoxOrderTypeLimit.ResizeToFitText(0, false);
	m_CheckBoxOrderTypeMarket.ResizeToFitText(0, false);
	m_CheckBoxOrderTypeStop.ResizeToFitText(0, false);

	m_CheckBoxPqOpenYes.ResizeToFitText(0, false);
	m_CheckBoxPqOpenNo.ResizeToFitText(0, false);

	m_CheckBoxTCloseYes.ResizeToFitText(0, false);
	m_CheckBoxTCloseNo.ResizeToFitText(0, false);

	m_CheckBoxExcludeDestinations.ResizeToFitText(0, false);
	m_CheckBoxExcludeMnemonics.ResizeToFitText(0, false);

	m_CheckBoxReplaceOrder.ResizeToFitText(0, false);
	m_CheckBoxReplaceNative.ResizeToFitText(0, false);
	m_CheckBoxMarketOrder.ResizeToFitText(0, false);

	m_CheckBoxReplacePriceOffsetPercent.ResizeToFitText(0, false);
	m_CheckBoxReplacePriceEnforceMinOffset.ResizeToFitText(0, false);
	m_CheckBoxReplacePriceReverseForSell.ResizeToFitText(0, false);

	m_CheckBoxReplaceSizeOffsetPercent.ResizeToFitText(0, false);
	m_CheckBoxReplaceSizeReverseForSell.ResizeToFitText(0, false);
	m_CheckBoxReplaceAdjustable.ResizeToFitText(0, false);
	m_CheckBoxReplaceAdjustableUseMmBox.ResizeToFitText(0, false);

	RadioOwnerDraw* displaySizeCursor = m_radioDisplaySizeMode;
	for(unsigned int i = 0; i <= OCDSM_COUNT; ++i, ++displaySizeCursor)
	{
		displaySizeCursor->ResizeToFitText(0, true);
	}

	const COLORREF color = m_visualBase->GetWarningColor();
#ifndef TAKION_NO_OPTIONS
	DoUpdateInstrumentColors(color);
#endif
	DoUpdateSideColors(color);
	DoUpdateDirectionColors(color);
	DoUpdateTifColors(color);
	DoUpdateOrderTypeColors(color);

	m_CheckBoxPqOpenYes.SetForegroundColor(color);
	m_CheckBoxPqOpenNo.SetForegroundColor(color);

	m_CheckBoxTCloseYes.SetForegroundColor(color);
	m_CheckBoxTCloseNo.SetForegroundColor(color);

	const COLORREF grayColor = m_visualBase->GetGrayColor();
	m_SpinReplacePriceOffset.SetBkColorEnabled(grayColor);
	m_SpinReplaceSize.SetBkColorEnabled(grayColor);
	m_SpinBorrowPrice.SetBkColorEnabled(grayColor);
	m_SpinDisplaySizeFraction.SetBkColorEnabled(grayColor);
	m_SpinDisplaySizeRoundLot.SetBkColorEnabled(grayColor);

	const bool notReplaceOrder = !m_replaceOrder;
	m_CheckBoxReplaceNative.SetInvalid(notReplaceOrder);
	m_CheckBoxMarketOrder.SetInvalid(notReplaceOrder);
	m_CheckBoxReplacePriceOffsetPercent.SetInvalid(notReplaceOrder);
	m_CheckBoxReplacePriceEnforceMinOffset.SetInvalid(notReplaceOrder || m_marketOrder || m_replacePriceOffset.isZero());

	m_CheckBoxReplacePriceReverseForSell.SetInvalid(notReplaceOrder);
	m_CheckBoxReplaceSizeOffsetPercent.SetInvalid(notReplaceOrder);
	m_CheckBoxReplaceSizeReverseForSell.SetInvalid(notReplaceOrder);
	m_CheckBoxReplaceAdjustable.SetInvalid(notReplaceOrder);
	m_CheckBoxReplaceAdjustableUseMmBox.SetInvalid(notReplaceOrder);
	m_CheckBoxPreBorrow.SetInvalid(notReplaceOrder);
	RadioOwnerDraw* cursor = m_radioDisplaySizeMode;
	for(unsigned char i = 0; i <= OCDSM_COUNT; ++i, ++cursor)
	{
		cursor->SetInvalid(notReplaceOrder);
	}

	InvalidateControlsOnReplaceNative();

	m_groupBoxReplaceOrder.Adjust();

	return ret;
}

void SampleDialogSettingCancel::DoDataExchange(CDataExchange* pDX)
{
	BaseSampleDialogSettingOption::DoDataExchange(pDX);
	DDX_Control(pDX, buttonCancelOrderId, m_ButtonCancelOrder);
	DDX_Control(pDX, buttonCheckAllId, m_ButtonCheckAll);
	DDX_Control(pDX, buttonUncheckAllId, m_ButtonUncheckAll);

	DDX_Control(pDX, spinClientIdId, m_SpinClientId);
	DDX_Control(pDX, checkBoxClientIdId, m_CheckBoxClientId);

	DDX_Control(pDX, radioAccountCurrentId, m_RadioAccountCurrent);
	DDX_Control(pDX, radioAccountAllId, m_RadioAccountAll);

	DDX_Control(pDX, radioPositionCurrentId, m_RadioPositionCurrent);
	DDX_Control(pDX, radioPositionEachId, m_RadioPositionEach);
	DDX_Control(pDX, radioPositionAllId, m_RadioPositionAll);

#ifndef TAKION_NO_OPTIONS
	DDX_Control(pDX, checkBoxInstrumentStockId, m_CheckBoxInstrumentStock);
	DDX_Control(pDX, checkBoxInstrumentOptionId, m_CheckBoxInstrumentOption);
	DDX_Control(pDX, checkBoxInstrumentFutureId, m_CheckBoxInstrumentFuture);
#endif
	DDX_Control(pDX, checkBoxSideBuyId, m_CheckBoxSideBuy);
	DDX_Control(pDX, checkBoxSideSellId, m_CheckBoxSideSell);
	DDX_Control(pDX, checkBoxSideShortId, m_CheckBoxSideShort);

	DDX_Control(pDX, checkBoxDirectionCloseId, m_CheckBoxDirectionClose);
	DDX_Control(pDX, checkBoxDirectionOpenId, m_CheckBoxDirectionOpen);
	DDX_Control(pDX, checkBoxDirectionAddId, m_CheckBoxDirectionAdd);

	DDX_Control(pDX, listBoxPickOrderId, m_ListBoxOrder);

	DDX_Control(pDX, checkBoxTifOnOpenId, m_CheckBoxTifOnOpen);
	DDX_Control(pDX, checkBoxTifOnCloseId, m_CheckBoxTifOnClose);
	DDX_Control(pDX, checkBoxTifDayId, m_CheckBoxTifDay);
	DDX_Control(pDX, checkBoxTifExtendedId, m_CheckBoxTifExtended);
	DDX_Control(pDX, checkBoxTifTimeId, m_CheckBoxTifTime);
	DDX_Control(pDX, checkBoxTifIocId, m_CheckBoxTifIoc);
	DDX_Control(pDX, checkBoxTifFokId, m_CheckBoxTifFok);
	DDX_Control(pDX, checkBoxTifGtcId, m_CheckBoxTifGtc);
	DDX_Control(pDX, checkBoxTifGttId, m_CheckBoxTifGtt);
	DDX_Control(pDX, checkBoxTifExtendedTradingCloseId, m_CheckBoxTifExtendedTradingClose);

	DDX_Control(pDX, checkBoxOrderTypeAlgoId, m_CheckBoxOrderTypeAlgo);
	DDX_Control(pDX, checkBoxOrderTypeLimitId, m_CheckBoxOrderTypeLimit);
	DDX_Control(pDX, checkBoxOrderTypeMarketId, m_CheckBoxOrderTypeMarket);
	DDX_Control(pDX, checkBoxOrderTypeStopId, m_CheckBoxOrderTypeStop);

	DDX_Control(pDX, checkBoxPqOpenYesId, m_CheckBoxPqOpenYes);
	DDX_Control(pDX, checkBoxPqOpenNoId, m_CheckBoxPqOpenNo);

	DDX_Control(pDX, checkBoxTCloseYesId, m_CheckBoxTCloseYes);
	DDX_Control(pDX, checkBoxTCloseNoId, m_CheckBoxTCloseNo);

	DDX_Control(pDX, listBoxDestinationsId, m_ListBoxDestination);
	DDX_Control(pDX, checkBoxExcludeDestinationsId, m_CheckBoxExcludeDestinations);
	DDX_Control(pDX, staticDestinationCountId, m_StaticDestinationCount);

	DDX_Control(pDX, listBoxMnemonicsId, m_ListBoxMnemonics);
	DDX_Control(pDX, checkBoxExcludeMnemonicsId, m_CheckBoxExcludeMnemonics);

	DDX_Control(pDX, editCancelOrderMnemonicId, m_EditMnemonic);
	DDX_Control(pDX, buttonMnemonicFindId, m_ButtonMnemonicFind);
	DDX_Control(pDX, buttonMnemonicAddId, m_ButtonMnemonicAdd);
	DDX_Control(pDX, buttonMnemonicRemoveId, m_ButtonMnemonicRemove);
	DDX_Control(pDX, buttonMnemonicClearId, m_ButtonMnemonicClear);

//Replace
	DDX_Control(pDX, groupReplaceOrderDisplaySizeId, m_GroupDisplaySize);
	DDX_Control(pDX, checkBoxReplaceOrderId, m_CheckBoxReplaceOrder);
	DDX_Control(pDX, checkBoxReplaceOrderNativeId, m_CheckBoxReplaceNative);
	DDX_Control(pDX, checkBoxReplaceOrderMarketId, m_CheckBoxMarketOrder);

	DDX_Control(pDX, staticReplaceOrderPriceOffsetId, m_StaticReplacePrice);
	DDX_Control(pDX, staticReplaceOrderSizeOffsetId, m_StaticReplaceSize);
	DDX_Control(pDX, spinReplaceOrderPriceOffsetId, m_SpinReplacePriceOffset);
	DDX_Control(pDX, checkBoxReplaceOrderPriceOffsetPercentId, m_CheckBoxReplacePriceOffsetPercent);
	DDX_Control(pDX, checkBoxReplaceOrderPriceEnforceMinOffsetId, m_CheckBoxReplacePriceEnforceMinOffset);

	DDX_Control(pDX, checkBoxReplaceOrderSizeOffsetPercentId, m_CheckBoxReplaceSizeOffsetPercent);

	DDX_Control(pDX, checkBoxReplaceOrderPriceOffsetReverseForSellId, m_CheckBoxReplacePriceReverseForSell);
	DDX_Control(pDX, spinReplaceOrderSizeOffsetId, m_SpinReplaceSize);
	DDX_Control(pDX, checkBoxReplaceOrderSizeOffsetReverseForSellId, m_CheckBoxReplaceSizeReverseForSell);

	DDX_Control(pDX, checkBoxReplaceOrderPreBorrowId, m_CheckBoxPreBorrow);
	DDX_Control(pDX, spinReplaceOrderPreBorrowId, m_SpinBorrowPrice);

//	DDX_Control(pDX, staticReplaceOrderDisplaySizeId, m_StaticDisplaySize);

	DDX_Control(pDX, radioReplaceOrderDisplaySizeNoChangeId, m_radioDisplaySizeMode[OCDSM_COUNT]);
	DDX_Control(pDX, radioReplaceOrderDisplaySizeFractionId, m_radioDisplaySizeMode[OCDSM_SIZE_FRACTION]);
	DDX_Control(pDX, radioReplaceOrderDisplaySizeRoundLotId, m_radioDisplaySizeMode[OCDSM_ROUND_LOT]);

	DDX_Control(pDX, spinReplaceOrderDisplaySizeFractionId, m_SpinDisplaySizeFraction);
	DDX_Control(pDX, spinReplaceOrderDisplaySizeRoundLotId, m_SpinDisplaySizeRoundLot);

	DDX_Control(pDX, checkBoxReplaceOrderAdjustableId, m_CheckBoxReplaceAdjustable);
	DDX_Control(pDX, checkBoxReplaceOrderAdjustableMmBoxId, m_CheckBoxReplaceAdjustableUseMmBox);
//CancelReplace Orders
	DDX_Control(pDX, groupCancelReplaceOrdersAllId, m_GroupCancelReplaceOrdersAll);
////
	AddCustomGroupBox(&m_groupBoxReplaceOrder);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingCancel, BaseSampleDialogSettingOption)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(buttonCancelOrderId, OnCancelOrder)

	ON_BN_CLICKED(buttonCheckAllId, OnCheckAll)
	ON_BN_CLICKED(buttonUncheckAllId, OnUncheckAll)

	ON_BN_CLICKED(checkBoxClientIdId, OnClientId)

	ON_BN_CLICKED(radioAccountCurrentId, OnAccountCurrent)
	ON_BN_CLICKED(radioAccountAllId, OnAccountAll)

	ON_BN_CLICKED(radioPositionCurrentId, OnPositionCurrent)
	ON_BN_CLICKED(radioPositionEachId, OnPositionEach)
	ON_BN_CLICKED(radioPositionAllId, OnPositionAll)

#ifndef TAKION_NO_OPTIONS
	ON_BN_CLICKED(checkBoxInstrumentStockId, OnInstrumentStock)
	ON_BN_CLICKED(checkBoxInstrumentOptionId, OnInstrumentOption)
	ON_BN_CLICKED(checkBoxInstrumentFutureId, OnInstrumentFuture)
#endif

	ON_BN_CLICKED(checkBoxSideBuyId, OnSideBuy)
	ON_BN_CLICKED(checkBoxSideSellId, OnSideSell)
	ON_BN_CLICKED(checkBoxSideShortId, OnSideShort)

	ON_BN_CLICKED(checkBoxDirectionCloseId, OnDirectionClose)
	ON_BN_CLICKED(checkBoxDirectionOpenId, OnDirectionOpen)
	ON_BN_CLICKED(checkBoxDirectionAddId, OnDirectionAdd)

	ON_LBN_SELCHANGE(listBoxPickOrderId, OnSelchangeOrder)

	ON_BN_CLICKED(checkBoxTifOnOpenId, OnTifOnOpen)
	ON_BN_CLICKED(checkBoxTifOnCloseId, OnTifOnClose)
	ON_BN_CLICKED(checkBoxTifDayId, OnTifDay)
	ON_BN_CLICKED(checkBoxTifExtendedId, OnTifExtended)
	ON_BN_CLICKED(checkBoxTifTimeId, OnTifTime)
	ON_BN_CLICKED(checkBoxTifIocId, OnTifIoc)
	ON_BN_CLICKED(checkBoxTifFokId, OnTifFok)
	ON_BN_CLICKED(checkBoxTifGtcId, OnTifGtc)
	ON_BN_CLICKED(checkBoxTifGttId, OnTifGtt)
	ON_BN_CLICKED(checkBoxTifExtendedTradingCloseId, OnTifExtendedTradingCloseClose)

	ON_BN_CLICKED(checkBoxOrderTypeAlgoId, OnOrderTypeAlgo)
	ON_BN_CLICKED(checkBoxOrderTypeLimitId, OnOrderTypeLimit)
	ON_BN_CLICKED(checkBoxOrderTypeMarketId, OnOrderTypeMarket)
	ON_BN_CLICKED(checkBoxOrderTypeStopId, OnOrderTypeStop)

	ON_BN_CLICKED(checkBoxPqOpenYesId, OnPqOpenYes)
	ON_BN_CLICKED(checkBoxPqOpenNoId, OnPqOpenNo)

	ON_BN_CLICKED(checkBoxTCloseYesId, OnTCloseYes)
	ON_BN_CLICKED(checkBoxTCloseNoId, OnTCloseNo)

	ON_BN_CLICKED(checkBoxExcludeDestinationsId, OnExcludeDestinations)
	ON_BN_CLICKED(checkBoxExcludeMnemonicsId, OnExcludeMnemonics)

	ON_LBN_SELCHANGE(listBoxMnemonicsId, OnSelchangeMnemonic)
	ON_LBN_DBLCLK(listBoxMnemonicsId, OnDblClkMnemonic)

	ON_EN_CHANGE(editCancelOrderMnemonicId, OnMnemonicChange)
	ON_BN_CLICKED(buttonMnemonicFindId, OnMnemonicFind)
	ON_BN_CLICKED(buttonMnemonicAddId, OnMnemonicAdd)
	ON_BN_CLICKED(buttonMnemonicRemoveId, OnMnemonicRemove)
	ON_BN_CLICKED(buttonMnemonicClearId, OnMnemonicClear)

//Replace
	ON_BN_CLICKED(checkBoxReplaceOrderId, OnReplaceOrder)
	ON_BN_CLICKED(checkBoxReplaceOrderNativeId, OnReplaceNative)
	ON_BN_CLICKED(checkBoxReplaceOrderMarketId, OnMarketOrder)
	ON_BN_CLICKED(checkBoxReplaceOrderPriceOffsetPercentId, OnReplacePriceOffsetPercent)
	ON_BN_CLICKED(checkBoxReplaceOrderPriceEnforceMinOffsetId, OnReplacePriceEnforceMinOffset)
	ON_BN_CLICKED(checkBoxReplaceOrderPriceOffsetReverseForSellId, OnReplacePriceOffsetReverseForSell)
	ON_BN_CLICKED(checkBoxReplaceOrderSizeOffsetPercentId, OnReplaceSizeOffsetPercent)
	ON_BN_CLICKED(checkBoxReplaceOrderSizeOffsetReverseForSellId, OnReplaceSizeOffsetReverseForSell)

	ON_BN_CLICKED(checkBoxReplaceOrderPreBorrowId, OnPreBorrow)

	ON_BN_CLICKED(checkBoxReplaceOrderAdjustableId, OnAdjustable)
	ON_BN_CLICKED(checkBoxReplaceOrderAdjustableMmBoxId, OnAdjustableMmBox)

	ON_BN_CLICKED(radioReplaceOrderDisplaySizeNoChangeId, OnDisplaySizeNoChange)
	ON_BN_CLICKED(radioReplaceOrderDisplaySizeFractionId, OnDisplaySizeFraction)
	ON_BN_CLICKED(radioReplaceOrderDisplaySizeRoundLotId, OnDisplaySizeRoundLot)
////
////
	ON_BN_CLICKED(buttonCancelReplaceOrdersAllId, OnCancelReplaceOrdersAll)
	ON_BN_CLICKED(buttonCancelReplaceOrdersBuyId, OnCancelReplaceOrdersBuy)
	ON_BN_CLICKED(buttonCancelReplaceOrdersSellShortLimitId, OnCancelReplaceOrdersSellShortLimit)
	ON_BN_CLICKED(buttonCancelReplaceOrdersShortStopId, OnCancelReplaceOrdersShortStop)
	ON_BN_CLICKED(buttonCancelReplaceOrdersLimitNsdqDayExtId, OnCancelReplaceOrdersLimitNsdqDayExt)
	ON_BN_CLICKED(buttonCancelReplaceOrdersAllOpeningId, OnCancelReplaceOrdersAllOpening)
	ON_BN_CLICKED(buttonCancelReplaceOrdersSellShortClosingId, OnCancelReplaceOrdersSellShortClosing)
////
	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)//one item is checked/unchecked
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//multiple items checked/unchecked

	ON_MESSAGE(WM_USER + 21, OnListBoxRightButtonSelection)
	ON_MESSAGE(WM_USER + 22, OnListBoxItemsCleared)
	ON_MESSAGE(WM_USER + 123, OnListBoxItemRemoved)
	ON_MESSAGE(WM_USER + 24, OnListBoxAddItem)
	ON_MESSAGE(WM_USER + 124, OnListBoxFindItem)
	ON_MESSAGE(WM_USER + 25, OnListBoxItemsPastedFromClipboard)

	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingCancel::FillCancelReplaceParams(SignedPrice& replacePriceOffset,
	Price& preBorrowPrice,
	int& replaceSizeOffset,
	unsigned int& displaySize,
	unsigned int& displaySizeFraction,
	unsigned __int64& numericSymbol,
#ifndef TAKION_NO_OPTIONS
	unsigned __int64& optionBlock,
	bool& optionSettingsValid,
	unsigned char& securityType,
#endif
	bool& validSymbol) const
{
	if(m_replaceOrder)
	{
		FillSignedPriceFromSpinControl(replacePriceOffset, m_SpinReplacePriceOffset, 10000);
		FillPriceFromSpinControl(preBorrowPrice, m_SpinBorrowPrice, 10000);
		FillDisplaySize(m_displaySizeMode, displaySize, displaySizeFraction);
	}

	replaceSizeOffset = m_SpinReplaceSize.GetValue();

	validSymbol = !m_symbol.empty();
	numericSymbol = validSymbol ? U_RepresentStringAsUnsignedNumber<unsigned __int64>(m_symbol.c_str(), sizeof(unsigned __int64) - 1) : 0;
#ifndef TAKION_NO_OPTIONS
	const bool futureSymbol = validSymbol && TU_IsFutureNumericSymbol(numericSymbol) != 0;
	unsigned __int64 underlierSymbol;
//	unsigned __int64 optionBlock = 0;
	unsigned __int64 rootSymbol;//should be the same as numericSymbol
	unsigned short optionType;
	optionSettingsValid = GetOptionValues(rootSymbol, underlierSymbol, optionBlock, optionType);
	securityType = optionSettingsValid && optionBlock ? ST_OPTION :
		futureSymbol ? ST_FUTURE :
		ST_STOCK;
#endif
}

void SampleDialogSettingCancel::CancelReplaceOrdersAll(unsigned char sideMask,
	unsigned char directionMask,
#ifndef TAKION_NO_OPTIONS
	unsigned char instrumentMask,
#endif
	OrderCancelPick orderCancelPick,
	unsigned int tifMask,
	unsigned char orderTypeMask,
	unsigned char pqOpenMask,
	unsigned char tCloseMask,
	bool excludeDestinations,
	const std::set<unsigned int>* destinationSet,
	bool excludeMnemonics,
	const std::set<unsigned int>* mnemonicSet)
{
	SignedPrice replacePriceOffset;
	Price preBorrowPrice;
	unsigned int displaySize = 0;
	unsigned int displaySizeFraction = 0;
	int replaceSizeOffset = 0;
	bool validSymbol = false;
	unsigned __int64 numericSymbol = 0;
#ifndef TAKION_NO_OPTIONS
	unsigned __int64 optionBlock = 0;
	bool optionSettingsValid = false;
	unsigned char securityType = ST_STOCK;
#endif
	FillCancelReplaceParams(replacePriceOffset,
		preBorrowPrice,
		replaceSizeOffset,
		displaySize,
		displaySizeFraction,
		numericSymbol,
#ifndef TAKION_NO_OPTIONS
		optionBlock,
		optionSettingsValid,
		securityType,
#endif
		validSymbol);

	Order* oneOrder = nullptr;
	Order** oneOrderPtr = m_adjustableUseMmBox && m_replaceOrder ? &oneOrder : nullptr;
	OrderReplaceSet orderSet;
	OrderReplaceSet* orderSetPtr = m_adjustable && m_replaceOrder ? &orderSet : nullptr;
	unsigned int orderCount = 0;
	Account* account;

	if(m_accountAll && TD_GetAccountCount() > 1)
	{
		TakionIterator* iterator = TD_CreateAccountIterator();
		TD_LockAccountStorage();
		iterator->Reset();
		while(account = TD_GetNextAccount(iterator))
		{
			if(m_positionType != 0)//all or each
			{
				if(m_replaceOrder)
				{
					orderCount += account->ReplaceOrders(m_positionType == 1,//each
						m_replaceNative,
						sideMask,
						directionMask,
#ifndef TAKION_NO_OPTIONS
						instrumentMask,
#endif
						orderCancelPick,
						tifMask,
						orderTypeMask,
						pqOpenMask,
						tCloseMask,
						excludeDestinations,
						destinationSet,
						excludeMnemonics,
						mnemonicSet,
						replacePriceOffset,
						m_priceOffsetPercent,
						m_priceEnforceMinOffset,
						m_priceOffsetReverseForSell,
						m_marketOrder,
						replaceSizeOffset,
						m_sizeOffsetPercent,
						m_sizeOffsetReverseForSell,
						m_preBorrow,
						preBorrowPrice,
						0xFFFFFFFFFFFFFFFF,//0,
						m_displaySizeMode,
						displaySize,
						displaySizeFraction,
						oneOrderPtr,
						orderSetPtr);
				}
				else
				{
					orderCount += account->CancelOrders(m_positionType == 1,//each
						sideMask,
						directionMask,
#ifndef TAKION_NO_OPTIONS
						instrumentMask,
#endif
						orderCancelPick,
						tifMask,
						orderTypeMask,
						pqOpenMask,
						tCloseMask,
						excludeDestinations,
						destinationSet,
						excludeMnemonics,
						mnemonicSet);
				}
			}
			else if(validSymbol
#ifndef TAKION_NO_OPTIONS
				&& optionSettingsValid
#endif
				)//current Position (symbol in the control)
			{
				Position* position =
#ifndef TAKION_NO_OPTIONS
					account->FindPositionWithLock(numericSymbol, optionBlock, securityType);
#else
					account->FindStockPositionWithLock(numericSymbol);
#endif
				if(position)
				{
					if(m_replaceOrder)
					{
						orderCount += position->ReplaceOrders(m_replaceNative,
							sideMask,
							directionMask,
							orderCancelPick,
							tifMask,
							orderTypeMask,
							pqOpenMask,
							tCloseMask,
							excludeDestinations,
							destinationSet,
							excludeMnemonics,
							mnemonicSet,

							replacePriceOffset,
							m_priceOffsetPercent,
							m_priceEnforceMinOffset,
							m_priceOffsetReverseForSell,
							m_marketOrder,
							replaceSizeOffset,
							m_sizeOffsetPercent,
							m_sizeOffsetReverseForSell,
							m_preBorrow,
							preBorrowPrice,
							0xFFFFFFFFFFFFFFFF,//0,
							m_displaySizeMode,
							displaySize,
							displaySizeFraction,
							oneOrderPtr,
							orderSetPtr);
					}
					else
					{
						orderCount += position->CancelOrders(sideMask,
							directionMask,
							orderCancelPick,
							tifMask,
							orderTypeMask,
							pqOpenMask,
							tCloseMask,
							excludeDestinations,
							destinationSet,
							excludeMnemonics,
							mnemonicSet);
					}
				}
			}
		}
		TD_UnlockAccountStorage();
		delete iterator;
	}
	else if(m_account)
	{
		if(m_positionType != 0)//all or each
		{
			if(m_replaceOrder)
			{
				orderCount += m_account->ReplaceOrders(m_positionType == 1,//each
					m_replaceNative,
					sideMask,
					directionMask,
#ifndef TAKION_NO_OPTIONS
					instrumentMask,
#endif
					orderCancelPick,
					tifMask,
					orderTypeMask,
					pqOpenMask,
					tCloseMask,
					excludeDestinations,
					destinationSet,
					excludeMnemonics,
					mnemonicSet,
					replacePriceOffset,
					m_priceOffsetPercent,
					m_priceEnforceMinOffset,
					m_priceOffsetReverseForSell,
					m_marketOrder,
					replaceSizeOffset,
					m_sizeOffsetPercent,
					m_sizeOffsetReverseForSell,
					m_preBorrow,
					preBorrowPrice,
					0xFFFFFFFFFFFFFFFF,//0,
					m_displaySizeMode,
					displaySize,
					displaySizeFraction,
					oneOrderPtr,
					orderSetPtr);
			}
			else
			{
				orderCount += m_account->CancelOrders(m_positionType == 1,//each
					sideMask,
					directionMask,
#ifndef TAKION_NO_OPTIONS
					instrumentMask,
#endif
					orderCancelPick,
					tifMask,
					orderTypeMask,
					pqOpenMask,
					tCloseMask,
					excludeDestinations,
					destinationSet,
					excludeMnemonics,
					mnemonicSet);
			}
		}
		else if(validSymbol
#ifndef TAKION_NO_OPTIONS
			&& optionSettingsValid
#endif
			)//current Position (symbol in the control)
		{
			Position* position =
#ifndef TAKION_NO_OPTIONS
				m_account->FindPositionWithLock(numericSymbol, optionBlock, securityType);
#else
				m_account->FindStockPositionWithLock(numericSymbol);
#endif
			if(position)
			{
				if(m_replaceOrder)
				{
					orderCount += position->ReplaceOrders(m_replaceNative,
						sideMask,
						directionMask,
						orderCancelPick,
						tifMask,
						orderTypeMask,
						pqOpenMask,
						tCloseMask,
						excludeDestinations,
						destinationSet,
						excludeMnemonics,
						mnemonicSet,
						replacePriceOffset,
						m_priceOffsetPercent,
						m_priceEnforceMinOffset,
						m_priceOffsetReverseForSell,
						m_marketOrder,
						replaceSizeOffset,
						m_sizeOffsetPercent,
						m_sizeOffsetReverseForSell,
						m_preBorrow,
						preBorrowPrice,
						0xFFFFFFFFFFFFFFFF,//0,
						m_displaySizeMode,
						displaySize,
						displaySizeFraction,
						oneOrderPtr,
						orderSetPtr);
				}
				else
				{
					orderCount += position->CancelOrders(sideMask,
						directionMask,
						orderCancelPick,
						tifMask,
						orderTypeMask,
						pqOpenMask,
						tCloseMask,
						excludeDestinations,
						destinationSet,
						excludeMnemonics,
						mnemonicSet);
				}
			}
		}
	}
	AdjustReplacement(oneOrder,
		orderSet,
		replacePriceOffset,
		preBorrowPrice,
		displaySize,
		displaySizeFraction,
		replaceSizeOffset);
}

void SampleDialogSettingCancel::OnCancelReplaceOrdersAll()
{
	CancelReplaceOrdersAll((1 << TU_ORDER_SIDE_COUNT) - 1,//m_sideMask,
		ODIR_LIVE_MASK,//m_directionMask,
#ifndef TAKION_NO_OPTIONS
		ST_MASK_ALL,//m_instrumentMask,
#endif
		TU_ORDER_CANCEL_ALL,
		TIFMASK_ALL & ~(1 << TIFMASK_OTHER),//m_tifMask,
		ALGORITHMMASK_ALL,//m_orderTypeMask,
		YNM_ALL,//m_pqOpenMask,
		YNM_ALL,//m_tCloseMask,
		true,//m_excludeDestinations,
		nullptr,//&destinationSet,
		true,//m_excludeMnemonics,
		nullptr);//&mnemonicSet,
}

void SampleDialogSettingCancel::AdjustReplacement(Order* oneOrder,
	const OrderReplaceSet& orderSet,
	const SignedPrice& replacePriceOffset,
	const Price& preBorrowPrice,
	const unsigned int displaySize,
	const unsigned int displaySizeFraction,
	const int replaceSizeOffset)
{
	TakionMainWnd* takionMainWnd = TGUI_GetTakionMainWnd();
	if(oneOrder)
	{
		if(oneOrder->CanBeReplaced() && !oneOrder->isDeleted())
		{
			if(oneOrder->isAlgorithm())//|| !m_adjustableOrderBox
			{
				const bool sell = oneOrder->GetSide() != 'B';
				oneOrder->ReplacePriceAndSize(nullptr,
					nullptr,//const Security*
					m_replaceNative,
					m_marketOrder,
					sell && m_priceOffsetReverseForSell ? -replacePriceOffset : replacePriceOffset,
					m_priceOffsetPercent,
					m_priceEnforceMinOffset,
					sell && m_sizeOffsetReverseForSell ? -replaceSizeOffset : replaceSizeOffset,
					m_sizeOffsetPercent,
					m_preBorrow,
					preBorrowPrice,
					0xFFFFFFFFFFFFFFFF,//0,//m_userId,
					m_displaySizeMode,//0 - size fraction, 1 - round lot, 2 - no change
					displaySize,
					displaySizeFraction,
					0);//remainigSize

			}
			else
			{
				int left = 0;
				int top = 0;
				if(mainDialog)
				{
					const CRect& rect = mainDialog->GetDialogFrameRect();
					left = rect.left;
					top = rect.top;
				}
				takionMainWnd->CancelReplaceOrderWithAdjustment(oneOrder,
					left,
					top,
					m_replaceNative,
					replacePriceOffset,
					m_priceOffsetPercent,
					m_priceEnforceMinOffset,
					m_priceOffsetReverseForSell,
					m_marketOrder,//order->isMarketOrder(),
					replaceSizeOffset,
					m_sizeOffsetPercent,
					m_sizeOffsetReverseForSell,
					m_preBorrow,
					preBorrowPrice,
					m_displaySizeMode,//0 - size fraction, 1 - round lot, 2 - no change
					displaySize,
					displaySizeFraction);
			}
		}
	}
	else if(!orderSet.empty())
	{
		CPoint corner(0, 0);
		if(mainDialog)
		{
			const CRect& rect = mainDialog->GetDialogFrameRect();
			corner.x = rect.left;
			corner.y = rect.top;
		}
		DialogReplaceOrder* dlg = takionMainWnd->GetReplaceOrderDialog();
		if(dlg->isDisplayed() && dlg->isCancelPosted())
		{
			dlg->IncrementIgnoreCancelCount();
		}
		const unsigned int size = m_SpinDisplaySizeFraction.GetValue();
		const unsigned int displaySize = size / 10;
		const unsigned int displaySizeFraction = size - 10 * displaySize;
		const unsigned int displaySizeRoundLot = m_SpinDisplaySizeRoundLot.GetValue();

		dlg->Display(orderSet,
			!replacePriceOffset.isZero() || !replaceSizeOffset,//priceColumn,
			m_preBorrow,
			m_marketOrder,
			m_replaceNative,
			m_priceOffsetPercent,
			m_priceEnforceMinOffset,
			m_priceOffsetReverseForSell,
			m_sizeOffsetPercent,
			m_sizeOffsetReverseForSell,
			replacePriceOffset,
			preBorrowPrice,
			replaceSizeOffset,
			(OrderCommandDisplaySizeMode)m_displaySizeMode,
			displaySize,
			displaySizeFraction,
			displaySizeRoundLot,

			corner,
			0,
			this,
			corner);
	}
}

void SampleDialogSettingCancel::OnCancelReplaceOrdersBuy()
{
	CancelReplaceOrdersAll(1 << TU_ORDER_SIDE_BUY,//m_sideMask,
		ODIR_LIVE_MASK,//m_directionMask,
#ifndef TAKION_NO_OPTIONS
		ST_MASK_ALL,//m_instrumentMask,
#endif
		TU_ORDER_CANCEL_ALL,
		TIFMASK_ALL & ~(1 << TIFMASK_OTHER),//m_tifMask,
		ALGORITHMMASK_ALL,//m_orderTypeMask,
		YNM_ALL,//m_pqOpenMask,
		YNM_ALL,//m_tCloseMask,
		true,//m_excludeDestinations,
		nullptr,//&destinationSet,
		true,//m_excludeMnemonics,
		nullptr);//&mnemonicSet,
}

void SampleDialogSettingCancel::OnCancelReplaceOrdersSellShortLimit()
{
	CancelReplaceOrdersAll((1 << TU_ORDER_SIDE_SELL)|(1 << TU_ORDER_SIDE_SHORT),//m_sideMask,
		ODIR_LIVE_MASK,//m_directionMask,
#ifndef TAKION_NO_OPTIONS
		ST_MASK_ALL,//m_instrumentMask,
#endif
		TU_ORDER_CANCEL_ALL,
		TIFMASK_ALL & ~(1 << TIFMASK_OTHER),//m_tifMask,
		ALGORITHMMASK_ALL,//m_orderTypeMask,
		YNM_ALL,//m_pqOpenMask,
		YNM_ALL,//m_tCloseMask,
		true,//m_excludeDestinations,
		nullptr,//&destinationSet,
		true,//m_excludeMnemonics,
		nullptr);//&mnemonicSet,
}

void SampleDialogSettingCancel::OnCancelReplaceOrdersShortStop()
{
	CancelReplaceOrdersAll(1 << TU_ORDER_SIDE_SHORT,//m_sideMask,
		ODIR_LIVE_MASK,//m_directionMask,
#ifndef TAKION_NO_OPTIONS
		ST_MASK_ALL,//m_instrumentMask,
#endif
		TU_ORDER_CANCEL_ALL,
		TIFMASK_ALL & ~(1 << TIFMASK_OTHER),//m_tifMask,
		1 << ALGORITHMMASK_STOP,//m_orderTypeMask,
		YNM_ALL,//m_pqOpenMask,
		YNM_ALL,//m_tCloseMask,
		true,//m_excludeDestinations,
		nullptr,//&destinationSet,
		true,//m_excludeMnemonics,
		nullptr);//&mnemonicSet,
}

void SampleDialogSettingCancel::OnCancelReplaceOrdersLimitNsdqDayExt()
{
	std::set<unsigned int> destinationSet;
	destinationSet.insert(*(unsigned int*)"NSDQ");

	CancelReplaceOrdersAll((1 << TU_ORDER_SIDE_COUNT) - 1,//m_sideMask,
		ODIR_LIVE_MASK,//m_directionMask,
#ifndef TAKION_NO_OPTIONS
		ST_MASK_ALL,//m_instrumentMask,
#endif
		TU_ORDER_CANCEL_ALL,
		(1 << TIFMASK_DAY)|(1 << TIFMASK_EXTENDED),//m_tifMask,
		1 << ALGORITHMMASK_LIMIT,//m_orderTypeMask,
		YNM_ALL,//m_pqOpenMask,
		YNM_ALL,//m_tCloseMask,
		false,//m_excludeDestinations,
		&destinationSet,
		true,//m_excludeMnemonics,
		nullptr);//&mnemonicSet,
}

void SampleDialogSettingCancel::OnCancelReplaceOrdersAllOpening()
{
	CancelReplaceOrdersAll((1 << TU_ORDER_SIDE_COUNT) - 1,//m_sideMask,
		(1 << ODIR_OPEN)|(1 << ODIR_ADD),//m_directionMask,
#ifndef TAKION_NO_OPTIONS
		ST_MASK_ALL,//m_instrumentMask,
#endif
		TU_ORDER_CANCEL_ALL,
		TIFMASK_ALL & ~(1 << TIFMASK_OTHER),//m_tifMask,
		ALGORITHMMASK_ALL,//m_orderTypeMask,
		YNM_ALL,//m_pqOpenMask,
		YNM_ALL,//m_tCloseMask,
		true,//m_excludeDestinations,
		nullptr,//&destinationSet,
		true,//m_excludeMnemonics,
		nullptr);//&mnemonicSet,
}

void SampleDialogSettingCancel::OnCancelReplaceOrdersSellShortClosing()
{
	CancelReplaceOrdersAll((1 << TU_ORDER_SIDE_SELL)|(1 << TU_ORDER_SIDE_SHORT),//m_sideMask,
		1 << ODIR_CLOSE,//m_directionMask,
#ifndef TAKION_NO_OPTIONS
		ST_MASK_ALL,//m_instrumentMask,
#endif
		TU_ORDER_CANCEL_ALL,
		TIFMASK_ALL & ~(1 << TIFMASK_OTHER),//m_tifMask,
		ALGORITHMMASK_ALL,//m_orderTypeMask,
		YNM_ALL,//m_pqOpenMask,
		YNM_ALL,//m_tCloseMask,
		true,//m_excludeDestinations,
		nullptr,//&destinationSet,
		true,//m_excludeMnemonics,
		nullptr);//&mnemonicSet,
}

LRESULT SampleDialogSettingCancel::OnListBoxRightButtonSelection(WPARAM ctrlId, LPARAM l)
{
	OnSelchangeMnemonic();
	return 0;
}

LRESULT SampleDialogSettingCancel::OnListBoxItemsCleared(WPARAM ctrlId, LPARAM l)
{
	MnemonicsCleared();
	m_modified = true;
	return 0;
}

LRESULT SampleDialogSettingCancel::OnListBoxItemRemoved(WPARAM ctrlId, LPARAM l)
{
	MnemonicRemoved((unsigned int)l);
	m_modified = true;
	return 0;
}

LRESULT SampleDialogSettingCancel::OnListBoxItemsPastedFromClipboard(WPARAM ctrlId, LPARAM l)
{
	OnSelchangeMnemonic();
	if(m_ListBoxMnemonics.GetItemCount() > 1)EnableControl(m_ButtonMnemonicClear.m_hWnd, true, m_ListBoxMnemonics.m_hWnd);

	return 0;
}

LRESULT SampleDialogSettingCancel::OnListBoxAddItem(WPARAM ctrlId, LPARAM l)
{
	OnMnemonicAdd();
	return 0;
}

LRESULT SampleDialogSettingCancel::OnListBoxFindItem(WPARAM ctrlId, LPARAM l)
{
	OnMnemonicFind();
	return 0;
}

void SampleDialogSettingCancel::OnMnemonicChange()
{
	CString text;
	m_EditMnemonic.GetWindowText(text);
	m_mnemonic = U_StringToUInt(text);
	EnableControl(m_ButtonMnemonicAdd.m_hWnd, m_mnemonic > 0, m_ListBoxMnemonics.m_hWnd);
}

void SampleDialogSettingCancel::OnSelchangeMnemonic()
{
	const int sel = m_ListBoxMnemonics.GetCurSel();
	EnableControl(m_ButtonMnemonicRemove.m_hWnd, sel > 0, m_ListBoxMnemonics.m_hWnd);
}

void SampleDialogSettingCancel::OnDblClkMnemonic()
{
	const int sel = m_ListBoxMnemonics.GetCurSel();
	if(sel >= 0)
	{
		const MnemonicType val = (const MnemonicType)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			m_ListBoxMnemonics.GetItemDataPtr(sel);
		char buf[sizeof(MnemonicType) + 1];
		*(MnemonicType*)buf = val;
		buf[sizeof(MnemonicType)] = '\0';
		m_EditMnemonic.SetWindowText(buf);
	}
}

void SampleDialogSettingCancel::OnMnemonicFind()
{
	m_ListBoxMnemonics.SetCurSel(m_ListBoxMnemonics.FindItem((const void*)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		m_mnemonic));
	OnSelchangeMnemonic();
}

void SampleDialogSettingCancel::OnMnemonicAdd()
{
	unsigned char added;
	const int sel = m_ListBoxMnemonics.AddItemChecked(m_mnemonic, true, false, added);
	if(sel >= 0)
	{
		m_ListBoxMnemonics.SetCurSel(sel);
		OnSelchangeMnemonic();
		if(m_ListBoxMnemonics.GetItemCount() > 1)EnableControl(m_ButtonMnemonicClear.m_hWnd, true, m_ListBoxMnemonics.m_hWnd);
		m_modified = true;

//		EnableObjectUpdateButton();
	}
}

void SampleDialogSettingCancel::MnemonicRemoved(unsigned int i)
{
	const unsigned int count = m_ListBoxMnemonics.GetItemCount();
	if(count > 0)
	{
		if(i >= count)i = count - 1;
		m_ListBoxMnemonics.SetCurSel(i);
		if(count == 1)EnableControl(m_ButtonMnemonicClear.m_hWnd, false, m_ListBoxMnemonics.m_hWnd);

//		EnableObjectUpdateButton();
	}
	else
	{
		EnableControl(m_ButtonMnemonicClear.m_hWnd, false, m_ListBoxMnemonics.m_hWnd);
	}
	OnSelchangeMnemonic();
}

void SampleDialogSettingCancel::OnMnemonicRemove()
{
	int sel = m_ListBoxMnemonics.GetCurSel();
	if(sel > 0)
	{
		sel = m_ListBoxMnemonics.RemoveItemAt(sel);
		if(sel >= 0)
		{
			MnemonicRemoved((unsigned int)sel);
		}
		m_modified = true;
	}
}

void SampleDialogSettingCancel::MnemonicsCleared()
{
	EnableControl(m_ButtonMnemonicRemove.m_hWnd, false, m_ListBoxMnemonics.m_hWnd);
	EnableControl(m_ButtonMnemonicClear.m_hWnd, false, m_ListBoxMnemonics.m_hWnd);
}

void SampleDialogSettingCancel::OnMnemonicClear()
{
	if(m_ListBoxMnemonics.ClearItems(true))
	{
		MnemonicsCleared();
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnCancelOrder()
{
	SignedPrice replacePriceOffset;
	Price preBorrowPrice;
	unsigned int displaySize = 0;
	unsigned int displaySizeFraction = 0;
	int replaceSizeOffset = 0;

	bool validSymbol = false;
	unsigned __int64 numericSymbol = 0;
#ifndef TAKION_NO_OPTIONS
	unsigned __int64 optionBlock = 0;
	bool optionSettingsValid = false;
	unsigned char securityType = ST_STOCK;
#endif
	FillCancelReplaceParams(replacePriceOffset,
		preBorrowPrice,
		replaceSizeOffset,
		displaySize,
		displaySizeFraction,
		numericSymbol,
#ifndef TAKION_NO_OPTIONS
		optionBlock,
		optionSettingsValid,
		securityType,
#endif
		validSymbol);

	TakionMainWnd* takionMainWnd = TGUI_GetTakionMainWnd();
	if(m_useClientId)
	{
		if(m_clientId && m_account)
		{
			Order* order = m_account->FindOrderByClientId(m_clientId);
			if(order)
			{
				if(m_replaceOrder)
				{
					if(order->CanBeReplaced() && !order->isDeleted())// && !order->isAlgorithm())
					{
						const bool buy = order->GetSide() == 'B';

						if(m_adjustableUseMmBox && !order->isAlgorithm())
						{
							int left = 0;
							int top = 0;
							if(mainDialog)
							{
								const CRect& rect = mainDialog->GetDialogFrameRect();
								left = rect.left;
								top = rect.top;
							}
							takionMainWnd->CancelReplaceOrderWithAdjustment(order,
								left,
								top,
								m_replaceNative,
								replacePriceOffset,
								m_priceOffsetPercent,
								m_priceEnforceMinOffset,
								m_priceOffsetReverseForSell,
								m_marketOrder,//order->isMarketOrder(),
								replaceSizeOffset,
								m_sizeOffsetPercent,
								m_sizeOffsetReverseForSell,
								m_preBorrow,
								preBorrowPrice,
								m_displaySizeMode,//0 - size fraction, 1 - round lot, 2 - no change
								displaySize,
								displaySizeFraction);
						}
						else if(m_adjustable)
						{
							OrderReplaceSet orderSet;
							orderSet.insert(order);
							CPoint corner(0, 0);
							if(mainDialog)
							{
								const CRect& rect = mainDialog->GetDialogFrameRect();
								corner.x = rect.left;
								corner.y = rect.top;
							}
							DialogReplaceOrder* dlg = takionMainWnd->GetReplaceOrderDialog();
							if(dlg->isDisplayed() && dlg->isCancelPosted())
							{
								dlg->IncrementIgnoreCancelCount();
							}
							const unsigned int size = m_SpinDisplaySizeFraction.GetValue();
							displaySize = size / 10;
							displaySizeFraction = size - 10 * displaySize;
							const unsigned int displaySizeRoundLot = m_SpinDisplaySizeRoundLot.GetValue();
							dlg->Display(orderSet,
								!replacePriceOffset.isZero() || !replaceSizeOffset,//priceColumn,
								m_preBorrow,
								m_marketOrder,
								m_replaceNative,
								m_priceOffsetPercent,
								m_priceEnforceMinOffset,
								m_priceOffsetReverseForSell,
								m_sizeOffsetPercent,
								m_sizeOffsetReverseForSell,
								replacePriceOffset,
								preBorrowPrice,
								replaceSizeOffset,
								(OrderCommandDisplaySizeMode)m_displaySizeMode,
								displaySize,
								displaySizeFraction,
								displaySizeRoundLot,

								corner,
								0,
								this,
								corner);
						}
						else
						{
							unsigned char errorCode;
							if(order->isAlgorithm())
							{
								if(buy)
								{
									errorCode = order->ReplacePriceAndSize(nullptr,
										nullptr,//const Security*
										m_replaceNative,
										m_marketOrder,
										replacePriceOffset,
										m_priceOffsetPercent,
										m_priceEnforceMinOffset,
										replaceSizeOffset,
										m_sizeOffsetPercent,
										m_preBorrow,
										preBorrowPrice,
										0xFFFFFFFFFFFFFFFF,
										m_displaySizeMode,//0 - size fraction, 1 - round lot, 2 - no change
										displaySize,
										displaySizeFraction,
										0);
								}
								else
								{
									errorCode = order->ReplacePriceAndSize(nullptr,
										nullptr,//const Security*
										m_replaceNative,
										m_marketOrder,
										m_priceOffsetReverseForSell ? -replacePriceOffset : replacePriceOffset,
										m_priceOffsetPercent,
										m_priceEnforceMinOffset,
										m_sizeOffsetReverseForSell ? -replaceSizeOffset : replaceSizeOffset,
										m_sizeOffsetPercent,
										m_preBorrow,
										preBorrowPrice,
										0xFFFFFFFFFFFFFFFF,
										m_displaySizeMode,//0 - size fraction, 1 - round lot, 2 - no change
										displaySize,
										displaySizeFraction,
										0);
								}
							}
							else if(buy)
							{
								errorCode = order->ReplacePriceAndSize(nullptr,
									nullptr,//const Security*
									m_replaceNative,
									m_marketOrder,
									replacePriceOffset,
									m_priceOffsetPercent,
									m_priceEnforceMinOffset,
									replaceSizeOffset,
									m_sizeOffsetPercent,
									m_preBorrow,
									preBorrowPrice,
									0xFFFFFFFFFFFFFFFF,
									m_displaySizeMode,//0 - size fraction, 1 - round lot, 2 - no change
									displaySize,
									displaySizeFraction,
									0);
							}
							else
							{
								errorCode = order->ReplacePriceAndSize(nullptr,
									nullptr,//const Security*
									m_replaceNative,
									m_marketOrder,
									m_priceOffsetReverseForSell ? -replacePriceOffset : replacePriceOffset,
									m_priceOffsetPercent,
									m_priceEnforceMinOffset,
									m_sizeOffsetReverseForSell ? -replaceSizeOffset : replaceSizeOffset,
									m_sizeOffsetPercent,
									m_preBorrow,
									preBorrowPrice,
									0xFFFFFFFFFFFFFFFF,
									m_displaySizeMode,//0 - size fraction, 1 - round lot, 2 - no change
									displaySize,
									displaySizeFraction,
									0);
							}
							if(errorCode != OE_OK)
							{
/*
								unsigned int l1Bid = 0;
								unsigned int l1Ask = 0;
								const Position* position = order->GetPosition();
								if(position)
								{
									const Security* security = position->GetASecurity();
									if(security)
									{
										l1Bid = security->GetL1Bid();
										l1Ask = security->GetL1Ask();
									}
								}
								takionMainWnd->DisplayOrderError(order->GetAccount(), order->GetSymbol(), order->GetSide(), order->isBorrow(), order->GetReplacingQuantity(), errorCode, Money::moneyZero, Money::moneyZero, 0, 0, l1Bid, l1Ask);
*/
							}
						}
					}
				}
				else
				{
					order->Cancel();
				}
			}
		}
	}
	else// if(m_account)
	{
/*
		Order* oneOrder = nullptr;
		Order** oneOrderPtr = m_adjustableUseMmBox && m_replaceOrder ? &oneOrder : nullptr;
		OrderReplaceSet orderSet;
		OrderReplaceSet* orderSetPtr = m_adjustable && m_replaceOrder ? &orderSet : nullptr;
		unsigned int orderCount = 0;
*/
		std::set<unsigned int> destinationSet;
		const Destination* destination;
		const ListBoxCheckDestination::DestinationSet& checkSet = m_ListBoxDestination.GetCheckedSet();
		for(ListBoxCheckDestination::DestinationSet::const_iterator it = checkSet.cbegin(), itend = checkSet.cend(); it != itend; ++it)
		{
			destination = *it;
			destinationSet.insert(destination->GetNumericProperName());
		}

		std::set<MnemonicType> mnemonicSet;
		const ListBoxStringAsUIntChecked<MnemonicType>::StringSet& mnemonicCheckSet = m_ListBoxMnemonics.GetCheckedSet();
		for(ListBoxStringAsUIntChecked<MnemonicType>::StringSet::const_iterator itm = mnemonicCheckSet.cbegin(), itmend = mnemonicCheckSet.cend(); itm != itmend; ++itm)
		{
			mnemonicSet.insert(*itm);
		}

		CancelReplaceOrdersAll(m_sideMask,
			m_directionMask,
#ifndef TAKION_NO_OPTIONS
			m_instrumentMask,
#endif
			(OrderCancelPick)m_orderPick,
			m_tifMask,
			m_orderTypeMask,
			m_pqOpenMask,
			m_tCloseMask,
			m_excludeDestinations,
			&destinationSet,
			m_excludeMnemonics,
			&mnemonicSet);
/*
		Account* account;

		if(m_accountAll && TD_GetAccountCount() > 1)
		{
			TakionIterator* iterator = TD_CreateAccountIterator();
			TD_LockAccountStorage();
			iterator->Reset();
			while(account = TD_GetNextAccount(iterator))
			{
				if(m_positionType != 0)//all or each
				{
					if(m_replaceOrder)
					{
						orderCount += account->ReplaceOrders(m_positionType == 1,//each
							m_replaceNative,
							m_sideMask,
							m_directionMask,
#ifndef TAKION_NO_OPTIONS
							m_instrumentMask,
#endif
							(OrderCancelPick)m_orderPick,
							m_tifMask,
							m_orderTypeMask,
							m_pqOpenMask,
							m_tCloseMask,
							m_excludeDestinations,
							&destinationSet,
							m_excludeMnemonics,
							&mnemonicSet,
							replacePriceOffset,
							m_priceOffsetPercent,
							m_priceEnforceMinOffset,
							m_priceOffsetReverseForSell,
							m_marketOrder,
							replaceSizeOffset,
							m_sizeOffsetPercent,
							m_sizeOffsetReverseForSell,
							m_preBorrow,
							preBorrowPrice,
							0xFFFFFFFFFFFFFFFF,//0,
							m_displaySizeMode,
							displaySize,
							displaySizeFraction,
							oneOrderPtr,
							orderSetPtr);
					}
					else
					{
						orderCount += account->CancelOrders(m_positionType == 1,//each
							m_sideMask,
							m_directionMask,
#ifndef TAKION_NO_OPTIONS
							m_instrumentMask,
#endif
							(OrderCancelPick)m_orderPick,
							m_tifMask,
							m_orderTypeMask,
							m_pqOpenMask,
							m_tCloseMask,
							m_excludeDestinations,
							&destinationSet,
							m_excludeMnemonics,
							&mnemonicSet);
					}
				}
				else if(validSymbol
#ifndef TAKION_NO_OPTIONS
					&& optionSettingsValid
#endif
					)//current Position (symbol in the control)
				{
//					account->LockInquiryWait();

					Position* position =
#ifndef TAKION_NO_OPTIONS
						account->FindPositionWithLock(numericSymbol, optionBlock, securityType);
#else
						account->FindStockPositionWithLock(numericSymbol);
#endif
					if(position)
					{
						if(m_replaceOrder)
						{
							orderCount += position->ReplaceOrders(m_replaceNative,
								m_sideMask,
								m_directionMask,
								(OrderCancelPick)m_orderPick,
								m_tifMask,
								m_orderTypeMask,
								m_pqOpenMask,
								m_tCloseMask,
								m_excludeDestinations,
								&destinationSet,
								m_excludeMnemonics,
								&mnemonicSet,
								replacePriceOffset,
								m_priceOffsetPercent,
								m_priceEnforceMinOffset,
								m_priceOffsetReverseForSell,
								m_marketOrder,
								replaceSizeOffset,
								m_sizeOffsetPercent,
								m_sizeOffsetReverseForSell,
								m_preBorrow,
								preBorrowPrice,
								0xFFFFFFFFFFFFFFFF,//0,
								m_displaySizeMode,
								displaySize,
								displaySizeFraction,
								oneOrderPtr,
								orderSetPtr);
						}
						else
						{
							orderCount += position->CancelOrders(m_sideMask,
								m_directionMask,
								(OrderCancelPick)m_orderPick,
								m_tifMask,
								m_orderTypeMask,
								m_pqOpenMask,
								m_tCloseMask,
								m_excludeDestinations,
								&destinationSet,
								m_excludeMnemonics,
								&mnemonicSet);
						}
					}
//					account->UnlockInquiry();
				}
			}
			TD_UnlockAccountStorage();
			delete iterator;
		}
		else if(m_account)
		{
			if(m_positionType != 0)//all or each
			{
				if(m_replaceOrder)
				{
					orderCount += m_account->ReplaceOrders(m_positionType == 1,//each
						m_replaceNative,
						m_sideMask,
						m_directionMask,
#ifndef TAKION_NO_OPTIONS
						m_instrumentMask,
#endif
						(OrderCancelPick)m_orderPick,
						m_tifMask,
						m_orderTypeMask,
						m_pqOpenMask,
						m_tCloseMask,
						m_excludeDestinations,
						&destinationSet,
						m_excludeMnemonics,
						&mnemonicSet,
						replacePriceOffset,
						m_priceOffsetPercent,
						m_priceEnforceMinOffset,
						m_priceOffsetReverseForSell,
						m_marketOrder,
						replaceSizeOffset,
						m_sizeOffsetPercent,
						m_sizeOffsetReverseForSell,
						m_preBorrow,
						preBorrowPrice,
						0xFFFFFFFFFFFFFFFF,//0,
						m_displaySizeMode,
						displaySize,
						displaySizeFraction,
						oneOrderPtr,
						orderSetPtr);
				}
				else
				{
					orderCount += m_account->CancelOrders(m_positionType == 1,//each
						m_sideMask,
						m_directionMask,
#ifndef TAKION_NO_OPTIONS
						m_instrumentMask,
#endif
						(OrderCancelPick)m_orderPick,
						m_tifMask,
						m_orderTypeMask,
						m_pqOpenMask,
						m_tCloseMask,
						m_excludeDestinations,
						&destinationSet,
						m_excludeMnemonics,
						&mnemonicSet);
				}
			}
			else if(validSymbol
#ifndef TAKION_NO_OPTIONS
				&& optionSettingsValid
#endif
				)//current Position (symbol in the control)
			{
//				m_account->LockInquiryWait();

				Position* position =
#ifndef TAKION_NO_OPTIONS
					m_account->FindPositionWithLock(numericSymbol, optionBlock, securityType);
#else
					m_account->FindStockPositionWithLock(numericSymbol);
#endif
				if(position)
				{
					if(m_replaceOrder)
					{
						orderCount += position->ReplaceOrders(m_replaceNative,
							m_sideMask,
							m_directionMask,
							(OrderCancelPick)m_orderPick,
							m_tifMask,
							m_orderTypeMask,
							m_pqOpenMask,
							m_tCloseMask,
							m_excludeDestinations,
							&destinationSet,
							m_excludeMnemonics,
							&mnemonicSet,
							replacePriceOffset,
							m_priceOffsetPercent,
							m_priceEnforceMinOffset,
							m_priceOffsetReverseForSell,
							m_marketOrder,
							replaceSizeOffset,
							m_sizeOffsetPercent,
							m_sizeOffsetReverseForSell,
							m_preBorrow,
							preBorrowPrice,
							0xFFFFFFFFFFFFFFFF,//0,
							m_displaySizeMode,
							displaySize,
							displaySizeFraction,
							oneOrderPtr,
							orderSetPtr);
					}
					else
					{
						orderCount += position->CancelOrders(m_sideMask,
							m_directionMask,
							(OrderCancelPick)m_orderPick,
							m_tifMask,
							m_orderTypeMask,
							m_pqOpenMask,
							m_tCloseMask,
							m_excludeDestinations,
							&destinationSet,
							m_excludeMnemonics,
							&mnemonicSet);
					}
				}
//				m_account->UnlockInquiry();
			}
		}
		AdjustReplacement(oneOrder,
			orderSet,
			replacePriceOffset,
			preBorrowPrice,
			displaySize,
			displaySizeFraction,
			replaceSizeOffset);
*/
	}
}

bool SampleDialogSettingCancel::CanCancel() const
{
	return m_useClientId ?
		m_clientId != 0 && m_account != nullptr:

		(m_positionType != 0 || !m_symbol.empty())
		&& m_sideMask != 0
#ifndef TAKION_NO_OPTIONS
		&& m_instrumentMask != 0
#endif
		&& m_directionMask != 0
		&& m_tifMask != 0
		&& m_orderTypeMask != 0
		&& m_pqOpenMask != 0
		&& m_tCloseMask != 0
		&& m_ListBoxDestination.hasDestinations()
		;
}

void SampleDialogSettingCancel::EnableOrderCancelButton()
{
	EnableControl(m_ButtonCancelOrder.m_hWnd, CanCancel(), m_SpinClientId.m_hWnd);
}

#ifndef TAKION_NO_OPTIONS
bool SampleDialogSettingCancel::OptionModeChanged()
{
//	return !m_useClientId && BaseSampleDialogSettingOption::OptionModeChanged();
	if(m_useClientId)
	{
/*
		m_ComboBoxOptionSuffix.SetInvalid(true);
		m_SpinOptionStrikePrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
		m_DateTimeOptionExpiration.SetBkColor(m_visualBase->GetGrayColor());
		m_RadioOptionCall.Invalidate(FALSE);
		m_RadioOptionPut.Invalidate(FALSE);
		m_CheckBoxOption.Invalidate(FALSE);
*/
		return false;
	}
	else
	{
		return BaseSampleDialogSettingOption::OptionModeChanged();
	}
}
#endif

void SampleDialogSettingCancel::OnClientId()
{
	const bool useClientId = m_CheckBoxClientId.GetCheck() == BST_CHECKED;
	if(useClientId != m_useClientId)
	{
		m_useClientId = useClientId;

//		OptionModeChanged();
//		m_CheckBoxOption.Invalidate(FALSE);
		m_EditSymbol.Invalidate(FALSE);
		if(useClientId)
		{
			m_ComboBoxAccount.SetInvalid(false);

			m_CheckBoxClientId.SetForegroundColor(m_visualBase->GetOkTextColor());

#ifndef TAKION_NO_OPTIONS
			m_CheckBoxOption.SetForegroundColor(m_visualBase->GetGrayColor());
			m_ComboBoxOptionSuffix.SetInvalid(true);
			m_SpinOptionStrikePrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
			m_DateTimeOptionExpiration.SetBkColor(m_visualBase->GetGrayColor());
			m_RadioOptionCall.Invalidate(FALSE);
			m_RadioOptionPut.Invalidate(FALSE);
#endif
			m_SpinClientId.SetBkColorEnabled(m_clientId ? 0xFFFFFFFF : m_visualBase->GetWarningColor());
		}
		else
		{
#ifndef TAKION_NO_OPTIONS
			BaseSampleDialogSettingOption::OptionModeChanged();
			m_CheckBoxOption.SetForegroundColor(0xFFFFFFFF);
#endif
			m_ComboBoxAccount.SetInvalid(m_accountAll);
			m_CheckBoxClientId.SetForegroundColor(0xFFFFFFFF);
			m_SpinClientId.SetBkColorEnabled(m_visualBase->GetGrayColor());
		}

		m_RadioAccountCurrent.SetInvalid(useClientId);
		m_RadioAccountAll.SetInvalid(useClientId);

		m_RadioPositionCurrent.SetInvalid(useClientId);
		m_RadioPositionEach.SetInvalid(useClientId);
		m_RadioPositionAll.SetInvalid(useClientId);

#ifndef TAKION_NO_OPTIONS
		m_CheckBoxInstrumentStock.SetInvalid(useClientId);
		m_CheckBoxInstrumentOption.SetInvalid(useClientId);
		m_CheckBoxInstrumentFuture.SetInvalid(useClientId);
#endif

		m_CheckBoxSideBuy.SetInvalid(useClientId);
		m_CheckBoxSideSell.SetInvalid(useClientId);
		m_CheckBoxSideShort.SetInvalid(useClientId);

		m_CheckBoxDirectionClose.SetInvalid(useClientId);
		m_CheckBoxDirectionOpen.SetInvalid(useClientId);
		m_CheckBoxDirectionAdd.SetInvalid(useClientId);

		m_ListBoxOrder.SetInvalid(useClientId);

		m_CheckBoxTifOnOpen.SetInvalid(useClientId);
		m_CheckBoxTifOnClose.SetInvalid(useClientId);
		m_CheckBoxTifExtendedTradingClose.SetInvalid(useClientId);
		m_CheckBoxTifDay.SetInvalid(useClientId);
		m_CheckBoxTifExtended.SetInvalid(useClientId);
		m_CheckBoxTifTime.SetInvalid(useClientId);
		m_CheckBoxTifIoc.SetInvalid(useClientId);
		m_CheckBoxTifFok.SetInvalid(useClientId);
		m_CheckBoxTifGtc.SetInvalid(useClientId);
		m_CheckBoxTifGtt.SetInvalid(useClientId);

		m_CheckBoxOrderTypeAlgo.SetInvalid(useClientId);
		m_CheckBoxOrderTypeLimit.SetInvalid(useClientId);
		m_CheckBoxOrderTypeMarket.SetInvalid(useClientId);
		m_CheckBoxOrderTypeStop.SetInvalid(useClientId);

		m_CheckBoxPqOpenYes.SetInvalid(useClientId);
		m_CheckBoxPqOpenNo.SetInvalid(useClientId);

		m_CheckBoxTCloseYes.SetInvalid(useClientId);
		m_CheckBoxTCloseNo.SetInvalid(useClientId);

		m_CheckBoxExcludeDestinations.SetInvalid(useClientId);
		m_CheckBoxExcludeMnemonics.SetInvalid(useClientId);

		m_ListBoxDestination.SetInvalid(useClientId);
		m_ListBoxMnemonics.SetInvalid(useClientId);

		m_modified = true;

		EnableOrderCancelButton();
	}
}

void SampleDialogSettingCancel::OnExcludeDestinations()
{
	const bool excludeDestinations = m_CheckBoxExcludeDestinations.GetCheck() == BST_CHECKED;
	if(excludeDestinations != m_excludeDestinations)
	{
		m_excludeDestinations = excludeDestinations;
		m_ListBoxDestination.SetExclude(excludeDestinations);
		EnableOrderCancelButton();
	}
}

void SampleDialogSettingCancel::OnExcludeMnemonics()
{
	const bool excludeMnemonics = m_CheckBoxExcludeMnemonics.GetCheck() == BST_CHECKED;
	if(excludeMnemonics != m_excludeMnemonics)
	{
		m_excludeMnemonics = excludeMnemonics;
//		m_ListBoxMnemonics.SetExclude(excludeMnemonics);
//		EnableOrderCancelButton();
	}
}

void SampleDialogSettingCancel::OnSelchangeOrder()
{
	const int sel = m_ListBoxOrder.GetCurSel();
	const unsigned char orderPick = sel >= 0 ? (unsigned char)sel : 0;
	if(orderPick != m_orderPick)
	{
		m_orderPick = orderPick;
	}
}

void SampleDialogSettingCancel::OnAccount()
{
	const bool all = m_RadioAccountAll.GetCheck() == BST_CHECKED;
	if(all != m_accountAll)
	{
		m_accountAll = all;

		if(!m_useClientId)
		m_ComboBoxAccount.SetInvalid(all);

		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnAccountCurrent()
{
	OnAccount();
}

void SampleDialogSettingCancel::OnAccountAll()
{
	OnAccount();
}

void SampleDialogSettingCancel::OnPosition()
{
	const unsigned char positionType = m_RadioPositionCurrent.GetCheck() == BST_CHECKED ? 0:
		m_RadioPositionEach.GetCheck() == BST_CHECKED ? 1:
		2;
	if(positionType != m_positionType)
	{
		m_positionType = positionType;
		m_EditSymbol.Invalidate(FALSE);

		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnPositionCurrent()
{
	OnPosition();
}

void SampleDialogSettingCancel::OnPositionEach()
{
	OnPosition();
}

void SampleDialogSettingCancel::OnPositionAll()
{
	OnPosition();
}

bool SampleDialogSettingCancel::CanCheckAll() const
{
	return (m_sideMask & ((1 << TU_ORDER_SIDE_COUNT) - 1)) != (1 << TU_ORDER_SIDE_COUNT) - 1
#ifndef TAKION_NO_OPTIONS
		|| (m_instrumentMask & ST_MASK_ALL) != ST_MASK_ALL
#endif
		|| (m_directionMask & ODIR_LIVE_MASK) != ODIR_LIVE_MASK
		|| (m_tifMask & (TIFMASK_ALL & ~(1 << TIFMASK_OTHER))) != (TIFMASK_ALL & ~(1 << TIFMASK_OTHER))
		|| (m_orderTypeMask & ALGORITHMMASK_ALL) != ALGORITHMMASK_ALL
		|| (m_pqOpenMask & YNM_ALL) != YNM_ALL
		|| (m_tCloseMask & YNM_ALL) != YNM_ALL
		;
}

bool SampleDialogSettingCancel::CanUncheckAll() const
{
	return m_sideMask != 0
#ifndef TAKION_NO_OPTIONS
		|| m_instrumentMask != 0
#endif
		|| m_directionMask != 0
		|| m_tifMask != 0
		|| m_orderTypeMask != 0
		|| m_pqOpenMask != 0
		|| m_tCloseMask != 0
		;
}

void SampleDialogSettingCancel::EnableCheckUncheckAllButtons()
{
	EnableControl(m_ButtonCheckAll.m_hWnd, CanCheckAll(), m_SpinClientId.m_hWnd);
	EnableControl(m_ButtonUncheckAll.m_hWnd, CanUncheckAll(), m_SpinClientId.m_hWnd);
}

void SampleDialogSettingCancel::OnCheckAll()
{
#ifndef TAKION_NO_OPTIONS
	SetInstrumentMask(ST_MASK_ALL);
#endif
	SetSideMask((1 << TU_ORDER_SIDE_COUNT) - 1);
	SetDirectionMask(ODIR_LIVE_MASK);
	SetTifMask(TIFMASK_ALL & ~(1 << TIFMASK_OTHER));
	SetOrderTypeMask(ALGORITHMMASK_ALL);
	SetPqOpenMask(YNM_ALL);
	SetTCloseMask(YNM_ALL);
//	EnableCheckUncheckAllButtons();
}

void SampleDialogSettingCancel::OnUncheckAll()
{
#ifndef TAKION_NO_OPTIONS
	SetInstrumentMask(0);
#endif
	SetSideMask(0);
	SetDirectionMask(0);
	SetTifMask(0);
	SetOrderTypeMask(0);
	SetPqOpenMask(0);
	SetTCloseMask(0);
//	EnableCheckUncheckAllButtons();
}

#ifndef TAKION_NO_OPTIONS
void SampleDialogSettingCancel::OnInstrumentStock()
{
	OnInstrument(1 << ST_STOCK, m_CheckBoxInstrumentStock);
}

void SampleDialogSettingCancel::OnInstrumentOption()
{
	OnInstrument(1 << ST_OPTION, m_CheckBoxInstrumentOption);
}

void SampleDialogSettingCancel::OnInstrumentFuture()
{
	OnInstrument(1 << ST_FUTURE, m_CheckBoxInstrumentFuture);
}

void SampleDialogSettingCancel::OnInstrument(unsigned char instrumentFlag, const CheckBoxOwnerDraw& instrumentCheckBox)
{
	if(instrumentCheckBox.GetCheck() == BST_CHECKED)
	{
		if(!(m_instrumentMask & instrumentFlag))
		{
			const bool toChangeColor = !m_instrumentMask;
			m_instrumentMask |= instrumentFlag;
			if(toChangeColor)
			{
				DoUpdateInstrumentColors(0xFFFFFFFF);
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
	else
	{
		if(m_instrumentMask & instrumentFlag)
		{
			m_instrumentMask &= ~instrumentFlag;
			if(!m_instrumentMask)
			{
				DoUpdateInstrumentColors(m_visualBase->GetWarningColor());
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
}

void SampleDialogSettingCancel::OnInstrument()
{
	const unsigned char instrumentMask = (m_CheckBoxInstrumentStock.GetCheck() == BST_CHECKED ? (1 << ST_STOCK) : 0)
		| (m_CheckBoxInstrumentOption.GetCheck() == BST_CHECKED ? (1 << ST_OPTION) : 0)
		| (m_CheckBoxInstrumentFuture.GetCheck() == BST_CHECKED ? (1 << ST_FUTURE) : 0);
	if(instrumentMask != m_instrumentMask)
	{
		const bool toChangeColor = !m_instrumentMask != !instrumentMask;
		m_instrumentMask = instrumentMask;
		if(toChangeColor)
		{
			UpdateInstrumentColors();
			if(!m_useClientId)
			EnableOrderCancelButton();

			EnableCheckUncheckAllButtons();
		}
		m_modified = true;
	}
}

void SampleDialogSettingCancel::SetInstrumentMask(unsigned char instrumentMask)
{
	if(instrumentMask != m_instrumentMask)
	{
		m_CheckBoxInstrumentStock.SetCheck(instrumentMask & (1 << ST_STOCK) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxInstrumentOption.SetCheck(instrumentMask & (1 << ST_OPTION) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxInstrumentFuture.SetCheck(instrumentMask & (1 << ST_FUTURE) ? BST_CHECKED : BST_UNCHECKED);
		OnInstrument();
	}
}

void SampleDialogSettingCancel::UpdateInstrumentColors()
{
	DoUpdateInstrumentColors(!m_instrumentMask ? m_visualBase->GetWarningColor() : 0xFFFFFFFF);
}

void SampleDialogSettingCancel::DoUpdateInstrumentColors(const COLORREF color)
{
	m_CheckBoxInstrumentStock.SetForegroundColor(color);
	m_CheckBoxInstrumentOption.SetForegroundColor(color);
	m_CheckBoxInstrumentFuture.SetForegroundColor(color);
}
#endif

void SampleDialogSettingCancel::OnSideBuy()
{
	OnSide(1 << TU_ORDER_SIDE_BUY, m_CheckBoxSideBuy);
}

void SampleDialogSettingCancel::OnSideSell()
{
	OnSide(1 << TU_ORDER_SIDE_SELL, m_CheckBoxSideSell);
}

void SampleDialogSettingCancel::OnSideShort()
{
	OnSide(1 << TU_ORDER_SIDE_SHORT, m_CheckBoxSideShort);
}

void SampleDialogSettingCancel::OnSide(unsigned char sideFlag, const CheckBoxOwnerDraw& sideCheckBox)
{
	if(sideCheckBox.GetCheck() == BST_CHECKED)
	{
		if(!(m_sideMask & sideFlag))
		{
			const bool toChangeColor = !m_sideMask;
			m_sideMask |= sideFlag;
			if(toChangeColor)
			{
				DoUpdateSideColors(0xFFFFFFFF);
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
	else
	{
		if(m_sideMask & sideFlag)
		{
			m_sideMask &= ~sideFlag;
			if(!m_sideMask)
			{
				DoUpdateSideColors(m_visualBase->GetWarningColor());
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
}

void SampleDialogSettingCancel::UpdateSideColors()
{
	DoUpdateSideColors(!m_sideMask ? m_visualBase->GetWarningColor() : 0xFFFFFFFF);
}

void SampleDialogSettingCancel::DoUpdateSideColors(const COLORREF color)
{
	m_CheckBoxSideBuy.SetForegroundColor(color);
	m_CheckBoxSideSell.SetForegroundColor(color);
	m_CheckBoxSideShort.SetForegroundColor(color);
}

void SampleDialogSettingCancel::OnSide()
{
	const unsigned char sideMask = (m_CheckBoxSideBuy.GetCheck() == BST_CHECKED ? (1 << TU_ORDER_SIDE_BUY) : 0)
		| (m_CheckBoxSideSell.GetCheck() == BST_CHECKED ? (1 << TU_ORDER_SIDE_SELL) : 0)
		| (m_CheckBoxSideShort.GetCheck() == BST_CHECKED ? (1 << TU_ORDER_SIDE_SHORT) : 0);
	if(sideMask != m_sideMask)
	{
		const bool toChangeColor = !m_sideMask != !sideMask;
		m_sideMask = sideMask;
		if(toChangeColor)
		{
			UpdateSideColors();
			if(!m_useClientId)
			EnableOrderCancelButton();

			EnableCheckUncheckAllButtons();
		}
		m_modified = true;
	}
}

void SampleDialogSettingCancel::SetSideMask(unsigned char sideMask)
{
	if(sideMask != m_sideMask)
	{
		m_CheckBoxSideBuy.SetCheck(sideMask & (1 << TU_ORDER_SIDE_BUY) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxSideSell.SetCheck(sideMask & (1 << TU_ORDER_SIDE_SELL) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxSideShort.SetCheck(sideMask & (1 << TU_ORDER_SIDE_SHORT) ? BST_CHECKED : BST_UNCHECKED);
		OnSide();
	}
}
/////////

void SampleDialogSettingCancel::OnDirectionClose()
{
	OnDirection(1 << ODIR_CLOSE, m_CheckBoxDirectionClose);
}

void SampleDialogSettingCancel::OnDirectionOpen()
{
	OnDirection(1 << ODIR_OPEN, m_CheckBoxDirectionOpen);
}

void SampleDialogSettingCancel::OnDirectionAdd()
{
	OnDirection(1 << ODIR_ADD, m_CheckBoxDirectionAdd);
}

void SampleDialogSettingCancel::OnDirection(unsigned char directionFlag, const CheckBoxOwnerDraw& directionCheckBox)
{
	if(directionCheckBox.GetCheck() == BST_CHECKED)
	{
		if(!(m_directionMask & directionFlag))
		{
			const bool toChangeColor = !m_directionMask;
			m_directionMask |= directionFlag;
			if(toChangeColor)
			{
				DoUpdateDirectionColors(0xFFFFFFFF);
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
	else
	{
		if(m_directionMask & directionFlag)
		{
			m_directionMask &= ~directionFlag;
			if(!m_directionMask)
			{
				DoUpdateDirectionColors(m_visualBase->GetWarningColor());
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
}

void SampleDialogSettingCancel::OnDirection()
{
	const unsigned char directionMask = (m_CheckBoxDirectionClose.GetCheck() == BST_CHECKED ? (1 << ODIR_CLOSE) : 0)
		| (m_CheckBoxDirectionOpen.GetCheck() == BST_CHECKED ? (1 << ODIR_OPEN) : 0)
		| (m_CheckBoxDirectionAdd.GetCheck() == BST_CHECKED ? (1 << ODIR_ADD) : 0);
	if(directionMask != m_directionMask)
	{
		const bool toChangeColor = !m_directionMask != !directionMask;
		m_directionMask = directionMask;
		if(toChangeColor)
		{
			UpdateDirectionColors();
			if(!m_useClientId)
			EnableOrderCancelButton();

			EnableCheckUncheckAllButtons();
		}
		m_modified = true;
	}
}

void SampleDialogSettingCancel::SetDirectionMask(unsigned char directionMask)
{
	if(directionMask != m_directionMask)
	{
		m_CheckBoxDirectionClose.SetCheck(directionMask & (1 << ODIR_CLOSE) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxDirectionOpen.SetCheck(directionMask & (1 << ODIR_OPEN) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxDirectionAdd.SetCheck(directionMask & (1 << ODIR_ADD) ? BST_CHECKED : BST_UNCHECKED);
		OnDirection();
	}
}

void SampleDialogSettingCancel::UpdateDirectionColors()
{
	DoUpdateDirectionColors(!m_directionMask ? m_visualBase->GetWarningColor() : 0xFFFFFFFF);
}

void SampleDialogSettingCancel::DoUpdateDirectionColors(const COLORREF color)
{
	m_CheckBoxDirectionClose.SetForegroundColor(color);
	m_CheckBoxDirectionOpen.SetForegroundColor(color);
	m_CheckBoxDirectionAdd.SetForegroundColor(color);
}

/////
void SampleDialogSettingCancel::OnTifOnOpen()
{
	OnTif(1 << TIFMASK_ON_OPEN, m_CheckBoxTifOnOpen);
}

void SampleDialogSettingCancel::OnTifOnClose()
{
	OnTif(1 << TIFMASK_ON_CLOSE, m_CheckBoxTifOnClose);
}

void SampleDialogSettingCancel::OnTifDay()
{
	OnTif(1 << TIFMASK_DAY, m_CheckBoxTifDay);
}

void SampleDialogSettingCancel::OnTifExtended()
{
	OnTif(1 << TIFMASK_EXTENDED, m_CheckBoxTifExtended);
}

void SampleDialogSettingCancel::OnTifTime()
{
	OnTif(1 << TIFMASK_TIME, m_CheckBoxTifTime);
}

void SampleDialogSettingCancel::OnTifIoc()
{
	OnTif(1 << TIFMASK_IOC, m_CheckBoxTifIoc);
}

void SampleDialogSettingCancel::OnTifFok()
{
	OnTif(1 << TIFMASK_FOK, m_CheckBoxTifFok);
}

void SampleDialogSettingCancel::OnTifGtc()
{
	OnTif(1 << TIFMASK_GTC, m_CheckBoxTifGtc);
}

void SampleDialogSettingCancel::OnTifGtt()
{
	OnTif(1 << TIFMASK_GTT, m_CheckBoxTifGtt);
}

void SampleDialogSettingCancel::OnTifExtendedTradingCloseClose()
{
	OnTif(1 << TIFMASK_ETC, m_CheckBoxTifExtendedTradingClose);
}

void SampleDialogSettingCancel::OnTif(unsigned int tifFlag, const CheckBoxOwnerDraw& tifCheckBox)
{
	if(tifCheckBox.GetCheck() == BST_CHECKED)
	{
		if(!(m_tifMask & tifFlag))
		{
			const bool toChangeColor = !m_tifMask;
			m_tifMask |= tifFlag;
			if(toChangeColor)
			{
				DoUpdateTifColors(0xFFFFFFFF);
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
	else
	{
		if(m_tifMask & tifFlag)
		{
			m_tifMask &= ~tifFlag;
			if(!m_tifMask)
			{
				DoUpdateTifColors(m_visualBase->GetWarningColor());
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
}

void SampleDialogSettingCancel::OnTif()
{
/*
#if defined(T_RELEASED) || defined(_DEBUG)
	unsigned int mask = 0;
	if(m_CheckBoxTifOnOpen.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_ON_OPEN;
	}
	if(m_CheckBoxTifOnClose.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_ON_CLOSE;
	}
	if(m_CheckBoxTifDay.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_DAY;
	}
	if(m_CheckBoxTifExtended.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_EXTENDED;
	}
	if(m_CheckBoxTifTime.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_TIME;
	}
	if(m_CheckBoxTifIoc.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_IOC;
	}
	if(m_CheckBoxTifFok.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_FOK;
	}
	if(m_CheckBoxTifGtc.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_GTC;
	}
	if(m_CheckBoxTifGtt.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_GTT;
	}
	if(m_CheckBoxTifExtendedTradingClose.GetCheck() == BST_CHECKED)
	{
		mask |= 1 << TIFMASK_ETC;
	}
	printf("%d", mask);
#endif
*/
	const unsigned int tifMask = (m_CheckBoxTifOnOpen.GetCheck() == BST_CHECKED ? (1 << TIFMASK_ON_OPEN) : 0)
		| (m_CheckBoxTifOnClose.GetCheck() == BST_CHECKED ? (1 << TIFMASK_ON_CLOSE) : 0)
		| (m_CheckBoxTifDay.GetCheck() == BST_CHECKED ? (1 << TIFMASK_DAY) : 0)
		| (m_CheckBoxTifExtended.GetCheck() == BST_CHECKED ? (1 << TIFMASK_EXTENDED) : 0)
		| (m_CheckBoxTifTime.GetCheck() == BST_CHECKED ? (1 << TIFMASK_TIME) : 0)
		| (m_CheckBoxTifIoc.GetCheck() == BST_CHECKED ? (1 << TIFMASK_IOC) : 0)
		| (m_CheckBoxTifFok.GetCheck() == BST_CHECKED ? (1 << TIFMASK_FOK) : 0)
		| (m_CheckBoxTifGtc.GetCheck() == BST_CHECKED ? (1 << TIFMASK_GTC) : 0)
		| (m_CheckBoxTifGtt.GetCheck() == BST_CHECKED ? (1 << TIFMASK_GTT) : 0)
		| (m_CheckBoxTifExtendedTradingClose.GetCheck() == BST_CHECKED ? (1 << TIFMASK_ETC) : 0)
		;
	if(tifMask != m_tifMask)
	{
		const bool toChangeColor = !m_tifMask != !tifMask;
		m_tifMask = tifMask;
		if(toChangeColor)
		{
			UpdateTifColors();
			if(!m_useClientId)
			EnableOrderCancelButton();

			EnableCheckUncheckAllButtons();
		}
		m_modified = true;
	}
}

void SampleDialogSettingCancel::SetTifMask(unsigned int tifMask)
{
	if(tifMask != m_tifMask)
	{
		m_CheckBoxTifOnOpen.SetCheck(tifMask & (1 << TIFMASK_ON_OPEN) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTifOnClose.SetCheck(tifMask & (1 << TIFMASK_ON_CLOSE) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTifDay.SetCheck(tifMask & (1 << TIFMASK_DAY) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTifExtended.SetCheck(tifMask & (1 << TIFMASK_EXTENDED) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTifTime.SetCheck(tifMask & (1 << TIFMASK_TIME) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTifIoc.SetCheck(tifMask & (1 << TIFMASK_IOC) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTifFok.SetCheck(tifMask & (1 << TIFMASK_FOK) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTifGtc.SetCheck(tifMask & (1 << TIFMASK_GTC) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTifGtt.SetCheck(tifMask & (1 << TIFMASK_GTT) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTifExtendedTradingClose.SetCheck(tifMask & (1 << TIFMASK_ETC) ? BST_CHECKED : BST_UNCHECKED);
		OnTif();
	}
}

void SampleDialogSettingCancel::UpdateTifColors()
{
	DoUpdateTifColors(!m_tifMask ? m_visualBase->GetWarningColor() : 0xFFFFFFFF);
}

void SampleDialogSettingCancel::DoUpdateTifColors(const COLORREF color)
{
	m_CheckBoxTifOnOpen.SetForegroundColor(color);
	m_CheckBoxTifOnClose.SetForegroundColor(color);
	m_CheckBoxTifExtendedTradingClose.SetForegroundColor(color);
	m_CheckBoxTifDay.SetForegroundColor(color);
	m_CheckBoxTifExtended.SetForegroundColor(color);
	m_CheckBoxTifTime.SetForegroundColor(color);
	m_CheckBoxTifIoc.SetForegroundColor(color);
	m_CheckBoxTifFok.SetForegroundColor(color);
	m_CheckBoxTifGtc.SetForegroundColor(color);
	m_CheckBoxTifGtt.SetForegroundColor(color);
}

/////
void SampleDialogSettingCancel::OnOrderTypeLimit()
{
	OnOrderType(1 << ALGORITHMMASK_LIMIT, m_CheckBoxOrderTypeLimit);
}

void SampleDialogSettingCancel::OnOrderTypeMarket()
{
	OnOrderType(1 << ALGORITHMMASK_MARKET, m_CheckBoxOrderTypeMarket);
}

void SampleDialogSettingCancel::InvalidateOnOrderTypeStop()
{
	if(!m_useClientId && m_replaceOrder && m_marketOrder)
	{
		if(m_orderTypeMask & (1 << ALGORITHMMASK_STOP))
		{
			m_SpinReplacePriceOffset.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
		}
		else
		{
			m_SpinReplacePriceOffset.SetBkColorEnabled(m_visualBase->GetGrayColor());
		}
		m_StaticReplacePrice.Invalidate(FALSE);
		const bool replacePriceOffsetInvalid = isReplacePriceOffsetInvalid();
		m_CheckBoxReplacePriceOffsetPercent.SetInvalid(replacePriceOffsetInvalid);
		m_CheckBoxReplacePriceReverseForSell.SetInvalid(replacePriceOffsetInvalid);
	}
}

void SampleDialogSettingCancel::OnOrderTypeStop()
{
	OnOrderType(1 << ALGORITHMMASK_STOP, m_CheckBoxOrderTypeStop);
	InvalidateOnOrderTypeStop();
}

void SampleDialogSettingCancel::OnOrderTypeAlgo()
{
	OnOrderType(1 << ALGORITHMMASK_YES, m_CheckBoxOrderTypeAlgo);
}

void SampleDialogSettingCancel::OnOrderType(unsigned char orderTypeFlag, const CheckBoxOwnerDraw& orderTypeCheckBox)
{
	if(orderTypeCheckBox.GetCheck() == BST_CHECKED)
	{
		if(!(m_orderTypeMask & orderTypeFlag))
		{
			const bool toChangeColor = !m_orderTypeMask;
			m_orderTypeMask |= orderTypeFlag;
			if(toChangeColor)
			{
				DoUpdateOrderTypeColors(0xFFFFFFFF);
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
	else
	{
		if(m_orderTypeMask & orderTypeFlag)
		{
			m_orderTypeMask &= ~orderTypeFlag;
			if(!m_orderTypeMask)
			{
				DoUpdateOrderTypeColors(m_visualBase->GetWarningColor());
				if(!m_useClientId)
				EnableOrderCancelButton();

				EnableCheckUncheckAllButtons();
			}
			m_modified = true;
		}
	}
}

void SampleDialogSettingCancel::OnOrderType()
{
	const unsigned char orderTypeMask = (m_CheckBoxOrderTypeAlgo.GetCheck() == BST_CHECKED ? (1 << ALGORITHMMASK_YES) : 0)
		| (m_CheckBoxOrderTypeLimit.GetCheck() == BST_CHECKED ? (1 << ALGORITHMMASK_LIMIT) : 0)
		| (m_CheckBoxOrderTypeMarket.GetCheck() == BST_CHECKED ? (1 << ALGORITHMMASK_MARKET) : 0)
		| (m_CheckBoxOrderTypeStop.GetCheck() == BST_CHECKED ? (1 << ALGORITHMMASK_STOP) : 0);
	if(orderTypeMask != m_orderTypeMask)
	{
		const bool toChangeColor = !m_orderTypeMask != !orderTypeMask;
		m_orderTypeMask = orderTypeMask;
		if(toChangeColor)
		{
			UpdateOrderTypeColors();
			if(!m_useClientId)
			EnableOrderCancelButton();

			EnableCheckUncheckAllButtons();
		}
		m_modified = true;
	}
}

void SampleDialogSettingCancel::SetOrderTypeMask(unsigned char orderTypeMask)
{
	if(orderTypeMask != m_orderTypeMask)
	{
		m_CheckBoxOrderTypeAlgo.SetCheck(orderTypeMask & (1 << ALGORITHMMASK_YES) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxOrderTypeLimit.SetCheck(orderTypeMask & (1 << ALGORITHMMASK_LIMIT) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxOrderTypeMarket.SetCheck(orderTypeMask & (1 << ALGORITHMMASK_MARKET) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxOrderTypeStop.SetCheck(orderTypeMask & (1 << ALGORITHMMASK_STOP) ? BST_CHECKED : BST_UNCHECKED);
		OnOrderType();
		InvalidateOnOrderTypeStop();
	}
}

void SampleDialogSettingCancel::UpdateOrderTypeColors()
{
	DoUpdateOrderTypeColors(!m_orderTypeMask ? m_visualBase->GetWarningColor() : 0xFFFFFFFF);
}

void SampleDialogSettingCancel::DoUpdateOrderTypeColors(const COLORREF color)
{
	m_CheckBoxOrderTypeAlgo.SetForegroundColor(color);
	m_CheckBoxOrderTypeLimit.SetForegroundColor(color);
	m_CheckBoxOrderTypeMarket.SetForegroundColor(color);
	m_CheckBoxOrderTypeStop.SetForegroundColor(color);
}
/////
bool SampleDialogSettingCancel::SetYesNoMask(CheckBoxOwnerDraw& checkBox, CheckBoxOwnerDraw& otherCheckBox, unsigned char& mask, unsigned char yesnoMask)
{
	if(mask != yesnoMask)
	{
		const bool toChangeColor = !mask != !yesnoMask;
		mask = yesnoMask;
		if(toChangeColor)
		{
			const COLORREF color = !mask ? m_visualBase->GetWarningColor() : 0xFFFFFFFF;
			checkBox.SetForegroundColor(color);
			otherCheckBox.SetForegroundColor(color);
		}

		return true;
	}
	return false;
}

void SampleDialogSettingCancel::OnYesNoMask(CheckBoxOwnerDraw& checkBox, CheckBoxOwnerDraw& otherCheckBox, unsigned char& mask, YesNoMask yesno)
{
	unsigned char yesnoMask = mask;
	if(checkBox.GetCheck() == BST_CHECKED)
	{
		yesnoMask |= 1 << yesno;
	}
	else
	{
		yesnoMask &= ~(1 << yesno);
	}
	if(SetYesNoMask(checkBox, otherCheckBox, mask, yesnoMask))
	{
		if(!m_useClientId)
		EnableOrderCancelButton();

		EnableCheckUncheckAllButtons();
	}
}

void SampleDialogSettingCancel::OnPqOpenYes()
{
	OnYesNoMask(m_CheckBoxPqOpenYes, m_CheckBoxPqOpenNo, m_pqOpenMask, YNM_YES);
}

void SampleDialogSettingCancel::OnPqOpenNo()
{
	OnYesNoMask(m_CheckBoxPqOpenNo, m_CheckBoxPqOpenYes, m_pqOpenMask, YNM_NO);
}

void SampleDialogSettingCancel::SetPqOpenMask(unsigned char pqOpenMask)
{
	if(pqOpenMask != m_pqOpenMask)
	{
		m_CheckBoxPqOpenYes.SetCheck(pqOpenMask & (1 << YNM_YES) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxPqOpenNo.SetCheck(pqOpenMask & (1 << YNM_NO) ? BST_CHECKED : BST_UNCHECKED);
//		OnPqOpen();
		if(SetYesNoMask(m_CheckBoxPqOpenYes, m_CheckBoxPqOpenNo, m_pqOpenMask, pqOpenMask))
		{
			if(!m_useClientId)
			EnableOrderCancelButton();

			EnableCheckUncheckAllButtons();
		}
	}
}

void SampleDialogSettingCancel::OnTCloseYes()
{
	OnYesNoMask(m_CheckBoxTCloseYes, m_CheckBoxTCloseNo, m_tCloseMask, YNM_YES);
}

void SampleDialogSettingCancel::OnTCloseNo()
{
	OnYesNoMask(m_CheckBoxTCloseNo, m_CheckBoxTCloseYes, m_tCloseMask, YNM_NO);
}

void SampleDialogSettingCancel::SetTCloseMask(unsigned char tCloseMask)
{
	if(tCloseMask != m_tCloseMask)
	{
		m_CheckBoxTCloseYes.SetCheck(tCloseMask & (1 << YNM_YES) ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxTCloseNo.SetCheck(tCloseMask & (1 << YNM_NO) ? BST_CHECKED : BST_UNCHECKED);
//		OnTClose();
		if(SetYesNoMask(m_CheckBoxTCloseYes, m_CheckBoxTCloseNo, m_tCloseMask, tCloseMask))
		{
			if(!m_useClientId)
			EnableOrderCancelButton();

			EnableCheckUncheckAllButtons();
		}
	}
}

void SampleDialogSettingCancel::Resize(bool widthChanged, bool heightChanged)
{
	BaseSampleDialogSettingOption::Resize(widthChanged, heightChanged);
//	if(heightChanged)InvalidateAllTotalValueRects(listBoxNewsId);

	if(heightChanged)
	{
		::InvalidateRect(m_ButtonMnemonicFind.m_hWnd, nullptr, FALSE);
		::InvalidateRect(m_ButtonMnemonicAdd.m_hWnd, nullptr, FALSE);
		::InvalidateRect(m_ButtonMnemonicRemove.m_hWnd, nullptr, FALSE);
		::InvalidateRect(m_ButtonMnemonicClear.m_hWnd, nullptr, FALSE);
	}
}

void SampleDialogSettingCancel::ListBoxDestinationChecked()
{
	m_ListBoxDestination.InvalidateClient();
	{
		char num[33];
		m_StaticDestinationCount.SetWindowText(U_UnsignedNumberToStrNoDecDigits(m_ListBoxDestination.GetCheckCount(), num, sizeof(num)));
	}

	if(!m_useClientId)
	EnableOrderCancelButton();
}

LRESULT SampleDialogSettingCancel::OnListBoxChecked(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case listBoxDestinationsId:
		ListBoxDestinationChecked();
		break;

		case listBoxMnemonicsId:
		break;

//		default:
//		if(isValid())EnableSubscribeButton();
//		break;
	}
	return 0;
}

LRESULT SampleDialogSettingCancel::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinClientIdId:
		{
			const unsigned int clientId = m_SpinClientId.GetValue();
			if(clientId != m_clientId)
			{
				if(m_useClientId)
				{
					const bool wasZero = !m_clientId;
					if(wasZero || !clientId)
					{
						m_SpinClientId.SetBkColorEnabled(wasZero ? 0xFFFFFFFF : m_visualBase->GetWarningColor());
					}
				}
				m_clientId = clientId;

				if(m_useClientId)
				EnableOrderCancelButton();

				m_modified = true;
			}
		}
		break;

		case spinReplaceOrderPriceOffsetId:
		FillSignedPriceFromSpinControl(m_replacePriceOffset, m_SpinReplacePriceOffset, 10000);
		m_CheckBoxReplacePriceEnforceMinOffset.SetInvalid(!m_replaceOrder || m_marketOrder || m_replacePriceOffset.isZero());
		m_modified = true;
		break;

		case spinReplaceOrderSizeOffsetId:
		m_modified = true;
		break;

		case spinReplaceOrderPreBorrowId:
		m_modified = true;
		break;

		case spinReplaceOrderDisplaySizeFractionId:
		m_modified = true;
		break;

		case spinReplaceOrderDisplaySizeRoundLotId:
		m_modified = true;
		break;


#ifndef TAKION_NO_OPTIONS
		default:
		BaseSampleDialogSettingOption::OnSpinChange(w, l);
		break;
#endif
	}
	return 0;
}

HBRUSH SampleDialogSettingCancel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if(m_useClientId)
	{
		switch(pWnd->GetDlgCtrlID())
		{
			case editSymbolId:
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();

#ifndef TAKION_NO_OPTIONS
//			case checkBoxSymbolOptionId:
			case radioSymbolOptionCallId:
			case radioSymbolOptionPutId:
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
#endif
		}
	}
	else if(m_positionType > 0)
	{
		switch(pWnd->GetDlgCtrlID())
		{
			case editSymbolId:
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
	}

	switch(pWnd->GetDlgCtrlID())
	{
		case staticReplaceOrderPriceOffsetId:
		if(isReplacePriceOffsetInvalid())
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetSystemGrayTextColor());
			return *m_bkBrush;
		}
		break;

		case staticReplaceOrderSizeOffsetId:
//		case staticReplaceOrderDisplaySizeId:
		if(!m_replaceOrder)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetSystemGrayTextColor());
			return *m_bkBrush;
		}
		break;
	}

	return BaseSampleDialogSettingOption::OnCtlColor(pDC, pWnd, nCtlColor);
}

bool SampleDialogSettingCancel::isReplacePriceOffsetInvalid() const
{
	return !m_replaceOrder || !(m_orderTypeMask & (1 << ALGORITHMMASK_STOP)) && m_marketOrder && !m_useClientId;//)// || m_marketOrder)//can be used for stop order
}

void SampleDialogSettingCancel::InvalidateControlsOnReplaceNative()
{
	m_CheckBoxReplaceNative.SetForegroundColor(m_replaceNative ? m_visualBase->GetOkTextColor() : m_visualBase->GetWarningTextColor());
}

void SampleDialogSettingCancel::InvalidateControlsOnMarketOrder()
{
	m_CheckBoxReplacePriceEnforceMinOffset.SetInvalid(!m_replaceOrder || m_marketOrder || m_replacePriceOffset.isZero());
	if(m_replaceOrder)
	{
		m_CheckBoxMarketOrder.SetForegroundColor(m_marketOrder ? m_visualBase->GetBlueTextColor() : 0xFFFFFFFF);

//		m_CheckBoxReplaceNative.SetInvalid(!m_replaceOrder || m_marketOrder);// ? m_replaceNative ? m_visualBase->GetOkTextColor() : m_visualBase->GetWarningTextColor() : m_visualBase->GetSystemGrayTextColor());
		m_CheckBoxReplaceNative.SetInvalid(m_marketOrder);

		if(!(m_orderTypeMask & (1 << ALGORITHMMASK_STOP)) && !m_useClientId)
		{
			m_SpinReplacePriceOffset.SetBkColorEnabled(m_marketOrder ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));//Can be used for Stop orders
			m_StaticReplacePrice.Invalidate(FALSE);
			const bool replacePriceOffsetInvalid = isReplacePriceOffsetInvalid();
			m_CheckBoxReplacePriceOffsetPercent.SetInvalid(replacePriceOffsetInvalid);
			m_CheckBoxReplacePriceReverseForSell.SetInvalid(replacePriceOffsetInvalid);
		}
	}
}

void SampleDialogSettingCancel::InvalidateControlsOnReplaceOrder()
{
	m_ButtonCancelOrder.SetWindowText(m_replaceOrder ? replaceOrderText : cancelOrderText);

	m_GroupCancelReplaceOrdersAll.SetWindowText(m_replaceOrder ? replaceOrdersText : cancelOrdersText);

	m_CheckBoxReplaceOrder.SetForegroundColor(m_replaceOrder ? m_visualBase->GetOkTextColor() : 0xFFFFFFFF);

	m_CheckBoxReplaceNative.SetInvalid(!m_replaceOrder || m_marketOrder);
	m_CheckBoxReplacePriceEnforceMinOffset.SetInvalid(!m_replaceOrder || m_marketOrder || m_replacePriceOffset.isZero());

	m_StaticReplaceSize.Invalidate(FALSE);
	m_StaticReplacePrice.Invalidate(FALSE);
	m_StaticReplacePrice.Invalidate(FALSE);

	const COLORREF grayColor = m_visualBase->GetGrayColor();
	const COLORREF whiteColor = GetSysColor(COLOR_WINDOW);

	const bool replacePriceOffsetInvalid = isReplacePriceOffsetInvalid();
	m_CheckBoxReplacePriceOffsetPercent.SetInvalid(replacePriceOffsetInvalid);
	m_CheckBoxReplacePriceReverseForSell.SetInvalid(replacePriceOffsetInvalid);

	m_CheckBoxMarketOrder.SetInvalid(!m_replaceOrder);

	m_CheckBoxReplaceSizeOffsetPercent.SetInvalid(!m_replaceOrder);
	m_CheckBoxReplaceSizeReverseForSell.SetInvalid(!m_replaceOrder);

	m_CheckBoxPreBorrow.SetInvalid(!m_replaceOrder);

	RadioOwnerDraw* cursor = m_radioDisplaySizeMode;
	for(unsigned char i = 0; i <= OCDSM_COUNT; ++i, ++cursor)
	{
		cursor->SetInvalid(!m_replaceOrder);
	}

	m_CheckBoxReplaceAdjustable.SetInvalid(!m_replaceOrder);
	m_CheckBoxReplaceAdjustableUseMmBox.SetInvalid(!m_replaceOrder);

	if(m_replaceOrder)
	{
		if((m_orderTypeMask & (1 << ALGORITHMMASK_STOP)) || !m_marketOrder || m_useClientId)
		{
			m_SpinReplacePriceOffset.SetBkColorEnabled(whiteColor);
		}
		m_SpinReplaceSize.SetBkColorEnabled(whiteColor);
		if(m_preBorrow)
		{
			m_SpinBorrowPrice.SetBkColorEnabled(whiteColor);
		}
		switch(m_displaySizeMode)
		{
			case OCDSM_SIZE_FRACTION:
			m_SpinDisplaySizeFraction.SetBkColorEnabled(whiteColor);
			break;

			case OCDSM_ROUND_LOT:
			m_SpinDisplaySizeRoundLot.SetBkColorEnabled(whiteColor);
			break;
		}
	}
	else
	{
		m_SpinReplacePriceOffset.SetBkColorEnabled(grayColor);
		m_SpinReplaceSize.SetBkColorEnabled(grayColor);
		m_SpinBorrowPrice.SetBkColorEnabled(grayColor);
		m_SpinDisplaySizeFraction.SetBkColorEnabled(grayColor);
		m_SpinDisplaySizeRoundLot.SetBkColorEnabled(grayColor);
	}
	InvalidateControlsOnReplaceNative();
}

void SampleDialogSettingCancel::OnReplaceOrder()
{
	if(m_replaceOrder != (m_CheckBoxReplaceOrder.GetCheck() == BST_CHECKED))
	{
		m_replaceOrder = !m_replaceOrder;
		InvalidateControlsOnReplaceOrder();
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnReplaceNative()
{
	if(m_replaceNative != (m_CheckBoxReplaceNative.GetCheck() == BST_CHECKED))
	{
		m_replaceNative = !m_replaceNative;
//		EnableObjectUpdateButton();
		InvalidateControlsOnReplaceNative();
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnMarketOrder()
{
	const bool marketOrder = m_CheckBoxMarketOrder.GetCheck() == BST_CHECKED;
	if(marketOrder != m_marketOrder)
	{
		m_marketOrder = marketOrder;
//		EnableObjectUpdateButton();
		InvalidateControlsOnMarketOrder();
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnPreBorrow()
{
	const bool preBorrow = m_CheckBoxPreBorrow.GetCheck() == BST_CHECKED;
	if(preBorrow != m_preBorrow)
	{
		m_preBorrow = preBorrow;
//		EnableObjectUpdateButton();
		if(m_replaceOrder)
		{
			m_SpinBorrowPrice.SetBkColorEnabled(m_preBorrow ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
		}
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnReplacePriceOffsetPercent()
{
	const bool priceOffsetPercent = m_CheckBoxReplacePriceOffsetPercent.GetCheck() == BST_CHECKED;
	if(priceOffsetPercent != m_priceOffsetPercent)
	{
		m_priceOffsetPercent = priceOffsetPercent;
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnReplacePriceEnforceMinOffset()
{
	const bool enforceMinOffset = m_CheckBoxReplacePriceEnforceMinOffset.GetCheck() == BST_CHECKED;
	if(enforceMinOffset != m_priceEnforceMinOffset)
	{
		m_priceEnforceMinOffset = enforceMinOffset;
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnReplacePriceOffsetReverseForSell()
{
	const bool priceOffsetReverseForSell = m_CheckBoxReplacePriceReverseForSell.GetCheck() == BST_CHECKED;
	if(priceOffsetReverseForSell != m_priceOffsetReverseForSell)
	{
		m_priceOffsetReverseForSell = priceOffsetReverseForSell;
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnReplaceSizeOffsetPercent()
{
	const bool sizeOffsetPercent = m_CheckBoxReplaceSizeOffsetPercent.GetCheck() == BST_CHECKED;
	if(sizeOffsetPercent != m_sizeOffsetPercent)
	{
		m_sizeOffsetPercent = sizeOffsetPercent;
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnReplaceSizeOffsetReverseForSell()
{
	const bool sizeOffsetReverseForSell = m_CheckBoxReplaceSizeReverseForSell.GetCheck() == BST_CHECKED;
	if(sizeOffsetReverseForSell != m_sizeOffsetReverseForSell)
	{
		m_sizeOffsetReverseForSell = sizeOffsetReverseForSell;
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnAdjustable()
{
	const bool adjustable = m_CheckBoxReplaceAdjustable.GetCheck() == BST_CHECKED;
	if(adjustable != m_adjustable)
	{
		m_adjustable = adjustable;
		m_modified = true;
	}
}

void SampleDialogSettingCancel::OnAdjustableMmBox()
{
	const bool adjustableUseMmBox = m_CheckBoxReplaceAdjustableUseMmBox.GetCheck() == BST_CHECKED;
	if(adjustableUseMmBox != m_adjustableUseMmBox)
	{
		m_adjustableUseMmBox = adjustableUseMmBox;
		m_modified = true;
	}
}


void SampleDialogSettingCancel::OnDisplaySizeNoChange()
{
	DisplaySizeModeChange(OCDSM_COUNT);
}

void SampleDialogSettingCancel::OnDisplaySizeFraction()
{
	DisplaySizeModeChange(OCDSM_SIZE_FRACTION);
}

void SampleDialogSettingCancel::OnDisplaySizeRoundLot()
{
	DisplaySizeModeChange(OCDSM_ROUND_LOT);
}

void SampleDialogSettingCancel::DisplaySizeModeChange(unsigned char mode)
{
	if(m_displaySizeMode != mode)
	{
		RadioOwnerDraw& button = m_radioDisplaySizeMode[mode];
		if(button.GetCheck() == BST_CHECKED)
		{
			if(m_replaceOrder)
			{
				switch(m_displaySizeMode)
				{
					case OCDSM_SIZE_FRACTION:
					m_SpinDisplaySizeFraction.SetBkColorEnabled(m_visualBase->GetGrayColor());
					break;

					case OCDSM_ROUND_LOT:
					m_SpinDisplaySizeRoundLot.SetBkColorEnabled(m_visualBase->GetGrayColor());
					break;
				}
			}
			m_displaySizeMode = mode;
			if(m_replaceOrder)
			{
				switch(m_displaySizeMode)
				{
					case OCDSM_SIZE_FRACTION:
					m_SpinDisplaySizeFraction.SetBkColorEnabled(0xFFFFFFFF);//GetSysColor(COLOR_WINDOW));
					break;

					case OCDSM_ROUND_LOT:
					m_SpinDisplaySizeRoundLot.SetBkColorEnabled(0xFFFFFFFF);//GetSysColor(COLOR_WINDOW));
					break;

					default:
					m_SpinDisplaySizeFraction.SetBkColorEnabled(m_visualBase->GetGrayColor());
					m_SpinDisplaySizeRoundLot.SetBkColorEnabled(m_visualBase->GetGrayColor());
					break;
				}
			}
		}
//		EnableObjectUpdateButton();
		m_modified = true;
	}
}

void SampleDialogSettingCancel::FillDisplaySize(unsigned char displaySizeMode, unsigned int& displaySize, unsigned int& displaySizeFraction) const
{
	switch(displaySizeMode)
	{
		case OCDSM_SIZE_FRACTION:
		{
			const unsigned int size = m_SpinDisplaySizeFraction.GetValue();
			displaySize = size / 10;
			displaySizeFraction = size - 10 * displaySize;
		}
		break;

		case OCDSM_ROUND_LOT:
		displaySize = m_SpinDisplaySizeRoundLot.GetValue();
		displaySizeFraction = 0;
		break;

		default:
		displaySize = 0;
		displaySizeFraction = 0;
		break;
	}
}

