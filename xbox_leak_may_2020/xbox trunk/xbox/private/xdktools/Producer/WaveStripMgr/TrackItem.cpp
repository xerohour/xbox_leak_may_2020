#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackItem.h"
#include "TrackMgr.h"
#include <dmusicf.h>

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//	CTrackItem Constructors/Destructor

CTrackItem::CTrackItem( void )
{
	m_pTrackMgr = NULL;
	m_pWaveStrip = NULL;

	Clear();
}

CTrackItem::CTrackItem( CTrackMgr* pTrackMgr, CWaveStrip* pWaveStrip )
{
	ASSERT( pTrackMgr != NULL );
	m_pTrackMgr = pTrackMgr;

	ASSERT( pWaveStrip != NULL );
	m_pWaveStrip = pWaveStrip;

	Clear();
}

CTrackItem::CTrackItem( CTrackMgr* pTrackMgr, CWaveStrip* pWaveStrip, const CTrackItem& item )
{
	ASSERT( pTrackMgr != NULL );
	m_pTrackMgr = pTrackMgr;

	ASSERT( pWaveStrip != NULL );
	m_pWaveStrip = pWaveStrip;

	// Copy the passed-in item
	Copy( &item );
}

CTrackItem::~CTrackItem( void )
{
	if( m_pTrackMgr )
	{
		SetFileReference( NULL );
	}
	else
	{
		ClearListInfo();
		RELEASE( m_FileRef.pIDocRootNode );
	}
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::Clear

void CTrackItem::Clear()
{
	m_rtTimeLogical = 0;
	m_rtTimePhysical = 0;
	m_rtStartOffset = 0;
	m_rtDuration = 0;

	m_dwVariations = 0xFFFFFFFF;
	m_dwFlagsDM = 0;
	m_lVolume = 0;
	m_lPitch = 0;
	m_lVolumeRange = 0;
	m_lPitchRange = 0;

	m_fLoopedUI = FALSE;
	m_dwLoopStartUI = 0;
	m_dwLoopEndUI = 0;
	m_fLockLoopLengthUI = FALSE;
	m_fLockEndUI = FALSE;
	m_fLockLengthUI = TRUE;

	m_dwBitsUI = 0;
	m_pLayer = NULL;
	m_fSelected = FALSE;
	m_fSyncDuration = TRUE;
	m_nPasteLayerIndex = 0;

	::SetRect( &m_rectWave, 0, 0, 0, 0 );
	::SetRect( &m_rectSelect, 0, 0, 0, 0 );

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( _Module.GetModuleInstance() );

	if( m_pTrackMgr )
	{
		SetFileReference( NULL );
	}
	else
	{
		ClearListInfo();
		RELEASE( m_FileRef.pIDocRootNode );
	}

	AfxSetResourceHandle( hInstance );
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::ClearListInfo

void CTrackItem::ClearListInfo()
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( _Module.GetModuleInstance() );

	m_FileRef.li.pIProject = NULL;

	m_FileRef.li.strProjectName.LoadString( IDS_EMPTY_TEXT );
	m_FileRef.li.strName.LoadString( IDS_EMPTY_TEXT );
	m_FileRef.li.strDescriptor.LoadString( IDS_EMPTY_TEXT );

	memset( &m_FileRef.li.guidFile, 0, sizeof(GUID) );

	AfxSetResourceHandle( hInstance );
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

	m_rtTimeLogical = pItem->m_rtTimeLogical;
	m_rtTimePhysical = pItem->m_rtTimePhysical;
	m_rtStartOffset = pItem->m_rtStartOffset;
	m_rtDuration = pItem->m_rtDuration;

	m_dwVariations = pItem->m_dwVariations;
	m_dwFlagsDM = pItem->m_dwFlagsDM;
	m_lVolume = pItem->m_lVolume;
	m_lPitch = pItem->m_lPitch;
	m_lVolumeRange = pItem->m_lVolumeRange;
	m_lPitchRange = pItem->m_lPitchRange;

	m_fLoopedUI = pItem->m_fLoopedUI;
	m_dwLoopStartUI = pItem->m_dwLoopStartUI;
	m_dwLoopEndUI = pItem->m_dwLoopEndUI;
	m_fLockLoopLengthUI = pItem->m_fLockLoopLengthUI;
	m_fLockEndUI = pItem->m_fLockEndUI;
	m_fLockLengthUI = pItem->m_fLockLengthUI;

	m_dwBitsUI = pItem->m_dwBitsUI;
	m_pLayer = pItem->m_pLayer;
	m_fSelected = pItem->m_fSelected;
	m_fSyncDuration = pItem->m_fSyncDuration;
	m_nPasteLayerIndex = pItem->m_nPasteLayerIndex;

	m_WaveInfo = pItem->m_WaveInfo;

	if( m_pTrackMgr )
	{
		SetFileReference( pItem->m_FileRef.pIDocRootNode );
	}
	else
	{
		m_FileRef.li.pIProject = pItem->m_FileRef.li.pIProject;
		m_FileRef.li.strProjectName = pItem->m_FileRef.li.strProjectName;
		m_FileRef.li.strName = pItem->m_FileRef.li.strName;
		m_FileRef.li.strDescriptor = pItem->m_FileRef.li.strDescriptor;
		memcpy( &m_FileRef.li.guidFile, &pItem->m_FileRef.li.guidFile, sizeof(GUID) );

		RELEASE( m_FileRef.pIDocRootNode );
		m_FileRef.pIDocRootNode = pItem->m_FileRef.pIDocRootNode;
		if( m_FileRef.pIDocRootNode )
		{
			m_FileRef.pIDocRootNode->AddRef();
		}
	}
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::After

BOOL CTrackItem::After( const CTrackItem& item )
{
	// Check if this item is after the one passed in.
	if( m_rtTimePhysical > item.m_rtTimePhysical )
	{
		// Our physical time is greater - we're after the item
		return TRUE;
	}

	// We're either before the item, or occur at the same time
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::Before

BOOL CTrackItem::Before( const CTrackItem& item )
{
	// Check if this item is before the one passed in.
	if( m_rtTimePhysical < item.m_rtTimePhysical )
	{
		// Our physical time is less - we're before the item
		return TRUE;
	}

	// We're either after the item, or occur at the same time
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::GetWaveInfo

HRESULT CTrackItem::GetWaveInfo( void )
{
	REFERENCE_TIME rtWaveLength = 0;

	// Prepare WaveInfoParams struct
	WaveInfoParams	wip;
	memset( &wip, 0, sizeof(WaveInfoParams) );
	wip.cbSize = sizeof(WaveInfoParams);

	HRESULT hr = S_OK;

	// Ask DLS Designer for info about the wave
	if( m_FileRef.pIDocRootNode )
	{
		IDMUSProdWaveTimelineDraw* pIWaveTimelineDraw;
		if( SUCCEEDED ( m_FileRef.pIDocRootNode->QueryInterface( IID_IDMUSProdWaveTimelineDraw, (void **)&pIWaveTimelineDraw ) ) )
		{
			if( FAILED ( pIWaveTimelineDraw->GetWaveInfo( &wip ) ) )
			{
				hr = E_FAIL;
			}

			if( FAILED ( pIWaveTimelineDraw->SampleToRefTime( wip.dwWaveDuration, &rtWaveLength, m_lPitch ) ) )
			{
				hr = E_FAIL;
			}

			RELEASE( pIWaveTimelineDraw );
		}
	}

	m_WaveInfo.rtWaveLength = rtWaveLength;	// REFERENCE_TIME
	m_WaveInfo.dwWaveEnd = wip.dwWaveDuration - 1;
	m_WaveInfo.fIsLooped = (wip.dwFlags & WAVE_LOOPED) ? TRUE : FALSE;
	m_WaveInfo.dwLoopType = wip.dwLoopType;
	m_WaveInfo.dwLoopStart = wip.dwLoopStart;
	m_WaveInfo.dwLoopEnd = wip.dwLoopEnd;
	m_WaveInfo.guidVersion = wip.guidVersion;
	m_WaveInfo.fIsStreaming = (wip.dwFlags & WAVE_STREAMING) ? TRUE : FALSE;

	// Cannot loop streaming waves
/*	if( m_WaveInfo.fIsStreaming )
	{
		m_fLoopedUI = FALSE;
		m_dwLoopStartUI = 0;
		m_dwLoopEndUI = 0;
	}*/

	// Convert WaveLength values to either MUSIC_TIME or REFERENCE_TIME
	if( m_pTrackMgr->m_pTimeline )
	{
		REFERENCE_TIME utWaveLength;
		SourceWaveLengthToUnknownTime( m_pTrackMgr, &utWaveLength );

		// Need to reset m_fSyncDuration in case this was a runtime file
		if( m_pTrackMgr->m_fWasRuntimeTrack
		&&  m_pTrackMgr->m_fInAllTracksAdded )
		{
			m_fSyncDuration = (m_rtDuration == utWaveLength) ? TRUE : FALSE;
		}

		// Sync duration
		if( m_rtDuration == 0 )
//		||  m_fSyncDuration )
		{
			m_rtDuration = utWaveLength;	// REFERENCE_TIME or MUSIC_TIME
		}

		// If item is not looped, then make sure item's duration is not longer than wave 
		if( m_fLoopedUI == FALSE )
		{
			if( m_rtDuration > utWaveLength )
			{
				m_rtDuration = utWaveLength;
				m_fSyncDuration = TRUE;
			}
		}
	}

	// If item not looped, then default to wave's loop points
	if( m_fLoopedUI == FALSE )
	{
		m_dwLoopStartUI = m_WaveInfo.dwLoopStart;
		m_dwLoopEndUI = m_WaveInfo.dwLoopEnd;
	}

	// Adjust loop end when applicable
	if( m_dwLoopEndUI == 0
	||  m_dwLoopEndUI > m_WaveInfo.dwWaveEnd )
	{
		m_dwLoopEndUI = m_WaveInfo.dwWaveEnd;
	}

	// Make sure loop is not longer than max loop 
	if( (m_dwLoopEndUI - m_dwLoopStartUI) > MAX_LOOP_LENGTH )
	{
		m_dwLoopEndUI = m_dwLoopStartUI + MAX_LOOP_LENGTH;
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::SetFileReference

HRESULT CTrackItem::SetFileReference( IDMUSProdNode* pINewDocRootNode )
{
	CWaitCursor wait;

	HRESULT hr = S_OK;

	ASSERT( m_pTrackMgr != NULL );
	if( m_pTrackMgr == NULL )
	{
		// Will be NULL when CTrackItem used for properties!
		// Must not set file references for properties!
		return E_FAIL;
	}

	// Get Framework pointer
	IDMUSProdFramework* pIFramework;
	VARIANT variant;
	hr = m_pTrackMgr->GetStripMgrProperty( SMP_IDMUSPRODFRAMEWORK, &variant );
	if( FAILED ( hr ) )
	{
		return hr;
	}
	hr = V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&pIFramework );
	V_UNKNOWN( &variant )->Release();
	if( FAILED ( hr ) )
	{
		return hr;
	}

	// Get DocRoot pointer (i.e. Segment's DocRoot node)
	IDMUSProdNode* pIDocRootNode;
	hr = m_pTrackMgr->GetParam( GUID_DocRootNode, 0, 0, &pIDocRootNode );
	if( FAILED ( hr ) )
	{
		RELEASE( pIFramework );
		return hr;
	}

	// Clean up old file reference
	if( m_FileRef.pIDocRootNode )
	{
		// Turn off notifications for this node
		if( m_FileRef.fRemoveNotify )
		{
			if( pIDocRootNode )
			{
				hr = pIFramework->RemoveFromNotifyList( m_FileRef.pIDocRootNode, pIDocRootNode );
			}
			m_FileRef.fRemoveNotify = FALSE;
		}

		// Unload the wave
		m_pTrackMgr->QueueWaveForUnload( this );
		GetWaveInfo();

		// Release reference 
		RELEASE( m_FileRef.pIDocRootNode );

		// Initialize pertinent fields
		ClearListInfo();
		m_dwBitsUI = 0;
	}

	// Set DocRoot of new file reference
	if( pINewDocRootNode )
	{
		// Turn on notifications
		ASSERT( m_FileRef.fRemoveNotify == FALSE );
		if( pIDocRootNode )
		{
			hr = pIFramework->AddToNotifyList( pINewDocRootNode, pIDocRootNode );
			if( SUCCEEDED ( hr ) )
			{
				m_FileRef.fRemoveNotify = TRUE;
			}
		}
		
		// Update file reference's DocRoot member variable
		m_FileRef.pIDocRootNode = pINewDocRootNode;
		m_FileRef.pIDocRootNode->AddRef();

		// Download the wave
		GetWaveInfo();
		m_pTrackMgr->QueueWaveForDownload( this );

		// Update file reference's list info
		SetListInfo( pIFramework );
	}

	RELEASE( pIFramework );
	RELEASE( pIDocRootNode );

	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::SetListInfo

HRESULT CTrackItem::SetListInfo( IDMUSProdFramework* pIFramework )
{
	HRESULT hr = S_OK;

	ClearListInfo();

	if( m_FileRef.pIDocRootNode )
	{
		DMUSProdListInfo ListInfo;
		ZeroMemory( &ListInfo, sizeof(ListInfo) );
		ListInfo.wSize = sizeof(ListInfo);

		hr = m_FileRef.pIDocRootNode->GetNodeListInfo ( &ListInfo );
		if( SUCCEEDED ( hr ) )
		{
			IDMUSProdProject* pIProject;

			if( ListInfo.bstrName )
			{
				m_FileRef.li.strName = ListInfo.bstrName;
				::SysFreeString( ListInfo.bstrName );
			}
			if( ListInfo.bstrDescriptor )
			{
				m_FileRef.li.strDescriptor = ListInfo.bstrDescriptor;
				::SysFreeString( ListInfo.bstrDescriptor );
			}
			if( SUCCEEDED ( pIFramework->FindProject( m_FileRef.pIDocRootNode, &pIProject ) ) )
			{
				BSTR bstrProjectName;

				m_FileRef.li.pIProject = pIProject;
//				m_FileRef.li.pIProject->AddRef();		intentionally missing

				if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
				{
					m_FileRef.li.strProjectName = bstrProjectName;
					::SysFreeString( bstrProjectName );
				}

				pIProject->Release();
			}

		    pIFramework->GetNodeFileGUID( m_FileRef.pIDocRootNode, &m_FileRef.li.guidFile );
		}
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::FormatUIText

void CTrackItem::FormatUIText( CString& strText )
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( _Module.GetModuleInstance() );

	// Wave name
	if( m_FileRef.pIDocRootNode )
	{
		strText = m_FileRef.li.strName;
	}
	else
	{
		strText.LoadString( IDS_EMPTY_TEXT );
	}

	AfxSetResourceHandle( hInstance );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::LoadListItem

HRESULT CTrackItem::LoadListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent,
								  IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr )
{
	MMCKINFO		ck;
	DWORD			dwByteCount;
	DWORD			dwSize;
	DWORD			dwCurrentFilePos;
	IDMUSProdNode*	pIDocRootNode = NULL;
	CString			strObjectName;
	int				nLayerIndex = 0;
	HRESULT			hr = E_FAIL;

	if( pIRiffStream == NULL 
	||  pckParent == NULL 
	||  pIFramework == NULL 
	||  pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	// TODO - Can remove pTrackMgr arg if this ASSERT never hits
	ASSERT( m_pTrackMgr != NULL );

	// LoadListItem does not expect to be called twice on the same object
	// Code assumes item consists of initial values
	ASSERT( m_FileRef.pIDocRootNode == NULL ); 
	ASSERT( m_rtTimePhysical == 0 );

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	dwCurrentFilePos = StreamTell( pIStream );

	// Load the track item
	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_WAVEITEM_CHUNK:
			{
				DMUS_IO_WAVE_ITEM_HEADER iItemHeader;
                iItemHeader.wPitchRange = 0;
                iItemHeader.wVolumeRange = 0;

				// Read in the item's header structure
				dwSize = min( sizeof( DMUS_IO_WAVE_ITEM_HEADER ), ck.cksize );
				hr = pIStream->Read( &iItemHeader, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_lVolume = iItemHeader.lVolume;
				m_lPitch = iItemHeader.lPitch;
				m_lVolumeRange = iItemHeader.wVolumeRange;
				m_lPitchRange = iItemHeader.wPitchRange;
				m_dwVariations = iItemHeader.dwVariations;
				m_rtStartOffset = iItemHeader.rtStartOffset;
				m_rtDuration = iItemHeader.rtDuration;
				m_dwFlagsDM = iItemHeader.dwFlags;

				// Is wave looped?
				if( (iItemHeader.dwLoopEnd - iItemHeader.dwLoopStart) >= MIN_LOOP_LENGTH ) 
				{
					m_fLoopedUI = TRUE;
				}
				else
				{
					m_fLoopedUI = FALSE;
				}

				// Loop start/end will be overlayed by DMUS_FOURCC_WAVEITEM_UI_CHUNK when present
				m_dwLoopStartUI = iItemHeader.dwLoopStart;	
				m_dwLoopEndUI = iItemHeader.dwLoopEnd;

				m_rtTimePhysical = iItemHeader.rtTime;			// rtTime stores offset
				m_rtTimeLogical = iItemHeader.mtLogicalTime;	// mtLogicalTime stores beat difference
																// between physical and logical times
				break;
			}

			case DMUS_FOURCC_WAVEITEM_UI_CHUNK:
			{
				ioWaveItemUI iWaveItemUI;

				// Read in the item's header structure
				dwSize = min( sizeof( ioWaveItemUI ), ck.cksize );
				hr = pIStream->Read( &iWaveItemUI, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_pTrackMgr->m_fWasRuntimeTrack = false;

				nLayerIndex = iWaveItemUI.m_nLayerIndex;
				m_dwLoopStartUI = iWaveItemUI.m_dwLoopStartUI;	
				m_dwLoopEndUI = iWaveItemUI.m_dwLoopEndUI;
				m_fLockLoopLengthUI = iWaveItemUI.m_fLockLoopLengthUI;
				m_fLockEndUI = iWaveItemUI.m_fLockEndUI;
				m_fLockLengthUI = iWaveItemUI.m_fLockLengthUI;
				m_fSyncDuration = iWaveItemUI.m_fSyncDuration;
				break;
			}

			case FOURCC_DMUSPROD_FILEREF:
			{
				IDMUSProdFileRefChunk* pIFileRef;

				hr = pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**) &pIFileRef );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
				StreamSeek( pIStream, dwCurrentFilePos, 0 );
				pIFileRef->LoadRefChunk( pIStream, &pIDocRootNode );
				pIFileRef->Release();
				break;
			}

			case FOURCC_LIST:
				if( ck.fccType == DMUS_FOURCC_REF_LIST )
				{
					MMCKINFO ckName;

					ckName.ckid = DMUS_FOURCC_NAME_CHUNK;
					if( pIRiffStream->Descend( &ckName, NULL, MMIO_FINDCHUNK ) == 0 )
					{
						// Store wave name
						ReadMBSfromWCS( pIStream, ckName.cksize, &strObjectName );
					}
				}
				break;
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
		dwCurrentFilePos = StreamTell( pIStream );
	}

	// Use m_nPasteLayerIndex to store original layer index
	// Will be converted to actual layer as part of paste (or drop)
	m_nPasteLayerIndex = nLayerIndex;

	if( pIDocRootNode == NULL )
	{
		// Do we have an object name?
		if( !strObjectName.IsEmpty() )
		{
			// Framework could not resolve wave file reference
			// so we will ask user to help
			pIDocRootNode = FindWaveFile( strObjectName, pIStream, pIFramework, pTrackMgr );
		}
	}

	if( pIDocRootNode )
	{
		hr = SetFileReference( pIDocRootNode );
		pIDocRootNode->Release();

		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

ON_ERROR:
	RELEASE( pIStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SaveListItem

HRESULT CTrackItem::SaveListItem( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr )
{
	MMCKINFO ckItem;
	MMCKINFO ck;
	HRESULT hr;

	if( pIRiffStream == NULL 
	||  pTrackMgr == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

	// TODO - Can remove pTrackMgr arg if this ASSERT never hits
	ASSERT( m_pTrackMgr != NULL );

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Structures for determining the stream type
	DMUSProdStreamInfo	StreamInfo;
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

	// Create the DMUS_FOURCC_WAVE_LIST list chunk
	ckItem.fccType = DMUS_FOURCC_WAVE_LIST;
	if( pIRiffStream->CreateChunk( &ckItem, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_WAVEITEM_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_WAVEITEM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_WAVE_ITEM_HEADER oItemHeader;
		ZeroMemory( &oItemHeader, sizeof(DMUS_IO_WAVE_ITEM_HEADER) );

		// Fill in the members of the DMUS_IO_WAVE_ITEM_HEADER structure
		oItemHeader.lVolume = m_lVolume;
		oItemHeader.lPitch = m_lPitch;
		oItemHeader.wVolumeRange = (WORD) m_lVolumeRange;
		oItemHeader.wPitchRange = (WORD) m_lPitchRange;
		oItemHeader.dwVariations = m_dwVariations;
		oItemHeader.rtTime = m_rtTimePhysical;
		oItemHeader.rtStartOffset = m_rtStartOffset;
		oItemHeader.rtReserved = 0;
		oItemHeader.rtDuration = m_rtDuration;
		oItemHeader.mtLogicalTime = (MUSIC_TIME)m_rtTimeLogical;	// m_rtTimeLogical stores beat difference
																	// between physical and logical times
		oItemHeader.dwFlags = m_dwFlagsDM;
		
		if( m_fLoopedUI )
		{
			ASSERT( (m_dwLoopEndUI - m_dwLoopStartUI) >= MIN_LOOP_LENGTH );
			oItemHeader.dwLoopStart = m_dwLoopStartUI;
			oItemHeader.dwLoopEnd = m_dwLoopEndUI;
		}
		else
		{
			oItemHeader.dwLoopStart = 0;
			oItemHeader.dwLoopEnd = 0;
		}

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oItemHeader, sizeof(DMUS_IO_WAVE_ITEM_HEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_WAVE_ITEM_HEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_WAVEITEM_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Create the DMUS_FOURCC_WAVEITEM_UI_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_WAVEITEM_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		ioWaveItemUI oWaveItemUI;
		ZeroMemory( &oWaveItemUI, sizeof(ioWaveItemUI) );

		// Fill in the members of the ioWaveItemUI structure
		oWaveItemUI.m_nLayerIndex = m_pWaveStrip->LayerToIndex( m_pLayer );
		oWaveItemUI.m_dwLoopStartUI = m_dwLoopStartUI;
		oWaveItemUI.m_dwLoopEndUI = m_dwLoopEndUI;
		oWaveItemUI.m_fLockLoopLengthUI = m_fLockLoopLengthUI;
		oWaveItemUI.m_fLockEndUI = m_fLockEndUI;
		oWaveItemUI.m_fLockLengthUI = m_fLockLengthUI;
		oWaveItemUI.m_fSyncDuration = m_fSyncDuration;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oWaveItemUI, sizeof(ioWaveItemUI), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(ioWaveItemUI) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_WAVEITEM_UI_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Create the wave file reference chunk(s)
	{
		if( m_FileRef.pIDocRootNode )
		{
			if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
			{
				SaveDMRef( pIRiffStream, pIFramework, m_FileRef.pIDocRootNode, WL_PRODUCER );
			}
			else if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) )
			{
				SaveDMRef( pIRiffStream, pIFramework, m_FileRef.pIDocRootNode, WL_DIRECTMUSIC );
				if( ftFileType == FT_DESIGN )
				{
					SaveProducerRef( pIRiffStream, pIFramework, m_FileRef.pIDocRootNode );
				}
			}
		}
	}

	// Ascend out of the DMUS_FOURCC_WAVE_LIST list chunk
	if( pIRiffStream->Ascend(&ckItem, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIStream );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::LoadTrackItem

HRESULT CTrackItem::LoadTrackItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent,
								   IDMUSProdFramework* pIFramework )
{
	MMCKINFO		ck;
	DWORD			dwByteCount;
	DWORD			dwSize;
	DWORD			dwCurrentFilePos;
	IDMUSProdNode*	pIDocRootNode = NULL;
	CString			strObjectName;
	int				nLayerIndex = 0;
	HRESULT			hr = E_FAIL;

	// LoadListItem does not expect to be called twice on the same object
	// Code assumes item consists of initial values
	ASSERT( m_FileRef.pIDocRootNode == NULL ); 
	ASSERT( m_rtTimePhysical == 0 );

	if( pIRiffStream == NULL 
	||  pckParent == NULL 
	||  pIFramework == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	dwCurrentFilePos = StreamTell( pIStream );

	// Load the track item
	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_WAVEITEM_CHUNK:
			{
				DMUS_IO_WAVE_ITEM_HEADER iItemHeader;
                iItemHeader.wVolumeRange = 0;
                iItemHeader.wPitchRange = 0;

				// Read in the item's header structure
				dwSize = min( sizeof( DMUS_IO_WAVE_ITEM_HEADER ), ck.cksize );
				hr = pIStream->Read( &iItemHeader, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_lVolume = iItemHeader.lVolume;
				m_lPitch = iItemHeader.lPitch;
				m_lVolumeRange = (long) iItemHeader.wVolumeRange;
				m_lPitchRange = (long) iItemHeader.wPitchRange;
				m_dwVariations = iItemHeader.dwVariations;
				m_rtTimePhysical = iItemHeader.rtTime;
				m_rtStartOffset = iItemHeader.rtStartOffset;
				m_rtDuration = iItemHeader.rtDuration;
				if( m_pTrackMgr->IsRefTimeTrack() )
				{
					// Convert Milliseconds to REFERENCE_TIME
					m_rtTimeLogical = iItemHeader.mtLogicalTime * REFCLOCKS_PER_MILLISECOND;
				}
				else
				{
					m_rtTimeLogical = iItemHeader.mtLogicalTime;
				}
				m_dwFlagsDM = iItemHeader.dwFlags;

				// Is wave looped?
				if( (iItemHeader.dwLoopEnd - iItemHeader.dwLoopStart) >= MIN_LOOP_LENGTH ) 
				{
					m_fLoopedUI = TRUE;
				}
				else
				{
					m_fLoopedUI = FALSE;
				}

				// Loop start/end will be overlayed by DMUS_FOURCC_WAVEITEM_UI_CHUNK when present
				m_dwLoopStartUI = iItemHeader.dwLoopStart;	
				m_dwLoopEndUI = iItemHeader.dwLoopEnd;
				break;
			}

			case DMUS_FOURCC_WAVEITEM_UI_CHUNK:
			{
				ioWaveItemUI iWaveItemUI;

				// Read in the item's header structure
				dwSize = min( sizeof( ioWaveItemUI ), ck.cksize );
				hr = pIStream->Read( &iWaveItemUI, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_pTrackMgr->m_fWasRuntimeTrack = false;

				nLayerIndex = iWaveItemUI.m_nLayerIndex;
				m_dwLoopStartUI = iWaveItemUI.m_dwLoopStartUI;	
				m_dwLoopEndUI = iWaveItemUI.m_dwLoopEndUI;
				m_fLockLoopLengthUI = iWaveItemUI.m_fLockLoopLengthUI;
				m_fLockEndUI = iWaveItemUI.m_fLockEndUI;
				m_fLockLengthUI = iWaveItemUI.m_fLockLengthUI;
				m_fSyncDuration = iWaveItemUI.m_fSyncDuration;
				break;
			}

			case FOURCC_DMUSPROD_FILEREF:
			{
				IDMUSProdFileRefChunk* pIFileRef;

				hr = pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**) &pIFileRef );
				if( FAILED ( hr ) )
				{
					goto ON_ERROR;
				}
				StreamSeek( pIStream, dwCurrentFilePos, 0 );
				pIFileRef->LoadRefChunk( pIStream, &pIDocRootNode );
				pIFileRef->Release();
				break;
			}

			case FOURCC_LIST:
				if( ck.fccType == DMUS_FOURCC_REF_LIST )
				{
					MMCKINFO ckName;

					ckName.ckid = DMUS_FOURCC_NAME_CHUNK;
					if( pIRiffStream->Descend( &ckName, NULL, MMIO_FINDCHUNK ) == 0 )
					{
						// Store wave name
						ReadMBSfromWCS( pIStream, ckName.cksize, &strObjectName );
					}
				}
				break;
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
		dwCurrentFilePos = StreamTell( pIStream );
	}

	// Need to reset m_fSyncDuration in case this was a runtime file
	if( m_pTrackMgr->m_fWasRuntimeTrack )
	{
		// Will be set more intelligently in GetWaveinfo when editor is opened
		// Set here to catch the case where editor is never opened
		m_fSyncDuration = m_fLoopedUI ? FALSE : TRUE;
	}

	// Set the item's layer
	m_pLayer = m_pWaveStrip->CreateLayerForIndex( nLayerIndex );

	if( pIDocRootNode == NULL )
	{
		// Do we have an object name?
		if( !strObjectName.IsEmpty() )
		{
			// Framework could not resolve file reference
			// so we will ask user to help
			pIDocRootNode = FindWaveFile( strObjectName, pIStream, pIFramework, m_pTrackMgr );
		}
	}

	if( pIDocRootNode )
	{
		hr = SetFileReference( pIDocRootNode );
		pIDocRootNode->Release();

		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

ON_ERROR:
	RELEASE( pIStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SaveTrackItem

HRESULT CTrackItem::SaveTrackItem( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdFramework* pIFramework )
{
	MMCKINFO ckItem;
	MMCKINFO ck;
	HRESULT hr;

	if( pIRiffStream == NULL )
	{
		ASSERT( 0 );
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Structures for determining the stream type
	DMUSProdStreamInfo	StreamInfo;
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

	// Create the DMUS_FOURCC_WAVE_LIST list chunk
	ckItem.fccType = DMUS_FOURCC_WAVE_LIST;
	if( pIRiffStream->CreateChunk( &ckItem, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_WAVEITEM_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_WAVEITEM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_WAVE_ITEM_HEADER oItemHeader;
		ZeroMemory( &oItemHeader, sizeof(DMUS_IO_WAVE_ITEM_HEADER) );

		// Fill in the members of the DMUS_IO_WAVE_ITEM_HEADER structure
		oItemHeader.lVolume = m_lVolume;
		oItemHeader.lPitch = m_lPitch;
		oItemHeader.wVolumeRange = (WORD) m_lVolumeRange;
		oItemHeader.wPitchRange = (WORD) m_lPitchRange;
		oItemHeader.dwVariations = m_dwVariations;
		oItemHeader.rtTime = m_rtTimePhysical;
		oItemHeader.rtStartOffset = m_rtStartOffset;
		oItemHeader.rtReserved = 0;
		oItemHeader.rtDuration = m_rtDuration;
		if( m_pTrackMgr->IsRefTimeTrack() )
		{
			// Convert REFERENCE_TIME to milliseconds
			oItemHeader.mtLogicalTime = (MUSIC_TIME)(m_rtTimeLogical / REFCLOCKS_PER_MILLISECOND);
		}
		else
		{
			oItemHeader.mtLogicalTime = (MUSIC_TIME)m_rtTimeLogical;
		}
		oItemHeader.dwFlags = m_dwFlagsDM;
		
		if( m_fLoopedUI )
		{
			ASSERT( (m_dwLoopEndUI - m_dwLoopStartUI) >= MIN_LOOP_LENGTH );
			oItemHeader.dwLoopStart = m_dwLoopStartUI;
			oItemHeader.dwLoopEnd = m_dwLoopEndUI;
		}
		else
		{
			oItemHeader.dwLoopStart = 0;
			oItemHeader.dwLoopEnd = 0;
		}

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oItemHeader, sizeof(DMUS_IO_WAVE_ITEM_HEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_WAVE_ITEM_HEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_WAVEITEM_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Create the DMUS_FOURCC_WAVEITEM_UI_CHUNK chunk
	if( ftFileType == FT_DESIGN )
	{
		ck.ckid = DMUS_FOURCC_WAVEITEM_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		ioWaveItemUI oWaveItemUI;
		ZeroMemory( &oWaveItemUI, sizeof(ioWaveItemUI) );

		// Fill in the members of the ioWaveItemUI structure
		oWaveItemUI.m_nLayerIndex = m_pWaveStrip->LayerToIndex( m_pLayer );
		oWaveItemUI.m_dwLoopStartUI = m_dwLoopStartUI;
		oWaveItemUI.m_dwLoopEndUI = m_dwLoopEndUI;
		oWaveItemUI.m_fLockLoopLengthUI = m_fLockLoopLengthUI;
		oWaveItemUI.m_fLockEndUI = m_fLockEndUI;
		oWaveItemUI.m_fLockLengthUI = m_fLockLengthUI;
		oWaveItemUI.m_fSyncDuration	= m_fSyncDuration;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oWaveItemUI, sizeof(ioWaveItemUI), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(ioWaveItemUI) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_WAVEITEM_UI_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Create the file reference chunk(s)
	{
		if( m_FileRef.pIDocRootNode )
		{
			if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
			{
				SaveDMRef( pIRiffStream, pIFramework, m_FileRef.pIDocRootNode, WL_PRODUCER );
			}
			else if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) )
			{
				SaveDMRef( pIRiffStream, pIFramework, m_FileRef.pIDocRootNode, WL_DIRECTMUSIC );
				if( ftFileType == FT_DESIGN )
				{
					SaveProducerRef( pIRiffStream, pIFramework, m_FileRef.pIDocRootNode );
				}
			}
		}
	}

	// Ascend out of the DMUS_FOURCC_WAVE_LIST list chunk
	if( pIRiffStream->Ascend(&ckItem, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIStream );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SaveDMRef

HRESULT CTrackItem::SaveDMRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdFramework* pIFramework,
							   IDMUSProdNode* pIDocRootNode, WhichLoader whichLoader )
{
	IDMUSProdLoaderRefChunk* pIRefChunkLoader;

	ASSERT( pIFramework != NULL );
	if( pIFramework == NULL )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( SUCCEEDED( pIFramework->QueryInterface( IID_IDMUSProdLoaderRefChunk, (void**)&pIRefChunkLoader ) ) )
	{
		if( pIRefChunkLoader )
		{
			switch( whichLoader )
			{
				case WL_PRODUCER:
					hr = pIRefChunkLoader->SaveRefChunkForLoader( pIStream,
																  pIDocRootNode,
																  CLSID_DirectSoundWave,
																  NULL,
																  whichLoader );
					break;

				case WL_DIRECTMUSIC:
				{
					hr = E_FAIL;

					// Get DocRoot of pIDocRootNode
					IDMUSProdDocType* pIDocType = NULL;
					if( SUCCEEDED ( pIFramework->FindDocTypeByNodeId( GUID_WaveNode, &pIDocType ) ) )
					{
						// Initialize the DMUS_OBJECTDESC structure
						DMUS_OBJECTDESC dmusObjectDesc;
						memset( &dmusObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
						dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);

						// Get object descriptor for pIDocRootNode
						IDMUSProdDocType8* pIDocType8;
						if( SUCCEEDED ( pIDocType->QueryInterface( IID_IDMUSProdDocType8, (void**)&pIDocType8 ) ) )
						{
							if( SUCCEEDED ( pIDocType8->GetObjectDescriptorFromNode( pIDocRootNode, &dmusObjectDesc ) ) )
							{
								hr = pIRefChunkLoader->SaveRefChunkForLoader( pIStream,
																			  pIDocRootNode,
																			  CLSID_DirectSoundWave,
																			  &dmusObjectDesc,
																			  whichLoader );
							}

							RELEASE( pIDocType8 );
						}

						RELEASE( pIDocType );
					}
					break;
				}
			}

			pIRefChunkLoader->Release();
		}
	}

	pIStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SaveProducerRef

HRESULT CTrackItem::SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdFramework* pIFramework, IDMUSProdNode* pIDocRootNode )
{
	IDMUSProdFileRefChunk* pIFileRefChunk;

	ASSERT( pIFramework != NULL );
	if( pIFramework == NULL )
	{
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if ( SUCCEEDED ( pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**)&pIFileRefChunk ) ) )
	{
		pIFileRefChunk->SaveRefChunk( pIStream, pIDocRootNode );
		pIFileRefChunk->Release();
	}

	pIStream->Release();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::FindWaveFile

IDMUSProdNode* CTrackItem::FindWaveFile( CString strWaveName, IStream* pIStream,
										 IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr )
{
	IDMUSProdNode*		pIDocRootNode = NULL;
	IDMUSProdNode*		pITargetDirectoryNode = NULL;
	IDMUSProdDocType*	pIDocType = NULL;
	HRESULT				hr;

	ASSERT( pIFramework != NULL );
	ASSERT( pIStream != NULL );

	// Get DocType for DLS Collections
	hr = pIFramework->FindDocTypeByNodeId( GUID_WaveNode, &pIDocType );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Get the target directory
	DMUSProdStreamInfo	StreamInfo;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pITargetDirectoryNode = StreamInfo.pITargetDirectoryNode;
		pPersistInfo->Release();
	}

	// If a target directory is not associated with the stream
	// use the strip's DocRoot node
	if( pITargetDirectoryNode == NULL )
	{
		pITargetDirectoryNode = pTrackMgr->GetDocRootNode();
	}

	// See if there is a wave named 'strWaveName' in this Project
	if( !strWaveName.IsEmpty() )
	{
		BSTR bstrWaveName = strWaveName.AllocSysString();

		if( FAILED ( pIFramework->GetBestGuessDocRootNode( pIDocType,
										 				   bstrWaveName,
														   pITargetDirectoryNode,
														   &pIDocRootNode ) ) )
		{
			pIDocRootNode = NULL;
		}
	}

	if( pIDocRootNode == NULL )
	{
		// Cannot find the Wave
		// If user cancelled previous search for this Wave, no need to ask again
		if( strWaveName.CompareNoCase( pTrackMgr->m_strLastWaveName ) == 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR; 
		}
		pTrackMgr->m_strLastWaveName = strWaveName;

		// Determine File Open dialog prompt
		CString	strOpenDlgTitle;
		if( strWaveName.IsEmpty() )
		{
			strOpenDlgTitle.LoadString( IDS_FILE_OPEN_ANY_WAVE );
		}
		else
		{
			AfxFormatString1( strOpenDlgTitle, IDS_FILE_OPEN_WAVE, strWaveName );
		}
		BSTR bstrOpenDlgTitle = strOpenDlgTitle.AllocSysString();

		// Display File open dialog
		if( pIFramework->OpenFile(pIDocType, bstrOpenDlgTitle, pITargetDirectoryNode, &pIDocRootNode) != S_OK )
		{
			// Did not open a file, or opened file other than wave file
			// so we do not want this DocRoot
			if( pIDocRootNode )
			{
				pIDocRootNode->Release();
				pIDocRootNode = NULL;
			}
		}
	}

	if( pIDocRootNode )
	{
		// Set dirty flag so that GUID_Segment_AllTracksAdded notification will call OnUpdate()
		pTrackMgr->SetDirtyFlag( TRUE );
	}

ON_ERROR:
	if( pIDocType )
	{
		pIDocType->Release();
	}

	return pIDocRootNode;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SwitchTimeBase

void CTrackItem::SwitchTimeBase( void )
{
	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtActualEnd;

		// Switching from MUSIC_TIME to REFERENCE_TIME
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( (MUSIC_TIME)m_rtTimePhysical + (MUSIC_TIME)m_rtDuration, &rtActualEnd );
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( (MUSIC_TIME)m_rtTimePhysical, &m_rtTimePhysical );
		m_rtDuration = rtActualEnd - m_rtTimePhysical;
		m_rtTimeLogical = m_rtTimePhysical;
	}
	else
	{
		MUSIC_TIME mtActualEnd;
		MUSIC_TIME mtTime;

		// Switching from REFERENCE_TIME to MUSIC_TIME
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( m_rtTimePhysical + m_rtDuration, &mtActualEnd );
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( m_rtTimePhysical, &mtTime );
		m_rtTimePhysical = mtTime;
		m_rtDuration = mtActualEnd - (MUSIC_TIME)m_rtTimePhysical;;
		SetTimePhysical( m_rtTimePhysical, STP_LOGICAL_SET_DEFAULT );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::DrawWave

HRESULT	CTrackItem::DrawWave( HDC hDC, LONG lXOffset, int nLayerIndex,
							  WaveDrawParams* pWDP, WaveTimelineInfoParams* pWTIP )
{
	// Get our clipping rectangle
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );

	// Update WaveTimelineInfoParams struct 
	REFERENCE_TIME rtActualEnd = m_rtTimePhysical + (m_rtDuration + 1);
	m_pTrackMgr->UnknownTimeToRefTime( rtActualEnd, &rtActualEnd );
	m_pTrackMgr->UnknownTimeToRefTime( m_rtTimePhysical, &pWTIP->rtStart );
	pWTIP->rtDuration = rtActualEnd - pWTIP->rtStart;
	pWTIP->rtOffset = PitchAdjustedStartOffset();
	pWTIP->rtDuration -= pWTIP->rtOffset;
	pWTIP->lPitch = m_lPitch;
	if( m_fLoopedUI )
	{
		pWTIP->dwFlags = WAVE_LOOPED;
		pWTIP->dwLoopStart = m_dwLoopStartUI;
		pWTIP->dwLoopEnd = m_dwLoopEndUI;
	}
	else
	{
		pWTIP->dwFlags = 0;
		pWTIP->dwLoopStart = 0;
		pWTIP->dwLoopEnd = 0;
	}

	// Set m_rectWave.top
	// Set m_rectWave.bottom
	m_pWaveStrip->IndexToLayerYCoords( nLayerIndex, &m_rectWave );
	m_rectWave.top++;

	// Set m_rectWave.left
	long lPosition;
	m_pTrackMgr->m_pTimeline->RefTimeToPosition( pWTIP->rtStart, &lPosition );
	lPosition += -lXOffset + 1;
	m_rectWave.left = lPosition;

	// Set m_rectWave.right
	m_pTrackMgr->m_pTimeline->RefTimeToPosition( (rtActualEnd - pWTIP->rtOffset), &lPosition );
	lPosition += -lXOffset + 1;
	m_rectWave.right = lPosition;

	// Check if the left edge of the item is beyond the region we're displaying
	if( m_rectWave.left > rectClip.right )
	{
		// Wave not visible
		return S_FALSE;
	}

	// Check if the right edge of the item is before the region we're displaying
	if( m_rectWave.right <= rectClip.left )
	{
		// Wave not visible
		return S_FALSE;
	}

	if( m_rectWave.left == m_rectWave.right )
	{
		m_rectWave.right++;
	}
	ASSERT( m_rectWave.left < m_rectWave.right );

	// Draw the wave
	if( m_FileRef.pIDocRootNode )
	{
		if( m_rectWave.left < m_rectWave.right )
		{
			IDMUSProdWaveTimelineDraw* pIWaveTimelineDraw;
			if( SUCCEEDED ( m_FileRef.pIDocRootNode->QueryInterface( IID_IDMUSProdWaveTimelineDraw, (void **)&pIWaveTimelineDraw ) ) )
			{
				if( nLayerIndex >= 0 )
				{
					m_rectWave.top += 8;
				}
				pIWaveTimelineDraw->DrawWave( m_pTrackMgr->m_pTimeline, hDC, &m_rectWave, lXOffset, pWDP, pWTIP );
				if( nLayerIndex >= 0 )
				{
					m_rectWave.top -= 8;
				}

				RELEASE( pIWaveTimelineDraw );
			}
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SetTimePhysical

HRESULT CTrackItem::SetTimePhysical( REFERENCE_TIME rtTimePhysical, short nAction )
{
	// Set item's physical time
	REFERENCE_TIME rtOrigTimePhysical = m_rtTimePhysical;
	m_rtTimePhysical = rtTimePhysical;

	if( m_pTrackMgr 
	&&  m_pTrackMgr->IsRefTimeTrack() )
	{
		m_rtTimeLogical = max( 0, m_rtTimePhysical );
		return S_OK;
	}

	if( m_pTrackMgr == NULL
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		if( nAction == STP_LOGICAL_NO_ACTION )
		{
			return S_OK;
		}
		else
		{
			ASSERT( 0 );
			return E_UNEXPECTED;
		}
	}

	// Set logical time
	// RefTimeTrack handled above so we can assume we are dealing with clocks at this point
	long lMeasure, lBeat, lGrid, lTick;
	switch( nAction )
	{
		case STP_LOGICAL_NO_ACTION:
			// Nothing to do
			break;

		case STP_LOGICAL_SET_DEFAULT:
		{
			MUSIC_TIME mtTimePhysical;
			MUSIC_TIME mtTimeLogical;
			m_pTrackMgr->UnknownTimeToClocks( m_rtTimePhysical, &mtTimePhysical );
			m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTimePhysical, &lMeasure, &lBeat, &lGrid, &lTick );
			m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, 0, 0, &mtTimeLogical );
			m_rtTimeLogical = mtTimeLogical;
			break;
		}

		case STP_LOGICAL_FROM_BEAT_OFFSET:
		{
			MUSIC_TIME mtTimePhysical;
			MUSIC_TIME mtTimeLogical;

			// Get track GroupBits
			DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();

			// Snap new physical time to number of beats
			long lPhysicalBeats;
			m_pTrackMgr->UnknownTimeToClocks( m_rtTimePhysical, &mtTimePhysical );
			m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTimePhysical, &lMeasure, &lBeat, &lGrid, &lTick );
			MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Set item's new logical time
			long lNewLogicalBeats = lPhysicalBeats + (MUSIC_TIME)m_rtTimeLogical;	// m_rtTimeLogical stores beat offset
			if( lNewLogicalBeats < 0 )
			{
				lNewLogicalBeats = 0;
			}
			BeatsToMeasureBeat( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lNewLogicalBeats, lMeasure, lBeat );
			m_pTrackMgr->ForceBoundaries( lMeasure, lBeat, &mtTimeLogical );
			m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTimeLogical, &lMeasure, &lBeat, &lGrid, &lTick );
			m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, 0, 0, &mtTimeLogical );
			m_rtTimeLogical = mtTimeLogical;
			break;
		}

		case STP_LOGICAL_ADJUST:
		{
			MUSIC_TIME mtTimePhysical;
			MUSIC_TIME mtTimeLogical;

			// Get track GroupBits
			DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();

			// Snap original physical time to number of beats
			long lPhysicalBeats;
			m_pTrackMgr->UnknownTimeToClocks( rtOrigTimePhysical, &mtTimePhysical );
			m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTimePhysical, &lMeasure, &lBeat, &lGrid, &lTick );
			MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Convert logical time to number of beats
			long lLogicalBeats;
			m_pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)m_rtTimeLogical, &lMeasure, &lBeat, &lGrid, &lTick );
			MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lLogicalBeats );

			// Compute difference between original physical time and original logical time
			long lBeatDiff = lLogicalBeats - lPhysicalBeats;

			// Snap new physical time to number of beats
			m_pTrackMgr->UnknownTimeToClocks( m_rtTimePhysical, &mtTimePhysical );
			m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTimePhysical, &lMeasure, &lBeat, &lGrid, &lTick );
			MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Set item's new logical time
			long lNewLogicalBeats = lPhysicalBeats + lBeatDiff;
			if( lNewLogicalBeats < 0 )
			{
				lNewLogicalBeats = 0;
			}
			BeatsToMeasureBeat( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lNewLogicalBeats, lMeasure, lBeat );
			m_pTrackMgr->ForceBoundaries( lMeasure, lBeat, &mtTimeLogical );
			m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTimeLogical, &lMeasure, &lBeat, &lGrid, &lTick );
			m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, 0, 0, &mtTimeLogical );
			m_rtTimeLogical = mtTimeLogical;
			break;
		}

		default:
			ASSERT( 0 );	// Should not happen!
			break;
	}

	// Make sure item's logical time is not less than zero
	if( m_rtTimeLogical < 0 )
	{
		ASSERT( 0 );
		m_rtTimeLogical = 0;
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::SetTimeLogical
	
HRESULT CTrackItem::SetTimeLogical( REFERENCE_TIME rtTimeLogical )
{
	ASSERT( m_pTrackMgr != NULL );
	if( m_pTrackMgr == NULL )
	{
		// Will be NULL when CTrackItem used for properties!
		return E_FAIL;
	}

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		// Cannot edit logical time in REFERENCE_TIME tracks
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Make sure item's logical time is not less than zero
	if( rtTimeLogical < 0 )
	{
		ASSERT( 0 );
		rtTimeLogical = 0;
	}

	// Set item's logical time
	m_rtTimeLogical = rtTimeLogical;

	if( m_pTrackMgr == NULL 
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Set item's logical measure, beat
	MUSIC_TIME mtTimeLogical;
	long lMeasure, lBeat, lGrid, lTick;
	m_pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)m_rtTimeLogical, &lMeasure, &lBeat, &lGrid, &lTick );
	m_pTrackMgr->MeasureBeatGridTickToClocks( lMeasure, lBeat, 0, 0, &mtTimeLogical );
	m_rtTimeLogical = mtTimeLogical;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SetWaveStripForPropSheet

HRESULT CTrackItem::SetWaveStripForPropSheet( CWaveStrip* pWaveStrip )
{
	m_pWaveStrip = pWaveStrip;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::GetWaveStripForPropSheet

CWaveStrip* CTrackItem::GetWaveStripForPropSheet( void ) const
{
	return m_pWaveStrip;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SetTrackMgr

HRESULT CTrackItem::SetTrackMgr( CTrackMgr* pTrackMgr )
{
	m_pTrackMgr = pTrackMgr;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::GetTrackMgr

CTrackMgr* CTrackItem::GetTrackMgr( void ) const
{
	return m_pTrackMgr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::PitchAdjustedStartOffset

REFERENCE_TIME CTrackItem::PitchAdjustedStartOffset( void )
{
	if( m_rtStartOffset
	&&  m_lPitch )
	{
		if( m_FileRef.pIDocRootNode )
		{
			IDMUSProdWaveTimelineDraw* pIWaveTimelineDraw;
			if( SUCCEEDED ( m_FileRef.pIDocRootNode->QueryInterface( IID_IDMUSProdWaveTimelineDraw, (void **)&pIWaveTimelineDraw ) ) )
			{
				REFERENCE_TIME rtAdjustedOffset;
				DWORD dwSample;

				if( SUCCEEDED ( pIWaveTimelineDraw->RefTimeToSample( m_rtStartOffset, &dwSample, 0 ) )
				&&  SUCCEEDED ( pIWaveTimelineDraw->SampleToRefTime( dwSample, &rtAdjustedOffset, m_lPitch ) ) )
				{
					RELEASE( pIWaveTimelineDraw );
					return rtAdjustedOffset;
				}

				RELEASE( pIWaveTimelineDraw );
			}
		}
	}

	return m_rtStartOffset;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::StartOffsetToUnknownTime

HRESULT CTrackItem::StartOffsetToUnknownTime( CTrackMgr* pTrackMgr, REFERENCE_TIME* prtStartOffset )
{
	if( prtStartOffset == NULL )
	{
		return E_POINTER;
	}
	*prtStartOffset = 0;

	// Must be passed a CTrackMgr* since properties m_Item correctly does not set m_pTrackMgr
	if( pTrackMgr == NULL 
	||  pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	REFERENCE_TIME rtPitchAdjustedStartOffset = PitchAdjustedStartOffset();

	REFERENCE_TIME utStartOffset;
	REFERENCE_TIME rtTimePhysical;
	if( SUCCEEDED ( pTrackMgr->UnknownTimeToRefTime( m_rtTimePhysical, &rtTimePhysical ) ) 
	&&  SUCCEEDED ( pTrackMgr->RefTimeToUnknownTime( rtTimePhysical + rtPitchAdjustedStartOffset, &utStartOffset ) ) )
	{
		*prtStartOffset = utStartOffset - m_rtTimePhysical;
		return S_OK;
	}

	ASSERT( 0 );
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SourceWaveLengthToUnknownTime

HRESULT CTrackItem::SourceWaveLengthToUnknownTime( CTrackMgr* pTrackMgr, REFERENCE_TIME* prtSourceWaveLength )
{
	if( prtSourceWaveLength == NULL )
	{
		return E_POINTER;
	}
	*prtSourceWaveLength = 0;

	// Must be passed a CTrackMgr* since properties m_Item correctly does not set m_pTrackMgr
	if( pTrackMgr == NULL 
	||  pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	REFERENCE_TIME utWaveActualEnd;
	REFERENCE_TIME rtTimePhysical;
	if( SUCCEEDED ( pTrackMgr->UnknownTimeToRefTime( m_rtTimePhysical, &rtTimePhysical ) ) 
	&&  SUCCEEDED ( pTrackMgr->RefTimeToUnknownTime( rtTimePhysical + m_WaveInfo.rtWaveLength, &utWaveActualEnd ) ) )
	{
		*prtSourceWaveLength = utWaveActualEnd - m_rtTimePhysical;
		return S_OK;
	}

	ASSERT( 0 );
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::RefreshWave

HRESULT CTrackItem::RefreshWave( void )
{
	if( m_FileRef.pIDocRootNode == NULL 
	||  m_pWaveStrip == NULL )
	{
		// Nothing to do
		return S_OK;
	}

	// Get the corresponding DirectSound Wave object
	IDirectSoundWave* pIDSWave;
	if( FAILED ( m_FileRef.pIDocRootNode->GetObject( CLSID_DirectSoundWave, IID_IDirectSoundWave, (void **)&pIDSWave ) ) )
	{
		return E_UNEXPECTED;
	}

	HRESULT hr = S_OK;

	IUnknown* pIUnknown = NULL;

	// Get pIUnknown for RefreshWave method
	IDirectMusicPerformance8* pIDMPerformance8;
	if( SUCCEEDED ( m_pTrackMgr->GetObject( CLSID_DirectMusicPerformance, IID_IDirectMusicPerformance8, (void **)&pIDMPerformance8 ) ) )
	{
		IDirectMusicAudioPath* pIDMAudioPath;
		if( SUCCEEDED ( pIDMPerformance8->GetDefaultAudioPath( &pIDMAudioPath ) ) )
		{
			if( FAILED ( pIDMAudioPath->QueryInterface( IID_IUnknown, (void **)&pIUnknown ) ) )
			{
				pIUnknown = NULL;
			}

			RELEASE( pIDMAudioPath );
		}
		else
		{
			if( FAILED ( pIDMPerformance8->QueryInterface( IID_IUnknown, (void **)&pIUnknown ) ) )
			{
				pIUnknown = NULL;
			}
		}

		RELEASE( pIDMPerformance8 );
	}
	ASSERT( pIUnknown != NULL );

	if( pIUnknown )
	{
		// Get the IPrivateWaveTrack interface
		IPrivateWaveTrack* pIPrivateWaveTrack;
		if( SUCCEEDED ( m_pTrackMgr->GetObject( CLSID_DirectMusicWaveTrack, IID_IPrivateWaveTrack, (void **)&pIPrivateWaveTrack ) ) )
		{
			// Refresh the Wave
			hr = pIPrivateWaveTrack->RefreshWave( pIDSWave, pIUnknown,
												  m_pWaveStrip->GetPChannel(), m_WaveInfo.guidVersion );
			RELEASE( pIPrivateWaveTrack );
		}

		RELEASE( pIUnknown );
	}

	RELEASE( pIDSWave );
	return hr;
}
