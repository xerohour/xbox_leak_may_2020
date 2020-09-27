// SignPostMgr.cpp : Implementation of CSignPostMgr

#include "stdafx.h"
#include "SignPostStripMgr.h"
#include "SignPostMgr.h"
#include "SignPostIO.h"
#include <dmusici.h>
#include <dmusicf.h>
#include "RiffStructs.h"
#include "RiffStrm.h"
#include "SegmentGuids.h"
#include "SegmentIO.h"

/////////////////////////////////////////////////////////////////////////////
// CSignPostMgr

typedef struct _ioSignPostDesign
{
	DWORD	dwActivityLevel;
} ioSignPostDesign;

#define DMUSPROD_FOURCC_SIGNPOST_DESIGN_CHUNK	mmioFOURCC('p','s','p','d')

/////////////////////////////////////////////////////////////////////////////
// CSignPostMgr IDMUSProdStripMgr

HRESULT STDMETHODCALLTYPE CSignPostMgr::GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	UNREFERENCED_PARAMETER(mtTime);
	UNREFERENCED_PARAMETER(pmtNext);
	
	ASSERT( pData != NULL );
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		BSTR bstr;
		CString str;
		str.LoadString(m_pSignPostStrip->m_nLastEdit);
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

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSignPostMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	UNREFERENCED_PARAMETER(guidType);
	UNREFERENCED_PARAMETER(mtTime);
	UNREFERENCED_PARAMETER(pData);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSignPostMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if(::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		return S_OK;
	}
	return S_FALSE;

}

HRESULT STDMETHODCALLTYPE CSignPostMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwGroupBits);
	UNREFERENCED_PARAMETER(pData);

	HRESULT hr = S_OK;
	if(::IsEqualGUID(rguidType, GUID_Segment_Length_Change))
	{
		hr = ResizeMeasureArray();
		if(hr == S_OK)
		{
			m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)m_pSignPostStrip, NULL, TRUE);
		}
	}
	else if(::IsEqualGUID(rguidType, GUID_TimeSignature))
	{
		RecalculateMusicTimeValues();
		UpdateSegment();
	}
	else if(::IsEqualGUID(rguidType, GUID_Segment_AllTracksAdded))
	{
		// Make sure measure array is the correct size
		hr = VerifyMeasureArray();
		if( FAILED ( hr ) )
		{
			return hr;
		}
		m_pSignPostStrip->m_fInAllTracksAdded = TRUE;
		ResizeMeasureArray();
		m_pSignPostStrip->m_fInAllTracksAdded = FALSE;
		// Load stream (deferred load)
		if( m_pTempIStream )
		{
			//seek to beginning
			LARGE_INTEGER	liTemp;
			liTemp.QuadPart = 0;
			m_pTempIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);	//seek to beginning
			Load( m_pTempIStream );

			// No need to keep m_pTempIStream around - we'll still have the same data
			// when the Timeline closes and re-opens
			m_pTempIStream->Release();
			m_pTempIStream = NULL;
		}
		// Make sure signpost times match time signature
		RecalculateMusicTimeValues();
		// Sync with DirectMusic
		SyncWithDirectMusic();
	}
	// Set the "Use to Compose" flag by default
	else if( ::IsEqualGUID(rguidType, GUID_Segment_CreateTrack))
	{
		m_dwTrackExtrasFlags |= DMUS_TRACKCONFIG_COMPOSING;
		UpdateSegment();
	}
	return hr;
}
HRESULT STDMETHODCALLTYPE CSignPostMgr::GetStripMgrProperty(
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

			// Make sure track belongs to only one track group
			for( int i = 0 ;  i < 32 ;  i++ )
			{
				if( m_dwGroupBits & (1 << i) )
				{
					m_dwGroupBits = (1 << i);
					m_dwOldGroupBits = (1 << i);
					break;
				}
			}
			pioTrackHeader->guidClassID = CLSID_DirectMusicSignPostTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = DMUS_FOURCC_SIGNPOST_TRACK_CHUNK;
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

			// Always set DMUS_TRACKCONFIG_COMPOSING
			m_dwTrackExtrasFlags |= DMUS_TRACKCONFIG_COMPOSING;
			pioTrackExtrasHeader->dwFlags = m_dwTrackExtrasFlags;
			pioTrackExtrasHeader->dwPriority = COMPOSITION_PRIORITY_SIGNPOST_STRIP;
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

HRESULT STDMETHODCALLTYPE CSignPostMgr::SetStripMgrProperty(
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
		if(m_pTimeline)
		{
			m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject *)this);
			if( m_pSignPostStrip )
			{
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject *)m_pSignPostStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pSignPostStrip );
				m_pSignPostStrip->Release();
				m_pSignPostStrip = NULL;
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Length_Change, m_dwOldGroupBits);
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwOldGroupBits);
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits);
			m_pTimeline->Release();
			m_pTimeline = NULL;
		}
		if( V_UNKNOWN( &variant ) )
		{
			if ( FAILED(V_UNKNOWN( &variant )->QueryInterface(IID_IDMUSProdTimeline, (void**)&m_pTimeline) ) )
			{
				return E_FAIL;
			}
			else
			{
				// Only support handling one strip at a time
				if ( m_pSignPostStrip )
				{
					return E_FAIL;
				}


				// Create a strip and add it to the timeline
				m_pSignPostStrip = new CSignPostStrip(this);
				if( !m_pSignPostStrip )
				{
					return E_OUTOFMEMORY;
				}

				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pSignPostStrip, CLSID_DirectMusicSignPostTrack, m_dwGroupBits, 0 );
				m_pTimeline->AddToNotifyList((IDMUSProdStripMgr *)this, GUID_Segment_Length_Change, m_dwGroupBits);
				m_pTimeline->AddToNotifyList((IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
				m_pTimeline->AddToNotifyList((IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
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
		if( m_pIFramework )
		{
			m_pIFramework->Release();
			m_pIFramework = NULL;
		}
		if( V_UNKNOWN( &variant ) )
		{
			return V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pIFramework);
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
// CSignPostMgr IPersist

HRESULT CSignPostMgr::GetClassID(CLSID* pClsId)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pClsId);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostMgr IPersistStream functions

HRESULT CSignPostMgr::IsDirty()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return S_FALSE;
}

HRESULT CSignPostMgr::Load(IStream* pIStream)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	// If the strip hasn't been added to the timeline, save the stream until it
	// is added to the Timeline.
	if( m_pTimeline == NULL )
	{
		if( m_pTempIStream )
		{
			m_pTempIStream->Release();
			m_pTempIStream = NULL;
		}
		if( SUCCEEDED( CreateStreamOnHGlobal( NULL, TRUE, &m_pTempIStream ) ) )
		{
			STATSTG StatStg;
			if ( SUCCEEDED( pIStream->Stat( &StatStg, STATFLAG_NONAME ) ) )
			{
				if( SUCCEEDED( pIStream->CopyTo( m_pTempIStream, StatStg.cbSize, NULL, NULL ) ) )
				{
					if( m_pIDMTrack )
					{
						IPersistStream* pIPersistStream;
						if( SUCCEEDED( m_pIDMTrack->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
						{
							//seek to beginning
							LARGE_INTEGER	liTemp;
							liTemp.QuadPart = 0;
							m_pTempIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );
							pIPersistStream->Load( m_pTempIStream );
							pIPersistStream->Release();
						}
					}
					return S_OK;
				}
				else
				{
					TRACE("CSignPostMgr: Unable to copy to memory stream\n");
					m_pTempIStream->Release();
					m_pTempIStream = NULL;
					return E_FAIL;
				}
			}
			else
			{
				TRACE("CSignPostMgr: Unable to stat stream\n");
				m_pTempIStream->Release();
				m_pTempIStream = NULL;
				return E_FAIL;
			}
		}
		else
		{
			TRACE("CSignPostMgr: Unable to create memory stream\n");
			return E_FAIL;
		}
	}

	VARIANT var;
	long	lMeasureLength;
	long	lTimeLength;

	// Clear the list in case the clocks per measure has changed.
	ClearSignPostList(m_pSignPostList, FALSE);
	m_pSignPostList = NULL;

	// Get the length of the timeline and make sure that m_nMeasures and m_lShiftFromMeasure
	// are accurate.

	m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	lTimeLength = V_I4( &var );
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lTimeLength, &lMeasureLength, NULL );
	// Ensure lMeasureLength is at least 1
	lMeasureLength = max( 1, lMeasureLength );
	if( m_nMeasures != (short) lMeasureLength )
	{
		if( m_pMeasureArray != NULL )
		{
			delete [] m_pMeasureArray;
			m_pMeasureArray = NULL;
		}

		m_pMeasureArray = new SignPostMeasureInfo[lMeasureLength];
		if( m_pMeasureArray == NULL)
		{
			return E_OUTOFMEMORY;
		}
		m_nMeasures = (short) lMeasureLength;
		// Initialize the array.
		for( short n = 0; n < m_nMeasures; n++ )
		{
			m_pMeasureArray[n].dwFlags = 0;
			m_pMeasureArray[n].pSignPost = NULL;
		}

		if( m_lShiftFromMeasure >= m_nMeasures )
		{
			m_lShiftFromMeasure = m_nMeasures - 1;
		}
	}
	
	BOOL fChanged = FALSE;

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Check for Direct Music format
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Load the Track
	MMCKINFO	ck;
	DWORD dwCurPos = StreamTell( pIStream );
	bool fLoadedDesignChunk = false;
	while( SUCCEEDED(hr)
	&&	(pIRiffStream->Descend( &ck, NULL, 0 ) == 0) )
	{
		switch( ck.ckid )
		{
		case DMUS_FOURCC_SIGNPOST_TRACK_CHUNK:
			StreamSeek( pIStream, dwCurPos, STREAM_SEEK_SET );
			hr = LoadSignPostList( pIStream, -1, FALSE, fChanged );
			break;

		case DMUSPROD_FOURCC_SIGNPOST_DESIGN_CHUNK:
			{
				ioSignPostDesign iSignPostDesign;
				ZeroMemory( &iSignPostDesign, sizeof(ioSignPostDesign) );
				DWORD dwRead, dwSize = min( ck.cksize, sizeof(ioSignPostDesign) );
				hr = pIStream->Read( &iSignPostDesign, dwSize, &dwRead );
				if( FAILED(hr) || (dwSize != dwRead) )
				{
					hr = E_FAIL;
				}
				else
				{
					m_dwActivityLevel = iSignPostDesign.dwActivityLevel;
				}
				fLoadedDesignChunk = true;
			}
			break;
		}
		pIRiffStream->Ascend( &ck, 0 );
		dwCurPos = StreamTell( pIStream );
	}

	pIRiffStream->Release();

	// If we didn't load a design chunk, try and get the activity level from the segment
	if( !fLoadedDesignChunk )
	{
		m_pTimeline->GetParam( GUID_LegacyTemplateActivityLevel, 0xFFFFFFFF, 0, 0, NULL, &m_dwActivityLevel );
	}

	ClearSelected();
	return hr;
}

HRESULT CSignPostMgr::Save(IStream* pIStream, BOOL fClearDirty)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(fClearDirty);
	HRESULT hr = SaveSignPostList(pIStream, FALSE);
	if( FAILED( hr ) )
	{
		return hr;
	}

	DMUSProdStreamInfo	StreamInfo;
	StreamInfo.ftFileType = FT_DESIGN;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pPersistInfo->Release();
	}

	// If we're saving the design file
	if( StreamInfo.ftFileType == FT_DESIGN )
	{
		// Create a RIFF Stream
		IDMUSProdRIFFStream* pIRiffStream;
		if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			return hr;
		}

		// Create a design chunk
		MMCKINFO ck;
		ck.ckid = DMUSPROD_FOURCC_SIGNPOST_DESIGN_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			pIRiffStream->Release();
			return E_FAIL;
		}

		// Write out the design data
		ioSignPostDesign oSignPostDesign;
		ZeroMemory( &oSignPostDesign, sizeof(ioSignPostDesign) );
		oSignPostDesign.dwActivityLevel = m_dwActivityLevel;
		DWORD dwWritten;
		hr = pIStream->Write( &oSignPostDesign, sizeof(ioSignPostDesign), &dwWritten );
		if( FAILED(hr) || (sizeof(ioSignPostDesign) != dwWritten) )
		{
			hr = E_FAIL;
		}

		// Ascend out of the design chunk
		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
		}

		// Release the RIFF stream
		pIRiffStream->Release();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostMgr::SyncWithDirectMusic

HRESULT CSignPostMgr::SyncWithDirectMusic(  )
{
	HRESULT hr = E_FAIL;

	// Validate m_pIDMTrack and m_pIFramework
	ASSERT( m_pIDMTrack );
	ASSERT( m_pIFramework );
	if( m_pIDMTrack && m_pIFramework )
	{
		// Create a Memory stream
		IStream* pIMemStream;
		hr = m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );
		if( SUCCEEDED ( hr ) )
		{
			// Seek to the beginning
			StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );

			// Save ourself
			hr = Save(pIMemStream, FALSE);
			if (SUCCEEDED(hr))
			{
				// Query m_pIDMTrack for a IPersistStream interface
				IPersistStream* pIPersistStream;
				hr = m_pIDMTrack->QueryInterface(IID_IPersistStream, (void**)&pIPersistStream);
				if (SUCCEEDED(hr))
				{
					// Seek to the beginning
					hr = StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
					if( SUCCEEDED( hr ) )
					{
						// Load the stream into the track.
						hr = pIPersistStream->Load(pIMemStream);
					}
					pIPersistStream->Release();
				}
			}
			pIMemStream->Release();
		}
	}

	return hr;
}


HRESULT CSignPostMgr::GetSizeMax(ULARGE_INTEGER FAR* pcbSize)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pcbSize);
	return E_NOTIMPL;
}

HRESULT CSignPostMgr::VerifyMeasureArray()
{
	if(m_pMeasureArray == NULL)
	{
		VARIANT var;
		long	lMeasureLength;
		long	lTimeLength;

		m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		lTimeLength = V_I4( &var );
		m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lTimeLength, &lMeasureLength, NULL );
		// Ensure lMeasureLength is at least 1
		lMeasureLength = max( 1, lMeasureLength );

		m_pMeasureArray = new SignPostMeasureInfo[lMeasureLength];
		if( m_pMeasureArray == NULL)
		{
			return E_OUTOFMEMORY;
		}
		m_nMeasures = (short) lMeasureLength;
		// Initialize the array.
		for( short n = 0; n < m_nMeasures; n++ )
		{
			m_pMeasureArray[n].dwFlags = 0;
			m_pMeasureArray[n].pSignPost = NULL;
		}

		if( m_lShiftFromMeasure >= m_nMeasures )
		{
			m_lShiftFromMeasure = m_nMeasures - 1;
		}
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject functions

HRESULT STDMETHODCALLTYPE CSignPostMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL	bValid = FALSE;
	short	n;

	//ASSERT(m_pMeasureArray != NULL);
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return hr;

	m_PPGSignPost.dwValid = 0;

	if(m_bSelected)
	{
		// Set the fields of the PPGSignPost
		for( n = 0; n < m_nMeasures; n++ )
		{
			if( m_pMeasureArray[n].dwFlags & SPMI_SELECTED )
			{
				if( m_pMeasureArray[n].pSignPost != NULL )
				{
					if( !bValid )
					{
						m_PPGSignPost.dwValid		= VALID_SIGNPOST | VALID_CADENCE;
						m_PPGSignPost.dwSignPost	= m_pMeasureArray[n].pSignPost->dwSignPost;
						bValid						= TRUE;
					}
					else
					{
						 if(m_PPGSignPost.dwValid & VALID_SIGNPOST)
						 {
							 if((m_PPGSignPost.dwSignPost & ~SP_CADENCE) !=
								(m_pMeasureArray[n].pSignPost->dwSignPost & ~SP_CADENCE))
							 {
								 m_PPGSignPost.dwValid &= ~VALID_SIGNPOST;
							 }
						 }
						 if(m_PPGSignPost.dwValid & VALID_CADENCE)
						 {
							 if((m_PPGSignPost.dwSignPost & SP_CADENCE) !=
								(m_pMeasureArray[n].pSignPost->dwSignPost & SP_CADENCE))
							 {
								 m_PPGSignPost.dwValid &= ~VALID_CADENCE;
							 }
						 }

						 if(m_PPGSignPost.dwValid == 0)
						 {
							 break;
						 }
					}
				}
				else if(!bValid)
				{
					bValid = TRUE;
					m_PPGSignPost.dwValid = VALID_SIGNPOST | VALID_CADENCE;
					m_PPGSignPost.dwSignPost = 0;
				}
				else
				{
					if(m_PPGSignPost.dwValid & VALID_SIGNPOST && 
					  (m_PPGSignPost.dwSignPost & ~SP_CADENCE) != 0)
					{
						m_PPGSignPost.dwValid &= ~VALID_SIGNPOST;
					}
					if( m_PPGSignPost.dwValid & VALID_CADENCE && 
						( m_PPGSignPost.dwSignPost & SP_CADENCE) != 0 )
					{
						m_PPGSignPost.dwValid &= ~VALID_CADENCE;
					}
					m_PPGSignPost.dwSignPost = 0;
				}		
			}
		}
		*ppData = &m_PPGSignPost;
	}
	else
	{
		*ppData = NULL;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSignPostMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL		bValid = FALSE;
	DWORD		dwSignPost;
	SignPostExt	*pSPE;
	short		n;

	if(pData == NULL)
	{
		return E_INVALIDARG;
	}

	ASSERT(m_bSelected && m_pMeasureArray != NULL);
	if(!m_bSelected || m_pMeasureArray == NULL)
	{
		return E_UNEXPECTED;
	}

	memcpy(&m_PPGSignPost, pData, sizeof(PPGSignPost));

	if(m_PPGSignPost.dwValid == (VALID_SIGNPOST | VALID_CADENCE) &&
	   m_PPGSignPost.dwSignPost == 0)
	{
		DeleteSelected();
		if (m_pTimeline)
		{
			m_pTimeline->Refresh();
		}
		return S_OK;
	}

	// Apply the changes to the selected measures.
	for(n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].dwFlags & SPMI_SELECTED)
		{
			if(m_pMeasureArray[n].pSignPost != NULL)
			{
				dwSignPost = m_pMeasureArray[n].pSignPost->dwSignPost;
			}
			else
			{
				dwSignPost = 0;
			}

			// Get the signpost for this measure.
			if(m_PPGSignPost.dwValid & VALID_SIGNPOST)
			{
				dwSignPost = ((dwSignPost & SP_CADENCE) | (m_PPGSignPost.dwSignPost & ~SP_CADENCE));
			}
			if(m_PPGSignPost.dwValid & VALID_CADENCE)
			{
				dwSignPost = ((dwSignPost & ~SP_CADENCE) | (m_PPGSignPost.dwSignPost & SP_CADENCE));
			}

			// Update the measure.
			if(dwSignPost == 0 && m_pMeasureArray[n].pSignPost != NULL)
			{
				delete m_pMeasureArray[n].pSignPost;
				m_pMeasureArray[n].pSignPost = NULL;
			}
			if(dwSignPost != 0)
			{
				if(m_pMeasureArray[n].pSignPost == NULL)
				{
					m_pMeasureArray[n].pSignPost = new SignPostExt;
					if(m_pMeasureArray[n].pSignPost == NULL)
					{
						return E_OUTOFMEMORY;
					}
				}
				m_pMeasureArray[n].pSignPost->dwSignPost = dwSignPost;
				m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, (long) n, 0, &(m_pMeasureArray[n].pSignPost->lTime));
			}
			bValid = TRUE;
		}
	}

	// Update the "next" pointers
	m_pSignPostList = NULL;
	pSPE = NULL;
	for(n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].pSignPost != NULL)
		{
			if(m_pSignPostList == NULL)
			{
				m_pSignPostList = m_pMeasureArray[n].pSignPost;
			}
			else
			{
				pSPE->pNext = m_pMeasureArray[n].pSignPost;
			}
			pSPE = m_pMeasureArray[n].pSignPost;
			pSPE->pNext = NULL;
		}
	}

	ASSERT(bValid);
	ASSERT(m_pTimeline != NULL);
	if(!bValid || m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}
	m_pTimeline->Refresh();

	// Let the object know about the changes
	m_pSignPostStrip->m_nLastEdit = IDS_UNDO_CHANGE;
	UpdateSegment();
	return S_OK;
}

HRESULT CSignPostMgr::OnShowProperties(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr = S_OK;
	if(m_pIPageManager == NULL)
	{
		CSignPostPPGMgr *pPPM = new CSignPostPPGMgr;
		if(pPPM == NULL)
		{
			return E_OUTOFMEMORY;
		}
		hr = pPPM->QueryInterface(IID_IDMUSProdPropPageManager, (void**)&m_pIPageManager);
		m_pIPageManager->Release(); // this releases the 2nd ref, leaving only one
		if(FAILED(hr))
		{
			return hr;
		}
	}
	ASSERT(m_pTimeline != NULL);
	if(m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}
	m_pTimeline->SetPropertyPage(m_pIPageManager, (IDMUSProdPropPageObject*)this);
	m_pIPageManager->RefreshData();
	return hr;
}

HRESULT CSignPostMgr::OnRemoveFromPageManager(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return S_OK;
}

HRESULT CSignPostMgr::SaveSignPostList(IStream* pStream, BOOL bOnlySelected, bool bNormalize)
{
    MMCKINFO ck;
	HRESULT		hr;
    DWORD		dwSizeSignPost;
	DWORD		cb;
    DMUS_IO_SIGNPOST	iSignPost;
	WORD		wMeasureOffset = 0;

	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pMeasureArray != NULL );
	if( m_pMeasureArray == NULL )
	{
		return E_UNEXPECTED;
	}

	if(NULL == pStream)
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( hr = AllocRIFFStream( pStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	ck.ckid = DMUS_FOURCC_SIGNPOST_TRACK_CHUNK;
	hr = pIRiffStream->CreateChunk( &ck, 0 );
    if( hr == S_OK )
    {
		if(bNormalize)
		{
			wMeasureOffset = GetFirstSelectedMeasure();
		}
		// Write the size of the signpost structure
		dwSizeSignPost = sizeof(DMUS_IO_SIGNPOST);
		hr = pStream->Write(&dwSizeSignPost, sizeof(dwSizeSignPost), &cb);
		if(FAILED(hr) || cb != sizeof(dwSizeSignPost))
		{
			hr = E_FAIL;
		}

		else
		{
			// Write the signposts
			for(short n = 0; n < m_nMeasures; n++)
			{
				m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, n, 0, &iSignPost.mtTime);
				if(m_pMeasureArray[n].pSignPost != NULL)
				{
					iSignPost.dwChords = m_pMeasureArray[n].pSignPost->dwSignPost;
					m_pMeasureArray[n].pSignPost->lTime = iSignPost.mtTime;
				}
				else
				{
					iSignPost.dwChords = 0;
				}
				iSignPost.wMeasure = (WORD)(n - wMeasureOffset);

				if((bOnlySelected && (m_pMeasureArray[n].dwFlags & SPMI_SELECTED)) ||
				   (!bOnlySelected && iSignPost.dwChords != 0))
				{
					hr = pStream->Write(&iSignPost, sizeof(DMUS_IO_SIGNPOST), &cb);
					if(FAILED(hr) || cb != sizeof(DMUS_IO_SIGNPOST))
					{
						hr = E_FAIL;
						break;
					}
				}
			}
		}
	}
	hr = pIRiffStream->Ascend( &ck, 0 );
	pIRiffStream->Release();

	return hr;
}


void CSignPostMgr::ClearSignPostList(SignPostExt *pSignPostList, BOOL bClearSelection)
{
	SignPostExt* pSPE;

	if(pSignPostList == m_pSignPostList)
	{
		if(m_nMeasures > 0)
		{
			ASSERT(m_pMeasureArray != NULL);
			if(m_pMeasureArray != NULL)
			{
				for(short n = 0; n < m_nMeasures; n++)
				{
					m_pMeasureArray[n].pSignPost = NULL;
					if(bClearSelection)
					{
						m_pMeasureArray[n].dwFlags &= ~SPMI_SELECTED;
					}
				}
			}
		}
	}

	while(pSignPostList)
	{
		pSPE = pSignPostList->pNext;
		delete pSignPostList;
		pSignPostList = pSPE;
	}
}

void CSignPostMgr::SelectSegment(long lBeginTime, long lEndTime)
{
	long	lBegin;
	long	lEnd;
	long	lMeasure;
	long	lTemp;

	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return;
	}

	ASSERT( m_pMeasureArray != NULL );
	if( m_pMeasureArray == NULL )
	{
		return;
	}

	lTemp = lBeginTime;
	if(lBeginTime == -1)	// Use shiftfrommeasure
	{
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, m_lShiftFromMeasure, 0, &lTemp);
	}

	if(lTemp > lEndTime)
	{
		lBegin	= lEndTime;
		lEnd	= lTemp;
	}
	else
	{
		lBegin	= lTemp;
		lEnd	= lEndTime;
	}

	// Convert lBegin and lEnd to measures.
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lBegin, &lMeasure, NULL);
	lBegin = lMeasure;
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lEnd, &lMeasure, NULL);
	lEnd = lMeasure;

	ASSERT(lEnd <= (long) m_nMeasures);
	if(lEnd > m_nMeasures)
	{
		return;
	}

	// Set the selected flag of the signposts in this segment.
	short n;
	for(n = (short) lBegin; n <= (short) lEnd && n < m_nMeasures; n++)
	{
		m_pMeasureArray[n].dwFlags |= SPMI_SELECTED;
		m_bSelected = TRUE;
	}
}

HRESULT CSignPostMgr::InsertSignPost(long lMeasure)
{
	long		lClocks;
	long		lInsertMeasure;

	lInsertMeasure = lMeasure;
	if(lInsertMeasure == -1)
	{
		lInsertMeasure = (long) GetFirstSelectedMeasure();
	}

	ASSERT(lInsertMeasure > -1 && lInsertMeasure < (long) m_nMeasures);
	if(lInsertMeasure < 0 || lInsertMeasure >= (long)m_nMeasures)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pTimeline != NULL && m_pMeasureArray != NULL);
	if(m_pTimeline == NULL || m_pMeasureArray == NULL)
	{
		return E_UNEXPECTED;
	}

	if(m_pMeasureArray[lInsertMeasure].pSignPost != NULL)
	{
		// Just open the property page.
		OnShowProperties();
		return S_OK;
	}

	m_pMeasureArray[lInsertMeasure].pSignPost = new SignPostExt;
	if(m_pMeasureArray[lInsertMeasure].pSignPost == NULL)
	{
		return E_OUTOFMEMORY;
	}

	ClearSelected();
	m_pMeasureArray[lInsertMeasure].dwFlags |= SPMI_SELECTED;
	m_bSelected = TRUE;

	// Update the list pointers.
	short n;

	// Find the previous signpost.
	for(n = short (lInsertMeasure - 1); n > -1; n--)
	{
		if(m_pMeasureArray[n].pSignPost != NULL)
		{
			break;
		}
	}

	if(n > -1)
	{
		m_pMeasureArray[n].pSignPost->pNext = m_pMeasureArray[lInsertMeasure].pSignPost;

		// Find the next signpost.
		for(n = short (lInsertMeasure + 1); n < m_nMeasures; n++)
		{
			if(m_pMeasureArray[n].pSignPost != NULL)
			{
				break;
			}
		}
		if(n < m_nMeasures)
		{
			m_pMeasureArray[lInsertMeasure].pSignPost->pNext = m_pMeasureArray[n].pSignPost;
		}
		else
		{
			m_pMeasureArray[lInsertMeasure].pSignPost->pNext = NULL;
		}
	}

	else
	{
		/* this doesn't seem right
		ASSERT(m_pSignPostList == NULL);
		if(m_pSignPostList != NULL)
		{
			return E_UNEXPECTED;
		}
		*/
		SignPostExt* pSignPost = m_pSignPostList;
		m_pSignPostList = m_pMeasureArray[lInsertMeasure].pSignPost;
		m_pSignPostList->pNext = pSignPost;
	}

	m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lInsertMeasure, 0, &lClocks);

	// Initialize the signpost.
	m_pMeasureArray[lInsertMeasure].pSignPost->lTime		= lClocks;
	m_pMeasureArray[lInsertMeasure].pSignPost->dwSignPost	= SP_1;


	// Get a pointer to the property sheet and show it
	ASSERT( m_pIFramework );
	IDMUSProdPropSheet	*pIPropSheet;
	if( FAILED( m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet) ) )
	{
		return E_FAIL;
	}
	pIPropSheet->Show(TRUE);
	pIPropSheet->Release();

	// Update the property page.
	OnShowProperties();
	UpdateSegment();
	return S_OK;
}

void CSignPostMgr::ClearSelected()
{
	//ASSERT(m_pMeasureArray != NULL);
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	short n;

	for(n = 0; n < m_nMeasures; n++)
	{
		m_pMeasureArray[n].dwFlags &= ~SPMI_SELECTED;
	}

	m_bSelected = FALSE;
}

void CSignPostMgr::SelectAll()
{
	//ASSERT(m_pMeasureArray != NULL);
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	short n;

	for(n = 0; n < m_nMeasures; n++)
	{
		m_pMeasureArray[n].dwFlags |= SPMI_SELECTED;
	}

	m_bSelected = TRUE;

	if(m_pIPageManager != NULL)
	{
		m_pIPageManager->RefreshData();
	}
}

void CSignPostMgr::DeleteSelected()
{
	SignPostExt *pSPE = NULL;
	short		n;

	//ASSERT(m_pMeasureArray != NULL || m_nMeasures != 0);
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	for(n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].pSignPost != NULL)
		{
			if(m_pMeasureArray[n].dwFlags & SPMI_SELECTED)
			{
				if(pSPE == NULL)
				{
					ASSERT(m_pSignPostList == m_pMeasureArray[n].pSignPost);
					m_pSignPostList = m_pMeasureArray[n].pSignPost->pNext;
				}
				else
				{
					ASSERT(pSPE->pNext == m_pMeasureArray[n].pSignPost);
					pSPE->pNext = m_pMeasureArray[n].pSignPost->pNext;
				}
				delete m_pMeasureArray[n].pSignPost;
				m_pMeasureArray[n].pSignPost = NULL;
			}
			else
			{
				pSPE = m_pMeasureArray[n].pSignPost;
			}
		}
		// Leave the measure selected, so the user can still use the selection.
//		m_pMeasureArray[n].dwFlags &= ~SPMI_SELECTED;
	}

	if(m_pIPageManager != NULL)
	{
		m_pIPageManager->RefreshData();
	}

	m_pSignPostStrip->m_nLastEdit = IDS_UNDO_DELETE;
	UpdateSegment();
}

void CSignPostMgr::ToggleSelect(long lMeasure)
{
	//ASSERT(m_pMeasureArray != NULL);
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	if(lMeasure >= (long) m_nMeasures)
	{
		return;
	}

	m_pMeasureArray[lMeasure].dwFlags ^= SPMI_SELECTED;
	if(m_pMeasureArray[lMeasure].dwFlags & SPMI_SELECTED)
	{
		m_bSelected = TRUE;
	}

	if( m_pIPageManager != NULL )
	{
		m_pIPageManager->RefreshData();
	}
}

BOOL CSignPostMgr::IsSelected(long lMeasure)
{
	//ASSERT(m_pMeasureArray != NULL);
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return FALSE;

	ASSERT(lMeasure < (long) m_nMeasures);
	if(lMeasure >= (long) m_nMeasures)
	{
		return FALSE;
	}

	return (m_pMeasureArray[lMeasure].dwFlags & SPMI_SELECTED ? TRUE : FALSE);
}

short CSignPostMgr::GetFirstSelectedMeasure()
{
	short n;

	if(m_pMeasureArray == NULL)
	{
		return -1;
	}

	for(n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].dwFlags & SPMI_SELECTED)
		{
			return n;
		}
	}

	return -1;
}

SignPostExt* CSignPostMgr::GetFirstSelectedSignPost()
{
	for(int n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].pSignPost && m_pMeasureArray[n].dwFlags & SPMI_SELECTED)
		{
			return m_pMeasureArray[n].pSignPost;
		}
	}
	return NULL;
}

void CSignPostMgr::GetBoundariesOfSelectedCommands( long *plStart, long *plEnd )
{
	ASSERT( plStart );
	ASSERT( plEnd );

	if( m_pMeasureArray == NULL )
	{
		*plStart = -1;
		*plEnd = -1;
		return;
	}

	BOOL fSetStart = FALSE;
	long lEnd = -1;
	for( int n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].dwFlags & SPMI_SELECTED )
		{
			if( lEnd < n )
			{
				lEnd = n;
			}
			if( !fSetStart )
			{
				fSetStart = TRUE;
				*plStart = n;
			}
		}
	}

	if( !fSetStart )
	{
		*plStart = -1;
		*plEnd = -1;
	}
	else
	{
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, *plStart, 0, plStart );
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lEnd, 0, plEnd );
		if( *plEnd <= *plStart )
		{
			*plEnd = *plStart + 1;
		}
	}
}

BOOL CSignPostMgr::DeleteBetweenTimes( long lStart, long lEnd )
{
	ASSERT( lStart >=0 );
	ASSERT( lEnd >= 0 );

	BOOL fChanged = FALSE;

	if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lStart, &lStart, NULL ) )
	&&  SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lEnd, &lEnd, NULL ) ) )
	{
		lEnd = min( lEnd, (m_nMeasures - 1) );
		SignPostExt*	pCE = NULL;
		for( int n=0; n <= lEnd; n++ )
		{
			if( n >= lStart )
			{
				if( n > lEnd )
				{
					break;
				}
				if( m_pMeasureArray[n].pSignPost )
				{
					if( pCE == NULL )
					{
						ASSERT( m_pSignPostList == m_pMeasureArray[n].pSignPost );
						m_pSignPostList = m_pMeasureArray[n].pSignPost->pNext;
					}
					else
					{
						ASSERT( pCE->pNext == m_pMeasureArray[n].pSignPost );
						pCE->pNext = m_pMeasureArray[n].pSignPost->pNext;
					}
					delete m_pMeasureArray[n].pSignPost;
					m_pMeasureArray[n].pSignPost = NULL;
					fChanged = TRUE;
				}
			}
			else
			{
				if( m_pMeasureArray[n].pSignPost )
				{
					pCE = m_pMeasureArray[n].pSignPost;
				}
			}
		}
	}

	return fChanged;
}

void CSignPostMgr::UpdateSegment( void )
{
	ASSERT( m_pTimeline );
	if( m_pTimeline )
	{
		m_pTimeline->OnDataChanged( (IDMUSProdStripMgr *)this );
	}

	SyncWithDirectMusic();
}

short CSignPostMgr::MarkSelectedSignPosts(DWORD flags)
// returns offset of first measure from zero
{
	short	n;
	BOOL	bFirstTime = TRUE;
	short	nOffset = 0;

	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return 0;

	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].dwFlags & SPMI_SELECTED )
		{
			if(bFirstTime)
			{
				nOffset = n;
				bFirstTime = FALSE;
			}
			if( m_pMeasureArray[n].pSignPost != NULL )
			{
				m_pMeasureArray[n].pSignPost->dwDragDrop |= flags;
			}
		}
	}
	return nOffset;
}

void CSignPostMgr::DeleteMarkedSignPosts(DWORD dwFlags)
{
	SignPostExt*	pCE = NULL;
	short		n;

	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pSignPost != NULL )
		{
			if( m_pMeasureArray[n].pSignPost->dwDragDrop & dwFlags )
			{
				if( pCE == NULL )
				{
					ASSERT( m_pSignPostList == m_pMeasureArray[n].pSignPost );
					m_pSignPostList = m_pMeasureArray[n].pSignPost->pNext;
				}
				else
				{
					ASSERT( pCE->pNext == m_pMeasureArray[n].pSignPost );
					pCE->pNext = m_pMeasureArray[n].pSignPost->pNext;
				}
				delete m_pMeasureArray[n].pSignPost;
				m_pMeasureArray[n].pSignPost = NULL;
			}
			else
			{
				pCE = m_pMeasureArray[n].pSignPost;
			}
		}
		// Leave the measure selected, so the user can still use the selection.
//		m_pMeasureArray[n].dwFlags &= ~CMI_SELECTED;
	}

	if( m_pIPageManager != NULL )
	{
		m_pIPageManager->RefreshData();
	}
//	UpdateSegment();
}

void CSignPostMgr::UnMarkSignPosts(WORD flags)
{
	short		n;
	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pSignPost != NULL )
		{
			m_pMeasureArray[n].pSignPost->dwDragDrop &= ~flags;
		}
	}
}

HRESULT CSignPostMgr::SaveDroppedSignPostList( IStream * pStream, BOOL bOnlySelected, short nOffsetMeasure )
{
	HRESULT		hr;
    DWORD		dwSizeSignPost;
	DWORD		cb;
    DMUS_IO_SIGNPOST	iSignPost;
	long		lMeasureLength;

	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	//ASSERT(m_pMeasureArray != NULL );
	hr = VerifyMeasureArray();
	if (FAILED(hr)) return hr;

	if( NULL == pStream )
	{
		return E_INVALIDARG;
	}

	m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, 1, 0, &lMeasureLength );

	// Write the size of the signpost structure
	dwSizeSignPost = sizeof( DMUS_IO_SIGNPOST );
	hr = pStream->Write( &dwSizeSignPost, sizeof( dwSizeSignPost ), &cb );
    if( FAILED( hr ) || cb != sizeof( dwSizeSignPost ))
	{
		return E_FAIL;
	}

	// Write the signposts
	for( short n = 0; n < m_nMeasures; n++ )
	{
		iSignPost.mtTime		= (long) n * lMeasureLength;
		iSignPost.wMeasure		= WORD (n - nOffsetMeasure);
		if( m_pMeasureArray[n].pSignPost != NULL )
		{
			iSignPost.dwChords = m_pMeasureArray[n].pSignPost->dwSignPost;
		}
		else
		{
			iSignPost.dwChords = 0;
		}

		if(( bOnlySelected && ( m_pMeasureArray[n].dwFlags & SPMI_SELECTED )) ||
		   ( !bOnlySelected && (iSignPost.dwChords != 0 )))
		{
			hr = pStream->Write( &iSignPost, sizeof( DMUS_IO_SIGNPOST ), &cb );
			if( FAILED( hr ) || cb != sizeof( DMUS_IO_SIGNPOST ))
			{
				return E_FAIL;
			}
		}
	}
	return S_OK;
}

HRESULT CSignPostMgr::LoadSignPostList(IStream *pStream, long lDropPos, BOOL bDrop, BOOL &fChanged)
{
	HRESULT 		hr = S_OK;

	ASSERT( m_pTimeline );
	if( !m_pTimeline )
	{
		return E_UNEXPECTED;
	}

	ASSERT( pStream );
	if( !pStream )
	{
		return E_POINTER;
	}

	hr = VerifyMeasureArray();
	if (FAILED(hr))
	{
		return hr;
	}

	DWORD			cSize;	// Size of stream
	DWORD			cb; 	// Byte count when reading in data
	long			lDragMeasure = 0;	// measure at which mouse grabbed dragged data
	if( bDrop )
	{
		// Get the size of the stream.
		STATSTG statstg;
		hr = pStream->Stat( &statstg, STATFLAG_NONAME );
		if( FAILED( hr ))
		{
			return hr;
		}

		// Set the initial size
		cSize = statstg.cbSize.LowPart;

		// load drag pt
		hr = pStream->Read(&lDragMeasure, sizeof( long), &cb);
		if( FAILED( hr ) || cb != sizeof( long ) )
		{
			return E_FAIL;
		}

		// Subtract off what we just read
		cSize -= cb;
	}
	else
	{
		// Try and allocate a RIFF Stream
		IDMUSProdRIFFStream* pIRIFFStream;
		if( FAILED( hr = AllocRIFFStream( pStream, &pIRIFFStream ) ) )
		{
			return hr;
		}

		// Descend into the next chunk
		MMCKINFO ck;
		hr = pIRIFFStream->Descend( &ck, NULL, 0 );
		pIRIFFStream->Release();

		// Verify that the descend succeeded, and that we found a SignPost chunk
		if (hr != S_OK || ck.ckid != DMUS_FOURCC_SIGNPOST_TRACK_CHUNK)
		{
			return E_FAIL;
		}

		// Set the initial size
		cSize = ck.cksize;
	}

	// load size of signpost structure
	DWORD dwSizeSignPost;
	hr = pStream->Read( &dwSizeSignPost, sizeof( dwSizeSignPost ), &cb );
	if( FAILED( hr ) || cb != sizeof( dwSizeSignPost ) )
	{
		return E_FAIL;
	}

	cSize -= cb;

	DMUS_IO_SIGNPOST iSignPost;
	LARGE_INTEGER liStreamPos;

	// Save our current position
	DWORD cSizeOrig = cSize;	// Size of stream
	DWORD cCurPos = StreamTell( pStream );

	long lLastMeasure = LONG_MIN;
	while( cSize >= dwSizeSignPost )
	{
		// Read the signpost struct.
		if( dwSizeSignPost > sizeof( DMUS_IO_SIGNPOST ) )
		{
			hr = pStream->Read( &iSignPost, sizeof( DMUS_IO_SIGNPOST ), &cb );
			if( FAILED( hr ) || cb != sizeof( DMUS_IO_SIGNPOST ) )
			{
				return E_FAIL;
			}
			// Seek past the extra data.
			liStreamPos.QuadPart = dwSizeSignPost - sizeof( DMUS_IO_SIGNPOST );
			hr = pStream->Seek( liStreamPos, STREAM_SEEK_CUR, NULL );
			if( FAILED( hr ) )
			{
				return E_FAIL;
			}
		}
		else
		{
			hr = pStream->Read( &iSignPost, dwSizeSignPost, &cb );
			if( FAILED( hr ) || cb != dwSizeSignPost )
			{
				return E_FAIL;
			}
		}

		// If this measure is greater than any previous ones, save it
		if( iSignPost.wMeasure > lLastMeasure )
		{
			lLastMeasure = iSignPost.wMeasure;
		}

		cSize -= dwSizeSignPost;
	}

	// Restore our position back to the start of the signposts
	cSize = cSizeOrig;
	StreamSeek( pStream, cCurPos, STREAM_SEEK_SET );

	BOOL bFirstTime = TRUE;
	long lAdjustMeasure = 0;
	long lMeasure;
	while( cSize >= dwSizeSignPost )
	{
		// Read the signpost struct.
		if( dwSizeSignPost > sizeof( DMUS_IO_SIGNPOST ) )
		{
			hr = pStream->Read( &iSignPost, sizeof( DMUS_IO_SIGNPOST ), &cb );
			if( FAILED( hr ) || cb != sizeof( DMUS_IO_SIGNPOST ) )
			{
				return E_FAIL;
			}
			// Seek past the extra data.
			liStreamPos.QuadPart = dwSizeSignPost - sizeof( DMUS_IO_SIGNPOST );
			hr = pStream->Seek( liStreamPos, STREAM_SEEK_CUR, NULL );
			if( FAILED( hr ) )
			{
				return E_FAIL;
			}
		}
		else
		{
			hr = pStream->Read( &iSignPost, dwSizeSignPost, &cb );
			if( FAILED( hr ) || cb != dwSizeSignPost )
			{
				return E_FAIL;
			}
		}

		// If this is a paste, and we are looking at the first signpost, determine the amount
		// of time the lTime field of each of the signposts being inserted should be shifted.
		if( bFirstTime )
		{
			if( !bDrop )
			{
				if( lDropPos == -1 )
				{
					// In Load() - no adjustment necessary
					lAdjustMeasure = 0;
				}
				else
				{
					// Use position of cursor
					long lTime;
					m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime);
					m_pTimeline->ClocksToMeasureBeat(m_dwGroupBits, 0, lTime, &lMeasure, NULL);

					// Adjust the commands so the first one starts in lMeasure
					lAdjustMeasure = lMeasure - iSignPost.wMeasure;
				}
			}
			else
			{
				// If drop position is less than or equal to 0, drop in measure 0
				if(lDropPos <= 0)
				{
					lMeasure = 0;
				}
				else
				{
					m_pTimeline->PositionToMeasureBeat(m_dwGroupBits, 0, lDropPos, &lMeasure, NULL );
				}

				// Adjustment is drop measure - drag measure
				lAdjustMeasure = lMeasure - lDragMeasure;
			}

			// Ensure that the last signpost starts before the end of the segment
			if( lAdjustMeasure + lLastMeasure >= m_nMeasures )
			{
				lAdjustMeasure = m_nMeasures - lLastMeasure - 1;
			}

			// Ensure that the first signpost starts at or after measure 0
			if(  lAdjustMeasure < -iSignPost.wMeasure )
			{
				lAdjustMeasure = -iSignPost.wMeasure;
			}
			bFirstTime = FALSE;
			ClearSelected();
		}

		// Adjust the time of the signpost from the drag and drop measures
		lMeasure = iSignPost.wMeasure + lAdjustMeasure;

		// Should be at least 0
		ASSERT( lMeasure >= 0 );
		if(lMeasure < 0)
		{
			lMeasure = 0;
		}

		// convert measure to a time
		m_pTimeline->MeasureBeatToClocks(m_dwGroupBits, 0, lMeasure, 0, &iSignPost.mtTime);

		cSize -= dwSizeSignPost;

		if( lMeasure >= (long) m_nMeasures )
		{
			break;
		}

		if( iSignPost.dwChords == 0)
		{
			if( bDrop && (m_pMeasureArray[lMeasure].pSignPost != NULL) )
			{
				delete m_pMeasureArray[lMeasure].pSignPost;
				m_pMeasureArray[lMeasure].pSignPost = NULL;
				fChanged = TRUE;
			}
		}
		else
		{
			if( m_pMeasureArray[lMeasure].pSignPost == NULL )
			{
				m_pMeasureArray[lMeasure].pSignPost = new SignPostExt;
				if( m_pMeasureArray[lMeasure].pSignPost == NULL )
				{
					return E_OUTOFMEMORY;
				}
			}
			m_pMeasureArray[lMeasure].pSignPost->lTime	= iSignPost.mtTime;
			m_pMeasureArray[lMeasure].pSignPost->dwSignPost = iSignPost.dwChords;
			UnMarkAtMeasure(lMeasure, DRAG_SELECT);
			fChanged = TRUE;
		}

		m_pMeasureArray[lMeasure].dwFlags |= SPMI_SELECTED;
		m_bSelected = TRUE;
	} // while( cSize > 0 )

	// Update the "next" pointers
	SignPostExt *pSPE = NULL;
	m_pSignPostList = NULL;
	for( short n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pSignPost != NULL )
		{
			if( m_pSignPostList == NULL )
			{
				m_pSignPostList = m_pMeasureArray[n].pSignPost;
			}
			else
			{
				pSPE->pNext = m_pMeasureArray[n].pSignPost;
			}
			pSPE = m_pMeasureArray[n].pSignPost;
			pSPE->pNext = NULL;
		}
	}

	// Update the property page and the object we represent.
	if( m_pIPageManager != NULL )
	{
		m_pIPageManager->RefreshData();
	}

	return S_OK;
}

void CSignPostMgr::UnMarkAtMeasure(DWORD measure, WORD flags)
{
	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	if(m_pMeasureArray[measure].pSignPost != NULL)
	{
		m_pMeasureArray[measure].pSignPost->dwDragDrop &= ~flags;
	}

}

HRESULT CSignPostMgr::ResizeMeasureArray()
{
	HRESULT hr = S_OK;
	VARIANT var;
	long	lMeasureLength;
	long	lTimeLength;
	bool	bSmallerNotLarger;
	hr = m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	if(hr != S_OK)
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}
	lTimeLength = V_I4( &var );
	hr = m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lTimeLength, &lMeasureLength, NULL );
	if(hr != S_OK)
	{
		ASSERT(FALSE);
		return E_UNEXPECTED;
	}
	// Ensure lMeasureLength is at least 1
	lMeasureLength = max( 1, lMeasureLength );
	if( m_nMeasures != (short) lMeasureLength )
	{
		bSmallerNotLarger = (short)lMeasureLength < m_nMeasures;
		SignPostMeasureInfo* pMeasureArray = new SignPostMeasureInfo[lMeasureLength];
		if( m_pMeasureArray == NULL)
		{
			return E_OUTOFMEMORY;
		}
		// copy from the array.
		short nlen = m_nMeasures <= (short)lMeasureLength ? m_nMeasures : (short)lMeasureLength;
		short n = 0;
		for(n = 0; n < nlen; n++)
		{
			pMeasureArray[n].dwFlags = m_pMeasureArray[n].dwFlags;
			pMeasureArray[n].pSignPost = m_pMeasureArray[n].pSignPost;
		}
		// initialize any remaining new measures 
		// (if lMeasure < m_pMeasureArray, then this for loop never entered)
		for( n = nlen; n < (short)lMeasureLength; n++ )
		{
			pMeasureArray[n].dwFlags = 0;
			pMeasureArray[n].pSignPost = NULL;
		}

		m_nMeasures = (short) lMeasureLength;
		delete [] m_pMeasureArray;
		m_pMeasureArray = pMeasureArray;
		if( m_lShiftFromMeasure >= m_nMeasures )
		{
			m_lShiftFromMeasure = m_nMeasures - 1;
		}
		if(hr == S_OK)
		{
			if(bSmallerNotLarger)
			{
				SyncMeasureArray();
			}
			if( m_pSignPostStrip->m_fInAllTracksAdded == FALSE )
			{
				m_pSignPostStrip->m_nLastEdit = IDS_UNDO_CHANGE;
				UpdateSegment();
			}
		}
	}

	return hr;
}

void CSignPostMgr::SyncMeasureArray()
{
	SignPostExt	*pSPE;
	RemoveOrphanSignposts();
	// Update the "next" pointers
	m_pSignPostList = NULL;
	pSPE = NULL;
	for(short n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].pSignPost != NULL)
		{
			if(m_pSignPostList == NULL)
			{
				m_pSignPostList = m_pMeasureArray[n].pSignPost;
			}
			else
			{
				pSPE->pNext = m_pMeasureArray[n].pSignPost;
			}
			pSPE = m_pMeasureArray[n].pSignPost;
			pSPE->pNext = NULL;
		}
	}
}

void CSignPostMgr::RemoveOrphanSignposts()
// removes signposts that don't have a corresponding measure
// for purposes of this function, signposts with corresponding measures
// are said to be "legal".
{
	SignPostExt* pExt = NULL;
	for(short n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].pSignPost)
		{
			SignPostExt* p = new SignPostExt;
			memcpy(p, m_pMeasureArray[n].pSignPost, sizeof(SignPostExt));
			p->pNext = 0;
			m_pMeasureArray[n].pSignPost = p;
			if(pExt)
			{
				pExt->pNext = p;
			}
			else
			{
				pExt = p;
			}
		}
	}

	SignPostExt* p = m_pSignPostList;
	while(p)
	{
		SignPostExt* q = p->pNext;
		delete p;
		p = q;
	}
	m_pSignPostList = pExt;
}

void CSignPostMgr::RecalculateMusicTimeValues()
{
	SignPostExt	*pSPE;
	RemoveOrphanSignposts();
	// Update the "next" pointers
	m_pSignPostList = NULL;
	pSPE = NULL;
	for(short n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].pSignPost != NULL)
		{
			long lTemp;
			m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, n, 0, &lTemp);

			m_pMeasureArray[n].pSignPost->lTime = lTemp;

			if(m_pSignPostList == NULL)
			{
				m_pSignPostList = m_pMeasureArray[n].pSignPost;
			}
			else
			{
				pSPE->pNext = m_pMeasureArray[n].pSignPost;
			}
			pSPE = m_pMeasureArray[n].pSignPost;
			pSPE->pNext = NULL;
		}
	}
}

/*
HRESULT	CSignPostMgr::Compose( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (m_pISegmentNode == NULL) || (m_pIFramework == NULL) )
	{
		return E_UNEXPECTED;
	}

	IDirectMusicSegment *pIDirectMusicSegment;
	if( FAILED( m_pISegmentNode->GetObject( CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pIDirectMusicSegment ) ) )
	{
		return E_UNEXPECTED;
	}

	IDirectMusicPerformance *pIDirectMusicPerformance = NULL;
	IDMUSProdComponent* pIComponent;
	if( SUCCEEDED ( m_pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
	{
		IDMUSProdConductor *pIDMUSProdConductor;
		if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pIDMUSProdConductor ) ) )
		{
			IUnknown* punk;
			if( SUCCEEDED( m_pIConductor->GetPerformanceEngine( &punk ) ) )
			{
				punk->QueryInterface( IID_IDirectMusicPerformance, (void **)&pIDirectMusicPerformance );
				punk->Release();
			}
			pIDMUSProdConductor->Release();
		}
		pIComponent->Release();
	}

	if( pIDirectMusicPerformance == NULL) )
	{
		pIDirectMusicSegment->Release();
		return E_UNEXPECTED;
	}

	// Illegal to compose chords while a segment is playing.
	if( pIDirectMusicPerformance->IsPlaying( pIDirectMusicSegment, NULL ) == S_OK )
	{
		pIDirectMusicSegment->Release();
		pIDirectMusicPerformance->Release();
		return E_ABORT;
	}

	IDirectMusicComposer8 *pIDMComposer;
	::CoCreateInstance( CLSID_DirectMusicComposer, NULL, CLSCTX_INPROC, 
					   IID_IDirectMusicComposer8, (void**)&pIDMComposer );
	if( !pIDMComposer )
	{
		pIDirectMusicSegment->Release();
		pIDirectMusicPerformance->Release();
		return E_NOINTERFACE;
	}

	IDirectMusicStyle *pIDMStyle = NULL;
	IDirectMusicChordMap *pIDMChordMap = NULL;

	HRESULT hr = S_OK;
	IDirectMusicSegment* pIDMComposedSegment = NULL;
	IDirectMusicTrack* pIDMTrack = NULL;
	IDirectMusicTrack* pIDMComposedChordTrack = NULL;
	CTrack* pTrack = NULL;
	IPersistStream* pIPersistStream = NULL;
	IStream* pIStream = NULL;
	IDMUSProdStripMgr* pIStripMgr = NULL;
	DWORD dwChordTrackGroup = dwGroupBits;

	// Remove the existing ChordTrack, if any
	if( SUCCEEDED( pIDirectMusicSegment->GetTrack( CLSID_DirectMusicChordTrack, dwGroupBits, 0, &pIDMTrack ) ) )
	{
		pIDirectMusicSegment->GetTrackGroup( pIDMTrack, &dwChordTrackGroup );
		pIDirectMusicSegment->RemoveTrack( pIDMTrack );
	}

	// Compose a segment with a new Chord Track
	DWORD dwComposerFlags = DMUS_COMPOSE_TEMPLATEF_CLONE;
	if( m_wLegacyActivityLevel < 4 )
	{
		dwComposerFlags = DMUS_COMPOSE_TEMPLATEF_CLONE | DMUS_COMPOSE_TEMPLATEF_ACTIVITY;
	}
	hr = pIDMComposer->ComposeSegmentFromTemplateEx( pIDMStyle, pIDirectMusicSegment, dwComposerFlags, m_wLegacyActivityLevel, pIDMChordMap, &pIDMComposedSegment );
	if( FAILED( hr ) )
	{
		if( pIDMTrack )
		{
			// Add the chord track back.
			pIDirectMusicSegment->InsertTrack( pIDMTrack, dwChordTrackGroup );
			UpdateTrackConfig( FindCTrackFromDMTrack( pIDMTrack ) );
		}

		//hr = E_FAIL;
		goto ON_ERROR;
	}

	BOOL fSaved = FALSE;

	if( pIDMTrack )
	{
		// Old chord track exists, update the Chord strip with the new chord track

		// Find the new Chord Track
		if( FAILED( pIDMComposedSegment->GetTrack( CLSID_DirectMusicChordTrack, dwGroupBits, 0, &pIDMComposedChordTrack) ) )
		{
			TRACE("Whoops, can't find a Chord track in a composed segment.\n");
			// Add the chord track back.
			pIDirectMusicSegment->InsertTrack( pIDMTrack, dwChordTrackGroup );
			UpdateTrackConfig( FindCTrackFromDMTrack( pIDMTrack ) );
			hr = S_FALSE;
			goto ON_ERROR;
		}

		// Find the old ChordStrip
		CTrack *pTmpTrack;
		pTmpTrack = NULL;
		POSITION position;
		position = m_lstTracks.GetHeadPosition();
		while(position != NULL)
		{
			pTmpTrack = m_lstTracks.GetNext(position);
			ASSERT( pTmpTrack );
			IDirectMusicTrack* pIDMTmpTrack;
			pTmpTrack->GetDMTrack( &pIDMTmpTrack );
			if( pIDMTmpTrack == pIDMTrack )
			{
				pTmpTrack->SetDMTrack( pIDMComposedChordTrack );
				pIDMTmpTrack->Release();
				pIDMTmpTrack = NULL;

				if( FAILED( pIDirectMusicSegment->InsertTrack( pIDMComposedChordTrack, dwChordTrackGroup ) ) )
				{
					hr = E_UNEXPECTED;
					goto ON_ERROR;
				}
				pTmpTrack->GetStripMgr( &pIStripMgr );
				if( pIStripMgr == NULL )
				{
					hr = E_UNEXPECTED;
					goto ON_ERROR;
				}

				VARIANT varDMTrack;
				varDMTrack.vt = VT_UNKNOWN;
				V_UNKNOWN( &varDMTrack ) = pIDMComposedChordTrack;
				hr = pIStripMgr->SetStripMgrProperty( SMP_IDIRECTMUSICTRACK, varDMTrack );
				if( FAILED( hr ) )
				{
					hr = E_UNEXPECTED;
					goto ON_ERROR;
				}
				if( FAILED( pIStripMgr->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
				{
					hr = E_UNEXPECTED;
					goto ON_ERROR;
				}

				// This causes the ChordStrip to load chords from its attached IDirectMusicTrack.
				if( FAILED( pIPersistStream->Load( NULL ) ) )
				{
					hr = E_UNEXPECTED;
					goto ON_ERROR;
				}

				if( FAILED( m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIStream ) ) )
				{
					hr = E_UNEXPECTED;
					goto ON_ERROR;
				}
				if( FAILED( pIPersistStream->Save( pIStream, TRUE ) ) )
				{
					hr = E_UNEXPECTED;
					goto ON_ERROR;
				}
				UpdateSavedState(fSaved, IDS_UNDO_COMPOSE);
				pTmpTrack->SetStream( pIStream );
			}
			if( pIDMTmpTrack )
			{
				pIDMTmpTrack->Release();
				pIDMTmpTrack = NULL;
			}
		}
	}
	else
	{
		// Created a new Chord Track, add it to the display and our internal list of tracks.
		if( FAILED( pIDMComposedSegment->GetTrack( CLSID_DirectMusicChordTrack, dwGroupBits, 0, &pIDMComposedChordTrack) ) )
		{
			TRACE("Whoops, can't find a Chord track in a composed segment.\n");
			hr = S_FALSE;
			goto ON_ERROR;
		}

		DWORD dwNewChordTrackGroupBits = dwGroupBits;
		if( FAILED( pIDMComposedSegment->GetTrackGroup( pIDMComposedChordTrack, &dwNewChordTrackGroupBits) ) )
		{
			TRACE("CSegment: Whoops, can't find group bits of the compose chord track.\n");
		}

		pTrack = new CTrack();
		if( pTrack == NULL )
		{
			hr = E_UNEXPECTED;
			goto ON_ERROR;
		}

		pTrack->m_dwGroupBits = dwNewChordTrackGroupBits;
		memcpy( &pTrack->m_guidClassID, &CLSID_DirectMusicChordTrack, sizeof( GUID ) );
		if( FAILED( GUIDToStripMgr( pTrack->m_guidClassID, &pTrack->m_guidEditorID ) ) )
		{
			TRACE("Segment::AddTrack: Unable to find Strip Editor for Track's CLSID.\n");
			ASSERT(FALSE); // This shouldn't happen, since GUIDToStripMgr should default
			// to the UnknownStripMgr if it can't find a match in the registry.
			hr = E_UNEXPECTED;
			goto ON_ERROR;
		}

		BOOL fUnknownStripMgr = FALSE;
		hr = ::CoCreateInstance( pTrack->m_guidEditorID, NULL, CLSCTX_INPROC,
								 IID_IDMUSProdStripMgr, (void**)&pIStripMgr );
		if( FAILED( hr ) )
		{
			TRACE("Segment: Unable to CoCreate an IDMUSProdStripMgr - going to try the UnknownStripMgr\n");
			memcpy( &pTrack->m_guidEditorID, &CLSID_UnknownStripMgr, sizeof(GUID) );
			hr = ::CoCreateInstance( pTrack->m_guidEditorID, NULL, CLSCTX_INPROC,
									 IID_IDMUSProdStripMgr, (void**)&pIStripMgr );
			if( FAILED( hr ) )
			{
				TRACE("Segment: Unable to CoCreate an UnknownStripMgr\n");
				hr = E_UNEXPECTED;
				goto ON_ERROR;
			}
			fUnknownStripMgr = TRUE;
		}

		pTrack->SetStripMgr( pIStripMgr );
		pTrack->SetDMTrack( pIDMComposedChordTrack );
		if( FAILED( pIDirectMusicSegment->InsertTrack( pIDMComposedChordTrack, dwNewChordTrackGroupBits ) ) )
		{
			hr = E_UNEXPECTED;
			goto ON_ERROR;
		}

		VARIANT varDMTrack;
		varDMTrack.vt = VT_UNKNOWN;
		V_UNKNOWN( &varDMTrack ) = pIDMComposedChordTrack;
		if( FAILED( pIStripMgr->SetStripMgrProperty( SMP_IDIRECTMUSICTRACK, varDMTrack ) ) )
		{
			hr = E_UNEXPECTED;
			goto ON_ERROR;
		}

		varDMTrack.vt = VT_UNKNOWN;
		V_UNKNOWN( &varDMTrack ) = m_pIFramework;
		if( FAILED( pIStripMgr->SetStripMgrProperty( SMP_IDMUSPRODFRAMEWORK, varDMTrack ) ) )
		{
			hr = E_UNEXPECTED;
			goto ON_ERROR;
		}

		// Tell the strip manager what its group bits are
		DMUS_IO_TRACK_HEADER ioTrackHeader;
		memcpy( &ioTrackHeader.guidClassID, &pTrack->m_guidClassID, sizeof(GUID) );
		ioTrackHeader.dwPosition = pTrack->m_dwPosition;
		ioTrackHeader.dwGroup = pTrack->m_dwGroupBits;
		ioTrackHeader.ckid = pTrack->m_ckid;
		ioTrackHeader.fccType = pTrack->m_fccType;
		varDMTrack.vt = VT_BYREF;
		V_BYREF(&varDMTrack) = &ioTrackHeader;

		if( FAILED( pIStripMgr->SetStripMgrProperty( SMP_DMUSIOTRACKHEADER, varDMTrack ) ) )
		{
			hr = E_UNEXPECTED;
			goto ON_ERROR;
		}


		// The unknown stripmgr doesn't have a way to get the data out of the Chord Track
		if( !fUnknownStripMgr )
		{
			if( FAILED( pIStripMgr->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
			{
				hr = E_UNEXPECTED;
				goto ON_ERROR;
			}

			// This causes the ChordStrip to load chords from its attached IDirectMusicTrack.
			if( FAILED( pIPersistStream->Load( NULL ) ) )
			{
				hr = E_UNEXPECTED;
				goto ON_ERROR;
			}

			if( FAILED( m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIStream ) ) )
			{
				hr = E_UNEXPECTED;
				goto ON_ERROR;
			}
			if( FAILED( pIPersistStream->Save( pIStream, TRUE ) ) )
			{
				hr = E_UNEXPECTED;
				goto ON_ERROR;
			}
			UpdateSavedState(fSaved, IDS_UNDO_COMPOSE);
			pTrack->SetStream( pIStream );
		}

		// Add the track to our list and the dialog (if it exists)
		InsertTrackAtDefaultPos( pTrack );
		//m_lstTracks.AddTail( pTrack );
		if(m_pSegmentDlg != NULL)
		{
			m_pSegmentDlg->AddTrack( pTrack );
		}
	}

ON_ERROR:
	if( FAILED( hr ) )
	{
		if( pTrack )
		{
			delete pTrack;
		}
	}
	if( pIStripMgr )
	{
		pIStripMgr->Release();
	}
	if( pIDMComposedSegment )
	{
		pIDMComposedSegment->Release();
	}
	if( pIDMTrack )
	{
		pIDMTrack->Release();
	}
	if( pIDMComposedChordTrack )
	{
		pIDMComposedChordTrack->Release();
	}
	if( pIPersistStream )
	{
		pIPersistStream->Release();
	}
	if( pIStream )
	{
		pIStream->Release();
	}
	if( pIDMChordMap )
	{
		pIDMChordMap->Release();
	}
	if( pIDMStyle )
	{
		pIDMStyle->Release();
	}
	pIDMComposer->Release();
	return hr;
}
*/