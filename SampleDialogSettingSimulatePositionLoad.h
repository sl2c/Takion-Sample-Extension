#pragma once

#include "BaseDialogSettingSimulateAccountOption.h"

#define SEND_LOAD_POSITION_TO_ALL_ACCOUNTS

enum SampleSettingSimulatePositionLoadIds
{
//	spinSimQuotePriceId = baseSimulatePositionIdsCount,
	spinPosLoadSizeId = baseSimulateAccountOptionIdsCount,
	staticPosLoadSizeId,

	spinPosLoadPriceId,
	staticPosLoadPriceId,

	spinPosLoadCostId,
	staticPosLoadCostId,

	spinPosLoadOvernightSizeId,
	staticPosLoadOvernightSizeId,

	spinPosLoadOvernightPriceId,
	staticPosLoadOvernightPriceId,

	spinPosLoadOpenOvernightSizeId,
	staticPosLoadOpenOvernightSizeId,

	spinPosLoadClsLongSizeId,
	staticPosLoadClsLongSizeId,

	spinPosLoadClsLongCostId,
	staticPosLoadClsLongCostId,

	spinPosLoadClsLongValueId,
	staticPosLoadClsLongValueId,

	spinPosLoadClsShortSizeId,
	staticPosLoadClsShortSizeId,

	spinPosLoadClsShortCostId,
	staticPosLoadClsShortCostId,

	spinPosLoadClsShortValueId,
	staticPosLoadClsShortValueId,

	spinPosLoadBorrowSizeId,
	staticPosLoadBorrowSizeId,

	spinPosLoadBorrowPriceId,
	staticPosLoadBorrowPriceId,

#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	buttonPosLoadSendMessageToAllAccountsId,
	spinPosLoadSendMessageToAllAccountsDelayId,
	staticPosLoadSendMessageToAllAccountsDelayId,
#endif
	simulatePositionLoadIdsCount
};

class SampleDialogSettingSimulatePositionLoad : public BaseDialogSettingSimulateAccountOption
{
public:
	SampleDialogSettingSimulatePositionLoad(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
//	static void SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
//	static void SetSignedSizeSpinCells(SignedNumberTypeSpin<int>& spin);
//	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
//	static void SetMoneySpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	virtual bool isSendMessageButtonEnabled() const override;
#endif
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	SignedNumberTypeSpin<int> m_SpinSize;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPrice;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinCost;

	SignedNumberTypeSpin<int> m_SpinOvernightSize;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinOvernightPrice;

	SignedNumberTypeSpin<int> m_SpinOpenOvernightSize;

	UnsignedNumberTypeSpin<unsigned int> m_SpinClsLongSize;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinClsLongCost;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinClsLongValue;

	UnsignedNumberTypeSpin<unsigned int> m_SpinClsShortSize;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinClsShortCost;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinClsShortValue;

	UnsignedNumberTypeSpin<unsigned int> m_SpinBorrowSize;
	UnsignedNumberTypeSpin<unsigned int> m_SpinBorrowPrice;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;
	TMsgPosition m_msg;

#ifdef SEND_LOAD_POSITION_TO_ALL_ACCOUNTS
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSendMessageToAllAccounts();
	ButtonToolTip m_ButtonSendMessageToAllAccounts;
	UnsignedNumberTypeSpin<unsigned int> m_SpinSendMessageToAllAccountsDelay;
	virtual void EnableSendMessageAdditional(bool enable, HWND focusControl) override
	{
		EnableControl(m_ButtonSendMessageToAllAccounts.m_hWnd, enable, focusControl);
	}

	void SendMessageToNextAccount();
	bool SendMessageToAccountAt(unsigned int i);
	char m_accountIdBeingSent[MAX_ACCOUNT_SIZE];
	unsigned int m_millisecondDelay;
#endif
};