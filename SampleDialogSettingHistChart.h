#pragma once

#include "BaseDialogSettingChart.h"

#include <fstream>

class SampleDialogSettingHistChart : public BaseDialogSettingChart
{
public:
	SampleDialogSettingHistChart(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual ~SampleDialogSettingHistChart();
//	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;

	virtual void RestoreLayout() override;
	const bool isMarketSorterConnected() const{return m_marketSorterConnected;}
	bool isMarketSorterLoaded() const{return m_marketSorterLoaded;}
	bool isHistoricalPrintConnected() const{return m_historicalPrintConnected;}

	unsigned short GetNodeMinuteCount() const
	{
		switch(GetStaticFrequencyCode(m_frequency))
		{
			case CHF_HOUR:
			return 60;

			case CHF_30MIN:
			return 30;

			case CHF_15MIN:
			return 15;

			case CHF_5MIN:
			return 5;

			case CHF_1MIN:
			return 1;

			default:
			return 0;
		}
	}
	const unsigned char& GetFrequency() const{return m_frequency;}
	unsigned char GetControlFrequency() const;
	static char GetStaticFrequencyCode(unsigned int frequency);
	static unsigned int GetStaticFrequencyFromCode(char frequencyCode);
	char GetFrequencyCode() const{return GetStaticFrequencyCode(m_frequency);}
	unsigned int GetDateFrom() const{return m_dateFrom;}
	unsigned int GetControlDateFrom() const;//{return m_dateFrom;}
//	void UpdateToday();
	virtual bool DoReset();
//	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
//	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;
	virtual bool CanFill() const override;
	void NextDayStarted();
protected:
//	virtual void TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff) override;
	virtual void ToolTipsEnabled(bool enable) override;

//	ListBoxHistChart m_ListBoxChart;

//	ValidEdit m_EditSymbol;
//	CStatic m_StaticSymbol;

	UnsignedNumberTypeSpin<unsigned int> m_SpinDayCount;
	CStatic m_StaticDayCount;

	RadioGroup m_RadioGroupChartDuration;
	RadioOwnerDraw m_RadioDayCount;
	RadioOwnerDraw m_RadioDateFrom;

	ColorDateTimeCtrl m_DateFrom;
	CStatic m_StaticDateFrom;

	ComboBoxStringCurrentItem m_ComboBoxFrequency;
	CStatic m_StaticFrequency;

	CButton m_ButtonReset;
	CButton m_ButtonSaveAll;
	CButton m_ButtonOpenAll;

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
//	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

//	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	bool CanReset() const;
	virtual bool canLoad() const;
	virtual bool CanLoad() const override;
	virtual bool SymbolCanChange() const override{return !m_historicalChartLoadingToSave;}
	virtual bool ShouldReloadOnNewDay() const override{return m_dayCountMode;}

	virtual void OnMarketSorterLoaded() override
	{
		UpdateSaveAllEnabled();
	}

	virtual void EnableLoadButton() override;

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg void OnSymbolUpdate();
//	afx_msg void OnSymbolChange();
	afx_msg void OnReset();
	afx_msg void OnSaveAll();
	afx_msg void OnOpenAll();

	afx_msg void OnDayCountMode();

	afx_msg BOOL OnCalendarDropdown(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnCalendarCloseup(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDateChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeFrequency();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);

	DECLARE_MESSAGE_MAP()

	virtual bool DoLoad();

	void UpdateDayCountMode();
	void UpdateCurrentLoadSettings();

	virtual void UpdateAllCurrentLoadSettings() override
	{
		UpdateDayCountMode();
		UpdateCurrentLoadSettings();
	}
/*
	unsigned int m_today;

	std::string m_symbol;
	std::string m_symbolInitial;
*/
	unsigned int m_dayCount;
	unsigned char m_frequency;
	unsigned int m_dateFrom;

	unsigned int m_dayCountInitial;
	unsigned char m_frequencyInitial;
	unsigned int m_dateFromInitial;

	bool m_dayCountMode;
	bool m_dayCountModeInitial;

//	virtual void AddChartPoint(const ChartPoint& chartPoint) override;
//	void FillListBox();
	virtual void DoFillListBox(const Security::ChartPointVector& chartPointVector) override;

	virtual const Security::ChartPointVector* GetChartVector() const{return m_historicalChart ? &m_historicalChart->GetHistoricalChart() : nullptr;}
	HistoricalChart* m_historicalChart;

//	void SetMarketSorterConnected(bool connected);
//	void SetMarketSorterLoaded(bool loaded);
	void SetHistoricalPrintConnected(bool connected);
	virtual void UpdateConnection()
	{
		BaseDialogSettingChart::UpdateConnection();
		SetHistoricalPrintConnected(TD_IsHistoricalPrintConnected());
	}
//	bool m_marketSorterConnected;
//	bool m_marketSorterLoaded;
	bool m_historicalPrintConnected;
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
/*
	unsigned __int64 m_totalVolume;

	int m_listBoxBorderSize2;
	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;
*/
	bool CanSaveAll() const;
	void UpdateSaveAllEnabled();
	bool CanOpenAll() const;
	void UpdateOpenAllEnabled();
	void EnableAllControls(bool enable);
	void LoadNextChart();
	void SaveCurrentHistoricalChartLoading();
	std::string m_allChartFileName;
	std::ofstream m_allChartFile;

	typedef std::set<std::string> StringSet;
	StringSet m_symbolSet;
	HistoricalChart* m_historicalChartLoadingToSave;
	unsigned int m_dateFromLoadingToSave;
};