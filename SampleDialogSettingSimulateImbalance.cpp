#include "stdafx.h"
#include "SampleDialogSettingSimulateImbalance.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateImbalance::SampleDialogSettingSimulateImbalance(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateSource(mainWnd, parentTab, "Imbalance"),
	m_ComboBoxBookId(false),
	m_ComboBoxImbalanceType(false),
	m_ComboBoxImbalanceDirection(false),
	m_SpinImbalance(99999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinMarketImbalance(99999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPairedShares(99999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPriceFar(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPriceNear(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPriceRef(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_ComboBoxPriceVariationIndicator(m_visualBase->GetGrayColor()),
	m_SpinAuctionTime(false, 235959, 0, 0),
	m_bookId(0),//BT_NYSE),
	m_imbalanceType(0),//IT_OPENING),
	m_imbalanceDirection(0),//ID_BUY
	m_regulatory(false),
	m_freezeStatus(false),
	m_imbalance(0),
	m_marketImbalance(0),
	m_pairedShares(0),
	m_priceVariationIndicator('\0'),
	m_auctionTimeSeconds(0)
{
	m_ComboBoxBookId.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxImbalanceType.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxImbalanceDirection.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_ComboBoxPriceVariationIndicator.SetColorInvalid(m_visualBase->GetGrayColor());

	SetBigSizeSpinCells(m_SpinImbalance);
	SetBigSizeSpinCells(m_SpinMarketImbalance);
	SetBigSizeSpinCells(m_SpinPairedShares);
	SetPriceSpinCells(m_SpinPriceFar);
	SetPriceSpinCells(m_SpinPriceNear);
	SetPriceSpinCells(m_SpinPriceRef);

	m_SpinAuctionTime.AddSpinCell(1, false);
	m_SpinAuctionTime.AddSpinCell(100, false);
	m_SpinAuctionTime.AddSpinCell(10000, true);

	m_bottomControl += vgap;
	int sideWidth = 32;
	const int priceWidth = 75;

	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	const int w = controlWidth;
	const int cbHeight = 70;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, cbHeight, comboBoxImbalanceBookId);
	AddStaticIndirect("Book", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticImbalanceBookId);

	m_bottomControl += comboBoxHeight + vgap;

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, cbHeight, comboBoxImbalanceTypeId);
	AddStaticIndirect("Type", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticImbalanceTypeId);

	m_bottomControl += comboBoxHeight + vgap;

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, cbHeight, comboBoxImbalanceDirectionId);
	AddStaticIndirect("Direction", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticImbalanceDirectionId);

	m_bottomControl += comboBoxHeight + vgap;

	const int half = m_firstColumnWidth / 2;
	AddButtonIndirect("Regulatory", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, checkBoxHeight, checkBoxSimRegulatoryId);

	m_bottomControl += checkBoxHeight + vgap;
	AddButtonIndirect("Freeze", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, checkBoxHeight, checkBoxSimFreezeStatusId);

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	m_bottomControl += checkBoxHeight + vgap;

	controlWidth = m_xc + priceWidth + hgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimImbalanceId);
	AddStaticIndirect("Imbalance", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimImbalanceId);

	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimMarketImbalanceId);
	AddStaticIndirect("MarketImb", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimMarketImbalanceId);

	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimPairedSharesId);
	AddStaticIndirect("PairedSh", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimPairedSharesId);

	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimImbalancePriceFarId);
	AddStaticIndirect("FarPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimImbalancePriceFarId);

	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimImbalancePriceNearId);
	AddStaticIndirect("NearPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimImbalancePriceNearId);

	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimImbalancePriceRefId);
	AddStaticIndirect("RefPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimImbalancePriceRefId);

	m_bottomControl += spinHeight + vgap;

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL|WS_GROUP, 0, m_xc, m_bottomControl, half, 400, comboBoxPriceVariationIndicatorId);
	AddStaticIndirect("PVI", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + half + hgap, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticPriceVariationIndicatorId);

	m_bottomControl += comboBoxHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinAuctionTimeId);
	AddStaticIndirect("AuctionTime", WS_VISIBLE | SS_NOPREFIX, 0, w, m_bottomControl + (spinHeight - staticHeight) / 2, controlWidth + sideWidth - w, staticHeight, staticAuctionTimeId);

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}
/*
void SampleDialogSettingSimulateImbalance::SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}

void SampleDialogSettingSimulateImbalance::SetSignedSizeSpinCells(SignedNumberTypeSpin<int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}

void SampleDialogSettingSimulateImbalance::SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}
*/
BOOL SampleDialogSettingSimulateImbalance::OnInitDialog()
{
	m_bookId = 0;//BT_NYSE;
	m_imbalanceType = 0;//IT_OPENING;
	m_imbalanceDirection = 0;//ID_BUY;
	m_regulatory = false;
	m_freezeStatus = false;
	m_imbalance = 0;
	m_marketImbalance = 0;
	m_pairedShares = 0;
	m_priceVariationIndicator = '\0';
	m_auctionTimeSeconds = 0;

	m_priceFar.SetZero();
	m_priceNear.SetZero();
	m_priceRef.SetZero();

	BOOL ret = BaseDialogSettingSimulateSource::OnInitDialog();
	
//	m_SpinPriceLow.SetBkColorEnabled(m_takionVisual->GetWarningColor());
//	m_SpinPriceHigh.SetBkColorEnabled(m_takionVisual->GetWarningColor());

	m_ComboBoxBookId.AddString("NYSE");
	m_ComboBoxBookId.AddString("AMEX");
	m_ComboBoxBookId.AddString("ARCA");
	m_ComboBoxBookId.AddString("NSDQ");
	m_ComboBoxBookId.AddString("BATS");
	m_ComboBoxBookId.AddString("BATY");

	m_ComboBoxBookId.SetCurSel(0);
	OnSelchangeBookId();

	m_ComboBoxImbalanceType.AddString("Opening");
	m_ComboBoxImbalanceType.AddString("Closing");
	m_ComboBoxImbalanceType.AddString("HaltOrIpo");
	m_ComboBoxImbalanceType.AddString("Market");
	m_ComboBoxImbalanceType.AddString("Undefined");

	m_ComboBoxImbalanceType.SetCurSel(0);
	OnSelchangeImbalanceType();

	m_ComboBoxImbalanceDirection.AddString("Buy");
	m_ComboBoxImbalanceDirection.AddString("Sell");
	m_ComboBoxImbalanceDirection.AddString("None");
	m_ComboBoxImbalanceDirection.AddString("Undefined");

	m_ComboBoxImbalanceDirection.SetCurSel(0);
	OnSelchangeImbalanceDirection();

	m_ComboBoxPriceVariationIndicator.AddString(nullptr);
	m_ComboBoxPriceVariationIndicator.FillCharRange('A', 'Z', false);
	m_ComboBoxPriceVariationIndicator.FillCharRange('0', '9', false);
	m_ComboBoxPriceVariationIndicator.SetCurSel(0);
	m_ComboBoxPriceVariationIndicator.SetInvalid(true);
	OnSelchangePriceVariationIndicator();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateImbalance::UpdateSettings()
{
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simImbalanceSymbolType.SetValue(symbolType);

	extensionConfig->m_simImbalanceSymbol.SetValue(m_symbol);
	extensionConfig->m_simImbalanceSymbolMap.SetValue(m_symbolMap);

//	unsigned char source = m_source;
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(m_source);
	extensionConfig->m_simImbalanceSource.SetValue(source);

	unsigned char bookId = m_bookId;
	if(bookId >= BT_Count)bookId = BT_Count - 1;
	extensionConfig->m_simImbalanceBookId.SetValue(bookId);

	unsigned char imbalanceType = m_imbalanceType;
	if(imbalanceType >= IT_Count)imbalanceType = IT_Count - 1;
	extensionConfig->m_simImbalanceType.SetValue(imbalanceType);

	unsigned char imbalanceDirection = m_imbalanceDirection;
	if(imbalanceDirection >= ID_Count)imbalanceDirection = ID_Count - 1;
	extensionConfig->m_simImbalanceDirection.SetValue(imbalanceDirection);

	extensionConfig->m_simImbalanceRegulatory.SetValue(m_regulatory);
	extensionConfig->m_simImbalanceFreezeStatus.SetValue(m_freezeStatus);

	char priceVariationIndicator = m_priceVariationIndicator;
	if(priceVariationIndicator < '0')priceVariationIndicator = '0';
	else if(priceVariationIndicator > '9' && priceVariationIndicator < 'A')priceVariationIndicator = 'A';
	else if(priceVariationIndicator > 'Z')priceVariationIndicator = 'Z';
	extensionConfig->m_simImbalancePv.SetValue(priceVariationIndicator);

	extensionConfig->m_simImbalance.SetValue(m_imbalance);
	extensionConfig->m_simMarketImbalance.SetValue(m_marketImbalance);
	extensionConfig->m_simPairedShares.SetValue(m_pairedShares);

	extensionConfig->m_simImbalancePriceFar.SetValue(m_priceFar);
	extensionConfig->m_simImbalancePriceNear.SetValue(m_priceNear);
	extensionConfig->m_simImbalancePriceRef.SetValue(m_priceRef);

	extensionConfig->m_simImbalanceAuctionTimeSeconds.SetValue(m_auctionTimeSeconds);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateImbalance::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simImbalanceSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simImbalanceSymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simImbalanceSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

//	unsigned char source = extensionConfig->m_simImbalanceSource.GetValue();
//	if(source >= ST_Count)source = ST_Count - 1;
	const unsigned char source = GetValidSource(extensionConfig->m_simImbalanceSource.GetValue());
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}

	unsigned char bookId = extensionConfig->m_simImbalanceBookId.GetValue();
	if(bookId >= BT_Count)bookId = BT_Count - 1;
	if(bookId != m_bookId)
	{
//		m_bookId = bookId;
		m_ComboBoxBookId.SetCurSel(bookId);
		OnSelchangeBookId();
	}

	unsigned char imbalanceType = extensionConfig->m_simImbalanceType.GetValue();
	if(imbalanceType >= IT_Count)imbalanceType = IT_Count - 1;
	if(imbalanceType != m_imbalanceType)
	{
//		m_imbalanceType = imbalanceType;
		m_ComboBoxImbalanceType.SetCurSel(imbalanceType);
		OnSelchangeImbalanceType();
	}

	unsigned char imbalanceDirection = extensionConfig->m_simImbalanceType.GetValue();
	if(imbalanceDirection >= ID_Count)imbalanceDirection = ID_Count - 1;
	if(imbalanceDirection != m_imbalanceDirection)
	{
		m_ComboBoxImbalanceDirection.SetCurSel(imbalanceDirection);
		OnSelchangeImbalanceDirection();
	}

	const bool& regulatory = extensionConfig->m_simImbalanceRegulatory.GetValue();
	if(regulatory != m_regulatory)
	{
		m_regulatory = regulatory;
		m_CheckBoxRegulatory.SetCheck(m_regulatory ? BST_CHECKED : BST_UNCHECKED);
	}

	const bool& freezeStatus = extensionConfig->m_simImbalanceFreezeStatus.GetValue();
	if(freezeStatus != m_freezeStatus)
	{
		m_freezeStatus = freezeStatus;
		m_CheckBoxFreezeStatus.SetCheck(m_freezeStatus ? BST_CHECKED : BST_UNCHECKED);
	}

	int found = m_ComboBoxPriceVariationIndicator.FindItem((const void*)extensionConfig->m_simImbalancePv.GetValue());
	if(found != m_ComboBoxPriceVariationIndicator.GetCurSel())
	{
		m_ComboBoxPriceVariationIndicator.SetCurSel(found >= 0 ? found : 0);
		OnSelchangePriceVariationIndicator();
	}

	const unsigned int& imbalance = extensionConfig->m_simImbalance.GetValue();
	if(imbalance != m_imbalance)
	{
		m_imbalance = imbalance;
		m_SpinImbalance.SetValue(m_imbalance, false, true, false);
	}

	const unsigned int& marketImbalance = extensionConfig->m_simMarketImbalance.GetValue();
	if(marketImbalance != m_marketImbalance)
	{
		m_marketImbalance = marketImbalance;
		m_SpinMarketImbalance.SetValue(m_marketImbalance, false, true, false);
	}

	const unsigned int& pairedShares = extensionConfig->m_simPairedShares.GetValue();
	if(pairedShares != m_pairedShares)
	{
		m_pairedShares = pairedShares;
		m_SpinPairedShares.SetValue(m_pairedShares, false, true, false);
	}

	const Price& priceFar = extensionConfig->m_simImbalancePriceFar.GetValue();
	if(priceFar != m_priceFar)
	{
		m_priceFar = priceFar;
		FillSpinControlFromPrice(m_priceFar, m_SpinPriceFar, 10000, false);
	}

	const Price& priceNear = extensionConfig->m_simImbalancePriceNear.GetValue();
	if(priceNear != m_priceNear)
	{
		m_priceNear = priceNear;
		FillSpinControlFromPrice(m_priceNear, m_SpinPriceNear, 10000, false);
	}

	const Price& priceRef = extensionConfig->m_simImbalancePriceRef.GetValue();
	if(priceRef != m_priceRef)
	{
		m_priceRef = priceRef;
		FillSpinControlFromPrice(m_priceRef, m_SpinPriceRef, 10000, false);
	}

	const unsigned int& auctionTimeSeconds = extensionConfig->m_simImbalanceAuctionTimeSeconds.GetValue();
	if(auctionTimeSeconds != m_auctionTimeSeconds)
	{
		m_auctionTimeSeconds = auctionTimeSeconds;
		m_SpinAuctionTime.SetSecond(m_auctionTimeSeconds, false);
	}

	EnableApplyButton(false);
	EnableSendMessageButton();

	m_modified = false;
}

const Message* SampleDialogSettingSimulateImbalance::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
		static const unsigned int maxDollars = (1 << 18) - 1;
		switch(m_source)
		{
			case ST_MARKET_DATA:
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{
				U_CopyAndPad(m_msgMd.m_symbol, sizeof(m_msgMd.m_symbol), m_symbol.c_str(), '\0');
				m_msgMd.m_time = TL_GetCurrentMillisecond();
				m_msgMd.m_farPriceDollar = m_priceFar.GetDollars();
				m_msgMd.m_farPriceFraction = m_priceFar.GetDollarFraction();
				m_msgMd.m_nearPriceDollar = m_priceNear.GetDollars();
				m_msgMd.m_nearPriceFraction = m_priceNear.GetDollarFraction();
				m_msgMd.m_currentReferencePriceDollar = m_priceRef.GetDollars();
				m_msgMd.m_currentReferencePriceFraction = m_priceRef.GetDollarFraction();
				m_msgMd.m_pairedShares = m_pairedShares;
				m_msgMd.m_imbalanceShares = m_imbalance;// < 0 ? -m_imbalance : m_imbalance;
				m_msgMd.m_marketImbalanceShares = m_marketImbalance;
				m_msgMd.m_auctionTime = m_auctionTimeSeconds * 1000;
				m_msgMd.m_imbalanceDirection = GetImbalanceDirection();
				m_msgMd.m_crossType = GetImbalanceType();
				m_msgMd.m_priceVariationIndicator = m_priceVariationIndicator;
				m_msgMd.m_bookID = GetImbalanceBookId();
				m_msgMd.m_regulatoryImbalance_StockOpen = m_regulatory ? 1 : 0;
				m_msgMd.m_freezeStatus = m_freezeStatus ? 1 : 0;

				return &m_msgMd;
			}
			else
			{
				m_msgMdSm.m_symbol = m_symbolMap;
				m_msgMdSm.m_time = TL_GetCurrentMillisecond();
				m_msgMdSm.m_farPriceDollar = m_priceFar.GetDollars();
				m_msgMdSm.m_farPriceFraction = m_priceFar.GetDollarFraction();
				m_msgMdSm.m_nearPriceDollar = m_priceNear.GetDollars();
				m_msgMdSm.m_nearPriceFraction = m_priceNear.GetDollarFraction();
				m_msgMdSm.m_currentReferencePriceDollar = m_priceRef.GetDollars();
				m_msgMdSm.m_currentReferencePriceFraction = m_priceRef.GetDollarFraction();
				m_msgMdSm.m_pairedShares = m_pairedShares;
				m_msgMdSm.m_imbalanceShares = m_imbalance;// < 0 ? -m_imbalance : m_imbalance;
				m_msgMdSm.m_marketImbalanceShares = m_marketImbalance;
				m_msgMdSm.m_auctionTime = m_auctionTimeSeconds * 1000;
				m_msgMdSm.m_imbalanceDirection = GetImbalanceDirection();
				m_msgMdSm.m_crossType = GetImbalanceType();
				m_msgMdSm.m_priceVariationIndicator = m_priceVariationIndicator;
				m_msgMdSm.m_bookID = GetImbalanceBookId();
				m_msgMdSm.m_regulatoryImbalance_StockOpen = m_regulatory ? 1 : 0;
				m_msgMdSm.m_freezeStatus = m_freezeStatus ? 1 : 0;

				return &m_msgMdSm;
			}
			break;

			case ST_MARKET_SORTER:
			if(m_priceFar.GetDollars() > maxDollars
				|| m_priceNear.GetDollars() > maxDollars
				|| m_priceRef.GetDollars() > maxDollars)
			{
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					U_CopyAndPad(m_msgLongMs.m_symbol, sizeof(m_msgLongMs.m_symbol), m_symbol.c_str(), '\0');
					m_msgLongMs.m_time = TL_GetCurrentMillisecond();
					m_msgLongMs.m_farPriceDollar = m_priceFar.GetDollars();
					m_msgLongMs.m_farPriceFraction = m_priceFar.GetDollarFraction();
					m_msgLongMs.m_nearPriceDollar = m_priceNear.GetDollars();
					m_msgLongMs.m_nearPriceFraction = m_priceNear.GetDollarFraction();
					m_msgLongMs.m_currentReferencePriceDollar = m_priceRef.GetDollars();
					m_msgLongMs.m_currentReferencePriceFraction = m_priceRef.GetDollarFraction();
					m_msgLongMs.m_pairedShares = m_pairedShares;
					m_msgLongMs.m_imbalanceShares = m_imbalance;// < 0 ? -m_imbalance : m_imbalance;
					m_msgLongMs.m_marketImbalanceShares = m_marketImbalance;
					m_msgLongMs.m_auctionTime = m_auctionTimeSeconds * 1000;
					m_msgLongMs.m_imbalanceDirection = GetImbalanceDirection();
					m_msgLongMs.m_crossType = GetImbalanceType();
					m_msgLongMs.m_priceVariationIndicator = m_priceVariationIndicator;
					m_msgLongMs.m_bookID = GetImbalanceBookId();
					m_msgLongMs.m_regulatoryImbalance_StockOpen = m_regulatory ? 1 : 0;
					m_msgLongMs.m_freezeStatus = m_freezeStatus ? 1 : 0;

					return &m_msgLongMs;
				}
				else
				{
					m_msgLongMsSm.m_symbol = m_symbolMap;
					m_msgLongMsSm.m_time = TL_GetCurrentMillisecond();
					m_msgLongMsSm.m_farPriceDollar = m_priceFar.GetDollars();
					m_msgLongMsSm.m_farPriceFraction = m_priceFar.GetDollarFraction();
					m_msgLongMsSm.m_nearPriceDollar = m_priceNear.GetDollars();
					m_msgLongMsSm.m_nearPriceFraction = m_priceNear.GetDollarFraction();
					m_msgLongMsSm.m_currentReferencePriceDollar = m_priceRef.GetDollars();
					m_msgLongMsSm.m_currentReferencePriceFraction = m_priceRef.GetDollarFraction();
					m_msgLongMsSm.m_pairedShares = m_pairedShares;
					m_msgLongMsSm.m_imbalanceShares = m_imbalance;// < 0 ? -m_imbalance : m_imbalance;
					m_msgLongMsSm.m_marketImbalanceShares = m_marketImbalance;
					m_msgLongMsSm.m_auctionTime = m_auctionTimeSeconds * 1000;
					m_msgLongMsSm.m_imbalanceDirection = GetImbalanceDirection();
					m_msgLongMsSm.m_crossType = GetImbalanceType();
					m_msgLongMsSm.m_priceVariationIndicator = m_priceVariationIndicator;
					m_msgLongMsSm.m_bookID = GetImbalanceBookId();
					m_msgLongMsSm.m_regulatoryImbalance_StockOpen = m_regulatory ? 1 : 0;
					m_msgLongMsSm.m_freezeStatus = m_freezeStatus ? 1 : 0;

					return &m_msgLongMsSm;
				}
			}
			else
			{
				if(m_symbolType == SYMT_SYMBOL_STRING)
				{
					U_CopyAndPad(m_msgShortMs.m_symbol, sizeof(m_msgShortMs.m_symbol), m_symbol.c_str(), '\0');
					m_msgShortMs.m_time = TL_GetCurrentMillisecond();
					m_msgShortMs.m_farPrice = m_priceFar.GetCompactPrice();
					m_msgShortMs.m_nearPrice = m_priceNear.GetCompactPrice();
					m_msgShortMs.m_currentReferencePrice = m_priceRef.GetCompactPrice();
					m_msgShortMs.m_pairedShares = m_pairedShares;
					m_msgShortMs.m_imbalanceShares = m_imbalance;// < 0 ? -m_imbalance : m_imbalance;
					m_msgShortMs.m_marketImbalanceShares = m_marketImbalance;
					m_msgShortMs.m_auctionTime = m_auctionTimeSeconds * 1000;
					m_msgShortMs.m_imbalanceDirection = GetImbalanceDirection();
					m_msgShortMs.m_crossType = GetImbalanceType();
					m_msgShortMs.m_priceVariationIndicator = m_priceVariationIndicator;
					m_msgShortMs.m_bookID = GetImbalanceBookId();
					m_msgShortMs.m_regulatoryImbalance_StockOpen = m_regulatory ? 1 : 0;
					m_msgShortMs.m_freezeStatus = m_freezeStatus ? 1 : 0;

					return &m_msgShortMs;
				}
				else
				{
					m_msgShortMsSm.m_symbol = m_symbolMap;
					m_msgShortMsSm.m_time = TL_GetCurrentMillisecond();
					m_msgShortMsSm.m_farPrice = m_priceFar.GetCompactPrice();
					m_msgShortMsSm.m_nearPrice = m_priceNear.GetCompactPrice();
					m_msgShortMsSm.m_currentReferencePrice = m_priceRef.GetCompactPrice();
					m_msgShortMsSm.m_pairedShares = m_pairedShares;
					m_msgShortMsSm.m_imbalanceShares = m_imbalance;// < 0 ? -m_imbalance : m_imbalance;
					m_msgShortMsSm.m_marketImbalanceShares = m_marketImbalance;
					m_msgShortMsSm.m_auctionTime = m_auctionTimeSeconds * 1000;
					m_msgShortMsSm.m_imbalanceDirection = GetImbalanceDirection();
					m_msgShortMsSm.m_crossType = GetImbalanceType();
					m_msgShortMsSm.m_priceVariationIndicator = m_priceVariationIndicator;
					m_msgShortMsSm.m_bookID = GetImbalanceBookId();
					m_msgShortMsSm.m_regulatoryImbalance_StockOpen = m_regulatory ? 1 : 0;
					m_msgShortMsSm.m_freezeStatus = m_freezeStatus ? 1 : 0;

					return &m_msgShortMsSm;
				}
			}
			break;
		}
	}
	return nullptr;
}

void SampleDialogSettingSimulateImbalance::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSource::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxImbalanceBookId, m_ComboBoxBookId);
	DDX_Control(pDX, comboBoxImbalanceTypeId, m_ComboBoxImbalanceType);
	DDX_Control(pDX, comboBoxImbalanceDirectionId, m_ComboBoxImbalanceDirection);
	DDX_Control(pDX, checkBoxSimRegulatoryId, m_CheckBoxRegulatory);
	DDX_Control(pDX, checkBoxSimFreezeStatusId, m_CheckBoxFreezeStatus);

	DDX_Control(pDX, spinSimImbalanceId, m_SpinImbalance);
	DDX_Control(pDX, spinSimMarketImbalanceId, m_SpinMarketImbalance);
	DDX_Control(pDX, spinSimPairedSharesId, m_SpinPairedShares);

	DDX_Control(pDX, spinSimImbalancePriceFarId, m_SpinPriceFar);
	DDX_Control(pDX, spinSimImbalancePriceNearId, m_SpinPriceNear);
	DDX_Control(pDX, spinSimImbalancePriceRefId, m_SpinPriceRef);
	DDX_Control(pDX, comboBoxPriceVariationIndicatorId, m_ComboBoxPriceVariationIndicator);
	DDX_Control(pDX, spinAuctionTimeId, m_SpinAuctionTime);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateImbalance, BaseDialogSettingSimulateSource)
	ON_CBN_SELCHANGE(comboBoxImbalanceBookId, OnSelchangeBookId)
	ON_CBN_SELCHANGE(comboBoxImbalanceTypeId, OnSelchangeImbalanceType)
	ON_CBN_SELCHANGE(comboBoxImbalanceDirectionId, OnSelchangeImbalanceDirection)
	ON_CBN_SELCHANGE(comboBoxPriceVariationIndicatorId, OnSelchangePriceVariationIndicator)
	ON_BN_CLICKED(checkBoxSimRegulatoryId, OnRegulatory)
	ON_BN_CLICKED(checkBoxSimFreezeStatusId, OnFreezeStatus)
/*
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(checkBoxOptionId, OnOption)
	ON_BN_CLICKED(radioOptionCallId, OnOptionCall)
	ON_BN_CLICKED(radioOptionPutId, OnOptionPut)
	ON_NOTIFY(DTN_DROPDOWN, datetimeOptionExpirationId, OnCalendarDropdown)
	ON_NOTIFY(DTN_CLOSEUP, datetimeOptionExpirationId, OnCalendarCloseup)
	ON_NOTIFY(DTN_DATETIMECHANGE, datetimeOptionExpirationId, OnExpirationDateChange)
*/
	ON_MESSAGE(WM_USER + 15, OnSpinChange)

END_MESSAGE_MAP()

LRESULT SampleDialogSettingSimulateImbalance::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimImbalancePriceFarId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceFar, 10000);
//			bool changed = m_priceFar.isZero() != price.isZero();
			if(price != m_priceFar)
			{
				m_priceFar = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimImbalancePriceNearId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceNear, 10000);
			if(price != m_priceNear)
			{
				m_priceNear = price;
				m_modified = true;
			}
		}
		break;

		case spinSimImbalancePriceRefId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceRef, 10000);
			if(price != m_priceRef)
			{
				m_priceRef = price;
				m_modified = true;
			}
		}
		break;

		case spinSimImbalanceId:
		{
			const unsigned int& imbalance = m_SpinImbalance.GetValue();
			if(imbalance != m_imbalance)
			{
				m_imbalance = imbalance;
				m_modified = true;
			}
		}
		break;

		case spinSimMarketImbalanceId:
		{
			const unsigned int& marketImbalance = m_SpinMarketImbalance.GetValue();
			if(marketImbalance != m_marketImbalance)
			{
				m_marketImbalance = marketImbalance;
				m_modified = true;
			}
		}
		break;

		case spinSimPairedSharesId:
		{
			const unsigned int& pairedShares = m_SpinPairedShares.GetValue();
			if(pairedShares != m_pairedShares)
			{
				m_pairedShares = pairedShares;
				m_modified = true;
			}
		}
		break;

		case spinAuctionTimeId:
		{
			const unsigned int& auctionTimeSeconds = m_SpinAuctionTime.GetSecond();
			if(auctionTimeSeconds != m_auctionTimeSeconds)
			{
				m_auctionTimeSeconds = auctionTimeSeconds;
				m_modified = true;
			}
		}
		break;

		default:
		BaseDialogSettingSimulateSource::OnSpinChange(w, l);
		break;
	}
	return 0;
}

void SampleDialogSettingSimulateImbalance::OnSelchangeBookId()
{
	int sel = m_ComboBoxBookId.GetCurSel();
	if(sel < 0)sel = 0;//BT_NYSE;
	else if(sel >= BT_Count)sel = BT_Count - 1;
	const unsigned char bookId = (unsigned char)sel;
	if(bookId != m_bookId)
	{
		m_bookId = bookId;
		m_modified = true;
		m_ComboBoxPriceVariationIndicator.SetInvalid(m_bookId != BT_NSDQ);
		m_SpinPriceNear.SetBkColorEnabled(m_bookId == BT_NSDQ || m_bookId == BT_ARCA ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
		m_SpinMarketImbalance.SetBkColorEnabled(m_bookId == BT_ARCA ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());
	}
}

void SampleDialogSettingSimulateImbalance::OnSelchangeImbalanceType()
{
	int sel = m_ComboBoxImbalanceType.GetCurSel();
	if(sel < 0)sel = 0;//IT_OPENING;
	else if((unsigned char)sel >= IT_Count)sel = IT_Count - 1;
	const unsigned char imbalanceType = (unsigned char)sel;
	if(imbalanceType != m_imbalanceType)
	{
		m_imbalanceType = imbalanceType;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateImbalance::OnSelchangeImbalanceDirection()
{
	int sel = m_ComboBoxImbalanceDirection.GetCurSel();
	if(sel < 0)sel = 0;//ID_BUY;
	else if((unsigned char)sel >= ID_Count)sel = ID_Count - 1;
	const unsigned char imbalanceDirection = (unsigned char)sel;
	if(imbalanceDirection != m_imbalanceDirection)
	{
		m_imbalanceDirection = imbalanceDirection;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateImbalance::OnSelchangePriceVariationIndicator()
{
	int sel = m_ComboBoxPriceVariationIndicator.GetCurSel();
	const char priceVariationIndicator = sel >= 0 ? (char)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		m_ComboBoxPriceVariationIndicator.GetItemDataPtr(sel) : '\0';
	if(priceVariationIndicator != m_priceVariationIndicator)
	{
		m_priceVariationIndicator = priceVariationIndicator;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateImbalance::OnRegulatory()
{
	const bool regulatory = m_CheckBoxRegulatory.GetCheck() == BST_CHECKED;
	if(regulatory != m_regulatory)
	{
		m_regulatory = regulatory;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateImbalance::OnFreezeStatus()
{
	const bool freezeStatus = m_CheckBoxFreezeStatus.GetCheck() == BST_CHECKED;
	if(freezeStatus != m_freezeStatus)
	{
		m_freezeStatus = freezeStatus;
		m_modified = true;
	}
}
