#include "stdafx.h"
#include "BaseSampleDialogSettingSendOrder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseSampleDialogSettingSendOrder::BaseSampleDialogSettingSendOrder(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseSampleDialogSettingOption(mainWnd, parentTab, true, label, 64, 0),
	m_SpinUserId(0xFFFFFFFFFFFFFFFF, 0, 0, 100000, 3, TakionSpin::charComma),
	m_SpinUserType(0xFFFF, 0, 0, 100, 3, TakionSpin::charComma),
	m_SpinExtensionId(0xFFFFFFFF, 0, 0, 1000, 3, TakionSpin::charComma),
	m_order(nullptr)
{
	m_SpinUserId.AddSpinCell(1, false);
	m_SpinUserId.AddSpinCell(100, false);
	m_SpinUserId.AddSpinCell(1000000, true);
	m_SpinUserId.AddSpinCell(1000, true);

	m_SpinUserType.AddSpinCell(1, false);
	m_SpinUserType.AddSpinCell(100, false);

	m_SpinExtensionId.AddSpinCell(1, false);
	m_SpinExtensionId.AddSpinCell(100, false);
	m_SpinExtensionId.AddSpinCell(1000000, true);
	m_SpinExtensionId.AddSpinCell(1000, true);

	int x = 0;
/*
//	int y = m_bottomControl + vgap;
	m_bottomControl += vgap;
	int width = m_accountWidth;//52;
	int xc = x + hgap;
	AddStaticIndirect("Symbol", WS_VISIBLE | SS_NOPREFIX, 0, xc, m_bottomControl, width, staticHeight, staticSymbolId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xc, m_bottomControl += staticHeight, width, editHeight, editSymbolId);

	m_bottomControl += editHeight;
*/
	int y = m_height - 25;
//	AddButtonIndirect("Account", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, width + hgap + hgap + comboBoxHeight, 24, groupAccountId);
//	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, y + groupOffset, width + comboBoxHeight, 400, comboBoxAccountId);

//	x += m_accountWidth + hgap + hgap + comboBoxHeight + hgap;
	x += m_accountWidth + hgap;// + hgap;
	int xc = x + hgap;
	const int width = 44;
	const int w = 2 * width + 3 * comboBoxHeight;
	const int wt = width / 2 + 2 * spinHeight;// comboBoxHeight;
	const int groupHeight = 24;
	AddButtonIndirect("User ID", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, w + hgap + hgap, groupHeight, groupUserIdId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, y + groupOffset, w, spinHeight, spinUserIdId);

	x += w + 3 * hgap;
	xc = x + hgap;
	AddButtonIndirect("UserType", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, wt + hgap + hgap, groupHeight, groupUserTypeId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, y + groupOffset, wt, spinHeight, spinUserTypeId);

	x += wt + 3 * hgap;
	xc = x + hgap;
	const int destWidth = 80;// 72;// 36;

	AddButtonIndirect("Extension ID", WS_VISIBLE|BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, destWidth + hgap + hgap, groupHeight, groupExtensionId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, xc, y + groupOffset, destWidth, spinHeight, spinExtensionId);

//	AddButtonIndirect("Send Order", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y + groupOffset, destWidth + destWidth + hgap, comboBoxHeight, buttonSendOrderId);
	AddButtonIndirect("Send Order", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x, y + groupHeight + vgap, destWidth + hgap, comboBoxHeight, buttonSendOrderId);
}

void BaseSampleDialogSettingSendOrder::RemoveStock(Security* stock)
{
	PendingStockMap::iterator found = m_pendingStockMap.find(stock);
	if(found != m_pendingStockMap.end())
	{
		delete found->second;
		m_pendingStockMap.erase(found);
	}
}

void BaseSampleDialogSettingSendOrder::ClearStocks()
{
	for(PendingStockMap::iterator it = m_pendingStockMap.begin(), itend = m_pendingStockMap.end(); it != itend; ++it)
	{
		delete it->second;
	}
	m_pendingStockMap.clear();
}

void BaseSampleDialogSettingSendOrder::ClearAccountOrders(Account* account)
{
	MyStock* stock;
	for(PendingStockMap::iterator it = m_pendingStockMap.begin(), itend = m_pendingStockMap.end(); it != itend;)
	{
		stock = it->second;
		if(stock->ClearAccountOrders(account))
		{
			it = m_pendingStockMap.erase(it);
			delete stock;
		}
		else
		{
			++it;
		}
	}
}

void BaseSampleDialogSettingSendOrder::BeforeDestroy()
{
	if(m_order)
	{
		m_order->RemoveObserver(this);
		m_order = nullptr;
	}
	TD_GetNewOrderObservable().RemoveObserver(this);
	ClearStocks();
	BaseSampleDialogSettingOption::BeforeDestroy();
}

unsigned char BaseSampleDialogSettingSendOrder::FillOrderParams(BaseOrderParams& orderParams) const
{
	orderParams.SetUserId(m_SpinUserId.GetValue());
	orderParams.SetUserType(m_SpinUserType.GetValue());
	orderParams.SetExtensionId(m_SpinExtensionId.GetValue());
/*
	int sel = m_ComboBoxAccount.GetCurSel();
	Account* account = sel < 0 ? nullptr : (Account*)m_ComboBoxAccount.GetItemDataPtr(sel);
	if(!account)
	{
		account = TD_GetCurrentAccount();
	}
	orderParams.SetAccount(account);
	if(!account)return SPE_NO_ACCOUNT;
*/
	Account* account = (Account*)m_account;
	if(!account)
	{
		account = TD_GetCurrentAccount();
	}
	orderParams.SetAccount(account);
	if(!account)return SPE_NO_ACCOUNT;

	return SPE_OK;
}

BOOL BaseSampleDialogSettingSendOrder::OnInitDialog()
{
	BOOL ret = BaseSampleDialogSettingOption::OnInitDialog();
	TD_GetNewOrderObservable().AddObserver(this);
	return ret;
}

void BaseSampleDialogSettingSendOrder::DoDataExchange(CDataExchange* pDX)
{
	BaseSampleDialogSettingOption::DoDataExchange(pDX);

	DDX_Control(pDX, spinUserIdId, m_SpinUserId);
	DDX_Control(pDX, spinUserTypeId, m_SpinUserType);
	DDX_Control(pDX, spinExtensionId, m_SpinExtensionId);

	DDX_Control(pDX, buttonSendOrderId, m_ButtonSendOrder);
}

BEGIN_MESSAGE_MAP(BaseSampleDialogSettingSendOrder, BaseSampleDialogSettingOption)
//	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(buttonSendOrderId, OnSendOrder)
END_MESSAGE_MAP()

bool BaseSampleDialogSettingSendOrder::AdjustPrice(const SignedPrice& offset, bool priceOffsetPercent, Price& price, char side)// const
{
	if(!offset.isZero())
	{
		SignedPrice signedPrice(price);
		AdjustSignedPrice(offset, priceOffsetPercent, signedPrice, side);
		if(signedPrice.isNegative())
		{
			price.SetZero();
			return false;
		}
		else
		{
			price.SetValue(signedPrice.GetDollars(), signedPrice.GetDollarFraction());
		}
	}
	return true;
}

void BaseSampleDialogSettingSendOrder::AdjustSignedPrice(const SignedPrice& offset, bool priceOffsetPercent, SignedPrice& price, char side)
{
	switch(side)
	{
		case 'B':
		if(priceOffsetPercent)
		{
			Money m = price * ((__int64)offset.GetDollars() * 10000 + offset.GetDollarFraction() / (offset.divider / 10000));
			m /= 1000000;
			SignedPrice po = SignedPrice(m);
			bool rounded2 = (unsigned int)price.GetDollarFraction() % (SignedPrice::divider / 100) == 0;
			price += po;
//			price.Round(price >= SignedPrice(1, 0) && rounded2 ? 100 : 10000, -1);
			price.Round(price.GetDollars() >= 1 && rounded2 ? 100 : 10000, -1);
		}
		else
		{
			price += offset;
		}
		break;

		case 'S':
		if(priceOffsetPercent)
		{
			Money m = price * ((__int64)offset.GetDollars() * 10000 + offset.GetDollarFraction() / (offset.divider / 10000));
			m /= 1000000;
			SignedPrice po = SignedPrice(m);
			bool rounded2 = (unsigned int)price.GetDollarFraction() % (SignedPrice::divider / 100) == 0;
			price -= po;
//			price.Round(price >= SignedPrice(1, 0) && rounded2 ? 100 : 10000, 1);
			price.Round(price.GetDollars() >= 1 && rounded2 ? 100 : 10000, 1);
		}
		else
		{
			price -= offset;
		}
		break;
	}
}

unsigned char BaseSampleDialogSettingSendOrder::CalculatePriceByOffset(char side,
	const Security* stock,
	const Price& insideBid,
	const Price& insideAsk,
	const Price& positionPrice,
	const Price& positionCost,
	unsigned char priceBase,
	const SignedPrice& priceOffset,
	bool priceOffsetPercent,
	Price& limitPrice)
{
	Price price;
	switch(priceBase)
	{
		case PB_ECN_OPPOSITE_SIDE:
		price = Price(stock->GetEcnQuote(side != 'B'));
		break;

		case PB_ECN_SAME_SIDE:
		price = Price(stock->GetEcnQuote(side == 'B'));
		break;

		case PB_MH_HIGH:
		price = Price(stock->GetMarketHoursHighPriceCompact());
		break;

		case PB_MH_LOW:
		price = Price(stock->GetMarketHoursLowPriceCompact());
		break;

		case PB_LAST_PRINT:
		price = Price(stock->GetLastNbboPrintOrClosePrice());
		break;

		case PB_OPEN_PRICE:
		price = Price(stock->GetOpenPriceCompact());
		break;

		case PB_AVG_POSITION_COST:
		price = positionCost;
		break;

		case PB_CUSTOM:
		price = Price(stock->GetCustomPriceBaseCompact());
		break;

		case PB_IMB_EX_NEAR:
		price = Price(stock->GetNearPriceCompactExch());
		break;

		case PB_MIDPOINT:
		price = (insideBid + insideAsk) / 2;
		break;

		case PB_NYSE_OPPOSITE_SIDE:
		price = Price(stock->GetNysQuote(side != 'B'));
		if(!price.isZero())
		{
			break;
		}
//fall through
		case PB_OPPOSITE_SIDE:
		price = side == 'B' ? insideAsk : insideBid;
		break;

		case PB_NYSE_SAME_SIDE:
		price = Price(stock->GetNysQuote(side == 'B'));
//		price = Price(side == 'S' ? stock->GetNysAsk() : stock->GetNysBid());
		if(!price.isZero())
		{
			break;
		}
//fall through
		case PB_SAME_SIDE:
		price = side == 'S' ? insideAsk : insideBid;
		break;

		case PB_TODAYS_CLOSE_PRICE:
		price = Price(stock->GetTodaysClosePriceCompact());
		if(!price.isZero())
		{
			break;
//				price = SignedPrice(security->GetClosePriceCompact());
		}
//fall through
		case PB_CLOSE_PRICE:
		price = Price(stock->GetClosePriceCompact());
		break;

		case PB_AVG_POSITION_PRICE:
		price = positionPrice;
		break;
	}
	if(!price.isZero() || TD_IsSimulation())//mm && mm->isSimulation())
	{
		AdjustPrice(priceOffset, priceOffsetPercent, price, side);//, hasPrints);
//			return price.isPositive();
		if(price.isPositive())
		{
			limitPrice = price;
		}
		else
		{
			return OE_PRICE_ZERO;
		}
	}
	else
	{
		return OE_PRICE_ZERO;
	}
	return OE_OK;
}

void BaseSampleDialogSettingSendOrder::OnSendOrder()
{
	std::string errorMessage;
//	CString symbol;
//	m_EditSymbol.GetWindowText(symbol);
	if(m_symbol.empty())
	{
		errorMessage = "No Symbol";
	}
#ifndef TAKION_NO_OPTIONS
	else if(!AreOptionSettingsValid())
	{
		errorMessage = "Option Settings invalid";
	}
#endif
	else
	{
		BaseOrderParams* orderParams = CreateOrderParams();
		unsigned char errorCode = FillOrderParams(*orderParams);
		switch(errorCode)
		{
			case SPE_OK:
			{
				Security* stock = nullptr;
#ifndef TAKION_NO_OPTIONS
				if(isOptionUsed())
				{
					unsigned __int64 rootSymbol;
					unsigned __int64 underlierSymbol;
					unsigned __int64 optionBlock;
					unsigned short optionType;
					bool option = false;
					option = GetOptionValues(rootSymbol, underlierSymbol, optionBlock, optionType);
					if(option)
					{
						stock = TD_ObtainOptionByRootAndOptionBlock(rootSymbol, optionBlock, false);//When the underlier symbol is not known
					}
				}
				else
				{
#endif
					if(MustBeSubscribed())
					{
						stock = TD_ObtainStock(m_symbol.c_str(), false);
					}
					else
					{
						stock = TD_FindStock(m_symbol.c_str());
						if(!stock)
						{
							stock = TD_FindSortableEquity(m_symbol.c_str());

							if(!stock || !stock->isLoaded())
							{
								stock = TD_ObtainStock(m_symbol.c_str(), false);
							}
						}
					}
#ifndef TAKION_NO_OPTIONS
				}
#endif
				if(stock)
				{
					if(stock->isLoaded())
					{
						unsigned char sendErrorCode = SendOrder(stock, *orderParams);
					}
					else
					{
//						stock->AddObserver(this);
//Stock is not loaded.
//Remember the orderParams and wait for message TM_STOCK_REFRESHED, TM_STOCK_INVALID, TM_STOCK_DELETE, TM_FUTURE_DELETE, TM_FUTURE_REFRESHED, TM_FUTURE_INVALID
						PendingStockMap::iterator found = m_pendingStockMap.find(stock);
						if(found == m_pendingStockMap.end())
						{
							found = m_pendingStockMap.insert(PendingStockMap::value_type(stock, new MyStock(stock, this))).first;
						}
						found->second->AddOrderParams(orderParams);
						orderParams = nullptr;
						stock->Subscribe();
					}
					TD_ReleaseStock(stock);
				}
				else
				{
					errorMessage = "Can't obtain stock";
				}
			}
			break;

			case SPE_NO_SIDE:
			errorMessage = "No Side";
			break;

			case SPE_NO_PRICE_TYPE:
			errorMessage = "No Price Type";
			break;

			case SPE_NO_PRICE_BASE:
			errorMessage = "No Price Base";
			break;

			case SPE_NO_SIZE_TYPE:
			errorMessage = "No Size Type";
			break;

			case SPE_NO_DISPLAY_SIZE_TYPE:
			errorMessage = "No Display Size Type";
			break;

			case SPE_NO_TIF_TYPE:
			errorMessage = "No TIF Type";
			break;

			case SPE_NO_DESTINATION:
			errorMessage = "No Destination";
			break;

			case SPE_NO_ACCOUNT:
			errorMessage = "No Account";
			break;

			default:
			{
				errorMessage = "Unknown Error ";
				char num[33];
				errorMessage += U_SignedNumberToStrNoDecDigits(errorCode, num, sizeof(num));
			}
			break;
		}

		if(orderParams)
		{
			delete orderParams;
		}
	}
}
/*
HBRUSH BaseSampleDialogSettingOption::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSymbolId:
		if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_takionVisual->GetWarningBrush();
		}
		break;

	}
	return BaseDialogSettingAccount::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/
void MyStock::ClearOrderParamsSet()
{
	for(OrderParamsSet::const_iterator it = m_orderParamsSet.begin(), itend = m_orderParamsSet.end(); it != itend; ++it)
	{
		delete *it;
	}
	m_orderParamsSet.clear();
}

bool MyStock::ClearAccountOrders(Account* account)
{
	const BaseOrderParams* orderParams;
	for(OrderParamsSet::const_iterator it = m_orderParamsSet.begin(), itend = m_orderParamsSet.end(); it != itend; )
	{
		orderParams = *it;
		if(orderParams->GetAccount() == account)
		{
			it = m_orderParamsSet.erase(it);
			delete orderParams;
		}
		else
		{
			++it;
		}
	}
	return m_orderParamsSet.empty();
}

void MyStock::SendOrders()
{
	const BaseOrderParams* orderParams;
	for(OrderParamsSet::const_iterator it = m_orderParamsSet.begin(), itend = m_orderParamsSet.end(); it != itend; ++it)
	{
		orderParams = *it;
		m_dlg->SendOrder(m_stock, *orderParams);
		delete orderParams;
	}
	m_orderParamsSet.clear();
}

void MyStock::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_STOCK_REFRESHED:
#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_REFRESHED:
		case TM_FUTURE_REFRESHED:
#endif
		SendOrders();
//		break;

//fall through

		case TM_STOCK_INVALID:
		case TM_STOCK_DELETE:
#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_INVALID:
		case TM_FUTURE_INVALID:
		case TM_FUTURE_DELETE:
#endif
		m_dlg->RemoveStock(m_stock);
//this object is destroyed in the above line function. don't call yny more functions of this object.
		break;
	}
}

void BaseSampleDialogSettingSendOrder::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_UPDATED_ORDER:
		{
			const TMsgUpdatedOrder* msg = (const TMsgUpdatedOrder*)message;
			Order* order = msg->m_order;
		}
		break;

		case TM_NEW_ORDER:
//		DisplayNewOrder(((const TMsgOrder*)message)->m_order);
		{
			const TMsgNewOrder* msg = (const TMsgNewOrder*)message;
			m_order = msg->m_order;
			unsigned int pendingShares = msg->m_pendingShares;
			m_order->AddObserver(this);
		}
		break;
/*
		case TM_ORDER_UPDATE:
		break;
*/
		case TM_DEAD_ORDER:
//		DisplayCanceledOrder(((const TMsgOrder*)message)->m_order);
		if(&TD_GetNewOrderObservable() == from)
		{
			const TMsgDeadOrder* msg = (const TMsgDeadOrder*)message;
			Order* order = msg->m_order;
			if(order == m_order)
			{
				order->RemoveObserver(this);
				m_order = nullptr;
			}
		}
		break;

		case TM_NEW_EXECUTION:
//		DisplayExecution(((const TMsgExecution*)message)->m_execution);
		break;

		case TM_AUTO_ACTION:
		{
			const TMsgAutoAction* msg = (const TMsgAutoAction*)message;

			if(&TD_GetAdminObservable() == from && info)
			{
				switch(info->m_type)
				{
					case TM_UPDATED_ACCOUNT:
					{
						const TMsgUpdatedAccount* infoMsg = (const TMsgUpdatedAccount*)info;
						const Account* account = infoMsg->m_account;
					}
					break;

					case TM_UPDATED_ACCOUNT_AND_POSITION:
					{
						const TMsgUpdatedAccountAndPosition* infoMsg = (const TMsgUpdatedAccountAndPosition*)info;
						const Account* account = infoMsg->m_account;
						const Position* position = infoMsg->m_position;
					}
					break;
				}
			}
		}
		break;

		case TM_ACCOUNT_LOSS_90_PERCENT:
		{
			const TMsgAccountLoss90Percent* msg = (const TMsgAccountLoss90Percent*)message;
//			msg->m_accountId
			if(info && info->m_type == TM_UPDATED_ACCOUNT)
			{
				const TMsgUpdatedAccount* infoMsg = (const TMsgUpdatedAccount*)info;
				const Account* account = infoMsg->m_account;
			}
		}
		break;

		case TM_ORDER_ERROR:
		if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
		{
			Account* account = ((TMsgUpdatedAccountAndPosition*)info)->m_account;
//			if(account == m_account || !m_account)
//			{
				const TMsgOrderError* msg = (const TMsgOrderError*)message;
				char side;
				switch(msg->Side)
				{
					case ORDER_SIDE_BUY://					'1'		// = Buy
					side = 'B';
					break;

					case ORDER_SIDE_SELL://					'2'		// = Sell
					side = 'S';
					break;

					case ORDER_SIDE_SELL_SHORT://			'5'		// = Sell short
					side = 'T';
					break;

					case ORDER_SIDE__BUY_MINUS://			'3'		// = Buy minus
					side = 'B';
					break;

					case ORDER_SIDE_SELL_PLUS://			'4'		// = Sell plus
					side = 'S';
					break;

					case ORDER_SIDE_SELL_SHORT_EXEMPT://	'6'		// = Sell short exempt
					side = 'T';
					break;

					case ORDER_SIDE_UNDISCLOSED://			'7'		// = Undisclosed (valid for IOI and List Order messages only)
					case ORDER_SIDE_CROSS://				'8'		// = Cross (orders where counterparty is an exchange, valid for all messages except IOIs)
					case ORDER_SIDE_CROSS_SHORT://			'9'		// = Cross short
					case ORDER_SIDE_SHORT_EXEMPT://			'A'		//= Cross short exxmpt
					case ORDER_SIDE_AS_DEFINED://			'B'		//"As Defined" (for use with multileg instruments)
					case ORDER_SIDE_OPPOSITE://				'C'		// = "Opposite" (for use with multileg instruments)
					side = msg->Side;
					break;

					default:
					side = 'U';
					break;
				}
//				DisplayOrderError(account, msg->Symbol, side, msg->m_errorCode, msg->m_moneyValue, msg->m_moneyConstraint, msg->m_uintValue, msg->m_uintConstraint);
//			}
		}
		break;

		case TM_CANCEL_ERROR:
		if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
		{
			Account* account = ((TMsgUpdatedAccountAndPosition*)info)->m_account;
			const TMsgCancelError* msg = (const TMsgCancelError*)message;
			char side;
			switch(msg->Side)
			{
				case ORDER_SIDE_BUY://					'1'		// = Buy
				side = 'B';
				break;

				case ORDER_SIDE_SELL://					'2'		// = Sell
				side = 'S';
				break;

				case ORDER_SIDE_SELL_SHORT://			'5'		// = Sell short
				side = 'T';
				break;

				case ORDER_SIDE__BUY_MINUS://			'3'		// = Buy minus
				side = 'B';
				break;

				case ORDER_SIDE_SELL_PLUS://			'4'		// = Sell plus
				side = 'S';
				break;

				case ORDER_SIDE_SELL_SHORT_EXEMPT://	'6'		// = Sell short exempt
				side = 'T';
				break;

				case ORDER_SIDE_UNDISCLOSED://			'7'		// = Undisclosed (valid for IOI and List Order messages only)
				case ORDER_SIDE_CROSS://				'8'		// = Cross (orders where counterparty is an exchange, valid for all messages except IOIs)
				case ORDER_SIDE_CROSS_SHORT://			'9'		// = Cross short
				case ORDER_SIDE_SHORT_EXEMPT://			'A'		//= Cross short exxmpt
				case ORDER_SIDE_AS_DEFINED://			'B'		//"As Defined" (for use with multileg instruments)
				case ORDER_SIDE_OPPOSITE://				'C'		// = "Opposite" (for use with multileg instruments)
				side = msg->Side;
				break;

				default:
				side = 'U';
				break;
			}
//			DisplayCancelError(account, msg->Symbol, side, msg->m_errorCode, msg->m_moneyValue, msg->m_moneyConstraint, msg->m_uintValue, msg->m_uintConstraint);
		}
		break;

		case M_TEXT:
		{
//			const TMsgText* msg = (const TMsgText*)message;
//			DisplayAdminBroadcastMessage((const char*)msg + sizeof(TMsgText));
		}
		break;

		case TM_MARKET_SORTER_LOADED:
//		MarketSorterLoaded();
		break;

		case TM_MARKET_SORTER_CLEANUP:
//		MarketSorterCleanup();
		break;

		default:
		BaseSampleDialogSettingOption::Notify(message, from, info);
		break;
	}
}

