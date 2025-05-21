#include "stdafx.h"
#include "SampleDialogSettingAccountAction.h"
//#include "StockMarketSorter.h"
#include "ExtensionSample.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

enum SampleSettingAccountActionIds
{
	buttonDestroyUntradedPositionsId = baseDialogSettingAccountIdsCount,
	buttonDestroyAllUntradedPositionsId,
	buttonSetPhantomPositionsFlatId,
	buttonSetAllPhantomPositionsFlatId,

	staticSymbolMarketSorterId,
	editSymbolMarketSorterId,
	buttonSubscribeMarketSorterId,
	buttonResetMarketSorterId,

	staticSymbolMarketDataId,
	editSymbolMarketDataId,
	buttonSubscribeMarketDataId,
	buttonResetMarketDataId,

#ifndef TAKION_NO_OPTIONS
	checkBoxMarketDataOptionId,

	radioMarketDataOptionCallId,
	radioMarketDataOptionPutId,

	comboBoxMarketDataOptionSuffixId,
	staticMarketDataOptionSuffixId,

	staticMarketDataOptionStrikePriceId,
	spinMarketDataOptionStrikePriceId,

	staticMarketDataOptionExpirationId,
	datetimeMarketDataOptionExpirationId,
#endif
	accountActionIdsCount
};

SampleDialogSettingAccountAction::SampleDialogSettingAccountAction(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab)://, const Account* account):
	BaseDialogSettingAccount(mainWnd, parentTab, true, "Action", true, 56, 0, true),

	m_EditSymbolMarketSorter(U_GetValidStockChars(), false),
	m_securityMarketSorter(nullptr),
	m_positionMarketSorter(nullptr),

	m_EditSymbolMarketData(U_GetValidStockChars(), false),

#ifndef TAKION_NO_OPTIONS
	m_CheckBoxOption("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_RadioOptionCall("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),//4, -3),
	m_RadioOptionPut("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),//4, -3),
	m_ComboBoxOptionSuffix(m_visualBase->GetGrayColor()),
	m_SpinOptionStrikePrice(99999999, 0, 0, 1000, 3, TakionSpin::charComma, 2),
	m_DateTimeOptionExpiration(457, 0xFFFFFFFF),

	m_optionMode(false),
	m_optionSuffix('\0'),
	m_optionCall(false),
	m_expirationYear(0),
	m_expirationMonth(0),
	m_expirationDay(0),

	m_currentOptionMode(false),
	m_currentOptionSuffix('\0'),
	m_currentOptionCall(false),
	m_currentExpirationYear(0),
	m_currentExpirationMonth(0),
	m_currentExpirationDay(0),
#endif

	m_securityMarketData(nullptr),
	m_positionMarketData(nullptr),

	m_colorGreen(RGB(0, 128, 0)),
	m_colorRed(RGB(128, 0, 0)),

	m_groupBoxAccountReady(this, 0, RGB(255, 255, 255), nullptr, "Account Ready", 8),

	m_labelAccountReady(this, DT_LEFT, 0, 0xFFFFFFFF, nullptr, nullptr, "Can Trade:", false),
	m_valueAccountReady(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelHistoryLoaded(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Positions and Live Orders Loaded:", false),
	m_valueHistoryLoaded(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelUntradeableStatusLoaded(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Untradeable Status Loaded:", false),
	m_valueUntradeableStatusLoaded(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelRestrictedSymbolListLoaded(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Restricted Symbol List Loaded:", false),
	m_valueRestrictedSymbolListLoaded(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelTradingAllowed(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Trading Allowed:", false),
	m_valueTradingAllowed(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelSuspended(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Suspended:", false),
	m_valueSuspended(this, DT_LEFT, m_colorGreen, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelExecutorConnected(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Executor Connected:", false),
	m_valueExecutorConnected(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelExecutorLogged(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Executor Logged:", false),
	m_valueExecutorLogged(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

#ifndef TAKION_NO_OPTIONS
	m_labelFutureExecutorConnected(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "FutureExecutor Connected:", false),
	m_valueFutureExecutorConnected(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelFutureExecutorLogged(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Future Executor Logged:", false),
	m_valueFutureExecutorLogged(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),
#endif
	m_labelTraderIdEnabled(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "TraderId Enabled:", false),
	m_valueTraderIdEnabled(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelTradingLockedByTrader(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Trading Locked by Trader:", false),
	m_valueTradingLockedByTrader(this, DT_LEFT, m_colorGreen, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelTradingDefaultAccountOnlyByTrader(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Trading Default Account Only by Trader:", false),
	m_valueTradingDefaultAccountOnlyByTrader(this, DT_LEFT, m_colorGreen, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelTradingTestStocksOnlyByTrader(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Trading Test Stocks Only by Trader:", false),
	m_valueTradingTestStocksOnlyByTrader(this, DT_LEFT, m_colorGreen, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_groupBoxMarketSorterReady(this, 0, RGB(255, 255, 255), nullptr, "MarketSorter Ready", 8),

	m_valueMarketSorterCurrentStock(this, DT_LEFT, 0, 0xFFFFFFFF, nullptr, nullptr, "", false),

	m_labelMarketSorterAllStocksLoaded(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "MarketSorter All Stocks Loaded:", false),
	m_valueMarketSorterAllStocksLoaded(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelMarketSorterAllIndicesLoaded(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "MarketSorter All Indices Loaded:", false),
	m_valueMarketSorterAllIndicesLoaded(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelMarketSorterAllStockChartsLoaded(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "MarketSorter All Stock Charts Loaded:", false),
	m_valueMarketSorterAllStockChartsLoaded(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelMarketSorterAllIndexChartsLoaded(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "MarketSorter All Index Charts Loaded:", false),
	m_valueMarketSorterAllIndexChartsLoaded(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelMarketSorterConnected(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "MarketSorter Connected:", false),
	m_valueMarketSorterConnected(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelMarketSorterLogged(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "MarketSorter Logged:", false),
	m_valueMarketSorterLogged(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelMarketSorterStockLoaded(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Stock Loaded:", false),
	m_valueMarketSorterStockLoaded(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelMarketSorterStockInvalid(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Stock Invalid:", false),
	m_valueMarketSorterStockInvalid(this, DT_LEFT, m_colorGreen, 0xFFFFFFFF, nullptr, nullptr, "No", false),

	m_labelMarketSorterPositionSize(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Position Size:", false),
	m_valueMarketSorterPositionSize(this, DT_LEFT, 0, 0xFFFFFFFF, nullptr, nullptr, "", false),

	m_marketSorterAllStocksLoaded(false),
	m_marketSorterAllIndicesLoaded(false),
	m_marketSorterAllStockChartsLoaded(false),
	m_marketSorterAllIndexChartsLoaded(false),
	m_marketSorterConnected(false),
	m_marketSorterLogged(false),
	m_marketSorterStockLoaded(false),
	m_marketSorterStockInvalid(false),
	m_marketSorterPositionSize(0),

//MarketData
	m_groupBoxMarketDataReady(this, 0, RGB(255, 255, 255), nullptr, "MarketData Ready", 8),

	m_valueMarketDataCurrentStock(this, DT_LEFT, 0, 0xFFFFFFFF, nullptr, nullptr, "", false),

#ifndef TAKION_NO_OPTIONS
	m_groupBoxOption(this, 0, RGB(255, 255, 255), nullptr, "", 0),
	m_labelOptionMarketDataConnected(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Option MarketData Connected:", false),
	m_valueOptionMarketDataConnected(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),
	m_labelOptionMarketDataLogged(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Option MarketData Logged:", false),
	m_valueOptionMarketDataLogged(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),
	m_optionMarketDataConnected(false),
	m_optionMarketDataLogged(false),

	m_labelFutureMarketDataConnected(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Future MarketData Connected:", false),
	m_valueFutureMarketDataConnected(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),
	m_labelFutureMarketDataLogged(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Future MarketData Logged:", false),
	m_valueFutureMarketDataLogged(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),
	m_futureMarketDataConnected(false),
	m_futureMarketDataLogged(false),
#endif
	m_labelStockMarketDataConnected(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Stock MarketData Connected:", false),
	m_valueStockMarketDataConnected(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),
	m_labelStockMarketDataLogged(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Stock MarketData Logged:", false),
	m_valueStockMarketDataLogged(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),
	m_stockMarketDataConnected(false),
	m_stockMarketDataLogged(false),

	m_labelMarketDataStockLoaded(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Stock Loaded:", false),
	m_valueMarketDataStockLoaded(this, DT_LEFT, m_colorRed, 0xFFFFFFFF, nullptr, nullptr, "No", false),
	m_labelMarketDataStockInvalid(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Stock Invalid:", false),
	m_valueMarketDataStockInvalid(this, DT_LEFT, m_colorGreen, 0xFFFFFFFF, nullptr, nullptr, "No", false),
	m_labelMarketDataPositionSize(this, DT_RIGHT, 0, 0xFFFFFFFF, nullptr, nullptr, "Position Size:", false),
	m_valueMarketDataPositionSize(this, DT_LEFT, 0, 0xFFFFFFFF, nullptr, nullptr, "", false),
	m_marketDataStockLoaded(false),
	m_marketDataStockInvalid(false),
	m_marketDataPositionSize(0),

	m_resizeInitialized(false)
{
	*m_marketSorterPositionSizeText = '\0';
	*m_marketDataPositionSizeText = '\0';
	int x = 0;
	const int xc = m_accountWidth + hgap;
	const int buttonWidth = 110;
	int y = 0;
//	m_bottomControl += vgap;
	AddButtonIndirect("Destroy Untraded Positions", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc, y, buttonWidth, editHeight, buttonDestroyUntradedPositionsId);
	AddButtonIndirect("Set Phantom Positions Flat", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc, y += editHeight + vgap, buttonWidth, editHeight, buttonSetPhantomPositionsFlatId);

	AddButtonIndirect("Destroy ALL Untraded Positions", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc, y += editHeight + vgap + vgap, buttonWidth, editHeight, buttonDestroyAllUntradedPositionsId);
	AddButtonIndirect("Set ALL Phantom Positions Flat", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc, y += editHeight + vgap, buttonWidth, editHeight, buttonSetAllPhantomPositionsFlatId);

	const int width = m_accountWidth - hgap - hgap;
	const int width34 = width * 3 / 4;
	AddStaticIndirect("Symbol", WS_VISIBLE|SS_NOPREFIX, 0, xc, y += editHeight + vgap, width, staticHeight, staticSymbolMarketSorterId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL|ES_UPPERCASE, 0, xc, y += staticHeight, width, editHeight, editSymbolMarketSorterId);
	x = xc + width + hgap;
	AddButtonIndirect("Subscribe", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x, y, width, editHeight, buttonSubscribeMarketSorterId);
	AddButtonIndirect("Reset", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x + width + hgap, y, width34, editHeight, buttonResetMarketSorterId);

	AddStaticIndirect("Symbol", WS_VISIBLE|SS_NOPREFIX, 0, xc, y += editHeight + vgap, width, staticHeight, staticSymbolMarketDataId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL|ES_UPPERCASE, 0, xc, y += staticHeight, width, editHeight, editSymbolMarketDataId);
	x = xc + width + hgap;
	AddButtonIndirect("Subscribe", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x, y, width, editHeight, buttonSubscribeMarketDataId);
	AddButtonIndirect("Reset", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x + width + hgap, y, width34, editHeight, buttonResetMarketDataId);

#ifndef TAKION_NO_OPTIONS
	m_ComboBoxOptionSuffix.SetColorInvalid(m_visualBase->GetGrayColor());

	m_SpinOptionStrikePrice.AddSpinCell(1, false);
	m_SpinOptionStrikePrice.AddSpinCell(100, false);
	m_SpinOptionStrikePrice.AddSpinCell(10000, true);

	int halfWidth = width / 2;
	AddButtonIndirect("Option", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, xc, y, width, checkBoxHeight, checkBoxMarketDataOptionId);

	AddButtonIndirect("Call", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += checkBoxHeight + vgap, halfWidth, checkBoxHeight, radioMarketDataOptionCallId);
	AddButtonIndirect("Put", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc + halfWidth, y, halfWidth, checkBoxHeight, radioMarketDataOptionPutId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|CBS_SORT|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED|WS_VSCROLL|WS_GROUP, 0, xc, y += checkBoxHeight + vgap, halfWidth, 400, comboBoxMarketDataOptionSuffixId);
	AddStaticIndirect("Suffix", WS_VISIBLE|SS_NOPREFIX, 0, xc + halfWidth, y + (comboBoxHeight - staticHeight) / 2, halfWidth, staticHeight, staticMarketDataOptionSuffixId);

	AddStaticIndirect("StrikePrice", WS_VISIBLE|SS_NOPREFIX, 0, xc, y += comboBoxHeight + vgap, width, staticHeight, staticMarketDataOptionStrikePriceId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += staticHeight, width, spinHeight, spinMarketDataOptionStrikePriceId);

	AddStaticIndirect("Expiration", WS_VISIBLE|SS_NOPREFIX, 0, xc, y += spinHeight + vgap, width, staticHeight, staticMarketDataOptionExpirationId);
	AddComponentIndirect("SysDateTimePick32", nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP|DTS_RIGHTALIGN, 0, xc, y += staticHeight, width, comboBoxHeight, datetimeMarketDataOptionExpirationId);
#endif
//	m_bottomControl += editHeight;
//Executor
	AddCustomGroupBox(&m_groupBoxAccountReady);
	AddStringArea(&m_labelAccountReady);
	AddStringArea(&m_valueAccountReady);
	AddStringArea(&m_labelHistoryLoaded);
	AddStringArea(&m_valueHistoryLoaded);
	AddStringArea(&m_labelUntradeableStatusLoaded);
	AddStringArea(&m_valueUntradeableStatusLoaded);
	AddStringArea(&m_labelRestrictedSymbolListLoaded);
	AddStringArea(&m_valueRestrictedSymbolListLoaded);
	AddStringArea(&m_labelTradingAllowed);
	AddStringArea(&m_valueTradingAllowed);
	AddStringArea(&m_labelSuspended);
	AddStringArea(&m_valueSuspended);
	AddStringArea(&m_labelExecutorConnected);
	AddStringArea(&m_valueExecutorConnected);
	AddStringArea(&m_labelExecutorLogged);
	AddStringArea(&m_valueExecutorLogged);
#ifndef TAKION_NO_OPTIONS
	AddStringArea(&m_labelFutureExecutorConnected);
	AddStringArea(&m_valueFutureExecutorConnected);
	AddStringArea(&m_labelFutureExecutorLogged);
	AddStringArea(&m_valueFutureExecutorLogged);
#endif
	AddStringArea(&m_labelTraderIdEnabled);
	AddStringArea(&m_valueTraderIdEnabled);
	AddStringArea(&m_labelTradingLockedByTrader);
	AddStringArea(&m_valueTradingLockedByTrader);
	AddStringArea(&m_labelTradingDefaultAccountOnlyByTrader);
	AddStringArea(&m_valueTradingDefaultAccountOnlyByTrader);
	AddStringArea(&m_labelTradingTestStocksOnlyByTrader);
	AddStringArea(&m_valueTradingTestStocksOnlyByTrader);

//MarketSorter
	AddCustomGroupBox(&m_groupBoxMarketSorterReady);
	AddStringArea(&m_valueMarketSorterCurrentStock);

	AddStringArea(&m_labelMarketSorterAllStocksLoaded);
	AddStringArea(&m_valueMarketSorterAllStocksLoaded);
	AddStringArea(&m_labelMarketSorterAllIndicesLoaded);
	AddStringArea(&m_valueMarketSorterAllIndicesLoaded);
	AddStringArea(&m_labelMarketSorterAllStockChartsLoaded);
	AddStringArea(&m_valueMarketSorterAllStockChartsLoaded);
	AddStringArea(&m_labelMarketSorterAllIndexChartsLoaded);
	AddStringArea(&m_valueMarketSorterAllIndexChartsLoaded);
	AddStringArea(&m_labelMarketSorterConnected);
	AddStringArea(&m_valueMarketSorterConnected);
	AddStringArea(&m_labelMarketSorterLogged);
	AddStringArea(&m_valueMarketSorterLogged);
	AddStringArea(&m_labelMarketSorterStockLoaded);
	AddStringArea(&m_valueMarketSorterStockLoaded);
	AddStringArea(&m_labelMarketSorterStockInvalid);
	AddStringArea(&m_valueMarketSorterStockInvalid);
	AddStringArea(&m_labelMarketSorterPositionSize);
	AddStringArea(&m_valueMarketSorterPositionSize);

//MarketData
	AddCustomGroupBox(&m_groupBoxMarketDataReady);
	AddStringArea(&m_valueMarketDataCurrentStock);

	AddStringArea(&m_labelStockMarketDataConnected);
	AddStringArea(&m_valueStockMarketDataConnected);
	AddStringArea(&m_labelStockMarketDataLogged);
	AddStringArea(&m_valueStockMarketDataLogged);

#ifndef TAKION_NO_OPTIONS
	AddCustomGroupBox(&m_groupBoxOption);

//OptionMarketData
	AddStringArea(&m_labelOptionMarketDataConnected);
	AddStringArea(&m_valueOptionMarketDataConnected);
	AddStringArea(&m_labelOptionMarketDataLogged);
	AddStringArea(&m_valueOptionMarketDataLogged);
//FutureMarketData
	AddStringArea(&m_labelFutureMarketDataConnected);
	AddStringArea(&m_valueFutureMarketDataConnected);
	AddStringArea(&m_labelFutureMarketDataLogged);
	AddStringArea(&m_valueFutureMarketDataLogged);

	m_RadioOptionCall.SetRadioGroupOwner(&m_RadioGroupOption);
	m_RadioOptionPut.SetRadioGroupOwner(&m_RadioGroupOption);
#endif
	AddStringArea(&m_labelMarketDataStockLoaded);
	AddStringArea(&m_valueMarketDataStockLoaded);
	AddStringArea(&m_labelMarketDataStockInvalid);
	AddStringArea(&m_valueMarketDataStockInvalid);
	AddStringArea(&m_labelMarketDataPositionSize);
	AddStringArea(&m_valueMarketDataPositionSize);
}

void SampleDialogSettingAccountAction::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingAccount::ToolTipsEnabled(enable);
#ifndef TAKION_NO_OPTIONS
	m_CheckBoxOption.EnableTooltips(enable, m_toolTip);
	m_RadioOptionCall.EnableTooltips(enable, m_toolTip);
	m_RadioOptionPut.EnableTooltips(enable, m_toolTip);
#endif
}

void SampleDialogSettingAccountAction::ResetStrings()
{
	UpdateAccountReadyToTrade();
	SetStringValue(m_valueTradingAllowed, m_tradingAllowed);
	SetStringValueStatic(m_valueSuspended, m_suspended, m_colorRed, m_colorGreen);
	SetStringValue(m_valueHistoryLoaded, m_historyLoaded);// m_account->isHistoryLoaded());
	SetStringValue(m_valueUntradeableStatusLoaded, m_untradeableStatusLoaded);
	SetStringValue(m_valueRestrictedSymbolListLoaded, m_restrictedSymbolListLoaded);
	SetStringValue(m_valueExecutorConnected, m_executorConnected);
	SetStringValue(m_valueExecutorLogged, m_executorLogged);
#ifndef TAKION_NO_OPTIONS
	SetStringValue(m_valueFutureExecutorConnected, m_futureExecutorConnected);
	SetStringValue(m_valueFutureExecutorLogged, m_futureExecutorLogged);
#endif
	SetStringValue(m_valueTraderIdEnabled, m_traderIdEnabled);
	SetStringValueStatic(m_valueTradingLockedByTrader, m_tradingLockedByTrader, m_colorRed, m_colorGreen);
	SetStringValueStatic(m_valueTradingDefaultAccountOnlyByTrader, m_tradingDefaultAccountOnlyByTrader, m_colorRed, m_colorGreen);
	SetStringValueStatic(m_valueTradingTestStocksOnlyByTrader, m_tradingTestStocksOnlyByTrader, m_colorRed, m_colorGreen);

//MarketSorter
	m_valueMarketSorterCurrentStock.SetTextInRect(m_currentSymbolMarketSorter.c_str());

	SetStringValue(m_valueMarketSorterAllStocksLoaded, m_marketSorterAllStocksLoaded);
	SetStringValue(m_valueMarketSorterAllIndicesLoaded, m_marketSorterAllIndicesLoaded);
	SetStringValue(m_valueMarketSorterAllStockChartsLoaded, m_marketSorterAllStockChartsLoaded);
	SetStringValue(m_valueMarketSorterAllIndexChartsLoaded, m_marketSorterAllIndexChartsLoaded);
	SetStringValue(m_valueMarketSorterConnected, m_marketSorterConnected);
	SetStringValue(m_valueMarketSorterLogged, m_marketSorterLogged);

	SetStringValue(m_valueMarketSorterStockLoaded, m_marketSorterStockLoaded);
	SetStringValueStatic(m_valueMarketSorterStockInvalid, m_marketSorterStockInvalid, m_colorRed, m_colorGreen);
//	char num[33];
//	m_valueMarketSorterPositionSize.SetTextInRect(U_SignedNumberToStrNoDecDigits(m_marketSorterPositionSize, num, sizeof(num)));
//	m_valueMarketSorterPositionSize.SetTextInRect(m_marketSorterPositionSizeText);
	SetMarketSorterPositionSize(0, true);

//MarketData
	m_valueMarketDataCurrentStock.SetTextInRect(m_realSymbolMarketData.c_str());// m_currentSymbolMarketData.c_str());

	SetStringValue(m_valueStockMarketDataConnected, m_stockMarketDataConnected);
	SetStringValue(m_valueStockMarketDataLogged, m_stockMarketDataLogged);
#ifndef TAKION_NO_OPTIONS
	SetStringValue(m_valueOptionMarketDataConnected, m_optionMarketDataConnected);
	SetStringValue(m_valueOptionMarketDataLogged, m_optionMarketDataLogged);

	SetStringValue(m_valueFutureMarketDataConnected, m_futureMarketDataConnected);
	SetStringValue(m_valueFutureMarketDataLogged, m_futureMarketDataLogged);
#endif
	SetStringValue(m_valueMarketDataStockLoaded, m_marketDataStockLoaded);
	SetStringValueStatic(m_valueMarketDataStockInvalid, m_marketDataStockInvalid, m_colorRed, m_colorGreen);

//	m_valueMarketDataPositionSize.SetTextInRect(U_SignedNumberToStrNoDecDigits(m_marketDataPositionSize, num, sizeof(num)));
	m_valueMarketDataPositionSize.SetTextInRect(m_marketDataPositionSizeText);
	SetMarketDataPositionSize(0, true);
}

void SampleDialogSettingAccountAction::SetMarketSorterAllStocksLoaded(const bool loaded)
{
	if(loaded != m_marketSorterAllStocksLoaded)
	{
		m_marketSorterAllStocksLoaded = loaded;

		SetStringValue(m_valueMarketSorterAllStocksLoaded, m_marketSorterAllStocksLoaded);
		if(loaded)
		{
			if(!m_currentSymbolMarketSorter.empty() && !m_securityMarketSorter)
			SetMarketSorterStockInvalid(true);
		}
	}
}

void SampleDialogSettingAccountAction::SetMarketSorterAllIndicesLoaded(const bool loaded)
{
	if(loaded != m_marketSorterAllIndicesLoaded)
	{
		m_marketSorterAllIndicesLoaded = loaded;

		SetStringValue(m_valueMarketSorterAllIndicesLoaded, m_marketSorterAllIndicesLoaded);
/*
		if(loaded)
		{
			if(!m_currentSymbolIndexMarketSorter.empty() && !m_indexMarketSorter)
			SetMarketSorterIndexInvalid(true);
		}
*/
	}
}

void SampleDialogSettingAccountAction::SetMarketSorterAllStockChartsLoaded(const bool loaded)
{
	if(loaded != m_marketSorterAllStockChartsLoaded)
	{
		m_marketSorterAllStockChartsLoaded = loaded;

		SetStringValue(m_valueMarketSorterAllStockChartsLoaded, m_marketSorterAllStockChartsLoaded);
	}
}

void SampleDialogSettingAccountAction::SetMarketSorterAllIndexChartsLoaded(const bool loaded)
{
	if(loaded != m_marketSorterAllIndexChartsLoaded)
	{
		m_marketSorterAllIndexChartsLoaded = loaded;

		SetStringValue(m_valueMarketSorterAllIndexChartsLoaded, m_marketSorterAllIndexChartsLoaded);
	}
}

void SampleDialogSettingAccountAction::SetMarketSorterConnected(const bool connected)
{
	if(connected != m_marketSorterConnected)
	{
		m_marketSorterConnected = connected;

//		OnMarketSorterConnected();
		SetStringValue(m_valueMarketSorterConnected, m_marketSorterConnected);
//		UpdateAccountReadyToTrade();

//		SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);
		if(!connected)
		{
			SetMarketSorterLogged(false);
		}
	}
}

void SampleDialogSettingAccountAction::SetMarketSorterLogged(const bool logged)
{
	if(logged != m_marketSorterLogged)
	{
		m_marketSorterLogged = logged;

//		OnExecutorLogged();
		SetStringValue(m_valueMarketSorterLogged, m_marketSorterLogged);

		if(!logged)
		{
			SetMarketSorterAllStocksLoaded(false);
			SetMarketSorterAllIndicesLoaded(false);
			SetMarketSorterAllStockChartsLoaded(false);
			SetMarketSorterAllIndexChartsLoaded(false);
			SetMarketSorterStockLoaded(false);
			SetMarketSorterStockInvalid(false);
		}
	}
}

void SampleDialogSettingAccountAction::SetMarketSorterStockLoaded(const bool loaded)
{
	if(loaded != m_marketSorterStockLoaded)
	{
		m_marketSorterStockLoaded = loaded;

		SetStringValue(m_valueMarketSorterStockLoaded, m_marketSorterStockLoaded);
	}
}

void SampleDialogSettingAccountAction::SetMarketSorterStockInvalid(const bool invalid)
{
	if(invalid != m_marketSorterStockInvalid)
	{
		m_marketSorterStockInvalid = invalid;

		SetStringValueStatic(m_valueMarketSorterStockInvalid, m_marketSorterStockInvalid, m_colorRed, m_colorGreen);
	}
}

void SampleDialogSettingAccountAction::SetMarketSorterPositionSize(const int positionSize, const bool noPosition)
{
	if(noPosition)
	{
		m_marketSorterPositionSize = 0;
		*m_marketSorterPositionSizeText = '\0';
		m_valueMarketSorterPositionSize.SetTextInRect(m_marketSorterPositionSizeText);
	}
	else if(positionSize != m_marketSorterPositionSize)
	{
		m_marketSorterPositionSize = positionSize;
		char num[33];
		strcpy_s(m_marketSorterPositionSizeText, sizeof(m_marketSorterPositionSizeText), U_SignedNumberToStrNoDecDigits(m_marketSorterPositionSize, num, sizeof(num)));
//		m_valueMarketSorterPositionSize.SetTextInRect(U_SignedNumberToStrNoDecDigits(m_marketSorterPositionSize, num, sizeof(num)));
		m_valueMarketSorterPositionSize.SetTextInRect(m_marketSorterPositionSizeText);
	}
}


void SampleDialogSettingAccountAction::SetStockMarketDataConnected(const bool connected)
{
	if(connected != m_stockMarketDataConnected)
	{
		m_stockMarketDataConnected = connected;

		SetStringValue(m_valueStockMarketDataConnected, m_stockMarketDataConnected);
		if(!connected)
		{
			SetStockMarketDataLogged(false);
		}
	}
}

void SampleDialogSettingAccountAction::SetStockMarketDataLogged(const bool logged)
{
	if(logged != m_stockMarketDataLogged)
	{
		m_stockMarketDataLogged = logged;

		SetStringValue(m_valueStockMarketDataLogged, m_stockMarketDataLogged);
		if(!logged)
		{
#ifndef TAKION_NO_OPTIONS
			if(!m_securityMarketData || !m_securityMarketData->isOption() && !m_securityMarketData->isFuture())
			{
#endif
				SetSecurityMarketData(nullptr);
				SetMarketDataStockLoaded(false);
				SetMarketDataStockInvalid(false);
#ifndef TAKION_NO_OPTIONS
			}
#endif
		}
	}
}

void SampleDialogSettingAccountAction::SetMarketDataStockLoaded(const bool loaded)
{
	if(loaded != m_marketDataStockLoaded)
	{
		m_marketDataStockLoaded = loaded;

		SetStringValue(m_valueMarketDataStockLoaded, m_marketDataStockLoaded);
	}
}

void SampleDialogSettingAccountAction::SetMarketDataStockInvalid(const bool invalid)
{
	if(invalid != m_marketDataStockInvalid)
	{
		m_marketDataStockInvalid = invalid;

		SetStringValueStatic(m_valueMarketDataStockInvalid, m_marketDataStockInvalid, m_colorRed, m_colorGreen);
	}
}

void SampleDialogSettingAccountAction::SetMarketDataPositionSize(const int positionSize, const bool noPosition)
{
	if(noPosition)
	{
		m_marketDataPositionSize = 0;
		*m_marketDataPositionSizeText = '\0';
		m_valueMarketDataPositionSize.SetTextInRect(m_marketDataPositionSizeText);
	}
	else if(positionSize != m_marketDataPositionSize)
	{
		m_marketDataPositionSize = positionSize;
		char num[33];
		strcpy_s(m_marketDataPositionSizeText, sizeof(m_marketDataPositionSizeText), U_SignedNumberToStrNoDecDigits(m_marketDataPositionSize, num, sizeof(num)));
//		m_valueMarketDataPositionSize.SetTextInRect(U_SignedNumberToStrNoDecDigits(m_marketDataPositionSize, num, sizeof(num)));
		m_valueMarketDataPositionSize.SetTextInRect(m_marketDataPositionSizeText);
	}
}

#ifndef TAKION_NO_OPTIONS
void SampleDialogSettingAccountAction::SetOptionMarketDataConnected(const bool connected)
{
	if(connected != m_optionMarketDataConnected)
	{
		m_optionMarketDataConnected = connected;

		SetStringValue(m_valueOptionMarketDataConnected, m_optionMarketDataConnected);
		if(!connected)
		{
			SetOptionMarketDataLogged(false);
		}
	}
}

void SampleDialogSettingAccountAction::SetOptionMarketDataLogged(const bool logged)
{
	if(logged != m_optionMarketDataLogged)
	{
		m_optionMarketDataLogged = logged;

		SetStringValue(m_valueOptionMarketDataLogged, m_optionMarketDataLogged);
		if(!logged)
		{
			if(!m_securityMarketData || m_securityMarketData->isOption())
			{
				SetSecurityMarketData(nullptr);
				SetMarketDataStockLoaded(false);
				SetMarketDataStockInvalid(false);
			}
		}
	}
}

void SampleDialogSettingAccountAction::SetFutureMarketDataConnected(const bool connected)
{
	if(connected != m_futureMarketDataConnected)
	{
		m_futureMarketDataConnected = connected;

		SetStringValue(m_valueFutureMarketDataConnected, m_futureMarketDataConnected);
		if(!connected)
		{
			SetFutureMarketDataLogged(false);
		}
	}
}

void SampleDialogSettingAccountAction::SetFutureMarketDataLogged(const bool logged)
{
	if(logged != m_futureMarketDataLogged)
	{
		m_futureMarketDataLogged = logged;

		SetStringValue(m_valueFutureMarketDataLogged, m_futureMarketDataLogged);
		if(!logged)
		{
			if(!m_securityMarketData || m_securityMarketData->isFuture())
			{
				SetSecurityMarketData(nullptr);
				SetMarketDataStockLoaded(false);
				SetMarketDataStockInvalid(false);
			}
		}
	}
}
#endif

void SampleDialogSettingAccountAction::ProcessConnectDisconnect(const Connection* connection)
{
	if(connection == TD_GetMarketSorterConnection())
	{
		SetMarketSorterConnected(TD_IsMarketSorterConnected());
	}
	else if(connection == TD_GetMarketDataConnection())
	{
		SetStockMarketDataConnected(TD_IsMarketDataConnected());
	}
#ifndef TAKION_NO_OPTIONS
	else if(connection == TD_GetOptionMarketDataConnection())
	{
		SetOptionMarketDataConnected(TD_IsOptionMarketDataConnected());
	}
	else if(connection == TD_GetFutureMarketDataConnection())
	{
		SetFutureMarketDataConnected(TD_IsFutureMarketDataConnected());
	}
#endif
}
/*
void SampleDialogSettingAccountAction::SecurityRefreshedMarketSorter()
{
}

void SampleDialogSettingAccountAction::SecurityRefreshedMarketData()
{
}
*/
void SampleDialogSettingAccountAction::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case M_RESP_MD_LOGON:
		{
			const TMsgMdLogonResponse* msg = (const TMsgMdLogonResponse*)message;
			switch(msg->m_requestId)
			{
				case MDLR_MARKET_DATA_STOCK:
				SetStockMarketDataLogged(TD_IsMarketDataLogged());
				break;

				case MDLR_MARKET_SORTER_STOCK:
				SetMarketSorterLogged(TD_IsMarketSorterLogged());
				break;

#ifndef TAKION_NO_OPTIONS
				case MDLR_MARKET_DATA_OPTION:
				SetOptionMarketDataLogged(TD_IsOptionMarketDataLogged());
				break;

				case MDLR_MARKET_DATA_FUTURE:
				SetFutureMarketDataLogged(TD_IsFutureMarketDataLogged());
				break;
#endif
			}
		}
		break;

		case TM_MARKET_SORTER_LOADED:
		SetMarketSorterAllStocksLoaded(TD_IsMarketSorterLoaded());
		break;

		case TM_MARKET_SORTER_INDEXES_LOADED:
		SetMarketSorterAllIndicesLoaded(TD_IsMarketSorterIndexesLoaded());
		break;

		case TM_MARKET_SORTER_STOCK_CHARTS_LOADED:
		SetMarketSorterAllStockChartsLoaded(TD_IsMarketSorterStockChartsLoaded());
		break;

		case TM_MARKET_SORTER_INDEX_CHARTS_LOADED:
		SetMarketSorterAllIndexChartsLoaded(TD_IsMarketSorterIndexChartsLoaded());
		break;

		case TM_EQUITY_NEW_MINUTE:
		break;

		case TM_EQUITY_CHART_LOADED:
		break;

		case TM_EQUITY_REFRESHED:
		SetMarketSorterStockLoaded(true);
		break;

		case TM_STOCK_REFRESHED:
#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_REFRESHED:
		case TM_FUTURE_REFRESHED:
#endif
		SetMarketDataStockLoaded(true);
//		SecurityRefreshedMarketData();
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_INVALID:
		case TM_FUTURE_INVALID:
#endif
		case TM_STOCK_INVALID:
		if(from == m_securityMarketData)
		{
			SetSecurityMarketData(nullptr);
		}
		break;

		case TM_SORTABLE_SECURITY_INVALID:
		if(from == m_securityMarketSorter)
		{
			SetSecurityMarketSorter(nullptr);
//			SetMarketSorterStockInvalid(true);
		}
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_FUTURE_DELETE:
#endif
		case TM_STOCK_DELETE:
		if(from == m_securityMarketData)
		{
			SetSecurityMarketData(nullptr);
		}
		break;

		case TM_SORTABLE_SECURITY_DELETE:
		if(from == m_securityMarketData)
		{
			SetSecurityMarketSorter(nullptr);
		}
		break;

		case TM_POSITION_DELETE:
		if(from == m_positionMarketData)
		{
			SetPositionMarketData(nullptr);
		}
		else if(from == m_positionMarketSorter)
		{
			SetPositionMarketSorter(nullptr);
		}
		break;

		case TM_NEW_EQUITY:
		case TM_REFRESHED_EQUITY:
		{
			const Security* security = ((const TMsgSecurity*)message)->m_security;
			if(!strcmp(m_currentSymbolMarketSorter.c_str(), security->GetSymbol()))
			{
				SetSecurityMarketSorter(security);
			}
		}
		break;

		case TM_NEW_POSITION:
		{
			const Position* position = ((TMsgNewPosition*)message)->m_position;
			if(m_account == position->GetAccount())
			{
#ifndef TAKION_NO_OPTIONS
				if(position->isOption())
				{
					if(!strcmp(m_symbolMarketData.c_str(), position->GetSymbol()))
					{
						SetPositionMarketData(position);
					}
				}
				else if(position->isFuture())
				{
					if(!strcmp(m_symbolMarketData.c_str(), position->GetSymbol()))
					{
						SetPositionMarketData(position);
					}
				}
				else
				{
#endif
					if(!strcmp(m_symbolMarketSorter.c_str(), position->GetSymbol()))
					{
						SetPositionMarketSorter(position);
					}
					if(!strcmp(m_symbolMarketData.c_str(), position->GetSymbol()))
					{
						SetPositionMarketData(position);
					}
#ifndef TAKION_NO_OPTIONS
				}
#endif
			}
		}
		break;

		case TM_UPDATED_POSITION:
		if(from == m_positionMarketSorter)
		{
			SetMarketSorterPositionSize(m_positionMarketSorter->GetSize(), false);
		}
		else if(from == m_positionMarketData)
		{
			SetMarketDataPositionSize(m_positionMarketData->GetSize(), false);
		}
		break;

		default:
		BaseDialogSettingAccount::Notify(message, from, info);
		break;
	}
}

BOOL SampleDialogSettingAccountAction::OnInitDialog()
{
	m_resizeInitialized = false;
	SetMarketSorterConnected(TD_IsMarketSorterConnected());
	SetMarketSorterLogged(TD_IsMarketSorterLogged());
	SetMarketSorterAllStocksLoaded(TD_IsMarketSorterLoaded());
	SetMarketSorterAllIndicesLoaded(TD_IsMarketSorterIndexesLoaded());
	SetMarketSorterAllStockChartsLoaded(TD_IsMarketSorterStockChartsLoaded());
	SetMarketSorterAllIndexChartsLoaded(TD_IsMarketSorterIndexChartsLoaded());

	SetStockMarketDataConnected(TD_IsMarketDataConnected());
	SetStockMarketDataLogged(TD_IsMarketDataLogged());

#ifndef TAKION_NO_OPTIONS
	m_optionMode = false;
	m_optionSuffix = '\0';
	m_optionCall = false;
	m_expirationYear = m_expirationMonth = m_expirationDay = 0;
	m_strikePrice.SetZero();

	m_currentOptionMode = false;
	m_currentOptionSuffix = '\0';
	m_currentOptionCall = false;
	m_currentExpirationYear = m_currentExpirationMonth = m_currentExpirationDay = 0;
	m_currentStrikePrice.SetZero();

	m_RadioOptionCall.SetInvalid(true);
	m_RadioOptionPut.SetInvalid(true);

	SetOptionMarketDataConnected(TD_IsOptionMarketDataConnected());
	SetOptionMarketDataLogged(TD_IsOptionMarketDataLogged());

	SetFutureMarketDataConnected(TD_IsFutureMarketDataConnected());
	SetFutureMarketDataLogged(TD_IsFutureMarketDataLogged());
#endif
	ResetStrings();

	m_symbolMarketSorter.clear();
	m_currentSymbolMarketSorter.clear();

	m_symbolMarketData.clear();
	m_currentSymbolMarketData.clear();
	m_realSymbolMarketData.clear();

	m_marketSorterStockInvalid = false;
	m_marketDataStockInvalid = false;
//Done in ResetStrings
//	SetMarketSorterPositionSize(0);
//	SetMarketDataPositionSize(0);

	TD_GetNewEquityObservable().AddObserver(this);
	TD_GetNewPositionObservable().AddObserver(this);

	BOOL ret = BaseDialogSettingAccount::OnInitDialog();

	m_EditSymbolMarketSorter.SetLimitText(7);
	m_EditSymbolMarketData.SetLimitText(7);

#ifndef TAKION_NO_OPTIONS
	m_CheckBoxOption.ResizeToFitText(0);
	m_RadioOptionCall.ResizeToFitText(0);
	m_RadioOptionPut.ResizeToFitText(0);

	m_ComboBoxOptionSuffix.AddString(nullptr);
	m_ComboBoxOptionSuffix.FillCharRange('A', 'Z', false);
	m_ComboBoxOptionSuffix.FillCharRange('0', '9', false);
	m_ComboBoxOptionSuffix.SetCurSel(0);
	
	m_RadioOptionPut.SetCheck(BST_CHECKED);

	m_ComboBoxOptionSuffix.SetInvalid(true);

	UpdateStrikePriceColor();
//	m_SpinOptionStrikePrice.SetBkColorEnabled(m_visualBase->GetGrayColor());

//	UpdateExpirationColor();
	MakeExpirationCurrent();
//	m_DateTimeOptionExpiration.SetBkColor(m_visualBase->GetGrayColor());

	UpdateOptionExpirationValue();
#endif
	return ret;
}

void SampleDialogSettingAccountAction::BeforeDestroy()
{
	m_resizeInitialized = false;

	m_marketSorterAllStocksLoaded = false;
	m_marketSorterAllIndicesLoaded = false;
	m_marketSorterAllStockChartsLoaded = false;
	m_marketSorterAllIndexChartsLoaded = false;
	m_marketSorterConnected = false;
	m_marketSorterLogged = false;
	m_marketSorterStockLoaded = false;
	m_marketSorterStockInvalid = false;

#ifndef TAKION_NO_OPTIONS
	m_optionMarketDataConnected = false;
	m_optionMarketDataLogged = false;

	m_futureMarketDataConnected = false;
	m_futureMarketDataLogged = false;

	m_optionMode = false;
	m_optionSuffix = '\0';
	m_optionCall = false;
	m_expirationYear = m_expirationMonth = m_expirationDay = 0;
	m_strikePrice.SetZero();

	m_currentOptionMode = false;
	m_currentOptionSuffix = '\0';
	m_currentOptionCall = false;
	m_currentExpirationYear = m_currentExpirationMonth = m_currentExpirationDay = 0;
	m_currentStrikePrice.SetZero();

	m_RadioOptionCall.SetInvalid(true);
	m_RadioOptionPut.SetInvalid(true);
#endif
	m_stockMarketDataConnected = false;
	m_stockMarketDataLogged = false;

	m_marketDataStockLoaded = false;
	m_marketDataStockInvalid = false;

//	if(m_security)
//	SecurityFailed();

//	SetPosition(nullptr);
	m_currentSymbolMarketSorter.clear();
	m_symbolMarketSorter.clear();
	SetSecurityMarketSorter(nullptr);
	SetPositionMarketSorter(nullptr);

	m_currentSymbolMarketData.clear();
	m_symbolMarketData.clear();
	m_realSymbolMarketData.clear();
	SetSecurityMarketData(nullptr);
	SetPositionMarketData(nullptr);

	TD_GetNewEquityObservable().RemoveObserver(this);
	TD_GetNewPositionObservable().RemoveObserver(this);

	ResetStrings();

	BaseDialogSettingAccount::BeforeDestroy();
}

void SetLabelAndValueRects(StringArea& labelString, StringArea& valueString, CRect& rect, const int stringVOffset, const int textHeight, const int labelLeft, const int labelRight, const int valueLeft, const int valueRight)
{
	rect.top += stringVOffset;
	rect.bottom = rect.top + textHeight;
	rect.left = labelLeft;
	rect.right = labelRight;
	labelString.SetRect(rect);
	rect.left = valueLeft;
	rect.right = valueRight;
	valueString.SetRect(rect);
}

void SampleDialogSettingAccountAction::Resize(bool widthChanged, bool heightChanged)
{
	BaseDialogSettingAccount::Resize(widthChanged, heightChanged);

	if(!m_resizeInitialized && m_ButtonSetAllPhantomPositionsFlat.m_hWnd)
	{
		m_resizeInitialized = true;
		CRect rect;
		m_ButtonSetAllPhantomPositionsFlat.GetWindowRect(&rect);
		ScreenToClient(&rect);

		const int groupTop = rect.bottom + vgap * 8;
		const int groupRight = rect.right;
		rect.left = 0;
		rect.top = groupTop;// rect.bottom + vgap * 8;

		const int vOffset = vgap * 2;
		const int hOffsetHalf = hgap * 2;
		const int hOffset = hgap * 4;

		int labelLeft = rect.left + hOffset;
		int valueRight = groupRight - hOffset;
		int valueWidth = (valueRight - labelLeft) / 8 - hOffset;
		int valueLeft = valueRight - valueWidth;
		int labelRight = valueLeft - hOffset - hOffset;

		const int textHeight = m_groupBoxAccountReady.GetTextHeight();
		const int stringVOffset = textHeight + vOffset;

		const int right = labelLeft + m_labelAccountReady.GetTextWidth() + hOffset;
		SetLabelAndValueRects(m_labelAccountReady, m_valueAccountReady, rect, stringVOffset, textHeight, labelLeft, right, right, valueRight);

		SetLabelAndValueRects(m_labelHistoryLoaded, m_valueHistoryLoaded, rect, stringVOffset + vOffset * 3, textHeight, labelLeft, labelRight, valueLeft, valueRight);

		SetLabelAndValueRects(m_labelUntradeableStatusLoaded, m_valueUntradeableStatusLoaded, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelRestrictedSymbolListLoaded, m_valueRestrictedSymbolListLoaded, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);

		SetLabelAndValueRects(m_labelTradingAllowed, m_valueTradingAllowed, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelSuspended, m_valueSuspended, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelExecutorConnected, m_valueExecutorConnected, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelExecutorLogged, m_valueExecutorLogged, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
#ifndef TAKION_NO_OPTIONS
		SetLabelAndValueRects(m_labelFutureExecutorConnected, m_valueFutureExecutorConnected, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelFutureExecutorLogged, m_valueFutureExecutorLogged, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
#endif
		SetLabelAndValueRects(m_labelTraderIdEnabled, m_valueTraderIdEnabled, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelTradingLockedByTrader, m_valueTradingLockedByTrader, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);

		SetLabelAndValueRects(m_labelTradingDefaultAccountOnlyByTrader, m_valueTradingDefaultAccountOnlyByTrader, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelTradingTestStocksOnlyByTrader, m_valueTradingTestStocksOnlyByTrader, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);

		rect.left = 0;
		rect.right = groupRight;
		rect.top = groupTop;// rect.bottom + vgap * 8;
		rect.bottom += vOffset * 2;// vgap * 4;
		m_groupBoxAccountReady.SetRect(rect);

//MarketSorter
		const int groupWidth = rect.Width();

		const int groupLeftMs = groupRight + hgap * 8;
		rect.left = groupLeftMs;
		rect.right = rect.left + groupWidth;
		rect.top = groupTop;

		const int groupRightMs = rect.right;

		labelLeft = rect.left + hOffset;
		valueRight = groupRightMs - hOffset;
		valueWidth = (valueRight - labelLeft) / 8 - hOffset;
		valueLeft = valueRight - valueWidth;
		labelRight = valueLeft - hOffset - hOffset;

		SetLabelAndValueRects(m_labelMarketSorterAllStocksLoaded, m_valueMarketSorterAllStocksLoaded, rect, stringVOffset + vOffset * 3, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelMarketSorterAllIndicesLoaded, m_valueMarketSorterAllIndicesLoaded, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelMarketSorterAllStockChartsLoaded, m_valueMarketSorterAllStockChartsLoaded, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelMarketSorterAllIndexChartsLoaded, m_valueMarketSorterAllIndexChartsLoaded, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);

		SetLabelAndValueRects(m_labelMarketSorterConnected, m_valueMarketSorterConnected, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelMarketSorterLogged, m_valueMarketSorterLogged, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);

		CRect r;
		m_StaticSymbolMarketSorter.GetWindowRect(&r);
		const int stHeight = r.Height();

		m_EditSymbolMarketSorter.GetWindowRect(&r);

		int controlWidth = r.Width();
		int controlHeight = r.Height();
		CRect symbolRect(rect);
		symbolRect.top += stringVOffset;
		symbolRect.bottom = symbolRect.top + controlHeight;
		symbolRect.left = labelLeft;
		symbolRect.right = symbolRect.left + controlWidth;
		m_StaticSymbolMarketSorter.SetWindowPos(nullptr, symbolRect.left, symbolRect.top, controlWidth, stHeight, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		symbolRect.top += stHeight;
		symbolRect.bottom += stHeight;
		m_EditSymbolMarketSorter.SetWindowPos(nullptr, symbolRect.left, symbolRect.top, controlWidth, controlHeight, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		symbolRect.left += controlWidth + hgap;
		m_ButtonSubscribeMarketSorter.GetWindowRect(&r);
		controlWidth = r.Width();
//		controlHeight = r.Height();
		symbolRect.right = symbolRect.left + controlWidth;
		m_ButtonSubscribeMarketSorter.SetWindowPos(nullptr, symbolRect.left, symbolRect.top, controlWidth, controlHeight, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		symbolRect.left += controlWidth + hgap;
		m_ButtonResetMarketSorter.GetWindowRect(&r);
		controlWidth = r.Width();
		symbolRect.right = symbolRect.left + controlWidth;
		m_ButtonResetMarketSorter.SetWindowPos(nullptr, symbolRect.left, symbolRect.top, controlWidth, controlHeight, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		rect.top = symbolRect.bottom + vOffset;

		rect.left = labelLeft;
		rect.bottom = rect.top + textHeight;
		rect.right = valueRight;
		m_valueMarketSorterCurrentStock.SetRect(rect);

		SetLabelAndValueRects(m_labelMarketSorterStockLoaded, m_valueMarketSorterStockLoaded, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelMarketSorterStockInvalid, m_valueMarketSorterStockInvalid, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelMarketSorterPositionSize, m_valueMarketSorterPositionSize, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);

//MarketSorter Group
		rect.left = groupLeftMs;
		rect.right = groupRightMs;// rect.left + groupWidth;
		rect.top = groupTop;
		rect.bottom += vOffset * 2;// vgap * 4;
		m_groupBoxMarketSorterReady.SetRect(rect);

//MarketData
		const int groupLeftMd = groupRightMs + hgap * 8;
		rect.left = groupLeftMd;
		rect.right = rect.left + groupWidth;
		rect.top = groupTop;

		const int groupRightMd = rect.right;

		labelLeft = rect.left + hOffset;
		valueRight = groupRightMd - hOffset;
		valueWidth = (valueRight - labelLeft) / 8 - hOffset;
		valueLeft = valueRight - valueWidth;
		labelRight = valueLeft - hOffset - hOffset;

		SetLabelAndValueRects(m_labelStockMarketDataConnected, m_valueStockMarketDataConnected, rect, stringVOffset + vOffset * 3, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelStockMarketDataLogged, m_valueStockMarketDataLogged, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);

#ifndef TAKION_NO_OPTIONS
//OptionMarketData
		SetLabelAndValueRects(m_labelOptionMarketDataConnected, m_valueOptionMarketDataConnected, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelOptionMarketDataLogged, m_valueOptionMarketDataLogged, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
//FutureMarketData
		SetLabelAndValueRects(m_labelFutureMarketDataConnected, m_valueFutureMarketDataConnected, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelFutureMarketDataLogged, m_valueFutureMarketDataLogged, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
#endif

		m_EditSymbolMarketData.GetWindowRect(&r);

		controlWidth = r.Width();
		controlHeight = r.Height();
		symbolRect = rect;
		symbolRect.top += stringVOffset;
		symbolRect.bottom = symbolRect.top + controlHeight;
		symbolRect.left = labelLeft;
		symbolRect.right = symbolRect.left + controlWidth;
		m_StaticSymbolMarketData.SetWindowPos(nullptr, symbolRect.left, symbolRect.top, controlWidth, stHeight, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		symbolRect.top += stHeight;
		symbolRect.bottom += stHeight;
		m_EditSymbolMarketData.SetWindowPos(nullptr, symbolRect.left, symbolRect.top, controlWidth, controlHeight, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		symbolRect.left += controlWidth + hgap;
		m_ButtonSubscribeMarketData.GetWindowRect(&r);
		controlWidth = r.Width();
//		controlHeight = r.Height();
		symbolRect.right = symbolRect.left + controlWidth;
		m_ButtonSubscribeMarketData.SetWindowPos(nullptr, symbolRect.left, symbolRect.top, controlWidth, controlHeight, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		symbolRect.left += controlWidth + hgap;
		m_ButtonResetMarketData.GetWindowRect(&r);
		controlWidth = r.Width();
		symbolRect.right = symbolRect.left + controlWidth;
		m_ButtonResetMarketData.SetWindowPos(nullptr, symbolRect.left, symbolRect.top, controlWidth, controlHeight, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		rect.top = symbolRect.bottom + vOffset;
		rect.left = labelLeft;
		rect.bottom = rect.top + textHeight;
		rect.right = valueRight;
		m_valueMarketDataCurrentStock.SetRect(rect);

#ifndef TAKION_NO_OPTIONS
		m_CheckBoxOption.GetWindowRect(&r);
		ScreenToClient(&r);

		const int moveX = labelLeft - r.left;

		const int optionTop = rect.bottom + vOffset * 2;
		const int moveY = optionTop - r.top;

		MoveControl(m_CheckBoxOption, moveX, moveY);
		MoveControl(m_RadioOptionCall, moveX, moveY);
		MoveControl(m_RadioOptionPut, moveX, moveY);
		MoveControl(m_ComboBoxOptionSuffix, moveX, moveY);
		MoveControl(m_StaticOptionSuffix, moveX, moveY);
		MoveControl(m_StaticOptionStrikePrice, moveX, moveY);
		MoveControl(m_SpinOptionStrikePrice, moveX, moveY);
		MoveControl(m_StaticOptionExpiration, moveX, moveY);
		MoveControl(m_DateTimeOptionExpiration, moveX, moveY);

		m_DateTimeOptionExpiration.GetWindowRect(&r);
		ScreenToClient(&r);

		rect.left = r.left - hOffsetHalf;
		rect.right = r.right + hOffsetHalf;// rect.left + groupWidth;
		rect.top = optionTop - vOffset;
		rect.bottom = r.bottom + vOffset;// vgap * 4;
		m_groupBoxOption.SetRect(rect);

		rect.top = r.bottom + vOffset * 2;
#endif

		SetLabelAndValueRects(m_labelMarketDataStockLoaded, m_valueMarketDataStockLoaded, rect, vOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelMarketDataStockInvalid, m_valueMarketDataStockInvalid, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);
		SetLabelAndValueRects(m_labelMarketDataPositionSize, m_valueMarketDataPositionSize, rect, stringVOffset, textHeight, labelLeft, labelRight, valueLeft, valueRight);

//MarketData Group
		rect.left = groupLeftMd;
		rect.right = groupRightMd;// rect.left + groupWidth;
		rect.top = groupTop;
		rect.bottom += vOffset * 2;// vgap * 4;
		m_groupBoxMarketDataReady.SetRect(rect);
	}
}

void SampleDialogSettingAccountAction::SetStringValueNo(StringArea& stringArea) const
{
	SetStringValueNoStatic(stringArea, m_colorRed);
}

void SampleDialogSettingAccountAction::SetStringValueNoStatic(StringArea& stringArea, COLORREF color)
{
	stringArea.SetTextInRect("No");
	stringArea.SetFgColor(color);
}

void SampleDialogSettingAccountAction::SetStringValueStatic(StringArea& stringArea, bool yes, COLORREF colorYes, COLORREF colorNo)
{
	if(yes)
	{
		stringArea.SetTextInRect("Yes");
		stringArea.SetFgColor(colorYes);
	}
	else
	{
		SetStringValueNoStatic(stringArea, colorNo);
	}
}

void SampleDialogSettingAccountAction::SetStringValue(StringArea& stringArea, bool yes) const
{
	SetStringValueStatic(stringArea, yes, m_colorGreen, m_colorRed);
}

void SampleDialogSettingAccountAction::AccountChanged()
{
	if(m_account)
	{
		UpdateAccountReadyToTrade();
//Components of isReadyToTrade
		SetStringValue(m_valueTradingAllowed, m_tradingAllowed);
		SetStringValueStatic(m_valueSuspended, m_suspended, m_colorRed, m_colorGreen);
		SetStringValue(m_valueHistoryLoaded, m_historyLoaded);// m_account->isHistoryLoaded());
		SetStringValue(m_valueUntradeableStatusLoaded, m_untradeableStatusLoaded);
		SetStringValue(m_valueRestrictedSymbolListLoaded, m_restrictedSymbolListLoaded);
		if(m_securityMarketSorter)
		{
			SetPositionMarketSorter(m_account->FindPositionWithLock(m_securityMarketSorter));
		}
		if(m_securityMarketData)
		{
			SetPositionMarketData(m_account->FindPositionWithLock(m_securityMarketData));
		}
	}
	else
	{
		ResetStrings();
		SetPositionMarketSorter(nullptr);
		SetPositionMarketData(nullptr);
	}
}

void SampleDialogSettingAccountAction::UpdateAccountReadyToTrade()
{
	SetStringValue(m_valueAccountReady, m_account != nullptr && m_account->isReadyToTrade() && (!m_tradingDefaultAccountOnlyByTrader || m_account == TD_GetDefaultAccount()
#ifndef TAKION_NO_OPTIONS
		|| m_account == TD_GetFutureDefaultAccount()
#endif
		));
}

void SampleDialogSettingAccountAction::OnTradingAllowed()
{
	SetStringValue(m_valueTradingAllowed, m_tradingAllowed);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnSuspended()
{
	SetStringValueStatic(m_valueSuspended, m_suspended, m_colorRed, m_colorGreen);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnHistoryLoaded()
{
	SetStringValue(m_valueHistoryLoaded, m_historyLoaded);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnUntradeableStatusLoaded()
{
	SetStringValue(m_valueUntradeableStatusLoaded, m_untradeableStatusLoaded);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnRestrictedSymbolListLoaded()
{
	SetStringValue(m_valueRestrictedSymbolListLoaded, m_restrictedSymbolListLoaded);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnExecutorConnected()
{
	SetStringValue(m_valueExecutorConnected, m_executorConnected);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnExecutorLogged()
{
	SetStringValue(m_valueExecutorLogged, m_executorLogged);
	UpdateAccountReadyToTrade();
	if(!m_executorLogged)
	{
		SetPositionMarketSorter(nullptr);
		SetPositionMarketData(nullptr);
	}
}

#ifndef TAKION_NO_OPTIONS
void SampleDialogSettingAccountAction::OnFutureExecutorConnected()
{
	SetStringValue(m_valueFutureExecutorConnected, m_futureExecutorConnected);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnFutureExecutorLogged()
{
	SetStringValue(m_valueFutureExecutorLogged, m_futureExecutorLogged);
	UpdateAccountReadyToTrade();
}
#endif

void SampleDialogSettingAccountAction::OnTraderIdEnabledChanged()
{
	SetStringValue(m_valueTraderIdEnabled, m_traderIdEnabled);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnTradingLockedByTrader()
{
//Color Opposite
	SetStringValueStatic(m_valueTradingLockedByTrader, m_tradingLockedByTrader, m_colorRed, m_colorGreen);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnTradingDefaultAccountOnlyByTrader()
{
	SetStringValueStatic(m_valueTradingDefaultAccountOnlyByTrader, m_tradingDefaultAccountOnlyByTrader, m_colorRed, m_colorGreen);
	UpdateAccountReadyToTrade();
}

void SampleDialogSettingAccountAction::OnTradingTestStocksOnlyByTrader()
{
	SetStringValueStatic(m_valueTradingTestStocksOnlyByTrader, m_tradingTestStocksOnlyByTrader, m_colorRed, m_colorGreen);
//	UpdateAccountReadyToTrade();
}

StringSetting* SampleDialogSettingAccountAction::GetAccountIdSetting()
{
	return &extensionConfig->m_accountActionAccountId;
}


void SampleDialogSettingAccountAction::RestoreLayout()
{
	BaseDialogSettingAccount::RestoreLayout();
	m_EditSymbolMarketSorter.SetWindowText(extensionConfig->m_accountActionMsSymbol.GetValue().c_str());
	EnableSubscribeMarketSorterButton();
	OnSubscribeMarketSorter();

	m_EditSymbolMarketData.SetWindowText(extensionConfig->m_accountActionMdSymbol.GetValue().c_str());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_accountActionOption.GetValue(),
		extensionConfig->m_accountActionOptionSuffix.GetValue(),
		extensionConfig->m_accountActionOptionCall.GetValue(),
		extensionConfig->m_accountActionOptionStrikePrice.GetValue(),
		extensionConfig->m_accountActionOptionExpiration.GetValue());
#endif
	OnSubscribeMarketData();
	EnableSubscribeMarketDataButton();

	m_modified = false;
}

void SampleDialogSettingAccountAction::UpdateSettings()
{
	BaseDialogSettingAccount::UpdateSettings();
	extensionConfig->m_accountActionMsSymbol.SetValue(m_currentSymbolMarketSorter);
	extensionConfig->m_accountActionMdSymbol.SetValue(m_currentSymbolMarketData);

#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_accountActionOption.SetValue(m_currentOptionMode);
	extensionConfig->m_accountActionOptionCall.SetValue(m_currentOptionCall);
	extensionConfig->m_accountActionOptionSuffix.SetValue(m_currentOptionSuffix);
	extensionConfig->m_accountActionOptionStrikePrice.SetValue(m_currentStrikePrice);
	extensionConfig->m_accountActionOptionExpiration.SetValue(GetCurrentExpirationDate());
#endif
}

void SampleDialogSettingAccountAction::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingAccount::DoDataExchange(pDX);
	DDX_Control(pDX, buttonDestroyUntradedPositionsId, m_ButtonDestroyUntradedPositions);
	DDX_Control(pDX, buttonSetPhantomPositionsFlatId, m_ButtonSetPhantomPositionsFlat);
	DDX_Control(pDX, buttonDestroyAllUntradedPositionsId, m_ButtonDestroyAllUntradedPositions);
	DDX_Control(pDX, buttonSetAllPhantomPositionsFlatId, m_ButtonSetAllPhantomPositionsFlat);

	DDX_Control(pDX, staticSymbolMarketSorterId, m_StaticSymbolMarketSorter);
	DDX_Control(pDX, editSymbolMarketSorterId, m_EditSymbolMarketSorter);
	DDX_Control(pDX, buttonSubscribeMarketSorterId, m_ButtonSubscribeMarketSorter);
	DDX_Control(pDX, buttonResetMarketSorterId, m_ButtonResetMarketSorter);

	DDX_Control(pDX, staticSymbolMarketDataId, m_StaticSymbolMarketData);
	DDX_Control(pDX, editSymbolMarketDataId, m_EditSymbolMarketData);
	DDX_Control(pDX, buttonSubscribeMarketDataId, m_ButtonSubscribeMarketData);
	DDX_Control(pDX, buttonResetMarketDataId, m_ButtonResetMarketData);

#ifndef TAKION_NO_OPTIONS
	DDX_Control(pDX, checkBoxMarketDataOptionId, m_CheckBoxOption);
	DDX_Control(pDX, radioMarketDataOptionCallId, m_RadioOptionCall);
	DDX_Control(pDX, radioMarketDataOptionPutId, m_RadioOptionPut);
	DDX_Control(pDX, comboBoxMarketDataOptionSuffixId, m_ComboBoxOptionSuffix);
	DDX_Control(pDX, staticMarketDataOptionSuffixId, m_StaticOptionSuffix);
	DDX_Control(pDX, staticMarketDataOptionStrikePriceId, m_StaticOptionStrikePrice);
	DDX_Control(pDX, spinMarketDataOptionStrikePriceId, m_SpinOptionStrikePrice);
	DDX_Control(pDX, staticMarketDataOptionExpirationId, m_StaticOptionExpiration);
	DDX_Control(pDX, datetimeMarketDataOptionExpirationId, m_DateTimeOptionExpiration);
#endif
}

BEGIN_MESSAGE_MAP(SampleDialogSettingAccountAction, BaseDialogSettingAccount)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(buttonDestroyUntradedPositionsId, OnDestroyUntradedPositions)
	ON_BN_CLICKED(buttonSetPhantomPositionsFlatId, OnSetPhantomPositionsFlat)
	ON_BN_CLICKED(buttonDestroyAllUntradedPositionsId, OnDestroyAllUntradedPositions)
	ON_BN_CLICKED(buttonSetAllPhantomPositionsFlatId, OnSetAllPhantomPositionsFlat)
	ON_EN_CHANGE(editSymbolMarketSorterId, OnSymbolMarketSorterChange)
	ON_EN_UPDATE(editSymbolMarketSorterId, OnSymbolMarketSorterUpdate)
	ON_BN_CLICKED(buttonSubscribeMarketSorterId, OnSubscribeMarketSorter)
	ON_BN_CLICKED(buttonResetMarketSorterId, OnResetMarketSorter)
	ON_EN_CHANGE(editSymbolMarketDataId, OnSymbolMarketDataChange)
	ON_EN_UPDATE(editSymbolMarketDataId, OnSymbolMarketDataUpdate)
	ON_BN_CLICKED(buttonSubscribeMarketDataId, OnSubscribeMarketData)
	ON_BN_CLICKED(buttonResetMarketDataId, OnResetMarketData)
#ifndef TAKION_NO_OPTIONS
	ON_BN_CLICKED(checkBoxMarketDataOptionId, OnOption)
	ON_BN_CLICKED(radioMarketDataOptionCallId, OnOptionCall)
	ON_BN_CLICKED(radioMarketDataOptionPutId, OnOptionPut)
	ON_CBN_SELCHANGE(comboBoxMarketDataOptionSuffixId, OnSelchangeOptionSuffix)
	ON_NOTIFY(DTN_DROPDOWN, datetimeMarketDataOptionExpirationId, OnCalendarDropdown)
	ON_NOTIFY(DTN_CLOSEUP, datetimeMarketDataOptionExpirationId, OnCalendarCloseup)
	ON_NOTIFY(DTN_DATETIMECHANGE, datetimeMarketDataOptionExpirationId, OnExpirationDateChange)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
#endif
END_MESSAGE_MAP()

void SampleDialogSettingAccountAction::OnSymbolMarketDataUpdate()
{
	m_EditSymbolMarketData.TryUpdate();
}

bool SampleDialogSettingAccountAction::MarketDataSymbolDifferent() const
{
	return m_currentSymbolMarketData != m_symbolMarketData
#ifndef TAKION_NO_OPTIONS
		|| m_currentOptionMode != m_optionMode
		|| m_currentOptionSuffix != m_optionSuffix
		|| m_currentOptionCall != m_optionCall
		|| m_currentExpirationYear != m_expirationYear
		|| m_currentExpirationMonth != m_expirationMonth
		|| m_currentExpirationDay != m_expirationDay
		|| m_currentStrikePrice != m_strikePrice
#endif
		;
}

bool SampleDialogSettingAccountAction::MarketDataSymbolValid() const
{
	const unsigned int len = (unsigned int)m_symbolMarketData.length();
	return (len != 1 || m_symbolMarketData[0] != '/')
#ifndef TAKION_NO_OPTIONS
		&& (!m_optionMode || !len || m_symbolMarketData[0] != '/' && AreOptionSettingsValid())
#endif
		;
}

bool SampleDialogSettingAccountAction::CanSubscribeMarketData() const
{
	return MarketDataSymbolDifferent() && MarketDataSymbolValid();
}

void SampleDialogSettingAccountAction::EnableSubscribeMarketDataButton()
{
	const bool different = MarketDataSymbolDifferent();
	EnableControl(m_ButtonSubscribeMarketData.m_hWnd, different && MarketDataSymbolValid(), m_EditSymbolMarketData.m_hWnd);
//	EnableControl(m_ButtonSubscribeMarketData.m_hWnd, CanSubscribeMarketData(), m_EditSymbolMarketData.m_hWnd);
	EnableControl(m_ButtonResetMarketData.m_hWnd, different, m_EditSymbolMarketData.m_hWnd);
}

void SampleDialogSettingAccountAction::OnResetMarketData()
{
	ResetMarketDataSymbolCurrent();
	EnableSubscribeMarketDataButton();
}

void SampleDialogSettingAccountAction::OnSubscribeMarketData()
{
	DoSubscribeMarketData();
//	EnableSubscribeButton();
}

bool SampleDialogSettingAccountAction::SetSecurityMarketData(const Security* security)
{
	if(security != m_securityMarketData)
	{
		if(m_securityMarketData)
		{
			m_securityMarketData->RemoveObserver(this);
#ifndef TAKION_NO_OPTIONS
			m_securityMarketData->RemoveOptionLevel2Observer(this);
#endif
		}
		m_securityMarketData = security;
		if(m_securityMarketData)
		{
			m_securityMarketData->AddObserver(this);
#ifndef TAKION_NO_OPTIONS
			m_securityMarketData->AddOptionLevel2Observer(this);
#endif

			m_securityMarketData->Subscribe();

//			m_roundLot = m_securityMarketData->GetRoundLot();
//			if(!m_roundLot)m_roundLot = 100;
			TD_ReleaseStock(m_securityMarketData);
//			m_unappliedMessageReader.SetLineBufferArray(m_security->GetUnappliedMessages());
			SetMarketDataStockLoaded(m_securityMarketData->isLoaded());
			SetMarketDataStockInvalid(false);

			if(m_account)
			{
				SetPositionMarketData(m_account->FindPositionWithLock(m_securityMarketData));
			}
		}
		else
		{
			SetMarketDataStockLoaded(false);
			SetMarketDataStockInvalid(!m_realSymbolMarketData.empty() && m_stockMarketDataLogged);
			SetPositionMarketData(nullptr);
		}
		return true;
	}
	return false;
}

bool SampleDialogSettingAccountAction::SetPositionMarketData(const Position* position)
{
	if(m_positionMarketData != position)
	{
		if(m_positionMarketData)
		{
			m_positionMarketData->RemoveObserver(this);
		}
		m_positionMarketData = position;
		if(m_positionMarketData)
		{
			m_positionMarketData->AddObserver(this);
			SetMarketDataPositionSize(m_positionMarketData->GetSize(), false);
		}
		else
		{
			SetMarketDataPositionSize(0, true);
		}
//		UpdatePosition();
		return true;
	}
	return false;
}

void SampleDialogSettingAccountAction::MakeMarketDataSymbolCurrent()
{
	m_currentSymbolMarketData = m_symbolMarketData;
#ifndef TAKION_NO_OPTIONS
	m_currentOptionMode = m_optionMode;
	m_currentOptionSuffix = m_optionSuffix;
	m_currentOptionCall = m_optionCall;

//	m_currentExpirationYear = m_expirationYear;
//	m_currentExpirationMonth = m_expirationMonth;
//	m_currentExpirationDay = m_expirationDay;
	MakeExpirationCurrent();
//	UpdateExpirationColor();

	m_currentStrikePrice = m_strikePrice;
	UpdateStrikePriceColor();
	UpdateCallPutBkColor();
#endif
}

void SampleDialogSettingAccountAction::ResetMarketDataSymbolCurrent()
{
//	m_symbolMarketData = m_currentSymbolMarketData;
	m_EditSymbolMarketData.SetWindowText(m_currentSymbolMarketData.c_str());
#ifndef TAKION_NO_OPTIONS
	SetOptionValues(m_currentOptionMode, m_currentOptionSuffix, m_currentOptionCall, m_currentStrikePrice, GetCurrentExpirationDate());
#endif
	EnableSubscribeMarketDataButton();
}

bool SampleDialogSettingAccountAction::DoSubscribeMarketData()
{
	const bool ret = CanSubscribeMarketData();
	MakeMarketDataSymbolCurrent();
	if(ret)
	{
		Security* security = nullptr;;

//		MakeMarketDataSymbolCurrent();
//		m_currentSymbolMarketData = m_symbolMarketData;

		m_realSymbolMarketData = m_symbolMarketData;

//		::InvalidateRect(m_EditSymbolMarketData.m_hWnd, nullptr, FALSE);
//		EnableSubscribeMarketDataButton();
		const char* symbol = m_currentSymbolMarketData.c_str();
		if(*symbol)
		{
#ifndef TAKION_NO_OPTIONS
			if(isOptionUsed())
			{
				unsigned __int64 rootSymbol;
				unsigned __int64 underlierSymbol;
				unsigned __int64 optionBlock;
				unsigned short optionType;
				const bool option = GetOptionValues(rootSymbol, underlierSymbol, optionBlock, optionType);
				if(option)
				{
//					if(CanSubscribeMarketData())
//					{
					m_realSymbolMarketData += ' ';
					TD_OptionBlockToString(m_realSymbolMarketData, optionBlock);
					security = TD_ObtainOptionByRootAndOptionBlock(rootSymbol, optionBlock, false);//When the underlier symbol is not known
//					}
				}
			}
			else if(*symbol == '/')
			{
				const unsigned __int64 currentFutureSymbol = TU_GetCurrentFutureSymbol(symbol + 1);
				if(currentFutureSymbol)
				{
					m_realSymbolMarketData = (const char*)&currentFutureSymbol;
//					if(CanSubscribeMarketData())
					security = TD_ObtainFutureByNumericSymbol(currentFutureSymbol, false);
				}
			}
			else if(TU_IsFutureSymbol(symbol))
			{
//				if(CanSubscribeMarketData())
				security = TD_ObtainFuture(symbol, false);
			}
			else
#endif
//			if(CanSubscribeMarketData())
			security = TD_ObtainStock(symbol, false);
		}
		SetSecurityMarketData(security);
		m_valueMarketDataCurrentStock.SetTextInRect(m_realSymbolMarketData.c_str());
	}
	::InvalidateRect(m_EditSymbolMarketData.m_hWnd, nullptr, FALSE);
	EnableSubscribeMarketDataButton();
	return ret;
}


void SampleDialogSettingAccountAction::OnSymbolMarketDataChange()
{
	const bool empty = m_symbolMarketData.empty();
	const bool symbolCurrent = m_symbolMarketData == m_currentSymbolMarketData;// m_securityMarketData && !strcmp(m_securityMarketData->GetSymbol(), m_symbolMarketData.c_str());
	CString text;
	m_EditSymbolMarketData.GetWindowText(text);
	m_symbolMarketData = text;
	if(m_symbolMarketData.empty() != empty || (m_symbolMarketData == m_currentSymbolMarketData) != symbolCurrent)//(m_securityMarketData && !strcmp(m_securityMarketData->GetSymbol(), m_symbol.c_str())) != symbolCurrent)
	{
		m_EditSymbolMarketData.Invalidate(FALSE);
	}
	EnableSubscribeMarketDataButton();
}


//MarketSorter
void SampleDialogSettingAccountAction::OnSymbolMarketSorterUpdate()
{
	m_EditSymbolMarketSorter.TryUpdate();
}

bool SampleDialogSettingAccountAction::MarketSorterSymbolDifferent() const
{
	return m_currentSymbolMarketSorter != m_symbolMarketSorter;
}

bool SampleDialogSettingAccountAction::MarketSorterSymbolValid() const
{
	const unsigned int len = (unsigned int)m_symbolMarketSorter.length();
	return !len || m_symbolMarketSorter[0] != '/';
}

bool SampleDialogSettingAccountAction::CanSubscribeMarketSorter() const
{
	return MarketSorterSymbolDifferent() && MarketSorterSymbolValid();
}

void SampleDialogSettingAccountAction::EnableSubscribeMarketSorterButton()
{
	const bool different = MarketSorterSymbolDifferent();
	EnableControl(m_ButtonSubscribeMarketSorter.m_hWnd, different && MarketSorterSymbolValid(), m_EditSymbolMarketSorter.m_hWnd);
	EnableControl(m_ButtonResetMarketSorter.m_hWnd, different, m_EditSymbolMarketSorter.m_hWnd);
}

void SampleDialogSettingAccountAction::OnResetMarketSorter()
{
	ResetMarketSorterSymbolCurrent();
	EnableSubscribeMarketSorterButton();
}

void SampleDialogSettingAccountAction::OnSubscribeMarketSorter()
{
	DoSubscribeMarketSorter();
//	EnableSubscribeButton();
}

bool SampleDialogSettingAccountAction::SetSecurityMarketSorter(const Security* security)
{
	if(security != m_securityMarketSorter)
	{
		if(m_securityMarketSorter)
		{
			m_securityMarketSorter->RemoveObserver(this);
		}
		m_securityMarketSorter = security;
		if(m_securityMarketSorter)
		{
			m_securityMarketSorter->AddObserver(this);
//#ifndef TAKION_NO_OPTIONS
//			m_security->AddOptionLevel2Observer(this);
//#endif

			m_securityMarketSorter->Subscribe();

//			m_StaticSymbolSubscribed.SetWindowText(m_security->GetSymbol());
//			m_roundLot = m_securityMarketSorter->GetRoundLot();
//			if(!m_roundLot)m_roundLot = 100;
			TD_ReleaseStock(m_securityMarketSorter);
//			m_unappliedMessageReader.SetLineBufferArray(m_security->GetUnappliedMessages());
			SetMarketSorterStockLoaded(m_securityMarketSorter->isLoaded());
			SetMarketSorterStockInvalid(false);

			if(m_account)
			{
				SetPositionMarketSorter(m_account->FindPositionWithLock(m_securityMarketSorter));
			}
		}
		else
		{
			SetMarketSorterStockLoaded(false);
			SetMarketSorterStockInvalid(!m_currentSymbolMarketSorter.empty() && m_marketSorterAllStocksLoaded);
			SetPositionMarketSorter(nullptr);
		}
		return true;
	}
	return false;
}

bool SampleDialogSettingAccountAction::SetPositionMarketSorter(const Position* position)
{
	if(m_positionMarketSorter != position)
	{
		if(m_positionMarketSorter)
		{
			m_positionMarketSorter->RemoveObserver(this);
		}
		m_positionMarketSorter = position;
		if(m_positionMarketSorter)
		{
			m_positionMarketSorter->AddObserver(this);
			SetMarketSorterPositionSize(m_positionMarketSorter->GetSize(), false);
		}
		else
		{
			SetMarketSorterPositionSize(0, true);
		}
//		UpdatePosition();
		return true;
	}
	return false;
}

void SampleDialogSettingAccountAction::MakeMarketSorterSymbolCurrent()
{
	m_currentSymbolMarketSorter = m_symbolMarketSorter;
}

void SampleDialogSettingAccountAction::ResetMarketSorterSymbolCurrent()
{
	m_EditSymbolMarketSorter.SetWindowText(m_currentSymbolMarketSorter.c_str());
//	m_symbolMarketSorter = m_currentSymbolMarketSorter;
//	EnableSubscribeMarketSorterButton();
}

bool SampleDialogSettingAccountAction::DoSubscribeMarketSorter()
{
	const bool ret = CanSubscribeMarketSorter();
	MakeMarketSorterSymbolCurrent();
	if(ret)
	{
		Security* security = nullptr;

//		MakeMarketSorterSymbolCurrent();
//		m_currentSymbolMarketSorter = m_symbolMarketSorter;

//		m_marketSorterStockInvalid = false;
		if(!m_currentSymbolMarketSorter.empty())
		{
			security = TD_ObtainSortableEquity(m_currentSymbolMarketSorter.c_str(), false);
		}
		SetSecurityMarketSorter(security);
		m_valueMarketSorterCurrentStock.SetTextInRect(m_currentSymbolMarketSorter.c_str());
	}
	::InvalidateRect(m_EditSymbolMarketSorter.m_hWnd, nullptr, FALSE);
	EnableSubscribeMarketSorterButton();
	return ret;
}


void SampleDialogSettingAccountAction::OnSymbolMarketSorterChange()
{
	const bool empty = m_symbolMarketSorter.empty();
	const bool symbolCurrent = m_symbolMarketSorter == m_currentSymbolMarketSorter;// m_securityMarketSorter && !strcmp(m_securityMarketSorter->GetSymbol(), m_symbolMarketSorter.c_str());
	CString text;
	m_EditSymbolMarketSorter.GetWindowText(text);
	m_symbolMarketSorter = text;
	if(m_symbolMarketSorter.empty() != empty || (m_symbolMarketSorter == m_currentSymbolMarketSorter) != symbolCurrent)//(m_securityMarketSorter && !strcmp(m_securityMarketSorter->GetSymbol(), m_symbol.c_str())) != symbolCurrent)
	{
		m_EditSymbolMarketSorter.Invalidate(FALSE);
	}
	EnableSubscribeMarketSorterButton();
}

HBRUSH SampleDialogSettingAccountAction::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSymbolMarketSorterId:
		if(m_symbolMarketSorter.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		if(m_symbolMarketSorter.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		else if(m_symbolMarketSorter != m_currentSymbolMarketSorter)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;

		case editSymbolMarketDataId:
		if(m_symbolMarketData.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		if(m_symbolMarketData.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		else if(m_symbolMarketData != m_currentSymbolMarketData)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;
	}
	return BaseDialogSettingAccount::OnCtlColor(pDC, pWnd, nCtlColor);
}

#ifndef TAKION_NO_OPTIONS
void SampleDialogSettingAccountAction::OnSelchangeOptionSuffix()
{
	int sel = m_ComboBoxOptionSuffix.GetCurSel();
	const char optionSuffix = sel >= 0 ? (char)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		m_ComboBoxOptionSuffix.GetItemDataPtr(sel) : '\0';
	if(optionSuffix != m_optionSuffix)
	{
		m_optionSuffix = optionSuffix;
		EnableSubscribeMarketDataButton();
		m_modified = true;
	}
}

void SampleDialogSettingAccountAction::OnCalendarDropdown(NMHDR* pNMHDR, LRESULT* pResult)
{
	UGUI_IdleInterrupt(true, 4);
	*pResult = 0;
}

void SampleDialogSettingAccountAction::OnCalendarCloseup(NMHDR* pNMHDR, LRESULT* pResult)
{
	UGUI_IdleInterrupt(false, 4);
	*pResult = 0;
}

bool SampleDialogSettingAccountAction::GetOptionValues(unsigned __int64& rootSymbol, unsigned __int64& underlierSymbol, unsigned __int64& optionBlock, unsigned short& optionType) const
{
	if(AreSettingsValid())
	{
		optionType = 0;
//		rootSymbol = m_numericSymbol;
		rootSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(m_symbolMarketData.c_str(), sizeof(unsigned __int64) - 1);
		if(isOptionUsed())
		{
			if(m_optionSuffix)
			{
				underlierSymbol = rootSymbol;
				OptionKey::AddSuffixToSymbol(rootSymbol, m_optionSuffix);
			}
			else
			{
				underlierSymbol = 0;
			}
			OptionKey optionKey(m_strikePrice.GetCompactPrice(),
				m_optionCall,
				m_optionSuffix,
				m_expirationDay,
				m_expirationMonth,
				m_expirationYear);
			optionBlock = optionKey.CalculateOptionBlock();
		}
		else
		{
			underlierSymbol = 0;
			optionBlock = 0;
		}
		return true;
	}
	return false;
}

void SampleDialogSettingAccountAction::OnExpirationDateChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(UpdateOptionExpirationValue())
	{
		m_modified = true;
	}
	*pResult = 0;
}

bool SampleDialogSettingAccountAction::UpdateOptionExpirationValue()
{
	bool ret = false;
	SYSTEMTIME d;
	m_DateTimeOptionExpiration.GetTime(&d);
	unsigned char token = (unsigned char)d.wMonth;
	if(token != m_expirationMonth)
	{
		m_expirationMonth = token;
		ret = true;
	}
	token = (unsigned char)d.wDay;
	if(token != m_expirationDay)
	{
		m_expirationDay = token;
		ret = true;
	}
	token = (unsigned char)(d.wYear % 100);
	if(token != m_expirationYear)
	{
		m_expirationYear = token;
		ret = true;
	}
	if(ret)
	{
		OptionValuesChanged();
		UpdateExpirationColor();
	}
	return ret;
}

void SampleDialogSettingAccountAction::UpdateCallPutBkColor()
{
	const COLORREF color = m_optionCall == m_currentOptionCall || !m_optionMode ? 0xFFFFFFFF : m_visualBase->GetPendingColor();
	m_RadioOptionCall.SetBackgroundColor(color);//AndBWTextColor(color);
	m_RadioOptionPut.SetBackgroundColor(color);//AndBWTextColor(color);
}

void SampleDialogSettingAccountAction::SetOptionCall(const bool call)
{
	if(call != m_optionCall)
	{
		m_optionCall = call;
		UpdateCallPutBkColor();

		m_modified = true;
		OptionValuesChanged();
	}
}

void SampleDialogSettingAccountAction::OnOptionCall()
{
	SetOptionCall(m_RadioOptionCall.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingAccountAction::OnOptionPut()
{
	SetOptionCall(m_RadioOptionPut.GetCheck() == BST_UNCHECKED);
}

void SampleDialogSettingAccountAction::OnOption()
{
	const bool optionMode = m_CheckBoxOption.GetCheck() == BST_CHECKED;
	if(optionMode != m_optionMode)
	{
		m_optionMode = optionMode;
		m_modified = true;
		OptionModeChanged();
	}
}

void SampleDialogSettingAccountAction::SetCallPut(bool call)
{
	if(m_RadioOptionCall.m_hWnd)
	{
		if(call)
		{
			m_RadioOptionPut.SetCheck(BST_UNCHECKED);
			m_RadioOptionCall.SetCheck(BST_CHECKED);
		}
		else
		{
			m_RadioOptionCall.SetCheck(BST_UNCHECKED);
			m_RadioOptionPut.SetCheck(BST_CHECKED);
		}
		OnOptionCall();
	}
	else
	{
		SetOptionCall(call);
	}
}

void SampleDialogSettingAccountAction::SetStrikePrice(const Price& strikePrice)
{
	if(strikePrice != m_strikePrice)
	{
		m_strikePrice = strikePrice;
		FillSpinControlFromPrice(m_strikePrice, m_SpinOptionStrikePrice, 100, false);
		UpdateStrikePriceColor();
	}
}

void SampleDialogSettingAccountAction::SetExpirationDate(unsigned int date)
{
//	unsigned int date = extensionConfig->m_positionActionOptionExpiration.GetValue();
	if(!date)
	{
		unsigned short year;
//		unsigned char month;
//		unsigned char day;
		U_GetDateTimeTokensDateOnly(TL_GetTodaysDateInShiftFormat(), year, m_expirationMonth, m_expirationDay);
		m_expirationYear = year % 100;
	}
	else
	{
		unsigned int token = date / 100;
		const unsigned char expirationDay = (unsigned char)(date - token * 100);
		date = token;
		token = date / 100;
		const unsigned char expirationMonth = (unsigned char)(date - token * 100);
		const unsigned char expirationYear = (unsigned char)token;
//	if(expirationDay != m_expirationDay || expirationMonth != m_expirationMonth || expirationYear != m_expirationYear)
//	{
		m_expirationDay = expirationDay;
		m_expirationMonth = expirationMonth;
		m_expirationYear = expirationYear;
	}
	if(m_DateTimeOptionExpiration.m_hWnd)
	{
		SYSTEMTIME d;
		d.wDay = m_expirationDay;
		d.wMonth = m_expirationMonth;
		d.wYear = 2000 + m_expirationYear;
		d.wDayOfWeek = d.wHour = d.wMilliseconds = d.wMinute = d.wSecond = 0;
		m_DateTimeOptionExpiration.SetTime(&d);
	}
	UpdateExpirationColor();
}

void SampleDialogSettingAccountAction::SetOptionValues(bool useOption, char suffix, bool call, const Price& strikePrice, unsigned int expirationDate)
{
	SetCallPut(call);//extensionConfig->m_mdOptionCall.GetValue());
	
	const int found = m_ComboBoxOptionSuffix.FindItem((const void*)suffix);//extensionConfig->m_mdOptionSuffix.GetValue());
	m_ComboBoxOptionSuffix.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeOptionSuffix();

	SetStrikePrice(strikePrice);//extensionConfig->m_mdOptionStrikePrice.GetValue());

	SetExpirationDate(expirationDate);//extensionConfig->m_mdOptionExpiration.GetValue());

	m_CheckBoxOption.SetCheck(useOption ? BST_CHECKED : BST_UNCHECKED);//extensionConfig->m_mdOption.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnOption();
}

bool SampleDialogSettingAccountAction::isExpirationValid() const
{
	return !TL_IsDatePassed((unsigned int)m_expirationYear + 2000, m_expirationMonth, m_expirationDay);
}

bool SampleDialogSettingAccountAction::AreOptionSettingsValid() const
{
//	return !m_optionMode || (!m_strikePrice.isZero() && isExpirationValid());
	return !m_optionMode || !m_strikePrice.isZero();
}

bool SampleDialogSettingAccountAction::AreSettingsValid() const
{
	return !m_symbolMarketData.empty() &&//BaseDialogSettingAccount::AreSettingsValid() &&
		AreOptionSettingsValid();
}

bool SampleDialogSettingAccountAction::OptionModeChanged()
{
	const bool option = isOptionUsed();
	m_ComboBoxOptionSuffix.SetInvalid(!option);

//	m_SpinOptionStrikePrice.SetBkColorEnabled(option ? m_strikePrice.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
	UpdateStrikePriceColor();
	UpdateCallPutBkColor();
	UpdateExpirationColor();
//	m_DateTimeOptionExpiration.SetBkColor(option ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
	m_RadioOptionCall.SetInvalid(!option);
	m_RadioOptionPut.SetInvalid(!option);
	OptionValuesChanged();
	return option;
}

void SampleDialogSettingAccountAction::OptionValuesChanged()
{
	EnableSubscribeMarketDataButton();
}

void SampleDialogSettingAccountAction::UpdateStrikePriceColor()
{
	m_SpinOptionStrikePrice.SetBkColorEnabled(m_optionMode ?
		m_strikePrice.isZero() ? m_visualBase->GetWarningColor():
		m_strikePrice == m_currentStrikePrice ? GetSysColor(COLOR_WINDOW):
		m_visualBase->GetPendingColor():
		m_visualBase->GetGrayColor());
}

void SampleDialogSettingAccountAction::MakeExpirationCurrent()
{
	m_currentExpirationYear = m_expirationYear;
	m_currentExpirationMonth = m_expirationMonth;
	m_currentExpirationDay = m_expirationDay;
	UpdateExpirationColor();
}

void SampleDialogSettingAccountAction::UpdateExpirationColor()
{
	m_DateTimeOptionExpiration.SetBkColor(m_optionMode ?
		m_currentExpirationDay == m_expirationDay && m_currentExpirationMonth == m_expirationMonth && m_currentExpirationYear == m_expirationYear ? GetSysColor(COLOR_WINDOW):
		m_visualBase->GetPendingColor():
		m_visualBase->GetGrayColor());
}

LRESULT SampleDialogSettingAccountAction::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinMarketDataOptionStrikePriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinOptionStrikePrice, 100);
			const bool priceZero = price.isZero();
//			const bool changed = isOptionUsed() && m_strikePrice.isZero() != priceZero;
//			if(changed)m_SpinOptionStrikePrice.SetBkColorEnabled(priceZero ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
			if(price != m_strikePrice)
			{
				m_strikePrice = price;
				UpdateStrikePriceColor();
				OptionValuesChanged();
				m_modified = true;
			}
//			if(changed)OptionValuesChanged();
		}
		break;
/*
		default:
		BaseDialogSettingAccount::OnSpinChange(w, l);
		break;
*/
	}
	return 0;
}
#endif

void SampleDialogSettingAccountAction::OnSetPhantomPositionsFlat()
{
	TD_SetPhantomPositionsFlat((Account*)m_account);
}

void SampleDialogSettingAccountAction::OnSetAllPhantomPositionsFlat()
{
	TD_SetPhantomPositionsFlat(nullptr);
}

void SampleDialogSettingAccountAction::OnDestroyUntradedPositions()
{
	TD_DestroyUntradedPositions((Account*)m_account);
}

void SampleDialogSettingAccountAction::OnDestroyAllUntradedPositions()
{
	TD_DestroyUntradedPositions(nullptr);
}
