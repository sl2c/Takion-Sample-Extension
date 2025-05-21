#pragma once

#include "ConfigEnums.h"

class ExtensionDerivedConfig : public ExtensionConfig
{
public:
	ExtensionDerivedConfig();
	ExtensionDerivedConfig(const ExtensionDerivedConfig& other);
//	virtual const char* GetIniFileName() const override;
//	virtual const char* GetLogFileName() const override;
//	virtual const char* GetPosFileName() const override;
	virtual void Copy(const ConfigBase& other) override;
	virtual ConfigBase* Clone() const override;

	virtual void Update() override;

	const ExtensionDerivedConfig& operator=(const ExtensionDerivedConfig& other);

	BoolSetting m_msInThread;
	BoolSetting m_msPositionInThread;
	BoolSetting m_msSendToMm;
	BoolSetting m_msAddAll;
	BoolSetting m_msUpdateOnTimer;
	UIntSetting m_msUpdateOnTimerMillisecond;
	UIntSetting m_msSecondBack;
	BoolSetting m_msLoadOnStart;
	StringSettingRest m_msStockFilePath;
	typedef MapSetting<USecondSetting, UIntSetting, std::map<USecondSetting, UIntSetting> > TimerMap;
	TimerMap m_msTimerMap;

	BoolSetting m_mdCalculateL2;
	BoolSetting m_mdLoadOnStart;
	StringSettingRest m_mdStockFilePath;

	StringSetting m_venueAccountId;
	StringSetting m_venueSymbol;
	StringAsUnsignedNumberSetting<unsigned int> m_venueVenue;
	StringAsUnsignedNumberSetting<unsigned int> m_venueRouting;
	StringAsUnsignedNumberSetting<unsigned int> m_venueMnemonic;
	UIntSetting m_venueReserveSize;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_venueOption;
	BoolSetting m_venueOptionCall;
	CharSetting m_venueOptionSuffix;
	PriceSetting m_venueOptionStrikePrice;
	UIntSetting m_venueOptionExpiration;
#endif

	StringSetting m_swipeAccountId;
	StringSetting m_swipeSymbol;
	StringAsUnsignedNumberSetting<unsigned int> m_swipeVenue;
	StringAsUnsignedNumberSetting<unsigned int> m_swipeRouting;
	StringAsUnsignedNumberSetting<unsigned int> m_swipeMnemonic;
	UIntSetting m_swipeReserveSize;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_swipeOption;
	BoolSetting m_swipeOptionCall;
	CharSetting m_swipeOptionSuffix;
	PriceSetting m_swipeOptionStrikePrice;
	UIntSetting m_swipeOptionExpiration;
#endif

	StringSetting m_borrowAccountId;
	StringSetting m_borrowSymbol;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_borrowOption;
	BoolSetting m_borrowOptionCall;
	CharSetting m_borrowOptionSuffix;
	PriceSetting m_borrowOptionStrikePrice;
	UIntSetting m_borrowOptionExpiration;
#endif

	StringSetting m_mdAccountId;
	StringSetting m_mdSymbol;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_mdOption;
	BoolSetting m_mdOptionCall;
	CharSetting m_mdOptionSuffix;
	PriceSetting m_mdOptionStrikePrice;
	UIntSetting m_mdOptionExpiration;
#endif

	StringSetting m_orderAccountId;
	StringSetting m_orderSymbol;
	BoolSetting m_orderInThread;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_orderOption;
	BoolSetting m_orderOptionCall;
//	CharSetting m_orderOptionSuffix;
	PriceSetting m_orderOptionStrikePrice;
	UIntSetting m_orderOptionExpiration;
#endif
	UCharSetting m_orderStatusMask;
	UCharSetting m_orderSideMask;
	UCharSetting m_orderInstrumentMask;
	BoolSetting m_orderReplaceMarketOrder;
	BoolSetting m_orderReplacePriceOffsetPercent;
	BoolSetting m_orderReplacePriceEnforceMinOffset;
	BoolSetting m_orderReplaceSizeOffsetPercent;
	SignedPriceSetting m_orderReplacePriceOffset;
	IntSetting m_orderReplaceSizeOffset;
	BoolSetting m_orderReplaceNative;
	BoolSetting m_orderReplacePreBorrow;
	PriceSetting m_orderReplacePreBorrowPrice;
	BoolSetting m_orderFuture;

	StringSetting m_positionAccountId;
	BoolSetting m_positionInThread;
	UCharSetting m_positionStatusMask;
	UCharSetting m_positionInstrumentMask;

	StringSetting m_histPrintSymbol;
	BoolSetting m_histPrintOddLot;
	UIntSetting m_histPrintFilter;

	StringSetting m_accountActionAccountId;
	StringSetting m_accountActionMsSymbol;
	StringSetting m_accountActionMdSymbol;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_accountActionOption;
	BoolSetting m_accountActionOptionCall;
	CharSetting m_accountActionOptionSuffix;
	PriceSetting m_accountActionOptionStrikePrice;
	UIntSetting m_accountActionOptionExpiration;
#endif

	StringSetting m_positionActionAccountId;
	StringSetting m_positionActionSymbol;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_positionActionOption;
	BoolSetting m_positionActionOptionCall;
	CharSetting m_positionActionOptionSuffix;
	PriceSetting m_positionActionOptionStrikePrice;
	UIntSetting m_positionActionOptionExpiration;
#endif

//HiLoStrategy Settings
	StringSetting m_strategyHiLoAccountId;
	StringSetting m_strategyHiLoStockFilePath;
	BoolSetting m_strategyHiLoLoadOnStart;
	BoolSetting m_strategyHiLoLoadFromFile;
	UCharSetting m_strategyHiLoSymbolDelimiter;
	BoolSetting m_strategyHiLoTradeOnStart;
	UIntSetting m_strategyHiLoMillisecondEntry;
	UIntSetting m_strategyHiLoMillisecondStop;
	UIntSetting m_strategyHiLoMillisecondExit;
	UIntSetting m_strategyHiLoMillisecondStopExit;
	UInt64Setting m_strategyHiLoYesterdayVolumeLimit;
	PriceSetting m_strategyHiLoMinPrice;
	PriceSetting m_strategyHiLoMaxPrice;
	UIntSetting m_strategyHiLoOrderSize;
	StringAsUnsignedNumberSetting<unsigned int> m_strategyHiLoEnterVenue;
	StringAsUnsignedNumberSetting<unsigned int> m_strategyHiLoEnterRouting;
	StringAsUnsignedNumberSetting<unsigned int> m_strategyHiLoEnterMnemonic;
	UIntSetting m_strategyHiLoEnterReserveSize;
	StringAsUnsignedNumberSetting<unsigned int> m_strategyHiLoExitVenue;
	StringAsUnsignedNumberSetting<unsigned int> m_strategyHiLoExitRouting;
	StringAsUnsignedNumberSetting<unsigned int> m_strategyHiLoExitMnemonic;
	UIntSetting m_strategyHiLoExitReserveSize;

	UCharSetting m_winType;
	typedef MapSetting<UIntSetting, StringSettingRest, std::map<UIntSetting, StringSettingRest> > ExtToolMap;
	ExtToolMap m_extTools;

	UIntSetting m_watchRequestFilter;
	UIntSetting m_watchResponseFilter;
	UIntSetting m_watchMessageCount;
	BoolSetting m_watchSuspended;

	UCharSetting m_simQuoteSymbolType;
	StringSetting m_simQuoteSymbol;
	UShortSetting m_simQuoteSymbolMap;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simQuoteOption;
	BoolSetting m_simQuoteOptionCall;
	CharSetting m_simQuoteOptionSuffix;
	PriceSetting m_simQuoteOptionStrikePrice;
	UIntSetting m_simQuoteOptionExpiration;
#endif
	UInt64HexSetting m_simQuoteRefNumber;
	UCharSetting m_simQuoteSide;
	StringSetting m_simQuoteBookName;
	PriceSetting m_simQuotePrice;
	UIntSetting m_simQuoteSize;
	CharSetting m_simQuoteCondition;
	BoolSetting m_simQuoteRetailLiquidity;
	StringAsUIntSetting m_simQuoteAttribution;
	BoolSetting m_simQuoteUseRemainingSize;
	UIntSetting m_simQuoteRemainingSize;
	BoolSetting m_simQuotePrintable;
	BoolSetting m_simQuoteRefAutoIncrement;
	StringSetting m_simQuoteMessageName;

	UCharSetting m_simPrintSymbolType;
	StringSetting m_simPrintSymbol;
	UShortSetting m_simPrintSymbolMap;
	UCharSetting m_simPrintSource;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simPrintOption;
	BoolSetting m_simPrintOptionCall;
	CharSetting m_simPrintOptionSuffix;
	PriceSetting m_simPrintOptionStrikePrice;
	UIntSetting m_simPrintOptionExpiration;
#endif
	UInt64HexSetting m_simPrintRefNumber;
	PriceSetting m_simPrintPrice;
	UIntSetting m_simPrintSize;
	CharSetting m_simPrintSaleCondition1;
	CharSetting m_simPrintSaleCondition2;
	CharSetting m_simPrintSaleCondition3;
	CharSetting m_simPrintSaleCondition4;
	CharSetting m_simPrintMarketCenter;
	CharSetting m_simPrintMarketListing;

	UCharSetting m_simPrintMessageType;
	PriceSetting m_simPrintLastPrice;
	PriceSetting m_simPrintHighPrice;
	PriceSetting m_simPrintLowPrice;
	UInt64Setting m_simPrintVolume;

	UInt64HexSetting m_simPrintRefNumberCorr;
	PriceSetting m_simPrintPriceCorr;
	UIntSetting m_simPrintSizeCorr;
	CharSetting m_simPrintSaleCondition1Corr;
	CharSetting m_simPrintSaleCondition2Corr;
	CharSetting m_simPrintSaleCondition3Corr;
	CharSetting m_simPrintSaleCondition4Corr;
	BoolSetting m_simPrintRefAutoIncrement;
	BoolSetting m_simPrintRefCorrAutoIncrement;

	UCharSetting m_simLevel1SymbolType;
	StringSetting m_simLevel1Symbol;
	UShortSetting m_simLevel1SymbolMap;
	UCharSetting m_simLevel1Source;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simLevel1Option;
	BoolSetting m_simLevel1OptionCall;
	CharSetting m_simLevel1OptionSuffix;
	PriceSetting m_simLevel1OptionStrikePrice;
	UIntSetting m_simLevel1OptionExpiration;
#endif
	PriceSetting m_simLevel1Bid;
	PriceSetting m_simLevel1Ask;
	UIntSetting m_simLevel1BidSize;
	UIntSetting m_simLevel1AskSize;
	BoolSetting m_simLevel1Side;
	CharSetting m_simLevel1SaleCondition;
	CharSetting m_simLevel1BidMarketCenter;
	CharSetting m_simLevel1AskMarketCenter;
	CharSetting m_simLevel1MarketListing;

	UCharSetting m_simLrpLuldSymbolType;
	StringSetting m_simLrpLuldSymbol;
	UShortSetting m_simLrpLuldSymbolMap;
	UCharSetting m_simLrpLuldSource;
	UCharSetting m_simLrpLuldMessageType;
	PriceSetting m_simLrpLuldPriceLow;
	PriceSetting m_simLrpLuldPriceHigh;

	UCharSetting m_simStockTypeSymbolType;
	StringSetting m_simStockTypeSymbol;
	UShortSetting m_simStockTypeSymbolMap;
	UCharSetting m_simStockTypeSource;
	CharSetting m_simStockTypeSecurityType;

	UCharSetting m_simTradingActionSymbolType;
	StringSetting m_simTradingActionSymbol;
	UShortSetting m_simTradingActionSymbolMap;
	UCharSetting m_simTradingActionSource;
	CharSetting m_simTradingActionTradingState;
	StringAsUIntSetting m_simTradingActionReason;

	UCharSetting m_simClsPriceSymbolType;
	StringSetting m_simClsPriceSymbol;
	UShortSetting m_simClsPriceSymbolMap;
	UCharSetting m_simClsPriceSource;
	PriceSetting m_simClsPrice;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simClsPriceOption;
	BoolSetting m_simClsPriceOptionCall;
	CharSetting m_simClsPriceOptionSuffix;
	PriceSetting m_simClsPriceOptionStrikePrice;
	UIntSetting m_simClsPriceOptionExpiration;
#endif
/*
	UCharSetting m_simInvalidSymbolSymbolType;
	StringSetting m_simInvalidSymbolSymbol;
//	UShortSetting m_simInvalidSymbolSymbolMap;
	UCharSetting m_simInvalidSymbolSource;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simInvalidSymbolOption;
	BoolSetting m_simInvalidSymbolOptionCall;
	CharSetting m_simInvalidSymbolOptionSuffix;
	PriceSetting m_simInvalidSymbolOptionStrikePrice;
	UIntSetting m_simInvalidSymbolOptionExpiration;
#endif
*/
	UCharSetting m_simResetSymbolType;
	StringSetting m_simResetSymbol;
	UShortSetting m_simResetSymbolMap;
	UCharSetting m_simResetSource;
	UCharSetting m_simResetMessageType;
	UCharSetting m_simResetBook;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simResetOption;
	BoolSetting m_simResetOptionCall;
	CharSetting m_simResetOptionSuffix;
	PriceSetting m_simResetOptionStrikePrice;
	UIntSetting m_simResetOptionExpiration;
#endif

	StringSetting m_simIndexSnapshotSymbol;
	UCharSetting m_simIndexSnapshotFlags;
	StringSetting m_simIndexSnapshotDescription;

	UCharSetting m_simSnapshotSymbolType;
	StringSetting m_simSnapshotSymbol;
	UShortSetting m_simSnapshotSymbolMap;
	UCharSetting m_simSnapshotSource;
	UCharSetting m_simSnapshotMarketStatus;
	CharSetting m_simSnapshotPrimaryExchange;
	UCharSetting m_simSnapshotFlags;
	StringSetting m_simSnapshotDescription;
	UIntSetting m_simSnapshotRoundLot;
	BoolSetting m_simSnapshotRoundLotsOnly;
	UCharSetting m_simSnapshotMarketCategory;
	CharSetting m_simSnapshotFinancialStatus;
	CharSetting m_simSnapshotTradingState;
	StringAsUIntSetting m_simSnapshotTradingActionReason;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simSnapshotOption;
	BoolSetting m_simSnapshotOptionCall;
	CharSetting m_simSnapshotOptionSuffix;
	PriceSetting m_simSnapshotOptionStrikePrice;
	UIntSetting m_simSnapshotOptionExpiration;
#endif

	StringSetting m_simAcctRiskAccountId;

	StringSetting m_simBustExecAccountId;
	StringSetting m_simBustExecRefNumber;
	StringSetting m_simBustExecSymbol;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simBustExecOption;
	BoolSetting m_simBustExecOptionCall;
	CharSetting m_simBustExecOptionSuffix;
	PriceSetting m_simBustExecOptionStrikePrice;
	UIntSetting m_simBustExecOptionExpiration;
#endif
	BoolSetting m_simBustExecRef;
	BoolSetting m_simBustExecBusted;
	BoolSetting m_simBustExecDropcopy;
	UIntSetting m_simBustExecSize;
	PriceSetting m_simBustExecPrice;
	CharSetting m_simBustExecSide;

	UIntSetting m_simBustExecOrderSize;
	StringAsUnsignedNumberSetting<unsigned int> m_simBustExecMmid;
	StringAsUnsignedNumberSetting<unsigned int> m_simBustExecVenue;
	StringAsUnsignedNumberSetting<unsigned int> m_simBustExecLiquidity;

	UIntSetting m_simBustExecSendMessageToAllAccountsDelay;

//	PriceSetting m_simBustExecOrderPrice;
//////
	StringSetting m_simOrderUpdateAccountId;
	StringSetting m_simOrderUpdateSymbol;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simOrderUpdateOption;
	BoolSetting m_simOrderUpdateOptionCall;
	CharSetting m_simOrderUpdateOptionSuffix;
	PriceSetting m_simOrderUpdateOptionStrikePrice;
	UIntSetting m_simOrderUpdateOptionExpiration;
#endif
	UIntSetting m_simOrderUpdateSize;
	UIntSetting m_simOrderUpdateRemaining;
	UIntSetting m_simOrderUpdateClientId;
	UIntSetting m_simOrderUpdateServerId;
	UShortSetting m_simOrderUpdateReasonCode;
	PriceSetting m_simOrderUpdatePrice;
	PriceSetting m_simOrderUpdateBid;
	PriceSetting m_simOrderUpdateAsk;
	PriceSetting m_simOrderUpdateLast;
	BoolSetting m_simOrderUpdateHistorical;
//	BoolSetting m_simOrderUpdateReroutedToBaml;
	UCharSetting m_simOrderUpdateReRoute;
	UCharSetting m_simOrderUpdateMessageType;
	BoolSetting m_simOrderUpdateMarket;
	CharSetting m_simOrderUpdateSide;
	UIntSetting m_simOrderUpdateReserveSize;

	StringAsUnsignedNumberSetting<unsigned int> m_simOrderUpdateVenue;
	StringAsUnsignedNumberSetting<unsigned int> m_simOrderUpdateRouting;
	StringAsUnsignedNumberSetting<unsigned int> m_simOrderUpdateMmid;

	StringSetting m_simOrderUpdateMarketReferenceId;

//	BoolSetting m_simOrderUpdateBorrowAccept;//Use m_simOrderUpdateMessageType = 1

	UIntHexSetting m_simOrderUpdateExecutionInstructions;

	CharSetting m_simOrderUpdateLiquidity1;
	CharSetting m_simOrderUpdateLiquidity2;
	UIntSetting m_simOrderUpdateExecutionFilled;
	UIntSetting m_simOrderUpdateExecutionCancelled;
	UIntSetting m_simOrderUpdateExecutionRemaining;
	PriceSetting m_simOrderUpdateExecutionPrice;
	StringAsUnsignedNumberSetting<unsigned int> m_simOrderUpdateExecutionVenue;
	StringAsUnsignedNumberSetting<unsigned int> m_simOrderUpdateExecutionMmid;
	StringSetting m_simOrderUpdateExecutionReferenceId;
	UShortHexSetting m_simOrderUpdateExecutionReportFlags;
//////

#ifndef TAKION_NO_OPTIONS
	BoolSetting m_simPosLoadOption;
	BoolSetting m_simPosLoadOptionCall;
	CharSetting m_simPosLoadOptionSuffix;
	PriceSetting m_simPosLoadOptionStrikePrice;
	UIntSetting m_simPosLoadOptionExpiration;
#endif
	StringSetting m_simPosLoadAccountId;
	StringSetting m_simPosLoadSymbol;
	IntSetting m_simPosLoadSize;
	PriceSetting m_simPosLoadPrice;
	PriceSetting m_simPosLoadCost;
	IntSetting m_simPosLoadOvernightSize;
	PriceSetting m_simPosLoadOvernightPrice;
	IntSetting m_simPosLoadOpenOvernightSize;

	UIntSetting m_simPosLoadClsLongSize;
	MoneySetting m_simPosLoadClsLongCost;
	MoneySetting m_simPosLoadClsLongValue;

	UIntSetting m_simPosLoadClsShortSize;
	MoneySetting m_simPosLoadClsShortCost;
	MoneySetting m_simPosLoadClsShortValue;

	UIntSetting m_simPosLoadBorrowSize;
	PriceSetting m_simPosLoadBorrowPrice;

	UIntSetting m_simPosLoadSendMessageToAllAccountsDelay;
//
	StringSetting m_simSlAccountId;
	StringSetting m_simSlSymbol;
	UIntSetting m_simSlRequestId;
	StringSetting m_simSlFirm;
	PriceSetting m_simSlBorrowPrice;
	MoneySetting m_simSlBeta;
	UIntSetting m_simSlBorrowSize;
	UCharSetting m_simSlDiscountFlag;
	BoolSetting m_simSlOnTheFly;
	BoolSetting m_simSlOnTheFlyBorrow;
	BoolSetting m_simSlMarginable;
	UCharAsCharSetting m_simSlHtb;
	UCharAsCharSetting m_simSlAssetType;
	UCharAsCharSetting m_simSlSecType;
	UCharAsCharSetting m_simSlStatus;
	UCharSetting m_simSlMargin;
	UCharSetting m_simSlPmElRate;
	UCharSetting m_simSlPmEsRate;
	UCharSetting m_simSlPmHlRate;
	UCharSetting m_simSlPmHsRate;
	UCharSetting m_simSlElRate;
	UCharSetting m_simSlEsRate;
	UCharSetting m_simSlHlRate;
	UCharSetting m_simSlHsRate;
	UCharSetting m_simSlFedReq;
//
	StringSetting m_simEtbListAccountId;
	StringSetting m_simEtbListSymbol;
	UIntSetting m_simEtbListRequestId;
	StringSetting m_simEtbListFirm;
	UIntSetting m_simEtbListFlags;
	BoolSetting m_simEtbListDone;
//
	StringSetting m_simFirmPositionAccountId;
	StringSetting m_simFirmPositionSymbol;
	IntSetting m_simFirmPositionSize;
	UIntSetting m_simFirmPositionPendingSellPassive;
	UIntSetting m_simFirmPositionPendingSellNonPassive;
	UIntSetting m_simFirmPositionPendingShortPassive;
	UIntSetting m_simFirmPositionPendingShortNonPassive;

	StringSetting m_simUsAccountId;
	StringSetting m_simUsSymbol;
	StringSetting m_simUsFirm;//SlFirm 4 chars
	StringSetting m_simUsTraderId;
	UCharSetting m_simUsStatus;//Untradeable Status. 0 - tradeable; 1 - untradeable full; 2 - untradeable partial

	StringSetting m_simEnforceAggregationClearingFirmId;
	BoolSetting m_simEnforceAggregationEnforce;
	BoolSetting m_simEnforceAggregationConservativeOrderMarking;
	BoolSetting m_simEnforceAggregationLocalSideMarking;
	UIntSetting m_simEnforceAggregationEnforceNiteBetaTimeSeconds;
	MoneySetting m_simEnforceAggregationNiteBetaBp;

	CharSetting m_simMarketStatus;
	BoolSetting m_simMarketStatusSymbolMapping;

	UCharSetting m_simImbalanceSymbolType;
	StringSetting m_simImbalanceSymbol;
	UShortSetting m_simImbalanceSymbolMap;
	UCharSetting m_simImbalanceSource;
	UCharSetting m_simImbalanceBookId;
	UCharSetting m_simImbalanceType;
	UCharSetting m_simImbalanceDirection;
	BoolSetting m_simImbalanceRegulatory;
	BoolSetting m_simImbalanceFreezeStatus;
	UIntSetting m_simImbalance;
	UIntSetting m_simMarketImbalance;
	UIntSetting m_simPairedShares;
	PriceSetting m_simImbalancePriceFar;
	PriceSetting m_simImbalancePriceNear;
	PriceSetting m_simImbalancePriceRef;
	CharSetting m_simImbalancePv;
	UIntSetting m_simImbalanceAuctionTimeSeconds;
//
	UCharSetting m_simChartCorrectionSymbolType;
	StringSetting m_simChartCorrectionSymbol;
	UShortSetting m_simChartCorrectionSymbolMap;
	PriceSetting m_simChartCorrectionPriceStart;
	PriceSetting m_simChartCorrectionPriceHigh;
	PriceSetting m_simChartCorrectionPriceLow;
	PriceSetting m_simChartCorrectionPriceEnd;
	MoneySetting m_simChartCorrectionMoneyTraded;
	UIntSetting m_simChartCorrectionVolume;
	UShortSetting m_simChartCorrectionMinute;
//
	UCharSetting m_simChartSnapshotSymbolType;
	StringSetting m_simChartSnapshotSymbol;
	UShortSetting m_simChartSnapshotSymbolMap;
	UIntSetting m_simChartSnapshotRequestId;
	UShortSetting m_simChartSnapshotPointCount;
	UShortSetting m_simChartSnapshotStartMinute;
	PriceSetting m_simChartSnapshotPriceStart;
	PriceSetting m_simChartSnapshotPriceHigh;
	PriceSetting m_simChartSnapshotPriceLow;
	PriceSetting m_simChartSnapshotPriceEnd;
	MoneySetting m_simChartSnapshotMoneyTraded;
	UIntSetting m_simChartSnapshotVolume;
	BoolSetting m_simChartSnapshotDone;
//

	StringSetting m_simAdminBroadcastText;
	
	StringSetting m_tcSymbol;
	UCharSetting m_tcDecDigit;

	StringSetting m_hcAllChartFileName;
	StringSetting m_hcSymbol;
	UCharSetting m_hcDecDigit;
	BoolSetting m_hcDayCountMode;
	UIntSetting m_hcDayCount;
	UIntSetting m_hcDateFrom;
	UCharSetting m_hcFrequency;

//	BoolSetting m_miLoadOnStart;
	BoolSetting m_miInThread;
	BoolSetting m_miSendToMm;
	UIntSetting m_miMarketCategoryFilter;
	UIntSetting m_miTestStockFilter;
	CharSetting m_miCharFrom;
	CharSetting m_miCharTo;
	UInt64Setting m_miYestVolumeFrom;
	UInt64Setting m_miYestVolumeTo;

	UIntHexSetting m_simMdLogonMdVersion;
	UIntSetting m_simMdLogonCompression;
	UIntSetting m_simMdLogonInstrument;
	UIntSetting m_simMdLogonResult;
	BoolSetting m_simMdLogonSubscriptionOnly;
	BoolSetting m_simMdLogonForceSubscriptionOnly;
	BoolSetting m_simMdLogonDisableExtensions;
	UInt64HexSetting m_simMdLogonEntitlements;
	UInt64HexSetting m_simMdLogonExtEntitlements;

//	UShortSetting m_topicOrdinal;
	StringSetting m_topic;
	UShortSetting m_newsDaysBack;
	UIntSetting m_newsHeadlineCount;
	BoolSetting m_newsAfterMarket;
	UCharSetting m_searchType;
	BoolSetting m_allEntitled;
	UIntSetting m_storyFilter;
	UIntSetting m_metaFilter;
	UIntSetting m_relevance;
	BoolSetting m_showStoryChain;
	typedef PairSetting<UIntSetting, UShortSetting> WireSetting;
	typedef SetSetting<WireSetting, std::set<WireSetting> > WireSettingSet;
	WireSettingSet m_wireSettingSet;

	StringSetting m_cancelOrderAccountId;
	StringSetting m_cancelOrderSymbol;
#ifndef TAKION_NO_OPTIONS
	BoolSetting m_cancelOrderOption;
	BoolSetting m_cancelOrderOptionCall;
	CharSetting m_cancelOrderOptionSuffix;
	PriceSetting m_cancelOrderOptionStrikePrice;
	UIntSetting m_cancelOrderOptionExpiration;
#endif
	UIntSetting m_cancelOrderClientId;
	BoolSetting m_cancelOrderUseClientId;
	BoolSetting m_cancelOrderAccountAll;
	UCharSetting m_cancelOrderPosition;
#ifndef TAKION_NO_OPTIONS
	UCharSetting m_cancelOrderInstrument;
#endif
	UCharSetting m_cancelOrderSide;
	UCharSetting m_cancelOrderDirection;
	UCharSetting m_cancelOrderPick;
	UIntSetting m_cancelOrderTif;
	UCharSetting m_cancelOrderType;
	UCharSetting m_cancelOrderPqOpen;
	UCharSetting m_cancelOrderTClose;
	UCharSetting m_cancelOrderExcludeDestinations;
	SetSetting<StringSetting, std::set<StringSetting> > m_cancelOrderDestinations;
	UCharSetting m_cancelOrderExcludeMnemonics;
	MapSetting<StringSetting, BoolSetting, std::map<StringSetting, BoolSetting> > m_cancelOrderMnemonics;
	BoolSetting m_cancelOrderReplace;
	BoolSetting m_cancelOrderReplaceNative;
	BoolSetting m_cancelOrderReplaceMarket;
	BoolSetting m_cancelOrderReplacePriceOffsetPercent;
	BoolSetting m_cancelOrderReplacePriceEnforceMinOffset;
	BoolSetting m_cancelOrderReplacePriceOffsetReverseForSell;
	BoolSetting m_cancelOrderReplaceSizeOffsetPercent;
	BoolSetting m_cancelOrderReplaceSizeOffsetReverseForSell;
	BoolSetting m_cancelOrderReplaceAdjustable;
	BoolSetting m_cancelOrderReplaceAdjustableUseMmBox;
	BoolSetting m_cancelOrderReplacePreBorrow;
	UCharSetting m_cancelOrderDisplaySizeMode;

	IntSetting m_cancelOrdeReplacePriceOffset;
	IntSetting m_cancelOrdeReplaceSizeOffset;
	UIntSetting m_cancelOrdeReplaceBorrowPrice;
	UIntSetting m_cancelOrdeReplaceDisplaySizeFraction;
	UIntSetting m_cancelOrdeReplaceDisplaySizeRoundLot;

	BoolSetting m_commandPost;
	StringSetting m_commandName;
	StringSetting m_commandSymbol;
protected:
	void AddSettings();
//	virtual void AdjustOldVersion() override;
};