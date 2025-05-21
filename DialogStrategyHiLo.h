#pragma once

#include "BaseDialogSettingStrategyTimer.h"

class DialogStrategyHiLo;

class ListBoxSecurityHiLo : public ListBoxSecurityTimer
{
public:
	ListBoxSecurityHiLo(DialogStrategyHiLo* owner, Account* account);
	DialogStrategyHiLo* GetDlgHiLo(){return m_dlgHiLo;}
protected:
	void DrawPrice(const Price& price, HDC hDC, CRect& r, unsigned int alignment);
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
	virtual SecurityObserver* CreateSecurityObserver(const unsigned __int64& numericSymbol) override;//, const Security* security, const Position* position) override;

	DialogStrategyHiLo* m_dlgHiLo;
};

class HiLoSecurityObserver : public TimerSecurityObserver
{
public:
//	HiLoSecurityObserver(const unsigned __int64& numericSymbol, const Security* security, const Position* position, ListBoxSecurityHiLo* listBox):
	HiLoSecurityObserver(const unsigned __int64& numericSymbol, ListBoxSecurityHiLo* listBox):
		TimerSecurityObserver(numericSymbol, listBox),//security, position, listBox),
		m_yesterdayVolume(0),
		m_yesterdayVolumeNew(0),
		m_positionEnterTime(0),
		m_doNotEnter(false),
		m_lbHiLo(listBox)
	{
	}
	const Price& GetDayHiPrice() const{return m_dayHiPrice;}
	const Price& GetDayLoPrice() const{return m_dayLoPrice;}
	const Price& GetYesterdayHiPrice() const{return m_yesterdayHiPrice;}
	const Price& GetYesterdayLoPrice() const{return m_yesterdayLoPrice;}
	const unsigned __int64& GetYesterdayVolume() const{return m_yesterdayVolume;}
	const bool& isDoNotEnter() const{return m_doNotEnter;}
	const unsigned int& GetPositionEnterTime() const{return m_positionEnterTime;}
	bool CanEnter() const;
protected:
	void Enter();
	bool EnterHi();
	bool EnterLo();
	void Exit();
	virtual void OnPosSizeChanged() override;
	void SendOpeningOrder(const bool hi);

	virtual void NullifySecurityValues() override;
	virtual void NullifySecurityConstValues() override;
	virtual void RememberSecurityValues() override;
	virtual void RememberSecurityConstValues() override;
	virtual bool DoUpdateSecurity() override;
	virtual bool DoUpdateSecurityConstValues() override;
//	virtual bool DoUpdatePosition() override;
//	virtual bool DoUpdatePositionConstValues() override;

	Price m_dayHiPrice;
	Price m_dayLoPrice;
	Price m_yesterdayHiPrice;
	Price m_yesterdayLoPrice;
	unsigned __int64 m_yesterdayVolume;

	Price m_dayHiPriceNew;
	Price m_dayLoPriceNew;
	Price m_yesterdayHiPriceNew;
	Price m_yesterdayLoPriceNew;
	unsigned __int64 m_yesterdayVolumeNew;

	unsigned int m_positionEnterTime;

	bool m_doNotEnter;

	ListBoxSecurityHiLo* m_lbHiLo;

	virtual void OnTradingStateChange();
};

enum HiLoColumnId : unsigned short
{
	ST_HL_HI = SO_Count,
	ST_HL_LO,
	ST_HL_Y_HI,
	ST_HL_Y_LO,
	ST_HL_Y_VOLUME,
	ST_HL_LAST_ENTER,

	ST_HL_Count,
};

enum HiLoStrategyIds
{
	groupHiloFilterId = timerStrategyIdsCount,
	staticHiLoYesterdayVolumeId,
	spinHiLoYesterdayVolumeId,
	staticHiLoMinPriceId,
	spinHiLoMinPriceId,
	staticHiLoMaxPriceId,
	spinHiLoMaxPriceId,
	buttonHiLoFilterApplyId,
	buttonHiLoFilterResetId,

	groupHiloOrderSizeId,
	spinHiLoOrderSizeId,
	staticHiLoOrderSizeId,
	buttonHiLoOrderSizeApplyId,
	buttonHiLoOrderSizeResetId,

	groupHiLoEnterId,
	staticHiLoEnterDestinationId,
	staticHiLoEnterRoutingId,
	staticHiLoEnterMnemonicId,
	staticHiLoEnterReserveSizeId,
	comboBoxHiLoEnterDestinationId,
	comboBoxHiLoEnterRoutingId,
	editHiLoEnterMnemonicId,
	spinHiLoEnterReserveSizeId,
	buttonHiLoEnterVenueApplyId,
	buttonHiLoEnterVenueResetId,

	groupHiLoExitId,
	staticHiLoExitDestinationId,
	staticHiLoExitRoutingId,
	staticHiLoExitMnemonicId,
	staticHiLoExitReserveSizeId,
	comboBoxHiLoExitDestinationId,
	comboBoxHiLoExitRoutingId,
	editHiLoExitMnemonicId,
	spinHiLoExitReserveSizeId,
	buttonHiLoExitVenueApplyId,
	buttonHiLoExitVenueResetId,

	hiloStrategyIdsCount
};

class DialogStrategyHiLo : public BaseDialogSettingStrategyTimer
{
public:
	DialogStrategyHiLo(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
	virtual StringSetting* GetAccountIdSetting() override;
	virtual BoolSetting* GetSettingTradeOnStart() override;
	virtual BoolSetting* GetSettingLoadOnStart() override;
	virtual BoolSetting* GetSettingLoadFromFile() override;
	virtual StringSetting* GetSettingStockFilePath() override;
	virtual UCharSetting* GetSettingSymbolDelimiter() override;

	virtual UIntSetting* GetSettingMillisecondEntry() override;
	virtual UIntSetting* GetSettingMillisecondStop() override;
	virtual UIntSetting* GetSettingMillisecondExit() override;
	virtual UIntSetting* GetSettingMillisecondStopExit() override;

	virtual void RepaintTotals() const override;

//	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
//	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

	virtual bool SecurityBelongs(const Security* security) const override
	{
		if(BaseDialogSettingStrategy::SecurityBelongs(security))
		{
			if(security->GetYesterdaysVolume() >= m_yesterdayVolume)
			{
				return true;
			}
		}
		return false;
/*
		switch(security->GetPrimaryExchange())//NYSE only
		{
			case PE_NYSE:
			case PE_AMEX:
			return !security->isTest();

			default:
			return false;
		}
*/
	}
//	virtual bool CanTrade() const{return m_trading && m_account;}

	const unsigned int& GetEnterReserveQuantity() const{return m_enterReserveSize;}
	const unsigned int& GetExitReserveQuantity() const{return m_exitReserveSize;}
	const Destination* GetCurrentEnterDestination() const{return m_currentEnterDestination;}
	const Destination* GetCurrentExitDestination() const{return m_currentExitDestination;}
	const Destination* GetExchangeDestination() const{return &m_exchangeDestination;}
	const DestinationRouting* GetCurrentEnterRouting() const{return m_currentEnterRouting;}
	const DestinationRouting* GetCurrentExitRouting() const{return m_currentExitRouting;}
	const unsigned int& GetEnterMnemonic() const{return m_enterMnemonic;}
	const unsigned int& GetExitMnemonic() const{return m_exitMnemonic;}
	const unsigned int& GetOrderSize() const{return m_orderSize;}
	const Price& GetMinPrice() const{return m_minPrice;}
	const Price& GetMaxPrice() const{return m_maxPrice;}
protected:
	virtual void InitControls() override;
	virtual BOOL OnInitDialog() override;

	virtual void OnHistoryLoaded();
	virtual void OnExecutorConnected();

	virtual void ToolTipsEnabled(bool enable) override;

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual void BeforeDestroy() override;
//	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
//	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	afx_msg void OnFilterApply();
	afx_msg void OnFilterReset();
//	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnOrderSizeApply();
	afx_msg void OnOrderSizeReset();

	afx_msg void OnEnterMnemonicUpdate();
	afx_msg void OnEnterMnemonicChange();
	afx_msg void OnSelchangeEnterDestination();
	afx_msg void OnSelchangeEnterRouting();
	afx_msg void OnEnterVenueApply();
	afx_msg void OnEnterVenueReset();

	afx_msg void OnExitMnemonicUpdate();
	afx_msg void OnExitMnemonicChange();
	afx_msg void OnSelchangeExitDestination();
	afx_msg void OnSelchangeExitRouting();
	afx_msg void OnExitVenueApply();
	afx_msg void OnExitVenueReset();

	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinYesterdayVolume;
	UnsignedNumberTypeSpin<unsigned int> m_SpinMinPrice;
	UnsignedNumberTypeSpin<unsigned int> m_SpinMaxPrice;
	CButton m_ButtonFilterApply;
	CButton m_ButtonFilterReset;

	UnsignedNumberTypeSpin<unsigned int> m_SpinOrderSize;
	CButton m_ButtonOrderSizeApply;
	CButton m_ButtonOrderSizeReset;

	ComboBoxDestination m_ComboBoxEnterDestination;
	ComboBoxRouting m_ComboBoxEnterRouting;
	ValidEdit m_EditEnterMnemonic;
	UnsignedNumberTypeSpin<unsigned int> m_SpinEnterReserveSize;
	CStatic m_StaticEnterReserveSize;
	CButton m_ButtonEnterVenueApply;
	CButton m_ButtonEnterVenueReset;

	ComboBoxDestination m_ComboBoxExitDestination;
	ComboBoxRouting m_ComboBoxExitRouting;
	ValidEdit m_EditExitMnemonic;
	UnsignedNumberTypeSpin<unsigned int> m_SpinExitReserveSize;
	CStatic m_StaticExitReserveSize;
	CButton m_ButtonExitVenueApply;
	CButton m_ButtonExitVenueReset;

	ListBoxSecurityHiLo m_ListBoxHiLo;

	static bool UpdateReserveSize(UnsignedNumberTypeSpin<unsigned int>& spin, unsigned int& controlReserveSize, const unsigned int& currentReserveSize);

	const Destination* m_controlEnterDestination;
	const Destination* m_currentEnterDestination;
	const DestinationRouting* m_controlEnterRouting;
	const DestinationRouting* m_currentEnterRouting;
//	void SetEnterRoutingId(unsigned short routingId, unsigned short routingSubType);

	const Destination* m_controlExitDestination;
	const Destination* m_currentExitDestination;
	const DestinationRouting* m_controlExitRouting;
	const DestinationRouting* m_currentExitRouting;
//	void SetExitRoutingId(unsigned short routingId, unsigned short routingSubType);

	static bool OnMnemonicChange(CEdit& edit, unsigned int& controlMnemonic, const unsigned int currentMnemonic);

	void EnableEnterVenueButtons();
	void ResetEnterVenueControls();
	unsigned int m_controlEnterVenue;
	unsigned int m_enterVenue;
	unsigned int m_controlEnterRoutingName;
	unsigned int m_enterRoutingName;
	unsigned int m_controlEnterMnemonic;
	unsigned int m_enterMnemonic;
	unsigned int m_controlEnterReserveSize;
	unsigned int m_enterReserveSize;

	void EnableExitVenueButtons();
	void ResetExitVenueControls();
	unsigned int m_controlExitVenue;
	unsigned int m_exitVenue;
	unsigned int m_controlExitRoutingName;
	unsigned int m_exitRoutingName;
	unsigned int m_controlExitMnemonic;
	unsigned int m_exitMnemonic;
	unsigned int m_controlExitReserveSize;
	unsigned int m_exitReserveSize;

	void EnableFilterButtons();
	unsigned __int64 m_controlYesterdayVolume;
	unsigned __int64 m_yesterdayVolume;
	Price m_controlMinPrice;
	Price m_minPrice;
	Price m_controlMaxPrice;
	Price m_maxPrice;
//	void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);

	void EnableOrderSizeButtons();
	unsigned int m_controlOrderSize;
	unsigned int m_orderSize;

	Destination m_exchangeDestination;

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
