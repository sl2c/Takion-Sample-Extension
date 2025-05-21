#pragma once

#include "BaseDialogSetting.h"

class SampleDialogSettingConnection : public BaseDialogSetting, public Observer
{
public:
	SampleDialogSettingConnection(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;
	virtual void UpdateControls() override;
	virtual void Leaving() override;
protected:
	void UpdateConnections();
	void UpdateConnection(const Connection* connection, bool connected);
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	
	ListBoxConnection m_ListBoxConnection;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeConnection();
	DECLARE_MESSAGE_MAP()

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
