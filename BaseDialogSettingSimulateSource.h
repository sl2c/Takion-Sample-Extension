#pragma once

#include "BaseDialogSettingSimulateSymbolMap.h"

enum BaseDialogSettingSimulateSourceIds
{
	comboBoxSourceId = baseSimulateSymbolMapIdsCount,
	staticSourceId,

	baseSimulateSourceIdsCount
};

class BaseDialogSettingSimulateSource : public BaseDialogSettingSimulateSymbolMap
{
public:
	enum SourceType : unsigned char
	{
		ST_MARKET_DATA,
		ST_MARKET_SORTER,

		ST_Count
	};
	virtual unsigned char GetValidSource(unsigned char source) const
	{
		return source < ST_Count ? source : ST_Count - 1;
	}
protected:
	BaseDialogSettingSimulateSource(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	ComboBoxString m_ComboBoxSource;

	afx_msg void OnSelchangeSource();
	DECLARE_MESSAGE_MAP()

	unsigned char m_source;
};
