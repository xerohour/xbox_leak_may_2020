#include "stdafx.h"
#include "PropItem.h"
#include "EventItem.h"
#include "SequenceMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPropItem::CPropItem( CSequenceMgr *pSequenceMgr )
{
	Clear();
	ASSERT( pSequenceMgr );
	m_pSequenceMgr = pSequenceMgr;
}

CPropItem::CPropItem()
{
	Clear();
	m_pSequenceMgr = NULL;
}

void CPropItem::Clear()
{
	CEventItem::Clear();

	m_lStartBar = 0;
	m_lStartBeat = 0;
	m_lStartGrid = 0;
	m_lStartTick = 0;

	m_lEndBar = 0;
	m_lEndBeat = 0;
	m_lEndGrid = 0;
	m_lEndTick = 0;

	m_lDurBar = 0;
	m_lDurBeat = 0;
	m_lDurGrid = 0;
	m_lDurTick = 0;

	m_lChordBar = 0;
	m_lChordBeat = 0;

	m_dwUndetermined = UD_ALL;
	m_dwChanged = 0;
}

void CPropItem::Import( const CEventItem* pEventItem )
{
	ASSERT( pEventItem != NULL );
	if( (pEventItem == NULL) )
	{
		return;
	}

	Clear();
	CEventItem::Copy( pEventItem );
	m_dwUndetermined &= ~(UD_OFFSET | UD_TIMESTART | UD_DURATION);

	TimeOffsetToBarBeat();
}

void CPropItem::Copy( const CPropItem* pPropItem )
{
	if( pPropItem == NULL )
	{
		Clear();
		return;
	}

	if( pPropItem == this )
	{
		return;
	}

	CEventItem::Copy( pPropItem );

	m_lStartBar = pPropItem->m_lStartBar;
	m_lStartBeat = pPropItem->m_lStartBeat;
	m_lStartGrid = pPropItem->m_lStartGrid;
	m_lStartTick = pPropItem->m_lStartTick;

	m_lEndBar = pPropItem->m_lEndBar;
	m_lEndBeat = pPropItem->m_lEndBeat;
	m_lEndGrid = pPropItem->m_lEndGrid;
	m_lEndTick = pPropItem->m_lEndTick;

	m_lDurBar = pPropItem->m_lDurBar;
	m_lDurBeat = pPropItem->m_lDurBeat;
	m_lDurGrid = pPropItem->m_lDurGrid;
	m_lDurTick = pPropItem->m_lDurTick;

	m_lChordBar = pPropItem->m_lChordBar;
	m_lChordBeat = pPropItem->m_lChordBeat;

	m_pSequenceMgr = pPropItem->m_pSequenceMgr;
	m_dwUndetermined = pPropItem->m_dwUndetermined;
	m_dwChanged = pPropItem->m_dwChanged;
}

BOOL CPropItem::IsEqual( const CPropItem *pPropItem ) const
{
	if( CEventItem::IsEqual( pPropItem ) &&
		(m_dwUndetermined == pPropItem->m_dwUndetermined) &&
		(m_dwChanged == pPropItem->m_dwChanged) &&
		(m_lStartBar == pPropItem->m_lStartBar) &&
		(m_lStartBeat == pPropItem->m_lStartBeat) &&
		(m_lStartGrid == pPropItem->m_lStartGrid) &&
		(m_lStartTick == pPropItem->m_lStartTick) &&
		(m_lEndBar == pPropItem->m_lEndBar) &&
		(m_lEndBeat == pPropItem->m_lEndBeat) &&
		(m_lEndGrid == pPropItem->m_lEndGrid) &&
		(m_lEndTick == pPropItem->m_lEndTick) &&
		(m_lDurBar == pPropItem->m_lDurBar) &&
		(m_lDurBeat == pPropItem->m_lDurBeat) &&
		(m_lDurGrid == pPropItem->m_lDurGrid) &&
		(m_lDurTick == pPropItem->m_lDurTick) &&
		(m_lChordBar == pPropItem->m_lChordBar) &&
		(m_lChordBeat == pPropItem->m_lChordBeat) )
	{
		return TRUE;
	}
	return FALSE;
}

void CPropItem::TimeOffsetToBarBeat()
{
	// If both the start time and the duration are undetermined, exit
	if( (m_dwUndetermined & UD_TIMESTART) &&
		(m_dwUndetermined & UD_DURATION) )
	{
		return;
	}

	//
	// Set Start parameters
	//
	if ( (m_dwUndetermined & (UD_TIMESTART | UD_OFFSET )) == 0)
	{
		TimeToBarBeatGridTick( AbsTime(), &m_lStartBar, &m_lStartBeat, &m_lStartGrid, &m_lStartTick );
		m_lStartBar++;
		m_lStartBeat++;
		m_lStartGrid++;
		
		m_dwUndetermined = m_dwUndetermined & ~(UD_STARTBAR | UD_STARTBEAT | UD_STARTGRID | UD_STARTTICK);
	}

	//
	// Set Duration parameters
	//
	if ( (m_dwUndetermined & UD_DURATION) == 0)
	{
		// BUGBUG: This doesn't work correctly if lGridClocks * wGridsPerBeat != lBeatClocks
		// Any notes in the second half of the last grid in a beat will have the WRONG tick
		// (since there are more ticks in the last grid of the beat than in the other grids).

		// Get the Time Signature at the start point
		DMUS_TIMESIGNATURE ts;
		GetTimeSig( AbsTime(), &ts );

		// Compute the number of clocks for this time sig
		long lMeasureClocks, lBeatClocks, lGridClocks;
		lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
		lMeasureClocks = lBeatClocks * ts.bBeatsPerMeasure;
		lGridClocks = lBeatClocks / ts.wGridsPerBeat;

		long lTempMeas, lTempBeat, lTempGrid, lTempTick;
		long lTempTime = m_mtDuration % lBeatClocks;
		BOOL fAddedGrid = FALSE;
		if( (lTempTime % lGridClocks) > ( lGridClocks - (lGridClocks / 2) - 1) )
		{
			lTempTime = m_mtDuration + lGridClocks;
			fAddedGrid = TRUE;
		}
		else
		{
			lTempTime = m_mtDuration;
		}
		
		lTempMeas = lTempTime / lMeasureClocks;
		lTempBeat = lTempTime % lMeasureClocks;
		lTempGrid = lTempBeat % lBeatClocks;
		lTempBeat /= lBeatClocks;
		lTempTick = lTempGrid % lGridClocks;
		lTempGrid /= lGridClocks;

		if( fAddedGrid )
		{
			lTempTick -= lGridClocks;
		}

		m_lDurBar = lTempMeas;
		m_lDurBeat = lTempBeat;
		m_lDurGrid = lTempGrid;
		m_lDurTick = lTempTick;
		m_dwUndetermined = m_dwUndetermined & ~(UD_DURBAR | UD_DURBEAT | UD_DURGRID | UD_DURTICK);
	}

	//
	// Set Belongs to Beat parameters
	//
	if ( (m_dwUndetermined & UD_TIMESTART) == 0)
	{
		m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, m_mtTime, &m_lChordBar, &m_lChordBeat );
		m_lChordBar++;
		m_lChordBeat++;
		m_dwUndetermined = m_dwUndetermined & ~(UD_CHORDBAR | UD_CHORDBEAT);
	}

	//
	// Set End parameters
	//
	if ( (m_dwUndetermined & (UD_TIMESTART | UD_OFFSET | UD_DURATION)) == 0)
	{
		TimeToBarBeatGridTick( AbsTime() + m_mtDuration, &m_lEndBar, &m_lEndBeat, &m_lEndGrid, &m_lEndTick );
		m_lEndBar++;
		m_lEndBeat++;
		m_lEndGrid++;

		m_dwUndetermined = m_dwUndetermined & ~(UD_ENDBAR | UD_ENDBEAT | UD_ENDGRID | UD_ENDTICK);
	}
}

void CPropItem::GetTimeSig( MUSIC_TIME mtTime, DMUS_TIMESIGNATURE *pTimeSig ) const
{
	ASSERT( pTimeSig );
	if( m_pSequenceMgr )
	{
		::GetTimeSig( mtTime, pTimeSig, m_pSequenceMgr->m_pTimeline, m_pSequenceMgr->m_dwGroupBits );
	}
	else
	{
		pTimeSig->bBeatsPerMeasure = 4;
		pTimeSig->bBeat = 4;
		pTimeSig->wGridsPerBeat = 4;
	}
}

/*
void CPropItem::GetTimeSigTimeFromBar( long lMeasure, MUSIC_TIME *pmtTime, DMUS_TIMESIGNATURE *pTimeSig ) const
{
	// If the measure # is negative, set it to 0
	long lNegative = 0;
	if( lMeasure < 0 )
	{
		lNegative = lMeasure;
		lMeasure = 0;
	}

	ASSERT( m_pSequenceMgr );
	if( m_pSequenceMgr && m_pSequenceMgr->m_pTimeline )
	{
		ASSERT( pmtTime );
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lMeasure, 0, pmtTime ) ) )
		{
			ASSERT( pTimeSig );
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, *pmtTime, NULL, pTimeSig ) ) )
			{
				if( lNegative != 0 )
				{
					*pmtTime = lNegative * ((DMUS_PPQ * 4) / pTimeSig->bBeat) * pTimeSig->bBeatsPerMeasure;
				}
			}
		}
	}
}
*/

/*
void CPropItem::GetTimeSigBarTimeFromTime( MUSIC_TIME mtTime, long *plMeasure, MUSIC_TIME *pmtTime, DMUS_TIMESIGNATURE *pTimeSig ) const
{
	// If the time is negative, set it to 0
	MUSIC_TIME mtNegative = 0;
	if( mtTime < 0 )
	{
		mtNegative = mtTime;
		mtTime = 0;
	}

	ASSERT( m_pSequenceMgr );
	if( m_pSequenceMgr && m_pSequenceMgr->m_pTimeline )
	{
		ASSERT( plMeasure );
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, mtTime, plMeasure, NULL ) ) )
		{
			ASSERT( pmtTime );
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, *plMeasure, 0, pmtTime ) ) )
			{
				ASSERT( pTimeSig );
				if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, *pmtTime, NULL, pTimeSig ) ) )
				{
					if( mtNegative != 0 )
					{
						long lMeasureClocks = ((DMUS_PPQ * 4) / pTimeSig->bBeat) * pTimeSig->bBeatsPerMeasure;
						*plMeasure = mtNegative / lMeasureClocks;
						if( *plMeasure * lMeasureClocks > mtNegative )
						{
							*plMeasure--;
						}
						*pmtTime = *plMeasure * lMeasureClocks;
					}
				}
			}
		}
	}
}
*/

// If Chord bar/beat and start bar/beat/grid/tick are all determined, update m_mtTime and m_nOffset
// m_dwUndetermined and m_dwChanged will be updated
// If Dur bar/beat/grid/tick are all determined, update m_mtDuration, m_dwUndetermined and m_dwChanged
void CPropItem::BarBeatToTimeOffset()
{
	// only set the start time if all our data is determined
	//ASSERT ( (m_dwUndetermined & (UD_CHORDBAR | UD_CHORDBEAT | UD_STARTBAR | UD_STARTBEAT | UD_STARTGRID | UD_STARTTICK)) == 0);
	if ( (m_dwUndetermined & (UD_CHORDBAR | UD_CHORDBEAT | UD_STARTBARBEATGRIDTICK)) == 0)
	{
		// Compute the time that the note belongs to
		BarBeatGridTickToTime( m_lChordBar - 1, m_lChordBeat - 1, 0, 0, &m_mtTime );

		// Ensure m_mtTime is no later than the end of the segment and no earlier than time 0
		m_mtTime = min( max(0, m_mtTime), m_pSequenceMgr->m_pSequenceStrip->m_mtLength - 1 );

		// Compute the start time of the note
		long lStartTime;
		BarBeatGridTickToTime( m_lStartBar - 1, m_lStartBeat - 1, m_lStartGrid - 1, m_lStartTick, &lStartTime );
		
		// Convert lStartTime into an offset from m_mtTime;
		lStartTime -= m_mtTime;

		// lStartTime is the offset from m_mtTime to the actual start time of the note - ensure it is withing
		// the bounds of a short
		if ( lStartTime > SHRT_MAX )
		{
			lStartTime = SHRT_MAX;
		}
		else if ( lStartTime < SHRT_MIN )
		{
			lStartTime = SHRT_MIN;
		}

		// Now ensure lStartTime does not put the event more than one measure ahead of the sequence
		DMUS_TIMESIGNATURE ts;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, 0, NULL, &ts ) ) )
		{
			if( lStartTime + m_mtTime < -(ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat)) )
			{
				lStartTime = -(ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat)) - m_mtTime;
			}
		}

		// Now ensure lStartTime does not put the event more than one measure after the segment
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, m_pSequenceMgr->m_pSequenceStrip->m_mtLength - 1, NULL, &ts ) ) )
		{
			if( lStartTime + m_mtTime > m_pSequenceMgr->m_pSequenceStrip->m_mtLength - 1 + (ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat)) )
			{
				lStartTime = m_pSequenceMgr->m_pSequenceStrip->m_mtLength - 1 + (ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat)) - m_mtTime;
			}
		}

		m_nOffset = short( lStartTime );

		m_dwUndetermined = m_dwUndetermined & ~(UD_TIMESTART | UD_OFFSET);
		m_dwChanged |= CHGD_TIMESTART | CHGD_OFFSET;
	}

	// only set the duration if all our data is determined
	//ASSERT ( (m_dwUndetermined & (UD_DURBAR | UD_DURBEAT | UD_DURGRID | UD_DURTICK)) == 0);
	if ( (m_dwUndetermined & UD_DURBARBEATGRIDTICK) == 0)
	{
		DMUS_TIMESIGNATURE ts;
		GetTimeSig( AbsTime(), &ts );
		long lTemp, lBeatClocks, lMeasureClocks, lGridClocks;
		lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
		lMeasureClocks = lBeatClocks * ts.bBeatsPerMeasure;
		lGridClocks = lBeatClocks / ts.wGridsPerBeat;
		lTemp = m_lDurTick;
		lTemp += m_lDurGrid * lGridClocks;
		lTemp += m_lDurBeat * lBeatClocks;
		lTemp += m_lDurBar * lMeasureClocks;
		if ( lTemp < 1 )
		{
			lTemp = 1;
		}
		m_mtDuration = lTemp;
		m_dwUndetermined = m_dwUndetermined & ~(UD_DURATION);
		m_dwChanged |= CHGD_DURATION;
	}
}

void CPropItem::TimeToBarBeatGridTick( MUSIC_TIME mtTime, long *plMeasure, long *plBeat, long *plGrid, long *plTick ) const
{
	ASSERT( plMeasure );
	ASSERT( m_pSequenceMgr && m_pSequenceMgr->m_pTimeline );

	// Find out which measure we're in
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, mtTime, plMeasure, NULL ) ) )
	{
		// Find the time of the start of this measure
		long lTime;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, *plMeasure, 0, &lTime ) ) )
		{
			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE ts;
			if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lTime, NULL, &ts ) ) )
			{
				// Compute the number of clocks in a beat and a grid
				long lBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
				long lGridClocks = lBeatClocks / ts.wGridsPerBeat;

				// BUGBUG: This doesn't work correctly if lGridClocks * wGridsPerBeat != lBeatClocks
				// Any notes in the second half of the last grid in a beat will have the WRONG tick
				// (since there are more ticks in the last grid of the beat than in the other grids).

				// Check if the time is in the second half of the grid - if so bump up the start time by one grid
				long lTempTime = (mtTime - lTime) % lBeatClocks;
				BOOL fAddedGrid = FALSE;
				if( (lTempTime % lGridClocks) > ( lGridClocks - (lGridClocks / 2) - 1) )
				{
					mtTime += lGridClocks;
					fAddedGrid = TRUE;
				}

				// Convert mtTime into an offset from the start of this measure
				mtTime -= lTime;

				// If mtTime is as long as or greater than a measure, we must have been in the second half of a grid
				// that was the very last grid in the measure
				if( mtTime >= lBeatClocks * ts.bBeatsPerMeasure )
				{
					ASSERT( fAddedGrid );
					mtTime -= lBeatClocks * ts.bBeatsPerMeasure;
					(*plMeasure)++;
				}
				
				if( plBeat )
				{
					*plBeat = mtTime / lBeatClocks;
				}

				mtTime %= lBeatClocks;
				if( plGrid )
				{
					*plGrid = mtTime / lGridClocks;
				}

				if( plTick )
				{
					*plTick = mtTime % lGridClocks;
					if( fAddedGrid )
					{
						*plTick -= lGridClocks;
					}
				}
			}
		}
	}
}

void CPropItem::BarBeatGridTickToTime( long lMeasure, long lBeat, long lGrid, long lTick, MUSIC_TIME *pmtTime ) const
{
	ASSERT( pmtTime );
	ASSERT( m_pSequenceMgr && m_pSequenceMgr->m_pTimeline );
	if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, lMeasure, lBeat, pmtTime ) ) )
	{
		DMUS_TIMESIGNATURE ts;
		if( SUCCEEDED( m_pSequenceMgr->m_pTimeline->GetParam( GUID_TimeSignature, m_pSequenceMgr->m_dwGroupBits, m_pSequenceMgr->m_dwIndex, *pmtTime, NULL, &ts ) ) )
		{
			*pmtTime += lTick + lGrid * (((DMUS_PPQ * 4) / ts.bBeat) / ts.wGridsPerBeat);
		}
	}
}

DWORD CPropItem::ApplyPropItem( const CPropItem* pPropNote )
{
	DWORD dwChanged = 0;

	if (pPropNote->m_dwChanged & CHGD_OFFSET)
	{
		// If the offset values are different or our offset is Undefined, update it
		if ( (m_nOffset!=pPropNote->m_nOffset) || (m_dwUndetermined & UD_OFFSET) )
		{
			dwChanged |= CHGD_OFFSET;
			m_nOffset = pPropNote->m_nOffset;
			m_dwUndetermined &= ~UD_OFFSET;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_TIMESTART)
	{
		// If the grid start values are different or our grid start is Undefined, update it
		if ( (m_mtTime != pPropNote->m_mtTime) || (m_dwUndetermined & UD_TIMESTART) )
		{
			dwChanged |= CHGD_TIMESTART;
			m_mtTime = pPropNote->m_mtTime;
			m_dwUndetermined &= ~UD_TIMESTART;
		}
	}
	// if possible, update the Start and End bar/beat/grid/tick
	if ( (dwChanged!=0) && !(m_dwUndetermined & (UD_OFFSET | UD_TIMESTART)))
	{
		TimeOffsetToBarBeat();
		dwChanged |= CHGD_START_BARBEATGRIDTICK;
		if( !(m_dwUndetermined & UD_DURATION) )
		{
			dwChanged |= CHGD_END_BARBEATGRIDTICK;
		}
	}
	
	if (pPropNote->m_dwChanged & CHGD_DURATION)
	{
		// If the duration values are different or our duration is Undefined, update it
		if ( (m_mtDuration != pPropNote->m_mtDuration) || (m_dwUndetermined & UD_DURATION) )
		{
			dwChanged |= CHGD_DURATION;
			m_mtDuration = pPropNote->m_mtDuration;
			m_dwUndetermined &= ~UD_DURATION;

			// update the Duration bar/beat/grid/tick
			TimeOffsetToBarBeat();
			dwChanged |= CHGD_DUR_BARBEATGRIDTICK;

			// If the start time is determined, mark that we also changed the end bar/beat/grid/tick
			if( !(m_dwUndetermined & (UD_OFFSET | UD_TIMESTART)) )
			{
				dwChanged |= CHGD_END_BARBEATGRIDTICK;
			}
		}
	}

	if (pPropNote->m_dwChanged & CHGD_STARTBAR)
	{
		// If the Start bar values are different
		if (m_lStartBar != pPropNote->m_lStartBar)
		{
			// If Chord bar and Start bar are the same, also update the chord bar
			// But only if the new start bar is at least 1
			if ((pPropNote->m_lStartBar >= 1) && (m_lChordBar == m_lStartBar))
			{
				dwChanged |= CHGD_CHORDBAR;
				m_lChordBar = pPropNote->m_lStartBar;
				m_dwUndetermined &= ~UD_CHORDBAR;
			}

			// Now update the start bar
			dwChanged |= CHGD_STARTBAR;
			m_lStartBar = pPropNote->m_lStartBar;
			m_dwUndetermined &= ~UD_STARTBAR;
		}

	}
	if (pPropNote->m_dwChanged & CHGD_STARTBEAT)
	{
		// If the Start beat values are different
		if (m_lStartBeat != pPropNote->m_lStartBeat)
		{
			// If Chord bar and Start bar are the same, and chord beat and start beat
			// are the same, also update the chord beat
			// But only if the new start bar and beat are at least 1
			if ( ( (pPropNote->m_lStartBar > 1) ||
				   (pPropNote->m_lStartBeat >= 1) )
			  && (m_lChordBeat == m_lStartBeat) && (m_lChordBar == m_lStartBar))
			{
				dwChanged |= CHGD_CHORDBEAT;
				m_lChordBeat = pPropNote->m_lStartBeat;
				m_dwUndetermined &= ~UD_CHORDBEAT;
			}

			// Now update the start beat
			dwChanged |= CHGD_STARTBEAT;
			m_lStartBeat = pPropNote->m_lStartBeat;
			m_dwUndetermined &= ~UD_STARTBEAT;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_STARTGRID)
	{
		// If the Start grid values are different or our start grid is Undefined, update it
		if (m_lStartGrid != pPropNote->m_lStartGrid)
		{
			dwChanged |= CHGD_STARTGRID;
			m_lStartGrid = pPropNote->m_lStartGrid;
			m_dwUndetermined &= ~UD_STARTGRID;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_STARTTICK)
	{
		// If the Start tick values are different or our start tick is Undefined, update it
		if (m_lStartTick != pPropNote->m_lStartTick)
		{
			dwChanged |= CHGD_STARTTICK;
			m_lStartTick = pPropNote->m_lStartTick;
			m_dwUndetermined &= ~UD_STARTTICK;
		}
	}
	// If at least one of the start bar/beat/grid/tick changed, convert start to a Grid and Offset
	// (but only if all of our start parameters are known)
	if( (dwChanged & CHGD_START_BARBEATGRIDTICK) && !(m_dwUndetermined & UD_STARTBARBEATGRIDTICK) )
	{
		BarBeatToTimeOffset();
		dwChanged |= CHGD_OFFSET | CHGD_TIMESTART;
	}
	
	if (pPropNote->m_dwChanged & CHGD_ENDBAR)
	{
		// If the end bar values are different or our end bar is Undefined, update it
		if (m_lEndBar != pPropNote->m_lEndBar)
		{
			dwChanged |= CHGD_ENDBAR;
			m_lEndBar = pPropNote->m_lEndBar;
			m_dwUndetermined &= ~UD_ENDBAR;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_ENDBEAT)
	{
		// If the end beat values are different or our end beat is Undefined, update it
		if (m_lEndBeat != pPropNote->m_lEndBeat)
		{
			dwChanged |= CHGD_ENDBEAT;
			m_lEndBeat = pPropNote->m_lEndBeat;
			m_dwUndetermined &= ~UD_ENDBEAT;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_ENDGRID)
	{
		// If the end grid values are different or our end grid is Undefined, update it
		if (m_lEndGrid != pPropNote->m_lEndGrid)
		{
			dwChanged |= CHGD_ENDGRID;
			m_lEndGrid = pPropNote->m_lEndGrid;
			m_dwUndetermined &= ~UD_ENDGRID;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_ENDTICK)
	{
		// If the end tick values are different or our end tick is Undefined, update it
		if (m_lEndTick != pPropNote->m_lEndTick)
		{
			dwChanged |= CHGD_ENDTICK;
			m_lEndTick = pPropNote->m_lEndTick;
			m_dwUndetermined &= ~UD_ENDTICK;
		}
	}
	// If at least one of the end bar/beat/grid/tick changed, update duration
	// only set the duration if all of our start and end data is determined
	if( (dwChanged & CHGD_END_BARBEATGRIDTICK) &&
		!(m_dwUndetermined & (UD_ENDBARBEATGRIDTICK | UD_STARTBARBEATGRIDTICK)) )
	{
		long lStartTime, lEndTime;
		BarBeatGridTickToTime( m_lStartBar - 1, m_lStartBeat - 1, m_lStartGrid - 1, m_lStartTick, &lStartTime );
		BarBeatGridTickToTime( m_lEndBar - 1, m_lEndBeat - 1, m_lEndGrid - 1, m_lEndTick, &lEndTime );

		lEndTime -= lStartTime;
		if ( lEndTime < 1 )
		{
			lEndTime = 1;
		}
		//else if ( lEndTime > LONG_MAX )
		//{
		//	lEndTime = LONG_MAX;
		//}
		m_mtDuration = lEndTime;
		m_dwUndetermined &= ~(UD_DURATION);
		m_dwChanged |= CHGD_DURATION;
	}
	
	if (pPropNote->m_dwChanged & CHGD_DURBAR)
	{
		// If the duration bar values are different or our duration bar is Undefined, update it
		if (m_lDurBar != pPropNote->m_lDurBar)
		{
			dwChanged |= CHGD_DURBAR;
			m_lDurBar = pPropNote->m_lDurBar;
			m_dwUndetermined &= ~UD_DURBAR;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_DURBEAT)
	{
		// If the duration beat values are different or our duration beat is Undefined, update it
		if (m_lDurBeat != pPropNote->m_lDurBeat)
		{
			dwChanged |= CHGD_DURBEAT;
			m_lDurBeat = pPropNote->m_lDurBeat;
			m_dwUndetermined &= ~UD_DURBEAT;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_DURGRID)
	{
		// If the duration grid values are different or our duration grid is Undefined, update it
		if (m_lDurGrid != pPropNote->m_lDurGrid)
		{
			dwChanged |= CHGD_DURGRID;
			m_lDurGrid = pPropNote->m_lDurGrid;
			m_dwUndetermined &= ~UD_DURGRID;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_DURTICK)
	{
		// If the duration tick values are different or our duration tick is Undefined, update it
		if (m_lDurTick != pPropNote->m_lDurTick)
		{
			dwChanged |= CHGD_DURTICK;
			m_lDurTick = pPropNote->m_lDurTick;
			m_dwUndetermined &= ~UD_DURTICK;
		}
	}
	// If at least one of the duration bar/beat/grid/tick changed, update duration
	if( (dwChanged & CHGD_DUR_BARBEATGRIDTICK) &&
		!(m_dwUndetermined & UD_DURBARBEATGRIDTICK) )
	{
		BarBeatToTimeOffset();
		dwChanged |= CHGD_DURATION;
	}
	
	if (pPropNote->m_dwChanged & CHGD_CHORDBAR)
	{
		// If the chord bar values are different or our chord bar is Undefined, update it
		if (m_lChordBar != pPropNote->m_lChordBar)
		{
			dwChanged |= CHGD_CHORDBAR;
			m_lChordBar = pPropNote->m_lChordBar;
			m_dwUndetermined &= ~UD_CHORDBAR;
		}
	}
	if (pPropNote->m_dwChanged & CHGD_CHORDBEAT)
	{
		// If the chord beat values are different or our chord beat is Undefined, update it
		if (m_lChordBeat != pPropNote->m_lChordBeat)
		{
			// Ensure the ChordBeat and ChordBar don't go before bar 1, beat 1
			if( (pPropNote->m_lChordBeat >= 1) || (m_lChordBar > 1) )
			{
				dwChanged |= CHGD_CHORDBEAT;
				m_lChordBeat = pPropNote->m_lChordBeat;
				m_dwUndetermined &= ~UD_CHORDBEAT;
			}
		}
	}
	// If either the chord bar or chord beat changed, update the grid start time
	if( (dwChanged & (CHGD_CHORDBAR|CHGD_CHORDBEAT)) &&
		!(m_dwUndetermined & (UD_STARTBARBEATGRIDTICK | UD_CHORDBAR | UD_CHORDBEAT)) )
	{
		BarBeatToTimeOffset();
		dwChanged |= CHGD_TIMESTART | CHGD_OFFSET;
	}

	m_dwChanged |= dwChanged;

	return dwChanged;
}

CPropItem CPropItem::operator +=(const CPropItem &SeqItem)
{
	if (m_nOffset != SeqItem.m_nOffset)
	{
		m_dwUndetermined |= UD_OFFSET;
	}
	if (m_mtTime != SeqItem.m_mtTime)
	{
		m_dwUndetermined |= UD_TIMESTART;
	}
	if (m_mtDuration != SeqItem.m_mtDuration)
	{
		m_dwUndetermined |= UD_DURATION;
	}

	if (m_lStartBar != SeqItem.m_lStartBar)
	{
		m_dwUndetermined |= UD_STARTBAR;
	}
	if (m_lStartBeat != SeqItem.m_lStartBeat)
	{
		m_dwUndetermined |= UD_STARTBEAT;
	}
	if (m_lStartGrid != SeqItem.m_lStartGrid)
	{
		m_dwUndetermined |= UD_STARTGRID;
	}
	if (m_lStartTick != SeqItem.m_lStartTick)
	{
		m_dwUndetermined |= UD_STARTTICK;
	}

	if (m_lEndBar != SeqItem.m_lEndBar)
	{
		m_dwUndetermined |= UD_ENDBAR;
	}
	if (m_lEndBeat != SeqItem.m_lEndBeat)
	{
		m_dwUndetermined |= UD_ENDBEAT;
	}
	if (m_lEndGrid != SeqItem.m_lEndGrid)
	{
		m_dwUndetermined |= UD_ENDGRID;
	}
	if (m_lEndTick != SeqItem.m_lEndTick)
	{
		m_dwUndetermined |= UD_ENDTICK;
	}

	if (m_lDurBar != SeqItem.m_lDurBar)
	{
		m_dwUndetermined |= UD_DURBAR;
	}
	if (m_lDurBeat != SeqItem.m_lDurBeat)
	{
		m_dwUndetermined |= UD_DURBEAT;
	}
	if (m_lDurGrid != SeqItem.m_lDurGrid)
	{
		m_dwUndetermined |= UD_DURGRID;
	}
	if (m_lDurTick != SeqItem.m_lDurTick)
	{
		m_dwUndetermined |= UD_DURTICK;
	}

	if (m_lChordBar != SeqItem.m_lChordBar)
	{
		m_dwUndetermined |= UD_CHORDBAR;
	}
	if (m_lChordBeat != SeqItem.m_lChordBeat)
	{
		m_dwUndetermined |= UD_CHORDBEAT;
	}

	return *this;
}

DWORD CPropItem::ApplyToEvent( CEventItem* pDMNote ) const
{
	ASSERT( pDMNote != NULL );
	if (pDMNote == NULL)
	{
		return 0;
	}

	DWORD dwChanged = 0;

	if (m_dwChanged & CHGD_OFFSET)
	{
		if (pDMNote->m_nOffset != m_nOffset)
		{
			dwChanged |= CHGD_OFFSET;
			pDMNote->m_nOffset = m_nOffset;
		}
	}
	if (m_dwChanged & CHGD_TIMESTART)
	{
		if (pDMNote->m_mtTime != m_mtTime)
		{
			dwChanged |= CHGD_TIMESTART;
			pDMNote->m_mtTime = m_mtTime;
		}
	}

	if (m_dwChanged & CHGD_DURATION)
	{
		if (pDMNote->m_mtDuration != m_mtDuration)
		{
			dwChanged |= CHGD_DURATION;
			pDMNote->m_mtDuration	= m_mtDuration;
		}
	}
	return dwChanged;
}
