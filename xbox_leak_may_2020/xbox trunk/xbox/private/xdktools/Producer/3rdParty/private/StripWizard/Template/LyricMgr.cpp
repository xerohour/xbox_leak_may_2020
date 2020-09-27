// $$Safe_root$$Mgr.cpp : implementation file
//

/*--------------
@doc $$SAFE_ROOT$$SAMPLE
--------------*/

#include "stdafx.h"
#include "$$Safe_root$$Mgr.h"
#include "PropPageMgr.h"
#include <RiffStrm.h>

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Set information about this strip manager
const CLSID C$$Safe_root$$Mgr::m_clsid = CLSID_$$Safe_root$$Track;
const DWORD C$$Safe_root$$Mgr::m_ckid = NULL;
const DWORD C$$Safe_root$$Mgr::m_fccType = FOURCC_$$SAFE_ROOT$$_LIST;

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr constructor/destructor 

C$$Safe_root$$Mgr::C$$Safe_root$$Mgr() : CBaseMgr()
{
	// Create a $$Safe_root$$Strip
	m_p$$Safe_root$$Strip = new C$$Safe_root$$Strip(this);
	ASSERT( m_p$$Safe_root$$Strip );

	// Copy the pointer to the base strip manager
	m_pBaseStrip = m_p$$Safe_root$$Strip;
}

C$$Safe_root$$Mgr::~C$$Safe_root$$Mgr()
{
	// Clean up our references
	if( m_p$$Safe_root$$Strip )
	{
		m_p$$Safe_root$$Strip->Release();
		m_p$$Safe_root$$Strip = NULL;
	}

	// Clear the base strip manager's pointer to the strip
	m_pBaseStrip = NULL;

	// Delete all the items in m_lst$$Safe_root$$s
	Empty$$Safe_root$$List( m_lst$$Safe_root$$s );
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr I$$Safe_root$$Mgr implementation

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::IsMeasureBeatOpen
// Returns S_OK if the specified measure and beat is empty.
// Returns S_FALSE if the specified measure and beat already has an item

HRESULT STDMETHODCALLTYPE C$$Safe_root$$Mgr::IsMeasureBeatOpen( long lMeasure, long lBeat )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Iterate through the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$Item;
		p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos );

		// Since the list is sorted, if this item is later than the specified time, we can exit
		if( p$$Safe_root$$Item->m_lMeasure > lMeasure )
		{
			// The measure and beat are empty
			return S_OK;
		}
		// Check if the measure and beat values match
		else if( p$$Safe_root$$Item->m_lMeasure == lMeasure &&
				 p$$Safe_root$$Item->m_lBeat == lBeat )
		{
			// They match - return that the measure and beat are already occupied
			return S_FALSE;
		}
	}

	// The measure and beat are empty
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::Load

HRESULT C$$Safe_root$$Mgr::Load( IStream* pIStream )
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

	// Remove all existing items
	Empty$$Safe_root$$List( m_lst$$Safe_root$$s );

	// Variables used when loading the $$Safe_root$$ track
	MMCKINFO	ckList;
	MMCKINFO	ck;
	DWORD		dwByteCount;
	DMUS_IO_$$SAFE_ROOT$$ i$$Safe_root$$;

	// Interate through every chunk in the stream
	while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
		case FOURCC_LIST:
			// Only look into LIST chunks
			switch( ck.fccType )
			{
			case FOURCC_$$SAFE_ROOT$$_LIST:
				// Found the $$Safe_root$$ list chunk - descend into it
				while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
				{
					switch( ckList.ckid )
					{
					case FOURCC_$$SAFE_ROOT$$_ITEM:
						// Found the $$Safe_root$$ item chunk - read it
						{
							// Read in a $$Safe_root$$ item structure
							hr = pIStream->Read( &i$$Safe_root$$, sizeof(DMUS_IO_$$SAFE_ROOT$$), &dwByteCount );

							// Handle any I/O error by returning a failure code
							if( FAILED( hr ) || dwByteCount != sizeof(DMUS_IO_$$SAFE_ROOT$$) )
							{
								hr = E_FAIL;
								goto ON_ERROR;
							}

							// Create a new item
							C$$Safe_root$$Item* pItem = new C$$Safe_root$$Item;

							// Double-check that the memory was allocated
							if ( pItem == NULL )
							{
								hr = E_OUTOFMEMORY;
								goto ON_ERROR;
							}

							// Read the text from the stream into pItem->m_strText
							ReadMBSfromWCS( pIStream, i$$Safe_root$$.dwLength, &pItem->m_strText );

							// Initialize the measure and beat members of the C$$Safe_root$$Item
							pItem->m_lMeasure = i$$Safe_root$$.dwMeasure;
							pItem->m_lBeat = i$$Safe_root$$.bBeat;

							// Insert the item into the list, if one is not already there
							if( IsMeasureBeatOpen( pItem->m_lMeasure, pItem->m_lBeat ) == S_OK )
							{
								// Nothing exists in the beat, go ahead and insert the item
								InsertByAscendingTime( pItem );
							}
							else
							{
								// Something already exists, delete the item and don't insert it
								delete pItem;
							}
						}
						break;
					}

					// Ascend out of the chunk in the $$Safe_root$$ list chunk
					pIRiffStream->Ascend( &ckList, 0 );
				}
				break;
			}
		}

		// Ascend out of the main chunk
		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
	// Release the RIFF stream pointer
	pIRiffStream->Release();

	// Return the success/failure status
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::Save

HRESULT C$$Safe_root$$Mgr::Save( IStream* pIStream, BOOL fClearDirty )
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
	// stream that will be loaded into a DirectMusic $$Safe_root$$ track (GUID_DirectMusicObject)
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

	// If the $$Safe_root$$ list isn't empty, save it
	if ( !m_lst$$Safe_root$$s.IsEmpty() )
	{
		// Create a LIST chunk to store the $$Safe_root$$ data
		MMCKINFO ckMain;
		ckMain.fccType = FOURCC_$$SAFE_ROOT$$_LIST;
		if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
		{
			// If unable to create the LIST chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Iterate through the $$Safe_root$$ list
		POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			C$$Safe_root$$Item* p$$Safe_root$$Item;
			p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos );

			// Create a chunk to store the item in
			MMCKINFO ck;
			ck.ckid = FOURCC_$$SAFE_ROOT$$_ITEM;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				// If unable to create the chunk, return E_FAIL
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Convert the Measure and Beat of each item to a MusicTime value
			MUSIC_TIME mtTime;
			if( (m_pTimeline == NULL) || FAILED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, p$$Safe_root$$Item->m_lMeasure, p$$Safe_root$$Item->m_lBeat, &mtTime ) ) )
			{
				// This shouldn't happen, but if it does, just set the time to 0
				mtTime = 0;
			}

			// Clear out the structure (clears out the padding bytes as well).
			DMUS_IO_$$SAFE_ROOT$$ o$$Safe_root$$;
			ZeroMemory( &o$$Safe_root$$, sizeof(DMUS_IO_$$SAFE_ROOT$$) );

			// Fill in the members of the DMUS_IO_$$SAFE_ROOT$$ structure
			o$$Safe_root$$.mtTime = mtTime;
			o$$Safe_root$$.dwMeasure = p$$Safe_root$$Item->m_lMeasure;
			o$$Safe_root$$.bBeat = BYTE( min(p$$Safe_root$$Item->m_lBeat, 255) ); // Ensure the beat value doesn't overflow
			o$$Safe_root$$.dwLength = p$$Safe_root$$Item->m_strText.GetLength() + 1;
			o$$Safe_root$$.dwLength *= sizeof(wchar_t);

			// Write the structure out to the stream
			DWORD dwBytesWritten;
			hr = pIStream->Write( &o$$Safe_root$$, sizeof(DMUS_IO_$$SAFE_ROOT$$), &dwBytesWritten );
			if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_$$SAFE_ROOT$$) )
			{
				// Handle I/O errors by return an error code
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Write out the $$Safe_root$$ text as a Wide Character String
			hr = SaveMBStoWCS( pIStream, &p$$Safe_root$$Item->m_strText );
			if( FAILED( hr ) )
			{
				// Handle I/O errors by return an error code
				goto ON_ERROR;
			}

			// Ascend out of the $$Safe_root$$ chunk.
			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				// Handle I/O errors by return an error code
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}
		// Ascend out of the $$Safe_root$$ LIST chunk.
		pIRiffStream->Ascend( &ckMain, 0 );
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
// C$$Safe_root$$Mgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::GetData

// This method is called by C$$Safe_root$$PropPageMgr to get data to send to the
// $$Safe_root$$ property page.
// The C$$Safe_root$$Strip::GetData() method is called by CGroupBitsPropPageMgr
// to get the strip's properties (currently just Group Bits)
HRESULT STDMETHODCALLTYPE C$$Safe_root$$Mgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate the ppData pointer
	if ( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Flag set to TRUE if more than one item is selected
	BOOL fMultipleSelect = FALSE;

	// Initialize the pointer to the first selected item to NULL
	C$$Safe_root$$Item* pFirstSelected$$Safe_root$$Item = NULL;

	// Start iterating through the item list
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$Item;
		p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos );

		// Check if the item is selected
		if ( p$$Safe_root$$Item->m_fSelected )
		{
			// $$Safe_root$$ is selected - save a pointer to it in pFirstSelected$$Safe_root$$Item
			pFirstSelected$$Safe_root$$Item = p$$Safe_root$$Item;

			// Now, continue through the list to see if any other items are selected
			while( pos )
			{
				// Get a pointer to each item
				p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos );

				// Check if the item is selected
				if ( p$$Safe_root$$Item->m_fSelected )
				{
					// More than one item is selected - set fMultipleSelect to TRUE
					// and break out of this loop
					fMultipleSelect = TRUE;
					break;
				}
			}

			// Found a selected item - break out of the main while loop
			break;
		}
	}

	// If at least one item is selected
	if( pFirstSelected$$Safe_root$$Item )
	{
		// Copy the first selected item to a C$$Safe_root$$Item class
		m_Selected$$Safe_root$$Item.Copy( pFirstSelected$$Safe_root$$Item );

		// If more than one item was selected, set UD_MULTIPLESELECT
		if( fMultipleSelect )
		{
			m_Selected$$Safe_root$$Item.m_dwBits |= UD_MULTIPLESELECT;
		}
		else
		{
			// Only one item selected, clear UD_MULTIPLESELECT
			m_Selected$$Safe_root$$Item.m_dwBits &= ~UD_MULTIPLESELECT;
		}

		// Set the passed in pointer to point to the C$$Safe_root$$Item class
		*ppData = &m_Selected$$Safe_root$$Item;

		// Return a success code
		return S_OK;
	}

	// Nothing selected, so clear the passed in pointer
	*ppData = NULL;

	// Return a success code
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::SetData

// This method is called by C$$Safe_root$$PropPageMgr in response to user actions
// in the $$Safe_root$$ Property page.  It changes the currenly selected $$Safe_root$$. 
HRESULT STDMETHODCALLTYPE C$$Safe_root$$Mgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate the pData pointer
	if ( pData == NULL )
	{
		return E_POINTER;
	}

	// Get a pointer to the first selected item.
	C$$Safe_root$$Item* p$$Safe_root$$Item = FirstSelected$$Safe_root$$();

	// If a selected item was found
	if ( p$$Safe_root$$Item )
	{
		// Convert the passed-in pointer to a C$$Safe_root$$Item*
		C$$Safe_root$$Item* pNew$$Safe_root$$ = static_cast<C$$Safe_root$$Item*>(pData);

		// Check to see if the time position of the item changed
		if( (pNew$$Safe_root$$->m_lBeat != p$$Safe_root$$Item->m_lBeat) ||
			(pNew$$Safe_root$$->m_lMeasure != p$$Safe_root$$Item->m_lMeasure) )
		{
			// Update measure and beat position
			p$$Safe_root$$Item->m_lBeat = pNew$$Safe_root$$->m_lBeat;
			p$$Safe_root$$Item->m_lMeasure = pNew$$Safe_root$$->m_lMeasure;

			// Convert measure and beat position to a time value
			MUSIC_TIME mtTime;
			if( SUCCEEDED( m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0,
								pNew$$Safe_root$$->m_lMeasure, pNew$$Safe_root$$->m_lBeat, &mtTime ) ) )
			{
				// Get the length of the segment
				VARIANT varLength;
				m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &varLength );

				// Ensure the item doesn't move past the end of the segment
				mtTime = min( mtTime, V_I4(&varLength) - 1 );

				// Convert time value to a measure and beat position
				long lBeat, lMeasure;
				if( SUCCEEDED( m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0,
								mtTime, &lMeasure, &lBeat ) ) )
				{
					if( lMeasure < 0 )
					{
						// If the measure is negative, set the measure and beat #s to 0
						p$$Safe_root$$Item->m_lMeasure = 0;
						p$$Safe_root$$Item->m_lBeat = 0;
					}
					else
					{
						// Check that the beat value is valid
						ASSERT( lBeat <= UCHAR_MAX );

						// Set the Measure and beat values
						p$$Safe_root$$Item->m_lBeat = BYTE(lBeat);
						p$$Safe_root$$Item->m_lMeasure = lMeasure;
					}
				}
			}

			// We just moved the item
			m_nLastEdit = IDS_UNDO_MOVE;

			// Remove the $$Safe_root$$Item from the list
			C$$Safe_root$$Item* p$$Safe_root$$Tmp;
			POSITION pos2, pos1 = m_lst$$Safe_root$$s.GetHeadPosition();
			while( pos1 )
			{
				pos2 = pos1;
				p$$Safe_root$$Tmp = m_lst$$Safe_root$$s.GetNext( pos1 );
				if ( p$$Safe_root$$Tmp == p$$Safe_root$$Item )
				{
					m_lst$$Safe_root$$s.RemoveAt( pos2 );
					break;
				}
			}

			// Re-add the item at its new position - this will overwrite any existing
			// item at this position
			InsertByAscendingTime( p$$Safe_root$$Item );

			// Clear all selections
			m_p$$Safe_root$$Strip->m_pSelectedRegions->Clear();

			// Select just the changed item
			CListSelectedRegion_AddRegion(*m_p$$Safe_root$$Strip->m_pSelectedRegions, *p$$Safe_root$$Item);

		}

		// Check to see if the text of the item changed
		else if( pNew$$Safe_root$$->m_strText != p$$Safe_root$$Item->m_strText )
		{
			// Update the item's text
			p$$Safe_root$$Item->m_strText = pNew$$Safe_root$$->m_strText;

			// We just changed the item
			m_nLastEdit = IDS_UNDO_CHANGE;
		}

		// Nothing changed
		else
		{
			// Return a success code saying that nothing happened
			return S_FALSE;
		}

		// Redraw the $$Safe_root$$ strip
		m_pTimeline->StripInvalidateRect( m_p$$Safe_root$$Strip, NULL, TRUE );

		// Let our hosting editor know about the changes
		OnDataChanged();

		// Refresh the property page with new values
		m_pPropPageMgr->RefreshData();

		return S_OK;
	}
	// No items selected - nothing to change
	else
	{
		// Return a success code saying that nothing happened
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::OnShowProperties

HRESULT STDMETHODCALLTYPE C$$Safe_root$$Mgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// If we don't have a property page manager yet, create one.
	if( m_pPropPageMgr == NULL )
	{
		// Create a new $$Safe_root$$ property page manager
		C$$Safe_root$$PropPageMgr* pPPM = new C$$Safe_root$$PropPageMgr;

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
// C$$Safe_root$$Mgr implementation

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::DeleteSelected$$Safe_root$$s

void C$$Safe_root$$Mgr::DeleteSelected$$Safe_root$$s()
{
	// Start iterating through the list of items
	POSITION pos1 = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos1 );

		// Check if the current item is selected
		if ( p$$Safe_root$$Item->m_fSelected )
		{
			// This item is selected, remove it from the list
			m_lst$$Safe_root$$s.RemoveAt( pos2 );

			// Now, delete this item
			delete p$$Safe_root$$Item;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::SaveSelected$$Safe_root$$s

HRESULT C$$Safe_root$$Mgr::SaveSelected$$Safe_root$$s(LPSTREAM pIStream, C$$Safe_root$$Item* p$$Safe_root$$AtDragPoint)
{
	// if p$$Safe_root$$AtDragPoint is valid, set mtOffset and dwMeasureOffset so that the time p$$Safe_root$$AtDragPoint is 0.
	// if p$$Safe_root$$AtDragPoint is NULL, set mtOffset and dwMeasureOffset so that the time of the first $$Safe_root$$ is 0.

	// Verify pIStream is valid
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	// If the $$Safe_root$$ list has anything in it, look for selected $$Safe_root$$s
	if ( !m_lst$$Safe_root$$s.IsEmpty() )
	{
		// Initialize the beat offset to an invalid value
		long lBeatOffset = -1;

		// If p$$Safe_root$$AtDragPoint is valid, just use the measure and beat information from it
		if( p$$Safe_root$$AtDragPoint )
		{
			// Verify that this item is selected
			ASSERT( p$$Safe_root$$AtDragPoint->m_fSelected );

			// Compute how many beats from the start it is
			MeasureBeatToBeats( m_pTimeline, m_dwGroupBits, 0, p$$Safe_root$$AtDragPoint->m_lMeasure, p$$Safe_root$$AtDragPoint->m_lBeat, lBeatOffset );
		}
		// Otherwise look for the first selected item
		else
		{
			POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
			while( pos )
			{
				C$$Safe_root$$Item* p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos );
				if ( p$$Safe_root$$Item->m_fSelected )
				{
					// Found a selected item - compute how many beats from the start it is
					MeasureBeatToBeats( m_pTimeline, m_dwGroupBits, 0, p$$Safe_root$$Item->m_lMeasure, p$$Safe_root$$Item->m_lBeat, lBeatOffset );
					break;
				}
			}
		}

		// p$$Safe_root$$AtDragPoint is NULL and there are no selected items - return with S_FALSE
		if ( lBeatOffset == -1 )
		{
			return S_FALSE;
		}

		// Now, actually save the items
		return SaveSelected$$Safe_root$$s( pIStream, lBeatOffset );
	}
	else
	{
		return S_FALSE; // Nothing in the list
	}
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::MarkSelected$$Safe_root$$s

// ORs dwFlags with the m_dwBits of each selected item
void C$$Safe_root$$Mgr::MarkSelected$$Safe_root$$s( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos );

		// Check if the item is selected
		if ( p$$Safe_root$$Item->m_fSelected )
		{
			// It's selected - update m_dwBits
			p$$Safe_root$$Item->m_dwBits |= dwFlags;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::DeleteMarked

// deletes items marked by given flag
void C$$Safe_root$$Mgr::DeleteMarked( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos1 = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos1 );

		// Check if any of the specified dwFlags are set in this item
		if ( p$$Safe_root$$Item->m_dwBits & dwFlags )
		{
			// At least one of the flags in dwFlags is set, remove the item
			m_lst$$Safe_root$$s.RemoveAt( pos2 );

			// Now, delete it
			delete p$$Safe_root$$Item;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::UnMark$$Safe_root$$s

// unmarks flag m_dwUndermined field C$$Safe_root$$Items in list
void C$$Safe_root$$Mgr::UnMark$$Safe_root$$s( DWORD dwFlags )
{
	// Iterate through the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Clear the selected flags in each item
		m_lst$$Safe_root$$s.GetNext( pos )->m_dwBits &= ~dwFlags;
	}
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::UnselectAll

void C$$Safe_root$$Mgr::UnselectAll()
{
	// Iterate through the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Clear the selection flag for each item
		m_lst$$Safe_root$$s.GetNext( pos )->m_fSelected = FALSE;
	}

	// Clear the list of selected regions
	m_p$$Safe_root$$Strip->m_pSelectedRegions->Clear();
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::SelectAll

void C$$Safe_root$$Mgr::SelectAll()
{
	// Iterate through the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Set the selection flag for each item
		m_lst$$Safe_root$$s.GetNext( pos )->m_fSelected = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::IsSelected

bool C$$Safe_root$$Mgr::IsSelected()
{
	// If anything is selected, return true
	
	// Iterate through the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Check if this item is selected
		if ( m_lst$$Safe_root$$s.GetNext( pos )->m_fSelected )
		{
			// $$Safe_root$$ is selected - return true
			return true;
		}
	}

	// No items selected - return false
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::IsEmpty

bool C$$Safe_root$$Mgr::IsEmpty()
{
	return m_lst$$Safe_root$$s.IsEmpty() ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::FirstSelected$$Safe_root$$

C$$Safe_root$$Item* C$$Safe_root$$Mgr::FirstSelected$$Safe_root$$()
{
	// Iterate through the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to this lyic
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos );

		// Check if the item is selected
		if ( p$$Safe_root$$Item->m_fSelected )
		{
			// $$Safe_root$$ is selected, return a pointer to it
			return p$$Safe_root$$Item;
		}
	}

	// No items are selected, return NULL
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::SelectSegment

// Return true if anything changed
bool C$$Safe_root$$Mgr::SelectSegment(MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime)
{
	// Verify that we have a pointer to the Timeline
	if( !m_pTimeline )
	{
		ASSERT(FALSE);
		return false;
	}

	// Convert the start position from clocks to a measure and beat value
	long lMeasure, lBeat;
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0,
									  mtBeginTime, &lMeasure, &lBeat );

	
	// Convert back to a clock value
	m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0,
									  lMeasure, lBeat, &mtBeginTime );

	// Convert the end position from clocks to a measure and beat value
	m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0,
									  mtEndTime, &lMeasure, &lBeat );

	
	// Increment the beat so the last beat is selected
	lBeat++;

	// Convert back to a clock value
	m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0,
									  lMeasure, lBeat, &mtEndTime );

	// Convert the passed in times to a generic time class
	CMusicTimeConverter cmtBeg(mtBeginTime);
	CMusicTimeConverter cmtEnd(mtEndTime);

	// Create a region that contains the selected time
	CSelectedRegion region(cmtBeg, cmtEnd);

	// Add the region to the list of selected regions
	m_p$$Safe_root$$Strip->m_pSelectedRegions->AddRegion(region);

	// Select all items in the list of selected regions
	// This will return true if the selection state of any item changed
	return m_p$$Safe_root$$Strip->SelectItemsInSelectedRegions();
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::InsertByAscendingTime

void C$$Safe_root$$Mgr::InsertByAscendingTime( C$$Safe_root$$Item *p$$Safe_root$$ToInsert )
{
	// Ensure the p$$Safe_root$$ToInsert pointer is valid
	if ( p$$Safe_root$$ToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	// Iterate through the list of items
	POSITION pos1 = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos1 )
	{
		// Save the current position
		POSITION pos2 = pos1;

		// Get a pointer to the current item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos1 );

		// Check if the current item is in the same measure as the one to be inserted
		if ( p$$Safe_root$$Item->m_lMeasure == p$$Safe_root$$ToInsert->m_lMeasure )
		{
			//  Check if the current item is in the same beat or later than the one to be inserted
			if ( p$$Safe_root$$Item->m_lBeat >= p$$Safe_root$$ToInsert->m_lBeat )
			{
				// insert before pos2 (current position of p$$Safe_root$$Item)
				m_lst$$Safe_root$$s.InsertBefore( pos2, p$$Safe_root$$ToInsert );

				// If we've overwritten an existing $$Safe_root$$, remove and delete the existing item
				if( p$$Safe_root$$Item->m_lBeat == p$$Safe_root$$ToInsert->m_lBeat )
				{
					m_lst$$Safe_root$$s.RemoveAt( pos2 );
					delete p$$Safe_root$$Item;
				}

				// Return, since we found the position to insert the item in
				return;
			}
		}

		// Check if the current item is later than the one to be inserted
		if ( p$$Safe_root$$Item->m_lMeasure > p$$Safe_root$$ToInsert->m_lMeasure )
		{
			// insert before pos2 (current position of p$$Safe_root$$Item)
			m_lst$$Safe_root$$s.InsertBefore( pos2, p$$Safe_root$$ToInsert );

			// Return, since we found the position to insert the item in
			return;
		}
	}

	// p$$Safe_root$$ToInsert is later than all items in the list, add it at the end of the list
	m_lst$$Safe_root$$s.AddTail( p$$Safe_root$$ToInsert );

	// Return, since we found the position to insert the item in
	return;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::RemoveItem

bool C$$Safe_root$$Mgr::RemoveItem( C$$Safe_root$$Item* pItem )
{
	// Find the given item
	POSITION posToRemove = m_lst$$Safe_root$$s.Find( pItem, NULL );

	// If item wasn't found, return false
	if( posToRemove == NULL )
	{
		return false;
	}

	// Remove the item from the list - the caller must delete it
	m_lst$$Safe_root$$s.RemoveAt( posToRemove );

	// Return true since we found the item
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::GetNextGreatestUniqueTime

bool C$$Safe_root$$Mgr::GetNextGreatestUniqueTime( long lMeasure, long lBeat, MUSIC_TIME* pmtTime )
{
	// Iterate throught the $$Safe_root$$ list
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to the current item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos );

		// If this item is beyond the measure we're looking for, exit
		// since the passed in lMeasure and lBeat are empty
		if( p$$Safe_root$$Item->m_lMeasure > lMeasure )
		{
			break;
		}

		// If this item is in the same measure as the passed in lMeasure
		if( p$$Safe_root$$Item->m_lMeasure == lMeasure )
		{
			// If this item is after the passed in lBeat, exit
			// since the passed in lMeasure and lBeat are empty
			if( p$$Safe_root$$Item->m_lBeat > lBeat )
			{
				break;
			}

			// If this item is in the same beat as the passed in lBeat
			if( p$$Safe_root$$Item->m_lBeat == lBeat )
			{
				// Look in the next beat
				lBeat++;

				// Check to see if we've rolled over to the next measure
				m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, pmtTime );
				m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, *pmtTime, &lMeasure, &lBeat );
				
				// Check if measure rolled over
				if( p$$Safe_root$$Item->m_lMeasure != lMeasure )
				{
					// No more beats in this measure so discard the item
					return false;
				}
			}
		}
	}

	// Found an empty lBeat for the new item - update pmtTime with the time
	m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, lBeat, pmtTime );

	// Return that we were able to find a spot for the new item
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::RecomputeTimes

bool C$$Safe_root$$Mgr::RecomputeTimes()
{
	MUSIC_TIME mtTime;
	long lMeasure;
	long lBeat;

	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while( pos )
	{
		// Save our current position
		POSITION pos2 = pos;

		// Get a pointer to the current item
		C$$Safe_root$$Item* p$$Safe_root$$Item = m_lst$$Safe_root$$s.GetNext( pos );

		// Using the item's current measure and beat settings, determine which measure and beat
		// the item will end up on
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, p$$Safe_root$$Item->m_lMeasure, p$$Safe_root$$Item->m_lBeat, &mtTime );
		m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );

		// Check if either the measure or beat were changed
		if(	p$$Safe_root$$Item->m_lMeasure != lMeasure
		||	p$$Safe_root$$Item->m_lBeat != lBeat )
		{
			// Remove the itme from the list
			m_lst$$Safe_root$$s.RemoveAt( pos2 );

			// The measure the $$Safe_root$$ is in changed
			if( p$$Safe_root$$Item->m_lMeasure != lMeasure )
			{
				// This would happen when moving from 7/4 to 4/4, for example
				// $$Safe_root$$s on beat 7 would end up on next measure's beat 3
				while( p$$Safe_root$$Item->m_lMeasure != lMeasure )
				{
					// Keep moving back a beat until the measure does not change
					m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, lMeasure, --lBeat, &mtTime );
					m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );
				}
			}

			// Try and find an empty beat in this measure on or after lBeat
			if( GetNextGreatestUniqueTime( lMeasure, lBeat, &mtTime ) )
			{
				// Found an empty beat, find out what it is
				m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &lMeasure, &lBeat );

				// Update the item with the new measure and beat
				p$$Safe_root$$Item->m_lMeasure = lMeasure;
				p$$Safe_root$$Item->m_lBeat = lBeat;

				// Flag that something changed
				fChanged = true;

				// Reinsert it into the list
				InsertByAscendingTime( p$$Safe_root$$Item );
			}
			else
			{
				// Couldn't find a spot for the item - delete it
				delete p$$Safe_root$$Item;

				// Flag that something changed
				fChanged = true;
			}
		}
	}

	// Return whether or not anything changed
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::DeleteBetweenMeasureBeats

bool C$$Safe_root$$Mgr::DeleteBetweenMeasureBeats(long lmStart, long lbStart, long lmEnd, long lbEnd )
{
	// Initially, nothing changed
	bool fChanged = false;

	// Iterate through the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while(pos)
	{
		// Save current position
		POSITION posTemp = pos;

		// Get a pointer to the current item
		C$$Safe_root$$Item* pItem = m_lst$$Safe_root$$s.GetNext(pos);

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
		m_lst$$Safe_root$$s.RemoveAt(posTemp);
		delete pItem;
		fChanged = true;
	}

	// Return whether or not anything changed
	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$Mgr::SaveSelected$$Safe_root$$s

HRESULT C$$Safe_root$$Mgr::SaveSelected$$Safe_root$$s( IStream* pStream, long lBeatOffset )
// save selected items and adjust by "offset"
{
	// Verify that the pStream pointer is valid
	if(pStream == NULL)
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	// Create a list to store the items to save in
	CTypedPtrList<CPtrList, C$$Safe_root$$Item*> lst$$Safe_root$$sToSave;

	// Iterate throught the list of items
	POSITION pos = m_lst$$Safe_root$$s.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$ = m_lst$$Safe_root$$s.GetNext(pos);

		// Check if the item is selected
		if( p$$Safe_root$$->m_fSelected )
		{
			// Add the item to the list of items to save
			lst$$Safe_root$$sToSave.AddTail(new C$$Safe_root$$Item(*p$$Safe_root$$));
		}
	}


	//check that anything is selected
	if( lst$$Safe_root$$sToSave.IsEmpty() )
	{
		return S_FALSE;
	}

	// For each item in lst$$Safe_root$$sToSave, convert to a number of beats and subtract lBeatOffset
	Normalize$$Safe_root$$List( m_pTimeline, m_dwGroupBits, lst$$Safe_root$$sToSave, lBeatOffset );

	// Save the list of items into pStream
	HRESULT hr = Save$$Safe_root$$List( lst$$Safe_root$$sToSave, pStream );

	// Empty the temporary list of items
	Empty$$Safe_root$$List(lst$$Safe_root$$sToSave);

	// Return whether or not the save succeeded
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// Normalize$$Safe_root$$List

void Normalize$$Safe_root$$List( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list, long lBeatOffset )
{
	// Iterate through the list of items
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$ = list.GetNext(pos);

		// Convert the start time to a number of beats
		MeasureBeatToBeats( pITimeline, dwGroupBits, 0, p$$Safe_root$$->m_lMeasure, p$$Safe_root$$->m_lBeat, p$$Safe_root$$->m_lBeat );
		p$$Safe_root$$->m_lMeasure = 0;

		// Offset the beat value
		p$$Safe_root$$->m_lBeat -= lBeatOffset;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Save$$Safe_root$$List

HRESULT Save$$Safe_root$$List( CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list, IStream* pIStream )
{
	// Try and allocate an IDMUSProdRIFFStream from the IStream pointer
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		// Couldn't allocate an IDMUSProdRIFFStream, return a failure code
		return hr;
	}

	// Create the list chunk to store the $$Safe_root$$ data inside
	MMCKINFO ckMain;
	ckMain.fccType = FOURCC_$$SAFE_ROOT$$_LIST;
	if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
	{
		// Couldn't create the chunk, exit with a failure code
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Iterate through the list of items
	POSITION pos;
	pos = list.GetHeadPosition();
	while( pos )
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* p$$Safe_root$$Item = list.GetNext( pos );

		// Create a chunk to store each item inside
		MMCKINFO ck;
		ck.ckid = FOURCC_$$SAFE_ROOT$$_ITEM;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// Couldn't create the chunk, exit with a failure code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Initialize a DMUS_IO_$$SAFE_ROOT$$ structure to store the information about the $$Safe_root$$
		DMUS_IO_$$SAFE_ROOT$$ o$$Safe_root$$;
		ZeroMemory( &o$$Safe_root$$, sizeof( DMUS_IO_$$SAFE_ROOT$$ ) );

		// No need to set the mtTime member, since it's not used when pasting or dropping
		// o$$Safe_root$$.mtTime = 0;

		// Copy the measure and beat information to the DMUS_IO_$$SAFE_ROOT$$ structure
		// Since this is only for doing a copy/paste or drag/drop, store the beat
		// offset into dwMeasure
		o$$Safe_root$$.dwMeasure = p$$Safe_root$$Item->m_lBeat;
		o$$Safe_root$$.bBeat = 0;

		// Set the length of the text
		o$$Safe_root$$.dwLength = (p$$Safe_root$$Item->m_strText.GetLength() + 1) * sizeof(wchar_t);

		// Write out the DMUS_IO_$$SAFE_ROOT$$ structure
		DWORD dwBytesWritten;
		hr = pIStream->Write( &o$$Safe_root$$, sizeof(DMUS_IO_$$SAFE_ROOT$$), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_$$SAFE_ROOT$$) )
		{
			// An I/O error occurred, so exit with a failure code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Save the text as a Wide Character String
		hr = SaveMBStoWCS( pIStream, &p$$Safe_root$$Item->m_strText );
		if( FAILED( hr ) )
		{
			// An I/O error occurred, so exit with a failure code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out the $$Safe_root$$ item's chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// An I/O error occurred, so exit with a failure code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Ascend out of the main $$Safe_root$$ LIST chunk
	pIRiffStream->Ascend( &ckMain, 0 );

ON_ERROR:
	// Release the IDMUSProdRIFFStream interface
	pIRiffStream->Release();

	// Return the success or failure code
    return hr;
}

HRESULT Load$$Safe_root$$List( CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list, IStream* pIStream )
{
	// Try and allocate an IDMUSProdRIFFStream interface from the pIStream
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	HRESULT hr;
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		// Couldn't allocate an IDMUSProdRIFFStream interface, return an error code
		return hr;
	}

	// Descend into the $$Safe_root$$ LIST chunk
	MMCKINFO ck;
	ck.fccType = FOURCC_$$SAFE_ROOT$$_LIST;
	if( pIRiffStream->Descend(&ck, NULL, MMIO_FINDLIST) == 0)
	{
		// Now, descend into each chunk in this LIST chunk
		MMCKINFO ckList;
		while(pIRiffStream->Descend(&ckList, &ck, 0) == 0)
		{
			switch(ckList.ckid)
			{
			// We can only read FOURCC_$$SAFE_ROOT$$_ITEM chunks
			case FOURCC_$$SAFE_ROOT$$_ITEM:
				// Create a DMUS_IO_$$SAFE_ROOT$$ structure to store the data in
				DMUS_IO_$$SAFE_ROOT$$ i$$Safe_root$$;

				// Try and read the DMUS_IO_$$SAFE_ROOT$$ structure
				DWORD dwByteCount;
				hr = pIStream->Read(&i$$Safe_root$$, sizeof(DMUS_IO_$$SAFE_ROOT$$), &dwByteCount);
				if(FAILED(hr) || dwByteCount != sizeof(DMUS_IO_$$SAFE_ROOT$$))
				{
					// An I/O error occurred, so exit with a failure code
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Create a new C$$Safe_root$$Item to store the data in
				C$$Safe_root$$Item* pItem = new C$$Safe_root$$Item;
				if( pItem == NULL)
				{
					// If the allocation failed, return an error code
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}

				// Read in the text from a Wide Character String to a MultiByte String
				ReadMBSfromWCS(pIStream, i$$Safe_root$$.dwLength, &pItem->m_strText);

				// Set the C$$Safe_root$$Item's data members
				pItem->m_lMeasure = i$$Safe_root$$.dwMeasure;
				pItem->m_lBeat = i$$Safe_root$$.bBeat;

				// By default, the item is selected when it is pasted or dropped
				pItem->m_fSelected = TRUE;

				// Add the item to list passed in
				list.AddTail(pItem);

				// Finished reading the FOURCC_$$SAFE_ROOT$$_ITEM chunk
				break;
			}

			// Ascend out of the chunk inside the FOURCC_$$SAFE_ROOT$$_LIST chunk
			pIRiffStream->Ascend(&ckList, 0);
		}

		// Ascend out of the FOURCC_$$SAFE_ROOT$$_LIST chunk
		pIRiffStream->Ascend(&ck, 0);
	}

ON_ERROR:
	// Release the IDMUSProdRIFFStream interface
	pIRiffStream->Release();

	// Return the success or failure code
    return hr;
}

void Empty$$Safe_root$$List( CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list )
{
	// Remove and delete all the C$$Safe_root$$Items from the given list
	while( !list.IsEmpty() )
	{
		delete list.RemoveHead();
	}
}

HRESULT GetBoundariesOf$$Safe_root$$s( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, long &lStartTime, long &lEndTime, CTypedPtrList<CPtrList, C$$Safe_root$$Item*>& list)
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

	// No items in list, return S_FALSE since there's nothing to do
	if( list.IsEmpty() )
	{
		return S_FALSE;
	}

	// Initialize our return value to S_OK
	HRESULT hr = S_OK;
	MUSIC_TIME mtTime;

	// Iterate through the list of items
	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		// Get a pointer to each item
		C$$Safe_root$$Item* pItem = list.GetNext(pos);

		// Try and convert the measure and beat of the item to a value in number of clocks
		hr = pTimeline->MeasureBeatToClocks( dwGroupBits, 0, pItem->m_lMeasure, pItem->m_lBeat, &mtTime );

		// If the conversion failed, break out of the while loop
		if(FAILED(hr))
		{
			break;
		}

		// If the start time is not yet set, or the item's time is earlier
		// than any other item, update lStartTime.
		if( ( lStartTime == -1 )
		||	( mtTime < lStartTime ) )
		{
			lStartTime = mtTime;
		}

		// If the end time is not yet set, or the item's time is later
		// than any other item, update lEnd.
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

		// Get the measure and beat of the last item
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
