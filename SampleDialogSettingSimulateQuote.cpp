#include "stdafx.h"
#include "SampleDialogSettingSimulateQuote.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateQuote::SampleDialogSettingSimulateQuote(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateQuote(mainWnd, parentTab, "Quote"),
	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinSize(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_EditAttribution("2ABCDEFGHIJKLMNOPQRSTUVWXYZ", false),
	m_SpinRemainingSize(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_size(0),
	m_remainingSize(0),
	m_useRemainingSize(false),
	m_numericAttribution(0),
	m_printable(false),
	m_messageId(0),
	m_quoteCondition('\0'),
	m_retailLiquidity(false),
	m_marketCenter(nullptr)
{
	m_messageNewQuote.m_symbol[sizeof(m_messageNewQuote.m_symbol) - 1] = '\0';
#ifndef TAKION_NO_OPTIONS
	m_messageModifyOptionQuote.m_symbol[sizeof(m_messageModifyOptionQuote.m_symbol) - 1] = '\0';
	m_messageModifyFutureQuote.m_symbol[sizeof(m_messageModifyFutureQuote.m_symbol) - 1] = '\0';
	m_messageDeleteFutureQuote.m_symbol[sizeof(m_messageDeleteFutureQuote.m_symbol) - 1] = '\0';
#endif
	m_messageModifyQuote.m_symbol[sizeof(m_messageModifyQuote.m_symbol) - 1] = '\0';
	m_messageDeleteQuote.m_symbol[sizeof(m_messageDeleteQuote.m_symbol) - 1] = '\0';
	m_messageExecuteQuote.m_symbol[sizeof(m_messageExecuteQuote.m_symbol) - 1] = '\0';
	m_messageExecuteWithPriceQuote.m_symbol[sizeof(m_messageExecuteWithPriceQuote.m_symbol) - 1] = '\0';
	m_messageCancelQuote.m_symbol[sizeof(m_messageCancelQuote.m_symbol) - 1] = '\0';
	m_messageExecuteHiddenQuote.m_symbol[sizeof(m_messageExecuteHiddenQuote.m_symbol) - 1] = '\0';
	m_messageCrossTradeQuote.m_symbol[sizeof(m_messageCrossTradeQuote.m_symbol) - 1] = '\0';

	m_SpinPrice.AddSpinCell(1, false);
	m_SpinPrice.AddSpinCell(100, false);
	m_SpinPrice.AddSpinCell(1000000, true);
	m_SpinPrice.AddSpinCell(10000, true);

	m_SpinSize.AddSpinCell(1, false);
	m_SpinSize.AddSpinCell(100, false);
	m_SpinSize.AddSpinCell(1000, true);

	m_SpinRemainingSize.AddSpinCell(1, false);
	m_SpinRemainingSize.AddSpinCell(100, false);
	m_SpinRemainingSize.AddSpinCell(1000, true);

	m_bottomControl += vgap;
	const int priceWidth = 75;
	const int staticWidth = 32;
	const int sizeWidth = 54;
	const int charWidth = 24;
	const int attrWidth = 32;

	int controlWidth = m_xc + priceWidth + hgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimQuotePriceId);
	AddStaticIndirect("Price", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimQuotePriceId);
	controlWidth += staticWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + sizeWidth + hgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl += spinHeight + vgap, sizeWidth, spinHeight, spinSimQuoteSizeId);
	AddStaticIndirect("Size", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimQuoteSizeId);
	controlWidth += staticWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + charWidth;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += spinHeight + vgap, charWidth, 250, comboQuoteConditionId);
	AddStaticIndirect("QuoteCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticQuoteConditionId);
	controlWidth += sizeWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + attrWidth + hgap;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, attrWidth, editHeight, editSimAttributionId);
	AddStaticIndirect("Attribution", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (editHeight - staticHeight) / 2, sizeWidth, staticHeight, staticSimAttributionId);
	controlWidth += sizeWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

//spinSimQuoteRemainingSizeId
	controlWidth = m_xc + sizeWidth + hgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl += editHeight + vgap, sizeWidth, spinHeight, spinSimQuoteRemainingSizeId);
//	AddStaticIndirect("RemainingSize", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, staticWidth, checkBoxHeight, checkBoxSimQuoteRemainingSizeId);
	AddButtonIndirect("RemainingSize", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, controlWidth, m_bottomControl + (spinHeight - checkBoxHeight) / 2, priceWidth, checkBoxHeight, checkBoxSimQuoteRemainingSizeId);
	controlWidth += priceWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

//	controlWidth = m_xc + sizeWidth;
	AddButtonIndirect("Printable", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, m_xc, m_bottomControl += spinHeight + vgap, sizeWidth, checkBoxHeight, checkBoxSimQuotePrintableId);
	AddButtonIndirect("Retail Liquidity", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, m_xc, m_bottomControl += checkBoxHeight + vgap, priceWidth, checkBoxHeight, checkBoxSimQuoteRetailLiquidityId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_maxControlWidth + hgap, vgap, priceWidth, 150, comboSimQuoteMessageId);
	AddStaticIndirect("MessageId", WS_VISIBLE | SS_NOPREFIX, 0, m_maxControlWidth + hgap + priceWidth, vgap + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticSimQuoteMessageId);

	m_bottomControl += spinHeight;
}

BOOL SampleDialogSettingSimulateQuote::OnInitDialog()
{
	m_messageId = 0;
	m_quoteCondition = '\0';
	m_retailLiquidity = false;
	m_price.SetZero();
	m_size = 0;
	m_remainingSize = 0;
	m_useRemainingSize = false;
	m_printable = false;
	m_attribution.clear();
	m_numericAttribution = 0;
	BOOL ret = BaseDialogSettingSimulateQuote::OnInitDialog();
	m_SpinPrice.SetBkColorEnabled(m_visualBase->GetWarningColor());
	m_SpinSize.SetBkColorEnabled(m_visualBase->GetWarningColor());
	m_SpinRemainingSize.SetBkColorEnabled(m_visualBase->GetGrayColor());

	m_EditAttribution.SetLimitText(4);

	FillCharRange(m_ComboBoxQuoteCondition);

	m_ComboBoxMessageId.AddItem("New", M_BOOK_NEW_QUOTE);
	m_ComboBoxMessageId.AddItem("Modify", M_BOOK_MODIFY_QUOTE);
	m_ComboBoxMessageId.AddItem("Delete", M_BOOK_DELETE_QUOTE);
	m_ComboBoxMessageId.AddItem("Cancel", M_BOOK_ORDER_CANCELED);
	m_ComboBoxMessageId.AddItem("Execute", M_BOOK_ORDER_EXECUTED);
	m_ComboBoxMessageId.AddItem("Execute@Price", M_BOOK_ORDER_EXECUTED_WITH_PRICE);
	m_ComboBoxMessageId.AddItem("ExecuteHidden", M_BOOK_TRADE);
	m_ComboBoxMessageId.AddItem("CrossTrade", M_BOOK_CROSS_TRADE);

	m_ComboBoxMessageId.SetCurSel(0);
	OnSelchangeMessageId();

//	RestoreLayout();

	return ret;
}

#ifndef TAKION_NO_OPTIONS
bool SampleDialogSettingSimulateQuote::isOptionModeUsed() const
{
	switch(m_messageId)
	{
		case M_BOOK_MODIFY_QUOTE:
		return BaseDialogSettingSimulateQuote::isOptionModeUsed();

		default:
		return false;
	}
}
#endif
void SampleDialogSettingSimulateQuote::UpdateSettings()
{
	BaseDialogSettingSimulateQuote::UpdateSettings();
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simQuoteSymbolType.SetValue(symbolType);
	extensionConfig->m_simQuoteSymbol.SetValue(m_symbol);
	extensionConfig->m_simQuoteSymbolMap.SetValue(m_symbolMap);

#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_simQuoteOption.SetValue(m_optionMode);
	extensionConfig->m_simQuoteOptionCall.SetValue(m_optionCall);
	extensionConfig->m_simQuoteOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_simQuoteOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_simQuoteOptionExpiration.SetValue(GetExpirationDate());
#endif
	unsigned __int64 refNumber;
	U_ReadHexUnsignedNumberFromString(refNumber, m_refNumber.c_str());
	extensionConfig->m_simQuoteRefNumber.SetValue(refNumber);
/*
	int sel = m_ComboBoxSide.GetCurSel();
	if(sel < Buy)sel = Buy;
	else if(sel > Sell)sel = Sell;
*/
	extensionConfig->m_simQuoteSide.SetValue(m_side);

	extensionConfig->m_simQuoteBookName.SetValue(m_bookName);

	extensionConfig->m_simQuotePrice.SetValue(m_price);

	extensionConfig->m_simQuoteSize.SetValue(m_size);

	extensionConfig->m_simQuoteCondition.SetValue(m_quoteCondition);
	extensionConfig->m_simQuoteRetailLiquidity.SetValue(m_retailLiquidity);

	extensionConfig->m_simQuoteAttribution.SetValue(m_numericAttribution);

	extensionConfig->m_simQuoteUseRemainingSize.SetValue(m_useRemainingSize);
	extensionConfig->m_simQuoteRemainingSize.SetValue(m_remainingSize);

	extensionConfig->m_simQuotePrintable.SetValue(m_printable);

	extensionConfig->m_simQuoteRefAutoIncrement.SetValue(m_EditRef.isAutoAdd());

	extensionConfig->m_simQuoteMessageName.SetValue(m_messageName);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateQuote::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simQuoteSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simQuoteSymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simQuoteSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

	unsigned __int64 refNumberCurrent;
	U_ReadHexUnsignedNumberFromString(refNumberCurrent, m_refNumber.c_str());
	const unsigned __int64& refNumber = extensionConfig->m_simQuoteRefNumber.GetValue();
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
	unsigned char side = extensionConfig->m_simQuoteSide.GetValue();
	if(side > Sell)side = Sell;
	if(side != m_side)
	{
		m_side = side;
		m_ComboBoxSide.SetCurSel(m_side);
	}

	int sel;
	const std::string& bookName = extensionConfig->m_simQuoteBookName.GetValue();
	if(bookName.empty())
	{
		m_ComboBoxBookId.SetCurSel(0);
		m_bookName = m_ComboBoxBookId.GetSelectedName();
	}
	else
	{
		sel = m_ComboBoxBookId.FindItemByName(bookName.c_str());
		if(sel >= 0)
		{
			m_ComboBoxBookId.SetCurSel(sel);
			m_bookName = bookName;
		}
		else
		{
			m_ComboBoxBookId.SetCurSel(0);
			m_bookName = m_ComboBoxBookId.GetSelectedName();
		}
	}
	OnSelchangeBook();
//	m_bookId = m_ComboBoxBookId.GetSelectedNumber();
//	if(m_bookId < ECNBOOK_NSDQ)m_bookId = ECNBOOK_NSDQ;

	const Price& price = extensionConfig->m_simQuotePrice.GetValue();
	if(price != m_price)
	{
		m_price = price;
		FillSpinControlFromPrice(m_price, m_SpinPrice, 10000, false);
	}

	const unsigned int& size = extensionConfig->m_simQuoteSize.GetValue();
	if(size != m_size)
	{
		m_size = size;
		m_SpinSize.SetValue(m_size, false, true, false);
	}

	const unsigned char& quoteCondition = extensionConfig->m_simQuoteCondition.GetValue();
	if(quoteCondition != m_quoteCondition)
	{
		m_quoteCondition = quoteCondition;
		sel = m_ComboBoxQuoteCondition.FindItem((const void*)m_quoteCondition);
		if(sel >= 0)
		{
			m_ComboBoxQuoteCondition.SetCurSel(sel);
		}
		else
		{
			m_ComboBoxQuoteCondition.SetCurSel(0);
			m_quoteCondition = m_ComboBoxQuoteCondition.GetSelectedChar();
		}
	}

//	const std::string& attribution = extensionConfig->m_simQuoteAttribution.GetValue();
	const unsigned int& numericAttribution = extensionConfig->m_simQuoteAttribution.GetValue();

//	if(strcmp(attribution.c_str(), m_attribution.c_str()))
	if(numericAttribution != m_numericAttribution)
	{
//		m_numericAttribution = numericAttribution;
//		m_attribution.clear();
		std::string attribution;
		U_AppendUnsignedNumberAsString(attribution, numericAttribution);
		m_EditAttribution.SetWindowText(attribution.c_str());
//		OnAttributionChange();
	}

	const bool& useRemainingSize = extensionConfig->m_simQuoteUseRemainingSize.GetValue();
	if(useRemainingSize != m_useRemainingSize)
	{
		m_useRemainingSize = useRemainingSize;
		m_CheckBoxUseRemainingSize.SetCheck(m_useRemainingSize ? BST_CHECKED : BST_UNCHECKED);
	}
	const unsigned int& remainingSize = extensionConfig->m_simQuoteRemainingSize.GetValue();
	if(remainingSize != m_remainingSize)
	{
		m_remainingSize = remainingSize;
		m_SpinRemainingSize.SetValue(m_remainingSize, false, true, false);
	}

	const bool& retailLiquidity = extensionConfig->m_simQuoteRetailLiquidity.GetValue();
	if(retailLiquidity != m_retailLiquidity)
	{
		m_retailLiquidity = retailLiquidity;
		m_CheckBoxRetailLiquidity.SetCheck(m_retailLiquidity ? BST_CHECKED : BST_UNCHECKED);
	}

	const bool& printable = extensionConfig->m_simQuotePrintable.GetValue();
	if(printable != m_printable)
	{
		m_printable = printable;
		m_CheckBoxPrintable.SetCheck(m_printable ? BST_CHECKED : BST_UNCHECKED);
	}

	m_EditRef.InitializeAutoAdd(extensionConfig->m_simQuoteRefAutoIncrement.GetValue());

	const std::string& messageName = extensionConfig->m_simQuoteMessageName.GetValue();
	if(messageName.empty())
	{
		m_ComboBoxMessageId.SetCurSel(0);
		m_messageName = m_ComboBoxMessageId.GetSelectedName();
	}
	else
	{
		sel = m_ComboBoxMessageId.FindItemByName(messageName.c_str());
		if(sel >= 0)
		{
			m_ComboBoxMessageId.SetCurSel(sel);
			m_messageName = messageName;
		}
		else
		{
			m_ComboBoxMessageId.SetCurSel(0);
			m_messageName = m_ComboBoxMessageId.GetSelectedName();
		}
	}
	m_messageId = m_ComboBoxMessageId.GetSelectedNumber();
//	if(m_bookId < ECNBOOK_NSDQ)m_bookId = ECNBOOK_NSDQ;
	UpdateMessageId();

	m_SpinRemainingSize.SetBkColorEnabled(m_messageId == M_BOOK_ORDER_EXECUTED_WITH_PRICE && m_useRemainingSize ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
	m_SpinPrice.SetBkColorEnabled(isPriceUsed() ? m_price.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
	m_SpinSize.SetBkColorEnabled(isSizeUsed() ? m_size || ModifyMessageAcceptsSizeZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor() : m_visualBase->GetGrayColor());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_simQuoteOption.GetValue(),
		extensionConfig->m_simQuoteOptionSuffix.GetValue(),
		extensionConfig->m_simQuoteOptionCall.GetValue(),
		extensionConfig->m_simQuoteOptionStrikePrice.GetValue(),
		extensionConfig->m_simQuoteOptionExpiration.GetValue());
/*
	SetCallPut(extensionConfig->m_simQuoteOptionCall.GetValue());
	
	int found = m_ComboBoxOptionSuffix.FindItem((const void*)extensionConfig->m_simQuoteOptionSuffix.GetValue());
	m_ComboBoxOptionSuffix.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeOptionSuffix();

	SetStrikePrice(extensionConfig->m_simQuoteOptionStrikePrice.GetValue());

	SetExpirationDate(extensionConfig->m_simQuoteOptionExpiration.GetValue());

	m_CheckBoxOption.SetCheck(extensionConfig->m_simQuoteOption.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnOption();
*/
#endif
	EnableApplyButton(false);
//	EnableSendMessageButton();
	m_modified = false;
}

void SampleDialogSettingSimulateQuote::FillQuote(MsgQuote& quote) const
{
	quote.m_bookID = m_bookId;//m_ComboBoxBookId.GetSelectedNumber();
	if(m_numericAttribution)
	{
		quote.m_mmid = m_numericAttribution;//U_StringToUInt(m_attribution.c_str());
	}
	else if(BOOK_LEVEL2 != m_bookId)
	{
		quote.m_mmid = *(unsigned int*)m_bookName.c_str();
	}
	quote.m_quoteCondition = m_quoteCondition;
	if(BOOK_LEVEL2 == m_bookId)quote.m_referenceNumber = m_marketCenter ? (unsigned __int64)('A' + m_marketCenter->GetId()) << 56 : 0;
	else U_ReadHexUnsignedNumberFromString(quote.m_referenceNumber, m_refNumber.c_str());//U_ReadUInt64HexToken(m_refNumber.c_str(), '\0', 0);
	quote.m_priceDollars = m_price.GetDollars();
	quote.m_priceFraction = m_price.GetDollarFraction();
	quote.m_size = m_size;
	quote.m_flags = m_retailLiquidity ? MsgQuote::RetailInterest : 0;
	quote.m_millisecond = TL_GetCurrentMillisecond();
}

bool SampleDialogSettingSimulateQuote::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateQuote::isSendMessageButtonEnabled()
		&& (BOOK_LEVEL2 != m_bookId || m_marketCenter && m_messageId == M_BOOK_MODIFY_QUOTE)
		&& (!isPriceUsed() || !m_price.isZero())
		&& (!isSizeUsed() || m_size != 0 || ModifyMessageAcceptsSizeZero());
}

const Message* SampleDialogSettingSimulateQuote::CreateMessage(WSABUF& wsaBuf)
{
//	if(!m_symbol.empty() && (!isPriceUsed() || !m_price.isZero()) && (!isSizeUsed() || m_size != 0) && (!isRefUsed() || m_EditRef.GetWindowTextLength() != 0))
	if(isSendMessageButtonEnabled())
	{
		switch(m_messageId)
		{
			case M_BOOK_NEW_QUOTE:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				U_CopyAndPad(m_messageNewQuote.m_symbol, sizeof(m_messageNewQuote.m_symbol) - 1, m_symbol.c_str(), '\0');
				m_messageNewQuote.m_side = m_side;
				FillQuote(m_messageNewQuote.m_quote);
				return &m_messageNewQuote;
			}
			else
			{
				m_messageNewQuoteSm.m_symbol = m_symbolMap;
				m_messageNewQuoteSm.m_side = m_side;
				FillQuote(m_messageNewQuoteSm.m_quote);
				return &m_messageNewQuoteSm;
			}
			break;

			case M_BOOK_MODIFY_QUOTE:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
#ifndef TAKION_NO_OPTIONS
				if(isOptionUsed())//m_optionMode)
				{
					OptionKey optionKey(m_strikePrice.GetCompactPrice(),
						m_optionCall,
						m_optionSuffix,
						m_expirationDay,
						m_expirationMonth,
						m_expirationYear);

					if(m_optionSuffix)U_CopyPlusOneCharAndPad(m_messageModifyOptionQuote.m_symbol, sizeof(m_messageModifyOptionQuote.m_symbol) - 1, m_symbol.c_str(), m_optionSuffix, '\0');
					else U_CopyAndPad(m_messageModifyOptionQuote.m_symbol, sizeof(m_messageModifyOptionQuote.m_symbol) - 1, m_symbol.c_str(), '\0');
					m_messageModifyOptionQuote.m_priceDollars = m_price.GetDollars();
					m_messageModifyOptionQuote.m_priceFraction = m_price.GetDollarFraction();
					m_messageModifyOptionQuote.m_size = m_size;
					m_messageModifyOptionQuote.m_side = m_side;
					m_messageModifyOptionQuote.m_marketCenterID = m_marketCenter ? 'A' + m_marketCenter->GetId() : 0;
					m_messageModifyOptionQuote.m_quoteCondition = m_quoteCondition;
					m_messageModifyOptionQuote.m_optionBlock = OptionBlock(optionKey.CalculateOptionBlock());
					m_messageModifyOptionQuote.m_time = TL_GetCurrentMillisecond();
//					m_messageModifyOptionQuote.m_flags;
					return &m_messageModifyOptionQuote;
				}
				else if(m_bookId == ECNBOOK_GLBX)
				{
					U_CopyAndPad(m_messageModifyFutureQuote.m_symbol, sizeof(m_messageModifyFutureQuote.m_symbol) - 1, m_symbol.c_str(), '\0');
					m_messageModifyFutureQuote.m_side = m_side;
					m_messageModifyFutureQuote.m_quote.m_bookID = m_bookId;// ECNBOOK_GLBX;//m_bookId;
					m_messageModifyFutureQuote.m_quote.m_quoteCondition = m_quoteCondition;
					if(BOOK_LEVEL2 == m_bookId)m_messageModifyFutureQuote.m_quote.m_referenceNumber = m_marketCenter ? (unsigned __int64)('A' + m_marketCenter->GetId()) << 56 : 0;
					else U_ReadHexUnsignedNumberFromString(m_messageModifyFutureQuote.m_quote.m_referenceNumber, m_refNumber.c_str());//U_ReadUInt64HexToken(m_refNumber.c_str(), '\0', 0);
					m_messageModifyFutureQuote.m_quote.m_priceDollars = m_price.GetDollars();
					m_messageModifyFutureQuote.m_quote.m_priceFraction = m_price.GetDollarFraction();
					m_messageModifyFutureQuote.m_quote.m_size = m_size;
					m_messageModifyFutureQuote.m_quote.m_flags = m_retailLiquidity ? MsgQuote::RetailInterest : 0;
					m_messageModifyFutureQuote.m_quote.m_millisecond = TL_GetCurrentMillisecond();
					return &m_messageModifyFutureQuote;
				}
				else
				{
#endif
					U_CopyAndPad(m_messageModifyQuote.m_symbol, sizeof(m_messageModifyQuote.m_symbol) - 1, m_symbol.c_str(), '\0');
					m_messageModifyQuote.m_side = m_side;
					FillQuote(m_messageModifyQuote.m_quote);
					return &m_messageModifyQuote;
#ifndef TAKION_NO_OPTIONS
				}
#endif
			}
			else
			{
				m_messageModifyQuoteSm.m_symbol = m_symbolMap;
				m_messageModifyQuoteSm.m_side = m_side;
				FillQuote(m_messageModifyQuoteSm.m_quote);
				return &m_messageModifyQuoteSm;
			}
			break;

			case M_BOOK_DELETE_QUOTE:
#ifndef TAKION_NO_OPTIONS
			if(m_bookId == ECNBOOK_GLBX)
			{
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					U_CopyAndPad(m_messageDeleteFutureQuote.m_symbol, sizeof(m_messageDeleteFutureQuote.m_symbol), m_symbol.c_str(), '\0');
					m_messageDeleteFutureQuote.m_bookID = m_bookId;
					U_ReadHexUnsignedNumberFromString(m_messageDeleteFutureQuote.m_referenceNumber, m_refNumber.c_str());
					m_messageDeleteFutureQuote.m_side = m_side;
					m_messageDeleteFutureQuote.m_time = TL_GetCurrentMillisecond();
					return &m_messageDeleteFutureQuote;
				}
			}
			else
#endif
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				U_CopyAndPad(m_messageDeleteQuote.m_symbol, sizeof(m_messageDeleteQuote.m_symbol), m_symbol.c_str(), '\0');
				m_messageDeleteQuote.m_bookID = m_bookId;
				U_ReadHexUnsignedNumberFromString(m_messageDeleteQuote.m_referenceNumber, m_refNumber.c_str());
				m_messageDeleteQuote.m_side = m_side;
				m_messageDeleteQuote.m_time = TL_GetCurrentMillisecond();
				return &m_messageDeleteQuote;
			}
			else
			{
				m_messageDeleteQuoteSm.m_symbol = m_symbolMap;
				m_messageDeleteQuoteSm.m_bookID = m_bookId;
				U_ReadHexUnsignedNumberFromString(m_messageDeleteQuoteSm.m_referenceNumber, m_refNumber.c_str());
				m_messageDeleteQuoteSm.m_side = m_side;
				m_messageDeleteQuoteSm.m_time = TL_GetCurrentMillisecond();
				return &m_messageDeleteQuoteSm;
			}
			break;

			case M_BOOK_ORDER_EXECUTED:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				U_CopyAndPad(m_messageExecuteQuote.m_symbol, sizeof(m_messageExecuteQuote.m_symbol), m_symbol.c_str(), '\0');
				m_messageExecuteQuote.m_bookID = m_bookId;
				U_ReadHexUnsignedNumberFromString(m_messageExecuteQuote.m_referenceNumber, m_refNumber.c_str());
				m_messageExecuteQuote.m_side = m_side;
				m_messageExecuteQuote.m_shares = m_size;
				m_messageExecuteQuote.m_time = TL_GetCurrentMillisecond();
				return &m_messageExecuteQuote;
			}
			else
			{
				m_messageExecuteQuoteSm.m_symbol = m_symbolMap;
				m_messageExecuteQuoteSm.m_bookID = m_bookId;
				U_ReadHexUnsignedNumberFromString(m_messageExecuteQuoteSm.m_referenceNumber, m_refNumber.c_str());
				m_messageExecuteQuoteSm.m_side = m_side;
				m_messageExecuteQuoteSm.m_shares = m_size;
				m_messageExecuteQuoteSm.m_time = TL_GetCurrentMillisecond();
				return &m_messageExecuteQuoteSm;
			}
			break;

			case M_BOOK_ORDER_EXECUTED_WITH_PRICE:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				U_CopyAndPad(m_messageExecuteWithPriceQuote.m_symbol, sizeof(m_messageExecuteWithPriceQuote.m_symbol), m_symbol.c_str(), '\0');
				m_messageExecuteWithPriceQuote.m_bookID = m_bookId;
				U_ReadHexUnsignedNumberFromString(m_messageExecuteWithPriceQuote.m_referenceNumber, m_refNumber.c_str());
				m_messageExecuteWithPriceQuote.m_side = m_side;
				m_messageExecuteWithPriceQuote.m_priceDollars = m_price.GetDollars();
				m_messageExecuteWithPriceQuote.m_priceFraction = m_price.GetDollarFraction();
				m_messageExecuteWithPriceQuote.m_shares = m_size;
				m_messageExecuteWithPriceQuote.m_remainingShares = m_useRemainingSize ? m_remainingSize : 0xFFFFFFFF;
				m_messageExecuteWithPriceQuote.m_bPrintable = m_printable;
				m_messageExecuteWithPriceQuote.m_time = TL_GetCurrentMillisecond();
				return &m_messageExecuteWithPriceQuote;
			}
			else
			{
				m_messageExecuteWithPriceQuoteSm.m_symbol = m_symbolMap;
				m_messageExecuteWithPriceQuoteSm.m_bookID = m_bookId;
				U_ReadHexUnsignedNumberFromString(m_messageExecuteWithPriceQuoteSm.m_referenceNumber, m_refNumber.c_str());
				m_messageExecuteWithPriceQuoteSm.m_side = m_side;
				m_messageExecuteWithPriceQuoteSm.m_priceDollars = m_price.GetDollars();
				m_messageExecuteWithPriceQuoteSm.m_priceFraction = m_price.GetDollarFraction();
				m_messageExecuteWithPriceQuoteSm.m_shares = m_size;
				m_messageExecuteWithPriceQuoteSm.m_remainingShares = m_useRemainingSize ? m_remainingSize : 0xFFFFFFFF;
				m_messageExecuteWithPriceQuote.m_bPrintable = m_printable;
				m_messageExecuteWithPriceQuoteSm.m_time = TL_GetCurrentMillisecond();
				return &m_messageExecuteWithPriceQuoteSm;
			}
			break;

			case M_BOOK_ORDER_CANCELED:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				U_CopyAndPad(m_messageCancelQuote.m_symbol, sizeof(m_messageCancelQuote.m_symbol), m_symbol.c_str(), '\0');
				m_messageCancelQuote.m_bookID = m_bookId;
				U_ReadHexUnsignedNumberFromString(m_messageCancelQuote.m_referenceNumber, m_refNumber.c_str());
				m_messageCancelQuote.m_side = m_side;
				m_messageCancelQuote.m_shares = m_size;
				m_messageCancelQuote.m_time = TL_GetCurrentMillisecond();
				return &m_messageCancelQuote;
			}
			else
			{
				m_messageCancelQuoteSm.m_symbol = m_symbolMap;
				m_messageCancelQuoteSm.m_bookID = m_bookId;
				U_ReadHexUnsignedNumberFromString(m_messageCancelQuoteSm.m_referenceNumber, m_refNumber.c_str());
				m_messageCancelQuoteSm.m_side = m_side;
				m_messageCancelQuoteSm.m_shares = m_size;
				m_messageCancelQuoteSm.m_time = TL_GetCurrentMillisecond();
				return &m_messageCancelQuoteSm;
			}
			break;

			case M_BOOK_TRADE:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				U_CopyAndPad(m_messageExecuteHiddenQuote.m_symbol, sizeof(m_messageExecuteHiddenQuote.m_symbol), m_symbol.c_str(), '\0');
				m_messageExecuteHiddenQuote.m_bookID = m_bookId;
				m_messageExecuteHiddenQuote.m_side = m_side;
				m_messageExecuteHiddenQuote.m_shares = m_size;
				m_messageExecuteHiddenQuote.m_priceDollars = m_price.GetDollars();
				m_messageExecuteHiddenQuote.m_priceFraction = m_price.GetDollarFraction();
				m_messageExecuteHiddenQuote.m_time = TL_GetCurrentMillisecond();
				return &m_messageExecuteHiddenQuote;
			}
			else
			{
				m_messageExecuteHiddenQuoteSm.m_symbol = m_symbolMap;
				m_messageExecuteHiddenQuoteSm.m_bookID = m_bookId;
				m_messageExecuteHiddenQuoteSm.m_side = m_side;
				m_messageExecuteHiddenQuoteSm.m_shares = m_size;
				m_messageExecuteHiddenQuoteSm.m_priceDollars = m_price.GetDollars();
				m_messageExecuteHiddenQuoteSm.m_priceFraction = m_price.GetDollarFraction();
				m_messageExecuteHiddenQuoteSm.m_time = TL_GetCurrentMillisecond();
				return &m_messageExecuteHiddenQuoteSm;
			}
			break;

			case M_BOOK_CROSS_TRADE:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				U_CopyAndPad(m_messageCrossTradeQuote.m_symbol, sizeof(m_messageCrossTradeQuote.m_symbol), m_symbol.c_str(), '\0');
				m_messageCrossTradeQuote.m_bookID = m_bookId;
//				m_messageCrossTradeQuote.m_side = m_side;
				m_messageCrossTradeQuote.m_shares = m_size;
				m_messageCrossTradeQuote.m_priceDollars = m_price.GetDollars();
				m_messageCrossTradeQuote.m_priceFraction = m_price.GetDollarFraction();
				m_messageCrossTradeQuote.m_time = TL_GetCurrentMillisecond();
				m_messageCrossTradeQuote.m_crossType = 'C';
				return &m_messageCrossTradeQuote;
			}
			else
			{
				m_messageCrossTradeQuoteSm.m_symbol = m_symbolMap;
				m_messageCrossTradeQuoteSm.m_bookID = m_bookId;
//				m_messageCrossTradeQuoteSm.m_side = m_side;
				m_messageCrossTradeQuoteSm.m_shares = m_size;
				m_messageCrossTradeQuoteSm.m_priceDollars = m_price.GetDollars();
				m_messageCrossTradeQuoteSm.m_priceFraction = m_price.GetDollarFraction();
				m_messageCrossTradeQuoteSm.m_time = TL_GetCurrentMillisecond();
				m_messageCrossTradeQuoteSm.m_crossType = 'C';
				return &m_messageCrossTradeQuoteSm;
			}
			break;
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulateQuote::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateQuote::DoDataExchange(pDX);
	DDX_Control(pDX, spinSimQuotePriceId, m_SpinPrice);
	DDX_Control(pDX, spinSimQuoteSizeId, m_SpinSize);
	DDX_Control(pDX, comboQuoteConditionId, m_ComboBoxQuoteCondition);
	DDX_Control(pDX, editSimAttributionId, m_EditAttribution);
	DDX_Control(pDX, spinSimQuoteRemainingSizeId, m_SpinRemainingSize);
	DDX_Control(pDX, checkBoxSimQuoteRemainingSizeId, m_CheckBoxUseRemainingSize);
	DDX_Control(pDX, checkBoxSimQuotePrintableId, m_CheckBoxPrintable);
	DDX_Control(pDX, checkBoxSimQuoteRetailLiquidityId, m_CheckBoxRetailLiquidity);
	DDX_Control(pDX, comboSimQuoteMessageId, m_ComboBoxMessageId);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateQuote, BaseDialogSettingSimulateQuote)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editSimAttributionId, OnAttributionChange)
	ON_EN_UPDATE(editSimAttributionId, OnAttributionUpdate)
	ON_BN_CLICKED(checkBoxSimQuoteRemainingSizeId, OnRemainingSize)
	ON_BN_CLICKED(checkBoxSimQuotePrintableId, OnPrintable)
	ON_BN_CLICKED(checkBoxSimQuoteRetailLiquidityId, OnRetailLiquidity)
	ON_CBN_SELCHANGE(comboQuoteConditionId, OnSelchangeQuoteCondition)
	ON_CBN_SELCHANGE(comboSimQuoteMessageId, OnSelchangeMessageId)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateQuote::Level2Changed()
{
	if(BOOK_LEVEL2 == m_bookId)
	{
		m_marketCenter = TU_FindMarketCenter(m_numericAttribution);
		if(!m_marketCenter)
		{
			m_EditAttribution.Invalidate(FALSE);
			EnableSendMessageButton();
		}
	}
	else if(m_marketCenter)
	{
		m_marketCenter = nullptr;
	}
	else
	{
		m_EditAttribution.Invalidate(FALSE);
		EnableSendMessageButton();
	}
	if(isSizeUsed())m_SpinSize.SetBkColorEnabled(m_size || ModifyMessageAcceptsSizeZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
}

void SampleDialogSettingSimulateQuote::OnPrintable()
{
	const bool printable = m_CheckBoxPrintable.GetCheck() == BST_CHECKED;
	if(printable != m_printable)
	{
		m_printable = printable;
		m_modified = true;
//		if(m_messageId == M_BOOK_ORDER_EXECUTED_WITH_PRICE)m_SpinRemainingSize.SetBkColorEnabled(m_useRemainingSize ? GetSysColor(COLOR_WINDOW) : m_takionVisual->GetGrayColor());
	}
}

void SampleDialogSettingSimulateQuote::OnRetailLiquidity()
{
	const bool retailLiquidity = m_CheckBoxRetailLiquidity.GetCheck() == BST_CHECKED;
	if(retailLiquidity != m_retailLiquidity)
	{
		m_retailLiquidity = retailLiquidity;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateQuote::OnRemainingSize()
{
	const bool useRemainingSize = m_CheckBoxUseRemainingSize.GetCheck() == BST_CHECKED;
	if(useRemainingSize != m_useRemainingSize)
	{
		m_useRemainingSize = useRemainingSize;
		m_modified = true;
		if(m_messageId == M_BOOK_ORDER_EXECUTED_WITH_PRICE)m_SpinRemainingSize.SetBkColorEnabled(m_useRemainingSize ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
	}
}

void SampleDialogSettingSimulateQuote::OnAttributionUpdate()
{
	m_EditAttribution.TryUpdate();
}

void SampleDialogSettingSimulateQuote::OnAttributionChange()
{
	CString text;
	m_EditAttribution.GetWindowText(text);
	if(strcmp(m_attribution.c_str(), text))
	{
		m_attribution = text;
		m_numericAttribution = U_RepresentStringAsUnsignedNumber<unsigned int>(m_attribution.c_str());
		m_modified = true;
		UpdateMarketCenter();
	}
}

void SampleDialogSettingSimulateQuote::UpdateMarketCenter()
{
	if(BOOK_LEVEL2 == m_bookId)
	{
		const MarketCenter* mc =
#ifndef TAKION_NO_OPTIONS
			isOptionUsed() ? TU_FindOptionMarketCenter(m_numericAttribution):
#endif
			TU_FindMarketCenter(m_numericAttribution);
		if(mc != m_marketCenter)
		{
			switch(m_messageId)
			{
//				case M_BOOK_NEW_QUOTE:
				case M_BOOK_MODIFY_QUOTE:
				if(!mc != !m_marketCenter)m_EditAttribution.Invalidate(FALSE);
				break;
			}
			m_marketCenter = mc;
			EnableSendMessageButton();
		}
	}
}

void SampleDialogSettingSimulateQuote::OnSelchangeQuoteCondition()
{
	const unsigned char quoteCondition = m_ComboBoxQuoteCondition.GetSelectedChar();
	if(quoteCondition != m_quoteCondition)
	{
		m_quoteCondition = quoteCondition;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateQuote::OnSelchangeMessageId()
{
	unsigned short messageId;
	int sel = m_ComboBoxMessageId.GetCurSel();
	if(sel >= 0)
	{
		const StringUnsignedNumber<unsigned short>* item = (const StringUnsignedNumber<unsigned short>*)m_ComboBoxMessageId.GetItemDataPtr(sel);
		messageId = item->GetNumber();
	}
	else
	{
		messageId = 0;
	}
	if(messageId != m_messageId)
	{
		m_messageId = messageId;
		m_messageName = m_ComboBoxMessageId.GetSelectedName();
		m_modified = true;
#ifndef TAKION_NO_OPTIONS
		m_CheckBoxOption.Invalidate(FALSE);
		OptionModeChanged();
#endif
		UpdateMessageId();
	}
}

bool SampleDialogSettingSimulateQuote::isPriceUsed() const
{
	switch(m_messageId)
	{
//#ifndef TAKION_NO_OPTIONS
//		case M_FUT_BOOK_DELETE_QUOTE:
//#endif
		case M_BOOK_DELETE_QUOTE:
		case M_BOOK_ORDER_CANCELED:
		case M_BOOK_ORDER_EXECUTED:
		return false;

		default:
		return true;
	}
}

bool SampleDialogSettingSimulateQuote::isSizeUsed() const
{
	switch(m_messageId)
	{
		case M_BOOK_DELETE_QUOTE:
		return false;

		default:
		return true;
	}
}

bool SampleDialogSettingSimulateQuote::isQuoteConditionUsed() const
{
	switch(m_messageId)
	{
		case M_BOOK_NEW_QUOTE:
		case M_BOOK_MODIFY_QUOTE:
		return true;

		default:
		return true;
	}
}

bool SampleDialogSettingSimulateQuote::isRefUsed() const
{
	switch(m_messageId)
	{
		case M_BOOK_TRADE:
		case M_BOOK_CROSS_TRADE:
		return false;

		default:
		return BaseDialogSettingSimulateQuote::isRefUsed();
	}
}

bool SampleDialogSettingSimulateQuote::ModifyMessageAcceptsSizeZero() const
{
	return BOOK_LEVEL2 == m_bookId
#ifndef TAKION_NO_OPTIONS
		|| ECNBOOK_GLBX == m_bookId
#endif
		;
}

void SampleDialogSettingSimulateQuote::UpdateMessageId()
{
	EnableSendMessageButton();
	m_EditAttribution.Invalidate(FALSE);
	m_EditRef.Invalidate(FALSE);
	m_CheckBoxUseRemainingSize.Invalidate(FALSE);
	m_CheckBoxPrintable.Invalidate(FALSE);
	m_CheckBoxRetailLiquidity.Invalidate(FALSE);
	switch(m_messageId)
	{
//		case M_OPT_LEVEL2:
		case M_BOOK_MODIFY_QUOTE:
		case M_BOOK_NEW_QUOTE:
		m_SpinPrice.SetBkColorEnabled(m_price.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
		m_SpinSize.SetBkColorEnabled(m_size || ModifyMessageAcceptsSizeZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
		m_ComboBoxQuoteCondition.SetInvalid(false);
		if(m_useRemainingSize)m_SpinRemainingSize.SetBkColorEnabled(m_visualBase->GetGrayColor());
		break;

		case M_BOOK_ORDER_EXECUTED_WITH_PRICE:
		m_SpinPrice.SetBkColorEnabled(m_price.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
		m_SpinSize.SetBkColorEnabled(m_size || BOOK_LEVEL2 == m_bookId ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
		m_ComboBoxQuoteCondition.SetInvalid(true);
		if(m_useRemainingSize)m_SpinRemainingSize.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
		break;

		case M_BOOK_TRADE:
		case M_BOOK_CROSS_TRADE:
		m_SpinPrice.SetBkColorEnabled(m_price.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
		m_SpinSize.SetBkColorEnabled(m_size || BOOK_LEVEL2 == m_bookId ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
		m_ComboBoxQuoteCondition.SetInvalid(true);
		if(m_useRemainingSize)m_SpinRemainingSize.SetBkColorEnabled(m_visualBase->GetGrayColor());
		break;

		case M_BOOK_DELETE_QUOTE:
		m_SpinPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
		m_SpinSize.SetBkColorEnabled(m_visualBase->GetGrayColor());
		m_ComboBoxQuoteCondition.SetInvalid(true);
		if(m_useRemainingSize)m_SpinRemainingSize.SetBkColorEnabled(m_visualBase->GetGrayColor());
		break;

		case M_BOOK_ORDER_CANCELED:
		case M_BOOK_ORDER_EXECUTED:
		m_SpinPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
		m_SpinSize.SetBkColorEnabled(m_size || BOOK_LEVEL2 == m_bookId ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
		m_ComboBoxQuoteCondition.SetInvalid(true);
		if(m_useRemainingSize)m_SpinRemainingSize.SetBkColorEnabled(m_visualBase->GetGrayColor());
		break;
	}
}

LRESULT SampleDialogSettingSimulateQuote::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimQuotePriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPrice, 10000);
			bool changed = isPriceUsed() && m_price.isZero() != price.isZero();
			if(changed)m_SpinPrice.SetBkColorEnabled(m_price.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(price != m_price)
			{
				m_price = price;
				m_modified = true;
			}
			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimQuoteSizeId:
		{
			const unsigned int size = m_SpinSize.GetValue();
			bool changed = isSizeUsed() && !m_size != !size && !ModifyMessageAcceptsSizeZero();// BOOK_LEVEL2 != m_bookId;
			if(changed)m_SpinSize.SetBkColorEnabled(size || ModifyMessageAcceptsSizeZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(size != m_size)
			{
				m_size = size;
				m_modified = true;
			}
			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimQuoteRemainingSizeId:
		{
			const unsigned int size = m_SpinRemainingSize.GetValue();
//			bool changed = isSizeUsed() && !m_size != !size;
//			if(changed)m_SpinSize.SetBkColorEnabled(size ? GetSysColor(COLOR_WINDOW) : m_takionVisual->GetWarningColor());
			if(size != m_remainingSize)
			{
				m_remainingSize = size;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		default:
		BaseDialogSettingSimulateQuote::OnSpinChange(w, l);
		break;
	}
	return 0;
}

HBRUSH SampleDialogSettingSimulateQuote::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSimAttributionId:
		switch(m_messageId)
		{
//			case M_OPT_LEVEL2:
			case M_BOOK_NEW_QUOTE:
			if(BOOK_LEVEL2 == m_bookId)
			{
				pDC->SetBkMode(TRANSPARENT);
				return *m_visualBase->GetGrayBrush();
			}
			break;

			case M_BOOK_MODIFY_QUOTE:
			if(BOOK_LEVEL2 == m_bookId)
			{
				if(!m_marketCenter)
				{
					pDC->SetBkMode(TRANSPARENT);
					return *m_visualBase->GetWarningBrush();
				}
			}
			break;

			default:
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		break;

		case checkBoxSimQuoteRemainingSizeId:
		case checkBoxSimQuotePrintableId:
		if(m_messageId != M_BOOK_ORDER_EXECUTED_WITH_PRICE)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;

		case checkBoxSimQuoteRetailLiquidityId:
		switch(m_messageId)
		{
			case M_BOOK_NEW_QUOTE:
			case M_BOOK_MODIFY_QUOTE:
			break;

			default:
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;
	}
	return BaseDialogSettingSimulateQuote::OnCtlColor(pDC, pWnd, nCtlColor);
}
