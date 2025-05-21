#include "stdafx.h"
#include "DialogStrategyHiLo.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

class LbHiLoSecurityHorizontalScroller : public LbSymbolHorizontalScroller
{
public:
	LbHiLoSecurityHorizontalScroller(ListBoxSecurityHiLo* listBox):LbSymbolHorizontalScroller(listBox){}
	virtual void AddColumns() override;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ListBoxSecurityHiLo::ListBoxSecurityHiLo(DialogStrategyHiLo* owner, Account* account):
	ListBoxSecurityTimer(owner, account),
	m_dlgHiLo(owner)
{
}

SecurityObserver* ListBoxSecurityHiLo::CreateSecurityObserver(const unsigned __int64& numericSymbol)//, const Security* security, const Position* position)
{
	return new HiLoSecurityObserver(numericSymbol, this);
}

LbHorizontalScroller* ListBoxSecurityHiLo::CreateHorizontalScroller()
{
	return new LbHiLoSecurityHorizontalScroller(this);
}

void LbHiLoSecurityHorizontalScroller::AddColumns()
{
	LbSymbolHorizontalScroller::AddColumns();

	const int priceWidth = m_fiveHeight;
	AddColumn("Hi", ST_HL_HI, priceWidth, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "High Price");
	AddColumn("Lo", ST_HL_LO, priceWidth, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Low Price");
	AddColumn("YHi", ST_HL_Y_HI, priceWidth, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Yesterday's High Price");
	AddColumn("YLo", ST_HL_Y_LO, priceWidth, DT_RIGHT, DT_CENTER, 0xFFFFFFFF, "Yesterday's Low Price");
	AddColumn("YVolume", ST_HL_Y_VOLUME, m_fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Yesterday's Volume");
	AddColumn("LastEnter", ST_HL_LAST_ENTER, m_fiveHeight, DT_RIGHT, DT_RIGHT, 0xFFFFFFFF, "Last Time entered into Position");
}

void ListBoxSecurityHiLo::DrawPrice(const Price& price, HDC hDC, CRect& r, unsigned int alignment)
{
	static char num[33];
	static std::string info;
	if(!price.isZero())
	{
		unsigned int dollars = price.GetDollars();
		unsigned int fraction = price.GetDollarFraction();
		unsigned char decDigits = 4;
		U_RoundUnsignedMonetaryFraction(dollars, fraction, decDigits, Price::decDigits, 0);
		const char* dollarPtr = U_UnsignedNumberToStrWithCommas(dollars, num, sizeof(num), '\0', '.');
		if(DT_RIGHT & alignment)
		{
			const int x = r.right;
			r.right -= m_itemHeight + m_itemHeight;
			DrawText(hDC, dollarPtr, -1, &r, alignment);
			
			const int left = r.left;
			r.left = r.right;
			r.right = x;

			info.clear();
			U_DollarFractionToStdStr(fraction, 4, info, '\0', true);
			alignment &= ~DT_RIGHT;
			alignment |= DT_LEFT;
			DrawText(hDC, info.c_str(), -1, &r, alignment);
			alignment &= ~DT_LEFT;
			alignment |= DT_RIGHT;

			r.left = left;
		}
		else
		{
			info = dollarPtr;
			U_DollarFractionToStdStr(fraction, 4, info, '\0', true);
			DrawText(hDC, info.c_str(), -1, &r, alignment);
		}
	}
}

void ListBoxSecurityHiLo::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
	const HiLoSecurityObserver* securityObserver = (const HiLoSecurityObserver*)item;
	static char num[33];
	static std::string info;
	switch(columnId)
	{
		case ST_HL_HI:
		DrawPrice(securityObserver->GetDayHiPrice(), hDC, r, alignment);
		break;

		case ST_HL_LO:
		DrawPrice(securityObserver->GetDayLoPrice(), hDC, r, alignment);
		break;

		case ST_HL_Y_HI:
		DrawPrice(securityObserver->GetYesterdayHiPrice(), hDC, r, alignment);
		break;

		case ST_HL_Y_LO:
		DrawPrice(securityObserver->GetYesterdayLoPrice(), hDC, r, alignment);
		break;

		case ST_HL_Y_VOLUME:
		DrawText(hDC, U_UnsignedNumberToStrWithCommas(securityObserver->GetYesterdayVolume(), num, sizeof(num)), -1, &r, alignment);
		break;

		case ST_HL_LAST_ENTER:
		if(securityObserver->GetPositionEnterTime())
		{
			U_FormatMillisecond(securityObserver->GetPositionEnterTime(), num, sizeof(num));
			DrawText(hDC, num, -1, &r, alignment);
		}
		break;

		default:
		ListBoxSymbol::DrawLine(columnId, item, itemOrdinal, hDC, r, alignment);
		break;
	}
}

void HiLoSecurityObserver::RememberSecurityValues()
{
	TimerSecurityObserver::RememberSecurityValues();
	m_dayHiPriceNew = m_security->GetMarketHoursHighPrice();//GetHighPrice();
	m_dayLoPriceNew = m_security->GetMarketHoursLowPrice();//GetLowPrice();
}

void HiLoSecurityObserver::RememberSecurityConstValues()
{
	TimerSecurityObserver::RememberSecurityConstValues();
	m_yesterdayHiPriceNew = m_security->GetYesterdaysHighPrice();
	m_yesterdayLoPriceNew = m_security->GetYesterdaysLowPrice();
	m_yesterdayVolumeNew = m_security->GetYesterdaysVolume();
}

void HiLoSecurityObserver::NullifySecurityValues()
{
	TimerSecurityObserver::NullifySecurityValues();
	m_dayHiPriceNew.SetZero();
	m_dayLoPriceNew.SetZero();
}

void HiLoSecurityObserver::NullifySecurityConstValues()
{
	TimerSecurityObserver::NullifySecurityConstValues();
	m_yesterdayHiPriceNew.SetZero();
	m_yesterdayLoPriceNew.SetZero();
	m_yesterdayVolumeNew = 0;
}

bool HiLoSecurityObserver::DoUpdateSecurityConstValues()
{
	bool ret = false;
	LbHorizontalScroller* scroller = m_listBoxSymbol->GetHorizontalScroller();
	if(m_yesterdayHiPrice != m_yesterdayHiPriceNew)
	{
		ret = true;
		m_yesterdayHiPrice = m_yesterdayHiPriceNew;
		if(scroller)scroller->InvalidateItemColumn(m_ordinal, ST_HL_Y_HI);
	}
	if(m_yesterdayLoPrice != m_yesterdayLoPriceNew)
	{
		ret = true;
		m_yesterdayLoPrice = m_yesterdayLoPriceNew;
		if(scroller)scroller->InvalidateItemColumn(m_ordinal, ST_HL_Y_LO);
	}
	if(m_yesterdayVolume != m_yesterdayVolumeNew)
	{
		ret = true;
		m_yesterdayVolume = m_yesterdayVolumeNew;
		if(scroller)scroller->InvalidateItemColumn(m_ordinal, ST_HL_Y_VOLUME);
	}
	return ret;
}

BoolSetting* DialogStrategyHiLo::GetSettingTradeOnStart(){return &extensionConfig->m_strategyHiLoTradeOnStart;}
BoolSetting* DialogStrategyHiLo::GetSettingLoadOnStart(){return &extensionConfig->m_strategyHiLoLoadOnStart;}
BoolSetting* DialogStrategyHiLo::GetSettingLoadFromFile(){return &extensionConfig->m_strategyHiLoLoadFromFile;}
StringSetting* DialogStrategyHiLo::GetSettingStockFilePath(){return &extensionConfig->m_strategyHiLoStockFilePath;}
UCharSetting* DialogStrategyHiLo::GetSettingSymbolDelimiter(){return &extensionConfig->m_strategyHiLoSymbolDelimiter;}
StringSetting* DialogStrategyHiLo::GetAccountIdSetting(){return &extensionConfig->m_strategyHiLoAccountId;}
UIntSetting* DialogStrategyHiLo::GetSettingMillisecondEntry(){return &extensionConfig->m_strategyHiLoMillisecondEntry;}
UIntSetting* DialogStrategyHiLo::GetSettingMillisecondStop(){return &extensionConfig->m_strategyHiLoMillisecondStop;}
UIntSetting* DialogStrategyHiLo::GetSettingMillisecondExit(){return &extensionConfig->m_strategyHiLoMillisecondExit;}
UIntSetting* DialogStrategyHiLo::GetSettingMillisecondStopExit(){return &extensionConfig->m_strategyHiLoMillisecondStopExit;}

DialogStrategyHiLo::DialogStrategyHiLo(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
//	BaseDialogSettingStrategyTimer(mainWnd, parentTab, "HiLo", &m_ListBoxHiLo, false, false, 73, 73 + 73 + hgap),
	BaseDialogSettingStrategyTimer(mainWnd, parentTab, "HiLo", &m_ListBoxHiLo, false, false, 88, 88 + 88 + hgap),
	m_SpinYesterdayVolume(999999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinMinPrice(99999999, 0, 0, 1000, 3, TakionSpin::charComma, 2),//, TakionSpin::charDot),
	m_SpinMaxPrice(99999999, 0, 0, 1000, 3, TakionSpin::charComma, 2),//, TakionSpin::charDot),
	m_SpinOrderSize(10000, 0, 0, 100, 3, TakionSpin::charComma),
	m_EditEnterMnemonic("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", false),
	m_EditExitMnemonic("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", false),
	m_SpinEnterReserveSize(100, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinExitReserveSize(100, 0, 0, 1000, 3, TakionSpin::charComma),
	m_ListBoxHiLo(this, nullptr),

	m_controlEnterDestination(nullptr),
	m_currentEnterDestination(nullptr),
	m_controlEnterRouting(nullptr),
	m_currentEnterRouting(nullptr),

	m_controlExitDestination(nullptr),
	m_currentExitDestination(nullptr),
	m_controlExitRouting(nullptr),
	m_currentExitRouting(nullptr),

	m_controlEnterVenue(0),
	m_enterVenue(0),
	m_controlEnterRoutingName(0),
	m_enterRoutingName(0),
	m_controlEnterMnemonic(0),
	m_enterMnemonic(0),
	m_controlEnterReserveSize(0),
	m_enterReserveSize(0),

	m_controlExitVenue(0),
	m_exitVenue(0),
	m_controlExitRoutingName(0),
	m_exitRoutingName(0),
	m_controlExitMnemonic(0),
	m_exitMnemonic(0),
	m_controlExitReserveSize(0),
	m_exitReserveSize(0),

	m_controlYesterdayVolume(0),
	m_yesterdayVolume(0),

	m_controlOrderSize(0),
	m_orderSize(0),

	m_exchangeDestination(" xch",
#ifndef TAKION_NO_OPTIONS
		nullptr, NSDQ_FINRA,
#endif
		NULL_ROUTE, NSDQ_FINRA, MT_NONE, false, SH_ALWAYS, SE_NO, STE_NO, true, 0, 0, 0, 0)//, nullptr, nullptr)
{
	m_exchangeDestination.AddRouting(TD_GetDefaultProActiveRoutingName(), RI_PROACTIVE, 0, "Default Proactive");

	m_ComboBoxEnterDestination.SetItemToolTips(true);
	m_ComboBoxEnterRouting.SetItemToolTips(true);
	m_ComboBoxExitDestination.SetItemToolTips(true);
	m_ComboBoxExitRouting.SetItemToolTips(true);

	m_SpinYesterdayVolume.AddSpinCell(1, false, 1);
	m_SpinYesterdayVolume.AddSpinCell(100, false, 100);
	m_SpinYesterdayVolume.AddSpinCell(1000, false, 1000);
	m_SpinYesterdayVolume.AddSpinCell(10000000, true, 10000000);
	m_SpinYesterdayVolume.AddSpinCell(100000, true, 100000);

	SetBigPriceSpinCells(m_SpinMinPrice);
	SetBigPriceSpinCells(m_SpinMaxPrice);

	m_SpinOrderSize.AddSpinCell(1, false);
	m_SpinOrderSize.AddSpinCell(100, true);

	m_SpinEnterReserveSize.AddSpinCell(1, false);
	m_SpinEnterReserveSize.AddSpinCell(100, true);

	m_SpinExitReserveSize.AddSpinCell(1, false);
	m_SpinExitReserveSize.AddSpinCell(100, true);

	m_ComboBoxEnterDestination.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxEnterDestination.SetColorInvalid(m_visualBase->GetPendingColor());
	m_ComboBoxEnterRouting.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxEnterRouting.SetColorInvalid(m_visualBase->GetPendingColor());

	m_ComboBoxExitDestination.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxExitDestination.SetColorInvalid(m_visualBase->GetPendingColor());
	m_ComboBoxExitRouting.SetNoSelectionColor(m_visualBase->GetWarningColor());
	m_ComboBoxExitRouting.SetColorInvalid(m_visualBase->GetPendingColor());

	m_xc += m_accountWidth + hgap;

//	m_bottomControl += vgap;
	int y = m_topLineY;//m_bottomControl;

	int yTop = y;

	int w = m_accountWidth - hgap - hgap;
	int buttonWidth = (w - hgap) / 2;

	const int filterGroupHeight = groupOffset + 3 * (staticHeight + spinHeight + vgap) + editHeight + hgap;//30;

	AddButtonIndirect("Filter", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, m_xc - hgap, y, m_accountWidth, filterGroupHeight, groupHiloFilterId);
	AddStaticIndirect("Yesterday Volume >=", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y += groupOffset, w, staticHeight, staticHiLoYesterdayVolumeId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y += staticHeight, w, spinHeight, spinHiLoYesterdayVolumeId);
	AddStaticIndirect("Price >=", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y += spinHeight + vgap, w, staticHeight, staticHiLoMinPriceId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y += staticHeight, w, spinHeight, spinHiLoMinPriceId);
	AddStaticIndirect("Price <=", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y += spinHeight + vgap, w, staticHeight, staticHiLoMaxPriceId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y += staticHeight, w, spinHeight, spinHiLoMaxPriceId);
	AddButtonIndirect("Apply", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, m_xc, y += spinHeight + vgap, buttonWidth, editHeight, buttonHiLoFilterApplyId);
	AddButtonIndirect("Reset", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, m_xc + buttonWidth + hgap, y, buttonWidth, editHeight, buttonHiLoFilterResetId);

	y = yTop + filterGroupHeight + vgap;

	int destWidth = 40;
	int destNameWidth = m_accountWidth - destWidth - hgap;
	int xVenueName = m_xc + destWidth;

	const int orderSizeGroupHeight = groupOffset + spinHeight + vgap + editHeight + hgap;//30;

	yTop = y;
	AddButtonIndirect("Order Params", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, m_xc - hgap, y, m_accountWidth, orderSizeGroupHeight, groupHiloOrderSizeId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y += groupOffset, destWidth, spinHeight, spinHiLoOrderSizeId);
	AddStaticIndirect("Size", WS_VISIBLE | SS_NOPREFIX, 0, xVenueName, y + (spinHeight - staticHeight) / 2, destNameWidth, staticHeight, staticHiLoOrderSizeId);
	AddButtonIndirect("Apply", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, m_xc, y += spinHeight + vgap, buttonWidth, editHeight, buttonHiLoOrderSizeApplyId);
	AddButtonIndirect("Reset", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, m_xc + buttonWidth + hgap, y, buttonWidth, editHeight, buttonHiLoOrderSizeResetId);

	y = yTop + orderSizeGroupHeight + vgap;

	const int venueGroupHeight = groupOffset + 3 * vgap + 2 * comboBoxHeight + spinHeight + editHeight + vgap + editHeight + hgap;//30;
	destWidth = 32;
	destNameWidth = m_accountWidth - destWidth - hgap;
	xVenueName = m_xc + destWidth;

	yTop = y;

	AddButtonIndirect("Enter Venue", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, m_xc - hgap, y, m_accountWidth, venueGroupHeight, groupHiLoEnterId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += groupOffset, destWidth, 400, comboBoxHiLoEnterDestinationId);
	AddStaticIndirect("Venue", WS_VISIBLE | SS_NOPREFIX, 0, xVenueName, y + (comboBoxHeight - staticHeight) / 2, destNameWidth, staticHeight, staticHiLoEnterDestinationId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += comboBoxHeight + vgap, destWidth, 400, comboBoxHiLoEnterRoutingId);
	AddStaticIndirect("Routing", WS_VISIBLE | SS_NOPREFIX, 0, xVenueName, y + (comboBoxHeight - staticHeight) / 2, destNameWidth, staticHeight, staticHiLoEnterRoutingId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, y += comboBoxHeight + vgap, destWidth, editHeight, editHiLoEnterMnemonicId);
	AddStaticIndirect("Mnemonic", WS_VISIBLE | SS_NOPREFIX, 0, xVenueName, y + (editHeight - staticHeight) / 2, destNameWidth, staticHeight, staticHiLoEnterMnemonicId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y += editHeight + vgap, destWidth, spinHeight, spinHiLoEnterReserveSizeId);
	AddStaticIndirect("Reserve", WS_VISIBLE | SS_NOPREFIX, 0, xVenueName, y + (spinHeight - staticHeight) / 2, destNameWidth, staticHeight, staticHiLoEnterReserveSizeId);
	AddButtonIndirect("Apply", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, m_xc, y += spinHeight + vgap, buttonWidth, editHeight, buttonHiLoEnterVenueApplyId);
	AddButtonIndirect("Reset", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, m_xc + buttonWidth + hgap, y, buttonWidth, editHeight, buttonHiLoEnterVenueResetId);

	y = yTop + venueGroupHeight + vgap;

	AddButtonIndirect("Exit Venue", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, m_xc - hgap, y, m_accountWidth, venueGroupHeight, groupHiLoExitId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += groupOffset, destWidth, 400, comboBoxHiLoExitDestinationId);
	AddStaticIndirect("Venue", WS_VISIBLE | SS_NOPREFIX, 0, xVenueName, y + (comboBoxHeight - staticHeight) / 2, destWidth, staticHeight, staticHiLoExitDestinationId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += comboBoxHeight + vgap, destWidth, 400, comboBoxHiLoExitRoutingId);
	AddStaticIndirect("Routing", WS_VISIBLE | SS_NOPREFIX, 0, xVenueName, y + (comboBoxHeight - staticHeight) / 2, destWidth, staticHeight, staticHiLoExitRoutingId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, m_xc, y += comboBoxHeight + vgap, destWidth, editHeight, editHiLoExitMnemonicId);
	AddStaticIndirect("Mnemonic", WS_VISIBLE | SS_NOPREFIX, 0, xVenueName, y + (editHeight - staticHeight) / 2, destWidth, staticHeight, staticHiLoExitMnemonicId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, m_xc, y += editHeight + vgap, destWidth, spinHeight, spinHiLoExitReserveSizeId);
	AddStaticIndirect("Reserve", WS_VISIBLE | SS_NOPREFIX, 0, xVenueName, y + (spinHeight - staticHeight) / 2, destWidth, staticHeight, staticHiLoExitReserveSizeId);
	AddButtonIndirect("Apply", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, m_xc, y += spinHeight + vgap, buttonWidth, editHeight, buttonHiLoExitVenueApplyId);
	AddButtonIndirect("Reset", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, m_xc + buttonWidth + hgap, y, buttonWidth, editHeight, buttonHiLoExitVenueResetId);
}
/*
void DialogStrategyHiLo::SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(10000, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(100000, true);
}
*/
void DialogStrategyHiLo::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingStrategyTimer::DoDataExchange(pDX);
	DDX_Control(pDX, spinHiLoYesterdayVolumeId, m_SpinYesterdayVolume);
	DDX_Control(pDX, spinHiLoMinPriceId, m_SpinMinPrice);
	DDX_Control(pDX, spinHiLoMaxPriceId, m_SpinMaxPrice);
	DDX_Control(pDX, buttonHiLoFilterApplyId, m_ButtonFilterApply);
	DDX_Control(pDX, buttonHiLoFilterResetId, m_ButtonFilterReset);

	DDX_Control(pDX, spinHiLoOrderSizeId, m_SpinOrderSize);
	DDX_Control(pDX, buttonHiLoOrderSizeApplyId, m_ButtonOrderSizeApply);
	DDX_Control(pDX, buttonHiLoOrderSizeResetId, m_ButtonOrderSizeReset);

	DDX_Control(pDX, comboBoxHiLoEnterDestinationId, m_ComboBoxEnterDestination);
	DDX_Control(pDX, comboBoxHiLoEnterRoutingId, m_ComboBoxEnterRouting);
	DDX_Control(pDX, editHiLoEnterMnemonicId, m_EditEnterMnemonic);
	DDX_Control(pDX, spinHiLoEnterReserveSizeId, m_SpinEnterReserveSize);
	DDX_Control(pDX, staticHiLoEnterReserveSizeId, m_StaticEnterReserveSize);
	DDX_Control(pDX, buttonHiLoEnterVenueApplyId, m_ButtonEnterVenueApply);
	DDX_Control(pDX, buttonHiLoEnterVenueResetId, m_ButtonEnterVenueReset);

	DDX_Control(pDX, comboBoxHiLoExitDestinationId, m_ComboBoxExitDestination);
	DDX_Control(pDX, comboBoxHiLoExitRoutingId, m_ComboBoxExitRouting);
	DDX_Control(pDX, editHiLoExitMnemonicId, m_EditExitMnemonic);
	DDX_Control(pDX, spinHiLoExitReserveSizeId, m_SpinExitReserveSize);
	DDX_Control(pDX, staticHiLoExitReserveSizeId, m_StaticExitReserveSize);
	DDX_Control(pDX, buttonHiLoExitVenueApplyId, m_ButtonExitVenueApply);
	DDX_Control(pDX, buttonHiLoExitVenueResetId, m_ButtonExitVenueReset);
}

BEGIN_MESSAGE_MAP(DialogStrategyHiLo, BaseDialogSettingStrategyTimer)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(buttonHiLoFilterApplyId, OnFilterApply)
	ON_BN_CLICKED(buttonHiLoFilterResetId, OnFilterReset)

	ON_BN_CLICKED(buttonHiLoOrderSizeApplyId, OnOrderSizeApply)
	ON_BN_CLICKED(buttonHiLoOrderSizeResetId, OnOrderSizeReset)

	ON_CBN_SELCHANGE(comboBoxHiLoEnterDestinationId, OnSelchangeEnterDestination)
	ON_CBN_SELCHANGE(comboBoxHiLoEnterRoutingId, OnSelchangeEnterRouting)
	ON_EN_CHANGE(editHiLoEnterMnemonicId, OnEnterMnemonicChange)
	ON_EN_UPDATE(editHiLoEnterMnemonicId, OnEnterMnemonicUpdate)
	ON_BN_CLICKED(buttonHiLoEnterVenueApplyId, OnEnterVenueApply)
	ON_BN_CLICKED(buttonHiLoEnterVenueResetId, OnEnterVenueReset)

	ON_CBN_SELCHANGE(comboBoxHiLoExitDestinationId, OnSelchangeExitDestination)
	ON_CBN_SELCHANGE(comboBoxHiLoExitRoutingId, OnSelchangeExitRouting)
	ON_EN_CHANGE(editHiLoExitMnemonicId, OnExitMnemonicChange)
	ON_EN_UPDATE(editHiLoExitMnemonicId, OnExitMnemonicUpdate)
	ON_BN_CLICKED(buttonHiLoExitVenueApplyId, OnExitVenueApply)
	ON_BN_CLICKED(buttonHiLoExitVenueResetId, OnExitVenueReset)

	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void DialogStrategyHiLo::EnableEnterVenueButtons()
{
	bool enable = m_controlEnterDestination != m_currentEnterDestination
		|| m_controlEnterRouting != m_currentEnterRouting
		|| m_controlEnterMnemonic != m_enterMnemonic
		|| m_controlEnterReserveSize != m_enterReserveSize;
	EnableControl(m_ButtonEnterVenueApply.m_hWnd, enable, m_ListBoxHiLo.m_hWnd);
	EnableControl(m_ButtonEnterVenueReset.m_hWnd, enable, m_ListBoxHiLo.m_hWnd);
}

void DialogStrategyHiLo::EnableExitVenueButtons()
{
	bool enable = m_controlExitDestination != m_currentExitDestination
		|| m_controlExitRouting != m_currentExitRouting
		|| m_controlExitMnemonic != m_exitMnemonic
		|| m_controlExitReserveSize != m_exitReserveSize;
	EnableControl(m_ButtonExitVenueApply.m_hWnd, enable, m_ListBoxHiLo.m_hWnd);
	EnableControl(m_ButtonExitVenueReset.m_hWnd, enable, m_ListBoxHiLo.m_hWnd);
}

void DialogStrategyHiLo::OnEnterVenueApply()
{
	m_currentEnterDestination = m_controlEnterDestination;
	m_currentEnterRouting = m_controlEnterRouting;
	m_enterVenue = m_controlEnterVenue;
	m_enterRoutingName = m_controlEnterRoutingName;
	m_enterMnemonic = m_controlEnterMnemonic;
	m_enterReserveSize = m_controlEnterReserveSize;
	ResetEnterVenueControls();
}

void DialogStrategyHiLo::OnEnterVenueReset()
{
	int found = m_ComboBoxEnterDestination.FindItemByNumericName(m_enterVenue);
	m_ComboBoxEnterDestination.SetCurSel(found);
	OnSelchangeEnterDestination();

	found = m_ComboBoxEnterRouting.FindItemByNumericName(m_enterRoutingName);
	m_ComboBoxEnterRouting.SetCurSel(found);
	OnSelchangeEnterRouting();

	m_EditEnterMnemonic.SetTextAsUInt(m_enterMnemonic);
		
	m_SpinEnterReserveSize.SetValue(m_enterReserveSize);
}

void DialogStrategyHiLo::ResetEnterVenueControls()
{
	m_ComboBoxEnterDestination.SetInvalid(false);
	m_ComboBoxEnterRouting.SetInvalid(false);
	m_EditEnterMnemonic.Invalidate(FALSE);
	m_SpinEnterReserveSize.SetBkColorEnabled(m_SpinEnterReserveSize.hasRange() ? 0xFFFFFFFF : m_visualBase->GetGrayColor());
	EnableControl(m_ButtonEnterVenueApply.m_hWnd, false, m_ListBoxHiLo.m_hWnd);
	EnableControl(m_ButtonEnterVenueReset.m_hWnd, false, m_ListBoxHiLo.m_hWnd);
}

void DialogStrategyHiLo::ResetExitVenueControls()
{
	m_ComboBoxExitDestination.SetInvalid(false);
	m_ComboBoxExitRouting.SetInvalid(false);
	m_EditExitMnemonic.Invalidate(FALSE);
	m_SpinExitReserveSize.SetBkColorEnabled(m_SpinExitReserveSize.hasRange() ? 0xFFFFFFFF : m_visualBase->GetGrayColor());
	EnableControl(m_ButtonExitVenueApply.m_hWnd, false, m_ListBoxHiLo.m_hWnd);
	EnableControl(m_ButtonExitVenueReset.m_hWnd, false, m_ListBoxHiLo.m_hWnd);
}

void DialogStrategyHiLo::OnExitVenueApply()
{
	m_currentExitDestination = m_controlExitDestination;
	m_currentExitRouting = m_controlExitRouting;
	m_exitVenue = m_controlExitVenue;
	m_exitRoutingName = m_controlExitRoutingName;
	m_exitMnemonic = m_controlExitMnemonic;
	m_exitReserveSize = m_controlExitReserveSize;
	ResetExitVenueControls();
}

void DialogStrategyHiLo::OnExitVenueReset()
{
	int found = m_ComboBoxExitDestination.FindItemByNumericName(m_exitVenue);
	m_ComboBoxExitDestination.SetCurSel(found);
	OnSelchangeExitDestination();

	found = m_ComboBoxExitRouting.FindItemByNumericName(m_exitRoutingName);
	m_ComboBoxExitRouting.SetCurSel(found);
	OnSelchangeExitRouting();

	m_EditExitMnemonic.SetTextAsUInt(m_exitMnemonic);
		
	m_SpinExitReserveSize.SetValue(m_exitReserveSize);
}

void DialogStrategyHiLo::OnEnterMnemonicUpdate()
{
	m_EditEnterMnemonic.TryUpdate();
}

bool DialogStrategyHiLo::OnMnemonicChange(CEdit& edit, unsigned int& controlMnemonic, const unsigned int currentMnemonic)
{
	CString text;
	edit.GetWindowText(text);
	const unsigned int mnemonic = U_RepresentStringAsUnsignedNumber<unsigned int>(text);
	if(mnemonic != controlMnemonic)
	{
		if((controlMnemonic == currentMnemonic) != (mnemonic == currentMnemonic))
		{
			edit.Invalidate(FALSE);
		}
		controlMnemonic = mnemonic;
		return true;
	}
	return false;
}

void DialogStrategyHiLo::OnEnterMnemonicChange()
{
	if(OnMnemonicChange(m_EditEnterMnemonic, m_controlEnterMnemonic, m_enterMnemonic))
	{
		EnableEnterVenueButtons();
		m_modified = true;
	}
}

void DialogStrategyHiLo::OnSelchangeEnterDestination()
{
	m_controlEnterVenue = m_controlEnterDestination ? m_controlEnterDestination->GetMainNumericName() : 0;
//	m_ComboBoxEnterDestination.SetTooltipText(m_controlEnterDestination == &m_exchangeDestination ? "Primary Exchange Venue" : nullptr);
	if(UpdateDestinationRoutingStatic(m_controlEnterDestination, m_currentEnterDestination, m_ComboBoxEnterDestination, m_ComboBoxEnterRouting, &m_EditEnterMnemonic))
	{
		m_ComboBoxEnterDestination.SetInvalid(m_controlEnterDestination != m_currentEnterDestination);
		OnSelchangeEnterRouting();
		UpdateReserveSize(m_SpinEnterReserveSize, m_controlEnterReserveSize, m_enterReserveSize);
		EnableEnterVenueButtons();
		m_modified = true;
	}
}

void DialogStrategyHiLo::OnSelchangeEnterRouting()
{
	unsigned int routingName;
	m_controlEnterRouting = OnSelchangeRoutingStatic(m_ComboBoxEnterRouting, m_SpinEnterReserveSize, m_StaticEnterReserveSize, &routingName);
	if(routingName != m_controlEnterRoutingName)
	{
		m_controlEnterRoutingName = routingName;
//		m_ComboBoxEnterRouting.SetToolTipText(m_controlEnterRouting ? m_controlEnterRouting->GetDescription().c_str() : nullptr);
		m_ComboBoxEnterRouting.SetInvalid(m_controlEnterRoutingName != m_enterRoutingName);
		EnableEnterVenueButtons();
		UpdateReserveSize(m_SpinEnterReserveSize, m_controlEnterReserveSize, m_enterReserveSize);
		m_modified = true;
	}
}

void DialogStrategyHiLo::OnExitMnemonicUpdate()
{
	m_EditExitMnemonic.TryUpdate();
}

void DialogStrategyHiLo::OnExitMnemonicChange()
{
	if(OnMnemonicChange(m_EditExitMnemonic, m_controlExitMnemonic, m_exitMnemonic))
	{
		EnableExitVenueButtons();
		m_modified = true;
	}
}

void DialogStrategyHiLo::OnSelchangeExitDestination()
{
	m_controlExitVenue = m_controlExitDestination ? m_controlExitDestination->GetMainNumericName() : 0;
//	m_ComboBoxExitDestination.SetTooltipText(m_controlExitDestination == &m_exchangeDestination ? "Primary Exchange Venue" : nullptr);
	if(UpdateDestinationRoutingStatic(m_controlExitDestination, m_currentExitDestination, m_ComboBoxExitDestination, m_ComboBoxExitRouting, &m_EditExitMnemonic))
	{
//		m_controlExitVenue = m_controlExitDestination ? m_controlExitDestination->GetMainNumericName() : 0;
//		m_ComboBoxExitDestination.SetTooltipText(m_controlExitDestination == &m_exchangeDestination ? "Primary Exchange Venue" : nullptr);
		m_ComboBoxExitDestination.SetInvalid(m_controlExitDestination != m_currentExitDestination);
		OnSelchangeExitRouting();
		UpdateReserveSize(m_SpinExitReserveSize, m_controlExitReserveSize, m_exitReserveSize);
		EnableExitVenueButtons();
		m_modified = true;
	}
}

void DialogStrategyHiLo::OnSelchangeExitRouting()
{
	unsigned int routingName;
	m_controlExitRouting = OnSelchangeRoutingStatic(m_ComboBoxExitRouting, m_SpinExitReserveSize, m_StaticExitReserveSize, &routingName);
	if(routingName != m_controlExitRoutingName)
	{
		m_controlExitRoutingName = routingName;
//		m_ComboBoxExitRouting.SetToolTipText(m_controlExitRouting ? m_controlExitRouting->GetDescription().c_str() : nullptr);
		m_ComboBoxExitRouting.SetInvalid(m_controlExitRoutingName != m_exitRoutingName);
		UpdateReserveSize(m_SpinExitReserveSize, m_controlExitReserveSize, m_exitReserveSize);
		EnableExitVenueButtons();
		m_modified = true;
	}
}

void DialogStrategyHiLo::OnFilterApply()
{
	bool added = false;
	bool removed = false;
	bool changed = false;
	if(m_controlYesterdayVolume != m_yesterdayVolume)
	{
		changed = true;
		if(m_controlYesterdayVolume < m_yesterdayVolume)
		{
			added = true;
		}
		else
		{
			removed = true;
		}
		m_yesterdayVolume = m_controlYesterdayVolume;
	}

	if(m_controlMinPrice != m_minPrice)
	{
		changed = true;
		m_minPrice = m_controlMinPrice;
	}
	if(m_controlMaxPrice != m_maxPrice)
	{
		changed = true;
		m_maxPrice = m_controlMaxPrice;
	}
	if(changed)
	{
		changed = false;
//		m_modified = true;
		EnableFilterButtons();
	}
	if(removed)
	{
		HiLoSecurityObserver* item;
		for(unsigned int i = 0, end = m_ListBoxHiLo.GetItemCount(); i < end;)
		{
			item = (HiLoSecurityObserver*)m_ListBoxHiLo.GetItemDataPtr(i);
			if(item->GetYesterdayVolume() < m_yesterdayVolume)
			{
				changed = true;
				m_ListBoxHiLo.DoRemoveItemAt(i, item);
				--end;
			}
			else
			{
				++i;
			}
		}
	}
	if(added)
	{
		bool securityAdded;
		if(m_loadFromFile)
		{
		}
		else
		{
			TakionIterator* iterator = TD_CreateEquityIterator();
			if(iterator)
			{
				const Security* security;
				TD_LockEquityStorage();
				iterator->Reset();
				while(security = TD_GetNextEquity(iterator))
				{
					m_listBoxSymbol->AddSecurity(security, nullptr, false, &securityAdded);
					if(securityAdded)
					{
						changed = true;
					}
				}
				TD_UnlockEquityStorage();
				delete iterator;
			}
		}
	}
	if(changed)
	{
		m_listBoxSymbol->RefreshOrdinals();
		RepaintCount();
	}
}

void DialogStrategyHiLo::OnFilterReset()
{
	bool changed = false;
	if(m_controlYesterdayVolume != m_yesterdayVolume)
	{
		m_controlYesterdayVolume = m_yesterdayVolume;
		changed = true;
		m_SpinYesterdayVolume.SetValue(m_controlYesterdayVolume, false, true, false);
	}
	if(m_controlMinPrice != m_minPrice)
	{
		m_controlMinPrice = m_minPrice;
		changed = true;
		FillSpinControlFromPrice(m_controlMinPrice, m_SpinMinPrice, 100, false);
	}
	if(m_controlMaxPrice != m_maxPrice)
	{
		m_controlMaxPrice = m_maxPrice;
		changed = true;
		FillSpinControlFromPrice(m_controlMaxPrice, m_SpinMaxPrice, 100, false);
	}
	if(changed)
	{
		m_modified = true;
		EnableFilterButtons();
	}
}

void DialogStrategyHiLo::EnableFilterButtons()
{
	bool enable = false;
	if(m_controlYesterdayVolume != m_yesterdayVolume)
	{
		enable = true;
	}
	m_SpinYesterdayVolume.SetBkColorEnabled(enable ? m_visualBase->GetPendingColor() : 0xFFFFFFFF);

	if(m_controlMinPrice > m_controlMaxPrice || m_controlMaxPrice.isZero())
	{
		m_SpinMinPrice.SetBkColorEnabled(m_visualBase->GetWarningColor());
		m_SpinMaxPrice.SetBkColorEnabled(m_visualBase->GetWarningColor());
	}
	else
	{
		if(m_controlMinPrice != m_minPrice)
		{
			enable = true;
//			m_SpinMinPrice.SetBkColorEnabled(enable ? m_visualBase->GetPendingColor() : 0xFFFFFFFF);
		}
		if(m_controlMaxPrice != m_maxPrice)
		{
			enable = true;
//			m_SpinMaxPrice.SetBkColorEnabled(enable ? m_visualBase->GetPendingColor() : 0xFFFFFFFF);
		}
		m_SpinMinPrice.SetBkColorEnabled(enable ? m_visualBase->GetPendingColor() : 0xFFFFFFFF);
		m_SpinMaxPrice.SetBkColorEnabled(enable ? m_visualBase->GetPendingColor() : 0xFFFFFFFF);
	}
	EnableControl(m_ButtonFilterApply.m_hWnd, enable, m_ListBoxHiLo.m_hWnd);
	EnableControl(m_ButtonFilterReset.m_hWnd, enable, m_ListBoxHiLo.m_hWnd);
}

void DialogStrategyHiLo::OnOrderSizeApply()
{
	if(m_controlOrderSize != m_orderSize)
	{
		m_orderSize = m_controlOrderSize;
//		m_modified = true;
		EnableOrderSizeButtons();
	}
}

void DialogStrategyHiLo::OnOrderSizeReset()
{
	if(m_controlOrderSize != m_orderSize)
	{
		m_controlOrderSize = m_orderSize;
		m_modified = true;
		m_SpinOrderSize.SetValue(m_controlOrderSize, false, true, false);
		EnableOrderSizeButtons();
	}
}

void DialogStrategyHiLo::EnableOrderSizeButtons()
{
	bool enable = m_controlOrderSize != m_orderSize;
	EnableControl(m_ButtonOrderSizeApply.m_hWnd, enable, m_ListBoxHiLo.m_hWnd);
	EnableControl(m_ButtonOrderSizeReset.m_hWnd, enable, m_ListBoxHiLo.m_hWnd);

	m_SpinOrderSize.SetBkColorEnabled(m_controlOrderSize ? enable ? m_visualBase->GetPendingColor() : 0xFFFFFFFF : m_visualBase->GetWarningColor());
}

bool DialogStrategyHiLo::UpdateReserveSize(UnsignedNumberTypeSpin<unsigned int>& spin, unsigned int& controlReserveSize, const unsigned int& currentReserveSize)
{
	const unsigned int reserveSize = spin.GetValue();
	if(reserveSize != controlReserveSize)
	{
		const bool wasCurrent = controlReserveSize == currentReserveSize;
		controlReserveSize = reserveSize;
		if(wasCurrent != (controlReserveSize == currentReserveSize))
		{
			spin.SetBkColorEnabled(spin.hasRange() ? wasCurrent ? m_visualBase->GetPendingColor() : 0xFFFFFFFF : m_visualBase->GetGrayColor());
		}
		return true;
	}
	return false;
}

LRESULT DialogStrategyHiLo::OnSpinChange(WPARAM spinId, LPARAM l)
{
	switch(spinId)
	{
		case spinHiLoYesterdayVolumeId:
		{
			const unsigned __int64& volume = m_SpinYesterdayVolume.GetValue();
			if(volume != m_controlYesterdayVolume)
			{
				m_controlYesterdayVolume = volume;
				m_modified = true;
				EnableFilterButtons();
			}
		}
		break;

		case spinHiLoMinPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinMinPrice, 100);
			if(price != m_controlMinPrice)
			{
				m_controlMinPrice = price;
				m_modified = true;
				EnableFilterButtons();
			}
		}
		break;

		case spinHiLoMaxPriceId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinMaxPrice, 100);
			if(price != m_controlMaxPrice)
			{
				m_controlMaxPrice = price;
				m_modified = true;
				EnableFilterButtons();
			}
		}
		break;

		case spinHiLoOrderSizeId:
		{
			const unsigned int& size = m_SpinOrderSize.GetValue();
			if(size != m_controlOrderSize)
			{
				m_controlOrderSize = size;
				m_modified = true;
				EnableOrderSizeButtons();
			}
		}
		break;

		case spinHiLoEnterReserveSizeId:
		if(UpdateReserveSize(m_SpinEnterReserveSize, m_controlEnterReserveSize, m_enterReserveSize))
		{
			EnableEnterVenueButtons();
			m_modified = true;
		}
		break;

		case spinHiLoExitReserveSizeId:
		if(UpdateReserveSize(m_SpinExitReserveSize, m_controlExitReserveSize, m_exitReserveSize))
		{
			EnableExitVenueButtons();
			m_modified = true;
		}
		break;

		default:
		BaseDialogSettingStrategyTimer::OnSpinChange(spinId, l);
		break;
	}
	return 0;
}

void DialogStrategyHiLo::ToolTipsEnabled(bool enable)
{
	BaseDialogSettingStrategyTimer::ToolTipsEnabled(enable);
	m_ComboBoxEnterDestination.EnableTooltips(enable, m_toolTip);
	m_ComboBoxExitDestination.EnableTooltips(enable, m_toolTip);
	m_ComboBoxEnterRouting.EnableTooltips(enable, m_toolTip);
	m_ComboBoxExitRouting.EnableTooltips(enable, m_toolTip);
}

void DialogStrategyHiLo::InitControls()
{
	BaseDialogSettingStrategyTimer::InitControls();

/*
	const Destination* xch = frame->GetExchangeDestination();
	const Destination* ino = frame->GetInsideOppositeSideDestination();
	const Destination* ins = frame->GetInsideSameSideDestination();

	m_ComboBoxDestination.AddString((const char*)xch);
	m_ComboBoxDestination.AddString((const char*)ino);
	m_ComboBoxDestination.AddString((const char*)ins);

	TakionIterator* iterator = TD_CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		if(!destination->isBorrow() && frame->isDestinationValid(destination->GetId()))
		{
			m_ListBoxDestination.AddString((const char*)destination);
			m_ComboBoxDestination.AddString((const char*)destination);
		}
	}
	delete iterator;

	m_ComboBoxDestination.CreateItemToolTipArray();//PB_Count);
	int found = m_ComboBoxDestination.FindItem(xch);
	if(found >= 0)m_ComboBoxDestination.SetItemToolTip(found, "Stock's Primary Exchange", 0);
	found = m_ComboBoxDestination.FindItem(ino);
	if(found >= 0)m_ComboBoxDestination.SetItemToolTip(found, "1st Market Participant on the opposite side", 0);
	found = m_ComboBoxDestination.FindItem(ins);
	if(found >= 0)m_ComboBoxDestination.SetItemToolTip(found, "1st Market Participant on the same side", 0);

	m_ListBoxDestination.Invalidate(FALSE);
	m_ComboBoxDestination.Invalidate(FALSE);
*/

	m_ComboBoxEnterDestination.AddString((const char*)&m_exchangeDestination);
	m_ComboBoxExitDestination.AddString((const char*)&m_exchangeDestination);

	TakionIterator* iterator = TD_CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		if(!destination->isBorrow() 
#ifndef TAKION_NO_OPTIONS
			&& (destination->GetApplicability() & (1 << ST_STOCK))
#endif
			)
		{
			m_ComboBoxEnterDestination.AddString((const char*)destination);
			m_ComboBoxExitDestination.AddString((const char*)destination);
		}
	}
	delete iterator;

	const char* const toolTipText = "Stock's Primary Exchange";
	m_ComboBoxEnterDestination.CreateItemToolTipArray();
	int found = m_ComboBoxEnterDestination.FindItem(&m_exchangeDestination);
	if(found >= 0)m_ComboBoxEnterDestination.SetItemToolTip(found, toolTipText, 0);

	m_ComboBoxExitDestination.CreateItemToolTipArray();
	found = m_ComboBoxExitDestination.FindItem(&m_exchangeDestination);
	if(found >= 0)m_ComboBoxExitDestination.SetItemToolTip(found, toolTipText, 0);

	m_ComboBoxEnterDestination.SetCurSel(0);
	OnSelchangeEnterDestination();
	OnEnterVenueApply();

//	const bool enableToolTips = true;
//	SetToolTipsEnabled(enableToolTips);

	m_ComboBoxExitDestination.SetCurSel(0);
	OnSelchangeExitDestination();
	OnExitVenueApply();

}

BOOL DialogStrategyHiLo::OnInitDialog()
{
	BOOL ret = BaseDialogSettingStrategyTimer::OnInitDialog();

	m_EditEnterMnemonic.SetLimitText(4);
	m_EditExitMnemonic.SetLimitText(4);
/*
	m_ComboBoxEnterDestination.AddString((const char*)&m_exchangeDestination);
	m_ComboBoxExitDestination.AddString((const char*)&m_exchangeDestination);

	TakionIterator* iterator = TD_CreateDestinationIterator();
	iterator->Reset();
	const Destination* destination;
	while(destination = TD_GetNextDestination(iterator))
	{
		if(!destination->isBorrow() 
#ifndef TAKION_NO_OPTIONS
			&& (destination->GetApplicability() & (1 << ST_STOCK))
#endif
			)
		{
			m_ComboBoxEnterDestination.AddString((const char*)destination);
			m_ComboBoxExitDestination.AddString((const char*)destination);
		}
	}
	delete iterator;

	m_ComboBoxEnterDestination.SetCurSel(0);
	OnSelchangeEnterDestination();
	OnEnterVenueApply();

//	const bool enableToolTips = true;
//	SetToolTipsEnabled(enableToolTips);

	m_ComboBoxExitDestination.SetCurSel(0);
	OnSelchangeExitDestination();
	OnExitVenueApply();
*/
	return ret;
}

void DialogStrategyHiLo::BeforeDestroy()
{
	BaseDialogSettingStrategyTimer::BeforeDestroy();
	m_controlEnterDestination = nullptr;
	m_currentEnterDestination = nullptr;
	m_controlEnterRouting = nullptr;
	m_currentEnterRouting = nullptr;

	m_controlExitDestination = nullptr;
	m_currentExitDestination = nullptr;
	m_controlExitRouting = nullptr;
	m_currentExitRouting = nullptr;

	m_controlEnterVenue = 0;
	m_enterVenue = 0;
	m_controlEnterRoutingName = 0;
	m_enterRoutingName = 0;
	m_controlEnterMnemonic = 0;
	m_enterMnemonic = 0;
	m_controlEnterReserveSize = 0;
	m_enterReserveSize = 0;

	m_controlExitVenue = 0;
	m_exitVenue = 0;
	m_controlExitRoutingName = 0;
	m_exitRoutingName = 0;
	m_controlExitMnemonic = 0;
	m_exitMnemonic = 0;
	m_controlExitReserveSize = 0;
	m_exitReserveSize = 0;

	m_controlYesterdayVolume = 0;
	m_yesterdayVolume = 0;

	m_controlMinPrice.SetZero();
	m_minPrice.SetZero();
	m_controlMaxPrice.SetZero();
	m_maxPrice.SetZero();

	m_controlOrderSize = 0;
	m_orderSize = 0;
}

HBRUSH DialogStrategyHiLo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editHiLoEnterMnemonicId:
		if(!m_controlEnterDestination || !m_controlEnterDestination->HasMnemonic())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		else if(m_controlEnterMnemonic != m_enterMnemonic)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;

		case editHiLoExitMnemonicId:
		if(!m_controlExitDestination || !m_controlExitDestination->HasMnemonic())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetGrayBrush();
		}
		else if(m_controlExitMnemonic != m_exitMnemonic)
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetPendingBrush();
		}
		break;
	}
	return BaseDialogSettingStrategyTimer::OnCtlColor(pDC, pWnd, nCtlColor);
}

void DialogStrategyHiLo::RepaintTotals() const
{
	BaseDialogSettingStrategyTimer::RepaintTotals();
}

void DialogStrategyHiLo::OnHistoryLoaded()
{
	BaseDialogSettingStrategyTimer::OnHistoryLoaded();
}

void DialogStrategyHiLo::OnExecutorConnected()
{
	BaseDialogSettingStrategyTimer::OnExecutorConnected();
}

void DialogStrategyHiLo::RestoreLayout()
{
	bool changed = false;
	const unsigned __int64& volume = extensionConfig->m_strategyHiLoYesterdayVolumeLimit.GetValue();
	if(volume != m_yesterdayVolume)
	{
		m_yesterdayVolume = volume;
		changed = true;
	}

	const Price& minPrice = extensionConfig->m_strategyHiLoMinPrice.GetValue();
	if(minPrice != m_minPrice)
	{
		m_minPrice = minPrice;
		changed = true;
	}

	const Price& maxPrice = extensionConfig->m_strategyHiLoMaxPrice.GetValue();
	if(maxPrice != m_maxPrice)
	{
		m_maxPrice = maxPrice;
		changed = true;
	}

	if(changed)
	{
		OnFilterReset();
	}

	const unsigned int& orderSize = extensionConfig->m_strategyHiLoOrderSize.GetValue();
	if(orderSize != m_orderSize)
	{
		m_orderSize = orderSize;
//		m_SpinYesterdayVolume.SetValue(volume, false, true, false);
		OnOrderSizeReset();
	}

	m_enterVenue = extensionConfig->m_strategyHiLoEnterVenue.GetValue();
	if(m_enterVenue)
	{
		if(m_enterVenue == m_exchangeDestination.GetMainNumericName())
		{
			m_currentEnterDestination = &m_exchangeDestination;
		}
		else
		{
			m_currentEnterDestination = TD_FindDestinationByNumericName(m_enterVenue);
		}
	}
	else
	{
		m_enterVenue = m_exchangeDestination.GetMainNumericName();//*(unsigned int*)
		m_currentEnterDestination = &m_exchangeDestination;
	}
	m_enterRoutingName = extensionConfig->m_strategyHiLoEnterRouting.GetValue();
	if(m_enterRoutingName && m_currentEnterDestination)
	{
		m_currentEnterRouting = m_currentEnterDestination->FindRoutingByNumericName(m_enterRoutingName);
		if(!m_currentEnterRouting)
		{
			m_enterRoutingName = 0;
		}
	}
	else
	{
		m_currentEnterRouting = nullptr;
	}
	m_enterMnemonic = extensionConfig->m_strategyHiLoEnterMnemonic.GetValue();
	m_enterReserveSize = extensionConfig->m_strategyHiLoEnterReserveSize.GetValue();
	OnEnterVenueReset();

	m_exitVenue = extensionConfig->m_strategyHiLoExitVenue.GetValue();
//	m_currentExitDestination = 
	if(m_exitVenue)
	{
		if(m_exitVenue == m_exchangeDestination.GetMainNumericName())
		{
			m_currentExitDestination = &m_exchangeDestination;
		}
		else
		{
			m_currentExitDestination = TD_FindDestinationByNumericName(m_exitVenue);
		}
	}
	else
	{
		m_exitVenue = m_exchangeDestination.GetMainNumericName();
		m_currentExitDestination = &m_exchangeDestination;
	}
	m_exitRoutingName = extensionConfig->m_strategyHiLoExitRouting.GetValue();
	if(m_exitRoutingName && m_currentExitDestination)
	{
		m_currentExitRouting = m_currentExitDestination->FindRoutingByNumericName(m_exitRoutingName);
		if(!m_currentExitRouting)
		{
			m_exitRoutingName = 0;
		}
	}
	else
	{
		m_currentExitRouting = nullptr;
	}
	m_exitMnemonic = extensionConfig->m_strategyHiLoExitMnemonic.GetValue();
	m_exitReserveSize = extensionConfig->m_strategyHiLoExitReserveSize.GetValue();
	OnExitVenueReset();
//	m_currentEnterDestination = m_controlEnterDestination;
//	m_currentExitDestination = m_controlExitDestination;

	BaseDialogSettingStrategyTimer::RestoreLayout();
}

void DialogStrategyHiLo::UpdateSettings()
{
	BaseDialogSettingStrategyTimer::UpdateSettings();
	extensionConfig->m_strategyHiLoYesterdayVolumeLimit.SetValue(m_controlYesterdayVolume);
	extensionConfig->m_strategyHiLoMinPrice.SetValue(m_controlMinPrice);
	extensionConfig->m_strategyHiLoMaxPrice.SetValue(m_controlMaxPrice);

	extensionConfig->m_strategyHiLoOrderSize.SetValue(m_controlOrderSize);

	extensionConfig->m_strategyHiLoEnterVenue.SetValue(m_controlEnterVenue);
	extensionConfig->m_strategyHiLoEnterRouting.SetValue(m_controlEnterRoutingName);
	extensionConfig->m_strategyHiLoEnterMnemonic.SetValue(m_controlEnterMnemonic);
	extensionConfig->m_strategyHiLoEnterReserveSize.SetValue(m_controlEnterReserveSize);

	extensionConfig->m_strategyHiLoExitVenue.SetValue(m_controlExitVenue);
	extensionConfig->m_strategyHiLoExitRouting.SetValue(m_controlExitRoutingName);
	extensionConfig->m_strategyHiLoExitMnemonic.SetValue(m_controlExitMnemonic);
	extensionConfig->m_strategyHiLoExitReserveSize.SetValue(m_controlExitReserveSize);
}

void DialogStrategyHiLo::Notify(const Message* message, const Observable* from, const Message* info)
{
	BaseDialogSettingStrategyTimer::Notify(message, from, info);
}


//HiLoStrategyCode

bool HiLoSecurityObserver::EnterHi()
{
#ifdef _DEBUG
if(m_dayHiPriceNew.isZero())
{
	if(!m_dayHiPrice.isZero())
	{
		printf("X");
	}
}
#endif
	if(m_dayHiPrice < m_dayHiPriceNew)
	{
		if(m_dayHiPrice.isZero())//first initialization from zero. Do not try to trade
		{
			m_dayHiPrice = m_dayHiPriceNew;
		}
		else
		{
			m_dayHiPrice = m_dayHiPriceNew;
			if(TS_ENTRY == m_tradingState && m_lastPrintPrice + Price::oneCent >= m_dayHiPrice)//Last Price within one cent of Hi Price
			{
				if(m_dayHiPrice > m_yesterdayHiPrice)
				{
					if(CanEnter() && CanTrade())
					{
			//			m_doNotEnter = true;
						SendOpeningOrder(true);
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool HiLoSecurityObserver::EnterLo()
{
#ifdef _DEBUG
if(m_dayLoPriceNew.isZero())
{
	if(!m_dayLoPrice.isZero())
	{
		printf("X");
	}
}
#endif
	if((m_dayLoPrice > m_dayLoPriceNew || m_dayLoPrice.isZero()) && !m_dayLoPriceNew.isZero())
	{
		if(m_dayLoPrice.isZero())//first initialization from zero. Do not try to trade
		{
			m_dayLoPrice = m_dayLoPriceNew;
		}
		else
		{
			m_dayLoPrice = m_dayLoPriceNew;
			if(TS_ENTRY == m_tradingState && m_lastPrintPrice <= m_dayLoPrice + Price::oneCent)//Last Price within one cent of Lo Price
			{
//				EnterLo();
				if(m_dayLoPrice < m_yesterdayLoPrice)
				{
					if(CanEnter() && CanTrade())
					{
			//			m_doNotEnter = true;
						SendOpeningOrder(false);
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool HiLoSecurityObserver::DoUpdateSecurity()
{
	bool ret = TimerSecurityObserver::DoUpdateSecurity();
	LbHorizontalScroller* scroller = m_listBoxSymbol->GetHorizontalScroller();
	if(EnterHi())
	{
		if(scroller)scroller->InvalidateItemColumn(m_ordinal, ST_HL_HI);
		ret = true;
	}
	if(EnterLo())
	{
		if(scroller)scroller->InvalidateItemColumn(m_ordinal, ST_HL_LO);
		ret = true;
	}
	return ret;
}

void HiLoSecurityObserver::Enter()
{
	EnterHi();
	EnterLo();
}

void HiLoSecurityObserver::OnPosSizeChanged()
{
	m_doNotEnter = true;
}

bool HiLoSecurityObserver::CanEnter() const
{
	
	if(!m_doNotEnter && TS_ENTRY == m_tradingState)// && m_lbHiLo->GetDlgHiLo()->GetCurrentEnterDestination();//m_currentEnterDestination;
	{
		const DialogStrategyHiLo* dlg = m_lbHiLo->GetDlgHiLo();
		return dlg->GetCurrentEnterDestination() && dlg->GetOrderSize();
	}
	return false;
}

void HiLoSecurityObserver::SendOpeningOrder(const bool hi)
{
	const DialogStrategyHiLo* dlg = m_lbHiLo->GetDlgHiLo();
	const Price& nbboPrice = hi ? m_level1AskPrice : m_level1BidPrice;
	if(nbboPrice >= dlg->GetMinPrice() && nbboPrice <= dlg->GetMaxPrice())
	{
		const unsigned int orderSize = dlg->GetOrderSize();
		if(orderSize)
		{
			const Destination* destination = dlg->GetCurrentEnterDestination();
			if(destination == dlg->GetExchangeDestination())//use the default destination of the exchange to which the stock belongs
			{
				destination = TD_GetDestinationByPrimaryExchange(m_security->GetPrimaryExchange());
			}
			if(destination)
			{
				Account* account = m_listBoxSymbol->GetAccount();
				const DestinationRouting* routing = dlg->GetCurrentEnterRouting();
				unsigned int orderClientId = 0;//output parameter
				unsigned char error = account->SendOrder(&orderClientId,
					nullptr,//orderToReplace
					false,//replaceNative
					0,//replaceSizeOffset
					m_security,
					PE_NOT_POSTPONED,
					0,//timeToActivate
					destination->GetMainNumericName(),
					destination->GetId(),
					hi ? 'B' : 'S',//side
					0,//unsigned char repriceFlags
					ORDER_TYPE_MARKET,//order type
					0,//userOrderType
					nullptr,//userOrderInfo
					nullptr,//commandName
					Price::priceZero,//limitPrice
					Price::priceZero,//discretionaryPrice
					Price::priceZero,//stopPrice
					OST_NONE,//stopOrderType
					OSB_BID,//stopOrderBase
					m_level1BidPrice,
					m_level1AskPrice,
					m_lastPrintPrice,
					orderSize,
					0,//minQuantity
					orderSize,//displayQuantity
					dlg->GetEnterReserveQuantity(),
					routing ? routing->GetId() : 0, 
					routing ? routing->GetSubType() : 0, 
					routing ? routing->GetNumericName() : 0,
					destination->HasMnemonic() ? dlg->GetEnterMnemonic() : 0,
					false,//iso
					PEG_NONE,//pegType
					SignedPrice::signedPriceZero,//pegOffset
					TIF_IOC,//tifType
					0,//tifMilliseconds (for TIF_GTD only)
					WOP_CANCEL,//TD_GetWashOrderPolicy(),
					OOP_RESIZE,//TD_GetOversellSplitPolicy(),
					false,//TD_IsResizeShortToBorrowed(),
					true,//preBorrow
					Price::oneCent,//preBorrow Limit Price
					false,//noRedirect
					false,//closeOnly
					false,//resizeToClose
					false,//allOrNone
					false,//syntheticIgnoreFinra,
					false,//syntheticWaitNext,
					false,//imbalanceOnly,
//					false,//proAts
					true,//blockAggressiveOrders
					RLP_ROUND_DOWN,//TD_GetRoundLotPolicy(),
					nullptr,//isoAttachment
					0,//userId
					0,//parentId
					0);//ocoId
				switch(error)
				{
	//can try again later in case of these errors
					case OE_SIZE_ZERO:
					case OE_ORDER_SIZE_EXCEEDED:
					case OE_ORDER_VALUE_EXCEEDED:
					case OE_HISTORY_NOT_LOADED:
					case OE_HISTORY_LOAD_ERROR:
					case OE_SECURITY_NOT_LOADED:
					case OE_BUYING_POWER:
					case OE_HARD_BUYING_POWER:
	//				case OE_POSITION_COUNT:
					case OE_SHORT_PRICE:
					case OE_STOCK_NOT_SHORTABLE:
					case OE_HTB_STOCK_NOT_ENOUGH_BORROWED:
					case OE_REGSHO_SHORT_VIOLATION:
					case OE_HARD_TO_BORROW_SHORT_VIOLATION:
					case OE_WASH_ORDERS:
					case OE_ODD_LOT_VIOLATION:
					case OE_SHORT_ORDER_AGGRESSIVE:
					case OE_PRICE_AWAY_FROM_NBBO://For Market order this can be the error if the stock has a big spread
					case OE_ORDER_TOO_AGGRESSIVE://For Market order this can be the error if the stock has a big spread
					case OE_NO_CURRENT_ACCOUNT:
					case OE_MAX_LOSS_MARKED_EXCEEDED:
					case OE_MAX_SHARES_OPEN_EXCEEDED:
					case OE_MAX_POSITION_LOSS_MARKED_EXCEEDED:
					case OE_OVERSELL:
					case OE_OVERBUY:
					case OE_NON_TRADABLE_STOCK:
					case OE_NON_TRADABLE_POSITION:
					case OE_MAX_POSITION_BPU_EXCEEDED:
					case OE_MAX_LONG_INVESTMENT_EXCEEDED:
					case OE_MAX_SHORT_INVESTMENT_EXCEEDED:
					case OE_UNOPEN_IPO_MARKET_ORDER:
					case OE_UNTRADEABLE_SYMBOL:
					case OE_UNTRADEABLE_STATUS_NOT_LOADED:
					case OE_PRE_BORROW_REQUIRED:
					case OE_SPOOFING_ORDER_COUNT_RATIO_EXCEEDED:
					case OE_SPOOFING_ORDER_SIZE_RATIO_EXCEEDED:
					case OE_RESTRICTED_SYMBOL_LIST_NOT_LOADED:
					case OE_MIN_SHARE_PRICE_EXCEEDED:
					case OE_MAX_SHARE_PRICE_EXCEEDED:
					case OE_ADV_ORDER_COUNT_EXCEEDED:
					case OE_ADV_ORDER_SIZE_RATIO_EXCEEDED:
					case OE_ADV_TOO_LOW:
					case OE_ORDER_TOO_PASSIVE:
					case OE_MAX_ORDERS_PER_POSITION_EXCEEDED:
					case OE_ORDERS_PER_TIME_FRAME_EXCEEDED:
					case OE_NBBO_ORDER_SIZE_EXCEEDED:
					break;

					case OE_OK:
					m_positionEnterTime = TL_GetCurrentMillisecond();
					{
						const LbHorizontalScroller* scroller = m_listBoxSymbol->GetHorizontalScroller();
						if(scroller)
						{
							scroller->InvalidateItemColumn(m_ordinal, ST_HL_LAST_ENTER);
						}
					}
//fall through
					default:
					m_doNotEnter = true;
					break;
				}
			}
			else
			{
				m_doNotEnter = true;
			}
		}
	}
}

void HiLoSecurityObserver::Exit()
{
	if(m_posSize)
	{
		const DialogStrategyHiLo* dlg = m_lbHiLo->GetDlgHiLo();
		const Destination* destination = dlg->GetCurrentExitDestination();
		if(destination == dlg->GetExchangeDestination())//use the default destination of the exchange to which the stock belongs
		{
			destination = TD_GetDestinationByPrimaryExchange(m_security->GetPrimaryExchange());
		}
		const DestinationRouting* routing;// = dlg->GetCurrentExitRouting();
		if(destination)
		{
			routing = dlg->GetCurrentExitRouting();
		}
		else
		{
			destination = TD_GetDestinationNsdq();
			routing = destination->FindRouting(RI_PROACTIVE, 0);
		}

		char side = m_posSize > 0 ? 'S' : 'B';
		unsigned int orderSize = m_posSize > 0 ? (unsigned int)m_posSize : (unsigned int)-m_posSize;
		Account* account = m_listBoxSymbol->GetAccount();
		unsigned int orderClientId = 0;//output parameter
		unsigned char error = account->SendOrder(&orderClientId,
			nullptr,//orderToReplace
			false,//replaceNative
			0,//replaceSizeOffset
			m_security,
			PE_NOT_POSTPONED,
			0,//timeToActivate
			destination->GetMainNumericName(),
			destination->GetId(),
			side,
			0,//unsigned char repriceFlags
			ORDER_TYPE_MARKET,//order type
			0,//userOrderType
			nullptr,//userOrderInfo
			nullptr,//commandName
			Price::priceZero,//limitPrice
			Price::priceZero,//discretionaryPrice
			Price::priceZero,//stopPrice
			OST_NONE,//stopOrderType
			OSB_BID,//stopOrderBase
			m_level1BidPrice,
			m_level1AskPrice,
			m_lastPrintPrice,
			orderSize,
			0,//minQuantity
			orderSize,//displayQuantity
			dlg->GetExitReserveQuantity(),
			routing ? routing->GetId() : 0, 
			routing ? routing->GetSubType() : 0, 
			routing ? routing->GetNumericName() : 0,
			destination->HasMnemonic() ? dlg->GetEnterMnemonic() : 0,
			false,//iso
			PEG_NONE,//pegType
			SignedPrice::signedPriceZero,//pegOffset
			TIF_DAY,//tifType
			0,//tifMilliseconds (for TIF_GTD only)
			WOP_CANCEL,//TD_GetWashOrderPolicy();
			OOP_RESIZE,//TD_GetOversellSplitPolicy()
			false,//TD_IsResizeShortToBorrowed(),
			true,//preBorrow
			Price::oneCent,//preBorrow Limit Price
			false,//noRedirect
			false,//closeOnly
			false,//resizeToClose
			false,//allOrNone
			false,//syntheticIgnoreFinra,
			false,//syntheticWaitNext,
			false,//imbalanceOnly,
//			false,//proAts
			true,//blockAggressiveOrders
			RLP_ROUND_DOWN,//TD_GetRoundLotPolicy(),
			nullptr,//isoAttachment
			0,//userId
			0,//parentId
			0);//ocoId
		if(OE_OK == error)
		{
//			m_positionEnterTime = 0;
		}
	}
}

void HiLoSecurityObserver::OnTradingStateChange()
{
	switch(m_tradingState)
	{
		case TS_BEFORE_ENTRY:
		break;

		case TS_ENTRY:
		Enter();
		break;

		case TS_STOP:
		break;

		case TS_EXIT:
		Exit();
		break;

		case TS_STOP_EXIT:
		break;
	}
}

