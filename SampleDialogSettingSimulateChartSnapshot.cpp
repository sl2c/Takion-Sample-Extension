#include "stdafx.h"
#include "SampleDialogSettingSimulateChartSnapshot.h"
#include "ExtensionDerivedConfig.h"

extern ExtensionDerivedConfig* extensionConfig;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SampleDialogSettingSimulateChartSnapshot::SampleDialogSettingSimulateChartSnapshot(TakionMainWnd* mainWnd, TakionSettingTabDialog* parentTab):
	BaseDialogSettingSimulateSymbolMap(mainWnd, parentTab, "ChartSnapshot"),
	m_SpinRequestId(999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_SpinPointCount(1000),
	m_SpinStartMinute(1439, 0, 0, 10),
	m_SpinPriceStart(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPriceHigh(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPriceLow(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinPriceEnd(9999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinMoneyTraded(99999999999, 0, 0, 10000, 3, TakionSpin::charComma, 4),
	m_SpinVolume(99999999, 0, 0, 1000, 3, TakionSpin::charComma),
	m_requestId(0),
	m_pointCount(0),
	m_startMinute(0),
	m_volume(0),
	m_done(false),
	m_maxStockPointCount((0xFFFF - sizeof(m_msgStock)) / sizeof(m_chartPointStock)),
#ifndef TAKION_NO_OPTIONS
	m_maxFuturePointCount((0xFFFF - sizeof(m_msgFuture)) / sizeof(m_chartPointFuture)),
#endif
	m_buffer(nullptr)
{
	SetSizeSpinCells(m_SpinRequestId);

//	m_msgStock.m_length = sizeof(m_msgStock) + sizeof(m_chartPointStock);
//	m_msgFuture.m_length = sizeof(m_msgFuture) + sizeof(m_chartPointFuture);

	m_SpinPointCount.AddSpinCell(1, false);
	m_SpinPointCount.AddSpinCell(10, true);

	SetBigSizeSpinCells(m_SpinVolume);
	SetPriceSpinCells(m_SpinPriceStart);
	SetPriceSpinCells(m_SpinPriceHigh);
	SetPriceSpinCells(m_SpinPriceLow);
	SetPriceSpinCells(m_SpinPriceEnd);
	SetPriceSpinCells(m_SpinMoneyTraded);

//	AddListBox(&m_ListBoxSnapshotFlags);

	m_bottomControl += vgap * 4;
	const int sideWidth = 44;
	const int priceWidth = 82;// 75;

	int controlWidth = m_xc + m_firstColumnWidth + hgap;
	const int w = controlWidth;

//	controlWidth += sideWidth;
//	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;

	controlWidth = m_xc + priceWidth + hgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartSnapshotRequestIdId);
	AddStaticIndirect("RequestId", WS_VISIBLE|SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartSnapshotRequestIdId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartSnapshotPointCountId);
	AddStaticIndirect("PointCount", WS_VISIBLE|SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartSnapshotPointCountId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartSnapshotStartMinuteId);
	AddStaticIndirect("StartMinute", WS_VISIBLE|SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartSnapshotStartMinuteId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartSnapshotPriceStartId);
	AddStaticIndirect("StartPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartSnapshotPriceStartId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartSnapshotPriceHighId);
	AddStaticIndirect("HighPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartSnapshotPriceHighId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartSnapshotPriceLowId);
	AddStaticIndirect("LowPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartSnapshotPriceLowId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartSnapshotPriceEndId);
	AddStaticIndirect("EndPrice", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartSnapshotPriceEndId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartSnapshotMoneyTradedId);
	AddStaticIndirect("MoneyTraded", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartSnapshotMoneyTradedId);
	m_bottomControl += spinHeight + vgap;

	AddComponentIndirect(TakionSpin::takionSpinClassName, nullptr, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, m_xc, m_bottomControl, priceWidth, spinHeight, spinSimChartSnapshotVolumeId);
	AddStaticIndirect("Volume", WS_VISIBLE | SS_NOPREFIX, 0, controlWidth, m_bottomControl + (spinHeight - staticHeight) / 2, sideWidth, staticHeight, staticSimChartSnapshotVolumeId);
	m_bottomControl += spinHeight + vgap;

	AddButtonIndirect("Done", WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX, 0, m_xc, m_bottomControl, priceWidth / 2, checkBoxHeight, checkBoxSimChartSnapshotDoneId);

	controlWidth += sideWidth;
	if(m_maxControlWidth < controlWidth)m_maxControlWidth = controlWidth;
}

SampleDialogSettingSimulateChartSnapshot::~SampleDialogSettingSimulateChartSnapshot()
{
	if(m_buffer)delete[] m_buffer;
}

BOOL SampleDialogSettingSimulateChartSnapshot::OnInitDialog()
{
	m_priceStart.SetZero();
	m_priceHigh.SetZero();
	m_priceLow.SetZero();
	m_priceEnd.SetZero();
	m_moneyTraded.SetZero();
	m_requestId = 0;
	m_pointCount = 0;
	m_startMinute = 0;
	m_volume = 0;
	m_done = false;

	BOOL ret = BaseDialogSettingSimulateSymbolMap::OnInitDialog();

	if(m_done)
	{
		m_CheckBoxDone.SetCheck(BST_CHECKED);
	}

	return ret;
}

void SampleDialogSettingSimulateChartSnapshot::UpdateSettings()
{

	unsigned char symbolType = m_symbolType;
	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
	extensionConfig->m_simChartSnapshotSymbolType.SetValue(symbolType);

	extensionConfig->m_simChartSnapshotSymbol.SetValue(m_symbol);
	extensionConfig->m_simChartSnapshotSymbolMap.SetValue(m_symbolMap);

//	if(symbolType >= SYMT_SYMBOL_Count)symbolType = SYMT_SYMBOL_Count - 1;
//	extensionConfig->m_simChartSnapshotSymbol.SetValue(m_symbol);

	extensionConfig->m_simChartSnapshotRequestId.SetValue(m_requestId);
	extensionConfig->m_simChartSnapshotPointCount.SetValue(m_pointCount);
	extensionConfig->m_simChartSnapshotStartMinute.SetValue(m_startMinute);
	extensionConfig->m_simChartSnapshotPriceStart.SetValue(m_priceStart);
	extensionConfig->m_simChartSnapshotPriceHigh.SetValue(m_priceHigh);
	extensionConfig->m_simChartSnapshotPriceLow.SetValue(m_priceLow);
	extensionConfig->m_simChartSnapshotPriceEnd.SetValue(m_priceEnd);
	extensionConfig->m_simChartSnapshotMoneyTraded.SetValue(m_moneyTraded);
	extensionConfig->m_simChartSnapshotVolume.SetValue(m_volume);
	extensionConfig->m_simChartSnapshotDone.SetValue(m_done);

//	extensionConfig->m_simChartSnapshotFlags.SetValue(m_ListBoxSnapshotFlags.GetFilter());

	EnableApplyButton(false);
}

void SampleDialogSettingSimulateChartSnapshot::RestoreLayout()
{
	const std::string& symbol = extensionConfig->m_simChartSnapshotSymbol.GetValue();
	if(symbol != m_symbol)
	{
		m_symbol = symbol;
		m_EditSymbol.SetWindowText(symbol.c_str());
		m_EditSymbol.Invalidate(FALSE);
	}

	unsigned char symbolType = extensionConfig->m_simChartSnapshotSymbolType.GetValue();
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
	const unsigned short& symbolMap = extensionConfig->m_simChartSnapshotSymbolMap.GetValue();
	if(symbolMap != m_symbolMap)
	{
		m_symbolMap = symbolMap;
		m_SpinSymbolMap.SetValue(m_symbolMap, false, true, false);
	}

	const unsigned short& requestId = extensionConfig->m_simChartSnapshotRequestId.GetValue();
	if(requestId != m_requestId)
	{
		m_requestId = requestId;
		m_SpinRequestId.SetValue(m_requestId, false);
	}

	const unsigned short& pointCount = extensionConfig->m_simChartSnapshotPointCount.GetValue();
	if(pointCount != m_pointCount)
	{
		m_pointCount = pointCount;
		m_SpinPointCount.SetValue(m_pointCount, false);
	}

	const unsigned short& startMinute = extensionConfig->m_simChartSnapshotStartMinute.GetValue();
	if(startMinute != m_startMinute)
	{
		m_startMinute = startMinute;
		m_SpinStartMinute.SetMinute(m_startMinute, false);
	}

	const Price& priceStart = extensionConfig->m_simChartSnapshotPriceStart.GetValue();
	if(priceStart != m_priceStart)
	{
		m_priceStart = priceStart;
		FillSpinControlFromPrice(m_priceStart, m_SpinPriceStart, 10000, false);
	}

	const Price& priceHigh = extensionConfig->m_simChartSnapshotPriceHigh.GetValue();
	if(priceHigh != m_priceHigh)
	{
		m_priceHigh = priceHigh;
		FillSpinControlFromPrice(m_priceHigh, m_SpinPriceHigh, 10000, false);
	}

	const Price& priceLow = extensionConfig->m_simChartSnapshotPriceLow.GetValue();
	if(priceLow != m_priceLow)
	{
		m_priceLow = priceLow;
		FillSpinControlFromPrice(m_priceLow, m_SpinPriceLow, 10000, false);
	}

	const Price& priceEnd = extensionConfig->m_simChartSnapshotPriceEnd.GetValue();
	if(priceEnd != m_priceEnd)
	{
		m_priceEnd = priceEnd;
		FillSpinControlFromPrice(m_priceEnd, m_SpinPriceEnd, 10000, false);
	}

	const Price& moneyTraded = extensionConfig->m_simChartSnapshotMoneyTraded.GetValue();
	if(moneyTraded != m_moneyTraded)
	{
		m_moneyTraded = moneyTraded;
		FillSpinControlFromPrice(m_moneyTraded, m_SpinMoneyTraded, 10000, false);
	}

	const unsigned int& volume = extensionConfig->m_simChartSnapshotVolume.GetValue();
	if(volume != m_volume)
	{
		m_volume = volume;
		m_SpinVolume.SetValue(m_volume, false, true, false);
	}

	const bool done = extensionConfig->m_simChartSnapshotDone.GetValue();
	m_CheckBoxDone.SetCheck(done ? BST_CHECKED : BST_UNCHECKED);
	m_done = done;

//	m_ListBoxSnapshotFlags.SetFilter(extensionConfig->m_simChartSnapshotFlags.GetValue());

	EnableApplyButton(false);
	EnableSendMessageButton();

	m_modified = false;
}

const Message* SampleDialogSettingSimulateChartSnapshot::CreateMessage(WSABUF& wsaBuf)
{
	if(isSendMessageButtonEnabled() && m_symbol.length())
	{
		const char* symbol = m_symbol.c_str();
#ifndef TAKION_NO_OPTIONS
		unsigned __int64 futureNumericSymbol;
		bool future = *symbol == '/';
		if(future)
		{
			futureNumericSymbol = TU_GetCurrentFutureSymbol(symbol + 1);
			if(futureNumericSymbol)
			symbol = (const char*)&futureNumericSymbol;
		}
		else
		{
			future = TU_IsFutureSymbol(symbol) != 0;
		}
		if(future)
		{
			U_CopyAndPad(m_msgFuture.m_symbol, sizeof(m_msgFuture.m_symbol), symbol, '\0');

			m_msgFuture.m_requestId = m_requestId;
			m_msgFuture.m_done = m_done;
			m_msgFuture.m_minute = m_startMinute;

			unsigned short pointCount = m_pointCount;
			if(pointCount > m_maxFuturePointCount)pointCount = m_maxFuturePointCount;

			m_msgFuture.m_length = sizeof(m_msgFuture);// + pointCount * sizeof(m_chartPointFuture);

			if(pointCount)
			{
				m_chartPointFuture.m_first = m_priceStart;
				m_chartPointFuture.m_last = m_priceEnd;
				m_chartPointFuture.m_high = m_priceHigh;
				m_chartPointFuture.m_low = m_priceLow;
//				m_chartPointFuture.m_moneySpent = m_moneyTraded;
				m_chartPointFuture.m_volume = m_volume;
				if(pointCount == 1)
				{
					wsaBuf.buf = (char*)&m_chartPointFuture;
					wsaBuf.len = sizeof(m_chartPointFuture);
				}
				else
				{
					if(!m_buffer)
					{
						m_buffer = new char[0xFFFF];
					}
					const char* src = (const char*)&m_chartPointFuture;
					char* cursor = m_buffer;
					unsigned short remainingSize = sizeof(m_buffer);
					unsigned short i = 0;
					for(; i < pointCount && remainingSize >= sizeof(m_chartPointFuture); ++i, cursor += sizeof(m_chartPointFuture))
					{
						memcpy(cursor, src, sizeof(m_chartPointFuture));
						remainingSize -= sizeof(m_chartPointFuture);
					}
					wsaBuf.buf = m_buffer;
					wsaBuf.len = sizeof(m_chartPointFuture) * i;
				}
				m_msgFuture.m_length += (unsigned short)wsaBuf.len;
			}
			return &m_msgFuture;
		}
		else
		{
#endif
			if(m_symbolType == SYMT_SYMBOL_STRING)
			{


				U_CopyAndPad(m_msgStock.m_symbol, sizeof(m_msgStock.m_symbol), symbol, '\0');

				m_msgStock.m_requestId = m_requestId;
				m_msgStock.m_done = m_done;
				m_msgStock.m_minute = m_startMinute;

				unsigned short pointCount = m_pointCount;
				if(pointCount > m_maxStockPointCount)pointCount = m_maxStockPointCount;

				m_msgStock.m_length = sizeof(m_msgStock);// + pointCount * sizeof(m_chartPointStock);

				if(pointCount)
				{
					m_chartPointStock.m_firstPrice = m_priceStart.GetCompactPrice();
					m_chartPointStock.m_lastPrice = m_priceEnd.GetCompactPrice();
					m_chartPointStock.m_highPrice = m_priceHigh.GetCompactPrice();
					m_chartPointStock.m_lowPrice = m_priceLow.GetCompactPrice();
					m_chartPointStock.m_moneySpent = m_moneyTraded;
					m_chartPointStock.m_volume = m_volume;
					if(pointCount == 1)
					{
						wsaBuf.buf = (char*)&m_chartPointStock;
						wsaBuf.len = sizeof(m_chartPointStock);
					}
					else
					{
						if(!m_buffer)
						{
							m_buffer = new char[0xFFFF];
						}
						const char* src = (const char*)&m_chartPointStock;
						char* cursor = m_buffer;
						unsigned short remainingSize = sizeof(m_buffer);
						unsigned short i = 0;
						for(; i < pointCount && remainingSize >= sizeof(m_chartPointStock); ++i, cursor += sizeof(m_chartPointStock))
						{
							memcpy(cursor, src, sizeof(m_chartPointStock));
							remainingSize -= sizeof(m_chartPointStock);
						}
						wsaBuf.buf = m_buffer;
						wsaBuf.len = sizeof(m_chartPointStock) * i;
					}
					m_msgStock.m_length += (unsigned short)wsaBuf.len;
				}
				return &m_msgStock;
			}
			else
			{
				m_msgSmStock.m_symbol = m_symbolMap;
				m_msgSmStock.m_requestId = m_requestId;
				m_msgSmStock.m_done = m_done;
				m_msgSmStock.m_minute = m_startMinute;

				unsigned short pointCount = m_pointCount;
				if(pointCount > m_maxStockPointCount)pointCount = m_maxStockPointCount;

				m_msgSmStock.m_length = sizeof(m_msgSmStock);// + pointCount * sizeof(m_chartPointStock);

				if(pointCount)
				{
					m_chartPointStock.m_firstPrice = m_priceStart.GetCompactPrice();
					m_chartPointStock.m_lastPrice = m_priceEnd.GetCompactPrice();
					m_chartPointStock.m_highPrice = m_priceHigh.GetCompactPrice();
					m_chartPointStock.m_lowPrice = m_priceLow.GetCompactPrice();
					m_chartPointStock.m_moneySpent = m_moneyTraded;
					m_chartPointStock.m_volume = m_volume;
					if(pointCount == 1)
					{
						wsaBuf.buf = (char*)&m_chartPointStock;
						wsaBuf.len = sizeof(m_chartPointStock);
					}
					else
					{
						if(!m_buffer)
						{
							m_buffer = new char[0xFFFF];
						}
						const char* src = (const char*)&m_chartPointStock;
						char* cursor = m_buffer;
						unsigned short remainingSize = sizeof(m_buffer);
						unsigned short i = 0;
						for(; i < pointCount && remainingSize >= sizeof(m_chartPointStock); ++i, cursor += sizeof(m_chartPointStock))
						{
							memcpy(cursor, src, sizeof(m_chartPointStock));
							remainingSize -= sizeof(m_chartPointStock);
						}
						wsaBuf.buf = m_buffer;
						wsaBuf.len = sizeof(m_chartPointStock) * i;
					}
					m_msgSmStock.m_length += (unsigned short)wsaBuf.len;
				}
				return &m_msgSmStock;
			}
#ifndef TAKION_NO_OPTIONS
		}
#endif
	}
	return nullptr;
}

void SampleDialogSettingSimulateChartSnapshot::DoDataExchange(CDataExchange* pDX)
{
	BaseDialogSettingSimulateSymbolMap::DoDataExchange(pDX);

	DDX_Control(pDX, spinSimChartSnapshotRequestIdId, m_SpinRequestId);
	DDX_Control(pDX, spinSimChartSnapshotPointCountId, m_SpinPointCount);
	DDX_Control(pDX, spinSimChartSnapshotStartMinuteId, m_SpinStartMinute);
	DDX_Control(pDX, spinSimChartSnapshotPriceStartId, m_SpinPriceStart);
	DDX_Control(pDX, spinSimChartSnapshotPriceHighId, m_SpinPriceHigh);
	DDX_Control(pDX, spinSimChartSnapshotPriceLowId, m_SpinPriceLow);
	DDX_Control(pDX, spinSimChartSnapshotPriceEndId, m_SpinPriceEnd);
	DDX_Control(pDX, spinSimChartSnapshotMoneyTradedId, m_SpinMoneyTraded);
	DDX_Control(pDX, spinSimChartSnapshotVolumeId, m_SpinVolume);
	DDX_Control(pDX, checkBoxSimChartSnapshotDoneId, m_CheckBoxDone);
}

BEGIN_MESSAGE_MAP(SampleDialogSettingSimulateChartSnapshot, BaseDialogSettingSimulateSymbolMap)
	ON_BN_CLICKED(checkBoxSimChartSnapshotDoneId, OnDone)
	ON_MESSAGE(WM_USER + 15, OnSpinChange)
END_MESSAGE_MAP()

void SampleDialogSettingSimulateChartSnapshot::SetDone(const bool done)
{
	if(done != m_done)
	{
		m_done = done;
		m_modified = true;
	}
}

void SampleDialogSettingSimulateChartSnapshot::OnDone()
{
	SetDone(m_CheckBoxDone.GetCheck() == BST_CHECKED);
}

LRESULT SampleDialogSettingSimulateChartSnapshot::OnSpinChange(WPARAM w, LPARAM l)
{
	switch(w)
	{
		case spinSimChartSnapshotPriceStartId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceStart, 10000);
//			bool changed = m_priceStart.isZero() != price.isZero();
			if(price != m_priceStart)
			{
				m_priceStart = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartSnapshotPriceHighId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceHigh, 10000);
//			bool changed = m_priceHigh.isZero() != price.isZero();
			if(price != m_priceHigh)
			{
				m_priceHigh = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartSnapshotPriceLowId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceLow, 10000);
//			bool changed = m_priceHigh.isZero() != price.isZero();
			if(price != m_priceLow)
			{
				m_priceLow = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartSnapshotPriceEndId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinPriceEnd, 10000);
//			bool changed = m_priceEnd.isZero() != price.isZero();
			if(price != m_priceEnd)
			{
				m_priceEnd = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartSnapshotMoneyTradedId:
		{
			Price price;
			FillPriceFromSpinControl(price, m_SpinMoneyTraded, 10000);
//			bool changed = m_priceEnd.isZero() != price.isZero();
			if(price != m_moneyTraded)
			{
				m_moneyTraded = price;
				m_modified = true;
			}
//			if(changed)EnableSendMessageButton();
		}
		break;

		case spinSimChartSnapshotVolumeId:
		{
			const unsigned int& volume = m_SpinVolume.GetValue();
			if(volume != m_volume)
			{
				m_volume = volume;
				m_modified = true;
			}
		}
		break;

		case spinSimChartSnapshotRequestIdId:
		{
			const unsigned int& requestId = m_SpinRequestId.GetValue();
			if(requestId != m_requestId)
			{
				m_requestId = requestId;
				m_modified = true;
			}
		}
		break;

		case spinSimChartSnapshotStartMinuteId:
		{
			const unsigned short& minute = m_SpinStartMinute.GetMinute();
			if(minute != m_startMinute)
			{
				m_startMinute = minute;
				m_modified = true;
			}
		}
		break;

		case spinSimChartSnapshotPointCountId:
		{
			const unsigned short& pointCount = m_SpinPointCount.GetValue();
			if(pointCount != m_pointCount)
			{
				m_pointCount = pointCount;
				m_modified = true;
			}
		}
		break;

		default:
		BaseDialogSettingSimulateSymbolMap::OnSpinChange(w, l);
		break;
	}
	return 0;
}

