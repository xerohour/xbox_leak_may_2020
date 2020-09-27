/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#include "stdafx.h"
#include <dmusici.h>
#include <dmusicf.h>

#include "SelectedRegion.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CListSelectedRegion::CListSelectedRegion(IDMUSProdTimeline* pTimeline, DWORD dwGroupbits)
: m_dwGroupbits(dwGroupbits), m_pTimeline(pTimeline)
{
//	m_pTimeline->AddRef();
	lShiftSelectAnchorPosition = 0;
}

CListSelectedRegion::~CListSelectedRegion()
{
	Clear();
//	m_pTimeline->Release();
}

// methods to add and modify region list

// add a region at position(converted to a time) whose duration = measure:beat
void CListSelectedRegion::AddRegion(long position, long measure, long beat)
{
	// now create and add this one
	CSelectedRegion* psr = new CSelectedRegion(position, measure, beat, m_pTimeline, m_dwGroupbits);
	AddHead(psr);
}

void CListSelectedRegion::AddRegion(const CSelectedRegion& csr)
{
	CSelectedRegion* psr = new CSelectedRegion(csr);
	AddHead(psr);
}


// toggle a region that contains the region formed by the given position
void CListSelectedRegion::ToggleRegion(long position)
{
	POSITION pos = FindContainingRegion(position);
	if(pos == NULL)
	{
		AddRegion(position, 0, 1);
	}
	else
	{
		CMusicTimeConverter mtcPos;
		mtcPos.SetTimeUsingPosition( position, m_pTimeline, m_dwGroupbits );
		CSelectedRegion* psr = GetAt(pos);
		if( (psr->m_mtcEnd == mtcPos) && (psr->m_mtcBeg == mtcPos) )
		{
			// Only this beat is in the selected regions, delete it
			RemoveAt(pos);
			delete psr;
		}
		else if( psr->m_mtcEnd == mtcPos )
		{
			// The region to toggle is at the end of psr - shorten psr
			psr->m_mtcEnd = psr->m_mtcEnd - 1;
			long lMeasure, lBeat;
			psr->m_mtcEnd.GetMeasureBeat( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
			psr->m_mtcEnd.SetTime( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );

			// Check if this region is empty.  If so, delete it
			if( psr->m_mtcBeg == psr->m_mtcEnd )
			{
				RemoveAt(pos);
				delete psr;
			}
		}
		else if( psr->m_mtcBeg == mtcPos )
		{
			// The region to toggle is at the start of psr - shorten psr
			long lMeasure, lBeat;
			psr->m_mtcBeg.GetMeasureBeat( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
			lBeat++;
			psr->m_mtcBeg.SetTime( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );

			// Check if this region is empty.  If so, delete it
			if( psr->m_mtcBeg == psr->m_mtcEnd )
			{
				RemoveAt(pos);
				delete psr;
			}
		}
		else
		{
			// Create a new CSelectedRegion for the first half
			CSelectedRegion *psrNew = new CSelectedRegion( psr->m_mtcBeg, mtcPos );
			InsertBefore( pos, psrNew );

			// Modify the existing CSelectedRegion to display the second half
			long lMeasure, lBeat;
			mtcPos.GetMeasureBeat( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
			lBeat++;
			psr->m_mtcBeg.SetTime( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
		}
	}
}


// toggle a region that contains the region formed by the given measure and beat
void CListSelectedRegion::ToggleRegion(long lMeasure, long lBeat)
{
	POSITION pos = FindContainingRegion(lMeasure, lBeat);
	if(pos == NULL)
	{
		CSelectedRegion *pSelectedRegion = new CSelectedRegion;
		if( pSelectedRegion )
		{
			pSelectedRegion->m_mtcBeg.SetTime( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
			pSelectedRegion->m_mtcEnd.SetTime( lMeasure, lBeat + 1, m_pTimeline, m_dwGroupbits );
			AddRegion( *pSelectedRegion );
		}
	}
	else
	{
		CMusicTimeConverter mtcPos;
		mtcPos.SetTime( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
		CSelectedRegion* psr = GetAt(pos);
		if( (psr->m_mtcEnd == mtcPos) && (psr->m_mtcBeg == mtcPos) )
		{
			// Only this beat is in the selected regions, delete it
			RemoveAt(pos);
			delete psr;
		}
		else if( psr->m_mtcEnd == mtcPos )
		{
			// The region to toggle is at the end of psr - shorten psr
			psr->m_mtcEnd = psr->m_mtcEnd - 1;
			long lMeasure, lBeat;
			psr->m_mtcEnd.GetMeasureBeat( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
			psr->m_mtcEnd.SetTime( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );

			// Check if this region is empty.  If so, delete it
			if( psr->m_mtcBeg == psr->m_mtcEnd )
			{
				RemoveAt(pos);
				delete psr;
			}
		}
		else if( psr->m_mtcBeg == mtcPos )
		{
			// The region to toggle is at the start of psr - shorten psr
			long lMeasure, lBeat;
			psr->m_mtcBeg.GetMeasureBeat( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
			lBeat++;
			psr->m_mtcBeg.SetTime( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );

			// Check if this region is empty.  If so, delete it
			if( psr->m_mtcBeg == psr->m_mtcEnd )
			{
				RemoveAt(pos);
				delete psr;
			}
		}
		else
		{
			// Create a new CSelectedRegion for the first half
			CSelectedRegion *psrNew = new CSelectedRegion( psr->m_mtcBeg, mtcPos );
			InsertBefore( pos, psrNew );

			// Modify the existing CSelectedRegion to display the second half
			long lMeasure, lBeat;
			mtcPos.GetMeasureBeat( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
			lBeat++;
			psr->m_mtcBeg.SetTime( lMeasure, lBeat, m_pTimeline, m_dwGroupbits );
		}
	}
}

// delay toggle: complete after drag by calling CompleteToggle
// behavior: (if not selected before and drag aborted, deselect)
void CListSelectedRegion::DelayToggle(long position, long measure, long beat)
{
	POSITION pos = FindContainingRegion(position);
	if(pos == NULL)
	{
		// unconditional add
		AddRegion(position, measure, beat);
	}
	else
	{
		CSelectedRegion* psr = GetAt(pos);
		psr->Flags() |= CSelectedRegion::WasToggleSelected;
	}
}
void CListSelectedRegion::CompleteToggle()
{
	POSITION pos = GetHeadPosition();
	while(pos)
	{
		POSITION tmpPos = pos;
		CSelectedRegion* psr = GetNext(pos);
		if(psr->Flags() & CSelectedRegion::WasToggleSelected)
		{
			RemoveAt(tmpPos);
			delete psr;
		}
	}
}

// select, but delay deselecting so that multiple selected regions can be dragged.
// call CompleteDeselect after drop to complete
void CListSelectedRegion::AddAndDelayDeselect(long position, long measure, long beat)
{
	POSITION pos = FindContainingRegion(position);
	if(pos == NULL)
	{
		// mark other selected regions so they can be removed later if drop was aborted
		pos = GetHeadPosition();
		while(pos)
		{
			CSelectedRegion* psr = GetNext(pos);
			psr->Flags() |= CSelectedRegion::WasSelected;
		}
		// now add region
		AddRegion(position, measure, beat);
	}
	else
	{
		CSelectedRegion* pCur = GetNext(pos);
		// mark all regions except for this one
		pos = GetHeadPosition();
		while(pos)
		{
			CSelectedRegion* psr = GetNext(pos);
			if(psr != pCur)
			{
				psr->Flags() |= CSelectedRegion::WasSelected;
			}
		}
	}
}

void CListSelectedRegion::CompleteDeselect()
{
	POSITION pos = GetHeadPosition();
	while(pos)
	{
		POSITION tmpPos = pos;
		CSelectedRegion* psr = GetNext(pos);
		if(psr->Flags() & CSelectedRegion::WasSelected)
		{
			RemoveAt(tmpPos);
			delete psr;
		}
	}
}

// select all between current position and anchor region (first region in list).  If list is empty
// then just do an add region from first measure first beat to current position
void CListSelectedRegion::ShiftAddRegion(long position, long measure, long beat)
{
	if(IsEmpty())
	{
		lShiftSelectAnchorPosition = 0;
	}

	CSelectedRegion* psrAnchor = NULL;

	// Get the shift-select anchor
	POSITION pos = FindContainingRegion(lShiftSelectAnchorPosition);
	if(pos)
	{
		psrAnchor = GetAt(pos);
		RemoveAt(pos);
	}
	else
	{
		psrAnchor = new CSelectedRegion(0, 0, 0, m_pTimeline, m_dwGroupbits);
	}

	// Delete all items in the list
	while(!IsEmpty())
	{
		CSelectedRegion* psr = RemoveHead();
		delete psr;
	}

	// Rebuild the selected region list
	CSelectedRegion* psrX = new CSelectedRegion(position, measure, beat, m_pTimeline, m_dwGroupbits);
	if(psrX->m_mtcBeg >= psrAnchor->m_mtcEnd)
	{
		psrX->m_mtcBeg = psrAnchor->m_mtcEnd;
	}
	if(psrX->m_mtcEnd <= psrAnchor->m_mtcBeg)
	{
		psrX->m_mtcEnd = psrAnchor->m_mtcBeg;
	}
	AddHead(psrAnchor);
	AddTail(psrX);
}

void CListSelectedRegion::SetShiftSelectAnchor(long position)
{
	// Set shift-select anchor
	lShiftSelectAnchorPosition = position;
}

long CListSelectedRegion::GetShiftSelectAnchor(void)
{
	// Get shift-select anchor
	return lShiftSelectAnchorPosition;
}

POSITION CListSelectedRegion::FindContainingRegion(long lPosition)
{
	CSelectedRegion srTarget(lPosition, 0, 1, m_pTimeline, m_dwGroupbits);

	// Get a pointer to the first item in the list
	POSITION pos = GetHeadPosition();
	while(pos)
	{
		// Save a pointer to this item
		POSITION posMatch = pos;

		// Get a pointer to the data in this item
		CSelectedRegion* psrTest = GetNext(pos);

		// Check if this selected range is the one we want
		if(psrTest->Contains(srTarget))
		{
			// Found it - return it.
			return posMatch;
		}
	}

	// Didn't find it - return NULL
	return NULL;
}

POSITION CListSelectedRegion::FindContainingRegion(long lMeasure, long lBeat)
{
	// Get a pointer to the first item in the list
	POSITION pos = GetHeadPosition();
	while(pos)
	{
		// Save a pointer to this item
		POSITION posMatch = pos;

		// Get a pointer to the data in this item
		CSelectedRegion* psrTest = GetNext(pos);

		// Check if this selected range is the one we want
		if(psrTest->Contains(lMeasure, lBeat, m_pTimeline, m_dwGroupbits))
		{
			// Found it - return it.
			return posMatch;
		}
	}

	return NULL;
}

bool CListSelectedRegion::Clear()
{
	// Check if there's anything to remove
	if( IsEmpty() )
	{
		// Nothing to remove - return false
		return false;
	}

	// Delete all items in the list
	while(!IsEmpty())
	{
		CSelectedRegion* psr = RemoveHead();
		delete psr;
	}

	// Found something to remove - return true
	return true;
}

HRESULT CListSelectedRegion::Load(IStream* pIStream)
{
	// Initialize shift-select anchor
	lShiftSelectAnchorPosition = 0;

	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_INVALIDARG;
	HRESULT hr = S_OK;

	// check if anything to read
	DWORD nRegion, cb;
	hr = pIStream->Read(&nRegion, sizeof(DWORD), &cb);
	if(FAILED(hr))
	{
		return hr;
	}
	else if(cb == 0)
	{
		return S_FALSE;
	}
	else
	{
		Clear();
		for(unsigned int i = 0; i < nRegion && hr == S_OK; i++)
		{
			CSelectedRegion* psr = new CSelectedRegion;
			hr = psr->Load(pIStream);
		}
	}

	return hr;
}

HRESULT CListSelectedRegion::Save(IStream* pIStream)
{
	ASSERT(pIStream);
	if(pIStream == NULL)
		return E_INVALIDARG;
	HRESULT hr = S_OK;

	DWORD nRegion = static_cast<DWORD>(GetCount());
	DWORD cb;
	hr = pIStream->Write(&nRegion, sizeof(DWORD), &cb);

	hr = cb == sizeof(DWORD) && SUCCEEDED(hr) ? S_OK : E_FAIL;

	POSITION pos = GetHeadPosition();
	while(pos)
	{
		CSelectedRegion* psr = GetNext(pos);
		hr = psr->Save(pIStream);
	}
	return hr;
}

CListSelectedRegion& CListSelectedRegion::operator=(const CListSelectedRegion& list)
{
	if(&list != this)
	{
		Clear();
		POSITION pos = list.GetHeadPosition();
		while(pos)
		{
			CSelectedRegion* psr = list.GetNext(pos);
			AddTail(new CSelectedRegion(*psr));
		}
	}
	return *this;
}

void CListSelectedRegion::GetSpan(long& beg, long& end)
{
	beg = LONG_MAX;
	end = LONG_MIN;

	POSITION pos = GetHeadPosition();
	while(pos)
	{
		CSelectedRegion* psr = GetNext(pos);
		if(psr->m_mtcBeg.Time() < beg)
		{
			beg = psr->m_mtcBeg.Time();
		}
		if(psr->m_mtcEnd.Time() > end)
		{
			end = psr->m_mtcEnd.Time();
		}
	}
}

HRESULT CListSelectedRegion::SelectAll()
{
	VARIANT var;
	long ltime;
	HRESULT hr = m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	if(hr == S_OK)
	{
		ltime = V_I4(&var);
		CMusicTimeConverter cmtBeg(0);
		CMusicTimeConverter cmtEnd(ltime);
		CSelectedRegion* psr = new CSelectedRegion(cmtBeg, cmtEnd);
		Clear();
		AddHead(psr);
	}
	return hr;
}

bool CListSelectedRegion::Contains(long measure, long beat)
{
	return FindContainingRegion((short)measure, (BYTE)beat) != NULL;
}

bool CListSelectedRegion::Contains(long lTime)
{
	// Get a pointer to the first item in the list
	POSITION pos = GetHeadPosition();
	while(pos)
	{
		// Get a pointer to the data in this item
		CSelectedRegion* psrTest = GetNext(pos);

		if(lTime >= psrTest->m_mtcBeg.Time() && lTime < psrTest->m_mtcEnd.Time())
		{
			return true;
		}
	}

	return false;
}

void CListSelectedRegion::Sort(void)
{
	// Do a simple bubble sort on the list of selected regions
	// Order them by their start time

	// Initialize fChange to true so we go through the sort at least once
	bool fChange = true;

	// Continue while something changed the last time through
	while( fChange )
	{
		// Now initialize fChange to false, since nothing's changed
		fChange = false;

		// Get a pointer to the head of the list
		POSITION posRegion = GetHeadPosition();

		// Continue until there are no more items to look at
		while( posRegion )
		{
			// Save the current position
			POSITION posPrev = posRegion;

			// Get the next position
			GetNext( posRegion );

			// If there is a next position, and the current item should be
			// after the next item
			if( posRegion && (GetAt(posPrev)->Beg() > GetAt(posRegion)->Beg()) )
			{
				// Get a pointer to the current item
				CSelectedRegion* psr = GetAt(posPrev);

				// Remove it from the list
				RemoveAt( posPrev );

				// Insert it after the next item, and update our list pointer to point at it
				posRegion = InsertAfter( posRegion, psr );

				// Note that we changed so we go through the list again
				fChange = true;
			}
		}
	}
}
