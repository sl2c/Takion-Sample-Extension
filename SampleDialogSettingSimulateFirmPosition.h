#pragma once

#include "BaseDialogSettingSimulateAccountSymbol.h"

enum SampleSettingSimulateFirmPositionIds
{
	spinFirmPositionSizeId = baseSimulateAccountSymbolIdsCount,
	staticFirmPositionSizeId,

	staticFirmPositionPendingSellPassiveId,
	spinFirmPositionPendingSellPassiveId,

	staticFirmPositionPendingSellNonPassiveId,
	spinFirmPositionPendingSellNonPassiveId,

	staticFirmPositionPendingShortPassiveId,
	spinFirmPositionPendingShortPassiveId,

	staticFirmPositionPendingShortNonPassiveId,
	spinFirmPositionPendingShortNonPassiveId,

	simulateFirmPositionIdsCount
};

class SampleDialogSettingSimulateFirmPosition : public BaseDialogSettingSimulateAccountSymbol
{
public:
	SampleDialogSettingSimulateFirmPosition(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
//	static void SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
//	static void SetSignedSizeSpinCells(SignedNumberTypeSpin<int>& spin);
	virtual void UpdateSettings() override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);

	DECLARE_MESSAGE_MAP()

	SignedNumberTypeSpin<int> m_SpinSize;
	UnsignedNumberTypeSpin<unsigned int> m_SpinPendingSellPassive;
	UnsignedNumberTypeSpin<unsigned int> m_SpinPendingSellNonPassive;
	UnsignedNumberTypeSpin<unsigned int> m_SpinPendingShortPassive;
	UnsignedNumberTypeSpin<unsigned int> m_SpinPendingShortNonPassive;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;
	TMsgUpdateFirmPosition m_msg;
};
