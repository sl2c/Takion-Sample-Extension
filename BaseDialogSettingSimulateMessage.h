#pragma once

#include "BaseDialogSetting.h"

enum BaseDialogSettingSimulateMessageIds
{
	buttonSendMessageId = 10000,
	baseSimulateMessageIdsCount
};

class BaseDialogSettingSimulateMessage : public BaseDialogSetting
{
public:
	virtual ~BaseDialogSettingSimulateMessage(){ReleaseMessageSimulationThread();}
//	virtual HWND GetFirstTabControl() const;
	virtual void UpdateSettings() override;
	virtual void UpdateControls() override;
//	virtual void Leaving();

	static const int defaultSendButtonWidth = 56;

	static void FillCharRange(ComboBoxChar& comboBox);
	static void SetUnsignedCharInComboBox(ComboBoxChar& comboBox, const unsigned char& val, unsigned char& valRef);
	static void SetCharInComboBox(ComboBoxChar& comboBox, const char& val, unsigned char& valRef);
protected:
	BaseDialogSettingSimulateMessage(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label, int sendButtonWidth = defaultSendButtonWidth);
//	virtual void InitControls();
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;

	virtual bool isSendMessageButtonEnabled() const{return false;}
	virtual void Level2Changed(){}

//	void EnableSendControl(HWND hwnd){EnableControl(m_hWnd, isSendMessageButtonEnabled(), GetFirstTabControl());}
	virtual void EnableSendMessageAdditional(bool enable, HWND focusControl){}
	void EnableSendMessageButton()
	{
		const bool enable = isSendMessageButtonEnabled();
		const HWND focusControl = GetFirstTabControl();
		EnableControl(m_ButtonSendMessage.m_hWnd, enable, focusControl);
//		EnableSendControl(m_ButtonSendMessage.m_hWnd);
		EnableSendMessageAdditional(enable, focusControl);
	}
	ButtonToolTip m_ButtonSendMessage;

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSendMessage();
	DECLARE_MESSAGE_MAP()

	bool OnSelchangeUChar(ComboBoxChar& comboBox, unsigned char& saleConditionVal);

	int m_bottomControl;
	int m_xc;
	int m_maxControlWidth;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) = 0;
	virtual void MessageSent(){}
	void ReleaseMessageSimulationThread();
	MessageThread* m_messageSimulationThread;

	static void InitComboBoxNameDescription(ComboBoxNameDescription& comboBox, char& c);
	void OnSelchangeNameDescription(ComboBoxNameDescription& comboBox, char& c, const char& defaultChar);

	static void InitComboBoxChar(ComboBoxChar& comboBox, char& c);
	void OnSelchangeChar(ComboBoxChar& comboBox, char& c);

	int m_firstColumnWidth;
	int m_firstColumnHalfWidth;

	int m_sendButtonWidth;
};
