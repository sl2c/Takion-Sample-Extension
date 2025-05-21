#include "stdafx.h"
#include "BaseDialogSettingSimulateOptionSource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BaseDialogSettingSimulateOptionSource::BaseDialogSettingSimulateOptionSource(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
	BaseDialogSettingSimulateOption(mainWnd, parentTab, label),
	m_ComboBoxSource(false),
	m_source(ST_MARKET_DATA)
{
	m_ComboBoxSource.SetColorInvalid(m_visualBase->GetGrayColor());
	m_ComboBoxSource.SetNoSelectionColor(m_visualBase->GetWarningColor());

	m_bottomControl += vgap;
	int sideWidth = 32;

	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, m_bottomControl, m_firstColumnWidth, 50, comboBoxOptionSourceId);
	AddStaticIndirect("Source", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (comboBoxHeight - staticHeight) / 2, sideWidth, staticHeight, staticOptionSourceId);

	m_bottomControl += comboBoxHeight;

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}

BOOL BaseDialogSettingSimulateOptionSource::OnInitDialog()
{
	m_source = ST_MARKET_DATA;
	BOOL ret = BaseDialogSettingSimulateOption::OnInitDialog();

	m_ComboBoxSource.AddString("MarketData");
	m_ComboBoxSource.AddString("MarketSorter");

	m_ComboBoxSource.SetCurSel(0);
	OnSelchangeSource();

	return ret;
}

void BaseDialogSettingSimulateOptionSource::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateOption::DoDataExchange(pDX);
	DDX_Control(pDX, comboBoxOptionSourceId, m_ComboBoxSource);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateOptionSource, BaseDialogSettingSimulateOption)
	ON_CBN_SELCHANGE(comboBoxOptionSourceId, OnSelchangeSource)
END_MESSAGE_MAP()

void BaseDialogSettingSimulateOptionSource::OnSelchangeSource()
{
	int sel = m_ComboBoxSource.GetCurSel();
	if(sel < 0)sel = ST_MARKET_DATA;
	else if(sel >= 0xFF)sel = 0xFF;
	const unsigned char source = GetValidSource((unsigned char)sel);
	if(source != m_source)
	{
		m_source = source;
		SourceChanged();
		m_modified = true;
	}
}

#ifndef TAKION_NO_OPTIONS
bool BaseDialogSettingSimulateOptionSource::OptionModeChanged()
{
	const bool ret = BaseDialogSettingSimulateOption::OptionModeChanged();
	m_ComboBoxSource.SetInvalid(ret);
	return ret;
}
#endif