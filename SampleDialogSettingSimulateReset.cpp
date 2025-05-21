#include "stdafx.h"
#include "SampleDialogSettingSimulateReset.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateReset::SampleDialogSettingSimulateReset(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateOptionSource(mainWnd, parentTab, "Reset"),
	m_ComboBoxMessageType(false),
	m_ComboBoxBook("All", false),
	m_messageType(MTR_QUOTE),
	m_book(0),
	m_allBooksOrdinal(0xFF)
//	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4)
{
//	AddPriceSpinCells(m_SpinPrice);
	m_emptySymbolValid = true;

	m_ComboBoxMessageType.SetColorInvalid(m_visualBase->GetGrayColor());
	m_ComboBoxMessageType.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_ComboBoxBook.SetColorInvalid(m_visualBase->GetGrayColor());
	m_ComboBoxBook.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_bottomControl += vgap;
	int sideWidth = 32;
	const int priceWidth = 75;

//	int controlWidth = m_xc + priceWidth;
////
	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 400, comboBoxSimResetMessageId);
	AddStaticIndirect("Message", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimResetMessageId);

////
//	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimClosingPriceId);
//	AddStaticIndirect("ClosingPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimClosingPriceId);

	m_bottomControl += comboBoxHeight + vgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 400, comboBoxSimBookId);
	AddStaticIndirect("Book", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimBookId);

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	m_bottomControl += comboBoxHeight;
}

BOOL SampleDialogSettingSimulateReset::OnInitDialog()
{
	m_messageType = MTR_QUOTE;
	m_book = 0;
	m_allBooksOrdinal = 0xFF;

	BOOL ret = BaseDialogSettingSimulateOptionSource::OnInitDialog();
	
	m_ComboBoxMessageType.AddString("Quote");
	m_ComboBoxMessageType.AddString("Imbalance");

	m_ComboBoxMessageType.SetCurSel(0);
	m_ComboBoxSource.SetInvalid(MTR_QUOTE == m_messageType);
	m_ComboBoxBook.SetInvalid(MTR_QUOTE != m_messageType);
	OnSelchangeMessageType();
/*
	BOOK_MM_NSDQ,
	ECNBOOK_NSDQ = 1,
	ECNBOOK_ARCA,
	ECNBOOK_BATS,
	ECNBOOK_EDGA,
	ECNBOOK_EDGX,
	BOOK_LEVEL2,
	ECNBOOK_NYS,
	ECNBOOK_BATY,
	ECNBOOK_AMEX,
//	ECNBOOK_NSX_UNUSED,

	ECNBOOK_FLOW = 11,

	BOOK_MM_ARCA,
	BOOK_MM_BATS,
	BOOK_MM_BATY,

	ECNBOOK_COUNT
*/
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

	m_ComboBoxBook.AddString(nullptr);
	m_allBooksOrdinal = 0;
	for(i = 0, cursor = ecnVector; i < ECNBOOK_COUNT; ++i, ++cursor)
	{
		if(*cursor)m_ComboBoxBook.AddString((const char*)*cursor);
/*
		else
		{
			if(m_allBooksOrdinal == 0xFF)m_allBooksOrdinal = i;
			m_ComboBoxBook.AddString(nullptr);
		}
*/
	}

	m_ComboBoxBook.SetCurSel(0);
	OnSelchangeBook();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateReset::UpdateSettings()
{
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simResetSymbolType.SetValue(symbolType);

	extensionConfig->m_simResetSymbol.SetValue(m_symbol);
	extensionConfig->m_simResetSymbolMap.SetValue(m_symbolMap);

//	unsigned char source = m_source;
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(m_source);
	extensionConfig->m_simResetSource.SetValue(source);

	unsigned char messageType = m_messageType;
	if(messageType >= MTR_Count)messageType = MTR_Count - 1;
	extensionConfig->m_simResetMessageType.SetValue(messageType);

#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_simResetOption.SetValue(m_optionMode);
	extensionConfig->m_simResetOptionCall.SetValue(m_optionCall);
	extensionConfig->m_simResetOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_simResetOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_simResetOptionExpiration.SetValue(GetExpirationDate());
#endif

//	extensionConfig->m_simClsPrice.SetValue(m_price);
	unsigned char book = m_book;
//	if(messageType >= MTR_Count)messageType = MTR_Count - 1;
	extensionConfig->m_simResetBook.SetValue(book);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateReset::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simResetSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simResetSymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simResetSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

//	unsigned char source = extensionConfig->m_simResetSource.GetValue();
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(extensionConfig->m_simResetSource.GetValue());
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}

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
/*
	const Price& price = extensionConfig->m_simReset.GetValue();
	if(price != m_price)
	{
		m_price = price;
		FillSpinControlFromPrice(m_price, m_SpinPrice, 10000, false);
//		m_SpinPriceLow.SetBkColorEnabled(m_priceLow.isZero() ? GetSysColor(COLOR_WINDOW) : m_takionVisual->GetWarningColor());
	}
*/
#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_simResetOption.GetValue(),
		extensionConfig->m_simResetOptionSuffix.GetValue(),
		extensionConfig->m_simResetOptionCall.GetValue(),
		extensionConfig->m_simResetOptionStrikePrice.GetValue(),
		extensionConfig->m_simResetOptionExpiration.GetValue());
/*
	SetCallPut(extensionConfig->m_simResetOptionCall.GetValue());
	
	int found = m_ComboBoxOptionSuffix.FindItem((const void*)extensionConfig->m_simResetOptionSuffix.GetValue());
	m_ComboBoxOptionSuffix.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeOptionSuffix();

	SetStrikePrice(extensionConfig->m_simResetOptionStrikePrice.GetValue());

	SetExpirationDate(extensionConfig->m_simResetOptionExpiration.GetValue());

	m_CheckBoxOption.SetCheck(extensionConfig->m_simResetOption.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnOption();
*/
#endif

	EnableApplyButton(false);
	EnableSendMessageButton();
	m_modified = false;
}

const Message* SampleDialogSettingSimulateReset::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
#ifndef TAKION_NO_OPTIONS
/*
		if(isOptionUsed())
		{
			OptionKey optionKey(m_strikePrice.GetCompactPrice(),
				m_optionCall,
				m_optionSuffix,
				m_expirationDay,
				m_expirationMonth,
				m_expirationYear);
			m_closeOptionMd.m_closePriceDollar = m_price.GetDollars();
			m_closeOptionMd.m_closePriceFraction = m_price.GetDollarFraction();
			U_CopyAndPad(m_closeOptionMd.m_symbol, sizeof(m_closeOptionMd.m_symbol), m_symbol.c_str(), '\0');
			m_closeOptionMd.m_time = TL_GetCurrentMillisecond();
			m_closeOptionMd.m_optionBlock = OptionBlock(optionKey.CalculateOptionBlock());
			return &m_closeOptionMd;
		}
		else
*/
#endif
		switch(m_messageType)
		{
			case MTR_QUOTE:
			if(SYMT_SYMBOL_STRING == m_symbolType)
			{
				if(m_symbol.empty())
				{
					m_resetBook.m_bookId = m_book == m_allBooksOrdinal ? AllBooks : m_book - 1;
					return &m_resetBook;
				}
				else
				{
					m_resetSymbol.m_bookId = m_book == m_allBooksOrdinal ? AllBooks : m_book - 1;
					U_CopyAndPad(m_resetSymbol.m_symbol, sizeof(m_resetSymbol.m_symbol), m_symbol.c_str(), '\0');
					return &m_resetSymbol;
				}
			}
			else
			{
				if(!m_symbolMap)
				{
					m_resetBookSm.m_bookId = m_book == m_allBooksOrdinal ? AllBooks : m_book - 1;
					return &m_resetBookSm;
				}
				else
				{
					m_resetSymbolSm.m_bookId = m_book == m_allBooksOrdinal ? AllBooks : m_book - 1;
					m_resetSymbolSm.m_symbol = m_symbolMap;
					return &m_resetSymbolSm;
				}
			}
			break;

			case MTR_IMBALANCE:
			switch(m_source)
			{
				case ST_MARKET_DATA:
				if(SYMT_SYMBOL_STRING == m_symbolType)
				{
					return &m_resetImbalancesMd;
				}
				else
				{
					return &m_resetImbalancesMdSm;
				}
				break;

				case ST_MARKET_SORTER:
				if(SYMT_SYMBOL_STRING == m_symbolType)
				{
					return &m_resetImbalancesMs;
				}
				else
				{
					return &m_resetImbalancesMsSm;
				}
				break;
			}
			break;
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulateReset::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateOptionSource::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxSimResetMessageId, m_ComboBoxMessageType);
	DDX_Control(pDX, comboBoxSimBookId, m_ComboBoxBook);
//	DDX_Control(pDX, spinSimClosingPriceId, m_SpinPrice);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateReset, BaseDialogSettingSimulateOptionSource)
	ON_CBN_SELCHANGE(comboBoxSimResetMessageId, OnSelchangeMessageType)
	ON_CBN_SELCHANGE(comboBoxSimBookId, OnSelchangeBook)
//	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()
/*
LRESULT SampleDialogSettingSimulateReset::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimClosingPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPrice, 10000);
			if(price != m_price)
			{
				m_price = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		default:
		BaseDialogSettingSimulateOptionSource::OnSpinChange(w, l);
		break;
	}
	return 0;
}
*/

#ifndef TAKION_NO_OPTIONS
bool SampleDialogSettingSimulateReset::OptionModeChanged()
{
//	const bool ret = BaseDialogSettingSimulateOptionSource::OptionModeChanged();
	const bool ret = BaseDialogSettingSimulateOption::OptionModeChanged();
//	m_ComboBoxSource.SetInvalid(ret);
	return ret;
}
#endif

void SampleDialogSettingSimulateReset::OnSelchangeMessageType()
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
		SetSymbolNotUsed(messageImbalance);
/*
		if(messageImbalance != m_symbolNotUsed)
		{
			m_symbolNotUsed = messageImbalance;

			m_EditSymbol.Invalidate(FALSE);
#ifndef TAKION_NO_OPTIONS
//			m_CheckBoxOption.Invalidate(FALSE);
			BaseDialogSettingSimulateOption::OptionModeChanged();
#endif
		}
		else
		{
			EnableSendMessageButton();
		}
		m_modified = true;
*/
	}
}

#ifndef TAKION_NO_OPTIONS
bool SampleDialogSettingSimulateReset::isOptionModeUsed() const
{
	return BaseDialogSettingSimulateOption::isOptionModeUsed()
		&& MTR_IMBALANCE != m_messageType
		&& BOOK_LEVEL2 != m_book
//		&& ST_MARKET_DATA == m_source
		;
}
#endif

void SampleDialogSettingSimulateReset::OnSelchangeBook()
{
	int sel = m_ComboBoxBook.GetCurSel();
	if(sel < 0)sel = 0;
//	else if(sel >= MTR_Count)sel = MTR_Count - 1;
	const unsigned char book = (unsigned char)sel;
	if(book != m_book)
	{
		const bool level2Changed = (BOOK_LEVEL2 == m_book) != (BOOK_LEVEL2 == book);
		m_book = book;

//		m_bookName = m_ComboBoxBookId.GetSelectedName();
		m_modified = true;
		if(level2Changed)
		{
#ifndef TAKION_NO_OPTIONS
//			m_CheckBoxOption.Invalidate(FALSE);
			BaseDialogSettingSimulateOption::OptionModeChanged();
#endif
			Level2Changed();
			EnableSendMessageButton();
		}
	}
}

void SampleDialogSettingSimulateReset::SourceChanged()
{
/*
#ifndef TAKION_NO_OPTIONS
		m_CheckBoxOption.Invalidate(FALSE);
//		BaseDialogSettingSimulateOption::OptionModeChanged();
#endif
*/
//	EnableSendMessageButton();
}

bool SampleDialogSettingSimulateReset::isSendMessageButtonEnabled() const
{
	return MTR_IMBALANCE == m_messageType || BaseDialogSettingSimulateOptionSource::isSendMessageButtonEnabled();
/*
		&& (BOOK_LEVEL2 != m_book || m_marketCenter && m_messageId == M_BOOK_MODIFY_QUOTE)
		&& (!isPriceUsed() || !m_price.isZero())
		&& (!isSizeUsed() || m_size != 0 || BOOK_LEVEL2 == m_bookId);
*/
}

