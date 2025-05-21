#include "stdafx.h"
#include "SampleDialogSettingPosition.h"
#include "ExtensionSample.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;
/*
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
*/
class PositionObserver : public Observer
{
public:
	PositionObserver(const Position* const& position, ListBoxPosition* const& listBoxPosition, const bool& inThread):
		m_position(position),
		m_listBoxPosition(listBoxPosition),
		m_size(0),
		m_absSize(0),
		m_overnightSize(0),
		m_status(CalculatePositionStatus()),
		m_inThread(inThread)
	{
		AddObserver();
		listBoxPosition->AddItem(this);
		Update();
//		if(m_status == TU_POSITION_STATUS_COUNT)m_status = CalculatePositionStatus();

	}
//	virtual ~PositionObserver();
	void SetInThread(const bool& inThread)
	{
		if(inThread != m_inThread)
		{
			if(inThread)
			{
				m_position->AddInThreadObserver(this);
				m_position->RemoveObserver(this);
			}
			else
			{
				m_position->AddObserver(this);
				m_position->RemoveInThreadObserver(this);
			}
			m_inThread = inThread;
		}
	}
	void RemoveObserver()
	{
		if(m_inThread)
		{
			m_position->RemoveInThreadObserver(this);
		}
		else
		{
			m_position->RemoveObserver(this);
		}
	}
	void AddObserver()
	{
		if(m_inThread)
		{
			m_position->AddInThreadObserver(this);
		}
		else
		{
			m_position->AddObserver(this);
		}
	}
	const Position* GetPosition() const{return m_position;}
#ifndef TAKION_NO_OPTIONS
	char isCallPut() const{return m_position->GetOptionBlock() ? m_position->isCall() ? 'C' : 'P' : '\0';}
	const unsigned __int64& GetOptionBlock() const{return m_position->GetOptionBlock();}
//	const OptionInfo* GetOptionInfo() const{return m_position->GetOptionKey();}
	unsigned char GetSecurityType() const{return m_position->GetSecurityType();}
#endif
	const Money& GetClosedPnl() const{return m_closedPnl;}
	const Money& GetOpenPnl() const{return m_openPnl;}
	const Price& GetPrice() const{return m_price;}
	const Price& GetOvernightPrice() const{return m_overnightPrice;}
	const int& GetSize() const{return m_size;}
	const int& GetOvernightSize() const{return m_overnightSize;}
	const unsigned int& GetAbsSize() const{return m_absSize;}
	const unsigned char& GetStatus() const{return m_status;}//OrderStatusEnum
	unsigned char CalculatePositionStatus() const;

	const unsigned __int64& GetNumericSymbol() const{return m_position->GetNumericSymbol();}

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

	const Position* m_position;
	ListBoxPosition* m_listBoxPosition;
	Money m_closedPnl;
	Money m_openPnl;
	Price m_price;
	Price m_overnightPrice;
	int m_size;
	unsigned int m_absSize;
	int m_overnightSize;
	unsigned char m_status;//FilterPositionStatus
	bool m_inThread;

private:
	PositionObserver(const PositionObserver& other){}
	PositionObserver& operator=(const PositionObserver& other){return *this;}
};

class LbPositionHorizontalScroller : public LbHorizontalScroller
{
public:
	LbPositionHorizontalScroller(ListBoxPosition* listBox);
	virtual void AddColumns() override;
protected:
	ListBoxPosition* m_listBoxPosition;
};

/*
class TMsgSampleNewOrderSendOrderDone : public TMsgSampleOrderClientId
{
public:
	TMsgSampleNewOrderSendOrderDone(const unsigned int& sendId):
		TMsgSampleOrderClientId(sendId, NEW_ORDER_SEND_ORDER_DONE, sizeof(TMsgSampleNewOrderSendOrderDone))
	{}
};
*/
class TMsgSamplePositionTotalsChanged : public TMsgCustomExternal
{
public:
	TMsgSamplePositionTotalsChanged(const Money& closedPnlDiff, const Money& openPnlDiff, const int& sizeDiff, const int& overnightSizeDiff)://, const unsigned int& absSizeDiff):
		TMsgCustomExternal(POSITION_TOTALS_CHANGED, sizeof(TMsgSamplePositionTotalsChanged)),
		m_closedPnlDiff(closedPnlDiff),
		m_openPnlDiff(openPnlDiff),
		m_sizeDiff(sizeDiff),
		m_overnightSizeDiff(overnightSizeDiff)
//		m_sizeExecutedDiff(sizeExecutedDiff)
	{}
	Money m_closedPnlDiff;
	Money m_openPnlDiff;
	int m_sizeDiff;
	int m_overnightSizeDiff;
//	unsigned int m_sizeExecutedDiff;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/*
PositionObserver::~PositionObserver()
{
}
*/

unsigned char PositionObserver::CalculatePositionStatus() const
{
	const int size = m_position->GetSize();
	return size > 0 ? TU_POSITION_STATUS_LONG:
		size < 0 ? TU_POSITION_STATUS_SHORT:
		TU_POSITION_STATUS_FLAT;
}

class PositionId
{
public:
	PositionId(const unsigned __int64& numericSymbol
#ifndef TAKION_NO_OPTIONS
		, unsigned __int64 optionBlock
#endif
		):
		m_numericSymbol(numericSymbol)
#ifndef TAKION_NO_OPTIONS
		,m_optionBlock(optionBlock)
#endif
	{}
	const unsigned __int64& GetNumericSymbol() const{return m_numericSymbol;}
#ifndef TAKION_NO_OPTIONS
	const unsigned __int64& GetOptionBlock() const{return m_optionBlock;}
#endif
protected:
	unsigned __int64 m_numericSymbol;
#ifndef TAKION_NO_OPTIONS
	unsigned __int64 m_optionBlock;
#endif
};

void PositionObserver::PostRepaintMessageFromWorkerThread(unsigned short startColumn, unsigned short endColumn) const
{
	if(startColumn <= endColumn)
	{
		m_listBoxPosition->PostMessage(WM_USER + 444, (WPARAM)new PositionId(m_position->GetNumericSymbol()
#ifndef TAKION_NO_OPTIONS
			, m_position->GetOptionBlock()
#endif
			), ((unsigned int)startColumn << 16) | endColumn);
	}
}

void PositionObserver::UpdateInThread()
{
	if(m_inThread)
	{
		const Money closedPnl = m_position->GetClosedPnl();
		const Money openPnl = m_position->GetOpenPnlPrint();
		const Price price = m_position->GetAveragePrice();
		const Price overnightPrice = m_position->GetOvernightAveragePrice();
		const int size = m_position->GetSize();
		const int overnightSize = m_position->GetOvernightSize();
//		const unsigned int absSize = m_position->GetAbsSize();

		const Money closedPnlDiff = closedPnl - m_closedPnl;

		const Money openPnlDiff = openPnl - m_openPnl;

		const int sizeDiff = size - m_size;
		const int overnightSizeDiff = overnightSize - m_overnightSize;

		const bool priceChanged = price != m_price;
		const bool overnightPriceChanged = overnightPrice != m_overnightPrice;

//		const bool sizeExecutedChanged = sizeExecuted != m_sizeExecuted;
//		const unsigned int sizeExecutedDiff = sizeExecuted - m_sizeExecuted;

		const unsigned char status = size > 0 ? TU_POSITION_STATUS_LONG:
			size < 0 ? TU_POSITION_STATUS_SHORT:
			TU_POSITION_STATUS_FLAT;

		const bool statusChanged = status != m_status;

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
		if(priceChanged)
		{
			m_price = price;
			if(PCI_PRICE < firstColumnUpdated)firstColumnUpdated = PCI_PRICE;
			if(PCI_PRICE > lastColumnUpdated)lastColumnUpdated = PCI_PRICE;
		}
		if(overnightPriceChanged)
		{
			m_overnightPrice = overnightPrice;
			if(PCI_OVERNIGHT_PRICE < firstColumnUpdated)firstColumnUpdated = PCI_OVERNIGHT_PRICE;
			if(PCI_OVERNIGHT_PRICE > lastColumnUpdated)lastColumnUpdated = PCI_OVERNIGHT_PRICE;
		}
		if(!closedPnlDiff.isZero())
		{
			totalsChanged = true;
			m_closedPnl = closedPnl;
			if(PCI_CLOSED_PNL < firstColumnUpdated)firstColumnUpdated = PCI_CLOSED_PNL;
			if(PCI_CLOSED_PNL > lastColumnUpdated)lastColumnUpdated = PCI_CLOSED_PNL;
		}
		if(!openPnlDiff.isZero())
		{
			totalsChanged = true;
			m_openPnl = openPnl;
			if(PCI_OPEN_PNL < firstColumnUpdated)firstColumnUpdated = PCI_OPEN_PNL;
			if(PCI_OPEN_PNL > lastColumnUpdated)lastColumnUpdated = PCI_OPEN_PNL;
		}
		if(sizeDiff)
		{
			totalsChanged = true;
			m_size = size;
			const unsigned int absSize = size >= 0 ? size : -size;
			if(absSize != m_absSize)
			{
				m_absSize = absSize;
				if(PCI_ABS_SIZE < firstColumnUpdated)firstColumnUpdated = PCI_ABS_SIZE;
				if(PCI_ABS_SIZE > lastColumnUpdated)lastColumnUpdated = PCI_ABS_SIZE;
			}
			if(PCI_SIZE < firstColumnUpdated)firstColumnUpdated = PCI_SIZE;
			if(PCI_SIZE > lastColumnUpdated)lastColumnUpdated = PCI_SIZE;
		}
		if(overnightSizeDiff)
		{
			totalsChanged = true;
			m_overnightSize = overnightSize;
			if(PCI_OVERNIGHT_SIZE < firstColumnUpdated)firstColumnUpdated = PCI_OVERNIGHT_SIZE;
			if(PCI_OVERNIGHT_SIZE > lastColumnUpdated)lastColumnUpdated = PCI_OVERNIGHT_SIZE;
		}

		if(unlock)UnlockModification();

//Can't repaint from a thread. Post Windows Message
		PostRepaintMessageFromWorkerThread(firstColumnUpdated, lastColumnUpdated);

//Write custom message to main thread to change the totals
		if(totalsChanged)
		{
			TD_WriteCustomExternalMessageToMainThread(&TMsgSamplePositionTotalsChanged(closedPnlDiff, openPnlDiff, sizeDiff, overnightSizeDiff));
		}
	}
}

bool PositionObserver::Update()//returns true if order status changed. If returns true, then we need to check if the order needs to be removed from the list box because it no longer belongs there.
{
	if(m_position->LockInquiry())//have to lock because the position may be being modified in a worker thread
	{
//Remember the values that can change. Other values don't change and can be safely taken from the order object itself at any time even if the order is being modified in a different thread.
		const Money closedPnl = m_position->GetClosedPnl();
		const Money openPnl = m_position->GetOpenPnlPrint();
		const Price price = m_position->GetAveragePrice();
		const Price overnightPrice = m_position->GetOvernightAveragePrice();
		const int size = m_position->GetSize();
		const int overnightSize = m_position->GetOvernightSize();
		const unsigned int absSize = m_position->GetAbsSize();
		m_position->UnlockInquiry();

		int found = m_listBoxPosition->FindPosition(m_position->GetNumericSymbol()
#ifndef TAKION_NO_OPTIONS
			, m_position->GetOptionBlock()
#endif
			);

		const unsigned char status = size > 0 ? TU_POSITION_STATUS_LONG:
			size < 0 ? TU_POSITION_STATUS_SHORT:
			TU_POSITION_STATUS_FLAT;
		const bool statusChanged = m_status != status;
		if(statusChanged)
		{
			m_status = status;
			m_listBoxPosition->InvalidateItem(found);//the whole row needs to be repainted because it's color changed
		}
		LbHorizontalScroller* scroller = m_listBoxPosition->GetHorizontalScroller();
		if(closedPnl != m_closedPnl)
		{
			m_listBoxPosition->UpdateClosedPnl(m_closedPnl, closedPnl);
			m_closedPnl = closedPnl;
			if(scroller)scroller->InvalidateItemColumn(found, PCI_CLOSED_PNL);
		}
		if(openPnl != m_openPnl)
		{
			m_listBoxPosition->UpdateOpenPnl(m_openPnl, openPnl);
			m_openPnl = openPnl;
			if(scroller)scroller->InvalidateItemColumn(found, PCI_OPEN_PNL);
		}
		if(price != m_price)
		{
//			m_listBoxPosition->UpdatePrice(m_price, price);
			m_price = price;
			if(scroller)scroller->InvalidateItemColumn(found, PCI_PRICE);
		}
		if(overnightPrice != m_overnightPrice)
		{
//			m_listBoxPosition->UpdatePrice(m_price, price);
			m_overnightPrice = overnightPrice;
			if(scroller)scroller->InvalidateItemColumn(found, PCI_OVERNIGHT_PRICE);
		}
		if(size != m_size)
		{
			m_listBoxPosition->UpdateSize(m_size, size);
			m_size = size;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, PCI_SIZE);
		}
		if(overnightSize != m_overnightSize)
		{
			m_listBoxPosition->UpdateOvernightSize(m_overnightSize, overnightSize);
			m_overnightSize = overnightSize;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, PCI_OVERNIGHT_SIZE);
		}
		if(absSize != m_absSize)
		{
			m_listBoxPosition->UpdateAbsSize(m_absSize, absSize);
			m_absSize = absSize;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, PCI_ABS_SIZE);
		}
		if(size != m_size)
		{
			m_listBoxPosition->UpdateSize(m_size, size);
			m_size = size;
			if(!statusChanged && scroller)scroller->InvalidateItemColumn(found, PCI_SIZE);
		}
		return statusChanged;
	}
//else - LockInquiry failed which means that the order is being updated and a new notification will come soon and the function will be called again.
	return false;
}

void PositionObserver::Notify(const Message* message, const Observable* from, const Message* info)
{
	if(m_inThread)
	{
		switch(message->m_type)
		{
			case TS_ACCOUNT_RECOVERY_RESP://Position Loaded from the database. Usually it's a newly created Position ans it's processed in SampleDialogSettingPosition::Notify TM_NEW_POSITION, where all the newly created Positions are processed, not only loaded from the databse.
			{
				const TMsgPosition* msg = (const TMsgPosition*)message;
/*
				if(info && info->m_type == TM_UPDATED_POSITION)//make sure the info is of the type we expect it to be
				{
					const TMsgUpdatedPosition* positionInfo = (const TMsgUpdatedPosition*)info;
					const PositionData* position = positionInfo->m_position;
				}
*/
				UpdateInThread();
			}
			break;

			case TS_ORDER:
			{
				const TMsgReqOrder* msg = (const TMsgReqOrder*)message;
				if(info && info->m_type == TM_UPDATED_ORDER)//make sure the info is of the type we expect it to be
				{
					const TMsgUpdatedOrder* orderInfo = (const TMsgUpdatedOrder*)info;
					const Order* order = orderInfo->m_order;
				}
				UpdateInThread();
			}
			break;

			case TM_POSITION_RESTRICTED:
			{
				const TMsgPositionRestricted* msg = (const TMsgPositionRestricted*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_HTB:
			{
				const TMsgPositionHtb* msg = (const TMsgPositionHtb*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_BORROW_PRICE:
			{
				const TMsgPositionBorrowPrice* msg = (const TMsgPositionBorrowPrice*)message;
				UpdateInThread();
			}
			break;

			case TM_ACCOUNT_COMMAND_INVENTORY:
			{
				const TMsgInventory* msg = (const TMsgInventory*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_COST:
			{
				const TMsgPositionCost* msg = (const TMsgPositionCost*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_CLOSING_PRICE:
			{
				const TMsgPositionClosingPriceUpdate* msg = (const TMsgPositionClosingPriceUpdate*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_TODAYS_CLOSING_PRICE:
			{
				const TMsgPositionTodaysClosingPriceUpdate* msg = (const TMsgPositionTodaysClosingPriceUpdate*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_OPEN_PNL_LEVEL1:
			{
				const TMsgPositionLevel1OpenPnlUpdate* msg = (const TMsgPositionLevel1OpenPnlUpdate*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_OPEN_PNL_PRINT:
			{
				const TMsgPositionPrintOpenPnlUpdate* msg = (const TMsgPositionPrintOpenPnlUpdate*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_OPEN_PNL_IMBEXNEAR:
			{
				const TMsgPositionImbExNearOpenPnlUpdate* msg = (const TMsgPositionImbExNearOpenPnlUpdate*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_OPEN_PNL_TCLOSE:
			{
				const TMsgPositionTCloseOpenPnlUpdate* msg = (const TMsgPositionTCloseOpenPnlUpdate*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_TRAILING_ORDERS_PRINT:
			{
				const TMsgPositionPrintTrailingOrdersUpdate* msg = (const TMsgPositionPrintTrailingOrdersUpdate*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_TRAILING_ORDERS_LEVEL1:
			{
				const TMsgPositionLevel1TrailingOrdersUpdate* msg = (const TMsgPositionLevel1TrailingOrdersUpdate*)message;
				UpdateInThread();
			}
			break;

			case TM_SEND_ORDER_ALGORITHM:
			{
				const TMsgSendOrderAlgorithm* msg = (const TMsgSendOrderAlgorithm*)message;
				UpdateInThread();
			}
			break;

			case TM_DEFAULT_COMMISSION_FEE:
			{
				const TMsgCommissionFee* msg = (const TMsgCommissionFee*)message;
				UpdateInThread();
			}
			break;

			case TM_ORDER_ALGORITHM_CANCEL:
			{
				const TMsgOrderAlgorithmCancel* msg = (const TMsgOrderAlgorithmCancel*)message;
				UpdateInThread();
			}
			break;

			case TS_ORDER_CANCEL_REQ:
			{
				const TMsgReqOrderCancel* msg = (const TMsgReqOrderCancel*)message;
				UpdateInThread();
			}
			break;

			case TM_CANCEL_ERROR:
			{
				const TMsgCancelError* msg = (const TMsgCancelError*)message;
				UpdateInThread();
			}
			break;

			case TM_POSITION_PHANTOM_SHARES_CHANGED:
			{
				const PositionPhantomSharesChanged* msg = (const PositionPhantomSharesChanged*)message;
				UpdateInThread();
			}
			break;

			case TM_ORDER_STUCK:
			{
				const TMsgStuckOrder* msg = (const TMsgStuckOrder*)message;
				UpdateInThread();
			}
			break;

			case TS_ORDER_REJECT:
			{
				const TMsgOrderReject* msg = (const TMsgOrderReject*)message;
				if(info && info->m_type == TM_UPDATED_ORDER)//make sure the info is of the type we expect it to be
				{
					const TMsgUpdatedOrder* orderInfo = (const TMsgUpdatedOrder*)info;
					const Order* order = orderInfo->m_order;
				}
				UpdateInThread();
			}
			break;

			case TS_CANCEL_REJECT:
			{
				const TMsgCancelReject* msg = (const TMsgCancelReject*)message;
				if(info && info->m_type == TM_UPDATED_ORDER)//make sure the info is of the type we expect it to be
				{
					const TMsgUpdatedOrder* orderInfo = (const TMsgUpdatedOrder*)info;
					const Order* order = orderInfo->m_order;
				}
				UpdateInThread();
			}
			break;

			case TS_ORDER_CANCEL_ACK:
			{
				const TMsgOrderCancelAck* msg = (const TMsgOrderCancelAck*)message;
				if(info && info->m_type == TM_UPDATED_ORDER)//make sure the info is of the type we expect it to be
				{
					const TMsgUpdatedOrder* orderInfo = (const TMsgUpdatedOrder*)info;
					const Order* order = orderInfo->m_order;
				}
				UpdateInThread();
			}
			break;

			case TS_ORDER_REPLACE_ACK:
			{
				const TMsgOrderReplaceAck* msg = (const TMsgOrderReplaceAck*)message;
				if(info && info->m_type == TM_UPDATED_ORDER)//make sure the info is of the type we expect it to be
				{
					const TMsgUpdatedOrder* orderInfo = (const TMsgUpdatedOrder*)info;
					const Order* order = orderInfo->m_order;
				}
				UpdateInThread();
			}
			break;

			case TS_ORDER_REPLACED:
			{
				const TMsgOrderReplaceDone* msg = (const TMsgOrderReplaceDone*)message;
				if(info && info->m_type == TM_UPDATED_ORDER)//make sure the info is of the type we expect it to be
				{
					const TMsgUpdatedOrder* orderInfo = (const TMsgUpdatedOrder*)info;
					const Order* order = orderInfo->m_order;
				}
				UpdateInThread();
			}
			break;

			case TS_ACCOUNT_ORDER_HISTORY_COMPLETE://Loading of todays Orders is complete. It is done from the right click menu of the Order window.
			{
				const TMsgOrderHistoryComplete* msg = (const TMsgOrderHistoryComplete*)message;
				UpdateInThread();
			}
			break;

			case TS_ACCOUNT_EXEC_HISTORY_COMPLETE://Loading of todays Executions is complete. It is done from the right click menu of the Execution window.
			{
				const TMsgExecutionHistoryComplete* msg = (const TMsgExecutionHistoryComplete*)message;
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

			case TM_POSITION_REMOVE_DELAYED_ORDERS:
			{
				const TMsgPositionRemoveDelayedOrders* msg = (const TMsgPositionRemoveDelayedOrders*)message;
				UpdateInThread();
			}
			break;

			case TS_ORDER_UPDATE:
			{
				const TMsgOrderUpdate* msg = (const TMsgOrderUpdate*)message;
				if(info && info->m_type == TM_UPDATED_ORDER)//make sure the info is of the type we expect it to be
				{
					const TMsgUpdatedOrder* orderInfo = (const TMsgUpdatedOrder*)info;
					const Order* order = orderInfo->m_order;
				}
				UpdateInThread();
			}
			break;

			case TS_ORDER_ACK:
			{
				const TMsgOrderAck* msg = (const TMsgOrderAck*)message;
				if(info && info->m_type == TM_UPDATED_ORDER)//make sure the info is of the type we expect it to be
				{
					const TMsgUpdatedOrder* orderInfo = (const TMsgUpdatedOrder*)info;
					const Order* order = orderInfo->m_order;
				}
				UpdateInThread();
			}
			break;

			case TS_ORDER_KILL:
			{
				const TMsgOrderKill* msg = (const TMsgOrderKill*)message;
				UpdateInThread();
			}
			break;
		}
	}
	else
	{
		if(Update())
		{
			if(!m_listBoxPosition->GetOwnerDialog()->PositionObserverStatusBelongs(this))
			{
				m_listBoxPosition->RemoveRow(this);
			}
		}
	}
}

LbPositionHorizontalScroller::LbPositionHorizontalScroller(ListBoxPosition* listBox):
	LbHorizontalScroller(listBox),
	m_listBoxPosition(listBox)
{
}

void LbPositionHorizontalScroller::AddColumns()
{

	const int itemHeight = m_listBox->GetMyItemHeight();
	const int doubleHeight = itemHeight + itemHeight;
	const int tripleHeight = doubleHeight + itemHeight;
	const int fourHeight = tripleHeight + itemHeight;
	const int fiveHeight = fourHeight + itemHeight;
	const int sixHeight = fiveHeight + itemHeight;

	AddColumn("Symbol", PCI_SYMBOL, fiveHeight, DT_LEFT, DT_LEFT, DT_RIGHT, "Symbol");//, 0xFFFFFFFF, itemHeight, itemHeight);
#ifndef TAKION_NO_OPTIONS
	AddColumn("C/P", PCI_OPTION_CALLPUT, doubleHeight, DT_CENTER, DT_CENTER, DT_RIGHT, "Option Call (C) or Put (P)");
	AddColumn("Expiration", PCI_OPTION_EXPIRATION, fourHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Option Expiration Date");
	AddColumn("Strike", PCI_OPTION_STRIKE_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Option Strike Price");
#endif
	AddColumn("Price", PCI_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Position Average Price");
	AddColumn("Size", PCI_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Position Size");
	AddColumn("AbsSize", PCI_ABS_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Position Absolute Size");

//	AddColumn("Created", OCI_TIME_CREATED, fiveHeight, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Time Created");

	AddColumn("ClosedP&L", PCI_CLOSED_PNL, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Closed P&&L");
	AddColumn("OpenP&Lpr", PCI_OPEN_PNL, fiveHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Open P&&L based on Last NBBO Print");

	AddColumn("OvnSize", PCI_OVERNIGHT_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, DT_RIGHT, "Position Overnight Size");
	AddColumn("OvnPrice", PCI_OVERNIGHT_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Position Overnight Price");
}


int ListBoxPosition::Compare(const void* item1, const void* item2) const
{
//	const PositionObserver* observer1 = (const PositionObserver*)item1;
	const PositionObserver* observer2 = (const PositionObserver*)item2;
	return CompareItemTo(item1, observer2->GetNumericSymbol()
#ifndef TAKION_NO_OPTIONS
		,OptionBlock(observer2->GetOptionBlock())
#endif
		);
/*
	int ret = U_CompareUIntAsString(observer1->GetNumericSymbol(), observer2->GetNumericSymbol());
#ifndef TAKION_NO_OPTIONS
	if(!ret)
	{
		const OptionBlock optionBlock1(observer1->GetOptionBlock());
		const OptionBlock optionBlock2(observer2->GetOptionBlock());
		if(optionBlock1.GetExpirationYear() < optionBlock2.GetExpirationYear())return -1;
		else if(optionBlock1.GetExpirationYear() > optionBlock2.GetExpirationYear())return 1;
		else
		{
			const unsigned char month1 = optionBlock1.GetExpirationMonth();
			const unsigned char month2 = optionBlock2.GetExpirationMonth();
			return month1 < month2 ? -1 :
				month1 > month2 ? 1 :
				optionBlock1.GetExpirationDay() < optionBlock2.GetExpirationDay() ? -1 :
				optionBlock1.GetExpirationDay() > optionBlock2.GetExpirationDay() ? 1 :
				optionBlock1.GetStrikePrice() < optionBlock2.GetStrikePrice() ? -1 :
				optionBlock1.GetStrikePrice() > optionBlock2.GetStrikePrice() ? 1 :
				optionBlock1.IsCall() == optionBlock2.IsCall() ? 0 :
				optionBlock1.IsCall() ? -1 : 1;
		}
	}
#endif
	return ret;
*/
}

int ListBoxPosition::CompareItemTo(const void* item1, const unsigned __int64& numericSymbol
#ifndef TAKION_NO_OPTIONS
	, const OptionBlock& optionBlock
#endif
	) const
{
	const PositionObserver* observer1 = (const PositionObserver*)item1;
	int ret = U_CompareUIntAsString(observer1->GetNumericSymbol(), numericSymbol);
#ifndef TAKION_NO_OPTIONS
	if(!ret)
	{
		const OptionBlock optionBlock1(observer1->GetOptionBlock());
		if(optionBlock1.GetExpirationYear() < optionBlock.GetExpirationYear())return -1;
		else if(optionBlock1.GetExpirationYear() > optionBlock.GetExpirationYear())return 1;
		else
		{
			const unsigned char month1 = optionBlock.GetExpirationMonth();
			const unsigned char month2 = optionBlock.GetExpirationMonth();
			return month1 < month2 ? -1:
				month1 > month2 ? 1:
				optionBlock1.GetExpirationDay() < optionBlock.GetExpirationDay() ? -1:
				optionBlock1.GetExpirationDay() > optionBlock.GetExpirationDay() ? 1:
				optionBlock1.GetStrikePrice() < optionBlock.GetStrikePrice() ? -1:
				optionBlock1.GetStrikePrice() > optionBlock.GetStrikePrice() ? 1:
				optionBlock1.IsCall() == optionBlock.IsCall() ? 0:
				optionBlock1.IsCall() ? -1 : 1;
		}
	}
#endif
	return ret;
}

void ListBoxPosition::SetInThread(const bool inThread)
{
	if(inThread != m_inThread)
	{
		m_inThread = inThread;
		PositionObserver* positionObserver;
		for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
		{
			positionObserver = (PositionObserver*)GetItemDataPtr(i);
			positionObserver->SetInThread(m_inThread);
		}
	}
}

int ListBoxPosition::FindPosition(const unsigned __int64& numericSymbol
#ifndef TAKION_NO_OPTIONS
	, const unsigned __int64& numericOptionBlock
#endif
	) const//search in the sorted collection using break in half
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
#ifndef TAKION_NO_OPTIONS
	OptionBlock optionBlock(numericOptionBlock);
#endif
    int low = 0;
    int high = count - 1;
    int middle;
//	int ret;
    while(high >= low)
    {
        middle = (low + high) >> 1;
//		ret = CompareItemTo(GetItemDataPtr(middle), numericSymbol, optionBlock);
		switch(CompareItemTo(GetItemDataPtr(middle), numericSymbol
#ifndef TAKION_NO_OPTIONS
			, optionBlock
#endif
		))
		{
			case -1:
			low = middle + 1;
			break;

			case 1:
			high = middle - 1;
			break;

			default:
			return middle;
		}
    }
    return -1;
}

BEGIN_MESSAGE_MAP(ListBoxPosition, ColumnedListBox)
	ON_MESSAGE(WM_USER + 444, OnItemRepaint)
END_MESSAGE_MAP()

LRESULT ListBoxPosition::OnItemRepaint(WPARAM id, LPARAM columns)
{
	const unsigned short from = (unsigned short)(columns >> 16);
	const unsigned short to = (unsigned short)columns;
//	if(from < to)
	const PositionId* positionId = (const PositionId*)id;
	int ordinal = FindPosition(positionId->GetNumericSymbol()
#ifndef TAKION_NO_OPTIONS
		, positionId->GetOptionBlock()
#endif
		);
	InvalidateItemColumnRange(ordinal, from, to);
	delete positionId;
	return 0;
}

void ListBoxPosition::AddItem(const PositionObserver* item)
{
	AddString((const char*)item);
	RepaintCount();
#ifndef TAKION_NO_OPTIONS
	if(item->isCallPut())
	{
		UpdateOptionCount(0, 1);
	}
#endif
}

void ListBoxPosition::RemoveItemAt(int found, const PositionObserver* item)
{
	UpdateSize(item->GetSize(), 0);
	UpdateAbsSize(item->GetAbsSize(), 0);
	UpdateOvernightSize(item->GetOvernightSize(), 0);
#ifndef TAKION_NO_OPTIONS
	if(item->isCallPut())
	{
		UpdateOptionCount(1, 0);
	}
#endif
	UpdateClosedPnl(item->GetClosedPnl(), Money::moneyZero);
	UpdateOpenPnl(item->GetOpenPnl(), Money::moneyZero);
	DeleteString(found);
	RepaintCount();
}

bool ListBoxPosition::RemoveRowAt(int found)
{
	if(found >= 0 && (unsigned int)found < m_itemCount)//GetCount())
	{
		RemoveItemAt(found, (const PositionObserver*)GetItemDataPtr(found));
		return true;
	}
	return false;
}

bool ListBoxPosition::RemoveRow(const PositionObserver* item)
{
	int found = FindPosition(item->GetNumericSymbol()
#ifndef TAKION_NO_OPTIONS
		, item->GetOptionBlock()
#endif
		);
	if(found >= 0)
	{
		RemoveItemAt(found, item);
		return true;
	}
	return false;
}

bool ListBoxPosition::RemovePosition(const Position* position)
{
	int found = FindPosition(position->GetNumericSymbol()
#ifndef TAKION_NO_OPTIONS
		, position->GetOptionBlock()
#endif
	);
	if(found >= 0)
	{
		RemoveRowAt(found);
		return true;
	}
	return false;
}

void ListBoxPosition::ResetTotals()
{
	m_owner->RepaintCount();
	if(!m_closedPnl.isZero())
	{
		m_closedPnl.SetZero();
		m_owner->RepaintClosedPnl();//RepaintClosedPnl();
	}
	if(!m_openPnl.isZero())
	{
		m_openPnl.SetZero();
		m_owner->RepaintOpenPnl();//RepaintOpenPnl();
	}
	if(m_size)
	{
		m_size = 0;
		m_absSize = 0;
		m_owner->RepaintSize();
		m_owner->RepaintAbsSize();
	}
	if(m_overnightSize)
	{
		m_overnightSize = 0;
		m_owner->RepaintOvernightSize();
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
void ListBoxPosition::RepaintOptionCount() const
{
	m_owner->RepaintOptionCount();
}
#endif

void ListBoxPosition::RepaintCount() const
{
	m_owner->RepaintCount();
}

void ListBoxPosition::RepaintClosedPnl() const
{
	m_owner->RepaintClosedPnl();
}

void ListBoxPosition::RepaintOpenPnl() const
{
	m_owner->RepaintOpenPnl();
}

void ListBoxPosition::RepaintSize() const
{
	m_owner->RepaintSize();
}

void ListBoxPosition::RepaintAbsSize() const
{
	m_owner->RepaintAbsSize();
}

void ListBoxPosition::RepaintOvernightSize() const
{
	m_owner->RepaintOvernightSize();
}
/*
void ListBoxPosition::RepaintPrice()
{
	m_owner->RepaintPrice();
}

void ListBoxPosition::RepaintOvernightPrice()
{
	m_owner->RepaintOvernightPrice();
}
*/
COLORREF ListBoxPosition::GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	const PositionObserver* positionObserver = (const PositionObserver*)lpDrawItemStruct->itemData;
	switch(positionObserver->GetStatus())
	{
		case TU_POSITION_STATUS_FLAT:
		return m_colorFlat;

		case TU_POSITION_STATUS_LONG:
		return m_colorLong;

		case TU_POSITION_STATUS_SHORT:
		return m_colorShort;

		default:
		return m_colorError;
	}
}

COLORREF ListBoxPosition::GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	if(lpDrawItemStruct->itemState & ODS_SELECTED)return RGB(255, 255, 255);
	return GetSelectedBkColor(lpDrawItemStruct);
}

void ListBoxPosition::DestroyItem(const void* item) const
{
	delete (PositionObserver*)item;
}
/*
void ListBoxPosition::AddMenuItems(CMenu& menu)
{
	bool hasItems = m_itemCount > 0;
	int sel = GetCurSel();
	bool selected = sel >= 0;

	menu.AppendMenu(MF_STRING|(selected ? MF_ENABLED : MF_GRAYED), TMID_PASTE, "Paste");
//	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLIPBOARD, "Copy");
}

BOOL ListBoxPosition::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD loword = (DWORD)wParam;//LOWORD(wParam);
    switch(loword)
    {

		case TMID_CLIPBOARD:
		{
			std::string info;
			CopySymbols(info);
			TU_CopyStringToClipboard(info, m_hWnd);
		}
		break;

		case TMID_PASTE:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				const Position* position = ((PositionObserver*)GetItemDataPtr(sel))->GetPosition();
				m_owner->PastePosition(position);
			}
		}
		break;

		default:
		return CListBox::OnCommand(wParam, lParam);
	}
	return TRUE;

}
*/
void ListBoxPosition::SetHistoryLoaded(bool loaded)
{
	if(m_historyLoaded != loaded)
	{
		m_historyLoaded = loaded;
	}
}

void ListBoxPosition::SetExecutorConnected(bool connected)
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

void ListBoxPosition::SetAccount(const Account* account)
{
	if(m_account != account)
	{
		m_account = account;
		m_historyLoaded = m_account && m_account->isHistoryLoaded();
	}
}

void ListBoxPosition::SizeChanged(bool horizontalChange, bool verticalChange)
{
/*
	if(verticalChange)
	{
		m_oneSecondRect.bottom = m_listBoxHeight;
	}
*/
}

LbHorizontalScroller* ListBoxPosition::CreateHorizontalScroller()
{
	return new LbPositionHorizontalScroller(this);
}

/*
void ListBoxOrder::DoCreateColumnToolTips()
{
}

void ListBoxOrder::ListBoxCreated()
{
	ListBoxOwnerDraw::ListBoxCreated();
}
*/

void ListBoxPosition::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
	const PositionObserver* positionObserver = (const PositionObserver*)item;
	const Position* position = positionObserver->GetPosition();
	std::string info;
//	char num[33];
//	static const char y = 'Y';
	static const unsigned int al = DT_VCENTER|DT_NOCLIP|DT_SINGLELINE|DT_NOPREFIX;
	switch(columnId)
	{
		case PCI_SYMBOL:
		DrawText(hDC, position->GetSymbol(), -1, &r, alignment);
		return;

#ifndef TAKION_NO_OPTIONS
		case PCI_OPTION_CALLPUT:
		if(position->GetOptionBlock())
		{
			static const char c = 'C';
			static const char p = 'P';
			DrawText(hDC, position->isCall() ? &c : &p, 1, &r, alignment);
		}
		return;

		case PCI_OPTION_EXPIRATION:
		if(position->GetOptionBlock())
		{
			info.clear();
			U_AppendThreeCharDateToStr(info, position->GetExpirationDate(), '/', '\0', false, false);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		return;

		case PCI_OPTION_STRIKE_PRICE:
		if(position->GetOptionBlock())
		{
			info.clear();
			U_PriceToStr(info, position->GetStrikePrice(), 4, -1, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		return;
#endif
	}

	if(!m_inThread || positionObserver->LockInquiry())//If LockInquiry returns false, it means that currently the StockImbalance object is being modified and it will be signaled to repaint after the modification is done
	{
		std::string info;
		char num[33];
		switch(columnId)
		{
			case PCI_PRICE:
			if(!positionObserver->GetPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, positionObserver->GetPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case PCI_OVERNIGHT_PRICE:
			if(!positionObserver->GetOvernightPrice().isZero())
			{
				info.clear();
				U_PriceToStr(info, positionObserver->GetOvernightPrice(), 4, -1, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case PCI_SIZE:
			if(positionObserver->GetSize())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(positionObserver->GetSize(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case PCI_ABS_SIZE:
			if(positionObserver->GetAbsSize())
			{
				DrawText(hDC, U_UnsignedNumberToStrWithCommas(positionObserver->GetAbsSize(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case PCI_OVERNIGHT_SIZE:
			if(positionObserver->GetOvernightSize())
			{
				DrawText(hDC, U_SignedNumberToStrWithCommas(positionObserver->GetOvernightSize(), num, sizeof(num)), -1, &r, alignment);
			}
			break;

			case PCI_CLOSED_PNL:
			if(!positionObserver->GetClosedPnl().isZero())
			{
				info.clear();
				U_MoneyToStr(info, positionObserver->GetClosedPnl(), 4, 0, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;

			case PCI_OPEN_PNL:
			if(!positionObserver->GetOpenPnl().isZero())
			{
				info.clear();
				U_MoneyToStr(info, positionObserver->GetOpenPnl(), 4, 0, true, '\0', true);
				DrawText(hDC, info.c_str(), -1, &r, alignment);
			}
			break;
		}
		if(m_inThread)positionObserver->UnlockInquiry();
	}
}

void ListBoxPosition::HorizontalScrollingDone(int diff)
{
}

enum SampleSettingPositionIds
{
	inThreadCheckBoxId = baseDialogSettingAccountIdsCount,
/*
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
#endif
*/
	staticPositionStatusId,
	listBoxPositionStatusId,

	staticPositionInstrumentId,
	listBoxPositionInstrumentId,
/*
	groupReplaceId,
	checkBoxReplaceMarketOrderId,
	staticReplacePriceOffsetId,
	checkBoxReplacePriceOffsetPercentId,
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
*/
	listBoxPositionId,

	positionIdsCount,
};

const unsigned char decDigits = 2;
const unsigned char preBorrowDecDigits = 4;

//////////////////
SampleDialogSettingPosition::SampleDialogSettingPosition(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab)://, const Account* account):
	BaseDialogSettingAccount(mainWnd, parentTab, true, "Position", true, 56),
	m_CheckBoxInThread("", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_ListBoxPosition(this, m_account),

	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),

	m_inThread(false)
{
	AddListBox(&m_ListBoxPosition);
	AddListBox(&m_ListBoxPositionStatus);
	AddListBox(&m_ListBoxPositionInstrument);
	int w = m_accountWidth - hgap - hgap;
	const int halfWidth = w / 2;
	int x = 0;
	const int xc = x + hgap;
	int y = m_bottomControl + vgap;
/*
	const int replaceGroupHeight = groupOffset
		+ checkBoxHeight + vgap
		+ staticHeight + spinHeight + vgap
		+ staticHeight + spinHeight + vgap
		+ checkBoxHeight + vgap

		+ checkBoxHeight
		+ spinHeight + vgap

		+ staticHeight + vgap;
*/
//	int w23 = w * 2 / 3 + hgap;

	AddButtonIndirect("In Thread", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, xc, y, w, checkBoxHeight, inThreadCheckBoxId);

	y += checkBoxHeight + vgap;
	int yStart = y;

	AddStaticIndirect("Status Filter", WS_VISIBLE | SS_NOPREFIX, 0, xc, y, w, staticHeight, staticPositionStatusId);
	int lbHeight = TU_POSITION_STATUS_COUNT * checkBoxHeight + 2 * vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y += staticHeight, w, lbHeight, listBoxPositionStatusId);

	AddStaticIndirect("Instrument Filter", WS_VISIBLE | SS_NOPREFIX, 0, xc, y += lbHeight + vgap, w, staticHeight, staticPositionInstrumentId);
	lbHeight = ST_COUNT * checkBoxHeight + 2 * vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y += staticHeight, w, lbHeight, listBoxPositionInstrumentId);
///
	y = staticHeight;
	x += w + hgap + hgap;
	w = m_width - x;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x, y, w, m_height - staticHeight - y, listBoxPositionId);

}

HWND SampleDialogSettingPosition::GetFirstTabControl() const
{
	return m_ListBoxPosition.m_hWnd;
}

bool SampleDialogSettingPosition::PositionObserverStatusBelongs(const PositionObserver* positionObserver) const
{
	return SizeBelongs(positionObserver->GetSize());
}

bool SampleDialogSettingPosition::PositionObserverBelongs(const PositionObserver* positionObserver) const
{
	if(!m_positionStatusMask
#ifndef TAKION_NO_OPTIONS
		|| !m_positionInstrumentMask
#endif
		)return false;

	if(!InstrumentBelongs(
#ifndef TAKION_NO_OPTIONS
//		positionObserver->GetOptionBlock() != 0
		positionObserver->GetSecurityType()
#endif
		))return false;

	return PositionObserverStatusBelongs(positionObserver);
}

bool SampleDialogSettingPosition::PositionBelongs(const Position* position) const
{
	if(!m_positionStatusMask
#ifndef TAKION_NO_OPTIONS
		|| !m_positionInstrumentMask
#endif
		)return false;

	if(!InstrumentBelongs(
#ifndef TAKION_NO_OPTIONS
//		position->GetOptionBlock() != 0
		position->GetSecurityType()
#endif
		))return false;

	return SizeBelongs(position->GetSize());
}

void SampleDialogSettingPosition::UpdateSettings()
{
	BaseDialogSettingAccount::UpdateSettings();
	extensionConfig->m_positionInThread.SetValue(m_CheckBoxInThread.GetCheck() == BST_CHECKED);

	extensionConfig->m_positionStatusMask.SetValue(m_positionStatusMask);
	extensionConfig->m_positionInstrumentMask.SetValue(m_positionInstrumentMask);

}

void SampleDialogSettingPosition::ResetOffsets()
{
	m_inThread = false;

	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;
//	SetAccount(nullptr);

	m_positionStatusMask = 0;
	m_positionInstrumentMask = 0;
}

void SampleDialogSettingPosition::BeforeDestroy()
{
	ResetOffsets();
//Done in the base class
//	TD_GetAdminObservable().RemoveObserver(this);
	TD_GetCustomExternalObservable().RemoveObserver(this);
	BaseDialogSettingAccount::BeforeDestroy();
}

void SampleDialogSettingPosition::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingAccount::ToolTipsEnabled(enable);
	m_CheckBoxInThread.EnableTooltips(enable, m_toolTip);
	m_ListBoxPositionStatus.EnableTooltips(enable, m_toolTip);
	m_ListBoxPositionInstrument.EnableTooltips(enable, m_toolTip);
	m_ListBoxPosition.EnableTooltips(enable, m_toolTip);
}

BOOL SampleDialogSettingPosition::OnInitDialog()
{
	ResetOffsets();

//Done in the base class
//	TD_GetAdminObservable().AddObserver(this);//to get notified about connect / disconnect
	TD_GetCustomExternalObservable().AddObserver(this);

	BOOL ret = BaseDialogSettingAccount::OnInitDialog();

	m_CheckBoxInThread.ResizeToFitText(0);

	SetToolTipsEnabled(true);
//	m_ListBoxPosition.EnableTooltips(true, m_toolTip);
	AddTitle(&m_ListBoxPosition);

//	RestoreLayout();
	return ret;
}

void SampleDialogSettingPosition::RepaintCount() const
{
	InvalidateTotalValueRect(listBoxPositionId, PCI_SYMBOL);
}

void SampleDialogSettingPosition::RepaintClosedPnl() const
{
	InvalidateTotalValueRect(listBoxPositionId, PCI_CLOSED_PNL);
}

void SampleDialogSettingPosition::RepaintOpenPnl() const
{
	InvalidateTotalValueRect(listBoxPositionId, PCI_OPEN_PNL);
}

void SampleDialogSettingPosition::RepaintSize() const
{
	InvalidateTotalValueRect(listBoxPositionId, PCI_SIZE);
}

void SampleDialogSettingPosition::RepaintAbsSize() const
{
	InvalidateTotalValueRect(listBoxPositionId, PCI_ABS_SIZE);
}

void SampleDialogSettingPosition::RepaintOvernightSize() const
{
	InvalidateTotalValueRect(listBoxPositionId, PCI_OVERNIGHT_SIZE);
}

void SampleDialogSettingPosition::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingAccount::DoDataExchange(pDX);

	DDX_Control(pDX, inThreadCheckBoxId, m_CheckBoxInThread);

	DDX_Control(pDX, listBoxPositionStatusId, m_ListBoxPositionStatus);
	DDX_Control(pDX, listBoxPositionInstrumentId, m_ListBoxPositionInstrument);
	DDX_Control(pDX, listBoxPositionId, m_ListBoxPosition);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingPosition, BaseDialogSettingAccount)
//	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(inThreadCheckBoxId, OnInThread)

//	ON_MESSAGE(WM_USER + 15, OnSpinChange)

	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)
END_MESSAGE_MAP()

void SampleDialogSettingPosition::OnInThread()
{
	SetInThread(m_CheckBoxInThread.GetCheck() == BST_CHECKED);
}

void SampleDialogSettingPosition::AddAccountObserver()
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

void SampleDialogSettingPosition::RemoveAccountObserver()
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

void SampleDialogSettingPosition::SetInThread(const bool inThread)
{
	if(inThread != m_inThread)
	{
		m_inThread = inThread;
/*
		bool accountObserver = !m_numericSymbol
#ifndef TAKION_NO_OPTIONS
			|| m_optionMode && !m_optionBlockValid
#endif
			;
*/
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

		m_ListBoxPosition.SetInThread(m_inThread);
		m_modified = true;
	}
}

#ifndef TAKION_NO_OPTIONS
void SampleDialogSettingPosition::RepaintOptionCount() const
{
	InvalidateTotalValueRect(listBoxPositionId, PCI_OPTION_CALLPUT);
}
#endif

void SampleDialogSettingPosition::OnDblclkPosition()
{
	int sel = m_ListBoxPosition.GetCurSel();
	if(sel >= 0)
	{
		const Position* position = ((PositionObserver*)m_ListBoxPosition.GetItemDataPtr(sel))->GetPosition();
//		PastePosition(position);
	}
}
/*
void SampleDialogSettingPosition::PastePosition(const Position* position)
{
	m_EditSymbol.SetWindowText(position->GetSymbol());
#ifndef TAKION_NO_OPTIONS
	if(position->GetOptionBlock())
	{
		m_CheckBoxOption.SetCheck(BST_CHECKED);
		OnOption();
		if(position->isCall())
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
		const Price strikePrice(position->GetStrikePrice());
		const unsigned int expiration = position->GetExpirationDate();
		FillSpinControlFromPrice(strikePrice, m_SpinOptionStrikePrice, 100, true);

//		U_FormatDateTokensStr((unsigned char)(date >> 8), (unsigned char)date, (unsigned short)2000 + (unsigned char)(date >> 16), str, tokenDel, ' ', monthName, fullYear);
		SetExpirationDate((unsigned char)(expiration >> 16), (unsigned char)(expiration >> 8), (unsigned char)expiration);
//		SetExpirationDate(expiration);
	}
#endif
}
*/
/*
LRESULT SampleDialogSettingPosition::OnSpinChange(WPARAM spinId, LPARAM l)
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
		break;

		case spinReplaceSizeOffsetId:
		m_replaceSizeOffset = m_SpinReplaceSizeOffset.GetValue();
		break;

		case spinReplacePreBorrowPriceId:
		FillPriceFromSpinControl(m_replacePreBorrowPrice, m_SpinReplacePreBorrowPrice, U_GetDivider<unsigned int>(preBorrowDecDigits));
		break;

		default:
		BaseSampleDialogSettingSymbol::OnSpinChange(w, l);
		break;
	}
	return 0;
}
*/
void SampleDialogSettingPosition::AddPositionByFilter(unsigned char addStatusFilter, unsigned char addInstrumentFilter)
{
	if(addStatusFilter && addInstrumentFilter && m_account)
	{
		TakionIterator* iterator;
		const Position* position;
		typedef std::list<const Position*> PositionList;
		PositionList positionList;

		if(addInstrumentFilter & (1 << ST_STOCK))
		{
			iterator = m_account->CreatePositionIterator(addStatusFilter);
			if(iterator)
			{
				m_account->LockInquiryWait();
			
				iterator->Reset();
				while(position = TU_GetNextPosition(iterator))
				{
					positionList.push_back(position);
				}
			
				m_account->UnlockInquiry();

				delete iterator;
			}
		}
#ifndef TAKION_NO_OPTIONS
		if(addInstrumentFilter & (1 << ST_OPTION))
		{
			iterator = m_account->CreateOptionPositionIterator(addStatusFilter);
			if(iterator)
			{
				m_account->LockInquiryWait();
			
				iterator->Reset();
				while(position = TU_GetNextPosition(iterator))
				{
					positionList.push_back(position);
				}
			
				m_account->UnlockInquiry();

				delete iterator;
			}
		}
		if(addInstrumentFilter & (1 << ST_FUTURE))
		{
			iterator = m_account->CreateFuturePositionIterator(addStatusFilter);
			if(iterator)
			{
				m_account->LockInquiryWait();
			
				iterator->Reset();
				while(position = TU_GetNextPosition(iterator))
				{
					positionList.push_back(position);
				}
			
				m_account->UnlockInquiry();

				delete iterator;
			}
		}
#endif
		if(!positionList.empty())
		{
			m_ListBoxPosition.SetRedraw(FALSE);
			for(PositionList::iterator it = positionList.begin(), itend = positionList.end(); it != itend; ++it)
			{
				AddPosition(*it);
			}
			m_ListBoxPosition.SetRedraw(TRUE);
			m_ListBoxPosition.Invalidate(FALSE);
		}
	}
}

void SampleDialogSettingPosition::RemovePositionStatus(unsigned char removeFilter)
{
	if(removeFilter)
	{
		m_ListBoxPosition.SetRedraw(FALSE);

		const PositionObserver* positionObserver;
		for(unsigned int i = 0, end = m_ListBoxPosition.GetItemCount(); i < end; )
		{
			positionObserver = ((const PositionObserver*)m_ListBoxPosition.GetItemDataPtr(i));
			switch(positionObserver->GetStatus())
			{
				case TU_POSITION_STATUS_FLAT:
				if(removeFilter & (1 << TU_POSITION_STATUS_FLAT))
				{
					m_ListBoxPosition.RemoveItemAt(i, positionObserver);
					--end;
				}
				else ++i;
				break;

				case TU_POSITION_STATUS_LONG:
				if(removeFilter & (1 << TU_POSITION_STATUS_LONG))
				{
					m_ListBoxPosition.RemoveItemAt(i, positionObserver);
					--end;
				}
				else ++i;
				break;

				default:
				if(removeFilter & (1 << TU_POSITION_STATUS_SHORT))
				{
					m_ListBoxPosition.RemoveItemAt(i, positionObserver);
					--end;
				}
				else ++i;
				break;
			}
		}
		m_ListBoxPosition.SetRedraw(TRUE);
		m_ListBoxPosition.Invalidate(FALSE);
		RepaintCount();
	}
}

void SampleDialogSettingPosition::UpdatePositionStatusMask()
{
	const unsigned char mask = GetPositionStatusMask();
	if(mask != m_positionStatusMask)
	{
		const unsigned char removeFilter = m_positionStatusMask & ~mask;
		const unsigned char addFilter = mask & ~m_positionStatusMask;
		m_positionStatusMask = mask;
		RemovePositionStatus(removeFilter);
		AddPositionByFilter(addFilter, m_positionInstrumentMask);
	}
}

/*
unsigned int SampleDialogSettingPosition::CalculateTakionPositionInstrumentMask(unsigned char positionInstrumentMask)
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
*/
void SampleDialogSettingPosition::RemovePositionInstrument(unsigned char removeFilter)
{
	if(removeFilter)
	{
		m_ListBoxPosition.SetRedraw(FALSE);
		const PositionObserver* positionObserver;
//		const Order* order;
		for(unsigned int i = 0, end = m_ListBoxPosition.GetItemCount(); i < end; )
		{
			positionObserver = (const PositionObserver*)m_ListBoxPosition.GetItemDataPtr(i);
#ifndef TAKION_NO_OPTIONS
			if(positionObserver->GetOptionBlock())
			{
				if(removeFilter & (1 << ST_OPTION))
				{
					m_ListBoxPosition.RemoveItemAt(i, positionObserver);
					--end;
				}
				else ++i;
			}
			else if(positionObserver->GetSecurityType() == ST_FUTURE)
			{
				if(removeFilter & (1 << ST_FUTURE))
				{
					m_ListBoxPosition.RemoveItemAt(i, positionObserver);
					--end;
				}
				else ++i;
			}
			else
#endif
			if(removeFilter & (1 << ST_STOCK))
			{
				m_ListBoxPosition.RemoveItemAt(i, positionObserver);
				--end;
			}
			else ++i;
		}
		m_ListBoxPosition.SetRedraw(TRUE);
		m_ListBoxPosition.Invalidate(FALSE);
		RepaintCount();
	}
}

void SampleDialogSettingPosition::UpdatePositionInstrumentMask()
{
	const unsigned char mask = GetPositionInstrumentMask();
	if(mask != m_positionInstrumentMask)
	{
		unsigned char removeFilter = m_positionInstrumentMask & ~mask;
		unsigned char addFilter = mask & ~m_positionInstrumentMask;
		m_positionInstrumentMask = mask;
		RemovePositionInstrument(removeFilter);
		AddPositionByFilter(m_positionStatusMask, addFilter);
	}
}

LRESULT SampleDialogSettingPosition::OnListBoxChecked(WPARAM listBoxId, LPARAM l)
{
	switch(listBoxId)
	{
		case listBoxPositionStatusId:
		m_ListBoxPositionStatus.InvalidateClient();
		UpdatePositionStatusMask();
		break;

		case listBoxPositionInstrumentId:
		m_ListBoxPositionInstrument.InvalidateClient();
		UpdatePositionInstrumentMask();
		break;
/*
		default:
		BaseDialogSettingAccount::OnListBoxChecked(w, l);
		break;
*/
	}
	return 0;
}

unsigned char SampleDialogSettingPosition::GetPositionStatusMask() const
{
	return m_ListBoxPositionStatus.GetFilter();
}

unsigned char SampleDialogSettingPosition::GetPositionInstrumentMask() const
{
	return m_ListBoxPositionInstrument.GetFilter();
}

void SampleDialogSettingPosition::ClearPositions()
{
	m_ListBoxPosition.ClearItemsWithRedraw();
}
/*
bool SampleDialogSettingOrder::ShouldAddAccountObserver() const
{
	return !m_numericSymbol
#ifndef TAKION_NO_OPTIONS
		|| m_optionMode && !m_optionBlockValid
#endif
		;
}
*/
void SampleDialogSettingPosition::AccountChanged()
{
	ClearPositions();
	m_ListBoxPosition.SetAccount(m_account);
//	SetPosition(FindPosition());
	AddPositionByFilter(m_positionStatusMask, m_positionInstrumentMask);
}

void SampleDialogSettingPosition::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
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

const char* SampleDialogSettingPosition::GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const
{
	std::string info;
	switch(columnId)
	{
		case PCI_SYMBOL:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxPosition.GetItemCount(), buf, buflen);

		case PCI_CLOSED_PNL:
		U_MoneyToStr(info, m_ListBoxPosition.GetClosedPnl(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		case PCI_OPEN_PNL:
		U_MoneyToStr(info, m_ListBoxPosition.GetOpenPnl(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		case PCI_SIZE:
		return U_SignedNumberToStrWithCommas(m_ListBoxPosition.GetSize(), buf, buflen);

		case PCI_ABS_SIZE:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxPosition.GetAbsSize(), buf, buflen);

		case PCI_OVERNIGHT_SIZE:
		return U_SignedNumberToStrWithCommas(m_ListBoxPosition.GetOvernightSize(), buf, buflen);

#ifndef TAKION_NO_OPTIONS
		case PCI_OPTION_CALLPUT:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxPosition.GetOptionCount(), buf, buflen);
#endif

		default:
		return nullptr;
	}
}

void SampleDialogSettingPosition::Resize(bool widthChanged, bool heightChanged)
{
	if(m_ListBoxPosition.m_hWnd)
	{
		if(!m_bottomOffset)
		{
			CRect rect;
			m_ListBoxPosition.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_ListBoxPosition.GetClientRect(&clientRect);
			m_listBoxBorderSize = (rect.Width() - clientRect.Width()) / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;

			m_titleOffset = m_leftOffset + m_listBoxBorderSize;
			
		}
		int bottom = m_clientRect.bottom - m_bottomOffset;
		m_ListBoxPosition.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);

/*
		if(heightChanged)
		{
			UpdateTotalRects();//m_leftOffset + m_listBoxBorderSize, bottom);
		}
*/
	}
	BaseDialogSettingAccount::Resize(widthChanged, heightChanged);
	if(heightChanged)InvalidateAllTotalValueRects(listBoxPositionId);
}

void SampleDialogSettingPosition::RepaintTotals() const
{
	InvalidateAllTotalValueRects(listBoxPositionId);
}

StringSetting* SampleDialogSettingPosition::GetAccountIdSetting()//{return m_orderAccountId;}
{
	return &extensionConfig->m_orderAccountId;
}

void SampleDialogSettingPosition::RestoreLayout()
{
	BaseDialogSettingAccount::RestoreLayout();

	const bool inThread = extensionConfig->m_orderInThread.GetValue();
	m_CheckBoxInThread.SetCheck(inThread ? BST_CHECKED : BST_UNCHECKED);
	SetInThread(inThread);

	m_ListBoxPositionStatus.SetFilter(extensionConfig->m_positionStatusMask.GetValue());
	UpdatePositionStatusMask();
	m_ListBoxPositionInstrument.SetFilter(extensionConfig->m_positionInstrumentMask.GetValue());
	UpdatePositionInstrumentMask();

	m_modified = false;
}

void SampleDialogSettingPosition::OnSelchangePosition()
{
}

/*
HBRUSH SampleDialogSettingPosition::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
		case checkBoxReplacePriceOffsetPercentId:
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

#endif
	}
	return BaseDialogSettingAccount::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/
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
void SampleDialogSettingPosition::OnHistoryLoaded()
{
	m_ListBoxPosition.SetHistoryLoaded(m_historyLoaded);
}
/*
void SampleDialogSettingPosition::SetExecutorConnected(bool connected)
{
	if(m_executorConnected != connected)
	{
		m_executorConnected = connected;
		OnExecutorConnected();
	}
}
*/
void SampleDialogSettingPosition::OnExecutorConnected()
{
	m_ListBoxPosition.SetExecutorConnected(m_executorConnected);

	if(!m_executorConnected)
	{
		RepaintTotals();
	}
}

void SampleDialogSettingPosition::AddNewPosition(const Position* position)
{
	if(position && PositionBelongs(position))
	{
		new PositionObserver(position, &m_ListBoxPosition, m_inThread);
	}
}

void SampleDialogSettingPosition::AddPosition(const Position* position)
{
	int found = m_ListBoxPosition.FindPosition(position->GetNumericSymbol()
#ifndef TAKION_NO_OPTIONS
		, position->GetOptionBlock()
#endif
		);
	if(found < 0)//not found
	{
		new PositionObserver(position, &m_ListBoxPosition, m_inThread);
	}
}

void SampleDialogSettingPosition::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_NEW_POSITION:
		{
			const TMsgNewPosition* msg = (const TMsgNewPosition*)message;
			const Position* position = msg->m_position;
			if(m_inThread)
			{
//				UpdateInThread();
				TD_WriteCustomExternalMessageToMainThread(&TMsgSampleNewPositionCreated(position->GetNumericSymbol()
#ifndef TAKION_NO_OPTIONS
					,position->GetOptionBlock()
					,position->GetSecurityType()
#endif
					));
			}
			else
			{
				AddNewPosition(position);
			}
		}
		break;

		case TM_CUSTOM_EXTERNAL:
		switch(((const TMsgCustom*)message)->m_customId)
		{
			case NEW_POSITION_CREATED:
			if(m_account)//sanity check
			{
				const TMsgSampleNewPositionCreated* msg = (const TMsgSampleNewPositionCreated*)message;
				m_account->LockInquiryWait();
				const Position* position = m_account->
#ifndef TAKION_NO_OPTIONS
					FindPosition(msg->m_numericSymbol, msg->m_optionBlock, msg->m_securityType);
#else
					FindStockPosition(msg->m_numericSymbol);
#endif
				m_account->UnlockInquiry();
				AddNewPosition(position);
			}
			break;

			case POSITION_TOTALS_CHANGED:
			{
				const TMsgSamplePositionTotalsChanged* msg = (const TMsgSamplePositionTotalsChanged*)message;
				m_ListBoxPosition.UpdateClosedPnlByDiff(msg->m_closedPnlDiff);
				m_ListBoxPosition.UpdateOpenPnlByDiff(msg->m_openPnlDiff);
				m_ListBoxPosition.UpdateSizeByDiff(msg->m_sizeDiff);
				m_ListBoxPosition.UpdateOvernightSizeByDiff(msg->m_overnightSizeDiff);
			}
			break;
		}
		break;

		case TM_POSITION_DELETE:
		if(!m_inThread)
		{
			m_ListBoxPosition.RemovePosition(((const TMsgPositionDelete*)message)->m_position);
		}
		break;

		default:
		BaseDialogSettingAccount::Notify(message, from, info);
		break;
	}
}
