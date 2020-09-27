// TimelineDataObject.cpp : implementation file
//
// @doc Timeline

#include "stdafx.h"
#include "timeline.h"
#include "TimelineDataObject.h"
#include "DllJazzDataObject.h"
#include <RiffStrm.h>

/*  --------------------------------------------------------------------------
	@interface IDMUSProdTimelineDataObject | Wrapper interface for data stored on the clipboard.

	@meth HRESULT | AddInternalClipFormat | Adds a stream to the clipboard for use by other DirectMusic Producer components.

	@meth HRESULT | AddExternalClipFormat | Adds a stream to the clipboard for use by other applications.

	@meth HRESULT | IsClipFormatAvailable | Checks for data in a specified clipboard format in this object.

	@meth HRESULT | AttemptRead | Tries to read a stream for a specified clipboard format.

	@meth HRESULT | GetBoundaries | Get the boundaries of the data in this object.

	@meth HRESULT | SetBoundaries | Set the boundaries of the data in this object.

	@meth HRESULT | Import | Import the data from an <i IDataObject> into this object.

	@meth HRESULT | Export | Export the data from this object into an <i IDataObject>.

	@base public | IUnknown

	@xref	<i IDMUSProdTimeline>
	--------------------------------------------------------------------------*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimelineDataObject

CTimelineDataObject::CTimelineDataObject()
{
	_Module.Lock();
	m_cRef = 0;
	AddRef();
	m_lStart = -1;
	m_lEnd = -1;
	m_cfTimeline = 0;
}

CTimelineDataObject::~CTimelineDataObject()
{
	CClipboardStorage *pStorage;

	while( !m_lstInternalClipboard.IsEmpty() )
	{
		pStorage = m_lstInternalClipboard.RemoveHead();
		delete pStorage;
	}

	while( !m_lstExternalClipboard.IsEmpty() )
	{
		pStorage = m_lstExternalClipboard.RemoveHead();
		delete pStorage;
	}

	while( !m_lstUsedInternalClipboard.IsEmpty() )
	{
		pStorage = m_lstUsedInternalClipboard.RemoveHead();
		delete pStorage;
	}

	while( !m_lstUsedExternalClipboard.IsEmpty() )
	{
		pStorage = m_lstUsedExternalClipboard.RemoveHead();
		delete pStorage;
	}
	_Module.Unlock();
}


/////////////////////////////////////////////////////////////////////////////
// CTimelineDataObject

/////////////////////////////////////////////////////////////////////////////
// CTimelineDataObject IUnknown
STDMETHODIMP CTimelineDataObject::QueryInterface( REFIID riid, LPVOID *ppvObj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppvObj == NULL )
	{
		return E_POINTER;
	}

    if( ::IsEqualIID(riid, IID_IDMUSProdTimelineDataObject)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdTimelineDataObject *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CTimelineDataObject::AddRef(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CTimelineDataObject::Release(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( 0L == --m_cRef )
	{
		delete this;
		return 0;
	}
	else
	{
		return m_cRef;
	}
}


//  @method HRESULT | IDMUSProdTimelineDataObject | AddInternalClipFormat | This method adds the specified
//		<p pIStream> with the format <p uClipFormat> to the internal list of streams stored
//		in this TimelineDataObject.  The stream will be unavailable to other applications.
//
//  @parm	UINT | uClipFormat | The clipboard format of the stream
//  @parm   IStream* | pIStream | The stream containing data
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStream>
//	@rvalue E_OUTOFMEMORY | Unable to allocate memory while adding the stream
//
//	@xref	<om IDMUSProdTimelineDataObject::AddExternalClipFormat> <i IDMUSProdTimelineDataObject> <om IDMUSProdTimeline::AllocTimelineDataObject>
HRESULT CTimelineDataObject::AddInternalClipFormat( UINT uClipFormat, IStream* pIStream)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate pIStream
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	CClipboardStorage* pStorage = NULL;
	try
	{
		// Create an object to store the stream and clipboard format
		pStorage = new CClipboardStorage( uClipFormat, pIStream );
		
		// Add it to our internal list
		m_lstInternalClipboard.AddTail( pStorage );
	}
	catch ( CMemoryException* pMemoryException )
	{
		// Catch out of memory errors
		pMemoryException->Delete();

		// This will be true if AddTail ran out of memory
		if( pStorage )
		{
			delete pStorage;
		}

		return E_OUTOFMEMORY;
	}

	// Succeeded
	return S_OK;
}


//  @method HRESULT | IDMUSProdTimelineDataObject | AddExternalClipFormat | This method adds the specified
//		<p pIStream> with the format <p uClipFormat> to the list of streams stored in this
//		TimelineDataObject.  The stream will be available to other applications that support
//		<p uClipFormat>.
//
//  @parm	UINT | uClipFormat | The clipboard format of the stream
//  @parm   IStream* | pIStream | The stream containing data
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStream>
//	@rvalue E_UNEXPECTED | An unexpected error occurred
//	@rvalue E_OUTOFMEMORY | Unable to allocate memory while adding the stream
//
//	@xref	<om IDMUSProdTimelineDataObject::AddInternalClipFormat> <i IDMUSProdTimelineDataObject> <om IDMUSProdTimeline::AllocTimelineDataObject>
HRESULT CTimelineDataObject::AddExternalClipFormat( UINT uClipFormat, IStream* pIStream)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate pIStream
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	CClipboardStorage* pStorage = NULL;
	try
	{
		// Create an object to store the stream and clipboard format
		pStorage = new CClipboardStorage( uClipFormat, pIStream );
		
		// Add it to our external list
		m_lstExternalClipboard.AddTail( pStorage );
	}
	catch ( CMemoryException* pMemoryException )
	{
		// Catch out of memory errors
		pMemoryException->Delete();

		// This will be true if AddTail ran out of memory
		if( pStorage )
		{
			delete pStorage;
		}

		return E_OUTOFMEMORY;
	}

	// Succeeded
	return S_OK;
}


//  @method HRESULT | IDMUSProdTimelineDataObject | IsClipFormatAvailable | This method searches the list
//		of internal and external clipboard formats available, and returns S_OK if it finds
//		<p uClipFormat>.  S_FALSE is returned if <p uClipFormat> is not found.
//
//  @parm	UINT | uClipFormat | The clipboard format to search for
//
//  @rvalue S_OK | The clipboard format is available.
//	@rvalue S_FALSE | The clipboard format is not available
//
//	@xref	<om IDMUSProdTimelineDataObject::AttemptRead> <i IDMUSProdTimelineDataObject> <om IDMUSProdTimeline::AllocTimelineDataObject>
HRESULT CTimelineDataObject::IsClipFormatAvailable( UINT uClipFormat )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( FindClipboardStorage( uClipFormat ) != NULL )
	{
		return S_OK;
	}

	return S_FALSE;
}


//  @method HRESULT | IDMUSProdTimelineDataObject | AttemptRead | This method searches the list
//		of internal and external clipboard formats available, and if a match is found it fills
//		<p ppIStream> with a pointer to the stream containing the data.
//
//  @parm	UINT | uClipFormat | The clipboard format to search for
//  @parm	IStream | ppIStream | A pointer to the location to store a pointer to the stream
//		containing the data.
//
//	@comm	A subsequence call to this method with the same <p uClipFormat> will return a different
//		stream pointer, if one is availabe.  This allows multiple strips of the same type to be copied
//		and pasted without losing data.
//
//  @rvalue S_OK | The clipboard format is available. <p ppIStream> contains a pointer to the
//		stream containing the data.
//	@rvalue E_INVALIDARG | The clipboard format was not found.
//	@rvalue E_POINTER | <p ppIStream> is NULL.
//
//	@xref	<om IDMUSProdTimelineDataObject::IsClipFormatAvailable> <i IDMUSProdTimelineDataObject> <om IDMUSProdTimeline::AllocTimelineDataObject>
HRESULT CTimelineDataObject::AttemptRead( UINT uClipFormat, IStream** ppIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppIStream == NULL )
	{
		return E_POINTER;
	}


	// Search our internal list
	CClipboardStorage *pClipboardStorage;
	pClipboardStorage = FindClipboardStorage( uClipFormat );

	// If found, set ppIStream to the stream
	if( pClipboardStorage )
	{
		pClipboardStorage->m_pIStream->Clone( ppIStream );
		// Reset the stream pointer to the beginning
		LARGE_INTEGER li;
		li.QuadPart = 0;
		(*ppIStream)->Seek( li, STREAM_SEEK_SET, NULL );

		// Now, remove this item from the main lists and add it to
		// the Used list
		POSITION pos;
		pos = m_lstInternalClipboard.Find( pClipboardStorage );
		if( pos )
		{
			m_lstInternalClipboard.RemoveAt( pos );
			m_lstUsedInternalClipboard.AddTail( pClipboardStorage );
		}
		else
		{
			pos = m_lstExternalClipboard.Find( pClipboardStorage );
			ASSERT( pos );
			m_lstExternalClipboard.RemoveAt( pos );
			m_lstUsedExternalClipboard.AddTail( pClipboardStorage );
		}
		return S_OK;
	}

	return E_INVALIDARG;
}


//  @method HRESULT | IDMUSProdTimelineDataObject | GetBoundaries | This method returns the
//		start and edit times of the selection that was pasted to the clipboard.
//
//	@comm	These boundaries are only available if a <i IDMUSProdTimelineDataObject> was
//		pasted to the clipboard with valid boundaries.  Either <p plStartTime> or <p plEndTime>
//		may be NULL, but not both.
//
//  @parm	long | plStartTime | A pointer to the location to store the start time in.
//  @parm	long | plEndTime | A pointer to the location to store the end time in.
//
//  @rvalue S_OK | <p plStartTime> and/or <p plEndTime> were successfully set.
//	@rvalue E_PENDING | The start and end boundaries are not available (they are both internally set to -1).
//	@rvalue E_POINTER | Both <p plStartTime> and <p plEndTime> are NULL.
//
//	@xref	<i IDMUSProdTimelineDataObject> <om IDMUSProdTimeline::AllocTimelineDataObject> <om IDMUSProdTimelineDataObject::SetBoundaries>
HRESULT CTimelineDataObject::GetBoundaries( long* plStartTime, long* plEndTime )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate plStartTime and plEndTime
	if( (plStartTime == NULL) && (plEndTime == NULL) )
	{
		return E_POINTER;
	}

	// Ensure the start and edit times were set
	if( (m_lEnd == -1)
	&&	(m_lStart == -1) )
	{
		return E_PENDING;
	}

	// Set plStartTime
	if( plStartTime )
	{
		*plStartTime = m_lStart;
	}

	// Set plEndTime
	if( plEndTime )
	{
		*plEndTime = m_lEnd;
	}

	return S_OK;
}


//  @method HRESULT | IDMUSProdTimelineDataObject | SetBoundaries | This method sets the
//		boundaries to save to the clipboard and return with <om IDMUSProdTimelineDataObject::GetBoundaries>
//
//  @parm	long | lStartTime | The start time.
//  @parm	long | lEndTime | The end time.
//
//	@comm	If <p lStartTime> and <p lEndTime> are both -1, the TimelineDataObject will not persist
//		them to the clipboard, and <om IDMUSProdTimelineDataObject::GetBoundaries> will return
//		E_PENDING.
//
//  @rvalue S_OK | The start and end time were successfully set.
//
//	@xref	<i IDMUSProdTimelineDataObject> <om IDMUSProdTimeline::AllocTimelineDataObject> <om IDMUSProdTimelineDataObject::GetBoundaries>
HRESULT CTimelineDataObject::SetBoundaries( long lStartTime, long lEndTime )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_lStart = lStartTime;
	m_lEnd = lEndTime;

	return S_OK;
}


//  @method HRESULT | IDMUSProdTimelineDataObject | Import | This method imports the clipboard
//		data formats from an IDataObject and makes them accessable from the methods of
//		<i IDMUSProdTimelineDataObject>
//
//  @parm	IDataObject | pIDataObject | A pointer to <i IDataObject> to import.
//
//  @rvalue S_OK | <p pIDataObject> was successfully imported.
//	@rvalue E_FAIL | An error occurred.
//	@rvalue E_POINTER | <p pIDataObject> is NULL.
//
//	@xref	<i IDMUSProdTimelineDataObject> <i IDataObject> <om IDMUSProdTimeline::AllocTimelineDataObject> <om IDMUSProdTimelineDataObject::Import>
HRESULT CTimelineDataObject::Import( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Register our clipboard format if we haven't done so already
	if( m_cfTimeline == 0 )
	{
		m_cfTimeline = RegisterClipboardFormat( CF_TIMELINE );
		if( m_cfTimeline == 0 )
		{
			return E_FAIL;
		}
	}

	// Initialize variables
	IEnumFORMATETC *pIEnumFORMATETC;
	FORMATETC formatEtc;
	ULONG ulElem;
	HRESULT hr;
	CDllJazzDataObject *pDllJazzDataObject = new CDllJazzDataObject;

	// Get IEnumFORMATETC interface
	hr = pIDataObject->EnumFormatEtc( DATADIR_GET, &pIEnumFORMATETC );
	ASSERT( hr == S_OK );
	if ( hr != S_OK )
	{
		pDllJazzDataObject->Release();
		return E_FAIL;
	}

	// Reset the enumeration counter
	pIEnumFORMATETC->Reset();

	// Iterate through all formats
	while ( pIEnumFORMATETC->Next( 1, &formatEtc, &ulElem ) == S_OK)
	{
		// Get the IStream
		IStream	*pIStream;
		hr = pDllJazzDataObject->AttemptRead( pIDataObject, formatEtc.cfFormat, &pIStream);
		if ( SUCCEEDED( hr ) )
		{
			// Seek to the beginning of the stream
			LARGE_INTEGER liStreamPos;
			liStreamPos.QuadPart = 0;
			hr = pIStream->Seek( liStreamPos, STREAM_SEEK_SET, NULL );
			ASSERT( SUCCEEDED( hr ));

			// Check for timeline format
			if( formatEtc.cfFormat == m_cfTimeline )
			{
				LoadInternalListFromStream( pIStream );
			}
			// Add it to our external list
			else
			{
				CClipboardStorage *pStorage;
				pStorage = new CClipboardStorage( formatEtc.cfFormat, pIStream ); 
				m_lstExternalClipboard.AddTail( pStorage );
			}
			pIStream->Release();
		}
	}

	// Clean up
	pIEnumFORMATETC->Release();
	pDllJazzDataObject->Release();

	return S_OK;
}


//  @method HRESULT | IDMUSProdTimelineDataObject | Export | This method exports the clipboard
//		data formats stored in this <i ITimleineDataObject> and creates an IDataObject
//		that can be subsequently added to the clipboard.
//
//  @parm	IDataObject | ppIDataObject | A pointer location to store the pointer to the new <i IDataObject>.
//
//  @rvalue S_OK | <p ppIDataObject> was successfully exported.
//	@rvalue E_UNEXPECTED | We failed to register the Timeline's clipboard format.
//	@rvalue E_POINTER | <p ppIDataObject> is NULL.
//
//	@xref	<i IDMUSProdTimelineDataObject> <i IDataObject> <om IDMUSProdTimeline::AllocTimelineDataObject> <om IDMUSProdTimelineDataObject::Import>
HRESULT CTimelineDataObject::Export( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	// Initialize variables
	HRESULT hr = S_OK;
	*ppIDataObject = NULL;

	// Create a DllJazzDataObject
	CDllJazzDataObject*	pDllJazzDataObject = NULL;
	pDllJazzDataObject = new CDllJazzDataObject;

	// Need to keep Timeline.DLL around forever, so that OLE can use this clipboard object
	// BUGBUG: Need to fix this in DX8.1
	_Module.Lock();

	// If boundaries are set, or if there are any internal clipboard items,
	// add a CF_TIMELINE object to the clipboard
	if( (m_lEnd != -1) || (m_lStart != -1) || !m_lstInternalClipboard.IsEmpty() )
	{
		// Register our clipboard format if we haven't done so already
		if( m_cfTimeline == 0 )
		{
			m_cfTimeline = RegisterClipboardFormat( CF_TIMELINE );
			if( m_cfTimeline == 0 )
			{
				hr = E_UNEXPECTED;
				goto ON_ERROR;
			}
		}

		// Get our stream and add it to the clipboard
		IStream *pIStream;
		if( SUCCEEDED( GetInternalListAsStream( &pIStream ) ) )
		{
			pDllJazzDataObject->AddClipFormat( m_cfTimeline, pIStream );
			pIStream->Release();
		}
	}

	// Add all the external clipboard streams
	if( !m_lstExternalClipboard.IsEmpty() )
	{
		CClipboardStorage *pStorage;
		POSITION pos;
		pos = m_lstExternalClipboard.GetHeadPosition();
		while( pos )
		{
			pStorage = m_lstExternalClipboard.GetNext( pos );
			pDllJazzDataObject->AddClipFormat( pStorage->m_cfClipboardFormat, pStorage->m_pIStream );
		}
	}

	// QI for an IDataObject interface to return
	hr = pDllJazzDataObject->QueryInterface(IID_IDataObject, (void**) ppIDataObject);

ON_ERROR:
	// Clean up and return
	pDllJazzDataObject->Release();
	return hr;
}

CClipboardStorage* CTimelineDataObject::FindClipboardStorage( UINT uClipFormat )
{
	POSITION pos;
	CClipboardStorage *pClipboardStorage;

	// Search our internal list
	pos = m_lstInternalClipboard.GetHeadPosition();
	while ( pos != NULL )
	{
		pClipboardStorage = m_lstInternalClipboard.GetNext( pos );
		if( pClipboardStorage->m_cfClipboardFormat == uClipFormat )
		{
			return pClipboardStorage;
		}
	}

	// Search our external list
	pos = m_lstExternalClipboard.GetHeadPosition();
	while ( pos != NULL )
	{
		pClipboardStorage = m_lstExternalClipboard.GetNext( pos );
		if( pClipboardStorage->m_cfClipboardFormat == uClipFormat )
		{
			return pClipboardStorage;
		}
	}

	return NULL;
}

HRESULT CTimelineDataObject::GetInternalListAsStream( IStream** ppIStream )
{
	// Validate and initialize ppIStream
	ASSERT( ppIStream );
	*ppIStream = NULL;

	// Create a new stream
	IStream* pIStream;
	if( FAILED( CreateStreamOnHGlobal( NULL, TRUE, &pIStream ) ) )
	{
		return E_UNEXPECTED;
	}

	// Now, finally save ourself
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Alloc an IDMUSProdRIFFStream from the IStream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		pIStream->Release();
		return hr;
	}

	// If the start and end times are set, save them
	if ( (m_lStart != -1)
	||	 (m_lEnd != -1) )
	{
		if( FAILED( SaveBoundaries( pIRiffStream ) ) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// If the internal list is non-empty, save it
	if ( !m_lstInternalClipboard.IsEmpty() )
	{
		if( FAILED( SaveInternalList( pIRiffStream ) ) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Successful, set ppIStream
	*ppIStream = pIStream;
	(*ppIStream)->AddRef();

ON_ERROR:
	pIRiffStream->Release();
	pIStream->Release();

	return hr;
}

HRESULT CTimelineDataObject::SaveBoundaries( IDMUSProdRIFFStream* pIRiffStream )
{
	ASSERT( pIRiffStream );

	// Get the stream
	IStream* pIStream;
	pIStream = pIRiffStream->GetStream();

	// initialize variables
	HRESULT hr = S_OK;
	MMCKINFO ckMain;
	DWORD dwBytesWritten;

	// Create the chunk to store the Start and End data
	ckMain.ckid = FOURCC_TIMELINE_BOUNDARY;
	if( pIRiffStream->CreateChunk( &ckMain, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write the start time
	hr = pIStream->Write( &m_lStart, sizeof(long), &dwBytesWritten );
	if( FAILED( hr ) || dwBytesWritten != sizeof(long) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write the end time
	hr = pIStream->Write( &m_lEnd, sizeof(long), &dwBytesWritten );
	if( FAILED( hr ) || dwBytesWritten != sizeof(long) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Ascend out of the boundary chunk.
	pIRiffStream->Ascend( &ckMain, 0 );

ON_ERROR:
	pIStream->Release();

	return hr;
}

HRESULT CTimelineDataObject::SaveInternalList( IDMUSProdRIFFStream* pIRiffStream )
{
	ASSERT( pIRiffStream );

	// Get the stream
	IStream* pIStream;
	pIStream = pIRiffStream->GetStream();

	// initialize variables
	HRESULT hr = S_OK;
	MMCKINFO ckMain, ckList, ckSubChunk;
	DWORD dwBytesWritten;

	// Create the list to store the clipboard data
	ckMain.fccType = FOURCC_TIMELINE_LIST;
	if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Iterate through the list of CClipboardStorage objects
	CClipboardStorage *pStorage;
	POSITION pos;
	pos = m_lstInternalClipboard.GetHeadPosition();
	while( pos )
	{
		pStorage = m_lstInternalClipboard.GetNext( pos );

		TCHAR tcstr[MAX_PATH];
		long lCharCount;
		lCharCount = GetClipboardFormatName( pStorage->m_cfClipboardFormat, tcstr, MAX_PATH );
		if( lCharCount > 0 )
		{
			// Create the list to store this clipboard data
			ckList.fccType = FOURCC_TIMELINE_CLIPBOARD;
			if( pIRiffStream->CreateChunk( &ckList, MMIO_CREATELIST ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Create the chunk to store the clipboard format name
			ckSubChunk.ckid = FOURCC_TIMELINE_CLIP_NAME;
			if( pIRiffStream->CreateChunk( &ckSubChunk, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Write the format's name
			hr = pIStream->Write( tcstr, sizeof(TCHAR) * lCharCount, &dwBytesWritten );
			if( FAILED( hr ) || dwBytesWritten != sizeof(TCHAR) * lCharCount )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Ascend out of the clipboard format name chunk.
			pIRiffStream->Ascend( &ckSubChunk, 0 );

			// Create the chunk to store the clipboard data
			ckSubChunk.ckid = FOURCC_TIMELINE_CLIP_DATA;
			if( pIRiffStream->CreateChunk( &ckSubChunk, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Get the size of the stream
			STATSTG stats;
			pStorage->m_pIStream->Stat( &stats, STATFLAG_NONAME );

			// Reset the stream pointer to the beginning
			LARGE_INTEGER li;
			li.QuadPart = 0;
			pStorage->m_pIStream->Seek( li, STREAM_SEEK_SET, NULL );

			// Copy the stream to the clipboard
			hr = pStorage->m_pIStream->CopyTo( pIStream, stats.cbSize, NULL, NULL );
			if( FAILED( hr ) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Ascend out of the clipboard data chunk.
			pIRiffStream->Ascend( &ckSubChunk, 0 );

			// Ascend out of the clipboard data list chunk.
			pIRiffStream->Ascend( &ckList, 0 );
		}
	}

	// Ascend out of the cipboard data list.
	pIRiffStream->Ascend( &ckMain, 0 );

ON_ERROR:
	pIStream->Release();

	return hr;
}


HRESULT CTimelineDataObject::LoadInternalListFromStream( IStream* pIStream )
{
	// Initialize variables
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = S_OK;

	// Alloc an IDMUSProdRIFFStream from the IStream
	if( FAILED( hr = AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		return hr;
	}

	DWORD cbRead;
	
	MMCKINFO ckMain;
	
	ckMain.ckid = 0;
	ckMain.fccType = 0;
		
	while( SUCCEEDED(hr) && (pIRiffStream->Descend(&ckMain, NULL, 0) == 0) )
	{
		switch(ckMain.ckid)
		{
			case FOURCC_TIMELINE_BOUNDARY:
				// Read the start time
				hr = pIStream->Read( &m_lStart, sizeof(long), &cbRead );
				if( SUCCEEDED( hr ) && (cbRead == sizeof(long)) )
				{
					// Read the end time
					hr = pIStream->Read( &m_lEnd, sizeof(long), &cbRead );
					if( FAILED( hr ) || (cbRead != sizeof(long)) )
					{
						hr = E_FAIL;
					}
				}
				break;
			case FOURCC_LIST:
				switch(ckMain.fccType)
				{
					case  FOURCC_TIMELINE_LIST:
						hr = BuildInternalList(pIStream, pIRiffStream, ckMain);
						break;

					default:
						break;
				}
			default:
				hr = E_UNEXPECTED;
				break;
		}

		if(SUCCEEDED(hr) && pIRiffStream->Ascend(&ckMain, 0) == 0)
		{
			ckMain.ckid = 0;
			ckMain.fccType = 0;
		}
	}

	pIRiffStream->Release();

	return hr;
}

HRESULT CTimelineDataObject::BuildInternalList(	IStream* pStream,
												IDMUSProdRIFFStream *pIRiffStream,
												MMCKINFO& ckParent )
{
	ASSERT(pIRiffStream);
	ASSERT(pStream);
	HRESULT hr = S_OK;

	MMCKINFO ckNext;
	ckNext.ckid = 0;
	ckNext.fccType = 0;

	while( SUCCEEDED(hr) && (pIRiffStream->Descend(&ckNext, &ckParent, 0) == 0) )
	{
		switch(ckNext.ckid)
		{
			case FOURCC_LIST :
				switch(ckNext.fccType)
				{
					case FOURCC_TIMELINE_CLIPBOARD:
						hr = ExtractItem(pStream, pIRiffStream, ckNext);
						break;
					
					default:
						break;

				}
				break;

			default:
				break;

		}
    
		if(SUCCEEDED(hr) && pIRiffStream->Ascend(&ckNext, 0) == 0)
		{
			ckNext.ckid = 0;
			ckNext.fccType = 0;
		}
	}

	return hr;
}

HRESULT CTimelineDataObject::ExtractItem( IStream* pIStream, 
										  IDMUSProdRIFFStream *pIRiffStream, 
										  MMCKINFO& ckParent )
{
	ASSERT(pIRiffStream);
	ASSERT(pIStream);
	
	MMCKINFO ckNext;
	ckNext.ckid = 0;
	ckNext.fccType = 0;
	
	DWORD cbRead;    
	DWORD cbSize;
	TCHAR tcstr[MAX_PATH];
	HRESULT hr = S_OK;

	IStream *pIDataStream = NULL;
	UINT cfClipboardFormat = 0;

	while( SUCCEEDED(hr) && (pIRiffStream->Descend(&ckNext, &ckParent, 0) == 0) )
	{
		switch(ckNext.ckid)
		{
			case FOURCC_TIMELINE_CLIP_NAME:
				// Read the format's name
				cbSize = min( ckNext.cksize, MAX_PATH );
				memset( tcstr, 0, min( cbSize + 2, MAX_PATH * sizeof( TCHAR ) ) );
				hr = pIStream->Read( tcstr, cbSize, &cbRead );
				if( SUCCEEDED( hr ) && cbRead == cbSize )
				{
					cfClipboardFormat = RegisterClipboardFormat( tcstr );
				}
				else 
				{
					hr = E_FAIL;
				}
				break;

			case FOURCC_TIMELINE_CLIP_DATA:
				// Copy the stream to the local data stream
				hr = CreateStreamOnHGlobal( NULL, TRUE, &pIDataStream );
				if( SUCCEEDED( hr ) )
				{
					ULARGE_INTEGER ui;
					ui.QuadPart = ckNext.cksize;

					hr = pIStream->CopyTo( pIDataStream, ui, NULL, NULL );
				}
				break;

			default:
				break;
		}
		if( SUCCEEDED(hr) && pIRiffStream->Ascend(&ckNext, 0) == 0 )
		{
			ckNext.ckid = 0;
			ckNext.fccType = 0;
		}
	}

	if( SUCCEEDED(hr) && cfClipboardFormat && pIDataStream )
	{
		CClipboardStorage *pStorage;
		pStorage = new CClipboardStorage( cfClipboardFormat, pIDataStream ); 
		m_lstInternalClipboard.AddTail( pStorage );
	}

	if( pIDataStream )
	{
		pIDataStream->Release();
	}

	return hr;
}