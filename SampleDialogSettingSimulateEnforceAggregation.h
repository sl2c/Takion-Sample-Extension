#pragma once

#include "BaseDialogSettingSimulateMessage.h"

enum SampleSettingSimulateEnforceAggregationIds
{
	comboBoxClearingFirmId = baseSimulateMessageIdsCount,
	groupClearingFirmId,
	checkBoxEnforceId,
	checkBoxConservativeOrderMarkingId,
	checkBoxLocalSideMarkingId,

	spinEnforceNiteBetaTimeId,
	staticEnforceNiteBetaTimeId,

	spinEnforceNiteBetaBpId,
	staticEnforceNiteBetaBpId,

	buttonPasteClearingFirmId,

	simulateEnforceAggregationIdsCount
};

class SampleDialogSettingSimulateEnforceAggregation : public BaseDialogSettingSimulateMessage, public Observer
{
public:
	SampleDialogSettingSimulateEnforceAggregation(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;

	bool isHistoryLoaded() const{return m_historyLoaded;}
	bool isExecutorConnected() const{return m_executorConnected;}
	virtual HWND GetFirstTabControl() const{return m_ComboBoxClearingFirm.m_hWnd;}
	virtual bool isSendMessageButtonEnabled() const;
//	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
//	static void SetMoneySpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);

	bool isPasteEnabled() const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;

	afx_msg void OnEnforceAggregation();
	afx_msg void OnConservativeOrderMarking();
	afx_msg void OnLocalSideMarking();

	afx_msg void OnSelchangeClearingFirm();
	afx_msg void OnPasteCleringFirm();
	LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()
////
	ComboBoxClearingFirm m_ComboBoxClearingFirm;
	CButton m_CheckBoxEnforce;
	CButton m_CheckBoxConservativeOrderMarking;
	CButton m_CheckBoxLocalSideMarking;
	TimeSpin m_SpinEnforceNiteBetaTime;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinNiteBetaBp;
	ButtonToolTip m_ButtonPaste;

	void UpdateComboBoxClearingFirm();
	void SetClearingFirm(ClearingFirm* clearingFirm);
	void AddClearingFirm(ClearingFirm* clearingFirm);
	void SetHistoryLoaded(const bool loaded);
	void SetExecutorConnected(const bool connected);

	ClearingFirm* m_clearingFirm;
	AccountId m_clearingFirmId;
//	char m_accountId[MAX_ACCOUNT_SIZE];
	int m_clearingFirmSel;

	bool m_historyLoaded;
	bool m_executorConnected;

	unsigned int m_enforceNiteBetaTimeSeconds;
	Money m_niteBetaBp;

	bool m_enforceAggregation;
	bool m_conservativeOrderMarking;
	bool m_localSideMarking;

	void EnablePasteButton();

	void OnCheckBox(const CButton& checkBox, bool& value);
	void SetBooleanValueToCheckBox(bool& value, CButton& checkBox, const bool boolValue);
	void SetSecondsToTimeSpin(unsigned int& value, TimeSpin& timeSpin, const unsigned int uintValue);
	void SetMoneyToSpin(Money& money, UnsignedNumberTypeSpin<unsigned __int64>& spin, const Money& moneyValue);

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
////
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;
	TMsgFirmEnforcement m_msg;
};
