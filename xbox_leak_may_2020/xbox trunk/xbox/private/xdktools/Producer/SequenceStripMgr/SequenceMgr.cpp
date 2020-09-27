// SequenceMgr.cpp : implementation file
//

/*--------------
@doc SEQUENCESAMPLE
--------------*/

#include "stdafx.h"
#include <stdlib.h>
#include "SequenceIO.h"
#include "SequenceStripMgr.h"
#include "SequenceMgr.h"
#include "QuantizeDlg.h"
#include "MusicTimeConverter.h"
#include "MidiFileIO.h"
#include "DLLJazzDataObject.h"
#include <RiffStrm.h>
#include <mmreg.h>
#include "SegmentGuids.h"
#include "SegmentIO.h"
#include "SeqSegmentRiff.h"
#include "NotePropPageMgr.h"
#include <PChannelName.h>
#include "GroupBitsPPG.h"
#include "NewPartDlg.h"
#include "CurveStrip.h"
#include "Templates.h"
#include "PropPageSeqTrack.h"
#include "SharedPianoRoll.h"
#include "DialogVelocity.h"
#include "BandStripMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BYTE gbChannel;
extern short gnPPQN;
extern IStream* gpTempoStream;
extern IStream* gpSysExStream;
extern IStream* gpTimeSigStream;
extern long	glTimeSig; // flag to see if MIDI import should be paying attention to time sigs.


void GetTimeSig( MUSIC_TIME mtTime, DMUS_TIMESIGNATURE *pTimeSig, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits )
{
	ASSERT( pTimeSig );
	if( pTimeline )
	{
		if( FAILED( pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, mtTime, NULL, pTimeSig ) ) )
		{
			pTimeSig->bBeatsPerMeasure = 4;
			pTimeSig->bBeat = 4;
			pTimeSig->wGridsPerBeat = 4;
		}
	}
	else
	{
		pTimeSig->bBeatsPerMeasure = 4;
		pTimeSig->bBeat = 4;
		pTimeSig->wGridsPerBeat = 4;
	}
}

static void InitializeSeqStripDesign( ioSeqStripDesign *pSeqStripDesign )
{
	ASSERT( pSeqStripDesign );
	if( !pSeqStripDesign )
	{
		return;
	}

	pSeqStripDesign->m_svView = SV_MINIMIZED;
	pSeqStripDesign->m_lVScroll = -1;
	pSeqStripDesign->m_lHeight = DEFAULT_HEIGHT;
	pSeqStripDesign->m_fHybridNotation = FALSE;
	pSeqStripDesign->m_dblVerticalZoom = 0.1;
	pSeqStripDesign->m_crUnselectedNoteColor = COLOR_DEFAULT_UNSELECTED;
	pSeqStripDesign->m_crSelectedNoteColor = COLOR_DEFAULT_SELECTED;
	pSeqStripDesign->m_crOverlappingNoteColor = COLOR_DEFAULT_OVERLAPPING;
	pSeqStripDesign->m_crAccidentalColor = COLOR_DEFAULT_ACCIDENTAL;
	pSeqStripDesign->m_dwPChannel = 0;
	pSeqStripDesign->m_dwExtraBars = 0;
	pSeqStripDesign->m_fPickupBar = FALSE;
}

int CALLBACK EnumFontFamilyCallback( const LOGFONT *pLogfont, const TEXTMETRIC *pTextMetric, DWORD dwData, LPARAM lParam )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwData);
	UNREFERENCED_PARAMETER(pTextMetric);
	ASSERT( pLogfont );

	CString strFontName;
	strFontName.LoadString(IDS_NOTATION_FONT);
	if( pLogfont && _tcscmp( pLogfont->lfFaceName, strFontName ) == 0 )
	{
		CSequenceMgr *pSequenceMgr = (CSequenceMgr *)lParam;
		ASSERT( pSequenceMgr );
		if( pSequenceMgr )
		{
			pSequenceMgr->m_fHasNotationStation = TRUE;
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr constructor/destructor 

CSequenceMgr::CSequenceMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pDMProdSegmentNode = NULL;
	m_pIDMPerformance = NULL;
	m_pIPChannelName = NULL;
	m_pIConductor = NULL;
	m_pPropPageMgr = NULL;
	m_pSequenceStrip = NULL;
	m_pIDMTrack = NULL;
	m_dwPChannel = 0;
	m_fSetPChannel = FALSE;
	m_fShuttingDown = FALSE;
	m_fRecordEnabled = FALSE;
	m_pSegmentState = NULL;
	m_fNoteInserted = FALSE;
	m_fCurveInserted = FALSE;
	m_fWindowActive = FALSE;
	m_mtCurrentStartPoint = 0;
	m_mtCurrentStartTime = 0;
	m_mtCurrentLoopStart = 0;
	m_mtCurrentLoopEnd = 0;
	m_dwCurrentMaxLoopRepeats = 0;
	m_dwCurrentLoopRepeats = 0;
	m_fUpdateDirectMusic = TRUE;
	m_hStatusBar = NULL;

	m_fDisplayingFlats = FALSE;
	m_nKeyRoot = 0;
	m_nNumAccidentals = 0;

	m_rtLastPlayNoteOffTime = 0;
	m_mtLastPlayNoteEnd = 0;
	m_bLastPlayNoteMIDIValue = 0;

	InitializeSeqStripDesign( &m_SeqStripDesign );

	// Initially we don't need to be saved
	m_fDirty = FALSE;

	// By default, belong to Group 1 only
	m_dwGroupBits = 1;
	m_dwOldGroupBits = 1;
	m_dwIndex = 0;

	// Initialize the track flags
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags = 0;

	m_strName.LoadString( IDS_STRIP_NAME );

	// Create a SequenceStrip
	m_pSequenceStrip = new CSequenceStrip(this);
	ASSERT( m_pSequenceStrip );

	// Initialize m_PropSequence
	m_PropSequence.Clear();
	m_PropSequence.m_pSequenceMgr = this;

	m_fHasNotationStation = FALSE;

	HDC hdc = ::GetDC( NULL );
	if( hdc )
	{
		// Create the font to draw the sharps and flats with
		LOGFONT lf;
		memset( &lf, 0 , sizeof(LOGFONT));
		lf.lfCharSet = SYMBOL_CHARSET;
		lf.lfPitchAndFamily = DEFAULT_PITCH;
		// If re-implemented, use IDS_NOTATION_FONT
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
}

CSequenceMgr::~CSequenceMgr()
{
	// Clean up our references
	if( m_pIPChannelName )
	{
		m_pIPChannelName->Release();
		m_pIPChannelName = NULL;
	}

	// No Release!
	/*
	if( m_pDMProdSegmentNode )
	{
		m_pDMProdSegmentNode->Release();
		m_pDMProdSegmentNode = NULL;
	}
	*/

	if( m_pIConductor )
	{
		m_pIConductor->Release();
		m_pIConductor = NULL;
	}

	if( m_pIDMPerformance )
	{
		m_pIDMPerformance->Release();
		m_pIDMPerformance = NULL;
	}

	if( m_pDMProdFramework )
	{
		m_pDMProdFramework->Release();
		m_pDMProdFramework = NULL;
	}

	if( m_pSegmentState )
	{
		m_pSegmentState->Release();;
		m_pSegmentState = NULL;
	}

	ASSERT( m_pTimeline == NULL );

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
		m_pPropPageMgr = NULL;
	}
	if( m_pIDMTrack )
	{
		m_pIDMTrack->Release();
		m_pIDMTrack = NULL;
	}
	if( m_pSequenceStrip )
	{
		m_pSequenceStrip->Release();
		m_pSequenceStrip = NULL;
	}

	// Delete all the sequences in m_lstSequences and m_lstCurves
	EmptySequenceList();
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr ISequenceMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SetPChannel

HRESULT STDMETHODCALLTYPE CSequenceMgr::SetPChannel( DWORD dwPChannel )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	// Always do this method, even if the PChannel doesn't change
	//if( m_dwPChannel != dwPChannel )
	{
		m_dwPChannel = dwPChannel;
		if( m_pSequenceStrip )
		{
			m_pSequenceStrip->UpdateName();

			// Remove and re-add the strip to make the position in the Timeline correct
			ASSERT( m_pSequenceStrip->m_cRef > 1 );
			m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pSequenceStrip );
			m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pSequenceStrip, CLSID_DirectMusicSeqTrack, m_dwGroupBits, PChannelToStripIndex( m_dwPChannel ) );

			// Initialize our UI data
			m_pSequenceStrip->ApplyUIChunk( &m_SeqStripDesign );

			// Add Curve Strips to the Timeline
			m_pSequenceStrip->AddCurveStrips();

			// Make the strip the active strip
			VARIANT var;
			var.vt = VT_UNKNOWN;
			if( SUCCEEDED( m_pSequenceStrip->QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&var)) ) ) )
			{
				m_pTimeline->SetTimelineProperty( TP_ACTIVESTRIP, var );
			}

			// Now, make the strip the top strip
			var.vt = VT_UNKNOWN;
			if( SUCCEEDED( m_pSequenceStrip->QueryInterface( IID_IUnknown, (void **)&(V_UNKNOWN(&var)) ) ) )
			{
				m_pTimeline->SetTimelineProperty( TP_TOP_STRIP, var );
			}

			// Need to set TP_FREEZE_UNDO or the segment will add an undo state for us
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );

			m_pSequenceStrip->m_nLastEdit = IDS_UNDO_ADD_PART;
			OnDataChanged();

			// Reset TP_FREEZE_UNDO
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );
		}
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SaveEventsToMIDITrack

HRESULT STDMETHODCALLTYPE CSequenceMgr::SaveEventsToMIDITrack( IStream *pStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_FALSE;

	// Always write out a track, even if it's empty
	MUSIC_TIME mtTimeOffset = 0;
	if ( m_pSequenceStrip->m_lBeginSelect != 0 )
	{
		// Snap mtTimeOffset to the nearest grid before it
		mtTimeOffset = m_pSequenceStrip->FloorTimeToGrid( m_pSequenceStrip->m_lBeginSelect, NULL );
	}

	// Create a MIDI track to store the data in
	CSMFTrack MIDITrack;

	// Offset times for sequences (for notes and curves)
	POSITION pos = m_lstSequences.GetHeadPosition();
	while(pos)
	{
		CSequenceItem* pItem = m_lstSequences.GetNext(pos);
		if(pItem->m_fSelected)
		{
			CSequenceItem* pItemNew = new CSequenceItem(*pItem);
			// Subtract off the offset to normalize the item
			// (if not normalizing, mtTimeOffset will be 0)
			pItemNew->m_mtTime -= mtTimeOffset;
			MIDITrack.AddItem(pItemNew);
		}
	}

	MUSIC_TIME mtIncrement;
	DWORD dwLastValue;
	DWORD dwValue;

	// Offset times for curves
	pos = m_lstCurves.GetHeadPosition();
	while(pos)
	{
		CCurveItem* pCurveItem = m_lstCurves.GetNext(pos);
		if(pCurveItem->m_fSelected)
		{
			// Generate MIDI events for the curve
			dwLastValue = 0xFFFFFFFF;
			pCurveItem->m_mtCurrent = 0;

			while( pCurveItem->m_mtCurrent <= pCurveItem->m_mtDuration )
			{
				// Compute value of curve at m_mtCurrent
				dwValue = pCurveItem->ComputeCurve( &mtIncrement );

				if( dwValue != dwLastValue )
				{
					dwLastValue = dwValue;

					// Place sequence item in list
					CSequenceItem* pItemNew = new CSequenceItem;

					if( pItemNew )
					{
						pItemNew->m_mtTime = pCurveItem->m_mtTime - mtTimeOffset;
						pItemNew->m_nOffset = (short)(pCurveItem->m_nOffset + (short)pCurveItem->m_mtCurrent);
						pItemNew->m_mtDuration = 1;

						switch( pCurveItem->m_bType )
						{
							case DMUS_CURVET_PBCURVE:
								pItemNew->m_bStatus = MIDI_PBEND;
								pItemNew->m_bByte1 = (BYTE)(dwValue & 0x7F);
								pItemNew->m_bByte2 = (BYTE)((dwValue >> 7) & 0x7F);
								break;
							
							case DMUS_CURVET_CCCURVE:
								pItemNew->m_bStatus = MIDI_CCHANGE;
								pItemNew->m_bByte1 = pCurveItem->m_bCCData;
								pItemNew->m_bByte2 = (BYTE)(dwValue & 0x7F);
								break;

							case DMUS_CURVET_PATCURVE:
								pItemNew->m_bStatus = MIDI_PTOUCH;
								pItemNew->m_bByte1 = pCurveItem->m_bCCData;
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
								pItemNew->m_dwMIDISaveData = (pCurveItem->m_wParamType << 16) | (dwValue & 0xFFFF);
								break;

							case DMUS_CURVET_NRPNCURVE:
								pItemNew->m_bStatus = MIDI_CCHANGE;
								pItemNew->m_bByte1 = 0xFF;
								pItemNew->m_bByte2 = DMUS_CURVET_NRPNCURVE;
								pItemNew->m_dwMIDISaveData = (pCurveItem->m_wParamType << 16) | (dwValue & 0xFFFF);
								break;

							default:
								ASSERT( 0 );	// Should not happen!
								break;
						}

						// No need to merge here - the MIDITrack does a sort when it writes the events out
						MIDITrack.AddCurveItem( pItemNew );
					}
				}

				// If increment is zero - break out of the loop
				if( mtIncrement == 0 )
				{
					break;
				}

				// Increment time offset into Curve
				pCurveItem->m_mtCurrent += mtIncrement;
			}
		}
	}

	if( m_dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS )
	{
		gbChannel = 0;
	}
	else
	{
		gbChannel = static_cast<BYTE>(m_dwPChannel & 0xF);
	}
	hr = MIDITrack.Write( pStream );

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
// CSequenceMgr::PasteEventsFromMIDITrack

HRESULT STDMETHODCALLTYPE CSequenceMgr::PasteEventsFromMIDITrack( IStream *pStream, short nPPQN, DWORD dwLength )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check for the MIDI Track header
	DWORD dwID;
    pStream->Read( &dwID, sizeof( FOURCC ), NULL );
    if( dwID != mmioFOURCC( 'M', 'T', 'r', 'k' ) )
	{
		return E_UNEXPECTED;
	}

	// Since  this is a multiple-strip paste, use the Time Cursor, since it should already have
	// been set using the snap-to value
	MUSIC_TIME mtTime;
	if (FAILED(m_pTimeline->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime )))
	{
		return E_FAIL;
	}

	// Quantize it to the nearest grid
	mtTime = m_pSequenceStrip->FloorTimeToGrid( mtTime, NULL );

	mtTime = CalculatePasteTime( mtTime );

	// Get the paste type
	TIMELINE_PASTE_TYPE tlPasteType;
	if( SUCCEEDED( m_pTimeline->GetPasteType( &tlPasteType ) )
	&&	(tlPasteType == TL_PASTE_OVERWRITE) )
	{
		// Delete sequences
		DeleteBetweenTimes( mtTime, mtTime + dwLength );

		// Delete curves
		POSITION pos2, pos;
		pos = m_lstCurves.GetHeadPosition();
		while(pos)
		{
			// Save the current position
			pos2 = pos;
			CCurveItem* pCurveItem = m_lstCurves.GetNext(pos);

			// If the curve occurs between lStart and lEnd, delete it
			if( (pCurveItem->AbsTime() >= mtTime) && (pCurveItem->AbsTime() <= mtTime + (signed)dwLength) ) 
			{
				m_lstCurves.RemoveAt( pos2 );
				delete pCurveItem;
			}
		}
	}

	// Get the size of this track
	DWORD dwSize;
	GetMLong( pStream, dwSize );

	// Initialize the MIDI import global variables
	gnPPQN = nPPQN;
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

	BOOL fChanged = (lstTrackEvent != NULL);

	// Convert the events into curve or sequence items and insert them in our list
	for(FullSeqEvent* pEvent = lstTrackEvent; pEvent; pEvent = pEvent->pNext )
	{
		pEvent->mtTime += mtTime;

		if( (Status(pEvent->bStatus) == MIDI_PBEND)
		||  (Status(pEvent->bStatus) == MIDI_PTOUCH)
		||  (Status(pEvent->bStatus) == MIDI_MTOUCH)
		||  (Status(pEvent->bStatus) == MIDI_CCHANGE) )
		{
			CCurveItem* pCurveItem = new CCurveItem( pEvent );
			if( pCurveItem )
			{
				pCurveItem->m_fSelected = TRUE;
				InsertByAscendingTime( pCurveItem );
			}
		}
		else
		{
			CSequenceItem* pItem = new CSequenceItem( pEvent );
			if( pItem )
			{
				pItem->m_fSelected = TRUE;
				InsertByAscendingTime( pItem );
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
	
	if( fChanged )
	{
		m_pSequenceStrip->SyncCurveStripStateList();
		m_pSequenceStrip->AddCurveStrips();
		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_PASTE_MIDI;
		OnDataChanged();
		m_pSequenceStrip->InvalidateStrip();
		m_pSequenceStrip->InvalidateCurveStrips();
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::GetParam

HRESULT STDMETHODCALLTYPE CSequenceMgr::GetParam(
		/* [in] */	REFGUID 	rguidType,
		/* [in] */	MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME* pmtNext,
		/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pmtNext);
	UNREFERENCED_PARAMETER(mtTime);

	ASSERT( pData != NULL );
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( rguidType, GUID_Segment_Undo_BSTR ) )
	{
		CString str;
		if( str.LoadString(m_pSequenceStrip->m_nLastEdit) )
		{
			BSTR bstr;
			try
			{
				bstr = str.AllocSysString();
			}
			catch(CMemoryException*)
			{
				return E_OUTOFMEMORY;
			}
			*(BSTR*)pData = bstr;
			return S_OK;
		}
		return E_FAIL;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_ExtensionLength ) )
	{
		m_pSequenceStrip->m_dwExtraBars = m_pSequenceStrip->GetNumExtraBars();

		// Check if there are extra bars
		if( m_pSequenceStrip->m_dwExtraBars )
		{
			// Yes - return the number of them
			*(DWORD *)pData = m_pSequenceStrip->m_dwExtraBars;
			return S_OK;
		}

		// No - return E_FAIL
		return E_FAIL;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_PickupLength ) )
	{
		m_pSequenceStrip->m_fPickupBar = m_pSequenceStrip->ShouldDisplayPickupBar();

		// Check if there is a pickup bar
		if( m_pSequenceStrip->m_fPickupBar )
		{
			// Yes - return 1
			*(DWORD *)pData = 1;
			return S_OK;
		}

		// No - return E_FAIL
		return E_FAIL;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SetParam

HRESULT STDMETHODCALLTYPE CSequenceMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	UNREFERENCED_PARAMETER(mtTime);
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		m_pDMProdSegmentNode = reinterpret_cast<IDMUSProdNode*>(pData);
		// No addref!
		//m_pDMProdSegmentNode->AddRef();
		return S_OK;
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::IsParamSupported

HRESULT STDMETHODCALLTYPE CSequenceMgr::IsParamSupported(
		/* [in] */ REFGUID		rguidType)
{
	if( ::IsEqualGUID( rguidType, GUID_Segment_Undo_BSTR )
	||	::IsEqualGUID( rguidType, GUID_DocRootNode ) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CSequenceMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwGroupBits);

	// TimeSig change
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		// Redraw our strip
		m_pSequenceStrip->InvalidateStrip();
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		UpdateFlatsAndKey();

		// Add Curve Strips to the Timeline
		m_pSequenceStrip->AddCurveStrips();
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_BandTrackChange ) )
	{
		if( m_pSequenceStrip )
		{
			m_pSequenceStrip->InvalidateFunctionBar();
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_RecordButton ) )
	{
		if( pData == NULL )
		{
			return E_POINTER;
		}

		if( m_fRecordEnabled != *((BOOL *)pData) )
		{
			m_fRecordEnabled = *((BOOL *)pData);

			// Update MIDI thruing, if necessary 
			if( m_fWindowActive && m_pSegmentState && (m_pSequenceStrip->m_iHaveFocus != 0) )
			{
				if( m_fRecordEnabled )
				{
					// Try and set up MIDI thruing
					if( m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
					{
						m_pSequenceStrip->SetPChannelThru();
					}
				}
				else
				{
					if( m_pSequenceStrip->m_fMIDIThruEnabled )
					{
						m_pSequenceStrip->CancelPChannelThru();
					}
				}
			}
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_WindowActive ) )
	{
		if( pData == NULL )
		{
			return E_POINTER;
		}

		if( m_fWindowActive != *((BOOL *)pData) )
		{
			m_fWindowActive = *((BOOL *)pData);
			if( m_pSequenceStrip->m_iHaveFocus != 0 )
			{
				if( m_fWindowActive )
				{
					m_pSequenceStrip->RegisterMidi();

					// Try and set up MIDI thruing, if necessary 
					if( m_fRecordEnabled && m_pSegmentState
					&& (m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS)
					&& (m_pSequenceStrip->m_iHaveFocus != 0) )
					{
						m_pSequenceStrip->SetPChannelThru();
					}
				}
				else
				{
					// Should never be de-activated while the strip has focus - see CSegmentDlg::Activate
					ASSERT( FALSE );
					//m_pSequenceStrip->UnRegisterMidi();
				}
			}
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_Start ) )
	{
		if( pData == NULL )
		{
			return E_POINTER;
		}

		IDirectMusicSegmentState *pSegState;
		if( SUCCEEDED( ((IUnknown *)pData)->QueryInterface( IID_IDirectMusicSegmentState, (void **) &pSegState ) ) )
		{
			pSegState->GetStartTime( &m_mtCurrentStartTime );
			pSegState->GetStartPoint( &m_mtCurrentStartPoint );
			pSegState->GetRepeats( &m_dwCurrentMaxLoopRepeats );
			m_dwCurrentLoopRepeats = 0;

			IDirectMusicSegment* pSegment;
			if( SUCCEEDED( pSegState->GetSegment( &pSegment ) ) )
			{
				pSegment->GetLoopPoints( &m_mtCurrentLoopStart, &m_mtCurrentLoopEnd );
				pSegment->Release();
			}
			m_pSegmentState = pSegState;
			// These two cancel, so no need to call them
			//m_pSegmentState->AddRef();
			//pSegState->Release();


			// Try and set up MIDI thruing, if necessary 
			if( m_fWindowActive && m_fRecordEnabled
			&&	(m_dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS)
			&&	(m_pSequenceStrip->m_iHaveFocus != 0) )
			{
				m_pSequenceStrip->SetPChannelThru();
			}
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Segment_Stop ) )
	{
		if( m_pSegmentState )
		{
			m_pSegmentState->Release();
			m_pSegmentState = NULL;
			m_mtCurrentStartTime = 0;
			m_mtCurrentStartPoint = 0;
			m_mtCurrentLoopStart = 0;
			m_mtCurrentLoopEnd = 0;
			m_dwCurrentMaxLoopRepeats = 0;
			m_dwCurrentLoopRepeats = 0;

			// Cancel MIDI thruing
			if( m_pSequenceStrip->m_fMIDIThruEnabled )
			{
				m_pSequenceStrip->CancelPChannelThru();
			}

			if( m_fNoteInserted || m_fCurveInserted )
			{
				if( m_fNoteInserted )
				{
					if( m_fCurveInserted )
					{
						m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT;
					}
					else
					{
						m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT;
					}
				}
				else // if( m_fCurveInserted )
				{
					m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT_CURVE;
				}

				// Need to reset TP_FREEZE_UNDO or the segment won't add an undo state for us
				VARIANT var;
				var.vt = VT_BOOL;
				V_BOOL(&var) = FALSE;
				m_pTimeline->SetTimelineProperty( TP_FREEZE_UNDO, var );

				CoInitialize(NULL);
				OnDataChanged( );
				CoUninitialize();
				// Handled by OnDataChanged
				//m_fNoteInserted = FALSE;
				//m_fCurveInserted = FALSE;
			}
		}

		// If sequence strip is active
		if( m_pSequenceStrip->m_iHaveFocus == 1 )
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
					PlayPatch( lTime );
				}
			}
			else
			{
				pSegState->Release();
			}
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Sequence_Color_Change ) )
	{
		ioSeqTrackPPG *pioSeqTrackPPG = static_cast<ioSeqTrackPPG *>(pData);
		if( pioSeqTrackPPG->crSelectedNoteColor != m_pSequenceStrip->m_crSelectedNoteColor ||
			pioSeqTrackPPG->crUnselectedNoteColor != m_pSequenceStrip->m_crUnselectedNoteColor ||
			pioSeqTrackPPG->crOverlappingNoteColor != m_pSequenceStrip->m_crOverlappingNoteColor ||
			pioSeqTrackPPG->crAccidentalColor != m_pSequenceStrip->m_crAccidentalColor )
		{
			m_pSequenceStrip->m_crSelectedNoteColor = pioSeqTrackPPG->crSelectedNoteColor;
			m_pSequenceStrip->m_crUnselectedNoteColor = pioSeqTrackPPG->crUnselectedNoteColor;
			m_pSequenceStrip->m_crOverlappingNoteColor = pioSeqTrackPPG->crOverlappingNoteColor;
			m_pSequenceStrip->m_crAccidentalColor = pioSeqTrackPPG->crAccidentalColor;

			m_pSequenceStrip->InvalidateStrip();
			m_pSequenceStrip->InvalidateCurveStrips();

			// Since we're in OnUpdate, this won't add a step to the Undo queue
			m_fUpdateDirectMusic = FALSE;
			OnDataChanged();
		}
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Sequence_Quantize ) )
	{
		SequenceQuantize* pSequenceQuantize = (SequenceQuantize *)pData;

		Quantize( pSequenceQuantize );
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Sequence_Velocitize ) )
	{
		SequenceVelocitize* pSequenceVelocitize = (SequenceVelocitize *)pData;

		Velocitize( pSequenceVelocitize );
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_ChordSharpsFlats )
		 ||  ::IsEqualGUID( rguidType, GUID_ChordKey ) )
	{
		UpdateFlatsAndKey();
		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_Sequence_Notation_Change ) )
	{
		if( m_pSequenceStrip
		&&	(((m_dwPChannel & 0xF) != 9) ||
			 (m_dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS)) )
		{
			// Change our notation to the specified one.
			// This method will just return if the notation type is already the desired value
			m_pSequenceStrip->ChangeNotationType( *static_cast<BOOL *>(pData) );
		}

		return S_OK;
	}

	else if( m_pTimeline && m_pSequenceStrip && ::IsEqualGUID( rguidType, GUID_Segment_ExtensionLength ) )
	{
		DWORD dwTemp = m_pSequenceStrip->m_dwExtraBars;
		m_pSequenceStrip->m_dwExtraBars = *(DWORD *)pData;
		m_pSequenceStrip->m_dwExtraBars = m_pSequenceStrip->GetNumExtraBars();
		if( m_pSequenceStrip->m_dwExtraBars != dwTemp )
		{
			m_pSequenceStrip->InvalidateStrip();
		}

		return S_OK;
	}

	else if( m_pTimeline && m_pSequenceStrip && ::IsEqualGUID( rguidType, GUID_Segment_PickupLength ) )
	{
		BOOL fTemp = m_pSequenceStrip->m_fPickupBar;
		m_pSequenceStrip->m_fPickupBar = *(DWORD *)pData;
		m_pSequenceStrip->m_fPickupBar = m_pSequenceStrip->ShouldDisplayPickupBar();
		if( m_pSequenceStrip->m_fPickupBar != fTemp )
		{
			m_pSequenceStrip->InvalidateStrip();
		}

		return S_OK;
	}

	else if( ::IsEqualGUID( rguidType, GUID_TimelineSetCursor ) )
	{
		if( m_pTimeline && m_pSequenceStrip )
		{
			// Update displayed instrument name, if necessary
			m_pSequenceStrip->UpdateInstrumentName();
		}
	}

	else if( ::IsEqualGUID( rguidType, GUID_TimelineSetSnapTo )
		 ||  ::IsEqualGUID( rguidType, GUID_TimelineUserSetCursor ) )
	{
		if( m_pTimeline && m_pSequenceStrip )
		{
			// If we've inserted a note, add an undo state
			if( m_fNoteInserted )
			{
				m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT;
				OnDataChanged();
				// Handled by OnDataChanged
				//m_fNoteInserted = FALSE;
			}

			// Save the current insert time
			long lInsertTime = m_pSequenceStrip->m_lInsertTime;

			// Only redraw if the sequence strip has focus
			m_pSequenceStrip->UpdateNoteCursorTime( m_pSequenceStrip->m_iHaveFocus == 1 );

			// Check if the insert time stayed the same, but
			// the snap-to setting changed and the sequence strip has focus
			if( (lInsertTime == m_pSequenceStrip->m_lInsertTime)
			&&	(m_pSequenceStrip->m_iHaveFocus == 1)
			&&	::IsEqualGUID( rguidType, GUID_TimelineSetSnapTo ) )
			{
				m_pSequenceStrip->InvalidatePianoRoll();
			}

			if( ::IsEqualGUID( rguidType, GUID_TimelineUserSetCursor ) )
			{
				// If sequence strip is active
				if( (m_pSequenceStrip->m_iHaveFocus == 1) )
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
							PlayPatch( lTime );
						}
					}
					else
					{
						pSegState->Release();
					}
				}

				// Update displayed instrument name, if necessary
				m_pSequenceStrip->UpdateInstrumentName();
			}
		}
	}
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CSequenceMgr::GetStripMgrProperty(
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
		if( m_pDMProdFramework )
		{
			return m_pDMProdFramework->QueryInterface( IID_IUnknown, (void**)&V_UNKNOWN( pVariant ) );
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

			pioTrackHeader->guidClassID = CLSID_DirectMusicSeqTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = DMUS_FOURCC_SEQ_TRACK;
			pioTrackHeader->fccType = NULL;
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


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CSequenceMgr::SetStripMgrProperty(
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
		// If we were previously attached to a timeline, remove our PropPageObject and strip from it
		if( m_pTimeline )
		{
			m_fShuttingDown = TRUE;
			m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->Release();
				m_pPropPageMgr = NULL;
			}
			if ( m_pSequenceStrip )
			{
				m_pSequenceStrip->UnRegisterMidi();
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pSequenceStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pSequenceStrip );
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_ExtensionLength, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_PickupLength, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordKey, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordSharpsFlats, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_WindowActive, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_RecordButton, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Sequence_Color_Change, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Sequence_Notation_Change, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Sequence_Quantize, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Sequence_Velocitize, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_BandTrackChange, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetSnapTo, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineUserSetCursor, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetCursor, m_dwOldGroupBits );
			m_pTimeline->Release();
			m_pTimeline = NULL;

			if( m_pIPChannelName )
			{
				m_pIPChannelName->Release();
				m_pIPChannelName = NULL;
			}

			// If we have a valid segment state, then the segment is still playing
			if( m_pSegmentState )
			{
				m_pSegmentState->Release();
				m_pSegmentState = NULL;

				// This should only be able to be true iff m_pSegmentState is non-NULL
				if( m_fNoteInserted || m_fCurveInserted )
				{
					if( m_fNoteInserted )
					{
						if( m_fCurveInserted )
						{
							m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT;
						}
						else
						{
							m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT;
						}
					}
					else // if( m_fCurveInserted )
					{
						m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT_CURVE;
					}

					CoInitialize(NULL);
					OnDataChanged( );
					CoUninitialize();
					// Handled by OnDataChanged
					//m_fNoteInserted = FALSE;
					//m_fCurveInserted = FALSE;
				}
			}

			// m_fNoteInserted should only be able to be true iff m_pSegmentState is non-NULL
			// (which we checked above)
			ASSERT( !m_fNoteInserted );
			ASSERT( !m_fCurveInserted );
		}

		if( V_UNKNOWN( &variant ) )
		{
			m_fShuttingDown = FALSE;

			if( FAILED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pTimeline )))
			{
				return E_FAIL;
			}
			else
			{
				// Get a m_pIPChannelName pointer
				VARIANT var;
				ASSERT( m_pIPChannelName == NULL );
				if (SUCCEEDED(m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &var )))
				{
					IDMUSProdNode* pINode;
					if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
					{
						IDMUSProdProject* pIProject;
						if( SUCCEEDED ( m_pDMProdFramework->FindProject( pINode, &pIProject ) ) )
						{
							pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&m_pIPChannelName );
							pIProject->Release();
						}
						pINode->Release();
					}
					V_UNKNOWN(&var)->Release();
				}

				// Add the strip to the timeline
				m_pSequenceStrip->UpdateName(); // Necessary to ensure the strips are sorted correctly
				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pSequenceStrip, CLSID_DirectMusicSeqTrack, m_dwGroupBits, PChannelToStripIndex( m_dwPChannel ) );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetCursor, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineUserSetCursor, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetSnapTo, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_BandTrackChange, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Sequence_Velocitize, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Sequence_Quantize, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Sequence_Notation_Change, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Sequence_Color_Change, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_RecordButton, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_WindowActive, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Start, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Stop, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordSharpsFlats, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_ChordKey, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_PickupLength, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_ExtensionLength, m_dwGroupBits );

				// Make sure there is a curve strip for each type of Curve in this sequence
				m_pSequenceStrip->SyncCurveStripStateList();

				// Initialize the Sequence Strip's UI data
				m_pSequenceStrip->ApplyUIChunk( &m_SeqStripDesign );
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
		if( m_pDMProdFramework )
		{
			m_pDMProdFramework->Release();
			m_pDMProdFramework = NULL;
		}
		if( SUCCEEDED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**) &m_pDMProdFramework ) ) )
		{
			IDMUSProdComponent* pIComponent = NULL;
			if( SUCCEEDED ( m_pDMProdFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
			{
				pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor );
				pIComponent->Release();
			}

			if( m_pIConductor && (m_pIDMPerformance == NULL) )
			{
				IUnknown* punk;
				if( SUCCEEDED( m_pIConductor->GetPerformanceEngine( &punk ) ) )
				{
					punk->QueryInterface( IID_IDirectMusicPerformance, (void **)&m_pIDMPerformance );
					punk->Release();
				}
			}
			return S_OK;
		}
		else
		{
			return E_FAIL;
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
			if( m_pSequenceStrip )
			{
				m_pSequenceStrip->UpdateName();
			}
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
// CSequenceMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::GetClassID

HRESULT CSequenceMgr::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pClsId == NULL )
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy( pClsId, &CLSID_SequenceMgr, sizeof( CLSID ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::IsDirty

HRESULT CSequenceMgr::IsDirty()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( m_fDirty )
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::Load


HRESULT CSequenceMgr::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Check for Direct Music format
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Remove all existing sequences
	EmptySequenceList();
	m_dwPChannel = 0;
	m_fSetPChannel = FALSE;

	m_strName.LoadString( IDS_STRIP_NAME );

	MMCKINFO	ck, ckList;
	// Load the Track
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
		case FOURCC_LIST:
			switch( ck.fccType )
			{
			case DMUS_FOURCC_UNFO_LIST:
				while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
				{
					switch( ckList.ckid )
					{
						case DMUS_FOURCC_UNAM_CHUNK:
						case RIFFINFO_INAM:
							ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
							break;

						case RIFFINFO_IART:
						case DMUS_FOURCC_UART_CHUNK:
							ReadMBSfromWCS( pIStream, ckList.cksize, &m_strAuthor );
							break;

						case RIFFINFO_ICOP:
						case DMUS_FOURCC_UCOP_CHUNK:
							ReadMBSfromWCS( pIStream, ckList.cksize, &m_strCopyright );
							break;

						case RIFFINFO_ISBJ:
						case DMUS_FOURCC_USBJ_CHUNK:
							ReadMBSfromWCS( pIStream, ckList.cksize, &m_strSubject );
							break;

						case RIFFINFO_ICMT:
						case DMUS_FOURCC_UCMT_CHUNK:
							ReadMBSfromWCS( pIStream, ckList.cksize, &m_strInfo );
							break;
					}
					pIRiffStream->Ascend( &ckList, 0 );
				}
				break;

			case DMUS_FOURCC_SEQUENCE_UI_LIST:
				hr = LoadSequenceUIList( pIRiffStream, &ck );
				if( FAILED( hr ) )
				{
					hr = E_FAIL;
			        goto ON_ERROR;
				}
				break;
			}
			break;

		case DMUS_FOURCC_SEQ_TRACK:
			hr = LoadSeqTrack( pIRiffStream, ck );
			if( FAILED( hr ) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
			break;
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

	if( !m_fSetPChannel )
	{
		m_dwPChannel = m_SeqStripDesign.m_dwPChannel;
	}

	SyncWithDirectMusic();

ON_ERROR:
	pIRiffStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::Save

HRESULT CSequenceMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(fClearDirty);

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	// We only support saving to a design-time stream (GUID_CurrentVersion) or a DirectMusic
	// stream that will be loaded into a DirectMusicSequenceTrack (GUID_DirectMusicObject)
	if( !::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) &&
		!::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
	{
		return E_INVALIDARG;
	}

	// Now, finally save ourself
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Alloc an IDMUSProdRIFFStream from the IStream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	DWORD dwBytesWritten;

	// If design file, save track unfo chunk
	if( ftFileType == FT_DESIGN )
	{
		WriteListChunk listUnfo(pIRiffStream, DMUS_FOURCC_UNFO_LIST);
		hr = listUnfo.State();
		if( hr == S_OK )
		{
			if( !m_strName.IsEmpty() )
			{
				WriteChunk chunk(pIRiffStream, DMUS_FOURCC_UNAM_CHUNK);
				hr = chunk.State();
				if( hr == S_OK )
				{
					hr = SaveMBStoWCS( pIStream, &m_strName );
				}
				if( FAILED( hr ) )
				{
					goto ON_ERROR;
				}
			}

			if( !m_strAuthor.IsEmpty() )
			{
				WriteChunk chunk(pIRiffStream, DMUS_FOURCC_UART_CHUNK);
				hr = chunk.State();
				if( hr == S_OK )
				{
					hr = SaveMBStoWCS( pIStream, &m_strAuthor );
				}
				if( FAILED( hr ) )
				{
					goto ON_ERROR;
				}
			}

			if( !m_strCopyright.IsEmpty() )
			{
				WriteChunk chunk(pIRiffStream, DMUS_FOURCC_UCOP_CHUNK);
				hr = chunk.State();
				if( hr == S_OK )
				{
					hr = SaveMBStoWCS( pIStream, &m_strCopyright );
				}
				if( FAILED( hr ) )
				{
					goto ON_ERROR;
				}
			}

			if( !m_strSubject.IsEmpty() )
			{
				WriteChunk chunk(pIRiffStream, DMUS_FOURCC_USBJ_CHUNK);
				hr = chunk.State();
				if( hr == S_OK )
				{
					hr = SaveMBStoWCS( pIStream, &m_strSubject );
				}
				if( FAILED( hr ) )
				{
					goto ON_ERROR;
				}
			}

			if( !m_strInfo.IsEmpty() )
			{
				WriteChunk chunk(pIRiffStream, DMUS_FOURCC_UCMT_CHUNK);
				hr = chunk.State();
				if( hr == S_OK )
				{
					hr = SaveMBStoWCS( pIStream, &m_strInfo );
				}
				if( FAILED( hr ) )
				{
					goto ON_ERROR;
				}
			}
		}
	}

	{
		WriteChunk chunk(pIRiffStream, DMUS_FOURCC_SEQ_TRACK);
		hr = chunk.State();
		if(hr == S_OK)
		{
			// write notes
			WriteChunk chunk(pIRiffStream, DMUS_FOURCC_SEQ_LIST);
			hr = chunk.State();
			if(hr == S_OK)
			{
				WriteDWORD(pIRiffStream, sizeof(DMUS_IO_SEQ_ITEM));
				DMUS_IO_SEQ_ITEM oSequence;
				POSITION pos = m_lstSequences.GetHeadPosition();
				while( pos )
				{
					// Clear out the structure (clears out the padding bytes as well).
					ZeroMemory( &oSequence, sizeof(DMUS_IO_SEQ_ITEM) );
					m_lstSequences.GetNext(pos)->CopyTo(oSequence);

					oSequence.dwPChannel = m_dwPChannel;

					hr = pIStream->Write( &oSequence, sizeof(DMUS_IO_SEQ_ITEM), &dwBytesWritten );
					if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_SEQ_ITEM) )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}

					if( FAILED( hr ) )
					{
						goto ON_ERROR;
					}
				}
			}
		}
		// write curves
		if(hr == S_OK)
		{
			WriteChunk curve(pIRiffStream, DMUS_FOURCC_CURVE_LIST);
			hr = curve.State();
			if(hr == S_OK)
			{			
				WriteDWORD(pIRiffStream, sizeof(DMUS_IO_CURVE_ITEM));
				POSITION pos = m_lstCurves.GetHeadPosition();
				while( pos )
				{
					CCurveItem* pCurveItem;
					pCurveItem = m_lstCurves.GetNext( pos );
					ASSERT( pCurveItem );
					if( pCurveItem )
					{
						DMUS_IO_CURVE_ITEM oCurve;
						// Clear out the structure (clears out the padding bytes as well).
						ZeroMemory( &oCurve, sizeof(DMUS_IO_CURVE_ITEM) );
						pCurveItem->CopyTo(oCurve);

						oCurve.dwPChannel = m_dwPChannel;

						hr = pIStream->Write( &oCurve, sizeof(DMUS_IO_CURVE_ITEM), &dwBytesWritten );
						if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_CURVE_ITEM) )
						{
							hr = E_FAIL;
							goto ON_ERROR;
						}

						if( FAILED( hr ) )
						{
							goto ON_ERROR;
						}
					}
				}
			}
		}
	}

	if( ftFileType == FT_DESIGN )
	{
		SaveSequenceUIList( pIRiffStream );
	}


ON_ERROR:
	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::GetSizeMax

HRESULT CSequenceMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pcbSize);
	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::GetData

// This method is called by CNotePropPageMgr to get data to send to the
// Sequence property page.
// The CSequenceStrip::GetData() method is called by CSequenceStripPropPageMgr
// to get the strip's properties (Group Bits, etc.)
HRESULT STDMETHODCALLTYPE CSequenceMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	if( GetNumSelected() )
	{
		if( m_fDisplayingFlats )
		{
			m_PropSequence.m_dwBits |= BF_DISPLAYFLATS;
		}
		else
		{
			m_PropSequence.m_dwBits &= ~BF_DISPLAYFLATS;
		}
		*ppData = (void*)&m_PropSequence;
		return S_OK;
	}
	else
	{
		*ppData = NULL;
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SetData

// This method is called by CSequencePropPageMgr in response to user actions
// in the Sequence Property page.  It changes the currenly selected Sequence. 
HRESULT STDMETHODCALLTYPE CSequenceMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pData == NULL )
	{
		return E_INVALIDARG;
	}

	DWORD dwChanged = ApplyToSelectedNotes( (CPropSequence *)pData );

	// Play the selected Note if the velocity or value has changed.
	/* Don't do this
	if ( m_pSequenceStrip->m_pActiveNote &&
		 ((dwChanged & (UD_OCTAVE | UD_MIDIVALUE | UD_VELOCITY)) != 0) )
	{
		PlayNote( m_pSequenceStrip->m_pActiveNote );
	}
	*/

	// If anything has changed, redraw the strip(s)
	if( dwChanged != 0 )
	{
		m_fDirty = TRUE;

		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_CHANGE;
		OnDataChanged();
		m_pSequenceStrip->InvalidateStrip();

		// Always recompute m_SelectedPropNote and update the property page
		RefreshPropertyPage();
		return S_OK;
	}
	else
	{
		// Nothing changed - return S_FALSE
		// Update the property page
		RefreshPropertyPage();
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CSequenceMgr::OnShowProperties( void)
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
	m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
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
	pIPropSheet->Release();

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		CNotePropPageMgr* pPPM = new CNotePropPageMgr;
		if( pPPM == NULL )
		{
			return E_OUTOFMEMORY;
		}

		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		
		// Remove the reference created by the contrustor, leaving the one created by QueryInterface.
		// If QueryInterface failed, this will delete m_pPropPageMgr.
		m_pPropPageMgr->Release();

		if( FAILED(hr) )
		{
			return hr;
		}
	}

	// Save the focus so we can restore after changing the property page
	HWND hwndHadFocus = ::GetFocus();

	// Set the displayed property page to our property page
	m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);

	// Restore the focus if it changed
	if( hwndHadFocus != ::GetFocus() )
	{
		::SetFocus( hwndHadFocus );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CSequenceMgr::OnRemoveFromPageManager( void)
{
	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::OnDataChanged

HRESULT STDMETHODCALLTYPE CSequenceMgr::OnDataChanged( void)
{
	if( m_fUpdateDirectMusic )
	{
		SyncWithDirectMusic();
	}
	else
	{
		m_fUpdateDirectMusic = TRUE;
	}

	ASSERT( m_pTimeline );
	if ( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Let our hosting editor know about the change
	m_pTimeline->OnDataChanged( (ISequenceMgr*)this );

	m_fNoteInserted = FALSE;
	m_fCurveInserted = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::DeleteSelectedSequences

HRESULT CSequenceMgr::DeleteSelectedSequences()
{
	// If fChanged is set, something changed (so we should update our editor)
	BOOL fChanged = FALSE;

	CSequenceItem* pSequenceItem; // Temp. variable
	POSITION pos2, pos1 = m_lstSequences.GetHeadPosition();
	while( pos1 )
	{
		// Need to save current position, since GetNext will set pos1 to point
		// to the next sequence.
		pos2 = pos1;
		pSequenceItem = m_lstSequences.GetNext( pos1 );
		if ( pSequenceItem->m_fSelected )
		{
			// Remove the sequence from the current position.  pos1 will
			// still be valid, and it will still point to the next sequence.
			m_lstSequences.RemoveAt( pos2 );
			if(m_pSequenceStrip && pSequenceItem == m_pSequenceStrip->m_pSelectPivotSeq)
			{
				m_pSequenceStrip->m_pSelectPivotSeq = 0;
			}
			delete pSequenceItem;
			fChanged = TRUE;
		}
	}

	// If fChanged is set, something changed (so we should update our editor)
	if( fChanged )
	{
		m_fDirty = TRUE;
		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_DELETE;
		OnDataChanged();
		RefreshPropertyPage();
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SaveSelectedSequenceChunk

HRESULT CSequenceMgr::SaveSelectedSequenceChunk(IDMUSProdRIFFStream* pIRiffStream, MUSIC_TIME mtBeatsOffset)
{
	IStream *pIStream =	pIRiffStream->GetStream();

	// write notes
	WriteChunk chunk(pIRiffStream, DMUS_FOURCC_SEQ_LIST);
	HRESULT hr = chunk.State();
	if(hr == S_OK)
	{
		WriteDWORD(pIRiffStream, sizeof(DMUS_IO_SEQ_ITEM));
		POSITION pos = m_lstSequences.GetHeadPosition();
		while( pos )
		{
			DMUS_IO_SEQ_ITEM oSequence;
			CSequenceItem* pSequenceItem = m_lstSequences.GetNext( pos );
			if ( pSequenceItem->m_fSelected )
			{
				ASSERT( Status(pSequenceItem->m_bStatus) == MIDI_NOTEON );

				// Clear out the structure (clears out the padding bytes as well).
				ZeroMemory( &oSequence, sizeof(DMUS_IO_SEQ_ITEM) );
				pSequenceItem->CopyTo(oSequence);
				// POSSIBLE BUG: need to recalibrate time to meter
				oSequence.mtTime -= mtBeatsOffset;

				DWORD dwBytesWritten;
				hr = pIStream->Write( &oSequence, sizeof(DMUS_IO_SEQ_ITEM), &dwBytesWritten );
				if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_SEQ_ITEM) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( FAILED( hr ) )
				{
					goto ON_ERROR;
				}
			}
		}
	}

ON_ERROR:
	pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SaveSelectedCurveChunk

HRESULT CSequenceMgr::SaveSelectedCurveChunk(IDMUSProdRIFFStream* pIRiffStream, MUSIC_TIME mtBeatsOffset)
{
	IStream *pIStream =	pIRiffStream->GetStream();

	WriteChunk curve(pIRiffStream, DMUS_FOURCC_CURVE_LIST);
	HRESULT hr = curve.State();
	if(hr == S_OK)
	{			
		WriteDWORD(pIRiffStream, sizeof(DMUS_IO_CURVE_ITEM));
		POSITION pos = m_lstCurves.GetHeadPosition();
		while( pos )
		{
			CCurveItem* pCurveItem = m_lstCurves.GetNext( pos );
			ASSERT( pCurveItem );
			if( pCurveItem->m_fSelected )
			{
				DMUS_IO_CURVE_ITEM oCurve;
				// Clear out the structure (clears out the padding bytes as well).
				ZeroMemory( &oCurve, sizeof(DMUS_IO_CURVE_ITEM) );
				pCurveItem->CopyTo(oCurve);
				oCurve.mtStart -= mtBeatsOffset;

				DWORD dwBytesWritten;
				hr = pIStream->Write( &oCurve, sizeof(DMUS_IO_CURVE_ITEM), &dwBytesWritten );
				if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_CURVE_ITEM) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( FAILED( hr ) )
				{
					goto ON_ERROR;
				}
			}
		}
	}

ON_ERROR:
	pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SaveSelectedSequences

HRESULT CSequenceMgr::SaveSelectedSequences( LPSTREAM pIStream, MUSIC_TIME mtOffset )
{
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// If the Sequence list has anything in it, look for selected Sequences
	if ( !m_lstSequences.IsEmpty() )
	{
		MUSIC_TIME mtTime = 0;
		if ( mtOffset != 0 )
		{
			// Snap mtOffset to the nearest grid before it
			mtTime = m_pSequenceStrip->FloorTimeToGrid( mtOffset, NULL );
		}
		
		// Now, write out each selected item
		WriteChunk chunk(pIRiffStream, DMUS_FOURCC_SEQ_TRACK);
		hr = chunk.State();
		if(hr == S_OK)
		{
			hr = SaveSelectedSequenceChunk( pIRiffStream, mtTime );
		}
	}
	else
	{
		hr = S_FALSE; // Nothing in the list
	}

	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SaveSelectedEvents

HRESULT CSequenceMgr::SaveSelectedEvents( LPSTREAM pIStream, MUSIC_TIME mtOffset )
{
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// If the Sequence list has anything in it, look for selected Sequences
	if( !m_lstCurves.IsEmpty()
	||	!m_lstSequences.IsEmpty() )
	{
		MUSIC_TIME mtTime = 0;
		if ( mtOffset != 0 )
		{
			// Snap mtOffset to the nearest grid before it
			mtTime = m_pSequenceStrip->FloorTimeToGrid( mtOffset, NULL );
		}
		
		// Now, write out each selected item
		WriteChunk chunk(pIRiffStream, DMUS_FOURCC_SEQ_TRACK);
		hr = chunk.State();
		if(hr == S_OK)
		{
			hr = SaveSelectedSequenceChunk( pIRiffStream, mtTime );
			if( SUCCEEDED( hr ) )
			{
				hr = SaveSelectedCurveChunk( pIRiffStream, mtTime );
			}
		}
	}
	else
	{
		hr = S_FALSE; // Nothing in the list
	}

	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SaveSelectedSequencesAsMidi

HRESULT CSequenceMgr::SaveSelectedSequencesAsMidi( LPSTREAM pIStream, MUSIC_TIME mtOffset )
{
	HRESULT hr = S_FALSE;

	// If the Sequence list has anything in it, look for selected Sequences
	if ( !m_lstSequences.IsEmpty() )
	{
		MUSIC_TIME mtTime = 0;
		if ( mtOffset != 0 )
		{
			// Snap mtOffset to the nearest grid before it
			mtTime = m_pSequenceStrip->FloorTimeToGrid( mtOffset, NULL );
		}

		// Offset times
		CTypedPtrList<CPtrList, CSequenceItem*> lstSequences;
		POSITION pos = m_lstSequences.GetHeadPosition();
		while(pos)
		{
			CSequenceItem* pItem = m_lstSequences.GetNext(pos);
			if(pItem->m_fSelected)
			{
				CSequenceItem* pItemNew = new CSequenceItem(*pItem);
				// Subtract off the offset to normalize the item
				// (if not normalizing, mtBeatsOffset will be 0)
				pItemNew->m_mtTime -= mtTime;
				lstSequences.AddTail(pItemNew);
			}
		}

		if( !lstSequences.IsEmpty() )
		{
			// Only need to do this if anything was selected
			hr = WriteMidiSequenceToStream(pIStream, m_dwPChannel, lstSequences);

			// cleanup
			while( !lstSequences.IsEmpty() )
			{
				delete lstSequences.RemoveHead();
			}
		}
	}
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::FirstGutterSelectedSequenceMgr

BOOL CSequenceMgr::FirstGutterSelectedSequenceMgr( void )
{
	IDMUSProdStrip *pStrip;
	DWORD dwEnum = 0;
	BOOL fFirstSelectedSequenceMgr = FALSE;
	BOOL fContinueEnum = TRUE;
	// Iterate through all the strips
	while( fContinueEnum && SUCCEEDED( m_pTimeline->EnumStrip( dwEnum, &pStrip ) ) )
	{
		VARIANT varGutter;
		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pStrip, STP_GUTTER_SELECTED, &varGutter ) )
		&&	(varGutter.vt == VT_BOOL) && ( V_BOOL(&varGutter) == TRUE ) )
		{
			// Get their strip manager
			VARIANT varStripMgr;
			if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) ) )
			{
				// Check if they belong to a Sequence strip manager
				ISequenceMgr *pISequenceMgr;
				if( SUCCEEDED( V_UNKNOWN( &varStripMgr )->QueryInterface( IID_ISequenceMgr, (void**)&pISequenceMgr ) ) )
				{
					// Yes - check if it is this Sequence strip manager
					ISequenceMgr *pThisSequenceMgr = NULL;
					if( SUCCEEDED( QueryInterface( IID_ISequenceMgr, (void **)&pThisSequenceMgr ) ) )
					{
						if( pThisSequenceMgr == pISequenceMgr )
						{
							// We are the first selected sequence strip - do the save to MIDI file operation
							fFirstSelectedSequenceMgr = TRUE;
						}
						pThisSequenceMgr->Release();
					}
					fContinueEnum = FALSE;
					pISequenceMgr->Release();
				}
				V_UNKNOWN( &varStripMgr )->Release();
			}
		}
		pStrip->Release();
		dwEnum++;
	}

	return fFirstSelectedSequenceMgr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SaveSelectedEventsAsMidi

HRESULT CSequenceMgr::SaveSelectedEventsAsMidi( LPSTREAM pIStream )
{
	// This method is only called by CSequenceStrip::Copy when
	// m_bGutterSelected && (m_lBeginSelect >= 0) && (m_lEndSelect > 0) is true

	if( !FirstGutterSelectedSequenceMgr() )
	{
		// Not the first selected sequence strip - return S_FALSE so we don't add MIDI data to the clipboard
		return S_FALSE;
	}

	// We are the first selected sequence strip - do the save to MIDI file operation

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
			// Get their strip manager
			VARIANT varStripMgr;
			if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) ) )
			{
				// Check if they belong to a Sequence strip manager
				ISequenceMgr *pISequenceMgr;
				if( SUCCEEDED( V_UNKNOWN( &varStripMgr )->QueryInterface( IID_ISequenceMgr, (void**)&pISequenceMgr ) ) )
				{
					// Yes - Sequence strip - increment number of tracks
					wNumMIDITracks++;
					pISequenceMgr->Release();
				}
				V_UNKNOWN( &varStripMgr )->Release();
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

	// Write empty Tempo map track
	static const BYTE abControlTrack[12] = {
		0x4d, 0x54, 0x72, 0x6b, // MTrk
		0x00, 0x00, 0x00, 0x04, // Length
		0x00, 0xFF, 0x2F, 0x00 }; // Track end marker

	pIStream->Write( abControlTrack, 12, NULL );

	// write tracks
	dwEnum = 0;

	// Save a copy of the pointer to the last SequenceMgr so we don't add the same
	// strip more than once
	ISequenceMgr *rpLastISequenceMgr = NULL;

	// Iterate through all the strips
	while( SUCCEEDED( m_pTimeline->EnumStrip( dwEnum, &pStrip ) ) )
	{
		VARIANT varGutter;
		if( SUCCEEDED( m_pTimeline->StripGetTimelineProperty( pStrip, STP_GUTTER_SELECTED, &varGutter ) )
		&&	(varGutter.vt == VT_BOOL) && ( V_BOOL(&varGutter) == TRUE ) )
		{
			// Get their strip manager
			VARIANT varStripMgr;
			if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) ) )
			{
				// Check if they belong to a Sequence strip manager
				ISequenceMgr *pISequenceMgr;
				if( SUCCEEDED( V_UNKNOWN( &varStripMgr )->QueryInterface( IID_ISequenceMgr, (void**)&pISequenceMgr ) ) )
				{
					if( rpLastISequenceMgr != pISequenceMgr )
					{
						// Yes - Sequence strip - Write the track
						pISequenceMgr->SaveEventsToMIDITrack(pIStream);
						rpLastISequenceMgr = pISequenceMgr;
					}
					pISequenceMgr->Release();
				}
				V_UNKNOWN( &varStripMgr )->Release();
			}
		}
		pStrip->Release();
		dwEnum++;
	}
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::ImportMIDIFileToMultipleTracks

HRESULT CSequenceMgr::ImportMIDIFileToMultipleTracks( LPSTREAM pStream )
{
	if(pStream == NULL)
	{
		return E_POINTER;
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
			// Get their strip manager
			VARIANT varStripMgr;
			if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) ) )
			{
				// Check if they belong to a Sequence strip manager
				ISequenceMgr *pISequenceMgr;
				if( SUCCEEDED( V_UNKNOWN( &varStripMgr )->QueryInterface( IID_ISequenceMgr, (void**)&pISequenceMgr ) ) )
				{
					// Yes - Sequence strip - read the track
					pISequenceMgr->PasteEventsFromMIDITrack(pStream, gnPPQN, dwLength);
					pISequenceMgr->Release();
				}
				V_UNKNOWN( &varStripMgr )->Release();
			}
		}
		pStrip->Release();
		dwStripEnum++;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::ImportSequenceChunkData

HRESULT CSequenceMgr::ImportSequenceChunkData( LPSTREAM pIStream, long lChunkSize, long lTimeOffset )
{
	DWORD dwSequenceSize, dwByteCount;
	HRESULT hr = pIStream->Read( &dwSequenceSize, sizeof(DWORD), &dwByteCount );
	if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
	{
		return E_FAIL;
	}

	ASSERT( dwSequenceSize == sizeof(DMUS_IO_SEQ_ITEM) );
	if( dwSequenceSize != sizeof(DMUS_IO_SEQ_ITEM) )
	{
		return E_FAIL;
	}

	lChunkSize -= sizeof(DWORD);

	DMUS_IO_SEQ_ITEM iSequence;
	while( lChunkSize > 0 )
	{
		ZeroMemory(&iSequence, sizeof(DMUS_IO_SEQ_ITEM));
		hr = pIStream->Read( &iSequence, sizeof(DMUS_IO_SEQ_ITEM), &dwByteCount );
		if( FAILED( hr ) || dwByteCount != sizeof(DMUS_IO_SEQ_ITEM) )
		{
			return E_FAIL;
		}

		lChunkSize -= dwByteCount;

		CSequenceItem* pItem = new CSequenceItem;
		if ( pItem == NULL )
		{
			return E_OUTOFMEMORY;
		}
		
		pItem->CopyFrom(iSequence);

		// offset the note's MusicTime value
		pItem->MusicTime() += lTimeOffset;

		pItem->m_fSelected = TRUE;

		InsertByAscendingTime(pItem);
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::ImportCurveChunkData

HRESULT CSequenceMgr::ImportCurveChunkData( LPSTREAM pIStream, long lChunkSize, long lTimeOffset )
{
	DWORD dwCurveSize, dwByteCount;
	HRESULT hr = pIStream->Read(&dwCurveSize, sizeof(DWORD), &dwByteCount);
	if(FAILED(hr) || dwByteCount != sizeof(DWORD))
	{	
		return E_FAIL;
	}
	lChunkSize -= sizeof(DWORD);

	DMUS_IO_CURVE_ITEM iCurve;
	while(lChunkSize > 0)
	{
		ZeroMemory(&iCurve, sizeof(DMUS_IO_CURVE_ITEM));
		hr = pIStream->Read(&iCurve, dwCurveSize, &dwByteCount);
		if(FAILED(hr) || dwByteCount != dwCurveSize)
		{
			return E_FAIL;
		}

		lChunkSize -= dwByteCount;

		CCurveItem* pItem = new CCurveItem;
		if(pItem == NULL)
		{
			return E_OUTOFMEMORY;
		}

		pItem->CopyFrom(iCurve);

		// offset the curve's MusicTime value
		pItem->MusicTime() = iCurve.mtStart + lTimeOffset;

		pItem->m_fSelected = TRUE;

		InsertByAscendingTime(pItem);
	}

	return S_OK;
}


/*
/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::MarkSelectedSequences

void CSequenceMgr::MarkSelectedSequences( DWORD dwFlags )
{
	// For all selected items, OR dwFlags with the item's m_dwBits member.
	POSITION pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		CSequenceItem* pSequenceItem;
		pSequenceItem = m_lstSequences.GetNext( pos );
		if ( pSequenceItem->m_fSelected )
		{
			pSequenceItem->m_dwBits |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::DeleteMarked

void CSequenceMgr::DeleteMarked( DWORD dwFlags )
{
	// For all items, if the item's m_dwBits member has any of the same bits set as
	// dwFlags (m_dwBits & dwFlags is non-zero), delete it.
	CSequenceItem* pSequenceItem;
	POSITION pos2, pos1 = m_lstSequences.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pSequenceItem = m_lstSequences.GetNext( pos1 );
		if ( pSequenceItem->m_dwBits & dwFlags )
		{
			m_lstSequences.RemoveAt( pos2 );
			if(m_pSequenceStrip && pSequenceItem == m_pSequenceStrip->m_pSelectPivotSeq)
			{
				m_pSequenceStrip->m_pSelectPivotSeq = 0;
			}
			m_fDirty = TRUE;
			delete pSequenceItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::UnMarkSequences

void CSequenceMgr::UnMarkSequences( DWORD dwFlags )
{
	// For all items, clear the bits in the item's m_dwBits member that are also set in dwFlags.
	POSITION pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		m_lstSequences.GetNext( pos )->m_dwBits &= ~dwFlags;
	}
}
*/


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SelectAllNotes

void CSequenceMgr::SelectAllNotes()
{
	BOOL fChanged = FALSE;
	POSITION pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		CSequenceItem *pSeqItem = m_lstSequences.GetNext( pos );
		if( !pSeqItem->m_fSelected )
		{
			fChanged = TRUE;
			pSeqItem->m_fSelected = TRUE;
		}
	}

	// If anything changed, refresh the display and property pages
	if (fChanged)
	{
		m_pSequenceStrip->InvalidateStrip();
		RefreshPropertyPage( );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::IsSelected

BOOL CSequenceMgr::IsSelected()
{
	// If anything is selected, return TRUE.
	POSITION pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		if ( m_lstSequences.GetNext( pos )->m_fSelected )
		{
			return TRUE;
		}
	}

	if( !m_pSequenceStrip ) 
	{
		return FALSE;
	}

	// If the gutter is selected, need to also check for selected curves
	if( m_pSequenceStrip->m_bGutterSelected && (m_pSequenceStrip->m_lBeginSelect >= 0) && (m_pSequenceStrip->m_lEndSelect > 0))
	{
		pos = m_lstCurves.GetHeadPosition();
		while( pos )
		{
			if ( m_lstCurves.GetNext( pos )->m_fSelected )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::FirstSelectedSequence

CSequenceItem* CSequenceMgr::FirstSelectedSequence(POSITION* pPos)
{
	POSITION pos;
	if(pPos == NULL)
	{
		pPos = &pos;
	}

	*pPos = m_lstSequences.GetHeadPosition();

	CSequenceItem* pSequenceItem;
	while( *pPos )
	{
		pSequenceItem = m_lstSequences.GetNext( *pPos );
		if ( pSequenceItem->m_fSelected )
		{
			return pSequenceItem;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SelectSegmentNotes

BOOL CSequenceMgr::SelectSegmentNotes(long lBeginTime, long lEndTime)
{
	ASSERT(m_pTimeline);
	if( !m_pTimeline )
	{
		return FALSE;
	}

	BOOL result = FALSE;
	if( lBeginTime == -1)
	{
		// till we implement m_lShiftFromMeasure (see CommandMgr::SelectSegment),
		// just set it to beginning
		lBeginTime = 0;
	}

	CSequenceItem* pSequenceItem;
	POSITION pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		pSequenceItem = m_lstSequences.GetNext( pos );
		pSequenceItem->m_fSelected = FALSE;
		if( (lBeginTime <= pSequenceItem->AbsTime()) && (pSequenceItem->AbsTime() < lEndTime) )
		{
			pSequenceItem->m_fSelected = TRUE;
			result = TRUE;
		}
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::EmptySequenceList

void CSequenceMgr::EmptySequenceList(void)
{
	// Remove all items from the list
	if( !m_lstSequences.IsEmpty() )
	{
		CSequenceItem *pSequenceItem;
		while ( !m_lstSequences.IsEmpty() )
		{
			pSequenceItem = m_lstSequences.RemoveHead();
			if(m_pSequenceStrip && pSequenceItem == m_pSequenceStrip->m_pSelectPivotSeq)
			{
				m_pSequenceStrip->m_pSelectPivotSeq = 0;
			}
			delete pSequenceItem;
		}
	}
	// Remove all items from the list
	if( !m_lstCurves.IsEmpty() )
	{
		CCurveItem *pCurveItem;
		while ( !m_lstCurves.IsEmpty() )
		{
			pCurveItem = m_lstCurves.RemoveHead();
			/*
			if(m_pCurveStrip && pCurveItem == m_pCurveStrip->m_pSelectPivotSeq)
			{
				m_pCurveStrip->m_pSelectPivotSeq = 0;
			}
			*/
			delete pCurveItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::InsertByAscendingTime

void CSequenceMgr::InsertByAscendingTime( CSequenceItem *pSequence )
{
	// Insert pSequence into the list in ascending order, determined by the values of
	// the item's start time.
	ASSERT( pSequence );
	if ( pSequence == NULL )
	{
		return;
	}

	POSITION pos2, pos1 = m_lstSequences.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		if ( m_lstSequences.GetNext( pos1 )->AbsTime() >= pSequence->AbsTime() )
		{
			// insert before pos2 (which is current position of pSequenceItem)
			m_lstSequences.InsertBefore( pos2, pSequence );
			return;
		}
	}
	// insert at end of list
	m_lstSequences.AddTail( pSequence );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::InsertByAscendingTime

void CSequenceMgr::InsertByAscendingTime( CCurveItem *pCurve )
{
	// Insert pCurve into the list in ascending order, determined by the values of
	// the item's start time.
	ASSERT( pCurve );
	if ( pCurve == NULL )
	{
		return;
	}

	POSITION pos2, pos1 = m_lstCurves.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		if ( m_lstCurves.GetNext( pos1 )->AbsTime() >= pCurve->AbsTime() )
		{
			// insert before pos2 (which is current position of pCurveItem)
			m_lstCurves.InsertBefore( pos2, pCurve );
			return;
		}
	}
	// insert at end of list
	m_lstCurves.AddTail( pCurve );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::InsertByAscendingTimeReplaceSame
/*
void CSequenceMgr::InsertByAscendingTimeReplaceSame( CSequenceItem *pSequence )
{
	// Insert pSequence into the list in ascending order, determined by the values of
	// the item's start time.
	ASSERT( pSequence );
	if ( pSequence == NULL )
	{
		return;
	}

	CSequenceItem* pTempSeq;
	POSITION pos2, pos1 = m_lstSequences.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pTempSeq = m_lstSequences.GetNext( pos1 );
		ASSERT( pTempSeq );
		if ( pTempSeq )
		{
			if ( pTempSeq->AbsTime() >= pSequence->AbsTime() )
			{
				// insert before pos2 (which is current position of pSequenceItem)
				m_lstSequences.InsertBefore( pos2, pSequence );

				if( (pTempSeq->AbsTime() == pSequence->AbsTime()) &&
					(pTempSeq->m_dwPChannel == pSequence->m_dwPChannel) &&
					(pTempSeq->m_bStatus == pSequence->m_bStatus) &&
					(pTempSeq->m_bByte1 == pSequence->m_bByte1) )
				{
					//TRACE("Replacing item %x,%d,%d with %d on %d.\n", pTempSeq->m_bStatus, pTempSeq->m_bByte1, pTempSeq->m_nStartValue, pSequence->m_nStartValue, pSequence->m_dwPChannel );
					// delete the item
					m_lstSequences.RemoveAt(pos2);
					delete pTempSeq;
				}
				// replace all events whose start is in the span of pSequenceItem
				while(pos1)
				{
					pos2 = pos1;
					pTempSeq = m_lstSequences.GetNext(pos1);
					if( (pTempSeq->AbsTime() == pSequence->AbsTime()) &&
						(pTempSeq->m_dwPChannel == pSequence->m_dwPChannel) &&
						(pTempSeq->m_bStatus == pSequence->m_bStatus) &&
						(pTempSeq->m_bByte1 == pSequence->m_bByte1) )
					{
						//TRACE("Replacing item %x,%d,%d with %d on %d.\n", pTempSeq->m_bStatus, pTempSeq->m_bByte1, pTempSeq->m_nStartValue, pSequence->m_nStartValue, pSequence->m_dwPChannel );
						// delete the item
						m_lstSequences.RemoveAt(pos2);
						delete pTempSeq;
					}
					else
					{
						break;	// seqlist ascending -> no more events in timespan
					}
				}
				return;
			}
		}
	}
	// insert at end of list
	m_lstSequences.AddTail( pSequence );
	return;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::InsertByAscendingTimeBackwards

void CSequenceMgr::InsertByAscendingTimeBackwards( CSequenceItem *pSequence )
{
	// Insert pSequence into the list in ascending order, determined by the values of
	// the item's m_lTime member.  Search from the end of the list forwards.
	ASSERT( pSequence );
	if ( pSequence == NULL )
	{
		return;
	}

	POSITION pos2, pos1 = m_lstSequences.GetTailPosition();
	while( pos1 )
	{
		pos2 = pos1;
		if ( m_lstSequences.GetPrev( pos1 )->AbsTime() < pSequence->AbsTime() )
		{
			// insert after pos2 (which is current position of pSequenceItem)
			m_lstSequences.InsertAfter( pos2, pSequence );
			return;
		}
	}
	// insert at head of list
	m_lstSequences.AddHead( pSequence );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::InsertByAscendingTimeBackwards

void CSequenceMgr::InsertByAscendingTimeBackwards( CCurveItem *pCurve )
{
	// Insert pCurve into the list in ascending order, determined by the values of
	// the item's start time.  Search from the end of the list forwards.
	ASSERT( pCurve );
	if ( pCurve == NULL )
	{
		return;
	}

	CCurveItem* pCurveItem;
	POSITION pos2, pos1 = m_lstCurves.GetTailPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pCurveItem = m_lstCurves.GetPrev( pos1 );
		ASSERT( pCurveItem );
		if ( pCurveItem )
		{
			if ( pCurveItem->AbsTime() < pCurve->AbsTime() )
			{
				// insert after pos2 (which is current position of pCurveItem)
				m_lstCurves.InsertAfter( pos2, pCurve );
				return;
			}
		}
	}
	// insert at head of list
	m_lstCurves.AddHead( pCurve );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::RemoveItem

BOOL CSequenceMgr::RemoveItem( CSequenceItem* pItem )
{
	// Removes the specified item from the list
	POSITION pos2;
	POSITION pos1 = m_lstSequences.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		if ( m_lstSequences.GetNext( pos1 ) == pItem )
		{
			m_lstSequences.RemoveAt( pos2 );
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SyncWithDirectMusic

HRESULT CSequenceMgr::SyncWithDirectMusic(  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;
//	return hr;

	// 1. persist the strip as a sequence track to a stream
	IStream* pIMemStream = NULL;
	IPersistStream* pIPersistStream = NULL;
	hr = m_pDMProdFramework->AllocMemoryStream( FT_RUNTIME, GUID_CurrentVersion, &pIMemStream );
	if( SUCCEEDED ( hr ) )
	{
		StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
		hr = Save(pIMemStream, FALSE);
		if ( SUCCEEDED( hr ) )
		{
			// 2. load the stream into m_pIDMTrack
			hr = m_pIDMTrack->QueryInterface(IID_IPersistStream, (void**)&pIPersistStream);
			if ( SUCCEEDED( hr ) )
			{
				StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
				hr = pIPersistStream->Load(pIMemStream);

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
		pIMemStream->Release();
	}

	return hr;
}


HRESULT CSequenceMgr::GetBoundariesOfSelectedEvents(long & lStartTime, long & lEndTime)
{

	ASSERT(m_pTimeline);
	HRESULT hr = S_OK;
	lStartTime = lEndTime = -1;
	CMusicTimeConverter cmtTime, cmtDur;
	CSequenceItem* pSequenceItem;
	POSITION pos = m_lstSequences.GetHeadPosition();
	while(pos)
	{
		pSequenceItem = m_lstSequences.GetNext( pos );
		if( pSequenceItem->m_fSelected )
		{
			cmtTime = pSequenceItem->AbsTime();
			cmtDur = pSequenceItem->m_mtDuration > 0 
							? pSequenceItem->m_mtDuration : 1;
			long lTestEnd = cmtTime + cmtDur;
			if(lStartTime == -1 || cmtTime < lStartTime)
			{
				lStartTime = cmtTime;
			}
			if(lEndTime == -1 || (lTestEnd > lEndTime))
			{
				lEndTime = lTestEnd;
			}
		}
	}
	if(hr == S_OK && (lStartTime == -1 || lEndTime == -1))
	{
		// nothing selected
		hr = S_FALSE;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::DeleteBetweenTimes

void CSequenceMgr::DeleteBetweenTimes( long lStart, long lEnd )
{
	// Iterate through the list
	CSequenceItem* pSequenceItem;
	POSITION pos2, pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		// Save the current position
		pos2 = pos;
		pSequenceItem = m_lstSequences.GetNext( pos );

		// If the tempo occurs between lStart and lEnd, delete it
		if( (pSequenceItem->AbsTime() >= lStart) && (pSequenceItem->AbsTime() <= lEnd) ) 
		{
			m_lstSequences.RemoveAt( pos2 );
			delete pSequenceItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::LoadSeqTrack

HRESULT CSequenceMgr::LoadSeqTrack( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO ck )
{
	ASSERT( pIRiffStream );
	if( pIRiffStream == NULL )
	{
		return E_POINTER;
	}

	IStream *pIStream;
	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream );
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;

	MMCKINFO ck1;
	ck1.ckid = DMUS_FOURCC_SEQ_LIST;
	if(pIRiffStream->Descend(&ck1, &ck, MMIO_FINDCHUNK) == 0)
	{
		DWORD dwSequenceSize, dwByteCount;
		hr = pIStream->Read(&dwSequenceSize, sizeof(DWORD), &dwByteCount);
		if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		dwSequenceSize = min( dwSequenceSize, sizeof(DMUS_IO_SEQ_ITEM) );

		DMUS_IO_SEQ_ITEM iSequence;
		long lChunkSize = ck1.cksize - sizeof(DWORD);
		while(lChunkSize > 0)
		{
			ZeroMemory(	&iSequence, sizeof(DMUS_IO_SEQ_ITEM) );
			hr = pIStream->Read( &iSequence, dwSequenceSize, &dwByteCount );
			if( FAILED( hr ) || dwByteCount != dwSequenceSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			lChunkSize -= dwSequenceSize;

			if( Status(iSequence.bStatus) != MIDI_NOTEON )
			{
				CCurveItem* pItem = new CCurveItem;
				if ( pItem == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}

				if( !m_fSetPChannel )
				{
					m_fSetPChannel = TRUE;
					m_dwPChannel = iSequence.dwPChannel;
				}

				pItem->CopyFrom(iSequence);

				InsertByAscendingTimeBackwards( pItem );
			}
			else
			{
				CSequenceItem* pItem = new CSequenceItem;
				if ( pItem == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}

				if( !m_fSetPChannel )
				{
					m_fSetPChannel = TRUE;
					m_dwPChannel = iSequence.dwPChannel;
				}

				pItem->CopyFrom(iSequence);

				InsertByAscendingTimeBackwards( pItem );
			}
		}
		pIRiffStream->Ascend(&ck1, 0);
	}

	ck1.ckid = DMUS_FOURCC_CURVE_LIST;
	if(pIRiffStream->Descend(&ck1, &ck, MMIO_FINDCHUNK) == 0)
	{
		DWORD dwSequenceSize, dwByteCount;
		hr = pIStream->Read(&dwSequenceSize, sizeof(DWORD), &dwByteCount);
		if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		dwSequenceSize = min( dwSequenceSize, sizeof(DMUS_IO_CURVE_ITEM) );

		DMUS_IO_CURVE_ITEM iCurve;
		long lChunkSize = ck1.cksize - sizeof(DWORD);
		while(lChunkSize > 0)
		{
			ZeroMemory(	&iCurve, sizeof(DMUS_IO_CURVE_ITEM) );
			hr = pIStream->Read( &iCurve, dwSequenceSize, &dwByteCount );
			if( FAILED( hr ) || dwByteCount != dwSequenceSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			lChunkSize -= dwSequenceSize;

			CCurveItem* pItem = new CCurveItem;
			if ( pItem == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto ON_ERROR;
			}

			if( !m_fSetPChannel )
			{
				m_fSetPChannel = TRUE;
				m_dwPChannel = iCurve.dwPChannel;
			}

			pItem->CopyFrom(iCurve);

			InsertByAscendingTimeBackwards( pItem );
		}
		pIRiffStream->Ascend(&ck1, 0);
	}

ON_ERROR:
	pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::UnselectAllNotes

void CSequenceMgr::UnselectAllNotes()
{
	BOOL fChanged = FALSE;
	POSITION pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		CSequenceItem *pSeqItem = m_lstSequences.GetNext( pos );
		if( pSeqItem->m_fSelected )
		{
			fChanged = TRUE;
			pSeqItem->m_fSelected = FALSE;
		}
	}

	// If anything changed, refresh the display and property pages
	if (fChanged)
	{
		m_pSequenceStrip->InvalidateStrip();
		RefreshPropertyPage( );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::UnselectAllCurves

void CSequenceMgr::UnselectAllCurves()
{
	BOOL fChanged = FALSE;
	POSITION pos = m_lstCurves.GetHeadPosition();
	while( pos )
	{
		CCurveItem *pCurveItem = m_lstCurves.GetNext( pos );
		if( pCurveItem->m_fSelected )
		{
			fChanged = TRUE;
			pCurveItem->m_fSelected = FALSE;
		}
	}

	// If anything changes, refresh the display and property page
	if (fChanged)
	{
		// This also refreshes the curve strip property page(s)
		m_pSequenceStrip->InvalidateCurveStrips();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::PlayPatch

void CSequenceMgr::PlayPatch( MUSIC_TIME mtTime )
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
			// Try and get the instrument on PChannel m_dwPChannel
			BandStrip_InstrumentItem bandStrip_InstrumentItem;
			bandStrip_InstrumentItem.dwPChannel = m_dwPChannel;
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
		pPatchEvent->dwPChannel = m_dwPChannel;
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


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::PlayNote

void CSequenceMgr::PlayNote( CSequenceItem *pSequence )
{
	ASSERT( pSequence );

	if( !pSequence || !m_pIDMPerformance )
	{
		return;
	}

	// Stop any playing note
	StopNote();

	DMUS_MIDI_PMSG *pMIDIOnEvent = NULL;
	if( FAILED( m_pIDMPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG **)&pMIDIOnEvent ) ) )
	{
		return;
	}

	// If there currently is no primary segment playing, try and play the patch for this PChannel
	REFERENCE_TIME rtLatency;
	m_pIDMPerformance->GetLatencyTime( &rtLatency );
	MUSIC_TIME mtLatency;
	m_pIDMPerformance->ReferenceToMusicTime( rtLatency, &mtLatency );

	IDirectMusicSegmentState *pSegState;
	if( FAILED( m_pIDMPerformance->GetSegmentState( &pSegState, mtLatency ) ) )
	{
		PlayPatch( pSequence->AbsTime() );
	}
	else
	{
		pSegState->Release();
	}

	ASSERT( pMIDIOnEvent != NULL );
	ZeroMemory( pMIDIOnEvent, sizeof(DMUS_MIDI_PMSG) );
	// PMSG fields
	pMIDIOnEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
	m_pIDMPerformance->GetLatencyTime( &rtLatency );
	pMIDIOnEvent->rtTime = rtLatency + 5000; // Add 0.5ms
//	pMIDIOnEvent->mtTime = 0;
	pMIDIOnEvent->dwFlags = DMUS_PMSGF_REFTIME;
	pMIDIOnEvent->dwPChannel = m_dwPChannel;
	pMIDIOnEvent->dwVirtualTrackID = 1;
//	pMIDIOnEvent->pTool = NULL;
//	pMIDIOnEvent->pGraph = NULL;
	pMIDIOnEvent->dwType = DMUS_PMSGT_MIDI;
//	pMIDIOnEvent->punkUser = 0;

	// DMNoteEvent fields
	pMIDIOnEvent->bStatus = MIDI_NOTEON;
	pMIDIOnEvent->bByte1 = pSequence->m_bByte1;
	pMIDIOnEvent->bByte2 = pSequence->m_bByte2;
//	pMIDIOnEvent->bPad[0] = 0;

	SendPMsg( (DMUS_PMSG *)pMIDIOnEvent );
	// The playback engine will release the event

	//TRACE("Note On sent at %I64x for %x on %x.\n", pMIDIOffEvent->rtTime, pMIDIOnEvent->bByte1, m_dwPChannel );

	// Reset the time and value of the last note played
	m_rtLastPlayNoteOffTime = rtLatency + 10000;
	m_bLastPlayNoteMIDIValue = pSequence->m_bByte1;

	return;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::StopNote

void CSequenceMgr::StopNote( void )
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

	REFERENCE_TIME rtLatency;
	if( SUCCEEDED( m_pIDMPerformance->GetLatencyTime( &rtLatency ) )
	&&	(rtLatency > m_rtLastPlayNoteOffTime) )
	{
		m_rtLastPlayNoteOffTime = rtLatency;
	}

	ASSERT( pMIDIOffEvent != NULL );
	ZeroMemory( pMIDIOffEvent, sizeof(DMUS_MIDI_PMSG) );
	// PMSG fields
	pMIDIOffEvent->dwSize = sizeof(DMUS_MIDI_PMSG);
	pMIDIOffEvent->rtTime = m_rtLastPlayNoteOffTime;
//	pMIDIOffEvent->mtTime = 0;
	pMIDIOffEvent->dwFlags = DMUS_PMSGF_REFTIME;
	pMIDIOffEvent->dwPChannel = m_dwPChannel;
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

	//TRACE("Note Off sent at %I64x for %x on %x.\n", pMIDIOffEvent->rtTime, pMIDIOnEvent->bByte1, m_dwPChannel );

	// Reset the time and value of the last note played
	m_rtLastPlayNoteOffTime = 0;

	return;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SendPMsg

HRESULT CSequenceMgr::SendPMsg( DMUS_PMSG *pPMsg )
{
	if( !m_pIDMPerformance )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;
	IDirectMusicPerformance8 *pDMPerf8;
	if( SUCCEEDED( m_pIDMPerformance->QueryInterface( IID_IDirectMusicPerformance8, (void **)&pDMPerf8 ) ) )
	{
		IDirectMusicAudioPath *pDMAudioPath;
		if( SUCCEEDED( pDMPerf8->GetDefaultAudioPath( &pDMAudioPath ) ) )
		{
			if( (pPMsg->dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH)
			||	(pPMsg->dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT) )
			{
				pPMsg->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
			}

			if( (pPMsg->dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS)
			||	SUCCEEDED( pDMAudioPath->ConvertPChannel( pPMsg->dwPChannel, &pPMsg->dwPChannel ) ) )
			{
				hr = pDMPerf8->SendPMsg( pPMsg );
			}
			pDMAudioPath->Release();
		}
		pDMPerf8->Release();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::OffsetSelectedNoteDurationAndVelocity

BOOL CSequenceMgr::OffsetSelectedNoteDurationAndVelocity( long lDuration, long lVelocity, BOOL fMakeCopy )
{
	//TODO: Implement fMakeCopy parameter (make a copy of the notes, don't change the existing ones)
	UNREFERENCED_PARAMETER(fMakeCopy);

	BOOL fChange = FALSE;

	CSequenceItem* pSequenceItem;
	POSITION pos2, pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		pSequenceItem = m_lstSequences.GetNext( pos );
		if( pSequenceItem->m_fSelected )
		{
			if( lDuration != 0 )
			{
				long lNewValue = pSequenceItem->m_mtDuration + lDuration;

				if( lNewValue < 1 )
				{
					lNewValue = 1;
				}
				if( lNewValue != pSequenceItem->m_mtDuration )
				{
					// Duplicate the note if fMakeCopy is set
					if( fMakeCopy )
					{
						CSequenceItem *pSeqItem = new CSequenceItem( pSequenceItem );
						pSeqItem->m_fSelected = FALSE;
						m_lstSequences.InsertBefore( pos2, pSeqItem );
					}

					fChange = TRUE;
					pSequenceItem->m_mtDuration = lNewValue;
				}
			}

			if( lVelocity != 0 )
			{
				long lNewValue = pSequenceItem->m_bByte2 + lVelocity;

				if( lNewValue < 1 )
				{
					lNewValue = 1;
				}
				if( lNewValue > 127 )
				{
					lNewValue = 127;
				}
				if( lNewValue != pSequenceItem->m_bByte2 )
				{
					// Duplicate the note if fMakeCopy is set
					if( fMakeCopy )
					{
						CSequenceItem *pSeqItem = new CSequenceItem( pSequenceItem );
						pSeqItem->m_fSelected = FALSE;
						m_lstSequences.InsertBefore( pos2, pSeqItem );
					}

					fChange = TRUE;
					pSequenceItem->m_bByte2 = (BYTE)lNewValue;

					if( pSequenceItem == m_pSequenceStrip->m_pActiveNote )
					{
						PlayNote( pSequenceItem );
					}
				}
			}
		}
	}

	if( fChange )
	{
		m_pSequenceStrip->InvalidateStrip();
		RefreshPropertyPage();
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::OffsetSelectedNoteValuePositionAndStart

BOOL CSequenceMgr::OffsetSelectedNoteValuePositionAndStart( int nValue, long lMoveOffset, long lStartOffset, BOOL fMakeCopy )
{
	//TODO: Implement fMakeCopy parameter (make a copy of the notes, don't change the existing ones)
	UNREFERENCED_PARAMETER(fMakeCopy);

	BOOL fChange = FALSE;

	// Compute the length of the pickup bar
	long lEarlyMeasureClocks = 0;
	DMUS_TIMESIGNATURE ts;
	if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, m_dwIndex, 0, NULL, &ts ) ) )
	{
		lEarlyMeasureClocks = -ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat);
	}

	// Compute the length of the extension bar
	long lLateMeasureClocks = 0;
	if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, m_dwIndex, m_pSequenceStrip->m_mtLength - 1, NULL, &ts ) ) )
	{
		lLateMeasureClocks = ts.bBeatsPerMeasure * ((DMUS_PPQ * 4) / ts.bBeat);
	}

	CTypedPtrList<CPtrList, CSequenceItem*> lstMovedSequences;
	CSequenceItem* pSequenceItem;
	POSITION pos2, pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		pSequenceItem = m_lstSequences.GetNext( pos );
		if( pSequenceItem->m_fSelected )
		{
			if( nValue != 0 )
			{
				int nNewValue = pSequenceItem->m_bByte1 + nValue;

				if( nNewValue < 0 )
				{
					nNewValue = 0;
				}
				if( nNewValue > 127 )
				{
					nNewValue = 127;
				}
				if( nNewValue != pSequenceItem->m_bByte1 )
				{
					// Duplicate the note if fMakeCopy is set
					if( fMakeCopy )
					{
						CSequenceItem *pSeqItem = new CSequenceItem( pSequenceItem );
						pSeqItem->m_fSelected = FALSE;
						m_lstSequences.InsertBefore( pos2, pSeqItem );
					}

					fChange = TRUE;
					pSequenceItem->m_bByte1 = (BYTE)nNewValue;

					if( pSequenceItem == m_pSequenceStrip->m_pActiveNote )
					{
						PlayNote( pSequenceItem );
					}
				}
			}

			// TODO: These could be optimized into two functions, one for lMoveOffset < 0
			// (using GetPrev() to step through the list, and one for lMoveOffset > 0
			// (using GetNext() to step through the list.
			if( lMoveOffset != 0 )
			{
				// Note: Don't use AbsTime() here - we only want to modify m_mtTime
				long lNewValue = pSequenceItem->MusicTime() + lMoveOffset;

				if( lNewValue < 0 )
				{
					lNewValue = 0;
				}
				if( lNewValue >= m_pSequenceStrip->m_mtLength )
				{
					lNewValue = m_pSequenceStrip->m_mtLength - 1;
				}

				// Now ensure lAbsTime does not put the event more than one measure ahead of the sequence
				long lAbsTime = lNewValue + pSequenceItem->m_nOffset;
				if(lAbsTime < lEarlyMeasureClocks )
				{
					lAbsTime = lEarlyMeasureClocks;
				}

				// Now ensure lStartTime does not put the event more than one measure after the segment
				if( lAbsTime > m_pSequenceStrip->m_mtLength - 1 + lLateMeasureClocks )
				{
					lAbsTime = m_pSequenceStrip->m_mtLength - 1 + lLateMeasureClocks;
				}

				lNewValue = lAbsTime - pSequenceItem->m_nOffset;

				/*
				long lNewMusicTime = pSequenceItem->MusicTime();
				long lNewOffset = pSequenceItem->m_nOffset;

				ASSERT( lNewMusicTime >= 0 );

				if( (lNewMusicTime == 0)
				&&	(lNewOffset < 0) )
				{
					lNewOffset += lMoveOffset;
					if( lNewOffset > 0 )
					{
						lNewMusicTime = lNewOffset;
						lNewOffset = 0;

						// Ensure the music time is before the segment's end
						if( lNewMusicTime >= m_pSequenceStrip->m_mtLength )
						{
							lNewMusicTime = m_pSequenceStrip->m_mtLength - 1;
						}
					}

					// Now ensure the event is not more than one measure ahead of the sequence
					ASSERT( lNewMusicTime + lNewOffset >= lEarlyMeasureClocks );
					//if( lNewMusicTime + lNewOffset < lEarlyMeasureClocks )
					//{
					//	lNewOffset = lEarlyMeasureClocks;
					//}
				}
				else
				{
					lNewMusicTime += lMoveOffset;

					if( lNewMusicTime < 0 )
					{
						lNewOffset += lNewMusicTime;
						lNewMusicTime = 0;
					}
					if( lNewMusicTime >= m_pSequenceStrip->m_mtLength )
					{
						lNewMusicTime = m_pSequenceStrip->m_mtLength - 1;
					}

					// Now ensure the event is not more than one measure ahead of the sequence
					ASSERT( lNewMusicTime + lNewOffset >= lEarlyMeasureClocks );
					//if( lNewMusicTime + lNewOffset < lEarlyMeasureClocks )
					//{
					//	lNewOffset = lEarlyMeasureClocks - lNewMusicTime;
					//}
				}

				lNewOffset = max( SHRT_MIN, min( SHRT_MAX, lNewOffset ) );

				// Now ensure lStartTime does not put the event more than one measure after the segment
				ASSERT( lNewMusicTime <= m_pSequenceStrip->m_mtLength - 1 + lLateMeasureClocks - lNewOffset );
				//if( lNewMusicTime > m_pSequenceStrip->m_mtLength - 1 + lLateMeasureClocks - lNewOffset)
				//{
				//	lNewMusicTime = m_pSequenceStrip->m_mtLength - 1 + lLateMeasureClocks - lNewOffset;
				//}

				if( (lNewMusicTime != pSequenceItem->MusicTime())
				||	(lNewOffset != pSequenceItem->m_nOffset) )
				*/
				if( lNewValue != pSequenceItem->MusicTime() )
				{
					// Duplicate the note if fMakeCopy is set
					if( fMakeCopy )
					{
						CSequenceItem *pSeqItem = new CSequenceItem( pSequenceItem );
						pSeqItem->m_fSelected = FALSE;
						if( lMoveOffset < 0 )
						{
							m_lstSequences.InsertAfter( pos2, pSeqItem );
						}
						else
						{
							m_lstSequences.InsertBefore( pos2, pSeqItem );
						}
					}

					fChange = TRUE;
					pSequenceItem->MusicTime() = lNewValue;
					/*
					pSequenceItem->m_nOffset = short(lNewOffset);
					*/

					// If lMoveOffset < 0, only add to lstMovedSequences if we're now before
					// the item before the one at pos2.
					if( lMoveOffset < 0 )
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

					// If lMoveOffset > 0, only add to lstMovedSequences if we're now after
					// the item at pos.
					else if( pos && lMoveOffset > 0 )
					{
						//CSequenceItem* pItem = m_lstSequences.GetAt( pos );
						if( pSequenceItem->AbsTime() > m_lstSequences.GetAt( pos )->AbsTime() )
						{
							m_lstSequences.RemoveAt( pos2 );
							lstMovedSequences.AddTail( pSequenceItem );
						}
					}
				}
			}

			// TODO: These could be optimized into two functions, one for lStartOffset < 0
			// (using GetPrev() to step through the list, and one for lStartOffset > 0
			// (using GetNext() to step through the list.
			if( lStartOffset != 0 )
			{
				/*
				long lNewMusicTime = pSequenceItem->MusicTime();
				long lNewOffset = pSequenceItem->m_nOffset;
				long lNewDuration = pSequenceItem->m_mtDuration - lStartOffset;

				ASSERT( lNewDuration > 0 );

				ASSERT( lNewMusicTime >= 0 );

				if( (lNewMusicTime == 0)
				&&	(lNewOffset <= 0) )
				{
					lNewOffset += lMoveOffset;
					if( lNewOffset > 0 )
					{
						lNewMusicTime = lNewOffset;
						lNewOffset = 0;
					}

					// Ensure the event is not more than one measure ahead of the sequence
					ASSERT( lNewMusicTime + lNewOffset >= lEarlyMeasureClocks );

					// Ensure the event starts before the end of the segment
					ASSERT( lNewMusicTime < m_pSequenceStrip->m_mtLength );
				}
				else
				{
					lNewMusicTime += lMoveOffset;

					// Ensure the event is not more than one measure ahead of the sequence
					if( lNewMusicTime + lNewOffset < lEarlyMeasureClocks )
					{
						lNewMusicTime += lEarlyMeasureClocks - (lNewMusicTime + lNewOffset);
					}

					if( lNewMusicTime < 0 )
					{
						lNewOffset += lNewMusicTime;
						lNewMusicTime = 0;
					}
					if( lNewMusicTime >= m_pSequenceStrip->m_mtLength )
					{
						lNewMusicTime = m_pSequenceStrip->m_mtLength - 1;
					}
				}
				*/

				// Note: Don't use AbsTime() here - we only want to modify m_mtTime
				long lNewValue = pSequenceItem->MusicTime() + lStartOffset;
				long lNewDuration = pSequenceItem->m_mtDuration - lStartOffset;

				if( lNewValue < 0 )
				{
					lNewDuration += lNewValue;
					lNewValue = 0;
				}
				if( lNewValue >= m_pSequenceStrip->m_mtLength )
				{
					lNewDuration += lNewValue - m_pSequenceStrip->m_mtLength - 1;
					lNewValue = m_pSequenceStrip->m_mtLength - 1;
				}
				if( lNewDuration <= 0 )
				{
					lNewValue += lNewDuration - 1;
					lNewDuration = 1;
				}

				// Now ensure lAbsTime does not put the event more than one measure ahead of the sequence
				long lAbsTime = lNewValue + pSequenceItem->m_nOffset;
				if( lAbsTime < lEarlyMeasureClocks )
				{
					lAbsTime = lEarlyMeasureClocks;
				}

				// Now ensure lStartTime does not put the event more than one measure after the segment
				if( lAbsTime > m_pSequenceStrip->m_mtLength - 1 + lLateMeasureClocks )
				{
					lAbsTime = m_pSequenceStrip->m_mtLength - 1 + lLateMeasureClocks;
				}

				lNewValue = lAbsTime - pSequenceItem->m_nOffset;

				if( lNewValue != pSequenceItem->MusicTime() )
				{
					fChange = TRUE;
					pSequenceItem->MusicTime() = lNewValue;
					pSequenceItem->m_mtDuration = lNewDuration;

					// If lStartOffset < 0, only add to lstMovedSequences if we're now before
					// the item before the one at pos2.
					if( lStartOffset < 0 )
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

					// If lStartOffset > 0, only add to lstMovedSequences if we're now after
					// the item at pos.
					else if( pos && lStartOffset > 0 )
					{
						//CSequenceItem* pItem = m_lstSequences.GetAt( pos );
						if( pSequenceItem->AbsTime() > m_lstSequences.GetAt( pos )->AbsTime() )
						{
							m_lstSequences.RemoveAt( pos2 );
							lstMovedSequences.AddTail( pSequenceItem );
						}
					}
				}
			}
		}
	}

	if( fChange )
	{
		if( !lstMovedSequences.IsEmpty() )
		{
			MergeSequence( lstMovedSequences );
		}

		m_pSequenceStrip->InvalidateStrip();
		RefreshPropertyPage();
	}

	return fChange;
}

void CSequenceMgr::MergeSequence( CTypedPtrList<CPtrList, CSequenceItem*>& lstSequences )
{
	// We assume lstSequences is in sorted (ascending) order
	if( lstSequences.IsEmpty() )
	{
		return;
	}

	POSITION pos = m_lstSequences.GetHeadPosition();
	while( !lstSequences.IsEmpty() )
	{
		CSequenceItem *pMergeSeq = lstSequences.RemoveHead();

		while( pos && (pMergeSeq->AbsTime() > m_lstSequences.GetAt(pos)->AbsTime()) )
		{
			m_lstSequences.GetNext( pos );
		}

		if( pos )
		{
			m_lstSequences.InsertBefore( pos, pMergeSeq );
		}
		else
		{
			m_lstSequences.AddTail( pMergeSeq );
		}
	}
}

void CSequenceMgr::MergeCurve( CTypedPtrList<CPtrList, CCurveItem*>& lstCurves )
{
	// We assume lstCurves is in sorted (ascending) order
	if( lstCurves.IsEmpty() )
	{
		return;
	}

	POSITION pos = m_lstCurves.GetHeadPosition();
	while( !lstCurves.IsEmpty() )
	{
		CCurveItem *pMergeCurve = lstCurves.RemoveHead();

		while( pos && (pMergeCurve->AbsTime() > m_lstCurves.GetAt(pos)->AbsTime()) )
		{
			m_lstCurves.GetNext( pos );
		}

		if( pos )
		{
			m_lstCurves.InsertBefore( pos, pMergeCurve );
		}
		else
		{
			m_lstCurves.AddTail( pMergeCurve );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::GetNumSelected

int CSequenceMgr::GetNumSelected()
{
	int nRes = 0;
	POSITION pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		if( m_lstSequences.GetNext(pos)->m_fSelected )
		{
			nRes++;
		}
	}
	return nRes;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::RefreshPropertyPage

void CSequenceMgr::RefreshPropertyPage( )
{
	bool fFoundOne = false;
	POSITION pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		CSequenceItem *pTempSeq = m_lstSequences.GetNext( pos );

		if( pTempSeq->m_fSelected )
		{
			if( fFoundOne )
			{
				CPropSequence propseq(this);
				propseq.Import(pTempSeq);
				m_PropSequence += propseq;
			}
			else
			{
				m_PropSequence.Import(pTempSeq);
				fFoundOne = true;
			}
		}
	}

	// If nothing selected, re-initialize m_PropSequence
	if (!fFoundOne)
	{
		m_PropSequence.Clear();
	}

	//if (!m_fNoDisplayUpdate)
	{
		// Let the property page know about the changes
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}

	UpdateStatusBarDisplay();
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::ApplyToSelectedNotes

DWORD CSequenceMgr::ApplyToSelectedNotes( const CPropSequence* pPropNote )
{
	ASSERT( pPropNote != NULL );
	if ( pPropNote == NULL )
	{
		return 0;
	}

	// A list to temporarily store the sequences in if their start times are moved
	CTypedPtrList<CPtrList, CSequenceItem*> lstMovedSequences;

	DWORD dwChanged = 0;
	POSITION pos2, pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		pos2 = pos;
		CSequenceItem *pTempSeq = m_lstSequences.GetNext( pos );

		if ( pTempSeq->m_fSelected )
		{
			// If we should copy the note
			if( !(pPropNote->m_dwUndetermined & UD_COPY) )
			{
				// Create a new note
				CSequenceItem *pNewSeq;
				pNewSeq = new CSequenceItem;
				pNewSeq->Copy( pTempSeq );

				// Insert just after the existing note
				pos2 = m_lstSequences.InsertAfter( pos2, pNewSeq );

				pTempSeq->m_fSelected = FALSE;
				pTempSeq = pNewSeq;
			}

			// Save the old start time
			long lOldValue = pTempSeq->AbsTime();

			if (!(pPropNote->m_dwChanged & (UD_CHORDBAR | UD_CHORDBEAT| UD_STARTBARBEATGRIDTICK | UD_ENDBARBEATGRIDTICK | UD_DURBARBEATGRIDTICK)) )
			{
				// We're not changing any start/end/dur bar/beat/grid/tick

				// Apply the changes
				dwChanged |= pPropNote->ApplyToNote(pTempSeq);
			}
			else
			{
				// We're changing at least one of start/end/dur/chord bar/beat/grid/tick
				ASSERT( ~pPropNote->m_dwUndetermined & (UD_CHORDBAR | UD_CHORDBEAT| UD_STARTBARBEATGRIDTICK | UD_ENDBARBEATGRIDTICK | UD_DURBARBEATGRIDTICK) );
				CPropSequence propnote( pTempSeq, this );
				propnote.ApplyPropNote( pPropNote );

				// Duration, GridStart, and Offset should all be determined
				ASSERT( (propnote.m_dwUndetermined & (UD_DURATION | UD_TIMESTART | UD_OFFSET)) == 0);
				propnote.m_dwUndetermined = DWORD(~(UD_DURATION | UD_TIMESTART | UD_OFFSET));

				// Apply the changes
				dwChanged |= propnote.ApplyToNote(pTempSeq);
			}

			// If the note's start position changed, remove and re-add it to the list
			if( lOldValue != pTempSeq->AbsTime() )
			{
				// If moved backwards, only add to lstMovedSequences if we're now before
				// the item before the one at pos2.
				if( pTempSeq->AbsTime() < lOldValue )
				{
					POSITION pos3 = pos2;
					m_lstSequences.GetPrev( pos3 );
					if( pos3 )
					{
						//CSequenceItem* pItem = m_lstSequences.GetAt( pos3 );
						if( pTempSeq->AbsTime() < m_lstSequences.GetAt( pos3 )->AbsTime() )
						{
							// Need to move pTempSeq, so remove it from its current location
							m_lstSequences.RemoveAt( pos2 );

							lstMovedSequences.AddTail( pTempSeq );

							// This should be fine
							//InsertByAscendingTimeBackwardsFrom( pTempSeq, pos3 );
						}
					}
				}
				// If moved forwards, only add to lstMovedSequences if we're now after
				// the item at pos.
				else if( pos )
				{
					//CSequenceItem* pItem = m_lstSequences.GetAt( pos );
					if( pTempSeq->AbsTime() > m_lstSequences.GetAt( pos )->AbsTime() )
					{
						// Need to move pTempSeq, so remove it from its current location
						m_lstSequences.RemoveAt( pos2 );

						lstMovedSequences.AddTail( pTempSeq );

						// If we do this, the notes will be changed repeatedly...
						//InsertByAscendingTimeFrom( pTempSeq, pos );
					}
				}
			}
		}
	}

	if( !lstMovedSequences.IsEmpty() )
	{
		MergeSequence( lstMovedSequences );
	}

	return dwChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::InsertByAscendingTimeFrom
/*
void CSequenceMgr::InsertByAscendingTimeFrom( CSequenceItem *pSequence, POSITION pos )
{
	// This optimization probably isn't worth the space - jhd
	// Move pos3 to the next item in the list (since we already
	// checked the one at pos3)
	m_lstSequences.GetNext( pos3 );
	if( pos3 )
	{
		pItem = m_lstSequends.GetAt( pos3 );
	}

	// Iterate until pos3 points to the location to insert after
	POSITION pos3 = pos;
	while( pos3 && (pTempSeq->AbsTime() > pItem->AbsTime()) )
	{
		m_lstSequences.GetNext( pos3 );
		if( pos3 )
		{
			pItem = m_lstSequends.GetAt( pos3 );
		}
	}

	// Insert pTempSeq back into the correct point in the list
	if( pos3 )
	{
		m_lstSequences.InsertBefore( pos3, pTempSeq );
	}
	else
	{
		m_lstSequences.AddTail( pTempSeq );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::InsertByAscendingTimeBackwardsFrom

void CSequenceMgr::InsertByAscendingTimeBackwardsFrom( CSequenceItem *pSequence, POSITION pos )
{
	// This optimization probably isn't worth the space - jhd
	// Move pos3 to the previous item in the list (since we already
	// checked the one at pos3)
	m_lstSequences.GetPrev( pos3 );
	if( pos3 )
	{
		pItem = m_lstSequends.GetAt( pos3 );
	}

	// Iterate until pos3 points to the location to insert after
	while( pos3 && (pTempSeq->AbsTime() < pItem->AbsTime()) )
	{
		m_lstSequences.GetPrev( pos3 );
		if( pos3 )
		{
			pItem = m_lstSequends.GetAt( pos3 );
		}
	}

	// Insert pTempSeq back into the correct point in the list
	if( pos3 )
	{
		m_lstSequences.InsertAfter( pos3, pTempSeq );
	}
	else
	{
		m_lstSequences.AddHead( pTempSeq );
	}
	}
*/


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::InsertNote

void CSequenceMgr::InsertNote( CSequenceItem *pSequence, BOOL fUpdateSegment )
{
	//TRACE("InsertNote In %d\n",  ::timeGetTime());
	ASSERT( pSequence != NULL );
	if( pSequence == NULL )
	{
		return;
	}

	InsertByAscendingTime( pSequence );
	m_fDirty = TRUE;

	if( fUpdateSegment )
	{
		// Let the object know about the changes
		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT;
		OnDataChanged( );
	}
	else
	{
		m_fNoteInserted = TRUE;
	}

	// Invalidate only this note
	RECT rect;
	if( m_pSequenceStrip->m_svView == SV_MINIMIZED )
	{
		rect.top = 0;
		rect.bottom = MINIMIZE_HEIGHT;
		m_pTimeline->ClocksToPosition( pSequence->AbsTime(), &rect.left );
		m_pTimeline->ClocksToPosition( pSequence->AbsTime() + pSequence->m_mtDuration, &rect.right );
	}
	else
	{
		m_pSequenceStrip->GetNoteRect( pSequence, &rect );
		if( m_pSequenceStrip->m_fHybridNotation )
		{
			// Extend to the left so the accidental is drawn
			rect.left -= ((m_pSequenceStrip->m_lMaxNoteHeight * 3) / 2) + 2;

			// Extend to the top and bottom so the parts of the accidental that extend outside
			// the note are drawn
			rect.top -= m_pSequenceStrip->m_lMaxNoteHeight / 2;
			rect.bottom += m_pSequenceStrip->m_lMaxNoteHeight / 2;

			// Extend to the end of the measure
			long lTime;
			m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, m_dwIndex, pSequence->AbsTime(), &lTime, NULL );
			m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, m_dwIndex, lTime + 1, 0, &lTime );

			// If the end of the measure is after the end of the note, extent the
			// invalidation to the right
			if( lTime > pSequence->AbsTime() + pSequence->m_mtDuration )
			{
				m_pTimeline->ClocksToPosition( lTime, &rect.right );
			}

			// Ensure the measure line is drawn correctly
			rect.right++;
		}
	}

	m_pTimeline->StripInvalidateRect( m_pSequenceStrip, &rect, FALSE );
	//TRACE("InsertNote out %d\n",  ::timeGetTime());
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::InsertCurve

void CSequenceMgr::InsertCurve( CCurveItem *pCurve, BOOL fUpdateSegment )
{
	//TRACE("InsertCurve In %d\n",  ::timeGetTime());
	ASSERT( pCurve != NULL );
	if( pCurve == NULL )
	{
		return;
	}

	InsertByAscendingTime( pCurve );
	m_fDirty = TRUE;

	if( fUpdateSegment )
	{
		// Let the object know about the changes
		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_INSERT_CURVE;
		OnDataChanged( );
	}
	else
	{
		m_fCurveInserted = TRUE;
	}

	// Invalidate only this curve strip (no need to redraw if the sequence strip is minimized)
	if( m_pSequenceStrip->m_svView != SV_MINIMIZED )
	{
		if( m_pSequenceStrip->m_CurveStripView != SV_MINIMIZED )
		{
			CCurveStrip* pCurveStrip = m_pSequenceStrip->GetCurveStrip( m_pSequenceStrip->CurveTypeToStripCCType( pCurve ), pCurve->m_wParamType );
			ASSERT( pCurveStrip );
			if( pCurveStrip )
			{
				m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)pCurveStrip, NULL, TRUE );
			}
		}
		else
		{
			// Redraw the minimized curve strip
			CCurveStrip* pCurveStrip = m_pSequenceStrip->GetCurveStrip( CCTYPE_MINIMIZED_CURVE_STRIP, 0 );
			ASSERT( pCurveStrip );
			if( pCurveStrip )
			{
				m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)pCurveStrip, NULL, TRUE );
			}
		}
	}

	//TRACE("InsertCurve out %d\n",  ::timeGetTime());
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::UpdateFlatsAndKey

BOOL CSequenceMgr::UpdateFlatsAndKey()
{
	BOOL fChange = FALSE;

	// Get the show Flats/Sharps flag
	BOOL fShowFlats = FALSE;
	if( m_pTimeline && m_pSequenceStrip && SUCCEEDED( m_pTimeline->GetParam( GUID_ChordSharpsFlats, m_dwGroupBits, m_dwIndex, 0, NULL, &fShowFlats) ) )
	{
		if( m_fDisplayingFlats != fShowFlats )
		{
			m_fDisplayingFlats = fShowFlats;
			fChange = TRUE;
		}
	}

	DWORD dwKey = 0;
	if( m_pTimeline && m_pSequenceStrip &&
		SUCCEEDED( m_pTimeline->GetParam( GUID_ChordKey, m_dwGroupBits, m_dwIndex, 0, NULL, &dwKey) ) )
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
		m_pSequenceStrip->UpdateKeyPattern();

		// If displaying in hybrid notation
		if( m_pSequenceStrip->m_fHybridNotation )
		{
			// Redraw our strip
			m_pSequenceStrip->InvalidateStrip();
		}

		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}

	return fChange;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::LoadSequenceUIList

HRESULT CSequenceMgr::LoadSequenceUIList( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO *pckMain )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwByteCount;
	DWORD dwSize;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_SEQUENCE_UI_CHUNK:
			{
				ioSeqStripDesign iSeqStripDesign;

				InitializeSeqStripDesign( &iSeqStripDesign );
				dwSize = min( ck.cksize, sizeof( ioSeqStripDesign ) );
				hr = pIStream->Read( &iSeqStripDesign, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Check for garbage data from earlier versions of file format
				if( iSeqStripDesign.m_dwExtraBars > 999 )
				{
					iSeqStripDesign.m_dwExtraBars = 0;
				}
				if( iSeqStripDesign.m_fPickupBar > 1 )
				{
					iSeqStripDesign.m_fPickupBar = FALSE;
				}

				memcpy( &m_SeqStripDesign, &iSeqStripDesign, dwSize );
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

				if( m_pSequenceStrip )
				{
					m_pSequenceStrip->m_CurveStripView = (STRIPVIEW)iGlobalCurveStripState.m_nCurveStripView;
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

				if( m_pSequenceStrip )
				{
					m_pSequenceStrip->m_lstCurveStripStates.AddTail( pCurveStripState );
				}
				break;
			}
		}

        pIRiffStream->Ascend( &ck, 0 );
	}

	if( m_pTimeline )
	{
		// Make sure there is a curve strip for each type of Curve in this sequence
		m_pSequenceStrip->SyncCurveStripStateList();
		m_pSequenceStrip->ApplyUIChunk( &m_SeqStripDesign );

		// Add Curve Strips to the Timeline
		m_pSequenceStrip->AddCurveStrips();
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::SaveSequenceUIList

HRESULT CSequenceMgr::SaveSequenceUIList( IDMUSProdRIFFStream* pIRiffStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	IStream* pIStream = NULL;
	HRESULT hr = S_OK;
	MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	// Update PChannel #
	m_SeqStripDesign.m_dwPChannel = m_dwPChannel;

	if( m_pSequenceStrip )
	{
		// Write SequenceUI list header
		ckMain.fccType = DMUS_FOURCC_SEQUENCE_UI_LIST;
		if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write SequenceUI chunk header
		ck.ckid = DMUS_FOURCC_SEQUENCE_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare ioSeqStripDesign structure
		m_pSequenceStrip->UpdateSequenceUIChunk();

		// Write SequenceStrip chunk data
		hr = pIStream->Write( &m_SeqStripDesign, sizeof(ioSeqStripDesign), &dwBytesWritten);
		if( FAILED( hr ) || dwBytesWritten != sizeof(ioSeqStripDesign) )
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
		m_pSequenceStrip->SaveCurveStripStateData( pIRiffStream );

		// Ascend out of SequenceUI list chunk
		if( pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

ON_ERROR:
	if( pIStream != NULL )
	{
		pIStream->Release();
	}
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::AddNewPart

HRESULT CSequenceMgr::AddNewPart( void )
{
	// Have user select a PChannel
	CNewPartDlg newPartDlg;

	newPartDlg.SetTrack( 0 );
	newPartDlg.SetSequenceMgr( this );

	// The dialog handles adding parts itself
	newPartDlg.DoModal();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::Quantize

BOOL CSequenceMgr::Quantize( SequenceQuantize* pSequenceQuantize )
{
	CSequenceItem* pSequenceItem;

	BOOL fChanged = FALSE;

	if( pSequenceQuantize->m_bStrength )
	{
		// Flag if we should modify all notes
		const bool fUseAllNotes = (pSequenceQuantize->m_wQuantizeTarget != QUANTIZE_TARGET_SELECTED);

		// Iterate through all events
		// Only quantize notes
		POSITION pos = m_lstSequences.GetHeadPosition();
		while( pos )
		{
			pSequenceItem = m_lstSequences.GetNext( pos );

			if( fUseAllNotes || pSequenceItem->m_fSelected )
			{
				if( QuantizeNote( pSequenceItem, pSequenceQuantize ) )
				{
					fChanged = TRUE;
				}
			}
		}
	}

	if( fChanged )
	{
		m_fDirty = TRUE;

		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_QUANTIZE;
		OnDataChanged();
		m_pSequenceStrip->InvalidateStrip();

		// Always recompute m_SelectedPropNote and update the property page
		RefreshPropertyPage();
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::QuantizeNote

BOOL CSequenceMgr::QuantizeNote( CSequenceItem* pSequenceItem, SequenceQuantize* pSequenceQuantize )
{
	DMUS_TIMESIGNATURE ts;
	MUSIC_TIME mtBeatClocks;
	MUSIC_TIME mtMeasureClocks;
	MUSIC_TIME mtGridClocks;
	BOOL fChanged = FALSE;

	ASSERT( pSequenceItem );
	ASSERT( pSequenceQuantize );
	ASSERT( pSequenceQuantize->m_bResolution );

	if( pSequenceQuantize->m_bStrength )
	{
		if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, m_dwIndex, pSequenceItem->m_mtTime, NULL, &ts ) ) )
		{
			// Compute the number of clocks per Measure, Beat, and Grid
			mtBeatClocks = (DMUS_PPQ * 4) / ts.bBeat;
			mtMeasureClocks = mtBeatClocks * ts.bBeatsPerMeasure;
			mtGridClocks = mtBeatClocks / ts.wGridsPerBeat;
			
			if( pSequenceQuantize->m_dwFlags & SEQUENCE_QUANTIZE_START_TIME )
			{
				// Split m_nOffset into 2 parts
				// 1 - Clocks for an evenly disible number of beats
				//     this represents prop page 'Belongs to Beat' 
				// 2 - Remaining ticks
				//     when m_nOffset is not zero, this represents the tru offset of the note
				long lOrigBeatClocksInOffset, lOrigBeatOffsetClocksInOffset;
				lOrigBeatClocksInOffset = (pSequenceItem->m_nOffset / mtBeatClocks) * mtBeatClocks;
				lOrigBeatOffsetClocksInOffset = pSequenceItem->m_nOffset % mtBeatClocks;

				// Compute the Measure, Beat, and Grid this note occurs in
				long lTempMeas, lTempBeat, lTempGrid, lTempTick, lTempBeatOffset;
				lTempMeas = pSequenceItem->m_mtTime / mtMeasureClocks;
				lTempBeat = pSequenceItem->m_mtTime % mtMeasureClocks;
				lTempGrid = lTempBeat % mtBeatClocks;
				lTempTick = lTempGrid % mtGridClocks;
				lTempBeat /= mtBeatClocks;
				lTempGrid /= mtGridClocks;

				// Compute our offset from lTempMeas and lTempBeat
				lTempBeatOffset = lOrigBeatOffsetClocksInOffset + lTempTick + (lTempGrid * mtGridClocks);

				BOOL fMeasureBeatMatchesChord = TRUE;
				if( (lTempBeatOffset < 0) || (lTempBeatOffset >= mtMeasureClocks) )
				{
					fMeasureBeatMatchesChord = FALSE;
				}

				// Compute how many clocks in a quantize unit
				MUSIC_TIME mtQuantizeClocks;
				mtQuantizeClocks = mtBeatClocks / pSequenceQuantize->m_bResolution;

				// Compute how many quantize units the offset is away from lTempMeas and lTempBeat
				if( lTempBeatOffset >= 0 )
				{
					lTempBeatOffset = (lTempBeatOffset + mtQuantizeClocks/2) / mtQuantizeClocks;
				}
				else
				{
					lTempBeatOffset = (lTempBeatOffset + -mtQuantizeClocks/2) / mtQuantizeClocks;
				}

				// lTempBeatOffset will now contain the new, 100% quantized offset
				// Use mtBeatClocks for each beat, mtQuantizeClocks for each grid.
				// (Otherwise, rounding errors may occur).
				lTempBeatOffset = (lTempBeatOffset % pSequenceQuantize->m_bResolution) * mtQuantizeClocks +
								  (lTempBeatOffset / pSequenceQuantize->m_bResolution) * mtBeatClocks;

				// lDeltaOffset is the amount for 100% quantization
				long lDeltaOffset = lTempBeatOffset - (lOrigBeatOffsetClocksInOffset + lTempTick + (lTempGrid * mtGridClocks));

				// Now, scale it based on bStrength
				lDeltaOffset = (lDeltaOffset * pSequenceQuantize->m_bStrength) / 100;

				if( lDeltaOffset )
				{
					fChanged = TRUE;
					// Calculate the new offset from lTempMeas and lTempBeat
					lTempBeatOffset = lOrigBeatOffsetClocksInOffset + lTempTick + (lTempGrid * mtGridClocks) + lDeltaOffset;

					// Calculate the new Bar,Beat,Grid and offset
					lTempGrid = 0;
					if( fMeasureBeatMatchesChord )
					{
						// Move lTempBeat and/or lTempMeasure to match the new location

						// Move by beats
						while( lTempBeatOffset <= -mtBeatClocks )
						{
							if( lTempBeat )
							{
								lTempBeat--;
							}
							else if( lTempMeas )
							{
								lTempMeas--;
								lTempBeat = ts.bBeatsPerMeasure - 1;
							}
							else
							{
								break;
							}
							lTempBeatOffset += mtBeatClocks;
						}
						while( lTempBeatOffset >= mtBeatClocks )
						{
							if( lTempBeat < ts.bBeatsPerMeasure - 1 )
							{
								lTempBeat++;
							}
							else
							{
								lTempMeas++;
								lTempBeat = 0;
							}
							lTempBeatOffset -= mtBeatClocks;
						}

						// Move by grids
						while( lTempBeatOffset < 0 )
						{
							if( lTempGrid )
							{
								lTempGrid--;
							}
							else if( lTempBeat )
							{
								lTempBeat--;
								lTempGrid = ts.wGridsPerBeat - 1;
							}
							else if ( lTempMeas )
							{
								lTempMeas--;
								lTempBeat = ts.bBeatsPerMeasure - 1;
								lTempGrid = ts.wGridsPerBeat - 1;
							}
							else
							{
								break;
							}
							lTempBeatOffset += mtGridClocks;
						}
						while( lTempBeatOffset >= mtGridClocks )
						{
							if( lTempGrid < ts.wGridsPerBeat - 1 )
							{
								lTempGrid++;
							}
							else if( lTempBeat < ts.bBeatsPerMeasure - 1 )
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
							lTempBeatOffset -= mtGridClocks;
						}
					}
					else
					{
						// Don't touch lTempBeat or lTempMeas
						while( (lTempGrid < ts.wGridsPerBeat - 1) && (lTempBeatOffset > mtGridClocks / 2) )
						{
							lTempGrid++;
							lTempBeatOffset -= mtGridClocks;
						}
					}

					long lOldStartTime = pSequenceItem->AbsTime();

					// Set the note's m_mtTime and m_nOffset
					ASSERT( (lTempBeatOffset < SHRT_MAX) && (lTempBeatOffset > SHRT_MIN) );
					if( pSequenceItem->m_nOffset )
					{
						// User set 'Belongs to Beat' fields 
						// Offset belongs in m_nOffset
						pSequenceItem->m_nOffset = (short)(lTempBeatOffset + lOrigBeatClocksInOffset);
						pSequenceItem->m_mtTime = (lTempMeas * mtMeasureClocks) +
												  (lTempBeat * mtBeatClocks) +
												  (lTempGrid * mtGridClocks);
					}
					else
					{
						// User did not set 'Belongs to Beat' fields 
						// Offset belongs in m_mtTime
						pSequenceItem->m_nOffset = 0;
						pSequenceItem->m_mtTime = (lTempMeas * mtMeasureClocks) +
												  (lTempBeat * mtBeatClocks) +
												  (lTempGrid * mtGridClocks) +
												   lTempBeatOffset;
					}

					if( lOldStartTime != pSequenceItem->AbsTime() )
					{
						// Remove the event from the part's list
						POSITION pos = m_lstSequences.Find( pSequenceItem );
						ASSERT( pos != NULL);	// This should not happen!
						if( pos )
						{
							m_lstSequences.RemoveAt( pos );

							// And re-insert it in order
							InsertByAscendingTime( pSequenceItem );
						}
					}
				}
			}

			if( pSequenceQuantize->m_dwFlags & SEQUENCE_QUANTIZE_DURATION )
			{
				// Compute the Measure, Beat, and Grid of the duration
				long lTempMeas, lTempBeat, lTempBeatOffset;
				lTempMeas = pSequenceItem->m_mtDuration / mtMeasureClocks;
				lTempBeat = (pSequenceItem->m_mtDuration % mtMeasureClocks) / mtBeatClocks;

				// Compute our offset from lTempMeas and lTempBeat
				// This is valid because mtBeatClocks * ts.bBeatsPerMeasure is
				// always equal mtMeasureClocks. (No rounding occurs)
				lTempBeatOffset = pSequenceItem->m_mtDuration % mtBeatClocks;

				// Compute how many clocks in a quantize unit
				MUSIC_TIME mtQuantizeClocks;
				mtQuantizeClocks = mtBeatClocks / pSequenceQuantize->m_bResolution;

				// Compute how many quantize units the offset is away from lTempMeas and lTempBeat
				lTempBeatOffset = (lTempBeatOffset + mtQuantizeClocks/2) / mtQuantizeClocks;

				// lTempBeatOffset will now contain the new, 100% quantized offset
				if( lTempBeatOffset == pSequenceQuantize->m_bResolution )
				{
					// If we're at the next beat, use mtBeatClocks. (Otherwise rounding
					// errors may occur).
					lTempBeatOffset = mtBeatClocks;
				}
				else
				{
					lTempBeatOffset *= mtQuantizeClocks;
				}

				// lDeltaOffset is the amount for 100% quantization
				long lDeltaOffset = lTempBeatOffset - (pSequenceItem->m_mtDuration % mtBeatClocks);

				// Now, scale it based on bStrength
				lDeltaOffset = (lDeltaOffset * pSequenceQuantize->m_bStrength) / 100;

				if( lDeltaOffset )
				{
					fChanged = TRUE;

					// Set the note's m_mtDuration
					pSequenceItem->m_mtDuration += lDeltaOffset;
					pSequenceItem->m_mtDuration = max( pSequenceItem->m_mtDuration, (mtQuantizeClocks - 1) );
				}
			}
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::Velocitize

BOOL CSequenceMgr::Velocitize( SequenceVelocitize* pSequenceVelocitize )
{
	BOOL fChanged = FALSE;

	// Flag if we should modify all notes
	const bool fUseAllNotes = (pSequenceVelocitize->m_wVelocityTarget != VELOCITY_TARGET_SELECTED);

	// The time of the first and last selected notes
	MUSIC_TIME mtFirstSelected = LONG_MAX;
	MUSIC_TIME mtLastSelected = LONG_MIN;
	MUSIC_TIME mtSelectedSpan = 0;
	const long lAbsChangeSpan = pSequenceVelocitize->m_lAbsoluteChangeEnd - pSequenceVelocitize->m_lAbsoluteChangeStart;

	// If doing an absolute change and the start and end values are different
	if( (pSequenceVelocitize->m_dwVelocityMethod != SEQUENCE_VELOCITIZE_COMPRESS)
	&&	(lAbsChangeSpan != 0) )
	{
		// Find the time of the first and last selected notes
		POSITION pos = m_lstSequences.GetHeadPosition();
		while( pos )
		{
			CSequenceItem* pSequenceItem = m_lstSequences.GetNext( pos );

			// If not looking at all notes, only look at notes that are selected
			if( fUseAllNotes || pSequenceItem->m_fSelected )
			{
				// Check if this note is earlier than all others, or later than all others
				const MUSIC_TIME mtStart = pSequenceItem->AbsTime();
				if( mtStart < mtFirstSelected )
				{
					mtFirstSelected = mtStart;
				}
				if( mtStart > mtLastSelected )
				{
					mtLastSelected = mtStart;
				}
			}
		}

		mtSelectedSpan = mtLastSelected - mtFirstSelected;
	}

	// Iterate through all events
	// Only velocitize notes
	POSITION pos = m_lstSequences.GetHeadPosition();
	while( pos )
	{
		CSequenceItem* pSequenceItem = m_lstSequences.GetNext( pos );

		if( fUseAllNotes || pSequenceItem->m_fSelected )
		{
			// Compute the absolute velocity change for this note, if necessary
			long lAbsChange = pSequenceVelocitize->m_lAbsoluteChangeStart;
			if( (pSequenceVelocitize->m_dwVelocityMethod != SEQUENCE_VELOCITIZE_COMPRESS)
			&&	(lAbsChangeSpan != 0)
			&&	(mtSelectedSpan != 0) )
			{
				lAbsChange += (lAbsChangeSpan * (pSequenceItem->AbsTime() - mtFirstSelected)) / mtSelectedSpan;
			}

			if( VelocitizeNote( pSequenceItem, pSequenceVelocitize->m_bCompressMin, pSequenceVelocitize->m_bCompressMax, lAbsChange, pSequenceVelocitize->m_dwVelocityMethod ) )
			{
				fChanged = TRUE;
			}
		}
	}

	if( fChanged )
	{
		m_fDirty = TRUE;

		m_pSequenceStrip->m_nLastEdit = IDS_UNDO_VELOCITY;
		OnDataChanged();
		m_pSequenceStrip->InvalidateStrip();

		// Always recompute m_SelectedPropNote and update the property page
		RefreshPropertyPage();
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::VelocitizeNote

BOOL CSequenceMgr::VelocitizeNote( CSequenceItem* pSequenceItem, BYTE bCompressMin, BYTE bCompressMax, LONG lAbsoluteChange, DWORD dwVelocityMethod )
{
	BOOL fChanged = FALSE;

	ASSERT( pSequenceItem );

	long lNewValue = pSequenceItem->m_bByte2;
	switch( dwVelocityMethod & SEQUENCE_VELOCITIZE_METHOD_MASK )
	{
	case SEQUENCE_VELOCITIZE_PERCENT:
		if( lAbsoluteChange != 0 )
		{
			lNewValue = long(pSequenceItem->m_bByte2) + (long(pSequenceItem->m_bByte2) * lAbsoluteChange) / 100;
			lNewValue = min( 127, max( lNewValue, 1 ) );
			if( BYTE(lNewValue) != pSequenceItem->m_bByte2 )
			{
				fChanged = TRUE;
				pSequenceItem->m_bByte2 = BYTE(lNewValue);
			}
		}
		break;
	case SEQUENCE_VELOCITIZE_LINEAR:
		if( lAbsoluteChange != 0 )
		{
			lNewValue = long(pSequenceItem->m_bByte2) + lAbsoluteChange;
		}
		break;
	case SEQUENCE_VELOCITIZE_COMPRESS:
		lNewValue = min( bCompressMax, max( bCompressMin, pSequenceItem->m_bByte2 ) );
		break;
	}

	lNewValue = min( 127, max( lNewValue, 1 ) );
	if( BYTE(lNewValue) != pSequenceItem->m_bByte2 )
	{
		fChanged = TRUE;
		pSequenceItem->m_bByte2 = BYTE(lNewValue);
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr::CalculatePasteTime

MUSIC_TIME CSequenceMgr::CalculatePasteTime( MUSIC_TIME mtTime )
{
	// get offset of first sequence's drag position to normalize all sequences to offset zero:
	long lTimeOffset = mtTime;
	long lTimeOffset0=-1;
	long lTimeOffset1;

	// quantize the time to the nearest grid.
	lTimeOffset1 = m_pSequenceStrip->FloorTimeToGrid( lTimeOffset, NULL );

	// get clocks per beat
	DMUS_TIMESIGNATURE TimeSig;
	if(SUCCEEDED(m_pTimeline->GetParam(GUID_TimeSignature,
										m_dwGroupBits,
										m_dwIndex,
										lTimeOffset, NULL,
										&TimeSig)))
	{
		// see if next grid is closer to drop pos
		// (since FloorTimeToGrid returns floor of
		// time quantized to the grid)
		long lClocksPerGrid = ((DMUS_PPQ * 4) / TimeSig.bBeat) / TimeSig.wGridsPerBeat;
		lTimeOffset0 = lTimeOffset1 + lClocksPerGrid;

		// see which is closer
		lTimeOffset = (mtTime - lTimeOffset1) <= (lTimeOffset0 - mtTime) ? lTimeOffset1 : lTimeOffset0;
	}

	if(lTimeOffset0 == -1)
	{
		// pos calcs failed, fallback to first value
		lTimeOffset = lTimeOffset1;
	}

	return lTimeOffset;
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
// CSequenceMgr::UpdateStatusBarDisplay

void CSequenceMgr::UpdateStatusBarDisplay( void )
{
	bool fShowStatusBar = false;
	if( m_pSequenceStrip )
	{
		switch( m_pSequenceStrip->m_iHaveFocus)
		{
		case 1:
			if( m_pSequenceStrip->m_pActiveNote
			&&	m_pSequenceStrip->m_pActiveNote->m_fSelected )
			{
				fShowStatusBar = true;

				if( !m_hStatusBar
				&&	SUCCEEDED( m_pDMProdFramework->SetNbrStatusBarPanes( 1, SBLS_CONTROL, &m_hStatusBar ) ) )
				{
					m_pDMProdFramework->SetStatusBarPaneInfo( m_hStatusBar, 0, SBS_SUNKEN, 14 );
				}

				if( m_hStatusBar )
				{
					REFERENCE_TIME rtNote;
					if( SUCCEEDED( m_pTimeline->ClocksToRefTime( m_pSequenceStrip->m_pActiveNote->AbsTime(),
																 &rtNote ) ) )
					{
						CString strText;
						RefTimeToString( rtNote, IDS_NOTE_STATUS_TEXT, IDS_NOTE_NEG_STATUS_TEXT, strText );
						m_pDMProdFramework->SetStatusBarPaneText( m_hStatusBar, 0, strText.AllocSysString(), TRUE );
					}
				}
			}
			break;

		case 2:
			if( m_pSequenceStrip->m_pActiveCurveStrip )
			{
				CCurveItem *pCurve = m_pSequenceStrip->m_pActiveCurveStrip->GetEarliestSelectedCurve();
				if( pCurve )
				{
					fShowStatusBar = true;

					if( !m_hStatusBar
					&&	SUCCEEDED( m_pDMProdFramework->SetNbrStatusBarPanes( 1, SBLS_CONTROL, &m_hStatusBar ) ) )
					{
						m_pDMProdFramework->SetStatusBarPaneInfo( m_hStatusBar, 0, SBS_SUNKEN, 15 );
					}

					if( m_hStatusBar )
					{
						REFERENCE_TIME rtNote;
						if( SUCCEEDED( m_pTimeline->ClocksToRefTime( pCurve->AbsTime(),
																	 &rtNote ) ) )
						{
							CString strText;
							RefTimeToString( rtNote, IDS_CURVE_STATUS_TEXT, IDS_CURVE_NEG_STATUS_TEXT, strText );
							m_pDMProdFramework->SetStatusBarPaneText( m_hStatusBar, 0, strText.AllocSysString(), TRUE );
						}
					}
				}
			}
			break;
		}
	}


	if( !fShowStatusBar
	&&	m_hStatusBar )
	{
		m_pDMProdFramework->RestoreStatusBar( m_hStatusBar );
		m_hStatusBar = NULL;
	}
}
