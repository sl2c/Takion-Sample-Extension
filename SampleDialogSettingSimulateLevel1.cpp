#include "stdafx.h"
#include "SampleDialogSettingSimulateLevel1.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateLevel1::SampleDialogSettingSimulateLevel1(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateOptionSource(mainWnd, parentTab, "Level1"),
	m_quoteCondition('\0'),
	m_marketListing('\0'),
	m_bidMarketCenter('\0'),
	m_askMarketCenter('\0'),
	m_bidSize(0),
	m_askSize(0),
	m_side(true),

	m_SpinBid(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinAsk(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinBidSize(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinAskSize(100000, 0, 0, 1000, 3, TakionSpin::charComma)
//		DDX_Control(pDX, spinSimLevel1BidId, m_SpinBid);

/*
	m_EditRef("0123456789ABCDEF", false),
	m_size(0),
	m_saleCondition1('\0'),
	m_saleCondition2('\0'),
	m_saleCondition3('\0'),
	m_saleCondition4('\0'),
	m_marketCenter('\0'),
	m_marketListing('\0')
*/
{
#ifndef TAKION_NO_OPTIONS
//	m_messageOptionLevel1.m_symbol[sizeof(m_messageOptionLevel1.m_symbol) - 1] = '\0';
//	m_messageFutureLevel1.m_symbol[sizeof(m_messageOptionLevel1.m_symbol) - 1] = '\0';
#endif	
//	m_messageStockLevel1.m_symbol[sizeof(m_messageStockLevel1.m_symbol) - 1] = '\0';

//	m_messageEquityLevel1Long.m_symbol[sizeof(m_messageEquityLevel1Long.m_symbol) - 1] = '\0';
//	m_messageEquityLevel1Short.m_symbol[sizeof(m_messageEquityLevel1Short.m_symbol) - 1] = '\0';

	m_SpinBid.AddSpinCell(1, false);
	m_SpinBid.AddSpinCell(100, false);
	m_SpinBid.AddSpinCell(1000000, true);
	m_SpinBid.AddSpinCell(10000, true);

	m_SpinAsk.AddSpinCell(1, false);
	m_SpinAsk.AddSpinCell(100, false);
	m_SpinAsk.AddSpinCell(1000000, true);
	m_SpinAsk.AddSpinCell(10000, true);

	m_SpinBidSize.AddSpinCell(1, false);
	m_SpinBidSize.AddSpinCell(100, false);
	m_SpinBidSize.AddSpinCell(1000, true);

	m_SpinAskSize.AddSpinCell(1, false);
	m_SpinAskSize.AddSpinCell(100, false);
	m_SpinAskSize.AddSpinCell(1000, true);

	m_bottomControl += vgap;
	const int priceWidth = 75;
	const int staticWidth = 32;
	const int sizeWidth = 54;
	const int charWidth = 24;
	const int attrWidth = 32;

	int controlWidth = m_xc + priceWidth + hgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimLevel1BidId);
	AddStaticIndirect("Bid", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimLevel1BidId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl += spinHeight + vgap, priceWidth, spinHeight, spinSimLevel1AskId);
	AddStaticIndirect("Ask", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimLevel1AskId);

	controlWidth += staticWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + sizeWidth + hgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl += spinHeight + vgap, sizeWidth, spinHeight, spinSimLevel1BidSizeId);
	AddStaticIndirect("BidSize", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimLevel1BidSizeId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl += spinHeight + vgap, sizeWidth, spinHeight, spinSimLevel1AskSizeId);
	AddStaticIndirect("AskSize", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, staticWidth, staticHeight, staticSimLevel1AskSizeId);

	controlWidth += staticWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	AddButtonIndirect("BidSide", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, m_bottomControl += spinHeight + vgap, staticWidth, checkBoxHeight, checkBoxSideId);

	controlWidth = m_xc + charWidth;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += checkBoxHeight + vgap, charWidth, 250, comboLevel1ConditionId);
	AddStaticIndirect("QuoteCond", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticLevel1ConditionId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, charWidth, 250, comboLevel1BidMarketCenterId);
	AddStaticIndirect("BidCenter", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticLevel1BidMarketCenterId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, charWidth, 250, comboLevel1AskMarketCenterId);
	AddStaticIndirect("AskCenter", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticLevel1AskMarketCenterId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, charWidth, 250, comboLevel1MarketListingId);
	AddStaticIndirect("MarketListing", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sizeWidth, staticHeight, staticLevel1MarketListingId);

	m_bottomControl += comboBoxHeight;

	controlWidth += sizeWidth + hgap;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

}

BOOL SampleDialogSettingSimulateLevel1::OnInitDialog()
{
	m_bid.SetZero();
	m_ask.SetZero();
	m_bidSize = 0;
	m_askSize = 0;
	m_side = true;
	m_quoteCondition = '\0';
	m_marketListing = '\0';
	m_bidMarketCenter = '\0';
	m_askMarketCenter = '\0';
/*
	m_refNumber.clear();
//	m_messageId = 0;
	m_saleCondition1 = '\0';
	m_saleCondition2 = '\0';
	m_saleCondition3 = '\0';
	m_saleCondition4 = '\0';
	m_marketCenter = '\0';
	m_marketListing = '\0';
	m_price.SetZero();
	m_size = 0;
*/
	BOOL ret = BaseDialogSettingSimulateOptionSource::OnInitDialog();
/*
	m_SpinBid.SetBkColorEnabled(m_visualBase->GetWarningColor());
	m_SpinAsk.SetBkColorEnabled(m_visualBase->GetWarningColor());
	m_SpinBidSize.SetBkColorEnabled(m_visualBase->GetWarningColor());
	m_SpinAskSize.SetBkColorEnabled(m_visualBase->GetWarningColor());
*/
	m_CheckBoxSide.SetCheck(m_side ? BST_CHECKED : BST_UNCHECKED);

	FillCharRange(m_ComboBoxQuoteCondition);
	FillCharRange(m_ComboBoxBidMarketCenter);
	FillCharRange(m_ComboBoxAskMarketCenter);
	FillCharRange(m_ComboBoxMarketListing);

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateLevel1::UpdateSettings()
{
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simLevel1SymbolType.SetValue(symbolType);
	extensionConfig->m_simLevel1Symbol.SetValue(m_symbol);
	extensionConfig->m_simLevel1SymbolMap.SetValue(m_symbolMap);

#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_simLevel1Option.SetValue(m_optionMode);
	extensionConfig->m_simLevel1OptionCall.SetValue(m_optionCall);
	extensionConfig->m_simLevel1OptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_simLevel1OptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_simLevel1OptionExpiration.SetValue(GetExpirationDate());
#endif

	extensionConfig->m_simLevel1Bid.SetValue(m_bid);
	extensionConfig->m_simLevel1Ask.SetValue(m_ask);

	extensionConfig->m_simLevel1BidSize.SetValue(m_bidSize);
	extensionConfig->m_simLevel1AskSize.SetValue(m_askSize);

	extensionConfig->m_simLevel1Side.SetValue(m_side);

	extensionConfig->m_simLevel1SaleCondition.SetValue(m_quoteCondition);

	extensionConfig->m_simLevel1BidMarketCenter.SetValue(m_bidMarketCenter);
	extensionConfig->m_simLevel1AskMarketCenter.SetValue(m_askMarketCenter);

	extensionConfig->m_simLevel1MarketListing.SetValue(m_marketListing);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateLevel1::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simLevel1Symbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simLevel1SymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simLevel1SymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

//	unsigned char source = extensionConfig->m_simLevel1Source.GetValue();
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(extensionConfig->m_simLevel1Source.GetValue());
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}

	const Price& bid = extensionConfig->m_simLevel1Bid.GetValue();
	if(bid != m_bid)
	{
		m_bid = bid;
		FillSpinControlFromPrice(m_bid, m_SpinBid, 10000, false);
	}

	const Price& ask = extensionConfig->m_simLevel1Ask.GetValue();
	if(ask != m_ask)
	{
		m_ask = ask;
		FillSpinControlFromPrice(m_ask, m_SpinAsk, 10000, false);
	}

	const unsigned int& bidSize = extensionConfig->m_simLevel1BidSize.GetValue();
	if(bidSize != m_bidSize)
	{
		m_bidSize = bidSize;
		m_SpinBidSize.SetValue(m_bidSize, false, true, false);
	}

	const unsigned int& askSize = extensionConfig->m_simLevel1AskSize.GetValue();
	if(askSize != m_askSize)
	{
		m_askSize = askSize;
		m_SpinAskSize.SetValue(m_askSize, false, true, false);
	}

	const bool side = extensionConfig->m_simLevel1Side.GetValue();
	if(side != m_side)
	{
		m_side = side;
		m_CheckBoxSide.SetCheck(m_side ? BST_CHECKED : BST_UNCHECKED);
	}

	SetCharInComboBox(m_ComboBoxQuoteCondition, extensionConfig->m_simLevel1SaleCondition.GetValue(), m_quoteCondition);
	SetCharInComboBox(m_ComboBoxBidMarketCenter, extensionConfig->m_simLevel1BidMarketCenter.GetValue(), m_bidMarketCenter);
	SetCharInComboBox(m_ComboBoxAskMarketCenter, extensionConfig->m_simLevel1AskMarketCenter.GetValue(), m_askMarketCenter);
	SetCharInComboBox(m_ComboBoxMarketListing, extensionConfig->m_simLevel1MarketListing.GetValue(), m_marketListing);
/*
	m_SpinBid.SetBkColorEnabled(m_bid.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
	m_SpinAsk.SetBkColorEnabled(m_ask.isZero() ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
	m_SpinBidSize.SetBkColorEnabled(m_bidSize ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
	m_SpinAskSize.SetBkColorEnabled(m_askSize ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
*/
#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_simLevel1Option.GetValue(),
		extensionConfig->m_simLevel1OptionSuffix.GetValue(),
		extensionConfig->m_simLevel1OptionCall.GetValue(),
		extensionConfig->m_simLevel1OptionStrikePrice.GetValue(),
		extensionConfig->m_simLevel1OptionExpiration.GetValue());
#endif

	EnableApplyButton(false);
	EnableSendMessageButton();
	m_modified = false;
}

bool SampleDialogSettingSimulateLevel1::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateOptionSource::isSendMessageButtonEnabled()
		&& m_marketListing;
//		&& !m_bid.isZero()
//		&& !m_ask.isZero();
//		&& m_size != 0;
}

const Message* SampleDialogSettingSimulateLevel1::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
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
		if(m_symbolType == SYMT_SYMBOL_STRING)
		{
#ifndef TAKION_NO_OPTIONS
			if(optionUsed)//m_optionMode)
			{
				OptionKey optionKey(m_strikePrice.GetCompactPrice(),
					m_optionCall,
					m_optionSuffix,
					m_expirationDay,
					m_expirationMonth,
					m_expirationYear);

				if(m_optionSuffix)U_CopyPlusOneCharAndPad(m_messageOptionLevel1.m_symbol, sizeof(m_messageOptionLevel1.m_symbol) - 1, symbol, m_optionSuffix, '\0');
				else U_CopyAndPad(m_messageOptionLevel1.m_symbol, sizeof(m_messageOptionLevel1.m_symbol) - 1, symbol, '\0');
				m_messageOptionLevel1.m_bidPriceDollars = m_bid.GetDollars();
				m_messageOptionLevel1.m_bidPriceFraction = m_bid.GetDollarFraction();
				m_messageOptionLevel1.m_askPriceDollars = m_ask.GetDollars();
				m_messageOptionLevel1.m_askPriceFraction = m_ask.GetDollarFraction();

				m_messageOptionLevel1.m_bidSize = m_bidSize;
				m_messageOptionLevel1.m_askSize = m_askSize;
				m_messageOptionLevel1.m_bidMarketCenter = m_bidMarketCenter;
				m_messageOptionLevel1.m_askMarketCenter = m_askMarketCenter;
				m_messageOptionLevel1.m_quoteCondition = m_quoteCondition;

				m_messageOptionLevel1.m_optionBlock = OptionBlock(optionKey.CalculateOptionBlock());
				m_messageOptionLevel1.m_time = TL_GetCurrentMillisecond();
				return &m_messageOptionLevel1;
			}
			else if(future)
			{
				m_messageFutureLevel1.m_bidPriceDollars = m_bid.GetDollars();
				m_messageFutureLevel1.m_bidPriceFraction = m_bid.GetDollarFraction();
				m_messageFutureLevel1.m_askPriceDollars = m_ask.GetDollars();
				m_messageFutureLevel1.m_askPriceFraction = m_ask.GetDollarFraction();
				m_messageFutureLevel1.m_bidSize = m_bidSize;
				m_messageFutureLevel1.m_askSize = m_askSize;
				m_messageFutureLevel1.m_bidMarketCenter = m_bidMarketCenter;
				m_messageFutureLevel1.m_askMarketCenter = m_askMarketCenter;
				m_messageFutureLevel1.m_quoteCondition = m_quoteCondition;

				U_CopyAndPad(m_messageFutureLevel1.m_symbol, sizeof(m_messageFutureLevel1.m_symbol) - 1, symbol, '\0');
				m_messageFutureLevel1.m_time = TL_GetCurrentMillisecond();

				return &m_messageFutureLevel1;
			}
			else
#endif
			switch(m_source)
			{
				case ST_MARKET_DATA:
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					m_messageStockLevel1.m_bidDollars = m_bid.GetDollars();
					m_messageStockLevel1.m_bidFraction = m_bid.GetDollarFraction();
					m_messageStockLevel1.m_askDollars = m_ask.GetDollars();
					m_messageStockLevel1.m_askFraction = m_ask.GetDollarFraction();
					m_messageStockLevel1.m_bidSize = m_bidSize;
					m_messageStockLevel1.m_askSize = m_askSize;
					m_messageStockLevel1.m_primaryListingMarket = m_marketListing;
					m_messageStockLevel1.m_bidMarketCenter = m_bidMarketCenter;
					m_messageStockLevel1.m_askMarketCenter = m_askMarketCenter;
					m_messageStockLevel1.m_quoteCondition = m_quoteCondition;

					U_CopyAndPad(m_messageStockLevel1.m_symbol, sizeof(m_messageStockLevel1.m_symbol) - 1, symbol, '\0');
					m_messageStockLevel1.m_time = TL_GetCurrentMillisecond();
					return &m_messageStockLevel1;
				}
				else
				{
					m_messageStockLevel1Sm.m_bidDollars = m_bid.GetDollars();
					m_messageStockLevel1Sm.m_bidFraction = m_bid.GetDollarFraction();
					m_messageStockLevel1Sm.m_askDollars = m_ask.GetDollars();
					m_messageStockLevel1Sm.m_askFraction = m_ask.GetDollarFraction();
					m_messageStockLevel1Sm.m_bidSize = m_bidSize;
					m_messageStockLevel1Sm.m_askSize = m_askSize;
					m_messageStockLevel1Sm.m_primaryListingMarket = m_marketListing;
					m_messageStockLevel1Sm.m_bidMarketCenter = m_bidMarketCenter;
					m_messageStockLevel1Sm.m_askMarketCenter = m_askMarketCenter;
					m_messageStockLevel1Sm.m_quoteCondition = m_quoteCondition;

					m_messageStockLevel1Sm.m_primaryListingMarket = m_marketListing;
					m_messageStockLevel1Sm.m_symbol = m_symbolMap;
					m_messageStockLevel1Sm.m_time = TL_GetCurrentMillisecond();
					return &m_messageStockLevel1Sm;
				}
				break;

				case ST_MARKET_SORTER:
				if(m_bidSize > 0xFFFF || m_askSize > 0xFFFF || m_bid.GetDollars() >= 1 << 18 || m_ask.GetDollars() >= 1 << 18)//big
				{
					if(m_symbolType == SYMT_SYMBOL_STRING)
					{
						if(m_side)
						{
							m_messageEquityLevel1Long.m_priceDollars = m_bid.GetDollars();
							m_messageEquityLevel1Long.m_priceFraction = m_bid.GetDollarFraction();
							m_messageEquityLevel1Long.m_side = 'B';
						}
						else
						{
							m_messageEquityLevel1Long.m_priceDollars = m_ask.GetDollars();
							m_messageEquityLevel1Long.m_priceFraction = m_ask.GetDollarFraction();
							m_messageEquityLevel1Long.m_side = 'S';
						}
						m_messageEquityLevel1Long.m_quoteCondition = m_quoteCondition;
						U_CopyAndPad(m_messageEquityLevel1Long.m_symbol, sizeof(m_messageEquityLevel1Long.m_symbol) - 1, symbol, '\0');
						return &m_messageEquityLevel1Long;
					}
					else
					{
						if(m_side)
						{
							m_messageEquityLevel1LongSm.m_priceDollars = m_bid.GetDollars();
							m_messageEquityLevel1LongSm.m_priceFraction = m_bid.GetDollarFraction();
							m_messageEquityLevel1LongSm.m_side = 'B';
						}
						else
						{
							m_messageEquityLevel1LongSm.m_priceDollars = m_ask.GetDollars();
							m_messageEquityLevel1LongSm.m_priceFraction = m_ask.GetDollarFraction();
							m_messageEquityLevel1LongSm.m_side = 'S';
						}
						m_messageEquityLevel1LongSm.m_quoteCondition = m_quoteCondition;
						m_messageEquityLevel1LongSm.m_symbol = m_symbolMap;
						return &m_messageEquityLevel1LongSm;
					}
				}
				else//small
				{
					if(m_symbolType == SYMT_SYMBOL_STRING)
					{
						if(m_side)
						{
							m_messageEquityLevel1Short.m_price = m_bid.GetCompactPrice();
							m_messageEquityLevel1Short.m_side = 'B';
						}
						else
						{
							m_messageEquityLevel1Short.m_price = m_ask.GetCompactPrice();
							m_messageEquityLevel1Short.m_side = 'S';
						}
						m_messageEquityLevel1Short.m_quoteCondition = m_quoteCondition;
						U_CopyAndPad(m_messageEquityLevel1Short.m_symbol, sizeof(m_messageEquityLevel1Short.m_symbol) - 1, symbol, '\0');
						return &m_messageEquityLevel1Short;
					}
					else
					{
//						m_messageEquityLevel1ShortSm.m_price = m_price.GetCompactPrice();

						if(m_side)
						{
							m_messageEquityLevel1ShortSm.m_price = m_bid.GetCompactPrice();
							m_messageEquityLevel1ShortSm.m_side = 'B';
						}
						else
						{
							m_messageEquityLevel1ShortSm.m_price = m_ask.GetCompactPrice();
							m_messageEquityLevel1ShortSm.m_side = 'S';
						}
						m_messageEquityLevel1ShortSm.m_quoteCondition = m_quoteCondition;
						m_messageEquityLevel1ShortSm.m_symbol = m_symbolMap;
						return &m_messageEquityLevel1ShortSm;
					}
				}
				break;
			}
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulateLevel1::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateOptionSource::DoDataExchange(pDX);
	DDX_Control(pDX, spinSimLevel1BidId, m_SpinBid);
	DDX_Control(pDX, spinSimLevel1AskId, m_SpinAsk);
	DDX_Control(pDX, spinSimLevel1BidSizeId, m_SpinBidSize);
	DDX_Control(pDX, spinSimLevel1AskSizeId, m_SpinAskSize);

	DDX_Control(pDX, checkBoxSideId, m_CheckBoxSide);

	DDX_Control(pDX, comboLevel1ConditionId, m_ComboBoxQuoteCondition);
	DDX_Control(pDX, comboLevel1BidMarketCenterId, m_ComboBoxBidMarketCenter);
	DDX_Control(pDX, comboLevel1AskMarketCenterId, m_ComboBoxAskMarketCenter);
	DDX_Control(pDX, comboLevel1MarketListingId, m_ComboBoxMarketListing);
/*
	DDX_Control(pDX, editSimPrintRefId, m_EditRef);
	DDX_Control(pDX, spinSimPrintSizeId, m_SpinSize);
	DDX_Control(pDX, comboPrintCondition1Id, m_ComboBoxSaleCondition1);
	DDX_Control(pDX, comboPrintCondition2Id, m_ComboBoxSaleCondition2);
	DDX_Control(pDX, comboPrintCondition3Id, m_ComboBoxSaleCondition3);
	DDX_Control(pDX, comboPrintCondition4Id, m_ComboBoxSaleCondition4);
	DDX_Control(pDX, comboPrintMarketCenterId, m_ComboBoxMarketCenter);
	DDX_Control(pDX, comboPrintMarketListingId, m_ComboBoxMarketListing);
*/
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateLevel1, BaseDialogSettingSimulateOptionSource)
//	ON_WM_CTLCOLOR()
	
	ON_BN_CLICKED(checkBoxSideId, OnSide)
	ON_CBN_SELCHANGE(comboLevel1ConditionId, OnSelchangeQuoteCondition)
	ON_CBN_SELCHANGE(comboLevel1BidMarketCenterId, OnSelchangeBidMarketCenter)
	ON_CBN_SELCHANGE(comboLevel1AskMarketCenterId, OnSelchangeAskMarketCenter)
	ON_CBN_SELCHANGE(comboLevel1MarketListingId, OnSelchangeMarketListing)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateLevel1::OnSide()
{
	const bool side = m_CheckBoxSide.GetCheck() == BST_CHECKED;
	if(side != m_side)
	{
		m_side = side;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateLevel1::OnSelchangeQuoteCondition()
{
	OnSelchangeUChar(m_ComboBoxQuoteCondition, m_quoteCondition);
}

void SampleDialogSettingSimulateLevel1::OnSelchangeBidMarketCenter()
{
	if(OnSelchangeUChar(m_ComboBoxBidMarketCenter, m_bidMarketCenter))
	{
		EnableSendMessageButton();
	}
}

void SampleDialogSettingSimulateLevel1::OnSelchangeAskMarketCenter()
{
	if(OnSelchangeUChar(m_ComboBoxAskMarketCenter, m_askMarketCenter))
	{
		EnableSendMessageButton();
	}
}

void SampleDialogSettingSimulateLevel1::OnSelchangeMarketListing()
{
	if(OnSelchangeUChar(m_ComboBoxMarketListing, m_marketListing))
	{
		EnableSendMessageButton();
	}
}

LRESULT SampleDialogSettingSimulateLevel1::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimLevel1BidId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinBid, 10000);
//			const bool changed = m_bid.isZero() != price.isZero();
//			if(changed)m_SpinBid.SetBkColorEnabled(m_bid.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(price != m_bid)
			{
				m_bid = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimLevel1AskId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinAsk, 10000);
//			const bool changed = m_ask.isZero() != price.isZero();
//			if(changed)m_SpinAsk.SetBkColorEnabled(m_ask.isZero() ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(price != m_ask)
			{
				m_ask = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimLevel1BidSizeId:
		{
			const unsigned int size = m_SpinBidSize.GetValue();
//			const bool changed = !m_bidSize != !size;
//			if(changed)m_SpinBidSize.SetBkColorEnabled(size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(size != m_bidSize)
			{
				m_bidSize = size;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimLevel1AskSizeId:
		{
			const unsigned int size = m_SpinAskSize.GetValue();
//			const bool changed = !m_askSize != !size;
//			if(changed)m_SpinAskSize.SetBkColorEnabled(size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
			if(size != m_askSize)
			{
				m_askSize = size;
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
