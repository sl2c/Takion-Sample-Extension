#include "stdafx.h"
#include "SampleDialogSettingSimulateAccountRisk.h"
//#include "AccountConstraint.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateAccountRisk::SampleDialogSettingSimulateAccountRisk(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab)://, const char* label):
	BaseDialogSettingSimulateAccount(mainWnd, parentTab, "AccountRisk", true),
//	m_ComboBoxAccount(0xFFFFFFFF, "Current"),
	m_SpinAccountBalance(999999999999, -999999999999, 0, 100000, 3, TakionSpin::charComma, 2),
	m_ListBoxConstraint(GetBkColor()),
	m_EditSearch(false, "Type in text to search for Account Constraints whose name contains the text"),
	m_searchDone(true),
	m_zeroAccountConstraint("", true),
	m_leftOffset(0),
	m_topOffset(0),
	m_listBoxBorderSize(0)
{
	SetSignedMoneySpinCells(m_SpinAccountBalance);

	m_ListBoxConstraint.SetAccountPresent(true);

	m_ListBoxConstraint.AddListBoxToTakionDialog(this);
/*
	int width = 52;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc + m_sendButtonWidth + hgap, vgap, width, 400, comboBoxRiskAccountId);
*/

	int x = m_xc + m_sendButtonWidth + hgap + m_comboBoxAccountWidth + 3 * hgap;
	int y = vgap;
	if(buttonHeight > editHeight)
	{
		y += (buttonHeight - comboBoxHeight) / 2;
	}
	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL|ES_UPPERCASE, 0, x, y, m_comboBoxAccountWidth, editHeight, editSearchConstraintId);
	y = vgap;
	const int buttonWidth = m_sendButtonWidth / 2 + 2 * hgap;
	AddButtonIndirect("Search", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, x += m_comboBoxAccountWidth + hgap, y, buttonWidth, buttonHeight, buttonSearchConstraintId);
	AddButtonIndirect("Nullify", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, x += buttonWidth + 3 * hgap, y, buttonWidth, buttonHeight, buttonNullifyConstraintId);
	AddButtonIndirect("Reset", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, x += buttonWidth + hgap, y, buttonWidth, buttonHeight, buttonResetConstraintId);

	x = 0;
	m_bottomControl += vgap;

	const int doubleWidth = m_firstColumnWidth + m_firstColumnWidth;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, doubleWidth, spinHeight, spinConstraintAccountBalanceId);
	AddStaticIndirect("Balance", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + doubleWidth + 1, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticConstraintAccountBalanceId);

	m_bottomControl += spinHeight + vgap;
//	y = m_bottomControl;
	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT|WS_VSCROLL, 0, x, m_bottomControl, m_width, m_height - m_bottomControl, listBoxConstraintId);

//	SetExecutorConnected(TD_IsExecutorConnected());
}

void SampleDialogSettingSimulateAccountRisk::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingSimulateAccount::ToolTipsEnabled(enable);
	m_ListBoxConstraint.EnableTooltips(enable, m_toolTip);
	m_EditSearch.EnableTooltips(enable, m_toolTip);
}

bool SampleDialogSettingSimulateAccountRisk::SearchItems()
{
//	return SearchItemsStatic(m_EditSearch, m_searchStr, m_ListBoxConstraint, nullptr, m_ButtonSearch, m_searchDone);
	CString text;
	m_EditSearch.GetWindowText(text);

	if(m_searchStr != text)
	{
		m_searchStr = text;

		m_ListBoxConstraint.SetSearchString(m_searchStr);
		m_searchDone = true;
		m_EditSearch.Invalidate();
		EnableControl(m_ButtonSearch.m_hWnd, false, m_EditSearch.m_hWnd);
		m_ListBoxConstraint.MakeHighlightedItemVisibleIfNoneVisible();
		return true;
	}

	return false;
}

BOOL SampleDialogSettingSimulateAccountRisk::PreTranslateMessage(MSG* pMsg)
{
/*
//	if(m_toolTip)m_toolTip->RelayEvent(pMsg);
	bool sysKey = false;
	switch(pMsg->message)
	{
		case WM_SYSKEYDOWN:
		sysKey = true;
//fall through
		case WM_KEYDOWN:
		{
			unsigned char ascii = (unsigned char)pMsg->wParam;
			switch(ascii)
			{
				case VK_CAPITAL:
				case VK_SHIFT:
				case VK_CONTROL:
				case VK_MENU:
				case VK_NUMLOCK:
				case VK_SCROLL:
				case VK_LWIN:
				case VK_RWIN:
				case VK_APPS:
				break;

				default:
//				{
					if(!sysKey)
					{
						switch(ProcessKeyInSearch(pMsg))
						{
							case 1:
							return TRUE;

							case 2:
//							return BaseDialogSettingSimulateAccount::PreTranslateMessage(pMsg);
							return CDialog::PreTranslateMessage(pMsg);
						}
					}
//				}
				break;
			}
		}
		break;
	}
*/
	switch(pMsg->message)
	{
		case WM_KEYDOWN:
		switch(ProcessKeyInSearch(pMsg))
		{
			case 1:
			return TRUE;

			case 2:
			return CDialog::PreTranslateMessage(pMsg);
		}
		break;
	}
	return BaseDialogSettingSimulateAccount::PreTranslateMessage(pMsg);
}

unsigned char SampleDialogSettingSimulateAccountRisk::ProcessKeyInSearch(MSG* pMsg)
{
	HWND focus = ::GetFocus();
	if(focus == m_EditSearch.m_hWnd || focus == m_ButtonSearch.m_hWnd)
	{
		unsigned char modifier = UGUI_GetKeyModifier();
		if(!(modifier & (KM_SHIFT_LEFT|KM_CTRL_LEFT|KM_ALT_LEFT|KM_CTRL_RIGHT|KM_ALT_RIGHT)))
		{
			if(pMsg->wParam == VK_RETURN)
			{
				SearchItems();
				return 1;//TRUE;
			}
			if(pMsg->wParam >= 'A' && pMsg->wParam <= 'Z')// || pMsg->wParam == ' ')
			{
				return 2;//CDialog::PreTranslateMessage(pMsg);
			}
			if(pMsg->wParam == VK_BACK || pMsg->wParam == VK_DELETE || pMsg->wParam == ' ')
			{
				if(m_EditSearch.GetWindowTextLength())
				{
					return 2;//CDialog::PreTranslateMessage(pMsg);
				}
			}
		}
	}
	return 0;
}


const Message* SampleDialogSettingSimulateAccountRisk::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
		if(m_account)
		{
			U_CopyAndPad(m_messageAccount.AccountName, sizeof(m_messageAccount.AccountName), m_accountId, '\0');

			const ConstraintItemMoney* moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_BUYING_POWER);
			const Money bp = moneyItem->isChecked() ? moneyItem->GetCustomValue() : m_account->GetBuyingPower();
			const Money bp1 = moneyItem->GetValueOrCustomValue();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_HARD_BUYING_POWER);
			const Money hbp = moneyItem->isChecked() ? moneyItem->GetCustomValue() : m_account->GetHardBuyingPower();

			m_messageAccount.BuyingPower = hbp.toDouble();

			m_messageAccount.ExecutionEntitlementsLow = 0;
			m_messageAccount.ExecutionEntitlementsHigh = 0;
			m_messageAccount.SecurityEntitlements =
#ifdef USE_EXECUTION_ENTITLEMENTS
				m_account->GetExchangeEntitlements();
#else
				0xFFFFFFFF;
#endif
			m_messageAccount.TradingAllowed = m_account->isTradingAllowed();

			TMsgAccountRiskProfile& messageRiskProfile = m_messageAccount.AccountRiskProfile;

			U_CopyAndPad(messageRiskProfile.UserName, sizeof(messageRiskProfile.UserName), m_account->GetUserName(), '\0');
//			U_CopyAndPad(messageRiskProfile.AccountName, sizeof(messageRiskProfile.AccountName), m_account->GetId(), '\0');
			U_CopyAndPad(messageRiskProfile.AccountName, sizeof(messageRiskProfile.AccountName), m_accountId, '\0');
			messageRiskProfile.AccountType = m_account->GetAccountType();
			U_CopyAndPad(messageRiskProfile.ClearingAccountName, sizeof(messageRiskProfile.ClearingAccountName), m_account->GetClearingAccountName(), '\0');

			messageRiskProfile.DisplayMultiplier_LSTK =
#ifdef BORROW_PRICE_DISCOUNT
				m_account->GetBorrowPriceMultiplier().toDouble();
#else
				1.0;
#endif
			messageRiskProfile.EnforceClearingAccountAggregation = m_account->isPositionAggregation();
//			messageRiskProfile.EstBeginningDayEquity = m_account->GetEstimatedBeginningEquity().toDouble();

			U_CopyAndPad(messageRiskProfile.FirmID, sizeof(messageRiskProfile.FirmID), m_account->GetStockLoanFirmId(), '\0');
			messageRiskProfile.FirmType = m_account->GetFirmType();

			messageRiskProfile.AcctProcessingFlags = 0;//m_account->

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_AWAY_FROM_NBBO_CHEAP_PERCENT);

			messageRiskProfile.PercentAwayFromNbboCheap = //0;// DailyPLLimit = 0;
				moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				(double)m_account->GetAwayFromNbboCheapPercent();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_AWAY_FROM_NBBO_PERCENT);
			messageRiskProfile.PercentAwayFromNbbo = //0;// InitialMemberContribution = 0;
				moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble() :
				(double)m_account->GetAwayFromNbboPercent();

			messageRiskProfile.FixedBP = 0;
//			messageRiskProfile.MaxLoss = 0;
			messageRiskProfile.Reserved1 = 0;

			messageRiskProfile.MaxSharePrice = 0;
			messageRiskProfile.MinEquityAmountAllowed = 0;
			
//			*(unsigned int*)messageRiskProfile.GroupID_LSTK = m_account->GetStockLoanFirmIdNum();
			*(unsigned int*)messageRiskProfile.GroupID_LSTK = m_account->GetFirmIdNum();
			messageRiskProfile.GroupID_LSTK[sizeof(messageRiskProfile.GroupID_LSTK) - 1] = '\0';
			
			unsigned __int64 hbpNumber = hbp.GetDollars() * hbp.divider + hbp.GetDollarFraction();
			if(hbpNumber)
			{
				unsigned __int64 bpNumber = (bp.GetDollars() * bp.divider + bp.GetDollarFraction());
				Money m(bpNumber, 0);
				m /= hbpNumber;
				messageRiskProfile.IntradayEquityMultiplier = (double)(unsigned int)m.GetDollars() + (double)m.GetDollarFraction() / (double)bp.divider;
			}
			else
			{
				messageRiskProfile.IntradayEquityMultiplier = 0;
			}

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_DAY_LONG_INVESTMENT);
			messageRiskProfile.IntradayMaxLongValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
//				m_account->GetDayMaxLongInvestmentConstraint().GetCustomValue().toDouble();
				m_account->GetDayMaxLongInvestment().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_DAY_SHORT_INVESTMENT);
			messageRiskProfile.IntradayMaxShortValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetDayMaxShortInvestment().toDouble();

			const ConstraintItemUInt* uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_DAY_POS_SHARES);
			messageRiskProfile.IntradayMaxPosShares = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetDayMaxPosShares();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_NIGHT_POS_SHARES);
			messageRiskProfile.OvernightMaxPosShares = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetNightMaxPosShares();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_DAY_POS_INVESTMENT);
			messageRiskProfile.IntradayMaxPosValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetDayMaxPosInvestment().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_NIGHT_POS_INVESTMENT);
			messageRiskProfile.OvernightMaxPosValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetNightMaxPosInvestment().toDouble();

			const ConstraintItemBool* boolItem = (const ConstraintItemBool*)m_ListBoxConstraint.GetConstraintItem(CI_ISO_ALLOWED);
			messageRiskProfile.ISOAllowed = boolItem->isChecked() ?
				boolItem->GetCustomValue() != 0:
				m_account->GetIsoAllowed() != 0;

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_LOSS);
			messageRiskProfile.MaxLossMarked = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetCtrnMaxLoss().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_LOSS_SLACK);
			messageRiskProfile.MaxLossPerAccountSlack = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxLossSlack().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_LOSS_CLOSE);
			messageRiskProfile.MaxLossPerAccountClose = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetCtrnMaxLossClose().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_LOSS_PER_POSITION);
			messageRiskProfile.MaxLossPerPosition = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxLossPerPosition().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_LOSS_PER_POSITION_SLACK);
			messageRiskProfile.MaxLossPerPositionSlack = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxLossPerPositionSlack().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_LOSS_PER_POSITION_CLOSE);
			messageRiskProfile.MaxLossPerPositionClose = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxLossPerPositionClose().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_OPEN_LOSS_PER_POSITION);
			messageRiskProfile.MaxOpenLossPerPosition = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxOpenLossPerPosition().toDouble();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_OPEN_POSITIONS);
			messageRiskProfile.MaxOpenPositions = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMaxOpenPositions();

#ifndef TAKION_NO_OPTIONS
			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_OPTION_CONTRACTS_OPEN);
			messageRiskProfile.MaxOptionContractsOpen = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMaxOptionContractsOpen();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_OPTION_OPEN_POSITIONS);
			messageRiskProfile.MaxOptionOpenPositions = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMaxOptionOpenPositions();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_OPTION_ORDER_SIZE);
			messageRiskProfile.MaxOptionOrderSize = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMaxOptionOrderSize();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_OPTION_ORDER_VALUE);
			messageRiskProfile.MaxOptionOrderValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxOptionOrderValue().toDouble();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_OPTION_SHARES_TRADED);
			messageRiskProfile.MaxOptionSharesTraded = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMaxOptionSharesTraded();

			boolItem = (const ConstraintItemBool*)m_ListBoxConstraint.GetConstraintItem(CI_OPTION_NAKED);
			messageRiskProfile.NakedOptionSellAllowed = boolItem->isChecked() ?
				boolItem->GetCustomValue() != 0:
				m_account->GetOptionNaked() != 0;

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_OPTION_BUYING_POWER);
			messageRiskProfile.OptionBuyingPower = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetOptionBuyingPower().toDouble();
/////////
			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURE_BUYING_POWER);
			messageRiskProfile.FuturesBuyingPower = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetFuturesBuyingPower().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURE_ESTIMATED_BEGINNING_EQUITY);
			messageRiskProfile.FuturesEstBeg = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetFuturesEstimatedBeginningEquity().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURE_ORDER_VALUE);
			messageRiskProfile.MaxFutureOrderValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble() :
				m_account->GetFuturesMaxOrderValue().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURE_LOSS);
			messageRiskProfile.MaxFuturesLoss = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetFuturesMaxLoss().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURE_LOSS_CLOSE);
			messageRiskProfile.MaxFuturesLossClose = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetFuturesMaxLossClose().toDouble();
/*
			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURE_LOSS_PER_POSITION);
			messageRiskProfile.MaxFuturesLossPerPosition = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxLossPerPosition().toDouble();
*/
			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURE_LOSS_PER_POSITION_CLOSE);
			messageRiskProfile.PosFuturesLossClose = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetFuturesMaxPosLossClose().toDouble();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURES_OPEN_POSITIONS);
			messageRiskProfile.MaxFuturesPos = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetFuturesMaxOpenPositions();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURES_ORDER_SIZE);
			messageRiskProfile.MaxFutureOrderSize = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetFuturesMaxOrderSize();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURES_CONTRACTS_OPEN);
			messageRiskProfile.MaxFuturesContractsOpen = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetFuturesMaxContractsOpen();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURES_CONTRACTS_TRADED);
			messageRiskProfile.MaxFuturesContractsTraded = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetFuturesMaxContractsTraded();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURES_EST_BEG_PERCENT_LOSS);
			messageRiskProfile.MaxFuturesLoss = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetFuturesEstBegPercentMaxLoss();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_FUTURES_EST_BEG_PERCENT_LOSS_CLOSE);
			messageRiskProfile.MaxFuturesLossClose = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetFuturesEstBegPercentMaxLossClose();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_OPTION_BLOCK_AGGR_PRICE_PERCENT);
			messageRiskProfile.OptionBlockAggrPricePercent = uintItem->isChecked() ?
				uintItem->GetCustomValue() :
				m_account->GetOptionBlockAggrPricePercent();

//			if(GetConstraintItem(CI_FUTURES_COMMISSION_FEE)->SetValue(constraintCollection.GetCommissionPer1FutureContractConstraint()))InvalidateItem(CI_FUTURES_COMMISSION_FEE);
#endif

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_ORDER_SIZE);
			messageRiskProfile.MaxOrderSize = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMaxOrderSize();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_ORDER_VALUE);
			messageRiskProfile.MaxOrderValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxOrderValue().toDouble();
/*
			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_COMMISSION_FEE);
			messageRiskProfile.MaxOrderValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxOrderValue().toDouble();
*/
			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_POS_PENDING_ORDERS);
			messageRiskProfile.MaxPendingOrdersPerPosition = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMaxPosPendingOrder();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_SHARES_TOTAL);
			messageRiskProfile.MaxSharesTotal = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMaxSharesTotal();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_SHARES_TRADED);
			messageRiskProfile.MaxSharesTraded = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMaxSharesTraded();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_TRADED_MONEY);
			messageRiskProfile.MaxTradedMoney = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMaxTradedMoney().toDouble();

			const ConstraintItemPrice* priceItem = (const ConstraintItemPrice*)m_ListBoxConstraint.GetConstraintItem(CI_SHORT_PRICE);
			messageRiskProfile.MinShortPrice = priceItem->isChecked() ?
				priceItem->GetCustomValue().toDouble():
				m_account->GetMinShortPrice().toDouble();

			messageRiskProfile.OptionVenue =
#ifndef TAKION_NO_OPTIONS
				m_account->GetOptionVenue();
#else
				0;
#endif
			messageRiskProfile.OrderProcessingFlags = m_account->GetOrderProcessingFlags();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_NIGHT_LONG_INVESTMENT);
			messageRiskProfile.OvernightMaxLongValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetNightMaxLongInvestment().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_NIGHT_SHORT_INVESTMENT);
			messageRiskProfile.OvernightMaxShortValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetNightMaxShortInvestment().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_NIGHT_INVESTMENT);
			messageRiskProfile.OvernightMaxValue = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetNightMaxInvestment().toDouble();
/*
			priceItem = (const ConstraintItemPrice*)m_ListBoxConstraint.GetConstraintItem(CI_SPOOFING_BIG_PRICE_OFFSET);
			messageRiskProfile.SpoofingBigPriceOffset = priceItem->isChecked() ?
				priceItem->GetCustomValue().toDouble():
				m_account->GetSpoofingBigPriceOffset().toDouble();

			priceItem = (const ConstraintItemPrice*)m_ListBoxConstraint.GetConstraintItem(CI_SPOOFING_SMALL_PRICE_OFFSET);
			messageRiskProfile.SpoofingSmallPriceOffset = priceItem->isChecked() ?
				priceItem->GetCustomValue().toDouble():
				m_account->GetSpoofingSmallPriceOffset().toDouble();
*/
			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_SPOOFING_ORDER_COUNT_RATIO);
			messageRiskProfile.SpoofingOrderCountRatio = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetSpoofingOrderCountRatio();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_SPOOFING_ORDER_SIZE_RATIO);
			messageRiskProfile.SpoofingOrderSizeRatio = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetSpoofingOrderSizeRatio();

			priceItem = (const ConstraintItemPrice*)m_ListBoxConstraint.GetConstraintItem(CI_MIN_SHARE_PRICE);
			messageRiskProfile.MinSharePrice = priceItem->isChecked() ?
				priceItem->GetCustomValue().toDouble():
				m_account->GetMinSharePrice().toDouble();

			priceItem = (const ConstraintItemPrice*)m_ListBoxConstraint.GetConstraintItem(CI_MAX_SHARE_PRICE);
			messageRiskProfile.MaxSharePrice = priceItem->isChecked() ?
				priceItem->GetCustomValue().toDouble():
				m_account->GetMaxSharePrice().toDouble();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_ADV_ORDER_COUNT);
			messageRiskProfile.ADVOrderCount = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetAdvOrderCount();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_ADV_ORDER_SIZE_RATIO);
			messageRiskProfile.ADVOrderSizePercent = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetAdvOrderSizePercent();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_ADV_BORDER);
			messageRiskProfile.ADVValue = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetAdvBorder();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_ADV_FORCE_HIDDEN);
			messageRiskProfile.ForceHiddenBelowADV = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetAdvForceHidden();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_ADV_REJECT);
			messageRiskProfile.RejectBelowADV = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetAdvReject();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_ADV_FORCE_HIDDEN_CLOSE);
			messageRiskProfile.ForceHiddenCloseBelowADV = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetAdvForceHiddenClose();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_BLOCK_AGGR_PRICE_PERCENT);
			messageRiskProfile.BlockAggrPricePercent = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetBlockAggrPricePercent();

			const ConstraintItemUInt64* uintItem64 = (const ConstraintItemUInt64*)m_ListBoxConstraint.GetConstraintItem(CI_TIME_FRAME_VENUES);
			messageRiskProfile.MaxOrderVenues = uintItem64->isChecked() ?
				(unsigned int)(uintItem64->GetCustomValue() & 0xFFFFFFFF):
				(unsigned int)(m_account->GetTimeFrameVenues() & 0xFFFFFFFF);

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_ORDER_TIME_FRAME);
			const unsigned int orderTimeFrame = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetOrderTimeFrame();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_TIME_FRAME_ORDER_COUNT);
			const unsigned int timeFrameOrderCount = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetTimeFrameOrderCount();

			messageRiskProfile.MaxOrderVenuesTime = orderTimeFrame | (timeFrameOrderCount << 16);

			uintItem64 = (const ConstraintItemUInt64*)m_ListBoxConstraint.GetConstraintItem(CI_ORDER_SIZE_NBBO_VENUES);
			messageRiskProfile.MaxOrderSizeVenues = uintItem64->isChecked() ?
				(unsigned int)(uintItem64->GetCustomValue() & 0xFFFFFFFF):
				(unsigned int)(m_account->GetOrderSizeNbboVenues() & 0xFFFFFFFF);

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_ORDER_SIZE_NBBO_MULTIPLIER);
			messageRiskProfile.MaxOrderSizeVenuesMultiplier = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetOrderSizeNbboMultiplier();

			boolItem = (const ConstraintItemBool*)m_ListBoxConstraint.GetConstraintItem(CI_HTBT_ALLOWED);
			messageRiskProfile.HTBThreshold = boolItem->isChecked() ?
				boolItem->GetCustomValue() != 0:
				m_account->GetHtbTborrowOrdersAllowed() != 0;

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_MORNING_POS_INVESTMENT);
			messageRiskProfile.MorningPosInv = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMorningMaxPosInvestment().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_MORNING_LONG_INVESTMENT);
			messageRiskProfile.MorningLongInv = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMorningMaxLongInvestment().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_MORNING_SHORT_INVESTMENT);
			messageRiskProfile.MorningShortInv = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMorningMaxShortInvestment().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_MORNING_INVESTMENT);
			messageRiskProfile.MorningMaxTotalInvestment = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMorningMaxInvestment().toDouble();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_MORNING_POS_SHARES);
			messageRiskProfile.MorningPosShares = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetMorningMaxPosShares();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_MORNING_LOSS);
			messageRiskProfile.MorningMaxLoss = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetCtrnMorningMaxLoss().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_MORNING_LOSS_CLOSE);
			messageRiskProfile.MorningMaxLossExit = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetCtrnMorningMaxLossClose().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_MORNING_LOSS_PER_POSITION);
			messageRiskProfile.MorningMaxLossSymbol = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMorningMaxLossPerPosition().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_MORNING_LOSS_PER_POSITION_CLOSE);
			messageRiskProfile.MorningMaxLossSymbolClose = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetMorningMaxLossPerPositionClose().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_NIGHT_LOSS);
			messageRiskProfile.NiteMaxLoss = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetCtrnNightMaxLoss().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_NIGHT_LOSS_CLOSE);
			messageRiskProfile.NiteMaxLossExit = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetCtrnNightMaxLossClose().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_NIGHT_LOSS_PER_POSITION);
			messageRiskProfile.NiteMaxLossSymbol = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetNightMaxLossPerPosition().toDouble();

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_NIGHT_LOSS_PER_POSITION_CLOSE);
			messageRiskProfile.NiteMaxLossSymbolClose = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetNightMaxLossPerPositionClose().toDouble();

			boolItem = (const ConstraintItemBool*)m_ListBoxConstraint.GetConstraintItem(CI_CONSERVATIVE_ORDER_MARKING);
			messageRiskProfile.ConservativeOrderMarking = boolItem->isChecked() ?
				boolItem->GetCustomValue() != 0:
				m_account->GetConservativeOrderMarking() != 0;

			boolItem = (const ConstraintItemBool*)m_ListBoxConstraint.GetConstraintItem(CI_LOCAL_SIDE_MARKING);
			messageRiskProfile.LocalSideMarking = boolItem->isChecked() ?
				boolItem->GetCustomValue() != 0:
				m_account->GetLocalSideMarking() != 0;

//			CI_MARGIN_EXCESS,
//			CI_HARD_MARGIN_EXCESS,

			boolItem = (const ConstraintItemBool*)m_ListBoxConstraint.GetConstraintItem(CI_POOL_LOCATES);
			messageRiskProfile.PoolLocates = boolItem->isChecked() ?
				boolItem->GetCustomValue() != 0:
				m_account->GetPoolLocates() != 0;

			moneyItem = (const ConstraintItemMoney*)m_ListBoxConstraint.GetConstraintItem(CI_ESTIMATED_BEGINNING_EQUITY);
			messageRiskProfile.EstBeginningDayEquity = moneyItem->isChecked() ?
				moneyItem->GetCustomValue().toDouble():
				m_account->GetEstimatedBeginningEquity().toDouble();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_EST_BEG_PERCENT_LOSS);
			messageRiskProfile.EstBegPctCancelOpeningOrders = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetEstBegPercentMaxLoss();

			uintItem = (const ConstraintItemUInt*)m_ListBoxConstraint.GetConstraintItem(CI_EST_BEG_PERCENT_LOSS_CLOSE);
			messageRiskProfile.EstBegPctClosePositions = uintItem->isChecked() ?
				uintItem->GetCustomValue():
				m_account->GetEstBegPercentMaxLossClose();

			Money money;
			FillMoneyFromSpinControl(money, m_SpinAccountBalance, 100);
			messageRiskProfile.AccountBalance = money.toDouble();

			return &m_messageAccount;
		}
	}
	return nullptr;
}

HWND SampleDialogSettingSimulateAccountRisk::GetFirstTabControl() const
{
	return m_ListBoxConstraint.GetHwnd();
}

void SampleDialogSettingSimulateAccountRisk::Resize(bool widthChanged, bool heightChanged)
{
	BaseDialogSettingSimulateAccount::Resize(widthChanged, heightChanged);
	if(m_ListBoxConstraint.GetWnd().m_hWnd)
	{
		if(!m_topOffset)
		{
			CRect rect;
//			m_ListBoxConstraint.GetWindowRect(&rect);
			::GetWindowRect(m_ListBoxConstraint.GetHwnd(), &rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			::GetClientRect(m_ListBoxConstraint.GetHwnd(), &clientRect);
			m_listBoxBorderSize = (rect.Width() - clientRect.Width()) / 2;
		}
		int bottom = m_clientRect.bottom;// - m_bottomOffset;
		::SetWindowPos(m_ListBoxConstraint.GetHwnd(), nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
	}
}

bool SampleDialogSettingSimulateAccountRisk::isAccountConstraintModified(const AccountConstraint* ac) const
{
	if(!ac)
	{
		return false;
	}
	return m_ListBoxConstraint.isModified();
}

bool SampleDialogSettingSimulateAccountRisk::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateAccount::isSendMessageButtonEnabled() && TD_IsSimulation();
}

void SampleDialogSettingSimulateAccountRisk::BeforeDestroy()
{
//	TD_GetNewAccountObservable().RemoveObserver(this);
//	TD_GetAdminObservable().RemoveObserver(this);
	m_searchStr = "";
	m_searchDone = true;
	BaseDialogSettingSimulateAccount::BeforeDestroy();
}


BOOL SampleDialogSettingSimulateAccountRisk::OnInitDialog()
{
	m_searchStr = "";
	m_searchDone = true;

	BOOL ret = BaseDialogSettingSimulateAccount::OnInitDialog();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateAccountRisk::UpdateSettings()
{
	BaseDialogSettingSimulateAccount::UpdateSettings();
	UpdateAccountInConfig(extensionConfig->m_simAcctRiskAccountId);
}

void SampleDialogSettingSimulateAccountRisk::RestoreLayout()
{
	BaseDialogSettingSimulateAccount::RestoreLayout();
	SetAccountById(extensionConfig->m_simAcctRiskAccountId.GetValue().c_str());

	EnableSendMessageButton();

	m_modified = false;
}
/*
void SampleDialogSettingSimulateAccountRisk::SettingPageDestroyed()
{
	BaseDialogSettingSimulateAccount::SettingPageDestroyed();
	m_searchStr = "";
	m_searchDone = true;
}
*/
void SampleDialogSettingSimulateAccountRisk::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateAccount::DoDataExchange(pDX);
//	DDX_Control(pDX, comboBoxRiskAccountId, m_ComboBoxAccount);
	DDX_Control(pDX, editSearchConstraintId, m_EditSearch);
	DDX_Control(pDX, buttonSearchConstraintId, m_ButtonSearch);
	DDX_Control(pDX, buttonNullifyConstraintId, m_ButtonNullify);
	DDX_Control(pDX, buttonResetConstraintId, m_ButtonResetConstraint);
	DDX_Control(pDX, spinConstraintAccountBalanceId, m_SpinAccountBalance);
	DDX_Control(pDX, listBoxConstraintId, m_ListBoxConstraint.GetWnd());
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateAccountRisk, BaseDialogSettingSimulateAccount)
//	ON_CBN_SELCHANGE(comboBoxRiskAccountId, OnSelchangeAccount)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editSearchConstraintId, OnSearchChange)
	ON_BN_CLICKED(buttonSearchConstraintId, OnSearch)
	ON_BN_CLICKED(buttonNullifyConstraintId, OnNullify)
	ON_BN_CLICKED(buttonResetConstraintId, OnResetConstraint)
	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)
	ON_MESSAGE(WM_USER + 23, OnListBoxValueSpinned)
END_MESSAGE_MAP()

LRESULT SampleDialogSettingSimulateAccountRisk::OnListBoxChecked(WPARAM controlId, LPARAM l)
{
	switch(controlId)
	{
		case listBoxConstraintId:
		EnableResetCustomButton();
		break;
	}
	return 0;
}

LRESULT SampleDialogSettingSimulateAccountRisk::OnListBoxValueSpinned(WPARAM controlId, LPARAM l)
{
	switch(controlId)
	{
		case listBoxConstraintId:
		EnableResetCustomButton();
		break;
	}
	return 0;
}
void SampleDialogSettingSimulateAccountRisk::EnableResetCustomButton()
{
	bool enable = !m_ListBoxConstraint.isEqualConstraintCollection(&m_zeroAccountConstraint)
		|| m_SpinAccountBalance.GetValue() != 0;
	EnableControl(m_ButtonNullify.m_hWnd, enable, m_ListBoxConstraint.GetWnd().m_hWnd);

	enable = !m_ListBoxConstraint.isEqualConstraintCollection(m_account);
	if(!enable)
	{
		Money money;
		FillMoneyFromSpinControl(money, m_SpinAccountBalance, 100);
		if(m_account)
		{
			if(m_account->GetAccountBalance() != money)
			enable = true;
		}
		else if(!money.isZero())
		{
			enable = true;
		}
	}
	EnableControl(m_ButtonResetConstraint.m_hWnd, enable, m_ListBoxConstraint.GetWnd().m_hWnd);
}

void SampleDialogSettingSimulateAccountRisk::OnNullify()
{
	bool changed = !m_ListBoxConstraint.isEqualConstraintCollection(&m_zeroAccountConstraint);
	if(changed)
	{
		m_ListBoxConstraint.SetValues(m_zeroAccountConstraint);
		EnableResetCustomButton();
	}
	if(m_SpinAccountBalance.GetValue() != 0)
	{
		changed = true;
		m_SpinAccountBalance.SetValue(0, false, true, false);
	}
	if(changed)
	{
		EnableResetCustomButton();
	}
}

void SampleDialogSettingSimulateAccountRisk::OnResetConstraint()
{
	bool changed = !m_ListBoxConstraint.isEqualConstraintCollection(m_account);
	if(changed)
	{
		m_ListBoxConstraint.SetValues(m_account);
	}
	Money money;
	FillMoneyFromSpinControl(money, m_SpinAccountBalance, 100);
	if(m_account)
	{
		if(m_account->GetAccountBalance() != money)
		{
			changed = true;
			FillSpinControlFromMoney(m_account->GetAccountBalance(), m_SpinAccountBalance, 100, false);
		}
	}
	else if(!money.isZero())
	{
		changed = true;
		FillSpinControlFromMoney(Money::moneyZero, m_SpinAccountBalance, 100, false);
	}
	if(changed)
	{
		EnableResetCustomButton();
	}
}

HBRUSH SampleDialogSettingSimulateAccountRisk::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSearchConstraintId:
		if(!m_searchDone)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;

	}
	return BaseDialogSettingSimulateAccount::OnCtlColor(pDC, pWnd, nCtlColor);
}

void SampleDialogSettingSimulateAccountRisk::OnSearch()
{
	if(!m_searchDone)
	{
		SearchItems();
	}
}

void SampleDialogSettingSimulateAccountRisk::OnSearchChange()
{
	SearchChangeStatic(m_EditSearch, m_searchStr, m_ButtonSearch, m_searchDone);
}



void SampleDialogSettingSimulateAccountRisk::UpdateControls()
{
	BaseDialogSettingSimulateAccount::UpdateControls();
	if(m_account)
	{
//		m_ListBoxConstraint.SetItem(CI_COMMISSION_FEE, &((Account*)m_account)->GetCommissionPer1000SharesConstraint(), true);
	}
}

void SampleDialogSettingSimulateAccountRisk::AccountChanged()
{
	SetItems(true);
	EnableSendMessageButton();
}

/*
void SampleDialogSettingSimulateAccountRisk::CurrentAccountSet(const Account* account)
{
	if(m_ComboBoxAccount.m_hWnd && m_accountSel <= 0)//m_ComboBoxAccount.GetCurSel() <= 0)
	{
		if(m_currentAccount != account)
		{
			m_currentAccount = (Account*)account;
			AccountSelected();
		}
	}
}

void SampleDialogSettingSimulateAccountRisk::AccountAdded(const Account* account)
{
	if(m_ComboBoxAccount.FindItem(account) < 0)
	{
		int sel = m_ComboBoxAccount.AddString((const char*)account);
		if(*m_accountId)
		{
			if(!strcmp(m_accountId, account->GetId()))
			{
				m_ComboBoxAccount.SetCurSel(sel);
				OnSelchangeAccount();
//			SetAccount(account);
//				SetItems(false);
			}
		}
	}
}

void SampleDialogSettingSimulateAccountRisk::CurrentAccountSet(const Account* account)
{
	if(m_ComboBoxAccount.m_hWnd && m_accountSel <= 0)//m_ComboBoxAccount.GetCurSel() <= 0)
	{
//		SetAccount(account);
		if(m_currentAccount != account)
		{
			m_currentAccount = (Account*)account;
			AccountSelected();
		}
	}
}

void SampleDialogSettingSimulateAccountRisk::OnSelchangeAccount()
{
	int sel = m_ComboBoxAccount.GetCurSel();
	if(m_accountSel != sel)
	{
		m_accountSel = sel;
		m_modified = true;
		Account* account = sel > 0 ? (Account*)m_ComboBoxAccount.GetItemDataPtr(sel) : TD_GetCurrentAccount();
		if(m_currentAccount != account)
		{
			m_currentAccount = account;
			AccountSelected();
		}
	}
	else if(!m_accountSel)
	{
		Account* account = TD_GetCurrentAccount();
		if(m_currentAccount != account)
		{
			m_currentAccount = account;
			AccountSelected();
		}
	}
}

void SampleDialogSettingSimulateAccountRisk::SetHistoryLoaded(const bool loaded)
{
	if(loaded != m_historyLoaded)
	{
		m_historyLoaded = loaded;
	}
}

void SampleDialogSettingSimulateAccountRisk::SetExecutorConnected(const bool connected)
{
	if(connected != m_executorConnected)
	{
		m_executorConnected = connected;
		if(!m_executorConnected)
		{
			SetHistoryLoaded(false);
			EnableSendMessageButton();
		}
	}
}

void SampleDialogSettingSimulateAccountRisk::AccountSelected()
{
	if(m_currentAccount)
	{
		U_CopyAndPad(m_accountId, sizeof(m_accountId), m_currentAccount->GetId(), '\0');
//		m_currentAccount->AddObserver(this);
		SetHistoryLoaded(m_currentAccount->isHistoryLoaded());
	}
	else
	{
		U_CopyAndPad(m_accountId, sizeof(m_accountId), "", '\0');
		SetHistoryLoaded(false);
	}
	SetItems(true);
	EnableSendMessageButton();
}
*/
void SampleDialogSettingSimulateAccountRisk::SetItems(bool reset)
{
	m_ListBoxConstraint.SetConstraintCollection(m_account);
	FillSpinControlFromMoney(m_account ? m_account->GetAccountBalance() : Money::moneyZero, m_SpinAccountBalance, 100, false);
	EnableResetCustomButton();
}
/*
void SampleDialogSettingSimulateAccountRisk::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TS_NEW_ACCOUNT:
		AccountAdded(((TMsgNewAccountObject*)info)->m_account);
		break;

		case TM_NEW_ACCOUNT:
		AccountAdded(((TMsgNewAccountObject*)message)->m_account);
		break;

		case TM_UPDATED_ACCOUNT://TM_ACCOUNT_UPDATE:
//		UpdateAccount(((TMsgAccount*)message)->m_account);
		break;

		case TM_ACCOUNT_DELETE:
		{
			Account* account = ((const TMsgAccountDelete*)message)->m_account;
//			ClearAccountOrders(account);
			int found = m_ComboBoxAccount.FindItem(account);
			if(found >= 0)
			{
				const int sel = m_ComboBoxAccount.GetCurSel();
				m_ComboBoxAccount.DeleteString(found);
				if(sel >= 0)
				{
					const unsigned int count = m_ComboBoxAccount.GetItemCount();
					if(count > 0)
					{
						if((unsigned int)sel >= count)
						{
							m_ComboBoxAccount.SetCurSel(count - 1);
						}
					}
				}
				OnSelchangeAccount();
			}
		}
		break;

		case TM_DELETE_ALL_ACCOUNTS:
//		ClearStocks();
		m_ComboBoxAccount.ResetContent();
		m_ComboBoxAccount.AddString(nullptr);
		m_ComboBoxAccount.SetCurSel(0);
		OnSelchangeAccount();
		break;

		case M_RESP_MD_LOGON:
//		UpdateMarketDataEntitlements();
		break;

		case TS_LOGIN_RESP:
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			AccountAdded(msg->m_account);
		}
		EnableSendMessageButton();
		break;

		case TS_ACCOUNT_POSITION_RESP:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)//History Loaded
		{
			if(m_currentAccount == ((TMsgUpdatedAccount*)info)->m_account)
			{
				SetHistoryLoaded(true);
			}
		}
		break;

		case TS_ACCOUNT_EXEC_HISTORY_COMPLETE:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			if(m_currentAccount == ((TMsgUpdatedAccount*)info)->m_account)
			{
			}
		}
		break;

		case TS_ACCOUNT_ORDER_HISTORY_COMPLETE:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			if(m_currentAccount == ((TMsgUpdatedAccount*)info)->m_account)
			{
			}
		}
		break;

		case TM_MARKET_STATUS:
		{
		}
		break;

		case TM_NEXT_DAY_STARTED:
		{
//			NextDayStarted(((const TMsgNextDayStarted*)message)->m_date);
		}
		break;

		case TM_CONNECTED:
		case TM_DISCONNECTED:
		SetExecutorConnected(TD_IsExecutorConnected());
		break;
	}
}
*/
