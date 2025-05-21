#include "stdafx.h"
#include "BaseDialogSettingSimulateSource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulateSource::BaseDialogSettingSimulateSource(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseDialogSettingSimulateSymbolMap(mainWnd, parentTab, label),
	m_ComboBoxSource(false),
	m_source(ST_MARKET_DATA)
{
	m_ComboBoxSource.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_bottomControl += vgap;
	int sideWidth = 32;

	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 50, comboBoxSourceId);
	AddStaticIndirect("Source", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticSourceId);

	m_bottomControl += comboBoxHeight;

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}

BOOL BaseDialogSettingSimulateSource::OnInitDialog()
{
	m_source = ST_MARKET_DATA;
	BOOL ret = BaseDialogSettingSimulateSymbolMap::OnInitDialog();

	m_ComboBoxSource.AddString("MarketData");
	m_ComboBoxSource.AddString("MarketSorter");

	m_ComboBoxSource.SetCurSel(0);
	OnSelchangeSource();

	return ret;
}

void BaseDialogSettingSimulateSource::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSymbolMap::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxSourceId, m_ComboBoxSource);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateSource, BaseDialogSettingSimulateSymbolMap)
	ON_CBN_SELCHANGE(comboBoxSourceId, OnSelchangeSource)
END_MESSAGE_MAP()

void BaseDialogSettingSimulateSource::OnSelchangeSource()
{
	int sel = m_ComboBoxSource.GetCurSel();
	if(sel < 0)sel = ST_MARKET_DATA;
//	else if(sel >= ST_Count)sel = ST_Count - 1;
	else if(sel >= 0xFF)sel = 0xFF;
	const unsigned char source = GetValidSource((unsigned char)sel);
//	const unsigned char source = (unsigned char)sel;
	if(source != m_source)
	{
		m_source = source;
		m_modified = true;
	}
}
