#pragma once

#include "BaseSampleDialogSettingSendOrder.h"

enum SampleSettingBorrowIds
{
	groupBorrowPriceId = baseSendOrderIdsCount,
	radioBorrowPriceMarketId,
	radioBorrowPriceAbsoluteId,
	spinBorrowPriceId,

	groupBorrowSizeId,
	spinBorrowSizeShareId,

	groupBorrowTifId,
	comboBoxBorrowTifId,

	borrowIdsCount
};

enum BorrowPriceType : unsigned char
{
	BPT_MARKET,
	BPT_ABSOLUTE,

	BPT_Count
};

class BorrowOrderParams : public BaseOrderParams
{
public:
	BorrowOrderParams(
		const unsigned char& priceType = BPT_MARKET,
		const Price& priceAbsolute = Price(0, 0),
		const unsigned int& sizeShares = 0,
		const unsigned char& tifType = TIF_DAY,
		const unsigned __int64& userId = 0,
		const unsigned int& extensionId = 0,
		const unsigned short& userType = 0,
		Account* const& account = nullptr):
		BaseOrderParams(
			userId,
			extensionId,
			userType,
			account),
		m_priceType(priceType),
		m_priceAbsolute(priceAbsolute),
		m_sizeShares(sizeShares),
		m_tifType(tifType)
	{}
	unsigned char GetPriceType() const{return m_priceType;}
	void SetPriceType(unsigned char priceType){m_priceType = priceType;}

	const Price& GetPriceAbsolute() const{return m_priceAbsolute;}
	void SetPriceAbsolute(const Price& price){m_priceAbsolute = price;}

	unsigned int GetSizeShares() const{return m_sizeShares;}
	void SetSizeShares(unsigned int sizeShares){m_sizeShares = sizeShares;}

	unsigned char GetTifType() const{return m_tifType;}
	void SetTifType(unsigned char tifType){m_tifType = tifType;}
protected:
	unsigned char m_priceType;
	Price m_priceAbsolute;

	unsigned int m_sizeShares;

	unsigned char m_tifType;
};

class SampleDialogSettingBorrow : public BaseSampleDialogSettingSendOrder
{
public:
	SampleDialogSettingBorrow(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual unsigned char FillOrderParams(BaseOrderParams& orderParams) const override;
	virtual BaseOrderParams* CreateOrderParams() const override;
	virtual unsigned char SendOrder(Security* stock, const BaseOrderParams& orderParams) override;
	unsigned char GetTifType() const;
	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;
	virtual StringSetting* GetAccountIdSetting() override;
protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	
	CButton m_RadioPriceMarket;
	CButton m_RadioPriceAbsolute;
	UnsignedNumberTypeSpin<unsigned int> m_SpinPrice;
	unsigned char m_priceType;
	void UpdatePriceType();

	UnsignedNumberTypeSpin<unsigned int> m_SpinSizeShare;

	ComboBoxString m_ComboBoxTifType;

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPriceType();
	DECLARE_MESSAGE_MAP()
};