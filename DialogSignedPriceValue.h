#pragma once

#include "BaseDialogValue.h"

class DialogSignedPriceValue : public BaseDialogValue
{
public:
	DialogSignedPriceValue(TakionMainWnd* mainWnd, bool toDestroyOnFocus = false, TakionFrame* receipient = nullptr, CWnd* parent = nullptr);
	void Display(TakionFrame* receipient, unsigned int code, const SignedPrice& value, CWnd* parent, const CPoint& mousePoint);
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void DoApply() override;
	DECLARE_MESSAGE_MAP()
};

