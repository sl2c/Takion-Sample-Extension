#pragma once

#include "ConfigEnums.h"

class ListBoxSnapshotFlags : public ListBoxSimpleFilterSameColor<unsigned char, 8>
{
public:
	ListBoxSnapshotFlags():ListBoxSimpleFilterSameColor((1 << SF_Count) - 1, SF_Count, SF_Count - 1){}
	virtual const char* GetItemName(const void* item) const override
	{
		switch((unsigned char)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			item)
		{
			case SF_BLOCK_END:
			return "BlockEnd";

			case SF_REFRESH_END:
			return "RefreshEnd";

			case SF_FAILED:
			return "Failed";

			default:
			return "";
		}
	}
	unsigned char GetRefreshFlags() const
	{
		unsigned char ret = 0;
		
		if(m_filter & (1 << SF_BLOCK_END))ret |= REFRESH_EndOfBlock;
		if(m_filter & (1 << SF_REFRESH_END))ret |= REFRESH_EndOfRefresh;
		if(m_filter & (1 << SF_FAILED))ret |= REFRESH_RefreshFailed;

		return ret;
	}
};

