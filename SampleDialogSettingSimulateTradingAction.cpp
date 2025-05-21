#include "stdafx.h"
#include "SampleDialogSettingSimulateTradingAction.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateTradingAction::SampleDialogSettingSimulateTradingAction(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateSource(mainWnd, parentTab, "TradingAction"),
	m_ComboBoxTradingState(DT_CENTER),
	m_tradingActionReason(0),
	m_tradingState('T')
{
	m_ComboBoxTradingState.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_bottomControl += vgap;
	int sideWidth = 32;
//	const int priceWidth = 65;
	const int column2 = m_firstColumnWidth / 2 + staticHeight;

	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, column2, 300, comboBoxSimTradingActionId);
//	AddStaticIndirect("TradingState", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth - column2, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth + column2, staticHeight, staticSimTradingActionId);
	AddStaticIndirect("TradingState", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + column2, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth + column2, staticHeight, staticSimTradingActionId);

	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, column2, editHeight, editSimTradingActionReasonId);
//	AddStaticIndirect("Trading Action Reason", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth - column2, m_bottomControl + (editHeight - staticHeight) / 2, sideWidth + column2, staticHeight, staticSimTradingActionReasonId);
	AddStaticIndirect("Trading Action Reason", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + column2, m_bottomControl + (editHeight - staticHeight) / 2, sideWidth + column2, staticHeight, staticSimTradingActionReasonId);

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	m_bottomControl += comboBoxHeight + vgap;

	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}

BOOL SampleDialogSettingSimulateTradingAction::OnInitDialog()
{
	m_tradingActionReason = 0;
	m_tradingState = 'T';

	BOOL ret = BaseDialogSettingSimulateSource::OnInitDialog();
	
	m_EditTradingActionReason.SetLimitText(sizeof(unsigned int));

//	m_SpinPriceLow.SetBkColorEnabled(m_takionVisual->GetWarningColor());
//	m_SpinPriceHigh.SetBkColorEnabled(m_takionVisual->GetWarningColor());
	m_ComboBoxTradingState.AddItem('T', "Trading");
	m_ComboBoxTradingState.AddItem('H', "Halted");
	m_ComboBoxTradingState.AddItem('E', "E");
	m_ComboBoxTradingState.AddItem('3', "3");
	m_ComboBoxTradingState.AddItem('2', "Halted");
	m_ComboBoxTradingState.AddItem('1', "Halted");
	m_ComboBoxTradingState.AddItem('P', "Halted");

	m_ComboBoxTradingState.SetCurSel(0);
	OnSelchangeTradingState();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateTradingAction::UpdateSettings()
{
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simTradingActionSymbolType.SetValue(symbolType);

	extensionConfig->m_simTradingActionSymbol.SetValue(m_symbol);
	extensionConfig->m_simTradingActionSymbolMap.SetValue(m_symbolMap);

//	unsigned char source = m_source;
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(m_source);
	extensionConfig->m_simTradingActionSource.SetValue(source);

//	unsigned char messageType = m_messageType;
//	if(messageType >= MT_Count)messageType = MT_Count - 1;
	extensionConfig->m_simTradingActionTradingState.SetValue(m_tradingState);
	extensionConfig->m_simTradingActionReason.SetValue(m_tradingActionReason);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateTradingAction::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simTradingActionSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simTradingActionSymbolType.GetValue();
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	if(symbolType != m_symbolType)
	{
		m_symbolType = symbolType;
		switch(m_symbolType)
		{
			case SYMT_SYMBOL_STRING:
			m_RadioSymbolMap.SetCheck(BST_UNCHECKED);
			m_RadioSymbol.SetCheck(BST_CHECKED);
			break;

			case SYMT_SYMBOL_MAPPING:
			m_RadioSymbol.SetCheck(BST_UNCHECKED);
			m_RadioSymbolMap.SetCheck(BST_CHECKED);
			break;
		}
		SymbolTypeChanged();
	}
	const unsigned short& symbolMap = extensionConfig->m_simTradingActionSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

//	unsigned char source = extensionConfig->m_simTradingActionSource.GetValue();
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(extensionConfig->m_simTradingActionSource.GetValue());
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}

	const char tradingState = extensionConfig->m_simTradingActionTradingState.GetValue();
//	if(messageType >= MT_Count)messageType = MT_Count - 1;
	if(tradingState != m_tradingState)
	{
		m_tradingState = tradingState;
		int found = m_ComboBoxTradingState.FindItemByCharName(tradingState);
		if(found < 0)found = 0;
		m_ComboBoxTradingState.SetCurSel(found);
		OnSelchangeTradingState();
	}

	const unsigned int& tradingActionReason = extensionConfig->m_simTradingActionReason.GetValue();
	if(tradingActionReason != m_tradingActionReason)
	{
		m_tradingActionReason = tradingActionReason;
		char buf[sizeof(unsigned int) + 1];
		*(unsigned int*)buf = m_tradingActionReason;
		buf[sizeof(unsigned int)] = '\0';
		m_EditTradingActionReason.SetWindowText(buf);
	}

	EnableApplyButton(false);
	EnableSendMessageButton();

	m_modified = false;
}

void SampleDialogSettingSimulateTradingAction::OnTradingActionReasonChange()
{
	CString text;
	m_EditTradingActionReason.GetWindowText(text);
	const unsigned int tradingActionReason = U_RepresentStringAsUnsignedNumber<unsigned int>(text);
	if(tradingActionReason != m_tradingActionReason)
	{
		m_tradingActionReason = tradingActionReason;
		m_modified = true;
	}
}

const Message* SampleDialogSettingSimulateTradingAction::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
		switch(m_source)
		{
			case ST_MARKET_DATA:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				m_tradingActionMd.m_tradingState = m_tradingState;
				*(unsigned int*)m_tradingActionMd.m_tradingActionReason = m_tradingActionReason;
				U_CopyAndPad(m_tradingActionMd.m_symbol, sizeof(m_tradingActionMd.m_symbol), m_symbol.c_str(), '\0');
				m_tradingActionMd.m_time = TL_GetCurrentMillisecond();
				return &m_tradingActionMd;
			}
			else
			{
				m_tradingActionMdSm.m_tradingState = m_tradingState;
				*(unsigned int*)m_tradingActionMdSm.m_tradingActionReason = m_tradingActionReason;
				m_tradingActionMdSm.m_symbol = m_symbolMap;
				m_tradingActionMdSm.m_time = TL_GetCurrentMillisecond();
				return &m_tradingActionMdSm;
			}
			break;

			case ST_MARKET_SORTER:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				m_tradingActionMs.m_tradingState = m_tradingState;
				*(unsigned int*)m_tradingActionMs.m_tradingActionReason = m_tradingActionReason;
				U_CopyAndPad(m_tradingActionMs.m_symbol, sizeof(m_tradingActionMs.m_symbol), m_symbol.c_str(), '\0');
				m_tradingActionMs.m_time = TL_GetCurrentMillisecond();
				return &m_tradingActionMs;
			}
			else
			{
				m_tradingActionMsSm.m_tradingState = m_tradingState;
				*(unsigned int*)m_tradingActionMsSm.m_tradingActionReason = m_tradingActionReason;
				m_tradingActionMsSm.m_symbol = m_symbolMap;
				m_tradingActionMsSm.m_time = TL_GetCurrentMillisecond();
				return &m_tradingActionMsSm;
			}
			break;
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulateTradingAction::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSource::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxSimTradingActionId, m_ComboBoxTradingState);
	DDX_Control(pDX, editSimTradingActionReasonId, m_EditTradingActionReason);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateTradingAction, BaseDialogSettingSimulateSource)
	ON_CBN_SELCHANGE(comboBoxSimTradingActionId, OnSelchangeTradingState)
	ON_EN_CHANGE(editSimTradingActionReasonId, OnTradingActionReasonChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateTradingAction::OnSelchangeTradingState()
{
	int sel = m_ComboBoxTradingState.GetCurSel();
	if(sel < 0)sel = 0;//MT_LRP;

	const void* item = m_ComboBoxTradingState.GetItemDataPtr(sel);
	const char tradingState = *m_ComboBoxTradingState.GetItemName(item);
	if(tradingState != m_tradingState)
	{
		m_tradingState = tradingState;
		m_modified = true;
	}
}
