#include "stdafx.h"
#include "BaseDialogSettingSimulateQuote.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulateQuote::BaseDialogSettingSimulateQuote(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseDialogSettingSimulateOption(mainWnd, parentTab, label),
//	m_EditRef("0123456789ABCDEF", false),
	m_ComboBoxSide(false),
	m_side(Buy),
	m_bookId(ECNBOOK_NSDQ)
{
	m_ComboBoxSide.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_bottomControl += vgap;
	int width = 64;
	int sideWidth = width / 2;

	int controlWidth = m_xc + width;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE | ES_RIGHT, 0, m_xc, m_bottomControl, width, editHeight, editRefId);
	AddStaticIndirect("Ref# (Hex)", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (editHeight - staticHeight) / 2, sideWidth, staticHeight, staticRefId);
	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + sideWidth;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += editHeight + vgap, sideWidth, 50, comboSideId);
	AddStaticIndirect("Side", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticSideId);
	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + sideWidth;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, sideWidth, 150, comboBookId);
	AddStaticIndirect("BookId", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticBookId);
	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	m_bottomControl += comboBoxHeight;
}

void BaseDialogSettingSimulateQuote::UpdateSettings()
{
	BaseDialogSettingSimulateOption::UpdateSettings();
	m_EditRef.MakeOriginal();
}

void BaseDialogSettingSimulateQuote::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
	BaseDialogSettingSimulateOption::UpdateControls();
}

void BaseDialogSettingSimulateQuote::BeforeDestroy()
{
	BaseDialogSettingSimulateOption::BeforeDestroy();
}

/*
void BaseDialogSettingSimulateSymbolMap::Leaving()
{
	TD_GetNewAccountObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
}
*/

BOOL BaseDialogSettingSimulateQuote::OnInitDialog()
{
	m_side = Buy;
	m_bookId = ECNBOOK_NSDQ;
	m_bookName.clear();
	m_refNumber.clear();

	BOOL ret = BaseDialogSettingSimulateOption::OnInitDialog();
	m_EditRef.SetLimitText(16);

	m_ComboBoxSide.AddString("Buy");
	m_ComboBoxSide.AddString("Sell");

	m_ComboBoxSide.SetCurSel(0);

	m_ComboBoxBookId.AddItem("ARCA", ECNBOOK_ARCA);
	m_ComboBoxBookId.AddItem("NSDQ", ECNBOOK_NSDQ);
	m_ComboBoxBookId.AddItem("BATS", ECNBOOK_BATS);
	m_ComboBoxBookId.AddItem("EDGA", ECNBOOK_EDGA);
	m_ComboBoxBookId.AddItem("EDGX", ECNBOOK_EDGX);
	m_ComboBoxBookId.AddItem("NYSE", ECNBOOK_NYS);
	m_ComboBoxBookId.AddItem("BATY", ECNBOOK_BATY);
	m_ComboBoxBookId.AddItem("AMEX", ECNBOOK_AMEX);
//	m_ComboBoxBookId.AddItem("NSX", ECNBOOK_NSX);
//	m_ComboBoxBookId.AddItem("IEX", ECNBOOK_IEX);
	m_ComboBoxBookId.AddItem("LVL2", BOOK_LEVEL2);
	m_ComboBoxBookId.AddItem("GLBX", ECNBOOK_GLBX);
	m_ComboBoxBookId.AddItem("OTCX", ECNBOOK_OTCX);

	m_ComboBoxBookId.SetCurSel(0);

	return ret;
}

void BaseDialogSettingSimulateQuote::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateOption::DoDataExchange(pDX);
	DDX_Control(pDX, editRefId, m_EditRef);
	DDX_Control(pDX, comboSideId, m_ComboBoxSide);
	DDX_Control(pDX, comboBookId, m_ComboBoxBookId);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateQuote, BaseDialogSettingSimulateOption)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editRefId, OnRefChange)
	ON_EN_UPDATE(editRefId, OnRefUpdate)
	ON_CBN_SELCHANGE(comboSideId, OnSelchangeSide)
	ON_CBN_SELCHANGE(comboBookId, OnSelchangeBook)
END_MESSAGE_MAP()

void BaseDialogSettingSimulateQuote::OnSelchangeBook()
{
	unsigned char bookId = m_ComboBoxBookId.GetSelectedNumber();
	if(bookId < ECNBOOK_NSDQ)bookId = ECNBOOK_NSDQ;
	if(bookId != m_bookId)
	{
		bool level2Changed = (BOOK_LEVEL2 == m_bookId) != (BOOK_LEVEL2 == bookId);
		m_bookId = bookId;
		m_bookName = m_ComboBoxBookId.GetSelectedName();
		m_modified = true;
		if(level2Changed)
		{
#ifndef TAKION_NO_OPTIONS
			m_CheckBoxOption.Invalidate(FALSE);
			OptionModeChanged();
#endif
			Level2Changed();
			m_EditRef.Invalidate(FALSE);
			EnableSendMessageButton();
		}
	}
}

void BaseDialogSettingSimulateQuote::OnSelchangeSide()
{
	int sel = m_ComboBoxSide.GetCurSel();
	if(sel < Buy)sel = Buy;
	else if(sel > Sell)sel = Sell;
	const unsigned char side = (unsigned char)sel;
	if(side != m_side)
	{
		m_side = side;
		m_modified = true;
	}
}

void BaseDialogSettingSimulateQuote::OnRefUpdate()
{
	m_EditRef.TryUpdate();
}

void BaseDialogSettingSimulateQuote::MessageSent()
{
	if(m_EditRef.isAutoAdd())
	{
		unsigned __int64 refNumberCurrent;
		U_ReadHexUnsignedNumberFromString(refNumberCurrent, m_refNumber.c_str());
		ReplaceRefNumber(m_EditRef, m_refNumber, refNumberCurrent, refNumberCurrent + 1);
	}
}

void BaseDialogSettingSimulateQuote::OnRefChange()
{
	bool empty = m_refNumber.empty();
	CString text;
	m_EditRef.GetWindowText(text);
	m_refNumber = text;
	m_modified = true;
	if(m_refNumber.empty() != empty)
	{
		m_EditRef.Invalidate(FALSE);
		EnableSendMessageButton();
	}
}

bool BaseDialogSettingSimulateQuote::isSendMessageButtonEnabled() const
{
	return BaseDialogSettingSimulateOption::isSendMessageButtonEnabled()
		&& (!isRefUsed() || !m_refNumber.empty());
}

#ifndef TAKION_NO_OPTIONS
bool BaseDialogSettingSimulateQuote::isOptionModeUsed() const
{
	switch(m_bookId)
	{
		case BOOK_LEVEL2:
		return BaseDialogSettingSimulateOption::isOptionModeUsed();

		default:
		return false;
	}
}
#endif

HBRUSH BaseDialogSettingSimulateQuote::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editRefId:
		if(isRefUsed())
		{
			if(m_refNumber.empty())
			{
				pDC->SetBkMode(TRANSPARENT);
				return *m_visualBase->GetWarningBrush();
			}
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		break;

	}
	return BaseDialogSettingSimulateOption::OnCtlColor(pDC, pWnd, nCtlColor);
}
