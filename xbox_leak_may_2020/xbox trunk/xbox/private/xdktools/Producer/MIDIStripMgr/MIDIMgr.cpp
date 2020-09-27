// MIDIMgr.cpp : Implementation of CMIDIMgr
#include <stdafx.h>
#include <dmusici.h>
#include <DllJazzDataObject.h>
#include <PChannelName.h>
#include "MIDIStripMgr.h"
#include "MIDIMgr.h"
#include "CurveStrip.h"
#include "Templates.h"
#include "Pattern.h"
#include "DialogNewPart.h"
#include <PChannelName.h>
#include "PropPageMgr.h"
#include "SequenceIO.h"
#include "MIDIFileIO.h"
#include "StyleDesigner.h"
#include <riffstrm.h>
#include <SegmentGUIDs.h>
#include "ChordTrack.h"
#include "VarSwitchStrip.h"
#include "SegmentIO.h"
#include "SharedPianoRoll.h"
#include "SegmentDesigner.h"
#include "BandEditor.h"
#include "BandStripMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PCHANGE    0xC0

#define MIDI_BASEPITCH		60

extern const int aChromToScaleSharps[12];
extern const int aChromToScaleFlats[12];
extern const int aChromToSharpAccidentals[12];
extern const int aChromToFlatAccidentals[12];

extern short gnPPQN;
extern BYTE gbChannel;

DMUS_CHORD_PARAM dmSegmentDefaultChord;

void InitializeIOPianoRollDesign( ioPianoRollDesign* pPRD )
{
	ZeroMemory( pPRD, sizeof(ioPianoRollDesign) );
	pPRD->m_dwVariations = 1;
	pPRD->m_crUnselectedNoteColor = COLOR_DEFAULT_UNSELECTED;
	pPRD->m_crSelectedNoteColor = COLOR_DEFAULT_SELECTED;
	pPRD->m_dblVerticalZoom = 0.1;
	pPRD->m_lHeight = DEFAULT_HEIGHT;
	pPRD->m_nStripView = SV_MINIMIZED;
	pPRD->m_lSnapValue = 192;
	//pPRD->m_dwFlags = 0;
	pPRD->m_crOverlappingNoteColor = COLOR_DEFAULT_OVERLAPPING;
	pPRD->m_crAccidentalColor = COLOR_DEFAULT_ACCIDENTAL;
	//pPRD->m_fHybridNotation = FALSE;
	//pPRD->m_dwExtraBars = 0;
}

void ConvertTimeSigs( CDirectMusicEventItem *pEvent, DirectMusicTimeSig &tsOld, CDirectMusicPart *pPart )
{
	ASSERT( pPart != NULL );

	// Convert from the source TimeSig to the destination TimeSig
	MUSIC_TIME mtTmpGrid = CLOCKS_TO_GRID( TS_GRID_TO_CLOCKS( pEvent->m_mtGridStart, tsOld ), pPart );
	// Add the difference to the TimeOffset - I think this should never increase the
	// TimeOffset so it makes the event take on a chord from a different beat (unless that's how
	// it was to begin with.
	pEvent->m_nTimeOffset += TS_GRID_TO_CLOCKS( pEvent->m_mtGridStart, tsOld ) - GRID_TO_CLOCKS( mtTmpGrid, pPart );
	// Set the new s grid start position, offsetting it by mtGrid (the position to paste at)
	pEvent->m_mtGridStart = mtTmpGrid;
}

int CALLBACK EnumFontFamilyCallback( const LOGFONT *pLogfont, const TEXTMETRIC* /*pTextMetric*/, DWORD /*dwData*/, LPARAM lParam )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( pLogfont );

	CString strFontName;
	strFontName.LoadString(IDS_NOTATION_FONT);
	if( pLogfont && _tcscmp( pLogfont->lfFaceName, strFontName ) == 0 )
	{
		CMIDIMgr *pMIDIMgr = (CMIDIMgr *)lParam;
		ASSERT( pMIDIMgr );
		if( pMIDIMgr )
		{
			pMIDIMgr->m_fHasNotationStation = TRUE;
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr

CMIDIMgr::CMIDIMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pTimeline = NULL;
	m_pDMPattern = NULL;
	m_TimeSignature.m_bBeatsPerMeasure = 4;
	m_TimeSignature.m_bBeat = 4;
	m_TimeSignature.m_wGridsPerBeat = 4;
	m_dwNextPatternID = 0;
	m_pPropPageMgr = NULL;
	m_pCopyIDataObject = NULL;
	m_pChordTrack = NULL;
	m_fShuttingDown = FALSE;
	m_fChordStripChanged = FALSE;
	m_fPChannelChange = FALSE;
	m_nUndoString = 0;
	m_pDMProdSegmentNode = NULL;

	// Prepare a default chord
	ZeroMemory( &m_chordDefault, sizeof(DMUS_CHORD_PARAM) );
	wcscpy( m_chordDefault.wszName, L"M7" );
	//m_chordDefault.wMeasure = 0;
	//m_chordDefault.bBeat = 0;
	m_chordDefault.bSubChordCount = 4;
	m_chordDefault.SubChordList[0].dwChordPattern = 0x891;			// M7
	m_chordDefault.SubChordList[0].dwScalePattern = 0xAB5AB5;		// Major
	m_chordDefault.SubChordList[0].dwInversionPoints = 0xffffff;	// default: inversions everywhere
	m_chordDefault.SubChordList[0].dwLevels = 0xfffffff1;
	m_chordDefault.SubChordList[0].bChordRoot = 12;
	//m_chordDefault.SubChordList[0].bScaleRoot = 0;
	m_chordDefault.SubChordList[1] = m_chordDefault.SubChordList[0]; 
	m_chordDefault.SubChordList[1].dwLevels = 0x00000002;
	m_chordDefault.SubChordList[2] = m_chordDefault.SubChordList[0]; 
	m_chordDefault.SubChordList[2].dwLevels = 0x00000004;
	m_chordDefault.SubChordList[3] = m_chordDefault.SubChordList[0]; 
	m_chordDefault.SubChordList[3].dwLevels = 0x00000008;
	m_chordDefault.bKey = 12;
	//m_chordDefault.bFlags = 0;
	m_fDefaultChordFlatsNotSharps = FALSE;

	memcpy( &dmSegmentDefaultChord, &m_chordDefault, sizeof(DMUS_CHORD_PARAM) );

	//	m_lLastTrackOpened = 0x7FFFFFFF;

	m_NotePropData.m_Param.pPart = NULL;
	m_NotePropData.m_Param.dwVariations = 0x00000000;

//	memset( &m_PianoRollData.guid, 0, sizeof(PianoRollData) - sizeof(CString) );
	m_PianoRollData.ts = m_TimeSignature;
	m_PianoRollData.dwVariations = ALL_VARIATIONS;
	m_PianoRollData.crUnselectedNoteColor = COLOR_DEFAULT_UNSELECTED;
	m_PianoRollData.crSelectedNoteColor = COLOR_DEFAULT_SELECTED;
	m_PianoRollData.crOverlappingNoteColor = COLOR_DEFAULT_OVERLAPPING;
	m_PianoRollData.crAccidentalColor = COLOR_DEFAULT_ACCIDENTAL;
	m_PianoRollData.dblVerticalZoom = 0.1;
	m_PianoRollData.lVerticalScroll = 0;
	m_PianoRollData.StripView = SV_NORMAL;
	m_PianoRollData.lSnapValue = TS_GRID_TO_CLOCKS( 1, m_PianoRollData.ts );
	m_PianoRollData.dwExtraBars = 0;
	m_PianoRollData.fPickupBar = FALSE;

//	m_PianoRollData.dwVariations = ALL_VARIATIONS; // default to all variations

	m_fDirty = FALSE;
	m_fNoDisplayUpdate = FALSE;
	m_fPropPageActive = FALSE;
	m_fRecordEnabled = FALSE;
	m_fWindowActive = FALSE;
	m_hStatusBar = NULL;

	m_pActivePianoRollStrip = NULL;
	m_iFocus = 0;
	m_pSegmentState = NULL;
	m_fNoteInserted = FALSE;
	m_fUpdateDirectMusic = TRUE;

	m_pIFramework = NULL;
	m_pIConductor = NULL;
	m_pIPChannelName = NULL;
	m_pIDMPerformance = NULL;
	m_pIStyleNode = NULL;

	m_ptPasteType = TL_PASTE_MERGE;

    InitializeCriticalSection(&m_critsecUpdate);
	m_fCriticalSectionValid = TRUE;

	m_dwGroupBits = 1;

	m_rtLastPlayNoteOffTime = 0;
	m_dwLastPlayNotePChannel = 0;
	m_bLastPlayNoteMIDIValue = 0;
	m_mtLastLoop = 0;
	m_mtCurrentLength = 0;
	m_mtCurrentLoopStart = 0;
	m_mtCurrentLoopEnd = 0;
	m_dwCurrentMaxLoopRepeats = 0;
	m_bTempPatternEditorMode = CHORDSTRIP_MODE_MIDI_CONSTANT;

	// Initialize Quantization variables
	m_qtQuantTarget = QUANTIZE_TARGET_SELECTED;
	m_bQuantResolution = 4;
	m_bQuantStrength = 100;
	m_dwQuantFlags = PATTERN_QUANTIZE_START_TIME;

	// Initialize Velocity variables
	m_vtVelocityTarget = VELOCITY_TARGET_PART;
	m_fVelocityAbsolue = true;
	m_fVelocityPercent = true;
	m_lVelocityAbsChangeStart = 0;
	m_lVelocityAbsChangeEnd = 0;
	m_bVelocityCompressMin = 0;
	m_bVelocityCompressMax = 127;

	m_fDisplayingFlats = FALSE;
	m_nNumAccidentals = 0;
	m_nKeyRoot = 0;
	ZeroMemory(m_aiScalePattern, sizeof(int) * 7);

	m_fHasNotationStation = FALSE;

	HDC hdc = ::GetDC( NULL );
	if( hdc )
	{
		// Create the font to draw the sharps and flats with
		LOGFONT lf;
		memset( &lf, 0 , sizeof(LOGFONT));
		lf.lfCharSet = SYMBOL_CHARSET;
		lf.lfPitchAndFamily = DEFAULT_PITCH;
		//_tcsncpy( lf.lfFaceName, _T("NotationStation"), LF_FACESIZE );
		//lf.lfHeight = 0;
		//lf.lfWidth = 0;
		//lf.lfEscapement = 0;
		//lf.lfOrientation = 0;
		lf.lfWeight = FW_NORMAL;
		//lf.lfItalic = FALSE;
		//lf.lfUnderline = FALSE;
		//lf.lfStrikeOut = FALSE;
		//lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		//lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		//lf.lfQuality = DEFAULT_QUALITY;
		::EnumFontFamiliesEx( hdc, &lf, EnumFontFamilyCallback, (LPARAM) this, 0 );
 		::ReleaseDC( NULL, hdc );
	}

	m_pIDMTrack = NULL;
	m_dwGroupBits = 1;
	m_dwOldGroupBits = 1;
	m_dwIndex = 0;
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags = 0;
}

CMIDIMgr::~CMIDIMgr()
{
	// No Release!
	/*
	if( m_pDMProdSegmentNode )
	{
		m_pDMProdSegmentNode->Release();
		m_pDMProdSegmentNode = NULL;
	}
	*/

	if( m_fCriticalSectionValid )
	{
		DeleteCriticalSection(&m_critsecUpdate);
		m_fCriticalSectionValid = FALSE;
	}
	if( m_pDMPattern )
	{
		delete m_pDMPattern;
		m_pDMPattern = NULL;
	}
	while( !m_lstStyleParts.IsEmpty() )
	{
		delete m_lstStyleParts.RemoveHead();
	}
	while( !m_lstMelodyLoadStructures.IsEmpty() )
	{
		delete m_lstMelodyLoadStructures.RemoveHead();
	}

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
		m_pPropPageMgr = NULL;
	}
	if( m_pCopyIDataObject )
	{
		if(S_OK == OleIsCurrentClipboard(m_pCopyIDataObject))
		{
			OleFlushClipboard();
		}
		m_pCopyIDataObject->Release();
		m_pCopyIDataObject = NULL;
	}
	// This list is emptied by SetClientTimeline(NULL), which should have already been called
	ASSERT( m_pPRSList.IsEmpty() );
	if( !m_pPRSList.IsEmpty() )
	{
		CPianoRollStrip *pPianoRollStrip;
		while ( !m_pPRSList.IsEmpty() )
		{
			pPianoRollStrip = m_pPRSList.RemoveHead();
			KillFocus( pPianoRollStrip );
			pPianoRollStrip->Release();
		}
	}

	if( m_pChordTrack )
	{
		delete m_pChordTrack;
		m_pChordTrack = NULL;
	}

	if( m_pSegmentState )
	{
		m_pSegmentState->Release();
		m_pSegmentState = NULL;
	}

	if( m_pIDMTrack )
	{
		m_pIDMTrack->Release();
		m_pIDMTrack = NULL;
	}

	if( m_pIStyleNode )
	{
		m_pIStyleNode->Release();
		m_pIStyleNode = NULL;
	}

	if( m_pIPChannelName )
	{
		m_pIPChannelName->Release();
		m_pIPChannelName = NULL;
	}

	if( m_pIDMPerformance )
	{
		m_pIDMPerformance->Release();
		m_pIDMPerformance = NULL;
	}

	if( m_pIConductor )
	{
		m_pIConductor->Release();
		m_pIConductor = NULL;
	}

	if( m_pIFramework )
	{
		m_pIFramework->Release();
		m_pIFramework = NULL;
	}

	ASSERT( m_pTimeline == NULL );
}

HRESULT CMIDIMgr::SelectNote( CPianoRollStrip* pPRS, CDirectMusicStyleNote *pDMNote)
{
	BOOL fRefreshPropPage = FALSE;

	ASSERT( pPRS != NULL );
	if ( pPRS == NULL )
	{
		return E_INVALIDARG;
	}

	ASSERT( pPRS->ValidPartRefPtr() );
	if( !pPRS->ValidPartRefPtr() )
	{
		return E_FAIL;
	}

	ASSERT( pDMNote != NULL );
	if ( pDMNote == NULL )
	{
		return E_INVALIDARG;
	}

	if ( pPRS->m_pPartRef->m_pDMPart != m_NotePropData.m_Param.pPart )
	{
		m_NotePropData.m_Param.pPart = pPRS->m_pPartRef->m_pDMPart;
		m_NotePropData.m_Param.dwVariations = pPRS->m_dwVariations;

		fRefreshPropPage = TRUE;
	}

	if (pDMNote->m_fSelected != TRUE)
	{
		pDMNote->m_fSelected = TRUE;
		RefreshPartDisplay( pPRS->m_pPartRef->m_pDMPart, ALL_VARIATIONS, FALSE, FALSE );
		ComputeSelectedPropNote( pPRS->m_pPartRef->m_pDMPart, ALL_VARIATIONS );
		RefreshPropertyPage( pPRS->m_pPartRef->m_pDMPart );
	}
	else if ( fRefreshPropPage )
	{
		RefreshPropertyPage( pPRS->m_pPartRef->m_pDMPart );
	}

	return S_OK;
}

HRESULT CMIDIMgr::UnselectNote( CPianoRollStrip* pPRS, CDirectMusicStyleNote *pDMNote)
{
	BOOL fRefreshPropPage = FALSE;

	ASSERT( pPRS != NULL );
	if ( pPRS == NULL )
	{
		return E_INVALIDARG;
	}

	ASSERT( pPRS->ValidPartRefPtr() );
	if( !pPRS->ValidPartRefPtr() )
	{
		return E_FAIL;
	}

	ASSERT( pDMNote != NULL );
	if ( pDMNote == NULL )
	{
		return E_INVALIDARG;
	}

	if ( pPRS->m_pPartRef->m_pDMPart != m_NotePropData.m_Param.pPart )
	{
		m_NotePropData.m_Param.pPart = pPRS->m_pPartRef->m_pDMPart;
		m_NotePropData.m_Param.dwVariations = pPRS->m_dwVariations;
		
		fRefreshPropPage = TRUE;
	}

	if (pDMNote->m_fSelected == TRUE)
	{
		pDMNote->m_fSelected = FALSE;
		RefreshPartDisplay( pPRS->m_pPartRef->m_pDMPart, ALL_VARIATIONS, FALSE, FALSE );
		ComputeSelectedPropNote( pPRS->m_pPartRef->m_pDMPart, ALL_VARIATIONS );
		RefreshPropertyPage( pPRS->m_pPartRef->m_pDMPart );
	}
	else if ( fRefreshPropPage )
	{
		RefreshPropertyPage( pPRS->m_pPartRef->m_pDMPart );
	}

	return S_OK;
}

HRESULT CMIDIMgr::UnselectAllNotes( CDirectMusicPart* pPart, DWORD dwVariations )
{
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return E_INVALIDARG;
	}

	// Unselect all notes
	BOOL fChanged = FALSE;
	CDirectMusicEventItem* pEvent = pPart->GetFirstNote( dwVariations );
	while( pEvent )
	{
		if( (pEvent->m_fSelected)
		&&  (pEvent->m_dwVariation & dwVariations) )
		{
			fChanged = TRUE;
			pEvent->m_fSelected = FALSE;
		}
		pEvent = pEvent->GetNext();
	}

	// If anything changes, refresh the display and property pages
	if (fChanged)
	{
		m_NotePropData.m_Param.pPart = NULL;
		RefreshPartDisplay( pPart, dwVariations, FALSE, FALSE );
		ComputeSelectedPropNote( pPart, dwVariations );
		RefreshPropertyPage( pPart );
	}
	return S_OK;
}

HRESULT CMIDIMgr::UnselectAllEvents( CDirectMusicPart* pPart, DWORD dwVariations )
{
	// Validate pPart
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return E_INVALIDARG;
	}

	// Unselect all notes
	BOOL fChanged = FALSE;
	CDirectMusicEventItem* pEvent = pPart->GetFirstNote( dwVariations );
	while( pEvent )
	{
		if( (pEvent->m_fSelected) && (pEvent->m_dwVariation & dwVariations) )
		{
			fChanged = TRUE;
			pEvent->m_fSelected = FALSE;
		}
		pEvent = pEvent->GetNext();
	}

	// Unselect all curves
	pEvent = pPart->GetFirstCurve( dwVariations );
	while( pEvent )
	{
		if( (pEvent->m_fSelected) && (pEvent->m_dwVariation & dwVariations) )
		{
			fChanged = TRUE;
			pEvent->m_fSelected = FALSE;
		}
		pEvent = pEvent->GetNext();
	}

	// Unselect all markers
	pEvent = pPart->GetFirstMarker( dwVariations );
	while( pEvent )
	{
		if( pEvent->m_fSelected && MARKER_AND_VARIATION(reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), dwVariations) )
		{
			fChanged = TRUE;
			pEvent->m_fSelected = FALSE;
		}
		pEvent = pEvent->GetNext();
	}

	// If anything changes, refresh the display and property pages
	if (fChanged)
	{
		m_NotePropData.m_Param.pPart = NULL;
		RefreshPartDisplay( pPart, dwVariations, TRUE, TRUE );
		ComputeSelectedPropNote( pPart, dwVariations );
		RefreshPropertyPage( pPart );
	}
	return S_OK;
}

// This will delete all selected events in the given variations and Part
void CMIDIMgr::DeleteSelectedEvents( CDirectMusicPartRef* pPartRef, DWORD dwVariations )
{
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return;
	}

	BOOL	fChanged = FALSE;

	// Delete Notes
	CDirectMusicEventItem* pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	while( !fChanged && pEvent )
	{
		if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
		{
			// Call PreChangePartRef since this is the first change
			fChanged = TRUE;
			PreChangePartRef( pPartRef );
			break;
		}
		pEvent = pEvent->GetNext();
	}
	// Need to look through the list again from the start, since we may be using a different m_pDMPart 
	pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	while( pEvent )
	{
		if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
		{
			pEvent = DeleteEvent( pEvent, pPartRef->m_pDMPart->m_lstNotes, dwVariations );
		}
		else
		{
			pEvent = pEvent->GetNext();
		}
	}

	// Delete Curves
	pEvent = pPartRef->m_pDMPart->GetFirstCurve( dwVariations );
	while( !fChanged && pEvent )
	{
		if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
		{
			// Call PreChangePartRef since this is the first change
			fChanged = TRUE;
			PreChangePartRef( pPartRef );
			break;
		}
		pEvent = pEvent->GetNext();
	}
	// Need to look through the list again from the start, since we may be using a different m_pDMPart 
	pEvent = pPartRef->m_pDMPart->GetFirstCurve( dwVariations );
	while( pEvent )
	{
		if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
		{
			pEvent = DeleteEvent( pEvent, pPartRef->m_pDMPart->m_lstCurves, dwVariations );
		}
		else
		{
			pEvent = pEvent->GetNext();
		}
	}

	// Delete Markers
	pEvent = pPartRef->m_pDMPart->GetFirstMarker( dwVariations );
	while( !fChanged && pEvent )
	{
		if( pEvent->m_fSelected && MARKER_AND_VARIATION(reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), dwVariations) )
		{
			// Call PreChangePartRef since this is the first change
			fChanged = TRUE;
			PreChangePartRef( pPartRef );
			break;
		}
		pEvent = pEvent->GetNext();
	}
	// Need to look through the list again from the start, since we may be using a different m_pDMPart 
	pEvent = pPartRef->m_pDMPart->GetFirstMarker( dwVariations );
	while( pEvent )
	{
		if( pEvent->m_fSelected && MARKER_AND_VARIATION(reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), dwVariations) )
		{
			// Remove the marker from the displayed variations
			// Update the current item to point to the next marker
			pEvent = pPartRef->m_pDMPart->m_lstMarkers.RemoveMarkerFromVariations( reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), dwVariations );
		}
		else
		{
			pEvent = pEvent->GetNext();
		}
	}

	if (fChanged)
	{
		// update the corresponding DirectMusicPart's parameters
		UpdatePartParamsAfterChange( pPartRef->m_pDMPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_UNDO_DELETE_EVENTS );

		// Set pPart to NULL, since no notes should be selected.
		m_NotePropData.m_Param.pPart = NULL;

		// updated m_SelectedPropNote
	    ValidateActiveDMNote( pPartRef->m_pDMPart, dwVariations );
		RefreshPartDisplay( pPartRef->m_pDMPart, dwVariations, TRUE, TRUE );
		ComputeSelectedPropNote( pPartRef->m_pDMPart, dwVariations );
		RefreshPropertyPage( pPartRef->m_pDMPart );
	}
}

// This will delete all selected notes in the given variations and Part
void CMIDIMgr::DeleteSelectedNotes( CDirectMusicPartRef* pPartRef, DWORD dwVariations )
{
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return;
	}

	BOOL	fChanged = FALSE;

	CDirectMusicEventItem* pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	while( !fChanged && pEvent )
	{
		if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
		{
			// Call PreChangePartRef since this is the first change
			fChanged = TRUE;
			PreChangePartRef( pPartRef );
			break;
		}
		pEvent = pEvent->GetNext();
	}
	// Need to look through the list again from the start, since we may be using a different m_pDMPart 
	pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	while( pEvent )
	{
		if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
		{
			pEvent = DeleteEvent( pEvent, pPartRef->m_pDMPart->m_lstNotes, dwVariations );
		}
		else
		{
			pEvent = pEvent->GetNext();
		}
	}
	if (fChanged)
	{
		// update the corresponding DirectMusicPart's parameters
		UpdatePartParamsAfterChange( pPartRef->m_pDMPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_EDIT_DELETE );

		// Set pPart to NULL, since no notes should be selected.
		m_NotePropData.m_Param.pPart = NULL;

		// updated m_SelectedPropNote
	    ValidateActiveDMNote( pPartRef->m_pDMPart, dwVariations );
		RefreshPartDisplay( pPartRef->m_pDMPart, dwVariations, FALSE, FALSE );
		ComputeSelectedPropNote( pPartRef->m_pDMPart, dwVariations );
		RefreshPropertyPage( pPartRef->m_pDMPart );
	}
}

void CMIDIMgr::SelectAllNotes( CPianoRollStrip* pPRS, DWORD dwVariations )
{
	ASSERT( pPRS != NULL );
	if ( pPRS == NULL )
	{
		return;
	}

	ASSERT( pPRS->ValidPartRefPtr() );
	if( !pPRS->ValidPartRefPtr() )
	{
		return;
	}

	BOOL fDisplayChanged = FALSE;
	BOOL fPropPageChanged = FALSE;

	CDirectMusicEventItem* pEvent = pPRS->m_pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	if ( pEvent != NULL)
	{
		for( ; pEvent != NULL; pEvent = pEvent->GetNext() )
		{
			if ( !pEvent->m_fSelected && (pEvent->m_dwVariation & dwVariations) )
			{
				if ( !fDisplayChanged )
				{
					fDisplayChanged = TRUE; // This includes changing the property page.
					//fPropPageChanged = TRUE:
				}
				pEvent->m_fSelected = TRUE;
			}
		}

		if ( pPRS->m_pPartRef->m_pDMPart != m_NotePropData.m_Param.pPart )
		{
			m_NotePropData.m_Param.pPart = pPRS->m_pPartRef->m_pDMPart;
			fPropPageChanged = TRUE;

			m_NotePropData.m_Param.dwVariations = pPRS->m_dwVariations;
		}
	}
	if (fDisplayChanged)
	{
		RefreshPartDisplay( pPRS->m_pPartRef->m_pDMPart, dwVariations, FALSE, FALSE );
		ComputeSelectedPropNote( pPRS->m_pPartRef->m_pDMPart, dwVariations );
		RefreshPropertyPage( pPRS->m_pPartRef->m_pDMPart );
	}
	else if (fPropPageChanged)
	{
		ComputeSelectedPropNote( pPRS->m_pPartRef->m_pDMPart, dwVariations );
		RefreshPropertyPage( pPRS->m_pPartRef->m_pDMPart );
	}
}

void CMIDIMgr::MergeVariations( CDirectMusicPartRef* pPartRef, DWORD dwVariations )
{
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return;
	}

	BOOL fChange = FALSE;

	// Merge Notes
	CDirectMusicEventItem* pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	for( ;  !fChange && pEvent ;  pEvent = pEvent->GetNext() )
	{
		// For all Notes that are in the affected variations,
		// mask out all unaffected variations, and check if any of the affected
		// variations are unselected.
		if( (pEvent->m_dwVariation & dwVariations)
		&& ((pEvent->m_dwVariation & dwVariations)^dwVariations) )
		{
			// Since this is the first change, set fChange and call PreChangePartRef()
			fChange = TRUE;
			PreChangePartRef( pPartRef );
			break;
		}
	}
	// Need to look through the list again from the start, since we may be using a different m_pDMPart 
	pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	for( ;  pEvent ;  pEvent = pEvent->GetNext() )
	{
		// For all Notes that are in the affected variations,
		// mask out all unaffected variations, and check if any of the affected
		// variations are unselected.
		if( (pEvent->m_dwVariation & dwVariations)
		&& ((pEvent->m_dwVariation & dwVariations)^dwVariations) )
		{
			pEvent->m_dwVariation |= dwVariations;
		}
	}

	// Merge Curves
	pEvent = pPartRef->m_pDMPart->GetFirstCurve( dwVariations );
	for( ;  !fChange && pEvent ;  pEvent = pEvent->GetNext() )
	{
		// For all Curves that are in the affected variations,
		// mask out all unaffected variations, and check if any of the affected
		// variations are unselected.
		if( (pEvent->m_dwVariation & dwVariations)
		&& ((pEvent->m_dwVariation & dwVariations)^dwVariations) )
		{
			// Since this is the first change, set fChange and call PreChangePartRef()
			fChange = TRUE;
			PreChangePartRef( pPartRef );
			break;
		}
	}
	// Need to look through the list again from the start, since we may be using a different m_pDMPart 
	pEvent = pPartRef->m_pDMPart->GetFirstCurve( dwVariations );
	for( ;  pEvent ;  pEvent = pEvent->GetNext() )
	{
		// For all Curves that are in the affected variations,
		// mask out all unaffected variations, and check if any of the affected
		// variations are unselected.
		if( (pEvent->m_dwVariation & dwVariations)
		&& ((pEvent->m_dwVariation & dwVariations)^dwVariations) )
		{
			pEvent->m_dwVariation |= dwVariations;
		}
	}

	// Merge Markers
	pEvent = pPartRef->m_pDMPart->GetFirstMarker( dwVariations );
	for( ;  !fChange && pEvent ;  pEvent = pEvent->GetNext() )
	{
		// For all markers that are in the affected variations,
		if( MARKER_AND_VARIATION( reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), dwVariations ) )
		{
			// Get a pointer to the marker
			CDirectMusicStyleMarker *pDMMarker = reinterpret_cast<CDirectMusicStyleMarker*>(pEvent);

			// For each marker type, check if it is displayed in dwVariations
			// If so, check if it is displayed in all of dwVariations
			// If not, we need to merge the variations for this marker
			DWORD dwVarEnterTemp = pDMMarker->m_dwEnterVariation & dwVariations;
			DWORD dwVarEnterChordTemp = pDMMarker->m_dwEnterChordVariation & dwVariations;
			DWORD dwVarExitTemp = pDMMarker->m_dwExitVariation & dwVariations;
			DWORD dwVarExitChordTemp = pDMMarker->m_dwExitChordVariation & dwVariations;
			if( (dwVarEnterTemp && (dwVarEnterTemp != dwVariations))
			||	(dwVarEnterChordTemp && (dwVarEnterChordTemp != dwVariations))
			||	(dwVarExitTemp && (dwVarExitTemp != dwVariations))
			||	(dwVarExitChordTemp && (dwVarExitChordTemp != dwVariations)) )
			{
				// Since this is the first change, set fChange and call PreChangePartRef()
				fChange = TRUE;
				PreChangePartRef( pPartRef );
				break;
			}
		}
	}
	// Need to look through the list again from the start, since we may be using a different m_pDMPart 
	pEvent = pPartRef->m_pDMPart->GetFirstMarker( dwVariations );
	for( ;  pEvent ;  pEvent = pEvent->GetNext() )
	{
		// For all markers that are in the affected variations,
		if( MARKER_AND_VARIATION( reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), dwVariations ) )
		{
			// Get a pointer to the marker
			CDirectMusicStyleMarker *pDMMarker = reinterpret_cast<CDirectMusicStyleMarker*>(pEvent);

			// For each marker type, check if it is displayed in dwVariations
			// If so, check if it is displayed in all of dwVariations
			// If not, we need to merge the variations for this marker
			DWORD dwVarEnterTemp = pDMMarker->m_dwEnterVariation & dwVariations;
			DWORD dwVarEnterChordTemp = pDMMarker->m_dwEnterChordVariation & dwVariations;
			DWORD dwVarExitTemp = pDMMarker->m_dwExitVariation & dwVariations;
			DWORD dwVarExitChordTemp = pDMMarker->m_dwExitChordVariation & dwVariations;
			if( (dwVarEnterTemp && (dwVarEnterTemp != dwVariations))
			||	(dwVarEnterChordTemp && (dwVarEnterChordTemp != dwVariations))
			||	(dwVarExitTemp && (dwVarExitTemp != dwVariations))
			||	(dwVarExitChordTemp && (dwVarExitChordTemp != dwVariations)) )
			{
				// For each marker type, if it is displayed in any of dwVariations
				// merge it so it is displayed in all of dwVariations
				if( dwVarEnterTemp )
				{
					pDMMarker->m_dwEnterVariation |= dwVariations;
				}
				if( dwVarEnterChordTemp )
				{
					pDMMarker->m_dwEnterChordVariation |= dwVariations;
				}
				if( dwVarExitTemp )
				{
					pDMMarker->m_dwExitVariation |= dwVariations;
				}
				if( dwVarExitChordTemp )
				{
					pDMMarker->m_dwExitChordVariation |= dwVariations;
				}
			}
		}
	}

	if( fChange )
	{
		// update the corresponding DirectMusicPart
		UpdatePartParamsAfterChange( pPartRef->m_pDMPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_EDIT_MERGE_VARIATIONS );

		RefreshPartDisplay( pPartRef->m_pDMPart, dwVariations, TRUE, TRUE );
		ComputeSelectedPropNote( pPartRef->m_pDMPart, dwVariations );
		RefreshPropertyPage( pPartRef->m_pDMPart );
	}
}
	
CPianoRollStrip* CMIDIMgr::CreatePianoRollStrip( CDirectMusicPartRef* pDMPartRef )
{
	CPianoRollStrip* pPianoRollStrip = NULL;

	ASSERT( pDMPartRef != NULL );
	ASSERT( pDMPartRef->m_pPattern != NULL );
	ASSERT( pDMPartRef->m_pDMPart != NULL );

	//CDirectMusicPattern* pDMPattern = pDMPartRef->m_pPattern; 
	CDirectMusicPart* pDMPart = pDMPartRef->m_pDMPart;

//	memcpy( &m_PianoRollData.guid, &(pDMPart->m_guidPartID), sizeof(GUID) );

	// Create the Piano Roll strip
	pPianoRollStrip = new CPianoRollStrip( this, pDMPartRef );
	if( pPianoRollStrip )
	{
		// Deliberately commented out.  Otherwise we have a bad
		// cyclical dependency (we have a reference to the strip, and
		// they have a reference back to us.)
		// pPianoRollStrip->AddRef();

		pPianoRollStrip->InitializePianoRollData( &m_PianoRollData );

		// Part fields
		m_PianoRollData.bPlayModeFlags	 = pDMPart->m_bPlayModeFlags;
		m_PianoRollData.ts				 = pDMPart->m_TimeSignature;
		m_PianoRollData.wNbrMeasures	 = pDMPart->m_wNbrMeasures;
		m_PianoRollData.bInvertUpper	 = pDMPart->m_bInvertUpper;
		m_PianoRollData.bInvertLower	 = pDMPart->m_bInvertLower;
		m_PianoRollData.bAutoInvert		 = pDMPart->m_bAutoInvert;
		m_PianoRollData.lSnapValue		 = TS_GRID_TO_CLOCKS( 1, m_PianoRollData.ts);

		// PartRef fields
		m_PianoRollData.dwPChannel		 = pDMPartRef->m_dwPChannel;
		m_PianoRollData.cstrPartRefName	 = pDMPartRef->m_strName;
		WCHAR wstrName[MAX_PATH];
		ASSERT( m_pIPChannelName );
		if(	SUCCEEDED( m_pIPChannelName->GetPChannelName( pDMPartRef->m_dwPChannel, wstrName) ) )
		{
			m_PianoRollData.cstrPChannelName = wstrName;
		}
		m_PianoRollData.bSubChordLevel	 = pDMPartRef->m_bSubChordLevel;
		m_PianoRollData.bVariationLockID = pDMPartRef->m_bVariationLockID;
		m_PianoRollData.bRandomVariation = pDMPartRef->m_bRandomVariation;
		m_PianoRollData.dwExtraBars		 = 0;
		m_PianoRollData.fPickupBar		 = FALSE;

		m_PianoRollData.dwVariations	 = ALL_VARIATIONS;

		// Display the first variation that has notes in it
		CDirectMusicEventItem* pDMEvent = pDMPart->m_lstNotes.GetHead();
		if( pDMEvent )
		{
			// Calculate which variations there are
			DWORD dwVariations = 0;
			while( pDMEvent && !(dwVariations & 0x1) )
			{
				dwVariations |= pDMEvent->m_dwVariation;

				pDMEvent = pDMEvent->GetNext();
			}

			// Now find the first one
			for( int i = 0 ;  i < 32 ;  i++ )
			{
				if( dwVariations & (1 << i) )
				{
					// At least one variation has notes
					m_PianoRollData.dwVariations = 1 << i;
					break;
				}
			}
		}

		pPianoRollStrip->SetData( &m_PianoRollData );

		if (pPianoRollStrip->m_pPropPageMgr != NULL)
		{
			pPianoRollStrip->m_pPropPageMgr->RefreshData();
		}

		// Add this strip to the list of PianoRollStrips we control
		m_pPRSList.AddTail( pPianoRollStrip );
	}

	return pPianoRollStrip;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr IDMUSProdStripMgr

HRESULT STDMETHODCALLTYPE CMIDIMgr::AddNewStrip()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	// We don't care about paVariants anymore. Always pop up a dialog and ask the user which strip
	// they want to create.

	// Have user select a PChannel
	CDialogNewPart newPartDlg;

	newPartDlg.SetTrack( 0 );
	newPartDlg.SetMIDIMgr( this );
	newPartDlg.DoModal();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CMIDIMgr::GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(mtTime);
	UNREFERENCED_PARAMETER(pmtNext);

	ASSERT( pData != NULL );
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( guidType, GUID_StyleNode ) )
	{
		IDMUSProdNode** ppIStyleNode = (IDMUSProdNode **)pData;

		*ppIStyleNode = m_pIStyleNode;
		if( m_pIStyleNode )
		{
			m_pIStyleNode->AddRef();
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_ChordStripChanged ) )
	{
		BOOL* fChordChordStripChanged = (BOOL *)pData;

		*fChordChordStripChanged = m_fChordStripChanged;
		return S_OK;
	}
	
	else if( ::IsEqualGUID( guidType, STYLE_PChannelChange ) )
	{
		BOOL* fPChannelChange = (BOOL *)pData;

		*fPChannelChange = m_fPChannelChange;
		return S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		return GetUndoText( NULL, (BSTR *)pData );
	}

	else if( ::IsEqualGUID( guidType, GUID_Segment_ExtensionLength ) )
	{
		DWORD dwTemp = 0;
		bool fFoundOne = false;
		POSITION position = m_pPRSList.GetHeadPosition();
		while( position != NULL )
		{
			CPianoRollStrip* pPianoRollStrip = m_pPRSList.GetNext(position);
			pPianoRollStrip->m_dwExtraBars = pPianoRollStrip->GetNumExtraBars();
			if( fFoundOne )
			{
				if( dwTemp != pPianoRollStrip->m_dwExtraBars )
				{
					dwTemp = 0xFFFFFFFF;
					break;
				}
			}
			else
			{
				dwTemp = pPianoRollStrip->m_dwExtraBars;
				fFoundOne = true;
			}
		}

		DWORD *pdwData = (DWORD *)pData;
		*pdwData = dwTemp;
		return S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_Segment_PickupLength ) )
	{
		BOOL fTemp = FALSE;
		bool fFoundOne = false;
		POSITION position = m_pPRSList.GetHeadPosition();
		while( position != NULL )
		{
			CPianoRollStrip* pPianoRollStrip = m_pPRSList.GetNext(position);
			pPianoRollStrip->m_fPickupBar = pPianoRollStrip->ShouldDisplayPickupBar();
			if( fFoundOne )
			{
				if( fTemp != pPianoRollStrip->m_fPickupBar )
				{
					fTemp = 0xFFFFFFFF;
					break;
				}
			}
			else
			{
				fTemp = pPianoRollStrip->m_fPickupBar;
				fFoundOne = true;
			}
		}

		DWORD *pdwData = (DWORD *)pData;
		*pdwData = fTemp;
		return S_OK;
	}


	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CMIDIMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(mtTime);

	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		m_pDMProdSegmentNode = reinterpret_cast<IDMUSProdNode*>(pData);
		// No addref!
		//m_pDMProdSegmentNode->AddRef();
		return S_OK;
	}

	if( ::IsEqualGUID( guidType, GUID_StyleNode ) )
	{
		IDMUSProdNode* pIStyleNode = (IDMUSProdNode *)pData;

		if( m_pIStyleNode )
		{
			m_pIStyleNode->Release();
		}
		m_pIStyleNode = pIStyleNode;
		if( m_pIStyleNode )
		{
			m_pIStyleNode->AddRef();
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( guidType, STYLE_PChannelChange ) )
	{
		BOOL* fPChannelChange = (BOOL *)pData;

		m_fPChannelChange = *fPChannelChange;
		return S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		if( m_pIFramework && pData && (m_pIPChannelName == NULL) )
		{
			IDMUSProdNode* pINode;
			if( SUCCEEDED( static_cast<IUnknown *>(pData)->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
			{
				IDMUSProdProject* pIProject;
				if( SUCCEEDED ( m_pIFramework->FindProject( pINode, &pIProject ) ) )
				{
					pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&m_pIPChannelName );
					pIProject->Release();
				}
				pINode->Release();
			}
		}
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CMIDIMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	// support GUID_StyleNode for maintaining pointer to Style's DocRoot node
	if( ::IsEqualGUID( guidType, GUID_StyleNode )
	||	::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR )
	||	::IsEqualGUID( guidType, GUID_DocRootNode )
	||  ::IsEqualGUID( guidType, STYLE_PChannelChange ) )
	{
		return S_OK;
	}

	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CMIDIMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwGroupBits);

	// Chord track chord or key change
	if( ::IsEqualGUID( rguidType, GUID_ChordParam )
	||	::IsEqualGUID( rguidType, GUID_ChordKey )
	||	::IsEqualGUID( rguidType, CLSID_DirectMusicChordTrack ) )
	{
		if( ::IsEqualGUID( rguidType, GUID_ChordKey ) )
		{
			UpdateFlatsAndKey();
		}

		// Make sure there is a Chord on the first beat of the first measure
		DMUS_CHORD_PARAM chordData;
		BOOL fFlatsNotSharps = FALSE;
		if( GetChordFromChordStrip( 0, &chordData, &fFlatsNotSharps ) != S_OK )
		{
			if( !m_pIDMTrack )
			{
				HWND hwndHadFocus = ::GetFocus();
				InsertDefaultChord();
				AfxMessageBox( IDS_ERR_DELETE_FIRST_CHORD );
				if( hwndHadFocus )
				{
					::SetFocus( hwndHadFocus );
				}
			}
		}
		else
		{
			ASSERT( chordData.wMeasure == 0 );
			ASSERT( chordData.bBeat == 0 );

			if( chordData.wMeasure == 0
			&&  chordData.bBeat == 0 )
			{
				m_chordDefault = chordData;
				m_fDefaultChordFlatsNotSharps = fFlatsNotSharps;
			}
		}

		// Get the ChordStrip mode
		const BYTE bOldMode = m_bTempPatternEditorMode;
		BYTE bMode;
		if( FAILED ( m_pTimeline->GetParam( GUID_PatternEditorMode, m_pIDMTrack ? m_dwGroupBits : m_pChordTrack->m_dwGroupBits,
											0, 0, NULL, &bMode ) ) )
		{
			bMode = CHORDSTRIP_MODE_MIDI_CONSTANT;
		}
		else if( m_pIDMTrack )
		{
			UpdateChordTrackEditorMode();
			if( bMode != 0xFF )
			{
				m_bTempPatternEditorMode = bMode;
			}
		}

		if( m_pDMPattern )
		{
			if( bMode == CHORDSTRIP_MODE_FUNCTION_CONSTANT )
			{
				// Recalc the MIDI values for the entire Pattern
				m_pDMPattern->RecalcMIDIValues();
			}
			else
			{
				// Recalc the musicvalues for the entire Pattern
				m_pDMPattern->RecalcMusicValues();
			}
		}

		// Refresh all our strips
		InvalidateAllStrips();

		// Always recompute m_SelectedPropNote and update the property page
		if( m_NotePropData.m_Param.pPart )
		{
			ComputeSelectedPropNote( m_NotePropData.m_Param.pPart,
									 m_NotePropData.m_Param.dwVariations );
			RefreshPropertyPage( m_NotePropData.m_Param.pPart );
		}

		// Let object know about the change

		BOOL fResetFreeze = FALSE;
		VARIANT var;
		m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var );
		if( V_BOOL(&var) == TRUE
		&&	(!m_pIDMTrack
			 || bOldMode != m_bTempPatternEditorMode) )
		{
			fResetFreeze = TRUE;

			// Need to clear TP_FREEZE_UNDO or the editor won't add an undo state for us
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
		}

		// Double-check to make sure the PatternEditorMode is set
		UpdateChordTrackEditorMode();

		m_fChordStripChanged = TRUE;
		UpdateOnDataChanged( m_pIDMTrack ? IDS_UNDO_CHORDSTRIP_FUNCTIONALITY : IDS_UNDO_CHORDSTRIP_CHANGE );
		m_fChordStripChanged = FALSE;
		m_fDirty = TRUE;

		if( fResetFreeze )
		{
			// Need to set TP_FREEZE_UNDO
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_ChordSharpsFlats ) )
	{
		UpdateFlatsAndKey();

		if( !m_pIDMTrack )
		{
			// Let object know about the change

			// Need to reset TP_FREEZE_UNDO or the editor won't add an undo state for us
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );

			m_fChordStripChanged = TRUE;
			UpdateOnDataChanged( IDS_UNDO_CHORDSTRIP_CHANGE );
			m_fChordStripChanged = FALSE;
			m_fDirty = TRUE;
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, STYLE_NotationTypeChange ) )
	{
		if( pData )
		{
			ChangeNotationType( *static_cast<DWORD *>(pData) );
			return S_OK;
		}
		else
		{
			return E_POINTER;
		}
	}

	else if( ::IsEqualGUID( rguidType, STYLE_PatternWindowClose ) )
	{
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		ASSERT( m_pIDMTrack );
		UpdateFlatsAndKey();

		// Add Curve Strips to the Timeline
		m_pDMPattern->RecalcMIDIValues();
		//ASSERT(FALSE);
		// TODO: Implement
		//m_pSequenceStrip->AddCurveStrips();

		// Double-check to make sure the PatternEditorMode is set
		UpdateChordTrackEditorMode();
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_BandTrackChange ) )
	{
		// Refresh all our strips
		POSITION pos = m_pPRSList.GetHeadPosition();
		while( pos )
		{
			m_pPRSList.GetNext( pos )->InvalidateFunctionBar();
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_RecordButton ) )
	{
		if( pData == NULL )
		{
			return E_POINTER;
		}

		OnRecord( *(BOOL *)pData );
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_WindowActive ) )
	{
		if( pData == NULL )
		{
			return E_POINTER;
		}

		Activate( *(BOOL *)pData );
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_Start ) )
	{
		if( pData == NULL )
		{
			return E_POINTER;
		}

		SetSegmentState( (IUnknown *) pData );
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_Stop ) )
	{
		SetSegmentState( NULL );

		// If PianoRoll strip is active
		if( m_pActivePianoRollStrip && (m_iFocus == 1) )
		{
			// If there currently is no primary segment playing, try and play the patch for this PChannel
			REFERENCE_TIME rtLatency;
			m_pIDMPerformance->GetLatencyTime( &rtLatency );
			MUSIC_TIME mtLatency;
			m_pIDMPerformance->ReferenceToMusicTime( rtLatency, &mtLatency );

			IDirectMusicSegmentState *pSegState;
			if( FAILED( m_pIDMPerformance->GetSegmentState( &pSegState, mtLatency ) ) )
			{
				long lTime;
				if (SUCCEEDED(m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
				{
					PlayPatch( lTime, m_pActivePianoRollStrip->m_pPartRef->m_dwPChannel );
				}
			}
			else
			{
				pSegState->Release();
			}
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_CreateTrack ) )
	{
		// Double-check to make sure the PatternEditorMode is set
		UpdateChordTrackEditorMode();

		// If a pattern doesn't exist, create one
		CreateDefaultPattern();

		// Initialize our DirectMusic track
		UpdateDirectMusicTrack();

		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_DeletedTrack ) )
	{
		// Check if there are any other MIDIStripMgrs in m_dwGroupBits
		IDMUSProdTimeline *pTimeline = (IDMUSProdTimeline *)pData;
		IDMUSProdStripMgr *pStripMgr;
		if( SUCCEEDED( pTimeline->GetStripMgr( CLSID_DirectMusicPatternTrack, m_dwGroupBits, 0, &pStripMgr ) ) )
		{
			pStripMgr->Release();
		}
		else
		{
			// No other MIDIStripMgrs, reset the CHORDSTRIP_MODE to nothing
			BYTE bPatternEditorMode = 0xFF;
			pTimeline->SetParam( GUID_PatternEditorMode, m_dwGroupBits, 0, 0, &bPatternEditorMode );
		}
		return S_OK;
	}

	else if( m_pTimeline && ::IsEqualGUID( rguidType, GUID_Segment_ExtensionLength ) )
	{
		POSITION position = m_pPRSList.GetHeadPosition();
		while( position != NULL )
		{
			CPianoRollStrip* pPianoRollStrip = m_pPRSList.GetNext(position);
			DWORD dwTemp = pPianoRollStrip->m_dwExtraBars;
			pPianoRollStrip->m_dwExtraBars = *(DWORD *)pData;
			pPianoRollStrip->m_dwExtraBars = pPianoRollStrip->GetNumExtraBars();
			if( pPianoRollStrip->m_dwExtraBars != dwTemp )
			{
				m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)pPianoRollStrip, NULL, TRUE );
			}
		}

		return S_OK;
	}

	else if( m_pTimeline && ::IsEqualGUID( rguidType, GUID_Segment_PickupLength ) )
	{
		POSITION position = m_pPRSList.GetHeadPosition();
		while( position != NULL )
		{
			CPianoRollStrip* pPianoRollStrip = m_pPRSList.GetNext(position);
			BOOL fTemp = pPianoRollStrip->m_fPickupBar;
			pPianoRollStrip->m_fPickupBar = *(DWORD *)pData;
			pPianoRollStrip->m_fPickupBar = pPianoRollStrip->ShouldDisplayPickupBar();
			if( pPianoRollStrip->m_fPickupBar != fTemp )
			{
				m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)pPianoRollStrip, NULL, TRUE );
			}
		}

		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_TimelineSetCursor ) )
	{
		if( m_pTimeline )
		{
			POSITION pos = m_pPRSList.GetHeadPosition();
			while( pos )
			{
				// Update displayed instrument name, if necessary
				m_pPRSList.GetNext( pos )->UpdateInstrumentName();
			}
		}
	}

	else if( ::IsEqualGUID( rguidType, GUID_TimelineSetSnapTo )
		 ||  ::IsEqualGUID( rguidType, GUID_TimelineUserSetCursor ) )
	{
		if( m_pActivePianoRollStrip )
		{
			long lInsertTime = m_pActivePianoRollStrip->m_lInsertTime;
			m_pActivePianoRollStrip->UpdateNoteCursorTime();
			if( (lInsertTime == m_pActivePianoRollStrip->m_lInsertTime)
			&&	::IsEqualGUID( rguidType, GUID_TimelineSetSnapTo ) )
			{
				m_pActivePianoRollStrip->InvalidatePianoRoll();
			}

			if( ::IsEqualGUID( rguidType, GUID_TimelineUserSetCursor ) )
			{
				// If PianoRoll strip strip is active, and we're in the segment designer
				if( m_pIDMTrack
				&&	(m_iFocus == 1) )
				{
					// If there currently is no primary segment playing, try and play the patch for this PChannel
					REFERENCE_TIME rtLatency;
					m_pIDMPerformance->GetLatencyTime( &rtLatency );
					MUSIC_TIME mtLatency;
					m_pIDMPerformance->ReferenceToMusicTime( rtLatency, &mtLatency );

					IDirectMusicSegmentState *pSegState;
					if( FAILED( m_pIDMPerformance->GetSegmentState( &pSegState, mtLatency ) ) )
					{
						long lTime;
						if (SUCCEEDED(m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
						{
							PlayPatch( lTime, m_pActivePianoRollStrip->m_pPartRef->m_dwPChannel );
						}
					}
					else
					{
						pSegState->Release();
					}
				}

				// Update displayed instrument name, if necessary
				POSITION pos = m_pPRSList.GetHeadPosition();
				while( pos )
				{
					// Update displayed instrument name, if necessary
					m_pPRSList.GetNext( pos )->UpdateInstrumentName();
				}
			}
		}
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_WindowClosing ) )
	{
		if( m_fDirty )
		{
			UpdateOnDataChanged( 0 );
		}
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_Length_Change ) )
	{
		// Check if the time signatures match
		if( m_pTimeline && pData )
		{
			DMUS_TIMESIGNATURE dmTimeSig;
			MUSIC_TIME mtNext = 0;
			if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, 0, &mtNext, &dmTimeSig ) ) )
			{
				if( m_pDMPattern->m_TimeSignature.m_bBeat == dmTimeSig.bBeat
				&&	m_pDMPattern->m_TimeSignature.m_bBeatsPerMeasure == dmTimeSig.bBeatsPerMeasure )
				{
					// Time signatures match, now check if the old length matches
					if( m_pDMPattern->CalcLength() == *(MUSIC_TIME *)pData )
					{
						// Old length matches, get the new length
						VARIANT varLength;
						if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
						{
							// Compute the new number of measures
							const long lClocksPerMeasure = long(dmTimeSig.bBeatsPerMeasure) * (DM_PPQNx4 / long(dmTimeSig.bBeat));
							long lNumMeasures = (V_I4(&varLength) + (lClocksPerMeasure - 1)) / lClocksPerMeasure;

							// Only update if the new length is an even number of measures
							if( lNumMeasures * lClocksPerMeasure == V_I4(&varLength) )
							{
								m_pDMPattern->SetNbrMeasures( WORD( min( USHRT_MAX, max( 1, lNumMeasures ) ) ) );
								return S_OK;
							}
						}
					}
				}
			}
		}
	}

	return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE CMIDIMgr::GetStripMgrProperty(
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [out] */ VARIANT*	pVariant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pVariant );
	if( !pVariant )
	{
		return E_POINTER;
	}

	switch( stripMgrProperty )
	{
	case SMP_ITIMELINECTL:
		pVariant->vt = VT_UNKNOWN;
		if( m_pTimeline )
		{
			V_UNKNOWN( pVariant ) = m_pTimeline;
			V_UNKNOWN( pVariant )->AddRef();
			return S_OK;
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

	case SMP_IDIRECTMUSICTRACK:
		pVariant->vt = VT_UNKNOWN;
		if( m_pIDMTrack )
		{
			return m_pIDMTrack->QueryInterface( IID_IUnknown, (void**)&V_UNKNOWN( pVariant ) );
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

	case SMP_IDMUSPRODFRAMEWORK:
		pVariant->vt = VT_UNKNOWN;
		if( m_pIFramework )
		{
			return m_pIFramework->QueryInterface( IID_IUnknown, (void**)&V_UNKNOWN( pVariant ) );
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

	case SMP_DMUSIOTRACKHEADER:
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( pVariant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}

			pioTrackHeader->guidClassID = CLSID_DirectMusicPatternTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = NULL;
			pioTrackHeader->fccType = DMUS_FOURCC_PATTERN_FORM;
		}
		break;

	case SMP_DMUSIOTRACKEXTRASHEADER:
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_EXTRAS_HEADER *pioTrackExtrasHeader = static_cast<DMUS_IO_TRACK_EXTRAS_HEADER *>(V_BYREF( pVariant ));
			if( pioTrackExtrasHeader == NULL )
			{
				return E_POINTER;
			}

			pioTrackExtrasHeader->dwFlags = m_dwTrackExtrasFlags;
		}
		break;

	case SMP_DMUSIOTRACKEXTRASHEADER_MASK:
		pVariant->vt = VT_I4;
		V_I4(pVariant) = TRACKCONFIG_VALID_MASK;
		break;

	case SMP_PRODUCERONLY_FLAGS:
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			IOProducerOnlyChunk *pioProducerOnlyChunk = static_cast<IOProducerOnlyChunk *>(V_BYREF( pVariant ));
			if( pioProducerOnlyChunk == NULL )
			{
				return E_POINTER;
			}

			pioProducerOnlyChunk->dwProducerOnlyFlags = m_dwProducerOnlyFlags;
		}
		break;

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CMIDIMgr::SetStripMgrProperty(
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [in] */ VARIANT		variant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	switch( stripMgrProperty )
	{
	case SMP_ITIMELINECTL:
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}
		if( m_pTimeline )
		{
			// if the timeline is going away, we need to release any propertypage object
			// we set on it previously
			if( V_UNKNOWN( &variant ) == NULL )
			{
				m_fShuttingDown = TRUE;
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);

				// Remove all strips from the Timeline
				while( !m_pPRSList.IsEmpty() )
				{
					CPianoRollStrip* pPRS;
					pPRS = m_pPRSList.RemoveHead();
					m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)pPRS);
					m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pPRS );
					KillFocus( pPRS );
					pPRS->Release();
				}

				if( m_pChordTrack )
				{
					m_pTimeline->RemoveStripMgr( m_pChordTrack->m_pIStripMgr );
					delete m_pChordTrack;
					m_pChordTrack = NULL;
				}
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Length_Change, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, CLSID_DirectMusicChordTrack, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_ExtensionLength, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_PickupLength, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_BandTrackChange, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_RecordButton, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_WindowActive, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordKey, m_pIDMTrack ? m_dwOldGroupBits : 0xFFFFFFFF );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordSharpsFlats, m_pIDMTrack ? m_dwOldGroupBits : 0xFFFFFFFF );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordParam, m_pIDMTrack ? m_dwOldGroupBits : 0xFFFFFFFF );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineUserSetCursor, m_pIDMTrack ? m_dwOldGroupBits : 0xFFFFFFFF );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetSnapTo, m_pIDMTrack ? m_dwOldGroupBits : 0xFFFFFFFF );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_WindowClosing, m_pIDMTrack ? m_dwOldGroupBits : 0xFFFFFFFF );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetCursor, m_pIDMTrack ? m_dwOldGroupBits : 0xFFFFFFFF );
			m_pTimeline->Release();
			m_pTimeline = NULL;

			// This will return a failure code if we haven't registered.
			if( m_pIConductor )
			{
				m_pIConductor->UnregisterNotify( (IDMUSProdNotifyCPt*)this, GUID_NOTIFICATION_SEGMENT );
				m_pIConductor->UnregisterNotify( (IDMUSProdNotifyCPt*)this, GUID_NOTIFICATION_RECOMPOSE );
			}

			if( m_pIPChannelName )
			{
				m_pIPChannelName->Release();
				m_pIPChannelName = NULL;
			}
		}

		if( V_UNKNOWN( &variant ) )
		{
			m_fShuttingDown = FALSE;

			if( SUCCEEDED ( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pTimeline ) ) )
			{
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetCursor, m_pIDMTrack ? m_dwOldGroupBits : 0xFFFFFFFF );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_WindowClosing, m_pIDMTrack ? m_dwOldGroupBits : 0xFFFFFFFF );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetSnapTo, m_pIDMTrack ? m_dwGroupBits : 0xFFFFFFFF );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineUserSetCursor, m_pIDMTrack ? m_dwGroupBits : 0xFFFFFFFF );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordParam, m_pIDMTrack ? m_dwGroupBits : 0xFFFFFFFF );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordSharpsFlats, m_pIDMTrack ? m_dwGroupBits : 0xFFFFFFFF );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordKey, m_pIDMTrack ? m_dwGroupBits : 0xFFFFFFFF );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_WindowActive, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_RecordButton, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_BandTrackChange, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_PickupLength, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_ExtensionLength, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, CLSID_DirectMusicChordTrack, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Length_Change, m_dwGroupBits );
			}

			// If m_pDMPattern doesn't exist, create a default pattern
			CreateDefaultPattern();

			if( m_pIFramework == NULL )
			{
				VARIANT var;
				if (SUCCEEDED(m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var )))
				{
					V_UNKNOWN(&var)->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pIFramework );
					V_UNKNOWN(&var)->Release();
				}
			}

			if( (m_pIConductor == NULL)
			&&	(m_pIFramework != NULL) )
			{
				IDMUSProdComponent* pIComponent = NULL;
				if( SUCCEEDED ( m_pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
				{
					pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor );
					pIComponent->Release();
				}
			}

			if( (m_pIDMPerformance == NULL)
			&&	(m_pIConductor != NULL) )
			{
				IUnknown* punk;
				if( SUCCEEDED( m_pIConductor->GetPerformanceEngine( &punk ) ) )
				{
					punk->QueryInterface( IID_IDirectMusicPerformance8, (void **)&m_pIDMPerformance );
					punk->Release();
				}

				ASSERT( m_pIDMPerformance );
			}

			VARIANT var;
			if ( (m_pIPChannelName == NULL)
			&&	 SUCCEEDED(m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &var )) )
			{
				IDMUSProdNode* pINode;
				if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
				{
					IDMUSProdProject* pIProject;
					if( SUCCEEDED ( m_pIFramework->FindProject( pINode, &pIProject ) ) )
					{
						pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&m_pIPChannelName );
						pIProject->Release();
					}
					pINode->Release();
				}
				V_UNKNOWN(&var)->Release();
			}

			if( m_pIDMTrack )
			{
				AddPianoRollStripsToTimeline();
			}
		}
		break;

	case SMP_IDIRECTMUSICTRACK:
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}
		if( m_pIDMTrack )
		{
			m_pIDMTrack->Release();
			m_pIDMTrack = NULL;
		}
		if( V_UNKNOWN( &variant ) )
		{
			V_UNKNOWN( &variant )->QueryInterface( IID_IDirectMusicTrack, (void**)&m_pIDMTrack );
		}
		else
		{
			m_pIDMTrack = NULL;
		}
		break;

	case SMP_IDMUSPRODFRAMEWORK:
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}
		if( m_pIFramework )
		{
			m_pIFramework->Release();
			m_pIFramework = NULL;
		}
		if( V_UNKNOWN( &variant ) )
		{
			if( SUCCEEDED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pIFramework) ) )
			{
				if( m_pIConductor == NULL )
				{
					IDMUSProdComponent* pIComponent = NULL;
					if( SUCCEEDED ( m_pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
					{
						pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor );
						pIComponent->Release();
					}
				}

				if( m_pIConductor
				&&	(m_pIDMPerformance != NULL) )
				{
					IUnknown* punk;
					if( SUCCEEDED( m_pIConductor->GetPerformanceEngine( &punk ) ) )
					{
						punk->QueryInterface( IID_IDirectMusicPerformance8, (void **)&m_pIDMPerformance );
						punk->Release();
					}

					ASSERT( m_pIDMPerformance );
				}
			}
		}
		break;

	case SMP_DMUSIOTRACKHEADER:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( &variant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}
			m_dwGroupBits = pioTrackHeader->dwGroup;
			m_dwOldGroupBits = pioTrackHeader->dwGroup;
		}
		break;

	case SMP_DMUSIOTRACKEXTRASHEADER:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_EXTRAS_HEADER *pioTrackExtrasHeader = static_cast<DMUS_IO_TRACK_EXTRAS_HEADER *>(V_BYREF( &variant ));
			if( pioTrackExtrasHeader == NULL )
			{
				return E_POINTER;
			}

			m_dwTrackExtrasFlags = TRACKCONFIG_VALID_MASK & (pioTrackExtrasHeader->dwFlags);
		}
		break;

	case SMP_PRODUCERONLY_FLAGS:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			IOProducerOnlyChunk *pioProducerOnlyChunk = static_cast<IOProducerOnlyChunk *>(V_BYREF( &variant ));
			if( pioProducerOnlyChunk == NULL )
			{
				return E_POINTER;
			}

			m_dwProducerOnlyFlags = pioProducerOnlyChunk->dwProducerOnlyFlags;
		}
		break;

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr IMIDIMgr

HRESULT CMIDIMgr::GetUndoText( /*[out]*/ BOOL* pfUpdateDirectMusic,
							   /*[out,retval]*/ BSTR* pbstrUndoText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pbstrUndoText == NULL )
	{
		return E_POINTER;
	}

	if( pfUpdateDirectMusic )
	{
		*pfUpdateDirectMusic = m_fUpdateDirectMusic;
		m_fUpdateDirectMusic = TRUE;
	}

	*pbstrUndoText = NULL;
	if( m_nUndoString == 0
	&&	m_strUndoString.IsEmpty() )
	{
		return E_FAIL;
	}

	// Try and load our current undo string
	if( m_nUndoString != 0 )
	{
		m_strUndoString.Empty();
		CComBSTR comBSTR;
		if( comBSTR.LoadString( m_nUndoString ) )
		{
			// Succeeded - return the BSTR
			*pbstrUndoText = comBSTR.Detach();
			return S_OK;
		}
	}
	else
	{
		*pbstrUndoText = m_strUndoString.AllocSysString();
		m_strUndoString.Empty();
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CMIDIMgr::OnRecord( /*[in]*/ BOOL fEnableRecord )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if ( m_fRecordEnabled != fEnableRecord )
	{
		m_fRecordEnabled = fEnableRecord;

		if( m_fWindowActive && m_pSegmentState && m_pActivePianoRollStrip )
		{
			if( m_fRecordEnabled )
			{
				// Enable thruing
				m_pActivePianoRollStrip->SetPChannelThru();
			}
			else
			{
				//Cancel thru
				if( m_pActivePianoRollStrip->m_fMIDIThruEnabled )
				{
					m_pActivePianoRollStrip->CancelPChannelThru();
				}
			}
		}
		//TRACE("CMIDIMgr::OnRecord %d\n", m_fRecordEnabled);
	}
	return S_OK;
}

HRESULT CMIDIMgr::MergeVariations( /*[in]*/ BOOL fChangeData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	IDMUSProdStrip* pIActiveStrip;
	VARIANT var;

	// Get the active Strip
	if( SUCCEEDED ( m_pTimeline->GetTimelineProperty( TP_ACTIVESTRIP, &var ) ) )
	{
		pIActiveStrip = (IDMUSProdStrip *)V_UNKNOWN( &var );

		CPianoRollStrip* pPianoRollStrip = NULL;

		// Get the active PianoRollStrip
		if( pIActiveStrip )
		{
			CLSID clsid;
			var.vt = VT_BYREF;
			V_BYREF(&var) = &clsid;

			if( SUCCEEDED ( pIActiveStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &var ) ) )
			{
				if( ::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
				{
					pPianoRollStrip = (CPianoRollStrip *)pIActiveStrip;
				}
				else if( ::IsEqualCLSID( clsid, CLSID_CurveStrip ) )
				{
					CCurveStrip* pCurveStrip = (CCurveStrip *)pIActiveStrip;

					pPianoRollStrip = pCurveStrip->m_pPianoRollStrip;
				}
				else if( ::IsEqualCLSID( clsid, CLSID_VarSwitchStrip ) )
				{
					CVarSwitchStrip* pVarSwitchStrip = (CVarSwitchStrip *)pIActiveStrip;

					pPianoRollStrip = pVarSwitchStrip->m_pPianoRollStrip;
				}
			}

			pIActiveStrip->Release();
			pIActiveStrip = NULL;
		}

		if( pPianoRollStrip
		&&	pPianoRollStrip->m_pPartRef
		&&	pPianoRollStrip->m_pPartRef->m_pDMPart )
		{
			if( fChangeData )
			{
				MergeVariations( pPianoRollStrip->m_pPartRef,
								 pPianoRollStrip->m_dwVariations );
			}

			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CMIDIMgr::SetSegmentState( /*[in]*/ IUnknown* punkSegmentState )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( punkSegmentState == m_pSegmentState )
	{
		return S_FALSE;
	}

	if( m_pSegmentState )
	{
		m_pIConductor->UnregisterNotify( (IDMUSProdNotifyCPt*)this, GUID_NOTIFICATION_SEGMENT );
		m_pIConductor->UnregisterNotify( (IDMUSProdNotifyCPt*)this, GUID_NOTIFICATION_RECOMPOSE );
		m_pSegmentState->Release();
		m_pSegmentState = NULL;
	}

	if( punkSegmentState )
	{
		punkSegmentState->QueryInterface( IID_IDirectMusicSegmentState, (void **) &m_pSegmentState );

		m_pIConductor->RegisterNotify( (IDMUSProdNotifyCPt*)this, GUID_NOTIFICATION_SEGMENT );
		m_pIConductor->RegisterNotify( (IDMUSProdNotifyCPt*)this, GUID_NOTIFICATION_RECOMPOSE );
		MUSIC_TIME mtSegStartTime;
		if( SUCCEEDED( m_pSegmentState->GetStartTime( &mtSegStartTime ) ) )
		{
			MUSIC_TIME mtSegStartPoint;
			if( SUCCEEDED( m_pSegmentState->GetStartPoint( &mtSegStartPoint ) ) )
			{
				if( m_pIDMTrack )
				{
					m_pSegmentState->GetRepeats( &m_dwCurrentMaxLoopRepeats );

					IDirectMusicSegment* pSegment;
					if( SUCCEEDED( m_pSegmentState->GetSegment( &pSegment ) ) )
					{
						pSegment->GetLoopPoints( &m_mtCurrentLoopStart, &m_mtCurrentLoopEnd );
						pSegment->GetLength( &m_mtCurrentLength );
						pSegment->Release();
					}
				}

				//TRACE("SegmentState StartTime, Point: %d, %d.\n", mtSegStartTime, mtSegStartPoint);
				MUSIC_TIME mtPatternLength = m_pDMPattern->CalcLength();
				m_pIDMPerformance->GetTime( NULL, &m_mtLastLoop );
				// LastLoop = max( time of start, now - (elapsed play time % pattern length) )
				m_mtLastLoop = max( mtSegStartTime - mtSegStartPoint,
					m_mtLastLoop - (max(0, m_mtLastLoop - (mtSegStartTime - mtSegStartPoint)) % mtPatternLength) );

				// Play the correct audition variations
				UpdateAuditionVariations();

				// Clear out all piano roll strips
				POSITION position = m_pPRSList.GetHeadPosition();
				while (position != NULL)
				{
					CPianoRollStrip *pPRS = m_pPRSList.GetNext(position);
					ZeroMemory(pPRS->m_mtStartTimeOffset, sizeof(MUSIC_TIME) * 128);
					ZeroMemory(pPRS->m_mtStartTime, sizeof(MUSIC_TIME) * 128);
					ZeroMemory(pPRS->m_bVelocity, sizeof(BYTE) * 128);

					// Enable the variation timer.  This is a no-op if there is no
					// Timeline window
					pPRS->EnableVariationTimer();
				}
			}
		}
	}
	else
	{
		m_mtCurrentLoopStart = 0;
		m_mtCurrentLoopEnd = 0;
		m_dwCurrentMaxLoopRepeats = 0;

		if( m_fNoteInserted )
		{
			if( m_pIDMTrack )
			{
				// Need to reset TP_FREEZE_UNDO or the segment won't add an undo state for us
				VARIANT var;
				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
			}
			CoInitialize(NULL);
			UpdateOnDataChanged( IDS_EDIT_INSERT );
			CoUninitialize();
			// Handled by UpdateOnDataChanged
			//m_fNoteInserted = FALSE;
		}

		// Update all the note cursors
		POSITION position = m_pPRSList.GetHeadPosition();
		while (position != NULL)
		{
			m_pPRSList.GetNext(position)->UpdateNoteCursorTime();
		}
	}

	if( m_fWindowActive && m_fRecordEnabled && m_pActivePianoRollStrip )
	{
		if( m_pSegmentState )
		{
			// Enable thruing
			m_pActivePianoRollStrip->SetPChannelThru();
		}
		else
		{
			//Cancel thru
			if( m_pActivePianoRollStrip->m_fMIDIThruEnabled )
			{
				m_pActivePianoRollStrip->CancelPChannelThru();
			}
		}
	}

	return S_OK;
}

HRESULT CMIDIMgr::CanDeleteTrack()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	DWORD dwEnum = 1;
	IDMUSProdStrip* pIStrip;
	VARIANT variant;

	// Try and find gutter-selected strips
	while( m_pTimeline->EnumStrip( dwEnum, &pIStrip ) == S_OK )
	{
		ASSERT( pIStrip );
		dwEnum++;
		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pIStrip, STP_GUTTER_SELECTED, &variant ) ) )
		{
			// If the strip's gutter is selected
			if( V_BOOL( &variant ) == TRUE )
			{
				// Release our pointer to the strip
				pIStrip->Release();

				// Return S_OK
				return S_OK;
			}
		}

		pIStrip->Release();
	}

	// No gutter selected strip, can only delete if there is an active strip,
	// and the PianoRoll strip has focus (not a curve strip)
	return (m_pActivePianoRollStrip && (m_iFocus == 1)) ? S_OK : S_FALSE;
}

HRESULT CMIDIMgr::DeleteTrack()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return OnDeletePart( m_pActivePianoRollStrip );
}

HRESULT CMIDIMgr::Activate( /*[in]*/ BOOL fActive )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if ( m_fWindowActive != fActive )
	{
		m_fWindowActive = fActive;
		//TRACE("CMIDIMgr::Activate %d\n", fActive);
		if ( m_fWindowActive )
		{
			// enable input for the currently active strip
			if ( m_pActivePianoRollStrip )
			{
				m_pActivePianoRollStrip->RegisterMidi();

				if( m_fRecordEnabled && m_pSegmentState )
				{
					// Enable thruing
					m_pActivePianoRollStrip->SetPChannelThru();
				}
			}
		}
		else
		{
			// disable input for the currently active strip
			if ( m_pActivePianoRollStrip )
			{
				m_pActivePianoRollStrip->UnRegisterMidi();

				// Fake a mouse-button up, since we lose the cursor
				m_pActivePianoRollStrip->OnLButtonUp( 0, 0 );
			}
		}

		// Stop any playing note, if the window is deactivated
		if( !m_fWindowActive )
		{
			StopNote();
		}
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr IDMUSProdNotifyCPt

HRESULT CMIDIMgr::OnNotify( ConductorNotifyEvent *pConductorNotifyEvent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch( pConductorNotifyEvent->m_dwType )
	{
		case DMUS_PMSGT_NOTIFICATION:
		{
			DMUS_NOTIFICATION_PMSG* pNotifyEvent = (DMUS_NOTIFICATION_PMSG *)pConductorNotifyEvent->m_pbData;

			// Handle GUID_NOTIFICATION_SEGMENT notifications
			if( ::IsEqualGUID ( pNotifyEvent->guidNotificationType, GUID_NOTIFICATION_SEGMENT ) )
			{
				IDirectMusicSegmentState *pSegmentState;
				if( SUCCEEDED( pNotifyEvent->punkUser->QueryInterface( IID_IDirectMusicSegmentState, (void**)&pSegmentState ) ) )
				{
					// Ensure this notification is for our segment state
					if( pSegmentState == m_pSegmentState )
					{
						switch( pNotifyEvent->dwNotificationOption )
						{
							case DMUS_NOTIFICATION_SEGLOOP:
								m_mtLastLoop = pNotifyEvent->mtTime;

								// If a piano roll strip is active, call its OnLoop method to update the active
								// variation if we're recording and the part ref plays numerical variations.
								if( m_pActivePianoRollStrip )
								{
									m_pActivePianoRollStrip->OnLoop();
								}

								// If any MIDI notes were inserted, update the pattern editor on a segment loop event.
								if( m_fNoteInserted )
								{
									CoInitialize(NULL);
									UpdateOnDataChanged( IDS_EDIT_INSERT );
									CoUninitialize();
									// Handled by UpdateOnDataChanged
									//m_fNoteInserted = FALSE;
								}
								break;
							case DMUS_NOTIFICATION_SEGABORT:
							case DMUS_NOTIFICATION_SEGEND:
								{
									POSITION position = m_pPRSList.GetHeadPosition();
									while (position != NULL)
									{
										m_pPRSList.GetNext(position)->UpdateNoteCursorTime();
									}
								}
								break;
						}
					}
					pSegmentState->Release();
				}
			}
			// Handle GUID_NOTIFICATION_RECOMPOSE notifications
			else if( ::IsEqualGUID ( pNotifyEvent->guidNotificationType, GUID_NOTIFICATION_RECOMPOSE ) )
			{
                // BUGBUG 
                // More needs to be added here (maybe all that's needed are critical sections).
                // Desired behavior is to reload the strip manager from the track.
                // This is causing a number of problems stemming from the fact that the
                // track is playing while it's being reloaded.
                /*  This causes various crashes...
                Load( NULL );
                OnUpdate( GUID_Segment_AllTracksAdded, pNotifyEvent->dwGroupID, NULL );
                InvalidateAllStrips();
                */
            }
		}
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr IDMUSProdPortNotify

HRESULT CMIDIMgr::OnOutputPortsChanged( void )
{
	// If we have an active piano roll strip
	if( m_pActivePianoRollStrip )
	{
		// If that strip is already thruing, cancel the thru
		if( m_pActivePianoRollStrip->m_fMIDIThruEnabled )
		{
			m_pActivePianoRollStrip->CancelPChannelThru();
		}

		// Enable thruing to the new port
		if( m_fWindowActive && m_pSegmentState && m_fRecordEnabled )
		{
			m_pActivePianoRollStrip->SetPChannelThru();
		}
	}
	return S_OK;
}

HRESULT CMIDIMgr::OnOutputPortsRemoved( void )
{
	if( m_pActivePianoRollStrip && 
		m_pActivePianoRollStrip->m_fMIDIThruEnabled )
	{
		m_pActivePianoRollStrip->CancelPChannelThru();
	}
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// CMIDIMgr IPersist

HRESULT CMIDIMgr::GetClassID( CLSID* pClsId )
{
	UNREFERENCED_PARAMETER(pClsId);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr IPersistStream functions

HRESULT CMIDIMgr::IsDirty()
{
	return m_fDirty;
}

HRESULT CMIDIMgr::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    bool fLoadFromTrack = false;

	m_fShuttingDown = TRUE;

	// The melody load structure list must be empty
	while( !m_lstMelodyLoadStructures.IsEmpty() )
	{
		delete m_lstMelodyLoadStructures.RemoveHead();
	}

    if (!pIStream)
    {
		PopulateMelodyLoadStructureList();
	}

	// Remove all piano roll strips
	while( !m_pPRSList.IsEmpty() )
	{
		CPianoRollStrip* pPRS;
		pPRS = m_pPRSList.RemoveHead();
		m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)pPRS);
		m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pPRS );
		KillFocus( pPRS );
		pPRS->Release();
	}

	// Delete the pattern
	if (m_pDMPattern != NULL)
	{
		// This is either an Undo/Redo or a change to the Pattern from the Pattern properties.
		delete m_pDMPattern;
		m_pDMPattern = NULL;
	}

	// This should already be empty.. But just in case.
	while( !m_lstStyleParts.IsEmpty() )
	{
		delete m_lstStyleParts.RemoveHead();
	}
	m_fShuttingDown = FALSE;

	// This should be NULL.. But just in case.
	if( m_pChordTrack )
	{
		m_pTimeline->RemoveStripMgr( m_pChordTrack->m_pIStripMgr );
		delete m_pChordTrack;
		m_pChordTrack = NULL;
	}

	m_pDMPattern = new CDirectMusicPattern(this, FALSE);
	if( m_pDMPattern == NULL )
	{
		return E_OUTOFMEMORY;
	}

	m_NotePropData.m_Param.pPart = NULL;
	RefreshPropertyPage( NULL );
	HRESULT hr = S_OK;
    if (!pIStream)
    {
        hr = CreateStreamFromPatternTrack(&pIStream);
        fLoadFromTrack = true;
    }

    if (SUCCEEDED(hr))
    {
	    hr = m_pDMPattern->Load( pIStream );
        if (fLoadFromTrack)
        {
            pIStream->Release();
        }
    }

	// If there are not parts
	if( m_lstStyleParts.IsEmpty() )
	{
		// Create an empty Part
		CDirectMusicPart* pPart = AllocPart();
		if( pPart != NULL )
		{
			// Initialize the part's TimeSig and length with the Pattern's
			pPart->m_TimeSignature = m_pDMPattern->m_TimeSignature;
			pPart->m_mtClocksPerBeat = DM_PPQNx4 / (long)pPart->m_TimeSignature.m_bBeat;
			pPart->m_mtClocksPerGrid = pPart->m_mtClocksPerBeat / (long)pPart->m_TimeSignature.m_wGridsPerBeat;
			pPart->m_wNbrMeasures = m_pDMPattern->m_wNbrMeasures;

			// Create a Part Reference
			CDirectMusicPartRef* pPartRef = m_pDMPattern->AllocPartRef();
			if( pPartRef == NULL )
			{
				DeletePart( pPart );
			}
			else
			{
				// If in a Pattern track, name the PartRef "Pattern"
				if( m_pIDMTrack )
				{
					pPartRef->m_strName.LoadString( IDS_PATTERN_TEXT );
				}
				// Otherwise, convert the PChannel # to a string
				else
				{
					CString cstrTrack;
					cstrTrack.LoadString( IDS_TRACK );
					pPartRef->m_strName.Format( cstrTrack, 1 );
				}

				// Point the PartRef at the Part
				pPartRef->SetPart( pPart );
			}
		}
	}

	if( SUCCEEDED( hr ) && m_pIDMTrack )
	{
		UpdateDirectMusicTrack();
	}

	return hr;
}

HRESULT CMIDIMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr;

	ASSERT( m_pDMPattern != NULL );

	if (m_pDMPattern != NULL)
	{
		hr = m_pDMPattern->Save( pIStream, fClearDirty );
	}
	else
	{
		hr = E_ABORT;
	}

	if (SUCCEEDED(hr))
	{
		//if (fClearDirty == TRUE)
		//{
			m_fDirty = FALSE;
		//}
	}
	return hr;
}

HRESULT CMIDIMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	UNREFERENCED_PARAMETER(pcbSize);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::CreateStreamFromPatternTrack
HRESULT CMIDIMgr::CreateStreamFromPatternTrack(IStream** ppIStream)
{
    HRESULT hr = E_FAIL;

    if( m_pIDMTrack == NULL || m_pIFramework == NULL)
	{
		return E_FAIL;
	}

	IStream* pIStream = NULL;
	if( SUCCEEDED ( m_pIFramework->AllocMemoryStream(FT_RUNTIME, GUID_CurrentVersion, &pIStream) ) )
	{
		// Now, save the track to the stream
		IPersistStream* pIPersistStream;
		if( SUCCEEDED( m_pIDMTrack->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
        {
		    if( SUCCEEDED( pIPersistStream->Save( pIStream, TRUE ) ) )
		    {
			    // Seek back to the beginning
			    StreamSeek( pIStream, 0, STREAM_SEEK_SET );
    			// Return the stream
                *ppIStream = pIStream;
                hr = S_OK;
            }
            pIPersistStream->Release();
        }
    }

    if (FAILED(hr) && pIStream)
    {
        pIStream->Release();
    }

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject functions

HRESULT STDMETHODCALLTYPE CMIDIMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_NotePropData.m_PropNote = m_SelectedPropNote;
	*ppData = (void*)&m_NotePropData;
	return S_OK;
}

void CMIDIMgr::PlayPatch( MUSIC_TIME mtTime, DWORD dwPChannel )
{
	bool fFoundPatch = false;
	DWORD dwPatch = 0;
	DWORD dwIndex = 0;
	MUSIC_TIME mtLatestBandTime = LONG_MIN;

	// Continue forever
	while( TRUE )
	{
		// Get a pointer to the Band stripmgr at index dwIndex
		IDMUSProdStripMgr *pBandStripMgr = NULL;
		if( SUCCEEDED( m_pTimeline->GetStripMgr( GUID_BandStrip_InstrumentItem, m_dwGroupBits, dwIndex, &pBandStripMgr ) ) )
		{
			// Try and get the instrument on PChannel dwPChannel
			BandStrip_InstrumentItem bandStrip_InstrumentItem;
			bandStrip_InstrumentItem.dwPChannel = dwPChannel;
			MUSIC_TIME mtNextBand;
			HRESULT hr = pBandStripMgr->GetParam( GUID_BandStrip_InstrumentItem, mtTime, &mtNextBand, &bandStrip_InstrumentItem );

			// If we found the instrument
			if( SUCCEEDED( hr ) )
			{
				// Flag that we found a patch
				fFoundPatch = true;

				// If we found a band and it's later than all other bands,
				// save it to use for this time span
				if( bandStrip_InstrumentItem.lPhysicalTime > mtLatestBandTime )
				{
					dwPatch = bandStrip_InstrumentItem.dwPatch;
					mtLatestBandTime = bandStrip_InstrumentItem.lPhysicalTime;
				}
			}

			pBandStripMgr->Release();
		}
		else
		{
			// No more strips to check - exit
			break;
		}
		dwIndex++;
	}

	if( fFoundPatch )
	{
		DMUS_PATCH_PMSG *pPatchEvent;
		if( FAILED( m_pIDMPerformance->AllocPMsg( sizeof(DMUS_PATCH_PMSG), (DMUS_PMSG **)&pPatchEvent ) ) )
		{
			return;
		}

		REFERENCE_TIME rtLatency;
		m_pIDMPerformance->GetLatencyTime( &rtLatency );

		ZeroMemory( pPatchEvent, sizeof(DMUS_PATCH_PMSG) );
		// PMSG fields
		pPatchEvent->dwSize = sizeof(DMUS_PATCH_PMSG);
		m_pIDMPerformance->GetLatencyTime( &pPatchEvent->rtTime );
	//	pPatchEvent->mtTime = 0;
		pPatchEvent->dwFlags = DMUS_PMSGF_REFTIME;
		pPatchEvent->dwPChannel = dwPChannel;
		pPatchEvent->dwVirtualTrackID = 1;
	//	pPatchEvent->pTool = NULL;
	//	pPatchEvent->pGraph = NULL;
		pPatchEvent->dwType = DMUS_PMSGT_PATCH;
	//	pPatchEvent->punkUser = 0;

		// PATCH fields
		pPatchEvent->byInstrument = BYTE(dwPatch & 0x7f);
		pPatchEvent->byMSB = BYTE((dwPatch >> 16) & 0x7f);
		pPatchEvent->byLSB = BYTE((dwPatch >> 8) & 0x7f);
	//	pPatchEvent->byPad[0] = 0;

		SendPMsg( (DMUS_PMSG *)pPatchEvent );
	}
}

void CMIDIMgr::PlayNote( const CPropNote *pPropNote, DWORD dwPChannel )
{
	ASSERT( pPropNote );

	if( !pPropNote || !m_pIDMPerformance )
	{
		return;
	}

	// Stop any playing note
	StopNote();

	REFERENCE_TIME rtLatency;
	m_pIDMPerformance->GetLatencyTime( &rtLatency );
	MUSIC_TIME mtLatency;
	m_pIDMPerformance->ReferenceToMusicTime( rtLatency, &mtLatency );

	// If there currently is no primary segment playing, send down the current active band
	IDirectMusicSegmentState *pSegState;
	if( FAILED( m_pIDMPerformance->GetSegmentState( &pSegState, mtLatency ) ) )
	{
		IDMUSProdStyleInfo *pStyleInfo;
		if( m_pIStyleNode
		&&	SUCCEEDED( m_pIStyleNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pStyleInfo ) ) )
		{
			IUnknown *punkMIDIMgr;
			if( SUCCEEDED( QueryInterface( IID_IUnknown, (void**)&punkMIDIMgr ) ) )
			{
				IUnknown *punkBand;
				if( SUCCEEDED( pStyleInfo->GetActiveBandForObject( punkMIDIMgr, &punkBand ) ) )
				{
					IDMUSProdBandEdit8 *pIDMUSProdBandEdit8;
					if( SUCCEEDED( punkBand->QueryInterface( IID_IDMUSProdBandEdit8, (void **)&pIDMUSProdBandEdit8 ) ) )
					{
						DWORD dwPatch;
						if( SUCCEEDED( pIDMUSProdBandEdit8->GetPatchForPChannel( dwPChannel, &dwPatch ) ) )
						{
							REFERENCE_TIME rtLatency;
							m_pIDMPerformance->GetLatencyTime( &rtLatency );
							DMUS_PATCH_PMSG *pPatchEvent;
							if( FAILED( m_pIDMPerformance->AllocPMsg( sizeof(DMUS_PATCH_PMSG), (DMUS_PMSG **)&pPatchEvent ) ) )
							{
								pIDMUSProdBandEdit8->Release();
								punkBand->Release();
								punkMIDIMgr->Release();
								pStyleInfo->Release();
								return;
							}

							ZeroMemory( pPatchEvent, sizeof(DMUS_PATCH_PMSG) );
							// PMSG fields
							pPatchEvent->dwSize = sizeof(DMUS_PATCH_PMSG);
							m_pIDMPerformance->GetLatencyTime( &pPatchEvent->rtTime );
						//	pPatchEvent->mtTime = 0;
							pPatchEvent->dwFlags = DMUS_PMSGF_REFTIME;
							pPatchEvent->dwPChannel = dwPChannel;
							pPatchEvent->dwVirtualTrackID = 1;
						//	pPatchEvent->pTool = NULL;
						//	pPatchEvent->pGraph = NULL;
							pPatchEvent->dwType = DMUS_PMSGT_PATCH;
						//	pPatchEvent->punkUser = 0;

							// PATCH fields
							pPatchEvent->byInstrument = BYTE(dwPatch & 0x7f);
							pPatchEvent->byMSB = BYTE((dwPatch >> 16) & 0x7f);
							pPatchEvent->byLSB = BYTE((dwPatch >> 8) & 0x7f);
						//	pPatchEvent->byPad[0] = 0;

							SendPMsg( (DMUS_PMSG *)pPatchEvent );
						}
						pIDMUSProdBandEdit8->Release();
					}
					punkBand->Release();
				}
				punkMIDIMgr->Release();
			}
			pStyleInfo->Release();
		}

		// If we're in the segment designer
		if( m_pIDMTrack )
		{
			PlayPatch( pPropNote->AbsTime(), dwPChannel );
		}
	}
	else
	{
		pSegState->Release();
		//pSegState = NULL;
	}

	DMUS_MIDI_PMSG *pMIDIOnEvent = NULL;
	if( FAILED( m_pIDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pMIDIOnEvent ) ) )
	{
		return;
	}

	ASSERT( pMIDIOnEvent != NULL );
	ZeroMemory( pMIDIOnEvent, sizeof(DMUS_MIDI_PMSG) );
	// PMSG fields
	pMIDIOnEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
	m_pIDMPerformance->GetLatencyTime( &rtLatency );
	pMIDIOnEvent->rtTime = rtLatency + 5000; // Add 0.5ms
//	pMIDIOnEvent->mtTime = 0;
	pMIDIOnEvent->dwFlags = DMUS_PMSGF_REFTIME;
	pMIDIOnEvent->dwPChannel = dwPChannel;
	pMIDIOnEvent->dwVirtualTrackID = 1;
//	pMIDIOnEvent->pTool = NULL;
//	pMIDIOnEvent->pGraph = NULL;
	pMIDIOnEvent->dwType = DMUS_PMSGT_MIDI;
//	pMIDIOnEvent->punkUser = 0;

	// DMNoteEvent fields
	pMIDIOnEvent->bStatus = MIDI_NOTEON;
	pMIDIOnEvent->bByte1 = pPropNote->m_bMIDIValue;
	pMIDIOnEvent->bByte2 = pPropNote->m_bVelocity;
//	pMIDIOnEvent->bPad[0] = 0;

	SendPMsg( (DMUS_PMSG *)pMIDIOnEvent );
	// The playback engine will release the event

	//TRACE("Note On sent at %I64x for %x on %x.\n", pMIDIOnEvent->rtTime, pMIDIOnEvent->bByte1, pMIDIOnEvent->dwPChannel );

	// Reset the time and value of the last note played
	m_rtLastPlayNoteOffTime = rtLatency + 10000;
	m_bLastPlayNoteMIDIValue = pPropNote->m_bMIDIValue;
	m_dwLastPlayNotePChannel = dwPChannel;

	return;
}

void CMIDIMgr::StopNote( void )
{
	if( !m_pIDMPerformance )
	{
		return;
	}

	if( !m_rtLastPlayNoteOffTime ) 
	{
		return;
	}

	DMUS_MIDI_PMSG *pMIDIOffEvent = NULL;
	if( FAILED( m_pIDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pMIDIOffEvent ) ) )
	{
		return;
	}

	REFERENCE_TIME rtQueue;
	if( SUCCEEDED( m_pIDMPerformance->GetQueueTime( &rtQueue ) )
	&&	(rtQueue > m_rtLastPlayNoteOffTime) )
	{
		m_rtLastPlayNoteOffTime = rtQueue;
	}

	ASSERT( pMIDIOffEvent != NULL );
	ZeroMemory( pMIDIOffEvent, sizeof(DMUS_MIDI_PMSG) );
	// PMSG fields
	pMIDIOffEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
	pMIDIOffEvent->rtTime = m_rtLastPlayNoteOffTime;
//	pMIDIOffEvent->mtTime = 0;
	pMIDIOffEvent->dwFlags = DMUS_PMSGF_REFTIME;
	pMIDIOffEvent->dwPChannel = m_dwLastPlayNotePChannel;
	pMIDIOffEvent->dwVirtualTrackID = 1;
//	pMIDIOffEvent->pTool = NULL;
//	pMIDIOffEvent->pGraph = NULL;
	pMIDIOffEvent->dwType = DMUS_PMSGT_MIDI;
//	pMIDIOffEvent->punkUser = 0;

	// DMNoteEvent fields
	pMIDIOffEvent->bStatus = MIDI_NOTEOFF;
	pMIDIOffEvent->bByte1 = m_bLastPlayNoteMIDIValue;
//	pMIDIOffEvent->bByte2 = 0;
//	pMIDIOffEvent->bPad[0] = 0;

	SendPMsg( (DMUS_PMSG *)pMIDIOffEvent );
	// The playback engine will release the event

	//TRACE("Note Off sent at %I64x for %x on %x.\n", pMIDIOffEvent->rtTime, pMIDIOffEvent->bByte1, pMIDIOffEvent->dwPChannel );

	// Reset the time and value of the last note played
	m_rtLastPlayNoteOffTime = 0;

	return;
}

HRESULT CMIDIMgr::SendPMsg( DMUS_PMSG *pPMsg )
{
	if( !m_pIDMPerformance )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	IDirectMusicAudioPath *pDMAudioPath;
	if( SUCCEEDED( m_pIDMPerformance->GetDefaultAudioPath( &pDMAudioPath ) ) )
	{
		if( (pPMsg->dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH)
		||	(pPMsg->dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT) )
		{
			pPMsg->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
		}

		if( (pPMsg->dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS)
		||	SUCCEEDED( pDMAudioPath->ConvertPChannel( pPMsg->dwPChannel, &pPMsg->dwPChannel ) ) )
		{
			hr = m_pIDMPerformance->SendPMsg( pPMsg );
		}
		pDMAudioPath->Release();
	}

	return hr;
}

DWORD CMIDIMgr::SelectedNoteVariations( CDirectMusicPart* pPart )
{
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return 0;
	}

	DWORD dwVariations = 0;
	CDirectMusicEventItem* pEvent = pPart->m_lstNotes.GetHead();
	while( pEvent )
	{
		if ( pEvent->m_fSelected )
		{
			dwVariations |= pEvent->m_dwVariation;
		}
		pEvent = pEvent->GetNext();
	}
	return dwVariations;
}


DWORD CMIDIMgr::ApplyToSelectedNotes( CDirectMusicPartRef* pPartRef, DWORD dwVariations, const CPropNote* pPropNote )
{
	// NOTE:ApplyToSelected should call PreChangePartRef before it changes anything
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return 0;
	}

	ASSERT( pPropNote != NULL );
	if ( pPropNote == NULL )
	{
		return 0;
	}

	CDirectMusicPart *pPart = pPartRef->m_pDMPart;
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return 0;
	}

	CDirectMusicNoteList lstMovedEvents;

	DWORD dwChanged = 0;
	CDirectMusicEventItem* pEvent = pPart->GetFirstNote( dwVariations );
	while( pEvent )
	{
		if ( pEvent->m_fSelected &&
			 pEvent->m_dwVariation & dwVariations )
		{
			CDirectMusicStyleNote* pDMNote;
			
			pDMNote = (CDirectMusicStyleNote *)pEvent;

			// If UD_COPY is cleared, or UD_COPY is set and we're not changing the variations
			if( !(pPropNote->m_dwUndetermined & UD_COPY) ||
				((pPropNote->m_dwUndetermined & UD_COPY) && !(pPropNote->m_dwChanged & CHGD_VARIATIONS)) )
			{
				// Duplicate the note if necessary (if the note belongs to other variations, or if UD_COPY is cleared)
				pPart->CreateNoteCopyIfNeeded( pDMNote, dwVariations, !(pPropNote->m_dwUndetermined & UD_COPY) );
			}

			// Save the old start time
			long lOldValue = pPart->AbsTime( pDMNote );

			// Save the old chord time
			MUSIC_TIME mtOldChordTime = pDMNote->m_mtGridStart;

			if ( ( pPropNote->m_fOffset ) ||
				!((~pPropNote->m_dwUndetermined) & (UD_CHORDBAR | UD_CHORDBEAT| UD_STARTBARBEATGRIDTICK | UD_ENDBARBEATGRIDTICK | UD_DURBARBEATGRIDTICK)) )
			{
				// Either we're doing an offset or we're not changing any start/end/dur
				// bar/beat/grid/tick
				dwChanged |= pPropNote->ApplyToDMNote(pDMNote, pPartRef);
			}
			else
			{
				// We're not doing an offset, and we're changing at least one
				// start/end/dur bar/beat/grid/tick
				ASSERT( ~pPropNote->m_dwUndetermined & (UD_CHORDBAR | UD_CHORDBEAT| UD_STARTBARBEATGRIDTICK | UD_ENDBARBEATGRIDTICK | UD_DURBARBEATGRIDTICK) );
				CPropNote propnote( pPart );
				propnote.ImportFromDMNote(pDMNote, pPartRef);
				propnote.ApplyPropNote(pPropNote, pPartRef);

				// Duration, GridStart, and Offset should all be determined
				ASSERT( (propnote.m_dwUndetermined & (UD_DURATION | UD_GRIDSTART | UD_OFFSET)) == 0);
				propnote.m_dwUndetermined = ~(UD_DURATION | UD_GRIDSTART | UD_OFFSET);
				dwChanged |= propnote.ApplyToDMNote(pDMNote, pPartRef);
			}

			if( pDMNote->m_mtGridStart != mtOldChordTime )
			{
				pPartRef->UpdateNoteAfterMove( pDMNote );
			}

			// If the note's start position changed, remove and re-add it to the list
			if( lOldValue != pPart->AbsTime( pDMNote ) )
			{
				// Save a pointer to the next event
				CDirectMusicEventItem *pDMEvent2 = pEvent->GetNext();

				if( pPart->AbsTime( pDMNote ) < lOldValue )
				{
					// TODO: Optimize this, if possible
					// Move note backwards - remove it and re-add it later

					// Remove the event from the part's list
					pPart->m_lstNotes.Remove( pEvent );

					// Add it to our private list
					lstMovedEvents.AddTail( pEvent );
				}

				// If moved forwards, only add to lstMovedSequences if we're now after
				// the item at pos.
				else
				{
					if( pDMEvent2
					&& (pPart->AbsTime( pDMEvent2 ) < pPart->AbsTime(pEvent)) )
					{
						// Remove the event from the part's list
						pPart->m_lstNotes.Remove( pEvent );

						// Add it to our private list
						lstMovedEvents.AddTail( pEvent );
					}
				}
				// Set the next pDMEvent
				pEvent = pDMEvent2;
			}
			else
			{
				pEvent = pEvent->GetNext();
			}
		}
		else
		{
			pEvent = pEvent->GetNext();
		}
	}

	// Merge back in any notes we moved
	if( lstMovedEvents.GetHead() )
	{
		pPart->MergeNoteList( &lstMovedEvents );
	}

	return dwChanged;
}

void CMIDIMgr::SetVarLocksArray( PianoRollData* pPRD ) const
{
	// Initialize variation lock id's array
	memset( pPRD->adwVarLocks, 0, sizeof(pPRD->adwVarLocks) );

	CDirectMusicPartRef* pDMPartRef;

	// Populate variation lock id's array
	POSITION pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		pDMPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );

		if( pDMPartRef->m_bVariationLockID )
		{
			pPRD->adwVarLocks[pDMPartRef->m_bVariationLockID >> 5] |= 
				( 1 << (pDMPartRef->m_bVariationLockID % 32) ); 
		}
	}
}

int CMIDIMgr::ComputeSelectedPropNote( CDirectMusicPart* pPart, DWORD dwVariations )
{
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return -1;
	}

	CPianoRollStrip* pPRS;
	pPRS = DMPartToStrip(pPart); // BUGBUG: This can find the wrong strip!
	ASSERT( pPRS != NULL );
	if ( pPRS == NULL )
	{
		return E_UNEXPECTED;
	}

	// Initialize InversionId array
	memset( m_SelectedPropNote.m_adwInversionIds, 0, sizeof(m_SelectedPropNote.m_adwInversionIds) );

	// Initialize TimeSig
	m_SelectedPropNote.SetPart( pPart );

	int iVal = 0;
	CDirectMusicEventItem* pEvent = pPart->m_lstNotes.GetHead();
	while( pEvent )
	{
		CDirectMusicStyleNote* pDMNote = (CDirectMusicStyleNote *)pEvent;

		// Populate InversionId array
		if( pDMNote->m_bInversionId )
		{
			m_SelectedPropNote.m_adwInversionIds[pDMNote->m_bInversionId >> 5] |= 
				( 1 << (pDMNote->m_bInversionId % 32) ); 
		}

		if ( pEvent->m_fSelected && (pEvent->m_dwVariation & dwVariations) )
		{
			if ( iVal != 0 )
			{
				CPropNote propnote(pPart);
				propnote.ImportFromDMNote(pDMNote, pPRS->m_pPartRef);
				m_SelectedPropNote += propnote;
			}
			else
			{
				m_SelectedPropNote.ImportFromDMNote(pDMNote, pPRS->m_pPartRef);
			}
			iVal++;
		}
		pEvent = pEvent->GetNext();
	}
	if (iVal == 0)
	{
		m_NotePropData.m_Param.pPart = NULL;
		m_SelectedPropNote = CPropNote(pPart);
	}
	else
	{
		if ( ( m_NotePropData.m_Param.pPart != pPart) ||
			(m_NotePropData.m_Param.dwVariations !=  dwVariations) )
		{
			m_NotePropData.m_Param.pPart = pPart;
			m_NotePropData.m_Param.dwVariations = dwVariations;
		}
	}
	return iVal;
}


HRESULT STDMETHODCALLTYPE CMIDIMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( pData != NULL );
	NotePropData* pNotePropData = (NotePropData*)pData;
	
	//TRACE("%s\n", pNotePropData->m_PropNote.ToString());

	// We better have this
	ASSERT( m_pActivePianoRollStrip );

	//BUGBUG: Assume we're changing m_pActivePianoRollStrip->m_pPartRef
	CDirectMusicPartRef *pDMPartRef;
	if( m_pActivePianoRollStrip && m_pActivePianoRollStrip->m_pPartRef )
	{
		pDMPartRef = m_pActivePianoRollStrip->m_pPartRef;
		PreChangePartRef( pDMPartRef );
		pNotePropData->m_Param.pPart = pDMPartRef->m_pDMPart;
	}
	else if( pNotePropData->m_Param.pPart )
	{
		pDMPartRef = m_pDMPattern->FindPartRefByGUID( pNotePropData->m_Param.pPart->m_guidPartID );
		if( !pDMPartRef )
		{
			ASSERT(FALSE);
			return E_FAIL;
		}
	}
	else
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	if ( ( m_NotePropData.m_Param.pPart != pNotePropData->m_Param.pPart) ||
		  (m_NotePropData.m_Param.dwVariations !=  pNotePropData->m_Param.dwVariations) )
	{
		m_NotePropData.m_Param.pPart = pNotePropData->m_Param.pPart;
		m_NotePropData.m_Param.dwVariations = pNotePropData->m_Param.dwVariations;
	}

	// Apply the data to our selected notes and see what's changed
	DWORD dwChanged;
	dwChanged = ApplyToSelectedNotes( pDMPartRef,
									  pNotePropData->m_Param.dwVariations,
									 &pNotePropData->m_PropNote );

	// Play the selected DMNote if the velocity or value has changed.
	/* Don't do this
	if ( ((dwChanged & (UD_MUSICVALUE | UD_VELOCITY)) != 0) &&
		 (m_pActivePianoRollStrip != NULL) )
	{
		if ( m_pActivePianoRollStrip->m_pPartRef &&
			(m_pActivePianoRollStrip->m_pPartRef->m_pDMPart == pNotePropData->m_Param.pPart) )
		{
			DWORD dwPRVariations = m_pActivePianoRollStrip->m_dwVariations;
			CDirectMusicEventItem* pEvent = pNotePropData->m_Param.pPart->GetFirstNote( dwPRVariations );

			for( ; pEvent; pEvent = pEvent->GetNext() )
			{
				if ( (pEvent->m_dwVariation & dwPRVariations) &&
					( pEvent->m_fSelected == TRUE ))
				{
					CPropNote propNote;
					propNote.m_mtDuration = reinterpret_cast<CDirectMusicStyleNote *>(pEvent)->m_mtDuration;
					propNote.m_bVelocity = reinterpret_cast<CDirectMusicStyleNote *>(pEvent)->m_bVelocity;
					propNote.m_bMIDIValue = reinterpret_cast<CDirectMusicStyleNote *>(pEvent)->m_bMIDIValue;
					PlayNote( &propNote, m_pActivePianoRollStrip->m_pPartRef->m_dwPChannel );
					break;
				}
			}
		}
	}
	*/
	// If anything has changed, redraw the strip(s)
	if ( dwChanged != 0 )
	{
		m_fDirty = TRUE;

		if( pNotePropData->m_fUpdatePatternEditor )
		{
			// update the corresponding DirectMusicPart
			UpdatePartParamsAfterChange( pNotePropData->m_Param.pPart );

			// Let the object know about the changes
			UpdateOnDataChanged( IDS_UNDO_NOTE_CHANGE );
		}

		// Invalidate all strips displaying this part
		RefreshPartDisplay( pNotePropData->m_Param.pPart,
							pNotePropData->m_Param.dwVariations,
							FALSE, FALSE );
	}
	// Always recompute m_SelectedPropNote and update the property page
	ComputeSelectedPropNote( pNotePropData->m_Param.pPart,
							 pNotePropData->m_Param.dwVariations );
	RefreshPropertyPage( pNotePropData->m_Param.pPart );

	return S_OK;
}

void CMIDIMgr::RefreshPartDisplay( CDirectMusicPart* pPart, DWORD dwVariations, BOOL fRefreshCurveStrips, BOOL fRefreshMarkerStrip )
{
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return;
	}

	if (!m_fNoDisplayUpdate)
	{
		CPianoRollStrip* pPianoRollStrip;
		POSITION position;
		position = m_pPRSList.GetHeadPosition();
		while (position != NULL)
		{
			pPianoRollStrip = m_pPRSList.GetNext(position);
			if ( (pPianoRollStrip->m_pPartRef != NULL) &&
				 (pPianoRollStrip->m_pPartRef->m_pDMPart == pPart) &&
				 (pPianoRollStrip->m_dwVariations & dwVariations) )
			{
				// Tell the strip to recompute its selection state, since notes may
				// have been selected/unselected
				pPianoRollStrip->UpdateSelectionState();

				// Invalidate only the region displaying notes, not the entire strip
				pPianoRollStrip->InvalidatePianoRoll();
				pPianoRollStrip->InvalidateVariationBar();
				
				if( fRefreshCurveStrips )
				{
					pPianoRollStrip->InvalidateCurveStrips();
				}

				if( fRefreshMarkerStrip )
				{
					pPianoRollStrip->InvalidateMarkerStrip();
				}

				//m_pTimeline->StripInvalidateRect(pPianoRollStrip, NULL, TRUE);
			}
		}
	}
}

void CMIDIMgr::RefreshPartRefDisplay( const CDirectMusicPartRef* pPartRef )
{
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return;
	}

	if (!m_fNoDisplayUpdate)
	{
		CPianoRollStrip* pPianoRollStrip;
		POSITION position;
		position = m_pPRSList.GetHeadPosition();
		while (position != NULL)
		{
			pPianoRollStrip = m_pPRSList.GetNext(position);
			if ( pPianoRollStrip->m_pPartRef == pPartRef )
			{
				/*
				if( fOnlyStatusBar )
				{
				*/
					// Invalidate only the function bar area
					pPianoRollStrip->InvalidateFunctionBar();
				/*
				}
				else
				{
					m_pTimeline->StripInvalidateRect(pPianoRollStrip, NULL, TRUE);
				}
				*/
			}
		}
	}
}

void CMIDIMgr::RefreshCurveStripStateLists( CDirectMusicPart* pPart )
{
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return;
	}

	ASSERT( !m_fNoDisplayUpdate );
	CPianoRollStrip* pPianoRollStrip;
	POSITION position;
	position = m_pPRSList.GetHeadPosition();
	while (position != NULL)
	{
		pPianoRollStrip = m_pPRSList.GetNext(position);
		if ( (pPianoRollStrip->m_pPartRef != NULL) &&
			 (pPianoRollStrip->m_pPartRef->m_pDMPart == pPart) )
		{
			// Make sure there is a curve strip for each type of Curve in this Part
			pPianoRollStrip->SyncCurveStripStateList();
			pPianoRollStrip->AddCurveStrips( FALSE );
		}
	}
}

// BUGBUG: Why does this method have a parameter?
void CMIDIMgr::RefreshPropertyPage( CDirectMusicPart* pPart )
{
	ASSERT( (m_NotePropData.m_Param.pPart == NULL) || (pPart == m_NotePropData.m_Param.pPart) );
	if( (m_NotePropData.m_Param.pPart != NULL) && (pPart != m_NotePropData.m_Param.pPart) )
	{
		return;
	}

	if (!m_fNoDisplayUpdate)
	{
		// Let the property page know about the changes
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}
}

HRESULT STDMETHODCALLTYPE CMIDIMgr::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Ensure our timeline pointer is valid
	if( m_pTimeline == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	// Get a pointer to the property sheet
	IDMUSProdPropSheet* pIPropSheet = NULL;
	m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
	if( pIPropSheet == NULL )
	{
		ASSERT(FALSE);
		return E_FAIL;
	}

	//  If the property sheet is hidden, exit
	if( pIPropSheet->IsShowing() != S_OK )
	{
		pIPropSheet->Release();
		return S_OK;
	}

	BOOL fDisplayPartProperties = TRUE;

	// See if there are selected notes
	if( m_pActivePianoRollStrip && m_pActivePianoRollStrip->ValidPartRefPtr() )
	{
		CDirectMusicEventItem* pEvent = m_pActivePianoRollStrip->m_pPartRef->m_pDMPart->GetFirstNote( m_pActivePianoRollStrip->m_dwVariations );
		while( pEvent )
		{
			if( pEvent->m_fSelected
			&&  pEvent->m_dwVariation & m_pActivePianoRollStrip->m_dwVariations )
			{
				fDisplayPartProperties = FALSE;
				break;
			}
			pEvent = pEvent->GetNext();
		}
	}

	if( fDisplayPartProperties )
	{
		if( m_pActivePianoRollStrip )
		{
			pIPropSheet->Release();

			// Display Part properties when no notes are selected
			return m_pActivePianoRollStrip->OnShowProperties();
		}
	}

	// If our property page is already displayed, exit
	if(m_fPropPageActive)
	{
		ASSERT( m_pPropPageMgr != NULL );
		pIPropSheet->Release();
		RefreshPropertyPage( m_NotePropData.m_Param.pPart );
		return S_OK;
	}

	// Get a reference to our property page manager
	HRESULT hr = S_OK;
	if( m_pPropPageMgr == NULL )
	{
		//CNotePropPageMgr* pPPM = new CNotePropPageMgr;
		m_pPropPageMgr = new CNotePropPageMgr;
		//if( NULL == pPPM ) return E_OUTOFMEMORY;
		if( NULL == m_pPropPageMgr )
		{
			pIPropSheet->Release();
			return E_OUTOFMEMORY;
		}
		//hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		//m_pPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
		//if( FAILED(hr) )
		//	goto EXIT;
	}

	// Save the focus so we can restore after changing the property page
	HWND hwndHadFocus = ::GetFocus();

	// Set the property page to refer to the Note property page.
	short nActiveTab = CNotePropPageMgr::sm_nActiveTab;
	m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	pIPropSheet->SetActivePage( nActiveTab );

	// Restore the focus if it has changed
	if( hwndHadFocus != ::GetFocus() )
	{
		::SetFocus( hwndHadFocus );
	}

	m_fPropPageActive = TRUE;

	// release our reference to the property sheet
	pIPropSheet->Release();

//EXIT:
	return hr;
}

HRESULT STDMETHODCALLTYPE CMIDIMgr::OnRemoveFromPageManager( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_fPropPageActive = FALSE;
	return S_OK;
}

CDirectMusicStyleNote* CMIDIMgr::DMPartToDMNote( CDirectMusicPart* pPart ) const
{
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return NULL;
	}

	return pPart->GetFirstNote( 0xFFFFFFFF );
}

// BUGBUG: This can find the wrong strip!
CPianoRollStrip* CMIDIMgr::DMPartToStrip( CDirectMusicPart* pPart ) const
{
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return NULL;
	}

	CPianoRollStrip* pPianoRollStrip = NULL;
	POSITION position;
	position = m_pPRSList.GetHeadPosition();
	while (position != NULL)
	{
		pPianoRollStrip = m_pPRSList.GetNext(position);
		ASSERT( pPianoRollStrip != NULL );
		if ( pPianoRollStrip->m_pPartRef )
		{
			if( pPianoRollStrip->m_pPartRef->m_pDMPart == pPart )
			{
				return pPianoRollStrip;
			}
		}
	}
	return NULL;
}

// BUGBUG: This can find the wrong strip!
CPianoRollStrip* CMIDIMgr::DMPartRefToStrip( CDirectMusicPartRef* pPartRef ) const
{
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return NULL;
	}

	CPianoRollStrip* pPianoRollStrip = NULL;
	POSITION position;
	position = m_pPRSList.GetHeadPosition();
	while (position != NULL)
	{
		pPianoRollStrip = m_pPRSList.GetNext(position);
		ASSERT( pPianoRollStrip != NULL );
		if ( pPianoRollStrip->m_pPartRef == pPartRef)
		{
			return pPianoRollStrip;
		}
	}
	return NULL;
}

void CMIDIMgr::InsertNote( CDirectMusicPartRef* pPartRef, CDirectMusicStyleNote * pDMNote, BOOL fUpdate )
{
	//TRACE("InsertNote In %d\n",  ::timeGetTime());
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return;
	}

	// This note can never play or be displayed - don't insert it
	if( pDMNote->m_dwVariation == 0 )
	{
		return;
	}

	PreChangePartRef( pPartRef );

	pPartRef->m_pDMPart->InsertNoteInAscendingOrder( pDMNote );
	m_fDirty = TRUE;
	
	// update the corresponding DirectMusicPart's parameters
	UpdatePartParamsAfterChange( pPartRef->m_pDMPart );

	if( fUpdate )
	{
		// Let the object know about the changes
		UpdateOnDataChanged( IDS_EDIT_INSERT );
	}
	else
	{
		m_fNoteInserted = TRUE;
	}
	
	// updated all views of this part
	RefreshPartDisplay( pPartRef->m_pDMPart, pDMNote->m_dwVariation, FALSE, FALSE );
	//TRACE("InsertNote out %d\n",  ::timeGetTime());
}

void CMIDIMgr::UpdateOnDataChanged( int nUndoString )
{
	if( m_fShuttingDown )
	{
		// Always reset this flag
		m_fUpdateDirectMusic = TRUE;
		return;
	}

	UpdateSelectedParamsFromActiveStrip();

	/*
	if( m_pActivePianoRollStrip
	&&  m_pActivePianoRollStrip->m_pPartRef )
	{
		m_pActivePianoRollStrip->m_pPartRef->SetInversionBoundaries();
	}
	*/

	m_nUndoString = nUndoString;
	m_fDirty = TRUE;

	BOOL fUpdateDirectMusic = m_fUpdateDirectMusic;

	// If the undo string is NULL, set TP_FREEZE_UNDO to TRUE so an
	// undo state is not added to the segment
	bool fResetFreezeToFalse = false;
	if( m_nUndoString == 0
	&&	m_strUndoString.IsEmpty() )
	{
		// Get the current value of TP_FREEZE_UNDO
		VARIANT var;
		m_pTimeline->GetTimelineProperty( TP_FREEZE_UNDO, &var );

		// If it is currently FALSE
		if( V_BOOL(&var) == FALSE )
		{
			// Flag that we need to reset it to FALSE when we're done
			fResetFreezeToFalse = true;

			// Set it to TRUE
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
		}
	}

	// Let the object know about the changes
	m_pTimeline->OnDataChanged(static_cast<IMIDIMgr *>(this));

	// If the original freeze state was FALSE, reset TP_FREEZE_UNDO to FALSE.
	if( fResetFreezeToFalse )
	{
		VARIANT var;
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
	}

	// Check if we need to update the DirectMusicTrack
	if( fUpdateDirectMusic && m_pIDMTrack )
	{
		UpdateDirectMusicTrack();
	}

	UpdateAuditionVariations();

	// Always reset this flag
	m_fUpdateDirectMusic = TRUE;

	m_fNoteInserted = FALSE;
}

DWORD MapVariations( DWORD dwOrigVaritions, DWORD dwVariationMap[32] )
{
	// Initialize the variation bits to return to 0
	DWORD dwVarsToReturn = 0;

	// Iterate through all 32 variations
	for ( int nClip = 0; nClip < 32; nClip++ )
	{
		// Check if the original variation bit nClip is set
		if ( dwOrigVaritions & (1 << nClip) )
		{
			// If so, set the bit in dwVarsToReturn that matches original bit nClip
			dwVarsToReturn |= dwVariationMap[nClip];
		}
	}

	// Return the new variation flags
	return dwVarsToReturn;
}

HRESULT CMIDIMgr::ImportCurveList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, PianoRollClipboardData *pPRCD, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged )
{
	// Validate parameters
	ASSERT( pPRCD != NULL );
	if ( pPRCD == NULL )
	{
		return E_INVALIDARG;
	}

	ASSERT( pDMPartRef != NULL );
	if( pDMPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a mapping of source variations to destination variations
	DWORD dwVariationMap[32];
	ZeroMemory( dwVariationMap, sizeof(DWORD) * 32 );
	int nClip, nCur, nTmp;
	HRESULT hr = S_OK;

	nCur = 0;
	for ( nClip = 0; nClip < 32 && hr == S_OK; nClip++ )
	{
		if ( pPRCD->dwVariations & (1 << nClip) )
		{
			for ( nTmp = nCur; nTmp < 32; nTmp++ )
			{
				if ( dwVariations & (1 << nTmp) )
				{
					dwVariationMap[nClip] = (1 << nTmp);
					nCur = nTmp + 1;
					break;
				}
			}
			if ( nTmp == 32 )
			{
				return E_ABORT;
			}
		}
	}

	// Read in the size of a curve
	DWORD dwCurveSize;
	ULONG ulRead;
	hr = pIStream->Read( &dwCurveSize, sizeof(long), &ulRead);
	if( ulRead != sizeof(long) || (sizeof(DMUS_IO_STYLECURVE) != dwCurveSize) )
	{
		return E_FAIL;
	}

	// Subtract off the data just read
	lDataSize -= ulRead; 

	// Insert the new curves into our part
	{
		// Prepare DMUS_IO_STYLECURVE structure
		DMUS_IO_STYLECURVE iDMStyleCurve;
		memset( &iDMStyleCurve, 0, dwCurveSize );
		hr = pIStream->Read( &iDMStyleCurve, dwCurveSize, NULL);
		while( (lDataSize >= (signed)dwCurveSize) && (hr == S_OK) )
		{
			// Subtract what was just read
			lDataSize -= dwCurveSize;

			ULONG ulRead;
			CDirectMusicStyleCurve *pTmpDMCurve;
			pTmpDMCurve = new CDirectMusicStyleCurve;

			// Copy the  DMUS_IO_STYLECURVE to a CDirectMusicStyleCurve
			pTmpDMCurve->m_dwVariation = iDMStyleCurve.dwVariation;
			pTmpDMCurve->m_mtDuration = iDMStyleCurve.mtDuration;
			pTmpDMCurve->m_mtResetDuration = iDMStyleCurve.mtResetDuration;
			pTmpDMCurve->m_nTimeOffset = iDMStyleCurve.nTimeOffset;

			pTmpDMCurve->m_nStartValue = iDMStyleCurve.nStartValue;
			pTmpDMCurve->m_nEndValue = iDMStyleCurve.nEndValue;
			pTmpDMCurve->m_nResetValue = iDMStyleCurve.nResetValue;
			pTmpDMCurve->m_bEventType = iDMStyleCurve.bEventType;
			pTmpDMCurve->m_bCurveShape = iDMStyleCurve.bCurveShape;
			pTmpDMCurve->m_bCCData = iDMStyleCurve.bCCData;
			pTmpDMCurve->m_bFlags = iDMStyleCurve.bFlags;
			pTmpDMCurve->m_wParamType = FILE_TO_MEMORY_WPARAMTYPE( iDMStyleCurve.wParamType );
			pTmpDMCurve->m_wMergeIndex = iDMStyleCurve.wMergeIndex;

			// Convert from the source TimeSig to the destination TimeSig
			pTmpDMCurve->m_mtGridStart = iDMStyleCurve.mtGridStart;
			ConvertTimeSigs( pTmpDMCurve, pPRCD->ts, pDMPartRef->m_pDMPart );
			pTmpDMCurve->m_mtGridStart += mtGrid;

			// Only insert the curve if it will start before the end of time.
			if ( pTmpDMCurve->m_mtGridStart < pDMPartRef->m_pDMPart->GetGridLength() )
			{
				// Select the curve
				pTmpDMCurve->m_fSelected = TRUE;

				// Set the curve's variation flags
				pTmpDMCurve->m_dwVariation = MapVariations( pTmpDMCurve->m_dwVariation, dwVariationMap );

				if( !fChanged )
				{
					fChanged = TRUE;
					PreChangePartRef( pDMPartRef );
				}

				// Finally, actually inset the curve
				pDMPartRef->m_pDMPart->InsertCurveInAscendingOrder( pTmpDMCurve );
			}
			else
			{
				pTmpDMCurve->SetNext(NULL);
				delete pTmpDMCurve;
			}
			if( lDataSize >= (signed)dwCurveSize )
			{
				hr = pIStream->Read( &iDMStyleCurve, dwCurveSize, &ulRead);
				if( ulRead != dwCurveSize)
				{
					hr = S_FALSE;
				}
			}
		}
	}

	if ( SUCCEEDED(hr) )
	{
		hr = S_OK;
	}

	return hr;
}

HRESULT CMIDIMgr::ImportMarkerList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, PianoRollClipboardData *pPRCD, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged )
{
	// Validate parameters
	ASSERT( pPRCD != NULL );
	if ( pPRCD == NULL )
	{
		return E_INVALIDARG;
	}

	ASSERT( pDMPartRef != NULL );
	if( pDMPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a mapping of source variations to destination variations
	DWORD dwVariationMap[32];
	ZeroMemory( dwVariationMap, sizeof(DWORD) * 32 );
	int nClip, nCur, nTmp;
	HRESULT hr = S_OK;

	nCur = 0;
	for ( nClip = 0; nClip < 32 && hr == S_OK; nClip++ )
	{
		if ( pPRCD->dwVariations & (1 << nClip) )
		{
			for ( nTmp = nCur; nTmp < 32; nTmp++ )
			{
				if ( dwVariations & (1 << nTmp) )
				{
					dwVariationMap[nClip] = (1 << nTmp);
					nCur = nTmp + 1;
					break;
				}
			}
			if ( nTmp == 32 )
			{
				return E_ABORT;
			}
		}
	}

	// Read in the size of a marker
	DWORD dwMarkerSize;
	ULONG ulRead;
	hr = pIStream->Read( &dwMarkerSize, sizeof(long), &ulRead);
	if( ulRead != sizeof(long) || (sizeof(DMUS_IO_STYLEMARKER) != dwMarkerSize) )
	{
		return E_FAIL;
	}

	// Subtract off the data just read
	lDataSize -= ulRead; 

	// Insert the new markers into our part

	// Prepare DMUS_IO_STYLEMARKER structure
	DMUS_IO_STYLEMARKER iDMStyleMarker;
	memset( &iDMStyleMarker, 0, sizeof(DMUS_IO_STYLEMARKER) );
	hr = pIStream->Read( &iDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER), NULL);
	while( (lDataSize >= (signed)sizeof(DMUS_IO_STYLEMARKER)) && (hr == S_OK) )
	{
		// Subtract what was just read
		lDataSize -= sizeof(DMUS_IO_STYLEMARKER);

		ULONG ulRead;
		CDirectMusicStyleMarker *pTmpDMMarker;
		pTmpDMMarker = new CDirectMusicStyleMarker;

		// Convert from the source TimeSig to the destination TimeSig
		pTmpDMMarker->m_mtGridStart = iDMStyleMarker.mtGridStart;
		ConvertTimeSigs( pTmpDMMarker, pPRCD->ts, pDMPartRef->m_pDMPart );
		pTmpDMMarker->m_mtGridStart += mtGrid;

		// Only insert the marker if it will start before the end of time.
		if ( pTmpDMMarker->m_mtGridStart < pDMPartRef->m_pDMPart->GetGridLength() )
		{
			// Select the marker
			pTmpDMMarker->m_fSelected = TRUE;

			// Already done by the CDirectMusicStyleMarker constructor
			//pTmpDMMarker->m_nTimeOffset = 0;

			// Copy the  DMUS_IO_STYLEMARKER to a CDirectMusicStyleMarker
			if( iDMStyleMarker.wMarkerFlags & DMUS_MARKERF_START )
			{
				pTmpDMMarker->m_dwEnterVariation = MapVariations( iDMStyleMarker.dwVariation, dwVariationMap );

				if( iDMStyleMarker.wMarkerFlags & DMUS_MARKERF_CHORD_ALIGN )
				{
					pTmpDMMarker->m_dwEnterChordVariation = pTmpDMMarker->m_dwEnterVariation;
				}
			}


			if( iDMStyleMarker.wMarkerFlags & DMUS_MARKERF_STOP )
			{
				pTmpDMMarker->m_dwExitVariation = MapVariations( iDMStyleMarker.dwVariation, dwVariationMap );

				if( iDMStyleMarker.wMarkerFlags & DMUS_MARKERF_CHORD_ALIGN )
				{
					pTmpDMMarker->m_dwExitChordVariation = pTmpDMMarker->m_dwExitVariation;
				}
			}

			if( !fChanged )
			{
				fChanged = TRUE;
				PreChangePartRef( pDMPartRef );
			}

			// Finally, actually inset the marker
			pDMPartRef->m_pDMPart->InsertMarkerInAscendingOrder( pTmpDMMarker );
		}
		else
		{
			pTmpDMMarker->SetNext(NULL);
			delete pTmpDMMarker;
		}

		if( lDataSize >= (signed)sizeof(DMUS_IO_STYLEMARKER) )
		{
			hr = pIStream->Read( &iDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER), &ulRead);
			if( ulRead != sizeof(DMUS_IO_STYLEMARKER))
			{
				hr = S_FALSE;
			}
		}
	}

	// Now, compact the marker list
	pDMPartRef->m_pDMPart->m_lstMarkers.CompactMarkerList();

	if ( SUCCEEDED(hr) )
	{
		hr = S_OK;
	}

	return hr;
}

HRESULT CMIDIMgr::ImportNoteList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, PianoRollClipboardData *pPRCD, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged, BOOL fUpdatePatternEditorIfNeeded )
{
	// Validate parameters
	ASSERT( pPRCD != NULL );
	if ( pPRCD == NULL )
	{
		return E_INVALIDARG;
	}

	ASSERT( pDMPartRef != NULL );
	if( pDMPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a mapping of source variations to destination variations
	DWORD dwVariationMap[32];
	ZeroMemory( dwVariationMap, sizeof(DWORD) * 32 );
	int nClip, nCur, nTmp;
	HRESULT hr = S_OK;

	nCur = 0;
	for ( nClip = 0; nClip < 32 && hr == S_OK; nClip++ )
	{
		if ( pPRCD->dwVariations & (1 << nClip) )
		{
			for ( nTmp = nCur; nTmp < 32; nTmp++ )
			{
				if ( dwVariations & (1 << nTmp) )
				{
					dwVariationMap[nClip] = (1 << nTmp);
					nCur = nTmp + 1;
					break;
				}
			}
			if ( nTmp == 32 )
			{
				return E_ABORT;
			}
		}
	}

	// Read in the size of a note
	DWORD dwNoteSize;
	ULONG ulRead;
	hr = pIStream->Read( &dwNoteSize, sizeof(long), &ulRead);
	if( ulRead != sizeof(long) || (sizeof(CDirectMusicStyleNote) != dwNoteSize) )
	{
		return E_FAIL;
	}

	// Subtract off the data just read
	lDataSize -= ulRead; 

	// Insert the new notes into our part
	{
		CDirectMusicStyleNote dmNote;
		hr = pIStream->Read( &dmNote, dwNoteSize, NULL);
		while( (lDataSize >= (signed)dwNoteSize) && (hr == S_OK) )
		{
			// Subtract what was just read
			lDataSize -= dwNoteSize;

			ULONG ulRead;
			CDirectMusicStyleNote *pTmpDMNote;
			pTmpDMNote = new CDirectMusicStyleNote( &dmNote );

			// Convert from the source TimeSig to the destination TimeSig
			ConvertTimeSigs( pTmpDMNote, pPRCD->ts, pDMPartRef->m_pDMPart );
			pTmpDMNote->m_mtGridStart += mtGrid;

			// Only insert the note if it will start before the end of time.
			if ( pTmpDMNote->m_mtGridStart < pDMPartRef->m_pDMPart->GetGridLength() )
			{
				pTmpDMNote->m_fSelected = TRUE;
				// Adjust the note's MuiscValue if necessary
				if ( pTmpDMNote->m_bPlayModeFlags == DMUS_PLAYMODE_NONE )
				{
					ASSERT( pDMPartRef->m_pDMPart->m_bPlayModeFlags != DMUS_PLAYMODE_NONE );

					// If the strip's playmode is DMUS_PLAYMODE_FIXED, convert the value from a musicvalue to a note
					if( pDMPartRef->m_pDMPart->m_bPlayModeFlags == DMUS_PLAYMODE_FIXED )
					{
						pTmpDMNote->m_wMusicValue = pDMPartRef->DMNoteToMIDIValue( pTmpDMNote, DMUS_PLAYMODE_ALWAYSPLAY );
						ASSERT( (pTmpDMNote->m_wMusicValue >= 0) && (pTmpDMNote->m_wMusicValue < 128) );
					}
				}

				// Set the note's variation flags
				pTmpDMNote->m_dwVariation = MapVariations( pTmpDMNote->m_dwVariation, dwVariationMap );

				if( !fChanged )
				{
					fChanged = TRUE;
					PreChangePartRef( pDMPartRef );
				}

				// Update the note's MIDIValue or MusicValue
				pDMPartRef->UpdateNoteAfterMove( pTmpDMNote );

				// Finally, actually inset the note
				pDMPartRef->m_pDMPart->InsertNoteInAscendingOrder( pTmpDMNote );
			}
			else
			{
				pTmpDMNote->SetNext(NULL);
				delete pTmpDMNote;
			}
			if( lDataSize >= (signed)dwNoteSize )
			{
				hr = pIStream->Read( &dmNote, dwNoteSize, &ulRead);
				if( ulRead != dwNoteSize)
				{
					hr = S_FALSE;
				}
			}
		}
		dmNote.SetNext(NULL); // This should be NULL, but just to be sure..
	}

	if ( SUCCEEDED(hr) && fChanged && fUpdatePatternEditorIfNeeded )
	{
		// update the corresponding DirectMusicPart's parameters
		UpdatePartParamsAfterChange( pDMPartRef->m_pDMPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_EDIT_PASTE );
		
		// updated m_SelectedPropNote
		RefreshPartDisplay( pDMPartRef->m_pDMPart, dwVariations, FALSE, FALSE );
		ComputeSelectedPropNote( pDMPartRef->m_pDMPart, dwVariations );
		RefreshPropertyPage( pDMPartRef->m_pDMPart );
		hr = S_OK;
	}

	return hr;
}

HRESULT CMIDIMgr::ImportEventList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, PianoRollClipboardData *pPRCD, IStream* pIStream, MUSIC_TIME mtGrid, BOOL &fChanged )
{
 	ASSERT( pDMPartRef != NULL );
	if( pDMPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	HRESULT hr = E_FAIL;

	// Create RIFFStream
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		while( pIRiffStream->Descend( &ckMain, 0, 0 ) == 0 )
		{
			switch( ckMain.ckid )
			{
			case FOURCC_START_END:
				/*
				ASSERT( ckMain.cksize >= sizeof(long) * 2 );
				if( ckMain.cksize >= sizeof(long) * 2 )
				{
					long nStart, nEnd;
					hr = pIStream->Read( &nStart, sizeof(long), NULL );
					if( SUCCEEDED(hr) )
					{
						hr = pIStream->Read( &nEnd, sizeof(long), NULL );

						lGridSpan = nEnd - nStart;

						if( !lGridSpan )
						{
							lGridSpan = 1;
						}
					}
				}
				*/
				break;

			case DMUS_FOURCC_NOTE_CHUNK:
				// Tell ImportNoteList not to update the pattern editor (FALSE parameter)
				hr = ImportNoteList( pDMPartRef, dwVariations, pPRCD, pIStream, mtGrid, ckMain.cksize, fChanged, FALSE );
				break;

			case DMUS_FOURCC_CURVE_CHUNK:
				hr = ImportCurveList( pDMPartRef, dwVariations, pPRCD, pIStream, mtGrid, ckMain.cksize, fChanged );
				break;

			case DMUS_FOURCC_MARKER_CHUNK:
				hr = ImportMarkerList( pDMPartRef, dwVariations, pPRCD, pIStream, mtGrid, ckMain.cksize, fChanged );
				break;
			}

			pIRiffStream->Ascend( &ckMain, 0 );
		}
		pIRiffStream->Release();
	}

	// If anything changed, update the pattern editor here, rather than each of ImportNoteList, ImportCurveList, and ImportMarkerList
	if( fChanged )
	{
		// update the corresponding DirectMusicPart's parameters
		UpdatePartParamsAfterChange( pDMPartRef->m_pDMPart );

		// Update the list of curve strips, in case we added a new curve type.
		RefreshCurveStripStateLists( pDMPartRef->m_pDMPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_UNDO_PASTE_EVENTS );
		
		// update m_SelectedPropNote
		RefreshPartDisplay( pDMPartRef->m_pDMPart, dwVariations, TRUE, TRUE );
		ComputeSelectedPropNote( pDMPartRef->m_pDMPart, dwVariations );
		RefreshPropertyPage( pDMPartRef->m_pDMPart );
	}

	return hr;
}

HRESULT CMIDIMgr::ImportSeqEventList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, IStream* pIStream, MUSIC_TIME mtGrid, BOOL fChanged)
{
 	ASSERT( pDMPartRef != NULL );
	if( pDMPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Create RIFFStream
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
	   MMCKINFO ckMain;
		ckMain.ckid = DMUS_FOURCC_SEQ_TRACK;
		if ( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDCHUNK ) == 0 )
		{
			MMCKINFO ck;
			while( pIRiffStream->Descend( &ck, 0, 0 ) == 0 )
			{
				switch( ck.ckid )
				{
				case DMUS_FOURCC_SEQ_LIST:
					hr = ImportSeqNoteChunk( pDMPartRef, dwVariations, pIStream, mtGrid, ck.cksize, fChanged );
					break;

				case DMUS_FOURCC_CURVE_LIST:
					hr = ImportSeqCurveChunk( pDMPartRef, dwVariations, pIStream, mtGrid, ck.cksize, fChanged );
					break;
				// Markers don't exist in sequence tracks
				}

				pIRiffStream->Ascend( &ck, 0 );
			}
			pIRiffStream->Ascend( &ckMain, 0 );
		}
		pIRiffStream->Release();
	}

	// If anything changed, update the pattern editor here, rather than each of ImportSeqNoteChunk and ImportSeqCurveChunk
	if( fChanged )
	{
		// update the corresponding DirectMusicPart's parameters
		UpdatePartParamsAfterChange( pDMPartRef->m_pDMPart );

		// Update the list of curve strips, in case we added a new curve type.
		RefreshCurveStripStateLists( pDMPartRef->m_pDMPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_UNDO_PASTE_SEQ );
		
		// update m_SelectedPropNote
		RefreshPartDisplay( pDMPartRef->m_pDMPart, dwVariations, TRUE, FALSE );
		ComputeSelectedPropNote( pDMPartRef->m_pDMPart, dwVariations );
		RefreshPropertyPage( pDMPartRef->m_pDMPart );
	}

	return hr;
}

HRESULT CMIDIMgr::ImportSeqCurveChunk( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged )
{
	// Validate parameters
	ASSERT( pDMPartRef != NULL );
	if( pDMPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	// Read in the size of a curve
	DWORD dwCurveSize;
	ULONG ulRead;
	HRESULT hr = pIStream->Read( &dwCurveSize, sizeof(long), &ulRead);
	if( ulRead != sizeof(long) || (sizeof(DMUS_IO_CURVE_ITEM) != dwCurveSize) )
	{
		return E_FAIL;
	}

	// Subtract off the data just read
	lDataSize -= ulRead; 

	// Insert the new curves into our part
	{
		// Prepare DMUS_IO_CURVE_ITEM structure
		while( (lDataSize >= (signed)dwCurveSize) && (hr == S_OK) )
		{
			DMUS_IO_CURVE_ITEM iDMStyleCurve;
			memset( &iDMStyleCurve, 0, dwCurveSize );

			ULONG ulRead;
			hr = pIStream->Read( &iDMStyleCurve, dwCurveSize, &ulRead);
			if( FAILED(hr) || ulRead != dwCurveSize)
			{
				hr = E_FAIL;
				break;
			}

			// Subtract what was just read
			lDataSize -= dwCurveSize;

			// Copy the  DMUS_IO_CURVE_ITEM to a CDirectMusicStyleCurve
			CDirectMusicStyleCurve *pTmpDMCurve = new CDirectMusicStyleCurve( &iDMStyleCurve );
			pTmpDMCurve->m_dwVariation = dwVariations;

			// Convert from clocks to a grid value
			MUSIC_TIME mtTmpGrid = CLOCKS_TO_GRID( iDMStyleCurve.mtStart, pDMPartRef->m_pDMPart );
			// Find out how much time is left over
			pTmpDMCurve->m_nTimeOffset += iDMStyleCurve.mtStart - GRID_TO_CLOCKS( mtTmpGrid, pDMPartRef->m_pDMPart );
			// Store the grid value
			pTmpDMCurve->m_mtGridStart = mtGrid + mtTmpGrid;

			// Only insert the curve if it will start before the end of time.
			if ( pTmpDMCurve->m_mtGridStart < pDMPartRef->m_pDMPart->GetGridLength() )
			{
				// Select the curve
				pTmpDMCurve->m_fSelected = TRUE;

				// Finally, actually insert the curve
				pDMPartRef->m_pDMPart->InsertCurveInAscendingOrder( pTmpDMCurve );

				fChanged = TRUE;
			}
			else
			{
				pTmpDMCurve->SetNext(NULL);
				delete pTmpDMCurve;
			}
		}
	}

	if( SUCCEEDED(hr) )
	{
		hr = S_OK;
	}

	return hr;
}


HRESULT CMIDIMgr::ImportSeqNoteChunk( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged )
{
	// Validate parameters
	ASSERT( pDMPartRef != NULL );
	if( pDMPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	// Read in the size of a note
	DWORD dwNoteSize;
	ULONG ulRead;
	HRESULT hr = pIStream->Read( &dwNoteSize, sizeof(DWORD), &ulRead);
	if( ulRead != sizeof(DWORD) || (sizeof(DMUS_IO_SEQ_ITEM) != dwNoteSize) )
	{
		return E_FAIL;
	}

	// Subtract off the data just read
	lDataSize -= ulRead; 

	// Insert the new notes into our part
	{
		DMUS_IO_SEQ_ITEM iSeqNote;
		while( (lDataSize >= (signed)dwNoteSize) && (hr == S_OK) )
		{
			// Read the note
			ULONG ulRead;
			hr = pIStream->Read( &iSeqNote, dwNoteSize, &ulRead);
			if( FAILED(hr) || (ulRead != dwNoteSize) )
			{
				hr = E_FAIL;
				break;
			}

			// Subtract what was just read
			lDataSize -= dwNoteSize;

			// Insert the note into the partref
			pDMPartRef->InsertSeqItem( &iSeqNote, fChanged, dwVariations, mtGrid );
		}
	}

	if( SUCCEEDED(hr)  )
	{
		hr = S_OK;
	}

	return hr;
}

HRESULT	CMIDIMgr::SaveSelectedEvents( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lStart, long lEnd, IStream* pIStream )
{
 	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	// Compute the earliest and latest selected grid
	long lStartGrid, lEndGrid;
	lStartGrid = CLOCKS_TO_GRID( lStart, pPartRef->m_pDMPart );
	lEndGrid = CLOCKS_TO_GRID( lEnd, pPartRef->m_pDMPart );

	IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	HRESULT hr = E_FAIL;

	// Create RIFFStream
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.ckid = FOURCC_START_END;
		if( pIRiffStream->CreateChunk( &ckMain, 0 ) == 0
		&&	SUCCEEDED( SaveSelectedTime( lStartGrid, lEndGrid, pIStream) )
		&&	pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
		{
			ckMain.ckid = DMUS_FOURCC_NOTE_CHUNK;
			if( pIRiffStream->CreateChunk( &ckMain, 0 ) == 0
			&&	SUCCEEDED( SaveSelectedNoteList( pPartRef, dwVariations, lStartGrid, pIStream) )
			&&	pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				ckMain.ckid = DMUS_FOURCC_CURVE_CHUNK;
				if( pIRiffStream->CreateChunk( &ckMain, 0 ) == 0
				&&	SUCCEEDED( SaveSelectedCurveList( pPartRef->m_pDMPart, dwVariations, lStartGrid, pIStream) )
				&&	pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
				{
					ckMain.ckid = DMUS_FOURCC_MARKER_CHUNK;
					if( pIRiffStream->CreateChunk( &ckMain, 0 ) == 0
					&&	SUCCEEDED( SaveSelectedMarkerList( pPartRef->m_pDMPart, dwVariations, lStartGrid, pIStream) )
					&&	pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
					{
						hr = S_OK;
					}
				}
			}
		}
		pIRiffStream->Release();
	}

	return hr;
}

HRESULT CMIDIMgr::SaveSelectedTime( long lGridStart, long lGridEnd, IStream *pIStream )
{
	HRESULT hr = S_OK;
	DWORD dwBytesWritten;

	// Save selection start time
	hr = pIStream->Write( &lGridStart, sizeof(long), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(long) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save selection end time
	hr = pIStream->Write( &lGridEnd, sizeof(long), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(long) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	return hr;
}

HRESULT	CMIDIMgr::SaveSelectedMarkerList( CDirectMusicPart* pPart, DWORD dwVariations, long lGridStart, IStream* pIStream )
{
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return E_INVALIDARG;
	}

	// Save size of DMUS_IO_STYLEMARKER structure
	DWORD dwBytesWritten, dwStructSize = sizeof(DMUS_IO_STYLEMARKER);
	HRESULT hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		return E_FAIL;
	}

	// Now save all the markers
	CDirectMusicStyleMarker* pDMMarker = pPart->m_lstMarkers.GetHead();
	for( ;  pDMMarker ;  pDMMarker = reinterpret_cast<CDirectMusicStyleMarker*>(pDMMarker->GetNext()) )
	{
		if( (pDMMarker->m_fSelected == TRUE) &&
			MARKER_AND_VARIATION(pDMMarker, dwVariations) )
		{
			CDirectMusicStyleMarker marker = *pDMMarker;
			marker.m_mtGridStart -= lGridStart;
			marker.m_dwEnterVariation &= dwVariations;
			marker.m_dwEnterChordVariation &= dwVariations;
			marker.m_dwExitVariation &= dwVariations;
			marker.m_dwExitChordVariation &= dwVariations;
			if( FAILED( marker.Write( pIStream ) ) )
			{
				return E_FAIL;
			}
		}
	}

	return hr;
}

HRESULT	CMIDIMgr::SaveSelectedCurveList( CDirectMusicPart* pPart, DWORD dwVariations, long lGridStart, IStream* pIStream )
{
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;
	CDirectMusicEventItem* pDMEvent;
	CDirectMusicStyleCurve* pDMCurve;
	DWORD dwBytesWritten;
	DWORD dwStructSize;
	DMUS_IO_STYLECURVE oDMStyleCurve;

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
	pDMEvent = pPart->m_lstCurves.GetHead();
	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		if( (pDMEvent->m_fSelected == TRUE) &&
			(pDMEvent->m_dwVariation & dwVariations) )
		{
			pDMCurve = (CDirectMusicStyleCurve *)pDMEvent;

			// Prepare DMUS_IO_STYLECURVE structure
			memset( &oDMStyleCurve, 0, sizeof(DMUS_IO_STYLECURVE) );

			oDMStyleCurve.mtGridStart = pDMCurve->m_mtGridStart - lGridStart;
			oDMStyleCurve.dwVariation = pDMCurve->m_dwVariation & dwVariations;
			oDMStyleCurve.nTimeOffset = pDMCurve->m_nTimeOffset;	

			oDMStyleCurve.mtDuration = pDMCurve->m_mtDuration;
			oDMStyleCurve.mtResetDuration = pDMCurve->m_mtResetDuration;
			oDMStyleCurve.nStartValue = pDMCurve->m_nStartValue;	
			oDMStyleCurve.nEndValue = pDMCurve->m_nEndValue;	
			oDMStyleCurve.nResetValue = pDMCurve->m_nResetValue;	
			oDMStyleCurve.bEventType = pDMCurve->m_bEventType;	
			oDMStyleCurve.bCurveShape = pDMCurve->m_bCurveShape;	
			oDMStyleCurve.bCCData = pDMCurve->m_bCCData;	
			oDMStyleCurve.bFlags = pDMCurve->m_bFlags;	
			oDMStyleCurve.wParamType = MEMORY_TO_FILE_WPARAMTYPE( pDMCurve->m_wParamType );
			oDMStyleCurve.wMergeIndex = pDMCurve->m_wMergeIndex;

			// Write DMUS_IO_STYLECURVE structure
			hr = pIStream->Write( &oDMStyleCurve, sizeof(DMUS_IO_STYLECURVE), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_STYLECURVE) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}
	}

ON_ERROR:
	return hr;
}

HRESULT	CMIDIMgr::SaveSelectedNoteList( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IStream* pIStream )
{
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	// Save size of CDirectMusicStyleNote
	DWORD dwBytesWritten;
	DWORD dwStructSize;
	dwStructSize = sizeof(CDirectMusicStyleNote);
	hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		return E_FAIL;
	}

	CDirectMusicEventItem* pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );

	if( pEvent )
	{
		// write out the selected DMNotes to the stream
		while ((pEvent != NULL) && (hr == S_OK))
		{
			if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
			{
				CDirectMusicStyleNote dmNote((CDirectMusicStyleNote*)pEvent);
				dmNote.m_mtGridStart -= lGridStart;
				dmNote.m_dwVariation &= dwVariations;
				if ( dmNote.m_bPlayModeFlags == DMUS_PLAYMODE_NONE )
				{
					// If the part's playmode is DMUS_PLAYMODE_FIXED, convert the value from a note to a musicvalue
					ASSERT( pPartRef->m_pDMPart->m_bPlayModeFlags != DMUS_PLAYMODE_NONE );
					if (pPartRef->m_pDMPart->m_bPlayModeFlags == DMUS_PLAYMODE_FIXED)
					{
						ASSERT( (dmNote.m_wMusicValue >= 0) && (dmNote.m_wMusicValue < 128) );
						dmNote.m_bMIDIValue = (BYTE)dmNote.m_wMusicValue;
						dmNote.m_wMusicValue = pPartRef->DMNoteToMusicValue( &dmNote, DMUS_PLAYMODE_ALWAYSPLAY );
					}
				}
				dmNote.SetNext(NULL);// Need to do this, otherwise all notes after this one are deleted.
				hr = pIStream->Write( &dmNote, sizeof(CDirectMusicStyleNote), &dwBytesWritten);
				if (dwBytesWritten != sizeof(CDirectMusicStyleNote))
				{
					hr = E_UNEXPECTED;
				}
			}
			pEvent = pEvent->GetNext();
		}
	}
	else
	{
		// No notes were saved, since none are in these variations
		hr = S_FALSE;
	}
	return hr;
}

HRESULT	CMIDIMgr::SaveSelectedNoteListForSeqTrack( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IStream* pIStream )
{
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Create RIFFStream
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		MMCKINFO ckMain;
		ckMain.ckid = DMUS_FOURCC_SEQ_TRACK;
		if( pIRiffStream->CreateChunk( &ckMain, 0 ) == 0 )
		{
			hr = SaveSelectedSeqNoteChunk( pPartRef, dwVariations, lGridStart, pIRiffStream );
			pIRiffStream->Ascend( &ckMain, 0 );
		}
		pIRiffStream->Release();
	}

	return hr;
}

HRESULT	CMIDIMgr::SaveSelectedCurveListForSeqTrack( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IStream* pIStream )
{
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Create RIFFStream
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		MMCKINFO ckMain;
		ckMain.ckid = DMUS_FOURCC_SEQ_TRACK;
		if( pIRiffStream->CreateChunk( &ckMain, 0 ) == 0 )
		{
			hr = SaveSelectedSeqCurveChunk( pPartRef, dwVariations, lGridStart, pIRiffStream );
			pIRiffStream->Ascend( &ckMain, 0 );
		}
		pIRiffStream->Release();
	}

	return hr;
}

HRESULT	CMIDIMgr::SaveSelectedEventsForSeqTrack( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lStart, IStream* pIStream )
{
 	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	// Compute the earliest and latest selected grid
	long lStartGrid;
	lStartGrid = CLOCKS_TO_GRID( lStart, pPartRef->m_pDMPart );

	IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	HRESULT hr = E_FAIL;

	// Create RIFFStream
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.ckid = DMUS_FOURCC_SEQ_TRACK;
		if( pIRiffStream->CreateChunk( &ckMain, 0 ) == 0 )
		{
			if( SUCCEEDED( SaveSelectedSeqNoteChunk( pPartRef, dwVariations, lStartGrid, pIRiffStream) ) )
			{
				if( SUCCEEDED( SaveSelectedSeqCurveChunk( pPartRef, dwVariations, lStartGrid, pIRiffStream) ) )
				{
					// Markers don't exist in sequence tracks
					hr = S_OK;
				}
			}
			pIRiffStream->Ascend( &ckMain, 0 );
		}
		pIRiffStream->Release();
	}

	return hr;
}

HRESULT CMIDIMgr::SaveSelectedSeqNoteChunk( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IDMUSProdRIFFStream* pIRiffStream )
{
	CDirectMusicEventItem* pEvent;
	pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );

	// If no notes, exit early
	if( !pEvent )
	{
		return S_FALSE;
	}

    MMCKINFO ckMain;
	ckMain.ckid = DMUS_FOURCC_SEQ_LIST;
	if( pIRiffStream->CreateChunk( &ckMain, 0 ) != 0 )
	{
		return E_FAIL;
	}

	IStream *pIStream = pIRiffStream->GetStream();

	DWORD dwBytesWritten;
	DWORD dwStructSize;
	dwStructSize = sizeof(DMUS_IO_SEQ_ITEM);
	HRESULT hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// It's fine if we don't have an event here, there may only be curves in the region of time we're copying
	//ASSERT( pEvent );
	{
		// write out the selected DMNotes to the stream
		while ((pEvent != NULL) && (hr == S_OK))
		{
			if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
			{
				DMUS_IO_SEQ_ITEM oSequenceItem;
				oSequenceItem.mtTime = GRID_TO_CLOCKS(pEvent->m_mtGridStart - lGridStart, pPartRef->m_pDMPart);
				oSequenceItem.mtDuration = ((CDirectMusicStyleNote*)pEvent)->m_mtDuration;
				oSequenceItem.dwPChannel = pPartRef->m_dwPChannel;
				oSequenceItem.nOffset = ((CDirectMusicStyleNote*)pEvent)->m_nTimeOffset;
				oSequenceItem.bStatus = MIDI_NOTEON;
				oSequenceItem.bByte1 = ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue;
				oSequenceItem.bByte2 = ((CDirectMusicStyleNote*)pEvent)->m_bVelocity;
				hr = pIStream->Write( &oSequenceItem, sizeof(DMUS_IO_SEQ_ITEM), &dwBytesWritten);
				if (dwBytesWritten != sizeof(DMUS_IO_SEQ_ITEM))
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
			}
			pEvent = pEvent->GetNext();
		}
	}

	pIRiffStream->Ascend( &ckMain, 0 );

ON_ERROR:
	pIStream->Release();
	return hr;
}

HRESULT CMIDIMgr::SaveSelectedSeqCurveChunk( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IDMUSProdRIFFStream* pIRiffStream )
{
	CDirectMusicEventItem* pEvent;
	pEvent = pPartRef->m_pDMPart->GetFirstCurve( dwVariations );

	// If no curves, exit early
	if( !pEvent )
	{
		return S_FALSE;
	}

    MMCKINFO ckMain;
	ckMain.ckid = DMUS_FOURCC_CURVE_LIST;
	if( pIRiffStream->CreateChunk( &ckMain, 0 ) != 0 )
	{
		return E_FAIL;
	}

	IStream *pIStream = pIRiffStream->GetStream();

	DWORD dwBytesWritten;
	DWORD dwStructSize;
	dwStructSize = sizeof(DMUS_IO_CURVE_ITEM);
	HRESULT hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr ) || dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// It's fine if we don't have an event here, there may only be notes in the region of time we're copying
	//ASSERT( pEvent );
	{
		// write out the selected DMNotes to the stream
		while ((pEvent != NULL) && (hr == S_OK))
		{
			if ( pEvent->m_fSelected && (pEvent->m_dwVariation & dwVariations) )
			{
				CDirectMusicStyleCurve *pDMCurve;
				pDMCurve = (CDirectMusicStyleCurve *)pEvent;

				// Prepare DMUS_IO_CURVE_ITEM structure
				DMUS_IO_CURVE_ITEM oDMStyleCurve;
				memset( &oDMStyleCurve, 0, sizeof(DMUS_IO_CURVE_ITEM) );

				oDMStyleCurve.mtStart = GRID_TO_CLOCKS(pDMCurve->m_mtGridStart - lGridStart, pPartRef->m_pDMPart);
				oDMStyleCurve.mtDuration = pDMCurve->m_mtDuration;
				oDMStyleCurve.mtResetDuration = pDMCurve->m_mtResetDuration;
				oDMStyleCurve.dwPChannel = pPartRef->m_dwPChannel;
				oDMStyleCurve.nOffset = pDMCurve->m_nTimeOffset;	

				oDMStyleCurve.nStartValue = pDMCurve->m_nStartValue;	
				oDMStyleCurve.nEndValue = pDMCurve->m_nEndValue;	
				oDMStyleCurve.nResetValue = pDMCurve->m_nResetValue;	
				oDMStyleCurve.bType = pDMCurve->m_bEventType;	
				oDMStyleCurve.bCurveShape = pDMCurve->m_bCurveShape;	
				oDMStyleCurve.bCCData = pDMCurve->m_bCCData;	
				oDMStyleCurve.bFlags = pDMCurve->m_bFlags;	
				oDMStyleCurve.wParamType = MEMORY_TO_FILE_WPARAMTYPE( pDMCurve->m_wParamType );
				oDMStyleCurve.wMergeIndex = pDMCurve->m_wMergeIndex;	

				// Write DMUS_IO_CURVE_ITEM structure
				hr = pIStream->Write( &oDMStyleCurve, sizeof(DMUS_IO_CURVE_ITEM), &dwBytesWritten );
				if( FAILED( hr ) ||  dwBytesWritten != sizeof(DMUS_IO_CURVE_ITEM) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
			}
			pEvent = pEvent->GetNext();
		}
	}

	pIRiffStream->Ascend( &ckMain, 0 );

ON_ERROR:
	pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::SaveSelectedEventsInAllSelectedStripsForMidi

HRESULT CMIDIMgr::SaveSelectedEventsInAllSelectedStripsForMidi( long lStartGrid, IStream* pIStream )
{
	// Count the number of sequence strips in this MIDI file
	HRESULT hr = S_FALSE;
	WORD wNumMIDITracks = 0;
	IDMUSProdStrip *pStrip;
	DWORD dwEnum = 0;
	// Iterate through all the strips
	while( SUCCEEDED( m_pTimeline->EnumStrip( dwEnum, &pStrip ) ) )
	{
		VARIANT varGutter;
		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pStrip, STP_GUTTER_SELECTED, &varGutter ) )
		&&	(varGutter.vt == VT_BOOL) && ( V_BOOL(&varGutter) == TRUE ) )
		{
			// Get the strip's CLSID
			CLSID clsid;
			VARIANT  varClsid;
			varClsid.vt = VT_BYREF;
			V_BYREF(&varClsid) = &clsid;

			if( SUCCEEDED ( pStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &varClsid ) )
			&&	::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
			{
				// This strip is a PianoRollStrip - increment number of tracks
				wNumMIDITracks++;
			}
		}
		pStrip->Release();
		dwEnum++;
	}

	wNumMIDITracks++;

	// write header
	hr = WriteSMFHeader(pIStream, wNumMIDITracks);
	if(FAILED(hr))
	{
		return hr;
	}

	// Empty Tempo map track
	static const BYTE abControlTrack[12] = {
		0x4d, 0x54, 0x72, 0x6b, // MTrk
		0x00, 0x00, 0x00, 0x04, // Length
		0x00, 0xFF, 0x2F, 0x00 }; // Track end marker

	pIStream->Write( abControlTrack, 12, NULL );

	// write tracks
	dwEnum = 0;
	// Iterate through all the strips
	while( SUCCEEDED( m_pTimeline->EnumStrip( dwEnum, &pStrip ) ) )
	{
		VARIANT varGutter;
		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pStrip, STP_GUTTER_SELECTED, &varGutter ) )
		&&	(varGutter.vt == VT_BOOL) && ( V_BOOL(&varGutter) == TRUE ) )
		{
			// Get the strip's CLSID
			CLSID clsid;
			VARIANT  varClsid;
			varClsid.vt = VT_BYREF;
			V_BYREF(&varClsid) = &clsid;

			if( SUCCEEDED ( pStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &varClsid ) )
			&&	::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
			{
				// This strip is a PianoRollStrip - Write the track
				gbChannel = static_cast<BYTE>(static_cast<CPianoRollStrip *>(pStrip)->m_pPartRef->m_dwPChannel & 0xF);
				static_cast<CPianoRollStrip *>(pStrip)->m_pPartRef->m_pDMPart->SaveSelectedEventsToMIDITrack( pIStream, lStartGrid, (static_cast<CPianoRollStrip *>(pStrip))->m_dwVariations );
			}
		}
		pStrip->Release();
		dwEnum++;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::SaveSelectedNotesForMidi

HRESULT CMIDIMgr::SaveSelectedNotesForMidi( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IStream* pIStream )
{
	ASSERT( pPartRef != NULL );
	if( pPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	CDirectMusicEventItem* pEvent;
	pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );

	// If the note list has anything in it, look for selected notes
	if( pEvent )
	{
		// list to store selected notes in
		CTypedPtrList<CPtrList, CSequenceItem*> lstSequences;

		// write out the selected DMNotes to the stream
		while (pEvent != NULL)
		{
			if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
			{
				CSequenceItem* pItemNew = new CSequenceItem;
				pItemNew->m_mtTime = GRID_TO_CLOCKS(pEvent->m_mtGridStart - lGridStart, pPartRef->m_pDMPart);
				pItemNew->m_mtDuration = ((CDirectMusicStyleNote*)pEvent)->m_mtDuration;
				pItemNew->m_nOffset = ((CDirectMusicStyleNote*)pEvent)->m_nTimeOffset;
				pItemNew->m_bStatus = MIDI_NOTEON;
				pItemNew->m_bByte1 = ((CDirectMusicStyleNote*)pEvent)->m_bMIDIValue;
				pItemNew->m_bByte2 = ((CDirectMusicStyleNote*)pEvent)->m_bVelocity;

				// Events should now be sorted, so we should not need to re-sort them
				lstSequences.AddTail( pItemNew );
			}
			pEvent = pEvent->GetNext();
		}

		if( lstSequences.IsEmpty() )
		{
			hr = S_FALSE;
		}
		else
		{
			hr = WriteMidiSequenceToStream(pIStream, pPartRef->m_dwPChannel, lstSequences);

			// cleanup
			while( !lstSequences.IsEmpty() )
			{
				delete lstSequences.RemoveHead();
			}
		}
	}
	else
	{
		hr = S_FALSE;
	}
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::ImportEventsFromMIDIFile

HRESULT CMIDIMgr::ImportEventsFromMIDIFile( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, IStream* pIStream, TIMELINE_PASTE_TYPE ptPasteType, MUSIC_TIME mtGrid )
{
	UNREFERENCED_PARAMETER(ptPasteType);
 	ASSERT( pDMPartRef != NULL );
	if( pDMPartRef == NULL )
	{
		return E_INVALIDARG;
	}

	DWORD dwLength;
	CTypedPtrList<CPtrList, CSequenceItem*> lstSequences;
	CTypedPtrList<CPtrList, CCurveItem*> lstCurves;
	HRESULT hr = ReadMidiSequenceFromStream( pIStream, lstSequences, lstCurves, 0, dwLength );

	BOOL fChanged = FALSE;

	if( SUCCEEDED( hr ) )
	{
		if( ptPasteType == TL_PASTE_OVERWRITE )
		{
			MUSIC_TIME mtLatest = 0;
			POSITION pos = lstSequences.GetHeadPosition();
			while( pos )
			{
				CSequenceItem *pItem = lstSequences.GetNext( pos );
				mtLatest = max( mtLatest, pItem->AbsTime() + pItem->m_mtDuration );
			}

			pos = lstCurves.GetHeadPosition();
			while( pos )
			{
				CCurveItem *pItem = lstCurves.GetNext( pos );
				mtLatest = max( mtLatest, pItem->AbsTime() + pItem->m_mtDuration );
			}

			long lGridSpan = CLOCKS_TO_GRID( mtLatest, pDMPartRef->m_pDMPart );

			// Delete all curves that start between mtGrid and mtGrid + dwGridsOccupied, inclusive
			CDirectMusicEventItem* pEvent = pDMPartRef->m_pDMPart->GetFirstCurve( dwVariations );
			while( !fChanged && pEvent )
			{
				if ( pEvent->m_mtGridStart >= mtGrid &&
					 pEvent->m_mtGridStart <= (mtGrid + lGridSpan) &&
					(pEvent->m_dwVariation & dwVariations) )
				{
					// Call PreChangePartRef since this is the first change
					fChanged = TRUE;
					PreChangePartRef( pDMPartRef );
					break;
				}
				pEvent = pEvent->GetNext();
			}
			// Need to look through the list again from the start, since we may be using a different m_pDMPart 
			pEvent = pDMPartRef->m_pDMPart->GetFirstCurve( dwVariations );
			while (pEvent != NULL)
			{
				if ( pEvent->m_mtGridStart >= mtGrid &&
					 pEvent->m_mtGridStart <= (mtGrid + lGridSpan) &&
					(pEvent->m_dwVariation & dwVariations) )
				{
					pEvent = DeleteEvent( pEvent, pDMPartRef->m_pDMPart->m_lstCurves, dwVariations );
				}
				else
				{
					pEvent = pEvent->GetNext();
				}
			}

			// Delete all notes that start between mtGrid and mtGrid + dwGridsOccupied, inclusive
			pEvent = pDMPartRef->m_pDMPart->GetFirstNote( dwVariations );
			while( !fChanged && pEvent )
			{
				if ( pEvent->m_mtGridStart >= mtGrid &&
					 pEvent->m_mtGridStart <= (mtGrid + lGridSpan) &&
					(pEvent->m_dwVariation & dwVariations) )
				{
					// Call PreChangePartRef since this is the first change
					fChanged = TRUE;
					PreChangePartRef( pDMPartRef );
					break;
				}
				pEvent = pEvent->GetNext();
			}
			// Need to look through the list again from the start, since we may be using a different m_pDMPart 
			pEvent = pDMPartRef->m_pDMPart->GetFirstNote( dwVariations );
			while (pEvent != NULL)
			{
				if ( pEvent->m_mtGridStart >= mtGrid &&
					 pEvent->m_mtGridStart <= (mtGrid + lGridSpan) &&
					(pEvent->m_dwVariation & dwVariations) )
				{
					pEvent = DeleteEvent( pEvent, pDMPartRef->m_pDMPart->m_lstNotes, dwVariations );
				}
				else
				{
					pEvent = pEvent->GetNext();
				}
			}

			ValidateActiveDMNote( pDMPartRef->m_pDMPart, dwVariations );
		}

		while( !lstSequences.IsEmpty() )
		{
			DMUS_IO_SEQ_ITEM iSeqNote;
			CSequenceItem *pItem = lstSequences.RemoveHead();
			pItem->CopyTo( iSeqNote );
			delete pItem;

			pDMPartRef->InsertSeqItem( &iSeqNote, fChanged, dwVariations, mtGrid );
		}

		while( !lstCurves.IsEmpty() )
		{
			CCurveItem *pCurveItem = lstCurves.RemoveHead();

			pDMPartRef->InsertCurveItem( pCurveItem, fChanged, dwVariations, mtGrid );
			delete pCurveItem;
		}

		if ( fChanged )
		{
			// update the corresponding DirectMusicPart's parameters
			UpdatePartParamsAfterChange( pDMPartRef->m_pDMPart );

			// Update the list of curve strips, in case we added a new curve type.
			RefreshCurveStripStateLists( pDMPartRef->m_pDMPart );

			// Let the object know about the changes
			UpdateOnDataChanged( IDS_UNDO_PASTE_MIDI );
			
			// updated m_SelectedPropNote
			RefreshPartDisplay( pDMPartRef->m_pDMPart, dwVariations, TRUE, FALSE );
			ComputeSelectedPropNote( pDMPartRef->m_pDMPart, dwVariations );
			RefreshPropertyPage( pDMPartRef->m_pDMPart );
			hr = S_OK;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::ImportMIDIFileToMultipleStrips

HRESULT CMIDIMgr::ImportMIDIFileToMultipleStrips( IStream* pStream )
{
	if(pStream == NULL)
	{
		return E_POINTER;
	}

	long lStartTime;
	if (FAILED(m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lStartTime )))
	{
		return E_FAIL;
	}

	// Save the location of the start of the file
	DWORD dwStartOfFile = StreamTell( pStream );

	// Determined the length of the MIDI clip
	CTypedPtrList<CPtrList, CSequenceItem*> lstSequences;
	CTypedPtrList<CPtrList, CCurveItem*> lstCurves;
	DWORD dwLength;
	HRESULT hr = ReadMidiSequenceFromStream(pStream, lstSequences, lstCurves, 0, dwLength);
	while( !lstSequences.IsEmpty() )
	{
		delete lstSequences.RemoveHead();
	}
	while( !lstCurves.IsEmpty() )
	{
		delete lstCurves.RemoveHead();
	}
	ASSERT(hr == S_OK);
	if( FAILED(hr) )
	{
		return hr;
	}

	POSITION pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		m_pDMPattern->m_lstPartRefs.GetNext( pos )->m_fChanged = FALSE;
	}

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( SUCCEEDED( m_pTimeline->GetPasteType( &tlPasteType ) )
	&&	(tlPasteType == TL_PASTE_OVERWRITE) )
	{
		// Iterate through all the strips
		DWORD dwStripEnum = 0;
		IDMUSProdStrip *pStrip;
		while( SUCCEEDED( m_pTimeline->EnumStrip( dwStripEnum, &pStrip ) ) )
		{
			VARIANT varGutter;
			if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pStrip, STP_GUTTER_SELECTED, &varGutter ) )
			&&	(varGutter.vt == VT_BOOL) && ( V_BOOL(&varGutter) == TRUE ) )
			{
				// Get the strip's CLSID
				CLSID clsid;
				VARIANT  varClsid;
				varClsid.vt = VT_BYREF;
				V_BYREF(&varClsid) = &clsid;

				if( SUCCEEDED ( pStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &varClsid ) )
				&&	::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
				{
					// This strip is a PianoRollStrip - delete the items that will be overwritten
					CDirectMusicPartRef *pPartRef = static_cast<CPianoRollStrip *>(pStrip)->m_pPartRef;

					// Compute the grids to delete
					long lStartGrid, lEndGrid;
					lStartGrid = CLOCKS_TO_GRID( lStartTime, pPartRef->m_pDMPart );
					lEndGrid = CLOCKS_TO_GRID( GRID_TO_CLOCKS(lStartGrid, pPartRef->m_pDMPart) + dwLength, pPartRef->m_pDMPart );

					DeleteNotesBetweenBoundaries( pPartRef, static_cast<CPianoRollStrip *>(pStrip)->m_dwVariations, lStartGrid, lEndGrid, pPartRef->m_fChanged );
					DeleteCurvesBetweenBoundaries( pPartRef, static_cast<CPianoRollStrip *>(pStrip)->m_dwVariations, lStartGrid, lEndGrid, pPartRef->m_fChanged );
				}
			}
			pStrip->Release();
			dwStripEnum++;
		}
	}

	// Jump back to the start of the file
	StreamSeek( pStream, dwStartOfFile, STREAM_SEEK_SET );

    DWORD dwID, dwSize;
    if( ( S_OK != pStream->Read( &dwID, sizeof( FOURCC ), NULL ) ) ||
		!GetMLong( pStream, dwSize ) )
    {
		return E_FAIL;
    }
// check for RIFF MIDI files
    if( dwID == mmioFOURCC( 'R', 'I', 'F', 'F' ) )
    {
		StreamSeek( pStream, 12, STREAM_SEEK_CUR );
		if( ( S_OK != pStream->Read( &dwID, sizeof( FOURCC ), NULL ) ) ||
			!GetMLong( pStream, dwSize ) )
		{
			return E_FAIL;
		}
    }
// check for normal MIDI files
	if( dwID != mmioFOURCC( 'M', 'T', 'h', 'd' ) )
	{
		return E_FAIL;
	}

    short nFormat, nNumTracks;
    GetMShort( pStream, nFormat );
    GetMShort( pStream, nNumTracks );
    GetMShort( pStream, gnPPQN );
    if( dwSize > 6 )
    {
		StreamSeek( pStream, dwSize - 6, STREAM_SEEK_CUR );
    }

	DWORD dwStreamPos = StreamTell( pStream );

	// Check for a MIDI track that only includes Meta-Events, and ignore it if found
	if( nFormat == 1 )
	{
		BOOL fOnly0xFFMIDIEvents = TRUE;

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

		BYTE bRunningStatus = 0;

		while( dwSize > 0 )
		{
			DWORD dwOffsetTime;
			dwSize -= GetVarLength( pStream, dwOffsetTime );
			BYTE b;

			if( FAILED( pStream->Read( &b, 1, NULL ) ) )
			{
				return E_FAIL;
			}

		    DWORD dwBytes;
			if( b < 0x80 )
			{
				StreamSeek( pStream, -1, STREAM_SEEK_CUR );
				b = bRunningStatus;
				dwBytes = 0;
			}
			else
			{
				dwBytes = 1;
			}

			if( b < 0xf0 )
			{
				bRunningStatus = b;

				switch( b & 0xf0 )
				{
				case MIDI_CCHANGE:
				case MIDI_PTOUCH:
				case MIDI_PBEND:
				case MIDI_NOTEOFF:
				case MIDI_NOTEON:
					if( FAILED( pStream->Read( &b, 1, NULL ) ) )
					{
						return E_FAIL;
					}
					++dwBytes;
					/*if( FAILED( pStream->Read( &b, 1, NULL ) ) )
					{
						return E_FAIL;
					}
					++dwBytes;
					break;*/
				case MIDI_PCHANGE:
				case MIDI_MTOUCH:
					if(FAILED(pStream->Read(&b, 1, NULL)))
					{
						return E_FAIL;
					}
					++dwBytes;
					fOnly0xFFMIDIEvents = FALSE;
					break;
				default:
					// this should NOT be possible - unknown midi note event type
					ASSERT(FALSE);
					fOnly0xFFMIDIEvents = FALSE;
					break;
				}
			}
			else
			{
				DWORD dwLen;
				switch( b )
				{
				case 0xff:
					if( FAILED( pStream->Read( &b, 1, NULL ) ) )
					{
						return E_FAIL;
					}
					++dwBytes;
					dwBytes += GetVarLength( pStream, dwLen );
					StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
					dwBytes += dwLen;
					break;
				case 0xf0:
				case 0xf7:
					dwBytes += GetVarLength( pStream, dwLen );
					StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
					dwBytes += dwLen;
					fOnly0xFFMIDIEvents = FALSE;
					break;
				default:
					TRACE( "Unknown midi event type: 0x%x", b );
					fOnly0xFFMIDIEvents = FALSE;
					break;
				}
			}

			// Subtrack off the number of bytes read
			dwSize -= dwBytes;
		}

		if( !fOnly0xFFMIDIEvents )
		{
			// Seek back to the begining, since there are some useful MIDI events
			// in ths track
			StreamSeek( pStream, dwStreamPos, STREAM_SEEK_SET );
		}
	}

	// Iterate through all the strips
	DWORD dwStripEnum = 0;
	IDMUSProdStrip *pStrip;
	while( SUCCEEDED( m_pTimeline->EnumStrip( dwStripEnum, &pStrip ) ) )
	{
		VARIANT varGutter;
		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pStrip, STP_GUTTER_SELECTED, &varGutter ) )
		&&	(varGutter.vt == VT_BOOL) && ( V_BOOL(&varGutter) == TRUE ) )
		{
			// Get the strip's CLSID
			CLSID clsid;
			VARIANT  varClsid;
			varClsid.vt = VT_BYREF;
			V_BYREF(&varClsid) = &clsid;

			if( SUCCEEDED ( pStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &varClsid ) )
			&&	::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
			{
				// This strip is a PianoRollStrip - read the track
				CDirectMusicPartRef *pPartRef = static_cast<CPianoRollStrip *>(pStrip)->m_pPartRef;

				// Compute the start grid value
				long lStartGrid = CLOCKS_TO_GRID( lStartTime, pPartRef->m_pDMPart );

				// Import the events
				pPartRef->ImportEventsFromMIDITrack( pStream, lStartGrid, (static_cast<CPianoRollStrip *>(pStrip))->m_dwVariations );
			}
		}
		pStrip->Release();
		dwStripEnum++;
	}

	// Iterate through all PartRefs and update them, as necessary
	
	pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		CDirectMusicPartRef *pDMPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );
		if( pDMPartRef->m_fChanged )
		{
			// update the corresponding DirectMusicPart's parameters
			UpdatePartParamsAfterChange( pDMPartRef->m_pDMPart );

			// Update the list of curve strips, in case we added a new curve type.
			RefreshCurveStripStateLists( pDMPartRef->m_pDMPart );

			// Let the object know about the changes
			UpdateOnDataChanged( IDS_UNDO_PASTE_MIDI );
			
			// update the display
			RefreshPartDisplay( pDMPartRef->m_pDMPart, ALL_VARIATIONS, TRUE, FALSE );
		}
	}

	return S_OK;
}


// Part support routines

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::FindPartByGUID

CDirectMusicPart* CMIDIMgr::FindPartByGUID( GUID guidPartID )
{
	CDirectMusicPart* pThePart = NULL;
	CDirectMusicPart* pPart;

	POSITION pos = m_lstStyleParts.GetHeadPosition();
	while( pos )
	{
		pPart = m_lstStyleParts.GetNext( pos );
		
		if( ::IsEqualGUID( pPart->m_guidPartID, guidPartID ) )
		{
			pThePart = pPart;
			break;
		}
	}

	return pThePart;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::FindPartByTrack

CDirectMusicPart* CMIDIMgr::FindPartByTrack( long nPart )
{
	ASSERT( (nPart < 256) && (nPart >= 0) );
	if (m_pDMPattern && (nPart < 256) && (nPart >= 0) )
	{
		return m_pDMPattern->FindPart( (unsigned char) nPart );
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::AllocPart

CDirectMusicPart* CMIDIMgr::AllocPart( void )
{
	CDirectMusicPart* pPart = new CDirectMusicPart( this );

	if( pPart )
	{
		// Add Part to Style's list of Parts
		m_lstStyleParts.AddTail( pPart );
	}

	return pPart;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::DeletePart

void CMIDIMgr::DeletePart( CDirectMusicPart* pPart )
{
	ASSERT( pPart != NULL );
	ASSERT( pPart->m_dwUseCount == 0 );

	// Remove Part from Style's Part list
	POSITION pos = m_lstStyleParts.Find( pPart );

	if( pos )
	{
		m_lstStyleParts.RemoveAt( pos );
	}

	delete pPart;
}

HRESULT	CMIDIMgr::UpdatePartParamsAfterChange( CDirectMusicPart* pPart )
{
	HRESULT hr = E_FAIL;

	ASSERT( m_pDMPattern != NULL );
	ASSERT( pPart != NULL );
	if ((m_pDMPattern != NULL) && (pPart != NULL))
	{
		// Update the m_dwVariationsHasNotes flags
		pPart->UpdateHasNotes();

		// BUGBUG: This may cause problems if it two PartRefs in two different
		// Patterns both reference the same part and use different Chords
		// for composing.
		// Update Inversion Boundaries
		if( pPart->m_bAutoInvert )
		{
			// Find first PartRef that uses this part
			CDirectMusicPartRef* pPartRef;
			POSITION pos;
			pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
			while( pos != NULL )
			{
				pPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );

				if( pPartRef->m_pDMPart == pPart )
				{
					// Update the Part's inversion boundaries
					pPartRef->SetInversionBoundaries();
					break;
				}
			}
		}
	}
	return hr;
}

/*
void CMIDIMgr::PropagateGUID( GUID oldGuid, GUID newGuid )
{
	// Update the piano roll strip(s) and note property page
	CPianoRollStrip* pPianoRollStrip;
	POSITION position;
	position = m_pPRSList.GetHeadPosition();
	while (position != NULL)
	{
		pPianoRollStrip = m_pPRSList.GetNext(position);
		if (IsEqualGUID(pPianoRollStrip->m_guid, oldGuid))
		{
			memcpy ( &pPianoRollStrip->m_guid, &newGuid, sizeof(GUID) );
		}
	}

	if (IsEqualGUID( oldGuid, m_NotePropData.m_Param.guid ))
	{
		memcpy( &m_NotePropData.m_Param.guid, &newGuid, sizeof(GUID));
		// We don't really need to update the property page, since it only uses
		// the GUID as a flag to tell if it should disable everything or not.
		// Since the guid in m_NotePropData was already non-zero, the refresh wouldn't
		// change anything anyways.
		// Commenting this out is a good thing, because the SetNote() call on the
		// property page can take around 0.720 seconds when inserting notes
		// from a MIDI keyboard on my PPro-200.
		//RefreshPropertyPage( newGuid );
	}

	if (IsEqualGUID( oldGuid, m_PianoRollData.guid ))
	{
		memcpy( &m_PianoRollData.guid, &newGuid, sizeof(GUID));
	}

}
*/

void CMIDIMgr::StopDisplayUpdate()
{
	m_fNoDisplayUpdate = TRUE;
}

void CMIDIMgr::StartDisplayUpdate()
{
	m_fNoDisplayUpdate = FALSE;
}

HRESULT CMIDIMgr::CreateDefaultPianoRollStrip( CDirectMusicPartRef* pPartRef )
{
	ASSERT( pPartRef );
	if( pPartRef == NULL )
	{
		return E_INVALIDARG;
	}
	ASSERT( pPartRef->m_pDMPart );
	if( pPartRef->m_pDMPart == NULL )
	{
		return E_INVALIDARG;
	}

	// Initialize the DMPart->m_dwVariationHasNotes flags
	pPartRef->m_pDMPart->UpdateHasNotes();

	CPianoRollStrip* pPianoRollStrip;
	pPianoRollStrip = CreatePianoRollStrip( pPartRef );
	ASSERT( pPianoRollStrip != NULL );
	if( pPianoRollStrip == NULL )
	{
		return E_FAIL;
	}

	// Make sure there is a curve strip for each type of Curve in this Part
	pPartRef->m_pDMPart->AddCurveTypesFromData();

	// 19279: Create curve strip states for all curves or empty curve strips in this part
	int i;
	for( i=0; i< 17; i++ )
	{
		int j;
		for( j=0; j < 8; j++)
		{
			if( pPartRef->m_pDMPart->m_bHasCurveTypes[i] & (1 << j) )
			{
				pPianoRollStrip->GetCurveStripState( BYTE(i * 8 + j), 0 );
			}
		}
	}

	// Add Piano Roll Strip to the Timeline
	if( m_pIDMTrack )
	{
		m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)pPianoRollStrip, CLSID_DirectMusicPatternTrack, m_dwGroupBits, PChannelToStripIndex( pPartRef->m_dwPChannel ) );
	}
	else
	{
		DWORD dwPosition;
		dwPosition = DeterminePositionForPianoRollStrip( pPartRef );
		m_pTimeline->InsertStripAtPos( (IDMUSProdStrip *)pPianoRollStrip, dwPosition );
		//m_pTimeline->AddStrip( (IDMUSProdStrip *)pPianoRollStrip );
	}

	// By default, make the strip open up in its minimized state
	ioPianoRollDesign ioPRD;

	// If loading a Melody generated pattern, try and persist the PianoRollStrip data
	bool fFoundIt = false;
	POSITION posMLSList = m_lstMelodyLoadStructures.GetHeadPosition();
	while( posMLSList )
	{
		MelodyLoadStructure *pTmpMelodyLoadStructure = m_lstMelodyLoadStructures.GetNext( posMLSList );
		if( pPianoRollStrip->m_pPartRef->m_dwPChannel == pTmpMelodyLoadStructure->dwPChannel )
		{
			ioPRD = pTmpMelodyLoadStructure->prdDesign;
			fFoundIt = true;
		}
	}

	if( !fFoundIt )
	{
		pPianoRollStrip->GetDesignInfo( &ioPRD );
		ioPRD.m_nStripView = (int) SV_MINIMIZED;

		// Get the style's notation setting if we're not a drum track
		if( ((pPartRef->m_dwPChannel & 0xF) != 9)
		||	(pPartRef->m_dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS) )
		{
			DWORD dwType = 0;
			IDMUSProdStyleInfo *pStyleInfo;
			if( m_pIStyleNode
			&&	SUCCEEDED( m_pIStyleNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pStyleInfo ) ) )
			{
				if( SUCCEEDED( pStyleInfo->GetNotationType( &dwType ) ) )
				{
					if( ioPRD.m_fHybridNotation != BOOL(dwType) )
					{
						ioPRD.m_fHybridNotation = dwType;
						ioPRD.m_dblVerticalZoom *= (dwType == 1) ? 1.6 : (1.0 / 1.6) ;
						// This is approximately correct
						if( dwType )
						{
							ioPRD.m_lVerticalScroll = MulDiv( ioPRD.m_lVerticalScroll, 38, 128 );
						}
						else
						{
							ioPRD.m_lVerticalScroll = MulDiv( ioPRD.m_lVerticalScroll, 128, 38 );
						}
					}
				}
				pStyleInfo->Release();
			}
		}
	}

	pPianoRollStrip->SetDesignInfo( &ioPRD );

	// Add Curve Strips to the Timeline
	pPianoRollStrip->AddCurveStrips( FALSE );
	return S_OK;

}

HRESULT CMIDIMgr::LoadPianoRollDesignData( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain,
										   CDirectMusicPartRef* pDMPartRef )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
	CPianoRollStrip* pPianoRollStrip = NULL;
	ioPianoRollDesign iPianoRollDesign;

 	// Prepare ioPianoRollDesign structure
	InitializeIOPianoRollDesign( &iPianoRollDesign );

	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_PIANOROLL_CHUNK:
			{
				dwSize = min( ck.cksize, sizeof( ioPianoRollDesign ) );
				hr = pIStream->Read( &iPianoRollDesign, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				pPianoRollStrip = CreatePianoRollStrip( pDMPartRef );
				if( pPianoRollStrip == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}
				break;
			}

			case DMUS_FOURCC_ALLCURVES_UI_CHUNK:
			{
				ioGlobalCurveStripState iGlobalCurveStripState;

				// Prepare ioGlobalCurveStripState structure
				memset( &iGlobalCurveStripState, 0, sizeof(ioGlobalCurveStripState) );
				
				dwSize = min( ck.cksize, sizeof( ioGlobalCurveStripState ) );
				hr = pIStream->Read( &iGlobalCurveStripState, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( pPianoRollStrip )
				{
					pPianoRollStrip->m_CurveStripView = (STRIPVIEW)iGlobalCurveStripState.m_nCurveStripView;
				}
				break;
			}

			case DMUS_FOURCC_CURVE_UI_CHUNK:
			{
				ioCurveStripState* pCurveStripState = new ioCurveStripState;
				if( pCurveStripState == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}

				// Initialize ioCurveStripState structure
				memset( pCurveStripState, 0, sizeof(ioCurveStripState) );

				dwSize = min( ck.cksize, sizeof( ioCurveStripState ) );
				hr = pIStream->Read( pCurveStripState, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					delete pCurveStripState;
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( pPianoRollStrip )
				{
					ASSERT( pCurveStripState->m_bCCType < 17 * 8 );

					// 19279: Only add the curve strip if the corresponding bit in m_bHasCurveTypes is set.
					// But, only do this if we actually loaded in m_bHasCurveTypes
					if( !pDMPartRef->m_pDMPart->m_fLoadedHasCurveTypes ||
						(pDMPartRef->m_pDMPart->m_bHasCurveTypes[ pCurveStripState->m_bCCType>>3 ] &
						 (1 << (pCurveStripState->m_bCCType & 0x07))) )
					{
						pPianoRollStrip->m_lstCurveStripStates.AddTail( pCurveStripState );
						pDMPartRef->m_pDMPart->m_bHasCurveTypes[ pCurveStripState->m_bCCType>>3 ] |= 1 << (pCurveStripState->m_bCCType & 0x07);
					}
					else
					{
						delete pCurveStripState;
					}
				}
				break;
			}
		}

        pIRiffStream->Ascend( &ck, 0 );
	}

	// This will happen when opening up old Producer files
	if( !pPianoRollStrip )
	{
		pPianoRollStrip = CreatePianoRollStrip( pDMPartRef );
		if( pPianoRollStrip == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto ON_ERROR;
		}
	}

	// Make sure there is a curve strip for each type of Curve in this Part
	pDMPartRef->m_pDMPart->AddCurveTypesFromData();

	// 19279: Create curve strip states for all curves or empty curve strips in this part
	int i;
	for( i=0; i< 17; i++ )
	{
		int j;
		for( j=0; j < 8; j++)
		{
			if( pDMPartRef->m_pDMPart->m_bHasCurveTypes[i] & (1 << j) )
			{
				pPianoRollStrip->GetCurveStripState( BYTE(i * 8 + j), 0 );
			}
		}
	}

	// Add Piano Roll Strip to the Timeline
	if( m_pIDMTrack )
	{
		m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)pPianoRollStrip, CLSID_DirectMusicPatternTrack, m_dwGroupBits, PChannelToStripIndex( pDMPartRef->m_dwPChannel ) );
	}
	else
	{
		DWORD dwPosition;
		dwPosition = DeterminePositionForPianoRollStrip( pDMPartRef );
		m_pTimeline->InsertStripAtPos( (IDMUSProdStrip *)pPianoRollStrip, dwPosition );
		//m_pTimeline->AddStrip( (IDMUSProdStrip *)pPianoRollStrip );
	}
		
	// Get the style's notation setting if we're not a drum track
	if( (pDMPartRef->m_dwPChannel & 0xF) != 9 )
	{
		DWORD dwType = 0;
		IDMUSProdStyleInfo *pStyleInfo;
		if( m_pIStyleNode
		&&	SUCCEEDED( m_pIStyleNode->QueryInterface( IID_IDMUSProdStyleInfo, (void**)&pStyleInfo ) ) )
		{
			if( SUCCEEDED( pStyleInfo->GetNotationType( &dwType ) ) )
			{
				if( iPianoRollDesign.m_fHybridNotation != BOOL(dwType) )
				{
					iPianoRollDesign.m_fHybridNotation = dwType;
					iPianoRollDesign.m_dblVerticalZoom *= (dwType == 1) ? 1.6 : (1.0 / 1.6) ;
					// This is approximately correct
					if( dwType )
					{
						iPianoRollDesign.m_lVerticalScroll = MulDiv( iPianoRollDesign.m_lVerticalScroll, 38, 128 );
					}
					else
					{
						iPianoRollDesign.m_lVerticalScroll = MulDiv( iPianoRollDesign.m_lVerticalScroll, 128, 38 );
					}
				}
			}
			pStyleInfo->Release();
		}
	}

	// Set Piano Roll state information
	pPianoRollStrip->SetDesignInfo( &iPianoRollDesign );

	// Add Curve Strips to the Timeline
	pPianoRollStrip->AddCurveStrips( TRUE );

ON_ERROR:
	if( hr != S_OK )
	{
		if( pPianoRollStrip )
		{
			POSITION pos = m_pPRSList.Find( pPianoRollStrip );
			if( pos )
			{
				m_pPRSList.RemoveAt( pos );
				pPianoRollStrip->Release();
			}
		}
	}

    pIStream->Release();
    return hr;
}

HRESULT CMIDIMgr::SavePianoRollDesignData( IDMUSProdRIFFStream* pIRiffStream, CDirectMusicPartRef* pDMPartRef )
{
	IStream* pIStream = NULL;
	HRESULT hr = S_OK;
	MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	ioPianoRollDesign oPianoRollDesign;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	// Remove all existing PianoRoll UI states from this PartRef, but only if we're in
	// the segment designer, and if we currently have piano roll strips
	if( m_pIDMTrack
	&&	!m_pPRSList.IsEmpty() )
	{
		while( !pDMPartRef->m_lstPianoRollUIStates.IsEmpty() )
		{
			PianoRollUIState* pPianoRollUIState = static_cast<PianoRollUIState*>( pDMPartRef->m_lstPianoRollUIStates.RemoveHead() );

			if( pPianoRollUIState->pPianoRollData )
			{
				GlobalFree( pPianoRollUIState->pPianoRollData );
			}
			delete pPianoRollUIState;
		}
	}

	POSITION position = m_pPRSList.GetHeadPosition();
	while( position != NULL )
	{
		CPianoRollStrip* pPianoRollStrip = m_pPRSList.GetNext(position);

		if( pPianoRollStrip->m_pPartRef == pDMPartRef )
		{
			// Write PianoRoll list header
			ckMain.fccType = DMUS_FOURCC_PIANOROLL_LIST;
			if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Save the current position
			const DWORD dwPosition = StreamTell( pIStream );

			// Write PianoRoll chunk header
			ck.ckid = DMUS_FOURCC_PIANOROLL_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Prepare ioPianoRollDesign structure
			InitializeIOPianoRollDesign( &oPianoRollDesign );
			pPianoRollStrip->GetDesignInfo( &oPianoRollDesign );

			// Write PianoRoll chunk data
			hr = pIStream->Write( &oPianoRollDesign, sizeof(ioPianoRollDesign), &dwBytesWritten);
			if( FAILED( hr ) || dwBytesWritten != sizeof(ioPianoRollDesign) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Write UI state info for this Piano Roll's Curve Strips
			pPianoRollStrip->SaveCurveStripStateData( pIRiffStream );

			// Save the ending position
			const DWORD dwEndPosition = StreamTell( pIStream );

			// Copy the PianoRoll UI states into this PartRef
			if( m_pIDMTrack )
			{
				// Try and allocate the stream
				IStream *pTmpStream = NULL;
				if( SUCCEEDED( m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pTmpStream ) ) )
				{
					IDMUSProdRIFFStream* pITmpRiffStream;
					if( SUCCEEDED( AllocRIFFStream( pTmpStream, &pITmpRiffStream ) ) )
					{
						// Write PianoRoll chunk data
						ck.ckid = DMUS_FOURCC_PIANOROLL_CHUNK;
						if( pITmpRiffStream->CreateChunk( &ck, 0 ) == 0
						&&	SUCCEEDED( pTmpStream->Write( &oPianoRollDesign, sizeof(ioPianoRollDesign), &dwBytesWritten) )
						&&	dwBytesWritten == sizeof(ioPianoRollDesign)
						&&	pITmpRiffStream->Ascend(&ck, 0) == 0 )
						{
							// Write UI state info for this Piano Roll's Curve Strips
							pPianoRollStrip->SaveCurveStripStateData( pITmpRiffStream );

							// Create a new PianoRollUIState
							PianoRollUIState* pPianoRollUIState = new PianoRollUIState;
							if( pPianoRollUIState )
							{
								// Allocate memory to store the design-time chunk in
								pPianoRollUIState->pPianoRollData = (BYTE *)GlobalAlloc( GPTR, dwEndPosition - dwPosition );
								if( pPianoRollUIState->pPianoRollData == NULL )
								{
									delete pPianoRollUIState;
								}
								else
								{
									// Inititialize a temporary pointer
									BYTE *pTemp = (BYTE *)pPianoRollUIState->pPianoRollData;

									if( SUCCEEDED( StreamSeek( pTmpStream, 0, STREAM_SEEK_SET ) )
									&&	SUCCEEDED( pTmpStream->Read( pTemp, dwEndPosition - dwPosition, NULL ) ) )
									{
										// Set the amount of data stored in pPianoRollData
										pPianoRollUIState->dwPianoRollDataSize = dwEndPosition - dwPosition;

										// Add the design data to the end of the list for this PartRef
										pDMPartRef->m_lstPianoRollUIStates.AddTail( pPianoRollUIState );
									}
									else
									{
										GlobalFree( pPianoRollUIState->pPianoRollData );
										delete pPianoRollUIState;
									}
								}
							}
						}
						pITmpRiffStream->Release();
					}
					pTmpStream->Release();
				}
			}

			// Ascend out of PianoRoll list chunk
			if( pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
			{
 				hr = E_FAIL;
				goto ON_ERROR;
			}
		}
	}

	// If there are no PianoRollStrips
	if( m_pPRSList.IsEmpty()
	&&	!pDMPartRef->m_lstPianoRollUIStates.IsEmpty() )
	{
		position = pDMPartRef->m_lstPianoRollUIStates.GetHeadPosition();
		while( position )
		{
			PianoRollUIState* pPianoRollUIState = pDMPartRef->m_lstPianoRollUIStates.GetNext( position );
			if( pPianoRollUIState->pPianoRollData )
			{
				// Write PianoRoll list header
				ckMain.fccType = DMUS_FOURCC_PIANOROLL_LIST;
				if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Write PianoRoll chunk data
				hr = pIStream->Write( pPianoRollUIState->pPianoRollData, pPianoRollUIState->dwPianoRollDataSize, &dwBytesWritten);
				if( FAILED( hr ) || dwBytesWritten != pPianoRollUIState->dwPianoRollDataSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Ascend out of PianoRoll list chunk
				if( pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
				{
 					hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}
	}

ON_ERROR:
	if( pIStream != NULL )
	{
		pIStream->Release();
	}
    return hr;
}

HRESULT CMIDIMgr::InsertDefaultChord( void )
{
	if( m_pIDMTrack )
	{
		return S_FALSE;
	}

	HRESULT hr = E_FAIL;

	ASSERT( m_pChordTrack != NULL );	// Should not happen!
	if( m_pChordTrack )
	{
		ASSERT( m_pChordTrack->m_pIStripMgr != NULL );	// Should not happen!
		if( m_pChordTrack->m_pIStripMgr )
		{
			hr = m_pChordTrack->m_pIStripMgr->SetParam( GUID_ChordParam, 0, &m_chordDefault );
			if(SUCCEEDED(hr))
			{
				hr = m_pChordTrack->m_pIStripMgr->SetParam(GUID_ChordIndividualChordSharpsFlats,
															0, &m_fDefaultChordFlatsNotSharps);
			}
		}
	}

	return hr;
}

HRESULT CMIDIMgr::AddChordTrack( CChordTrack* pChordTrack, IStream* pIStream )
{
	if( m_pIDMTrack )
	{
		return S_FALSE;
	}

	HRESULT hr;
	IDMUSProdStripMgr* pIStripMgr = NULL;
	IPersistStream* pIPersistStreamStrip = NULL;
	BOOL fUseGroupBitsPPG;
	BYTE bMode;
	BOOL fFlatsNotSharps = m_pDMPattern->m_fKeyFlatsNotSharps;

	ASSERT( pChordTrack != NULL );
	if( pChordTrack == NULL )
	{
		hr = E_INVALIDARG;
		goto ON_ERROR;
	}

	// Create ChordMgr
	hr = ::CoCreateInstance( CLSID_ChordMgr, NULL, CLSCTX_INPROC, IID_IDMUSProdStripMgr, (void**)&pIStripMgr );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	// Set pChordTrack's ChordMgr
	pChordTrack->SetStripMgr( pIStripMgr );

	// Hand ChordMgr a Framework pointer
	VARIANT varFramework;
	varFramework.vt = VT_UNKNOWN;
	V_UNKNOWN( &varFramework ) = m_pIFramework;
	hr = pIStripMgr->SetStripMgrProperty( SMP_IDMUSPRODFRAMEWORK, varFramework );
	if( FAILED( hr ) )
	{
		TRACE("MIDIMgr: Failed to set ChordStripMgr's Framework pointer\n");
		goto ON_ERROR;
	}

	// Set ChordMgr's group bits
	DMUS_IO_TRACK_HEADER ioTrackHeader;
	ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
	ioTrackHeader.dwGroup = pChordTrack->m_dwGroupBits;
	VARIANT varTrackHeader;
	varTrackHeader.vt = VT_BYREF;
	V_BYREF(&varTrackHeader) = &ioTrackHeader;
	hr = pIStripMgr->SetStripMgrProperty( SMP_DMUSIOTRACKHEADER, varTrackHeader );
	if( FAILED( hr ) )
	{
		TRACE("MIDIMgr: Failed to set ChordStripMgr's GroupBits\n");
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Don't allow user to set ChordStrip GroupBits
	fUseGroupBitsPPG = FALSE;
	hr = pIStripMgr->SetParam( GUID_UseGroupBitsPPG, 0, &fUseGroupBitsPPG );
	if( FAILED( hr ) )
	{
		TRACE("MIDIMgr: Failed to set ChordStripMgr's UseGroupBits flag\n");
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Set the default ChordStrip mode
	bMode = CHORDSTRIP_MODE_MIDI_CONSTANT;
	hr = pIStripMgr->SetParam( GUID_PatternEditorMode, 0, &bMode );
	if( FAILED( hr ) )
	{
		TRACE("MIDIMgr: Failed to set ChordStripMgr's PatternEditorMode\n");
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Load Chord track chords into ChordMgr
	if( pIStream )
	{
		if( SUCCEEDED ( pIStripMgr->QueryInterface( IID_IPersistStream, (void **)&pIPersistStreamStrip ) ) )
		{
			pIPersistStreamStrip->Load( pIStream );
		}
	}

	m_pChordTrack = pChordTrack;
	m_pTimeline->AddStripMgr( pIStripMgr, pChordTrack->m_dwGroupBits );

	// Make sure the Chord track has Chords!
	// Get the Chord on the first beat of the first measure
	DMUS_CHORD_PARAM chordData;

	if( GetChordFromChordStrip( 0, &chordData, &fFlatsNotSharps ) == S_OK )
	{
		ASSERT( chordData.wMeasure == 0 );
		ASSERT( chordData.bBeat == 0 );

		if( chordData.wMeasure == 0
		&&  chordData.bBeat == 0 )
		{
			m_chordDefault = chordData;
			m_fDefaultChordFlatsNotSharps = fFlatsNotSharps;
		}
	}
	else
	{
		// Prepare default Chord
		wcscpy( m_chordDefault.wszName, L"M7" );
		m_chordDefault.wMeasure = 0;
		m_chordDefault.bBeat = 0;
		m_chordDefault.bSubChordCount = 4;
		m_chordDefault.SubChordList[0].dwChordPattern = m_pDMPattern->m_dwDefaultChordPattern;
		m_chordDefault.SubChordList[0].dwScalePattern = m_pDMPattern->m_dwDefaultKeyPattern;
		m_chordDefault.SubChordList[0].dwInversionPoints = 0xffffff;	// default: inversions everywhere
		m_chordDefault.SubChordList[0].dwLevels = 0xfffffff1;
		m_chordDefault.SubChordList[0].bChordRoot = m_pDMPattern->m_bDefaultChordRoot;
		m_chordDefault.SubChordList[0].bScaleRoot = m_pDMPattern->m_bDefaultKeyRoot;
		m_chordDefault.SubChordList[1] = m_chordDefault.SubChordList[0]; 
		m_chordDefault.SubChordList[1].dwLevels = 0x00000002;
		m_chordDefault.SubChordList[2] = m_chordDefault.SubChordList[0]; 
		m_chordDefault.SubChordList[2].dwLevels = 0x00000004;
		m_chordDefault.SubChordList[3] = m_chordDefault.SubChordList[0]; 
		m_chordDefault.SubChordList[3].dwLevels = 0x00000008;

		m_fDefaultChordFlatsNotSharps = m_pDMPattern->m_fChordFlatsNotSharps;
		
		// Insert default Chord
		InsertDefaultChord();

		// Set key of chord strip
		WORD key = (WORD)(m_pDMPattern->m_bDefaultKeyRoot % 12);
		WORD nAcc = 0;
		switch(key)
		{
		case 0:
			nAcc = 0;
			break;
		case 1:
			nAcc = (WORD)(fFlatsNotSharps ? 5 : 7);
			break;
		case 2:
			nAcc = 2;
			break;
		case 3:
			nAcc = 3;
			break;
		case 4:
			nAcc = 4;
			break;
		case 5:
			nAcc = 1;
			break;
		case 6:
			nAcc = 6;
			break;
		case 7:
			nAcc = 1;
			break;
		case 8:
			nAcc = 4;
			break;
		case 9:
			nAcc = 3;
			break;
		case 10:
			nAcc = 2;
			break;
		case 11:
			nAcc = (WORD)(fFlatsNotSharps ? 7 : 5);
			break;
		}
		hr = m_pChordTrack->m_pIStripMgr->SetParam( GUID_ChordSharpsFlats, 0, &(m_pDMPattern->m_fKeyFlatsNotSharps));
		DWORD data = key | (nAcc << 16);
		hr = m_pChordTrack->m_pIStripMgr->SetParam( GUID_ChordKey, 0, &data );
	}

ON_ERROR:
	RELEASE( pIPersistStreamStrip );
	RELEASE( pIStripMgr );

	return hr;
}

HRESULT CMIDIMgr::CreateDefaultChordStrip( void )
{
	if( m_pIDMTrack )
	{
		return S_FALSE;
	}

	CChordTrack* pChordTrack = new CChordTrack;
	if( pChordTrack == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Set Chord track group bits
	pChordTrack->m_dwGroupBits = 1;

	// Add Chord track to the timeline
	HRESULT hr = AddChordTrack( pChordTrack, NULL );

	return hr;
}

HRESULT CMIDIMgr::GetChordFromChordStrip( MUSIC_TIME mt, DMUS_CHORD_PARAM* pchordData, BOOL* pfFlatsNotSharps )
{
	ASSERT( m_pTimeline != NULL );
	ASSERT( (m_pChordTrack != NULL) || (m_pIDMTrack != NULL) );
	ASSERT( pchordData != NULL );

	HRESULT hr;
	
	hr = m_pTimeline->GetParam( GUID_ChordParam, m_pIDMTrack ? m_dwGroupBits : m_pChordTrack->m_dwGroupBits, 0, mt, NULL, pchordData );

	if( SUCCEEDED ( hr ) )
	{
		if( pfFlatsNotSharps )
		{
			hr = m_pTimeline->GetParam( GUID_ChordIndividualChordSharpsFlats,
										m_pIDMTrack ? m_dwGroupBits : m_pChordTrack->m_dwGroupBits, 0, mt, NULL, pfFlatsNotSharps);
		}
	}

	return hr;
}
HRESULT CMIDIMgr::GetChord( CDirectMusicPart *pDMPart, const CDirectMusicStyleNote *pDMNote, DMUS_CHORD_PARAM* pchordData )
{
	ASSERT( m_pTimeline != NULL );
	ASSERT( (m_pChordTrack != NULL) || (m_pIDMTrack != NULL) );
	ASSERT( pDMPart != NULL );
	ASSERT( pDMNote != NULL );
	ASSERT( pchordData != NULL );

	// Only the grid position determines where we get the chord from.
	long lStartClock = GRID_TO_CLOCKS( pDMNote->m_mtGridStart, pDMPart );

	HRESULT hr = GetChordFromChordStrip( lStartClock, pchordData, NULL );
	
	if( FAILED ( hr ) )
	{
		*pchordData = dmSegmentDefaultChord;
		hr = S_OK;
	}

	return hr;
}

HRESULT CMIDIMgr::LoadChordStripDesignData( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	CChordTrack* pChordTrack = NULL;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;
	DWORD dwPos;
	HRESULT hr = S_OK;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );


    dwPos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_CHORDTRACK_LIST:
					if( m_pIDMTrack == NULL )
					{
						pChordTrack = new CChordTrack;
						if( pChordTrack == NULL )
						{
							hr = E_OUTOFMEMORY;
							goto ON_ERROR;
						}

						// Set Chord track group bits
						pChordTrack->m_dwGroupBits = 1;

						// Add Chord track to the timeline
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = AddChordTrack( pChordTrack, pIStream );
						if( FAILED( hr ) )
						{
							goto ON_ERROR;
						}
						break;
					}
				}
				break;

			case DMUS_FOURCC_CHORDSTRIP_UI_CHUNK:
			{
			    ioDMChordStripUI oDMChordStripUI;

				dwSize = min( ck.cksize, sizeof( ioDMChordStripUI ) );
				hr = pIStream->Read( &oDMChordStripUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( pChordTrack 
				&&  pChordTrack->m_pIStripMgr )
				{
					pChordTrack->m_pIStripMgr->SetParam( GUID_PatternEditorMode, 0, &oDMChordStripUI.m_bMode );
				}
				else if( m_pIDMTrack )
				{
					m_bTempPatternEditorMode = oDMChordStripUI.m_bMode;
				}
				break;
			}
        }

        pIRiffStream->Ascend( &ck, 0 );
	    dwPos = StreamTell( pIStream );
    }

ON_ERROR:
	if( FAILED ( hr ) )
	{
		m_pChordTrack = NULL;
		delete pChordTrack;
	}
	RELEASE( pIStream );
	return hr;
}

HRESULT CMIDIMgr::SaveChordStripDesignData( IDMUSProdRIFFStream* pIRiffStream )
{
	ASSERT( m_pTimeline != NULL );

	IPersistStream* pIPersistStreamStrip = NULL;
	MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	HRESULT hr = S_OK;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write DMUS_FOURCC_CHORDSTRIP_LIST header
	ckMain.fccType = DMUS_FOURCC_CHORDSTRIP_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save the chord list
	if( m_pChordTrack != NULL )
	{
		ASSERT( m_pChordTrack->m_pIStripMgr != NULL );

		// Get DirectMusic Chord track IPersistStream interface
		hr = m_pChordTrack->m_pIStripMgr->QueryInterface( IID_IPersistStream, (void**)&pIPersistStreamStrip );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}

		// Save DirectMusic Chord track into the stream
		hr = pIPersistStreamStrip->Save( pIStream, FALSE );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

	// Save the ChordStrip UI state
	{
		ioDMChordStripUI oDMChordStripUI;

		// Get the ChordStrip mode
		BYTE bMode;
		if( (m_pChordTrack == NULL)
		||	FAILED ( m_pTimeline->GetParam( GUID_PatternEditorMode, m_pChordTrack->m_dwGroupBits,
											0, 0, NULL, &bMode ) ) )
		{
			bMode = m_bTempPatternEditorMode;
			if( m_pIDMTrack && m_pTimeline )
			{
				m_pTimeline->GetParam( GUID_PatternEditorMode, m_dwGroupBits,
									   0, 0, NULL, &bMode );
			}
		}

		// Write DMUS_FOURCC_CHORDSTRIP_UI_CHUNK header
		ck.ckid = DMUS_FOURCC_CHORDSTRIP_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare ioDMChordStripUI structure
		memset( &oDMChordStripUI, 0, sizeof(ioDMChordStripUI) );

		oDMChordStripUI.m_bMode = bMode;

		// Write DMUS_FOURCC_CHORDSTRIP_UI_CHUNK data
		hr = pIStream->Write( &oDMChordStripUI, sizeof(ioDMChordStripUI), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioDMChordStripUI) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		
		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
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
	RELEASE( pIStream );
	RELEASE( pIPersistStreamStrip );

	return hr;
}

void CMIDIMgr::KillFocus( CPianoRollStrip* pPRStrip )
{
	if (m_pActivePianoRollStrip == pPRStrip)
	{
		if( pPRStrip )
		{
			pPRStrip->UnRegisterMidi();
		}
		m_pActivePianoRollStrip = NULL;
		m_NotePropData.m_Param.pPart = NULL;

		int iOldFocus = m_iFocus;
		m_iFocus = 0;

		if( pPRStrip && (iOldFocus == 1) )
		{
			// Need to erase note cursor
			pPRStrip->InvalidatePianoRoll();
		}

		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}

		UpdateAuditionVariations();

		UpdateStatusBarDisplay();
	}
}

void CMIDIMgr::SetFocus( CPianoRollStrip* pPRStrip, int iFocus )
{
	ASSERT( pPRStrip != NULL );
	if( pPRStrip == NULL )
	{
		return;
	}

	if (m_pActivePianoRollStrip != pPRStrip)
	{
		// m_pActivePianoRollStrip should always be null, because KillFocus is always called on the old strip before SetFocus
		// is called on the new strip
		ASSERT( m_pActivePianoRollStrip == NULL );

		m_pActivePianoRollStrip = pPRStrip;
		if ( m_fWindowActive )
		{
			m_pActivePianoRollStrip->RegisterMidi();

			if( m_fRecordEnabled && m_pSegmentState )
			{
				// Enable thruing
				m_pActivePianoRollStrip->SetPChannelThru();
			}
		}

		ASSERT( pPRStrip->ValidPartRefPtr() );
		if ( m_pActivePianoRollStrip->ValidPartRefPtr() )
		{
			ComputeSelectedPropNote( m_pActivePianoRollStrip->m_pPartRef->m_pDMPart, m_pActivePianoRollStrip->m_dwVariations );
			if( (m_pDMPattern->m_fInLoad == FALSE)
			||	!ParentNotePPGShowing() )
			{
				RefreshPropertyPage( m_pActivePianoRollStrip->m_pPartRef->m_pDMPart );
			}
		}

		UpdateAuditionVariations();

		m_iFocus = iFocus;

		if( m_pActivePianoRollStrip && (m_iFocus == 1) )
		{
			m_pActivePianoRollStrip->UpdateNoteCursorTime();
			// Need to draw the note cursor
			m_pActivePianoRollStrip->InvalidatePianoRoll();

			// If we're in the segment designer
			if( m_pIDMTrack )
			{
				// If there currently is no primary segment playing, try and play the patch for this PChannel
				REFERENCE_TIME rtLatency;
				m_pIDMPerformance->GetLatencyTime( &rtLatency );
				MUSIC_TIME mtLatency;
				m_pIDMPerformance->ReferenceToMusicTime( rtLatency, &mtLatency );

				IDirectMusicSegmentState *pSegState;
				if( FAILED( m_pIDMPerformance->GetSegmentState( &pSegState, mtLatency ) ) )
				{
					long lTime;
					if (SUCCEEDED(m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime )))
					{
						PlayPatch( lTime, m_pActivePianoRollStrip->m_pPartRef->m_dwPChannel );
					}
				}
				else
				{
					pSegState->Release();
				}
			}
		}

		UpdateStatusBarDisplay();
	}
}

BOOL CMIDIMgr::UpdateSelectedParamsFromActiveStrip( void )
{
	if ( m_pActivePianoRollStrip == NULL )
	{
		return FALSE;
	}

	// This sometimes would overwrite a guid of all zeros (a valid state),
	// which means that the note property page should be disabled.
	//memcpy( &m_NotePropData.m_Param.guid, &pPRD->guid, sizeof(GUID) );
	m_NotePropData.m_Param.dwVariations = m_pActivePianoRollStrip->m_dwVariations;

	return TRUE;
}

HRESULT CMIDIMgr::SaveQuantizeDesignData( IDMUSProdRIFFStream* pIRiffStream )
{
	MMCKINFO ck;
	HRESULT hr = S_OK;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write DMUS_FOURCC_QUANTIZE_CHUNK header
	ck.ckid = DMUS_FOURCC_QUANTIZE_CHUNK;
	if( pIRiffStream->CreateChunk(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Fill in ioDMPatternQuantize structure
	ioDMPatternQuantize oQuantize;
	ZeroMemory( &oQuantize, sizeof( ioDMPatternQuantize ) );
	oQuantize.m_wQuantizeTarget = (WORD)m_qtQuantTarget;
	oQuantize.m_bResolution = m_bQuantResolution;
	oQuantize.m_bStrength = m_bQuantStrength;
	oQuantize.m_dwFlags = m_dwQuantFlags;

	// Write DMUS_FOURCC_QUANTIZE_CHUNK chunk data
	DWORD dwBytesWritten;
	hr = pIStream->Write( &oQuantize, sizeof(ioDMPatternQuantize), &dwBytesWritten);
	if( FAILED( hr ) || dwBytesWritten != sizeof(ioDMPatternQuantize) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		//goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIStream );

	return hr;
}

HRESULT CMIDIMgr::LoadQuantizeDesignData( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	UNREFERENCED_PARAMETER(pckMain);

	HRESULT hr = S_OK;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Read DMUS_FOURCC_QUANTIZE_CHUNK chunk data
	DWORD dwBytesRead;
	ioDMPatternQuantize iQuantize;
	ZeroMemory( &iQuantize, sizeof( ioDMPatternQuantize ) );
	hr = pIStream->Read( &iQuantize, sizeof(ioDMPatternQuantize), &dwBytesRead);
	if( FAILED( hr ) || dwBytesRead != sizeof(ioDMPatternQuantize) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Fill in our local variables
	m_qtQuantTarget = static_cast<QUANTIZE_TARGET>(iQuantize.m_wQuantizeTarget);
	m_bQuantResolution = iQuantize.m_bResolution;
	m_bQuantStrength = iQuantize.m_bStrength;
	m_dwQuantFlags = iQuantize.m_dwFlags;

ON_ERROR:
	RELEASE( pIStream );

	return hr;
}

HRESULT CMIDIMgr::SaveVelocitizeDesignData( IDMUSProdRIFFStream* pIRiffStream )
{
	MMCKINFO ck;
	HRESULT hr = S_OK;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write DMUS_FOURCC_VELOCITIZE_CHUNK header
	ck.ckid = DMUS_FOURCC_VELOCITIZE_CHUNK;
	if( pIRiffStream->CreateChunk(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Fill in ioDMPatternVelocitize structure
	ioDMPatternVelocitize oVelocitize;
	ZeroMemory( &oVelocitize, sizeof( ioDMPatternVelocitize ) );
	oVelocitize.m_wVelocityTarget = (WORD)m_vtVelocityTarget;
	oVelocitize.m_bCompressMin = m_bVelocityCompressMin;
	oVelocitize.m_bCompressMax = m_bVelocityCompressMax;
	oVelocitize.m_lAbsoluteChangeStart = m_lVelocityAbsChangeStart;
	oVelocitize.m_lAbsoluteChangeEnd = m_lVelocityAbsChangeEnd;
	oVelocitize.m_dwVelocityMethod = m_fVelocityAbsolue ?
		(m_fVelocityPercent ? PATTERN_VELOCITIZE_PERCENT : PATTERN_VELOCITIZE_LINEAR) :
		PATTERN_VELOCITIZE_COMPRESS;

	// Write DMUS_FOURCC_VELOCITIZE_CHUNK chunk data
	DWORD dwBytesWritten;
	hr = pIStream->Write( &oVelocitize, sizeof(ioDMPatternVelocitize), &dwBytesWritten);
	if( FAILED( hr ) || dwBytesWritten != sizeof(ioDMPatternVelocitize) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		//goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIStream );

	return hr;
}

HRESULT CMIDIMgr::LoadVelocitizeDesignData( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	HRESULT hr = S_OK;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Read DMUS_FOURCC_VELOCITIZE_CHUNK chunk data
	DWORD dwBytesRead, dwBytesToRead;
	ioDMPatternVelocitize iVelocitize;
	ZeroMemory( &iVelocitize, sizeof( ioDMPatternVelocitize ) );
	dwBytesToRead = min( pckMain->cksize, sizeof(ioDMPatternVelocitize) );
	hr = pIStream->Read( &iVelocitize, dwBytesToRead, &dwBytesRead);
	if( FAILED( hr ) || dwBytesRead != dwBytesToRead )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// If this is an old structure, copy the start value to the end value
	if( dwBytesToRead < sizeof( ioDMPatternVelocitize ) )
	{
		iVelocitize.m_lAbsoluteChangeEnd = iVelocitize.m_lAbsoluteChangeStart;
	}

	// Fill in our local variables
	m_vtVelocityTarget = static_cast<VELOCITY_TARGET>(iVelocitize.m_wVelocityTarget);
	m_bVelocityCompressMin = iVelocitize.m_bCompressMin;
	m_bVelocityCompressMax = iVelocitize.m_bCompressMax;
	m_lVelocityAbsChangeStart = iVelocitize.m_lAbsoluteChangeStart;
	m_lVelocityAbsChangeEnd = iVelocitize.m_lAbsoluteChangeEnd;
	switch( iVelocitize.m_dwVelocityMethod & PATTERN_VELOCITIZE_METHOD_MASK )
	{
	case PATTERN_VELOCITIZE_PERCENT:
		m_fVelocityAbsolue = true;
		m_fVelocityPercent = true;
		break;
	case PATTERN_VELOCITIZE_LINEAR:
		m_fVelocityAbsolue = true;
		m_fVelocityPercent = false;
		break;
	case PATTERN_VELOCITIZE_COMPRESS:
		m_fVelocityAbsolue = false;
		m_fVelocityPercent = true;
		break;
	default:
		ASSERT(FALSE);
		m_fVelocityAbsolue = true;
		m_fVelocityPercent = true;
		break;
	}

ON_ERROR:
	RELEASE( pIStream );

	return hr;
}

void CMIDIMgr::QuantizeSelectedNotes( CDirectMusicPart* pPart, DWORD dwVariations )
{
	// Validate the partref pointer
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return;
	}

	BOOL fChanged = FALSE;

	// Iterate through all events
	CDirectMusicEventItem* pEvent = pPart->GetFirstNote( dwVariations );
	while( pEvent )
	{
		// Only quantize notes that are selected and belong to the given variations
		if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
		{
			if (pEvent->m_dwVariation & ~dwVariations)
			{
				// This note belongs to at least 1 other variation, create a new note
				// to store the variations that were quantized
				CDirectMusicStyleNote* pNote = new CDirectMusicStyleNote((CDirectMusicStyleNote*)pEvent);

				MUSIC_TIME mtOrigStartTime = pPart->AbsTime( pNote );

				// Quantize it
				if( pPart->QuantizeNote( pNote, m_dwQuantFlags, m_bQuantStrength, m_bQuantResolution ) )
				{
					// If it changed, set fChanged to TRUE and add the new, quantized note
					// to the list of events
					fChanged = TRUE;

					// Delete the unquantized note from the variations we're quantizing
					pEvent->m_dwVariation &= ~dwVariations;

					// Add the quantized note to the selected variations
					pNote->m_dwVariation &= dwVariations;

					// 22893: Only insert the note if pPart->QuantizeNote() didn't already.
					// It removes and re-adds the note only if the start time changes, so if it didn't change
					// (but the duration did), add the note.
					// Add it, and go to the next event
					if( pPart->AbsTime( pNote ) == mtOrigStartTime )
					{
						pPart->InsertNoteInAscendingOrder( pNote );
					}
				}
				else
				{
					pNote->m_pNext = NULL;
					delete pNote;
				}
			}
			else
			{
				// Quantizing all variations
				if( pPart->QuantizeNote( (CDirectMusicStyleNote*)pEvent, m_dwQuantFlags, m_bQuantStrength, m_bQuantResolution ) )
				{
					fChanged = TRUE;
				}
			}
		}

		// Go to the next event
		pEvent = pEvent->GetNext();
	}

	if (fChanged)
	{
		// update the corresponding DirectMusicPart's parameters
		UpdatePartParamsAfterChange( pPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_UNDO_QUANTIZE );

		// Invalidate all strips displaying this part
		RefreshPartDisplay( pPart, dwVariations, FALSE, FALSE );
	}
}

void CMIDIMgr::QuantizeEntirePart( CDirectMusicPart* pPart )
{
	BOOL fChanged;
	fChanged = pPart->Quantize( m_dwQuantFlags, m_bQuantStrength, m_bQuantResolution );

	if (fChanged)
	{
		// update the corresponding DirectMusicPart's parameters
		UpdatePartParamsAfterChange( pPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_UNDO_QUANTIZE );

		// Invalidate all strips displaying this part
		RefreshPartDisplay( pPart, ALL_VARIATIONS, FALSE, FALSE );
	}
}

void CMIDIMgr::QuantizeEntirePattern( )
{
	// Mark all parts as not yet quantized
	POSITION pos = m_lstStyleParts.GetHeadPosition();
	while( pos )
	{
		m_lstStyleParts.GetNext( pos )->m_fQuantOrVelocitized = FALSE;
	}

	// Iterate through all PartRefs
	pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		CDirectMusicPartRef *pPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );

		// Check if we haven't yet quantized this part
		if( !pPartRef->m_pDMPart->m_fQuantOrVelocitized )
		{
			// Assume something will change
			PreChangePartRef( pPartRef );

			// Quantize each part
			QuantizeEntirePart( pPartRef->m_pDMPart );

			// Make the part as Quantized
			pPartRef->m_pDMPart->m_fQuantOrVelocitized = TRUE;
		}
	}
}

void CMIDIMgr::VelocitizeSelectedNotes( CDirectMusicPart* pPart, DWORD dwVariations )
{
	// Validate the partref pointer
	ASSERT( pPart != NULL );
	if( pPart == NULL )
	{
		return;
	}

	BOOL fChanged = FALSE;

	// The time of the first and last selected notes
	MUSIC_TIME mtFirstSelected = LONG_MAX;
	MUSIC_TIME mtLastSelected = LONG_MIN;
	MUSIC_TIME mtSelectedSpan = 0;
	const long lAbsChangeSpan = m_lVelocityAbsChangeEnd - m_lVelocityAbsChangeStart;

	// If doing an absolute change and the start and end values are different
	if( m_fVelocityAbsolue
	&&	(lAbsChangeSpan != 0) )
	{
		// Find the time of the first and last selected notes
		CDirectMusicEventItem* pEvent = pPart->GetFirstNote( dwVariations );
		while( pEvent )
		{
			// Only look at notes that are selected and belong to the given variations
			if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
			{
				// Check if this note is earlier than all others, or later than all others
				MUSIC_TIME mtStart = pPart->AbsTime( pEvent );
				if( mtStart < mtFirstSelected )
				{
					mtFirstSelected = mtStart;
				}
				if( mtStart > mtLastSelected )
				{
					mtLastSelected = mtStart;
				}
			}

			// Go to the next event
			pEvent = pEvent->GetNext();
		}

		mtSelectedSpan = mtLastSelected - mtFirstSelected;
	}

	// Iterate through all events
	CDirectMusicEventItem* pEvent = pPart->GetFirstNote( dwVariations );
	while( pEvent )
	{
		// Only velocitize notes that are selected and belong to the given variations
		if ( (pEvent->m_fSelected == TRUE) && (pEvent->m_dwVariation & dwVariations) )
		{
			// Compute the absolute velocity change for this note, if necessary
			long lAbsChange = m_lVelocityAbsChangeStart;
			if( m_fVelocityAbsolue
			&&	(lAbsChangeSpan != 0)
			&&	(mtSelectedSpan != 0) )
			{
				lAbsChange += (lAbsChangeSpan * (pPart->AbsTime( pEvent ) - mtFirstSelected)) / mtSelectedSpan;
			}

			if (pEvent->m_dwVariation & ~dwVariations)
			{
				// This note belongs to at least 1 other variation, create a new note
				// to store the variations that were velocitied
				CDirectMusicStyleNote* pNote = new CDirectMusicStyleNote((CDirectMusicStyleNote*)pEvent);

				// Velocitize it
				if( pPart->VelocitizeNote( pNote, m_fVelocityAbsolue, m_fVelocityPercent, lAbsChange, m_bVelocityCompressMin, m_bVelocityCompressMax ) )
				{
					// If it changed, set fChanged to TRUE and add the new, velocitied note
					// to the list of events
					fChanged = TRUE;

					// Delete the unvelocitied note from the variations we're velocitizing
					pEvent->m_dwVariation &= ~dwVariations;

					// Add the velocitied note to the selected variations
					pNote->m_dwVariation &= dwVariations;

					// Add it to the part, and go to the next event
					pPart->InsertNoteInAscendingOrder( pNote );
				}
				else
				{
					pNote->m_pNext = NULL;
					delete pNote;
				}
			}
			else
			{
				// Velocitizing all variations
				if( pPart->VelocitizeNote( (CDirectMusicStyleNote*)pEvent, m_fVelocityAbsolue, m_fVelocityPercent, lAbsChange, m_bVelocityCompressMin, m_bVelocityCompressMax ) )
				{
					fChanged = TRUE;
				}
			}
		}

		// Go to the next event
		pEvent = pEvent->GetNext();
	}

	if (fChanged)
	{
		// update the corresponding DirectMusicPart's parameters
		UpdatePartParamsAfterChange( pPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_UNDO_VELOCITY );

		// Invalidate all strips displaying this part
		RefreshPartDisplay( pPart, dwVariations, FALSE, FALSE );
	}
}

void CMIDIMgr::VelocitizeEntirePart( CDirectMusicPart* pPart )
{
	BOOL fChanged;
	fChanged = pPart->Velocitize( m_fVelocityAbsolue, m_fVelocityPercent, m_lVelocityAbsChangeStart, m_lVelocityAbsChangeEnd, m_bVelocityCompressMin, m_bVelocityCompressMax );

	if (fChanged)
	{
		// update the corresponding DirectMusicPart's parameters
		UpdatePartParamsAfterChange( pPart );

		// Let the object know about the changes
		UpdateOnDataChanged( IDS_UNDO_VELOCITY );

		// Invalidate all strips displaying this part
		RefreshPartDisplay( pPart, ALL_VARIATIONS, FALSE, FALSE );
	}
}

void CMIDIMgr::VelocitizeEntirePattern( )
{
	// Mark all parts as not yet velocitized
	POSITION pos = m_lstStyleParts.GetHeadPosition();
	while( pos )
	{
		m_lstStyleParts.GetNext( pos )->m_fQuantOrVelocitized = FALSE;
	}

	// Iterate through all PartRefs
	pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		CDirectMusicPartRef *pPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );

		// Check if we haven't yet velocitized this part
		if( !pPartRef->m_pDMPart->m_fQuantOrVelocitized )
		{
			// Assume something will change
			PreChangePartRef( pPartRef );

			// Velocitize each part
			VelocitizeEntirePart( pPartRef->m_pDMPart );

			// Make the part as Velocitized
			pPartRef->m_pDMPart->m_fQuantOrVelocitized = TRUE;
		}
	}
}

HRESULT CMIDIMgr::OnDeletePart( CPianoRollStrip* pPRSClickedOn )
{
	if( AfxMessageBox(IDS_WARNING_DELETE_TRACK, MB_OKCANCEL) != IDOK )
	{
		return S_FALSE;
	}

	// Skip the TimeStrip, since it can't be removed.
	DWORD dwEnum = 1;
	IDMUSProdStrip* pIStrip;
	VARIANT variant;
	HRESULT hr = E_FAIL;

	// Try and find gutter-selected strips
	while( m_pTimeline->EnumStrip( dwEnum, &pIStrip ) == S_OK )
	{
		ASSERT( pIStrip );
		dwEnum++;
		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pIStrip, STP_GUTTER_SELECTED, &variant ) ) )
		{
			// If the strip's gutter is selected
			if( V_BOOL( &variant ) == TRUE )
			{
				if( m_pPRSList.Find( (CPianoRollStrip *)pIStrip ) )
				{
					// It must be a PianoRollStrip
					CPianoRollStrip *pPRS = (CPianoRollStrip *)pIStrip;
					HRESULT hrTmp = RemoveStrip( pPRS );
					if( SUCCEEDED( hrTmp ) )
					{
						hr = hrTmp;

						// Start over, since any number of strips may have been removed.
						dwEnum = 1;
					}
				}
			}
		}

		pIStrip->Release();
	}

	// If no strips were removed, try and remove pPRSClickedOn
	if( FAILED( hr )
	&&	pPRSClickedOn )
	{
		hr = RemoveStrip( pPRSClickedOn );
	}

	// If at least one strip was removed
	if( SUCCEEDED( hr ) )
	{
		bool fSavedState = false;

		// If there are no more PartRefs
		if( m_pDMPattern->m_lstPartRefs.IsEmpty() )
		{
			// Delete our track
			IDMUSProdSegmentEdit8 *pIDMUSProdSegmentEdit8;
			if( m_pDMProdSegmentNode
			&&	SUCCEEDED( m_pDMProdSegmentNode->QueryInterface( IID_IDMUSProdSegmentEdit8, (void **)&pIDMUSProdSegmentEdit8 ) ) )
			{
				if( SUCCEEDED( pIDMUSProdSegmentEdit8->RemoveStripMgr( static_cast<IDMUSProdStripMgr *>(this) ) ) )
				{
					fSavedState = true;
				}
				pIDMUSProdSegmentEdit8->Release();
			}
		}

		if( !fSavedState )
		{
			// Save an undo state
			UpdateOnDataChanged( IDS_UNDO_DELETE_PART );
		}
	}

	return hr;
}

HRESULT CMIDIMgr::RemoveStrip( CPianoRollStrip* pPRS )
{
	// Try and find this strip in the list of PianoRollStrips we control
	POSITION pos;
	pos = m_pPRSList.Find( pPRS );

	ASSERT( pos );
	if( pos )
	{
		KillFocus( pPRS );

		// Remove the Piano Roll Strip from the Timeline
		m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pPRS );

		// Remove this strip from the list of PianoRollStrips we control
		m_pPRSList.RemoveAt( pos );

		// Remove the strip's property page (will fail if not active)
		m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)pPRS);

		// Save a pointer to the partref
		CDirectMusicPartRef* pPartRef = pPRS->m_pPartRef;

		// Release the strip
		pPRS->Release();

		// Set an undo string
		if( DMPartRefToStrip( pPartRef ) == NULL )
		{
			// No more PianoRollStrips display this partref - delete it
			m_pDMPattern->DeletePartRef( pPartRef );
			// If the part that this partref referenced is no longer used,
			// DeletePartRef will remove it.
		}

		return S_OK;
	}

	return E_INVALIDARG;
}

DWORD CMIDIMgr::DeterminePositionForPianoRollStrip( const CDirectMusicPartRef* pPartRef ) const
{
	ASSERT( m_pTimeline != NULL );

	DWORD dwPosition = 0;
	VARIANT var;

	CPianoRollStrip* pPianoRollStrip;
	IDMUSProdStrip* pIStrip;
	CLSID clsid;

	BOOL fContinue = TRUE;

	const long lPartRefIndex = long( PChannelToStripIndex( pPartRef->m_dwPChannel ) );
	while( fContinue  &&  SUCCEEDED( m_pTimeline->EnumStrip( ++dwPosition, &pIStrip ) ) )
	{
		var.vt = VT_BYREF;
		V_BYREF(&var) = &clsid;

		if( SUCCEEDED ( pIStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &var ) ) )
		{
			if( ::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
			{
				pPianoRollStrip= (CPianoRollStrip *)pIStrip;
				const long lPRSIndex = long( PChannelToStripIndex( pPianoRollStrip->m_pPartRef->m_dwPChannel ) );
				if( (lPRSIndex > lPartRefIndex)
				||	((lPRSIndex == lPartRefIndex )
					  && (pPianoRollStrip->m_pPartRef->m_strName.CompareNoCase( pPartRef->m_strName ) > 0) ) )
				{
					fContinue = FALSE;
				}
			}
		}

		pIStrip->Release();
	}

	return dwPosition;
}

void CMIDIMgr::PreChangePartRef( CDirectMusicPartRef* pPartRef )
{
	// If other PartRef's use the part that will change
	if( pPartRef->m_pDMPart->m_dwUseCount > 1 )
	{
		// If we're not linked to the part
		if( !pPartRef->m_fHardLink )
		{
			// Create a new part
			CDirectMusicPart *pPart;
			pPart = AllocPart();
			if( pPart )
			{
				// Copy the existing part to the new part
				pPartRef->m_pDMPart->CopyTo( pPart );

				// Point ourself to the new part
				pPartRef->SetPart( pPart );
			}
		}
		// If we ARE linked to the part
		else // pPartRef->m_fHardLink IS set
		{
			// Check if all other PartRefs are also linked to our part
			BOOL fOtherHardLinkNotSet = FALSE;
			CDirectMusicPartRef *pTmpPartRef;

			// Iterate through all PartRefs
			POSITION pos;
			pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
			while( pos != NULL )
			{
				pTmpPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );

				// If this PartRef points to the same part, and does NOT
				// have the m_fHardLink set, set fOtherHardLinkNotSet and
				// break out
				if( (pTmpPartRef->m_pDMPart == pPartRef->m_pDMPart) &&
					!pTmpPartRef->m_fHardLink )
				{
					fOtherHardLinkNotSet = TRUE;
					break;
				}
			}

			// Another PartRef references this part, but does not have the
			// m_fHardLink flag set
			if( fOtherHardLinkNotSet )
			{
				// Create a new part
				CDirectMusicPart *pPart;
				pPart = AllocPart();
				if( pPart )
				{
					// Copy the existing part to the new part
					pPartRef->m_pDMPart->CopyTo( pPart );

					// Set all linked PartRefs to point to this new part
					pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
					while( pos != NULL )
					{
						pTmpPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );
						if( pTmpPartRef->m_fHardLink )
						{
							pTmpPartRef->SetPart( pPart );
						}
					}
				}
			}
		}
	}

	// Update Part's GUID
	GUID newGuid;
	if( SUCCEEDED(CoCreateGuid( &newGuid )) )
	{
		// Set m_guidOldPartID if it is unset
		if( ::IsEqualGUID( GUID_AllZeros, pPartRef->m_guidOldPartID ) )
		{
			memcpy( &pPartRef->m_guidOldPartID, &pPartRef->m_pDMPart->m_guidPartID, sizeof(GUID) );
		}

		// Just copy the new guid 
		memcpy( &pPartRef->m_pDMPart->m_guidPartID, &newGuid, sizeof(GUID) );
	}
}

long CMIDIMgr::EarliestSelectedNote( CDirectMusicPart *pPart, DWORD dwVariations )
{
	// BUGBUG: I'm not sure this works correctly
	ASSERT( pPart );
	if( !pPart )
	{
		return 0;
	}

	long lStartTime;
	long lStartBar, lStartBeat, lStartGrid, lStartTick;
	long lEarlyGrid = LONG_MAX;

	CDirectMusicEventItem* pDMEvent = pPart->m_lstNotes.GetHead();

	for( ;  pDMEvent ;  pDMEvent = pDMEvent->GetNext() )
	{
		if( (pDMEvent->m_fSelected == TRUE)
		&&	(pDMEvent->m_dwVariation & dwVariations) )
		{
			// Determine start time of note
			lStartTime = GRID_TO_CLOCKS( pDMEvent->m_mtGridStart, pPart );
			lStartTime += pDMEvent->m_nTimeOffset;
			
			// Determine grid offset of note
			CPropItem::TimeToBarBeatGridTick( pPart->m_TimeSignature, lStartTime, &lStartBar, &lStartBeat, &lStartGrid, &lStartTick );
			lStartBeat += (lStartBar * pPart->m_TimeSignature.m_bBeatsPerMeasure);
			lStartGrid += (lStartBeat * pPart->m_TimeSignature.m_wGridsPerBeat);

			// Keep track of the earliest grid
			if( lStartGrid < lEarlyGrid )
			{
				lEarlyGrid = lStartGrid;
			}
		}
	}

	ASSERT( lEarlyGrid != LONG_MAX );

	return lEarlyGrid;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::OffsetSelectedNoteStart

BOOL CMIDIMgr::OffsetSelectedNoteStart( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lMoveTick, long lMoveGrid )
{
	BOOL fChange = FALSE;

	ASSERT( pPartRef );
	if( !pPartRef )
	{
		return FALSE;
	}

	ASSERT( dwVariations && (lMoveTick || lMoveGrid) );
	if( !dwVariations || (!lMoveTick && !lMoveGrid) )
	{
		return FALSE;
	}

	// Always update the PartRef
	PreChangePartRef( pPartRef );

	// Update m_NotePropData
	m_NotePropData.m_Param.pPart = pPartRef->m_pDMPart;
	m_NotePropData.m_Param.dwVariations = dwVariations;

	// Compute the # of grid clocks
	long lGridClocks = GRID_TO_CLOCKS(1, pPartRef->m_pDMPart);

	/*
	// Compute the length of the part (in grids)
	long lPartGridLength = pPartRef->m_pDMPart->GetGridLength();
	long lPartClockLength = GRID_TO_CLOCKS( lPartGridLength, pPartRef->m_pDMPart );

	// Update lTotalOffset
	lTotalOffset += lCurOffset - lOldCurOffset;
	*/

	CDirectMusicNoteList lstMovedEvents;
	CDirectMusicEventItem *pDMEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	BOOL fGetNext;
	while( pDMEvent )
	{
		fGetNext = TRUE;
		if( pDMEvent->m_fSelected && (pDMEvent->m_dwVariation & dwVariations) )
		{
			// TODO: These could be optimized into two functions, one for lStartTick/lStartGrid < 0
			// (using GetPrev() to step through the list, and one for lStartTick/lStartGrid > 0
			// (using GetNext() to step through the list.  Of course, we don't have a quick GetPrev, so
			// this is currently a moot point.

			long mtNewGridStart = pDMEvent->m_mtGridStart + lMoveGrid;
			ASSERT( mtNewGridStart >= 0 );
			ASSERT( mtNewGridStart < pPartRef->m_pDMPart->GetGridLength() );

			long lNewDuration = ((CDirectMusicStyleNote *)pDMEvent)->m_mtDuration;

			// BUGBUG: This does not work correctly if we divide the beat into 5, 7, 9, 11, 13, etc.
			lNewDuration -= lMoveTick + lMoveGrid * lGridClocks;

			// Ensure the end time remains constant
			ASSERT( pDMEvent->m_nTimeOffset + pDMEvent->m_mtGridStart * lGridClocks + ((CDirectMusicStyleNote *)pDMEvent)->m_mtDuration
				==	pDMEvent->m_nTimeOffset + lMoveTick + lNewDuration + mtNewGridStart * lGridClocks );

			if( lMoveTick || (mtNewGridStart != pDMEvent->m_mtGridStart) )
			{
				// Duplicate the note if necessary (if the note belongs to other variations)
				pPartRef->m_pDMPart->CreateNoteCopyIfNeeded( (CDirectMusicStyleNote*)pDMEvent, dwVariations, FALSE );

				((CDirectMusicStyleNote *)pDMEvent)->m_mtDuration = lNewDuration;

				if( pDMEvent->m_mtGridStart != mtNewGridStart )
				{
					pDMEvent->m_mtGridStart = mtNewGridStart;
				
					// Remember, if a note moves, its MIDI value or MusicValue should be updated
					// from the chord at its new position
					pPartRef->UpdateNoteAfterMove( (CDirectMusicStyleNote *)pDMEvent );
				}

				mtNewGridStart = pDMEvent->m_nTimeOffset + lMoveTick;
				pDMEvent->m_nTimeOffset = short(mtNewGridStart);

				// TODO: Optimize this, if possible
				if( lMoveTick + GRID_TO_CLOCKS(lMoveGrid, pPartRef->m_pDMPart) < 0 )
				{
					// Save a pointer to the next event
					CDirectMusicEventItem *pDMEvent2 = pDMEvent->GetNext();

					// Remove the event from the part's list
					pPartRef->m_pDMPart->m_lstNotes.Remove( pDMEvent );

					// Add it to our private list
					lstMovedEvents.AddTail( pDMEvent );

					// Set the next pDMEvent, and set fGetNext to FALSE
					fGetNext = FALSE;
					pDMEvent = pDMEvent2;
				}
				// GetPrev() is very slow, so we ignore optimizing this case for now
				/*
				// If lCurOffset < 0, only add to lstMovedEvents if we're now before
				// the item before the one at pos2.
				if( lStartTime < 0 )
				{
					POSITION pos3 = pos2;
					m_lstSequences.GetPrev( pos3 );
					if( pos3 )
					{
						//CSequenceItem* pItem = m_lstSequences.GetAt( pos3 );
						if( pSequenceItem->AbsTime() < m_lstSequences.GetAt( pos3 )->AbsTime() )
						{
							m_lstSequences.RemoveAt( pos2 );
							lstMovedSequences.AddTail( pSequenceItem );
						}
					}
				}
				*/

				// If lCurOffset > 0, only add to lstMovedEvents if we're now after the next item
				else if( lMoveTick + GRID_TO_CLOCKS(lMoveGrid, pPartRef->m_pDMPart) > 0 )
				{
					CDirectMusicEventItem *pDMEvent2 = pDMEvent->GetNext();
					if( pDMEvent2
					&& (pPartRef->m_pDMPart->AbsTime( pDMEvent2 ) < pPartRef->m_pDMPart->AbsTime(pDMEvent)) )
					{
						// Remove the event from the part's list
						pPartRef->m_pDMPart->m_lstNotes.Remove( pDMEvent );

						// Add it to our private list
						lstMovedEvents.AddTail( pDMEvent );

						// Set the next pDMEvent, and set fGetNext to FALSE
						fGetNext = FALSE;
						pDMEvent = pDMEvent2;
					}
				}

				// We changed
				fChange = TRUE;
			}
		}
		if( fGetNext )
		{
			pDMEvent = pDMEvent->GetNext();
		}
	}

	if( fChange )
	{
		if( lstMovedEvents.GetHead() )
		{
			pPartRef->m_pDMPart->MergeNoteList( &lstMovedEvents );
		}

		// Invalidate all strips displaying this part
		RefreshPartDisplay( pPartRef->m_pDMPart,
							dwVariations,
							FALSE, FALSE );

		// Always recompute m_SelectedPropNote and update the property page
		ComputeSelectedPropNote( pPartRef->m_pDMPart,
								 dwVariations );
		RefreshPropertyPage( pPartRef->m_pDMPart );
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::OffsetSelectedNoteValuePosition

BOOL CMIDIMgr::OffsetSelectedNoteValuePosition( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lMoveTick, long lMoveGrid, int nMIDIValue, BOOL fMakeCopy )
{
	BOOL fChange = FALSE;

	ASSERT( pPartRef );
	if( !pPartRef )
	{
		return FALSE;
	}

	ASSERT( dwVariations && (lMoveTick || lMoveGrid || nMIDIValue) );
	if( !dwVariations || (!lMoveTick && !lMoveGrid && !nMIDIValue) )
	{
		return FALSE;
	}

	// Always update the PartRef
	PreChangePartRef( pPartRef );

	// Update m_NotePropData
	m_NotePropData.m_Param.pPart = pPartRef->m_pDMPart;
	m_NotePropData.m_Param.dwVariations = dwVariations;

	CDirectMusicNoteList lstMovedEvents;
	CDirectMusicEventItem *pDMEvent = pPartRef->m_pDMPart->m_lstNotes.GetHead();
	BOOL fGetNext;
	while( pDMEvent )
	{
		fGetNext = TRUE;
		if( pDMEvent->m_fSelected && (pDMEvent->m_dwVariation & dwVariations) )
		{
			bool fMakeCopyThisNote = true;

			if( nMIDIValue != 0 )
			{
				// Get the new MIDI Value
				int nNewValue = ((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue;
				nNewValue += nMIDIValue;

				// Restrict it to 0-127
				if( nNewValue < 0 )
				{
					nNewValue = 0;
				}
				if( nNewValue > 127 )
				{
					nNewValue = 127;
				}

				BYTE bOldValue = ((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue;
				WORD wOldValue = ((CDirectMusicStyleNote*)pDMEvent)->m_wMusicValue;

				// Update the note's MIDI Value
				((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue = BYTE(nNewValue);

				// Calculate the new MusicValue
				((CDirectMusicStyleNote*)pDMEvent)->m_wMusicValue = pPartRef->DMNoteToMusicValue( (CDirectMusicStyleNote*)pDMEvent, DMUS_PLAYMODE_NONE );

				// The DMNoteToMusicValue conversion may not have been exact,
				// so recalculate the MIDIValue
				((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue = pPartRef->DMNoteToMIDIValue( (CDirectMusicStyleNote*)pDMEvent, DMUS_PLAYMODE_NONE );

				if( ((nMIDIValue < 0) && (((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue > bOldValue))
				||	((nMIDIValue > 0) && (((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue < bOldValue)) )
				{
					// Note went the wrong way - ignore the change
					((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue = bOldValue;
					((CDirectMusicStyleNote*)pDMEvent)->m_wMusicValue = wOldValue;
				}
				// Update the note, if it changed
				else if( (bOldValue != ((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue)
					 ||	 (wOldValue != ((CDirectMusicStyleNote*)pDMEvent)->m_wMusicValue) )
				{
					BYTE bNewValue = ((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue;
					((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue = bOldValue;
					WORD wNewValue = ((CDirectMusicStyleNote*)pDMEvent)->m_wMusicValue;
					((CDirectMusicStyleNote*)pDMEvent)->m_wMusicValue = wOldValue;

					// Duplicate the note if necessary (if the note belongs to other variations, or if fMakeCopy is set)
					pPartRef->m_pDMPart->CreateNoteCopyIfNeeded( (CDirectMusicStyleNote*)pDMEvent, dwVariations, fMakeCopy );
					fMakeCopyThisNote = false;

					// Update the note's MIDI Value and MusicValue
					((CDirectMusicStyleNote*)pDMEvent)->m_bMIDIValue = bNewValue;
					((CDirectMusicStyleNote*)pDMEvent)->m_wMusicValue = wNewValue;

					// We changed
					fChange = TRUE;
				}
			}

			// TODO: These could be optimized into two functions, one for lMoveTick/lMoveGrid < 0
			// (using GetPrev() to step through the list, and one for lMoveTick/lMoveGrid > 0
			// (using GetNext() to step through the list.  Of course, we don't have a quick GetPrev, so
			// this is currently a moot point.
			if( lMoveTick || lMoveGrid )
			{
				long lNewGrid = pDMEvent->m_mtGridStart + lMoveGrid;

				if( lNewGrid < 0 )
				{
					lNewGrid = 0;
				}
				if( lNewGrid >= pPartRef->m_pDMPart->GetGridLength() )
				{
					lNewGrid = pPartRef->m_pDMPart->GetGridLength() - 1;
				}
				if( lMoveTick || (lNewGrid != pDMEvent->m_mtGridStart) )
				{
					// Duplicate the note if necessary (if the note belongs to other variations, or if fMakeCopy is set)
					pPartRef->m_pDMPart->CreateNoteCopyIfNeeded( (CDirectMusicStyleNote*)pDMEvent, dwVariations, fMakeCopyThisNote && fMakeCopy );

					if( pDMEvent->m_mtGridStart != lNewGrid )
					{
						pDMEvent->m_mtGridStart = lNewGrid;
					
						// Remember, if a note moves, its MIDI value or MusicValue should be updated
						// from the chord at its new position
						pPartRef->UpdateNoteAfterMove( (CDirectMusicStyleNote *)pDMEvent );
					}

					lNewGrid = pDMEvent->m_nTimeOffset + lMoveTick;
					pDMEvent->m_nTimeOffset = short(lNewGrid);

					// TODO: Optimize this, if possible
					if( lMoveTick + GRID_TO_CLOCKS(lMoveGrid, pPartRef->m_pDMPart) < 0 )
					{
						// Save a pointer to the next event
						CDirectMusicEventItem *pDMEvent2 = pDMEvent->GetNext();

						// Remove the event from the part's list
						pPartRef->m_pDMPart->m_lstNotes.Remove( pDMEvent );

						// Add it to our private list
						lstMovedEvents.AddTail( pDMEvent );

						// Set the next pDMEvent, and set fGetNext to FALSE
						fGetNext = FALSE;
						pDMEvent = pDMEvent2;
					}
					// GetPrev() is very slow, so we ignore optimizing this case for now
					/*
					// If lStartTime < 0, only add to lstMovedEvents if we're now before
					// the item before the one at pos2.
					if( lStartTime < 0 )
					{
						POSITION pos3 = pos2;
						m_lstSequences.GetPrev( pos3 );
						if( pos3 )
						{
							//CSequenceItem* pItem = m_lstSequences.GetAt( pos3 );
							if( pSequenceItem->AbsTime() < m_lstSequences.GetAt( pos3 )->AbsTime() )
							{
								m_lstSequences.RemoveAt( pos2 );
								lstMovedSequences.AddTail( pSequenceItem );
							}
						}
					}
					*/

					// If lStartTime > 0, only add to lstMovedEvents if we're now after the next item
					else if( lMoveTick + GRID_TO_CLOCKS(lMoveGrid, pPartRef->m_pDMPart) > 0 )
					{
						CDirectMusicEventItem *pDMEvent2 = pDMEvent->GetNext();
						if( pDMEvent2
						&& (pPartRef->m_pDMPart->AbsTime( pDMEvent2 ) < pPartRef->m_pDMPart->AbsTime(pDMEvent)) )
						{
							// Remove the event from the part's list
							pPartRef->m_pDMPart->m_lstNotes.Remove( pDMEvent );

							// Add it to our private list
							lstMovedEvents.AddTail( pDMEvent );

							// Set the next pDMEvent, and set fGetNext to FALSE
							fGetNext = FALSE;
							pDMEvent = pDMEvent2;
						}
					}

					// We changed
					fChange = TRUE;
				}
			}
		}
		if( fGetNext )
		{
			pDMEvent = pDMEvent->GetNext();
		}
	}

	if( fChange )
	{
		if( lstMovedEvents.GetHead() )
		{
			pPartRef->m_pDMPart->MergeNoteList( &lstMovedEvents );
		}

		// Invalidate all strips displaying this part
		RefreshPartDisplay( pPartRef->m_pDMPart,
							dwVariations,
							FALSE, FALSE );

		// Always recompute m_SelectedPropNote and update the property page
		ComputeSelectedPropNote( pPartRef->m_pDMPart,
								 dwVariations );
		RefreshPropertyPage( pPartRef->m_pDMPart );
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::OffsetSelectedNoteDurationAndVelocity

BOOL CMIDIMgr::OffsetSelectedNoteDurationAndVelocity( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lDuration, long lVelocity, BOOL fMakeCopy )
{
	BOOL fChange = FALSE;

	ASSERT( pPartRef );
	if( !pPartRef )
	{
		return FALSE;
	}

	ASSERT( dwVariations || lDuration || lVelocity );
	if( !dwVariations && !lDuration && !lVelocity )
	{
		return FALSE;
	}

	// Always update the PartRef
	PreChangePartRef( pPartRef );

	// Update m_NotePropData
	m_NotePropData.m_Param.pPart = pPartRef->m_pDMPart;
	m_NotePropData.m_Param.dwVariations = dwVariations;

	CDirectMusicEventItem *pDMEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	BOOL fGetNext;
	while( pDMEvent )
	{
		fGetNext = TRUE;
		if( pDMEvent->m_fSelected && (pDMEvent->m_dwVariation & dwVariations) )
		{
			if( lDuration != 0 )
			{
				long lNewValue = ((CDirectMusicStyleNote*)pDMEvent)->m_mtDuration + lDuration;

				if( lNewValue < 1 )
				{
					lNewValue = 1;
				}
				/*  No effect, since lNewValue is a long to begin with
				if( lNewValue > LONG_MAX )
				{
					lNewValue = LONG_MAX;
				}
				*/
				if( lNewValue != ((CDirectMusicStyleNote*)pDMEvent)->m_mtDuration )
				{
					// Duplicate the note if necessary (if the note belongs to other variations, or if fMakeCopy is set)
					pPartRef->m_pDMPart->CreateNoteCopyIfNeeded( (CDirectMusicStyleNote*)pDMEvent, dwVariations, fMakeCopy );

					// Update the note's duration
					((CDirectMusicStyleNote*)pDMEvent)->m_mtDuration = lNewValue;

					// We changed
					fChange = TRUE;
				}
			}

			if( lVelocity != 0 )
			{
				long lNewValue = ((CDirectMusicStyleNote*)pDMEvent)->m_bVelocity + lVelocity;

				if( lNewValue < 1 )
				{
					lNewValue = 1;
				}
				if( lNewValue > 127 )
				{
					lNewValue = 127;
				}
				if( lNewValue != ((CDirectMusicStyleNote*)pDMEvent)->m_bVelocity )
				{
					// Duplicate the note if necessary (if the note belongs to other variations, or if fMakeCopy is set)
					pPartRef->m_pDMPart->CreateNoteCopyIfNeeded( (CDirectMusicStyleNote*)pDMEvent, dwVariations, fMakeCopy );

					// Update the note's velocity
					((CDirectMusicStyleNote*)pDMEvent)->m_bVelocity = (BYTE)lNewValue;

					// We changed
					fChange = TRUE;
				}
			}
		}
		if( fGetNext )
		{
			pDMEvent = pDMEvent->GetNext();
		}
	}

	if( fChange )
	{
		// Invalidate all strips displaying this part
		RefreshPartDisplay( pPartRef->m_pDMPart,
							dwVariations,
							FALSE, FALSE );

		// Always recompute m_SelectedPropNote and update the property page
		ComputeSelectedPropNote( pPartRef->m_pDMPart,
								 dwVariations );
		RefreshPropertyPage( pPartRef->m_pDMPart );
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::UpdateFlatsAndKey

BOOL CMIDIMgr::UpdateFlatsAndKey()
{
	BOOL fChange = FALSE;

	// Get the show Flats/Sharps flag
	BOOL fShowFlats = FALSE;
	if( m_pTimeline && SUCCEEDED( m_pTimeline->GetParam( GUID_ChordSharpsFlats, 0xFFFFFFFF, 0, 0, NULL, &fShowFlats) ) )
	{
		if( m_fDisplayingFlats != fShowFlats )
		{
			m_fDisplayingFlats = fShowFlats;
			fChange = TRUE;
		}
	}

	DWORD dwKey = 0;
	if( m_pTimeline &&
		SUCCEEDED( m_pTimeline->GetParam( GUID_ChordKey, 0xFFFFFFFF, 0, 0, NULL, &dwKey) ) )
	{
		int nNewKeyRoot, nNewNumAccidentals;
		nNewKeyRoot = dwKey & 0x0000FFFF;
		nNewKeyRoot %= 12;
		nNewNumAccidentals = (dwKey & 0xFFFF0000) >> 16;
		nNewNumAccidentals = min( nNewNumAccidentals, 7 );
		if( nNewKeyRoot != m_nKeyRoot )
		{
			m_nKeyRoot = nNewKeyRoot;
			fChange = TRUE;
		}
		if( nNewNumAccidentals != m_nNumAccidentals )
		{
			m_nNumAccidentals = nNewNumAccidentals;
			fChange = TRUE;
		}
	}

	if( fChange )
	{
		// Update KeyPattern
		UpdateKeyPattern();

		// Refresh all strips displaying in hybrid notation
		POSITION position = m_pPRSList.GetHeadPosition();
		while (position != NULL)
		{
			CPianoRollStrip* pPianoRollStrip = m_pPRSList.GetNext(position);
			if ( pPianoRollStrip->m_fHybridNotation )
			{
				// Invalidate the entire strip, since the key may have changed
				m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)pPianoRollStrip, NULL, FALSE );
			}
		}

		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}

	return fChange;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::UpdateKeyPattern

void CMIDIMgr::UpdateKeyPattern( void )
{
	ZeroMemory( m_aiScalePattern, sizeof(int) * 7 );
	if( m_fDisplayingFlats )
	{
		// Set the # of flats
		for( int i=0; i < m_nNumAccidentals; i++ )
		{
			// Cirlce of fourths
			m_aiScalePattern[(6 + i * 3) % 7] = -1;
		}

		// Set the Key Root to the chosen accidental
		// (Say they want 3b with key of E - then this resets Eb back to Enat)
		m_aiScalePattern[aChromToScaleFlats[m_nKeyRoot]] = aChromToFlatAccidentals[m_nKeyRoot];
	}
	else
	{
		// Set the # of sharps
		for( int i=0; i < m_nNumAccidentals; i++ )
		{
			// Cirlce of fifths
			m_aiScalePattern[(3 + i * 4) % 7] = 1;
		}

		// Set the Key Root to the chosen accidental
		// (Say they want 2# with key of F - then this resets F# back to Fnat)
		m_aiScalePattern[aChromToScaleSharps[m_nKeyRoot]] = aChromToSharpAccidentals[m_nKeyRoot];
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::ChangeNotationType

void CMIDIMgr::ChangeNotationType( BOOL fHybridNotation )
{
	POSITION pos = m_pPRSList.GetHeadPosition();
	while( pos )
	{
		CPianoRollStrip *pStrip = m_pPRSList.GetNext( pos );

		// If not a drum track, update it
		if( (pStrip->m_pPartRef->m_dwPChannel & 0xF) != 9 )
		{
			pStrip->ChangeNotationType( fHybridNotation, FALSE );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::DeleteEvent

CDirectMusicEventItem *CMIDIMgr::DeleteEvent( CDirectMusicEventItem* pEvent, AList &lstEvents, DWORD dwVariations )
{
	if (pEvent->m_dwVariation & ~dwVariations)
	{
		// Event will still belong to at least 1 variation, keep it
		pEvent->m_dwVariation &= ~dwVariations;
		return pEvent->GetNext();
	}
	else
	{
		// Event would belong to 0 variations, delete it
		CDirectMusicEventItem* pEventNext = pEvent->GetNext();
		lstEvents.Remove(pEvent);
		delete pEvent;
		return pEventNext;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::DeleteEventsBetweenBoundaries

void CMIDIMgr::DeleteEventsBetweenBoundaries( AList &lstEvents, DWORD dwVariations, long lGridStart, long lGridEnd )
{
	// Find the first event in dwVariations
	CDirectMusicEventItem* pEvent = (CDirectMusicEventItem*)lstEvents.GetHead();
	while( pEvent && !(pEvent->m_dwVariation & dwVariations) )
	{
		pEvent = pEvent->GetNext();
	}

	while (pEvent != NULL)
	{
		if ( (pEvent->m_dwVariation & dwVariations)
		&&	 pEvent->m_mtGridStart >= lGridStart
		&&	 pEvent->m_mtGridStart <= lGridEnd )
		{
			pEvent = DeleteEvent( pEvent, lstEvents, dwVariations );
		}
		else
		{
			pEvent = pEvent->GetNext();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::DeleteNotesBetweenBoundaries

void CMIDIMgr::DeleteNotesBetweenBoundaries( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, long lGridEnd, BOOL &fChanged )
{
	// Delete all notes that start between lBeginTime and lEnd, inclusive

	// Need to look through the list to see if we have anything to change
	CDirectMusicEventItem* pEvent = pPartRef->m_pDMPart->GetFirstNote( dwVariations );
	while( !fChanged && pEvent )
	{
		if ( (pEvent->m_dwVariation & dwVariations)
		&&	 pEvent->m_mtGridStart >= lGridStart
		&&	 pEvent->m_mtGridStart <= lGridEnd )
		{
			// Call PreChangePartRef since this is the first change
			fChanged = TRUE;
			PreChangePartRef( pPartRef );
			break;
		}
		pEvent = pEvent->GetNext();
	}

	DeleteEventsBetweenBoundaries( pPartRef->m_pDMPart->m_lstNotes, dwVariations, lGridStart, lGridEnd );

	ValidateActiveDMNote( pPartRef->m_pDMPart, dwVariations );
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::DeleteCurvesBetweenBoundaries

void CMIDIMgr::DeleteCurvesBetweenBoundaries( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, long lGridEnd, BOOL &fChanged )
{
	// Delete all curves that start between lGridStart and lGridEnd, inclusive

	// Need to look through the list to see if we have anything to change
	CDirectMusicEventItem* pEvent = pPartRef->m_pDMPart->GetFirstCurve( dwVariations );
	while( !fChanged && pEvent )
	{
		if ( (pEvent->m_dwVariation & dwVariations)
		&&	 pEvent->m_mtGridStart >= lGridStart
		&&	 pEvent->m_mtGridStart <= lGridEnd )
		{
			// Call PreChangePartRef since this is the first change
			fChanged = TRUE;
			PreChangePartRef( pPartRef );
			break;
		}
		pEvent = pEvent->GetNext();
	}

	DeleteEventsBetweenBoundaries( pPartRef->m_pDMPart->m_lstCurves, dwVariations, lGridStart, lGridEnd );
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::DeleteMarkersBetweenBoundaries

void CMIDIMgr::DeleteMarkersBetweenBoundaries( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, long lGridEnd, BOOL &fChanged )
{
	// Delete all markers that fall between lGridStart and lGridEnd, inclusive

	// Need to look through the list to see if we have anything to change
	CDirectMusicEventItem *pEvent = pPartRef->m_pDMPart->GetFirstMarker( dwVariations );
	while( !fChanged && pEvent )
	{
		if( pEvent->m_mtGridStart >= lGridStart
		&&	pEvent->m_mtGridStart <= lGridEnd
		&&	MARKER_AND_VARIATION(reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), dwVariations) )
		{
			// Call PreChangePartRef since this is the first change
			fChanged = TRUE;
			PreChangePartRef( pPartRef );
			break;
		}
		pEvent = pEvent->GetNext();
	}

	pEvent = pPartRef->m_pDMPart->GetFirstMarker( dwVariations );
	while( pEvent )
	{
		if( pEvent->m_mtGridStart >= lGridStart
		&&	pEvent->m_mtGridStart <= lGridEnd
		&&	MARKER_AND_VARIATION(reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), dwVariations) )
		{
			// Remove the marker from the displayed variations
			// Update the current item to point to the next marker
			pEvent = pPartRef->m_pDMPart->m_lstMarkers.RemoveMarkerFromVariations( reinterpret_cast<CDirectMusicStyleMarker*>(pEvent), dwVariations );
		}
		else
		{
			pEvent = pEvent->GetNext();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::UpdatePositionOfStrips

void CMIDIMgr::UpdatePositionOfStrips( const CDirectMusicPartRef* pPartRef )
{
	// Update all strips using this PartRef in the Timeline
	DWORD dwEnum = 0;
	IDMUSProdStrip* pIStrip;
	CLSID clsid;
	ioPianoRollDesign iPianoRollDesign;
	VARIANT var;

	// Ensures the PianoRollStrip's, CurveStrip's, and VarSwitchStrip's handler for WM_SETFOCUS
	// don't update the Style Designer component.
	m_pDMPattern->m_fInLoad = TRUE;

	while( SUCCEEDED( m_pTimeline->EnumStrip( ++dwEnum, &pIStrip ) ) )
	{
		var.vt = VT_BYREF;
		V_BYREF(&var) = &clsid;

		if( SUCCEEDED ( pIStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &var ) ) )
		{
			if( ::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
			{
				CPianoRollStrip* pPianoRollStrip = (CPianoRollStrip *)pIStrip;
				if( pPianoRollStrip->m_pPartRef == pPartRef && !pPianoRollStrip->m_fInSetData )
				{
					// Update the strip's position in the Timeline
					// Flag that we've moved this strip
					pPianoRollStrip->m_fInSetData = TRUE;
					// Save the strip's design info
					pPianoRollStrip->GetDesignInfo( &iPianoRollDesign );

					// Remove its curve strips
					if( pPianoRollStrip->m_StripView == SV_NORMAL )
					{
						CCurveStrip *pCurveStrip;
						while( !pPianoRollStrip->m_lstCurveStrips.IsEmpty() )
						{
							pCurveStrip = pPianoRollStrip->m_lstCurveStrips.RemoveHead();

							// Remove Curve Strip from the Timeline
							m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pCurveStrip );

							// Keep selection states
							//pCurveStrip->SelectAllCurves( FALSE );
							pCurveStrip->Release();
						}

						// Remove its VarSwitchStrip
						m_pTimeline->RemoveStrip( (IDMUSProdStrip *)pPianoRollStrip->m_pVarSwitchStrip );
					}

					// Remove the strip from the timeline
					m_pTimeline->RemoveStrip( pIStrip );

					// Insert the strip at its new position
					DWORD dwPosition = 0;
					if( m_pIDMTrack )
					{
						// Insert the strip
						m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)pPianoRollStrip, CLSID_DirectMusicPatternTrack, m_dwGroupBits, PChannelToStripIndex( pPartRef->m_dwPChannel ) );

						// Get its new position
						VARIANT varPosition;
						if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pPianoRollStrip, STP_POSITION, &varPosition ) ) )
						{
							dwPosition = V_I4(&varPosition);
						}
					}
					else
					{
						// Get the strip's new position
						dwPosition = DeterminePositionForPianoRollStrip( pPartRef );
						// Add it at that position
						m_pTimeline->InsertStripAtPos( pIStrip, dwPosition );
					}

					// Reset its vertical scroll to what the Timeline now has
					pPianoRollStrip->m_lVerticalScroll = 0;

					// Restore its design info
					pPianoRollStrip->SetDesignInfo( &iPianoRollDesign );

					// Make sure there is a curve strip for each type of Curve in this Part
					pPianoRollStrip->SyncCurveStripStateList();

					// Re-add its curve strips
					pPianoRollStrip->AddCurveStrips( FALSE );

					// If we moved further down the display, update dwEnum so we will get the strip
					// that just moved into our old position.
					if( dwPosition > dwEnum )
					{
						dwEnum--;
					}
				}
			}
		}
		pIStrip->Release();
	}

	// Unmark m_fInSetData
	dwEnum = 0;
	while( SUCCEEDED( m_pTimeline->EnumStrip( ++dwEnum, &pIStrip ) ) )
	{
		var.vt = VT_BYREF;
		V_BYREF(&var) = &clsid;

		if( SUCCEEDED ( pIStrip->GetStripProperty( (STRIPPROPERTY) PRIVATE_SP_CLSID, &var ) ) )
		{
			if( ::IsEqualCLSID( clsid, CLSID_PianoRollStrip ) )
			{
				CPianoRollStrip* pPianoRollStrip = (CPianoRollStrip *)pIStrip;
				if( (pPianoRollStrip->m_pPartRef == pPartRef) )
				{
					// Clear the m_fInSetData flag
					pPianoRollStrip->m_fInSetData = FALSE;
				}
			}
		}
		pIStrip->Release();
	}

	m_pDMPattern->m_fInLoad = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::AddPianoRollStripsToTimeline

void CMIDIMgr::AddPianoRollStripsToTimeline( void )
{
	// Flag that we're in a load operation
	BOOL fOldInLoad = m_pDMPattern->m_fInLoad;
	m_pDMPattern->m_fInLoad = TRUE;

	// Iterate through all Part References
	POSITION pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the part reference
		CDirectMusicPartRef *pDMPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );

		// Check if the list of PianoRollUI States is empty
		if( pDMPartRef->m_lstPianoRollUIStates.IsEmpty() )
		{
			// Yes - create a default PianoRoll strip
			CreateDefaultPianoRollStrip( pDMPartRef );
		}
		else
		{
			// No - create PianoRoll strips from the UI data
			POSITION posUI = pDMPartRef->m_lstPianoRollUIStates.GetHeadPosition();
			while( posUI )
			{
				// Get a pointer to the UI state
				PianoRollUIState* pPianoRollUIState = pDMPartRef->m_lstPianoRollUIStates.GetNext( posUI );

				// Create a memory stream to store the UI state in
				IStream* pIStream;
				if( SUCCEEDED ( m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
				{
					// Write out the UI state to the strem
					pIStream->Write( pPianoRollUIState->pPianoRollData, pPianoRollUIState->dwPianoRollDataSize, NULL );

					// Seek back to the beginning
					StreamSeek( pIStream, 0, STREAM_SEEK_SET );

					// Create a RIFFStream to pass to LoadPianoRollDesignData
					IDMUSProdRIFFStream* pIRiffStream;
					if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
					{
						// Create a fake chunk to enclose the UI data
						MMCKINFO ck;
						ck.ckid = NULL;
						ck.cksize = pPianoRollUIState->dwPianoRollDataSize + 4;
						ck.dwDataOffset = 0;
						ck.dwFlags = 0;
						ck.fccType = NULL;

						// Now, load the UI data
						LoadPianoRollDesignData( pIRiffStream, &ck, pDMPartRef );

						// Release the RIFF Stream
						pIRiffStream->Release();
					}

					// Release the IStream
					pIStream->Release();
				}
			}
		}
	}

	// Restore InLoad flag
	m_pDMPattern->m_fInLoad = fOldInLoad;
}

HRESULT CMIDIMgr::CreateDefaultPattern()
{
	if( m_pDMPattern )
	{
		return S_FALSE;
	}

	// Create an empty pattern
	m_pDMPattern = new CDirectMusicPattern(this, FALSE);
	if( m_pDMPattern == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Now, create the rhythm map - this must happen before calling m_pDMPattern->SetTimeSignature
	VARIANT varLength;
	if( m_pTimeline
	&&	SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength ) ) )
	{
		long lClocksPerMeasure = long(m_pDMPattern->m_TimeSignature.m_bBeatsPerMeasure) * (DM_PPQNx4 / long(m_pDMPattern->m_TimeSignature.m_bBeat));
		m_pDMPattern->m_wNbrMeasures = WORD((V_I4(&varLength) + (lClocksPerMeasure - 1)) / lClocksPerMeasure);
		m_pDMPattern->m_pRhythmMap = new DWORD[ m_pDMPattern->m_wNbrMeasures ];

		if( m_pDMPattern->m_pRhythmMap == NULL )
		{
			return E_OUTOFMEMORY;
		}

		ZeroMemory( m_pDMPattern->m_pRhythmMap, sizeof(DWORD) * m_pDMPattern->m_wNbrMeasures );
	}

	// Initialize the pattern's time signature.
	DMUS_TIMESIGNATURE dmTimeSig;
	if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, 0, NULL, &dmTimeSig ) ) )
	{
		DirectMusicTimeSig timeSig;
		timeSig.m_bBeat = dmTimeSig.bBeat;
		timeSig.m_bBeatsPerMeasure = dmTimeSig.bBeatsPerMeasure;
		timeSig.m_wGridsPerBeat = dmTimeSig.wGridsPerBeat;
		m_pDMPattern->SetTimeSignature( timeSig, FALSE );
	}

	// Create an empty Part
	CDirectMusicPart* pPart = AllocPart();
	if( pPart == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Initialize the part's TimeSig and length with the Pattern's
	pPart->m_TimeSignature = m_pDMPattern->m_TimeSignature;
	pPart->m_mtClocksPerBeat = DM_PPQNx4 / (long)pPart->m_TimeSignature.m_bBeat;
	pPart->m_mtClocksPerGrid = pPart->m_mtClocksPerBeat / (long)pPart->m_TimeSignature.m_wGridsPerBeat;
	pPart->m_wNbrMeasures = m_pDMPattern->m_wNbrMeasures;

	// Create a Part Reference
	CDirectMusicPartRef* pPartRef = m_pDMPattern->AllocPartRef();
	if( pPartRef == NULL )
	{
		DeletePart( pPart );
		return E_OUTOFMEMORY;
	}

	// If in a Pattern track, name the PartRef "Pattern"
	if( m_pIDMTrack )
	{
		pPartRef->m_strName.LoadString( IDS_PATTERN_TEXT );
	}
	// Otherwise, convert the PChannel # to a string
	else
	{
		CString cstrTrack;
		cstrTrack.LoadString( IDS_TRACK );
		pPartRef->m_strName.Format( cstrTrack, 1 );
	}

	// Point the PartRef at the Part
	pPartRef->SetPart( pPart );

	// A PianoRoll strip will be created for the partref in AddPianoRollStripsToTimeline
	return S_OK;
}

void CMIDIMgr::UpdateDirectMusicTrack( void )
{
	ASSERT( m_pIDMTrack );
	ASSERT( m_pDMPattern );
	if( !m_pIDMTrack || !m_pDMPattern )
	{
		return;
	}

	// Create a memory stream to store the UI state in
	IStream* pIStream;
	if( SUCCEEDED ( m_pIFramework->AllocMemoryStream(FT_RUNTIME, GUID_CurrentVersion, &pIStream) ) )
	{
		// Now, save the pattern to the stream
		if( SUCCEEDED( m_pDMPattern->Save( pIStream, TRUE ) ) )
		{
			// Seek back to the beginning
			StreamSeek( pIStream, 0, STREAM_SEEK_SET );

			// Now, load the stream into the track
			IPersistStream* pIPersistStream;
			if( SUCCEEDED( m_pIDMTrack->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
			{
				pIPersistStream->Load( pIStream );

				if( m_pDMProdSegmentNode )
				{
					IDirectMusicSegment *pSegment;
					if( SUCCEEDED( m_pDMProdSegmentNode->GetObject(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pSegment ) ) )
					{
						m_pIDMTrack->Init( pSegment );
						pSegment->Release();
					}
				}

				pIPersistStream->Release();
			}
		}
		pIStream->Release();
	}

	if( m_pSegmentState )
	{
		if( SUCCEEDED ( m_pIFramework->AllocMemoryStream(FT_RUNTIME, GUID_SinglePattern, &pIStream) ) )
		{
			// Now, save the pattern to the stream
			if( SUCCEEDED( m_pDMPattern->Save( pIStream, TRUE ) ) )
			{
				// Seek back to the beginning
				StreamSeek( pIStream, 0, STREAM_SEEK_SET );

				// Now, load the stream into the track
				IPrivatePatternTrack *pDMPatternTrack;
				if( SUCCEEDED( m_pIDMTrack->QueryInterface( IID_IPrivatePatternTrack, (void**)&pDMPatternTrack ) ) )
				{
					// Now, update the pattern track with the new data
					DWORD dwTemp;
					pDMPatternTrack->SetPattern( m_pSegmentState, pIStream, &dwTemp );
					pDMPatternTrack->Release();
				}
			}
			pIStream->Release();
		}
	}
}

void CMIDIMgr::InvalidateAllStrips( void )
{
	// Refresh all our strips
	POSITION pos = m_pPRSList.GetHeadPosition();
	while( pos )
	{
		CPianoRollStrip *pPRS = m_pPRSList.GetNext( pos );
		pPRS->InvalidatePianoRoll();
		pPRS->InvalidateCurveStrips();
		pPRS->InvalidateMarkerStrip();
	}
}

void CMIDIMgr::UpdateChordTrackEditorMode( void )
{
	// Double-check to make sure the PatternEditorMode is set
	BYTE bPatternEditorMode;
	if( m_pIDMTrack && m_pTimeline
	&&	(S_OK == m_pTimeline->GetParam( GUID_PatternEditorMode, m_dwGroupBits, 0, 0, NULL, &bPatternEditorMode )) )
	{
		if( bPatternEditorMode == 0xFF )
		{
			ASSERT( m_bTempPatternEditorMode != 0xFF );
			bPatternEditorMode = m_bTempPatternEditorMode;
			m_pTimeline->SetParam( GUID_PatternEditorMode, m_dwGroupBits, 0, 0, &bPatternEditorMode );
		}
	}
}

bool CMIDIMgr::ParentNotePPGShowing( void )
{
	bool fResult = false;

	// Get a pointer to our hosting node
	VARIANT var;
	if( SUCCEEDED(m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &var )) )
	{
		// QI for the IDMUSProdPropPageObject interface
		IDMUSProdPropPageObject* pIDMUSProdPropPageObject;
		if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IDMUSProdPropPageObject, (void**)&pIDMUSProdPropPageObject ) ) )
		{
			if( m_pIFramework )
			{
				// Get a pointer to the property sheet
				IDMUSProdPropSheet* pIPropSheet;
				if( SUCCEEDED( m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet) ) )
				{
					// Now, see if the nosting node is displaying its PPG
					if( pIPropSheet->IsEqualPageManagerObject( pIDMUSProdPropPageObject ) == S_OK )
					{
						// Yes
						fResult = true;
					}
					pIPropSheet->Release();
				}
			}
			pIDMUSProdPropPageObject->Release();
		}
		V_UNKNOWN(&var)->Release();
	}

	return fResult;
}

// Helper function for RealTime display

void RefTimeToString( REFERENCE_TIME rtTime, int nResourceID, int nNegativeResourceID, CString &cstrTime )
{
	bool fNegative = false;
	if( rtTime < 0 )
	{
		fNegative = true;
		rtTime = -rtTime;
	}

	int iMillisecond, iSecond, iMinute;//, iHour;
	// Convert to milliseconds
	iMillisecond = int(rtTime / 10000);
	iSecond = iMillisecond / 1000;
	iMillisecond %= 1000;
	iMinute = iSecond / 60;
	iSecond %= 60;
	//iHour = iMinute / 60;
	//iMinute %= 60;

	CString strFormat;
	if( strFormat.LoadString( fNegative ? nNegativeResourceID : nResourceID ) )
	{
		cstrTime.Format(strFormat, iMinute, iSecond, iMillisecond);
	}
	else
	{
		cstrTime.Format(fNegative ? "-%02d:%02d.%03d" : "%02d:%02d.%03d", iMinute, iSecond, iMillisecond);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::UpdateStatusBarDisplay

void CMIDIMgr::UpdateStatusBarDisplay( void )
{
	bool fShowStatusBar = false;
	if( m_pActivePianoRollStrip )
	{
		switch( m_iFocus )
		{
		case 1:
			if( m_pActivePianoRollStrip->m_pActiveDMNote )
			{
				fShowStatusBar = true;

				if( !m_hStatusBar
				&&	SUCCEEDED( m_pIFramework->SetNbrStatusBarPanes( 1, SBLS_CONTROL, &m_hStatusBar ) ) )
				{
					m_pIFramework->SetStatusBarPaneInfo( m_hStatusBar, 0, SBS_SUNKEN, 14 );
				}

				if( m_hStatusBar )
				{
					REFERENCE_TIME rtNote;
					if( SUCCEEDED( m_pTimeline->ClocksToRefTime( m_pActivePianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( m_pActivePianoRollStrip->m_pActiveDMNote ),
																 &rtNote ) ) )
					{
						CString strText;
						RefTimeToString( rtNote, IDS_NOTE_STATUS_TEXT, IDS_NOTE_NEG_STATUS_TEXT, strText );
						m_pIFramework->SetStatusBarPaneText( m_hStatusBar, 0, strText.AllocSysString(), TRUE );
					}
				}
			}
			break;

		case 2:
			if( m_pActivePianoRollStrip->m_pActiveCurveStrip )
			{
				CDirectMusicStyleCurve *pCurve = m_pActivePianoRollStrip->m_pActiveCurveStrip->GetEarliestSelectedCurve();
				if( pCurve )
				{
					fShowStatusBar = true;

					if( !m_hStatusBar
					&&	SUCCEEDED( m_pIFramework->SetNbrStatusBarPanes( 1, SBLS_CONTROL, &m_hStatusBar ) ) )
					{
						m_pIFramework->SetStatusBarPaneInfo( m_hStatusBar, 0, SBS_SUNKEN, 15 );
					}

					if( m_hStatusBar )
					{
						REFERENCE_TIME rtNote;
						if( SUCCEEDED( m_pTimeline->ClocksToRefTime( m_pActivePianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pCurve ),
																	 &rtNote ) ) )
						{
							CString strText;
							RefTimeToString( rtNote, IDS_CURVE_STATUS_TEXT, IDS_CURVE_NEG_STATUS_TEXT, strText );
							m_pIFramework->SetStatusBarPaneText( m_hStatusBar, 0, strText.AllocSysString(), TRUE );
						}
					}
				}
			}
			break;

		case 3:
			if( m_pActivePianoRollStrip->m_pVarSwitchStrip )
			{
				CDirectMusicStyleMarker *pMarker = m_pActivePianoRollStrip->m_pVarSwitchStrip->GetEarliestSelectedMarker();
				if( pMarker )
				{
					fShowStatusBar = true;

					if( !m_hStatusBar
					&&	SUCCEEDED( m_pIFramework->SetNbrStatusBarPanes( 1, SBLS_CONTROL, &m_hStatusBar ) ) )
					{
						m_pIFramework->SetStatusBarPaneInfo( m_hStatusBar, 0, SBS_SUNKEN, 19 );
					}

					if( m_hStatusBar )
					{
						REFERENCE_TIME rtNote;
						if( SUCCEEDED( m_pTimeline->ClocksToRefTime( m_pActivePianoRollStrip->m_pPartRef->m_pDMPart->AbsTime( pMarker ),
																	 &rtNote ) ) )
						{
							CString strText;
							RefTimeToString( rtNote, IDS_VARSWITCH_STATUS_TEXT, IDS_VARSWITCH_NEG_STATUS_TEXT, strText );
							m_pIFramework->SetStatusBarPaneText( m_hStatusBar, 0, strText.AllocSysString(), TRUE );
						}
					}
				}
			}
		}
	}


	if( !fShowStatusBar
	&&	m_hStatusBar )
	{
		m_pIFramework->RestoreStatusBar( m_hStatusBar );
		m_hStatusBar = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::RefreshVarChoicesEditor

void CMIDIMgr::RefreshVarChoicesEditor( const CDirectMusicPart* pPart )
{
	// Find all PartRefs that use this part
	POSITION pos = m_pDMPattern->m_lstPartRefs.GetHeadPosition();
	while( pos != NULL )
	{
		CDirectMusicPartRef* pPartRef = m_pDMPattern->m_lstPartRefs.GetNext( pos );

		if( pPartRef->m_pVarChoicesNode
		&&	pPartRef->m_pDMPart == pPart )
		{
			pPartRef->InitializeVarChoicesEditor();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::SetVarsForPartPChannel

void CMIDIMgr::SetVarsForPartPChannel( const DWORD dwVariations, const CDirectMusicPartRef *pPartRef )
{
	const DWORD dwPChannel = pPartRef->m_dwPChannel;
	const CDirectMusicPart *pPart = pPartRef->m_pDMPart;
	POSITION position = m_pPRSList.GetHeadPosition();
	while( position != NULL )
	{
		CPianoRollStrip* pPianoRollStrip = m_pPRSList.GetNext(position);

		if( (dwPChannel == pPianoRollStrip->m_pPartRef->m_dwPChannel)
		&&	(pPart == pPianoRollStrip->m_pPartRef->m_pDMPart) )
		{
			pPianoRollStrip->SetSelectedVariations( dwVariations );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::PopulateMelodyLoadStructureList

void CMIDIMgr::PopulateMelodyLoadStructureList( void )
{
	POSITION position = m_pPRSList.GetHeadPosition();
	while( position != NULL )
	{
		CPianoRollStrip* pPianoRollStrip = m_pPRSList.GetNext(position);

		MelodyLoadStructure *pMelodyLoadStructure = NULL;

		POSITION posMLSList = m_lstMelodyLoadStructures.GetHeadPosition();
		while( posMLSList )
		{
			MelodyLoadStructure *pTmpMelodyLoadStructure = m_lstMelodyLoadStructures.GetNext( posMLSList );
			if( pPianoRollStrip->m_pPartRef->m_dwPChannel == pTmpMelodyLoadStructure->dwPChannel )
			{
				pMelodyLoadStructure = pTmpMelodyLoadStructure;
				break;
			}
		}

		if( !pMelodyLoadStructure )
		{
			pMelodyLoadStructure = new MelodyLoadStructure;
			if( pMelodyLoadStructure )
			{
				pMelodyLoadStructure->dwPChannel = pPianoRollStrip->m_pPartRef->m_dwPChannel;
				pPianoRollStrip->GetDesignInfo( &pMelodyLoadStructure->prdDesign );
				m_lstMelodyLoadStructures.AddTail( pMelodyLoadStructure );
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::ValidateActiveDMNote

void CMIDIMgr::ValidateActiveDMNote( CDirectMusicPart* pPart, DWORD dwVariations )
{
	POSITION pos = m_pPRSList.GetHeadPosition();
	while( pos )
	{
		CPianoRollStrip *pPianoRollStrip = m_pPRSList.GetNext( pos );
		if( (pPianoRollStrip->m_pPartRef->m_pDMPart == pPart)
		&&	pPianoRollStrip->m_pActiveDMNote )
		{
			if( !pPart->IsValidNote( pPianoRollStrip->m_pActiveDMNote ) )
			{
				pPianoRollStrip->m_pActiveDMNote = NULL;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr::UpdateAuditionVariations

void CMIDIMgr::UpdateAuditionVariations( void )
{
	// Always call SetVariationByGUID, even if m_pSegmentState or m_pActivePianoRollStrip are NULL
	if( m_pIDMTrack )
	{
		IPrivatePatternTrack* pIDMPrivatePatternTrack;
		IPrivatePatternTrack9* pIDMPrivatePatternTrack9;
		if( SUCCEEDED ( m_pIDMTrack->QueryInterface( IID_IPrivatePatternTrack9, (void **)&pIDMPrivatePatternTrack9 ) ) )
		{
			// If there is no active strip, or the active strip has no selected variations,
			// then disable the audition functionality by passing GUID_AllZeros
			GUID guidPartID = GUID_AllZeros;
			if( m_pActivePianoRollStrip
			&&	m_pActivePianoRollStrip->m_dwVariations )
			{
				guidPartID = m_pActivePianoRollStrip->m_pPartRef->m_pDMPart->m_guidPartID;
			}
			pIDMPrivatePatternTrack9->SetVariationMaskByGUID( m_pSegmentState,
				m_pActivePianoRollStrip ? m_pActivePianoRollStrip->m_dwVariations : 0xFFFFFFFF,
				guidPartID,
				m_pActivePianoRollStrip ? m_pActivePianoRollStrip->m_pPartRef->m_dwPChannel : 0 );

			RELEASE( pIDMPrivatePatternTrack9 );
		}
		else if( SUCCEEDED ( m_pIDMTrack->QueryInterface( IID_IPrivatePatternTrack, (void **)&pIDMPrivatePatternTrack ) ) )
		{
			pIDMPrivatePatternTrack->SetVariationByGUID( m_pSegmentState,
				m_pActivePianoRollStrip ? m_pActivePianoRollStrip->m_dwVariations : 0,
				m_pActivePianoRollStrip ? m_pActivePianoRollStrip->m_pPartRef->m_pDMPart->m_guidPartID : GUID_AllZeros,
				m_pActivePianoRollStrip ? m_pActivePianoRollStrip->m_pPartRef->m_dwPChannel : 0 );

			RELEASE( pIDMPrivatePatternTrack );
		}
	}
	else if( m_pTimeline )
	{
		// Audition the variations from the active strip
		VARIANT var;
		if (SUCCEEDED(m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &var )))
		{
			IPatternNodePrivate *pPatternNodePrivate;
			if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IPatternNodePrivate, (void**)&pPatternNodePrivate ) ) )
			{
				pPatternNodePrivate->SetAuditionVariations( m_pActivePianoRollStrip ? m_pActivePianoRollStrip->m_dwVariations : 0,
					m_pActivePianoRollStrip ? m_pActivePianoRollStrip->m_pPartRef->m_pDMPart->m_guidPartID : GUID_AllZeros,
					m_pActivePianoRollStrip ? m_pActivePianoRollStrip->m_pPartRef->m_dwPChannel : 0 );
				pPatternNodePrivate->Release();
			}
			V_UNKNOWN(&var)->Release();
		}
	}
}
