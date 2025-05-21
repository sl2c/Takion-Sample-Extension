#include "stdafx.h"
#include "BaseDialogSettingSimulateMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//const int BaseDialogSettingSimulateMessage::defaultSendButtonWidth = 56;

BaseDialogSettingSimulateMessage::BaseDialogSettingSimulateMessage(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab, const char* label, int sendButtonWidth):
	BaseDialogSetting(mainWnd, parentTab, label),
	m_ButtonSendMessage("Shut down Takion"),
	m_bottomControl(0),
	m_xc(hgap),
	m_maxControlWidth(0),
	m_messageSimulationThread(nullptr),
	m_firstColumnWidth(58),//(52),
	m_firstColumnHalfWidth(m_firstColumnWidth / 2),
	m_sendButtonWidth(sendButtonWidth)
{
	m_bottomControl = vgap;
//	int buttonWidth = 56;
	AddButtonIndirect("Send Message", WS_VISIBLE|WS_DISABLED|BS_PUSHBUTTON|WS_TABSTOP, 0, m_xc, m_bottomControl, m_sendButtonWidth, buttonHeight, buttonSendMessageId);
	m_bottomControl += buttonHeight;

	if(m_maxControlWidth < m_sendButtonWidth)m_maxControlWidth = m_sendButtonWidth;
}

bool BaseDialogSettingSimulateMessage::OnSelchangeUChar(ComboBoxChar& comboBox, unsigned char& saleConditionVal)
{
	const unsigned char saleCondition = comboBox.GetSelectedChar();
	if(saleCondition != saleConditionVal)
	{
		bool ret = !saleCondition != !saleConditionVal;
		saleConditionVal = saleCondition;
		m_modified = true;
		return ret;
	}
	return false;
}

void BaseDialogSettingSimulateMessage::UpdateSettings()
{
//	BaseDialogSetting::UpdateSettings();
}

void BaseDialogSettingSimulateMessage::UpdateControls()
{
}

void BaseDialogSettingSimulateMessage::ReleaseMessageSimulationThread()
{
	if(m_messageSimulationThread)
	{
		TD_ReleaseMessageSimulationThread(m_messageSimulationThread);
		m_messageSimulationThread = nullptr;
	}
}
void BaseDialogSettingSimulateMessage::BeforeDestroy()
{
	ReleaseMessageSimulationThread();
	BaseDialogSetting::BeforeDestroy();
}
/*
void BaseDialogSettingSimulateMessage::Leaving()
{
}
*/

void BaseDialogSettingSimulateMessage::InitComboBoxNameDescription(ComboBoxNameDescription& comboBox, char& c)
{
	char buf[2];
	*buf = c;
	*(buf + 1) = '\0';
	int sel = comboBox.FindItemByName(buf);
	if(sel >= 0)comboBox.SetCurSel(sel);
	else
	{
		comboBox.SetCurSel(0);
		c = *((const NameDescriptionItem*)comboBox.GetItemDataPtr(0))->GetName().c_str();
	}
}

void BaseDialogSettingSimulateMessage::OnSelchangeNameDescription(ComboBoxNameDescription& comboBox, char& c, const char& defaultChar)
{
	int sel = comboBox.GetCurSel();
	const char ch = sel >= 0 ? *((const NameDescriptionItem*)comboBox.GetItemDataPtr(sel))->GetName().c_str() : defaultChar;
	if(ch != c)
	{
		c = ch;
		m_modified = true;
	}
}

void BaseDialogSettingSimulateMessage::InitComboBoxChar(ComboBoxChar& comboBox, char& c)
{
	comboBox.AddString(nullptr);
	comboBox.AddString((const char*)' ');
	comboBox.FillCharRange('A', 'Z', false);
	comboBox.FillCharRange('0', '9', false);
	int sel = comboBox.FindItem((const void*)c);
	if(sel >= 0)comboBox.SetCurSel(sel);
	else
	{
		comboBox.SetCurSel(0);
		c = (char)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			comboBox.GetItemDataPtr(0);
	}
}

void BaseDialogSettingSimulateMessage::OnSelchangeChar(ComboBoxChar& comboBox, char& c)
{
	int sel = comboBox.GetCurSel();
	const char ch = sel >= 0 ? (char)
#if (_MSC_VER > 1600)
		(size_t)
#endif
		comboBox.GetItemDataPtr(sel) : '\0';
	if(ch != c)
	{
		c = ch;
		m_modified = true;
	}
}

BOOL BaseDialogSettingSimulateMessage::OnInitDialog()
{
	BOOL ret = BaseDialogSetting::OnInitDialog();
	if(!m_messageSimulationThread)
	{
		m_messageSimulationThread = TD_ObtainMessageSimulationThread();
	}
	EnableSendMessageButton();
	return ret;
}

void BaseDialogSettingSimulateMessage::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
	DDX_Control(pDX, buttonSendMessageId, m_ButtonSendMessage);
}

BEGIN_MESSAGE_MAP(BaseDialogSettingSimulateMessage, BaseDialogSetting)
	ON_BN_CLICKED(buttonSendMessageId, OnSendMessage)
END_MESSAGE_MAP()

void BaseDialogSettingSimulateMessage::FillCharRange(ComboBoxChar& comboBox)
{
	comboBox.AddString(nullptr);
	comboBox.FillCharRange('A', 'Z', false);
	comboBox.SetCurSel(0);
}

void BaseDialogSettingSimulateMessage::SetUnsignedCharInComboBox(ComboBoxChar& comboBox, const unsigned char& val, unsigned char& valRef)
{
//	const unsigned char& saleCondition1 = extensionConfig->m_simPrintSaleCondition1.GetValue();
	if(val != valRef)
	{
		valRef = val;
		int sel = comboBox.FindItem((const void*)valRef);
		if(sel >= 0)
		{
			comboBox.SetCurSel(sel);
		}
		else
		{
			comboBox.SetCurSel(0);
			valRef = comboBox.GetSelectedChar();
		}
	}
}

void BaseDialogSettingSimulateMessage::SetCharInComboBox(ComboBoxChar& comboBox, const char& val, unsigned char& valRef)
{
//	const unsigned char& saleCondition1 = extensionConfig->m_simPrintSaleCondition1.GetValue();
	if(val != valRef)
	{
		valRef = val;
		int sel = comboBox.FindItem((const void*)valRef);
		if(sel >= 0)
		{
			comboBox.SetCurSel(sel);
		}
		else
		{
			comboBox.SetCurSel(0);
			valRef = comboBox.GetSelectedChar();
		}
	}
}

void BaseDialogSettingSimulateMessage::OnSendMessage()
{
/*
	const Message* message = CreateMessage();
	if(message)
	{
		MessageThread* messageSimulationThread = TD_ObtainMessageSimulationThread();
		messageSimulationThread->WriteMessage(message);
		TD_ReleaseMessageSimulationThread(messageSimulationThread);
	}
/**/
	if(m_messageSimulationThread && isSendMessageButtonEnabled())
	{
//		unsigned short additionalBufferLength;
//		const char* additionalBuffer;
		WSABUF wsabuf[2];
		WSABUF* second = wsabuf + 1;
		second->buf = nullptr;
		second->len = 0;
		const Message* message = CreateMessage(*second);
		if(message)
		{
			if(second->buf && second->len)
			{
				if((ULONG)message->m_length > second->len)
				{
					wsabuf->buf = (char*)message;
					wsabuf->len = message->m_length - (unsigned short)second->len;
					m_messageSimulationThread->WriteBuffers(wsabuf, 2, message->m_length);
				}
			}
			else
			{
				m_messageSimulationThread->WriteMessage(message);
			}
			MessageSent();
		}
	}
}
/*
void BaseDialogSettingSimulateMessage::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void BaseDialogSettingSimulateMessage::OnSymbolChange()
{
	bool empty = m_symbol.empty();
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_symbol = text;
	if(m_symbol.empty() != empty)
	{
		m_EditSymbol.Invalidate();
	}
}

HBRUSH BaseDialogSettingSimulateMessage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
		case editSymbolId:
		if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_takionVisual->GetWarningBrush();
		}
		break;

	}
	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}
*/