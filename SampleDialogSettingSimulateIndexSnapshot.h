#pragma once

#include "BaseDialogSettingSimulateSymbol.h"
#include "ListBoxSnapshotFlags.h"

enum BaseDialogSettingSimulateIndexSnapshotIds
{
	listBoxSimIndexSnapshotFlagsId = baseSimulateSymbolIdsCount,
	staticSimIndexSnapshotFlagsId,

	editSimIndexSnapshotDescriptionId,
	staticSimIndexSnapshotDescriptionId,

	simulateIndexSnapshotIdsCount
};

class SampleDialogSettingSimulateIndexSnapshot : public BaseDialogSettingSimulateSymbol
{
public:
	SampleDialogSettingSimulateIndexSnapshot(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
/*
#ifndef TAKION_NO_OPTIONS
	virtual bool isOptionModeUsed() const override;
	virtual unsigned char GetValidSource(unsigned char source) const
	{
		return source < ST_Count ? source : ST_Count;
	}
#endif
*/
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

//	ComboBoxNameDescription m_ComboBoxMarketStatus;
	ListBoxSnapshotFlags m_ListBoxSnapshotFlags;
	CEdit m_EditDescription;
/*
	UnsignedNumberTypeSpin<unsigned int> m_SpinRoundLot;
	CButton m_CheckBoxRoundLotsOnly;
	ComboBoxMarketCategory m_ComboBoxMarketCategory;
	ComboBoxPrimaryExchangeDescription m_ComboBoxPrimaryExchange;
	ComboBoxChar m_ComboBoxFinancialStatus;
	ComboBoxChar m_ComboBoxTradingState;
	CEdit m_EditTradingActionReason;
*/
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg void OnSelchangeMarketStatus();
	afx_msg void OnDescriptionChange();
/*
	afx_msg void OnRoundLotsOnly();
	afx_msg void OnSelchangeMarketCategory();
	afx_msg void OnSelchangePrimaryExchange();
	afx_msg void OnSelchangeFinancialStatus();
	afx_msg void OnSelchangeTradingState();
	afx_msg void OnTradingActionReasonChange();
*/
	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);
//	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()
/*
#ifndef TAKION_NO_OPTIONS
	virtual bool OptionModeChanged();
#endif
*/
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	TMsgRespRefreshIndex m_refreshIndex;

	std::string m_description;

	char m_attachment[0xFFFF];
	char* m_attachmentCursor;
	unsigned short m_attachmentLength;
	unsigned short m_remainingLength;

/*
	unsigned char m_marketStatus;
	unsigned char m_marketStatusCount;

	unsigned int m_roundLot;
	bool m_roundLotsOnly;

	unsigned char m_marketCategory;
	char m_primaryExchange;

	char m_financialStatus;
	char m_tradingState;
	unsigned int m_tradingActionReason;
*/
	virtual bool isSendMessageButtonEnabled() const override;
//	virtual void SourceChanged();

	unsigned char FillAttachment();
	void MoveForwardInAttachment(const unsigned short& length)
	{
		m_attachmentLength += length;
		m_attachmentCursor += length;
		m_remainingLength -= length;
	}
};
