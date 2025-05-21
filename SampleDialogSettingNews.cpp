#include "stdafx.h"
#include "SampleDialogSettingNews.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

class LbNewsHorizontalScroller : public LbHorizontalScroller
{
public:
	LbNewsHorizontalScroller(ListBoxOwnerDraw* listBox):LbHorizontalScroller(listBox){}
	virtual void AddColumns() override;
};

class SampleHeadline
{
public:
	SampleHeadline(const NewsHeadline* headline):
		m_headline(headline),
		m_dateTime(headline->GetDateTime()),
		m_dateTimePublished(headline->GetDateTimePublished()),
		m_ordinal(headline->GetOrdinal()),
		m_alertTakeNumber(headline->GetAlertTakeNumber()),
		m_relevance(headline->GetRelevance()),
		m_displayWireId(headline->GetExtDisplayWireId()),
		m_subWireId(headline->GetSubWireId()),
		m_headlineType(headline->GetHeadlineType()),
		m_resourceId(headline->GetResourceId()),
		m_text(headline->GetText()),
		m_tickerSymbols(headline->GetTickerSymbols()),
		m_storyCodes(headline->GetValuedStoryCodes()),
		m_providerService(headline->GetProviderService()),
		m_synopsis(headline->GetSynopsis()),
		m_symbol(headline->GetSymbol())
	{
	}
	const NewsHeadline* GetHeadline() const{return m_headline;}
	const unsigned __int64& GetDateTime() const{return m_dateTime;}
	void SetDateTime(const unsigned __int64& dateTime){m_dateTime = dateTime;}
	const unsigned __int64& GetDateTimePublished() const{return m_dateTimePublished;}
	const unsigned int& GetOrdinal() const{return m_ordinal;}
	const unsigned int& GetAlertTakeNumber() const{return m_alertTakeNumber;}
	const unsigned int& GetRelevance() const{return m_relevance;}
	const unsigned int& GetDisplayWireId() const{return m_displayWireId;}
	const unsigned short& GetSubWireId() const{return m_subWireId;}
	const unsigned char& GetHeadlineType() const{return m_headlineType;}
	const NewsResourceAsNumberArray& GetResourceId() const{return m_resourceId;}
	const std::string& GetText() const{return m_text;}
	const std::string& GetTickerSymbols() const{return m_tickerSymbols;}
	const std::string& GetStoryCodes() const{return m_storyCodes;}
	const std::string& GetProviderService() const{return m_providerService;}
	const std::string& GetSynopsis() const{return m_synopsis;}
	const std::string& GetSymbol() const{return m_symbol;}
	DECLARE_NED_NEW
protected:
	const NewsHeadline* m_headline;
	unsigned __int64 m_dateTime;
	unsigned __int64 m_dateTimePublished;
	unsigned int m_ordinal;
	unsigned int m_alertTakeNumber;
	unsigned int m_relevance;
	unsigned int m_displayWireId;
	unsigned short m_subWireId;
	unsigned char m_headlineType;
	NewsResourceAsNumberArray m_resourceId;
	std::string m_text;
	std::string m_tickerSymbols;
	std::string m_storyCodes;
	std::string m_providerService;
	std::string m_synopsis;
	std::string m_symbol;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool UnappliedHeadlineReader::ProcessDataToken(const char* cursor, unsigned short length) const
{
	return m_window->ProcessUnappliedMessage((const Message*)cursor);
}

void ListBoxNewsStock::AddMenuItems(CMenu& menu)
{
	AddInOutMenuItems(menu);
/*
	bool hasItems = GetCount() > 0;
	int sel = GetCurSel();
	bool selected = sel >= 0;

//	menu.AppendMenu(MF_STRING, TMID_CLIPBOARD, "Copy");
//	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, TMID_ADD, "Add");
//	bool clipboardAvailable = IsClipboardFormatAvailable(CF_TEXT) == TRUE;
//	menu.AppendMenu(MF_STRING|(clipboardAvailable ? MF_ENABLED : MF_GRAYED), TMID_PASTE, "Paste Add");
//	menu.AppendMenu(MF_STRING|(clipboardAvailable ? MF_ENABLED : MF_GRAYED), TMID_REPLACE, "Paste Replace");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING|(selected ? MF_ENABLED : MF_GRAYED), TMID_REMOVE, "Remove");
	menu.AppendMenu(MF_STRING|(hasItems ? MF_ENABLED : MF_GRAYED), TMID_CLEAR, "Clear");
*/
}

bool ListBoxNewsStock::ContentsToString(std::string& info) const
//void ListBoxNewsStock::CopySymbols(std::string& info) const
{
	bool first = true;
	for(unsigned int i = 0, end = m_itemCount; i < end; ++i)
	{
		if(first)
		{
			first = false;
		}
		else
		{
//			info += '\r';
			info += '\n';
		}
		info += GetItemNameAt(i);//GetItemName(GetItemDataPtr(i));
	}
	return true;
}

bool ListBoxNewsStock::CreateBasketFilterString(std::string& filter) const
{
	filter.clear();
	const unsigned int end = m_itemCount;//GetCount();
	if(end > 0 && end != 0xFFFFFFFF)
	{
		for(unsigned int i = 0; i < end; ++i)
		{
			if(!filter.empty())
			{
				filter += ' ';
			}
			filter += '/';
			filter += GetItemNameAt(i);//GetItemName(GetItemDataPtr(i));
		}
		return true;
	}
	return false;
}

bool ListBoxNewsStock::isSameStockSet(const StockSet& stockSet) const
{
	const unsigned int end = m_itemCount;//(unsigned int)GetCount();
	if(end != (unsigned int)stockSet.size())
	{
		return false;
	}
	else
	{
		unsigned int i = 0;
		
		for(StockSet::const_iterator it = stockSet.begin(), itend = stockSet.end(); it != itend && i < end; ++it, ++i)
		{
			if(*it != (ValueType)GetItemDataPtr(i))
			{
				return false;
			}
		}
	}
	return true;
}

bool ListBoxNewsStock::GetStockSet(StockSet& stockSet) const
{
	bool ret = false;
	StockSet::iterator it = stockSet.begin(), itend = stockSet.end();
	unsigned int i = 0;
	const unsigned int end = m_itemCount;//(const unsigned int)GetCount();
	ValueType value;
	for(;;)
	{
		if(it == itend)
		{
			if(i < end)
			{
				ret = true;
				stockSet.insert((ValueType)GetItemDataPtr(i));
				++i;
			}
			else
			{
				break;
			}
		}
		else if(i < end)
		{
			value = (ValueType)GetItemDataPtr(i);
			switch(U_CompareUIntAsString(*it, value))
			{
				case -1:
				ret = true;
				it = stockSet.erase(it);
				break;

				case 1:
				ret = true;
				it = stockSet.insert(value).first;
//fall thriugh
				default:
				++it;
				++i;
				break;
			}
		}
		else
		{
			ret = true;
			stockSet.erase(it, itend);
			break;
		}
	}
	return ret;
}

bool ListBoxNewsStock::SetStockSet(const StockSet& stockSet)
{
	bool ret = false;
	StockSet::const_iterator it = stockSet.begin(), itend = stockSet.end();
	unsigned int i = 0;
	unsigned int end = m_itemCount;//(unsigned int)GetCount();
	ValueType value;
	unsigned char added;

	SetRedraw(FALSE);
	for(;;)
	{
		if(it == itend)
		{
			if(i < end)
			{
				ret = true;
				RemoveItemAt(i);//DeleteString(i);
				--end;
			}
			else
			{
				break;
			}
		}
		else if(i < end)
		{
			value = (ValueType)GetItemDataPtr(i);
			switch(U_CompareUIntAsString(*it, value))
			{
				case 1:
				ret = true;
				RemoveItemAt(i);//DeleteString(i);
				--end;
				break;

				case -1:
				ret = true;
				AddItem(value, false, added);
				++end;
//fall thriugh
				default:
				++it;
				++i;
				break;
			}
		}
		else
		{
			ret = true;
			AddItem(*it, false, added);
			++it;
		}
	}
	SetRedraw(TRUE);
	Invalidate(FALSE);
	return ret;
}

bool ListBoxNewsStock::StringToContents(const char* contents, size_t contentsLength)
{
//	std::string str;
//	if(TU_CopyClipboardToString(str, m_hWnd) == CLIPBOARD_OK)
//	{
		StringMap stringMap;
		unsigned int duplicateCount;
		unsigned int emptyCount;
//		U_ParseStringToMap(str.c_str(), str.length(), stringMap, '\n', &duplicateCount, &emptyCount);
		U_ParseStringToMap(contents, contentsLength, stringMap, '\n', &duplicateCount, &emptyCount);
		PasteStocks(stringMap);
//	}
	return true;
}

bool ListBoxNewsStock::PasteStocks(StringMap& stringMap)//, bool replace)
{
	bool ret = false;
	StringMap::const_iterator it;
	StringMap::const_iterator itend = stringMap.end();
	std::string token;
	SetRedraw(FALSE);
/*
	const char* symbol;
	for(unsigned int i = 0, end = GetCount(); i < end;)
	{
		symbol = GetItemNameAt(i);//GetItemName(GetItemDataPtr(i));
		it = stringMap.find(symbol);
		if(it == itend)
		{
			if(replace)
			{
				RemoveItemAt(i);//DeleteString(i);
				--end;
				ret = true;
			}
			else
			{
				++i;
			}
		}
		else
		{
			++i;
			stringMap.erase(it);
		}
	}
*/
	ValueType value;
	unsigned char added;
	for(it = stringMap.begin(); it != itend; ++it)
	{
		const std::string& sym = it->first;
		if(!sym.empty())
		{
			value = U_RepresentStringAsUnsignedNumber<ValueType>(it->first.c_str(), sizeof(ValueType) - 1);
			AddItem(value, false, added);
			if(added)
			{
				ret = true;
			}
		}
	}
	SetRedraw(TRUE);
	Invalidate(FALSE);
	return ret;
}

DialogTextEntry& ListBoxNewsStock::DisplayStockEntryField()
{
	CFont* font = GetFont();
	DialogTextEntry& symbolEntryDialog = TGUI_GetTakionMainWnd()->GetSymbolEntryDialog();
	CPoint point;
	point.x = m_owner->GetNewsStockLeftOffset();
	point.y = m_owner->GetNewsStockTopOffset();
	m_owner->ClientToScreen(&point);
	symbolEntryDialog.Display(true, true, nullptr, this, 0, this,
		U_GetValidStockChars(), false, true, U_GetValidStockFirstChars(), false, ".",
		RGB(255, 0, 255), 0, 7, font, point.x, point.y, 1, 1);
	return symbolEntryDialog;
}

BEGIN_MESSAGE_MAP(ListBoxNewsStock, ListBoxStringAsUInt<unsigned __int64>)
	ON_MESSAGE(WM_USER + 999, OnStockAdd)
END_MESSAGE_MAP()

void ListBoxNewsStock::DoRemoveStockAt(int sel)
{
	RemoveItemAt(sel);//DeleteString(sel);
	unsigned int usel = (unsigned int)sel;
	unsigned int count = m_itemCount;//(unsigned int)GetCount();

	if(usel < count)SetCurSel(usel);
	else if(count)SetCurSel(count - 1);

	m_owner->OnStocksChanged();
}

int ListBoxNewsStock::RemoveCurrentStock()
{
	int sel = GetCurSel();
	if(sel >= 0)
	{
		DoRemoveStockAt(sel);
	}
	return sel;
}

bool ListBoxNewsStock::ClearStocks()
{
	if(ClearItemsWithRedraw())
	{
		m_owner->OnStocksChanged();
		return true;
	}
	return false;
}

int ListBoxNewsStock::FindStock(const char* symbol)
{
	int sel = -1;
	if(symbol && *symbol)
	{
		ValueType value = U_RepresentStringAsUnsignedNumber<ValueType>(symbol, sizeof(ValueType) - 1);
		sel = FindItem((const void*)value);
		SetCurSel(sel);
	}
	return sel;
}

bool ListBoxNewsStock::AddStock(const char* symbol)
{
	bool ret = false;
	if(symbol && *symbol)
	{
		ValueType value = U_RepresentStringAsUnsignedNumber<ValueType>(symbol, sizeof(ValueType) - 1);
		unsigned char added;
		int sel = AddItem(value, false, added);
		if(added)
		{
			ret = true;
			SetCurSel(sel);
			m_owner->OnStocksChanged();
		}
	}
	return ret;
}

int ListBoxNewsStock::AddItemOnMenuCommand()
{
	DisplayStockEntryField().ExpandForChar('A');
	return -2;
}

bool ListBoxNewsStock::RemoveSelectedItemOnMenuCommand(int sel)
{
	DoRemoveStockAt(sel);
//	DeleteString(sel);
	return false;
}

bool ListBoxNewsStock::ClearItemsOnMenuCommand()
{
	ClearStocks();
	return false;
//	return ClearItemsWithRedraw();
}

LRESULT ListBoxNewsStock::OnStockAdd(WPARAM spinId, LPARAM l)
{
	const std::string& textResult = TGUI_GetTakionMainWnd()->GetSymbolEntryDialog().GetTextResult();
	AddStock(textResult.c_str());
/*
	if(!textResult.empty())
	{
		ValueType value = U_RepresentStringAsUnsignedNumber<ValueType>(textResult.c_str(), sizeof(ValueType) - 1);
		int sel = FindItem((const void*)value);
		if(sel < 0)
		{
			sel = AddItem(value);
			if(sel >= 0)m_owner->OnStocksChanged();
		}
		SetCurSel(sel);
	}
*/
	return 0;
}

/*
BOOL ListBoxNewsStock::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD loword = (DWORD)wParam;//LOWORD(wParam);
    switch(loword)
    {
		case TMID_ADD:
		DisplayStockEntryField().ExpandForChar('A');
		break;

		case TMID_CLIPBOARD:
		{
			std::string info;
			CopySymbols(info);
			TU_CopyStringToClipboard(info, m_hWnd);
		}
		break;

		case TMID_REPLACE://replace
		{
			StringMap stringMap;
			unsigned int duplicateCount;
			unsigned int emptyCount = TU_CopyClipboardToStringMap(stringMap, m_hWnd, '\n', &duplicateCount);
			if(PasteStocks(stringMap, true))m_owner->OnStocksChanged();
		}
		break;

		case TMID_PASTE:
		{
			StringMap stringMap;
			unsigned int duplicateCount;
			unsigned int emptyCount = TU_CopyClipboardToStringMap(stringMap, m_hWnd, '\n', &duplicateCount);
			if(PasteStocks(stringMap, false))m_owner->OnStocksChanged();
		}
		break;

		case TMID_REMOVE:
		RemoveCurrentStock();
		break;

		case TMID_CLEAR:
		ClearStocks();
		break;

		default:
		return CListBox::OnCommand(wParam, lParam);
	}
	return TRUE;
}
*/
///////////////
int ListBoxNews::Compare(const void* item1, const void* item2) const
{
	const SampleHeadline* h1 = (const SampleHeadline*)item1;
	const SampleHeadline* h2 = (const SampleHeadline*)item2;
	const unsigned __int64& datetime1 = h1->GetDateTime();
	const unsigned __int64& datetime2 = h2->GetDateTime();
	if(datetime1 < datetime2)
	{
		return 1;
	}
	else if(datetime1 > datetime2)
	{
		return -1;
	}
	else
	{
		const unsigned int& ordinal1 = h1->GetOrdinal();
		const unsigned int& ordinal2 = h2->GetOrdinal();
		if(ordinal1 < ordinal2)
		{
			return 1;
		}
		else if(ordinal1 > ordinal2)
		{
			return -1;
		}
		else
		{
			int res = strcmp(h1->GetResourceId().GetString(), h2->GetResourceId().GetString());
#ifdef _DEBUG
if(!res)
{
printf("X");
}
#endif
			return res < 0 ? -1:
				res > 0 ? 1:
//				h1 < h2 ? -1:
//				h1 > h2 ? 1:
				0;
/*
			if(res)
			{
				return res < 0 ? -1 : 1;
			}
			return h1 < h2 ? -1:
				h1 > h2 ? 1:
				0;
*/
		}
	}
	return 0;
}

void ListBoxNews::DestroyItem(const void* item) const
{
	delete (SampleHeadline*)item;
}

void* ListBoxNews::CloneItem(const void* item) const
{
	return new SampleHeadline(*(const SampleHeadline*)item);
}

LbHorizontalScroller* ListBoxNews::CreateHorizontalScroller()
{
	return new LbNewsHorizontalScroller(this);
}

void LbNewsHorizontalScroller::AddColumns()
{
	const int itemHeight = m_listBox->GetMyItemHeight();
	const int doubleHeight = itemHeight + itemHeight;
	const int tripleHeight = doubleHeight + itemHeight;
	const int fourHeight = tripleHeight + itemHeight;
	const int fiveHeight = fourHeight + itemHeight;
	const int sixHeight = fiveHeight + itemHeight;
	const int sevenHeight = sixHeight + itemHeight;
	const int eightHeight = sevenHeight + itemHeight;
	const int fifteenHeight = eightHeight + sevenHeight;
	const int sixteenHeight = eightHeight + eightHeight;
	const int twentyfourHeight = sixteenHeight + eightHeight;
	const int fourtyeightHeight = twentyfourHeight + twentyfourHeight;
	AddColumn("DateTime", NHCI_DATETIME, sevenHeight, DT_RIGHT, DT_CENTER, DT_RIGHT, "Date and Time");//, 0xFFFFFFFF, 0xFFFFFFFF);//, itemHeight, itemHeight, itemHeight);
	AddColumn("DateTime Pub", NHCI_DATETIME_PUBLISHED, sevenHeight, DT_RIGHT, DT_CENTER, DT_RIGHT, "Date and Time Published");
	AddColumn("Ord", NHCI_ORDINAL, tripleHeight, DT_RIGHT, DT_CENTER, DT_RIGHT, "Ordinal");
	AddColumn("Alert", NHCI_ALERT, tripleHeight, DT_RIGHT, DT_CENTER, DT_RIGHT, "Alert Take Number");
	AddColumn("Rel", NHCI_RELEVANCE, doubleHeight, DT_RIGHT, DT_CENTER, DT_RIGHT, "Relevance", 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, 0, itemHeight);
	AddColumn("Wire", NHCI_WIRE, tripleHeight, DT_LEFT, DT_CENTER, DT_LEFT, "Wire ID");
	AddColumn("Sub", NHCI_SUBWIRE, tripleHeight, DT_LEFT, DT_CENTER, DT_LEFT, "Sub Wire ID");
	AddColumn("Type", NHCI_TYPE, doubleHeight, DT_RIGHT, DT_CENTER, DT_RIGHT, "Headline Type", 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, 0, itemHeight);

	AddColumn("Text", NHCI_TEXT, sixteenHeight, DT_LEFT, DT_CENTER, DT_LEFT, "Headline Type");//, 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, itemHeight, itemHeight);
	AddColumn("Symbol", NHCI_SYMBOL, fiveHeight, DT_LEFT, DT_CENTER, DT_LEFT, "Symbol", 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, itemHeight, itemHeight);
	AddColumn("Synopsis", NHCI_SYNOPSIS, sixteenHeight, DT_LEFT, DT_CENTER, DT_LEFT, "Synopsis", 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, itemHeight, itemHeight);
	AddColumn("Codes", NHCI_CODES, eightHeight, DT_LEFT, DT_CENTER, DT_LEFT, "Story Codes", 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, itemHeight, itemHeight);
	AddColumn("Services", NHCI_PROVIDER_SERVICES, eightHeight, DT_LEFT, DT_CENTER, DT_LEFT, "Provider Services", 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, itemHeight, itemHeight);
	AddColumn("Ticker", NHCI_SYMBOLS, eightHeight, DT_LEFT, DT_CENTER, DT_LEFT, "Symbols", 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, itemHeight, itemHeight);
	AddColumn("Resource", NHCI_RESOURCE, fourtyeightHeight, DT_LEFT, DT_LEFT, DT_LEFT, "Resource ID", 0xFFFFFFFF, 0xFFFFFFFF, itemHeight, itemHeight, itemHeight);
}

void ListBoxNews::DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment)
{
	const SampleHeadline* headlineItem = (const SampleHeadline*)item;
	std::string text;
	char num[33];
	switch(columnId)
	{
		case NHCI_DATETIME:
		U_AppendDateTime(text, headlineItem->GetDateTime(), false, false, '-', '\0', false, false, false);
		DrawText(hDC, text.c_str(), -1, &r, alignment);
		break;

		case NHCI_DATETIME_PUBLISHED:
		U_AppendDateTime(text, headlineItem->GetDateTimePublished(), false, false, '-', '\0', false, false, false);
		DrawText(hDC, text.c_str(), -1, &r, alignment);
		break;

		case NHCI_ORDINAL:
		DrawText(hDC, U_UnsignedNumberToStr(headlineItem->GetOrdinal(), num, sizeof(num)), -1, &r, alignment);
		break;

		case NHCI_ALERT:
		DrawText(hDC, U_UnsignedNumberToStr(headlineItem->GetAlertTakeNumber(), num, sizeof(num)), -1, &r, alignment);
		break;

		case NHCI_RELEVANCE:
		DrawText(hDC, U_UnsignedNumberToStr(headlineItem->GetRelevance(), num, sizeof(num)), -1, &r, alignment);
		break;

		case NHCI_WIRE:
		U_AppendUnsignedNumberAsString(text, headlineItem->GetDisplayWireId());
		DrawText(hDC, text.c_str(), -1, &r, alignment);
		break;

		case NHCI_SUBWIRE:
		U_AppendUnsignedNumberAsString(text, headlineItem->GetSubWireId());
		DrawText(hDC, text.c_str(), -1, &r, alignment);
		break;

		case NHCI_TYPE:
		DrawText(hDC, U_UnsignedNumberToStr(headlineItem->GetHeadlineType(), num, sizeof(num)), -1, &r, alignment);
		break;

		case NHCI_TEXT:
		DrawText(hDC, headlineItem->GetText().c_str(), -1, &r, alignment & ~DT_NOCLIP);
		break;

		case NHCI_SYMBOL:
		DrawText(hDC, headlineItem->GetSymbol().c_str(), -1, &r, alignment);
		break;

		case NHCI_SYNOPSIS:
		DrawText(hDC, headlineItem->GetSynopsis().c_str(), -1, &r, alignment & ~DT_NOCLIP);
		break;

		case NHCI_CODES:
		DrawText(hDC, headlineItem->GetStoryCodes().c_str(), -1, &r, alignment & ~DT_NOCLIP);
		break;

		case NHCI_PROVIDER_SERVICES:
		DrawText(hDC, headlineItem->GetProviderService().c_str(), -1, &r, alignment & ~DT_NOCLIP);
		break;

		case NHCI_SYMBOLS:
		DrawText(hDC, headlineItem->GetTickerSymbols().c_str(), -1, &r, alignment & ~DT_NOCLIP);
		break;

		case NHCI_RESOURCE:
		DrawText(hDC, headlineItem->GetResourceId().GetString(), -1, &r, alignment & ~DT_NOCLIP);
		break;
	}
}

enum SampleSettingNewsIds
{
	listBoxNewsId = 10000,

	checkBoxAllEntitledId,

	staticWireFilterId,
	listBoxWireFilterId,

	staticStoryFilterId,
	listBoxStoryFilterId,

	staticMetaFilterId,
	listBoxMetaFilterId,

	staticDayBackId,
	spinDayBackId,

	staticCountLimitId,
	spinCountLimitId,

	checkBoxAfterMarketId,

	radioUnfilteredId,
	radioAllId,
	radioAllUsId,
	radioTopicId,
	comboBoxTopicId,
	radioStockId,

	listBoxStockId,
	editSymbolId,
	buttonFindStockId,
	buttonAddStockId,
	buttonRemoveStockId,
	buttonClearStocksId,

	checkBoxShowStoryChainId,
	buttonRequestStoryId,
	editStoryResourceId,
	editStoryId,

	buttonSubscribeId,
	buttonResetId,

	newsIdsCount
};

SampleDialogSettingNews::SampleDialogSettingNews(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSetting(mainWnd, parentTab, "News"),

	m_newsDll(TD_GetNewsDll()),
	m_newsCollection(nullptr),
	m_stockNews(nullptr),

	m_allWireNamesEnd(m_allWireNames.end()),
	m_newsWireMapEnd(m_newsWireMap.end()),
	m_newsWireEntitledMapEnd(m_newsWireEntitledMap.end()),
	m_newsTopicMapEnd(m_newsTopicMap.end()),

	m_wireSetEnd(m_wireSet.end()),
	m_entitledWireSetEnd(m_entitledWireSet.end()),

	m_storyFilter(0),
	m_metaFilter(0),
	m_relevance(0),
	m_currentRelevance(m_relevance),

	m_ListBoxWire(m_wireSet),
	m_ListBoxStory(3, true),
	m_ListBoxMeta(2, true),
	m_SpinDaysBack(999, 0, 0, 100),
	m_SpinCountLimit(999999, 0, 100, 1000),
	m_ComboBoxTopic(false),
	m_ListBoxStock(this),
	m_EditSymbol(U_GetValidStockChars(), false),

	m_listBoxBorderSize2(0),
	m_listBoxBorderSize(0),
	m_leftOffset(0),
	m_titleOffset(0),
	m_topOffset(0),
	m_bottomOffset(0),
	m_stockWidth(0),
	m_stockLeftOffset(0),
	m_stockTopOffset(0),
	m_maxStockHeight(0),
	m_stockHeight(0),
	m_storyWidth(0),
	m_storyTopOffset(0),

	m_stockCountRect(0, 0, 0, 0),

	m_basket(false),

	m_newsConnected(false),
//	m_noProfile(true),
	m_showStoryChain(false),
//	m_displayLastHistoricStory(false),
	m_selectedHeadlineStoryDisplayed(false),
	m_storyError(false),

	m_searchType(SEA_ALL),
	m_currentSearchType(m_searchType),
	m_allEntitled(true),
	m_daysBack(0),
//	m_currentDaysBack(m_daysBack),
	m_headlineCount(100),
//	m_currentHeadlineCount(m_headlineCount)

	m_afterMarket(false),
	m_currentAfterMarket(m_afterMarket),

	m_addedVisibleHeadline(false),

	m_topicOrdinal(0xFFFF),
	m_currentTopicOrdinal(0xFFFF),

	m_stockSetEnd(m_stockSet.end()),
	m_unappliedHeadlineReader(this)
{
	m_SpinDaysBack.AddSpinCell(1, false);
	m_SpinDaysBack.AddSpinCell(100, true);

	m_SpinCountLimit.AddSpinCell(1, false);
	m_SpinCountLimit.AddSpinCell(10, false);
	m_SpinCountLimit.AddSpinCell(1000, true);
	m_SpinCountLimit.AddSpinCell(100, true);

	m_ListBoxStory.AddStringItem("Child", NSF_CHILD);
	m_ListBoxStory.AddStringItem("HeadlineOnly", NSF_HEADLINE_ONLY);
	m_ListBoxStory.AddStringItem("Temporary", NSF_TEMPORARY);

	m_ListBoxMeta.AddStringItem("Symbols", NMF_SYMBOLS);
	m_ListBoxMeta.AddStringItem("Codes", NMF_CODES);

	m_ComboBoxTopic.SetNoSelectionColor(m_visualBase->GetWarningColor());

	int x = 0;
	int y = vgap;
	int width = 140;
	int halfWidth = (width - hgap) / 2;
	int xc = x;// + hgap;
	const int wireWidth = 20;
///////////////
	AddStaticIndirect("Wire", WS_VISIBLE | SS_NOPREFIX | SS_CENTERIMAGE, 0, xc, y, wireWidth, staticHeight, staticWireFilterId);
//	AddButtonIndirect("Select all enabled", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc + wireWidth + hgap, y, width - wireWidth - hgap, checkBoxHeight, checkBoxAllEntitledId);
	AddButtonIndirect("Select entitled", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xc + wireWidth + hgap, y, halfWidth, checkBoxHeight, checkBoxAllEntitledId);

	int h = 7 * itemHeight + vgap + vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_SORT | WS_VSCROLL, 0, xc,
		y += staticHeight, width, h, listBoxWireFilterId);

	AddStaticIndirect("Story", WS_VISIBLE | SS_NOPREFIX | SS_CENTERIMAGE, 0, xc, y += h + vgap, halfWidth, staticHeight, staticStoryFilterId);

	const int halfX = xc + halfWidth + hgap;
	AddStaticIndirect("Meta Data", WS_VISIBLE | SS_NOPREFIX | SS_CENTERIMAGE, 0, halfX, y, halfWidth, staticHeight, staticMetaFilterId);

	h = 3 * itemHeight + vgap + vgap;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc,
		y += staticHeight, halfWidth, h, listBoxStoryFilterId);

//	h = 2 * itemHeight + gapv + gapv;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0, halfX,
		y, halfWidth, h, listBoxMetaFilterId);

//	AddButtonIndirect("Subscribe", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xc + width - halfWidth, y + h + vgap, halfWidth, comboBoxHeight, buttonSubscribeId);
///////////////
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += h + checkBoxHeight, spinWidth, spinHeight, spinDayBackId);
	AddStaticIndirect("Days", WS_VISIBLE | SS_NOPREFIX, 0, xc + spinWidth, y + (spinHeight - staticHeight) / 2, spinWidth, staticHeight, staticDayBackId);

	AddButtonIndirect("AfterMarket", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, 0, y += spinHeight + vgap, spinWidth + spinWidth, checkBoxHeight, checkBoxAfterMarketId);

	const int spinWidth32 = (spinWidth + spinWidth + spinWidth) / 2;
//	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += spinHeight + vgap, spinWidth, spinHeight, spinCountLimitId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, xc, y += checkBoxHeight + vgap, spinWidth32, spinHeight, spinCountLimitId);
	AddStaticIndirect("Headlines", WS_VISIBLE | SS_NOPREFIX, 0, xc + spinWidth32, y + (spinHeight - staticHeight) / 2, width - spinWidth32, staticHeight, staticCountLimitId);
///////////////
/*
	AddButtonIndirect("Unfiltered", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += spinHeight + checkBoxHeight, spinWidth + spinHeight, checkBoxHeight, radioUnfilteredId);
	AddButtonIndirect("All", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap, spinWidth, checkBoxHeight, radioAllId);
	AddButtonIndirect("All US", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap, spinWidth, checkBoxHeight, radioAllUsId);
	AddButtonIndirect("Topic", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap, spinWidth, checkBoxHeight, radioTopicId);
*/
	int buttonWidth = halfWidth - comboBoxHeight;
	int chbWidth = width - buttonWidth - hgap;
	AddButtonIndirect("All Global News", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON|WS_GROUP|WS_TABSTOP, 0, xc, y += spinHeight + checkBoxHeight, chbWidth, checkBoxHeight, radioUnfilteredId);
	AddButtonIndirect("Global Companies", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap, chbWidth, checkBoxHeight, radioAllId);
	AddButtonIndirect("Global US Companies", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap, chbWidth, checkBoxHeight, radioAllUsId);
	AddButtonIndirect("Topic", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, y += checkBoxHeight + vgap, spinWidth, checkBoxHeight, radioTopicId);

	h = spinHeight * 20;//7 * itemHeight + vgap + vgap;

	int stockY = y + checkBoxHeight + comboBoxHeight + vgap;
	AddButtonIndirect("Stock", WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, 0, xc, stockY, spinWidth, checkBoxHeight, radioStockId);
//	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_SORT | WS_VSCROLL, 0, xc, y += staticHeight, width, h, listBoxTopicId);

	AddComboBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_SORT | WS_VSCROLL | WS_GROUP, 0, xc, y += checkBoxHeight, width, h, comboBoxTopicId);

	const int buttonHeightPlusGap = comboBoxHeight + vgap;
	const int hX = xc + width - buttonWidth;
	AddButtonIndirect("Subscribe", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, hX, y - 2 * buttonHeightPlusGap, buttonWidth, comboBoxHeight, buttonSubscribeId);
	AddButtonIndirect("Reset", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, hX, y - buttonHeightPlusGap, buttonWidth, comboBoxHeight, buttonResetId);

	stockY += checkBoxHeight;
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0, xc, stockY, halfWidth, m_height - stockY, listBoxStockId);

	const int xcHalf = xc + halfWidth + hgap;
	buttonWidth = width - xcHalf - hgap;
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xcHalf, stockY, buttonWidth, editHeight, editSymbolId);
	AddButtonIndirect("Find", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xcHalf, stockY += editHeight + vgap, buttonWidth, comboBoxHeight, buttonFindStockId);
	AddButtonIndirect("Add", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xcHalf, stockY += comboBoxHeight + vgap, buttonWidth, comboBoxHeight, buttonAddStockId);
	AddButtonIndirect("Remove", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xcHalf, stockY += comboBoxHeight + vgap, buttonWidth, comboBoxHeight, buttonRemoveStockId);
	AddButtonIndirect("Clear", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 0, xcHalf, stockY += comboBoxHeight + vgap, buttonWidth, comboBoxHeight, buttonClearStocksId);

	AddButtonIndirect("Show Story Chain", WS_VISIBLE | BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP, 0, xcHalf, stockY += comboBoxHeight + staticHeight, buttonWidth, checkBoxHeight, checkBoxShowStoryChainId);
	AddButtonIndirect("Request Story", WS_DISABLED | WS_VISIBLE|BS_PUSHBUTTON|WS_GROUP|WS_TABSTOP, 0, xcHalf, stockY += checkBoxHeight + vgap, buttonWidth, comboBoxHeight, buttonRequestStoryId);

	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY, 0, xc, stockY += comboBoxHeight + vgap, width, editHeight, editStoryResourceId);
	
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | WS_BORDER | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY | ES_MULTILINE, 0, xc, stockY += editHeight, width, m_height - stockY, editStoryId);

	y = vgap + staticHeight;
	xc += width + hgap;
	width = m_width - xc;

	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, xc, y, width, m_height - staticHeight - y, listBoxNewsId);
	AddListBox(&m_ListBoxNews);

	AddListBox(&m_ListBoxWire);
	AddListBox(&m_ListBoxStory);
	AddListBox(&m_ListBoxMeta);
	AddListBox(&m_ListBoxStock);

	SetNewsConnected(m_newsDll->isConnected());
}

void SampleDialogSettingNews::UpdateSelectedTopic()
{
	const std::string& topic = extensionConfig->m_topic.GetValue();
	if(topic.empty())m_topicOrdinal = 0xFFFF;
	else
	{
		int found = m_ComboBoxTopic.FindItem(topic.c_str());
		m_topicOrdinal = found >= 0 ? (unsigned short)found : 0xFFFF;
		m_ComboBoxTopic.SetCurSel(m_topicOrdinal == 0xFFFF ? -1 : m_topicOrdinal);
	}
	OnSelchangeTopic();
}

void SampleDialogSettingNews::UpdateAllEntitledWireSet()
{
	if(m_allEntitled)// && m_newsDll)
	{
		WireSet wireSet;
//		m_wireSet.clear();
		for(WireMap::const_iterator it = m_allWireNames.begin(); it != m_allWireNamesEnd; ++it)
		{
			const NewsWire& wire = it->second;
			if(wire.isEntitled())
			{
//				m_wireSet.insert(wire.GetSuperId());
				wireSet.insert(wire.GetSuperId());
			}
		}
//		if(m_ListBoxWire.SetWireSet(m_wireSet))
		if(m_ListBoxWire.SetWireSet(wireSet))
		{
			EnableSubscribeButton();
		}
	}
}

void SampleDialogSettingNews::UpdateWireList()
{
	std::string description;
	TakionIterator* iterator = m_newsDll->CreateAllWireIterator();
	if(iterator)
	{
		m_allWireNames.clear();
		m_newsWireMap.clear();
		m_newsWireEntitledMap.clear();
		m_newsTopicMap.clear();

		unsigned int id;
		unsigned short subid;
		unsigned short baseId;
		bool subwire;
		bool entitled;
		unsigned __int64 superId;

		m_newsDll->LockInquiryWaitAllWireNameStorage();
		iterator->Reset();

		for(unsigned short i = 0; m_newsDll->GetNextAllWireInfo(iterator, description, id, subid, baseId, subwire, entitled); ++i)
		{
			superId = NewsWire::CalculateSuperIdStatic(id, subid);
			m_allWireNames.insert(WireMap::value_type(superId,
				NewsWire(description.c_str(),
				id,
				subid,
				baseId,
				subwire,
				entitled)));
			m_newsWireMap.insert(UInt64UShortMap::value_type(superId, i));
			if(entitled)
			{
				m_newsWireEntitledMap.insert(UInt64UShortMap::value_type(superId, i));
			}
		}
		m_newsDll->UnlockInquiryAllWireNameStorage();
		delete iterator;
	}

	iterator = m_newsDll->CreateTopicIterator();
	if(iterator)
	{
		m_newsDll->LockInquiryWaitTopicNameStorage();
		iterator->Reset();
		const std::string* name = m_newsDll->GetNextTopicInfo(iterator, &description);
		for(; name; name = m_newsDll->GetNextTopicInfo(iterator, &description))
		{
			m_newsTopicMap.insert(StringMap::value_type(*name, description));
		}
		m_newsDll->UnlockInquiryTopicNameStorage();
		delete iterator;
	}

	if(m_ListBoxWire.m_hWnd)
	{
		m_ListBoxWire.SetRedraw(FALSE);

		m_ListBoxWire.ResetContent();
		for(WireMap::const_iterator it = m_allWireNames.begin(); it != m_allWireNamesEnd; ++it)
		{
			m_ListBoxWire.AddString((const char*)new NewsWire(it->second));
		}
		m_ListBoxWire.InitCheckedCount();

		m_ListBoxWire.SetRedraw(TRUE);
		m_ListBoxWire.Invalidate(FALSE);
	}
	UpdateAllEntitledWireSet();

	if(m_ComboBoxTopic.m_hWnd)
	{
		m_ComboBoxTopic.SetRedraw(FALSE);

		m_ComboBoxTopic.ResetContent();
		for(StringMap::const_iterator it = m_newsTopicMap.begin(); it != m_newsTopicMapEnd; ++it)
		{
			m_ComboBoxTopic.AddString(it->first.c_str());
		}

		UpdateSelectedTopic();

		m_ComboBoxTopic.SetRedraw(TRUE);
		m_ComboBoxTopic.Invalidate(FALSE);
	}
	EnableSubscribeButton();
//	OnSubscribe();
}

HWND SampleDialogSettingNews::GetFirstTabControl() const
{
	return m_ListBoxNews.m_hWnd;
}

void SampleDialogSettingNews::RestoreLayout()
{
//	m_currentDaysBack =
	m_daysBack = extensionConfig->m_newsDaysBack.GetValue();
//	m_currentHeadlineCount =
	m_headlineCount = extensionConfig->m_newsHeadlineCount.GetValue();

	m_currentAfterMarket = m_afterMarket = extensionConfig->m_newsAfterMarket.GetValue();
//	m_currentSearchType =
	m_searchType = extensionConfig->m_searchType.GetValue();
//	m_currentTopicOrdinal =
	UpdateSelectedTopic();
/*
	const std::string& topic = extensionConfig->m_topic.GetValue();
	if(topic.empty())m_topicOrdinal = 0xFFFF;
	else
	{
		int found = m_ComboBoxTopic.FindItem(topic.c_str());
		m_topicOrdinal = found >= 0 ? (unsigned short)found : 0xFFFF;
	}
*/
	const bool& allEntitled = extensionConfig->m_allEntitled.GetValue();
//	if(allEntitled != m_allEntitled)
//	{
		m_allEntitled = allEntitled;//extensionConfig->m_allEntitled.GetValue();
		m_CheckBoxAllEntitled.SetCheck(m_allEntitled ? BST_CHECKED : BST_UNCHECKED);
		if(m_allEntitled)
		{
			UpdateAllEntitledWireSet();
			m_wireSet = m_ListBoxWire.GetWireSet();
		}
		else
		{
			m_wireSet.clear();
			for(ExtensionDerivedConfig::WireSettingSet::ConstIterator it = extensionConfig->m_wireSettingSet.GetConstBegin(), itend = extensionConfig->m_wireSettingSet.GetConstEnd(); it != itend; ++it)
			{
				const ExtensionDerivedConfig::WireSetting& wireSetting = *it;
				m_wireSet.insert(NewsWire::CalculateSuperIdStatic(wireSetting.m_first.GetValue(), wireSetting.m_second.GetValue()));
			}
		}
//	}
	m_storyFilter = extensionConfig->m_storyFilter.GetValue();
	m_metaFilter = extensionConfig->m_metaFilter.GetValue();
	m_currentRelevance = m_relevance = extensionConfig->m_relevance.GetValue();

	m_showStoryChain = extensionConfig->m_showStoryChain.GetValue();
	m_CheckBoxShowStoryChain.SetCheck(m_showStoryChain ? BST_CHECKED : BST_UNCHECKED);

	ResetControls();
//	DoReset();
	EnableSubscribeButton();
	OnSubscribe();
	m_modified = false;
}

void SampleDialogSettingNews::UpdateSettings()
{
	extensionConfig->m_newsDaysBack.SetValue(m_daysBack);
	extensionConfig->m_newsHeadlineCount.SetValue(m_headlineCount);
	extensionConfig->m_newsAfterMarket.SetValue(m_afterMarket);
	extensionConfig->m_searchType.SetValue(m_searchType);
	extensionConfig->m_allEntitled.SetValue(m_allEntitled);

	extensionConfig->m_storyFilter.SetValue(m_storyFilter);
	extensionConfig->m_metaFilter.SetValue(m_metaFilter);
	extensionConfig->m_relevance.SetValue(m_relevance);
	extensionConfig->m_showStoryChain.SetValue(m_showStoryChain);
	std::string topic;
	if(m_topicOrdinal != 0xFFFF)
	{
		const void* topicItem = m_ComboBoxTopic.GetItemDataPtr(m_topicOrdinal);
		if(topicItem)topic = (const char*)topicItem;
	}
	extensionConfig->m_topic.SetValue(topic);

	extensionConfig->m_wireSettingSet.Clear();
	if(!m_allEntitled)
	{
		unsigned __int64 wireId;
	//	unsigned int id;
	//	unsigned short subId;
		ExtensionDerivedConfig::WireSetting wireSetting;
		for(WireSet::const_iterator it = m_wireSet.begin(); it != m_wireSetEnd; ++it)
		{
			wireId = *it;
	//		id = (unsigned int)(wireId >> 32);
	//		subId = (unsigned short)(wireId & 0xFFFF);
			wireSetting.m_first.SetValue((unsigned int)(wireId >> 32));
			wireSetting.m_second.SetValue((unsigned short)(wireId & 0xFFFF));
			extensionConfig->m_wireSettingSet.AddValue(wireSetting);
		}
	}
//	BaseDialogSetting::UpdateSettings();
}

void SampleDialogSettingNews::UpdateControls()
{
//	TD_GetNewAccountObservable().AddObserver(this);
//	TD_GetAdminObservable().AddObserver(this);
}

void SampleDialogSettingNews::BeforeDestroy()
{
	TD_GetAdminObservable().RemoveObserver(this);
	Observable* storyObservable = m_newsDll->GetStoryObservable();
	if(storyObservable)
	{
		storyObservable->RemoveObserver(this);
	}

	ReleaseNewsCollection();

	ResetOffsets();

	BaseDialogSetting::BeforeDestroy();
}

void SampleDialogSettingNews::Leaving()
{
}

void SampleDialogSettingNews::ResetOffsets()
{
	m_listBoxBorderSize2 = 0;
	m_listBoxBorderSize = 0;
	m_leftOffset = 0;
	m_titleOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;
	m_stockWidth = 0;
	m_stockLeftOffset = 0;
	m_stockTopOffset = 0;
	m_maxStockHeight = 0;
	m_stockHeight = 0;
	m_storyWidth = 0;
	m_storyTopOffset = 0;
	m_stockCountRect.SetRectEmpty();

	m_currentSearchType = m_searchType = SEA_ALL;
	m_allEntitled = true;
	m_showStoryChain = false;
	m_selectedHeadlineStoryDisplayed = false;
	m_storyError = false;
//	m_currentDaysBack =
	m_daysBack = 0;
//	m_currentHeadlineCount =
	m_headlineCount = 100;
	m_currentAfterMarket = m_afterMarket = false;
	m_addedVisibleHeadline = false;
	m_currentRelevance = m_relevance = 0;
	m_currentTopicOrdinal = m_topicOrdinal = 0xFFFF;
}

void SampleDialogSettingNews::ToolTipsEnabled(bool enable)
{
	BaseDialogSetting::ToolTipsEnabled(enable);
	m_ListBoxNews.EnableTooltips(enable, m_toolTip);
}

BOOL SampleDialogSettingNews::OnInitDialog()
{
	TD_GetAdminObservable().AddObserver(this);
	Observable* storyObservable = m_newsDll->GetStoryObservable();
	if(storyObservable)
	{
		storyObservable->AddObserver(this);
	}

	ResetOffsets();

	BOOL ret = BaseDialogSetting::OnInitDialog();

	m_EditSymbol.SetLimitText(7);

	SetToolTipsEnabled(true);
//	m_ListBoxNews.EnableTooltips(true, m_toolTip);
	AddTitle(&m_ListBoxNews);
//	UpdateTotalRects();//winRect.left, winRect.bottom);
	UpdateWireList();

	m_RadioAll.SetCheck(BST_CHECKED);
	OnSearchType();

	m_ComboBoxTopic.SetInvalid(m_currentSearchType != SEA_TOPIC);
	m_ListBoxStock.SetInvalid(m_currentSearchType != SEA_STOCK);

//	RestoreLayout();

	return ret;
}

void SampleDialogSettingNews::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSetting::DoDataExchange(pDX);
	DDX_Control(pDX, listBoxNewsId, m_ListBoxNews);
//	DDX_Control(pDX, staticWireFilterId, m_StaticWire);
	DDX_Control(pDX, listBoxWireFilterId, m_ListBoxWire);
	DDX_Control(pDX, listBoxStoryFilterId, m_ListBoxStory);
	DDX_Control(pDX, listBoxMetaFilterId, m_ListBoxMeta);
	DDX_Control(pDX, spinDayBackId, m_SpinDaysBack);
	DDX_Control(pDX, checkBoxAfterMarketId, m_CheckBoxAfterMarket);
	DDX_Control(pDX, spinCountLimitId, m_SpinCountLimit);

	DDX_Control(pDX, comboBoxTopicId, m_ComboBoxTopic);

	DDX_Control(pDX, listBoxStockId, m_ListBoxStock);
	DDX_Control(pDX, editSymbolId, m_EditSymbol);
	DDX_Control(pDX, buttonFindStockId, m_ButtonStockFind);
	DDX_Control(pDX, buttonAddStockId, m_ButtonStockAdd);
	DDX_Control(pDX, buttonRemoveStockId, m_ButtonStockRemove);
	DDX_Control(pDX, buttonClearStocksId, m_ButtonStockClear);
	DDX_Control(pDX, checkBoxShowStoryChainId, m_CheckBoxShowStoryChain);
	DDX_Control(pDX, buttonRequestStoryId, m_ButtonRequestStory);
	DDX_Control(pDX, editStoryResourceId, m_EditStoryResource);
	DDX_Control(pDX, editStoryId, m_EditStory);

	DDX_Control(pDX, radioUnfilteredId, m_RadioUnfiltered);
	DDX_Control(pDX, radioAllId, m_RadioAll);
	DDX_Control(pDX, radioAllUsId, m_RadioAllUs);
	DDX_Control(pDX, radioTopicId, m_RadioTopic);
	DDX_Control(pDX, radioStockId, m_RadioStock);
	DDX_Control(pDX, checkBoxAllEntitledId, m_CheckBoxAllEntitled);
	DDX_Control(pDX, buttonSubscribeId, m_ButtonSubscribe);
	DDX_Control(pDX, buttonResetId, m_ButtonReset);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingNews, BaseDialogSetting)
	ON_WM_CTLCOLOR()
	ON_LBN_SELCHANGE(listBoxNewsId, OnSelchangeNews)
	ON_CBN_SELCHANGE(comboBoxTopicId, OnSelchangeTopic)
	ON_BN_CLICKED(radioUnfilteredId, OnSearchType)
	ON_BN_CLICKED(radioAllId, OnSearchType)
	ON_BN_CLICKED(radioAllUsId, OnSearchType)
	ON_BN_CLICKED(radioTopicId, OnSearchType)
	ON_BN_CLICKED(radioStockId, OnSearchType)
	ON_BN_CLICKED(checkBoxAllEntitledId, OnAllEntitled)
	ON_BN_CLICKED(buttonSubscribeId, OnSubscribe)
	ON_BN_CLICKED(buttonResetId, OnReset)

	ON_BN_CLICKED(checkBoxAfterMarketId, OnAfterMarket)

	ON_LBN_SELCHANGE(listBoxStockId, OnSelchangeStock)
	ON_LBN_DBLCLK(listBoxStockId, OnDblClkStock)
	ON_EN_CHANGE(editSymbolId, OnSymbolChange)
	ON_EN_UPDATE(editSymbolId, OnSymbolUpdate)
	ON_BN_CLICKED(buttonFindStockId, OnStockFind)
	ON_BN_CLICKED(buttonAddStockId, OnStockAdd)
	ON_BN_CLICKED(buttonRemoveStockId, OnStockRemove)
	ON_BN_CLICKED(buttonClearStocksId, OnStockClear)
	ON_BN_CLICKED(buttonRequestStoryId, OnRequestStory)
	ON_BN_CLICKED(checkBoxShowStoryChainId, OnShowStoryChain)

	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)//one item is checked/unchecked
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//multiple items checked/unchecked
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingNews::OnDblClkStock()
{
	int sel = m_ListBoxStock.GetCurSel();
	if(sel >= 0)
	{
		const char* symbol = m_ListBoxStock.GetItemNameAt(sel);
		m_EditSymbol.SetWindowText(symbol);
	}
}

void SampleDialogSettingNews::OnSelchangeStock()
{
	EnableControl(m_ButtonStockRemove.m_hWnd, m_ListBoxStock.GetCurSel() >= 0, m_ListBoxNews.m_hWnd);
}

void SampleDialogSettingNews::OnStockFind()
{
	if(m_EditSymbol.GetWindowTextLength())
	{
		CString symbol;
		m_EditSymbol.GetWindowText(symbol);
		bool removeEnabled = m_ListBoxStock.FindStock(symbol) >= 0;
		EnableControl(m_ButtonStockRemove.m_hWnd, removeEnabled, m_ListBoxNews.m_hWnd);
	}
}

void SampleDialogSettingNews::OnStockAdd()
{
	AddCurrentStock();
}

bool SampleDialogSettingNews::AddCurrentStock()
{
	if(m_EditSymbol.GetWindowTextLength())
	{
		CString symbol;
		m_EditSymbol.GetWindowText(symbol);
		return m_ListBoxStock.AddStock(symbol);
	}
	return false;
}

void SampleDialogSettingNews::OnStockRemove()
{
	m_ListBoxStock.RemoveCurrentStock();
}

void SampleDialogSettingNews::OnStockClear()
{
	m_ListBoxStock.ClearStocks();
}

void SampleDialogSettingNews::OnSymbolUpdate()
{
	m_EditSymbol.TryUpdate();
}

void SampleDialogSettingNews::OnSymbolChange()
{
	bool empty = m_symbol.empty();
	CString text;
	m_EditSymbol.GetWindowText(text);
	m_symbol = text;
	if(m_symbol.empty() != empty)
	{
//		m_EditSymbol.Invalidate(FALSE);
		EnableControl(m_ButtonStockFind.m_hWnd, empty, m_ListBoxNews.m_hWnd);
		EnableControl(m_ButtonStockAdd.m_hWnd, empty, m_ListBoxNews.m_hWnd);
	}
}

void SampleDialogSettingNews::OnAllEntitled()
{
	const bool allEntitled = m_CheckBoxAllEntitled.GetCheck() == BST_CHECKED;
	if(allEntitled != m_allEntitled)
	{
		m_allEntitled = allEntitled;
		m_modified = true;
/*
		if(m_allEntitled)
		{
			UpdateAllEntitledWireSet();
		}
*/
	}
}

void SampleDialogSettingNews::OnShowStoryChain()
{
	const bool showStoryChain = m_CheckBoxShowStoryChain.GetCheck() == BST_CHECKED;
	if(showStoryChain != m_showStoryChain)
	{
		m_showStoryChain = showStoryChain;
		m_modified = true;
	}
}

void SampleDialogSettingNews::OnSelchangeTopic()
{
	const unsigned short topicOrdinal = (unsigned short)m_ComboBoxTopic.GetCurSel();
	if(topicOrdinal != m_currentTopicOrdinal)
	{
		m_currentTopicOrdinal = topicOrdinal;
		EnableSubscribeButton();
	}
}

void SampleDialogSettingNews::OnRequestStory()
{
	int sel = m_ListBoxNews.GetCurSel();
	if(sel >= 0)
	{
//		const SampleHeadline* headline = (const SampleHeadline*)m_ListBoxNews.GetItemDataPtr(sel);
		RequestStory(((const SampleHeadline*)m_ListBoxNews.GetItemDataPtr(sel))->GetHeadline());
	}
}

void SampleDialogSettingNews::RequestStory(const NewsHeadline* headline)
{
	bool request = true;
	headline->LockInquiryWait();
//added 12/24/2014
	headline->LockStoryInquiryWait();
//
	const std::string& story = headline->GetStoryText();
	if(!story.empty() && (headline->isShowChain() || !m_showStoryChain))
	{
		request = false;
	}
	if(request)
	{
		DoRequestStory(headline);
	}
	else
	{
		DisplayStoryText(headline->GetResourceId().GetString(), story, headline->isStoryError());
//		m_mainWnd->ShowNewsStory(story, headline->GetResourceIdStr());
	}
	headline->UnlockStoryInquiry();
	headline->UnlockInquiry();
}

bool SampleDialogSettingNews::DoRequestStory(const NewsHeadline* headline)//must lock outside: headline->LockStoryInquiryWait();
{
	bool ret = false;
	const std::string& story = headline->GetStoryText();
	if(!story.empty())
	{
		bool showChain = headline->isShowChain();
		if(showChain || !m_showStoryChain)
		{
			ret = true;
		}
	}
	if(m_newsDll->RequestStory(headline, m_showStoryChain, false))
	{
		ret = true;
	}
	else
	{
		m_storyRequestId = headline->GetResourceIdStr();
	}
	if(ret)
	{
//		m_mainWnd->ShowNewsStory(story, headline->GetResourceIdStr());
		DisplayStoryText(headline->GetResourceId().GetString(), story, headline->isStoryError());
	}
	return ret;
}

bool SampleDialogSettingNews::UpdateSelectedLineStoryDisplayed()
{
	int sel = m_ListBoxNews.GetCurSel();
	bool ret = sel >= 0;
	bool selectedHeadlineStoryDisplayed = false;
	if(ret)
	{
		const SampleHeadline* sampleHeadline = (const SampleHeadline*)m_ListBoxNews.GetItemDataPtr(sel);
		selectedHeadlineStoryDisplayed = !strcmp(sampleHeadline->GetResourceId().GetString(), m_displayedStoryResourceId.c_str());
	}
	if(selectedHeadlineStoryDisplayed != m_selectedHeadlineStoryDisplayed)
	{
		m_selectedHeadlineStoryDisplayed = selectedHeadlineStoryDisplayed;
		m_EditStory.Invalidate(TRUE);
		m_EditStoryResource.Invalidate(TRUE);
	}
	return ret;
}

void SampleDialogSettingNews::OnSelchangeNews()
{
	bool selected = UpdateSelectedLineStoryDisplayed();
	EnableControl(m_ButtonRequestStory.m_hWnd, selected, m_ListBoxNews.m_hWnd);
}

unsigned char SampleDialogSettingNews::ObtainSearchType() const
{
	unsigned char ret = 0xFF;
	if(m_RadioAll.m_hWnd)
	{
		if(m_RadioUnfiltered.GetCheck() == BST_CHECKED)ret = SEA_UNFILTERED;
		else if(m_RadioAll.GetCheck() == BST_CHECKED)ret = SEA_ALL;
		else if(m_RadioAllUs.GetCheck() == BST_CHECKED)ret = SEA_ALL_US;
		else if(m_RadioTopic.GetCheck() == BST_CHECKED)ret = SEA_TOPIC;
		else if(m_RadioStock.GetCheck() == BST_CHECKED)ret = SEA_STOCK;
	}
	return ret;
}

void SampleDialogSettingNews::OnSearchType()
{
	if(SetSearchType(ObtainSearchType()))EnableSubscribeButton();
}

bool SampleDialogSettingNews::SetSearchType(const unsigned char& searchType)
{
	if(searchType != m_currentSearchType)
	{
		m_currentSearchType = searchType;
		m_ComboBoxTopic.SetInvalid(m_currentSearchType != SEA_TOPIC);
		m_ListBoxStock.SetInvalid(m_currentSearchType != SEA_STOCK);
		return true;
	}
	return false;
}

LRESULT SampleDialogSettingNews::OnListBoxChecked(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case listBoxWireFilterId:
		EnableSubscribeButton();
		break;

		default:
		if(isValid())EnableSubscribeButton();
		break;
	}
	return 0;
}

void SampleDialogSettingNews::EnableSubscribeButton()
{
	const bool changed = isChanged();
	EnableControl(m_ButtonSubscribe.m_hWnd, changed && isValid() && !m_ListBoxNews.isBusy(), m_ListBoxNews.m_hWnd);
	EnableControl(m_ButtonReset.m_hWnd, changed, m_ListBoxNews.m_hWnd);
}

void SampleDialogSettingNews::OnReset()
{
	DoReset();
}

bool SampleDialogSettingNews::ResetControls()
{
	bool ret = false;
	if(m_ListBoxWire.SetWireSet(m_wireSet))
	{
		ret = true;
//		m_currentWireFilter = m_wireFilter;
	}
//	if(m_currentStoryFilter != m_storyFilter)
	if(m_ListBoxStory.SetFilter(m_storyFilter))
	{
		ret = true;
//		m_currentStoryFilter = m_storyFilter;
	}
//	if(m_currentMetaFilter != m_metaFilter)
	if(m_ListBoxMeta.SetFilter(m_metaFilter))
	{
		ret = true;
//		m_currentMetaFilter = m_metaFilter;
	}
	if(m_topicOrdinal != (unsigned short)m_ComboBoxTopic.GetCurSel())
	{
		ret = true;
		m_ComboBoxTopic.SetCurSel(m_topicOrdinal == 0xFFFF ? -1 : m_topicOrdinal);
	}
	if(m_SpinDaysBack.SetValue(m_daysBack))
	{
		ret = true;
		m_SpinDaysBack.UpdateValueNum();
	}
	if(m_SpinCountLimit.SetValue(m_headlineCount))
	{
		ret = true;
		m_SpinCountLimit.UpdateValueNum();
	}
	if(m_currentRelevance != m_relevance)
	{
		m_currentRelevance = m_relevance;
		ret = true;
	}

//	if((m_CheckBoxAfterMarket.GetCheck() == BST_CHECKED) != m_afterMarket)
	if(m_currentAfterMarket != m_afterMarket)
	{
		m_currentAfterMarket = m_afterMarket;
		ret = true;
		m_CheckBoxAfterMarket.SetCheck(m_afterMarket ? BST_CHECKED : BST_UNCHECKED);
	}
//	const unsigned char searchType = ObtainSearchType();
	if(m_searchType != m_currentSearchType)
	{
		ret = true;
		switch(m_currentSearchType)
		{
			case SEA_UNFILTERED:
			m_RadioUnfiltered.SetCheck(BST_UNCHECKED);
			break;

			case SEA_ALL:
			m_RadioAll.SetCheck(BST_UNCHECKED);
			break;

			case SEA_ALL_US:
			m_RadioAllUs.SetCheck(BST_UNCHECKED);
			break;

			case SEA_TOPIC:
			m_RadioTopic.SetCheck(BST_UNCHECKED);
			break;

			case SEA_STOCK:
			m_RadioStock.SetCheck(BST_UNCHECKED);
			break;
		}
		switch(m_searchType)
		{
			case SEA_UNFILTERED:
			m_RadioUnfiltered.SetCheck(BST_CHECKED);
			break;

			case SEA_ALL:
			m_RadioAll.SetCheck(BST_CHECKED);
			break;

			case SEA_ALL_US:
			m_RadioAllUs.SetCheck(BST_CHECKED);
			break;

			case SEA_TOPIC:
			m_RadioTopic.SetCheck(BST_CHECKED);
			break;

			case SEA_STOCK:
			m_RadioStock.SetCheck(BST_CHECKED);
			break;
		}
		SetSearchType(m_searchType);
	}

	if(m_ListBoxStock.SetStockSet(m_stockSet))
	{
		ret = true;
		OnStocksChanged();
	}
	return ret;
}

bool SampleDialogSettingNews::DoReset()
{
	bool ret = ResetControls();
	if((m_CheckBoxAllEntitled.GetCheck() == BST_CHECKED) != m_allEntitled)
	{
		ret = true;
		m_CheckBoxAllEntitled.SetCheck(m_allEntitled ? BST_CHECKED : BST_UNCHECKED);
	}
	
	if((m_CheckBoxShowStoryChain.GetCheck() == BST_CHECKED) != m_showStoryChain)
	{
		ret = true;
		m_CheckBoxShowStoryChain.SetCheck(m_showStoryChain ? BST_CHECKED : BST_UNCHECKED);
	}

	if(m_currentRelevance != m_relevance)
	{
		ret = true;
		m_currentRelevance = m_relevance;
	}

	if(m_currentAfterMarket != m_afterMarket)
	{
		ret = true;
		m_currentAfterMarket = m_afterMarket;
	}

	if(m_currentTopicOrdinal != m_topicOrdinal)
	{
		ret = true;
		m_currentTopicOrdinal = m_topicOrdinal;
	}
	if(m_searchType != m_currentSearchType)
	{
		ret = true;
		m_currentSearchType = m_searchType;
	}

	if(ret)
	{
		EnableSubscribeButton();
	}
	return ret;
}

void SampleDialogSettingNews::OnSubscribe()
{
	if(isValid())
	{
		const bool subscriptionChanged = isSubscriptionChanged();
		m_wireSet = m_ListBoxWire.GetWireSet();
		if(subscriptionChanged)
		{
			m_entitledWireSet = m_ListBoxWire.GetCheckedEntitledWireSet();
		}
		m_storyFilter = m_ListBoxStory.GetFilter();//m_currentStoryFilter;
		m_metaFilter = m_ListBoxMeta.GetFilter();//m_currentMetaFilter;
		m_relevance = m_currentRelevance;

//		m_allEntitled = m_CheckBoxAllEntitled.GetCheck() == BST_CHECKED;
		m_searchType = m_currentSearchType;
		m_daysBack = m_SpinDaysBack.GetValue();
		m_headlineCount = m_SpinCountLimit.GetValue();

		m_afterMarket = m_currentAfterMarket;//m_CheckBoxAfterMarket.GetCheck() == BST_CHECKED;

//		int sel = m_ComboBoxTopic.GetCurSel();
//		m_topicOrdinal = sel >= 0 ? (unsigned short)sel : 0xFFFF;
		m_topicOrdinal = m_currentTopicOrdinal;//(unsigned short)m_ComboBoxTopic.GetCurSel();

		m_ListBoxStock.GetStockSet(m_stockSet);

		EnableSubscribeButton();

		if(subscriptionChanged)
		{
//			m_ListBoxNews.ClearItemsWithRedraw();
//Send Subscription
			UpdateNewsCollectionFilter();
		}
		m_modified = true;
	}
}

BOOL SampleDialogSettingNews::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		unsigned char ascii = (unsigned char)pMsg->wParam;
		HWND focus = ::GetFocus();
		if(focus == m_EditSymbol.m_hWnd)
		{
			switch(ascii)
			{
				case VK_RETURN:
				case VK_INSERT:
				if(AddCurrentStock())
				{
					m_EditSymbol.SetWindowText("");
				}
				return TRUE;
			}
		}
	}
	return BaseDialogSetting::PreTranslateMessage(pMsg);
}

bool SampleDialogSettingNews::isValid() const
{
	if(m_ListBoxWire.GetEntitledCheckedCount() != 0)
	{
		switch(m_currentSearchType)
		{
			case SEA_UNFILTERED:
			case SEA_ALL:
			case SEA_ALL_US:
			return true;

			case SEA_TOPIC:
			return m_ComboBoxTopic.GetCurSel() >= 0;

			case SEA_STOCK:
//			return m_ListBoxStock.GetCount() > 0;
			return true;

			default:
			return false;
		}
	}
	return false;
}

void SampleDialogSettingNews::OnStocksChanged()
{
	EnableSubscribeButton();
	InvalidateStockCountRect();
	EnableControl(m_ButtonStockClear.m_hWnd, m_ListBoxStock.GetItemCount() > 0, m_ListBoxNews.m_hWnd);
	EnableControl(m_ButtonStockRemove.m_hWnd, m_ListBoxStock.GetCurSel() >= 0, m_ListBoxNews.m_hWnd);
}
/*
bool SampleDialogSettingNews::isStockSetChanged() const
{
}
*/
bool SampleDialogSettingNews::isBaseChanged() const
{
	return m_ListBoxStory.GetFilter() != m_storyFilter
		|| m_ListBoxMeta.GetFilter() != m_metaFilter
		|| m_currentRelevance != m_relevance
		|| m_searchType != m_currentSearchType
		|| m_SpinDaysBack.GetValue() != m_daysBack
		|| m_SpinCountLimit.GetValue() != m_headlineCount
		|| m_currentAfterMarket != m_afterMarket;//m_CheckBoxAfterMarket.GetCheck() 
}

bool SampleDialogSettingNews::isChanged() const
{
	return isBaseChanged()
		|| U_CompareSets(m_ListBoxWire.GetWireSet(), m_wireSet) != 0
		|| m_currentTopicOrdinal != m_topicOrdinal
		|| !m_ListBoxStock.isSameStockSet(m_stockSet);
}

bool SampleDialogSettingNews::isSubscriptionChanged() const
{
	if(isBaseChanged() || U_CompareSets(m_ListBoxWire.GetCheckedEntitledWireSet(), m_entitledWireSet) != 0)
	{
		return true;
	}
	else
	{
		switch(m_currentSearchType)
		{
			case SEA_UNFILTERED:
			case SEA_ALL:
			case SEA_ALL_US:
			return false;

			case SEA_TOPIC:
			return m_currentTopicOrdinal != m_topicOrdinal;

			case SEA_STOCK:
			return !m_ListBoxStock.isSameStockSet(m_stockSet);

			default:
			return false;
		}
	}
}

void SampleDialogSettingNews::OnAfterMarket()
{
	const bool afterMarket = m_CheckBoxAfterMarket.GetCheck() == BST_CHECKED;
	if(afterMarket != m_currentAfterMarket)
	{
		m_currentAfterMarket = afterMarket;
		if(isValid())EnableSubscribeButton();
	}
}

LRESULT SampleDialogSettingNews::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinDayBackId:
		m_SpinDaysBack.UpdateValueNum();
/*
		{
//			m_currentDaysBack = m_SpinDaysBack.GetValue();
//			if(m_currentDaysBack != m_daysBack)DoEnableOkButton(true);
//			else EnableOkButton();
		}
*/
		if(isValid())EnableSubscribeButton();
		break;

		case spinCountLimitId:
		m_SpinCountLimit.UpdateValueNum();
/*
		{
//			m_currentHeadlineCount = m_SpinCountLimit.GetValue();
//			if(m_currentHeadlineCount != m_headlineCount)DoEnableOkButton(true);
//			else EnableOkButton();
		}
*/
		if(isValid())EnableSubscribeButton();
		break;
	}
	return 0;
}

HBRUSH SampleDialogSettingNews::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	switch(pWnd->GetDlgCtrlID())
	{
		case editStoryId:
		if(m_storyError)
		{
			pDC->SetTextColor(m_selectedHeadlineStoryDisplayed ? RGB(96, 0, 0) : RGB(64, 64, 0));
			return *m_bkBrush;
		}
		else if(m_selectedHeadlineStoryDisplayed)
		{
//			pDC->SetBkMode(TRANSPARENT);
//			return *m_takionVisual->GetBlueBrush();
			pDC->SetTextColor(RGB(0, 0, 96));
			return *m_bkBrush;
		}
		break;

		case editStoryResourceId:
		if(m_storyError)
		{
			pDC->SetTextColor(m_selectedHeadlineStoryDisplayed ? RGB(96, 0, 0) : RGB(64, 64, 0));
			return *m_bkBrush;
		}
		else if(m_selectedHeadlineStoryDisplayed)
		{
//			pDC->SetBkMode(TRANSPARENT);
//			return *m_takionVisual->GetBlueBrush();
			pDC->SetTextColor(RGB(0, 0, 96));
			return *m_bkBrush;
		}
		break;
/*
		case editSymbolId:
		if(m_symbol.empty())
		{
			pDC->SetBkMode(TRANSPARENT);
			return *m_takionVisual->GetWarningBrush();
		}
		break;
*/
	}

	return BaseDialogSetting::OnCtlColor(pDC, pWnd, nCtlColor);
}

void SampleDialogSettingNews::ProfilesLoaded()
{
	SetNewsConnected(true);

//	UpdateAll();
/*
	UpdateAllEntitledWireSet();
	UpdateWireSet();
	UpdateNewsCollectionFilter();
	UpdateNewsCollectionLimits(m_headlineCount);
*/
}

void SampleDialogSettingNews::ClearHeadlines()
{
	if(m_ListBoxNews.ClearItemsWithRedraw())
	{
		InvalidateTotalValueRect(listBoxNewsId, NHCI_DATETIME);
		OnSelchangeNews();
	}
}

bool SampleDialogSettingNews::SetStockNews(Observable* stockNews, bool cleanup)
{
	if(m_stockNews != stockNews)
	{
		if(cleanup)
		{
			ClearHeadlines();
		}
		if(m_stockNews)
		{
			m_stockNews->RemoveObserver(this);
		}
		m_stockNews = stockNews;
		UpdateStockNews();
		return true;
	}
	return false;
}

void SampleDialogSettingNews::UpdateHeadlines()
{
	m_addedVisibleHeadline = false;
	m_ListBoxNews.SetRedraw(FALSE);

	m_unappliedHeadlineReader.ProcessAllData();

	m_ListBoxNews.SetRedraw(TRUE);
	if(m_addedVisibleHeadline)
	{
		m_ListBoxNews.Invalidate(FALSE);
		m_addedVisibleHeadline = false;
	}
/*
	unsigned int count = m_unappliedHeadlineReader.ProcessAllData();
	if(count)
	{
	}
*/
}

bool SampleDialogSettingNews::CreateBasketFilterString(std::string& filter, HashStockSet* hashStockSet) const
{
//	const std::hash_set<unsigned __int64>* stockSet
	filter.clear();
	const unsigned int stockCount = (unsigned int)m_stockSet.size();
	if(stockCount > 1)
	{
		for(ListBoxNewsStock::StockSet::const_iterator it = m_stockSet.begin(); it != m_stockSetEnd; ++it)
		{
			if(!filter.empty())
			{
				filter += ' ';
			}
			filter += '/';
			filter += (const char*)&*it;
			if(hashStockSet)hashStockSet->insert(*it);
		}
		return true;
	}
	return false;
}

bool SampleDialogSettingNews::HeadlineBelongs(const NewsHeadline* headline) const
{
	if(m_entitledWireSet.find(((unsigned __int64)headline->GetExtDisplayWireId() << 32) | (unsigned __int64)headline->GetSubWireId()) == m_entitledWireSetEnd)
	{
		return false;
	}
/*Add some text filtering here if you want.
	if(m_textFilter && !m_textFilter->isTrue(headline->GetText().c_str()))
	{
		return false;
	}
*/
	switch(m_searchType)
	{
		case SEA_UNFILTERED:
		return true;

		case SEA_ALL:
		return true;

		case SEA_ALL_US:
/*
		if(m_newsDll)
		{
			const std::string* us = m_newsDll->GetSearchStringAllUs();
			const std::string& codes = headline->GetStoryCodes();
			const std::string& valuedCodes = headline->GetValuedStoryCodes();
			return !us || codes.find(*us, 0) != std::string::npos || valuedCodes.find(*us, 0) != std::string::npos;
		}
*/
		return true;
/*
		{

			std::string toFind("US");
			std::string::size_type len = toFind.length();
			std::string::size_type found = 0;
			const std::string& codes = headline->GetStoryCodes();
			std::string::size_type codelen = codes.length();
			for(found = codes.find(toFind, found); found != std::string::npos; found = codes.find(toFind, found + len))
			{
				if((found == 0 || !isalnum(codes[found - 1])) && (found + len >= codelen || !isalnum(codes[found + len])))
				{
					return true;
				}
			}
		}
		return false;
*/

		default:
		if(m_metaFilter & (1 << NMF_SYMBOLS))
		{
			const NewsHeadline::SymbolSet& symbolSet = headline->GetSymbolSet();
			NewsHeadline::SymbolSet::const_iterator itend = symbolSet.end();
//			NewsHeadline::SymbolSet::const_iterator found;
//			for(NewsHeadline::SymbolSet::const_iterator it = symbolSet.begin(), itend = symbolSet.end(); it != itend; ++it)
			for(ListBoxNewsStock::StockSet::const_iterator it = m_stockSet.begin(); it != m_stockSetEnd; ++it)
			{
				if(symbolSet.find(*it) != itend)
				{
					return true;
				}
			}
			return false;
		}
		return true;
	}
}

void SampleDialogSettingNews::StopSearch()
{
	m_ListBoxNews.SetBusy(false);
//	EnableControl(m_ButtonSubscribe.m_hWnd, isChanged() && isValid() && !m_ListBoxNews.isBusy(), m_ListBoxNews.m_hWnd);
	EnableControl(m_ButtonSubscribe.m_hWnd, isChanged() && isValid(), m_ListBoxNews.m_hWnd);
}

void SampleDialogSettingNews::StartSearch()
{
	ClearHeadlines();
	m_ListBoxNews.SetBusy(true);
//	EnableControl(m_ButtonSubscribe.m_hWnd, isChanged() && isValid() && !m_ListBoxNews.isBusy(), m_ListBoxNews.m_hWnd);
	EnableControl(m_ButtonSubscribe.m_hWnd, false, m_ListBoxNews.m_hWnd);
}

void SampleDialogSettingNews::UpdateNewsCollectionFilter()
{

	if(m_entitledWireSet.empty() || !m_headlineCount)
	{
		ReleaseNewsCollection();
	}
	else
	{
		if(m_newsDll)
		{

			bool oldCollectionDeleted = false;
			Observable* newsCollection = m_newsDll->UpdateNewsCollectionFilter(m_newsCollection, this, m_entitledWireSet, m_storyFilter, m_metaFilter, m_relevance, oldCollectionDeleted);
			bool newsCollectionChanged = oldCollectionDeleted;
			if(oldCollectionDeleted)
			{
				m_stockNews = nullptr;
				m_newsCollection = newsCollection;
//				newsCollectionChanged = true;
			}
			else
			{
				newsCollectionChanged = m_newsCollection != newsCollection;
				if(newsCollectionChanged)m_newsCollection = newsCollection;
			}

			if(m_newsCollection)
			{
				if(m_searchType == SEA_STOCK)
				{
					const unsigned int stockCount = (unsigned int)m_stockSet.size();
					if(stockCount)
					{
						HashStockSet hashStockSet;
						if(CreateBasketFilterString(m_basketFilter, &hashStockSet))
						{
//							SetStockNews(m_newsDll->GetStringNews(m_stockNews, m_newsCollection, FilterStringAsNumberArray(m_basketFilter), &hashStockSet), true);//cleanup);
							if(!SetStockNews(m_newsDll->GetStringNews(m_newsCollection, FilterStringAsNumberArray(m_basketFilter), &hashStockSet), true))
							{
								if(m_stockNews && m_newsDll->RequestHeadlines(m_stockNews, m_daysBack, m_headlineCount, m_afterMarket))
								{
									StartSearch();
								}
								else
								{
									PopulateStockNews();
								}
							}
						}
						else
						{
							const char* symbol = (const char*)&*m_stockSet.begin();
							if(*symbol)
							{
								if(!SetStockNews(m_newsDll->GetStockNews(m_newsCollection, symbol), true))
								{
									if(m_stockNews && m_newsDll->RequestHeadlines(m_stockNews, m_daysBack, m_headlineCount, m_afterMarket))
									{
										StartSearch();
									}
									else
									{
										PopulateStockNews();
									}
								}
							}
							else
							{
								SetStockNews(nullptr, true);
							}
						}
					}
					else
					{
						SetStockNews(nullptr, true);
					}
				}
				else
				{
//					if(newsCollectionChanged)//prevNewsCollection != m_newsCollection)
//					{
//						ClearHeadlines();
//					}

//					ClearHeadlines();
						std::string topicStr;
						const std::string* str = UpdateSearchFilter(topicStr);
//						SetStockNews(str ? m_newsDll->GetStringNews(m_stockNews, m_newsCollection, FilterStringAsNumberArray(*str), nullptr) : nullptr, true);
//
						if(!SetStockNews(str ? m_newsDll->GetStringNews(m_newsCollection, FilterStringAsNumberArray(*str), nullptr) : nullptr, true))
						{
/*
							if(!m_newsDll->RequestHeadlines(m_stockNews, m_daysBack, m_headlineCount, m_afterMarket))
							{
								UpdateStockNews();
							}
*/
							if(m_stockNews && m_newsDll->RequestHeadlines(m_stockNews, m_daysBack, m_headlineCount, m_afterMarket))
							{
								StartSearch();
							}
							else
							{
								PopulateStockNews();
							}
						}
/**/
/*
						SetStockNews(str ? m_newsDll->GetStringNews(m_newsCollection, FilterStringAsNumberArray(*str), nullptr) : nullptr, true);
						if(m_stockNews)// && !m_newsDll->isStockNewsRequested(m_stockNews))
						{
							if(m_newsDll->RequestHeadlines(m_stockNews, m_daysBack, m_headlineCount, m_afterMarket))
							{
								ClearHeadlines();
							}
							else
							{
								PopulateStockNews();
							}
						}
/**/

/*
					if(m_stockNews && !m_newsDll->isStockNewsRequested(m_stockNews))
					{
						if(!newsCollectionChanged)//prevNewsCollection == m_newsCollection)
						{
							ClearHeadlines();
						}
						if(m_newsDll->RequestHeadlines(m_stockNews, m_daysBack, m_headlineCount))
						{
//							SetLoadingHistory(true);
							ClearHeadlines();
						}
					}
*/
/*
					if(m_stockNews)// && !m_newsDll->isStockNewsRequested(m_stockNews))
					{
//						if(!newsCollectionChanged)//prevNewsCollection == m_newsCollection)
//						{
						if(m_newsDll->RequestHeadlines(m_stockNews, m_daysBack, m_headlineCount))
						{
							ClearHeadlines();
						}
//						}
					}
*/
				}
//				SetNoProfile(!m_newsCollection || !m_newsDll->NewsCollectionHasProfile(m_newsCollection));
				TrimHeadlineCount(false);
			}
			else
			{
				ClearHeadlines();
//				SetNoProfile(true);
			}
		}
	}
}

void SampleDialogSettingNews::ReleaseNewsCollection()
{
	if(m_newsCollection)
	{
		if(m_newsDll)
		{
			m_newsDll->ReleaseNewsCollection(m_newsCollection, this);
		}
		m_newsCollection = nullptr;
//		SetNoProfile(true);
	}
	m_unappliedHeadlineReader.SetLineBufferArray(nullptr);
	m_stockNews = nullptr;
	StopSearch();
}
/*
void SampleDialogSettingNews::DisplayLastHistoricStory()
{
	if(m_displayLastHistoricStory)
	{
		const TakionNewsHeadlineRow* row = (const TakionNewsHeadlineRow*)GetRowAt(0);
		if(row)
		{
			RequestStory(row->GetHeadline());
		}
	}
}
*/
bool SampleDialogSettingNews::AddHeadlineToListBox(const NewsHeadline* headline)//returns true if the headline is added to the visible area of the List Box
{
	bool ret = false;
	if(HeadlineBelongs(headline))
	{
		SampleHeadline* headlineItem = new SampleHeadline(headline);
		int found = m_ListBoxNews.FindItem(headlineItem);//uses ListBoxNews::Compare to find the item in the ListBox
		if(found >= 0)//item already in the list box
		{
			delete headlineItem;
		}
		else
		{
//			m_ListBoxNews.SetRedraw(FALSE);
			const int index = m_ListBoxNews.AddString((const char*)headlineItem);
			if(index >= 0)
			{
				TrimHeadlineCount(true);
				ret = m_ListBoxNews.isItemVisible(index);
			}
//			m_ListBoxNews.SetRedraw(TRUE);
//			m_ListBoxNews.Invalidate(FALSE);
		}
	}
#ifdef _DEBUG
	else
	{
printf("X");
	}
#endif
	return ret;
}

void SampleDialogSettingNews::TrimHeadlineCount(bool added)
{
	unsigned int count = (unsigned int)m_ListBoxNews.GetItemCount();
	if(count > m_headlineCount)
	{
		if(count == m_headlineCount + 1)
		{
			m_ListBoxNews.DeleteString(m_headlineCount);
			if(!added)InvalidateTotalValueRect(listBoxNewsId, NHCI_DATETIME);
		}
		else
		{
			while(count > m_headlineCount)
			{
				--count;
				m_ListBoxNews.DeleteString(count);
			}
			InvalidateTotalValueRect(listBoxNewsId, NHCI_DATETIME);
		}
		OnSelchangeNews();
	}
	else if(added)
	{
		InvalidateTotalValueRect(listBoxNewsId, NHCI_DATETIME);
	}
}

bool SampleDialogSettingNews::DoAddHeadline(const TMsgHeadline* msg)
{
	if(m_newsDll && m_stockNews && msg->m_length > sizeof(TMsgHeadline))
	{
//		const NewsHeadline* headline = m_newsDll->FindHeadline(m_stockNews, std::string((const char*)msg + sizeof(TMsgHeadline), msg->m_length - sizeof(TMsgHeadline)));
		const NewsHeadline* headline = m_newsDll->FindHeadline(m_stockNews, NewsResourceAsNumberArray(msg->m_length - sizeof(TMsgHeadline), (const char*)msg + sizeof(TMsgHeadline)));
		if(headline)
		{
			if(AddHeadlineToListBox(headline))
			{
				m_addedVisibleHeadline = true;
			}
/*
			if(m_displayLastHistoricStory)
			{
				RequestStory(headline);
			}
*/
		}
	}
	return false;
}

bool SampleDialogSettingNews::ProcessUnappliedMessage(const Message* message)
{
	switch(message->m_type)
	{
		case TM_HEADLINE:
		return DoAddHeadline((const TMsgHeadline*)message);

		default:
		return false;
	}
}

void SampleDialogSettingNews::PopulateStockNews()
{
	if(m_newsDll && m_stockNews)
	{
		const NewsHeadline* headline;
		TakionIterator* iterator = m_newsDll->CreateHeadlineIterator(m_stockNews);
		if(iterator)
		{
			m_ListBoxNews.SetRedraw(FALSE);
			m_newsDll->LockInquiryWaitHeadlines(m_stockNews);
			iterator->Reset();
			while(headline = m_newsDll->GetNextHeadline(iterator))
			{
//				NewRow(headline);
//				m_ListBoxNews.AddString((const char*)new SampleHeadline(headline));
				AddHeadlineToListBox(headline);
			}
			m_newsDll->UnlockInquiryHeadlines(m_stockNews);

			m_ListBoxNews.SetRedraw(TRUE);
			m_ListBoxNews.Invalidate(FALSE);

			delete iterator;

//			InvalidateTotalValueRect(listBoxNewsId, NHCI_DATETIME);
//			DisplayLastHistoricStory();
		}
	}
}

void SampleDialogSettingNews::UpdateStockNews()
{
	if(m_stockNews)
	{
		m_stockNews->AddObserver(this);
		if(m_newsDll)
		{
			PopulateStockNews();
//						m_unappliedHeadlineReader.SetLineBufferArray(m_stockNews->GetUnappliedHeadlines());
			m_unappliedHeadlineReader.SetLineBufferArray(m_newsDll->GetUnappliedHeadlines(m_stockNews));
			m_newsDll->RequestHeadlines(m_stockNews, m_daysBack, m_headlineCount, m_afterMarket);
			m_ListBoxNews.SetBusy(true);
			EnableControl(m_ButtonSubscribe.m_hWnd, false, m_ListBoxNews.m_hWnd);
		}
//					m_stockNews->GetHeadlines();
	}
	else
	{
		ReleaseNewsCollection();
//		m_unappliedHeadlineReader.SetLineBufferArray(nullptr);
	}
}
/*
void SampleDialogSettingNews::SetNoProfile(bool noProfile)
{
	if(m_noProfile != noProfile)
	{
		m_noProfile = noProfile;
	}
}
*/
const std::string* SampleDialogSettingNews::UpdateSearchFilter(std::string& topicStr)
{
//	std::string topicStr;
	const std::string* str = nullptr;
	switch(m_searchType)
	{
		case SEA_UNFILTERED:
		str = m_newsDll->GetSearchStringUnfiltered();
		break;

		case SEA_ALL:
		str = m_newsDll->GetSearchStringAll();
		break;

		case SEA_ALL_US:
		str = m_newsDll->GetSearchStringAllUs();
		break;

		case SEA_TOPIC:
		if(m_topicOrdinal < (unsigned short)m_ComboBoxTopic.GetItemCount())
		{
			topicStr = (const char*)m_ComboBoxTopic.GetItemDataPtr(m_topicOrdinal);
			str = &topicStr;
		}
		break;
	}
	if(str)
	{
		m_basketFilter = *str;
	}
	return str;
}

void SampleDialogSettingNews::SetNewsConnected(bool connected)
{
	if(m_newsConnected != connected)
	{
		Observable* storyObservable = m_newsDll->GetStoryObservable();
		m_newsConnected = connected;
		if(m_newsConnected)
		{
			if(storyObservable)
			{
				storyObservable->AddObserver(this);
			}
			UpdateWireList();
//			UpdateSearchFilter();
			OnSubscribe();
		}
		else
		{
			if(storyObservable)
			{
				storyObservable->RemoveObserver(this);
			}
//			UpdateWireList();
			m_unappliedHeadlineReader.SetLineBufferArray(nullptr);
			m_newsCollection = nullptr;
			m_stockNews = nullptr;
//			SetNoProfile(true);
		}
/*
		if(m_newsConnected)
		{
			if(!m_symbol.empty())
			{
				SymbolAccepted(m_symbol.c_str(), false);
			}
		}
*/
	}
}

void SampleDialogSettingNews::DisplayStoryText(const char* resourceId, const std::string& story, const bool error)
{
	m_displayedStoryResourceId = resourceId;
	m_storyRequestId.clear();
	m_EditStory.SetWindowText(story.c_str());
	m_EditStoryResource.SetWindowText(resourceId);
	UpdateSelectedLineStoryDisplayed();
	if(error != m_storyError)
	{
		m_storyError = error;
		m_EditStory.Invalidate(TRUE);
		m_EditStoryResource.Invalidate(TRUE);
	}
}

void SampleDialogSettingNews::Notify(const Message* message, const Observable* from, const Message* info)
{
	switch(message->m_type)
	{
		case TM_STOCK_UPDATE_HEADLINES:
		case TM_STRING_UPDATE_HEADLINES:
		UpdateHeadlines();
		break;

		case TM_STOCK_NEWS_HISTORIC_SEARCH_DONE:
		{
			const TMsgStockNewsHistoricSearchDone* msg = (const TMsgStockNewsHistoricSearchDone*)message;
			std::string str(msg->m_symbol);
			str += ' ';
			char num[33];
			str += U_UnsignedNumberToStrNoDecDigits(msg->m_newsCollectionId, num, sizeof(num));
			TD_Log(str.c_str(), "TM_STOCK_NEWS_HISTORIC_SEARCH_DONE");
			StopSearch();
		}
		break;

		case TM_STRING_NEWS_HISTORIC_SEARCH_DONE:
//		DisplayLastHistoricStory();
		{
			const TMsgStringNewsHistoricSearchDone* msg = (const TMsgStringNewsHistoricSearchDone*)message;
			
			char num[33];
			std::string str = U_UnsignedNumberToStrNoDecDigits(msg->m_newsCollectionId, num, sizeof(num));
			if(msg->m_length > sizeof(TMsgStringNewsHistoricSearchDone))
			{
				str += ' ';
				const unsigned short length = msg->m_length - sizeof(TMsgStringNewsHistoricSearchDone);
				const unsigned short length1 = length + 1;
				char* buf = new char[length1];
				strncpy_s(buf, length1, (const char*)msg + sizeof(TMsgStringNewsHistoricSearchDone), length);
				buf[length] = '\0';
				str += buf;
				delete[] buf;
			}
			TD_Log(str.c_str(), "TM_STRING_NEWS_HISTORIC_SEARCH_DONE");
			StopSearch();
		}
		break;

		case TM_NEWS_STORY_CONTENTS:
		{
			NewsDll* newsDll = TD_GetNewsDll();
			if(newsDll)
			{
				const TMsgNewsStoryContents* msg = (const TMsgNewsStoryContents*)message;
				const NewsStory* story = msg->m_newsStory;

				std::string storyText;
				std::string title;
				bool error;

				newsDll->LockInquiryWaitStory();
				storyText = story->GetStory();
				title = story->GetResourceIdStr();
				error = story->isStoryError();
				newsDll->UnlockInquiryStory();

				if(!strcmp(m_storyRequestId.c_str(), title.c_str()))
				{
					DisplayStoryText(title.c_str(), storyText, error);
				}
			}
		}
		break;

		case TM_DISCONNECTED:
		if(((const TMessageConnection*)message)->m_connectionDataType == CDT_NEWS)
		{
			SetNewsConnected(false);//m_newsDll->isConnected());
		}
		break;

		case TM_CONNECTED:
		if(((const TMessageConnection*)message)->m_connectionDataType == CDT_NEWS)
		{
			SetNewsConnected(true);
		}
		break;

		case TM_MARKET_STATUS:
		{
		}
		break;

		case TM_NEXT_DAY_STARTED:
		{
//			NextDayStarted(((const TMsgNextDayStarted*)message)->m_date);
		}
		break;

		case TM_MARKET_SORTER_LOADED:
//		MarketSorterLoaded();
		break;

		case TM_MARKET_SORTER_CLEANUP:
//		MarketSorterCleanup();
		break;

		case TM_DATA_THREAD_BUFFER_OVERFLOW:
//		PostMessage(WM_USER + 500, ((const TMsgDataThreadBufferOverflow*)message)->m_threadId, 0);
		break;
	}
}

void SampleDialogSettingNews::DoPaint(const RECT& rcPaint, CDC& dc)
{
	BaseDialogSetting::DoPaint(rcPaint, dc);
	CRect intersectRect;
	if(intersectRect.IntersectRect(&m_stockCountRect, &rcPaint))
	{
		unsigned int count = m_ListBoxStock.GetItemCount();
//		if(count >= 0)
//		{
			HGDIOBJ oldFont = dc.SelectObject(GetFont());
			char num[33];
			dc.DrawText(U_UnsignedNumberToStr(count, num, sizeof(num)), -1, &m_stockCountRect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP|DT_NOPREFIX);//|DT_END_ELLIPSIS);
			dc.SelectObject(oldFont);
//		}
	}
}

void SampleDialogSettingNews::Resize(bool widthChanged, bool heightChanged)
{
	if(m_ListBoxNews.m_hWnd)
	{
		if(!m_bottomOffset)
		{
			CRect rect;
			m_ListBoxNews.GetWindowRect(&rect);
			ScreenToClient(&rect);

			m_leftOffset = rect.left;
			m_topOffset = rect.top;

			CRect clientRect;
			m_ListBoxNews.GetClientRect(&clientRect);
			m_listBoxBorderSize2 = rect.Width() - clientRect.Width();
			m_listBoxBorderSize = m_listBoxBorderSize2 / 2;

			GetClientRect(&clientRect);
			m_bottomOffset = clientRect.bottom - rect.bottom;

			m_titleOffset = m_leftOffset + m_listBoxBorderSize;

			m_ListBoxStock.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_stockWidth = rect.Width();
			m_stockLeftOffset = rect.left;
			m_stockTopOffset = rect.top;

			m_stockCountRect.right = rect.right;

			m_ButtonRequestStory.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_maxStockHeight = rect.bottom - m_stockTopOffset;

			m_RadioStock.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_stockCountRect.left = rect.right;
			m_stockCountRect.top = rect.top;
			m_stockCountRect.bottom = rect.bottom;

			m_EditStory.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_storyWidth = rect.Width();
			m_storyTopOffset = rect.top;
		}
		int bottom = m_clientRect.bottom - m_bottomOffset;
		m_ListBoxNews.SetWindowPos(nullptr, m_leftOffset, m_topOffset, m_clientRect.right - m_leftOffset, bottom - m_topOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);

		if(heightChanged)
		{
			const int stockHeight = m_clientRect.bottom - m_stockTopOffset;
			const bool stockHeightSmall = stockHeight < m_maxStockHeight;
			if(stockHeightSmall || m_stockHeight < m_maxStockHeight)
			{
				m_stockHeight = stockHeightSmall ? stockHeight : m_maxStockHeight;
				m_ListBoxStock.SetWindowPos(nullptr, m_stockLeftOffset, m_stockTopOffset, m_stockWidth, m_stockHeight, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
			}
			if(!stockHeightSmall)
			{
				m_EditStory.SetWindowPos(nullptr, 0, m_storyTopOffset, m_storyWidth, m_clientRect.bottom - m_storyTopOffset, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
			}
		}
	}

	BaseDialogSetting::Resize(widthChanged, heightChanged);
	if(heightChanged)InvalidateAllTotalValueRects(listBoxNewsId);
}

void SampleDialogSettingNews::GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const
{
	titleRect.bottom = m_topOffset;
	titleRect.top = titleRect.bottom - m_bottomOffset;

	totalRect.bottom = m_clientRect.bottom;
	totalRect.top = totalRect.bottom - m_bottomOffset;

	const LbHorizontalScroller* scroller = listBox->GetHorizontalScroller();
	if(scroller)
	{
		titleRect.left = totalRect.left = scroller->GetColumnLeftOffset() + m_titleOffset;
		titleRect.right = totalRect.right = m_clientRect.right;
	}
}

const char* SampleDialogSettingNews::GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const
{
	switch(columnId)
	{
		case NHCI_DATETIME:
		return U_UnsignedNumberToStrWithCommas(m_ListBoxNews.GetItemCount(), buf, buflen);

		default:
		return nullptr;
	}
}

void SampleDialogSettingNews::TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff)
{
/*
	m_totalClosedPnlRect.left += diff;
	m_totalOpenPnlRect.left += diff;
	m_totalOpenPrintPnlRect.left += diff;
	m_totalRangeAtFirstTimerRect.left += diff;

	m_totalClosedPnlRect.right += diff;
	m_totalOpenPnlRect.right += diff;
	m_totalOpenPrintPnlRect.right += diff;
	m_totalRangeAtFirstTimerRect.right += diff;
*/
}
