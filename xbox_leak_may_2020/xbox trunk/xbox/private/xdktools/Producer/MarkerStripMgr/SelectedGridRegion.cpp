/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#include "stdafx.h"
#include <dmusici.h>
#include <dmusicf.h>

#include "SelectedGridRegion.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CListSelectedGridRegion::CListSelectedGridRegion(IDMUSProdTimeline* pTimeline, DWORD dwGroupbits)
: m_dwGroupbits(dwGroupbits), m_pTimeline(pTimeline)
{
//	m_pTimeline->AddRef();
	lShiftSelectAnchorPosition = 0;
}

CListSelectedGridRegion::~CListSelectedGridRegion()
{
	Clear();
//	m_pTimeline->Release();
}

// methods to add and modify region list

// add a region at position(converted to a time)
void CListSelectedGridRegion::AddRegion(long lPosition)
{
	// now create and add this one
	CSelectedGridRegion* psr = new CSelectedGridRegion(lPosition, m_pTimeline, m_dwGroupbits);
	AddHead(psr);
}

void CListSelectedGridRegion::AddRegion(const CSelectedGridRegion& csr)
{
	CSelectedGridRegion* psr = new CSelectedGridRegion(csr);
	AddHead(psr);
}


// toggle a region that contains the region formed by the given position
void CListSelectedGridRegion::ToggleRegion(long position)
{
	POSITION pos = FindContainingRegion(position);
	if(pos == NULL)
	{
		AddRegion(position);
	}
	else
	{
		CMusicTimeGridConverter mtgcPos;
		mtgcPos.SetTimeUsingPosition( position, m_pTimeline, m_dwGroupbits );
		CSelectedGridRegion* psr = GetAt(pos);
		if( (psr->m_mtgcEnd == mtgcPos) && (psr->m_mtgcBeg == mtgcPos) )
		{
			// Only this beat is in the selected regions, delete it
			RemoveAt(pos);
			delete psr;
		}
		else if( psr->m_mtgcEnd == mtgcPos )
		{
			// The region to toggle is at the end of psr - shorten psr
			psr->m_mtgcEnd = psr->m_mtgcEnd - 1;
			long lMeasure, lBeat, lGrid;
			psr->m_mtgcEnd.GetMeasureBeatGrid( lMeasure, lBeat, lGrid, m_pTimeline, m_dwGroupbits );
			psr->m_mtgcEnd.SetTime( lMeasure, lBeat, lGrid, m_pTimeline, m_dwGroupbits );

			// Check if this region is empty.  If so, delete it
			if( psr->m_mtgcBeg == psr->m_mtgcEnd )
			{
				RemoveAt(pos);
				delete psr;
			}
		}
		else if( psr->m_mtgcBeg == mtgcPos )
		{
			// The region to toggle is at the start of psr - shorten psr
			long lMeasure, lBeat, lGrid;
			psr->m_mtgcBeg.GetMeasureBeatGrid( lMeasure, lBeat, lGrid, m_pTimeline, m_dwGroupbits );
			lGrid++;
			psr->m_mtgcBeg.SetTime( lMeasure, lBeat, lGrid, m_pTimeline, m_dwGroupbits );

			// Check if this region is empty.  If so, delete it
			if( psr->m_mtgcBeg == psr->m_mtgcEnd )
			{
				RemoveAt(pos);
				delete psr;
			}
		}
		else
		{
			// Create a new CSelectedGridRegion for the first half
			CSelectedGridRegion *psrNew = new CSelectedGridRegion( psr->m_mtgcBeg, mtgcPos );
			InsertBefore( pos, psrNew );

			// Modify the existing CSelectedGridRegion to display the second half
			long lMeasure, lBeat, lGrid;
			mtgcPos.GetMeasureBeatGrid( lMeasure, lBeat, lGrid, m_pTimeline, m_dwGroupbits );
			lGrid++;
			psr->m_mtgcBeg.SetTime( lMeasure, lBeat, lGrid, m_pTimeline, m_dwGroupbits );
		}
	}
}


// toggle a region that contains the region formed by the given measure and beat and grid
void CListSelectedGridRegion::ToggleRegion(long lMeasure, long lBeat, long lGrid)
{
	POSITION pos = FindContainingRegion(lMeasure, lBeat, lGrid);
	if(pos == NULL)
	{
		CSelectedGridRegion *pSelectedRegion = new CSelectedGridRegion;
		if( pSelectedRegion )
		{
			pSelectedRegion->m_mtgcBeg.SetTime( lMeasure, lBeat, lGrid, m_pTimeline, m_dwGroupbits );
			pSelectedRegion->m_mtgcEnd.SetTime( lMeasure, lBeat, lGrid + 1, m_pTimeline, m_dwGroupbits );
			AddRegion( *pSelectedRegion );
		}
	}
	else
	{
		CMusicTimeGridConverter mtgcPos;
		mtgcPos.SetTime( lMeasure, lBeat, lGrid, m_pTimeline, m_dwGroupbits );
		CSelectedGridRegion* psr = GetAt(pos);
		if( (psr->m_mtgcEnd == mtgcPos) && (psr->m_mtgcBeg == mtgcPos) )
		{
			// Only this beat is in the selected regions, delete it
			RemoveAt(pos);
			delete psr;
		}
		else if( psr->m_mtgcEnd == mtgcPos )
		{
			// The region to toggle is at the end of psr - shorten psr
			psr->m_mtgcEnd = psr->m_mtgcEnd - 1;
			long lTmpMeasure, lTmpBeat, lTmpGrid;
			psr->m_mtgcEnd.GetMeasureBeatGrid( lTmpMeasure, lTmpBeat, lTmpGrid, m_pTimeline, m_dwGroupbits );
			psr->m_mtgcEnd.SetTime( lTmpMeasure, lTmpBeat, lTmpGrid, m_pTimeline, m_dwGroupbits );

			// Check if this region is empty.  If so, delete it
			if( psr->m_mtgcBeg == psr->m_mtgcEnd )
			{
				RemoveAt(pos);
				delete psr;
			}
		}
		else if( psr->m_mtgcBeg == mtgcPos )
		{
			// The region to toggle is at the start of psr - shorten psr
			long lTmpMeasure, lTmpBeat, lTmpGrid;
			psr->m_mtgcBeg.GetMeasureBeatGrid( lTmpMeasure, lTmpBeat, lTmpGrid, m_pTimeline, m_dwGroupbits );
			lTmpGrid++;
			psr->m_mtgcBeg.SetTime( lTmpMeasure, lTmpBeat, lTmpGrid, m_pTimeline, m_dwGroupbits );

			// Check if this region is empty.  If so, delete it
			if( psr->m_mtgcBeg == psr->m_mtgcEnd )
			{
				RemoveAt(pos);
				delete psr;
			}
		}
		else
		{
			// Create a new CSelectedGridRegion for the first half
			CSelectedGridRegion *psrNew = new CSelectedGridRegion( psr->m_mtgcBeg, mtgcPos );
			InsertBefore( pos, psrNew );

			// Modify the existing CSelectedGridRegion to display the second half
			long lTmpMeasure, lTmpBeat, lTmpGrid;
			mtgcPos.GetMeasureBeatGrid( lTmpMeasure, lTmpBeat, lTmpGrid, m_pTimeline, m_dwGroupbits );
			lTmpGrid++;
			psr->m_mtgcBeg.SetTime( lTmpMeasure, lTmpBeat, lTmpGrid, m_pTimeline, m_dwGroupbits );
		}
	}
}

// select all between current position and anchor region (first region in list).  If list is empty
// then just do an add region from first measure first beat to current position
void CListSelectedGridRegion::ShiftAddRegion(long position)
{
	/* This breaks shift-select between the cue and switch areas
	if(IsEmpty())
	{
		lShiftSelectAnchorPosition = 0;
	}
	*/

	CSelectedGridRegion* psrAnchor = NULL;

	// Get the shift-select anchor
	POSITION pos = FindContainingRegion(lShiftSelectAnchorPosition);
	if(pos)
	{
		psrAnchor = GetAt(pos);
		RemoveAt(pos);
	}
	else
	{
		// This breaks shift-select between the cue and switch areas
		//psrAnchor = new CSelectedGridRegion(0, m_pTimeline, m_dwGroupbits);
		psrAnchor = new CSelectedGridRegion(lShiftSelectAnchorPosition, m_pTimeline, m_dwGroupbits);
	}

	// Delete all items in the list
	while(!IsEmpty())
	{
		CSelectedGridRegion* psr = RemoveHead();
		delete psr;
	}

	// Rebuild the selected region list
	CSelectedGridRegion* psrX = new CSelectedGridRegion(position, m_pTimeline, m_dwGroupbits);
	if(psrX->m_mtgcBeg >= psrAnchor->m_mtgcEnd)
	{
		psrX->m_mtgcBeg = psrAnchor->m_mtgcEnd;
	}
	if(psrX->m_mtgcEnd <= psrAnchor->m_mtgcBeg)
	{
		psrX->m_mtgcEnd = psrAnchor->m_mtgcBeg;
	}
	AddHead(psrAnchor);
	AddTail(psrX);
}

void CListSelectedGridRegion::SetShiftSelectAnchor(long position)
{
	// Set shift-select anchor
	lShiftSelectAnchorPosition = position;
}

POSITION CListSelectedGridRegion::FindContainingRegion(long lPosition)
{
	CSelectedGridRegion srTarget(lPosition, m_pTimeline, m_dwGroupbits);

	// Get a pointer to the first item in the list
	POSITION pos = GetHeadPosition();
	while(pos)
	{
		// Save a pointer to this item
		POSITION posMatch = pos;

		// Get a pointer to the data in this item
		CSelectedGridRegion* psrTest = GetNext(pos);

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

POSITION CListSelectedGridRegion::FindContainingRegion(long lMeasure, long lBeat, long lGrid)
{
	// Get a pointer to the first item in the list
	POSITION pos = GetHeadPosition();
	while(pos)
	{
		// Save a pointer to this item
		POSITION posMatch = pos;

		// Get a pointer to the data in this item
		CSelectedGridRegion* psrTest = GetNext(pos);

		// Check if this selected range is the one we want
		if(psrTest->Contains(lMeasure, lBeat, lGrid, m_pTimeline, m_dwGroupbits))
		{
			// Found it - return it.
			return posMatch;
		}
	}

	return NULL;
}

bool CListSelectedGridRegion::Clear()
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
		CSelectedGridRegion* psr = RemoveHead();
		delete psr;
	}

	// Found something to remove - return true
	return true;
}

HRESULT CListSelectedGridRegion::Load(IStream* pIStream)
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
			CSelectedGridRegion* psr = new CSelectedGridRegion;
			hr = psr->Load(pIStream);
		}
	}

	return hr;
}

HRESULT CListSelectedGridRegion::Save(IStream* pIStream)
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
		CSelectedGridRegion* psr = GetNext(pos);
		hr = psr->Save(pIStream);
	}
	return hr;
}

CListSelectedGridRegion& CListSelectedGridRegion::operator=(const CListSelectedGridRegion& list)
{
	if(&list != this)
	{
		Clear();
		POSITION pos = list.GetHeadPosition();
		while(pos)
		{
			CSelectedGridRegion* psr = list.GetNext(pos);
			AddTail(new CSelectedGridRegion(*psr));
		}
	}
	return *this;
}

void CListSelectedGridRegion::GetSpan(long& beg, long& end)
{
	beg = LONG_MAX;
	end = LONG_MIN;

	POSITION pos = GetHeadPosition();
	while(pos)
	{
		CSelectedGridRegion* psr = GetNext(pos);
		if(psr->m_mtgcBeg.Time() < beg)
		{
			beg = psr->m_mtgcBeg.Time();
		}
		if(psr->m_mtgcEnd.Time() > end)
		{
			end = psr->m_mtgcEnd.Time();
		}
	}
}

HRESULT CListSelectedGridRegion::SelectAll()
{
	VARIANT var;
	long ltime;
	HRESULT hr = m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	if(hr == S_OK)
	{
		ltime = V_I4(&var);
		CMusicTimeGridConverter cmtBeg(0);
		CMusicTimeGridConverter cmtEnd(ltime);
		CSelectedGridRegion* psr = new CSelectedGridRegion(cmtBeg, cmtEnd);
		Clear();
		AddHead(psr);
	}
	return hr;
}

bool CListSelectedGridRegion::Contains(long lMeasure, long lBeat, long lGrid)
{
	return FindContainingRegion(lMeasure, lBeat, lGrid) != NULL;
}

void CListSelectedGridRegion::Sort(void)
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
				CSelectedGridRegion* psr = GetAt(posPrev);

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
