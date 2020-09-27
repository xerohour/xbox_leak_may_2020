#include "stdafx.h"
#include "PropCurve.h"
#include "SequenceMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPropCurve::CPropCurve() : CPropItem()
{
	Clear();
}

CPropCurve::CPropCurve( CSequenceMgr *pSequenceMgr ) : CPropItem( pSequenceMgr )
{
	Clear();
}

CPropCurve::CPropCurve(const CCurveItem& SeqItem, CSequenceMgr* pSequenceMgr)
{
	m_pSequenceMgr = pSequenceMgr;
	Import( &SeqItem );
}

CPropCurve::CPropCurve(const CCurveItem *pSeqItem, CSequenceMgr* pSequenceMgr)
{
	m_pSequenceMgr = pSequenceMgr;
	Import( pSeqItem );
}

void CPropCurve::Copy( const CPropCurve* pPropItem )
{
	if( pPropItem == this )
	{
		return;
	}

	CPropItem::Copy( pPropItem );
	if( pPropItem == NULL )
	{
		Clear();
		return;
	}

	m_dwUndetermined2 = pPropItem->m_dwUndetermined2;
	m_dwChanged2 = pPropItem->m_dwChanged2;
	m_mtResetDuration = pPropItem->m_mtResetDuration;
	m_nStartValue = pPropItem->m_nStartValue;
	m_nEndValue = pPropItem->m_nEndValue;
	m_nResetValue = pPropItem->m_nResetValue;
	m_bEventType = pPropItem->m_bEventType; 
	m_bCurveShape = pPropItem->m_bCurveShape;
	m_bCCData = pPropItem->m_bCCData;
	m_bFlags = pPropItem->m_bFlags;
	m_wParamType = pPropItem->m_wParamType;
	m_wMergeIndex = pPropItem->m_wMergeIndex;

	m_lResetBar = pPropItem->m_lResetBar;
	m_lResetBeat = pPropItem->m_lResetBeat;
	m_lResetGrid = pPropItem->m_lResetGrid;
	m_lResetTick = pPropItem->m_lResetTick;
}

void CPropCurve::Import( const CCurveItem* pPropItem )
{
	CPropItem::Import( pPropItem );
	m_mtResetDuration = pPropItem->m_mtResetDuration;
	m_nStartValue = pPropItem->m_nStartValue;
	m_nEndValue = pPropItem->m_nEndValue;
	m_nResetValue = pPropItem->m_nResetValue;
	m_bEventType = pPropItem->m_bType;	
	m_bCurveShape = pPropItem->m_bCurveShape;
	m_bCCData = pPropItem->m_bCCData;
	m_bFlags = pPropItem->m_bFlags;
	m_wParamType = pPropItem->m_wParamType;
	m_wMergeIndex = pPropItem->m_wMergeIndex;
	m_dwUndetermined &= ~(UNDT_STARTVAL | UNDT_ENDVAL | UNDT_RESETVAL | UNDT_SHAPE
						| UNDT_FLIPVERT | UNDT_FLIPHORZ | UNDT_RESETENABLE | UNDT_RESETDURATION );
	m_dwUndetermined2 = 0; // &= ~(UNDT2_MERGEINDEX | UNDT2_STARTCURRENT);
	m_dwChanged2 = 0;

	CalcResetBarBeatGridTick();

	// offset values for pitchbends
	if (m_bEventType == DMUS_CURVET_PBCURVE) {

		m_nStartValue -= PB_DISP_OFFSET;
		m_nEndValue -= PB_DISP_OFFSET;
		m_nResetValue -= PB_DISP_OFFSET;
	}
}

void CPropCurve::Clear()
{
	CPropItem::Clear();
	m_dwUndetermined2 = 0;
	m_dwChanged2 = 0;
	m_mtResetDuration = 0;
	m_nStartValue = 0;
	m_nEndValue = 0;
	m_nResetValue = 0;
	m_bEventType = 0;	
	m_bCurveShape = 0;
	m_bCCData = 0;
	m_bFlags = 0;
	m_wParamType = 0;
	m_wMergeIndex = 0;
	m_lResetBar = 0;
	m_lResetBeat = 0;
	m_lResetGrid = 0;
	m_lResetTick = 0;
}

CPropCurve CPropCurve::operator +=(const CPropCurve &propcurve)
{
	CPropItem::operator +=( propcurve );

	if (m_nStartValue != propcurve.m_nStartValue)
	{
		m_dwUndetermined |= UNDT_STARTVAL;
	}
	if (m_nEndValue != propcurve.m_nEndValue)
	{
		m_dwUndetermined |= UNDT_ENDVAL;
	}
	if (m_nResetValue != propcurve.m_nResetValue)
	{
		m_dwUndetermined |= UNDT_RESETVAL;
	}

	if (m_lResetBar != propcurve.m_lResetBar)
	{
		m_dwUndetermined |= UNDT_RESETBAR;
	}
	if (m_lResetBeat != propcurve.m_lResetBeat)
	{
		m_dwUndetermined |= UNDT_RESETBEAT;
	}
	if (m_lResetGrid != propcurve.m_lResetGrid)
	{
		m_dwUndetermined |= UNDT_RESETGRID;
	}
	if (m_lResetTick != propcurve.m_lResetTick)
	{
		m_dwUndetermined |= UNDT_RESETTICK;
	}

	/*
	if (m_bCCData != propcurve.m_bCCData)
	{
		m_dwUndetermined |= UNDT_CCDATA;
	}
	*/
	if (m_bCurveShape != propcurve.m_bCurveShape)
	{
		m_dwUndetermined |= UNDT_SHAPE;
	}
	/*
	if (m_bEventType != propcurve.m_bEventType)
	{
		m_dwUndetermined |= UNDT_EVENTTYPE;
	}
	*/
	if ((m_bFlags & DMUS_CURVE_RESET) != (propcurve.m_bFlags & DMUS_CURVE_RESET))
	{
		m_dwUndetermined |= UNDT_RESETENABLE;
	}
	if ((m_bFlags & DMUS_CURVE_START_FROM_CURRENT) != (propcurve.m_bFlags & DMUS_CURVE_START_FROM_CURRENT))
	{
		m_dwUndetermined2 |= UNDT2_STARTCURRENT;
	}
	/*
	if (m_wParamType != propcurve.m_wParamType)
	{
		m_dwUndetermined |= UNDT_PARAMTYPE;
	}
	*/
	if (m_wMergeIndex != propcurve.m_wMergeIndex)
	{
		m_dwUndetermined2 |= UNDT2_MERGEINDEX;
	}
	if (m_mtResetDuration != propcurve.m_mtResetDuration)
	{
		m_dwUndetermined |= UNDT_RESETDURATION;
	}

	return *this;
}

DWORD CPropCurve::ApplyToCurve( CCurveItem* pDMCurve ) const
{
	ASSERT( pDMCurve != NULL );
	if (pDMCurve == NULL)
	{
		return 0;
	}

	DWORD dwChanged = CPropItem::ApplyToEvent( pDMCurve );

	// Start Value
	if( m_dwChanged & CHGD_STARTVAL )
	{
		if (pDMCurve->m_bType == DMUS_CURVET_PBCURVE)
		{
			if( pDMCurve->m_nStartValue != m_nStartValue + PB_DISP_OFFSET )
			{
				dwChanged |= CHGD_STARTVAL;
				pDMCurve->m_nStartValue = short(m_nStartValue + PB_DISP_OFFSET);
			}
		}
		else
		{
			if( pDMCurve->m_nStartValue != m_nStartValue )
			{
				dwChanged |= CHGD_STARTVAL;
				pDMCurve->m_nStartValue = m_nStartValue;
			}
		}
	}

	// End Value
	if( m_dwChanged & CHGD_ENDVAL )
	{
		if (pDMCurve->m_bType == DMUS_CURVET_PBCURVE)
		{
			if( pDMCurve->m_nEndValue != m_nEndValue + PB_DISP_OFFSET )
			{
				dwChanged |= CHGD_ENDVAL;
				pDMCurve->m_nEndValue = short(m_nEndValue + PB_DISP_OFFSET);
			}
		}
		else
		{
			if( pDMCurve->m_nEndValue != m_nEndValue )
			{
				dwChanged |= CHGD_ENDVAL;
				pDMCurve->m_nEndValue = m_nEndValue;
			}
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
				pDMCurve->SetDefaultResetValues( m_pSequenceMgr->m_pSequenceStrip->m_mtLength );
				if( !(pDMCurve->m_bFlags & DMUS_CURVE_RESET) )
				{
					pDMCurve->m_bFlags &= ~DMUS_CURVE_RESET;
					pDMCurve->m_bFlags |= m_bFlags & DMUS_CURVE_RESET;
					pDMCurve->m_nResetValue = 0;	
					pDMCurve->m_mtResetDuration = m_pSequenceMgr->m_pSequenceStrip->m_mtLength;
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
		if (pDMCurve->m_bType == DMUS_CURVET_PBCURVE)
		{
			if( pDMCurve->m_nResetValue != m_nResetValue + PB_DISP_OFFSET )
			{
				dwChanged |= CHGD_RESETVAL;
				pDMCurve->m_nResetValue = short(m_nResetValue + PB_DISP_OFFSET);
			}
		}
		else
		{
			if( pDMCurve->m_nResetValue != m_nResetValue )
			{
				dwChanged |= CHGD_RESETVAL;
				pDMCurve->m_nResetValue = m_nResetValue;
			}
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
				if( pDMCurve->m_bType == DMUS_CURVET_PBCURVE )
				{
					nMaxValue = MAX_PB_VALUE;
				}
				else if( (pDMCurve->m_bType == DMUS_CURVET_RPNCURVE)
					 ||  (pDMCurve->m_bType == DMUS_CURVET_NRPNCURVE) )
				{
					nMaxValue = MAX_RPN_VALUE;
				}
				else
				{
					nMaxValue = MAX_CC_VALUE;
				}
				pDMCurve->m_nStartValue = short(nMaxValue - pDMCurve->m_nStartValue);
				pDMCurve->m_nEndValue = short(nMaxValue - pDMCurve->m_nEndValue);
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
		pDMCurve->m_nStartValue = pDMCurve->m_nEndValue;
		pDMCurve->m_nEndValue = nStartValue;
	}

	// Shape
	if( m_dwChanged & CHGD_SHAPE )
	{
		if( pDMCurve->m_bCurveShape != m_bCurveShape )
		{
			dwChanged |= CHGD_SHAPE;
			pDMCurve->m_bCurveShape = m_bCurveShape;
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

	// Reset duration
	if( m_dwChanged & CHGD_RESET_BARBEATGRIDTICK )
	{
		CPropCurve propcurve( m_pSequenceMgr );
		propcurve.Import( pDMCurve );

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

		propcurve.CalcResetDuration();

		if( pDMCurve->m_mtResetDuration != propcurve.m_mtResetDuration )
		{
			dwChanged |= CHGD_RESETVAL;
			pDMCurve->m_mtResetDuration = propcurve.m_mtResetDuration;

			//TODO: Implement
			/*
			if( pDMCurve->m_mtResetDuration > lPartClockLength )
			{
				pDMCurve->m_mtResetDuration = lPartClockLength;
			}
			*/
		}
	}

	return dwChanged;
}

DWORD CPropCurve::ApplyPropCurve( const CPropCurve* pPropNote )
{
	ASSERT( pPropNote );
	if( !pPropNote )
	{
		return 0;
	}

	DWORD dwChanged = CPropItem::ApplyPropItem( pPropNote );

	// Assume we're only changing start, end or duration bar/beat/grid/tick times.
	ASSERT( 0 == (m_dwChanged & ~(UD_OFFSET | UD_TIMESTART | UD_DURATION | UD_CHORDBAR | UD_CHORDBEAT| UD_STARTBARBEATGRIDTICK | UD_ENDBARBEATGRIDTICK | UD_DURBARBEATGRIDTICK)) );

	return dwChanged;
}

void CPropCurve::CalcResetBarBeatGridTick( void )
{
	//
	// Set Reset Duration parameters
	//
	if ( (m_dwUndetermined & UNDT_RESETDURATION) == 0)
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
		long lTempTime = m_mtResetDuration % lBeatClocks;
		BOOL fAddedGrid = FALSE;
		if( (lTempTime % lGridClocks) > ( lGridClocks - (lGridClocks / 2) - 1) )
		{
			lTempTime = m_mtResetDuration + lGridClocks;
			fAddedGrid = TRUE;
		}
		else
		{
			lTempTime = m_mtResetDuration;
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

		m_lResetBar = lTempMeas;
		m_lResetBeat = lTempBeat;
		m_lResetGrid = lTempGrid;
		m_lResetTick = lTempTick;
		m_dwUndetermined &= ~(UNDT_RESETBAR | UNDT_RESETBEAT | UNDT_RESETGRID | UNDT_RESETTICK);
	}
}


void CPropCurve::CalcResetDuration( void )
{
	//
	// Set Reset Duration
	//

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

	m_mtResetDuration = max( 1, m_lResetTick + m_lResetGrid * lGridClocks +
								m_lResetBeat * lBeatClocks + m_lResetBar * lMeasureClocks );

	m_dwUndetermined &= ~(UNDT_RESETVAL);
}