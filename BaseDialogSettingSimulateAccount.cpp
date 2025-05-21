#include "stdafx.h"
#include "BaseDialogSettingSimulateAccount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulateAccount::BaseDialogSettingSimulateAccount(TakionMainWnd* mainWnd,
	TakionSettingTabDialog* parentTab,
	const char* label,
	bool accountControlNextToSend):
	BaseDialogSettingSimulateMessage(mainWnd, parentTab, label),
	m_ComboBoxAccount(0xFFFFFFFF, "Current"),
	m_account(nullptr),
	m_accountSel(-1),
	m_comboBoxAccountWidth(52),
	m_historyLoaded(false),
	m_untradeableStatusLoaded(false),
	m_restrictedSymbolListLoaded(false),
	m_executorConnected(false),
	m_executorLogged(false)
{
	U_CopyAndPad(m_accountId, sizeof(m_accountId), "", '\0');
	if(accountControlNextToSend)
	{
//		const int width = 52;
		int y = vgap;
		if(buttonHeight > comboBoxHeight)
		{
			y += (buttonHeight - comboBoxHeight) / 2;
		}
		AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc + m_sendButtonWidth + hgap, y, m_comboBoxAccountWidth, 400, comboBoxAccountAccountId);
	}
	else
	{
		const int accountGroupHeight = 24;
		AddButtonIndirect("Account", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, m_xc, m_bottomControl, m_firstColumnWidth + m_firstColumnWidth, accountGroupHeight, groupAccountAccountId);
		AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc + hgap, m_bottomControl + groupOffset, m_firstColumnWidth + m_firstColumnWidth - hgap - hgap, 400, comboBoxAccountAccountId);
		m_bottomControl += accountGroupHeight;

		const int controlWidth =
#ifndef TAKION_NO_OPTIONS
			hgap + m_firstColumnWidth +
#endif
			m_xc + m_firstColumnWidth;
		if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
	}
}
/*
HBRUSH BaseDialogSettingSimulateAccount::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case radioOptionCallId:
		case radioOptionPutId:
		if(!isOptionUsed())//!m_optionMode || SYMT_SYMBOL_STRING != m_symbolType)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_takionVisual->GetGrayColor());
			return *m_bkBrush;
		}
		break;
	}
	return BaseDialogSettingSimulateSymbol::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/
void BaseDialogSettingSimulateAccount::UpdateAccountInConfig(StringSetting& accountId) const
{
//	int sel = m_ComboBoxAccount.GetCurSel();
//	if(sel > 0 && m_account)
	if(m_accountSel > 0 && m_account)
	{
		accountId.SetValue(std::string(m_account->GetId()));
	}
	else
	{
		accountId.SetValue(std::string());
	}
}

void BaseDialogSettingSimulateAccount::SetAccountById(const char* accountId)
{
//	const std::string& accountName = extensionConfig->m_simPosLoadAccountId.GetValue();
	int sel;
	if(accountId && *accountId)
	{
		sel = m_ComboBoxAccount.FindItemByName(accountId);
		if(sel < 0)sel = 0;
	}
	else
	{
		sel = 0;
	}
	m_ComboBoxAccount.SetCurSel(sel);
	OnSelchangeAccount();
/*
	if(sel <= 0)
	{
		SetAccount(TD_GetCurrentAccount());
	}
	else
	{
		SetAccount((Account*)m_ComboBoxAccount.GetItemDataPtr(sel));
	}
*/
}

bool BaseDialogSettingSimulateAccount::isSendMessageButtonEnabled() const
{
	return m_account != nullptr;
}

void BaseDialogSettingSimulateAccount::BeforeDestroy()
{
	TD_GetNewAccountObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
	BaseDialogSettingSimulateMessage::BeforeDestroy();
}

BOOL BaseDialogSettingSimulateAccount::OnInitDialog()
{
	m_accountSel = -1;
	m_account = nullptr;
	U_CopyAndPad(m_accountId, sizeof(m_accountId), "", '\0');

	TD_GetNewAccountObservable().AddObserver(this);
	TD_GetAdminObservable().AddObserver(this);

	SetExecutorConnected(TD_IsExecutorConnected());
	SetExecutorLogged(TD_IsExecutorLogged());

	SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);

	BOOL ret = BaseDialogSettingSimulateMessage::OnInitDialog();

	m_ComboBoxAccount.AddString(nullptr);
	TakionIterator* iterator = TD_CreateAccountIterator();
	Account* account;
	TD_LockAccountStorage();
	iterator->Reset();
	while(account = (Account*)TD_GetNextAccount(iterator))
	{
		m_ComboBoxAccount.AddString((const char*)account);
	}
	TD_UnlockAccountStorage();
	delete iterator;

	return ret;
}

void BaseDialogSettingSimulateAccount::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateMessage::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxAccountAccountId, m_ComboBoxAccount);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateAccount, BaseDialogSettingSimulateMessage)
	ON_CBN_SELCHANGE(comboBoxAccountAccountId, OnSelchangeAccount)
END_MESSAGE_MAP()

void BaseDialogSettingSimulateAccount::OnSelchangeAccount()
{
	int sel = m_ComboBoxAccount.GetCurSel();
	if(m_accountSel != sel)
	{
		m_accountSel = sel;
		m_modified = true;
		Account* account = sel > 0 ? (Account*)m_ComboBoxAccount.GetItemDataPtr(sel) : nullptr;
		if(account)
		{
			U_CopyAndPad(m_accountId, sizeof(m_accountId), account->GetId(), '\0');
		}
		else
		{
			U_CopyAndPad(m_accountId, sizeof(m_accountId), "", '\0');
			account = TD_GetCurrentAccount();
		}
		SetAccount(account);
	}
	else if(!m_accountSel)
	{
//		m_modified = true;
		SetAccount(TD_GetCurrentAccount());
	}
}

void BaseDialogSettingSimulateAccount::UpdateComboBoxAccount()
{
	if(m_accountSel > 0)
	{
		int sel = m_ComboBoxAccount.FindItem(m_account);
		m_ComboBoxAccount.SetCurSel(sel);
		OnSelchangeAccount();
	}
}

void BaseDialogSettingSimulateAccount::AddAccount(Account* account)
{
	int sel = m_ComboBoxAccount.AddString((const char*)account);
	if(*m_accountId)
	{
		if(!strcmp(m_accountId, account->GetId()))
		{
			m_ComboBoxAccount.SetCurSel(sel);
			SetAccount(account);
		}
	}
}

void BaseDialogSettingSimulateAccount::SetHistoryLoaded(const bool loaded)
{
	if(loaded != m_historyLoaded)
	{
		m_historyLoaded = loaded;
	}
}

void BaseDialogSettingSimulateAccount::SetUntradeableStatusLoaded(const bool loaded)
{
	if(loaded != m_untradeableStatusLoaded)
	{
		m_untradeableStatusLoaded = loaded;
	}
}

void BaseDialogSettingSimulateAccount::SetRestrictedSymbolListLoaded(const bool loaded)
{
	if(loaded != m_restrictedSymbolListLoaded)
	{
		m_restrictedSymbolListLoaded = loaded;
	}
}

void BaseDialogSettingSimulateAccount::SetExecutorConnected(const bool connected)
{
	if(connected != m_executorConnected)
	{
		m_executorConnected = connected;
		SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);
		if(!connected)
		{
			SetExecutorLogged(false);
//			SetHistoryLoaded(false);
//			SetRestrictedSymbolListLoaded(false);
//			EnableSendMessageButton();
		}
	}
}

void BaseDialogSettingSimulateAccount::SetExecutorLogged(const bool logged)
{
	if(logged != m_executorLogged)
	{
		m_executorLogged = logged;
		if(!logged)
		{
			SetHistoryLoaded(false);
			SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);
			SetRestrictedSymbolListLoaded(false);
		}
		EnableSendMessageButton();
	}
}

void BaseDialogSettingSimulateAccount::SetAccount(Account* account)
{
	if(m_account != account)
	{
		if(m_account)
		{
			m_account->RemoveObserver(this);
		}
		m_account = account;
		if(m_account)
		{
			U_CopyAndPad(m_accountId, sizeof(m_accountId), m_account->GetId(), '\0');
			m_account->AddObserver(this);
			SetHistoryLoaded(m_account->isHistoryLoaded());
			SetRestrictedSymbolListLoaded(m_account->isRestrictedSymbolListLoaded());
//			SetUntradeableStatusLoaded(m_account->isUntradeableStatusLoaded());
		}
		else
		{
			U_CopyAndPad(m_accountId, sizeof(m_accountId), "", '\0');
//			const Account* account = TD_GetCurrentAccount();
//			SetHistoryLoaded(account && account->isHistoryLoaded());
			SetHistoryLoaded(false);
			SetRestrictedSymbolListLoaded(false);
		}
		m_modified = true;
		AccountChanged();
	}
}

void BaseDialogSettingSimulateAccount::CurrentAccountSet(Account* account)
{
	if(m_ComboBoxAccount.m_hWnd && m_accountSel <= 0)//m_ComboBoxAccount.GetCurSel() <= 0)
	{
		SetAccount(account);
	}
}

void BaseDialogSettingSimulateAccount::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TS_NEW_ACCOUNT:
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			Account* account = msg->m_account;
			m_ComboBoxAccount.AddString((const char*)account);
			if(*m_accountId && !strcmp(account->GetId(), m_accountId))
			{
				SetAccount(account);
				UpdateComboBoxAccount();
			}
		}
		break;

		case TM_NEW_ACCOUNT:
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)message;
			Account* account = msg->m_account;
			m_ComboBoxAccount.AddString((const char*)account);
			if(*m_accountId && !strcmp(account->GetId(), m_accountId))
			{
				SetAccount(account);
				UpdateComboBoxAccount();
			}
		}
		break;

		case TM_UPDATED_ACCOUNT://TM_ACCOUNT_UPDATE:
/*
		{
			const TMsgUpdatedAccount* msg = (const TMsgUpdatedAccount*)message;
//			if(msg->m_constraintsChanged)
//			{
				const Account* account = msg->m_account;
				UpdateAccount(account);
//			}
		}
*/
		break;

		case TM_ACCOUNT_DELETE:
		{
			const Account* account = ((const TMsgAccountDelete*)message)->m_account;
			const int found = m_ComboBoxAccount.FindItem(account);
			if(found >= 0)
			{
				const int sel = m_ComboBoxAccount.GetCurSel();
				m_ComboBoxAccount.DeleteString(found);
				if(sel >= 0)
				{
					const unsigned int count = m_ComboBoxAccount.GetItemCount();
					if(count > 0)
					{
//						if(count > (unsigned int)found)count = found;
//						else --count;
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
#ifndef TAKION_NO_OPTIONS
		m_ComboBoxAccount.RemoveAllAccountsOfSecurityType(AST_STOCKS);
#else
		m_ComboBoxAccount.RemoveAllButFirstItem();
#endif
//		m_ComboBoxAccount.ResetContent();
//		m_ComboBoxAccount.AddString(nullptr);
		m_ComboBoxAccount.SetCurSel(0);
		OnSelchangeAccount();
		break;


#ifndef TAKION_NO_OPTIONS
		case TM_DELETE_ALL_FUTURE_ACCOUNTS:
		m_ComboBoxAccount.RemoveAllAccountsOfSecurityType(AST_FUTURES);
		m_ComboBoxAccount.SetCurSel(0);
		OnSelchangeAccount();
		break;
#endif

/*
		case M_RESP_MD_LOGON:
//		UpdateMarketDataEntitlements();
		break;
*/
		case TS_LOGIN_RESP:
		SetExecutorLogged(TD_IsExecutorLogged());
//		SetTraderIdEnabled(TD_IsTraderIdEnabled());
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			Account* account = msg->m_account;
			if(m_ComboBoxAccount.FindItem(account) < 0)
			{
				m_ComboBoxAccount.AddString((const char*)account);
				if(*m_accountId && !strcmp(account->GetId(), m_accountId))
				{
					SetAccount(account);
					UpdateComboBoxAccount();
				}
			}
		}
		EnableSendMessageButton();
		break;

		case TS_ACCOUNT_POSITION_RESP:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)//History Loaded
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;
			if(m_account == account)
			{
				SetHistoryLoaded(true);
			}
		}
		break;

		case TM_ALL_UNTRADEABLE_LISTS_LOADED://This message is received from TD_GetAdminObservable() when the Untradeable Lists are loaded fpr all the Accounts
		SetUntradeableStatusLoaded(true);
		break;

		case TM_ACCOUNT_RESTRICTED_SYMBOL_LIST_LOADED:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)//Restricted Symbol List Loaded
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;
			if(m_account == account)
			{
				SetRestrictedSymbolListLoaded(true);
			}
		}
		break;

		case TS_ACCOUNT_EXEC_HISTORY_COMPLETE:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;
			if(m_account == account)
			{
			}
		}
		break;

		case TS_ACCOUNT_ORDER_HISTORY_COMPLETE:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;
			if(m_account == account)
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