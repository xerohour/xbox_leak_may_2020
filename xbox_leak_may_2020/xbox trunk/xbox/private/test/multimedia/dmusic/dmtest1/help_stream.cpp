/****************************************************************************

    Contains the implementation of CFileStream

    Copyright (C) Microsoft Corporation  1994 - 1999
    
    Stolen and Modified 3/5/96 by Mark Burton from 10/10/95 Brian McDowell

    07/28/1998 - t-jwex - added unicode support to the methods/functions contained
						here.  Also updated all old 16-bit File I/O calls to 
						respective Win32 calls.
    08/13/1998 - jimmo  - Changed CFileStream::Read so it doesn't fail when
                        you send in a NULL for BytesRead param.
    11/11/1999 - danhaff - copied into dsapitst8.
                           added fake g_isUnicode variabel.

*******************************************************************************/
#include <globals.h>
#include "Help_Stream.h"

using namespace LocalFileStream;

namespace LocalFileStream
{

#define FILE_BEGIN	0
#define FILE_CURRENT	1
#define	FILE_END	2



// ************************************************************************
// ************************************************************************
//
//  Unicode functions
//
// ************************************************************************
// ************************************************************************


// ---------------------------------------------------
//  AllocFileStream()
// ---------------------------------------------------
HRESULT STDAPICALLTYPE AllocFileStream( LPSTREAM FAR *ppStream, 
									   LPCSTR szAbsFilePath, 
									   DWORD dwOffset, 
									   DWORD dwFlags )
{
	HRESULT		hr	= E_OUTOFMEMORY;
    CFileStream	*pStream;
	
	pStream = new CFileStream;

	if ( pStream )
	{
		pStream->AddRef();

		hr = pStream->Init( szAbsFilePath, dwOffset, dwFlags );

		if ( SUCCEEDED(hr) )
		{
			*ppStream = pStream;
		}
		else
		{
			pStream->Release();
		}

        Log(FYILOGLEVEL, "New IStream %08X", pStream);
	}

    return(hr);
}


// ---------------------------------------------------
//  AllocStreamFromFile()
// ---------------------------------------------------
HRESULT AllocStreamFromFile( LPSTREAM *ppStream, 
											LPCSTR szAbsFilePath, 
											DWORD dwOffset )
{
	return AllocFileStream( ppStream, szAbsFilePath, dwOffset, STREAM_READ );
}


// ************************************************************************
// ************************************************************************
//
//  CFileStream
//
// ************************************************************************
// ************************************************************************

// ---------------------------------------------------
//  CFileStream
// ---------------------------------------------------
CFileStream::CFileStream(void)
{
    m_RefCnt = 0;
//    m_szFileName[0] = '0';
    m_dwStartOffset = 0;
	m_fInited = false;
    //m_pCachedClone = NULL;
    //m_pOriginal = NULL;



	// Unicode stuff
	//m_wzFileName[0] = '0';
}

// ---------------------------------------------------
//  ~CFileStream
// ---------------------------------------------------
CFileStream::~CFileStream()
{

    Log(FYILOGLEVEL, "Deleting IStream %08X", this);

    // close file if it is open
    if ( m_hFile )
    {
		CloseHandle( m_hFile );
        m_hFile = NULL;
    }

    //If we're referencing another stream, then release it.
//    RELEASE(m_pOriginal);

}

// ---------------------------------------------------
//  CFileStream::AddRef()
// ---------------------------------------------------
STDMETHODIMP_(ULONG) CFileStream::AddRef() 
{ 
    return ++m_RefCnt;
} 

// ---------------------------------------------------
//  CFileStream::Release()
// ---------------------------------------------------
STDMETHODIMP_(ULONG) CFileStream::Release() 
{
    DWORD RefCnt = --m_RefCnt;
    if (RefCnt == 0)
        delete this; 
    return RefCnt; 
}

// ---------------------------------------------------
//  CFileStream::QueryInterface()
// ---------------------------------------------------
STDMETHODIMP CFileStream::QueryInterface(REFIID iid, LPVOID* ppvObj) 
{ 
//    ASSERT(ppvObj);

    *ppvObj = NULL;

    if (IsEqualIID(iid, IID_IUnknown))
        *ppvObj = (LPVOID)(LPUNKNOWN)this;

    if (IsEqualIID(iid, IID_IStream))
        *ppvObj = (LPVOID)(LPSTREAM)this;

    if (*ppvObj)
    {
        ((LPUNKNOWN)*ppvObj)->AddRef();
        return NOERROR;
    }
    return E_NOINTERFACE;
} 


// ---------------------------------------------------
//  CFileStream::Init
// ---------------------------------------------------
STDMETHODIMP CFileStream::Init( const char *szAbsFilePath, 
							   DWORD dwOffset, 
							   DWORD fdwFlags )
{
	HRESULT	hr	= E_FAIL;

	m_fdwFlags = fdwFlags;

	if ( m_fdwFlags & STREAM_READ )
	{ 
	    // open the file read only
		m_hFile = CreateFile( szAbsFilePath, 
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								fdwFlags,
								NULL );

	    // if we opened it, and we could seek to position specified
	    if ( ( INVALID_HANDLE_VALUE != m_hFile ) && 
			( 0xffffFFFF != SetFilePointer( m_hFile, (long)dwOffset, NULL, FILE_BEGIN ) ) )
	    {
	        // then set up all of our variables
	        strcpy( m_szFileName, szAbsFilePath );
	        m_dwStartOffset = dwOffset;
	       	m_dwLength = 0;
			m_fInited = TRUE;
	    }
	}
	else if ( m_fdwFlags & STREAM_WRITE )
	{
		// open the file for writing - create the file if it does not exist.
		m_hFile = CreateFile( szAbsFilePath, 
								GENERIC_WRITE,
								0,
								NULL,
								OPEN_ALWAYS,
								fdwFlags,
								NULL );
		if ( INVALID_HANDLE_VALUE != m_hFile )
		{
			m_fInited = TRUE;
		}
	}
	
	if ( m_fInited )
	{
		hr = S_OK;
	}

	return(hr);
}


// ---------------------------------------------------
//  CFileStream::Read()
// ---------------------------------------------------
STDMETHODIMP CFileStream::Read
(
    void* pv, 
    ULONG cb, 
    ULONG* pcbRead 
)
{ 
	DWORD dw;

	if( ReadFile( m_hFile, pv, cb, &dw, NULL ) &&
		dw == cb )
	{
		if( pcbRead != NULL )
		{
			*pcbRead = dw;
		}
		return S_OK;
	}
	return E_FAIL;
} 

// ---------------------------------------------------
//  CFileStream::Seek()
// ---------------------------------------------------
STDMETHODIMP CFileStream::Seek( THIS_ 
							   LARGE_INTEGER dlibMove, 
							   DWORD dwOrigin, 
							   ULARGE_INTEGER *pliNewPosition )
{
	HRESULT	hr			= E_FAIL;
	DWORD	dwOffset	= 0xffffFFFF;

	long	lOffset	= HFILE_ERROR;

    switch(dwOrigin)
    {
    case STREAM_SEEK_SET : 
		dwOffset = SetFilePointer( m_hFile, (LONG)(m_dwStartOffset + dlibMove.LowPart), NULL, FILE_BEGIN );
        break;

    case STREAM_SEEK_CUR : 
		dwOffset = SetFilePointer( m_hFile, (LONG)(dlibMove.LowPart), NULL, FILE_CURRENT );
        break;

    case STREAM_SEEK_END :
		dwOffset = SetFilePointer( m_hFile, (LONG)(dlibMove.LowPart), NULL, FILE_END );
        break;
    
    default:
    	break;
//        ASSERTSZ(FALSE,"Invalid Flag for Stream::Seek");
    }

	if ( 0xffffFFFF == dwOffset )
	{
		hr = E_FAIL;
	}
	else
	{ 
		hr = S_OK;

		if ( pliNewPosition )
		{
			pliNewPosition->LowPart = SetFilePointer( m_hFile, 0, NULL, FILE_CURRENT );
			pliNewPosition->HighPart = 0;
		}
	}

    return hr;
} 

// ---------------------------------------------------
//  CFileStream::Write()
// ---------------------------------------------------

STDMETHODIMP CFileStream::Write( THIS_ 
								VOID const HUGEP *pv,
								ULONG cb, 
								ULONG * pcbWritten )
{   
	return ( ( WriteFile( m_hFile, pv, cb, pcbWritten, NULL ) == 0 ) 
		? E_FAIL : S_OK );
}


STDMETHODIMP CFileStream::Clone(THIS_ IStream ** ppFileStream)
{   
    HRESULT hr;
    *ppFileStream = NULL;
    hr = AllocStreamFromFile(ppFileStream, m_szFileName, 0);
    
    //Set the clone's original pointer to us, and addref us.
    //((CFileStream *)pStream)->m_pOriginal = this;
    //AddRef();
    //m_pCachedClone = pStream;

    Log(FYILOGLEVEL, "Cloned %08X -> %08X", this, *ppFileStream);

    return hr;
}

// NOT IMPLEMENTED CFileStream IStream methods


STDMETHODIMP CFileStream::SetSize(THIS_ ULARGE_INTEGER libNewSize)
{   return E_NOTIMPL; }

STDMETHODIMP CFileStream::CopyTo(THIS_ IStream * pStm, ULARGE_INTEGER cb,   ULARGE_INTEGER * pcbRead, ULARGE_INTEGER * pcbWritten)
{   return E_NOTIMPL; }

STDMETHODIMP CFileStream::Commit(THIS_ DWORD grfCommitFlags)
{   return E_NOTIMPL; }

STDMETHODIMP CFileStream::Revert(THIS)
{   return E_NOTIMPL; }

STDMETHODIMP CFileStream::LockRegion(THIS_ ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{   return E_NOTIMPL; }

STDMETHODIMP CFileStream::UnlockRegion(THIS_ ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,   DWORD dwLockType)
{   return E_NOTIMPL; }

STDMETHODIMP CFileStream::Stat(THIS_ STATSTG * pstatstg, DWORD grfStatFlag)
{   return E_NOTIMPL; }




}//end namespace