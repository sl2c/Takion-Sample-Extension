#pragma once

#include "BaseDialogSettingSimulateOption.h"

enum BaseDialogSettingSimulateOptionSourceIds
{
	comboBoxOptionSourceId = baseSimulateOptionIdsCount,
	staticOptionSourceId,

	baseSimulateOptionSourceIdsCount
};

class BaseDialogSettingSimulateOptionSource : public BaseDialogSettingSimulateOption
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
	BaseDialogSettingSimulateOptionSource(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	ComboBoxString m_ComboBoxSource;

	afx_msg void OnSelchangeSource();
	DECLARE_MESSAGE_MAP()

#ifndef TAKION_NO_OPTIONS
	virtual bool OptionModeChanged();
#endif
	unsigned char m_source;
	virtual void SourceChanged(){}
};
