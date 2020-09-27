// Pattern.cpp : implementation file
//

#include "stdafx.h"
#include "MIDIStripMgr.h"
#include "MIDIMgr.h"
#include "Pattern.h"
#include "ioDMStyle.h"
#include <mmreg.h>
#include "PropCurve.h"
#include "StyleDesigner.h"
#include <riffstrm.h>
#include "ChordTrack.h"
#include "MIDIFileIO.h"
#include "SeqSegmentRiff.h"
#include "Templates.h"
#include "SharedPattern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// default scale is C Major
const DWORD DEFAULT_SCALE_PATTERN = 0xab5ab5;

// Global variables for importing MIDI files
extern long	glTimeSig; // flag to see if MIDI import should be paying attention to time sigs.
extern IStream* gpTempoStream;
extern IStream* gpSysExStream;
extern IStream* gpTimeSigStream;

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicEventItem destructor

CDirectMusicEventItem::~CDirectMusicEventItem()
{
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleCurve contructor

CDirectMusicStyleCurve::CDirectMusicStyleCurve()
{
	m_mtGridStart = 0;
	m_nTimeOffset = 0;
	m_dwVariation = 0;

	m_mtDuration = 0;
	m_mtResetDuration = 0;
	m_nStartValue = 0;	
	m_nEndValue = 0;	
	m_nResetValue = 0;	
    m_bEventType = DMUS_CURVET_CCCURVE;	
	m_bCurveShape = DMUS_CURVES_SINE;	
	m_bCCData = 0;
	m_bFlags = 0;
	m_wParamType = 0;
	m_wMergeIndex =0;
}

CDirectMusicStyleCurve::CDirectMusicStyleCurve( const DMUS_IO_CURVE_ITEM *piSeqCurve )
{
	m_mtGridStart = 0;
	m_nTimeOffset = piSeqCurve->nOffset;
	m_dwVariation = 0;
	m_fSelected = FALSE;

	m_mtDuration = piSeqCurve->mtDuration;
	m_mtResetDuration = piSeqCurve->mtResetDuration;
	m_nStartValue = piSeqCurve->nStartValue;
	m_nEndValue = piSeqCurve->nEndValue;
	m_nResetValue = piSeqCurve->nResetValue;
    m_bEventType = piSeqCurve->bType;
	m_bCurveShape = piSeqCurve->bCurveShape;
	m_bCCData = piSeqCurve->bCCData;
	m_bFlags = piSeqCurve->bFlags;
	m_wParamType = FILE_TO_MEMORY_WPARAMTYPE( piSeqCurve->wParamType );
	m_wMergeIndex =piSeqCurve->wMergeIndex;
}

CDirectMusicStyleCurve::CDirectMusicStyleCurve( const CCurveItem* pCurveItem  )
{
	m_mtGridStart = 0;
	m_nTimeOffset = pCurveItem->m_nOffset;
	m_dwVariation = 0;
	m_fSelected = FALSE;

	m_mtDuration = pCurveItem->m_mtDuration;
	m_mtResetDuration = pCurveItem->m_mtResetDuration;
	m_nStartValue = pCurveItem->m_nStartValue;
	m_nEndValue = pCurveItem->m_nEndValue;
	m_nResetValue = pCurveItem->m_nResetValue;
    m_bEventType = pCurveItem->m_bType;
	m_bCurveShape = pCurveItem->m_bCurveShape;
	m_bCCData = pCurveItem->m_bCCData;
	m_bFlags = pCurveItem->m_bFlags;
	m_wParamType = pCurveItem->m_wParamType;
	m_wMergeIndex =pCurveItem->m_wMergeIndex;
}

CDirectMusicStyleCurve::CDirectMusicStyleCurve( const CDirectMusicStyleCurve* pCDirectMusicStyleCurve  )
{
	m_mtGridStart = pCDirectMusicStyleCurve->m_mtGridStart;
	m_nTimeOffset = pCDirectMusicStyleCurve->m_nTimeOffset;
	m_dwVariation = pCDirectMusicStyleCurve->m_dwVariation;
	m_fSelected = pCDirectMusicStyleCurve->m_fSelected;

	m_mtDuration = pCDirectMusicStyleCurve->m_mtDuration;
	m_mtResetDuration = pCDirectMusicStyleCurve->m_mtResetDuration;
	m_nStartValue = pCDirectMusicStyleCurve->m_nStartValue;
	m_nEndValue = pCDirectMusicStyleCurve->m_nEndValue;
	m_nResetValue = pCDirectMusicStyleCurve->m_nResetValue;
    m_bEventType = pCDirectMusicStyleCurve->m_bEventType;
	m_bCurveShape = pCDirectMusicStyleCurve->m_bCurveShape;
	m_bCCData = pCDirectMusicStyleCurve->m_bCCData;
	m_bFlags = pCDirectMusicStyleCurve->m_bFlags;
	m_wParamType = pCDirectMusicStyleCurve->m_wParamType;
	m_wMergeIndex = pCDirectMusicStyleCurve->m_wMergeIndex;
	m_mtCurrent = pCDirectMusicStyleCurve->m_mtCurrent;
	m_rectFrame = pCDirectMusicStyleCurve->m_rectFrame;
	m_rectSelect = pCDirectMusicStyleCurve->m_rectSelect;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleCurve destructor

CDirectMusicStyleCurve::~CDirectMusicStyleCurve()
{
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleCurve::ComputeCurve

DWORD CDirectMusicStyleCurve::ComputeCurve( MUSIC_TIME* pmtIncrement )
{
	DWORD dwRet;
	short *panTable;

	switch( m_bCurveShape )
	{
		case DMUS_CURVES_LINEAR:
			panTable = &ganCT_Linear[ 0 ];
			break;

		case DMUS_CURVES_EXP:
			panTable = &ganCT_Exp[ 0 ];
			break;

		case DMUS_CURVES_LOG:
			panTable = &ganCT_Log[ 0 ];
			break;

		case DMUS_CURVES_SINE:
			panTable = &ganCT_Sine[ 0 ];
			break;

		case DMUS_CURVES_INSTANT:
		default:
			if( pmtIncrement )
			{
				*pmtIncrement = 0;
			}
			return (DWORD)m_nEndValue;
	}

	// Compute index into table
	// There are CT_MAX + 1 elements in the table.
	short nIndex;

	if( (m_mtDuration == 0)
	|| 	(m_mtCurrent >= m_mtDuration) )
	{
		if( pmtIncrement )
		{
			*pmtIncrement = 0;
		}
		return (DWORD)m_nEndValue;
	}
	else
	{
        nIndex = short((m_mtCurrent * (CT_MAX + 1)) / m_mtDuration);

		// Check boundaries
		if( nIndex < 0 )
		{
			nIndex = 0;
		}
		if( nIndex >= CT_MAX )
		{
			nIndex = CT_MAX;
			dwRet = (DWORD)m_nEndValue;
		}
		else
		{
            // Okay, in the curve, so calculate the return value.
            dwRet = ((panTable[nIndex] * (m_nEndValue - m_nStartValue)) / 
                CT_DIVFACTOR) + m_nStartValue;
		}

		// Does caller want us to compute mtIncrement?
		if( pmtIncrement )
		{
			// Yes.... So compute mtIncrement
			MUSIC_TIME mtIncrement = 1;
			if( nIndex < CT_MAX )
			{
				DWORD dwTotalDistance;
				DWORD dwResolution;
				if ((m_bEventType == DMUS_CURVET_PBCURVE) ||
					(m_bEventType == DMUS_CURVET_RPNCURVE) ||
					(m_bEventType == DMUS_CURVET_NRPNCURVE))
				{
					dwResolution = 100;
				}
				else
				{
					dwResolution = 3;
				}

				if (m_nEndValue > m_nStartValue)
					dwTotalDistance = m_nEndValue - m_nStartValue;
				else 
					dwTotalDistance = m_nStartValue - m_nEndValue;

				if (dwTotalDistance == 0) dwTotalDistance = 1;

				mtIncrement = (m_mtDuration * dwResolution) / dwTotalDistance;

				// Force to no smaller than 192nd note (10ms at 120 bpm.)
				if( mtIncrement < (DMUS_PPQ/48) ) mtIncrement = DMUS_PPQ/48;

				if( (m_mtCurrent + mtIncrement) > m_mtDuration )
				{
					mtIncrement = m_mtDuration - m_mtCurrent;
				}
			}
			else
			{
				mtIncrement = m_mtDuration - m_mtCurrent;
			}
			if( mtIncrement <= 0 )
			{
				mtIncrement = 1;
			}
			*pmtIncrement = mtIncrement;
		}
	}

//	TRACE( "ComputeCurve=%ld   %ld   %ld\n", dwRet, mtIncrement, m_mtCurrent );
	return dwRet;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleMarker contructor

CDirectMusicStyleMarker::CDirectMusicStyleMarker()
{
	m_pNext = 0;
	m_mtGridStart = 0;
	m_nTimeOffset = 0;
	m_dwVariation = 0;
	m_fSelected = 0;
	m_dwEnterVariation = 0;
	m_dwEnterChordVariation = 0;
	m_dwExitVariation = 0;
	m_dwExitChordVariation = 0;
}

CDirectMusicStyleMarker::~CDirectMusicStyleMarker()
{
	// Do nothing
}

HRESULT CDirectMusicStyleMarker::Write( IStream* pIStream )
{
	DMUS_IO_STYLEMARKER oDMStyleMarker;
	DWORD dwBytesWritten;
	HRESULT hr;

	// Check if this marker has any variations of type 'enter'
	if( m_dwEnterVariation )
	{
		// Check if m_dwEnterVariation != m_dwEnterChordVariation
		if( m_dwEnterVariation != m_dwEnterChordVariation )
		{
			// Write out a DMUS_MARKERF_START marker

			// Prepare DMUS_IO_STYLEMARKER structure
			ZeroMemory( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER) );
			oDMStyleMarker.mtGridStart = m_mtGridStart;
			oDMStyleMarker.wMarkerFlags = DMUS_MARKERF_START;

			// Write only the variations where m_dwEnterVariation is set
			oDMStyleMarker.dwVariation = m_dwEnterVariation ^ m_dwEnterChordVariation;

			// Write DMUS_IO_STYLEMARKER structure
			hr = pIStream->Write( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_STYLEMARKER) )
			{
				return E_FAIL;
			}
		}

		// Now, write a DMUS_MARKERF_START | DMUS_MARKERF_CHORD_ALIGN marker, if needed
		if( m_dwEnterChordVariation )
		{
			// Prepare DMUS_IO_STYLEMARKER structure
			ZeroMemory( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER) );
			oDMStyleMarker.mtGridStart = m_mtGridStart;
			oDMStyleMarker.wMarkerFlags = DMUS_MARKERF_START | DMUS_MARKERF_CHORD_ALIGN;

			// Write only the variations where m_dwEnterChordVariation is set
			oDMStyleMarker.dwVariation = m_dwEnterChordVariation;

			// Write DMUS_IO_STYLEMARKER structure
			hr = pIStream->Write( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_STYLEMARKER) )
			{
				return E_FAIL;
			}
		}
	}

	// Check if this marker has any variations of type 'exit'
	if( m_dwExitVariation )
	{
		// Check if m_dwExitVariation != m_dwExitChordVariation
		if( m_dwExitVariation != m_dwExitChordVariation )
		{
			// Write out a DMUS_MARKERF_STOP marker

			// Prepare DMUS_IO_STYLEMARKER structure
			ZeroMemory( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER) );
			oDMStyleMarker.mtGridStart = m_mtGridStart;
			oDMStyleMarker.wMarkerFlags = DMUS_MARKERF_STOP;

			// Write only the variations where m_dwExitVariation is set
			oDMStyleMarker.dwVariation = m_dwExitVariation ^ m_dwExitChordVariation;

			// Write DMUS_IO_STYLEMARKER structure
			hr = pIStream->Write( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_STYLEMARKER) )
			{
				return E_FAIL;
			}
		}

		// Now, write a DMUS_MARKERF_STOP | DMUS_MARKERF_CHORD_ALIGN marker, if needed
		if( m_dwExitChordVariation )
		{
			// Prepare DMUS_IO_STYLEMARKER structure
			ZeroMemory( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER) );
			oDMStyleMarker.mtGridStart = m_mtGridStart;
			oDMStyleMarker.wMarkerFlags = DMUS_MARKERF_STOP | DMUS_MARKERF_CHORD_ALIGN;

			// Write only the variations where m_dwExitChordVariation is set
			oDMStyleMarker.dwVariation = m_dwExitChordVariation;

			// Write DMUS_IO_STYLEMARKER structure
			hr = pIStream->Write( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_STYLEMARKER) )
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CDirectMusicStyleMarker::Read( IStream* pIStream, DWORD dwSize, DWORD dwExtra )
{
	// Prepare DMUS_IO_STYLEMARKER structure
	DMUS_IO_STYLEMARKER iDMStyleMarker;
	memset( &iDMStyleMarker, 0, sizeof(DMUS_IO_STYLEMARKER) );

	// Read the marker
	DWORD dwRead;
	HRESULT hr = pIStream->Read( &iDMStyleMarker, dwSize, &dwRead);
	if( FAILED(hr) || (dwRead != dwSize) )
	{
		return E_FAIL;
	}

	// Skip extra data (if necessary)
	if( dwExtra )
	{
		StreamSeek( pIStream, dwExtra, STREAM_SEEK_CUR );
	}

	// Set the grid time
	m_mtGridStart = iDMStyleMarker.mtGridStart;

	// Already done by the CDirectMusicStyleMarker constructor
	//pTmpDMMarker->m_nTimeOffset = 0;

	// Copy the  DMUS_IO_STYLEMARKER to a CDirectMusicStyleMarker
	if( iDMStyleMarker.wMarkerFlags & DMUS_MARKERF_START )
	{
		m_dwEnterVariation = iDMStyleMarker.dwVariation;

		if( iDMStyleMarker.wMarkerFlags & DMUS_MARKERF_CHORD_ALIGN )
		{
			m_dwEnterChordVariation = m_dwEnterVariation;
		}
	}


	if( iDMStyleMarker.wMarkerFlags & DMUS_MARKERF_STOP )
	{
		m_dwExitVariation = iDMStyleMarker.dwVariation;

		if( iDMStyleMarker.wMarkerFlags & DMUS_MARKERF_CHORD_ALIGN )
		{
			m_dwExitChordVariation = m_dwExitVariation;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// SortEvents

CDirectMusicEventItem* SortEvents( CDirectMusicPart* pDMPart, CDirectMusicEventItem* pEvent, long lLength )
{
	CDirectMusicEventItem* pTop;
	CDirectMusicEventItem* pLeft;
	CDirectMusicEventItem* pRight;
	long lCut;

    if( lLength < 3 )
    {
        if( pEvent == NULL)
		{
            return NULL;
		}

        if( lLength == 1 )
		{
            return pEvent;
		}

        pLeft = pEvent;
        pRight = pEvent->GetNext();

        if( pRight == NULL )
		{
            return pLeft;
		}

        if( pDMPart->AbsTime(pLeft) > pDMPart->AbsTime(pRight) )
        {
            pLeft->SetNext( NULL );
            pRight->SetNext( pLeft );
            return pRight;
        }

        return pLeft;
    }

    lCut   = lLength >> 1;
    pLeft  = pEvent;
    pEvent = (CDirectMusicEventItem *)(pEvent->GetItem( lCut - 1 ));
    pRight = SortEvents( pDMPart, pEvent->GetNext(), (lLength - lCut) );
    pEvent->SetNext( NULL );
    pLeft  = SortEvents( pDMPart, pLeft, lCut );
    pTop   = NULL;

    for( ;  pLeft && pRight ;  )
    {
        if( pDMPart->AbsTime(pLeft) < pDMPart->AbsTime(pRight) )
        {
            if( pTop == NULL )
			{
                pTop = pLeft;
			}
            else
			{
                pEvent->SetNext( pLeft );
			}

            pEvent = pLeft;
            pLeft = pEvent->GetNext();
        }
        else
        {
            if( pTop == NULL )
			{
                pTop = pRight;
			}
            else
			{
                pEvent->SetNext( pRight );
			}

            pEvent = pRight;
            pRight = pEvent->GetNext();
        }
    }

    if( pLeft )
	{
        pEvent->SetNext( pLeft );
	}
    else
	{
        pEvent->SetNext( pRight );
	}

    return pTop;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicNoteList::SortNoteList

void CDirectMusicNoteList::SortNoteList( CDirectMusicPart* pDMPart )
{
	m_pHead = SortEvents( pDMPart, reinterpret_cast<CDirectMusicEventItem*>(m_pHead), GetCount() );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicCurveList::SortCurveList

void CDirectMusicCurveList::SortCurveList( CDirectMusicPart* pDMPart )
{
	m_pHead = SortEvents( pDMPart, reinterpret_cast<CDirectMusicEventItem*>(m_pHead), GetCount() );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicMarkerList::SortMarkerList

void CDirectMusicMarkerList::SortMarkerList( CDirectMusicPart* pDMPart )
{
	m_pHead = SortEvents( pDMPart, reinterpret_cast<CDirectMusicEventItem*>(m_pHead), GetCount() );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicMarkerList::CompactMarkerList

void CDirectMusicMarkerList::CompactMarkerList( void )
{
	CDirectMusicStyleMarker* pMarker = reinterpret_cast<CDirectMusicStyleMarker*>(m_pHead);

	while( pMarker )
	{
		CDirectMusicStyleMarker* pMarkerNext = reinterpret_cast<CDirectMusicStyleMarker*>(pMarker->GetNext());
		if( pMarkerNext && (pMarkerNext->m_mtGridStart == pMarker->m_mtGridStart) )
		{
			// Or the markers together
			pMarker->m_dwEnterVariation |= pMarkerNext->m_dwEnterVariation;
			pMarker->m_dwEnterChordVariation |= pMarkerNext->m_dwEnterChordVariation;
			pMarker->m_dwExitVariation |= pMarkerNext->m_dwExitVariation;
			pMarker->m_dwExitChordVariation |= pMarkerNext->m_dwExitChordVariation;

			// Set pMarker to jump around pMarkerNext
			pMarker->SetNext( pMarkerNext->GetNext() );

			// Delete pMarkerNext
			delete pMarkerNext;

			// Don't go on to next marker - go through again with pMarker
		}
		else
		{
			// Go on to the next marker (either NULL, or on a different grid)
			pMarker = pMarkerNext;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicMarkerList::RemoveMarkerFromVariations

CDirectMusicStyleMarker *CDirectMusicMarkerList::RemoveMarkerFromVariations( CDirectMusicStyleMarker* pDMMarker, DWORD dwVariations )
{
	// Mask out the variations
	pDMMarker->m_dwEnterVariation &= ~dwVariations;
	pDMMarker->m_dwEnterChordVariation &= ~dwVariations;
	pDMMarker->m_dwExitVariation &= ~dwVariations;
	pDMMarker->m_dwExitChordVariation &= ~dwVariations;

	CDirectMusicStyleMarker *pDMMarkerNext = reinterpret_cast<CDirectMusicStyleMarker *>(pDMMarker->GetNext());

	// Check if the marker now belongs to no variations
	if( MARKER_AND_VARIATION(pDMMarker, ALL_VARIATIONS) == 0 )
	{
		// Yes - delete the marker
		Remove( pDMMarker );
		delete pDMMarker;
	}

	return pDMMarkerNext;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart constructor/destructor

CDirectMusicPart::CDirectMusicPart( CMIDIMgr* pMIDIMgr )
{
	ASSERT( pMIDIMgr != NULL );
    
	m_pMIDIMgr = pMIDIMgr;
//	m_pMIDIMgr->AddRef();		intentionally missing

	m_dwUseCount = 0;
	m_fSelecting = FALSE;
	m_fQuantOrVelocitized = FALSE;
	m_bSelectionCC = 0xFF;
	m_wSelectionParamType = 0xFFFF;
	m_fLoadedHasCurveTypes = FALSE;

	
	// Set defaults
	m_TimeSignature = pMIDIMgr->m_TimeSignature;
	m_mtClocksPerBeat = DM_PPQNx4 / (long)m_TimeSignature.m_bBeat;
	m_mtClocksPerGrid = m_mtClocksPerBeat / (long)m_TimeSignature.m_wGridsPerBeat;
	m_wNbrMeasures = 1;
	m_bPlayModeFlags = DMUS_PLAYMODE_PURPLEIZED;			
	m_bInvertLower = 0;			
	m_bInvertUpper = 127;
	m_bAutoInvert = 0;
	m_dwFlags = 0;
	m_dwVariationHasNotes = 0;
	m_dwVariationsDisabled = 0;
	m_bStyleTimeSigChange = 0;

	for( int i = 0 ;  i < 32 ;  i++ )
	{
		m_dwVariationChoices[i] = (DM_VF_MODE_DMUSIC | DM_VF_FLAG_BITS);
	}

	ZeroMemory( m_dwDisabledChoices, sizeof(DWORD) * 32 );
	ZeroMemory( m_bHasCurveTypes, sizeof(BYTE) * 17 );

	CoCreateGuid( &m_guidPartID ); 
}

CDirectMusicPart::~CDirectMusicPart()
{
}

void AdjustEvent( CDirectMusicEventItem* pEvent, DWORD dwOrigGridsPerBeat, DWORD dwNewGridsPerBeat,
				 DWORD dwOrigClocksPerBeat, DWORD dwNewClocksPerBeat,
				 DWORD dwOrigClocksPerGrid, DWORD dwNewClocksPerGrid )
{
	// Store number of grids into the beat
	DWORD dwExtraGrids = (DWORD)pEvent->m_mtGridStart % dwOrigGridsPerBeat;

	// Sync to the beat
	DWORD dwBeat = (DWORD)pEvent->m_mtGridStart / dwOrigGridsPerBeat;
	dwBeat *= dwOrigClocksPerBeat;
	DWORD dwExtraClocks = dwBeat % dwNewClocksPerBeat;
	dwBeat /= dwNewClocksPerBeat;
	pEvent->m_mtGridStart = dwBeat * dwNewGridsPerBeat;

	// Take dwExtraGrids into consideration
	dwExtraGrids *= dwOrigClocksPerGrid;
	dwExtraGrids += dwExtraClocks;
	DWORD dwOffsetClocks = dwExtraGrids % dwNewClocksPerGrid;
	dwExtraGrids /= dwNewClocksPerGrid;
	pEvent->m_mtGridStart = pEvent->m_mtGridStart + dwExtraGrids;

	// Adjust pEvent->m_nTimeOffset
	pEvent->m_nTimeOffset = (short)(pEvent->m_nTimeOffset + (short)dwOffsetClocks);

	// Make pEvent->m_mtGridStart is valid
	ASSERT( pEvent->m_mtGridStart >= 0 ); 
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::SetTimeSignature

void CDirectMusicPart::SetTimeSignature( DirectMusicTimeSig timeSig )
{
	if( timeSig.m_bBeatsPerMeasure != m_TimeSignature.m_bBeatsPerMeasure
	||  timeSig.m_bBeat != m_TimeSignature.m_bBeat
	||  timeSig.m_wGridsPerBeat != m_TimeSignature.m_wGridsPerBeat )
	{
		// Store values based on original time signature
		DWORD dwOrigClocksPerBeat = m_mtClocksPerBeat;
		DWORD dwOrigClocksPerGrid = m_mtClocksPerGrid;
		DWORD dwOrigClocksPerMeasure = dwOrigClocksPerBeat * (DWORD)m_TimeSignature.m_bBeatsPerMeasure;
		DWORD dwOrigLength = dwOrigClocksPerMeasure * m_wNbrMeasures;

		// Change Part time signature
		DirectMusicTimeSig timeSigOrig = m_TimeSignature;
		m_TimeSignature = timeSig;

		// Store values based on new time signature
		m_mtClocksPerBeat = DM_PPQNx4 / (long)m_TimeSignature.m_bBeat;
		m_mtClocksPerGrid = m_mtClocksPerBeat / (long)m_TimeSignature.m_wGridsPerBeat;
		DWORD dwNewClocksPerMeasure = m_mtClocksPerBeat * (long)m_TimeSignature.m_bBeatsPerMeasure;
		
		// Adjust Part length
		m_wNbrMeasures = (WORD)(dwOrigLength / dwNewClocksPerMeasure);
		if( dwOrigLength % dwNewClocksPerMeasure )
		{
			m_wNbrMeasures++;
		}
		DWORD dwNewLength = dwNewClocksPerMeasure * m_wNbrMeasures;
		
		// Adjust event list
		if( m_mtClocksPerBeat != (signed) dwOrigClocksPerBeat
		||  m_TimeSignature.m_wGridsPerBeat != timeSigOrig.m_wGridsPerBeat )  
		{
			DWORD dwMaxGrids = (DWORD)m_wNbrMeasures *
							   (DWORD)m_TimeSignature.m_bBeatsPerMeasure *
							   (DWORD)m_TimeSignature.m_wGridsPerBeat;

			CDirectMusicEventItem* pEvent = m_lstNotes.GetHead();
			while( pEvent )
			{
				AdjustEvent( pEvent, timeSigOrig.m_wGridsPerBeat, m_TimeSignature.m_wGridsPerBeat,
					dwOrigClocksPerBeat, m_mtClocksPerBeat,
					dwOrigClocksPerGrid, m_mtClocksPerGrid );
				ASSERT( pEvent->m_mtGridStart <= (signed)dwMaxGrids ); 
				pEvent = pEvent->GetNext();
			}

			pEvent = m_lstCurves.GetHead();
			while( pEvent )
			{
				AdjustEvent( pEvent, timeSigOrig.m_wGridsPerBeat, m_TimeSignature.m_wGridsPerBeat,
					dwOrigClocksPerBeat, m_mtClocksPerBeat,
					dwOrigClocksPerGrid, m_mtClocksPerGrid );
				ASSERT( pEvent->m_mtGridStart <= (signed)dwMaxGrids ); 

				// Adjust Curve reset duration
				CDirectMusicStyleCurve* pDMCurve = (CDirectMusicStyleCurve *)pEvent;

				if( (pDMCurve->m_bFlags & DMUS_CURVE_RESET)
				&&  (pDMCurve->m_mtResetDuration == (long)dwOrigLength) )
				{
					pDMCurve->m_mtResetDuration = (long)dwNewLength;
				}

				pEvent = pEvent->GetNext();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::SetNbrMeasures

void CDirectMusicPart::SetNbrMeasures( WORD wNbrMeasures )
{
	// Helper function - no need to save undo, refresh, sync, set modified

	if( wNbrMeasures != m_wNbrMeasures )
	{
		long lClocksPerMeasure = m_mtClocksPerBeat * (long)m_TimeSignature.m_bBeatsPerMeasure;
		long lOldPartClockLength = lClocksPerMeasure * (long)m_wNbrMeasures;

		m_wNbrMeasures = wNbrMeasures;
		
		long lNewPartClockLength = lClocksPerMeasure * (long)m_wNbrMeasures; 
		long lNewNbrBeats = (long)m_wNbrMeasures * (long)m_TimeSignature.m_bBeatsPerMeasure; 
		long lNewLastGrid = lNewNbrBeats * (long)m_TimeSignature.m_wGridsPerBeat; 

		CDirectMusicEventItem* pEvent;
		CDirectMusicEventItem* pEventNext = m_lstNotes.GetHead();
		while( pEventNext )
		{
			pEvent = pEventNext;
			pEventNext = pEvent->GetNext();

			// Delete events with start times past end of Part
			if( pEvent->m_mtGridStart >= lNewLastGrid )
			{
				 m_lstNotes.Remove( pEvent );
				 pEvent->m_pNext = NULL;	// To prevent entire list from being deleted
				 delete pEvent;
			}
		}

		pEventNext = m_lstCurves.GetHead();
		while( pEventNext )
		{
			pEvent = pEventNext;
			pEventNext = pEvent->GetNext();

			// Delete events with start times past end of Part
			if( pEvent->m_mtGridStart >= lNewLastGrid )
			{
				 m_lstCurves.Remove( pEvent );
				 pEvent->m_pNext = NULL;	// To prevent entire list from being deleted
				 delete pEvent;
			}
			else
			{
				CDirectMusicStyleCurve* pDMCurve = (CDirectMusicStyleCurve *)pEvent;

				if( pDMCurve->m_bFlags & DMUS_CURVE_RESET )
				{
					if( (pDMCurve->m_mtResetDuration == 0) 
					||  (pDMCurve->m_mtResetDuration == lOldPartClockLength) )
					{
						pDMCurve->m_mtResetDuration = lNewPartClockLength;
					}
				}
			}
		}

		pEventNext = m_lstMarkers.GetHead();
		while( pEventNext )
		{
			pEvent = pEventNext;
			pEventNext = pEvent->GetNext();

			// Delete events with start times past end of Part
			if( pEvent->m_mtGridStart >= lNewLastGrid )
			{
				 m_lstMarkers.Remove( pEvent );
				 pEvent->m_pNext = NULL;	// To prevent entire list from being deleted
				 delete pEvent;
			}
		}

        m_pMIDIMgr->ValidateActiveDMNote( this, ALL_VARIATIONS );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::UpdateHasNotes

BOOL CDirectMusicPart::UpdateHasNotes( void )
{
	DWORD dwNewVariationHasNotes = 0;
	CDirectMusicEventItem *pDMEventItem;

	pDMEventItem = m_lstNotes.GetHead();
	while( pDMEventItem )
	{
		dwNewVariationHasNotes |= pDMEventItem->m_dwVariation;
		pDMEventItem = pDMEventItem->GetNext();
	}

	if (m_dwVariationHasNotes != dwNewVariationHasNotes)
	{
		m_dwVariationHasNotes = dwNewVariationHasNotes;
		return TRUE;
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::Quantize

BOOL CDirectMusicPart::Quantize( DWORD dwFlags, BYTE bStrength, BYTE bGridsPerBeat )
{
	BOOL fChanged = FALSE;

	if( bStrength )
	{
		// Iterate through all events
		// Only quantize notes
		CDirectMusicEventItem* pDMEventItem = m_lstNotes.GetHead();
		while( pDMEventItem )
		{
			if( QuantizeNote( (CDirectMusicStyleNote *)pDMEventItem, dwFlags, bStrength, bGridsPerBeat ) )
			{
				fChanged = TRUE;
			}

			pDMEventItem = pDMEventItem->GetNext();
		}
	}
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::QuantizeNote

BOOL CDirectMusicPart::QuantizeNote( CDirectMusicStyleNote* pDMNote, DWORD dwFlags, BYTE bStrength, BYTE bGridsPerBeat )
{
	BOOL fChanged = FALSE;

	ASSERT( pDMNote );
	ASSERT( bGridsPerBeat );

	if( bStrength )
	{
		// Compute the number of clocks per Measure
		MUSIC_TIME mtMeasureClocks;
		mtMeasureClocks = m_mtClocksPerBeat * m_TimeSignature.m_bBeatsPerMeasure;
		
		if( dwFlags & PATTERN_QUANTIZE_START_TIME )
		{
			// Compute the Measure, Beat, and Grid this note occurs in
			long lTempMeas, lTempBeat, lTempGrid, lTempOffset;
			lTempMeas = pDMNote->m_mtGridStart / (m_TimeSignature.m_wGridsPerBeat * m_TimeSignature.m_bBeatsPerMeasure);
			lTempBeat = pDMNote->m_mtGridStart % (m_TimeSignature.m_wGridsPerBeat * m_TimeSignature.m_bBeatsPerMeasure);
			lTempGrid = lTempBeat % m_TimeSignature.m_wGridsPerBeat;
			lTempBeat /= m_TimeSignature.m_wGridsPerBeat;

			// Compute our offset from lTempMeas and lTempBeat
			lTempOffset = pDMNote->m_nTimeOffset + lTempGrid * m_mtClocksPerGrid;

			BOOL fMeasureBeatMatchesChord = TRUE;
			if( (lTempOffset < 0) || (lTempOffset >= mtMeasureClocks) )
			{
				fMeasureBeatMatchesChord = FALSE;
			}

			// Compute how many clocks in a quantize unit
			MUSIC_TIME mtQuantizeClocks;
			mtQuantizeClocks = m_mtClocksPerBeat / bGridsPerBeat;

			// Compute how many quantize units the offset is away from lTempMeas and lTempBeat
			if( lTempOffset >= 0 )
			{
				lTempOffset = (lTempOffset + mtQuantizeClocks/2) / mtQuantizeClocks;
			}
			else
			{
				lTempOffset = (lTempOffset + -mtQuantizeClocks/2) / mtQuantizeClocks;
			}

			// lTempOffset will now contain the new, 100% quantized offset
			// Use m_mtClocksPerBeat for each beat, mtQuantizeClocks for each grid.
			// (Otherwise, rounding errors may occur).
			lTempOffset = (lTempOffset % bGridsPerBeat) * mtQuantizeClocks +
						  (lTempOffset / bGridsPerBeat) * m_mtClocksPerBeat;

			// lDeltaOffset is the amount for 100% quantization
			long lDeltaOffset = lTempOffset - (pDMNote->m_nTimeOffset + lTempGrid * m_mtClocksPerGrid);

			// Now, scale it based on bStrength
			lDeltaOffset = (lDeltaOffset * bStrength) / 100;

			if( lDeltaOffset )
			{
				fChanged = TRUE;
				// Calculate the new offset from lTempMeas and lTempBeat
				lTempOffset = pDMNote->m_nTimeOffset + lTempGrid * m_mtClocksPerGrid + lDeltaOffset;

				// Calculate the new Bar,Beat,Grid and offset
				lTempGrid = 0;
				if( fMeasureBeatMatchesChord )
				{
					// Move lTempBeat and/or lTempMeasure to match the new location

					// Move by beats
					while( lTempOffset <= -m_mtClocksPerBeat )
					{
						if( lTempBeat )
						{
							lTempBeat--;
						}
						else if( lTempMeas )
						{
							lTempMeas--;
							lTempBeat = m_TimeSignature.m_bBeatsPerMeasure - 1;
						}
						else
						{
							break;
						}
						lTempOffset += m_mtClocksPerBeat;
					}
					while( lTempOffset >= m_mtClocksPerBeat )
					{
						if( lTempBeat < m_TimeSignature.m_bBeatsPerMeasure - 1 )
						{
							lTempBeat++;
						}
						else
						{
							lTempMeas++;
							lTempBeat = 0;
						}
						lTempOffset -= m_mtClocksPerBeat;
					}

					// Move by grids
					while( lTempOffset < 0 )
					{
						if( lTempGrid )
						{
							lTempGrid--;
						}
						else if( lTempBeat )
						{
							lTempBeat--;
							lTempGrid = m_TimeSignature.m_wGridsPerBeat - 1;
						}
						else if ( lTempMeas )
						{
							lTempMeas--;
							lTempBeat = m_TimeSignature.m_bBeatsPerMeasure - 1;
							lTempGrid = m_TimeSignature.m_wGridsPerBeat - 1;
						}
						else
						{
							break;
						}
						lTempOffset += m_mtClocksPerGrid;
					}
					while( lTempOffset >= m_mtClocksPerGrid )
					{
						if( lTempGrid < m_TimeSignature.m_wGridsPerBeat - 1 )
						{
							lTempGrid++;
						}
						else if( lTempBeat < m_TimeSignature.m_bBeatsPerMeasure - 1 )
						{
							lTempBeat++;
							lTempGrid = 0;
						}
						else
						{
							lTempMeas++;
							lTempBeat = 0;
							lTempGrid = 0;
						}
						lTempOffset -= m_mtClocksPerGrid;
					}
				}
				else
				{
					// Don't touch lTempBeat or lTempMeas
					while( (lTempGrid < m_TimeSignature.m_wGridsPerBeat - 1) && (lTempOffset > m_mtClocksPerGrid / 2) )
					{
						lTempGrid++;
						lTempOffset -= m_mtClocksPerGrid;
					}
				}

				long lOldStartTime = AbsTime( pDMNote );

				// Set the note's m_mtGridStart and m_nTimeOffset
				ASSERT( (lTempOffset < SHRT_MAX) && (lTempOffset > SHRT_MIN) );
				pDMNote->m_nTimeOffset = (short)lTempOffset;
				pDMNote->m_mtGridStart = lTempMeas * m_TimeSignature.m_wGridsPerBeat * m_TimeSignature.m_bBeatsPerMeasure +
					lTempBeat * m_TimeSignature.m_wGridsPerBeat + lTempGrid;

				if( lOldStartTime != AbsTime( pDMNote ) )
				{
					// Remove the event from the part's list
					m_lstNotes.Remove( pDMNote );
					// And re-instert it in order
					InsertNoteInAscendingOrder( pDMNote );
				}
			}
		}

		if( dwFlags & PATTERN_QUANTIZE_DURATION )
		{
			// Compute the Measure, Beat, and Grid of the duration
			long lTempMeas, lTempBeat, lTempOffset;
			lTempMeas = pDMNote->m_mtDuration / mtMeasureClocks;
			lTempBeat = (pDMNote->m_mtDuration % mtMeasureClocks) / m_mtClocksPerBeat;

			// Compute our offset from lTempMeas and lTempBeat
			// This is valid because m_mtClocksPerBeat * m_TimeSignature.m_bBeatsPerMeasure is
			// always equal mtMeasureClocks. (No rounding occurs)
			lTempOffset = pDMNote->m_mtDuration % m_mtClocksPerBeat;

			// Compute how many clocks in a quantize unit
			MUSIC_TIME mtQuantizeClocks;
			mtQuantizeClocks = m_mtClocksPerBeat / bGridsPerBeat;

			// Compute how many quantize units the offset is away from lTempMeas and lTempBeat
			lTempOffset = (lTempOffset + mtQuantizeClocks/2) / mtQuantizeClocks;

			// lTempOffset will now contain the new, 100% quantized offset
			if( lTempOffset == bGridsPerBeat )
			{
				// If we're at the next beat, use m_mtClocksPerBeat. (Otherwise rounding
				// errors may occur).
				lTempOffset = m_mtClocksPerBeat;
			}
			else
			{
				lTempOffset *= mtQuantizeClocks;
			}

			// lDeltaOffset is the amount for 100% quantization
			long lDeltaOffset = lTempOffset - (pDMNote->m_mtDuration % m_mtClocksPerBeat);

			// Now, scale it based on bStrength
			lDeltaOffset = (lDeltaOffset * bStrength) / 100;

			if( lDeltaOffset )
			{
				fChanged = TRUE;

				// Set the note's m_mtDuration
				pDMNote->m_mtDuration += lDeltaOffset;
				pDMNote->m_mtDuration = max( pDMNote->m_mtDuration, (mtQuantizeClocks - 1) );
			}
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::Velocitize

BOOL CDirectMusicPart::Velocitize( bool fAbsolute, bool fPercent, long lAbsoluteChangeStart, long lAbsoluteChangeEnd, BYTE bCompressMin, BYTE bCompressMax )
{
	BOOL fChanged = FALSE;

	// The time of the first and last notes
	MUSIC_TIME mtFirstSelected = LONG_MAX;
	MUSIC_TIME mtLastSelected = LONG_MIN;
	MUSIC_TIME mtSelectedSpan = 0;
	const long lAbsChangeSpan = lAbsoluteChangeEnd - lAbsoluteChangeStart;

	// If doing an absolute change and the start and end values are different
	if( fAbsolute
	&&	(lAbsChangeSpan != 0) )
	{
		// Find the time of the first and last selected notes
		CDirectMusicEventItem* pEvent = m_lstNotes.GetHead();
		while( pEvent )
		{
			// Check if this note is earlier than all others, or later than all others
			MUSIC_TIME mtStart = AbsTime( pEvent );
			if( mtStart < mtFirstSelected )
			{
				mtFirstSelected = mtStart;
			}
			if( mtStart > mtLastSelected )
			{
				mtLastSelected = mtStart;
			}

			// Go to the next event
			pEvent = pEvent->GetNext();
		}

		mtSelectedSpan = mtLastSelected - mtFirstSelected;
	}

	// Iterate through all events
	// Only velocitize notes
	CDirectMusicEventItem* pDMEventItem = m_lstNotes.GetHead();
	while( pDMEventItem )
	{
		// Compute the absolute velocity change for this note, if necessary
		long lAbsChange = lAbsoluteChangeStart;
		if( fAbsolute
		&&	(lAbsChangeSpan != 0)
		&&	(mtSelectedSpan != 0) )
		{
			lAbsChange = lAbsoluteChangeStart + ((lAbsChangeSpan * (AbsTime( pDMEventItem ) - mtFirstSelected)) / mtSelectedSpan);
		}

		if( VelocitizeNote( (CDirectMusicStyleNote *)pDMEventItem, fAbsolute, fPercent, lAbsChange, bCompressMin, bCompressMax ) )
		{
			fChanged = TRUE;
		}

		pDMEventItem = pDMEventItem->GetNext();
	}
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::VelocitizeNote

BOOL CDirectMusicPart::VelocitizeNote( CDirectMusicStyleNote* pDMNote, bool fAbsolute, bool fPercent, long lAbsoluteChange, BYTE bCompressMin, BYTE bCompressMax )
{
	BOOL fChanged = FALSE;

	ASSERT( pDMNote );

	long lNewValue = pDMNote->m_bVelocity;
	if( fAbsolute )
	{
		if( lAbsoluteChange != 0 )
		{
			if( fPercent )
			{
				lNewValue = long(pDMNote->m_bVelocity) + (long(pDMNote->m_bVelocity) * lAbsoluteChange) / 100;
				lNewValue = min( 127, max( lNewValue, 1 ) );
				if( BYTE(lNewValue) != pDMNote->m_bVelocity )
				{
					fChanged = TRUE;
					pDMNote->m_bVelocity = BYTE(lNewValue);
				}
			}
			else
			{
				lNewValue = long(pDMNote->m_bVelocity) + lAbsoluteChange;
			}
		}
	}
	else
	{
		lNewValue = min( bCompressMax, max( bCompressMin, pDMNote->m_bVelocity ) );
	}

	lNewValue = min( 127, max( lNewValue, 1 ) );
	if( BYTE(lNewValue) != pDMNote->m_bVelocity )
	{
		fChanged = TRUE;
		pDMNote->m_bVelocity = BYTE(lNewValue);
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_LoadPart

HRESULT CDirectMusicPart::DM_LoadPart( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
    DWORD dwStructSize;
    DWORD dwExtra;

	CDirectMusicNoteList  lstNotes;	
	CDirectMusicCurveList lstCurves;
	CDirectMusicMarkerList lstMarkers;

	ASSERT( m_pMIDIMgr != NULL );

	m_fLoadedHasCurveTypes = FALSE;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_PART_CHUNK:
			{
				DMUS_IO_STYLEPART iDMStylePart;
				ZeroMemory( &iDMStylePart, sizeof( DMUS_IO_STYLEPART ) );

				dwSize = min( ck.cksize, sizeof( DMUS_IO_STYLEPART ) );
				hr = pIStream->Read( &iDMStylePart, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( m_pMIDIMgr->FindPartByGUID( iDMStylePart.guidPartID ) )
				{
					hr = S_FALSE;	// Style already contains a Part with this GUID
					goto ON_ERROR;	// so we must bypass this Part
				}

				m_TimeSignature.m_bBeatsPerMeasure = iDMStylePart.timeSig.bBeatsPerMeasure;
				m_TimeSignature.m_bBeat = iDMStylePart.timeSig.bBeat;
				m_TimeSignature.m_wGridsPerBeat = iDMStylePart.timeSig.wGridsPerBeat;

				m_mtClocksPerBeat = DM_PPQNx4 / (long)m_TimeSignature.m_bBeat;
				m_mtClocksPerGrid = m_mtClocksPerBeat / (long)m_TimeSignature.m_wGridsPerBeat;

				m_wNbrMeasures = iDMStylePart.wNbrMeasures;
				m_bInvertUpper = iDMStylePart.bInvertUpper;
				m_bInvertLower = iDMStylePart.bInvertLower;
				m_bPlayModeFlags = iDMStylePart.bPlayModeFlags;
				m_dwFlags = iDMStylePart.dwFlags;

				memcpy( &m_guidPartID, &iDMStylePart.guidPartID, sizeof(GUID) );
				memcpy( &m_dwVariationChoices, &iDMStylePart.dwVariationChoices, sizeof(m_dwVariationChoices) );
				break;
			}

			case DMUS_FOURCC_NOTE_CHUNK:
			{
				dwSize = ck.cksize;

				// Read size of the note structure
				hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != sizeof( dwStructSize ) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwByteCount;

				// Check if there is any extra data to skip over
				if( dwStructSize > sizeof( DMUS_IO_STYLENOTE ) )
				{
					dwExtra = dwStructSize - sizeof( DMUS_IO_STYLENOTE );
					dwStructSize = sizeof( DMUS_IO_STYLENOTE );
				}
				else
				{
					dwExtra = 0;
				}

				// now read in the notes
				DMUS_IO_STYLENOTE iDMStyleNote;
				while( dwSize > 0 )
				{
					ZeroMemory( &iDMStyleNote, sizeof( DMUS_IO_STYLENOTE ) );
					hr = pIStream->Read( &iDMStyleNote, dwStructSize, &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != dwStructSize )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
					dwSize -= dwStructSize;

					if( dwExtra > 0 )
					{
						StreamSeek( pIStream, dwExtra, STREAM_SEEK_CUR );
						dwSize -= dwExtra;
					}

					// Create Direct Music Note
					CDirectMusicStyleNote* pNote;
					pNote = new CDirectMusicStyleNote( iDMStyleNote );
					if( pNote )
					{
						// Place note in Part's event list
						lstNotes.AddHead( pNote );
					}
				}
				break;
			}

			case DMUS_FOURCC_CURVE_CHUNK:
			{
				dwSize = ck.cksize;

				// Read size of the curve structure
				hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != sizeof( dwStructSize ) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwByteCount;

				// Check if there is any extra data to skip over
				if( dwStructSize > sizeof( DMUS_IO_STYLECURVE ) )
				{
					dwExtra = dwStructSize - sizeof( DMUS_IO_STYLECURVE );
					dwStructSize = sizeof( DMUS_IO_STYLECURVE );
				}
				else
				{
					dwExtra = 0;
				}

				// now read in the curves
				DMUS_IO_STYLECURVE iDMStyleCurve;
				while( dwSize > 0 )
				{
					ZeroMemory( &iDMStyleCurve, sizeof( DMUS_IO_STYLECURVE ) );
					hr = pIStream->Read( &iDMStyleCurve, dwStructSize, &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != dwStructSize )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
					dwSize -= dwStructSize;

					if( dwExtra > 0 )
					{
						StreamSeek( pIStream, dwExtra, STREAM_SEEK_CUR );
						dwSize -= dwExtra;
					}

					// Create Direct Music Curve
					CDirectMusicStyleCurve* pCurve;
					pCurve = new CDirectMusicStyleCurve;
					if( pCurve )
					{
						pCurve->m_mtGridStart = iDMStyleCurve.mtGridStart;
						pCurve->m_dwVariation = iDMStyleCurve.dwVariation;
						pCurve->m_mtDuration = iDMStyleCurve.mtDuration;
						pCurve->m_mtResetDuration = iDMStyleCurve.mtResetDuration;
						pCurve->m_nTimeOffset = iDMStyleCurve.nTimeOffset;

						pCurve->m_nStartValue = iDMStyleCurve.nStartValue;
						pCurve->m_nEndValue = iDMStyleCurve.nEndValue;
						pCurve->m_nResetValue = iDMStyleCurve.nResetValue;
						pCurve->m_bEventType = iDMStyleCurve.bEventType;
						pCurve->m_bCurveShape = iDMStyleCurve.bCurveShape;
						pCurve->m_bCCData = iDMStyleCurve.bCCData;
						pCurve->m_bFlags = iDMStyleCurve.bFlags;
						pCurve->m_wParamType = FILE_TO_MEMORY_WPARAMTYPE( iDMStyleCurve.wParamType );
						pCurve->m_wMergeIndex = iDMStyleCurve.wMergeIndex;

						// Place curve in Part's event list
						lstCurves.AddHead( pCurve );
					}
				}
				break;
			}

			case DMUS_FOURCC_MARKER_CHUNK:
			{
				dwSize = ck.cksize;

				// Read size of the marker structure
				hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != sizeof( dwStructSize ) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwByteCount;

				// Check if there is any extra data to skip over
				if( dwStructSize > sizeof( DMUS_IO_STYLEMARKER ) )
				{
					dwExtra = dwStructSize - sizeof( DMUS_IO_STYLEMARKER );
					dwStructSize = sizeof( DMUS_IO_STYLEMARKER );
				}
				else
				{
					dwExtra = 0;
				}

				// now read in the markers
				while( dwSize >= dwStructSize )
				{
					// Create Direct Music marker
					CDirectMusicStyleMarker* pMarker;
					pMarker = new CDirectMusicStyleMarker;
					if( pMarker )
					{
						if( FAILED( pMarker->Read( pIStream, dwStructSize, dwExtra ) ) )
						{
							hr = E_FAIL;
							goto ON_ERROR;
						}

						dwSize -= dwStructSize;
						dwSize -= dwExtra;

						// Place marker in Part's event list
						lstMarkers.AddHead( pMarker );
					}
				}
				break;
			}

			case DMUS_FOURCC_PART_DESIGN:
			{
				ioDMStylePartDesign iDMStylePartDesign;
				ZeroMemory( &iDMStylePartDesign, sizeof( ioDMStylePartDesign ) );

				dwSize = min( ck.cksize, sizeof( ioDMStylePartDesign ) );
				hr = pIStream->Read( &iDMStylePartDesign, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_bAutoInvert = iDMStylePartDesign.m_bAutoInvert;
				m_dwVariationsDisabled = iDMStylePartDesign.m_dwVariationsDisabled;
				m_bStyleTimeSigChange = iDMStylePartDesign.m_bStyleTimeSigChange;

				memcpy( m_dwDisabledChoices, iDMStylePartDesign.m_dwDisabledChoices, sizeof(m_dwDisabledChoices) );
				memcpy( m_bHasCurveTypes, iDMStylePartDesign.m_bHasCurveTypes, sizeof(m_bHasCurveTypes) );


				if( ck.cksize >= sizeof( ioDMStylePartDesign ) )
				{
					m_fLoadedHasCurveTypes = TRUE;
				}
				break;
			}
			case FOURCC_LIST:
			{
				MMCKINFO ckList;
				switch( ck.fccType )
				{
					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
			}
		}

        pIRiffStream->Ascend( &ck, 0 );
	}

	CDirectMusicEventItem* pEvent;

	// Transfer lstNotes to m_lstNotes
	ASSERT( m_lstNotes.GetHead() == NULL );		// Part note list should be empty
	pEvent = lstNotes.RemoveHead();
	while( pEvent )
	{
		m_lstNotes.AddHead( pEvent );
		pEvent = lstNotes.RemoveHead();
	}
	m_lstNotes.SortNoteList( this );

	// Transfer lstCurves to m_lstCurves
	ASSERT( m_lstCurves.GetHead() == NULL );		// Part curve list should be empty
	pEvent = lstCurves.RemoveHead();
	while( pEvent )
	{
		m_lstCurves.AddHead( pEvent );
		pEvent = lstCurves.RemoveHead();
	}
	m_lstCurves.SortCurveList( this );

	// Transfer lstMarkers to m_lstMarkers
	ASSERT( m_lstMarkers.GetHead() == NULL );		// Part marker list should be empty
	pEvent = lstMarkers.RemoveHead();
	while( pEvent )
	{
		m_lstMarkers.AddHead( pEvent );
		pEvent = lstMarkers.RemoveHead();
	}
	m_lstMarkers.SortMarkerList( this );
	m_lstMarkers.CompactMarkerList();

	UpdateHasNotes();

	int i;
	for( i=0; i<32; i++ )
	{
		if( (m_dwVariationChoices[i] & DM_VF_FLAG_BITS) == 0 )
		{
			if( !(m_dwVariationsDisabled & (1<<i)) )
			{
				m_dwVariationsDisabled |= (1<<i);
				m_dwDisabledChoices[i] = m_dwVariationChoices[i] | DM_VF_FLAG_BITS;
			}
		}
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SavePart

HRESULT CDirectMusicPart::DM_SavePart( IDMUSProdRIFFStream* pIRiffStream ) const
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_STYLEPART oDMStylePart;

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pPersistInfo->Release();
		pPersistInfo = NULL;
	}

	// Write Part list header
	ckMain.fccType = DMUS_FOURCC_PART_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Part chunk header
	ck.ckid = DMUS_FOURCC_PART_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_STYLEPART structure
	memset( &oDMStylePart, 0, sizeof(DMUS_IO_STYLEPART) );

	oDMStylePart.timeSig.bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
	oDMStylePart.timeSig.bBeat = m_TimeSignature.m_bBeat;
	oDMStylePart.timeSig.wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;

	oDMStylePart.wNbrMeasures = m_wNbrMeasures;
	oDMStylePart.bInvertUpper = m_bInvertUpper;
	oDMStylePart.bInvertLower = m_bInvertLower;
	oDMStylePart.bPlayModeFlags = m_bPlayModeFlags;
	if( m_lstMarkers.IsEmpty() )
	{
		oDMStylePart.dwFlags = m_dwFlags & ~DMUS_PARTF_USE_MARKERS;
	}
	else
	{
		oDMStylePart.dwFlags = m_dwFlags | DMUS_PARTF_USE_MARKERS;
	}

	memcpy( &oDMStylePart.guidPartID, &m_guidPartID, sizeof(GUID) );
	memcpy( &oDMStylePart.dwVariationChoices, &m_dwVariationChoices, sizeof(m_dwVariationChoices) );

	// Write Part chunk data
	hr = pIStream->Write( &oDMStylePart, sizeof(DMUS_IO_STYLEPART), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_STYLEPART) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save part's Info chunk
	hr = DM_SavePartInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

#ifdef _DEBUG
	// Ensure the list is in order
	CDirectMusicEventItem* pEvent;
	pEvent = m_lstNotes.GetHead();
	while( pEvent )
	{
		if( pEvent->GetNext() )
		{
			ASSERT( AbsTime(pEvent) <= AbsTime(pEvent->GetNext()) );
		}
		pEvent = pEvent->GetNext();
	}

	// Ensure the list is in order
	pEvent = m_lstCurves.GetHead();
	while( pEvent )
	{
		if( pEvent->GetNext() )
		{
			ASSERT( AbsTime(pEvent) <= AbsTime(pEvent->GetNext()) );
		}
		pEvent = pEvent->GetNext();
	}

	// Ensure the list is in order
	pEvent = m_lstMarkers.GetHead();
	while( pEvent )
	{
		if( pEvent->GetNext() )
		{
			ASSERT( AbsTime(pEvent) <= AbsTime(pEvent->GetNext()) );
		}
		pEvent = pEvent->GetNext();
	}
#endif

	// Save Part note list (when applicable)
	if( m_lstNotes.GetHead() )
	{
		hr = DM_SaveNoteList( pIRiffStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

	// Save Part curve list (when applicable)
	if( m_lstCurves.GetHead() )
	{
		hr = DM_SaveCurveList( pIRiffStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

	// Save Marker list (when applicable)
	if( m_lstMarkers.GetHead() )
	{
		hr = DM_SaveMarkerList( pIRiffStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

	if( StreamInfo.ftFileType == FT_DESIGN )
	{
		// Save Part design info
		hr = DM_SaveDesignInfo( pIRiffStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SaveNoteList

HRESULT CDirectMusicPart::DM_SaveNoteList( IDMUSProdRIFFStream* pIRiffStream ) const
{
	CDirectMusicEventItem* pEvent;
	CDirectMusicStyleNote* pNote;
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwStructSize;
	DMUS_IO_STYLENOTE oDMStyleNote;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Note chunk header
	ck.ckid = DMUS_FOURCC_NOTE_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save size of DMUS_IO_STYLENOTE structure
	dwStructSize = sizeof(DMUS_IO_STYLENOTE);
	hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Now save all of the notes
	pEvent = m_lstNotes.GetHead();
	while( pEvent )
	{
		pNote = (CDirectMusicStyleNote *)pEvent;
		
		// Prepare DMUS_IO_STYLENOTE structure
		memset( &oDMStyleNote, 0, sizeof(DMUS_IO_STYLENOTE) );

		oDMStyleNote.mtGridStart = pNote->m_mtGridStart;
		oDMStyleNote.dwVariation = pNote->m_dwVariation;
		oDMStyleNote.nTimeOffset = pNote->m_nTimeOffset;	

		oDMStyleNote.mtDuration = pNote->m_mtDuration;
		oDMStyleNote.wMusicValue = pNote->m_wMusicValue;	
		oDMStyleNote.bVelocity = pNote->m_bVelocity;	
		oDMStyleNote.bTimeRange = pNote->m_bTimeRange;	
		oDMStyleNote.bDurRange = pNote->m_bDurRange;	
		oDMStyleNote.bVelRange = pNote->m_bVelRange;	
		oDMStyleNote.bInversionID = pNote->m_bInversionId;	
		oDMStyleNote.bPlayModeFlags = pNote->m_bPlayModeFlags;
		oDMStyleNote.bNoteFlags = pNote->m_bNoteFlags;

		// Write DMUS_IO_STYLENOTE structure
		hr = pIStream->Write( &oDMStyleNote, sizeof(DMUS_IO_STYLENOTE), &dwBytesWritten );
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(DMUS_IO_STYLENOTE) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		pEvent = pEvent->GetNext();
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SaveDesignInfo

HRESULT CDirectMusicPart::DM_SaveDesignInfo( IDMUSProdRIFFStream* pIRiffStream ) const
{
    HRESULT hr = S_OK;
 
	IStream* pIStream;
    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_PART_DESIGN;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioDMStylePartDesign structure
	ioDMStylePartDesign oDMPartDesign;
	memset( &oDMPartDesign, 0, sizeof(ioDMStylePartDesign) );

	oDMPartDesign.m_bAutoInvert = m_bAutoInvert;
	oDMPartDesign.m_dwVariationsDisabled = m_dwVariationsDisabled;	
	oDMPartDesign.m_bStyleTimeSigChange = m_bStyleTimeSigChange;	

	memcpy( oDMPartDesign.m_dwDisabledChoices, m_dwDisabledChoices, sizeof(oDMPartDesign.m_dwDisabledChoices) );
	memcpy( oDMPartDesign.m_bHasCurveTypes, m_bHasCurveTypes, sizeof(oDMPartDesign.m_bHasCurveTypes) );

	// Write PartDesign chunk data
	DWORD dwBytesWritten;
	hr = pIStream->Write( &oDMPartDesign, sizeof(ioDMStylePartDesign), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioDMStylePartDesign) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
ON_ERROR:
    pIStream->Release();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SavePartInfoList

HRESULT CDirectMusicPart::DM_SavePartInfoList( IDMUSProdRIFFStream* pIRiffStream ) const
{
	if( m_strName.IsEmpty() )
	{
		return S_OK;
	}

	IStream* pIStream;
    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	HRESULT hr = S_OK;
    MMCKINFO ckMain;
    MMCKINFO ck;

	// Write INFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Part name
	if( !m_strName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SaveCurveList

HRESULT CDirectMusicPart::DM_SaveCurveList( IDMUSProdRIFFStream* pIRiffStream ) const
{
	CDirectMusicEventItem* pEvent;
	CDirectMusicStyleCurve* pCurve;
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwStructSize;
	DMUS_IO_STYLECURVE oDMStyleCurve;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	ck.ckid = DMUS_FOURCC_CURVE_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save size of DMUS_IO_STYLECURVE structure
	dwStructSize = sizeof(DMUS_IO_STYLECURVE);
	hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Now save all of the curves
	pEvent = m_lstCurves.GetHead();
	while( pEvent )
	{
		pCurve = (CDirectMusicStyleCurve *)pEvent;
		
		// Prepare DMUS_IO_STYLECURVE structure
		memset( &oDMStyleCurve, 0, sizeof(DMUS_IO_STYLECURVE) );

		oDMStyleCurve.mtGridStart = pCurve->m_mtGridStart;
		oDMStyleCurve.dwVariation = pCurve->m_dwVariation;
		oDMStyleCurve.nTimeOffset = pCurve->m_nTimeOffset;	

		oDMStyleCurve.mtDuration = pCurve->m_mtDuration;
		oDMStyleCurve.mtResetDuration = pCurve->m_mtResetDuration;
		oDMStyleCurve.nStartValue = pCurve->m_nStartValue;	
		oDMStyleCurve.nEndValue = pCurve->m_nEndValue;	
		oDMStyleCurve.nResetValue = pCurve->m_nResetValue;	
		oDMStyleCurve.bEventType = pCurve->m_bEventType;	
		oDMStyleCurve.bCurveShape = pCurve->m_bCurveShape;	
		oDMStyleCurve.bCCData = pCurve->m_bCCData;	
		oDMStyleCurve.bFlags = pCurve->m_bFlags;	
		oDMStyleCurve.wParamType = MEMORY_TO_FILE_WPARAMTYPE( pCurve->m_wParamType );
		oDMStyleCurve.wMergeIndex = pCurve->m_wMergeIndex;

		// Write DMUS_IO_STYLECURVE structure
		hr = pIStream->Write( &oDMStyleCurve, sizeof(DMUS_IO_STYLECURVE), &dwBytesWritten );
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(DMUS_IO_STYLECURVE) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		pEvent = pEvent->GetNext();
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SaveMarkerList

HRESULT CDirectMusicPart::DM_SaveMarkerList( IDMUSProdRIFFStream* pIRiffStream ) const
{
    MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_MARKER_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Save size of DMUS_IO_STYLEMARKER structure
	DWORD dwBytesWritten, dwStructSize = sizeof(DMUS_IO_STYLEMARKER);
	HRESULT hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Now save all the markers
	CDirectMusicStyleMarker* pDMMarker;
	pDMMarker = m_lstMarkers.GetHead();
	while( pDMMarker )
	{
		if( FAILED( pDMMarker->Write( pIStream ) ) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Get pointer to next marker
		pDMMarker = reinterpret_cast<CDirectMusicStyleMarker*>(pDMMarker->GetNext());
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::CopyTo

void CDirectMusicPart::CopyTo( CDirectMusicPart *pDMPart ) const
{
	ASSERT( pDMPart );
	
	pDMPart->m_bAutoInvert = m_bAutoInvert;
	pDMPart->m_bInvertLower = m_bInvertLower;
	pDMPart->m_bInvertUpper = m_bInvertUpper;
	pDMPart->m_bPlayModeFlags = m_bPlayModeFlags;
	memcpy( pDMPart->m_dwDisabledChoices, m_dwDisabledChoices, sizeof(DWORD) * 32 );
	memcpy( pDMPart->m_bHasCurveTypes, m_bHasCurveTypes, sizeof(BYTE) * 17 );
	memcpy( pDMPart->m_dwVariationChoices, m_dwVariationChoices, sizeof(DWORD) * 32 );
	pDMPart->m_dwVariationHasNotes = m_dwVariationHasNotes;
	pDMPart->m_dwVariationsDisabled = m_dwVariationsDisabled;
	memcpy( &pDMPart->m_guidPartID, &m_guidPartID, sizeof(GUID) );
	pDMPart->m_strName = m_strName;
	pDMPart->m_TimeSignature = m_TimeSignature;
	pDMPart->m_mtClocksPerBeat = DM_PPQNx4 / (long)pDMPart->m_TimeSignature.m_bBeat;
	pDMPart->m_mtClocksPerGrid = m_mtClocksPerBeat / (long)pDMPart->m_TimeSignature.m_wGridsPerBeat;
	pDMPart->m_wNbrMeasures = m_wNbrMeasures;
	pDMPart->m_dwFlags = m_dwFlags;

	// Copy the list of events
	CDirectMusicEventItem* pItem;
	pItem = m_lstNotes.GetHead();
	while( pItem )
	{
		CDirectMusicStyleNote* pNote = new CDirectMusicStyleNote( (CDirectMusicStyleNote *)pItem );
		if( pNote )
		{
			pDMPart->InsertNoteInAscendingOrder( pNote );
		}

		pItem = pItem->GetNext();
	}

	pItem = m_lstCurves.GetHead();
	while( pItem )
	{
		CDirectMusicStyleCurve* pCurve = new CDirectMusicStyleCurve;
		*pCurve = *(CDirectMusicStyleCurve *)pItem;
		pDMPart->InsertCurveInAscendingOrder( pCurve );

		pItem = pItem->GetNext();
	}

	pItem = m_lstMarkers.GetHead();
	while( pItem )
	{
		CDirectMusicStyleMarker* pMarker = new CDirectMusicStyleMarker;
		*pMarker = *(CDirectMusicStyleMarker *)pItem;
		pDMPart->InsertMarkerInAscendingOrder( pMarker );

		pItem = pItem->GetNext();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetNumSelected

int CDirectMusicPart::GetNumSelected( BYTE bType, DWORD dwVariations ) const
{
	int nSelected = 0;

	// Search for all matching events
	if( bType == ET_NOTE )
	{
		CDirectMusicEventItem* pItem;
		pItem = m_lstNotes.GetHead();
		while( pItem )
		{
			if( pItem->m_fSelected && (pItem->m_dwVariation & dwVariations) )
			{
				nSelected++;
			}

			pItem = pItem->GetNext();
		}
	}
	else if( bType == ET_CURVE )
	{
		CDirectMusicEventItem* pItem;
		pItem = m_lstCurves.GetHead();
		while( pItem )
		{
			if( pItem->m_fSelected && (pItem->m_dwVariation & dwVariations) )
			{
				nSelected++;
			}

			pItem = pItem->GetNext();
		}
	}
	else if( bType == ET_MARKER )
	{
		CDirectMusicStyleMarker* pItem;
		pItem = m_lstMarkers.GetHead();
		while( pItem )
		{
			if( pItem->m_fSelected
			&&	MARKER_AND_VARIATION(pItem, dwVariations) )
			{
				nSelected++;
			}

			pItem = reinterpret_cast<CDirectMusicStyleMarker*>(pItem->GetNext());
		}
	}
	else
	{
		TRACE("CDirectMusicPart::GetNumSelected: Unknown Type asked for\n");
		ASSERT(FALSE);
	}

	return nSelected;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::InsertNoteInAscendingOrder

void CDirectMusicPart::InsertNoteInAscendingOrder( CDirectMusicStyleNote *pNote )
{
	ASSERT( pNote->m_mtGridStart >= 0 );
	CDirectMusicEventItem *pList = m_lstNotes.GetHead();
	MUSIC_TIME mtTime = AbsTime(pNote);
	for( ; pList && (AbsTime(pList) < mtTime); pList = pList->GetNext() );

	if( pList )
	{
		m_lstNotes.InsertBefore( pList, pNote );
	}
	else
	{
		// Not always necessary, but it will fix the strange problems that occur when it is not NULL
		pNote->SetNext( NULL );

		m_lstNotes.AddTail( pNote );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::InsertCurveInAscendingOrder

void CDirectMusicPart::InsertCurveInAscendingOrder( CDirectMusicStyleCurve *pCurve )
{
	ASSERT( pCurve->m_mtGridStart >= 0 );
	CDirectMusicEventItem *pList = m_lstCurves.GetHead();
	MUSIC_TIME mtTime = AbsTime(pCurve);
	for( ; pList && (AbsTime(pList) < mtTime); pList = pList->GetNext() );

	if( pList )
	{
		m_lstCurves.InsertBefore( pList, pCurve );
	}
	else
	{
		// Not always necessary, but it will fix the strange problems that occur when it is not NULL
		pCurve->SetNext( NULL );

		m_lstCurves.AddTail( pCurve );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::InsertMarkerInAscendingOrder

void CDirectMusicPart::InsertMarkerInAscendingOrder( CDirectMusicStyleMarker *pMarker )
{
	// Ensure the grid time of the marker is positive
	ASSERT( pMarker->m_mtGridStart >= 0 );

	// Get a pointer to the head of the list
	CDirectMusicEventItem *pList = m_lstMarkers.GetHead();

	// Save the grid time of the marker to insert
	MUSIC_TIME mtGrid = pMarker->m_mtGridStart;

	// Search through the list until we find a marker that is after mtGrid
	for( ; pList; pList = pList->GetNext() )
	{
		// Check if this marker is later than the one we're inserting
		if( pList->m_mtGridStart > mtGrid )
		{
			// Yes - insert pMarker before pList
			break;
		}
		// Check if this marker is on the same grid
		else if( pList->m_mtGridStart == mtGrid )
		{
			// This should only happen in CMIDIMgr::ImportMarkerList

			// Yes - insert pMarker before pList
			break;
		}
	}

	if( pList )
	{
		m_lstMarkers.InsertBefore( pList, pMarker );
	}
	else
	{
		// Not always necessary, but it will fix the strange problems that occur when it is not NULL
		pMarker->SetNext( NULL );

		m_lstMarkers.AddTail( pMarker );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::CreateNoteCopyIfNeeded

void CDirectMusicPart::CreateNoteCopyIfNeeded( CDirectMusicStyleNote *pOrigNote, DWORD dwVariations, BOOL fMakeCopy )
{
	// If we should copy the note
	if( fMakeCopy )
	{
		// Create a new note (to store the existing note in)
		CDirectMusicStyleNote *pDMNote = new CDirectMusicStyleNote( pOrigNote );

		if( pDMNote )
		{
			pDMNote->SetNext( NULL );

			// Insert the new note into the part
			InsertNoteInAscendingOrder( pDMNote );

			// Make the existing note belong to only the variations we change (it's the note we will change)
			pOrigNote->m_dwVariation &= dwVariations;

			// The existing (new) note is now unselected, while the old (note to change) retains its selection state
			pDMNote->m_fSelected = FALSE;
		}
	}
	// If the event belongs to other variations...
	else if ( pOrigNote->m_dwVariation & ~dwVariations )
	{
		// Create a new note (to store the existing note in)
		CDirectMusicStyleNote *pDMNote = new CDirectMusicStyleNote( pOrigNote );

		if( pDMNote )
		{
			pDMNote->SetNext( NULL );

			// Insert the new note into the part
			InsertNoteInAscendingOrder( pDMNote );

			// Make the existing note belong to only the variations we change (it's the note we will change)
			pOrigNote->m_dwVariation &= dwVariations;

			// Make the new note belong to all other variations
			pDMNote->m_dwVariation &= ~dwVariations;

			// The 'other variation' (new) note is now unselected, while the selected variations (note to change)
			// retain their original selection state
			pDMNote->m_fSelected = FALSE;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::CreateCurveCopyIfNeeded

void CDirectMusicPart::CreateCurveCopyIfNeeded( CDirectMusicStyleCurve *pOrigCurve, DWORD dwVariations, BOOL fMakeCopy )
{
	// If we should copy the curve
	if( fMakeCopy )
	{
		// Create a new curve (to store the existing note curve in)
		CDirectMusicStyleCurve *pDMCurve = new CDirectMusicStyleCurve;

		// Copy the existing curve to it
		*pDMCurve = *pOrigCurve;
		pDMCurve->SetNext( NULL );

		// Insert the new curve into the part
		InsertCurveInAscendingOrder( pDMCurve );

		// Make the existing curve belong to only the variations we change (it's the curve we will change)
		pOrigCurve->m_dwVariation &= dwVariations;

		// The existing (new) curve is now unselected, while the old (curve to change) is still selected
		pDMCurve->m_fSelected = FALSE;
	}
	// If the event belongs to other variations...
	else if ( pOrigCurve->m_dwVariation & ~dwVariations )
	{
		// Create a new curve (to store the existing curve in)
		CDirectMusicStyleCurve *pDMCurve = new CDirectMusicStyleCurve;

		// Copy the existing curve to it
		*pDMCurve = *pOrigCurve;
		pDMCurve->SetNext( NULL );

		// Insert the new curve into the part
		InsertCurveInAscendingOrder( pDMCurve );

		// Make the existing curve belong to only the variations we change (it's the curve we will change)
		pOrigCurve->m_dwVariation &= dwVariations;

		// Make the new curve belong to all other variations
		pDMCurve->m_dwVariation &= ~dwVariations;

		// The 'other variation' (new) curve is now unselected, while the selected variations (curve to change)
		// are still selected
		pDMCurve->m_fSelected = FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::MergeNoteList

void CDirectMusicPart::MergeNoteList( CDirectMusicNoteList *plstEvents )
{
	// We assume lstEvents is in sorted (ascending) order
	if( !plstEvents->GetHead() )
	{
		return;
	}

	// Get the first element in our list
	CDirectMusicEventItem* pEvent = m_lstNotes.GetHead();

	// Continue while the merge sequence list is not empty
	while( plstEvents->GetHead() )
	{
		CDirectMusicEventItem *pMergeEvent = plstEvents->RemoveHead();

		while( pEvent && (AbsTime(pMergeEvent) > AbsTime(pEvent)) )
		{
			pEvent = pEvent->GetNext();
		}

		if( pEvent )
		{
			m_lstNotes.InsertBefore( pEvent, pMergeEvent );
		}
		else
		{
			pMergeEvent->SetNext( NULL );
			m_lstNotes.AddTail( pMergeEvent );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::MergeCurveList

void CDirectMusicPart::MergeCurveList( CDirectMusicCurveList *plstEvents )
{
	// We assume lstEvents is in sorted (ascending) order
	if( !plstEvents->GetHead() )
	{
		return;
	}

	// Must sort list because curves are not always in order!
	plstEvents->SortCurveList( this );

	// Get the first element in our list
	CDirectMusicEventItem* pEvent = m_lstCurves.GetHead();
	long lEventTime = AbsTime( pEvent );

	// Continue while the merge sequence list is not empty
	while( plstEvents->GetHead() )
	{
		CDirectMusicEventItem *pMergeEvent = plstEvents->RemoveHead();
		long lMergeEventTime = AbsTime( pMergeEvent );

		while( pEvent && (lMergeEventTime > lEventTime) )
		{
			pEvent = pEvent->GetNext();
			lEventTime = AbsTime( pEvent );
		}

		if( pEvent )
		{
			m_lstCurves.InsertBefore( pEvent, pMergeEvent );
		}
		else
		{
			pMergeEvent->SetNext( NULL );
			m_lstCurves.AddTail( pMergeEvent );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetFirstNote

CDirectMusicStyleNote *CDirectMusicPart::GetFirstNote( DWORD dwVariation ) const
{
	// Get the first element in our list
	CDirectMusicEventItem* pEvent = m_lstNotes.GetHead();

	// Search the list for the first event that matches the dwVariation flags
	while( pEvent )
	{
		if( pEvent->m_dwVariation & dwVariation )
		{
			return (CDirectMusicStyleNote*)pEvent;
		}

		pEvent = pEvent->GetNext();
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetFirstCurve

CDirectMusicStyleCurve *CDirectMusicPart::GetFirstCurve( DWORD dwVariation ) const
{
	// Get the first element in our list
	CDirectMusicEventItem* pEvent = m_lstCurves.GetHead();

	// Search the list for the first event that matches the dwVariation flags
	while( pEvent )
	{
		if( pEvent->m_dwVariation & dwVariation )
		{
			return (CDirectMusicStyleCurve*)pEvent;
		}

		pEvent = pEvent->GetNext();
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetFirstMarker

CDirectMusicStyleMarker *CDirectMusicPart::GetFirstMarker( DWORD dwVariation ) const
{
	// Get the first element in our list
	CDirectMusicStyleMarker* pEvent = m_lstMarkers.GetHead();

	// Search the list for the first event that matches the dwVariation flags
	while( pEvent )
	{
		if( MARKER_AND_VARIATION(pEvent, dwVariation) )
		{
			return (CDirectMusicStyleMarker*)pEvent;
		}

		pEvent = reinterpret_cast<CDirectMusicStyleMarker*>(pEvent->GetNext());
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetLastNote

CDirectMusicStyleNote *CDirectMusicPart::GetLastNote( DWORD dwVariation ) const
{
	// Get the first element in our list
	CDirectMusicEventItem* pEvent = m_lstNotes.GetHead();

	// Store the last event here
	CDirectMusicEventItem* pEventLast = NULL;

	// Search the list for the last event that matches the dwVariation flags
	while( pEvent )
	{
		if( pEvent->m_dwVariation & dwVariation )
		{
			pEventLast = pEvent;
		}

		pEvent = pEvent->GetNext();
	}

	return (CDirectMusicStyleNote*)pEventLast;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::AddCurveTypesFromData

void CDirectMusicPart::AddCurveTypesFromData( void )
{
	//ZeroMemory( pPart->m_bHasCurveTypes, sizeof(BYTE) * 17 );

	// Update m_bHasCurveTypes to reflect which curve types we actually have
	CDirectMusicEventItem* pDMEvent = m_lstCurves.GetHead();
	while( pDMEvent )
	{
		BYTE bCCType = CPianoRollStrip::CurveTypeToStripCCType( (CDirectMusicStyleCurve *)pDMEvent );
		ASSERT( bCCType < 17 * 8 );
		m_bHasCurveTypes[ bCCType>>3 ] |= 1 << (bCCType & 0x07);

		pDMEvent = pDMEvent->GetNext();
	}

	/*
	// Update m_bHasCurveTypes to reflect which curve strips we're currently displaying
	CPianoRollStrip* pPianoRollStrip = NULL;
	POSITION position = m_pMIDIMgr->m_pPRSList.GetHeadPosition();
	while (position != NULL)
	{
		pPianoRollStrip = m_pMIDIMgr->m_pPRSList.GetNext(position);
		if ( pPianoRollStrip->m_pPartRef && (pPianoRollStrip->m_pPartRef->m_pDMPart == pPart) )
		{
			POSITION posCurveStrips = pPianoRollStrip->m_lstCurveStripStates.GetHeadPosition();
			while( posCurveStrips )
			{
				BYTE bCCType = pPianoRollStrip->m_lstCurveStripStates.GetNext( posCurveStrips )->m_bCCType;
			}
		}
	}
	*/
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetBoundariesOfSelectedNotes

void CDirectMusicPart::GetBoundariesOfSelectedNotes( DWORD dwVariations, long &lStartTime, long &lEndTime) const
{
	lStartTime = LONG_MAX;
	lEndTime = LONG_MIN;

	// Get the first element in our list
	CDirectMusicEventItem* pEvent = m_lstNotes.GetHead();

	// Continue while the merge sequence list is not empty
	while( pEvent )
	{
		if( pEvent->m_fSelected 
		&&	(pEvent->m_dwVariation & dwVariations) )
		{
			lStartTime = min( lStartTime, AbsTime( pEvent ) );
			lEndTime = max( lEndTime, AbsTime( pEvent ) + ((CDirectMusicStyleNote*)pEvent)->m_mtDuration );
		}

		pEvent = pEvent->GetNext();
	}

	if( lStartTime == LONG_MAX )
	{
		// Nothing selected - set lStartTime and lEndTime to invalid values
		ASSERT( lEndTime == LONG_MIN );
		lStartTime = -1;
		lEndTime = -1;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetGridLength

MUSIC_TIME CDirectMusicPart::GetGridLength( void ) const
{
	ASSERT( m_pMIDIMgr && m_pMIDIMgr->m_pDMPattern );

	// This isn't a simple multiplication because we need to handle the case
	// where the pattern is shorter than the part (and of a different TimeSig).
	MUSIC_TIME mtMaxClock = GetClockLength();
	return CLOCKS_TO_GRID( mtMaxClock + m_mtClocksPerGrid - 1, this );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetClockLength

MUSIC_TIME CDirectMusicPart::GetClockLength( void ) const
{
	ASSERT( m_pMIDIMgr && m_pMIDIMgr->m_pDMPattern );

	MUSIC_TIME mtPartLength = m_TimeSignature.m_bBeatsPerMeasure * m_wNbrMeasures * m_mtClocksPerBeat;
	MUSIC_TIME mtPatternLength = m_pMIDIMgr->m_pDMPattern->CalcLength();

	return min(mtPartLength, mtPatternLength);
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::SaveSelectedEventsToMIDITrack

HRESULT CDirectMusicPart::SaveSelectedEventsToMIDITrack( IStream* pIStream, long lStartGrid, DWORD dwVariations ) const
{
	// Create a MIDI track to store the data in
	CSMFTrack MIDITrack;

	// Place selected notes in lstSequences
	CDirectMusicEventItem* pEvent = GetFirstNote( dwVariations );
	while( pEvent )
	{
		if( (pEvent->m_fSelected == TRUE)
		&&  (pEvent->m_dwVariation & dwVariations) )
		{
			CSequenceItem* pItemNew = new CSequenceItem;

			if( pItemNew )
			{
				pItemNew->m_mtTime = GRID_TO_CLOCKS(pEvent->m_mtGridStart - lStartGrid, this);
				pItemNew->m_mtDuration = ((CDirectMusicStyleNote*)pEvent)->m_mtDuration;
				pItemNew->m_nOffset = ((CDirectMusicStyleNote*)pEvent)->m_nTimeOffset;
				pItemNew->m_bStatus = MIDI_NOTEON;
				pItemNew->m_bByte1 = ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue;
				pItemNew->m_bByte2 = ((CDirectMusicStyleNote*)pEvent)->m_bVelocity;

				// Add the item to the MIDI track
				MIDITrack.AddItem(pItemNew);
			}
		}
		pEvent = pEvent->GetNext();
	}

	MUSIC_TIME mtIncrement;
	DWORD dwLastValue;
	DWORD dwValue;

	// Place selected curves in lstSequences
	CDirectMusicStyleCurve *pDMCurve = m_lstCurves.GetHead();
	while( pDMCurve )
	{
		if( (pDMCurve->m_fSelected == TRUE)
		&&  (pDMCurve->m_dwVariation & dwVariations) )
		{
			// Generate MIDI events for the curve
			dwLastValue = 0xFFFFFFFF;
			pDMCurve->m_mtCurrent = 0;

			while( pDMCurve->m_mtCurrent <= pDMCurve->m_mtDuration )
			{
				// Compute value of curve at m_mtCurrent
				dwValue = pDMCurve->ComputeCurve( &mtIncrement );

				if( dwValue != dwLastValue )
				{
					dwLastValue = dwValue;

					// Place sequence item in list
					CSequenceItem* pItemNew = new CSequenceItem;

					if( pItemNew )
					{
						pItemNew->m_mtTime = GRID_TO_CLOCKS(pDMCurve->m_mtGridStart - lStartGrid, this);
						pItemNew->m_nOffset = short(pDMCurve->m_nTimeOffset + pDMCurve->m_mtCurrent);
						pItemNew->m_mtDuration = 1;

						switch( pDMCurve->m_bEventType )
						{
							case DMUS_CURVET_PBCURVE:
								pItemNew->m_bStatus = MIDI_PBEND;
								pItemNew->m_bByte1 = (BYTE)(dwValue & 0x7F);
								pItemNew->m_bByte2 = (BYTE)((dwValue >> 7) & 0x7F);
								break;
							
							case DMUS_CURVET_CCCURVE:
								pItemNew->m_bStatus = MIDI_CCHANGE;
								pItemNew->m_bByte1 = pDMCurve->m_bCCData;
								pItemNew->m_bByte2 = (BYTE)(dwValue & 0x7F);
								break;

							case DMUS_CURVET_PATCURVE:
								pItemNew->m_bStatus = MIDI_PTOUCH;
								pItemNew->m_bByte1 = pDMCurve->m_bCCData;
								pItemNew->m_bByte2 = (BYTE)(dwValue & 0x7F);
								break;

							case DMUS_CURVET_MATCURVE:
								pItemNew->m_bStatus = MIDI_MTOUCH;
								pItemNew->m_bByte1 = (BYTE)(dwValue & 0x7F);
								pItemNew->m_bByte2 = 0;
								break;

							case DMUS_CURVET_RPNCURVE:
								pItemNew->m_bStatus = MIDI_CCHANGE;
								pItemNew->m_bByte1 = 0xFF;
								pItemNew->m_bByte2 = DMUS_CURVET_RPNCURVE;
								pItemNew->m_dwMIDISaveData = (pDMCurve->m_wParamType << 16) | (dwValue & 0xFFFF);
								break;

							case DMUS_CURVET_NRPNCURVE:
								pItemNew->m_bStatus = MIDI_CCHANGE;
								pItemNew->m_bByte1 = 0xFF;
								pItemNew->m_bByte2 = DMUS_CURVET_NRPNCURVE;
								pItemNew->m_dwMIDISaveData = (pDMCurve->m_wParamType << 16) | (dwValue & 0xFFFF);
								break;

							default:
								ASSERT( 0 );	// Should not happen!
								break;
						}

						// No need to merge here - the MIDITrack does a sort when it writes the events out
						MIDITrack.AddCurveItem( pItemNew );
					}
				}

				// Exit the loop if mtIncrement is zero
				if( mtIncrement == 0 )
				{
					break;
				}

				// Increment time offset into Curve
				pDMCurve->m_mtCurrent += mtIncrement;
			}
		}

		pDMCurve = (CDirectMusicStyleCurve *)pDMCurve->GetNext();
	}

	HRESULT hr = MIDITrack.Write( pIStream );

	// cleanup
	while( !MIDITrack.m_notes.IsEmpty() )
	{
		delete MIDITrack.m_notes.RemoveHead();
	}
	while( !MIDITrack.m_curves.IsEmpty() )
	{
		delete MIDITrack.m_curves.RemoveHead();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetLastNoteOff

MUSIC_TIME CDirectMusicPart::GetLastNoteOff( void ) const
{
	MUSIC_TIME mtLastNoteOffTime = LONG_MIN;

	// Get the first note in our list
	CDirectMusicEventItem* pEvent = m_lstNotes.GetHead();

	while( pEvent )
	{
		const MUSIC_TIME mtNoteOffTime = AbsTime( pEvent ) + ((CDirectMusicStyleNote *)pEvent)->m_mtDuration;;
		if( mtLastNoteOffTime < mtNoteOffTime )
		{
			mtLastNoteOffTime = mtNoteOffTime;
		}
		pEvent = pEvent->GetNext();
	}

	return mtLastNoteOffTime;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetLastCurveEnd

MUSIC_TIME CDirectMusicPart::GetLastCurveEnd( void ) const
{
	MUSIC_TIME mtLastNoteOffTime = LONG_MIN;

	// Get the first note in our list
	CDirectMusicEventItem* pEvent = m_lstCurves.GetHead();

	while( pEvent )
	{
		const MUSIC_TIME mtNoteOffTime = AbsTime( pEvent ) + ((CDirectMusicStyleCurve *)pEvent)->m_mtDuration;
		if( mtLastNoteOffTime < mtNoteOffTime )
		{
			mtLastNoteOffTime = mtNoteOffTime;
		}
		pEvent = pEvent->GetNext();
	}

	return mtLastNoteOffTime;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::UpdateFromVarChoices

void CDirectMusicPart::UpdateFromVarChoices( IDMUSProdFramework *pFramework, IPersistStream *pIPersistStream )
{
	// Validate the parameters
	ASSERT( pIPersistStream );
	ASSERT( pFramework );
	if( pFramework == NULL
	||	pIPersistStream == NULL )
	{
		return;
	}

	// Try and allocate the stream
	IStream *pStream = NULL;
	if( SUCCEEDED( pFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStream ) ) )
	{
		IDMUSProdRIFFStream* pIRiffStream;
		if( SUCCEEDED( AllocRIFFStream( pStream, &pIRiffStream ) ) )
		{
			// Save the editor into the stream
			if( SUCCEEDED( pIPersistStream->Save( pStream, TRUE ) ) )
			{
				// Seek back to the start of the stream
				StreamSeek( pStream, 0, STREAM_SEEK_SET );

				// Prepare the chunk headers
				MMCKINFO ckMain, ckSubChunk;
				ckMain.fccType = DM_FOURCC_VARCHOICES_FORM;
				ckSubChunk.ckid = DM_FOURCC_VARCHOICES_CHUNK;

				// Prepare ioVarChoices structure
				ioVarChoices iVarChoices;
				memset( &iVarChoices, 0, sizeof(ioVarChoices) );
				memcpy( iVarChoices.m_dwVariationChoices, m_dwVariationChoices, sizeof(DWORD) * NBR_VARIATIONS );

				// Read in the data
				DWORD dwBytesRead;
				if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0
				&&	pIRiffStream->Descend( &ckSubChunk, NULL, MMIO_FINDCHUNK ) == 0
				&&	SUCCEEDED( pStream->Read( &iVarChoices, sizeof(ioVarChoices), &dwBytesRead) )
				&&	sizeof(ioVarChoices) == dwBytesRead )
				{
					// Copy the data
					memcpy( m_dwVariationChoices, iVarChoices.m_dwVariationChoices, sizeof(DWORD) * NBR_VARIATIONS );

					// Update the design data
					BOOL fRowDisabled;
					BOOL fDisabledFlag;

					for( short nRow = 0 ;  nRow < NBR_VARIATIONS ; nRow++ )
					{
						fRowDisabled = IsVarChoicesRowDisabled( nRow );
						fDisabledFlag = m_dwVariationsDisabled & (1 << nRow) ? TRUE : FALSE;

						if( fRowDisabled != fDisabledFlag )
						{
							if( fRowDisabled )
							{
								m_dwVariationsDisabled |= (1 << nRow);
								m_dwDisabledChoices[nRow] = m_dwVariationChoices[nRow];
							}
							else
							{
								m_dwVariationsDisabled &= ~(1 << nRow);
								m_dwDisabledChoices[nRow] = 0x00000000;
							}
						}
					}
				}
			}
			RELEASE( pIRiffStream );
		}
		RELEASE( pStream );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::IsValidNote

bool CDirectMusicPart::IsValidNote( CDirectMusicStyleNote *pNote )
{
	return m_lstNotes.IsMember( pNote ) ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef constructor/destructor

CDirectMusicPartRef::CDirectMusicPartRef( CDirectMusicPattern* pPattern )
{
	ASSERT( pPattern != NULL );
    
	m_pPattern = pPattern;
//	m_pPattern->AddRef();		intentionally missing

	m_pDMPart = NULL;
	m_dwPChannel = 0;
	m_bSubChordLevel = SUBCHORD_BASS;
	m_bPriority = 100;
	m_bVariationLockID = 0;		// no locking
	m_bRandomVariation = 1;
	memset( &m_guidOldPartID, 0, sizeof(GUID) );
	m_fHardLink = FALSE;
	m_pVarChoicesNode = NULL;
}

CDirectMusicPartRef::~CDirectMusicPartRef()
{
	if( m_pVarChoicesNode )
	{
		if( m_pPattern
		&&	m_pPattern->m_pMIDIMgr
		&&	m_pPattern->m_pMIDIMgr->m_pIFramework )
		{
			m_pPattern->m_pMIDIMgr->m_pIFramework->CloseEditor( m_pVarChoicesNode );
		}
		m_pVarChoicesNode->Release();
		m_pVarChoicesNode = NULL;
	}

	while( !m_lstPianoRollUIStates.IsEmpty() )
	{
		PianoRollUIState* pPianoRollUIState = static_cast<PianoRollUIState*>( m_lstPianoRollUIStates.RemoveHead() );

		if( pPianoRollUIState->pPianoRollData )
		{
			GlobalFree( pPianoRollUIState->pPianoRollData );
		}
		delete pPianoRollUIState;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::SetPart

void CDirectMusicPartRef::SetPart( CDirectMusicPart* pPart )
{
	if( m_pDMPart == pPart )
	{
		return;
	}

	if( m_pDMPart )
	{
		ASSERT( m_pDMPart->m_pMIDIMgr != NULL );
		ASSERT( m_pDMPart->m_dwUseCount > 0 );

		m_pDMPart->m_dwUseCount--;

		if( m_pDMPart->m_dwUseCount == 0 )
		{
			m_pDMPart->m_pMIDIMgr->DeletePart( m_pDMPart );
		}

		m_pDMPart = NULL;
	}

	if( pPart )
	{
		m_pDMPart = pPart;
		m_pDMPart->m_dwUseCount++;

		//if( ::IsEqualGUID( GUID_AllZeros, m_guidOldPartID ) )
		//{
		//	memcpy ( &m_guidOldPartID, &m_pDMPart->m_guidPartID, sizeof( GUID ));
		//}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::RecalcMusicValues

void CDirectMusicPartRef::RecalcMusicValues( void )
{
	ASSERT( m_pDMPart != NULL ); 
	ASSERT( m_pDMPart->m_pMIDIMgr != NULL ); 

	BYTE bPlayMode;
	WORD wMusicValue;

	CDirectMusicStyleNote* pDMNote;
	CDirectMusicEventItem* pDMEventItem = m_pDMPart->m_lstNotes.GetHead();

	while( pDMEventItem )
	{
		pDMNote = (CDirectMusicStyleNote *)pDMEventItem;

		bPlayMode = pDMNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE ?
			m_pDMPart->m_bPlayModeFlags : pDMNote->m_bPlayModeFlags;

		// Source MIDIValue is pDMNote->m_bMIDIValue
		DMUS_CHORD_PARAM chordData;
		if( m_pPattern->m_pMIDIMgr->GetChord( m_pDMPart, pDMNote, &chordData ) == S_OK )
		{
			// Using the chord, we now want to convert from the stored
			// MIDIValue to a MusicValue and store the new MusicValue in
			// pDMNote
			if( SUCCEEDED ( m_pPattern->m_pMIDIMgr->m_pIDMPerformance->MIDIToMusic( pDMNote->m_bMIDIValue, &chordData,
																					bPlayMode, m_bSubChordLevel,
																					&wMusicValue ) ) )
			{
				pDMNote->m_wMusicValue = wMusicValue;
			}
			/*
#ifdef _DEBUG
			// Determine value
			DMUS_SUBCHORD *pSubChord = NULL;
			DWORD dwLevel = 1 << m_bSubChordLevel;
			for (int i = 0; i < chordData.bSubChordCount; i++)
			{
				if (dwLevel & chordData.SubChordList[i].dwLevels)
				{
					pSubChord = &chordData.SubChordList[i];
					break;
				}
			}
			if (pSubChord == NULL) // Ran out? Use first chord.
			{
				pSubChord = &chordData.SubChordList[0];
			}
			WORD wProducerMusicValue;
			wProducerMusicValue = CDirectMusicPattern::NoteToMusicValue( pDMNote->m_bMIDIValue,
																		 bPlayMode,
																		 *pSubChord );
			TRACE("CDirectMusicPartRef::RecalcMusicValues %x %x\n", pDMNote->m_wMusicValue, wProducerMusicValue);
#endif
			*/
		}
		else
		{
			// Should not happen!
			// We need a chord!
			ASSERT( 0 );
		}

		pDMEventItem = pDMEventItem->GetNext();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::RecalcMIDIValues

void CDirectMusicPartRef::RecalcMIDIValues( void )
{
	ASSERT( m_pDMPart != NULL ); 
	ASSERT( m_pDMPart->m_pMIDIMgr != NULL ); 

	CDirectMusicStyleNote* pDMNote;
	CDirectMusicEventItem* pDMEventItem = m_pDMPart->m_lstNotes.GetHead();

	BYTE bPlayMode;

	while( pDMEventItem )
	{
		pDMNote = (CDirectMusicStyleNote *)pDMEventItem;

		bPlayMode = pDMNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE ?
			m_pDMPart->m_bPlayModeFlags : pDMNote->m_bPlayModeFlags;

		if( bPlayMode == DMUS_PLAYMODE_FIXED )
		{
			ASSERT( (pDMNote->m_wMusicValue >=0) && (pDMNote->m_wMusicValue <= 127) );
			pDMNote->m_bMIDIValue = (BYTE)pDMNote->m_wMusicValue;
		}
		else
		{
			pDMNote->m_bMIDIValue = DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
		}

		pDMEventItem = pDMEventItem->GetNext();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::SetInversionBoundaries

void CDirectMusicPartRef::SetInversionBoundaries( void )
{
	ASSERT( m_pDMPart != NULL );

	if( m_pDMPart->m_bAutoInvert )
	{
		int nUpper = 0;
		int nLower = 127;
		int nValue;
		BYTE bPlayMode;

		CDirectMusicStyleNote* pDMNote;
		CDirectMusicEventItem* pDMEventItem = m_pDMPart->m_lstNotes.GetHead();

		while( pDMEventItem )
		{
			pDMNote = (CDirectMusicStyleNote *)pDMEventItem;

			if( pDMNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE )
			{
				bPlayMode = m_pDMPart->m_bPlayModeFlags;
			}
			else
			{
				bPlayMode = pDMNote->m_bPlayModeFlags;
			}

			if( bPlayMode == DMUS_PLAYMODE_FIXED )
			{
				ASSERT( (pDMNote->m_wMusicValue >=0) && (pDMNote->m_wMusicValue <= 127) );
				nValue = (int)pDMNote->m_wMusicValue;
			}
			else
			{
				nValue = DMNoteToMIDIValue( pDMNote, DMUS_PLAYMODE_NONE );
			}

			if( nUpper < nValue )
			{
				nUpper = nValue;
			}

			if( nLower > nValue )
			{
				nLower = nValue;
			}

			pDMEventItem = pDMEventItem->GetNext();
		}

		if( nUpper == 0 )
		{
			nUpper = 126;
			nLower = 1;
		}
		nUpper++;
		nLower--;

		if( nUpper < MIN_INV_UPPER)
		{
			nUpper = MIN_INV_UPPER;
		}
		if( nUpper > MAX_INV_UPPER )
		{
			nUpper = MAX_INV_UPPER;
		}

		if( nLower < MIN_INV_LOWER)
		{
			nLower = MIN_INV_LOWER;
		}
		if( nLower > MAX_INV_LOWER )
		{
			nLower = MAX_INV_LOWER;
		}

		if( (nLower + 12) > nUpper )
		{
			nUpper = nLower + 12 ;
		}

		m_pDMPart->m_bInvertUpper = BYTE(nUpper);
		m_pDMPart->m_bInvertLower = BYTE(nLower);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DMNoteToMIDIValue

BYTE CDirectMusicPartRef::DMNoteToMIDIValue( const CDirectMusicStyleNote *pDMNote, BYTE bPlayModeFlags ) const
{
	ASSERT( m_pDMPart != NULL );
	ASSERT( m_pPattern != NULL );
	ASSERT( m_pPattern->m_pMIDIMgr != NULL );

	// Determine playmode
	if( bPlayModeFlags == DMUS_PLAYMODE_NONE )
	{
		bPlayModeFlags = pDMNote->m_bPlayModeFlags;
		if( bPlayModeFlags == DMUS_PLAYMODE_NONE )
		{
			bPlayModeFlags = m_pDMPart->m_bPlayModeFlags;
		}
	}

	// Initialize bMIDIValue, in case MusicToMIDI fails
	BYTE bMIDIValue = 0;
	DMUS_CHORD_PARAM chordData;

	// Get chord
	if( m_pPattern->m_pMIDIMgr->GetChord( m_pDMPart, pDMNote, &chordData ) == S_OK )
	{
		HRESULT hr = m_pPattern->m_pMIDIMgr->m_pIDMPerformance->MusicToMIDI( pDMNote->m_wMusicValue, &chordData,
																			bPlayModeFlags, m_bSubChordLevel,
																			&bMIDIValue );
#ifdef _DEBUG
		if( FAILED( hr )
		||	(hr == DMUS_S_OVER_CHORD) )
		{
			TRACE("MIDIStripMgr: DMNoteToMIDIValue failed conversion!\n");
		}
#endif
	}
	else
	{
		// Should not happen!
		// We need a chord!
		ASSERT( 0 );

		// BUGBUG: may overflow
		bMIDIValue = pDMNote->m_bMIDIValue;
	}

//	ASSERT( (bValue < 128) && (bValue >=0) );
	return bMIDIValue;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DMNoteToMusicValue

WORD CDirectMusicPartRef::DMNoteToMusicValue( const CDirectMusicStyleNote *pDMNote, BYTE bPlayModeFlags ) const
{
	ASSERT( m_pDMPart != NULL );
	ASSERT( m_pPattern != NULL );
	ASSERT( m_pPattern->m_pMIDIMgr != NULL );

	// Determine playmode
	if( bPlayModeFlags == DMUS_PLAYMODE_NONE )
	{
		bPlayModeFlags = pDMNote->m_bPlayModeFlags;
		if( bPlayModeFlags == DMUS_PLAYMODE_NONE )
		{
			bPlayModeFlags = m_pDMPart->m_bPlayModeFlags;
		}
	}

	WORD wMusicValue;
	DMUS_CHORD_PARAM chordData;

	// Get chord
	if( m_pPattern->m_pMIDIMgr->GetChord( m_pDMPart, pDMNote, &chordData ) == S_OK )
	{
		if( FAILED( m_pPattern->m_pMIDIMgr->m_pIDMPerformance->MIDIToMusic( pDMNote->m_bMIDIValue, &chordData,
																			bPlayModeFlags, m_bSubChordLevel,
																			&wMusicValue ) ) )
		{
			TRACE("MIDIStripMgr: DMNoteToMusicValue failed conversion!\n");
			wMusicValue = pDMNote->m_wMusicValue;
		}
	}
	else
	{
		// Should not happen!
		// We need a chord!
		ASSERT( 0 );
		wMusicValue = pDMNote->m_wMusicValue;
	}

	return wMusicValue;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DMNoteToLogicalScaleAccidental

WORD CDirectMusicPartRef::DMNoteToLogicalScaleAccidental( const CDirectMusicStyleNote *pDMNote ) const
{
	ASSERT( pDMNote != NULL );
	ASSERT( m_pDMPart != NULL );

	// Determine playmode
	BYTE bPlayMode = pDMNote->m_bPlayModeFlags;
	if( bPlayMode == DMUS_PLAYMODE_NONE )
	{
		bPlayMode = m_pDMPart->m_bPlayModeFlags;
	}

	// Determine Scale/Accidental value
	WORD wScaleValue;

	if( (bPlayMode == DMUS_PLAYMODE_FIXED) ||
		(bPlayMode == DMUS_PLAYMODE_FIXEDTOKEY) ||
		(bPlayMode == DMUS_PLAYMODE_FIXEDTOCHORD) )
	{
		wScaleValue = DMNoteToMusicValue( pDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
	}
	else
	{
		wScaleValue = pDMNote->m_wMusicValue;
	}

	return wScaleValue;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DM_SavePartRef

HRESULT CDirectMusicPartRef::DM_SavePartRef( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_PARTREF oDMPartRef;
	ioDMPartOldGUID oDMPartOldGUID;

	ASSERT( m_pDMPart != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pPersistInfo->Release();
		pPersistInfo = NULL;
	}

	// Write PartRef list header
	ckMain.fccType = DMUS_FOURCC_PARTREF_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write PartRef chunk header
	ck.ckid = DMUS_FOURCC_PARTREF_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_PARTREF structure
	memset( &oDMPartRef, 0, sizeof(DMUS_IO_PARTREF) );

	oDMPartRef.wLogicalPartID = WORD(m_dwPChannel);
	oDMPartRef.bSubChordLevel = m_bSubChordLevel;
	oDMPartRef.bPriority = m_bPriority;
	oDMPartRef.bVariationLockID = m_bVariationLockID; 
	oDMPartRef.bRandomVariation = m_bRandomVariation;
	oDMPartRef.dwPChannel = m_dwPChannel;

	memcpy( &oDMPartRef.guidPartID, &m_pDMPart->m_guidPartID, sizeof(GUID) );

	// Write PartRef chunk data
	hr = pIStream->Write( &oDMPartRef, sizeof(DMUS_IO_PARTREF), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_PARTREF) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	if( StreamInfo.ftFileType == FT_DESIGN )
	{
		if( !::IsEqualGUID( GUID_AllZeros, m_guidOldPartID ) )
		{
			// Write PartOldGUID chunk header
			ck.ckid = DMUS_FOURCC_OLDGUID_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Prepare ioDMPartOldGUID structure
			memset( &oDMPartOldGUID, 0, sizeof(ioDMPartOldGUID) );

			memcpy( &oDMPartOldGUID.m_guidOldPartID, &m_guidOldPartID, sizeof(GUID) );

			// Write PartOldGUID data
			hr = pIStream->Write( &oDMPartOldGUID, sizeof(ioDMPartOldGUID), &dwBytesWritten);
			if( FAILED( hr )
				||  dwBytesWritten != sizeof(ioDMPartOldGUID) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Reset m_guidOldPartID to GUID_AllZeros
			memset( &m_guidOldPartID, 0, sizeof(GUID) );
		}

		// Save PartRef info
		hr = DM_SavePartRefInfoList( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}

		// Save PartRef Design data
		hr = DM_SavePartRefDesignData( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}

		hr = m_pPattern->m_pMIDIMgr->SavePianoRollDesignData( pIRiffStream, this );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DM_SavePartRefDesignData

HRESULT CDirectMusicPartRef::DM_SavePartRefDesignData( IDMUSProdRIFFStream* pIRiffStream ) const
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pPersistInfo );
	}

	// Only save this chunk in "Design" files
	if( StreamInfo.ftFileType != FT_DESIGN )
	{
		hr = S_OK;
		goto ON_ERROR;	// Not an error, just need to free pIStream
	}

	// Write PartRef Design header
	ck.ckid = DMUS_FOURCC_PARTREF_DESIGN;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Initialize ioDMStylePartRefDesign
	ioDMStylePartRefDesign oDMStylePartRefDesign;
	oDMStylePartRefDesign.m_fHardLink = m_fHardLink;

    hr = pIStream->Write( &oDMStylePartRefDesign, sizeof(ioDMStylePartRefDesign), &dwBytesWritten );
    if( FAILED( hr )
    ||  dwBytesWritten != sizeof(ioDMStylePartRefDesign) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DM_LoadPartRef

HRESULT CDirectMusicPartRef::DM_LoadPartRef( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	CDirectMusicPart* pPart;
	IStream* pIStream;
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
	BOOL fSetOldGuid = TRUE;
	BOOL fLoadedPianoRollDesign = FALSE;

	ASSERT( m_pPattern != NULL );
	ASSERT( m_pPattern->m_pMIDIMgr != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_PARTREF_CHUNK:
			{
				DMUS_IO_PARTREF iDMPartRef;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_PARTREF ) );
				hr = pIStream->Read( &iDMPartRef, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if (dwSize < DX8_PARTREF_SIZE)
				{
					m_dwPChannel = iDMPartRef.wLogicalPartID;	
				}
				else
				{
					m_dwPChannel = iDMPartRef.dwPChannel;
				}
				m_bSubChordLevel = iDMPartRef.bSubChordLevel;
				m_bPriority = iDMPartRef.bPriority;
				m_bVariationLockID = iDMPartRef.bVariationLockID; 
				m_bRandomVariation = iDMPartRef.bRandomVariation;

				pPart = m_pPattern->m_pMIDIMgr->FindPartByGUID( iDMPartRef.guidPartID );
				if( pPart == NULL )
				{
					ASSERT( 0 );
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if (fSetOldGuid)
				{
					//memcpy ( &m_guidOldPartID, &iDMPartRef.guidPartID, sizeof( GUID ));
				}
				SetPart( pPart );

				// Set MIDI values.
				if( m_pPattern->m_pMIDIMgr->m_pIDMTrack == NULL )
				{
					RecalcMIDIValues();
				}
				break;
			}

			case DMUS_FOURCC_OLDGUID_CHUNK:
			{
				ioDMPartOldGUID iDMPartOldGUID;

				dwSize = min( ck.cksize, sizeof( ioDMPartOldGUID ) );
				hr = pIStream->Read( &iDMPartOldGUID, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				fSetOldGuid = FALSE;
				//memcpy ( &m_guidOldPartID, &iDMPartOldGUID.m_guidOldPartID, sizeof( GUID ));
				break;
			}

			case DMUS_FOURCC_PARTREF_DESIGN:
			{
				ioDMStylePartRefDesign iDMStylePartRefDesign;

				dwSize = min( ck.cksize, sizeof( ioDMStylePartRefDesign ) );
				hr = pIStream->Read( &iDMStylePartRefDesign, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_fHardLink = iDMStylePartRefDesign.m_fHardLink;
				break;
			}

			case FOURCC_LIST:
			{
				MMCKINFO ckList;
				switch( ck.fccType )
				{
					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case DMUS_FOURCC_PIANOROLL_LIST:
						if( m_pPattern->m_pMIDIMgr->m_pIDMTrack )
						{
							PianoRollUIState* pPianoRollUIState;

							pPianoRollUIState = new PianoRollUIState;
							if( pPianoRollUIState == NULL )
							{
								hr = E_OUTOFMEMORY;
								goto ON_ERROR;
							}

							dwSize = ck.cksize;
							dwSize -= 4;
							pPianoRollUIState->pPianoRollData = (BYTE *)GlobalAlloc( GPTR, dwSize );
							if( pPianoRollUIState->pPianoRollData == NULL )
							{
								delete pPianoRollUIState;
								hr = E_OUTOFMEMORY;
								goto ON_ERROR;
							}
							pPianoRollUIState->dwPianoRollDataSize = dwSize;
							hr = pIStream->Read( pPianoRollUIState->pPianoRollData,
												 pPianoRollUIState->dwPianoRollDataSize,
												 &dwByteCount );
							if( FAILED( hr )
							||  dwByteCount != pPianoRollUIState->dwPianoRollDataSize )
							{
								hr = E_FAIL;
								GlobalFree( pPianoRollUIState->pPianoRollData );
								delete pPianoRollUIState;
								goto ON_ERROR;
							}
							m_lstPianoRollUIStates.AddTail( pPianoRollUIState );
						}
						else
						{
							hr = m_pPattern->m_pMIDIMgr->LoadPianoRollDesignData( pIRiffStream, &ck, this );
							if( FAILED( hr ) )
							{
								goto ON_ERROR;
							}
							fLoadedPianoRollDesign = TRUE;
						}
						break;
				}
				break;
			}
		}

        pIRiffStream->Ascend( &ck, 0 );
	}

	if( (m_pPattern->m_pMIDIMgr->m_pIDMTrack == NULL)
	&&	(fLoadedPianoRollDesign == FALSE) )
	{
		hr = m_pPattern->m_pMIDIMgr->CreateDefaultPianoRollStrip( this );
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::InsertSeqItem

void CDirectMusicPartRef::InsertSeqItem( const DMUS_IO_SEQ_ITEM *pSeqItem, BOOL &fChanged, DWORD dwVariations, MUSIC_TIME mtGrid )
{
	// Need to set m_mtGridStart, m_dwVariation, m_wMusicValue after this constructor
	CDirectMusicStyleNote *pTmpDMNote = new CDirectMusicStyleNote( pSeqItem );

	// Convert  to the destination TimeSig, ensuring the result isn't negative
	MUSIC_TIME mtTmpGrid = max( -mtGrid, CLOCKS_TO_GRID( pSeqItem->mtTime, m_pDMPart ) );
	// Add the difference to the note's TimeOffset - I think this should never increase the
	// TimeOffset so it makes the note take on a chord from a different beat (unless that's how
	// it was to begin with
	pTmpDMNote->m_nTimeOffset += pSeqItem->mtTime - GRID_TO_CLOCKS( mtTmpGrid, m_pDMPart );
	// Set the new notes's grid start position, offsetting it by mtGrid (the position to paste at)
	pTmpDMNote->m_mtGridStart = mtGrid + mtTmpGrid;

	// Only insert the note if it will start before the end of time.
	if ( pTmpDMNote->m_mtGridStart < m_pDMPart->GetGridLength() )
	{
		// Commented out to prevent both curves and notes from being selected at the same time
		pTmpDMNote->m_fSelected = TRUE; 

		// Adjust the note's MuiscValue if necessary
		ASSERT( m_pDMPart->m_bPlayModeFlags != DMUS_PLAYMODE_NONE );

		// If the strip's playmode is DMUS_PLAYMODE_FIXED, convert the value from a musicvalue to a note
		if( m_pDMPart->m_bPlayModeFlags == DMUS_PLAYMODE_FIXED )
		{
			pTmpDMNote->m_wMusicValue = pTmpDMNote->m_bMIDIValue;
		}
		else
		{
			pTmpDMNote->m_wMusicValue = DMNoteToMusicValue( pTmpDMNote, DMUS_PLAYMODE_NONE );
		}

		// Set the note's variation flags
		pTmpDMNote->m_dwVariation = dwVariations;

		if( !fChanged )
		{
			fChanged = TRUE;
			m_pPattern->m_pMIDIMgr->PreChangePartRef( this );
		}

		// Finally, actually inset the note
		m_pDMPart->InsertNoteInAscendingOrder( pTmpDMNote );
	}
	else
	{
		pTmpDMNote->SetNext(NULL);
		delete pTmpDMNote;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::InsertCurveItem

void CDirectMusicPartRef::InsertCurveItem( const CCurveItem *pCurveItem, BOOL &fChanged, DWORD dwVariations, MUSIC_TIME mtGrid )
{
	// Need to set m_mtGridStart, m_dwVariation, reset values after this constructor
	CDirectMusicStyleCurve *pTmpDMCurve = new CDirectMusicStyleCurve( pCurveItem );

	if( pTmpDMCurve )
	{
		MUSIC_TIME mtPartClockLength = m_pDMPart->GetClockLength();

		VARIANT var;
		m_pPattern->m_pMIDIMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		MUSIC_TIME mtPatternLength = V_I4(&var);

		// Set default reset values
		pTmpDMCurve->SetDefaultResetValues( min(mtPartClockLength, mtPatternLength) );

		// Convert  to the destination TimeSig
		MUSIC_TIME mtTmpGrid = max( -mtGrid, CLOCKS_TO_GRID( pCurveItem->m_mtTime, m_pDMPart ) );
		pTmpDMCurve->m_nTimeOffset += pCurveItem->m_mtTime - GRID_TO_CLOCKS( mtTmpGrid, m_pDMPart );
		// Set the new notes's grid start position, offsetting it by mtGrid (the position to paste at)
		pTmpDMCurve->m_mtGridStart = mtGrid + mtTmpGrid;

		// Only insert the curve if it will start before the end of time.
		if( pTmpDMCurve->m_mtGridStart < m_pDMPart->GetGridLength() )
		{
			// Commented out to prevent both curves and notes from being selected at the same time
			pTmpDMCurve->m_fSelected = TRUE;

			// Set the curve's variation flags
			pTmpDMCurve->m_dwVariation = dwVariations;

			if( !fChanged )
			{
				fChanged = TRUE;
				m_pPattern->m_pMIDIMgr->PreChangePartRef( this );
			}

			// Finally, actually insert the curve
			m_pDMPart->InsertCurveInAscendingOrder( pTmpDMCurve );
		}
		else
		{
			pTmpDMCurve->SetNext(NULL);
			delete pTmpDMCurve;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::UpdateNoteAfterMove

BOOL CDirectMusicPartRef::UpdateNoteAfterMove( CDirectMusicStyleNote *pDMNote ) const
{
	// Get the ChordStrip mode
	BYTE bMode;
	if( FAILED ( m_pPattern->m_pMIDIMgr->m_pTimeline->GetParam( GUID_PatternEditorMode, m_pPattern->m_pMIDIMgr->m_pIDMTrack ? m_pPattern->m_pMIDIMgr->m_dwGroupBits : m_pPattern->m_pMIDIMgr->m_pChordTrack->m_dwGroupBits,
																0, 0, NULL, &bMode ) ) )
	{
		bMode = CHORDSTRIP_MODE_MIDI_CONSTANT;
	}

	if( bMode == CHORDSTRIP_MODE_FUNCTION_CONSTANT )
	{
		// Recalc the MIDI value of this note
		BYTE bPlayMode = pDMNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE ?
			m_pDMPart->m_bPlayModeFlags : pDMNote->m_bPlayModeFlags;

		if( bPlayMode == DMUS_PLAYMODE_FIXED )
		{
			ASSERT( (pDMNote->m_wMusicValue >=0) && (pDMNote->m_wMusicValue <= 127) );
			if( pDMNote->m_bMIDIValue != (BYTE)pDMNote->m_wMusicValue )
			{
				pDMNote->m_bMIDIValue = (BYTE)pDMNote->m_wMusicValue;
				return TRUE;
			}
		}
		else
		{
			DMUS_CHORD_PARAM chordData;
			if( m_pPattern->m_pMIDIMgr->GetChord( m_pDMPart, pDMNote, &chordData ) == S_OK )
			{
				// Using the chord, we now want to convert from the stored
				// MusicValue to a MIDIValue and store the new MIDIValue in
				// pDMNote
				BYTE bMIDIValue = 0;
				// This may fail with DMUS_S_OVER_CHORD, so initialize bMIDIValue to 0
				if( SUCCEEDED ( m_pPattern->m_pMIDIMgr->m_pIDMPerformance->MusicToMIDI( pDMNote->m_wMusicValue, &chordData,
																						bPlayMode, m_bSubChordLevel,
																						&bMIDIValue ) ) )
				{
					if( pDMNote->m_bMIDIValue != bMIDIValue )
					{
						pDMNote->m_bMIDIValue = bMIDIValue;
						return TRUE;
					}
				}
			}
		}
	}
	else
	{
		// Recalc the musicvalues of this note
		BYTE bPlayMode = pDMNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE ?
			m_pDMPart->m_bPlayModeFlags : pDMNote->m_bPlayModeFlags;

		// Source MIDIValue is pDMNote->m_bMIDIValue
		if( bPlayMode == DMUS_PLAYMODE_FIXED )
		{
			if( pDMNote->m_wMusicValue != pDMNote->m_bMIDIValue )
			{
				pDMNote->m_wMusicValue = pDMNote->m_bMIDIValue;
				return TRUE;
			}
		}
		else
		{
			DMUS_CHORD_PARAM chordData;
			if( m_pPattern->m_pMIDIMgr->GetChord( m_pDMPart, pDMNote, &chordData ) == S_OK )
			{
				// Using the chord, we now want to convert from the stored
				// MIDIValue to a MusicValue and store the new MusicValue in
				// pDMNote
				WORD wMusicValue;
				if( SUCCEEDED ( m_pPattern->m_pMIDIMgr->m_pIDMPerformance->MIDIToMusic( pDMNote->m_bMIDIValue, &chordData,
																						bPlayMode, m_bSubChordLevel,
																						&wMusicValue ) ) )
				{
					if( pDMNote->m_wMusicValue != wMusicValue )
					{
						pDMNote->m_wMusicValue = wMusicValue;
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::ImportEventsFromMIDITrack

HRESULT CDirectMusicPartRef::ImportEventsFromMIDITrack( IStream* pStream, long lStartGrid, DWORD dwVariations )
{
	// Check for the MIDI Track header
	DWORD dwID;
    pStream->Read( &dwID, sizeof( FOURCC ), NULL );
    if( dwID != mmioFOURCC( 'M', 'T', 'r', 'k' ) )
	{
		return E_UNEXPECTED;
	}

	// Get the size of this track
	DWORD dwSize;
	GetMLong( pStream, dwSize );

	// Initialize the MIDI import global variables
	glTimeSig = 0; // Don't care about TimeSig information
	gpTempoStream = NULL;
	gpSysExStream = NULL;
	gpTimeSigStream = NULL;

	// Read in all the events
	DWORD dwCurTime = 0;
	DWORD dwOffsetTime = 0;
	FullSeqEvent* lstTrackEvent = NULL;
	while( dwSize > 0 )
	{
		dwSize -= GetVarLength( pStream, dwOffsetTime );
		dwCurTime += dwOffsetTime;
		dwSize -= ReadEvent( pStream, dwCurTime, &lstTrackEvent, NULL);
	}
	lstTrackEvent = SortEventList( lstTrackEvent );
	lstTrackEvent = CompressEventList( lstTrackEvent );

	// Convert the events into curve or sequence items and insert them in our list
	for(FullSeqEvent* pEvent = lstTrackEvent; pEvent; pEvent = pEvent->pNext )
	{
		if( (Status(pEvent->bStatus) == MIDI_PBEND)
		||  (Status(pEvent->bStatus) == MIDI_PTOUCH)
		||  (Status(pEvent->bStatus) == MIDI_MTOUCH)
		||  (Status(pEvent->bStatus) == MIDI_CCHANGE) )
		{
			CCurveItem* pCurveItem = new CCurveItem( pEvent );
			if( pCurveItem )
			{
				pCurveItem->m_fSelected = TRUE;
				InsertCurveItem( pCurveItem, m_fChanged, dwVariations, lStartGrid );
				delete pCurveItem;
			}
		}
		else
		{
			CSequenceItem* pItem = new CSequenceItem( pEvent );
			if( pItem )
			{
				pItem->m_fSelected = TRUE;
				DMUS_IO_SEQ_ITEM iSeqNote;
				pItem->CopyTo( iSeqNote );
				delete pItem;

				InsertSeqItem( &iSeqNote, m_fChanged, dwVariations, lStartGrid );
			}
		}
	}

	// Clean up
	List_Free( lstTrackEvent );
	if( gpTempoStream )
	{
		gpTempoStream->Release();
		gpTempoStream = NULL;
	}
	if( gpSysExStream )
	{
		gpSysExStream->Release();
		gpSysExStream = NULL;
	}
	if( gpTimeSigStream )
	{
		gpTimeSigStream->Release();
		gpTimeSigStream = NULL;
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::InitializeVarChoicesEditor

void CDirectMusicPartRef::InitializeVarChoicesEditor( void )
{
	// Must have segment node and framework pointers
	if( !m_pPattern
	||	!m_pPattern->m_pMIDIMgr
	||	!m_pPattern->m_pMIDIMgr->m_pDMProdSegmentNode
	||	!m_pPattern->m_pMIDIMgr->m_pIFramework )
	{
		ASSERT(FALSE);
		return;
	}

	// Set the dialog's title and callback and data
	CString strTitle;

	// "Segment name - "
	BSTR bstrNodeName;
	if( SUCCEEDED( m_pPattern->m_pMIDIMgr->m_pDMProdSegmentNode->GetNodeName( &bstrNodeName ) ) )
	{
		strTitle = bstrNodeName;
		strTitle += _T(" - " );
		::SysFreeString( bstrNodeName );
	}

	// "Pattern name "
	strTitle += m_pPattern->m_strName;
	strTitle += _T(" " );

	// "(Track name)"
	strTitle += _T("(" );
	strTitle += m_strName;
	strTitle += _T(")" );

	IVarChoices *pIVarChoices;
	if( SUCCEEDED( m_pVarChoicesNode->QueryInterface( IID_IVarChoices, (void **)&pIVarChoices ) ) )
	{
		pIVarChoices->SetVarChoicesTitle( strTitle.AllocSysString() );

		pIVarChoices->SetDataChangedCallback( static_cast<IDMUSProdTimelineCallback *>(m_pPattern) );

		pIVarChoices->Release();
	}

	m_pVarChoicesNode->SetDocRootNode( m_pPattern->m_pMIDIMgr->m_pDMProdSegmentNode );


	IPersistStream *pIPersistStream;
	if( SUCCEEDED( m_pVarChoicesNode->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
	{
		IStream *pStream = CreateStreamForVarChoices( m_pPattern->m_pMIDIMgr->m_pIFramework, m_pDMPart->m_dwVariationChoices );
		if( pStream )
		{
			StreamSeek( pStream, 0, STREAM_SEEK_SET );
			pIPersistStream->Load( pStream );
			pStream->Release();
		}
		pIPersistStream->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern constructor/destructor

CDirectMusicPattern::CDirectMusicPattern( CMIDIMgr* pMIDIMgr, BOOL fMotif )
{
//	ASSERT( theApp.m_pMIDIMgrComponent != NULL );
	ASSERT( pMIDIMgr != NULL );
    
	m_dwRef = 0;
	AddRef();

	m_fModified = FALSE;
	m_fInLoad = FALSE;

	m_pMIDIMgr = pMIDIMgr;
//	m_pMIDIMgr->AddRef();		intentionally missing

	m_dwPlaybackID = ++m_pMIDIMgr->m_dwNextPatternID;

    m_dwDefaultKeyPattern = 0xAB5AB5;	// Major	
    m_dwDefaultChordPattern = 0x891;	// M7
    m_bDefaultKeyRoot = 0;		
    m_bDefaultChordRoot = 0;		
	m_fChordFlatsNotSharps = FALSE;
	m_fKeyFlatsNotSharps = FALSE;

	// Set defaults
	m_TimeSignature = m_pMIDIMgr->m_TimeSignature;
	m_wNbrMeasures = 1;
 
	m_dwRepeats = 0;
    m_mtPlayStart = 0;
    m_mtLoopStart = 0;
    m_mtLoopEnd = CalcLength();
    m_dwResolution = DMUS_SEGF_BEAT;

	m_bGrooveBottom = 1; 
	m_bGrooveTop = 100;
	m_bDestGrooveBottom = 1; 
	m_bDestGrooveTop = 100;

	// By default, set DMUS_PATTERNF_PERSIST_CONTROL for pattern tracks.
	// For patterns and motifs in styles, this will be overwritten when the pattern is loaded.
	m_dwFlags = DMUS_PATTERNF_PERSIST_CONTROL;

	m_pRhythmMap = NULL;
	if( fMotif )
	{
		m_wEmbellishment = EMB_MOTIF;
	}
	else
	{
		m_wEmbellishment = EMB_NORMAL;
	}

	// Set default name
	if( fMotif )
	{
		m_strName.LoadString( IDS_MOTIF_TEXT );
	}
	else
	{
		m_strName.LoadString( IDS_PATTERN_TEXT );
	}
}

CDirectMusicPattern::~CDirectMusicPattern()
{
	OleFlushClipboard( );

	if( m_pRhythmMap )
	{
		delete [] m_pRhythmMap;
	}

	CDirectMusicPartRef* pPartRef;

	while( !m_lstPartRefs.IsEmpty() )
	{
		pPartRef = static_cast<CDirectMusicPartRef*>( m_lstPartRefs.RemoveHead() );
		delete pPartRef;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetModified

void CDirectMusicPattern::SetModified( BOOL fModified )
{
	m_fModified = fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::RecalcMusicValues

void CDirectMusicPattern::RecalcMusicValues( void )
{
	CDirectMusicPartRef* pPartRef;

	POSITION pos = m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		pPartRef = m_lstPartRefs.GetNext( pos );
		
		// Assume that this will change the note values, so make a new Part if we need to
		m_pMIDIMgr->PreChangePartRef( pPartRef );

		pPartRef->RecalcMusicValues();
		// Need to update MIDI values as well - they may have chanaged (if notes
		// are now below the bottom of the chord, aren't in the key, etc.)
		pPartRef->RecalcMIDIValues();

		// Update inversion boundaries
		m_pMIDIMgr->UpdatePartParamsAfterChange( pPartRef->m_pDMPart );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::RecalcMIDIValues

void CDirectMusicPattern::RecalcMIDIValues( void )
{
	POSITION pos = m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		m_lstPartRefs.GetNext( pos )->RecalcMIDIValues();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::GetNumHardLinks

DWORD CDirectMusicPattern::GetNumHardLinks( const CDirectMusicPart* pPart ) const
{
	// Compute the number of PartRefs that hard link to the specified part
	DWORD dwResult = 0;
	POSITION pos = m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		CDirectMusicPartRef *pPartRef = m_lstPartRefs.GetNext( pos );
		if( pPartRef->m_fHardLink && (pPartRef->m_pDMPart == pPart) )
		{
			dwResult++;
		}
	}
	return dwResult;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IUnknown implementation

HRESULT CDirectMusicPattern::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsEqualIID(riid, IID_IPersist) )
	{
		*ppvObj = static_cast<IPersist *>(this);
	}
	else if( ::IsEqualIID(riid, IID_IPersistStream) )
	{
		*ppvObj = static_cast<IPersistStream *>(this);
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdTimelineCallback) )
	{
		*ppvObj = static_cast<IDMUSProdTimelineCallback *>(this);
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CDirectMusicPattern::AddRef()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CDirectMusicPattern::Release()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersist::GetClassID

HRESULT CDirectMusicPattern::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    ASSERT( pClsId != NULL );

	if( pClsId == NULL )
	{
		return E_POINTER;
	}

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersistStream implementation


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersistStream::Load

HRESULT CDirectMusicPattern::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

    DWORD dwPos = StreamTell( pIStream );

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr = E_FAIL;

	m_fInLoad = TRUE;
	BOOL fFoundFormat = FALSE;

	// Check for Direct Music format
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_PATTERN_LIST;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
		{
			hr = DM_LoadPattern( pIRiffStream, &ckMain );
			fFoundFormat = TRUE;
		}

		pIRiffStream->Release();
	}

	// Check for Direct Music single Pattern format
	if( fFoundFormat == FALSE )
	{
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_PATTERN_LIST;

			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				hr = DM_LoadPattern( pIRiffStream, &ckMain );
				fFoundFormat = TRUE;
			}

			pIRiffStream->Release();
		}
	}

	// Check for Pattern Track format
	if( fFoundFormat == FALSE )
	{
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_PATTERN_FORM;
			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				// Ignore the style header chunk
				ckMain.fccType = DMUS_FOURCC_PATTERN_LIST;
				if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
				{
					hr = DM_LoadPattern( pIRiffStream, &ckMain );
					fFoundFormat = TRUE;
				}
			}

			pIRiffStream->Release();
		}
	}

	if( fFoundFormat == FALSE )
	{
		// This can happen when the segment tries to load us with an empty stream
		hr = E_INVALIDARG;
	}

	m_fInLoad = FALSE;
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersistStream::Save

HRESULT CDirectMusicPattern::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}
	
	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pPersistInfo->Release();
		pPersistInfo = NULL;
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

    HRESULT hr = E_FAIL;

	// Save the Pattern
	if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion ) )
	{
		// Save for a Pattern Track
		// Single Pattern (Direct Music format)
		// Used by clipboard and MIDI Manager
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_PATTERN_FORM;

			MMCKINFO ckPattern;
			ckPattern.fccType = DMUS_FOURCC_PATTERN_LIST;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&	SUCCEEDED( DM_SaveStyleHeader( pIRiffStream ) )
			&&	pIRiffStream->CreateChunk( &ckPattern, MMIO_CREATELIST ) == 0
			&&  SUCCEEDED( DM_SaveSinglePattern( pIRiffStream ) )
			&&	pIRiffStream->Ascend( &ckPattern, 0 ) == 0
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				if( fClearDirty )
				{
					SetModified( FALSE );
				}
				hr = S_OK;
			}
			pIRiffStream->Release();
		}
	}

	else if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_SinglePattern ) )
	{
		// Single Pattern (Direct Music format)
		// Used by clipboard and MIDI Manager
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_PATTERN_LIST;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( DM_SaveSinglePattern( pIRiffStream ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				if( fClearDirty )
				{
					SetModified( FALSE );
				}
				hr = S_OK;
			}
			pIRiffStream->Release();
		}
	}

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdTimelineCallback implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdTimelineCallback::OnDataChanged

HRESULT CDirectMusicPattern::OnDataChanged( IUnknown* punkStripMgr )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( punkStripMgr == NULL )
	{
		return E_POINTER;
	}

	// Check if this is a change from the variation choices editor
	IDMUSProdNode *pIVarChoicesNode;
	if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IDMUSProdNode, (void **)&pIVarChoicesNode ) ) )
	{
		// Find the part that has this var choices pointer
		POSITION pos = m_lstPartRefs.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each part ref
			CDirectMusicPartRef* pPartRef = m_lstPartRefs.GetNext( pos );

			// Check if we found the editor
			if( pPartRef->m_pVarChoicesNode == pIVarChoicesNode )
			{
				// QI the variation choices node for its IPersistStream interface
				IPersistStream *pIPersistStream;
				if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
				{
					// QI the variation choices node for its IVarChoices interface
					IVarChoices *pIVarChoices;
					if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IVarChoices, (void **)&pIVarChoices ) ) )
					{
						// Try and get undo text to display
						BSTR bstrUndoText;
						if( SUCCEEDED( pIVarChoices->GetUndoText( &bstrUndoText ) ) )
						{
							m_pMIDIMgr->m_strUndoString = bstrUndoText;
							m_pMIDIMgr->m_nUndoString = 0;
							::SysFreeString( bstrUndoText );
						}

						// Create a new part, if needed
						m_pMIDIMgr->PreChangePartRef( pPartRef );

						pPartRef->m_pDMPart->UpdateFromVarChoices( m_pMIDIMgr->m_pIFramework, pIPersistStream );

						// Redraw this part
						m_pMIDIMgr->RefreshPartDisplay( pPartRef->m_pDMPart, ALL_VARIATIONS, false, false );

						// Let the object know about the changes
						m_pMIDIMgr->UpdateOnDataChanged( 0 );

						pIVarChoices->Release();
					}
					pIPersistStream->Release();
				}
				break;
			}
		}

		pIVarChoicesNode->Release();
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern additional functions


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::CalcLength

MUSIC_TIME CDirectMusicPattern::CalcLength( void ) const
{
	long lClocksPerBeat = DM_PPQNx4 / m_TimeSignature.m_bBeat;
	long lClocksPerMeasure = lClocksPerBeat * (long)m_TimeSignature.m_bBeatsPerMeasure;
	
	MUSIC_TIME mtLength = lClocksPerMeasure * (long)m_wNbrMeasures;

	return mtLength;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::OldMusicValueToNote

/*
unsigned char CDirectMusicPattern::OldMusicValueToNote(

unsigned short value,   // Music value to convert.
char scalevalue,        // Scale value if chord failes.
long keypattern,        // Description of key as interval pattern.
char keyroot,           // Root note of key.
long chordpattern,      // Description of chord as interval pattern.
char chordroot,         // Root note of chord.
char count)             // Total notes in chord.

{
unsigned char   result ;
char            octpart   = (char)(value >> 12) ;
char            chordpart = (char)((value >> 8) & 0xF) ;
char            keypart   = (char)((value >> 4) & 0xF) ;
char            accpart   = (char)(value & 0xF) ;
unsigned char   bits      = (unsigned char) (count & 0xF0) ;

    count  &= CHORD_COUNT ;

    result  = unsigned char(12 * octpart) ;
    result += chordroot ;

    if( accpart > 8 )
        accpart -= 16 ;

    if( count ) {
        if( bits & CHORD_FOUR ) {
            if( count > 4 )
                chordpart += (count - 4) ;
        } else {
            if( count > 3 )
                chordpart += (count - 3) ;
        }
    }

//    if( chordpart ) {
        for( ;  chordpattern ;  result++ ) {
            if( chordpattern & 1L ) {
                if( !chordpart )
                    break ;
                chordpart-- ;
            }
            chordpattern = chordpattern >> 1L ;
            if( !chordpattern ) {
                if( !scalevalue )
                    return( 0 ) ;
                result  = unsigned char(12 * octpart) ;
                result += chordroot ;
                keypart = char(scalevalue >> 4) ;
                accpart = char(scalevalue & 0x0F) ;
                break ;
            }
        }
//    }

    if( keypart ) {
        keypattern |= (keypattern << 12L) ;
        keypattern  = keypattern >> (LONG)((result - keyroot) % 12) ;
        for( ;  keypattern ;  result++ ) {
            if( keypattern & 1L ) {
                if( !keypart )
                    break ;
                keypart-- ;
            }
            keypattern = keypattern >> 1L ;
        }
    }

    result += unsigned char(accpart) ;
    return( result ) ;

}
*/


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::OldNoteToMusicValue

/*
unsigned short CDirectMusicPattern::OldNoteToMusicValue(

unsigned char note,     // MIDI note to convert.
long keypattern,        // Description of key as interval pattern.
char keyroot,           // Root note of key.
long chordpattern,      // Description of chord as interval pattern.
char chordroot)         // Root note of chord.

{
unsigned char   octpart = 0 ;
unsigned char   chordpart ;
unsigned char   keypart = (BYTE)-1 ;       
unsigned char   accpart = 0 ;
unsigned char   scan, test, base, last ;    // was char
long            pattern ;
short           testa, testb ;


    scan = chordroot ;

	// If we're trying to play a note below the bottom of our chord, forget it
	if( note < scan)
	{
		return 0;
	}

    while( scan < (note - 24) )
    {
        scan += 12 ;
        octpart++ ;
    }

    base = scan ;

    for( ;  base<=note ;  base+=12 )
    {
        chordpart = (unsigned char)-1 ;
        pattern   = chordpattern ;
        scan      = last = base ;
        if( scan == note )
            return( unsigned short (octpart << 12) ) ;           // if octave, return.
        for( ;  pattern ;  pattern=pattern >> 1 )
        {
            if( pattern & 1 )                   // chord interval?
            {                 
                if( scan == note )              // note in chord?
                {            
                    chordpart++ ;
                    return(unsigned short ((octpart << 12) | (chordpart << 8))) ; // yes, return.
                }
                else if (scan > note)           // above note?
                {         
                    test = scan ;
                    break ;                     // go on to key.
                }
                chordpart++ ;
                last = scan ;
            }
            scan++ ;
        }
        if( !pattern )                          // end of chord.
        {                        
            test = unsigned char(base + 12) ;                  // set to next note.
        }
        octpart++ ;
        if( test > note )
        {
            break ;                             // above our note?
        }
    }

    octpart-- ;

//  To get here, the note is not in the chord.  Scan should show the last
//  note in the chord.  octpart and chordpart have their final values.
//  Now, increment up the key to find the match.

    scan        = last ;
	pattern		= keypattern | keypattern << 12 ;
    pattern		= pattern >> ((scan - keyroot) % 12) ;

    for( ;  pattern ;  pattern=pattern >> 1 )
    {
        if( 1 & pattern )
        {
            keypart++ ;
            accpart = 0 ;
        }
        else
        {
            accpart++ ;
        }
        if( scan == note )
            break ;
        scan++;
    }

    if( accpart && keypart )
    {
        testa = short((octpart << 12) + (chordpart << 8) + (keypart << 4) + accpart + 1);
        testb = short((octpart << 12) + ((chordpart + 1) << 8) + 0);
        testa = OldMusicValueToNote( testa, 0, keypattern, keyroot,
                                     chordpattern, chordroot, (char)0 );
        testb = OldMusicValueToNote( testb, 0, keypattern, keyroot,
                                     chordpattern, chordroot, (char)0 );
        if( testa == testb )
        {
            chordpart++ ;
            keypart = 0 ;
            accpart = -1 ;
        }
    }

	// If the conversion didn't find an exact match, fudge accpart to make it work
	testa = short((octpart << 12) + (chordpart << 8) + (keypart << 4) + (accpart & 0xF));
    testa = OldMusicValueToNote( testa, 0, keypattern, keyroot,
                                 chordpattern, chordroot, (char)0 );

	if( testa != note )
	{
		accpart += note - testa;
	}

    return unsigned short((octpart << 12) + (chordpart << 8) + (keypart << 4) + (accpart & 0xF));

}
*/


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_SavePatternEditInfo

HRESULT CDirectMusicPattern::DM_SavePatternEditInfo( IDMUSProdRIFFStream* pIRiffStream )
{
    MMCKINFO ckMain;
    HRESULT hr = S_OK;

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	ASSERT( m_pMIDIMgr != NULL );
	if ( m_pMIDIMgr == NULL )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Get additional stream information
	IStream* pIStream;
	pIStream = pIRiffStream->GetStream();
	if( pIStream )
	{
		IDMUSProdPersistInfo* pPersistInfo;
		if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
		{
			pPersistInfo->GetStreamInfo( &StreamInfo );
			pPersistInfo->Release();
			pPersistInfo = NULL;
		}
		pIStream->Release();
	}

	if( StreamInfo.ftFileType != FT_DESIGN )
	{
		hr = S_FALSE;
		goto ON_ERROR;
	}

	// Write DMUS_FOURCC_PATTERN_DESIGN LIST header
	ckMain.fccType = DMUS_FOURCC_PATTERN_DESIGN;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Chord strip info
	hr = m_pMIDIMgr->SaveChordStripDesignData( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Write Quantize info
	hr = m_pMIDIMgr->SaveQuantizeDesignData( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Write Velocitize info
	hr = m_pMIDIMgr->SaveVelocitizeDesignData( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_LoadPatternEditInfo

HRESULT CDirectMusicPattern::DM_LoadPatternEditInfo( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain,
													 BOOL* fLoadedChordStripDesign )
{
    IStream* pIStream;
    HRESULT  hr = S_OK;
	MMCKINFO ck;

    ASSERT( m_pMIDIMgr != NULL );
	ASSERT( pIRiffStream != NULL );
    ASSERT( pckMain != NULL );

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

	*fLoadedChordStripDesign = FALSE;

	// Load the Pattern design info
    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_CHORDSTRIP_LIST:
						hr = m_pMIDIMgr->LoadChordStripDesignData( pIRiffStream, &ck );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
						*fLoadedChordStripDesign = TRUE;
						break;
				}
				break;

			case DMUS_FOURCC_QUANTIZE_CHUNK:
				hr = m_pMIDIMgr->LoadQuantizeDesignData( pIRiffStream, &ck );
				if( FAILED( hr ) )
				{
			        goto ON_ERROR;
				}
				break;

			case DMUS_FOURCC_VELOCITIZE_CHUNK:
				hr = m_pMIDIMgr->LoadVelocitizeDesignData( pIRiffStream, &ck );
				if( FAILED( hr ) )
				{
			        goto ON_ERROR;
				}
				break;
		}

        pIRiffStream->Ascend( &ck, 0 );
    }

ON_ERROR:
    pIStream->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_SaveStyleHeader

HRESULT CDirectMusicPattern::DM_SaveStyleHeader( IDMUSProdRIFFStream* pIRiffStream )
{
    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Style header chunk header
    HRESULT hr = S_OK;
	MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_STYLE_CHUNK;
    if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	DMUS_IO_STYLE oDMusIOStyle;
	ZeroMemory( &oDMusIOStyle, sizeof( DMUS_IO_STYLE ) );

	oDMusIOStyle.timeSig.bBeat = m_TimeSignature.m_bBeat;
	oDMusIOStyle.timeSig.bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
	oDMusIOStyle.timeSig.wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;
	oDMusIOStyle.dblTempo = 120.00;

	// Write Rhythm chunk data
	DWORD dwBytesWritten;
	hr = pIStream->Write( &oDMusIOStyle, sizeof(DMUS_IO_STYLE), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_STYLE) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_SavePatternChunk

HRESULT CDirectMusicPattern::DM_SavePatternChunk( IDMUSProdRIFFStream* pIRiffStream )
{
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_PATTERN oDMPattern;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );
	if ( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	// Write Pattern chunk header
    ck.ckid = DMUS_FOURCC_PATTERN_CHUNK;
    if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_PATTERN
	memset( &oDMPattern, 0, sizeof(DMUS_IO_PATTERN) );

	oDMPattern.timeSig.bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
	oDMPattern.timeSig.bBeat = m_TimeSignature.m_bBeat;
	oDMPattern.timeSig.wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;

	oDMPattern.bGrooveBottom = m_bGrooveBottom;
	oDMPattern.bGrooveTop = m_bGrooveTop;
	oDMPattern.bDestGrooveBottom = m_bDestGrooveBottom;
	oDMPattern.bDestGrooveTop = m_bDestGrooveTop;

	oDMPattern.dwFlags = m_dwFlags;

	oDMPattern.wEmbellishment = m_wEmbellishment;
	oDMPattern.wNbrMeasures = m_wNbrMeasures;

	// Write Pattern chunk data
	hr = pIStream->Write( &oDMPattern, sizeof(DMUS_IO_PATTERN), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_PATTERN) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_SaveSinglePattern

HRESULT CDirectMusicPattern::DM_SaveSinglePattern( IDMUSProdRIFFStream* pIRiffStream )
{
	CTypedPtrList<CPtrList, CDirectMusicPart*> lstParts;
	CDirectMusicPartRef* pPartRef;
	CDirectMusicPart* pPart;
	HRESULT hr = S_OK;
	IStream* pIStream;
	POSITION pos;
	POSITION pos2;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

// Save Pattern chunk
	hr = DM_SavePatternChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Pattern rhythm
	hr = DM_SavePatternRhythm( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Pattern info
	hr = DM_SavePatternInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Motif Settings
	hr = DM_SaveMotifSettingsChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Pattern Edit info
	hr = DM_SavePatternEditInfo( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Build list of Parts
    pos = m_lstPartRefs.GetHeadPosition();
    while( pos )
    {
        pPartRef = m_lstPartRefs.GetNext( pos );
		ASSERT( pPartRef->m_pDMPart != NULL );

		pos2 = lstParts.Find( pPartRef->m_pDMPart );
		if( pos2 == NULL )
		{
			lstParts.AddTail( pPartRef->m_pDMPart ); 
		}
    }

// Save Parts
    pos = lstParts.GetHeadPosition();
    while( pos )
    {
        pPart = lstParts.GetNext( pos );

		hr = pPart->DM_SavePart( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
    }

// Save Part References
    pos = m_lstPartRefs.GetHeadPosition();
    while( pos )
    {
        pPartRef = m_lstPartRefs.GetNext( pos );

		hr = pPartRef->DM_SavePartRef( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
    }

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::FindPartRefByGUID

CDirectMusicPartRef* CDirectMusicPattern::FindPartRefByGUID( GUID guid ) const
{
	CDirectMusicPartRef *pPartRef;

    POSITION pos = m_lstPartRefs.GetHeadPosition();

    while( pos )
    {
        pPartRef = m_lstPartRefs.GetNext( pos );

		if( IsEqualGUID(guid, pPartRef->m_pDMPart->m_guidPartID) )
		{
			return pPartRef;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_LoadPattern

HRESULT CDirectMusicPattern::DM_LoadPattern( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	CDirectMusicPartRef* pPartRef;
	CDirectMusicPart* pPart;
    IStream*    pIStream;
    HRESULT     hr = S_OK;
	DWORD		dwByteCount;
	DWORD		dwSize;
	MMCKINFO	ck;
	MMCKINFO	ckList;
	int			i;
	BOOL	    fLoadedChordStripDesign = FALSE;

    ASSERT( m_pMIDIMgr != NULL );
	ASSERT( pIRiffStream != NULL );
    ASSERT( pckMain != NULL );

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

	// Load the Pattern
    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_PATTERN_CHUNK:
			{
				DMUS_IO_PATTERN iDMPattern;

				ZeroMemory( &iDMPattern, sizeof( DMUS_IO_PATTERN ) );

				dwSize = min( ck.cksize, sizeof( DMUS_IO_PATTERN ) );
				hr = pIStream->Read( &iDMPattern, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_TimeSignature.m_bBeatsPerMeasure = iDMPattern.timeSig.bBeatsPerMeasure;
				m_TimeSignature.m_bBeat = iDMPattern.timeSig.bBeat;
				m_TimeSignature.m_wGridsPerBeat = iDMPattern.timeSig.wGridsPerBeat;
				m_wNbrMeasures = iDMPattern.wNbrMeasures;
				m_bGrooveBottom = iDMPattern.bGrooveBottom;
				m_bGrooveTop = iDMPattern.bGrooveTop;
				m_bDestGrooveBottom = iDMPattern.bDestGrooveBottom;
				m_bDestGrooveTop = iDMPattern.bDestGrooveTop;
				m_wEmbellishment = iDMPattern.wEmbellishment;
				m_dwFlags = iDMPattern.dwFlags;
				break;
			}

			case DMUS_FOURCC_PATTERN_UI_CHUNK:
			{
				ioDMPatternUI iDMPatternUI;

				dwSize = min( ck.cksize, sizeof( ioDMPatternUI ) );
				hr = pIStream->Read( &iDMPatternUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( dwSize > sizeof( WORD ) )
				{
					m_dwDefaultKeyPattern = iDMPatternUI.m_dwDefaultKeyPattern;
					m_dwDefaultChordPattern = iDMPatternUI.m_dwDefaultChordPattern;
					m_bDefaultKeyRoot = iDMPatternUI.m_bDefaultKeyRoot;
					m_bDefaultChordRoot = iDMPatternUI.m_bDefaultChordRoot;
					if(dwSize > (2*sizeof(DWORD) + 2*sizeof(BYTE)))
					{
						/* not needed: ioDMPatternUI carries this info
						hr = pIStream->Read(&m_fChordFlatsNotSharps, sizeof(BOOL), &dwByteCount);
						if(FAILED(hr) || dwByteCount != sizeof(BOOL))
						{
							hr = E_FAIL;
							goto ON_ERROR;
						}
						hr = pIStream->Read(&m_fKeyFlatsNotSharps, sizeof(BOOL), &dwByteCount);
						if(FAILED(hr) || dwByteCount != sizeof(BOOL))
						{
							hr = E_FAIL;
							goto ON_ERROR;
						}
						*/
						m_fKeyFlatsNotSharps = iDMPatternUI.m_fDefaultKeyFlatsNotSharps;
						m_fChordFlatsNotSharps = iDMPatternUI.m_fDefaultChordFlatsNotSharps;
					}
					else
					{
						m_fKeyFlatsNotSharps = FALSE;
						m_fChordFlatsNotSharps = FALSE;
					}
				}
				break;
			}

			case DMUS_FOURCC_RHYTHM_CHUNK:
				if( m_pRhythmMap )
				{
					delete [] m_pRhythmMap;
					m_pRhythmMap = NULL;
				}
				m_pRhythmMap = new DWORD[m_wNbrMeasures];
				if( m_pRhythmMap == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}
				for( i = 0 ;  i < m_wNbrMeasures ;  i++ )
				{
					hr = pIStream->Read( &m_pRhythmMap[i], sizeof(DWORD), &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != sizeof(DWORD) )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
				}
				break;

			case DMUS_FOURCC_MOTIFSETTINGS_CHUNK:
			{
				DMUS_IO_MOTIFSETTINGS dmusMotifSettingsIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_MOTIFSETTINGS ) );
				hr = pIStream->Read( &dmusMotifSettingsIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwRepeats = dmusMotifSettingsIO.dwRepeats;
				m_mtPlayStart = dmusMotifSettingsIO.mtPlayStart;
				m_mtLoopStart = dmusMotifSettingsIO.mtLoopStart;
				m_mtLoopEnd = dmusMotifSettingsIO.mtLoopEnd;
				m_dwResolution = dmusMotifSettingsIO.dwResolution;
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case DMUS_FOURCC_PATTERN_DESIGN:
						hr = DM_LoadPatternEditInfo( pIRiffStream, &ck, &fLoadedChordStripDesign );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
						break;

					case DMUS_FOURCC_PART_LIST:		// only in GUID_SinglePattern format
						pPart = m_pMIDIMgr->AllocPart();
						if( pPart == NULL )
						{
							hr = E_OUTOFMEMORY ;
			                goto ON_ERROR;
						}
						hr = pPart->DM_LoadPart( pIRiffStream, &ck );
						if( FAILED( hr ) )
						{
							m_pMIDIMgr->DeletePart( pPart );
			                goto ON_ERROR;
						}
						if( hr == S_FALSE )
						{
							// Bypass this Part because Style already contains a Part
							// whose GUID matches pPart->m_guidPartID
							m_pMIDIMgr->DeletePart( pPart );
						}
						break;

					case DMUS_FOURCC_PARTREF_LIST:
						// Make sure we have a Chord strip
						if( (fLoadedChordStripDesign == FALSE)
						&&	(m_pMIDIMgr->m_pIDMTrack == NULL) )
						{
							fLoadedChordStripDesign = TRUE;
							hr = m_pMIDIMgr->CreateDefaultChordStrip();
							if( FAILED( hr ) )
							{
								goto ON_ERROR;
							}
						}
						pPartRef = AllocPartRef();
						if( pPartRef == NULL )
						{
							hr = E_OUTOFMEMORY ;
			                goto ON_ERROR;
						}
						hr = pPartRef->DM_LoadPartRef( pIRiffStream, &ck );
						if( FAILED( hr ) )
						{
							DeletePartRef( pPartRef );
			                goto ON_ERROR;
						}
						if( pPartRef->m_strName.IsEmpty() )
						{
							CString cstrTrack;
							cstrTrack.LoadString( IDS_TRACK );

							pPartRef->m_strName.Format( cstrTrack, pPartRef->m_dwPChannel + 1 );
						}
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

	if( (m_pMIDIMgr->m_pIDMTrack == NULL)
	&&	(fLoadedChordStripDesign == FALSE) )
	{
		hr = m_pMIDIMgr->CreateDefaultChordStrip();
	}

	// Not needed - each PartRef does this when loaded.
	// Recompute all CDirectMusicStyleNotes' m_bMIDIValue value
	//RecalcMIDIValues();

ON_ERROR:
    pIStream->Release();
    return hr;
}

	
/*
inline short MusicValueOctave(WORD wMusicValue)
{ return short((wMusicValue >> 12) & 0xf) * 12; }

inline short MusicValueAccidentals(WORD wMusicValue)
{ 
	short acc = short(wMusicValue & 0xf);
	return (acc > 8) ? acc - 16 : acc;
}

inline short BitsInChord(DWORD dwChordPattern)
{
	
	for (short nResult = 0; dwChordPattern != 0; dwChordPattern >>= 1)
		if (dwChordPattern & 1) nResult++;
	return nResult;
}

short MusicValueIntervals(WORD wMusicValue, BYTE bPlayModes, const DMUS_SUBCHORD &SubChord, BYTE bRoot)
{ 
	if ((bPlayModes & DMUS_PLAYMODE_CHORD_INTERVALS) || (bPlayModes & DMUS_PLAYMODE_SCALE_INTERVALS))
	{
		const DWORD dwDefaultScale = 
			(SubChord.dwScalePattern) ? (SubChord.dwScalePattern) : DEFAULT_SCALE_PATTERN;
		const short nChordPosition = (wMusicValue >> 8) & 0xf;
		const short nScalePosition = (wMusicValue >> 4) & 0xf;
		const short nChordBits = BitsInChord(SubChord.dwChordPattern);
		short nSemitones;
		short nTransposetones;
		DWORD dwPattern;
		short nPosition;
		BYTE bOctRoot = bRoot % 12; // root in one octave
		// if using chord intervals and the note is in the chord
		if ((bPlayModes & DMUS_PLAYMODE_CHORD_INTERVALS) && 
			!nScalePosition &&
			(nChordPosition < nChordBits) )
		{
			nSemitones = 0;
			nTransposetones = bRoot + MusicValueAccidentals(wMusicValue);
			dwPattern = SubChord.dwChordPattern;
			nPosition = nChordPosition;
		}
		// if using chord intervals and note is inside the chord (including 6ths)
		else if ((bPlayModes & DMUS_PLAYMODE_CHORD_INTERVALS) &&
				 (nChordPosition < nChordBits) )
//				 (nChordPosition + nScalePosition < nChordBits) )
		{
			nSemitones = 0;
			dwPattern = SubChord.dwChordPattern;
			nPosition = nChordPosition;
			if (nPosition > 0)
			{
				do
				{
					dwPattern >>= 1; // this will ignore the first bit
					nSemitones++;
					if (dwPattern & 1)
					{
						nPosition--;
					}
					if (!dwPattern)
					{
						ASSERT (0); // This shouldn't happen...
						break;
					}
				} while (nPosition > 0);
			}

			nSemitones += bOctRoot;
			nTransposetones = MusicValueAccidentals(wMusicValue) + bRoot - bOctRoot;
			dwPattern = dwDefaultScale >> nSemitones;  // start comparing partway through the pattern
			nPosition = nScalePosition;
		}
		// if using scale intervals
		else if (bPlayModes & DMUS_PLAYMODE_SCALE_INTERVALS)
		{
			nSemitones = bOctRoot;
			nTransposetones = MusicValueAccidentals(wMusicValue) + bRoot - bOctRoot;
			dwPattern = dwDefaultScale >> bOctRoot;  // start comparing partway through the pattern
			nPosition = nChordPosition * 2 + nScalePosition;
		}
		else
		{
			return -1;
		}
		nPosition++; // Now nPosition corresponds to actual scale positions
		for (; nPosition > 0; dwPattern >>= 1)
		{
			nSemitones++;
			if (dwPattern & 1)
			{
				nPosition--;
			}
			if (!dwPattern)
			{
				ASSERT (0); // This shouldn't happen...
				break;
			}
		}
		nSemitones--; // the loop counts one too many semitones...
		return nSemitones + nTransposetones;
	}
	else
	{
		// should be impossible for 2.5 format
		return bRoot + wMusicValue;
	}
}

inline short MusicValueChord(WORD wMusicValue, BYTE bPlayModes, const DMUS_SUBCHORD &SubChord)
{ 
	// first, get the root for transposition.
	BYTE bRoot = 0;
	if (bPlayModes & DMUS_PLAYMODE_CHORD_ROOT)
		bRoot = SubChord.bChordRoot + SubChord.bScaleRoot;
	else if (bPlayModes & DMUS_PLAYMODE_SCALE_ROOT)
		bRoot = SubChord.bScaleRoot;
	// Next, get an interval and combine it with the root.
	return MusicValueIntervals(wMusicValue, bPlayModes, SubChord, bRoot);
}
*/

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::MusicValueToNote

/*
BYTE CDirectMusicPattern::MusicValueToNote( WORD wMusicValue, BYTE bPlayModes,
											const DMUS_SUBCHORD &dmSubChord )
{
	ASSERT( bPlayModes != DMUS_PLAYMODE_NONE );

	if( bPlayModes == DMUS_PLAYMODE_FIXED )
	{
		return (BYTE)wMusicValue;
	}
	else if (bPlayModes == DMUS_PLAYMODE_CHORD_ROOT) // fixed to chord
	{
		return BYTE(wMusicValue + dmSubChord.bChordRoot + dmSubChord.bScaleRoot);
	}
	else if (bPlayModes == DMUS_PLAYMODE_SCALE_ROOT) // fixed to scale
	{
		return BYTE(wMusicValue + dmSubChord.bScaleRoot);
	}

	short nChordValue = MusicValueChord(wMusicValue, bPlayModes, dmSubChord);
	if (nChordValue >= 0)
	{
		// If DMUS_PLAYMODE_CHORD_ROOT is set, take the result down an octave.
		if ((bPlayModes & DMUS_PLAYMODE_CHORD_ROOT))
			return MusicValueOctave(wMusicValue) + nChordValue - 12;
		else
			return MusicValueOctave(wMusicValue) + nChordValue;
	}
	else
		return 0;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::NoteToMusicValue

/*
WORD CDirectMusicPattern::NoteToMusicValue( BYTE bMIDINote, BYTE bPlayModes,
											const DMUS_SUBCHORD &dmSubChord )
{
	ASSERT( bPlayModes != DMUS_PLAYMODE_NONE );

	if( bPlayModes == DMUS_PLAYMODE_FIXED )
	{
		return (WORD)bMIDINote;
	}
	else if( bPlayModes == DMUS_PLAYMODE_FIXEDTOSCALE )
	{
		return (bMIDINote >= dmSubChord.bScaleRoot) ?
			WORD(bMIDINote - dmSubChord.bScaleRoot) : 0;
	}
	else if( bPlayModes == DMUS_PLAYMODE_FIXEDTOCHORD )
	{
		return (bMIDINote >= dmSubChord.bChordRoot + dmSubChord.bScaleRoot) ?
			WORD(bMIDINote - dmSubChord.bChordRoot - dmSubChord.bScaleRoot) : 0;
	}

	WORD wNewMusicValue = OldNoteToMusicValue( bMIDINote, dmSubChord.dwScalePattern,
			dmSubChord.bScaleRoot, dmSubChord.dwChordPattern, dmSubChord.bChordRoot );

	// If DMUS_PLAYMODE_CHORD_ROOT is set, take the result up an octave.
	if( (bPlayModes & DMUS_PLAYMODE_CHORD_ROOT) ||
		((bPlayModes & DMUS_PLAYMODE_KEY_ROOT) && (dmSubChord.bChordRoot > 11)) )
	{
		if( (wNewMusicValue & 0xF000) != 0xF000 )
		{
			wNewMusicValue += 0x1000;
		}
	}

	return wNewMusicValue;
}
*/


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetNodeName

void CDirectMusicPattern::SetNodeName( LPCTSTR strName )
{
	ASSERT( m_pMIDIMgr->m_pIDMTrack );

	if( m_strName.CompareNoCase( strName ) != 0 )
	{
		m_strName = strName;

		// Let the object know about the changes
		m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PATTERN_NAME );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetNbrMeasures

void CDirectMusicPattern::SetNbrMeasures( WORD wNbrMeasures )
{
	ASSERT( m_pMIDIMgr->m_pIDMTrack );

	if( wNbrMeasures != m_wNbrMeasures )
	{
		// Change Pattern rhythm map
		DWORD* pRhythmMap = m_pRhythmMap;
		m_pRhythmMap = new DWORD[wNbrMeasures];
		if( m_pRhythmMap )
		{
			for( int i = 0 ;  i < wNbrMeasures ;  ++i )
			{
				if( i < m_wNbrMeasures )
				{
					m_pRhythmMap[i] = pRhythmMap[i];
				}
				else
				{
					m_pRhythmMap[i] = 0;
				}
			}
		}
		if( pRhythmMap )
		{
			delete [] pRhythmMap;
		}

		// Change Pattern length
		WORD wOrigNbrMeasures = m_wNbrMeasures;
		m_wNbrMeasures = wNbrMeasures; 

		// Change length of all Parts
		CDirectMusicPartRef* pPartRef;

		POSITION pos = m_lstPartRefs.GetHeadPosition();
		while( pos )
		{
			pPartRef = m_lstPartRefs.GetNext( pos );

			ASSERT( pPartRef->m_pDMPart != NULL);
			
			// Only change when length of part equals original pattern length
			// and time signature of part equals time signature of pattern
			if( pPartRef->m_pDMPart->m_wNbrMeasures == wOrigNbrMeasures 
			&&  pPartRef->m_pDMPart->m_TimeSignature.m_bBeat == m_TimeSignature.m_bBeat 
			&&  pPartRef->m_pDMPart->m_TimeSignature.m_bBeatsPerMeasure == m_TimeSignature.m_bBeatsPerMeasure 
			&&  pPartRef->m_pDMPart->m_TimeSignature.m_wGridsPerBeat == m_TimeSignature.m_wGridsPerBeat )
			{
				if( pPartRef->m_fHardLink == FALSE )
				{
					m_pMIDIMgr->PreChangePartRef( pPartRef );
					pPartRef->m_pDMPart->SetNbrMeasures( wNbrMeasures );
				}
				else
				{
					DWORD dwLinkCount = pPartRef->m_pDMPart->m_dwUseCount;
					POSITION pos2 = m_lstPartRefs.GetHeadPosition();
					while( pos2 && dwLinkCount )
					{
						CDirectMusicPartRef* pTmpPartRef = m_lstPartRefs.GetNext( pos2 );
						if( pTmpPartRef->m_fHardLink
						&&	pTmpPartRef->m_pDMPart == pPartRef->m_pDMPart )
						{
							dwLinkCount--;
						}
					}

					// If all hard links to this part are in this pattern, change the part's length
					if( dwLinkCount == 0 )
					{
						m_pMIDIMgr->PreChangePartRef( pPartRef );
						pPartRef->m_pDMPart->SetNbrMeasures( wNbrMeasures );
					}
				}
			}
		}

		// Redraw this pattern
		m_pMIDIMgr->InvalidateAllStrips();

		// Let the object know about the changes
		m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PATTERN_LENGTH );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetTimeSignature

void CDirectMusicPattern::SetTimeSignature( DirectMusicTimeSig timeSig, BOOL /*fSyncWithEngine*/ )
{
	if( timeSig.m_bBeatsPerMeasure != m_TimeSignature.m_bBeatsPerMeasure
	||  timeSig.m_bBeat != m_TimeSignature.m_bBeat
	||  timeSig.m_wGridsPerBeat != m_TimeSignature.m_wGridsPerBeat )
	{
		// Change the TimeSig of all parts that have the same TimeSig as the pattern
		POSITION pos = m_pMIDIMgr->m_lstStyleParts.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to the part
			CDirectMusicPart* pPart = m_pMIDIMgr->m_lstStyleParts.GetNext( pos );

			// Check if the part's time sig is the same as the pattern's
			if( m_TimeSignature.m_bBeatsPerMeasure == pPart->m_TimeSignature.m_bBeatsPerMeasure
			&&  m_TimeSignature.m_bBeat == pPart->m_TimeSignature.m_bBeat
			&&  m_TimeSignature.m_wGridsPerBeat == pPart->m_TimeSignature.m_wGridsPerBeat )
			{
				// Yes - update it
				pPart->SetTimeSignature( timeSig );

				// Update the display of this part
				m_pMIDIMgr->RefreshPartDisplay( pPart, ALL_VARIATIONS, TRUE, TRUE );
			}
		}

		// Update the local timesig information in each PianoRollStrip
		POSITION position = m_pMIDIMgr->m_pPRSList.GetHeadPosition();
		while (position != NULL)
		{
			CPianoRollStrip* pPianoRollStrip = m_pMIDIMgr->m_pPRSList.GetNext(position);

			pPianoRollStrip->m_lBeatClocks = pPianoRollStrip->m_pPartRef->m_pDMPart->m_mtClocksPerBeat;
			pPianoRollStrip->m_lMeasureClocks = pPianoRollStrip->m_lBeatClocks * pPianoRollStrip->m_pPartRef->m_pDMPart->m_TimeSignature.m_bBeatsPerMeasure;
			pPianoRollStrip->m_lGridClocks = pPianoRollStrip->m_pPartRef->m_pDMPart->m_mtClocksPerGrid;
		}

		// Calculate current length
		DWORD dwLength = CalcLength();

		// Change Pattern time signature
		m_TimeSignature = timeSig;
		
		// Adjust Pattern length
		DWORD dwClocksPerBeat = DM_PPQNx4 / m_TimeSignature.m_bBeat;
		DWORD dwClocksPerMeasure = dwClocksPerBeat * (DWORD)m_TimeSignature.m_bBeatsPerMeasure;
		WORD wNbrMeasures = (WORD)(dwLength / dwClocksPerMeasure);
		if( dwLength % dwClocksPerMeasure )
		{
			wNbrMeasures++;
		}

		if( wNbrMeasures != m_wNbrMeasures )
		{
			// Change Pattern rhythm map
			DWORD* pRhythmMap = m_pRhythmMap;
			m_pRhythmMap = new DWORD[wNbrMeasures];
			if( m_pRhythmMap )
			{
				if( pRhythmMap )
				{
					for( int i = 0 ;  i < wNbrMeasures ;  ++i )
					{
						if( i < m_wNbrMeasures )
						{
							m_pRhythmMap[i] = pRhythmMap[i];
						}
						else
						{
							m_pRhythmMap[i] = 0;
						}
					}
				}
				else
				{
					ZeroMemory( m_pRhythmMap, sizeof( DWORD ) * wNbrMeasures );
				}
			}
			if( pRhythmMap )
			{
				delete [] pRhythmMap;
			}

			// Change Pattern length
			m_wNbrMeasures = wNbrMeasures; 
		}

		// Let the object know about the changes
		if( m_pMIDIMgr->m_pIDMTrack )
		{
			m_pMIDIMgr->UpdateOnDataChanged( IDS_PATTERN_TIMESIG );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetFlags

void CDirectMusicPattern::SetFlags( DWORD dwFlags )
{
	ASSERT( m_pMIDIMgr != NULL );

	if( dwFlags != m_dwFlags )
	{
		// Change flags
		m_dwFlags = dwFlags; 

		// Let the object know about the changes
		m_pMIDIMgr->UpdateOnDataChanged( IDS_UNDO_PATTERN_FLAGS );
	}
}
