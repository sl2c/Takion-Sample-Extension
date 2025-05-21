#pragma once

#include "BaseSampleDialogSettingOption.h"

enum SampleDialogSettingCancelIds
{
	buttonCancelOrderId = baseSymbolOptionIdsCount,
	buttonCheckAllId,
	buttonUncheckAllId,

	spinClientIdId,
	checkBoxClientIdId,

	groupAccountAllId,
	radioAccountCurrentId,
	radioAccountAllId,

	groupPositionId,
	radioPositionCurrentId,
	radioPositionEachId,
	radioPositionAllId,

#ifndef TAKION_NO_OPTIONS
	groupInstrumentId,
	checkBoxInstrumentStockId,
	checkBoxInstrumentOptionId,
	checkBoxInstrumentFutureId,
#endif

	groupCancelSideId,
	checkBoxSideBuyId,
	checkBoxSideSellId,
	checkBoxSideShortId,

	groupDirectionId,
	checkBoxDirectionCloseId,
	checkBoxDirectionOpenId,
	checkBoxDirectionAddId,

	staticPickOrderId,
	listBoxPickOrderId,

	groupCancelTifId,
	checkBoxTifOnOpenId,
	checkBoxTifOnCloseId,
	checkBoxTifDayId,
	checkBoxTifExtendedId,
	checkBoxTifTimeId,
	checkBoxTifIocId,
	checkBoxTifFokId,
	checkBoxTifGtcId,
	checkBoxTifGttId,
	checkBoxTifExtendedTradingCloseId,

	groupCancelOrderTypeId,
	checkBoxOrderTypeAlgoId,
	checkBoxOrderTypeLimitId,
	checkBoxOrderTypeMarketId,
	checkBoxOrderTypeStopId,

	groupCancelPqOpenId,
	checkBoxPqOpenYesId,
	checkBoxPqOpenNoId,

	groupCancelTCloseId,
	checkBoxTCloseYesId,
	checkBoxTCloseNoId,

	checkBoxExcludeDestinationsId,
	listBoxDestinationsId,
	staticDestinationCountId,

	checkBoxExcludeMnemonicsId,
	listBoxMnemonicsId,

	editCancelOrderMnemonicId,
	buttonMnemonicFindId,
	buttonMnemonicAddId,
	buttonMnemonicRemoveId,
	buttonMnemonicClearId,
//Replace
	checkBoxReplaceOrderId,
	checkBoxReplaceOrderNativeId,
	checkBoxReplaceOrderMarketId,
	staticReplaceOrderPriceOffsetId,
	staticReplaceOrderSizeOffsetId,
	spinReplaceOrderPriceOffsetId,
	checkBoxReplaceOrderPriceOffsetPercentId,
	checkBoxReplaceOrderPriceEnforceMinOffsetId,
	checkBoxReplaceOrderPriceOffsetReverseForSellId,
	spinReplaceOrderSizeOffsetId,
	checkBoxReplaceOrderSizeOffsetPercentId,
	checkBoxReplaceOrderSizeOffsetReverseForSellId,
	checkBoxReplaceOrderPreBorrowId,
	spinReplaceOrderPreBorrowId,
	staticReplaceOrderPreBorrowCentId,
	checkBoxReplaceOrderAdjustableId,
	checkBoxReplaceOrderAdjustableMmBoxId,
	groupReplaceOrderDisplaySizeId,
	radioReplaceOrderDisplaySizeNoChangeId,
	radioReplaceOrderDisplaySizeFractionId,
	radioReplaceOrderDisplaySizeRoundLotId,
	spinReplaceOrderDisplaySizeFractionId,
	spinReplaceOrderDisplaySizeRoundLotId,
////
//Cancel / Replace All
	groupCancelReplaceOrdersAllId,
	buttonCancelReplaceOrdersAllId,
	buttonCancelReplaceOrdersBuyId,
	buttonCancelReplaceOrdersSellShortLimitId,
	buttonCancelReplaceOrdersShortStopId,
	buttonCancelReplaceOrdersLimitNsdqDayExtId,
	buttonCancelReplaceOrdersAllOpeningId,
	buttonCancelReplaceOrdersSellShortClosingId,
////
	sampleDialogSettingCancelIdsCount
};

class SampleDialogSettingCancel : public BaseSampleDialogSettingOption
{
public:
	SampleDialogSettingCancel(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual void UpdateControls() override;
	virtual void Leaving() override;

	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;

	virtual StringSetting* GetAccountIdSetting() override;

	bool CanCancel() const;

	bool CanCheckAll() const;
	bool CanUncheckAll() const;

protected:
	virtual void ToolTipsEnabled(bool enable) override;

	virtual void InitControls() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCancelOrder();

	afx_msg void OnCheckAll();
	afx_msg void OnUncheckAll();

	afx_msg void OnClientId();
	afx_msg void OnAccountCurrent();
	afx_msg void OnAccountAll();
	afx_msg void OnPositionCurrent();
	afx_msg void OnPositionEach();
	afx_msg void OnPositionAll();
#ifndef TAKION_NO_OPTIONS
	afx_msg void OnInstrumentStock();
	afx_msg void OnInstrumentOption();
	afx_msg void OnInstrumentFuture();
#endif
	afx_msg void OnSideBuy();
	afx_msg void OnSideSell();
	afx_msg void OnSideShort();
	afx_msg void OnDirectionClose();
	afx_msg void OnDirectionOpen();
	afx_msg void OnDirectionAdd();

	afx_msg void OnTifOnOpen();
	afx_msg void OnTifOnClose();
	afx_msg void OnTifDay();
	afx_msg void OnTifExtended();
	afx_msg void OnTifTime();
	afx_msg void OnTifIoc();
	afx_msg void OnTifFok();
	afx_msg void OnTifGtc();
	afx_msg void OnTifGtt();
	afx_msg void OnTifExtendedTradingCloseClose();

	afx_msg void OnOrderTypeAlgo();
	afx_msg void OnOrderTypeLimit();
	afx_msg void OnOrderTypeMarket();
	afx_msg void OnOrderTypeStop();

	afx_msg void OnPqOpenYes();
	afx_msg void OnPqOpenNo();

	afx_msg void OnTCloseYes();
	afx_msg void OnTCloseNo();

	afx_msg void OnSelchangeOrder();

	afx_msg void OnExcludeDestinations();
	afx_msg void OnExcludeMnemonics();

	afx_msg void OnSelchangeMnemonic();
	afx_msg void OnDblClkMnemonic();
	afx_msg void OnMnemonicChange();
	afx_msg void OnMnemonicFind();
	afx_msg void OnMnemonicAdd();
	afx_msg void OnMnemonicRemove();
	afx_msg void OnMnemonicClear();

	afx_msg LRESULT OnListBoxRightButtonSelection(WPARAM ctrlId, LPARAM l);
	afx_msg LRESULT OnListBoxItemsCleared(WPARAM ctrlId, LPARAM l);
	afx_msg LRESULT OnListBoxItemRemoved(WPARAM ctrlId, LPARAM l);
	afx_msg LRESULT OnListBoxAddItem(WPARAM ctrlId, LPARAM l);
	afx_msg LRESULT OnListBoxFindItem(WPARAM ctrlId, LPARAM l);
	afx_msg LRESULT OnListBoxItemsPastedFromClipboard(WPARAM ctrlId, LPARAM l);
//ReplaceOrder
	afx_msg void OnReplaceOrder();
	afx_msg void OnReplaceNative();
	afx_msg void OnReplacePriceOffsetPercent();
	afx_msg void OnReplacePriceEnforceMinOffset();
	afx_msg void OnReplacePriceOffsetReverseForSell();
	afx_msg void OnMarketOrder();
	afx_msg void OnReplaceSizeOffsetPercent();
	afx_msg void OnReplaceSizeOffsetReverseForSell();
	afx_msg void OnAdjustable();
	afx_msg void OnAdjustableMmBox();
	afx_msg void OnPreBorrow();
	afx_msg void OnDisplaySizeNoChange();
	afx_msg void OnDisplaySizeFraction();
	afx_msg void OnDisplaySizeRoundLot();
////
//CancelReplaceOrders
	afx_msg void OnCancelReplaceOrdersAll();
	afx_msg void OnCancelReplaceOrdersBuy();
	afx_msg void OnCancelReplaceOrdersSellShortLimit();
	afx_msg void OnCancelReplaceOrdersShortStop();
	afx_msg void OnCancelReplaceOrdersLimitNsdqDayExt();
	afx_msg void OnCancelReplaceOrdersAllOpening();
	afx_msg void OnCancelReplaceOrdersSellShortClosing();
////
	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	DECLARE_MESSAGE_MAP()

	void CancelReplaceOrdersAll(unsigned char sideMask,
		unsigned char directionMask,
#ifndef TAKION_NO_OPTIONS
		unsigned char instrumentMask,
#endif
		OrderCancelPick orderCancelPick,
		unsigned int tifMask,
		unsigned char orderTypeMask,
		unsigned char pqOpenMask,
		unsigned char tCloseMask,
		bool excludeDestinations,
		const std::set<unsigned int>* destinationSet,
		bool excludeMnemonics,
		const std::set<unsigned int>* mnemonicSet);

	ButtonToolTip m_ButtonCancelOrder;
	ButtonToolTip m_ButtonCheckAll;
	ButtonToolTip m_ButtonUncheckAll;

	UnsignedNumberTypeSpin<unsigned int> m_SpinClientId;
	CheckBoxOwnerDraw m_CheckBoxClientId;

	RadioGroup m_RadioGroupAccount;
	RadioOwnerDraw m_RadioAccountCurrent;
	RadioOwnerDraw m_RadioAccountAll;

	RadioGroup m_RadioGroupPosition;
	RadioOwnerDraw m_RadioPositionCurrent;
	RadioOwnerDraw m_RadioPositionEach;
	RadioOwnerDraw m_RadioPositionAll;

#ifndef TAKION_NO_OPTIONS
	void UpdateInstrumentColors();
	void DoUpdateInstrumentColors(const COLORREF color);
	void OnInstrument();
	void OnInstrument(unsigned char instrumentFlag, const CheckBoxOwnerDraw& instrumentCheckBox);
	void SetInstrumentMask(unsigned char instrumentMask);
	CheckBoxOwnerDraw m_CheckBoxInstrumentStock;
	CheckBoxOwnerDraw m_CheckBoxInstrumentOption;
	CheckBoxOwnerDraw m_CheckBoxInstrumentFuture;
#endif

	void UpdateSideColors();
	void DoUpdateSideColors(const COLORREF color);
	void OnSide();
	void OnSide(unsigned char sideFlag, const CheckBoxOwnerDraw& sideCheckBox);
	void SetSideMask(unsigned char sideMask);
	CheckBoxOwnerDraw m_CheckBoxSideBuy;
	CheckBoxOwnerDraw m_CheckBoxSideSell;
	CheckBoxOwnerDraw m_CheckBoxSideShort;

	void UpdateDirectionColors();
	void DoUpdateDirectionColors(const COLORREF color);
	void OnDirection();
	void OnDirection(unsigned char directionFlag, const CheckBoxOwnerDraw& directionCheckBox);
	void SetDirectionMask(unsigned char directionMask);
	CheckBoxOwnerDraw m_CheckBoxDirectionClose;
	CheckBoxOwnerDraw m_CheckBoxDirectionOpen;
	CheckBoxOwnerDraw m_CheckBoxDirectionAdd;

	ListBoxString m_ListBoxOrder;

	void UpdateTifColors();
	void DoUpdateTifColors(const COLORREF color);
	void OnTif();
	void OnTif(unsigned int tifFlag, const CheckBoxOwnerDraw& tifCheckBox);
	void SetTifMask(unsigned int tifMask);
	CheckBoxOwnerDraw m_CheckBoxTifOnOpen;
	CheckBoxOwnerDraw m_CheckBoxTifOnClose;
	CheckBoxOwnerDraw m_CheckBoxTifDay;
	CheckBoxOwnerDraw m_CheckBoxTifExtended;
	CheckBoxOwnerDraw m_CheckBoxTifTime;
	CheckBoxOwnerDraw m_CheckBoxTifIoc;
	CheckBoxOwnerDraw m_CheckBoxTifFok;
	CheckBoxOwnerDraw m_CheckBoxTifGtc;
	CheckBoxOwnerDraw m_CheckBoxTifGtt;
	CheckBoxOwnerDraw m_CheckBoxTifExtendedTradingClose;

	void UpdateOrderTypeColors();
	void DoUpdateOrderTypeColors(const COLORREF color);
	void OnOrderType();
	void OnOrderType(unsigned char orderTypeFlag, const CheckBoxOwnerDraw& orderTypeCheckBox);
	void SetOrderTypeMask(unsigned char orderTypeMask);
	void InvalidateOnOrderTypeStop();
	CheckBoxOwnerDraw m_CheckBoxOrderTypeAlgo;
	CheckBoxOwnerDraw m_CheckBoxOrderTypeLimit;
	CheckBoxOwnerDraw m_CheckBoxOrderTypeMarket;
	CheckBoxOwnerDraw m_CheckBoxOrderTypeStop;

//	void UpdatePqOpenColors();
//	void DoUpdatePqOpenColors(const COLORREF color);
//	void OnPqOpen();
//	void OnPqOpen(unsigned char pqOpenType, const CheckBoxOwnerDraw& pqOpenCheckBox);
	void SetPqOpenMask(unsigned char pqOpenMask);
	CheckBoxOwnerDraw m_CheckBoxPqOpenYes;
	CheckBoxOwnerDraw m_CheckBoxPqOpenNo;

//	void UpdateTCloseColors();
//	void OnTClose();
//	void OnTClose(unsigned char tCloseType, const CheckBoxOwnerDraw& tCloseCheckBox);
	void SetTCloseMask(unsigned char tCloseMask);
	CheckBoxOwnerDraw m_CheckBoxTCloseYes;
	CheckBoxOwnerDraw m_CheckBoxTCloseNo;

	CheckBoxOwnerDraw m_CheckBoxExcludeDestinations;
	ListBoxCheckDestination m_ListBoxDestination;
	CStatic m_StaticDestinationCount;

	typedef unsigned int MnemonicType;
	CheckBoxOwnerDraw m_CheckBoxExcludeMnemonics;
	ListBoxStringAsUIntChecked<MnemonicType> m_ListBoxMnemonics;

	ValidEdit m_EditMnemonic;
	CButton m_ButtonMnemonicFind;
	CButton m_ButtonMnemonicAdd;
	CButton m_ButtonMnemonicRemove;
	CButton m_ButtonMnemonicClear;

///Replace
	CustomGroupBoxAdjustable m_groupBoxReplaceOrder;

	CButton m_GroupDisplaySize;
	CheckBoxOwnerDraw m_CheckBoxReplaceOrder;
	CheckBoxOwnerDraw m_CheckBoxReplaceNative;
	CheckBoxOwnerDraw m_CheckBoxMarketOrder;

	CStatic m_StaticReplacePrice;
	CStatic m_StaticReplaceSize;
//	CStatic m_StaticDisplaySize;

	bool isReplacePriceOffsetInvalid() const;
	SignedNumberTypeSpin<int> m_SpinReplacePriceOffset;
	CheckBoxOwnerDraw m_CheckBoxReplacePriceOffsetPercent;
	CheckBoxOwnerDraw m_CheckBoxReplacePriceEnforceMinOffset;
	CheckBoxOwnerDraw m_CheckBoxReplacePriceReverseForSell;

	SignedNumberTypeSpin<int> m_SpinReplaceSize;
	CheckBoxOwnerDraw m_CheckBoxReplaceSizeOffsetPercent;
	CheckBoxOwnerDraw m_CheckBoxReplaceSizeReverseForSell;
	CheckBoxOwnerDraw m_CheckBoxReplaceAdjustable;
	CheckBoxOwnerDraw m_CheckBoxReplaceAdjustableUseMmBox;

	CheckBoxOwnerDraw m_CheckBoxPreBorrow;
	UnsignedNumberTypeSpin<unsigned int> m_SpinBorrowPrice;

	RadioGroup m_RadioGroupDisplaySize;
	RadioOwnerDraw m_radioDisplaySizeMode[OCDSM_COUNT + 1];

	UnsignedNumberTypeSpin<unsigned int> m_SpinDisplaySizeFraction;
	UnsignedNumberTypeSpin<unsigned int> m_SpinDisplaySizeRoundLot;
//CancelReplace Orders
	CButton m_GroupCancelReplaceOrdersAll;
////

	void InvalidateControlsOnReplaceOrder();
	void InvalidateControlsOnReplaceNative();
	void InvalidateControlsOnMarketOrder();
///
	unsigned int m_clientId;
	bool m_useClientId;
	bool m_accountAll;
	unsigned char m_positionType;
#ifndef TAKION_NO_OPTIONS
	virtual bool OptionModeChanged() override;
	unsigned char m_instrumentMask;
#endif
	unsigned char m_sideMask;
	unsigned char m_directionMask;
	unsigned char m_orderPick;
	unsigned int m_tifMask;
	unsigned char m_orderTypeMask;
	unsigned char m_pqOpenMask;
	unsigned char m_tCloseMask;

	bool m_excludeDestinations;
	bool m_excludeMnemonics;
//ReplaceOrder
	bool m_replaceOrder;
	bool m_replaceNative;
	bool m_marketOrder;
	bool m_priceOffsetPercent;
	bool m_priceEnforceMinOffset;
	bool m_priceOffsetReverseForSell;
	bool m_sizeOffsetPercent;
	bool m_sizeOffsetReverseForSell;
	bool m_adjustable;
	bool m_adjustableUseMmBox;
	bool m_preBorrow;
//	OrderCommandDisplaySizeMode m_displaySizeMode;
	unsigned char m_displaySizeMode;
////
	unsigned int m_mnemonic;

	SignedPrice m_replacePriceOffset;

	void NullifyValues();
	void EnableOrderCancelButton();
	void EnableCheckUncheckAllButtons();

	void ListBoxDestinationChecked();

	void OnAccount();
	void OnPosition();

	static bool SetYesNoMask(CheckBoxOwnerDraw& checkBox, CheckBoxOwnerDraw& otherCheckBox, unsigned char& mask, unsigned char yesnoMask);
	void OnYesNoMask(CheckBoxOwnerDraw& checkBox, CheckBoxOwnerDraw& otherCheckBox, unsigned char& mask, YesNoMask yesno);

	void MnemonicsCleared();
	void MnemonicRemoved(unsigned int i);

	void FillDisplaySize(unsigned char displaySizeMode, unsigned int& displaySize, unsigned int& displaySizeFraction) const;
//	void DisplaySizeModeChange(OrderCommandDisplaySizeMode mode);
	void DisplaySizeModeChange(unsigned char mode);

	void FillCancelReplaceParams(SignedPrice& replacePriceOffset,
		Price& preBorrowPrice,
		int& replaceSizeOffset,
		unsigned int& displaySize,
		unsigned int& displaySizeFraction,
		unsigned __int64& numericSymbol,
#ifndef TAKION_NO_OPTIONS
		unsigned __int64& optionBlock,
		bool& optionSettingsValid,
		unsigned char& securityType,
#endif
		bool& validSymbol) const;

	void AdjustReplacement(Order* oneOrder,
		const OrderReplaceSet& orderSet,
		const SignedPrice& replacePriceOffset,
		const Price& preBorrowPrice,
		const unsigned int displaySize,
		const unsigned int displaySizeFraction,
		const int replaceSizeOffset);
};