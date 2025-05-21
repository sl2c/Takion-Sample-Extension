#include "stdafx.h"
#include "SampleDialogSettingSimulatePrint.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulatePrint::SampleDialogSettingSimulatePrint(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulatePrint(mainWnd, parentTab, "Print"),
	m_SpinLastPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinHighPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinLowPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinVolume(10000000000ui64, 0, 0, 10000, 3, TakionSpin::charComma),
//	m_EditRefCorr("0123456789ABCDEF", false),
	m_SpinPriceCorr(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinSizeCorr(100000, 0, 0, 1000, 3, TakionSpin::charComma),

	m_volume(0),
	m_printMessageType(PMT_ERROR),
	m_sizeCorr(0),
	m_saleCondition1Corr('\0'),
	m_saleCondition2Corr('\0'),
	m_saleCondition3Corr('\0'),
	m_saleCondition4Corr('\0')
/*
//	BaseDialogSettingSimulateOptionSource(mainWnd, parentTab, "Print"),
	m_EditRef("0123456789ABCDEF", false),
	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinSize(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_size(0),
	m_saleCondition1('\0'),
	m_saleCondition2('\0'),
	m_saleCondition3('\0'),
	m_saleCondition4('\0'),
	m_marketCenter('\0'),
	m_marketListing('\0')
*/
{
//	m_messageStockPrint.m_symbol[sizeof(m_messageStockPrint.m_symbol) - 1] = '\0';
//	m_messageStockPrint.m_flags = 0;
#ifndef TAKION_NO_OPTIONS
//	m_messageOptionPrint.m_symbol[sizeof(m_messageOptionPrint.m_symbol) - 1] = '\0';
//	m_messageFuturePrint.m_symbol[sizeof(m_messageFuturePrint.m_symbol) - 1] = '\0';
//	m_messageFuturePrint.m_flags = (FutOpeningTrade << 1) - 1;
#endif
//	m_messageEquityPrintLong.m_symbol[sizeof(m_messageEquityPrintLong.m_symbol) - 1] = '\0';
//	m_messageEquityPrintShort.m_symbol[sizeof(m_messageEquityPrintShort.m_symbol) - 1] = '\0';

//	m_messageStockPrintError.m_symbol[sizeof(m_messageStockPrintError.m_symbol) - 1] = '\0';
//	m_messageEquityPrintError.m_symbol[sizeof(m_messageEquityPrintError.m_symbol) - 1] = '\0';

//	m_messageStockPrintCorrection.m_symbol[sizeof(m_messageStockPrintCorrection.m_symbol) - 1] = '\0';
//	m_messageEquityPrintCorrection.m_symbol[sizeof(m_messageEquityPrintCorrection.m_symbol) - 1] = '\0';

	m_SpinLastPrice.AddSpinCell(1, false);
	m_SpinLastPrice.AddSpinCell(100, false);
	m_SpinLastPrice.AddSpinCell(1000000, true);
	m_SpinLastPrice.AddSpinCell(10000, true);

	m_SpinHighPrice.AddSpinCell(1, false);
	m_SpinHighPrice.AddSpinCell(100, false);
	m_SpinHighPrice.AddSpinCell(1000000, true);
	m_SpinHighPrice.AddSpinCell(10000, true);

	m_SpinLowPrice.AddSpinCell(1, false);
	m_SpinLowPrice.AddSpinCell(100, false);
	m_SpinLowPrice.AddSpinCell(1000000, true);
	m_SpinLowPrice.AddSpinCell(10000, true);

	m_SpinVolume.AddSpinCell(1, false);
	m_SpinVolume.AddSpinCell(100, false);
	m_SpinVolume.AddSpinCell(1000000, true);
	m_SpinVolume.AddSpinCell(10000, true);

	m_SpinPriceCorr.AddSpinCell(1, false);
	m_SpinPriceCorr.AddSpinCell(100, false);
	m_SpinPriceCorr.AddSpinCell(1000000, true);
	m_SpinPriceCorr.AddSpinCell(10000, true);

	m_SpinSizeCorr.AddSpinCell(1, false);
	m_SpinSizeCorr.AddSpinCell(100, false);
	m_SpinSizeCorr.AddSpinCell(1000, true);

	m_ComboBoxSaleCondition1Corr.SetColorInvalid(m_visualBase->GetGrayColor());
	m_ComboBoxSaleCondition2Corr.SetColorInvalid(m_visualBase->GetGrayColor());
	m_ComboBoxSaleCondition3Corr.SetColorInvalid(m_visualBase->GetGrayColor());
	m_ComboBoxSaleCondition4Corr.SetColorInvalid(m_visualBase->GetGrayColor());

	const int priceWidth = 75;
//	const int staticWidth = 32;
//	const int sizeWidth = 44;
	const int charWidth = 24;
	const int attrWidth = 32;

	m_xc = m_firstColumnWidth + m_firstColumnWidth + 4 * hgap;
	const int sizeWidth = 54;
	const int halfWidth = sizeWidth / 2;
	const int staticWidth = sizeWidth * 2;//3 / 2;
	const int secondColumnWidth = staticWidth;//sizeWidth;// + sideWidth;

	m_maxControlWidth += secondColumnWidth + 4 * hgap;

	int y = vgap;

	const int staticPriceX = m_xc + priceWidth + hgap;

	AddButtonIndirect("Print", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, m_xc, y, staticWidth, checkBoxHeight, radioTypePrintId);
	AddButtonIndirect("Error", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, m_xc, y += checkBoxHeight + vgap, staticWidth, checkBoxHeight, radioTypeErrorId);
	AddButtonIndirect("Correction", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, m_xc, y += checkBoxHeight + vgap, staticWidth, checkBoxHeight, radioTypeCorrectionId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += checkBoxHeight + checkBoxHeight + vgap, priceWidth, spinHeight, spinSimPrintLastPriceId);
	AddStaticIndirect("Last Price", WS_VISIBLE | SS_NOPREFIX, 0, staticPriceX, y + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintLastPriceId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += spinHeight + vgap, priceWidth, spinHeight, spinSimPrintHighPriceId);
	AddStaticIndirect("High Price", WS_VISIBLE | SS_NOPREFIX, 0, staticPriceX, y + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintHighPriceId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += spinHeight + vgap, priceWidth, spinHeight, spinSimPrintLowPriceId);
	AddStaticIndirect("Low Price", WS_VISIBLE | SS_NOPREFIX, 0, staticPriceX, y + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintLowPriceId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += spinHeight + vgap, priceWidth, spinHeight, spinSimPrintVolumeId);
	AddStaticIndirect("Volume", WS_VISIBLE | SS_NOPREFIX, 0, staticPriceX, y + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintVolumeId);

	y = m_refNumberY;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE | ES_RIGHT, 0, m_xc, y, priceWidth, editHeight, editSimPrintCorrRefId);
	AddStaticIndirect("!Ref# (Hex)", WS_VISIBLE | SS_NOPREFIX, 0, staticPriceX, y + (editHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintCorrRefId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += editHeight + vgap, priceWidth, spinHeight, spinSimPrintCorrPriceId);
	AddStaticIndirect("!Price", WS_VISIBLE | SS_NOPREFIX, 0, staticPriceX, y + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintCorrPriceId);
//	controlWidth += staticWidth;
//	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	int controlWidth = m_xc + sizeWidth;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += spinHeight + vgap, sizeWidth, spinHeight, spinSimPrintCorrSizeId);
	AddStaticIndirect("!Size", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, y + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimPrintCorrSizeId);
//	controlWidth += staticWidth;
//	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + charWidth;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += spinHeight + vgap, charWidth, 250, comboPrintCorrCondition1Id);
	AddStaticIndirect("!SaleCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, y + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintCorrCondition1Id);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += comboBoxHeight + vgap, charWidth, 250, comboPrintCorrCondition2Id);
	AddStaticIndirect("!SaleCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, y + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintCorrCondition2Id);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += comboBoxHeight + vgap, charWidth, 250, comboPrintCorrCondition3Id);
	AddStaticIndirect("!SaleCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, y + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintCorrCondition3Id);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += comboBoxHeight + vgap, charWidth, 250, comboPrintCorrCondition4Id);
	AddStaticIndirect("!SaleCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, y + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticPrintCorrCondition4Id);
}

BOOL SampleDialogSettingSimulatePrint::OnInitDialog()
{
	m_printMessageType = PMT_ERROR;
	m_lastPrice.SetZero();
	m_highPrice.SetZero();
	m_lowPrice.SetZero();
	m_volume = 0;

	m_refNumberCorr.clear();
	m_priceCorr.SetZero();
	m_sizeCorr = 0;
	m_saleCondition1Corr = '\0';
	m_saleCondition2Corr = '\0';
	m_saleCondition3Corr = '\0';
	m_saleCondition4Corr = '\0';

	m_SpinLastPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
	m_SpinHighPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
	m_SpinLowPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
	m_SpinVolume.SetBkColorEnabled(m_visualBase->GetGrayColor());

	BOOL ret = BaseDialogSettingSimulatePrint::OnInitDialog();

	m_RadioTypePrint.SetCheck(BST_CHECKED);

	m_EditRefCorr.SetLimitText(16);
	m_SpinPriceCorr.SetBkColorEnabled(m_visualBase->GetGrayColor());
	m_SpinSizeCorr.SetBkColorEnabled(m_visualBase->GetGrayColor());

	m_ComboBoxSaleCondition1Corr.SetInvalid(true);
	m_ComboBoxSaleCondition2Corr.SetInvalid(true);
	m_ComboBoxSaleCondition3Corr.SetInvalid(true);
	m_ComboBoxSaleCondition4Corr.SetInvalid(true);

	FillCharRange(m_ComboBoxSaleCondition1Corr);
	FillCharRange(m_ComboBoxSaleCondition2Corr);
	FillCharRange(m_ComboBoxSaleCondition3Corr);
	FillCharRange(m_ComboBoxSaleCondition4Corr);

	m_ComboBoxSaleCondition1Corr.AddString((const char*)'@');
	m_ComboBoxSaleCondition2Corr.AddString((const char*)'@');
	m_ComboBoxSaleCondition3Corr.AddString((const char*)'@');
	m_ComboBoxSaleCondition4Corr.AddString((const char*)'@');
	m_ComboBoxSaleCondition1Corr.AddString((const char*)' ');
	m_ComboBoxSaleCondition2Corr.AddString((const char*)' ');
	m_ComboBoxSaleCondition3Corr.AddString((const char*)' ');
	m_ComboBoxSaleCondition4Corr.AddString((const char*)' ');
/*
	m_SpinLastPrice.SetBkColorEnabled(m_lastPrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
	m_SpinHighPrice.SetBkColorEnabled(m_highPrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
	m_SpinLowPrice.SetBkColorEnabled(m_lowPrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
	m_SpinVolume.SetBkColorEnabled(!m_volume ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
*/
//	RestoreLayout();

	return ret;
}

UCharSetting* SampleDialogSettingSimulatePrint::GetSettingSymbolType(){return &extensionConfig->m_simPrintSymbolType;}
StringSetting* SampleDialogSettingSimulatePrint::GetSettingSymbol(){return &extensionConfig->m_simPrintSymbol;}
UShortSetting* SampleDialogSettingSimulatePrint::GetSettingSymbolMap(){return &extensionConfig->m_simPrintSymbolMap;}
UCharSetting* SampleDialogSettingSimulatePrint::GetSettingPrintSource(){return &extensionConfig->m_simPrintSource;}
#ifndef TAKION_NO_OPTIONS
BoolSetting* SampleDialogSettingSimulatePrint::GetSettingUseOption(){return &extensionConfig->m_simPrintOption;}
BoolSetting* SampleDialogSettingSimulatePrint::GetSettingOptionCall(){return &extensionConfig->m_simPrintOptionCall;}
CharSetting* SampleDialogSettingSimulatePrint::GetSettingOptionSuffix(){return &extensionConfig->m_simPrintOptionSuffix;}
PriceSetting* SampleDialogSettingSimulatePrint::GetSettingOptionStrikePrice(){return &extensionConfig->m_simPrintOptionStrikePrice;}
UIntSetting* SampleDialogSettingSimulatePrint::GetSettingOptionExpiration(){return &extensionConfig->m_simPrintOptionExpiration;}
#endif
UInt64HexSetting* SampleDialogSettingSimulatePrint::GetSettingRefNumber(){return &extensionConfig->m_simPrintRefNumber;}
PriceSetting* SampleDialogSettingSimulatePrint::GetSettingPrintPrice(){return &extensionConfig->m_simPrintPrice;}
UIntSetting* SampleDialogSettingSimulatePrint::GetSettingPrintSize(){return &extensionConfig->m_simPrintSize;}
CharSetting* SampleDialogSettingSimulatePrint::GetSettingPrintSaleCondition1(){return &extensionConfig->m_simPrintSaleCondition1;}
CharSetting* SampleDialogSettingSimulatePrint::GetSettingPrintSaleCondition2(){return &extensionConfig->m_simPrintSaleCondition2;}
CharSetting* SampleDialogSettingSimulatePrint::GetSettingPrintSaleCondition3(){return &extensionConfig->m_simPrintSaleCondition3;}
CharSetting* SampleDialogSettingSimulatePrint::GetSettingPrintSaleCondition4(){return &extensionConfig->m_simPrintSaleCondition4;}
CharSetting* SampleDialogSettingSimulatePrint::GetSettingPrintMarketCenter(){return &extensionConfig->m_simPrintMarketCenter;}
CharSetting* SampleDialogSettingSimulatePrint::GetSettingStockMarketListing(){return &extensionConfig->m_simPrintMarketListing;}
BoolSetting* SampleDialogSettingSimulatePrint::GetSettingPrintRefAutoIncrement(){return &extensionConfig->m_simPrintRefAutoIncrement;}


void SampleDialogSettingSimulatePrint::UpdateSettings()
{
	BaseDialogSettingSimulatePrint::UpdateSettings();

	unsigned char printMessageType = m_printMessageType;
	if(printMessageType >= PMT_Count)printMessageType = PMT_Count - 1;
	extensionConfig->m_simPrintMessageType.SetValue(printMessageType);

	extensionConfig->m_simPrintLastPrice.SetValue(m_lastPrice);
	extensionConfig->m_simPrintHighPrice.SetValue(m_highPrice);
	extensionConfig->m_simPrintLowPrice.SetValue(m_lowPrice);
	extensionConfig->m_simPrintVolume.SetValue(m_volume);

	unsigned __int64 refNumberCorr;
	U_ReadHexUnsignedNumberFromString(refNumberCorr, m_refNumberCorr.c_str());
	extensionConfig->m_simPrintRefNumberCorr.SetValue(refNumberCorr);

	extensionConfig->m_simPrintPriceCorr.SetValue(m_priceCorr);

	extensionConfig->m_simPrintSizeCorr.SetValue(m_sizeCorr);

	extensionConfig->m_simPrintSaleCondition1Corr.SetValue(m_saleCondition1Corr);
	extensionConfig->m_simPrintSaleCondition2Corr.SetValue(m_saleCondition2Corr);
	extensionConfig->m_simPrintSaleCondition3Corr.SetValue(m_saleCondition3Corr);
	extensionConfig->m_simPrintSaleCondition4Corr.SetValue(m_saleCondition4Corr);

//	extensionConfig->m_simPrintRefAutoIncrement.SetValue(m_EditRef.isAutoAdd());
	extensionConfig->m_simPrintRefCorrAutoIncrement.SetValue(m_EditRefCorr.isAutoAdd());

	m_EditRefCorr.MakeOriginal();
/*
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simPrintSymbolType.SetValue(symbolType);
	extensionConfig->m_simPrintSymbol.SetValue(m_symbol);
	extensionConfig->m_simPrintSymbolMap.SetValue(m_symbolMap);
	extensionConfig->m_simPrintSource.SetValue(m_source);

#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_simPrintOption.SetValue(m_optionMode);
	extensionConfig->m_simPrintOptionCall.SetValue(m_optionCall);
	extensionConfig->m_simPrintOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_simPrintOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_simPrintOptionExpiration.SetValue(GetExpirationDate());
#endif
	unsigned __int64 refNumber;
	U_ReadHexUnsignedNumberFromString(refNumber, m_refNumber.c_str());
	extensionConfig->m_simPrintRefNumber.SetValue(refNumber);

	extensionConfig->m_simPrintPrice.SetValue(m_price);

	extensionConfig->m_simPrintSize.SetValue(m_size);

	extensionConfig->m_simPrintSaleCondition1.SetValue(m_saleCondition1);
	extensionConfig->m_simPrintSaleCondition2.SetValue(m_saleCondition2);
	extensionConfig->m_simPrintSaleCondition3.SetValue(m_saleCondition3);
	extensionConfig->m_simPrintSaleCondition4.SetValue(m_saleCondition4);
	extensionConfig->m_simPrintMarketCenter.SetValue(m_marketCenter);
	extensionConfig->m_simPrintMarketListing.SetValue(m_marketListing);

	EnableApplyButton(false);
*/
}

void SampleDialogSettingSimulatePrint::RestoreLayout()
{
	unsigned char printMessageType = extensionConfig->m_simPrintMessageType.GetValue();
	if(printMessageType >= PMT_Count)printMessageType = PMT_Count - 1;
	if(printMessageType != m_printMessageType)
	{
		SetMessageType(printMessageType);
//		m_printMessageType = printMessageType;
		switch(m_printMessageType)
		{
			case PMT_PRINT:
			m_RadioTypeError.SetCheck(BST_UNCHECKED);
			m_RadioTypeCorrection.SetCheck(BST_UNCHECKED);
			m_RadioTypePrint.SetCheck(BST_CHECKED);
			break;

			case PMT_ERROR:
			m_RadioTypePrint.SetCheck(BST_UNCHECKED);
			m_RadioTypeCorrection.SetCheck(BST_UNCHECKED);
			m_RadioTypeError.SetCheck(BST_CHECKED);
			break;

			case PMT_CORRECTION:
			m_RadioTypePrint.SetCheck(BST_UNCHECKED);
			m_RadioTypeError.SetCheck(BST_UNCHECKED);
			m_RadioTypeCorrection.SetCheck(BST_CHECKED);
			break;
		}
//		SymbolTypeChanged();
	}

	const Price& lastPrice = extensionConfig->m_simPrintLastPrice.GetValue();
	if(lastPrice != m_lastPrice)
	{
		m_lastPrice = lastPrice;
		FillSpinControlFromPrice(m_lastPrice, m_SpinLastPrice, 10000, false);
		if(PMT_PRINT != m_printMessageType)m_SpinLastPrice.SetBkColorEnabled(m_lastPrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
	}

	const Price& highPrice = extensionConfig->m_simPrintHighPrice.GetValue();
	if(highPrice != m_highPrice)
	{
		m_highPrice = highPrice;
		FillSpinControlFromPrice(m_highPrice, m_SpinHighPrice, 10000, false);
		if(PMT_PRINT != m_printMessageType)m_SpinHighPrice.SetBkColorEnabled(m_highPrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
	}

	const Price& lowPrice = extensionConfig->m_simPrintLowPrice.GetValue();
	if(lowPrice != m_lowPrice)
	{
		m_lowPrice = lowPrice;
		FillSpinControlFromPrice(m_lowPrice, m_SpinLowPrice, 10000, false);
		if(PMT_PRINT != m_printMessageType)m_SpinLowPrice.SetBkColorEnabled(m_lowPrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
	}

	const unsigned __int64& volume = extensionConfig->m_simPrintVolume.GetValue();
	if(volume != m_volume)
	{
		m_volume = volume;
		m_SpinVolume.SetValue(m_volume, false, true, false);
		if(PMT_PRINT != m_printMessageType)m_SpinVolume.SetBkColorEnabled(!m_volume ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
	}

	unsigned __int64 refNumberCurrentCorr;
	U_ReadHexUnsignedNumberFromString(refNumberCurrentCorr, m_refNumberCorr.c_str());
	const unsigned __int64& refNumberCorr = extensionConfig->m_simPrintRefNumberCorr.GetValue();
	ReplaceRefNumber(m_EditRefCorr, m_refNumberCorr, refNumberCurrentCorr, refNumberCorr);
/*
	if(refNumberCorr != refNumberCurrentCorr)
	{
		if(refNumberCorr)
		{
			char num[33];
			m_refNumberCorr = U_UnsignedNumberHexToStr(refNumberCorr, num, sizeof(num));
			m_EditRefCorr.SetWindowText(m_refNumberCorr.c_str());
		}
		else
		{
			m_EditRefCorr.SetWindowText("");
		}
		m_EditRefCorr.Invalidate(FALSE);
	}
*/
	const Price& priceCorr = extensionConfig->m_simPrintPriceCorr.GetValue();
	if(priceCorr != m_priceCorr)
	{
		m_priceCorr = priceCorr;
		FillSpinControlFromPrice(m_priceCorr, m_SpinPriceCorr, 10000, false);
	}

	const unsigned int& sizeCorr = extensionConfig->m_simPrintSizeCorr.GetValue();
	if(sizeCorr != m_sizeCorr)
	{
		m_sizeCorr = sizeCorr;
		m_SpinSizeCorr.SetValue(m_sizeCorr, false, true, false);
	}

	SetCharInComboBox(m_ComboBoxSaleCondition1Corr, extensionConfig->m_simPrintSaleCondition1Corr.GetValue(), m_saleCondition1Corr);
	SetCharInComboBox(m_ComboBoxSaleCondition2Corr, extensionConfig->m_simPrintSaleCondition2Corr.GetValue(), m_saleCondition2Corr);
	SetCharInComboBox(m_ComboBoxSaleCondition3Corr, extensionConfig->m_simPrintSaleCondition3Corr.GetValue(), m_saleCondition3Corr);
	SetCharInComboBox(m_ComboBoxSaleCondition4Corr, extensionConfig->m_simPrintSaleCondition4Corr.GetValue(), m_saleCondition4Corr);

	m_EditRefCorr.InitializeAutoAdd(extensionConfig->m_simPrintRefCorrAutoIncrement.GetValue());

	if(PMT_CORRECTION == m_printMessageType)
	{
		m_SpinPriceCorr.SetBkColorEnabled(m_priceCorr.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
		m_SpinSizeCorr.SetBkColorEnabled(m_sizeCorr ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
	}

	BaseDialogSettingSimulatePrint::RestoreLayout();
/*
	const std::string& symbol = extensionConfig->m_simPrintSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simPrintSymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simPrintSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

	unsigned char source = extensionConfig->m_simPrintSource.GetValue();
	if(source >= ST_Count)source = ST_Count - 1;
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}

	unsigned __int64 refNumberCurrent;
	U_ReadHexUnsignedNumberFromString(refNumberCurrent, m_refNumber.c_str());
	const unsigned __int64& refNumber = extensionConfig->m_simPrintRefNumber.GetValue();
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

	const Price& price = extensionConfig->m_simPrintPrice.GetValue();
	if(price != m_price)
	{
		m_price = price;
		FillSpinControlFromPrice(m_price, m_SpinPrice, 10000, false);
	}

	const unsigned int& size = extensionConfig->m_simPrintSize.GetValue();
	if(size != m_size)
	{
		m_size = size;
		m_SpinSize.SetValue(m_size, false, true, false);
	}

	SetCharInComboBox(m_ComboBoxSaleCondition1, extensionConfig->m_simPrintSaleCondition1.GetValue(), m_saleCondition1);
	SetCharInComboBox(m_ComboBoxSaleCondition2, extensionConfig->m_simPrintSaleCondition1.GetValue(), m_saleCondition1);
	SetCharInComboBox(m_ComboBoxSaleCondition3, extensionConfig->m_simPrintSaleCondition1.GetValue(), m_saleCondition1);
	SetCharInComboBox(m_ComboBoxSaleCondition4, extensionConfig->m_simPrintSaleCondition1.GetValue(), m_saleCondition1);

	SetCharInComboBox(m_ComboBoxMarketCenter, extensionConfig->m_simPrintMarketCenter.GetValue(), m_marketCenter);
	SetCharInComboBox(m_ComboBoxMarketListing, extensionConfig->m_simPrintMarketListing.GetValue(), m_marketListing);

	m_SpinPrice.SetBkColorEnabled(m_price.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
	m_SpinSize.SetBkColorEnabled(m_size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_simPrintOption.GetValue(),
		extensionConfig->m_simPrintOptionSuffix.GetValue(),
		extensionConfig->m_simPrintOptionCall.GetValue(),
		extensionConfig->m_simPrintOptionStrikePrice.GetValue(),
		extensionConfig->m_simPrintOptionExpiration.GetValue());
#endif
	EnableApplyButton(false);

	EnableSendMessageButton();
	m_modified = false;
*/
}

bool SampleDialogSettingSimulatePrint::isSendMessageButtonEnabled() const
{
	if(BaseDialogSettingSimulatePrint::isSendMessageButtonEnabled())
	{
		switch(m_printMessageType)
		{
			case PMT_PRINT:
			return true;

			case PMT_ERROR:
#ifndef TAKION_NO_OPTIONS
			if(isOptionUsed())//m_optionMode)
			{
				break;
			}
#endif
			return true;

			case PMT_CORRECTION:
#ifndef TAKION_NO_OPTIONS
			if(isOptionUsed())//m_optionMode)
			{
				break;
			}
#endif
			return !m_priceCorr.isZero() && m_sizeCorr != 0;
		}
	}
	return false;
}

const Message* SampleDialogSettingSimulatePrint::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
//		if(m_symbolType == SYMT_SYMBOL_STRING)
		const char* symbol = m_symbol.c_str();
#ifndef TAKION_NO_OPTIONS
		const bool optionUsed = isOptionUsed();
		unsigned __int64 futureNumericSymbol;
		bool future = !optionUsed && *symbol == '/';
		if(future)
		{
			futureNumericSymbol = TU_GetCurrentFutureSymbol(symbol + 1);
			if(futureNumericSymbol)
			symbol = (const char*)&futureNumericSymbol;
		}
		else
		{
			future = TU_IsFutureSymbol(symbol) != 0;
		}
#endif
		switch(m_printMessageType)
		{
			case PMT_PRINT:
#ifndef TAKION_NO_OPTIONS
			if(optionUsed)//m_optionMode)
			{
				OptionKey optionKey(m_strikePrice.GetCompactPrice(),
					m_optionCall,
					m_optionSuffix,
					m_expirationDay,
					m_expirationMonth,
					m_expirationYear);

				if(m_optionSuffix)U_CopyPlusOneCharAndPad(m_messageOptionPrint.m_symbol, sizeof(m_messageOptionPrint.m_symbol) - 1, symbol, m_optionSuffix, '\0');
				else U_CopyAndPad(m_messageOptionPrint.m_symbol, sizeof(m_messageOptionPrint.m_symbol) - 1, symbol, '\0');
				m_messageOptionPrint.m_priceDollars = m_price.GetDollars();
				m_messageOptionPrint.m_priceFraction = m_price.GetDollarFraction();
				m_messageOptionPrint.m_size = m_size;
				unsigned __int64 refNumber;
				U_ReadHexUnsignedNumberFromString(refNumber, m_refNumber.c_str());
				m_messageOptionPrint.m_tradeId = refNumber > 0xFFFFFFFF ? 0 : (unsigned int)refNumber;//??
				m_messageOptionPrint.m_marketCenterId = m_marketCenter;
				m_messageOptionPrint.m_saleCondition = m_saleCondition1;

				m_messageOptionPrint.m_optionBlock = OptionBlock(optionKey.CalculateOptionBlock());
				m_messageOptionPrint.m_time = TL_GetCurrentMillisecond();
				return &m_messageOptionPrint;
			}
			else if(future)
			{
				U_ReadHexUnsignedNumberFromString(m_messageFuturePrint.m_referenceNumber, m_refNumber.c_str());
				m_messageFuturePrint.m_marketCenterID = m_marketCenter;
				m_messageFuturePrint.m_priceDollars = m_price.GetDollars();
				m_messageFuturePrint.m_priceFraction = m_price.GetDollarFraction();
				m_messageFuturePrint.m_saleCondition = m_saleCondition1;
				m_messageFuturePrint.m_size = m_size;
				U_CopyAndPad(m_messageFuturePrint.m_symbol, sizeof(m_messageFuturePrint.m_symbol) - 1, symbol, '\0');
				m_messageFuturePrint.m_time = TL_GetCurrentMillisecond();
//				m_messageFuturePrint.m_vwapDollars = 0;
//				m_messageFuturePrint.m_vwapFraction = 0;
//				m_messageFuturePrint.m_flags = (FutOpeningTrade << 1) - 1;;
				return &m_messageStockPrint;
			}
			else
#endif
			switch(m_source)
			{
				case ST_MARKET_DATA:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					U_ReadHexUnsignedNumberFromString(m_messageStockPrint.m_referenceNumber, m_refNumber.c_str());
					m_messageStockPrint.m_priceDollars = m_price.GetDollars();
					m_messageStockPrint.m_priceFraction = m_price.GetDollarFraction();
					m_messageStockPrint.m_size = m_size;
					char* cursor = m_messageStockPrint.m_saleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageStockPrint.m_marketCenterID = m_marketCenter;
					m_messageStockPrint.m_primaryListingMarket = m_marketListing;
					U_CopyAndPad(m_messageStockPrint.m_symbol, sizeof(m_messageStockPrint.m_symbol) - 1, symbol, '\0');
					m_messageStockPrint.m_time = TL_GetCurrentMillisecond();
					return &m_messageStockPrint;
				}
				else
				{
					U_ReadHexUnsignedNumberFromString(m_messageStockPrintSm.m_referenceNumber, m_refNumber.c_str());
					m_messageStockPrintSm.m_priceDollars = m_price.GetDollars();
					m_messageStockPrintSm.m_priceFraction = m_price.GetDollarFraction();
					m_messageStockPrintSm.m_size = m_size;
					char* cursor = m_messageStockPrintSm.m_saleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageStockPrintSm.m_marketCenterID = m_marketCenter;
					m_messageStockPrintSm.m_primaryListingMarket = m_marketListing;
//					U_CopyAndPad(m_messageStockPrintSm.m_symbol, sizeof(m_messageStockPrint.m_symbol) - 1, symbol, '\0');
					m_messageStockPrintSm.m_symbol = m_symbolMap;
					m_messageStockPrint.m_time = TL_GetCurrentMillisecond();
					return &m_messageStockPrintSm;
				}
				break;

				case ST_MARKET_SORTER:
				if(m_saleCondition2 || m_saleCondition3 || m_saleCondition4 || m_size > 0xFFFF || m_price.GetDollars() >= 1 << 18)//big
				{
					if(m_symbolType == SYMT_SYMBOL_STRING)
					{
						m_messageEquityPrintLong.m_priceDollars = m_price.GetDollars();
						m_messageEquityPrintLong.m_priceFraction = m_price.GetDollarFraction();
						m_messageEquityPrintLong.m_size = m_size;
						char* cursor = m_messageEquityPrintLong.m_saleCondition;
						*cursor = m_saleCondition1;
						*++cursor = m_saleCondition2;
						*++cursor = m_saleCondition3;
						*++cursor = m_saleCondition4;
						m_messageEquityPrintLong.m_marketCenterID = m_marketCenter;
						U_CopyAndPad(m_messageEquityPrintLong.m_symbol, sizeof(m_messageEquityPrintLong.m_symbol) - 1, symbol, '\0');
						return &m_messageEquityPrintLong;
					}
					else
					{
						m_messageEquityPrintLongSm.m_priceDollars = m_price.GetDollars();
						m_messageEquityPrintLongSm.m_priceFraction = m_price.GetDollarFraction();
						m_messageEquityPrintLongSm.m_size = m_size;
						char* cursor = m_messageEquityPrintLongSm.m_saleCondition;
						*cursor = m_saleCondition1;
						*++cursor = m_saleCondition2;
						*++cursor = m_saleCondition3;
						*++cursor = m_saleCondition4;
						m_messageEquityPrintLongSm.m_marketCenterID = m_marketCenter;
						m_messageEquityPrintLongSm.m_symbol = m_symbolMap;
						return &m_messageEquityPrintLongSm;
					}
				}
				else//small
				{
					if(m_symbolType == SYMT_SYMBOL_STRING)
					{
						m_messageEquityPrintShort.m_price = m_price.GetCompactPrice();
						m_messageEquityPrintShort.m_size = (unsigned short)m_size;
						m_messageEquityPrintShort.m_saleCondition = m_saleCondition1;
						m_messageEquityPrintShort.m_marketCenterID = m_marketCenter;
						U_CopyAndPad(m_messageEquityPrintShort.m_symbol, sizeof(m_messageEquityPrintShort.m_symbol) - 1, symbol, '\0');
						return &m_messageEquityPrintShort;
					}
					else
					{
						m_messageEquityPrintShortSm.m_price = m_price.GetCompactPrice();
						m_messageEquityPrintShortSm.m_size = (unsigned short)m_size;
						m_messageEquityPrintShortSm.m_saleCondition = m_saleCondition1;
						m_messageEquityPrintShortSm.m_marketCenterID = m_marketCenter;
						m_messageEquityPrintShortSm.m_symbol = m_symbolMap;
						return &m_messageEquityPrintShortSm;
					}
				}
				break;
			}
			break;

			case PMT_ERROR:
#ifndef TAKION_NO_OPTIONS
			if(!isOptionUsed())
#endif
			switch(m_source)
			{
				case ST_MARKET_DATA:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					U_ReadHexUnsignedNumberFromString(m_messageStockPrintError.m_originalReferenceNumber, m_refNumber.c_str());
					m_messageStockPrintError.m_originalPriceDollars = m_price.GetDollars();
					m_messageStockPrintError.m_originalPriceFraction = m_price.GetDollarFraction();
					m_messageStockPrintError.m_originalSize = m_size;
					char* cursor = m_messageStockPrintError.m_originalSaleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageStockPrintError.m_marketCenterID = m_marketCenter;
					m_messageStockPrintError.m_primaryListingMarket = m_marketListing;
					U_CopyAndPad(m_messageStockPrintError.m_symbol, sizeof(m_messageStockPrintError.m_symbol) - 1, symbol, '\0');
					m_messageStockPrintError.m_time = TL_GetCurrentMillisecond();
					m_messageStockPrintError.m_lastPriceDollars = m_lastPrice.GetDollars();
					m_messageStockPrintError.m_lastPriceFraction = m_lastPrice.GetDollarFraction();
					m_messageStockPrintError.m_highPriceDollars = m_highPrice.GetDollars();
					m_messageStockPrintError.m_highPriceFraction = m_highPrice.GetDollarFraction();
					m_messageStockPrintError.m_lowPriceDollars = m_lowPrice.GetDollars();
					m_messageStockPrintError.m_lowPriceFraction = m_lowPrice.GetDollarFraction();
					m_messageStockPrintError.m_volume = m_volume;
					m_messageStockPrintError.m_flags = 0;
					if(!m_lastPrice.isZero())m_messageStockPrintError.m_flags |= LastPriceChanged;
					if(!m_highPrice.isZero())m_messageStockPrintError.m_flags |= HighPriceChanged;
					if(!m_lowPrice.isZero())m_messageStockPrintError.m_flags |= LowPriceChanged;
					return &m_messageStockPrintError;
				}
				else
				{
					U_ReadHexUnsignedNumberFromString(m_messageStockPrintErrorSm.m_originalReferenceNumber, m_refNumber.c_str());
					m_messageStockPrintErrorSm.m_originalPriceDollars = m_price.GetDollars();
					m_messageStockPrintErrorSm.m_originalPriceFraction = m_price.GetDollarFraction();
					m_messageStockPrintErrorSm.m_originalSize = m_size;
					char* cursor = m_messageStockPrintErrorSm.m_originalSaleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageStockPrintErrorSm.m_marketCenterID = m_marketCenter;
					m_messageStockPrintErrorSm.m_primaryListingMarket = m_marketListing;
					m_messageStockPrintErrorSm.m_symbol = m_symbolMap;
					m_messageStockPrintErrorSm.m_time = TL_GetCurrentMillisecond();
					m_messageStockPrintErrorSm.m_lastPriceDollars = m_lastPrice.GetDollars();
					m_messageStockPrintErrorSm.m_lastPriceFraction = m_lastPrice.GetDollarFraction();
					m_messageStockPrintErrorSm.m_highPriceDollars = m_highPrice.GetDollars();
					m_messageStockPrintErrorSm.m_highPriceFraction = m_highPrice.GetDollarFraction();
					m_messageStockPrintErrorSm.m_lowPriceDollars = m_lowPrice.GetDollars();
					m_messageStockPrintErrorSm.m_lowPriceFraction = m_lowPrice.GetDollarFraction();
					m_messageStockPrintErrorSm.m_volume = m_volume;
					m_messageStockPrintErrorSm.m_flags = 0;
					if(!m_lastPrice.isZero())m_messageStockPrintErrorSm.m_flags |= LastPriceChanged;
					if(!m_highPrice.isZero())m_messageStockPrintErrorSm.m_flags |= HighPriceChanged;
					if(!m_lowPrice.isZero())m_messageStockPrintErrorSm.m_flags |= LowPriceChanged;
					return &m_messageStockPrintErrorSm;
				}
				break;

				case ST_MARKET_SORTER:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					U_ReadHexUnsignedNumberFromString(m_messageEquityPrintError.m_originalReferenceNumber, m_refNumber.c_str());
					m_messageEquityPrintError.m_originalPriceDollars = m_price.GetDollars();
					m_messageEquityPrintError.m_originalPriceFraction = m_price.GetDollarFraction();
					m_messageEquityPrintError.m_originalSize = m_size;
					char* cursor = m_messageEquityPrintError.m_originalSaleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageEquityPrintError.m_marketCenterID = m_marketCenter;
					m_messageEquityPrintError.m_primaryListingMarket = m_marketListing;
					U_CopyAndPad(m_messageEquityPrintError.m_symbol, sizeof(m_messageEquityPrintError.m_symbol) - 1, symbol, '\0');
					m_messageEquityPrintError.m_time = TL_GetCurrentMillisecond();
					m_messageEquityPrintError.m_lastPriceDollars = m_lastPrice.GetDollars();
					m_messageEquityPrintError.m_lastPriceFraction = m_lastPrice.GetDollarFraction();
					m_messageEquityPrintError.m_highPriceDollars = m_highPrice.GetDollars();
					m_messageEquityPrintError.m_highPriceFraction = m_highPrice.GetDollarFraction();
					m_messageEquityPrintError.m_lowPriceDollars = m_lowPrice.GetDollars();
					m_messageEquityPrintError.m_lowPriceFraction = m_lowPrice.GetDollarFraction();
					m_messageEquityPrintError.m_volume = m_volume;
					m_messageEquityPrintError.m_flags = 0;
					if(!m_lastPrice.isZero())m_messageEquityPrintError.m_flags |= LastPriceChanged;
					if(!m_highPrice.isZero())m_messageEquityPrintError.m_flags |= HighPriceChanged;
					if(!m_lowPrice.isZero())m_messageEquityPrintError.m_flags |= LowPriceChanged;
					return &m_messageEquityPrintError;
				}
				else
				{
					U_ReadHexUnsignedNumberFromString(m_messageEquityPrintErrorSm.m_originalReferenceNumber, m_refNumber.c_str());
					m_messageEquityPrintErrorSm.m_originalPriceDollars = m_price.GetDollars();
					m_messageEquityPrintErrorSm.m_originalPriceFraction = m_price.GetDollarFraction();
					m_messageEquityPrintErrorSm.m_originalSize = m_size;
					char* cursor = m_messageEquityPrintErrorSm.m_originalSaleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageEquityPrintErrorSm.m_marketCenterID = m_marketCenter;
					m_messageEquityPrintErrorSm.m_primaryListingMarket = m_marketListing;
					m_messageEquityPrintErrorSm.m_symbol = m_symbolMap;
					m_messageEquityPrintErrorSm.m_time = TL_GetCurrentMillisecond();
					m_messageEquityPrintErrorSm.m_lastPriceDollars = m_lastPrice.GetDollars();
					m_messageEquityPrintErrorSm.m_lastPriceFraction = m_lastPrice.GetDollarFraction();
					m_messageEquityPrintErrorSm.m_highPriceDollars = m_highPrice.GetDollars();
					m_messageEquityPrintErrorSm.m_highPriceFraction = m_highPrice.GetDollarFraction();
					m_messageEquityPrintErrorSm.m_lowPriceDollars = m_lowPrice.GetDollars();
					m_messageEquityPrintErrorSm.m_lowPriceFraction = m_lowPrice.GetDollarFraction();
					m_messageEquityPrintErrorSm.m_volume = m_volume;
					m_messageEquityPrintErrorSm.m_flags = 0;
					if(!m_lastPrice.isZero())m_messageEquityPrintErrorSm.m_flags |= LastPriceChanged;
					if(!m_highPrice.isZero())m_messageEquityPrintErrorSm.m_flags |= HighPriceChanged;
					if(!m_lowPrice.isZero())m_messageEquityPrintErrorSm.m_flags |= LowPriceChanged;
					return &m_messageEquityPrintErrorSm;
				}
				break;
			}
			break;

			case PMT_CORRECTION:
#ifndef TAKION_NO_OPTIONS
			if(!isOptionUsed())
#endif
			switch(m_source)
			{
				case ST_MARKET_DATA:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					U_ReadHexUnsignedNumberFromString(m_messageStockPrintCorrection.m_originalReferenceNumber, m_refNumber.c_str());
					m_messageStockPrintCorrection.m_originalPriceDollars = m_price.GetDollars();
					m_messageStockPrintCorrection.m_originalPriceFraction = m_price.GetDollarFraction();
					m_messageStockPrintCorrection.m_originalSize = m_size;
					char* cursor = m_messageStockPrintCorrection.m_originalSaleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageStockPrintCorrection.m_marketCenterID = m_marketCenter;
					m_messageStockPrintCorrection.m_primaryListingMarket = m_marketListing;
					U_CopyAndPad(m_messageStockPrintCorrection.m_symbol, sizeof(m_messageStockPrintCorrection.m_symbol) - 1, symbol, '\0');
					m_messageStockPrintCorrection.m_time = TL_GetCurrentMillisecond();
					m_messageStockPrintCorrection.m_lastPriceDollars = m_lastPrice.GetDollars();
					m_messageStockPrintCorrection.m_lastPriceFraction = m_lastPrice.GetDollarFraction();
					m_messageStockPrintCorrection.m_highPriceDollars = m_highPrice.GetDollars();
					m_messageStockPrintCorrection.m_highPriceFraction = m_highPrice.GetDollarFraction();
					m_messageStockPrintCorrection.m_lowPriceDollars = m_lowPrice.GetDollars();
					m_messageStockPrintCorrection.m_lowPriceFraction = m_lowPrice.GetDollarFraction();
					m_messageStockPrintCorrection.m_volume = m_volume;
					m_messageStockPrintCorrection.m_flags = 0;
					if(!m_lastPrice.isZero())m_messageStockPrintCorrection.m_flags |= LastPriceChanged;
					if(!m_highPrice.isZero())m_messageStockPrintCorrection.m_flags |= HighPriceChanged;
					if(!m_lowPrice.isZero())m_messageStockPrintCorrection.m_flags |= LowPriceChanged;

					m_messageStockPrintCorrection.m_correctedPriceDollars = m_priceCorr.GetDollars();
					m_messageStockPrintCorrection.m_correctedPriceFraction = m_priceCorr.GetDollarFraction();
					m_messageStockPrintCorrection.m_correctedSize = m_sizeCorr;
					U_ReadHexUnsignedNumberFromString(m_messageStockPrintCorrection.m_correctedReferenceNumber, m_refNumberCorr.c_str());
					cursor = m_messageStockPrintCorrection.m_correctedSaleConditions;
					*cursor = m_saleCondition1Corr;
					*++cursor = m_saleCondition2Corr;
					*++cursor = m_saleCondition3Corr;
					*++cursor = m_saleCondition4Corr;

					return &m_messageStockPrintCorrection;
				}
				else
				{
					U_ReadHexUnsignedNumberFromString(m_messageStockPrintCorrectionSm.m_originalReferenceNumber, m_refNumber.c_str());
					m_messageStockPrintCorrectionSm.m_originalPriceDollars = m_price.GetDollars();
					m_messageStockPrintCorrectionSm.m_originalPriceFraction = m_price.GetDollarFraction();
					m_messageStockPrintCorrectionSm.m_originalSize = m_size;
					char* cursor = m_messageStockPrintCorrectionSm.m_originalSaleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageStockPrintCorrectionSm.m_marketCenterID = m_marketCenter;
					m_messageStockPrintCorrectionSm.m_primaryListingMarket = m_marketListing;
					m_messageStockPrintCorrectionSm.m_symbol = m_symbolMap;
					m_messageStockPrintCorrectionSm.m_time = TL_GetCurrentMillisecond();
					m_messageStockPrintCorrectionSm.m_lastPriceDollars = m_lastPrice.GetDollars();
					m_messageStockPrintCorrectionSm.m_lastPriceFraction = m_lastPrice.GetDollarFraction();
					m_messageStockPrintCorrectionSm.m_highPriceDollars = m_highPrice.GetDollars();
					m_messageStockPrintCorrectionSm.m_highPriceFraction = m_highPrice.GetDollarFraction();
					m_messageStockPrintCorrectionSm.m_lowPriceDollars = m_lowPrice.GetDollars();
					m_messageStockPrintCorrectionSm.m_lowPriceFraction = m_lowPrice.GetDollarFraction();
					m_messageStockPrintCorrectionSm.m_volume = m_volume;
					m_messageStockPrintCorrectionSm.m_flags = 0;
					if(!m_lastPrice.isZero())m_messageStockPrintCorrectionSm.m_flags |= LastPriceChanged;
					if(!m_highPrice.isZero())m_messageStockPrintCorrectionSm.m_flags |= HighPriceChanged;
					if(!m_lowPrice.isZero())m_messageStockPrintCorrectionSm.m_flags |= LowPriceChanged;

					m_messageStockPrintCorrectionSm.m_correctedPriceDollars = m_priceCorr.GetDollars();
					m_messageStockPrintCorrectionSm.m_correctedPriceFraction = m_priceCorr.GetDollarFraction();
					m_messageStockPrintCorrectionSm.m_correctedSize = m_sizeCorr;
					U_ReadHexUnsignedNumberFromString(m_messageStockPrintCorrectionSm.m_correctedReferenceNumber, m_refNumberCorr.c_str());
					cursor = m_messageStockPrintCorrectionSm.m_correctedSaleConditions;
					*cursor = m_saleCondition1Corr;
					*++cursor = m_saleCondition2Corr;
					*++cursor = m_saleCondition3Corr;
					*++cursor = m_saleCondition4Corr;

					return &m_messageStockPrintCorrectionSm;
				}
				break;

				case ST_MARKET_SORTER:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					U_ReadHexUnsignedNumberFromString(m_messageEquityPrintCorrection.m_originalReferenceNumber, m_refNumber.c_str());
					m_messageEquityPrintCorrection.m_originalPriceDollars = m_price.GetDollars();
					m_messageEquityPrintCorrection.m_originalPriceFraction = m_price.GetDollarFraction();
					m_messageEquityPrintCorrection.m_originalSize = m_size;
					char* cursor = m_messageEquityPrintCorrection.m_originalSaleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageEquityPrintCorrection.m_marketCenterID = m_marketCenter;
					m_messageEquityPrintCorrection.m_primaryListingMarket = m_marketListing;
					U_CopyAndPad(m_messageEquityPrintCorrection.m_symbol, sizeof(m_messageEquityPrintCorrection.m_symbol) - 1, symbol, '\0');
					m_messageEquityPrintCorrection.m_time = TL_GetCurrentMillisecond();
					m_messageEquityPrintCorrection.m_lastPriceDollars = m_lastPrice.GetDollars();
					m_messageEquityPrintCorrection.m_lastPriceFraction = m_lastPrice.GetDollarFraction();
					m_messageEquityPrintCorrection.m_highPriceDollars = m_highPrice.GetDollars();
					m_messageEquityPrintCorrection.m_highPriceFraction = m_highPrice.GetDollarFraction();
					m_messageEquityPrintCorrection.m_lowPriceDollars = m_lowPrice.GetDollars();
					m_messageEquityPrintCorrection.m_lowPriceFraction = m_lowPrice.GetDollarFraction();
					m_messageEquityPrintCorrection.m_volume = m_volume;
					m_messageEquityPrintCorrection.m_flags = 0;
					if(!m_lastPrice.isZero())m_messageEquityPrintCorrection.m_flags |= LastPriceChanged;
					if(!m_highPrice.isZero())m_messageEquityPrintCorrection.m_flags |= HighPriceChanged;
					if(!m_lowPrice.isZero())m_messageEquityPrintCorrection.m_flags |= LowPriceChanged;

					m_messageEquityPrintCorrection.m_correctedPriceDollars = m_priceCorr.GetDollars();
					m_messageEquityPrintCorrection.m_correctedPriceFraction = m_priceCorr.GetDollarFraction();
					m_messageEquityPrintCorrection.m_correctedSize = m_sizeCorr;
					U_ReadHexUnsignedNumberFromString(m_messageEquityPrintCorrection.m_correctedReferenceNumber, m_refNumberCorr.c_str());
					cursor = m_messageEquityPrintCorrection.m_correctedSaleConditions;
					*cursor = m_saleCondition1Corr;
					*++cursor = m_saleCondition2Corr;
					*++cursor = m_saleCondition3Corr;
					*++cursor = m_saleCondition4Corr;

					return &m_messageEquityPrintCorrection;
				}
				else
				{
					U_ReadHexUnsignedNumberFromString(m_messageEquityPrintCorrectionSm.m_originalReferenceNumber, m_refNumber.c_str());
					m_messageEquityPrintCorrectionSm.m_originalPriceDollars = m_price.GetDollars();
					m_messageEquityPrintCorrectionSm.m_originalPriceFraction = m_price.GetDollarFraction();
					m_messageEquityPrintCorrectionSm.m_originalSize = m_size;
					char* cursor = m_messageEquityPrintCorrectionSm.m_originalSaleConditions;
					*cursor = m_saleCondition1;
					*++cursor = m_saleCondition2;
					*++cursor = m_saleCondition3;
					*++cursor = m_saleCondition4;
					m_messageEquityPrintCorrectionSm.m_marketCenterID = m_marketCenter;
					m_messageEquityPrintCorrectionSm.m_primaryListingMarket = m_marketListing;
					m_messageEquityPrintCorrectionSm.m_symbol = m_symbolMap;
					m_messageEquityPrintCorrectionSm.m_time = TL_GetCurrentMillisecond();
					m_messageEquityPrintCorrectionSm.m_lastPriceDollars = m_lastPrice.GetDollars();
					m_messageEquityPrintCorrectionSm.m_lastPriceFraction = m_lastPrice.GetDollarFraction();
					m_messageEquityPrintCorrectionSm.m_highPriceDollars = m_highPrice.GetDollars();
					m_messageEquityPrintCorrectionSm.m_highPriceFraction = m_highPrice.GetDollarFraction();
					m_messageEquityPrintCorrectionSm.m_lowPriceDollars = m_lowPrice.GetDollars();
					m_messageEquityPrintCorrectionSm.m_lowPriceFraction = m_lowPrice.GetDollarFraction();
					m_messageEquityPrintCorrectionSm.m_volume = m_volume;
					m_messageEquityPrintCorrectionSm.m_flags = 0;
					if(!m_lastPrice.isZero())m_messageEquityPrintCorrectionSm.m_flags |= LastPriceChanged;
					if(!m_highPrice.isZero())m_messageEquityPrintCorrectionSm.m_flags |= HighPriceChanged;
					if(!m_lowPrice.isZero())m_messageEquityPrintCorrectionSm.m_flags |= LowPriceChanged;

					m_messageEquityPrintCorrectionSm.m_correctedPriceDollars = m_priceCorr.GetDollars();
					m_messageEquityPrintCorrectionSm.m_correctedPriceFraction = m_priceCorr.GetDollarFraction();
					m_messageEquityPrintCorrectionSm.m_correctedSize = m_sizeCorr;
					U_ReadHexUnsignedNumberFromString(m_messageEquityPrintCorrectionSm.m_correctedReferenceNumber, m_refNumberCorr.c_str());
					cursor = m_messageEquityPrintCorrectionSm.m_correctedSaleConditions;
					*cursor = m_saleCondition1Corr;
					*++cursor = m_saleCondition2Corr;
					*++cursor = m_saleCondition3Corr;
					*++cursor = m_saleCondition4Corr;

					return &m_messageEquityPrintCorrectionSm;
				}
				break;
			}
			break;
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulatePrint::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulatePrint::DoDataExchange(pDX);
	DDX_Control(pDX, radioTypePrintId, m_RadioTypePrint);
	DDX_Control(pDX, radioTypeErrorId, m_RadioTypeError);
	DDX_Control(pDX, radioTypeCorrectionId, m_RadioTypeCorrection);

	DDX_Control(pDX, spinSimPrintLastPriceId, m_SpinLastPrice);
	DDX_Control(pDX, spinSimPrintHighPriceId, m_SpinHighPrice);
	DDX_Control(pDX, spinSimPrintLowPriceId, m_SpinLowPrice);
	DDX_Control(pDX, spinSimPrintVolumeId, m_SpinVolume);

	DDX_Control(pDX, editSimPrintCorrRefId, m_EditRefCorr);
	DDX_Control(pDX, spinSimPrintCorrPriceId, m_SpinPriceCorr);
	DDX_Control(pDX, spinSimPrintCorrSizeId, m_SpinSizeCorr);
	DDX_Control(pDX, comboPrintCorrCondition1Id, m_ComboBoxSaleCondition1Corr);
	DDX_Control(pDX, comboPrintCorrCondition2Id, m_ComboBoxSaleCondition2Corr);
	DDX_Control(pDX, comboPrintCorrCondition3Id, m_ComboBoxSaleCondition3Corr);
	DDX_Control(pDX, comboPrintCorrCondition4Id, m_ComboBoxSaleCondition4Corr);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulatePrint, BaseDialogSettingSimulatePrint)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(radioTypePrintId, OnTypePrint)
	ON_BN_CLICKED(radioTypeErrorId, OnTypeError)
	ON_BN_CLICKED(radioTypeCorrectionId, OnTypeCorrection)

	ON_EN_CHANGE(editSimPrintCorrRefId, OnRefChangeCorr)
	ON_EN_UPDATE(editSimPrintCorrRefId, OnRefUpdateCorr)
	ON_CBN_SELCHANGE(comboPrintCorrCondition1Id, OnSelchangeSaleCondition1Corr)
	ON_CBN_SELCHANGE(comboPrintCorrCondition2Id, OnSelchangeSaleCondition2Corr)
	ON_CBN_SELCHANGE(comboPrintCorrCondition3Id, OnSelchangeSaleCondition3Corr)
	ON_CBN_SELCHANGE(comboPrintCorrCondition4Id, OnSelchangeSaleCondition4Corr)

	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulatePrint::OnRefUpdateCorr()
{
	m_EditRefCorr.TryUpdate();
}

void SampleDialogSettingSimulatePrint::MessageSent()
{
	BaseDialogSettingSimulatePrint::MessageSent();
	if(m_EditRefCorr.isAutoAdd())
	{
		unsigned __int64 refNumberCurrent;
		U_ReadHexUnsignedNumberFromString(refNumberCurrent, m_refNumberCorr.c_str());
		ReplaceRefNumber(m_EditRefCorr, m_refNumberCorr, refNumberCurrent, refNumberCurrent + 1);
	}
}

void SampleDialogSettingSimulatePrint::OnRefChangeCorr()
{
	bool empty = m_refNumberCorr.empty();
	CString text;
	m_EditRefCorr.GetWindowText(text);
	m_refNumberCorr = text;
	m_modified = true;
	if(m_refNumberCorr.empty() != empty)
	{
		m_EditRefCorr.Invalidate(FALSE);
		EnableSendMessageButton();
	}
}

void SampleDialogSettingSimulatePrint::OnSelchangeSaleCondition1Corr()
{
	OnSelchangeUChar(m_ComboBoxSaleCondition1Corr, m_saleCondition1Corr);
}

void SampleDialogSettingSimulatePrint::OnSelchangeSaleCondition2Corr()
{
	OnSelchangeUChar(m_ComboBoxSaleCondition2Corr, m_saleCondition2Corr);
}

void SampleDialogSettingSimulatePrint::OnSelchangeSaleCondition3Corr()
{
	OnSelchangeUChar(m_ComboBoxSaleCondition3Corr, m_saleCondition3Corr);
}

void SampleDialogSettingSimulatePrint::OnSelchangeSaleCondition4Corr()
{
	OnSelchangeUChar(m_ComboBoxSaleCondition4Corr, m_saleCondition4Corr);
}

bool SampleDialogSettingSimulatePrint::SetMessageType(const unsigned char messageType)
{
	if(messageType != m_printMessageType)
	{
		const bool corr = PMT_CORRECTION == messageType;
		if((PMT_CORRECTION == m_printMessageType) != corr)
		{
			m_EditRefCorr.Invalidate(FALSE);
			if(corr)
			{
				m_SpinPriceCorr.SetBkColorEnabled(m_priceCorr.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
				m_SpinSizeCorr.SetBkColorEnabled(m_sizeCorr ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());

				m_ComboBoxSaleCondition1Corr.SetInvalid(false);
				m_ComboBoxSaleCondition2Corr.SetInvalid(false);
				m_ComboBoxSaleCondition3Corr.SetInvalid(false);
				m_ComboBoxSaleCondition4Corr.SetInvalid(false);
			}
			else
			{
				m_SpinPriceCorr.SetBkColorEnabled(m_visualBase->GetGrayColor());
				m_SpinSizeCorr.SetBkColorEnabled(m_visualBase->GetGrayColor());

				m_ComboBoxSaleCondition1Corr.SetInvalid(true);
				m_ComboBoxSaleCondition2Corr.SetInvalid(true);
				m_ComboBoxSaleCondition3Corr.SetInvalid(true);
				m_ComboBoxSaleCondition4Corr.SetInvalid(true);
			}
		}
		m_printMessageType = messageType;
		if(PMT_PRINT == messageType)
		{
			m_SpinLastPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
			m_SpinHighPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
			m_SpinLowPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
			m_SpinVolume.SetBkColorEnabled(m_visualBase->GetGrayColor());
		}
		else
		{
			const COLORREF color = GetSysColor(COLOR_WINDOW);
			m_SpinLastPrice.SetBkColorEnabled(m_lastPrice.isZero() ? color : m_visualBase->GetBlueColor());
			m_SpinHighPrice.SetBkColorEnabled(m_highPrice.isZero() ? color : m_visualBase->GetBlueColor());
			m_SpinLowPrice.SetBkColorEnabled(m_lowPrice.isZero() ? color : m_visualBase->GetBlueColor());
			m_SpinVolume.SetBkColorEnabled(!m_volume ? color : m_visualBase->GetBlueColor());
		}
		return true;
	}
	return false;
}

void SampleDialogSettingSimulatePrint::OnPrintMessageType()
{
//	m_messageType
	const unsigned char messageType = m_RadioTypeError.GetCheck() == BST_CHECKED ? PMT_ERROR:
		m_RadioTypeCorrection.GetCheck() == BST_CHECKED ? PMT_CORRECTION:
		PMT_PRINT;
	if(SetMessageType(messageType))
	{
//		SymbolTypeChanged();
		EnableSendMessageButton();
		m_modified = true;
	}
}

void SampleDialogSettingSimulatePrint::OnTypePrint()
{
	OnPrintMessageType();
}

void SampleDialogSettingSimulatePrint::OnTypeError()
{
	OnPrintMessageType();
}

void SampleDialogSettingSimulatePrint::OnTypeCorrection()
{
	OnPrintMessageType();
}

LRESULT SampleDialogSettingSimulatePrint::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimPrintLastPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinLastPrice, 10000);
			bool changed = m_lastPrice.isZero() != price.isZero();
			if(changed && PMT_PRINT != m_printMessageType)m_SpinLastPrice.SetBkColorEnabled(!m_lastPrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
			if(price != m_lastPrice)
			{
				m_lastPrice = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimPrintHighPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinHighPrice, 10000);
			bool changed = m_highPrice.isZero() != price.isZero();
			if(changed && PMT_PRINT != m_printMessageType)m_SpinHighPrice.SetBkColorEnabled(!m_highPrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
			if(price != m_highPrice)
			{
				m_highPrice = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimPrintLowPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinLowPrice, 10000);
			bool changed = m_lowPrice.isZero() != price.isZero();
			if(changed && PMT_PRINT != m_printMessageType)m_SpinLowPrice.SetBkColorEnabled(!m_lowPrice.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
			if(price != m_lowPrice)
			{
				m_lowPrice = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimPrintVolumeId:
		{
			const unsigned __int64 volume = m_SpinVolume.GetValue();
			bool changed = !m_volume != !volume;
			if(changed && PMT_PRINT != m_printMessageType)m_SpinVolume.SetBkColorEnabled(!volume ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetBlueColor());
			if(volume != m_volume)
			{
				m_volume = volume;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimPrintCorrPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceCorr, 10000);
			bool changed = m_priceCorr.isZero() != price.isZero();
			if(changed && PMT_CORRECTION == m_printMessageType)m_SpinPriceCorr.SetBkColorEnabled(m_priceCorr.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(price != m_priceCorr)
			{
				m_priceCorr = price;
				m_modified = true;
			}
			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimPrintCorrSizeId:
		{
			const unsigned int size = m_SpinSizeCorr.GetValue();
			bool changed = !m_sizeCorr != !size;
			if(changed && PMT_CORRECTION == m_printMessageType)m_SpinSizeCorr.SetBkColorEnabled(size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(size != m_sizeCorr)
			{
				m_sizeCorr = size;
				m_modified = true;
			}
			if(changed)EnableSendMessageButton();
		}
		break;

		default:
		BaseDialogSettingSimulatePrint::OnSpinChange(w, l);
		break;
	}
	return 0;
}

HBRUSH SampleDialogSettingSimulatePrint::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSimPrintCorrRefId:
		if(PMT_CORRECTION != m_printMessageType)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		break;
	}
	return BaseDialogSettingSimulatePrint::OnCtlColor(pDC, pWnd, nCtlColor);
}
