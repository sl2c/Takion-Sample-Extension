#pragma once

class ExtensionConfig : public ConfigBase
{
public:
	ExtensionConfig();
	ExtensionConfig(const ExtensionConfig& other);
//	virtual const char* GetIniFileName() const;
//	virtual const char* GetLogFileName() const;
//	virtual const char* GetPosFileName() const;
	virtual void Copy(const ConfigBase& other);
	virtual ConfigBase* Clone() const;

	virtual void Update();

	const ExtensionConfig& operator=(const ExtensionConfig& other);

	IntSetting m_x;
	IntSetting m_y;
	UIntSetting m_w;
	UIntSetting m_h;
	BoolSetting m_alwaysOnTop;
	StringSetting m_tab;

	UIntSetting m_msSecondBack;
	BoolSetting m_msLoadOnStart;
	StringSettingRest m_msStockFilePath;
	typedef MapSetting<USecondSetting, UIntSetting, std::map<USecondSetting, UIntSetting> > TimerMap;
	TimerMap m_msTimerMap;

	StringSetting m_mdAccountId;
	StringSetting m_mdSymbol;
protected:
	void AddSettings();
//	virtual void AdjustOldVersion();
};