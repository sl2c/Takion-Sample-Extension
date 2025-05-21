#include "stdafx.h"
#include "SampleDialogSettingCommand.h"
#include "ExtensionDerivedConfig.h"
//#include "ConnectionApi.h"

extern ExtensionDerivedConfig* extensionConfig;

class MyCommand1 : public Command
{
public:
	MyCommand1():Command("MyCommand1", CC_NAVIGATE)
	{
	}
	virtual void Invoke(TakionTool* activeTool,
		TakionTool* activeCommandTool,
		bool repeat,
		bool aggregate = false,
		const CommandInfo* commandInfo = nullptr) override;
/*
	virtual void InvokeForSecurity(TakionTool* activeTool,
		Account* account,
		const Security* security,
		const Position* position,
		unsigned int sendId,
		unsigned int tierSize,
		int count,
		bool closeout,
		bool closeOnly,
		bool passiveOnly,
		unsigned char inventoryMode,
		const CommandInfo* commandInfo)
	{
		Invoke(activeTool, TGUI_GetActiveCommandTool(), false, true, commandInfo);
	}
*/
	virtual Command* CreateDeepCopy() const override;
};

class MyCommand2 : public Command
{
public:
	MyCommand2():Command("MyCommand2", CC_NAVIGATE)
	{
	}
	virtual void Invoke(TakionTool* activeTool,
		TakionTool* activeCommandTool,
		bool repeat,
		bool aggregate = false,
		const CommandInfo* commandInfo = nullptr) override;
/*
	virtual void InvokeForSecurity(TakionTool* activeTool,
		Account* account,
		const Security* security,
		const Position* position,
		unsigned int sendId,
		unsigned int tierSize,
		int count,
		bool closeout,
		bool closeOnly,
		bool passiveOnly,
		unsigned char inventoryMode,
		const CommandInfo* commandInfo)
	{
		Invoke(activeTool, TGUI_GetActiveCommandTool(), false, true, commandInfo);
	}
*/
	virtual Command* CreateDeepCopy() const override;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void MyCommand1::Invoke(TakionTool* activeTool,
	TakionTool* activeCommandTool,
	bool repeat,
	bool aggregate,
	const CommandInfo* commandInfo)
{
//	Connection connection(5);//reconnectSeconds
//	connection.AddConnectionLine("10.20.30.40", 1234);//ip, port. you can add more than 1 line.
//	TD_ConnectToMarketSorter(&connection, true);
	struct IpPortStruct line = {"10.2.88.217", 7400, nullptr, 0};
	TD_ConnectToMarketSorter(&line, 1, 5, true);
}

Command* MyCommand1::CreateDeepCopy() const
{
	return new MyCommand1(*this);
}

void MyCommand2::Invoke(TakionTool* activeTool,
	TakionTool* activeCommandTool,
	bool repeat,
	bool aggregate,
	const CommandInfo* commandInfo)
{
	TD_DisconnectFromMarketSorter();
}

Command* MyCommand2::CreateDeepCopy() const
{
	return new MyCommand2(*this);
}

enum SampleSettingCommandIds
{
	buttonAddCommandId = 10000,
	buttonRemoveCommandId,
	buttonMapCommandId,
	buttonUnmapCommandId,

	checkBoxOverridePreviousMappingId,

	staticCommandNameId,
	editCommandNameId,
	staticSymbolId,
	editSymbolId,
	checkBoxCommandPostId,
	buttonCommandExecuteId,

	commandIdsCount
};

SampleDialogSettingCommand::SampleDialogSettingCommand(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSetting(mainWnd, parentTab, "Command"),
	m_CheckBoxOverridePreviousMapping("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_EditSymbol(U_GetValidStockChars(), false),
	m_CheckBoxExecuteCommandPost("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_overridePreviousMapping(false),
	m_commandPost(false)
{
//	m_ListBoxConnection.SetSelectable(false);

	const int hgap = 2;
	const int vgap = 1;
	int x = hgap;
	int y = vgap;
	int width = 100;
	const int checkBoxHeight = 8;
	const int buttonHeight = 12;
	const int buttonGap = buttonHeight + vgap;

	AddButtonIndirect("Add Command", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y, width, buttonHeight, buttonAddCommandId);
	AddButtonIndirect("Remove Command", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += buttonGap, width, buttonHeight, buttonRemoveCommandId);
	AddButtonIndirect("Map Command", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += buttonGap, width, buttonHeight, buttonMapCommandId);
	AddButtonIndirect("Override Previous Mapping", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, x + width + hgap, y + (buttonHeight - checkBoxHeight) / 2, width * 2, checkBoxHeight, checkBoxOverridePreviousMappingId);
	AddButtonIndirect("Unmap Command", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += buttonGap, width, buttonHeight, buttonUnmapCommandId);

	const int width2 = width / 2;
	AddStaticIndirect("Command Name", WS_VISIBLE | SS_NOPREFIX, 0, x, y += 3 * buttonGap, width, staticHeight, staticCommandNameId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, x, y += staticHeight, width, editHeight, editCommandNameId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, x, y += editHeight + vgap, width2, editHeight, editSymbolId);
	AddStaticIndirect("Symbol", WS_VISIBLE | SS_NOPREFIX, 0, x + width2, y + (editHeight - staticHeight) / 2, width2, staticHeight, staticSymbolId);
	AddButtonIndirect("Post", WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP, 0, x, y += editHeight + vgap, checkBoxHeight * 4, checkBoxHeight, checkBoxCommandPostId);
	AddButtonIndirect("Execute Command", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y += checkBoxHeight + vgap, width, buttonHeight, buttonCommandExecuteId);
}

HWND SampleDialogSettingCommand::GetFirstTabControl() const
{
	return m_ButtonAddCommand.m_hWnd;
}
////
void SampleDialogSettingCommand::RestoreLayout()
{
	m_commandPost = extensionConfig->m_commandPost.GetValue();
	m_CheckBoxExecuteCommandPost.SetCheck(m_commandPost ? BST_CHECKED : BST_UNCHECKED);

//	const std::string& name = extensionConfig->m_commandName.GetValue();
	m_EditCommandName.SetWindowText(extensionConfig->m_commandName.GetValue().c_str());
	m_EditSymbol.SetWindowText(extensionConfig->m_commandSymbol.GetValue().c_str());

	m_modified = false;
}

void SampleDialogSettingCommand::UpdateSettings()
{
	extensionConfig->m_commandPost.SetValue(m_commandPost);//m_CheckBoxExecuteCommandPost.GetCheck() == BST_CHECKED);
//	CString path;
//	m_EditSymbol.GetWindowText(path);
	extensionConfig->m_commandName.SetValue(m_commandNameToExecute);
	extensionConfig->m_commandSymbol.SetValue(m_symbol);
}

void SampleDialogSettingCommand::BeforeDestroy()
{
	m_overridePreviousMapping = false;
	m_commandNameToExecute.clear();
	m_symbol.clear();

	BaseDialogSetting::BeforeDestroy();
}

BOOL SampleDialogSettingCommand::OnInitDialog()
{
	m_overridePreviousMapping = false;
	m_commandNameToExecute.clear();
	m_symbol.clear();

	BOOL ret = BaseDialogSetting::OnInitDialog();

	m_EditSymbol.SetLimitText(7);
	m_CheckBoxOverridePreviousMapping.ResizeToFitText(0);
	m_CheckBoxExecuteCommandPost.ResizeToFitText(0);

	return ret;
}

void SampleDialogSettingCommand::ToolTipsEnabled(bool enable)
{
	BaseDialogSetting::ToolTipsEnabled(enable);
	m_EditSymbol.EnableTooltips(enable, m_toolTip);
	m_CheckBoxOverridePreviousMapping.EnableTooltips(enable, m_toolTip);
	m_CheckBoxExecuteCommandPost.EnableTooltips(enable, m_toolTip);
}

void SampleDialogSettingCommand::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
	DDX_Control(pDX, buttonAddCommandId, m_ButtonAddCommand);
	DDX_Control(pDX, buttonRemoveCommandId, m_ButtonRemoveCommand);
	DDX_Control(pDX, buttonMapCommandId, m_ButtonMapCommand);
	DDX_Control(pDX, buttonUnmapCommandId, m_ButtonUnmapCommand);
	DDX_Control(pDX, checkBoxOverridePreviousMappingId, m_CheckBoxOverridePreviousMapping);

	DDX_Control(pDX, editCommandNameId, m_EditCommandName);
	DDX_Control(pDX, editSymbolId, m_EditSymbol);
	DDX_Control(pDX, checkBoxCommandPostId, m_CheckBoxExecuteCommandPost);
	DDX_Control(pDX, buttonCommandExecuteId, m_ButtonExecuteCommand);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingCommand, BaseDialogSetting)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(buttonAddCommandId, OnAddCommand)
	ON_BN_CLICKED(buttonRemoveCommandId, OnRemoveCommand)
	ON_BN_CLICKED(buttonMapCommandId, OnMapCommand)
	ON_BN_CLICKED(buttonUnmapCommandId, OnUnmapCommand)
	ON_BN_CLICKED(checkBoxOverridePreviousMappingId, OnOverridePreviousMapping)

	ON_EN_CHANGE(editCommandNameId, OnCommandNameChange)
	ON_EN_CHANGE(editSymbolId, OnSymbolChange)
	ON_EN_UPDATE(editSymbolId, OnSymbolUpdate)
	ON_BN_CLICKED(checkBoxCommandPostId, OnCommandPost)
	ON_BN_CLICKED(buttonCommandExecuteId, OnCommandExecute)
END_MESSAGE_MAP()

HBRUSH SampleDialogSettingCommand::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editCommandNameId:
		if(m_commandNameToExecute.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		break;

	}
	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}

void SampleDialogSettingCommand::OnCommandNameChange()
{
	bool empty = m_commandNameToExecute.empty();
	CString text;
	m_EditCommandName.GetWindowText(text);
	m_commandNameToExecute = text;
	if(m_commandNameToExecute.empty() != empty)
	{
		m_EditCommandName.Invalidate(FALSE);
		EnableExecuteButton();
	}
	m_modified = true;
}

void SampleDialogSettingCommand::EnableExecuteButton()
{
	EnableControl(m_ButtonExecuteCommand.m_hWnd, !m_commandNameToExecute.empty(), m_EditCommandName.m_hWnd);
}

void SampleDialogSettingCommand::OnSymbolChange()
{
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_symbol = text;
//	m_numericSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(m_symbol.c_str(), sizeof(unsigned __int64) - 1);
	m_modified = true;
}

void SampleDialogSettingCommand::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void SampleDialogSettingCommand::OnCommandExecute()
{
	if(!m_commandNameToExecute.empty())
	{
		if(m_symbol.empty())
		{
			TU_ExecuteCommand(m_commandNameToExecute.c_str(), m_commandPost);
		}
		else
		{
			Account* account = TD_GetCurrentAccount();
			if(account)
			{
				const Security* security = TD_ObtainSortableEquity(m_symbol.c_str(), false);
				if(!security || !security->isLoaded())
				{
					security = TD_ObtainStock(m_symbol.c_str(), true);
				}
				if(security && security->isLoaded())
				{
//					const Position* position = account->FindPosition(security);
					TU_ExecuteCommandForSecurity(m_commandNameToExecute.c_str(),
						m_commandPost,
						security,
						account,
						nullptr,//position,
						0,//unsigned int sendId,
						security->GetTierSize(),//unsigned int tierSize,
						Price::priceZero,
						Price::priceZero,
						false,
						Price::priceZero,
						Price::priceZero,
						OST_NONE,
						OSB_PRINT,
						SignedPrice::signedPriceZero,
						account->GetCurrentOcoId(),
						0,//Calculate by Command
						1,//int count,
						false,//bool closeout,
						false,//bool closeOnly,
						false,//bool passiveOnly,
						true,//bool blockAggressiveOrders
						OCINV_DEFAULT);//0);//unsigned char inventoryMode);
				}
			}
		}
	}
}

void SampleDialogSettingCommand::OnCommandPost()
{
	m_commandPost = m_CheckBoxExecuteCommandPost.GetCheck() == BST_CHECKED;
}

void SampleDialogSettingCommand::OnOverridePreviousMapping()
{
	const bool overridePreviousMapping = m_CheckBoxOverridePreviousMapping.GetCheck() == BST_CHECKED;
	if(overridePreviousMapping != m_overridePreviousMapping)
	{
		m_overridePreviousMapping = overridePreviousMapping;
	}
}

void SampleDialogSettingCommand::OnAddCommand()
{
	Command* command = new MyCommand1();
	if(!TGUI_AddCommand(command))
	{
		delete command;
	}
	command = new MyCommand2();
	if(!TGUI_AddCommand(command))
	{
		delete command;
	}
}

void SampleDialogSettingCommand::CleanupCommands()
{
	Command* command = TGUI_RemoveCommand("MyCommand1");
	if(command)
	{
		delete command;
	}
	command = TGUI_RemoveCommand("MyCommand2");
	if(command)
	{
		delete command;
	}
}

void SampleDialogSettingCommand::OnRemoveCommand()
{
	CleanupCommands();
}

void SampleDialogSettingCommand::OnMapCommand()
{
	Command* command = TGUI_FindCommand("MyCommand1");
	if(command)
	{
		Key key('A',//unsigned char ascii
			true,//bool leftShift
			true,//bool leftCtrl
			true,//bool leftAlt,
			false,//bool rightShift
			false,//bool rightCtrl
			false,//bool rightAlt
			false,//bool capsLock
			false);//bool numLock):
		TGUI_MapCommand(key, command, m_overridePreviousMapping);
	}
	command = TGUI_FindCommand("MyCommand2");
	if(command)
	{
		Key key('A',//unsigned char ascii
			false,//bool leftShift
			false,//bool leftCtrl
			false,//bool leftAlt,
			true,//bool rightShift
			true,//bool rightCtrl
			true,//bool rightAlt
			false,//bool capsLock
			false);//bool numLock):
		TGUI_MapCommand(key, command, m_overridePreviousMapping);
	}
}

void SampleDialogSettingCommand::OnUnmapCommand()
{
	Key key1('A',//unsigned char ascii
		true,//bool leftShift
		true,//bool leftCtrl
		true,//bool leftAlt,
		false,//bool rightShift
		false,//bool rightCtrl
		false,//bool rightAlt
		false,//bool capsLock
		false);//bool numLock):

	TGUI_UnmapKey(key1);

	Key key2('A',//unsigned char ascii
		false,//bool leftShift
		false,//bool leftCtrl
		false,//bool leftAlt,
		true,//bool rightShift
		true,//bool rightCtrl
		true,//bool rightAlt
		false,//bool capsLock
		false);//bool numLock):

	TGUI_UnmapKey(key2);
}

