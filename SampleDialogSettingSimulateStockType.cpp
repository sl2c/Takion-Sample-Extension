#include "stdafx.h"
#include "SampleDialogSettingSimulateStockType.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateStockType::SampleDialogSettingSimulateStockType(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateSource(mainWnd, parentTab, "StockType"),
	m_ComboBoxSecurityType(DT_CENTER),
	m_securityType(STYPE_UNDEFINED)
{
	m_ComboBoxSecurityType.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_bottomControl += vgap;
	int sideWidth = 32;
//	const int priceWidth = 65;
	const int column2 = m_firstColumnWidth / 2;

	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, column2, 300, comboBoxSimStockTypeId);
	AddStaticIndirect("StockType", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth - column2, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth + column2, staticHeight, staticSimStockTypeId);

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	m_bottomControl += comboBoxHeight + vgap;
/*
	controlWidth = m_xc + priceWidth;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimLrpLuldPriceLowId);
	AddStaticIndirect("Low", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimLrpLuldPriceLowId);

	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimLrpLuldPriceHighId);
	AddStaticIndirect("High", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimLrpLuldPriceHighId);

	m_bottomControl += spinHeight;

	controlWidth += sideWidth;
*/
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}

BOOL SampleDialogSettingSimulateStockType::OnInitDialog()
{
	m_securityType = STYPE_UNDEFINED;

	BOOL ret = BaseDialogSettingSimulateSource::OnInitDialog();
/*	
//	m_SpinPriceLow.SetBkColorEnabled(m_takionVisual->GetWarningColor());
//	m_SpinPriceHigh.SetBkColorEnabled(m_takionVisual->GetWarningColor());
	m_ComboBoxSecurityType.AddItem(STYPE_UNDEFINED, "Undefined");
	m_ComboBoxSecurityType.AddItem(STYPE_ETF, "ETF");
	m_ComboBoxSecurityType.AddItem(STYPE_COMMON_STOCK, "Common Stock");
	m_ComboBoxSecurityType.AddItem(STYPE_PREFERRED_STOCK, "Preferred Stock");
	m_ComboBoxSecurityType.AddItem(STYPE_WARRANT, "Warrant");
	m_ComboBoxSecurityType.AddItem(STYPE_RIGHT, "Right");
	m_ComboBoxSecurityType.AddItem(STYPE_CORPORATE_BOND, "Corporate Bond");
	m_ComboBoxSecurityType.AddItem(STYPE_TREASURY_BOND, "Treasury Bond");
	m_ComboBoxSecurityType.AddItem(STYPE_STRUCTURED_PRODUCT, "Structured Product");
	m_ComboBoxSecurityType.AddItem(STYPE_ADR_COMMON, "ADR Common");
	m_ComboBoxSecurityType.AddItem(STYPE_ADR_PREFERRED, "ADR Preferred");
	m_ComboBoxSecurityType.AddItem(STYPE_ADR_WARRANT, "ADR Warrant");
	m_ComboBoxSecurityType.AddItem(STYPE_ADR_RIGHT, "ADR Right");
	m_ComboBoxSecurityType.AddItem(STYPE_ADR_CORPORATE_BOND, "ADR Corporate Bond");
	m_ComboBoxSecurityType.AddItem(STYPE_NY_REGISTERED_SHARE, "NY Registered Share");
	m_ComboBoxSecurityType.AddItem(STYPE_GLOBAL_REGISTERED_SHARE, "Global Registered Share");
	m_ComboBoxSecurityType.AddItem(STYPE_INDEX, "Index");
	m_ComboBoxSecurityType.AddItem(STYPE_FUND, "Fund");
	m_ComboBoxSecurityType.AddItem(STYPE_BASKET, "Basket");
	m_ComboBoxSecurityType.AddItem(STYPE_UNIT, "Unit");
	m_ComboBoxSecurityType.AddItem(STYPE_LIQUIDATING_TRUST, "Liquidating Trust");
	m_ComboBoxSecurityType.AddItem(STYPE_UNKNOWN, "Unknown");
*/
	m_ComboBoxSecurityType.AddItem(STYPE_UNDEFINED, "Undefined");
	m_ComboBoxSecurityType.AddItem(STYPE_ETF, "ETF");

	m_ComboBoxSecurityType.AddItem(STYPE_ADR, "ADR");
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ComboBoxSecurityType.AddItem(STYPE_PREFERRED_STOCK, "(Preferred)");
#endif
	m_ComboBoxSecurityType.AddItem(STYPE_COMMON_STOCK, "Common");
	m_ComboBoxSecurityType.AddItem(STYPE_DEBENTURES, "Debentures");
	m_ComboBoxSecurityType.AddItem(STYPE_ETFE, "ETF");
	m_ComboBoxSecurityType.AddItem(STYPE_FOREIGN, "Foreign");
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ComboBoxSecurityType.AddItem(STYPE_STRUCTURED_PRODUCT, "(Structured)");
#endif
	m_ComboBoxSecurityType.AddItem(STYPE_US_DEPOSITARY_SHARES, "US Depository");
	m_ComboBoxSecurityType.AddItem(STYPE_UNITS, "Units");
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ComboBoxSecurityType.AddItem(STYPE_ADR_WARRANT, "(ADR Warrant)");
	m_ComboBoxSecurityType.AddItem(STYPE_ADR_RIGHT, "(ADR Right)");
#endif
	m_ComboBoxSecurityType.AddItem(STYPE_INDEX_LINKED_NOTES, "Index Note");
	m_ComboBoxSecurityType.AddItem(STYPE_MISC_LIQUID_TRUST, "Liquid Trust");
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ComboBoxSecurityType.AddItem(STYPE_GLOBAL_REGISTERED_SHARE, "(GlobalRegShare)");
#endif
	m_ComboBoxSecurityType.AddItem(STYPE_ORDINARY_SHARES, "Ordinary");
	m_ComboBoxSecurityType.AddItem(STYPE_PREF_STOCK, "Preferred");
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ComboBoxSecurityType.AddItem(STYPE_BASKET, "(Basket)");
#endif
	m_ComboBoxSecurityType.AddItem(STYPE_RIGHTS, "Right");
	m_ComboBoxSecurityType.AddItem(STYPE_SHARES_OF_BENEFICIARY_INTEREST, "Benefit Interest");
	m_ComboBoxSecurityType.AddItem(STYPE_TEST, "Test");
	m_ComboBoxSecurityType.AddItem(STYPE_CLOSED_END_FUND, "Cls End Fund");
	m_ComboBoxSecurityType.AddItem(STYPE_WARRANT, "Warrant");

	m_ComboBoxSecurityType.SetCurSel(0);
	OnSelchangeSecurityType();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateStockType::UpdateSettings()
{
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simStockTypeSymbolType.SetValue(symbolType);

	extensionConfig->m_simStockTypeSymbol.SetValue(m_symbol);
	extensionConfig->m_simStockTypeSymbolMap.SetValue(m_symbolMap);

//	unsigned char source = m_source;
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(m_source);
	extensionConfig->m_simStockTypeSource.SetValue(source);

//	unsigned char messageType = m_messageType;
//	if(messageType >= MT_Count)messageType = MT_Count - 1;
	extensionConfig->m_simStockTypeSecurityType.SetValue(m_securityType);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateStockType::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simStockTypeSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simStockTypeSymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simStockTypeSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

//	unsigned char source = extensionConfig->m_simStockTypeSource.GetValue();
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(extensionConfig->m_simStockTypeSource.GetValue());
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}

	char securityType = extensionConfig->m_simStockTypeSecurityType.GetValue();
//	if(messageType >= MT_Count)messageType = MT_Count - 1;
	if(securityType != m_securityType)
	{
		m_securityType = securityType;
		int found = m_ComboBoxSecurityType.FindItemByCharName(securityType);
		if(found < 0)found = 0;
		m_ComboBoxSecurityType.SetCurSel(found);
	}

	EnableApplyButton(false);
	EnableSendMessageButton();

	m_modified = false;
}

const Message* SampleDialogSettingSimulateStockType::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
		switch(m_source)
		{
			case ST_MARKET_DATA:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				m_securityTypeMd.m_indicator = m_securityType;
				U_CopyAndPad(m_securityTypeMd.m_symbol, sizeof(m_securityTypeMd.m_symbol), m_symbol.c_str(), '\0');
				m_securityTypeMd.m_time = TL_GetCurrentMillisecond();
				return &m_securityTypeMd;
			}
			else
			{
				m_securityTypeMdSm.m_indicator = m_securityType;
				m_securityTypeMdSm.m_symbol = m_symbolMap;
				m_securityTypeMdSm.m_time = TL_GetCurrentMillisecond();
				return &m_securityTypeMdSm;
			}
			break;

			case ST_MARKET_SORTER:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				m_securityTypeMs.m_indicator = m_securityType;
				U_CopyAndPad(m_securityTypeMs.m_symbol, sizeof(m_securityTypeMs.m_symbol), m_symbol.c_str(), '\0');
				m_securityTypeMs.m_time = TL_GetCurrentMillisecond();
				return &m_securityTypeMs;
			}
			else
			{
				m_securityTypeMsSm.m_indicator = m_securityType;
				m_securityTypeMsSm.m_symbol = m_symbolMap;
				m_securityTypeMsSm.m_time = TL_GetCurrentMillisecond();
				return &m_securityTypeMsSm;
			}
			break;
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulateStockType::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSource::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxSimStockTypeId, m_ComboBoxSecurityType);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateStockType, BaseDialogSettingSimulateSource)
	ON_CBN_SELCHANGE(comboBoxSimStockTypeId, OnSelchangeSecurityType)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateStockType::OnSelchangeSecurityType()
{
	int sel = m_ComboBoxSecurityType.GetCurSel();
	if(sel < 0)sel = 0;//MT_LRP;

	const void* item = m_ComboBoxSecurityType.GetItemDataPtr(sel);
	const char securityType = *m_ComboBoxSecurityType.GetItemName(item);
	if(securityType != m_securityType)
	{
		m_securityType = securityType;
		m_modified = true;
	}
}
