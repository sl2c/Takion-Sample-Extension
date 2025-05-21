#pragma once

enum FilterOrderStatus : unsigned char
{
	FOST_PENDING,
	FOST_FILLED,
	FOST_UNFILLED,

	FOST_Count
};

enum FilterOrderSide : unsigned char
{
	FOS_BUY,
	FOS_SELL,
	FOS_SHORT,

	FOS_Count
};
/*
enum FilterPositionInstrument : unsigned char
{
	FPI_STOCK,
#ifndef TAKION_NO_OPTIONS
	FPI_OPTION,
	FPI_FUTURE,
#endif
	FPI_Count
};
*/
enum SnapshotFlags : unsigned char
{
	SF_BLOCK_END,
	SF_REFRESH_END,
	SF_FAILED,

	SF_Count
};

enum SearchType : unsigned char
{
	SEA_ALL,
	SEA_ALL_US,
	SEA_TOPIC,
	SEA_STOCK,
	SEA_UNFILTERED,

	SEA_Count
};

class ListBoxInstrument : public ListBoxSimpleFilter<unsigned char, 8>
{
public:
	ListBoxInstrument():ListBoxSimpleFilter((1 << ST_COUNT) - 1, ST_COUNT){}
	virtual const char* GetItemName(const void* item) const override
	{
		switch((unsigned char)
#if (_MSC_VER > 1600)
			(size_t)
#endif
			item)
		{
			case ST_STOCK:
			return "Stock";

#ifndef TAKION_NO_OPTIONS
			case ST_OPTION:
			return "Option";

			case ST_FUTURE:
			return "Future";
#endif

			default:
			return "";
		}
	}
};

