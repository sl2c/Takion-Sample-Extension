#pragma once

#include "BaseDialogSettingAccount.h"
#include "ConfigEnums.h"
#include <UtilsGuiApi.h>
#include <TakionUtilsApi.h>

class SampleDialogSettingPosition;
class PositionObserver;

enum PositionColumnId : unsigned short
{
	PCI_SYMBOL,
#ifndef TAKION_NO_OPTIONS
	PCI_OPTION_CALLPUT,
	PCI_OPTION_EXPIRATION,
	PCI_OPTION_STRIKE_PRICE,
#endif
	PCI_PRICE,
	PCI_SIZE,
	PCI_ABS_SIZE,
	PCI_CLOSED_PNL,
	PCI_OPEN_PNL,
	PCI_OVERNIGHT_SIZE,
	PCI_OVERNIGHT_PRICE,

	PCI_Count,
};

class ListBoxPosition : public ColumnedListBox
{
public:
	ListBoxPosition(SampleDialogSettingPosition* owner, const Account* account) :
		ColumnedListBox(0, true, true),
//		m_loadedColor(0),
//		m_unloadedColor(RGB(128, 0, 0)),
		m_executorConnected(false),
		m_historyLoaded(false),
		m_owner(owner),
		m_account(account),
		m_size(0),
		m_absSize(0),
		m_overnightSize(0),
#ifndef TAKION_NO_OPTIONS
		m_optionCount(0),
#endif
		m_colorFlat(RGB(0, 0, 0)),
		m_colorLong(RGB(0, 128, 0)),
		m_colorShort(RGB(128, 0, 0)),
		m_colorError(RGB(96, 0, 96)),
		m_inThread(false)
	{
	}
	void SetInThread(const bool inThread);

	virtual void DestroyItem(const void* item) const override;
	//	virtual const char* GetItemName(const void* item) const override;

	void AddItem(const PositionObserver* item);
	bool RemoveRowAt(int found);
	bool RemoveRow(const PositionObserver* item);
	bool RemovePosition(const Position* position);
	void RemoveItemAt(int found, const PositionObserver* item);
	void SetExecutorConnected(bool connected);
	bool isExecutorConnected() const{return m_executorConnected;}
	void SetHistoryLoaded(bool historyLoaded);
	bool isHistoryLoaded() const{return m_historyLoaded;}
	void SetAccount(const Account* account);

	SampleDialogSettingPosition* GetOwnerDialog(){return m_owner;}
	const SampleDialogSettingPosition* GetOwnerDialog() const{return m_owner;}

	virtual bool HasItemToolTips() const override{return false;}

//	static void LogOrderError(unsigned char errorCode, const char* prefix);
//	static void LogCancelError(unsigned char errorCode, const char* prefix);
	virtual COLORREF GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	virtual COLORREF GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;

	void RepaintCount() const;
	void RepaintSize() const;
	void RepaintAbsSize() const;
	void RepaintOvernightSize() const;
//	void RepaintPrice();
//	void RepaintOvernightPrice();
	void RepaintClosedPnl() const;
	void RepaintOpenPnl() const;
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
	const int& GetSize() const{return m_size;}
	const unsigned int& GetAbsSize() const{return m_absSize;}
	const int& GetOvernightSize() const{return m_overnightSize;}
	const Money& GetClosedPnl() const{return m_closedPnl;}
	const Money& GetOpenPnl() const{return m_openPnl;}
//	const Price& Getprice() const{return m_price;}
//	const Price& GetOvernightPrice() const{return m_overnightPrice;}
	bool UpdateSize(const int& oldValue, const int& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_size))
		{
			RepaintSize();
			return true;
		}
		return false;
	}
	bool UpdateAbsSize(const unsigned int& oldValue, const unsigned int& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_absSize))
		{
			RepaintAbsSize();
			return true;
		}
		return false;
	}
	bool UpdateOvernightSize(const int& oldValue, const int& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_overnightSize))
		{
			RepaintOvernightSize();
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
	bool UpdateOpenPnl(const Money& oldValue, const Money& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_openPnl))
		{
			RepaintOpenPnl();
			return true;
		}
		return false;
	}
/*
	bool UpdatePrice(const Price& oldValue, const Price& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_price))
		{
			RepaintPrice();
			return true;
		}
		return false;
	}
	bool UpdateOvernightPrice(const Price& oldValue, const Price& newValue)
	{
		if(U_UpdateValueByDifference(oldValue, newValue, m_overnightPrice))
		{
			RepaintOvernightPrice();
			return true;
		}
		return false;
	}
*/
	bool UpdateSizeByDiff(const int& diff)
	{
		if(diff)
		{
			m_size += diff;
			RepaintSize();
			m_absSize = m_size >= 0 ? m_size : -m_size;
			RepaintAbsSize();
			return true;
		}
		return false;
	}
	bool UpdateOvernightSizeByDiff(const int& diff)
	{
		if(diff)
		{
			m_overnightSize += diff;
			RepaintOvernightSize();
			return true;
		}
		return false;
	}

/*
	bool UpdateAbsSizeByDiff(const unsigned int& diff)
	{
		if(diff)
		{
			m_absSize += diff;
			RepaintAbsSize();
			return true;
		}
		return false;
	}
*/
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
	bool UpdateOpenPnlByDiff(const Money& diff)
	{
		if(!diff.isZero())
		{
			m_openPnl += diff;
			RepaintOpenPnl();
			return true;
		}
		return false;
	}
/*
	bool UpdatePriceByDiff(const Money& diff)
	{
		if(!diff.isZero())
		{
			m_price += diff;
			RepaintPrice();
			return true;
		}
		return false;
	}
*/
	virtual int Compare(const void* item1, const void* item2) const override;
	int CompareItemTo(const void* item1, const unsigned __int64& numericSymbol
#ifndef TAKION_NO_OPTIONS
		, const OptionBlock& optionBlock
#endif
		) const;
	int FindPosition(const unsigned __int64& numericSymbol
#ifndef TAKION_NO_OPTIONS
		, const unsigned __int64& optionBlock
#endif
		) const;
	bool ClearItemsWithRedraw()
	{
		ResetTotals();
		return m_hWnd && ColumnedListBox::ClearItemsWithRedraw();
	}
protected:
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
	virtual void HorizontalScrollingDone(int diff) override;
	//	virtual void DoCreateColumnToolTips() override;
	//	virtual void ListBoxCreated() override;

	virtual void SizeChanged(bool horizontalChange, bool verticalChange) override;
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;
//	virtual void AddMenuItems(CMenu& menu) override;
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;

//	COLORREF m_loadedColor;
//	COLORREF m_unloadedColor;
	bool m_executorConnected;
	bool m_historyLoaded;
	SampleDialogSettingPosition* m_owner;
	const Account* m_account;

//totals
	void ResetTotals();

	Money m_closedPnl;
	Money m_openPnl;
//	Price m_price;
//	Price m_overnightPrice;
	int m_size;
	unsigned int m_absSize;
	int m_overnightSize;

#ifndef TAKION_NO_OPTIONS
	unsigned int m_optionCount;
#endif

	COLORREF m_colorLong;
	COLORREF m_colorShort;
	COLORREF m_colorFlat;
	COLORREF m_colorError;

	DECLARE_MESSAGE_MAP()
	LRESULT OnItemRepaint(WPARAM clientId, LPARAM columns);
	bool m_inThread;

};

class ListBoxPositionStatus : public ListBoxSimpleFilter<unsigned char, 8>
{
public:
	ListBoxPositionStatus():ListBoxSimpleFilter((1 << TU_POSITION_STATUS_COUNT) - 1, TU_POSITION_STATUS_COUNT){}
	virtual const char* GetItemName(const void* item) const override
	{
		switch((unsigned char)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			item)
		{
			case TU_POSITION_STATUS_FLAT:
			return "Flat";

			case TU_POSITION_STATUS_LONG:
			return "Long";

			case TU_POSITION_STATUS_SHORT:
			return "Short";

			default:
			return "";
		}
	}
};

class SampleDialogSettingPosition : public BaseDialogSettingAccount//, public Observer
{
friend class ListBoxPosition;
public:
	SampleDialogSettingPosition(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
//	virtual ~SampleDialogSettingPosition(){}
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;

#ifndef TAKION_NO_OPTIONS
	void RepaintOptionCount() const;
#endif
	void RepaintCount() const;
	void RepaintClosedPnl() const;
	void RepaintOpenPnl() const;
	void RepaintSize() const;
	void RepaintAbsSize() const;
	void RepaintOvernightSize() const;

	void RepaintTotals() const;

//	void CurrentAccountSet(const Account* account);
//	const Account* GetAccount() const{return m_account;}
	virtual void RestoreLayout() override;
	virtual StringSetting* GetAccountIdSetting() override;//{return m_orderAccountId;}

	int GetListBoxBorderSize() const{return m_listBoxBorderSize;}
	int GetLeftOffset() const{return m_leftOffset;}
	int GetTopOffset() const{return m_topOffset;}

	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

//	const Position* FindPosition() const;
//	void PastePosition(const Position* position);

	unsigned char GetPositionStatusMask() const;
	unsigned char GetPositionInstrumentMask() const;
	bool PositionBelongs(const Position* order) const;
	bool PositionObserverBelongs(const PositionObserver* orderObserver) const;
	bool PositionObserverStatusBelongs(const PositionObserver* orderObserver) const;

	bool SizeBelongs(const int& size) const
	{
		return size > 0 ? (m_positionStatusMask & (1 << TU_POSITION_STATUS_LONG)) != 0 :
			size < 0 ? (m_positionStatusMask & (1 << TU_POSITION_STATUS_SHORT)) != 0 :
			(m_positionStatusMask & (1 << TU_POSITION_STATUS_FLAT)) != 0;
	}
	bool InstrumentBelongs(
#ifndef TAKION_NO_OPTIONS
		const unsigned char& securityType
#endif
		) const
	{
		return
#ifndef TAKION_NO_OPTIONS
//			option ? (m_positionInstrumentMask & (1 << ST_OPTION)) != 0:
			(m_positionInstrumentMask & (1 << securityType)) != 0;
#else
			(m_positionInstrumentMask & (1 << ST_STOCK)) != 0;
#endif
	}

protected:
	virtual void ToolTipsEnabled(bool enable) override;

	virtual BOOL OnInitDialog() override;

	virtual void AddAccountObserver();
	virtual void RemoveAccountObserver();

	void ResetOffsets();

	void ClearPositions();


	void AddNewPosition(const Position* position);
	void AddPosition(const Position* position);

	void AddPositionByFilter(unsigned char addStatusFilter, unsigned char addInstrumentFilter);

	void UpdatePositionStatusMask();
	void RemovePositionStatus(unsigned char removeFilter);
	unsigned char m_positionStatusMask;//enum FilterPositionStatus in ConfigEnums.h

	void UpdatePositionInstrumentMask();
	void RemovePositionInstrument(unsigned char removeFilter);
	unsigned char m_positionInstrumentMask;//enum FilterOrderSide in ConfigEnums.h

//	virtual void TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff) override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
//	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnInThread();
	afx_msg void OnSelchangePosition();
	afx_msg void OnDblclkPosition();

//	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	afx_msg LRESULT OnListBoxChecked(WPARAM listBoxId, LPARAM l);

	DECLARE_MESSAGE_MAP()

	CheckBoxOwnerDraw m_CheckBoxInThread;

	ListBoxPositionStatus m_ListBoxPositionStatus;
//#ifndef TAKION_NO_OPTIONS
	ListBoxInstrument m_ListBoxPositionInstrument;
//#endif
	ListBoxPosition m_ListBoxPosition;

	virtual void AccountChanged() override;
	virtual void OnHistoryLoaded() override;
	virtual void OnExecutorConnected() override;
	virtual bool ShouldAddAccountObserver() const override{return true;}
	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;

	void SetInThread(const bool inThread);
	bool m_inThread;

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};

