#include "stdafx.h"
#include "SampleDialogSettingSimulateEnforceAggregation.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateEnforceAggregation::SampleDialogSettingSimulateEnforceAggregation(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateMessage(mainWnd, parentTab, "Enforce Aggregation"),
	m_ComboBoxClearingFirm(0xFFFFFFFF, "Choose Clearing Firm"),
	m_SpinEnforceNiteBetaTime(false, 235959, 0, 0),
	m_SpinNiteBetaBp(999999999999, 0, 0, 100000, 3, TakionSpin::charComma, 2),
	m_ButtonPaste("Paste Values from the Clearing Firm into Controls"),
	m_clearingFirm(nullptr),
	m_clearingFirmSel(-1),
	m_historyLoaded(false),
	m_executorConnected(false),
	m_enforceNiteBetaTimeSeconds(0),
	m_enforceAggregation(false),
	m_conservativeOrderMarking(false),
	m_localSideMarking(false)
{
	m_SpinEnforceNiteBetaTime.AddSpinCell(1, false);
	m_SpinEnforceNiteBetaTime.AddSpinCell(100, false);
	m_SpinEnforceNiteBetaTime.AddSpinCell(10000, true);

	SetMoneySpinCells(m_SpinNiteBetaBp);

	const int accountGroupHeight = 24;
	const int firstColumnWidth2 = m_firstColumnWidth + m_firstColumnWidth;
	const int controlX = m_xc + hgap;
	const int controlW = firstColumnWidth2 - hgap - hgap;
	AddButtonIndirect("Clearing Firm", WS_VISIBLE | BS_GROUPBOX, WS_EX_TRANSPARENT, m_xc, m_bottomControl, firstColumnWidth2, accountGroupHeight, groupClearingFirmId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, controlX, m_bottomControl + groupOffset, controlW, 200, comboBoxClearingFirmId);
	m_bottomControl += accountGroupHeight;

	const int controlWidth =
#ifndef TAKION_NO_OPTIONS
		hgap + m_firstColumnWidth +
#endif
		m_xc + m_firstColumnWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	m_bottomControl += vgap;
	AddButtonIndirect("Enforce Aggregation", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, controlX, m_bottomControl, controlW, checkBoxHeight, checkBoxEnforceId);
	AddButtonIndirect("ConservativeOrderMarking", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, controlX, m_bottomControl += checkBoxHeight + vgap, controlW, checkBoxHeight, checkBoxConservativeOrderMarkingId);
	AddButtonIndirect("LocalSideMarking", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, controlX, m_bottomControl += checkBoxHeight + vgap, controlW, checkBoxHeight, checkBoxLocalSideMarkingId);

//	int controlWidth = m_xc + m_firstColumnWidth + hgap;
//	int sideWidth = 32;
//	const int w = controlWidth;
	int timeWidth = m_firstColumnWidth - spinHeight / 2;
	int staticX = controlX + timeWidth + hgap;
	int staticW = controlW - timeWidth;
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_GROUP|WS_TABSTOP, 0, controlX, m_bottomControl += checkBoxHeight + vgap, timeWidth, spinHeight, spinEnforceNiteBetaTimeId);
	AddStaticIndirect("NiteBetaTime", WS_VISIBLE|SS_NOPREFIX, 0, staticX, m_bottomControl + (spinHeight - staticHeight) / 2, staticW, staticHeight, staticEnforceNiteBetaTimeId);
//	m_bottomControl += spinHeight;

//	m_bottomControl += spinHeight + vgap;
	const int moneyWidth = m_firstColumnWidth + spinHeight * 24 / 8 + 7;
	const int moneyNameWidth = firstColumnWidth2 - moneyWidth + 14;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE| WS_TABSTOP, 0, m_xc, m_bottomControl += spinHeight + vgap, moneyWidth, spinHeight, spinEnforceNiteBetaBpId);
	AddStaticIndirect("NiteBetaBp", WS_VISIBLE|SS_NOPREFIX, 0, m_xc + moneyWidth, m_bottomControl + (spinHeight - staticHeight) / 2, moneyNameWidth, staticHeight, staticEnforceNiteBetaBpId);

	AddButtonIndirect("Paste", WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP|WS_DISABLED, 0, m_xc, m_bottomControl += spinHeight + vgap + vgap, m_sendButtonWidth, buttonHeight, buttonPasteClearingFirmId);

	m_bottomControl += buttonHeight;

	SetExecutorConnected(TD_IsExecutorConnected());
}
/*
void SampleDialogSettingSimulateEnforceAggregation::SetPriceSpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(1, false);
	spin.AddSpinCell(100, false);
	spin.AddSpinCell(1000000, true);
	spin.AddSpinCell(10000, true);
}

void SampleDialogSettingSimulateEnforceAggregation::SetMoneySpinCells(UnsignedNumberTypeSpin<unsigned __int64>& spin)
{
	spin.AddSpinCell(100000000, true);
	SetPriceSpinCells(spin);
}
*/

void SampleDialogSettingSimulateEnforceAggregation::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateMessage::DoDataExchange(pDX);

	DDX_Control(pDX, comboBoxClearingFirmId, m_ComboBoxClearingFirm);
	DDX_Control(pDX, checkBoxEnforceId, m_CheckBoxEnforce);
	DDX_Control(pDX, checkBoxConservativeOrderMarkingId, m_CheckBoxConservativeOrderMarking);
	DDX_Control(pDX, checkBoxLocalSideMarkingId, m_CheckBoxLocalSideMarking);
	DDX_Control(pDX, spinEnforceNiteBetaTimeId, m_SpinEnforceNiteBetaTime);
	DDX_Control(pDX, spinEnforceNiteBetaBpId, m_SpinNiteBetaBp);
	DDX_Control(pDX, buttonPasteClearingFirmId, m_ButtonPaste);
}

bool SampleDialogSettingSimulateEnforceAggregation::isSendMessageButtonEnabled() const
{
	return m_clearingFirm != nullptr;
}

void SampleDialogSettingSimulateEnforceAggregation::BeforeDestroy()
{
	TD_GetNewClearingFirmObservable().RemoveObserver(this);
	TD_GetNewAccountObservable().RemoveObserver(this);
	TD_GetAdminObservable().RemoveObserver(this);
	BaseDialogSettingSimulateMessage::BeforeDestroy();
}

BOOL SampleDialogSettingSimulateEnforceAggregation::OnInitDialog()
{
	m_clearingFirmSel = -1;
	m_clearingFirm = nullptr;
	m_clearingFirmId.Nullify();

	m_enforceNiteBetaTimeSeconds = 0;
	m_niteBetaBp.SetZero();
	m_enforceAggregation = false;
	m_conservativeOrderMarking = false;
	m_localSideMarking = false;

	TD_GetNewClearingFirmObservable().AddObserver(this);
	TD_GetNewAccountObservable().AddObserver(this);
	TD_GetAdminObservable().AddObserver(this);
	BOOL ret = BaseDialogSettingSimulateMessage::OnInitDialog();

	TakionIterator* iterator = TD_CreateClearingFirmIterator();
	ClearingFirm* clearingFirm;
	TD_LockClearingFirmStorage();
	iterator->Reset();
	while(clearingFirm = TD_GetNextClearingFirm(iterator))
	{
		m_ComboBoxClearingFirm.AddString((const char*)clearingFirm);
	}
	TD_UnlockClearingFirmStorage();
	delete iterator;

//	RestoreLayout();

	return ret;
}

bool SampleDialogSettingSimulateEnforceAggregation::isPasteEnabled() const
{
	bool ret = false;
	if(m_clearingFirm)
	{
		m_clearingFirm->LockInquiryWait();
		ret = m_clearingFirm->GetNiteBetaBuyingPower() != m_niteBetaBp
			|| m_clearingFirm->GetNiteBetaEnforcementStartTime() != m_enforceNiteBetaTimeSeconds * 1000
			|| m_clearingFirm->isEnforceFirm() != m_enforceAggregation
			|| m_clearingFirm->isConservativeOrderMarking() != m_conservativeOrderMarking
			|| m_clearingFirm->isLocalSideMarking() != m_localSideMarking;
		m_clearingFirm->UnlockInquiry();
	}
	return ret;
}

void SampleDialogSettingSimulateEnforceAggregation::EnablePasteButton()
{
	EnableControl(m_ButtonPaste.m_hWnd, isPasteEnabled(), m_ComboBoxClearingFirm.m_hWnd);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateEnforceAggregation, BaseDialogSettingSimulateMessage)
	ON_CBN_SELCHANGE(comboBoxClearingFirmId, OnSelchangeClearingFirm)
	ON_BN_CLICKED(checkBoxEnforceId, OnEnforceAggregation)
	ON_BN_CLICKED(checkBoxConservativeOrderMarkingId, OnConservativeOrderMarking)
	ON_BN_CLICKED(checkBoxLocalSideMarkingId, OnLocalSideMarking)
	ON_BN_CLICKED(buttonPasteClearingFirmId, OnPasteCleringFirm)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateEnforceAggregation::OnCheckBox(const CButton& checkBox, bool& value)
{
	const bool boolValue = checkBox.GetCheck() == BST_CHECKED;
	if(boolValue != value)
	{
		value = boolValue;
		EnablePasteButton();
	}
}

void SampleDialogSettingSimulateEnforceAggregation::OnEnforceAggregation()
{
	OnCheckBox(m_CheckBoxEnforce, m_enforceAggregation);
}

void SampleDialogSettingSimulateEnforceAggregation::OnConservativeOrderMarking()
{
	OnCheckBox(m_CheckBoxConservativeOrderMarking, m_conservativeOrderMarking);
}

void SampleDialogSettingSimulateEnforceAggregation::OnLocalSideMarking()
{
	OnCheckBox(m_CheckBoxLocalSideMarking, m_localSideMarking);
}

const Message* SampleDialogSettingSimulateEnforceAggregation::CreateMessage(WSABUF& wsaBuf)
{
	if(m_clearingFirm)
	{
		m_msg.m_time = TL_GetCurrentMillisecond();

		U_CopyAndPad(m_msg.m_clearingAccountName, sizeof(m_msg.m_clearingAccountName), m_clearingFirm->GetName(), '\0');
	
//		m_msg.m_enforceFirm = m_CheckBoxEnforce.GetCheck() == BST_CHECKED;
//		m_msg.m_conservativeOrderMarking = m_CheckBoxConservativeOrderMarking.GetCheck() == BST_CHECKED;
//		m_msg.m_localSideMarking = m_CheckBoxLocalSideMarking.GetCheck() == BST_CHECKED;

		m_msg.m_enforceFirm = m_enforceAggregation;
		m_msg.m_conservativeOrderMarking = m_conservativeOrderMarking;
		m_msg.m_localSideMarking = m_localSideMarking;

		m_msg.m_niteBetaEnforcementStartTime = m_enforceNiteBetaTimeSeconds * 1000;

		m_msg.m_niteBetaGroupBuyingPower = m_niteBetaBp.toDouble();

		return &m_msg;
	}
	return nullptr;
}

void SampleDialogSettingSimulateEnforceAggregation::UpdateSettings()
{
//	UpdateAccountInConfig(extensionConfig->m_simEnforceAggregationAccountId);

//	extensionConfig->m_simEnforceAggregationClearingFirmId.SetValue()
	if(m_clearingFirmSel >= 0 && m_clearingFirm)
	{
		extensionConfig->m_simEnforceAggregationClearingFirmId.SetValue(std::string(m_clearingFirm->GetName()));
	}
	else
	{
		extensionConfig->m_simEnforceAggregationClearingFirmId.SetValue(std::string());
	}

//	extensionConfig->m_simEnforceAggregationEnforce.SetValue(m_CheckBoxEnforce.GetCheck() == BST_CHECKED);
//	extensionConfig->m_simEnforceAggregationConservativeOrderMarking.SetValue(m_CheckBoxConservativeOrderMarking.GetCheck() == BST_CHECKED);
//	extensionConfig->m_simEnforceAggregationLocalSideMarking.SetValue(m_CheckBoxLocalSideMarking.GetCheck() == BST_CHECKED);

	extensionConfig->m_simEnforceAggregationEnforce.SetValue(m_enforceAggregation);
	extensionConfig->m_simEnforceAggregationConservativeOrderMarking.SetValue(m_conservativeOrderMarking);
	extensionConfig->m_simEnforceAggregationLocalSideMarking.SetValue(m_localSideMarking);

	extensionConfig->m_simEnforceAggregationEnforceNiteBetaTimeSeconds.SetValue(m_enforceNiteBetaTimeSeconds);

//	Money money;
//	FillMoneyFromUnsignedSpinControl(money, m_SpinNiteBetaBp, 100);
//	extensionConfig->m_simEnforceAggregationNiteBetaBp.SetValue(money);

	extensionConfig->m_simEnforceAggregationNiteBetaBp.SetValue(m_niteBetaBp);
}


void SampleDialogSettingSimulateEnforceAggregation::SetBooleanValueToCheckBox(bool& value, CButton& checkBox, const bool boolValue)
{
//	bool boolValue = extensionConfig->m_simEnforceAggregationEnforce.GetValue();
	if(boolValue != value)
	{
		value = boolValue;
		checkBox.SetCheck(boolValue ? BST_CHECKED : BST_UNCHECKED);
	}
}

void SampleDialogSettingSimulateEnforceAggregation::SetSecondsToTimeSpin(unsigned int& value, TimeSpin& timeSpin, const unsigned int uintValue)
{
	const unsigned int& enforceNiteBetaTimeSeconds = extensionConfig->m_simEnforceAggregationEnforceNiteBetaTimeSeconds.GetValue();
	if(uintValue != value)
	{
		value = uintValue;
		timeSpin.SetSecond(uintValue, false);
	}
}

void SampleDialogSettingSimulateEnforceAggregation::SetMoneyToSpin(Money& money, UnsignedNumberTypeSpin<unsigned __int64>& spin, const Money& moneyValue)
{
//	const Money& bp = extensionConfig->m_simEnforceAggregationNiteBetaBp.GetValue();
	if(moneyValue != money)
	{
		money = moneyValue;
		FillUnsignedSpinControlFromMoney(moneyValue, spin, 100, false);
	}
}

void SampleDialogSettingSimulateEnforceAggregation::RestoreLayout()
{
	const char* clearingFirmName = extensionConfig->m_simEnforceAggregationClearingFirmId.GetValue().c_str();
//	SetAccountById(extensionConfig->m_simEnforceAggregationAccountId.GetValue().c_str());
	int sel;
	if(clearingFirmName && *clearingFirmName)
	{
		sel = m_ComboBoxClearingFirm.FindItemByName(clearingFirmName);
		if(sel < 0)sel = 0;
	}
	else
	{
		sel = 0;
	}
	m_ComboBoxClearingFirm.SetCurSel(sel);
	OnSelchangeClearingFirm();

//	m_CheckBoxEnforce.SetCheck(extensionConfig->m_simEnforceAggregationEnforce.GetValue() ? BST_CHECKED : BST_UNCHECKED);
//	m_CheckBoxConservativeOrderMarking.SetCheck(extensionConfig->m_simEnforceAggregationConservativeOrderMarking.GetValue() ? BST_CHECKED : BST_UNCHECKED);
//	m_CheckBoxLocalSideMarking.SetCheck(extensionConfig->m_simEnforceAggregationLocalSideMarking.GetValue() ? BST_CHECKED : BST_UNCHECKED);

	SetBooleanValueToCheckBox(m_enforceAggregation, m_CheckBoxEnforce, extensionConfig->m_simEnforceAggregationEnforce.GetValue());
	SetBooleanValueToCheckBox(m_conservativeOrderMarking, m_CheckBoxConservativeOrderMarking, extensionConfig->m_simEnforceAggregationConservativeOrderMarking.GetValue());
	SetBooleanValueToCheckBox(m_localSideMarking, m_CheckBoxLocalSideMarking, extensionConfig->m_simEnforceAggregationLocalSideMarking.GetValue());
/*
	bool boolValue = extensionConfig->m_simEnforceAggregationEnforce.GetValue();
	if(boolValue != m_enforceAggregation)
	{
		m_enforceAggregation = boolValue;
		m_CheckBoxEnforce.SetCheck(boolValue ? BST_CHECKED : BST_UNCHECKED);
	}
	boolValue = extensionConfig->m_simEnforceAggregationConservativeOrderMarking.GetValue();
	if(boolValue != m_conservativeOrderMarking)
	{
		m_conservativeOrderMarking = boolValue;
		m_CheckBoxConservativeOrderMarking.SetCheck(boolValue ? BST_CHECKED : BST_UNCHECKED);
	}
	boolValue = extensionConfig->m_simEnforceAggregationLocalSideMarking.GetValue();
	if(boolValue != m_localSideMarking)
	{
		m_localSideMarking = boolValue;
		m_CheckBoxLocalSideMarking.SetCheck(boolValue ? BST_CHECKED : BST_UNCHECKED);
	}
*/
	SetSecondsToTimeSpin(m_enforceNiteBetaTimeSeconds, m_SpinEnforceNiteBetaTime, extensionConfig->m_simEnforceAggregationEnforceNiteBetaTimeSeconds.GetValue());
/*
	const unsigned int& enforceNiteBetaTimeSeconds = extensionConfig->m_simEnforceAggregationEnforceNiteBetaTimeSeconds.GetValue();
	if(enforceNiteBetaTimeSeconds != m_enforceNiteBetaTimeSeconds)
	{
		m_enforceNiteBetaTimeSeconds = enforceNiteBetaTimeSeconds;
		m_SpinEnforceNiteBetaTime.SetSecond(enforceNiteBetaTimeSeconds, false);
	}
*/

//	FillUnsignedSpinControlFromMoney(extensionConfig->m_simEnforceAggregationNiteBetaBp.GetValue(), m_SpinNiteBetaBp, 100, false);

	SetMoneyToSpin(m_niteBetaBp, m_SpinNiteBetaBp, extensionConfig->m_simEnforceAggregationNiteBetaBp.GetValue());
/*
	const Money& bp = extensionConfig->m_simEnforceAggregationNiteBetaBp.GetValue();
	if(bp != m_niteBetaBp)
	{
		m_niteBetaBp = bp;
		FillUnsignedSpinControlFromMoney(bp, m_SpinNiteBetaBp, 100, false);
	}
*/
	EnablePasteButton();

	EnableSendMessageButton();

	m_modified = false;
}

LRESULT SampleDialogSettingSimulateEnforceAggregation::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinEnforceNiteBetaTimeId:
		{
			const unsigned int& enforceNiteBetaTimeSeconds = m_SpinEnforceNiteBetaTime.GetSecond();
			if(enforceNiteBetaTimeSeconds != m_enforceNiteBetaTimeSeconds)
			{
				m_enforceNiteBetaTimeSeconds = enforceNiteBetaTimeSeconds;
				m_modified = true;
				EnablePasteButton();
			}
		}
		break;

		case spinEnforceNiteBetaBpId:
		{
			Money money;
			FillMoneyFromUnsignedSpinControl(money, m_SpinNiteBetaBp, 100);
			if(money != m_niteBetaBp)
			{
				m_niteBetaBp = money;
				m_modified = true;
				EnablePasteButton();
			}
		}
		break;

//		default:
//		BaseDialogSettingSimulateMessage::OnSpinChange(w, l);
//		break;
	}
	return 0;
}

void SampleDialogSettingSimulateEnforceAggregation::OnPasteCleringFirm()
{
	if(m_clearingFirm)
	{
		m_clearingFirm->LockInquiryWait();
		const Money bp = m_clearingFirm->GetNiteBetaBuyingPower();
		unsigned int enforceTime = m_clearingFirm->GetNiteBetaEnforcementStartTime();
		const bool enforceAggregation = m_clearingFirm->isEnforceFirm();
		const bool conservativeOrderMarking = m_clearingFirm->isConservativeOrderMarking();
		const bool localSideMarking = m_clearingFirm->isLocalSideMarking();
		m_clearingFirm->UnlockInquiry();

		SetBooleanValueToCheckBox(m_enforceAggregation, m_CheckBoxEnforce, enforceAggregation);
		SetBooleanValueToCheckBox(m_conservativeOrderMarking, m_CheckBoxConservativeOrderMarking, conservativeOrderMarking);
		SetBooleanValueToCheckBox(m_localSideMarking, m_CheckBoxLocalSideMarking, localSideMarking);
		enforceTime /= 1000;
//		SetSecondsToTimeSpin(m_enforceNiteBetaTimeSeconds, m_SpinEnforceNiteBetaTime, enforceTime);
		if(enforceTime != m_enforceNiteBetaTimeSeconds)
		{
			m_enforceNiteBetaTimeSeconds = enforceTime;
			m_SpinEnforceNiteBetaTime.SetSecond(enforceTime, false);
		}

		SetMoneyToSpin(m_niteBetaBp, m_SpinNiteBetaBp, bp);

		EnablePasteButton();
	}
}

////////
void SampleDialogSettingSimulateEnforceAggregation::OnSelchangeClearingFirm()
{
	int sel = m_ComboBoxClearingFirm.GetCurSel();
	if(m_clearingFirmSel != sel)
	{
		m_clearingFirmSel = sel;
		m_modified = true;
		ClearingFirm* clearingFirm = sel >= 0 ? (ClearingFirm*)m_ComboBoxClearingFirm.GetItemDataPtr(sel) : nullptr;
		if(clearingFirm)
		{
			m_clearingFirmId = clearingFirm->GetFirmId();
//			U_CopyAndPad(m_accountId, sizeof(m_accountId), account->GetId(), '\0');
		}
		else
		{
			m_clearingFirmId.Nullify();
//			U_CopyAndPad(m_accountId, sizeof(m_accountId), "", '\0');
//			account = TD_GetCurrentAccount();
		}
		SetClearingFirm(clearingFirm);
	}
/*
	else if(!m_clearingFirmSel)
	{
//		m_modified = true;
		SetClearingFirm(nullptr);
	}
*/
}

void SampleDialogSettingSimulateEnforceAggregation::UpdateComboBoxClearingFirm()
{
	if(m_clearingFirmSel >= 0)
	{
		int sel = m_ComboBoxClearingFirm.FindItem(m_clearingFirm);
		m_ComboBoxClearingFirm.SetCurSel(sel);
		OnSelchangeClearingFirm();
	}
}

void SampleDialogSettingSimulateEnforceAggregation::AddClearingFirm(ClearingFirm* clearingFirm)
{
	int sel = m_ComboBoxClearingFirm.AddString((const char*)clearingFirm);
	if(!m_clearingFirmId.isZero())
	{
		if(m_clearingFirmId == clearingFirm->GetFirmId())
		{
			m_ComboBoxClearingFirm.SetCurSel(sel);
			SetClearingFirm(clearingFirm);
		}
	}
}

void SampleDialogSettingSimulateEnforceAggregation::SetHistoryLoaded(const bool loaded)
{
	if(loaded != m_historyLoaded)
	{
		m_historyLoaded = loaded;
	}
}

void SampleDialogSettingSimulateEnforceAggregation::SetExecutorConnected(const bool connected)
{
	if(connected != m_executorConnected)
	{
		m_executorConnected = connected;
		if(!m_executorConnected)
		{
			SetHistoryLoaded(false);
			EnableSendMessageButton();
		}
	}
}

void SampleDialogSettingSimulateEnforceAggregation::SetClearingFirm(ClearingFirm* clearingFirm)
{
	if(m_clearingFirm != clearingFirm)
	{
		if(m_clearingFirm)
		{
			m_clearingFirm->RemoveObserver(this);
		}
		m_clearingFirm = clearingFirm;
		if(m_clearingFirm)
		{
			m_clearingFirmId = clearingFirm->GetFirmId();
			m_clearingFirm->AddObserver(this);
			SetHistoryLoaded(m_clearingFirm->isLoaded());
		}
		else
		{
			m_clearingFirmId.Nullify();
		}
		m_modified = true;
		EnableSendMessageButton();
		EnablePasteButton();
//		AccountChanged();
	}
}

void SampleDialogSettingSimulateEnforceAggregation::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
//		case TM_CLEARING_FIRM_UPDATE:
//		break;

		case TM_NEW_CLEARING_FIRM:
		{
			TMsgNewClearingFirm* msg = (TMsgNewClearingFirm*)message;
			ClearingFirm* clearingFirm = msg->m_clearingFirm;
			m_ComboBoxClearingFirm.AddString((const char*)clearingFirm);
			if(!m_clearingFirmId.isZero() && clearingFirm->GetFirmId() ==  m_clearingFirmId)
			{
				SetClearingFirm(clearingFirm);
				UpdateComboBoxClearingFirm();
			}
		}
		break;

		case TM_UPDATED_CLEARING_FIRM:
		{
			TMsgUpdatedClearingFirm* msg = (TMsgUpdatedClearingFirm*)message;
			ClearingFirm* clearingFirm = msg->m_clearingFirm;
		}
		break;

		case TM_UPDATED_CLEARING_FIRM_CONSTRAINTS:
		{
			TMsgUpdatedClearingFirmConstraints* msg = (TMsgUpdatedClearingFirmConstraints*)message;
			ClearingFirm* clearingFirm = msg->m_clearingFirm;
			if(clearingFirm == m_clearingFirm)
			{
				EnablePasteButton();
			}
		}
		break;
/*
		case TM_CLEARING_FIRM_DELETE:
		{
			ClearingFirm* clearingFirm = ((const TMsgClearingFirmDelete*)message)->m_clearingFirm;
			const int found = m_ComboBoxClearingFirm.FindItemByName(clearingFirm->GetName());
			if(found >= 0)
			{
				const int sel = m_ComboBoxClearingFirm.GetCurSel();
				m_ComboBoxClearingFirm.DeleteString(found);
				if(sel > 0)
				{
					const unsigned int count = m_ComboBoxClearingFirm.GetItemCount();
					if(count > 0)
					{
						if((unsigned int)sel >= count)
						{
							m_ComboBoxClearingFirm.SetCurSel(count - 1);
							OnSelchangeClearingFirm();
						}
					}
				}
			}
		}
		break;
*/
		case TM_DELETE_ALL_CLEARING_FIRMS:
//		m_clearingFirm = nullptr;
		m_ComboBoxClearingFirm.ResetContent();
		m_ComboBoxClearingFirm.SetCurSel(0);
		OnSelchangeClearingFirm();
		break;

		case TM_NEXT_DAY_STARTED:
		{
//			NextDayStarted(((const TMsgNextDayStarted*)message)->m_date);
		}
		break;

		case TM_CONNECTED:
		case TM_DISCONNECTED:
		SetExecutorConnected(TD_IsExecutorConnected());
		break;
	}
}