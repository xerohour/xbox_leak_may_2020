// JazzDataObject.cpp : Implementation of CJazzDataObject

#include <afxole.h>         // MFC OLE classes

#include "JazzDataObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject Constructor/Destructor

CJazzDataObject::CJazzDataObject(IStream *pIStream) :
				 m_bValid(FALSE),
				 m_pEnumFormats(NULL),
				 m_dwRef(0)
{
	// Set the stream pointer
	m_pIStream = pIStream;
	if(pIStream != NULL)
	{
		m_pIStream->AddRef();
	}

	// Initialize the FORMATETC's clipboard format.
	m_feCurrentFormat.cfFormat	= 0;

	// Initialize the FORMATETC.  These values should never change.
	m_feCurrentFormat.ptd		= NULL;
	m_feCurrentFormat.dwAspect	= DVASPECT_CONTENT;
	m_feCurrentFormat.lindex		= -1;
	m_feCurrentFormat.tymed		= TYMED_ISTREAM;

	AddRef();
}

CJazzDataObject::~CJazzDataObject()
{
	// Clean up any references we have.
	if(m_pIStream != NULL)
	{
		m_pIStream->Release();
		m_pIStream = NULL;
	}
	if(m_pEnumFormats != NULL)
	{
		m_pEnumFormats->Release();
		m_pEnumFormats = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject IUnknown implementation

HRESULT CJazzDataObject::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

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

ULONG CJazzDataObject::AddRef()
{
    return ++m_dwRef;
}

ULONG CJazzDataObject::Release()
{
    ASSERT(m_dwRef != 0);

    --m_dwRef;

    if(m_dwRef == 0)
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject IDataObject implementation

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
HRESULT CJazzDataObject::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT	hr;

	ASSERT(pmedium != NULL);
	if(pmedium == NULL)
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
	if(FAILED(CreateStreamOnHGlobal(NULL, TRUE, &(pmedium->pstm))))
	{
		return E_OUTOFMEMORY;
	}

	// Setup the medium to be filled.
	pmedium->tymed = TYMED_ISTREAM;

	hr = GetDataHere(pformatetcIn, pmedium);
	if(FAILED(hr))
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
HRESULT CJazzDataObject::GetDataHere(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	ASSERT(pmedium != NULL);
	if(pmedium == NULL)
	{
		return E_POINTER;
	}

	// Make sure the object is valid (SetStream and AddClipFormat have been called)
	ASSERT(m_bValid);

	if(!m_bValid)
	{
		return E_FAIL;
	}

	// Check all the arguments
	ASSERT(pformatetcIn != NULL);
	if(pformatetcIn == NULL)
	{
		return E_INVALIDARG;
	}

	if(pformatetcIn->lindex != -1)
	{
		return DV_E_LINDEX;
	}

	if(pformatetcIn->dwAspect != DVASPECT_CONTENT)
	{
		return DV_E_DVASPECT;
	}

	if(!(pformatetcIn->tymed & TYMED_ISTREAM))
	{
		return DV_E_FORMATETC;
	}

	//Determine whether the format is supported.
	ASSERT(m_pEnumFormats != NULL);
	if(m_pEnumFormats == NULL)
	{
		return E_UNEXPECTED;
	}
	if(m_pEnumFormats->FormatInList(pformatetcIn->cfFormat) != S_OK)
	{
		return DV_E_FORMATETC;
	}

	// The pmedium should have been setup before calling this function.
	if(pmedium->tymed != TYMED_ISTREAM)
	{
		return E_INVALIDARG;
	}

	if(pmedium->pstm == NULL)
	{
		return E_INVALIDARG;
	}

	// m_bValid means m_pIStream and m_pEnumFormats are not NULL
	ASSERT(m_pIStream != NULL);
	if(m_pIStream == NULL)
	{
		return E_UNEXPECTED;
	}

	STATSTG			statstg;
	ULARGE_INTEGER	uliSizeOut, uliSizeRead, uliSizeWritten;
	LARGE_INTEGER	liTemp;

	// Copy m_pIStream into the provided stream

	// Get our stream's size
	hr = m_pIStream->Stat(&statstg, STATFLAG_NONAME);
	if(FAILED(hr))
	{
		return E_FAIL;
	}
	uliSizeOut = statstg.cbSize;

	// Copy the entire stream to the one passed in.
	// First, seek to the beginning.
	liTemp.QuadPart = 0;
	m_pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);

	// Then copy it.
	hr = m_pIStream->CopyTo(pmedium->pstm, uliSizeOut, &uliSizeRead, &uliSizeWritten);
	if(FAILED(hr) || uliSizeRead.QuadPart != uliSizeOut.QuadPart || uliSizeWritten.QuadPart != uliSizeOut.QuadPart)
	{
		// If the storage ran out of space, return STG_E_MEDIUMFULL, otherwise just return E_FAIL
		if(hr == STG_E_MEDIUMFULL)
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
HRESULT CJazzDataObject::QueryGetData(FORMATETC *pformatetc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Check all the arguments
	ASSERT(pformatetc != NULL);
	if(pformatetc == NULL)
	{
		return E_INVALIDARG;
	}

	if(pformatetc->lindex != -1)
	{
		return DV_E_LINDEX;
	}

	if(pformatetc->dwAspect != DVASPECT_CONTENT)
	{
		return DV_E_DVASPECT;
	}

	if(!(pformatetc->tymed & TYMED_ISTREAM))
	{
		return DV_E_FORMATETC;
	}

	// First make sure the object is valid (SetStream and AddClipFormat have been called)
	ASSERT(m_bValid);
	
	if(m_bValid)
	{
		ASSERT(m_pEnumFormats != NULL);
		if(m_pEnumFormats == NULL)
		{
			return E_UNEXPECTED;
		}

		if(m_pEnumFormats->FormatInList(pformatetc->cfFormat) == S_OK)
		{
			return S_OK;
		}

		// This FORMATETC should not be passed to GetData
		return DV_E_FORMATETC;
	}

	// The object was invalid
	return E_FAIL;
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
HRESULT CJazzDataObject::GetCanonicalFormatEtc(FORMATETC *pformatetcIn, FORMATETC *pformatetcOut)
{
	HRESULT hr;

	// Check all the arguments
	ASSERT(pformatetcOut != NULL);
	if(pformatetcOut == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pformatetcIn != NULL);
	if(pformatetcIn == NULL)
	{
		return E_INVALIDARG;
	}

	if(pformatetcIn->lindex != -1)
	{
		return DV_E_LINDEX;
	}

	if(pformatetcIn->dwAspect != DVASPECT_CONTENT || !(pformatetcIn->tymed & TYMED_ISTREAM) ||
	   pformatetcIn->cfFormat == 0)
	{
		return DV_E_FORMATETC;
	}

	// Check the clipboard format
	ASSERT(m_pEnumFormats != NULL);
	if(m_pEnumFormats == NULL)
	{
		return E_UNEXPECTED;
	}

	if(m_pEnumFormats->FormatInList(pformatetcIn->cfFormat) != S_OK)
	{
		return DV_E_FORMATETC;
	}

	hr = S_OK;

	if(pformatetcIn->ptd == NULL)
	{
		// pformatetcIn is the same as m_feCurrentFormat
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

/////////////////////////////////////////////////////////////////////////////
// IDataObject::SetData
//
//	This is the standard method to copy data from a storage medium into an
//	object where the format is specified in pformatetc.  If fRelease is TRUE,
//	SetData also frees the storage medium when finished with it.
//
//	Return values: standard error codes and the following
//		S_OK:				A subsequent call to GetData would probably succeed.
//		DV_E_XXXX:			XXXX was invalid
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzDataObject::SetData(FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT			hr;
	ULARGE_INTEGER	uliTemp;

	// Check the arguments.
	ASSERT(pformatetc != NULL && pmedium != NULL);
	if(pformatetc == NULL || pmedium == NULL)
	{
		return E_INVALIDARG;
	}

	if(pformatetc->lindex != -1)
	{
		return DV_E_LINDEX;
	}

	if(pformatetc->dwAspect != DVASPECT_CONTENT)
	{
		return DV_E_DVASPECT;
	}

	if(!(pformatetc->tymed & TYMED_ISTREAM) || pmedium->tymed != TYMED_ISTREAM)
	{
		return DV_E_TYMED;
	}

	// Check the clipboard format.
	ASSERT(m_pEnumFormats != NULL);
	if(m_pEnumFormats == NULL)
	{
		return E_UNEXPECTED;
	}

	if(m_pEnumFormats->FormatInList(pformatetc->cfFormat) != S_OK)
	{
		return DV_E_FORMATETC;
	}

	ASSERT(pmedium->pstm != NULL);
	if(pmedium->pstm == NULL)
	{
		return E_INVALIDARG;
	}

	// The data is valid, now get it.
	// If we already have a stream, release it.
	if(m_pIStream != NULL)
	{
		m_pIStream->Release();
		m_pIStream = NULL;
	}
	
	// We can't just set the pointer to the other object's stream, because that application
	// might exit.  So allocate a new stream.
	hr = CreateStreamOnHGlobal(NULL, TRUE, &m_pIStream);
	if(FAILED(hr))
	{
		m_bValid = FALSE;
		return E_OUTOFMEMORY;
	}

	// Set uliTemp to -1 to copy the entire stream.
	uliTemp.QuadPart = -1;

	// Copy the stream into our stream
	hr = pmedium->pstm->CopyTo(m_pIStream, uliTemp, NULL, NULL);
	if(FAILED(hr))
	{
		m_bValid = FALSE;
		m_pIStream->Release();
		m_pIStream = NULL;
		return E_OUTOFMEMORY;
	}

	// If m_pEnumFormats is valid, then the object is now valid
	if(m_pEnumFormats != NULL)
	{
		m_bValid = TRUE;
	}

	// If the caller wants the medium released, release it.
	if(fRelease)
	{
		ReleaseStgMedium(pmedium);
	}

	return S_OK;
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
HRESULT CJazzDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	// Validate the argument
	ASSERT(ppenumFormatEtc != NULL);
	if(ppenumFormatEtc == NULL)
	{
		return E_POINTER;
	}

	// Make sure we have an interface pointer to pass back
	if(m_pEnumFormats == NULL)
	{
		return E_FAIL;
	}

	// We ignore dwDirection since we have the same FORMATETC's for getting and setting
	hr = m_pEnumFormats->Clone(ppenumFormatEtc);
	if(FAILED(hr))
	{
		return E_FAIL;
	}
	return hr;
}

// The Advise functions are not implemented.
HRESULT CJazzDataObject::DAdvise(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink,
								 DWORD *pdwConnection)
{
	return E_NOTIMPL;
}

HRESULT CJazzDataObject::DUnadvise(DWORD dwConnection)
{
	return E_NOTIMPL;
}

HRESULT CJazzDataObject::EnumDAdvise(IEnumSTATDATA **ppenumAdvise)
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////////////
// Additional functions
//
//	Note: Jazz developers should use these methods instead of the IDataObject methods
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject::Initialize
//
//	This method sets the internal variables so that the data object will
//	be valid.
//
//	Return values:
//		S_OK on success, or a standard error code.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzDataObject::Initialize(UINT uClipFormat, IStream *pIStream)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	// Attempt to set the clipboard format.
	hr = SetCurrentClipFormat(uClipFormat);
	if(FAILED(hr))
	{
		return hr;
	}

	ASSERT(m_feCurrentFormat.cfFormat != 0);

	// Get a valid pointer to an IEnumFORMATETC interface
	if(m_pEnumFormats == NULL)
	{
		// Attempt to create a new CJazzEnumFormatEtc
		m_pEnumFormats = new CJazzEnumFormatEtc();
		if(m_pEnumFormats == NULL)
		{
			return E_OUTOFMEMORY;
		}

		// Add this data objects FORMATETC to m_pEnumFormats's list.
		hr = m_pEnumFormats->AddFormat(&m_feCurrentFormat);
		if(FAILED(hr))
		{
			return E_FAIL;
		}
	}

	// Attempt to set the IStream pointer.
	// This call should set m_bValid to TRUE
	hr = SetIStream(pIStream);
	if(SUCCEEDED(hr))
	{
		ASSERT(m_bValid);
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject::GetIStream
//
//	This method returns an AddRef'd pointer to the object's IStream
//
//	Return values:
//		S_OK:		A non-NULL value for m_pIStream was returned.
//		S_FALSE:	m_pIStream is NULL, but no error occurred.
//		E_POINTER:	ppIStream was invalid
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzDataObject::GetIStream(IStream **ppIStream)
{
	if(ppIStream == NULL)
	{
		return E_POINTER;
	}

	*ppIStream = m_pIStream;
	if(m_pIStream == NULL)
	{
		return S_FALSE;
	}

	m_pIStream->AddRef();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject::SetIStream
//
//	This method sets m_pIStream to the IStream specified
//
//	Return values: S_OK
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzDataObject::SetIStream(IStream *pIStream)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pIStream)
	{
		m_pIStream->Release();
	}

	// Set m_pIStream and update m_bValid
	m_pIStream = pIStream;
	if(m_pIStream)
	{
		m_pIStream->AddRef();
		if(m_pEnumFormats != NULL)
		{
			m_bValid = TRUE;
		}
	}
	else
	{
		m_bValid = FALSE;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject::AddClipFormat
//
//	This method adds a clipboard format to the list recognized by this object.
//	If there was no valid clipboard format before, this also sets the current
//	clipboard format to uClipFormat.
//
//	Return values:
//		S_OK:			The function succeeded.
//		E_INVALIDARG:	The clipboard format was invalid.
//		E_OUTOFMEMORY:	Out of memory.
//		E_FAIL:			Unable to add the format.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzDataObject::AddClipFormat(UINT uClipFormat)
{
	HRESULT hr;
	UINT	cfTempFormat;

	if(uClipFormat == 0)
	{
		return E_INVALIDARG;
	}

	if(m_pEnumFormats == NULL)
	{
		// Attempt to create a new CJazzEnumFormatEtc
		m_pEnumFormats = new CJazzEnumFormatEtc();
		if(m_pEnumFormats == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}

	ASSERT( uClipFormat <= USHRT_MAX );
	cfTempFormat = m_feCurrentFormat.cfFormat;
	m_feCurrentFormat.cfFormat = (CLIPFORMAT)uClipFormat;

	// Add the FORMATETC to m_pEnumFormats's list.
	hr = m_pEnumFormats->AddFormat(&m_feCurrentFormat);
	// If the current format was invalid, leave it valid with the new format,
	// otherwise, set it back to what it was.
	if(cfTempFormat != 0)
	{
	    ASSERT( uClipFormat <= USHRT_MAX );
		m_feCurrentFormat.cfFormat = (CLIPFORMAT)cfTempFormat;
	}
	if(FAILED(hr))
	{
		return E_FAIL;
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject::RemoveClipFormat
//
//	This method removes the specified clipboard format from the list of
//	recognized formats.
//
//	Return values:
//		S_OK:			The format was successfully removed.
//		E_INVALIDARG:	The clipboard format was invalid.
//		E_FAIL:			The format could not be removed.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzDataObject::RemoveClipFormat(UINT uClipFormat)
{
	HRESULT hr;
	UINT	cfTempFormat;

	if(uClipFormat == 0)
	{
		return E_INVALIDARG;
	}

	if(m_pEnumFormats == NULL)
	{
		// Can't remove a clipboard format when there aren't any.
		return E_FAIL;
	}

	ASSERT( uClipFormat <= USHRT_MAX );
	cfTempFormat = m_feCurrentFormat.cfFormat;
	m_feCurrentFormat.cfFormat = (CLIPFORMAT)uClipFormat;

	ASSERT(cfTempFormat != 0);
	if(cfTempFormat == 0)
	{
		return E_UNEXPECTED;
	}

	// Remove the FORMATETC from m_pEnumFormats's list.
	hr = m_pEnumFormats->RemoveFormat(&m_feCurrentFormat);
	if(FAILED(hr))
	{
		return E_FAIL;
	}

	// Reset the current clipboard format
	if(cfTempFormat != uClipFormat)
	{
		ASSERT( uClipFormat <= USHRT_MAX );
		m_feCurrentFormat.cfFormat = (CLIPFORMAT)cfTempFormat;
	}
	else
	{
		FORMATETC fe;

		// Set the current format to the first in the list.
		m_pEnumFormats->Reset();
		hr = m_pEnumFormats->Next(1, &fe, NULL);
		if(hr == S_OK)
		{
			m_feCurrentFormat.cfFormat = fe.cfFormat;
		}
		else
		{
			m_feCurrentFormat.cfFormat = 0;
		}
	}
		
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject::SetCurrentClipFormat
//
//	This method sets the current clipboard format for the data object.
//	Note: This doesn't add the format to the list of formats supported
//	by the object.  Call AddClipFormat to add the format to the list.
//
//	Return values:
//		S_OK:			The function succeeded.
//		E_INVALIDARG:	The clipboard format was invalid.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzDataObject::SetCurrentClipFormat(UINT uClipFormat)
{
	if(uClipFormat == 0)
	{
		return E_INVALIDARG;
	}

	ASSERT( uClipFormat <= USHRT_MAX );
	m_feCurrentFormat.cfFormat = (CLIPFORMAT)uClipFormat;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject::DataObjectHasClipFormat
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
HRESULT CJazzDataObject::DataObjectHasClipFormat(UINT uClipFormat, IDataObject *pIDataObject)
{
	// Validate the arguments.
	if(uClipFormat == 0)
	{
		return E_INVALIDARG;
	}

	if(pIDataObject == NULL)
	{
		return E_INVALIDARG;
	}

	UINT	cfTempFormat;
	HRESULT	hr;

	// Temporarily set m_feCurrentFormat's clipboard format to the one passed in.
	ASSERT( uClipFormat <= USHRT_MAX );
	cfTempFormat = m_feCurrentFormat.cfFormat;
	m_feCurrentFormat.cfFormat = (CLIPFORMAT)uClipFormat;

	// See if the data object supports this format
	hr = pIDataObject->QueryGetData(&m_feCurrentFormat);

	// Set m_feCurrentFormat's clipboard format back.
	ASSERT( uClipFormat <= USHRT_MAX );
	m_feCurrentFormat.cfFormat = (CLIPFORMAT)cfTempFormat;

	if(FAILED(hr))
	{
		// The object does not support the specified format.
		return E_FAIL;
	}

	// QueryGetData succeeded; the format is supported
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CJazzDataObject::AttemptRead
//
//	This method attempts to read the specified IDataObject into this
//	CJazzDataObject.
//
//	Return values:
//		S_OK:	The data was read.
//		E_FAIL:	An error occurred, and the data could not be read.
/////////////////////////////////////////////////////////////////////////////
HRESULT CJazzDataObject::AttemptRead(IDataObject *pIDataObject)
{
	HRESULT			hr;
	STGMEDIUM		stgMedium;
	LARGE_INTEGER	liTemp;

	// Make sure we have been initialized
	if(m_feCurrentFormat.cfFormat == 0)
	{
		return E_FAIL;
	}

	// Attempt to get the data.
	hr = pIDataObject->GetData(&m_feCurrentFormat, &stgMedium);
	if(FAILED(hr))
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
	stgMedium.pstm->Seek(liTemp, STREAM_SEEK_SET, NULL);

	// Attempt to set the data
	hr = SetData(&m_feCurrentFormat, &stgMedium, FALSE);

	// Release the storage medium.
	ReleaseStgMedium(&stgMedium);

	if(FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}
