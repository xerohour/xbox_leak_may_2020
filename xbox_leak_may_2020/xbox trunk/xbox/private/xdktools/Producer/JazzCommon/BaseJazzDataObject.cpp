// BaseJazzDataObject.cpp : Implementation of CBaseJazzDataObject

#include <afxole.h>         // MFC OLE classes

#include "BaseJazzDataObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CJzClipData constructor/destructor 

CJzClipData::CJzClipData( UINT uClipFormat, IStream* pIStream )
{
	ASSERT( uClipFormat != 0 );
	ASSERT( pIStream != NULL );

    m_uClipFormat = uClipFormat;

	m_pIStream = pIStream;
	m_pIStream->AddRef();
}


CJzClipData::~CJzClipData()
{
	if( m_pIStream )
	{
		m_pIStream->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CBaseJazzDataObject Constructor/Destructor

CBaseJazzDataObject::CBaseJazzDataObject()
{
	m_dwRef = 0;
	AddRef();

	m_pEnumFormats = NULL;
}

CBaseJazzDataObject::~CBaseJazzDataObject()
{
	// Clean up any references we have.
	if( m_pEnumFormats != NULL )
	{
		m_pEnumFormats->Release();
		m_pEnumFormats = NULL;
	}

	// Free ClipData list.
	CJzClipData* pJzClipData;

	while( !m_lstClips.IsEmpty() )
	{
		pJzClipData = static_cast<CJzClipData*>( m_lstClips.RemoveHead() );
		delete pJzClipData;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CBaseJazzDataObject IUnknown implementation

HRESULT CBaseJazzDataObject::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
    if( ::IsEqualIID(riid, IID_IDataObject)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDataObject *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CBaseJazzDataObject::AddRef()
{
    return ++m_dwRef;
}

ULONG CBaseJazzDataObject::Release()
{
    ASSERT(m_dwRef != 0);

    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseJazzDataObject IDataObject implementation

/////////////////////////////////////////////////////////////////////////////
// IDataObject::GetData
//
//	This is the standard call to retrieve the data in the object in a medium
//	allocated by the object.
//
//	Return values: standard error codes and the following
//		S_OK:				Data was successfully retrieved.
//		DV_E_XXXX:			XXXX was invalid
//		STG_E_MEDIUMFULL:	Could not allocate the medium.
/////////////////////////////////////////////////////////////////////////////
HRESULT CBaseJazzDataObject::GetData( FORMATETC *pformatetcIn, STGMEDIUM *pmedium )
{
	HRESULT	hr;

	ASSERT( pmedium != NULL );
	if( pmedium == NULL )
	{
		return E_POINTER;
	}

	ASSERT( pformatetcIn != NULL );
	if( pformatetcIn == NULL )
	{
		return E_POINTER;
	}

	// We only support IStream format
	if( !(pformatetcIn->tymed & TYMED_ISTREAM) )
	{
		return DV_E_FORMATETC;
	}

	// Allocate the stream
	if( FAILED( CreateStreamOnHGlobal( NULL, TRUE, &(pmedium->pstm) ) ) )
	{
		return STG_E_MEDIUMFULL;
	}

	// Setup the medium to be filled.
	pmedium->tymed = TYMED_ISTREAM;

	hr = GetDataHere( pformatetcIn, pmedium );
	if( FAILED( hr ) )
	{
		pmedium->pstm->Release();
		pmedium->pstm = NULL;
		pmedium->tymed = 0;
		pmedium->pUnkForRelease	= NULL;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// IDataObject::GetDataHere
//
//	This function does the same as GetData, but the caller has already
//	allocated the requested medium.
/////////////////////////////////////////////////////////////////////////////
HRESULT CBaseJazzDataObject::GetDataHere( FORMATETC *pformatetcIn, STGMEDIUM *pmedium )
{
	HRESULT hr;

	// Validate pformatetcIn
	ASSERT( pformatetcIn != NULL );
	if( pformatetcIn == NULL )
	{
		return E_INVALIDARG;
	}

	if( pformatetcIn->lindex != -1 )
	{
		return DV_E_LINDEX;
	}

	if( pformatetcIn->dwAspect != DVASPECT_CONTENT)
	{
		return DV_E_DVASPECT;
	}

	if( !(pformatetcIn->tymed & TYMED_ISTREAM) )
	{
		return DV_E_FORMATETC;
	}

	// Validate pmedium
	ASSERT( pmedium != NULL );
	if( pmedium == NULL )
	{
		return E_POINTER;
	}

	if( pmedium->tymed != TYMED_ISTREAM )
	{
		return E_INVALIDARG;
	}

	if( pmedium->pstm == NULL )
	{
		return E_INVALIDARG;
	}

	//Determine whether the format is supported.
	if( (m_pEnumFormats == NULL)
	||  (m_pEnumFormats->FormatInList(pformatetcIn->cfFormat) != S_OK) )
	{
		return DV_E_FORMATETC;
	}

	// Find the stream associated with pformatetcIn->cfFormat
	IStream* pIStream = FindStream( pformatetcIn->cfFormat );
	if( pIStream == NULL )
	{
		return E_FAIL;
	}

	STATSTG			statstg;
	ULARGE_INTEGER	uliSizeOut, uliSizeRead, uliSizeWritten;
	LARGE_INTEGER	liTemp;

	// Copy pIStream into the provided stream

	// Get our stream's size
	hr = pIStream->Stat( &statstg, STATFLAG_NONAME );
	if( FAILED( hr ) )
	{
		return E_FAIL;
	}
	uliSizeOut = statstg.cbSize;

	// Copy the entire stream to the one passed in.
	// First, seek to the beginning.
	liTemp.QuadPart = 0;
	pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

	// Then copy it.
	hr = pIStream->CopyTo( pmedium->pstm, uliSizeOut, &uliSizeRead, &uliSizeWritten );
	if( FAILED( hr )
	||  uliSizeRead.QuadPart != uliSizeOut.QuadPart
	||  uliSizeWritten.QuadPart != uliSizeOut.QuadPart )
	{
		// If the storage ran out of space, return STG_E_MEDIUMFULL, otherwise just return E_FAIL
		if( hr == STG_E_MEDIUMFULL )
		{
			return hr;
		}
		return E_FAIL;
	}

	// The user should just call the IStream->Release to release the medium.
	pmedium->pUnkForRelease	= NULL;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IDataObject::QueryGetData
//
//	This method is used to determine whether a subsequent call to GetData with
//	the specified FORMATETC is likely to succeed.
//
//	Return values: standard error codes and the following
//		S_OK:				A subsequent call to GetData would probably succeed.
//		DV_E_XXXX:			XXXX was invalid
/////////////////////////////////////////////////////////////////////////////
HRESULT CBaseJazzDataObject::QueryGetData( FORMATETC *pformatetc )
{
	// Validate pformatetc
	ASSERT( pformatetc != NULL );
	if( pformatetc == NULL )
	{
		return E_INVALIDARG;
	}

	if( pformatetc->lindex != -1 )
	{
		return DV_E_LINDEX;
	}

	if( pformatetc->dwAspect != DVASPECT_CONTENT )
	{
		return DV_E_DVASPECT;
	}

	if( !(pformatetc->tymed & TYMED_ISTREAM) )
	{
		return DV_E_FORMATETC;
	}

	// Determine whether this object contains the requested clipboard format
	if( m_pEnumFormats
	&&  m_pEnumFormats->FormatInList( pformatetc->cfFormat ) == S_OK )
	{
		return S_OK;
	}

	// This FORMATETC should not be passed to GetData
	return DV_E_FORMATETC;
}


/////////////////////////////////////////////////////////////////////////////
// IDataObject::GetCanonicalFormatEtc
//
//	This method is used to find the simplest FORMATETC which will return
//	the same data from GetData as the FORMATETC which is passed in.
//
//	Return values: standard error codes and the following
//		S_OK:					pformatetcOut is a simpler form of pformatetcIn.
//		DATA_S_SAMEFORMATETC:	The FORMATETC structures are the same and NULL is
//								returned in pformatetcOut. 
//		DV_E_XXXX:				XXXX was invalid
/////////////////////////////////////////////////////////////////////////////
HRESULT CBaseJazzDataObject::GetCanonicalFormatEtc( FORMATETC *pformatetcIn, FORMATETC *pformatetcOut )
{
	HRESULT hr;

	// Validate pformpformatetcOutatetc
	ASSERT( pformatetcOut != NULL );
	if( pformatetcOut == NULL )
	{
		return E_POINTER;
	}

	// Validate pformatetcIn
	ASSERT( pformatetcIn != NULL );
	if( pformatetcIn == NULL )
	{
		return E_INVALIDARG;
	}

	if( pformatetcIn->lindex != -1 )
	{
		return DV_E_LINDEX;
	}

	if(  pformatetcIn->dwAspect != DVASPECT_CONTENT
	|| !(pformatetcIn->tymed & TYMED_ISTREAM)
	||   pformatetcIn->cfFormat == 0 )
	{
		return DV_E_FORMATETC;
	}

	// Determine whether this object contains the requested clipboard format
	if( m_pEnumFormats == NULL
	||  m_pEnumFormats->FormatInList( pformatetcIn->cfFormat ) != S_OK )
	{
		return DV_E_FORMATETC;
	}

	hr = S_OK;

	if( pformatetcIn->ptd == NULL )
	{
		hr = DATA_S_SAMEFORMATETC;
	}

	// Copy pformatetcIn to pformatetcOut, but make sure ptd is NULL
	pformatetcOut->ptd		= NULL;
	pformatetcOut->lindex	= pformatetcIn->lindex;
	pformatetcOut->tymed	= pformatetcIn->tymed;
	pformatetcOut->dwAspect	= pformatetcIn->dwAspect;
	pformatetcOut->cfFormat	= pformatetcIn->cfFormat;

	return hr;
}


// The SetData function is not implemented.
HRESULT CBaseJazzDataObject::SetData( FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// IDataObject::EnumFormatEtc
//
//	This method returns an IEnumFORMATETC interface which enumerates the
//	formats supported by this object.
//
//	Return values: standard error codes and the following
//		S_OK:	The function succeeded.
/////////////////////////////////////////////////////////////////////////////
HRESULT CBaseJazzDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc )
{
	HRESULT hr;

	// Validate the argument
	ASSERT(ppenumFormatEtc != NULL);
	if( ppenumFormatEtc == NULL )
	{
		return E_POINTER;
	}

	// Make sure we have an interface pointer to pass back
	if( m_pEnumFormats == NULL )
	{
		return E_FAIL;
	}

	// We ignore dwDirection since we have the same FORMATETC's for getting and setting
	hr = m_pEnumFormats->Clone( ppenumFormatEtc );
	if( FAILED( hr ) )
	{
		return E_FAIL;
	}

	(*ppenumFormatEtc)->Reset();

	return hr;
}


// The Advise functions are not implemented.
HRESULT CBaseJazzDataObject::DAdvise( FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink,
								 DWORD *pdwConnection )
{
	return E_NOTIMPL;
}


HRESULT CBaseJazzDataObject::DUnadvise( DWORD dwConnection )
{
	return E_NOTIMPL;
}


HRESULT CBaseJazzDataObject::EnumDAdvise( IEnumSTATDATA **ppenumAdvise )
{
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////////////
// Additional private functions
/////////////////////////////////////////////////////////////////////////////////////
IStream* CBaseJazzDataObject::FindStream( UINT uClipFormat )
{
	IStream* pIStream = NULL;

	POSITION pos = m_lstClips.GetHeadPosition();

	while( pos )
	{
		CJzClipData* pJzClipData = m_lstClips.GetNext( pos );

		if( pJzClipData->m_uClipFormat == uClipFormat )
		{
			pIStream = pJzClipData->m_pIStream;
//			pIStream->AddRef();		intentionally missing
			break;
		}
	}

	return pIStream;
}


/////////////////////////////////////////////////////////////////////////////////////
// Additional functions
//
//	Note: Jazz developers should use these methods instead of the IDataObject methods
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CBaseJazzDataObject::AddClipFormat
//
//	This method sets the internal variables so that the data object will
//	be valid.
//
//	Return values:
//		S_OK on success, or a standard error code.
/////////////////////////////////////////////////////////////////////////////
HRESULT CBaseJazzDataObject::AddClipFormat( UINT uClipFormat, IStream *pIStream )
{
	HRESULT	hr;

	ASSERT( uClipFormat != 0 );
	ASSERT( pIStream != NULL );

	// Validate the arguments.
	if( uClipFormat == 0
	||  pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	// Create CJzClipData to store clipboard format and associated stream
	CJzClipData* pJzClipData = new CJzClipData( uClipFormat, pIStream );
	if( pJzClipData == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Add pJzClipData to m_lstClips
	m_lstClips.AddTail( pJzClipData );

	// Get a valid pointer to an IEnumFORMATETC interface
	if( m_pEnumFormats == NULL )
	{
		// Attempt to create a new CJazzEnumFormatEtc
		m_pEnumFormats = new CJazzEnumFormatEtc();
		if( m_pEnumFormats == NULL )
		{
			return E_OUTOFMEMORY;
		}
	}

	// Set FORMATETC's clipboard format to the one passed in.
	FORMATETC fe;

	ASSERT( uClipFormat <= USHRT_MAX );
	fe.cfFormat = (CLIPFORMAT)uClipFormat;
	fe.ptd		= NULL;
	fe.dwAspect	= DVASPECT_CONTENT;
	fe.lindex	= -1;
	fe.tymed	= TYMED_ISTREAM;

	// Add this data objects FORMATETC to m_pEnumFormats's list.
	hr = m_pEnumFormats->AddFormat( &fe );
	if( FAILED( hr ) )
	{
		return E_FAIL;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseJazzDataObject::IsClipFormatAvailable
//
//	This method determines whether the specified IDataObject supports the
//	specified clipboard format.  This is generally called to determine whether
//	a paste or drop command would succeed.
//
//	Return values:
//		S_OK:			The specified clipboard format is supported.
//		E_INVALIDARG:	One of the arguments was invalid.
//		E_FAIL:			The specified clipboard format is not supported
/////////////////////////////////////////////////////////////////////////////
HRESULT	CBaseJazzDataObject::IsClipFormatAvailable( IDataObject *pIDataObject, UINT uClipFormat )
{
	HRESULT	hr;

	ASSERT( pIDataObject != NULL );
	ASSERT( uClipFormat != 0 );

	// Validate the arguments.
	if( pIDataObject == NULL
	||  uClipFormat == 0 )
	{
		return E_INVALIDARG;
	}

	// Set FORMATETC's clipboard format to the one passed in.
	FORMATETC fe;

	ASSERT( uClipFormat <= USHRT_MAX );
	fe.cfFormat = (CLIPFORMAT)uClipFormat;
	fe.ptd		= NULL;
	fe.dwAspect	= DVASPECT_CONTENT;
	fe.lindex	= -1;
	fe.tymed	= TYMED_ISTREAM;

	// See if the data object supports this format
	hr = pIDataObject->QueryGetData( &fe );
	if( hr != S_OK )
	{
		// The object does not support the specified format.
		hr = E_FAIL;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CBaseJazzDataObject::AttemptRead
//
//	This method attempts to read the specified IDataObject into this
//	CBaseJazzDataObject.
//
//	Return values:
//		S_OK:	The data was read.
//		E_FAIL:	An error occurred, and the data could not be read.
/////////////////////////////////////////////////////////////////////////////

HRESULT CBaseJazzDataObject::AttemptRead( IDataObject *pIDataObject, UINT uClipFormat, IStream** ppIStream  )
{
	STGMEDIUM stgMedium;
	LARGE_INTEGER liTemp;
	HRESULT	hr;

	ASSERT( pIDataObject != NULL );
	ASSERT( uClipFormat != 0 );
	ASSERT( ppIStream != NULL );

	// Validate the arguments.
	if( ppIStream == NULL )
	{
		return E_POINTER;
	}

	*ppIStream = NULL;

	if( pIDataObject == NULL
	||  uClipFormat == 0 )
	{
		return E_INVALIDARG;
	}

	// Set FORMATETC's clipboard format to the one passed in.
	FORMATETC fe;

	ASSERT( uClipFormat <= USHRT_MAX );
	fe.cfFormat = (CLIPFORMAT)uClipFormat;
	fe.ptd		= NULL;
	fe.dwAspect	= DVASPECT_CONTENT;
	fe.lindex	= -1;
	fe.tymed	= TYMED_ISTREAM;

	// Attempt to get the data.
	hr = pIDataObject->GetData( &fe, &stgMedium );
	if( FAILED( hr ) )
	{
		return E_FAIL;
	}

	if( stgMedium.tymed != TYMED_ISTREAM )
	{
		return E_FAIL;
	}

	if( stgMedium.pstm == NULL )
	{
		return E_UNEXPECTED;
	}


	liTemp.QuadPart = 0;
	stgMedium.pstm->Seek( liTemp, STREAM_SEEK_SET, NULL );

	*ppIStream = stgMedium.pstm;
	return S_OK;
}
