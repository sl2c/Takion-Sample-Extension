#pragma once

#include "BaseDialogSettingAccount.h"
#include "ConfigEnums.h"

class SampleDialogSettingOrder;
class OrderObserver;

enum OrderColumnId : unsigned short
{
	OCI_SYMBOL,
#ifndef TAKION_NO_OPTIONS
	OCI_OPTION_CALLPUT,
	OCI_OPTION_EXPIRATION,
	OCI_OPTION_STRIKE_PRICE,
#endif
	OCI_CLIENT_ID,
	OCI_SIDE,
	OCI_LIMIT_PRICE,
	OCI_STOP_PRICE,
	OCI_SIZE,

	OCI_VENUE,
	OCI_ROUTING,
	OCI_TIF,
	OCI_TIME_CREATED,
	OCI_SERVER_ID,

	OCI_EXECUTED_PRICE,
	OCI_PENDING_SIZE,
	OCI_EXECUTED_SIZE,
	OCI_VISIBLE_SIZE,
	OCI_STATUS,
	OCI_TIME_CANCEL_SENT,
	OCI_TIME_DEAD,
	OCI_REPLACED_CLIENT_ID,
	OCI_REPLACED_BY_CLIENT_ID,
	OCI_REPLACED_BY_SEND_ID,
	OCI_CLOSED_PNL,
	OCI_BP_USED,
	OCI_REJECT_CODE,
	OCI_REJECT_TEXT,

	OCI_SEND_ID,
	OCI_PARENT_ID,
	OCI_ISO,
	OCI_REPLACING_NATIVELY,

	OCI_Count,
};

class ListBoxOrder : public ColumnedListBox
{
public:
	ListBoxOrder(SampleDialogSettingOrder* owner, const Account* account):
		ColumnedListBox(0, true, true),
//		m_loadedColor(0),
//		m_unloadedColor(RGB(128, 0, 0)),
		m_executorConnected(false),
		m_historyLoaded(false),
		m_owner(owner),
		m_account(account),
		m_sizePending(0),
		m_sizeExecuted(0),
		m_isoCount(0),
		m_replacingNativelyCount(0),
#ifndef TAKION_NO_OPTIONS
		m_optionCount(0),
#endif
		m_colorUnacknowledged(RGB(96, 128, 0)),
		m_colorPending(RGB(0, 0, 0)),
		m_colorPartiallyFilled(RGB(0, 192, 0)),
		m_colorFilled(RGB(0, 128, 0)),
		m_colorCancelled(RGB(128, 0, 0)),
		m_colorReplaced(RGB(128, 0, 64)),
		m_colorRejected(RGB(128, 64, 0)),
		m_colorError(RGB(96, 0, 96)),
		m_menuWidth(0),
		m_inThread(false)
	{
	}
	void SetInThread(const bool inThread);

	virtual void DestroyItem(const void* item) const override;
//	virtual const char* GetItemName(const void* item) const override;

	void AddItem(const OrderObserver* item);
	bool RemoveRowAt(int found);
	bool RemoveRow(const OrderObserver* item);
	bool RemoveOrder(const Order* order);
	bool RemoveOrderByClientId(unsigned int clientId);
	void RemoveItemAt(int found, const OrderObserver* item);
	void SetExecutorConnected(bool connected);
	bool isExecutorConnected() const{return m_executorConnected;}
	void SetHistoryLoaded(bool historyLoaded);
	bool isHistoryLoaded() const{return m_historyLoaded;}
	void SetAccount(const Account* account);

	SampleDialogSettingOrder* GetOwnerDialog(){return m_owner;}
	const SampleDialogSettingOrder* GetOwnerDialog() const{return m_owner;}

	virtual bool HasItemToolTips() const override{return false;}

	static void LogOrderError(unsigned char errorCode, const char* prefix);

	static void LogCancelError(unsigned char errorCode, const char* prefix);
	virtual COLORREF GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	virtual COLORREF GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;

	void RepaintCount() const;
	void RepaintClosedPnl() const;
	void RepaintBpUsed() const;
	void RepaintSizePending() const;
	void RepaintSizeExecuted() const;
	void RepaintIsoCount() const;
	void RepaintReplacingNativelyCount() const;
#ifndef TAKION_NO_OPTIONS
	void RepaintOptionCount() const;
	bool UpdateOptionCount(const unsigned int& oldValue, const unsigned int& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_optionCount))
		{
			RepaintOptionCount();
			return true;
		}
		return false;
	}
	const unsigned int& GetOptionCount() const{return m_optionCount;}
#endif
//Totals
	const Money& GetClosedPnl() const{return m_closedPnl;}
	const Money& GetBpUsed() const{return m_bpUsed;}
	const unsigned int& GetSizePending() const{return m_sizePending;}
	const unsigned int& GetSizeExecuted() const{return m_sizeExecuted;}
	const unsigned int& GetIsoCount() const{return m_isoCount;}
	const unsigned int& GetReplacingNativelyCount() const{return m_replacingNativelyCount;}
/*
	template <class T>
	static bool UpdateUnsignedValue(T& value, const T& oldValue, const T& newValue)
	{
		if(oldValue < newValue)
		{
			value += newValue - oldValue;
			return true;
		}
		else if(oldValue > newValue)
		{
			value -= oldValue - newValue;
			return true;
		}
		return false;
	}
	template <class T>
	static bool UpdateSignedValue(T& value, const T& oldValue, const T& newValue)
	{
		if(newValue != oldValue)
		{
			value += newValue - oldValue;
			return true;
		}
		return false;
	}
*/
	bool UpdateIsoCount(const unsigned int& oldValue, const unsigned int& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_isoCount))
		{
			RepaintIsoCount();
			return true;
		}
		return false;
	}
	bool UpdateReplacingNativelyCount(const unsigned int& oldValue, const unsigned int& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_replacingNativelyCount))
		{
			RepaintReplacingNativelyCount();
			return true;
		}
		return false;
	}
	bool UpdateSizePending(const unsigned int& oldValue, const unsigned int& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_sizePending))
		{
			RepaintSizePending();
			return true;
		}
		return false;
	}
	bool UpdateSizeExecuted(const unsigned int& oldValue, const unsigned int& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_sizeExecuted))
		{
			RepaintSizeExecuted();
			return true;
		}
		return false;
	}
	bool UpdateClosedPnl(const Money& oldValue, const Money& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_closedPnl))
		{
			RepaintClosedPnl();
			return true;
		}
		return false;
	}
	bool UpdateBpUsed(const Money& oldValue, const Money& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_bpUsed))
		{
			RepaintBpUsed();
			return true;
		}
		return false;
	}

	bool UpdateSizePendingByDiff(const unsigned int& diff)
	{
		if(diff)
		{
			if(m_sizePending > diff)m_sizePending -= diff;
			else m_sizePending = 0;
			RepaintSizePending();
			return true;
		}
		return false;
	}
	bool UpdateSizeExecutedByDiff(const unsigned int& diff)
	{
		if(diff)
		{
			m_sizeExecuted += diff;
			RepaintSizeExecuted();
			return true;
		}
		return false;
	}
	bool UpdateClosedPnlByDiff(const Money& diff)
	{
		if(!diff.isZero())
		{
			m_closedPnl += diff;
			RepaintClosedPnl();
			return true;
		}
		return false;
	}
	bool UpdateBpUsedByDiff(const Money& diff)
	{
		if(!diff.isZero())
		{
			m_bpUsed += diff;
			RepaintBpUsed();
			return true;
		}
		return false;
	}

	virtual int Compare(const void* item1, const void* item2) const override;
	int FindOrderByClientId(const unsigned int clientId) const;
	bool ClearItemsWithRedraw()
	{
		ResetTotals();
		return m_hWnd && ColumnedListBox::ClearItemsWithRedraw();
	}
protected:
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
	virtual void HorizontalScrollingDone(int diff) override;
//	virtual void DoCreateColumnToolTips() override;
	virtual void ListBoxCreated() override;

	virtual void SizeChanged(bool horizontalChange, bool verticalChange) override;
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;

	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;
	virtual void AddMenuItems(CMenu& menu) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;

//	COLORREF m_loadedColor;
//	COLORREF m_unloadedColor;
	bool m_executorConnected;
	bool m_historyLoaded;
	SampleDialogSettingOrder* m_owner;
	const Account* m_account;

//totals
	void ResetTotals();

	Money m_closedPnl;
	Money m_bpUsed;
	unsigned int m_sizePending;
	unsigned int m_sizeExecuted;
	unsigned int m_isoCount;
	unsigned int m_replacingNativelyCount;

#ifndef TAKION_NO_OPTIONS
	unsigned int m_optionCount;
#endif

	COLORREF m_colorUnacknowledged;
	COLORREF m_colorPending;
	COLORREF m_colorPartiallyFilled;
	COLORREF m_colorFilled;
	COLORREF m_colorCancelled;
	COLORREF m_colorReplaced;
	COLORREF m_colorRejected;
	COLORREF m_colorError;

	int m_menuWidth;

	DECLARE_MESSAGE_MAP()
	LRESULT OnItemRepaint(WPARAM clientId, LPARAM columns);
	bool m_inThread;

};

class ListBoxOrderStatus : public ListBoxSimpleFilter<unsigned char, 8>
{
public:
	ListBoxOrderStatus():ListBoxSimpleFilter((1 << FOST_Count) - 1, FOST_Count){}
	virtual const char* GetItemName(const void* item) const override
	{
		switch((unsigned char)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			item)
		{
			case FOST_PENDING:
			return "Pending";

			case FOST_FILLED:
			return "Filled";

			case FOST_UNFILLED:
			return "Unfilled";

			default:
			return "";
		}
	}
};

class ListBoxOrderSide : public ListBoxSimpleFilter<unsigned char, 8>
{
public:
	ListBoxOrderSide():ListBoxSimpleFilter((1 << FOS_Count) - 1, FOS_Count){}
	virtual const char* GetItemName(const void* item) const override
	{
		switch((unsigned char)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			item)
		{
			case FOS_BUY:
			return "Buy";

			case FOS_SELL:
			return "Sell";

			case FOS_SHORT:
			return "Short";

			default:
			return "";
		}
	}
};

class SampleDialogSettingOrder : public BaseDialogSettingAccount//, public Observer
{
friend class ListBoxOrder;
public:
	SampleDialogSettingOrder(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
//	virtual ~SampleDialogSettingOrder(){}
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;

#ifndef TAKION_NO_OPTIONS
	unsigned int GetExpirationDate() const{return (unsigned int)m_expirationYear * 10000 + (unsigned int)m_expirationMonth * 100 + m_expirationDay;}
	unsigned int GetControlExpirationDate() const{return (unsigned int)m_controlExpirationYear * 10000 + (unsigned int)m_controlExpirationMonth * 100 + m_controlExpirationDay;}
//	bool isOptionModeUsed() const{return true;}

	bool isOptionUsed() const{return m_optionMode;}
	bool isControlOptionUsed() const{return m_controlOptionMode;}
	bool AreSettingsValid() const;
	bool AreOptionSettingsValid() const;

	bool isExpirationValid() const;
	bool isControlExpirationValid() const;
	bool GetOptionValues(unsigned __int64& rootSymbol, unsigned __int64& underlierSymbol, unsigned __int64& optionBlock, unsigned short& optionType, unsigned char& securityType) const;
	void HideOptionControls(bool hide);
	void SetExpirationDate(unsigned int date);
	void SetExpirationDate(const unsigned char expirationYear, const unsigned char expirationMonth, const unsigned char expirationDay);
	void SetCallPut(bool call);
	void SetStrikePrice(const Price& strikePrice);
	void SetOptionValues(bool useOption,
//		char suffix,
		bool call, const Price& strikePrice, unsigned int expirationDate);
	unsigned __int64 CalculateCurrentOptionBlock() const;
	void RepaintOptionCount() const;
#endif

	void RepaintCount() const;
	void RepaintClosedPnl() const;
	void RepaintBpUsed() const;
	void RepaintSizePending() const;
	void RepaintSizeExecuted() const;
	void RepaintIsoCount() const;
	void RepaintReplacingNativelyCount() const;

	void RepaintTotals() const;

//	void CurrentAccountSet(const Account* account);
//	const Account* GetAccount() const{return m_account;}
	virtual void RestoreLayout() override;
	virtual StringSetting* GetAccountIdSetting() override;//{return m_orderAccountId;}

	const int& GetListBoxBorderSize() const{return m_listBoxBorderSize;}
	const int& GetLeftOffset() const{return m_leftOffset;}
	const int& GetTopOffset() const{return m_topOffset;}

	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

	const Position* FindPosition() const;

	void PasteOrder(const Order* order);

	unsigned char GetOrderStatusMask() const;
	unsigned char GetOrderSideMask() const;
	unsigned char GetOrderInstrumentMask() const;
	bool OrderBelongs(const Order* order) const;
	bool OrderObserverBelongs(const OrderObserver* orderObserver) const;
	bool OrderObserverStatusBelongs(const OrderObserver* orderObserver) const;
	static unsigned int CalculateTakionOrderStatusMask(unsigned char orderStatusMask);
	static unsigned int CalculateTakionOrderSideMask(unsigned char orderSideMask);

	const SignedPrice& GetReplacePriceOffset() const{return m_replacePriceOffset;}
	const int& GetReplaceSizeOffset() const{return m_replaceSizeOffset;}
	const bool& isReplacePriceOffsetPercent() const{return m_replacePriceOffsetPercent;}
	const bool& isReplacePriceEnforceMinOffset() const{return m_replacePriceEnforceMinOffset;}
	const bool& isReplaceSizeOffsetPercent() const{return m_replaceSizeOffsetPercent;}
	const bool& isReplaceNative() const{return m_replaceNative;}
	const bool& isReplaceMarketOrder() const{return m_replaceMarketOrder;}
	const bool& isReplacePreBorrow() const{return m_replacePreBorrow;}
	const Price& GetReplacePreBorrowPrice() const{return m_replacePreBorrowPrice;}

	unsigned char SampleDialogSettingOrder::GetSecurityType() const;
protected:
	virtual void ToolTipsEnabled(bool enable) override;

	virtual BOOL OnInitDialog() override;

	virtual void AddAccountObserver();
	virtual void RemoveAccountObserver();

	void ResetOffsets();

	void ClearOrders();

	bool SetPosition(const Position* position);
	void EnableSymbolButton();
	bool ApplySymbol();
	bool CanApplySymbol() const;

	void AddNewOrder(Order* order);
	void AddOrder(Order* order);

	const Position* m_position;
//	std::string m_symbol;
//	std::string m_controlSymbol;
	bool m_symbolCurrent;

	unsigned __int64 m_numericSymbol;
	unsigned __int64 m_controlNumericSymbol;

	void AddOrderByFilter(unsigned char addStatusFilter, unsigned char addSideFilter, unsigned char addInstrumentFilter);

	void UpdateOrderStatusMask();
	void RemoveOrderStatus(unsigned char removeFilter);
	unsigned char m_orderStatusMask;//enum FilterOrderStatus in ConfigEnums.h

	void UpdateOrderSideMask();
	void RemoveOrderSide(unsigned char removeFilter);
	unsigned char m_orderSideMask;//enum FilterOrderSide in ConfigEnums.h

	void UpdateOrderInstrumentMask();
	void RemoveOrderInstrument(unsigned char removeFilter);
	unsigned char m_orderInstrumentMask;//enum FilterOrderStatus in ConfigEnums.h

	void RemoveOrdersButSymbol();

//	virtual void TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff) override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnInThread();
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnSymbol();
	afx_msg void OnSelchangeOrder();
	afx_msg void OnDblclkOrder();
	afx_msg void OnReplacePriceOffsetPercent();
	afx_msg void OnReplacePriceEnforceMinOffset();
	afx_msg void OnReplaceSizeOffsetPercent();
	afx_msg void OnReplaceNative();
	afx_msg void OnPreBorrow();
	afx_msg void OnReplaceMarketOrder();
	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	afx_msg LRESULT OnListBoxChecked(WPARAM listBoxId, LPARAM l);

	DECLARE_MESSAGE_MAP()

	CheckBoxOwnerDraw m_CheckBoxInThread;

	ValidEdit m_EditSymbol;
	CButton m_ButtonSymbol;
	CStatic m_StaticSymbol;

#ifndef TAKION_NO_OPTIONS
	afx_msg void OnCalendarDropdown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCalendarCloseup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExpirationDateChange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOption();
	afx_msg void OnOptionCall();
	afx_msg void OnOptionPut();
	afx_msg void OnFuture();

	CStatic m_StaticOptionInfo;
	CStatic m_StaticOptionStrikePrice;
	CStatic m_StaticOptionExpiration;
	CheckBoxOwnerDraw m_CheckBoxOption;
	RadioGroup m_RadioGroupOption;
	RadioOwnerDraw m_RadioOptionCall;
	RadioOwnerDraw m_RadioOptionPut;
	UnsignedNumberTypeSpin<unsigned int> m_SpinOptionStrikePrice;
	ColorDateTimeCtrl m_DateTimeOptionExpiration;

	CheckBoxOwnerDraw m_CheckBoxFuture;

	unsigned __int64 m_optionBlock;
	bool m_optionBlockValid;
	bool m_optionMode;
	bool m_optionCall;
	unsigned char m_expirationYear;
	unsigned char m_expirationMonth;
	unsigned char m_expirationDay;
	bool m_future;
	Price m_strikePrice;


	unsigned __int64 m_controlOptionBlock;
	bool m_controlOptionBlockValid;
	bool m_controlOptionMode;
	bool m_controlOptionCall;
	unsigned char m_controlExpirationYear;
	unsigned char m_controlExpirationMonth;
	unsigned char m_controlExpirationDay;
	bool m_controlFuture;
	Price m_controlStrikePrice;

	bool isExpirationDateCurrent() const
	{
		return m_expirationDay == m_controlExpirationDay
			&& m_expirationMonth == m_controlExpirationMonth
			&& m_expirationYear == m_controlExpirationYear;
	}

	void UpdateCallPutColor();
	void UpdateControlOptionBlock();
	bool UpdateOptionExpirationValue();
	void SetOptionCall(const bool call);
	bool OptionModeChanged();
//	virtual void OptionValuesChanged(){}
	void OptionValuesChanged();
	bool InvalidateOptionControls();

	bool FutureModeChanged();
#endif

//	ComboBoxAccount m_ComboBoxAccount;

	ListBoxOrderStatus m_ListBoxOrderStatus;
	ListBoxOrderSide m_ListBoxOrderSide;
	ListBoxInstrument m_ListBoxOrderInstrument;

	CheckBoxOwnerDraw m_CheckBoxReplaceMarketOrder;
	CStatic m_StaticReplacePriceOffset;
	CheckBoxOwnerDraw m_CheckBoxReplacePriceOffsetPercent;
	CheckBoxOwnerDraw m_CheckBoxReplacePriceEnforceMinOffset;
	SignedNumberTypeSpin<int> m_SpinReplacePriceOffset;
	CheckBoxOwnerDraw m_CheckBoxReplaceSizeOffsetPercent;
	SignedNumberTypeSpin<int> m_SpinReplaceSizeOffset;
	CheckBoxOwnerDraw m_CheckBoxReplaceNative;
	CheckBoxOwnerDraw m_CheckBoxPreBorrow;
	UnsignedNumberTypeSpin<unsigned int> m_SpinReplacePreBorrowPrice;

	CStatic m_StaticReplaceSendId;
	CStatic m_StaticLastSendIdDone;

	ListBoxOrder m_ListBoxOrder;

	virtual void AccountChanged() override;
	virtual void OnHistoryLoaded() override;
	virtual void OnExecutorConnected() override;
	virtual bool ShouldAddAccountObserver() const override;
//	int m_editLeftOffset;
//	int m_editTopOffset;
	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;
//	int m_timerLeftOffset;
//	int m_timerTopOffset;

	void SetReplaceSendId(unsigned int sendId);
	void SetLastSendIdDone(unsigned int sendId);
	SignedPrice m_replacePriceOffset;
	int m_replaceSizeOffset;
	bool m_replacePriceOffsetPercent;
	bool m_replacePriceEnforceMinOffset;
	bool m_replaceSizeOffsetPercent;
	bool m_replaceNative;
	bool m_replaceMarketOrder;
	bool m_replacePreBorrow;
	Price m_replacePreBorrowPrice;
//    virtual void DoPaint(const RECT& rcPaint, CDC& dc) override;
/*
	void SetExecutorConnected(bool connected);
	void SetHistoryLoaded(bool loaded);
	bool m_executorConnected;
	bool m_historyLoaded;
*/
	void SetInThread(const bool inThread);
	bool m_inThread;

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
