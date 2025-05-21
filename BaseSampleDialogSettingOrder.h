#pragma once

#include "BaseSampleDialogSettingSendOrder.h"

enum BaseSampleSettingOrderIds
{
	groupSideId = baseSendOrderIdsCount,
	comboBoxSideId,
//	checkBoxProAtsId,

	groupPriceId,
	radioPriceMarketId,
	radioPriceOffsetId,
	radioPriceAbsoluteId,

	checkBoxPricePercentId,
	comboBoxPriceBaseId,
	spinPriceOffsetId,
	spinPriceId,

	groupSizeId,
	radioSizePositionId,
	radioSizeShareId,
	spinSizeShareId,

	groupDisplaySizeId,
	radioDisplaySizeAllId,
	radioDisplaySizeShareId,
	spinDisplaySizeShareId,

	groupTifId,
	comboBoxTifId,
	spinTifId,

	staticDestinationId,
	staticRoutingId,
	staticMnemonicId,
	staticReserveSizeId,

	comboBoxDestinationId,
	comboBoxRoutingId,
	editMnemonicId,
	spinReserveSizeId,

	groupPegId,
	comboBoxPegTypeId,
	spinPegPriceOffsetId,

	staticMinQuantityId,
	spinMinQuantityId,

	checkboxAllOrNoneId,
/*
	groupAccountId,
	comboBoxAccountId,

	groupUserIdId,
	spinUserIdId,

	buttonSendOrderId,
*/
	baseOrderIdsCount
};

enum PriceType : unsigned char
{
	PT_MARKET,
	PT_OFFSET,
	PT_ABSOLUTE,

	PT_Count
};

enum DisplaySizeType : unsigned char
{
	DST_ALL,
	DST_SHARES,

	DST_Count
};

enum SizeType : unsigned char
{
	SIT_POSITION,
	SIT_SHARES,

	SIT_Count
};

class OrderParams : public BaseOrderParams
{
public:
	const unsigned char& GetSide() const{return m_side;}
	void SetSide(const unsigned char& side){m_side = side;}

	const unsigned char& GetPriceType() const{return m_priceType;}
	void SetPriceType(const unsigned char& priceType){m_priceType = priceType;}

	const bool& isPriceOffsetPercent() const{return m_priceOffsetPercent;}
	void SetPriceOffsetPercent(const bool& priceOffsetPercent){m_priceOffsetPercent = priceOffsetPercent;}

	const unsigned char& GetPriceBase() const{return m_priceBase;}
	void SetPriceBase(const unsigned char& priceBase){m_priceBase = priceBase;}

	const unsigned char& GetSizeType() const{return m_sizeType;}
	void SetSizeType(const unsigned char& sizeType){m_sizeType = sizeType;}

	const unsigned char& GetDisplaySizeType() const{return m_displaySizeType;}
	void SetDisplaySizeType(const unsigned char& displaySizeType){m_displaySizeType = displaySizeType;}

	const unsigned char& GetTifType() const{return m_tifType;}
	void SetTifType(const unsigned char& tifType){m_tifType = tifType;}

	const SignedPrice& GetPriceOffset() const{return m_priceOffset;}
	void SetPriceOffset(const SignedPrice& priceOffset){m_priceOffset = priceOffset;}

	const Price& GetPriceAbsolute() const{return m_priceAbsolute;}
	void SetPriceAbsolute(const Price& price){m_priceAbsolute = price;}

	const unsigned int& GetSizeShares() const{return m_sizeShares;}
	void SetSizeShares(const unsigned int& sizeShares){m_sizeShares = sizeShares;}

	const unsigned int& GetMinQuantity() const{return m_minQuantity;}
	void SetMinQuantity(const unsigned int& minQuantity){m_minQuantity = minQuantity;}

	const bool& isAllOrNone() const{return m_allOrNone;}
	void SetAllOrNone(const bool& allOrNone){m_allOrNone = allOrNone;}

	const unsigned int& GetDisplaySizeShares() const{return m_displaySizeShares;}
	void SetDisplaySizeShares(const unsigned int& displaySizeShares){m_displaySizeShares = displaySizeShares;}

	const unsigned int& GetTifMilliseconds() const{return m_tifMilliseconds;}
	void SetTifMilliseconds(const unsigned int& milliseconds){m_tifMilliseconds = milliseconds;}

	const unsigned int& GetDestinationName() const{return m_destinationName;}
	void SetDestinationName(const unsigned int& name){m_destinationName = name;}

	const unsigned __int64& GetDestinationId() const{return m_destinationId;}
	void SetDestinationId(const unsigned __int64& id){m_destinationId = id;}

	const unsigned short& GetRoutingId() const{return m_routingId;}
	void SetRoutingId(const unsigned short& id){m_routingId = id;}

	const unsigned short& GetRoutingSubType() const{return m_routingSubType;}
	void SetRoutingSubType(const unsigned short& subType){m_routingSubType = subType;}

	const unsigned int& GetRoutingName() const{return m_routingName;}
	void SetRoutingName(const unsigned int& name){m_routingName = name;}

	const unsigned int& GetMnemonic() const{return m_mnemonic;}
	void SetMnemonic(const unsigned int& mnemonic){m_mnemonic = mnemonic;}

	const unsigned int& GetReserveSize() const{return m_reserveSize;}
	void SetReserveSize(const unsigned int& reserveSize){m_reserveSize = reserveSize;}

	const unsigned char& GetPegType() const{return m_pegType;}
	void SetPegType(const unsigned char& pegType){m_pegType = pegType;}

	const SignedPrice& GetPegOffset() const{return m_pegOffset;}
	void SetPegOffset(const SignedPrice& pegOffset){m_pegOffset = pegOffset;}

protected:
	OrderParams(
		const unsigned char& side = OCS_BUY,
		const unsigned char& priceType = PT_OFFSET,
		const bool& priceOffsetPercent = false,
		const unsigned char& priceBase = PB_OPPOSITE_SIDE,
		const unsigned char& sizeType = SIT_SHARES,
		const unsigned char& displaySizeType = DST_SHARES,
		const unsigned char& tifType = TIF_DAY,
//		unsigned char stopType = OST_NONE,
//		unsigned char stopBase = OSB_BID,
		const SignedPrice& priceOffset = SignedPrice(0, 0),
		const Price& priceAbsolute = Price(0, 0),
		const unsigned int& sizeShares = 0,
		const unsigned int& minQuantity = 0,
		const bool& allOrNone = false,
		const unsigned int& displaySizeShares = 0,
		const unsigned int& tifMilliseconds = 0,
//		const SignedPrice& stopPriceOffset = SignedPrice(0, 0),
		const unsigned int& destinationName = nsdqNum,
		const unsigned __int64& destinationId = NSDQ_FIX_ROUTE,
		const unsigned short& routingId = RI_NONE,
		const unsigned short& routingSubType = 0,
		const unsigned int& routingName = 0,
		const unsigned int& mnemonic = 0,
		const unsigned int& reserveSize = 0,
		const unsigned char& pegType = PEG_NONE,
		const SignedPrice& pegOffset = SignedPrice(0, 0),
//		const bool& proAts = true,
		const unsigned __int64& userId = 0,
		const unsigned int& extensionId = 0,
		const unsigned short& userType = 0,
		Account* const& account = nullptr
		):
		BaseOrderParams(userId, extensionId, userType, account),
		m_side(side),
		m_priceType(priceType),
		m_priceOffsetPercent(priceOffsetPercent),
		m_priceBase(priceBase),
		m_sizeType(sizeType),
		m_displaySizeType(displaySizeType),
		m_tifType(tifType),
		m_priceOffset(priceOffset),
		m_priceAbsolute(priceAbsolute),
		m_destinationId(destinationId),
		m_destinationName(destinationName),
		m_sizeShares(sizeShares),
		m_minQuantity(minQuantity),
		m_displaySizeShares(displaySizeShares),
		m_tifMilliseconds(tifMilliseconds),
		m_routingId(routingId),
		m_routingSubType(routingSubType),
		m_routingName(routingName),
		m_mnemonic(mnemonic),
		m_reserveSize(reserveSize),
		m_allOrNone(allOrNone),
		m_pegType(pegType),
		m_pegOffset(pegOffset)
	{
	}
	unsigned char m_side;
	unsigned char m_priceType;
	bool m_priceOffsetPercent;
	unsigned char m_priceBase;
	unsigned char m_sizeType;
	unsigned char m_displaySizeType;
	unsigned char m_tifType;
	SignedPrice m_priceOffset;
	Price m_priceAbsolute;
	unsigned __int64 m_destinationId;
	unsigned int m_destinationName;
	unsigned int m_sizeShares;
	unsigned int m_minQuantity;
	unsigned int m_displaySizeShares;
	unsigned int m_tifMilliseconds;
	unsigned short m_routingId;
	unsigned short m_routingSubType;
	unsigned int m_routingName;
	unsigned int m_mnemonic;
	unsigned int m_reserveSize;

	bool m_allOrNone;
	unsigned char m_pegType;
	SignedPrice m_pegOffset;
};

class BaseSampleDialogSettingOrder : public BaseSampleDialogSettingSendOrder
{
public:
	unsigned int GetTifMilliseconds() const;
	unsigned char GetTifType() const;
	unsigned int GetReserveQuantity() const;

	virtual unsigned char FillOrderParams(BaseOrderParams& orderParams) const override;
protected:
	BaseSampleDialogSettingOrder(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	virtual void InitControls() override;
	virtual void ToolTipsEnabled(bool enable) override;

	ComboBoxString m_ComboBoxSide;

//	CButton m_CheckBoxProAts;

	CButton m_RadioPriceMarket;
	CButton m_RadioPriceOffset;
	CButton m_RadioPriceAbsolute;
	unsigned char m_priceType;

	CButton m_CheckBoxPricePercent;

	ComboBoxString m_ComboBoxPriceBase;
	SignedNumberTypeSpin<int> m_SpinPriceOffset;
	UnsignedNumberTypeSpin<unsigned __int64> m_SpinPrice;

	CButton m_RadioSizePosition;
	CButton m_RadioSizeShare;
	unsigned char m_sizeType;
	UnsignedNumberTypeSpin<unsigned int> m_SpinSizeShare;

	CButton m_RadioDisplaySizeAll;
	CButton m_RadioDisplaySizeShare;
	unsigned char m_displaySizeType;
	UnsignedNumberTypeSpin<unsigned int> m_SpinDisplaySizeShare;

//	void FillTifComboBox(CComboBox& comboBox);
	ComboBoxString m_ComboBoxTifType;
	TifSpin m_SpinTif;

	CStatic m_StaticDestination;
	CStatic m_StaticRouting;
	CStatic m_StaticMnemonic;
	CStatic m_StaticReserveSize;
	ComboBoxDestination m_ComboBoxDestination;
	ComboBoxRouting m_ComboBoxRouting;
	ValidEdit m_EditMnemonic;
	UnsignedNumberTypeSpin<unsigned int> m_SpinReserveSize;

	CButton m_GroupPeg;
	ComboBoxString m_ComboBoxPegType;
	SignedNumberTypeSpin<int> m_SpinPegOffset;

	UnsignedNumberTypeSpin<unsigned int> m_SpinMinQuantity;
	CheckBoxOwnerDraw m_CheckBoxAllOrNone;

	const Destination* m_currentDestination;
	const DestinationRouting* m_currentRouting;
//	void UpdateDestinationRouting();
//	void SetRoutingId(unsigned short routingId, unsigned short routingSubType);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelchangeTif();
	afx_msg void OnSelchangeDestination();
	afx_msg void OnSelchangeRouting();

	afx_msg void OnPriceType();
	afx_msg void OnSizeType();
	afx_msg void OnDisplaySizeType();

	afx_msg void OnMnemonicUpdate();
	afx_msg void OnMnemonicChange();
	
	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);

	afx_msg void OnSelchangePegType();

	afx_msg void OnAllOrNone();

	DECLARE_MESSAGE_MAP()
	virtual void UpdatePegType(){}
	virtual void UpdatePegValid(){}

	void UpdatePriceType();
	void UpdateTifType();

	int m_priceHorizontalOffset;
	int m_displaySizeVerticalOffset;
	int m_tifHorizontalOffset;
	int m_horizontalOffset;

	int m_destinationHorizontalalOffset;
	int m_destinationVerticalOffset;

	virtual void VenueChanged(){}
	virtual void RoutingChanged(){}
	virtual void UpdateAllOrNone();
	bool m_allOrNone;

	unsigned char m_pegType;
};
