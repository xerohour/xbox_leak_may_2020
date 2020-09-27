/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#include "stdafx.h"
#include <RiffStrm.h>
#include "LyricMgr.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLyricItem::CLyricItem( void )
{
	m_pLyricMgr = NULL;

	Clear();
}

CLyricItem::CLyricItem( CLyricMgr* pLyricMgr )
{
	ASSERT( pLyricMgr != NULL );
	m_pLyricMgr = pLyricMgr;

	Clear();
}

CLyricItem::CLyricItem( CLyricMgr* pLyricMgr, const CLyricItem& lyric )
{
	ASSERT( pLyricMgr != NULL );
	m_pLyricMgr = pLyricMgr;

	// Copy the passed-in lyric
	Copy( &lyric );
}

void CLyricItem::Clear()
{
	m_dwTimingFlagsDM = DMUS_PMSGF_TOOL_ATTIME;
	m_mtTimeLogical = 0;
	m_mtTimePhysical = 0;
	m_strText.Empty();

	m_lMeasure = 0;
	m_lBeat = 0;
	m_lTick = 0;
	m_lLogicalMeasure = 0;
	m_lLogicalBeat = 0;

	m_dwBitsUI = 0;
	m_wFlagsUI = 0;
	m_fSelected = FALSE;
}

void CLyricItem::Copy( const CLyricItem* pLyricItem )
{
	ASSERT( pLyricItem != NULL );
	if ( pLyricItem == NULL )
	{
		return;
	}

	if( pLyricItem == this )
	{
		return;
	}

	m_dwTimingFlagsDM = pLyricItem->m_dwTimingFlagsDM;
	m_mtTimeLogical = pLyricItem->m_mtTimeLogical;
	m_mtTimePhysical = pLyricItem->m_mtTimePhysical;
	m_strText = pLyricItem->m_strText;

	m_lMeasure = pLyricItem->m_lMeasure;
	m_lBeat = pLyricItem->m_lBeat;
	m_lTick = pLyricItem->m_lTick;
	m_lLogicalMeasure = pLyricItem->m_lLogicalMeasure;
	m_lLogicalBeat = pLyricItem->m_lLogicalBeat;

	m_dwBitsUI = pLyricItem->m_dwBitsUI;
	m_wFlagsUI = pLyricItem->m_wFlagsUI;
	m_fSelected = pLyricItem->m_fSelected;
}

BOOL CLyricItem::After(const CLyricItem& Lyric)
{
	// Check if this lyric is after the one passed as Lyric.

	// Check if our measure number is greater.
	if( m_lMeasure > Lyric.m_lMeasure )
	{
		// Our measure number is greater - we're after Lyric
		return TRUE;
	}
	// Check if our measure number is equal
	else if( m_lMeasure == Lyric.m_lMeasure )
	{
		// Check if our beat number is greater
		if( m_lBeat > Lyric.m_lBeat )
		{
			// Our beat number is greater - we're after Lyric
			return TRUE;
		}
		// Check if our beat number is equal
		else if( m_lBeat == Lyric.m_lBeat )
		{
			// Check if our tick number is greater
			if( m_lTick > Lyric.m_lTick )
			{
				// Our tick number is greater - we're after the item
				return TRUE;
			}
		}
	}

	// We're either before Lyric, or on the same measure, beat, tick.
	return FALSE;
}

BOOL CLyricItem::Before(const CLyricItem& Lyric)
{
	// Check if this lyric is before the one passed as Lyric.

	// Check if our measure number is lesser.
	if( m_lMeasure < Lyric.m_lMeasure )
	{
		// Our measure number is lesser - we're before Lyric
		return TRUE;
	}
	// Check if our measure number is equal.
	else if( m_lMeasure == Lyric.m_lMeasure )
	{
		// Check if our measure number is lesser.
		if( m_lBeat < Lyric.m_lBeat )
		{
			// Our measure number is lesser - we're before Lyric
			return TRUE;
		}
		// Check if our beat number is equal.
		else if( m_lBeat == Lyric.m_lBeat )
		{
			// Check if our tick number is lesser.
			if( m_lTick < Lyric.m_lTick )
			{
				// Our tick number is lesser - we're before the item
				return TRUE;
			}
		}
	}

	// We're either after Lyric, or on the same measure, beat, tick.
	return FALSE;
}
	
HRESULT CLyricItem::SetTimePhysical( MUSIC_TIME mtTimePhysical, short nAction )
{
	ASSERT( m_pLyricMgr != NULL );
	if( m_pLyricMgr == NULL )
	{
		// Will be NULL when CLyricItem used for properties!
		return E_FAIL;
	}

	// Set item's physical time
	MUSIC_TIME mtOrigTimePhysical = m_mtTimePhysical;
	m_mtTimePhysical = mtTimePhysical;

	if( m_pLyricMgr == NULL 
	||  m_pLyricMgr->m_pTimeline == NULL )
	{
		if( nAction == STP_LOGICAL_NO_ACTION )
		{
			return S_OK;
		}
		else
		{
			ASSERT( 0 );
			return E_UNEXPECTED;
		}
	}

	// Set item's measure, beat, tick
	long lMeasure, lBeat, lTick;
	m_pLyricMgr->ClocksToMeasureBeatTick( m_mtTimePhysical, &lMeasure, &lBeat, &lTick );
	m_lMeasure = lMeasure;
	m_lBeat = lBeat;
	m_lTick = lTick;

	// Set item's logical time
	switch( nAction )
	{
		case STP_LOGICAL_NO_ACTION:
			// Simply recalc item's logical measure/beat
			m_pLyricMgr->ClocksToMeasureBeatTick( m_mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			break;

		case STP_LOGICAL_SET_DEFAULT:
			m_pLyricMgr->ClocksToMeasureBeatTick( m_mtTimePhysical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			m_pLyricMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &m_mtTimeLogical );
			break;

		case STP_LOGICAL_ADJUST:
		{
			// Get track GroupBits
			DWORD dwGroupBits = m_pLyricMgr->GetGroupBits();

			// Snap original physical time to number of beats
			long lPhysicalBeats;
			m_pLyricMgr->ClocksToMeasureBeatTick( mtOrigTimePhysical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pLyricMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Convert logical time to number of beats
			long lLogicalBeats;
			m_pLyricMgr->ClocksToMeasureBeatTick( m_mtTimeLogical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pLyricMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lLogicalBeats );

			// Compute difference between original physical time and original logical time
			long lBeatDiff = lLogicalBeats - lPhysicalBeats;

			// Snap new physical time to number of beats
			m_pLyricMgr->ClocksToMeasureBeatTick( m_mtTimePhysical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pLyricMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Set item's new logical time
			long lNewLogicalBeats = lPhysicalBeats + lBeatDiff;
			if( lNewLogicalBeats < 0 )
			{
				lNewLogicalBeats = 0;
			}
			BeatsToMeasureBeat( m_pLyricMgr->m_pTimeline, dwGroupBits, 0, lNewLogicalBeats, lMeasure, lBeat );
			m_pLyricMgr->ForceBoundaries( lMeasure, lBeat, 0, &m_mtTimeLogical );
			m_pLyricMgr->ClocksToMeasureBeatTick( m_mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			m_pLyricMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &m_mtTimeLogical );
			break;
		}

		default:
			ASSERT( 0 );	// Should not happen!
			break;
	}

	// Make sure item's logical time is not less than zero
	if( m_mtTimeLogical < 0 )
	{
		ASSERT( 0 );
		m_mtTimeLogical = 0;
	}
	ASSERT( m_lLogicalMeasure >= 0 );
	ASSERT( m_lLogicalBeat >= 0 );

	return S_OK;
}
	
HRESULT CLyricItem::SetTimeLogical( MUSIC_TIME mtTimeLogical )
{
	ASSERT( m_pLyricMgr != NULL );
	if( m_pLyricMgr == NULL )
	{
		// Will be NULL when CLyricItem used for properties!
		return E_FAIL;
	}

	// Make sure item's logical time is not less than zero
	if( mtTimeLogical < 0 )
	{
		ASSERT( 0 );
		mtTimeLogical = 0;
	}

	// Set item's logical time
	m_mtTimeLogical = mtTimeLogical;

	if( m_pLyricMgr == NULL 
	||  m_pLyricMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Set item's logical measure, beat
	long lTick;
	m_pLyricMgr->ClocksToMeasureBeatTick( m_mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
	m_pLyricMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &m_mtTimeLogical );

	return S_OK;
}
