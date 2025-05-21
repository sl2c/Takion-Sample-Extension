#pragma once

#include "BaseDialogSetting.h"

enum SampleWindowType : unsigned char
{
	SWT_STOCK_COMPARATOR,
	SWT_MY_MARKET_SORTER,

	SWT_Count
};

class SampleDialogSettingWindow;

class ListBoxExtensionWindow : public ListBoxOwnerDraw
{
public:
	ListBoxExtensionWindow(SampleDialogSettingWindow* dlg):
		ListBoxOwnerDraw(0, true, true, false),
		m_dlg(dlg),
		m_destroyedColor(RGB(128, 0, 128)),
		m_visibleColor(RGB(0, 128, 0)),
		m_hiddenColor(RGB(128, 0, 0)),
		m_menuWidth(0)
	{}
	int Compare(const void* item1, const void* item2) const;
	int FindItemByOrdinal(const unsigned int ordinal) const;
	virtual const char* GetItemName(const void* item) const override;
	virtual COLORREF GetStringColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	virtual COLORREF GetSelectedBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override;
	int TakionWindowAdded(TakionTool* tool);
	int TakionWindowRemoved(TakionTool* tool);
	int TakionWindowRepaintTool(TakionTool* tool);
	void DestroyTools();
	void CornerAll();
protected:
	virtual void ListBoxCreated() override;
	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;
	virtual void DrawNumber(const DRAWITEMSTRUCT* lpDrawItemStruct, const CRect& r) override;
	virtual void AddMenuItems(CMenu& menu) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	SampleDialogSettingWindow* m_dlg;
	COLORREF m_destroyedColor;
	COLORREF m_visibleColor;
	COLORREF m_hiddenColor;

	int m_menuWidth;
};

class SampleDialogSettingWindow : public BaseDialogSetting
{
public:
	SampleDialogSettingWindow(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateSettings() override;
	virtual void UpdateControls() override;
	virtual void Leaving() override;

	virtual void RestoreLayout() override;

	void TakionWindowAdded(TakionTool* tool);
	void TakionWindowRemoved(TakionTool* tool);
	void TakionWindowRepaintTool(TakionTool* tool);

	typedef std::map<unsigned int, std::string> UIntStrMap;
	bool GetWindowsLayout(UIntStrMap& windowsMap, unsigned int contentModifier) const;

	void HideSelectedWindow();
	void UnhideSelectedWindow();
	TakionTool* CreateTakionWindow() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;
	
	void CreateTakionWindowOrDisplayError();

	CButton m_ButtonNew;
	CStatic m_StaticWindowType;
	ListBoxString m_ListBoxWindowType;

	ListBoxExtensionWindow m_ListBoxExtensionWindow;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeWindowType();
	afx_msg void OnNewWindow();
	afx_msg void OnSelchangeWindow();
	afx_msg void OnDblclkWindow();
	afx_msg void OnDblclkWindowType();
	DECLARE_MESSAGE_MAP()

	int m_leftTypeOffset;
	int m_topTypeOffset;
	int m_typeWidth;
	int m_leftOffset;
	int m_topOffset;

	unsigned char m_currentType;
};