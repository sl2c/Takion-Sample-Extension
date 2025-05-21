#pragma once

#include "BaseSampleDialogSettingOption.h"

class SampleDialogSettingMarketData;

class UnappliedMessageReader : public LineBufferArrayReader
{
public:
	UnappliedMessageReader(SampleDialogSettingMarketData* window, const LineBufferArray* bufferArray = nullptr):LineBufferArrayReader(bufferArray), m_window(window){}
	virtual bool ProcessDataToken(const char* cursor, unsigned short length) const override;
protected:
	SampleDialogSettingMarketData* m_window;
};

class RectCollection
{
public:
	void SetWindow(CWnd* wnd){m_wnd = wnd;}
	void SetEmpty();
	void SetSizeRect(const CRect& rect){m_sizeRect = rect;}
	void SetVenueRect(const CRect& rect){m_venueRect = rect;}
	void SetPriceRect(const CRect& rect){m_priceRect = rect;}
	void SetConditionRect(const CRect& rect){m_conditionRect = rect;}
	void SetTimeRect(const CRect& rect){m_timeRect = rect;}
	void SetDelayRect(const CRect& rect){m_delayRect = rect;}
	void CopyData(const RectCollection& other)
	{
		m_sizeStr = other.m_sizeStr;
		m_priceStr = other.m_priceStr;
		m_timeStr = other.m_timeStr;
		m_delayStr = other.m_delayStr;

		*(unsigned int*)m_venueStr = *(unsigned int*)other.m_venueStr;
	}
protected:
	RectCollection(CWnd* wnd = nullptr):
		m_wnd(wnd),
		m_sizeRect(0, 0, 0, 0),
		m_venueRect(0, 0, 0, 0),
		m_priceRect(0, 0, 0, 0),
		m_conditionRect(0, 0, 0, 0),
		m_timeRect(0, 0, 0, 0),
		m_delayRect(0, 0, 0, 0)
	{
		*(unsigned int*)m_venueStr = 0;
		m_venueStr[sizeof(unsigned int)] = '\0';
	}
	CWnd* m_wnd;
	CRect m_sizeRect;
	CRect m_venueRect;
	CRect m_priceRect;
	CRect m_conditionRect;
	CRect m_timeRect;
	CRect m_delayRect;

	std::string m_sizeStr;
	std::string m_priceStr;
	std::string m_timeStr;
	std::string m_delayStr;

	char m_venueStr[sizeof(unsigned int) + 1];
};

class MyQuote : public Quote, public RectCollection
{
public:
	MyQuote(bool bid = true, CWnd* wnd = nullptr):
		Quote(bid),
		RectCollection(wnd),
		m_conditionChar('\0')
	{
	}
	void SetEmpty();
	void SetQuote(const Quote& quote, unsigned int roundLot);
	void Draw(HDC dc, const RECT& paintRect) const;
	void CopyData(const MyQuote& other)
	{
		RectCollection::CopyData(other);
		Quote::operator=(other);
		m_conditionChar = other.m_conditionChar;
	}
	char m_conditionChar;
};

class MyPrint : public NamedPrint, public RectCollection
{
public:
	MyPrint(CWnd* wnd = nullptr):
		RectCollection(wnd),
		m_bgColor(0xFFFFFFFF),
		m_fgColor(0),
		m_conditionRect2(0, 0, 0, 0),
		m_conditionRect3(0, 0, 0, 0),
		m_conditionRect4(0, 0, 0, 0)
	{
	}
	void SetConditionRect2(const CRect& rect){m_conditionRect2 = rect;}
	void SetConditionRect3(const CRect& rect){m_conditionRect3 = rect;}
	void SetConditionRect4(const CRect& rect){m_conditionRect4 = rect;}
	void SetEmpty();
	void SetPrint(const NamedPrint& print);
	void Draw(HDC dc, const RECT& paintRect) const;
	void CopyData(const MyPrint& other)
	{
		RectCollection::CopyData(other);
		NamedPrint::operator=(other);
		m_bgColor = other.m_bgColor;
		m_fgColor = other.m_fgColor;
	}
	void SetBgColor(COLORREF color){m_bgColor = color;}
	void SetFgColor(COLORREF color){m_fgColor = color;}
	COLORREF GetBgColor() const{return m_bgColor;}
	COLORREF GetFgColor() const{return m_fgColor;}
	void SetValues(unsigned int mmid,
		unsigned short shortMmid,
		unsigned int dollars,
		unsigned int dollarFraction,
		unsigned int quantity,
		unsigned int roundLot,
		unsigned char source,
		unsigned char level1,
		char saleCondition1,
		char saleCondition2,
		char saleCondition3,
		char saleCondition4,
		unsigned int millisecond,
		short delay,
		bool historical,
		COLORREF bgColor,
		COLORREF fgColor);
protected:
	COLORREF m_bgColor;
	COLORREF m_fgColor;
	CRect m_conditionRect2;
	CRect m_conditionRect3;
	CRect m_conditionRect4;
};

class SampleDialogSettingMarketData : public BaseSampleDialogSettingOption//BaseSampleDialogSettingSymbol//, public Observer
{
public:
	SampleDialogSettingMarketData(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual ~SampleDialogSettingMarketData();
//	virtual HWND GetFirstTabControl() const override;
	virtual StringSetting* GetAccountIdSetting() override;

	virtual void UpdateSettings() override;
	virtual void UpdateControls() override;

	virtual void RestoreLayout() override;

	static const int quoteHgap = 3;
	static const int quoteVgap = 1;

	CFont* GetQuoteFont() const;

	bool ProcessUnappliedMessage(const Message* message);
protected:
	virtual void Entering() override;
	virtual void Leaving() override;
//	void AddPrint(const TMsgStockAddPrint* msg);
	bool DoAddPrint(const TMsgStockAddPrint* msg);
	void ScrollPrintLines(int count);

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	
	virtual void DoPaint(const RECT& rcPaint, CDC& dc) override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;
	virtual void DCCreated() override;

	void CalculateSizes();
//	int CalculateWidth(const char* str);
	CFont* m_quoteFont;
	unsigned int m_quoteCount;
	unsigned int m_roundLot;
	bool m_sizesCalculated;
	int m_quoteHeight;
	int m_quoteSpace;
	int m_sizeWidth;
	int m_venueWidth;
	int m_priceWidth;
	int m_conditionWidth;
	int m_timeWidth;
	int m_delayWidth;

	CRect m_bidRect;
	CRect m_bidTitleRect;
	CRect m_bidSizeNameRect;
	CRect m_bidVenueNameRect;
	CRect m_bidPriceNameRect;
	CRect m_bidConditionNameRect;
	CRect m_bidTimeNameRect;
	CRect m_bidDelayNameRect;

	CRect m_askRect;
	CRect m_askTitleRect;
	CRect m_askSizeNameRect;
	CRect m_askVenueNameRect;
	CRect m_askPriceNameRect;
	CRect m_askConditionNameRect;
	CRect m_askTimeNameRect;
	CRect m_askDelayNameRect;

	CRect m_printRect;
	CRect m_printRectMinusOneLine;
	CRect m_printTitleRect;
	CRect m_printSizeNameRect;
	CRect m_printVenueNameRect;
	CRect m_printPriceNameRect;
	CRect m_printConditionNameRect1;
	CRect m_printConditionNameRect2;
	CRect m_printConditionNameRect3;
	CRect m_printConditionNameRect4;
	CRect m_printTimeNameRect;
	CRect m_printDelayNameRect;

	CRect m_l1TitleRect;
	CRect m_l1BidNameRect;
	CRect m_l1AskNameRect;
	CRect m_l1SpreadNameRect;
	CRect m_l2BidNameRect;
	CRect m_l2AskNameRect;
	CRect m_l2SpreadNameRect;
	CRect m_l1LuldBidNameRect;
	CRect m_l1LuldAskNameRect;
#ifdef LRPS
	CRect m_l1LrpBidNameRect;
	CRect m_l1LrpAskNameRect;
#endif
	CRect m_l1StatusNameRect;
	CRect m_positionSizeNameRect;
	CRect m_positionPriceNameRect;
	CRect m_positionClosedPnlNameRect;
	CRect m_positionOpenPnlNameRect;

	CRect m_l1ValueRect;
	CRect m_l1BidRect;
	CRect m_l1AskRect;
	CRect m_l1SpreadRect;
	CRect m_l2BidRect;
	CRect m_l2AskRect;
	CRect m_l2SpreadRect;
	CRect m_l1LuldBidRect;
	CRect m_l1LuldAskRect;
#ifdef LRPS
	CRect m_l1LrpBidRect;
	CRect m_l1LrpAskRect;
#endif
	CRect m_l1StatusRect;
	CRect m_positionSizeRect;
	CRect m_positionPriceRect;
	CRect m_positionClosedPnlRect;
	CRect m_positionOpenPnlRect;

	Price m_l1Bid;
	Price m_l1Ask;
	SignedPrice m_l1Spread;
	Price m_l2Bid;
	Price m_l2Ask;
	SignedPrice m_l2Spread;
	Price m_l1LuldBid;
	Price m_l1LuldAsk;
#ifdef LRPS
	Price m_l1LrpBid;
	Price m_l1LrpAsk;
#endif
	char m_statusChar;
	int m_positionSize;
	Price m_positionPrice;
	Money m_positionClosedPnl;
	Money m_positionOpenPnl;

	std::string m_l1BidStr;
	std::string m_l1AskStr;
	std::string m_l1SpreadStr;
	std::string m_l2BidStr;
	std::string m_l2AskStr;
	std::string m_l2SpreadStr;
	std::string m_luldBidStr;
	std::string m_luldAskStr;
#ifdef LRPS
	std::string m_lrpBidStr;
	std::string m_lrpAskStr;
#endif
	std::string m_positionSizeStr;
	std::string m_positionPriceStr;
	std::string m_positionClosedPnlStr;
	std::string m_positionOpenPnlStr;

	typedef std::vector<MyQuote> QuoteVector;
	QuoteVector m_bidVector;
	QuoteVector m_askVector;

	typedef std::vector<MyPrint> PrintVector;
	PrintVector m_printVector;
/*
	QuoteVector::const_iterator m_bidVectorBegin;
	QuoteVector::const_iterator m_bidVectorEnd;
	QuoteVector::const_iterator m_askVectorBegin;
	QuoteVector::const_iterator m_askVectorEnd;
	PrintVector::const_iterator m_printVectorBegin;
	PrintVector::const_iterator m_printVectorEnd;
*/
	QuoteVector::iterator m_bidVectorBegin;
	QuoteVector::iterator m_bidVectorEnd;
	QuoteVector::iterator m_askVectorBegin;
	QuoteVector::iterator m_askVectorEnd;
	PrintVector::iterator m_printVectorBegin;
	PrintVector::iterator m_printVectorEnd;

	COLORREF m_bidColor;
	COLORREF m_askColor;
	COLORREF m_printColor;

	COLORREF m_colorPrintBuyAggressiveText;
	COLORREF m_colorPrintBuyAggressive;
	COLORREF m_colorPrintBuy;
	COLORREF m_colorPrintNeutral;
	COLORREF m_colorPrintSell;
	COLORREF m_colorPrintSellAggressive;
	COLORREF m_colorPrintSellAggressiveText;
/*
	int m_accountGroupVerticalOffset;
	int m_accountGroupHorizontalOffset;
	int m_accountComboVerticalOffset;
	int m_accountComboHorizontalOffset;
*/
//	ValidEdit m_EditSymbol;
	CButton m_ButtonSubscribe;
	CStatic m_StaticSymbolSubscribed;
//	CButton m_GroupAccount;
//	ComboBoxAccount m_ComboBoxAccount;
//	std::string m_symbol;
	const Security* m_security;
	const Position* m_position;
/*
	const Account* m_account;
	char m_accountId[MAX_ACCOUNT_SIZE];
	void AddAccount(const Account* account);
	void SetAccount(const Account* account);
*/
	void SetPosition(const Position* position);
	bool CanSubscribe() const;
	void EnableSubscribeButton();

	virtual void AccountChanged();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg void OnSelchangeConnection();
	
//	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnSubscribe();
//	afx_msg void OnSelchangeAccount();
	DECLARE_MESSAGE_MAP()

	TakionIterator* m_bidIterator;
	TakionIterator* m_askIterator;
	TakionIterator* m_printIterator;
	
	UnappliedMessageReader m_unappliedMessageReader;//for print processing. When an update message comes, there can be several prints in this object.

	bool DoSubscribe();
	void SecurityFailed();
	void UpdateEntitlements();
//	void UpdateExtensionEntitlements();
	void SecurityRefreshed();
	void SecurityLevel2Updated();
	void SecurityLevel1Updated();
	void UpdatePrints();
	void UpdatePosition();
	void Cleanup();
	void SetIteratorSecurity();
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;

#ifndef TAKION_NO_OPTIONS
	virtual bool OptionModeChanged();
	virtual void OptionValuesChanged();
#endif
};
