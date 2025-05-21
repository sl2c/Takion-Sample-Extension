#include "stdafx.h"
#include "SampleDialogSettingLevel2InThread.h"
//#include "StockMarketSorter.h"
#include "ExtensionSample.h"
#include "ExtensionDerivedConfig.h"
#include <fstream>

extern ExtensionSample theApp;
extern ExtensionDerivedConfig* extensionConfig;
//extern TakionMessageBox* messageBox;
/*
class TMsgAccountCustom : public TMsgCustomExternal
{
protected:
	TMsgAccountCustom(const unsigned short& customId, const unsigned short& length):TMsgCustomExternal(customId, length){}
};
*/
class StockLevel2 : public StockBase
{
friend class ListBoxStockLevel2;
friend class SampleDialogSettingLevel2InThread;
public:
	StockLevel2(const char* symbol, ListBoxStockLevel2* owner):
		StockBase(symbol),
		m_ordinal(0xFFFFFFFF),//ordinal number in the list box.
		m_owner(owner),

		m_closePriceCompact(0),

		m_level2BidCompact(0),
		m_level2BidSize(0),

		m_peBidTotalSize(0),
		m_peBidMaxSize(0),
		m_peBidPriceWithMaxSizeCompact(0),

		m_level2AskCompact(0),
		m_level2AskSize(0),

		m_peAskTotalSize(0),
		m_peAskMaxSize(0),
		m_peAskPriceWithMaxSizeCompact(0),

		m_book(nullptr),
		m_bidBookIterator(nullptr),
		m_askBookIterator(nullptr),
		m_bookId(ECNBOOK_COUNT),

		m_attributedBook(nullptr),
		m_bidAttributedBookIterator(nullptr),
		m_askAttributedBookIterator(nullptr),
		m_attributedBookId(ECNBOOK_COUNT),
		m_calculateL2(owner->isCalculateL2())
	{
		Security* security = TD_ObtainStock(symbol, false);
		if(security)
		{
			SetSecurity(security);
			security->Subscribe();
			TD_ReleaseStock(security);
		}
	}
	virtual ~StockLevel2()
	{
		if(m_security)
		{
			SetSecurity(nullptr);
//			TD_ReleaseStock(m_security);
		}
	}
	virtual void RemoveSecurityObserver(const Security* security) override{security->RemoveInThreadObserver(this);}
	virtual void AddSecurityObserver(const Security* security) override{security->AddInThreadObserver(this);}
	virtual void RemovePositionObserver(const Position* position){position->RemoveInThreadObserver(this);}
	virtual void AddPositionObserver(const Position* position){position->AddInThreadObserver(this);}
	virtual bool LockPositionInquiry(){return true;}//m_position->LockInquiry();}
	virtual void LockPositionInquiryWait(){}//m_position->LockInquiryWait();}
	virtual void UnlockPositionInquiry(){}//return m_position->UnlockInquiry();}

	const unsigned int& GetClosePriceCompact() const{return m_closePriceCompact;}
	const Price& GetClosePrice() const{return m_closePrice;}

	const unsigned int& GetLevel2BidCompact() const{return m_level2BidCompact;}
	const unsigned int& GetLevel2BidSize() const{return m_level2BidSize;}

	const unsigned int& GetPeBidTotalSize() const{return m_peBidTotalSize;}
	const unsigned int& GetPeBidMaxSize() const{return m_peBidMaxSize;}
	const unsigned int& GetPeBidPriceWithMaxSizeCompact() const{return m_peBidPriceWithMaxSizeCompact;}

	const unsigned int& GetLevel2AskCompact() const{return m_level2AskCompact;}
	const unsigned int& GetLevel2AskSize() const{return m_level2AskSize;}

	const unsigned int& GetPeAskTotalSize() const{return m_peAskTotalSize;}
	const unsigned int& GetPeAskMaxSize() const{return m_peAskMaxSize;}
	const unsigned int& GetPeAskPriceWithMaxSizeCompact() const{return m_peAskPriceWithMaxSizeCompact;}

	const SignedPrice& GetLevel2Spread() const{return m_level2Spread;}

	void SetOrdinal(const unsigned int& ordinal){m_ordinal = ordinal;}
	const unsigned int& GetOrdinal() const{return m_ordinal;}
	void IncrementOrdinal(){++m_ordinal;}
	void IncrementOrdinal(unsigned int number){m_ordinal += number;}
	void DecrementOrdinal(){if(m_ordinal)--m_ordinal;}
	void DecrementOrdinal(unsigned int number){if(m_ordinal > number)m_ordinal -= number; else m_ordinal = 0;}

	void UpdatePositionTotals(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff);
//	virtual void PositionValuesChanged(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff) override;

	void Repaint(int left = -1, int right = -1);

	bool isMyBookId(const unsigned char& bookId) const
	{
		return m_bookId == bookId || m_attributedBookId == bookId;
	}

protected:
	virtual void UpdatePositionOnAccountChange()//this function is called in main thread. Send a custom message to account thread
	{
/*
		Money closedPnlDiff, openPnlDiff, openPrintPnlDiff;
		if(UpdatePositionValues(closedPnlDiff, openPnlDiff, openPrintPnlDiff))
		{
			PositionValuesChanged(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
		}
*/
	}
	virtual bool DoInitialize();
	void DoInitializeWithoutLocking();

//	void UpdateLevel2BidInThread();
//	void UpdateLevel2AskInThread();
	bool UpdateBookBidValues();
	bool UpdateBookAskValues();

	void UpdateBookBidValues(TakionIterator* iterator,
		unsigned int& peTotalSize,
		unsigned int& peMaxSize,
		unsigned int& pePriceWithMaxSizeCompact);

	void UpdateBookAskValues(TakionIterator* iterator,
		unsigned int& peTotalSize,
		unsigned int& peMaxSize,
		unsigned int& pePriceWithMaxSizeCompact);

	bool UpdateBidValues();
	bool UpdateAskValues();

	void SetBook(const Book* const& book)
	{
		if(book != m_book)
		{
			if(m_book)
			{
				delete m_bidBookIterator;
				m_bidBookIterator = nullptr;
				delete m_askBookIterator;
				m_askBookIterator = nullptr;
				m_bookId = ECNBOOK_COUNT;
			}
			m_book = book;
			if(book)
			{
				m_bidBookIterator = book->CreateBookIterator(true, true);
				m_askBookIterator = book->CreateBookIterator(false, true);
				m_bookId = book->GetBookId();
			}
		}
	}

	void SetAttributedBook(const Book* const& book)
	{
		if(book != m_attributedBook)
		{
			if(m_attributedBook)
			{
				delete m_bidAttributedBookIterator;
				m_bidAttributedBookIterator = nullptr;
				delete m_askAttributedBookIterator;
				m_askAttributedBookIterator = nullptr;
				m_attributedBookId = ECNBOOK_COUNT;
			}
			m_attributedBook = book;
			if(book)
			{
				m_bidAttributedBookIterator = book->CreateBookIterator(true, true);
				m_askAttributedBookIterator = book->CreateBookIterator(false, true);
				m_attributedBookId = book->GetBookId();
			}
		}
	}

	bool DoResetBookValues();

	virtual void DoReset() override;
	virtual void SecuritySet() override
	{
		if(m_security)
		{
			SetBook(m_security->GetPrimaryBook());//m_security->GetBook(ECNBOOK_NYS);
			SetAttributedBook(m_security->GetPrimaryAttributedBook());
		}
		else
		{
			SetBook(nullptr);
			SetAttributedBook(nullptr);
		}
	}
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
	unsigned int m_ordinal;//ordinal number in the list box.
	ListBoxStockLevel2* m_owner;
/*
	Price m_closePrice;
	Price m_level2Bid;
	Price m_level2Ask;
*/
	unsigned int m_closePriceCompact;
	Price m_closePrice;

	unsigned int m_level2BidCompact;
	unsigned int m_level2BidSize;

	unsigned int m_peBidTotalSize;
	unsigned int m_peBidMaxSize;
	unsigned int m_peBidPriceWithMaxSizeCompact;

	unsigned int m_level2AskCompact;
	unsigned int m_level2AskSize;

	unsigned int m_peAskMaxSize;
	unsigned int m_peAskTotalSize;
	unsigned int m_peAskPriceWithMaxSizeCompact;

	SignedPrice m_level2Spread;

	const Book* m_book;
	TakionIterator* m_bidBookIterator;
	TakionIterator* m_askBookIterator;
	unsigned char m_bookId;

	const Book* m_attributedBook;
	TakionIterator* m_bidAttributedBookIterator;
	TakionIterator* m_askAttributedBookIterator;
	unsigned char m_attributedBookId;

	bool m_calculateL2;
/*
	const Account* m_account;
	const Position* m_position;
	const Security* m_security;
	unsigned __int64 m_numericSymbol;
	char m_marketCategory;
	bool m_loaded;
	bool m_initialized;

	unsigned int m_roundLot;

	int m_positionSize;
	unsigned int m_buyPendingShares;
	unsigned int m_sellPendingShares;
	unsigned int m_positionEntryTimestamp;
	unsigned int m_lastCloseTimestamp;
	Price m_positionPrice;
	Price m_lastExecutionPrice;
	Money m_closedPnl;
	Money m_openPnl;
	Money m_openPnlPrint;

	virtual void PositionEntered(){}
	virtual void PositionFlat(){}
	virtual void PositionFlipped(){}
*/
};

class LbStockLevel2HorizontalScroller : public LbHorizontalScroller
{
public:
	LbStockLevel2HorizontalScroller(ListBoxStockLevel2* listBox);
	virtual void AddColumns() override;
protected:
	ListBoxStockLevel2* m_listBoxLevel2;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LbStockLevel2HorizontalScroller::LbStockLevel2HorizontalScroller(ListBoxStockLevel2* listBox):
	LbHorizontalScroller(listBox),
	m_listBoxLevel2(listBox)
{
}

void LbStockLevel2HorizontalScroller::AddColumns()
{
	const int itemHeight = m_listBox->GetMyItemHeight();
	const int doubleHeight = itemHeight + itemHeight;
	const int tripleHeight = doubleHeight + itemHeight;
	const int fourHeight = tripleHeight + itemHeight;
	const int fiveHeight = fourHeight + itemHeight;
	const int sixHeight = fiveHeight + itemHeight;
	AddColumn("Symbol", L2T_SYMBOL, fiveHeight, DT_LEFT, DT_LEFT, DT_RIGHT, "Symbol");//, 0xFFFFFFFF, itemHeight, itemHeight);

#ifdef L2T_SHOW_ORDINALS
	AddColumn("#Line", L2T_LINE_ORDINAL, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Line Ordinal Number");
	AddColumn("#Stock", L2T_STOCK_ORDINAL, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Stock Ordinal Number");
#endif

	AddColumn("Close", L2T_CLOSE_PRICE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Yesterday's Close Price");

	AddColumn("Spread", L2T_LEVEL2_SPREAD, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level2 Spread (Level2 Ask - Level2 Bid)");

	AddColumn("Bid", L2T_LEVEL2_BID, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level2 Bid");
	AddColumn("BidSize", L2T_LEVEL2_BID_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level2 Bid Size");

	AddColumn("PeBidTSz", L2T_TOTAL_PE_BID_SIZE_ABOVE_CLOSE_PRICE, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Primary Exchange Bid Total Size above Close Price");
	AddColumn("PeBidMSz", L2T_MAX_PE_BID_SIZE_ABOVE_CLOSE_PRICE, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Primary Exchange Bid Max Size above Close Price");
	AddColumn("$PeBidMSz", L2T_PE_BID_ABOVE_CLOSE_PRICE_WITH_MAX_SIZE, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Primary Exchange Bid Price above Close Price with Max Size");

	AddColumn("Ask", L2T_LEVEL2_ASK, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level2 Ask");
	AddColumn("AskSize", L2T_LEVEL2_ASK_SIZE, fourHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Level2 Ask Size");

	AddColumn("PeAskTSz", L2T_TOTAL_PE_ASK_SIZE_BELOW_CLOSE_PRICE, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Primary Exchange Ask Total Size below Close Price");
	AddColumn("PeAskMSz", L2T_MAX_PE_ASK_SIZE_BELOW_CLOSE_PRICE, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Primary Exchange Ask Max Size below Close Price");
	AddColumn("$PeAskMSz", L2T_PE_ASK_BELOW_CLOSE_PRICE_WITH_MAX_SIZE, sixHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Primary Exchange Ask Price below Close Price with Max Size");
}
/*
unsigned char StockLevel2::SendMarketOrder(char side, unsigned int orderSize, bool close)
{
	if(orderSize)
	{
		if(m_account)
		{
			if(m_account->isSimulation())//SIMULATION only
			{
				unsigned int destinationId = SPEED_ROUTE;
				unsigned int destinationName = qstkNum;
//				const Price& zeroPrice = TU_GetPriceZero();
//				const SignedPrice& zeroSignedPrice = TU_GetSignedPriceZero();
				bool preBorrow = false;
				bool imbalanceOnly = false;
				const Price& preBorrowPrice = Price::priceZero;//TU_GetPriceZero();
				unsigned char errorCode = ((Account*)m_account)->SendOrder(
					nullptr,//unsigned int* orderClientId - output parameter
					nullptr,//Order* orderToreplace
					true,//replaceNative
					0,//int replaceSizeOffset
					m_security,
					0,//timeToActivate
					PE_NOT_POSTPONED,
					destinationName,
					destinationId,
					side,
					ORDER_TYPE_MARKET,
					0,//userOrderType
					nullptr,//userOrderInfo
					Price::priceZero,//limitPrice,
					Price::priceZero,//discretionaryPrice,
					Price::priceZero,//signedStopPrice,
					OST_NONE,//venueOrderParams.GetStopType(),
					OSB_BID,//venueOrderParams.GetStopBase(),
					m_level1Bid,
					m_level1Ask,
					m_lastPrint,
					orderSize,
					orderSize,//displaySize,
					0,//venueOrderParams.GetReserveSize(),
					RI_PROACTIVE,//routingId,
					0,//routingSubType,
					TD_GetDefaultProActiveRoutingNumericName(),//*(unsigned int*)"ProA",//venueOrderParams.GetRoutingName(),
					0,//venueOrderParams.GetMnemonic(),
					false,//iso,
					PEG_NONE,//pegType,
					SignedPrice::oneCent,//pegOffset,
					TIF_IOC,//tifType,
					0,//tifMillisecond,
					true,//cancelWashOrders,
					OOP_REJECT,//oversellSplitPolicy,
					!preBorrow,//resizeShortToBorrowed,
					preBorrow,
					preBorrowPrice,
					false,//noRedirect,
					close,//false,//closeOnly,
					close,//false,//resizeToClose,
					imbalanceOnly,
//					true,//proAts
					!close,//true,//blockAggressiveOrders,
					RLP_ROUND_DOWN,//roundLotPolicy,
					nullptr,//iso ? &isoAttachment : nullptr,
					0,//venueOrderParams.GetUserId());
					0,//parentId
					0);//ocoId

				if(errorCode != OE_OK)
				{
					m_owner->LogOrderError(errorCode, "Send Order");
				}
				return errorCode;
			}
			return OE_WRONG_ACCOUNT;
		}
		return OE_NO_CURRENT_ACCOUNT;
	}
	return OE_SIZE_ZERO;
}
*/

#if defined(T_RELEASED) || defined(_DEBUG)
#define MY_STOCK_SYMBOL ""
#endif

bool StockLevel2::UpdateBidValues()
{
	if(m_security)// && m_security->LockInquiryLevel2())//No need to lock. Because it's a worker Thread
	{
#ifdef MY_STOCK_SYMBOL
const char* symbol = MY_STOCK_SYMBOL;
if(!*symbol)symbol = TGUI_GetActiveSecuritySymbolPtr();
const bool myStock = symbol && !strcmp(m_security->GetSymbol(), symbol);
if(myStock)
{
printf("X");
}
#endif
//		const Price level2Bid = Price(m_security->GetL2Bid());
//		const Price level2Ask = Price(m_security->GetL2Ask());
		unsigned int level2BidCompact;
		unsigned int level2BidSize;

		const bool calculateL2 = m_owner->isCalculateL2();

		if(calculateL2)
		{
			if(m_security->isCalculateLevel2Top())
			{
				level2BidCompact = m_security->GetL2Bid();
				level2BidSize = m_security->GetL2BidSize();
			}
			else
			{
				Price l2TopPrice;
				level2BidSize = m_security->CalculateL2BidPriceAndSize(l2TopPrice);
				level2BidCompact = l2TopPrice.GetCompactPrice();
			}
		}
		else
		{
			level2BidCompact = m_security->GetL1Bid();
			level2BidSize = 0;
		}
//		m_security->UnlockInquiryLevel2();

		int left;
		int right;
		int leftResult = m_owner->GetListBoxWidth();
		int rightResult = 0;

		bool changed = level2BidCompact != m_level2BidCompact;
	
		if(changed)
		{
			m_level2BidCompact = level2BidCompact;
//			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_LEVEL2_BID, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
			const bool spreadValid = m_level2AskCompact && level2BidCompact;
			SignedPrice spread;
			if(spreadValid)
			{
				spread = Price(m_level2AskCompact) - Price(level2BidCompact);
			}
			if(spread != m_level2Spread)
			{
				m_level2Spread = spread;
#if defined(T_RELEASED) || defined(_DEBUG)
if(!m_level2Spread.isPositive() && spreadValid)
{
	std::string level2SpreadStr;
	U_SignedPriceToStr(level2SpreadStr, m_level2Spread, 4, '\0', true, '\0', true);
	printf("%s", level2SpreadStr.c_str());
}
#endif
				if(m_owner->GetColumnBounds(L2T_LEVEL2_SPREAD, left, right))
				{
					if(leftResult > left)leftResult = left;
					if(rightResult < right)rightResult = right;
				}
			}
		}

		if(level2BidSize != m_level2BidSize)
		{
			m_level2BidSize = level2BidSize;
			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_LEVEL2_BID_SIZE, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
		}

//Invalidate a rectangle for repainting. InvalidateItem is thread safe. Actual painting is done by the operating system
#ifdef MY_STOCK_SYMBOL
if(myStock)
{
printf("X");
}
#endif
		if(changed && leftResult < rightResult)// && rightResult > 0)
		{
//				if(leftResult < 0)leftResult = 0;
			m_owner->InvalidateItem(m_ordinal, leftResult, rightResult);
		}

		if(calculateL2 != m_calculateL2)
		{
			m_calculateL2 = calculateL2;
			if(UpdateAskValues())
			{
				changed = true;
			}
		}
		return changed;
	}
	return false;
}

bool StockLevel2::UpdateAskValues()
{
	if(m_security)// && m_security->LockInquiryLevel2())//No need to lock. Because it's a worker Thread
	{
#ifdef MY_STOCK_SYMBOL
const char* symbol = MY_STOCK_SYMBOL;
if(!*symbol)symbol = TGUI_GetActiveSecuritySymbolPtr();
const bool myStock = symbol && !strcmp(m_security->GetSymbol(), symbol);
//const bool myStock = !strcmp(m_security->GetSymbol(), MY_STOCK_SYMBOL);
if(myStock)
{
printf("X");
}
#endif
//		const Price level2Bid = Price(m_security->GetL2Bid());
//		const Price level2Ask = Price(m_security->GetL2Ask());
		unsigned int level2AskCompact;
		unsigned int level2AskSize;

		const bool calculateL2 = m_owner->isCalculateL2();

		if(calculateL2)
		{
			if(m_security->isCalculateLevel2Top())
			{
				level2AskCompact = m_security->GetL2Ask();
				level2AskSize = m_security->GetL2AskSize();
			}
			else
			{
				Price l2TopPrice;
				level2AskSize = m_security->CalculateL2AskPriceAndSize(l2TopPrice);
				level2AskCompact = l2TopPrice.GetCompactPrice();
			}
		}
		else
		{
			level2AskCompact = m_security->GetL1Ask();
			level2AskSize = 0;
		}

//		m_security->UnlockInquiryLevel2();

		int left;
		int right;
		int leftResult = m_owner->GetListBoxWidth();
		int rightResult = 0;

		bool changed = level2AskCompact != m_level2AskCompact;

		if(changed)
		{
			m_level2AskCompact = level2AskCompact;
//			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_LEVEL2_ASK, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
			const bool spreadValid = level2AskCompact && m_level2BidCompact;
			SignedPrice spread;
			if(spreadValid)
			{
				spread = Price(level2AskCompact) - Price(m_level2BidCompact);
			}
			if(spread != m_level2Spread)
			{
				m_level2Spread = spread;
#if defined(T_RELEASED) || defined(_DEBUG)
if(!spread.isPositive() && spreadValid)
{
	std::string level2SpreadStr;
	U_SignedPriceToStr(level2SpreadStr, m_level2Spread, 4, '\0', true, '\0', true);
	printf("%s", level2SpreadStr.c_str());
}
#endif
				if(m_owner->GetColumnBounds(L2T_LEVEL2_SPREAD, left, right))
				{
					if(leftResult > left)leftResult = left;
					if(rightResult < right)rightResult = right;
				}
			}
		}

		if(level2AskSize != m_level2AskSize)
		{
			m_level2AskSize = level2AskSize;
			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_LEVEL2_ASK_SIZE, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
		}
//Invalidate a rectangle for repainting. InvalidateItem is thread safe. Actual painting is done by the operating system
#ifdef MY_STOCK_SYMBOL
if(myStock)
{
printf("X");
}
#endif
		if(changed && leftResult < rightResult)// && rightResult > 0)
		{
			m_owner->InvalidateItem(m_ordinal, leftResult, rightResult);
		}

		if(calculateL2 != m_calculateL2)
		{
			m_calculateL2 = calculateL2;
			if(UpdateBidValues())
			{
				changed = true;
			}
		}
		return changed;
	}
	return false;
}

void StockLevel2::UpdateBookBidValues(TakionIterator* iterator,
	unsigned int& peTotalSize,
	unsigned int& peMaxSize,
	unsigned int& pePriceWithMaxSizeCompact)
{
	unsigned int size;
	const Quote* quote;

//	bool changed = false;
	iterator->Reset();
	while(quote = TU_GetNextQuote(iterator))
	{
		if(m_closePrice <= *quote)
		{
			size = quote->GetQuantity();
			peTotalSize += size;
			if(peMaxSize < size)
			{
				peMaxSize = size;
				pePriceWithMaxSizeCompact = quote->GetCompactPrice();
			}
		}
		else
		{
			break;
		}
	}
}

void StockLevel2::UpdateBookAskValues(TakionIterator* iterator,
	unsigned int& peTotalSize,
	unsigned int& peMaxSize,
	unsigned int& pePriceWithMaxSizeCompact)
{
	unsigned int size;
	const Quote* quote;

//	bool changed = false;
	iterator->Reset();
	while(quote = TU_GetNextQuote(iterator))
	{
		if(m_closePrice >= *quote)
		{
			size = quote->GetQuantity();
			peTotalSize += size;
			if(peMaxSize < size)
			{
				peMaxSize = size;
				pePriceWithMaxSizeCompact = quote->GetCompactPrice();
			}
		}
		else
		{
			break;
		}
	}
}

bool StockLevel2::UpdateBookBidValues()
{
	if(m_closePriceCompact)
	{
		unsigned int peTotalSize = 0;
		unsigned int peMaxSize = 0;
		unsigned int pePriceWithMaxSizeCompact = 0;

		if(m_bidBookIterator)
		{
			UpdateBookBidValues(m_bidBookIterator, peTotalSize, peMaxSize, pePriceWithMaxSizeCompact);
		}
		if(m_bidAttributedBookIterator)
		{
			UpdateBookBidValues(m_bidAttributedBookIterator, peTotalSize, peMaxSize, pePriceWithMaxSizeCompact);
		}

		int leftResult = m_owner->GetListBoxWidth();
		int rightResult = 0;
		int left;
		int right;
		bool changed = peTotalSize != m_peBidTotalSize;
		if(changed)
		{
			m_peBidTotalSize = peTotalSize;
//			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_TOTAL_PE_BID_SIZE_ABOVE_CLOSE_PRICE, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
		}

		if(peMaxSize != m_peBidMaxSize)
		{
			m_peBidMaxSize = peMaxSize;
			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_MAX_PE_BID_SIZE_ABOVE_CLOSE_PRICE, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
		}

		if(pePriceWithMaxSizeCompact != m_peBidPriceWithMaxSizeCompact)
		{
			m_peBidPriceWithMaxSizeCompact = pePriceWithMaxSizeCompact;
			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_PE_BID_ABOVE_CLOSE_PRICE_WITH_MAX_SIZE, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
		}

//Invalidate a rectangle for repainting. InvalidateItem is thread safe. Actual painting is done by the operating system
		if(changed && leftResult < rightResult)// && rightResult > 0)
		{
			m_owner->InvalidateItem(m_ordinal, leftResult, rightResult);
		}
		return changed;
	}
	return false;
}

bool StockLevel2::UpdateBookAskValues()
{
	if(m_closePriceCompact)
	{
		unsigned int peTotalSize = 0;
		unsigned int peMaxSize = 0;
		unsigned int pePriceWithMaxSizeCompact = 0;

		if(m_askBookIterator)
		{
			UpdateBookAskValues(m_askBookIterator, peTotalSize, peMaxSize, pePriceWithMaxSizeCompact);
		}
		if(m_askAttributedBookIterator)
		{
			UpdateBookAskValues(m_askAttributedBookIterator, peTotalSize, peMaxSize, pePriceWithMaxSizeCompact);
		}

		int leftResult = m_owner->GetListBoxWidth();
		int rightResult = 0;

		int left;
		int right;

		bool changed = peTotalSize != m_peAskTotalSize;
		if(changed)
		{
			m_peAskTotalSize = peTotalSize;
//			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_TOTAL_PE_ASK_SIZE_BELOW_CLOSE_PRICE, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
		}

		if(peMaxSize != m_peAskMaxSize)
		{
			m_peAskMaxSize = peMaxSize;
			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_MAX_PE_ASK_SIZE_BELOW_CLOSE_PRICE, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
		}

		if(pePriceWithMaxSizeCompact != m_peAskPriceWithMaxSizeCompact)
		{
			m_peAskPriceWithMaxSizeCompact = pePriceWithMaxSizeCompact;
			changed = true;
//Calculation of the ListBox rectangle to Invalidate
//No need to do it if stocks are not displayed in a window and must be repainted
//GetColumnBounds should be thread safe since columns are added at startup and stay the same.
			if(m_owner->GetColumnBounds(L2T_PE_ASK_BELOW_CLOSE_PRICE_WITH_MAX_SIZE, left, right))
			{
				if(leftResult > left)leftResult = left;
				if(rightResult < right)rightResult = right;
			}
		}
//Invalidate a rectangle for repainting. InvalidateItem is thread safe. Actual painting is done by the operating system
		if(changed && leftResult < rightResult)// && rightResult > 0)
		{
			m_owner->InvalidateItem(m_ordinal, leftResult, rightResult);
		}
		return changed;
	}
	return false;
}

void StockLevel2::DoInitializeWithoutLocking()
{
	SetBook(m_security->GetPrimaryBook());
	m_closePriceCompact = m_security->GetClosePriceCompact();
	if(m_owner->isCalculateL2())
	{
		m_level2BidCompact = m_security->GetL2Bid();
		m_level2AskCompact = m_security->GetL2Ask();
		m_level2BidSize = m_security->GetL2BidSize();
		m_level2AskSize = m_security->GetL2AskSize();
	}
	else
	{
		m_level2BidCompact = m_security->GetL1Bid();
		m_level2AskCompact = m_security->GetL1Ask();
		m_level2BidSize = 0;
		m_level2AskSize = 0;
	}
	const bool spreadValid = m_level2AskCompact && m_level2BidCompact;
	if(spreadValid)
	{
		m_level2Spread = Price(m_level2AskCompact) - Price(m_level2BidCompact);
	}
	else
	{
		m_level2Spread.SetZero();
	}
#ifdef _DEBUG
if(!m_level2Spread.isPositive() && spreadValid)
{
	std::string level2SpreadStr;
	U_SignedPriceToStr(level2SpreadStr, m_level2Spread, 4, '\0', true, '\0', true);
	printf("%s", level2SpreadStr.c_str());
}
#endif

	m_closePrice.SetCompactValue(m_closePriceCompact);

	m_peBidTotalSize = 0;
	m_peBidMaxSize = 0;
	m_peBidPriceWithMaxSizeCompact = 0;

	if(m_bidBookIterator)
	{
		UpdateBookBidValues(m_bidBookIterator, m_peBidTotalSize, m_peBidMaxSize, m_peBidPriceWithMaxSizeCompact);
	}
	if(m_bidAttributedBookIterator)
	{
		UpdateBookBidValues(m_bidAttributedBookIterator, m_peBidTotalSize, m_peBidMaxSize, m_peBidPriceWithMaxSizeCompact);
	}

	m_peAskTotalSize = 0;
	m_peAskMaxSize = 0;
	m_peAskPriceWithMaxSizeCompact = 0;

	if(m_askBookIterator)
	{
		UpdateBookAskValues(m_askBookIterator, m_peAskTotalSize, m_peAskMaxSize, m_peAskPriceWithMaxSizeCompact);
	}
	if(m_askAttributedBookIterator)
	{
		UpdateBookAskValues(m_askAttributedBookIterator, m_peAskTotalSize, m_peAskMaxSize, m_peAskPriceWithMaxSizeCompact);
	}
}

bool StockLevel2::DoInitialize()
{
	if(m_security)
	{
		if(m_security->LockInquiryLevel2())
		{
			DoInitializeWithoutLocking();
			m_security->UnlockInquiryLevel2();
		}
		else
		{
			m_closePriceCompact = m_security->GetClosePriceCompact();
			m_closePrice.SetCompactValue(m_closePriceCompact);
		}
		return true;
	}
	return false;
}

bool StockLevel2::DoResetBookValues()
{
	bool changed = false;
	if(m_peBidTotalSize)
	{
		m_peBidTotalSize = 0;
		changed = true;
	}
	if(m_peBidMaxSize)
	{
		m_peBidMaxSize = 0;
		changed = true;
	}
	if(m_peBidPriceWithMaxSizeCompact)
	{
		m_peBidPriceWithMaxSizeCompact = 0;
		changed = true;
	}

	if(m_peAskTotalSize)
	{
		m_peAskTotalSize = 0;
		changed = true;
	}
	if(m_peAskMaxSize)
	{
		m_peAskMaxSize = 0;
		changed = true;
	}
	if(m_peAskPriceWithMaxSizeCompact)
	{
		m_peAskPriceWithMaxSizeCompact = 0;
		changed = true;
	}
	return changed;
}

void StockLevel2::DoReset()
{
	StockBase::DoReset();

	m_closePriceCompact = 0;//.SetZero();
	m_closePrice.SetZero();

	m_level2Spread.SetZero();

	m_level2BidCompact = 0;//.SetZero();
	m_level2BidSize = 0;

	m_level2AskCompact = 0;//.SetZero();
	m_level2AskSize = 0;

	DoResetBookValues();

	Repaint();
}

void StockLevel2::UpdatePositionTotals(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff)
{
//	m_owner->UpdatePositionTotals(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
}

void ListBoxStockLevel2::ResetTotals()
{
	m_owner->RepaintCount();
/*
	if(!m_closedPnl.isZero())
	{
		m_closedPnl.SetZero();
		m_owner->RepaintClosedPnl();
	}
	if(!m_openPnl.isZero())
	{
		m_openPnl.SetZero();
		m_owner->RepaintOpenPnl();
	}
	if(!m_openPrintPnl.isZero())
	{
		m_openPrintPnl.SetZero();
		m_owner->RepaintOpenPrintPnl();
	}
*/
}
/*
void ListBoxStockLevel2::UpdatePositionTotals(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff)
{
	if(!closedPnlDiff.isZero())
	{
		m_closedPnl += closedPnlDiff;
		m_owner->RepaintClosedPnl();
	}
	if(!openPnlDiff.isZero())
	{
		m_openPnl += openPnlDiff;
		m_owner->RepaintOpenPnl();
	}
	if(!openPrintPnlDiff.isZero())
	{
		m_openPrintPnl += openPrintPnlDiff;
		m_owner->RepaintOpenPrintPnl();
	}
}
*/
/*
void ListBoxStockLevel2::CancelOpeningOrders()
{
	if(m_position)
	{
		unsigned char sideFilter;
		if(m_positionSize > 0)
		{
			sideFilter = 1 << TU_ORDER_SIDE_BUY;
		}
		else if(m_positionSize < 0)
		{
			sideFilter = (1 << TU_ORDER_SIDE_SELL) | (1 << TU_ORDER_SIDE_SHORT);
		}
		else
		{
			sideFilter = (1 << TU_ORDER_SIDE_BUY) | (1 << TU_ORDER_SIDE_SELL) | (1 << TU_ORDER_SIDE_SHORT);
		}
//		Position::UIntSet uintSet;
		((Position*)m_position)->CancelOrders(sideFilter,
			TU_ORDER_CANCEL_ALL,
			TIFMASK_ALL,
			ALGORITHMMASK_ALL,
			YNM_ALL,
			YNM_ALL,
			true,
			nullptr);//uintSet);
	}
}

void ListBoxStockLevel2::ClosePosition()
{
	if(m_positionSize)
	{
		unsigned int absSize = (unsigned int)m_positionSize;
		if(m_positionSize > 0)//long position
		{
			if(absSize > m_sellPendingShares)
			{
				SendMarketOrder('S', absSize - m_sellPendingShares, true);
			}
		}
		else//short position
		{
			if(absSize > m_buyPendingShares)
			{
				SendMarketOrder('B', absSize - m_buyPendingShares, true);
			}
		}
	}
}

void StockLevel2::PositionValuesChanged(const Money& closedPnlDiff, const Money& openPnlDiff, const Money& openPrintPnlDiff)
{
//	Repaint(m_owner->GetPositionSizeLeft(), m_owner->GetOpenPnlPrintRight());
	UpdatePositionTotals(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
}
*/

//In main thread you use message TM_STOCK_UPDATE_LEVEL2 which is sent on any level2 change.
//The next TM_STOCK_UPDATE_LEVEL2 messages are not distributed to Observers and are thrown away until the previous TM_STOCK_UPDATE_LEVEL2 is processed by Observers.
//So, when the TM_STOCK_UPDATE_LEVEL2 reaches an Observer there are several changes done to the stock's Level2, and some of the changes are done after the TM_STOCK_UPDATE_LEVEL2 was sent.
//In main thread you have to look at the entire Level2 to find out what changes were done (if you saved the previous Level2 state).
//What's different for InThread processing, you get detailed messages for each Level2 change and you can take a quick decision if the change is interesting to you.
void StockLevel2::Notify(const Message* message, const Observable* from, const Message* info)
{
#ifdef MY_STOCK_SYMBOL
const char* symbol = MY_STOCK_SYMBOL;
if(!*symbol)symbol = TGUI_GetActiveSecuritySymbolPtr();
const bool myStock = symbol && !strcmp(m_security->GetSymbol(), symbol);
//const bool myStock = !strcmp(m_security->GetSymbol(), MY_STOCK_SYMBOL);
if(myStock)
{
printf("X");
}
#endif
	switch(message->m_type)
	{
//		case TM_SORTABLE_SECURITY_ENTITLEMENTS_CHANGED:
//		case TM_SORTABLE_SECURITY_EXT_ENTITLEMENTS_CHANGED:
		case TM_STOCK_ENTITLEMENTS_CHANGED:
		case TM_STOCK_EXT_ENTITLEMENTS_CHANGED:
#ifndef TAKION_NO_OPTIONS
		case TM_FUTURE_ENTITLEMENTS_CHANGED:
		case TM_FUTURE_EXT_ENTITLEMENTS_CHANGED:
		case TM_OPTION_ENTITLEMENTS_CHANGED:
		case TM_OPTION_EXT_ENTITLEMENTS_CHANGED:
#endif
		case M_SNAPSHOT:
		case SM_M_SNAPSHOT:
//		case M_FUT_SNAPSHOT:
		UpdateBidValues();
		UpdateAskValues();
		UpdateBookBidValues();
		UpdateBookAskValues();
		break;

#ifndef TAKION_NO_OPTIONS
		case M_FUT_LEVEL1://Level1 Prices
		if(!m_owner->isCalculateL2())
		{
			const TMsgFutLevel1* msg = (const TMsgFutLevel1*)message;

			if(Price::CalculateCompactPrice(msg->m_bidPriceDollars, msg->m_bidPriceFraction) != m_level2BidCompact)
			UpdateBidValues();

			if(Price::CalculateCompactPrice(msg->m_askPriceDollars, msg->m_askPriceFraction) != m_level2AskCompact)
			UpdateAskValues();
		}
		break;

		case M_OPT_LEVEL1://Level1 Prices
		if(!m_owner->isCalculateL2())
		{
			const TMsgOptLevel1* msg = (const TMsgOptLevel1*)message;

			if(Price::CalculateCompactPrice(msg->m_bidPriceDollars, msg->m_bidPriceFraction) != m_level2BidCompact)
			UpdateBidValues();

			if(Price::CalculateCompactPrice(msg->m_askPriceDollars, msg->m_askPriceFraction) != m_level2AskCompact)
			UpdateAskValues();
		}
		break;
#endif

		case M_LEVEL1:
		if(!m_owner->isCalculateL2())
		{
			const TMsgLevel1* msg = (const TMsgLevel1*)message;
//			unsigned int compactPrice = Price::CalculateCompactPrice(msg->m_bidDollars, msg->m_bidFraction);

			if(Price::CalculateCompactPrice(msg->m_bidDollars, msg->m_bidFraction) != m_level2BidCompact)
			UpdateBidValues();

			if(Price::CalculateCompactPrice(msg->m_askDollars, msg->m_askFraction) != m_level2AskCompact)
			UpdateAskValues();
		}
		break;

		case SM_M_LEVEL1:
		if(!m_owner->isCalculateL2())
		{
			const TSmMsgLevel1* msg = (const TSmMsgLevel1*)message;

			if(Price::CalculateCompactPrice(msg->m_bidDollars, msg->m_bidFraction) != m_level2BidCompact)
			UpdateBidValues();

			if(Price::CalculateCompactPrice(msg->m_askDollars, msg->m_askFraction) != m_level2AskCompact)
			UpdateAskValues();
		}
		break;

		case M_BOOK_NEW_QUOTE:
		{
			const TMsgBookNewQuote* msg = (const TMsgBookNewQuote*)message;
			const MsgQuote& quote = msg->m_quote;
			const unsigned int compactPrice = Price::CalculateCompactPrice(quote.m_priceDollars, quote.m_priceFraction);
			if(msg->m_side == Buy)
			{
				if(m_owner->isCalculateL2() && compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote.m_bookID == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(m_owner->isCalculateL2() && (compactPrice <= m_level2AskCompact || !m_level2AskCompact))
				UpdateAskValues();

				if(quote.m_bookID == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;
#ifndef TAKION_NO_OPTIONS
		case M_FUT_BOOK_MODIFY_QUOTE:
		case M_OPT_LEVEL2:
#endif
		case M_BOOK_MODIFY_QUOTE:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TMsgBookModifyQuote* msg = (const TMsgBookModifyQuote*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPriceOld = quote->m_oldPrice.GetCompactPrice();// Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			const unsigned int compactPriceNew = quote->m_newPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_newPriceDollars, quote->m_newPriceFraction);
			if(msg->m_side == Buy)
			{
				if(m_owner->isCalculateL2() && (compactPriceOld >= m_level2BidCompact || compactPriceNew >= m_level2BidCompact))
				UpdateBidValues();

				if(m_closePriceCompact && quote->m_bookId == m_bookId && (compactPriceOld >= m_closePriceCompact || compactPriceNew >= m_closePriceCompact))
				UpdateBookBidValues();
			}
			else
			{
				if(m_owner->isCalculateL2() && (compactPriceOld && compactPriceOld <= m_level2AskCompact || compactPriceNew && compactPriceNew <= m_level2AskCompact || !m_level2AskCompact))
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && (compactPriceOld && compactPriceOld <= m_closePriceCompact || compactPriceNew && compactPriceNew <= m_closePriceCompact))
				UpdateBookAskValues();
			}
		}
		break;

		case M_BOOK_DELETE_QUOTE:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TMsgBookDeleteQuote* msg = (const TMsgBookDeleteQuote*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPrice = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			if(msg->m_side == Buy)
			{
				if(m_owner->isCalculateL2() && compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote->m_bookId == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(m_owner->isCalculateL2() && (compactPrice <= m_level2AskCompact || !m_level2AskCompact))
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

#ifndef TAKION_NO_OPTIONS
		case M_FUT_BOOK_DELETE_QUOTE:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TMsgFutBookDeleteQuote* msg = (const TMsgFutBookDeleteQuote*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPrice = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			if(msg->m_side == Buy)
			{
				if(m_owner->isCalculateL2() && compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote->m_bookId == m_bookId && (compactPrice >= m_closePriceCompact && m_closePriceCompact))
				UpdateBookBidValues();
			}
			else
			{
				if(m_owner->isCalculateL2() && (compactPrice <= m_level2AskCompact || !m_level2AskCompact))
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;
#endif
		case M_BOOK_ORDER_EXECUTED:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TMsgBookOrderExecuted* msg = (const TMsgBookOrderExecuted*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPrice = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			if(msg->m_side == Buy)
			{
				if(m_owner->isCalculateL2() && compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote->m_bookId == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(m_owner->isCalculateL2() && (compactPrice <= m_level2AskCompact || !m_level2AskCompact))
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

		case M_BOOK_ORDER_EXECUTED_WITH_PRICE:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TMsgBookOrderExecutedWithPrice* msg = (const TMsgBookOrderExecutedWithPrice*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPrice = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			if(msg->m_side == Buy)
			{
				if(m_owner->isCalculateL2() && compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote->m_bookId == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(m_owner->isCalculateL2() && (compactPrice <= m_level2AskCompact || !m_level2AskCompact))
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

		case M_BOOK_ORDER_CANCELED:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TMsgBookOrderCanceled* msg = (const TMsgBookOrderCanceled*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPrice = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			if(msg->m_side == Buy)
			{
				if(m_owner->isCalculateL2() && compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote->m_bookId == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(m_owner->isCalculateL2() && (compactPrice <= m_level2AskCompact || !m_level2AskCompact))
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

		case TM_REMOVE_QUOTE://quote is removed manually
		{
			const TMsgRemoveQuote* msg = (const TMsgRemoveQuote*)message;
			const unsigned int compactPriceFrom = Price::CalculateCompactPrice(msg->m_dollarsFrom, msg->m_dollarFractionFrom);
			const unsigned int compactPriceTo = Price::CalculateCompactPrice(msg->m_dollarsTo, msg->m_dollarFractionTo);
			if(msg->m_side)//bool true for bids
			{
				if(m_owner->isCalculateL2() && (compactPriceFrom >= m_level2BidCompact || compactPriceTo >= m_level2BidCompact))
				UpdateBidValues();

				if(m_closePriceCompact && (msg->m_bookId == m_bookId || msg->m_bookId == AllBooks) && (compactPriceFrom >= m_closePriceCompact || compactPriceTo >= m_closePriceCompact))
				UpdateBookBidValues();
			}
			else
			{
				if(m_owner->isCalculateL2() && (compactPriceFrom <= m_level2AskCompact || compactPriceTo <= m_level2AskCompact))
				UpdateAskValues();

				if(m_closePriceCompact && (msg->m_bookId == m_bookId || msg->m_bookId == AllBooks) && compactPriceFrom <= m_closePriceCompact || compactPriceTo <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

		case M_TRADE_REPORT://Print
#ifndef TAKION_NO_OPTIONS
		case M_FUT_TRADE_REPORT:
		case M_OPT_TRADE_REPORT:
		case M_FUT_TRADING_ACTION://Halted
#endif
		case M_TRADE_CANCEL_ERROR://Print Cancel
		case M_TRADE_CORRECTION://Print Correction
		case M_AGGREGATED_PRINT://Aggregated prints come instead of regular prints if you connect to an aggregated server.
		break;

		case M_STOCK_TRADING_ACTION://Halted
		break;

		case M_BOOK_TRADE://Hidden Trade; does not affect quotes
		case M_BOOK_CROSS_TRADE:
		break;

		case M_RESP_REFRESH_SYMBOL://Refreshed (or Invalid Stock Symbol)
		{
			const TMsgRespRefreshSymbol* msg = (const TMsgRespRefreshSymbol*)message;
			if(msg->m_flags & REFRESH_RefreshFailed)
			{
				SetSecurity(nullptr);
			}
			else if(msg->m_flags & REFRESH_EndOfRefresh)//Last chunk of refresh message
			{
				m_loaded = true;
				Initialize();
//				UpdateBidValues();
//				UpdateAskValues();
				m_owner->InvalidateItem(m_ordinal, -1, -1);
			}
		}
		break;

//		case M_RESET_BOOK://The server can send this message if there is a problem
//		break;

		case M_RESET_SYMBOL://The server can send this message if there is a problem
		{
			UpdateBidValues();
			UpdateAskValues();
			const TMsgResetSymbol* msg = (const TMsgResetSymbol*)message;
			if(msg->m_bookId == AllBooks || isMyBookId(msg->m_bookId))
			{
				UpdateBookBidValues();
				UpdateBookAskValues();
			}
		}
		break;

#ifndef TAKION_NO_OPTIONS
		case M_FUT_RESET_SYMBOL://The server can send this message if there is a problem
		{
			UpdateBidValues();
			UpdateAskValues();
			const TMsgFutResetSymbol* msg = (const TMsgFutResetSymbol*)message;
			if(msg->m_level2)
			{
				UpdateBookBidValues();
				UpdateBookAskValues();
			}
		}
		break;

		case M_OPT_RESET_SYMBOL://The server can send this message if there is a problem
		{
			UpdateBidValues();
			UpdateAskValues();
//			const TMsgOptResetSymbol* msg = (const TMsgOptResetSymbol*)message;
//			if(msg->m_bookId == AllBooks || isMyBookId(msg->m_bookId))
//			{
				UpdateBookBidValues();
				UpdateBookAskValues();
//			}
		}
		break;

#endif
		break;

#ifndef TAKION_NO_OPTIONS
		case TM_OPTION_OPENED:
		case TM_FUTURE_OPENED:
		case TM_OPTION_CLOSED:
		case TM_FUTURE_CLOSED:
		case M_FUT_EOD_SUMMARY_REPORT:
		case M_OPT_EOD_SUMMARY_REPORT:
#endif
		case TM_STOCK_OPENED:
		case TM_STOCK_CLOSED:
		case M_EOD_SUMMARY_REPORT:
		case TM_STOCK_YEST_CLOSE_PRICE_CHANGED:
		case M_SHORT_SALE_INDICATORS:
		case M_DIVIDENDS_AND_SPLITS:
		case M_PMI:
		case TM_SET_STOCK_TIER_SIZE:
		case TM_SET_STOCK_CUSTOM_PRICE_BASE:
		case TM_SET_STOCK_CUSTOM_DATE:
		case TM_STOCK_BORROW_PRICE:
		case TM_STOCK_BETA_AND_RETAIL_INFO:
		case TM_STOCK_BORROW_INFO:
		case TM_STOCK_HTB:
		case M_52WEEK_30DAY_HIGH_LOW:
#ifdef LRPS
		case M_LRP:
#endif
		case M_T_TIME:
//		case M_RPI:
		case M_LULD:
#ifndef TAKION_NO_OPTIONS
		case M_FUT_LULD_PRICE_BAND:
#endif
		case M_SECURITY_TYPE:
		break;

		case M_PREVIOUS_DAY_CLOSE://Yesterday's close price corrected
		break;

		case M_AVERAGE_DAILY_VOLUME:
		case M_PREVIOUS_DAY_HIGH_LOW:
		break;

//SymbolMapping variation of the above messages
		case SM_M_BOOK_NEW_QUOTE:
		{
			const TSmMsgBookNewQuote* msg = (const TSmMsgBookNewQuote*)message;
			const MsgQuote& quote = msg->m_quote;
			const unsigned int compactPrice = Price::CalculateCompactPrice(quote.m_priceDollars, quote.m_priceFraction);
			if(msg->m_side == Buy)
			{
				if(compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote.m_bookID == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(compactPrice <= m_level2AskCompact || !m_level2AskCompact)
				UpdateAskValues();

				if(quote.m_bookID == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

		case SM_M_BOOK_MODIFY_QUOTE:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TSmMsgBookModifyQuote* msg = (const TSmMsgBookModifyQuote*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPriceOld = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			const unsigned int compactPriceNew = quote->m_newPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_newPriceDollars, quote->m_newPriceFraction);
			if(msg->m_side == Buy)
			{
				if(compactPriceOld >= m_level2BidCompact || compactPriceNew >= m_level2BidCompact)
				UpdateBidValues();

				if(m_closePriceCompact && quote->m_bookId == m_bookId && (compactPriceOld >= m_closePriceCompact || compactPriceNew >= m_closePriceCompact))
				UpdateBookBidValues();
			}
			else
			{
				if(compactPriceOld && compactPriceOld <= m_level2AskCompact || compactPriceNew && compactPriceNew <= m_level2AskCompact || !m_level2AskCompact)
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && (compactPriceOld && compactPriceOld <= m_closePriceCompact || compactPriceNew && compactPriceNew <= m_closePriceCompact))
				UpdateBookAskValues();
			}
		}
		break;

		case SM_M_BOOK_DELETE_QUOTE:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TSmMsgBookDeleteQuote* msg = (const TSmMsgBookDeleteQuote*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPrice = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			if(msg->m_side == Buy)
			{
				if(compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote->m_bookId == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(compactPrice <= m_level2AskCompact || !m_level2AskCompact)
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

		case SM_M_BOOK_ORDER_EXECUTED:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TSmMsgBookOrderExecuted* msg = (const TSmMsgBookOrderExecuted*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPrice = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			if(msg->m_side == Buy)
			{
				if(compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote->m_bookId == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(compactPrice <= m_level2AskCompact || !m_level2AskCompact)
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

		case SM_M_BOOK_ORDER_EXECUTED_WITH_PRICE:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TSmMsgBookOrderExecutedWithPrice* msg = (const TSmMsgBookOrderExecutedWithPrice*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPrice = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			if(msg->m_side == Buy)
			{
				if(compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote->m_bookId == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(compactPrice <= m_level2AskCompact || !m_level2AskCompact)
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

		case SM_M_BOOK_ORDER_CANCELED:
		if(info && info->m_type == TM_QUOTE_INFO)
		{
			const TSmMsgBookOrderCanceled* msg = (const TSmMsgBookOrderCanceled*)message;
			const TMsgQuoteInfo* quote = (const TMsgQuoteInfo*)info;
			const unsigned int compactPrice = quote->m_oldPrice.GetCompactPrice();//Price::CalculateCompactPrice(quote->m_oldPriceDollars, quote->m_oldPriceFraction);
			if(msg->m_side == Buy)
			{
				if(compactPrice >= m_level2BidCompact)
				UpdateBidValues();

				if(quote->m_bookId == m_bookId && compactPrice >= m_closePriceCompact && m_closePriceCompact)
				UpdateBookBidValues();
			}
			else
			{
				if(compactPrice <= m_level2AskCompact || !m_level2AskCompact)
				UpdateAskValues();

				if(quote->m_bookId == m_bookId && compactPrice <= m_closePriceCompact)
				UpdateBookAskValues();
			}
		}
		break;

		case SM_M_TRADE_REPORT://Print
		case SM_M_TRADE_CANCEL_ERROR://Print Cancel
		case SM_M_TRADE_CORRECTION://Print Correction
		case SM_M_AGGREGATED_PRINT://Aggregated prints come instead of regular prints if you connect to an aggregated server.
		break;

		case SM_M_STOCK_TRADING_ACTION://Halted
		break;

		case SM_M_BOOK_TRADE://Hidden Trade; does not affect quotes
		case SM_M_BOOK_CROSS_TRADE:
		break;

		case SM_M_RESP_REFRESH_SYMBOL://Refreshed (or Invalid Stock Symbol)
		{
			const TSmMsgRespRefreshSymbol* msg = (const TSmMsgRespRefreshSymbol*)message;
			if(msg->m_flags & REFRESH_RefreshFailed)
			{
				SetSecurity(nullptr);
			}
			else if(msg->m_flags & REFRESH_EndOfRefresh)//Last chunk of refresh message
			{
				m_loaded = true;
				Initialize();
//				UpdateBidValues();
//				UpdateAskValues();
				m_owner->InvalidateItem(m_ordinal, -1, -1);
			}
		}
		break;

//		case SM_M_RESET_BOOK://The srver can send this message if there is a problem
//		break;

		case SM_M_RESET_SYMBOL:
		{
			UpdateBidValues();
			UpdateAskValues();
			const TSmMsgResetSymbol* msg = (const TSmMsgResetSymbol*)message;
			if(msg->m_bookId == AllBooks || isMyBookId(msg->m_bookId))
			{
				UpdateBookBidValues();
				UpdateBookAskValues();
			}
		}
		break;

		case SM_M_EOD_SUMMARY_REPORT:
		break;

		case SM_M_SHORT_SALE_INDICATORS:
		case SM_M_DIVIDENDS_AND_SPLITS:
		case SM_M_52WEEK_30DAY_HIGH_LOW:
		case SM_M_PMI:
#ifdef LRPS
		case SM_M_LRP:
#endif
		case SM_M_T_TIME:
//		case SM_M_RPI:
		case SM_M_LULD:
		case SM_M_SECURITY_TYPE:
		break;

		case SM_M_PREVIOUS_DAY_CLOSE://Yesterday's close price corrected
		break;

		case SM_M_AVERAGE_DAILY_VOLUME:
		case SM_M_PREVIOUS_DAY_HIGH_LOW:
		break;

		default:
#ifdef MY_STOCK_SYMBOL
if(myStock)
{
printf("X");
}
#endif
		break;
//
/*
		case TM_UPDATED_POSITION:
		{
			Money closedPnlDiff, openPnlDiff, openPrintPnlDiff;
			if(UpdatePositionValues(closedPnlDiff, openPnlDiff, openPrintPnlDiff))
			{
				PositionValuesChanged(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
			}
		}
		break;
*/
/*
		case TM_EQUITY_REFRESHED:
		if(m_loaded)
		{
//stock reloaded for whatever reason. Should not happen
		}
		else
		{
			m_loaded = true;
			m_owner->IncrementLoadedStocks();
		}
		Initialize();
		Repaint();//m_owner->GetLevel1BidLeft(), m_owner->GetVolumeRight());
		break;
*/
/*
		default://update message
		if(UpdateValues())
		{
			Repaint(m_owner->GetLevel1BidLeft(), m_owner->GetVolumeRight());
		}
		break;
*/
	}

}

void StockLevel2::Repaint(int left, int right)
{
	m_owner->InvalidateItem(m_ordinal, left, right);
}

void ListBoxStockLevel2::RepaintCount() const
{
	m_owner->RepaintCount();
}
/*
void ListBoxStockLevel2::RepaintClosedPnl()
{
	m_owner->RepaintClosedPnl();
}

void ListBoxStockLevel2::RepaintOpenPnl()
{
	m_owner->RepaintOpenPnl();
}

void ListBoxStockLevel2::RepaintOpenPrintPnl()
{
	m_owner->RepaintOpenPrintPnl();
}
*/
const char* ListBoxStockLevel2::GetItemName(const void* item) const
{
	return ((const StockLevel2*)item)->GetSymbol();
}

COLORREF ListBoxStockLevel2::GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	return ((const StockLevel2*)lpDrawItemStruct->itemData)->isLoaded() ? m_loadedColor : m_unloadedColor;
}

COLORREF ListBoxStockLevel2::GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const
{
	if(lpDrawItemStruct->itemState & ODS_SELECTED)return RGB(255, 255, 255);
	return GetSelectedBkColor(lpDrawItemStruct);
//	return m_selectable && (lpDrawItemStruct->itemState & ODS_SELECTED) != 0 ?
//		RGB(255, 255, 255)://GetSysColor(COLOR_HIGHLIGHTTEXT):
//		((const StockLevel2*)lpDrawItemStruct->itemData)->isLoaded() ? m_loadedColor : m_unloadedColor;
}

void ListBoxStockLevel2::DestroyItem(const void* item) const
{
	delete (StockLevel2*)item;
}

void ListBoxStockLevel2::DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->itemID)
	{
		case TMID_FIND_ALT:
		case TMID_ADD_ALL:
		{
			const char* symbol = TGUI_GetActiveSecuritySymbolPtr();
			if(symbol)
			{
				std::string menuText((const char*)lpDrawItemStruct->itemData);
				menuText += ' ';
				menuText += symbol;
				UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false, false, 0xFFFFFFFF, nullptr, menuText.c_str());// , 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr);// , height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
			}
			else
			{
				UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false);// , false, 0xFFFFFFFF, nullptr, "");// , 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr);// , height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
			}
		}
		break;

		case TMID_FIND:
		case TMID_ADD:
		case TMID_REMOVE:
		case TMID_CLEAR:

#if defined(T_RELEASED) || defined(_DEBUG)
		case TMID_UPDATE:
#endif
		case TMID_PASTE:
		case TMID_PASTE_STOCKS:
		case TMID_CLIPBOARD:
		case TMID_OPEN:
		case TMID_SAVE_AS:
		UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
		break;

		default:
		ColumnedListBox::DrawMenuItem(lpDrawItemStruct);
		break;
	}
}

bool ListBoxStockLevel2::MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	switch(lpMeasureItemStruct->itemID)
	{
		case TMID_FIND_ALT:
		case TMID_FIND:
		case TMID_ADD_ALL:
		case TMID_ADD:
		case TMID_REMOVE:
		case TMID_CLEAR:

#if defined(T_RELEASED) || defined(_DEBUG)
		case TMID_UPDATE:
#endif
		case TMID_PASTE:
		case TMID_PASTE_STOCKS:
		case TMID_CLIPBOARD:
		case TMID_OPEN:
		case TMID_SAVE_AS:
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
		lpMeasureItemStruct->itemWidth = m_menuWidth;// mainMessageLoopWnd->GetAddMenuWidth();// + lpMeasureItemStruct->itemHeight;
		break;

		default:
		return ColumnedListBox::MeasureMenuItem(lpMeasureItemStruct);
	}
	return true;
}

void ListBoxStockLevel2::AddMenuItems(CMenu& menu)
{
	const bool hasItems = m_itemCount > 0;
	const int sel = GetCurSel();
	const bool selected = sel >= 0;

	const bool clipboardAvailable = IsClipboardFormatAvailable(CF_TEXT) == TRUE;
//	menu.AppendMenu(MF_STRING|(hasItems || clipboardAvailable ? MF_ENABLED : MF_GRAYED), TMID_CLIPBOARD, "Copy");
	menu.AppendMenu(MF_OWNERDRAW |(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLIPBOARD, "Copy");
	AddMenuSeparator(menu);
//	menu.AppendMenu(MF_SEPARATOR);
	const char* symbol = TGUI_GetActiveSecuritySymbolPtr();
	const bool toAdd = symbol && *symbol;
//	if(toAdd)
//	{
	menu.AppendMenu(MF_OWNERDRAW|(toAdd ? MF_ENABLED : MF_GRAYED), TMID_FIND_ALT, "Find");
	menu.AppendMenu(MF_OWNERDRAW|(toAdd ? MF_ENABLED : MF_GRAYED), TMID_ADD_ALL, "Add");
//	}
	menu.AppendMenu(MF_OWNERDRAW, TMID_FIND, "Find...");
	menu.AppendMenu(MF_OWNERDRAW, TMID_ADD, "Add...");
	menu.AppendMenu(MF_OWNERDRAW|(clipboardAvailable ? MF_ENABLED : MF_GRAYED), TMID_PASTE, "Paste Add");
	menu.AppendMenu(MF_OWNERDRAW|(clipboardAvailable ? MF_ENABLED : MF_GRAYED), TMID_PASTE_STOCKS, "Paste Replace");
	AddMenuSeparator(menu);
//	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_OWNERDRAW|(selected ? MF_ENABLED : MF_GRAYED), TMID_REMOVE, "Remove");
	menu.AppendMenu(MF_OWNERDRAW|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLEAR, "Clear");
#if defined(T_RELEASED) || defined(_DEBUG)
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW|(selected ? MF_ENABLED : MF_GRAYED), TMID_UPDATE, "Update");
#endif
	AddMenuSeparator(menu);
	menu.AppendMenu(MF_OWNERDRAW, TMID_OPEN, "Open...");
	menu.AppendMenu(MF_OWNERDRAW, TMID_SAVE_AS, "Save as...");
}

void ListBoxStockLevel2::CopySymbols(std::string& info) const
{
	bool first = true;
	const StockLevel2* item;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		item = (const StockLevel2*)GetItemDataPtr(i);
		if(first)
		{
			first = false;
		}
		else
		{
//			info += '\r';
			info += '\n';
		}
		info += item->GetSymbol();
	}
}

void ListBoxStockLevel2::RefreshOrdinals() const
{
	StockLevel2* stock;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		stock = (StockLevel2*)GetItemDataPtr(i);
		stock->SetOrdinal(i);
	}
	::InvalidateRect(m_hWnd, &CRect(0, 0, m_listBoxWidth, m_listBoxHeight), FALSE);
}

void ListBoxStockLevel2::IncrementLoadedStocks()
{
	++m_loadedCount;
//	UpdateAverageRange();
}

void ListBoxStockLevel2::DecrementLoadedStocks()
{
	if(m_loadedCount)
	{
		--m_loadedCount;
//		UpdateAverageRange();
	}
}

void ListBoxStockLevel2::PasteStocks(StringMap& stringMap, bool replace)
{
	std::string symbol;
	StringMap::const_iterator it;
	StringMap::const_iterator itend = stringMap.cend();
	StockLevel2* stock;
	bool loaded;
	const char* cursor;
	std::string token;
	Price stockProperty;
	for(unsigned int i = 0, end = m_itemCount; i < end;)
	{
		stock = (StockLevel2*)GetItemDataPtr(i);
		symbol = stock->GetSymbol();
		it = stringMap.find(symbol);
		if(it == itend)
		{
			if(replace)
			{
//				UpdatePositionTotals(-stock->GetClosedPnl(), -stock->GetOpenPnl(), -stock->GetOpenPnlPrint());
				loaded = stock->isLoaded();
				DeleteString(i);
				RepaintCount();
				if(loaded && m_loadedCount)
				{
					--m_loadedCount;
				}

				--end;
			}
			else
			{
				++i;
			}
		}
		else
		{
			cursor = it->second.c_str();
			U_ReadStrToken(token, cursor, '\t');
			token.clear();
			U_ReadStrToken(token, cursor, '\t');
/*
			if(!token.empty())
			{
				cursor = token.c_str();
				U_StrToPrice(stockProperty, cursor, 9);
			}
			else
			{
				stockProperty.SetZero();
			}
			stock->SetStockProperty(stockProperty);
*/
			++i;
			stringMap.erase(it);
		}
	}
	for(it = stringMap.cbegin(); it != itend; ++it)
	{
		const std::string& symbol = it->first;
		if(!symbol.empty())
		{
			cursor = it->second.c_str();
			U_ReadStrToken(token, cursor, '\t');
			token.clear();
			U_ReadStrToken(token, cursor, '\t');
/*
			if(!token.empty())
			{
				cursor = token.c_str();
				U_StrToPrice(stockProperty, cursor, 9);
			}
			else
			{
				stockProperty.SetZero();
			}
*/
			stock = new StockLevel2(it->first.c_str(), this);
			if(stock->isLoaded())
			{
				++m_loadedCount;
			}
			AddString((const char*)stock);
			UpdateStockAccount(stock);
		}
	}
	RefreshOrdinals();
	RepaintCount();
}

DialogTextEntry& ListBoxStockLevel2::DisplayStockEntryField(unsigned int code)
{
	CFont* font = GetFont();//m_visual->GetFont("Arial", -14, true);
	DialogTextEntry& symbolEntryDialog = TGUI_GetTakionMainWnd()->GetSymbolEntryDialog();
	CPoint point;
//	int GetListBoxBorderSize() const{return m_listBoxBorderSize;}
	point.x = m_owner->GetLeftOffset();
	point.y = m_owner->GetTopOffset();
	m_owner->ClientToScreen(&point);
//	symbolEntryDialog.Display(true, true, nullptr, this, 0, this,
	symbolEntryDialog.Display(true, true, nullptr, this, code, this,
		U_GetValidStockChars(), false, true, U_GetValidStockFirstChars(), false, ".",
		RGB(255, 0, 255), 0, 7, font, point.x, point.y, 1, 1);
	return symbolEntryDialog;
}

BEGIN_MESSAGE_MAP(ListBoxStockLevel2, ColumnedListBox)
	ON_MESSAGE(WM_USER + 999, OnStockAdd)
END_MESSAGE_MAP()

LRESULT ListBoxStockLevel2::OnStockAdd(WPARAM code, LPARAM l)
{
	const std::string& textResult = TGUI_GetTakionMainWnd()->GetSymbolEntryDialog().GetTextResult();
	if(!textResult.empty())
	{
		int sel = FindItemByName(textResult.c_str());
		switch(code)
		{
			case 0://ADD
			if(sel < 0)
			{
				StockLevel2* stock = new StockLevel2(textResult.c_str(), this);
				if(stock->isLoaded())
				{
					++m_loadedCount;
				}
				sel = AddString((const char*)stock);
				unsigned int i = (unsigned int)sel;
				stock->SetOrdinal(i);
				++i;
				int top = GetItemTop(i);
				const unsigned int end = m_itemCount;
				for(; i < end; ++i)
				{
					((StockLevel2*)GetItemDataPtr(i))->IncrementOrdinal();
				}
				UpdateStockAccount(stock);
				RepaintCount();
				if(end)
				{
					if(top < 0)top = 0;
					int bottom = GetItemTop(end);
					if(bottom > m_listBoxHeight)bottom = m_listBoxHeight;
					::InvalidateRect(m_hWnd, &CRect(0, top, m_listBoxWidth, bottom), FALSE);
				}
			}
//			break;
//fall through
			case 1://FIND
			SetCurSel(sel);
			m_owner->OnSelchangeStock();
			break;
		}
//		::SetFocus(m_hWnd);
		m_owner->GotoDlgCtrl(this);
	}
	return 0;
}

BOOL ListBoxStockLevel2::OnCommand(WPARAM wParam, LPARAM lParam)
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

		case TMID_ADD_ALL:
		{
			const char* symbol = TGUI_GetActiveSecuritySymbolPtr();
			if(symbol && *symbol)
			{
				int sel = FindItemByName(symbol);
				if(sel < 0)
				{
					StockLevel2* stock = new StockLevel2(symbol, this);
					if(stock->isLoaded())
					{
						++m_loadedCount;
					}
					sel = AddString((const char*)stock);
					unsigned int i = (unsigned int)sel;
					stock->SetOrdinal(i);
					++i;
					int top = GetItemTop(i);
					const unsigned int end = m_itemCount;
					for(; i < end; ++i)
					{
						((StockLevel2*)GetItemDataPtr(i))->IncrementOrdinal();
					}
					UpdateStockAccount(stock);
					RepaintCount();
					if(end)
					{
						if(top < 0)top = 0;
						int bottom = GetItemTop(end);
						if(bottom > m_listBoxHeight)bottom = m_listBoxHeight;
						::InvalidateRect(m_hWnd, &CRect(0, top, m_listBoxWidth, bottom), FALSE);
					}
				}
				SetCurSel(sel);
				m_owner->OnSelchangeStock();
			}
		}
		break;

		case TMID_FIND_ALT:
		{
			const char* symbol = TGUI_GetActiveSecuritySymbolPtr();
			if(symbol && *symbol)
			{
				const int sel = FindItemByName(symbol);
				SetCurSel(sel);
				m_owner->OnSelchangeStock();
			}
		}
		break;

		case TMID_FIND:
		DisplayStockEntryField(1).ExpandForChar('A');
		break;

		case TMID_ADD:
		DisplayStockEntryField(0).ExpandForChar('A');
		break;

		case TMID_PASTE_STOCKS://replace
		{
			StringMap stringMap;
			unsigned int duplicateCount;
			unsigned int emptyCount = TU_CopyClipboardToStringMap(stringMap, m_hWnd, '\n', &duplicateCount);
			PasteStocks(stringMap, true);
		}
		break;

		case TMID_PASTE:
		{
			StringMap stringMap;
			unsigned int duplicateCount;
			unsigned int emptyCount = TU_CopyClipboardToStringMap(stringMap, m_hWnd, '\n', &duplicateCount);
			PasteStocks(stringMap, false);
		}
		break;

		case TMID_REMOVE:
		{
			int sel = GetCurSel();
			if(sel >= 0)
			{
				StockLevel2* stock = (StockLevel2*)GetItemDataPtr(sel);
//				UpdatePositionTotals(-stock->GetClosedPnl(), -stock->GetOpenPnl(), -stock->GetOpenPnlPrint());
				bool loaded = stock->isLoaded();
				DeleteString(sel);
				unsigned int uSel = (unsigned int)sel;
				unsigned int i = uSel, end = m_itemCount;
				int top = GetItemTop(i);
				for(; i < end; ++i)
				{
					stock = (StockLevel2*)GetItemDataPtr(i);
					stock->DecrementOrdinal();
				}
				RepaintCount();
				if(loaded && m_loadedCount)
				{
					--m_loadedCount;
				}

				if(end)
				{
					if(top < 0)top = 0;
					int bottom = GetItemTop(end);
					if(bottom > m_listBoxHeight)bottom = m_listBoxHeight;
					::InvalidateRect(m_hWnd, &CRect(0, top, m_listBoxWidth, bottom), FALSE);
				}
				if(m_itemCount)
				{
					SetCurSel(uSel < m_itemCount ? uSel : m_itemCount - 1);
					m_owner->OnSelchangeStock();
				}
			}
		}
		break;

		case TMID_CLEAR:
		ResetContent();
		ResetTotals();
		break;

#if defined(T_RELEASED) || defined(_DEBUG)
		case TMID_UPDATE:
		{
			const int sel = GetCurSel();
			if(sel >= 0)
			{
				StockLevel2* stock = (StockLevel2*)GetItemDataPtr(sel);
				stock->UpdateBidValues();
				stock->UpdateAskValues();
			}
		}
		break;
#endif
		case TMID_SAVE_AS:
		{
			char buf[MAX_PATH];
			strcpy_s(buf, sizeof(buf), theApp.GetFilePathAndName().c_str());
			char* p = strrchr(buf, '\\');
			if(p)
			{
				*++p = '*';
				*++p = '.';
				*++p = 't';
				*++p = 'x';
				*++p = 't';
				*++p = '\0';
			}
			TakionFileDialog fileDlg(m_owner->GetMainWnd(), false, "txt", buf,
				OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_EXTENSIONDIFFERENT,
				"Stock Files (*.txt)|*.txt|All Files (*.*)|*.*||", "Save Stock File", this);
			if(fileDlg.DoModal() == IDOK)
			{
				std::string pathName = fileDlg.GetPathName();
				std::string info;
				CopySymbols(info);
				U_WriteStringToFile(pathName.c_str(), info);
			}
		}
		break;

		case TMID_OPEN:
		{
			char buf[MAX_PATH];
			strcpy_s(buf, sizeof(buf), theApp.GetFilePathAndName().c_str());
			char* p = strrchr(buf, '\\');
			if(p)
			{
				*++p = '*';
				*++p = '.';
				*++p = 't';
				*++p = 'x';
				*++p = 't';
				*++p = '\0';
			}
			TakionFileDialog fileDlg(m_owner->GetMainWnd(), true, "txt", buf,
				OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
				"Stock Files (*.txt)|*.txt|All Files (*.*)|*.*||", "Load Stock File", this);
			if(fileDlg.DoModal() == IDOK)
			{
				std::string pathName = fileDlg.GetPathName();
				m_owner->DoReadStocks(true, pathName.c_str());
			}
		}
		break;

		default:
		return CListBox::OnCommand(wParam, lParam);
	}
	return TRUE;
}
/*
void ListBoxStockLevel2::MarketSorterCleanup()
{
	StockLevel2* stock;
	for(unsigned int i = 0, end = GetCount(); i < end; ++i)
	{
		stock = (StockLevel2*)GetItemDataPtr(i);
		stock->MarketSorterCleanup();
	}
}
*/

void ListBoxStockLevel2::SetMarketDataConnected(const bool connected)
{
	if(connected != m_marketDataConnected)
	{
		m_marketDataConnected = connected;
		SetInvalid(!m_marketDataConnected);
/*
		if(!m_marketDataConnected)
		{
			SetMarketSorterLoaded(false);
		}
*/
	}
}

void ListBoxStockLevel2::SetMarketSorterLoaded(const bool loaded)
{
	if(loaded != m_marketSorterLoaded)
	{
		m_marketSorterLoaded = loaded;
//		EnableControl(m_ButtonLoadAdd.m_hWnd, enable, m_EditPath.m_hWnd);
	}
}

void ListBoxStockLevel2::UpdateStockAccount(StockLevel2* stock)
{
	if(stock->SetAccount(m_currentAccount))
	{
/*
		Money closedPnlDiff, openPnlDiff, openPrintPnlDiff;
		if(stock->UpdatePositionValues(closedPnlDiff, openPnlDiff, openPrintPnlDiff))
		{
			stock->PositionValuesChanged(closedPnlDiff, openPnlDiff, openPrintPnlDiff);
		}
*/
	}
}

void ListBoxStockLevel2::CurrentAccountSet(Account* account)
{
	if(m_currentAccount != account)
	{
		m_currentAccount = account;
		if(m_hWnd)
		{
			StockLevel2* stock;
			for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
			{
				stock = (StockLevel2*)GetItemDataPtr(i);
				UpdateStockAccount(stock);
			}
		}
	}
}
/*
void ListBoxStockLevel2::SizeChanged(bool horizontalChange, bool verticalChange)
{
	if(verticalChange)
	{
		m_oneSecondRect.bottom = m_listBoxHeight;
	}
}
*/

LbHorizontalScroller* ListBoxStockLevel2::CreateHorizontalScroller()
{
	return new LbStockLevel2HorizontalScroller(this);
}
/*
void ListBoxStockMarketSorter::DoCreateColumnToolTips()
{
}
*/
void ListBoxStockLevel2::ListBoxCreated()
{
	ColumnedListBox::ListBoxCreated();
	m_menuWidth = MeasureText("Paste Replace", GetMenuFont());
/*
	HDC hdc = ::GetDC(m_hWnd);
	if(hdc)
	{
		HGDIOBJ oldFont = nullptr;
		CFont* menuFont = GetMenuFont();
		if(menuFont)
		{
			oldFont = ::SelectObject(hdc, menuFont->m_hObject);
		}
		CRect rect(0, 0, 0, 0);
		TakionBaseWnd::CalculateTextWidth(rect, hdc, "Paste Replace", m_menuWidth);

		if(oldFont)
		{
			::SelectObject(hdc, oldFont);
		}
		::ReleaseDC(m_hWnd, hdc);
	}
*/
/*
	m_horizontalScroller->GetColumnBounds(MSCI_ONE_SECOND, m_oneSecondPrintLeft, m_oneSecondPrintRight);
	m_horizontalScroller->GetColumnBounds(MSCI_RANGE, m_rangeLeft, m_rangeRight);
	m_horizontalScroller->GetColumnBounds(MSCI_POS_SIZE, m_positionSizeLeft, m_positionSizeRight);
	m_horizontalScroller->GetColumnBounds(MSCI_OPEN_PNL_PRINT, m_openPnlPrintLeft, m_openPnlPrintRight);
	m_horizontalScroller->GetColumnBounds(MSCI_LEVEL1_BID, m_level1BidLeft, m_level1BidRight);
	m_horizontalScroller->GetColumnBounds(MSCI_VOLUME, m_volumeLeft, m_volumeRight);
	
	m_oneSecondRect.left = m_oneSecondPrintLeft;
	m_oneSecondRect.right = m_oneSecondPrintRight;
	m_oneSecondRect.bottom = m_listBoxHeight;
*/
}

void ListBoxStockLevel2::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
	const StockLevel2* stock = (const StockLevel2*)item;
	static std::string info;
	static char num[33];
	switch(columnId)
	{
		case L2T_SYMBOL:
		DrawText(hDC, stock->GetSymbol(), -1, &r, alignment);
		break;

#ifdef L2T_SHOW_ORDINALS
		case L2T_LINE_ORDINAL:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(itemOrdinal, num, sizeof(num)), -1, &r, alignment);
		break;

		case L2T_STOCK_ORDINAL:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetOrdinal(), num, sizeof(num)), -1, &r, alignment);
		break;
#endif

		case L2T_LEVEL2_SPREAD:
		if(stock->GetLevel2BidCompact() && stock->GetLevel2AskCompact())
		{
			const SignedPrice& spread = stock->GetLevel2Spread();
			info.clear();
			U_SignedPriceToStr(info, spread, 4, '\0', true, '\0', true);
			if(!spread.isPositive())
			{
				UGUI_FillSolidRect(hDC, r, spread.isZero() ? RGB(192, 192, 0) : RGB(255, 0, 0));
			}
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case L2T_LEVEL2_BID:
		if(stock->GetLevel2BidCompact())
		{
			info.clear();
			U_PriceToStr(info, Price(stock->GetLevel2BidCompact()), 4, -1, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case L2T_LEVEL2_ASK:
		if(stock->GetLevel2AskCompact())
		{
			info.clear();
			U_PriceToStr(info, Price(stock->GetLevel2AskCompact()), 4, -1, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case L2T_LEVEL2_BID_SIZE:
		if(stock->GetLevel2BidSize())
		{
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetLevel2BidSize(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case L2T_LEVEL2_ASK_SIZE:
		if(stock->GetLevel2AskSize())
		{
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetLevel2AskSize(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case L2T_CLOSE_PRICE:
		if(stock->GetClosePriceCompact())
		{
			info.clear();
			U_PriceToStr(info, Price(stock->GetClosePriceCompact()), 4, -1, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case L2T_TOTAL_PE_BID_SIZE_ABOVE_CLOSE_PRICE:
		if(stock->GetPeBidTotalSize())
		{
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetPeBidTotalSize(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case L2T_MAX_PE_BID_SIZE_ABOVE_CLOSE_PRICE:
		if(stock->GetPeBidMaxSize())
		{
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetPeBidMaxSize(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case L2T_PE_BID_ABOVE_CLOSE_PRICE_WITH_MAX_SIZE:
		if(stock->GetPeBidPriceWithMaxSizeCompact())
		{
			info.clear();
			U_PriceToStr(info, Price(stock->GetPeBidPriceWithMaxSizeCompact()), 4, -1, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;

		case L2T_TOTAL_PE_ASK_SIZE_BELOW_CLOSE_PRICE:
		if(stock->GetPeAskTotalSize())
		{
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetPeAskTotalSize(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case L2T_MAX_PE_ASK_SIZE_BELOW_CLOSE_PRICE:
		if(stock->GetPeAskMaxSize())
		{
			DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetPeAskMaxSize(), num, sizeof(num)), -1, &r, alignment);
		}
		break;

		case L2T_PE_ASK_BELOW_CLOSE_PRICE_WITH_MAX_SIZE:
		if(stock->GetPeAskPriceWithMaxSizeCompact())
		{
			info.clear();
			U_PriceToStr(info, Price(stock->GetPeAskPriceWithMaxSizeCompact()), 4, -1, true, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
		break;
/*
		case MSCI_POS_SIZE:
		DrawText(hDC, U_SignedNumberToStrWithCommas(stock->GetPositionSize(), num, sizeof(num)), -1, &r, alignment);
		break;

		case MSCI_BUY_PENDING:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetBuyPendingShares(), num, sizeof(num)), -1, &r, alignment);
		break;

		case MSCI_SELL_PENDING:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(stock->GetSellPendingShares(), num, sizeof(num)), -1, &r, alignment);
		break;

		case MSCI_POS_TIMESTAMP:
		info.clear();
		U_AppendMillisecond(info, stock->GetPositionEntryTimestamp(), false);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;

		case MSCI_POS_PRICE:
		info.clear();
		U_PriceToStr(info, stock->GetPositionPrice(), 4, 0, true, '\0', true);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;

		case MSCI_LAST_EXEC_PRICE:
		info.clear();
		U_PriceToStr(info, stock->GetLastExecutionPrice(), 4, -1, true, '\0', true);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;

		case MSCI_CLOSED_PNL:
		info.clear();
		U_MoneyToStr(info, stock->GetClosedPnl(), 2);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;

		case MSCI_OPEN_PNL:
		info.clear();
		U_MoneyToStr(info, stock->GetOpenPnl(), 2);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;

		case MSCI_OPEN_PNL_PRINT:
		info.clear();
		U_MoneyToStr(info, stock->GetOpenPnlPrint(), 2);
		DrawText(hDC, info.c_str(), -1, &r, alignment);
		break;
*/
	}
}

enum SampleSettingStockLevel2Ids
{
	listBoxStockId = 1000,

	calculateL2CheckBoxId,
	loadOnStartCheckBoxId,
	buttonLoadAllStockId,
	buttonLoadAddStockId,
	buttonLoadReplaceStockId,
	buttonViewStocksId,
	buttonBrowseId,
	editPathId,

	stockLevel2IdsCount,
};


SampleDialogSettingLevel2InThread::SampleDialogSettingLevel2InThread(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, Account* account) :
	BaseDialogSetting(mainWnd, parentTab, "L2InThread"),
	m_calculateL2(false),
	m_currentAccount(account),
	m_CheckBoxLoadOnStart("Load Stocks when this tab is created", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_CheckBoxCalculateL2("Calculate Level2 Values (Price and Size)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_visualBase->GetLightGrayColor(), checkBoxDeflation, checkBoxTextOffset),
	m_ListBoxStock(this, m_currentAccount, m_calculateL2),
	m_marketDataConnected(false),
	m_marketSorterLoaded(false),
	m_editLeftOffset(0),
	m_editTopOffset(0),
	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0)
{
	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
	SetMarketDataConnected(TD_IsMarketDataConnected());
	m_ListBoxStock.SetInvalid(!m_marketDataConnected);

	AddListBox(&m_ListBoxStock);

	int w = 58;
	int x = hgap;
	int y = vgap;
	AddButtonIndirect("Load on Start", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, x, y + (editHeight - checkBoxHeight) / 2, w, checkBoxHeight, loadOnStartCheckBoxId);
	AddButtonIndirect("Load Add", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x += w + hgap, y, w, editHeight, buttonLoadAddStockId);
	AddButtonIndirect("Load Replace", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x += w + hgap, y, w, editHeight, buttonLoadReplaceStockId);
	AddButtonIndirect("View", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, x += w + hgap, y, w / 2, editHeight, buttonViewStocksId);
	AddButtonIndirect("Browse", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, x += w / 2 + hgap, y, w, editHeight, buttonBrowseId);
	x += w + hgap;
	AddEditBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL, 0, x, y, m_width - x, editHeight, editPathId);
	
	y = editHeight + vgap + vgap + staticHeight;
	x = hgap;
	x += w + hgap;
	w = m_width - x;
	AddListBoxIndirect(nullptr, WS_VISIBLE|WS_TABSTOP|WS_BORDER|LBS_NOTIFY|LBS_SORT|LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT|WS_VSCROLL, 0, x, y, w, m_height - staticHeight - y, listBoxStockId);

	w = x - hgap - hgap;
	AddButtonIndirect("Load All", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, hgap, y, w, editHeight, buttonLoadAllStockId);
	AddButtonIndirect("Calculate L2", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, hgap, y += editHeight + 8 * vgap, w, checkBoxHeight, calculateL2CheckBoxId);
}

bool SampleDialogSettingLevel2InThread::MainThreadIdleState(unsigned int idleCount, bool takionIdleDone)
{
	return false;
}

void SampleDialogSettingLevel2InThread::CurrentAccountSet(Account* account)
{
	if(m_currentAccount != account)
	{
		m_currentAccount = account;
		m_ListBoxStock.CurrentAccountSet(m_currentAccount);
	}
}

HWND SampleDialogSettingLevel2InThread::GetFirstTabControl() const
{
	return m_ListBoxStock.m_hWnd;
}

void SampleDialogSettingLevel2InThread::RemoveStock(Security* stock)
{
/*
	PendingStockMap::iterator found = m_pendingStockMap.find(stock);
	if(found != m_pendingStockMap.end())
	{
		delete found->second;
		m_pendingStockMap.erase(found);
	}
*/
}

void SampleDialogSettingLevel2InThread::ClearStocks()
{
/*
	for(PendingStockMap::iterator it = m_pendingStockMap.begin(), itend = m_pendingStockMap.end(); it != itend; ++it)
	{
		delete it->second;
	}
	m_pendingStockMap.clear();
*/
}

void SampleDialogSettingLevel2InThread::BeforeDestroy()
{
	m_editLeftOffset = 0;
	m_editTopOffset = 0;
	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;
	m_calculateL2 = false;
	m_ListBoxStock.SetCalculateL2(m_calculateL2);
	ClearStocks();
//	TD_GetNewEquityObservable().RemoveObserver(this);
	TD_GetNewPositionObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
	BaseDialogSetting::BeforeDestroy();
}

BOOL SampleDialogSettingLevel2InThread::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)// || pMsg->message == WM_SYSKEYDOWN)
	{
		unsigned char ascii = (unsigned char)pMsg->wParam;
		HWND focus = ::GetFocus();
		if(focus == m_ListBoxStock.m_hWnd)
		{
			if(ascii >= 'A' && ascii <= 'Z')
			{
				DialogTextEntry& dlg = m_ListBoxStock.DisplayStockEntryField(0);//.ExpandForChar('A');
				dlg.AddChar(ascii);
				return TRUE;
			}
		}
	}
	return BaseDialogSetting::PreTranslateMessage(pMsg);
}

void SampleDialogSettingLevel2InThread::ToolTipsEnabled(bool enable)
{
	BaseDialogSetting::ToolTipsEnabled(enable);
	m_ListBoxStock.EnableTooltips(enable, m_toolTip);
	m_CheckBoxLoadOnStart.EnableTooltips(enable, m_toolTip);
	m_CheckBoxCalculateL2.EnableTooltips(enable, m_toolTip);
}

BOOL SampleDialogSettingLevel2InThread::OnInitDialog()
{
//	TD_GetNewEquityObservable().AddObserver(this);//to get notified about new Securities created
	TD_GetNewPositionObservable().AddObserver(this);//to get notified about new Positions created
	TD_GetAdminObservable().AddObserver(this);//to get notified about connect / disconnect

	m_calculateL2 = false;
	m_ListBoxStock.SetCalculateL2(m_calculateL2);

	SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
	SetMarketDataConnected(TD_IsMarketDataConnected());
	m_ListBoxStock.SetInvalid(!m_marketDataConnected);

	m_CheckBoxLoadOnStart.SetInvalid(true);

	BOOL ret = BaseDialogSetting::OnInitDialog();

	m_CheckBoxLoadOnStart.ResizeToFitText(0);
	m_CheckBoxCalculateL2.ResizeToFitText(0);

	EnableControl(m_ButtonLoadAll.m_hWnd, m_marketSorterLoaded, m_ListBoxStock.m_hWnd);

	SetToolTipsEnabled(true);
//	m_ListBoxStock.EnableTooltips(true, m_toolTip);

	AddTitle(&m_ListBoxStock);

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingLevel2InThread::RestoreLayout()
{
	CString oldPath;
	m_EditPath.GetWindowText(oldPath);
//	bool oldLoadOnStart = m_CheckBoxLoadOnStart.GetCheck() == BST_CHECKED;

	m_CheckBoxCalculateL2.SetCheck(extensionConfig->m_mdCalculateL2.GetValue() ? BST_CHECKED : BST_UNCHECKED);

	const bool loadOnStart = extensionConfig->m_mdLoadOnStart.GetValue();
	m_CheckBoxLoadOnStart.SetCheck(loadOnStart ? BST_CHECKED : BST_UNCHECKED);
	const std::string& path = extensionConfig->m_mdStockFilePath.GetValue();
	if(strcmp(oldPath, path.c_str()))
//	if(!path.empty())
	{
		m_EditPath.SetWindowText(path.c_str());
		OnPathChange();
		if(loadOnStart && !path.empty())
		{
			DoReadStocks(true, path.c_str());
		}
	}

	m_modified = false;
}

void SampleDialogSettingLevel2InThread::UpdateSettings()
{
	extensionConfig->m_mdCalculateL2.SetValue(m_CheckBoxCalculateL2.GetCheck() == BST_CHECKED);
	extensionConfig->m_mdLoadOnStart.SetValue(m_CheckBoxLoadOnStart.GetCheck() == BST_CHECKED);
	CString path;
	m_EditPath.GetWindowText(path);
	extensionConfig->m_mdStockFilePath.SetValue(std::string(path));
}

void SampleDialogSettingLevel2InThread::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
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

const char* SampleDialogSettingLevel2InThread::GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const
{
	std::string info;
	switch(columnId)
	{
		case L2T_SYMBOL:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxStock.GetItemCount(), buf, buflen);
/*
		case MSCI_CLOSED_PNL:
		U_MoneyToStr(info, m_ListBoxStock.GetClosedPnl(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		case MSCI_OPEN_PNL:
		U_MoneyToStr(info, m_ListBoxStock.GetOpenPnl(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		case MSCI_OPEN_PNL_PRINT:
		U_MoneyToStr(info, m_ListBoxStock.GetOpenPrintPnl(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;

		case MSCI_ONE_SECOND:
		U_PriceToStr(info, m_ListBoxStock.GetAverageRange(), 2);
		strcpy_s(buf, buflen, info.c_str());
		return buf;
*/
		default:
		return nullptr;
	}
}

void SampleDialogSettingLevel2InThread::Resize(bool widthChanged, bool heightChanged)
{
	BaseDialogSetting::Resize(widthChanged, heightChanged);
	if(m_ListBoxStock.m_hWnd)
	{
		if(!m_bottomOffset)
		{
			CRect rect;
			m_ListBoxStock.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_ListBoxStock.GetClientRect(&clientRect);
			m_listBoxBorderSize = (rect.Width() - clientRect.Width()) / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;

			m_EditPath.GetWindowRect(&clientRect);
			ScreenToClient(&clientRect);
			m_editLeftOffset = clientRect.left;
			m_editTopOffset = clientRect.top;

			m_titleOffset = m_leftOffset + m_listBoxBorderSize;
			
		}
		int bottom = m_clientRect.bottom - m_bottomOffset;
		m_ListBoxStock.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);

		m_EditPath.SetWindowPos(nullptr, m_editLeftOffset, m_editTopOffset, m_clientRect.right - m_editLeftOffset, m_topOffset - m_editTopOffset - m_editTopOffset - m_bottomOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);

		if(heightChanged)InvalidateAllTotalValueRects(listBoxStockId);
//		if(heightChanged)UpdateTotalRects();//m_leftOffset + m_listBoxBorderSize, bottom);
	}
}
/*
void ListBoxStockLevel2::HorizontalScrollingDone(int diff)
{
	m_level1BidLeft += diff;
	m_level1BidRight += diff;

	m_volumeLeft += diff;
	m_volumeRight += diff;

	m_openPnlPrintLeft += diff;
	m_openPnlPrintRight += diff;

	m_positionSizeLeft += diff;
	m_positionSizeRight += diff;
}
*/
/*
void SampleDialogSettingLevel2InThread::TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff)
{
	m_totalClosedPnlRect.left += diff;
	m_totalOpenPnlRect.left += diff;
	m_totalOpenPrintPnlRect.left += diff;
	m_totalRangeAtFirstTimerRect.left += diff;

	m_totalClosedPnlRect.right += diff;
	m_totalOpenPnlRect.right += diff;
	m_totalOpenPrintPnlRect.right += diff;
	m_totalRangeAtFirstTimerRect.right += diff;
}
*/
void SampleDialogSettingLevel2InThread::RepaintTotals() const
{
	InvalidateAllTotalValueRects(listBoxStockId);
}

void SampleDialogSettingLevel2InThread::RepaintCount() const
{
	InvalidateTotalValueRect(listBoxStockId, L2T_SYMBOL);
}
/*
void SampleDialogSettingLevel2InThread::RepaintClosedPnl() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_CLOSED_PNL);
}

void SampleDialogSettingLevel2InThread::RepaintOpenPnl() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_OPEN_PNL);
}

void SampleDialogSettingLevel2InThread::RepaintOpenPrintPnl() const
{
	InvalidateTotalValueRect(listBoxStockId, MSCI_OPEN_PNL_PRINT);
}
*/
void SampleDialogSettingLevel2InThread::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
//	DDX_Control(pDX, editSymbolId, m_EditSymbol);

	DDX_Control(pDX, loadOnStartCheckBoxId, m_CheckBoxLoadOnStart);
	DDX_Control(pDX, buttonLoadAllStockId, m_ButtonLoadAll);
	DDX_Control(pDX, buttonLoadAddStockId, m_ButtonLoadAdd);
	DDX_Control(pDX, buttonLoadReplaceStockId, m_ButtonLoadReplace);
	DDX_Control(pDX, buttonViewStocksId, m_ButtonViewStocks);
	DDX_Control(pDX, buttonBrowseId, m_ButtonBrowse);
	DDX_Control(pDX, editPathId, m_EditPath);

	DDX_Control(pDX, calculateL2CheckBoxId, m_CheckBoxCalculateL2);

	DDX_Control(pDX, listBoxStockId, m_ListBoxStock);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingLevel2InThread, BaseDialogSetting)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(loadOnStartCheckBoxId, OnModified)
	ON_BN_CLICKED(buttonLoadAllStockId, OnLoadAll)
	ON_BN_CLICKED(buttonLoadAddStockId, OnLoadAdd)
	ON_BN_CLICKED(buttonLoadReplaceStockId, OnLoadReplace)
	ON_BN_CLICKED(buttonViewStocksId, OnViewStocks)
	ON_BN_CLICKED(buttonBrowseId, OnBrowse)
	ON_EN_CHANGE(editPathId, OnPathChange)

	ON_BN_CLICKED(calculateL2CheckBoxId, OnCalculateL2)

	ON_LBN_SELCHANGE(listBoxStockId, OnSelchangeStock)
	ON_LBN_DBLCLK(listBoxStockId, OnDblclkStock)

//	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

#if defined(T_RELEASED) || defined(_DEBUG)
//#define UPDATE_L2_ON_DBLCLK
#endif

void SampleDialogSettingLevel2InThread::OnDblclkStock()
{
	int sel = m_ListBoxStock.GetCurSel();
	if(sel >= 0)
	{
		StockLevel2* stock = (StockLevel2*)m_ListBoxStock.GetItemDataPtr(sel);
#ifdef UPDATE_L2_ON_DBLCLK
		stock->UpdateBidValues();
		stock->UpdateAskValues();
#else
		TD_SetSymbolInCommandTool(stock->GetSymbol(), true);//, unsigned int commandToolOrdinal = 0xFFFFFFFF);
#endif
	}
}

/*
LRESULT SampleDialogSettingLevel2InThread::OnSpinChange(WPARAM spinId, LPARAM l)
{
	switch(spinId)
	{
		case spinSecondBackId:
		SetSecondBack(m_SpinSecondBack.GetSecond());
		break;
	}
	return 0;
}
*/
void SampleDialogSettingLevel2InThread::OnPathChange()
{
	m_modified = true;
	const bool enable = m_EditPath.GetWindowTextLength() != 0;
	EnableControl(m_ButtonLoadAdd.m_hWnd, enable, m_EditPath.m_hWnd);
	EnableControl(m_ButtonLoadReplace.m_hWnd, enable, m_EditPath.m_hWnd);
	EnableControl(m_ButtonViewStocks.m_hWnd, enable, m_EditPath.m_hWnd);
//	m_CheckBoxLoadOnStart.Invalidate(FALSE);
	m_CheckBoxLoadOnStart.SetInvalid(!enable);
}

void SampleDialogSettingLevel2InThread::DoReadStocks(bool replace, const char* currentPath)
{
	ListBoxStockLevel2::StringMap stringMap;
	unsigned int duplicateCount;
	unsigned int emptyCount;
	if(U_ReadFileToStringMap(currentPath, stringMap, '\t', ';', true, &duplicateCount, &emptyCount))
	{
		m_ListBoxStock.PasteStocks(stringMap, replace);
	}
	else
	{
		TakionMessageBox mb(m_mainWnd);
		mb.AddMessage(currentPath);
		mb.DisplayModal(this, "File does not exist", MB_OK|MB_ICONERROR);//, RGB(0, 0, 128), RGB(255, 255, 255));
	}
}

void SampleDialogSettingLevel2InThread::ReadStocks(bool replace)
{
	CString currentPath;
	m_EditPath.GetWindowText(currentPath);
	if(!currentPath.IsEmpty())
	{
		DoReadStocks(replace, currentPath);
	}
}

void SampleDialogSettingLevel2InThread::OnViewStocks()
{
	CString currentPath;
	m_EditPath.GetWindowText(currentPath);
	if(!currentPath.IsEmpty())
	{
		const char* error = U_ShellExecute(m_hWnd, currentPath);
		if(error)
		{
			TakionMessageBox mb(m_mainWnd);
			mb.AddMessage(currentPath);
			mb.AddMessage(error);
			mb.DisplayModal(this, "Cannot open file", MB_OK | MB_ICONERROR);
		}
	}
}

void SampleDialogSettingLevel2InThread::OnLoadAll()
{
	LoadAllStocks(false);
}

void SampleDialogSettingLevel2InThread::LoadAllStocks(bool replace)
{
	if(m_marketSorterLoaded)
	{
		EnableControl(m_ButtonLoadAll.m_hWnd, false, m_ListBoxStock.m_hWnd);
		::UpdateWindow(m_ButtonLoadAll.m_hWnd);

		std::string sym;
		std::string empty;
		ListBoxStockLevel2::StringMap stringMap;
		TakionIterator* iterator = TD_CreateEquityIterator();
		const Security* security;
		TD_LockEquityStorage();
		iterator->Reset();
		while(security = TD_GetNextEquity(iterator))
		{
			sym = security->GetSymbol();
			stringMap.insert(ListBoxStockLevel2::StringMap::value_type(sym, empty));
		}
		TD_UnlockEquityStorage();
		delete iterator;

		m_ListBoxStock.PasteStocks(stringMap, replace);

		EnableControl(m_ButtonLoadAll.m_hWnd, true, m_ListBoxStock.m_hWnd);
		::UpdateWindow(m_ButtonLoadAll.m_hWnd);
	}
}

void SampleDialogSettingLevel2InThread::OnLoadAdd()
{
	ReadStocks(false);
}

void SampleDialogSettingLevel2InThread::OnLoadReplace()
{
	ReadStocks(true);
}

void SampleDialogSettingLevel2InThread::OnBrowse()
{
	char buf[MAX_PATH];
	CString currentPath;
	m_EditPath.GetWindowText(currentPath);

//	const std::string& theApp.GetFilePathAndName()
	if(currentPath.IsEmpty())
	{
		strcpy_s(buf, sizeof(buf), theApp.GetFilePathAndName().c_str());
		char* p = strrchr(buf, '\\');
		if(p)
		{
			*++p = '*';
			*++p = '.';
			*++p = 't';
			*++p = 'x';
			*++p = 't';
			*++p = '\0';
		}
	}
	else
	{
		strcpy_s(buf, sizeof(buf), currentPath);
	}

	TakionFileDialog fileDlg(m_mainWnd, true, "txt", buf,
		OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
		"Stock Files (*.txt)|*.txt|All Files (*.*)|*.*||", "Load Stock File", this);
	if(fileDlg.DoModal() == IDOK)
	{
		std::string pathName = fileDlg.GetPathName();
		if(strcmp(pathName.c_str(), currentPath))
		{
			m_EditPath.SetWindowText(pathName.c_str());
			OnPathChange();
		}
	}
}

void SampleDialogSettingLevel2InThread::OnCalculateL2()
{
	const bool checked = m_CheckBoxCalculateL2.GetCheck() == BST_CHECKED;
	if(checked != m_calculateL2)
	{
		m_calculateL2 = checked;
		m_ListBoxStock.SetCalculateL2(m_calculateL2);
		OnModified();
	}
}

void SampleDialogSettingLevel2InThread::OnSelchangeStock()
{
}

HBRUSH SampleDialogSettingLevel2InThread::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case loadOnStartCheckBoxId:
		if(m_EditPath.GetWindowTextLength() == 0)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
			return *m_bkBrush;
		}
		break;
/*
		case editSymbolId:
		if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_takionVisual->GetWarningBrush();
		}
		break;
*/
	}
	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}

void SampleDialogSettingLevel2InThread::SetMarketDataConnected(const bool connected)
{
	if(connected != m_marketDataConnected)
	{
		m_marketDataConnected = connected;
		m_ListBoxStock.SetMarketDataConnected(connected);

		if(!m_marketDataConnected)
		{
			RepaintTotals();
		}
	}
}

void SampleDialogSettingLevel2InThread::SetMarketSorterLoaded(const bool loaded)
{
	if(loaded != m_marketSorterLoaded)
	{
		m_marketSorterLoaded = loaded;
		EnableControl(m_ButtonLoadAll.m_hWnd, loaded, m_ListBoxStock.m_hWnd);
	}
}

void SampleDialogSettingLevel2InThread::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
/*
		case TM_NEW_EQUITY:
		case TM_REFRESHED_EQUITY:
		{
			const Security* security = ((const TMsgEquity*)message)->m_security;
			if(security->isLoaded())
			{
				int found = m_ListBoxStock.FindItemByName(security->GetSymbol());
				if(found >= 0)
				{
					StockMarketSorter* stock = (StockMarketSorter*)m_ListBoxStock.GetItemDataPtr(found);
					bool loaded = stock->isLoaded();
					stock->SetSecurity(security);
					stock->Repaint();//m_ListBoxStock.GetLevel1BidLeft(), m_ListBoxStock.GetVolumeRight());
					if(loaded != stock->isLoaded())
					{
						if(loaded)
						{
							m_ListBoxStock.DecrementLoadedStocks();
						}
						else
						{
							m_ListBoxStock.IncrementLoadedStocks();
						}
					}
				}
			}
		}
		break;

		case TM_NEW_POSITION:
		{
			const Position* position = ((TMsgNewPosition*)message)->m_position;
			if(m_currentAccount == position->GetAccount())
			{
				int sel = m_ListBoxStock.FindItemByName(position->GetSymbol());
				if(sel >= 0)
				{
					StockMarketSorter* stock = (StockMarketSorter*)m_ListBoxStock.GetItemDataPtr(sel);
					stock->SetPosition(position);
					stock->Repaint(m_ListBoxStock.GetPositionSizeLeft(), m_ListBoxStock.GetOpenPnlPrintRight());
				}
			}
		}
		break;

		case TM_MARKET_SORTER_LOADED:
		SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
		break;

		case TM_MARKET_SORTER_CLEANUP:
		if(from == &TD_GetAdminObservable())
		{
			m_ListBoxStock.MarketSorterCleanup();
		}
		break;
*/
		case TM_DISCONNECTED:
		SetMarketSorterLoaded(TD_IsMarketSorterLoaded());
		case TM_CONNECTED:
		SetMarketDataConnected(TD_IsMarketDataConnected());
		break;

		case TM_MARKET_SORTER_LOADED:
		SetMarketSorterLoaded(true);
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
/*
		case TM_ORDER_ERROR:
		if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
		{
			const TMsgOrderError* msg = (const TMsgOrderError*)message;
			m_ListBoxStock.LogOrderError(msg->m_errorCode, "OrderError");
		}
		break;

		case TM_CANCEL_ERROR:
		if(info && info->m_type == TM_UPDATED_ACCOUNT_AND_POSITION)
		{
//			Account* account = ((TMsgUpdatedAccountAndPosition*)info)->m_account;
			const TMsgCancelError* msg = (const TMsgCancelError*)message;
			m_ListBoxStock.LogCancelError(msg->m_errorCode, "CancelError");
		}
*/
	}
/*
		case TM_DATA_THREAD_BUFFER_OVERFLOW:
//		PostMessage(WM_USER + 500, ((const TMsgDataThreadBufferOverflow*)message)->m_threadId, 0);
		break;
	}
*/
}
