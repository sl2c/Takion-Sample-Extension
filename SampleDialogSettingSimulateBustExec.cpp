#include "stdafx.h"
#include "SampleDialogSettingSimulateBustExec.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateBustExec::SampleDialogSettingSimulateBustExec(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateAccountOption(mainWnd, parentTab, "Bust Exec"),
	m_SpinSize(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPrice(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_ComboBoxSide(false),
	m_SpinOrderSize(100000, 0, 0, 1000, 3, TakionSpin::charComma),
	m_EditMmid("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", false),
	m_EditLiquidity("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890", false),
	m_currentDestination(nullptr),
	m_ref(false),
	m_busted(false),
	m_dropcopy(false),
	m_side('B'),
	m_size(0),
	m_orderSize(0),
	m_msg(0, TD_GetNumericTraderId())
#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	, m_SpinSendMessageToAllAccountsDelay(1000)
	, m_millisecondDelay(0)
#endif
{
	m_ComboBoxDestination.SetItemToolTips(true);
	m_ComboBoxDestination.SetNoSelectionColor(m_visualBase->GetWarningColor());

#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	U_CopyAndPad(m_accountIdBeingSent, sizeof(m_accountIdBeingSent), "", '\0');
#endif
	m_msg.AccountName[sizeof(m_msg.AccountName) - 1] = '\0';
	m_msg.FirmID[sizeof(m_msg.FirmID) - 1] = '\0';
	m_msg.MMID[sizeof(m_msg.MMID) - 1] = '\0';
	m_msg.Ip = U_GetIpAddressNum();
	m_msg.ReasonText[0] = '\0';
	m_msg.Tracking = TR_ON_MARKET;

//	U_CopyAndPad(m_msg.TraderID, sizeof(m_msg.TraderID) - 1, TD_GetTraderId(), '\0');
	*(TraderIdType*)m_msg.TraderID = TD_GetNumericTraderIdPrintable();
	m_msg.TraderID[sizeof(m_msg.TraderID) - 1] = '\0';

	m_msg.ExecutionReferenceID[sizeof(m_msg.ExecutionReferenceID) - 1] = '\0';
	m_msg.Symbol[sizeof(m_msg.Symbol) - 1] = '\0';
	m_msg.MarketReferenceID[sizeof(m_msg.MarketReferenceID) - 1] = '\0';

	*(unsigned int*)m_mmid = 0;
	m_mmid[sizeof(unsigned int)] = '\0';

	*(unsigned int*)m_liquidity = 0;
	m_liquidity[sizeof(unsigned int)] = '\0';

	m_ComboBoxSide.SetInvalid(m_ref);
	m_ComboBoxSide.SetColorInvalid(m_visualBase->GetGrayColor());
	SetSizeSpinCells(m_SpinSize);
	SetPriceSpinCells(m_SpinPrice);
	SetSizeSpinCells(m_SpinOrderSize);

	m_bottomControl += vgap;
	int width = 64;
	int sideWidth = m_firstColumnWidth + m_firstColumnWidth - width;//width / 2;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinBustExecSizeId);
	AddStaticIndirect("Size", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth + 1, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticBustExecSizeId);

	const int doubleWidth = m_firstColumnWidth + m_firstColumnWidth;
	const int priceWidth = m_firstColumnWidth + spinHeight * 11 / 8;
	const int priceNameWidth = doubleWidth - priceWidth;

	m_bottomControl += spinHeight + vgap;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinBustExecPriceId);
	AddStaticIndirect("Price", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + priceWidth, m_bottomControl + (spinHeight - staticHeight) / 2, priceNameWidth, staticHeight, staticBustExecPriceId);

	m_bottomControl += spinHeight + vgap;

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 400, comboBoxBustExecSideId);
	AddStaticIndirect("Side", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticBustExecSideId);

	m_bottomControl += comboBoxHeight + vgap;

	AddButtonIndirect("Busted", WS_VISIBLE | WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, m_bottomControl, m_firstColumnWidth, checkBoxHeight, checkBoxBustExecBustedId);
	AddButtonIndirect("Dropcopy", WS_VISIBLE | WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, m_bottomControl += checkBoxHeight + vgap, m_firstColumnWidth, checkBoxHeight, checkBoxBustExecDropcopyId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += checkBoxHeight + vgap, m_firstColumnWidth, 400, comboBoxBustExecDestinationId);
	AddStaticIndirect("Venue", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticBustExecDestinationId);

	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, m_firstColumnWidth, editHeight, editBustExecMmidId);
	AddStaticIndirect("MMID", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticBustExecMmidId);

	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, m_bottomControl += editHeight + vgap, m_firstColumnWidth, editHeight, editBustExecLiquidityId);
	AddStaticIndirect("Liquidity", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth, m_bottomControl + (editHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticBustExecLiquidityId);

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl += editHeight + vgap, m_firstColumnWidth, spinHeight, spinBustExecOrderSizeId);
	AddStaticIndirect("Order Size", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + m_firstColumnWidth + 1, m_bottomControl + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticBustExecOrderSizeId);

	m_bottomControl += spinHeight;

	int controlWidth = m_xc + width;
	controlWidth += sideWidth;

//	m_bottomControl += comboBoxHeight;
#ifdef MUTIPLE_BORROWS
	TD_GetCustomExternalObservable().AddObserver(this);
#endif

	int y = vgap + buttonHeight + vgap + groupOffset;
	const int x = m_firstColumnWidth + m_firstColumnWidth + hgap * 3;
#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	m_SpinSendMessageToAllAccountsDelay.AddSpinCell(1, false);
	m_SpinSendMessageToAllAccountsDelay.AddSpinCell(10, false);
	m_SpinSendMessageToAllAccountsDelay.AddSpinCell(100, true);

	y = vgap;
	AddButtonIndirect("Send Message to all Accounts", WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON | WS_TABSTOP, 0, x, y, m_sendButtonWidth * 2, buttonHeight, buttonBustExecSendMessageToAllAccountsId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP, 0, x, y += buttonHeight + vgap, m_firstColumnWidth, spinHeight, spinBustExecSendMessageToAllAccountsDelayId);
	AddStaticIndirect("Delay", WS_VISIBLE | SS_NOPREFIX, 0, x + m_firstColumnWidth, y + (spinHeight - staticHeight) / 2, m_firstColumnWidth, staticHeight, staticBustExecSendMessageToAllAccountsDelayId);

	y += spinHeight + vgap * 5;
#endif

//	const int x2 = m_xc + m_firstColumnWidth + m_firstColumnWidth + hgap;
//	y = vgap + buttonHeight + vgap + groupOffset;
//	const int x3 = x2 + width;
	const int x3 = x + width;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, x, y, width, editHeight, editBustExecRefId);
	AddButtonIndirect("Ref ID", WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX | WS_GROUP, 0, x3, y + (editHeight - checkBoxHeight) / 2, sideWidth, checkBoxHeight, checkBoxBustExecRefId);
}
/*
void SampleDialogSettingSimulateBustExec::SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000, true);
}

void SampleDialogSettingSimulateBustExec::SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}
*/
#ifdef MUTIPLE_BORROWS

class TMsgMyCustomExternal : public TMsgCustomExternal
{
public:
	TMsgMyCustomExternal():TMsgCustomExternal(234, sizeof(TMsgMyCustomExternal)){}
};

static unsigned int ii = 0;
static const unsigned int slee = 10;
static const unsigned int ccount = 40;
static char s = 'A';

void SampleDialogSettingSimulateBustExec::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_CUSTOM_EXTERNAL:
		if(((const TMsgCustomExternal*)message)->m_customId == 234)
		{
			char buf[4];
			buf[0] = 'A';
			buf[1] = 'A';
			buf[3] = '\0';
			if(ii % 2)
			{
				buf[2] = s;
				U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol) - 1, buf, '\0');
				++s;
				if (s > 'Z')s = 'A';
			}
			else U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol) - 1, m_symbol.c_str(), '\0');

			++ii;

			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, m_accountId, '\0');
			m_messageSimulationThread->WriteMessage(&m_msg);
			Sleep(slee);
			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, "2345", '\0');
			m_messageSimulationThread->WriteMessage(&m_msg);
			Sleep(slee);
			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, "18960016", '\0');
			m_messageSimulationThread->WriteMessage(&m_msg);
			Sleep(slee);
			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, "18960001", '\0');
			m_messageSimulationThread->WriteMessage(&m_msg);
			Sleep(slee);
			if(ii < ccount)
			{
				TD_WriteMessageToMainThread(&TMsgMyCustomExternal());
			}
		}
		break;

		default:
		BaseDialogSettingSimulateAccountOption::Notify(message, from, info);
		break;
	}
}
#endif

const Message* SampleDialogSettingSimulateBustExec::CreateMessage(WSABUF& wsaBuf)
{
	if(m_ref)
	{
		const Execution* execution = m_account->FindExecutionByRefId(m_refNumber);
		if(execution)
		{
			const Order* order = execution->GetOrder();
			const Position* position = order->GetPosition();

			m_msg.m_time = TL_GetCurrentMillisecond();
			m_msg.AcceptedTime = m_msg.m_time;
			m_msg.Time = m_msg.m_time;

			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, m_accountId, '\0');
			U_CopyAndPad(m_msg.ExecutionReferenceID, sizeof(m_msg.ExecutionReferenceID) - 1, m_refNumber.c_str(), '\0');
			U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol) - 1, execution->GetSymbol(), '\0');
//			m_msg.BillableFlag = 0;//execution->GetBillable();
			m_msg.CanceledQuantity = order->GetCanceledQuantity();
			m_msg.ClOrdID = order->GetClientId();
			m_msg.SrOrdID = order->GetServerId();
#ifndef TAKION_NO_OPTIONS
			const unsigned __int64& optionBlock = position->GetOptionBlock();
			m_msg.InstrumentType = optionBlock ? 1 : 0;
			*(unsigned __int64*)m_msg.ConfirmationID = optionBlock;
//			*((unsigned __int64*)m_msg.ConfirmationID + 1) = 0;
#endif
			m_msg.FillPriceDollar = execution->GetDollars();
			m_msg.FillPriceFraction = execution->GetDollarFraction();
			m_msg.FillQuantity = execution->GetQuantity();
//			*(unsigned int*)m_msg.FirmID = m_account->GetClearingFirmIdNum();
			*(unsigned int*)m_msg.FirmID = m_account->GetStockLoanFirmIdNum();
//			m_msg.LiquidityFlag = 0;//execution->GetLiquidity();
			m_msg.Liquidity = execution->GetLiquidity();
			U_CopyAndPad(m_msg.MarketReferenceID, sizeof(m_msg.MarketReferenceID) - 1, order->GetMarketReferenceId(), '\0', true);
//			m_msg.MessageSequenceNumber = 0;
			*(unsigned int*)m_msg.MMID = execution->GetMmid();
			m_msg.m_orderRemainingQuantity = order->GetPendingQuantity();
			m_msg.Quantity = order->GetQuantity();
			unsigned int shares = order->GetFilledQuantity() + m_msg.m_orderRemainingQuantity;
//			m_msg.NofillQuantity = m_msg.Quantity;
//			if(m_msg.NofillQuantity > shares)m_msg.NofillQuantity -= shares;
//			else m_msg.NofillQuantity = 0;
			m_msg.ParentClOrdID = order->GetParentId();
			bool recovered = execution->GetTimeReceived() == 0;
			m_msg.RealTimeOrRecovered = order->isDropcopy() ? 'D':
				recovered ? 'R':
				'\0';
			m_msg.RemainingQuantity = m_msg.m_orderRemainingQuantity;
			m_msg.ReportType = REPORT_BUSTED;
			m_msg.Route = order->GetDestinationId();
			switch(order->GetSide())
			{
				case 'B':
				m_msg.Side = ORDER_SIDE_BUY;
				break;

				case 'S':
				m_msg.Side = ORDER_SIDE_SELL;
				break;

				default:
				m_msg.Side = ORDER_SIDE_SELL_SHORT;
				break;
			}
			m_msg.Status = m_msg.RemainingQuantity ?
				order->isBeingCanceled() ? OS_PENDING_CANCEL : OS_NEW:
				OS_NONPENDING;

			return &m_msg;
		}
	}
	else
	{
#ifdef MUTIPLE_BORROWS
		m_msg.m_time = 0;
#else
		m_msg.m_time = TL_GetCurrentMillisecond();
#endif
		m_msg.AcceptedTime = m_msg.m_time;
		m_msg.Time = m_msg.m_time;

		U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, m_accountId, '\0');
		U_CopyAndPad(m_msg.ExecutionReferenceID, sizeof(m_msg.ExecutionReferenceID) - 1, "", '\0');
		U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol) - 1, m_symbol.c_str(), '\0');
		m_msg.BillableFlag = 0;//execution->GetBillable();
		m_msg.CanceledQuantity = 0;//order->GetCanceledQuantity();
		m_msg.ClOrdID = 0;//order->GetClientId();
		m_msg.SrOrdID = 0;//order->GetServerId();
#ifndef TAKION_NO_OPTIONS
		if(isOptionUsed())//m_optionMode)
		{
			OptionKey optionKey(m_strikePrice.GetCompactPrice(),
				m_optionCall,
				m_optionSuffix,
				m_expirationDay,
				m_expirationMonth,
				m_expirationYear);

			if(m_optionSuffix)U_CopyPlusOneCharAndPad(m_msg.Symbol, sizeof(m_msg.Symbol) - 1, m_symbol.c_str(), m_optionSuffix, '\0');
			else U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol) - 1, m_symbol.c_str(), '\0');

			m_msg.InstrumentType = 1;
			*(unsigned __int64*)m_msg.ConfirmationID = OptionBlock(optionKey.CalculateOptionBlock()).GetOptionBlock();
//			*((unsigned __int64*)m_msg.ConfirmationID + 1) = 0;
		}
		else
		{
			*(unsigned __int64*)m_msg.ConfirmationID = 0;
			m_msg.InstrumentType = 0;
//			*((unsigned __int64*)m_msg.ConfirmationID + 1) = 0;
#endif
			U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol) - 1, m_symbol.c_str(), '\0');
#ifndef TAKION_NO_OPTIONS
		}
#endif
//		unsigned int quantity = m_SpinSize.GetValue();

//		Price price;
//		FillPriceFromSpinControl(price, m_SpinPrice, 10000);
		m_msg.FillPriceDollar = m_price.GetDollars();
		m_msg.FillPriceFraction = m_price.GetDollarFraction();

		m_msg.FillQuantity = m_size;//quantity;
//		*(unsigned int*)m_msg.FirmID = m_account->GetClearingFirmIdNum();
		*(unsigned int*)m_msg.FirmID = m_account->GetStockLoanFirmIdNum();
		m_msg.LiquidityFlag = '\0';//execution->GetLiquidity();
		U_CopyAndPad(m_msg.MarketReferenceID, sizeof(m_msg.MarketReferenceID) - 1, "", '\0');
//			m_msg.MessageSequenceNumber = 0;
//		*(unsigned int*)m_msg.MMID = U_RepresentStringAsUnsignedNumber<unsigned int>(m_mmid.c_str());//*(unsigned int*)"XXXX";//execution->GetMmid();
		*(unsigned int*)m_msg.MMID = U_RepresentStringAsUnsignedNumber<unsigned int>(m_mmid);//*(unsigned int*)"XXXX";//execution->GetMmid();

		m_msg.Quantity = m_orderSize;// < m_size? m_size : m_orderSize;

//		m_msg.m_orderRemainingQuantity = 0;//order->GetPendingQuantity();
		m_msg.m_orderRemainingQuantity = m_msg.Quantity - m_msg.FillQuantity;

		m_msg.Liquidity = U_RepresentStringAsUnsignedNumber<unsigned int>(m_liquidity);// m_msg.Quantity;
		unsigned int shares = m_size;//quantity;
//		m_msg.NofillQuantity = m_msg.Quantity;
//		if(m_msg.NofillQuantity > shares)m_msg.NofillQuantity -= shares;
//		else m_msg.NofillQuantity = 0;
		m_msg.ParentClOrdID = 0;//order->GetParentId();
		bool recovered = false;//execution->GetTimeReceived() == 0;
		m_msg.RealTimeOrRecovered = m_dropcopy ? 'D':
			recovered ? 'R':
			'\0';
		m_msg.RemainingQuantity = m_msg.m_orderRemainingQuantity;
		m_msg.ReportType = m_busted ? REPORT_BUSTED : REPORT_FOREIGN;
		m_msg.Route = m_currentDestination ? m_currentDestination->GetId() : 0;//order->GetDestinationId();

		switch(m_side)
		{
			case 'B':
			m_msg.Side = ORDER_SIDE_BUY;
			break;

			case 'S':
			m_msg.Side = ORDER_SIDE_SELL;
			break;

			default:
			m_msg.Side = ORDER_SIDE_SELL_SHORT;
			break;
		}
		m_msg.Status = OS_NONPENDING;

#ifdef MUTIPLE_BORROWS
		char buf[4];
		buf[0] = 'A';
		buf[1] = 'A';
		buf[3] = '\0';
		ii = 0;
//		for(unsigned int i = 0; i < 40; ++i)
//		{
			if(ii % 2)
			{
				buf[2] = s;
				U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol) - 1, buf, '\0');
				++s;
				if (s > 'Z')s = 'A';
			}
			else U_CopyAndPad(m_msg.Symbol, sizeof(m_msg.Symbol) - 1, m_symbol.c_str(), '\0');

			++ii;

			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, m_accountId, '\0');
			m_messageSimulationThread->WriteMessage(&m_msg);
			Sleep(slee);
			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, "2345", '\0');
			m_messageSimulationThread->WriteMessage(&m_msg);
			Sleep(slee);
			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, "18960016", '\0');
			m_messageSimulationThread->WriteMessage(&m_msg);
			Sleep(slee);
			U_CopyAndPad(m_msg.AccountName, sizeof(m_msg.AccountName) - 1, "18960001", '\0');
			m_messageSimulationThread->WriteMessage(&m_msg);
			Sleep(slee);
//		}
		TD_WriteMessageToMainThread(&TMsgMyCustomExternal());
#else
		return &m_msg;
#endif
	}
	return nullptr;
}

void SampleDialogSettingSimulateBustExec::ResetValues()
{
#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	U_CopyAndPad(m_accountIdBeingSent, sizeof(m_accountIdBeingSent), "", '\0');
	m_millisecondDelay = 0;
#endif
	m_refNumber.clear();
	m_ref = false;
	m_busted = false;
	m_dropcopy = false;
	m_side = 'B';
	m_size = 0;
	m_price.SetZero();
	m_orderSize = 0;
	m_currentDestination = nullptr;
	*(unsigned int*)m_mmid = 0;
	m_mmid[sizeof(unsigned int)] = '\0';
	*(unsigned int*)m_liquidity = 0;
	m_liquidity[sizeof(unsigned int)] = '\0';
//	m_mmid.clear();
}

void SampleDialogSettingSimulateBustExec::BeforeDestroy()
{
	ResetValues();
	BaseDialogSettingSimulateAccountOption::BeforeDestroy();
}

void SampleDialogSettingSimulateBustExec::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingSimulateAccountOption::ToolTipsEnabled(enable);
	m_ComboBoxDestination.EnableTooltips(enable, m_toolTip);
//	m_ComboBoxRouting.EnableTooltips(enable, m_toolTip);
}

void SampleDialogSettingSimulateBustExec::InitControls()
{
	BaseDialogSettingSimulateAccountOption::InitControls();
//	m_ComboBoxDestination.AddString((const char*)&m_exchangeDestination);

	TakionIterator* iterator = TD_CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		m_ComboBoxDestination.AddString((const char*)destination);
	}
	delete iterator;

	m_ComboBoxDestination.SetCurSel(0);
	OnSelchangeDestination();

}

BOOL SampleDialogSettingSimulateBustExec::OnInitDialog()
{
	ResetValues();

	BOOL ret = BaseDialogSettingSimulateAccountOption::OnInitDialog();
	m_EditRef.SetLimitText(24);
	m_EditMmid.SetLimitText(4);
	m_EditLiquidity.SetLimitText(4);

	m_ComboBoxSide.AddString("Buy");
	m_ComboBoxSide.AddString("Sell");
	m_ComboBoxSide.AddString("Short");

	m_ComboBoxSide.SetCurSel(0);

	m_ComboBoxSide.SetInvalid(m_ref);

//	m_SpinSize.SetBkColorEnabled(m_visualBase->GetWarningColor());
//	m_SpinPrice.SetBkColorEnabled(m_visualBase->GetWarningColor());
/*Moved to InitControls
	TakionIterator* iterator = TD_CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		m_ComboBoxDestination.AddString((const char*)destination);
	}
	delete iterator;

	m_ComboBoxDestination.SetCurSel(0);

	OnSelchangeDestination();
*/
	UpdateSpinColor();
//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateBustExec::UpdateSettings()
{
	UpdateAccountInConfig(extensionConfig->m_simBustExecAccountId);
/*
	int sel = m_ComboBoxAccount.GetCurSel();
	if(sel > 0 && m_account)
	{
		extensionConfig->m_simBustExecAccountId.SetValue(std::string(m_account->GetId()));
	}
	else
	{
		extensionConfig->m_simBustExecAccountId.SetValue(std::string());
	}
*/
	extensionConfig->m_simBustExecRefNumber.SetValue(m_refNumber);
	extensionConfig->m_simBustExecRef.SetValue(m_ref);
	extensionConfig->m_simBustExecBusted.SetValue(m_busted);
	extensionConfig->m_simBustExecDropcopy.SetValue(m_dropcopy);

	extensionConfig->m_simBustExecSymbol.SetValue(m_symbol);
#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_simBustExecOption.SetValue(m_optionMode);
	extensionConfig->m_simBustExecOptionCall.SetValue(m_optionCall);
	extensionConfig->m_simBustExecOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_simBustExecOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_simBustExecOptionExpiration.SetValue(GetExpirationDate());
#endif

	extensionConfig->m_simBustExecSize.SetValue(m_size);//m_SpinSize.GetValue());
//	Price price;
//	FillPriceFromSpinControl(price, m_SpinPrice, 10000);
	extensionConfig->m_simBustExecPrice.SetValue(m_price);

	extensionConfig->m_simBustExecOrderSize.SetValue(m_orderSize);//m_SpinSize.GetValue());
//	extensionConfig->m_simBustExecMmid.SetValue(U_RepresentStringAsUnsignedNumber<unsigned int>(m_mmid.c_str()));
	extensionConfig->m_simBustExecMmid.SetValue(U_RepresentStringAsUnsignedNumber<unsigned int>(m_mmid));
	extensionConfig->m_simBustExecVenue.SetValue(m_currentDestination ? m_currentDestination->GetMainNumericName() : 0);
	extensionConfig->m_simBustExecLiquidity.SetValue(U_RepresentStringAsUnsignedNumber<unsigned int>(m_liquidity));

	extensionConfig->m_simBustExecSide.SetValue(m_side);

#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	extensionConfig->m_simBustExecSendMessageToAllAccountsDelay.SetValue(m_SpinSendMessageToAllAccountsDelay.GetValue());
#endif
}

void SampleDialogSettingSimulateBustExec::RestoreLayout()
{
	SetAccountById(extensionConfig->m_simBustExecAccountId.GetValue().c_str());
/*
	const std::string& accountName = extensionConfig->m_simBustExecAccountId.GetValue();
	int sel;
	if(accountName.empty())
	{
		sel = 0;
	}
	else
	{
		sel = m_ComboBoxAccount.FindItemByName(accountName.c_str());
		if(sel < 0)sel = 0;
	}
	m_ComboBoxAccount.SetCurSel(sel);
	if(sel <= 0)
	{
		SetAccount(TD_GetCurrentAccount());
	}
	else
	{
		SetAccount((Account*)m_ComboBoxAccount.GetItemDataPtr(sel));
	}
*/
	const bool ref = extensionConfig->m_simBustExecRef.GetValue();
	if(ref != m_ref)
	{
		m_CheckBoxRef.SetCheck(ref ? BST_CHECKED : BST_UNCHECKED);
		m_ref = ref;
		m_ComboBoxSide.SetInvalid(m_ref);
	}

	const bool busted = extensionConfig->m_simBustExecBusted.GetValue();
	if(busted != m_busted)
	{
		m_CheckBoxBusted.SetCheck(busted ? BST_CHECKED : BST_UNCHECKED);
		m_busted = busted;
	}

	const bool dropcopy = extensionConfig->m_simBustExecDropcopy.GetValue();
	if(dropcopy != m_dropcopy)
	{
		m_CheckBoxDropcopy.SetCheck(dropcopy ? BST_CHECKED : BST_UNCHECKED);
		m_dropcopy = dropcopy;
	}

	const std::string& refNumber = extensionConfig->m_simBustExecRefNumber.GetValue();
	if(refNumber != m_refNumber)
	{
		m_EditRef.SetWindowText(refNumber.c_str());
		m_EditRef.Invalidate(FALSE);
	}

	m_EditSymbol.SetWindowText(extensionConfig->m_simBustExecSymbol.GetValue().c_str());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_simBustExecOption.GetValue(),
		extensionConfig->m_simBustExecOptionSuffix.GetValue(),
		extensionConfig->m_simBustExecOptionCall.GetValue(),
		extensionConfig->m_simBustExecOptionStrikePrice.GetValue(),
		extensionConfig->m_simBustExecOptionExpiration.GetValue());
#endif

	const unsigned int& size = extensionConfig->m_simBustExecSize.GetValue();
	if(size != m_size)
	{
		m_size = size;
		FillSpinControlFromUInt(m_size, m_SpinSize, false);
	}

	const Price& price = extensionConfig->m_simBustExecPrice.GetValue();
	if(price != m_price)
	{
		m_price = price;
		FillSpinControlFromPrice(m_price, m_SpinPrice, 10000, false);
	}

	const unsigned int& orderSize = extensionConfig->m_simBustExecOrderSize.GetValue();
	if(orderSize != m_orderSize)
	{
		m_orderSize = orderSize;
		FillSpinControlFromUInt(m_orderSize, m_SpinOrderSize, false);
	}

	char buf[sizeof(unsigned int) + 1];
	buf[sizeof(unsigned int)] = '\0';

	const unsigned int& mmid = extensionConfig->m_simBustExecMmid.GetValue();
//	const unsigned int oldMmid = U_RepresentStringAsUnsignedNumber<unsigned int>(m_mmid.c_str());//*(unsigned int*)"XXXX";//execution->GetMmid();
	const unsigned int oldMmid = U_RepresentStringAsUnsignedNumber<unsigned int>(m_mmid);
	if(oldMmid != mmid)
	{
		*(unsigned int*)buf = mmid;
		m_EditMmid.SetWindowText(buf);
	}

	const unsigned int& liquidity = extensionConfig->m_simBustExecLiquidity.GetValue();
	const unsigned int oldLiquidity = U_RepresentStringAsUnsignedNumber<unsigned int>(m_liquidity);
	if(oldLiquidity != liquidity)
	{
		*(unsigned int*)buf = liquidity;
		m_EditLiquidity.SetWindowText(buf);
	}

	const unsigned int& venueNumericName = extensionConfig->m_simBustExecVenue.GetValue();
	if(venueNumericName != (m_currentDestination ? m_currentDestination->GetMainNumericName() : 0))
	{
		int found = m_ComboBoxDestination.FindItemByNumericName(venueNumericName);
		if(found >= 0)
		{
			m_ComboBoxDestination.SetCurSel(found);
			OnSelchangeDestination();
		}
		else if(m_ComboBoxDestination.GetCurSel() != 0)
		{
			m_ComboBoxDestination.SetCurSel(0);
			OnSelchangeDestination();
		}
	}

	UpdateSpinColor();

	const char side = extensionConfig->m_simBustExecSide.GetValue();
	if(side != m_side)
	{
		switch(side)
		{
			case 'B':
			m_side = side;
			m_ComboBoxSide.SetCurSel(0);
			break;

			case 'S':
			m_side = side;
			m_ComboBoxSide.SetCurSel(1);
			break;

			case 'T':
			m_side = side;
			m_ComboBoxSide.SetCurSel(2);
			break;

			default:
			if('B' != m_side)
			{
				m_side = 'B';
				m_ComboBoxSide.SetCurSel(0);
			}
			break;
		}
	}

#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	FillSpinControlFromUInt(extensionConfig->m_simBustExecSendMessageToAllAccountsDelay.GetValue(), m_SpinSendMessageToAllAccountsDelay, false);
#endif

	EnableSendMessageButton();

	m_modified = false;
}

void SampleDialogSettingSimulateBustExec::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateAccountOption::DoDataExchange(pDX);
	DDX_Control(pDX, editBustExecRefId, m_EditRef);
	DDX_Control(pDX, checkBoxBustExecRefId, m_CheckBoxRef);
	DDX_Control(pDX, checkBoxBustExecBustedId, m_CheckBoxBusted);
	DDX_Control(pDX, checkBoxBustExecDropcopyId, m_CheckBoxDropcopy);
	DDX_Control(pDX, spinBustExecSizeId, m_SpinSize);
	DDX_Control(pDX, spinBustExecPriceId, m_SpinPrice);
	DDX_Control(pDX, comboBoxBustExecSideId, m_ComboBoxSide);

	DDX_Control(pDX, spinBustExecOrderSizeId, m_SpinOrderSize);

	DDX_Control(pDX, comboBoxBustExecDestinationId, m_ComboBoxDestination);

	DDX_Control(pDX, editBustExecMmidId, m_EditMmid);
	DDX_Control(pDX, editBustExecLiquidityId, m_EditLiquidity);

#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	DDX_Control(pDX, buttonBustExecSendMessageToAllAccountsId, m_ButtonSendMessageToAllAccounts);
	DDX_Control(pDX, spinBustExecSendMessageToAllAccountsDelayId, m_SpinSendMessageToAllAccountsDelay);
#endif
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateBustExec, BaseDialogSettingSimulateAccountOption)
#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	ON_WM_TIMER()
	ON_BN_CLICKED(buttonBustExecSendMessageToAllAccountsId, OnSendMessageToAllAccounts)
#endif
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editBustExecRefId, OnRefChange)
	ON_BN_CLICKED(checkBoxBustExecRefId, OnRef)
	ON_BN_CLICKED(checkBoxBustExecBustedId, OnBusted)
	ON_BN_CLICKED(checkBoxBustExecDropcopyId, OnDropcopy)
	ON_CBN_SELCHANGE(comboBoxBustExecSideId, OnSelchangeSide)

	ON_CBN_SELCHANGE(comboBoxBustExecDestinationId, OnSelchangeDestination)

	ON_EN_CHANGE(editBustExecMmidId, OnMmidChange)
	ON_EN_UPDATE(editBustExecMmidId, OnMmidUpdate)

	ON_EN_CHANGE(editBustExecLiquidityId, OnLiquidityChange)
	ON_EN_UPDATE(editBustExecLiquidityId, OnLiquidityUpdate)

	ON_MESSAGE(WM_USER + 15, OnSpinChange)
//	ON_EN_UPDATE(editBustExecRefId, OnRefUpdate)

END_MESSAGE_MAP()

#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS

//bool SampleDialogSettingSimulateBustExec::isSendMessageButtonEnabled() const
//{
//	return BaseDialogSettingSimulateAccountOption::isSendMessageButtonEnabled() && !*m_accountIdBeingSent;
//}

void SampleDialogSettingSimulateBustExec::OnSendMessageToAllAccounts()
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

void SampleDialogSettingSimulateBustExec::OnTimer(UINT_PTR nIDEvent)
{
	SendMessageToNextAccount();
}

bool SampleDialogSettingSimulateBustExec::SendMessageToAccountAt(unsigned int i)
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

void SampleDialogSettingSimulateBustExec::SendMessageToNextAccount()
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

void SampleDialogSettingSimulateBustExec::OnMmidUpdate()
{
	m_EditMmid.TryUpdate();
}

void SampleDialogSettingSimulateBustExec::OnMmidChange()
{
	CString mmid;
	m_EditMmid.GetWindowText(mmid);
//	if(strcmp(mmid, m_mmid.c_str()))
	if(strcmp(mmid, m_mmid))
	{
		strncpy_s(m_mmid, sizeof(m_mmid), mmid, sizeof(unsigned int));
//		m_mmid = mmid;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateBustExec::OnLiquidityUpdate()
{
	m_EditLiquidity.TryUpdate();
}

void SampleDialogSettingSimulateBustExec::OnLiquidityChange()
{
	CString liquidity;
	m_EditLiquidity.GetWindowText(liquidity);
	if(strcmp(liquidity, m_liquidity))
	{
		strncpy_s(m_liquidity, sizeof(m_liquidity), liquidity, sizeof(unsigned int));
		m_modified = true;
	}
}

void SampleDialogSettingSimulateBustExec::OnSelchangeDestination()
{
//	UpdateDestinationRouting();
	int sel = m_ComboBoxDestination.GetCurSel();
	const Destination* const dest = sel >= 0 ? (const Destination*)m_ComboBoxDestination.GetItemDataPtr(sel) : nullptr;
	if(dest != m_currentDestination)
	{
		m_currentDestination = dest;
		m_modified = true;
		UpdateExecutionPriceSpinColor();
		EnableSendMessageButton();
	}
}

LRESULT SampleDialogSettingSimulateBustExec::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
		case spinBustExecSendMessageToAllAccountsDelayId:
		m_modified = true;
		break;
#endif
		case spinBustExecSizeId:
		{
			const unsigned int size = m_SpinSize.GetValue();
			if(size != m_size)
			{
/*
				if(!m_ref)
				{
					if(size == 0)
					{
						m_SpinSize.SetBkColorEnabled(m_visualBase->GetWarningColor());
					}
					else if(m_size == 0)
					{
						m_SpinSize.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
					}
				}
*/
				m_size = size;
				m_modified = true;
				UpdateOrderAndExecutionSizeSpinColor();
				EnableSendMessageButton();
			}
		}
		break;

		case spinBustExecPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPrice, 10000);
			if(price != m_price)
			{
/*
				if(!m_ref)
				{
					if(price.isZero())
					{
						m_SpinPrice.SetBkColorEnabled(m_visualBase->GetWarningColor());
					}
					else if(m_price.isZero())
					{
						m_SpinPrice.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
					}
				}
*/
				m_price = price;
				m_modified = true;
				UpdateExecutionPriceSpinColor();
				EnableSendMessageButton();
			}
		}
		break;

		case spinBustExecOrderSizeId:
		{
			const unsigned int size = m_SpinOrderSize.GetValue();
			if(size != m_orderSize)
			{
/*
				if(!m_ref)
				{
					if(size == 0)
					{
						m_SpinOrderSize.SetBkColorEnabled(m_visualBase->GetWarningColor());
					}
					else if(m_orderSize == 0)
					{
						m_SpinOrderSize.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
					}
				}
*/
				m_orderSize = size;
				m_modified = true;
				UpdateOrderSizeSpinColor();
				EnableSendMessageButton();
			}
		}
		break;

#ifndef TAKION_NO_OPTIONS
		default:
		BaseDialogSettingSimulateAccountOption::OnSpinChange(w, l);
		break;
#endif
	}
	return 0;
}

void SampleDialogSettingSimulateBustExec::OnSelchangeSide()
{
//	const int sel = m_ComboBoxSide.GetCurSel();
	char side;
	switch(m_ComboBoxSide.GetCurSel())
	{
		case 1:
		side = 'S';
		break;

		case 2:
		side = 'T';
		break;

		default:
		side = 'B';
		break;
	}
	if(side != m_side)
	{
		m_side = side;
		m_modified = true;
	}
}

#ifndef TAKION_NO_OPTIONS
bool SampleDialogSettingSimulateBustExec::isOptionModeUsed() const
{
	return BaseDialogSettingSimulateAccountOption::isOptionModeUsed() && !m_ref;//SYMT_SYMBOL_STRING == m_symbolType;
}
#endif

void SampleDialogSettingSimulateBustExec::OnBusted()
{
	const bool busted = m_CheckBoxBusted.GetCheck() == BST_CHECKED;
	if(busted != m_busted)
	{
		m_busted = busted;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateBustExec::OnDropcopy()
{
	const bool dropcopy = m_CheckBoxDropcopy.GetCheck() == BST_CHECKED;
	if(dropcopy != m_dropcopy)
	{
		m_dropcopy = dropcopy;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateBustExec::OnRef()
{
	const bool ref = m_CheckBoxRef.GetCheck() == BST_CHECKED;
	if(ref != m_ref)
	{
		m_ref = ref;
		m_modified = true;
		m_EditRef.Invalidate(FALSE);
		m_EditSymbol.Invalidate();
#ifndef TAKION_NO_OPTIONS
		OptionModeChanged();
		m_CheckBoxOption.Invalidate(FALSE);
#endif
		m_ComboBoxSide.SetInvalid(m_ref);

		UpdateSpinColor();

		m_CheckBoxBusted.Invalidate(FALSE);
		m_CheckBoxDropcopy.Invalidate(FALSE);
	}
}

void SampleDialogSettingSimulateBustExec::UpdateOrderSizeSpinColor()
{
//	m_SpinOrderSize.SetBkColorEnabled(m_ref ? m_visualBase->GetGrayColor() : m_orderSize >= m_size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
}

void SampleDialogSettingSimulateBustExec::UpdateOrderAndExecutionSizeSpinColor()
{
	m_SpinSize.SetBkColorEnabled(m_ref ? m_visualBase->GetGrayColor() : m_size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
	UpdateOrderSizeSpinColor();
}

void SampleDialogSettingSimulateBustExec::UpdateExecutionPriceSpinColor()
{
	m_SpinPrice.SetBkColorEnabled(m_ref ? m_visualBase->GetGrayColor() : m_price.isZero() && (!m_currentDestination || !m_currentDestination->isBorrow()) ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
}

void SampleDialogSettingSimulateBustExec::UpdateSpinColor()
{
	if(m_ref)
	{
		m_SpinSize.SetBkColorEnabled(m_visualBase->GetGrayColor());
		m_SpinPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
		m_SpinOrderSize.SetBkColorEnabled(m_visualBase->GetGrayColor());
	}
	else
	{
		m_SpinSize.SetBkColorEnabled(m_size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
		m_SpinPrice.SetBkColorEnabled(m_price.isZero() && (!m_currentDestination || !m_currentDestination->isBorrow()) ? m_visualBase->GetWarningColor() : GetSysColor(COLOR_WINDOW));
//		m_SpinOrderSize.SetBkColorEnabled(m_orderSize >= m_size ? GetSysColor(COLOR_WINDOW) : m_visualBase->GetWarningColor());
		m_SpinOrderSize.SetBkColorEnabled(GetSysColor(COLOR_WINDOW));
	}
}
/*
void BaseDialogSettingSimulateQuote::OnRefUpdate()
{
	m_EditRef.TryUpdate();
}
*/
void SampleDialogSettingSimulateBustExec::OnRefChange()
{
	bool empty = m_refNumber.empty();
	CString text;
	m_EditRef.GetWindowText(text);
	m_refNumber = text;
	m_modified = true;
	if(m_refNumber.empty() != empty && m_ref)
	{
		m_EditRef.Invalidate(FALSE);
		EnableSendMessageButton();
	}
}

bool SampleDialogSettingSimulateBustExec::isSendMessageButtonEnabled() const
{
	return m_ref ? BaseDialogSettingSimulateAccount::isSendMessageButtonEnabled() && !m_refNumber.empty():
		BaseDialogSettingSimulateAccountOption::isSendMessageButtonEnabled()
//		&& !m_price.isZero()
		&& m_size != 0
//		&& m_orderSize >= m_size
		&& m_currentDestination != nullptr
		&& !*m_accountIdBeingSent;
}

HBRUSH SampleDialogSettingSimulateBustExec::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editBustExecRefId:
		if(!m_ref)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		else if(m_refNumber.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetWarningBrush();
		}
		break;

		case checkBoxBustExecBustedId:
		case checkBoxBustExecDropcopyId:
		if(m_ref)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;
	}
	return BaseDialogSettingSimulateAccountOption::OnCtlColor(pDC, pWnd, nCtlColor);
}
