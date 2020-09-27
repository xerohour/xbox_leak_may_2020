/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#ifndef SELECTED_GRID_REGION_H
#define SELECTED_GRID_REGION_H

#include <afxtempl.h>
#include "MusicTimeGridConverter.h"

class CListSelectedGridRegion;

class CSelectedGridRegion
{
	friend class CListSelectedGridRegion;
protected:
	CMusicTimeGridConverter m_mtgcBeg;
	CMusicTimeGridConverter m_mtgcEnd;
	DWORD	m_dwFlags;
public:
	// if m_dwFlags == WasSelected, then region was selected before click select.  If successful drag drop
	// then remains selected, otherwise its deselected.  
	// Used by the pair CSelectedGridRegionList::DelayToggle/CompleteToggle
	enum { WasToggleSelected=1, WasSelected=2 };

	CSelectedGridRegion()
	{
		m_dwFlags = 0;
	}

	CSelectedGridRegion(CMusicTimeGridConverter mtgcBeg, CMusicTimeGridConverter mtgcEnd)
		: m_mtgcBeg(mtgcBeg), m_mtgcEnd(mtgcEnd)
	{
		m_dwFlags = 0;
	}

	CSelectedGridRegion(const CSelectedGridRegion& sel)
	{
		m_mtgcBeg = sel.m_mtgcBeg;
		m_mtgcEnd = sel.m_mtgcEnd;
		m_dwFlags = sel.m_dwFlags;
	}

	// create
	CSelectedGridRegion(long lPosition, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits)
	{
		// Set the start position
		m_mtgcBeg.SetTimeUsingPosition(lPosition, pTimeline, dwGroupBits);

		// Set the end position to the next grid
		long lMeasure, lBeat, lGrid;
		m_mtgcBeg.GetMeasureBeatGrid( lMeasure, lBeat, lGrid, pTimeline, dwGroupBits );
		m_mtgcEnd.SetTime(lMeasure, lBeat, lGrid + 1, pTimeline, dwGroupBits);

		// Initialize m_dwFlags
		m_dwFlags = 0;
	}

	CSelectedGridRegion& operator=(const CSelectedGridRegion& sel)
	{
		m_mtgcBeg = sel.m_mtgcBeg;
		m_mtgcEnd = sel.m_mtgcEnd;
		m_dwFlags = sel.m_dwFlags;
		return *this;
	}

	CMusicTimeGridConverter& Beg()
	{
		return m_mtgcBeg;
	}

	CMusicTimeGridConverter& End()
	{
		return m_mtgcEnd;
	}

	DWORD& Flags()
	{
		return m_dwFlags;
	}

	bool Contains(CSelectedGridRegion& selRegion)
	{
		if(m_mtgcBeg <= selRegion.m_mtgcBeg && m_mtgcEnd >= selRegion.m_mtgcEnd)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Contains(long lMeasure, long lBeat, long lGrid, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits)
	{
		CMusicTimeGridConverter mtgcTemp;
		mtgcTemp.SetTime( lMeasure, lBeat, lGrid, pTimeline, dwGroupBits );

		if(mtgcTemp >= m_mtgcBeg && mtgcTemp < m_mtgcEnd)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Intersects(CSelectedGridRegion& sel)
	{
		long b = sel.m_mtgcBeg > m_mtgcBeg ? sel.m_mtgcBeg : m_mtgcBeg; // b = max(beginning times)
		long e = sel.m_mtgcEnd < m_mtgcEnd ? sel.m_mtgcEnd : m_mtgcEnd; // e = min(ending times)
		// intersection is empty if min(ending times) <= max(beginning times)
		return e <= b ? false : true;
	}

	bool IsNull()
	{
		return m_mtgcEnd <= m_mtgcBeg;
	}

	long BeginPos(IDMUSProdTimeline* pTimeline)
	{
		long pos;
		pTimeline->ClocksToPosition( m_mtgcBeg.Time(), &pos);
		return pos;
	}

	long EndPos(IDMUSProdTimeline* pTimeline)
	{
		long pos;
		pTimeline->ClocksToPosition( m_mtgcEnd.Time(), &pos);
		return pos;
	}

	HRESULT Load(IStream* pIStream)
	{
		HRESULT hr, hrBeg, hrEnd, hrFlags;
		DWORD cbBeg, cbEnd, cbFlags;
		hrBeg = pIStream->Read(&m_mtgcBeg, sizeof(CMusicTimeGridConverter), &cbBeg);
		hrEnd = pIStream->Read(&m_mtgcEnd, sizeof(CMusicTimeGridConverter), &cbEnd);
		hrFlags = pIStream->Read(&m_dwFlags, sizeof(DWORD), &cbFlags);
		hr = (cbBeg == sizeof(CMusicTimeGridConverter)) && (cbEnd == sizeof(CMusicTimeGridConverter)
				&& (cbFlags == sizeof(DWORD))
				&& SUCCEEDED(hrBeg) && SUCCEEDED(hrEnd) && SUCCEEDED(hrFlags))
			? S_OK : E_FAIL;
		return hr;
	}

	HRESULT Save(IStream* pIStream)
	{
		HRESULT hr, hrBeg, hrEnd, hrFlags;
		DWORD cbBeg, cbEnd, cbFlags;
		hrBeg = pIStream->Write(&m_mtgcBeg, sizeof(CMusicTimeGridConverter), &cbBeg);
		hrEnd = pIStream->Write(&m_mtgcEnd, sizeof(CMusicTimeGridConverter), &cbEnd);
		hrFlags = pIStream->Write(&m_dwFlags, sizeof(DWORD), &cbFlags);
		hr = (cbBeg == sizeof(CMusicTimeGridConverter)) && (cbEnd == sizeof(CMusicTimeGridConverter))
				&& (cbFlags == sizeof(DWORD))
				&& SUCCEEDED(hrBeg) && SUCCEEDED(hrEnd) && SUCCEEDED(hrFlags)
			? S_OK : E_FAIL;
		return hr;
	}
};

// list handles memory for regions
class CListSelectedGridRegion : public CTypedPtrList<CPtrList, CSelectedGridRegion *>
{
protected:
	DWORD m_dwGroupbits;
	IDMUSProdTimeline*	m_pTimeline;
	long lShiftSelectAnchorPosition;

public:
	HRESULT SelectAll();
	CListSelectedGridRegion(IDMUSProdTimeline* pTimeline, DWORD dwGroupbits);
	~CListSelectedGridRegion();

	IDMUSProdTimeline*& Timeline()
	{
		return m_pTimeline;
	}

	DWORD& GroupBits()
	{
		return m_dwGroupbits;
	}

	CListSelectedGridRegion& operator=(const CListSelectedGridRegion& list);
	// methods to add and modify region list
	// add a region at position(converted to a time) whose duration = measure:beat
	void AddRegion(long lPosition);
	void AddRegion(const CSelectedGridRegion& csr);
	// toggle the beat that contains the specified position
	void ToggleRegion(long position);
	// toggle the specified measure and beat
	void ToggleRegion(long lMeasure, long lBeat, long lGrid);
	// delay select
	// select all between current position and anchor region (first region in list).  If list is empty
	// then just do an add region
	void ShiftAddRegion(long position);
	void SetShiftSelectAnchor(long position);
	bool Clear();
	void Sort(void);

	// methods to access list (by draw and cut, copy, paste, etc)
	// can also use CList methods
	POSITION FindContainingRegion(long lPosition);
	POSITION FindContainingRegion(long lMeasure, long lBeat, long lGrid);
	bool Contains(long lMeasure, long lBeat, long lGrid);
	HRESULT Load(IStream* pIStream);
	HRESULT Save(IStream* pIStream);
	void Copy(CListSelectedGridRegion& list)
	{
		*this = list;
	}
	void GetSpan(long& beg, long& end);
};



#endif // SELECTED_GRID_REGION_H