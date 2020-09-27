/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// LyricMgr.cpp : implementation file
//

/*--------------
@doc LYRICSAMPLE
--------------*/

#include "stdafx.h"
#include <RiffStrm.h>
#include "LyricMgr.h"
#include "PropPageMgr.h"
#include <dmusicf.h>

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Set information about this strip manager
const CLSID CLyricMgr::m_clsid = CLSID_DirectMusicLyricsTrack;
const DWORD CLyricMgr::m_ckid = NULL;
const DWORD CLyricMgr::m_fccType = DMUS_FOURCC_LYRICSTRACK_LIST;

/////////////////////////////////////////////////////////////////////////////
// CLyricMgr constructor/destructor 

CLyricMgr::CLyricMgr() : CBaseMgr()
{
	// Create a LyricStrip
	m_pLyricStrip = new CLyricStrip(this);
	ASSERT( m_pLyricStrip );

	// Copy the pointer to the base strip manager
	m_pBaseStrip = m_pLyricStrip;

	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
}

CLyricMgr::~CLyricMgr()
{
	// Clean up our references
	if( m_pLyricStrip )
	{
		m_pLyricStrip->Release();
		m_pLyricStrip = NULL;
	}

	// Clear the base strip manager's pointer to the strip
	m_pBaseStrip = NULL;

	// Delete all the lyrics in m_lstLyrics
	EmptyLyricList( m_lstLyrics );
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr ILyricMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::IsMeasureBeatOpen
// Returns S_OK if the specified measure and beat is empty.
// Returns S_FALSE if the specified measure and bear already has a lyric

HRESULT STDMETHODCALLTYPE CLyricMgr::IsMeasureBeatOpen( long lMeasure, long lBeat )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Iterate through the list of lyrics
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each lyric
		CLyricItem* pLyricItem;
		pLyricItem = m_lstLyrics.GetNext( pos );

		// Since the list is sorted, if this lyric is later than the specified time, we can exit
		if( pLyricItem->m_lMeasure > lMeasure )
		{
			// The measure and beat are empty
			return S_OK;
		}
		// Check if the measure and beat values match
		else if( pLyricItem->m_lMeasure == lMeasure &&
				 pLyricItem->m_lBeat == lBeat )
		{
			// They match - return that the measure and beat are already occupied
			return S_FALSE;
		}
	}

	// The measure and beat are empty
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CLyricMgr::OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the update isn't for our strip, exit
	if( !(dwGroupBits & m_dwGroupBits) )
	{
		return E_INVALIDARG;
	}

	// All Tracks Added
	if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		// Fix measure/beat of all lyrics
		RecomputeMeasureBeats();
		SyncWithDirectMusic();

		m_pTimeline->StripInvalidateRect( m_pLyricStrip, NULL, TRUE );
		return S_OK;
	}

	// We don't handle whichever notification was passed to us
	return CBaseMgr::OnUpdate( rguidType, dwGroupBits, pData );
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CLyricMgr::SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch( stripMgrProperty )
	{
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
			return S_OK;
	}

	// Let CBaseMgr handle
	return CBaseMgr::SetStripMgrProperty( stripMgrProperty, variant );
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::Load

HRESULT CLyricMgr::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that the stream pointer is non-null
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Try and allocate a RIFF stream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// Remove all existing lyrics
	EmptyLyricList( m_lstLyrics );

	// Variables used when loading the Lyric track
	MMCKINFO ckTrack;
	MMCKINFO ckLyricList;
	MMCKINFO ckItem;
	CLyricItem* pNewItem;
	DWORD dwByteCount;

	// Interate through every chunk in the stream
	while( pIRiffStream->Descend( &ckTrack, NULL, 0 ) == 0 )
	{
		switch( ckTrack.ckid )
		{
			case FOURCC_LIST:
				switch( ckTrack.fccType )
				{
					// DX8 format
					case DMUS_FOURCC_LYRICSTRACK_LIST:
						while( pIRiffStream->Descend( &ckLyricList, &ckTrack, 0 ) == 0 )
						{
							switch( ckLyricList.ckid )
							{
								case FOURCC_LIST:
									switch( ckLyricList.fccType )
									{
										case DMUS_FOURCC_LYRICSTRACKEVENTS_LIST:
											while( pIRiffStream->Descend( &ckItem, &ckLyricList, 0 ) == 0 )
											{
												switch( ckItem.ckid )
												{
													case FOURCC_LIST:
														switch( ckItem.fccType )
														{
															case DMUS_FOURCC_LYRICSTRACKEVENT_LIST:
																hr = LoadLyricItem( pIRiffStream, &ckItem, &pNewItem );
																if( FAILED ( hr ) )
																{
																	goto ON_ERROR;
																}
																InsertByAscendingTime( pNewItem, FALSE );
																break;
														}
														break;
												}

												pIRiffStream->Ascend( &ckItem, 0 );
											}
											break;
									}
									break;
							}

							pIRiffStream->Ascend( &ckLyricList, 0 );
						}
						break;

					// DX7 format
					case FOURCC_LYRIC_LIST:
						while( pIRiffStream->Descend( &ckLyricList, &ckTrack, 0 ) == 0 )
						{
							switch( ckLyricList.ckid )
							{
								case FOURCC_LYRIC_ITEM:
								{
									DMUS_IO_LYRIC iLyric;

									// Read in a Lyric item structure
									hr = pIStream->Read( &iLyric, sizeof(DMUS_IO_LYRIC), &dwByteCount );

									// Handle any I/O error by returning a failure code
									if( FAILED( hr )
									||  dwByteCount != sizeof(DMUS_IO_LYRIC) )
									{
										hr = E_FAIL;
										goto ON_ERROR;
									}

									// Create a new item
									CLyricItem* pItem = new CLyricItem( this );
									if( pItem == NULL )
									{
										hr = E_OUTOFMEMORY;
										goto ON_ERROR;
									}

									// Read the text from the stream into pItem->m_strText
									ReadMBSfromWCS( pIStream, iLyric.dwLength, &pItem->m_strText );

									// Initialize members of CLyricItem
									pItem->m_lMeasure = iLyric.dwMeasure;
									pItem->m_lBeat = iLyric.bBeat;
									pItem->m_lTick = 0;
									pItem->m_mtTimePhysical = iLyric.mtTime;
									if( m_pTimeline )
									{
										MeasureBeatTickToClocks( pItem->m_lMeasure, pItem->m_lBeat, 0, &pItem->m_mtTimeLogical );
									}
									else
									{
										pItem->m_mtTimeLogical = iLyric.mtTime;
									}

									// Insert the item into the list, if one is not already there
									if( IsMeasureBeatOpen( pItem->m_lMeasure, pItem->m_lBeat ) == S_OK )
									{
										// Nothing exists in the beat, go ahead and insert the item
										InsertByAscendingTime( pItem, FALSE );
									}
									else
									{
										// Something already exists, delete the item and don't insert it
										delete pItem;
									}
								}
								break;
							}

							// Ascend out of the chunk in the Lyric list chunk
							pIRiffStream->Ascend( &ckLyricList, 0 );
						}
						break;

				}
				break;
		}

		pIRiffStream->Ascend( &ckTrack, 0 );
	}

	SyncWithDirectMusic();

ON_ERROR:
	// Release the RIFF stream pointer
	pIRiffStream->Release();

	// Return the success/failure status
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::LoadLyricItem

HRESULT CLyricMgr::LoadLyricItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, CLyricItem** ppItem )
{
	MMCKINFO		ck;
	DWORD			dwByteCount;
	DWORD			dwSize;
	HRESULT			hr = E_FAIL;

	if( ppItem == NULL )
	{
		return E_POINTER;
	}
	*ppItem = NULL;

	CLyricItem* pNewItem = new CLyricItem( this );
	if( pNewItem == NULL )
	{
		return E_OUTOFMEMORY;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Load the Lyric item
	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_LYRICSTRACKEVENTHEADER_CHUNK:
			{
				DMUS_IO_LYRICSTRACK_EVENTHEADER iEventHeader;

				// Read in the Lyric item's header structure
				dwSize = min( sizeof( DMUS_IO_LYRICSTRACK_EVENTHEADER ), ck.cksize );
				hr = pIStream->Read( &iEventHeader, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				pNewItem->m_dwTimingFlagsDM = iEventHeader.dwTimingFlags;
				pNewItem->m_mtTimeLogical = iEventHeader.lTimeLogical;
				pNewItem->m_mtTimePhysical = iEventHeader.lTimePhysical;
				pNewItem->SetTimePhysical( iEventHeader.lTimePhysical, STP_LOGICAL_NO_ACTION );
				break;
			}

			case DMUS_FOURCC_LYRICSTRACKEVENTTEXT_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &pNewItem->m_strText );
				break;
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
	if( pIStream )
	{
		pIStream->Release();
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppItem = pNewItem;
	}
	else
	{
		delete pNewItem;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::SaveLyricItem

HRESULT CLyricMgr::SaveLyricItem( IDMUSProdRIFFStream* pIRiffStream, CLyricItem* pItem )
{
	MMCKINFO ckItem;
	MMCKINFO ck;
	HRESULT hr;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Create the DMUS_FOURCC_LYRICSTRACKEVENT_LIST list chunk
	ckItem.fccType = DMUS_FOURCC_LYRICSTRACKEVENT_LIST;
	if( pIRiffStream->CreateChunk( &ckItem, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_LYRICSTRACKEVENTHEADER_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_LYRICSTRACKEVENTHEADER_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_LYRICSTRACK_EVENTHEADER oEventHeader;
		ZeroMemory( &oEventHeader, sizeof(DMUS_IO_LYRICSTRACK_EVENTHEADER) );

		// Fill in the members of the DMUS_IO_LYRICSTRACK_EVENTHEADER structure
		oEventHeader.dwFlags = 0;	// Reserved - must be zero
		oEventHeader.dwTimingFlags = pItem->m_dwTimingFlagsDM;		
		oEventHeader.lTimeLogical = pItem->m_mtTimeLogical;
		oEventHeader.lTimePhysical = pItem->m_mtTimePhysical;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oEventHeader, sizeof(DMUS_IO_LYRICSTRACK_EVENTHEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_LYRICSTRACK_EVENTHEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_LYRICSTRACKEVENTHEADER_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Create the DMUS_FOURCC_LYRICSTRACKEVENTTEXT_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_LYRICSTRACKEVENTTEXT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write out the Lyric text as a Wide Character String
		hr = SaveMBStoWCS( pIStream, &pItem->m_strText);
		if( FAILED( hr ) )
		{
			// Handle I/O errors by return an error code
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_LYRICSTRACKEVENTTEXT_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Ascend out of the DMUS_FOURCC_LYRICSTRACKEVENT_LIST list chunk
	if( pIRiffStream->Ascend(&ckItem, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	if( pIStream )
	{
		pIStream->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::Save

HRESULT CLyricMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Verify that the stream pointer is non-null
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

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

	// We only support saving to a DirectMusic stream (GUID_CurrentVersion) or a DirectMusic
	// stream that will be loaded into a DirectMusic Lyric track (GUID_DirectMusicObject)
	if( !::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) &&
		!::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
	{
		return E_INVALIDARG;
	}

	// Now, finally save ourself
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Allocate an IDMUSProdRIFFStream from the IStream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	// If the Lyric list isn't empty, save it
	if ( !m_lstLyrics.IsEmpty() )
	{
		// Create a LIST chunk to store the Lyric data
		MMCKINFO ckTrack;
		ckTrack.fccType = DMUS_FOURCC_LYRICSTRACK_LIST;
		if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Create a LIST chunk to store the list of items 
		MMCKINFO ckLyricList;
		ckLyricList.fccType = DMUS_FOURCC_LYRICSTRACKEVENTS_LIST;
		if( pIRiffStream->CreateChunk( &ckLyricList, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Iterate through the Lyric list
		POSITION pos = m_lstLyrics.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			CLyricItem* pLyricItem = m_lstLyrics.GetNext( pos );

			// Save each item
			hr = SaveLyricItem( pIRiffStream, pLyricItem );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}

		// Ascend out of the Lyric LIST chunk.
		pIRiffStream->Ascend( &ckLyricList, 0 );

		// Ascend out of the Lyric track LIST chunk.
		pIRiffStream->Ascend( &ckTrack, 0 );
	}

	// If we're supposed to clear our dirty flag, do so now (since the save succeeded)
	if( fClearDirty )
	{
		m_fDirty = false;
	}

ON_ERROR:
	// Release our pointer to the IDMUSProdRIFFStream
	pIRiffStream->Release();

	// Return the success/failure code
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::GetData

// This method is called by CLyricPropPageMgr to get data to send to the
// Lyric property page.
// The CLyricStrip::GetData() method is called by CGroupBitsPropPageMgr
// to get the strip's properties (currently just Group Bits)
HRESULT STDMETHODCALLTYPE CLyricMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate the ppData pointer
	if ( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Flag set to TRUE if more than one lyric is selected
	BOOL fMultipleSelect = FALSE;

	// Initialize the pointer to the first selected lyric to NULL
	CLyricItem* pFirstSelectedLyricItem = NULL;

	// Start iterating through the lyric list
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each lyric
		CLyricItem* pLyricItem;
		pLyricItem = m_lstLyrics.GetNext( pos );

		// Check if the lyric is selected
		if ( pLyricItem->m_fSelected )
		{
			// Lyric is selected - save a pointer to it in pFirstSelectedLyricItem
			pFirstSelectedLyricItem = pLyricItem;

			// Now, continue through the list to see if any other lyric are selected
			while( pos )
			{
				// Get a pointer to each lyric
				pLyricItem = m_lstLyrics.GetNext( pos );

				// Check if the lyric is selected
				if ( pLyricItem->m_fSelected )
				{
					// More than one lyric is selected - set fMultipleSelect to TRUE
					// and break out of this loop
					fMultipleSelect = TRUE;
					break;
				}
			}

			// Found a selected lyric - break out of the main while loop
			break;
		}
	}

	// If at least one lyric is selected
	if( pFirstSelectedLyricItem )
	{
		// Copy the first selected lyric to a CLyricItem class
		m_SelectedLyricItem.Copy( pFirstSelectedLyricItem );

		// If more than one lyric was selected, set UD_MULTIPLESELECT
		if( fMultipleSelect )
		{
			m_SelectedLyricItem.m_dwBitsUI |= UD_MULTIPLESELECT;
		}
		else
		{
			// Only one lyric selected, clear UD_MULTIPLESELECT
			m_SelectedLyricItem.m_dwBitsUI &= ~UD_MULTIPLESELECT;
		}

		// Set the passed in pointer to point to the CLyricItem class
		*ppData = &m_SelectedLyricItem;

		// Return a success code
		return S_OK;
	}

	// Nothing selected, so clear the passed in pointer
	*ppData = NULL;

	// Return a success code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::SetData

// This method is called by CLyricPropPageMgr in response to user actions
// in the Lyric Property page.  It changes the currenly selected Lyric. 
HRESULT STDMETHODCALLTYPE CLyricMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate the pData pointer
	if ( pData == NULL )
	{
		return E_POINTER;
	}

	// Get a pointer to the first selected lyric.
	CLyricItem* pLyricItem = FirstSelectedLyric();

	// If a selected lyric was found
	if ( pLyricItem )
	{
		// Convert the passed-in pointer to a CLyricItem*
		CLyricItem* pNewLyric = static_cast<CLyricItem*>(pData);

		// m_mtTimePhysical
		if( (pNewLyric->m_lTick != pLyricItem->m_lTick)
		||  (pNewLyric->m_lBeat != pLyricItem->m_lBeat) 
		||  (pNewLyric->m_lMeasure != pLyricItem->m_lMeasure) )
		{
			MUSIC_TIME mtNewTimePhysical;
			if( SUCCEEDED ( ForceBoundaries( pNewLyric->m_lMeasure, pNewLyric->m_lBeat, pNewLyric->m_lTick, &mtNewTimePhysical ) ) )
			{
				if( mtNewTimePhysical != pLyricItem->m_mtTimePhysical )
				{
					m_nLastEdit = IDS_UNDO_MOVE;
					pLyricItem->SetTimePhysical( mtNewTimePhysical, STP_LOGICAL_ADJUST );

					// Remove the LyricItem from the list
					CLyricItem* pLyricTmp;
					POSITION pos2, pos1 = m_lstLyrics.GetHeadPosition();
					while( pos1 )
					{
						pos2 = pos1;
						pLyricTmp = m_lstLyrics.GetNext( pos1 );
						if ( pLyricTmp == pLyricItem )
						{
							m_lstLyrics.RemoveAt( pos2 );
							break;
						}
					}

					// Re-add the lyric at its new position - this will overwrite any existing
					// lyric at this position
					InsertByAscendingTime( pLyricItem, FALSE );

					// Clear all selections
					m_pLyricStrip->m_pSelectedRegions->Clear();

					// Select just the changed lyric
					CListSelectedRegion_AddRegion(*m_pLyricStrip->m_pSelectedRegions, *pLyricItem);
				}
			}
		}

		// m_mtTimeLogical
		else if( pNewLyric->m_lLogicalMeasure != pLyricItem->m_lLogicalMeasure
			 ||  pNewLyric->m_lLogicalBeat != pLyricItem->m_lLogicalBeat )
		{
			MUSIC_TIME mtNewTimeLogical;
			if( SUCCEEDED ( ForceBoundaries( pNewLyric->m_lLogicalMeasure, pNewLyric->m_lLogicalBeat, 0, &mtNewTimeLogical ) ) )
			{
				if( mtNewTimeLogical != pLyricItem->m_mtTimeLogical )
				{
					m_nLastEdit = IDS_UNDO_LOGICAL_TIME;
					pLyricItem->SetTimeLogical( mtNewTimeLogical );
				}
			}
		}

		// m_strText
		else if( pNewLyric->m_strText != pLyricItem->m_strText )
		{
			// Update the lyric's text
			pLyricItem->m_strText = pNewLyric->m_strText;

			// We just changed the lyric
			m_nLastEdit = IDS_UNDO_CHANGE;
		}

		// m_dwTimingFlagsDM
		else if( pNewLyric->m_dwTimingFlagsDM != pLyricItem->m_dwTimingFlagsDM )
		{
			// Update the item's flags
			pLyricItem->m_dwTimingFlagsDM = pNewLyric->m_dwTimingFlagsDM;

			// We just changed the item
			m_nLastEdit = IDS_UNDO_CHANGE_TIMING;
		}

		// Nothing changed
		else
		{
			// Return a success code saying that nothing happened
			return S_FALSE;
		}

		// Redraw the lyric strip
		m_pTimeline->StripInvalidateRect( m_pLyricStrip, NULL, TRUE );

		// Let our hosting editor know about the changes
		OnDataChanged();

		// Refresh the property page with new values
		m_pPropPageMgr->RefreshData();

		// Sync track with DirectMusic
		SyncWithDirectMusic();

		return S_OK;
	}
	// No lyrics selected - nothing to change
	else
	{
		// Return a success code saying that nothing happened
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CLyricMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		// Create a new Lyric property page manager
		CLyricPropPageMgr* pPPM = new CLyricPropPageMgr( this );

		// Verify that we're not out of memory
		if( pPPM == NULL )
		{
			return E_OUTOFMEMORY;
		}

		// Get the IDMUSProdPropPageManager interface from the property page
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		
		// Release the reference created by the contructor, leaving the one created by QueryInterface.
		// If QueryInterface failed, this will delete m_pPropPageMgr.
		m_pPropPageMgr->Release();

		// If we failed, return a failure code
		if( FAILED(hr) )
		{
			return hr;
		}
	}

	// Set the displayed property page to our property page
	m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);

	// Return a success code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::DeleteSelectedLyrics

void CLyricMgr::DeleteSelectedLyrics()
{
	// Start iterating through the list of lyrics
	POSITION pos1 = m_lstLyrics.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current lyric
		CLyricItem* pLyricItem = m_lstLyrics.GetNext( pos1 );

		// Check if the current lyric is selected
		if ( pLyricItem->m_fSelected )
		{
			// This lyric is selected, remove it from the list
			m_lstLyrics.RemoveAt( pos2 );

			// Now, delete this lyric
			delete pLyricItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::SaveSelectedLyrics

HRESULT CLyricMgr::SaveSelectedLyrics(LPSTREAM pIStream, CLyricItem* pLyricAtDragPoint)
{
	// if pLyricAtDragPoint is valid, set mtOffset and dwMeasureOffset so that the time pLyricAtDragPoint is 0.
	// if pLyricAtDragPoint is NULL, set mtOffset and dwMeasureOffset so that the time of the first Lyric is 0.

	// Verify pIStream is valid
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	// If the Lyric list has anything in it, look for selected Lyrics
	if ( !m_lstLyrics.IsEmpty() )
	{
		// Initialize the beat offset to an invalid value
		long lBeatOffset = -1;
		
		// If pLyricAtDragPoint is valid, just use the measure and beat information from it
		if( pLyricAtDragPoint )
		{
			// Verify that this lyric is selected
			ASSERT( pLyricAtDragPoint->m_fSelected );

			// Compute how many beats from the start it is
			MeasureBeatToBeats( m_pTimeline, m_dwGroupBits, 0, pLyricAtDragPoint->m_lMeasure, pLyricAtDragPoint->m_lBeat, lBeatOffset );
		}
		// Otherwise look for the first selected lyric
		else
		{
			POSITION pos = m_lstLyrics.GetHeadPosition();
			while( pos )
			{
				CLyricItem* pLyricItem = m_lstLyrics.GetNext( pos );
				if ( pLyricItem->m_fSelected )
				{
					// Found a selected lyric - compute how many beats from the start it is
					MeasureBeatToBeats( m_pTimeline, m_dwGroupBits, 0, pLyricItem->m_lMeasure, pLyricItem->m_lBeat, lBeatOffset );
					break;
				}
			}
		}

		// pLyricAtDragPoint is NULL and there are no selected lyrics - return with S_FALSE
		if ( lBeatOffset == -1 )
		{
			return S_FALSE;
		}

		// Now, actually save the lyrics
		return SaveSelectedLyrics( pIStream, lBeatOffset );
	}
	else
	{
		return S_FALSE; // Nothing in the list
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::MarkSelectedLyrics

// ORs dwFlags with the m_dwBitsUI of each selected lyric item
void CLyricMgr::MarkSelectedLyrics( DWORD dwFlags )
{
	// Iterate through the list of lyrics
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current lyric
		CLyricItem* pLyricItem = m_lstLyrics.GetNext( pos );

		// Check if the lyric is selected
		if ( pLyricItem->m_fSelected )
		{
			// It's selected - update m_dwBitsUI
			pLyricItem->m_dwBitsUI |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::DeleteMarked

// deletes lyrics marked by given flag
void CLyricMgr::DeleteMarked( DWORD dwFlags )
{
	// Iterate through the list of lyrics
	POSITION pos1 = m_lstLyrics.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current lyric
		CLyricItem* pLyricItem = m_lstLyrics.GetNext( pos1 );

		// Check if any of the specified dwFlags are set in this lyric
		if ( pLyricItem->m_dwBitsUI & dwFlags )
		{
			// At least one of the flags in dwFlags is set, remove the lyric
			m_lstLyrics.RemoveAt( pos2 );

			// Now, delete it
			delete pLyricItem;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::UnMarkLyrics

// unmarks flag m_dwUndermined field CLyricItems in list
void CLyricMgr::UnMarkLyrics( DWORD dwFlags )
{
	// Iterate through the list of lyrics
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Clear the selected flags in each lyric
		m_lstLyrics.GetNext( pos )->m_dwBitsUI &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::UnselectAll

void CLyricMgr::UnselectAll()
{
	// Iterate through the list of lyrics
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Clear the selection flag for each lyric
		m_lstLyrics.GetNext( pos )->m_fSelected = FALSE;
	}

	// Clear the list of selected regions
	m_pLyricStrip->m_pSelectedRegions->Clear();
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::SelectAll

void CLyricMgr::SelectAll()
{
	// Iterate through the list of lyrics
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Set the selection flag for each lyric
		m_lstLyrics.GetNext( pos )->m_fSelected = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::FirstSelectedLyric

CLyricItem* CLyricMgr::FirstSelectedLyric()
{
	// Iterate through the list of lyrics
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to this lyic
		CLyricItem* pLyricItem = m_lstLyrics.GetNext( pos );

		// Check if the lyric is selected
		if ( pLyricItem->m_fSelected )
		{
			// Lyric is selected, return a pointer to it
			return pLyricItem;
		}
	}

	// No lyrics are selected, return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::InsertByAscendingTime

void CLyricMgr::InsertByAscendingTime( CLyricItem *pLyricToInsert, BOOL fPaste )
{
	// Ensure the pLyricToInsert pointer is valid
	if( pLyricToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	CLyricItem* pItem;
	POSITION posCurrent, posNext = m_lstLyrics.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pItem = m_lstLyrics.GetNext( posNext );

		if( fPaste )
		{
			if( pItem->m_mtTimePhysical == pLyricToInsert->m_mtTimePhysical )
			{
				// Replace item
				m_lstLyrics.InsertBefore( posCurrent, pLyricToInsert );
				m_lstLyrics.RemoveAt( posCurrent );
				delete pItem;
				return;
			}
		}

		if( pItem->m_mtTimePhysical > pLyricToInsert->m_mtTimePhysical )
		{
			// insert before posCurrent (which is the position of pItem)
			m_lstLyrics.InsertBefore( posCurrent, pLyricToInsert );
			return;
		}
	}

	// pLyricToInsert is later than all items in the list, add it at the end of the list
	m_lstLyrics.AddTail( pLyricToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::RemoveItem

bool CLyricMgr::RemoveItem( CLyricItem* pItem )
{
	// Find the given item
	POSITION posToRemove = m_lstLyrics.Find( pItem, NULL );

	// If item wasn't found, return false
	if( posToRemove == NULL )
	{
		return false;
	}

	// Remove the item from the list - the caller must delete it
	m_lstLyrics.RemoveAt( posToRemove );

	// Return true since we found the item
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::RecomputeMeasureBeats

void CLyricMgr::RecomputeMeasureBeats()
{
	// Recompute measure/beat of all items
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		CLyricItem* pItem = m_lstLyrics.GetNext( pos );

		if( ((pItem->m_lMeasure > 0) || (pItem->m_lBeat > 0))
		&&  pItem->m_lTick == 0 
		&&  pItem->m_mtTimePhysical == 0 )
		{
			// Should only happen when loading pre-release DX7 files
			MeasureBeatTickToClocks( pItem->m_lMeasure, pItem->m_lBeat, pItem->m_lTick, &pItem->m_mtTimePhysical );  
		}

		pItem->SetTimePhysical( pItem->m_mtTimePhysical, STP_LOGICAL_ADJUST );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::GetNextGreatestUniqueTime

MUSIC_TIME CLyricMgr::GetNextGreatestUniqueTime( long lMeasure, long lBeat, long lTick )
{
	DMUS_TIMESIGNATURE dmTimeSig;
	MUSIC_TIME mtTime;

	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		CLyricItem* pItem = m_lstLyrics.GetNext( pos );

		if( pItem->m_lMeasure > lMeasure )
		{
			break;
		}

		MeasureBeatTickToClocks( pItem->m_lMeasure, 0, 0, &mtTime );
		m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, mtTime, NULL, &dmTimeSig );
		if( pItem->m_lBeat > dmTimeSig.bBeatsPerMeasure )
		{
			break;
		}

		if( pItem->m_lMeasure == lMeasure
		&&  pItem->m_lBeat == lBeat )
		{
			lTick = pItem->m_lTick + 1;

			// Take care of measure/beat rollover
			MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );  
			ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick );  
		}
	}

	MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );  
	return mtTime;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::RecomputeTimes

bool CLyricMgr::RecomputeTimes()
{
	MUSIC_TIME mtTime;
	long lMeasure;
	long lBeat;
	long lTick;

	bool fChanged = false;

	// Iterate through the list of lyrics
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while( pos )
	{
		// Save our current position
		POSITION pos2 = pos;

		// Get a pointer to the current lyric
		CLyricItem* pLyricItem = m_lstLyrics.GetNext( pos );

		// Make sure measure and beat are valid
		if( pLyricItem->m_lMeasure >= 0
		&&  pLyricItem->m_lBeat >= 0 )
		{
			// Using the lyric's current measure and beat settings, determine which measure and beat
			// the lyric will end up on
			MeasureBeatTickToClocks( pLyricItem->m_lMeasure, pLyricItem->m_lBeat, pLyricItem->m_lTick, &mtTime );  
			ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick ); 

			// Check if either the measure, beat or tick of the lyric changed
			if( pLyricItem->m_mtTimePhysical != mtTime
			||	pLyricItem->m_lMeasure != lMeasure
			||	pLyricItem->m_lBeat != lBeat
			||	pLyricItem->m_lTick != lTick )
			{
				// Remove the lyric from the list of lyrics
				m_lstLyrics.RemoveAt( pos2 );

				// The measure the lyric is in changed
				if( pLyricItem->m_lMeasure != lMeasure )
				{
					// This would happen when moving from 7/4 to 4/4, for example
					// Lyrics on beat 7 would end up on next measure's beat 3
					while( pLyricItem->m_lMeasure != lMeasure )
					{
						// Keep moving back a beat until the measure does not change
						MeasureBeatTickToClocks( lMeasure, --lBeat, lTick, &mtTime );  
						ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick ); 
					}
				}

				mtTime = GetNextGreatestUniqueTime( lMeasure, lBeat, lTick ); 
				pLyricItem->SetTimePhysical( mtTime, STP_LOGICAL_ADJUST );
				fChanged = TRUE;

				InsertByAscendingTime( pLyricItem, FALSE );
			}
			else
			{
				// Recompute logical time measure/beat
				pLyricItem->SetTimeLogical( pLyricItem->m_mtTimeLogical );
			}
		}
	}

	// Return whether or not anything changed
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::DeleteBetweenMeasureBeats

bool CLyricMgr::DeleteBetweenMeasureBeats(long lmStart, long lbStart, long lmEnd, long lbEnd )
{
	// Initially, nothing changed
	bool fChanged = false;

	// Iterate through the list of lyrics
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while(pos)
	{
		// Save current position
		POSITION posTemp = pos;

		// Get a pointer to the current lyric
		CLyricItem* pItem = m_lstLyrics.GetNext(pos);

		// Check if the item's measure value is before the start measure
		if( pItem->m_lMeasure < lmStart )
		{
			// Too early - keep looking
			continue;
		}

		// Check if the item's measure value is after the end measure
		if( pItem->m_lMeasure > lmEnd )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// If the item is in the start measure, check if it is before the start beat
		if( ( pItem->m_lMeasure == lmStart )
		&&	( pItem->m_lBeat < lbStart ) )
		{
			// Too early - keep looking
			continue;
		}

		// If the item is in the end measure, check if it is after the end beat
		if( ( pItem->m_lMeasure == lmEnd)
		&&	( pItem->m_lBeat > lbEnd ) )
		{
			// Too late - done looking (since the list is sorted)
			break;
		}

		// Within the given range, delete the item
		m_lstLyrics.RemoveAt(posTemp);
		delete pItem;
		fChanged = true;
	}

	// Return whether or not anything changed
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::CycleItems

HRESULT CLyricMgr::CycleItems( long lXPos  )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pTimeline != NULL );
	if( m_pTimeline == NULL )
	{
		return E_UNEXPECTED;
	}

	long lMeasure;
	long lBeat;
	HRESULT hr = m_pTimeline->PositionToMeasureBeat( m_dwGroupBits, 0, lXPos, &lMeasure, &lBeat );

	if( SUCCEEDED ( hr ) ) 
	{
		CLyricItem* pFirstItem = NULL;
		CLyricItem* pSecondItem = NULL;
		CLyricItem* pItem;

		hr = E_FAIL;

		POSITION pos = m_lstLyrics.GetHeadPosition();
		while( pos )
		{
			pItem = m_lstLyrics.GetNext( pos );

			if( pItem->m_lMeasure == lMeasure
			&&  pItem->m_lBeat == lBeat )
			{
				if( pFirstItem == NULL )
				{
					pFirstItem = pItem;
				}
				else if( pSecondItem == NULL )
				{
					pSecondItem = pItem;
				}

				if( pItem->m_wFlagsUI & RF_TOP_ITEM )
				{
					if( pos )
					{
						// Cycle to next Routine if on same measure/beat
						CLyricItem* pNextItem = m_lstLyrics.GetNext( pos );

						if( pNextItem->m_lMeasure == lMeasure
						&&  pNextItem->m_lBeat == lBeat )
						{
							UnselectAll();
							pItem->m_wFlagsUI &= ~RF_TOP_ITEM;
							pNextItem->m_fSelected = TRUE;
							pNextItem->m_wFlagsUI |= RF_TOP_ITEM;
							hr = S_OK;
							break;
						}
					}

					// Cycle to first Routine on same measure/beat
					UnselectAll();
					pItem->m_wFlagsUI &= ~RF_TOP_ITEM;
					pFirstItem->m_fSelected = TRUE;
					pFirstItem->m_wFlagsUI |= RF_TOP_ITEM;
					hr = S_OK;
					break;
				}
			}

			if( pItem->m_lMeasure > lMeasure
			||  pos == NULL )
			{
				UnselectAll();
				if( pSecondItem )
				{
					pSecondItem->m_fSelected = TRUE;
					pSecondItem->m_wFlagsUI |= RF_TOP_ITEM;
					hr = S_OK;
				}
				else if( pFirstItem )
				{
					pFirstItem->m_fSelected = TRUE;
					pFirstItem->m_wFlagsUI |= RF_TOP_ITEM;
					hr = S_OK;
				}
				break;
			}
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		// Update the selection regions to include just this selected item
		m_pLyricStrip->SelectRegionsFromSelectedLyrics();

		// Redraw the Lyric strip
		m_pTimeline->StripInvalidateRect( m_pLyricStrip, NULL, TRUE );

		// Update the property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::SaveSelectedLyrics

HRESULT CLyricMgr::SaveSelectedLyrics( IStream* pStream, long lBeatOffset )
// save selected lyrics and adjust by "offset"
{
	// Verify that the pStream pointer is valid
	if(pStream == NULL)
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Create a list to store the lyrics to save in
	CTypedPtrList<CPtrList, CLyricItem*> lstLyricsToSave;

	// Iterate throught the list of lyrics
	POSITION pos = m_lstLyrics.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each lyric
		CLyricItem* pLyric = m_lstLyrics.GetNext(pos);

		// Check if the lyric is selected
		if( pLyric->m_fSelected )
		{
			// Add the lyric to the list of lyrics to save
			lstLyricsToSave.AddTail(new CLyricItem(*pLyric));
		}
	}


	//check that anything is selected
	if( lstLyricsToSave.IsEmpty() )
	{
		return S_FALSE;
	}

	// For each lyric in lstLyricsToSave, convert to a number of beats and subtract lBeatOffset
	NormalizeLyricList( m_pTimeline, m_dwGroupBits, lstLyricsToSave, lBeatOffset );

	// Save the list of lyrics into pStream
	HRESULT hr = SaveLyricList( lstLyricsToSave, pStream );

	// Empty the temporary list of lyrics
	EmptyLyricList(lstLyricsToSave);

	// Return whether or not the save succeeded
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// NormalizeLyricList

void NormalizeLyricList( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, CTypedPtrList<CPtrList, CLyricItem*>& list, long lBeatOffset )
{
	long lBeat;

	// Iterate through the list of lyrics
	POSITION pos = list.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each lyric
		CLyricItem* pLyric = list.GetNext( pos );

		// This method only called for drag/drop and cut/copy/paste
		// so it is safe to mess with the values that are stored in time fields

		// Use m_mtTimePhysical to store beat offset 
		MeasureBeatToBeats( pITimeline, dwGroupBits, 0, pLyric->m_lMeasure, pLyric->m_lBeat, lBeat );
		pLyric->m_mtTimePhysical = lBeat - lBeatOffset;

		// Use m_mtTimeLogical to store beat offset 
		MeasureBeatToBeats( pITimeline, dwGroupBits, 0, pLyric->m_lLogicalMeasure, pLyric->m_lLogicalBeat, lBeat );
		pLyric->m_mtTimeLogical = lBeat - lBeatOffset;
	}
}


/////////////////////////////////////////////////////////////////////////////
// SaveListItem

HRESULT SaveListItem( IDMUSProdRIFFStream* pIRiffStream, CLyricItem* pItem )
{
	MMCKINFO ckItem;
	MMCKINFO ck;
	HRESULT hr;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Create the DMUS_FOURCC_LYRICSTRACKEVENT_LIST list chunk
	ckItem.fccType = DMUS_FOURCC_LYRICSTRACKEVENT_LIST;
	if( pIRiffStream->CreateChunk( &ckItem, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Create the DMUS_FOURCC_LYRICSTRACKEVENTHEADER_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_LYRICSTRACKEVENTHEADER_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_LYRICSTRACK_EVENTHEADER oEventHeader;
		ZeroMemory( &oEventHeader, sizeof(DMUS_IO_LYRICSTRACK_EVENTHEADER) );

		// Fill in the members of the DMUS_IO_LYRICSTRACK_EVENTHEADER structure
		oEventHeader.dwFlags = 0;	// Reserved - must be zero
		oEventHeader.dwTimingFlags = pItem->m_dwTimingFlagsDM;		
		oEventHeader.lTimePhysical = pItem->m_mtTimePhysical;
		oEventHeader.lTimeLogical = pItem->m_mtTimeLogical;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oEventHeader, sizeof(DMUS_IO_LYRICSTRACK_EVENTHEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_LYRICSTRACK_EVENTHEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_LYRICSTRACKEVENTHEADER_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Create the DMUS_FOURCC_COPYPASTE_UI_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_COPYPASTE_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		ioCopyPasteUI oCopyPasteUI;
		ZeroMemory( &oCopyPasteUI, sizeof(ioCopyPasteUI) );

		// Fill in the members of the DMUS_IO_LYRICSTRACK_EVENTHEADER structure
		oCopyPasteUI.lTick = pItem->m_lTick;	// Need to save tick offset

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oCopyPasteUI, sizeof(ioCopyPasteUI), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(ioCopyPasteUI) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_COPYPASTE_UI_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Create the DMUS_FOURCC_LYRICSTRACKEVENTTEXT_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_LYRICSTRACKEVENTTEXT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write out the Lyric text as a Wide Character String
		hr = SaveMBStoWCS( pIStream, &pItem->m_strText );
		if( FAILED( hr ) )
		{
			// Handle I/O errors by return an error code
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_LYRICSTRACKEVENTTEXT_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Ascend out of the DMUS_FOURCC_LYRICSTRACKEVENT_LIST list chunk
	if( pIRiffStream->Ascend(&ckItem, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	if( pIStream )
	{
		pIStream->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// SaveLyricList

HRESULT SaveLyricList( CTypedPtrList<CPtrList, CLyricItem*>& list, IStream* pIStream )
{
	// Try and allocate an IDMUSProdRIFFStream from the IStream pointer
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		// Couldn't allocate an IDMUSProdRIFFStream, return a failure code
		return hr;
	}

	// If the Lyrics list isn't empty, save it
	if( !list.IsEmpty() )
	{
		// Create a LIST chunk to store the Lyrics track data
		MMCKINFO ckTrack;
		ckTrack.fccType = DMUS_FOURCC_LYRICSTRACK_LIST;
		if( pIRiffStream->CreateChunk( &ckTrack, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Create a LIST chunk to store the list of items 
		MMCKINFO ckLyricList;
		ckLyricList.fccType = DMUS_FOURCC_LYRICSTRACKEVENTS_LIST;
		if( pIRiffStream->CreateChunk( &ckLyricList, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Iterate through the Lyric list
		POSITION pos = list.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			CLyricItem* pItem = list.GetNext( pos );

			// Save each item
			hr = SaveListItem( pIRiffStream, pItem );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
		
		// Ascend out of the Lyric LIST chunk.
		pIRiffStream->Ascend( &ckLyricList, 0 );

		// Ascend out of the Lyrics track LIST chunk.
		pIRiffStream->Ascend( &ckTrack, 0 );
	}

ON_ERROR:
	// Release the IDMUSProdRIFFStream interface
	pIRiffStream->Release();

	// Return the success or failure code
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// LoadListItem

HRESULT LoadListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent,
					  CLyricMgr* pLyricMgr, CLyricItem** ppItem )
{
	MMCKINFO		ck;
	DWORD			dwByteCount;
	DWORD			dwSize;
	HRESULT			hr = E_FAIL;

	if( ppItem == NULL )
	{
		return E_POINTER;
	}
	*ppItem = NULL;

	CLyricItem* pNewItem = new CLyricItem( pLyricMgr );
	if( pNewItem == NULL )
	{
		return E_OUTOFMEMORY;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Load the Lyric item
	while( pIRiffStream->Descend( &ck, pckParent, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_LYRICSTRACKEVENTHEADER_CHUNK:
			{
				DMUS_IO_LYRICSTRACK_EVENTHEADER iEventHeader;

				// Read in the Lyric item's header structure
				dwSize = min( sizeof( DMUS_IO_LYRICSTRACK_EVENTHEADER ), ck.cksize );
				hr = pIStream->Read( &iEventHeader, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				pNewItem->m_dwTimingFlagsDM = iEventHeader.dwTimingFlags;

				pNewItem->m_mtTimePhysical = iEventHeader.lTimePhysical;	// lTimePhysical stores beat offset
				pNewItem->m_mtTimeLogical = iEventHeader.lTimeLogical;		// lTimeLogical stores beat offset
				
				// Will recalc mtTime fields after paste (or drop)
				pNewItem->m_lMeasure = 0;
				pNewItem->m_lBeat = 0;
				pNewItem->m_lTick = 0;		// DMUS_FOURCC_COPYPASTE_UI_CHUNK stores tick offset
				break;
			}

			case DMUS_FOURCC_COPYPASTE_UI_CHUNK:
			{
				ioCopyPasteUI iCopyPasteUI;

				// Read in the Lyric item's copy/paste structure
				dwSize = min( sizeof( ioCopyPasteUI ), ck.cksize );
				hr = pIStream->Read( &iCopyPasteUI, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				pNewItem->m_lTick = iCopyPasteUI.lTick;
				break;
			}

			case DMUS_FOURCC_LYRICSTRACKEVENTTEXT_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &pNewItem->m_strText );
				break;
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
	if( pIStream )
	{
		pIStream->Release();
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppItem = pNewItem;
	}
	else
	{
		delete pNewItem;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// LoadLyricList

HRESULT LoadLyricList( CTypedPtrList<CPtrList, CLyricItem*>& list, IStream* pIStream, CLyricMgr* pLyricMgr )
{
	// Verify that the stream pointer is non-null
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	// Try and allocate an IDMUSProdRIFFStream interface from the pIStream
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr;
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		// Couldn't allocate an IDMUSProdRIFFStream interface, return an error code
		return hr;
	}

	// Variables used when loading the Lyric track
	MMCKINFO ckTrack;
	MMCKINFO ckLyricList;
	MMCKINFO ckItem;
	CLyricItem* pNewItem;

	// Descend into the Lyric LIST chunk
	ckTrack.fccType = DMUS_FOURCC_LYRICSTRACK_LIST;
	if( pIRiffStream->Descend(&ckTrack, NULL, MMIO_FINDLIST) == 0)
	{
		// Now, descend into each chunk in this LIST chunk
		while( pIRiffStream->Descend( &ckLyricList, &ckTrack, 0 ) == 0 )
		{
			switch( ckLyricList.ckid )
			{
				case FOURCC_LIST:
					switch( ckLyricList.fccType )
					{
						case DMUS_FOURCC_LYRICSTRACKEVENTS_LIST:
							while( pIRiffStream->Descend( &ckItem, &ckLyricList, 0 ) == 0 )
							{
								switch( ckItem.ckid )
								{
									case FOURCC_LIST:
										switch( ckItem.fccType )
										{
											case DMUS_FOURCC_LYRICSTRACKEVENT_LIST:
												hr = LoadListItem( pIRiffStream, &ckItem, pLyricMgr, &pNewItem );
												if( FAILED ( hr ) )
												{
													goto ON_ERROR;
												}
												list.AddTail( pNewItem );
												break;
										}
										break;
								}

								pIRiffStream->Ascend( &ckItem, 0 );
							}
							break;
					}
					break;
			}

			pIRiffStream->Ascend( &ckLyricList, 0 );
		}

		pIRiffStream->Ascend( &ckTrack, 0 );
	}

ON_ERROR:
	// Release the IDMUSProdRIFFStream interface
	pIRiffStream->Release();

	// Return the success or failure code
    return hr;
}

void EmptyLyricList( CTypedPtrList<CPtrList, CLyricItem*>& list )
{
	// Remove and delete all the CLyricItems from the given list
	while( !list.IsEmpty() )
	{
		delete list.RemoveHead();
	}
}

HRESULT GetBoundariesOfLyrics( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, long &lStartTime, long &lEndTime, CTypedPtrList<CPtrList, CLyricItem*>& list)
{
	// Validate the given pTimeline pointer
	if( pTimeline == NULL )
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Initialize the start and end times to -1
	lStartTime = -1;
	lEndTime = -1;

	// No lyrics in list, return S_FALSE since there's nothing to do
	if( list.IsEmpty() )
	{
		return S_FALSE;
	}

	// Initialize our return value to S_OK
	HRESULT hr = S_OK;
	MUSIC_TIME mtTime;

	// Iterate through the list of lyrics
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each lyric item
		CLyricItem* pItem = list.GetNext(pos);

		// Try and convert the measure and beat of the lyric to a value in number of clocks
		hr = pTimeline->MeasureBeatToClocks( dwGroupBits, 0, pItem->m_lMeasure, pItem->m_lBeat, &mtTime );

		// If the conversion failed, break out of the while loop
		if(FAILED(hr))
		{
			break;
		}

		// If the start time is not yet set, or the item's time is earlier
		// than any other lyric, update lStartTime.
		if( ( lStartTime == -1 )
		||	( mtTime < lStartTime ) )
		{
			lStartTime = mtTime;
		}

		// If the end time is not yet set, or the item's time is later
		// than any other lyric, update lEnd.
		if( ( lEndTime == -1 )
		||	( mtTime > lEndTime ) )
		{
			lEndTime = mtTime;
		}
	}

	// If the conversions (MeasureBeatToClocks) succeeded
	if( SUCCEEDED(hr) )
	{
		// The start and end times should be set to valid values
		ASSERT( lStartTime != -1 );
		ASSERT( lEndTime != -1 );

		// Get the measure and beat of the last lyric
		long lMeasure, lBeat;
		CMusicTimeConverter cmt = lEndTime;
		hr = cmt.GetMeasureBeat( lMeasure, lBeat, pTimeline, dwGroupBits );

		// If the conversion failed, return the failure code
		if( FAILED( hr ) )
		{
			return hr;
		}

		// Increment the beat value
		lBeat++;

		// Convert from measure and beat back to clocks
		hr = cmt.SetTime( lMeasure, lBeat, pTimeline, dwGroupBits );

		// If the conversion failed, return the failure code
		if( FAILED( hr ) )
		{
			return hr;
		}

		// Set the end time to one tick before the next beat
		lEndTime = cmt - 1;
	}

	// Return the success or failure code
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::ClocksToMeasureBeatTick

HRESULT CLyricMgr::ClocksToMeasureBeatTick( MUSIC_TIME mtTime,
										    long* plMeasure, long* plBeat, long* plTick )
{
	ASSERT( plMeasure != NULL );
	ASSERT( plBeat != NULL );
	ASSERT( plTick != NULL );
	
	long lMeasure = -1;
	long lBeat = -1;
	long lTick = -1;

	HRESULT hr = S_OK;

	if( m_pTimeline == NULL )
	{
		hr = S_FALSE;
	}
	else
	{
		if( mtTime < 0 )
		{
			lMeasure = 0;
			lBeat = 0;
			lTick = mtTime;
		}
		else
		{
			hr = m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
			if( SUCCEEDED ( hr ) )
			{
				long lClocks;

				hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &lClocks );
				if( SUCCEEDED ( hr ) )
				{
					lTick = mtTime - lClocks;

					// Try and preserve negative tick offsets
					if( lTick > 0 )
					{
						long lNewClocks;
						long lClocksPerBeat;

						hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat+1, &lNewClocks );
						if( SUCCEEDED ( hr ) )
						{
							lClocksPerBeat = lNewClocks - lClocks;

							if( lTick >= (lClocksPerBeat >> 1) )
							{
								// Get max clock
								VARIANT var;
								m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
								MUSIC_TIME mtMaxTimelineLength = V_I4( &var );

								if( lNewClocks < mtMaxTimelineLength )
								{
									lTick -= lClocksPerBeat;

									hr = m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, lNewClocks, &lMeasure, &lBeat );
								}
							}
						}
					}
				}
			}
		}
	}

	*plMeasure = lMeasure;
	*plBeat = lBeat;
	*plTick	= lTick;

	ASSERT( SUCCEEDED ( hr ) );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::MeasureBeatTickToClocks

HRESULT CLyricMgr::MeasureBeatTickToClocks( long lMeasure, long lBeat, long lTick,
											MUSIC_TIME* pmtTime )
{
	ASSERT( pmtTime != NULL );

	MUSIC_TIME mtTime = -1;
	long lClocks;

	HRESULT hr = S_OK;

	if( m_pTimeline == NULL )
	{
		hr = S_FALSE;
	}
	else
	{
		hr = m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, &lClocks );
		if( SUCCEEDED ( hr ) )
		{
			mtTime = lClocks + lTick;
		}
	}

	*pmtTime = mtTime;

	ASSERT( SUCCEEDED ( hr ) );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::ForceBoundaries

HRESULT CLyricMgr::ForceBoundaries( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmtTime )
{
	// Get length of segment
	MUSIC_TIME mtMaxTimelineLength;
	VARIANT var;
	m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	mtMaxTimelineLength = V_I4( &var );

	// Get maximum number of measures, beats
	long lMaxMeasure;
	long lMaxBeat;
	long lMaxTick;
	long lClocks;
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtMaxTimelineLength - 1, &lMaxMeasure, &lMaxBeat );
	lMaxMeasure = max( 0, lMaxMeasure );
	lMaxBeat = max( 0, lMaxBeat );
	MeasureBeatTickToClocks( lMaxMeasure, lMaxBeat, 0, &lClocks );
	lMaxTick = mtMaxTimelineLength - lClocks; 

	// Force boundaries 
	MUSIC_TIME mtTime;
	MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );
	ClocksToMeasureBeatTick( mtTime, &lMeasure, &lBeat, &lTick );

	// Enforce measure boundary
	if( lMeasure > lMaxMeasure )
	{
		lMeasure = lMaxMeasure;
		lBeat = lMaxBeat;
	}

	// Enforce beat boundary (must catch incomplete last measures - happens when importing MIDI files)
	if( lMeasure == lMaxMeasure 
	&&  lBeat > lMaxBeat )
	{
		lBeat = lMaxBeat;
	}

	// Enforce tick boundary (must catch incomplete last beats - happens when importing MIDI files)
	if( lMeasure == lMaxMeasure 
	&&  lBeat == lMaxBeat 
	&&  lTick > lMaxTick )
	{
		lTick = lMaxTick;
	}
	if( lTick < -MAX_TICK )
	{
		lTick = -MAX_TICK;
	}

	// Make sure mtTime matches adjusted measure, tick
	MeasureBeatTickToClocks( lMeasure, lBeat, lTick, &mtTime );
	*pmtTime = mtTime;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::GetGroupBits

DWORD CLyricMgr::GetGroupBits( void )
{
	return m_dwGroupBits;
}


/////////////////////////////////////////////////////////////////////////////
// CLyricMgr::IsRefTimeTrack

bool CLyricMgr::IsRefTimeTrack( void )
{
	return (m_dwTrackExtrasFlags & DMUS_TRACKCONFIG_PLAY_CLOCKTIME) ? true : false;
}
