#include "stdafx.h"
#include "SampleDialogSettingSimulateSnapshot.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateSnapshot::SampleDialogSettingSimulateSnapshot(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateOptionSource(mainWnd, parentTab, "Snapshot"),
	m_ComboBoxMarketStatus(DT_CENTER),
	m_SpinRoundLot(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_ComboBoxFinancialStatus(m_visualBase->GetGrayColor()),
	m_ComboBoxTradingState(m_visualBase->GetGrayColor()),

	m_attachmentCursor(m_attachment),
	m_attachmentLength(0),
	m_remainingLength(sizeof(m_attachment)),

	m_marketStatus(0),
	m_marketStatusCount(0),
	m_roundLot(100),
	m_roundLotsOnly(false),
	m_marketCategory(MCF_AMEX),
	m_primaryExchange(PE_OTHR),
	m_financialStatus(' '),
	m_tradingState('T'),
	m_tradingActionReason(0)
/*
	m_ComboBoxMessageType(false),
	m_ComboBoxBook("All", false),
	m_book(0),
	m_allBooksOrdinal(0xFF)
//	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4)
*/
{
	AddListBox(&m_ListBoxSnapshotFlags);

	m_SpinRoundLot.AddSpinCell(1, false);
	m_SpinRoundLot.AddSpinCell(100, false);
	m_SpinRoundLot.AddSpinCell(1000, true);

	m_ComboBoxFinancialStatus.SetColorInvalid(m_visualBase->GetGrayColor());
	m_ComboBoxTradingState.SetColorInvalid(m_visualBase->GetGrayColor());
//	AddPriceSpinCells(m_SpinPrice);
	m_emptySymbolValid = true;

	m_refreshStock.m_bookID = 0;
	m_refreshStockSm.m_bookID = 0;

#ifndef TAKION_NO_OPTIONS
	m_refreshUnderlier.m_time = TD_GetOptionUnderlierMarketDataSubscriptionRequestId();
	m_refreshOption.m_time = TD_GetOptionMarketDataSubscriptionRequestId();
	m_refreshFuture.m_requestID = TD_GetFutureMarketDataSubscriptionRequestId();
#endif
	m_ComboBoxMarketStatus.SetColorInvalid(m_visualBase->GetGrayColor());
	m_ComboBoxMarketStatus.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_bottomControl += vgap;
	int sideWidth = 32;
	const int priceWidth = 75;
	const int width = priceWidth + sideWidth;
	const int widthHalf = width / 2;

	const int firstColumnWidthHalf = m_firstColumnWidth / 2;
	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, firstColumnWidthHalf, 400, comboBoxSimSnapshotMarketStatusId);
	AddStaticIndirect("MarketStatus", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth - firstColumnWidthHalf, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth + firstColumnWidthHalf, staticHeight, staticSimSnapshotMarketStatusId);

	AddStaticIndirect("Flags", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, widthHalf, staticHeight, staticSimSnapshotFlagsId);
	int lbHeight = SF_Count * checkBoxHeight + 2 * vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, m_xc, m_bottomControl += staticHeight, widthHalf, lbHeight, listBoxSimSnapshotFlagsId);

	AddStaticIndirect("Description", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, m_bottomControl += lbHeight + vgap, widthHalf, staticHeight, staticSimSnapshotDescriptionId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, m_bottomControl + staticHeight, width, editHeight, editSimSnapshotDescriptionId);

	m_bottomControl += editHeight;

	m_xc = m_firstColumnWidth + m_firstColumnWidth + 4 * hgap;
	const int sizeWidth = 54;
	const int halfWidth = sizeWidth / 2;
	const int staticWidth = sizeWidth * 2;//3 / 2;
	const int secondColumnWidth = staticWidth;//sizeWidth;// + sideWidth;

	m_maxControlWidth += secondColumnWidth + 4 * hgap;

	int y = vgap;

	AddStaticIndirect("Round Lot", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y, staticWidth, staticHeight, staticSimSnapshotRoundLotId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += staticHeight, sizeWidth, spinHeight, spinSimSnapshotRoundLotId);

	AddButtonIndirect("Round Lots only", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, y += spinHeight + vgap, staticWidth, checkBoxHeight, checkBoxSimSnapshotRoundLotsOnlyId);

	AddStaticIndirect("Market Category", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y += checkBoxHeight + vgap, staticWidth, staticHeight, staticSimSnapshotMarketCategoryId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += staticHeight, sizeWidth, 400, comboBoxSimSnapshotMarketCategoryId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += comboBoxHeight, halfWidth, 400, comboBoxSimSnapshotPrimaryExchangeId);
	AddStaticIndirect("Primary Exchange", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + halfWidth, y + (comboBoxHeight - staticHeight) / 2, secondColumnWidth - halfWidth, staticHeight, staticSimSnapshotPrimaryExchangeId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL|WS_GROUP, 0, m_xc, y += comboBoxHeight + vgap, halfWidth, 400, comboBoxSimSnapshotFinancialStatusId);
	AddStaticIndirect("Fin Status", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + halfWidth, y + (comboBoxHeight - staticHeight) / 2, secondColumnWidth - halfWidth, staticHeight, staticSimSnapshotFinancialStatusId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL|WS_GROUP, 0, m_xc, y += comboBoxHeight + vgap, halfWidth, 400, comboBoxSimSnapshotTradingStateId);
	AddStaticIndirect("Trading State", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + halfWidth, y + (comboBoxHeight - staticHeight) / 2, secondColumnWidth - halfWidth, staticHeight, staticSimSnapshotTradingStateId);

	AddStaticIndirect("Trading Action Reason", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y += comboBoxHeight + vgap, staticWidth, staticHeight, staticSimSnapshotTradingActionReasonId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, y += staticHeight, sizeWidth, editHeight, editSimSnapshotTradingActionReasonId);
}

BOOL SampleDialogSettingSimulateSnapshot::OnInitDialog()
{
	m_marketStatus = 0;
	m_description.clear();
	m_roundLot = 100;
	m_roundLotsOnly = false;
	m_marketCategory = MC_OTHR;
	m_primaryExchange = PE_OTHR;
	m_financialStatus = ' ';
	m_tradingState = 'T';
	m_tradingActionReason = 0;
/*
	m_messageType = MTR_QUOTE;
	m_book = 0;
	m_allBooksOrdinal = 0xFF;
*/
	BOOL ret = BaseDialogSettingSimulateOptionSource::OnInitDialog();

#ifndef TAKION_NO_OPTIONS
	m_ComboBoxSource.AddString("OptionData");
#endif

	m_EditDescription.SetLimitText(MAX_SECURITY_NAME);
	m_EditTradingActionReason.SetLimitText(sizeof(unsigned int));
/*
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("", "Uninitialized"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("O", "Message Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("S", "System Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("Q", "Market Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("M", "Post Market Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("E", "Post System Hours"));
	m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("C", "Post Message Hours"));
*/
	m_ComboBoxMarketStatus.AddItem('\0', "Uninitialized");
	m_ComboBoxMarketStatus.AddItem('O', "Message Hours");
	m_ComboBoxMarketStatus.AddItem('S', "System Hours");
	m_ComboBoxMarketStatus.AddItem('Q', "Market Hours");
	m_ComboBoxMarketStatus.AddItem('M', "Post Market Hours");
	m_ComboBoxMarketStatus.AddItem('E', "Post System Hours");
	m_ComboBoxMarketStatus.AddItem('C', "Post Message Hours");

	m_marketStatusCount = (unsigned char)m_ComboBoxMarketStatus.GetItemCount();
	m_ComboBoxMarketStatus.SetCurSel(0);
	OnSelchangeMarketStatus();
/*	
	m_ComboBoxMessageType.AddString("Quote");
	m_ComboBoxMessageType.AddString("Imbalance");

	m_ComboBoxMessageType.SetCurSel(0);
	m_ComboBoxSource.SetInvalid(MTR_QUOTE == m_messageType);
	m_ComboBoxBook.SetInvalid(MTR_QUOTE != m_messageType);
	OnSelchangeMessageType();

	unsigned char i = 0;
	const Ecn* ecnVector[ECNBOOK_COUNT];
	const Ecn** cursor = ecnVector;
	for(; i < ECNBOOK_COUNT; ++i, ++cursor)
	{
		*cursor = nullptr;
	}
	const Ecn* const* ecnPtr = TU_GetEcnBookIds();
	const Ecn* ecn = *ecnPtr;
	for(; ecn; ++ecnPtr, ecn = *ecnPtr)
	{
		i = ecn->GetId();
		if(i < ECNBOOK_COUNT)
		{
			if(!ecnVector[i])ecnVector[i] = ecn;
//			m_ComboBoxBook.AddString((const char*)ecn);
		}
	}
	for(i = 0, cursor = ecnVector; i < ECNBOOK_COUNT; ++i, ++cursor)
	{
		if(*cursor)m_ComboBoxBook.AddString((const char*)*cursor);
		else
		{
			if(m_allBooksOrdinal == 0xFF)m_allBooksOrdinal = i;
			m_ComboBoxBook.AddString(nullptr);
		}
	}

	m_ComboBoxBook.SetCurSel(0);
	OnSelchangeBook();
*/

	m_SpinRoundLot.SetValue(m_roundLot, false, true, false);
	m_CheckBoxRoundLotsOnly.SetCheck(m_roundLotsOnly ? BST_CHECKED : BST_UNCHECKED);

	m_ComboBoxMarketCategory.SetCurSel(m_marketCategory);

//	m_ComboBoxPrimaryExchange
	InitComboBoxNameDescription(m_ComboBoxPrimaryExchange, m_primaryExchange);

	InitComboBoxChar(m_ComboBoxFinancialStatus, m_financialStatus);
/*
	m_ComboBoxFinancialStatus.AddString(nullptr);
	m_ComboBoxFinancialStatus.AddString((const char*)' ');
	m_ComboBoxFinancialStatus.FillCharRange('A', 'Z', false);
	m_ComboBoxFinancialStatus.FillCharRange('0', '9', false);
	int sel = m_ComboBoxFinancialStatus.FindItem((const void*)m_financialStatus);
	if(sel >= 0)m_ComboBoxFinancialStatus.SetCurSel(sel);
	else
	{
		m_ComboBoxFinancialStatus.SetCurSel(0);
		m_financialStatus = (char)m_ComboBoxFinancialStatus.GetItemDataPtr(0);
	}
*/
	InitComboBoxChar(m_ComboBoxTradingState, m_tradingState);

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateSnapshot::UpdateSettings()
{
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simSnapshotSymbolType.SetValue(symbolType);

	extensionConfig->m_simSnapshotSymbol.SetValue(m_symbol);
	extensionConfig->m_simSnapshotSymbolMap.SetValue(m_symbolMap);

//	unsigned char source = m_source;
	const unsigned char source = GetValidSource(m_source);
/*
#ifndef TAKION_NO_OPTIONS
	if(source > ST_Count)source = ST_Count;
#else
	if(source >= ST_Count)source = ST_Count - 1;
#endif
*/
	extensionConfig->m_simSnapshotSource.SetValue(source);

	unsigned char marketStatus = m_marketStatus;
	if(marketStatus >= m_marketStatusCount)marketStatus = 0;
	extensionConfig->m_simSnapshotMarketStatus.SetValue(marketStatus);

	extensionConfig->m_simSnapshotPrimaryExchange.SetValue(m_primaryExchange);
/*
	unsigned char messageType = m_messageType;
	if(messageType >= MTR_Count)messageType = MTR_Count - 1;
	extensionConfig->m_simResetMessageType.SetValue(messageType);
*/
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_simSnapshotOption.SetValue(m_optionMode);
	extensionConfig->m_simSnapshotOptionCall.SetValue(m_optionCall);
	extensionConfig->m_simSnapshotOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_simSnapshotOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_simSnapshotOptionExpiration.SetValue(GetExpirationDate());
#endif

	extensionConfig->m_simSnapshotFlags.SetValue(m_ListBoxSnapshotFlags.GetFilter());
/*
//	extensionConfig->m_simClsPrice.SetValue(m_price);
	unsigned char book = m_book;
//	if(messageType >= MTR_Count)messageType = MTR_Count - 1;
	extensionConfig->m_simResetBook.SetValue(book);
*/

	extensionConfig->m_simSnapshotDescription.SetValue(m_description);
	extensionConfig->m_simSnapshotRoundLot.SetValue(m_roundLot);
	extensionConfig->m_simSnapshotRoundLotsOnly.SetValue(m_roundLotsOnly);
	extensionConfig->m_simSnapshotMarketCategory.SetValue(m_marketCategory);
	extensionConfig->m_simSnapshotFinancialStatus.SetValue(m_financialStatus);
	extensionConfig->m_simSnapshotTradingState.SetValue(m_tradingState);
	extensionConfig->m_simSnapshotTradingActionReason.SetValue(m_tradingActionReason);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateSnapshot::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simSnapshotSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_numericSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(m_symbol.c_str(), sizeof(unsigned __int64) - 1);

		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simSnapshotSymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simSnapshotSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

//	unsigned char source = extensionConfig->m_simSnapshotSource.GetValue();
	const unsigned char source = GetValidSource(extensionConfig->m_simSnapshotSource.GetValue());
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}

	unsigned char marketStatus = extensionConfig->m_simSnapshotMarketStatus.GetValue();
	if(marketStatus >= m_marketStatusCount)marketStatus = 0;
	if(marketStatus != m_marketStatus)
	{
		m_marketStatus = marketStatus;
		m_ComboBoxMarketStatus.SetCurSel(m_marketStatus);
		OnSelchangeMarketStatus();
	}

	const unsigned int& roundLot = extensionConfig->m_simSnapshotRoundLot.GetValue();
	if(roundLot != m_roundLot)
	{
		m_roundLot = roundLot;
		m_SpinRoundLot.SetValue(m_roundLot, false, true, false); 
	}

	const bool& roundLotsOnly = extensionConfig->m_simSnapshotRoundLotsOnly.GetValue();
	if(roundLotsOnly != m_roundLotsOnly)
	{
		m_roundLotsOnly = roundLotsOnly;
		m_CheckBoxRoundLotsOnly.SetCheck(m_roundLotsOnly ? BST_CHECKED : BST_UNCHECKED);
	}
	unsigned char marketCategory = extensionConfig->m_simSnapshotMarketCategory.GetValue();
	if(marketCategory >= TU_GetMarketCategoryCount())marketCategory = 0;
	if(marketCategory != m_marketCategory)
	{
		m_marketCategory = marketCategory;
		m_ComboBoxMarketCategory.SetCurSel(m_marketCategory);
	}

	int found = m_ComboBoxPrimaryExchange.FindItemByCharName(extensionConfig->m_simSnapshotPrimaryExchange.GetValue());
	m_ComboBoxPrimaryExchange.SetCurSel(found >= 0 ? found : 0);
	OnSelchangePrimaryExchange();

	found = m_ComboBoxFinancialStatus.FindItem((const void*)extensionConfig->m_simSnapshotFinancialStatus.GetValue());
	m_ComboBoxFinancialStatus.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeFinancialStatus();

	found = m_ComboBoxTradingState.FindItem((const void*)extensionConfig->m_simSnapshotTradingState.GetValue());
	m_ComboBoxTradingState.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeTradingState();

	const unsigned int& tradingActionReason = extensionConfig->m_simSnapshotTradingActionReason.GetValue();
	if(tradingActionReason != m_tradingActionReason)
	{
		m_tradingActionReason = tradingActionReason;
		char buf[sizeof(unsigned int) + 1];
		*(unsigned int*)buf = m_tradingActionReason;
		buf[sizeof(unsigned int)] = '\0';
		m_EditTradingActionReason.SetWindowText(buf);
//		m_EditDescription.Invalidate(FALSE);
	}
/*
	unsigned char messageType = extensionConfig->m_simResetMessageType.GetValue();
	if(messageType >= MTR_Count)messageType = MTR_Count - 1;
	if(messageType != m_messageType)
	{
		m_messageType = messageType;
		m_ComboBoxMessageType.SetCurSel(m_messageType);
		OnSelchangeMessageType();
	}
	unsigned char book = extensionConfig->m_simResetBook.GetValue();
//	if(messageType >= MTR_Count)messageType = MTR_Count - 1;
	if(book != m_book)
	{
		m_book = book;
		m_ComboBoxBook.SetCurSel(m_book);
		OnSelchangeBook();
	}
*/
/*
	const Price& price = extensionConfig->m_simReset.GetValue();
	if(price != m_price)
	{
		m_price = price;
		FillSpinControlFromPrice(m_price, m_SpinPrice, 10000, false);
//		m_SpinPriceLow.SetBkColorEnabled(m_priceLow.isZero() ? GetSysColor(COLOR_WINDOW) : m_takionVisual->GetWarningColor());
	}
*/
	m_ListBoxSnapshotFlags.SetFilter(extensionConfig->m_simSnapshotFlags.GetValue());

	const std::string& description = extensionConfig->m_simSnapshotDescription.GetValue();
	if(description != m_description)
	{
		m_description = description;
		m_EditDescription.SetWindowText(description.c_str());
		m_EditDescription.Invalidate(FALSE);
	}

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_simSnapshotOption.GetValue(),
		extensionConfig->m_simSnapshotOptionSuffix.GetValue(),
		extensionConfig->m_simSnapshotOptionCall.GetValue(),
		extensionConfig->m_simSnapshotOptionStrikePrice.GetValue(),
		extensionConfig->m_simSnapshotOptionExpiration.GetValue());
#endif

	EnableApplyButton(false);
	EnableSendMessageButton();

	m_modified = false;
}
/*
unsigned char SampleDialogSettingSimulateSnapshot::GetRefreshFlags() const
{
	unsigned char ret = 0;
	const unsigned char& flags = m_ListBoxSnapshotFlags.GetFilter();
	if(flags)
	{
		if(flags & (1 << SF_BLOCK_END))ret |= REFRESH_EndOfBlock;
		if(flags & (1 << SF_REFRESH_END))ret |= REFRESH_EndOfRefresh;
		if(flags & (1 << SF_FAILED))ret |= REFRESH_RefreshFailed;
	}
	return ret;
}
*/
unsigned char SampleDialogSettingSimulateSnapshot::FillAttachment()
{
	m_attachmentCursor = m_attachment;
	m_attachmentLength = 0;
	m_remainingLength = sizeof(m_attachment);
	unsigned short length;
	unsigned char steps = 0;
#ifndef TAKION_NO_OPTIONS
	bool optionUsed = isOptionUsed();
	if(optionUsed)
	{
	}
	else if(m_source == ST_Count)//Underlier
	{
	}
	else
	{
#endif
		if(!m_description.empty())
		{
			length = MAX_SECURITY_NAME;//(unsigned short)m_description.length();
			if(m_remainingLength <= length)return steps;

			steps |= SRS_DESCRIPTION;

			U_CopyAndPad(m_attachmentCursor, MAX_SECURITY_NAME, m_description.c_str(), '\0');
			MoveForwardInAttachment(length);
			*m_attachmentCursor = '\0';
			MoveForwardInAttachment(1);
		}

		steps |= SRS_ATTRIBUTES;
		*(unsigned int*)m_attachmentCursor = m_roundLot;
		MoveForwardInAttachment(sizeof(unsigned int));

		*(bool*)m_attachmentCursor = m_roundLotsOnly;
		MoveForwardInAttachment(sizeof(bool));

		*m_attachmentCursor = TU_GetMarketCategoryCharByMarketCategoryId(m_marketCategory);
		MoveForwardInAttachment(sizeof(char));

		*m_attachmentCursor = m_financialStatus;
		MoveForwardInAttachment(sizeof(char));

		*m_attachmentCursor = m_tradingState;
		MoveForwardInAttachment(sizeof(char));

		*(unsigned int*)m_attachmentCursor = m_tradingActionReason;
		MoveForwardInAttachment(sizeof(unsigned int));

		*m_attachmentCursor = m_primaryExchange;
		MoveForwardInAttachment(sizeof(char));
#ifndef TAKION_NO_OPTIONS
	}
#endif
	switch(m_source)
	{
		case ST_MARKET_DATA:
#ifndef TAKION_NO_OPTIONS
		if(optionUsed)
		{
		}
		else
#endif
		{
		}
		break;

		case ST_MARKET_SORTER:
		break;
	}
	return steps;
}

const Message* SampleDialogSettingSimulateSnapshot::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
		unsigned char steps = FillAttachment();
		wsaBuf.buf = m_attachment;
		wsaBuf.len = m_attachmentLength;

#ifndef TAKION_NO_OPTIONS

		if(isOptionUsed())
		{
			OptionSymbolKey optionKey(m_numericSymbol, //U_StringToUInt64Term0(m_symbol.c_str()),
				m_strikePrice.GetCompactPrice(),
				m_optionCall,
				m_optionSuffix,
				m_expirationDay,
				m_expirationMonth,
				m_expirationYear);

			optionKey.AddSuffix(m_optionSuffix);

			m_refreshOption.m_flags = m_ListBoxSnapshotFlags.GetRefreshFlags();//REFRESH_RefreshFailed;REFRESH_EndOfRefresh;REFRESH_EndOfBlock
			U_CopyAndPad(m_refreshOption.m_symbol, sizeof(m_refreshOption.m_symbol), optionKey.GetSymbol(), '\0');
			m_refreshOption.m_optionBlock = OptionBlock(optionKey.CalculateOptionBlock());
			m_refreshOption.m_length = sizeof(m_refreshOption) + m_attachmentLength;
			m_refreshOption.m_optionType = eoNum;
			U_CopyAndPad(m_refreshOption.m_rootSymbol, sizeof(m_refreshOption.m_rootSymbol), m_symbol.c_str(), '\0');
			m_refreshOption.m_steps = steps;
			m_refreshOption.m_time = TL_GetCurrentMillisecond();
/*
			m_closeOptionMd.m_closePriceDollar = m_price.GetDollars();
			m_closeOptionMd.m_closePriceFraction = m_price.GetDollarFraction();
			m_closeOptionMd.m_time = TL_GetCurrentMillisecond();
			m_closeOptionMd.m_optionBlock = OptionBlock(optionKey.CalculateOptionBlock());
*/
			return &m_refreshOption;
		}
		else if(m_source == ST_Count)
		{
			m_refreshUnderlier.m_flags = m_ListBoxSnapshotFlags.GetRefreshFlags();//REFRESH_RefreshFailed;REFRESH_EndOfRefresh;REFRESH_EndOfBlock
			U_CopyAndPad(m_refreshUnderlier.m_symbol, sizeof(m_refreshUnderlier.m_symbol), m_symbol.c_str(), '\0');
			m_refreshUnderlier.m_length = sizeof(m_refreshUnderlier) + m_attachmentLength;
			m_refreshUnderlier.m_steps = steps;
			m_refreshUnderlier.m_time = TL_GetCurrentMillisecond();
			return &m_refreshUnderlier;
		}
		else
		{
			if(m_symbol.length() > 1)
			{
				unsigned __int64 futureNumericSymbol = 0;
				const char* futureSymbol = m_symbol.c_str();
				bool future = *futureSymbol == '/';
				if(future)
				{
					futureNumericSymbol = TU_GetCurrentFutureSymbol(futureSymbol + 1);
					futureSymbol = (const char*)&futureNumericSymbol;
				}
				else
				{
					future = TU_IsFutureSymbol(futureSymbol) != 0;
				}
				if(future)
				{
					U_CopyAndPad(m_refreshFuture.m_symbol, sizeof(m_refreshFuture.m_symbol), futureSymbol, '\0');
					m_refreshFuture.m_steps = steps;
					m_refreshFuture.m_flags = m_ListBoxSnapshotFlags.GetRefreshFlags();//REFRESH_RefreshFailed;REFRESH_EndOfRefresh;REFRESH_EndOfBlock
				}
			}
#endif
//		m_ComboBoxMarketStatus.AddString((const char*)new NameDescriptionItem("C", "Post Message Hours"));
		char msCode;
		if(!m_marketStatus || m_marketStatus >= m_marketStatusCount)
		{
			msCode = TD_GetMarketStatus();
		}
		else
		{
			const NameDescriptionItem* msItem = (const NameDescriptionItem*)m_ComboBoxMarketStatus.GetItemDataPtr(m_marketStatus);
			if(msItem)
			{
				msCode = *msItem->GetName().c_str();
			}
			else
			{
				msCode = TD_GetMarketStatus();
			}
		}
		switch(m_symbolType)
		{
			case SYMT_SYMBOL_STRING:
//			m_refreshStock.m_requestID = m_source == ST_MARKET_DATA ? 0 : 1;
			m_refreshStock.m_requestID = m_source == ST_MARKET_DATA ? TD_GetStockMarketDataSubscriptionRequestId() : TD_GetStockMarketSorterSubscriptionRequestId();
//			m_refreshStock.m_bookID = 0;
			m_refreshStock.m_flags = m_ListBoxSnapshotFlags.GetRefreshFlags();//REFRESH_RefreshFailed;REFRESH_EndOfRefresh;REFRESH_EndOfBlock
			m_refreshStock.m_length = sizeof(m_refreshStock) + m_attachmentLength;
			m_refreshStock.m_marketStatus = msCode;
			m_refreshStock.m_steps = steps;
			U_CopyAndPad(m_refreshStock.m_symbol, sizeof(m_refreshStock.m_symbol), m_symbol.c_str(), '\0');
			return &m_refreshStock;

			case SYMT_SYMBOL_MAPPING:
//			m_refreshStockSm.m_requestID = m_source == ST_MARKET_DATA ? 0 : 1;
			m_refreshStock.m_requestID = m_source == ST_MARKET_DATA ? TD_GetStockMarketDataSubscriptionRequestId() : TD_GetStockMarketSorterSubscriptionRequestId();
//			m_refreshStockSm.m_bookID = 0;
			m_refreshStockSm.m_flags = m_ListBoxSnapshotFlags.GetRefreshFlags();//REFRESH_RefreshFailed;REFRESH_EndOfRefresh;REFRESH_EndOfBlock
			m_refreshStockSm.m_length = sizeof(m_refreshStockSm) + m_attachmentLength;
			m_refreshStockSm.m_marketStatus = msCode;
			m_refreshStockSm.m_steps = steps;
			U_CopyAndPad(m_refreshStockSm.m_symbol, sizeof(m_refreshStockSm.m_symbol), m_symbol.c_str(), '\0');
			m_refreshStockSm.m_symbolIndex = m_symbolMap;
			return &m_refreshStockSm;
		}
#ifndef TAKION_NO_OPTIONS
		}
#endif
	}
	return nullptr;
}

void SampleDialogSettingSimulateSnapshot::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateOptionSource::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxSimSnapshotMarketStatusId, m_ComboBoxMarketStatus);
	DDX_Control(pDX, listBoxSimSnapshotFlagsId, m_ListBoxSnapshotFlags);
	DDX_Control(pDX, editSimSnapshotDescriptionId, m_EditDescription);
	DDX_Control(pDX, spinSimSnapshotRoundLotId, m_SpinRoundLot);
	DDX_Control(pDX, checkBoxSimSnapshotRoundLotsOnlyId, m_CheckBoxRoundLotsOnly);
	DDX_Control(pDX, comboBoxSimSnapshotMarketCategoryId, m_ComboBoxMarketCategory);
	DDX_Control(pDX, comboBoxSimSnapshotFinancialStatusId, m_ComboBoxFinancialStatus);
	DDX_Control(pDX, comboBoxSimSnapshotTradingStateId, m_ComboBoxTradingState);
	DDX_Control(pDX, editSimSnapshotTradingActionReasonId, m_EditTradingActionReason);
	DDX_Control(pDX, comboBoxSimSnapshotPrimaryExchangeId, m_ComboBoxPrimaryExchange);
/*
	DDX_Control(pDX, comboBoxSimResetMessageId, m_ComboBoxMessageType);
	DDX_Control(pDX, comboBoxSimBookId, m_ComboBoxBook);
*/
//	DDX_Control(pDX, spinSimClosingPriceId, m_SpinPrice);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateSnapshot, BaseDialogSettingSimulateOptionSource)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(comboBoxSimSnapshotMarketStatusId, OnSelchangeMarketStatus)
	ON_EN_CHANGE(editSimSnapshotDescriptionId, OnDescriptionChange)
	ON_BN_CLICKED(checkBoxSimSnapshotRoundLotsOnlyId, OnRoundLotsOnly)
	ON_CBN_SELCHANGE(comboBoxSimSnapshotMarketCategoryId, OnSelchangeMarketCategory)
	ON_CBN_SELCHANGE(comboBoxSimSnapshotPrimaryExchangeId, OnSelchangePrimaryExchange)
	ON_CBN_SELCHANGE(comboBoxSimSnapshotFinancialStatusId, OnSelchangeFinancialStatus)
	ON_CBN_SELCHANGE(comboBoxSimSnapshotTradingStateId, OnSelchangeTradingState)
	ON_EN_CHANGE(editSimSnapshotTradingActionReasonId, OnTradingActionReasonChange)
	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)
//	ON_CBN_SELCHANGE(comboBoxSimResetMessageId, OnSelchangeMessageType)
//	ON_CBN_SELCHANGE(comboBoxSimBookId, OnSelchangeBook)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()


LRESULT SampleDialogSettingSimulateSnapshot::OnListBoxChecked(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case listBoxSimSnapshotFlagsId:
		m_ListBoxSnapshotFlags.InvalidateClient();
//		UpdateOrderStatusMask();
		break;

		default:
//		BaseDialogSettingSimulateOptionSource::OnListBoxChecked(w, l);
		break;

	}
	return 0;
}

void SampleDialogSettingSimulateSnapshot::OnRoundLotsOnly()
{
	const bool roundLotsOnly = m_CheckBoxRoundLotsOnly.GetCheck() == BST_CHECKED;
	if(roundLotsOnly != m_roundLotsOnly)
	{
		m_roundLotsOnly = roundLotsOnly;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateSnapshot::OnSelchangeMarketCategory()
{
	int sel = m_ComboBoxMarketCategory.GetCurSel();
	const unsigned char marketCategory = sel < 0 || (unsigned int)sel > (unsigned int)TU_GetMarketCategoryCount() ? 0 : (unsigned char)sel;
	if(marketCategory != m_marketCategory)
	{
		m_marketCategory = marketCategory;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateSnapshot::OnSelchangePrimaryExchange()
{
	OnSelchangeNameDescription(m_ComboBoxPrimaryExchange, m_primaryExchange, PE_OTHR);
}

void SampleDialogSettingSimulateSnapshot::OnSelchangeFinancialStatus()
{
	OnSelchangeChar(m_ComboBoxFinancialStatus, m_financialStatus);
}

void SampleDialogSettingSimulateSnapshot::OnSelchangeTradingState()
{
	OnSelchangeChar(m_ComboBoxTradingState, m_tradingState);
}

LRESULT SampleDialogSettingSimulateSnapshot::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimSnapshotRoundLotId:
		{
			const unsigned int& roundLot = m_SpinRoundLot.GetValue();
			if(roundLot != m_roundLot)
			{
				m_roundLot = roundLot;
				m_modified = true;
			}
/*
			Price price;
			FillPriceFromSpinControl(price, m_SpinPrice, 10000);
			if(price != m_price)
			{
				m_price = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
*/
		}
		break;

		default:
		BaseDialogSettingSimulateOptionSource::OnSpinChange(w, l);
		break;
	}
	return 0;
}


void SampleDialogSettingSimulateSnapshot::OnDescriptionChange()
{
	bool empty = m_description.empty();
	CString text;
	m_EditDescription.GetWindowText(text);
	if(strcmp(text, m_description.c_str()))
	{
		m_description = text;
		if(m_description.empty() != empty)
		{
			m_EditDescription.Invalidate(FALSE);
	//		EnableSendMessageButton();
		}
		m_modified = true;
	}
}

void SampleDialogSettingSimulateSnapshot::OnTradingActionReasonChange()
{
//	bool empty = m_description.empty();
	CString text;
	m_EditTradingActionReason.GetWindowText(text);
	const unsigned int tradingActionReason = U_RepresentStringAsUnsignedNumber<unsigned int>(text);
	if(tradingActionReason != m_tradingActionReason)
	{
		m_tradingActionReason = tradingActionReason;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateSnapshot::OnSelchangeMarketStatus()
{
	int sel = m_ComboBoxMarketStatus.GetCurSel();
	if(sel < 0)sel = 0;
	else if(sel >= m_marketStatusCount)sel = 0;//MTR_Count - 1;
	const unsigned char marketStatus = (unsigned char)sel;
	if(marketStatus != m_marketStatus)
	{
		m_marketStatus = marketStatus;
/*
		const bool messageImbalance = MTR_IMBALANCE == m_messageType;
		m_ComboBoxSource.SetInvalid(!messageImbalance);
		m_ComboBoxBook.SetInvalid(messageImbalance);
		if(messageImbalance != m_symbolNotUsed)
		{
			m_symbolNotUsed = messageImbalance;
			m_EditSymbol.Invalidate(FALSE);
#ifndef TAKION_NO_OPTIONS
			m_CheckBoxOption.Invalidate(FALSE);
//			OptionModeChanged();
			BaseDialogSettingSimulateOption::OptionModeChanged();
#endif
		}
		else
		{
			EnableSendMessageButton();
		}
*/
		m_modified = true;
	}
}
/*
void SampleDialogSettingSimulateSnapshot::OnSelchangeMessageType()
{
	int sel = m_ComboBoxMessageType.GetCurSel();
	if(sel < 0)sel = 0;//MT_LRP;
	else if(sel >= MTR_Count)sel = MTR_Count - 1;
	const unsigned char messageType = (unsigned char)sel;
	if(messageType != m_messageType)
	{
		m_messageType = messageType;
		const bool messageImbalance = MTR_IMBALANCE == m_messageType;
		m_ComboBoxSource.SetInvalid(!messageImbalance);
		m_ComboBoxBook.SetInvalid(messageImbalance);
		if(messageImbalance != m_symbolNotUsed)
		{
			m_symbolNotUsed = messageImbalance;
			m_EditSymbol.Invalidate(FALSE);
#ifndef TAKION_NO_OPTIONS
			m_CheckBoxOption.Invalidate(FALSE);
//			OptionModeChanged();
			BaseDialogSettingSimulateOption::OptionModeChanged();
#endif
		}
		else
		{
			EnableSendMessageButton();
		}
		m_modified = true;
	}
}
*/
#ifndef TAKION_NO_OPTIONS
bool SampleDialogSettingSimulateSnapshot::isOptionModeUsed() const
{
	return BaseDialogSettingSimulateOption::isOptionModeUsed()
/*
		&& MTR_IMBALANCE != m_messageType
		&& BOOK_LEVEL2 != m_book
*/
//		&& ST_MARKET_DATA == m_source
		;
}
#endif
/*
void SampleDialogSettingSimulateSnapshot::OnSelchangeBook()
{
	int sel = m_ComboBoxBook.GetCurSel();
	if(sel < 0)sel = 0;
//	else if(sel >= MTR_Count)sel = MTR_Count - 1;
	const unsigned char book = (unsigned char)sel;
	if(book != m_book)
	{
		bool level2Changed = (BOOK_LEVEL2 == m_book) != (BOOK_LEVEL2 == book);
		m_book = book;

//		m_bookName = m_ComboBoxBookId.GetSelectedName();
		m_modified = true;
		if(level2Changed)
		{
#ifndef TAKION_NO_OPTIONS
			m_CheckBoxOption.Invalidate(FALSE);
//			OptionModeChanged();
			BaseDialogSettingSimulateOption::OptionModeChanged();
#endif
			Level2Changed();
			EnableSendMessageButton();
		}
	}
}
*/
void SampleDialogSettingSimulateSnapshot::SourceChanged()
{
/*
#ifndef TAKION_NO_OPTIONS
		m_CheckBoxOption.Invalidate(FALSE);
//		BaseDialogSettingSimulateOption::OptionModeChanged();
#endif
*/
//	EnableSendMessageButton();
}

#ifndef TAKION_NO_OPTIONS
bool SampleDialogSettingSimulateSnapshot::OptionModeChanged()
{
	bool ret = BaseDialogSettingSimulateOptionSource::OptionModeChanged();
	m_ComboBoxMarketStatus.SetInvalid(ret);
	m_EditDescription.Invalidate(FALSE);
	return ret;
}
#endif

bool SampleDialogSettingSimulateSnapshot::isSendMessageButtonEnabled() const
{
	return 
//		MTR_IMBALANCE == m_messageType || 
		BaseDialogSettingSimulateOptionSource::isSendMessageButtonEnabled();
/*
		&& (BOOK_LEVEL2 != m_book || m_marketCenter && m_messageId == M_BOOK_MODIFY_QUOTE)
		&& (!isPriceUsed() || !m_price.isZero())
		&& (!isSizeUsed() || m_size != 0 || BOOK_LEVEL2 == m_bookId);
*/
}

HBRUSH SampleDialogSettingSimulateSnapshot::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSimSnapshotDescriptionId:
#ifndef TAKION_NO_OPTIONS
		if(isOptionUsed())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		else
#endif
		if(m_description.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
//			return m_emptySymbolValid ? *m_takionVisual->GetBlueBrush() : *m_takionVisual->GetWarningBrush();
			return *m_visualBase->GetBlueBrush();
		}
		break;

	}
	return BaseDialogSettingSimulateOptionSource::OnCtlColor(pDC, pWnd, nCtlColor);
}
