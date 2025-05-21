#include "stdafx.h"
#include "BaseDialogSettingSimulateSymbolMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulateSymbolMap::BaseDialogSettingSimulateSymbolMap(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseDialogSettingSimulateSymbol(mainWnd, parentTab, label),
	m_SpinSymbolMap(0xFFFF, 0, 0, 1000, 3, TakionSpin::charComma),
	m_symbolMap(0)
{
	m_SpinSymbolMap.AddSpinCell(1, false);
	m_SpinSymbolMap.AddSpinCell(10, false);
	m_SpinSymbolMap.AddSpinCell(1000, true);
	m_SpinSymbolMap.AddSpinCell(100, true);

	m_bottomControl += vgap;
//	int width = 52;
//	int halfWidth = width / 2;
	AddButtonIndirect("SymbolMap", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, m_xc + m_firstColumnWidth + hgap, m_bottomControl + (spinHeight - checkBoxHeight) / 2, m_firstColumnWidth, checkBoxHeight, radioSimSymbolMapId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, m_bottomControl, m_firstColumnWidth, spinHeight, spinSimSymbolMapId);

	m_bottomControl += spinHeight;

	const int controlWidth = m_xc + m_firstColumnWidth + hgap + m_firstColumnWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}

/*
void BaseDialogSettingSimulateSymbolMap::UpdateSettings()
{
	BaseDialogSettingSimulateSymbol::UpdateSettings();
}

void BaseDialogSettingSimulateSymbolMap::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
	BaseDialogSettingSimulateSymbol::UpdateControls();
}

void BaseDialogSettingSimulateSymbolMap::BeforeDestroy()
{
	BaseDialogSettingSimulateSymbol::BeforeDestroy();
}

void BaseDialogSettingSimulateSymbolMap::Leaving()
{
	TD_GetNewAccountObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
}
*/

BOOL BaseDialogSettingSimulateSymbolMap::OnInitDialog()
{
	m_symbolMap = 0;

	BOOL ret = BaseDialogSettingSimulateSymbol::OnInitDialog();

	EnableControl(m_RadioSymbol.m_hWnd, true, m_EditSymbol.m_hWnd);
	SymbolTypeChanged();

	return ret;
}

void BaseDialogSettingSimulateSymbolMap::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSymbol::DoDataExchange(pDX);
	DDX_Control(pDX, radioSimSymbolMapId, m_RadioSymbolMap);
	DDX_Control(pDX, spinSimSymbolMapId, m_SpinSymbolMap);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateSymbolMap, BaseDialogSettingSimulateSymbol)
	ON_BN_CLICKED(radioSimSymbolMapId, OnSymbolType)

	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

LRESULT BaseDialogSettingSimulateSymbolMap::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimSymbolMapId:
		m_symbolMap = m_SpinSymbolMap.GetValue();
		m_modified = true;
		break;

		default:
		break;
	}
	return 0;
}

void BaseDialogSettingSimulateSymbolMap::UpdateSymbolTypeColor()
{
	m_SpinSymbolMap.SetBkColorEnabled(m_symbolType == SYMT_SYMBOL_STRING || m_symbolNotUsed ? m_visualBase->GetGrayColor() : GetSysColor(COLOR_WINDOW));
}

void BaseDialogSettingSimulateSymbolMap::SymbolTypeChanged()
{
	UpdateSymbolTypeColor();
	BaseDialogSettingSimulateSymbol::SymbolTypeChanged();
}

void BaseDialogSettingSimulateSymbolMap::SymbolNotUsedChanged()
{
	UpdateSymbolTypeColor();
	BaseDialogSettingSimulateSymbol::SymbolNotUsedChanged();
}
