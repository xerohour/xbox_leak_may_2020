// MelGenMgr.cpp : implementation file
//

/*--------------
@doc MELGENSAMPLE
--------------*/

#include "stdafx.h"
#include "MelGenIO.h"
#include "MelGenStripMgr.h"
#include "MelGenMgr.h"
#include "DLLJazzDataObject.h"
#include "PropPageMgr.h"
#include <dmusici.h>
#include <dmusicf.h>
#include <mmreg.h>
#include <RiffStrm.h>
#include <initguid.h>
#include <SegmentGuids.h>
#include "SegmentIO.h"
#include <SegmentDesigner.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_MEASURE 0xFFFFFFFF

////////////////////////////////////////////////////////////////////////////
// CMelGenMgr constructor/destructor 

CMelGenMgr::CMelGenMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pPropertyPage = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_pMelGenStrip = NULL;
	m_pIDMTrack = NULL;
	m_pISegmentNode = NULL;

	m_DefaultMelGen.mtTime = 0;
	m_DefaultMelGen.dwID = 0;
	wcscpy(m_DefaultMelGen.wszVariationLabel, L"<unused>");
	m_DefaultMelGen.dwVariationFlags = 0xFFFFFFFF;
	m_DefaultMelGen.dwRepeatFragmentID = 0;
	m_DefaultMelGen.dwFragmentFlags = DMUS_FRAGMENTF_USE_PLAYMODE;
	m_DefaultMelGen.dwPlayModeFlags = DMUS_PLAYMODE_NONE;
	// new...
	m_DefaultMelGen.dwTransposeIntervals = 0;
	ZeroMemory(&m_DefaultMelGen.Command, sizeof(m_DefaultMelGen.Command));
	ZeroMemory(&m_DefaultMelGen.ConnectionArc, sizeof(m_DefaultMelGen.ConnectionArc));
	m_dwPlaymode = DMUS_PLAYMODE_NONE;
	m_dwID = 0;

	// Initially we don't need to be saved
	m_fDirty = FALSE;

	// By default, belong to Group 1
	m_dwGroupBits = 1;

	// Initialize the track flags
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK | DMUS_TRACKCONFIG_COMPOSING;
	m_dwProducerOnlyFlags = 0;

	// Create a strip
	m_pMelGenStrip = new CMelGenStrip(this);
	ASSERT( m_pMelGenStrip );
}

CMelGenMgr::~CMelGenMgr()
{
	// Delete all the MelGens in m_lstMelGens
	EmptyMelGenList();

	// Clean up our references
	if( m_pMelGenStrip )
	{
		m_pMelGenStrip->Release();
		m_pMelGenStrip = NULL;
	}

	if( m_pDMProdFramework )
	{
		m_pDMProdFramework->Release();
		m_pDMProdFramework = NULL;
	}

	ASSERT( m_pTimeline == NULL );
	ASSERT( m_pPropertyPage == NULL );

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
	}
	if( m_pIDMTrack )
	{
		m_pIDMTrack->Release();
	}
	// If we have an object on the clipboard, make sure it doesn't have any references
	// back to us.
	if( m_pCopyDataObject )
	{
		if( S_OK == OleIsCurrentClipboard( m_pCopyDataObject ))
		{
			OleFlushClipboard();
		}
		m_pCopyDataObject->Release();
		m_pCopyDataObject = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr IMelGenMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::IsMeasureBeatOpen

HRESULT STDMETHODCALLTYPE CMelGenMgr::IsMeasureBeatOpen( DWORD dwMeasure, BYTE bBeat )
{
	// Get length of segment
	MUSIC_TIME mtMaxTimelineLength;
	VARIANT var;
	m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	mtMaxTimelineLength = V_I4( &var );
	mtMaxTimelineLength--;

	long lMeasure = dwMeasure;
	long lBeat = (char)bBeat;

	MUSIC_TIME mtTime;
	HRESULT hr = m_pTimeline->MeasureBeatToClocks(  m_dwGroupBits, 0, lMeasure, lBeat, &mtTime );
	ASSERT(SUCCEEDED(hr));

	// Make sure measure/beat fall within segment
	if( mtTime < 0
	||  mtTime > mtMaxTimelineLength )
	{
		return E_FAIL;
	}

	// Determine proposed measure/beat
	hr = m_pTimeline->ClocksToMeasureBeat(  m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
	ASSERT(SUCCEEDED(hr));

	hr = S_OK;
	
	// Does an item fall on this measure/beat?
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );

		if( !(pMelGenItem->m_dwBits & UD_FAKE) )
		{
			if( pMelGenItem->m_dwMeasure > (DWORD)lMeasure )
			{
				pos = NULL;
			}
			else if( pMelGenItem->m_dwMeasure == (DWORD)lMeasure 
				 &&	 pMelGenItem->m_bBeat == (BYTE)lBeat )
			{
				hr = S_FALSE;
				pos = NULL;
			}
		}
	}
	
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::EnumMelGens

HRESULT STDMETHODCALLTYPE CMelGenMgr::EnumMelGens(DWORD dwIndex, void** ppMelGen)
{
	HRESULT hr = S_FALSE;
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
		if( !(pMelGenItem->m_dwBits & UD_FAKE) )
		{
			if( dwIndex == 0 )
			{
				*ppMelGen = pMelGenItem;
				pos = NULL;
				hr = S_OK;
			}
			dwIndex--;
		}
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr IStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::GetParam

HRESULT STDMETHODCALLTYPE CMelGenMgr::GetParam(
		/* [in] */	REFGUID 	guidType,
		/* [in] */	MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME* pmtNext,
		/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pData != NULL );
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		IDMUSProdNode** ppIDocRootNode = (IDMUSProdNode **)pData;

		*ppIDocRootNode = m_pISegmentNode;
		if( m_pISegmentNode )
		{
			m_pISegmentNode->AddRef();
		}
		return S_OK;
	}

	if( ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		BSTR bstr;
		CString str;
		str.LoadString(m_pMelGenStrip->m_nLastEdit);
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

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::SetParam

HRESULT STDMETHODCALLTYPE CMelGenMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		IDMUSProdNode* pIDocRootNode = (IDMUSProdNode *)pData;

		m_pISegmentNode = pIDocRootNode;
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::IsParamSupported

// support GUID_DocRootNode for maintaining pointer to DocRoot node
HRESULT STDMETHODCALLTYPE CMelGenMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	if( ::IsEqualGUID( guidType, GUID_MelodyFragment ) 
	||  ::IsEqualGUID( guidType, GUID_DocRootNode )
	||  ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CMelGenMgr::GetStripMgrProperty(
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

			pioTrackHeader->guidClassID = CLSID_DirectMusicMelodyFormulationTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = NULL;
			pioTrackHeader->fccType = DMUS_FOURCC_MELODYFORM_TRACK_LIST;
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

			// Always set DMUS_TRACKCONFIG_COMPOSING
			m_dwTrackExtrasFlags |= DMUS_TRACKCONFIG_COMPOSING;
			pioTrackExtrasHeader->dwFlags = m_dwTrackExtrasFlags;
			pioTrackExtrasHeader->dwPriority = COMPOSITION_PRIORITY_MELODYGEN_STRIP;
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
// CMelGenMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CMelGenMgr::SetStripMgrProperty(
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
			m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);
			if ( m_pMelGenStrip )
			{
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pMelGenStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pMelGenStrip );
			}
			m_pTimeline->RemoveFromNotifyList( this, GUID_NOTIFICATION_RECOMPOSE, 0xFFFFFFFF ); 
			m_pTimeline->Release();
			m_pTimeline = NULL;
		}

		if( V_UNKNOWN( &variant ) )
		{
			if( FAILED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pTimeline )))
			{
				return E_FAIL;
			}
			else
			{
				m_pTimeline->AddToNotifyList( this, GUID_NOTIFICATION_RECOMPOSE, 0xFFFFFFFF ); 
				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pMelGenStrip, CLSID_DirectMusicMelodyFormulationTrack, m_dwGroupBits, 0 );
				// Fix measure/beat info for all current melgens
				POSITION pos = m_lstMelGens.GetHeadPosition();
				while( pos )
				{
					CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
					ASSERT( pMelGenItem );
					if ( pMelGenItem )
					{
						m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, pMelGenItem->m_MelGen.mtTime, (long*)&pMelGenItem->m_dwMeasure, (long*)&pMelGenItem->m_bBeat );
						
					}
				}
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
		if( V_UNKNOWN( &variant ) )
		{
			return V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMProdFramework);
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

			// Always set DMUS_TRACKCONFIG_COMPOSING
			m_dwTrackExtrasFlags |= DMUS_TRACKCONFIG_COMPOSING;
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
// CMelGenMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CMelGenMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwGroupBits);
	UNREFERENCED_PARAMETER(pData);

	// Set the "Use to Compose" flag by default
	if( ::IsEqualGUID(rguidType, GUID_Segment_CreateTrack))
	{
		m_dwTrackExtrasFlags |= DMUS_TRACKCONFIG_COMPOSING;
		if( m_pTimeline )
		{
			m_pTimeline->OnDataChanged( static_cast<IMelGenMgr*>(this) );

			// Look for a pattern track in our track group
			IDMUSProdStripMgr *pIDMUSProdStripMgr;
			if( FAILED( m_pTimeline->GetStripMgr( CLSID_DirectMusicPatternTrack, m_dwGroupBits, 0, &pIDMUSProdStripMgr ) ) )
			{
				// Didn't find one, so create a Pattern track
				VARIANT var;
				if( SUCCEEDED( m_pTimeline->GetTimelineProperty( TP_TIMELINECALLBACK, &var ) ) )
				{
					IDMUSProdSegmentEdit8* pISegmentEdit;
					if( SUCCEEDED( V_UNKNOWN( &var )->QueryInterface( IID_IDMUSProdSegmentEdit8, (void**)&pISegmentEdit ) ) )
					{
						IUnknown *punkStripMgr;
						if( SUCCEEDED( pISegmentEdit->AddStrip( CLSID_DirectMusicPatternTrack, m_dwGroupBits, &punkStripMgr ) ) )
						{
							punkStripMgr->Release();
						}
						pISegmentEdit->Release();
					}
					V_UNKNOWN( &var )->Release();
				}
			}
			else
			{
				pIDMUSProdStripMgr->Release();
			}
		}
		return S_OK;
	}
	else if( ::IsEqualGUID( rguidType, GUID_NOTIFICATION_RECOMPOSE ) )
	{
		OnRecompose( pData );
		return S_OK;
	}
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::GetClassID

HRESULT CMelGenMgr::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pClsId == NULL )
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy( pClsId, &CLSID_MelGenMgr, sizeof( CLSID ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::IsDirty

HRESULT CMelGenMgr::IsDirty()
{
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
// CMelGenMgr::Load

// This is obslolete and should not be public (but I need it for my old project)
#define DMUS_FOURCC_MELODYGEN_TRACK_CHUNK     mmioFOURCC( 'm', 'g', 'e', 'n' )

HRESULT CMelGenMgr::Load( IStream* pIStream )
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

	// Remove all existing melody fragments
	EmptyMelGenList();

	// Load the Track
	MMCKINFO ck;
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_MELODYGEN_TRACK_CHUNK: // MelGen Chunk (OBSOLETE)
				LoadMelGenChunk(pIStream, ck.cksize, FALSE, 0);
				pIRiffStream->Ascend( &ck, 0 );
				hr = S_OK;
				break;
			case FOURCC_LIST:
				if (ck.fccType == DMUS_FOURCC_MELODYFORM_TRACK_LIST)
				{
					MMCKINFO ckHeader, ckBody;
					long lFileSize = ck.cksize - 4; // subtract off the list type
					DMUS_IO_MELFORM iMelform;
					DWORD cb;
					if (pIRiffStream->Descend(&ckHeader, &ck, 0) == 0)
					{
						if (ckHeader.ckid == DMUS_FOURCC_MELODYFORM_HEADER_CHUNK )
						{
							lFileSize -= 8;  // chunk id + chunk size: double words
							lFileSize -= ckHeader.cksize;
							hr = pIStream->Read( &iMelform, sizeof( iMelform ), &cb );
							if (FAILED(hr) || cb != sizeof( iMelform ) ) 
							{
								if (SUCCEEDED(hr)) hr = DMUS_E_CHUNKNOTFOUND;
							}
							else
							{
//								m_dwPlaymode = iMelform.dwPlaymode;
								m_dwPlaymode = DMUS_PLAYMODE_NONE;		// Only flag supported in DX8
							}
						}
						pIRiffStream->Ascend( &ckHeader, 0 );
					}
					if (SUCCEEDED(hr))
					{
						hr = DMUS_E_CHUNKNOTFOUND;
						if (pIRiffStream->Descend(&ckBody, &ck, 0) == 0)
						{
							if (ckBody.ckid == DMUS_FOURCC_MELODYFORM_BODY_CHUNK )
							{
								LoadMelGenChunk(pIStream, ckBody.cksize, FALSE, 0);
							}
							pIRiffStream->Ascend( &ckBody, 0 );
						}
					}
				}
				pIRiffStream->Ascend( &ck, 0 );
				hr = S_OK;
				break;
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

	pIRiffStream->Release();

	HookUpRepeats();
	SyncWithDirectMusic();

	return hr;
}


HRESULT CMelGenMgr::LoadMelGenChunk( IStream* pIStream,
										long lSize,
										BOOL fPaste,
										MUSIC_TIME mtPasteTime )
{
	// Validate pIStream
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	// Read in the size of the MelGen structure
	DWORD cb, dwMelGenSize;
	HRESULT hr = pIStream->Read( &dwMelGenSize, sizeof( DWORD ), &cb );
	if( FAILED(hr) || cb != sizeof( DWORD ) )
	{
		if (SUCCEEDED(hr)) hr = E_FAIL;
		goto ON_END;
	}

	// Subtract off the four bytes we just read
	lSize -= sizeof(DWORD);

	// Compute how many bytes (if any) we'll need to skip
	ASSERT( dwMelGenSize >= sizeof(DMUS_IO_MELODY_FRAGMENT) );
	DWORD dwSkipBytes;
	dwSkipBytes = 0;
	if( dwMelGenSize > sizeof(DMUS_IO_MELODY_FRAGMENT) )
	{
		dwSkipBytes = dwMelGenSize - sizeof(DMUS_IO_MELODY_FRAGMENT);
	}

	// Ensure that the chunk size is an even number of MelGen items
	ASSERT( (lSize % dwMelGenSize) == 0 );

	if( fPaste )
	{
		DWORD dwCurrentFilePos = StreamTell( pIStream );
		long lTmpSize = lSize;

		MUSIC_TIME mtAdjustment = 0;;

		ASSERT( m_pTimeline != NULL );
		if( m_pTimeline )
		{
			// Get Timeline length
			VARIANT var;
			m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
			MUSIC_TIME mtMaxTimelineLength = V_I4( &var );

			// Determine new paste time to enforce boundaries
			while ( lTmpSize >= (signed)dwMelGenSize )
			{
				// Read in a MelGen item
				DMUS_IO_MELODY_FRAGMENT iMelGenItem;
				DWORD cb;
				hr = pIStream->Read( &iMelGenItem, sizeof( DMUS_IO_MELODY_FRAGMENT ), &cb );
				if (FAILED(hr) || cb != sizeof( DMUS_IO_MELODY_FRAGMENT ) ) 
				{
					if (SUCCEEDED(hr)) hr = E_FAIL;
					goto ON_END;
				}

				// If we need to, skip some bytes
				if( dwSkipBytes )
				{
					hr = StreamSeek( pIStream, dwSkipBytes, STREAM_SEEK_CUR );
					if( FAILED(hr) )
					{
						goto ON_END;
					}
				}

				// Subtract off the amount we read in
				lTmpSize -= dwMelGenSize;

				// Now, figure out if we need to adjust the start time of the paste
				MUSIC_TIME mtTime = iMelGenItem.mtTime;
				mtTime += mtPasteTime;
				if( mtTime < 0 )
				{
					mtTime = 0 - mtTime;
					if( mtTime > mtAdjustment )
					{
						mtAdjustment = mtTime;
					}
				}
				else if( mtTime >= mtMaxTimelineLength )
				{
					mtTime = mtMaxTimelineLength - mtTime;
					if( mtTime < mtAdjustment )
					{
						mtAdjustment = mtTime - 1;
					}
				}
			}
		}

		// New paste time which will enforce strip boundaries
		mtPasteTime += mtAdjustment;

		// Restore our position back to the start of the MelGens
		StreamSeek( pIStream, dwCurrentFilePos, STREAM_SEEK_SET );
	}

	while ( lSize )
	{
		DMUS_IO_MELODY_FRAGMENT iMelGenItem;
		DWORD cb;
		hr = pIStream->Read( &iMelGenItem, sizeof( DMUS_IO_MELODY_FRAGMENT ), &cb );
		if (FAILED(hr) || cb != sizeof( DMUS_IO_MELODY_FRAGMENT ) ) 
		{
			if (SUCCEEDED(hr)) hr = E_FAIL;
			goto ON_END;
		}

		// If we need to, skip some bytes
		if( dwSkipBytes )
		{
			hr = StreamSeek( pIStream, dwSkipBytes, STREAM_SEEK_CUR );
			if( FAILED(hr) )
			{
				goto ON_END;
			}
		}

		// Subtract off the amount we read in
		lSize -= dwMelGenSize;

		CMelGenItem* pItem = new CMelGenItem( this );
		if( pItem == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto ON_END;
		}
		pItem->m_dwMeasure = 0;
		pItem->m_bBeat = 0;

		MUSIC_TIME mtTime = iMelGenItem.mtTime;
		if( fPaste )
		{
			mtTime += mtPasteTime;
		}

		// Set the measure and beat this MelGen belongs to
		if (m_pTimeline)
		{
			m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, (long*)&pItem->m_dwMeasure, (long*)&pItem->m_bBeat );
		}
		//ClocksToMeasureAndBeat( mtTime, (DWORD&)pItem->m_dwMeasure, (DWORD&)pItem->m_bBeat );
		ASSERT( pItem->m_dwMeasure >= 0 );
		//ASSERT( pItem->m_bBeat == 0 );		// MelGens must be on a measure boundary

		// Copy MelGen to the pItem
		pItem->m_MelGen.mtTime = mtTime;
		pItem->m_MelGen.dwID = iMelGenItem.dwID;
		wcscpy(pItem->m_MelGen.wszVariationLabel, iMelGenItem.wszVariationLabel);
		pItem->m_MelGen.dwVariationFlags = iMelGenItem.dwVariationFlags;
		pItem->m_MelGen.dwRepeatFragmentID = iMelGenItem.dwRepeatFragmentID;
		pItem->m_MelGen.dwFragmentFlags = iMelGenItem.dwFragmentFlags;
//		pItem->m_MelGen.dwPlayModeFlags = iMelGenItem.dwPlayModeFlags;
		pItem->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_NONE;		// Only flag supported in DX8
		pItem->m_MelGen.dwTransposeIntervals = iMelGenItem.dwTransposeIntervals;
		pItem->m_MelGen.Command = iMelGenItem.Command;
		pItem->m_MelGen.ConnectionArc = iMelGenItem.ConnectionArc;

		// Insert it into the list
		InsertByAscendingTime( pItem );

		// If pasting, select the item
		// and mark it as having been pasted
		if( fPaste )
		{
			pItem->SetSelectFlag( TRUE );
			pItem->m_dwBits |= UD_FROMPASTE;
		}
	}

ON_END:
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::Save

HRESULT CMelGenMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_DESIGN;	// Default to FT_DESIGN so clipboard
										// gets proper file ref chunk
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
	if( !(::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ))
	&&  !(::IsEqualGUID( guidDataFormat, GUID_CurrentVersion )) )
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

    MMCKINFO ckMain, ckHeader, ckBody;
	DWORD dwBytesWritten = 0;

	// Always save the MelGen list, even if empty
	//if ( !m_lstMelGens.IsEmpty() )
	{
		// Create a chunk to store the MelGen data
		//ckMain.ckid = DMUS_FOURCC_MELODYGEN_TRACK_CHUNK;
		ckMain.fccType = DMUS_FOURCC_MELODYFORM_TRACK_LIST;
		if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write MelForm chunk header
		ckHeader.ckid = DMUS_FOURCC_MELODYFORM_HEADER_CHUNK;
		if( pIRiffStream->CreateChunk( &ckHeader, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare DMUS_IO_MELFORM
		DMUS_IO_MELFORM oMelForm;
		memset( &oMelForm, 0, sizeof(DMUS_IO_MELFORM) );

		oMelForm.dwPlaymode = m_dwPlaymode;

		// Write MelForm chunk data
		hr = pIStream->Write( &oMelForm, sizeof(DMUS_IO_MELFORM), &dwBytesWritten);
		if( FAILED( hr ) ||  dwBytesWritten != sizeof(DMUS_IO_MELFORM) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		
		if( pIRiffStream->Ascend( &ckHeader, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write MelForm chunk body
		ckBody.ckid = DMUS_FOURCC_MELODYFORM_BODY_CHUNK;
		if( pIRiffStream->CreateChunk( &ckBody, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		DWORD cb, dwMelGenSize = sizeof( DMUS_IO_MELODY_FRAGMENT );
		hr = pIStream->Write( &dwMelGenSize, sizeof( DWORD ), &cb );
		if( FAILED(hr) || cb != sizeof(DWORD) )
		{
			if(SUCCEEDED(hr)) hr = E_FAIL;
			goto ON_ERROR;
		}

		POSITION pos = m_lstMelGens.GetHeadPosition();
		while( pos )
		{
			CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
			if( !(pMelGenItem->m_dwBits & UD_FAKE) )
			{
				DMUS_IO_MELODY_FRAGMENT oMelGenItem;

				oMelGenItem.mtTime = pMelGenItem->m_MelGen.mtTime;
				oMelGenItem.dwID = pMelGenItem->m_MelGen.dwID;
				wcscpy(oMelGenItem.wszVariationLabel, pMelGenItem->m_MelGen.wszVariationLabel);
				oMelGenItem.dwVariationFlags = pMelGenItem->m_MelGen.dwVariationFlags;
				oMelGenItem.dwRepeatFragmentID = 0;
				if (pMelGenItem->m_pRepeat && 
					(pMelGenItem->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT))
				{
					oMelGenItem.dwRepeatFragmentID = pMelGenItem->m_pRepeat->dwID;
				}
				oMelGenItem.dwFragmentFlags = pMelGenItem->m_MelGen.dwFragmentFlags;
				oMelGenItem.dwPlayModeFlags = pMelGenItem->m_MelGen.dwPlayModeFlags;
				oMelGenItem.dwTransposeIntervals = pMelGenItem->m_MelGen.dwTransposeIntervals;
				oMelGenItem.Command = pMelGenItem->m_MelGen.Command;
				oMelGenItem.ConnectionArc = pMelGenItem->m_MelGen.ConnectionArc;

				hr = pIStream->Write( &oMelGenItem, sizeof( DMUS_IO_MELODY_FRAGMENT ), &cb );
				if( FAILED(hr) || cb != sizeof(DMUS_IO_MELODY_FRAGMENT) )
				{
					if(SUCCEEDED(hr)) hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}

		// Ascend out of the MelForm Body chunk
		if( pIRiffStream->Ascend( &ckBody, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the MelGen chunk.
		pIRiffStream->Ascend( &ckMain, 0 );
	}

ON_ERROR:
	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::SyncWithDirectMusic

HRESULT CMelGenMgr::SyncWithDirectMusic( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIDMTrack == NULL )
	{
		return E_FAIL;
	}

	IStream* pIMemStream;

	// Persist the strip into a stream
	HRESULT hr = m_pDMProdFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );
	if( SUCCEEDED ( hr ) )
	{
		hr = Save( pIMemStream, FALSE );
		if( SUCCEEDED ( hr ) )
		{
			IPersistStream* pIPersistStream;
			hr = m_pIDMTrack->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
			if( SUCCEEDED ( hr ) )
			{
				// Load into DirectMusic track
				StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
				hr = pIPersistStream->Load( pIMemStream );

				pIPersistStream->Release();
			}
		}

		pIMemStream->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::GetSizeMax

HRESULT CMelGenMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::GetData

// This method is called by CMelGenPropPageMgr to get data to send to the
// MelGen property page.
// The CMelGenStrip::GetData() method is called by CMelGenStripPropPageMgr
// to get the strip's properties (Group Bits, etc.)
HRESULT STDMETHODCALLTYPE CMelGenMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected MelGen
	BOOL fMultipleSelect = FALSE;
	CMelGenItem* pFirstMelGenItem = NULL;

	CMelGenItem* pMelGenItem;
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		pMelGenItem = m_lstMelGens.GetNext( pos );
		if( pMelGenItem->m_fSelected
		&&  !(pMelGenItem->m_dwBits & UD_FAKE) )
		{
			pFirstMelGenItem = pMelGenItem;
			while( pos )
			{
				pMelGenItem = m_lstMelGens.GetNext( pos );
				if( pMelGenItem->m_fSelected
				&&  !(pMelGenItem->m_dwBits & UD_FAKE) )
				{
					fMultipleSelect = TRUE;
					pos = NULL;
					break;
				}
			}
			break;
		}
	}

	*ppData = NULL;
	HRESULT hr = E_FAIL;

	// Multiple MelGens selected
	if( fMultipleSelect )
	{
		CPropMelGen* pPropMelGen = new CPropMelGen;
		if( pPropMelGen )
		{
			pPropMelGen->m_dwMeasure = 0xFFFFFFFF;		// Signifies multiple MelGens selected
			*ppData = pPropMelGen;
			hr = S_OK;
		}
	}

	// One MelGen selected
	else if( pFirstMelGenItem )
	{
		CPropMelGen* pPropMelGen = new CPropMelGen( pFirstMelGenItem );
		if( pPropMelGen )
		{
			*ppData = pPropMelGen;
			hr = S_OK;
		}
	}

	// Nothing selected
	else
	{
		*ppData = NULL;
		hr = S_OK;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::SetData

// This method is called by CMelGenPropPageMgr in response to user actions
// in the MelGen Property page.  It changes the currenly selected MelGen. 
HRESULT STDMETHODCALLTYPE CMelGenMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// Return a pointer to the currently selected melody fragment
	CMelGenItem* pMelGenItem = FirstSelectedMelGen();

	if( pMelGenItem )
	{
		CPropMelGen* pMelGen = (CPropMelGen*)pData;
		BOOL fChanged = FALSE;

		// Get length of segment
		MUSIC_TIME mtMaxTimelineLength;
		VARIANT var;
		m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		mtMaxTimelineLength = V_I4( &var );
		mtMaxTimelineLength--;

		long lNewMeasure = pMelGen->m_dwMeasure;
		long lNewBeat = (char)pMelGen->m_bBeat;

		MUSIC_TIME mtNewTime;
		HRESULT hr = m_pTimeline->MeasureBeatToClocks(  m_dwGroupBits, 0, lNewMeasure, lNewBeat, &mtNewTime );
		ASSERT(SUCCEEDED(hr));

		// Enforce boundaries
		mtNewTime = max( 0, mtNewTime );
		mtNewTime = min( mtNewTime, mtMaxTimelineLength );

		// Determine proposed measure/beat
		hr = m_pTimeline->ClocksToMeasureBeat(  m_dwGroupBits, 0, mtNewTime, &lNewMeasure, &lNewBeat );
		ASSERT(SUCCEEDED(hr));

		// Check if measure or beat # changed
		if( pMelGenItem->m_dwMeasure != (DWORD)lNewMeasure
		||	pMelGenItem->m_bBeat != (BYTE)lNewBeat)
		{
			pMelGenItem->m_dwMeasure = lNewMeasure;
			pMelGenItem->m_bBeat = (BYTE)lNewBeat;
			pMelGenItem->m_MelGen.mtTime = mtNewTime;
			pMelGen->m_MelGen.mtTime = mtNewTime;
			fChanged = TRUE;
			m_pMelGenStrip->m_nLastEdit = IDS_UNDO_MOVE;

			// Re-insert the melody fragment into the list, since its measure info 
			// changed and it may now be out of order w.r.t. the other elements.
			if( RemoveItem( pMelGenItem ) )
			{
				InsertByAscendingTime( pMelGenItem );
				HookUpRepeats();
			}
		}

		// Check if Repeats changed
		if (pMelGenItem->m_pRepeat != pMelGen->m_pRepeat)
		{
			pMelGenItem->m_pRepeat = pMelGen->m_pRepeat;
			if( pMelGen->m_pRepeat )
			{
				pMelGen->m_MelGen.dwRepeatFragmentID = pMelGen->m_pRepeat->dwID;
			}
			else
			{
				pMelGen->m_MelGen.dwRepeatFragmentID = 0;
			}
		}

		// Check if MelGen changed
		if( (pMelGenItem->m_MelGen.mtTime != pMelGen->m_MelGen.mtTime) ||
			(wcscmp(pMelGenItem->m_MelGen.wszVariationLabel, pMelGen->m_MelGen.wszVariationLabel)) ||
			(pMelGenItem->m_MelGen.dwVariationFlags != pMelGen->m_MelGen.dwVariationFlags) ||
			(pMelGenItem->m_MelGen.dwRepeatFragmentID != pMelGen->m_MelGen.dwRepeatFragmentID) ||
			(pMelGenItem->m_MelGen.dwFragmentFlags != pMelGen->m_MelGen.dwFragmentFlags) ||
			(pMelGenItem->m_MelGen.dwPlayModeFlags != pMelGen->m_MelGen.dwPlayModeFlags) ||
			(pMelGenItem->m_MelGen.dwTransposeIntervals != pMelGen->m_MelGen.dwTransposeIntervals) || 
			(pMelGenItem->m_MelGen.ConnectionArc.dwFlags != pMelGen->m_MelGen.ConnectionArc.dwFlags) ||
			(pMelGenItem->m_MelGen.ConnectionArc.dwIntervals != pMelGen->m_MelGen.ConnectionArc.dwIntervals) ||
			(pMelGenItem->m_MelGen.Command.bCommand != pMelGen->m_MelGen.Command.bCommand) ||
			(pMelGenItem->m_MelGen.Command.bGrooveLevel != pMelGen->m_MelGen.Command.bGrooveLevel) ||
			(pMelGenItem->m_MelGen.Command.bGrooveRange != pMelGen->m_MelGen.Command.bGrooveRange) )
		{
			pMelGenItem->m_MelGen = pMelGen->m_MelGen;
			if (pMelGenItem->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT)
			{
				HookUpRepeat(pMelGenItem);
			}
			fChanged = TRUE;
			m_pMelGenStrip->m_nLastEdit = IDS_CHANGE;
		}

		if( fChanged )
		{
			// Since it changed, make it into a 'real' melody fragment
			if( pMelGenItem->m_dwBits & UD_FAKE )
			{
				pMelGenItem->m_dwBits &= ~UD_FAKE;
				pMelGenItem->m_MelGen.dwID = NewFragmentID();
			}

			// Redraw the melgen strip
			// BUGBUG: Should be smarter and only redraw the melody fragment that changed
			m_pTimeline->StripInvalidateRect( m_pMelGenStrip, NULL, TRUE );

			// Let our hosting editor know about the changes
			OnDataChanged();

			// Refresh the property page with new values
			ASSERT( m_pPropPageMgr != NULL );	// OnDataChanged() should not set m_pPropPageMgr to NULL!
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->RefreshData();
			}

			// Notify the other strips of possible MelGen change
			m_pTimeline->NotifyStripMgrs( GUID_MelodyFragment, m_dwGroupBits, NULL );

			SyncWithDirectMusic();
			return S_OK;
		}
	}

	// Nothing changed, or no items are selected
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CMelGenMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		CMelGenPropPageMgr* pPPM = new CMelGenPropPageMgr(m_pDMProdFramework);
		if( pPPM == NULL )
		{
			return E_OUTOFMEMORY;
		}

		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		
		m_pPropPageMgr->SetObject( this );

		// Remove the reference created by the contrustor, leaving the one created by QueryInterface.
		// If QueryInterface failed, this will delete m_pPropPageMgr.
		m_pPropPageMgr->Release();

		if( FAILED(hr) )
		{
			return hr;
		}
	}

	// Set the displayed property page to our property page
	m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CMelGenMgr::OnRemoveFromPageManager( void)
{
	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::OnDataChanged

HRESULT STDMETHODCALLTYPE CMelGenMgr::OnDataChanged( void)
{
	ASSERT( m_pTimeline );
	if ( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	// Let our hosting editor know about the change
	m_pTimeline->OnDataChanged( (IMelGenMgr*)this );

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::DeleteSelectedMelGens

HRESULT CMelGenMgr::DeleteSelectedMelGens()
{
	CMelGenItem* pMelGenItem;
	POSITION pos2, pos1 = m_lstMelGens.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		pMelGenItem = m_lstMelGens.GetNext( pos1 );
		if( pMelGenItem->m_fSelected
		|| (pMelGenItem->m_dwBits & UD_FAKE) )
		{
			RemoveMelGen( pos2 );
			delete pMelGenItem;
		}
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::SaveSelectedMelGens

HRESULT CMelGenMgr::SaveSelectedMelGens(LPSTREAM pIStream, CMelGenItem* pMelGenAtDragPoint, BOOL fNormalize)
{
	// if fNormalize is TRUE and pMelGenAtDragPoint is valid, set mtOffset and dwMeasureOffset so that the time pMelGenAtDragPoint is 0.
	// if fNormalize is TRUE and pMelGenAtDragPoint is NULL, set mtOffset and dwMeasureOffset so that the time of the first MelGen is 0.
	// if fNormalize is FALSE, don't change the times at all.
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// If the MelGen list has anything in it, look for selected MelGens
	if ( !m_lstMelGens.IsEmpty() )
	{
		MUSIC_TIME mtOffset = -1;
		
		if( fNormalize )
		{
			// If the MelGen positions should be normalized based on pMelGenAtDragPoint, set
			// mtOffset to the time of pMelGenAtDragPoint.
			ASSERT( pMelGenAtDragPoint );
			if( pMelGenAtDragPoint && m_pTimeline)
			{
				hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits,
													   0,
													   pMelGenAtDragPoint->m_dwMeasure,
													   pMelGenAtDragPoint->m_bBeat,
													   &mtOffset );
				//mtOffset = MeasureAndBeatToClocks( pMelGenAtDragPoint->m_dwMeasure, 0 );
			}
			else
			{
				// Fail if pMelGenAtDragPoint is NULL
				hr = E_POINTER;
				goto ON_ERROR;
			}
		}
		else
		{
			// No offset
			mtOffset = 0;
		}


		// Create a chunk to store the MelGen data
		/*
		MMCKINFO ckMain;
		ckMain.ckid = DMUS_FOURCC_MELGEN_TRACK;
		if( pIRiffStream->CreateChunk( &ckMain, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		*/

		DWORD cb, dwMelGenSize = sizeof( DMUS_IO_MELODY_FRAGMENT );
		hr = pIStream->Write( &dwMelGenSize, sizeof( DWORD ), &cb );
		if( FAILED(hr) || cb != sizeof(DWORD) )
		{
			if(SUCCEEDED(hr)) hr = E_FAIL;
			goto ON_ERROR;
		}

		POSITION pos = m_lstMelGens.GetHeadPosition();
		while( pos )
		{
			CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
			if(  pMelGenItem->m_fSelected
			&& !(pMelGenItem->m_dwBits & UD_FAKE) )
			{
				DMUS_IO_MELODY_FRAGMENT oMelGenItem;

				oMelGenItem.mtTime = pMelGenItem->m_MelGen.mtTime - mtOffset;
				oMelGenItem.dwID = pMelGenItem->m_MelGen.dwID;
				wcscpy(oMelGenItem.wszVariationLabel, pMelGenItem->m_MelGen.wszVariationLabel);
				oMelGenItem.dwVariationFlags = pMelGenItem->m_MelGen.dwVariationFlags;
				oMelGenItem.dwRepeatFragmentID = 0;
				if (pMelGenItem->m_pRepeat && 
					(pMelGenItem->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT))
				{
					oMelGenItem.dwRepeatFragmentID = pMelGenItem->m_pRepeat->dwID;
				}
				oMelGenItem.dwFragmentFlags = pMelGenItem->m_MelGen.dwFragmentFlags;
				oMelGenItem.dwPlayModeFlags = pMelGenItem->m_MelGen.dwPlayModeFlags;
				oMelGenItem.dwTransposeIntervals = pMelGenItem->m_MelGen.dwTransposeIntervals;
				oMelGenItem.Command = pMelGenItem->m_MelGen.Command;
				oMelGenItem.ConnectionArc = pMelGenItem->m_MelGen.ConnectionArc;

				hr = pIStream->Write( &oMelGenItem, sizeof( DMUS_IO_MELODY_FRAGMENT ), &cb );
				if( FAILED(hr) || cb != sizeof(DMUS_IO_MELODY_FRAGMENT) )
				{
					if(SUCCEEDED(hr)) hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}
		/*
		// Ascend out of the MelGen chunk.
		pIRiffStream->Ascend( &ckMain, 0 );
		*/
	}
	else
	{
		hr = S_FALSE; // Nothing in the list
	}

ON_ERROR:
	pIRiffStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::CreateMelGen

HRESULT CMelGenMgr::CreateMelGen( long lXPos, CMelGenItem*& rpMelGen )
{
	rpMelGen = NULL;

	if( m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	rpMelGen = new CMelGenItem( this );
	if( rpMelGen == NULL )
	{
		return E_OUTOFMEMORY;
	}

	long lMeasure = 0;
	long lBeat = 0;
	long lClocks = 0;

	HRESULT hr;
	hr = m_pTimeline->PositionToMeasureBeat( m_dwGroupBits, 0, lXPos, &lMeasure, &lBeat );
	ASSERT( SUCCEEDED ( hr ) );
	hr = m_pTimeline->MeasureBeatToClocks(  m_dwGroupBits, 0, lMeasure, lBeat, &lClocks );
	ASSERT( SUCCEEDED ( hr ) );

	rpMelGen->m_MelGen.mtTime = lClocks;
	wcscpy(rpMelGen->m_MelGen.wszVariationLabel, L"<unused>");
	rpMelGen->m_MelGen.dwVariationFlags = 0xFFFFFFFF;
	rpMelGen->m_MelGen.dwRepeatFragmentID = 0;
	rpMelGen->m_MelGen.dwFragmentFlags = DMUS_FRAGMENTF_USE_PLAYMODE;
	rpMelGen->m_MelGen.dwPlayModeFlags = DMUS_PLAYMODE_NONE;
	// new...
	rpMelGen->m_MelGen.dwTransposeIntervals = 0;
	ZeroMemory(&rpMelGen->m_MelGen.Command, sizeof(rpMelGen->m_MelGen.Command));
	ZeroMemory(&rpMelGen->m_MelGen.ConnectionArc, sizeof(rpMelGen->m_MelGen.ConnectionArc));

	rpMelGen->m_dwMeasure = lMeasure;
	rpMelGen->m_bBeat = (BYTE)lBeat;

	//rpMelGen->SetSelectFlag( TRUE );
	InsertByAscendingTime( rpMelGen );

	// Mark this melody fragment as 'fake'
	rpMelGen->m_dwBits |= UD_FAKE;
	rpMelGen->m_MelGen.dwID = 0xFFFFFFFF;	// Fake ID nbr

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::MarkSelectedMelGens

// marks m_dwUndermined field CMelGenItems in list
void CMelGenMgr::MarkSelectedMelGens( DWORD dwFlags )
{
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
		if ( pMelGenItem->m_fSelected )
		{
			pMelGenItem->m_dwBits |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::DeleteMarked

// deletes MelGens marked by given flag
void CMelGenMgr::DeleteMarked( DWORD dwFlags )
{
	POSITION pos2, pos1 = m_lstMelGens.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos1 );
		if( (pMelGenItem->m_dwBits & dwFlags)
		||  (pMelGenItem->m_dwBits & UD_FAKE) )
		{
			RemoveMelGen( pos2 );
			delete pMelGenItem;
		}
	}

	// Update the property page
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::UnMarkMelGens

// unmarks flag m_dwUndermined field CMelGenItems in list
void CMelGenMgr::UnMarkMelGens( DWORD dwFlags )
{
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		m_lstMelGens.GetNext( pos )->m_dwBits &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::UnselectAllKeepBits

void CMelGenMgr::UnselectAllKeepBits()
{
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		m_lstMelGens.GetNext( pos )->m_fSelected = FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::UnselectAll

void CMelGenMgr::UnselectAll()
{
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		m_lstMelGens.GetNext( pos )->SetSelectFlag( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::SelectAll

void CMelGenMgr::SelectAll()
{
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
		// Only select "real" MelGens
		pMelGenItem->SetSelectFlag( !(pMelGenItem->m_dwBits & UD_FAKE) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::IsSelected

BOOL CMelGenMgr::IsSelected()
{
	// If anything "real" is selected, return TRUE.
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
		if( pMelGenItem->m_fSelected
		&&  !(pMelGenItem->m_dwBits & UD_FAKE) )
		{
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::FirstSelectedMelGen

CMelGenItem* CMelGenMgr::FirstSelectedMelGen()
{
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
		if ( pMelGenItem->m_fSelected )
		{
			return pMelGenItem;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::CurrentlySelectedMelGen

CMelGenItem* CMelGenMgr::CurrentlySelectedMelGen()
{
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
		if( pMelGenItem->m_fSelected
		&& (pMelGenItem->m_dwBits & UD_CURRENTSELECTION) )
		{
			return pMelGenItem;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::SelectSegment

BOOL CMelGenMgr::SelectSegment(long lBeginTime, long lEndTime)
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

	long lBeginMeas, lBeginBeat, lEndMeas, lEndBeat;
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lBeginTime, &lBeginMeas, &lBeginBeat );
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lEndTime, &lEndMeas, &lEndBeat );

	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pMelGenItem = m_lstMelGens.GetNext( pos );
		pMelGenItem->SetSelectFlag( FALSE );
		if( (DWORD)lBeginMeas < pMelGenItem->m_dwMeasure && pMelGenItem->m_dwMeasure < (DWORD)lEndMeas )
		{
			pMelGenItem->SetSelectFlag( TRUE );
			result = TRUE;
		}
		else if( (DWORD)lBeginMeas == pMelGenItem->m_dwMeasure )
		{
			if( (DWORD)lEndMeas == pMelGenItem->m_dwMeasure )
			{
				if( (BYTE)lBeginBeat <= pMelGenItem->m_bBeat && pMelGenItem->m_bBeat <= (BYTE)lEndBeat )
				{
					pMelGenItem->SetSelectFlag( TRUE );
					result = TRUE;
				}
			}
			else
			{
				if( lBeginBeat <= pMelGenItem->m_bBeat )
				{
					pMelGenItem->SetSelectFlag( TRUE );
					result = TRUE;
				}
			}
		}
		else if( (DWORD)lEndMeas == pMelGenItem->m_dwMeasure )
		{
			if( pMelGenItem->m_bBeat <= (BYTE)lEndBeat )
			{
				pMelGenItem->SetSelectFlag( TRUE );
				result = TRUE;
			}
		}
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::EmptyMelGenList

void CMelGenMgr::EmptyMelGenList(void)
{
	m_dwID = 0;
	if( !m_lstMelGens.IsEmpty() )
	{
		while ( !m_lstMelGens.IsEmpty() )
		{
			delete m_lstMelGens.RemoveHead();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::NewFragmentID

int CMelGenMgr::NewFragmentID(void)
{
/*	if (!m_dwID)
	{
		POSITION pos = m_lstMelGens.GetHeadPosition();
		CMelGenItem* pItem = NULL;
		DWORD dwMaxId = 1;
		while( pos )
		{
			pItem = m_lstMelGens.GetNext( pos );
			if (pItem->m_MelGen.dwID >= dwMaxId)
			{
				dwMaxId = pItem->m_MelGen.dwID + 1;
			}
		}
		m_dwID = dwMaxId;
	}
	else
	{
		m_dwID++;
	}

	return m_dwID;*/

    // This solution fixes bug 29770, but is n squared.
    // Find the highest ID
	POSITION pos = m_lstMelGens.GetHeadPosition();
	CMelGenItem* pItem = NULL;
	DWORD dwMaxId = 0;
	while( pos )
	{
		pItem = m_lstMelGens.GetNext( pos );
		if (pItem->m_MelGen.dwID > dwMaxId)
		{
			dwMaxId = pItem->m_MelGen.dwID;
		}
	}
    // Search for the lowest ID < maxID not in the lst
    DWORD dwMinId = 0;
    for (DWORD dwScan = 1; dwScan <= dwMaxId; dwScan++)
    {
	    pos = m_lstMelGens.GetHeadPosition();
	    CMelGenItem* pItem = NULL;
	    while( pos )
	    {
		    pItem = m_lstMelGens.GetNext( pos );
		    if (pItem->m_MelGen.dwID == dwScan)
		    {
			    dwMinId = dwScan;
                break;
		    }
	    }
        if (dwMinId != dwScan) break;
    }
    if (dwMinId < dwMaxId)
    {
        return dwScan;
    }
    return dwMaxId + 1;

}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::IsUniqueFragmentID

bool CMelGenMgr::IsUniqueFragmentID( CMelGenItem* pMelGen )
{
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pMelGenList = m_lstMelGens.GetNext( pos );

		if( pMelGen != pMelGenList )
		{
			if( pMelGen->m_MelGen.dwID == pMelGenList->m_MelGen.dwID )
			{
				return false;
			}
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::InsertByAscendingTime

void CMelGenMgr::InsertByAscendingTime( CMelGenItem *pMelGen )
{
	ASSERT( pMelGen );
	if ( pMelGen == NULL )
	{
		return;
	}

	CMelGenItem* pMelGenItem;
	POSITION posCurrent, posNext = m_lstMelGens.GetHeadPosition();
	while( posNext )
	{
		posCurrent = posNext;
		pMelGenItem = m_lstMelGens.GetNext( posNext );
		ASSERT( pMelGenItem );
		if( pMelGenItem )
		{
			if( pMelGenItem->m_MelGen.mtTime == pMelGen->m_MelGen.mtTime )
			{
				// replace item
				m_lstMelGens.InsertBefore( posCurrent, pMelGen );
				m_lstMelGens.RemoveAt( posCurrent );
				delete pMelGenItem;
				return;
			}
			if( pMelGenItem->m_MelGen.mtTime > pMelGen->m_MelGen.mtTime )
			{
				// insert before posCurrent (which is the position of pBandItem)
				m_lstMelGens.InsertBefore( posCurrent, pMelGen );
				return;
			}
		}
	}
	// insert at end of list
	m_lstMelGens.AddTail( pMelGen );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::RemoveItem

BOOL CMelGenMgr::RemoveItem( CMelGenItem* pItem )
{
	POSITION pos2;
	POSITION pos1 = m_lstMelGens.GetHeadPosition();
	while( pos1 )
	{
		pos2 = pos1;
		if ( m_lstMelGens.GetNext( pos1 ) == pItem )
		{
			RemoveMelGen( pos2 );
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::GetBoundariesOfSelectedMelGens

void CMelGenMgr::GetBoundariesOfSelectedMelGens( long *plStart, long *plEnd )
{
	ASSERT( plStart );
	ASSERT( plEnd );

	HRESULT hr;
	long lClocks;
	long lEnd = -1;
	BOOL fSetStart = FALSE;

	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		CMelGenItem* pItem = m_lstMelGens.GetNext( pos );

		if( pItem->m_fSelected ) 
//		&&	!(pMelGenItem->m_dwBits & UD_FAKE) )
		{
			hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits,
												   0,
												   pItem->m_dwMeasure,
												   pItem->m_bBeat,
												   &lClocks );
			ASSERT( SUCCEEDED ( hr ) );
			if( SUCCEEDED ( hr ) )
			{
				if( lEnd < lClocks )
				{
					lEnd = lClocks;
				}
				if( !fSetStart )
				{
					fSetStart = TRUE;
					*plStart = lClocks;
				}
			}
		}
	}

	if( lEnd >= 0 )
	{
		if( lEnd <= *plStart )
		{
			lEnd = *plStart + 1;
		}
		*plEnd = lEnd;
	}
	else
	{
		*plStart = -1;
		*plEnd = -1;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::DeleteBetweenTimes

void CMelGenMgr::DeleteBetweenTimes( long lStart, long lEnd )
{
	HRESULT hr;
	long lClocks;

	// Iterate through the list
	CMelGenItem* pItem;
	POSITION pos2, pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		// Save the current position
		pos2 = pos;
		pItem = m_lstMelGens.GetNext( pos );

		hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits,
											   0,
											   pItem->m_dwMeasure,
											   pItem->m_bBeat,
											   &lClocks );
		ASSERT( SUCCEEDED ( hr ) );
		if( SUCCEEDED ( hr ) )
		{
			// If the MelGen occurs between lStart and lEnd, delete it
			if( (lClocks >= lStart) && (lClocks <= lEnd) ) 
			{
				RemoveMelGen( pos2 );
				delete pItem;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::RemoveMelGen

void CMelGenMgr::RemoveMelGen(POSITION remPos)
{
	CMelGenItem* pRemItem = m_lstMelGens.GetAt(remPos);
	CMelGenItem* pItem = NULL;

	// get rid of all references to this item
	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		pItem = m_lstMelGens.GetNext( pos );
		if (pItem->m_pRepeat == &pRemItem->m_MelGen)
		{
			pItem->m_pRepeat = NULL;
		}

	}

	m_lstMelGens.RemoveAt( remPos );

}

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::HookUpRepeat

void CMelGenMgr::HookUpRepeat(CMelGenItem* pItem)
{
	CMelGenItem* pRepeat = NULL;

	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		pRepeat = m_lstMelGens.GetNext( pos );
		if (pItem->m_MelGen.dwRepeatFragmentID == pRepeat->m_MelGen.dwID)
		{
			pItem->m_pRepeat = &pRepeat->m_MelGen;
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::HookUpRepeats

void CMelGenMgr::HookUpRepeats()
{
	CMelGenItem* pItem = NULL;

	POSITION pos = m_lstMelGens.GetHeadPosition();
	while( pos )
	{
		pItem = m_lstMelGens.GetNext( pos );
		if (pItem->m_MelGen.dwFragmentFlags & DMUS_FRAGMENTF_USE_REPEAT)
		{
			HookUpRepeat(pItem);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr::OnRecompose

void CMelGenMgr::OnRecompose( void *pVoid )
{
	// Make sure we have the recompose on play or loop flag set
	if( 0 == (m_dwTrackExtrasFlags && (DMUS_TRACKCONFIG_PLAY_COMPOSE | DMUS_TRACKCONFIG_LOOP_COMPOSE)) )
	{
		return;
	}

	// Make sure we have a timeline pointer
	if( m_pTimeline == NULL )
	{
		return;
	}

	DMUS_NOTIFICATION_PMSG* pNotifyEvent = static_cast<DMUS_NOTIFICATION_PMSG *>(pVoid);
	if( NULL == pNotifyEvent )
	{
		return;
	}

	IDMUSProdSegmentEdit8 *pIDMUSProdSegmentEdit8;
	if( FAILED( m_pISegmentNode->QueryInterface( IID_IDMUSProdSegmentEdit8, (void**)&pIDMUSProdSegmentEdit8 ) ) )
	{
		return;
	}

	// Look for an existing pattern strip mgr
	IDMUSProdStripMgr *pPatternStripMgr = NULL;
	m_pTimeline->GetStripMgr(CLSID_DirectMusicPatternTrack, m_dwGroupBits, 0, &pPatternStripMgr );

	// No existing pattern strip mgr
	if( !pPatternStripMgr )
	{
		IDirectMusicSegmentState *pIDirectMusicSegmentState;
		if( (NULL == pNotifyEvent->punkUser)
		||	FAILED( pNotifyEvent->punkUser->QueryInterface( IID_IDirectMusicSegmentState, (void **)&pIDirectMusicSegmentState ) ) )
		{
			pIDMUSProdSegmentEdit8->Release();
			return;
		}

		IDirectMusicSegment *pIDirectMusicSegment;
		if( FAILED( pIDirectMusicSegmentState->GetSegment( &pIDirectMusicSegment ) ) )
		{
			pIDMUSProdSegmentEdit8->Release();
			pIDirectMusicSegmentState->Release();
			return;
		}
		pIDirectMusicSegmentState->Release();
		pIDirectMusicSegmentState = NULL;

		// Created a new Track, add it to the display and our internal list of tracks.
		IDirectMusicTrack *pIDMComposedTrack;
		if( FAILED( pIDirectMusicSegment->GetTrack( CLSID_DirectMusicPatternTrack, m_dwGroupBits, 0, &pIDMComposedTrack) ) )
		{
			pIDMUSProdSegmentEdit8->Release();
			pIDirectMusicSegment->Release();
			return;
		}
		pIDirectMusicSegment->Release();
		pIDirectMusicSegment = NULL;

		// Create a new strip manager for the track.
		IUnknown *punkStripMgr = NULL;

		// Create a Pattern track
		if( SUCCEEDED ( pIDMUSProdSegmentEdit8->AddStrip( CLSID_DirectMusicPatternTrack, m_dwGroupBits, &punkStripMgr ) ) )
		{
			punkStripMgr->QueryInterface(IID_IDMUSProdStripMgr, (void**) &pPatternStripMgr);
			punkStripMgr->Release();
		}
		if( pPatternStripMgr == NULL )
		{
			pIDMUSProdSegmentEdit8->Release();
			pIDMComposedTrack->Release();
			return;
		}

		// Let the pattern strip know about its new track
		VARIANT varDMTrack;
		varDMTrack.vt = VT_UNKNOWN;
		V_UNKNOWN( &varDMTrack ) = pIDMComposedTrack;
		HRESULT hr = pPatternStripMgr->SetStripMgrProperty( SMP_IDIRECTMUSICTRACK, varDMTrack );
		if( FAILED( hr ) )
		{
			pIDMComposedTrack->Release();
			pIDMUSProdSegmentEdit8->Release();
			pPatternStripMgr->Release();
			return;
		}

		// Let the segment editor know the pattern strip's new track
		IUnknown* pUnk = NULL;
		if (SUCCEEDED(pPatternStripMgr->QueryInterface(IID_IUnknown, (void**)&pUnk)))
		{
			IUnknown* pUnkTrack = NULL;
			if (SUCCEEDED(pIDMComposedTrack->QueryInterface(IID_IUnknown, (void**)&pUnkTrack)))
			{
				// Replace the track in the editor
				pIDMUSProdSegmentEdit8->ReplaceTrackInStrip(pUnk, pUnkTrack);
				pUnkTrack->Release();
			}

			// Release the IUnknown interface
			pUnk->Release();
		}

		pIDMComposedTrack->Release();
	}

	// Release the segment Edit interface
	pIDMUSProdSegmentEdit8->Release();
	pIDMUSProdSegmentEdit8 = NULL;

	// Reload the newly composed track data.
	IPersistStream *pIPersistStream;
	if( FAILED( pPatternStripMgr->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
	{
		pPatternStripMgr->Release();
		return;
	}

	// This causes the Pattern Strip to load the pattern from its attached IDirectMusicTrack.
	if( FAILED( pIPersistStream->Load( NULL ) ) )
	{
		pIPersistStream->Release();
		pPatternStripMgr->Release();
		return;
	}

	// Let the timeline display the strip
	VARIANT varTimeline;
	varTimeline.vt = VT_UNKNOWN;
	m_pTimeline->QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
	pPatternStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
	V_UNKNOWN(&varTimeline)->Release();

	// Let the pattern track give MIDI values to its note events, so they will display properly
	pPatternStripMgr->OnUpdate( GUID_Segment_AllTracksAdded, m_dwGroupBits, NULL );
  
	// Let the timeline know about the changes (the undo state is frozen, since we're within OnUpdate)
	m_pTimeline->OnDataChanged( pPatternStripMgr );

	// Release the strip manager
	pPatternStripMgr->Release();
	pPatternStripMgr = NULL;
	pIPersistStream->Release();
	pIPersistStream = NULL;

	// Disable playback of any style tracks in the track group(s) of the composed track.
	IDMUSProdStripMgr* pIStripMgr;
	DWORD dwIndex = 0;
	while( SUCCEEDED( m_pTimeline->GetStripMgr( CLSID_DirectMusicStyleTrack, m_dwGroupBits, dwIndex, &pIStripMgr ) ) )
	{
		DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
		ZeroMemory( &ioTrackExtrasHeader, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
		VARIANT varTrackHeader;
		varTrackHeader.vt = VT_BYREF;
		V_BYREF(&varTrackHeader) = &ioTrackExtrasHeader;
		if( SUCCEEDED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, &varTrackHeader ) ) )
		{
			ioTrackExtrasHeader.dwFlags &= ~DMUS_TRACKCONFIG_PLAY_ENABLED;
			pIStripMgr->SetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, varTrackHeader );
		}

		// Undo state aleady frozen, since we're in OnUpdate().

		// Let the object know about the changes
		IUnknown* pUnk;
		if (SUCCEEDED(pIStripMgr->QueryInterface(IID_IUnknown, (void**)&pUnk)))
		{
			m_pTimeline->OnDataChanged(pUnk);
			pUnk->Release();

			// At this point, it would also be nice if the Style strip could redraw
			// its property page, since we just modified it.  
			IDMUSProdPropSheet* pIPropSheet;
			if( SUCCEEDED ( m_pDMProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void **)&pIPropSheet ) ) )
			{
				pIPropSheet->RefreshActivePage();
				pIPropSheet->Release();
			}
		}

		pIStripMgr->Release();
		pIStripMgr = NULL;
		dwIndex++;
	}

	return;
}
