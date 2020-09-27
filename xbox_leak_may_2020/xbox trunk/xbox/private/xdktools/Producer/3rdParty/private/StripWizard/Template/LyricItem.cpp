#include "stdafx.h"
#include "$$Safe_root$$Item.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

C$$Safe_root$$Item::C$$Safe_root$$Item()
{
	Clear();
}

C$$Safe_root$$Item::C$$Safe_root$$Item(const C$$Safe_root$$Item& item)
{
	// Copy the passed-in item
	Copy( &item );
}

void C$$Safe_root$$Item::Clear()
{
	m_lMeasure = 0;
	m_lBeat = 0;
	m_strText.Empty();
	m_dwBits = 0;
	m_fSelected = FALSE;
}

void C$$Safe_root$$Item::Copy( const C$$Safe_root$$Item* pItem )
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

	m_lMeasure = pItem->m_lMeasure;
	m_lBeat = pItem->m_lBeat;
	m_strText = pItem->m_strText;
	m_dwBits = pItem->m_dwBits;
	m_fSelected = pItem->m_fSelected;
}

BOOL C$$Safe_root$$Item::After(const C$$Safe_root$$Item& item)
{
	// Check if this item is after the one passed in.

	// Check if our measure number is greater.
	if( m_lMeasure > item.m_lMeasure )
	{
		// Our measure number is greater - we're after the item
		return TRUE;
	}
	// Check if our measure number is equal
	else if( m_lMeasure == item.m_lMeasure )
	{
		// Check if our beat number is greater
		if( m_lBeat > item.m_lBeat )
		{
			// Our beat number is greater - we're after the item
			return TRUE;
		}
	}

	// We're either before the item, or on the same beat.
	return FALSE;
}

BOOL C$$Safe_root$$Item::Before(const C$$Safe_root$$Item& item)
{
	// Check if this lyric is before the one passed in.

	// Check if our measure number is lesser.
	if( m_lMeasure < item.m_lMeasure )
	{
		// Our measure number is lesser - we're before the item
		return TRUE;
	}
	// Check if our measure number is equal.
	else if( m_lMeasure == item.m_lMeasure )
	{
		// Check if our measure number is lesser.
		if( m_lBeat < item.m_lBeat )
		{
			// Our measure number is lesser - we're before the item
			return TRUE;
		}
	}

	// We're either after the item, or on the same beat.
	return FALSE;
}
