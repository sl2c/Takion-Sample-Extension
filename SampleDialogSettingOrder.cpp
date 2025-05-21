#include "stdafx.h"
#include "SampleDialogSettingOrder.h"
//#include "StockMarketSorter.h"
#include "ExtensionSample.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;

enum OrderStatusValues : unsigned char
{
	OSE_UNACKNOWLEDGED,
	OSE_PENDING,
	OSE_PARTIALLY_FILLED,
	OSE_FILLED,//Order Dead
	OSE_UNFILLED,//Order Dead
	OSE_REJECTED,

	OSE_Count
};

class OrderObserver : public Observer
{
public:
	OrderObserver(Order* const& order, ListBoxOrder* const& listBoxOrder, const bool& inThread):
		m_order(order),
		m_listBoxOrder(listBoxOrder),
		m_sizePending(0),
		m_sizeExecuted(0),
		m_serverId(0),
		m_replacedByClientId(0),
		m_replacedBySendId(0),
		m_timeCancelSent(0),
		m_timeDead(0),
		m_status(OSE_Count),
		m_rejectCode(0),
		m_rejectText(nullptr),

		m_inThread(inThread)
	{
		AddObserver();
		m_listBoxOrder->AddItem(this);
//		m_listBoxOrder->AddString((const char*)this);
//		m_listBoxOrder->RepaintCount();
		Update();
		if(m_status == OSE_Count)m_status = CalculateOrderStatus();

	}
	virtual ~OrderObserver();
	void SetInThread(const bool& inThread)
	{
		if(inThread != m_inThread)
		{
			if(inThread)
			{
				if(!m_order->isDead())
				{
					m_order->AddInThreadObserver(this);
				}
				m_order->RemoveObserver(this);
			}
			else
			{
				if(!m_order->isDead())
				{
					m_order->AddObserver(this);
				}
				m_order->RemoveInThreadObserver(this);
			}
			m_inThread = inThread;
		}
	}
	void RemoveObserver()
	{
		if(m_inThread)
		{
			m_order->RemoveInThreadObserver(this);
		}
		else
		{
			m_order->RemoveObserver(this);
		}
	}
	void AddObserver()
	{
		if(!m_order->isDead())
		{
			if(m_inThread)
			{
				m_order->AddInThreadObserver(this);
			}
			else
			{
				m_order->AddObserver(this);
			}
		}
	}
	Order* GetOrder() const{return m_order;}
	bool isDead() const{return m_status > OSE_PARTIALLY_FILLED;}
	const bool& isIso() const{return m_order->isIso();}
	const bool& isReplacingNatively() const{return m_order->isReplacingNatively();}
#ifndef TAKION_NO_OPTIONS
	char isCallPut() const{return m_order->GetOptionBlock() ? m_order->isCall() ? 'C' : 'P' : '\0';}
	const unsigned __int64& GetOptionBlock() const{return m_order->GetOptionBlock();}
	unsigned char GetSecurityType() const{return m_order->GetSecurityType();}
#endif
	const unsigned int& GetClientId() const{return m_order->GetClientId();}
	const Money& GetClosedPnl() const{return m_closedPnl;}
	const Money& GetBpUsed() const{return m_bpUsed;}
	const Price& GetExecutedPrice() const{return m_executedPrice;}
	const unsigned int& GetSizePending() const{return m_sizePending;}
	const unsigned int& GetSizeExecuted() const{return m_sizeExecuted;}
	const unsigned int& GetServerId() const{return m_serverId;}
	const unsigned int& GetReplacedByClientId() const{return m_replacedByClientId;}
	const unsigned int& GetReplacedBySendId() const{return m_replacedBySendId;}
	const unsigned int& GetTimeCancelSent() const{return m_timeCancelSent;}
	const unsigned int& GetTimeDead() const{return m_timeDead;}
	const unsigned char& GetStatus() const{return m_status;}//OrderStatusEnum
	const unsigned short& GetRejectCode() const{return m_rejectCode;}
	const char* GetRejectText() const{return m_rejectText;}
	unsigned char CalculateOrderStatus() const;

//When the order is updated in the worker thread, we need to LockInquiry when we paint in the main thread, and LockModification when we change the values in the worker thread.
	bool LockInquiry() const{return m_paintLock.LockInquiry();}
	void LockInquiryWait() const{m_paintLock.LockInquiryWait();}
	void UnlockInquiry() const{m_paintLock.UnlockInquiry();}
	bool LockModification() const{return m_paintLock.LockModification();}
	void UnlockModification() const{m_paintLock.UnlockModification();}
protected:
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
//InThread
	void UpdateInThread();
	void PostRepaintMessageFromWorkerThread(unsigned short startColumn, unsigned short endColumn) const;
	TakionLockMultithread m_paintLock;//When the order is updated in the worker thread, we need to LockInquiry when we paint in the main thread, and LockModification when we change the values in the worker thread.
//
	bool Update();

	Order* m_order;
	ListBoxOrder* m_listBoxOrder;
	Money m_closedPnl;
	Money m_bpUsed;
	Price m_executedPrice;
	unsigned int m_sizePending;
	unsigned int m_sizeExecuted;
	unsigned int m_serverId;
	unsigned int m_replacedByClientId;
	unsigned int m_replacedBySendId;
	unsigned int m_timeCancelSent;
	unsigned int m_timeDead;
	unsigned char m_status;//OrderStatusEnum
	unsigned short m_rejectCode;
	bool isRejectTextChanged(const char* rejectText) const;
	bool UpdateRejectText(const char* rejectText);
	char* m_rejectText;

	bool m_inThread;

private:
	OrderObserver(const OrderObserver& other){}
	OrderObserver& operator=(const OrderObserver& other){return *this;}
};

class LbOrderHorizontalScroller : public LbHorizontalScroller
{
public:
	LbOrderHorizontalScroller(ListBoxOrder* listBox);
	virtual void AddColumns() override;
protected:
	ListBoxOrder* m_listBoxOrder;
};

class TMsgSampleOrderClientId : public TMsgCustomExternal
{
public:
	unsigned int m_clientId;
protected:
	TMsgSampleOrderClientId(const unsigned int& clientId, const unsigned short& tipe, const unsigned short& length):
		TMsgCustomExternal(tipe, length),
 		m_clientId(clientId)
	{}
};

class TMsgSampleNewOrderCreated : public TMsgSampleOrderClientId
{
public:
	TMsgSampleNewOrderCreated(const unsigned int& clientId):
		TMsgSampleOrderClientId(clientId, NEW_ORDER_CREATED, sizeof(TMsgSampleNewOrderCreated))
	{}
};

class TMsgSampleNewOrderSendOrderDone : public TMsgSampleOrderClientId
{
public:
	TMsgSampleNewOrderSendOrderDone(const unsigned int& sendId):
		TMsgSampleOrderClientId(sendId, NEW_ORDER_SEND_ORDER_DONE, sizeof(TMsgSampleNewOrderSendOrderDone))
	{}
};

class TMsgSampleOrderDeleted : public TMsgSampleOrderClientId
{
public:
	TMsgSampleOrderDeleted(const unsigned int& clientId) :
		TMsgSampleOrderClientId(clientId, ORDER_DELETED, sizeof(TMsgSampleOrderDeleted))
	{}
};

class TMsgSampleOrderTotalsChanged : public TMsgCustomExternal
{
public:
	TMsgSampleOrderTotalsChanged(const Money& closedPnlDiff, const Money& bpUsedDiff, const unsigned int& sizePendingDiff, const unsigned int& sizeExecutedDiff):
		TMsgCustomExternal(ORDER_TOTALS_CHANGED, sizeof(TMsgSampleOrderTotalsChanged)),
		m_closedPnlDiff(closedPnlDiff),
		m_bpUsedDiff(bpUsedDiff),
		m_sizePendingDiff(sizePendingDiff),
		m_sizeExecutedDiff(sizeExecutedDiff)
	{}
	Money m_closedPnlDiff;
	Money m_bpUsedDiff;
	unsigned int m_sizePendingDiff;
	unsigned int m_sizeExecutedDiff;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

OrderObserver::~OrderObserver()
{
	if(m_rejectText)delete[] m_rejectText;
}

bool OrderObserver::isRejectTextChanged(const char* rejectText) const
{
	return rejectText != m_rejectText && (!rejectText || !m_rejectText || strcmp(m_rejectText, rejectText) != 0);
/*
	if(rejectText == m_rejectText)
	{
		return false;
	}
	if(rejectText && m_rejectText)
	{
		return strcmp(m_rejectText, rejectText) != 0;
	}
	return true;
*/
}

bool OrderObserver::UpdateRejectText(const char* rejectText)
{
	bool rejectTextChanged = false;
	if(rejectText)
	{
		if(m_rejectText)
		{
			if(strcmp(m_rejectText, rejectText))
			{
				unsigned int len = (unsigned int)strlen(m_rejectText);
				unsigned int lenOther = (unsigned int)strlen(rejectText);
				if(len != lenOther)
				{
					delete[] m_rejectText;
					m_rejectText = new char[lenOther + 1];
				}
				strcpy_s(m_rejectText, lenOther + 1, rejectText);
				rejectTextChanged = true;
			}
		}
		else
		{
			unsigned int lenOther = (unsigned int)strlen(rejectText);
			m_rejectText = new char[lenOther + 1];
			strcpy_s(m_rejectText, lenOther + 1, rejectText);
			rejectTextChanged = true;
		}
	}
	else if(m_rejectText)
	{
		delete[] m_rejectText;
		m_rejectText = nullptr;
		rejectTextChanged = true;
	}
	return rejectTextChanged;
}

unsigned char OrderObserver::CalculateOrderStatus() const
{
	return !m_order->GetServerId() ? OSE_UNACKNOWLEDGED:
		m_order->GetPendingQuantity() ? m_order->GetFilledQuantity() ? OSE_PARTIALLY_FILLED : OSE_PENDING:
		m_order->GetFilledQuantity() ? OSE_FILLED : m_order->GetRejectedQuantity() ? OSE_REJECTED : OSE_UNFILLED;
}

void OrderObserver::PostRepaintMessageFromWorkerThread(unsigned short startColumn, unsigned short endColumn) const
{
/*
	if(m_ordinal != 0xFFFFFFFF)
	{
		m_owner->PostMessage(WM_USER + 444, m_ordinal, ((unsigned int)startColumn << 16) | endColumn);
	}
*/
	if(startColumn <= endColumn)
	{
		m_listBoxOrder->PostMessage(WM_USER + 444, m_order->GetClientId(), ((unsigned int)startColumn << 16) | endColumn);
	}
}

void OrderObserver::UpdateInThread()
{
	if(m_inThread)
	{
		const Money closedPnl = m_order->GetClosedPnl();
		const Money bpUsed = m_order->GetBpUsed();
		const Price executedPrice = m_order->GetAveragePrice();
		const unsigned int sizePending = m_order->GetPendingQuantity();
		const unsigned int sizeExecuted = m_order->GetFilledQuantity();
		const unsigned int serverId = m_order->GetServerId();
		const unsigned int timeCancelSent = m_order->GetTimeCancelSent();
		const unsigned int timeDead = m_order->GetTimeDead();
		const unsigned int rejectCode = m_order->GetRejectCode();
		const unsigned int replacedByClientId = m_order->GetReplacedByClientId();
		const unsigned int replacedBySendId = m_order->GetReplacedBySendId();
		const bool rejected = m_order->GetRejectedQuantity() != 0;
		const char* rejectText = m_order->GetRejectText();

//		const bool rejectTextChanged = UpdateRejectText(m_order->GetRejectText());

		const unsigned char status = rejected ? OSE_REJECTED:
			!serverId ? OSE_UNACKNOWLEDGED:
			sizePending ? sizeExecuted ? OSE_PARTIALLY_FILLED : OSE_PENDING:
			sizeExecuted ? OSE_FILLED : OSE_UNFILLED;

		const bool statusChanged = status != m_status;
		const bool serverIdChanged = serverId != m_serverId;
		const bool replacedByClientIdChanged = replacedByClientId != m_replacedByClientId;
		const bool replacedBySendIdChanged = replacedBySendId != m_replacedBySendId;
		const bool executedPriceChanged = executedPrice != m_executedPrice;
		const bool sizeExecutedChanged = sizeExecuted != m_sizeExecuted;
		const bool timeCancelSentChanged = timeCancelSent != m_timeCancelSent;
		const bool timeDeadChanged = timeDead != m_timeDead;
		const bool rejectCodeChanged = rejectCode != m_rejectCode;
		const bool rejectTextChanged = isRejectTextChanged(rejectText);

//		const bool closedPnlChanged = closedPnl != m_closedPnl;
		const Money closedPnlDiff = closedPnl - m_closedPnl;

//		const bool bpUsedChanged = bpUsed != m_bpUsed;
		const Money bpUsedDiff = bpUsed - m_bpUsed;

//		const bool sizePendingChanged = sizePending != m_sizePending;
		const unsigned int sizePendingDiff = m_sizePending - sizePending;

//		const bool sizeExecutedChanged = sizeExecuted != m_sizeExecuted;
		const unsigned int sizeExecutedDiff = sizeExecuted - m_sizeExecuted;

		bool totalsChanged = false;

		unsigned short firstColumnUpdated = 0xFFFF;
		unsigned short lastColumnUpdated = 0;

		const bool unlock = LockModification();

		if(statusChanged)
		{
			m_status = status;
			firstColumnUpdated = 0;
			lastColumnUpdated = 0xFFFF;
		}
		if(serverIdChanged)
		{
			m_serverId = serverId;
			if(OCI_SERVER_ID < firstColumnUpdated)firstColumnUpdated = OCI_SERVER_ID;
			if(OCI_SERVER_ID > lastColumnUpdated)lastColumnUpdated = OCI_SERVER_ID;
		}
		if(replacedByClientIdChanged)
		{
			m_replacedByClientId = replacedByClientId;
			if(OCI_REPLACED_BY_CLIENT_ID < firstColumnUpdated)firstColumnUpdated = OCI_REPLACED_BY_CLIENT_ID;
			if(OCI_REPLACED_BY_CLIENT_ID > lastColumnUpdated)lastColumnUpdated = OCI_REPLACED_BY_CLIENT_ID;
		}
		if(replacedBySendIdChanged)
		{
			m_replacedBySendId = replacedBySendId;
			if(OCI_REPLACED_BY_SEND_ID < firstColumnUpdated)firstColumnUpdated = OCI_REPLACED_BY_SEND_ID;
			if(OCI_REPLACED_BY_SEND_ID > lastColumnUpdated)lastColumnUpdated = OCI_REPLACED_BY_SEND_ID;
		}
		if(!closedPnlDiff.isZero())
		{
			totalsChanged = true;
//			m_listBoxOrder->UpdateClosedPnl(m_closedPnl, closedPnl);
			m_closedPnl = closedPnl;
			if(OCI_CLOSED_PNL < firstColumnUpdated)firstColumnUpdated = OCI_CLOSED_PNL;
			if(OCI_CLOSED_PNL > lastColumnUpdated)lastColumnUpdated = OCI_CLOSED_PNL;
		}
		if(!bpUsedDiff.isZero())
		{
			totalsChanged = true;
//			m_listBoxOrder->UpdateBpUsed(m_bpUsed, bpUsed);
			m_bpUsed = bpUsed;
			if(OCI_BP_USED < firstColumnUpdated)firstColumnUpdated = OCI_BP_USED;
			if(OCI_BP_USED > lastColumnUpdated)lastColumnUpdated = OCI_BP_USED;
		}
		if(executedPriceChanged)
		{
			totalsChanged = true;
			m_executedPrice = executedPrice;
			if(OCI_EXECUTED_PRICE < firstColumnUpdated)firstColumnUpdated = OCI_EXECUTED_PRICE;
			if(OCI_EXECUTED_PRICE > lastColumnUpdated)lastColumnUpdated = OCI_EXECUTED_PRICE;
		}
		if(sizePendingDiff)
		{
			totalsChanged = true;
//			m_listBoxOrder->UpdateSizePending(m_sizePending, sizePending);
			m_sizePending = sizePending;
			if(OCI_PENDING_SIZE < firstColumnUpdated)firstColumnUpdated = OCI_PENDING_SIZE;
			if(OCI_PENDING_SIZE > lastColumnUpdated)lastColumnUpdated = OCI_PENDING_SIZE;
		}
		if(sizeExecutedDiff)
		{
			totalsChanged = true;
//			m_listBoxOrder->UpdateSizeExecuted(m_sizeExecuted, sizeExecuted);
			m_sizeExecuted = sizeExecuted;
			if(OCI_EXECUTED_SIZE < firstColumnUpdated)firstColumnUpdated = OCI_EXECUTED_SIZE;
			if(OCI_EXECUTED_SIZE > lastColumnUpdated)lastColumnUpdated = OCI_EXECUTED_SIZE;
		}
		if(timeCancelSentChanged)
		{
			m_timeCancelSent = timeCancelSent;
			if(OCI_TIME_CANCEL_SENT < firstColumnUpdated)firstColumnUpdated = OCI_TIME_CANCEL_SENT;
			if(OCI_TIME_CANCEL_SENT > lastColumnUpdated)lastColumnUpdated = OCI_TIME_CANCEL_SENT;
		}
		if(timeDeadChanged)
		{
			m_timeDead = timeDead;
			if(OCI_TIME_DEAD < firstColumnUpdated)firstColumnUpdated = OCI_TIME_DEAD;
			if(OCI_TIME_DEAD > lastColumnUpdated)lastColumnUpdated = OCI_TIME_DEAD;
		}
		if(rejectCodeChanged)
		{
			m_rejectCode = rejectCode;
			if(OCI_REJECT_CODE < firstColumnUpdated)firstColumnUpdated = OCI_REJECT_CODE;
			if(OCI_REJECT_CODE > lastColumnUpdated)lastColumnUpdated = OCI_REJECT_CODE;
		}
		if(rejectTextChanged)
		{
			UpdateRejectText(rejectText);
			if(OCI_REJECT_TEXT < firstColumnUpdated)firstColumnUpdated = OCI_REJECT_TEXT;
			if(OCI_REJECT_TEXT > lastColumnUpdated)lastColumnUpdated = OCI_REJECT_TEXT;
		}

		if(unlock)UnlockModification();

//Can't repaint from a thread. Post Windows Message
		PostRepaintMessageFromWorkerThread(firstColumnUpdated, lastColumnUpdated);

//Write custom message to main thread to change the totals
		if(totalsChanged)
		{
			TD_WriteCustomExternalMessageToMainThread(&TMsgSampleOrderTotalsChanged(closedPnlDiff, bpUsedDiff, sizePendingDiff, sizeExecutedDiff));
		}
	}
}

bool OrderObserver::Update()//returns true if order status changed. If returns true, then we need to check if the order needs to be removed from the list box because it no longer belongs there.
{
	if(m_order->LockInquiry())//have to lock because the order may be being modified in a worker thread
	{
//Remember the values that can change. Other values don't change and can be safely taken from the order object itself at any time even if the order is being modified in a different thread.
		const Money closedPnl = m_order->GetClosedPnl();
		const Money bpUsed = m_order->GetBpUsed();
		const Price executedPrice = m_order->GetAveragePrice();
		const unsigned int sizePending = m_order->GetPendingQuantity();
		const unsigned int sizeExecuted = m_order->GetFilledQuantity();
		const unsigned int serverId = m_order->GetServerId();
		const unsigned int timeCancelSent = m_order->GetTimeCancelSent();
		const unsigned int timeDead = m_order->GetTimeDead();
		const unsigned int rejectCode = m_order->GetRejectCode();
		const unsigned int replacedByClientId = m_order->GetReplacedByClientId();
		const unsigned int replacedBySendId = m_order->GetReplacedBySendId();
		bool rejected = m_order->GetRejectedQuantity() != 0;
		const bool rejectTextChanged = UpdateRejectText(m_order->GetRejectText());
//Unlock as soon as you don't need to access the order data.
		m_order->UnlockInquiry();

		int found = m_listBoxOrder->FindOrderByClientId(m_order->GetClientId());

		unsigned char status = rejected ? OSE_REJECTED:
			!serverId ? OSE_UNACKNOWLEDGED:
			sizePending ? sizeExecuted ? OSE_PARTIALLY_FILLED : OSE_PENDING:
			sizeExecuted ? OSE_FILLED : OSE_UNFILLED;
		bool statusChanged = m_status != status;
		if(statusChanged)
		{
			m_status = status;
			m_listBoxOrder->InvalidateItem(found);//the whole row needs to be repainted because it's color changed
		}

		LbHorizontalScroller* scroller = m_listBoxOrder->GetHorizontalScroller();
		if(serverId != m_serverId)
		{
			m_serverId = serverId;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_SERVER_ID);
		}
		if(replacedByClientId != m_replacedByClientId)
		{
			m_replacedByClientId = replacedByClientId;
			if(scroller)scroller->InvalidateItemColumn(found, OCI_REPLACED_BY_CLIENT_ID);
		}
		if(replacedBySendId != m_replacedBySendId)
		{
			m_replacedBySendId = replacedBySendId;
			if(scroller)scroller->InvalidateItemColumn(found, OCI_REPLACED_BY_SEND_ID);
		}
		if(closedPnl != m_closedPnl)
		{
			m_listBoxOrder->UpdateClosedPnl(m_closedPnl, closedPnl);
			m_closedPnl = closedPnl;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_CLOSED_PNL);
		}
		if(bpUsed != m_bpUsed)
		{
			m_listBoxOrder->UpdateBpUsed(m_bpUsed, bpUsed);
			m_bpUsed = bpUsed;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_BP_USED);
		}
		if(executedPrice != m_executedPrice)
		{
			m_executedPrice = executedPrice;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_EXECUTED_PRICE);
		}
		if(sizePending != m_sizePending)
		{
			m_listBoxOrder->UpdateSizePending(m_sizePending, sizePending);
			m_sizePending = sizePending;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_PENDING_SIZE);
		}
		if(sizeExecuted != m_sizeExecuted)
		{
			m_listBoxOrder->UpdateSizeExecuted(m_sizeExecuted, sizeExecuted);
			m_sizeExecuted = sizeExecuted;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_EXECUTED_SIZE);
		}
		if(timeCancelSent != m_timeCancelSent)
		{
			m_timeCancelSent = timeCancelSent;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_TIME_CANCEL_SENT);
		}
		if(timeDead != m_timeDead)
		{
			m_timeDead = timeDead;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_TIME_DEAD);
		}
		if(rejectCode != m_rejectCode)
		{
			m_rejectCode = rejectCode;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_REJECT_CODE);
		}
		if(rejectTextChanged)
		{
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, OCI_REJECT_TEXT);
		}
		return statusChanged;
	}
//else - LockInquiry failed which means that the order is being updated and a new notification will come soon and the function will be called again.
	return false;
}

void OrderObserver::Notify(const Message* message, const Observable* from, const Message* info)
{
	if(m_inThread)
	{
		switch(message->m_type)
		{
/*
			case TS_ORDER://New Order in thread. You get this message from Account
			{
				const TMsgReqOrder* msg = (const TMsgReqOrder*)message;
//				UpdateInThread();
			}
			break;
*/
			case TS_ORDER_CANCEL_REQ://Order is requested to cancel
			{
				const TMsgReqOrderCancel* msg = (const TMsgReqOrderCancel*)message;
				const Order* orderBeingCanceled = this->m_order;
				UpdateInThread();
			}
			break;

			case TS_ORDER_REPLACE_REQ://Order is requested to replace
			{
				const TMsgReqOrderReplace* msg = (const TMsgReqOrderReplace*)message;
				if(info && info->m_type == TM_UPDATED_ORDER)//sanity check
				{
					const TMsgUpdatedOrder* msgReplacingOrder = (const TMsgUpdatedOrder*)info;
					const Order* replacingOrder = msgReplacingOrder->m_order;
					const Order* orderBeingReplaced = this->m_order;
					UpdateInThread();
				}
			}
			break;

			case TS_ORDER_ACK:
			{
				const TMsgOrderAck* msg = (const TMsgOrderAck*)message;
				UpdateInThread();
			}
			break;

			case TS_ORDER_UPDATE:
			{
				const TMsgOrderUpdate* msg = (const TMsgOrderUpdate*)message;
				UpdateInThread();
			}
			break;

			case TS_ORDER_REJECT:
			{
				const TMsgOrderReject* msg = (const TMsgOrderReject*)message;
				UpdateInThread();
			}
			break;

			case TS_ORDER_REPORT:
			{
				const TMsgOrderReport* msg = (const TMsgOrderReport*)message;
				if(info && info->m_type == TM_NEW_EXECUTION)//make sure the info is of the type we expect it to be
				{
					const Execution* execution = ((TMsgNewExecution*)info)->m_execution;
					const Order* order = execution->GetOrder();
					const unsigned __int64 userId = order->GetUserId();
					const Position* position = order->GetPosition();
					const Account* account = position->GetAccount();
				}
				UpdateInThread();
			}
			break;

//			case TS_ORDER_CANCEL_REQ:
//			case TS_ORDER_REPLACE_REQ:

			case TS_ORDER_CANCEL_ACK:
			{
				const TMsgOrderCancelAck* msg = (const TMsgOrderCancelAck*)message;
				UpdateInThread();
			}
			break;

			case TS_ORDER_KILL:
			{
				const TMsgOrderKill* msg = (const TMsgOrderKill*)message;
				UpdateInThread();
			}
			break;

			case TS_ORDER_REPLACE_ACK:
			{
				const TMsgOrderReplaceAck* msg = (const TMsgOrderReplaceAck*)message;
				UpdateInThread();
			}
			break;

			case TS_ORDER_REPLACED://You will get 2 of this - one fro replaced order and one for replacing order
			{
				const TMsgOrderReplaceDone* msg = (const TMsgOrderReplaceDone*)message;
				UpdateInThread();
			}
			break;

			case TS_CANCEL_REJECT:
			{
				const TMsgCancelReject* msg = (const TMsgCancelReject*)message;
				UpdateInThread();
			}
			break;
		}
	}
	else
	{
		if(m_order->isDeleted())
		{
			m_listBoxOrder->RemoveRow(this);
		}
		else
		{
//'this' object can get destroyed before we exit the function
			if(m_order->GetOrderState() == TU_OS_DEAD)
			{
				m_order->RemoveObserver(this);
			}
			if(Update())
			{
				if(!m_listBoxOrder->GetOwnerDialog()->OrderObserverStatusBelongs(this))
				{
					m_listBoxOrder->RemoveRow(this);
				}
			}
		}
	}
}

LbOrderHorizontalScroller::LbOrderHorizontalScroller(ListBoxOrder* listBox):
	LbHorizontalScroller(listBox),
	m_listBoxOrder(listBox)
{
}

void LbOrderHorizontalScroller::AddColumns()
{

	const int itemHeight = m_listBox->GetMyItemHeight();
	const int doubleHeight = itemHeight + itemHeight;
	const int tripleHeight = doubleHeight + itemHeight;
	const int fourHeight = tripleHeight + itemHeight;
	const int fiveHeight = fourHeight + itemHeight;
	const int sixHeight = fiveHeight + itemHeight;

	AddColumn("Symbol", OCI_SYMBOL, fiveHeight, DT_LEFT, DT_LEFT, DT_RIGHT, "Symbol");//, 0xFFFFFFFF, itemHeight, itemHeight);
#ifndef TAKION_NO_OPTIONS
	AddColumn("C/P", OCI_OPTION_CALLPUT, doubleHeight, DT_CENTER, DT_CENTER, DT_RIGHT, "Option Call (C) or Put (P)");
	AddColumn("Expiration", OCI_OPTION_EXPIRATION, fourHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Option Expiration Date");
	AddColumn("Strike", OCI_OPTION_STRIKE_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Option Strike Price");
#endif
	AddColumn("ClientId", OCI_CLIENT_ID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Client Id");
	AddColumn("Side", OCI_SIDE, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Side");
	AddColumn("$Limit", OCI_LIMIT_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Limit Price");
	AddColumn("$Stop", OCI_STOP_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Stop Price");
	AddColumn("Size", OCI_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Order Size");

	AddColumn("Venue", OCI_VENUE, tripleHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Venue");
	AddColumn("Route", OCI_ROUTING, tripleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Routing");
	AddColumn("TIF", OCI_TIF, sixHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Time in Force");
	AddColumn("Created", OCI_TIME_CREATED, fiveHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Time Created");

	AddColumn("ServerId", OCI_SERVER_ID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Server Id");
	AddColumn("$Exec", OCI_EXECUTED_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Executed Price");
	AddColumn("Pending", OCI_PENDING_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Pending Size");
	AddColumn("Executed", OCI_EXECUTED_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Executed Size");
	AddColumn("Visible", OCI_VISIBLE_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Visible Size");
	AddColumn("Status", OCI_STATUS, fourHeight, DT_LEFT, DT_LEFT, 0xFFFFFFFF, "Status", 0xFFFFFFFF, 0xFFFFFFFF, 5, 5, 5);
	AddColumn("CancelSent", OCI_TIME_CANCEL_SENT, fiveHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Cancel Sent");
	AddColumn("Dead", OCI_TIME_DEAD, fiveHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Time Dead");
	AddColumn("Replaced", OCI_REPLACED_CLIENT_ID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Client Id of the Order that was replaced by this Order");
	AddColumn("ByCl", OCI_REPLACED_BY_CLIENT_ID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Client Id of the Order that replaced this Order");
	AddColumn("BySend", OCI_REPLACED_BY_SEND_ID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Send Id of the Order(s) that replaced this Order");
	AddColumn("ClosedP&L", OCI_CLOSED_PNL, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Closed P&&L");
	AddColumn("BP Used", OCI_BP_USED, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Buying Power used by the Order");
	AddColumn("Rej", OCI_REJECT_CODE, doubleHeight, DT_CENTER, DT_CENTER, 0xFFFFFFFF, "Reject Code");
	AddColumn("Reject Text", OCI_REJECT_TEXT, sixHeight, DT_LEFT, DT_LEFT, 0xFFFFFFFF, "Reject Text");

	AddColumn("SendId", OCI_SEND_ID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Client Id returned by the SendOrder function");
	AddColumn("ParentId", OCI_PARENT_ID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Parent Id");
	AddColumn("iso", OCI_ISO, doubleHeight, DT_CENTER, DT_RIGHT, DT_RIGHT, "Inter Market Sweep Order");
	AddColumn("RN", OCI_REPLACING_NATIVELY, doubleHeight, DT_CENTER, DT_RIGHT, DT_RIGHT, "Replacing Natively");
}


int ListBoxOrder::Compare(const void* item1, const void* item2) const
{
	const unsigned int& clientId1 = ((const OrderObserver*)item1)->GetClientId();
	const unsigned int& clientId2 = ((const OrderObserver*)item2)->GetClientId();
	return clientId1 > clientId2 ? -1 : clientId1 < clientId2 ? 1 : 0;
}

void ListBoxOrder::SetInThread(const bool inThread)
{
	if(inThread != m_inThread)
	{
		m_inThread = inThread;
		OrderObserver* stock;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			stock = (OrderObserver*)GetItemDataPtr(i);
			stock->SetInThread(m_inThread);
		}
	}
}

int ListBoxOrder::FindOrderByClientId(const unsigned int clientIdToFind) const//search in the sorted collection using break in half
{
	const unsigned int count = GetItemCount();
	if(!count)
	{
		return -1;
	}
/*if unsorted list box - linear search
	if((GetStyle() & LBS_SORT) == 0 || m_doNotSort)
	{
		for(int i = 0; i < count; ++i)
		{
			if(((const OrderObserver*)GetItemDataPtr(i))->GetClientId() == clientIdToFind)
			{
				return i;
			}
		}
		return -1;
	}
*/
    int low = 0;
    int high = count - 1;
    int middle;
	unsigned int itemClientId;
    while(high >= low)
    {
        middle = (low + high) >> 1;
		itemClientId = ((const OrderObserver*)GetItemDataPtr(middle))->GetClientId();
        if(itemClientId < clientIdToFind)
        {
            high = middle - 1;
        }
        else if(itemClientId > clientIdToFind)
        {
            low = middle + 1;
        }
		else
		{
/*
			if(middle > 0)//case when duplicates are possible; get the topmost item out of all duplicates
			{
				for(--middle; middle >= 0; --middle)
				{
					if(((const OrderObserver*)GetItemDataPtr(middle))->GetClientId() != clientIdToFind)
					{
						break;
					}
				}
	            return middle + 1;
			}
*/
			return middle;
		}
    }
    return -1;
}

void ListBoxOrder::LogCancelError(unsigned char errorCode, const char* prefix)
{
	if(errorCode != CE_OK)
	{
		std::string errorMessage;
		TU_GetCancelErrorMessage(errorCode, errorMessage);
		std::string extensionPrefix("ExtSample Order Cancel ");
		if(prefix)
		{
			extensionPrefix += ' ';
			extensionPrefix += prefix;
		}
		TD_Log(errorMessage.c_str(), extensionPrefix.c_str());
	}
}

void ListBoxOrder::LogOrderError(unsigned char errorCode, const char* prefix)
{
	if(errorCode != OE_OK)
	{
		std::string errorMessage;
		TU_GetOrderErrorMessage(errorCode, errorMessage);
		std::string extensionPrefix("ExtSample Order");
		if(prefix)
		{
			extensionPrefix += ' ';
			extensionPrefix += prefix;
		}
		TD_Log(errorMessage.c_str(), extensionPrefix.c_str());
	}
}
//////////////////

BEGIN_MESSAGE_MAP(ListBoxOrder, ColumnedListBox)
	ON_MESSAGE(WM_USER + 444, OnItemRepaint)
END_MESSAGE_MAP()

LRESULT ListBoxOrder::OnItemRepaint(WPARAM clientId, LPARAM columns)
{
	int ordinal = FindOrderByClientId((unsigned int)clientId);
//	InvalidateItem(ordinal);
	InvalidateItemColumnRange(ordinal, (unsigned short)(columns >> 16), (unsigned short)columns);
	return 0;
}

void ListBoxOrder::AddItem(const OrderObserver* item)
{
	AddString((const char*)item);
	RepaintCount();
	if(item->isIso())
	{
		UpdateIsoCount(0, 1);
	}
	if(item->isReplacingNatively())
	{
		UpdateReplacingNativelyCount(0, 1);
	}
#ifndef TAKION_NO_OPTIONS
	if(item->isCallPut())
	{
		UpdateOptionCount(0, 1);
	}
#endif
}

void ListBoxOrder::RemoveItemAt(int found, const OrderObserver* item)
{
	UpdateSizePending(item->GetSizePending(), 0);
	UpdateSizeExecuted(item->GetSizeExecuted(), 0);
	if(item->isIso())
	{
		UpdateIsoCount(1, 0);
	}
	if(item->isReplacingNatively())
	{
		UpdateReplacingNativelyCount(1, 0);
	}
#ifndef TAKION_NO_OPTIONS
	if(item->isCallPut())
	{
		UpdateOptionCount(1, 0);
	}
#endif
	UpdateClosedPnl(item->GetClosedPnl(), Money::moneyZero);//TU_GetMoneyZero());
	UpdateBpUsed(item->GetBpUsed(), Money::moneyZero);//TU_GetMoneyZero());
	DeleteString(found);
	RepaintCount();
}

bool ListBoxOrder::RemoveRowAt(int found)
{
	if(found >= 0 && (unsigned int)found < m_itemCount)//GetCount())
	{
		RemoveItemAt(found, (const OrderObserver*)GetItemDataPtr(found));
		return true;
	}
	return false;
}

bool ListBoxOrder::RemoveRow(const OrderObserver* item)
{
	return RemoveOrderByClientId(item->GetClientId());
}

bool ListBoxOrder::RemoveOrder(const Order* order)
{
	return RemoveOrderByClientId(order->GetClientId());
}

bool ListBoxOrder::RemoveOrderByClientId(unsigned int clientId)
{
	const int found = FindOrderByClientId(clientId);
	if(found >= 0)
	{
		RemoveRowAt(found);
		return true;
	}
	return false;
}

void ListBoxOrder::ResetTotals()
{
	m_owner->RepaintCount();
	if(!m_closedPnl.isZero())
	{
		m_closedPnl.SetZero();
		m_owner->RepaintClosedPnl();//RepaintClosedPnl();
	}
	if(!m_bpUsed.isZero())
	{
		m_bpUsed.SetZero();
		m_owner->RepaintBpUsed();//RepaintOpenPnl();
	}
	if(m_sizePending)
	{
		m_sizePending = 0;
		m_owner->RepaintSizePending();
	}
	if(m_sizeExecuted)
	{
		m_sizeExecuted = 0;
		m_owner->RepaintSizeExecuted();
	}
	if(m_isoCount)
	{
		m_isoCount = 0;
		m_owner->RepaintIsoCount();
	}
	if(m_replacingNativelyCount)
	{
		m_replacingNativelyCount = 0;
		m_owner->RepaintReplacingNativelyCount();
	}
#ifndef TAKION_NO_OPTIONS
	if(m_optionCount)
	{
		m_optionCount = 0;
		m_owner->RepaintOptionCount();
	}
#endif
}

#ifndef TAKION_NO_OPTIONS
void ListBoxOrder::RepaintOptionCount() const
{
	m_owner->RepaintOptionCount();
}
#endif

void ListBoxOrder::RepaintCount() const
{
	m_owner->RepaintCount();
}

void ListBoxOrder::RepaintClosedPnl() const
{
	m_owner->RepaintClosedPnl();
}

void ListBoxOrder::RepaintBpUsed() const
{
	m_owner->RepaintBpUsed();
}

void ListBoxOrder::RepaintSizePending() const
{
	m_owner->RepaintSizePending();
}

void ListBoxOrder::RepaintSizeExecuted() const
{
	m_owner->RepaintSizeExecuted();
}

void ListBoxOrder::RepaintIsoCount() const
{
	m_owner->RepaintIsoCount();
}

void ListBoxOrder::RepaintReplacingNativelyCount() const
{
	m_owner->RepaintReplacingNativelyCount();
}
/*
const char* ListBoxOrder::GetItemName(const void* item) const
{
	return "";//((const OrderObserver*)item)->GetSymbol();
}
*/
COLORREF ListBoxOrder::GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	const OrderObserver* orderObserver = (const OrderObserver*)lpDrawItemStruct->itemData;
	switch(orderObserver->GetStatus())
	{
		case OSE_UNACKNOWLEDGED:
		return m_colorUnacknowledged;

		case OSE_PENDING:
		return m_colorPending;

		case OSE_PARTIALLY_FILLED:
		return m_colorPartiallyFilled;

		case OSE_FILLED://Order Dead
		return m_colorFilled;

		case OSE_UNFILLED://Order Dead
		return orderObserver->GetReplacedByClientId() ? m_colorReplaced : m_colorCancelled;

		case OSE_REJECTED:
		return m_colorRejected;

		default:
		return m_colorError;
	}
}

COLORREF ListBoxOrder::GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	if(lpDrawItemStruct->itemState & ODS_SELECTED)return RGB(255, 255, 255);
	return GetSelectedBkColor(lpDrawItemStruct);
}

void ListBoxOrder::DestroyItem(const void* item) const
{
	delete (OrderObserver*)item;
}

void ListBoxOrder::DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->itemID)
	{
		case TMID_CANCEL_ORDER:
		case TMID_REPLACE_ORDER:
		case TMID_PASTE:
		case TMID_CLIPBOARD:
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
		break;

		default:
		ColumnedListBox::DrawMenuItem(lpDrawItemStruct);
		break;
	}
}

bool ListBoxOrder::MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	switch(lpMeasureItemStruct->itemID)
	{
		case TMID_CANCEL_ORDER:
		case TMID_REPLACE_ORDER:
		case TMID_PASTE:
		case TMID_CLIPBOARD:
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
		lpMeasureItemStruct->itemWidth = m_menuWidth;// +lpMeasureItemStruct->itemHeight;
		break;

		default:
		return ColumnedListBox::MeasureMenuItem(lpMeasureItemStruct);
	}
	return true;
}

void ListBoxOrder::AddMenuItems(CMenu& menu)
{
	bool hasItems = m_itemCount > 0;
	int sel = GetCurSel();
	bool selected = sel >= 0;

	if(selected)
	{
//		menu.AppendMenu(MF_OWNERDRAW|(((const OrderObserver*)GetItemDataPtr(sel))->isDead() ? MF_GRAYED : MF_ENABLED), TMID_CANCEL_ORDER, "Cancel");
		menu.AppendMenu(MF_OWNERDRAW, TMID_CANCEL_ORDER, "Cancel");
	}
	else
	{
		menu.AppendMenu(MF_OWNERDRAW |MF_GRAYED, TMID_CANCEL_ORDER, "Cancel");
	}
	menu.AppendMenu(MF_OWNERDRAW|(selected ? MF_ENABLED : MF_GRAYED), TMID_REPLACE_ORDER, "Replace");
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW|(selected ? MF_ENABLED : MF_GRAYED), TMID_PASTE, "Paste");
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLIPBOARD, "Copy");
}

/*
void ListBoxOrder::RefreshOrdinals()
{
	OrderObserver* orderObserver;
	for(unsigned int i = 0, end = GetCount(); i < end; ++i)
	{
		stock = (OrderObserver*)GetItemDataPtr(i);
		stock->SetOrdinal(i);
	}
}
*/

BOOL ListBoxOrder::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD loword = (DWORD)wParam;//LOWORD(wParam);
    switch(loword)
    {
/*
		case TMID_CLIPBOARD:
		{
			std::string info;
			CopySymbols(info);
			TU_CopyStringToClipboard(info, m_hWnd);
		}
		break;
*/
		case TMID_CANCEL_ORDER:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				Order* order = ((OrderObserver*)GetItemDataPtr(sel))->GetOrder();
				order->Cancel();
			}
		}
		break;

		case TMID_REPLACE_ORDER:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				Order* order = ((OrderObserver*)GetItemDataPtr(sel))->GetOrder();
				unsigned int sendId;
				order->ReplacePriceAndSize(&sendId,
					nullptr,//const Security*
					m_owner->isReplaceNative(),
					m_owner->isReplaceMarketOrder(),
//Reverse Price Offset for SELLs
					order->GetSide() == 'B' ? m_owner->GetReplacePriceOffset() : -m_owner->GetReplacePriceOffset(),
					m_owner->isReplacePriceOffsetPercent(),
					m_owner->isReplacePriceEnforceMinOffset(),
					m_owner->GetReplaceSizeOffset(),
					m_owner->isReplaceSizeOffsetPercent(),
					m_owner->isReplacePreBorrow(),
					m_owner->GetReplacePreBorrowPrice(),
					-1,//Take the Parent's User Id//m_owner->GetReplaceUserId(),
					2,//displaySizeMode; 0 - size fraction, 1 - round lot, 2 - no change
					0,//displaySize
					0,//displaySizeFraction
					0);//remainingSize
				m_owner->SetReplaceSendId(sendId);
			}
		}
		break;

		case TMID_PASTE:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				const Order* order = ((OrderObserver*)GetItemDataPtr(sel))->GetOrder();
				m_owner->PasteOrder(order);
			}
		}
		break;

		default:
		return CListBox::OnCommand(wParam, lParam);
	}
	return TRUE;

}

void ListBoxOrder::SetHistoryLoaded(bool loaded)
{
	if(m_historyLoaded != loaded)
	{
		m_historyLoaded = loaded;
	}
}

void ListBoxOrder::SetExecutorConnected(bool connected)
{
	if(m_executorConnected != connected)
	{
		m_executorConnected = connected;
		if(!m_executorConnected)
		{
			SetHistoryLoaded(false);
		}
	}
}

void ListBoxOrder::SetAccount(const Account* account)
{
	if(m_account != account)
	{
		m_account = account;
		m_historyLoaded = m_account && m_account->isHistoryLoaded();
	}
}

void ListBoxOrder::SizeChanged(bool horizontalChange, bool verticalChange)
{
/*
	if(verticalChange)
	{
		m_oneSecondRect.bottom = m_listBoxHeight;
	}
*/
}

LbHorizontalScroller* ListBoxOrder::CreateHorizontalScroller()
{
	return new LbOrderHorizontalScroller(this);
}

/*
void ListBoxOrder::DoCreateColumnToolTips()
{
}
*/

void ListBoxOrder::ListBoxCreated()
{
	ColumnedListBox::ListBoxCreated();
	m_menuWidth = MeasureText("Replace", GetMenuFont());
}


void ListBoxOrder::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
	const OrderObserver* orderObserver = (const OrderObserver*)item;
	const Order* order = orderObserver->GetOrder();
	std::string info;
	char num[33];
	static const char y = 'Y';
	static const unsigned int al = DT_VCENTER|DT_NOCLIP|DT_SINGLELINE|DT_NOPREFIX;
	switch(columnId)
	{
		case OCI_SYMBOL:
		DrawText(hDC, order->GetSymbol(), -1, &r, alignment);
		return;

#ifndef TAKION_NO_OPTIONS
		case OCI_OPTION_CALLPUT:
		if(order->GetOptionBlock())
		{
			static const char c = 'C';
			static const char p = 'P';
			DrawText(hDC, order->isCall() ? &c : &p, 1, &r, alignment);
		}
		return;

		case OCI_OPTION_EXPIRATION:
		if(order->GetOptionBlock())
		{
			info.clear();
			U_AppendThreeCharDateToStr(info, order->GetExpirationDate(), '/', '\0', false, false);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		return;

		case OCI_OPTION_STRIKE_PRICE:
		if(order->GetOptionBlock())
		{
			info.clear();
			U_PriceToStr(info, order->GetStrikePrice(), 4, -1, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		return;
#endif
		case OCI_SIDE:
		DrawText(hDC, &order->GetSide(), 1, &r, alignment);
		return;

		case OCI_LIMIT_PRICE:
		if(!order->GetLimitPrice().isZero())
		{
			info.clear();
			U_PriceToStr(info, order->GetLimitPrice(), 4, -1, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		else
		{
			static const char* const market = "Market";
			DrawText(hDC, market, -1, &r, alignment);
		}
		return;

		case OCI_STOP_PRICE:
		if(!order->GetStopPrice().isZero())
		{
			info.clear();
			U_PriceToStr(info, order->GetStopPrice(), 4, -1, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		return;

		case OCI_SIZE:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(order->GetQuantity(), num, sizeof(num)), -1, &r, alignment);
		return;

		case OCI_VENUE:
		info.clear();
		U_AppendUnsignedNumberAsString(info, order->GetDestinationName());
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		return;

		case OCI_ROUTING:
		if(order->GetRoutingName())
		{
			info.clear();
			U_AppendUnsignedNumberAsString(info, order->GetRoutingName());
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		return;

		case OCI_TIF:
		info.clear();
		if(order->GetTifAsString(info, true))
		{
			DrawText(hDC, info.c_str(), -1, &r, DT_RIGHT | al);
		}
		else
		{
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		return;

		case OCI_TIME_CREATED:
		info.clear();
		U_AppendMillisecond(info, order->GetTimeCreated(), true);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		return;

		case OCI_CLIENT_ID:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(order->GetClientId(), num, sizeof(num)), -1, &r, alignment);
		return;

		case OCI_SEND_ID:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(order->GetSendId(), num, sizeof(num)), -1, &r, alignment);
		return;

		case OCI_PARENT_ID:
		if(order->GetParentId())
		{
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(order->GetParentId(), num, sizeof(num)), -1, &r, alignment);
		}
		return;

		case OCI_ISO:
		if(order->isIso())
		{
			DrawText(hDC, &y, 1, &r, alignment);
		}
		return;

		case OCI_REPLACING_NATIVELY:
		if(order->isReplacingNatively())
		{
			DrawText(hDC, &y, 1, &r, alignment);
		}
		return;
	}

	if(!m_inThread || orderObserver->LockInquiry())//If LockInquiry returns false, it means that currently the StockImbalance object is being modified and it will be signaled to repaint after the modification is done
	{
		std::string info;
		char num[33];
		switch(columnId)
		{
			case OCI_EXECUTED_PRICE:
			if(!orderObserver->GetExecutedPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, orderObserver->GetExecutedPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case OCI_PENDING_SIZE:
			if(orderObserver->GetSizePending())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(orderObserver->GetSizePending(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case OCI_EXECUTED_SIZE:
			if(orderObserver->GetSizeExecuted())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(orderObserver->GetSizeExecuted(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case OCI_VISIBLE_SIZE:
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(order->GetDisplayQuantity(), num, sizeof(num)), -1, &r, alignment);
			break;

			case OCI_STATUS:
			{
				const char* status;
				switch(orderObserver->GetStatus())
				{
					case OSE_UNACKNOWLEDGED:
					status = "Sent";
					break;

					case OSE_PENDING:
					status = "Pending";
					break;

					case OSE_PARTIALLY_FILLED:
					status = "filled";
					break;

					case OSE_FILLED://Order Dead
					status = orderObserver->GetSizeExecuted() < order->GetQuantity() ? "filled" : "FILLED";
					break;

					case OSE_UNFILLED://Order Dead
					status = order->GetReplacedByClientId() ? "Replaced" : "Canceled";
					break;

					case OSE_REJECTED:
					status = "Rejected";
					break;

					default:
					status = "Unknown";
					break;
				}
				DrawText(hDC, status, -1, &r, alignment);
			}
			break;

			case OCI_TIME_CANCEL_SENT:
			if(orderObserver->GetTimeCancelSent())
			{
				info.clear();
				U_AppendMillisecond(info, orderObserver->GetTimeCancelSent(), true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case OCI_TIME_DEAD:
			if(orderObserver->GetTimeDead())
			{
				info.clear();
				U_AppendMillisecond(info, orderObserver->GetTimeDead(), true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case OCI_SERVER_ID:
			if(orderObserver->GetServerId())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(orderObserver->GetServerId(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case OCI_REPLACED_CLIENT_ID:
			if(order->GetReplacedClientId())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(order->GetReplacedClientId(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case OCI_REPLACED_BY_CLIENT_ID:
			if(orderObserver->GetReplacedByClientId())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(orderObserver->GetReplacedByClientId(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case OCI_REPLACED_BY_SEND_ID:
			if(orderObserver->GetReplacedBySendId())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(orderObserver->GetReplacedBySendId(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case OCI_CLOSED_PNL:
			if(!orderObserver->GetClosedPnl().isZero())
			{
				info.clear();
				U_MoneyToStr(info, orderObserver->GetClosedPnl(), 4, 0, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case OCI_BP_USED:
			if(!orderObserver->GetBpUsed().isZero())
			{
				info.clear();
				U_MoneyToStr(info, orderObserver->GetBpUsed(), 4, 0, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case OCI_REJECT_CODE:
			if(orderObserver->GetRejectCode())
			{
				DrawText(hDC, U_UnsignedNumberToStrNoDecDigits(orderObserver->GetRejectCode(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case OCI_REJECT_TEXT:
			if(orderObserver->GetRejectText())
			{
				DrawText(hDC, orderObserver->GetRejectText(), -1, &r, alignment);
			}
			break;
		}
		if(m_inThread)orderObserver->UnlockInquiry();
	}
}

void ListBoxOrder::HorizontalScrollingDone(int diff)
{
}

enum SampleSettingOrderIds
{
	inThreadCheckBoxId = baseDialogSettingAccountIdsCount,

	groupPositionId,
	editSymbolId,
	buttonSymbolId,
//	staticSymbolNameId,
	staticSymbolId,

#ifndef TAKION_NO_OPTIONS
	staticOptionInfoId,
	checkBoxSymbolOptionId,
	radioSymbolOptionCallId,
	radioSymbolOptionPutId,
//	comboBoxSymbolOptionSuffixId,
	spinSymbolOptionStrikePriceId,
	datetimeSymbolOptionExpirationId,

//	staticSymbolOptionSuffixId,
	staticSymbolOptionStrikePriceId,
	staticSymbolOptionExpirationId,

	checkBoxSymbolFutureId,
#endif

	staticOrderStatusId,
	listBoxOrderStatusId,

	staticOrderSideId,
	listBoxOrderSideId,

	staticOrderInstrumentId,
	listBoxOrderInstrumentId,

	groupReplaceId,
	checkBoxReplaceMarketOrderId,
	staticReplacePriceOffsetId,
	checkBoxReplacePriceOffsetPercentId,
	checkBoxReplaceOrderPriceEnforceMinOffsetId,
	checkBoxReplaceSizeOffsetPercentId,
	spinReplacePriceOffsetId,
	staticReplaceSizeOffsetId,
	spinReplaceSizeOffsetId,
	checkBoxReplaceNativeId,
	checkBoxReplacePreBorrowId,
	staticReplacePreBorrowPriceId,
	spinReplacePreBorrowPriceId,
	staticReplaceSendIdNameId,
	staticReplaceSendIdId,

	staticLastSendIdDoneNameId,
	staticLastSendIdDoneId,

	listBoxOrderId,

	orderIdsCount,
};

const unsigned char decDigits = 4;
const unsigned char preBorrowDecDigits = 4;

//////////////////
SampleDialogSettingOrder::SampleDialogSettingOrder(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab)://, const Account* account):
	BaseDialogSettingAccount(mainWnd, parentTab, true, "Order", true, 64),
	m_position(nullptr),
	m_symbolCurrent(true),
	m_numericSymbol(0),
	m_controlNumericSymbol(0),
	m_orderStatusMask(0),
	m_orderSideMask(0),
	m_CheckBoxInThread("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_EditSymbol(U_GetValidStockChars(), false),

#ifndef TAKION_NO_OPTIONS
	m_CheckBoxOption("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_RadioOptionCall("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),//4, -3),
	m_RadioOptionPut("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),//4, -3),
	m_SpinOptionStrikePrice(99999999, 0, 0, 1000, 3, TakionSpin::charComma, 2),
	m_DateTimeOptionExpiration(447, 0xFFFFFFFF),
	m_CheckBoxFuture("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),

	m_optionBlock(0),
	m_optionBlockValid(false),
	m_optionMode(false),
	m_optionCall(true),
	m_expirationYear(0),
	m_expirationMonth(0),
	m_expirationDay(0),
	m_future(false),

	m_controlOptionBlock(0),
	m_controlOptionBlockValid(false),
	m_controlOptionMode(false),
	m_controlOptionCall(true),
	m_controlExpirationYear(0),
	m_controlExpirationMonth(0),
	m_controlExpirationDay(0),
	m_controlFuture(false),
#endif

	m_CheckBoxReplaceMarketOrder("Replace with Market Order", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxReplacePriceOffsetPercent("Price offset as percentage of the original Order Price", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxReplacePriceEnforceMinOffset("If Price Offset is not 0, enforce the price offset to be at least $0.01 for price over $1.00 and $0.0001 for price below $1.00", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_SpinReplacePriceOffset(100000000, -100000000, 0, 1000, 3, TakionSpin::charComma, decDigits),
	m_CheckBoxReplaceSizeOffsetPercent("Size offset as percentage of the original Order Size", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_SpinReplaceSizeOffset(100000, -100000, 0, 1000, 3, TakionSpin::charComma, 0),
	m_CheckBoxReplaceNative("Use a native Replace. Otherwise Algorithm \"Replacer\" is created which sends a request to cancel the Order(s), waits for the Cancel to confirm and sends the new Order(s)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxPreBorrow("Borrow Stock if necessary before sending a replacing Order", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_SpinReplacePreBorrowPrice(999999, 0, 0, 10000, 3, TakionSpin::charComma, preBorrowDecDigits),
	m_ListBoxOrder(this, m_account),

	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),

	m_replaceSizeOffset(0),
	m_replacePriceOffsetPercent(false),
	m_replacePriceEnforceMinOffset(false),
	m_replaceSizeOffsetPercent(false),
	m_replaceNative(false),
	m_replaceMarketOrder(false),
	m_replacePreBorrow(false),

	m_inThread(false)
{
	AddListBox(&m_ListBoxOrder);
	AddListBox(&m_ListBoxOrderSide);
	AddListBox(&m_ListBoxOrderStatus);
	AddListBox(&m_ListBoxOrderInstrument);

#ifndef TAKION_NO_OPTIONS
//	m_ComboBoxOptionSuffix.SetColorInvalid(m_visualBase->GetGrayColor());
	m_SpinOptionStrikePrice.AddSpinCell(1, false);
	m_SpinOptionStrikePrice.AddSpinCell(100, false);
	m_SpinOptionStrikePrice.AddSpinCell(10000, true);
#endif

	m_SpinReplacePriceOffset.AddSpinCell(1, false);
	m_SpinReplacePriceOffset.AddSpinCell(100, false);
//	m_SpinReplacePriceOffset.AddSpinCell(1000000, true);
//	m_SpinReplacePriceOffset.AddSpinCell(10000, true);

	m_SpinReplaceSizeOffset.AddSpinCell(1, false);
	m_SpinReplaceSizeOffset.AddSpinCell(100, false);
	m_SpinReplaceSizeOffset.AddSpinCell(10000, true);
	m_SpinReplaceSizeOffset.AddSpinCell(1000, true);

	m_SpinReplacePreBorrowPrice.AddSpinCell(1, false);
	m_SpinReplacePreBorrowPrice.AddSpinCell(100, false);
	m_SpinReplacePreBorrowPrice.AddSpinCell(10000, true);

	int w = m_accountWidth - hgap - hgap;
	const int halfWidth = w / 2;
	int x = 0;
	const int xc = x + hgap;
	int y = m_bottomControl + vgap;

	const int replaceGroupHeight = groupOffset
		+ checkBoxHeight + vgap
		+ staticHeight + spinHeight + vgap
		+ staticHeight + spinHeight + vgap
		+ checkBoxHeight + vgap
		+ checkBoxHeight + vgap

		+ checkBoxHeight
		+ spinHeight + vgap

		+ staticHeight + vgap;

	int w23 = w * 2 / 3 + hgap;

	AddButtonIndirect("In Thread", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, xc, y, w, checkBoxHeight, inThreadCheckBoxId);

	y += checkBoxHeight + vgap;
	int yStart = y;

	AddButtonIndirect("Replace", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, m_accountWidth, replaceGroupHeight, groupReplaceId);
	AddButtonIndirect("Market Order", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, xc, y += groupOffset, w, checkBoxHeight, checkBoxReplaceMarketOrderId);
	AddStaticIndirect("Price Offset", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += checkBoxHeight + vgap, w23, staticHeight, staticReplacePriceOffsetId);
	AddButtonIndirect("%", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc + w23, y, w - w23, checkBoxHeight, checkBoxReplacePriceOffsetPercentId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += staticHeight, w, spinHeight, spinReplacePriceOffsetId);
	AddButtonIndirect("Force MinOffset", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, xc, y += spinHeight, w, checkBoxHeight, checkBoxReplaceOrderPriceEnforceMinOffsetId);

	AddStaticIndirect("Size Offset", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += spinHeight + vgap, w, staticHeight, staticReplaceSizeOffsetId);
	AddButtonIndirect("%", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc + w23, y, w - w23, checkBoxHeight, checkBoxReplaceSizeOffsetPercentId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += staticHeight, w, spinHeight, spinReplaceSizeOffsetId);
	AddButtonIndirect("Native", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, xc, y += spinHeight + vgap, w, checkBoxHeight, checkBoxReplaceNativeId);
	AddButtonIndirect("Pre Borrow", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, xc, y += checkBoxHeight + vgap, w, checkBoxHeight, checkBoxReplacePreBorrowId);

	y += checkBoxHeight;
	AddStaticIndirect("¢", WS_VISIBLE | SS_LEFTNOWORDWRAP | SS_NOPREFIX | WS_GROUP, WS_EX_RIGHT, xc, y + (spinHeight - staticHeight) / 2, staticHeight, staticHeight, staticReplacePreBorrowPriceId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc + staticHeight, y, w - staticHeight, spinHeight, spinReplacePreBorrowPriceId);

	AddStaticIndirect("SendId", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += spinHeight + vgap, halfWidth, staticHeight, staticReplaceSendIdNameId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX | SS_RIGHT, 0, xc + halfWidth, y, halfWidth, staticHeight, staticReplaceSendIdId);

	y = yStart + replaceGroupHeight + vgap;
	AddStaticIndirect("DoneId", WS_VISIBLE | SS_NOPREFIX, 0, xc, y, halfWidth, staticHeight, staticLastSendIdDoneNameId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX | SS_RIGHT, 0, xc + halfWidth, y, halfWidth, staticHeight, staticLastSendIdDoneId);

	y += staticHeight + vgap + vgap + vgap;
	yStart = y;

	const int positionGroupHeight = groupOffset + editHeight + staticHeight
#ifndef TAKION_NO_OPTIONS
		+ staticHeight//option info
		+ vgap + checkBoxHeight//option
		+ vgap + checkBoxHeight//call put
		+ vgap + staticHeight//"Strike Price"
		+ spinHeight
		+ vgap + staticHeight//"Expiration"
		+ comboBoxHeight//Expiration

		+ vgap + staticHeight + checkBoxHeight //Future
#endif
		+ vgap;

	AddButtonIndirect("Position Filter", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, x, y, m_accountWidth, positionGroupHeight, groupPositionId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xc, y += groupOffset, w - editHeight, editHeight, editSymbolId);
	AddButtonIndirect("->", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc + w - editHeight, y, editHeight, editHeight, buttonSymbolId);
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += editHeight, w, staticHeight, staticSymbolId);

#ifndef TAKION_NO_OPTIONS
	AddStaticIndirect("", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += staticHeight, w, staticHeight, staticOptionInfoId);

	AddButtonIndirect("Option", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, y += staticHeight + vgap, w, checkBoxHeight, checkBoxSymbolOptionId);

	AddButtonIndirect("Call", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, m_xc, y += checkBoxHeight + vgap, halfWidth, checkBoxHeight, radioSymbolOptionCallId);
	AddButtonIndirect("Put", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, m_xc + halfWidth, y, halfWidth, checkBoxHeight, radioSymbolOptionPutId);

//	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL|WS_GROUP, 0, m_xc, y += checkBoxHeight + vgap, halfWidth, 400, comboBoxSymbolOptionSuffixId);
//	AddStaticIndirect("Suffix", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + halfWidth, y + (comboBoxHeight - staticHeight) / 2, halfWidth, staticHeight, staticSymbolOptionSuffixId);

	AddStaticIndirect("Strike Price", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y += checkBoxHeight + vgap, w, staticHeight, staticSymbolOptionStrikePriceId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += staticHeight, w, spinHeight, spinSymbolOptionStrikePriceId);

	AddStaticIndirect("Expiration", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y += spinHeight + vgap, w, staticHeight, staticSymbolOptionExpirationId);
	AddComponentIndirect("SysDateTimePick32", nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP | DTS_RIGHTALIGN, 0, m_xc, y += staticHeight, w, comboBoxHeight, datetimeSymbolOptionExpirationId);

	AddButtonIndirect("Future", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, y += comboBoxHeight + staticHeight + vgap, w, checkBoxHeight, checkBoxSymbolFutureId);
#endif
////////////
	y = yStart + positionGroupHeight + vgap;
//	yStart = y;
///
	AddStaticIndirect("Status Filter", WS_VISIBLE | SS_NOPREFIX, 0, xc, y, w, staticHeight, staticOrderStatusId);
	int lbHeight = FOST_Count * checkBoxHeight + 2 * vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y += staticHeight, w, lbHeight, listBoxOrderStatusId);

	AddStaticIndirect("Side Filter", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += lbHeight + vgap, w, staticHeight, staticOrderSideId);
	lbHeight = FOS_Count * checkBoxHeight + 2 * vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y += staticHeight, w, lbHeight, listBoxOrderSideId);

	AddStaticIndirect("Instrument", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += lbHeight + vgap, w, staticHeight, staticOrderInstrumentId);
	lbHeight = ST_COUNT * checkBoxHeight + 2 * vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y += staticHeight, w, lbHeight, listBoxOrderInstrumentId);
///
	y = staticHeight;
	x += w + hgap + hgap;
	w = m_width - x;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, w, m_height - staticHeight - y, listBoxOrderId);

#ifndef TAKION_NO_OPTIONS
	m_RadioOptionCall.SetRadioGroupOwner(&m_RadioGroupOption);
	m_RadioOptionPut.SetRadioGroupOwner(&m_RadioGroupOption);
#endif
}

HWND SampleDialogSettingOrder::GetFirstTabControl() const
{
	return m_ListBoxOrder.m_hWnd;
}

bool SampleDialogSettingOrder::OrderObserverStatusBelongs(const OrderObserver* orderObserver) const
{
	if(!orderObserver->GetSizePending())
	{
		if(orderObserver->GetSizeExecuted())
		{
			if(!(m_orderStatusMask & (1 << FOST_FILLED)))return false;
		}
		else
		{
			if(!(m_orderStatusMask & (1 << FOST_UNFILLED)))return false;
		}
	}
	else
	{
		if(orderObserver->GetSizeExecuted())
		{
			if(!(m_orderStatusMask & ((1 << FOST_FILLED) | (1 << FOST_PENDING))))return false;
		}
		else
		{
			if(!(m_orderStatusMask & (1 << FOST_PENDING)))return false;
		}
	}
	return true;
}

bool SampleDialogSettingOrder::OrderObserverBelongs(const OrderObserver* orderObserver) const
{
	if(!m_orderSideMask || !m_orderStatusMask)return false;

	const Order* order = orderObserver->GetOrder();
	const Position* orderPosition = order->GetPosition();
	if(!m_numericSymbol)//All orders of the account
	{
		if(orderPosition->GetAccount() != m_account)return false;
	}
	else
	{
		if(orderPosition != m_position)return false;
	}
	switch(order->GetSide())
	{
		case 'B':
		if(!(m_orderSideMask & (1 << FOS_BUY)))return false;
		break;

		case 'S':
		if(!(m_orderSideMask & (1 << FOS_SELL)))return false;
		break;

		case 'T':
		if(!(m_orderSideMask & (1 << FOS_SHORT)))return false;
		break;
	}

	return OrderObserverStatusBelongs(orderObserver);
}

bool SampleDialogSettingOrder::OrderBelongs(const Order* order) const
{
	if(order->isAlgorithm())return false;

	if(!m_orderSideMask || !m_orderStatusMask)return false;

	const Position* orderPosition = order->GetPosition();
	if(!m_numericSymbol)//All orders of the account
	{
		if(orderPosition->GetAccount() != m_account)return false;
	}
	else
	{
		if(orderPosition != m_position)return false;
	}
	switch(order->GetSide())
	{
		case 'B':
		if(!(m_orderSideMask & (1 << FOS_BUY)))return false;
		break;

		case 'S':
		if(!(m_orderSideMask & (1 << FOS_SELL)))return false;
		break;

		case 'T':
		if(!(m_orderSideMask & (1 << FOS_SHORT)))return false;
		break;
	}

	bool dead = order->isDead();
	bool filled = order->GetFilledQuantity() != 0;

	if(dead)
	{
		if(filled)
		{
			if(!(m_orderStatusMask & (1 << FOST_FILLED)))return false;
		}
		else
		{
			if(!(m_orderStatusMask & (1 << FOST_UNFILLED)))return false;
		}
	}
	else
	{
		if(filled)
		{
			if(!(m_orderStatusMask & ((1 << FOST_FILLED) | (1 << FOST_PENDING))))return false;
		}
		else
		{
			if(!(m_orderStatusMask & (1 << FOST_PENDING)))return false;
		}
	}
	return true;
}

void SampleDialogSettingOrder::UpdateSettings()
{
	BaseDialogSettingAccount::UpdateSettings();
/*
	int sel = m_ComboBoxAccount.GetCurSel();
	if(sel > 0 && m_account)
	{
		extensionConfig->m_orderAccountId.SetValue(m_account->GetId());
	}
	else
	{
		extensionConfig->m_orderAccountId.SetValue(std::string());
	}
*/
	extensionConfig->m_orderInThread.SetValue(m_CheckBoxInThread.GetCheck() == BST_CHECKED);

	extensionConfig->m_orderSymbol.SetValue((const char*)&m_numericSymbol);

#ifndef TAKION_NO_OPTIONS
	extensionConfig->m_orderOption.SetValue(m_optionMode);
	extensionConfig->m_orderOptionCall.SetValue(m_optionCall);
//	extensionConfig->m_orderOptionSuffix.SetValue(m_optionSuffix);
	extensionConfig->m_orderOptionStrikePrice.SetValue(m_strikePrice);
	extensionConfig->m_orderOptionExpiration.SetValue(GetExpirationDate());
	extensionConfig->m_orderFuture.SetValue(m_future);
#endif
	extensionConfig->m_orderStatusMask.SetValue(m_orderStatusMask);
	extensionConfig->m_orderSideMask.SetValue(m_orderSideMask);

	extensionConfig->m_orderReplacePriceOffsetPercent.SetValue(m_replacePriceOffsetPercent);
	extensionConfig->m_orderReplacePriceEnforceMinOffset.SetValue(m_replacePriceEnforceMinOffset);
	extensionConfig->m_orderReplaceSizeOffsetPercent.SetValue(m_replaceSizeOffsetPercent);
	extensionConfig->m_orderReplaceMarketOrder.SetValue(m_replaceMarketOrder);
	extensionConfig->m_orderReplaceNative.SetValue(m_replaceNative);
	extensionConfig->m_orderReplacePreBorrow.SetValue(m_replacePreBorrow);
	extensionConfig->m_orderReplacePreBorrowPrice.SetValue(m_replacePreBorrowPrice);

	extensionConfig->m_orderReplacePriceOffset.SetValue(m_replacePriceOffset);
	extensionConfig->m_orderReplaceSizeOffset.SetValue(m_replaceSizeOffset);

/*
	extensionConfig->m_msLoadOnStart.SetValue(m_CheckBoxLoadOnStart.GetCheck() == BST_CHECKED);
	CString path;
	m_EditPath.GetWindowText(path);
	extensionConfig->m_msStockFilePath.SetValue(std::string(path));

	extensionConfig->m_msSecondBack.SetValue(m_ListBoxStock.GetSecondBack());

	if(m_timersChanged)
	{
		m_timersChanged = false;
		extensionConfig->m_msTimerMap.Clear();
		const StockTimer* timer;
		for(unsigned int i = 0, end = (unsigned int)m_ListBoxTimer.GetCount(); i < end; ++i)
		{
			timer = (const StockTimer*)m_ListBoxTimer.GetItemDataPtr(i);
			extensionConfig->m_msTimerMap.AddValue(USecondSetting(timer->GetMillisecondFromMidnight()), UIntSetting(0));
		}
	}
*/
}

/*
void SampleDialogSettingOrder::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
}
void SampleDialogSettingOrder::Leaving()
{
//	TD_GetNewAccountObservable().RemoveObserver(this);
//	TD_GetAdminObservable().RemoveObserver(this);
}
*/

void SampleDialogSettingOrder::ResetOffsets()
{
	m_inThread = false;

	m_position = nullptr;
	m_symbolCurrent = true;
	m_numericSymbol = 0;
	m_controlNumericSymbol = 0;
#ifndef TAKION_NO_OPTIONS
	m_optionBlock = 0;
	m_optionBlockValid = false;
	m_optionMode = false;
	m_optionCall = true;
	m_expirationYear = m_expirationMonth = m_expirationDay = 0;
	m_future = false;
	m_strikePrice.SetZero();

	m_controlOptionBlock = 0;
	m_controlOptionBlockValid = false;
	m_controlOptionMode = false;
	m_controlOptionCall = true;
	m_controlExpirationYear = m_controlExpirationMonth = m_controlExpirationDay = 0;
	m_controlFuture = false;
	m_controlStrikePrice.SetZero();
#endif

	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;
//	SetAccount(nullptr);

	m_orderStatusMask = 0;
	m_orderSideMask = 0;

	m_replacePriceOffset.SetZero();
	m_replaceSizeOffset = 0;
	m_replacePriceOffsetPercent = false;
	m_replaceSizeOffsetPercent = false;
	m_replaceNative = false;
	m_replaceMarketOrder = false;
	m_replacePreBorrow = false;
	m_replacePreBorrowPrice.SetZero();

	m_SpinReplacePriceOffset.SetBkColorEnabled(0xFFFFFFFF);
	m_SpinReplacePreBorrowPrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
#ifndef TAKION_NO_OPTIONS
	m_CheckBoxOption.SetInvalid(true);
	m_RadioOptionCall.SetInvalid(true);
	m_RadioOptionPut.SetInvalid(true);
	m_CheckBoxFuture.SetInvalid(true);
#endif
}

void SampleDialogSettingOrder::BeforeDestroy()
{
	ResetOffsets();
//Done in the base class
//	TD_GetAdminObservable().RemoveObserver(this);
	TD_GetCustomExternalObservable().RemoveObserver(this);
	BaseDialogSettingAccount::BeforeDestroy();
}

void SampleDialogSettingOrder::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingAccount::ToolTipsEnabled(enable);
	m_ListBoxOrder.EnableTooltips(enable, m_toolTip);
	m_CheckBoxInThread.EnableTooltips(enable, m_toolTip);
#ifndef TAKION_NO_OPTIONS
	m_CheckBoxOption.EnableTooltips(enable, m_toolTip);
	m_RadioOptionCall.EnableTooltips(enable, m_toolTip);
	m_RadioOptionPut.EnableTooltips(enable, m_toolTip);
	m_CheckBoxFuture.EnableTooltips(enable, m_toolTip);
#endif
	m_CheckBoxReplacePriceEnforceMinOffset.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplaceMarketOrder.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplacePriceOffsetPercent.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplaceSizeOffsetPercent.EnableTooltips(enable, m_toolTip);
	m_CheckBoxReplaceNative.EnableTooltips(enable, m_toolTip);
	m_CheckBoxPreBorrow.EnableTooltips(enable, m_toolTip);
	m_ListBoxOrderStatus.EnableTooltips(enable, m_toolTip);
	m_ListBoxOrderSide.EnableTooltips(enable, m_toolTip);
	m_ListBoxOrderInstrument.EnableTooltips(enable, m_toolTip);
	m_SpinReplacePriceOffset.EnableTooltips(enable, m_toolTip);
	m_SpinReplaceSizeOffset.EnableTooltips(enable, m_toolTip);
	m_SpinReplacePreBorrowPrice.EnableTooltips(enable, m_toolTip);
}

BOOL SampleDialogSettingOrder::OnInitDialog()
{
	ResetOffsets();

//Done in the base class
//	TD_GetAdminObservable().AddObserver(this);//to get notified about connect / disconnect
	TD_GetCustomExternalObservable().AddObserver(this);

	BOOL ret = BaseDialogSettingAccount::OnInitDialog();

	m_CheckBoxInThread.ResizeToFitText(0);
#ifndef TAKION_NO_OPTIONS
	m_CheckBoxOption.ResizeToFitText(0);
	m_RadioOptionCall.ResizeToFitText(0);
	m_RadioOptionPut.ResizeToFitText(0);
	m_CheckBoxFuture.ResizeToFitText(0);
#endif
	m_CheckBoxReplaceMarketOrder.ResizeToFitText(0);
	m_CheckBoxReplacePriceOffsetPercent.ResizeToFitText(0);
	m_CheckBoxReplacePriceEnforceMinOffset.ResizeToFitText(0);
	m_CheckBoxReplaceSizeOffsetPercent.ResizeToFitText(0);
	m_CheckBoxReplaceNative.ResizeToFitText(0);
	m_CheckBoxPreBorrow.ResizeToFitText(0);

	m_EditSymbol.SetLimitText(7);
	EnableSymbolButton();

	SetToolTipsEnabled(true);
//	m_ListBoxOrder.EnableTooltips(true, m_toolTip);
	AddTitle(&m_ListBoxOrder);

//	RestoreLayout();

#ifndef TAKION_NO_OPTIONS
	m_RadioOptionPut.SetCheck(BST_CHECKED);

//	m_ComboBoxOptionSuffix.SetInvalid(true);
	m_SpinOptionStrikePrice.SetBkColorEnabled(m_visualBase->GetGrayColor());
	m_DateTimeOptionExpiration.SetBkColor(m_visualBase->GetGrayColor());

	UpdateOptionExpirationValue();
#endif
	m_CheckBoxReplacePriceEnforceMinOffset.SetInvalid(m_replaceMarketOrder || m_replacePriceOffset.isZero());
	return ret;
}

void SampleDialogSettingOrder::RepaintCount() const
{
	InvalidateTotalValueRect(listBoxOrderId, OCI_SYMBOL);
}

void SampleDialogSettingOrder::RepaintClosedPnl() const
{
	InvalidateTotalValueRect(listBoxOrderId, OCI_CLOSED_PNL);
}

void SampleDialogSettingOrder::RepaintBpUsed() const
{
	InvalidateTotalValueRect(listBoxOrderId, OCI_BP_USED);
}

void SampleDialogSettingOrder::RepaintSizePending() const
{
	InvalidateTotalValueRect(listBoxOrderId, OCI_PENDING_SIZE);
}

void SampleDialogSettingOrder::RepaintSizeExecuted() const
{
	InvalidateTotalValueRect(listBoxOrderId, OCI_EXECUTED_SIZE);
}

void SampleDialogSettingOrder::RepaintIsoCount() const
{
	InvalidateTotalValueRect(listBoxOrderId, OCI_ISO);
}

void SampleDialogSettingOrder::RepaintReplacingNativelyCount() const
{
	InvalidateTotalValueRect(listBoxOrderId, OCI_REPLACING_NATIVELY);
}

void SampleDialogSettingOrder::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingAccount::DoDataExchange(pDX);

	DDX_Control(pDX, inThreadCheckBoxId, m_CheckBoxInThread);

	DDX_Control(pDX, editSymbolId, m_EditSymbol);
	DDX_Control(pDX, buttonSymbolId, m_ButtonSymbol);
	DDX_Control(pDX, staticSymbolId, m_StaticSymbol);

#ifndef TAKION_NO_OPTIONS
	DDX_Control(pDX, staticOptionInfoId, m_StaticOptionInfo);
	DDX_Control(pDX, checkBoxSymbolOptionId, m_CheckBoxOption);
	DDX_Control(pDX, radioSymbolOptionCallId, m_RadioOptionCall);
	DDX_Control(pDX, radioSymbolOptionPutId, m_RadioOptionPut);
//	DDX_Control(pDX, comboBoxSymbolOptionSuffixId, m_ComboBoxOptionSuffix);
	DDX_Control(pDX, spinSymbolOptionStrikePriceId, m_SpinOptionStrikePrice);
	DDX_Control(pDX, datetimeSymbolOptionExpirationId, m_DateTimeOptionExpiration);
	DDX_Control(pDX, checkBoxSymbolFutureId, m_CheckBoxFuture);

//	DDX_Control(pDX, groupSymbolOptionId, m_GroupOption);
//	DDX_Control(pDX, staticSymbolOptionSuffixId, m_StaticOptionSuffix);
	DDX_Control(pDX, staticSymbolOptionStrikePriceId, m_StaticOptionStrikePrice);
	DDX_Control(pDX, staticSymbolOptionExpirationId, m_StaticOptionExpiration);
#endif

	DDX_Control(pDX, listBoxOrderStatusId, m_ListBoxOrderStatus);
	DDX_Control(pDX, listBoxOrderSideId, m_ListBoxOrderSide);
	DDX_Control(pDX, listBoxOrderInstrumentId, m_ListBoxOrderInstrument);

	DDX_Control(pDX, staticReplacePriceOffsetId, m_StaticReplacePriceOffset);
	DDX_Control(pDX, checkBoxReplaceMarketOrderId, m_CheckBoxReplaceMarketOrder);
	DDX_Control(pDX, checkBoxReplacePriceOffsetPercentId, m_CheckBoxReplacePriceOffsetPercent);
	DDX_Control(pDX, checkBoxReplaceOrderPriceEnforceMinOffsetId, m_CheckBoxReplacePriceEnforceMinOffset);
	DDX_Control(pDX, checkBoxReplaceSizeOffsetPercentId, m_CheckBoxReplaceSizeOffsetPercent);
	DDX_Control(pDX, spinReplacePriceOffsetId, m_SpinReplacePriceOffset);
	DDX_Control(pDX, spinReplaceSizeOffsetId, m_SpinReplaceSizeOffset);
	DDX_Control(pDX, checkBoxReplaceNativeId, m_CheckBoxReplaceNative);

	DDX_Control(pDX, checkBoxReplacePreBorrowId, m_CheckBoxPreBorrow);
	DDX_Control(pDX, spinReplacePreBorrowPriceId, m_SpinReplacePreBorrowPrice);

	DDX_Control(pDX, staticReplaceSendIdId, m_StaticReplaceSendId);

	DDX_Control(pDX, staticLastSendIdDoneId, m_StaticLastSendIdDone);

	DDX_Control(pDX, listBoxOrderId, m_ListBoxOrder);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingOrder, BaseDialogSettingAccount)
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(inThreadCheckBoxId, OnInThread)

	ON_EN_CHANGE(editSymbolId, OnSymbolChange)
	ON_EN_UPDATE(editSymbolId, OnSymbolUpdate)
	ON_BN_CLICKED(buttonSymbolId, OnSymbol)
	ON_LBN_SELCHANGE(listBoxOrderId, OnSelchangeOrder)
	ON_LBN_DBLCLK(listBoxOrderId, OnDblclkOrder)
//	ON_CBN_SELCHANGE(comboBoxAccountId, OnSelchangeAccount)

	ON_BN_CLICKED(checkBoxReplaceMarketOrderId, OnReplaceMarketOrder)
	ON_BN_CLICKED(checkBoxReplacePriceOffsetPercentId, OnReplacePriceOffsetPercent)
	ON_BN_CLICKED(checkBoxReplaceOrderPriceEnforceMinOffsetId, OnReplacePriceEnforceMinOffset)
	ON_BN_CLICKED(checkBoxReplaceSizeOffsetPercentId, OnReplaceSizeOffsetPercent)
	ON_BN_CLICKED(checkBoxReplaceNativeId, OnReplaceNative)
	ON_BN_CLICKED(checkBoxReplacePreBorrowId, OnPreBorrow)

#ifndef TAKION_NO_OPTIONS
	ON_BN_CLICKED(checkBoxSymbolOptionId, OnOption)
	ON_BN_CLICKED(radioSymbolOptionCallId, OnOptionCall)
	ON_BN_CLICKED(radioSymbolOptionPutId, OnOptionPut)
//	ON_CBN_SELCHANGE(comboBoxSymbolOptionSuffixId, OnSelchangeOptionSuffix)
	ON_NOTIFY(DTN_DROPDOWN, datetimeSymbolOptionExpirationId, OnCalendarDropdown)
	ON_NOTIFY(DTN_CLOSEUP, datetimeSymbolOptionExpirationId, OnCalendarCloseup)
	ON_NOTIFY(DTN_DATETIMECHANGE, datetimeSymbolOptionExpirationId, OnExpirationDateChange)
	ON_BN_CLICKED(checkBoxSymbolFutureId, OnFuture)
#endif
	ON_MESSAGE(WM_USER + 15, OnSpinChange)

	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)
END_MESSAGE_MAP()

void SampleDialogSettingOrder::OnInThread()
{
	SetInThread(m_CheckBoxInThread.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingOrder::AddAccountObserver()
{
	if(m_account)
	{
		if(m_inThread)
		{
			m_account->AddInThreadObserver(this);
		}
		else
		{
			m_account->AddObserver(this);
		}
	}
}

void SampleDialogSettingOrder::RemoveAccountObserver()
{
	if(m_account)
	{
		if(m_inThread)
		{
			m_account->RemoveInThreadObserver(this);
		}
		else
		{
			m_account->RemoveObserver(this);
		}
	}
}

void SampleDialogSettingOrder::SetInThread(const bool inThread)
{
	if(inThread != m_inThread)
	{
		m_inThread = inThread;
		bool accountObserver = !m_numericSymbol
#ifndef TAKION_NO_OPTIONS
			|| m_optionMode && !m_optionBlockValid
#endif
			;

		if(accountObserver)
		{
			if(m_inThread)
			{
				m_account->AddInThreadObserver(this);
				m_account->RemoveObserver(this);
			}
			else
			{
				m_account->AddObserver(this);
				m_account->RemoveInThreadObserver(this);
			}
		}
		else if(m_position)
		{
			if(m_inThread)
			{
				m_position->AddInThreadObserver(this);
				m_position->RemoveObserver(this);
			}
			else
			{
				m_position->AddObserver(this);
				m_position->RemoveInThreadObserver(this);
			}
		}

		m_ListBoxOrder.SetInThread(m_inThread);
		m_modified = true;
	}
}

#ifndef TAKION_NO_OPTIONS

void SampleDialogSettingOrder::UpdateCallPutColor()
{
	if(m_controlNumericSymbol != 0 && isControlOptionUsed())
	{
		m_RadioOptionCall.SetBackgroundColor(m_controlOptionCall != m_optionCall && m_controlOptionCall ? m_visualBase->GetPendingColor() : 0xFFFFFFFF);
		m_RadioOptionCall.SetInvalid(false);
		m_RadioOptionPut.SetBackgroundColor(m_controlOptionCall != m_optionCall && !m_controlOptionCall ? m_visualBase->GetPendingColor() : 0xFFFFFFFF);
		m_RadioOptionPut.SetInvalid(false);
/*
		if(m_controlOptionCall != m_optionCall && m_controlOptionCall)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
*/
	}
	else
	{
		m_RadioOptionCall.SetInvalid(true);
		m_RadioOptionPut.SetInvalid(true);
		m_RadioOptionCall.SetBackgroundColor(0xFFFFFFFF);
		m_RadioOptionPut.SetBackgroundColor(0xFFFFFFFF);
/*
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_visualBase->GetGrayColor());
		return *m_bkBrush;
*/
	}
}

void SampleDialogSettingOrder::RepaintOptionCount() const
{
	InvalidateTotalValueRect(listBoxOrderId, OCI_OPTION_CALLPUT);
}

void SampleDialogSettingOrder::UpdateControlOptionBlock()
{
	m_controlOptionBlock = CalculateCurrentOptionBlock();
//	m_controlOptionBlockValid = !m_controlOptionBlock || OptionKey::isOptionBlockValid(m_controlOptionBlock);
	m_controlOptionBlockValid = m_controlOptionMode && OptionKey::isOptionBlockValid(m_controlOptionBlock);

	UpdateCallPutColor();
}

unsigned __int64 SampleDialogSettingOrder::CalculateCurrentOptionBlock() const
{
	if(m_controlOptionMode)
	{
		OptionKey key(m_controlStrikePrice.GetCompactPrice(),
			m_controlOptionCall,
			'\0',//const char& suffix,
			m_controlExpirationDay,
			m_controlExpirationMonth,
			m_controlExpirationYear);
		return key.CalculateOptionBlock();
	}
	return 0;
}

void SampleDialogSettingOrder::HideOptionControls(bool hide)
{
	int intHide = hide ? SW_HIDE : SW_SHOW;
	m_StaticOptionInfo.ShowWindow(intHide);
	m_CheckBoxOption.ShowWindow(intHide);
	m_RadioOptionCall.ShowWindow(intHide);
	m_RadioOptionPut.ShowWindow(intHide);
//	m_ComboBoxOptionSuffix.ShowWindow(intHide);
	m_SpinOptionStrikePrice.ShowWindow(intHide);
	m_DateTimeOptionExpiration.ShowWindow(intHide);

//	m_GroupOption.ShowWindow(intHide);
//	m_StaticOptionSuffix.ShowWindow(intHide);
	m_StaticOptionStrikePrice.ShowWindow(intHide);
	m_StaticOptionExpiration.ShowWindow(intHide);

	m_CheckBoxFuture.ShowWindow(intHide);
}

void SampleDialogSettingOrder::OnCalendarDropdown(NMHDR* pNMHDR, LRESULT* pResult)
{
	UGUI_IdleInterrupt(true, 4);
	*pResult = 0;
}

void SampleDialogSettingOrder::OnCalendarCloseup(NMHDR* pNMHDR, LRESULT* pResult)
{
	UGUI_IdleInterrupt(false, 4);
	*pResult = 0;
}

bool SampleDialogSettingOrder::GetOptionValues(unsigned __int64& rootSymbol, unsigned __int64& underlierSymbol, unsigned __int64& optionBlock, unsigned short& optionType, unsigned char& securityType) const
{
	if(AreSettingsValid())
	{
		optionType = 0;
		rootSymbol = m_numericSymbol;
		optionBlock = m_optionBlock;
		securityType = optionBlock ? ST_OPTION : m_future ? ST_FUTURE : ST_STOCK;
/*
		if(isOptionUsed())
		{

			if(m_optionSuffix)
			{
				underlierSymbol = rootSymbol;
				OptionKey::AddSuffixToSymbol(rootSymbol, m_optionSuffix);
			}
			else
			{
				underlierSymbol = 0;
			}

			OptionKey optionKey(m_strikePrice.GetCompactPrice(),
				m_optionCall,
				'\0',//m_optionSuffix,
				m_expirationDay,
				m_expirationMonth,
				m_expirationYear);
			optionBlock = optionKey.CalculateOptionBlock();
		}
		else
		{
			underlierSymbol = 0;
			optionBlock = 0;
		}
*/
		return true;
	}
	return false;
}

void SampleDialogSettingOrder::OnExpirationDateChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(UpdateOptionExpirationValue())
	{
//		m_modified = true;//??
	}
	*pResult = 0;
}

bool SampleDialogSettingOrder::UpdateOptionExpirationValue()
{
	bool ret = false;
	SYSTEMTIME d;
	m_DateTimeOptionExpiration.GetTime(&d);
	unsigned char token = (unsigned char)d.wMonth;
	if(token != m_controlExpirationMonth)
	{
		m_controlExpirationMonth = token;
		ret = true;
	}
	token = (unsigned char)d.wDay;
	if(token != m_controlExpirationDay)
	{
		m_controlExpirationDay = token;
		ret = true;
	}
	token = (unsigned char)(d.wYear % 100);
	if(token != m_controlExpirationYear)
	{
		m_controlExpirationYear = token;
		ret = true;
	}
	if(ret)
	{
		OptionValuesChanged();
		if(m_controlNumericSymbol && isControlOptionUsed())
		{
/*
			m_SpinOptionStrikePrice.SetBkColorEnabled(m_controlStrikePrice.isZero() ? m_visualBase->GetWarningColor():
				m_controlStrikePrice != m_strikePrice ? m_visualBase->GetPendingColor():
				0xFFFFFFFF);
*/
			m_DateTimeOptionExpiration.SetBkColor(isExpirationDateCurrent() ? GetSysColor(COLOR_WINDOW):
				m_visualBase->GetPendingColor());
		}
	}
	return ret;
}

void SampleDialogSettingOrder::SetOptionCall(const bool call)
{
	if(call != m_controlOptionCall)
	{
		m_controlOptionCall = call;
//		m_modified = true;
		OptionValuesChanged();

//		m_RadioOptionCall.Invalidate(FALSE);
//		m_RadioOptionPut.Invalidate(FALSE);
	}
}

void SampleDialogSettingOrder::OnOptionCall()
{
	SetOptionCall(m_RadioOptionCall.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingOrder::OnOptionPut()
{
	SetOptionCall(m_RadioOptionPut.GetCheck() == BST_UNCHECKED);
}

void SampleDialogSettingOrder::OnFuture()
{
	const bool futureMode = m_CheckBoxFuture.GetCheck() == BST_CHECKED;
	if(futureMode != m_controlFuture)
	{
		m_controlFuture = futureMode;
//		m_modified = true;
		FutureModeChanged();
//		m_CheckBoxFuture.Invalidate(FALSE);
	}
}

void SampleDialogSettingOrder::OnOption()
{
	const bool optionMode = m_CheckBoxOption.GetCheck() == BST_CHECKED;
	if(optionMode != m_controlOptionMode)
	{
		m_controlOptionMode = optionMode;
//		m_modified = true;
		OptionModeChanged();

//		m_CheckBoxFuture.Invalidate(FALSE);
	}
}

void SampleDialogSettingOrder::SetCallPut(bool call)
{
	if(m_RadioOptionCall.m_hWnd)
	{
		if(call)
		{
			m_RadioOptionPut.SetCheck(BST_UNCHECKED);
			m_RadioOptionCall.SetCheck(BST_CHECKED);
		}
		else
		{
			m_RadioOptionCall.SetCheck(BST_UNCHECKED);
			m_RadioOptionPut.SetCheck(BST_CHECKED);
		}
		OnOptionCall();
	}
}

void SampleDialogSettingOrder::SetStrikePrice(const Price& strikePrice)
{
	if(strikePrice != m_controlStrikePrice)
	{
		m_controlStrikePrice = strikePrice;
		FillSpinControlFromPrice(strikePrice, m_SpinOptionStrikePrice, 100, false);
	}
}

void SampleDialogSettingOrder::SetExpirationDate(unsigned int date)
{
//	unsigned int date = extensionConfig->m_positionActionOptionExpiration.GetValue();
	unsigned char expirationDay;// = (unsigned char)(date - token * 100);
	unsigned char expirationMonth;
	unsigned char expirationYear;
	if(!date)
	{
		unsigned short year;
//		unsigned char month;
//		unsigned char day;
		U_GetDateTimeTokensDateOnly(TL_GetTodaysDateInShiftFormat(), year, expirationMonth, expirationDay);
		expirationYear = year % 100;
	}
	else
	{
		unsigned int token = date / 100;
		expirationDay = (unsigned char)(date - token * 100);
		date = token;
		token = date / 100;
		expirationMonth = (unsigned char)(date - token * 100);
		expirationYear = (unsigned char)token;
	}
	SetExpirationDate(expirationYear, expirationMonth, expirationDay);
}

void SampleDialogSettingOrder::SetExpirationDate(const unsigned char expirationYear, const unsigned char expirationMonth, const unsigned char expirationDay)
{
	if(expirationDay != m_controlExpirationDay || m_controlExpirationMonth != m_controlExpirationMonth || expirationYear != m_controlExpirationYear)
	{
		m_controlExpirationDay = expirationDay;
		m_controlExpirationMonth = expirationMonth;
		m_controlExpirationYear = expirationYear;
		if(m_DateTimeOptionExpiration.m_hWnd)
		{
			SYSTEMTIME d;
			d.wDay = m_controlExpirationDay;
			d.wMonth = m_controlExpirationMonth;
			d.wYear = 2000 + m_controlExpirationYear;
			d.wDayOfWeek = d.wHour = d.wMilliseconds = d.wMinute = d.wSecond = 0;
			m_DateTimeOptionExpiration.SetTime(&d);
		}
	}
}

//void BaseSampleDialogSettingOption::SetOptionValues(unsigned int date)
void SampleDialogSettingOrder::SetOptionValues(bool useOption,
//	char suffix,
	bool call, const Price& strikePrice, unsigned int expirationDate)
{
	SetCallPut(call);//extensionConfig->m_mdOptionCall.GetValue());
/*	
	int found = m_ComboBoxOptionSuffix.FindItem((const void*)suffix);//extensionConfig->m_mdOptionSuffix.GetValue());
	m_ComboBoxOptionSuffix.SetCurSel(found >= 0 ? found : 0);
	OnSelchangeOptionSuffix();
*/
	SetStrikePrice(strikePrice);//extensionConfig->m_mdOptionStrikePrice.GetValue());

	if(expirationDate)
	{
		SetExpirationDate(expirationDate);//extensionConfig->m_mdOptionExpiration.GetValue());
	}

	m_CheckBoxOption.SetCheck(useOption ? BST_CHECKED : BST_UNCHECKED);//extensionConfig->m_mdOption.GetValue() ? BST_CHECKED : BST_UNCHECKED);
	OnOption();
}

bool SampleDialogSettingOrder::isExpirationValid() const
{
	return !TL_IsDatePassed((unsigned int)m_expirationYear + 2000, m_expirationMonth, m_expirationDay);
}

bool SampleDialogSettingOrder::isControlExpirationValid() const
{
	return !TL_IsDatePassed((unsigned int)m_controlExpirationYear + 2000, m_controlExpirationMonth, m_controlExpirationDay);
}

bool SampleDialogSettingOrder::AreOptionSettingsValid() const
{
//	return !m_optionMode || (!m_strikePrice.isZero() && isExpirationValid());
	return !m_controlOptionMode || !m_controlStrikePrice.isZero();
}

bool SampleDialogSettingOrder::AreSettingsValid() const
{
	return
//		BaseSampleDialogSettingSymbol::AreSettingsValid() &&
		AreOptionSettingsValid();
}

void SampleDialogSettingOrder::OptionValuesChanged()
{
	UpdateControlOptionBlock();
	EnableSymbolButton();
}

bool SampleDialogSettingOrder::InvalidateOptionControls()
{
	const bool option = isControlOptionUsed();
//	m_CheckBoxOption.Invalidate(FALSE);
	m_CheckBoxOption.SetBackgroundColor(!m_controlNumericSymbol || m_controlOptionMode == m_optionMode ? 0xFFFFFFFF : m_visualBase->GetPendingColor());

	m_CheckBoxOption.SetInvalid(!m_controlNumericSymbol);

	m_SpinOptionStrikePrice.SetBkColorEnabled(option && m_controlNumericSymbol ?
		m_controlStrikePrice.isZero() ? m_visualBase->GetWarningColor():
		m_controlStrikePrice != m_strikePrice ? m_visualBase->GetPendingColor():
			0xFFFFFFFF://GetSysColor(COLOR_WINDOW):
		m_visualBase->GetGrayColor());

	m_DateTimeOptionExpiration.SetBkColor(option && m_controlNumericSymbol ?
		isExpirationDateCurrent() ? GetSysColor(COLOR_WINDOW):
			m_visualBase->GetPendingColor():
		m_visualBase->GetGrayColor());

	const bool notOption = !m_controlNumericSymbol || !option;
	m_RadioOptionCall.SetInvalid(notOption);// Invalidate(FALSE);
	m_RadioOptionPut.SetInvalid(notOption);//Invalidate(FALSE);

	m_CheckBoxFuture.SetBackgroundColor(m_controlFuture == m_future ? 0xFFFFFFFF : m_visualBase->GetPendingColor());
	m_CheckBoxFuture.SetInvalid(!m_controlNumericSymbol || m_controlOptionMode);//Invalidate(FALSE);
	return option;
}

bool SampleDialogSettingOrder::OptionModeChanged()
{
//	InvalidateOptionControls();

	OptionValuesChanged();

	return InvalidateOptionControls();
}

bool SampleDialogSettingOrder::FutureModeChanged()
{
//	OptionValuesChanged();
//	m_CheckBoxFuture.SetBackgroundColor(!m_controlNumericSymbol || m_controlOptionMode || m_controlFuture == m_future ? 0xFFFFFFFF : m_visualBase->GetPendingColor());
	if(m_controlNumericSymbol != 0 && !m_controlOptionMode)
	{
		m_CheckBoxFuture.SetBackgroundColor(m_controlFuture == m_future ? 0xFFFFFFFF : m_visualBase->GetPendingColor());
//		if(m_controlFuture != m_future)
//		{
//			pDC->SetBkMode(TRANSPARENT);
//			return *m_visualBase->GetPendingBrush();
//		}
		m_CheckBoxFuture.SetInvalid(false);
	}
	else
	{
		m_CheckBoxFuture.SetBackgroundColor(0xFFFFFFFF);
		m_CheckBoxFuture.SetInvalid(true);
//		pDC->SetBkMode(TRANSPARENT);
//		pDC->SetTextColor(m_visualBase->GetGrayColor());
//		return *m_bkBrush;
	}
	EnableSymbolButton();

	return m_controlFuture;
}

#endif

void SampleDialogSettingOrder::OnDblclkOrder()
{
	int sel = m_ListBoxOrder.GetCurSel();
	if(sel >= 0)
	{
		const Order* order = ((OrderObserver*)m_ListBoxOrder.GetItemDataPtr(sel))->GetOrder();
		PasteOrder(order);
	}
}

void SampleDialogSettingOrder::PasteOrder(const Order* order)
{
	m_EditSymbol.SetWindowText(order->GetSymbol());
#ifndef TAKION_NO_OPTIONS
	m_CheckBoxFuture.SetCheck(order->GetSecurityType() == ST_FUTURE);
	if(order->GetOptionBlock())
	{
		m_CheckBoxOption.SetCheck(BST_CHECKED);
		OnOption();
		if(order->isCall())
		{
			m_RadioOptionPut.SetCheck(BST_UNCHECKED);
			m_RadioOptionCall.SetCheck(BST_CHECKED);
			SetOptionCall(true);
		}
		else
		{
			m_RadioOptionCall.SetCheck(BST_UNCHECKED);
			m_RadioOptionPut.SetCheck(BST_CHECKED);
			SetOptionCall(false);
		}
		const Price strikePrice(order->GetStrikePrice());
		const unsigned int expiration = order->GetExpirationDate();
		FillSpinControlFromPrice(strikePrice, m_SpinOptionStrikePrice, 100, true);

//		U_FormatDateTokensStr((unsigned char)(date >> 8), (unsigned char)date, (unsigned short)2000 + (unsigned char)(date >> 16), str, tokenDel, ' ', monthName, fullYear);
		SetExpirationDate((unsigned char)(expiration >> 16), (unsigned char)(expiration >> 8), (unsigned char)expiration);
//		SetExpirationDate(expiration);
	}
	else
	{
		m_CheckBoxOption.SetCheck(BST_UNCHECKED);
		OnOption();
/*
		if(order->isCall())
		{
			m_RadioOptionPut.SetCheck(BST_UNCHECKED);
			m_RadioOptionCall.SetCheck(BST_CHECKED);
			SetOptionCall(true);
		}
		else
		{
			m_RadioOptionCall.SetCheck(BST_UNCHECKED);
			m_RadioOptionPut.SetCheck(BST_CHECKED);
			SetOptionCall(false);
		}
		const Price strikePrice(order->GetStrikePrice());
		const unsigned int expiration = order->GetExpirationDate();
		FillSpinControlFromPrice(strikePrice, m_SpinOptionStrikePrice, 100, true);

//		U_FormatDateTokensStr((unsigned char)(date >> 8), (unsigned char)date, (unsigned short)2000 + (unsigned char)(date >> 16), str, tokenDel, ' ', monthName, fullYear);
		SetExpirationDate((unsigned char)(expiration >> 16), (unsigned char)(expiration >> 8), (unsigned char)expiration);
*/
	}
#endif
}

void SampleDialogSettingOrder::SetReplaceSendId(unsigned int sendId)
{
	char num[33];
	m_StaticReplaceSendId.SetWindowText(U_UnsignedNumberToStrNoDecDigits(sendId, num, sizeof(num)));
}

void SampleDialogSettingOrder::SetLastSendIdDone(unsigned int sendId)
{
	char num[33];
	m_StaticLastSendIdDone.SetWindowText(U_UnsignedNumberToStrNoDecDigits(sendId, num, sizeof(num)));
}

void SampleDialogSettingOrder::OnReplacePriceOffsetPercent()
{
	const bool checked = m_CheckBoxReplacePriceOffsetPercent.GetCheck() == BST_CHECKED;
	if(checked != m_replacePriceOffsetPercent)
	{
		m_replacePriceOffsetPercent = checked;
	}
}

void SampleDialogSettingOrder::OnReplacePriceEnforceMinOffset()
{
	const bool checked = m_CheckBoxReplacePriceEnforceMinOffset.GetCheck() == BST_CHECKED;
	if(checked != m_replacePriceEnforceMinOffset)
	{
		m_replacePriceEnforceMinOffset = checked;
	}
}

void SampleDialogSettingOrder::OnReplaceSizeOffsetPercent()
{
	const bool checked = m_CheckBoxReplaceSizeOffsetPercent.GetCheck() == BST_CHECKED;
	if(checked != m_replaceSizeOffsetPercent)
	{
		m_replaceSizeOffsetPercent = checked;
	}
}

void SampleDialogSettingOrder::OnReplaceNative()
{
	const bool checked = m_CheckBoxReplaceNative.GetCheck() == BST_CHECKED;
	if(checked != m_replaceNative)
	{
		m_replaceNative = checked;
	}
}

void SampleDialogSettingOrder::OnPreBorrow()
{
	const bool checked = m_CheckBoxPreBorrow.GetCheck() == BST_CHECKED;
	if(checked != m_replacePreBorrow)
	{
		m_replacePreBorrow = checked;
		m_SpinReplacePreBorrowPrice.SetBkColorEnabled(checked ? 0xFFFFFFFF : m_visualBase->GetGrayColor());
	}
}

void SampleDialogSettingOrder::OnReplaceMarketOrder()
{
	const bool checked = m_CheckBoxReplaceMarketOrder.GetCheck() == BST_CHECKED;
	if(checked != m_replaceMarketOrder)
	{
		m_replaceMarketOrder = checked;
		m_SpinReplacePriceOffset.SetBkColorEnabled(checked ? m_visualBase->GetGrayColor() : 0xFFFFFFFF);
		m_StaticReplacePriceOffset.Invalidate(FALSE);
//		m_CheckBoxReplacePriceOffsetPercent.Invalidate(FALSE);
		m_CheckBoxReplacePriceOffsetPercent.SetInvalid(m_replaceMarketOrder);

		m_CheckBoxReplacePriceEnforceMinOffset.SetInvalid(m_replaceMarketOrder || m_replacePriceOffset.isZero());
	}
}

LRESULT SampleDialogSettingOrder::OnSpinChange(WPARAM spinId, LPARAM l)
{
	switch(spinId)
	{
#ifndef TAKION_NO_OPTIONS
		case spinSymbolOptionStrikePriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinOptionStrikePrice, 100);
//			bool changed = isControlOptionUsed() && m_controlStrikePrice.isZero() != price.isZero();
//			if(changed)m_SpinOptionStrikePrice.SetBkColorEnabled(m_controlStrikePrice.isZero() ?
//					0xFFFFFFFF://GetSysColor(COLOR_WINDOW):
//				m_visualBase->GetWarningColor());
			if(price != m_controlStrikePrice)
			{
				m_controlStrikePrice = price;
//				m_modified = true;
				if(m_controlNumericSymbol && isControlOptionUsed())
				{
/*
					m_SpinOptionStrikePrice.SetBkColorEnabled(isControlOptionUsed() ?
						m_controlStrikePrice.isZero() ? m_visualBase->GetWarningColor():
						m_controlStrikePrice != m_strikePrice ? m_visualBase->GetPendingColor():
							0xFFFFFFFF://GetSysColor(COLOR_WINDOW):
						m_visualBase->GetGrayColor());
*/
					m_SpinOptionStrikePrice.SetBkColorEnabled(m_controlStrikePrice.isZero() ? m_visualBase->GetWarningColor():
						m_controlStrikePrice != m_strikePrice ? m_visualBase->GetPendingColor():
						0xFFFFFFFF);
				}
				OptionValuesChanged();
			}
		}
		break;
#endif
		case spinReplacePriceOffsetId:
		FillSignedPriceFromSpinControl(m_replacePriceOffset, m_SpinReplacePriceOffset, U_GetDivider<unsigned int>(decDigits));
		m_CheckBoxReplacePriceEnforceMinOffset.SetInvalid(m_replaceMarketOrder || m_replacePriceOffset.isZero());
		m_modified = true;
		break;

		case spinReplaceSizeOffsetId:
		m_replaceSizeOffset = m_SpinReplaceSizeOffset.GetValue();
		m_modified = true;
		break;

		case spinReplacePreBorrowPriceId:
		FillPriceFromSpinControl(m_replacePreBorrowPrice, m_SpinReplacePreBorrowPrice, U_GetDivider<unsigned int>(preBorrowDecDigits));
		m_modified = true;
		break;

/*
		default:
		BaseSampleDialogSettingSymbol::OnSpinChange(w, l);
		break;
*/
	}
	return 0;
}

void SampleDialogSettingOrder::AddOrderByFilter(unsigned char addStatusFilter, unsigned char addSideFilter, unsigned char addInstrumentFilter)
{
	if(addStatusFilter && addSideFilter && addInstrumentFilter && m_account)
	{
		TakionIterator* iterator;
		bool symbolEmpty = !m_numericSymbol;
		if(symbolEmpty)//whole account
		{
			iterator = m_account->CreateOrderIterator(CalculateTakionOrderStatusMask(addStatusFilter), CalculateTakionOrderSideMask(addSideFilter));
		}
		else if(m_position)
		{
			iterator = m_position->CreateOrderIterator(CalculateTakionOrderStatusMask(addStatusFilter), CalculateTakionOrderSideMask(addSideFilter));
		}
		else
		{
			iterator = nullptr;
		}
		if(iterator)
		{
			Order* order;
			typedef std::list<Order*> OrderList;
			OrderList orderList;

			if(symbolEmpty)m_account->LockInquiryWait();
			else m_position->LockInquiryWait();
			
			iterator->Reset();
			while(order = TU_GetNextOrder(iterator))
			{
#ifndef TAKION_NO_OPTIONS
				if(order->GetOptionBlock())
				{
					if(addInstrumentFilter & (1 << ST_OPTION))
					{
						orderList.push_back(order);
					}
				}
				else if(order->GetSecurityType())
				{
					if(addInstrumentFilter & (1 << ST_FUTURE))
					{
						orderList.push_back(order);
					}
				}
				else
#endif
				if(addInstrumentFilter & (1 << ST_STOCK))
				{
					orderList.push_back(order);
				}
			}
			
			if(symbolEmpty)m_account->UnlockInquiry();
			else m_position->UnlockInquiry();

			delete iterator;

			m_ListBoxOrder.SetRedraw(FALSE);
			for(OrderList::iterator it = orderList.begin(), itend = orderList.end(); it != itend; ++it)
			{
				AddOrder(*it);
			}
			m_ListBoxOrder.SetRedraw(TRUE);
			m_ListBoxOrder.Invalidate(FALSE);
		}
	}
}

void SampleDialogSettingOrder::RemoveOrderStatus(unsigned char removeFilter)
{
	if(removeFilter)
	{
		m_ListBoxOrder.SetRedraw(FALSE);

		const OrderObserver* orderObserver;
		for(unsigned int i = 0, end = m_ListBoxOrder.GetItemCount(); i < end; )
		{
			orderObserver = ((const OrderObserver*)m_ListBoxOrder.GetItemDataPtr(i));
			switch(orderObserver->GetStatus())
			{
				case OSE_UNACKNOWLEDGED:
				case OSE_PENDING:
				if(removeFilter & (1 << FOST_PENDING))
				{
					m_ListBoxOrder.RemoveItemAt(i, orderObserver);
					--end;
				}
				else ++i;
				break;

				case OSE_PARTIALLY_FILLED:
				if((removeFilter & (1 << FOST_PENDING)) && (removeFilter & (1 << FOST_FILLED)))
				{
					m_ListBoxOrder.RemoveItemAt(i, orderObserver);
					--end;
				}
				else ++i;
				break;

				case OSE_FILLED://Order Dead
				if(removeFilter & (1 << FOST_FILLED))
				{
					m_ListBoxOrder.RemoveItemAt(i, orderObserver);
					--end;
				}
				else ++i;
				break;

				case OSE_UNFILLED://Order Dead
				case OSE_REJECTED:
				if(removeFilter & (1 << FOST_UNFILLED))
				{
					m_ListBoxOrder.RemoveItemAt(i, orderObserver);
					--end;
				}
				else ++i;
				break;
			}
		}
		m_ListBoxOrder.SetRedraw(TRUE);
		m_ListBoxOrder.Invalidate(FALSE);
		RepaintCount();
	}
}

void SampleDialogSettingOrder::UpdateOrderStatusMask()
{
	const unsigned char mask = GetOrderStatusMask();
	if(mask != m_orderStatusMask)
	{
		unsigned char removeFilter = m_orderStatusMask & ~mask;
		unsigned char addFilter = mask & ~m_orderStatusMask;
		m_orderStatusMask = mask;
		RemoveOrderStatus(removeFilter);
		AddOrderByFilter(addFilter, m_orderSideMask, m_orderInstrumentMask);
	}
}

void SampleDialogSettingOrder::RemoveOrderInstrument(unsigned char removeFilter)
{
	if(removeFilter)
	{
		m_ListBoxOrder.SetRedraw(FALSE);
		const OrderObserver* orderObserver;
		for(unsigned int i = 0, end = m_ListBoxOrder.GetItemCount(); i < end; )
		{
			orderObserver = (const OrderObserver*)m_ListBoxOrder.GetItemDataPtr(i);
#ifndef TAKION_NO_OPTIONS
			if(orderObserver->GetOptionBlock())
			{
				if(removeFilter & (1 << ST_OPTION))
				{
					m_ListBoxOrder.RemoveItemAt(i, orderObserver);
					--end;
				}
				else ++i;
			}
			else if(orderObserver->GetSecurityType())
			{
				if(removeFilter & (1 << ST_FUTURE))
				{
					m_ListBoxOrder.RemoveItemAt(i, orderObserver);
					--end;
				}
				else ++i;
			}
			else
#endif
			if(removeFilter & (1 << ST_STOCK))
			{
				m_ListBoxOrder.RemoveItemAt(i, orderObserver);
				--end;
			}
			else ++i;
		}
		m_ListBoxOrder.SetRedraw(TRUE);
		m_ListBoxOrder.Invalidate(FALSE);
		RepaintCount();
	}
}

void SampleDialogSettingOrder::UpdateOrderInstrumentMask()
{
	const unsigned char mask = GetOrderInstrumentMask();
	if(mask != m_orderInstrumentMask)
	{
		unsigned char removeFilter = m_orderInstrumentMask & ~mask;
		unsigned char addFilter = mask & ~m_orderInstrumentMask;
		m_orderInstrumentMask = mask;
		RemoveOrderInstrument(removeFilter);
		AddOrderByFilter(m_orderStatusMask, m_orderSideMask, addFilter);
	}
}

unsigned int SampleDialogSettingOrder::CalculateTakionOrderStatusMask(unsigned char orderStatusMask)
{
	unsigned int takionOrderStatusMask = 0;
	if(orderStatusMask & (1 << FOST_PENDING))
	{
		takionOrderStatusMask |= (1 << TU_ORDER_STATUS_UNACKNOWLEDGED) | (1 << TU_ORDER_STATUS_PENDING);
	}
	if(orderStatusMask & (1 << FOST_FILLED))
	{
		takionOrderStatusMask |= (1 << TU_ORDER_STATUS_FILLED);
	}
	if(orderStatusMask & (1 << FOST_UNFILLED))
	{
		takionOrderStatusMask |= (1 << TU_ORDER_STATUS_CANCELED) | (1 << TU_ORDER_STATUS_REJECTED);
	}
	return takionOrderStatusMask;
}

unsigned int SampleDialogSettingOrder::CalculateTakionOrderSideMask(unsigned char orderSideMask)
{
	unsigned int takionOrderSideMask = 0;
	if(orderSideMask & (1 << FOS_BUY))
	{
		takionOrderSideMask |= (1 << TU_ORDER_SIDE_BUY);
	}
	if(orderSideMask & (1 << FOS_SELL))
	{
		takionOrderSideMask |= (1 << TU_ORDER_SIDE_SELL);
	}
	if(orderSideMask & (1 << FOS_SHORT))
	{
		takionOrderSideMask |= (1 << TU_ORDER_SIDE_SHORT);
	}
	return takionOrderSideMask;
}

void SampleDialogSettingOrder::RemoveOrderSide(unsigned char removeFilter)
{
	if(removeFilter)
	{
		m_ListBoxOrder.SetRedraw(FALSE);
		const OrderObserver* orderObserver;
//		const Order* order;
		for(unsigned int i = 0, end = m_ListBoxOrder.GetItemCount(); i < end; )
		{
			orderObserver = (const OrderObserver*)m_ListBoxOrder.GetItemDataPtr(i);
//			order = ->GetOrder();
			switch(orderObserver->GetOrder()->GetSide())
			{
				case 'B':
				if(removeFilter & (1 << FOS_BUY))
				{
					m_ListBoxOrder.RemoveItemAt(i, orderObserver);
					--end;
				}
				else ++i;
				break;

				case 'S':
				if(removeFilter & (1 << FOS_SELL))
				{
					m_ListBoxOrder.RemoveItemAt(i, orderObserver);
					--end;
				}
				else ++i;
				break;

				case 'T':
				if(removeFilter & (1 << FOS_SHORT))
				{
					m_ListBoxOrder.RemoveItemAt(i, orderObserver);
					--end;
				}
				else ++i;
				break;
			}
		}
		m_ListBoxOrder.SetRedraw(TRUE);
		m_ListBoxOrder.Invalidate(FALSE);
		RepaintCount();
	}
}

void SampleDialogSettingOrder::UpdateOrderSideMask()
{
	const unsigned char mask = GetOrderSideMask();
	if(mask != m_orderSideMask)
	{
		unsigned char removeFilter = m_orderSideMask & ~mask;
		unsigned char addFilter = mask & ~m_orderSideMask;
		m_orderSideMask = mask;
		RemoveOrderSide(removeFilter);
		AddOrderByFilter(m_orderStatusMask, addFilter, m_orderInstrumentMask);
	}
}

void SampleDialogSettingOrder::RemoveOrdersButSymbol()
{
	if(!m_numericSymbol
#ifndef TAKION_NO_OPTIONS
		|| m_optionMode && !m_optionBlockValid
#endif
		)
	{
		ClearOrders();
	}
	else
	{
		m_ListBoxOrder.SetRedraw(FALSE);
		const OrderObserver* orderObserver;
		const Order* order;
		for(unsigned int i = 0, end = m_ListBoxOrder.GetItemCount(); i < end; )
		{
			orderObserver = (const OrderObserver*)m_ListBoxOrder.GetItemDataPtr(i);
			order = orderObserver->GetOrder();
			if(order->GetNumericSymbol() != m_numericSymbol
#ifndef TAKION_NO_OPTIONS
				|| m_optionBlock != order->GetOptionBlock()
				|| m_future != (order->GetSecurityType() == ST_FUTURE)
#endif
			)
			{
				m_ListBoxOrder.RemoveItemAt(i, orderObserver);
				--end;
			}
			else ++i;
		}
		m_ListBoxOrder.SetRedraw(TRUE);
		m_ListBoxOrder.Invalidate(FALSE);
		RepaintCount();
	}
}

LRESULT SampleDialogSettingOrder::OnListBoxChecked(WPARAM listBoxId, LPARAM l)
{
	switch(listBoxId)
	{
		case listBoxOrderStatusId:
		m_ListBoxOrderStatus.InvalidateClient();
		UpdateOrderStatusMask();
		break;

		case listBoxOrderSideId:
		m_ListBoxOrderSide.InvalidateClient();
		UpdateOrderSideMask();
		break;

		case listBoxOrderInstrumentId:
		m_ListBoxOrderInstrument.InvalidateClient();
		UpdateOrderInstrumentMask();
		break;
/*
		default:
		BaseDialogSettingAccount::OnListBoxChecked(w, l);
		break;
*/
	}
	return 0;
}

unsigned char SampleDialogSettingOrder::GetOrderStatusMask() const
{
	return m_ListBoxOrderStatus.GetFilter();
}

unsigned char SampleDialogSettingOrder::GetOrderInstrumentMask() const
{
	return m_ListBoxOrderInstrument.GetFilter();
}

unsigned char SampleDialogSettingOrder::GetOrderSideMask() const
{
	return m_ListBoxOrderSide.GetFilter();
}

bool SampleDialogSettingOrder::SetPosition(const Position* position)
{
	if(m_position != position)
	{
//		bool toUpdateContents = true;
		if(m_position)
		{
			if(m_inThread)
			{
				m_position->RemoveInThreadObserver(this);
			}
			else
			{
				m_position->RemoveObserver(this);
			}
			if(position)
			ClearOrders();
		}
		else
		{
//RemoveAllOrdersBut(position);
//			toUpdateContents = false;
		}
		m_position = position;
		if(m_position)
		{
			if(m_inThread)
			{
				m_position->AddInThreadObserver(this);
			}
			else
			{
				m_position->AddObserver(this);
			}
//			AddOrderByFilter(m_orderStatusMask, m_orderSideMask, m_orderInstrumentMask);
		}
		m_StaticSymbol.Invalidate(FALSE);
/*
		if(toUpdateContents)
		{
			UpdatePosition();
		}
*/
		return true;
	}
	return false;
}

void SampleDialogSettingOrder::ClearOrders()
{
	m_ListBoxOrder.ClearItemsWithRedraw();
}

bool SampleDialogSettingOrder::ShouldAddAccountObserver() const
{
	return !m_numericSymbol
#ifndef TAKION_NO_OPTIONS
		|| m_optionMode && !m_optionBlockValid
#endif
		;
}

void SampleDialogSettingOrder::AccountChanged()
{
	ClearOrders();
	m_ListBoxOrder.SetAccount(m_account);
	if(SetPosition(FindPosition()))
	{
		AddOrderByFilter(m_orderStatusMask, m_orderSideMask, m_orderInstrumentMask);
	}
}

void SampleDialogSettingOrder::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
{
	titleRect.bottom = m_topOffset;
	titleRect.top = titleRect.bottom - m_bottomOffset;

	totalRect.bottom = m_clientRect.bottom;
	totalRect.top = totalRect.bottom - m_bottomOffset;

	const LbHorizontalScroller* scroller = listBox->GetHorizontalScroller();
	if(scroller)
	{
		titleRect.left = totalRect.left = scroller->GetColumnLeftOffset() + m_titleOffset;
		titleRect.right = totalRect.right = m_clientRect.right;
	}
}

const char* SampleDialogSettingOrder::GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const
{
	std::string info;
	switch(columnId)
	{
		case OCI_SYMBOL:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxOrder.GetItemCount(), buf, buflen);

		case OCI_CLOSED_PNL:
		U_MoneyToStr(info, m_ListBoxOrder.GetClosedPnl(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		case OCI_BP_USED:
		U_MoneyToStr(info, m_ListBoxOrder.GetBpUsed(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		case OCI_PENDING_SIZE:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxOrder.GetSizePending(), buf, buflen);

		case OCI_EXECUTED_SIZE:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxOrder.GetSizeExecuted(), buf, buflen);

		case OCI_ISO:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxOrder.GetIsoCount(), buf, buflen);

		case OCI_REPLACING_NATIVELY:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxOrder.GetReplacingNativelyCount(), buf, buflen);

#ifndef TAKION_NO_OPTIONS
		case OCI_OPTION_CALLPUT:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxOrder.GetOptionCount(), buf, buflen);
#endif

		default:
		return nullptr;
	}
}

void SampleDialogSettingOrder::Resize(bool widthChanged, bool heightChanged)
{
	if(m_ListBoxOrder.m_hWnd)
	{
		if(!m_bottomOffset)
		{
			CRect rect;
			m_ListBoxOrder.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_ListBoxOrder.GetClientRect(&clientRect);
			m_listBoxBorderSize = (rect.Width() - clientRect.Width()) / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;
/*
			m_EditPath.GetWindowRect(&clientRect);
			ScreenToClient(&clientRect);
			m_editLeftOffset = clientRect.left;
			m_editTopOffset = clientRect.top;
*/
			m_titleOffset = m_leftOffset + m_listBoxBorderSize;
			
			m_EditSymbol.GetWindowRect(&rect);
			m_EditSymbol.GetClientRect(&clientRect);
			const int editBoxBorderSize = (rect.Width() - clientRect.Width()) / 2 + LOWORD(m_EditSymbol.GetMargins());

			m_StaticSymbol.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_StaticSymbol.SetWindowPos(nullptr, rect.left + editBoxBorderSize, rect.top, rect.Width() - editBoxBorderSize, rect.Height(), SWP_NOZORDER|SWP_NOACTIVATE);
		}
		const int bottom = m_clientRect.bottom - m_bottomOffset;
		m_ListBoxOrder.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);

/*
		if(heightChanged)
		{
			UpdateTotalRects();//m_leftOffset + m_listBoxBorderSize, bottom);
		}
*/
	}
	BaseDialogSettingAccount::Resize(widthChanged, heightChanged);
	if(heightChanged)InvalidateAllTotalValueRects(listBoxOrderId);
}

void SampleDialogSettingOrder::RepaintTotals() const
{
	InvalidateAllTotalValueRects(listBoxOrderId);
}

StringSetting* SampleDialogSettingOrder::GetAccountIdSetting()//{return m_orderAccountId;}
{
	return &extensionConfig->m_orderAccountId;
}

void SampleDialogSettingOrder::RestoreLayout()
{
	BaseDialogSettingAccount::RestoreLayout();

	const bool inThread = extensionConfig->m_orderInThread.GetValue();
	m_CheckBoxInThread.SetCheck(inThread ? BST_CHECKED : BST_UNCHECKED);
	SetInThread(inThread);

	m_EditSymbol.SetWindowText(extensionConfig->m_orderSymbol.GetValue().c_str());

#ifndef TAKION_NO_OPTIONS
	SetOptionValues(extensionConfig->m_orderOption.GetValue(),
//		extensionConfig->m_orderOptionSuffix.GetValue(),
		extensionConfig->m_orderOptionCall.GetValue(),
		extensionConfig->m_orderOptionStrikePrice.GetValue(),
		extensionConfig->m_orderOptionExpiration.GetValue());

	m_CheckBoxFuture.SetCheck(extensionConfig->m_orderFuture.GetValue());
	OnFuture();
#endif

	m_ListBoxOrderStatus.SetFilter(extensionConfig->m_orderStatusMask.GetValue());
	UpdateOrderStatusMask();
	m_ListBoxOrderSide.SetFilter(extensionConfig->m_orderSideMask.GetValue());
	UpdateOrderSideMask();
	m_ListBoxOrderInstrument.SetFilter(extensionConfig->m_orderInstrumentMask.GetValue());
	UpdateOrderInstrumentMask();

	m_CheckBoxReplaceMarketOrder.SetCheck(extensionConfig->m_orderReplaceMarketOrder.GetValue());
	OnReplaceMarketOrder();
	m_CheckBoxReplacePriceOffsetPercent.SetCheck(extensionConfig->m_orderReplacePriceOffsetPercent.GetValue());
	OnReplacePriceOffsetPercent();
	m_CheckBoxReplaceSizeOffsetPercent.SetCheck(extensionConfig->m_orderReplaceSizeOffsetPercent.GetValue());
	OnReplaceSizeOffsetPercent();
	m_CheckBoxReplaceNative.SetCheck(extensionConfig->m_orderReplaceNative.GetValue());
	OnReplaceNative();
	m_CheckBoxPreBorrow.SetCheck(extensionConfig->m_orderReplacePreBorrow.GetValue());
	OnPreBorrow();
	FillSpinControlFromPrice(extensionConfig->m_orderReplacePreBorrowPrice.GetValue(), m_SpinReplacePreBorrowPrice, U_GetDivider<unsigned int>(preBorrowDecDigits), true);

	FillSpinControlFromSignedPrice(extensionConfig->m_orderReplacePriceOffset.GetValue(), m_SpinReplacePriceOffset, U_GetDivider<unsigned int>(decDigits), true);
	m_SpinReplaceSizeOffset.SetValue(extensionConfig->m_orderReplaceSizeOffset.GetValue());//, true);

	m_CheckBoxReplacePriceEnforceMinOffset.SetInvalid(m_replaceMarketOrder || m_replacePriceOffset.isZero());

	m_modified = false;
}

void SampleDialogSettingOrder::OnSelchangeOrder()
{
}

void SampleDialogSettingOrder::EnableSymbolButton()
{
	EnableControl(m_ButtonSymbol.m_hWnd, CanApplySymbol(), m_EditSymbol.m_hWnd);
}

void SampleDialogSettingOrder::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void SampleDialogSettingOrder::OnSymbolChange()
{
	CString text;
	m_EditSymbol.GetWindowText(text);
	const unsigned __int64 controlNumericSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(text, sizeof(unsigned __int64) - 1);
	if(controlNumericSymbol != m_controlNumericSymbol)
	{
		bool wasEmpty = !m_controlNumericSymbol;
		m_controlNumericSymbol = controlNumericSymbol;

		if(m_controlNumericSymbol == m_numericSymbol)
		{
			if(!m_symbolCurrent)
			{
				m_symbolCurrent = true;
				m_EditSymbol.Invalidate(FALSE);
				EnableSymbolButton();
			}
		}
		else
		{
			if(m_symbolCurrent)
			{
				m_symbolCurrent = false;
				m_EditSymbol.Invalidate(FALSE);
				EnableSymbolButton();
			}
		}

#ifndef TAKION_NO_OPTIONS
		if(wasEmpty != !m_controlNumericSymbol)
		{
			OptionModeChanged();
			FutureModeChanged();
		}
#endif
	}
}

BOOL SampleDialogSettingOrder::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		unsigned char ascii = (unsigned char)pMsg->wParam;
//		HWND focus = ::GetFocus();
//		if(focus == m_EditSymbol.m_hWnd)
//		{
			switch(ascii)
			{
				case VK_RETURN:
				case VK_INSERT:
				if(ApplySymbol())
				{
					return TRUE;
				}
				break;
			}
//		}
	}
	return BaseDialogSettingAccount::PreTranslateMessage(pMsg);
}

void SampleDialogSettingOrder::OnSymbol()
{
	ApplySymbol();
}

bool SampleDialogSettingOrder::CanApplySymbol() const
{
	return
#ifndef TAKION_NO_OPTIONS
		(!m_controlOptionMode || m_controlOptionBlockValid)
		&& ((m_controlOptionMode != m_optionMode || m_controlOptionMode && m_controlOptionBlock != m_optionBlock) ||
		
		!m_controlOptionMode && m_controlFuture != m_future ||
#endif
		!m_symbolCurrent
#ifndef TAKION_NO_OPTIONS
		)
#endif
		;
/*
	return !m_symbolCurrent
#ifndef TAKION_NO_OPTIONS
		&& (!m_controlOptionMode || m_controlOptionBlockValid)
		|| (!m_controlOptionMode || m_controlOptionBlockValid)
		&& (m_optionMode != m_controlOptionMode || m_controlOptionMode && m_controlOptionBlockValid && m_controlOptionBlock != m_optionBlock)
#endif
		;
*/
}

bool SampleDialogSettingOrder::ApplySymbol()
{
	if(CanApplySymbol())
	{
		const bool wasEmpty = !m_numericSymbol
#ifndef TAKION_NO_OPTIONS
			|| m_optionMode && !m_optionBlockValid
#endif
			;
		m_numericSymbol = m_controlNumericSymbol;
#ifndef TAKION_NO_OPTIONS
		m_optionMode = m_controlOptionMode;
		m_optionBlock = m_controlOptionBlock;
		m_optionBlockValid = m_controlOptionBlockValid;
		m_optionCall = m_controlOptionCall;
		m_expirationYear = m_controlExpirationYear;
		m_expirationMonth = m_controlExpirationMonth;
		m_expirationDay = m_controlExpirationDay;
		m_strikePrice = m_controlStrikePrice;
//		m_optionBlock = m_controlOptionBlock;
		m_optionBlockValid = m_controlOptionBlockValid;
		m_future = m_controlFuture;
#endif

		const bool becameEmpty = !m_numericSymbol
#ifndef TAKION_NO_OPTIONS
			|| m_optionMode && !m_optionBlockValid
#endif
			;

		m_symbolCurrent = true;
		SetPosition(becameEmpty ? nullptr : FindPosition());
		if(m_account)
		{
			if(becameEmpty)
			{
				AddAccountObserver();
			}
			else if(wasEmpty)
			{
				RemoveAccountObserver();
			}
		}
		if(becameEmpty)//whole account
		{
			AddOrderByFilter(m_orderStatusMask, m_orderSideMask, m_orderInstrumentMask);
		}
		else if(wasEmpty)
		{
			RemoveOrdersButSymbol();
		}
		else
		{
			ClearOrders();
			AddOrderByFilter(m_orderStatusMask, m_orderSideMask, m_orderInstrumentMask);
		}
		m_EditSymbol.Invalidate(FALSE);
		EnableSymbolButton();

		m_StaticSymbol.SetWindowText((const char*)&m_numericSymbol);

#ifndef TAKION_NO_OPTIONS
		InvalidateOptionControls();
		if(m_optionBlockValid && m_optionBlock && m_optionMode && m_numericSymbol)
		{
			std::string optionInfo;
			OptionSymbolBlockKey::ConvertOptionBlockToInstrumentString(optionInfo, m_optionBlock);
			m_StaticOptionInfo.SetWindowText(optionInfo.c_str());
		}
		else
		{
			m_StaticOptionInfo.SetWindowText("");
		}
		UpdateCallPutColor();
#endif
//		m_StaticSymbol.Invalidate(FALSE);
		m_modified = true;
		return true;
	}
	return false;
}

HBRUSH SampleDialogSettingOrder::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSymbolId:
		if(!m_symbolCurrent)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;

		case staticSymbolId:
		if(m_numericSymbol != 0 && !m_position)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetWarningTextColor());
			return *m_bkBrush;
		}
		break;

		case staticReplacePriceOffsetId:
//		case checkBoxReplacePriceOffsetPercentId:
		if(m_replaceMarketOrder)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;

#ifndef TAKION_NO_OPTIONS
		case staticOptionInfoId:
		if(m_numericSymbol != 0 && !m_position)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetWarningTextColor());
			return *m_bkBrush;
		}
		break;
/*
		case checkBoxSymbolOptionId:
		if(m_controlNumericSymbol != 0)
		{
			if(m_controlOptionMode != m_optionMode)
			{
				pDC->SetBkMode(TRANSPARENT);
				return *m_visualBase->GetPendingBrush();
			}
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;

		case checkBoxSymbolFutureId:
		if(m_controlNumericSymbol != 0 && !m_controlOptionMode)
		{
			if(m_controlFuture != m_future)
			{
				pDC->SetBkMode(TRANSPARENT);
				return *m_visualBase->GetPendingBrush();
			}
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;

		case radioSymbolOptionCallId:
		if(m_controlNumericSymbol != 0 && isControlOptionUsed())
		{
			if(m_controlOptionCall != m_optionCall && m_controlOptionCall)
			{
				pDC->SetBkMode(TRANSPARENT);
				return *m_visualBase->GetPendingBrush();
			}
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;

		case radioSymbolOptionPutId:
		if(m_controlNumericSymbol != 0 && isControlOptionUsed())
		{
			if(m_controlOptionCall != m_optionCall && !m_controlOptionCall)
			{
				pDC->SetBkMode(TRANSPARENT);
				return *m_visualBase->GetPendingBrush();
			}
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetGrayColor());
			return *m_bkBrush;
		}
		break;
*/
#endif
	}
	return BaseDialogSettingAccount::OnCtlColor(pDC, pWnd, nCtlColor);
}

unsigned char SampleDialogSettingOrder::GetSecurityType() const
{
	return 
#ifndef TAKION_NO_OPTIONS
		m_optionBlock ? ST_OPTION : m_future ? ST_FUTURE:
#endif
		ST_STOCK;
}

const Position* SampleDialogSettingOrder::FindPosition() const
{
	if(m_account && m_numericSymbol != 0
#ifndef TAKION_NO_OPTIONS
		&& (!m_optionBlock || m_optionBlockValid)
#endif
		)
	{
#ifndef TAKION_NO_OPTIONS
		return m_account->FindPositionWithLock(m_numericSymbol, m_optionBlock, GetSecurityType());
#else
		return m_account->FindStockPositionWithLock(m_numericSymbol);
#endif
	}
	else
	{
		return nullptr;
	}
}
/*
void SampleDialogSettingOrder::SetHistoryLoaded(bool loaded)
{
	if(m_historyLoaded != loaded)
	{
		m_historyLoaded = loaded;
//		m_ListBoxOrder.SetHistoryLoaded(m_historyLoaded);
		OnHistoryLoaded();
	}
}
*/
void SampleDialogSettingOrder::OnHistoryLoaded()
{
	m_ListBoxOrder.SetHistoryLoaded(m_historyLoaded);
}
/*
void SampleDialogSettingOrder::SetExecutorConnected(bool connected)
{
	if(m_executorConnected != connected)
	{
		m_executorConnected = connected;
		OnExecutorConnected();
	}
}
*/
void SampleDialogSettingOrder::OnExecutorConnected()
{
	m_ListBoxOrder.SetExecutorConnected(m_executorConnected);

	if(!m_executorConnected)
	{
		RepaintTotals();
	}
}

void SampleDialogSettingOrder::AddNewOrder(Order* order)
{
	if(order && OrderBelongs(order))
	{
		new OrderObserver(order, &m_ListBoxOrder, m_inThread);
	}
}

void SampleDialogSettingOrder::AddOrder(Order* order)
{
	int found = m_ListBoxOrder.FindOrderByClientId(order->GetClientId());
	if(found < 0)//not found
	{
		new OrderObserver(order, &m_ListBoxOrder, m_inThread);
	}
}

void SampleDialogSettingOrder::Notify(const Message* message, const Observable* from, const Message* info)
{
#ifdef _DEBUG
	if(&TD_GetAdminObservable() != from && &TD_GetCustomExternalObservable() != from)
	{
		switch(message->m_type)
		{
			case TM_POSITION_OPEN_PNL_LEVEL1:
			case TM_POSITION_OPEN_PNL_PRINT:
			break;

			default:
			printf("X");
			break;
		}
	}
#endif
	switch(message->m_type)
	{
		case TS_ORDER://New Order in thread
		if(m_inThread)
		{
			const TMsgReqOrder* msg = (const TMsgReqOrder*)message;
			if(info && info->m_type == TM_UPDATED_ORDER)//sanity check
			{
				const TMsgUpdatedOrder* updatedOrder = (const TMsgUpdatedOrder*)info;
				const Order* order = updatedOrder->m_order;
				TD_WriteCustomExternalMessageToMainThread(&TMsgSampleNewOrderCreated(order->GetClientId()));
			}
		}
		break;

		case TM_CUSTOM_EXTERNAL:
		switch(((const TMsgCustom*)message)->m_customId)
		{
			case NEW_ORDER_CREATED:
			if(m_account)//sanity check
			{
				const TMsgSampleNewOrderCreated* msg = (const TMsgSampleNewOrderCreated*)message;
				Order* order = m_account->FindOrderByClientId(msg->m_clientId);
				AddNewOrder(order);
			}
			break;

			case ORDER_TOTALS_CHANGED:
			{
				const TMsgSampleOrderTotalsChanged* msg = (const TMsgSampleOrderTotalsChanged*)message;
				m_ListBoxOrder.UpdateClosedPnlByDiff(msg->m_closedPnlDiff);
				m_ListBoxOrder.UpdateBpUsedByDiff(msg->m_bpUsedDiff);
				m_ListBoxOrder.UpdateSizePendingByDiff(msg->m_sizePendingDiff);
				m_ListBoxOrder.UpdateSizeExecutedByDiff(msg->m_sizeExecutedDiff);
			}
			break;

			case NEW_ORDER_SEND_ORDER_DONE:
			{
				const TMsgSampleNewOrderSendOrderDone* msg = (const TMsgSampleNewOrderSendOrderDone*)message;
				SetLastSendIdDone(msg->m_clientId);
			}
			break;

			case NEW_POSITION_CREATED:
			{
				const TMsgSampleNewPositionCreated* msg = (const TMsgSampleNewPositionCreated*)message;
				if(msg->m_numericSymbol == m_numericSymbol
#ifndef TAKION_NO_OPTIONS
					&& msg->m_optionBlock == m_optionBlock
					&& msg->m_securityType == GetSecurityType()
#endif
					)
				if(SetPosition(FindPosition()))
				{
					AddOrderByFilter(m_orderStatusMask, m_orderSideMask, m_orderInstrumentMask);
				}
			}
			break;

			case ORDER_DELETED:
			{
				const TMsgSampleOrderDeleted* msg = (const TMsgSampleOrderDeleted*)message;
//				SetLastSendIdDone(msg->m_clientId);
				m_ListBoxOrder.RemoveOrderByClientId(msg->m_clientId);
			}
			break;
		}
		break;

		case TM_SEND_ORDER_DONE://All the orders generated by SendOrder function are sent out
		{
			const Position* position = nullptr;
			if(info && info->m_type == TM_NEW_POSITION)
			{
				const TMsgNewPosition* positionMsg = (const TMsgNewPosition*)info;
				position = positionMsg->m_position;
			}
			const TMsgSendOrderDone* msg = (const TMsgSendOrderDone*)message;
			if(m_inThread)
			{
				TD_WriteCustomExternalMessageToMainThread(&TMsgSampleNewOrderSendOrderDone(msg->m_sendId));
			}
			else
			{
				SetLastSendIdDone(msg->m_sendId);
			}
		}
		break;

		case TM_NEW_ORDER:
		if(!m_inThread)
		{
			AddNewOrder(((const TMsgOrder*)message)->m_order);
		}
		break;

		case TM_ORDER_DELETE://This message only comes if check box "Destroy canceled Orders" is checked in Takion Settings tab Orders-Misc. Then all cancelled orders are destroyed and you get notified when it happens.
		if(m_inThread)
		{
			TD_WriteCustomExternalMessageToMainThread(&TMsgSampleOrderDeleted((((const TMsgOrder*)message)->m_order)->GetClientId()));
		}
		else
		{
			m_ListBoxOrder.RemoveOrder(((const TMsgOrder*)message)->m_order);
		}
		break;

		case TM_NEW_POSITION:
		if(m_numericSymbol != 0)
		{
			const Position* position = ((TMsgNewPosition*)message)->m_position;
			if(m_inThread)
			{
				TD_WriteCustomExternalMessageToMainThread(&TMsgSampleNewPositionCreated(position->GetNumericSymbol()
#ifndef TAKION_NO_OPTIONS
					,position->GetOptionBlock()
					,position->GetSecurityType()
#endif
					));
			}
			else
			{
				if(position->GetAccount() == m_account && position->GetNumericSymbol() == m_numericSymbol
#ifndef TAKION_NO_OPTIONS
					&& position->GetOptionBlock() == m_optionBlock
					&& position->GetSecurityType() == GetSecurityType()
#endif
					)
				{
					if(SetPosition(position))
					{
						AddOrderByFilter(m_orderStatusMask, m_orderSideMask, m_orderInstrumentMask);
					}
				}
			}
		}
		break;

		case TM_ORDER_ERROR:
		if(!m_inThread)
		{
			if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
			{
				Account* account = ((TMsgUpdatedAccountAndPosition*)info)->m_account;
				if(account == m_account || !m_account)
				{
					const TMsgOrderError* msg = (const TMsgOrderError*)message;
//					m_ListBoxStock.LogOrderError(msg->m_errorCode, "OrderError");
				}
			}
		}
		break;

		case TM_CANCEL_ERROR:
		if(!m_inThread)
		{
			if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
			{
//				Account* account = ((TMsgUpdatedAccountAndPosition*)info)->m_account;
				const TMsgCancelError* msg = (const TMsgCancelError*)message;
//				m_ListBoxStock.LogCancelError(msg->m_errorCode, "CancelError");
			}
		}
		break;

//		case TM_ORDER_UPDATE:
//		break;

		case TM_DEAD_ORDER:
		if(!m_inThread)
		{
			const TMsgDeadOrder* msgOrder = (const TMsgDeadOrder*)message;
			const Order* order = msgOrder->m_order;
			const Position* position = order->GetPosition();
//			DisplayCanceledOrder(((const TMsgOrder*)message)->m_order);
		}
		break;

		case TM_NEW_EXECUTION:
		if(!m_inThread)
		{
			const TMsgNewExecution* msgExecution = (const TMsgNewExecution*)message;
			const Execution* execution = msgExecution->m_execution;
			const Order* order = execution->GetOrder();
			const Position* position = order->GetPosition();
//			DisplayExecution(((const TMsgExecution*)message)->m_execution);
		}
		break;

		case TS_ORDER_UPDATE:
		if(m_inThread)
		{
			if(info)
			{
				if(info->m_type == TM_UPDATED_ORDER)
				{
					const TMsgUpdatedOrder* msgOrder = (const TMsgUpdatedOrder*)info;
					const Order* order = msgOrder->m_order;
					const Position* position = order->GetPosition();
				}
			}
		}
		break;

		case TS_ORDER_ACK:
		if(m_inThread)
		{
			if(info)
			{
				if(info->m_type == TM_UPDATED_ORDER)
				{
					const TMsgUpdatedOrder* msgOrder = (const TMsgUpdatedOrder*)info;
					const Order* order = msgOrder->m_order;
					const Position* position = order->GetPosition();
				}
			}
		}
		break;

		case TS_ORDER_CANCEL_REQ:
		if(m_inThread)
		{
			if(info)
			{
				if(info->m_type == TM_UPDATED_ORDER)
				{
					const TMsgUpdatedOrder* msgOrder = (const TMsgUpdatedOrder*)info;
					const Order* order = msgOrder->m_order;
					const Position* position = order->GetPosition();
				}
			}
		}
		break;

		case TS_ORDER_KILL:
		if(m_inThread)
		{
			if(info)
			{
				if(info->m_type == TM_UPDATED_ORDER)
				{
					const TMsgUpdatedOrder* msgOrder = (const TMsgUpdatedOrder*)info;
					const Order* order = msgOrder->m_order;
					const Position* position = order->GetPosition();
				}
			}
		}
		break;

		case TS_ORDER_CANCEL_ACK:
		if(m_inThread)
		{
			if(info)
			{
				if(info->m_type == TM_UPDATED_ORDER)
				{
					const TMsgUpdatedOrder* msgOrder = (const TMsgUpdatedOrder*)info;
					const Order* order = msgOrder->m_order;
					const Position* position = order->GetPosition();
				}
			}
		}
		break;

		case TM_ORDER_STUCK:
		if(m_inThread)
		{
			if(info)
			{
				if(info->m_type == TM_UPDATED_ORDER)
				{
					const TMsgUpdatedOrder* msgOrder = (const TMsgUpdatedOrder*)info;
					const Order* order = msgOrder->m_order;
					const Position* position = order->GetPosition();
				}
			}
		}
		break;

		case TM_SEND_ORDER_ALGORITHM:
		if(m_inThread)
		{
			const TMsgSendOrderAlgorithm* msg = (const TMsgSendOrderAlgorithm*)message;
		}
		break;

		case TM_ORDER_ALGORITHM_CANCEL:
		if(m_inThread)
		{
			const TMsgOrderAlgorithmCancel* msg = (const TMsgOrderAlgorithmCancel*)message;
		}
		break;

		case TS_ORDER_REPORT:
		if(m_inThread)
		{
			if(info)
			{
				switch(info->m_type)
				{
					case TM_NEW_EXECUTION:
					{
						const TMsgNewExecution* msgExecution = (const TMsgNewExecution*)info;
						const Execution* execution = msgExecution->m_execution;
						const Order* order = execution->GetOrder();
						const Position* position = order->GetPosition();
					}
					break;

					case TM_UPDATED_ORDER:
					{
						const TMsgUpdatedOrder* msgOrder = (const TMsgUpdatedOrder*)info;
						const Order* order = msgOrder->m_order;
						const Position* position = order->GetPosition();
					}
					break;
				}
			}
		}
		break;

		case TM_ACCOUNT_CONSTRAINTS://You can make Account Constraints more restrictive (than imposed by the back end) in tab Account-Constraints
		if(m_inThread)
		{
			const TMsgCustomAccountConstraints* msg = (const TMsgCustomAccountConstraints*)message;
		}
		break;

		case TM_AUTO_ACTION:
		//if(m_inThread)
		{
			const TMsgAutoAction* msg = (const TMsgAutoAction*)message;
		}
		break;

		case TM_UPDATED_ACCOUNT:
		if(!m_inThread)
		{
			const TMsgUpdatedAccount* msg = (const TMsgUpdatedAccount*)message;
		}
		break;

		case TS_ACCOUNT_POSITION_RESP://Loading Acoount History done.
		if(!m_inThread)
		{
			if(info)
			{
				if(info->m_type == TM_UPDATED_ACCOUNT)
				{
					const TMsgUpdatedAccount* msg = (const TMsgUpdatedAccount*)message;
				}
			}
		}
		break;

		case TM_ALL_ACCOUNTS_LOADED://Loading History done for all Acoounts.
		break;
/*
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

		case TS_LOGIN_RESP:
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			Account* account = msg->m_account;
			if(m_ComboBoxAccount.FindItem(account) < 0)
			{
				m_ComboBoxAccount.AddString((const char*)account);
			}

		}
		break;

		case TS_ACCOUNT_POSITION_RESP:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;

//			SendAccountPositionCost(account);
//			if(account == m_currentAccount)
//			{
//				CurrentAccountLoaded();
//			}
//			if(from != (Observable*)account)
//			{
//				AccountLoaded(account);
//			}
		}
		break;

		case TS_ACCOUNT_EXEC_HISTORY_COMPLETE:
		if(info && info->m_type == TM_UPDATED_ACCOUNT)
		{
			TMsgUpdatedAccount* msg = (TMsgUpdatedAccount*)info;
			Account* account = msg->m_account;
			int sel = m_ComboBoxAccount.GetCurSel();
			if(sel >= 0)
			{
				if(account == (Account*)m_ComboBoxAccount.GetItemDataPtr(sel))
				{
				}
			}
		}
		break;

		case TM_DISCONNECTED:

		if(info && info->m_type == TM_CONNECTION_OBJECT)
		{
			const TMsgConnectionObject* msg = (const TMsgConnectionObject*)info;
			Connection* connection = msg->m_connection;
			if(connection == TD_GetMarketSorterConnection())
			{
			}
			else if(connection == TD_GetMarketDataConnection())
			{
			}
			else if(connection == TD_GetExecutorConnection())
			{
			}
		}
		break;

		case TM_CONNECTED:
		if(info && info->m_type == TM_CONNECTION_OBJECT)
		{
//			const TMsgConnected* msg = (const TMsgConnected*)message;
//			ConnectionConnected(msg->m_ipAddress, msg->m_port, msg->m_ipBindAddress, msg->m_bindPort, msg->m_connectionName);
			const TMsgConnectionObject* msg = (const TMsgConnectionObject*)info;
			Connection* connection = msg->m_connection;
			if(connection == TD_GetMarketSorterConnection())
			{
			}
			else if(connection == TD_GetMarketDataConnection())
			{
			}
			else if(connection == TD_GetExecutorConnection())
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

		case TM_MARKET_SORTER_LOADED:
//		MarketSorterLoaded();
		break;

		case TM_MARKET_SORTER_CLEANUP:
//		MarketSorterCleanup();
		break;

		case TM_DATA_THREAD_BUFFER_OVERFLOW:
//		PostMessage(WM_USER + 500, ((const TMsgDataThreadBufferOverflow*)message)->m_threadId, 0);
		break;
*/
/*
		case TS_NEW_ACCOUNT:
		if(info && info->m_type == TM_NEW_ACCOUNT)
		{
			TMsgNewAccountObject* msg = (TMsgNewAccountObject*)info;
			const Account* account = msg->m_account;
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
//		UpdateAccount(((TMsgAccount*)message)->m_account);
		break;

		case TM_ACCOUNT_DELETE:
		{
			Account* account = ((const TMsgAccountDelete*)message)->m_account;
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
		m_ComboBoxAccount.ResetContent();
		m_ComboBoxAccount.AddString(nullptr);
//		SetPosition(nullptr);
		m_ComboBoxAccount.SetCurSel(0);
		OnSelchangeAccount();
		break;

//		case M_RESP_MD_LOGON:
//		UpdateMarketDataEntitlements();
//		break;

		case TS_LOGIN_RESP:
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
*/
		default:
		BaseDialogSettingAccount::Notify(message, from, info);
		break;
	}
}
