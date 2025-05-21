#pragma once

#include "BaseDialogSettingChart.h"

class SampleDialogSettingTodayChart;

class UnappliedChartMessageReader : public LineBufferArrayReader
{
public:
	UnappliedChartMessageReader(SampleDialogSettingTodayChart* window, const LineBufferArray* bufferArray = nullptr):LineBufferArrayReader(bufferArray), m_window(window){}
	virtual bool ProcessDataToken(const char* cursor, unsigned short length) const;
protected:
	SampleDialogSettingTodayChart* m_window;
};

class SampleDialogSettingTodayChart : public BaseDialogSettingChart
{
public:
	SampleDialogSettingTodayChart(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
//	virtual ~SampleDialogSettingTodayChart();
	virtual void UpdateSettings() override;

	virtual void RestoreLayout() override;
	virtual bool CanFill() const override;
	bool ProcessUnappliedMessage(const Message* message);
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
//	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	virtual bool DoLoad() override;
	virtual const Security::ChartPointVector* GetChartVector() const{return m_security ? m_security->GetChart() : nullptr;}
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;

	void AddPrint(const Price& price, unsigned int size, char marketCenterId, unsigned char printFlags, bool priceApplicableToChart);//printFlags: 4 - price; 8 - volume, 16 - high/low
	void PropagateTodaysChartToMinute(const unsigned short minute);

	virtual void NewSecurity(const Security* security)
	{
		if(!strcmp(security->GetSymbol(), m_symbolInitial.c_str()))
		{
			SetSecurity(security);
		}
	}

	void SetSecurity(const Security* security);
	const Security* m_security;
	UnappliedChartMessageReader m_unappliedMessageReader;

	unsigned short m_currentMinute;
	unsigned short m_chartMinute;
};