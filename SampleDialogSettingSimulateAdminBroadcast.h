#pragma once

#include "BaseDialogSettingSimulateMessage.h"

enum SampleDialogSettingSimulateAdminBroadcastIds
{
	staticTextId = baseSimulateMessageIdsCount,
	editTextId,
	buttonResetId,

	simulateAdminBroadcastIdsCount
};

class SampleDialogSettingSimulateAdminBroadcast : public BaseDialogSettingSimulateMessage
{
public:
	SampleDialogSettingSimulateAdminBroadcast(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual bool isSendMessageButtonEnabled() const{return true;}
protected:
	virtual bool DoReset();
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTextChange();
	afx_msg void OnReset();
	DECLARE_MESSAGE_MAP()

	CEdit m_EditText;
	CButton m_ButtonReset;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;

	void EnableResetButton();

	std::string m_text;
	std::string m_controlText;
//	bool m_textUpdated;
	TMsgText m_msg;
};
