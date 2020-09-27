// CommandMgr.cpp : Implementation of CCommandMgr
#include "stdafx.h"
#include "CommandStripMgr.h"
#include "CommandMgr.h"
#include "FileIO.h"
#include <dmusici.h>
#include <dmusicf.h>
#include "RiffStrm.h"
#include "SegmentGuids.h"
#include "SegmentIO.h"

/////////////////////////////////////////////////////////////////////////////
// CCommandMgr
CCommandMgr::CCommandMgr()
{
	m_pTimeline				= NULL;
	m_pCommandList			= NULL;
	m_pMeasureArray			= NULL;
	m_pIPageManager			= NULL;
	m_pDragDataObject		= NULL;
	m_pCopyDataObject		= NULL;
	m_bSelected				= FALSE;
	ZeroMemory( &m_PPGCommand, sizeof( PPGCommand ) );
	m_nMeasures				= 0;
	m_lShiftFromMeasure		= 0;
	m_pCommandStrip			= 0;
	m_dwGroupBits			= 1;
	m_dwOldGroupBits		= 1;
	m_dwTrackExtrasFlags	= DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags	= 0;
	m_pIDMTrack				= NULL;
	m_pTempIStream			= NULL;
}

CCommandMgr::~CCommandMgr()
{
	ClearCommandList( m_pCommandList );

	if( m_pMeasureArray != NULL )
	{
		delete [] m_pMeasureArray;
	}

	if( m_pIPageManager != NULL )
	{
		m_pIPageManager->Release();
		m_pIPageManager = NULL;
	}
	if( m_pTempIStream )
	{
		m_pTempIStream->Release();
	}
	if( m_pIDMTrack )
	{
		m_pIDMTrack->Release();
	}
	if( m_pDragDataObject )
	{
		m_pDragDataObject->Release();
		m_pDragDataObject = NULL;
	}
	if( m_pCopyDataObject )
	{
		if( S_OK == OleIsCurrentClipboard( m_pCopyDataObject ))
		{
			OleFlushClipboard();
		}
		m_pCopyDataObject->Release();
		m_pCopyDataObject = NULL;
	}
	if( m_pCommandStrip )
	{
		m_pCommandStrip->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCommandMgr IDMUSProdStripMgr

HRESULT STDMETHODCALLTYPE CCommandMgr::GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData)
{
	UNREFERENCED_PARAMETER(mtTime);
	UNREFERENCED_PARAMETER(pmtNext);
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if(::IsEqualGUID(guidType, GUID_Segment_Undo_BSTR))
	{
		BSTR bstr;
		CString str;
		str.LoadString(m_pCommandStrip->m_nLastEdit);
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

HRESULT STDMETHODCALLTYPE CCommandMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	UNREFERENCED_PARAMETER(guidType);
	UNREFERENCED_PARAMETER(mtTime);
	UNREFERENCED_PARAMETER(pData);
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CCommandMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if(::IsEqualGUID(guidType, GUID_Segment_Undo_BSTR))
	{
		return S_OK;
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CCommandMgr::OnUpdate(
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
			m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)m_pCommandStrip, NULL, TRUE);
		}
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
		m_pCommandStrip->m_fInAllTracksAdded = TRUE;
		ResizeMeasureArray();
		m_pCommandStrip->m_fInAllTracksAdded = FALSE;
		// Load stream (deferred load)
		if( m_pTempIStream )
		{
			//seek to beginning
			LARGE_INTEGER	liTemp;
			liTemp.QuadPart = 0;
			m_pTempIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);	//seek to beginning
			Load( m_pTempIStream );
			// After we've initialized ourself, we should never need to re-initialize
			m_pTempIStream->Release();
			m_pTempIStream = NULL;
		}
		// Sync with DirectMusic
		if( m_pIDMTrack )
		{
			SyncWithDirectMusic();
		}
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CCommandMgr::GetStripMgrProperty(
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

		/* Unsupported
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
		*/

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

			pioTrackHeader->guidClassID = CLSID_DirectMusicCommandTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = DMUS_FOURCC_COMMANDTRACK_CHUNK;
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

HRESULT STDMETHODCALLTYPE CCommandMgr::SetStripMgrProperty(
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
		if(m_pTimeline)
		{
			m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject *)this);
			if( m_pCommandStrip )
			{
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject *)m_pCommandStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pCommandStrip );
				m_pCommandStrip->Release();
				m_pCommandStrip = NULL;
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_Length_Change, m_dwOldGroupBits);
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
				if ( m_pCommandStrip )
				{
					return E_FAIL;
				}

				// Create a strip and add it to the timeline
				m_pCommandStrip = new CCommandStrip(this);
				if( !m_pCommandStrip )
				{
					return E_OUTOFMEMORY;
				}

				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pCommandStrip, CLSID_DirectMusicCommandTrack, m_dwGroupBits, 0 );
				m_pTimeline->AddToNotifyList((IDMUSProdStripMgr *)this, GUID_Segment_Length_Change, m_dwGroupBits);
				m_pTimeline->AddToNotifyList((IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits);
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

		/* Not implemented
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
		*/

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

			m_dwOldGroupBits = m_dwGroupBits;
			m_dwGroupBits = pioTrackHeader->dwGroup;
			if(m_pCommandStrip)
				m_pCommandStrip->m_nLastEdit = IDS_UNDO_TRACKGROUP;
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
// CCommandMgr IPersist

HRESULT CCommandMgr::GetClassID( CLSID* pClsId )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pClsId);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CCommandMgr IPersistStream functions

HRESULT CCommandMgr::IsDirty()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	// BUGBUG: Should implement
	return S_FALSE;
}

HRESULT CCommandMgr::Load( IStream* pIStream )
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
					TRACE("Segment: Unable to copy to memory stream\n");
					m_pTempIStream->Release();
					return E_FAIL;
				}
			}
			else
			{
				TRACE("Segment: Unable to stat stream\n");
				m_pTempIStream->Release();
				return E_FAIL;
			}
		}
		else
		{
			TRACE("Segment: Unable to create memory stream\n");
			return E_FAIL;
		}
	}

	VARIANT var;
	long	lMeasureLength;
	long	lTimeLength;

	// Clear the list in case the clocks per measure has changed.
	ClearCommandList( m_pCommandList, FALSE );
	m_pCommandList = NULL;
	// Get the length of the timeline and make sure that m_nMeasures and m_lShiftFromMeasure
	// are accurate.

	/*
	// Don't call load until the strip has been added to the timeline
	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}
	*/
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

		m_pMeasureArray = new CommandMeasureInfo[lMeasureLength];
		if( m_pMeasureArray == NULL)
		{
			return E_OUTOFMEMORY;
		}
		m_nMeasures = (short) lMeasureLength;
		// Initialize the array.
		for( short n = 0; n < m_nMeasures; n++ )
		{
			m_pMeasureArray[n].dwFlags = 0;
			m_pMeasureArray[n].pCommand = NULL;
		}

		if( m_lShiftFromMeasure >= m_nMeasures )
		{
			m_lShiftFromMeasure = m_nMeasures - 1;
		}
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Check for Direct Music format
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	MMCKINFO	ck;
	DWORD		dwByteCount;

	// Load the Track
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case mmioFOURCC('c','m','n','d'):
			{
				DWORD dwCommandSize;

				hr = pIStream->Read( &dwCommandSize, sizeof(DWORD), &dwByteCount );
				if(FAILED(hr) || dwByteCount != sizeof(DWORD))
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Compute amount to skip between each command
				DWORD dwSkip = max( 0, dwCommandSize - sizeof( DMUS_IO_COMMAND ) );

				// Compute amount of each command to read
				dwCommandSize = min( dwCommandSize, sizeof( DMUS_IO_COMMAND ) );

				long lLength;
				lLength = ck.cksize - sizeof(DWORD);

				while( lLength > 0 )
				{
					DMUS_IO_COMMAND iCommand;

					ZeroMemory( &iCommand, sizeof( DMUS_IO_COMMAND ) );
					hr = pIStream->Read( &iCommand, dwCommandSize, &dwByteCount );
					if(FAILED(hr) || dwByteCount != dwCommandSize)
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
					lLength -= dwCommandSize;

					if(iCommand.wMeasure >= m_nMeasures)
					{
						break;
					}

					// If there is data to skip, skip it
					if( dwSkip )
					{
						hr = StreamSeek( pIStream, dwSkip, SEEK_CUR );
						if( FAILED(hr) )
						{
							goto ON_ERROR;
						}
					}

					ASSERT( m_pMeasureArray[iCommand.wMeasure].pCommand == NULL );
					m_pMeasureArray[iCommand.wMeasure].pCommand = new CommandExt;
					if( m_pMeasureArray[iCommand.wMeasure].pCommand == NULL )
					{
						hr = E_OUTOFMEMORY;
						goto ON_ERROR;
					}
					m_pMeasureArray[iCommand.wMeasure].pCommand->lTime		= iCommand.mtTime;
					m_pMeasureArray[iCommand.wMeasure].pCommand->bCommand	= iCommand.bCommand;
					m_pMeasureArray[iCommand.wMeasure].pCommand->bGrooveLevel = iCommand.bGrooveLevel;
					if( iCommand.bGrooveRange > 100 )
					{
						m_pMeasureArray[iCommand.wMeasure].pCommand->bGrooveRange = 0;
					}
					else
					{
						m_pMeasureArray[iCommand.wMeasure].pCommand->bGrooveRange = iCommand.bGrooveRange;
					}
					if( iCommand.bRepeatMode > DMUS_PATTERNT_RANDOM_ROW )
					{
						m_pMeasureArray[iCommand.wMeasure].pCommand->bRepeatMode = 0;
					}
					else
					{
						m_pMeasureArray[iCommand.wMeasure].pCommand->bRepeatMode = iCommand.bRepeatMode;
					}
				}
				break;
			}
		}
		pIRiffStream->Ascend( &ck, 0 );
	}

	// Update the "next" pointers
	m_pCommandList = NULL;
	CommandExt*	pCE;
	pCE = NULL;
	int n;
	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pCommand != NULL )
		{
			if( m_pCommandList == NULL )
			{
				m_pCommandList = m_pMeasureArray[n].pCommand;
			}
			else
			{
				pCE->pNext = m_pMeasureArray[n].pCommand;
			}
			pCE = m_pMeasureArray[n].pCommand;
			pCE->pNext = NULL;
		}
	}

	// Update the property page and the object we represent.
	if( m_pIPageManager != NULL )
	{
		m_pIPageManager->RefreshData();
	}

ON_ERROR:
	pIRiffStream->Release();;
	return hr;
}


HRESULT CCommandMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(fClearDirty);
	HRESULT hr;
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	//ASSERT(m_pMeasureArray != NULL );
	hr = VerifyMeasureArray();
	if (FAILED(hr)) return hr;

	if( NULL == pIStream )
	{
		return E_INVALIDARG;
	}

    MMCKINFO ck;
	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	ck.ckid = mmioFOURCC( 'c', 'm', 'n', 'd' );
	hr = pIRiffStream->CreateChunk( &ck, 0 );
    if( hr == S_OK )
    {
		hr = SaveCommandList( pIStream, FALSE );
		// Always ascend.
		pIRiffStream->Ascend( &ck, 0 );
	}
	pIRiffStream->Release();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CCommandMgr::SyncWithDirectMusic

HRESULT CCommandMgr::SyncWithDirectMusic(  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;
	IDMUSProdFramework *pIFramework = NULL;
	VARIANT var;
	m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	if( var.vt == VT_UNKNOWN )
	{
		IUnknown *punk;
		punk = V_UNKNOWN( &var );
		if( punk )
		{
			if( SUCCEEDED( punk->QueryInterface( IID_IDMUSProdFramework, (void**)&pIFramework ) ) )
			{

				// 1. persist the strip to a stream
				IStream* pIMemStream = NULL;
				IPersistStream* pIPersistStream = NULL;
				hr = pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );
				if( SUCCEEDED ( hr ) )
				{
					StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
					hr = Save(pIMemStream, FALSE);
					if (SUCCEEDED(hr))
					{
						hr = m_pIDMTrack->QueryInterface(IID_IPersistStream, (void**)&pIPersistStream);
						if (SUCCEEDED(hr))
						{
							StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
							// 2. Load the stream into the track.
							hr = pIPersistStream->Load(pIMemStream);
							pIPersistStream->Release();
						}
					}
					pIMemStream->Release();
				}
				pIFramework->Release();
			}
			punk->Release();
		}
	}

	return hr;
}


HRESULT CCommandMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pcbSize);
	return E_NOTIMPL;
}


HRESULT CCommandMgr::VerifyMeasureArray()
{
	VARIANT var;
	long	lMeasureLength;
	long	lTimeLength;

	if (m_pMeasureArray == NULL)
	{
		m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		lTimeLength = V_I4( &var );
		m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lTimeLength, &lMeasureLength, NULL );
		// Ensure lMeasureLength is at least 1
		lMeasureLength = max( 1, lMeasureLength );

		m_pMeasureArray = new CommandMeasureInfo[lMeasureLength];
		if( m_pMeasureArray == NULL)
		{
			return E_OUTOFMEMORY;
		}
		m_nMeasures = (short) lMeasureLength;
		// Initialize the array.
		for( short n = 0; n < m_nMeasures; n++ )
		{
			m_pMeasureArray[n].dwFlags = 0;
			m_pMeasureArray[n].pCommand = NULL;
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

HRESULT STDMETHODCALLTYPE CCommandMgr::GetData( /* [retval][out] */ void** ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool fValid = false;
	short n;

	//ASSERT(m_pMeasureArray != NULL );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return hr;

	m_PPGCommand.dwValid = 0;

	if( m_bSelected )
	{
		// Set the fields of the PPGCommand
		for( n = 0; n < m_nMeasures; n++ )
		{
			if( m_pMeasureArray[n].dwFlags & CMI_SELECTED )
			{
				if( m_pMeasureArray[n].pCommand != NULL )
				{
					if( !fValid )
					{
						m_PPGCommand.dwValid	= VALID_COMMAND | VALID_GROOVE | VALID_GROOVERANGE | VALID_REPEATMODE;
						m_PPGCommand.bCommand	= m_pMeasureArray[n].pCommand->bCommand;
						m_PPGCommand.bGrooveLevel	= m_pMeasureArray[n].pCommand->bGrooveLevel;
						m_PPGCommand.bGrooveRange	= m_pMeasureArray[n].pCommand->bGrooveRange;
						m_PPGCommand.bRepeatMode	= m_pMeasureArray[n].pCommand->bRepeatMode;
						fValid = true;
					}
					else
					{
						 if( m_PPGCommand.dwValid & VALID_COMMAND )
						 {
							 if( m_PPGCommand.bCommand != m_pMeasureArray[n].pCommand->bCommand)
							 {
								 m_PPGCommand.dwValid &= ~VALID_COMMAND;
							 }
						 }
						 if( m_PPGCommand.dwValid & VALID_GROOVE )
						 {
							 if( m_PPGCommand.bGrooveLevel != m_pMeasureArray[n].pCommand->bGrooveLevel )
							 {
								 m_PPGCommand.dwValid &= ~VALID_GROOVE;
							 }
						 }
						 if( m_PPGCommand.dwValid & VALID_GROOVERANGE )
						 {
							 if( m_PPGCommand.bGrooveRange != m_pMeasureArray[n].pCommand->bGrooveRange )
							 {
								 m_PPGCommand.dwValid &= ~VALID_GROOVERANGE;
							 }
						 }
						 if( m_PPGCommand.dwValid & VALID_REPEATMODE )
						 {
							 if( m_PPGCommand.bRepeatMode != m_pMeasureArray[n].pCommand->bRepeatMode )
							 {
								 m_PPGCommand.dwValid &= ~VALID_REPEATMODE;
							 }
						 }
						 if( m_PPGCommand.dwValid == 0 )
						 {
							 break;
						 }
					}
				}
				else if( !fValid )
				{
					fValid = TRUE;
					m_PPGCommand.dwValid = VALID_COMMAND | VALID_GROOVE | VALID_GROOVERANGE | VALID_REPEATMODE;
					m_PPGCommand.bCommand = 0;
					m_PPGCommand.bGrooveLevel = 0;
					m_PPGCommand.bGrooveRange = 0;
					m_PPGCommand.bRepeatMode = 0;
				}
				else
				{
					if( (m_PPGCommand.dwValid & VALID_COMMAND)
					&&	(m_PPGCommand.bCommand != 0) )
					{
						m_PPGCommand.dwValid &= ~VALID_COMMAND;
					}
					if( (m_PPGCommand.dwValid & VALID_GROOVE)
					&&	(m_PPGCommand.bGrooveLevel != 0 ) )
					{
						m_PPGCommand.dwValid &= ~VALID_GROOVE;
					}
					if( (m_PPGCommand.dwValid & VALID_GROOVERANGE)
					&&	(m_PPGCommand.bGrooveRange != 0 ) )
					{
						m_PPGCommand.dwValid &= ~VALID_GROOVERANGE;
					}
					if( (m_PPGCommand.dwValid & VALID_REPEATMODE)
					&&	(m_PPGCommand.bRepeatMode != 0 ) )
					{
						m_PPGCommand.dwValid &= ~VALID_REPEATMODE;
					}
					m_PPGCommand.bCommand = 0;
					m_PPGCommand.bGrooveLevel = 0;
					m_PPGCommand.bGrooveRange = 0;
					m_PPGCommand.bRepeatMode = 0;
				}		
			}
		}
		*ppData = &m_PPGCommand;
	}
	else
	{
		*ppData = NULL;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CCommandMgr::SetData( /* [in] */ void* pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CommandExt*	pCE;
	BOOL		bValid = FALSE;
	BYTE		bCommand;
	BYTE		bGrooveLevel;
    BYTE        bGrooveRange;
    BYTE        bRepeatMode;
	short		n;

	m_pCommandStrip->m_nLastEdit = 0;

	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	ASSERT( m_bSelected && m_pMeasureArray != NULL && m_pTimeline != NULL );
	if( !m_bSelected || m_pMeasureArray == NULL || m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	memcpy( &m_PPGCommand, pData, sizeof( PPGCommand ));

	if( (m_PPGCommand.dwValid == ( VALID_COMMAND | VALID_GROOVE | VALID_GROOVERANGE | VALID_REPEATMODE ))
	&&	(m_PPGCommand.bCommand == 0)
	&&	(m_PPGCommand.bGrooveLevel == 0)
	&&	(m_PPGCommand.bGrooveRange == 0)
	&&	(m_PPGCommand.bRepeatMode == 0) )
	{
		// Delete the selected items
		m_pCommandStrip->m_nLastEdit = IDS_DELETE;
		DeleteSelected();

		// Update the segment and DirectMusic track with the new data
		UpdateSegment();

		// Refresh the display
		m_pTimeline->StripInvalidateRect( m_pCommandStrip, NULL, FALSE );

		return S_OK;
	}

	// Apply the changes to the selected measures.
	for( n = 0; n < m_nMeasures; n++ )
	{
 		bool bNewGroove = false;
		if( m_pMeasureArray[n].dwFlags & CMI_SELECTED )
		{
			if( m_pMeasureArray[n].pCommand != NULL )
			{
				bCommand = m_pMeasureArray[n].pCommand->bCommand;
				bGrooveLevel = m_pMeasureArray[n].pCommand->bGrooveLevel;
				bGrooveRange = m_pMeasureArray[n].pCommand->bGrooveRange;
				bRepeatMode = m_pMeasureArray[n].pCommand->bRepeatMode;
				if( (bCommand == 0) && (bGrooveLevel == 0) && (bGrooveRange == 0) && (bRepeatMode == 0) )
				{
					bNewGroove = true;
				}
			}
			else
			{
				bCommand = 0;
				bGrooveLevel = 0;
				bGrooveRange = 0;
				bRepeatMode = 0;
				bNewGroove = true;
			}

			// Get the command for this measure.
			if( m_PPGCommand.dwValid & VALID_COMMAND )
			{
				bCommand =  m_PPGCommand.bCommand;
			}
			if( m_PPGCommand.dwValid & VALID_GROOVE )
			{
				bGrooveLevel = m_PPGCommand.bGrooveLevel;
			}
			if( m_PPGCommand.dwValid & VALID_GROOVERANGE )
			{
				bGrooveRange = m_PPGCommand.bGrooveRange;
			}
			if( m_PPGCommand.dwValid & VALID_REPEATMODE )
			{
				bRepeatMode = m_PPGCommand.bRepeatMode;
			}

			// Update the measure.
			if( (bCommand == 0) && (bGrooveLevel == 0) && (bGrooveRange == 0) && (bRepeatMode == 0)
			&&	(m_pMeasureArray[n].pCommand != NULL) )
			{
				delete m_pMeasureArray[n].pCommand;
				m_pMeasureArray[n].pCommand = NULL;
				m_pCommandStrip->m_nLastEdit = IDS_DELETE;
			}
			else if( (bCommand != 0) || (bGrooveLevel != 0) || (bGrooveRange != 0) || (bRepeatMode != 0) )
			{
				if(bNewGroove)
				{
					m_pCommandStrip->m_nLastEdit = IDS_INSERT;
				}
				else
				{
					m_pCommandStrip->m_nLastEdit = IDS_CHANGE;
				}
				if( m_pMeasureArray[n].pCommand == NULL )
				{
					m_pMeasureArray[n].pCommand = new CommandExt;
					if( m_pMeasureArray[n].pCommand == NULL )
					{
						m_pCommandStrip->m_nLastEdit = NULL;
						return E_OUTOFMEMORY;
					}
					m_pCommandStrip->m_nLastEdit = IDS_INSERT;
				}
				m_pMeasureArray[n].pCommand->bCommand = bCommand;
				m_pMeasureArray[n].pCommand->bGrooveLevel = bGrooveLevel;
				m_pMeasureArray[n].pCommand->bGrooveRange = bGrooveRange;
				m_pMeasureArray[n].pCommand->bRepeatMode = bRepeatMode;
				m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, (long)n, 0, &( m_pMeasureArray[n].pCommand->lTime ));
			}
			bValid = TRUE;
		}
	}

	// Update the "next" pointers
	m_pCommandList = NULL;
	pCE = NULL;
	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pCommand != NULL )
		{
			if( m_pCommandList == NULL )
			{
				m_pCommandList = m_pMeasureArray[n].pCommand;
			}
			else
			{
				pCE->pNext = m_pMeasureArray[n].pCommand;
			}
			pCE = m_pMeasureArray[n].pCommand;
			pCE->pNext = NULL;
		}
	}

	ASSERT( bValid );
	if( !bValid )
	{
		return E_UNEXPECTED;
	}

	m_pTimeline->StripInvalidateRect( m_pCommandStrip, NULL, FALSE );

	// Let the object know about the changes
	UpdateSegment();
	return S_OK;
}

HRESULT CCommandMgr::OnShowProperties(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr = S_OK;
	if( m_pIPageManager == NULL )
	{
		CCommandPPGMgr *pPPM = new CCommandPPGMgr;
		if( pPPM == NULL )
		{
			return E_OUTOFMEMORY;
		}
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pIPageManager );
		m_pIPageManager->Release(); // this releases the 2nd ref, leaving only one
		if( FAILED( hr ))
		{
			return hr;
		}
	}
	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}
	m_pTimeline->SetPropertyPage( m_pIPageManager, (IDMUSProdPropPageObject*)this );
	m_pIPageManager->RefreshData();
	return hr;
}

HRESULT CCommandMgr::OnRemoveFromPageManager(void)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// BUGBUG: Should we do something here?
	return S_OK;
}

HRESULT CCommandMgr::SaveCommandList( IStream* pStream, BOOL bOnlySelected )
{
	return SaveDroppedCommandList( pStream, bOnlySelected, 0 );
	/*
	HRESULT		hr;
    DWORD		dwSizeCommand;
	DWORD		cb;
    DMUS_IO_COMMAND	iCommand;

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
		return E_POINTER;
	}

	// Write the size of the command structure
	dwSizeCommand = sizeof( DMUS_IO_COMMAND );
	hr = pStream->Write( &dwSizeCommand, sizeof( dwSizeCommand ), &cb );
    if( FAILED( hr ) || cb != sizeof( dwSizeCommand ))
	{
		return E_FAIL;
	}

	// Write the commands
	for( short n = 0; n < m_nMeasures; n++ )
	{
		ZeroMemory( &iCommand, sizeof( DMUS_IO_COMMAND ) );
		iCommand.wMeasure = n;
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, iCommand.wMeasure, 0, &iCommand.mtTime );

		if( m_pMeasureArray[n].pCommand != NULL )
		{
			iCommand.bCommand = m_pMeasureArray[n].pCommand->bCommand;
			iCommand.bGrooveLevel = m_pMeasureArray[n].pCommand->bGrooveLevel;
			iCommand.bGrooveRange = m_pMeasureArray[n].pCommand->bGrooveRange;
			iCommand.bRepeatMode = m_pMeasureArray[n].pCommand->bRepeatMode;
			m_pMeasureArray[n].pCommand->lTime = iCommand.mtTime;
		}

		if(( bOnlySelected && (m_pMeasureArray[n].dwFlags & CMI_SELECTED) ) ||
		   ( !bOnlySelected && (iCommand.bCommand != 0 || iCommand.bGrooveLevel != 0 ||
								iCommand.bGrooveRange != 0 || iCommand.bRepeatMode != 0) ))
		{
			hr = pStream->Write( &iCommand, sizeof( DMUS_IO_COMMAND ), &cb );
			if( FAILED( hr ) || cb != sizeof( DMUS_IO_COMMAND ))
			{
				return E_FAIL;
			}
		}
	}
	return S_OK;
	*/
}

HRESULT CCommandMgr::SaveDroppedCommandList( IStream* pStream, BOOL bOnlySelected, short nOffsetMeasure )
{
	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	//ASSERT(m_pMeasureArray != NULL );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return hr;

	if( NULL == pStream )
	{
		return E_POINTER;
	}

	// Convert nOffsetMeasure into a time offset
	long lTimeOffset;
	m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, nOffsetMeasure, 0, &lTimeOffset );

	// Write the size of the command structure
	DWORD cb, dwSizeCommand = sizeof( DMUS_IO_COMMAND );
	hr = pStream->Write( &dwSizeCommand, sizeof( dwSizeCommand ), &cb );
    if( FAILED( hr ) || cb != sizeof( dwSizeCommand ))
	{
		return E_FAIL;
	}

	// Write the commands
    DMUS_IO_COMMAND	iCommand;
	for( short n = 0; n < m_nMeasures; n++ )
	{
		// Initialize the DMUS_IO_COMMAND structure
		ZeroMemory( &iCommand, sizeof( DMUS_IO_COMMAND ) );
		// Set the measure number
		iCommand.wMeasure	= (WORD) (n - nOffsetMeasure);
		// Convert from a measure number to a time
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, n, 0, &iCommand.mtTime );
		// Offset the time of this command
		iCommand.mtTime	-= lTimeOffset;
		// If there is a command, update iCommand with the data from it
		if( m_pMeasureArray[n].pCommand != NULL )
		{
			iCommand.bCommand = m_pMeasureArray[n].pCommand->bCommand;
			iCommand.bGrooveLevel = m_pMeasureArray[n].pCommand->bGrooveLevel;
			iCommand.bGrooveRange = m_pMeasureArray[n].pCommand->bGrooveRange;
			iCommand.bRepeatMode = m_pMeasureArray[n].pCommand->bRepeatMode;
			if( nOffsetMeasure == 0 )
			{
				m_pMeasureArray[n].pCommand->lTime = iCommand.mtTime;
			}
		}

		if(( bOnlySelected && ( m_pMeasureArray[n].dwFlags & CMI_SELECTED )) ||
		   ( !bOnlySelected && (iCommand.bCommand != 0 || iCommand.bGrooveLevel != 0 ||
								iCommand.bGrooveRange != 0 || iCommand.bRepeatMode != 0) ))
		{
			hr = pStream->Write( &iCommand, sizeof( DMUS_IO_COMMAND ), &cb );
			if( FAILED( hr ) || cb != sizeof( DMUS_IO_COMMAND ))
			{
				return E_FAIL;
			}
		}
	}
	return S_OK;
}

void CCommandMgr::ClearCommandList( CommandExt* pCommandList, BOOL bClearSelection )
{
	CommandExt* pCE;

	if( pCommandList == m_pCommandList )
	{
		if( m_nMeasures > 0 )
		{
			//ASSERT(m_pMeasureArray != NULL );
			HRESULT hr = VerifyMeasureArray();
			if (FAILED(hr)) return;
			if( m_pMeasureArray != NULL )
			{
				for( short n = 0; n < m_nMeasures; n++ )
				{
					m_pMeasureArray[n].pCommand = NULL;
					if( bClearSelection )
					{
						m_pMeasureArray[n].dwFlags &= ~CMI_SELECTED;
					}
				}
			}
		}
	}

	while( pCommandList )
	{
		pCE = pCommandList->pNext;
		delete pCommandList;
		pCommandList = pCE;
	}
}

void CCommandMgr::SelectSegment( long lBeginTime, long lEndTime )
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

	//ASSERT(m_pMeasureArray != NULL );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	lTemp = lBeginTime;
	if( lBeginTime == -1 )	// Use shiftfrommeasure
	{
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, m_lShiftFromMeasure, 0, &lTemp );
	}

	if( lTemp > lEndTime )
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
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lBegin, &lMeasure, NULL );
	lBegin = lMeasure;
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lEnd, &lMeasure, NULL );
	lEnd = lMeasure;

//	ASSERT( lEnd < (long) m_nMeasures );
	if( lEnd >= m_nMeasures )
	{
//		return;
		lEnd = m_nMeasures - 1;	// round up to next higher measure ok unless its the last measure
	}

	// Set the selected flag of the measures in this segment.
	short n;
	for( n = (short) lBegin; n <= (short) lEnd; n++ )
	{
		m_pMeasureArray[n].dwFlags |= CMI_SELECTED;
		m_bSelected = TRUE;
	}
}

HRESULT CCommandMgr::InsertCommand( long lMeasure )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT		hr;
	long		lClocks;
	long		lInsertMeasure;

	lInsertMeasure = lMeasure;
	if( lInsertMeasure == -1 )
	{
		lInsertMeasure = (long) GetFirstSelectedMeasure();
	}

	ASSERT( lInsertMeasure > -1 && lInsertMeasure < (long) m_nMeasures );
	if( lInsertMeasure < 0 || lInsertMeasure >= (long) m_nMeasures )
	{
		return E_UNEXPECTED;
	}

	ASSERT( m_pTimeline != NULL && m_pMeasureArray != NULL );
	if( m_pTimeline == NULL || m_pMeasureArray == NULL )
	{
		return E_UNEXPECTED;
	}

	if( m_pMeasureArray[lInsertMeasure].pCommand != NULL )
	{
		// Just open the property page.
		OnShowProperties();
		return S_OK;
	}

	m_pMeasureArray[lInsertMeasure].pCommand = new CommandExt;
	if( m_pMeasureArray[lInsertMeasure].pCommand == NULL )
	{
		return E_OUTOFMEMORY;
	}

	ClearSelected();
	m_pMeasureArray[lInsertMeasure].dwFlags |= CMI_SELECTED;
	m_bSelected = TRUE;

	// Update the list pointers.
	short n;

	// Find the previous command.
	for( n = short (lInsertMeasure - 1); n > -1; n-- )
	{
		if( m_pMeasureArray[n].pCommand != NULL )
		{
			break;
		}
	}

	if( n > -1 )
	{
		m_pMeasureArray[n].pCommand->pNext = m_pMeasureArray[lInsertMeasure].pCommand;

		// Find the next command.
		for( n = short (lInsertMeasure + 1); n < m_nMeasures; n++ )
		{
			if( m_pMeasureArray[n].pCommand != NULL )
			{
				break;
			}
		}
		if( n < m_nMeasures )
		{
			m_pMeasureArray[lInsertMeasure].pCommand->pNext = m_pMeasureArray[n].pCommand;
		}
		else
		{
			m_pMeasureArray[lInsertMeasure].pCommand->pNext = NULL;
		}
	}
	else
	{
		/* this doesn't seem right
		ASSERT( m_pCommandList == NULL );
		if( m_pCommandList != NULL )
		{
			return E_UNEXPECTED;
		}
		*/
		CommandExt* pCmd = m_pCommandList;
		m_pCommandList = m_pMeasureArray[lInsertMeasure].pCommand;
		m_pCommandList->pNext = pCmd;
	}

	m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lInsertMeasure, 0, &lClocks );

	// Initialize the command.
	m_pMeasureArray[lInsertMeasure].pCommand->lTime		= lClocks;
	m_pMeasureArray[lInsertMeasure].pCommand->bCommand	= DMUS_COMMANDT_GROOVE;
	m_pMeasureArray[lInsertMeasure].pCommand->bGrooveLevel	= 1;
	m_pMeasureArray[lInsertMeasure].pCommand->bGrooveRange	= 0;
	m_pMeasureArray[lInsertMeasure].pCommand->bRepeatMode	= 0;

	// Get a pointer to the property sheet and show it
	IDMUSProdPropSheet*	pIPropSheet;
	VARIANT			var;
	LPUNKNOWN		punk;

	m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	if( var.vt == VT_UNKNOWN )
	{
		punk = V_UNKNOWN( &var );
		if( punk )
		{
			hr = punk->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet );
			ASSERT( SUCCEEDED( hr ));
			if( FAILED( hr ))
			{
				return E_UNEXPECTED;
			}
			pIPropSheet->Show( TRUE );
			pIPropSheet->Release();
			punk->Release();
		}
	}
	// Update the property page.
	OnShowProperties();
	return S_OK;
}

void CCommandMgr::ClearSelected()
{
	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	short n;

	for( n = 0; n < m_nMeasures; n++ )
	{
		m_pMeasureArray[n].dwFlags &= ~CMI_SELECTED;
	}

	m_bSelected = FALSE;
}

void CCommandMgr::SelectAll()
{
	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	short n;

	for( n = 0; n < m_nMeasures; n++ )
	{
		m_pMeasureArray[n].dwFlags |= CMI_SELECTED;
	}

	m_bSelected = TRUE;

	if( m_pIPageManager != NULL )
	{
		m_pIPageManager->RefreshData();
	}
}

void CCommandMgr::DeleteSelected()
{
	CommandExt*	pCE = NULL;
	short		n;

	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pCommand != NULL )
		{
			if( m_pMeasureArray[n].dwFlags & CMI_SELECTED )
			{
				if( pCE == NULL )
				{
					ASSERT( m_pCommandList == m_pMeasureArray[n].pCommand );
					m_pCommandList = m_pMeasureArray[n].pCommand->pNext;
				}
				else
				{
					ASSERT( pCE->pNext == m_pMeasureArray[n].pCommand );
					pCE->pNext = m_pMeasureArray[n].pCommand->pNext;
				}
				delete m_pMeasureArray[n].pCommand;
				m_pMeasureArray[n].pCommand = NULL;
			}
			else
			{
				pCE = m_pMeasureArray[n].pCommand;
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

void CCommandMgr::ToggleSelect( long lMeasure )
{
	//ASSERT(m_pMeasureArray != NULL );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	if( lMeasure >= (long) m_nMeasures )
	{
		return;
	}

	m_pMeasureArray[lMeasure].dwFlags ^= CMI_SELECTED;
	if( m_pMeasureArray[lMeasure].dwFlags & CMI_SELECTED )
	{
		m_bSelected = TRUE;
	}

	if( m_pIPageManager != NULL )
	{
		m_pIPageManager->RefreshData();
	}
}

BOOL CCommandMgr::IsSelected( long lMeasure )
{
	//ASSERT(m_pMeasureArray != NULL );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return FALSE;

	//ASSERT( lMeasure < (long) m_nMeasures );
	if( lMeasure >= (long) m_nMeasures )
	{
		return FALSE;
	}

	return ( m_pMeasureArray[lMeasure].dwFlags & CMI_SELECTED ? TRUE : FALSE );
}

short CCommandMgr::GetFirstSelectedMeasure()
{
	short n;

	if( m_pMeasureArray == NULL )
	{
		return -1;
	}

	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].dwFlags & CMI_SELECTED )
		{
			return n;
		}
	}

	return -1;
}

CommandExt* CCommandMgr::GetFirstSelectedCommand()
{
	for( int n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pCommand && (m_pMeasureArray[n].dwFlags & CMI_SELECTED) )
		{
			return m_pMeasureArray[n].pCommand;
		}
	}
	return NULL;
}

void CCommandMgr::GetBoundariesOfSelectedCommands( long *plStart, long *plEnd )
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
		if( m_pMeasureArray[n].dwFlags & CMI_SELECTED )
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

BOOL CCommandMgr::DeleteBetweenTimes( long lStart, long lEnd )
{
	ASSERT( lStart >=0 );
	ASSERT( lEnd >= 0 );

	BOOL fChanged = FALSE;

	if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lStart, &lStart, NULL ) )
	&&  SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lEnd, &lEnd, NULL ) ) )
	{
		lEnd = min( lEnd, (m_nMeasures - 1) );
		CommandExt*	pCE = NULL;
		for( int n=0; n <= lEnd; n++ )
		{
			if( n >= lStart )
			{
				if( m_pMeasureArray[n].pCommand )
				{
					if( pCE == NULL )
					{
						ASSERT( m_pCommandList == m_pMeasureArray[n].pCommand );
						m_pCommandList = m_pMeasureArray[n].pCommand->pNext;
					}
					else
					{
						ASSERT( pCE->pNext == m_pMeasureArray[n].pCommand );
						pCE->pNext = m_pMeasureArray[n].pCommand->pNext;
					}
					delete m_pMeasureArray[n].pCommand;
					m_pMeasureArray[n].pCommand = NULL;
					fChanged = TRUE;
				}
			}
			else
			{
				if( m_pMeasureArray[n].pCommand )
				{
					pCE = m_pMeasureArray[n].pCommand;
				}
			}
		}
	}

	return fChanged;
}

short CCommandMgr::MarkSelectedCommands(DWORD flags)
// returns offset of first measure from zero
{

	short	n;
	BOOL	bFirstTime = TRUE;
	short	nOffset = -1;
	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return 0;

	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].dwFlags & CMI_SELECTED )
		{
			if(bFirstTime)
			{
				nOffset = n;
				bFirstTime = FALSE;
			}
			if( m_pMeasureArray[n].pCommand != NULL )
			{
				m_pMeasureArray[n].pCommand->dwDragDrop |= flags;
			}
		}
	}
	return nOffset;
}

void CCommandMgr::DeleteMarkedCommands(DWORD dwFlags)
{
	CommandExt*	pCE = NULL;
	short		n;

	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pCommand != NULL )
		{
			if( m_pMeasureArray[n].pCommand->dwDragDrop & dwFlags )
			{
				if( pCE == NULL )
				{
					ASSERT( m_pCommandList == m_pMeasureArray[n].pCommand );
					m_pCommandList = m_pMeasureArray[n].pCommand->pNext;
				}
				else
				{
					ASSERT( pCE->pNext == m_pMeasureArray[n].pCommand );
					pCE->pNext = m_pMeasureArray[n].pCommand->pNext;
				}
				delete m_pMeasureArray[n].pCommand;
				m_pMeasureArray[n].pCommand = NULL;
			}
			else
			{
				pCE = m_pMeasureArray[n].pCommand;
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

void CCommandMgr::UnMarkCommands(WORD flags)
{

	short		n;
	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	for( n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pCommand != NULL )
		{
			m_pMeasureArray[n].pCommand->dwDragDrop &= ~flags;
		}
	}

}

void CCommandMgr::MarkAtMeasure(DWORD measure, WORD flags)
{
	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	if(m_pMeasureArray[measure].pCommand != NULL)
	{
		m_pMeasureArray[measure].pCommand->dwDragDrop |= flags;
	}

}

void CCommandMgr::UnMarkAtMeasure(DWORD measure, WORD flags)
{
	//ASSERT(m_pMeasureArray != NULL || m_nMeasures == 0 );
	HRESULT hr = VerifyMeasureArray();
	if (FAILED(hr)) return;

	if(m_pMeasureArray[measure].pCommand != NULL)
	{
		m_pMeasureArray[measure].pCommand->dwDragDrop &= ~flags;
	}

}

HRESULT CCommandMgr::LoadDroppedCommandList( IStream* pStream, long lDropPos, bool fDragging, BOOL &fChanged )
{
	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	//ASSERT(m_pMeasureArray != NULL );
	HRESULT	hr;
	hr = VerifyMeasureArray();
	if (FAILED(hr)) return hr;

	if( NULL == pStream )
	{
		return E_POINTER;
	}

	// Get the size of the stream.
	STATSTG statstg;
	hr = pStream->Stat( &statstg, STATFLAG_NONAME );
	if( FAILED( hr ))
	{
		return hr;
	}

	DWORD cSize;
	cSize = statstg.cbSize.LowPart;

	// Don't seek to the beginning of the stream (command strip data can be embedded)
	//liStreamPos.QuadPart = 0;
	//hr = pStream->Seek( liStreamPos, STREAM_SEEK_SET, NULL );
	//ASSERT( SUCCEEDED( hr ));

	DWORD cb;
	long lDragMeasure = 0;	// measure at which mouse grabbed dragged data
	if( fDragging )
	{
		// load drag pt
		hr = pStream->Read(&lDragMeasure, sizeof( long), &cb);
		if( FAILED( hr ) || cb != sizeof( long ) )
		{
			return E_FAIL;
		}

		cSize -= cb;
	}

    // load size of command structure
	DWORD dwSizeCommand;
    hr = pStream->Read( &dwSizeCommand, sizeof( dwSizeCommand ), &cb );
    if( FAILED( hr ) || cb != sizeof( dwSizeCommand ) )
	{
        return E_FAIL;
	}

	cSize -= cb;

	// Save our current position
	DWORD cSizeOrig = cSize;	// Size of stream
	DWORD cCurPos = StreamTell( pStream );

	long lLastMeasure = LONG_MIN;
	DMUS_IO_COMMAND iCommand;
	LARGE_INTEGER liStreamPos;
	while( cSize >= dwSizeCommand )
	{
		// Read the command struct.
        if( dwSizeCommand > sizeof( DMUS_IO_COMMAND ) )
		{
            hr = pStream->Read( &iCommand, sizeof( DMUS_IO_COMMAND ), &cb );
            if( FAILED( hr ) || cb != sizeof( DMUS_IO_COMMAND ) )
			{
				return E_FAIL;
			}
			// Seek past the extra data.
			liStreamPos.QuadPart = dwSizeCommand - sizeof( DMUS_IO_COMMAND );
			pStream->Seek( liStreamPos, STREAM_SEEK_CUR, NULL );
			if( FAILED( hr ) )
			{
				return E_FAIL;
			}
		}
		else
		{
            hr = pStream->Read( &iCommand, dwSizeCommand, &cb );
            if( FAILED( hr ) || cb != dwSizeCommand )
			{
				return E_FAIL;
			}
		}

		// If this measure is greater than any previous ones, save it
		if( iCommand.wMeasure > lLastMeasure )
		{
			lLastMeasure = iCommand.wMeasure;
		}

		cSize -= dwSizeCommand;
	}

	// Restore our position back to the start of the signposts
	cSize = cSizeOrig;
	StreamSeek( pStream, cCurPos, STREAM_SEEK_SET );

	BOOL bFirstTime = TRUE;
	long lAdjustMeasure = 0;
	long lMeasure;
	while( cSize >= dwSizeCommand )
	{
		// Read the command struct.
        if( dwSizeCommand > sizeof( DMUS_IO_COMMAND ) )
        {
            hr = pStream->Read( &iCommand, sizeof( DMUS_IO_COMMAND ), &cb );
            if( FAILED( hr ) || cb != sizeof( DMUS_IO_COMMAND ) )
            {
                return E_FAIL;
            }
			// Seek past the extra data.
			liStreamPos.QuadPart = dwSizeCommand - sizeof( DMUS_IO_COMMAND );
			pStream->Seek( liStreamPos, STREAM_SEEK_CUR, NULL );
 			if( FAILED( hr ) )
			{
				return E_FAIL;
			}
		}
        else
        {
            hr = pStream->Read( &iCommand, dwSizeCommand, &cb );
            if( FAILED( hr ) || cb != dwSizeCommand )
            {
                return E_FAIL;
            }
        }

        cSize -= dwSizeCommand;

		// If this is a paste, and we are looking at the first command, determine the amount
		// of time the lTime field of each of the commands being inserted should be shifted.
		if( bFirstTime )
		{
			if( !fDragging )
			{
				// Use position of cursor
				long lTime;
				m_pTimeline->GetMarkerTime(MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &lTime);
				m_pTimeline->ClocksToMeasureBeat(m_dwGroupBits, 0, lTime, &lMeasure, NULL);

				// Adjust the commands so the first one starts in lMeasure
				lAdjustMeasure = lMeasure - iCommand.wMeasure;
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

			// Ensure that the last command starts before the end of the segment
			if( lAdjustMeasure + lLastMeasure >= m_nMeasures )
			{
				lAdjustMeasure = m_nMeasures - lLastMeasure - 1;
			}

			// Ensure that the first command starts at or after measure 0
			if( iCommand.wMeasure + lAdjustMeasure < 0 )
			{
				lAdjustMeasure -= iCommand.wMeasure + lAdjustMeasure;
			}

			bFirstTime = FALSE;
			ClearSelected();
		}

		// Adjust the time of the command from the drag and drop measures
		lMeasure = iCommand.wMeasure + lAdjustMeasure;

		// Should be at least 0
		ASSERT( lMeasure >= 0 );
		if(lMeasure < 0)
		{
			lMeasure = 0;
		}

		// Any items that would paste beyond the end of this segment are lost
		if( lMeasure >= (long) m_nMeasures )
		{
			break;
		}

		if( (iCommand.bCommand == 0) && (iCommand.bGrooveLevel == 0)
		&&	(iCommand.bGrooveRange == 0) && (iCommand.bRepeatMode == 0) )
		{
			// If we are dragging a blank measure on top of a measure with a command, delete the command
			if( fDragging && m_pMeasureArray[lMeasure].pCommand != NULL )
			{
				delete m_pMeasureArray[lMeasure].pCommand;
				m_pMeasureArray[lMeasure].pCommand = NULL;
				fChanged = TRUE;
			}
		}
		else
		{
			// If the measure doesn't already have a command, create one
			if( m_pMeasureArray[lMeasure].pCommand == NULL )
			{
				m_pMeasureArray[lMeasure].pCommand = new CommandExt;
				if( m_pMeasureArray[lMeasure].pCommand == NULL )
				{
					return E_OUTOFMEMORY;
				}
			}

			// convert measure to a time
			m_pTimeline->MeasureBeatToClocks(m_dwGroupBits, 0, lMeasure, 0, &iCommand.mtTime);

			m_pMeasureArray[lMeasure].pCommand->lTime		= iCommand.mtTime;
			m_pMeasureArray[lMeasure].pCommand->bCommand	= iCommand.bCommand;
			m_pMeasureArray[lMeasure].pCommand->bGrooveLevel= iCommand.bGrooveLevel;
			m_pMeasureArray[lMeasure].pCommand->bGrooveRange= iCommand.bGrooveRange;
			m_pMeasureArray[lMeasure].pCommand->bRepeatMode	= iCommand.bRepeatMode;
			UnMarkAtMeasure(lMeasure, DRAG_SELECT);
			fChanged = TRUE;
		}

		m_pMeasureArray[lMeasure].dwFlags |= CMI_SELECTED;
		m_bSelected = TRUE;
	} // while( cSize > 0 )

	// Update the "next" pointers
	m_pCommandList = NULL;
	CommandExt* pCE = NULL;
	for( short n = 0; n < m_nMeasures; n++ )
	{
		if( m_pMeasureArray[n].pCommand != NULL )
		{
			if( m_pCommandList == NULL )
			{
				m_pCommandList = m_pMeasureArray[n].pCommand;
			}
			else
			{
				pCE->pNext = m_pMeasureArray[n].pCommand;
			}
			pCE = m_pMeasureArray[n].pCommand;
			pCE->pNext = NULL;
		}
	}

	// Update the property page and the object we represent.
	if( m_pIPageManager != NULL )
	{
		m_pIPageManager->RefreshData();
	}

	return S_OK;
}

void CCommandMgr::UpdateSegment( void )
{
	ASSERT( m_pTimeline );
	if( m_pTimeline )
	{
		m_pTimeline->OnDataChanged( (IDMUSProdStripMgr *)this );
	}

	ASSERT( m_pIDMTrack );
	if( m_pIDMTrack )
	{
		SyncWithDirectMusic();
	}
}

HRESULT CCommandMgr::ResizeMeasureArray()
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
		CommandMeasureInfo* pMeasureArray = new CommandMeasureInfo[lMeasureLength];
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
			pMeasureArray[n].pCommand = m_pMeasureArray[n].pCommand;
		}
		// initialize any remaining new measures 
		// (if lMeasure < m_pMeasureArray, then this for loop never entered)
		for( n = nlen; n < (short)lMeasureLength; n++ )
		{
			pMeasureArray[n].dwFlags = 0;
			pMeasureArray[n].pCommand = NULL;
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
			if( m_pCommandStrip->m_fInAllTracksAdded == FALSE )
			{
				UpdateSegment();
			}
		}
	}

	return hr;
}

void CCommandMgr::SyncMeasureArray()
{
	CommandExt	*pSPE;
	RemoveOrphanCommands();
	// Update the "next" pointers
	m_pCommandList = NULL;
	pSPE = NULL;
	for(short n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].pCommand != NULL)
		{
			if(m_pCommandList == NULL)
			{
				m_pCommandList = m_pMeasureArray[n].pCommand;
			}
			else
			{
				pSPE->pNext = m_pMeasureArray[n].pCommand;
			}
			pSPE = m_pMeasureArray[n].pCommand;
			pSPE->pNext = NULL;
		}
	}
}

void CCommandMgr::RemoveOrphanCommands()
// removes Commands that don't have a corresponding measure
// for purposes of this function, Commands with corresponding measures
// are said to be "legal".
{
	CommandExt* pExt = NULL;
	for(short n = 0; n < m_nMeasures; n++)
	{
		if(m_pMeasureArray[n].pCommand)
		{
			CommandExt* p = new CommandExt;
			memcpy(p, m_pMeasureArray[n].pCommand, sizeof(CommandExt));
			p->pNext = 0;
			m_pMeasureArray[n].pCommand = p;
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

	CommandExt* p = m_pCommandList;
	while(p)
	{
		CommandExt* q = p->pNext;
		delete p;
		p = q;
	}
	m_pCommandList = pExt;
}

