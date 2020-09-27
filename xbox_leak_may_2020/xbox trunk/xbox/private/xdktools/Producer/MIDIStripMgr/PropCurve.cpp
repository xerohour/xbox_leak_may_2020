#include "stdafx.h"
#include "PropCurve.h"
#include "MIDI.h"
#include "Pattern.h"
#include "MIDIStripMgr.h"
#include "ioDMStyle.h"
#include "PropNote.h"


CPropCurve::CPropCurve()
{
	Clear();
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::ApplyValuesToDMCurve

DWORD CPropCurve::ApplyValuesToDMCurve( const CDirectMusicPart* pDMPart, CDirectMusicStyleCurve* pDMCurve )
{
	ASSERT( pDMPart != NULL );
	ASSERT( pDMCurve != NULL );

	if( pDMPart == NULL
	||  pDMCurve == NULL )
	{
		return 0;
	}

	long lPartClockLength = pDMPart->GetClockLength();

	DWORD dwChanged = 0;

	// offset values for pitchbends since they center on zero
	if (pDMCurve->m_bEventType == DMUS_CURVET_PBCURVE) {

		m_nStartValue += PB_DISP_OFFSET;
		m_nEndValue += PB_DISP_OFFSET;
		m_lResetValue += PB_DISP_OFFSET;
	}

	// Start Value
	if( m_dwChanged & CHGD_STARTVAL )
	{
		if( pDMCurve->m_nStartValue != m_nStartValue )
		{
			dwChanged |= CHGD_STARTVAL;
			pDMCurve->m_nStartValue	= m_nStartValue;
		}
	}

	// End Value
	if( m_dwChanged & CHGD_ENDVAL )
	{
		if( pDMCurve->m_nEndValue != m_nEndValue )
		{
			dwChanged |= CHGD_ENDVAL;
			pDMCurve->m_nEndValue = m_nEndValue;
		}
	}

	// Reset enable
	if (m_dwChanged & CHGD_RESETENABLE) {

		if( (pDMCurve->m_bFlags & DMUS_CURVE_RESET) != (m_bFlags & DMUS_CURVE_RESET) )
		{
			dwChanged |= CHGD_RESETENABLE;
			pDMCurve->m_bFlags &= ~DMUS_CURVE_RESET;
			pDMCurve->m_bFlags |= m_bFlags & DMUS_CURVE_RESET;
			
			if( pDMCurve->m_bFlags & DMUS_CURVE_RESET )
			{
				pDMCurve->SetDefaultResetValues( lPartClockLength );
				if( !(pDMCurve->m_bFlags & DMUS_CURVE_RESET) )
				{
					pDMCurve->m_bFlags &= ~DMUS_CURVE_RESET;
					pDMCurve->m_bFlags |= m_bFlags & DMUS_CURVE_RESET;
					pDMCurve->m_nResetValue = 0;	
					pDMCurve->m_mtResetDuration = lPartClockLength;
				}
			}
			else
			{
				pDMCurve->m_mtResetDuration = 0;
				pDMCurve->m_nResetValue = 0;
			}
		}
	}

	// Reset Value
	if( m_dwChanged & CHGD_RESETVAL )
	{
		if( pDMCurve->m_nResetValue != m_lResetValue )
		{
			dwChanged |= CHGD_RESETVAL;
			pDMCurve->m_nResetValue = (short)m_lResetValue;
		}
	}

	// Flip Vert button clicked
	if( m_dwChanged & CHGD_FLIPVERT )
	{
		switch( pDMCurve->m_bCurveShape )
		{
			case DMUS_CURVES_SINE:
			case DMUS_CURVES_INSTANT:
			case DMUS_CURVES_LINEAR:
				dwChanged |= CHGD_FLIPVERT;

				short nMaxValue;
				if( pDMCurve->m_bEventType == DMUS_CURVET_PBCURVE )
				{
					nMaxValue = MAX_PB_VALUE;
				}
				else if( (pDMCurve->m_bEventType == DMUS_CURVET_RPNCURVE)
					 ||  (pDMCurve->m_bEventType == DMUS_CURVET_NRPNCURVE) )
				{
					nMaxValue = MAX_RPN_VALUE;
				}
				else
				{
					nMaxValue = MAX_CC_VALUE;
				}
				pDMCurve->m_nStartValue	= (short)(nMaxValue - pDMCurve->m_nStartValue);
				pDMCurve->m_nEndValue = (short)(nMaxValue - pDMCurve->m_nEndValue);
				break;

			case DMUS_CURVES_EXP:
				dwChanged |= CHGD_FLIPVERT;
				pDMCurve->m_bCurveShape = DMUS_CURVES_LOG;
				break;

			case DMUS_CURVES_LOG:
				dwChanged |= CHGD_FLIPVERT;
				pDMCurve->m_bCurveShape = DMUS_CURVES_EXP;
				break;
		}
	}

	// Flip horz button clicked
	if( m_dwChanged & CHGD_FLIPHORZ )
	{
		dwChanged |= CHGD_FLIPHORZ;

		short nStartValue = pDMCurve->m_nStartValue;
		pDMCurve->m_nStartValue	= pDMCurve->m_nEndValue;
		pDMCurve->m_nEndValue = nStartValue;
	}

	// Shape
	if( m_dwChanged & CHGD_SHAPE )
	{
		if( pDMCurve->m_bCurveShape != m_bCurveShape )
		{
			dwChanged |= CHGD_SHAPE;
			pDMCurve->m_bCurveShape	= m_bCurveShape;
		}
	}

	// Start at current
	if (m_dwChanged2 & CHGD2_STARTCURRENT) {

		if( (pDMCurve->m_bFlags & DMUS_CURVE_START_FROM_CURRENT) != (m_bFlags & DMUS_CURVE_START_FROM_CURRENT) )
		{
			dwChanged |= CHGD2_STARTCURRENT;
			pDMCurve->m_bFlags &= ~DMUS_CURVE_START_FROM_CURRENT;
			pDMCurve->m_bFlags |= m_bFlags & DMUS_CURVE_START_FROM_CURRENT;
		}
	}

	// Merge Index
	if( m_dwChanged2 & CHGD2_MERGEINDEX )
	{
		if( pDMCurve->m_wMergeIndex != m_wMergeIndex )
		{
			dwChanged |= CHGD2_MERGEINDEX;
			pDMCurve->m_wMergeIndex = m_wMergeIndex;
		}
	}

	// Variations
	if( m_dwChanged & CHGD_VARIATIONS )
	{
		if( (pDMCurve->m_dwVariation & ~m_dwVarUndetermined) != (m_dwVariation & ~m_dwVarUndetermined) )
		{
			DWORD dwNewVariation = (pDMCurve->m_dwVariation & m_dwVarUndetermined) |
								   (m_dwVariation & ~m_dwVarUndetermined);

			dwChanged |= CHGD_VARIATIONS;		// Set even if dwNewVariation == 0
			if( dwNewVariation != 0 )
			{
				pDMCurve->m_dwVariation = dwNewVariation;
			}
		}
	}

	// Grid Start or Offset
	if( (m_dwChanged & CHGD_START_BARBEATGRIDTICK)
	||  (m_dwChanged & CHGD_END_BARBEATGRIDTICK)
	||  (m_dwChanged & CHGD_DUR_BARBEATGRIDTICK) 
	||  (m_dwChanged & CHGD_RESET_BARBEATGRIDTICK) )
	{
		CPropCurve propcurve;
		propcurve.GetValuesFromDMCurve( pDMPart, pDMCurve );

		if( m_dwChanged & CHGD_STARTBAR ) 
		{
			propcurve.m_lStartBar = m_lStartBar;
		}
		if( m_dwChanged & CHGD_STARTBEAT ) 
		{
			propcurve.m_lStartBeat = m_lStartBeat;
		}
		if( m_dwChanged & CHGD_STARTGRID ) 
		{
			propcurve.m_lStartGrid = m_lStartGrid;
		}
		if( m_dwChanged & CHGD_STARTTICK ) 
		{
			propcurve.m_lStartTick = m_lStartTick;
		}

		if( m_dwChanged & CHGD_ENDBAR ) 
		{
			propcurve.m_lEndBar = m_lEndBar;
		}
		if( m_dwChanged & CHGD_ENDBEAT ) 
		{
			propcurve.m_lEndBeat = m_lEndBeat;
		}
		if( m_dwChanged & CHGD_ENDGRID ) 
		{
			propcurve.m_lEndGrid = m_lEndGrid;
		}
		if( m_dwChanged & CHGD_ENDTICK ) 
		{
			propcurve.m_lEndTick = m_lEndTick;
		}

		if( m_dwChanged & CHGD_DURBAR ) 
		{
			propcurve.m_lDurBar = m_lDurBar;
		}
		if( m_dwChanged & CHGD_DURBEAT ) 
		{
			propcurve.m_lDurBeat = m_lDurBeat;
		}
		if( m_dwChanged & CHGD_DURGRID ) 
		{
			propcurve.m_lDurGrid = m_lDurGrid;
		}
		if( m_dwChanged & CHGD_DURTICK ) 
		{
			propcurve.m_lDurTick = m_lDurTick;
		}
		
		// Reset duration
		if( m_dwChanged & CHGD_RESETBAR ) 
		{
			propcurve.m_lResetBar = m_lResetBar;
		}
		if( m_dwChanged & CHGD_RESETBEAT ) 
		{
			propcurve.m_lResetBeat = m_lResetBeat;
		}
		if( m_dwChanged & CHGD_RESETGRID ) 
		{
			propcurve.m_lResetGrid = m_lResetGrid;
		}
		if( m_dwChanged & CHGD_RESETTICK ) 
		{
			propcurve.m_lResetTick = m_lResetTick;
		}

		propcurve.CalcGridStartAndDuration( m_dwChanged );

		if( pDMCurve->m_mtGridStart != propcurve.m_mtGridStart )
		{
			dwChanged |= CHGD_GRIDSTART;
			pDMCurve->m_mtGridStart	= propcurve.m_mtGridStart;
		}

		if( pDMCurve->m_nTimeOffset != propcurve.m_nTimeOffset )
		{
			dwChanged |= CHGD_OFFSET;
			pDMCurve->m_nTimeOffset	= propcurve.m_nTimeOffset;
		}

		if( pDMCurve->m_mtDuration != propcurve.m_mtDuration )
		{
			dwChanged |= CHGD_DURATION;
			pDMCurve->m_mtDuration	= propcurve.m_mtDuration;

			if( pDMCurve->m_mtDuration >= lPartClockLength )
			{
				pDMCurve->m_mtDuration = lPartClockLength - 1;
			}
		}

		if( pDMCurve->m_mtResetDuration != propcurve.m_mtResetDuration )
		{
			dwChanged |= CHGD_RESETDURATION;
			pDMCurve->m_mtResetDuration = propcurve.m_mtResetDuration;

			if( pDMCurve->m_mtResetDuration > lPartClockLength )
			{
				pDMCurve->m_mtResetDuration = lPartClockLength;
			}
		}
	}

	// un-offset values for pitchbends
	if (pDMCurve->m_bEventType == DMUS_CURVET_PBCURVE) {

		m_nStartValue -= PB_DISP_OFFSET;
		m_nEndValue -= PB_DISP_OFFSET;
		m_lResetValue -= PB_DISP_OFFSET;
	}

	return dwChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::GetValuesFromDMCurve

void CPropCurve::GetValuesFromDMCurve( const CDirectMusicPart* pDMPart,
									   const CDirectMusicStyleCurve* pDMCurve )
{
	ASSERT( pDMPart != NULL );
	ASSERT( pDMCurve != NULL );

	m_mtGridStart		= pDMCurve->m_mtGridStart;
	m_nTimeOffset		= pDMCurve->m_nTimeOffset;
	m_dwVariation		= pDMCurve->m_dwVariation;

	m_mtDuration		= pDMCurve->m_mtDuration;
	m_mtResetDuration	= pDMCurve->m_mtResetDuration;
	m_nStartValue		= pDMCurve->m_nStartValue;
	m_nEndValue			= pDMCurve->m_nEndValue;
	m_lResetValue		= pDMCurve->m_nResetValue;
	m_bCurveShape		= pDMCurve->m_bCurveShape;

	m_TimeSignature		= pDMPart->m_TimeSignature;
	m_wNbrMeasures		= pDMPart->m_wNbrMeasures;
	m_bEventType		= pDMCurve->m_bEventType;
	m_bFlags			= pDMCurve->m_bFlags;
	m_bCCData			= pDMCurve->m_bCCData;
	m_wParamType		= pDMCurve->m_wParamType;
	m_wMergeIndex		= pDMCurve->m_wMergeIndex;

	// offset values for pitchbends
	if (m_bEventType == DMUS_CURVET_PBCURVE) {

		m_nStartValue -= PB_DISP_OFFSET;
		m_nEndValue -= PB_DISP_OFFSET;
		m_lResetValue -= PB_DISP_OFFSET;
	}
	
	CalcBarBeatGridTick();

	m_dwUndetermined	= UNDT_CURVENONE;
	m_dwVarUndetermined = 0x00000000;
	m_dwUndetermined2	= 0; // &= ~(UNDT2_MERGEINDEX | UNDT2_STARTCURRENT);
	m_dwChanged			= 0;
	m_dwChanged2		= 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::CalcBarBeatGridTick

void CPropCurve::CalcBarBeatGridTick( void )
{
	// Set Reset Duration Bar-Beat-Grid-Tick fields
	CPropItem::TimeToBarBeatGridTick( m_TimeSignature, m_mtResetDuration, &m_lResetBar, &m_lResetBeat, &m_lResetGrid, &m_lResetTick );

	//AMC TODO: What if m_lStartBar is > m_wNbrMeasures?
	// Set Start Bar-Beat-Grid-Tick fields
	CPropItem::TimeToBarBeatGridTick( m_TimeSignature, AbsTime(), &m_lStartBar, &m_lStartBeat, &m_lStartGrid, &m_lStartTick );
	m_lStartBar++;
	m_lStartBeat++;
	m_lStartGrid++;

	// Set Duration Bar-Beat-Grid-Tick fields
	CPropItem::TimeToBarBeatGridTick( m_TimeSignature, m_mtDuration, &m_lDurBar, &m_lDurBeat, &m_lDurGrid, &m_lDurTick );

	// Set End Bar-Beat-Grid-Tick fields
	CPropItem::TimeToBarBeatGridTick( m_TimeSignature, AbsTime() + m_mtDuration, &m_lEndBar, &m_lEndBeat, &m_lEndGrid, &m_lEndTick );
	m_lEndBar++;
	m_lEndBeat++;
	m_lEndGrid++;
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::CalcGridStartAndDuration

void CPropCurve::CalcGridStartAndDuration( DWORD dwChanged )
{
	long lClocksPerBeat = DM_PPQNx4 / (long)m_TimeSignature.m_bBeat;
	long lClocksPerMeasure = lClocksPerBeat * (long)m_TimeSignature.m_bBeatsPerMeasure;
	long lClocksPerGrid = lClocksPerBeat / (long)m_TimeSignature.m_wGridsPerBeat;

	if( dwChanged & CHGD_START_BARBEATGRIDTICK )
	{
		long lStartBeat = (m_lStartBar - 1) * (long)m_TimeSignature.m_bBeatsPerMeasure;
		lStartBeat += (m_lStartBeat - 1);

		long lStartGrid = lStartBeat * (long)m_TimeSignature.m_wGridsPerBeat;
		lStartGrid += (m_lStartGrid - 1); 

		if( lStartGrid < 0 )
		{
			long lTimeOffset = m_lStartTick + lStartGrid * lClocksPerGrid;

			// ensure lTimeOffset does not put the event more than one measure ahead of the pattern
			if( lTimeOffset < -lClocksPerMeasure )
			{
				lTimeOffset = -lClocksPerMeasure;
			}

			// ensure lTimeOffset is within the bounds of a short
			if ( lTimeOffset < SHRT_MIN )
			{
				lTimeOffset = SHRT_MIN;
			}

			m_nTimeOffset = (short)lTimeOffset;
			m_mtGridStart = 0;
		}
		else
		{
			if( lStartGrid >= m_wNbrMeasures * m_TimeSignature.m_wGridsPerBeat * m_TimeSignature.m_bBeatsPerMeasure )
			{
				m_mtGridStart = m_wNbrMeasures * m_TimeSignature.m_wGridsPerBeat * m_TimeSignature.m_bBeatsPerMeasure - 1;
				m_nTimeOffset = short(min( lClocksPerGrid - 1, m_lStartTick + lClocksPerGrid ));
			}
			else
			{
				m_mtGridStart = lStartGrid;
				m_nTimeOffset = (short)m_lStartTick;
			}
		}
	}

	else if( dwChanged & CHGD_END_BARBEATGRIDTICK )
	{
		long lStart = m_lStartTick;
		lStart += m_lStartGrid * lClocksPerGrid;
		lStart += m_lStartBeat * lClocksPerBeat;
		lStart += m_lStartBar * lClocksPerMeasure;
		if( lStart < 1 )
		{
			lStart = 1;
		}
		
		long lEnd = m_lEndTick;
		lEnd += m_lEndGrid * lClocksPerGrid;
		lEnd += m_lEndBeat * lClocksPerBeat;
		lEnd += m_lEndBar * lClocksPerMeasure;
		if( lEnd <= lStart )
		{
			lEnd = lStart + 1;
		}
		
		m_mtDuration = lEnd - lStart;
	}

	else if( dwChanged & CHGD_DUR_BARBEATGRIDTICK )
	{
		long lDuration = m_lDurTick;
		lDuration += m_lDurGrid * lClocksPerGrid;
		lDuration += m_lDurBeat * lClocksPerBeat;
		lDuration += m_lDurBar * lClocksPerMeasure;
		if( lDuration < 1 )
		{
			lDuration = 1;
		}

		m_mtDuration = lDuration;
	}

	else if( dwChanged & CHGD_RESET_BARBEATGRIDTICK )
	{
		long lDuration = m_lResetTick;
		lDuration += m_lResetGrid * lClocksPerGrid;
		lDuration += m_lResetBeat * lClocksPerBeat;
		lDuration += m_lResetBar * lClocksPerMeasure;
		if( lDuration < 1 )
		{
			lDuration = 1;
		}

		m_mtResetDuration = lDuration;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::operator +=

CPropCurve CPropCurve::operator +=(const CPropCurve PropCurve)
{
	// Grid Start
	if( m_mtGridStart != PropCurve.m_mtGridStart )
	{
		m_dwUndetermined |= UNDT_GRIDSTART;
	}

	// Grid Offset
	if( m_nTimeOffset != PropCurve.m_nTimeOffset )
	{
		m_dwUndetermined |= UNDT_OFFSET;
	}

	// Variations
	m_dwVarUndetermined |= m_dwVariation ^ PropCurve.m_dwVariation;
	if( ~m_dwVarUndetermined == 0 )
	{
		m_dwUndetermined |= UNDT_VARIATIONS;
	}
	m_dwVariation &= PropCurve.m_dwVariation;

	// Duration
	if( m_mtDuration != PropCurve.m_mtDuration )
	{
		m_dwUndetermined |= UNDT_DURATION;
	}

	// Reset Duration
	if( m_mtResetDuration != PropCurve.m_mtResetDuration )
	{
		m_dwUndetermined |= UNDT_RESETDURATION;
	}

	// Start Value
	if( m_nStartValue != PropCurve.m_nStartValue )
	{
		m_dwUndetermined |= UNDT_STARTVAL;
	}

	// End Value
	if( m_nEndValue != PropCurve.m_nEndValue )
	{
		m_dwUndetermined |= UNDT_ENDVAL;
	}

	// Reset Value
	if( m_lResetValue != PropCurve.m_lResetValue )
	{
		m_dwUndetermined |= UNDT_RESETVAL;
	}

	// Shape
	if( m_bCurveShape != PropCurve.m_bCurveShape )
	{
		m_dwUndetermined |= UNDT_SHAPE;
	}

	// Reset Enable
	if ((m_bFlags & DMUS_CURVE_RESET) != (PropCurve.m_bFlags & DMUS_CURVE_RESET))
	{
		m_dwUndetermined |= UNDT_RESETENABLE;
	}

	// Start from current
	if ((m_bFlags & DMUS_CURVE_START_FROM_CURRENT) != (PropCurve.m_bFlags & DMUS_CURVE_START_FROM_CURRENT))
	{
		m_dwUndetermined2 |= UNDT2_STARTCURRENT;
	}

	// Merge Index
	if (m_wMergeIndex != PropCurve.m_wMergeIndex)
	{
		m_dwUndetermined2 |= UNDT2_MERGEINDEX;
	}

	// Start Bar, Beat, Grid, Tick
	if( m_lStartBar != PropCurve.m_lStartBar )
	{
 		m_dwUndetermined |= UNDT_STARTBAR;
	}
	if( m_lStartBeat != PropCurve.m_lStartBeat )
	{
		m_dwUndetermined |= UNDT_STARTBEAT;
	}
	if( m_lStartGrid != PropCurve.m_lStartGrid )
	{
		m_dwUndetermined |= UNDT_STARTGRID;
	}
	if( m_lStartTick != PropCurve.m_lStartTick )
	{
		m_dwUndetermined |= UNDT_STARTTICK;
	}

	// End Bar, Beat, Grid, Tick
	if( m_lEndBar != PropCurve.m_lEndBar )
	{
		m_dwUndetermined |= UNDT_ENDBAR;
	}
	if( m_lEndBeat != PropCurve.m_lEndBeat )
	{
		m_dwUndetermined |= UNDT_ENDBEAT;
	}
	if( m_lEndGrid != PropCurve.m_lEndGrid )
	{
		m_dwUndetermined |= UNDT_ENDGRID;
	}
	if( m_lEndTick != PropCurve.m_lEndTick )
	{
		m_dwUndetermined |= UNDT_ENDTICK;
	}

	// Dur Bar, Beat, Grid, Tick
	if( m_lDurBar != PropCurve.m_lDurBar )
	{
		m_dwUndetermined |= UNDT_DURBAR;
	}
	if( m_lDurBeat != PropCurve.m_lDurBeat )
	{
		m_dwUndetermined |= UNDT_DURBEAT;
	}
	if( m_lDurGrid != PropCurve.m_lDurGrid )
	{
		m_dwUndetermined |= UNDT_DURGRID;
	}
	if( m_lDurTick != PropCurve.m_lDurTick )
	{
		m_dwUndetermined |= UNDT_DURTICK;
	}

	// Reset Duration Bar, Beat, Grid, Tick
	if( m_lResetBar != PropCurve.m_lResetBar )
	{
		m_dwUndetermined |= UNDT_RESETBAR;
	}
	if( m_lResetBeat != PropCurve.m_lResetBeat )
	{
		m_dwUndetermined |= UNDT_RESETBEAT;
	}
	if( m_lResetGrid != PropCurve.m_lResetGrid )
	{
		m_dwUndetermined |= UNDT_RESETGRID;
	}
	if( m_lResetTick != PropCurve.m_lResetTick )
	{
		m_dwUndetermined |= UNDT_RESETTICK;
	}

	return *this;
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::AbsTime

MUSIC_TIME CPropCurve::AbsTime()
{
	long lBeatClocks, lGridClocks;
	lBeatClocks = DM_PPQNx4 / m_TimeSignature.m_bBeat;
	lGridClocks = lBeatClocks / m_TimeSignature.m_wGridsPerBeat;

	return m_nTimeOffset + lBeatClocks * (m_mtGridStart / m_TimeSignature.m_wGridsPerBeat)
		+ lGridClocks * (m_mtGridStart % m_TimeSignature.m_wGridsPerBeat);
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::Copy

void CPropCurve::Copy( CPropCurve *pPropCurve )
{
	if( pPropCurve )
	{
		*this = *pPropCurve;
	}
	else
	{
		Clear();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::Clear

void CPropCurve::Clear()
{
	memset( this, 0, sizeof(CPropCurve) );

	m_dwUndetermined	= 0xFFFFFFFF;
	m_dwUndetermined2	= 0xFFFFFFFF;
	m_dwVarUndetermined = 0xFFFFFFFF;

	//m_dwChanged = 0x00000000;
	//m_dwChanged2 = 0x00000000;

	m_TimeSignature.m_bBeatsPerMeasure = 4;
	m_TimeSignature.m_bBeat = 4;
	m_TimeSignature.m_wGridsPerBeat = 4;
	m_wNbrMeasures = 1;
	m_bEventType = DMUS_CURVET_CCCURVE;
}
