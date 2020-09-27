#include "stdafx.h"
#include "PropNote.h"
#include "MIDI.h"
#include "MIDIStripMgr.h"
#include "ioDMStyle.h"

#pragma warning( push )
#pragma warning( disable : 4244 )
/*
ApplyPropNote:
pPropNote->m_wMusicvalue and pPropNote->m_bMIDIValue should not both be determined.

ApplyToDMNote
m_wMusicvalue and m_bMIDIValue should not both be determined.
*/

static const char pacMIDIToAccidental[12] = {0, -1, 0, -1, 0, 0, 1, 0, -1, 0, -1, 0};


void static MusicValueAndDiatonicOffsetToLogicalScaleAccidentalAndDiatonicOffset( WORD wMusicValue, char cDiatonicOffset, BYTE *pbScaleValue, signed char *pcAccidental, signed char *pcDiatonicOffset )
{
	ASSERT( pcAccidental || pbScaleValue || pcDiatonicOffset);
	if( !pcAccidental && !pbScaleValue && !pcDiatonicOffset )
	{
		return;
	}

	char cScale = ((wMusicValue & 0x00F0) >> 4) & 0x0f;
	if( cScale & 0x08 )
	{
		cScale |= 0xF0;
	}

	// Ensure the diatonic offset is only as big as the scale value
	if( cDiatonicOffset > cScale )
	{
		cDiatonicOffset = cScale;
	}
	else if( cDiatonicOffset == 0 )
	{
		if( cScale & 0x08 )
		{
			cDiatonicOffset = cScale;
		}
		else if( cScale > 1 )
		{
			cDiatonicOffset = cScale - 1;
		}
	}

	if( pcDiatonicOffset )
	{
		*pcDiatonicOffset = cDiatonicOffset;
	}

	if( pcAccidental )
	{
		*pcAccidental = (wMusicValue & 0x0008) ? wMusicValue | 0xF0 : wMusicValue & 0x07;
	}

	// get scale value of chord value
	if( pbScaleValue  )
	{
		*pbScaleValue = 1 + ((wMusicValue & 0x0F00) >> 7); //  >>7 is just >>8 followed by * 2

		// offset by diatonic value
		cScale -= cDiatonicOffset; 
		if (cScale)
		{
			(*pbScaleValue) += cScale;
		}
	}
}

void static LogicalScaleAccidentalAndDiatonicOffsetToMusicValueAndDiatonicOffset( BYTE bScaleValue, signed char cAccidental, signed char cDiatonicOffset, BYTE bOctave, WORD *pwMusicValue, signed char *pcDiatonicOffset )
{
	ASSERT( pwMusicValue && pcDiatonicOffset);
	if( !pwMusicValue || !pcDiatonicOffset )
	{
		return;
	}

	*pwMusicValue = 0;

	//
	//	Fill Chord Value...
	//
	if( bScaleValue )
	{
		*pwMusicValue = (unsigned short)((bScaleValue - 1) / 2) << 8;
	}

	// Initialize the diatonic offset
	*pcDiatonicOffset = cDiatonicOffset;

	//
	//	Fill the Key Value...
	//
	switch( bScaleValue )
	{
   	case 2:
	case 4:
	case 6:
		if( cDiatonicOffset < 7 )
		{
			// Add one to the diatonic offset used to compute the music value
			cDiatonicOffset++;
		}
		else
		{
			// Change the displayed diatonic offset to 6, to allow for the scale value to include an offset
			*pcDiatonicOffset = 6;
		}
	}

	*pwMusicValue |= (cDiatonicOffset & 0x0F) << 4;

	//
	//	Fill the Accidental Value...
	//	
	ASSERT( (cAccidental >= -7) && (cAccidental <= 7) );
	*pwMusicValue |= WORD( BYTE(cAccidental) & 0x0F );

	//
	//	Fill the Octive Value
	//
	*pwMusicValue |= WORD( (bOctave & 0xF) << 12 );
}

CPropItem::CPropItem()
{
	Clear();
}

void CPropItem::Clear()
{
	memset( this, 0, sizeof(CPropItem) );
	m_dwUndetermined = 0xFFFFFFFF;
	//m_fOffset = FALSE;
	//m_dwChanged = 0;
	//m_pDMPart = NULL;
	m_ts.m_bBeatsPerMeasure = 4;
	m_ts.m_bBeat = 4;
	m_ts.m_wGridsPerBeat = 4;
}

HRESULT CPropItem::SetPart( CDirectMusicPart *pDMPart )
{
	ASSERT( pDMPart != NULL );
	if ( pDMPart == NULL )
	{
		return E_INVALIDARG;
	}

	m_ts = pDMPart->m_TimeSignature;
	ASSERT( m_ts.m_bBeatsPerMeasure != 0 );
	ASSERT( m_ts.m_bBeat != 0 );
	ASSERT( m_ts.m_wGridsPerBeat != 0 );

	m_pDMPart = pDMPart;

	return S_OK;
}

HRESULT CPropItem::BarBeatToGridOffset()
{
	// Can't do this operation if we're doing an offset
	if (m_fOffset)
	{
		return E_ABORT;
	}

	ASSERT( m_pDMPart );
	if( !m_pDMPart )
	{
		return E_ABORT;
	}

	// only set the start time if all our data is determined
	//ASSERT ( (m_dwUndetermined & (UD_CHORDBAR | UD_CHORDBEAT | UD_STARTBAR | UD_STARTBEAT | UD_STARTGRID | UD_STARTTICK)) == 0);
	if ( (m_dwUndetermined & (UD_CHORDBAR | UD_CHORDBEAT | UD_STARTBARBEATGRIDTICK)) == 0)
	{
		long lBeatClocks = DM_PPQNx4 / m_ts.m_bBeat;

		// Compute the time that the note belongs to
		long lChordTime;
		lChordTime = BarBeatGridTickToTime( m_ts, m_lChordBar - 1, m_lChordBeat - 1, 0, 0 );

		// Ensure lChordTime is no later than the end of the part and no earlier than time 0
		lChordTime = min( max(0, lChordTime), m_pDMPart->GetClockLength() - (lBeatClocks / m_ts.m_wGridsPerBeat) );

		// Compute the start time of the note
		long lStartTime;
		lStartTime = BarBeatGridTickToTime( m_ts, m_lStartBar - 1, m_lStartBeat - 1, m_lStartGrid - 1, m_lStartTick );
		
		// Convert lStartTime into an offset from lChordTime;
		lStartTime -= lChordTime;

		// Now ensure lStartTime does not put the event more than one measure ahead of the sequence
		if( lStartTime + lChordTime < -(m_ts.m_bBeatsPerMeasure * lBeatClocks) )
		{
			lStartTime = -(m_ts.m_bBeatsPerMeasure * (DM_PPQNx4 / m_ts.m_bBeat)) - lChordTime;
		}

		// Now ensure lStartTime does not put the event more than one measure after the segment
		if( lStartTime + lChordTime > m_pDMPart->GetClockLength() + m_ts.m_bBeatsPerMeasure * lBeatClocks - 1 )
		{
			lStartTime = m_pDMPart->GetClockLength() + m_ts.m_bBeatsPerMeasure * lBeatClocks - lChordTime - 1;
		}

		m_nOffset = short( lStartTime );

		// If the chord time didn't change beat
		if( (lChordTime / lBeatClocks) == (m_mtGridStart / m_ts.m_wGridsPerBeat) )
		{
			// Don't update m_mtGridStart - modify the m_nOffset if we need to
			lChordTime -= GRID_TO_CLOCKS(m_mtGridStart, m_pDMPart);

			if( m_nOffset + lChordTime < -32768 )
			{
				m_nOffset = -32768;
			}
			else if( m_nOffset + lChordTime > 32767 )
			{
				m_nOffset = 32767;
			}
			else
			{
				m_nOffset += lChordTime;
			}
		}
		else
		{
			// Changed beat - update m_mtGridStart
			m_mtGridStart = CLOCKS_TO_GRID(lChordTime, m_pDMPart);
		}

		m_dwUndetermined = m_dwUndetermined & ~(UD_GRIDSTART | UD_OFFSET);
		m_dwChanged |= CHGD_GRIDSTART | CHGD_OFFSET;
	}

	// only set the duration if all our data is determined
	//ASSERT ( (m_dwUndetermined & (UD_DURBAR | UD_DURBEAT | UD_DURGRID | UD_DURTICK)) == 0);
	if ( (m_dwUndetermined & UD_DURBARBEATGRIDTICK) == 0)
	{
		long lTemp, lBeatClocks, lMeasureClocks, lGridClocks;
		lBeatClocks = DM_PPQNx4 / m_ts.m_bBeat;
		lMeasureClocks = lBeatClocks * m_ts.m_bBeatsPerMeasure;
		lGridClocks = lBeatClocks / m_ts.m_wGridsPerBeat;
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

	return S_OK;
}

MUSIC_TIME CPropItem::AbsTime() const
{
	long lBeatClocks, lGridClocks;
	lBeatClocks = DM_PPQNx4 / m_ts.m_bBeat;
	lGridClocks = lBeatClocks / m_ts.m_wGridsPerBeat;

	return m_nOffset + lBeatClocks * (m_mtGridStart / m_ts.m_wGridsPerBeat)
		+ lGridClocks * (m_mtGridStart % m_ts.m_wGridsPerBeat);
}

HRESULT CPropItem::GridOffsetToBarBeat()
{
	// Not supported if we are an offset
	if ( m_fOffset )
	{
		return E_ABORT;
	}

	// If both the grid time and the duration are undetermined, exit
	if( (m_dwUndetermined & UD_GRIDSTART) &&
		(m_dwUndetermined & UD_DURATION) )
	{
		return S_OK;
	}

	//
	// Set Start parameters
	//
	if ( (m_dwUndetermined & (UD_GRIDSTART | UD_OFFSET )) == 0)
	{
		TimeToBarBeatGridTick( m_ts, AbsTime(), &m_lStartBar, &m_lStartBeat, &m_lStartGrid, &m_lStartTick );
		m_lStartBar++;
		m_lStartBeat++;
		m_lStartGrid++;
		
		m_dwUndetermined &= ~(UD_STARTBAR | UD_STARTBEAT | UD_STARTGRID | UD_STARTTICK);
	}

	//
	// Set Duration parameters
	//
	if ( (m_dwUndetermined & UD_DURATION) == 0)
	{
		// This only works if the time signature doesn't change over time
		TimeToBarBeatGridTick( m_ts, m_mtDuration, &m_lDurBar, &m_lDurBeat, &m_lDurGrid, &m_lDurTick );
		
		m_dwUndetermined &= ~(UD_DURBAR | UD_DURBEAT | UD_DURGRID | UD_DURTICK);
	}

	//
	// Set End parameters
	//
	if ( (m_dwUndetermined & (UD_GRIDSTART | UD_OFFSET | UD_DURATION)) == 0)
	{
		TimeToBarBeatGridTick( m_ts, AbsTime() + m_mtDuration, &m_lEndBar, &m_lEndBeat, &m_lEndGrid, &m_lEndTick );
		m_lEndBar++;
		m_lEndBeat++;
		m_lEndGrid++;

		m_dwUndetermined &= ~(UD_ENDBAR | UD_ENDBEAT | UD_ENDGRID | UD_ENDTICK);
	}

	//
	// Set Chord parameters
	//
	if ( (m_dwUndetermined & UD_GRIDSTART) == 0)
	{
		m_lChordBar = 1 + (m_mtGridStart / (m_ts.m_bBeatsPerMeasure * m_ts.m_wGridsPerBeat));
		m_lChordBeat = 1 + ((m_mtGridStart % (m_ts.m_bBeatsPerMeasure * m_ts.m_wGridsPerBeat)) / m_ts.m_wGridsPerBeat);
		m_dwUndetermined = m_dwUndetermined & ~(UD_CHORDBAR | UD_CHORDBEAT);
	}

	return S_OK;
}

void CPropItem::TimeToBarBeatGridTick( DirectMusicTimeSig &dmTimeSig, MUSIC_TIME mtTime, long *plMeasure, long *plBeat, long *plGrid, long *plTick )
{
	ASSERT( plMeasure );

	long lBeatClocks, lMeasureClocks;
	lBeatClocks = DM_PPQNx4 / dmTimeSig.m_bBeat;
	lMeasureClocks = lBeatClocks * dmTimeSig.m_bBeatsPerMeasure;

	// Find out which measure we're in
	// If mtTime is less than 0
	if( mtTime < 0 )
	{
		*plMeasure = mtTime / lMeasureClocks;
		// Convert mtTime into an offset from the start of the measure
		mtTime = -(abs(mtTime) % lMeasureClocks);
	}
	else
	{
		*plMeasure = mtTime / lMeasureClocks;
		// Convert mtTime into an offset from the start of the measure
		mtTime %= lMeasureClocks;
	}

	if( plBeat || plGrid || plTick )
	{
		long lGridClocks = lBeatClocks / dmTimeSig.m_wGridsPerBeat;
		// BUGBUG: This doesn't work correctly if lGridClocks * wGridsPerBeat != lBeatClocks
		// Any notes in the second half of the last grid in a beat will have the WRONG tick
		// (since there are more ticks in the last grid of the beat than in the other grids).

		// If mtTime is negative, subtract one from lMeasure and add lMeasureClocks to lTime
		if ( mtTime < 0 )
		{
			mtTime += lMeasureClocks;
			(*plMeasure)--;
		}

		// Check if the time is in the second half of the grid - if so bump up the start time by one grid
		long lTempTime = mtTime % lBeatClocks;
		BOOL fAddedGrid = FALSE;
		if( (lTempTime % lGridClocks) > ( lGridClocks - (lGridClocks / 2) - 1) )
		{
			mtTime += lGridClocks;
			fAddedGrid = TRUE;
		}

		// If mtTime is as long as or greater than a measure, we must have been in the second half of a grid
		// that was the very last grid in the measure
		if( mtTime >= lMeasureClocks )
		{
			ASSERT( fAddedGrid );
			mtTime -= lMeasureClocks;
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

CPropNote::CPropNote()
{
	Clear();
}

CPropNote::CPropNote( CDirectMusicPart *pDMPart )
{
	ASSERT( pDMPart != NULL);

	CPropItem::Clear();

	m_ts = pDMPart->m_TimeSignature;
	ASSERT( m_ts.m_bBeatsPerMeasure != 0 );
	ASSERT( m_ts.m_bBeat != 0 );
	ASSERT( m_ts.m_wGridsPerBeat != 0 );

	m_pDMPart = pDMPart;
}

void CPropNote::Clear()
{
	memset( this, 0, sizeof(CPropNote) );
	CPropItem::Clear();
}

// If both the offset/clicktime and bar/beat/etc are being set, the bar/beat will
// override the clicktime.
DWORD CPropNote::ApplyPropNote(const CPropNote *pPropNote, const CDirectMusicPartRef* pPartRef)
{
	UNREFERENCED_PARAMETER( pPartRef );
	DWORD dwChanged = ApplyPropItem( pPropNote );

	if ( pPropNote->m_fOffset )
	{
		ASSERT(FALSE);
		/*
		// Here, m_bMIDIValue is actually an offset for the MIDI value of the note
		if (!((pPropNote->m_dwUndetermined & UD_MIDIVALUE) ||
			  (m_dwUndetermined & UD_MIDIVALUE)) )
		{
			// Should not happen!  AnnetteC
			ASSERT( 0 );

//			short nOffset = pPropNote->m_bMIDIValue;
//			if (m_bPlayMode == DMUS_PLAYMODE_FIXED)
//			{
//				// Assume m_wMusicvalue is in fixed format
//				if ((m_wMusicvalue + nOffset >= 0) &&
//					(m_wMusicvalue + nOffset < 128))
//				{
//					if ( nOffset != 0 )
//						dwChanged |= UD_MUSICVALUE;
//					m_wMusicvalue += nOffset;
//				}
//			}
//			else
//			{
//				// Assume m_wMusicvalue is in music format
//				BYTE bValue;
//				bValue = pPartRef->MusicValueToNote( m_wMusicvalue, DMUS_PLAYMODE_FIXEDTOSCALE );
//				if ((bValue + nOffset >= 0) && (bValue + nOffset < 128))
//				{
//					if ( nOffset != 0 )
//						dwChanged |= UD_MUSICVALUE;
//					bValue += nOffset;
//					m_wMusicvalue = pPartRef->NoteToMusicValue( bValue, DMUS_PLAYMODE_FIXEDTOSCALE );
//				}
//			}
		}
		*/
	}
	else if ( !m_fOffset )
	{
		if ( pPropNote->m_dwChanged & UD_VELOCITY )
		{
			// If the velocity values are different or our velocity is Undefined, update it
			if ( (m_bVelocity != pPropNote->m_bVelocity) || (m_dwUndetermined & UD_VELOCITY) )
			{
				dwChanged |= UD_VELOCITY;
				m_bVelocity = pPropNote->m_bVelocity;
				m_dwUndetermined &= ~UD_VELOCITY;
			}
		}
		if ( pPropNote->m_dwChanged & UD_PLAYMODE )
		{
			// Should not happen!  AnnetteC
			ASSERT( 0 );

//			if ( (m_bPlayMode != pPropNote->m_bPlayMode) || (m_dwUndetermined & UD_PLAYMODE) )
//			{
//				// If our Musicvalue is determined, and wMusicvalue is NOT changing
//				if (!(m_dwUndetermined & UD_MUSICVALUE) && (pPropNote->m_dwUndetermined & UD_MUSICVALUE))
//				{
//					// If we're changing to DMUS_PLAYMODE_FIXED, change m_wMusicvalue
//					if (pPropNote->m_bPlayMode == DMUS_PLAYMODE_FIXED)
//					{
//						WORD wNewMusicvalue = pPartRef->MusicValueToNote( m_wMusicvalue, DMUS_PLAYMODE_FIXEDTOSCALE );;
//						if (m_wMusicvalue != wNewMusicvalue)
//						{
//							m_wMusicvalue = wNewMusicvalue;
//							dwChanged |= UD_MUSICVALUE;
//						}
//					}
//					// If our playmode is determined and we're changing from DMUS_PLAYMODE_FIXED,
//					// change m_wMusicvalue
//					else if (!(m_dwUndetermined & UD_PLAYMODE) && (m_bPlayMode == DMUS_PLAYMODE_FIXED))
//					{
//						ASSERT( (m_wMusicvalue >=0) && (m_wMusicvalue <= 127) );
//						WORD wNewMusicvalue = pPartRef->NoteToMusicValue( (BYTE) m_wMusicvalue, DMUS_PLAYMODE_FIXEDTOSCALE );
//						if (m_wMusicvalue != wNewMusicvalue)
//						{
//							m_wMusicvalue = wNewMusicvalue;
//							dwChanged |= UD_MUSICVALUE;
//						}
//					}
//				}
//				dwChanged |= UD_PLAYMODE;
//				m_bPlayMode = pPropNote->m_bPlayMode;
//				m_dwUndetermined &= ~UD_PLAYMODE;
//			}
		}
		if ( pPropNote->m_dwChanged & UD_MUSICVALUE )
		{
			// Should not happen!  AnnetteC
			ASSERT( 0 );

//			if ( (m_wMusicvalue != pPropNote->m_wMusicvalue) || (m_dwUndetermined & UD_MUSICVALUE) )
//				dwChanged |= UD_MUSICVALUE;
//
//			// If the source propnote's playmode is undetermined, and our playmode is DMUS_PLAYMODE_FIXED,
//			// translate the musicvalue to a Note.
//			if ((pPropNote->m_dwUndetermined & UD_PLAYMODE) && !(m_dwUndetermined & UD_PLAYMODE) &&
//				(m_bPlayMode == DMUS_PLAYMODE_FIXED))
//			{
//				m_wMusicvalue = pPartRef->MusicValueToNote( pPropNote->m_wMusicvalue, DMUS_PLAYMODE_FIXEDTOSCALE );
//			}
//			// Otherwise, just copy it.
//			else
//			{
//				m_wMusicvalue = pPropNote->m_wMusicvalue;
//			}
//			m_dwUndetermined &= ~UD_MUSICVALUE;
		}
		if ( pPropNote->m_dwChanged & UD_MIDIVALUE )
		{
			// If the MIDIValue values are different or our MIDIValue is Undefined, update it
			if ( (m_bMIDIValue != pPropNote->m_bMIDIValue) || (m_dwUndetermined & UD_MIDIVALUE) )
			{
				dwChanged |= UD_MIDIVALUE;
				m_bMIDIValue = pPropNote->m_bMIDIValue;
				m_dwUndetermined &= ~UD_MIDIVALUE;
			}
		}
		if ( pPropNote->m_dwChanged & UD_SCALEVALUE )
		{
			// If the Scale values are different or our scale value is Undefined, update it
			if ( (m_bScaleValue != pPropNote->m_bScaleValue) || (m_dwUndetermined & UD_SCALEVALUE) )
			{
				dwChanged |= UD_SCALEVALUE;
				m_bScaleValue = pPropNote->m_bScaleValue;
				m_dwUndetermined &= ~UD_SCALEVALUE;
			}
		}
		if ( pPropNote->m_dwChanged & UD_ACCIDENTAL )
		{
			// If the accidental values are different or our accidental is Undefined, update it
			if ( (m_cAccidental != pPropNote->m_cAccidental) || (m_dwUndetermined & UD_ACCIDENTAL) )
			{
				dwChanged |= UD_ACCIDENTAL;
				m_cAccidental = pPropNote->m_cAccidental;
				m_dwUndetermined &= ~UD_ACCIDENTAL;
			}
		}
		if ( pPropNote->m_dwChanged & UD_DIATONICOFFSET )
		{
			// If the diatonic offset values are different or our diatonic offset is Undefined, update it
			if ( (m_cDiatonicOffset != pPropNote->m_cDiatonicOffset) || (m_dwUndetermined & UD_DIATONICOFFSET) )
			{
				dwChanged |= UD_DIATONICOFFSET;
				m_cDiatonicOffset = pPropNote->m_cDiatonicOffset;
				m_dwUndetermined &= ~UD_DIATONICOFFSET;
			}
		}
		if ( pPropNote->m_dwChanged & UD_OCTAVE )
		{
			// If the octave values are different or our octave is Undefined, update it
			if ( (m_bOctave != pPropNote->m_bOctave) || (m_dwUndetermined & UD_OCTAVE) )
			{
				dwChanged |= UD_OCTAVE;
				m_bOctave = pPropNote->m_bOctave;
				m_dwUndetermined &= ~UD_OCTAVE;
			}
		}
		if ( pPropNote->m_dwChanged & UD_TIMERANGE )
		{
			// If the time range values are different or our time range is Undefined, update it
			if ( (m_bTimeRange != pPropNote->m_bTimeRange) || (m_dwUndetermined & UD_TIMERANGE) )
			{
				dwChanged |= UD_TIMERANGE;
				m_bTimeRange = pPropNote->m_bTimeRange;
				m_dwUndetermined &= ~UD_TIMERANGE;
			}
		}
		if ( pPropNote->m_dwChanged & UD_DURRANGE )
		{
			// If the duration range values are different or our duration range is Undefined, update it
			if ( (m_bDurRange != pPropNote->m_bDurRange) || (m_dwUndetermined & UD_DURRANGE) )
			{
				dwChanged |= UD_DURRANGE;
				m_bDurRange = pPropNote->m_bDurRange;
				m_dwUndetermined &= ~UD_DURRANGE;
			}
		}
		if ( pPropNote->m_dwChanged & UD_VELRANGE )
		{
			// If the velocity range values are different or our velocity range is Undefined, update it
			if ( (m_bVelRange != pPropNote->m_bVelRange) || (m_dwUndetermined & UD_VELRANGE) )
			{
				dwChanged |= UD_VELRANGE;
				m_bVelRange = pPropNote->m_bVelRange;
				m_dwUndetermined &= ~UD_VELRANGE;
			}
		}

		if ( pPropNote->m_dwChanged & UD_INVERSIONID )
		{
			// If the inversion ID values are different or our inversion ID is Undefined, update it
			if ( (m_bInversionId != pPropNote->m_bInversionId) || (m_dwUndetermined & UD_INVERSIONID) )
			{
				dwChanged |= UD_INVERSIONID;
				m_bInversionId = pPropNote->m_bInversionId;
				m_dwUndetermined &= ~UD_INVERSIONID;
			}
		}

		if ( pPropNote->m_dwChanged & UD_NOTEFLAGS )
		{
			// If their determined note flags don't match our corresponding note flags
			// or if all of our note flags are undetermined, change our note flags
			if ( (m_dwUndetermined & UD_NOTEFLAGS) || 
				 (m_bNoteFlagsUndetermined != pPropNote->m_bNoteFlagsUndetermined) ||
				 ((pPropNote->m_bNoteFlags & ~pPropNote->m_bNoteFlagsUndetermined) !=
				  (m_bNoteFlags & ~pPropNote->m_bNoteFlagsUndetermined)) )
			{
				dwChanged |= UD_NOTEFLAGS;
				m_bNoteFlags = (m_bNoteFlags & pPropNote->m_bNoteFlagsUndetermined) |
								(pPropNote->m_bNoteFlags & ~pPropNote->m_bNoteFlagsUndetermined);
				m_bNoteFlagsUndetermined &= pPropNote->m_bNoteFlagsUndetermined;
				m_dwUndetermined &= ~UD_NOTEFLAGS;
			}
		}

		if ( pPropNote->m_dwChanged & UD_CHORDBAR )
		{
			// If the chord bar values are different or our chord bar is Undefined, update it
			if (m_lChordBar != pPropNote->m_lChordBar)
			{
				dwChanged |= UD_CHORDBAR;
				m_lChordBar = pPropNote->m_lChordBar;
				m_dwUndetermined &= ~UD_CHORDBAR;
			}
		}
		if ( pPropNote->m_dwChanged & UD_CHORDBEAT )
		{
			// If the chord beat values are different or our chord beat is Undefined, update it
			if (m_lChordBeat != pPropNote->m_lChordBeat)
			{
				// Ensure the ChordBeat and ChordBar don't go before bar 1, beat 1
				if( (pPropNote->m_lChordBeat >= 1) || (m_lChordBar > 1) )
				{
					dwChanged |= UD_CHORDBEAT;
					m_lChordBeat = pPropNote->m_lChordBeat;
					m_dwUndetermined &= ~UD_CHORDBEAT;
				}
			}
		}
		// If either the chord bar or chord beat changed, update the grid start time
		if( dwChanged & (UD_CHORDBAR|UD_CHORDBEAT) )
		// If at least one of the chord bar or chord beat are defined, update the grid start time
		//if ((pPropNote->m_dwUndetermined & (UD_CHORDBAR|UD_CHORDBEAT)) != (UD_CHORDBAR|UD_CHORDBEAT))
		{
			BarBeatToGridOffset();
		}
	}

	m_dwChanged |= dwChanged;
	return dwChanged;
}

DWORD CPropNote::ApplyToDMNote( CDirectMusicStyleNote *pDMNote, const CDirectMusicPartRef* pPartRef) const
{
	//DMNoteToValue(pDMNote)
	ASSERT( pDMNote != NULL );
	if (pDMNote == NULL)
	{
		return 0;
	}

	DWORD dwChanged = 0;
	if (m_fOffset)
	{
		ASSERT(FALSE);
		/*
		if ( !(m_dwUndetermined & UD_OFFSET) )
		{
			// Ensure the offset doesn't go out of range
			if( ( (m_nOffset > 0) && (SHRT_MAX - pDMNote->m_nTimeOffset <= m_nOffset) )
			||  ( (m_nOffset < 0) && (SHRT_MIN - pDMNote->m_nTimeOffset <= m_nOffset) ) )
			{
				pDMNote->m_nTimeOffset += m_nOffset;
				dwChanged |= UD_OFFSET;
			}
#ifdef _DEBUG
			else
			{
				TRACE("CPropNote::ApplyToDMNote Offset out of range.\n");
			}
#endif
		}
		if ( !(m_dwUndetermined & UD_GRIDSTART) )
		{
			// Ensure the grid start doesn't go out of range
			if( ( (m_mtGridStart > 0) && (SHRT_MAX - pDMNote->m_mtGridStart >= m_mtGridStart) )
			||	( (m_mtGridStart < 0) && (pDMNote->m_mtGridStart >= -m_mtGridStart) ) )
			{
				pDMNote->m_mtGridStart += m_mtGridStart;
				dwChanged |= UD_GRIDSTART;
			}
#ifdef _DEBUG
			else
			{
				TRACE("CPropNote::ApplyToDMNote Grid start out of range.\n");
			}
#endif
		}
		// Here, m_bMIDIValue is actually an offset for the MIDI value of the note
		if (!(m_dwUndetermined & UD_MIDIVALUE) )
		{
			// Need pPartRef to convert MusicValue <-> MIDI value
			ASSERT( pPartRef != NULL );
			if (pPartRef == NULL )
			{
				return dwChanged;
			}

			BYTE bValue;
			bValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
			bValue += (signed int)m_bMIDIValue;
			if ( bValue < 128 )
			// Ensure the MIDI value doesn't go out of range
			//if( (((signed int)m_bMIDIValue > 0) && (m_bMIDIValue <= 127 - bValue)) ||
			//	(((signed int)m_bMIDIValue < 0) && (bValue >= -(signed int)m_bMIDIValue)) )
			{
			//	bValue += (signed int)m_bMIDIValue;
				pDMNote->m_bMIDIValue = bValue;
				WORD wMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );
				if ( pDMNote->m_wMusicValue != wMusicValue )
				{
					dwChanged |= UD_MUSICVALUE;
					pDMNote->m_wMusicValue = wMusicValue;
				}
				// The DMNoteToMusicValue conversion may not have been exact
				pDMNote->m_bMIDIValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
			}
		}
		*/
	}
	else
	{
		// Not doing an offset
		if ( m_dwChanged & UD_VARIATIONS )
		{
			if ( (pDMNote->m_dwVariation & ~m_dwVarUndetermined) !=
				 (m_dwVariation & ~m_dwVarUndetermined) )
			{
				DWORD dwNewVariation = (pDMNote->m_dwVariation & m_dwVarUndetermined) |
										 (m_dwVariation & ~m_dwVarUndetermined);

				dwChanged |= UD_VARIATIONS;
				if( dwNewVariation != 0 )
				{
					pDMNote->m_dwVariation = dwNewVariation;
				}
			}
		}

		if ( m_dwChanged & UD_OFFSET )
		{
			// The offset and grid start are always set together, in BarBeatToGridOffset
			ASSERT( m_dwChanged & UD_GRIDSTART );
			if( (pDMNote->m_mtGridStart / m_ts.m_wGridsPerBeat) != (m_mtGridStart / m_ts.m_wGridsPerBeat) )
			{
				// We changed the beat the grid belongs to, so we're changing the chord time

				// All we have to do here is update the values - the real work was done in BarBeatToGridOffset
				if( pDMNote->m_mtGridStart != m_mtGridStart )
				{
					pDMNote->m_mtGridStart = m_mtGridStart;
					dwChanged |= UD_GRIDSTART;
				}

				if( pDMNote->m_nTimeOffset != m_nOffset )
				{
					pDMNote->m_nTimeOffset = m_nOffset;
					dwChanged |= UD_OFFSET;
				}
			}
			else
			{
				// We didn't change the beats the grid belongs to, so we're changing the start time
				if (pDMNote->m_nTimeOffset != m_nOffset)
				{
					// Compute the offset, in number of grids
					long lGridClocks = (DM_PPQNx4 / m_ts.m_bBeat) / m_ts.m_wGridsPerBeat;
					long lDelta = m_nOffset - pDMNote->m_nTimeOffset;
					lDelta /= lGridClocks;

					if( lDelta != 0 )
					{
						// If we moved by at least one grid, ensure the grid values don't go out of bounds
						if( lDelta + pDMNote->m_mtGridStart < 0 )
						{
							lDelta = -pDMNote->m_mtGridStart;
						}
						else if( lDelta + pDMNote->m_mtGridStart >= m_pDMPart->GetGridLength() )
						{
							lDelta = m_pDMPart->GetGridLength() - 1 - pDMNote->m_mtGridStart;
						}

						// Now, update the grid time and the offset time.  This keeps the
						// offset between the start time and the chord time constant.
						pDMNote->m_mtGridStart += lDelta;
						pDMNote->m_nTimeOffset = m_nOffset - lDelta * lGridClocks;
						dwChanged |= UD_OFFSET | UD_GRIDSTART;
					}
					else
					{
						// This will cause the offset between the start time and the chord time
						// to change.
						dwChanged |= UD_OFFSET;
						pDMNote->m_nTimeOffset = m_nOffset;
					}
				}
				else //if( m_dwChanged & UD_GRIDSTART ) - already checked via ASSERT
				{
					if (pDMNote->m_mtGridStart != m_mtGridStart)
					{
						dwChanged |= UD_GRIDSTART;
						pDMNote->m_mtGridStart = m_mtGridStart;
					}
				}
			}
		}
		else if ( m_dwChanged & UD_GRIDSTART )
		{
			// I don't think this case is ever called
			ASSERT( FALSE );
			// TODO: Remove this commented out code
			/*
			if (pDMNote->m_mtGridStart != m_mtGridStart)
			{
				dwChanged |= UD_GRIDSTART;
				pDMNote->m_mtGridStart = m_mtGridStart;
			}
			*/
		}
		if ( m_dwChanged & UD_VELOCITY )
		{
			if (pDMNote->m_bVelocity != m_bVelocity)
				dwChanged |= UD_VELOCITY;
			pDMNote->m_bVelocity	= m_bVelocity;
		}
		if ( m_dwChanged & UD_PLAYMODE )
		{
			if (pDMNote->m_bPlayModeFlags != m_bPlayMode)
			{
				pDMNote->m_bPlayModeFlags = m_bPlayMode;
				dwChanged |= UD_PLAYMODE;

				// If we're not setting the Musicvalue later, change the note's
				// musicvalue if we change from fixed -> melodic or back
				if ( !(m_dwChanged & UD_MUSICVALUE))
				{
					// This uses pDMNote->m_bMIDIValue to calculate a new wMusicValue
					WORD wNewMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );
					if (pDMNote->m_wMusicValue != wNewMusicValue)
					{
						pDMNote->m_wMusicValue = wNewMusicValue;
						dwChanged |= UD_MUSICVALUE;
						// Clear the diatonic offset field
						pDMNote->m_cDiatonicOffset = 0;
					}
					BYTE bNewMIDIValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
					if( pDMNote->m_bMIDIValue != bNewMIDIValue )
					{
						pDMNote->m_bMIDIValue = bNewMIDIValue;
						dwChanged |= UD_MIDIVALUE;
					}
				}
			}
		}

		// If changing MIDI Value (and possibly octave as well)
		if ( m_dwChanged & UD_MIDIVALUE )
		{
			// MIDIValue is 0-11
			BYTE bNewMIDIValue;
			if ( m_dwChanged & UD_OCTAVE )
			{
				// If changing Octave and MIDIValue
				bNewMIDIValue = BYTE(m_bOctave * 12 + m_bMIDIValue);
			}
			else
			{
				// If only changing MIDIValue
				ASSERT( pDMNote->m_bMIDIValue == pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE ) );
				bNewMIDIValue = pDMNote->m_bMIDIValue / 12;
				bNewMIDIValue = BYTE(bNewMIDIValue * 12 + m_bMIDIValue);
			}

			// Ensure new MIDI value is within playable range
			if( bNewMIDIValue > 127 )
			{
				bNewMIDIValue = 127;
			}

			if( bNewMIDIValue != pDMNote->m_bMIDIValue )
			{
				BYTE bOldMIDIValue = pDMNote->m_bMIDIValue;
				WORD wOldMusicValue = pDMNote->m_wMusicValue;

				pDMNote->m_bMIDIValue = bNewMIDIValue;
				pDMNote->m_wMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );
				pDMNote->m_bMIDIValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );

				if( pDMNote->m_wMusicValue != wOldMusicValue )
				{
					dwChanged |= UD_MUSICVALUE;
					// Clear the diatonic offset field
					pDMNote->m_cDiatonicOffset = 0;
				}
				if( pDMNote->m_bMIDIValue != bOldMIDIValue )
				{
					dwChanged |= UD_MIDIVALUE;
				}
			}
		}
		// If only changing Octave
		else if ( m_dwChanged & UD_OCTAVE )
		{
			BYTE bPlayMode = GetDMNotePlaymode( pDMNote, pPartRef );
			ASSERT( m_bOctave < 11 );

			// Figure out if we should write a musical or fixed value
			if ( bPlayMode == DMUS_PLAYMODE_FIXED )
			{
				// Dest is fixed
				BYTE bNewValue = (pDMNote->m_wMusicValue % 12) + m_bOctave * 12;

				// Ensure new MIDI value is within playable range
				if( bNewValue > 127 )
				{
					bNewValue = 127;
				}

				if ( pDMNote->m_wMusicValue != bNewValue )
				{
					pDMNote->m_wMusicValue = bNewValue;
					pDMNote->m_bMIDIValue = bNewValue;
					dwChanged |= UD_MUSICVALUE;
					// Clear the diatonic offset field
					pDMNote->m_cDiatonicOffset = 0;
				}
			}
			else
			{
				// Dest is music
				ASSERT( pPartRef != NULL );
				if (pPartRef == NULL )
				{
					return dwChanged;
				}

				ASSERT( pDMNote->m_bMIDIValue == pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE ) );

				BYTE bNewMIDIValue = BYTE(pDMNote->m_bMIDIValue % 12 + m_bOctave * 12);
				if( bNewMIDIValue > 127 )
				{
					bNewMIDIValue = 127;
				}

				if( bNewMIDIValue != pDMNote->m_bMIDIValue )
				{
					BYTE bOldMIDIValue = pDMNote->m_bMIDIValue;
					WORD wOldMusicValue = pDMNote->m_wMusicValue;

					pDMNote->m_bMIDIValue = bNewMIDIValue;
					pDMNote->m_wMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );
					pDMNote->m_bMIDIValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );

					if( pDMNote->m_wMusicValue != wOldMusicValue )
					{
						dwChanged |= UD_MUSICVALUE;
						// Clear the diatonic offset field
						pDMNote->m_cDiatonicOffset = 0;
					}
					if( pDMNote->m_bMIDIValue != bOldMIDIValue )
					{
						dwChanged |= UD_MIDIVALUE;
					}
				}

				/*
				WORD wNewMusicValue;
				ASSERT( m_bOctave < 11 );
				// BUGBUG: For some unknown reason, the octave value is actually one greater than
				// expected.
				wNewMusicValue = (pDMNote->m_wMusicValue & 0x0FFF) | (((m_bOctave + 1) & 0xF) << 12);

				WORD wOrigMusicValue = pDMNote->m_wMusicValue;

				pDMNote->m_wMusicValue = wNewMusicValue;
				BYTE bNewValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
				pDMNote->m_wMusicValue = wOrigMusicValue;

				if ((pDMNote->m_wMusicValue != wNewMusicValue) &&
					(bNewValue <= 127) && (bNewValue >= 0))
				{
					pDMNote->m_bMIDIValue = bNewValue;
					pDMNote->m_wMusicValue = wNewMusicValue;
					dwChanged |= UD_MUSICVALUE;
				}
				*/
			}
		}
		// If our playmode is not DMUS_PLAYMODE_FIXED, then our MusicValue is in music format
		// If our playmode is DMUS_PLAYMODE_FIXED, then our MusicValue is the MIDI note
		if ( m_dwChanged & UD_MUSICVALUE )
		{
			// Should not happen!  jdooley
			ASSERT( 0 );

			/*
			// If our playmode is undefined, assume Musicvalue is in music format
			BOOL fSourceFixed = (m_dwUndetermined & UD_PLAYMODE) ? FALSE : (m_bPlayMode == DMUS_PLAYMODE_FIXED);

			// Figure out if we should write a musical or fixed value
			BOOL fNoteFixed = IsDMNoteFixed( pDMNote, pPartRef );

			if (fSourceFixed != fNoteFixed)
			{
				if (fNoteFixed)
				{
					// Source is music, dest is fixed
					ASSERT( pPartRef != NULL );
					if (pPartRef == NULL )
					{
						return dwChanged;
					}
					
					WORD wOrigMusicValue = pDMNote->m_wMusicValue;

					pDMNote->m_wMusicValue = m_wMusicvalue;
					WORD wNewMusicValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
					pDMNote->m_wMusicValue = wOrigMusicValue;

					if ((pDMNote->m_wMusicValue != wNewMusicValue) &&
						(wNewMusicValue <= 127) && (wNewMusicValue >= 0))
					{
						pDMNote->m_wMusicValue = wNewMusicValue;
						dwChanged |= UD_MUSICVALUE;
					}
				}
				else
				{
					// Source is fixed, dest is music
					ASSERT( pPartRef != NULL );
					if (pPartRef == NULL )
					{
						return dwChanged;
					}
					ASSERT( (m_wMusicvalue >=0) && (m_wMusicvalue <= 127) );
					pDMNote->m_bMIDIValue = (BYTE)m_wMusicvalue;

					WORD wNewMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
					if ((pDMNote->m_wMusicValue != wNewMusicValue) &&
						(m_wMusicvalue <= 127) && (m_wMusicvalue >= 0))
					{
						pDMNote->m_wMusicValue = wNewMusicValue;
						dwChanged |= UD_MUSICVALUE;
					}
				}
			}
			else // just copy the MusicValue, since source and dest are same type
			{
				// If both are fixed
				if (fSourceFixed)
				{
					if ((pDMNote->m_wMusicValue != m_wMusicvalue) &&
						(m_wMusicvalue <= 127) && (m_wMusicvalue >= 0))
					{
						pDMNote->m_wMusicValue = m_wMusicvalue;
						dwChanged |= UD_MUSICVALUE;
					}
				}
				// Both are MusicValue
				else
				{
					
					WORD wOrigMusicValue = pDMNote->m_wMusicValue;

					pDMNote->m_wMusicValue = m_wMusicvalue;
					WORD wNewMusicValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
					pDMNote->m_wMusicValue = wOrigMusicValue;

					if ((pDMNote->m_wMusicValue != m_wMusicvalue) &&
						(wNewMusicValue <= 127) && (wNewMusicValue >= 0))
					{
						pDMNote->m_wMusicValue = m_wMusicvalue;
						dwChanged |= UD_MUSICVALUE;
					}
				}
			}
			*/
		}
		if ( m_dwChanged & UD_SCALEVALUE )
		{
			ASSERT( pPartRef != NULL );
			if (pPartRef == NULL )
			{
				return dwChanged;
			}
			WORD wNewMusicValue;
			signed char cAccidental;
			signed char cDiatonicOffset;
			BYTE bPlayMode = GetDMNotePlaymode( pDMNote, pPartRef );
			// Figure out if we should write a musical or fixed value
			if ( (bPlayMode == DMUS_PLAYMODE_FIXED) ||
				 (bPlayMode == DMUS_PLAYMODE_FIXEDTOKEY) ||
				 (bPlayMode == DMUS_PLAYMODE_FIXEDTOCHORD) )
			{
				// Dest is fixed
				// Convert from MIDI to MusicValue
				// Assume m_bMIDIValue is correct
				ASSERT( pDMNote->m_bMIDIValue == pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE ) );
				wNewMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );

				// Parse out the accidental and diatonic offset
				MusicValueAndDiatonicOffsetToLogicalScaleAccidentalAndDiatonicOffset( wNewMusicValue, pDMNote->m_cDiatonicOffset, 0, &cAccidental, &cDiatonicOffset );

				// Update the music value with the new scale value
				LogicalScaleAccidentalAndDiatonicOffsetToMusicValueAndDiatonicOffset( m_bScaleValue, cAccidental, cDiatonicOffset, (wNewMusicValue & 0xF000) >> 12, &wNewMusicValue, &cDiatonicOffset );

				// Convert from MusicValue back to MIDI
				WORD wOrigMusicValue = pDMNote->m_wMusicValue;
				BYTE bOrigMIDIValue = pDMNote->m_bMIDIValue;

				pDMNote->m_wMusicValue = wNewMusicValue;
				pDMNote->m_bMIDIValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
				wNewMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );

				if ((wOrigMusicValue != wNewMusicValue) &&
					(pDMNote->m_bMIDIValue <= 127) )
				{
					// pDMNote->m_bMIDIValue is already set
					pDMNote->m_wMusicValue = wNewMusicValue;
					pDMNote->m_cDiatonicOffset = cDiatonicOffset;
					dwChanged |= UD_MUSICVALUE | UD_NOTEFLAGS;
				}
				else
				{
					pDMNote->m_wMusicValue = wOrigMusicValue;
					pDMNote->m_bMIDIValue = bOrigMIDIValue;
				}
			}
			else
			{
				// Dest is music

				// Parse out the accidental and diatonic offset
				MusicValueAndDiatonicOffsetToLogicalScaleAccidentalAndDiatonicOffset( pDMNote->m_wMusicValue, pDMNote->m_cDiatonicOffset, 0, &cAccidental, &cDiatonicOffset );

				// Update the music value with the new scale value
				LogicalScaleAccidentalAndDiatonicOffsetToMusicValueAndDiatonicOffset( m_bScaleValue, cAccidental, m_cDiatonicOffset, (pDMNote->m_wMusicValue & 0xF000) >> 12, &wNewMusicValue, &cDiatonicOffset );
				//wNewMusicValue = LogicalScaleAndAccidentalToMusicValue( m_bScaleValue, cAccidental, (pDMNote->m_wMusicValue & 0xF000) >> 12 );

				// Convert from MusicValue to MIDI (in order to check range)
				WORD wOrigMusicValue = pDMNote->m_wMusicValue;

				pDMNote->m_wMusicValue = wNewMusicValue;
				BYTE bNewValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
				pDMNote->m_wMusicValue = wOrigMusicValue;

				if (((pDMNote->m_cDiatonicOffset != cDiatonicOffset) ||
					 (pDMNote->m_wMusicValue != wNewMusicValue)) &&
					(bNewValue <= 127) )
				{
					pDMNote->m_bMIDIValue = bNewValue;
					pDMNote->m_wMusicValue = wNewMusicValue;
					pDMNote->m_cDiatonicOffset = cDiatonicOffset;
					dwChanged |= UD_MUSICVALUE | UD_NOTEFLAGS;
				}
			}
		}
		if ( m_dwChanged & UD_ACCIDENTAL )
		{
			ASSERT( pPartRef != NULL );
			if (pPartRef == NULL )
			{
				return dwChanged;
			}
			WORD wNewMusicValue;
			signed char cDiatonicOffset;
			BYTE bScaleValue;
			BYTE bPlayMode = GetDMNotePlaymode( pDMNote, pPartRef );
			// Figure out if we should write a musical or fixed value
			if ( (bPlayMode == DMUS_PLAYMODE_FIXED) ||
				 (bPlayMode == DMUS_PLAYMODE_FIXEDTOKEY) ||
				 (bPlayMode == DMUS_PLAYMODE_FIXEDTOCHORD) )
			{
				// Dest is fixed
				// Convert from MIDI to MusicValue
				// Assume pDMNote->m_bMIDIValue is already set
				ASSERT( pDMNote->m_bMIDIValue == pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE ) );
				wNewMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );

				// Parse out the scale position and diatonic offset
				// Don't care about accidental
				MusicValueAndDiatonicOffsetToLogicalScaleAccidentalAndDiatonicOffset( wNewMusicValue, pDMNote->m_cDiatonicOffset, &bScaleValue, NULL, &cDiatonicOffset );

				// Update the music value with the new accidental
				LogicalScaleAccidentalAndDiatonicOffsetToMusicValueAndDiatonicOffset( bScaleValue, m_cAccidental, cDiatonicOffset, (wNewMusicValue & 0xF000) >> 12, &wNewMusicValue, &cDiatonicOffset );

				// Convert from MusicValue back to MIDI
				WORD wOrigMusicValue = pDMNote->m_wMusicValue;
				BYTE bOrigMIDIValue = pDMNote->m_bMIDIValue;

				pDMNote->m_wMusicValue = wNewMusicValue;
				pDMNote->m_bMIDIValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
				wNewMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );;

				if ((wOrigMusicValue != wNewMusicValue) &&
					(pDMNote->m_bMIDIValue <= 127) )
				{
					// pDMNote->m_bMIDIValue is already set
					pDMNote->m_wMusicValue = wNewMusicValue;
					pDMNote->m_cDiatonicOffset = cDiatonicOffset;
					dwChanged |= UD_MUSICVALUE | UD_NOTEFLAGS;
				}
				else
				{
					pDMNote->m_wMusicValue = wOrigMusicValue;
					pDMNote->m_bMIDIValue = bOrigMIDIValue;
				}
			}
			else
			{
				// Dest is music
				// Parse out the scale position and diatonic offset
				// Don't care about accidental
				MusicValueAndDiatonicOffsetToLogicalScaleAccidentalAndDiatonicOffset( pDMNote->m_wMusicValue, pDMNote->m_cDiatonicOffset, &bScaleValue, NULL, &cDiatonicOffset );

				// Update the music value with the new accidental
				LogicalScaleAccidentalAndDiatonicOffsetToMusicValueAndDiatonicOffset( bScaleValue, m_cAccidental, cDiatonicOffset, (pDMNote->m_wMusicValue & 0xF000) >> 12, &wNewMusicValue, &cDiatonicOffset );
				
				// Convert from MusicValue to MIDI (in order to check range)
				WORD wOrigMusicValue = pDMNote->m_wMusicValue;

				pDMNote->m_wMusicValue = wNewMusicValue;
				BYTE bNewValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
				pDMNote->m_wMusicValue = wOrigMusicValue;

				if ((pDMNote->m_wMusicValue != wNewMusicValue) &&
					(bNewValue <= 127) )
				{
					pDMNote->m_bMIDIValue = bNewValue;
					pDMNote->m_wMusicValue = wNewMusicValue;
					pDMNote->m_cDiatonicOffset = cDiatonicOffset;
					dwChanged |= UD_MUSICVALUE | UD_NOTEFLAGS;
				}
			}
		}
		if ( m_dwChanged & UD_DIATONICOFFSET )
		{
			ASSERT( pPartRef != NULL );
			if (pPartRef == NULL )
			{
				return dwChanged;
			}
			WORD wNewMusicValue;
			signed char cAccidental;
			signed char cNewDiatonicOffset;
			BYTE bScaleValue;
			BYTE bPlayMode = GetDMNotePlaymode( pDMNote, pPartRef );
			// Figure out if we should write a musical or fixed value
			if ( (bPlayMode == DMUS_PLAYMODE_FIXED) ||
				 (bPlayMode == DMUS_PLAYMODE_FIXEDTOKEY) ||
				 (bPlayMode == DMUS_PLAYMODE_FIXEDTOCHORD) )
			{
				// Dest is fixed
				// Convert from MIDI to MusicValue
				// Assume pDMNote->m_bMIDIValue is already set
				ASSERT( pDMNote->m_bMIDIValue == pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE ) );
				wNewMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );

				// Parse out the scale position and accidental
				// Don't care about diatonic offset
				MusicValueAndDiatonicOffsetToLogicalScaleAccidentalAndDiatonicOffset( wNewMusicValue, pDMNote->m_cDiatonicOffset, &bScaleValue, &cAccidental, NULL );

				// Update the music value with the new accidental
				LogicalScaleAccidentalAndDiatonicOffsetToMusicValueAndDiatonicOffset( bScaleValue, cAccidental, m_cDiatonicOffset, (wNewMusicValue & 0xF000) >> 12, &wNewMusicValue, &cNewDiatonicOffset );

				// Convert from MusicValue back to MIDI
				WORD wOrigMusicValue = pDMNote->m_wMusicValue;
				BYTE bOrigMIDIValue = pDMNote->m_bMIDIValue;

				pDMNote->m_wMusicValue = wNewMusicValue;
				pDMNote->m_bMIDIValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
				wNewMusicValue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_NONE );;

				if ((wOrigMusicValue != wNewMusicValue) &&
					(pDMNote->m_bMIDIValue <= 127) )
				{
					// pDMNote->m_bMIDIValue is already set
					pDMNote->m_wMusicValue = wNewMusicValue;
					pDMNote->m_cDiatonicOffset = cNewDiatonicOffset;
					dwChanged |= UD_MUSICVALUE | UD_NOTEFLAGS;
				}
				else
				{
					pDMNote->m_wMusicValue = wOrigMusicValue;
					pDMNote->m_bMIDIValue = bOrigMIDIValue;
				}
			}
			else
			{
				// Dest is music
				// Parse out the scale position and accidental
				// Don't care about diatonic offset
				MusicValueAndDiatonicOffsetToLogicalScaleAccidentalAndDiatonicOffset( pDMNote->m_wMusicValue, pDMNote->m_cDiatonicOffset, &bScaleValue, &cAccidental, NULL );

				// Update the music value with the new accidental
				LogicalScaleAccidentalAndDiatonicOffsetToMusicValueAndDiatonicOffset( bScaleValue, cAccidental, m_cDiatonicOffset, (pDMNote->m_wMusicValue & 0xF000) >> 12, &wNewMusicValue, &cNewDiatonicOffset );
				
				// Convert from MusicValue to MIDI (in order to check range)
				WORD wOrigMusicValue = pDMNote->m_wMusicValue;

				pDMNote->m_wMusicValue = wNewMusicValue;
				BYTE bNewValue = pPartRef->DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
				pDMNote->m_wMusicValue = wOrigMusicValue;

				if (((pDMNote->m_cDiatonicOffset != cNewDiatonicOffset) ||
					 (pDMNote->m_wMusicValue != wNewMusicValue)) &&
					(bNewValue <= 127) )
				{
					pDMNote->m_bMIDIValue = bNewValue;
					pDMNote->m_wMusicValue = wNewMusicValue;
					pDMNote->m_cDiatonicOffset = cNewDiatonicOffset;
					dwChanged |= UD_MUSICVALUE | UD_NOTEFLAGS;
				}
			}
		}
		if ( m_dwChanged & UD_DURATION )
		{
			if (pDMNote->m_mtDuration != m_mtDuration)
			{
				dwChanged |= UD_DURATION;
				pDMNote->m_mtDuration	= m_mtDuration;
			}
		}
		if ( m_dwChanged & UD_TIMERANGE )
		{
			if (pDMNote->m_bTimeRange != m_bTimeRange)
			{
				dwChanged |= UD_TIMERANGE;
				pDMNote->m_bTimeRange	= m_bTimeRange;
			}
		}
		if ( m_dwChanged & UD_DURRANGE )
		{
			if (pDMNote->m_bDurRange != m_bDurRange)
			{
				dwChanged |= UD_DURRANGE;
				pDMNote->m_bDurRange	= m_bDurRange;
			}
		}
		if ( m_dwChanged & UD_VELRANGE )
		{
			if (pDMNote->m_bVelRange != m_bVelRange)
			{
				dwChanged |= UD_VELRANGE;
				pDMNote->m_bVelRange	= m_bVelRange;
			}
		}
		if ( m_dwChanged & UD_INVERSIONID )
		{
			if (pDMNote->m_bInversionId != m_bInversionId)
			{
				dwChanged |= UD_INVERSIONID;
				pDMNote->m_bInversionId	= m_bInversionId;
			}
		}
		if ( m_dwChanged & UD_NOTEFLAGS )
		{
			if ( (pDMNote->m_bNoteFlags & ~m_bNoteFlagsUndetermined) !=
				 (m_bNoteFlags & ~m_bNoteFlagsUndetermined) )
			{
				pDMNote->m_bNoteFlags = (pDMNote->m_bNoteFlags & m_bNoteFlagsUndetermined) |
										 (m_bNoteFlags & ~m_bNoteFlagsUndetermined);

				dwChanged |= UD_NOTEFLAGS;
			}
		}
	}
	return dwChanged;
}

void CPropNote::ImportFromDMNote(const CDirectMusicStyleNote *pDMNote, const CDirectMusicPartRef *pPartRef)
{
	ASSERT( pPartRef != NULL );

	m_nOffset		= pDMNote->m_nTimeOffset;
	m_mtGridStart	= pDMNote->m_mtGridStart;
	m_dwVariation	= pDMNote->m_dwVariation;
	m_mtDuration	= pDMNote->m_mtDuration;

	m_bInversionId	= pDMNote->m_bInversionId;
	m_bNoteFlags	= pDMNote->m_bNoteFlags;
	m_bNoteFlagsUndetermined = 0;
	m_bMIDIValue	= pDMNote->m_bMIDIValue % 12;
	m_bOctave		= pDMNote->m_bMIDIValue / 12;
	MusicValueAndDiatonicOffsetToLogicalScaleAccidentalAndDiatonicOffset( pPartRef->DMNoteToLogicalScaleAccidental( pDMNote ), pDMNote->m_cDiatonicOffset, &m_bScaleValue, &m_cAccidental, &m_cDiatonicOffset );
	//MusicValueToLogicalScaleAndAccidental( pPartRef->DMNoteToLogicalScaleAccidental( pDMNote ), &m_bScaleValue, &m_cAccidental );
	m_bVelocity		= pDMNote->m_bVelocity;

	m_bPlayMode		= pDMNote->m_bPlayModeFlags;

	// Set m_wMusicvalue depending on what our Playmode and the Playmode of the strip is
	// If the note's playmode is DMUS_PLAYMODE_NONE, check what the strip's playmode is
	if (m_bPlayMode == DMUS_PLAYMODE_NONE)
	{
		// If the Part's playmode is DMUS_PLAYMODE_FIXED, convert the value from a note to a musicvalue
		ASSERT( pPartRef->m_pDMPart->m_bPlayModeFlags != DMUS_PLAYMODE_NONE );
		if (pPartRef->m_pDMPart->m_bPlayModeFlags == DMUS_PLAYMODE_FIXED)
		{
			ASSERT( (pDMNote->m_wMusicValue>=0) && (pDMNote->m_wMusicValue<=127) );
			ASSERT( pDMNote->m_wMusicValue == pDMNote->m_bMIDIValue );
//			pDMNote->m_bMIDIValue = (BYTE)pDMNote->m_wMusicValue;
			m_wMusicvalue = pPartRef->DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
		}
		else
		{
			m_wMusicvalue	= pDMNote->m_wMusicValue;
		}
	}
	else
	{
		m_wMusicvalue	= pDMNote->m_wMusicValue;
	}

	m_bTimeRange	= pDMNote->m_bTimeRange;
	m_bDurRange		= pDMNote->m_bDurRange;
	m_bVelRange		= pDMNote->m_bVelRange;

	m_dwUndetermined = ~( UD_OFFSET | UD_GRIDSTART | UD_VARIATIONS | UD_DURATION | UD_OCTAVE | UD_INVERSIONID
						| UD_ACCIDENTAL | UD_DIATONICOFFSET | UD_SCALEVALUE | UD_MIDIVALUE | UD_VELOCITY | UD_MUSICVALUE | UD_PLAYMODE
						| UD_TIMERANGE | UD_DURRANGE | UD_VELRANGE );
	m_dwVarUndetermined =	0x00000000;

	// set the variables for the note property page
	GridOffsetToBarBeat();
}

CPropNote CPropNote::operator +=(const CPropNote propnote)
{
//	m_bEventtype	= propnote.m_bEventtype;
	if (m_nOffset != propnote.m_nOffset)
	{
		m_dwUndetermined |= UD_OFFSET;
	}
	if (m_mtGridStart != propnote.m_mtGridStart)
	{
		m_dwUndetermined |= UD_GRIDSTART;
	}
	m_dwVarUndetermined |= m_dwVariation ^ propnote.m_dwVariation;
	if ( ~m_dwVarUndetermined == 0 )
	{
		m_dwUndetermined |= UD_VARIATIONS;
	}
	m_dwVariation &= propnote.m_dwVariation;
	if ( m_bScaleValue != propnote.m_bScaleValue)
	{
		m_dwUndetermined |= UD_SCALEVALUE;
	}
	if ( m_cAccidental != propnote.m_cAccidental)
	{
		m_dwUndetermined |= UD_ACCIDENTAL;
	}
	if (m_cDiatonicOffset != propnote.m_cDiatonicOffset)
	{
		m_dwUndetermined |= UD_DIATONICOFFSET;
	}
	if (m_bMIDIValue != propnote.m_bMIDIValue)
	{
		m_dwUndetermined |= UD_MIDIVALUE;
	}
	if (m_bOctave != propnote.m_bOctave)
	{
		m_dwUndetermined |= UD_OCTAVE;
	}
	if (m_bVelocity != propnote.m_bVelocity)
	{
		m_dwUndetermined |= UD_VELOCITY;
	}
	if (m_wMusicvalue != propnote.m_wMusicvalue)
	{
		m_dwUndetermined |= UD_MUSICVALUE;
	}
	if (m_mtDuration != propnote.m_mtDuration)
	{
		m_dwUndetermined |= UD_DURATION;
	}
	if (m_bTimeRange != propnote.m_bTimeRange)
	{
		m_dwUndetermined |= UD_TIMERANGE;
	}
	if (m_bDurRange != propnote.m_bDurRange)
	{
		m_dwUndetermined |= UD_DURRANGE;
	}
	if (m_bVelRange != propnote.m_bVelRange)
	{
		m_dwUndetermined |= UD_VELRANGE;
	}
	if (m_bInversionId != propnote.m_bInversionId)
	{
		m_dwUndetermined |= UD_INVERSIONID;
	}
	m_bNoteFlagsUndetermined |= m_bNoteFlags ^ propnote.m_bNoteFlags;
	if ( ~m_bNoteFlagsUndetermined == 0 )
	{
		m_dwUndetermined |= UD_NOTEFLAGS;
	}
	m_bNoteFlags &= propnote.m_bNoteFlags;
	if (m_bPlayMode != propnote.m_bPlayMode)
	{
		m_dwUndetermined |= UD_PLAYMODE;
	}

	if (m_lStartBar != propnote.m_lStartBar)
	{
		m_dwUndetermined |= UD_STARTBAR;
	}
	if (m_lStartBeat != propnote.m_lStartBeat)
	{
		m_dwUndetermined |= UD_STARTBEAT;
	}
	if (m_lStartGrid != propnote.m_lStartGrid)
	{
		m_dwUndetermined |= UD_STARTGRID;
	}
	if (m_lStartTick != propnote.m_lStartTick)
	{
		m_dwUndetermined |= UD_STARTTICK;
	}

	if (m_lEndBar != propnote.m_lEndBar)
	{
		m_dwUndetermined |= UD_ENDBAR;
	}
	if (m_lEndBeat != propnote.m_lEndBeat)
	{
		m_dwUndetermined |= UD_ENDBEAT;
	}
	if (m_lEndGrid != propnote.m_lEndGrid)
	{
		m_dwUndetermined |= UD_ENDGRID;
	}
	if (m_lEndTick != propnote.m_lEndTick)
	{
		m_dwUndetermined |= UD_ENDTICK;
	}

	if (m_lDurBar != propnote.m_lDurBar)
	{
		m_dwUndetermined |= UD_DURBAR;
	}
	if (m_lDurBeat != propnote.m_lDurBeat)
	{
		m_dwUndetermined |= UD_DURBEAT;
	}
	if (m_lDurGrid != propnote.m_lDurGrid)
	{
		m_dwUndetermined |= UD_DURGRID;
	}
	if (m_lDurTick != propnote.m_lDurTick)
	{
		m_dwUndetermined |= UD_DURTICK;
	}

	if (m_lChordBar != propnote.m_lChordBar)
	{
		m_dwUndetermined |= UD_CHORDBAR;
	}
	if (m_lChordBeat != propnote.m_lChordBeat)
	{
		m_dwUndetermined |= UD_CHORDBEAT;
	}

	return *this;
}

/*
CString CPropNote::ToString()
{
	CString str;
	CString space = " ";
	char pchar[20];

//	m_bEventtype	= mnote.eventtype;
//	m_bVoiceid		= mnote.voiceid;
	str = CString(_itoa(m_nOffset, pchar, 10)) + space;
	str += CString(_itoa(m_mtGridStart, pchar, 10)) + space;
	str += CString(_itoa(m_dwVariation, pchar, 16)) + space;
	str += CString(_itoa(m_bVelocity, pchar, 10)) + space;
	str += CString(_itoa(m_wMusicvalue, pchar, 16)) + space;
	str += CString(_itoa(m_bScaleValue, pchar, 10)) + space;
	str += CString(_itoa(m_cAccidental, pchar, 10)) + space;
	str += CString(_itoa(m_cDiatonicOffset, pchar, 10)) + space;
	str += CString(_itoa(m_bOctave, pchar, 10)) + space;
	str += CString(_itoa(m_mtDuration, pchar, 10)) + space;
	str += CString(_itoa(m_bTimeRange, pchar, 10)) + space;
	str += CString(_itoa(m_bDurRange, pchar, 10)) + space;
	str += CString(_itoa(m_bVelRange, pchar, 10)) + space;
	str += CString(_itoa(m_bInversionId, pchar, 10)) + space;
	str += CString(_itoa(m_bNoteFlags, pchar, 10)) + space;
	str += CString(_itoa(m_bPlayMode, pchar, 16)) + space;
	str += CString(_itoa(m_dwUndetermined, pchar, 16)) + space;
	str += CString(_itoa(m_dwVarUndetermined, pchar, 16)) + space;
	str += CString(_itoa(m_lStartBar, pchar, 10)) + space;
	str += CString(_itoa(m_lStartBeat, pchar, 10)) + space;
	str += CString(_itoa(m_lStartGrid, pchar, 10)) + space;
	str += CString(_itoa(m_lStartTick, pchar, 10)) + space;
	str += CString(_itoa(m_lEndBar, pchar, 10)) + space;
	str += CString(_itoa(m_lEndBeat, pchar, 10)) + space;
	str += CString(_itoa(m_lEndGrid, pchar, 10)) + space;
	str += CString(_itoa(m_lEndTick, pchar, 10)) + space;
	str += CString(_itoa(m_lDurBar, pchar, 10)) + space;
	str += CString(_itoa(m_lDurBeat, pchar, 10)) + space;
	str += CString(_itoa(m_lDurGrid, pchar, 10)) + space;
	str += CString(_itoa(m_lDurTick, pchar, 10)) + space;
	str += CString(_itoa(m_lChordBar, pchar, 10)) + space;
	str += CString(_itoa(m_lChordBeat, pchar, 10));
	return str;
}
*/

BYTE CPropNote::GetDMNotePlaymode( CDirectMusicStyleNote *pDMNote, const CDirectMusicPartRef* pPartRef ) const
{
	return (pDMNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE) ?
				pPartRef->m_pDMPart->m_bPlayModeFlags : pDMNote->m_bPlayModeFlags;
}

DWORD CPropItem::ApplyPropItem(const CPropItem *pPropNote)
{
	DWORD dwChanged = 0;

	if ( pPropNote->m_fOffset )
	{
		ASSERT(FALSE);
		/*
		// Ensure that pPropNote and ourself both have a determined offset
		if (!((pPropNote->m_dwUndetermined & UD_OFFSET) || (m_dwUndetermined & UD_OFFSET)) )
		{
			// Ensure the offset doesn't go out of range
			if( ( (m_nOffset > 0) && (SHRT_MAX - pPropNote->m_nOffset <= m_nOffset) )
			||  ( (m_nOffset < 0) && (SHRT_MIN - pPropNote->m_nOffset <= m_nOffset) ) )
			{
				dwChanged |= UD_OFFSET;
				m_nOffset = short(m_nOffset + pPropNote->m_nOffset);
			}
#ifdef _DEBUG
			else
			{
				TRACE("CPropNote::ApplyPropNote Offset out of range.\n");
			}
#endif
		}
		// Ensure that pPropNote and ourself both have a determined grid start
		if (!((pPropNote->m_dwUndetermined & UD_GRIDSTART) || (m_dwUndetermined & UD_GRIDSTART)) )
		{
			// Ensure the grid start doesn't go out of range
			if( ( (m_mtGridStart > 0) && (SHRT_MAX - pPropNote->m_mtGridStart <= m_mtGridStart) )
			||	( (m_mtGridStart < 0) && (pPropNote->m_mtGridStart >= -m_mtGridStart) ) )
			{
				dwChanged |= UD_GRIDSTART;
				m_mtGridStart += pPropNote->m_mtGridStart;
			}
#ifdef _DEBUG
			else
			{
				TRACE("CPropNote::ApplyPropNote Grid out of range.\n");
			}
#endif
		}
		// if something changed and both our offset and grid start are determined,
		// update the Start and End bar/beat/grid/tick
		if ( (dwChanged!=0) && !(m_dwUndetermined & (UD_OFFSET | UD_GRIDSTART)))
		{
			GridOffsetToBarBeat();
		}
		*/
	}
	else if ( !m_fOffset )
	{
		if ( pPropNote->m_dwChanged & UD_VARIATIONS )
		{
			// If their determined variations don't match our corresponding variations
			// or if all of our variation are undetermined, change our variations
			if ( (m_dwUndetermined & UD_VARIATIONS) || 
				 (m_dwVarUndetermined != pPropNote->m_dwVarUndetermined) ||
				 ((pPropNote->m_dwVariation & ~pPropNote->m_dwVarUndetermined) !=
				  (m_dwVariation & ~pPropNote->m_dwVarUndetermined)) )
			{
				dwChanged |= UD_VARIATIONS;
				m_dwVariation = (m_dwVariation & pPropNote->m_dwVarUndetermined) |
								(pPropNote->m_dwVariation & ~pPropNote->m_dwVarUndetermined);
				m_dwVarUndetermined &= pPropNote->m_dwVarUndetermined;
				m_dwUndetermined &= ~UD_VARIATIONS;
			}
		}
		if ( pPropNote->m_dwChanged & UD_OFFSET )
		{
			// If the offset values are different or our offset is Undefined, update it
			if ( (m_nOffset!=pPropNote->m_nOffset) || (m_dwUndetermined & UD_OFFSET) )
			{
				dwChanged |= UD_OFFSET;
				m_nOffset = pPropNote->m_nOffset;
				m_dwUndetermined &= ~UD_OFFSET;
			}
		}
		if ( pPropNote->m_dwChanged & UD_GRIDSTART )
		{
			// If the grid start values are different or our grid start is Undefined, update it
			if ( (m_mtGridStart != pPropNote->m_mtGridStart) || (m_dwUndetermined & UD_GRIDSTART) )
			{
				dwChanged |= UD_GRIDSTART;
				m_mtGridStart = pPropNote->m_mtGridStart;
				m_dwUndetermined &= ~UD_GRIDSTART;
			}
		}
		// if possible, update the Start and End bar/beat/grid/tick
		if ( (dwChanged!=0) && !(m_dwUndetermined & (UD_OFFSET | UD_GRIDSTART)))
		{
			GridOffsetToBarBeat();
		}

		if ( pPropNote->m_dwChanged & UD_DURATION )
		{
			// If the duration values are different or our duration is Undefined, update it
			if ( (m_mtDuration != pPropNote->m_mtDuration) || (m_dwUndetermined & UD_DURATION) )
			{
				dwChanged |= UD_DURATION;
				m_mtDuration = pPropNote->m_mtDuration;
				m_dwUndetermined &= ~UD_DURATION;
				// if possible, update the Duration bar/beat/grid/tick
				GridOffsetToBarBeat();
			}
		}

		if ( pPropNote->m_dwChanged & UD_STARTBAR )
		{
			// If the Start bar values are different
			if (m_lStartBar != pPropNote->m_lStartBar)
			{
				/* Taken care of in BarBeatToGridOffset
				// If Chord bar and Start bar are the same, also update the chord bar
				// But only if the new start bar is at least 1
				if ((pPropNote->m_lStartBar >= 1) && (m_lChordBar == m_lStartBar))
				{
					dwChanged |= UD_CHORDBAR;
					m_lChordBar = pPropNote->m_lStartBar;
					m_dwUndetermined &= ~UD_CHORDBAR;
				}
				*/

				// Now update the start bar
				dwChanged |= UD_STARTBAR;
				m_lStartBar = pPropNote->m_lStartBar;
				m_dwUndetermined &= ~UD_STARTBAR;
			}
		}
		if ( pPropNote->m_dwChanged & UD_STARTBEAT )
		{
			// If the Start beat values are different
			if (m_lStartBeat != pPropNote->m_lStartBeat)
			{
				/* Taken care of in BarBeatToGridOffset
				// If Chord bar and Start bar are the same, and chord beat and start beat
				// are the same, also update the chord beat
				// But only if the new start bar and beat are at least 1
				if ( ( (pPropNote->m_lStartBar > 1) ||
					   (pPropNote->m_lStartBeat >= 1) )
				  && (m_lChordBeat == m_lStartBeat) && (m_lChordBar == m_lStartBar))
				{
					dwChanged |= UD_CHORDBEAT;
					m_lChordBeat = pPropNote->m_lStartBeat;
					m_dwUndetermined &= ~UD_CHORDBEAT;
				}
				*/

				// Now update the start beat
				dwChanged |= UD_STARTBEAT;
				m_lStartBeat = pPropNote->m_lStartBeat;
				m_dwUndetermined &= ~UD_STARTBEAT;
			}
		}
		if ( pPropNote->m_dwChanged & UD_STARTGRID )
		{
			// If the Start grid values are different or our start grid is Undefined, update it
			if (m_lStartGrid != pPropNote->m_lStartGrid)
			{
				dwChanged |= UD_STARTGRID;
				m_lStartGrid = pPropNote->m_lStartGrid;
				m_dwUndetermined &= ~UD_STARTGRID;
			}
		}
		if ( pPropNote->m_dwChanged & UD_STARTTICK )
		{
			// If the Start tick values are different or our start tick is Undefined, update it
			if (m_lStartTick != pPropNote->m_lStartTick)
			{
				dwChanged |= UD_STARTTICK;
				m_lStartTick = pPropNote->m_lStartTick;
				m_dwUndetermined &= ~UD_STARTTICK;
			}
		}
		// If at least one of the start bar/beat/grid/tick changed, convert start to a Grid and Offset
		if( dwChanged & UD_STARTBARBEATGRIDTICK )
		// If at least one of the start bar/beat/grid/tick are defined, convert to a Grid offset
		//if ((pPropNote->m_dwUndetermined & UD_STARTBARBEATGRIDTICK) != UD_STARTBARBEATGRIDTICK)
		{
			BarBeatToGridOffset();
		}
	
		if ( pPropNote->m_dwChanged & UD_ENDBAR )
		{
			// If the end bar values are different or our end bar is Undefined, update it
			if (m_lEndBar != pPropNote->m_lEndBar)
			{
				dwChanged |= UD_ENDBAR;
				m_lEndBar = pPropNote->m_lEndBar;
				m_dwUndetermined &= ~UD_ENDBAR;
			}
		}
		if ( pPropNote->m_dwChanged & UD_ENDBEAT )
		{
			// If the end beat values are different or our end beat is Undefined, update it
			if (m_lEndBeat != pPropNote->m_lEndBeat)
			{
				dwChanged |= UD_ENDBEAT;
				m_lEndBeat = pPropNote->m_lEndBeat;
				m_dwUndetermined &= ~UD_ENDBEAT;
			}
		}
		if ( pPropNote->m_dwChanged & UD_ENDGRID )
		{
			// If the end grid values are different or our end grid is Undefined, update it
			if (m_lEndGrid != pPropNote->m_lEndGrid)
			{
				dwChanged |= UD_ENDGRID;
				m_lEndGrid = pPropNote->m_lEndGrid;
				m_dwUndetermined &= ~UD_ENDGRID;
			}
		}
		if ( pPropNote->m_dwChanged & UD_ENDTICK )
		{
			// If the end tick values are different or our end tick is Undefined, update it
			if (m_lEndTick != pPropNote->m_lEndTick)
			{
				dwChanged |= UD_ENDTICK;
				m_lEndTick = pPropNote->m_lEndTick;
				m_dwUndetermined &= ~UD_ENDTICK;
			}
		}
		// If at least one of the end bar/beat/grid/tick changed, update duration
		if( dwChanged & UD_ENDBARBEATGRIDTICK )
		// If at least one of the end bar/beat/grid/tick are defined, update duration
		//if ((pPropNote->m_dwUndetermined & UD_ENDBARBEATGRIDTICK) != UD_ENDBARBEATGRIDTICK)
		{
			// Update duration
			// only set the duration if all of our data is determined
			if ( ( m_dwUndetermined & (UD_ENDBARBEATGRIDTICK | UD_STARTBARBEATGRIDTICK) ) == 0)
			{
				long lStartTime, lEndTime, lBeatClocks, lMeasureClocks, lGridClocks;
				lBeatClocks = DM_PPQNx4 / m_ts.m_bBeat;
				lMeasureClocks = lBeatClocks * m_ts.m_bBeatsPerMeasure;
				lGridClocks = lBeatClocks / m_ts.m_wGridsPerBeat;
				lStartTime = m_lStartBar * lMeasureClocks + m_lStartBeat * lBeatClocks +
							 m_lStartGrid * lGridClocks + m_lStartTick;
				lEndTime = m_lEndBar * lMeasureClocks + m_lEndBeat * lBeatClocks +
						   m_lEndGrid * lGridClocks + m_lEndTick;
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
				m_dwUndetermined &= ~UD_DURATION;
				dwChanged |= CHGD_DURATION;
			}
		}
		
		if ( pPropNote->m_dwChanged & UD_DURBAR )
		{
			// If the duration bar values are different or our duration bar is Undefined, update it
			if (m_lDurBar != pPropNote->m_lDurBar)
			{
				dwChanged |= UD_DURBAR;
				m_lDurBar = pPropNote->m_lDurBar;
				m_dwUndetermined &= ~UD_DURBAR;
			}
		}
		if ( pPropNote->m_dwChanged & UD_DURBEAT )
		{
			// If the duration beat values are different or our duration beat is Undefined, update it
			if (m_lDurBeat != pPropNote->m_lDurBeat)
			{
				dwChanged |= UD_DURBEAT;
				m_lDurBeat = pPropNote->m_lDurBeat;
				m_dwUndetermined &= ~UD_DURBEAT;
			}
		}
		if ( pPropNote->m_dwChanged & UD_DURGRID )
		{
			// If the duration grid values are different or our duration grid is Undefined, update it
			if (m_lDurGrid != pPropNote->m_lDurGrid)
			{
				dwChanged |= UD_DURGRID;
				m_lDurGrid = pPropNote->m_lDurGrid;
				m_dwUndetermined &= ~UD_DURGRID;
			}
		}
		if ( pPropNote->m_dwChanged & UD_DURTICK )
		{
			// If the duration tick values are different or our duration tick is Undefined, update it
			if (m_lDurTick != pPropNote->m_lDurTick)
			{
				dwChanged |= UD_DURTICK;
				m_lDurTick = pPropNote->m_lDurTick;
				m_dwUndetermined &= ~UD_DURTICK;
			}
		}
		// If at least one of the duration bar/beat/grid/tick changed, update duration
		if( dwChanged & UD_DURBARBEATGRIDTICK )
		// If at least one of the duration bar/beat/grid/tick are defined, update duration
		//if ((pPropNote->m_dwUndetermined & UD_DURBARBEATGRIDTICK) != UD_DURBARBEATGRIDTICK)
		{
			BarBeatToGridOffset();
			dwChanged |= CHGD_DURATION;
		}
	}

	m_dwChanged |= dwChanged;
	return dwChanged;
}

void CPropNote::Copy( const CPropNote *pPropNote )
{
	m_nOffset = pPropNote->m_nOffset;
	m_mtGridStart = pPropNote->m_mtGridStart;
	m_dwVariation = pPropNote->m_dwVariation;
	m_bVelocity = pPropNote->m_bVelocity;
	m_wMusicvalue = pPropNote->m_wMusicvalue;
	m_mtDuration = pPropNote->m_mtDuration;
	m_bTimeRange = pPropNote->m_bTimeRange;
	m_bDurRange = pPropNote->m_bDurRange;
	m_bVelRange = pPropNote->m_bVelRange;
	m_bPlayMode = pPropNote->m_bPlayMode;
	m_bInversionId = pPropNote->m_bInversionId;
	memcpy( m_adwInversionIds, pPropNote->m_adwInversionIds, sizeof(DWORD) * 8);
	m_bNoteFlags = pPropNote->m_bNoteFlags;
	m_bNoteFlagsUndetermined = pPropNote->m_bNoteFlagsUndetermined;

	m_lStartBar = pPropNote->m_lStartBar;
	m_lStartBeat = pPropNote->m_lStartBeat;
	m_lStartGrid = pPropNote->m_lStartGrid;
	m_lStartTick = pPropNote->m_lStartTick;

	m_lEndBar = pPropNote->m_lEndBar;
	m_lEndBeat = pPropNote->m_lEndBeat;
	m_lEndGrid = pPropNote->m_lEndGrid;
	m_lEndTick = pPropNote->m_lEndTick;

	m_lDurBar = pPropNote->m_lDurBar;
	m_lDurBeat = pPropNote->m_lDurBeat;
	m_lDurGrid = pPropNote->m_lDurGrid;
	m_lDurTick = pPropNote->m_lDurTick;

	m_lChordBar = pPropNote->m_lChordBar;
	m_lChordBeat = pPropNote->m_lChordBeat;

	m_bMIDIValue = pPropNote->m_bMIDIValue;
	m_bOctave = pPropNote->m_bOctave;
	m_bScaleValue = pPropNote->m_bScaleValue;
	m_cAccidental = pPropNote->m_cAccidental;
	m_cDiatonicOffset = pPropNote->m_cDiatonicOffset;

	m_dwUndetermined = pPropNote->m_dwUndetermined;
	m_dwChanged = pPropNote->m_dwChanged;
	m_dwVarUndetermined = pPropNote->m_dwVarUndetermined;

	m_fOffset = pPropNote->m_fOffset;

	m_ts = pPropNote->m_ts;
}

#pragma warning( pop )
