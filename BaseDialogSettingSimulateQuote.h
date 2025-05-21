#pragma once

#include "BaseDialogSettingSimulateOption.h"
#include "IncrementalHexEdit.h"

enum BaseDialogSettingSimulateQuoteIds
{
	editRefId = baseSimulateOptionIdsCount,
	staticRefId,
	comboSideId,
	staticSideId,
	comboBookId,
	staticBookId,

	baseSimulateQuoteIdsCount
};

class BaseDialogSettingSimulateQuote : public BaseDialogSettingSimulateOption
{
public:
	virtual bool isModified() const override{return BaseDialogSettingSimulateOption::isModified() || m_EditRef.isModified();}
	virtual void UpdateSettings() override;
	virtual void UpdateControls() override;
#ifndef TAKION_NO_OPTIONS
	virtual bool isOptionModeUsed() const override;
#endif
protected:
	BaseDialogSettingSimulateQuote(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;

	virtual bool isRefUsed() const{return BOOK_LEVEL2 != m_bookId;}
	IncrementalHexEdit m_EditRef;
	ComboBoxString m_ComboBoxSide;
	ComboBoxStringUnsignedNumber<unsigned char> m_ComboBoxBookId;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRefUpdate();
	afx_msg void OnRefChange();
	afx_msg void OnSelchangeSide();
	afx_msg void OnSelchangeBook();
//	afx_msg LRESULT OnSpinChange(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	virtual bool isSendMessageButtonEnabled() const override;

	virtual void MessageSent() override;
	std::string m_refNumber;

	unsigned char m_side;
	unsigned char m_bookId;
	std::string m_bookName;
};
