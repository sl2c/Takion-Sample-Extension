#pragma once

#include "BaseDialogSettingSimulateSymbolMap.h"//"BaseDialogSettingSimulateSource.h"
#include "ListBoxSnapshotFlags.h"

enum SampleSettingSimulateChartSnapshotIds
{
	spinSimChartSnapshotRequestIdId = baseSimulateSymbolMapIdsCount,//baseSimulateSymbolIdsCount,
	staticSimChartSnapshotRequestIdId,

	spinSimChartSnapshotPointCountId,
	staticSimChartSnapshotPointCountId,

	spinSimChartSnapshotStartMinuteId,
	staticSimChartSnapshotStartMinuteId,

	spinSimChartSnapshotPriceStartId,
	staticSimChartSnapshotPriceStartId,

	spinSimChartSnapshotPriceHighId,
	staticSimChartSnapshotPriceHighId,

	spinSimChartSnapshotPriceLowId,
	staticSimChartSnapshotPriceLowId,

	spinSimChartSnapshotPriceEndId,
	staticSimChartSnapshotPriceEndId,

	spinSimChartSnapshotMoneyTradedId,
	staticSimChartSnapshotMoneyTradedId,

	spinSimChartSnapshotVolumeId,
	staticSimChartSnapshotVolumeId,

	checkBoxSimChartSnapshotDoneId,

	simulateChartSnapshotIdsCount
};

class SampleDialogSettingSimulateChartSnapshot : public BaseDialogSettingSimulateSymbolMap
{
public:
	SampleDialogSettingSimulateChartSnapshot(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual ~SampleDialogSettingSimulateChartSnapshot();
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
//	static void SetBigSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
//	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	UnsignedNumberTypeSpin<unsigned int> m_SpinRequestId;

	UnsignedNumberTypeSpin<unsigned short> m_SpinPointCount;

	MinuteSpin m_SpinStartMinute;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceStart;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceHigh;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceLow;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPriceEnd;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinMoneyTraded;

	UnsignedNumberTypeSpin<unsigned int> m_SpinVolume;

	CButton m_CheckBoxDone;

//	ListBoxSnapshotFlags m_ListBoxSnapshotFlags;

	afx_msg void OnDone();
	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

//	void AddPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);


	Price m_priceStart;
	Price m_priceHigh;
	Price m_priceLow;
	Price m_priceEnd;
	Price m_moneyTraded;

	unsigned int m_requestId;

	unsigned short m_pointCount;
	unsigned short m_startMinute;

	void SetDone(const bool done);
	unsigned int m_volume;

	bool m_done;

	TMsgRespChart m_msgStock;
	TSmMsgRespChart m_msgSmStock;
	MsgChartPoint m_chartPointStock;
	unsigned short m_maxStockPointCount;

#ifndef TAKION_NO_OPTIONS
	TMsgRespFutChart m_msgFuture;
	MsgFutChartPoint m_chartPointFuture;
	unsigned short m_maxFuturePointCount;
#endif
//	std::string m_buffer;
	char* m_buffer;// [0xFFFF];
};

