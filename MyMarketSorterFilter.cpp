#include "stdafx.h"
#include "MyMarketSorterFilter.h"
#include "MyMarketSorter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MySymbolFilter::MySymbolFilter():
	m_useExclude(false),
	m_useInclude(false),
#ifndef USE_CMAP_FOR_FILTER_SYMBOLS
	m_excludeSymbolSetEnd(m_excludeSymbolSet.cend()),
	m_includeSymbolSetEnd(m_includeSymbolSet.cend()),
#endif
	m_excludeIncludeSymbolMapEnd(m_excludeIncludeSymbolMap.cend()),
	m_useFirstCharSet(false),
	m_useNoFirstCharSet(false),
	m_useCharSet(false),
	m_useNoCharSet(false),
	m_firstCharSetEnd(m_firstCharSet.cend()),
	m_noFirstCharSetEnd(m_noFirstCharSet.cend()),
	m_charSetEnd(m_charSet.cend()),
	m_noCharSetEnd(m_noCharSet.cend())
{
#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
	m_excludeSymbolSet.InitHashTable(9001);
	m_includeSymbolSet.InitHashTable(9001);
#endif
}

MySymbolFilter::MySymbolFilter(const MySymbolFilter& other):
	m_useExclude(other.m_useExclude),
	m_useInclude(other.m_useInclude),
#ifndef USE_CMAP_FOR_FILTER_SYMBOLS
	m_excludeSymbolSet(other.m_excludeSymbolSet),
	m_includeSymbolSet(other.m_includeSymbolSet),
	m_excludeSymbolSetEnd(m_excludeSymbolSet.end()),
	m_includeSymbolSetEnd(m_includeSymbolSet.end()),
#endif
	m_excludeIncludeSymbolMap(other.m_excludeIncludeSymbolMap),
	m_excludeIncludeSymbolMapEnd(m_excludeIncludeSymbolMap.cend()),

	m_useFirstCharSet(other.m_useFirstCharSet),
	m_useNoFirstCharSet(other.m_useNoFirstCharSet),
	m_useCharSet(other.m_useCharSet),
	m_useNoCharSet(other.m_useNoCharSet),

	m_firstCharString(other.m_firstCharString),
	m_noFirstCharString(other.m_noFirstCharString),
	m_charString(other.m_charString),
	m_noCharString(other.m_noCharString),

	m_firstCharSet(other.m_firstCharSet),
	m_noFirstCharSet(other.m_noFirstCharSet),
	m_charSet(other.m_charSet),
	m_noCharSet(other.m_noCharSet),

	m_firstCharSetEnd(m_firstCharSet.end()),
	m_noFirstCharSetEnd(m_noFirstCharSet.end()),
	m_charSetEnd(m_charSet.end()),
	m_noCharSetEnd(m_noCharSet.end())
{
#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
	m_excludeSymbolSet.InitHashTable(9001);
	m_includeSymbolSet.InitHashTable(9001);
#endif
}

void SetString(std::string& charString, MySymbolFilter::CharSet& charSet, MySymbolFilter::CharSet::const_iterator& charSetEnd, const std::string& str)
{
	if(charString != str)
	{
		charString = str;
		charSet.clear();
		for(const char* cursor = charString.c_str(); *cursor; ++cursor)
		{
			charSet.insert(*cursor);
		}
		charSetEnd = charSet.end();
	}
}

void MySymbolFilter::SetFirstCharString(const std::string& str)
{
	SetString(m_firstCharString, m_firstCharSet, m_firstCharSetEnd, str);
}

void MySymbolFilter::SetNoFirstCharString(const std::string& str)
{
	SetString(m_noFirstCharString, m_noFirstCharSet, m_noFirstCharSetEnd, str);
}

void MySymbolFilter::SetCharString(const std::string& str)
{
	SetString(m_charString, m_charSet, m_charSetEnd, str);
}

void MySymbolFilter::SetNoCharString(const std::string& str)
{
	SetString(m_noCharString, m_noCharSet, m_noCharSetEnd, str);
}

bool MySymbolFilter::MyFieldsEqual(const MySymbolFilter& other, unsigned int doNotCopyFlags) const
{
	return (doNotCopyFlags & (1 << FDNC_SYMBOL)) != 0 ||
			
		m_useExclude == other.m_useExclude
		&& m_useInclude == other.m_useInclude
		&& m_excludeSymbolSet == other.m_excludeSymbolSet
		&& m_includeSymbolSet == other.m_includeSymbolSet

		&& m_useFirstCharSet == other.m_useFirstCharSet
		&& m_useNoFirstCharSet == other.m_useNoFirstCharSet
		&& m_useCharSet == other.m_useCharSet
		&& m_useNoCharSet == other.m_useNoCharSet

		&& m_firstCharString == other.m_firstCharString
		&& m_noFirstCharString == other.m_noFirstCharString
		&& m_charString == other.m_charString
		&& m_noCharString == other.m_noCharString
		;
}

void MySymbolFilter::CopyMyFields(const MySymbolFilter& other, unsigned int doNotCopyFlags)
{
	if(!(doNotCopyFlags & (1 << FDNC_SYMBOL)))
	{
		m_useExclude = other.m_useExclude;
		m_useInclude = other.m_useInclude;

#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
#else
		m_excludeSymbolSet = other.m_excludeSymbolSet;
		m_includeSymbolSet = other.m_includeSymbolSet;
#endif
		m_excludeIncludeSymbolMap = other.m_excludeIncludeSymbolMap;

		m_useFirstCharSet = other.m_useFirstCharSet;
		m_useNoFirstCharSet = other.m_useNoFirstCharSet;
		m_useCharSet = other.m_useCharSet;
		m_useNoCharSet = other.m_useNoCharSet;

		m_firstCharString = other.m_firstCharString;
		m_noFirstCharString = other.m_noFirstCharString;
		m_charString = other.m_charString;
		m_noCharString = other.m_noCharString;
	
		m_firstCharSet = other.m_firstCharSet;
		m_noFirstCharSet = other.m_noFirstCharSet;
		m_charSet = other.m_charSet;
		m_noCharSet = other.m_noCharSet;

		m_firstCharSetEnd = m_firstCharSet.end();
		m_noFirstCharSetEnd = m_noFirstCharSet.end();
		m_charSetEnd = m_charSet.end();
		m_noCharSetEnd = m_noCharSet.end();
	}
}

void MySymbolFilter::Copy(const TakionFilter& filter)
{
	TakionFilter::Copy(filter);
	CopyMyFields((const MySymbolFilter&)filter, 0);
}

void MySymbolFilter::toString(std::string& str) const
{
	U_AppendBool(str, m_useExclude, settingDel);
	U_AppendBool(str, m_useInclude, settingDel);

	U_AppendBool(str, m_useFirstCharSet, settingDel);
	str += m_firstCharString;
	str += settingDel;

	U_AppendBool(str, m_useNoFirstCharSet, settingDel);
	str += m_noFirstCharString;
	str += settingDel;

	U_AppendBool(str, m_useCharSet, settingDel);
	str += m_charString;
	str += settingDel;

	U_AppendBool(str, m_useNoCharSet, settingDel);
	str += m_noCharString;
	str += settingDel;

	typedef std::set<unsigned __int64, lessUIntAsStr<unsigned __int64> > UInt64SortedSet;

	str += groupDel;
	UInt64SortedSet sortedSet;
#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
	for(const UInt64Set::CPair* p = m_excludeSymbolSet.PGetFirstAssoc(); p; p = m_excludeSymbolSet.PGetNextAssoc(p))
	{
		sortedSet.insert(p->key);
	}
#else
	UInt64Set::const_iterator it = m_excludeSymbolSet.cbegin();//, itend = m_excludeSymbolSet.end();
	for(; it != m_excludeSymbolSetEnd; ++it)
	{
		sortedSet.insert(*it);
	}
#endif
	UInt64SortedSet::const_iterator its = sortedSet.cbegin(), itsend = sortedSet.cend();
	for(; its != itsend; ++its)
	{
		str += (const char*)&*its;
		str += settingDel;
	}

	sortedSet.clear();
	str += groupDel;
#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
	for(const UInt64Set::CPair* p = m_includeSymbolSet.PGetFirstAssoc(); p; p = m_includeSymbolSet.PGetNextAssoc(p))
	{
		sortedSet.insert(p->key);
	}
#else
	for(it = m_includeSymbolSet.begin(); it != m_includeSymbolSetEnd; ++it)
	{
		sortedSet.insert(*it);
	}
#endif
	for(its = sortedSet.begin(); its != itsend; ++its)
	{
		str += (const char*)&*its;
		str += settingDel;
	}

	str += groupDel;

	str += settingSubdel;
}

void MySymbolFilter::fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier)
{
	if(version >= U_GetVersionFromTokens(1, 0, 0, 115))
	{
		if(contentModifier & (1 << WTSC_NO_FILTER_STOCK_LIST))
		{
			U_SkipToDelimiter(cursor, settingSubdel);
		}
		else
		{
			ClearExcludeIncludeSymbolSet();
			bool endChange;
			char* end = U_MarkSectionEnd((char*)cursor, settingSubdel, endChange);
			bool groupEndChange;
			char* groupEnd = U_MarkSectionEnd((char*)cursor, groupDel, groupEndChange);

			m_useExclude = U_ReadBoolToken(cursor, settingDel);
			m_useInclude = U_ReadBoolToken(cursor, settingDel);

			m_useFirstCharSet = U_ReadBoolToken(cursor, settingDel);
			std::string token;
			U_ReadStrToken(token, cursor, settingDel);
			SetFirstCharString(token);

			m_useNoFirstCharSet = U_ReadBoolToken(cursor, settingDel);
			token.clear();
			U_ReadStrToken(token, cursor, settingDel);
			SetNoFirstCharString(token);

			m_useCharSet = U_ReadBoolToken(cursor, settingDel);
			token.clear();
			U_ReadStrToken(token, cursor, settingDel);
			SetCharString(token);

			m_useNoCharSet = U_ReadBoolToken(cursor, settingDel);
			token.clear();
			U_ReadStrToken(token, cursor, settingDel);
			SetNoCharString(token);

			if(groupEnd != end && groupEndChange)
			{
				*groupEnd = groupDel;

				cursor = groupEnd + 1;
				groupEnd = U_MarkSectionEnd((char*)cursor, groupDel, groupEndChange);
		//Exclude collection
				std::string symbol;
				unsigned __int64 numSymbol;
				while(*cursor)
				{
					symbol.clear();
					U_ReadStrToken(symbol, cursor, settingDel);
					if(!symbol.empty())
					{
						numSymbol = U_StringToUInt64Term0(symbol.c_str());
						m_excludeSymbolSet.insert(numSymbol);
						m_excludeIncludeSymbolMap.insert(ExcludeIncludeSymbolMap::value_type(numSymbol, false));
					}
				}
				if(groupEnd != end && groupEndChange)
				{
					*groupEnd = groupDel;

					cursor = groupEnd + 1;
					groupEnd = U_MarkSectionEnd((char*)cursor, groupDel, groupEndChange);
		//Include collection
					while(*cursor)
					{
						symbol.clear();
						U_ReadStrToken(symbol, cursor, settingDel);
						if(!symbol.empty())
						{
							numSymbol = U_StringToUInt64Term0(symbol.c_str());
							m_includeSymbolSet.insert(numSymbol);
							m_excludeIncludeSymbolMap.insert(ExcludeIncludeSymbolMap::value_type(numSymbol, true));
						}
					}
					if(groupEnd != end && groupEndChange)
					{
						*groupEnd = groupDel;
					}
				}
			}
			if(endChange)*end = settingSubdel;
			cursor = end + 1;
		}
	}
	else
	{
		ClearExcludeIncludeSymbolSet();
	}
}

unsigned char MySymbolFilter::ExcludeSymbol(unsigned __int64 symbol)
{
	unsigned char ret;
	if(m_useExclude)
	{
		ret = 0;
	}
	else
	{
		ret = 2;
		m_useExclude = true;
	}
	if(UnIncludeSymbol(symbol))
	{
		ret |= 1;
	}
	if(m_excludeSymbolSet.insert(symbol).second)
	{
		m_excludeIncludeSymbolMap.insert(ExcludeIncludeSymbolMap::value_type(symbol, false));
		ret |= 1;
	}
	if((ret & 2) && m_excludeSymbolSet.size() == 1)
	{
		ret &= ~2;
	}
	return ret;
}

unsigned char MySymbolFilter::IncludeSymbol(unsigned __int64 symbol)
{
	unsigned char ret;
	if(m_useInclude)
	{
		ret = 0;
	}
	else
	{
		ret = 2;
		m_useInclude = true;
	}
	if(UnExcludeSymbol(symbol))
	{
		ret |= 1;
	}
	if(m_includeSymbolSet.insert(symbol).second)
	{
		m_excludeIncludeSymbolMap.insert(ExcludeIncludeSymbolMap::value_type(symbol, true));
		ret |= 1;
	}
	if((ret & 2) && m_includeSymbolSet.size() == 1)
	{
		ret &= ~2;
	}
	return ret;
}

bool MySymbolFilter::isFiltering() const
{
	return m_useFirstCharSet// && !m_firstCharSet.empty()
		|| m_useNoFirstCharSet && !m_noFirstCharSet.empty()
		|| m_useCharSet
		|| m_useNoCharSet && !m_noCharSet.empty()
		|| m_useExclude && !m_excludeSymbolSet.empty()
		|| m_useInclude && !m_includeSymbolSet.empty()
		;
}
bool MySymbolFilter::isEmpty() const
{
	return !m_useFirstCharSet && m_firstCharSet.empty()
		&& !m_useNoFirstCharSet && m_noFirstCharSet.empty()
		&& !m_useCharSet && m_charSet.empty()
		&& !m_useNoCharSet && m_noCharSet.empty()
		&& !m_useExclude && m_excludeSymbolSet.empty()
		&& !m_useInclude && m_includeSymbolSet.empty()
		;
}

void MySymbolFilter::Reset()
{
	m_useExclude = false;
	m_useInclude = false;

	ClearExcludeIncludeSymbolSet();

	m_useFirstCharSet = false;
	m_useNoFirstCharSet = false;
	m_useCharSet = false;
	m_useNoCharSet = false;

	m_firstCharString.clear();
	m_noFirstCharString.clear();
	m_charString.clear();
	m_noCharString.clear();

	m_firstCharSet.clear();
	m_noFirstCharSet.clear();
	m_charSet.clear();
	m_noCharSet.clear();
}

MySymbolFilter& MySymbolFilter::operator=(const MySymbolFilter& other)
{
	TakionFilter::operator=(other);
	m_useExclude = other.m_useExclude;
	m_useInclude = other.m_useInclude;

#ifdef USE_CMAP_FOR_FILTER_SYMBOLS
#else
	m_excludeSymbolSet = other.m_excludeSymbolSet;
	m_includeSymbolSet = other.m_includeSymbolSet;
#endif

	m_excludeIncludeSymbolMap = other.m_excludeIncludeSymbolMap;

	m_useFirstCharSet = other.m_useFirstCharSet;
	m_useNoFirstCharSet = other.m_useNoFirstCharSet;
	m_useCharSet = other.m_useCharSet;
	m_useNoCharSet = other.m_useNoCharSet;

	m_firstCharString = other.m_firstCharString;
	m_noFirstCharString = other.m_noFirstCharString;
	m_charString = other.m_charString;
	m_noCharString = other.m_noCharString;

	m_firstCharSet = other.m_firstCharSet;
	m_noFirstCharSet = other.m_noFirstCharSet;
	m_charSet = other.m_charSet;
	m_noCharSet = other.m_noCharSet;

	return *this;
}

bool MySymbolFilter::isFilterSame(const TakionFilter& filter) const
{
	const MySymbolFilter& other = (const MySymbolFilter&)filter;
	return (m_useExclude == other.m_useExclude || m_excludeSymbolSet.empty() && other.m_excludeSymbolSet.empty())
		&& (m_useInclude == other.m_useInclude || m_includeSymbolSet.empty() && other.m_includeSymbolSet.empty())
		&& (!m_useExclude && !other.m_useExclude || m_excludeSymbolSet == other.m_excludeSymbolSet)
		&& (!m_useInclude && !other.m_useInclude || m_includeSymbolSet == other.m_includeSymbolSet)

		&& m_useFirstCharSet == other.m_useFirstCharSet// || m_firstCharSet.empty() && other.m_firstCharSet.empty())
		&& (m_useNoFirstCharSet == other.m_useNoFirstCharSet || m_noFirstCharSet.empty() && other.m_noFirstCharSet.empty())
		&& m_useCharSet == other.m_useCharSet// || m_charSet.empty() && other.m_charSet.empty())
		&& (m_useNoCharSet == other.m_useNoCharSet || m_noCharSet.empty() && other.m_noCharSet.empty())

		&& (!m_useFirstCharSet && !other.m_useFirstCharSet || m_firstCharSet == other.m_firstCharSet)
		&& (!m_useNoFirstCharSet && !other.m_useNoFirstCharSet || m_noFirstCharSet == other.m_noFirstCharSet)
		&& (!m_useCharSet && !other.m_useCharSet || m_charSet == other.m_charSet)
		&& (!m_useNoCharSet && !other.m_useNoCharSet || m_noCharSet == other.m_noCharSet)
		;
}

unsigned char MySymbolFilter::isPotentialFilterSame(const TakionFilter& filter) const//1 - add; 2 - remove; 3 - both
{
	unsigned char ret = 0;
	const MySymbolFilter& other = (const MySymbolFilter&)filter;
	if(m_useFirstCharSet)
	{
		if(other.m_useFirstCharSet)
		{
			if(m_firstCharSet.empty())
			{
				if(!other.m_firstCharSet.empty())
				{
					ret |= 1;//add
				}
			}
			else if(other.m_firstCharSet.empty())
			{
				ret |= 2;//remove
			}
			else if(m_firstCharSet != other.m_firstCharSet)
			{
				ret = 3;
			}
		}
		else// if(!m_firstCharSet.empty())
		{
			ret |= 1;//add
		}
	}
	else if(other.m_useFirstCharSet)
	{
		ret |= 2;//remove
	}

	if(m_useCharSet)
	{
		if(other.m_useCharSet)
		{
			if(m_charSet.empty())
			{
				if(!other.m_charSet.empty())
				{
					ret |= 1;//add
				}
			}
			else if(other.m_charSet.empty())
			{
				ret |= 2;//remove
			}
			else if(m_charSet != other.m_charSet)
			{
				ret = 3;
			}
		}
		else// if(!m_charSet.empty())
		{
			ret |= 1;//add
		}
	}
	else if(other.m_useCharSet)
	{
		ret |= 2;//remove
	}

	if(m_useNoFirstCharSet)
	{
		if(other.m_useNoFirstCharSet)
		{
			if(m_noFirstCharSet.empty())
			{
				if(!other.m_noFirstCharSet.empty())
				{
					ret |= 2;//remove
				}
			}
			else if(other.m_noFirstCharSet.empty())
			{
				ret |= 1;//add
			}
			else if(m_noFirstCharSet != other.m_noFirstCharSet)
			{
				ret = 3;
			}
		}
		else if(!m_noFirstCharSet.empty())
		{
			ret |= 1;//add
		}
	}
	else if(other.m_useNoFirstCharSet)
	{
		if(!other.m_noFirstCharSet.empty())
		{
			ret |= 2;//remove
		}
	}

	if(m_useNoCharSet)
	{
		if(other.m_useNoCharSet)
		{
			if(m_noCharSet.empty())
			{
				if(!other.m_noCharSet.empty())
				{
					ret |= 2;//remove
				}
			}
			else if(other.m_noCharSet.empty())
			{
				ret |= 1;//add
			}
			else if(m_noCharSet != other.m_noCharSet)
			{
				ret = 3;
			}
		}
		else if(!m_noCharSet.empty())
		{
			ret |= 1;//add
		}
	}
	else if(other.m_useNoCharSet)
	{
		if(!other.m_noCharSet.empty())
		{
			ret |= 2;//remove
		}
	}

	if(m_useExclude)
	{
		if(other.m_useExclude)
		{
			if(isExcludeSetEmpty())
			{
				if(!other.isExcludeSetEmpty())
				{
					ret |= 2;//add
				}
			}
			else if(other.isExcludeSetEmpty())
			{
				ret |= 1;
			}
			else if(m_excludeSymbolSet != other.m_excludeSymbolSet)
			{
				ret = 3;
			}
		}
		else
		{
			if(!isExcludeSetEmpty())
			{
				ret |= 1;//remove
			}
		}
	}
	else if(other.m_useExclude)
	{
		if(!other.isExcludeSetEmpty())
		{
			ret |= 2;
		}
	}
	if(m_useInclude)
	{
		if(other.m_useInclude)
		{
			if(isIncludeSetEmpty())
			{
				if(!other.isIncludeSetEmpty())
				{
					ret |= 1;//remove
				}
			}
			else if(other.isIncludeSetEmpty())
			{
				ret |= 2;//add
			}
			else if(m_includeSymbolSet != other.m_includeSymbolSet)
			{
				ret = 3;
			}
		}
		else
		{
			if(!isIncludeSetEmpty())
			{
				ret |= 2;//add
			}
		}
	}
	else if(other.m_useInclude)
	{
		if(!other.isIncludeSetEmpty())
		{
			ret |= 1;
		}
	}
	return ret;
}

enum TakionSymbolFilterDialogIds
{
	toolTipsCheckBoxId = 1010,

	includeCheckBoxId,
	excludeCheckBoxId,
	symbolListBoxId,
	symbolEditId,
	includeButtonId,
	excludeButtonId,
	removeButtonId,
	clearButtonId,
	findButtonId,

	symbolCharGroupId,
	firstCharsCheckBoxId,
	firstCharsEditId,
	noFirstCharsCheckBoxId,
	noFirstCharsEditId,
	charsCheckBoxId,
	charsEditId,
	noCharsCheckBoxId,
	noCharsEditId,

	symbolFilterCount,
};

MyDialogSymbolFilter::MyDialogSymbolFilter(TakionMainWnd* mainWnd, int x_symbol, int listBoxHeight, bool toDestroyOnFocus, TakionFrame* receipient, CWnd* parent, LPCSTR caption, DWORD style, DWORD exstendedStyle, int x, int y, int w, int h,
		LPCSTR font, WORD fontSize, bool executeCommands, bool closeOnCommandExecution):
	DialogFilter(mainWnd, toDestroyOnFocus, receipient, parent, caption, style, exstendedStyle, x, y, w, h + buttonHeight, font, fontSize, executeCommands, closeOnCommandExecution),
	m_enabledColor(m_visualBase->GetOkColor()),
	m_enabledTextColor(m_visualBase->GetOkTextColor()),
	m_disabledColor(m_visualBase->GetLightGrayColor()),
	m_invalidColor(m_visualBase->GetPendingColor()),
	m_checkBoxColor(GetSysColor(COLOR_WINDOW)),
	m_pressedCheckColor(m_visualBase->GetLightGrayColor()),

	m_CheckBoxToolTips("Show ToolTips in Filter Dialogs", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), m_visualBase->GetOkTextColor(), m_visualBase->GetLightGrayColor()),

	m_CheckBoxSymbolInclude("Use \"included\" (checked) symbols in filter (always filter them in)", 0xFFFFFFFF, 0xFFFFFFFF, m_checkBoxColor, m_enabledTextColor, m_pressedCheckColor),
	m_CheckBoxSymbolExclude("Use \"excluded\" (unchecked) symbols in filter (always filter them out)", 0xFFFFFFFF, 0xFFFFFFFF, m_checkBoxColor, m_enabledTextColor, m_pressedCheckColor),
	m_EditSymbol(U_GetValidStockChars(), false, false, nullptr, true, false, "Stock Symbol"),
	m_ListBoxSymbol(&m_EditSymbol, true, false, false, true, "Filter in (checked) and Filter out (unchecked) stock symbols"),

	m_CheckBoxFirstChars("Filter in stocks by the first character of symbol", 0xFFFFFFFF, 0xFFFFFFFF, m_checkBoxColor, m_enabledTextColor, m_pressedCheckColor),
	m_CheckBoxNoFirstChars("Filter out stocks by the first character of symbol", 0xFFFFFFFF, 0xFFFFFFFF, m_checkBoxColor, m_enabledTextColor, m_pressedCheckColor),
	m_CheckBoxChars("Filter in stocks by characters in the symbol", 0xFFFFFFFF, 0xFFFFFFFF, m_checkBoxColor, m_enabledTextColor, m_pressedCheckColor),
	m_CheckBoxNoChars("Filter out stocks by characters in the symbol", 0xFFFFFFFF, 0xFFFFFFFF, m_checkBoxColor, m_enabledTextColor, m_pressedCheckColor),

	m_EditFirstChars(U_GetValidStockFirstChars(), false, false, nullptr, true, false, "Filter in only symbols that start with these characters"),
	m_EditNoFirstChars(U_GetValidStockFirstChars(), false, false, nullptr, true, false, "Filter out all symbols that start with these characters"),
	m_EditChars(U_GetValidStockChars(), false, false, nullptr, true, false, "Filter in only symbols that contain these characters"),
	m_EditNoChars(U_GetValidStockChars(), false, false, nullptr, true, false, "Filter out all symbols that contain these characters"),

	m_useFirstChars(false),
	m_useNoFirstChars(false),
	m_useChars(false),
	m_useNoChars(false),

	m_lastUsedSymbolInclude(true)
{

#ifndef USE_SPACE_IN_STOCK_SYMBOLS
	m_EditChars.AddValidChar(' ', false);
	m_EditNoChars.AddValidChar(' ', false);
#endif
	y = topMost;
	const int width = firstColumnWidth;
	const int buttonH = 12;

	const int tooltipsX = m_leftHandSideButtonEnd + 3 * buttonGapH;
	const int tooltipsWidth = buttonWidth + checkBoxHeight;
	AddButtonIndirect("Tooltips", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, tooltipsX, m_buttonTop + (buttonHeight - checkBoxHeight) / 2, tooltipsWidth, checkBoxHeight, toolTipsCheckBoxId);

	AddButtonIndirect("Use Exclude", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, x_symbol, y, width, checkBoxHeight, excludeCheckBoxId);
	AddButtonIndirect("Use Include", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, x_symbol, y += (checkBoxHeight + vgap), width, checkBoxHeight, includeCheckBoxId);

	const int vgap1 = 1;
	const int bh = buttonH + vgap1;//vgap;

	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_SORT | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | WS_VSCROLL, 0, x_symbol, y += (checkBoxHeight + vgap), width, listBoxHeight, symbolListBoxId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, x_symbol, y += (listBoxHeight + vgap1), width, buttonH, symbolEditId);
	AddButtonIndirect("Find", WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON|WS_DISABLED, 0, x_symbol, y += bh, width, buttonH, findButtonId);
	AddButtonIndirect("Include", WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON|WS_DISABLED, 0, x_symbol, y += bh, width, buttonH, includeButtonId);
	AddButtonIndirect("Exclude", WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON|WS_DISABLED, 0, x_symbol, y += bh, width, buttonH, excludeButtonId);
	AddButtonIndirect("Remove", WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON|WS_DISABLED, 0, x_symbol, y += bh, width, buttonH, removeButtonId);
	AddButtonIndirect("Clear", WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON|WS_DISABLED, 0, x_symbol, y += bh, width, buttonH, clearButtonId);

	const int xs = x_symbol + hgap;
	const int ws = width - hgap - hgap;
	const int charHeight = 5 * checkBoxHeight + 4 * bh + vgap1;//vgap;

	AddButtonIndirect("Symbol Chars", WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP, WS_EX_TRANSPARENT, x_symbol, y += (buttonH + vgap1 + vgap1), width, charHeight, symbolCharGroupId);
	
	AddButtonIndirect("1st Chars", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, xs, y += checkBoxHeight, ws, checkBoxHeight, firstCharsCheckBoxId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xs, y += checkBoxHeight, ws, buttonH, firstCharsEditId);
	AddButtonIndirect("No 1st Chars", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, xs, y += bh, ws, checkBoxHeight, noFirstCharsCheckBoxId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xs, y += checkBoxHeight, ws, buttonH, noFirstCharsEditId);

	AddButtonIndirect("Chars", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, xs, y += bh, ws, checkBoxHeight, charsCheckBoxId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xs, y += checkBoxHeight, ws, buttonH, charsEditId);
	AddButtonIndirect("No Chars", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, xs, y += bh, ws, checkBoxHeight, noCharsCheckBoxId);
	AddEditBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | ES_UPPERCASE, 0, xs, y += checkBoxHeight, ws, buttonH, noCharsEditId);
}

int MyDialogSymbolFilter::AddRangeComponent(int groupLeft, int groupTop, int groupWidth, const char* useCheckBoxName, const RangeGroupBase& groupBase, const char* positiveCheckBoxName, const char* negativeCheckBoxName, int additionalHeight, int yOffset)//unsigned short useId, unsigned short minId, unsigned short maxId)
{
	const int x_control = groupLeft + hgap;
	const int controlWidth = groupWidth - hgap - hgap;
	int height = groupHeight + additionalHeight;
	const unsigned short idPositive = groupBase.GetIdPositive();
	if(idPositive)
	{
		height += checkBoxHeight;// + vgap;
	}

#ifndef FILTER_CUSTOM_GROUP_BOX
	AddButtonIndirect("", WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP, WS_EX_TRANSPARENT, groupLeft, groupTop, groupWidth, height, groupBase.GetIdGroup());
#endif
	AddButtonIndirect(useCheckBoxName, WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, x_control, groupTop += vgroupgap + yOffset, controlWidth, checkBoxHeight, groupBase.GetIdCheckBox());
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += checkBoxHeight, controlWidth, spinHeight, groupBase.GetIdSpinMin());
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += spinHeight + 1, controlWidth, spinHeight, groupBase.GetIdSpinMax());
	if(idPositive)
	{
		const int hg = 2;
		groupTop += spinHeight + 1;//vgap;
		const int w = (controlWidth - hg) / 2;
		AddButtonIndirect(positiveCheckBoxName ? positiveCheckBoxName : "Positive", WS_CHILD|WS_VISIBLE|BS_CHECKBOX|WS_TABSTOP, 0, x_control, groupTop, w - hg, checkBoxHeight, idPositive);
		AddButtonIndirect(negativeCheckBoxName ? negativeCheckBoxName : "Negative", WS_CHILD|WS_VISIBLE|BS_CHECKBOX|WS_TABSTOP, 0, x_control + w, groupTop, w + hg, checkBoxHeight, groupBase.GetIdNegative());
	}
	return height;
}

BEGIN_MESSAGE_MAP(MyDialogSymbolFilter, DialogFilter)
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(toolTipsCheckBoxId, OnCheckBoxToolTips)

	ON_BN_CLICKED(includeCheckBoxId, OnSymbolUseInclude)
	ON_BN_CLICKED(excludeCheckBoxId, OnSymbolUseExclude)
	ON_LBN_SELCHANGE(symbolListBoxId, OnSelchangeSymbol)
	ON_EN_CHANGE(symbolEditId, OnSymbolChange)
//	ON_EN_UPDATE(symbolEditId, OnSymbolUpdate)
	ON_BN_CLICKED(findButtonId, OnSymbolFind)
	ON_BN_CLICKED(includeButtonId, OnSymbolInclude)
	ON_BN_CLICKED(excludeButtonId, OnSymbolExclude)
	ON_BN_CLICKED(removeButtonId, OnSymbolRemove)
	ON_BN_CLICKED(clearButtonId, OnSymbolClear)

	ON_BN_CLICKED(firstCharsCheckBoxId, OnSymbolUseFirstChars)
	ON_BN_CLICKED(noFirstCharsCheckBoxId, OnSymbolUseNoFirstChars)
	ON_BN_CLICKED(charsCheckBoxId, OnSymbolUseChars)
	ON_BN_CLICKED(noCharsCheckBoxId, OnSymbolUseNoChars)

	ON_EN_CHANGE(firstCharsEditId, OnSymbolFirstCharsChange)
	ON_EN_CHANGE(noFirstCharsEditId, OnSymbolNoFirstCharsChange)
	ON_EN_CHANGE(charsEditId, OnSymbolCharsChange)
	ON_EN_CHANGE(noCharsEditId, OnSymbolNoCharsChange)

	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)

	ON_MESSAGE(WM_USER + 141, OnListBoxIncludeExclude)
	ON_MESSAGE(WM_USER + 143, OnListBoxFind)
	ON_MESSAGE(WM_USER + 144, OnListBoxRemove)
	ON_MESSAGE(WM_USER + 145, OnListBoxClear)
	ON_MESSAGE(WM_USER + 146, OnListBoxCopy)
	ON_MESSAGE(WM_USER + 147, OnListBoxPaste)
	ON_MESSAGE(WM_USER + 148, OnListBoxUnPaste)
END_MESSAGE_MAP()

LRESULT MyDialogSymbolFilter::OnListBoxIncludeExclude(WPARAM w, LPARAM l)
{
	if(l)
	{
		OnSymbolInclude();
	}
	else
	{
		OnSymbolExclude();
	}
	return 0;
}

LRESULT MyDialogSymbolFilter::OnListBoxFind(WPARAM w, LPARAM l)
{
	OnSymbolFind();
	return 0;
}

LRESULT MyDialogSymbolFilter::OnListBoxRemove(WPARAM w, LPARAM l)
{
	OnSymbolRemove();
	return 0;
}

LRESULT MyDialogSymbolFilter::OnListBoxClear(WPARAM w, LPARAM l)
{
	OnSymbolClear();
	return 0;
}

LRESULT MyDialogSymbolFilter::OnListBoxCopy(WPARAM w, LPARAM l)
{
	OnSymbolCopy();
	return 0;
}

LRESULT MyDialogSymbolFilter::OnListBoxPaste(WPARAM w, LPARAM l)
{
	OnSymbolPaste();
	return 0;
}

LRESULT MyDialogSymbolFilter::OnListBoxUnPaste(WPARAM w, LPARAM l)
{
	OnSymbolUnPaste();
	return 0;
}

void MyDialogSymbolFilter::OnSymbolCopy()
{
//http://www.cplusplus.com/forum/beginner/676/
	std::string info;
	const SymbolCheckedItem* item;
	for(unsigned int i = 0, end = m_ListBoxSymbol.GetItemCount(); i < end; ++i)
	{
		item = (const SymbolCheckedItem*)m_ListBoxSymbol.GetItemDataPtr(i);
		info += (const char*)&item->GetSymbol();
		info += "\r\n";
	}
	std::string errorName;
	std::string errorDescription;
	unsigned char errorCode = U_CopyStringToClipboardAndGetError(info, m_hWnd, &errorName, &errorDescription);
}

void MyDialogSymbolFilter::OnSymbolPaste()
{
	if(IsClipboardFormatAvailable(CF_TEXT))
	{
		if(OpenClipboard()) 
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			if(hData)
			{
				size_t size = GlobalSize(hData);
				if(size)
				{
					char * cursor = (char*)GlobalLock(hData);
					if(cursor)
					{
						bool checked = m_CheckBoxSymbolInclude.GetCheck() == BST_CHECKED || m_CheckBoxSymbolExclude.GetCheck() == BST_UNCHECKED;
						std::string str;
						char c;
						bool pastSymbol = false;
						unsigned int charCount = 0;
						const unsigned int maxCharCount = 7;
						m_ListBoxSymbol.SetRedraw(FALSE);
						for(size_t i = 0; i < size && *cursor; ++i, ++cursor)
						{
							c = *cursor;
							switch(c)
							{
								case '\r':
								break;

								case '\n':
								if(pastSymbol)
								{
									pastSymbol = false;
								}
								else
								{
									if(!str.empty())
									{
										DoAddSymbol(str.c_str(), checked);
										str.clear();
										charCount = 0;
									}
								}
								break;

								case '|':
								case '\t':
								case ' ':
								if(!pastSymbol)
								{
									if(!str.empty())
									{
										DoAddSymbol(str.c_str(), checked);
										str.clear();
										charCount = 0;
									}
									pastSymbol = true;
								}
								break;

								default:
								if(!pastSymbol)
								{
									++charCount;
									if(charCount > maxCharCount)
									{
										charCount = 0;
										pastSymbol = true;
										str.clear();
									}
									else
									{
										str += toupper(c);
									}
								}
								break;
							}
						}
						GlobalUnlock(hData); 
						if(!pastSymbol)
						{
							if(!str.empty())
							{
								DoAddSymbol(str.c_str(), checked);
								str.clear();
								charCount = 0;
							}
							pastSymbol = true;
						}
						m_ListBoxSymbol.SetRedraw(TRUE);
						m_ListBoxSymbol.Invalidate(FALSE);
					}
				}
			}
			CloseClipboard();
		}
	}
}

void MyDialogSymbolFilter::OnSymbolUnPaste()
{
	if(IsClipboardFormatAvailable(CF_TEXT))
	{
		if(OpenClipboard()) 
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			if(hData)
			{
				size_t size = GlobalSize(hData);
				if(size)
				{
					char * cursor = (char*)GlobalLock(hData);
					if(cursor)
					{
//						bool checked = m_CheckBoxSymbolInclude.GetCheck() == BST_CHECKED || m_CheckBoxSymbolExclude.GetCheck() == BST_UNCHECKED;
						std::string str;
						char c;
						bool pastSymbol = false;
						unsigned int charCount = 0;
						const unsigned int maxCharCount = 7;
						m_ListBoxSymbol.SetRedraw(FALSE);
						for(size_t i = 0; i < size && *cursor; ++i, ++cursor)
						{
							c = *cursor;
							switch(c)
							{
								case '\r':
								break;

								case '\n':
								if(pastSymbol)
								{
									pastSymbol = false;
								}
								else
								{
									if(!str.empty())
									{
										DoRemoveSymbol(str.c_str());
										str.clear();
										charCount = 0;
									}
								}
								break;

								case '|':
								case '\t':
								case ' ':
								if(!pastSymbol)
								{
									if(!str.empty())
									{
										DoRemoveSymbol(str.c_str());
										str.clear();
										charCount = 0;
									}
									pastSymbol = true;
								}
								break;

								default:
								if(!pastSymbol)
								{
									++charCount;
									if(charCount > maxCharCount)
									{
										charCount = 0;
										pastSymbol = true;
										str.clear();
									}
									else
									{
										str += toupper(c);
									}
								}
								break;
							}
						}
						GlobalUnlock(hData); 
						if(!pastSymbol)
						{
							if(!str.empty())
							{
								DoRemoveSymbol(str.c_str());
								str.clear();
								charCount = 0;
							}
							pastSymbol = true;
						}
						m_ListBoxSymbol.SetRedraw(TRUE);
						m_ListBoxSymbol.Invalidate(FALSE);
					}
				}
			}
			CloseClipboard();
		}
	}
}

void MyDialogSymbolFilter::OnSymbolFind()
{
	CString symbol;
	m_EditSymbol.GetWindowText(symbol);
	if(!symbol.IsEmpty())
	{
		int sel = m_ListBoxSymbol.FindItemByName(symbol);
		m_ListBoxSymbol.SetCurSel(sel);
		OnSelchangeSymbol();
	}
}

BOOL MyDialogSymbolFilter::PreTranslateMessage(MSG* pMsg)
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
				if(m_EditSymbol.GetWindowTextLength() > 0)
				{
					bool inc = m_CheckBoxSymbolInclude.GetCheck() == BST_CHECKED;
					bool exc = m_CheckBoxSymbolExclude.GetCheck() == BST_CHECKED;
					if(inc == exc)
					{
						AddSymbol(m_lastUsedSymbolInclude);
					}
					else
					{
						AddSymbol(inc);
					}
					return TRUE;
				}
			}
		}
	}
	return DialogFilter::PreTranslateMessage(pMsg);
}

void MyDialogSymbolFilter::DoAddSymbol(const char* symbol, bool checked)
{
	int sel = m_ListBoxSymbol.FindItemByName(symbol);
	if(sel >= 0)
	{
		SymbolCheckedItem* item = (SymbolCheckedItem*)m_ListBoxSymbol.GetItemDataPtr(sel);
		if(item->isChecked() != checked)
		{
			item->SetChecked(checked);
			m_ListBoxSymbol.InvalidateItem(sel, 0, m_ListBoxSymbol.GetMyItemHeight());
			EnableOkButton();
		}
	}
	else
	{
		sel = m_ListBoxSymbol.AddString((const char*)new SymbolCheckedItem(U_StringToUInt64Term0(symbol), checked));
		EnableControl(m_ButtonSymbolClear.m_hWnd, true, m_EditSymbol.m_hWnd);
		EnableOkButton();
	}
	m_ListBoxSymbol.SetCurSel(sel);
	OnSelchangeSymbol();
	m_EditSymbol.SetWindowText("");
	EnableAddSymbolButtons();
	EnableFindButton();
}

void MyDialogSymbolFilter::AddSymbol(bool checked)
{
	CString symbol;
	m_EditSymbol.GetWindowText(symbol);
	if(!symbol.IsEmpty())
	{
		DoAddSymbol(symbol, checked);
	}
}

void MyDialogSymbolFilter::OnSymbolInclude()
{
	m_lastUsedSymbolInclude = true;
	AddSymbol(true);
}

void MyDialogSymbolFilter::OnSymbolExclude()
{
	m_lastUsedSymbolInclude = false;
	AddSymbol(false);
}

void MyDialogSymbolFilter::DoRemoveSymbol(const char* symbol)
{
	int sel = m_ListBoxSymbol.FindItemByName(symbol);
	if(sel >= 0)
	{
		const int curSel = m_ListBoxSymbol.GetCurSel();
		m_ListBoxSymbol.DeleteString(sel);
		if(curSel == sel)
		{
			EnableControl(m_ButtonSymbolRemove.m_hWnd, false, m_EditSymbol.m_hWnd);
		}
		const unsigned int count = m_ListBoxSymbol.GetItemCount();
		if(!count)
		{
			EnableControl(m_ButtonSymbolClear.m_hWnd, false, m_EditSymbol.m_hWnd);
			EnableControl(m_ButtonSymbolFind.m_hWnd, false, m_EditSymbol.m_hWnd);
		}
		EnableOkButton();
	}
}

void MyDialogSymbolFilter::OnSymbolRemove()
{
	const unsigned char ret = m_ListBoxSymbol.DeleteSelectedItemAtAndSelectNext();//0 - not deleted; 1 - deleted the last item; 2 - deleted and there are items left.
	if(ret)
	{
		if(ret == 1)
		{
			EnableControl(m_ButtonSymbolRemove.m_hWnd, false, m_EditSymbol.m_hWnd);
			EnableControl(m_ButtonSymbolClear.m_hWnd, false, m_EditSymbol.m_hWnd);
			EnableControl(m_ButtonSymbolFind.m_hWnd, false, m_EditSymbol.m_hWnd);
		}
		EnableOkButton();
	}
}

void MyDialogSymbolFilter::OnSymbolClear()
{
	if(m_ListBoxSymbol.GetItemCount() > 0)
	{
		m_ListBoxSymbol.ResetContent();
		EnableControl(m_ButtonSymbolRemove.m_hWnd, false, m_EditSymbol.m_hWnd);
		EnableControl(m_ButtonSymbolClear.m_hWnd, false, m_EditSymbol.m_hWnd);
		EnableControl(m_ButtonSymbolFind.m_hWnd, false, m_EditSymbol.m_hWnd);
//		EnableFindButton();
		EnableOkButton();
	}
}

void MyDialogSymbolFilter::EnableFindButton()
{
	EnableControl(m_ButtonSymbolFind.m_hWnd, m_EditSymbol.GetWindowTextLength() > 0 && m_ListBoxSymbol.GetItemCount() > 0, m_EditSymbol.m_hWnd);
}

void MyDialogSymbolFilter::EnableAddSymbolButtons()
{
	bool enable = m_EditSymbol.GetWindowTextLength() > 0;
	EnableControl(m_ButtonSymbolInclude.m_hWnd, enable, m_EditSymbol.m_hWnd);
	EnableControl(m_ButtonSymbolExclude.m_hWnd, enable, m_EditSymbol.m_hWnd);
}

void MyDialogSymbolFilter::OnSymbolFirstCharsChange()
{
	CString str;
	m_EditFirstChars.GetWindowText(str);
	if(strcmp(m_firstChars.c_str(), str))
	{
		bool wasEmpty = m_firstChars.empty();
		m_firstChars = str;
		if(wasEmpty != m_firstChars.empty())
		{
			m_EditFirstChars.Invalidate(FALSE);
		}
		if(strcmp(str, ((const MySymbolFilter*)m_filter)->GetFirstCharString().c_str()))DoEnableOkButton(true);
		else EnableOkButton();

		if(!str.IsEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogSymbolFilter::OnSymbolNoFirstCharsChange()
{
	CString str;
	m_EditNoFirstChars.GetWindowText(str);
	if(strcmp(m_noFirstChars.c_str(), str))
	{
		m_noFirstChars = str;
		if(strcmp(str, ((const MySymbolFilter*)m_filter)->GetNoFirstCharString().c_str()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!str.IsEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogSymbolFilter::OnSymbolCharsChange()
{
	CString str;
	m_EditChars.GetWindowText(str);
	if(strcmp(m_chars.c_str(), str))
	{
		bool wasEmpty = m_chars.empty();
		m_chars = str;
		if(wasEmpty != m_chars.empty())
		{
			m_EditChars.Invalidate(FALSE);
		}
		if(strcmp(str, ((const MySymbolFilter*)m_filter)->GetCharString().c_str()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!str.IsEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogSymbolFilter::OnSymbolNoCharsChange()
{
	CString str;
	m_EditNoChars.GetWindowText(str);
	if(strcmp(m_noChars.c_str(), str))
	{
		m_noChars = str;
		if(strcmp(str, ((const MySymbolFilter*)m_filter)->GetNoCharString().c_str()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!str.IsEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogSymbolFilter::AfterClose()
{
	DialogFilter::AfterClose();
}

void MyDialogSymbolFilter::OnSymbolChange()
{
	EnableAddSymbolButtons();
	EnableFindButton();
}

void MyDialogSymbolFilter::OnSelchangeSymbol()
{
	EnableControl(m_ButtonSymbolRemove.m_hWnd, m_ListBoxSymbol.GetCurSel() >= 0, m_ListBoxSymbol.m_hWnd);
}

LRESULT MyDialogSymbolFilter::OnListBoxChecked(WPARAM w, LPARAM l)
{
	EnableOkButton();
	return 0;
}

void MyDialogSymbolFilter::OnSymbolUseFirstChars()
{
	const bool use = m_CheckBoxFirstChars.GetCheck() == BST_CHECKED;
	if(m_useFirstChars != use)
	{
		m_useFirstChars = use;
		m_EditFirstChars.Invalidate(FALSE);
		if(m_useFirstChars != ((const MySymbolFilter*)m_filter)->isUseFirstCharSet())DoEnableOkButton(true);
		else EnableOkButton();
		if(use)DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogSymbolFilter::OnSymbolUseNoFirstChars()
{
	const bool use = m_CheckBoxNoFirstChars.GetCheck() == BST_CHECKED;
	if(m_useNoFirstChars != use)
	{
		m_useNoFirstChars = use;
		m_EditNoFirstChars.Invalidate(FALSE);
		if(m_useNoFirstChars != ((const MySymbolFilter*)m_filter)->isUseNoFirstCharSet())DoEnableOkButton(true);
		else EnableOkButton();
		if(use)DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogSymbolFilter::OnSymbolUseChars()
{
	const bool use = m_CheckBoxChars.GetCheck() == BST_CHECKED;
	if(m_useChars != use)
	{
		m_useChars = use;
		m_EditChars.Invalidate(FALSE);
		if(m_useChars != ((const MySymbolFilter*)m_filter)->isUseCharSet())DoEnableOkButton(true);
		else EnableOkButton();
		if(use)DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogSymbolFilter::OnSymbolUseNoChars()
{
	const bool use = m_CheckBoxNoChars.GetCheck() == BST_CHECKED;
	if(m_useNoChars != use)
	{
		m_useNoChars = use;
		m_EditNoChars.Invalidate(FALSE);
		if(m_useNoChars != ((const MySymbolFilter*)m_filter)->isUseNoCharSet())DoEnableOkButton(true);
		else EnableOkButton();
		if(use)DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogSymbolFilter::OnCheckBoxToolTips()
{
	const bool checked = m_CheckBoxToolTips.GetCheck() == BST_CHECKED;
	if(m_configBase->m_showTooltips.GetValue())
	{
#ifdef DELETE_FILTER_DIALOGS
		SetToolTipsEnabled(checked);
#else
		takionMainWnd->DoEnableShowToolTipsInFilterDialogs(checked);
#endif
	}
}

void MyDialogSymbolFilter::OnSymbolUseInclude()
{
	const bool checked = m_CheckBoxSymbolInclude.GetCheck() == BST_CHECKED;
	if(checked != ((const MySymbolFilter*)m_filter)->isUseInclude())DoEnableOkButton(true);
	else EnableOkButton();
	if(checked)DoEnableClearButton(true);
	else EnableClearButton();

	if(!m_CheckBoxSymbolExclude.isChecked())
	{
		UpdateListBoxSymbolColor(checked);
	}
}

void MyDialogSymbolFilter::OnSymbolUseExclude()
{
	const bool checked = m_CheckBoxSymbolExclude.GetCheck() == BST_CHECKED;
	if(checked != ((const MySymbolFilter*)m_filter)->isUseExclude())DoEnableOkButton(true);
	else EnableOkButton();
	if(checked)DoEnableClearButton(true);
	else EnableClearButton();

	if(!m_CheckBoxSymbolInclude.isChecked())
	{
		UpdateListBoxSymbolColor(checked);
	}
}

void MyDialogSymbolFilter::UpdateListBoxSymbolColor(bool enable)
{
	if(enable)
	{
		m_ListBoxSymbol.SetBkBrush(nullptr);
		m_ListBoxSymbol.SetNotSelectionBkColor(0xFFFFFFFF);
	}
	else
	{
		m_ListBoxSymbol.SetBkBrush(m_visualBase->GetLightGrayBrush());
		m_ListBoxSymbol.SetNotSelectionBkColor(m_visualBase->GetLightGrayColor());
	}
}

void MyDialogSymbolFilter::ToolTipsEnabled(bool enable)
{
	DialogFilter::ToolTipsEnabled(enable);

	m_CheckBoxSymbolInclude.EnableTooltips(enable, m_toolTip);
	m_CheckBoxSymbolExclude.EnableTooltips(enable, m_toolTip);
	m_EditSymbol.EnableTooltips(enable, m_toolTip);
	m_ListBoxSymbol.EnableTooltips(enable, m_toolTip);

	m_CheckBoxFirstChars.EnableTooltips(enable, m_toolTip);
	m_CheckBoxNoFirstChars.EnableTooltips(enable, m_toolTip);
	m_CheckBoxChars.EnableTooltips(enable, m_toolTip);
	m_CheckBoxNoChars.EnableTooltips(enable, m_toolTip);

	m_EditFirstChars.EnableTooltips(enable, m_toolTip);
	m_EditNoFirstChars.EnableTooltips(enable, m_toolTip);
	m_EditChars.EnableTooltips(enable, m_toolTip);
	m_EditNoChars.EnableTooltips(enable, m_toolTip);
}

void MyDialogSymbolFilter::Display(const TakionFilter& filter, TakionFrame* receipient, unsigned int code, CWnd* parent, int x, int y, int w, int h, bool show)
{
	DialogFilter::Display(filter, receipient, code, parent, x, y, w, h, show);
}

void MyDialogSymbolFilter::DoDataExchangeRange(RangeGroupBase& cg, CDataExchange* pDX)
{
	DDX_Control(pDX, cg.GetIdCheckBox(), cg.m_CheckBox);
	DDX_Control(pDX, cg.GetIdSpinMin(), cg.GetSpinWnd(true));
	DDX_Control(pDX, cg.GetIdSpinMax(), cg.GetSpinWnd(false));
	CWnd* checkBox = cg.GetPositiveNegativeCheckBox(true);
	if(checkBox)
	{
		DDX_Control(pDX, cg.GetIdPositive(), *checkBox);
		DDX_Control(pDX, cg.GetIdNegative(), *cg.GetPositiveNegativeCheckBox(false));
	}

#ifdef FILTER_CUSTOM_GROUP_BOX
	AddCustomGroupBox(&cg.m_GroupBox);
#else
	DDX_Control(pDX, cg.GetIdGroup(), cg.m_GroupBox);
#endif
}

void MyDialogSymbolFilter::DoDataExchange(CDataExchange* pDX)
{
	DialogFilter::DoDataExchange(pDX);

	DDX_Control(pDX, toolTipsCheckBoxId, m_CheckBoxToolTips);

	DDX_Control(pDX, includeCheckBoxId, m_CheckBoxSymbolInclude);
	DDX_Control(pDX, excludeCheckBoxId, m_CheckBoxSymbolExclude);
	DDX_Control(pDX, symbolListBoxId, m_ListBoxSymbol);
	DDX_Control(pDX, symbolEditId, m_EditSymbol);
	DDX_Control(pDX, includeButtonId, m_ButtonSymbolInclude);
	DDX_Control(pDX, excludeButtonId, m_ButtonSymbolExclude);
	DDX_Control(pDX, findButtonId, m_ButtonSymbolFind);
	DDX_Control(pDX, removeButtonId, m_ButtonSymbolRemove);
	DDX_Control(pDX, clearButtonId, m_ButtonSymbolClear);

	DDX_Control(pDX, symbolCharGroupId, m_GroupBoxSymbolChars);
	DDX_Control(pDX, firstCharsCheckBoxId, m_CheckBoxFirstChars);
	DDX_Control(pDX, noFirstCharsCheckBoxId, m_CheckBoxNoFirstChars);
	DDX_Control(pDX, charsCheckBoxId, m_CheckBoxChars);
	DDX_Control(pDX, noCharsCheckBoxId, m_CheckBoxNoChars);

	DDX_Control(pDX, firstCharsEditId, m_EditFirstChars);
	DDX_Control(pDX, noFirstCharsEditId, m_EditNoFirstChars);
	DDX_Control(pDX, charsEditId, m_EditChars);
	DDX_Control(pDX, noCharsEditId, m_EditNoChars);
}

BOOL MyDialogSymbolFilter::OnInitDialog()
{
	m_firstChars.clear();
	m_noFirstChars.clear();
	m_chars.clear();
	m_noChars.clear();

	m_useFirstChars = false;
	m_useNoFirstChars = false;
	m_useChars = false;
	m_useNoChars = false;

	BOOL ret = DialogFilter::OnInitDialog();

	m_EditSymbol.SetLimitText(7);

	m_CheckBoxToolTips.EnableTooltips(true, nullptr);

	return ret;
}

bool MyDialogSymbolFilter::InitializeToolTips()
{
	m_CheckBoxToolTips.SetCheck(m_configBase->m_showTooltips.GetValue() ? BST_CHECKED : BST_UNCHECKED);

	return UpdateGlobalToolTipsEnabled();
}

bool MyDialogSymbolFilter::UpdateGlobalToolTipsEnabled()
{
	const bool enable = m_configBase->m_showTooltips.GetValue();
	m_CheckBoxToolTips.SetTooltipText(enable ?
		"Show ToolTips in Filter Dialogs" :
		"Show ToolTips in Filter Dialogs (Check Box \"Show Tooltips\" in tab \"Main\" should also be checked)");
	return SetToolTipsEnabled(enable);// && m_configBase->m_showTooltips.GetValue());
}

void MyDialogSymbolFilter::SetShowToolTips(bool showToolTips)
{
	if(m_CheckBoxToolTips.m_hWnd)
	{
		m_CheckBoxToolTips.SetCheck(showToolTips ? BST_CHECKED : BST_UNCHECKED);
	}
	if(m_configBase->m_showTooltips.GetValue())
	{
		SetToolTipsEnabled(showToolTips);
	}
}

void MyDialogSymbolFilter::UpdateMyFields(MySymbolFilter& filter) const// , unsigned int doNotCopyFlags);
{
#ifndef TAKION_NO_OPTIONS
	if(!((const MySymbolFilter*)m_filter)->isBaseOptionSorterFilter())
	{
#endif
		filter.SetUseInclude(m_CheckBoxSymbolInclude.GetCheck() == BST_CHECKED);
		filter.SetUseExclude(m_CheckBoxSymbolExclude.GetCheck() == BST_CHECKED);

		filter.SetUseFirstCharSet(m_useFirstChars);
		filter.SetUseNoFirstCharSet(m_useNoFirstChars);
		filter.SetUseCharSet(m_useChars);
		filter.SetUseNoCharSet(m_useNoChars);
		filter.SetFirstCharString(m_firstChars);//std::string(str));
		filter.SetNoFirstCharString(m_noFirstChars);//std::string(str));
		filter.SetCharString(m_chars);//std::string(str));
		filter.SetNoCharString(m_noChars);//std::string(str));

		MySymbolFilter::ExcludeIncludeSymbolMap& excludeIncludeMap = filter.GetExcludeIncludeSymbolMap();
		MySymbolFilter::ExcludeIncludeSymbolMap::iterator sit = excludeIncludeMap.begin();
		MySymbolFilter::ExcludeIncludeSymbolMap::iterator sitend = excludeIncludeMap.end();
		unsigned int i = 0;
		const unsigned int end = m_ListBoxSymbol.GetItemCount();
		const SymbolCheckedItem* item;
		MySymbolFilter::UInt64Set& includeSet = filter.GetIncludeSymbolSet();
		MySymbolFilter::UInt64Set& excludeSet = filter.GetExcludeSymbolSet();

		for(;;)
		{
			if(sit == sitend)
			{
				for(; i < end; ++i)
				{
					item = (const SymbolCheckedItem*)m_ListBoxSymbol.GetItemDataPtr(i);
					const unsigned __int64& numSymbol = item->GetSymbol();
					excludeIncludeMap.insert(MySymbolFilter::ExcludeIncludeSymbolMap::value_type(numSymbol, item->isChecked()));
					if(item->isChecked())
					{
						includeSet.insert(numSymbol);
					}
					else
					{
						excludeSet.insert(numSymbol);
					}
				}
				break;
			}
			else if(i >= end)
			{
				while(sit != sitend)
				{
					if(sit->second)
					{
						includeSet.erase(sit->first);
					}
					else
					{
						excludeSet.erase(sit->first);
					}
					sit = excludeIncludeMap.erase(sit);
				}
				break;
			}
			else
			{
				item = (const SymbolCheckedItem*)m_ListBoxSymbol.GetItemDataPtr(i);
				const unsigned __int64& numSymbol = item->GetSymbol();
				switch(U_CompareUIntAsString(numSymbol, sit->first))
				{
					case -1://numSymbol < sit->first
					sit = excludeIncludeMap.insert(MySymbolFilter::ExcludeIncludeSymbolMap::value_type(numSymbol, item->isChecked())).first;
					if(item->isChecked())
					{
						includeSet.insert(numSymbol);
					}
					else
					{
						excludeSet.insert(numSymbol);
					}
					++sit;
					++i;
					break;

					case 1://numSymbol > sit->first
					if(sit->second)
					{
						includeSet.erase(sit->first);
					}
					else
					{
						excludeSet.erase(sit->first);
					}
					sit = excludeIncludeMap.erase(sit);
					break;

					default://numSymbol = sit->first
					if(item->isChecked() != sit->second)
					{
						if(sit->second)
						{
							includeSet.erase(numSymbol);
							excludeSet.insert(numSymbol);
						}
						else
						{
							excludeSet.erase(numSymbol);
							includeSet.insert(numSymbol);
						}
						sit->second = item->isChecked();
					}
					++sit;
					++i;
					break;
				}
			}
		}
#ifndef TAKION_NO_OPTIONS
	}
#endif
}

void MyDialogSymbolFilter::UpdateFilterValues()
{
	DialogFilter::UpdateFilterValues();

	UpdateMyFields(*(MySymbolFilter*)m_filter);
}

void MyDialogSymbolFilter::MyFieldsToControls(const MySymbolFilter& filter, unsigned int doNotCopyFlags)
{
	if(!(doNotCopyFlags & (1 << FDNC_SYMBOL))
#ifndef TAKION_NO_OPTIONS
		&& !((const MySymbolFilter*)m_filter)->isBaseOptionSorterFilter()
#endif
		)
	{
		m_ListBoxSymbol.ResetContent();
		const MySymbolFilter::UInt64Set& includeSet = filter.GetIncludeSymbolSet();
		MySymbolFilter::UInt64Set::const_iterator it = includeSet.cbegin(), itend = includeSet.cend();
		for(; it != itend; ++it)
		{
			m_ListBoxSymbol.AddString((const char*)new SymbolCheckedItem(*it, true));
		}

		const MySymbolFilter::UInt64Set& excludeSet = filter.GetExcludeSymbolSet();
		it = excludeSet.begin();
		itend = excludeSet.end();
		for(; it != itend; ++it)
		{
			if(m_ListBoxSymbol.FindItemByName((const char*)&*it) < 0)
			{
				m_ListBoxSymbol.AddString((const char*)new SymbolCheckedItem(*it, false));
			}
		}
		EnableControl(m_ButtonSymbolRemove.m_hWnd, m_ListBoxSymbol.GetCurSel() >= 0, m_ListBoxSymbol.m_hWnd);
		EnableControl(m_ButtonSymbolClear.m_hWnd, m_ListBoxSymbol.GetItemCount() > 0, m_ListBoxSymbol.m_hWnd);

		m_CheckBoxSymbolInclude.SetCheck(filter.isUseInclude() ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxSymbolExclude.SetCheck(filter.isUseExclude() ? BST_CHECKED : BST_UNCHECKED);

		UpdateListBoxSymbolColor(m_CheckBoxSymbolInclude.isChecked() || m_CheckBoxSymbolExclude.isChecked());

		m_EditFirstChars.SetWindowText(filter.GetFirstCharString().c_str());
		m_EditNoFirstChars.SetWindowText(filter.GetNoFirstCharString().c_str());
		m_EditChars.SetWindowText(filter.GetCharString().c_str());
		m_EditNoChars.SetWindowText(filter.GetNoCharString().c_str());

		m_useFirstChars = filter.isUseFirstCharSet();
		m_useNoFirstChars = filter.isUseNoFirstCharSet();
		m_useChars = filter.isUseCharSet();
		m_useNoChars = filter.isUseNoCharSet();

		m_CheckBoxFirstChars.SetCheck(m_useFirstChars ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxNoFirstChars.SetCheck(m_useNoFirstChars ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxChars.SetCheck(m_useChars ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxNoChars.SetCheck(m_useNoChars ? BST_CHECKED : BST_UNCHECKED);
	}
}

void MyDialogSymbolFilter::BeforeShow()
{
	DialogFilter::BeforeShow();
	m_EditSymbol.SetWindowText("");
	MyFieldsToControls(*(const MySymbolFilter*)m_filter, 0);
}

void MyDialogSymbolFilter::SetFilterEmpty()
{
	DialogFilter::SetFilterEmpty();
	m_CheckBoxSymbolInclude.SetCheck(BST_UNCHECKED);
	m_CheckBoxSymbolExclude.SetCheck(BST_UNCHECKED);
	
	m_CheckBoxFirstChars.SetCheck(BST_UNCHECKED);
	m_EditFirstChars.SetWindowText("");
	m_CheckBoxNoFirstChars.SetCheck(BST_UNCHECKED);
	m_EditNoFirstChars.SetWindowText("");
	m_CheckBoxChars.SetCheck(BST_UNCHECKED);
	m_EditChars.SetWindowText("");
	m_CheckBoxNoChars.SetCheck(BST_UNCHECKED);
	m_EditNoChars.SetWindowText("");
	m_ListBoxSymbol.ClearItemsWithRedraw();

	UpdateListBoxSymbolColor(false);

	m_useFirstChars = false;
	m_useNoFirstChars = false;
	m_useChars = false;
	m_useNoChars = false;
}

bool MyDialogSymbolFilter::isFilterEmpty() const
{
	return DialogFilter::isFilterEmpty()
		&& m_CheckBoxSymbolInclude.GetCheck() != BST_CHECKED
		&& m_CheckBoxSymbolExclude.GetCheck() != BST_CHECKED
		&& !m_useFirstChars
		&& !m_useNoFirstChars
		&& !m_useChars
		&& !m_useNoChars
		&& m_firstChars.empty()
		&& m_noFirstChars.empty()
		&& m_chars.empty()
		&& m_noChars.empty()
		&& !m_ListBoxSymbol.GetItemCount()
		;
}

bool MyDialogSymbolFilter::MyFieldsIntersectionDifferent(const MySymbolFilter& filter, unsigned int doNotCopyFlags) const
{
	if((doNotCopyFlags & (1 << FDNC_SYMBOL)) != 0)return false;

	if((m_CheckBoxSymbolInclude.GetCheck() == BST_CHECKED) != filter.isUseInclude()
		|| (m_CheckBoxSymbolExclude.GetCheck() == BST_CHECKED) != filter.isUseExclude()
		|| m_useFirstChars != filter.isUseFirstCharSet()
		|| m_useNoFirstChars != filter.isUseNoFirstCharSet()
		|| m_useChars != filter.isUseCharSet()
		|| m_useNoChars != filter.isUseNoCharSet()
		)
	{
		return true;
	}

	if(strcmp(m_firstChars.c_str(), filter.GetFirstCharString().c_str()))
	{
		return true;
	}
	if(strcmp(m_noFirstChars.c_str(), filter.GetNoFirstCharString().c_str()))
	{
		return true;
	}
	if(strcmp(m_chars.c_str(), filter.GetCharString().c_str()))
	{
		return true;
	}
	if(strcmp(m_noChars.c_str(), filter.GetNoCharString().c_str()))
	{
		return true;
	}

	const SymbolCheckedItem* item;
	unsigned int i = 0;
	const unsigned int end = m_ListBoxSymbol.GetItemCount();
	const MySymbolFilter::ExcludeIncludeSymbolMap& symbolMap = filter.GetExcludeIncludeSymbolMap();
	MySymbolFilter::ExcludeIncludeSymbolMap::const_iterator it = symbolMap.cbegin(), itend = symbolMap.cend();
	for(; it != itend && i < end; ++it, ++i)
	{
		item = (const SymbolCheckedItem*)m_ListBoxSymbol.GetItemDataPtr(i);
		if(item->GetSymbol() != it->first || item->isChecked() != it->second)
		{
			return true;
		}
	}
	return it != itend || i < end;
}

bool MyDialogSymbolFilter::isFilterChanged() const
{
	return MyFieldsIntersectionDifferent(*(const MySymbolFilter*)m_filter, 0);// || m_ComboBoxNamedFilter.isModified();
}

bool MyDialogSymbolFilter::isFilterValid() const
{
	return DialogFilter::isFilterValid();
}

void MyDialogSymbolFilter::DoEnableOkButton(bool enable)
{
	DialogFilter::DoEnableOkButton(enable);
}

HBRUSH MyDialogSymbolFilter::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	unsigned char used = 0;
	switch(pWnd->GetDlgCtrlID())
	{
		case toolTipsCheckBoxId:
		if(!m_configBase->m_showTooltips.GetValue())
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(m_visualBase->GetSystemGrayTextColor());
			return *GetBkBrush();
		}
		break;

		case firstCharsEditId:
		used = m_useFirstChars ? m_firstChars.empty() ? 2 : 1 : 4;
		break;

		case charsEditId:
		used = m_useChars ? m_chars.empty() ? 2 : 1 : 4;
		break;

		case noFirstCharsEditId:
		used = m_useNoFirstChars ? 1 : 4;
		break;

		case noCharsEditId:
		used = m_useNoChars ? 1 : 4;
		break;
	}
	if(used)
	{
		switch(used)
		{
			case 4:
			pDC->SetBkMode(TRANSPARENT);
//			pDC->SetTextColor(m_visualBase->GetOkTextColor());
			return *m_visualBase->GetLightGrayBrush();
/*
			case 3:
			pDC->SetBkMode(TRANSPARENT);
			return *m_visualBase->GetNamedFilterIntersectionBrush();
*/
			case 2:
			pDC->SetBkMode(TRANSPARENT);
//			pDC->SetTextColor(m_visualBase->GetOkTextColor());
			return *m_visualBase->GetWarningBrush();//*GetBkBrush();

			default:
			return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		}
	}
	return DialogFilter::OnCtlColor(pDC, pWnd, nCtlColor);
}

////

MyPriceFilter::MyPriceFilter():
	m_priceRange(Price::priceZero, false, false),//
	m_openPriceRange(Price::priceZero),//
	m_dayNetChangeRange(3, Price::priceZero),
	m_dayPriceRangeRange(Price::priceZero),
	m_dayNetChangePercentRange(3, Price::priceZero),
	m_dayNetOpenChangePercentRange(3, Price::priceZero),
	m_dayPriceRangePercentRange(Price::priceZero)
{
}

bool MyPriceFilter::isFiltering() const
{
	return MySymbolFilter::isFiltering()
		|| m_priceRange.isFiltering()
		|| m_openPriceRange.isFiltering()
		|| m_dayNetChangeRange.isFiltering()
		|| m_dayPriceRangeRange.isFiltering()
		|| m_dayNetChangePercentRange.isFiltering()
		|| m_dayNetOpenChangePercentRange.isFiltering()
		|| m_dayPriceRangePercentRange.isFiltering()
		;
}
bool MyPriceFilter::isEmpty() const
{
	return MySymbolFilter::isEmpty()
		&& m_priceRange.isEmpty()
		&& m_openPriceRange.isEmpty()
		&& m_dayNetChangeRange.isEmpty()
		&& m_dayPriceRangeRange.isEmpty()
		&& m_dayNetChangePercentRange.isEmpty()
		&& m_dayNetOpenChangePercentRange.isEmpty()
		&& m_dayPriceRangePercentRange.isEmpty()
		;
}
void MyPriceFilter::Reset()
{
	MySymbolFilter::Reset();
	m_priceRange.Reset();
	m_openPriceRange.Reset();

	m_dayNetChangeRange.Reset();
	m_dayPriceRangeRange.Reset();

	m_dayNetChangePercentRange.Reset();
	m_dayNetOpenChangePercentRange.Reset();
	m_dayPriceRangePercentRange.Reset();
}

bool MyPriceFilter::MyFieldsEqual(const MyPriceFilter& other, unsigned int doNotCopyFlags) const
{
	return m_priceRange == other.m_priceRange
		&& m_openPriceRange == other.m_openPriceRange
		&& m_dayNetChangeRange == other.m_dayNetChangeRange
		&& m_dayPriceRangeRange == other.m_dayPriceRangeRange
		&& m_dayNetChangePercentRange == other.m_dayNetChangePercentRange
		&& m_dayNetOpenChangePercentRange == other.m_dayNetOpenChangePercentRange
		&& m_dayPriceRangePercentRange == other.m_dayPriceRangePercentRange
		;
}

void MyPriceFilter::CopyMyFields(const MyPriceFilter& other, unsigned int doNotCopyFlags)
{
	m_priceRange = other.m_priceRange;
	m_openPriceRange = other.m_openPriceRange;
	m_dayNetChangeRange = other.m_dayNetChangeRange;
	m_dayPriceRangeRange = other.m_dayPriceRangeRange;
	m_dayNetChangePercentRange = other.m_dayNetChangePercentRange;
	m_dayNetOpenChangePercentRange = other.m_dayNetOpenChangePercentRange;
	m_dayPriceRangePercentRange = other.m_dayPriceRangePercentRange;
}

void MyPriceFilter::Copy(const TakionFilter& filter)
{
	MySymbolFilter::Copy(filter);
//	const MyPriceFilter& other = (const MyPriceFilter&)filter;
	CopyMyFields((const MyPriceFilter&)filter, 0);
}

void MyPriceFilter::toString(std::string& str) const
{
	MySymbolFilter::toString(str);
	m_priceRange.toString(str);
	m_dayNetChangeRange.toString(str);
	m_dayPriceRangeRange.toString(str);
	m_dayNetChangePercentRange.toString(str);
	m_dayPriceRangePercentRange.toString(str);
	m_openPriceRange.toString(str);
	m_dayNetOpenChangePercentRange.toString(str);
	str += settingSubdel;
}

void MyPriceFilter::fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier)
{
	MySymbolFilter::fromString(cursor, version, clone, contentModifier);
	if(version >= U_GetVersionFromTokens(1, 0, 0, 115))
	{
		bool endChange;
		char* end = U_MarkSectionEnd((char*)cursor, settingSubdel, endChange);
		m_priceRange.fromString(cursor, version);
		m_dayNetChangeRange.fromString(cursor, version);
		m_dayPriceRangeRange.fromString(cursor, version);
		m_dayNetChangePercentRange.fromString(cursor, version);
		m_dayPriceRangePercentRange.fromString(cursor, version);
		m_openPriceRange.fromString(cursor, version);
		m_dayNetOpenChangePercentRange.fromString(cursor, version);
		if(endChange)*end = settingSubdel;
		cursor = end + 1;
	}
}

bool MyPriceFilter::isFilterSame(const TakionFilter& filter) const
{
	if(MySymbolFilter::isFilterSame(filter))
	{
		const MyPriceFilter& other = (const MyPriceFilter&)filter;
		return m_priceRange.isSameFiltering(other.m_priceRange)
			&& m_openPriceRange.isSameFiltering(other.m_openPriceRange)
			&& m_dayNetChangeRange.isSameFiltering(other.m_dayNetChangeRange)
			&& m_dayPriceRangeRange.isSameFiltering(other.m_dayPriceRangeRange)
			&& m_dayNetChangePercentRange.isSameFiltering(other.m_dayNetChangePercentRange)
			&& m_dayNetOpenChangePercentRange.isSameFiltering(other.m_dayNetOpenChangePercentRange)
			&& m_dayPriceRangePercentRange.isSameFiltering(other.m_dayPriceRangePercentRange)
			;
	}
	return false;
}

unsigned char MyPriceFilter::isPotentialFilterSame(const TakionFilter& filter) const//1 - add; 2 - remove; 3 - both
{
	unsigned char ret = MySymbolFilter::isPotentialFilterSame(filter);

//	const MyPriceFilter& other = (const MyPriceFilter&)filter;
	return ret;
}

bool MyPriceFilter::SecBelongsByValues(const Security* security) const
{
	return m_priceRange.ValueBelongs(Price(security->GetLastNbboPrintOrClosePrice()))
		&& m_openPriceRange.ValueBelongs(security->GetPrimaryQOpenPrice())
		&& m_dayNetChangeRange.ValueBelongs(security->GetNetChange())
		&& m_dayPriceRangeRange.ValueBelongs(security->GetPriceRange())
		&& m_dayNetChangePercentRange.ValueBelongs(security->GetClosePrice().GetPercentChange(security->GetNetChange()))
		&& m_dayNetOpenChangePercentRange.ValueBelongs(security->GetOpenPrice().GetPercentChange(security->GetOpenChange()))
		&& m_dayPriceRangePercentRange.ValueBelongs(security->GetLowPrice().GetPercentChange(security->GetPriceRange()))
		;
}

enum TakionPriceFilterDialogIds
{
	dayNetChangeGroupId = 1040,
	dayNetChangeUseId,
	dayNetChangeSpinMinId,
	dayNetChangeSpinMaxId,
	dayNetChangePositiveId,
	dayNetChangeNegativeId,

	dayPriceRangeGroupId,
	dayPriceRangeUseId,
	dayPriceRangeSpinMinId,
	dayPriceRangeSpinMaxId,

	dayNetChangePercentGroupId,
	dayNetChangePercentUseId,
	dayNetChangePercentSpinMinId,
	dayNetChangePercentSpinMaxId,
	dayNetChangePercentPositiveId,
	dayNetChangePercentNegativeId,

	dayNetOpenChangePercentGroupId,
	dayNetOpenChangePercentUseId,
	dayNetOpenChangePercentSpinMinId,
	dayNetOpenChangePercentSpinMaxId,
	dayNetOpenChangePercentPositiveId,
	dayNetOpenChangePercentNegativeId,

	dayPriceRangePercentGroupId,
	dayPriceRangePercentUseId,
	dayPriceRangePercentSpinMinId,
	dayPriceRangePercentSpinMaxId,

	priceGroupId,
	priceUseId,
	priceSpinMinId,
	priceSpinMaxId,

	openPriceGroupId,
	openPriceUseId,
	openPriceSpinMinId,
	openPriceSpinMaxId,

	priceFilterCount,
};

MyDialogPriceFilter::MyDialogPriceFilter(TakionMainWnd* mainWnd, int x_price, int top, int xSymbolOffset, int xSymbolHeight,
		bool toDestroyOnFocus, TakionFrame* receipient, CWnd* parent, LPCSTR caption, DWORD style, DWORD exstendedStyle, int x, int y, int w, int h,
		LPCSTR font, WORD fontSize, bool executeCommands, bool closeOnCommandExecution):
	MyDialogSymbolFilter(mainWnd, xSymbolOffset, xSymbolHeight,
		toDestroyOnFocus, receipient, parent, caption, style, exstendedStyle, x, y, w, h, font, fontSize, executeCommands, closeOnCommandExecution),
	m_price(priceGroupId, priceUseId, priceSpinMinId, priceSpinMaxId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Last Print NBBO Price (or Yesterday's Close Price, if no Print)"),
	m_openPrice(openPriceGroupId, openPriceUseId, openPriceSpinMinId, openPriceSpinMaxId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Primary Exchange Open Price (can be used for detecting if a stock is open)"),
	m_dayNetChange(dayNetChangeGroupId, dayNetChangeUseId, dayNetChangeSpinMinId, dayNetChangeSpinMaxId, dayNetChangePositiveId, dayNetChangeNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Net Change from Yesterday's Close Price"),
	m_dayPriceRange(dayPriceRangeGroupId, dayPriceRangeUseId, dayPriceRangeSpinMinId, dayPriceRangeSpinMaxId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Today's Price Range (Difference between High Price and Low Price of the Day)"),
	m_dayNetChangePercent(dayNetChangePercentGroupId, dayNetChangePercentUseId, dayNetChangePercentSpinMinId, dayNetChangePercentSpinMaxId, dayNetChangePercentPositiveId, dayNetChangePercentNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Net Change from Yesterday's Close Price as percent of Yesterday's Close Price"),
	m_dayNetOpenChangePercent(dayNetOpenChangePercentGroupId, dayNetOpenChangePercentUseId, dayNetOpenChangePercentSpinMinId, dayNetOpenChangePercentSpinMaxId, dayNetOpenChangePercentPositiveId, dayNetOpenChangePercentNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Net Change from the Open Price as percent of the Open Price"),
	m_dayPriceRangePercent(dayPriceRangePercentGroupId, dayPriceRangePercentUseId, dayPriceRangePercentSpinMinId, dayPriceRangePercentSpinMaxId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Today's Price Range as percent of Today's Low Price")
{
	int y1 = top;
	const int dy = groupHeight + vgap;
//	const int cy = 2 * (checkBoxHeight + vgap);
	const int cy = checkBoxHeight + vgap;
	AddRangeComponent(x_price, y1, groupWidth, "Price", m_price);
	AddRangeComponent(x_price, y1 += dy, groupWidth, "Day Net Change", m_dayNetChange);
	AddRangeComponent(x_price, y1 += dy + cy, groupWidth, "Day Price Range", m_dayPriceRange);

	AddRangeComponent(x_price, y1 += dy, groupWidth, "% Day Net Change", m_dayNetChangePercent);
	AddRangeComponent(x_price, y1 += dy + cy, groupWidth, "% Net Open Change", m_dayNetOpenChangePercent);
	AddRangeComponent(x_price, y1 += dy + cy, groupWidth, "% Day Price Range", m_dayPriceRangePercent);
	AddRangeComponent(x_price, y1 += dy, groupWidth, "PQOpen Price", m_openPrice);

	m_secondColumnBottom = y1 + dy;
}

BEGIN_MESSAGE_MAP(MyDialogPriceFilter, MyDialogSymbolFilter)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(priceUseId, OnUsePrice)
	ON_BN_CLICKED(openPriceUseId, OnUseOpenPrice)

	ON_BN_CLICKED(dayNetChangeUseId, OnUseDayNetChange)
	ON_BN_CLICKED(dayNetChangePositiveId, OnPositiveDayNetChange)
	ON_BN_CLICKED(dayNetChangeNegativeId, OnNegativeDayNetChange)
	ON_BN_CLICKED(dayPriceRangeUseId, OnUseDayPriceRange)

	ON_BN_CLICKED(dayNetChangePercentUseId, OnUseDayNetChangePercent)
	ON_BN_CLICKED(dayNetOpenChangePercentUseId, OnUseDayNetOpenChangePercent)
	ON_BN_CLICKED(dayNetChangePercentPositiveId, OnPositiveDayNetChangePercent)
	ON_BN_CLICKED(dayNetChangePercentNegativeId, OnNegativeDayNetChangePercent)

	ON_BN_CLICKED(dayNetOpenChangePercentPositiveId, OnPositiveDayNetOpenChangePercent)
	ON_BN_CLICKED(dayNetOpenChangePercentNegativeId, OnNegativeDayNetOpenChangePercent)

	ON_BN_CLICKED(dayPriceRangePercentUseId, OnUseDayPriceRangePercent)

	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void MyDialogPriceFilter::OnPositiveDayNetChange()
{
	if(m_dayNetChange.TogglePositive())
	{
		if(m_dayNetChange.isPositiveNegativeModified(((MyPriceFilter*)m_filter)->GetDayNetChangeRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetChange.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogPriceFilter::OnNegativeDayNetChange()
{
	if(m_dayNetChange.ToggleNegative())
	{
		if(m_dayNetChange.isPositiveNegativeModified(((MyPriceFilter*)m_filter)->GetDayNetChangeRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetChange.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogPriceFilter::OnPositiveDayNetChangePercent()
{
	if(m_dayNetChangePercent.TogglePositive())
	{
		if(m_dayNetChangePercent.isPositiveNegativeModified(((MyPriceFilter*)m_filter)->GetDayNetChangePercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetChangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogPriceFilter::OnNegativeDayNetChangePercent()
{
	if(m_dayNetChangePercent.ToggleNegative())
	{
		if(m_dayNetChangePercent.isPositiveNegativeModified(((MyPriceFilter*)m_filter)->GetDayNetChangePercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetChangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogPriceFilter::OnPositiveDayNetOpenChangePercent()
{
	if(m_dayNetOpenChangePercent.TogglePositive())
	{
		if(m_dayNetOpenChangePercent.isPositiveNegativeModified(((MyPriceFilter*)m_filter)->GetDayNetOpenChangePercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetOpenChangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogPriceFilter::OnNegativeDayNetOpenChangePercent()
{
	if(m_dayNetOpenChangePercent.ToggleNegative())
	{
		if(m_dayNetOpenChangePercent.isPositiveNegativeModified(((MyPriceFilter*)m_filter)->GetDayNetOpenChangePercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetOpenChangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogPriceFilter::OnUsePrice()
{
	m_price.UpdateUse();
	if(m_price.isRangeBaseModified(((MyPriceFilter*)m_filter)->GetPriceRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_price.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogPriceFilter::OnUseOpenPrice()
{
	m_openPrice.UpdateUse();
	if(m_openPrice.isRangeBaseModified(((MyPriceFilter*)m_filter)->GetOpenPriceRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_openPrice.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogPriceFilter::OnUseDayNetChange()
{
	m_dayNetChange.UpdateUse();
	if(m_dayNetChange.isRangeBaseModified(((MyPriceFilter*)m_filter)->GetDayNetChangeRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_dayNetChange.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogPriceFilter::OnUseDayPriceRange()
{
	m_dayPriceRange.UpdateUse();
	if(m_dayPriceRange.isRangeBaseModified(((MyPriceFilter*)m_filter)->GetDayPriceRangeRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_dayPriceRange.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogPriceFilter::OnUseDayNetChangePercent()
{
	m_dayNetChangePercent.UpdateUse();
	if(m_dayNetChangePercent.isRangeBaseModified(((MyPriceFilter*)m_filter)->GetDayNetChangePercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_dayNetChangePercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogPriceFilter::OnUseDayNetOpenChangePercent()
{
	m_dayNetOpenChangePercent.UpdateUse();
	if(m_dayNetOpenChangePercent.isRangeBaseModified(((MyPriceFilter*)m_filter)->GetDayNetOpenChangePercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_dayNetOpenChangePercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogPriceFilter::OnUseDayPriceRangePercent()
{
	m_dayPriceRangePercent.UpdateUse();
	if(m_dayPriceRangePercent.isRangeBaseModified(((MyPriceFilter*)m_filter)->GetDayPriceRangePercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_dayPriceRangePercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

LRESULT MyDialogPriceFilter::OnSpinChange(WPARAM w, LPARAM l)
{
//	EnableOkButton();
	switch(w)
	{
		case priceSpinMinId:
		m_price.UpdateRangeValid();
		if(m_price.isValueModified(((MyPriceFilter*)m_filter)->GetPriceRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_price.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case priceSpinMaxId:
		m_price.UpdateRangeValid();
		if(m_price.isValueModified(((MyPriceFilter*)m_filter)->GetPriceRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_price.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case openPriceSpinMinId:
		m_openPrice.UpdateRangeValid();
		if(m_openPrice.isValueModified(((MyPriceFilter*)m_filter)->GetOpenPriceRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_openPrice.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case openPriceSpinMaxId:
		m_openPrice.UpdateRangeValid();
		if(m_openPrice.isValueModified(((MyPriceFilter*)m_filter)->GetOpenPriceRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_openPrice.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayNetChangeSpinMinId:
		m_dayNetChange.UpdateRangeValid();
		if(m_dayNetChange.isValueModified(((MyPriceFilter*)m_filter)->GetDayNetChangeRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetChange.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayNetChangeSpinMaxId:
		m_dayNetChange.UpdateRangeValid();
		if(m_dayNetChange.isValueModified(((MyPriceFilter*)m_filter)->GetDayNetChangeRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetChange.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayPriceRangeSpinMinId:
		m_dayPriceRange.UpdateRangeValid();
		if(m_dayPriceRange.isValueModified(((MyPriceFilter*)m_filter)->GetDayPriceRangeRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayPriceRange.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayPriceRangeSpinMaxId:
		m_dayPriceRange.UpdateRangeValid();
		if(m_dayPriceRange.isValueModified(((MyPriceFilter*)m_filter)->GetDayPriceRangeRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayPriceRange.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayNetChangePercentSpinMinId:
		m_dayNetChangePercent.UpdateRangeValid();
		if(m_dayNetChangePercent.isValueModified(((MyPriceFilter*)m_filter)->GetDayNetChangePercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetChangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayNetChangePercentSpinMaxId:
		m_dayNetChangePercent.UpdateRangeValid();
		if(m_dayNetChangePercent.isValueModified(((MyPriceFilter*)m_filter)->GetDayNetChangePercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetChangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayNetOpenChangePercentSpinMinId:
		m_dayNetOpenChangePercent.UpdateRangeValid();
		if(m_dayNetOpenChangePercent.isValueModified(((MyPriceFilter*)m_filter)->GetDayNetOpenChangePercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetOpenChangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayNetOpenChangePercentSpinMaxId:
		m_dayNetOpenChangePercent.UpdateRangeValid();
		if(m_dayNetOpenChangePercent.isValueModified(((MyPriceFilter*)m_filter)->GetDayNetOpenChangePercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayNetOpenChangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayPriceRangePercentSpinMinId:
		m_dayPriceRangePercent.UpdateRangeValid();
		if(m_dayPriceRangePercent.isValueModified(((MyPriceFilter*)m_filter)->GetDayPriceRangePercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayPriceRangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case dayPriceRangePercentSpinMaxId:
		m_dayPriceRangePercent.UpdateRangeValid();
		if(m_dayPriceRangePercent.isValueModified(((MyPriceFilter*)m_filter)->GetDayPriceRangePercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_dayPriceRangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;
	}

	return 0;
}

void MyDialogPriceFilter::ToolTipsEnabled(bool enable)
{
	MyDialogSymbolFilter::ToolTipsEnabled(enable);
	m_price.EnableToolTips(enable, m_toolTip);
	m_openPrice.EnableToolTips(enable, m_toolTip);
	m_dayNetChange.EnableToolTips(enable, m_toolTip);
	m_dayPriceRange.EnableToolTips(enable, m_toolTip);
	m_dayNetChangePercent.EnableToolTips(enable, m_toolTip);
	m_dayNetOpenChangePercent.EnableToolTips(enable, m_toolTip);
	m_dayPriceRangePercent.EnableToolTips(enable, m_toolTip);
}

void MyDialogPriceFilter::Display(const TakionFilter& filter, TakionFrame* receipient, unsigned int code, CWnd* parent, int x, int y, int w, int h, bool show)
{
	MyDialogSymbolFilter::Display(filter, receipient, code, parent, x, y, w, h, show);
}

void MyDialogPriceFilter::DoDataExchange(CDataExchange* pDX)
{
	MyDialogSymbolFilter::DoDataExchange(pDX);

	DoDataExchangeRange(m_price, pDX);
	DoDataExchangeRange(m_openPrice, pDX);
	DoDataExchangeRange(m_dayNetChange, pDX);
	DoDataExchangeRange(m_dayPriceRange, pDX);
	DoDataExchangeRange(m_dayNetChangePercent, pDX);
	DoDataExchangeRange(m_dayNetOpenChangePercent, pDX);
	DoDataExchangeRange(m_dayPriceRangePercent, pDX);
}

void MyDialogPriceFilter::Resize(bool widthChanged, bool heightChanged)
{
	MyDialogSymbolFilter::Resize(widthChanged, heightChanged);
}


BOOL MyDialogPriceFilter::OnInitDialog()
{
	BOOL ret = MyDialogSymbolFilter::OnInitDialog();
	return ret;
}

void MyDialogPriceFilter::UpdateMyFields(MyPriceFilter& filter) const
{
	m_price.SetRange(filter.GetPriceRange());
	m_openPrice.SetRange(filter.GetOpenPriceRange());
	m_dayNetChange.SetRange(filter.GetDayNetChangeRange());
	m_dayNetChange.SetPositiveNegative(filter.GetDayNetChangeRange());
	m_dayPriceRange.SetRange(filter.GetDayPriceRangeRange());
	m_dayNetChangePercent.SetRange(filter.GetDayNetChangePercentRange());
	m_dayNetChangePercent.SetPositiveNegative(filter.GetDayNetChangePercentRange());
	m_dayNetOpenChangePercent.SetRange(filter.GetDayNetOpenChangePercentRange());
	m_dayNetOpenChangePercent.SetPositiveNegative(filter.GetDayNetOpenChangePercentRange());
	m_dayPriceRangePercent.SetRange(filter.GetDayPriceRangePercentRange());
}

void MyDialogPriceFilter::UpdateFilterValues()
{
	MyDialogSymbolFilter::UpdateFilterValues();
	UpdateMyFields(*(MyPriceFilter*)m_filter);
}

void MyDialogPriceFilter::MyFieldsToControls(const MyPriceFilter& filter, unsigned int doNotCopyFlags)
{
	m_price.SetRangeControls(filter.GetPriceRange());
	m_openPrice.SetRangeControls(filter.GetOpenPriceRange());
	m_dayNetChange.SetRangeControls(filter.GetDayNetChangeRange());
	m_dayNetChange.SetPositiveNegativeControls(filter.GetDayNetChangeRange());
	m_dayPriceRange.SetRangeControls(filter.GetDayPriceRangeRange());
	m_dayNetChangePercent.SetRangeControls(filter.GetDayNetChangePercentRange());
	m_dayNetChangePercent.SetPositiveNegativeControls(filter.GetDayNetChangePercentRange());
	m_dayNetOpenChangePercent.SetRangeControls(filter.GetDayNetOpenChangePercentRange());
	m_dayNetOpenChangePercent.SetPositiveNegativeControls(filter.GetDayNetOpenChangePercentRange());
	m_dayPriceRangePercent.SetRangeControls(filter.GetDayPriceRangePercentRange());
}

void MyDialogPriceFilter::BeforeShow()
{
	MyDialogSymbolFilter::BeforeShow();
	MyFieldsToControls(*(const MyPriceFilter*)m_filter, 0);
}

void MyDialogPriceFilter::SetFilterEmpty()
{
	MyDialogSymbolFilter::SetFilterEmpty();
	m_price.SetRangeEmpty();
	m_openPrice.SetRangeEmpty();
	m_dayNetChange.SetRangeEmpty();
	m_dayPriceRange.SetRangeEmpty();
	m_dayNetChangePercent.SetRangeEmpty();
	m_dayNetOpenChangePercent.SetRangeEmpty();
	m_dayPriceRangePercent.SetRangeEmpty();
}

bool MyDialogPriceFilter::isFilterEmpty() const
{
	return MyDialogSymbolFilter::isFilterEmpty()
		&& m_price.isRangeEmpty()
		&& m_openPrice.isRangeEmpty()
		&& m_dayNetChange.isRangeEmpty()
		&& m_dayPriceRange.isRangeEmpty()
		&& m_dayNetChangePercent.isRangeEmpty()
		&& m_dayNetOpenChangePercent.isRangeEmpty()
		&& m_dayPriceRangePercent.isRangeEmpty()
		;
}

bool MyDialogPriceFilter::MyFieldsIntersectionDifferent(const MyPriceFilter& filter, unsigned int doNotCopyFlags) const
{
	return m_price.isRangeModified(filter.GetPriceRange())
		|| m_openPrice.isRangeModified(filter.GetOpenPriceRange())
		|| m_dayNetChange.isRangeModified(filter.GetDayNetChangeRange())
		|| m_dayNetChange.isPositiveNegativeModified(filter.GetDayNetChangeRange())
		|| m_dayPriceRange.isRangeModified(filter.GetDayPriceRangeRange())

		|| m_dayNetChangePercent.isRangeModified(filter.GetDayNetChangePercentRange())
		|| m_dayNetChangePercent.isPositiveNegativeModified(filter.GetDayNetChangePercentRange())
		|| m_dayNetOpenChangePercent.isRangeModified(filter.GetDayNetOpenChangePercentRange())
		|| m_dayNetOpenChangePercent.isPositiveNegativeModified(filter.GetDayNetOpenChangePercentRange())
		|| m_dayPriceRangePercent.isRangeModified(filter.GetDayPriceRangePercentRange())
		;
}

bool MyDialogPriceFilter::isFilterChanged() const
{
	return MyDialogSymbolFilter::isFilterChanged() || MyFieldsIntersectionDifferent(*(const MyPriceFilter*)m_filter, 0);
}

bool MyDialogPriceFilter::isFilterValid() const
{
	return MyDialogSymbolFilter::isFilterValid()
		&& m_price.isRangeValid()
		&& m_openPrice.isRangeValid()
		&& m_dayNetChange.isRangeValid()
		&& m_dayPriceRange.isRangeValid()
		&& m_dayNetChangePercent.isRangeValid()
		&& m_dayNetOpenChangePercent.isRangeValid()
		&& m_dayPriceRangePercent.isRangeValid()
		;
}

HBRUSH MyDialogPriceFilter::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return MyDialogSymbolFilter::OnCtlColor(pDC, pWnd, nCtlColor);
}


/////////////////////


//#define YEST_VALUES_CHANGE

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
const unsigned int MyDialogStockFilter::securityTypeFilterAll =
	(1 << (unsigned char)(STYPE_UNDEFINED - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_ETF - DialogStockFilter::zeroChar))

	| (1 << (unsigned char)(STYPE_ADR - DialogStockFilter::zeroChar))
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	| (1 << (unsigned char)(STYPE_PREFERRED_STOCK - DialogStockFilter::zeroChar))
#endif
	| (1 << (unsigned char)(STYPE_COMMON_STOCK - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_DEBENTURES - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_ETFE - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_FOREIGN - DialogStockFilter::zeroChar))
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	| (1 << (unsigned char)(STYPE_STRUCTURED_PRODUCT - DialogStockFilter::zeroChar))
#endif
	| (1 << (unsigned char)(STYPE_US_DEPOSITARY_SHARES - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_UNITS - DialogStockFilter::zeroChar))
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	| (1 << (unsigned char)(STYPE_ADR_WARRANT - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_ADR_RIGHT - DialogStockFilter::zeroChar))
#endif
	| (1 << (unsigned char)(STYPE_INDEX_LINKED_NOTES - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_MISC_LIQUID_TRUST - DialogStockFilter::zeroChar))
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	| (1 << (unsigned char)(STYPE_GLOBAL_REGISTERED_SHARE - DialogStockFilter::zeroChar))
#endif
	| (1 << (unsigned char)(STYPE_ORDINARY_SHARES - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_PREF_STOCK - DialogStockFilter::zeroChar))
#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	| (1 << (unsigned char)(STYPE_BASKET - DialogStockFilter::zeroChar))
#endif
	| (1 << (unsigned char)(STYPE_RIGHTS - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_SHARES_OF_BENEFICIARY_INTEREST - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_TEST - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_CLOSED_END_FUND - DialogStockFilter::zeroChar))
	| (1 << (unsigned char)(STYPE_WARRANT - DialogStockFilter::zeroChar));

//const unsigned int MyDialogStockFilter::nasdaqQuoteConditionFilterAll = TU_GetNasdaqQcFilterAll();
//const unsigned int MyDialogStockFilter::listedQuoteConditionFilterAll = TU_GetListedQcFilterAll();
*/
MyStockFilter::MyStockFilter():
	m_marketCategoryFilter((1 << MCF_COUNT) - 1),
	m_testStockFilter(3),
	m_securityTypeFilter(DialogStockFilter::securityTypeFilterAll),
	m_htbFilter(HTBF_All),
	m_tradingStateFilter((1 << TSF_Count) - 1),
	m_circuitBreakerFilter((1 << CBF_Count) - 1),

	m_nasdaqQuoteConditionFilter(DialogStockFilter::nasdaqQuoteConditionFilterAll),
	m_listedQuoteConditionFilter(DialogStockFilter::listedQuoteConditionFilterAll),

	m_yesterdaysVolumeRange(0),
	m_todaysVolumeRange(0),
	m_todaysPxMhVolumeRange(0),

	m_avgDailyVolumeRange(0),
	m_relativeVolumeRange(Money::moneyZero),
	m_preMhVolRelRange(Money::moneyZero),
	m_yesterdaysPriceRangeRange(Price::priceZero),
	m_yesterdaysPriceRangePercentRange(Price::priceZero),
	m_imbalanceNyseRange(TRPN_ALL, 0),
	m_imbalanceNyseRegRange(TRPN_ALL, 0),
	m_imbalanceNsdqRange(TRPN_ALL, 0),
	m_imbalanceArcaRange(TRPN_ALL, 0),
	m_imbalanceExchRange(TRPN_ALL, 0),
	m_rateImbalanceRange(TRPN_ALL, Price::priceZero),
	m_imbNyDeltaInfRange(TRPN_ALL, Price::priceZero),
	m_imbExFarPriceDeltaRange(TRPN_ALL, Price::priceZero),
	m_imbNqFarPriceDeltaRange(TRPN_ALL, Price::priceZero),
	m_imbNqThruRange(TRPN_ALL, Price::priceZero),
	m_imbExValueRange(TRPN_ALL, Money::moneyZero),
	m_imbExNearRange(Price::priceZero),
	m_imbExFarRange(Price::priceZero),
	m_imbExMatchRange(0),
	m_imbExVolPercentRange(Money::moneyZero),
	m_imbExYestVolPercentRange(Money::moneyZero),
	m_deltaPvRange(Price::priceZero),
//	m_rsiMhRange(Price::priceZero, false, true, false),
//	m_rsiDaysRange(Price::priceZero, false, true, false),
	m_borrowPriceRange(Price::priceZero),
	m_pmiSpreadRange(Price::priceZero),
	m_todaysClosePriceRange(Price::priceZero),

	m_bidNetRange(TRPN_ALL, Price::priceZero),
	m_askNetRange(TRPN_ALL, Price::priceZero),

	m_tBidNetRange(TRPN_ALL, Price::priceZero),
	m_tAskNetRange(TRPN_ALL, Price::priceZero),

	m_spreadRange(TRPN_ALL, Price::priceZero),

	m_bidNetPercentRange(TRPN_ALL, Price::priceZero),
	m_askNetPercentRange(TRPN_ALL, Price::priceZero),

	m_tBidNetPercentRange(TRPN_ALL, Price::priceZero),
	m_tAskNetPercentRange(TRPN_ALL, Price::priceZero),

	m_luldRange(TRPN_ALL, SignedPrice::signedPriceZero),
	m_luldPercentRange(TRPN_ALL, Money::moneyZero)
/*
//RSI Minutes
	m_rsiMhPriorPointCount(250),
	m_rsiMhPeriodLength(1),
	m_rsiMhPeriodCount(14),
	m_rsiMhStartMinute(570),
	m_rsiMhCalculate(false),
	m_rsiMhTodayOnly(false),
	m_rsiMhPostMarket(false),
//RSI Days
	m_rsiDaysPriorPointCount(250),
	m_rsiDaysPeriodLength(1),
	m_rsiDaysPeriodCount(14),
	m_rsiDaysStartMinute(570),
	m_rsiDaysCalculate(false),
	m_rsiDaysPostMarket(false)
*/
{
}

bool MyStockFilter::MyFieldsEqual(const MyStockFilter& other, unsigned int doNotCopyFlags) const
{
	return m_yesterdaysVolumeRange == other.m_yesterdaysVolumeRange
		&& m_todaysVolumeRange == other.m_todaysVolumeRange

		&& m_avgDailyVolumeRange == other.m_avgDailyVolumeRange
		&& m_relativeVolumeRange == other.m_relativeVolumeRange
		&& m_preMhVolRelRange == other.m_preMhVolRelRange

		&& m_yesterdaysPriceRangeRange == other.m_yesterdaysPriceRangeRange
		&& m_yesterdaysPriceRangePercentRange == other.m_yesterdaysPriceRangePercentRange

		&& m_marketCategoryFilter == other.m_marketCategoryFilter
		&& m_imbalanceNyseRange == other.m_imbalanceNyseRange
		&& m_imbalanceNyseRegRange == other.m_imbalanceNyseRegRange
		&& m_imbalanceNsdqRange == other.m_imbalanceNsdqRange
		&& m_imbalanceArcaRange == other.m_imbalanceArcaRange
		&& m_imbalanceExchRange == other.m_imbalanceExchRange
		&& m_testStockFilter == other.m_testStockFilter
		&& m_securityTypeFilter == other.m_securityTypeFilter
		&& m_todaysPxMhVolumeRange == other.m_todaysPxMhVolumeRange
		&& m_rateImbalanceRange == other.m_rateImbalanceRange
		&& m_imbNyDeltaInfRange == other.m_imbNyDeltaInfRange
		&& m_imbExFarPriceDeltaRange == other.m_imbExFarPriceDeltaRange
		&& m_imbNqFarPriceDeltaRange == other.m_imbNqFarPriceDeltaRange
		&& m_imbNqThruRange == other.m_imbNqThruRange
		&& m_imbExValueRange == other.m_imbExValueRange
		&& m_imbExNearRange == other.m_imbExNearRange
		&& m_imbExFarRange == other.m_imbExFarRange
		&& m_imbExMatchRange == other.m_imbExMatchRange
		&& m_imbExVolPercentRange == other.m_imbExVolPercentRange
		&& m_imbExYestVolPercentRange == other.m_imbExYestVolPercentRange
		&& m_deltaPvRange == other.m_deltaPvRange
//		&& m_rsiMhRange == other.m_rsiMhRange
//		&& m_rsiDaysRange == other.m_rsiDaysRange
		&& m_borrowPriceRange == other.m_borrowPriceRange
		&& m_pmiSpreadRange == other.m_pmiSpreadRange
		&& m_todaysClosePriceRange == other.m_todaysClosePriceRange
		&& m_htbFilter == other.m_htbFilter
		&& m_tradingStateFilter == other.m_tradingStateFilter
		&& m_circuitBreakerFilter == other.m_circuitBreakerFilter
		&& m_nasdaqQuoteConditionFilter == other.m_nasdaqQuoteConditionFilter
		&& m_listedQuoteConditionFilter == other.m_listedQuoteConditionFilter

		&& m_bidNetRange == other.m_bidNetRange
		&& m_askNetRange == other.m_askNetRange

		&& m_tBidNetRange == other.m_tBidNetRange
		&& m_tAskNetRange == other.m_tAskNetRange

		&& m_spreadRange == other.m_spreadRange

		&& m_bidNetPercentRange == other.m_bidNetPercentRange
		&& m_askNetPercentRange == other.m_askNetPercentRange

		&& m_tBidNetPercentRange == other.m_tBidNetPercentRange
		&& m_tAskNetPercentRange == other.m_tAskNetPercentRange

		&& m_luldRange == other.m_luldRange
		&& m_luldPercentRange == other.m_luldPercentRange
/*
//RSI Minutes
		&& m_rsiMhCalculate == other.m_rsiMhCalculate
		&& m_rsiMhPriorPointCount == other.m_rsiMhPriorPointCount
		&& m_rsiMhPeriodLength == other.m_rsiMhPeriodLength
		&& m_rsiMhPeriodCount == other.m_rsiMhPeriodCount
		&& m_rsiMhStartMinute == other.m_rsiMhStartMinute
		&& m_rsiMhTodayOnly == other.m_rsiMhTodayOnly
		&& m_rsiMhPostMarket == other.m_rsiMhPostMarket
//RSI Days
		&& m_rsiDaysCalculate == other.m_rsiDaysCalculate
		&& m_rsiDaysPriorPointCount == other.m_rsiDaysPriorPointCount
		&& m_rsiDaysPeriodLength == other.m_rsiDaysPeriodLength
		&& m_rsiDaysPeriodCount == other.m_rsiDaysPeriodCount
		&& m_rsiDaysStartMinute == other.m_rsiDaysStartMinute
		&& m_rsiDaysPostMarket == other.m_rsiDaysPostMarket
*/
		;
}

void MyStockFilter::CopyMyFields(const MyStockFilter& other, unsigned int doNotCopyFlags)
{
	m_marketCategoryFilter = other.m_marketCategoryFilter;
	m_testStockFilter = other.m_testStockFilter;
	m_securityTypeFilter = other.m_securityTypeFilter;
	m_htbFilter = other.m_htbFilter;
	m_tradingStateFilter = other.m_tradingStateFilter;
	m_circuitBreakerFilter = other.m_circuitBreakerFilter;
	m_nasdaqQuoteConditionFilter = other.m_nasdaqQuoteConditionFilter;
	m_listedQuoteConditionFilter = other.m_listedQuoteConditionFilter;
	m_yesterdaysVolumeRange = other.m_yesterdaysVolumeRange;
	m_todaysVolumeRange = other.m_todaysVolumeRange;
	m_todaysPxMhVolumeRange = other.m_todaysPxMhVolumeRange;

	m_avgDailyVolumeRange = other.m_avgDailyVolumeRange;
	m_relativeVolumeRange = other.m_relativeVolumeRange;
	m_preMhVolRelRange = other.m_preMhVolRelRange;
	m_yesterdaysPriceRangeRange = other.m_yesterdaysPriceRangeRange;
	m_yesterdaysPriceRangePercentRange = other.m_yesterdaysPriceRangePercentRange;

	m_imbalanceNyseRange = other.m_imbalanceNyseRange;
	m_imbalanceNyseRegRange = other.m_imbalanceNyseRegRange;
	m_imbalanceNsdqRange = other.m_imbalanceNsdqRange;
	m_imbalanceArcaRange = other.m_imbalanceArcaRange;
	m_imbalanceExchRange = other.m_imbalanceExchRange;
	m_rateImbalanceRange = other.m_rateImbalanceRange;
	m_imbNyDeltaInfRange = other.m_imbNyDeltaInfRange;
	m_imbExFarPriceDeltaRange = other.m_imbExFarPriceDeltaRange;
	m_imbNqFarPriceDeltaRange = other.m_imbNqFarPriceDeltaRange;
	m_imbNqThruRange = other.m_imbNqThruRange;
	m_imbExValueRange = other.m_imbExValueRange;
	m_imbExNearRange = other.m_imbExNearRange;
	m_imbExFarRange = other.m_imbExFarRange;
	m_imbExMatchRange = other.m_imbExMatchRange;
	m_imbExVolPercentRange = other.m_imbExVolPercentRange;
	m_imbExYestVolPercentRange = other.m_imbExYestVolPercentRange;
	m_deltaPvRange = other.m_deltaPvRange;
//	m_rsiMhRange = other.m_rsiMhRange;
//	m_rsiDaysRange = other.m_rsiDaysRange;
	m_borrowPriceRange = other.m_borrowPriceRange;
	m_pmiSpreadRange = other.m_pmiSpreadRange;
	m_todaysClosePriceRange = other.m_todaysClosePriceRange;

	m_bidNetRange = other.m_bidNetRange;
	m_askNetRange = other.m_askNetRange;

	m_tBidNetRange = other.m_tBidNetRange;
	m_tAskNetRange = other.m_tAskNetRange;

	m_spreadRange = other.m_spreadRange;

	m_bidNetPercentRange = other.m_bidNetPercentRange;
	m_askNetPercentRange = other.m_askNetPercentRange;

	m_tBidNetPercentRange = other.m_tBidNetPercentRange;
	m_tAskNetPercentRange = other.m_tAskNetPercentRange;

	m_luldRange = other.m_luldRange;
	m_luldPercentRange = other.m_luldPercentRange;
/*
//RSI Minutes
	m_rsiMhPriorPointCount = other.m_rsiMhPriorPointCount;
	m_rsiMhPeriodLength = other.m_rsiMhPeriodLength;
	m_rsiMhPeriodCount = other.m_rsiMhPeriodCount;
	if(m_rsiMhPeriodCount < 2)m_rsiMhPeriodCount = 2;
	m_rsiMhStartMinute = other.m_rsiMhStartMinute;
	m_rsiMhCalculate = other.m_rsiMhCalculate;
	m_rsiMhTodayOnly = other.m_rsiMhTodayOnly;
	m_rsiMhPostMarket = other.m_rsiMhPostMarket;
//RSI Days
	m_rsiDaysPriorPointCount = other.m_rsiDaysPriorPointCount;
	m_rsiDaysPeriodLength = other.m_rsiDaysPeriodLength;
	m_rsiDaysPeriodCount = other.m_rsiDaysPeriodCount;
	if(m_rsiDaysPeriodCount < 2)m_rsiDaysPeriodCount = 2;
	m_rsiDaysStartMinute = other.m_rsiDaysStartMinute;
	m_rsiDaysCalculate = other.m_rsiDaysCalculate;
	m_rsiDaysPostMarket = other.m_rsiDaysPostMarket;
*/
}

void MyStockFilter::Copy(const TakionFilter& filter)
{
	MyPriceFilter::Copy(filter);
	CopyMyFields((const MyStockFilter&)filter, 0);
}

TakionFilter* MyStockFilter::CreateFilterCopy() const
{
	return new MyStockFilter(*this);
}

void MyStockFilter::toString(std::string& str) const
{
	MyPriceFilter::toString(str);
	char num[33];
	m_yesterdaysVolumeRange.toString(str);
	str += U_UnsignedNumberToStrNoDecDigits(m_marketCategoryFilter, num, sizeof(num), settingDel);//U_AppendUInt(str, m_marketCategoryFilter, settingDel);
	m_imbalanceNyseRange.toString(str);
	m_imbalanceNsdqRange.toString(str);
	m_imbalanceArcaRange.toString(str);
	m_imbalanceNyseRegRange.toString(str);
	str += U_UnsignedNumberToStrNoDecDigits(m_testStockFilter, num, sizeof(num), settingDel);//U_AppendUInt(str, m_testStockFilter, settingDel);
	m_todaysVolumeRange.toString(str);
	str += U_UnsignedNumberToStrNoDecDigits(m_securityTypeFilter, num, sizeof(num), settingDel);//U_AppendUInt(str, m_securityTypeFilter, settingDel);
	m_todaysPxMhVolumeRange.toString(str);
	m_rateImbalanceRange.toString(str);
	m_deltaPvRange.toString(str);
	m_borrowPriceRange.toString(str);
	str += U_UnsignedNumberToStrNoDecDigits(m_htbFilter, num, sizeof(num), settingDel);//, unsigned char decDigits = 0, char del = '\0');

	m_avgDailyVolumeRange.toString(str);
	m_yesterdaysPriceRangeRange.toString(str);

	m_pmiSpreadRange.toString(str);

	str += U_UnsignedNumberToStrNoDecDigits(m_tradingStateFilter, num, sizeof(num), settingDel);
	str += U_UnsignedNumberToStrNoDecDigits(m_nasdaqQuoteConditionFilter, num, sizeof(num), settingDel);
	str += U_UnsignedNumberToStrNoDecDigits(m_listedQuoteConditionFilter, num, sizeof(num), settingDel);

	m_bidNetPercentRange.toString(str);
	m_askNetPercentRange.toString(str);
/*
//RSI Minutes
	U_AppendUShort(str, m_rsiMhPeriodLength, settingDel);
	U_AppendUShort(str, m_rsiMhPeriodCount, settingDel);
	U_AppendBool(str, m_rsiMhPostMarket, settingDel);
	m_rsiMhRange.toString(str);
	U_AppendUShort(str, m_rsiMhStartMinute, settingDel);
	U_AppendBool(str, m_rsiMhCalculate, settingDel);
	U_AppendUShort(str, m_rsiMhPriorPointCount, settingDel);
//RSI Days
	U_AppendUShort(str, m_rsiDaysPeriodLength, settingDel);
	U_AppendUShort(str, m_rsiDaysPeriodCount, settingDel);
	U_AppendBool(str, m_rsiDaysPostMarket, settingDel);

	m_rsiDaysRange.toString(str);

	U_AppendUShort(str, m_rsiDaysStartMinute, settingDel);
	U_AppendBool(str, m_rsiDaysCalculate, settingDel);
	U_AppendUShort(str, m_rsiDaysPriorPointCount, settingDel);

	U_AppendBool(str, m_rsiMhTodayOnly, settingDel);
*/
	m_relativeVolumeRange.toString(str);
	m_imbNyDeltaInfRange.toString(str);

	m_luldRange.toString(str);
	m_luldPercentRange.toString(str);

	m_todaysClosePriceRange.toString(str);

	m_imbExFarPriceDeltaRange.toString(str);

	m_imbExNearRange.toString(str);
	m_imbExFarRange.toString(str);

	m_preMhVolRelRange.toString(str);

	m_imbalanceExchRange.toString(str);

	m_imbExMatchRange.toString(str);

	m_imbNqFarPriceDeltaRange.toString(str);

	m_imbExVolPercentRange.toString(str);

	m_yesterdaysPriceRangePercentRange.toString(str);

	m_tBidNetPercentRange.toString(str);
	m_tAskNetPercentRange.toString(str);

	m_imbNqThruRange.toString(str);

	str += U_UnsignedNumberToStrNoDecDigits(m_circuitBreakerFilter, num, sizeof(num), settingDel);

	m_imbExYestVolPercentRange.toString(str);

	m_imbExValueRange.toString(str);

	m_bidNetRange.toString(str);
	m_askNetRange.toString(str);
	m_tBidNetRange.toString(str);
	m_tAskNetRange.toString(str);

	m_spreadRange.toString(str);

	str += settingSubdel;
}

void MyStockFilter::fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier)
{
	MyPriceFilter::fromString(cursor, version, clone, contentModifier);
	bool endChange;
	char* end = U_MarkSectionEnd((char*)cursor, settingSubdel, endChange);

	m_yesterdaysVolumeRange.fromString(cursor, version);
	m_marketCategoryFilter = U_ReadUIntToken(cursor, settingDel, (1 << MCF_COUNT) - 1);
	if(version < U_GetVersionFromTokens(1, 0, 0, 159))
	{
		if(m_marketCategoryFilter == (1 << MCF_BATS) - 1)m_marketCategoryFilter = (1 << MCF_COUNT) - 1;
	}
	if(version < U_GetVersionFromTokens(1, 0, 1, 164))
	{
		if(m_marketCategoryFilter == (1 << MCF_OTCB) - 1)m_marketCategoryFilter = (1 << MCF_COUNT) - 1;
	}
	if(version < U_GetVersionFromTokens(1, 0, 4, 30))
	{
		if(m_marketCategoryFilter == (1 << MCF_IEX) - 1)m_marketCategoryFilter = (1 << MCF_COUNT) - 1;
	}
	m_imbalanceNyseRange.fromString(cursor, version);
	m_imbalanceNsdqRange.fromString(cursor, version);
	m_imbalanceArcaRange.fromString(cursor, version);
	m_imbalanceNyseRegRange.fromString(cursor, version);
	m_testStockFilter = U_ReadUIntToken(cursor, settingDel, 3);
	m_todaysVolumeRange.fromString(cursor, version);
	m_securityTypeFilter = U_ReadUIntToken(cursor, settingDel, DialogStockFilter::securityTypeFilterAll);
	m_securityTypeFilter &= DialogStockFilter::securityTypeFilterAll;

	m_todaysPxMhVolumeRange.fromString(cursor, version);

	m_rateImbalanceRange.fromString(cursor, version);
	m_deltaPvRange.fromString(cursor, version);

	m_borrowPriceRange.fromString(cursor, version);
	m_htbFilter = U_ReadUIntToken(cursor, settingDel, HTBF_All);

	m_avgDailyVolumeRange.fromString(cursor, version);
	m_yesterdaysPriceRangeRange.fromString(cursor, version);

	m_pmiSpreadRange.fromString(cursor, version);

	m_tradingStateFilter = U_ReadUIntToken(cursor, settingDel, (1 << TSF_Count) - 1);
	if(version < U_GetVersionFromTokens(1, 0, 2, 141))
	{
		if(m_tradingStateFilter == (1 << TSF_OTHER) - 1)
		{
			m_tradingStateFilter = (1 << TSF_Count) - 1;
		}
		else
		{
			if(m_tradingStateFilter & (1 << TSF_P))
			{
				m_tradingStateFilter &= ~(1 << TSF_P);
				m_tradingStateFilter |= 1 << TSF_OTHER;
			}
		}
	}
	m_tradingStateFilter &= (1 << TSF_Count) - 1;

	m_nasdaqQuoteConditionFilter = U_ReadUIntToken(cursor, settingDel, DialogStockFilter::nasdaqQuoteConditionFilterAll);
	m_nasdaqQuoteConditionFilter &= DialogStockFilter::nasdaqQuoteConditionFilterAll;

	m_listedQuoteConditionFilter = U_ReadUIntToken(cursor, settingDel, DialogStockFilter::listedQuoteConditionFilterAll);
	m_listedQuoteConditionFilter &= DialogStockFilter::listedQuoteConditionFilterAll;

	m_bidNetPercentRange.fromString(cursor, version);
	m_askNetPercentRange.fromString(cursor, version);
/*
//RSI Minutes
	m_rsiMhPeriodLength = U_ReadUShortToken(cursor, settingDel, 1);
	m_rsiMhPeriodCount = U_ReadUShortToken(cursor, settingDel, 14);
	if(m_rsiMhPeriodCount < 2)m_rsiMhPeriodCount = 2;
	m_rsiMhPostMarket = U_ReadBoolToken(cursor, settingDel);
	m_rsiMhRange.fromString(cursor);
	m_rsiMhStartMinute = U_ReadUShortToken(cursor, settingDel, 570);
	m_rsiMhCalculate = U_ReadBoolToken(cursor, settingDel);
	m_rsiMhPriorPointCount = U_ReadUShortToken(cursor, settingDel, 250);

//RSI Days
	m_rsiDaysPeriodLength = U_ReadUShortToken(cursor, settingDel, 1);
	m_rsiDaysPeriodCount = U_ReadUShortToken(cursor, settingDel, 14);
	if(m_rsiDaysPeriodCount < 2)m_rsiDaysPeriodCount = 2;
	m_rsiDaysPostMarket = U_ReadBoolToken(cursor, settingDel);
	m_rsiDaysRange.fromString(cursor);
	m_rsiDaysStartMinute = U_ReadUShortToken(cursor, settingDel, 570);
	m_rsiDaysCalculate = U_ReadBoolToken(cursor, settingDel);
	m_rsiDaysPriorPointCount = U_ReadUShortToken(cursor, settingDel, 250);

	m_rsiMhTodayOnly = U_ReadBoolToken(cursor, settingDel);
*/
	m_relativeVolumeRange.fromString(cursor, version);
	m_imbNyDeltaInfRange.fromString(cursor, version);

	m_luldRange.fromString(cursor, version);
	m_luldPercentRange.fromString(cursor, version);

	m_todaysClosePriceRange.fromString(cursor, version);

	m_imbExFarPriceDeltaRange.fromString(cursor, version);

	m_imbExNearRange.fromString(cursor, version);
	m_imbExFarRange.fromString(cursor, version);

	m_preMhVolRelRange.fromString(cursor, version);

	m_imbalanceExchRange.fromString(cursor, version);

	m_imbExMatchRange.fromString(cursor, version);

	m_imbNqFarPriceDeltaRange.fromString(cursor, version);

	m_imbExVolPercentRange.fromString(cursor, version);

	m_yesterdaysPriceRangePercentRange.fromString(cursor, version);

	m_tBidNetPercentRange.fromString(cursor, version);
	m_tAskNetPercentRange.fromString(cursor, version);

	m_imbNqThruRange.fromString(cursor, version);

	m_circuitBreakerFilter = U_ReadUIntToken(cursor, settingDel, (1 << CBF_Count) - 1);
	m_circuitBreakerFilter &= (1 << CBF_Count) - 1;

	m_imbExYestVolPercentRange.fromString(cursor, version);

	m_imbExValueRange.fromString(cursor, version);

	m_bidNetRange.fromString(cursor, version);
	m_askNetRange.fromString(cursor, version);
	m_tBidNetRange.fromString(cursor, version);
	m_tAskNetRange.fromString(cursor, version);

	m_spreadRange.fromString(cursor, version);

	if(endChange)*end = settingSubdel;
	cursor = end + 1;
}

bool MyStockFilter::isFiltering() const
{
	return MyPriceFilter::isFiltering()
		|| m_yesterdaysVolumeRange.isFiltering()
		|| m_todaysVolumeRange.isFiltering()
		|| m_todaysPxMhVolumeRange.isFiltering()
		|| m_avgDailyVolumeRange.isFiltering()
		|| m_relativeVolumeRange.isFiltering()
		|| m_preMhVolRelRange.isFiltering()
		|| m_yesterdaysPriceRangeRange.isFiltering()
		|| m_yesterdaysPriceRangePercentRange.isFiltering()

		|| m_imbalanceNyseRange.isFiltering()
		|| m_imbalanceNyseRegRange.isFiltering()
		|| m_imbalanceNsdqRange.isFiltering()
		|| m_imbalanceArcaRange.isFiltering()
		|| m_imbalanceExchRange.isFiltering()
		|| m_rateImbalanceRange.isFiltering()
		|| m_imbNyDeltaInfRange.isFiltering()
		|| m_imbExFarPriceDeltaRange.isFiltering()
		|| m_imbNqFarPriceDeltaRange.isFiltering()
		|| m_imbNqThruRange.isFiltering()
		|| m_imbExValueRange.isFiltering()

		|| m_imbExNearRange.isFiltering()
		|| m_imbExFarRange.isFiltering()
		|| m_imbExMatchRange.isFiltering()
		|| m_imbExVolPercentRange.isFiltering()
		|| m_imbExYestVolPercentRange.isFiltering()
		|| m_deltaPvRange.isFiltering()
//		|| m_rsiMhRange.isFiltering()
//		|| m_rsiDaysRange.isFiltering()
		|| m_borrowPriceRange.isFiltering()
		|| m_pmiSpreadRange.isFiltering()
		|| m_todaysClosePriceRange.isFiltering()

		|| m_bidNetRange.isFiltering()
		|| m_askNetRange.isFiltering()

		|| m_tBidNetRange.isFiltering()
		|| m_tAskNetRange.isFiltering()

		|| m_spreadRange.isFiltering()

		|| m_bidNetPercentRange.isFiltering()
		|| m_askNetPercentRange.isFiltering()

		|| m_tBidNetPercentRange.isFiltering()
		|| m_tAskNetPercentRange.isFiltering()

		|| m_luldRange.isFiltering()
		|| m_luldPercentRange.isFiltering()

		|| m_marketCategoryFilter != (1 << MCF_COUNT) - 1
		|| m_testStockFilter != 3//real + test
		|| m_securityTypeFilter != DialogStockFilter::securityTypeFilterAll
		|| m_htbFilter != HTBF_All
		|| m_tradingStateFilter != (1 << TSF_Count) - 1
		|| m_circuitBreakerFilter != (1 << CBF_Count) - 1
		|| m_nasdaqQuoteConditionFilter != DialogStockFilter::nasdaqQuoteConditionFilterAll
		|| m_listedQuoteConditionFilter != DialogStockFilter::listedQuoteConditionFilterAll

		;
}
bool MyStockFilter::isEmpty() const
{
	return MyPriceFilter::isEmpty()
		&& m_yesterdaysVolumeRange.isEmpty()
		&& m_todaysVolumeRange.isEmpty()
		&& m_todaysPxMhVolumeRange.isEmpty()
		&& m_avgDailyVolumeRange.isEmpty()
		&& m_relativeVolumeRange.isEmpty()
		&& m_preMhVolRelRange.isEmpty()
		&& m_yesterdaysPriceRangeRange.isEmpty()
		&& m_yesterdaysPriceRangePercentRange.isEmpty()

		&& m_imbalanceNyseRange.isEmpty()
		&& m_imbalanceNyseRegRange.isEmpty()
		&& m_imbalanceNsdqRange.isEmpty()
		&& m_imbalanceArcaRange.isEmpty()
		&& m_imbalanceExchRange.isEmpty()
		&& m_rateImbalanceRange.isEmpty()
		&& m_imbNyDeltaInfRange.isEmpty()
		&& m_imbExFarPriceDeltaRange.isEmpty()
		&& m_imbNqFarPriceDeltaRange.isEmpty()
		&& m_imbNqThruRange.isEmpty()
		&& m_imbExValueRange.isEmpty()

		&& m_imbExNearRange.isEmpty()
		&& m_imbExFarRange.isEmpty()
		&& m_imbExMatchRange.isEmpty()
		&& m_imbExVolPercentRange.isEmpty()
		&& m_imbExYestVolPercentRange.isEmpty()
		&& m_deltaPvRange.isEmpty()
//		&& m_rsiMhRange.isEmpty()
//		&& m_rsiDaysRange.isEmpty()
		&& m_borrowPriceRange.isEmpty()
		&& m_pmiSpreadRange.isEmpty()
		&& m_todaysClosePriceRange.isEmpty()

		&& m_bidNetRange.isEmpty()
		&& m_askNetRange.isEmpty()

		&& m_tBidNetRange.isEmpty()
		&& m_tAskNetRange.isEmpty()

		&& m_spreadRange.isEmpty()

		&& m_bidNetPercentRange.isEmpty()
		&& m_askNetPercentRange.isEmpty()

		&& m_tBidNetPercentRange.isEmpty()
		&& m_tAskNetPercentRange.isEmpty()

		&& m_luldRange.isEmpty()
		&& m_luldPercentRange.isEmpty()

		&& m_marketCategoryFilter == (1 << MCF_COUNT) - 1
		&& m_testStockFilter == 3//real + test
		&& m_securityTypeFilter == DialogStockFilter::securityTypeFilterAll
		&& m_htbFilter == HTBF_All
		&& m_tradingStateFilter == (1 << TSF_Count) - 1
		&& m_circuitBreakerFilter == (1 << CBF_Count) - 1
		&& m_nasdaqQuoteConditionFilter == DialogStockFilter::nasdaqQuoteConditionFilterAll
		&& m_listedQuoteConditionFilter == DialogStockFilter::listedQuoteConditionFilterAll
/*
		&& !m_rsiMhCalculate
		&& m_rsiMhPriorPointCount == 250
		&& m_rsiMhPeriodLength == 1
		&& m_rsiMhPeriodCount == 14
		&& m_rsiMhStartMinute == 570
		&& !m_rsiMhTodayOnly
		&& !m_rsiMhPostMarket

		&& !m_rsiDaysCalculate
		&& m_rsiDaysPriorPointCount == 250
		&& m_rsiDaysPeriodLength == 1
		&& m_rsiDaysPeriodCount == 14
		&& m_rsiDaysStartMinute == 570
		&& !m_rsiDaysPostMarket
*/
		;
}

void MyStockFilter::Reset()
{
	MyPriceFilter::Reset();
	m_marketCategoryFilter = (1 << MCF_COUNT) - 1;
	m_testStockFilter = 3;//real + test
	m_securityTypeFilter = DialogStockFilter::securityTypeFilterAll;
	m_htbFilter = HTBF_All;
	m_tradingStateFilter = (1 << TSF_Count) - 1;
	m_circuitBreakerFilter = (1 << CBF_Count) - 1;
	m_nasdaqQuoteConditionFilter = DialogStockFilter::nasdaqQuoteConditionFilterAll;
	m_listedQuoteConditionFilter = DialogStockFilter::listedQuoteConditionFilterAll;

	m_yesterdaysVolumeRange.Reset();
	m_todaysVolumeRange.Reset();
	m_todaysPxMhVolumeRange.Reset();

	m_avgDailyVolumeRange.Reset();
	m_relativeVolumeRange.Reset();
	m_preMhVolRelRange.Reset();

	m_yesterdaysPriceRangeRange.Reset();
	m_yesterdaysPriceRangePercentRange.Reset();
	m_imbalanceNyseRange.Reset();
	m_imbalanceNyseRegRange.Reset();
	m_imbalanceNsdqRange.Reset();
	m_imbalanceArcaRange.Reset();
	m_imbalanceExchRange.Reset();
	m_rateImbalanceRange.Reset();
	m_imbNyDeltaInfRange.Reset();
	m_imbExFarPriceDeltaRange.Reset();
	m_imbNqFarPriceDeltaRange.Reset();
	m_imbNqThruRange.Reset();
	m_imbExValueRange.Reset();
	m_imbExNearRange.Reset();
	m_imbExFarRange.Reset();
	m_imbExMatchRange.Reset();
	m_imbExVolPercentRange.Reset();
	m_imbExYestVolPercentRange.Reset();
	m_deltaPvRange.Reset();
//	m_rsiMhRange.Reset();
//	m_rsiDaysRange.Reset();
	m_borrowPriceRange.Reset();
	m_pmiSpreadRange.Reset();
	m_todaysClosePriceRange.Reset();

	m_bidNetRange.Reset();
	m_askNetRange.Reset();

	m_tBidNetRange.Reset();
	m_tAskNetRange.Reset();

	m_spreadRange.Reset();

	m_bidNetPercentRange.Reset();
	m_askNetPercentRange.Reset();

	m_tBidNetPercentRange.Reset();
	m_tAskNetPercentRange.Reset();

	m_luldRange.Reset();
	m_luldPercentRange.Reset();
/*
//RSI Minutes
	m_rsiMhCalculate = false;
	m_rsiMhPriorPointCount = 250;
	m_rsiMhPeriodLength = 1;
	m_rsiMhPeriodCount = 14;
	m_rsiMhStartMinute = 570;
	m_rsiMhTodayOnly = false;
	m_rsiMhPostMarket = false;
//RSI Days
	m_rsiDaysCalculate = false;
	m_rsiDaysPriorPointCount = 250;
	m_rsiDaysPeriodLength = 1;
	m_rsiDaysPeriodCount = 14;
	m_rsiDaysStartMinute = 570;
	m_rsiDaysPostMarket = false;
*/
}

bool MyStockFilter::isFilterSame(const TakionFilter& filter) const
{
	if(MyPriceFilter::isFilterSame(filter))
	{
		const MyStockFilter& other = (const MyStockFilter&)filter;
		return m_yesterdaysVolumeRange.isSameFiltering(other.m_yesterdaysVolumeRange)
			&& m_todaysVolumeRange.isSameFiltering(other.m_todaysVolumeRange)

			&& m_avgDailyVolumeRange.isSameFiltering(other.m_avgDailyVolumeRange)
			&& m_relativeVolumeRange.isSameFiltering(other.m_relativeVolumeRange)
			&& m_preMhVolRelRange.isSameFiltering(other.m_preMhVolRelRange)

			&& m_yesterdaysPriceRangeRange.isSameFiltering(other.m_yesterdaysPriceRangeRange)
			&& m_yesterdaysPriceRangePercentRange.isSameFiltering(other.m_yesterdaysPriceRangePercentRange)

			&& m_marketCategoryFilter == other.m_marketCategoryFilter

			&& m_imbalanceNyseRange.isSameFiltering(other.m_imbalanceNyseRange)
			&& m_imbalanceNyseRegRange.isSameFiltering(other.m_imbalanceNyseRegRange)
			&& m_imbalanceNsdqRange.isSameFiltering(other.m_imbalanceNsdqRange)
			&& m_imbalanceArcaRange.isSameFiltering(other.m_imbalanceArcaRange)
			&& m_imbalanceExchRange.isSameFiltering(other.m_imbalanceExchRange)

			&& m_testStockFilter == other.m_testStockFilter
			&& m_securityTypeFilter == other.m_securityTypeFilter

			&& m_todaysPxMhVolumeRange.isSameFiltering(other.m_todaysPxMhVolumeRange)
			&& m_rateImbalanceRange.isSameFiltering(other.m_rateImbalanceRange)
			&& m_imbNyDeltaInfRange.isSameFiltering(other.m_imbNyDeltaInfRange)
			&& m_imbExFarPriceDeltaRange.isSameFiltering(other.m_imbExFarPriceDeltaRange)
			&& m_imbNqFarPriceDeltaRange.isSameFiltering(other.m_imbNqFarPriceDeltaRange)
			&& m_imbNqThruRange.isSameFiltering(other.m_imbNqThruRange)
			&& m_imbExValueRange.isSameFiltering(other.m_imbExValueRange)
			&& m_imbExNearRange.isSameFiltering(other.m_imbExNearRange)
			&& m_imbExFarRange.isSameFiltering(other.m_imbExFarRange)
			&& m_imbExMatchRange.isSameFiltering(other.m_imbExMatchRange)
			&& m_imbExVolPercentRange.isSameFiltering(other.m_imbExVolPercentRange)
			&& m_imbExYestVolPercentRange.isSameFiltering(other.m_imbExYestVolPercentRange)
			&& m_deltaPvRange.isSameFiltering(other.m_deltaPvRange)
//			&& m_rsiMhRange.isSameFiltering(other.m_rsiMhRange)
//			&& m_rsiDaysRange.isSameFiltering(other.m_rsiDaysRange)
			&& m_borrowPriceRange.isSameFiltering(other.m_borrowPriceRange)
			&& m_pmiSpreadRange.isSameFiltering(other.m_pmiSpreadRange)
			&& m_todaysClosePriceRange.isSameFiltering(other.m_todaysClosePriceRange)

			&& m_htbFilter == other.m_htbFilter
			&& m_tradingStateFilter == other.m_tradingStateFilter
			&& m_circuitBreakerFilter == other.m_circuitBreakerFilter
			&& m_nasdaqQuoteConditionFilter == other.m_nasdaqQuoteConditionFilter
			&& m_listedQuoteConditionFilter == other.m_listedQuoteConditionFilter

			&& m_bidNetRange.isSameFiltering(other.m_bidNetRange)
			&& m_askNetRange.isSameFiltering(other.m_askNetRange)

			&& m_tBidNetRange.isSameFiltering(other.m_tBidNetRange)
			&& m_tAskNetRange.isSameFiltering(other.m_tAskNetRange)

			&& m_spreadRange.isSameFiltering(other.m_spreadRange)

			&& m_bidNetPercentRange.isSameFiltering(other.m_bidNetPercentRange)
			&& m_askNetPercentRange.isSameFiltering(other.m_askNetPercentRange)

			&& m_tBidNetPercentRange.isSameFiltering(other.m_tBidNetPercentRange)
			&& m_tAskNetPercentRange.isSameFiltering(other.m_tAskNetPercentRange)

			&& m_luldRange.isSameFiltering(other.m_luldRange)
			&& m_luldPercentRange.isSameFiltering(other.m_luldPercentRange)
			;
	}
	return false;
}

unsigned char MyStockFilter::isPotentialFilterSame(const TakionFilter& filter) const//1 - add; 2 - remove; 3 - both
{
	unsigned char ret = MyPriceFilter::isPotentialFilterSame(filter);

	const MyStockFilter& other = (const MyStockFilter&)filter;
	if(m_yesterdaysVolumeRange != other.m_yesterdaysVolumeRange)
	{
		if(m_yesterdaysVolumeRange.isUse())
		{
			if(other.m_yesterdaysVolumeRange.isUse())
			{
				if(m_yesterdaysVolumeRange.GetMin() < other.m_yesterdaysVolumeRange.GetMin())
				{
					ret |= 2;
				}
				else if(m_yesterdaysVolumeRange.GetMin() > other.m_yesterdaysVolumeRange.GetMin())
				{
					ret |= 1;
				}
				bool maxInf = m_yesterdaysVolumeRange.GetMax() < m_yesterdaysVolumeRange.GetMin();
				bool otherMaxInf = other.m_yesterdaysVolumeRange.GetMax() < other.m_yesterdaysVolumeRange.GetMin();
				if(maxInf)
				{
					if(!otherMaxInf)
					{
						ret |= 2;
					}
				}
				else if(otherMaxInf)
				{
					ret |= 1;
				}
				else if(m_yesterdaysVolumeRange.GetMax() < other.m_yesterdaysVolumeRange.GetMax())
				{
					ret |= 1;
				}
				else if(m_yesterdaysVolumeRange.GetMax() > other.m_yesterdaysVolumeRange.GetMax())
				{
					ret |= 2;
				}
			}
			else
			{
				ret |= 1;
			}
		}
		else if(other.m_yesterdaysVolumeRange.isUse())
		{
			ret |= 2;
		}
	}

#ifndef YEST_VALUES_CHANGE
	if(m_avgDailyVolumeRange != other.m_avgDailyVolumeRange)
	{
		if(m_avgDailyVolumeRange.isUse())
		{
			if(other.m_avgDailyVolumeRange.isUse())
			{
				if(m_avgDailyVolumeRange.GetMin() < other.m_avgDailyVolumeRange.GetMin())
				{
					ret |= 2;
				}
				else if(m_avgDailyVolumeRange.GetMin() > other.m_avgDailyVolumeRange.GetMin())
				{
					ret |= 1;
				}
				bool maxInf = m_avgDailyVolumeRange.GetMax() < m_avgDailyVolumeRange.GetMin();
				bool otherMaxInf = other.m_avgDailyVolumeRange.GetMax() < other.m_avgDailyVolumeRange.GetMin();
				if(maxInf)
				{
					if(!otherMaxInf)
					{
						ret |= 2;
					}
				}
				else if(otherMaxInf)
				{
					ret |= 1;
				}
				else if(m_avgDailyVolumeRange.GetMax() < other.m_avgDailyVolumeRange.GetMax())
				{
					ret |= 1;
				}
				else if(m_avgDailyVolumeRange.GetMax() > other.m_avgDailyVolumeRange.GetMax())
				{
					ret |= 2;
				}
			}
			else
			{
				ret |= 1;
			}
		}
		else if(other.m_avgDailyVolumeRange.isUse())
		{
			ret |= 2;
		}
	}

	if(m_yesterdaysPriceRangeRange != other.m_yesterdaysPriceRangeRange)
	{
		if(m_yesterdaysPriceRangeRange.isUse())
		{
			if(other.m_yesterdaysPriceRangeRange.isUse())
			{
				if(m_yesterdaysPriceRangeRange.GetMin() < other.m_yesterdaysPriceRangeRange.GetMin())
				{
					ret |= 2;
				}
				else if(m_yesterdaysPriceRangeRange.GetMin() > other.m_yesterdaysPriceRangeRange.GetMin())
				{
					ret |= 1;
				}
				bool maxInf = m_yesterdaysPriceRangeRange.GetMax() < m_yesterdaysPriceRangeRange.GetMin();
				bool otherMaxInf = other.m_yesterdaysPriceRangeRange.GetMax() < other.m_yesterdaysPriceRangeRange.GetMin();
				if(maxInf)
				{
					if(!otherMaxInf)
					{
						ret |= 2;
					}
				}
				else if(otherMaxInf)
				{
					ret |= 1;
				}
				else if(m_yesterdaysPriceRangeRange.GetMax() < other.m_yesterdaysPriceRangeRange.GetMax())
				{
					ret |= 1;
				}
				else if(m_yesterdaysPriceRangeRange.GetMax() > other.m_yesterdaysPriceRangeRange.GetMax())
				{
					ret |= 2;
				}
			}
			else
			{
				ret |= 1;
			}
		}
		else if(other.m_yesterdaysPriceRangeRange.isUse())
		{
			ret |= 2;
		}
	}
	if(m_yesterdaysPriceRangePercentRange != other.m_yesterdaysPriceRangePercentRange)
	{
		if(m_yesterdaysPriceRangePercentRange.isUse())
		{
			if(other.m_yesterdaysPriceRangePercentRange.isUse())
			{
				if(m_yesterdaysPriceRangePercentRange.GetMin() < other.m_yesterdaysPriceRangePercentRange.GetMin())
				{
					ret |= 2;
				}
				else if(m_yesterdaysPriceRangePercentRange.GetMin() > other.m_yesterdaysPriceRangePercentRange.GetMin())
				{
					ret |= 1;
				}
				bool maxInf = m_yesterdaysPriceRangePercentRange.GetMax() < m_yesterdaysPriceRangePercentRange.GetMin();
				bool otherMaxInf = other.m_yesterdaysPriceRangePercentRange.GetMax() < other.m_yesterdaysPriceRangePercentRange.GetMin();
				if(maxInf)
				{
					if(!otherMaxInf)
					{
						ret |= 2;
					}
				}
				else if(otherMaxInf)
				{
					ret |= 1;
				}
				else if(m_yesterdaysPriceRangePercentRange.GetMax() < other.m_yesterdaysPriceRangePercentRange.GetMax())
				{
					ret |= 1;
				}
				else if(m_yesterdaysPriceRangePercentRange.GetMax() > other.m_yesterdaysPriceRangePercentRange.GetMax())
				{
					ret |= 2;
				}
			}
			else
			{
				ret |= 1;
			}
		}
		else if(other.m_yesterdaysPriceRangePercentRange.isUse())
		{
			ret |= 2;
		}
	}
#endif

	if(m_marketCategoryFilter != other.m_marketCategoryFilter)
	{
		unsigned int sum = m_marketCategoryFilter | other.m_marketCategoryFilter;
		if(sum != m_marketCategoryFilter)
		{
			ret |= 1;
		}
		if(sum != other.m_marketCategoryFilter)
		{
			ret |= 2;
		}
	}
	if(m_testStockFilter != other.m_testStockFilter)
	{
		unsigned int sum = m_testStockFilter | other.m_testStockFilter;
		if(sum != m_testStockFilter)
		{
			ret |= 1;
		}
		if(sum != other.m_testStockFilter)
		{
			ret |= 2;
		}
	}
	if(m_securityTypeFilter != other.m_securityTypeFilter)
	{
		unsigned int sum = m_securityTypeFilter | other.m_securityTypeFilter;
		if(sum != m_securityTypeFilter)
		{
			ret |= 1;
		}
		if(sum != other.m_securityTypeFilter)
		{
			ret |= 2;
		}
	}
	return ret;
}

bool MyStockFilter::SecurityPotentiallyBelongs(const Security* security) const
{
	const char* symbol = security->GetSymbol();

	if(SecBelongsBySymbolInclude(security))return true;

	if(*symbol)
	{
		if(m_useFirstCharSet)
		{
			if(m_firstCharSet.find(*symbol) == m_firstCharSetEnd)
			{
				return false;
			}
		}
		if(m_useNoFirstCharSet)
		{
			if(m_noFirstCharSet.find(*symbol) != m_noFirstCharSetEnd)
			{
				return false;
			}
		}
		if(m_useCharSet)
		{
			const char* cursor;
			for(cursor = symbol + 1; *cursor; ++cursor)
			{
				if(m_charSet.find(*cursor) != m_charSetEnd)
				{
					break;
				}
			}
			if(!*cursor)
			{
				return false;
			}
		}
		if(m_useNoCharSet && !m_noCharSet.empty())
		{
			for(const char* cursor = symbol + 1; *cursor; ++cursor)
			{
				if(m_noCharSet.find(*cursor) != m_noCharSetEnd)
				{
					return false;
				}
			}
		}
	}
	return (m_testStockFilter & (security->isTest() ? 2 : 1)) != 0
		&& (m_securityTypeFilter & (1 << (security->GetType() - DialogStockFilter::zeroChar))) != 0
		&& (m_marketCategoryFilter & security->GetMarketCategoryFilter()) != 0
		&& m_yesterdaysVolumeRange.ValueBelongs(security->GetYesterdaysVolume())
#ifndef YEST_VALUES_CHANGE
		&& m_avgDailyVolumeRange.ValueBelongs(security->GetAverageDailyVolume())
		&& m_yesterdaysPriceRangeRange.ValueBelongs(security->GetYesterdaysPriceRange())
		&& m_yesterdaysPriceRangePercentRange.ValueBelongs(security->GetYesterdaysPriceRangePercent())
#endif
		&& SecBelongsBySymbolExclude(security)//!m_useExclude || m_excludeSymbolSet.find(security->GetNumericSymbol()) == m_excludeSymbolSet.end())
		;
}
/*
bool MyStockFilter::SecurityBelongsByRsiMh(const SecurityChartRow* securityChartRow) const
{
	return !m_rsiMhCalculate || !m_rsiMhRange.isUse() || securityChartRow->GetRsiMh().GetDollars() <= 100 && m_rsiMhRange.isPositiveValueOut(securityChartRow->GetRsiMh());
}

bool MyStockFilter::SecurityBelongsByRsiDays(const SecurityChartRow* securityChartRow) const
{
	return !m_rsiDaysCalculate || !m_rsiDaysRange.isUse() || securityChartRow->GetRsiDays().GetDollars() <= 100 && m_rsiDaysRange.isPositiveValueOut(securityChartRow->GetRsiDays());
}
*/
bool MyStockFilter::SecurityBelongsByCalcValues(const SecurityChartRow* securityChartRow) const
{
	return !m_relativeVolumeRange.isUse() || m_relativeVolumeRange.ValueBelongs(securityChartRow->GetMhRelativeVolume());
}

bool MyStockFilter::SecurityBelongs(const SecurityChartRow* securityChartRow) const
{
	return SecBelongs(securityChartRow->GetSecurity())
		&& SecurityBelongsByCalcValues(securityChartRow)
//		&& SecurityBelongsByRsiMh(securityChartRow)
//		&& SecurityBelongsByRsiDays(securityChartRow)
		;
}

bool MyStockFilter::SecBelongsByValues(const Security* security) const
{
	return MyPriceFilter::SecBelongsByValues(security)
		&& m_todaysVolumeRange.ValueBelongs(security->GetVolume())
		&& m_todaysPxMhVolumeRange.ValueBelongs(security->GetPrimaryExchangeMhVolume())

		&& m_todaysClosePriceRange.ValueBelongs(security->GetTodaysClosePrice())
		&& m_imbalanceNyseRange.ValueBelongs(security->GetImbalanceSharesNyse())
		&& m_imbalanceNyseRegRange.ValueBelongs(security->GetRegulatoryImbalanceSharesNyse())
		&& m_imbalanceNsdqRange.ValueBelongs(security->GetImbalanceSharesNsdq())
		&& m_imbalanceArcaRange.ValueBelongs(security->GetImbalanceSharesArca())
		&& m_imbalanceExchRange.ValueBelongs(security->GetImbalanceSharesExch())

		&& m_imbExNearRange.ValueBelongs(security->GetNearPriceExch())
		&& m_imbExFarRange.ValueBelongs(security->GetFarPriceExch())

		&& m_imbExMatchRange.ValueBelongs(security->GetPairedSharesExch())

		&& m_preMhVolRelRange.ValueBelongs(security->GetPreMarketVolumeByADV())

#ifdef YEST_VALUES_CHANGE
		&& m_avgDailyVolumeRange.ValueBelongs(security->GetAverageDailyVolume())
		&& m_yesterdaysPriceRangeRange.ValueBelongs(security->GetYesterdaysPriceRange())
		&& m_yesterdaysPriceRangePercentRange.ValueBelongs(security->GetYesterdaysPriceRangePercent())
#endif
		;
}

bool MyStockFilter::SecBelongsByCalcValues(const Security* security) const
{
	if(!(m_tradingStateFilter & security->GetTradingStateFilter())
		|| !(m_circuitBreakerFilter & security->GetCircuitBreakerFilter())
		|| (security->isNasdaq() ?//PE_NSDQ == security->GetPrimaryExchange() ?
			!(m_nasdaqQuoteConditionFilter & security->GetQuoteConditionFilter()):
			!(m_listedQuoteConditionFilter & security->GetQuoteConditionFilter()))
		)return false;

	TakionMainWnd* takionMainWnd = TGUI_GetTakionMainWnd();
	const unsigned int htbFilter = security->GetHtbFilter(takionMainWnd->GetCurrentStockLoanFirmOrdinal());
	if(!(m_htbFilter & htbFilter))return false;

	if(m_borrowPriceRange.isUse())
	{
		if(htbFilter & HTBF_ETB)
		{
			if(!m_borrowPriceRange.isValueIn(Price::priceZero))return false;
		}
		else
		{
			const unsigned int& slOrdinal = takionMainWnd->GetCurrentStockLoanFirmOrdinal();
			const unsigned int* borrowPrice = security->GetCompactBorrowPrice(slOrdinal);

			if(borrowPrice)
			{
				if(!m_borrowPriceRange.isValueIn(
#ifdef BORROW_PRICE_DISCOUNT
					security->GetBorrowDiscountFlag(slOrdinal) ? Price(*borrowPrice) : Price(Price(*borrowPrice) * takionMainWnd->GetBorrowPriceMultiplier())
#else
					Price(*borrowPrice)
#endif
				))
				{
					return false;
				}
			}
			else
			{
				if(!m_borrowPriceRange.isValueIn(Price::priceZero))return false;
			}
		}
	}
	if(m_pmiSpreadRange.isUse())
	{
		const Price& bid = security->GetPmiBid();
		const Price& ask = security->GetPmiAsk();
		Price spread;
		if(bid < ask)
		{
			spread = ask - bid;
		}
		if(!m_pmiSpreadRange.isValueIn(spread))
		{
			return false;
		}
	}
	if(m_rateImbalanceRange.isUse())
	{
		SignedPrice val;
		int denominator = security->GetInitialImbalanceSharesExch();
		if(denominator)
		{
			int numerator = security->GetImbalanceSharesExch();
			if(numerator)
			{
				const bool numeratorNegative = numerator < 0;
				if(numeratorNegative)numerator = -numerator;
				const bool denominatorNegative = denominator < 0;
				if(denominatorNegative)denominator = -denominator;

				numerator *= 200;
				numerator /= denominator;
				++numerator;
				numerator >>= 1;
				const int dollars = numerator / 100;
				if(numeratorNegative == denominatorNegative)
				{
					val.SetValue(dollars, (numerator - 100 * dollars) * (SignedPrice::divider / 100));
				}
				else
				{
					val.SetValue(-dollars, -((numerator - 100 * dollars) * (SignedPrice::divider / 100)));
				}
			}
		}
		if(!m_rateImbalanceRange.isValueIn(val))
		{
			return false;
		}
	}
	if(m_imbNyDeltaInfRange.isUse())
	{
		const unsigned int last = security->GetLastNbboPrintPrice();//D_GetLastPrice();
		if(last)
		{
			const Price& first = security->GetNearPriceNyse();
			if(!first.isZero())
			{
				if(!m_imbNyDeltaInfRange.isValueIn(first - Price(last)))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(m_imbExFarPriceDeltaRange.isUse())
	{
		if(security->Get2ndFarPriceExch().isZero()
			|| !m_imbExFarPriceDeltaRange.isValueIn(security->GetFarPriceDiffExch())//ValueBelongs(security->GetFarPriceDiffExch())
			)
		{
			return false;
		}
	}
	if(m_imbNqFarPriceDeltaRange.isUse())
	{
		if(security->Get2ndFarPriceNsdq().isZero()
			|| !m_imbNqFarPriceDeltaRange.isValueIn(security->GetFarPriceDiffNsdq())//ValueBelongs(security->GetFarPriceDiffNsdq())
			)
		{
			return false;
		}
	}
	if(m_imbNqThruRange.isUse())
	{
		const Price& nearPrice = security->GetNearPriceNsdq();
		if(nearPrice.isZero())
		{
			return false;
		}
		else
		{
			const Price& currentReferencePrice = security->GetCurrentReferencePriceNsdq();
			if(currentReferencePrice.isZero() || !m_imbNqThruRange.isValueIn(nearPrice - currentReferencePrice))
			{
				return false;
			}
		}
	}

	if(m_imbExValueRange.isUse())
	{
		const int& imbShares = security->GetImbalanceSharesExch();
		if(!imbShares)
		{
			return false;
		}
		else
		{
			const Price& currentReferencePrice = security->GetCurrentReferencePriceExch();
			if(currentReferencePrice.isZero() || !m_imbExValueRange.isValueIn(currentReferencePrice * imbShares))
			{
				return false;
			}
		}
	}

	if(m_bidNetRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetL1Bid();
		const Price& closePrice = security->GetClosePrice();
		if(priceAsUInt != 0 && !closePrice.isZero())
		{
			const Price p(priceAsUInt);
			const SignedPrice value(p - closePrice);
			if(!m_bidNetRange.isValueIn(value))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(m_askNetRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetL1Ask();
		const Price& closePrice = security->GetClosePrice();
		if(priceAsUInt != 0 && !closePrice.isZero())
		{
			const Price p(priceAsUInt);
			const SignedPrice value(p - closePrice);
			if(!m_askNetRange.isValueIn(value))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(m_tBidNetRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetL1Bid();
		const Price& closePrice = security->GetTodaysClosePrice();
		if(priceAsUInt != 0 && !closePrice.isZero())
		{
			const Price p(priceAsUInt);
			const SignedPrice value(p - closePrice);
			if(!m_tBidNetRange.isValueIn(value))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(m_tAskNetRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetL1Ask();
		const Price& closePrice = security->GetTodaysClosePrice();
		if (priceAsUInt != 0 && !closePrice.isZero())
		{
			const Price p(priceAsUInt);
			const SignedPrice value(p - closePrice);
			if(!m_tAskNetRange.isValueIn(value))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
////
	if(m_spreadRange.isUse())
	{
		const unsigned int bid = security->GetL1Bid();
		const unsigned int ask = security->GetL1Ask();
		if(bid && ask)
		{
			if(!m_spreadRange.isValueIn(Price(ask) - Price(bid)))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
////
	if(m_bidNetPercentRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetL1Bid();
		const Price& closePrice = security->GetClosePrice();
		if(priceAsUInt != 0 && !closePrice.isZero())
		{
			const Price p(priceAsUInt);
			Money value(p - closePrice);
			if(!RowValue::DivideMoneyByPrice(value, closePrice))value.SetZero();
			if(!m_bidNetPercentRange.isValueIn(value))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(m_askNetPercentRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetL1Ask();
		const Price& closePrice = security->GetClosePrice();
		if(priceAsUInt != 0 && !closePrice.isZero())
		{
			const Price p(priceAsUInt);
			Money value(p - closePrice);
			if(!RowValue::DivideMoneyByPrice(value, closePrice))value.SetZero();
			if(!m_askNetPercentRange.isValueIn(value))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(m_tBidNetPercentRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetL1Bid();
		const Price& closePrice = security->GetTodaysClosePrice();
		if(priceAsUInt != 0 && !closePrice.isZero())
		{
			const Price p(priceAsUInt);
			Money value(p - closePrice);
			if(!RowValue::DivideMoneyByPrice(value, closePrice))value.SetZero();
			if(!m_tBidNetPercentRange.isValueIn(value))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(m_tAskNetPercentRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetL1Ask();
		const Price& closePrice = security->GetTodaysClosePrice();
		if (priceAsUInt != 0 && !closePrice.isZero())
		{
			const Price p(priceAsUInt);
			Money value(p - closePrice);
			if(!RowValue::DivideMoneyByPrice(value, closePrice))value.SetZero();
			if(!m_tAskNetPercentRange.isValueIn(value))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(m_luldRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetLastNbboPrintPrice();
		if(priceAsUInt)
		{
			bool ret = false;
			if(m_luldRange.isPositiveUsed())
			{
				const Price& luldHigh = security->GetLuldHigh();
				if(!luldHigh.isZero())
				{
					const SignedPrice val = SignedPrice(luldHigh) - SignedPrice(priceAsUInt);
					if(val.isNegative() ? m_luldRange.GetMin().isZero() : m_luldRange.isPositiveValueIn(val))
					{
						ret = true;
					}
				}
			}
			if(!ret && m_luldRange.isNegativeUsed())
			{
				const Price& luldLow = security->GetLuldLow();
				if(!luldLow.isZero())
				{
					const SignedPrice val = SignedPrice(priceAsUInt) - SignedPrice(luldLow);
					if(val.isNegative() ? m_luldRange.GetMin().isZero() : m_luldRange.isPositiveValueIn(val))
					{
						ret = true;
					}
				}
			}
			if(!ret)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(m_luldPercentRange.isUse())
	{
		const unsigned int priceAsUInt = security->GetLastNbboPrintPrice();
		if(priceAsUInt)
		{
			bool ret = false;
			if(m_luldPercentRange.isPositiveUsed())
			{
				const Price& luldHigh = security->GetLuldHigh();
				if(!luldHigh.isZero())
				{
					const Price p(priceAsUInt);
					const SignedPrice val = luldHigh - p;
					if(val.isNegative())
					{
						ret = m_luldRange.GetMin().isZero();
					}
					else
					{
						Money value(val);
						if(!RowValue::DivideMoneyByPrice(value, p))value.SetZero();
						if(m_luldPercentRange.isPositiveValueIn(value))
						{
							ret = true;
						}
					}
				}
			}
			if(!ret && m_luldPercentRange.isNegativeUsed())
			{
				const Price& luldLow = security->GetLuldLow();
				if(!luldLow.isZero())
				{
					const Price p(priceAsUInt);
					const SignedPrice val = p - luldLow;
					if(val.isNegative())
					{
						ret = m_luldRange.GetMin().isZero();
					}
					else
					{
						Money value(val);
						if(!RowValue::DivideMoneyByPrice(value, p))value.SetZero();
						if(m_luldPercentRange.isPositiveValueIn(value))
						{
							ret = true;
						}
					}
				}
			}
			if(!ret)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
///
	if(m_imbExVolPercentRange.isUse())
	{
		const unsigned __int64 denominator = security->GetVolume();
		const int numerator = security->GetImbalanceSharesExch();
		if(denominator)
		{
			Money value((__int64)(numerator < 0 ? -numerator : numerator) * 100, 0);
			value.operator/=(denominator);
			if(!m_imbExVolPercentRange.isPositiveValueIn(value))
			{
				return false;
			}
		}
		else
		{
			if(!numerator || !m_imbExVolPercentRange.GetMax().isZero())
			{
				return false;
			}
		}
	}
///
	if(m_imbExYestVolPercentRange.isUse())
	{
		const unsigned __int64 denominator = security->GetYesterdaysVolume();
		const int numerator = security->GetImbalanceSharesExch();
		if(denominator)
		{
			Money value((__int64)(numerator < 0 ? -numerator : numerator) * 100, 0);
			value.operator/=(denominator);
			if(!m_imbExYestVolPercentRange.isPositiveValueIn(value))
			{
				return false;
			}
		}
		else
		{
			if(!numerator || !m_imbExYestVolPercentRange.GetMax().isZero())
			{
				return false;
			}
		}
	}
	if(m_deltaPvRange.isUse())
	{
		Price val;
		int denominator = security->GetImbalanceSharesExch();
		if(denominator)
		{
			unsigned int numerator = security->GetPairedSharesExch();
			if(numerator)
			{
				numerator *= 200;
				numerator /= (unsigned int)(denominator > 0 ? denominator : -denominator);
				++numerator;
				numerator >>= 1;
				unsigned int dollars = numerator / 100;
				val.SetValue(dollars, (numerator - 100 * dollars) * (Price::divider / 100));
			}
		}
		if(!m_deltaPvRange.isValueIn(val))
		{
			return false;
		}
	}
	return true;
}

bool MyStockFilter::SecBelongs(const Security* security) const
{
	if(SecBelongsBySymbolInclude(security))
	{
		return true;
	}
	if(!SecBelongsByValues(security))
	{
		return false;
	}
	return SecBelongsByCalcValues(security);
}

enum TakionStockFilterDialogIds
{
	yVolumeGroupId = 1150,
	yVolumeUseId,
	yVolumeSpinMinId,
	yVolumeSpinMaxId,

	avgDailyVolumeGroupId,
	avgDailyVolumeUseId,
	avgDailyVolumeSpinMinId,
	avgDailyVolumeSpinMaxId,

	relativeVolumeGroupId,
	relativeVolumeUseId,
	relativeVolumeSpinMinId,
	relativeVolumeSpinMaxId,

	preMhVolRelGroupId,
	preMhVolRelUseId,
	preMhVolRelSpinMinId,
	preMhVolRelSpinMaxId,

	yPriceRangeGroupId,
	yPriceRangeUseId,
	yPriceRangeSpinMinId,
	yPriceRangeSpinMaxId,

	yPriceRangePercentGroupId,
	yPriceRangePercentUseId,
	yPriceRangePercentSpinMinId,
	yPriceRangePercentSpinMaxId,

	tVolumeGroupId,
	tVolumeUseId,
	tVolumeSpinMinId,
	tVolumeSpinMaxId,

	tPxMhVolumeGroupId,
	tPxMhVolumeUseId,
	tPxMhVolumeSpinMinId,
	tPxMhVolumeSpinMaxId,

	exchangeStaticId,
	exchangeListBoxId,

	testStaticId,
	testListBoxId,
	securityTypeStaticId,
	securityTypeListBoxId,
	htbStaticId,
	htbListBoxId,

	imbalanceNyseGroupId,
	imbalanceNyseUseId,
	imbalanceNyseSpinMinId,
	imbalanceNyseSpinMaxId,
	imbalanceNysePositiveId,
	imbalanceNyseNegativeId,

	imbalanceNyseRegGroupId,
	imbalanceNyseRegUseId,
	imbalanceNyseRegSpinMinId,
	imbalanceNyseRegSpinMaxId,
	imbalanceNyseRegPositiveId,
	imbalanceNyseRegNegativeId,

	imbalanceNsdqGroupId,
	imbalanceNsdqUseId,
	imbalanceNsdqSpinMinId,
	imbalanceNsdqSpinMaxId,
	imbalanceNsdqPositiveId,
	imbalanceNsdqNegativeId,

	imbalanceArcaGroupId,
	imbalanceArcaUseId,
	imbalanceArcaSpinMinId,
	imbalanceArcaSpinMaxId,
	imbalanceArcaPositiveId,
	imbalanceArcaNegativeId,

	imbalanceExchGroupId,
	imbalanceExchUseId,
	imbalanceExchSpinMinId,
	imbalanceExchSpinMaxId,
	imbalanceExchPositiveId,
	imbalanceExchNegativeId,

	rateImbalanceGroupId,
	rateImbalanceUseId,
	rateImbalanceSpinMinId,
	rateImbalanceSpinMaxId,
	rateImbalancePositiveId,
	rateImbalanceNegativeId,

	imbNyDeltaInfGroupId,
	imbNyDeltaInfUseId,
	imbNyDeltaInfSpinMinId,
	imbNyDeltaInfSpinMaxId,
	imbNyDeltaInfPositiveId,
	imbNyDeltaInfNegativeId,

	imbExFarPriceDeltaGroupId,
	imbExFarPriceDeltaUseId,
	imbExFarPriceDeltaSpinMinId,
	imbExFarPriceDeltaSpinMaxId,
	imbExFarPriceDeltaPositiveId,
	imbExFarPriceDeltaNegativeId,

	imbNqFarPriceDeltaGroupId,
	imbNqFarPriceDeltaUseId,
	imbNqFarPriceDeltaSpinMinId,
	imbNqFarPriceDeltaSpinMaxId,
	imbNqFarPriceDeltaPositiveId,
	imbNqFarPriceDeltaNegativeId,

	imbNqThruGroupId,
	imbNqThruUseId,
	imbNqThruSpinMinId,
	imbNqThruSpinMaxId,
	imbNqThruPositiveId,
	imbNqThruNegativeId,

	imbExValueGroupId,
	imbExValueUseId,
	imbExValueSpinMinId,
	imbExValueSpinMaxId,
	imbExValuePositiveId,
	imbExValueNegativeId,

	imbExNearGroupId,
	imbExNearUseId,
	imbExNearSpinMinId,
	imbExNearSpinMaxId,

	imbExFarGroupId,
	imbExFarUseId,
	imbExFarSpinMinId,
	imbExFarSpinMaxId,

	imbExMatchGroupId,
	imbExMatchUseId,
	imbExMatchSpinMinId,
	imbExMatchSpinMaxId,

	imbExVolPercentGroupId,
	imbExVolPercentUseId,
	imbExVolPercentSpinMinId,
	imbExVolPercentSpinMaxId,

	imbExYestVolPercentGroupId,
	imbExYestVolPercentUseId,
	imbExYestVolPercentSpinMinId,
	imbExYestVolPercentSpinMaxId,

	deltaPvGroupId,
	deltaPvUseId,
	deltaPvSpinMinId,
	deltaPvSpinMaxId,

	borrowPriceGroupId,
	borrowPriceUseId,
	borrowPriceSpinMinId,
	borrowPriceSpinMaxId,

	pmiSpreadGroupId,
	pmiSpreadUseId,
	pmiSpreadSpinMinId,
	pmiSpreadSpinMaxId,

	todaysClosePriceGroupId,
	todaysClosePriceUseId,
	todaysClosePriceSpinMinId,
	todaysClosePriceSpinMaxId,
//
	bidNetGroupId,
	bidNetUseId,
	bidNetSpinMinId,
	bidNetSpinMaxId,
	bidNetPositiveId,
	bidNetNegativeId,

	askNetGroupId,
	askNetUseId,
	askNetSpinMinId,
	askNetSpinMaxId,
	askNetPositiveId,
	askNetNegativeId,

	tBidNetGroupId,
	tBidNetUseId,
	tBidNetSpinMinId,
	tBidNetSpinMaxId,
	tBidNetPositiveId,
	tBidNetNegativeId,

	tAskNetGroupId,
	tAskNetUseId,
	tAskNetSpinMinId,
	tAskNetSpinMaxId,
	tAskNetPositiveId,
	tAskNetNegativeId,
//
	spreadGroupId,
	spreadUseId,
	spreadSpinMinId,
	spreadSpinMaxId,
	spreadPositiveId,
	spreadNegativeId,
//
	bidNetPercentGroupId,
	bidNetPercentUseId,
	bidNetPercentSpinMinId,
	bidNetPercentSpinMaxId,
	bidNetPercentPositiveId,
	bidNetPercentNegativeId,

	askNetPercentGroupId,
	askNetPercentUseId,
	askNetPercentSpinMinId,
	askNetPercentSpinMaxId,
	askNetPercentPositiveId,
	askNetPercentNegativeId,

	tBidNetPercentGroupId,
	tBidNetPercentUseId,
	tBidNetPercentSpinMinId,
	tBidNetPercentSpinMaxId,
	tBidNetPercentPositiveId,
	tBidNetPercentNegativeId,

	tAskNetPercentGroupId,
	tAskNetPercentUseId,
	tAskNetPercentSpinMinId,
	tAskNetPercentSpinMaxId,
	tAskNetPercentPositiveId,
	tAskNetPercentNegativeId,

	luldDiffGroupId,
	luldDiffUseId,
	luldDiffSpinMinId,
	luldDiffSpinMaxId,
	luldDiffPositiveId,
	luldDiffNegativeId,

	luldDiffPercentGroupId,
	luldDiffPercentUseId,
	luldDiffPercentSpinMinId,
	luldDiffPercentSpinMaxId,
	luldDiffPercentPositiveId,
	luldDiffPercentNegativeId,
/*
	rsiMarketHoursCalculationGroupId,
	rsiMarketHoursCheckBoxCalculateId,
	rsiMarketHoursCheckBoxTodayOnlyId,
	rsiMarketHoursSpinPriorPointCountId,
	rsiMarketHoursStaticPriorPointCountId,
	rsiMarketHoursSpinPeriodLengthId,
	rsiMarketHoursStaticPeriodLengthId,
	rsiMarketHoursSpinPeriodCountId,
	rsiMarketHoursStaticPeriodCountId,
	rsiMarketHoursSpinStartMinuteId,
	rsiMarketHoursStaticStartMinuteId,
	rsiMarketHoursCheckBoxPostMarketId,

	rsiMhGroupId,
	rsiMhUseId,
	rsiMhSpinMinId,
	rsiMhSpinMaxId,

	rsiDaysCalculationGroupId,
	rsiDaysCheckBoxCalculateId,
	rsiDaysSpinPriorPointCountId,
	rsiDaysStaticPriorPointCountId,
	rsiDaysSpinPeriodLengthId,
	rsiDaysStaticPeriodLengthId,
	rsiDaysSpinPeriodCountId,
	rsiDaysStaticPeriodCountId,
	rsiDaysSpinStartMinuteId,
	rsiDaysStaticStartMinuteId,
	rsiDaysCheckBoxPostMarketId,

	rsiDaysFilterGroupId,
	rsiDaysFilterUseId,
	rsiDaysFilterSpinMinId,
	rsiDaysFilterSpinMaxId,
*/
	tradingStateStaticId,
	tradingStateListBoxId,

	circuitBreakerStaticId,
	circuitBreakerListBoxId,

	nasdaqQcStaticId,
	nasdaqQcListBoxId,

	listedQcStaticId,
	listedQcListBoxId,

	stockFilterCount,
};

MyDialogStockFilter::MyDialogStockFilter(TakionMainWnd* mainWnd,
	int xPriceOffset, int priceTop, int xSymbolOffset, int xSymbolHeight, bool useTimeFrame,

	bool toDestroyOnFocus, TakionFrame* receipient, CWnd* parent, LPCSTR caption, DWORD style, DWORD exstendedStyle, int x, int y, int w, int h,
	LPCSTR font, WORD fontSize, bool executeCommands, bool closeOnCommandExecution):

	MyDialogPriceFilter(mainWnd, xPriceOffset, priceTop, xSymbolOffset, xSymbolHeight,
		toDestroyOnFocus, receipient, parent, caption, style, exstendedStyle, x, y, w, h, font, fontSize, executeCommands, closeOnCommandExecution),

	m_ListBoxMarketCategory("Select Market Categories for Stocks to filter in"),
	m_ListBoxTestOrReal(2, false, "Filter in Test Stocks and / or Real Stocks"),
	m_ListBoxSecurityType(31, false, "Select Security Types for Stocks to filter in"),
	m_ListBoxHtb(3, true),
	m_ListBoxTradingState(TSF_Count, true),
	m_ListBoxCircuitBreaker(CBF_Count, true),
	m_ListBoxNasdaqQc('Z' - 'A' + 2, true),
	m_ListBoxListedQc('Z' - 'A' + 2, true),

	m_yesterdaysVolume(yVolumeGroupId, yVolumeUseId, yVolumeSpinMinId, yVolumeSpinMaxId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Yesterday's Volume"),
	m_todaysVolume(tVolumeGroupId, tVolumeUseId, tVolumeSpinMinId, tVolumeSpinMaxId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Today's Volume"),
	m_todaysPxMhVolume(tPxMhVolumeGroupId, tPxMhVolumeUseId, tPxMhVolumeSpinMinId, tPxMhVolumeSpinMaxId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Primary Exchange Market Hours Volume"),
	
	m_avgDailyVolume(avgDailyVolumeGroupId, avgDailyVolumeUseId, avgDailyVolumeSpinMinId, avgDailyVolumeSpinMaxId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Average Daily Volume (Market Hours) for the last 20 days"),
	m_relativeVolume(relativeVolumeGroupId, relativeVolumeUseId, relativeVolumeSpinMinId, relativeVolumeSpinMaxId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Today's Volume as percentage of prorated Average Daily Volume"),
	m_preMhVolRel(preMhVolRelGroupId, preMhVolRelUseId, preMhVolRelSpinMinId, preMhVolRelSpinMaxId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Today's Pre Market Volume as percentage of prorated Average Daily Volume"),

	m_yesterdaysPriceRange(yPriceRangeGroupId, yPriceRangeUseId, yPriceRangeSpinMinId, yPriceRangeSpinMaxId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Yesterday's Price Range (within Market Hours)"),
	m_yesterdaysPriceRangePercent(yPriceRangePercentGroupId, yPriceRangePercentUseId, yPriceRangePercentSpinMinId, yPriceRangePercentSpinMaxId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Yesterday's Price Range as % of Yesterday's Low Price (within Market Hours)"),

	m_imbalanceNyse(imbalanceNyseGroupId, imbalanceNyseUseId, imbalanceNyseSpinMinId, imbalanceNyseSpinMaxId, imbalanceNysePositiveId, imbalanceNyseNegativeId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by NYSE Informational Imbalance"),
	m_imbalanceNyseReg(imbalanceNyseRegGroupId, imbalanceNyseRegUseId, imbalanceNyseRegSpinMinId, imbalanceNyseRegSpinMaxId, imbalanceNyseRegPositiveId, imbalanceNyseRegNegativeId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by NYSE Regulatory Imbalance"),
	m_imbalanceNsdq(imbalanceNsdqGroupId, imbalanceNsdqUseId, imbalanceNsdqSpinMinId, imbalanceNsdqSpinMaxId, imbalanceNsdqPositiveId, imbalanceNsdqNegativeId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by NASDAQ Imbalance"),
	m_imbalanceArca(imbalanceArcaGroupId, imbalanceArcaUseId, imbalanceArcaSpinMinId, imbalanceArcaSpinMaxId, imbalanceArcaPositiveId, imbalanceArcaNegativeId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by ARCA Imbalance"),
	m_imbalanceExch(imbalanceExchGroupId, imbalanceExchUseId, imbalanceExchSpinMinId, imbalanceExchSpinMaxId, imbalanceExchPositiveId, imbalanceExchNegativeId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Primary Exchange Imbalance"),
	m_rateImbalance(rateImbalanceGroupId, rateImbalanceUseId, rateImbalanceSpinMinId, rateImbalanceSpinMaxId, rateImbalancePositiveId, rateImbalanceNegativeId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Primary Exchange Imbalance as percentage of Initial Primary Exchange Imbalance"),
	m_imbNyDeltaInf(imbNyDeltaInfGroupId, imbNyDeltaInfUseId, imbNyDeltaInfSpinMinId, imbNyDeltaInfSpinMaxId, imbNyDeltaInfPositiveId, imbNyDeltaInfNegativeId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between NYSE Imbalance Near Price and Last Print NBBO Price"),
	m_imbExFarPriceDelta(imbExFarPriceDeltaGroupId, imbExFarPriceDeltaUseId, imbExFarPriceDeltaSpinMinId, imbExFarPriceDeltaSpinMaxId, imbExFarPriceDeltaPositiveId, imbExFarPriceDeltaNegativeId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between Exchange Imbalance 2nd Far Price and 1st Far Price"),
	m_imbNqFarPriceDelta(imbNqFarPriceDeltaGroupId, imbNqFarPriceDeltaUseId, imbNqFarPriceDeltaSpinMinId, imbNqFarPriceDeltaSpinMaxId, imbNqFarPriceDeltaPositiveId, imbNqFarPriceDeltaNegativeId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between NASDAQ Imbalance 2nd Far Price and 1st Far Price"),
	m_imbNqThru(imbNqThruGroupId, imbNqThruUseId, imbNqThruSpinMinId, imbNqThruSpinMaxId, imbNqThruPositiveId, imbNqThruNegativeId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between NSDQ Near Price and Current Reference Price"),
	m_imbExValue(imbExValueGroupId, imbExValueUseId, imbExValueSpinMinId, imbExValueSpinMaxId, imbExValuePositiveId, imbExValueNegativeId, 99999999999ui64, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by ImbExValue = ImbEX * ImbEXref"),

	m_imbExNear(imbExNearGroupId, imbExNearUseId, imbExNearSpinMinId, imbExNearSpinMaxId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Exchange Imbalance Near Price"),
	m_imbExFar(imbExFarGroupId, imbExFarUseId, imbExFarSpinMinId, imbExFarSpinMaxId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Exchange Imbalance Far Price"),

	m_imbExMatch(imbExMatchGroupId, imbExMatchUseId, imbExMatchSpinMinId, imbExMatchSpinMaxId, 999999999, 4, 0, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Exchange Imbalance Match Shares"),
	m_imbExVolPercent(imbExVolPercentGroupId, imbExVolPercentUseId, imbExVolPercentSpinMinId, imbExVolPercentSpinMaxId, 999999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Exchange Imbalance as percentage of Today's Volume"),
	m_imbExYestVolPercent(imbExYestVolPercentGroupId, imbExYestVolPercentUseId, imbExYestVolPercentSpinMinId, imbExYestVolPercentSpinMaxId, 999999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Exchange Imbalance as percentage of Yesterday's Volume"),

	m_deltaPv(deltaPvGroupId, deltaPvUseId, deltaPvSpinMinId, deltaPvSpinMaxId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Exchange Imbalance Match Shares as percentage of absolute value of the Imbalance Shares"),
	m_borrowPrice(borrowPriceGroupId, borrowPriceUseId, borrowPriceSpinMinId, borrowPriceSpinMaxId, 99999999, 3, 4, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Borrow Price (in cents)"),
	m_pmiSpread(pmiSpreadGroupId, pmiSpreadUseId, pmiSpreadSpinMinId, pmiSpreadSpinMaxId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the PMI Spread"),
	m_todaysClosePrice(todaysClosePriceGroupId, todaysClosePriceUseId, todaysClosePriceSpinMinId, todaysClosePriceSpinMaxId, 99999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Today's Close Price (can be used for detecting if a stock is closed)"),

	m_bidNet(bidNetGroupId, bidNetUseId, bidNetSpinMinId, bidNetSpinMaxId, bidNetPositiveId, bidNetNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between current Bid and Yesterday's Close Price"),
	m_askNet(askNetGroupId, askNetUseId, askNetSpinMinId, askNetSpinMaxId, askNetPositiveId, askNetNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between current Ask and Yesterday's Close Price"),

	m_tBidNet(tBidNetGroupId, tBidNetUseId, tBidNetSpinMinId, tBidNetSpinMaxId, tBidNetPositiveId, tBidNetNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between current Bid and Today's Close Price"),
	m_tAskNet(tAskNetGroupId, tAskNetUseId, tAskNetSpinMinId, tAskNetSpinMaxId, tAskNetPositiveId, tAskNetNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between current Ask and Today's Close Price"),

	m_spread(spreadGroupId, spreadUseId, spreadSpinMinId, spreadSpinMaxId, spreadPositiveId, spreadNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Spread (Difference between Level1 Ask and Bid"),

	m_bidNetPercent(bidNetPercentGroupId, bidNetPercentUseId, bidNetPercentSpinMinId, bidNetPercentSpinMaxId, bidNetPercentPositiveId, bidNetPercentNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between current Bid and Yesterday's Close Price as percentage of Yesterday's Close Price"),
	m_askNetPercent(askNetPercentGroupId, askNetPercentUseId, askNetPercentSpinMinId, askNetPercentSpinMaxId, askNetPercentPositiveId, askNetPercentNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between current Ask and Yesterday's Close Price as percentage of Yesterday's Close Price"),

	m_tBidNetPercent(tBidNetPercentGroupId, tBidNetPercentUseId, tBidNetPercentSpinMinId, tBidNetPercentSpinMaxId, tBidNetPercentPositiveId, tBidNetPercentNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between current Bid and Today's Close Price as percentage of Today's Close Price"),
	m_tAskNetPercent(tAskNetPercentGroupId, tAskNetPercentUseId, tAskNetPercentSpinMinId, tAskNetPercentSpinMaxId, tAskNetPercentPositiveId, tAskNetPercentNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference between current Ask and Today's Close Price as percentage of Today's Close Price"),
/*
	m_CheckBoxRsiMhCalculate("Calculate Today's RSI (Relative Strength Index)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), m_enabledTextColor, m_pressedCheckColor),
	m_CheckBoxRsiMhTodayOnly("Use only Today's data to calculate Today's RSI (Don't go back to Yesterday's data)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_pressedCheckColor),
	m_SpinRsiMhPriorPointCount(500, 0, 250, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Number of Prior Points to Initialize Today's RSI"),
	m_SpinRsiMhPeriodLength(20, 1, 1, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Period Length for which a single Gain/Loss is determined (Usually 1 minute)"),
	m_SpinRsiMhPeriodCount(50, 2, 14, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Number of Periods (Minutes) for which Gains and Losses are counted (Usually 14)"),
	m_SpinRsiMhStartMinute(1200, 240, 570, 10, 1, true, true, 0, false, "The time from which Today's RSI is calculated"),
	m_CheckBoxRsiMhPostMarket("Use Post Market data to calculate Today's RSI", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_pressedCheckColor),

	m_CheckBoxRsiDaysCalculate("Calculate Historical RSI (Relative Strength Index)", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), m_enabledTextColor, m_pressedCheckColor),
	m_SpinRsiDaysPriorPointCount(250, 0, 250, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Number of Prior Points to Initialize Historical RSI"),
	m_SpinRsiDaysPeriodLength(20, 1, 1, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Period Length for which a single Gain/Loss is determined (Usually 1 day)"),
	m_SpinRsiDaysPeriodCount(50, 2, 14, 1000, 3, TakionSpin::charComma, 0, TakionSpin::charDot, 1, true, true, false, 0, true, "Number of Periods (Days) for which Gains and Losses are counted (Usually 14)"),
	m_SpinRsiDaysStartMinute(1200, 240, 570, 10, 1, true, true, 0, false, "Today's time from which Historical RSI is calculated"),
	m_CheckBoxRsiDaysPostMarket("Use Post Market data in previous days trading to calculate Historical RSI", 0xFFFFFFFF, 0xFFFFFFFF, GetSysColor(COLOR_WINDOW), 0xFFFFFFFF, m_pressedCheckColor),

	m_rsiMh(rsiMhGroupId, rsiMhUseId, rsiMhSpinMinId, rsiMhSpinMaxId, 10000, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Market Hours RSI", true, m_visualBase->GetBlueTextColor()),
	m_rsiDays(rsiDaysFilterGroupId, rsiDaysFilterUseId, rsiDaysFilterSpinMinId, rsiDaysFilterSpinMaxId, 10000, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by Historical RSI", true, m_visualBase->GetBlueTextColor()),
*/
	m_luldDiff(luldDiffGroupId, luldDiffUseId, luldDiffSpinMinId, luldDiffSpinMaxId, luldDiffPositiveId, luldDiffNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference LU - LastNbboPrintPrice (if LU is checked) or LastNbboPrintPrice - LD (if LD is checked)"),
	m_luldDiffPercent(luldDiffPercentGroupId, luldDiffPercentUseId, luldDiffPercentSpinMinId, luldDiffPercentSpinMaxId, luldDiffPercentPositiveId, luldDiffPercentNegativeId, 999999, 3, 2, m_disabledColor, m_enabledColor, m_invalidColor, "Filter by the Difference LU - LastNbboPrintPrice (if LU is checked) or LastNbboPrintPrice - LD (if LD is checked) as percentage of LastNbboPrintPrice"),

//	m_calculateRsiMh(false),
//	m_calculateRsiDays(false),
	m_fourthColumnBottom(0),
	m_fifthColumnBottom(0),
	m_sixthColumnBottom(0),
	m_seventhColumnBottom(0),
	m_eighthColumnBottom(0),
	m_timeFrameHeight(vtitlegroupgap + vgroupgap + 4 * (checkBoxHeight + vgap) + spinHeight),
	m_leftMostColumnBottom(0),
	m_leftMostColumnBottomPixel(0),
	m_leftMostColumnLeftPixel(0),
	m_leftMostColumnWidthPixel(0),
	m_staticHeightPixel(0),

	m_fifthColumnLeftPixel(0),
	m_fifthColumnWidthPixel(0),
	m_fifthColumnBottomPixel(0),

	m_marketCategoryVisibleItemCount(MCF_COUNT),
	m_testVisibleItemCount(2),
	m_securityTypeVisibleItemCount(8),
	m_htbVisibleItemCount(3),
	m_tradingStateVisibleItemCount(TSF_Count),
	m_quoteConditionVisibleItemCount(5)
{

	m_ListBoxTestOrReal.AddStringItem("Real", 0);
	m_ListBoxTestOrReal.AddStringItem("Test", 1);

	m_ListBoxSecurityType.AddStringItem("Undefined", STYPE_UNDEFINED - DialogStockFilter::zeroChar);
	m_ListBoxSecurityType.AddStringItem("ETF", STYPE_ETF - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("ADR", STYPE_ADR - DialogStockFilter::zeroChar);

#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ListBoxSecurityType.AddStringItem("(Preferred)", STYPE_PREFERRED_STOCK - DialogStockFilter::zeroChar);
#endif
	m_ListBoxSecurityType.AddStringItem("Common", STYPE_COMMON_STOCK - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("Debentures", STYPE_DEBENTURES - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("ETN", STYPE_ETFE - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("Foreign", STYPE_FOREIGN - DialogStockFilter::zeroChar);

#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ListBoxSecurityType.AddStringItem("(Structured)", STYPE_STRUCTURED_PRODUCT - DialogStockFilter::zeroChar);
#endif

	m_ListBoxSecurityType.AddStringItem("US Depository", STYPE_US_DEPOSITARY_SHARES - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("Units", STYPE_UNITS - DialogStockFilter::zeroChar);

#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ListBoxSecurityType.AddStringItem("(ADR Warrant)", STYPE_ADR_WARRANT - DialogStockFilter::zeroChar);
	m_ListBoxSecurityType.AddStringItem("(ADR Right)", STYPE_ADR_RIGHT - DialogStockFilter::zeroChar);
#endif
	m_ListBoxSecurityType.AddStringItem("Index Note", STYPE_INDEX_LINKED_NOTES - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("Liquid Trust", STYPE_MISC_LIQUID_TRUST - DialogStockFilter::zeroChar);

#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ListBoxSecurityType.AddStringItem("(GlobalRegShare)", STYPE_GLOBAL_REGISTERED_SHARE - DialogStockFilter::zeroChar);
#endif

	m_ListBoxSecurityType.AddStringItem("Ordinary", STYPE_ORDINARY_SHARES - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("Preferred", STYPE_PREF_STOCK - DialogStockFilter::zeroChar);

#ifdef PILLAR_MIGRATION_NOT_COMPLETE
	m_ListBoxSecurityType.AddStringItem("(Basket)", STYPE_BASKET - DialogStockFilter::zeroChar);
#endif

	m_ListBoxSecurityType.AddStringItem("Right", STYPE_RIGHTS - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("Benefit Interest", STYPE_SHARES_OF_BENEFICIARY_INTEREST - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("Test", STYPE_TEST - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("Cls End Fund", STYPE_CLOSED_END_FUND - DialogStockFilter::zeroChar);

	m_ListBoxSecurityType.AddStringItem("Warrant", STYPE_WARRANT - DialogStockFilter::zeroChar);

	const QuoteCondition* const* qcArray = TU_GetPackedQuoteConditions(true);
	const QuoteCondition* qc = *qcArray;
	for(; qc; qc = *++qcArray)
	{
		m_ListBoxNasdaqQc.AddStringItem(qc->GetId(), qc->GetId() - 'A', qc->GetDescription().c_str());
	}
	m_ListBoxNasdaqQc.AddStringItem("Other", 'Z' - 'A' + 1, "Any Unknown Quote Condition");

	qcArray = TU_GetPackedQuoteConditions(false);
	qc = *qcArray;
	for(; qc; qc = *++qcArray)
	{
		m_ListBoxListedQc.AddStringItem(qc->GetId(), qc->GetId() - 'A', qc->GetDescription().c_str());
	}
	m_ListBoxListedQc.AddStringItem("Other", 'Z' - 'A' + 1, "Any Unknown Quote Condition");

	m_ListBoxHtb.AddStringItem("ETB", HTB_ETB, "Easy to borrow");
	m_ListBoxHtb.AddStringItem("HTB", HTB_HTB, "Hard to borrow (Inventory available)");
	m_ListBoxHtb.AddStringItem("htb", HTB_htb, "Hard to borrow (No Inventory)");

	m_ListBoxTradingState.AddStringItem('T', TSF_T, "Trading");
	m_ListBoxTradingState.AddStringItem('H', TSF_H, "Halted");
	m_ListBoxTradingState.AddStringItem('E', TSF_E, "E");
	m_ListBoxTradingState.AddStringItem('3', TSF_3, "3");
	m_ListBoxTradingState.AddStringItem('2', TSF_2, "2");
	m_ListBoxTradingState.AddStringItem('1', TSF_1, "1");
	m_ListBoxTradingState.AddStringItem('P', TSF_P, "P");
	m_ListBoxTradingState.AddStringItem("Other", TSF_OTHER, "Other");

	m_ListBoxCircuitBreaker.AddStringItem('E', CBF_E, "Circuit Breaker");
	m_ListBoxCircuitBreaker.AddStringItem('3', CBF_3, "Circuit Breaker");
	m_ListBoxCircuitBreaker.AddStringItem('2', CBF_2, "Circuit Breaker");
	m_ListBoxCircuitBreaker.AddStringItem('1', CBF_1, "Circuit Breaker");
	m_ListBoxCircuitBreaker.AddStringItem('0', CBF_0, "Normal");
	m_ListBoxCircuitBreaker.AddStringItem('D', CBF_D, "Normal");
	m_ListBoxCircuitBreaker.AddStringItem(' ', CBF_SPACE, "Normal");
	m_ListBoxCircuitBreaker.AddStringItem("Other", CBF_OTHER, "Other");

	const int itemHeight = 8;

	const int borderSize = 10;//2;

	int top = topMost + m_timeFrameHeight + hgap;//vgap;

	const int marketCategoryLbHeight = itemHeight * m_marketCategoryVisibleItemCount + borderSize;

	AddStaticIndirect("Exchange", WS_VISIBLE | SS_NOPREFIX, 0, firstColumnOffset, top, firstColumnWidth, staticHeight, exchangeStaticId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0, firstColumnOffset,
		top += staticHeight, firstColumnWidth, marketCategoryLbHeight, exchangeListBoxId);

	m_leftMostColumnBottom = top + marketCategoryLbHeight;

	top = m_leftMostColumnBottom + vgap;
	const int testLbHeight = itemHeight * m_testVisibleItemCount + borderSize;

	AddStaticIndirect("Test Stock", WS_VISIBLE | SS_NOPREFIX, 0, firstColumnOffset, top, firstColumnWidth, staticHeight, testStaticId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0, firstColumnOffset,
		top += staticHeight, firstColumnWidth, testLbHeight, testListBoxId);

	m_leftMostColumnBottom = top + testLbHeight;

	top = m_leftMostColumnBottom + vgap;
	const int securityLbHeight = itemHeight * m_securityTypeVisibleItemCount + borderSize;

	AddStaticIndirect("SecurityType", WS_VISIBLE | SS_NOPREFIX, 0, firstColumnOffset, top, firstColumnWidth, staticHeight, securityTypeStaticId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0, firstColumnOffset,
		top += staticHeight, firstColumnWidth, securityLbHeight, securityTypeListBoxId);

	m_leftMostColumnBottom = top + securityLbHeight;
	top = m_leftMostColumnBottom + vgap;

	const int borrowLbHeight = itemHeight * m_htbVisibleItemCount + borderSize;

	AddStaticIndirect("Borrow", WS_VISIBLE | SS_NOPREFIX, 0, firstColumnOffset, top, firstColumnWidth, staticHeight, htbStaticId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0, firstColumnOffset,
		top += staticHeight, firstColumnWidth, borrowLbHeight, htbListBoxId);

	m_leftMostColumnBottom = top + borrowLbHeight;

	int x_yVolume = firstColumnOffset + firstColumnWidth + hgap + (groupWidth + hgap);
	if(useTimeFrame)
	{
		x_yVolume += groupWidth + hgap;
	}

	int y1 = topMost;
	AddRangeComponent(x_yVolume, y1, groupWidth, "Today's Px Mh Vol", m_todaysPxMhVolume);//yVolumeGroupId, yVolumeUseId, yVolumeSpinMinId, yVolumeSpinMaxId);
	AddRangeComponent(x_yVolume, y1 += groupHeight + vgap, groupWidth, "Todays Volume", m_todaysVolume);//yVolumeGroupId, yVolumeUseId, yVolumeSpinMinId, yVolumeSpinMaxId);
	AddRangeComponent(x_yVolume, y1 += groupHeight + vgap, groupWidth, "Borrow Price (cents)", m_borrowPrice);
	AddRangeComponent(x_yVolume, y1 += groupHeight + vgap, groupWidth, "PMI Spread", m_pmiSpread);
	AddRangeComponent(x_yVolume, y1 += groupHeight + vgap, groupWidth, "Today's Close Price", m_todaysClosePrice);

	int x_imbalance = x_yVolume + groupWidth + hgap;
	y1 = topMost;

	AddRangeComponent(x_imbalance, y1, groupWidth, "NYSE Imbalance", m_imbalanceNyse);
	AddRangeComponent(x_imbalance, y1 += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "NYSERegImbalance", m_imbalanceNyseReg);
	AddRangeComponent(x_imbalance, y1 += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "NSDQ Imbalance", m_imbalanceNsdq);
	AddRangeComponent(x_imbalance, y1 += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "ARCA Imbalance", m_imbalanceArca);
	AddRangeComponent(x_imbalance, y1 += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "EXCH Imbalance", m_imbalanceExch);
	AddRangeComponent(x_imbalance, y1 += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "Rate Imbalance", m_rateImbalance);
	AddRangeComponent(x_imbalance, y1 += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "NYSE Imb Delta", m_imbNyDeltaInf);
	AddRangeComponent(x_imbalance, y1 += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "Delta Pv", m_deltaPv);

	m_fourthColumnBottom = y1 + groupHeight + vgap;

	int x_yesterday = x_imbalance + groupWidth + hgap;
	y1 = topMost;
	AddRangeComponent(x_yesterday, y1, groupWidth, "Yesterdays Volume", m_yesterdaysVolume);
	AddRangeComponent(x_yesterday, y1 += groupHeight + vgap, groupWidth, "AvgDaily Volume", m_avgDailyVolume);
	AddRangeComponent(x_yesterday, y1 += groupHeight + vgap, groupWidth, "Relative Volume", m_relativeVolume);
	AddRangeComponent(x_yesterday, y1 += groupHeight + vgap, groupWidth, "%PreMhVol/ADV", m_preMhVolRel);
	AddRangeComponent(x_yesterday, y1 += groupHeight + vgap, groupWidth, "Yest Price Range", m_yesterdaysPriceRange);
	AddRangeComponent(x_yesterday, y1 += groupHeight + vgap, groupWidth, "%Yest Price Range", m_yesterdaysPriceRangePercent);

	AddRangeComponent(x_yesterday, y1 += groupHeight + vgap, groupWidth, "From LULD", m_luldDiff, "LU", "LD");
	AddRangeComponent(x_yesterday, y1 += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "% From LULD", m_luldDiffPercent, "LU", "LD");

	m_fifthColumnBottom = y1 + groupHeight + vgap + checkBoxHeight + vgap;
	int lby = m_fifthColumnBottom + staticHeight;
	const int groupWidth2 = groupWidth / 2;
	const int tradingStateListBoxHeight = checkBoxHeight * m_tradingStateVisibleItemCount + 1;//vgap;
	AddStaticIndirect("TradingState", WS_VISIBLE | SS_NOPREFIX, 0, x_yesterday, m_fifthColumnBottom, groupWidth2, staticHeight, tradingStateStaticId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0,
		x_yesterday, lby, groupWidth2, tradingStateListBoxHeight, tradingStateListBoxId);

	const int x2 = x_yesterday + groupWidth2;
	AddStaticIndirect("CBreaker", WS_VISIBLE | SS_NOPREFIX, 0, x2, m_fifthColumnBottom, groupWidth2, staticHeight, circuitBreakerStaticId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0,
		x2, lby, groupWidth2, tradingStateListBoxHeight, circuitBreakerListBoxId);

	m_fifthColumnBottom = lby + tradingStateListBoxHeight + vgap;
	lby = m_fifthColumnBottom + staticHeight;
	const int qcListBoxHeight = checkBoxHeight * m_quoteConditionVisibleItemCount + borderSize;
	AddStaticIndirect("Nasdaq QC", WS_VISIBLE | SS_NOPREFIX, 0, x_yesterday, m_fifthColumnBottom, groupWidth2, staticHeight, nasdaqQcStaticId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_SORT | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0,
		x_yesterday, lby, groupWidth2, qcListBoxHeight, nasdaqQcListBoxId);

	AddStaticIndirect("Listed QC", WS_VISIBLE | SS_NOPREFIX, 0, x2, m_fifthColumnBottom, groupWidth2, staticHeight, listedQcStaticId);
	AddListBoxIndirect(nullptr, WS_VISIBLE | WS_TABSTOP | WS_BORDER | LBS_SORT | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL, 0,
		x2, lby, groupWidth2, qcListBoxHeight, listedQcListBoxId);

	m_fifthColumnBottom = lby + qcListBoxHeight + vgap;

	const int x_net = x_yesterday + groupWidth + hgap;
	m_sixthColumnBottom = topMost;
	AddRangeComponent(x_net, m_sixthColumnBottom, groupWidth, "Bid Net", m_bidNet);//yVolumeGroupId, yVolumeUseId, yVolumeSpinMinId, yVolumeSpinMaxId);
	AddRangeComponent(x_net, m_sixthColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "Ask Net", m_askNet);//yVolumeGroupId, yVolumeUseId, yVolumeSpinMinId, yVolumeSpinMaxId);

	AddRangeComponent(x_net, m_sixthColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "TBid Net", m_tBidNet);
	AddRangeComponent(x_net, m_sixthColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "TAsk Net", m_tAskNet);

	AddRangeComponent(x_net, m_sixthColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "Spread", m_spread);

	m_sixthColumnBottom += groupHeight + vgap + checkBoxHeight + vgap;

	const int x_netPercent = x_net + groupWidth + hgap;
	m_seventhColumnBottom = topMost;
	AddRangeComponent(x_netPercent, m_seventhColumnBottom, groupWidth, "% Bid Net", m_bidNetPercent);//yVolumeGroupId, yVolumeUseId, yVolumeSpinMinId, yVolumeSpinMaxId);
	AddRangeComponent(x_netPercent, m_seventhColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "% Ask Net", m_askNetPercent);//yVolumeGroupId, yVolumeUseId, yVolumeSpinMinId, yVolumeSpinMaxId);

	AddRangeComponent(x_netPercent, m_seventhColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "% TBid Net", m_tBidNetPercent);
	AddRangeComponent(x_netPercent, m_seventhColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "% TAsk Net", m_tAskNetPercent);

	m_seventhColumnBottom += groupHeight + vgap + checkBoxHeight + vgap;
/*
	m_SpinRsiMhPriorPointCount.AddSpinCell(1, false);
	m_SpinRsiMhPriorPointCount.AddSpinCell(100, true);

	m_SpinRsiMhPeriodLength.AddSpinCell(1, false);
	m_SpinRsiMhPeriodCount.AddSpinCell(1, false);

	m_SpinRsiDaysPriorPointCount.AddSpinCell(1, false);
	m_SpinRsiDaysPriorPointCount.AddSpinCell(100, true);

	m_SpinRsiDaysPeriodLength.AddSpinCell(1, false);
	m_SpinRsiDaysPeriodCount.AddSpinCell(1, false);

	const int vgap1 = 1;
	int rsiGroupHeight = groupHeight + 2 * (checkBoxHeight + vgap) + 2 * spinHeight + 5 * vgap1;
	const int x_control = x_netPercent + hgap;
	const int controlWidth = groupWidth - hgap - hgap;

	int minuteWidth = controlWidth / 2 - hgap;//spinWidth * 2;
	const int spinWidth = minuteWidth - staticHeight;//controlWidth / 2 - hgap - staticHeight;

	minuteWidth += staticHeight / 2;
	const int minuteNameWidth = controlWidth - minuteWidth - 1;
	const int x_minute = x_control + minuteWidth + 1;

//RSI Minutes
	const int x_name = x_control + spinWidth + 1;
	const int nameWidth = controlWidth - spinWidth - 1;
	const int yOffset = (spinHeight - staticHeight) / 2;
	int groupTop = m_seventhColumnBottom + vgap;

	AddButtonIndirect("RSI Minutes Calculation", WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP, WS_EX_TRANSPARENT, x_netPercent, groupTop, groupWidth, rsiGroupHeight, rsiMarketHoursCalculationGroupId);
	AddButtonIndirect("Calculate", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, x_control, groupTop += vtitlegroupgap, controlWidth, checkBoxHeight, rsiMarketHoursCheckBoxCalculateId);
	AddButtonIndirect("Today Only", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, x_control, groupTop += checkBoxHeight + vgap, controlWidth, checkBoxHeight, rsiMarketHoursCheckBoxTodayOnlyId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += checkBoxHeight + vgap, minuteWidth, spinHeight, rsiMarketHoursSpinPriorPointCountId);
	AddStaticIndirect("Prior Points", WS_VISIBLE | SS_NOPREFIX, 0, x_minute, groupTop + yOffset, minuteNameWidth, staticHeight, rsiMarketHoursStaticPriorPointCountId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += spinHeight + vgap1, spinWidth, spinHeight, rsiMarketHoursSpinPeriodLengthId);
	AddStaticIndirect("Period Length", WS_VISIBLE | SS_NOPREFIX, 0, x_name, groupTop + yOffset, nameWidth, staticHeight, rsiMarketHoursStaticPeriodLengthId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += spinHeight + vgap1, spinWidth, spinHeight, rsiMarketHoursSpinPeriodCountId);
	AddStaticIndirect("Period Count", WS_VISIBLE | SS_NOPREFIX, 0, x_name, groupTop + yOffset, nameWidth, staticHeight, rsiMarketHoursStaticPeriodCountId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += spinHeight + vgap1, minuteWidth, spinHeight, rsiMarketHoursSpinStartMinuteId);
	AddStaticIndirect("Start Time", WS_VISIBLE | SS_NOPREFIX, 0, x_minute, groupTop + yOffset, minuteNameWidth, staticHeight, rsiMarketHoursStaticStartMinuteId);
	AddButtonIndirect("Post Market", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, x_control, groupTop += spinHeight + vgap1, controlWidth, checkBoxHeight, rsiMarketHoursCheckBoxPostMarketId);

	AddRangeComponent(x_netPercent, m_seventhColumnBottom += rsiGroupHeight, groupWidth, "RSI Minutes Filter", m_rsiMh);
	m_seventhColumnBottom += groupHeight + vgap;

//RSI Days
	rsiGroupHeight -= (checkBoxHeight + vgap);
	groupTop = m_seventhColumnBottom + vgap;
	AddButtonIndirect("RSI Daily Calculation", WS_CHILD|WS_VISIBLE|BS_GROUPBOX|WS_GROUP, WS_EX_TRANSPARENT, x_netPercent, groupTop, groupWidth, rsiGroupHeight, rsiDaysCalculationGroupId);
	AddButtonIndirect("Calculate", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, x_control, groupTop += vtitlegroupgap, controlWidth, checkBoxHeight, rsiDaysCheckBoxCalculateId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += checkBoxHeight + vgap, minuteWidth, spinHeight, rsiDaysSpinPriorPointCountId);
	AddStaticIndirect("Prior Points", WS_VISIBLE | SS_NOPREFIX, 0, x_minute, groupTop + yOffset, minuteNameWidth, staticHeight, rsiDaysStaticPriorPointCountId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += spinHeight + vgap1, spinWidth, spinHeight, rsiDaysSpinPeriodLengthId);
	AddStaticIndirect("Period Length", WS_VISIBLE | SS_NOPREFIX, 0, x_name, groupTop + yOffset, nameWidth, staticHeight, rsiDaysStaticPeriodLengthId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += spinHeight + vgap1, spinWidth, spinHeight, rsiDaysSpinPeriodCountId);
	AddStaticIndirect("Period Count", WS_VISIBLE | SS_NOPREFIX, 0, x_name, groupTop + yOffset, nameWidth, staticHeight, rsiDaysStaticPeriodCountId);
	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x_control, groupTop += spinHeight + vgap1, minuteWidth, spinHeight, rsiDaysSpinStartMinuteId);
	AddStaticIndirect("Start Time", WS_VISIBLE | SS_NOPREFIX, 0, x_minute, groupTop + yOffset, minuteNameWidth, staticHeight, rsiDaysStaticStartMinuteId);
	AddButtonIndirect("Post Market", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_GROUP|WS_CLIPCHILDREN|WS_TABSTOP, 0, x_control, groupTop += spinHeight + vgap1, controlWidth, checkBoxHeight, rsiDaysCheckBoxPostMarketId);

	AddRangeComponent(x_netPercent, m_seventhColumnBottom += rsiGroupHeight, groupWidth, "RSI Daily Filter", m_rsiDays);

	m_seventhColumnBottom += groupHeight + vgap;
*/
	const int x_imb = x_netPercent + groupWidth + hgap;
	m_eighthColumnBottom = topMost;

	AddRangeComponent(x_imb, m_eighthColumnBottom, groupWidth, "EXCH Near", m_imbExNear);
	AddRangeComponent(x_imb, m_eighthColumnBottom += groupHeight + vgap, groupWidth, "EXCH Far", m_imbExFar);
	AddRangeComponent(x_imb, m_eighthColumnBottom += groupHeight + vgap, groupWidth, "EXCH Far Delta", m_imbExFarPriceDelta);
	AddRangeComponent(x_imb, m_eighthColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "NSDQ Far Delta", m_imbNqFarPriceDelta);
	AddRangeComponent(x_imb, m_eighthColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "Imb NSDQ Thru", m_imbNqThru);
	AddRangeComponent(x_imb, m_eighthColumnBottom += groupHeight + vgap + checkBoxHeight + vgap, groupWidth, "EXCH Imb Match", m_imbExMatch);

	AddRangeComponent(x_imb, m_eighthColumnBottom += groupHeight + vgap, groupWidth, "% ImbEX / DayVol", m_imbExVolPercent);
	AddRangeComponent(x_imb, m_eighthColumnBottom += groupHeight + vgap, groupWidth, "% ImbEX / YestVol", m_imbExYestVolPercent);

	AddRangeComponent(x_imb, m_eighthColumnBottom += groupHeight + vgap, groupWidth, "ImbExValue", m_imbExValue);

	m_eighthColumnBottom += groupHeight + vgap + checkBoxHeight + vgap;
}

BEGIN_MESSAGE_MAP(MyDialogStockFilter, MyDialogPriceFilter)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(yVolumeUseId, OnUseYesterdaysVolume)
	ON_BN_CLICKED(avgDailyVolumeUseId, OnUseAvgDailyVolume)
	ON_BN_CLICKED(relativeVolumeUseId, OnUseRelativeVolume)
	ON_BN_CLICKED(preMhVolRelUseId, OnUsePreMhVolRel)
	ON_BN_CLICKED(yPriceRangeUseId, OnUseYesterdaysPriceRange)
	ON_BN_CLICKED(yPriceRangePercentUseId, OnUseYesterdaysPriceRangePercent)
	ON_BN_CLICKED(tVolumeUseId, OnUseTodaysVolume)
	ON_BN_CLICKED(tPxMhVolumeUseId, OnUseTodaysPxMhVolume)
	ON_BN_CLICKED(imbalanceNyseUseId, OnUseImbalanceNyse)
	ON_BN_CLICKED(imbalanceNyseRegUseId, OnUseImbalanceNyseReg)
	ON_BN_CLICKED(imbalanceNsdqUseId, OnUseImbalanceNsdq)
	ON_BN_CLICKED(imbalanceArcaUseId, OnUseImbalanceArca)
	ON_BN_CLICKED(imbalanceExchUseId, OnUseImbalanceExch)
	ON_BN_CLICKED(rateImbalanceUseId, OnUseRateImbalance)
	ON_BN_CLICKED(imbNyDeltaInfUseId, OnUseImbNyDeltaInf)
	ON_BN_CLICKED(imbExFarPriceDeltaUseId, OnUseImbExFarPriceDelta)
	ON_BN_CLICKED(imbNqFarPriceDeltaUseId, OnUseImbNqFarPriceDelta)
	ON_BN_CLICKED(imbNqThruUseId, OnUseImbNqThru)
	ON_BN_CLICKED(imbExValueUseId, OnUseImbExValue)
	ON_BN_CLICKED(imbExNearUseId, OnUseImbExNear)
	ON_BN_CLICKED(imbExFarUseId, OnUseImbExFar)
	ON_BN_CLICKED(imbExMatchUseId, OnUseImbExMatch)
	ON_BN_CLICKED(imbExVolPercentUseId, OnUseImbExVolPercent)
	ON_BN_CLICKED(imbExYestVolPercentUseId, OnUseImbExYestVolPercent)

	ON_BN_CLICKED(deltaPvUseId, OnUseDeltaPv)
//	ON_BN_CLICKED(rsiMhUseId, OnUseRsiMh)
//	ON_BN_CLICKED(rsiDaysFilterUseId, OnUseRsiDays)
	ON_BN_CLICKED(borrowPriceUseId, OnUseBorrowPrice)
	ON_BN_CLICKED(pmiSpreadUseId, OnUsePmiSpread)
	ON_BN_CLICKED(todaysClosePriceUseId, OnUseTodaysClosePrice)

	ON_BN_CLICKED(bidNetUseId, OnUseBidNet)
	ON_BN_CLICKED(askNetUseId, OnUseAskNet)

	ON_BN_CLICKED(tBidNetUseId, OnUseTBidNet)
	ON_BN_CLICKED(tAskNetUseId, OnUseTAskNet)

	ON_BN_CLICKED(spreadUseId, OnUseSpread)

	ON_BN_CLICKED(bidNetPercentUseId, OnUseBidNetPercent)
	ON_BN_CLICKED(askNetPercentUseId, OnUseAskNetPercent)

	ON_BN_CLICKED(tBidNetPercentUseId, OnUseTBidNetPercent)
	ON_BN_CLICKED(tAskNetPercentUseId, OnUseTAskNetPercent)

	ON_BN_CLICKED(luldDiffUseId, OnUseLuldDiff)
	ON_BN_CLICKED(luldDiffPositiveId, OnPositiveLuldDiff)//OnLuldDiffLu)luldDiffPercentPositiveId
	ON_BN_CLICKED(luldDiffNegativeId, OnNegativeLuldDiff)

	ON_BN_CLICKED(luldDiffPercentUseId, OnUseLuldDiffPercent)
	ON_BN_CLICKED(luldDiffPercentPositiveId, OnPositiveLuldDiffPercent)//OnLuldDiffLuPercent)
	ON_BN_CLICKED(luldDiffPercentNegativeId, OnNegativeLuldDiffPercent)

	ON_BN_CLICKED(imbalanceNysePositiveId, OnPositiveImbalanceNyse)
	ON_BN_CLICKED(imbalanceNyseNegativeId, OnNegativeImbalanceNyse)
	ON_BN_CLICKED(imbalanceNyseRegPositiveId, OnPositiveImbalanceNyseReg)
	ON_BN_CLICKED(imbalanceNyseRegNegativeId, OnNegativeImbalanceNyseReg)
	ON_BN_CLICKED(imbalanceNsdqPositiveId, OnPositiveImbalanceNsdq)
	ON_BN_CLICKED(imbalanceNsdqNegativeId, OnNegativeImbalanceNsdq)
	ON_BN_CLICKED(imbalanceArcaPositiveId, OnPositiveImbalanceArca)
	ON_BN_CLICKED(imbalanceArcaNegativeId, OnNegativeImbalanceArca)
	ON_BN_CLICKED(imbalanceExchPositiveId, OnPositiveImbalanceExch)
	ON_BN_CLICKED(imbalanceExchNegativeId, OnNegativeImbalanceExch)
	ON_BN_CLICKED(rateImbalancePositiveId, OnPositiveRateImbalance)
	ON_BN_CLICKED(rateImbalanceNegativeId, OnNegativeRateImbalance)
	ON_BN_CLICKED(imbNyDeltaInfPositiveId, OnPositiveImbNyDeltaInf)
	ON_BN_CLICKED(imbNyDeltaInfNegativeId, OnNegativeImbNyDeltaInf)

	ON_BN_CLICKED(imbExFarPriceDeltaPositiveId, OnPositiveImbExFarPriceDelta)
	ON_BN_CLICKED(imbExFarPriceDeltaNegativeId, OnNegativeImbExFarPriceDelta)

	ON_BN_CLICKED(imbNqFarPriceDeltaPositiveId, OnPositiveImbNqFarPriceDelta)
	ON_BN_CLICKED(imbNqFarPriceDeltaNegativeId, OnNegativeImbNqFarPriceDelta)

	ON_BN_CLICKED(imbNqThruPositiveId, OnPositiveImbNqThru)
	ON_BN_CLICKED(imbNqThruNegativeId, OnNegativeImbNqThru)

	ON_BN_CLICKED(imbExValuePositiveId, OnPositiveImbExValue)
	ON_BN_CLICKED(imbExValueNegativeId, OnNegativeImbExValue)

	ON_BN_CLICKED(bidNetPositiveId, OnPositiveBidNet)
	ON_BN_CLICKED(bidNetNegativeId, OnNegativeBidNet)
	ON_BN_CLICKED(askNetPositiveId, OnPositiveAskNet)
	ON_BN_CLICKED(askNetNegativeId, OnNegativeAskNet)

	ON_BN_CLICKED(tBidNetPositiveId, OnPositiveTBidNet)
	ON_BN_CLICKED(tBidNetNegativeId, OnNegativeTBidNet)
	ON_BN_CLICKED(tAskNetPositiveId, OnPositiveTAskNet)
	ON_BN_CLICKED(tAskNetNegativeId, OnNegativeTAskNet)

	ON_BN_CLICKED(spreadPositiveId, OnPositiveSpread)
	ON_BN_CLICKED(spreadNegativeId, OnNegativeSpread)

	ON_BN_CLICKED(bidNetPercentPositiveId, OnPositiveBidNetPercent)
	ON_BN_CLICKED(bidNetPercentNegativeId, OnNegativeBidNetPercent)
	ON_BN_CLICKED(askNetPercentPositiveId, OnPositiveAskNetPercent)
	ON_BN_CLICKED(askNetPercentNegativeId, OnNegativeAskNetPercent)

	ON_BN_CLICKED(tBidNetPercentPositiveId, OnPositiveTBidNetPercent)
	ON_BN_CLICKED(tBidNetPercentNegativeId, OnNegativeTBidNetPercent)
	ON_BN_CLICKED(tAskNetPercentPositiveId, OnPositiveTAskNetPercent)
	ON_BN_CLICKED(tAskNetPercentNegativeId, OnNegativeTAskNetPercent)
/*
	ON_BN_CLICKED(rsiMarketHoursCheckBoxCalculateId, OnRsiMhCalculate)
	ON_BN_CLICKED(rsiMarketHoursCheckBoxTodayOnlyId, OnRsiMhTodayOnly)
	ON_BN_CLICKED(rsiMarketHoursCheckBoxPostMarketId, OnRsiMhPostMarket)

	ON_BN_CLICKED(rsiDaysCheckBoxCalculateId, OnRsiDaysCalculate)
	ON_BN_CLICKED(rsiDaysCheckBoxPostMarketId, OnRsiDaysPostMarket)
*/
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
	ON_MESSAGE(WM_USER + 17, OnListBoxChecked)
	ON_MESSAGE(WM_USER + 18, OnListBoxChecked)//OnListBoxRangeChecked)
END_MESSAGE_MAP()
/*
void MyDialogStockFilter::OnRsiMhCalculate()
{
	const bool calculate = m_CheckBoxRsiMhCalculate.GetCheck() == BST_CHECKED;
	if(calculate != m_calculateRsiMh)
	{
		m_calculateRsiMh = calculate;
		if(calculate != ((const MyStockFilter*)m_filter)->isRsiMhCalculate())DoEnableOkButton(true);
		else EnableOkButton();
		if(calculate)DoEnableClearButton(true);
		else EnableClearButton();

		const COLORREF color = calculate ? m_visualBase->GetOkColor() : m_visualBase->GetLightGrayColor();
		m_SpinRsiMhPriorPointCount.SetBkColorEnabled(color);
		m_SpinRsiMhPeriodLength.SetBkColorEnabled(color);
		m_SpinRsiMhPeriodCount.SetBkColorEnabled(color);
		m_SpinRsiMhStartMinute.SetBkColorEnabled(color);

		m_CheckBoxRsiMhTodayOnly.SetInvalid(!calculate);
		m_CheckBoxRsiMhPostMarket.SetInvalid(!calculate);
	}
}

void MyDialogStockFilter::OnRsiMhPostMarket()
{
	const bool postMarket = m_CheckBoxRsiMhPostMarket.GetCheck() == BST_CHECKED;
	if(postMarket != ((const MyStockFilter*)m_filter)->isRsiMhPostMarket())DoEnableOkButton(true);
	else EnableOkButton();
	if(postMarket)DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnRsiMhTodayOnly()
{
	const bool todayOnly = m_CheckBoxRsiMhTodayOnly.GetCheck() == BST_CHECKED;
	if(todayOnly != ((const MyStockFilter*)m_filter)->isRsiMhTodayOnly())DoEnableOkButton(true);
	else EnableOkButton();
	if(todayOnly)DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnRsiDaysCalculate()
{
	const bool calculate = m_CheckBoxRsiDaysCalculate.GetCheck() == BST_CHECKED;
	if(calculate != m_calculateRsiDays)
	{
		m_calculateRsiDays = calculate;
		if(calculate != ((const MyStockFilter*)m_filter)->isRsiDaysCalculate())DoEnableOkButton(true);
		else EnableOkButton();
		if(calculate)DoEnableClearButton(true);
		else EnableClearButton();

		const COLORREF color = calculate ? m_visualBase->GetOkColor() : m_visualBase->GetLightGrayColor();
		m_SpinRsiDaysPriorPointCount.SetBkColorEnabled(color);
		m_SpinRsiDaysPeriodLength.SetBkColorEnabled(color);
		m_SpinRsiDaysPeriodCount.SetBkColorEnabled(color);
		m_SpinRsiDaysStartMinute.SetBkColorEnabled(color);

		m_CheckBoxRsiDaysPostMarket.SetInvalid(!calculate);
	}
}

void MyDialogStockFilter::OnRsiDaysPostMarket()
{
	const bool postMarket = m_CheckBoxRsiDaysPostMarket.GetCheck() == BST_CHECKED;
	if(postMarket != ((const MyStockFilter*)m_filter)->isRsiDaysPostMarket())DoEnableOkButton(true);
	else EnableOkButton();
	if(postMarket)DoEnableClearButton(true);
	else EnableClearButton();
}
*/
void MyDialogStockFilter::OnPositiveImbalanceNyse()
{
	if(m_imbalanceNyse.TogglePositive())
	{
		if(m_imbalanceNyse.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNyse.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbalanceNyse()
{
	if(m_imbalanceNyse.ToggleNegative())
	{
		if(m_imbalanceNyse.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNyse.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveImbalanceNyseReg()
{
	if(m_imbalanceNyseReg.TogglePositive())
	{
		if(m_imbalanceNyseReg.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRegRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNyseReg.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbalanceNyseReg()
{
	if(m_imbalanceNyseReg.ToggleNegative())
	{
		if(m_imbalanceNyseReg.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRegRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNyseReg.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveImbalanceNsdq()
{
	if(m_imbalanceNsdq.TogglePositive())
	{
		if(m_imbalanceNsdq.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceNsdqRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNsdq.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbalanceNsdq()
{
	if(m_imbalanceNsdq.ToggleNegative())
	{
		if(m_imbalanceNsdq.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceNsdqRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNsdq.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveRateImbalance()
{
	if(m_rateImbalance.TogglePositive())
	{
		if(m_rateImbalance.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetRateImbalanceRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_rateImbalance.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeRateImbalance()
{
	if(m_rateImbalance.ToggleNegative())
	{
		if(m_rateImbalance.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetRateImbalanceRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_rateImbalance.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveImbNyDeltaInf()
{
	if(m_imbNyDeltaInf.TogglePositive())
	{
		if(m_imbNyDeltaInf.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbNyDeltaInfRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNyDeltaInf.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbNyDeltaInf()
{
	if(m_imbNyDeltaInf.ToggleNegative())
	{
		if(m_imbNyDeltaInf.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbNyDeltaInfRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNyDeltaInf.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveImbExFarPriceDelta()
{
	if(m_imbExFarPriceDelta.TogglePositive())
	{
		if(m_imbExFarPriceDelta.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbExFarPriceDeltaRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbExFarPriceDelta()
{
	if(m_imbExFarPriceDelta.ToggleNegative())
	{
		if(m_imbExFarPriceDelta.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbExFarPriceDeltaRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveImbNqFarPriceDelta()
{
	if(m_imbNqFarPriceDelta.TogglePositive())
	{
		if(m_imbNqFarPriceDelta.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbNqFarPriceDeltaRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNqFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbNqFarPriceDelta()
{
	if(m_imbNqFarPriceDelta.ToggleNegative())
	{
		if(m_imbNqFarPriceDelta.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbNqFarPriceDeltaRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNqFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveImbNqThru()
{
	if(m_imbNqThru.TogglePositive())
	{
		if(m_imbNqThru.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbNqThruRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNqThru.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbNqThru()
{
	if(m_imbNqThru.ToggleNegative())
	{
		if(m_imbNqThru.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbNqThruRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNqThru.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveImbExValue()
{
	if(m_imbExValue.TogglePositive())
	{
		if(m_imbExValue.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbExValueRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExValue.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbExValue()
{
	if(m_imbExValue.ToggleNegative())
	{
		if(m_imbExValue.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbExValueRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExValue.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveBidNet()
{
	if(m_bidNet.TogglePositive())
	{
		if(m_bidNet.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetBidNetRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_bidNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeBidNet()
{
	if(m_bidNet.ToggleNegative())
	{
		if(m_bidNet.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetBidNetRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_bidNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveAskNet()
{
	if(m_askNet.TogglePositive())
	{
		if(m_askNet.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetAskNetRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_askNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeAskNet()
{
	if(m_askNet.ToggleNegative())
	{
		if(m_askNet.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetAskNetRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_askNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveTBidNet()
{
	if(m_tBidNet.TogglePositive())
	{
		if(m_tBidNet.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetTBidNetRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tBidNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeTBidNet()
{
	if(m_tBidNet.ToggleNegative())
	{
		if(m_tBidNet.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetTBidNetRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tBidNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveTAskNet()
{
	if(m_tAskNet.TogglePositive())
	{
		if(m_tAskNet.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetTAskNetRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tAskNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeTAskNet()
{
	if(m_tAskNet.ToggleNegative())
	{
		if(m_tAskNet.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetTAskNetRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tAskNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveSpread()
{
	if(m_spread.TogglePositive())
	{
		if(m_spread.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetSpreadRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_spread.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeSpread()
{
	if(m_spread.ToggleNegative())
	{
		if(m_spread.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetSpreadRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_spread.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveBidNetPercent()
{
	if(m_bidNetPercent.TogglePositive())
	{
		if(m_bidNetPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetBidNetPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_bidNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeBidNetPercent()
{
	if(m_bidNetPercent.ToggleNegative())
	{
		if(m_bidNetPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetBidNetPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_bidNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveAskNetPercent()
{
	if(m_askNetPercent.TogglePositive())
	{
		if(m_askNetPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetAskNetPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_askNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeAskNetPercent()
{
	if(m_askNetPercent.ToggleNegative())
	{
		if(m_askNetPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetAskNetPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_askNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveTBidNetPercent()
{
	if(m_tBidNetPercent.TogglePositive())
	{
		if(m_tBidNetPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetTBidNetPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tBidNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeTBidNetPercent()
{
	if(m_tBidNetPercent.ToggleNegative())
	{
		if(m_tBidNetPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetTBidNetPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tBidNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveTAskNetPercent()
{
	if(m_tAskNetPercent.TogglePositive())
	{
		if(m_tAskNetPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetTAskNetPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tAskNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeTAskNetPercent()
{
	if(m_tAskNetPercent.ToggleNegative())
	{
		if(m_tAskNetPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetTAskNetPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tAskNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveImbalanceArca()
{
	if(m_imbalanceArca.TogglePositive())
	{
		if(m_imbalanceArca.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceArcaRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceArca.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbalanceArca()
{
	if(m_imbalanceArca.ToggleNegative())
	{
		if(m_imbalanceArca.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceArcaRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceArca.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnPositiveImbalanceExch()
{
	if(m_imbalanceExch.TogglePositive())
	{
		if(m_imbalanceExch.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceExchRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceExch.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeImbalanceExch()
{
	if(m_imbalanceExch.ToggleNegative())
	{
		if(m_imbalanceExch.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetImbalanceExchRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceExch.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

LRESULT MyDialogStockFilter::OnListBoxChecked(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case exchangeListBoxId:
		m_ListBoxMarketCategory.InvalidateClient();
		if(((const MyStockFilter*)m_filter)->GetMarketCategoryFilter() != m_ListBoxMarketCategory.GetFilter())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_ListBoxMarketCategory.AreAllItemsChecked() != 2)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case testListBoxId:
		m_ListBoxTestOrReal.InvalidateClient();
		if(((const MyStockFilter*)m_filter)->GetTestStockFilter() != m_ListBoxTestOrReal.GetFilter())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_ListBoxTestOrReal.AreAllItemsChecked() != 2)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case securityTypeListBoxId:
		m_ListBoxSecurityType.InvalidateClient();
		if(((const MyStockFilter*)m_filter)->GetSecurityTypeFilter() != m_ListBoxSecurityType.GetFilter())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_ListBoxSecurityType.AreAllItemsChecked() != 2)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case htbListBoxId:
		m_ListBoxHtb.InvalidateClient();
		if(((const MyStockFilter*)m_filter)->GetHtbFilter() != m_ListBoxHtb.GetFilter())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_ListBoxHtb.AreAllItemsChecked() != 2)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tradingStateListBoxId:
		m_ListBoxTradingState.InvalidateClient();
		if(((const MyStockFilter*)m_filter)->GetTradingStateFilter() != m_ListBoxTradingState.GetFilter())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_ListBoxTradingState.AreAllItemsChecked() != 2)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case circuitBreakerListBoxId:
		m_ListBoxCircuitBreaker.InvalidateClient();
		if(((const MyStockFilter*)m_filter)->GetCircuitBreakerFilter() != m_ListBoxCircuitBreaker.GetFilter())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_ListBoxCircuitBreaker.AreAllItemsChecked() != 2)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case nasdaqQcListBoxId:
		m_ListBoxNasdaqQc.InvalidateClient();
		if(((const MyStockFilter*)m_filter)->GetNasdaqQuoteConditionFilter() != m_ListBoxNasdaqQc.GetFilter())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_ListBoxNasdaqQc.AreAllItemsChecked() != 2)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case listedQcListBoxId:
		m_ListBoxListedQc.InvalidateClient();
		if(((const MyStockFilter*)m_filter)->GetListedQuoteConditionFilter() != m_ListBoxListedQc.GetFilter())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_ListBoxListedQc.AreAllItemsChecked() != 2)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		default:
		MyDialogPriceFilter::OnListBoxChecked(w, l);
		break;
	}
	return 0;
}

void MyDialogStockFilter::OnUseYesterdaysVolume()
{
	m_yesterdaysVolume.UpdateUse();
	if(m_yesterdaysVolume.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetYesterdaysVolumeRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_yesterdaysVolume.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseAvgDailyVolume()
{
	m_avgDailyVolume.UpdateUse();
	if(m_avgDailyVolume.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetAvgDailyVolumeRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_avgDailyVolume.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseRelativeVolume()
{
	m_relativeVolume.UpdateUse();
	if(m_relativeVolume.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetRelativeVolumeRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_relativeVolume.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUsePreMhVolRel()
{
	m_preMhVolRel.UpdateUse();
	if(m_preMhVolRel.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetPreMhVolRelRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_preMhVolRel.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseYesterdaysPriceRange()
{
	m_yesterdaysPriceRange.UpdateUse();
	if(m_yesterdaysPriceRange.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetYesterdaysPriceRangeRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_yesterdaysPriceRange.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseYesterdaysPriceRangePercent()
{
	m_yesterdaysPriceRangePercent.UpdateUse();
	if(m_yesterdaysPriceRangePercent.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetYesterdaysPriceRangePercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_yesterdaysPriceRangePercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseTodaysVolume()
{
	m_todaysVolume.UpdateUse();
	if(m_todaysVolume.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetTodaysVolumeRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_todaysVolume.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseTodaysPxMhVolume()
{
	m_todaysPxMhVolume.UpdateUse();
	if(m_todaysPxMhVolume.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetTodaysPxMhVolumeRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_todaysPxMhVolume.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbalanceNyse()
{
	m_imbalanceNyse.UpdateUse();
	if(m_imbalanceNyse.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbalanceNyse.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbalanceNyseReg()
{
	m_imbalanceNyseReg.UpdateUse();
	if(m_imbalanceNyseReg.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRegRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbalanceNyseReg.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseRateImbalance()
{
	m_rateImbalance.UpdateUse();
	if(m_rateImbalance.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetRateImbalanceRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_rateImbalance.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbNyDeltaInf()
{
	m_imbNyDeltaInf.UpdateUse();
	if(m_imbNyDeltaInf.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbNyDeltaInfRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbNyDeltaInf.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbExFarPriceDelta()
{
	m_imbExFarPriceDelta.UpdateUse();
	if(m_imbExFarPriceDelta.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbExFarPriceDeltaRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbExFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbNqFarPriceDelta()
{
	m_imbNqFarPriceDelta.UpdateUse();
	if(m_imbNqFarPriceDelta.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbNqFarPriceDeltaRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbNqFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbNqThru()
{
	m_imbNqThru.UpdateUse();
	if(m_imbNqThru.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbNqThruRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbNqThru.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbExValue()
{
	m_imbExValue.UpdateUse();
	if(m_imbExValue.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbExValueRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbExValue.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseBidNet()
{
	m_bidNet.UpdateUse();
	if(m_bidNet.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetBidNetRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_bidNet.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseAskNet()
{
	m_askNet.UpdateUse();
	if(m_askNet.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetAskNetRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_askNet.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseTBidNet()
{
	m_tBidNet.UpdateUse();
	if(m_tBidNet.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetTBidNetRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_tBidNet.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseTAskNet()
{
	m_tAskNet.UpdateUse();
	if(m_tAskNet.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetTAskNetRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_tAskNet.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseSpread()
{
	m_spread.UpdateUse();
	if(m_spread.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetSpreadRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_spread.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseBidNetPercent()
{
	m_bidNetPercent.UpdateUse();
	if(m_bidNetPercent.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetBidNetPercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_bidNetPercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseAskNetPercent()
{
	m_askNetPercent.UpdateUse();
	if(m_askNetPercent.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetAskNetPercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_askNetPercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseTBidNetPercent()
{
	m_tBidNetPercent.UpdateUse();
	if(m_tBidNetPercent.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetTBidNetPercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_tBidNetPercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseTAskNetPercent()
{
	m_tAskNetPercent.UpdateUse();
	if(m_tAskNetPercent.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetTAskNetPercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_tAskNetPercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseLuldDiff()
{
	m_luldDiff.UpdateUse();
	if(m_luldDiff.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetLuldRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_luldDiff.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnPositiveLuldDiff()
{
	if(m_luldDiff.TogglePositive())
	{
		if(m_luldDiff.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetLuldRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_luldDiff.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeLuldDiff()
{
	if(m_luldDiff.ToggleNegative())
	{
		if(m_luldDiff.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetLuldRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_luldDiff.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnUseLuldDiffPercent()
{
	m_luldDiffPercent.UpdateUse();
	if(m_luldDiffPercent.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetLuldPercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_luldDiffPercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnPositiveLuldDiffPercent()
{
	if(m_luldDiffPercent.TogglePositive())
	{
		if(m_luldDiffPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetLuldPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_luldDiffPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnNegativeLuldDiffPercent()
{
	if(m_luldDiffPercent.ToggleNegative())
	{
		if(m_luldDiffPercent.isPositiveNegativeModified(((const MyStockFilter*)m_filter)->GetLuldPercentRange()))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_luldDiffPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
	}
}

void MyDialogStockFilter::OnUseDeltaPv()
{
	m_deltaPv.UpdateUse();
	if(m_deltaPv.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetDeltaPvRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_deltaPv.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}
/*
void MyDialogStockFilter::OnUseRsiMh()
{
	m_rsiMh.UpdateUse();
	if(m_rsiMh.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetRsiMhRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_rsiMh.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseRsiDays()
{
	m_rsiDays.UpdateUse();
	if(m_rsiDays.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetRsiDaysRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_rsiDays.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}
*/
void MyDialogStockFilter::OnUseBorrowPrice()
{
	m_borrowPrice.UpdateUse();
	if(m_borrowPrice.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetBorrowPriceRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_borrowPrice.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUsePmiSpread()
{
	m_pmiSpread.UpdateUse();
	if(m_pmiSpread.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetPmiSpreadRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_pmiSpread.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbExNear()
{
	m_imbExNear.UpdateUse();
	if(m_imbExNear.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbExNearRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbExNear.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbExFar()
{
	m_imbExFar.UpdateUse();
	if(m_imbExFar.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbExFarRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbExFar.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbExMatch()
{
	m_imbExMatch.UpdateUse();
	if(m_imbExMatch.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbExMatchRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbExMatch.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbExVolPercent()
{
	m_imbExVolPercent.UpdateUse();
	if(m_imbExVolPercent.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbExVolPercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbExVolPercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbExYestVolPercent()
{
	m_imbExYestVolPercent.UpdateUse();
	if(m_imbExYestVolPercent.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbExYestVolPercentRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbExYestVolPercent.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseTodaysClosePrice()
{
	m_todaysClosePrice.UpdateUse();
	if(m_todaysClosePrice.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetTodaysClosePriceRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_todaysClosePrice.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbalanceNsdq()
{
	m_imbalanceNsdq.UpdateUse();
	if(m_imbalanceNsdq.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbalanceNsdqRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbalanceNsdq.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbalanceArca()
{
	m_imbalanceArca.UpdateUse();
	if(m_imbalanceArca.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbalanceArcaRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbalanceArca.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

void MyDialogStockFilter::OnUseImbalanceExch()
{
	m_imbalanceExch.UpdateUse();
	if(m_imbalanceExch.isRangeBaseModified(((const MyStockFilter*)m_filter)->GetImbalanceExchRange()))DoEnableOkButton(true);
	else EnableOkButton();
	if(!m_imbalanceExch.isRangeEmpty())DoEnableClearButton(true);
	else EnableClearButton();
}

LRESULT MyDialogStockFilter::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case yVolumeSpinMinId:
		m_yesterdaysVolume.UpdateRangeValid();
		if(m_yesterdaysVolume.isValueModified(((const MyStockFilter*)m_filter)->GetYesterdaysVolumeRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_yesterdaysVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case yVolumeSpinMaxId:
		m_yesterdaysVolume.UpdateRangeValid();
		if(m_yesterdaysVolume.isValueModified(((const MyStockFilter*)m_filter)->GetYesterdaysVolumeRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_yesterdaysVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case avgDailyVolumeSpinMinId:
		m_avgDailyVolume.UpdateRangeValid();
		if(m_avgDailyVolume.isValueModified(((const MyStockFilter*)m_filter)->GetAvgDailyVolumeRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_avgDailyVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case avgDailyVolumeSpinMaxId:
		m_avgDailyVolume.UpdateRangeValid();
		if(m_avgDailyVolume.isValueModified(((const MyStockFilter*)m_filter)->GetAvgDailyVolumeRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_avgDailyVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case relativeVolumeSpinMinId:
		m_relativeVolume.UpdateRangeValid();
		if(m_relativeVolume.isValueModified(((const MyStockFilter*)m_filter)->GetRelativeVolumeRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_relativeVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case relativeVolumeSpinMaxId:
		m_relativeVolume.UpdateRangeValid();
		if(m_relativeVolume.isValueModified(((const MyStockFilter*)m_filter)->GetRelativeVolumeRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_relativeVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case preMhVolRelSpinMinId:
		m_preMhVolRel.UpdateRangeValid();
		if(m_preMhVolRel.isValueModified(((const MyStockFilter*)m_filter)->GetPreMhVolRelRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_preMhVolRel.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case preMhVolRelSpinMaxId:
		m_preMhVolRel.UpdateRangeValid();
		if(m_preMhVolRel.isValueModified(((const MyStockFilter*)m_filter)->GetPreMhVolRelRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_preMhVolRel.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case yPriceRangeSpinMinId:
		m_yesterdaysPriceRange.UpdateRangeValid();
		if(m_yesterdaysPriceRange.isValueModified(((const MyStockFilter*)m_filter)->GetYesterdaysPriceRangeRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_yesterdaysPriceRange.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case yPriceRangeSpinMaxId:
		m_yesterdaysPriceRange.UpdateRangeValid();
		if(m_yesterdaysPriceRange.isValueModified(((const MyStockFilter*)m_filter)->GetYesterdaysPriceRangeRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_yesterdaysPriceRange.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case yPriceRangePercentSpinMinId:
		m_yesterdaysPriceRangePercent.UpdateRangeValid();
		if(m_yesterdaysPriceRangePercent.isValueModified(((const MyStockFilter*)m_filter)->GetYesterdaysPriceRangePercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_yesterdaysPriceRangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case yPriceRangePercentSpinMaxId:
		m_yesterdaysPriceRangePercent.UpdateRangeValid();
		if(m_yesterdaysPriceRangePercent.isValueModified(((const MyStockFilter*)m_filter)->GetYesterdaysPriceRangePercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_yesterdaysPriceRangePercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tVolumeSpinMinId:
		m_todaysVolume.UpdateRangeValid();
		if(m_todaysVolume.isValueModified(((const MyStockFilter*)m_filter)->GetTodaysVolumeRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_todaysVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tVolumeSpinMaxId:
		m_todaysVolume.UpdateRangeValid();
		if(m_todaysVolume.isValueModified(((const MyStockFilter*)m_filter)->GetTodaysVolumeRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_todaysVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tPxMhVolumeSpinMinId:
		m_todaysPxMhVolume.UpdateRangeValid();
		if(m_todaysPxMhVolume.isValueModified(((const MyStockFilter*)m_filter)->GetTodaysPxMhVolumeRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_todaysPxMhVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tPxMhVolumeSpinMaxId:
		m_todaysPxMhVolume.UpdateRangeValid();
		if(m_todaysPxMhVolume.isValueModified(((const MyStockFilter*)m_filter)->GetTodaysPxMhVolumeRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_todaysPxMhVolume.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceNyseSpinMinId:
		m_imbalanceNyse.UpdateRangeValid();
		if(m_imbalanceNyse.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNyse.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceNyseSpinMaxId:
		m_imbalanceNyse.UpdateRangeValid();
		if(m_imbalanceNyse.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNyse.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceNyseRegSpinMinId:
		m_imbalanceNyseReg.UpdateRangeValid();
		if(m_imbalanceNyseReg.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRegRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNyseReg.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceNyseRegSpinMaxId:
		m_imbalanceNyseReg.UpdateRangeValid();
		if(m_imbalanceNyseReg.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceNyseRegRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNyseReg.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceNsdqSpinMinId:
		m_imbalanceNsdq.UpdateRangeValid();
		if(m_imbalanceNsdq.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceNsdqRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNsdq.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceNsdqSpinMaxId:
		m_imbalanceNsdq.UpdateRangeValid();
		if(m_imbalanceNsdq.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceNsdqRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceNsdq.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceArcaSpinMinId:
		m_imbalanceArca.UpdateRangeValid();
		if(m_imbalanceArca.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceArcaRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceArca.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceArcaSpinMaxId:
		m_imbalanceArca.UpdateRangeValid();
		if(m_imbalanceArca.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceArcaRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceArca.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceExchSpinMinId:
		m_imbalanceExch.UpdateRangeValid();
		if(m_imbalanceExch.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceExchRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceExch.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbalanceExchSpinMaxId:
		m_imbalanceExch.UpdateRangeValid();
		if(m_imbalanceExch.isValueModified(((const MyStockFilter*)m_filter)->GetImbalanceExchRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbalanceExch.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rateImbalanceSpinMinId:
		m_rateImbalance.UpdateRangeValid();
		if(m_rateImbalance.isValueModified(((const MyStockFilter*)m_filter)->GetRateImbalanceRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_rateImbalance.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rateImbalanceSpinMaxId:
		m_rateImbalance.UpdateRangeValid();
		if(m_rateImbalance.isValueModified(((const MyStockFilter*)m_filter)->GetRateImbalanceRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_rateImbalance.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbNyDeltaInfSpinMinId:
		m_imbNyDeltaInf.UpdateRangeValid();
		if(m_imbNyDeltaInf.isValueModified(((const MyStockFilter*)m_filter)->GetImbNyDeltaInfRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNyDeltaInf.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbNyDeltaInfSpinMaxId:
		m_imbNyDeltaInf.UpdateRangeValid();
		if(m_imbNyDeltaInf.isValueModified(((const MyStockFilter*)m_filter)->GetImbNyDeltaInfRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNyDeltaInf.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExFarPriceDeltaSpinMinId:
		m_imbExFarPriceDelta.UpdateRangeValid();
		if(m_imbExFarPriceDelta.isValueModified(((const MyStockFilter*)m_filter)->GetImbExFarPriceDeltaRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExFarPriceDeltaSpinMaxId:
		m_imbExFarPriceDelta.UpdateRangeValid();
		if(m_imbExFarPriceDelta.isValueModified(((const MyStockFilter*)m_filter)->GetImbExFarPriceDeltaRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbNqFarPriceDeltaSpinMinId:
		m_imbNqFarPriceDelta.UpdateRangeValid();
		if(m_imbNqFarPriceDelta.isValueModified(((const MyStockFilter*)m_filter)->GetImbNqFarPriceDeltaRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNqFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbNqFarPriceDeltaSpinMaxId:
		m_imbNqFarPriceDelta.UpdateRangeValid();
		if(m_imbNqFarPriceDelta.isValueModified(((const MyStockFilter*)m_filter)->GetImbNqFarPriceDeltaRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNqFarPriceDelta.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbNqThruSpinMinId:
		m_imbNqThru.UpdateRangeValid();
		if(m_imbNqThru.isValueModified(((const MyStockFilter*)m_filter)->GetImbNqThruRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNqThru.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbNqThruSpinMaxId:
		m_imbNqThru.UpdateRangeValid();
		if(m_imbNqThru.isValueModified(((const MyStockFilter*)m_filter)->GetImbNqThruRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbNqThru.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExValueSpinMinId:
		m_imbExValue.UpdateRangeValid();
		if(m_imbExValue.isValueModified(((const MyStockFilter*)m_filter)->GetImbExValueRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExValue.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExValueSpinMaxId:
		m_imbExValue.UpdateRangeValid();
		if(m_imbExValue.isValueModified(((const MyStockFilter*)m_filter)->GetImbExValueRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExValue.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case bidNetSpinMinId:
		m_bidNet.UpdateRangeValid();
		if(m_bidNet.isValueModified(((const MyStockFilter*)m_filter)->GetBidNetRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_bidNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case bidNetSpinMaxId:
		m_bidNet.UpdateRangeValid();
		if(m_bidNet.isValueModified(((const MyStockFilter*)m_filter)->GetBidNetRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_bidNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case askNetSpinMinId:
		m_askNet.UpdateRangeValid();
		if(m_askNet.isValueModified(((const MyStockFilter*)m_filter)->GetAskNetRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_askNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case askNetSpinMaxId:
		m_askNet.UpdateRangeValid();
		if(m_askNet.isValueModified(((const MyStockFilter*)m_filter)->GetAskNetRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_askNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tBidNetSpinMinId:
		m_tBidNet.UpdateRangeValid();
		if(m_tBidNet.isValueModified(((const MyStockFilter*)m_filter)->GetTBidNetRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tBidNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tBidNetSpinMaxId:
		m_tBidNet.UpdateRangeValid();
		if(m_tBidNet.isValueModified(((const MyStockFilter*)m_filter)->GetTBidNetRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tBidNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tAskNetSpinMinId:
		m_tAskNet.UpdateRangeValid();
		if(m_tAskNet.isValueModified(((const MyStockFilter*)m_filter)->GetTAskNetRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tAskNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tAskNetSpinMaxId:
		m_tAskNet.UpdateRangeValid();
		if(m_tAskNet.isValueModified(((const MyStockFilter*)m_filter)->GetTAskNetRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tAskNet.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case spreadSpinMinId:
		m_spread.UpdateRangeValid();
		if(m_spread.isValueModified(((const MyStockFilter*)m_filter)->GetSpreadRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_spread.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case spreadSpinMaxId:
		m_spread.UpdateRangeValid();
		if(m_spread.isValueModified(((const MyStockFilter*)m_filter)->GetSpreadRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_spread.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case bidNetPercentSpinMinId:
		m_bidNetPercent.UpdateRangeValid();
		if(m_bidNetPercent.isValueModified(((const MyStockFilter*)m_filter)->GetBidNetPercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_bidNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case bidNetPercentSpinMaxId:
		m_bidNetPercent.UpdateRangeValid();
		if(m_bidNetPercent.isValueModified(((const MyStockFilter*)m_filter)->GetBidNetPercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_bidNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case askNetPercentSpinMinId:
		m_askNetPercent.UpdateRangeValid();
		if(m_askNetPercent.isValueModified(((const MyStockFilter*)m_filter)->GetAskNetPercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_askNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case askNetPercentSpinMaxId:
		m_askNetPercent.UpdateRangeValid();
		if(m_askNetPercent.isValueModified(((const MyStockFilter*)m_filter)->GetAskNetPercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_askNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tBidNetPercentSpinMinId:
		m_tBidNetPercent.UpdateRangeValid();
		if(m_tBidNetPercent.isValueModified(((const MyStockFilter*)m_filter)->GetTBidNetPercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tBidNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tBidNetPercentSpinMaxId:
		m_tBidNetPercent.UpdateRangeValid();
		if(m_tBidNetPercent.isValueModified(((const MyStockFilter*)m_filter)->GetTBidNetPercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tBidNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tAskNetPercentSpinMinId:
		m_tAskNetPercent.UpdateRangeValid();
		if(m_tAskNetPercent.isValueModified(((const MyStockFilter*)m_filter)->GetTAskNetPercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tAskNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case tAskNetPercentSpinMaxId:
		m_tAskNetPercent.UpdateRangeValid();
		if(m_tAskNetPercent.isValueModified(((const MyStockFilter*)m_filter)->GetTAskNetPercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_tAskNetPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case luldDiffSpinMinId:
		m_luldDiff.UpdateRangeValid();
		if(m_luldDiff.isValueModified(((const MyStockFilter*)m_filter)->GetLuldRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_luldDiff.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case luldDiffSpinMaxId:
		m_luldDiff.UpdateRangeValid();
		if(m_luldDiff.isValueModified(((const MyStockFilter*)m_filter)->GetLuldRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_luldDiff.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case luldDiffPercentSpinMinId:
		m_luldDiffPercent.UpdateRangeValid();
		if(m_luldDiffPercent.isValueModified(((const MyStockFilter*)m_filter)->GetLuldPercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_luldDiffPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case luldDiffPercentSpinMaxId:
		m_luldDiffPercent.UpdateRangeValid();
		if(m_luldDiffPercent.isValueModified(((const MyStockFilter*)m_filter)->GetLuldPercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_luldDiffPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case deltaPvSpinMinId:
		m_deltaPv.UpdateRangeValid();
		if(m_deltaPv.isValueModified(((const MyStockFilter*)m_filter)->GetDeltaPvRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_deltaPv.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case deltaPvSpinMaxId:
		m_deltaPv.UpdateRangeValid();
		if(m_deltaPv.isValueModified(((const MyStockFilter*)m_filter)->GetDeltaPvRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_deltaPv.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;
/*
		case rsiMhSpinMinId:
		m_rsiMh.UpdateRangeValid();
		if(m_rsiMh.isValueModified(((const MyStockFilter*)m_filter)->GetRsiMhRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_rsiMh.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rsiMhSpinMaxId:
		m_rsiMh.UpdateRangeValid();
		if(m_rsiMh.isValueModified(((const MyStockFilter*)m_filter)->GetRsiMhRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_rsiMh.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rsiDaysFilterSpinMinId:
		m_rsiDays.UpdateRangeValid();
		if(m_rsiDays.isValueModified(((const MyStockFilter*)m_filter)->GetRsiDaysRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_rsiDays.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rsiDaysFilterSpinMaxId:
		m_rsiDays.UpdateRangeValid();
		if(m_rsiDays.isValueModified(((const MyStockFilter*)m_filter)->GetRsiDaysRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_rsiDays.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;
*/
		case borrowPriceSpinMinId:
		m_borrowPrice.UpdateRangeValid();
		if(m_borrowPrice.isValueModified(((const MyStockFilter*)m_filter)->GetBorrowPriceRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_borrowPrice.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case borrowPriceSpinMaxId:
		m_borrowPrice.UpdateRangeValid();
		if(m_borrowPrice.isValueModified(((const MyStockFilter*)m_filter)->GetBorrowPriceRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_borrowPrice.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case pmiSpreadSpinMinId:
		m_pmiSpread.UpdateRangeValid();
		if(m_pmiSpread.isValueModified(((const MyStockFilter*)m_filter)->GetPmiSpreadRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_pmiSpread.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case pmiSpreadSpinMaxId:
		m_pmiSpread.UpdateRangeValid();
		if(m_pmiSpread.isValueModified(((const MyStockFilter*)m_filter)->GetPmiSpreadRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_pmiSpread.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExNearSpinMinId:
		m_imbExNear.UpdateRangeValid();
		if(m_imbExNear.isValueModified(((const MyStockFilter*)m_filter)->GetImbExNearRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExNear.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExNearSpinMaxId:
		m_imbExNear.UpdateRangeValid();
		if(m_imbExNear.isValueModified(((const MyStockFilter*)m_filter)->GetImbExNearRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExNear.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExFarSpinMinId:
		m_imbExFar.UpdateRangeValid();
		if(m_imbExFar.isValueModified(((const MyStockFilter*)m_filter)->GetImbExFarRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExFar.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExFarSpinMaxId:
		m_imbExFar.UpdateRangeValid();
		if(m_imbExFar.isValueModified(((const MyStockFilter*)m_filter)->GetImbExFarRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExFar.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExMatchSpinMinId:
		m_imbExMatch.UpdateRangeValid();
		if(m_imbExMatch.isValueModified(((const MyStockFilter*)m_filter)->GetImbExMatchRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExMatch.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExMatchSpinMaxId:
		m_imbExMatch.UpdateRangeValid();
		if(m_imbExMatch.isValueModified(((const MyStockFilter*)m_filter)->GetImbExMatchRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExMatch.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExVolPercentSpinMinId:
		m_imbExVolPercent.UpdateRangeValid();
		if(m_imbExVolPercent.isValueModified(((const MyStockFilter*)m_filter)->GetImbExVolPercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExVolPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExVolPercentSpinMaxId:
		m_imbExVolPercent.UpdateRangeValid();
		if(m_imbExVolPercent.isValueModified(((const MyStockFilter*)m_filter)->GetImbExVolPercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExVolPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExYestVolPercentSpinMinId:
		m_imbExYestVolPercent.UpdateRangeValid();
		if(m_imbExYestVolPercent.isValueModified(((const MyStockFilter*)m_filter)->GetImbExYestVolPercentRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExYestVolPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case imbExYestVolPercentSpinMaxId:
		m_imbExYestVolPercent.UpdateRangeValid();
		if(m_imbExYestVolPercent.isValueModified(((const MyStockFilter*)m_filter)->GetImbExYestVolPercentRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_imbExYestVolPercent.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case todaysClosePriceSpinMinId:
		m_todaysClosePrice.UpdateRangeValid();
		if(m_todaysClosePrice.isValueModified(((const MyStockFilter*)m_filter)->GetTodaysClosePriceRange(), true))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_todaysClosePrice.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case todaysClosePriceSpinMaxId:
		m_todaysClosePrice.UpdateRangeValid();
		if(m_todaysClosePrice.isValueModified(((const MyStockFilter*)m_filter)->GetTodaysClosePriceRange(), false))DoEnableOkButton(true);
		else EnableOkButton();
		if(!m_todaysClosePrice.isRangeEmpty())DoEnableClearButton(true);
		else EnableClearButton();
		break;
/*
		case rsiMarketHoursSpinPriorPointCountId:
		if(m_SpinRsiMhPriorPointCount.GetValue() != ((const MyStockFilter*)m_filter)->GetRsiMhPriorPointCount())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_SpinRsiMhPriorPointCount.GetValue() != 250)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rsiMarketHoursSpinPeriodLengthId:
		if(m_SpinRsiMhPeriodLength.GetValue() != ((const MyStockFilter*)m_filter)->GetRsiMhPeriodLength())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_SpinRsiMhPeriodLength.GetValue() != 1)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rsiMarketHoursSpinPeriodCountId:
		if(m_SpinRsiMhPeriodCount.GetValue() != ((const MyStockFilter*)m_filter)->GetRsiMhPeriodCount())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_SpinRsiMhPeriodCount.GetValue() != 14)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rsiMarketHoursSpinStartMinuteId:
		if(m_SpinRsiMhStartMinute.GetMinute() != ((const MyStockFilter*)m_filter)->GetRsiMhStartMinute())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_SpinRsiMhStartMinute.GetMinute() != 570)DoEnableClearButton(true);
		else EnableClearButton();
		break;
//RSI Days
		case rsiDaysSpinPriorPointCountId:
		if(m_SpinRsiDaysPriorPointCount.GetValue() != ((const MyStockFilter*)m_filter)->GetRsiDaysPriorPointCount())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_SpinRsiDaysPriorPointCount.GetValue() != 250)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rsiDaysSpinPeriodLengthId:
		if(m_SpinRsiDaysPeriodLength.GetValue() != ((const MyStockFilter*)m_filter)->GetRsiDaysPeriodLength())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_SpinRsiDaysPeriodLength.GetValue() != 1)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rsiDaysSpinPeriodCountId:
		if(m_SpinRsiDaysPeriodCount.GetValue() != ((const MyStockFilter*)m_filter)->GetRsiDaysPeriodCount())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_SpinRsiDaysPeriodCount.GetValue() != 14)DoEnableClearButton(true);
		else EnableClearButton();
		break;

		case rsiDaysSpinStartMinuteId:
		if(m_SpinRsiDaysStartMinute.GetMinute() != ((const MyStockFilter*)m_filter)->GetRsiDaysStartMinute())DoEnableOkButton(true);
		else EnableOkButton();
		if(m_SpinRsiDaysStartMinute.GetMinute() != 570)DoEnableClearButton(true);
		else EnableClearButton();
		break;
*/
		default:
		MyDialogPriceFilter::OnSpinChange(w, l);
		break;
	}

	return 0;
}

void MyDialogStockFilter::ToolTipsEnabled(bool enable)
{
	MyDialogPriceFilter::ToolTipsEnabled(enable);

	m_yesterdaysVolume.EnableToolTips(enable, m_toolTip);
	m_todaysVolume.EnableToolTips(enable, m_toolTip);
	m_todaysPxMhVolume.EnableToolTips(enable, m_toolTip);
	m_avgDailyVolume.EnableToolTips(enable, m_toolTip);
	m_relativeVolume.EnableToolTips(enable, m_toolTip);
	m_preMhVolRel.EnableToolTips(enable, m_toolTip);
	m_yesterdaysPriceRange.EnableToolTips(enable, m_toolTip);
	m_yesterdaysPriceRangePercent.EnableToolTips(enable, m_toolTip);

	m_imbalanceNyse.EnableToolTips(enable, m_toolTip);
	m_imbalanceNyseReg.EnableToolTips(enable, m_toolTip);
	m_imbalanceNsdq.EnableToolTips(enable, m_toolTip);
	m_imbalanceArca.EnableToolTips(enable, m_toolTip);
	m_imbalanceExch.EnableToolTips(enable, m_toolTip);
	m_rateImbalance.EnableToolTips(enable, m_toolTip);
	m_imbNyDeltaInf.EnableToolTips(enable, m_toolTip);
	m_imbExFarPriceDelta.EnableToolTips(enable, m_toolTip);
	m_imbNqFarPriceDelta.EnableToolTips(enable, m_toolTip);
	m_imbNqThru.EnableToolTips(enable, m_toolTip);
	m_imbExValue.EnableToolTips(enable, m_toolTip);

	m_imbExNear.EnableToolTips(enable, m_toolTip);
	m_imbExFar.EnableToolTips(enable, m_toolTip);
	m_imbExMatch.EnableToolTips(enable, m_toolTip);
	m_imbExVolPercent.EnableToolTips(enable, m_toolTip);
	m_imbExYestVolPercent.EnableToolTips(enable, m_toolTip);
	m_deltaPv.EnableToolTips(enable, m_toolTip);
	m_borrowPrice.EnableToolTips(enable, m_toolTip);
	m_pmiSpread.EnableToolTips(enable, m_toolTip);
	m_todaysClosePrice.EnableToolTips(enable, m_toolTip);

	m_bidNet.EnableToolTips(enable, m_toolTip);
	m_askNet.EnableToolTips(enable, m_toolTip);

	m_tBidNet.EnableToolTips(enable, m_toolTip);
	m_tAskNet.EnableToolTips(enable, m_toolTip);

	m_spread.EnableToolTips(enable, m_toolTip);

	m_bidNetPercent.EnableToolTips(enable, m_toolTip);
	m_askNetPercent.EnableToolTips(enable, m_toolTip);

	m_tBidNetPercent.EnableToolTips(enable, m_toolTip);
	m_tAskNetPercent.EnableToolTips(enable, m_toolTip);
/*
	m_rsiMh.EnableToolTips(enable, m_toolTip);
	m_CheckBoxRsiMhCalculate.EnableTooltips(enable, m_toolTip);
	m_CheckBoxRsiMhTodayOnly.EnableTooltips(enable, m_toolTip);
	m_SpinRsiMhPriorPointCount.EnableTooltips(enable, m_toolTip);
	m_SpinRsiMhPeriodLength.EnableTooltips(enable, m_toolTip);
	m_SpinRsiMhPeriodCount.EnableTooltips(enable, m_toolTip);
	m_SpinRsiMhStartMinute.EnableTooltips(enable, m_toolTip);
	m_CheckBoxRsiMhPostMarket.EnableTooltips(enable, m_toolTip);

	m_rsiDays.EnableToolTips(enable, m_toolTip);
	m_CheckBoxRsiDaysCalculate.EnableTooltips(enable, m_toolTip);
	m_SpinRsiDaysPriorPointCount.EnableTooltips(enable, m_toolTip);
	m_SpinRsiDaysPeriodLength.EnableTooltips(enable, m_toolTip);
	m_SpinRsiDaysPeriodCount.EnableTooltips(enable, m_toolTip);
	m_SpinRsiDaysStartMinute.EnableTooltips(enable, m_toolTip);
	m_CheckBoxRsiDaysPostMarket.EnableTooltips(enable, m_toolTip);
*/
	m_luldDiff.EnableToolTips(enable, m_toolTip);
	m_luldDiffPercent.EnableToolTips(enable, m_toolTip);

	m_ListBoxMarketCategory.EnableTooltips(enable, m_toolTip);
	m_ListBoxTestOrReal.EnableTooltips(enable, m_toolTip);
	m_ListBoxSecurityType.EnableTooltips(enable, m_toolTip);
	m_ListBoxHtb.EnableTooltips(enable, m_toolTip);
	m_ListBoxTradingState.EnableTooltips(enable, m_toolTip);
	m_ListBoxCircuitBreaker.EnableTooltips(enable, m_toolTip);
	m_ListBoxNasdaqQc.EnableTooltips(enable, m_toolTip);
	m_ListBoxListedQc.EnableTooltips(enable, m_toolTip);
}

void MyDialogStockFilter::Display(const TakionFilter& filter, TakionFrame* receipient, unsigned int code, CWnd* parent, int x, int y, int w, int h, bool show)
{
	MyDialogPriceFilter::Display(filter, receipient, code, parent, x, y, w, h, show);
}

void MyDialogStockFilter::DoDataExchange(CDataExchange* pDX)
{
	MyDialogPriceFilter::DoDataExchange(pDX);
	DDX_Control(pDX, exchangeStaticId, m_StaticMarketCategory);
	DDX_Control(pDX, exchangeListBoxId, m_ListBoxMarketCategory);

	DDX_Control(pDX, testStaticId, m_StaticTestOrReal);
	DDX_Control(pDX, testListBoxId, m_ListBoxTestOrReal);

	DDX_Control(pDX, securityTypeStaticId, m_StaticSecurityType);
	DDX_Control(pDX, securityTypeListBoxId, m_ListBoxSecurityType);

	DDX_Control(pDX, htbStaticId, m_StaticHtb);
	DDX_Control(pDX, htbListBoxId, m_ListBoxHtb);

	DoDataExchangeRange(m_yesterdaysVolume, pDX);
	DoDataExchangeRange(m_todaysVolume, pDX);
	DoDataExchangeRange(m_todaysPxMhVolume, pDX);

	DoDataExchangeRange(m_avgDailyVolume, pDX);
	DoDataExchangeRange(m_relativeVolume, pDX);
	DoDataExchangeRange(m_preMhVolRel, pDX);

	DoDataExchangeRange(m_yesterdaysPriceRange, pDX);
	DoDataExchangeRange(m_yesterdaysPriceRangePercent, pDX);

	DoDataExchangeRange(m_imbalanceNyse, pDX);
	DoDataExchangeRange(m_imbalanceNyseReg, pDX);
	DoDataExchangeRange(m_imbalanceNsdq, pDX);
	DoDataExchangeRange(m_imbalanceArca, pDX);
	DoDataExchangeRange(m_imbalanceExch, pDX);
	DoDataExchangeRange(m_rateImbalance, pDX);
	DoDataExchangeRange(m_imbNyDeltaInf, pDX);
	DoDataExchangeRange(m_imbExFarPriceDelta, pDX);
	DoDataExchangeRange(m_imbNqFarPriceDelta, pDX);
	DoDataExchangeRange(m_imbNqThru, pDX);
	DoDataExchangeRange(m_imbExValue, pDX);
	DoDataExchangeRange(m_imbExNear, pDX);
	DoDataExchangeRange(m_imbExFar, pDX);
	DoDataExchangeRange(m_imbExMatch, pDX);
	DoDataExchangeRange(m_imbExVolPercent, pDX);
	DoDataExchangeRange(m_imbExYestVolPercent, pDX);
	DoDataExchangeRange(m_deltaPv, pDX);
//	DoDataExchangeRange(m_rsiMh, pDX);
//	DoDataExchangeRange(m_rsiDays, pDX);
	DoDataExchangeRange(m_borrowPrice, pDX);
	DoDataExchangeRange(m_pmiSpread, pDX);
	DoDataExchangeRange(m_todaysClosePrice, pDX);

	DDX_Control(pDX, tradingStateStaticId, m_StaticTradingState);
	DDX_Control(pDX, tradingStateListBoxId, m_ListBoxTradingState);

	DDX_Control(pDX, circuitBreakerStaticId, m_StaticCircuitBreaker);
	DDX_Control(pDX, circuitBreakerListBoxId, m_ListBoxCircuitBreaker);

	DDX_Control(pDX, nasdaqQcStaticId, m_StaticNasdaqQc);
	DDX_Control(pDX, nasdaqQcListBoxId, m_ListBoxNasdaqQc);

	DDX_Control(pDX, listedQcStaticId, m_StaticListedQc);
	DDX_Control(pDX, listedQcListBoxId, m_ListBoxListedQc);

	DoDataExchangeRange(m_bidNet, pDX);
	DoDataExchangeRange(m_askNet, pDX);

	DoDataExchangeRange(m_tBidNet, pDX);
	DoDataExchangeRange(m_tAskNet, pDX);

	DoDataExchangeRange(m_spread, pDX);

	DoDataExchangeRange(m_bidNetPercent, pDX);
	DoDataExchangeRange(m_askNetPercent, pDX);

	DoDataExchangeRange(m_tBidNetPercent, pDX);
	DoDataExchangeRange(m_tAskNetPercent, pDX);
/*
	DDX_Control(pDX, rsiMarketHoursCalculationGroupId, m_GroupBoxRsiMhCalculation);
	DDX_Control(pDX, rsiMarketHoursCheckBoxCalculateId, m_CheckBoxRsiMhCalculate);
	DDX_Control(pDX, rsiMarketHoursStaticPriorPointCountId, m_StaticRsiMhPriorPointCount);
	DDX_Control(pDX, rsiMarketHoursSpinPriorPointCountId, m_SpinRsiMhPriorPointCount);
	DDX_Control(pDX, rsiMarketHoursStaticPeriodLengthId, m_StaticRsiMhPeriodLength);
	DDX_Control(pDX, rsiMarketHoursSpinPeriodLengthId, m_SpinRsiMhPeriodLength);
	DDX_Control(pDX, rsiMarketHoursStaticPeriodCountId, m_StaticRsiMhPeriodCount);
	DDX_Control(pDX, rsiMarketHoursSpinPeriodCountId, m_SpinRsiMhPeriodCount);
	DDX_Control(pDX, rsiMarketHoursStaticStartMinuteId, m_StaticRsiMhStartMinute);
	DDX_Control(pDX, rsiMarketHoursSpinStartMinuteId, m_SpinRsiMhStartMinute);
	DDX_Control(pDX, rsiMarketHoursCheckBoxTodayOnlyId, m_CheckBoxRsiMhTodayOnly);
	DDX_Control(pDX, rsiMarketHoursCheckBoxPostMarketId, m_CheckBoxRsiMhPostMarket);

	DDX_Control(pDX, rsiDaysCalculationGroupId, m_GroupBoxRsiDaysCalculation);
	DDX_Control(pDX, rsiDaysCheckBoxCalculateId, m_CheckBoxRsiDaysCalculate);
	DDX_Control(pDX, rsiDaysStaticPriorPointCountId, m_StaticRsiDaysPriorPointCount);
	DDX_Control(pDX, rsiDaysSpinPriorPointCountId, m_SpinRsiDaysPriorPointCount);
	DDX_Control(pDX, rsiDaysStaticPeriodLengthId, m_StaticRsiDaysPeriodLength);
	DDX_Control(pDX, rsiDaysSpinPeriodLengthId, m_SpinRsiDaysPeriodLength);
	DDX_Control(pDX, rsiDaysStaticPeriodCountId, m_StaticRsiDaysPeriodCount);
	DDX_Control(pDX, rsiDaysSpinPeriodCountId, m_SpinRsiDaysPeriodCount);
	DDX_Control(pDX, rsiDaysStaticStartMinuteId, m_StaticRsiDaysStartMinute);
	DDX_Control(pDX, rsiDaysSpinStartMinuteId, m_SpinRsiDaysStartMinute);
	DDX_Control(pDX, rsiDaysCheckBoxPostMarketId, m_CheckBoxRsiDaysPostMarket);
*/
	DoDataExchangeRange(m_luldDiff, pDX);
	DoDataExchangeRange(m_luldDiffPercent, pDX);
}

BOOL MyDialogStockFilter::OnInitDialog()
{
/*
	m_calculateRsiMh = false;
	m_calculateRsiDays = false;

	m_CheckBoxRsiMhTodayOnly.SetInvalid(!m_calculateRsiMh);
	m_CheckBoxRsiMhPostMarket.SetInvalid(!m_calculateRsiMh);

	m_CheckBoxRsiDaysPostMarket.SetInvalid(!m_calculateRsiDays);

	const COLORREF color = m_visualBase->GetLightGrayColor();

	m_SpinRsiMhPriorPointCount.SetBkColorEnabled(color);
	m_SpinRsiMhPeriodLength.SetBkColorEnabled(color);
	m_SpinRsiMhPeriodCount.SetBkColorEnabled(color);
	m_SpinRsiMhStartMinute.SetBkColorEnabled(color);

	m_SpinRsiDaysPriorPointCount.SetBkColorEnabled(color);
	m_SpinRsiDaysPeriodLength.SetBkColorEnabled(color);
	m_SpinRsiDaysPeriodCount.SetBkColorEnabled(color);
	m_SpinRsiDaysStartMinute.SetBkColorEnabled(color);
*/

	m_leftMostColumnBottomPixel = m_leftMostColumnLeftPixel = m_leftMostColumnWidthPixel = m_staticHeightPixel = 0;
	m_fifthColumnLeftPixel = m_fifthColumnWidthPixel = m_fifthColumnBottomPixel = 0;

	BOOL ret = MyDialogPriceFilter::OnInitDialog();

	RECT rect;
	m_StaticTestOrReal.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_staticHeightPixel = rect.bottom - rect.top;

	m_ListBoxMarketCategory.GetWindowRect(&rect);
	ScreenToClient(&rect);

	m_leftMostColumnLeftPixel = rect.left;
	m_leftMostColumnWidthPixel = rect.right - rect.left;

	const int borderSize = 2 * GetSystemMetrics(SM_CYEDGE);
	const int itemHeight = m_ListBoxMarketCategory.GetMyItemHeight();

	int height = itemHeight * m_marketCategoryVisibleItemCount + borderSize;

	m_ListBoxMarketCategory.SetWindowPos(nullptr, 0, 0, m_leftMostColumnWidthPixel, height, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);

	int top = rect.top + height + hgap;
	m_StaticTestOrReal.SetWindowPos(nullptr, m_leftMostColumnLeftPixel, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
	height = itemHeight * m_testVisibleItemCount + borderSize;
	m_ListBoxTestOrReal.SetWindowPos(nullptr, m_leftMostColumnLeftPixel, top += m_staticHeightPixel, m_leftMostColumnWidthPixel, height, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
	m_leftMostColumnBottomPixel = top + height;

	top = m_leftMostColumnBottomPixel + hgap;
	m_StaticSecurityType.SetWindowPos(nullptr, m_leftMostColumnLeftPixel, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
	height = itemHeight * m_securityTypeVisibleItemCount + borderSize;
	m_ListBoxSecurityType.SetWindowPos(nullptr, m_leftMostColumnLeftPixel, top += m_staticHeightPixel, m_leftMostColumnWidthPixel, height, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
	m_leftMostColumnBottomPixel = top + height;

	top = m_leftMostColumnBottomPixel + hgap;
	m_StaticHtb.SetWindowPos(nullptr, m_leftMostColumnLeftPixel, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
	height = itemHeight * m_htbVisibleItemCount + borderSize;
	m_ListBoxHtb.SetWindowPos(nullptr, m_leftMostColumnLeftPixel, top += m_staticHeightPixel, m_leftMostColumnWidthPixel, itemHeight * m_htbVisibleItemCount + borderSize, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
	m_leftMostColumnBottomPixel = top + height;

	m_ListBoxTradingState.GetWindowRect(&rect);
	ScreenToClient(&rect);

	m_fifthColumnLeftPixel = rect.left;
	int w2 = rect.right - rect.left;

	height = itemHeight * m_tradingStateVisibleItemCount + borderSize;
	m_ListBoxTradingState.SetWindowPos(nullptr, 0, 0, w2, height, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);

	m_ListBoxCircuitBreaker.GetWindowRect(&rect);
	ScreenToClient(&rect);
	w2 = rect.right - rect.left;
	m_ListBoxCircuitBreaker.SetWindowPos(nullptr, 0, 0, w2, height, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);

	m_fifthColumnWidthPixel = rect.right - m_fifthColumnLeftPixel;

	m_fifthColumnBottomPixel = rect.top + height;

	top = m_fifthColumnBottomPixel + hgap;
	const int topList = top + m_staticHeightPixel;
	height = itemHeight * m_quoteConditionVisibleItemCount + borderSize;

	m_ListBoxNasdaqQc.GetWindowRect(&rect);
	ScreenToClient(&rect);
	int x2 = rect.left;
	w2 = rect.right - rect.left;

	m_StaticNasdaqQc.SetWindowPos(nullptr, x2, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
	m_ListBoxNasdaqQc.SetWindowPos(nullptr, x2, topList, w2, height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

	m_ListBoxListedQc.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x2 = rect.left;
	w2 = rect.right - rect.left;
	m_StaticListedQc.SetWindowPos(nullptr, x2, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
	m_ListBoxListedQc.SetWindowPos(nullptr, x2, topList, w2, height, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER);

	m_fifthColumnBottomPixel = topList + height;

	m_ListBoxMarketCategory.SetFilter(0);
	m_ListBoxTestOrReal.SetFilter(0);
	m_ListBoxSecurityType.SetFilter(0);
	m_ListBoxHtb.SetFilter(0);
	m_ListBoxTradingState.SetFilter(0);
	m_ListBoxCircuitBreaker.SetFilter(0);
	m_ListBoxNasdaqQc.SetFilter(0);
	m_ListBoxListedQc.SetFilter(0);

	return ret;
}

void MyDialogStockFilter::UpdateMyFields(MyStockFilter& filter) const
{
#ifndef TAKION_NO_OPTIONS
	if(!((const MySymbolFilter*)m_filter)->isBaseOptionSorterFilter())
	{
#endif
		filter.SetMarketCategoryFilter(m_ListBoxMarketCategory.GetFilter());//m_exchangeFilter);
		filter.SetTestStockFilter(m_ListBoxTestOrReal.GetFilter());//m_stockTestFilter);
		filter.SetSecurityTypeFilter(m_ListBoxSecurityType.GetFilter());//m_stockTestFilter);
		filter.SetHtbFilter(m_ListBoxHtb.GetFilter());//m_stockTestFilter);
		filter.SetTradingStateFilter(m_ListBoxTradingState.GetFilter());//m_stockTestFilter);
		filter.SetCircuitBreakerFilter(m_ListBoxCircuitBreaker.GetFilter());//m_stockTestFilter);
		filter.SetNasdaqQuoteConditionFilter(m_ListBoxNasdaqQc.GetFilter());
		filter.SetListedQuoteConditionFilter(m_ListBoxListedQc.GetFilter());

		m_imbalanceNyse.SetRange(filter.GetImbalanceNyseRange());
		m_imbalanceNyse.SetPositiveNegative(filter.GetImbalanceNyseRange());
		m_imbalanceNyseReg.SetRange(filter.GetImbalanceNyseRegRange());
		m_imbalanceNyseReg.SetPositiveNegative(filter.GetImbalanceNyseRegRange());
		m_imbalanceNsdq.SetRange(filter.GetImbalanceNsdqRange());
		m_imbalanceNsdq.SetPositiveNegative(filter.GetImbalanceNsdqRange());
		m_imbalanceArca.SetRange(filter.GetImbalanceArcaRange());
		m_imbalanceArca.SetPositiveNegative(filter.GetImbalanceArcaRange());
		m_imbalanceExch.SetRange(filter.GetImbalanceExchRange());
		m_imbalanceExch.SetPositiveNegative(filter.GetImbalanceExchRange());

		m_rateImbalance.SetRange(filter.GetRateImbalanceRange());
		m_rateImbalance.SetPositiveNegative(filter.GetRateImbalanceRange());
		m_imbNyDeltaInf.SetRange(filter.GetImbNyDeltaInfRange());
		m_imbNyDeltaInf.SetPositiveNegative(filter.GetImbNyDeltaInfRange());

		m_imbExFarPriceDelta.SetRange(filter.GetImbExFarPriceDeltaRange());
		m_imbExFarPriceDelta.SetPositiveNegative(filter.GetImbExFarPriceDeltaRange());

		m_imbNqFarPriceDelta.SetRange(filter.GetImbNqFarPriceDeltaRange());
		m_imbNqFarPriceDelta.SetPositiveNegative(filter.GetImbNqFarPriceDeltaRange());

		m_imbNqThru.SetRange(filter.GetImbNqThruRange());
		m_imbNqThru.SetPositiveNegative(filter.GetImbNqThruRange());

		m_imbExValue.SetRange(filter.GetImbExValueRange());
		m_imbExValue.SetPositiveNegative(filter.GetImbExValueRange());

		m_imbExNear.SetRange(filter.GetImbExNearRange());
		m_imbExFar.SetRange(filter.GetImbExFarRange());
		m_imbExMatch.SetRange(filter.GetImbExMatchRange());
		m_imbExVolPercent.SetRange(filter.GetImbExVolPercentRange());
		m_imbExYestVolPercent.SetRange(filter.GetImbExYestVolPercentRange());
		m_deltaPv.SetRange(filter.GetDeltaPvRange());
//		m_rsiMh.SetRange(filter.GetRsiMhRange());
//		m_rsiDays.SetRange(filter.GetRsiDaysRange());
		m_borrowPrice.SetRange(filter.GetBorrowPriceRange());
		m_pmiSpread.SetRange(filter.GetPmiSpreadRange());

		m_luldDiff.SetRange(filter.GetLuldRange());
		m_luldDiff.SetPositiveNegative(filter.GetLuldRange());

		m_luldDiffPercent.SetRange(filter.GetLuldPercentRange());
		m_luldDiffPercent.SetPositiveNegative(filter.GetLuldPercentRange());
/*
		filter.SetRsiMhCalculate(m_calculateRsiMh);
		filter.SetRsiMhPriorPointCount(m_SpinRsiMhPriorPointCount.GetValue());
		filter.SetRsiMhPeriodLength(m_SpinRsiMhPeriodLength.GetValue());
		filter.SetRsiMhPeriodCount(m_SpinRsiMhPeriodCount.GetValue());
		filter.SetRsiMhStartMinute(m_SpinRsiMhStartMinute.GetMinute());
		filter.SetRsiMhTodayOnly(m_CheckBoxRsiMhTodayOnly.GetCheck() == BST_CHECKED);
		filter.SetRsiMhPostMarket(m_CheckBoxRsiMhPostMarket.GetCheck() == BST_CHECKED);

		filter.SetRsiDaysCalculate(m_calculateRsiDays);
		filter.SetRsiDaysPriorPointCount(m_SpinRsiDaysPriorPointCount.GetValue());
		filter.SetRsiDaysPeriodLength(m_SpinRsiDaysPeriodLength.GetValue());
		filter.SetRsiDaysPeriodCount(m_SpinRsiDaysPeriodCount.GetValue());
		filter.SetRsiDaysStartMinute(m_SpinRsiDaysStartMinute.GetMinute());
		filter.SetRsiDaysPostMarket(m_CheckBoxRsiDaysPostMarket.GetCheck() == BST_CHECKED);
*/
#ifndef TAKION_NO_OPTIONS
	}
#endif
	m_yesterdaysVolume.SetRange(filter.GetYesterdaysVolumeRange());
	m_todaysVolume.SetRange(filter.GetTodaysVolumeRange());
	m_todaysPxMhVolume.SetRange(filter.GetTodaysPxMhVolumeRange());
	
	m_avgDailyVolume.SetRange(filter.GetAvgDailyVolumeRange());
	m_relativeVolume.SetRange(filter.GetRelativeVolumeRange());
	m_preMhVolRel.SetRange(filter.GetPreMhVolRelRange());

	m_yesterdaysPriceRange.SetRange(filter.GetYesterdaysPriceRangeRange());
	m_yesterdaysPriceRangePercent.SetRange(filter.GetYesterdaysPriceRangePercentRange());
	
	m_todaysClosePrice.SetRange(filter.GetTodaysClosePriceRange());

	m_bidNet.SetRange(filter.GetBidNetRange());
	m_bidNet.SetPositiveNegative(filter.GetBidNetRange());
	m_askNet.SetRange(filter.GetAskNetRange());
	m_askNet.SetPositiveNegative(filter.GetAskNetRange());

	m_tBidNet.SetRange(filter.GetTBidNetRange());
	m_tBidNet.SetPositiveNegative(filter.GetTBidNetRange());
	m_tAskNet.SetRange(filter.GetTAskNetRange());
	m_tAskNet.SetPositiveNegative(filter.GetTAskNetRange());

	m_spread.SetRange(filter.GetSpreadRange());
	m_spread.SetPositiveNegative(filter.GetSpreadRange());

	m_bidNetPercent.SetRange(filter.GetBidNetPercentRange());
	m_bidNetPercent.SetPositiveNegative(filter.GetBidNetPercentRange());
	m_askNetPercent.SetRange(filter.GetAskNetPercentRange());
	m_askNetPercent.SetPositiveNegative(filter.GetAskNetPercentRange());

	m_tBidNetPercent.SetRange(filter.GetTBidNetPercentRange());
	m_tBidNetPercent.SetPositiveNegative(filter.GetTBidNetPercentRange());
	m_tAskNetPercent.SetRange(filter.GetTAskNetPercentRange());
	m_tAskNetPercent.SetPositiveNegative(filter.GetTAskNetPercentRange());
}

void MyDialogStockFilter::UpdateFilterValues()
{
	MyDialogPriceFilter::UpdateFilterValues();
	UpdateMyFields(*(MyStockFilter*)m_filter);
}

void MyDialogStockFilter::MyFieldsToControls(const MyStockFilter& filter, unsigned int doNotCopyFlags)
{
#ifndef TAKION_NO_OPTIONS
	if(!((const MySymbolFilter*)m_filter)->isBaseOptionSorterFilter())
	{
#endif
		m_ListBoxMarketCategory.SetFilter(filter.GetMarketCategoryFilter());
		m_ListBoxTestOrReal.SetFilter(filter.GetTestStockFilter());
		m_ListBoxSecurityType.SetFilter(filter.GetSecurityTypeFilter());
		m_ListBoxHtb.SetFilter(filter.GetHtbFilter());
		m_ListBoxTradingState.SetFilter(filter.GetTradingStateFilter());
		m_ListBoxCircuitBreaker.SetFilter(filter.GetCircuitBreakerFilter());
		m_ListBoxNasdaqQc.SetFilter(filter.GetNasdaqQuoteConditionFilter());
		m_ListBoxListedQc.SetFilter(filter.GetListedQuoteConditionFilter());

		m_imbalanceNyse.SetRangeControls(filter.GetImbalanceNyseRange());
		m_imbalanceNyse.SetPositiveNegativeControls(filter.GetImbalanceNyseRange());
		m_imbalanceNyseReg.SetRangeControls(filter.GetImbalanceNyseRegRange());
		m_imbalanceNyseReg.SetPositiveNegativeControls(filter.GetImbalanceNyseRegRange());
		m_imbalanceNsdq.SetRangeControls(filter.GetImbalanceNsdqRange());
		m_imbalanceNsdq.SetPositiveNegativeControls(filter.GetImbalanceNsdqRange());
		m_imbalanceArca.SetRangeControls(filter.GetImbalanceArcaRange());
		m_imbalanceArca.SetPositiveNegativeControls(filter.GetImbalanceArcaRange());
		m_imbalanceExch.SetRangeControls(filter.GetImbalanceExchRange());
		m_imbalanceExch.SetPositiveNegativeControls(filter.GetImbalanceExchRange());

		m_rateImbalance.SetRangeControls(filter.GetRateImbalanceRange());
		m_rateImbalance.SetPositiveNegativeControls(filter.GetRateImbalanceRange());
		m_imbNyDeltaInf.SetRangeControls(filter.GetImbNyDeltaInfRange());
		m_imbNyDeltaInf.SetPositiveNegativeControls(filter.GetImbNyDeltaInfRange());

		m_imbExFarPriceDelta.SetRangeControls(filter.GetImbExFarPriceDeltaRange());
		m_imbExFarPriceDelta.SetPositiveNegativeControls(filter.GetImbExFarPriceDeltaRange());

		m_imbNqFarPriceDelta.SetRangeControls(filter.GetImbNqFarPriceDeltaRange());
		m_imbNqFarPriceDelta.SetPositiveNegativeControls(filter.GetImbNqFarPriceDeltaRange());

		m_imbNqThru.SetRangeControls(filter.GetImbNqThruRange());
		m_imbNqThru.SetPositiveNegativeControls(filter.GetImbNqThruRange());

		m_imbExValue.SetRangeControls(filter.GetImbExValueRange());
		m_imbExValue.SetPositiveNegativeControls(filter.GetImbExValueRange());

		m_imbExNear.SetRangeControls(filter.GetImbExNearRange());
		m_imbExFar.SetRangeControls(filter.GetImbExFarRange());
		m_imbExMatch.SetRangeControls(filter.GetImbExMatchRange());
		m_imbExVolPercent.SetRangeControls(filter.GetImbExVolPercentRange());
		m_imbExYestVolPercent.SetRangeControls(filter.GetImbExYestVolPercentRange());
		m_deltaPv.SetRangeControls(filter.GetDeltaPvRange());
//		m_rsiMh.SetRangeControls(filter.GetRsiMhRange());
//		m_rsiDays.SetRangeControls(filter.GetRsiDaysRange());
		m_borrowPrice.SetRangeControls(filter.GetBorrowPriceRange());
		m_pmiSpread.SetRangeControls(filter.GetPmiSpreadRange());

		m_luldDiff.SetRangeControls(filter.GetLuldRange());
		m_luldDiff.SetPositiveNegativeControls(filter.GetLuldRange());

		m_luldDiffPercent.SetRangeControls(filter.GetLuldPercentRange());
		m_luldDiffPercent.SetPositiveNegativeControls(filter.GetLuldPercentRange());
/*
		m_CheckBoxRsiMhCalculate.SetCheck(filter.isRsiMhCalculate() ? BST_CHECKED : BST_UNCHECKED);
		m_SpinRsiMhPriorPointCount.SetValue(filter.GetRsiMhPriorPointCount());
		m_SpinRsiMhPeriodLength.SetValue(filter.GetRsiMhPeriodLength());
		m_SpinRsiMhPeriodCount.SetValue(filter.GetRsiMhPeriodCount());
		m_SpinRsiMhStartMinute.SetMinute(filter.GetRsiMhStartMinute());
		m_CheckBoxRsiMhTodayOnly.SetCheck(filter.isRsiMhTodayOnly() ? BST_CHECKED : BST_UNCHECKED);
		m_CheckBoxRsiMhPostMarket.SetCheck(filter.isRsiMhPostMarket() ? BST_CHECKED : BST_UNCHECKED);
		if(filter.isRsiMhCalculate() != m_calculateRsiMh)
		{
			OnRsiMhCalculate();
		}

		m_CheckBoxRsiDaysCalculate.SetCheck(filter.isRsiDaysCalculate() ? BST_CHECKED : BST_UNCHECKED);
		m_SpinRsiDaysPriorPointCount.SetValue(filter.GetRsiDaysPriorPointCount());
		m_SpinRsiDaysPeriodLength.SetValue(filter.GetRsiDaysPeriodLength());
		m_SpinRsiDaysPeriodCount.SetValue(filter.GetRsiDaysPeriodCount());
		m_SpinRsiDaysStartMinute.SetMinute(filter.GetRsiDaysStartMinute());
		m_CheckBoxRsiDaysPostMarket.SetCheck(filter.isRsiDaysPostMarket() ? BST_CHECKED : BST_UNCHECKED);
		if(filter.isRsiDaysCalculate() != m_calculateRsiDays)
		{
			OnRsiDaysCalculate();
		}
*/
#ifndef TAKION_NO_OPTIONS
	}
#endif
	m_yesterdaysVolume.SetRangeControls(filter.GetYesterdaysVolumeRange());
	m_todaysVolume.SetRangeControls(filter.GetTodaysVolumeRange());
	m_todaysPxMhVolume.SetRangeControls(filter.GetTodaysPxMhVolumeRange());

	m_avgDailyVolume.SetRangeControls(filter.GetAvgDailyVolumeRange());
	m_relativeVolume.SetRangeControls(filter.GetRelativeVolumeRange());
	m_preMhVolRel.SetRangeControls(filter.GetPreMhVolRelRange());

	m_yesterdaysPriceRange.SetRangeControls(filter.GetYesterdaysPriceRangeRange());
	m_yesterdaysPriceRangePercent.SetRangeControls(filter.GetYesterdaysPriceRangePercentRange());

	m_todaysClosePrice.SetRangeControls(filter.GetTodaysClosePriceRange());

	m_bidNet.SetRangeControls(filter.GetBidNetRange());
	m_bidNet.SetPositiveNegativeControls(filter.GetBidNetRange());
	m_askNet.SetRangeControls(filter.GetAskNetRange());
	m_askNet.SetPositiveNegativeControls(filter.GetAskNetRange());

	m_tBidNet.SetRangeControls(filter.GetTBidNetRange());
	m_tBidNet.SetPositiveNegativeControls(filter.GetTBidNetRange());
	m_tAskNet.SetRangeControls(filter.GetTAskNetRange());
	m_tAskNet.SetPositiveNegativeControls(filter.GetTAskNetRange());

	m_spread.SetRangeControls(filter.GetSpreadRange());
	m_spread.SetPositiveNegativeControls(filter.GetSpreadRange());

	m_bidNetPercent.SetRangeControls(filter.GetBidNetPercentRange());
	m_bidNetPercent.SetPositiveNegativeControls(filter.GetBidNetPercentRange());
	m_askNetPercent.SetRangeControls(filter.GetAskNetPercentRange());
	m_askNetPercent.SetPositiveNegativeControls(filter.GetAskNetPercentRange());

	m_tBidNetPercent.SetRangeControls(filter.GetTBidNetPercentRange());
	m_tBidNetPercent.SetPositiveNegativeControls(filter.GetTBidNetPercentRange());
	m_tAskNetPercent.SetRangeControls(filter.GetTAskNetPercentRange());
	m_tAskNetPercent.SetPositiveNegativeControls(filter.GetTAskNetPercentRange());
}

void MyDialogStockFilter::BeforeShow()
{
	MyDialogPriceFilter::BeforeShow();
	MyFieldsToControls(*(const MyStockFilter*)m_filter, 0);
}

void MyDialogStockFilter::SetFilterEmpty()
{
	MyDialogPriceFilter::SetFilterEmpty();
	m_yesterdaysVolume.SetRangeEmpty();
	m_todaysVolume.SetRangeEmpty();
	m_todaysPxMhVolume.SetRangeEmpty();
	m_avgDailyVolume.SetRangeEmpty();
	m_relativeVolume.SetRangeEmpty();
	m_preMhVolRel.SetRangeEmpty();
	m_yesterdaysPriceRange.SetRangeEmpty();
	m_yesterdaysPriceRangePercent.SetRangeEmpty();

	m_ListBoxMarketCategory.SetFilterAll();
	m_ListBoxTestOrReal.SetFilterAll();
	m_ListBoxSecurityType.SetFilterAll();
	m_ListBoxHtb.SetFilterAll();
	m_ListBoxTradingState.SetFilterAll();
	m_ListBoxCircuitBreaker.SetFilterAll();
	m_ListBoxNasdaqQc.SetFilterAll();
	m_ListBoxListedQc.SetFilterAll();

	m_imbalanceNyse.SetRangeEmpty();
	m_imbalanceNyseReg.SetRangeEmpty();
	m_imbalanceNsdq.SetRangeEmpty();
	m_imbalanceArca.SetRangeEmpty();
	m_imbalanceExch.SetRangeEmpty();
	m_rateImbalance.SetRangeEmpty();
	m_imbNyDeltaInf.SetRangeEmpty();
	m_imbExFarPriceDelta.SetRangeEmpty();
	m_imbNqFarPriceDelta.SetRangeEmpty();
	m_imbNqThru.SetRangeEmpty();
	m_imbExValue.SetRangeEmpty();
	m_imbExNear.SetRangeEmpty();
	m_imbExFar.SetRangeEmpty();
	m_imbExMatch.SetRangeEmpty();
	m_imbExVolPercent.SetRangeEmpty();
	m_imbExYestVolPercent.SetRangeEmpty();
	m_deltaPv.SetRangeEmpty();
//	m_rsiMh.SetRangeEmpty();
//	m_rsiDays.SetRangeEmpty();
	m_borrowPrice.SetRangeEmpty();
	m_pmiSpread.SetRangeEmpty();
	m_todaysClosePrice.SetRangeEmpty();

	m_bidNet.SetRangeEmpty();
	m_askNet.SetRangeEmpty();

	m_tBidNet.SetRangeEmpty();
	m_tAskNet.SetRangeEmpty();

	m_spread.SetRangeEmpty();

	m_bidNetPercent.SetRangeEmpty();
	m_askNetPercent.SetRangeEmpty();

	m_tBidNetPercent.SetRangeEmpty();
	m_tAskNetPercent.SetRangeEmpty();

	m_luldDiff.SetRangeEmpty();
	m_luldDiffPercent.SetRangeEmpty();
/*
	m_calculateRsiMh = false;
	m_SpinRsiMhPriorPointCount.SetValue(250, false, true, false);
	m_SpinRsiMhPeriodLength.SetValue(1, false, true, false);
	m_SpinRsiMhPeriodCount.SetValue(14, false, true, false);
	m_SpinRsiMhStartMinute.SetMinute(570, false);
	m_CheckBoxRsiMhTodayOnly.SetCheck(BST_UNCHECKED);
	m_CheckBoxRsiMhPostMarket.SetCheck(BST_UNCHECKED);

	m_calculateRsiDays = false;
	m_SpinRsiDaysPriorPointCount.SetValue(250, false, true, false);
	m_SpinRsiDaysPeriodLength.SetValue(1, false, true, false);
	m_SpinRsiDaysPeriodCount.SetValue(14, false, true, false);
	m_SpinRsiDaysStartMinute.SetMinute(570, false);
	m_CheckBoxRsiDaysPostMarket.SetCheck(BST_UNCHECKED);
*/
}

bool MyDialogStockFilter::isFilterEmpty() const
{
	return MyDialogPriceFilter::isFilterEmpty()
		&& m_yesterdaysVolume.isRangeEmpty()
		&& m_todaysVolume.isRangeEmpty()
		&& m_todaysPxMhVolume.isRangeEmpty()
		&& m_avgDailyVolume.isRangeEmpty()
		&& m_relativeVolume.isRangeEmpty()
		&& m_preMhVolRel.isRangeEmpty()
		&& m_yesterdaysPriceRange.isRangeEmpty()
		&& m_yesterdaysPriceRangePercent.isRangeEmpty()

		&& m_ListBoxMarketCategory.AreAllItemsChecked() == 2
		&& m_ListBoxTestOrReal.AreAllItemsChecked() == 2
		&& m_ListBoxSecurityType.AreAllItemsChecked() == 2
		&& m_ListBoxHtb.AreAllItemsChecked() == 2
		&& m_ListBoxTradingState.AreAllItemsChecked() == 2
		&& m_ListBoxCircuitBreaker.AreAllItemsChecked() == 2
		&& m_ListBoxNasdaqQc.AreAllItemsChecked() == 2
		&& m_ListBoxListedQc.AreAllItemsChecked() == 2

		&& m_imbalanceNyse.isRangeEmpty()
		&& m_imbalanceNyseReg.isRangeEmpty()
		&& m_imbalanceNsdq.isRangeEmpty()
		&& m_imbalanceArca.isRangeEmpty()
		&& m_imbalanceExch.isRangeEmpty()
		&& m_rateImbalance.isRangeEmpty()
		&& m_imbNyDeltaInf.isRangeEmpty()
		&& m_imbExFarPriceDelta.isRangeEmpty()
		&& m_imbNqFarPriceDelta.isRangeEmpty()
		&& m_imbNqThru.isRangeEmpty()
		&& m_imbExValue.isRangeEmpty()
		&& m_imbExNear.isRangeEmpty()
		&& m_imbExFar.isRangeEmpty()
		&& m_imbExMatch.isRangeEmpty()
		&& m_imbExVolPercent.isRangeEmpty()
		&& m_imbExYestVolPercent.isRangeEmpty()
		&& m_deltaPv.isRangeEmpty()
//		&& m_rsiMh.isRangeEmpty()
//		&& m_rsiDays.isRangeEmpty()
		&& m_borrowPrice.isRangeEmpty()
		&& m_pmiSpread.isRangeEmpty()
		&& m_todaysClosePrice.isRangeEmpty()

		&& m_bidNet.isRangeEmpty()
		&& m_askNet.isRangeEmpty()

		&& m_tBidNet.isRangeEmpty()
		&& m_tAskNet.isRangeEmpty()

		&& m_spread.isRangeEmpty()

		&& m_bidNetPercent.isRangeEmpty()
		&& m_askNetPercent.isRangeEmpty()

		&& m_tBidNetPercent.isRangeEmpty()
		&& m_tAskNetPercent.isRangeEmpty()

		&& m_luldDiff.isRangeEmpty()
		&& m_luldDiffPercent.isRangeEmpty()
/*
		&& !m_calculateRsiMh
		&& m_SpinRsiMhPriorPointCount.GetValue() == 250
		&& m_SpinRsiMhPeriodLength.GetValue() == 1
		&& m_SpinRsiMhPeriodCount.GetValue() == 14
		&& m_SpinRsiMhStartMinute.GetMinute() == 570
		&& m_CheckBoxRsiMhTodayOnly.GetCheck() == BST_UNCHECKED
		&& m_CheckBoxRsiMhPostMarket.GetCheck() == BST_UNCHECKED

		&& !m_calculateRsiDays
		&& m_SpinRsiDaysPriorPointCount.GetValue() == 250
		&& m_SpinRsiDaysPeriodLength.GetValue() == 1
		&& m_SpinRsiDaysPeriodCount.GetValue() == 14
		&& m_SpinRsiDaysStartMinute.GetMinute() == 570
		&& m_CheckBoxRsiDaysPostMarket.GetCheck() == BST_UNCHECKED
*/
		;
}

bool MyDialogStockFilter::MyFieldsIntersectionDifferent(const MyStockFilter& filter, unsigned int doNotCopyFlags) const
{
	return m_yesterdaysVolume.isRangeModified(filter.GetYesterdaysVolumeRange())
		|| m_todaysVolume.isRangeModified(filter.GetTodaysVolumeRange())
		|| m_todaysPxMhVolume.isRangeModified(filter.GetTodaysPxMhVolumeRange())

		|| m_avgDailyVolume.isRangeModified(filter.GetAvgDailyVolumeRange())
		|| m_relativeVolume.isRangeModified(filter.GetRelativeVolumeRange())
		|| m_preMhVolRel.isRangeModified(filter.GetPreMhVolRelRange())
		|| m_yesterdaysPriceRange.isRangeModified(filter.GetYesterdaysPriceRangeRange())
		|| m_yesterdaysPriceRangePercent.isRangeModified(filter.GetYesterdaysPriceRangePercentRange())

		|| m_ListBoxMarketCategory.GetFilter() != filter.GetMarketCategoryFilter()

		|| m_ListBoxTestOrReal.GetFilter() != filter.GetTestStockFilter()
		|| m_ListBoxSecurityType.GetFilter() != filter.GetSecurityTypeFilter()
		|| m_ListBoxHtb.GetFilter() != filter.GetHtbFilter()
		|| m_ListBoxTradingState.GetFilter() != filter.GetTradingStateFilter()
		|| m_ListBoxCircuitBreaker.GetFilter() != filter.GetCircuitBreakerFilter()
		|| m_ListBoxNasdaqQc.GetFilter() != filter.GetNasdaqQuoteConditionFilter()
		|| m_ListBoxListedQc.GetFilter() != filter.GetListedQuoteConditionFilter()

		|| m_imbalanceNyse.isRangeModified(filter.GetImbalanceNyseRange())
		|| m_imbalanceNyse.isPositiveNegativeModified(filter.GetImbalanceNyseRange())
		|| m_imbalanceNyseReg.isRangeModified(filter.GetImbalanceNyseRegRange())
		|| m_imbalanceNyseReg.isPositiveNegativeModified(filter.GetImbalanceNyseRegRange())
		|| m_imbalanceNsdq.isRangeModified(filter.GetImbalanceNsdqRange())
		|| m_imbalanceNsdq.isPositiveNegativeModified(filter.GetImbalanceNsdqRange())
		|| m_imbalanceArca.isRangeModified(filter.GetImbalanceArcaRange())
		|| m_imbalanceArca.isPositiveNegativeModified(filter.GetImbalanceArcaRange())
		|| m_imbalanceExch.isRangeModified(filter.GetImbalanceExchRange())
		|| m_imbalanceExch.isPositiveNegativeModified(filter.GetImbalanceExchRange())
		|| m_rateImbalance.isRangeModified(filter.GetRateImbalanceRange())
		|| m_rateImbalance.isPositiveNegativeModified(filter.GetRateImbalanceRange())
		|| m_imbNyDeltaInf.isRangeModified(filter.GetImbNyDeltaInfRange())
		|| m_imbNyDeltaInf.isPositiveNegativeModified(filter.GetImbNyDeltaInfRange())

		|| m_imbExFarPriceDelta.isRangeModified(filter.GetImbExFarPriceDeltaRange())
		|| m_imbExFarPriceDelta.isPositiveNegativeModified(filter.GetImbExFarPriceDeltaRange())

		|| m_imbNqFarPriceDelta.isRangeModified(filter.GetImbNqFarPriceDeltaRange())
		|| m_imbNqFarPriceDelta.isPositiveNegativeModified(filter.GetImbNqFarPriceDeltaRange())

		|| m_imbNqThru.isRangeModified(filter.GetImbNqThruRange())
		|| m_imbNqThru.isPositiveNegativeModified(filter.GetImbNqThruRange())

		|| m_imbExValue.isRangeModified(filter.GetImbExValueRange())
		|| m_imbExValue.isPositiveNegativeModified(filter.GetImbExValueRange())

		|| m_imbExNear.isRangeModified(filter.GetImbExNearRange())
		|| m_imbExFar.isRangeModified(filter.GetImbExFarRange())
		|| m_imbExMatch.isRangeModified(filter.GetImbExMatchRange())
		|| m_imbExVolPercent.isRangeModified(filter.GetImbExVolPercentRange())
		|| m_imbExYestVolPercent.isRangeModified(filter.GetImbExYestVolPercentRange())
		|| m_deltaPv.isRangeModified(filter.GetDeltaPvRange())
//		|| m_rsiMh.isRangeModified(filter.GetRsiMhRange())
//		|| m_rsiDays.isRangeModified(filter.GetRsiDaysRange())
		|| m_borrowPrice.isRangeModified(filter.GetBorrowPriceRange())
		|| m_pmiSpread.isRangeModified(filter.GetPmiSpreadRange())
		|| m_todaysClosePrice.isRangeModified(filter.GetTodaysClosePriceRange())

		|| m_bidNet.isRangeModified(filter.GetBidNetRange())
		|| m_bidNet.isPositiveNegativeModified(filter.GetBidNetRange())
		|| m_askNet.isRangeModified(filter.GetAskNetRange())
		|| m_askNet.isPositiveNegativeModified(filter.GetAskNetRange())

		|| m_tBidNet.isRangeModified(filter.GetTBidNetRange())
		|| m_tBidNet.isPositiveNegativeModified(filter.GetTBidNetRange())
		|| m_tAskNet.isRangeModified(filter.GetTAskNetRange())
		|| m_tAskNet.isPositiveNegativeModified(filter.GetTAskNetRange())

		|| m_spread.isRangeModified(filter.GetSpreadRange())
		|| m_spread.isPositiveNegativeModified(filter.GetSpreadRange())

		|| m_bidNetPercent.isRangeModified(filter.GetBidNetPercentRange())
		|| m_bidNetPercent.isPositiveNegativeModified(filter.GetBidNetPercentRange())
		|| m_askNetPercent.isRangeModified(filter.GetAskNetPercentRange())
		|| m_askNetPercent.isPositiveNegativeModified(filter.GetAskNetPercentRange())

		|| m_tBidNetPercent.isRangeModified(filter.GetTBidNetPercentRange())
		|| m_tBidNetPercent.isPositiveNegativeModified(filter.GetTBidNetPercentRange())
		|| m_tAskNetPercent.isRangeModified(filter.GetTAskNetPercentRange())
		|| m_tAskNetPercent.isPositiveNegativeModified(filter.GetTAskNetPercentRange())

		|| m_luldDiff.isRangeModified(filter.GetLuldRange())
		|| m_luldDiff.isPositiveNegativeModified(filter.GetLuldRange())

		|| m_luldDiffPercent.isRangeModified(filter.GetLuldPercentRange())
		|| m_luldDiffPercent.isPositiveNegativeModified(filter.GetLuldPercentRange())
/*
		|| m_calculateRsiMh != filter.isRsiMhCalculate()
		|| m_SpinRsiMhPriorPointCount.GetValue() != filter.GetRsiMhPriorPointCount()
		|| m_SpinRsiMhPeriodLength.GetValue() != filter.GetRsiMhPeriodLength()
		|| m_SpinRsiMhPeriodCount.GetValue() != filter.GetRsiMhPeriodCount()
		|| m_SpinRsiMhStartMinute.GetMinute() != filter.GetRsiMhStartMinute()
		|| (m_CheckBoxRsiMhTodayOnly.GetCheck() == BST_CHECKED) != filter.isRsiMhTodayOnly()
		|| (m_CheckBoxRsiMhPostMarket.GetCheck() == BST_CHECKED) != filter.isRsiMhPostMarket()

		|| m_calculateRsiDays != filter.isRsiDaysCalculate()
		|| m_SpinRsiDaysPriorPointCount.GetValue() != filter.GetRsiDaysPriorPointCount()
		|| m_SpinRsiDaysPeriodLength.GetValue() != filter.GetRsiDaysPeriodLength()
		|| m_SpinRsiDaysPeriodCount.GetValue() != filter.GetRsiDaysPeriodCount()
		|| m_SpinRsiDaysStartMinute.GetMinute() != filter.GetRsiDaysStartMinute()
		|| (m_CheckBoxRsiDaysPostMarket.GetCheck() == BST_CHECKED) != filter.isRsiDaysPostMarket()
*/
		;
}

bool MyDialogStockFilter::isFilterChanged() const
{
	return MyDialogPriceFilter::isFilterChanged() || MyFieldsIntersectionDifferent(*(const MyStockFilter*)m_filter, 0);
}

bool MyDialogStockFilter::isFilterValid() const
{
	return MyDialogPriceFilter::isFilterValid()
		&& m_yesterdaysVolume.isRangeValid()
		&& m_todaysVolume.isRangeValid()
		&& m_todaysPxMhVolume.isRangeValid()

		&& m_avgDailyVolume.isRangeValid()
		&& m_relativeVolume.isRangeValid()
		&& m_preMhVolRel.isRangeValid()
		&& m_yesterdaysPriceRange.isRangeValid()
		&& m_yesterdaysPriceRangePercent.isRangeValid()

		&& m_imbalanceNyse.isRangeValid()
		&& m_imbalanceNyseReg.isRangeValid()
		&& m_imbalanceNsdq.isRangeValid()
		&& m_imbalanceArca.isRangeValid()
		&& m_imbalanceExch.isRangeValid()
		&& m_rateImbalance.isRangeValid()
		&& m_imbNyDeltaInf.isRangeValid()
		&& m_imbExFarPriceDelta.isRangeValid()
		&& m_imbExFarPriceDelta.isRangeValid()
		&& m_imbExNear.isRangeValid()
		&& m_imbExFar.isRangeValid()
		&& m_imbExMatch.isRangeValid()
		&& m_imbExVolPercent.isRangeValid()
		&& m_imbExYestVolPercent.isRangeValid()
		&& m_deltaPv.isRangeValid()
//		&& m_rsiMh.isRangeValid()//????
//		&& m_rsiDays.isRangeValid()//????
		&& m_borrowPrice.isRangeValid()
		&& m_pmiSpread.isRangeValid()
		&& m_todaysClosePrice.isRangeValid()

		&& m_bidNet.isRangeValid()
		&& m_askNet.isRangeValid()

		&& m_tBidNet.isRangeValid()
		&& m_tAskNet.isRangeValid()

		&& m_spread.isRangeValid()

		&& m_bidNetPercent.isRangeValid()
		&& m_askNetPercent.isRangeValid()

		&& m_tBidNetPercent.isRangeValid()
		&& m_tAskNetPercent.isRangeValid()

		&& m_luldDiff.isRangeValid()
		&& m_luldDiffPercent.isRangeValid()
		;
}

HBRUSH MyDialogStockFilter::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return MyDialogPriceFilter::OnCtlColor(pDC, pWnd, nCtlColor);
}


//////////////

TakionFilter* MyMarketSorterFilter::CreateFilterCopy() const
{
	return new MyMarketSorterFilter(*this);
}

bool MyMarketSorterFilter::MyFieldsEqual(const MyMarketSorterFilter& other, unsigned int doNotCopyFlags) const
{
//	return m_sortQuotesByActivity == other.m_sortQuotesByActivity
//		&& m_sortPrintsByActivity == other.m_sortPrintsByActivity;
	return true;
}

void MyMarketSorterFilter::CopyMyFields(const MyMarketSorterFilter& other, unsigned int doNotCopyFlags)
{
//	m_sortQuotesByActivity = other.m_sortQuotesByActivity;
//	m_sortPrintsByActivity = other.m_sortPrintsByActivity;
}

void MyMarketSorterFilter::Copy(const TakionFilter& filter)
{
	MyStockFilter::Copy(filter);
//	const MarketSorterFilter& other = (const MarketSorterFilter&)filter;
	CopyMyFields((const MyMarketSorterFilter&)filter, 0);
}

void MyMarketSorterFilter::toString(std::string& str) const
{
	MyStockFilter::toString(str);
//	U_AppendBool(str, m_sortQuotesByActivity, settingDel);
//	U_AppendBool(str, m_sortPrintsByActivity, settingDel);
	str += settingSubdel;
}

void MyMarketSorterFilter::fromString(const char*& cursor, unsigned __int64 version, bool clone, unsigned int contentModifier)
{
	MyStockFilter::fromString(cursor, version, clone, contentModifier);
	bool endChange;
	char* end = U_MarkSectionEnd((char*)cursor, settingSubdel, endChange);

//	m_sortQuotesByActivity = U_ReadBoolToken(cursor, settingDel, true);
//	m_sortPrintsByActivity = U_ReadBoolToken(cursor, settingDel, true);

	if(endChange)*end = settingSubdel;
	cursor = end + 1;
}

bool MyMarketSorterFilter::SecurityPotentiallyBelongs(const Security* security) const
{
	if(MyStockFilter::SecurityPotentiallyBelongs(security))
	{
		return true;
	}
	return false;
}

bool MyMarketSorterFilter::SecurityBelongs(const SecurityChartRow* securityChartRow) const
{
	if(MyStockFilter::SecurityBelongs(securityChartRow))
	{
		return true;
	}
	return false;
}

unsigned char MyMarketSorterFilter::isPotentialFilterSame(const TakionFilter& filter) const
{
	unsigned char different = MyStockFilter::isPotentialFilterSame(filter);
	return different;
}

bool MyMarketSorterFilter::isFilterSame(const TakionFilter& filter) const
{
	bool same = MyStockFilter::isFilterSame(filter);
	if(same)
	{
		const MyMarketSorterFilter& other = (const MyMarketSorterFilter&)filter;
	}
	return same;
}

MyDialogMarketSorterFilter::MyDialogMarketSorterFilter(TakionMainWnd* mainWnd, bool toDestroyOnFocus, MyMarketSorter* receipient, CWnd* parent):
	MyDialogStockFilter(mainWnd,
		firstColumnOffset + firstColumnWidth + hgap,//xPrice
		topMost,//yPrice
		firstColumnOffset + firstColumnWidth + hgap + 8 * (groupWidth + hgap),//xSymbolOffset

		2 * (groupHeight + vgap) + 4 * (checkBoxHeight + vgap),//symbolHeight
		true,
		toDestroyOnFocus, receipient, parent,
		nullptr, WS_BORDER|WS_POPUP|WS_CLIPCHILDREN, 0, 0, 0,
		340 + firstColumnWidth + hgap + 5 * (groupWidth + hgap),
		350 + 3 * (groupHeight + vgap) + 2 * (checkBoxHeight + vgap),
		nullptr, 0, false, false)

//		caption, style, exstendedStyle, x, y, w, h, font, fontSize, executeCommands, closeOnCommandExecution)
/*
	MyDialogStockFilter(mainWnd, toDestroyOnFocus, receipient, parent, nullptr, WS_BORDER|WS_POPUP | WS_CLIPCHILDREN, 0, 0, 0,
		340 + firstColumnWidth + hgap + 5 * (groupWidth + hgap),
		350 + 3 * (groupHeight + vgap) + 2 * (checkBoxHeight + vgap),
		nullptr, 0, false, false)
*/
{
	m_filter = new MyMarketSorterFilter();
}

void MyDialogMarketSorterFilter::Display(const TakionFilter& filter, MyMarketSorter* receipient, unsigned int code, CWnd* parent, int x, int y)
{
	MyDialogStockFilter::Display(filter, receipient, code, parent, x, y, 0, 0, true);//w, h, show);
}

void MyDialogMarketSorterFilter::ToolTipsEnabled(bool enable)
{
	MyDialogStockFilter::ToolTipsEnabled(enable);
/*
	m_RadioQuotesActivity.EnableTooltips(enable, m_toolTip);
	m_RadioQuotesGoodBad.EnableTooltips(enable, m_toolTip);
	m_RadioPrintsActivity.EnableTooltips(enable, m_toolTip);
	m_RadioPrintsGoodBad.EnableTooltips(enable, m_toolTip);
*/
}

