#include "stdafx.h"
#include "TrackMgr.h"
#include "TrackItem.h"
#include "ParamStrip.h"
#include "PropCurve.h"


CPropCurve::CPropCurve()
{
	Clear();
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::ApplyValuesToCurve

DWORD CPropCurve::ApplyValuesToCurve( CParamStrip* pParamStrip, CTrackItem* pCurve )
{
	ASSERT( pParamStrip != NULL );
	ASSERT( pCurve != NULL );

	if( pParamStrip == NULL
	||  pCurve == NULL )
	{
		return 0;
	}

	CTrackMgr* pTrackMgr = pParamStrip->GetTrackManager();
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return 0;
	}

	IDMUSProdTimeline* pTimeline = pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return 0;
	}

	// Get the length of the segment
	VARIANT var;
	pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	long lTimelineClockLength = V_I4(&var);

	DWORD dwChanged = 0;

	// Start Value
	if( m_dwChanged & CHGD_STARTVAL )
	{
		if( pCurve->m_fltStartValue != m_fltStartValue )
		{
			dwChanged |= CHGD_STARTVAL;
			pCurve->SetStartEndValue(m_fltStartValue, m_fltEndValue);
		}
	}

	// End Value
	if( m_dwChanged & CHGD_ENDVAL )
	{
		if( pCurve->m_fltEndValue != m_fltEndValue )
		{
			dwChanged |= CHGD_ENDVAL;
			pCurve->SetStartEndValue(m_fltStartValue, m_fltEndValue);
		}
	}

	// Flip Vert button clicked
	if( m_dwChanged & CHGD_FLIPVERT )
	{
		switch( pCurve->m_dwCurveType)
		{
			case MP_CURVE_SINE:
			case MP_CURVE_LINEAR:
			{
				dwChanged |= CHGD_FLIPVERT;

				float fltMaxValue = 0;
				fltMaxValue = pParamStrip->m_ParamInfo.mpdMaxValue;
				if(pParamStrip->m_ParamInfo.mpdMinValue < 0 && pParamStrip->m_ParamInfo.mpdMaxValue < 0)
				{
					pCurve->m_fltStartValue	= 0 - pCurve->m_fltStartValue;
					pCurve->m_fltEndValue = 0 - pCurve->m_fltEndValue;
				}
				else
				{
					pCurve->m_fltStartValue	= fltMaxValue - pCurve->m_fltStartValue;
					pCurve->m_fltEndValue = fltMaxValue - pCurve->m_fltEndValue;
				}

				pCurve->m_fltStartValue = pCurve->m_fltStartValue > pParamStrip->m_ParamInfo.mpdMaxValue ? pParamStrip->m_ParamInfo.mpdMaxValue : pCurve->m_fltStartValue;
				pCurve->m_fltStartValue = pCurve->m_fltStartValue < pParamStrip->m_ParamInfo.mpdMinValue ? pParamStrip->m_ParamInfo.mpdMinValue : pCurve->m_fltStartValue;
				pCurve->m_fltEndValue = pCurve->m_fltEndValue > pParamStrip->m_ParamInfo.mpdMaxValue ? pParamStrip->m_ParamInfo.mpdMaxValue : pCurve->m_fltEndValue;
				pCurve->m_fltEndValue = pCurve->m_fltEndValue < pParamStrip->m_ParamInfo.mpdMinValue ? pParamStrip->m_ParamInfo.mpdMinValue : pCurve->m_fltEndValue;

				
				break;
			}

			case MP_CURVE_SQUARE:
			{
				dwChanged |= CHGD_FLIPVERT;
				pCurve->m_dwCurveType = MP_CURVE_SQUARE;
				break;
			}

			case MP_CURVE_INVSQUARE:
			{
				dwChanged |= CHGD_FLIPVERT;
				pCurve->m_dwCurveType = MP_CURVE_INVSQUARE;
				break;
			}

			case MP_CURVE_JUMP:
			{
				dwChanged |= CHGD_FLIPVERT;
				pCurve->m_dwCurveType = MP_CURVE_JUMP;
				break;
			}
		}
	}

	// Flip horz button clicked
	if( m_dwChanged & CHGD_FLIPHORZ )
	{
		dwChanged |= CHGD_FLIPHORZ;

		float fltStartValue = pCurve->m_fltStartValue;
		pCurve->m_fltStartValue	= pCurve->m_fltEndValue;
		pCurve->m_fltEndValue = fltStartValue;
	}

	// Shape
	if( m_dwChanged & CHGD_SHAPE )
	{
		if( pCurve->m_dwCurveType != m_dwCurveShape )
		{
			dwChanged |= CHGD_SHAPE;
			pCurve->m_dwCurveType	= m_dwCurveShape;
		}
	}

	if( (m_dwChanged & CHGD_START_BARBEATGRIDTICK) || (m_dwChanged & CHGD_END_BARBEATGRIDTICK)
	||  (m_dwChanged & CHGD_DUR_BARBEATGRIDTICK) )
	{
		dwChanged |= CHGD_START_BARBEATGRIDTICK | CHGD_END_BARBEATGRIDTICK | CHGD_DUR_BARBEATGRIDTICK;

		MUSIC_TIME mtStartTime = 0;
		pTrackMgr->UnknownTimeToClocks(m_rtStartTime, &mtStartTime);

		REFERENCE_TIME rtEndTime = m_rtStartTime + m_rtDuration;
		MUSIC_TIME mtEndTime = 0;
		pTrackMgr->UnknownTimeToClocks(rtEndTime, &mtEndTime);

		if(pTrackMgr->IsRefTimeTrack())
		{
			pCurve->SetPhysicalTime(m_rtStartTime, m_rtDuration);
		}
		else
		{
			pCurve->SetStartEndTime(mtStartTime, mtEndTime);
		}
	}

	// Start from current checkbox selected
	if(m_dwChanged & CHGD_STARTCURRENT)
	{
		dwChanged |= CHGD_STARTCURRENT;
		pCurve->m_dwFlags = m_fStartFromCurrent == TRUE ? MPF_ENVLP_BEGIN_CURRENTVAL : MPF_ENVLP_STANDARD;
	}
	

	return dwChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::GetValuesFromCurve

void CPropCurve::GetValuesFromCurve( CParamStrip* pParamStrip, CTrackItem* pCurve )
{
	ASSERT( pParamStrip != NULL );
	if(pParamStrip == NULL)
	{
		return;
	}

	ASSERT( pCurve != NULL );
	if(pCurve == NULL)
	{
		return;
	}

	CTrackMgr* pTrackMgr = pParamStrip->GetTrackManager();
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	m_pParamStrip = pParamStrip;

	m_fltStartValue		= pCurve->m_fltStartValue;
	m_fltEndValue		= pCurve->m_fltEndValue;
	m_dwCurveShape		= pCurve->m_dwCurveType;

	MUSIC_TIME mtDuration = pCurve->m_mtDuration;
	MUSIC_TIME mtStartTime = pCurve->m_mtStartTime;
	if(pTrackMgr->IsRefTimeTrack())
	{
		m_rtStartTime = pCurve->m_rtTimePhysical;
		m_rtDuration = pCurve->m_rtDuration;
	}
	else
	{
		pTrackMgr->ClocksToUnknownTime(mtDuration, &m_rtDuration);
		pTrackMgr->ClocksToUnknownTime(mtStartTime, &m_rtStartTime);
	}

	MUSIC_TIME mtEndTime = mtStartTime + mtDuration;

	// Get the start, end and duration bar, beat, grid tick
	if(FAILED(pTrackMgr->ClocksToMeasureBeatGridTick(mtStartTime, &m_lStartBar, &m_lStartBeat, &m_lStartGrid, &m_lStartTick)))
	{
		return;
	}

	if(FAILED(pTrackMgr->ClocksToMeasureBeatGridTick(mtEndTime, &m_lEndBar, &m_lEndBeat, &m_lEndGrid, &m_lEndTick)))
	{
		return;
	}

	if(FAILED(pTrackMgr->ClocksToMeasureBeatGridTick(mtDuration, &m_lDurBar, &m_lDurBeat, &m_lDurGrid, &m_lDurTick)))
	{
		return;
	}


	m_dwUndetermined	= UNDT_CURVENONE;
	m_dwUndetermined2	= 0; // &= ~(UNDT2_MERGEINDEX | UNDT2_STARTCURRENT);
	m_dwChanged			= 0;
	m_dwChanged2		= 0;

	m_fStartFromCurrent = pCurve->m_dwFlags & MPF_ENVLP_BEGIN_CURRENTVAL;
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

	m_pParamStrip = NULL;
	m_fStartFromCurrent = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPropCurve::operator +=

CPropCurve CPropCurve::operator +=(const CPropCurve PropCurve)
{
	// Duration
	if( m_rtDuration != PropCurve.m_rtDuration )
	{
		m_dwUndetermined |= UNDT_DURATION;
	}


	// Start Value
	if( m_fltStartValue != PropCurve.m_fltStartValue )
	{
		m_dwUndetermined |= UNDT_STARTVAL;
	}

	// End Value
	if( m_fltEndValue != PropCurve.m_fltEndValue )
	{
		m_dwUndetermined |= UNDT_ENDVAL;
	}

	CTrackMgr* pTrackMgr = m_pParamStrip->GetTrackManager();
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return *this;
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

	if(m_dwCurveShape != PropCurve.m_dwCurveShape)
	{
		m_dwUndetermined |= UNDT_SHAPE;
	}

	if(m_fStartFromCurrent != PropCurve.m_fStartFromCurrent)
	{
		m_dwUndetermined |= UNDT2_STARTCURRENT;
	}

	return *this;
}

void CPropCurve::UpdateStartBarBeatGridTick(long lStartBar, long lStartBeat, long lStartGrid, long lStartTick)
{
	m_lStartBar = lStartBar;
	m_lStartBeat = lStartBeat;
	m_lStartGrid = lStartGrid;
	m_lStartTick = lStartTick;
}

void CPropCurve::UpdateEndBarBeatGridTick(long lEndBar, long lEndBeat, long lEndGrid, long lEndTick)
{
	m_lEndBar = lEndBar;
	m_lEndBeat =lEndBeat;
	m_lEndGrid =lEndGrid;
	m_lEndTick = lEndTick;
}

void CPropCurve::UpdateDurationBarBeatGridTick(long lDurationBar, long lDurationBeat, long lDurationGrid, long lDurationTick)
{
	m_lDurBar = lDurationBar;
	m_lDurBeat = lDurationBeat;
	m_lDurGrid = lDurationGrid;
	m_lDurTick = lDurationTick;
}