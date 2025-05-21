#include "stdafx.h"
#include "ExtensionConfig.h"
#include "MainDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern MainDialog* mainDialog;

ExtensionConfig::ExtensionConfig()
{
	AddSettings();
}

ExtensionConfig::ExtensionConfig(const ExtensionConfig& other):
	m_x(other.m_x),
	m_y(other.m_y),
	m_w(other.m_w),
	m_h(other.m_h),
	m_alwaysOnTop(other.m_alwaysOnTop),
	m_tab(other.m_tab),
	m_msSecondBack(other.m_msSecondBack),
	m_msLoadOnStart(other.m_msLoadOnStart),
	m_msStockFilePath(other.m_msStockFilePath),
	m_msTimerMap(other.m_msTimerMap),
	m_mdAccountId(other.m_mdAccountId),
	m_mdSymbol(other.m_mdSymbol)
{
	AddSettings();
}

void ExtensionConfig::AddSettings()
{
	AddSetting("X", m_x);
	AddSetting("Y", m_y);
	AddSetting("W", m_w);
	AddSetting("H", m_h);
	AddSetting("AlwaysOnTop", m_alwaysOnTop);
	AddSetting("Tab", m_tab);
	AddSetting("MS_SecondBack", m_msSecondBack);
	AddSetting("MS_LoadOnStart", m_msLoadOnStart);
	AddSetting("MS_StockFilePath", m_msStockFilePath);
	AddSetting("MS_TimerMap", m_msTimerMap);
	AddSetting("MD_AccountId", m_mdAccountId);
	AddSetting("MD_Symbol", m_mdSymbol);
}

ConfigBase* ExtensionConfig::Clone() const
{
	ExtensionConfig* clone = new ExtensionConfig;
	clone->Copy(*this);
	return clone;
}

void ExtensionConfig::Copy(const ConfigBase& other)
{
//	TakionConfig::Copy(other);
	operator=((const ExtensionConfig&)other);
}

const ExtensionConfig& ExtensionConfig::operator=(const ExtensionConfig& other)
{
	ConfigBase::operator=(other);
//	m_orderReplaceTemplateVersion = other.m_orderReplaceTemplateVersion;
	return *this;
}

void ExtensionConfig::Update()
{
	ConfigBase::Update();
	if(mainDialog)
	{
//		mainDialog->UpdateCurrentLeafSettings();
		const CRect& frameRect = mainDialog->GetFrameRect();
		m_x.SetValue(frameRect.left);
		m_y.SetValue(frameRect.top);
		m_w.SetValue((unsigned int)frameRect.Width());
		m_h.SetValue((unsigned int)frameRect.Height());
		TakionSettingDialog* leaf = mainDialog->GetCurrentLeaf();
		if(leaf)
		{
			if(leaf->isModified())
			{
				leaf->UpdateSettings();
				leaf->SetModified(false);
			}
			std::string tabPath;
			if(leaf->GetTabPath(tabPath, false) && !tabPath.empty())
			{
				m_tab.SetValue(tabPath);
			}
		}
	}
}
