#pragma once

#include "BaseDialogSettingSimulateSource.h"

enum BaseDialogSettingSimulateStockTypeIds
{
	staticSimStockTypeId = baseSimulateSourceIdsCount,
	comboBoxSimStockTypeId,

	simulateStockTypeIdsCount
};

class SampleDialogSettingSimulateStockType : public BaseDialogSettingSimulateSource
{
public:
	SampleDialogSettingSimulateStockType(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	ComboBoxNameDescription m_ComboBoxSecurityType;

	afx_msg void OnSelchangeSecurityType();
	DECLARE_MESSAGE_MAP()

//	virtual void SymbolTypeChanged() override;
	virtual const Message* CreateMessage(WSABUF& wsaBuf) override;

	TMsgSecurityType m_securityTypeMd;
	TSmMsgSecurityType m_securityTypeMdSm;
	TMsgMsSecurityType m_securityTypeMs;
	TSmMsgMsSecurityType m_securityTypeMsSm;

	char m_securityType;
};

