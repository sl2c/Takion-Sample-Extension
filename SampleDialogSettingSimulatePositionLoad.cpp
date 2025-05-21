#include "stdafx.h"
#include "SampleDialogSettingSimulatePositionLoad.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulatePositionLoad::SampleDialogSettingSimulatePositionLoad(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateAccountOption(mainWnd, parentTab, "Load Position"),
	m_SpinSize(999999, -999999, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinCost(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinOvernightSize(999999, -999999, 0, 1000, 3, TakionSpin::charComma),
	m_SpinOvernightPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinOpenOvernightSize(999999, -999999, 0, 1000, 3, TakionSpin::charComma),
	m_SpinClsLongSize(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinClsLongCost(999999999999, 0, 0, 100000, 3, TakionSpin::charComma, 2),
	m_SpinClsLongValue(999999999999, 0, 0, 100000, 3, TakionSpin::charComma, 2),
	m_SpinClsShortSize(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinClsShortCost(999999999999, 0, 0, 100000, 3, TakionSpin::charComma, 2),
	m_SpinClsShortValue(999999999999, 0, 0, 100000, 3, TakionSpin::charComma, 2),
	m_SpinBorrowSize(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinBorrowPrice(999999, 0, 0, 10000, 3, TakionSpin::charComma, 4)
#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	,m_SpinSendMessageToAllAccountsDelay(1000)
	,m_millisecondDelay(0)
#endif
{
#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	U_CopyAndPad(m_accountIdBeingSent, sizeof(m_accountIdBeingSent), "", '\0');
#endif
	m_EditSymbol.AddValidChar(';', false);
	SetSignedSizeSpinCells(m_SpinSize);
	SetPriceSpinCells(m_SpinPrice);
	SetPriceSpinCells(m_SpinCost);

	SetSignedSizeSpinCells(m_SpinOvernightSize);
	SetPriceSpinCells(m_SpinOvernightPrice);
	SetSignedSizeSpinCells(m_SpinOpenOvernightSize);

	SetSizeSpinCells(m_SpinClsLongSize);
	SetMoneySpinCells(m_SpinClsLongCost);
	SetMoneySpinCells(m_SpinClsLongValue);

	SetSizeSpinCells(m_SpinClsShortSize);
	SetMoneySpinCells(m_SpinClsShortCost);
	SetMoneySpinCells(m_SpinClsShortValue);

	SetSizeSpinCells(m_SpinBorrowSize);
	m_SpinBorrowPrice.AddSpinCell(1, false);
	m_SpinBorrowPrice.AddSpinCell(100, false);
	m_SpinBorrowPrice.AddSpinCell(10000, true);

	const int doubleWidth = m_firstColumnWidth + m_firstColumnWidth;
//	const int priceWidth = m_firstColumnWidth + spinHeight * 3 / 2;
	const int priceWidth = m_firstColumnWidth + spinHeight * 11 / 8;
	const int priceNameWidth = doubleWidth - priceWidth;
	const int moneyWidth = m_firstColumnWidth + spinHeight * 24 / 8 + 7;
	const int moneyNameWidth = doubleWidth - moneyWidth;

	m_bottomControl += vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinPosLoadSizeId);
	AddStaticIndirect("Size", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticPosLoadSizeId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinPosLoadPriceId);
	AddStaticIndirect("Price", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticPosLoadPriceId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinPosLoadCostId);
	AddStaticIndirect("Cost", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticPosLoadCostId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinPosLoadOvernightSizeId);
	AddStaticIndirect("OverniteSize", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticPosLoadOvernightSizeId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinPosLoadOvernightPriceId);
	AddStaticIndirect("OvnPrice", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticPosLoadOvernightPriceId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinPosLoadOpenOvernightSizeId);
	AddStaticIndirect("OpenOvnSize", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticPosLoadOpenOvernightSizeId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinPosLoadClsLongSizeId);
	AddStaticIndirect("LongSizeClosed", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticPosLoadClsLongSizeId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, moneyWidth, spinHeight, spinPosLoadClsLongCostId);
	AddStaticIndirect("LCost", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + moneyWidth, m_bottomControl + (spinHeight - staticHeight) / 2, moneyNameWidth, staticHeight, staticPosLoadClsLongCostId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, moneyWidth, spinHeight, spinPosLoadClsLongValueId);
	AddStaticIndirect("LVal", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + moneyWidth, m_bottomControl + (spinHeight - staticHeight) / 2, moneyNameWidth, staticHeight, staticPosLoadClsLongValueId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinPosLoadClsShortSizeId);
	AddStaticIndirect("ShortSizeClosed", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticPosLoadClsShortSizeId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, moneyWidth, spinHeight, spinPosLoadClsShortCostId);
	AddStaticIndirect("SCost", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + moneyWidth, m_bottomControl + (spinHeight - staticHeight) / 2, moneyNameWidth, staticHeight, staticPosLoadClsShortCostId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, moneyWidth, spinHeight, spinPosLoadClsShortValueId);
	AddStaticIndirect("SVal", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + moneyWidth, m_bottomControl + (spinHeight - staticHeight) / 2, moneyNameWidth, staticHeight, staticPosLoadClsShortValueId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinPosLoadBorrowSizeId);
	AddStaticIndirect("BorrowSize", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticPosLoadBorrowSizeId);

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinPosLoadBorrowPriceId);
	AddStaticIndirect("BorrowPrice", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticPosLoadBorrowPriceId);

#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	m_SpinSendMessageToAllAccountsDelay.AddSpinCell(1, false);
	m_SpinSendMessageToAllAccountsDelay.AddSpinCell(10, false);
	m_SpinSendMessageToAllAccountsDelay.AddSpinCell(100, true);

	const int x = m_firstColumnWidth + m_firstColumnWidth + hgap * 3;
	int y = vgap;
	AddButtonIndirect("Send Message to all Accounts", WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON | WS_TABSTOP, 0, x, y, m_sendButtonWidth * 2, buttonHeight, buttonPosLoadSendMessageToAllAccountsId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y += buttonHeight + vgap, m_firstColumnWidth, spinHeight, spinPosLoadSendMessageToAllAccountsDelayId);
	AddStaticIndirect("Delay", WS_VISIBLE | SS_NOPREFIX, 0, x + m_firstColumnWidth, y + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticPosLoadSendMessageToAllAccountsDelayId);
#endif
}
/*
void SampleDialogSettingSimulatePositionLoad::SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000, true);
}

void SampleDialogSettingSimulatePositionLoad::SetSignedSizeSpinCells(SignedNumberTypeSpin<int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000, true);
}

void SampleDialogSettingSimulatePositionLoad::SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}

void SampleDialogSettingSimulatePositionLoad::SetMoneySpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(100000000, true);
	SetPriceSpinCells(spin);
}
*/
void SampleDialogSettingSimulatePositionLoad::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateAccountOption::DoDataExchange(pDX);

	DDX_Control(pDX, spinPosLoadSizeId, m_SpinSize);
	DDX_Control(pDX, spinPosLoadPriceId, m_SpinPrice);
	DDX_Control(pDX, spinPosLoadCostId, m_SpinCost);

	DDX_Control(pDX, spinPosLoadOvernightSizeId, m_SpinOvernightSize);
	DDX_Control(pDX, spinPosLoadOvernightPriceId, m_SpinOvernightPrice);
	DDX_Control(pDX, spinPosLoadOpenOvernightSizeId, m_SpinOpenOvernightSize);

	DDX_Control(pDX, spinPosLoadClsLongSizeId, m_SpinClsLongSize);
	DDX_Control(pDX, spinPosLoadClsLongCostId, m_SpinClsLongCost);
	DDX_Control(pDX, spinPosLoadClsLongValueId, m_SpinClsLongValue);

	DDX_Control(pDX, spinPosLoadClsShortSizeId, m_SpinClsShortSize);
	DDX_Control(pDX, spinPosLoadClsShortCostId, m_SpinClsShortCost);
	DDX_Control(pDX, spinPosLoadClsShortValueId, m_SpinClsShortValue);

	DDX_Control(pDX, spinPosLoadBorrowSizeId, m_SpinBorrowSize);
	DDX_Control(pDX, spinPosLoadBorrowPriceId, m_SpinBorrowPrice);

#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	DDX_Control(pDX, buttonPosLoadSendMessageToAllAccountsId, m_ButtonSendMessageToAllAccounts);
	DDX_Control(pDX, spinPosLoadSendMessageToAllAccountsDelayId, m_SpinSendMessageToAllAccountsDelay);
#endif
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulatePositionLoad, BaseDialogSettingSimulateAccountOption)
#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	ON_WM_TIMER()
	ON_BN_CLICKED(buttonPosLoadSendMessageToAllAccountsId, OnSendMessageToAllAccounts)
#endif
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS

bool SampleDialogSettingSimulatePositionLoad::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateAccountOption::isSendMessageButtonEnabled() && !*m_accountIdBeingSent;
}

void SampleDialogSettingSimulatePositionLoad::OnSendMessageToAllAccounts()
{
	if(m_messageSimulationThread && isSendMessageButtonEnabled())
	{
		WSABUF wsabuf;
		const Message* message = CreateMessage(wsabuf);
		if(message)
		{
			m_millisecondDelay = m_SpinSendMessageToAllAccountsDelay.GetValue();;
			Account* account = nullptr;
			for(unsigned int i = 0, end = m_ComboBoxAccount.GetItemCount(); i < end; ++i)
			{
				account = (Account*)m_ComboBoxAccount.GetItemDataPtr(i);
				if(account)
				{
					U_CopyAndPad(m_accountIdBeingSent, sizeof(m_accountIdBeingSent), account->GetId(), '\0');
					U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName), m_accountIdBeingSent, '\0');
					m_messageSimulationThread->WriteMessage(&m_msg);
					if(m_millisecondDelay)
					{
						SetTimer(1, m_millisecondDelay, nullptr);
						EnableSendMessageButton();
						break;
					}
				}
			}
			if(!m_millisecondDelay)
			{
				U_CopyAndPad(m_accountIdBeingSent, sizeof(m_accountIdBeingSent), "", '\0');
				MessageSent();
			}
		}
	}
}

void SampleDialogSettingSimulatePositionLoad::OnTimer(UINT_PTR nIDEvent)
{
	SendMessageToNextAccount();
}

bool SampleDialogSettingSimulatePositionLoad::SendMessageToAccountAt(unsigned int i)
{
	Account* account = nullptr;
	const unsigned int end = m_ComboBoxAccount.GetItemCount();
	for(; i < end; ++i)
	{
		account = (Account*)m_ComboBoxAccount.GetItemDataPtr(i);
		if(account)
		{
			U_CopyAndPad(m_accountIdBeingSent, sizeof(m_accountIdBeingSent), account->GetId(), '\0');
			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName), m_accountIdBeingSent, '\0');
			m_messageSimulationThread->WriteMessage(&m_msg);
			return true;
		}
	}
	return false;
}

void SampleDialogSettingSimulatePositionLoad::SendMessageToNextAccount()
{
	if(*m_accountIdBeingSent)
	{
		const int i = m_ComboBoxAccount.FindItemByName(m_accountIdBeingSent);
		if(i >= 0)
		{
			if(SendMessageToAccountAt(i + 1))
			{
				return;
			}
		}
		U_CopyAndPad(m_accountIdBeingSent, sizeof(m_accountIdBeingSent), "", '\0');
	}
	m_millisecondDelay = 0;
	MessageSent();
	KillTimer(1);
	EnableSendMessageButton();
}
#endif

LRESULT SampleDialogSettingSimulatePositionLoad::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
		case spinPosLoadSendMessageToAllAccountsDelayId:
#endif
		case spinPosLoadSizeId:
		case spinPosLoadPriceId:
		case spinPosLoadCostId:

		case spinPosLoadOvernightSizeId:
		case spinPosLoadOvernightPriceId:
		case spinPosLoadOpenOvernightSizeId:

		case spinPosLoadClsLongSizeId:
		case spinPosLoadClsLongCostId:
		case spinPosLoadClsLongValueId:

		case spinPosLoadClsShortSizeId:
		case spinPosLoadClsShortCostId:
		case spinPosLoadClsShortValueId:

		case spinPosLoadBorrowSizeId:
		case spinPosLoadBorrowPriceId:

		m_modified = true;
		break;

#ifndef TAKION_NO_OPTIONS
		default:
		BaseDialogSettingSimulateAccountOption::OnSpinChange(w, l);
		break;
#endif
	}
	return 0;
}

const Message* SampleDialogSettingSimulatePositionLoad::CreateMessage(WSABUF& wsaBuf)
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

		if(m_optionSuffix)U_CopyPlusOneCharAndPad(m_msg.Symbol, sizeof(m_msg.Symbol), m_symbol.c_str(), m_optionSuffix, '\0');
		else U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol), m_symbol.c_str(), '\0');

		m_msg.m_optionBlock = optionKey.CalculateOptionBlock();

		m_msg.InstrumentType = 1;
	}
	else
	{
#endif
		U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol), m_symbol.c_str(), '\0');
#ifndef TAKION_NO_OPTIONS
		m_msg.InstrumentType = 0;
		m_msg.m_optionBlock = 0;
	}
#endif
	
	m_msg.m_time = TL_GetCurrentMillisecond();

	U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName), m_accountId, '\0');
	
	Price price;
	Money money;

	m_msg.CurrentPositionQuantity = m_SpinSize.GetValue();
	m_msg.Quantity = m_msg.CurrentPositionQuantity;
	FillPriceFromSpinControl(price, m_SpinPrice, 10000);
	m_msg.CurrentPositionAvgPrice = price.toDouble();
	m_msg.AveragePrice = m_msg.CurrentPositionAvgPrice;

	FillPriceFromSpinControl(price, m_SpinCost, 10000);
	m_msg.OvernightRealPositionPrice = price.toDouble();

	m_msg.OvernightPositionQuantity = m_SpinOvernightSize.GetValue();
	FillPriceFromSpinControl(price, m_SpinOvernightPrice, 10000);
	m_msg.OvernightPositionAvgPrice = price.toDouble();

	m_msg.OpenOvernightPositionQuantity = m_SpinOpenOvernightSize.GetValue();

	m_msg.ClosedLongPositionQuantity = m_SpinClsLongSize.GetValue();
	FillMoneyFromUnsignedSpinControl(money, m_SpinClsLongCost, 100);
	m_msg.ClosedLongPositionCost = money.toDouble();
	FillMoneyFromUnsignedSpinControl(money, m_SpinClsLongValue, 100);
	m_msg.ClosedLongPositionValue = money.toDouble();

	m_msg.ClosedShortPositionQuantity = m_SpinClsShortSize.GetValue();
	FillMoneyFromUnsignedSpinControl(money, m_SpinClsShortCost, 100);
	m_msg.ClosedShortPositionCost = money.toDouble();
	FillMoneyFromUnsignedSpinControl(money, m_SpinClsShortValue, 100);
	m_msg.ClosedShortPositionValue = money.toDouble();

	m_msg.m_borrowedQuantity = m_SpinBorrowSize.GetValue();
	FillPriceFromSpinControl(price, m_SpinBorrowPrice, 10000);
	m_msg.m_borrowedAvgPrice = price.toDouble();

//	m_msg.CurrentPositionAvgPriceMargin

	return &m_msg;
}

BOOL SampleDialogSettingSimulatePositionLoad::OnInitDialog()
{
#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	U_CopyAndPad(m_accountIdBeingSent, sizeof(m_accountIdBeingSent), "", '\0');
	m_millisecondDelay = 0;
#endif
	BOOL ret = BaseDialogSettingSimulateAccountOption::OnInitDialog();

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulatePositionLoad::UpdateSettings()
{
	extensionConfig->m_simPosLoadSymbol.SetValue(m_symbol);
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_simPosLoadOption.SetValue(m_optionMode);
	extensionConfig->m_simPosLoadOptionCall.SetValue(m_optionCall);
	extensionConfig->m_simPosLoadOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_simPosLoadOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_simPosLoadOptionExpiration.SetValue(GetExpirationDate());
#endif
	UpdateAccountInConfig(extensionConfig->m_simPosLoadAccountId);

	Price price;
	Money money;

	extensionConfig->m_simPosLoadSize.SetValue(m_SpinSize.GetValue());
	FillPriceFromSpinControl(price, m_SpinPrice, 10000);
	extensionConfig->m_simPosLoadPrice.SetValue(price);

	FillPriceFromSpinControl(price, m_SpinCost, 10000);
	extensionConfig->m_simPosLoadCost.SetValue(price);

	extensionConfig->m_simPosLoadOvernightSize.SetValue(m_SpinOvernightSize.GetValue());
	FillPriceFromSpinControl(price, m_SpinOvernightPrice, 10000);
	extensionConfig->m_simPosLoadOvernightPrice.SetValue(price);

	extensionConfig->m_simPosLoadOpenOvernightSize.SetValue(m_SpinOpenOvernightSize.GetValue());

	extensionConfig->m_simPosLoadClsLongSize.SetValue(m_SpinClsLongSize.GetValue());
	FillMoneyFromUnsignedSpinControl(money, m_SpinClsLongCost, 100);
	extensionConfig->m_simPosLoadClsLongCost.SetValue(money);
	FillMoneyFromUnsignedSpinControl(money, m_SpinClsLongValue, 100);
	extensionConfig->m_simPosLoadClsLongValue.SetValue(money);

	extensionConfig->m_simPosLoadClsShortSize.SetValue(m_SpinClsShortSize.GetValue());
	FillMoneyFromUnsignedSpinControl(money, m_SpinClsShortCost, 100);
	extensionConfig->m_simPosLoadClsShortCost.SetValue(money);
	FillMoneyFromUnsignedSpinControl(money, m_SpinClsShortValue, 100);
	extensionConfig->m_simPosLoadClsShortValue.SetValue(money);

	extensionConfig->m_simPosLoadBorrowSize.SetValue(m_SpinBorrowSize.GetValue());
	FillPriceFromSpinControl(price, m_SpinBorrowPrice, 10000);
	extensionConfig->m_simPosLoadBorrowPrice.SetValue(price);

#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	extensionConfig->m_simPosLoadSendMessageToAllAccountsDelay.SetValue(m_SpinSendMessageToAllAccountsDelay.GetValue());
#endif
}

void SampleDialogSettingSimulatePositionLoad::RestoreLayout()
{
	m_EditSymbol.SetWindowText(extensionConfig->m_simPosLoadSymbol.GetValue().c_str());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_simPosLoadOption.GetValue(),
		extensionConfig->m_simPosLoadOptionSuffix.GetValue(),
		extensionConfig->m_simPosLoadOptionCall.GetValue(),
		extensionConfig->m_simPosLoadOptionStrikePrice.GetValue(),
		extensionConfig->m_simPosLoadOptionExpiration.GetValue());
/*
	SetCallPut(extensionConfig->m_simPosLoadOptionCall.GetValue());
	
	int found = m_ComboBoxOptionSuffix.FindItem((const void*)extensionConfig->m_simPosLoadOptionSuffix.GetValue());
	m_ComboBoxOptionSuffix.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeOptionSuffix();

	SetStrikePrice(extensionConfig->m_simPosLoadOptionStrikePrice.GetValue());

	SetExpirationDate(extensionConfig->m_simPosLoadOptionExpiration.GetValue());

	m_CheckBoxOption.SetCheck(extensionConfig->m_simPosLoadOption.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnOption();
*/
#endif
	SetAccountById(extensionConfig->m_simPosLoadAccountId.GetValue().c_str());

	FillSpinControlFromInt(extensionConfig->m_simPosLoadSize.GetValue(), m_SpinSize, false);
	FillSpinControlFromPrice(extensionConfig->m_simPosLoadPrice.GetValue(), m_SpinPrice, 10000, false);
	FillSpinControlFromPrice(extensionConfig->m_simPosLoadCost.GetValue(), m_SpinCost, 10000, false);

	FillSpinControlFromInt(extensionConfig->m_simPosLoadOvernightSize.GetValue(), m_SpinOvernightSize, false);
	FillSpinControlFromPrice(extensionConfig->m_simPosLoadOvernightPrice.GetValue(), m_SpinOvernightPrice, 10000, false);

	FillSpinControlFromInt(extensionConfig->m_simPosLoadOpenOvernightSize.GetValue(), m_SpinOpenOvernightSize, false);

	FillSpinControlFromUInt(extensionConfig->m_simPosLoadClsLongSize.GetValue(), m_SpinClsLongSize, false);
	FillUnsignedSpinControlFromMoney(extensionConfig->m_simPosLoadClsLongCost.GetValue(), m_SpinClsLongCost, 100, false);
	FillUnsignedSpinControlFromMoney(extensionConfig->m_simPosLoadClsLongValue.GetValue(), m_SpinClsLongValue, 100, false);

	FillSpinControlFromUInt(extensionConfig->m_simPosLoadClsShortSize.GetValue(), m_SpinClsShortSize, false);
	FillUnsignedSpinControlFromMoney(extensionConfig->m_simPosLoadClsShortCost.GetValue(), m_SpinClsShortCost, 100, false);
	FillUnsignedSpinControlFromMoney(extensionConfig->m_simPosLoadClsShortValue.GetValue(), m_SpinClsShortValue, 100, false);

	FillSpinControlFromUInt(extensionConfig->m_simPosLoadBorrowSize.GetValue(), m_SpinBorrowSize, false);
	FillSpinControlFromPrice(extensionConfig->m_simPosLoadBorrowPrice.GetValue(), m_SpinBorrowPrice, 10000, false);

#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
//	m_millisecondDelay = extensionConfig->m_simPosLoadSendMessageToAllAccountsDelay.GetValue();
	FillSpinControlFromUInt(extensionConfig->m_simPosLoadSendMessageToAllAccountsDelay.GetValue(), m_SpinSendMessageToAllAccountsDelay, false);
#endif
	EnableSendMessageButton();

	m_modified = false;
}

