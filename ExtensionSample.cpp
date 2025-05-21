// ExtensionSample.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ExtensionSample.h"
#include "ExtensionSampleVersion.h"

#include "MainDialog.h"
#include "ExtensionDerivedConfig.h"
#include "StockComparator.h"
#include "MyMarketSorter.h"
#include "MyMarketSorterFilter.h"
#include "DialogStockPair.h"
#include "DialogSignedPriceValue.h"
#include "SampleDialogSettingCommand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only ExtensionSample object
ExtensionSample theApp;

MainDialog* mainDialog = nullptr;

TakionPopupWnd* messageWnd = nullptr;
TakionMessageBox* messageBox = nullptr;

DialogStockPair* stockPairDialog = nullptr;
DialogSignedPriceValue* signedPriceValueDialog = nullptr;

ExtensionDerivedConfig* extensionConfig = nullptr;

MyDialogMarketSorterFilter* marketSorterFilterDialog = nullptr;

//ExtensionDerivedConfig* initialExtensionConfig = nullptr;
//ExtensionDerivedConfig* savedConfig = nullptr;

const char* const configExtension = "ecg";


#ifdef __cplusplus
extern "C"
{
#endif

unsigned int WINAPI GetExtensionCode(){return ClientExtensionDll::validExtensionCode;}
unsigned __int64 WINAPI GetExtensionVersion(){return theApp.GetVersionNum();}
unsigned __int64 WINAPI GetUtilsVersion(){return U_GetModuleVersionNum();}
unsigned __int64 WINAPI GetUtilsHeaderVersion(){return U_VersionStrToNum(UtilsHeaderVersion);}
unsigned __int64 WINAPI GetUtilsGuiVersion(){return UGUI_GetModuleVersionNum();}
unsigned __int64 WINAPI GetUtilsGuiHeaderVersion(){return U_VersionStrToNum(UtilsGuiHeaderVersion);}
unsigned __int64 WINAPI GetTakionUtilsVersion(){return TU_GetModuleVersionNum();}
unsigned __int64 WINAPI GetTakionUtilsHeaderVersion(){return U_VersionStrToNum(TakionUtilsHeaderVersion);}
unsigned __int64 WINAPI GetTakionGuiVersion(){return TGUI_GetModuleVersionNum();}
unsigned __int64 WINAPI GetTakionGuiHeaderVersion(){return U_VersionStrToNum(TakionGuiHeaderVersion);}
unsigned __int64 WINAPI GetTakionDataVersion(){return TD_GetModuleVersionNum();}
unsigned __int64 WINAPI GetTakionDataHeaderVersion(){return U_VersionStrToNum(TakionDataHeaderVersion);}
unsigned __int64 WINAPI GetTakionLogVersion(){return TL_GetModuleVersionNum();}
unsigned __int64 WINAPI GetTakionLogHeaderVersion(){return U_VersionStrToNum(TakionLogHeaderVersion);}
unsigned __int64 WINAPI GetObserverVersion(){return O_GetModuleVersionNum();}
unsigned __int64 WINAPI GetObserverHeaderVersion(){return U_VersionStrToNum(ObserverHeaderVersion);}

//unsigned __int64 WINAPI GetReceiverVersion(){return R_GetModuleVersionNum();}
unsigned __int64 WINAPI GetReceiverHeaderVersion(){return U_VersionStrToNum(ReceiverHeaderVersion);}

bool WINAPI ExtensionIsDebug()
{
#ifdef _DEBUG
	return true;
#else
	return false;
#endif
}

const char* WINAPI GetTakionExtensionDescription(){return ExtensionSampleDescriptionStr;}//theApp.GetFileDescription().c_str();}
const char* WINAPI GetTakionExtensionProductName(){return ExtensionSampleProductNameStr;}
unsigned __int64 WINAPI GetExtensionBuildDateTime(){return theApp.GetBuildDateTime();}

void WINAPI MessageReport(unsigned char connectionType, const Message* message, bool outgoing)
{
	if(mainDialog)mainDialog->AddMessageReport(connectionType, message, outgoing);
}

void WINAPI NextDayStarted()//This function is called first thing when the next day starts. After some actions in Takion you also get a message TMsgNextDayStarted = TM_NEXT_DAY_STARTED
{
	if(mainDialog)
	{
		mainDialog->NextDayStarted();
	}
}

void WINAPI LineDoubleClicked()
{
	if(mainDialog)
	{
		mainDialog->Top();
	}
}

void WINAPI TradingLocked(bool locked)
{
	if(mainDialog)
	{
		mainDialog->TradingLocked(!TD_IsTradingAllowed());
	}
}

void WINAPI TradingDefaultAccountOnly(bool defaultAccountOnly)
{
	if(mainDialog)
	{
		mainDialog->TradingDefaultAccountOnly(TD_IsDefaultOnlyTradingAllowed());
	}
}

void WINAPI TradingTestStocksOnly(bool testStocksOnly)
{
	if(mainDialog)
	{
		mainDialog->TradingTestStocksOnly(TD_IsTestOnlyTradingAllowed());
	}
}

void WINAPI KeyStrokeAndCommand(unsigned char ascii, unsigned char modifier, bool repeat, const char* commandName, unsigned int extensionCode, unsigned int userExtensionCode)//commandName can be nullptr;
{
	switch(extensionCode)
	{
		case 1:
//Extension Keystroke command
		break;

		case 0:
//Takion command
		break;

		default:
//Unknown command
		break;
	}
}

void WINAPI SymbolEnteredInMmBox(const char* symbol, unsigned char securityType, bool valid, bool fromKeyboard, unsigned int ordinal)
{
}

void WINAPI SecurityRefreshed(const char* symbol, const Security* security, bool wasLoaded, unsigned int ordinal)//security can be nullptr;
{
}

void WINAPI ActiveMmBoxChanged(const char* symbol, const Security* security, bool wasLoaded, unsigned int ordinal)//symbol and security can be nullptr;
{
}

void WINAPI CommandWindowRemoved(unsigned int ordinal)
{
}

void WINAPI LayoutChanged()
{
}

void WINAPI AppActivated(bool applicationActive)
{
	if(!applicationActive && mainDialog)mainDialog->OnMouseForcedReleaseCapture();
}

void WINAPI TakionMoveWindows(int dx, int dy)
{
	if(mainDialog)mainDialog->DragWindow(dx, dy);
}

void WINAPI MarketSorterNotifyAdded(HWND hwnd, const char* prefix)
{
}

void WINAPI MarketSorterContents(HWND hwnd, const char* prefix, const Security* security)
{
}

void WINAPI MarketSorterContentsDone(HWND hwnd, const char* prefix)
{
}

void WINAPI MarketSorterNotifyRemoved(HWND hwnd, const char* prefix)
{
}

void WINAPI MarketSorterNotifyPrefixChanged(HWND hwnd, const char* oldPrefix, const char* prefix)
{
}

void WINAPI NotifyMarketSorterSymbol(HWND hwnd, const char* prefix, const Security* security, bool added)
{
}

void WINAPI MarketSorterNotifyAllRowsRemoved(HWND hwnd, const char* prefix)
{
}

void WINAPI MainThreadAvailableDataProcessed()
{
}

bool WINAPI MainThreadIdleState(unsigned int idleCount, bool takionIdleDone)
{
	return mainDialog->MainThreadIdleState(idleCount, takionIdleDone);
}

const char* WINAPI GetExtensionInfo()
{
//	return theApp.GetInfo().c_str();
	TakionSettingDialog* tab = mainDialog->GetCurrentLeaf();
	return tab ? tab->GetLabel() : "";
}

//Format: "<1 ifEnabled, or 0 ifDisabled><1 ifChecked, or 0 ifUnchecked><MenuItemText>|..."
//For a separator: just one char |
void AddMenuItemToString(std::string& menuStr, const char* text = nullptr, bool enabled = true, bool checked = false, bool checkable = false)
{
	if(text)
	{
		if(!menuStr.empty())
		{
			menuStr += '|';
		}
		menuStr += enabled ? '1' : '0';
		menuStr += checked ? '1' : checkable ? '2' : '0';
		menuStr += text;
	}
	else//separator
	{
		menuStr += '|';
	}
}

static bool check = true;
void WINAPI GetTakionExtensionMenu(std::string& menuStr)
{
/*
	menuStr += check ?
		"10First||11Second|00Third|01Fourth|10Show|10Hide|10Corner":
		"10First||10Second|00Third|01Fourth|10Show|10Hide|10Corner";
*/
	const bool dlgReady = mainDialog != nullptr && mainDialog->m_hWnd != nullptr;
	const bool displayed = dlgReady && mainDialog->isDialogDisplayed();
	const bool toShow = dlgReady;// && !displayed;
	bool toCorner = dlgReady;
/*
	if(toCorner)
	{
		const CRect& screenRect = mainDialog->GetScreenRect();
		const CRect& dialogRect = mainDialog->GetDialogFrameRect();
		if(screenRect.left == dialogRect.left && screenRect.top == dialogRect.top)
		{
			toCorner = false;
		}
	}
*/

	AddMenuItemToString(menuStr, "First", true, false);
	AddMenuItemToString(menuStr);
	AddMenuItemToString(menuStr, "Second", true, check, true);
	AddMenuItemToString(menuStr, "Third", false, false);
	AddMenuItemToString(menuStr, "Fourth", false, true);
	AddMenuItemToString(menuStr, "Show", toShow);
	AddMenuItemToString(menuStr, "Hide", displayed);
	AddMenuItemToString(menuStr, "Corner", toCorner);
	AddMenuItemToString(menuStr, "PopupWnd");
	AddMenuItemToString(menuStr, "MessageBox");
}

void WINAPI ExecuteTakionCommand(unsigned short commandId, CWnd* takionExtensionWindow)
{
	switch(commandId)
	{
		case 0:
		{
			TakionMessageBox mb(TGUI_GetTakionMainWnd());
			mb.AddMessage("Command 1 executed");
			mb.DisplayModal(takionExtensionWindow, "Extension Command", MB_OK|MB_ICONINFORMATION);//, RGB(0, 0, 128), RGB(255, 255, 255));

//FILE* fp = fopen( "Log.txt", "w" );
/*
TakionIterator* iterator = TD_CreateEquityIterator();
TD_LockEquityStorage();
iterator->Reset();

std::string info;
char buf[33];

Security* security;
while( security = TD_GetNextEquity( iterator ) )
{
info = security->GetSymbol();
info += ' ';
info += U_UnsignedNumberToStrWithCommas(security->GetYesterdaysVolume(), buf, sizeof(buf));
//fprintf_s( fp, "%s\t%s\n", security->GetSymbol(), volStr );
TD_Log(info.c_str(), "YVol");
}

TD_UnlockEquityStorage();
delete iterator;
*/
 		}
		break;

		case 1:
		check = !check;
		{
			TakionMessageBox mb(TGUI_GetTakionMainWnd());
			mb.AddMessage(check ? "Command 2 checked" : "Command 2 unchecked");
			mb.DisplayModal(takionExtensionWindow, "Extension Command", MB_OK|MB_ICONINFORMATION);//, RGB(0, 0, 128), RGB(255, 255, 255));
		}
		break;

		case 2:
		{
			TakionMessageBox mb(TGUI_GetTakionMainWnd());
			mb.AddMessage("Command 3 executed");
			mb.DisplayModal(takionExtensionWindow, "Extension Command", MB_OK|MB_ICONINFORMATION);//, RGB(0, 0, 128), RGB(255, 255, 255));
		}
		break;

		case 3:
		{
			TakionMessageBox mb(TGUI_GetTakionMainWnd());
			mb.AddMessage("Command 4 executed");
			mb.DisplayModal(takionExtensionWindow, "Extension Command", MB_OK|MB_ICONINFORMATION);//, RGB(0, 0, 128), RGB(255, 255, 255));
		}
		break;

		case 4:
		if(mainDialog)
		{
			mainDialog->Top();
		}
		break;

		case 5:
		if(mainDialog)
		{
			mainDialog->OnCancel();
		}
		break;

		case 6:
		if(mainDialog)
		{
			mainDialog->Corner();
		}
		break;

		case 7:
		if(messageWnd)
		{
			messageWnd->ClearMessageList(false);
			messageWnd->AddMessage("Text", false);
			messageWnd->DisplayModeless(mainDialog ? (CWnd*)mainDialog : (CWnd*)TGUI_GetTakionMainWnd(), "TakionExtension", MB_YESNO | MB_ICONQUESTION);
		}
		break;

		case 8:
		if(messageBox)
		{
			messageBox->ClearMessageList(false);
			messageBox->AddMessage("Text", false);
			messageBox->DisplayModeless(mainDialog ? (CWnd*)mainDialog : (CWnd*)TGUI_GetTakionMainWnd(), "TakionExtension", MB_YESNO | MB_ICONQUESTION);
		}
		break;

		default:
		{
			TakionMessageBox mb(TGUI_GetTakionMainWnd());
			std::string info("Unknown Command ");
			char num[33];
			info += U_UnsignedNumberToStrNoDecDigits(commandId, num, sizeof(num));
			mb.AddMessage(info);
			mb.DisplayModal(takionExtensionWindow, "Extension Command", MB_OK|MB_ICONERROR);//, RGB(0, 0, 128), RGB(255, 255, 255));
		}
		break;
	}
}

void WINAPI ExtensionInitialize()
{
	if(!mainDialog)
	{
		const std::string& configPath = theApp.GetConfigFilePath();

//		mainDialog = new MainDialog(theApp.GetProductName().c_str(), theApp.GetVersionNum(), configExtension);
		mainDialog = new MainDialog(ExtensionSampleProductNameStr, theApp.GetVersionNum(), configExtension);
//		mainDialog->AddSettingDialog(new DialogMirror(mainDialog->GetMainWnd(), mainDialog));

		extensionConfig = new ExtensionDerivedConfig;
//		initialExtensionConfig = new ExtensionDerivedConfig;

		extensionConfig->SetFileName(configPath.c_str());
		extensionConfig->Serialize(true, configPath.c_str(), theApp.GetVersionStr().c_str(), true, 0);
//		savedConfig = (ExtensionDerivedConfig*)extensionConfig->Clone();

		mainDialog->SetConfig(extensionConfig);
//		mainDialog = new MainDialog();
		TakionMainWnd* mainWnd = TGUI_GetTakionMainWnd();//mainDialog->GetMainWnd()

		mainDialog->ApplyConfig(0);
		mainDialog->CreateModeless(mainWnd);

		messageWnd = new TakionPopupWnd(mainWnd, true);
		messageWnd->AddCheckBox("CheckBox", true, false, false, false);

		messageBox = new TakionMessageBox(mainWnd, true);
		messageBox->AddCheckBox("CheckBox", false, true, false, false);

		stockPairDialog = new DialogStockPair(mainWnd);
		stockPairDialog->CreateModeless(mainWnd);
//		stockPairDialog->UpdateSystemRects();

		signedPriceValueDialog = new DialogSignedPriceValue(mainWnd);
		signedPriceValueDialog->CreateModeless(mainWnd);
	}
}

void WINAPI ExtensionTerminate()
{
//Cleanup commands
	SampleDialogSettingCommand::CleanupCommands();
//
	if(mainDialog)
	{
		if(messageWnd)
		{
			if(messageWnd->m_hWnd)
			{
				messageWnd->DestroyWindow();
//			delete messageWnd;//destruction is done in DestroyWindow by the operating system
			}
			else
			{
				delete messageWnd;
			}
			messageWnd = nullptr;
		}

		if(messageBox)
		{
			messageBox->DestroyWindow();
			delete messageBox;
			messageBox = nullptr;
		}

		if(stockPairDialog)
		{
			stockPairDialog->DestroyWindow();
			delete stockPairDialog;
			stockPairDialog = nullptr;
		}
		if(signedPriceValueDialog)
		{
			signedPriceValueDialog->DestroyWindow();
			delete signedPriceValueDialog;
			signedPriceValueDialog = nullptr;
		}

		if(marketSorterFilterDialog)
		{
			marketSorterFilterDialog->DestroyWindow();
			delete marketSorterFilterDialog;
			marketSorterFilterDialog = nullptr;
		}

		mainDialog->DestroyWindow();
		delete mainDialog;
		mainDialog = nullptr;
/*
		if(extensionConfig)
		{
			delete extensionConfig;
			extensionConfig = nullptr;
		}
*/
/*
		if(initialExtensionConfig)
		{
			delete initialExtensionConfig;
			initialExtensionConfig = nullptr;
		}

		if(savedConfig)
		{
			delete savedConfig;
			savedConfig = nullptr;
		}
*/
	}
}

void WINAPI CurrentAccountSet(Account* account)
{
	if(mainDialog)
	{
		mainDialog->CurrentAccountSet(account);
	}
}

void WINAPI OpenLayout()
{
	if(mainDialog)
	{
		mainDialog->OpenDlgLayout();
/*
		const std::string& configFile = theApp.GetConfigFilePath();
		if(!configFile.empty())
		{
			TakionMainWnd* mainWnd = TGUI_GetTakionMainWnd();//mainDialog->GetMainWnd()
			std::string files("Layout Files (*.");
			files += configExtension;
			files += ")|*.";
			files += configExtension;
			files += "|All Files (*.*)|*.*||";

			std::string title("Load ");
			const std::string& product = theApp.GetProductName();
			if(product.empty())
			{
				title += "Extension";
			}
			else
			{
				title += product;
			}
			title += " Layout";

			TakionFileDialog fileDlg(mainWnd, true, configExtension, configFile.c_str(),
				OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
				files.c_str(), title.c_str(), mainDialog);
			if(fileDlg.DoModal() == IDOK)
			{
				std::string pathName = fileDlg.GetPathName();
				extensionConfig->Update();
				unsigned int flags = 0;
				extensionConfig->Serialize(true, pathName.c_str(), theApp.GetVersionStr().c_str(), false, flags);
	//			ApplyConfig(fileDlg.GetFlags(), false);
				mainDialog->ApplyConfig(flags);
			}
		}
*/
	}
}

void WINAPI SaveLayout()
{
	if(mainDialog)
	{
		mainDialog->SaveDlgLayout();
	}
/*
	extensionConfig->Update();
	if(*extensionConfig != *savedConfig)
	{
		const std::string& fileNameStr = extensionConfig->GetOpenFileName();
		const char* fileName = fileNameStr.c_str();
		unsigned int flags = 0;
		extensionConfig->Serialize(false, fileName, theApp.GetVersionStr().c_str(), true, flags);
		*savedConfig = *extensionConfig;
	}
*/
}

void WINAPI RestoreLayout()
{
	if(mainDialog)
	{
		mainDialog->RestoreDlgLayout();
	}
/*
	extensionConfig->Update();
	if(*extensionConfig != *savedConfig)
	{
		*extensionConfig = *savedConfig;
		unsigned int flags = 0;
		mainDialog->ApplyConfig(flags);
	}
*/
}

const char* WINAPI GetLayoutFilePath()
{
	const std::string& configFile = theApp.GetConfigFilePath();
	return configFile.empty() ? nullptr : configFile.c_str();
}

void WINAPI SaveLayoutAs()
{
	if(mainDialog)
	{
		mainDialog->SaveDlgLayoutAs();
/*
		const std::string& configFile = theApp.GetConfigFilePath();
		if(!configFile.empty())
		{
			std::string files("Layout Files (*.");
			files += configExtension;
			files += ")|*.";
			files += configExtension;
			files += "|All Files (*.*)|*.*||";

			std::string title("Save ");
			const std::string& product = theApp.GetProductName();
			if(product.empty())
			{
				title += "Extension";
			}
			else
			{
				title += product;
			}
			title += " Layout";

			TakionMainWnd* mainWnd = TGUI_GetTakionMainWnd();//mainDialog->GetMainWnd()
			TakionFileDialog fileDlg(mainWnd, false, configExtension, configFile.c_str(),
				OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_EXTENSIONDIFFERENT,
				files.c_str(), title.c_str(), mainDialog);
			if(fileDlg.DoModal() == IDOK)
			{
				std::string pathName = fileDlg.GetPathName();
				extensionConfig->Update();
				unsigned int flags = 0;
				extensionConfig->Serialize(false, pathName.c_str(), theApp.GetVersionStr().c_str(), true, flags);
				*savedConfig = *extensionConfig;
				savedConfig->SetFileName(pathName.c_str());
			}
		}
*/
	}
}

CWnd* WINAPI TakionCreateExtensionTool(const char* id, const CBitmap* bitmap, unsigned int workspace, bool pinned)
{
//	return m_CreateExtensionToolFunction(id, bitmap, workspace, pinned);
	if(!strcmp(id, StockComparator::stockComparatorId))
	{
		return new StockComparator(bitmap, workspace, pinned);
	}
	if(!strcmp(id, MyMarketSorter::myMarketSorterId))
	{
		return new MyMarketSorter(bitmap, workspace, pinned);
	}
	return nullptr;
}

#ifdef __cplusplus
}
#endif
//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CExtensionSampleApp
/*
BEGIN_MESSAGE_MAP(CExtensionSampleApp, CWinApp)
END_MESSAGE_MAP()
*/

// ExtensionSample construction

ExtensionSample::ExtensionSample():
	m_versionNum(0),
	m_buildDateTime(U_ConvertStringDateTimeToUInt64(__DATE__, __TIME__))
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// CExtensionSampleApp initialization

BOOL ExtensionSample::InitInstance()
{
	CWinApp::InitInstance();
	U_InitializeInstanceInfo(m_hInstance, m_filePathAndName, m_fileDescription, m_platform, m_versionNum, m_versionStr);

//	U_QueryVersionResourceValue(m_filePathAndName.c_str(), "ProductName", m_productName);

	char path[MAX_PATH];
	strcpy_s(path, sizeof(path), m_filePathAndName.c_str());
	char* p = strrchr(path, '\\');
	if(!p)
	{
		p = path + strlen(path);
		*p = '\0';
	}
	else
	{
		*(p + 1) = '\0';
	}
	m_dllPath = path;

	m_configFilePath = m_dllPath;
	m_configFilePath += "ExtensionSample.";//ecg";
	m_configFilePath += configExtension;

//	U_QueryVersionResourceValue(m_filePathAndName.c_str(), "ProductName", m_productName);

/*
	std::string configFile;
	bool result = GetModuleFileName(theApp.m_hInstance, path, sizeof(path)) > 0 && strlen(path) < sizeof(path);

	char path[MAX_PATH];
	char absolutePath[MAX_PATH];
	char* exeFileName;
	DWORD ret = GetFullPathName(path, sizeof(absolutePath), absolutePath, &exeFileName);
	if(ret)
	{
		strcpy_s(path, sizeof(path), absolutePath);
	}
	if(result)//U_GetModulePath(m_hInstance, path, sizeof(path)))
	{
		char* p = strrchr(path, '\\');
		if(!p)
		{
			p = path + strlen(path);
			*p = '\0';
		}
		else
		{
			*(p + 1) = '\0';
		}
		m_dllPath = path;
	}
*/
	return TRUE;
}

int ExtensionSample::ExitInstance()
{
	int ret = CWinApp::ExitInstance();
	return ret;
}

void ExtensionSample::SetInfo(const char* info)
{
	TGUI_UpdateExtensionRow(m_hInstance);
/*
	if(info)
	{
		if(strcmp(info, m_info.c_str()))
		{
			m_info = info;
			TGUI_UpdateExtensionRow(m_hInstance);
		}
	}
	else if(!m_info.empty())
	{
		m_info.clear();
		TGUI_UpdateExtensionRow(m_hInstance);
	}
*/
}

MyDialogMarketSorterFilter* ExtensionSample::GetMarketSorterFilterDialog()
{
	if(!marketSorterFilterDialog)
	{
		marketSorterFilterDialog = new MyDialogMarketSorterFilter(TGUI_GetTakionMainWnd(), true, nullptr, nullptr);
	}
	if(!marketSorterFilterDialog->m_hWnd)
	{
		marketSorterFilterDialog->CreateModeless(TGUI_GetTakionMainWnd());
		marketSorterFilterDialog->UpdateSystemRects();
	}
	return marketSorterFilterDialog;
}


