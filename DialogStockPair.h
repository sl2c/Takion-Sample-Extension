#pragma once

#include "BaseDialogValue.h"

class DialogStockPair : public BaseDialogValue
{
public:
	DialogStockPair(TakionMainWnd* mainWnd, bool toDestroyOnFocus = false, TakionFrame* receipient = nullptr, CWnd* parent = nullptr);
	void Display(TakionFrame* receipient, unsigned int code, const char* symbol1, const char* symbol2, const SignedPrice& value, CWnd* parent, const CPoint& mousePoint);
//	const SignedPrice& GetValue() const{return m_currentValue;}
	const std::string& GetSymbol1() const{return m_currentSymbol1;}
	const std::string& GetSymbol2() const{return m_currentSymbol2;}

	virtual HWND GetFirstTabControl() const override{return m_EditStock1.m_hWnd;}

	enum StockPairDialogIds
	{
		staticStock1Id = dialogvalueIdCount,
		editStock1Id,
		staticStock2Id,
		editStock2Id,
//		staticValueId,
//		spinValueId,

		dialogStockPairIdCount
	};
	virtual bool DoReset() override;
	virtual bool isChanged() const override;
	virtual bool isValid() const override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void DoApply() override;
	virtual BOOL OnInitDialog() override;
//	virtual void DoEnableOkButton(bool enable) override;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSymbol1Update();
	afx_msg void OnSymbol1Change();
	afx_msg void OnSymbol2Update();
	afx_msg void OnSymbol2Change();
//	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	DECLARE_MESSAGE_MAP()
/*
	unsigned int m_decDigits;
	unsigned int m_multiplier;
	unsigned int m_divider;
*/
	CStatic m_StaticStock1;
	ValidEdit m_EditStock1;
	CStatic m_StaticStock2;
	ValidEdit m_EditStock2;
//	CStatic m_StaticValue;
//	SignedNumberTypeSpin<int, unsigned int> m_SpinValue;

	std::string m_symbol1;
	std::string m_symbol2;
//	SignedPrice m_value;
	std::string m_currentSymbol1;
	std::string m_currentSymbol2;
//	SignedPrice m_currentValue;
/*
	static const int staticWidth = 28;
	static const int spinWidth = 60;
	static const int spinHeight = 12;
*/
};

