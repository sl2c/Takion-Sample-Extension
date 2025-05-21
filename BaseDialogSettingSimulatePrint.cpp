#include "stdafx.h"
#include "BaseDialogSettingSimulatePrint.h"
#include "ExtensionDerivedConfig.h"

//extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulatePrint::BaseDialogSettingSimulatePrint(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseDialogSettingSimulateOptionSource(mainWnd, parentTab, label),
//	m_EditRef("0123456789ABCDEF", false),
	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinSize(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_size(0),
	m_saleCondition1('\0'),
	m_saleCondition2('\0'),
	m_saleCondition3('\0'),
	m_saleCondition4('\0'),
	m_marketCenter('\0'),
	m_marketListing('\0'),
	m_refNumberY(0)
{
/*
	m_messageStockPrint.m_symbol[sizeof(m_messageStockPrint.m_symbol) - 1] = '\0';
	m_messageStockPrint.m_flags = 0;
#ifndef TAKION_NO_OPTIONS
	m_messageOptionPrint.m_symbol[sizeof(m_messageOptionPrint.m_symbol) - 1] = '\0';
#endif
	m_messageEquityPrintLong.m_symbol[sizeof(m_messageEquityPrintLong.m_symbol) - 1] = '\0';
	m_messageEquityPrintShort.m_symbol[sizeof(m_messageEquityPrintShort.m_symbol) - 1] = '\0';
*/
	m_SpinPrice.AddSpinCell(1, false);
	m_SpinPrice.AddSpinCell(100, false);
	m_SpinPrice.AddSpinCell(1000000, true);
	m_SpinPrice.AddSpinCell(10000, true);

	m_SpinSize.AddSpinCell(1, false);
	m_SpinSize.AddSpinCell(100, false);
	m_SpinSize.AddSpinCell(1000, true);

	m_bottomControl += vgap;
	const int priceWidth = 75;
	const int staticWidth = 32;
	const int sizeWidth = 54;
	const int charWidth = 24;
	const int attrWidth = 32;


	m_refNumberY = m_bottomControl;

	int controlWidth = m_xc + priceWidth + hgap;

	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE | ES_RIGHT, 0, m_xc, m_bottomControl, priceWidth, editHeight, editSimPrintRefId);
	AddStaticIndirect("Ref# (Hex)", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (editHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintRefId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl += editHeight + vgap, priceWidth, spinHeight, spinSimPrintPriceId);
	AddStaticIndirect("Price", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintPriceId);
	controlWidth += staticWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + sizeWidth + hgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl += spinHeight + vgap, sizeWidth, spinHeight, spinSimPrintSizeId);
	AddStaticIndirect("Size", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintSizeId);
	controlWidth += staticWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + charWidth;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += spinHeight + vgap, charWidth, 250, comboPrintCondition1Id);
	AddStaticIndirect("SaleCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintCondition1Id);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, charWidth, 250, comboPrintCondition2Id);
	AddStaticIndirect("SaleCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintCondition2Id);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, charWidth, 250, comboPrintCondition3Id);
	AddStaticIndirect("SaleCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintCondition3Id);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, charWidth, 250, comboPrintCondition4Id);
	AddStaticIndirect("SaleCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintCondition4Id);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, charWidth, 250, comboPrintMarketCenterId);
	AddStaticIndirect("MarketCenter", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintMarketCenterId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, charWidth, 250, comboPrintMarketListingId);
	AddStaticIndirect("MarketListing", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintMarketListingId);

	m_bottomControl += comboBoxHeight;

	controlWidth += sizeWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}

BOOL BaseDialogSettingSimulatePrint::OnInitDialog()
{
	m_refNumber.clear();
	m_price.SetZero();
	m_size = 0;
	m_saleCondition1 = '\0';
	m_saleCondition2 = '\0';
	m_saleCondition3 = '\0';
	m_saleCondition4 = '\0';
	m_marketCenter = '\0';
	m_marketListing = '\0';

	BOOL ret = BaseDialogSettingSimulateOptionSource::OnInitDialog();
	m_EditRef.SetLimitText(16);
	m_SpinPrice.SetBkColorEnabled(m_visualBase->GetWarningColor());
	m_SpinSize.SetBkColorEnabled(m_visualBase->GetWarningColor());

	FillCharRange(m_ComboBoxSaleCondition1);
	FillCharRange(m_ComboBoxSaleCondition2);
	FillCharRange(m_ComboBoxSaleCondition3);
	FillCharRange(m_ComboBoxSaleCondition4);
	FillCharRange(m_ComboBoxMarketCenter);
	FillCharRange(m_ComboBoxMarketListing);

	m_ComboBoxSaleCondition1.AddString((const char*)'@');
	m_ComboBoxSaleCondition2.AddString((const char*)'@');
	m_ComboBoxSaleCondition3.AddString((const char*)'@');
	m_ComboBoxSaleCondition4.AddString((const char*)'@');
	m_ComboBoxSaleCondition1.AddString((const char*)' ');
	m_ComboBoxSaleCondition2.AddString((const char*)' ');
	m_ComboBoxSaleCondition3.AddString((const char*)' ');
	m_ComboBoxSaleCondition4.AddString((const char*)' ');

	return ret;
}

void BaseDialogSettingSimulatePrint::UpdateSettings()
{
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
//	extensionConfig->m_simPrintSymbolType.SetValue(symbolType);
	GetSettingSymbolType()->SetValue(symbolType);
//	extensionConfig->m_simPrintSymbol.SetValue(m_symbol);
	GetSettingSymbol()->SetValue(m_symbol);
//	extensionConfig->m_simPrintSymbolMap.SetValue(m_symbolMap);
	GetSettingSymbolMap()->SetValue(m_symbolMap);

	GetSettingPrintSource()->SetValue(m_source);

#ifndef TAKION_NO_OPTIONS
//	extensionConfig->m_simPrintOption.SetValue(m_optionMode);
	GetSettingUseOption()->SetValue(m_optionMode);
//	extensionConfig->m_simPrintOptionCall.SetValue(m_optionCall);
	GetSettingOptionCall()->SetValue(m_optionCall);
//	extensionConfig->m_simPrintOptionSuffix.SetValue(m_optionSuffix);
	GetSettingOptionSuffix()->SetValue(m_optionSuffix);
//	extensionConfig->m_simPrintOptionStrikePrice.SetValue(m_strikePrice);
	GetSettingOptionStrikePrice()->SetValue(m_strikePrice);
//	extensionConfig->m_simPrintOptionExpiration.SetValue(GetExpirationDate());
	GetSettingOptionExpiration()->SetValue(GetExpirationDate());
#endif
	unsigned __int64 refNumber;
	U_ReadHexUnsignedNumberFromString(refNumber, m_refNumber.c_str());
//	extensionConfig->m_simPrintRefNumber.SetValue(refNumber);
	GetSettingRefNumber()->SetValue(refNumber);

//	extensionConfig->m_simPrintPrice.SetValue(m_price);
	GetSettingPrintPrice()->SetValue(m_price);

//	extensionConfig->m_simPrintSize.SetValue(m_size);
	GetSettingPrintSize()->SetValue(m_size);

//	extensionConfig->m_simPrintSaleCondition1.SetValue(m_saleCondition1);
//	extensionConfig->m_simPrintSaleCondition2.SetValue(m_saleCondition2);
//	extensionConfig->m_simPrintSaleCondition3.SetValue(m_saleCondition3);
//	extensionConfig->m_simPrintSaleCondition4.SetValue(m_saleCondition4);

	GetSettingPrintSaleCondition1()->SetValue(m_saleCondition1);
	GetSettingPrintSaleCondition2()->SetValue(m_saleCondition2);
	GetSettingPrintSaleCondition3()->SetValue(m_saleCondition3);
	GetSettingPrintSaleCondition4()->SetValue(m_saleCondition4);

//	extensionConfig->m_simPrintMarketCenter.SetValue(m_marketCenter);
	GetSettingPrintMarketCenter()->SetValue(m_marketCenter);
//	extensionConfig->m_simPrintMarketListing.SetValue(m_marketListing);
	GetSettingStockMarketListing()->SetValue(m_marketListing);

	GetSettingPrintRefAutoIncrement()->SetValue(m_EditRef.isAutoAdd());

	m_EditRef.MakeOriginal();

//	EnableOkAndApplyButton(false);
	EnableApplyButton(false);
}

void BaseDialogSettingSimulatePrint::RestoreLayout()
{
	const std::string& symbol = GetSettingSymbol()->GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = GetSettingSymbolType()->GetValue();
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
	const unsigned short& symbolMap = GetSettingSymbolMap()->GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

	const unsigned char source = GetValidSource(GetSettingPrintSource()->GetValue());
//	if(source >= ST_Count)source = ST_Count - 1;
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}

	unsigned __int64 refNumberCurrent;
	U_ReadHexUnsignedNumberFromString(refNumberCurrent, m_refNumber.c_str());
	const unsigned __int64& refNumber = GetSettingRefNumber()->GetValue();
	ReplaceRefNumber(m_EditRef, m_refNumber, refNumberCurrent, refNumber);
/*
	if(refNumber != refNumberCurrent)
	{
		if(refNumber)
		{
			char num[33];
			m_refNumber = U_UnsignedNumberHexToStr(refNumber, num, sizeof(num));
			m_EditRef.SetWindowText(m_refNumber.c_str());
		}
		else
		{
			m_EditRef.SetWindowText("");
		}
		m_EditRef.Invalidate(FALSE);
	}
*/
	const Price& price = GetSettingPrintPrice()->GetValue();
	if(price != m_price)
	{
		m_price = price;
		FillSpinControlFromPrice(m_price, m_SpinPrice, 10000, false);
	}

	const unsigned int& size = GetSettingPrintSize()->GetValue();
	if(size != m_size)
	{
		m_size = size;
		m_SpinSize.SetValue(m_size, false, true, false);
	}

	SetCharInComboBox(m_ComboBoxSaleCondition1, GetSettingPrintSaleCondition1()->GetValue(), m_saleCondition1);
	SetCharInComboBox(m_ComboBoxSaleCondition2, GetSettingPrintSaleCondition2()->GetValue(), m_saleCondition2);
	SetCharInComboBox(m_ComboBoxSaleCondition3, GetSettingPrintSaleCondition3()->GetValue(), m_saleCondition3);
	SetCharInComboBox(m_ComboBoxSaleCondition4, GetSettingPrintSaleCondition4()->GetValue(), m_saleCondition4);

	SetCharInComboBox(m_ComboBoxMarketCenter, GetSettingPrintMarketCenter()->GetValue(), m_marketCenter);
	SetCharInComboBox(m_ComboBoxMarketListing, GetSettingStockMarketListing()->GetValue(), m_marketListing);

	m_SpinPrice.SetBkColorEnabled(m_price.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
	m_SpinSize.SetBkColorEnabled(m_size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());

	m_EditRef.InitializeAutoAdd(GetSettingPrintRefAutoIncrement()->GetValue());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(GetSettingUseOption()->GetValue(),
		GetSettingOptionSuffix()->GetValue(),
		GetSettingOptionCall()->GetValue(),
		GetSettingOptionStrikePrice()->GetValue(),
		GetSettingOptionExpiration()->GetValue());
#endif
//	EnableOkAndApplyButton(false);
	EnableApplyButton(false);

	EnableSendMessageButton();
	m_modified = false;
}

bool BaseDialogSettingSimulatePrint::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateOptionSource::isSendMessageButtonEnabled()
		&& m_marketCenter
		&& m_marketListing
		&& !m_price.isZero()
		&& m_size != 0;
}

void BaseDialogSettingSimulatePrint::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateOptionSource::DoDataExchange(pDX);
	DDX_Control(pDX, editSimPrintRefId, m_EditRef);
	DDX_Control(pDX, spinSimPrintPriceId, m_SpinPrice);
	DDX_Control(pDX, spinSimPrintSizeId, m_SpinSize);
	DDX_Control(pDX, comboPrintCondition1Id, m_ComboBoxSaleCondition1);
	DDX_Control(pDX, comboPrintCondition2Id, m_ComboBoxSaleCondition2);
	DDX_Control(pDX, comboPrintCondition3Id, m_ComboBoxSaleCondition3);
	DDX_Control(pDX, comboPrintCondition4Id, m_ComboBoxSaleCondition4);
	DDX_Control(pDX, comboPrintMarketCenterId, m_ComboBoxMarketCenter);
	DDX_Control(pDX, comboPrintMarketListingId, m_ComboBoxMarketListing);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulatePrint, BaseDialogSettingSimulateOptionSource)
//	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editSimPrintRefId, OnRefChange)
	ON_EN_UPDATE(editSimPrintRefId, OnRefUpdate)
	ON_CBN_SELCHANGE(comboPrintCondition1Id, OnSelchangeSaleCondition1)
	ON_CBN_SELCHANGE(comboPrintCondition2Id, OnSelchangeSaleCondition2)
	ON_CBN_SELCHANGE(comboPrintCondition3Id, OnSelchangeSaleCondition3)
	ON_CBN_SELCHANGE(comboPrintCondition4Id, OnSelchangeSaleCondition4)
	ON_CBN_SELCHANGE(comboPrintMarketCenterId, OnSelchangeMarketCenter)
	ON_CBN_SELCHANGE(comboPrintMarketListingId, OnSelchangeMarketListing)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void BaseDialogSettingSimulatePrint::OnRefUpdate()
{
	m_EditRef.TryUpdate();
}

void BaseDialogSettingSimulatePrint::MessageSent()
{
	if(m_EditRef.isAutoAdd())
	{
		unsigned __int64 refNumberCurrent;
		U_ReadHexUnsignedNumberFromString(refNumberCurrent, m_refNumber.c_str());
		ReplaceRefNumber(m_EditRef, m_refNumber, refNumberCurrent, refNumberCurrent + 1);
	}
}

void BaseDialogSettingSimulatePrint::OnRefChange()
{
	bool empty = m_refNumber.empty();
	CString text;
	m_EditRef.GetWindowText(text);
	m_refNumber = text;
	m_modified = true;
	if(m_refNumber.empty() != empty)
	{
		m_EditRef.Invalidate(FALSE);
		EnableSendMessageButton();
	}
}

void BaseDialogSettingSimulatePrint::OnSelchangeSaleCondition1()
{
	OnSelchangeUChar(m_ComboBoxSaleCondition1, m_saleCondition1);
}

void BaseDialogSettingSimulatePrint::OnSelchangeSaleCondition2()
{
	OnSelchangeUChar(m_ComboBoxSaleCondition2, m_saleCondition2);
}

void BaseDialogSettingSimulatePrint::OnSelchangeSaleCondition3()
{
	OnSelchangeUChar(m_ComboBoxSaleCondition3, m_saleCondition3);
}

void BaseDialogSettingSimulatePrint::OnSelchangeSaleCondition4()
{
	OnSelchangeUChar(m_ComboBoxSaleCondition4, m_saleCondition4);
}

void BaseDialogSettingSimulatePrint::OnSelchangeMarketCenter()
{
	if(OnSelchangeUChar(m_ComboBoxMarketCenter, m_marketCenter))
	{
		EnableSendMessageButton();
	}
}

void BaseDialogSettingSimulatePrint::OnSelchangeMarketListing()
{
	if(OnSelchangeUChar(m_ComboBoxMarketListing, m_marketListing))
	{
		EnableSendMessageButton();
	}
}

LRESULT BaseDialogSettingSimulatePrint::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimPrintPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPrice, 10000);
			bool changed = m_price.isZero() != price.isZero();
			if(changed)m_SpinPrice.SetBkColorEnabled(m_price.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(price != m_price)
			{
				m_price = price;
				m_modified = true;
			}
			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimPrintSizeId:
		{
			const unsigned int size = m_SpinSize.GetValue();
			bool changed = !m_size != !size;
			if(changed)m_SpinSize.SetBkColorEnabled(size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(size != m_size)
			{
				m_size = size;
				m_modified = true;
			}
			if(changed)EnableSendMessageButton();
		}
		break;

		default:
		BaseDialogSettingSimulateOptionSource::OnSpinChange(w, l);
		break;
	}
	return 0;
}
/*
HBRUSH BaseDialogSettingSimulatePrint::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSimAttributionId:
		switch(m_messageId)
		{
//			case M_OPT_LEVEL2:
			case M_BOOK_NEW_QUOTE:
			case M_BOOK_MODIFY_QUOTE:
			if(BOOK_LEVEL2 == m_bookId)
			{
				if(!m_marketCenter)
				{
					pDC->SetBkMode(TRANSPARENT);
					return *m_takionVisual->GetWarningBrush();
				}
			}
			break;

			default:
			pDC->SetBkMode(TRANSPARENT);
			return *m_takionVisual->GetGrayBrush();
		}
		break;

		case checkBoxSimQuoteRemainingSizeId:
		case checkBoxSimQuotePrintableId:
		if(m_messageId != M_BOOK_ORDER_EXECUTED_WITH_PRICE)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_takionVisual->GetGrayColor());
			return *m_bkBrush;
		}
		break;

	}
	return BaseDialogSettingSimulateQuote::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/