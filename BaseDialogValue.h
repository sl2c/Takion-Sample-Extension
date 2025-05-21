#pragma once

class BaseDialogValue : public DialogValueReset
{
public:
	void Display(TakionFrame* receipient, unsigned int code, const SignedPrice& value, CWnd* parent, const CPoint& mousePoint, int w, int h);
	const SignedPrice& GetValue() const{return m_currentValue;}

	virtual HWND GetFirstTabControl() const override{return m_SpinValue.m_hWnd;}

	enum BaseDialogValueIds
	{
		staticValueId = 1000,
		spinValueId,

		dialogvalueIdCount
	};
	virtual bool DoReset() override;
	virtual bool isChanged() const override;
protected:
	BaseDialogValue(TakionMainWnd* mainWnd, CWnd* rightmostControl, bool toDestroyOnFocus = false, TakionFrame* receipient = nullptr, CWnd* parent = nullptr);
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void DoApply() override;
//	virtual BOOL OnInitDialog();
	virtual void DoEnableOkButton(bool enable) override;
/*
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSymbol1Update();
	afx_msg void OnSymbol1Change();
	afx_msg void OnSymbol2Update();
	afx_msg void OnSymbol2Change();
*/
	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	DECLARE_MESSAGE_MAP()

	unsigned int m_decDigits;
	unsigned int m_multiplier;
	unsigned int m_divider;
/*
	CStatic m_StaticStock1;
	ValidEdit m_EditStock1;
	CStatic m_StaticStock2;
	ValidEdit m_EditStock2;
*/
	CStatic m_StaticValue;
	SignedNumberTypeSpin<int> m_SpinValue;
/*
	std::string m_symbol1;
	std::string m_symbol2;
	std::string m_currentSymbol1;
	std::string m_currentSymbol2;
*/
	SignedPrice m_value;
	SignedPrice m_currentValue;

	static const int staticWidth = 28;
	static const int spinWidth = 60;
	static const int spinHeight = 12;
};

