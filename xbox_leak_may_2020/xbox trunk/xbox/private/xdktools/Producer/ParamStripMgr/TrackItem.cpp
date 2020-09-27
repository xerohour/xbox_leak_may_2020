#include "stdafx.h"
#include <math.h>
#include <RiffStrm.h>
#include "TrackItem.h"
#include "TrackMgr.h"
#include "ParamStrip.h"
#include <dmusicf.h>

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//	CTrackItem Constructors/Destructor

CTrackItem::CTrackItem( void ) : m_fSelected(FALSE)
{
	m_pTrackMgr = NULL;
	m_pParamStrip = NULL;

	Clear();
}

CTrackItem::CTrackItem( CTrackMgr* pTrackMgr, CParamStrip* pParamStrip )
{
	ASSERT( pTrackMgr != NULL );
	m_pTrackMgr = pTrackMgr;
	m_pParamStrip = pParamStrip;

	Clear();
}

CTrackItem::CTrackItem( CTrackMgr* pTrackMgr, CParamStrip* pParamStrip, const CTrackItem& item )
{
	ASSERT( pTrackMgr != NULL );
	m_pTrackMgr = pTrackMgr;
	m_pParamStrip = pParamStrip;

	// Copy the passed-in item
	Copy( &item );
}

CTrackItem::~CTrackItem( void )
{
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::Clear

void CTrackItem::Clear()
{
	m_mtStartTime = 0;
	m_mtEndTime = 0;
	m_fltStartValue = 0;
	m_fltEndValue = 0;
	m_dwCurveType = MP_CURVE_SINE;

	m_rtTimePhysical = 0;
	m_rtDuration = 0;

	m_lMeasure = 0;
	m_lBeat = 0;
	m_lTick = 0;
	m_dwBitsUI = 0;
	m_wFlagsUI = 0;
	m_fSelected = FALSE;

	m_dwFlags = MPF_ENVLP_STANDARD;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::IsEqual

BOOL CTrackItem::IsEqual( const CTrackItem* pItem )
{
	ASSERT( pItem != NULL );
	
	if( pItem )
	{
		if( m_mtStartTime == pItem->m_mtStartTime
		&&  m_mtEndTime == pItem->m_mtEndTime
		&&  m_fltStartValue == pItem->m_fltStartValue
		&&  m_fltEndValue == pItem->m_fltEndValue
		&&  m_dwCurveType == pItem->m_dwCurveType )
		{
			return TRUE;
		}
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::Copy

void CTrackItem::Copy( const CTrackItem* pItem )
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

	m_mtStartTime = pItem->m_mtStartTime;
	m_mtEndTime = pItem->m_mtEndTime;
	m_fltStartValue = pItem->m_fltStartValue;
	m_fltEndValue = pItem->m_fltEndValue;
	m_dwCurveType = pItem->m_dwCurveType;

	m_rtTimePhysical = pItem->m_rtTimePhysical;
	m_rtDuration = pItem->m_rtDuration;

	m_lMeasure = pItem->m_lMeasure;
	m_lBeat = pItem->m_lBeat;
	m_lTick = pItem->m_lTick;
	m_dwBitsUI = pItem->m_dwBitsUI;
	m_wFlagsUI = pItem->m_wFlagsUI;
	m_fSelected = pItem->m_fSelected;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::After

BOOL CTrackItem::After( const CTrackItem& item )
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
		// Check if our beat number is equal
		else if( m_lBeat == item.m_lBeat )
		{
			// Check if our tick number is greater
			if( m_lTick > item.m_lTick )
			{
				// Our tick number is greater - we're after the item
				return TRUE;
			}
		}
	}

	// We're either before the item, or on the same measure, beat, tick.
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::Before

BOOL CTrackItem::Before( const CTrackItem& item )
{
	// Check if this item is before the one passed in.

	// Check if our measure number is lesser.
	if( m_lMeasure < item.m_lMeasure )
	{
		// Our measure number is lesser - we're before the item
		return TRUE;
	}
	// Check if our measure number is equal.
	else if( m_lMeasure == item.m_lMeasure )
	{
		// Check if our beat number is lesser.
		if( m_lBeat < item.m_lBeat )
		{
			// Our beat number is lesser - we're before the item
			return TRUE;
		}
		// Check if our beat number is equal.
		else if( m_lBeat == item.m_lBeat )
		{
			// Check if our tick number is lesser.
			if( m_lTick < item.m_lTick )
			{
				// Our tick number is lesser - we're before the item
				return TRUE;
			}
		}
	}

	// We're either after the item, or on the same measure, beat, tick.
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::LoadListItem

HRESULT CTrackItem::LoadListItem( IStream* pIStream, DWORD dwStructSize )
{
    ASSERT(m_pTrackMgr);
    if(m_pTrackMgr == NULL)
    {
        return E_UNEXPECTED;
    }

	// LoadListItem does not expect to be called twice on the same object
	// Code assumes item consists of initial values
	ASSERT( m_mtStartTime == 0 ); 
	ASSERT( m_mtEndTime == 0 );

	if( pIStream == NULL 
	||  dwStructSize == 0 )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	// Load the item
	DMUS_IO_PARAMCONTROLTRACK_CURVEINFO iCurveInfo;
	DWORD dwSize = min( sizeof( DMUS_IO_PARAMCONTROLTRACK_CURVEINFO ), dwStructSize );
	DWORD dwByteCount;
	HRESULT hr = pIStream->Read( &iCurveInfo, dwSize, &dwByteCount );

	// Handle any I/O error by returning a failure code
	if( FAILED( hr )
	||  dwByteCount != dwSize )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// We have the physical time in the start time
	m_rtTimePhysical = iCurveInfo.mtStartTime;
	m_rtDuration = iCurveInfo.mtEndTime;

	m_fltStartValue = iCurveInfo.fltStartValue;
	m_fltEndValue = iCurveInfo.fltEndValue;
	m_dwCurveType = iCurveInfo.dwCurveType;
	m_dwFlags = iCurveInfo.dwFlags;

	m_pTrackMgr->ClocksToMeasureBeatTick( m_mtStartTime, &m_lMeasure, &m_lBeat, &m_lTick );

ON_ERROR:
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SaveListItem

HRESULT CTrackItem::SaveListItem( IStream* pIStream )
{
	if( pIStream == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	// Clear out the structure (clears out the padding bytes as well).
	DMUS_IO_PARAMCONTROLTRACK_CURVEINFO oCurveInfo;
	ZeroMemory( &oCurveInfo, sizeof(DMUS_IO_PARAMCONTROLTRACK_CURVEINFO) );

	// Fill in the members of the DMUS_IO_PARAMCONTROLTRACK_CURVEINFO structure
	oCurveInfo.mtStartTime = m_mtStartTime;
	oCurveInfo.mtEndTime = m_mtEndTime;
	oCurveInfo.fltStartValue = m_fltStartValue;
	oCurveInfo.fltEndValue = m_fltEndValue;
	oCurveInfo.dwCurveType = m_dwCurveType;
	oCurveInfo.dwFlags = m_dwFlags;

	// Write the structure out to the stream
	DWORD dwBytesWritten;
	HRESULT hr = pIStream->Write( &oCurveInfo, sizeof(DMUS_IO_PARAMCONTROLTRACK_CURVEINFO), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_PARAMCONTROLTRACK_CURVEINFO) )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::LoadTrackItem

HRESULT CTrackItem::LoadTrackItem( IStream* pIStream, DWORD dwStructSize )
{
	// LoadTrackItem does not expect to be called twice on the same object
	// Code assumes item consists of initial values
	ASSERT( m_mtStartTime == 0 ); 
	ASSERT( m_mtEndTime == 0 );

	if( pIStream == NULL 
	||  dwStructSize == 0 )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	// Load the item
	DMUS_IO_PARAMCONTROLTRACK_CURVEINFO iCurveInfo;
	DWORD dwSize = min( sizeof( DMUS_IO_PARAMCONTROLTRACK_CURVEINFO ), dwStructSize );
	DWORD dwByteCount;
	HRESULT hr = pIStream->Read( &iCurveInfo, dwSize, &dwByteCount );

	// Handle any I/O error by returning a failure code
	if( FAILED( hr )
	||  dwByteCount != dwSize )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	SetStartEndTime(iCurveInfo.mtStartTime, iCurveInfo.mtEndTime);
	SetStartEndValue(iCurveInfo.fltStartValue, iCurveInfo.fltEndValue);
	
	m_dwCurveType = iCurveInfo.dwCurveType;
	m_dwFlags = iCurveInfo.dwFlags;

	m_pTrackMgr->ClocksToMeasureBeatTick( m_mtStartTime, &m_lMeasure, &m_lBeat, &m_lTick );

ON_ERROR:
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SaveTrackItem

HRESULT CTrackItem::SaveTrackItem( IStream* pIStream )
{
	if( pIStream == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	// Clear out the structure (clears out the padding bytes as well).
	DMUS_IO_PARAMCONTROLTRACK_CURVEINFO oCurveInfo;
	ZeroMemory( &oCurveInfo, sizeof(DMUS_IO_PARAMCONTROLTRACK_CURVEINFO) );

	// Fill in the members of the DMUS_IO_PARAMCONTROLTRACK_CURVEINFO structure
	oCurveInfo.mtStartTime = m_mtStartTime;
	oCurveInfo.mtEndTime = m_mtEndTime;
	oCurveInfo.fltStartValue = m_fltStartValue;
	oCurveInfo.fltEndValue = m_fltEndValue;
	oCurveInfo.dwCurveType = m_dwCurveType;
	oCurveInfo.dwFlags = m_dwFlags;

	// Write the structure out to the stream
	DWORD dwBytesWritten;
	HRESULT hr = pIStream->Write( &oCurveInfo, sizeof(DMUS_IO_PARAMCONTROLTRACK_CURVEINFO), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_PARAMCONTROLTRACK_CURVEINFO) )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::ComputeCurve

float CTrackItem::ComputeCurve( MUSIC_TIME* pmtIncrement )
{
	float fltRet = 0;
	
	IDMUSProdTimeline* pITimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pITimeline);
	if(pITimeline == NULL)
	{
		return 0;
	}

	MUSIC_TIME mtTimeChange = m_mtEndTime - m_mtStartTime;
	MUSIC_TIME mtTimeIntermediate = m_mtCurrent;

	// Horizontal distance along curve between 0 and 1
	float fltScalingX = static_cast<float>(mtTimeIntermediate) / mtTimeChange; 
	// Height of curve at that point between 0 and 1 based on curve f
	float fltScalingY = 0; 

	switch( m_dwCurveType )
	{
		case MP_CURVE_LINEAR:
		{
			fltScalingY = fltScalingX;
			break;
		}

		case MP_CURVE_SQUARE:
		{
			fltScalingY = fltScalingX * fltScalingX;
			break;
		}

		case MP_CURVE_INVSQUARE:
		{
			fltScalingY = (float) sqrt(fltScalingX);
			break;
		}

		case MP_CURVE_SINE:
		{
			fltScalingY = (float) (sin(fltScalingX * 3.1415926535 - (3.1415926535/2)) + 1) / 2;
			break;
		}

		case MP_CURVE_JUMP:
		default:
		{
			fltRet = m_fltEndValue;
			return fltRet;
		}
	}

	fltRet = (m_fltEndValue - m_fltStartValue) * fltScalingY + m_fltStartValue;

	return fltRet;
}

/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SwitchTimeBase

void CTrackItem::SwitchTimeBase( void )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		// Switching from MUSIC_TIME to REFERENCE_TIME
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( (MUSIC_TIME)m_rtTimePhysical, &m_rtTimePhysical );
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( m_rtDuration, &m_rtDuration );
	}
	else
	{
		// Switching from REFERENCE_TIME to MUSIC_TIME
		MUSIC_TIME mtTime;
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( m_rtTimePhysical, &mtTime );
		m_rtTimePhysical = mtTime;
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( m_rtDuration, &mtTime );
		m_mtDuration = mtTime;
        m_rtDuration = mtTime;
	}
}

void CTrackItem::SetStartEndValue(float fltStart, float fltEnd)
{
	ASSERT(m_pParamStrip);

	// Check for the type for the values
	if(m_pParamStrip->m_ParamInfo.mpType != MPT_FLOAT)
	{
		if(fltStart < 0)
		{
			fltStart = (int)(fltStart - 0.5);
		}
		else
		{
			fltStart = (int)(fltStart + 0.5);
		}

		if(fltEnd < 0)
		{
			fltEnd = (int)(fltEnd - 0.5);
		}
		else
		{
			fltEnd = (int)(fltEnd + 0.5);
		}
	}

	m_fltStartValue = fltStart;
	m_fltEndValue = fltEnd;
}


void CTrackItem::GetStartEndValue(float& fltStart, float& fltEnd)
{
	fltStart = m_fltStartValue;
	fltEnd = m_fltEndValue;
}


void CTrackItem::SetPhysicalTime(REFERENCE_TIME rtTimePhysical, REFERENCE_TIME rtDuration)
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}

	REFERENCE_TIME rtTemp = rtTimePhysical;

	MUSIC_TIME mtStartTime = 0;
	m_pTrackMgr->UnknownTimeToClocks(rtTemp, &mtStartTime);

	rtTemp = rtDuration;
	MUSIC_TIME mtDuration = 0;
	m_pTrackMgr->UnknownTimeToClocks(rtTemp, &mtDuration);

	MUSIC_TIME mtEndTime = mtStartTime + mtDuration;

	m_mtStartTime = mtStartTime;
	m_mtEndTime = mtEndTime;
	m_mtDuration = mtDuration;
	m_rtTimePhysical = rtTimePhysical;
	m_rtDuration = rtDuration;

	m_pTrackMgr->ClocksToMeasureBeatTick(m_mtStartTime, &m_lMeasure, &m_lBeat, &m_lTick);
}

void CTrackItem::SetStartEndTime(MUSIC_TIME mtStart, MUSIC_TIME mtEnd)
{
	m_mtStartTime = mtStart;
	m_mtEndTime = mtEnd;

	UpdateTimeFieldsFromMeasureBeatTick();
}


void CTrackItem::UpdateTimeFieldsFromMeasureBeatTick()
{
	m_pTrackMgr->ClocksToMeasureBeatTick(m_mtStartTime, &m_lMeasure, &m_lBeat, &m_lTick);
	m_pTrackMgr->ClocksToUnknownTime(m_mtStartTime, &m_rtTimePhysical);

	m_mtDuration = m_mtEndTime - m_mtStartTime;
	m_pTrackMgr->ClocksToUnknownTime(m_mtDuration, &m_rtDuration);
}