#include "stdafx.h"
#include "ExtensionDerivedConfig.h"
#include "MainDialog.h"
//#include "ConfigEnums.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern MainDialog* mainDialog;

ExtensionDerivedConfig::ExtensionDerivedConfig():
	ExtensionConfig(mainDialog),
#ifndef TAKION_NO_OPTIONS
	m_venueOptionCall(true),
	m_swipeOptionCall(true),
	m_borrowOptionCall(true),
	m_mdOptionCall(true),
	m_orderOptionCall(true),
	m_cancelOrderOptionCall(true),
	m_positionActionOptionCall(true),
	m_accountActionOptionCall(true),
	m_simQuoteOptionCall(true),
	m_simPrintOptionCall(true),
	m_simLevel1OptionCall(true),
	m_simClsPriceOptionCall(true),
	m_simResetOptionCall(true),
	m_simSnapshotOptionCall(true),
	m_simBustExecOptionCall(true),
	m_simOrderUpdateOptionCall(true),
	m_simPosLoadOptionCall(true),
#endif

	m_orderStatusMask((1 << FOST_Count) - 1),
	m_orderSideMask((1 << FOS_Count) - 1),
	m_orderInstrumentMask((1 << ST_COUNT) - 1),

	m_positionStatusMask((1 << TU_POSITION_STATUS_COUNT) - 1),
	m_positionInstrumentMask((1 << ST_COUNT) - 1),

	m_winType(0xFF),
	m_watchMessageCount(1000),
	m_miCharFrom('A'),
	m_miCharTo('Z'),
//	m_topicOrdinal(0xFFFF),
	m_newsHeadlineCount(100),
	m_allEntitled(true),
	m_simIndexSnapshotFlags(0),
	m_simLevel1Side(true),
	m_simMarketStatus('O'),
	m_simSnapshotPrimaryExchange(PE_OTHR),
	m_simSnapshotFlags(0),//(1 << SF_Count) - 1)
	m_simSnapshotRoundLot(100),
	m_simSnapshotFinancialStatus(' '),
	m_simSnapshotTradingState('T'),

	m_strategyHiLoMillisecondEntry(14400),//4AM
	m_strategyHiLoMillisecondStop(14400),
	m_strategyHiLoMillisecondExit(14400),
	m_strategyHiLoMillisecondStopExit(14400),
	m_strategyHiLoMinPrice(2, Price::priceZero),
	m_strategyHiLoMaxPrice(2, Price(999999, 0)),

	m_tcDecDigit(4),
	m_hcDecDigit(4),

	m_cancelOrderExcludeDestinations(true),
	m_cancelOrderDestinations('|', '\n', false),

	m_cancelOrderExcludeMnemonics(true),
	m_cancelOrderMnemonics('|', '\n', false, true),
	m_cancelOrderDisplaySizeMode(OCDSM_COUNT)

//	m_cancelOrderAccountAll(false)
//	m_strategyHiLoEnterVenue(*(unsigned int*)" xch"),
//	m_strategyHiLoExitVenue(m_strategyHiLoEnterVenue)
//	m_watchRequestFilter(0),
//	m_watchResponseFilter(0)
{
	AddSettings();
}

ExtensionDerivedConfig::ExtensionDerivedConfig(const ExtensionDerivedConfig& other):
	ExtensionConfig(other),
	m_msInThread(other.m_msInThread),
	m_msPositionInThread(other.m_msPositionInThread),
	m_msSendToMm(other.m_msSendToMm),
	m_msAddAll(other.m_msAddAll),
	m_msUpdateOnTimer(other.m_msUpdateOnTimer),
	m_msUpdateOnTimerMillisecond(other.m_msUpdateOnTimerMillisecond),
	m_msSecondBack(other.m_msSecondBack),
	m_msLoadOnStart(other.m_msLoadOnStart),
	m_msStockFilePath(other.m_msStockFilePath),
	m_msTimerMap(other.m_msTimerMap),

	m_mdCalculateL2(other.m_mdCalculateL2),
	m_mdLoadOnStart(other.m_mdLoadOnStart),
	m_mdStockFilePath(other.m_mdStockFilePath),

	m_venueAccountId(other.m_venueAccountId),
	m_venueSymbol(other.m_venueSymbol),
	m_venueVenue(other.m_venueVenue),
	m_venueRouting(other.m_venueRouting),
	m_venueMnemonic(other.m_venueMnemonic),
	m_venueReserveSize(other.m_venueReserveSize),
#ifndef TAKION_NO_OPTIONS
	m_venueOption(other.m_venueOption),
	m_venueOptionCall(other.m_venueOptionCall),
	m_venueOptionSuffix(other.m_venueOptionSuffix),
	m_venueOptionStrikePrice(other.m_venueOptionStrikePrice),
	m_venueOptionExpiration(other.m_venueOptionExpiration),
#endif

	m_swipeAccountId(other.m_swipeAccountId),
	m_swipeSymbol(other.m_swipeSymbol),
	m_swipeVenue(other.m_swipeVenue),
	m_swipeRouting(other.m_swipeRouting),
	m_swipeMnemonic(other.m_swipeMnemonic),
	m_swipeReserveSize(other.m_swipeReserveSize),
#ifndef TAKION_NO_OPTIONS
	m_swipeOption(other.m_swipeOption),
	m_swipeOptionCall(other.m_swipeOptionCall),
	m_swipeOptionSuffix(other.m_swipeOptionSuffix),
	m_swipeOptionStrikePrice(other.m_swipeOptionStrikePrice),
	m_swipeOptionExpiration(other.m_swipeOptionExpiration),
#endif

	m_borrowAccountId(other.m_borrowAccountId),
	m_borrowSymbol(other.m_borrowSymbol),
#ifndef TAKION_NO_OPTIONS
	m_borrowOption(other.m_borrowOption),
	m_borrowOptionCall(other.m_borrowOptionCall),
	m_borrowOptionSuffix(other.m_borrowOptionSuffix),
	m_borrowOptionStrikePrice(other.m_borrowOptionStrikePrice),
	m_borrowOptionExpiration(other.m_borrowOptionExpiration),
#endif

	m_mdAccountId(other.m_mdAccountId),
	m_mdSymbol(other.m_mdSymbol),
#ifndef TAKION_NO_OPTIONS
	m_mdOption(other.m_mdOption),
	m_mdOptionCall(other.m_mdOptionCall),
	m_mdOptionSuffix(other.m_mdOptionSuffix),
	m_mdOptionStrikePrice(other.m_mdOptionStrikePrice),
	m_mdOptionExpiration(other.m_mdOptionExpiration),
#endif

	m_orderAccountId(other.m_orderAccountId),
	m_orderSymbol(other.m_orderSymbol),
	m_orderInThread(other.m_orderInThread),
#ifndef TAKION_NO_OPTIONS
	m_orderOption(other.m_orderOption),
	m_orderOptionCall(other.m_orderOptionCall),
	m_orderOptionStrikePrice(other.m_orderOptionStrikePrice),
	m_orderOptionExpiration(other.m_orderOptionExpiration),
#endif
	m_orderStatusMask(other.m_orderStatusMask),
	m_orderSideMask(other.m_orderSideMask),
	m_orderInstrumentMask(other.m_orderInstrumentMask),
	m_orderReplaceMarketOrder(other.m_orderReplaceMarketOrder),
	m_orderReplacePriceOffsetPercent(other.m_orderReplacePriceOffsetPercent),
	m_orderReplacePriceEnforceMinOffset(other.m_orderReplacePriceEnforceMinOffset),
	m_orderReplaceSizeOffsetPercent(other.m_orderReplaceSizeOffsetPercent),
	m_orderReplacePriceOffset(other.m_orderReplacePriceOffset),
	m_orderReplaceSizeOffset(other.m_orderReplaceSizeOffset),
	m_orderReplaceNative(other.m_orderReplaceNative),
	m_orderReplacePreBorrow(other.m_orderReplacePreBorrow),
	m_orderReplacePreBorrowPrice(other.m_orderReplacePreBorrowPrice),
	m_orderFuture(other.m_orderFuture),

	m_positionAccountId(other.m_positionAccountId),
	m_positionInThread(other.m_positionInThread),
	m_positionStatusMask(other.m_positionStatusMask),
	m_positionInstrumentMask(other.m_positionInstrumentMask),

	m_histPrintSymbol(other.m_histPrintSymbol),
	m_histPrintOddLot(other.m_histPrintOddLot),
	m_histPrintFilter(other.m_histPrintFilter),

	m_accountActionAccountId(other.m_accountActionAccountId),
	m_accountActionMsSymbol(other.m_accountActionMsSymbol),
	m_accountActionMdSymbol(other.m_accountActionMdSymbol),
#ifndef TAKION_NO_OPTIONS
	m_accountActionOption(other.m_accountActionOption),
	m_accountActionOptionCall(other.m_accountActionOptionCall),
	m_accountActionOptionSuffix(other.m_accountActionOptionSuffix),
	m_accountActionOptionStrikePrice(other.m_accountActionOptionStrikePrice),
	m_accountActionOptionExpiration(other.m_accountActionOptionExpiration),
#endif

	m_positionActionAccountId(other.m_positionActionAccountId),
	m_positionActionSymbol(other.m_positionActionSymbol),
#ifndef TAKION_NO_OPTIONS
	m_positionActionOption(other.m_positionActionOption),
	m_positionActionOptionCall(other.m_positionActionOptionCall),
	m_positionActionOptionSuffix(other.m_positionActionOptionSuffix),
	m_positionActionOptionStrikePrice(other.m_positionActionOptionStrikePrice),
	m_positionActionOptionExpiration(other.m_positionActionOptionExpiration),
#endif

	m_strategyHiLoAccountId(other.m_strategyHiLoAccountId),
	m_strategyHiLoStockFilePath(other.m_strategyHiLoStockFilePath),
	m_strategyHiLoLoadOnStart(other.m_strategyHiLoLoadOnStart),
	m_strategyHiLoLoadFromFile(other.m_strategyHiLoLoadFromFile),
	m_strategyHiLoSymbolDelimiter(other.m_strategyHiLoSymbolDelimiter),
	m_strategyHiLoTradeOnStart(other.m_strategyHiLoTradeOnStart),
	m_strategyHiLoMillisecondEntry(other.m_strategyHiLoMillisecondEntry),
	m_strategyHiLoMillisecondStop(other.m_strategyHiLoMillisecondStop),
	m_strategyHiLoMillisecondExit(other.m_strategyHiLoMillisecondExit),
	m_strategyHiLoMillisecondStopExit(other.m_strategyHiLoMillisecondStopExit),
	m_strategyHiLoYesterdayVolumeLimit(other.m_strategyHiLoYesterdayVolumeLimit),
	m_strategyHiLoMinPrice(other.m_strategyHiLoMinPrice),
	m_strategyHiLoMaxPrice(other.m_strategyHiLoMaxPrice),
	m_strategyHiLoOrderSize(other.m_strategyHiLoOrderSize),
	m_strategyHiLoEnterVenue(other.m_strategyHiLoEnterVenue),
	m_strategyHiLoEnterRouting(other.m_strategyHiLoEnterRouting),
	m_strategyHiLoEnterMnemonic(other.m_strategyHiLoEnterMnemonic),
	m_strategyHiLoEnterReserveSize(other.m_strategyHiLoEnterReserveSize),
	m_strategyHiLoExitVenue(other.m_strategyHiLoExitVenue),
	m_strategyHiLoExitRouting(other.m_strategyHiLoExitRouting),
	m_strategyHiLoExitMnemonic(other.m_strategyHiLoExitMnemonic),
	m_strategyHiLoExitReserveSize(other.m_strategyHiLoExitReserveSize),

	m_winType(other.m_winType),
	m_extTools(other.m_extTools),
	m_watchRequestFilter(other.m_watchRequestFilter),
	m_watchResponseFilter(other.m_watchResponseFilter),
	m_watchMessageCount(other.m_watchMessageCount),
	m_watchSuspended(other.m_watchSuspended),

	m_simQuoteSymbolType(other.m_simQuoteSymbolType),
	m_simQuoteSymbol(other.m_simQuoteSymbol),
	m_simQuoteSymbolMap(other.m_simQuoteSymbolMap),
#ifndef TAKION_NO_OPTIONS
	m_simQuoteOption(other.m_simQuoteOption),
	m_simQuoteOptionCall(other.m_simQuoteOptionCall),
	m_simQuoteOptionSuffix(other.m_simQuoteOptionSuffix),
	m_simQuoteOptionStrikePrice(other.m_simQuoteOptionStrikePrice),
	m_simQuoteOptionExpiration(other.m_simQuoteOptionExpiration),
#endif
	m_simQuoteRefNumber(other.m_simQuoteRefNumber),
	m_simQuoteSide(other.m_simQuoteSide),
	m_simQuoteBookName(other.m_simQuoteBookName),
	m_simQuotePrice(other.m_simQuotePrice),
	m_simQuoteSize(other.m_simQuoteSize),
	m_simQuoteCondition(other.m_simQuoteCondition),
	m_simQuoteRetailLiquidity(other.m_simQuoteRetailLiquidity),
	m_simQuoteAttribution(other.m_simQuoteAttribution),
	m_simQuoteUseRemainingSize(other.m_simQuoteUseRemainingSize),
	m_simQuoteRemainingSize(other.m_simQuoteRemainingSize),
	m_simQuotePrintable(other.m_simQuotePrintable),
	m_simQuoteRefAutoIncrement(other.m_simQuoteRefAutoIncrement),
	m_simQuoteMessageName(other.m_simQuoteMessageName),

	m_simPrintSymbolType(other.m_simPrintSymbolType),
	m_simPrintSymbol(other.m_simPrintSymbol),
	m_simPrintSymbolMap(other.m_simPrintSymbolMap),
	m_simPrintSource(other.m_simPrintSource),
#ifndef TAKION_NO_OPTIONS
	m_simPrintOption(other.m_simPrintOption),
	m_simPrintOptionCall(other.m_simPrintOptionCall),
	m_simPrintOptionSuffix(other.m_simPrintOptionSuffix),
	m_simPrintOptionStrikePrice(other.m_simPrintOptionStrikePrice),
	m_simPrintOptionExpiration(other.m_simPrintOptionExpiration),
#endif
	m_simPrintRefNumber(other.m_simPrintRefNumber),
	m_simPrintPrice(other.m_simPrintPrice),
	m_simPrintSize(other.m_simPrintSize),
	m_simPrintSaleCondition1(other.m_simPrintSaleCondition1),
	m_simPrintSaleCondition2(other.m_simPrintSaleCondition2),
	m_simPrintSaleCondition3(other.m_simPrintSaleCondition3),
	m_simPrintSaleCondition4(other.m_simPrintSaleCondition4),
	m_simPrintMarketCenter(other.m_simPrintMarketCenter),
	m_simPrintMarketListing(other.m_simPrintMarketListing),

	m_simPrintMessageType(other.m_simPrintMessageType),
	m_simPrintLastPrice(other.m_simPrintLastPrice),
	m_simPrintHighPrice(other.m_simPrintHighPrice),
	m_simPrintLowPrice(other.m_simPrintLowPrice),
	m_simPrintVolume(other.m_simPrintVolume),

	m_simPrintRefNumberCorr(other.m_simPrintRefNumberCorr),
	m_simPrintPriceCorr(other.m_simPrintPriceCorr),
	m_simPrintSizeCorr(other.m_simPrintSizeCorr),
	m_simPrintSaleCondition1Corr(other.m_simPrintSaleCondition1Corr),
	m_simPrintSaleCondition2Corr(other.m_simPrintSaleCondition2Corr),
	m_simPrintSaleCondition3Corr(other.m_simPrintSaleCondition3Corr),
	m_simPrintSaleCondition4Corr(other.m_simPrintSaleCondition4Corr),

	m_simPrintRefAutoIncrement(other.m_simPrintRefAutoIncrement),
	m_simPrintRefCorrAutoIncrement(other.m_simPrintRefCorrAutoIncrement),

	m_simLevel1SymbolType(other.m_simLevel1SymbolType),
	m_simLevel1Symbol(other.m_simLevel1Symbol),
	m_simLevel1SymbolMap(other.m_simLevel1SymbolMap),
	m_simLevel1Source(other.m_simLevel1Source),
#ifndef TAKION_NO_OPTIONS
	m_simLevel1Option(other.m_simLevel1Option),
	m_simLevel1OptionCall(other.m_simLevel1OptionCall),
	m_simLevel1OptionSuffix(other.m_simLevel1OptionSuffix),
	m_simLevel1OptionStrikePrice(other.m_simLevel1OptionStrikePrice),
	m_simLevel1OptionExpiration(other.m_simLevel1OptionExpiration),
#endif
	m_simLevel1Bid(other.m_simLevel1Bid),
	m_simLevel1Ask(other.m_simLevel1Ask),
	m_simLevel1BidSize(other.m_simLevel1BidSize),
	m_simLevel1AskSize(other.m_simLevel1AskSize),
	m_simLevel1Side(other.m_simLevel1Side),
	m_simLevel1SaleCondition(other.m_simLevel1SaleCondition),
	m_simLevel1BidMarketCenter(other.m_simLevel1BidMarketCenter),
	m_simLevel1AskMarketCenter(other.m_simLevel1AskMarketCenter),
	m_simLevel1MarketListing(other.m_simLevel1MarketListing),

	m_simLrpLuldSymbolType(other.m_simLrpLuldSymbolType),
	m_simLrpLuldSymbol(other.m_simLrpLuldSymbol),
	m_simLrpLuldSymbolMap(other.m_simLrpLuldSymbolMap),
	m_simLrpLuldSource(other.m_simLrpLuldSource),
	m_simLrpLuldMessageType(other.m_simLrpLuldMessageType),
	m_simLrpLuldPriceLow(other.m_simLrpLuldPriceLow),
	m_simLrpLuldPriceHigh(other.m_simLrpLuldPriceHigh),

	m_simStockTypeSymbolType(other.m_simStockTypeSymbolType),
	m_simStockTypeSymbol(other.m_simStockTypeSymbol),
	m_simStockTypeSymbolMap(other.m_simStockTypeSymbolMap),
	m_simStockTypeSource(other.m_simStockTypeSource),
	m_simStockTypeSecurityType(other.m_simStockTypeSecurityType),

	m_simTradingActionSymbolType(other.m_simTradingActionSymbolType),
	m_simTradingActionSymbol(other.m_simTradingActionSymbol),
	m_simTradingActionSymbolMap(other.m_simTradingActionSymbolMap),
	m_simTradingActionSource(other.m_simTradingActionSource),
	m_simTradingActionTradingState(other.m_simTradingActionTradingState),
	m_simTradingActionReason(other.m_simTradingActionReason),

	m_simClsPriceSymbolType(other.m_simClsPriceSymbolType),
	m_simClsPriceSymbol(other.m_simClsPriceSymbol),
	m_simClsPriceSymbolMap(other.m_simClsPriceSymbolMap),
	m_simClsPriceSource(other.m_simClsPriceSource),
	m_simClsPrice(other.m_simClsPrice),
#ifndef TAKION_NO_OPTIONS
	m_simClsPriceOption(other.m_simClsPriceOption),
	m_simClsPriceOptionCall(other.m_simClsPriceOptionCall),
	m_simClsPriceOptionSuffix(other.m_simClsPriceOptionSuffix),
	m_simClsPriceOptionStrikePrice(other.m_simClsPriceOptionStrikePrice),
	m_simClsPriceOptionExpiration(other.m_simClsPriceOptionExpiration),
#endif
/*
	m_simInvalidSymbolSymbolType(other.m_simInvalidSymbolSymbolType),
	m_simInvalidSymbolSymbol(other.m_simInvalidSymbolSymbol),
//	m_simInvalidSymbolSymbolMap(other.m_simInvalidSymbolSymbolMap),
	m_simInvalidSymbolSource(other.m_simInvalidSymbolSource),
#ifndef TAKION_NO_OPTIONS
	m_simInvalidSymbolOption(other.m_simInvalidSymbolOption),
	m_simInvalidSymbolOptionCall(other.m_simInvalidSymbolOptionCall),
	m_simInvalidSymbolOptionSuffix(other.m_simInvalidSymbolOptionSuffix),
	m_simInvalidSymbolOptionStrikePrice(other.m_simInvalidSymbolOptionStrikePrice),
	m_simInvalidSymbolOptionExpiration(other.m_simInvalidSymbolOptionExpiration),
#endif
*/
	m_simResetSymbolType(other.m_simResetSymbolType),
	m_simResetSymbol(other.m_simResetSymbol),
	m_simResetSymbolMap(other.m_simResetSymbolMap),
	m_simResetSource(other.m_simResetSource),
	m_simResetMessageType(other.m_simResetMessageType),
	m_simResetBook(other.m_simResetBook),
#ifndef TAKION_NO_OPTIONS
	m_simResetOption(other.m_simResetOption),
	m_simResetOptionCall(other.m_simResetOptionCall),
	m_simResetOptionSuffix(other.m_simResetOptionSuffix),
	m_simResetOptionStrikePrice(other.m_simResetOptionStrikePrice),
	m_simResetOptionExpiration(other.m_simResetOptionExpiration),
#endif

	m_simIndexSnapshotSymbol(other.m_simIndexSnapshotSymbol),
	m_simIndexSnapshotFlags(other.m_simIndexSnapshotFlags),
	m_simIndexSnapshotDescription(other.m_simIndexSnapshotDescription),

	m_simSnapshotSymbolType(other.m_simSnapshotSymbolType),
	m_simSnapshotSymbol(other.m_simSnapshotSymbol),
	m_simSnapshotSymbolMap(other.m_simSnapshotSymbolMap),
	m_simSnapshotSource(other.m_simSnapshotSource),
	m_simSnapshotMarketStatus(other.m_simSnapshotMarketStatus),
	m_simSnapshotPrimaryExchange(other.m_simSnapshotPrimaryExchange),
	m_simSnapshotFlags(other.m_simSnapshotFlags),
	m_simSnapshotDescription(other.m_simSnapshotDescription),
	m_simSnapshotRoundLot(other.m_simSnapshotRoundLot),
	m_simSnapshotRoundLotsOnly(other.m_simSnapshotRoundLotsOnly),
	m_simSnapshotMarketCategory(other.m_simSnapshotMarketCategory),
	m_simSnapshotFinancialStatus(other.m_simSnapshotFinancialStatus),
	m_simSnapshotTradingState(other.m_simSnapshotTradingState),
	m_simSnapshotTradingActionReason(other.m_simSnapshotTradingActionReason),
#ifndef TAKION_NO_OPTIONS
	m_simSnapshotOption(other.m_simSnapshotOption),
	m_simSnapshotOptionCall(other.m_simSnapshotOptionCall),
	m_simSnapshotOptionSuffix(other.m_simSnapshotOptionSuffix),
	m_simSnapshotOptionStrikePrice(other.m_simSnapshotOptionStrikePrice),
	m_simSnapshotOptionExpiration(other.m_simSnapshotOptionExpiration),
#endif

	m_simAcctRiskAccountId(other.m_simAcctRiskAccountId),

	m_simBustExecAccountId(other.m_simBustExecAccountId),
	m_simBustExecRefNumber(other.m_simBustExecRefNumber),
	m_simBustExecRef(other.m_simBustExecRef),
	m_simBustExecBusted(other.m_simBustExecBusted),
	m_simBustExecDropcopy(other.m_simBustExecDropcopy),
	m_simBustExecSymbol(other.m_simBustExecSymbol),
#ifndef TAKION_NO_OPTIONS
	m_simBustExecOption(other.m_simBustExecOption),
	m_simBustExecOptionCall(other.m_simBustExecOptionCall),
	m_simBustExecOptionSuffix(other.m_simBustExecOptionSuffix),
	m_simBustExecOptionStrikePrice(other.m_simBustExecOptionStrikePrice),
	m_simBustExecOptionExpiration(other.m_simBustExecOptionExpiration),
#endif
	m_simBustExecSize(other.m_simBustExecSize),
	m_simBustExecPrice(other.m_simBustExecPrice),
	m_simBustExecSide(other.m_simBustExecSide),
	m_simBustExecOrderSize(other.m_simBustExecOrderSize),
	m_simBustExecMmid(other.m_simBustExecMmid),
	m_simBustExecVenue(other.m_simBustExecVenue),
	m_simBustExecLiquidity(other.m_simBustExecLiquidity),
//	m_simBustExecOrderPrice(other.m_simBustExecOrderPrice),
	m_simBustExecSendMessageToAllAccountsDelay(other.m_simBustExecSendMessageToAllAccountsDelay),

	m_simOrderUpdateAccountId(other.m_simOrderUpdateAccountId),
	m_simOrderUpdateSymbol(other.m_simOrderUpdateSymbol),
#ifndef TAKION_NO_OPTIONS
	m_simOrderUpdateOption(other.m_simOrderUpdateOption),
	m_simOrderUpdateOptionCall(other.m_simOrderUpdateOptionCall),
	m_simOrderUpdateOptionSuffix(other.m_simOrderUpdateOptionSuffix),
	m_simOrderUpdateOptionStrikePrice(other.m_simOrderUpdateOptionStrikePrice),
	m_simOrderUpdateOptionExpiration(other.m_simOrderUpdateOptionExpiration),
#endif
	m_simOrderUpdateSize(other.m_simOrderUpdateSize),
	m_simOrderUpdateRemaining(other.m_simOrderUpdateRemaining),
	m_simOrderUpdateClientId(other.m_simOrderUpdateClientId),
	m_simOrderUpdateServerId(other.m_simOrderUpdateServerId),
	m_simOrderUpdateReasonCode(other.m_simOrderUpdateReasonCode),
	m_simOrderUpdatePrice(other.m_simOrderUpdatePrice),
	m_simOrderUpdateBid(other.m_simOrderUpdateBid),
	m_simOrderUpdateAsk(other.m_simOrderUpdateAsk),
	m_simOrderUpdateLast(other.m_simOrderUpdateLast),
	m_simOrderUpdateHistorical(other.m_simOrderUpdateHistorical),
//	m_simOrderUpdateReroutedToBaml(other.m_simOrderUpdateReroutedToBaml),
	m_simOrderUpdateReRoute(other.m_simOrderUpdateReRoute),
	m_simOrderUpdateMessageType(other.m_simOrderUpdateMessageType),
	m_simOrderUpdateMarket(other.m_simOrderUpdateMarket),
	m_simOrderUpdateSide(other.m_simOrderUpdateSide),
	m_simOrderUpdateReserveSize(other.m_simOrderUpdateReserveSize),
	m_simOrderUpdateVenue(other.m_simOrderUpdateVenue),
	m_simOrderUpdateRouting(other.m_simOrderUpdateRouting),
	m_simOrderUpdateMmid(other.m_simOrderUpdateMmid),
	m_simOrderUpdateMarketReferenceId(other.m_simOrderUpdateMarketReferenceId),
//	m_simOrderUpdateBorrowAccept(other.m_simOrderUpdateBorrowAccept),
	m_simOrderUpdateExecutionInstructions(other.m_simOrderUpdateExecutionInstructions),
	m_simOrderUpdateLiquidity1(other.m_simOrderUpdateLiquidity1),
	m_simOrderUpdateLiquidity2(other.m_simOrderUpdateLiquidity2),
	m_simOrderUpdateExecutionFilled(other.m_simOrderUpdateExecutionFilled),
	m_simOrderUpdateExecutionCancelled(other.m_simOrderUpdateExecutionCancelled),
	m_simOrderUpdateExecutionRemaining(other.m_simOrderUpdateExecutionRemaining),
	m_simOrderUpdateExecutionPrice(other.m_simOrderUpdateExecutionPrice),
	m_simOrderUpdateExecutionVenue(other.m_simOrderUpdateExecutionVenue),
	m_simOrderUpdateExecutionMmid(other.m_simOrderUpdateExecutionMmid),
	m_simOrderUpdateExecutionReferenceId(other.m_simOrderUpdateExecutionReferenceId),

	m_simOrderUpdateExecutionReportFlags(other.m_simOrderUpdateExecutionReportFlags),

#ifndef TAKION_NO_OPTIONS
	m_simPosLoadOption(other.m_simPosLoadOption),
	m_simPosLoadOptionCall(other.m_simPosLoadOptionCall),
	m_simPosLoadOptionSuffix(other.m_simPosLoadOptionSuffix),
	m_simPosLoadOptionStrikePrice(other.m_simPosLoadOptionStrikePrice),
	m_simPosLoadOptionExpiration(other.m_simPosLoadOptionExpiration),
#endif
	m_simPosLoadAccountId(other.m_simPosLoadAccountId),
	m_simPosLoadSymbol(other.m_simPosLoadSymbol),
	m_simPosLoadSize(other.m_simPosLoadSize),
	m_simPosLoadPrice(other.m_simPosLoadPrice),
	m_simPosLoadCost(other.m_simPosLoadCost),
	m_simPosLoadOvernightSize(other.m_simPosLoadOvernightSize),
	m_simPosLoadOvernightPrice(other.m_simPosLoadOvernightPrice),
	m_simPosLoadOpenOvernightSize(other.m_simPosLoadOpenOvernightSize),
	m_simPosLoadClsLongSize(other.m_simPosLoadClsLongSize),
	m_simPosLoadClsLongCost(other.m_simPosLoadClsLongCost),
	m_simPosLoadClsLongValue(other.m_simPosLoadClsLongValue),
	m_simPosLoadClsShortSize(other.m_simPosLoadClsShortSize),
	m_simPosLoadClsShortCost(other.m_simPosLoadClsShortCost),
	m_simPosLoadClsShortValue(other.m_simPosLoadClsShortValue),
	m_simPosLoadBorrowSize(other.m_simPosLoadBorrowSize),
	m_simPosLoadBorrowPrice(other.m_simPosLoadBorrowPrice),
	m_simPosLoadSendMessageToAllAccountsDelay(other.m_simPosLoadSendMessageToAllAccountsDelay),

	m_simSlAccountId(other.m_simSlAccountId),
	m_simSlSymbol(other.m_simSlSymbol),
	m_simSlRequestId(other.m_simSlRequestId),
	m_simSlFirm(other.m_simSlFirm),
	m_simSlBorrowPrice(other.m_simSlBorrowPrice),
	m_simSlBeta(other.m_simSlBeta),
	m_simSlBorrowSize(other.m_simSlBorrowSize),
	m_simSlDiscountFlag(other.m_simSlDiscountFlag),
	m_simSlOnTheFly(other.m_simSlOnTheFly),
	m_simSlMarginable(other.m_simSlMarginable),
	m_simSlOnTheFlyBorrow(other.m_simSlOnTheFlyBorrow),
	m_simSlHtb(other.m_simSlHtb),
	m_simSlAssetType(other.m_simSlAssetType),
	m_simSlSecType(other.m_simSlSecType),
	m_simSlStatus(other.m_simSlStatus),
	m_simSlMargin(other.m_simSlMargin),
	m_simSlPmElRate(other.m_simSlPmElRate),
	m_simSlPmEsRate(other.m_simSlPmEsRate),
	m_simSlPmHlRate(other.m_simSlPmHlRate),
	m_simSlPmHsRate(other.m_simSlPmHsRate),
	m_simSlElRate(other.m_simSlElRate),
	m_simSlEsRate(other.m_simSlEsRate),
	m_simSlHlRate(other.m_simSlHlRate),
	m_simSlHsRate(other.m_simSlHsRate),
	m_simSlFedReq(other.m_simSlFedReq),

	m_simEtbListAccountId(other.m_simEtbListAccountId),
	m_simEtbListSymbol(other.m_simEtbListSymbol),
	m_simEtbListRequestId(other.m_simEtbListRequestId),
	m_simEtbListFirm(other.m_simEtbListFirm),
	m_simEtbListFlags(other.m_simEtbListFlags),
	m_simEtbListDone(other.m_simEtbListDone),

	m_simFirmPositionAccountId(other.m_simFirmPositionAccountId),
	m_simFirmPositionSymbol(other.m_simFirmPositionSymbol),
	m_simFirmPositionSize(other.m_simFirmPositionSize),
	m_simFirmPositionPendingSellPassive(other.m_simFirmPositionPendingSellPassive),
	m_simFirmPositionPendingSellNonPassive(other.m_simFirmPositionPendingSellNonPassive),
	m_simFirmPositionPendingShortPassive(other.m_simFirmPositionPendingShortPassive),
	m_simFirmPositionPendingShortNonPassive(other.m_simFirmPositionPendingShortNonPassive),

	m_simUsAccountId(other.m_simUsAccountId),
	m_simUsSymbol(other.m_simUsSymbol),
	m_simUsFirm(other.m_simUsFirm),
	m_simUsTraderId(other.m_simUsTraderId),
	m_simUsStatus(other.m_simUsStatus),//Untradeable Status. 0 - tradeable; 1 - untradeable full; 2 - untradeable partial

	m_simEnforceAggregationClearingFirmId(other.m_simEnforceAggregationClearingFirmId),
	m_simEnforceAggregationEnforce(other.m_simEnforceAggregationEnforce),
	m_simEnforceAggregationConservativeOrderMarking(other.m_simEnforceAggregationConservativeOrderMarking),
	m_simEnforceAggregationLocalSideMarking(other.m_simEnforceAggregationLocalSideMarking),
	m_simEnforceAggregationEnforceNiteBetaTimeSeconds(other.m_simEnforceAggregationEnforceNiteBetaTimeSeconds),
	m_simEnforceAggregationNiteBetaBp(other.m_simEnforceAggregationNiteBetaBp),

	m_simMarketStatus(other.m_simMarketStatus),
	m_simMarketStatusSymbolMapping(other.m_simMarketStatusSymbolMapping),

	m_simImbalanceSymbolType(other.m_simImbalanceSymbolType),
	m_simImbalanceSymbol(other.m_simImbalanceSymbol),
	m_simImbalanceSymbolMap(other.m_simImbalanceSymbolMap),
	m_simImbalanceSource(other.m_simImbalanceSource),
	m_simImbalanceBookId(other.m_simImbalanceBookId),
	m_simImbalanceType(other.m_simImbalanceType),
	m_simImbalanceDirection(other.m_simImbalanceDirection),
	m_simImbalanceRegulatory(other.m_simImbalanceRegulatory),
	m_simImbalanceFreezeStatus(other.m_simImbalanceFreezeStatus),
	m_simImbalance(other.m_simImbalance),
	m_simMarketImbalance(other.m_simMarketImbalance),
	m_simPairedShares(other.m_simPairedShares),
	m_simImbalancePriceFar(other.m_simImbalancePriceFar),
	m_simImbalancePriceNear(other.m_simImbalancePriceNear),
	m_simImbalancePriceRef(other.m_simImbalancePriceRef),
	m_simImbalancePv(other.m_simImbalancePv),
	m_simImbalanceAuctionTimeSeconds(other.m_simImbalanceAuctionTimeSeconds),

	m_simChartCorrectionSymbolType(other.m_simChartCorrectionSymbolType),
	m_simChartCorrectionSymbol(other.m_simChartCorrectionSymbol),
	m_simChartCorrectionSymbolMap(other.m_simChartCorrectionSymbolMap),
	m_simChartCorrectionPriceStart(other.m_simChartCorrectionPriceStart),
	m_simChartCorrectionPriceHigh(other.m_simChartCorrectionPriceHigh),
	m_simChartCorrectionPriceLow(other.m_simChartCorrectionPriceLow),
	m_simChartCorrectionPriceEnd(other.m_simChartCorrectionPriceEnd),
	m_simChartCorrectionMoneyTraded(other.m_simChartCorrectionMoneyTraded),
	m_simChartCorrectionVolume(other.m_simChartCorrectionVolume),
	m_simChartCorrectionMinute(other.m_simChartCorrectionMinute),

	m_simChartSnapshotSymbolType(other.m_simChartSnapshotSymbolType),
	m_simChartSnapshotSymbol(other.m_simChartSnapshotSymbol),
	m_simChartSnapshotSymbolMap(other.m_simChartSnapshotSymbolMap),
	m_simChartSnapshotRequestId(other.m_simChartSnapshotRequestId),
	m_simChartSnapshotPointCount(other.m_simChartSnapshotPointCount),
	m_simChartSnapshotStartMinute(other.m_simChartSnapshotStartMinute),
	m_simChartSnapshotPriceStart(other.m_simChartSnapshotPriceStart),
	m_simChartSnapshotPriceHigh(other.m_simChartSnapshotPriceHigh),
	m_simChartSnapshotPriceLow(other.m_simChartSnapshotPriceLow),
	m_simChartSnapshotPriceEnd(other.m_simChartSnapshotPriceEnd),
	m_simChartSnapshotMoneyTraded(other.m_simChartSnapshotMoneyTraded),
	m_simChartSnapshotVolume(other.m_simChartSnapshotVolume),
//	m_simChartSnapshotFlags(other.m_simChartSnapshotFlags),
	m_simChartSnapshotDone(other.m_simChartSnapshotDone),

	m_simAdminBroadcastText(other.m_simAdminBroadcastText),

	m_tcSymbol(other.m_tcSymbol),
	m_tcDecDigit(other.m_tcDecDigit),

	m_hcAllChartFileName(other.m_hcAllChartFileName),
	m_hcSymbol(other.m_hcSymbol),
	m_hcDecDigit(other.m_hcDecDigit),
	m_hcDayCountMode(other.m_hcDayCountMode),
	m_hcDayCount(other.m_hcDayCount),
	m_hcDateFrom(other.m_hcDateFrom),
	m_hcFrequency(other.m_hcFrequency),
//	m_topicOrdinal(other.m_topicOrdinal),

//	m_miLoadOnStart(other.m_miLoadOnStart),
	m_miInThread(other.m_miInThread),
	m_miSendToMm(other.m_miSendToMm),
	m_miMarketCategoryFilter(other.m_miMarketCategoryFilter),
	m_miTestStockFilter(other.m_miTestStockFilter),
	m_miCharFrom(other.m_miCharFrom),
	m_miCharTo(other.m_miCharTo),
	m_miYestVolumeFrom(other.m_miYestVolumeFrom),
	m_miYestVolumeTo(other.m_miYestVolumeTo),

	m_simMdLogonMdVersion(other.m_simMdLogonMdVersion),
	m_simMdLogonCompression(other.m_simMdLogonCompression),
	m_simMdLogonInstrument(other.m_simMdLogonInstrument),
	m_simMdLogonResult(other.m_simMdLogonResult),
	m_simMdLogonSubscriptionOnly(other.m_simMdLogonSubscriptionOnly),
	m_simMdLogonForceSubscriptionOnly(other.m_simMdLogonForceSubscriptionOnly),
	m_simMdLogonDisableExtensions(other.m_simMdLogonDisableExtensions),
	m_simMdLogonEntitlements(other.m_simMdLogonEntitlements),
	m_simMdLogonExtEntitlements(other.m_simMdLogonExtEntitlements),

	m_topic(other.m_topic),
	m_newsDaysBack(other.m_newsDaysBack),
	m_newsHeadlineCount(other.m_newsHeadlineCount),
	m_newsAfterMarket(other.m_newsAfterMarket),
	m_searchType(other.m_searchType),
	m_allEntitled(other.m_allEntitled),
	m_storyFilter(other.m_storyFilter),
	m_metaFilter(other.m_metaFilter),
	m_relevance(other.m_relevance),
	m_showStoryChain(other.m_showStoryChain),
	m_wireSettingSet(other.m_wireSettingSet),

	m_cancelOrderAccountId(other.m_cancelOrderAccountId),
	m_cancelOrderSymbol(other.m_cancelOrderSymbol),
#ifndef TAKION_NO_OPTIONS
	m_cancelOrderOption(other.m_cancelOrderOption),
	m_cancelOrderOptionCall(other.m_cancelOrderOptionCall),
	m_cancelOrderOptionSuffix(other.m_cancelOrderOptionSuffix),
	m_cancelOrderOptionStrikePrice(other.m_cancelOrderOptionStrikePrice),
	m_cancelOrderOptionExpiration(other.m_cancelOrderOptionExpiration),
#endif
	m_cancelOrderClientId(other.m_cancelOrderClientId),
	m_cancelOrderUseClientId(other.m_cancelOrderUseClientId),
	m_cancelOrderAccountAll(other.m_cancelOrderAccountAll),
	m_cancelOrderPosition(other.m_cancelOrderPosition),
#ifndef TAKION_NO_OPTIONS
	m_cancelOrderInstrument(other.m_cancelOrderInstrument),
#endif
	m_cancelOrderSide(other.m_cancelOrderSide),
	m_cancelOrderDirection(other.m_cancelOrderDirection),
	m_cancelOrderPick(other.m_cancelOrderPick),
	m_cancelOrderTif(other.m_cancelOrderTif),
	m_cancelOrderType(other.m_cancelOrderType),
	m_cancelOrderPqOpen(other.m_cancelOrderPqOpen),
	m_cancelOrderTClose(other.m_cancelOrderTClose),
	m_cancelOrderExcludeDestinations(other.m_cancelOrderExcludeDestinations),
	m_cancelOrderDestinations(other.m_cancelOrderDestinations),
	m_cancelOrderExcludeMnemonics(other.m_cancelOrderExcludeMnemonics),
	m_cancelOrderMnemonics(other.m_cancelOrderMnemonics),
	
	m_cancelOrderReplace(other.m_cancelOrderReplace),
	m_cancelOrderReplaceNative(other.m_cancelOrderReplaceNative),
	m_cancelOrderReplaceMarket(other.m_cancelOrderReplaceMarket),
	m_cancelOrderReplacePriceOffsetPercent(other.m_cancelOrderReplacePriceOffsetPercent),
	m_cancelOrderReplacePriceEnforceMinOffset(other.m_cancelOrderReplacePriceEnforceMinOffset),
	m_cancelOrderReplacePriceOffsetReverseForSell(other.m_cancelOrderReplacePriceOffsetReverseForSell),
	m_cancelOrderReplaceSizeOffsetPercent(other.m_cancelOrderReplaceSizeOffsetPercent),
	m_cancelOrderReplaceSizeOffsetReverseForSell(other.m_cancelOrderReplaceSizeOffsetReverseForSell),
	m_cancelOrderReplaceAdjustable(other.m_cancelOrderReplaceAdjustable),
	m_cancelOrderReplaceAdjustableUseMmBox(other.m_cancelOrderReplaceAdjustableUseMmBox),
	m_cancelOrderReplacePreBorrow(other.m_cancelOrderReplacePreBorrow),
	m_cancelOrderDisplaySizeMode(other.m_cancelOrderDisplaySizeMode),
	m_cancelOrdeReplacePriceOffset(other.m_cancelOrdeReplacePriceOffset),
	m_cancelOrdeReplaceSizeOffset(other.m_cancelOrdeReplaceSizeOffset),
	m_cancelOrdeReplaceBorrowPrice(other.m_cancelOrdeReplaceBorrowPrice),
	m_cancelOrdeReplaceDisplaySizeFraction(other.m_cancelOrdeReplaceDisplaySizeFraction),
	m_cancelOrdeReplaceDisplaySizeRoundLot(other.m_cancelOrdeReplaceDisplaySizeRoundLot),

	m_commandPost(other.m_commandPost),
	m_commandName(other.m_commandName),
	m_commandSymbol(other.m_commandSymbol)
{
	AddSettings();
}

void ExtensionDerivedConfig::AddSettings()
{
	AddSetting("MS_InThread", m_msInThread);
	AddSetting("MS_PositionInThread", m_msPositionInThread);
	AddSetting("MS_SendToMm", m_msSendToMm);
	AddSetting("MS_AddAll", m_msAddAll);
	AddSetting("MS_UpdateOnTimer", m_msUpdateOnTimer);
	AddSetting("MS_UpdateOnTimerMilliseccond", m_msUpdateOnTimerMillisecond);
	AddSetting("MS_SecondBack", m_msSecondBack);
	AddSetting("MS_LoadOnStart", m_msLoadOnStart);
	AddSetting("MS_StockFilePath", m_msStockFilePath);
	AddSetting("MS_TimerMap", m_msTimerMap);

	AddSetting("MD_CalculateL2", m_mdCalculateL2);
	AddSetting("MD_LoadOnStart", m_mdLoadOnStart);
	AddSetting("MD_StockFilePath", m_mdStockFilePath);

	AddSetting("Venue_AccountId", m_venueAccountId);
	AddSetting("Venue_Symbol", m_venueSymbol);
	AddSetting("Venue_Venue", m_venueVenue),
	AddSetting("Venue_Routing", m_venueRouting),
	AddSetting("Venue_Mnemonic", m_venueMnemonic),
	AddSetting("Venue_ReserveSize", m_venueReserveSize),
#ifndef TAKION_NO_OPTIONS
	AddSetting("Venue_Option", m_venueOption);
	AddSetting("Venue_OptionCall", m_venueOptionCall);
	AddSetting("Venue_OptionSuffix", m_venueOptionSuffix);
	AddSetting("Venue_OptionStrikePrice", m_venueOptionStrikePrice);
	AddSetting("Venue_OptionExpiration", m_venueOptionExpiration);
#endif

	AddSetting("Swipe_AccountId", m_swipeAccountId);
	AddSetting("Swipe_Symbol", m_swipeSymbol);
	AddSetting("Swipe_Venue", m_swipeVenue),
	AddSetting("Swipe_Routing", m_swipeRouting),
	AddSetting("Swipe_Mnemonic", m_swipeMnemonic),
	AddSetting("Swipe_ReserveSize", m_swipeReserveSize),
#ifndef TAKION_NO_OPTIONS
	AddSetting("Swipe_Option", m_swipeOption);
	AddSetting("Swipe_OptionCall", m_swipeOptionCall);
	AddSetting("Swipe_OptionSuffix", m_swipeOptionSuffix);
	AddSetting("Swipe_OptionStrikePrice", m_swipeOptionStrikePrice);
	AddSetting("Swipe_OptionExpiration", m_swipeOptionExpiration);
#endif

	AddSetting("Borrow_AccountId", m_borrowAccountId);
	AddSetting("Borrow_Symbol", m_borrowSymbol);
#ifndef TAKION_NO_OPTIONS
	AddSetting("Borrow_Option", m_borrowOption);
	AddSetting("Borrow_OptionCall", m_borrowOptionCall);
	AddSetting("Borrow_OptionSuffix", m_borrowOptionSuffix);
	AddSetting("Borrow_OptionStrikePrice", m_borrowOptionStrikePrice);
	AddSetting("Borrow_OptionExpiration", m_borrowOptionExpiration);
#endif

	AddSetting("MD_AccountId", m_mdAccountId);
	AddSetting("MD_Symbol", m_mdSymbol);
#ifndef TAKION_NO_OPTIONS
	AddSetting("MD_Option", m_mdOption);
	AddSetting("MD_OptionCall", m_mdOptionCall);
	AddSetting("MD_OptionSuffix", m_mdOptionSuffix);
	AddSetting("MD_OptionStrikePrice", m_mdOptionStrikePrice);
	AddSetting("MD_OptionExpiration", m_mdOptionExpiration);
#endif

	AddSetting("Order_AccountId", m_orderAccountId);
	AddSetting("Order_Symbol", m_orderSymbol);
	AddSetting("Order_InThread", m_orderInThread);
#ifndef TAKION_NO_OPTIONS
	AddSetting("Order_Option", m_orderOption);
	AddSetting("Order_OptionCall", m_orderOptionCall);
//	AddSetting("Order_OptionSuffix", m_orderOptionSuffix);
	AddSetting("Order_OptionStrikePrice", m_orderOptionStrikePrice);
	AddSetting("Order_OptionExpiration", m_orderOptionExpiration);
#endif
	AddSetting("Order_Status", m_orderStatusMask);
	AddSetting("Order_Side", m_orderSideMask);
	AddSetting("Order_Instrument", m_orderInstrumentMask);
	AddSetting("OrderReplace_MarketOrder", m_orderReplaceMarketOrder);
	AddSetting("OrderReplace_PriceOffsetPercent", m_orderReplacePriceOffsetPercent);
	AddSetting("OrderReplace_PriceEnforceMinOffset", m_orderReplacePriceEnforceMinOffset);
	AddSetting("OrderReplace_SizeOffsetPercent", m_orderReplaceSizeOffsetPercent);
	AddSetting("OrderReplace_PriceOffset", m_orderReplacePriceOffset);
	AddSetting("OrderReplace_SizeOffset", m_orderReplaceSizeOffset);
	AddSetting("OrderReplace_Native", m_orderReplaceNative);
	AddSetting("OrderReplace_PreBorrow", m_orderReplacePreBorrow);
	AddSetting("OrderReplace_PreBorrowPrice", m_orderReplacePreBorrowPrice);
	AddSetting("Order_Future", m_orderFuture);

	AddSetting("Position_AccountId", m_positionAccountId);
	AddSetting("Position_InThread", m_positionInThread);
	AddSetting("Position_Status", m_positionStatusMask);
	AddSetting("Position_Instrument", m_positionInstrumentMask);

	AddSetting("HpSymbol", m_histPrintSymbol),
	AddSetting("HpOddLot", m_histPrintOddLot),
	AddSetting("HpFilter", m_histPrintFilter),

	AddSetting("AccountAction_AccountId", m_accountActionAccountId);
	AddSetting("AA_MsSymbol", m_accountActionMsSymbol);
	AddSetting("AA_MdSymbol", m_accountActionMdSymbol);
#ifndef TAKION_NO_OPTIONS
	AddSetting("AA_Option", m_accountActionOption);
	AddSetting("AA_OptionCall", m_accountActionOptionCall);
	AddSetting("AA_OptionSuffix", m_accountActionOptionSuffix);
	AddSetting("AA_OptionStrikePrice", m_accountActionOptionStrikePrice);
	AddSetting("AA_OptionExpiration", m_accountActionOptionExpiration);
#endif

	AddSetting("PositionAction_AccountId", m_positionActionAccountId);
	AddSetting("PositionAction_Symbol", m_positionActionSymbol);
#ifndef TAKION_NO_OPTIONS
	AddSetting("PositionAction_Option", m_positionActionOption);
	AddSetting("PositionAction_OptionCall", m_positionActionOptionCall);
	AddSetting("PositionAction_OptionSuffix", m_positionActionOptionSuffix);
	AddSetting("PositionAction_OptionStrikePrice", m_positionActionOptionStrikePrice);
	AddSetting("PositionAction_OptionExpiration", m_positionActionOptionExpiration);
#endif

	AddSetting("StrategyHiLoAccountId", m_strategyHiLoAccountId);
	AddSetting("StrategyHiLoStockFilePath", m_strategyHiLoStockFilePath);
	AddSetting("StrategyHiLoLoadOnStart", m_strategyHiLoLoadOnStart);
	AddSetting("StrategyHiLoLoadFromFile", m_strategyHiLoLoadFromFile);
	AddSetting("StrategyHiLoSymbolDelimiter", m_strategyHiLoSymbolDelimiter);
	AddSetting("StrategyHiLoTradeOnStart", m_strategyHiLoTradeOnStart);
	AddSetting("StrategyHiLoMillisecondEntry", m_strategyHiLoMillisecondEntry);
	AddSetting("StrategyHiLoMillisecondStop", m_strategyHiLoMillisecondStop);
	AddSetting("StrategyHiLoMillisecondExit", m_strategyHiLoMillisecondExit);
	AddSetting("StrategyHiLoMillisecondStopExit", m_strategyHiLoMillisecondStopExit);
	AddSetting("StrategyHiLoYVolume", m_strategyHiLoYesterdayVolumeLimit);
	AddSetting("StrategyHiLoMinPrice", m_strategyHiLoMinPrice);
	AddSetting("StrategyHiLoMaxPrice", m_strategyHiLoMaxPrice);
	AddSetting("StrategyHiLoOrderSize", m_strategyHiLoOrderSize);
	AddSetting("StrategyHiLoEnterVenue", m_strategyHiLoEnterVenue);
	AddSetting("StrategyHiLoEnterRouting", m_strategyHiLoEnterRouting);
	AddSetting("StrategyHiLoEnterMnemonic", m_strategyHiLoEnterMnemonic);
	AddSetting("StrategyHiLoEnterReserveSize", m_strategyHiLoEnterReserveSize);
	AddSetting("StrategyHiLoExitVenue", m_strategyHiLoExitVenue);
	AddSetting("StrategyHiLoExitRouting", m_strategyHiLoExitRouting);
	AddSetting("StrategyHiLoExitMnemonic", m_strategyHiLoExitMnemonic);
	AddSetting("StrategyHiLoExitReserveSize", m_strategyHiLoExitReserveSize);

	AddSetting("Win_Type", m_winType);
	AddSetting("EWL", m_extTools);
	AddSetting("ReqFilter", m_watchRequestFilter);
	AddSetting("RspFilter", m_watchResponseFilter);
	AddSetting("MessageCount", m_watchMessageCount);
	AddSetting("WatchSuspended", m_watchSuspended);

	AddSetting("SimQuoteSimbolType", m_simQuoteSymbolType);
	AddSetting("SimQuoteSimbol", m_simQuoteSymbol);
	AddSetting("SimQuoteSimbolMap", m_simQuoteSymbolMap);
#ifndef TAKION_NO_OPTIONS
	AddSetting("SimQuoteOption", m_simQuoteOption);
	AddSetting("SimQuoteOptionCall", m_simQuoteOptionCall);
	AddSetting("SimQuoteOptionSuffix", m_simQuoteOptionSuffix);
	AddSetting("SimQuoteOptionStrikePrice", m_simQuoteOptionStrikePrice);
	AddSetting("SimQuoteOptionExpiration", m_simQuoteOptionExpiration);
#endif
	AddSetting("SimQuoteRefNumber", m_simQuoteRefNumber);
	AddSetting("SimQuoteSide", m_simQuoteSide);
	AddSetting("SimQuoteBookName", m_simQuoteBookName);
	AddSetting("SimQuotePrice", m_simQuotePrice);
	AddSetting("SimQuoteSize", m_simQuoteSize);
	AddSetting("SimQuoteCondition", m_simQuoteCondition);
	AddSetting("SimQuoteRetailLiquidity", m_simQuoteRetailLiquidity);
	AddSetting("SimQuoteAttribution", m_simQuoteAttribution);
	AddSetting("SimQuoteUseRemainingSize", m_simQuoteUseRemainingSize);
	AddSetting("SimQuoteRemainingSize", m_simQuoteRemainingSize);
	AddSetting("SimQuotePrintable", m_simQuotePrintable);
	AddSetting("SimQuoteRefAutoIncrement", m_simQuoteRefAutoIncrement);
	AddSetting("SimQuoteMessageName", m_simQuoteMessageName);

	AddSetting("SimPrintSimbolType", m_simPrintSymbolType);
	AddSetting("SimPrintSimbol", m_simPrintSymbol);
	AddSetting("SimPrintSimbolMap", m_simPrintSymbolMap);
	AddSetting("SimPrintSource", m_simPrintSource);
#ifndef TAKION_NO_OPTIONS
	AddSetting("SimPrintOption", m_simPrintOption);
	AddSetting("SimPrintOptionCall", m_simPrintOptionCall);
	AddSetting("SimPrintOptionSuffix", m_simPrintOptionSuffix);
	AddSetting("SimPrintOptionStrikePrice", m_simPrintOptionStrikePrice);
	AddSetting("SimPrintOptionExpiration", m_simPrintOptionExpiration);
#endif
	AddSetting("SimPrintRefNumber", m_simPrintRefNumber);
	AddSetting("SimPrintPrice", m_simPrintPrice);
	AddSetting("SimPrintSize", m_simPrintSize);
	AddSetting("SimPrintCondition1", m_simPrintSaleCondition1);
	AddSetting("SimPrintCondition2", m_simPrintSaleCondition2);
	AddSetting("SimPrintCondition3", m_simPrintSaleCondition3);
	AddSetting("SimPrintCondition4", m_simPrintSaleCondition4);
	AddSetting("SimPrintMarketCenter", m_simPrintMarketCenter);
	AddSetting("SimPrintMarketListing", m_simPrintMarketListing);
	AddSetting("SimPrintRefAutoIncrement", m_simPrintRefAutoIncrement);

	AddSetting("SimPrintMessageType", m_simPrintMessageType);
	AddSetting("SimPrintLastPrice", m_simPrintLastPrice);
	AddSetting("SimPrintHighPrice", m_simPrintHighPrice);
	AddSetting("SimPrintLowPrice", m_simPrintLowPrice);
	AddSetting("SimPrintVolume", m_simPrintVolume);

	AddSetting("SimPrintRefNumberCorr", m_simPrintRefNumberCorr);
	AddSetting("SimPrintPriceCorr", m_simPrintPriceCorr);
	AddSetting("SimPrintSizeCorr", m_simPrintSizeCorr);
	AddSetting("SimPrintCondition1Corr", m_simPrintSaleCondition1Corr);
	AddSetting("SimPrintCondition2Corr", m_simPrintSaleCondition2Corr);
	AddSetting("SimPrintCondition3Corr", m_simPrintSaleCondition3Corr);
	AddSetting("SimPrintCondition4Corr", m_simPrintSaleCondition4Corr);
	AddSetting("SimPrintRefCorrAutoIncrement", m_simPrintRefCorrAutoIncrement);

	AddSetting("SimLevel1SimbolType", m_simLevel1SymbolType);
	AddSetting("SimLevel1Simbol", m_simLevel1Symbol);
	AddSetting("SimLevel1SimbolMap", m_simLevel1SymbolMap);
	AddSetting("SimLevel1Source", m_simLevel1Source);
#ifndef TAKION_NO_OPTIONS
	AddSetting("SimLevel1Option", m_simLevel1Option);
	AddSetting("SimLevel1OptionCall", m_simLevel1OptionCall);
	AddSetting("SimLevel1OptionSuffix", m_simLevel1OptionSuffix);
	AddSetting("SimLevel1OptionStrikePrice", m_simLevel1OptionStrikePrice);
	AddSetting("SimLevel1OptionExpiration", m_simLevel1OptionExpiration);
#endif
	AddSetting("SimLevel1Bid", m_simLevel1Bid);
	AddSetting("SimLevel1Ask", m_simLevel1Ask);
	AddSetting("SimLevel1BidSize", m_simLevel1BidSize);
	AddSetting("SimLevel1AskSize", m_simLevel1AskSize);
	AddSetting("SimLevel1Side", m_simLevel1Side);
	AddSetting("SimLevel1Condition", m_simLevel1SaleCondition);
	AddSetting("SimLevel1BidMarketListing", m_simLevel1BidMarketCenter);
	AddSetting("SimLevel1AskMarketListing", m_simLevel1AskMarketCenter);
	AddSetting("SimLevel1MarketListing", m_simLevel1MarketListing);

	AddSetting("SimLrpLuldSimbolType", m_simLrpLuldSymbolType);
	AddSetting("SimLrpLuldSimbol", m_simLrpLuldSymbol);
	AddSetting("SimLrpLuldSimbolMap", m_simLrpLuldSymbolMap);
	AddSetting("SimLrpLuldSource", m_simLrpLuldSource);
	AddSetting("SimLrpLuldMessageType", m_simLrpLuldMessageType);
	AddSetting("SimLrpLuldPriceLow", m_simLrpLuldPriceLow);
	AddSetting("SimLrpLuldPriceHigh", m_simLrpLuldPriceHigh);

	AddSetting("SimStockTypeSimbolType", m_simStockTypeSymbolType);
	AddSetting("SimStockTypeSimbol", m_simStockTypeSymbol);
	AddSetting("SimStockTypeSimbolMap", m_simStockTypeSymbolMap);
	AddSetting("SimStockTypeSource", m_simStockTypeSource);
	AddSetting("SimStockTypeSecurityType", m_simStockTypeSecurityType);

	AddSetting("SimTradingActionSimbolType", m_simTradingActionSymbolType);
	AddSetting("SimTradingActionSimbol", m_simTradingActionSymbol);
	AddSetting("SimTradingActionSimbolMap", m_simTradingActionSymbolMap);
	AddSetting("SimTradingActionSource", m_simTradingActionSource);
	AddSetting("SimTradingActionTradingState", m_simTradingActionTradingState);
	AddSetting("SimTradingActionReason", m_simTradingActionReason);

	AddSetting("SimClsPriceSymbolType", m_simClsPriceSymbolType);
	AddSetting("SimClsPriceSymbol", m_simClsPriceSymbol);
	AddSetting("SimClsPriceSymbolMap", m_simClsPriceSymbolMap);
	AddSetting("SimClsPriceSource", m_simClsPriceSource);
	AddSetting("SimClsPrice", m_simClsPrice);
#ifndef TAKION_NO_OPTIONS
	AddSetting("SimClsPriceOption", m_simClsPriceOption);
	AddSetting("SimClsPriceOptionCall", m_simClsPriceOptionCall);
	AddSetting("SimClsPriceOptionSuffix", m_simClsPriceOptionSuffix);
	AddSetting("SimClsPriceOptionStrikePrice", m_simClsPriceOptionStrikePrice);
	AddSetting("SimClsPriceOptionExpiration", m_simClsPriceOptionExpiration);
#endif
/*
	AddSetting("SimInvalidSymbolSymbolType", m_simInvalidSymbolSymbolType);
	AddSetting("SimInvalidSymbolSymbol", m_simInvalidSymbolSymbol);
//	AddSetting("SimInvalidSymbolSymbolMap", m_simInvalidSymbolSymbolMap);
	AddSetting("SimInvalidSymbolSource", m_simInvalidSymbolSource);
#ifndef TAKION_NO_OPTIONS
	AddSetting("SimInvalidSymbolOption", m_simInvalidSymbolOption);
	AddSetting("SimInvalidSymbolOptionCall", m_simInvalidSymbolOptionCall);
	AddSetting("SimInvalidSymbolOptionSuffix", m_simInvalidSymbolOptionSuffix);
	AddSetting("SimInvalidSymbolOptionStrikePrice", m_simInvalidSymbolOptionStrikePrice);
	AddSetting("SimInvalidSymbolOptionExpiration", m_simInvalidSymbolOptionExpiration);
#endif
*/
	AddSetting("SimResetSymbolType", m_simResetSymbolType);
	AddSetting("SimResetSymbol", m_simResetSymbol);
	AddSetting("SimResetSymbolMap", m_simResetSymbolMap);
	AddSetting("SimResetSource", m_simResetSource);
	AddSetting("SimResetMessageType", m_simResetMessageType);
	AddSetting("SimResetBook", m_simResetBook);
#ifndef TAKION_NO_OPTIONS
	AddSetting("SimResetOption", m_simResetOption);
	AddSetting("SimResetOptionCall", m_simResetOptionCall);
	AddSetting("SimResetOptionSuffix", m_simResetOptionSuffix);
	AddSetting("SimResetOptionStrikePrice", m_simResetOptionStrikePrice);
	AddSetting("SimResetOptionExpiration", m_simResetOptionExpiration);
#endif

	AddSetting("SimIndexSnapshotSymbol", m_simIndexSnapshotSymbol);
	AddSetting("SimIndexSnapshotFlags", m_simIndexSnapshotFlags);
	AddSetting("SimIndexSnapshotDescription", m_simIndexSnapshotDescription);

	AddSetting("SimSnapshotSymbolType", m_simSnapshotSymbolType);
	AddSetting("SimSnapshotSymbol", m_simSnapshotSymbol);
	AddSetting("SimSnapshotSymbolMap", m_simSnapshotSymbolMap);
	AddSetting("SimSnapshotSource", m_simSnapshotSource);
	AddSetting("SimSnapshotMarketStatus", m_simSnapshotMarketStatus);
	AddSetting("SimSnapshotPrimaryExchange", m_simSnapshotPrimaryExchange);
	AddSetting("SimSnapshotFlags", m_simSnapshotFlags);
	AddSetting("SimSnapshotDescription", m_simSnapshotDescription);
	AddSetting("SimSnapshotRoundLot", m_simSnapshotRoundLot);
	AddSetting("SimSnapshotRoundLotsOnly", m_simSnapshotRoundLotsOnly);
	AddSetting("SimSnapshotMarketCategory", m_simSnapshotMarketCategory);
	AddSetting("SimSnapshotFinancialStatus", m_simSnapshotFinancialStatus);
	AddSetting("SimSnapshotTradingState", m_simSnapshotTradingState);
	AddSetting("SimSnapshotTradingActionReason", m_simSnapshotTradingActionReason);
#ifndef TAKION_NO_OPTIONS
	AddSetting("SimSnapshotOption", m_simSnapshotOption);
	AddSetting("SimSnapshotOptionCall", m_simSnapshotOptionCall);
	AddSetting("SimSnapshotOptionSuffix", m_simSnapshotOptionSuffix);
	AddSetting("SimSnapshotOptionStrikePrice", m_simSnapshotOptionStrikePrice);
	AddSetting("SimSnapshotOptionExpiration", m_simSnapshotOptionExpiration);
#endif

	AddSetting("SimAcctRiskAccountId", m_simAcctRiskAccountId);

	AddSetting("SimBustExecAccountId", m_simBustExecAccountId);
	AddSetting("SimBustExecRefNumber", m_simBustExecRefNumber);
	AddSetting("SimBustExecRef", m_simBustExecRef);
	AddSetting("SimBustExecBusted", m_simBustExecBusted);
	AddSetting("SimBustExecDropcopy", m_simBustExecDropcopy);
	AddSetting("SimBustExecSymbol", m_simBustExecSymbol);
#ifndef TAKION_NO_OPTIONS
	AddSetting("SimBustExecOption", m_simBustExecOption);
	AddSetting("SimBustExecOptionCall", m_simBustExecOptionCall);
	AddSetting("SimBustExecOptionSuffix", m_simBustExecOptionSuffix);
	AddSetting("SimBustExecOptionStrikePrice", m_simBustExecOptionStrikePrice);
	AddSetting("SimBustExecOptionExpiration", m_simBustExecOptionExpiration);
#endif
	AddSetting("SimBustExecSize", m_simBustExecSize);
	AddSetting("SimBustExecPrice", m_simBustExecPrice);
	AddSetting("SimBustExecSide", m_simBustExecSide);
	AddSetting("SimBustExecOrderSize", m_simBustExecOrderSize);
	AddSetting("SimBustExecMmid", m_simBustExecMmid);
	AddSetting("SimBustExecVenue", m_simBustExecVenue);
	AddSetting("SimBustExecLiquidity", m_simBustExecLiquidity);
//	AddSetting("SimBustExecOrderPrice", m_simBustExecOrderPrice);
	AddSetting("SimBustExecSendMessageToAllAccountsDelay", m_simBustExecSendMessageToAllAccountsDelay);

	AddSetting("SimOrderUpdateAccountId", m_simOrderUpdateAccountId);
	AddSetting("SimOrderUpdateSymbol", m_simOrderUpdateSymbol);
#ifndef TAKION_NO_OPTIONS
	AddSetting("SimOrderUpdateOption", m_simOrderUpdateOption);
	AddSetting("SimOrderUpdateOptionCall", m_simOrderUpdateOptionCall);
	AddSetting("SimOrderUpdateOptionSuffix", m_simOrderUpdateOptionSuffix);
	AddSetting("SimOrderUpdateOptionStrikePrice", m_simOrderUpdateOptionStrikePrice);
	AddSetting("SimOrderUpdateOptionExpiration", m_simOrderUpdateOptionExpiration);
#endif
	AddSetting("SimOrderUpdateSize", m_simOrderUpdateSize),
	AddSetting("SimOrderUpdateRemaining", m_simOrderUpdateRemaining);
	AddSetting("SimOrderUpdateClientId", m_simOrderUpdateClientId);
	AddSetting("SimOrderUpdateServerId", m_simOrderUpdateServerId);
	AddSetting("SimOrderUpdateReasonCode", m_simOrderUpdateReasonCode);
	AddSetting("SimOrderUpdatePrice", m_simOrderUpdatePrice);
	AddSetting("SimOrderUpdateBid", m_simOrderUpdateBid);
	AddSetting("SimOrderUpdateAsk", m_simOrderUpdateAsk);
	AddSetting("SimOrderUpdateLast", m_simOrderUpdateLast);
	AddSetting("SimOrderUpdateHistorical", m_simOrderUpdateHistorical);
//	AddSetting("SimOrderUpdateReroutedToBaml", m_simOrderUpdateReroutedToBaml);
	AddSetting("SimOrderUpdateReroutedToBaml", m_simOrderUpdateReRoute);
	AddSetting("SimOrderUpdateMarket", m_simOrderUpdateMarket);
	AddSetting("SimOrderUpdateVenue", m_simOrderUpdateVenue);
	AddSetting("SimOrderUpdateRouting", m_simOrderUpdateRouting);
	AddSetting("SimOrderUpdateMmid", m_simOrderUpdateMmid);
	AddSetting("SimOrderMarketReferenceId", m_simOrderUpdateMarketReferenceId);
	AddSetting("SimOrderUpdateSide", m_simOrderUpdateSide);
	AddSetting("SimOrderUpdateReserveSize", m_simOrderUpdateReserveSize),
	AddSetting("SimOrderUpdateBorrowAccept", m_simOrderUpdateMessageType);// m_simOrderUpdateBorrowAccept);
	AddSetting("SimOrderUpdateExecutionInstructions", m_simOrderUpdateExecutionInstructions);
	AddSetting("SimOrderUpdateExecutionLiquidity1", m_simOrderUpdateLiquidity1);
	AddSetting("SimOrderUpdateExecutionLiquidity2", m_simOrderUpdateLiquidity2);

	AddSetting("SimOrderUpdateExecutionFilled", m_simOrderUpdateExecutionFilled);
	AddSetting("SimOrderUpdateExecutionCancelled", m_simOrderUpdateExecutionCancelled);
	AddSetting("SimOrderUpdateExecutionRemaining", m_simOrderUpdateExecutionRemaining);
	AddSetting("SimOrderUpdateExecutionPrice", m_simOrderUpdateExecutionPrice);
	AddSetting("SimOrderUpdateExecutionVenue", m_simOrderUpdateExecutionVenue);
	AddSetting("SimOrderUpdateExecutionMmid", m_simOrderUpdateExecutionMmid);
	AddSetting("SimOrderExecutionReferenceId", m_simOrderUpdateExecutionReferenceId);

	AddSetting("SimOrderUpdateExecutionReportFlags", m_simOrderUpdateExecutionReportFlags);

#ifndef TAKION_NO_OPTIONS
	AddSetting("SimPosLoadOption", m_simPosLoadOption);
	AddSetting("SimPosLoadOptionCall", m_simPosLoadOptionCall);
	AddSetting("SimPosLoadOptionSuffix", m_simPosLoadOptionSuffix);
	AddSetting("SimPosLoadOptionStrikePrice", m_simPosLoadOptionStrikePrice);
	AddSetting("SimPosLoadOptionExpiration", m_simPosLoadOptionExpiration);
#endif
	AddSetting("SimPosLoadAccountId", m_simPosLoadAccountId);
	AddSetting("SimPosLoadSymbol", m_simPosLoadSymbol);
	AddSetting("SimPosLoadSize", m_simPosLoadSize);
	AddSetting("SimPosLoadPrice", m_simPosLoadPrice);
	AddSetting("SimPosLoadCost", m_simPosLoadCost);
	AddSetting("SimPosLoadOvnSize", m_simPosLoadOvernightSize);
	AddSetting("SimPosLoadOvnPrice", m_simPosLoadOvernightPrice);
	AddSetting("SimPosLoadOpenOvnSize", m_simPosLoadOpenOvernightSize);
	AddSetting("SimPosLoadClsLongSize", m_simPosLoadClsLongSize);
	AddSetting("SimPosLoadClsLongCost", m_simPosLoadClsLongCost);
	AddSetting("SimPosLoadClsLongValue", m_simPosLoadClsLongValue);
	AddSetting("SimPosLoadClsShortSize", m_simPosLoadClsShortSize);
	AddSetting("SimPosLoadClsShortCost", m_simPosLoadClsShortCost);
	AddSetting("SimPosLoadOvnClsShortValue", m_simPosLoadClsShortValue);
	AddSetting("SimPosLoadBorrowSize", m_simPosLoadBorrowSize);
	AddSetting("SimPosLoadBorrowPrice", m_simPosLoadBorrowPrice);
	AddSetting("SimPosLoadSendMessageToAllAccountsDelay", m_simPosLoadSendMessageToAllAccountsDelay);

	AddSetting("SimSlAccountId", m_simSlAccountId);
	AddSetting("SimSlSymbol", m_simSlSymbol);
	AddSetting("SimSlRequestId", m_simSlRequestId);
	AddSetting("SimSlFirm", m_simSlFirm);
	AddSetting("SimSlBorrowPrice", m_simSlBorrowPrice);
	AddSetting("SimSlBeta", m_simSlBeta);
	AddSetting("SimSlBorrowSize", m_simSlBorrowSize);
	AddSetting("SimSlDiscountFlag", m_simSlDiscountFlag);
	AddSetting("SimSlOnTheFly", m_simSlOnTheFly);
	AddSetting("SimSlOnTheFlyBorrow", m_simSlOnTheFlyBorrow);
	AddSetting("SimSlMarginable", m_simSlMarginable);
	AddSetting("SimSlHtb", m_simSlHtb);

	AddSetting("SimSlAssetType", m_simSlAssetType);
	AddSetting("SimSlSecType", m_simSlSecType);
	AddSetting("SimSlStatus", m_simSlStatus);

	AddSetting("SimSlMargin", m_simSlMargin);

	AddSetting("SimSlPmElRate", m_simSlPmElRate);
	AddSetting("SimSlPmEsRate", m_simSlPmEsRate);
	AddSetting("SimSlPmHlRate", m_simSlPmHlRate);
	AddSetting("SimSlPmHsRate", m_simSlPmHsRate);
	AddSetting("SimSlElRate", m_simSlElRate);
	AddSetting("SimSlEsRate", m_simSlEsRate);
	AddSetting("SimSlHlRate", m_simSlHlRate);
	AddSetting("SimSlHsRate", m_simSlHsRate);
	AddSetting("SimSlFedReq", m_simSlFedReq);
//
	AddSetting("SimEtbListAccountId", m_simEtbListAccountId);
	AddSetting("SimEtbListSymbol", m_simEtbListSymbol);
	AddSetting("SimEtbListRequestId", m_simEtbListRequestId);
	AddSetting("SimEtbListFirm", m_simEtbListFirm);
	AddSetting("SimEtbListFlags", m_simEtbListFlags);
	AddSetting("SimEtbListDone", m_simEtbListDone);
//
	AddSetting("SimImbalanceSymbolType", m_simImbalanceSymbolType);
	AddSetting("SimImbalanceSymbol", m_simImbalanceSymbol);
	AddSetting("SimImbalanceSymbolMap", m_simImbalanceSymbolMap);
	AddSetting("SimImbalanceSource", m_simImbalanceSource);
	AddSetting("SimImbalanceBookId", m_simImbalanceBookId);
	AddSetting("SimImbalanceType", m_simImbalanceType);
	AddSetting("SimImbalanceDirection", m_simImbalanceDirection);
	AddSetting("SimImbalanceRegulatory", m_simImbalanceRegulatory);
	AddSetting("SimImbalanceFreezeStatus", m_simImbalanceFreezeStatus);
	AddSetting("SimImbalance", m_simImbalance);
	AddSetting("SimImbalanceMarket", m_simMarketImbalance);
	AddSetting("SimImbalancePairedShares", m_simPairedShares);
	AddSetting("SimImbalancePriceFar", m_simImbalancePriceFar);
	AddSetting("SimImbalancePriceNear", m_simImbalancePriceNear);
	AddSetting("SimImbalancePriceRef", m_simImbalancePriceRef);
	AddSetting("SimImbalancePv", m_simImbalancePv);
	AddSetting("SimImbalanceAuctionTime", m_simImbalanceAuctionTimeSeconds);

	AddSetting("SimChartCorrectionSymbolType", m_simChartCorrectionSymbolType);
	AddSetting("SimChartCorrectionSymbol", m_simChartCorrectionSymbol);
	AddSetting("SimChartCorrectionSymbolMap", m_simChartCorrectionSymbolMap);
	AddSetting("SimChartCorrectionPriceStart", m_simChartCorrectionPriceStart);
	AddSetting("SimChartCorrectionPriceHigh", m_simChartCorrectionPriceHigh);
	AddSetting("SimChartCorrectionPriceLow", m_simChartCorrectionPriceLow);
	AddSetting("SimChartCorrectionPriceEnd", m_simChartCorrectionPriceEnd);
	AddSetting("SimChartCorrectionMoneyTraded", m_simChartCorrectionMoneyTraded);
	AddSetting("SimChartCorrectionVolume", m_simChartCorrectionVolume);
	AddSetting("SimChartCorrectionMinute", m_simChartCorrectionMinute);

	AddSetting("SimChartSnapshotSymbolType", m_simChartSnapshotSymbolType);
	AddSetting("SimChartSnapshotSymbol", m_simChartSnapshotSymbol);
	AddSetting("SimChartSnapshotSymbolMap", m_simChartSnapshotSymbolMap);
	AddSetting("SimChartSnapshotRequestId", m_simChartSnapshotRequestId);
	AddSetting("SimChartSnapshotPointCount", m_simChartSnapshotPointCount);
	AddSetting("SimChartSnapshotStartMinute", m_simChartSnapshotStartMinute);
	AddSetting("SimChartSnapshotPriceStart", m_simChartSnapshotPriceStart);
	AddSetting("SimChartSnapshotPriceHigh", m_simChartSnapshotPriceHigh);
	AddSetting("SimChartSnapshotPriceLow", m_simChartSnapshotPriceLow);
	AddSetting("SimChartSnapshotPriceEnd", m_simChartSnapshotPriceEnd);
	AddSetting("SimChartSnapshotMoneyTraded", m_simChartSnapshotMoneyTraded);
	AddSetting("SimChartSnapshotVolume", m_simChartSnapshotVolume);
	AddSetting("SimChartSnapshotDone", m_simChartSnapshotDone);
//	AddSetting("SimChartSnapshotFlags", m_simChartSnapshotFlags);

	AddSetting("SimFirmPosAccountId", m_simFirmPositionAccountId);
	AddSetting("SimFirmPosSymbol", m_simFirmPositionSymbol);
	AddSetting("SimFirmPosSize", m_simFirmPositionSize);
	AddSetting("SimFirmPosPassive", m_simFirmPositionPendingSellPassive);
	AddSetting("SimFirmPosNonPassive", m_simFirmPositionPendingSellNonPassive);
	AddSetting("SimFirmPosShortPassive", m_simFirmPositionPendingShortPassive);
	AddSetting("SimFirmPosShortNonPassive", m_simFirmPositionPendingShortNonPassive);

	AddSetting("SimUsAccountId", m_simUsAccountId);
	AddSetting("SimUsSymbol", m_simUsSymbol);
	AddSetting("SimUsFirm", m_simUsFirm);
	AddSetting("SimUsTraderId", m_simUsTraderId);
	AddSetting("SimUsStatus", m_simUsStatus);//Untradeable Status. 0 - tradeable; 1 - untradeable full; 2 - untradeable partial

	AddSetting("SimEnforceAggregationClearingFirmId", m_simEnforceAggregationClearingFirmId);
	AddSetting("SimEnforceAggregationEnforce", m_simEnforceAggregationEnforce);
	AddSetting("SimEnforceAggregationConservativeOrderMarking", m_simEnforceAggregationConservativeOrderMarking);
	AddSetting("SimEnforceAggregationLocalSideMarking", m_simEnforceAggregationLocalSideMarking);
	AddSetting("SimEnforceAggregationEnforceNiteBetaTime", m_simEnforceAggregationEnforceNiteBetaTimeSeconds);
	AddSetting("SimEnforceAggregationNiteBetaBp", m_simEnforceAggregationNiteBetaBp);

	AddSetting("SimMarketStatus", m_simMarketStatus);
	AddSetting("SimMarketStatusSymbolMapping", m_simMarketStatusSymbolMapping);

	AddSetting("SimAdminBroadcastText", m_simAdminBroadcastText);

	AddSetting("TcSymbol", m_tcSymbol);
	AddSetting("TcDecDigit", m_tcDecDigit);

	AddSetting("HcAllChartFileName", m_hcAllChartFileName);
	AddSetting("HcSymbol", m_hcSymbol);
	AddSetting("HcDecDigit", m_hcDecDigit);
	AddSetting("HcDayCountMode", m_hcDayCountMode);
	AddSetting("HcDayCount", m_hcDayCount);
	AddSetting("HcDateFrom", m_hcDateFrom);
	AddSetting("HcFrequency", m_hcFrequency);

//	AddSetting("MI_LoadOnStart", m_miLoadOnStart);
	AddSetting("MI_InThread", m_miInThread);
	AddSetting("MI_SendToMm", m_miSendToMm);
	AddSetting("MI_MarketCategoryFilter", m_miMarketCategoryFilter);
	AddSetting("MI_TestFilter", m_miTestStockFilter);
	AddSetting("MI_CharFrom", m_miCharFrom);
	AddSetting("MI_CharTo", m_miCharTo);
	AddSetting("MI_YestVolumeFrom", m_miYestVolumeFrom);
	AddSetting("MI_YestVolumeTo", m_miYestVolumeTo);

	AddSetting("MdLogonMdVersion", m_simMdLogonMdVersion);
	AddSetting("MdLogonCompression", m_simMdLogonCompression);
	AddSetting("MdLogonInstrument", m_simMdLogonInstrument);
	AddSetting("MdLogonResult", m_simMdLogonResult);
	AddSetting("MdLogonSubscriptionOnly", m_simMdLogonSubscriptionOnly);
	AddSetting("MdLogonForceSubscriptionOnly", m_simMdLogonForceSubscriptionOnly);
	AddSetting("MdLogonDisableExtensions", m_simMdLogonDisableExtensions);
	AddSetting("MdLogonEntitlements", m_simMdLogonEntitlements);
	AddSetting("MdLogonExtEntitlements", m_simMdLogonExtEntitlements);

//	AddSetting("NewsTopicOrdinal", m_topicOrdinal);
	AddSetting("NewsTopic", m_topic);
	AddSetting("NewsDaysBack", m_newsDaysBack);
	AddSetting("NewsHeadlineCount", m_newsHeadlineCount);
	AddSetting("NewsAfterMarket", m_newsAfterMarket);
	AddSetting("NewsSearchType", m_searchType);
	AddSetting("NewsAllEntitled", m_allEntitled);
	AddSetting("NewsWireSet", m_wireSettingSet);
	AddSetting("NewsStoryFilter", m_storyFilter);
	AddSetting("NewsMetaFilter", m_metaFilter);
	AddSetting("NewsRelevance", m_relevance);
	AddSetting("NewsShowStoryChain", m_showStoryChain);

	AddSetting("CancelOrder_AccountId", m_cancelOrderAccountId);
	AddSetting("CancelOrder_Symbol", m_cancelOrderSymbol);
#ifndef TAKION_NO_OPTIONS
	AddSetting("CancelOrder_Option", m_cancelOrderOption);
	AddSetting("CancelOrder_OptionCall", m_cancelOrderOptionCall);
	AddSetting("CancelOrder_OptionSuffix", m_cancelOrderOptionSuffix);
	AddSetting("CancelOrder_OptionStrikePrice", m_cancelOrderOptionStrikePrice);
	AddSetting("CancelOrder_OptionExpiration", m_cancelOrderOptionExpiration);
#endif
	AddSetting("CancelOrderClientId", m_cancelOrderClientId);
	AddSetting("CancelOrderUseClientId", m_cancelOrderUseClientId);
	AddSetting("CancelOrderAccountAll", m_cancelOrderAccountAll);
	AddSetting("CancelOrderPosition", m_cancelOrderPosition);
#ifndef TAKION_NO_OPTIONS
	AddSetting("CancelOrderInstrument", m_cancelOrderInstrument);
#endif
	AddSetting("CancelOrderSide", m_cancelOrderSide);
	AddSetting("CancelOrderDirection", m_cancelOrderDirection);
	AddSetting("CancelOrderPick", m_cancelOrderPick);
	AddSetting("CancelOrderTif", m_cancelOrderTif);
	AddSetting("CancelOrderType", m_cancelOrderType);
	AddSetting("CancelOrderPqOpen", m_cancelOrderPqOpen);
	AddSetting("CancelOrderTClose", m_cancelOrderTClose);
	AddSetting("CancelOrderExcludeDestinations", m_cancelOrderExcludeDestinations);
	AddSetting("CancelOrderDestinations", m_cancelOrderDestinations);
	AddSetting("CancelOrderExcludeMnemonics", m_cancelOrderExcludeMnemonics);
	AddSetting("CancelOrderMnemonics", m_cancelOrderMnemonics);
	AddSetting("CancelOrderReplace", m_cancelOrderReplace);
	AddSetting("CancelOrderReplaceNative", m_cancelOrderReplaceNative);
	AddSetting("CancelOrderReplaceMarket", m_cancelOrderReplaceMarket);
	AddSetting("CancelOrderReplacePriceOffsetPercent", m_cancelOrderReplacePriceOffsetPercent);
	AddSetting("CancelOrderReplacePriceEnforceMinOffset", m_cancelOrderReplacePriceEnforceMinOffset);
	AddSetting("CancelOrderReplacePriceOffsetReverseForSell", m_cancelOrderReplacePriceOffsetReverseForSell);
	AddSetting("CancelOrderReplaceSizeOffsetPercent", m_cancelOrderReplaceSizeOffsetPercent);
	AddSetting("CancelOrderReplaceSizeOffsetReverseForSell", m_cancelOrderReplaceSizeOffsetReverseForSell);
	AddSetting("CancelOrderReplaceAdjustable", m_cancelOrderReplaceAdjustable);
	AddSetting("CancelOrderReplaceAdjustableUseMmBox", m_cancelOrderReplaceAdjustableUseMmBox);
	AddSetting("CancelOrderReplacePreBorrow", m_cancelOrderReplacePreBorrow);
	AddSetting("CancelOrderReplaceDisplaySizeMode", m_cancelOrderDisplaySizeMode);
	AddSetting("CancelOrderReplacePriceOffset", m_cancelOrdeReplacePriceOffset);
	AddSetting("CancelOrderReplaceSizeOffset", m_cancelOrdeReplaceSizeOffset);
	AddSetting("CancelOrderReplaceBorrowPrice", m_cancelOrdeReplaceBorrowPrice);
	AddSetting("CancelOrderReplaceDisplaySizeFraction", m_cancelOrdeReplaceDisplaySizeFraction);
	AddSetting("CancelOrderReplaceDisplaySizeRoundLot", m_cancelOrdeReplaceDisplaySizeRoundLot);

	AddSetting("CommandPost", m_commandPost);
	AddSetting("CommandName", m_commandName);
	AddSetting("CommandSymbol", m_commandSymbol);
}

ConfigBase* ExtensionDerivedConfig::Clone() const
{
	ExtensionDerivedConfig* clone = new ExtensionDerivedConfig;
	clone->Copy(*this);
	return clone;
}

void ExtensionDerivedConfig::Copy(const ConfigBase& other)
{
//	TakionConfig::Copy(other);
	operator=((const ExtensionDerivedConfig&)other);
}

const ExtensionDerivedConfig& ExtensionDerivedConfig::operator=(const ExtensionDerivedConfig& other)
{
	ExtensionConfig::operator=(other);
//	m_orderReplaceTemplateVersion = other.m_orderReplaceTemplateVersion;
	return *this;
}

void ExtensionDerivedConfig::Update()
{
	ExtensionConfig::Update();
	if(mainDialog)
	{
		MainDialog::UIntStrMap windowsMap;
		if(mainDialog->GetWindowsLayout(windowsMap, 0))
		{
			m_extTools.Clear();
			UIntSetting key;
			StringSettingRest stringSetting;
			for(MainDialog::UIntStrMap::const_iterator it = windowsMap.begin(), itend = windowsMap.end(); it != itend; ++it)
			{
				key.SetValue(it->first);
				stringSetting.SetValue(it->second);
				m_extTools.AddValue(key, stringSetting);
			}
		}
	}
}
