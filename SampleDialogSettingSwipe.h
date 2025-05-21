#pragma once

#include "BaseSampleDialogSettingOrder.h"
/*TakionUtilsApi.h
enum SwipeType : unsigned char
{
	SWIPE_MEGA,
	SWIPE_DUMB,
	SWIPE_SMART,
	SWIPE_SPIDER,

	SWIPE_TYPE_COUNT
};
*/
class SwipeOrderParams : public OrderParams
{
public:
	SwipeOrderParams(
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

		const bool& skimOddLot = false,
		const bool& postRemainder = false,
		const unsigned char& swipeType = OST_NONE,
		const bool& excludeDestinations = true,
		const bool& proactive = false,
		const unsigned char& postTifType = TIF_DAY,
		const unsigned int& postTifMilliseconds = 0
/*
		unsigned char stopType = OST_NONE,
		unsigned char stopBase = OSB_BID,
		const SignedPrice& stopPriceOffset = SignedPrice(0, 0),
		unsigned char pegType = PEG_NONE,
		const SignedPrice& pegOffset = SignedPrice(0, 0),
		const Price& discretionaryOffset = Price(0, 0)
*/
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
//			proAts,
			userId,
			extensionId,
			userType,
			account),
		m_skimOddLot(skimOddLot),
		m_postRemainder(postRemainder),
		m_swipeType(swipeType),
		m_excludeDestinations(excludeDestinations),
		m_proactive(proactive),
		m_postTifType(postTifType),
		m_postTifMilliseconds(postTifMilliseconds)
/*
		m_stopType(stopType),
		m_stopBase(stopBase),
		m_pegType(pegType),
		m_stopPriceOffset(stopPriceOffset),
		m_pegOffset(pegOffset),
		m_discretionaryOffset(discretionaryOffset)
*/
	{}

	const bool& isSkimOddLot() const{return m_skimOddLot;}
	void SetSkimOddLot(const bool& skimOddLot){m_skimOddLot = skimOddLot;}

	const bool& isPostRemainder() const{return m_postRemainder;}
	void SetPostRemainder(const bool& postRemainder){m_postRemainder = postRemainder;}

	const unsigned char& GetSwipeType() const{return m_swipeType;}
	void SetSwipeType(const unsigned char& swipeType){m_swipeType = swipeType;}

	const bool& isExcludeDestinations() const{return m_excludeDestinations;}
	void SetExcludeDestinations(const bool& excludeDestinations){m_excludeDestinations = excludeDestinations;}

	const bool& isProactive() const{return m_proactive;}
	void SetProactive(const bool& proactive){m_proactive = proactive;}

	const unsigned char& GetPostTifType() const{return m_postTifType;}
	void SetPostTifType(const unsigned char& postTifType){m_postTifType = postTifType;}

	const unsigned int& GetPostTifMilliseconds() const{return m_postTifMilliseconds;}
	void SetPostTifMilliseconds(const unsigned int& postTifMilliseconds){m_postTifMilliseconds = postTifMilliseconds;}

	void ClearDestinationSet(){m_destinationSet.clear();}
	void AddDestination(const unsigned int& destinationName){m_destinationSet.insert(destinationName);}
	const Account::UIntSet& GetDestinationSet() const{return m_destinationSet;}
protected:
	bool m_skimOddLot;
	bool m_postRemainder;
	unsigned char m_swipeType;
	bool m_excludeDestinations;
	bool m_proactive;
	unsigned char m_postTifType;
	unsigned int m_postTifMilliseconds;
	Account::UIntSet m_destinationSet;
};

class SampleDialogSettingSwipe : public BaseSampleDialogSettingOrder
{
public:
	SampleDialogSettingSwipe(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual unsigned char FillOrderParams(BaseOrderParams& orderParams) const override;
	virtual BaseOrderParams* CreateOrderParams() const override;
	virtual unsigned char SendOrder(Security* stock, const BaseOrderParams& orderParams) override;
	unsigned char GetPostTifType() const;
	unsigned int GetPostTifMilliseconds() const;
	virtual StringSetting* GetAccountIdSetting() override;
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
	virtual bool MustBeSubscribed() const{return true;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;

	afx_msg void OnSelchangeSwipeType();
	afx_msg void OnSelchangePostTifType();
	afx_msg void OnExcludeDestinations();
	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);
	afx_msg LRESULT OnListBoxRangeChecked(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

	ComboBoxString m_ComboBoxSwipeType;
	CButton m_CheckBoxProactive;
	CButton m_CheckBoxSwipeExclude;
	ListBoxCheckDestination m_ListBoxDestination;
	unsigned char m_swipeType;
	void DoSetSwipeType(unsigned char swipeType);
	void SwitchSpider(bool spider);

	CButton m_CheckBoxPostRemainder;
	CButton m_CheckBoxSkimOddLot;

	void UpdatePostTifType();
	CStatic m_StaticPostTif;
	ComboBoxString m_ComboBoxPostTifType;
	TifSpin m_SpinPostTif;

	typedef std::list<CWnd*> WindowList;
	WindowList m_spiderOnlyControls;
	WindowList m_nonSpiderOnlyControls;
};