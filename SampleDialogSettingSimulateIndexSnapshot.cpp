#include "stdafx.h"
#include "SampleDialogSettingSimulateIndexSnapshot.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateIndexSnapshot::SampleDialogSettingSimulateIndexSnapshot(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateSymbol(mainWnd, parentTab, "IndexSnapshot"),
	m_attachmentCursor(m_attachment),
	m_attachmentLength(0),
	m_remainingLength(sizeof(m_attachment))
{
	AddListBox(&m_ListBoxSnapshotFlags);

//	m_emptySymbolValid = true;

	m_bottomControl += vgap;
	int sideWidth = 32;
	const int priceWidth = 75;
	const int width = priceWidth + sideWidth;
	const int widthHalf = width / 2;

	const int firstColumnWidthHalf = m_firstColumnWidth / 2;
	int controlWidth = m_xc + m_firstColumnWidth + hgap;

	AddStaticIndirect("Flags", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, m_bottomControl += comboBoxHeight + vgap, widthHalf, staticHeight, staticSimIndexSnapshotFlagsId);
	int lbHeight = SF_Count * checkBoxHeight + 2 * vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, m_xc, m_bottomControl += staticHeight, widthHalf, lbHeight, listBoxSimIndexSnapshotFlagsId);

	AddStaticIndirect("Description", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, m_bottomControl += lbHeight + vgap, widthHalf, staticHeight, staticSimIndexSnapshotDescriptionId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, m_bottomControl + staticHeight, width, editHeight, editSimIndexSnapshotDescriptionId);

	m_bottomControl += editHeight;
/*
	m_xc = m_firstColumnWidth + m_firstColumnWidth + 4 * hgap;

	const int sizeWidth = 54;
	const int halfWidth = sizeWidth / 2;
	const int staticWidth = sizeWidth * 2;//3 / 2;
	const int secondColumnWidth = staticWidth;//sizeWidth;// + sideWidth;

	m_maxControlWidth += secondColumnWidth + 4 * hgap;

	int y = vgap;

	AddStaticIndirect("Round Lot", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y, staticWidth, staticHeight, staticSimSnapshotRoundLotId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, y += staticHeight, sizeWidth, spinHeight, spinSimSnapshotRoundLotId);

	AddButtonIndirect("Round Lots only", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX|WS_GROUP, 0, m_xc, y += spinHeight + vgap, staticWidth, checkBoxHeight, checkBoxSimSnapshotRoundLotsOnlyId);

	AddStaticIndirect("Market Category", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y += checkBoxHeight + vgap, staticWidth, staticHeight, staticSimSnapshotMarketCategoryId);
	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += staticHeight, sizeWidth, 400, comboBoxSimSnapshotMarketCategoryId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL, 0, m_xc, y += comboBoxHeight, halfWidth, 400, comboBoxSimSnapshotPrimaryExchangeId);
	AddStaticIndirect("Primary Exchange", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + halfWidth, y + (comboBoxHeight - staticHeight) / 2, secondColumnWidth - halfWidth, staticHeight, staticSimSnapshotPrimaryExchangeId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL|WS_GROUP, 0, m_xc, y += comboBoxHeight + vgap, halfWidth, 400, comboBoxSimSnapshotFinancialStatusId);
	AddStaticIndirect("Fin Status", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + halfWidth, y + (comboBoxHeight - staticHeight) / 2, secondColumnWidth - halfWidth, staticHeight, staticSimSnapshotFinancialStatusId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_SORT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_VSCROLL|WS_GROUP, 0, m_xc, y += comboBoxHeight + vgap, halfWidth, 400, comboBoxSimSnapshotTradingStateId);
	AddStaticIndirect("Trading State", WS_VISIBLE | SS_NOPREFIX, 0, m_xc + halfWidth, y + (comboBoxHeight - staticHeight) / 2, secondColumnWidth - halfWidth, staticHeight, staticSimSnapshotTradingStateId);

	AddStaticIndirect("Trading Action Reason", WS_VISIBLE | SS_NOPREFIX, 0, m_xc, y += comboBoxHeight + vgap, staticWidth, staticHeight, staticSimSnapshotTradingActionReasonId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL, 0, m_xc, y += staticHeight, sizeWidth, editHeight, editSimSnapshotTradingActionReasonId);
*/
}

BOOL SampleDialogSettingSimulateIndexSnapshot::OnInitDialog()
{
	m_description.clear();
	BOOL ret = BaseDialogSettingSimulateSymbol::OnInitDialog();

	m_EditDescription.SetLimitText(MAX_SECURITY_NAME);

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingSimulateIndexSnapshot::UpdateSettings()
{
/*
	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simSnapshotSymbolType.SetValue(symbolType);
*/
	extensionConfig->m_simIndexSnapshotSymbol.SetValue(m_symbol);
//	extensionConfig->m_simSnapshotSymbolMap.SetValue(m_symbolMap);

//	const unsigned char source = GetValidSource(m_source);

//	extensionConfig->m_simSnapshotSource.SetValue(source);

	extensionConfig->m_simIndexSnapshotFlags.SetValue(m_ListBoxSnapshotFlags.GetFilter());

	extensionConfig->m_simIndexSnapshotDescription.SetValue(m_description);

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateIndexSnapshot::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simIndexSnapshotSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_numericSymbol = U_RepresentStringAsUnsignedNumber<unsigned __int64>(m_symbol.c_str(), sizeof(unsigned __int64) - 1);

		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}
/*
	unsigned char symbolType = extensionConfig->m_simSnapshotSymbolType.GetValue();
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	if(symbolType != m_symbolType)
	{
		m_symbolType = symbolType;
		switch(m_symbolType)
		{
			case SYMT_SYMBOL_STRING:
			m_RadioSymbolMap.SetCheck(BST_UNCHECKED);
			m_RadioSymbol.SetCheck(BST_CHECKED);
			break;

			case SYMT_SYMBOL_MAPPING:
			m_RadioSymbol.SetCheck(BST_UNCHECKED);
			m_RadioSymbolMap.SetCheck(BST_CHECKED);
			break;
		}
		SymbolTypeChanged();
	}
	const unsigned short& symbolMap = extensionConfig->m_simSnapshotSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

	const unsigned char source = GetValidSource(extensionConfig->m_simSnapshotSource.GetValue());
	if(source != m_source)
	{
		m_source = source;
		m_ComboBoxSource.SetCurSel(m_source);
	}
*/
	m_ListBoxSnapshotFlags.SetFilter(extensionConfig->m_simIndexSnapshotFlags.GetValue());

	const std::string& description = extensionConfig->m_simIndexSnapshotDescription.GetValue();
	if(description != m_description)
	{
		m_description = description;
		m_EditDescription.SetWindowText(description.c_str());
		m_EditDescription.Invalidate(FALSE);
	}

	EnableApplyButton(false);
	EnableSendMessageButton();

	m_modified = false;
}

unsigned char SampleDialogSettingSimulateIndexSnapshot::FillAttachment()
{
	m_attachmentCursor = m_attachment;
	m_attachmentLength = 0;
	m_remainingLength = sizeof(m_attachment);
	unsigned short length;
	unsigned char steps = 0;
	if(!m_description.empty())
	{
		length = MAX_SECURITY_NAME;//(unsigned short)m_description.length();
		if(m_remainingLength <= length)return steps;

		steps |= SRS_DESCRIPTION;

		U_CopyAndPad(m_attachmentCursor, MAX_SECURITY_NAME, m_description.c_str(), '\0');
		MoveForwardInAttachment(length);
		*m_attachmentCursor = '\0';
		MoveForwardInAttachment(1);
	}
/*
	steps |= SRS_ATTRIBUTES;
	*(unsigned int*)m_attachmentCursor = m_roundLot;
	MoveForwardInAttachment(sizeof(unsigned int));

	*(bool*)m_attachmentCursor = m_roundLotsOnly;
	MoveForwardInAttachment(sizeof(bool));

	*m_attachmentCursor = TU_GetMarketCategoryCharByMarketCategoryId(m_marketCategory);
	MoveForwardInAttachment(sizeof(char));

	*m_attachmentCursor = m_financialStatus;
	MoveForwardInAttachment(sizeof(char));

	*m_attachmentCursor = m_tradingState;
	MoveForwardInAttachment(sizeof(char));

	*(unsigned int*)m_attachmentCursor = m_tradingActionReason;
	MoveForwardInAttachment(sizeof(unsigned int));

	*m_attachmentCursor = m_primaryExchange;
	MoveForwardInAttachment(sizeof(char));
*/
	return steps;
}

const Message* SampleDialogSettingSimulateIndexSnapshot::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled())
	{
		unsigned char steps = FillAttachment();
		wsaBuf.buf = m_attachment;
		wsaBuf.len = m_attachmentLength;
/*
		char msCode;
		if(!m_marketStatus || m_marketStatus >= m_marketStatusCount)
		{
			msCode = TD_GetMarketStatus();
		}
		else
		{
			const NameDescriptionItem* msItem = (const NameDescriptionItem*)m_ComboBoxMarketStatus.GetItemDataPtr(m_marketStatus);
			if(msItem)
			{
				msCode = *msItem->GetName().c_str();
			}
			else
			{
				msCode = TD_GetMarketStatus();
			}
		}
*/
		m_refreshIndex.m_requestId = TD_GetStockMarketSorterSubscriptionRequestId();
		m_refreshIndex.m_flags = m_ListBoxSnapshotFlags.GetRefreshFlags();//REFRESH_RefreshFailed;REFRESH_EndOfRefresh;REFRESH_EndOfBlock
		m_refreshIndex.m_length = sizeof(m_refreshIndex) + m_attachmentLength;
		m_refreshIndex.m_steps = steps;
		U_CopyAndPad(m_refreshIndex.m_symbol, sizeof(m_refreshIndex.m_symbol), m_symbol.c_str(), '\0');

		return &m_refreshIndex;
	}
	return nullptr;
}

void SampleDialogSettingSimulateIndexSnapshot::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSymbol::DoDataExchange(pDX);
	DDX_Control(pDX, listBoxSimIndexSnapshotFlagsId, m_ListBoxSnapshotFlags);
	DDX_Control(pDX, editSimIndexSnapshotDescriptionId, m_EditDescription);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateIndexSnapshot, BaseDialogSettingSimulateSymbol)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(editSimIndexSnapshotDescriptionId, OnDescriptionChange)
	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)
//	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()


LRESULT SampleDialogSettingSimulateIndexSnapshot::OnListBoxChecked(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case listBoxSimIndexSnapshotFlagsId:
		m_ListBoxSnapshotFlags.InvalidateClient();
//		UpdateOrderStatusMask();
		break;

		default:
//		BaseDialogSettingSimulateSymbol::OnListBoxChecked(w, l);
		break;

	}
	return 0;
}
/*
LRESULT SampleDialogSettingSimulateIndexSnapshot::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimSnapshotRoundLotId:
		{
			const unsigned int& roundLot = m_SpinRoundLot.GetValue();
			if(roundLot != m_roundLot)
			{
				m_roundLot = roundLot;
				m_modified = true;
			}
		}
		break;

		default:
			BaseDialogSettingSimulateSymbol::OnSpinChange(w, l);
		break;
	}
	return 0;
}
*/

void SampleDialogSettingSimulateIndexSnapshot::OnDescriptionChange()
{
	bool empty = m_description.empty();
	CString text;
	m_EditDescription.GetWindowText(text);
	if(strcmp(text, m_description.c_str()))
	{
		m_description = text;
		if(m_description.empty() != empty)
		{
			m_EditDescription.Invalidate(FALSE);
//			EnableSendMessageButton();
		}
		m_modified = true;
	}
}

bool SampleDialogSettingSimulateIndexSnapshot::isSendMessageButtonEnabled() const
{
	return 
//		MTR_IMBALANCE == m_messageType || 
		BaseDialogSettingSimulateSymbol::isSendMessageButtonEnabled();
/*
		&& (BOOK_LEVEL2 != m_book || m_marketCenter && m_messageId == M_BOOK_MODIFY_QUOTE)
		&& (!isPriceUsed() || !m_price.isZero())
		&& (!isSizeUsed() || m_size != 0 || BOOK_LEVEL2 == m_bookId);
*/
}

HBRUSH SampleDialogSettingSimulateIndexSnapshot::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSimIndexSnapshotDescriptionId:
		if(m_description.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
//			return m_emptySymbolValid ? *m_takionVisual->GetBlueBrush() : *m_takionVisual->GetWarningBrush();
			return *m_visualBase->GetBlueBrush();
		}
		break;

	}
	return BaseDialogSettingSimulateSymbol::OnCtlColor(pDC, pWnd, nCtlColor);
}
