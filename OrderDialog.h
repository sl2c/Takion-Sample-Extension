#pragma once

//#include "TakionGui.h"

class OrderDialog : public DraggableDialog
{
public:
	OrderDialog();
//	virtual HWND GetFirstTabControl(){return m_edit.m_hWnd;}
protected:
//	virtual void Resize(bool widthChanged = true, bool heightChanged = true);
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CEdit m_edit;
	DECLARE_MESSAGE_MAP()
};