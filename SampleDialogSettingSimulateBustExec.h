#pragma once

#include "BaseDialogSettingSimulateAccountOption.h"

#define SEND_BUST_EXEC_TO_ALL_ACCOUNTS

#if defined(T_RELEASED) || defined(_DEBUG)
//#define MUTIPLE_BORROWS
#endif

enum SampleDialogSettingSimulateBustExecIds
{
	editBustExecRefId = baseSimulateAccountOptionIdsCount,
	checkBoxBustExecRefId,

	spinBustExecSizeId,
	staticBustExecSizeId,

	spinBustExecPriceId,
	staticBustExecPriceId,

	comboBoxBustExecSideId,
	staticBustExecSideId,

	checkBoxBustExecBustedId,
	checkBoxBustExecDropcopyId,

//

	spinBustExecOrderSizeId,
	staticBustExecOrderSizeId,

	staticBustExecDestinationId,
	comboBoxBustExecDestinationId,

	editBustExecMmidId,
	staticBustExecMmidId,

	editBustExecLiquidityId,
	staticBustExecLiquidityId,

#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	buttonBustExecSendMessageToAllAccountsId,
	spinBustExecSendMessageToAllAccountsDelayId,
	staticBustExecSendMessageToAllAccountsDelayId,
#endif

	simulateBustExecIdsCount
};

class SampleDialogSettingSimulateBustExec : public BaseDialogSettingSimulateAccountOption
{
public:
	SampleDialogSettingSimulateBustExec(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);

	virtual void UpdateSettings() override;
//	static void SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
//	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
#ifndef TAKION_NO_OPTIONS
	virtual bool isOptionModeUsed() const;
#endif
	virtual bool isSymbolUsed() const{return !m_ref;}

#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
	virtual bool isSendMessageButtonEnabled() const override;
#endif
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
	virtual void InitControls() override;
	virtual void ToolTipsEnabled(bool enable) override;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg void OnRefUpdate();
	afx_msg void OnRefChange();
	afx_msg void OnRef();
	afx_msg void OnBusted();
	afx_msg void OnDropcopy();
	afx_msg void OnSelchangeSide();
	afx_msg void OnSelchangeDestination();
	afx_msg void OnMmidUpdate();
	afx_msg void OnMmidChange();
	afx_msg void OnLiquidityUpdate();
	afx_msg void OnLiquidityChange();

	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);

	DECLARE_MESSAGE_MAP()

	CEdit m_EditRef;
	CButton m_CheckBoxRef;

	CButton m_CheckBoxBusted;

	CButton m_CheckBoxDropcopy;

	void UpdateSpinColor();
	void UpdateOrderAndExecutionSizeSpinColor();
	void UpdateExecutionPriceSpinColor();
	UnsignedNumberTypeSpin<unsigned int> m_SpinSize;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPrice;

	ComboBoxString m_ComboBoxSide;

	void UpdateOrderSizeSpinColor();
	UnsignedNumberTypeSpin<unsigned int> m_SpinOrderSize;
	ComboBoxDestination m_ComboBoxDestination;
	ValidEdit m_EditMmid;
	ValidEdit m_EditLiquidity;

	void ResetValues();

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;

	const Destination* m_currentDestination;

	std::string m_refNumber;
	bool m_ref;
	bool m_busted;
	bool m_dropcopy;
	char m_side;
	unsigned int m_size;
	unsigned int m_orderSize;
	Price m_price;
//	std::string m_mmid;
	char m_mmid[sizeof(unsigned int) + 1];
	char m_liquidity[sizeof(unsigned int) + 1];

	TMsgOrderReport m_msg;

#ifdef MUTIPLE_BORROWS
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
#endif

#ifdef SEND_BUST_EXEC_TO_ALL_ACCOUNTS
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
