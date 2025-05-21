#pragma once

#include "BaseDialogSettingSimulateMessage.h"

enum SampleSettingSimulateMarketStatusIds
{
	comboBoxMarketStatusId = baseSimulateMessageIdsCount,
	staticMarketStatusId,
	checkBoxSymbolMappingId,

	simulateMarketStatusIdsCount
};

class SampleDialogSettingSimulateMarketStatus : public BaseDialogSettingSimulateMessage
{
public:
	SampleDialogSettingSimulateMarketStatus(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual bool isSendMessageButtonEnabled() const{return m_marketStatus && m_marketStatus != 'U';}
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	afx_msg void OnSelchangeMarketStatus();
	afx_msg void OnSymbolMapping();

	DECLARE_MESSAGE_MAP()

	ComboBoxNameDescription m_ComboBoxMarketStatus;
	CButton m_CheckBoxSymbolMapping;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;

	char m_marketStatus;
	bool m_symbolMapping;

	TMsgSystemEvent m_msg;
	TSmMsgSystemEvent m_msgSymbolMapping;
};
