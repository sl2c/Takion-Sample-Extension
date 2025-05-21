#pragma once

#include "BaseSampleDialogSettingOrder.h"

class VenueOrderParams : public OrderParams
{
public:
	VenueOrderParams(
		const unsigned char& side = OCS_BUY,
		const unsigned char& priceType = PT_OFFSET,
		const bool& priceOffsetPercent = false,
		const unsigned char& priceBase = PB_OPPOSITE_SIDE,
		const unsigned char& sizeType = SIT_SHARES,
		const unsigned char& displaySizeType = DST_SHARES,
		const unsigned char& tifType = TIF_DAY,
		const SignedPrice& priceOffset = SignedPrice(0, 0),
		const Price& priceAbsolute = Price(0, 0),
		const unsigned int& sizeShares = 0,
		const unsigned int& minQuantity = 0,
		const bool& allOrNone = false,
		const unsigned int& displaySizeShares = 0,
		const unsigned int& tifMilliseconds = 0,
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
		Account* const& account = nullptr,

		const unsigned char& stopType = OST_NONE,
		const unsigned char& stopBase = OSB_BID,
		const SignedPrice& stopPriceOffset = SignedPrice(0, 0),
		const Price& discretionaryOffset = Price(0, 0)
		):
		OrderParams(side,
			priceType,
			priceOffsetPercent,
			priceBase,
			sizeType,
			displaySizeType,
			tifType,
			priceOffset,
			priceAbsolute,
			sizeShares,
			minQuantity,
			allOrNone,
			displaySizeShares,
			tifMilliseconds,
			destinationName,
			destinationId,
			routingId,
			routingSubType,
			routingName,
			mnemonic,
			reserveSize,
			pegType,
			pegOffset,
			userId,
			extensionId,
			userType,
			account),
		m_stopType(stopType),
		m_stopBase(stopBase),
		m_stopPriceOffset(stopPriceOffset),
//		m_pegType(pegType),
//		m_pegOffset(pegOffset),
		m_discretionaryOffset(discretionaryOffset)
	{}
	
	const unsigned char& GetStopType() const{return m_stopType;}
	void SetStopType(const unsigned char& stopType){m_stopType = stopType;}

	const unsigned char& GetStopBase() const{return m_stopBase;}
	void SetStopBase(const unsigned char& stopBase){m_stopBase = stopBase;}

	const SignedPrice& GetStopPriceOffset() const{return m_stopPriceOffset;}
	void SetStopPriceOffset(const SignedPrice& stopPriceOffset){m_stopPriceOffset = stopPriceOffset;}
/*
	unsigned char GetPegType() const{return m_pegType;}
	void SetPegType(unsigned char pegType){m_pegType = pegType;}

	const SignedPrice& GetPegOffset() const{return m_pegOffset;}
	void SetPegOffset(const SignedPrice& pegOffset){m_pegOffset = pegOffset;}
*/
	const Price& GetDiscretionaryOffset() const{return m_discretionaryOffset;}
	void SetDiscretionaryOffset(const Price& discretionaryOffset){m_discretionaryOffset = discretionaryOffset;}
protected:
	unsigned char m_stopType;
	unsigned char m_stopBase;
	SignedPrice m_stopPriceOffset;
//	unsigned char m_pegType;
//	SignedPrice m_pegOffset;
	Price m_discretionaryOffset;
};

/*These are defined in TakionUtilsApi.h
enum OrderStopType : unsigned char
{
	OST_NONE,
	OST_NATIVE,
	OST_TRAILING,
	OST_SYNTHETIC,

	OST_COUNT
};

enum OrderStopBase : unsigned char
{
	OSB_BID,
	OSB_ASK,
	OSB_MIDPOINT,
	OSB_PRINT,

	OSB_COUNT
};

enum PegType : unsigned char
{
	PEG_NONE,
	PEG_PRIMARY,
	PEG_MIDPOINT,
	PEG_MARKET,

	PEG_TYPE_COUNT
};

*/
class SampleDialogSettingVenue : public BaseSampleDialogSettingOrder
{
public:
	SampleDialogSettingVenue(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
//	virtual HWND GetFirstTabControl() const  override;
	virtual StringSetting* GetAccountIdSetting() override;
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
	virtual void UpdateControls() override;
	virtual unsigned char FillOrderParams(BaseOrderParams& orderParams) const override;
	virtual BaseOrderParams* CreateOrderParams() const override;
	virtual unsigned char SendOrder(Security* stock, const BaseOrderParams& orderParams) override;
protected:
//	virtual void InitControls();
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSelchangeStopType();
	afx_msg void OnSelchangeStopBase();

//	afx_msg void OnAllOrNone();
	
	virtual void UpdatePegType() override;

	UnsignedNumberTypeSpin<unsigned int> m_SpinDiscretionaryOffset;

//	ComboBoxString m_ComboBoxPegType;
//	SignedNumberTypeSpin<int, unsigned int> m_SpinPegOffset;

	ComboBoxString m_ComboBoxStopType;
	ComboBoxString m_ComboBoxStopBase;
	SignedNumberTypeSpin<int> m_SpinStopOffset;

//	UnsignedNumberTypeSpin<unsigned int> m_SpinMinQuantity;
//	CheckBoxOwnerDraw m_CheckBoxAllOrNone;

	virtual void UpdatePegValid() override;
	void UpdateStopTypeValid();
	void UpdateStopBaseValid();

	virtual void UpdateAllOrNone() override;

	unsigned char m_stopType;
	unsigned char m_stopBase;

	virtual void VenueChanged() override;
	virtual void RoutingChanged() override;
};