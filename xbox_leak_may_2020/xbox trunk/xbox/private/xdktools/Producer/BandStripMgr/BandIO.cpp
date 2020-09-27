#include "stdafx.h"
#include "BandIO.h"
#include "BandMgr.h"



CBandItem::CBandItem( CBandStrip* pBandStrip )
{
	m_fSelected = FALSE;
	m_pBandStrip = pBandStrip;
}

CBandItem::~CBandItem()
{
	// Must happen in CBanditem class - not CPropBand class
	// DeleteNode should only be called when CBandItem is deleted
	if( m_pIBandNode )
	{
		m_pIBandNode->DeleteNode( FALSE );
	}
}

BOOL CBandItem::After(const CBandItem& Band)
{
	if( m_mtTimePhysical > Band.m_mtTimePhysical )
	{
		return TRUE;
	}
	else if( m_dwMeasure > Band.m_dwMeasure )
	{
		return TRUE;
	}
	else if( m_dwMeasure == Band.m_dwMeasure )
	{
		if( m_bBeat > Band.m_bBeat )
		{
			return TRUE;
		}
		else if( m_bBeat == Band.m_bBeat )
		{
			if( m_lTick > Band.m_lTick )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CBandItem::Before(const CBandItem& Band)
{
	if( m_mtTimePhysical < Band.m_mtTimePhysical )
	{
		return TRUE;
	}
	if( m_dwMeasure < Band.m_dwMeasure )
	{
		return TRUE;
	}
	else if( m_dwMeasure == Band.m_dwMeasure )
	{
		if( m_bBeat < Band.m_bBeat )
		{
			return TRUE;
		}
		else if( m_bBeat == Band.m_bBeat )
		{
			if( m_lTick < Band.m_lTick )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
	
void CBandItem::SetSelectFlag( BOOL fSelected )
{
	m_fSelected = fSelected;
	m_dwBits = 0;
}
	
HRESULT CBandItem::SetTimePhysical( MUSIC_TIME mtTimePhysical, short nAction )
{
	// Set item's physical time
	MUSIC_TIME mtOrigTimePhysical = m_mtTimePhysical;
	m_mtTimePhysical = mtTimePhysical;

	if( m_pBandStrip == NULL
	||  m_pBandStrip->m_pBandMgr == NULL 
	||  m_pBandStrip->m_pBandMgr->m_pTimeline == NULL )
	{
		if( nAction == STP_LOGICAL_NO_ACTION 
		||  nAction == STP_LOGICAL_RECALC_MEASURE_BEAT )
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
	m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( m_mtTimePhysical, &lMeasure, &lBeat, &lTick );
	m_dwMeasure = lMeasure;
	m_bBeat = (BYTE)lBeat;
	m_lTick = lTick;

	// Set item's logical time
	switch( nAction )
	{
		case STP_LOGICAL_NO_ACTION:
			// Nothing to do
			break;

		case STP_LOGICAL_RECALC_MEASURE_BEAT:
			// Simply recalc item's logical measure/beat
			m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( m_mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			break;

		case STP_LOGICAL_SET_DEFAULT:
			m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( m_mtTimePhysical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			m_pBandStrip->m_pBandMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &m_mtTimeLogical );
			break;

		case STP_LOGICAL_FROM_BEAT_OFFSET:
		{
			// Get track GroupBits
			DWORD dwGroupBits = m_pBandStrip->m_pBandMgr->GetGroupBits();

			// Snap new physical time to number of beats
			long lPhysicalBeats;
			m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( m_mtTimePhysical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pBandStrip->m_pBandMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Set item's new logical time
			long lNewLogicalBeats = lPhysicalBeats + m_mtTimeLogical;	// m_mtTimeLogical stores beat offset
			if( lNewLogicalBeats < 0 )
			{
				lNewLogicalBeats = 0;
			}
			BeatsToMeasureBeat( m_pBandStrip->m_pBandMgr->m_pTimeline, dwGroupBits, 0, lNewLogicalBeats, lMeasure, lBeat );
			m_pBandStrip->m_pBandMgr->ForceBoundaries( lMeasure, lBeat, 0, &m_mtTimeLogical );
			m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( m_mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			m_pBandStrip->m_pBandMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &m_mtTimeLogical );
			break;
		}

		case STP_LOGICAL_ADJUST:
		{
			// Get track GroupBits
			DWORD dwGroupBits = m_pBandStrip->m_pBandMgr->GetGroupBits();

			// Snap original physical time to number of beats
			long lPhysicalBeats;
			m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( mtOrigTimePhysical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pBandStrip->m_pBandMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Convert logical time to number of beats
			long lLogicalBeats;
			m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( m_mtTimeLogical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pBandStrip->m_pBandMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lLogicalBeats );

			// Compute difference between original physical time and original logical time
			long lBeatDiff = lLogicalBeats - lPhysicalBeats;

			// Snap new physical time to number of beats
			m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( m_mtTimePhysical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pBandStrip->m_pBandMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Set item's new logical time
			long lNewLogicalBeats = lPhysicalBeats + lBeatDiff;
			if( lNewLogicalBeats < 0 )
			{
				lNewLogicalBeats = 0;
			}
			BeatsToMeasureBeat( m_pBandStrip->m_pBandMgr->m_pTimeline, dwGroupBits, 0, lNewLogicalBeats, lMeasure, lBeat );
			m_pBandStrip->m_pBandMgr->ForceBoundaries( lMeasure, lBeat, 0, &m_mtTimeLogical );
			m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( m_mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			m_pBandStrip->m_pBandMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &m_mtTimeLogical );
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
	
HRESULT CBandItem::SetTimeLogical( MUSIC_TIME mtTimeLogical )
{
	// Make sure item's logical time is not less than zero
	if( mtTimeLogical < 0 )
	{
		ASSERT( 0 );
		mtTimeLogical = 0;
	}

	// Set item's logical time
	m_mtTimeLogical = mtTimeLogical;

	if( m_pBandStrip == NULL
	||  m_pBandStrip->m_pBandMgr == NULL 
	||  m_pBandStrip->m_pBandMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Set item's logical measure, beat
	long lTick;
	m_pBandStrip->m_pBandMgr->ClocksToMeasureBeatTick( m_mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
	m_pBandStrip->m_pBandMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &m_mtTimeLogical );

	return S_OK;
}
