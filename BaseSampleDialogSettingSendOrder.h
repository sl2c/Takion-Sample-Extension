#pragma once

#include "BaseSampleDialogSettingOption.h"

enum BaseSampleSettingSendOrderIds
{
	groupUserIdId = baseSymbolOptionIdsCount,
	spinUserIdId,

	groupUserTypeId,
	spinUserTypeId,

	groupExtensionId,
	spinExtensionId,

	buttonSendOrderId,

	baseSendOrderIdsCount
};

enum ParamError
{
	SPE_OK,
	SPE_NO_SIDE,
	SPE_NO_PRICE_TYPE,
	SPE_NO_PRICE_BASE,
	SPE_NO_SIZE_TYPE,
	SPE_NO_DISPLAY_SIZE_TYPE,
	SPE_NO_TIF_TYPE,
	SPE_NO_DESTINATION,
	SPE_NO_ACCOUNT,

	SPE_NO_SWIPE_TYPE,

	SPE_Count
};
/*
enum PriceBaseEnum : unsigned char
{
	PB_OPPOSITE_SIDE,
	PB_SAME_SIDE,
	PB_CLOSE_PRICE,
	PB_TODAYS_CLOSE_PRICE,
	PB_NYSE_OPPOSITE_SIDE,
	PB_NYSE_SAME_SIDE,
	PB_AVG_POSITION_PRICE,

	PB_Count
};
*/
class BaseOrderParams
{
public:
	virtual ~BaseOrderParams(){}

	const unsigned __int64& GetUserId() const{return m_userId;}
	void SetUserId(const unsigned __int64& userId){m_userId = userId;}

	const unsigned short& GetUserType() const{return m_userType;}
	void SetUserType(const unsigned short& userType){m_userType = userType;}

	const unsigned int& GetExtensionId() const{return m_extensionId;}
	void SetExtensionId(const unsigned int& extensionId){m_extensionId = extensionId;}

	Account* GetAccount(){return m_account;}
	const Account* GetAccount() const{return m_account;}
	void SetAccount(Account* account){m_account = account;}
protected:
	BaseOrderParams(
		const unsigned __int64& userId = 0,
		const unsigned int& extensionId = 0,
		const unsigned short& userType = 0,
		Account* const& account = nullptr):
		m_userId(userId),
		m_extensionId(extensionId),
		m_userType(userType),
		m_account(account)
	{}
	unsigned __int64 m_userId;
	unsigned int m_extensionId;
	unsigned short m_userType;
	Account* m_account;
};

class BaseSampleDialogSettingSendOrder;

class MyStock : public Observer
{
public:
	MyStock(Security* stock, BaseSampleDialogSettingSendOrder* dlg):
		m_stock(stock),
		m_dlg(dlg)
	{
		m_stock->AddObserver(this);
	}
	virtual ~MyStock(){ClearOrderParamsSet();}
#if (_MSC_VER > 1600)
	typedef std::unordered_set<const BaseOrderParams*> OrderParamsSet;
#else
	typedef std::hash_set<const BaseOrderParams*> OrderParamsSet;
#endif
	void AddOrderParams(const BaseOrderParams* orderParams)
	{
		m_orderParamsSet.insert(orderParams);
	}
	void ClearOrderParamsSet();
	bool ClearAccountOrders(Account* account);
	void SendOrders();
	bool isOrderSetEmpty() const{return m_orderParamsSet.empty();}
protected:
	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
	Security* m_stock;
	BaseSampleDialogSettingSendOrder* m_dlg;
	OrderParamsSet m_orderParamsSet;
};

class BaseSampleDialogSettingSendOrder : public BaseSampleDialogSettingOption//BaseSampleDialogSettingSymbol
{
public:
	virtual ~BaseSampleDialogSettingSendOrder(){ClearStocks();}

	static void AdjustSignedPrice(const SignedPrice& offset, bool priceOffsetPercent, SignedPrice& price, char side);
	static bool AdjustPrice(const SignedPrice& offset, bool priceOffsetPercent, Price& price, char side);
	static unsigned char CalculatePriceByOffset(char side,
		const Security* security,
		const Price& insideBid,
		const Price& insideAsk,
		const Price& positionPrice,
		const Price& positionCost,
		unsigned char priceBase,
		const SignedPrice& priceOffset,
		bool priceOffsetPercent,
		Price& limitPrice);

	virtual unsigned char FillOrderParams(BaseOrderParams& orderParams) const;
	virtual BaseOrderParams* CreateOrderParams() const = 0;
	virtual unsigned char SendOrder(Security* stock, const BaseOrderParams& orderParams) = 0;
	void RemoveStock(Security* stock);
	void ClearStocks();
	void ClearAccountOrders(Account* account);
	virtual bool MustBeSubscribed() const{return false;}
protected:
	BaseSampleDialogSettingSendOrder(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label);
//	virtual void InitControls() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;

	UnsignedNumberTypeSpin<unsigned __int64> m_SpinUserId;
	UnsignedNumberTypeSpin<unsigned short> m_SpinUserType;
	UnsignedNumberTypeSpin<unsigned int> m_SpinExtensionId;
	CButton m_ButtonSendOrder;

//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSendOrder();
	DECLARE_MESSAGE_MAP()

#if (_MSC_VER > 1600)
	typedef std::unordered_map<Security*, MyStock*> PendingStockMap;
#else
	typedef std::hash_map<Security*, MyStock*> PendingStockMap;
#endif
	PendingStockMap m_pendingStockMap;

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;

	Order* m_order;
};
