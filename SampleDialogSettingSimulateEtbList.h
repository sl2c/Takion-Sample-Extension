#pragma once

#include "BaseDialogSettingSimulateFirm.h"

enum SampleSettingSimulateEtbListIds
{
	listBoxFlagsId = baseSimulateFirmIdsCount,
	checkBoxDoneId,
//	staticFlagsId,

	simulateEtbListIdsCount
};

class SampleDialogSettingSimulateEtbList : public BaseDialogSettingSimulateFirm
{
public:
	SampleDialogSettingSimulateEtbList(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;

//	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
//	static void SetMoneySpinCells(SignedNumberTypeSpin<__int64>& spin);

	virtual StringSetting* GetSettingFirm() override;
	virtual UIntSetting* GetSettingRequestId() override;
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

	virtual bool isSendMessageButtonEnabled() const override;

	afx_msg void OnDone();

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	ListBoxFilterString<unsigned int> m_ListBoxFlags;
	CheckBoxOwnerDraw m_CheckBoxDone;

//	virtual void FillFirmInfo() override;

	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;
	virtual void RestoreLayout() override;

	bool m_done;

	TMsgRespEtbSymbolList m_msg;
	EtbNode m_etbNode;
/*	
	TMsgRespLocateInfo m_msg;
	BorrowNode m_borrowNode;

	TMsgLocateInfo m_msgOnTheFlyBorrow;
	TMsgBetaInfo  m_msgOnTheFlyBetaAndMargin;

	TMsgRespHtbSymbolList m_msgHtb;
	HtbNode m_htbNode;
*/
};
