#pragma once

class IncrementalHexEdit : public ValidEdit
{
public:
	IncrementalHexEdit(const bool& autoAdd = false) : ValidEdit("0123456789ABCDEF", false), m_autoAdd(autoAdd), m_originalAutoAdd(autoAdd){}

	void SetAutoAdd(const bool& autoAdd){m_autoAdd = autoAdd;}
	void MakeOriginal(){m_originalAutoAdd = m_autoAdd;}
	void InitializeAutoAdd(const bool& autoAdd){m_autoAdd = m_originalAutoAdd = autoAdd;}

	const bool& isAutoAdd() const{return m_autoAdd;}
	bool isModified() const{return m_originalAutoAdd != m_autoAdd;}
	const bool& GetOriginalAutoAdd() const{return m_originalAutoAdd;}
protected:
	virtual void AddMenuItems(CMenu& menu) override
	{
		menu.AppendMenu(MF_OWNERDRAW, TMID_MOVE_UP, "Up");
		menu.AppendMenu(MF_OWNERDRAW, TMID_MOVE_DOWN, "Down");
		AddMenuSeparator(menu);
		menu.AppendMenu(MF_OWNERDRAW|(m_autoAdd ? MF_CHECKED : MF_UNCHECKED), TMID_ADD, "Auto");
		AddMenuSeparator(menu);
		ValidEdit::AddMenuItems(menu);
	}
	virtual void DrawMenuItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override
	{
		switch(lpDrawItemStruct->itemID)
		{
			case TMID_MOVE_UP:
			case TMID_MOVE_DOWN:
			UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, false);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
			break;

			case TMID_ADD:
			UGUI_DrawOwnerDrawStringMenuItem(lpDrawItemStruct, true);// , false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF, 0xFFFFFFFF, nullptr, nullptr, height, 1);//, false, 0xFFFFFFFF, nullptr, nullptr, 0xFFFFFFFF);
			break;

			default:
			ValidEdit::DrawMenuItem(lpDrawItemStruct);
			break;
		}
	}
	virtual bool MeasureMenuItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override
	{
		switch(lpMeasureItemStruct->itemID)
		{
			case TMID_MOVE_UP:
			case TMID_MOVE_DOWN:
			case TMID_ADD:
			lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYMENU);
			lpMeasureItemStruct->itemWidth = m_copyMenuWidth;// +lpMeasureItemStruct->itemHeight;
			break;

			default:
			return ValidEdit::MeasureMenuItem(lpMeasureItemStruct);
		}
		return true;
	}
	unsigned __int64 GetTextAsHex() const
	{
		CString text;
		GetWindowText(text);
		unsigned __int64 refNumberCurrent;
		U_ReadHexUnsignedNumberFromString(refNumberCurrent, text);
		return refNumberCurrent;
	}
	void SetNumber(const unsigned __int64& refNumberCurrent)
	{
		if(refNumberCurrent)
		{
			char num[33];
			const char* ptr = U_UnsignedNumberHexToStr(refNumberCurrent, num, sizeof(num));
			SetWindowText(ptr);
		}
		else
		{
			SetWindowText("");
		}
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override
	{
		switch(wParam)
		{
			case TMID_MOVE_UP:
			SetNumber(GetTextAsHex() + 1);
			break;

			case TMID_MOVE_DOWN:
			SetNumber(GetTextAsHex() - 1);
			break;

			case TMID_ADD:
			SetAutoAdd(!m_autoAdd);
			break;

			default:
			return ValidEdit::OnCommand(wParam, lParam);
		}
		return TRUE;
	}
	bool m_autoAdd;
	bool m_originalAutoAdd;
};

#pragma once
