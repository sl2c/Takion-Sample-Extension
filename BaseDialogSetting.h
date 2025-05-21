#pragma once

class BaseDialogSetting : public TakionSettingPageBase
{
public:
	static void ReplaceRefNumber(CEdit& editRef, std::string& refNumberStr, const unsigned __int64& refNumberCurrent, const unsigned __int64& refNumber);
	static bool UpdateDestinationRoutingStatic(const Destination*& controlDestination, const Destination* const& currentDestination, ComboBoxDestination& comboBoxDestination, ComboBoxRouting& comboBoxRouting, ValidEdit* editMnemonic);
	static void SetRoutingIdStatic(unsigned short routingId, unsigned short routingSubType, const Destination* currentDestination, ComboBoxRouting& comboBoxRouting);
	static const DestinationRouting* OnSelchangeRoutingStatic(ComboBoxRouting& comboBoxRouting, UnsignedNumberTypeSpin<unsigned int>& spinReserveSize, CStatic& staticReserverSize, unsigned int* routingName = nullptr);
	static void FillTifComboBox(CComboBox& comboBox);
	virtual void Entering() override;
	static const int staticHeight = 8;
	static const int editHeight = 12;
	static const int comboBoxHeight = 12;
	static const int buttonHeight = 14;
	static const int checkBoxHeight = 8;
	static const int spinHeight = 10;
	static const int spinWidth = 32;
	static const int groupOffset = 9;
	static const int hgap = 2;
	static const int vgap = 1;
	static const COLORREF checkBoxColor;
	static const COLORREF pressedCheckColor;
	static const COLORREF textCheckColor;

	static void SetBigSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
	static void SetSizeSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
	static void SetBigSignedSizeSpinCells(SignedNumberTypeSpin<int>& spin);
	static void SetSignedSizeSpinCells(SignedNumberTypeSpin<int>& spin);
	static void SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
	static void SetBigPriceSpinCells(UnsignedNumberTypeSpin<unsigned int>& spin);
	static void SetMoneySpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin);
	static void SetSignedMoneySpinCells(SignedNumberTypeSpin<__int64>& spin);
protected:
	BaseDialogSetting(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label):
		TakionSettingPageBase(mainWnd, parentTab, label)
	{
	}
};