/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#ifndef SELECTED_REGION_H
#define SELECTED_REGION_H

#include "stdafx.h"
#include <afxtempl.h>
#include "..\shared\musictimeconverter.h"

class CListSelectedRegion;

class CSelectedRegion
{
	friend class CListSelectedRegion;
	CMusicTimeConverter m_mtcBeg;
	CMusicTimeConverter m_mtcEnd;
	DWORD	m_dwFlags;
public:
	// if m_dwFlags == WasSelected, then region was selected before click select.  If successful drag drop
	// then remains selected, otherwise its deselected.  
	// Used by the pair CSelectedRegionList::DelayToggle/CompleteToggle
	enum { WasToggleSelected=1, WasSelected=2 };
	CSelectedRegion()
	{
		m_dwFlags = 0;
	}
	~CSelectedRegion()
	{
	}
	CSelectedRegion(CMusicTimeConverter beg, CMusicTimeConverter end)
		: m_mtcBeg(beg), m_mtcEnd(end)
	{
		m_dwFlags = 0;
	}
	CSelectedRegion(const CSelectedRegion& sel)
	{
		m_mtcBeg = sel.m_mtcBeg;
		m_mtcEnd = sel.m_mtcEnd;
		m_dwFlags = sel.m_dwFlags;
	}
	CSelectedRegion& operator=(const CSelectedRegion& sel)
	{
		m_mtcBeg = sel.m_mtcBeg;
		m_mtcEnd = sel.m_mtcEnd;
		m_dwFlags = sel.m_dwFlags;
		return *this;
	}
	CMusicTimeConverter& Beg()
	{
		return m_mtcBeg;
	}
	CMusicTimeConverter& End()
	{
		return m_mtcEnd;
	}
	DWORD& Flags()
	{
		return m_dwFlags;
	}
	// create
	CSelectedRegion(long position, long measure, long beat, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits)
	{
		m_mtcBeg.SetTimeUsingPosition(position, pTimeline, dwGroupBits);
		DMUS_TIMESIGNATURE dmTimeSig;
		m_mtcBeg.GetTimeSig(dmTimeSig, pTimeline, dwGroupBits);
		long m,b;
		m_mtcBeg.GetMeasureBeat(m, b, pTimeline, dwGroupBits);
		long beats = beat + b;
		beat = beats % dmTimeSig.bBeatsPerMeasure;
		measure += m + beats/dmTimeSig.bBeatsPerMeasure;
		m_mtcEnd.SetTime(measure, beat, pTimeline, dwGroupBits);
		m_dwFlags = 0;
	}
	bool Contains(CSelectedRegion& sel)
	{
		if(m_mtcBeg <= sel.m_mtcBeg && m_mtcEnd >= sel.m_mtcEnd)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool Contains(long measure, long beat, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits)
	{
		long ltime;
		HRESULT hr = pTimeline->MeasureBeatToClocks(dwGroupBits, 0, measure, beat, &ltime);
		if(hr == S_OK)
		{
			if(ltime >= m_mtcBeg.Time() && ltime < m_mtcEnd.Time())
			{
				return true;
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
	bool Intersects(CSelectedRegion& sel)
	{
		long b = sel.m_mtcBeg > m_mtcBeg ? sel.m_mtcBeg : m_mtcBeg; // b = max(beginning times)
		long e = sel.m_mtcEnd < m_mtcEnd ? sel.m_mtcEnd : m_mtcEnd; // e = min(ending times)
		// intersection is empty if min(ending times) <= max(beginning times)
		return e <= b ? false : true;
	}
	bool IsNull()
	{
		return m_mtcEnd < m_mtcBeg;
	}

	long BeginPos(IDMUSProdTimeline* pTimeline)
	{
		long pos;
		pTimeline->ClocksToPosition( m_mtcBeg.Time(), &pos);
		return pos;
	}
	long EndPos(IDMUSProdTimeline* pTimeline)
	{
		long pos;
		pTimeline->ClocksToPosition( m_mtcEnd.Time(), &pos);
		return pos;
	}
	HRESULT Load(IStream* pIStream)
	{
		HRESULT hr, hrBeg, hrEnd, hrFlags;
		DWORD cbBeg, cbEnd, cbFlags;
		hrBeg = pIStream->Read(&m_mtcBeg, sizeof(CMusicTimeConverter), &cbBeg);
		hrEnd = pIStream->Read(&m_mtcEnd, sizeof(CMusicTimeConverter), &cbEnd);
		hrFlags = pIStream->Read(&m_dwFlags, sizeof(DWORD), &cbFlags);
		hr = (cbBeg == sizeof(CMusicTimeConverter)) && (cbEnd == sizeof(CMusicTimeConverter)
				&& (cbFlags == sizeof(DWORD))
				&& SUCCEEDED(hrBeg) && SUCCEEDED(hrEnd) && SUCCEEDED(hrFlags))
			? S_OK : E_FAIL;
		return hr;
	}
	HRESULT Save(IStream* pIStream)
	{
		HRESULT hr, hrBeg, hrEnd, hrFlags;
		DWORD cbBeg, cbEnd, cbFlags;
		hrBeg = pIStream->Write(&m_mtcBeg, sizeof(CMusicTimeConverter), &cbBeg);
		hrEnd = pIStream->Write(&m_mtcEnd, sizeof(CMusicTimeConverter), &cbEnd);
		hrFlags = pIStream->Write(&m_dwFlags, sizeof(DWORD), &cbFlags);
		hr = (cbBeg == sizeof(CMusicTimeConverter)) && (cbEnd == sizeof(CMusicTimeConverter))
				&& (cbFlags == sizeof(DWORD))
				&& SUCCEEDED(hrBeg) && SUCCEEDED(hrEnd) && SUCCEEDED(hrFlags)
			? S_OK : E_FAIL;
		return hr;
	}

};

// list handles memory for regions
class CListSelectedRegion : public CTypedPtrList<CPtrList, CSelectedRegion *>
{
	friend class CChordStrip;
	DWORD m_dwGroupbits;
	IDMUSProdTimeline*	m_pTimeline;
	long lShiftSelectAnchorPosition;
public:
	HRESULT SelectAll();
	CListSelectedRegion(IDMUSProdTimeline* pTimeline, DWORD dwGroupbits);
	~CListSelectedRegion();

	IDMUSProdTimeline*& Timeline()
	{
		return m_pTimeline;
	}

	DWORD& GroupBits()
	{
		return m_dwGroupbits;
	}

	CListSelectedRegion& operator=(const CListSelectedRegion& list);
	// methods to add and modify region list
	// add a region at position(converted to a time) whose duration = measure:beat
	void AddRegion(long position, long measure=0, long beat=1);
	void AddRegion(const CSelectedRegion& csr);
	// select, but delay deselecting so that multiple selected regions can be dragged.
	void AddAndDelayDeselect(long position, long measure=0, long beat=1);
	// completes above by deselecting and removing
	void CompleteDeselect();
	// toggle the beat that contains the specified position
	void ToggleRegion(long position);
	// toggle the specified measure and beat
	void ToggleRegion(long measure, long beat);
	// delay toggle: complete after drag by calling CompleteToggle
	// behavior: (if not selected before and drag aborted, deselect)
	void DelayToggle(long position, long measure=0, long beat=1);
	void CompleteToggle();
	// delay select
	// select all between current position and anchor region (first region in list).  If list is empty
	// then just do an add region
	void ShiftAddRegion(long position, long measure=0, long beat=1);
	void SetShiftSelectAnchor(long position);
	long GetShiftSelectAnchor();
	bool Clear();
	void Sort(void);

	// methods to access list (by draw and cut, copy, paste, etc)
	// can also use CList methods
	POSITION FindContainingRegion(long lPosition);
	POSITION FindContainingRegion(long lMeasure, long lBeat);
	bool Contains(long measure, long beat);
	bool Contains(long lTime);
	HRESULT Load(IStream* pIStream);
	HRESULT Save(IStream* pIStream);
	void Copy(CListSelectedRegion& list)
	{
		*this = list;
	}
	void GetSpan(long& beg, long& end);
};



#endif