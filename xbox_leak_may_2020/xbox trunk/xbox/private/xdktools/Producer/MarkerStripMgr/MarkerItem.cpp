#include "stdafx.h"
#include "MarkerItem.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMarkerItem::CMarkerItem()
{
	Clear();
}

CMarkerItem::CMarkerItem(const CMarkerItem& item)
{
	// Copy the passed-in item
	Copy( &item );
}

void CMarkerItem::Clear()
{
	m_mtTime = 0;
	m_typeMarker = MARKER_CUE;
	m_lMeasure = 0;
	m_lBeat = 0;
	m_lGrid = 0;
	m_lTick = 0;
	m_dwBitsUI = 0;
	m_wFlagsUI = 0;
	m_fSelected = false;
}

void CMarkerItem::Copy( const CMarkerItem* pItem )
{
	ASSERT( pItem != NULL );
	if ( pItem == NULL )
	{
		return;
	}

	if( pItem == this )
	{
		return;
	}

	m_mtTime = pItem->m_mtTime;
	m_typeMarker = pItem->m_typeMarker;
	m_lMeasure = pItem->m_lMeasure;
	m_lBeat = pItem->m_lBeat;
	m_lGrid = pItem->m_lGrid;
	m_lTick = pItem->m_lTick;
	m_dwBitsUI = pItem->m_dwBitsUI;
	m_wFlagsUI = pItem->m_wFlagsUI;
	m_fSelected = pItem->m_fSelected;
}

BOOL CMarkerItem::After(const CMarkerItem& item)
{
	// Check if this item is after the one passed in.

	// Check if our time is greater.
	if( m_mtTime > item.m_mtTime )
	{
		// Our time number is greater - we're after the item
		return TRUE;
	}

	// We're either before the item, or on the same tick.
	return FALSE;
}

BOOL CMarkerItem::Before(const CMarkerItem& item)
{
	// Check if this lyric is before the one passed in.

	// Check if our time is lesser.
	if( m_mtTime < item.m_mtTime )
	{
		// Our time is lesser - we're before the item
		return TRUE;
	}

	// We're either after the item, or on the same tick.
	return FALSE;
}
