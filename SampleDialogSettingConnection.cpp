#include "stdafx.h"
#include "SampleDialogSettingConnection.h"
//#include "ConnectionApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum SampleSettingConnectionIds
{
	listBoxConnectionId = 10000,

	connectionIdsCount
};

SampleDialogSettingConnection::SampleDialogSettingConnection(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSetting(mainWnd, parentTab, "Connection"),
	m_ListBoxConnection(true)
{
	m_ListBoxConnection.SetSelectable(false);

	int x = 0;
	int y = 0;
	int width = 200;
	int listBoxHeight = 58;
//	int xc = x + hgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, width, listBoxHeight, listBoxConnectionId);
}

HWND SampleDialogSettingConnection::GetFirstTabControl() const
{
	return m_ListBoxConnection.m_hWnd;
}

void SampleDialogSettingConnection::UpdateSettings()
{
//	BaseDialogSetting::UpdateSettings();
}

void SampleDialogSettingConnection::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
}

void SampleDialogSettingConnection::BeforeDestroy()
{
	TD_GetNewAccountObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
	BaseDialogSetting::BeforeDestroy();
}

void SampleDialogSettingConnection::Leaving()
{
//	TD_GetNewAccountObservable().RemoveObserver(this);
//	TD_GetAdminObservable().RemoveObserver(this);
}

BOOL SampleDialogSettingConnection::OnInitDialog()
{
	TD_GetNewAccountObservable().AddObserver(this);
	TD_GetAdminObservable().AddObserver(this);

	BOOL ret = BaseDialogSetting::OnInitDialog();

	UpdateConnections();
/*
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

	m_ComboBoxAccount.SetCurSel(0);
*/
	return ret;
}

void SampleDialogSettingConnection::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
	DDX_Control(pDX, listBoxConnectionId, m_ListBoxConnection);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingConnection, BaseDialogSetting)
	ON_WM_CTLCOLOR()
	ON_LBN_SELCHANGE(listBoxConnectionId, OnSelchangeConnection)
END_MESSAGE_MAP()

void SampleDialogSettingConnection::OnSelchangeConnection()
{
}

void SampleDialogSettingConnection::UpdateConnections()
{
	m_ListBoxConnection.ResetContent();

	const ActiveConnection* connection;

	TakionIterator* iterator = TD_CreateActiveConnectionIterator();
	iterator->Reset();
	while(connection = TD_GetNextActiveConnection(iterator))
	{
		m_ListBoxConnection.AddString((const char*)new ConnectionItem(nullptr, connection->GetIpInvAddress(), connection->GetPort(), "", connection->GetName(), connection->GetConnectionDataType(), connection->isConnected()));//it->second.c_str()));
	}
	delete iterator;
//	OnSelchangeConnection();
}

HBRUSH SampleDialogSettingConnection::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
/*
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
*/
	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}

void SampleDialogSettingConnection::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_NEW_ORDER:
//		DisplayNewOrder(((const TMsgOrder*)message)->m_order);
		break;
/*
		case TM_ORDER_UPDATE:
		break;
*/
		case TM_DEAD_ORDER:
//		DisplayCanceledOrder(((const TMsgOrder*)message)->m_order);
		break;

		case TM_NEW_EXECUTION:
//		DisplayExecution(((const TMsgExecution*)message)->m_execution);
		break;

		case TM_AUTO_ACTION:
/*
		if(&TD_GetAdminObservable() == from && info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			Account* account = ((TMsgUpdatedAccount*)info)->m_account;
			if(account == m_currentAccount)// || config->m_showAutoActionForAllAccounts.GetValue())
			{
				const TMsgAutoAction* msg = (const TMsgAutoAction*)message;
				DisplayAutoAction(account, msg->m_symbol, msg->m_close, msg->m_loss, msg->m_constraint);//, unsigned int uintValue, unsigned int uintConstraint)
			}
		}
*/
		break;

		case M_TEXT:
		{
//			const TMsgText* msg = (const TMsgText*)message;
//			DisplayAdminBroadcastMessage((const char*)msg + sizeof(TMsgText));
		}
		break;

		case TS_NEW_ACCOUNT:
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			Account* account = msg->m_account;
//			m_ComboBoxAccount.AddString((const char*)account);
		}
		break;

		case TM_NEW_ACCOUNT:
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)message;
			Account* account = msg->m_account;
//			m_ComboBoxAccount.AddString((const char*)account);
		}
		break;

		case TM_UPDATED_ACCOUNT://TM_ACCOUNT_UPDATE:
//		UpdateAccount(((TMsgAccount*)message)->m_account);
		break;

		case TM_ACCOUNT_DELETE:
		{
			Account* account = ((const TMsgAccountDelete*)message)->m_account;
/*
			ClearAccountOrders(account);
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
*/
		}
		break;

		case TM_DELETE_ALL_ACCOUNTS:
//		ClearStocks();
//		m_ComboBoxAccount.ResetContent();
//		m_ComboBoxAccount.AddString(nullptr);
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_DELETE_ALL_FUTURE_ACCOUNTS:
		break;
#endif
		case M_RESP_MD_LOGON:
//		UpdateMarketDataEntitlements();
		break;

		case TS_LOGIN_RESP:
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			Account* account = msg->m_account;
/*
			if(m_ComboBoxAccount.FindItem(account) < 0)
			{
				m_ComboBoxAccount.AddString((const char*)account);
			}
*/
		}
		break;

		case TS_ACCOUNT_POSITION_RESP:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;
/*
			SendAccountPositionCost(account);
			if(account == m_currentAccount)
			{
				CurrentAccountLoaded();
			}
			if(from != (Observable*)account)
			{
				AccountLoaded(account);
			}
*/
		}
		break;

		case TS_ACCOUNT_EXEC_HISTORY_COMPLETE:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;
/*
			int sel = m_ComboBoxAccount.GetCurSel();
			if(sel >= 0)
			{
				if(account == (Account*)m_ComboBoxAccount.GetItemDataPtr(sel))
				{
				}
			}
*/
		}
		break;

		case TM_DISCONNECTED:

		if(info && info->m_type == TM_CONNECTION_OBJECT)
		{
			const TMsgConnectionObject* msg = (const TMsgConnectionObject*)info;
			const Connection* connection = msg->m_connection;
			UpdateConnection(connection, false);
/*
			if(connection == TD_GetMarketSorterConnection())
			{
			}
			else if(connection == TD_GetMarketDataConnection())
			{
			}
			else if(connection == TD_GetExecutorConnection())
			{
			}
*/
		}
		break;

		case TM_CONNECTED:
		if(info && info->m_type == TM_CONNECTION_OBJECT)
		{
//			const TMsgConnected* msg = (const TMsgConnected*)message;
//			ConnectionConnected(msg->m_ipAddress, msg->m_port, msg->m_ipBindAddress, msg->m_bindPort, msg->m_connectionName);
			const TMsgConnectionObject* msg = (const TMsgConnectionObject*)info;
			const Connection* connection = msg->m_connection;
			UpdateConnection(connection, true);
/*
			if(connection == TD_GetMarketSorterConnection())
			{
			}
			else if(connection == TD_GetMarketDataConnection())
			{
			}
			else if(connection == TD_GetExecutorConnection())
			{
			}
*/
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

		case TM_MARKET_SORTER_LOADED:
//		MarketSorterLoaded();
		break;

		case TM_MARKET_SORTER_CLEANUP:
//		MarketSorterCleanup();
		break;

		case TM_DATA_THREAD_BUFFER_OVERFLOW:
//		PostMessage(WM_USER + 500, ((const TMsgDataThreadBufferOverflow*)message)->m_threadId, 0);
		break;
	}
}

void SampleDialogSettingConnection::UpdateConnection(const Connection* connection, bool connected)
{
/*
enum ConnectionDataType : unsigned char
{
	CDT_MARKET_DATA,
	CDT_EXECUTOR,
	CDT_MARKET_SORTER,

	CDT_COUNT
};
*/
	unsigned int dataType = connection->GetConnectionDataType();
	ConnectionItem* item;
	for(unsigned int i = 0, end = m_ListBoxConnection.GetItemCount(); i < end; ++i)
	{
		item = (ConnectionItem*)m_ListBoxConnection.GetItemDataPtr(i);
		if(dataType == item->GetDataType())
		{
			if(item->isConnected() != connected)
			{
				item->SetConnected(connected);
				m_ListBoxConnection.InvalidateItem(i);
				return;
			}
		}
	}
	if(connected)
	{
		m_ListBoxConnection.AddString((const char*)new ConnectionItem(nullptr, connection->GetIpInvAddress(), connection->GetPort(), "", connection->GetName(), connection->GetConnectionDataType(), connected));//it->second.c_str()));
	}
}
