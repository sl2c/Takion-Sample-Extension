#pragma once

#include "BaseDialogSetting.h"

enum HistChartColumnId : unsigned short
{
	HCI_DATE,
	HCI_TIME,
	HCI_START,
	HCI_LOW,
	HCI_HIGH,
	HCI_END,
	HCI_VOLUME,

	HCI_Count
};

class LbHistChartHorizontalScroller : public LbHorizontalScroller
{
public:
	LbHistChartHorizontalScroller(ListBoxOwnerDraw* listBox):LbHorizontalScroller(listBox){}
	virtual void AddColumns() override;
};

class ListBoxHistChart : public ListBoxOwnerDraw
{
public:
	ListBoxHistChart():
		m_volumeLeft(0),
		m_volumeRight(0),
		m_decDigit(2)
	{}
	virtual bool HasItemToolTips() const override{return false;}

	virtual int Compare(const void* item1, const void* item2) const override;
	virtual void DestroyItem(const void* item) const override;
	virtual void* CloneItem(const void* item) const override;
//	virtual void ItemToString(const void* item, std::string& itemStr) const override;
//	virtual void ItemValueToString(const void* item, std::string& itemStr) const override;
//	virtual const char* GetItemName(const void* item) const override;
	void InvalidateVolume(unsigned int sel){InvalidateItem(sel, m_volumeLeft, m_volumeRight);}
//	void InvalidateCount(unsigned int sel){InvalidateItem(sel, m_countLeft, m_countRight);}
	void SetDecDigit(const unsigned char& decDigit)
	{
		if(decDigit != m_decDigit)
		{
			m_decDigit = decDigit;
			if(m_hWnd)
			{
				InvalidateColumnRange(HCI_START, HCI_END);
			}
		}
	}
protected:
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
//	virtual void DoCreateColumnToolTips() override;

	virtual void DoDrawItem(const DRAWITEMSTRUCT* lpDrawItemStruct, const CRect& drawItemRect) override;
	virtual void HorizontalScrollingDone(int diff) override
	{
		ListBoxOwnerDraw::HorizontalScrollingDone(diff);
		m_volumeLeft += diff;
		m_volumeRight += diff;
	}

	virtual void ListBoxCreated() override
	{
		ListBoxOwnerDraw::ListBoxCreated();
		m_horizontalScroller->GetColumnBounds(HCI_VOLUME, m_volumeLeft, m_volumeRight);
	}

//	int m_clientCountLeft;
//	int m_clientCountRight;

	int m_volumeLeft;
	int m_volumeRight;

	unsigned char m_decDigit;
};

enum BaseSettingChartIds
{
	listBoxHistChartId = 10000,

	staticChartSymbolNameId,
	editChartSymbolId,
	staticChartSymbolId,

	spinChartDecDigitId,
	staticChartDecDigitId,

	buttonLoadId,

	chartIdsCount
};

class BaseDialogSettingChart : public BaseDialogSetting, public Observer
{
public:
	BaseDialogSettingChart(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label, int width);
	virtual ~BaseDialogSettingChart();
	virtual HWND GetFirstTabControl() const override;
//	virtual void UpdateSettings() override;
//	virtual void UpdateControls() override;

//	virtual void RestoreLayout() override;
	bool isMarketSorterConnected() const{return m_marketSorterConnected;}
	bool isMarketSorterLoaded() const{return m_marketSorterLoaded;}
//	bool isHistoricalPrintConnected() const{return m_historicalPrintConnected;}

/*
	unsigned int GetFrequency() const{return m_frequency;}
	unsigned char GetControlFrequency() const;
	static char GetStaticFrequencyCode(unsigned int frequency);
	static unsigned int GetStaticFrequencyFromCode(char frequencyCode);
	char GetFrequencyCode() const{return GetStaticFrequencyCode(m_frequency);}
	unsigned int GetDateFrom() const{return m_dateFrom;}
	unsigned int GetControlDateFrom() const;//{return m_dateFrom;}
	bool DoReset();
*/
	void UpdateToday();
	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

	virtual bool DoReset();

	virtual bool CanFill() const{return false;}

	const unsigned char& GetDecDigit() const{return m_decDigit;}
protected:
	virtual void TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff) override;

	ListBoxHistChart m_ListBoxChart;

	ValidEdit m_EditSymbol;
	CStatic m_StaticSymbol;

	UnsignedNumberTypeSpin<unsigned char> m_SpinDecDigit;

	CButton m_ButtonLoad;

	virtual void ToolTipsEnabled(bool enable) override;

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	virtual void UpdateAllCurrentLoadSettings(){}
	void UpdateCurrentLoadSettingsAndSymbol();

	virtual bool canLoad() const;
	virtual bool CanLoad() const;
	virtual bool SymbolCanChange() const{return true;}
	virtual void EnableLoadButton();
	virtual bool ShouldReloadOnNewDay() const{return false;}//m_dayCountMode)

	virtual void OnMarketSorterLoaded(){}
	virtual const Security::ChartPointVector* GetChartVector() const{return nullptr;}
	void FillListBox();
	virtual void DoFillListBox(const Security::ChartPointVector& chartPointVector);
//	virtual void AddChartPoint(const ChartPoint& chartPoint);

/*
	bool CanReset() const;
	bool CanLoad() const;
	void EnableLoadButton();
*/
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnLoad();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);

/*
	afx_msg void OnReset();
	afx_msg void OnLoad();
	afx_msg void OnSaveAll();
	afx_msg void OnOpenAll();

	afx_msg void OnDayCountMode();

	afx_msg BOOL OnCalendarDropdown(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnCalendarCloseup(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDateChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeFrequency();
	LRESULT OnSpinChange(WPARAM w, LPARAM l);
*/
	DECLARE_MESSAGE_MAP()

	virtual bool DoLoad() = 0;
/*
	bool DoLoad();

	void UpdateDayCountMode();
	void UpdateCurrentLoadSettings();
	void UpdateCurrentLoadSettingsAndSymbol();
*/
	unsigned int m_today;

	virtual void NewSecurity(const Security* security){}

	std::string m_symbol;
	std::string m_symbolInitial;

	unsigned char m_decDigit;
/*
	unsigned int m_dayCount;
	unsigned char m_frequency;
	unsigned int m_dateFrom;

	unsigned int m_dayCountInitial;
	unsigned char m_frequencyInitial;
	unsigned int m_dateFromInitial;

	bool m_dayCountMode;
	bool m_dayCountModeInitial;

	void FillListBox();
	HistoricalChart* m_historicalChart;
*/
	void SetMarketSorterConnected(bool connected);
	void SetMarketSorterLoaded(bool loaded);
	virtual void UpdateConnection()
	{
		SetMarketSorterConnected(TD_IsMarketSorterConnected());
	}
//	void SetHistoricalPrintConnected(bool connected);
	bool m_marketSorterConnected;
	bool m_marketSorterLoaded;
//	bool m_historicalPrintConnected;
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;

	unsigned __int64 m_totalVolume;

	int m_listBoxBorderSize2;
	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;

	int m_width;
	int m_firstColumnBottom;
};