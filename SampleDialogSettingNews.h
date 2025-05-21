#pragma once

#include "BaseDialogSetting.h"

class SampleDialogSettingNews;

class ListBoxNewsStock : public ListBoxStringAsUInt<unsigned __int64>
{
public:
	ListBoxNewsStock(SampleDialogSettingNews* owner) : ListBoxStringAsUInt(false, true, 0, true, true, false, false, nullptr), m_owner(owner){}
	virtual CBrush* GetBkBrush() const override
	{
		return m_itemCount > 0 ? nullptr : GetWarningBrush();
	}
	virtual bool StringToContents(const char* contents, size_t contentsLength) override;//returns true if successful
	virtual bool ContentsToString(std::string& contents) const override;
//	void CopySymbols(std::string& info) const;
	bool CreateBasketFilterString(std::string& filter) const;
	typedef std::map<std::string, std::string> StringMap;
	bool PasteStocks(StringMap& stringMap);//, bool replace);
	DialogTextEntry& DisplayStockEntryField();
	SampleDialogSettingNews* GetOwnerDialog(){return m_owner;}
	const SampleDialogSettingNews* GetOwnerDialog() const{return m_owner;}

	typedef std::set<ValueType, lessUIntAsStr<ValueType> > StockSet;
	bool isSameStockSet(const StockSet& stockSet) const;
	bool GetStockSet(StockSet& stockSet) const;
	bool SetStockSet(const StockSet& stockSet);

	int FindStock(const char* symbol);
	bool AddStock(const char* symbol);
	int RemoveCurrentStock();
	bool ClearStocks();
protected:
	void DoRemoveStockAt(int sel);
	virtual void AddMenuItems(CMenu& menu) override;
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	virtual int AddItemOnMenuCommand();
	virtual bool RemoveSelectedItemOnMenuCommand(int sel);
	virtual bool ClearItemsOnMenuCommand();
	afx_msg LRESULT OnStockAdd(WPARAM spinId, LPARAM l);
	DECLARE_MESSAGE_MAP()

	SampleDialogSettingNews* m_owner;
};

enum NesHeadlineColumnId : unsigned short
{
	NHCI_DATETIME,
	NHCI_DATETIME_PUBLISHED,
	NHCI_ORDINAL,
	NHCI_ALERT,
	NHCI_RELEVANCE,
	NHCI_WIRE,
	NHCI_SUBWIRE,
	NHCI_TYPE,
	NHCI_TEXT,
	NHCI_SYMBOL,
	NHCI_SYNOPSIS,
	NHCI_CODES,
	NHCI_PROVIDER_SERVICES,
	NHCI_SYMBOLS,
	NHCI_RESOURCE,

	NHCI_Count
};

class ListBoxNews : public ColumnedListBox
{
public:
	ListBoxNews():m_busy(false)
//		m_itemCountLeft(0),
//		m_itemCountRight(0)
	{}
	virtual bool HasItemToolTips() const override{return false;}

	virtual int Compare(const void* item1, const void* item2) const override;
	virtual void DestroyItem(const void* item) const override;
	virtual void* CloneItem(const void* item) const override;
	virtual CBrush* GetBkBrush() const override{return m_busy ? GetLoadBrush() : ColumnedListBox::GetBkBrush();}
	virtual COLORREF GetBkColor(const DRAWITEMSTRUCT* lpDrawItemStruct) const override{return m_busy ? GetLoadColor() : ColumnedListBox::GetBkColor(lpDrawItemStruct);}
	const bool& isBusy() const{return m_busy;}
	void SetBusy(const bool& busy)
	{
		if(busy != m_busy)
		{
			m_busy = busy;
			if(m_hWnd)Invalidate(FALSE);
		}
	}
protected:
	virtual LbHorizontalScroller* CreateHorizontalScroller() override;
	virtual void DrawLine(unsigned short columnId, const void* item, UINT itemOrdinal, HDC hDC, CRect& r, unsigned int alignment) override;
/*
	virtual void HorizontalScrollingDone(int diff) override
	{
		m_itemCountLeft += diff;
		m_itemCountRight += diff;
	}
	virtual void ListBoxCreated() override
	{
		ColumnedListBox::ListBoxCreated();
		m_horizontalScroller->GetColumnBounds(NHCI_DATETIME, m_itemCountLeft, m_itemCountRight);
	}
	int m_itemCountLeft;
	int m_itemCountRight;
*/
	bool m_busy;
};

class UnappliedHeadlineReader : public LineBufferArrayReader
{
public:
	UnappliedHeadlineReader(SampleDialogSettingNews* window, const LineBufferArray* bufferArray = nullptr):LineBufferArrayReader(bufferArray), m_window(window){}
	virtual bool ProcessDataToken(const char* cursor, unsigned short length) const;
protected:
	SampleDialogSettingNews* m_window;
};

class SampleDialogSettingNews : public BaseDialogSetting, public Observer
{
public:
	SampleDialogSettingNews(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab);
	virtual HWND GetFirstTabControl() const override;
	virtual void UpdateControls() override;
	virtual void Leaving() override;

	virtual void UpdateSettings() override;
	virtual void RestoreLayout() override;

	unsigned char ObtainSearchType() const;

	bool ProcessUnappliedMessage(const Message* message);

//	static const int spinWidth = 32;
	static const int itemHeight = 8;

	virtual void GetTitleRect(const ListBoxOwnerDraw* listBox, CRect& titleRect, CRect& totalRect) const override;
	virtual const char* GetTotalValue(unsigned int listBoxId, unsigned short columnId, char* buf, unsigned int buflen) const override;

	const WireSet& GetWireSet() const{return m_wireSet;}
	const unsigned int& GetStoryFilter() const{return m_storyFilter;}
	const unsigned int& GetMetaFilter() const{return m_metaFilter;}
	const unsigned int& GetRelevance() const{return m_relevance;}

	void UpdateWireList();
//	void UpdateTopicList();

#if (_MSC_VER > 1600)
	typedef std::unordered_map<unsigned __int64, unsigned short> UInt64UShortMap;
	typedef std::unordered_map<unsigned __int64, NewsWire> WireMap;
	typedef std::unordered_set<unsigned __int64> HashStockSet;
#else
	typedef std::hash_map<unsigned __int64, unsigned short> UInt64UShortMap;
	typedef std::hash_map<unsigned __int64, NewsWire> WireMap;
	typedef std::hash_set<unsigned __int64> HashStockSet;
#endif
	typedef std::map<std::string, std::string> StringMap;

	const UInt64UShortMap& GetNewsWireMap() const{return m_newsWireMap;}
	const UInt64UShortMap& GetNewsWireEntitledMap() const{return m_newsWireEntitledMap;}
	const StringMap& GetNewsTopicMap() const{return m_newsTopicMap;}
	unsigned short FindNewsWire(unsigned __int64 id) const{UInt64UShortMap::const_iterator found = m_newsWireMap.find(id); return found == m_newsWireMapEnd ? 0xFFFF : found->second;}
	unsigned short FindNewsWireEntitled(unsigned __int64 id) const{UInt64UShortMap::const_iterator found = m_newsWireEntitledMap.find(id); return found == m_newsWireEntitledMapEnd ? 0xFFFF : found->second;}
	const std::string* FindNewsTopic(const std::string& name) const{StringMap::const_iterator found = m_newsTopicMap.find(name); return found == m_newsTopicMapEnd ? nullptr : &found->second;}

	unsigned int GetAllWireCount() const{return (unsigned int)m_allWireNames.size();}
	const WireMap& GetAllWireNames() const{return m_allWireNames;}

//	bool isStockSetChanged() const;
	bool isBaseChanged() const;
	bool isChanged() const;
	bool isSubscriptionChanged() const;

	bool isValid() const;

	const int& GetNewsStockLeftOffset() const{return m_stockLeftOffset;}
	const int& GetNewsStockTopOffset() const{return m_stockTopOffset;}
	void InvalidateStockCountRect() const{::InvalidateRect(m_hWnd, &m_stockCountRect, FALSE);}
	void OnStocksChanged();

	const std::string& GetBasketName() const{return m_basketName;}
	const std::string& GetBasketFilter() const{return m_basketFilter;}
	bool CreateBasketFilterString(std::string& filter, HashStockSet* hashStockSet = nullptr) const;
	bool HeadlineBelongs(const NewsHeadline* headline) const;
protected:
	virtual void TotalScrolledHorizontally(unsigned int listBoxId, const CRect& titleRect, int diff) override;

	virtual void ToolTipsEnabled(bool enable) override;

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void BeforeDestroy() override;
	virtual void Resize(bool widthChanged = true, bool heightChanged = true) override;
    virtual void DoPaint(const RECT& rcPaint, CDC& dc) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	
	bool DoReset();
	bool ResetControls();

	void StartSearch();
	void StopSearch();

	NewsDll* m_newsDll;
	Observable* m_newsCollection;
	Observable* m_stockNews;

	WireMap m_allWireNames;
	WireMap::const_iterator m_allWireNamesEnd;

	UInt64UShortMap m_newsWireMap;
	UInt64UShortMap::const_iterator m_newsWireMapEnd;

	UInt64UShortMap m_newsWireEntitledMap;
	UInt64UShortMap::const_iterator m_newsWireEntitledMapEnd;

	StringMap m_newsTopicMap;
	StringMap::const_iterator m_newsTopicMapEnd;

	WireSet m_wireSet;
	WireSet::const_iterator m_wireSetEnd;

	WireSet m_entitledWireSet;
	WireSet::const_iterator m_entitledWireSetEnd;

	unsigned int m_storyFilter;
	unsigned int m_metaFilter;
	unsigned int m_relevance;

	unsigned int m_currentRelevance;

	CButton m_CheckBoxAllEntitled;
//	CStatic m_StaticWire;
	ListBoxNewsWire m_ListBoxWire;
	ListBoxFilterString<unsigned int, 3> m_ListBoxStory;
	ListBoxFilterString<unsigned int, 2> m_ListBoxMeta;
	
	UnsignedNumberTypeSpin<unsigned short> m_SpinDaysBack;
	CButton m_CheckBoxAfterMarket;
	UnsignedNumberTypeSpin<unsigned int> m_SpinCountLimit;
	
	ComboBoxString m_ComboBoxTopic;

	ListBoxNewsStock m_ListBoxStock;
	ValidEdit m_EditSymbol;
	CButton m_ButtonStockFind;
	CButton m_ButtonStockAdd;
	CButton m_ButtonStockRemove;
	CButton m_ButtonStockClear;

	CButton m_CheckBoxShowStoryChain;
	CButton m_ButtonRequestStory;
	CEdit m_EditStoryResource;
	CEdit m_EditStory;

	CButton m_RadioUnfiltered;
	CButton m_RadioAll;
	CButton m_RadioAllUs;
	CButton m_RadioTopic;
	CButton m_RadioStock;

	ListBoxNews m_ListBoxNews;

	CButton m_ButtonSubscribe;
	CButton m_ButtonReset;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeNews();
	afx_msg void OnSelchangeTopic();
	afx_msg void OnSearchType();
	afx_msg void OnAllEntitled();
	afx_msg void OnSubscribe();
	afx_msg void OnReset();
	afx_msg void OnSymbolUpdate();
	afx_msg void OnSymbolChange();
	afx_msg void OnSelchangeStock();
	afx_msg void OnDblClkStock();
	afx_msg void OnStockFind();
	afx_msg void OnStockAdd();
	afx_msg void OnStockRemove();
	afx_msg void OnStockClear();
	afx_msg void OnRequestStory();
	afx_msg void OnShowStoryChain();

	afx_msg void OnAfterMarket();

	afx_msg LRESULT OnListBoxChecked(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSpinChange(WPARAM spinId, LPARAM l);
	DECLARE_MESSAGE_MAP()

	bool AddCurrentStock();

	void EnableSubscribeButton();

	void ResetOffsets();
	int m_listBoxBorderSize2;
	int m_listBoxBorderSize;
	int m_leftOffset;
	int m_titleOffset;
	int m_topOffset;
	int m_bottomOffset;

	int m_stockWidth;
	int m_stockLeftOffset;
	int m_stockTopOffset;
	int m_maxStockHeight;
	int m_stockHeight;

	int m_storyWidth;
	int m_storyTopOffset;

	CRect m_stockCountRect;

	void ClearHeadlines();

	void UpdateHeadlines();
	bool DoAddHeadline(const TMsgHeadline* msg);
	bool AddHeadlineToListBox(const NewsHeadline* headline);//returns true if the headline is added to the visible area of the List Box
	void TrimHeadlineCount(bool added);
//	void DisplayLastHistoricStory();

	void ProfilesLoaded();
//	void SetNoProfile(bool noProfile);
	void SetNewsConnected(bool connected);
	bool SetStockNews(Observable* stockNews, bool cleanup);
	void UpdateStockNews();
	void PopulateStockNews();
	void UpdateNewsCollectionFilter();
	void ReleaseNewsCollection();
	std::string m_symbol;
	std::string m_basketName;
	std::string m_basketFilter;
	bool m_basket;
	bool m_newsConnected;
//	bool m_noProfile;

	void RequestStory(const NewsHeadline* headline);
	bool DoRequestStory(const NewsHeadline* headline);

	void DisplayStoryText(const char* resourceId, const std::string& story, const bool error);
	bool UpdateSelectedLineStoryDisplayed();
	bool m_showStoryChain;
//	bool m_displayLastHistoricStory;
	bool m_selectedHeadlineStoryDisplayed;
	bool m_storyError;
	std::string m_storyRequestId;
	std::string m_displayedStoryResourceId;

	const std::string* UpdateSearchFilter(std::string& topicFilter);
	void UpdateAllEntitledWireSet();
	void UpdateSelectedTopic();
	bool SetSearchType(const unsigned char& searchType);
	unsigned char m_searchType;
	unsigned char m_currentSearchType;
	bool m_allEntitled;
	unsigned short m_daysBack;
//	unsigned short m_currentDaysBack;
	unsigned int m_headlineCount;
//	unsigned int m_currentHeadlineCount;
	bool m_afterMarket;
	bool m_currentAfterMarket;

	bool m_addedVisibleHeadline;
//	std::string m_topic;
//	std::string m_currentTopic;
	unsigned short m_topicOrdinal;
	unsigned short m_currentTopicOrdinal;

	ListBoxNewsStock::StockSet m_stockSet;
	ListBoxNewsStock::StockSet::const_iterator m_stockSetEnd;

//	HashStockSet m_hashStockSet;//to pass to the NewsDll

	UnappliedHeadlineReader m_unappliedHeadlineReader;

	virtual void Notify(const Message* message, const Observable* from, const Message* info) override;
};
