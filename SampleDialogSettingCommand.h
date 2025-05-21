#pragma once

#include "BaseDialogSetting.h"

class SampleDialogSettingCommand : public BaseDialogSetting
{
public:
	SampleDialogSettingCommand(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
//	virtual void UpdateControls() override;
//	virtual void Leaving() override;
	static void CleanupCommands();
protected:
	virtual void ToolTipsEnabled(bool enable) override;
//	void UpdateConnections();
//	void UpdateConnection(const Connection* connection, bool connected);
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	
	CButton m_ButtonAddCommand;
	CButton m_ButtonRemoveCommand;
	CButton m_ButtonMapCommand;
	CButton m_ButtonUnmapCommand;

	CheckBoxOwnerDraw m_CheckBoxOverridePreviousMapping;

	CEdit m_EditCommandName;
	ValidEdit m_EditSymbol;
	CButton m_ButtonExecuteCommand;
	CheckBoxOwnerDraw m_CheckBoxExecuteCommandPost;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnAddCommand();
	afx_msg void OnRemoveCommand();
	afx_msg void OnMapCommand();
	afx_msg void OnUnmapCommand();
	afx_msg void OnOverridePreviousMapping();

	afx_msg void OnCommandNameChange();
	afx_msg void OnSymbolChange();
	afx_msg void OnSymbolUpdate();
	afx_msg void OnCommandPost();
	afx_msg void OnCommandExecute();

	DECLARE_MESSAGE_MAP()

	bool m_overridePreviousMapping;

	void EnableExecuteButton();
	bool m_commandPost;
	std::string m_commandNameToExecute;
	std::string m_symbol;
//	unsigned __int64 m_numericSymbol;
};
