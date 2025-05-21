#pragma once

#include "BaseDialogSettingAccount.h"

enum BaseSampleSettingSymbolIds
{
	staticSymbolNameId = baseDialogSettingAccountIdsCount,
	editSymbolId,
//	groupAccountId,
//	comboBoxAccountId,

	baseSymbolIdsCount
};
class BaseSampleDialogSettingSymbol : public BaseDialogSettingAccount//, public Observer
{
public:
//	virtual ~BaseSampleDialogSettingSymbol(){ClearStocks();}
	virtual HWND GetFirstTabControl() const override;
	virtual bool AreSettingsValid() const{return !m_symbol.empty();}
	const std::string& GetSymbol() const{return m_symbol;}
protected:
	BaseSampleDialogSettingSymbol(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, bool useAccount, const char* label, int accountWidth, int accountTopOffset = 0);
//	virtual void InitControls() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
//	virtual void BeforeDestroy() override;

	ValidEdit m_EditSymbol;
	std::string m_symbol;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
//	afx_msg void OnSendOrder();
	DECLARE_MESSAGE_MAP()

//	typedef std::hash_map<Security*, MyStock*> PendingStockMap;
//	PendingStockMap m_pendingStockMap;
	virtual void SymbolEmptyChanged(){}

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
