#include "stdafx.h"
#include "BaseDialogSettingAccount.h"
//#include "ExtensionSample.h"
#include "ExtensionDerivedConfig.h"

//extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////////
BaseDialogSettingAccount::BaseDialogSettingAccount(TakionMainWnd* mainWnd,
	TakionSettingTabDialog* parentTab,
	bool useAccount,
	const char* label,
	bool includeCurrentAccount,
	int accountWidth,
	int topOffset,
	bool displayAccountChoice) :

	BaseDialogSetting(mainWnd, parentTab, label),
	m_useAccount(useAccount),
	m_account(nullptr),
	m_accountChoice(this, DT_LEFT, 0, RGB(255, 255, 255), nullptr, nullptr, "", false),
	m_includeCurrentAccount(includeCurrentAccount),
	m_displayAccountChoice(displayAccountChoice),
	//	m_position(nullptr),
	//	m_symbolCurrent(true),
	//	m_EditSymbol(U_GetValidStockChars(), false),
	m_groupBoxAccount(this, 0, RGB(255, 255, 255), nullptr, "Account", 8),// int borderWidth = 1, unsigned int borderColorPercentDiff = 36)
	m_ComboBoxAccount(0xFFFFFFFF, "Current"),
	m_accountGroupHeight(24),
	m_accountWidth(accountWidth),
	m_bottomControl(topOffset),
	m_accountBottom(m_bottomControl),
	m_xc(hgap),

#ifndef TAKION_NO_OPTIONS
	m_futureExecutorConnected(false),
	m_futureExecutorLogged(false),
#endif
	m_executorConnected(false),
	m_executorLogged(false),
	m_tradingAllowed(false),
	m_suspended(false),
	m_historyLoaded(false),
	m_untradeableStatusLoaded(false),
	m_restrictedSymbolListLoaded(false),

	m_traderIdEnabled(false),
	m_tradingLockedByTrader(false),
	m_tradingDefaultAccountOnlyByTrader(false),
	m_tradingTestStocksOnlyByTrader(false)
{
	U_CopyAndPad(m_accountId, sizeof(m_accountId), "", '\0');

	if(m_useAccount)
	{
		AddCustomGroupBox(&m_groupBoxAccount);
//		AddButtonIndirect("Account", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, m_xc - hgap, m_bottomControl, m_accountWidth, m_accountGroupHeight, groupAccountId);
		AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl + groupOffset, m_accountWidth - hgap - hgap, 400, comboBoxAccountId);

		m_bottomControl += m_accountGroupHeight;

		if(m_displayAccountChoice)
		{
			m_bottomControl += staticHeight + vgap;
			AddStringArea(&m_accountChoice);
		}

		m_accountBottom = m_bottomControl;
	}
}
/*
BaseDialogSettingAccount::~BaseDialogSettingAccount()
{
//	if(m_accountChoice)delete m_accountChoice;
}
*/
void BaseDialogSettingAccount::Resize(bool widthChanged, bool heightChanged)
{
	BaseDialogSetting::Resize(widthChanged, heightChanged);

	if(m_ComboBoxAccount.m_hWnd)
	{
		CRect rect;
		m_ComboBoxAccount.GetWindowRect(&rect);
		ScreenToClient(&rect);

		const int textHeight = m_groupBoxAccount.GetTextHeight();
		const int groupOffset = 4;
//		CRect groupRect(rect.left - groupOffset, rect.top - textHeight - groupOffset, rect.right + groupOffset, rect.bottom + groupOffset);
		CRect groupRect(0, rect.top - textHeight - groupOffset, rect.right + groupOffset, rect.bottom + groupOffset);
//		rect.left -= groupOffset;
//		rect.right += groupOffset;
//		rect.top -= textHeight / 2 + groupOffset;
//		rect.bottom += groupOffset;
		m_groupBoxAccount.SetRect(groupRect);
		if(m_displayAccountChoice)
		{
			const int vOffset = 1;
///`		rect.left = m_clientRect.left;// +extremeGap;
			rect.top = groupRect.bottom + vOffset;
			rect.bottom = rect.top + textHeight;
//			rect.right = rect.left + modifierWidth;
			m_accountChoice.TextAreaBase::SetRect(rect);
		}
	}
}

void BaseDialogSettingAccount::BeforeDestroy()
{
	SetAccount(nullptr);

#ifndef TAKION_NO_OPTIONS
	m_futureExecutorConnected = false;
	m_futureExecutorLogged = false;
#endif
	m_executorConnected = false;
	m_executorLogged = false;
	m_tradingAllowed = false;
	m_suspended = false;
	m_historyLoaded = false;
	m_untradeableStatusLoaded = false;
	m_restrictedSymbolListLoaded = false;

	m_traderIdEnabled = false;
	m_tradingLockedByTrader = false;
	m_tradingDefaultAccountOnlyByTrader = false;
	m_tradingTestStocksOnlyByTrader = false;

	if(m_displayAccountChoice)
	m_accountChoice.SetString("");

	TD_GetAdminObservable().RemoveObserver(this);
	TD_GetNewAccountObservable().RemoveObserver(this);
	BaseDialogSetting::BeforeDestroy();
}

BOOL BaseDialogSettingAccount::OnInitDialog()
{
	SetExecutorConnected(TD_IsExecutorConnected());
	SetExecutorLogged(TD_IsExecutorLogged());

#ifndef TAKION_NO_OPTIONS
	SetFutureExecutorConnected(TD_IsFutureExecutorConnected());
	SetFutureExecutorLogged(TD_IsFutureExecutorLogged());
#endif

	SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);
	SetTraderIdEnabled(TD_IsTraderIdEnabled() || !m_executorConnected
#ifndef TAKION_NO_OPTIONS
		&& !m_futureExecutorConnected
#endif
	);

	TradingLocked(!TD_IsTradingAllowed());
	TradingDefaultAccountOnly(TD_IsDefaultOnlyTradingAllowed());
	TradingTestStocksOnly(TD_IsTestOnlyTradingAllowed());

	TD_GetAdminObservable().AddObserver(this);//to get notified about connect / disconnect
	TD_GetNewAccountObservable().AddObserver(this);
	BOOL ret = BaseDialogSetting::OnInitDialog();

	if(m_useAccount)
	{
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

		if(m_includeCurrentAccount)
		{
			m_ComboBoxAccount.AddString(nullptr);
			m_ComboBoxAccount.SetCurSel(0);
			OnSelchangeAccount();
		}
	}
//	RestoreLayout();

	return ret;
}

void BaseDialogSettingAccount::RestoreLayout()
{
	if(m_useAccount)
	{
		StringSetting* accountId = GetAccountIdSetting();
		if(accountId)
		{
			SetAccountId(accountId->GetValue().c_str());
		}
	}
}

void BaseDialogSettingAccount::UpdateSettings()
{
	if(m_useAccount)
	{
		StringSetting* accountId = GetAccountIdSetting();
		if(accountId)
		{
			int sel = m_ComboBoxAccount.GetCurSel();
			if((sel > 0 || !m_includeCurrentAccount && !sel) && m_account)
			{
				accountId->SetValue(m_account->GetId());
			}
			else
			{
				accountId->SetValue(std::string());
			}
		}
	}
}

void BaseDialogSettingAccount::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);

	if(m_useAccount)
	{
		DDX_Control(pDX, comboBoxAccountId, m_ComboBoxAccount);
	}
}

BEGIN_MESSAGE_MAP(BaseDialogSettingAccount, BaseDialogSetting)
	ON_CBN_SELCHANGE(comboBoxAccountId, OnSelchangeAccount)
END_MESSAGE_MAP()

void BaseDialogSettingAccount::OnSelchangeAccount()
{
	if(m_useAccount)
	{
		const int sel = m_ComboBoxAccount.GetCurSel();
		Account* account = sel >= 0 ? (Account*)m_ComboBoxAccount.GetItemDataPtr(sel) : nullptr;
		if(account)
		{
			U_CopyAndPad(m_accountId, sizeof(m_accountId), account->GetId(), '\0');
		}
		else
		{
			U_CopyAndPad(m_accountId, sizeof(m_accountId), "", '\0');
			account = m_includeCurrentAccount ? TD_GetCurrentAccount() : nullptr;
		}
		SetAccount(account);
		m_modified = true;
	}
}

void BaseDialogSettingAccount::UpdateComboBoxAccount()
{
	if(m_useAccount)
	{
		int sel = m_ComboBoxAccount.FindItem(m_account);
		m_ComboBoxAccount.SetCurSel(sel);
		OnSelchangeAccount();
	}
}

void BaseDialogSettingAccount::AddAccount(Account* account)
{
	if(m_useAccount)
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
}

void BaseDialogSettingAccount::AddAccountObserver()
{
	if(m_account)
	{
		m_account->AddObserver(this);
	}
}

void BaseDialogSettingAccount::RemoveAccountObserver()
{
	if(m_account)
	{
		m_account->RemoveObserver(this);
	}
}

void BaseDialogSettingAccount::SetAccount(Account* account)
{
	if(m_useAccount)
	{
		if(m_account != account)
		{
			if(ShouldAddAccountObserver())
			{
				RemoveAccountObserver();
				m_account = account;
				AddAccountObserver();
			}
			else
			{
				m_account = account;
			}

			if(m_displayAccountChoice)
			m_accountChoice.SetString(m_account ? m_account->GetId() : "");

			if(m_account)
			{
				m_tradingAllowed = m_account->isTradingAllowed();
				m_suspended = m_account->isSuspended();
				m_historyLoaded = m_account->isHistoryLoaded();
				m_restrictedSymbolListLoaded = m_account->isRestrictedSymbolListLoaded();
			}
			else
			{
				m_tradingAllowed = false;
				m_suspended = false;
				m_historyLoaded = false;
				m_restrictedSymbolListLoaded = false;
			}
			AccountChanged();
		}
	}
}

void BaseDialogSettingAccount::CurrentAccountSet(Account* account)
{
//	if(m_ComboBoxAccount.m_hWnd && m_ComboBoxAccount.GetCurSel() <= 0)
//	if(m_ComboBoxAccount.m_hWnd && (m_includeCurrentAccount ? m_ComboBoxAccount.GetCurSel() <= 0 : m_ComboBoxAccount.GetCurSel() < 0))
	if(m_includeCurrentAccount && m_ComboBoxAccount.m_hWnd && m_ComboBoxAccount.GetCurSel() <= 0)
	{
		SetAccount(account);
	}
}

void BaseDialogSettingAccount::SetAccountId(const char* accountId)
{
	if(m_useAccount)
	{
		int sel;
		if(accountId && *accountId)
		{
			sel = m_ComboBoxAccount.FindItemByName(accountId);
			if(sel < 0 && m_includeCurrentAccount)sel = 0;
		}
		else
		{
			sel = m_includeCurrentAccount ? 0 : -1;
		}
		m_ComboBoxAccount.SetCurSel(sel);
		if(sel < 0)
		{
			SetAccount(m_includeCurrentAccount ? TD_GetCurrentAccount() : nullptr);
		}
		else if(m_includeCurrentAccount && sel == 0)
		{
			SetAccount(TD_GetCurrentAccount());
		}
		else
		{
			SetAccount((Account*)m_ComboBoxAccount.GetItemDataPtr(sel));
		}
		m_modified = false;
	}
}

void BaseDialogSettingAccount::SetTradingAllowed(const bool tradingAllowed)
{
	if(tradingAllowed != m_tradingAllowed)
	{
		m_tradingAllowed = tradingAllowed;
		OnTradingAllowed();
	}
}

void BaseDialogSettingAccount::SetSuspended(const bool suspended)
{
	if(suspended != m_suspended)
	{
		m_suspended = suspended;
		OnSuspended();
	}
}

void BaseDialogSettingAccount::SetHistoryLoaded(const bool loaded)
{
	if(loaded != m_historyLoaded)
	{
		m_historyLoaded = loaded;
		OnHistoryLoaded();
	}
}

void BaseDialogSettingAccount::SetUntradeableStatusLoaded(const bool loaded)
{
	if(loaded != m_untradeableStatusLoaded)
	{
		m_untradeableStatusLoaded = loaded;
		OnUntradeableStatusLoaded();
	}
}

void BaseDialogSettingAccount::SetRestrictedSymbolListLoaded(const bool loaded)
{
	if(loaded != m_restrictedSymbolListLoaded)
	{
		m_restrictedSymbolListLoaded = loaded;
		OnRestrictedSymbolListLoaded();
	}
}

void BaseDialogSettingAccount::SetExecutorConnected(const bool connected)
{
	if(connected != m_executorConnected)
	{
		m_executorConnected = connected;
		OnExecutorConnected();
		SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);
		if(!connected)
		{
			SetExecutorLogged(false);
#ifndef TAKION_NO_OPTIONS
			if(!m_futureExecutorConnected)
#endif
			SetTraderIdEnabled(true);
		}
	}
}

void BaseDialogSettingAccount::SetExecutorLogged(const bool logged)
{
	if(logged != m_executorLogged)
	{
		m_executorLogged = logged;
		OnExecutorLogged();
		SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);
		if(logged)
		{
			TradingDefaultAccountOnly(TD_IsDefaultOnlyTradingAllowed());
		}
	}
}

#ifndef TAKION_NO_OPTIONS
void BaseDialogSettingAccount::SetFutureExecutorConnected(const bool connected)
{
	if(connected != m_futureExecutorConnected)
	{
		m_futureExecutorConnected = connected;
		OnFutureExecutorConnected();
//		SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);
		if(!connected)
		{
			SetFutureExecutorLogged(false);
			if(!m_executorConnected)
			SetTraderIdEnabled(true);
		}
	}
}

void BaseDialogSettingAccount::SetFutureExecutorLogged(const bool logged)
{
	if(logged != m_futureExecutorLogged)
	{
		m_futureExecutorLogged = logged;
		OnFutureExecutorLogged();
//		SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);
		if(logged)
		{
			TradingDefaultAccountOnly(TD_IsDefaultOnlyTradingAllowed());
		}
	}
}
#endif
void BaseDialogSettingAccount::SetTraderIdEnabled(const bool traderIdEnabled)
{
	if(traderIdEnabled != m_traderIdEnabled)
	{
		m_traderIdEnabled = traderIdEnabled;
		OnTraderIdEnabledChanged();
	}
}

//Trading locked by the Trader through UI
void BaseDialogSettingAccount::TradingLocked(bool locked)
{
	if(m_tradingLockedByTrader != locked)
	{
		m_tradingLockedByTrader = locked;
		OnTradingLockedByTrader();
	}
}

void BaseDialogSettingAccount::TradingDefaultAccountOnly(bool defaultAccountOnly)
{
	if(m_tradingDefaultAccountOnlyByTrader != defaultAccountOnly)
	{
		m_tradingDefaultAccountOnlyByTrader = defaultAccountOnly;
		OnTradingDefaultAccountOnlyByTrader();
	}
}

void BaseDialogSettingAccount::TradingTestStocksOnly(bool testStocksOnly)
{
	if(m_tradingTestStocksOnlyByTrader != testStocksOnly)
	{
		m_tradingTestStocksOnlyByTrader = testStocksOnly;
		OnTradingTestStocksOnlyByTrader();
	}
}

void BaseDialogSettingAccount::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TS_NEW_ACCOUNT:
		if(m_useAccount && info && info->m_type == TM_NEW_ACCOUNT)
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
		if(m_useAccount)
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

		case TM_ACCOUNT_DELETE:
		if(m_useAccount)
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
		if(m_useAccount)
		{
#ifndef TAKION_NO_OPTIONS
			m_ComboBoxAccount.RemoveAllAccountsOfSecurityType(AST_STOCKS);
#else
			m_ComboBoxAccount.RemoveAllButFirstItem();
#endif
			if(m_includeCurrentAccount)
			{
				m_ComboBoxAccount.SetCurSel(0);
			}
			OnSelchangeAccount();
		}
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_DELETE_ALL_FUTURE_ACCOUNTS:
		if(m_useAccount)
		{
			m_ComboBoxAccount.RemoveAllAccountsOfSecurityType(AST_FUTURES);
			if(m_includeCurrentAccount)
			{
//				if(m_includeCurrentAccount)m_ComboBoxAccount.AddString(nullptr);
				m_ComboBoxAccount.SetCurSel(0);
			}
			OnSelchangeAccount();
		}
		break;
#endif

/*
		case M_RESP_MD_LOGON:
//		UpdateMarketDataEntitlements();
		break;
*/
		case TS_LOGIN_RESP:
#ifndef TAKION_NO_OPTIONS
		{
			const TMsgLogonResponse* msg = (const TMsgLogonResponse*)message;
			const unsigned __int64 miscSettings = msg->MiscSettings;
			const bool future = (miscSettings & FuturesTSServer) != 0;
			if(future)//miscSettings & 1ui64 << 63)
			SetFutureExecutorLogged(TD_IsFutureExecutorLogged());
			else
#endif
			SetExecutorLogged(TD_IsExecutorLogged());

			SetTraderIdEnabled(TD_IsTraderIdEnabled());
			if(m_useAccount && info && info->m_type == TM_NEW_ACCOUNT)
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
#ifndef TAKION_NO_OPTIONS
		}
#endif
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


		case TM_UPDATED_ACCOUNT://TM_ACCOUNT_UPDATE:
//		UpdateAccount(((TMsgAccount*)message)->m_account);
		if(m_useAccount)
		{
			const TMsgUpdatedAccount* msg = (const TMsgUpdatedAccount*)message;
			if(msg->m_constraintsChanged)
			{
				const Account* account = msg->m_account;
				if(m_account == account)
				{
					SetTradingAllowed(account->isTradingAllowed());
					SetSuspended(account->isSuspended());
				}
			}
		}
		break;

		case TM_ALL_UNTRADEABLE_LISTS_LOADED://This message is received from TD_GetAdminObservable() when the Untradeable Lists are loaded fpr all the Accounts
		SetUntradeableStatusLoaded(TD_GetNumberOfAccountAggregatorUntradeableListsToLoad() == 0);//true
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
		if(info && info->m_type == TM_CONNECTION_OBJECT)
		{
			const TMsgConnectionObject* msg = (const TMsgConnectionObject*)info;
			const Connection* connection = msg->m_connection;
			if(connection == TD_GetExecutorConnection())
			{
				SetExecutorConnected(TD_IsExecutorConnected());
			}
#ifndef TAKION_NO_OPTIONS
			else if(connection == TD_GetFutureExecutorConnection())
			{
				SetFutureExecutorConnected(TD_IsFutureExecutorConnected());
			}
#endif
			else
			{
				ProcessConnectDisconnect(connection);
			}
		}
		break;

		case TM_DATA_THREAD_BUFFER_OVERFLOW:
//		PostMessage(WM_USER + 500, ((const TMsgDataThreadBufferOverflow*)message)->m_threadId, 0);
		break;
	}
}
