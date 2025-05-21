#pragma once

#include "BaseDialogSetting.h"

#include <afxmt.h>

class SampleDialogSettingTestRequest;
class MessageItem;

enum MessageColumnId : unsigned char
{
	MI_TIMESTAMP,
	MI_TYPE,
	MI_LENGTH,
//	MI_ADDITIONAL_LENGTH,
	MI_SRC,
	MI_OUT,

	MI_Count,
};

class ListBoxMessage : public ColumnedListBox
{
public:
	ListBoxMessage(SampleDialogSettingTestRequest* owner):
		ColumnedListBox(0, true, true),
		m_owner(owner),
		m_lineCount(0)
//		m_menuWidth(0)
	{
	}
	virtual void DestroyItem(const void* item) const override;
//	virtual const char* GetItemName(const void* item) const override;

//	void RemoveRow(const MessageItem* item);

	SampleDialogSettingTestRequest* GetOwnerDialog(){return m_owner;}
	const SampleDialogSettingTestRequest* GetOwnerDialog() const{return m_owner;}

	virtual bool HasItemToolTips() const override{return false;}

	const unsigned int& GetLineCount() const{return m_lineCount;}
	void RepaintLineCount() const;
	int AddLine(const MessageItem* item)
	{
		if(m_hWnd)
		{
//			int ret = AddString((const char*)item);
			int ret = InsertString(0, (const char*)item);
			if(ret >= 0)
			{
				++m_lineCount;
				RepaintLineCount();
//				LineAdded(item);
			}
			return ret;
		}
		return LB_ERR;
	}
	int DeleteLine(unsigned int i)
	{
		if(m_hWnd)
		{
			if(i < m_itemCount)//(unsigned int)GetCount())
			{
//				const MessageItem* item = (const MessageItem*)GetItemDataPtr(i);
//				LineRemoved(item);
				int ret = DeleteString(i);
				if(ret >= 0)
				{
					--m_lineCount;
					RepaintLineCount();
				}
				AdjustTopItem();
				return ret;
			}
		}
		return LB_ERR;
	}
	void ClearLines()
	{
		if(m_hWnd)
		{
			m_lineCount = 0;
//			if(GetCount() > 0)
			if(m_itemCount > 0)
			{
				SetRedraw(FALSE);

				ResetContent();

				SetRedraw(TRUE);
				Invalidate(FALSE);

				RepaintLineCount();
//				LinesCleared();
			}
		}
	}
protected:
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
//	virtual void HorizontalScrollingDone(int diff) override;
//	virtual void DoCreateColumnToolTips() override;

//	virtual void SizeChanged(bool horizontalChange, bool verticalChange) override;
//	virtual void ListBoxCreated() override;
//	virtual void DoDrawItem(const DRAWITEMSTRUCT* lpDrawItemStruct, const CRect& drawItemRect) override;
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;

//	virtual void ListBoxCreated() override;
	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;
	virtual void AddMenuItems(CMenu& menu) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;

	SampleDialogSettingTestRequest* m_owner;
	unsigned int m_lineCount;
//	int m_menuWidth;
//totals
//	void ResetTotals();
private:
	int DeleteString(UINT nIndex){return ColumnedListBox::DeleteString(nIndex);}
	int AddString(LPCTSTR lpszItem){return ColumnedListBox::AddString(lpszItem);}
	void ResetContent(){ColumnedListBox::ResetContent();}
};

class ListBoxRequestFilter : public ListBoxSimpleFilter<unsigned int, 32>
{
public:
	ListBoxRequestFilter():ListBoxSimpleFilter((1 << REQF_Count) - 1, REQF_Count){}
//Enum	ExtensionRequestFilterEnum in TakionDataApi.h
	virtual const char* GetItemName(const void* item) const override
	{
		switch((unsigned char)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			item)
		{
			case REQF_ORDER:
			return "Order";

			case REQF_ORDER_CANCEL:
			return "Cancel";

			case REQF_ORDER_REPLACE:
			return "Replace";

			case REQF_STOCK_SUBSCRIBE:
			return "Subscribe";

			case REQF_HEARTBEAT:
			return "Heartbeat";

			case REQF_LOAD_HISTORY:
			return "AccountHistory";

			case REQF_LOGON:
			return "Logon";

			case REQF_PUBLIC_KEY:
			return "PublicKey";

			case REQF_MS_SUBSCRIBE:
			return "MS Subscribe";

			case REQF_MS_CHART:
			return "TodaysChart";

			case REQF_HIST_CHART:
			return "HistChart";

			case REQF_HIST_PRINT:
			return "HistPrint";

			case REQF_UNDERLIER_SUBSCRIBE:
			return "UnderlierSubscribe";

			case REQF_OPTION_SUBSCRIBE:
			return "OptionSubscribe";

			case REQF_FIRM:
			return "FirmPosition";

			default:
			return "";
		}
	}
};

class ListBoxResponseFilter : public ListBoxSimpleFilter<unsigned int, 32>
{
public:
	ListBoxResponseFilter():ListBoxSimpleFilter((1 << RSPF_Count) - 1, RSPF_Count){}
//Enum	ExtensionRequestFilterEnum in TakionDataApi.h
	virtual const char* GetItemName(const void* item) const override
	{
		switch((unsigned char)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			item)
		{
			case RSPF_LOGON:
			return "Logon";

			case RSPF_MD_SUBSCRIPTION:
			return "MdSubscription";

			case RSPF_MD_QUOTE:
			return "Quote";

			case RSPF_MD_QUOTE_EXEC_HIDDEN:
			return "QuoteExecHidden";

			case RSPF_MD_QUOTE_INDICATORS:
			return "MdQuoteIndicators";

			case RSPF_MD_LEVEL1:
			return "MdLevel1";

			case RSPF_MD_LEVEL2:
			return "MdLevel2";

			case RSPF_MD_PRINT:
			return "MdPrint";

			case RSPF_MD_PRINT_CORRECTION:
			return "MdPrintCorrection";

			case RSPF_MD_IMBALANCE:
			return "MdImbalance";

			case RSPF_MD_RESET:
			return "MdReset";

			case RSPF_MS_SUBSCRIPTION:
			return "MsSubscription";

			case RSPF_MS_LEVEL1:
			return "MsLevel1";

			case RSPF_MS_PRINT:
			return "MsPrint";

			case RSPF_MS_PRINT_CORRECTION:
			return "MsPrintCorrection";

			case RSPF_MS_IMBALANCE:
			return "MsImbalance";

			case RSPF_MS_QUOTE_INDICATORS:
			return "MsQuoteIndicators";

			case RSPF_MS_RESET:
			return "MsReset";

			case RSPF_MS_INDEX:
			return "Index";

			case RSPF_MS_CHART:
			return "Chart";

			case RSPF_MS_CHART_CORRECTION:
			return "ChartCorrection";

			case RSPF_MS_NEW_MINUTE:
			return "MsNewMinute";

			case RSPF_HIST_CHART:
			return "HistChart";

			case RSPF_HIST_PRINT:
			return "HistPrint";

			case RSPF_ORDER:
			return "Order";

			case RSPF_ACCOUNT:
			return "Account";

			case RSPF_FIRM:
			return "Firm";

			case RSPF_HTB:
			return "HTB";

			case RSPF_YESTERDAY_VALUES_CORRECTION:
			return "YesterdayCorrection";

			default:
			return "";
		}
	}
};

class SampleDialogSettingTestRequest : public BaseDialogSetting
{
public:
	SampleDialogSettingTestRequest(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual ~SampleDialogSettingTestRequest();
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;
	virtual void UpdateControls() override;
	virtual void Entering() override;
	virtual void Leaving() override;
	virtual void RepaintTotals() const;
	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	void RepaintLineCount() const;
	void AddMessageReport(unsigned char connectionType, const Message* message, bool outgoing);
	int GetListBoxBorderSize() const{return m_listBoxBorderSize;}
	int GetLeftOffset() const{return m_leftOffset;}
	int GetTopOffset() const{return m_topOffset;}

	virtual void RestoreLayout() override;
	unsigned int GetRequestFilter() const;
	unsigned int GetResponseFilter() const;
protected:
	virtual void ToolTipsEnabled(bool enable) override;

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	virtual void SetupControls() override;

	virtual void DoPaint(const RECT& rcPaint, CDC& dc) override;
	virtual void TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff) override;
	void UpdateTotalRects();//int left, int top);
	virtual void DoUpdateTotalRects();

	UnsignedNumberTypeSpin<unsigned int> m_SpinMessageCount;
	CButton m_CheckBoxSuspended;

	ListBoxRequestFilter m_ListBoxRequestFilter;
	ListBoxResponseFilter m_ListBoxResponseFilter;
	ListBoxMessage m_ListBoxMessage;
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeMessage();

	afx_msg void OnSuspended();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);
	afx_msg LRESULT OnRequestSent(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMessageReceived(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;

	int m_leftOffsetResponse;
	int m_topOffsetResponse;
	int m_widthResponse;

	void UpdateRequestFilter();
//	void RemoveRequestFilter(unsigned char removeFilter);
	unsigned int m_requestFilter;//enum FilterOrderStatus in ConfigEnums.h

	void UpdateResponseFilter();
//	void RemoveResponseFilter(unsigned char removeFilter);
	unsigned int m_responseFilter;//enum FilterOrderStatus in ConfigEnums.h

	void AddLine(unsigned int wparam, unsigned char connectionType, bool outgoing);
	void SetMessageCount(const unsigned int& messageCount);
	unsigned int m_messageCount;
	unsigned int m_messageCountCurrent;

	bool m_suspended;

	void DestroyItemsNotInListBox();
	TakionRingVectorBase* m_itemsNotInListBox;
	CCriticalSection m_itemsNotInListBoxLock;

	CRect m_totalLineCountRect;

	void UpdateModified();
};
