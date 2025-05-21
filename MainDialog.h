#pragma once

//#include "TakionGui.h"

class SampleDialogSettingMarketData;
class SampleDialogSettingStockMarketSorter;
class SampleDialogSettingLevel2InThread;
class SampleDialogSettingMarketSorterImbalance;
class SampleDialogSettingHistPrint;
class SampleDialogSettingHistChart;
class SampleDialogSettingTodayChart;
class SampleDialogSettingOrder;
class SampleDialogSettingPosition;
class SampleDialogSettingAccountAction;
class SampleDialogSettingPositionAction;
class SampleDialogSettingWindow;
class SampleDialogSettingTestRequest;
class SampleDialogSettingSimulateAccountRisk;
class SampleDialogSettingSimulatePositionLoad;
class SampleDialogSettingSimulateStockLoanInfo;
class SampleDialogSettingSimulateEtbList;
class SampleDialogSettingSimulateUntradeablStatus;
class SampleDialogSettingSimulateOrderUpdate;
class SampleDialogSettingSimulateBustExec;
class SampleDialogSettingSimulateFirmPosition;
class SampleDialogSettingSimulateEnforceAggregation;
class SampleDialogSettingCancel;
class DialogStrategyHiLo;

class MainDialog : public ExtensionMainDialog//TakionSettingTopDialog
{
public:
	MainDialog(const char* productName, unsigned __int64 productVersion, const char* configExtension);
	virtual bool CurrentAccountSet(Account* account) override;
	void TakionWindowAdded(TakionTool* tool);
	void TakionWindowRemoved(TakionTool* tool);
	void TakionWindowRepaintTool(TakionTool* tool);
	typedef std::map<unsigned int, std::string> UIntStrMap;
	bool GetWindowsLayout(UIntStrMap& windowsMap, unsigned int contentModifier) const;
	void AddMessageReport(unsigned char connectionType, const Message* message, bool outgoing);

	bool MainThreadIdleState(unsigned int idleCount, bool takionIdleDone);
	void NextDayStarted();
//Trader locked/unlocked trading through the UI
	void TradingLocked(bool locked);
	void TradingDefaultAccountOnly(bool defaultAccountOnly);
	void TradingTestStocksOnly(bool testStocksOnly);
protected:
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	SampleDialogSettingStockMarketSorter* m_marketSorterDialog;
	SampleDialogSettingLevel2InThread* m_level2InThreadDialog;
	SampleDialogSettingMarketData* m_marketDataDialog;
	SampleDialogSettingMarketSorterImbalance* m_imbalanceDialog;
	SampleDialogSettingHistPrint* m_histPrintDialog;
	SampleDialogSettingHistChart* m_histChartDialog;
	SampleDialogSettingTodayChart* m_todayChartDialog;
	SampleDialogSettingOrder* m_orderDialog;
	SampleDialogSettingPosition* m_positionDialog;
	SampleDialogSettingAccountAction* m_accountActionDialog;
	SampleDialogSettingPositionAction* m_positionActionDialog;
	SampleDialogSettingWindow* m_windowDialog;
	SampleDialogSettingTestRequest* m_testRequestDialog;
	SampleDialogSettingSimulateAccountRisk* m_accountRiskDialog;
	SampleDialogSettingSimulatePositionLoad* m_loadPositionDialog;
	SampleDialogSettingSimulateStockLoanInfo* m_stockLoanDialog;
	SampleDialogSettingSimulateEtbList* m_etbListDialog;
	SampleDialogSettingSimulateUntradeablStatus* m_untradeableStatusDialog;
	SampleDialogSettingSimulateOrderUpdate* m_orderUpdateDialog;
	SampleDialogSettingSimulateBustExec* m_bustExecDialog;
	SampleDialogSettingSimulateFirmPosition* m_firmPositionDialog;
	SampleDialogSettingSimulateEnforceAggregation* m_enforceAggregationDialog;
	SampleDialogSettingCancel* m_orderCancelDialog;
	DialogStrategyHiLo* m_dialogHiLo;
};
