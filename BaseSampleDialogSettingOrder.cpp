#include "stdafx.h"
#include "BaseSampleDialogSettingOrder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseSampleDialogSettingOrder::BaseSampleDialogSettingOrder(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseSampleDialogSettingSendOrder(mainWnd, parentTab, label),
//	m_EditSymbol(U_GetValidStockChars(), false),
	m_ComboBoxSide(false),
	m_ComboBoxPriceBase(false),
	m_priceType(0xFF),
	m_sizeType(0xFF),
	m_displaySizeType(0xFF),
	m_SpinPriceOffset(999999, -999999, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinSizeShare(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinDisplaySizeShare(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_ComboBoxTifType(false),
	m_SpinTif(true, false),
	m_EditMnemonic("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", false),
	m_SpinReserveSize(100, 0, 0, 1000, 3, TakionSpin::charComma),
	m_ComboBoxPegType(false),
	m_SpinPegOffset(999999, -999999, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinMinQuantity(99, 0, 0, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Minimum Quantity to fill in Round Lots"),
	m_CheckBoxAllOrNone("Fill all Shares or none", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_currentDestination(nullptr),
	m_currentRouting(nullptr),
	m_allOrNone(false),
	m_pegType(0)
{
	m_ComboBoxDestination.SetItemToolTips(true);
	m_ComboBoxRouting.SetItemToolTips(true);

	m_ComboBoxSide.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxPriceBase.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxTifType.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxDestination.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxRouting.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_SpinPriceOffset.AddSpinCell(1, false);
	m_SpinPriceOffset.AddSpinCell(100, false);
	m_SpinPriceOffset.AddSpinCell(10000, true);

	m_SpinPrice.AddSpinCell(1, false);
	m_SpinPrice.AddSpinCell(100, false);
	m_SpinPrice.AddSpinCell(1000000, true);
	m_SpinPrice.AddSpinCell(10000, true);

	m_SpinSizeShare.AddSpinCell(1, false);
	m_SpinSizeShare.AddSpinCell(100, false);
	m_SpinSizeShare.AddSpinCell(1000, true);

	m_SpinDisplaySizeShare.AddSpinCell(1, false);
	m_SpinDisplaySizeShare.AddSpinCell(100, false);
	m_SpinDisplaySizeShare.AddSpinCell(1000, true);

	m_SpinReserveSize.AddSpinCell(1, false);
	m_SpinReserveSize.AddSpinCell(100, false);

	m_SpinPegOffset.AddSpinCell(1, false);
	m_SpinPegOffset.AddSpinCell(100, false);
	m_SpinPegOffset.AddSpinCell(10000, true);

	m_SpinMinQuantity.AddSpinCell(1, false);
//	m_SpinMinQuantity.AddSpinCell(100, true);

	int x = m_accountWidth + hgap;//0;
//	int y = 0;
	int width = 64;//52;
	int xc = x + hgap;
/*
	AddStaticIndirect("Symbol", WS_VISIBLE | SS_NOPREFIX, 0, xc, y, width, staticHeight, staticSymbolId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xc, y += staticHeight, width, editHeight, editSymbolId);
*/
	m_bottomControl += vgap;

	int y = 0;//m_bottomControl;//staticHeight + editHeight + vgap;

	int sideWidth = width * 2 / 3;
	AddButtonIndirect("Side", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, sideWidth + hgap + hgap, 24, groupSideId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y + groupOffset, sideWidth, 50, comboBoxSideId);

//	AddButtonIndirect("TMtch", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc + sideWidth + hgap + hgap, y + groupOffset, sideWidth - hgap - hgap, checkBoxHeight, checkBoxProAtsId);
	
	y += 24 + vgap;
	const int yPrice = y;
	int priceHeight = groupOffset + checkBoxHeight + vgap + vgap + checkBoxHeight + vgap + vgap + comboBoxHeight + vgap + spinHeight + vgap + checkBoxHeight + vgap + spinHeight + vgap + vgap;
	AddButtonIndirect("Price", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap + comboBoxHeight, priceHeight, groupPriceId);

	AddButtonIndirect("Market Order", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += groupOffset, width, checkBoxHeight, radioPriceMarketId);
	AddButtonIndirect("Offset", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap + vgap, width / 2 + hgap, checkBoxHeight, radioPriceOffsetId);
	int yAbsolute = y + checkBoxHeight + vgap + vgap + comboBoxHeight + vgap + spinHeight + vgap;
	AddButtonIndirect("Absolute", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, yAbsolute, width, checkBoxHeight, radioPriceAbsoluteId);

	AddButtonIndirect("%", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc + width / 2 + hgap + hgap, y, width / 3, checkBoxHeight, checkBoxPricePercentId);
	int xOffset = xc + comboBoxHeight;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xOffset, y += checkBoxHeight + vgap, width, 240, comboBoxPriceBaseId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xOffset, y += comboBoxHeight + vgap, width, spinHeight, spinPriceOffsetId);
//	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, yAbsolute += checkBoxHeight + vgap, width + comboBoxHeight, spinHeight, spinPriceId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x + 1, yAbsolute += checkBoxHeight + vgap, width + comboBoxHeight + hgap - 1, spinHeight, spinPriceId);

	m_bottomControl = yPrice + priceHeight;

	y = m_bottomControl + vgap * 4;

	const int width2 = width / 2;
	const int width3 = width / 3;
//	y = groupOffset;// += groupHeight + vgap + vgap;
	AddButtonIndirect("AON", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 0, xc, y, width2, checkBoxHeight, checkboxAllOrNoneId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, xc, y += checkBoxHeight + vgap, width3, spinHeight, spinMinQuantityId);
	AddStaticIndirect("MinQty", WS_VISIBLE | SS_NOPREFIX | SS_LEFTNOWORDWRAP, 0, xc + width3 + 1, y + (checkBoxHeight - staticHeight) / 2, width2, staticHeight, staticMinQuantityId);

/*
	yPrice += 70 + vgap;
	AddButtonIndirect("Account", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, yPrice, width + hgap + hgap + comboBoxHeight, 24, groupAccountId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, yPrice + groupOffset, width + comboBoxHeight, 400, comboBoxAccountId);
*/
	x += width + hgap + hgap + comboBoxHeight + hgap;
	m_priceHorizontalOffset = x;
	xc = x + hgap;
	y = 0;
	width = 44;

	yAbsolute = y + 42;
	AddButtonIndirect("Size", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap + comboBoxHeight, 40, groupSizeId);
	AddButtonIndirect("Position", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += groupOffset, width, checkBoxHeight, radioSizePositionId);
	AddButtonIndirect("Share", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap + vgap, width, checkBoxHeight, radioSizeShareId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc + comboBoxHeight, y += checkBoxHeight + vgap, width, spinHeight, spinSizeShareId);

	y = yAbsolute;
	m_displaySizeVerticalOffset = y + 40;
	AddButtonIndirect("Display Size", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap + comboBoxHeight, 40, groupDisplaySizeId);
	yAbsolute = y + 42;
	AddButtonIndirect("All", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += groupOffset, width, checkBoxHeight, radioDisplaySizeAllId);
	AddButtonIndirect("Share", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap + vgap, width, checkBoxHeight, radioDisplaySizeShareId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc + comboBoxHeight, y += checkBoxHeight + vgap, width, spinHeight, spinDisplaySizeShareId);

	int destWidth = 36;
	y = yAbsolute + staticHeight;
	int xd = xc;
	AddStaticIndirect("Venue", WS_VISIBLE | SS_NOPREFIX, 0, xd, y, destWidth, staticHeight, staticDestinationId);
	AddStaticIndirect("Routing", WS_VISIBLE | SS_NOPREFIX, 0, xd += destWidth + hgap, y, destWidth, staticHeight, staticRoutingId);
	AddStaticIndirect("Mnemonic", WS_VISIBLE | SS_NOPREFIX, 0, xd += destWidth + hgap, y, destWidth, staticHeight, staticMnemonicId);
	AddStaticIndirect("Reserve", WS_VISIBLE | SS_NOPREFIX, 0, xd += destWidth + hgap, y, destWidth, staticHeight, staticReserveSizeId);
	xd = xc;
	y += staticHeight;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xd, y, destWidth, 400, comboBoxDestinationId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xd += destWidth + hgap, y, destWidth, 400, comboBoxRoutingId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xd += destWidth + hgap, y, destWidth, editHeight, editMnemonicId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xd += destWidth + hgap, y, destWidth, spinHeight, spinReserveSizeId);

	m_destinationHorizontalalOffset = xd + destWidth;
	m_destinationVerticalOffset = y;
/*
	int w = width + comboBoxHeight + comboBoxHeight;
	AddButtonIndirect("User ID", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, yPrice, w + hgap + hgap, 24, groupUserIdId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, yPrice + groupOffset, w, spinHeight, spinUserIdId);

	AddButtonIndirect("Send Order", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x + w + 4 * hgap, yPrice + groupOffset, destWidth + destWidth + hgap, comboBoxHeight, buttonSendOrderId);
*/
	x += width + hgap + hgap + comboBoxHeight + hgap;
	xc = x + hgap;
	m_tifHorizontalOffset = x;
	y = 0;
	width += comboBoxHeight + 4;
	AddButtonIndirect("TIF", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap, 40, groupTifId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y += groupOffset, width, 100, comboBoxTifId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, y += comboBoxHeight + vgap, width, spinHeight, spinTifId);

	m_horizontalOffset = xc + width + hgap;

//	x = m_horizontalOffset;
//	xc = x + hgap;
	y = 42;
	AddButtonIndirect("Peg", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap, 40, groupPegId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y += groupOffset, width, 100, comboBoxPegTypeId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += comboBoxHeight + vgap, width, spinHeight, spinPegPriceOffsetId);
}

unsigned int BaseSampleDialogSettingOrder::GetReserveQuantity() const
{
	return m_SpinReserveSize.GetValue();
}
/*
void BaseSampleDialogSettingOrder::SetRoutingId(unsigned short routingId, unsigned short routingSubType)
{
	if(routingId && m_currentDestination)
	{
		const DestinationRouting* routing = m_currentDestination->FindRouting(routingId, routingSubType);
		if(!routing)
		{
			routing = routingId == RI_PROACTIVE ? m_currentDestination->GetProActiveReplacement() : m_currentDestination->GetNoRoutingReplacement();
		}
		if(routing)
		{
			int found = m_ComboBoxRouting.FindItem(routing);
			if(found < 0)
			{
				routing = routingId == RI_PROACTIVE ? m_currentDestination->GetProActiveReplacement() : m_currentDestination->GetNoRoutingReplacement();
				if(routing)
				{
					found = m_ComboBoxRouting.FindItem(routing);
					if(found < 0)
					{
						found = 0;
					}
				}
				else
				{
					found = 0;
				}
			}
			m_ComboBoxRouting.SetCurSel(found);
		}
		else
		{
			m_ComboBoxRouting.SetCurSel(0);
		}
	}
	else
	{
		m_ComboBoxRouting.SetCurSel(0);
	}
}
*/
/*
void BaseSampleDialogSettingOrder::UpdateDestinationRouting()
{
	int sel = m_ComboBoxDestination.GetCurSel();
	if(sel >= 0)
	{
		unsigned int routingName = 0;
		unsigned int routingId = RI_NONE;
		const Destination* dest = (const Destination*)m_ComboBoxDestination.GetItemDataPtr(sel);
		if(m_currentDestination != dest)
		{
			sel = m_ComboBoxRouting.GetCurSel();
			if(sel >= 0)
			{
				const DestinationRouting* routing = (const DestinationRouting*)m_ComboBoxRouting.GetItemDataPtr(sel);
				if(routing)
				{
					routingName = routing->GetNumericName();
					routingId = routing->GetId();
				}
			}
			m_currentDestination = dest;
			m_EditMnemonic.Invalidate(FALSE);
			m_ComboBoxRouting.ResetContent();
			if(!m_currentDestination->GetNoRoutingReplacement())
			{
				m_ComboBoxRouting.AddString(nullptr);
			}
			const Destination::RoutingMap& routingMap = m_currentDestination->GetRoutingMap();
			for(Destination::RoutingMap::const_iterator it = routingMap.begin(), itend = routingMap.end(); it != itend; ++it)
			{
				m_ComboBoxRouting.AddString((const char*)it->second);
			}
			if(routingName)
			{
				char buf[sizeof(unsigned int) + 1];
				*(unsigned int*)buf = routingName;
				buf[sizeof(unsigned int)] = '\0';
				sel = m_ComboBoxRouting.FindItemByName(buf);
				if(sel < 0)
				{
					const DestinationRouting* routing = routingId == RI_PROACTIVE ? m_currentDestination->GetProActiveReplacement() : m_currentDestination->GetNoRoutingReplacement();
					if(routing)
					{
						sel = m_ComboBoxRouting.FindItem(routing);
						if(sel < 0)
						{
							sel = 0;
						}
					}
					else
					{
						sel = 0;
					}
				}
				m_ComboBoxRouting.SetCurSel(sel);
			}
			else
			{
				const DestinationRouting* routing = m_currentDestination->GetNoRoutingReplacement();
				if(routing)
				{
					sel = m_ComboBoxRouting.FindItem(routing);
					if(sel < 0)
					{
						sel = 0;
					}
				}
				else
				{
					sel = 0;
				}
				m_ComboBoxRouting.SetCurSel(sel);
			}
			OnSelchangeRouting();
		}
	}
	else if(m_currentDestination)
	{
		m_currentDestination = nullptr;
		m_EditMnemonic.Invalidate(FALSE);
		m_ComboBoxRouting.ResetContent();
		m_ComboBoxRouting.AddString(nullptr);
		m_ComboBoxRouting.SetCurSel(0);
		OnSelchangeRouting();
	}
}
*/

void BaseSampleDialogSettingOrder::UpdateAllOrNone()
{
	m_SpinMinQuantity.SetBkColorEnabled(m_allOrNone ? m_visualBase->GetGrayColor() : 0xFFFFFFFF);
}

unsigned char BaseSampleDialogSettingOrder::FillOrderParams(BaseOrderParams& baseOrderParams) const
{
	unsigned char ret = BaseSampleDialogSettingSendOrder::FillOrderParams(baseOrderParams);

	if(ret == SPE_OK)
	{
		OrderParams& orderParams = (OrderParams&)baseOrderParams;

		int sel = m_ComboBoxSide.GetCurSel();
		if(sel < 0 || sel >= OCS_COUNT)return SPE_NO_SIDE;
		orderParams.SetSide((unsigned char)sel);

		if(m_priceType >= PT_Count)return SPE_NO_PRICE_TYPE;
		orderParams.SetPriceType(m_priceType);

		orderParams.SetPriceOffsetPercent(m_CheckBoxPricePercent.GetCheck() == BST_CHECKED);

//		orderParams.SetProAts(m_CheckBoxProAts.GetCheck() == BST_CHECKED);

		sel = m_ComboBoxPriceBase.GetCurSel();
		if(sel < 0 || sel >= PB_Count)
		{
			if(orderParams.GetPriceType() == PT_OFFSET)
			{
				return SPE_NO_PRICE_BASE;
			}
			orderParams.SetPriceBase(PB_SAME_SIDE);
		}
		else
		{
			orderParams.SetPriceBase((unsigned char)sel);
		}

		if(m_sizeType >= SIT_Count)return SPE_NO_SIZE_TYPE;
		orderParams.SetSizeType(m_sizeType);

		if(m_displaySizeType >= DST_Count)return SPE_NO_DISPLAY_SIZE_TYPE;
		orderParams.SetDisplaySizeType(m_displaySizeType);

		unsigned char tifType = GetTifType();
		if(tifType == 0xFF)return SPE_NO_TIF_TYPE;
		orderParams.SetTifType(tifType);

		SignedPrice signedPrice;
		FillSignedPriceFromSpinControl(signedPrice, m_SpinPriceOffset, 10000);
		orderParams.SetPriceOffset(signedPrice);

		Price price;
		FillPriceFromSpinControl(price, m_SpinPrice, 10000);
		orderParams.SetPriceAbsolute(price);

		orderParams.SetSizeShares(m_SpinSizeShare.GetValue());

		orderParams.SetDisplaySizeShares(m_SpinDisplaySizeShare.GetValue());

		orderParams.SetTifMilliseconds(GetTifMilliseconds());

		sel = m_ComboBoxDestination.GetCurSel();
		if(sel < 0)return SPE_NO_DESTINATION;
		const Destination* destination = (const Destination*)m_ComboBoxDestination.GetItemDataPtr(sel);
		orderParams.SetDestinationId(destination->GetId());
		orderParams.SetDestinationName(destination->GetMainNumericName());

		unsigned int minReserve = 0;
		unsigned int maxReserve = 0;
		sel = m_ComboBoxRouting.GetCurSel();
		if(sel < 0)
		{
			orderParams.SetRoutingId(RI_NONE);
			orderParams.SetRoutingSubType(0);
			orderParams.SetRoutingName(0);
		}
		else
		{
			const DestinationRouting* routing = (const DestinationRouting*)m_ComboBoxRouting.GetItemDataPtr(sel);
			if(routing)
			{
				minReserve = routing->GetMinReserveQuantity();
				maxReserve = routing->GetMaxReserveQuantity();
				orderParams.SetRoutingId(routing->GetId());
				orderParams.SetRoutingSubType(routing->GetSubType());
				orderParams.SetRoutingName(routing->GetNumericName());
			}
			else
			{
				orderParams.SetRoutingId(RI_NONE);
				orderParams.SetRoutingSubType(0);
				orderParams.SetRoutingName(0);
			}
		}

		if(destination->HasMnemonic())
		{
			CString mnemonic;
			m_EditMnemonic.GetWindowText(mnemonic);
			orderParams.SetMnemonic(U_RepresentStringAsUnsignedNumber<unsigned int>(mnemonic));
		}
		else
		{
			orderParams.SetMnemonic(0);
		}

		if(minReserve < maxReserve)
		{
			unsigned int reserveSize = m_SpinReserveSize.GetValue();
			if(reserveSize < minReserve)
			{
				reserveSize = minReserve;
			}
			else if(reserveSize > maxReserve)
			{
				reserveSize = maxReserve;
			}
			orderParams.SetReserveSize(reserveSize);
		}
		else
		{
			orderParams.SetReserveSize(0);
		}

//		sel = m_ComboBoxPegType.GetCurSel();
//		if(sel < 0 || sel >= PEG_TYPE_COUNT)orderParams.SetPegType(PEG_NONE);
//		else orderParams.SetPegType((unsigned char)sel);
		 
		orderParams.SetPegType(m_pegType >= PEG_TYPE_COUNT ? PEG_NONE : m_pegType);

		FillSignedPriceFromSpinControl(signedPrice, m_SpinPegOffset, 10000);
		orderParams.SetPegOffset(signedPrice);
	}
	return ret;
}
/*
void BaseSampleDialogSettingOrder::FillTifComboBox(CComboBox& comboBox)
{
//TifType enum in TakionUtilsApi.h
//	for(unsigned char tif = TIF_DAY; tif < TIF_LAST; ++tif)
	for(unsigned char tif = TIF_DAY; tif <= TIF_GTT; ++tif)
	{
		switch(tif)
		{
			case TIF_DAY:
			comboBox.AddString("Day");
			break;

			case TIF_OPG:
			comboBox.AddString("OnOpen");
			break;

			case TIF_IOC:
			comboBox.AddString("IOC");
			break;

			case TIF_FOK:
			comboBox.AddString("FillOrKill");
			break;

			case TIF_GTX:
			comboBox.AddString("Extended");
			break;

			case TIF_GTD:
			comboBox.AddString("Milliseconds");
			break;

			case TIF_ATC:
			comboBox.AddString("OnClose");
			break;
		}
	}
	comboBox.AddString("ETC");
}
*/
void BaseSampleDialogSettingOrder::ToolTipsEnabled(bool enable)
{
	BaseSampleDialogSettingSendOrder::ToolTipsEnabled(enable);
	m_ComboBoxDestination.EnableTooltips(enable, m_toolTip);
	m_ComboBoxRouting.EnableTooltips(enable, m_toolTip);
}

void BaseSampleDialogSettingOrder::InitControls()
{
	BaseSampleDialogSettingSendOrder::InitControls();
//	m_ComboBoxDestination.AddString((const char*)&m_exchangeDestination);

	TakionIterator* iterator = TD_CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		if(!destination->isBorrow())
		{
			m_ComboBoxDestination.AddString((const char*)destination);
		}
	}
	delete iterator;
/*
	const char* const toolTipText = "Stock's Primary Exchange";
	m_ComboBoxEnterDestination.CreateItemToolTipArray();
	int found = m_ComboBoxEnterDestination.FindItem(&m_exchangeDestination);
	if(found >= 0)m_ComboBoxEnterDestination.SetItemToolTip(found, toolTipText, 0);

	m_ComboBoxExitDestination.CreateItemToolTipArray();
	found = m_ComboBoxExitDestination.FindItem(&m_exchangeDestination);
	if(found >= 0)m_ComboBoxExitDestination.SetItemToolTip(found, toolTipText, 0);
*/
	m_ComboBoxDestination.SetCurSel(0);
	OnSelchangeDestination();
}

BOOL BaseSampleDialogSettingOrder::OnInitDialog()
{
	m_allOrNone = false;
	m_pegType = 0;
	m_currentDestination = nullptr;
	m_currentRouting = nullptr;

	BOOL ret = BaseSampleDialogSettingSendOrder::OnInitDialog();

	m_CheckBoxAllOrNone.ResizeToFitText(0, false);


//	for(unsigned char side = OCS_BUY; side < OCS_COUNT; ++side)
	for(unsigned char side = 0; side < OCS_COUNT; ++side)
	{
		switch(side)
		{
			case OCS_BUY:
			m_ComboBoxSide.AddString("Buy");
			break;

			case OCS_SELL:
			m_ComboBoxSide.AddString("Sell");
			break;

			case OCS_CLOSE:
			m_ComboBoxSide.AddString("Close");
			break;

			case OCS_ADD:
			m_ComboBoxSide.AddString("Add");
			break;
		}
	}
	m_ComboBoxSide.SetCurSel(0);

//	m_CheckBoxProAts.SetCheck(BST_CHECKED);

	m_RadioPriceOffset.SetCheck(BST_CHECKED);
	m_priceType = PT_OFFSET;
	UpdatePriceType();

	for(unsigned char priceBase = PB_OPPOSITE_SIDE; priceBase < PB_Count; ++priceBase)
	{
		m_ComboBoxPriceBase.AddString(TU_GetPriceBaseName(priceBase));
	}
	m_ComboBoxPriceBase.SetCurSel(0);

	m_RadioSizeShare.SetCheck(BST_CHECKED);
	m_sizeType = SIT_SHARES;

	m_RadioDisplaySizeShare.SetCheck(BST_CHECKED);
	m_displaySizeType = DST_SHARES;

	FillTifComboBox(m_ComboBoxTifType);

	for(unsigned char pegType = PEG_NONE; pegType < PEG_TYPE_COUNT; ++pegType)
	{
		switch(pegType)
		{
			case PEG_NONE:
			m_ComboBoxPegType.AddString("");
			break;

			case PEG_PRIMARY:
			m_ComboBoxPegType.AddString("Primary");
			break;

			case PEG_MIDPOINT:
			m_ComboBoxPegType.AddString("Midpoint");
			break;

			case PEG_MARKET:
			m_ComboBoxPegType.AddString("Market");
			break;

			default:
			m_ComboBoxPegType.AddString("ERROR");
			break;
		}
	}
	m_ComboBoxPegType.SetCurSel(0);

	m_EditMnemonic.SetLimitText(4);

	m_SpinTif.SetLeftmostMaxDigitValue(8);

	m_ComboBoxTifType.SetCurSel(0);
	OnSelchangeTif();

/*Moved to InitControls
	TakionIterator* iterator = TD_CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		if(!destination->isBorrow())
		{
			m_ComboBoxDestination.AddString((const char*)destination);
		}
	}
	delete iterator;
*/
/*
	const bool enableToolTips = true;
	EnableToolTips(enableToolTips);
	m_ComboBoxDestination.EnableTooltips(enableToolTips, m_toolTip);
	m_ComboBoxDestination.SetCurSel(0);

	OnSelchangeDestination();
*/
	return ret;
}

unsigned int BaseSampleDialogSettingOrder::GetTifMilliseconds() const
{
	return m_SpinTif.GetMillisecond();
}

unsigned char BaseSampleDialogSettingOrder::GetTifType() const
{
	const int sel = m_ComboBoxTifType.GetCurSel();
	if(sel >= 0)
	{
/*
	TIF_DAY	= '0',		//Day (or session)
//	TIF_GTC = '1',		//Good Till Cancel (GTC)
	TIF_OPG = '2',		//At the Opening (OPG)
	TIF_IOC = '3',		//Immediate Or Cancel (IOC)
	TIF_FOK = '4',		//Fill Or Kill (FOK)
	TIF_GTX = '5',		//Good Till Crossing (GTX)
	TIF_GTD = '6',		//Good Till Date (GTD)
	TIF_ATC = '7',		//At the Close
*/
//		if(sel == 0)
//			return TIF_DAY;

		switch(sel)
		{
			case 0:
			return TIF_DAY;

			case 7:
			return TIF_ETC;

			default:
			return TIF_DAY + 1 + (unsigned char)sel;
		}
//		return TIF_DAY + 1 + (unsigned char)sel;
	}
	return 0xFF;
}

void BaseSampleDialogSettingOrder::DoDataExchange(CDataExchange* pDX)
{
	BaseSampleDialogSettingSendOrder::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxSideId, m_ComboBoxSide);
//	DDX_Control(pDX, checkBoxProAtsId, m_CheckBoxProAts);

	DDX_Control(pDX, radioPriceMarketId, m_RadioPriceMarket);
	DDX_Control(pDX, radioPriceOffsetId, m_RadioPriceOffset);
	DDX_Control(pDX, radioPriceAbsoluteId, m_RadioPriceAbsolute);

	DDX_Control(pDX, checkBoxPricePercentId, m_CheckBoxPricePercent);
	DDX_Control(pDX, comboBoxPriceBaseId, m_ComboBoxPriceBase);
	DDX_Control(pDX, spinPriceOffsetId, m_SpinPriceOffset);
	DDX_Control(pDX, spinPriceId, m_SpinPrice);

	DDX_Control(pDX, radioSizePositionId, m_RadioSizePosition);
	DDX_Control(pDX, radioSizeShareId, m_RadioSizeShare);
	DDX_Control(pDX, spinSizeShareId, m_SpinSizeShare);

	DDX_Control(pDX, radioDisplaySizeAllId, m_RadioDisplaySizeAll);
	DDX_Control(pDX, radioDisplaySizeShareId, m_RadioDisplaySizeShare);
	DDX_Control(pDX, spinDisplaySizeShareId, m_SpinDisplaySizeShare);

	DDX_Control(pDX, comboBoxTifId, m_ComboBoxTifType);
	DDX_Control(pDX, spinTifId, m_SpinTif);

	DDX_Control(pDX, staticDestinationId, m_StaticDestination);
	DDX_Control(pDX, staticRoutingId, m_StaticRouting);
	DDX_Control(pDX, staticMnemonicId, m_StaticMnemonic);
	DDX_Control(pDX, staticReserveSizeId, m_StaticReserveSize);

	DDX_Control(pDX, comboBoxDestinationId, m_ComboBoxDestination);
	DDX_Control(pDX, comboBoxRoutingId, m_ComboBoxRouting);
	DDX_Control(pDX, editMnemonicId, m_EditMnemonic);
	DDX_Control(pDX, spinReserveSizeId, m_SpinReserveSize);

	DDX_Control(pDX, groupPegId, m_GroupPeg);
	DDX_Control(pDX, comboBoxPegTypeId, m_ComboBoxPegType);
	DDX_Control(pDX, spinPegPriceOffsetId, m_SpinPegOffset);

	DDX_Control(pDX, spinMinQuantityId, m_SpinMinQuantity);
	DDX_Control(pDX, checkboxAllOrNoneId, m_CheckBoxAllOrNone);
}

BEGIN_MESSAGE_MAP(BaseSampleDialogSettingOrder, BaseSampleDialogSettingSendOrder)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(radioPriceMarketId, OnPriceType)
	ON_BN_CLICKED(radioPriceOffsetId, OnPriceType)
	ON_BN_CLICKED(radioPriceAbsoluteId, OnPriceType)

	ON_BN_CLICKED(radioSizePositionId, OnSizeType)
	ON_BN_CLICKED(radioSizeShareId, OnSizeType)

	ON_BN_CLICKED(radioDisplaySizeAllId, OnDisplaySizeType)
	ON_BN_CLICKED(radioDisplaySizeShareId, OnDisplaySizeType)

	ON_CBN_SELCHANGE(comboBoxTifId, OnSelchangeTif)
	ON_CBN_SELCHANGE(comboBoxDestinationId, OnSelchangeDestination)
	ON_CBN_SELCHANGE(comboBoxRoutingId, OnSelchangeRouting)
	ON_EN_CHANGE(editMnemonicId, OnMnemonicChange)
	ON_EN_UPDATE(editMnemonicId, OnMnemonicUpdate)

	ON_BN_CLICKED(checkboxAllOrNoneId, OnAllOrNone)

	ON_CBN_SELCHANGE(comboBoxPegTypeId, OnSelchangePegType)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void BaseSampleDialogSettingOrder::OnAllOrNone()
{
	const bool allOrNone = m_CheckBoxAllOrNone.GetCheck() == BST_CHECKED;
	if(allOrNone != m_allOrNone)
	{
		m_allOrNone = allOrNone;
		m_modified = true;
		UpdateAllOrNone();
	}
}

void BaseSampleDialogSettingOrder::OnSelchangePegType()
{
	const unsigned char pegType = (unsigned char)m_ComboBoxPegType.GetCurSel();
	if(pegType != m_pegType)
	{
		m_pegType = pegType;
		m_modified = true;
		UpdatePegType();
	}
}

void BaseSampleDialogSettingOrder::OnDisplaySizeType()
{
	unsigned char displaySizeType = m_RadioDisplaySizeAll.GetCheck() == BST_CHECKED ? DST_ALL:
		m_RadioDisplaySizeShare.GetCheck() == BST_CHECKED ? DST_SHARES:
		0xFF;
	if(m_displaySizeType != displaySizeType)
	{
		m_displaySizeType = displaySizeType;
		m_SpinDisplaySizeShare.SetBkColorEnabled(m_displaySizeType != DST_SHARES ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));
	}
}

void BaseSampleDialogSettingOrder::OnSizeType()
{
	unsigned char sizeType = m_RadioSizePosition.GetCheck() == BST_CHECKED ? SIT_POSITION:
		m_RadioSizeShare.GetCheck() == BST_CHECKED ? SIT_SHARES:
		0xFF;
	if(m_sizeType != sizeType)
	{
		m_sizeType = sizeType;
		m_SpinSizeShare.SetBkColorEnabled(m_sizeType != SIT_SHARES ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));
	}
}

void BaseSampleDialogSettingOrder::UpdatePriceType()
{
	switch(m_priceType)
	{
		case PT_OFFSET:
		m_SpinPriceOffset.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
		m_ComboBoxPriceBase.SetInvalid(false);
		m_SpinPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
		break;

		case PT_ABSOLUTE:
		m_SpinPriceOffset.SetBkColorEnabled(m_visualBase->GetGrayColor());
		m_ComboBoxPriceBase.SetInvalid(true);
		m_SpinPrice.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
		break;

		default:
		m_SpinPriceOffset.SetBkColorEnabled(m_visualBase->GetGrayColor());
		m_ComboBoxPriceBase.SetInvalid(true);
		m_SpinPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
		break;
	}
}

void BaseSampleDialogSettingOrder::OnPriceType()
{
	unsigned char priceType = m_RadioPriceMarket.GetCheck() == BST_CHECKED ? PT_MARKET:
		m_RadioPriceOffset.GetCheck() == BST_CHECKED ? PT_OFFSET:
		m_RadioPriceAbsolute.GetCheck() == BST_CHECKED ? PT_ABSOLUTE:
		0xFF;
	if(m_priceType != priceType)
	{
		m_priceType = priceType;
		UpdatePegValid();
		UpdatePriceType();
	}
}

void BaseSampleDialogSettingOrder::OnMnemonicUpdate()
{
	m_EditMnemonic.TryUpdate();
}

void BaseSampleDialogSettingOrder::OnMnemonicChange()
{
	m_modified = true;
}

LRESULT BaseSampleDialogSettingOrder::OnSpinChange(WPARAM spinId, LPARAM l)
{
	switch(spinId)
	{
		case spinPriceId:
		case spinSizeShareId:
		case spinDisplaySizeShareId:
		case spinTifId:
		case spinReserveSizeId:
		case spinPegPriceOffsetId:
		m_modified = true;
		break;
#ifndef TAKION_NO_OPTIONS
		default:
		BaseSampleDialogSettingSendOrder::OnSpinChange(spinId, l);
		break;
#endif
	}
	return 0;
}

void BaseSampleDialogSettingOrder::OnSelchangeDestination()
{
//	UpdateDestinationRouting();
	if(UpdateDestinationRoutingStatic(m_currentDestination, m_currentDestination, m_ComboBoxDestination, m_ComboBoxRouting, &m_EditMnemonic))
	{
//		m_ComboBoxDestination.SetInvalid(controlDestination != currentDestination);
		VenueChanged();
		OnSelchangeRouting();
		m_modified = true;
	}
}

void BaseSampleDialogSettingOrder::OnSelchangeRouting()
{
	const DestinationRouting* const routing = OnSelchangeRoutingStatic(m_ComboBoxRouting, m_SpinReserveSize, m_StaticReserveSize);
	if(routing != m_currentRouting)
	{
		m_currentRouting = routing;
		RoutingChanged();
		m_modified = true;
	}
}

void BaseSampleDialogSettingOrder::UpdateTifType()
{
	m_SpinTif.SetBkColorEnabled(GetTifType() == TIF_GTD ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetGrayColor());//m_SpinTif.GetBkColorDisabled());//GetSysColor(COLOR_BTNFACE));
}

void BaseSampleDialogSettingOrder::OnSelchangeTif()
{
	UpdateTifType();
}

HBRUSH BaseSampleDialogSettingOrder::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editMnemonicId:
		if(!m_currentDestination || !m_currentDestination->HasMnemonic())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		break;
	}
	return BaseSampleDialogSettingSendOrder::OnCtlColor(pDC, pWnd, nCtlColor);
}

