#pragma once

#include "BaseDialogSettingSimulateMessage.h"

enum SampleDialogSettingSimulateNewDayIds
{
	buttonShutdownId = baseSimulateMessageIdsCount,
#ifndef TAKION_NO_OPTIONS
	buttonNewFutureSessionId,
#endif
	simulateNewDayIdsCount
};

class SampleDialogSettingSimulateNewDay : public BaseDialogSettingSimulateMessage
{
public:
	SampleDialogSettingSimulateNewDay(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
//	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
//#ifndef TAKION_NO_OPTIONS
//	virtual bool isOptionModeUsed() const override;
//#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

#ifndef TAKION_NO_OPTIONS
	ButtonToolTip m_ButtonNewFutureSession;
	afx_msg void OnNewFutureSession();
#endif
	ButtonToolTip m_ButtonShutdown;

	afx_msg void OnSendMessage();
	afx_msg void OnShutdown();
	DECLARE_MESSAGE_MAP()

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override{return nullptr;}
	virtual bool isSendMessageButtonEnabled() const override;
};
