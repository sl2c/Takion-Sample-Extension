#include "stdafx.h"
#include "MainDialog.h"
//#include "ExtensionSample.h"
//#include "ExtensionDerivedConfig.h"
#include "SampleDialogSettingVenue.h"
#include "SampleDialogSettingSwipe.h"
#include "SampleDialogSettingBorrow.h"
#include "SampleDialogSettingCancel.h"
#include "SampleDialogSettingConnection.h"
#include "SampleDialogSettingCommand.h"
#include "SampleDialogSettingMarketData.h"
#include "SampleDialogSettingStockMarketSorter.h"
#include "SampleDialogSettingLevel2InThread.h"
#include "SampleDialogSettingMarketSorterImbalance.h"
#include "SampleDialogSettingHistPrint.h"
#include "SampleDialogSettingHistChart.h"
#include "SampleDialogSettingTodayChart.h"
#include "SampleDialogSettingOrder.h"
#include "SampleDialogSettingPosition.h"
#include "SampleDialogSettingAccountAction.h"
#include "SampleDialogSettingPositionAction.h"
#include "SampleDialogSettingWindow.h"
#include "SampleDialogSettingTestRequest.h"
#include "SampleDialogSettingSimulateQuote.h"
#include "SampleDialogSettingSimulatePrint.h"
#include "SampleDialogSettingSimulateLevel1.h"
#include "SampleDialogSettingSimulateReset.h"
#include "SampleDialogSettingSimulateNewDay.h"
#include "SampleDialogSettingSimulateAdminBroadcast.h"
#include "SampleDialogSettingSimulateSnapshot.h"
#include "SampleDialogSettingSimulateIndexSnapshot.h"
#include "SampleDialogSettingSimulateChartSnapshot.h"
#include "SampleDialogSettingSimulateLrpLuld.h"
#include "SampleDialogSettingSimulateStockType.h"
#include "SampleDialogSettingSimulateTradingAction.h"
#include "SampleDialogSettingSimulateClosingPrice.h"
//#include "SampleDialogSettingSimulateInvalidSymbol.h"
#include "SampleDialogSettingSimulateImbalance.h"
#include "SampleDialogSettingSimulateChartCorrection.h"
#include "SampleDialogSettingSimulateAccountRisk.h"
#include "SampleDialogSettingSimulatePositionLoad.h"
#include "SampleDialogSettingSimulateOrderUpdate.h"
#include "SampleDialogSettingSimulateBustExec.h"
#include "SampleDialogSettingSimulateFirmPosition.h"
#include "SampleDialogSettingSimulateEnforceAggregation.h"
#include "SampleDialogSettingSimulateMarketStatus.h"
#include "SampleDialogSettingSimulateMdLogon.h"
#include "SampleDialogSettingSimulateStockLoanInfo.h"
#include "SampleDialogSettingSimulateEtbList.h"
#include "SampleDialogSettingSimulateUntradeableStatus.h"
#include "SampleDialogSettingNews.h"
#include "DialogStrategyHiLo.h"
#include "ExtensionSample.h"

extern ExtensionSample theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//extern ExtensionSample theApp;
//extern ExtensionDerivedConfig* extensionConfig;
//extern ExtensionDerivedConfig* savedConfig;
/*
enum MainDialogIds
{
	buttonOpenId = 3600,

	buttonSaveId,
	buttonSaveAsId,
	buttonRestoreId,

	mainDialogIdsCount,
};
*/
std::string CreateTitle(const char* const& productName, const unsigned __int64& productVersion)
{
	std::string title(productName);
	title += ' ';
	U_VersionNumToStr(productVersion, title);
	return title;
}

MainDialog::MainDialog(const char* productName, unsigned __int64 productVersion, const char* configExtension):
	ExtensionMainDialog(280,
		164,
		CreateTitle(productName, productVersion).c_str(),
		productName,
		productVersion,
		configExtension,
		RGB(0, 0, 128),
		0,
		false,
		30,
		"Hide",
		"Exit",
		theApp.m_hInstance,
		true),

//	ExtensionMainDialog(360, 164, productName, productName, productVersion, configExtension),
	m_marketSorterDialog(nullptr),
	m_level2InThreadDialog(nullptr),
	m_marketDataDialog(nullptr),
	m_imbalanceDialog(nullptr),
	m_histPrintDialog(nullptr),
	m_histChartDialog(nullptr),
	m_todayChartDialog(nullptr),
	m_orderDialog(nullptr),
	m_positionDialog(nullptr),
	m_accountActionDialog(nullptr),
	m_positionActionDialog(nullptr),
	m_windowDialog(nullptr),
	m_testRequestDialog(nullptr),
	m_accountRiskDialog(nullptr),
	m_loadPositionDialog(nullptr),
	m_stockLoanDialog(nullptr),
	m_etbListDialog(nullptr),
	m_untradeableStatusDialog(nullptr),
	m_orderUpdateDialog(nullptr),
	m_bustExecDialog(nullptr),
	m_firmPositionDialog(nullptr),
	m_enforceAggregationDialog(nullptr),
	m_orderCancelDialog(nullptr),
	m_dialogHiLo(nullptr)
//	m_currentAccount(TD_GetCurrentAccount()),
//	m_myWidth(0)
{
/*
	AddStyleIndirect(WS_THICKFRAME);

	AddButtonIndirect("Open", WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON, 0, 441, 147, buttonWidth, buttonHeight, buttonOpenId);
	AddButtonIndirect("Save", WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON, 0, 441, 147, buttonWidth, buttonHeight, buttonSaveId);
	AddButtonIndirect("Save as", WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON, 0, 441, 147, buttonWidth, buttonHeight, buttonSaveAsId);
	AddButtonIndirect("Restore", WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON, 0, 441, 147, buttonWidth, buttonHeight, buttonRestoreId);
*/
}
/*
void MainDialog::DoDataExchange(CDataExchange* pDX)
{
	TakionSettingTopDialog::DoDataExchange(pDX);

	DDX_Control(pDX, buttonOpenId, m_ButtonOpenConfig);
	DDX_Control(pDX, buttonSaveId, m_ButtonSaveConfig);
	DDX_Control(pDX, buttonSaveAsId, m_ButtonSaveAsConfig);
	DDX_Control(pDX, buttonRestoreId, m_ButtonRestoreConfig);
}

BEGIN_MESSAGE_MAP(MainDialog, TakionSettingTopDialog)
	ON_BN_CLICKED(buttonOpenId, OnOpenConfig)
	ON_BN_CLICKED(buttonSaveId, OnSaveConfig)
	ON_BN_CLICKED(buttonSaveAsId, OnSaveAsConfig)
	ON_BN_CLICKED(buttonRestoreId, OnRestoreConfig)
END_MESSAGE_MAP()

void MainDialog::OnOpenConfig()
{
	OpenLayout();
}

void MainDialog::ApplyConfig(unsigned int flags)
{

	m_frameRect.left = extensionConfig->m_x.GetValue();
	m_frameRect.top = extensionConfig->m_y.GetValue();
	m_frameRect.right = m_frameRect.left + (int)extensionConfig->m_w.GetValue();
	m_frameRect.bottom = m_frameRect.top + (int)extensionConfig->m_h.GetValue();
	MoveToFrameRect();

	RestoreLayout();
//	CalculateClientRect(m_frameRect.Width(), m_frameRect.Height());
	SetTabToOpen(extensionConfig->m_tab.GetValue().c_str(), nullptr);
	if(m_hWnd)
	{
		SetInitialTab();
	}
}

void MainDialog::OnRestoreConfig()
{
	RestoreLayout();
}

void MainDialog::OnSaveConfig()
{
	SaveLayout();
}

void MainDialog::OnSaveAsConfig()
{
	SaveLayoutAs();
}

void MainDialog::Corner()
{
	if(m_hWnd)
	{
		bool visible = !::IsWindowVisible(m_hWnd);
		::SetWindowPos(m_hWnd, HWND_TOP, m_screenRect.left, m_screenRect.top, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
		if(visible)
		{
			Entering();
		}
	}
}

void MainDialog::Top()
{
	if(m_hWnd)
	{
		bool visible = !::IsWindowVisible(m_hWnd);
		::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		if(visible)
		{
			Entering();
		}
	}
}

void MainDialog::Resize(bool widthChanged, bool heightChanged)
{
	TakionSettingTopDialog::Resize(widthChanged, heightChanged);
}

bool MainDialog::PlaceButtons()
{
	if(TakionSettingTopDialog::PlaceButtons())
	{
		CRect rect;
		m_ButtonCancel.GetWindowRect(&rect);
		ScreenToClient(&rect);

		const int gap = 2;
		const int left = rect.left;
		const int height = rect.Height();
		int top = rect.bottom + gap;
		m_ButtonOpenConfig.SetWindowPos(nullptr, left, top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		top += height + gap;
		m_ButtonSaveConfig.SetWindowPos(nullptr, left, top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		top += height + gap;
		m_ButtonSaveAsConfig.SetWindowPos(nullptr, left, top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		top += height + gap;
		m_ButtonRestoreConfig.SetWindowPos(nullptr, left, top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		return true;
	}
	return false;
}

void MainDialog::FinishDialog(int result)
{
	extensionConfig->m_x.SetValue(m_frameRect.left);
	extensionConfig->m_y.SetValue(m_frameRect.top);
	extensionConfig->m_w.SetValue((unsigned int)m_frameRect.Width());
	extensionConfig->m_h.SetValue((unsigned int)m_frameRect.Height());
	TakionSettingTopDialog::FinishDialog(result);
}

void MainDialog::Apply()
{
	UpdateSettings();
	SaveLayout();
}
*/

void MainDialog::TradingLocked(bool locked)
{
	if(m_accountActionDialog)m_accountActionDialog->TradingLocked(locked);
}

void MainDialog::TradingDefaultAccountOnly(bool defaultAccountOnly)
{
	if(m_accountActionDialog)m_accountActionDialog->TradingDefaultAccountOnly(defaultAccountOnly);
}

void MainDialog::TradingTestStocksOnly(bool testStocksOnly)
{
	if(m_accountActionDialog)m_accountActionDialog->TradingTestStocksOnly(testStocksOnly);
}

void MainDialog::NextDayStarted()
{
	if(m_histChartDialog)m_histChartDialog->NextDayStarted();
}

void MainDialog::TakionWindowAdded(TakionTool* tool)
{
	if(m_windowDialog)m_windowDialog->TakionWindowAdded(tool);
}

void MainDialog::TakionWindowRemoved(TakionTool* tool)
{
	if(m_windowDialog)m_windowDialog->TakionWindowRemoved(tool);
}

void MainDialog::TakionWindowRepaintTool(TakionTool* tool)
{
	if(m_windowDialog)m_windowDialog->TakionWindowRepaintTool(tool);
}

bool MainDialog::GetWindowsLayout(UIntStrMap& windowsMap, unsigned int contentModifier) const
{
	return m_windowDialog && m_windowDialog->GetWindowsLayout(windowsMap, contentModifier);
}

void MainDialog::AddMessageReport(unsigned char connectionType, const Message* message, bool outgoing)
{
	if(m_testRequestDialog)m_testRequestDialog->AddMessageReport(connectionType, message, outgoing);
}

BOOL MainDialog::OnInitDialog()
{
	TakionMainWnd* frame = TGUI_GetTakionMainWnd();

	TakionSettingTabDialog* tabDlg = new TakionSettingTabDialog(m_mainWnd, this, "Account");//, 437, 152);
	AddSettingDialog(tabDlg);
	m_orderDialog = new SampleDialogSettingOrder(frame, tabDlg); 
	tabDlg->AddSettingDialog(m_orderDialog);
	m_positionDialog = new SampleDialogSettingPosition(frame, tabDlg);
	tabDlg->AddSettingDialog(m_positionDialog);
	tabDlg->AddSettingDialog(new SampleDialogSettingConnection(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingCommand(frame, tabDlg));
	m_accountActionDialog = new SampleDialogSettingAccountAction(frame, tabDlg);
	tabDlg->AddSettingDialog(m_accountActionDialog);

	tabDlg = new TakionSettingTabDialog(m_mainWnd, this, "Position");
	AddSettingDialog(tabDlg);
	m_positionActionDialog = new SampleDialogSettingPositionAction(frame, tabDlg);
	tabDlg->AddSettingDialog(m_positionActionDialog);

	tabDlg = new TakionSettingTabDialog(m_mainWnd, this, "MarketData");//, 437, 152);
	AddSettingDialog(tabDlg);
	m_marketDataDialog = new SampleDialogSettingMarketData(frame, tabDlg); 
	tabDlg->AddSettingDialog(m_marketDataDialog);
	m_marketSorterDialog = new SampleDialogSettingStockMarketSorter(frame, tabDlg, m_currentAccount);
	tabDlg->AddSettingDialog(m_marketSorterDialog);
	m_level2InThreadDialog = new SampleDialogSettingLevel2InThread(frame, tabDlg, m_currentAccount);
	tabDlg->AddSettingDialog(m_level2InThreadDialog);
	m_imbalanceDialog = new SampleDialogSettingMarketSorterImbalance(frame, tabDlg, m_currentAccount);
	tabDlg->AddSettingDialog(m_imbalanceDialog);
	m_histPrintDialog = new SampleDialogSettingHistPrint(frame, tabDlg); 
	tabDlg->AddSettingDialog(m_histPrintDialog);
	m_histChartDialog = new SampleDialogSettingHistChart(frame, tabDlg); 
	tabDlg->AddSettingDialog(m_histChartDialog);
	m_todayChartDialog = new SampleDialogSettingTodayChart(frame, tabDlg); 
	tabDlg->AddSettingDialog(m_todayChartDialog);

	tabDlg = new TakionSettingTabDialog(m_mainWnd, this, "Order");//, 437, 152);
	tabDlg->AddSettingDialog(new SampleDialogSettingVenue(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSwipe(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingBorrow(frame, tabDlg));
	m_orderCancelDialog = new SampleDialogSettingCancel(frame, tabDlg);
	tabDlg->AddSettingDialog(m_orderCancelDialog);
	AddSettingDialog(tabDlg);

	SampleDialogSettingNews* dn = new SampleDialogSettingNews(frame, this);
	AddSettingDialog(dn);

	m_windowDialog = new SampleDialogSettingWindow(frame, this);
	AddSettingDialog(m_windowDialog);

	tabDlg = new TakionSettingTabDialog(m_mainWnd, this, "Strategy");
	m_dialogHiLo = new DialogStrategyHiLo(frame, tabDlg);
	tabDlg->AddSettingDialog(m_dialogHiLo);
	AddSettingDialog(tabDlg);

	tabDlg = new TakionSettingTabDialog(m_mainWnd, this, "TestMessage");//, 0, 0, nullptr, RGB(0, 0, 128), 20);//, 437, 152);
	m_testRequestDialog = new SampleDialogSettingTestRequest(frame, tabDlg); 
	tabDlg->AddSettingDialog(m_testRequestDialog);
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateQuote(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulatePrint(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateLevel1(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateReset(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateLrpLuld(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateStockType(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateTradingAction(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateClosingPrice(frame, tabDlg));
//	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateInvalidSymbol(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateImbalance(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateChartCorrection(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateSnapshot(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateChartSnapshot(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateIndexSnapshot(frame, tabDlg));

	m_accountRiskDialog = new SampleDialogSettingSimulateAccountRisk(frame, tabDlg);
	tabDlg->AddSettingDialog(m_accountRiskDialog);

	m_loadPositionDialog = new SampleDialogSettingSimulatePositionLoad(frame, tabDlg);
	tabDlg->AddSettingDialog(m_loadPositionDialog);

	m_orderUpdateDialog = new SampleDialogSettingSimulateOrderUpdate(frame, tabDlg);
	tabDlg->AddSettingDialog(m_orderUpdateDialog);

	m_bustExecDialog = new SampleDialogSettingSimulateBustExec(frame, tabDlg);
	tabDlg->AddSettingDialog(m_bustExecDialog);

	m_firmPositionDialog = new SampleDialogSettingSimulateFirmPosition(frame, tabDlg);
	tabDlg->AddSettingDialog(m_firmPositionDialog);
	m_enforceAggregationDialog = new SampleDialogSettingSimulateEnforceAggregation(frame, tabDlg);
	tabDlg->AddSettingDialog(m_enforceAggregationDialog);

	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateMarketStatus(frame, tabDlg));

	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateNewDay(frame, tabDlg));
	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateAdminBroadcast(frame, tabDlg));

	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateMdLogon(frame, tabDlg));

//	tabDlg->AddSettingDialog(new SampleDialogSettingSimulateStockLoanInfo(frame, tabDlg));
	m_stockLoanDialog = new SampleDialogSettingSimulateStockLoanInfo(frame, tabDlg);
	tabDlg->AddSettingDialog(m_stockLoanDialog);
	
	m_etbListDialog = new SampleDialogSettingSimulateEtbList(frame, tabDlg);
	tabDlg->AddSettingDialog(m_etbListDialog);

	m_untradeableStatusDialog = new SampleDialogSettingSimulateUntradeablStatus(frame, tabDlg);
	tabDlg->AddSettingDialog(m_untradeableStatusDialog);

	AddSettingDialog(tabDlg);

	BOOL ret = ExtensionMainDialog::OnInitDialog();

//	m_myWidth = m_frameRect.Width();
	return ret;
}
/*
void MainDialog::GetPosRect(CRect& r)
{
	r.left = extensionConfig->m_x.GetValue();
	r.top = extensionConfig->m_y.GetValue();
	r.right = r.left + (int)extensionConfig->m_w.GetValue();
	r.bottom = r.top + (int)extensionConfig->m_h.GetValue();
}

bool MainDialog::isInitializedFromFile() const
{
	return extensionConfig->m_x.isInitializedFromFile();
}

void MainDialog::SaveAlwaysOnTop(bool onTop)
{
	extensionConfig->m_alwaysOnTop.SetValue(onTop);
}

bool MainDialog::ExtractIsAlwaysOnTop() const
{
	return extensionConfig->m_alwaysOnTop.GetValue();
}
*/
void MainDialog::BeforeDestroy()
{
	m_imbalanceDialog = nullptr;
	m_marketSorterDialog = nullptr;
	m_level2InThreadDialog = nullptr;
	m_marketDataDialog = nullptr;
	m_histPrintDialog = nullptr;
	m_histChartDialog = nullptr;
	m_todayChartDialog = nullptr;
	m_orderDialog = nullptr;
	m_positionDialog = nullptr;
	m_accountActionDialog = nullptr;
	m_positionActionDialog = nullptr;
	m_windowDialog = nullptr;
	m_testRequestDialog = nullptr;
	m_accountRiskDialog = nullptr;
	m_loadPositionDialog = nullptr;
	m_stockLoanDialog = nullptr;
	m_etbListDialog = nullptr;
	m_untradeableStatusDialog = nullptr;
	m_orderUpdateDialog = nullptr;
	m_bustExecDialog = nullptr;
	m_firmPositionDialog = nullptr;
	m_enforceAggregationDialog = nullptr;
	m_orderCancelDialog = nullptr;
	m_dialogHiLo = nullptr;
	ExtensionMainDialog::BeforeDestroy();
}

bool MainDialog::MainThreadIdleState(unsigned int idleCount, bool takionIdleDone)
{
	bool ret = false;
	if(m_level2InThreadDialog && m_level2InThreadDialog->MainThreadIdleState(idleCount, takionIdleDone))
	{
		ret = true;
	}
	return ret;
}

bool MainDialog::CurrentAccountSet(Account* account)
{
	if(ExtensionMainDialog::CurrentAccountSet(account))
	{
		if(m_marketDataDialog)m_marketDataDialog->CurrentAccountSet(m_currentAccount);
		if(m_marketSorterDialog)m_marketSorterDialog->CurrentAccountSet(m_currentAccount);
		if(m_level2InThreadDialog)m_level2InThreadDialog->CurrentAccountSet(m_currentAccount);
		if(m_orderDialog)m_orderDialog->CurrentAccountSet(m_currentAccount);
		if(m_positionDialog)m_positionDialog->CurrentAccountSet(m_currentAccount);
		if(m_accountActionDialog)m_accountActionDialog->CurrentAccountSet(m_currentAccount);
		if(m_positionActionDialog)m_positionActionDialog->CurrentAccountSet(m_currentAccount);
		if(m_imbalanceDialog)m_imbalanceDialog->CurrentAccountSet(m_currentAccount);
		if(m_accountRiskDialog)m_accountRiskDialog->CurrentAccountSet(m_currentAccount);
		if(m_loadPositionDialog)m_loadPositionDialog->CurrentAccountSet(m_currentAccount);
		if(m_stockLoanDialog)m_stockLoanDialog->CurrentAccountSet(m_currentAccount);
		if(m_etbListDialog)m_etbListDialog->CurrentAccountSet(m_currentAccount);
		if(m_untradeableStatusDialog)m_untradeableStatusDialog->CurrentAccountSet(m_currentAccount);
		if(m_orderUpdateDialog)m_orderUpdateDialog->CurrentAccountSet(m_currentAccount);
		if(m_bustExecDialog)m_bustExecDialog->CurrentAccountSet(m_currentAccount);
		if(m_firmPositionDialog)m_firmPositionDialog->CurrentAccountSet(m_currentAccount);
//		if(m_enforceAggregationDialog)m_enforceAggregationDialog->CurrentAccountSet(m_currentAccount);
		if(m_dialogHiLo)m_dialogHiLo->CurrentAccountSet(m_currentAccount);
		if(m_orderCancelDialog)m_orderCancelDialog->CurrentAccountSet(m_currentAccount);
		return true;
	}
	return false;
}

