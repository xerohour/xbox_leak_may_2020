/****************************************************************************

    Contains the implementation of CFileStream

    Copyright (c) Microsoft Corporation  1994,  1995
    
    Stolen and Modified 3/5/96 by Mark Burton from 10/10/95 Brian McDowell

*******************************************************************************/
#include <string.h>
#include "stdafx.h"
#include "stream.h"

#define FILE_BEGIN	0
#define FILE_CURRENT	1
#define	FILE_END	2

// ---------------------------------------------------
//  AllocFileStream()
// ---------------------------------------------------
STDAPI AllocFileStream(LPSTREAM FAR *ppStream, const char *pAbsFilePath, DWORD dwOffset, DWORD dwFlags)
{
	HRESULT		hr = E_OUTOFMEMORY;
    CFileStream *pStream;
	
	pStream = new CFileStream;

	if (pStream)
	{
		pStream->AddRef();

		hr = pStream->Init(pAbsFilePath, dwOffset, dwFlags);

		if (SUCCEEDED(hr))
		{
			*ppStream = pStream;
		}
		else
		{
			pStream->Release();
		}
	}

    return(hr);
}

// ---------------------------------------------------
//  AllocStreamFromFile()
// ---------------------------------------------------
STDAPI AllocStreamFromFile(LPSTREAM FAR *ppStream, const char *pAbsFilePath, DWORD dwOffset)
{
	return AllocFileStream(ppStream,pAbsFilePath,dwOffset,STREAM_READ);
}


// ---------------------------------------------------
//  AllocStreamToFile()
// ---------------------------------------------------
STDAPI AllocStreamToFile(LPSTREAM FAR *ppStream, const char *pAbsFilePath, DWORD dwOffset)
{
	return AllocFileStream(ppStream,pAbsFilePath,dwOffset,STREAM_WRITE);
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
    m_szFileName[0] = '0';
    m_dwStartOffset = 0;
}

// ---------------------------------------------------
//  ~CFileStream
// ---------------------------------------------------
CFileStream::~CFileStream()
{
    // close file if it is open
    if (m_hFile)
    {
        _lclose(m_hFile);
        m_hFile = NULL;
    }
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
STDMETHODIMP CFileStream::Init(const char *pAbsFilePath, DWORD dwOffset, DWORD fdwFlags)
{
	HRESULT hr = E_FAIL;

	m_fdwFlags = fdwFlags;

	if (m_fdwFlags & STREAM_READ)
	{ 
	    // open the file read only
	    m_hFile = _lopen(pAbsFilePath, OF_READ|OF_SHARE_DENY_WRITE);

	    // if we opened it, and we could seek to position specified
	    if ((m_hFile != HFILE_ERROR) && (_llseek(m_hFile, (long)dwOffset, FILE_BEGIN) != HFILE_ERROR))
	    {
	        // then set up all of our variables
	        strcpy(m_szFileName, pAbsFilePath);

	        m_dwStartOffset = dwOffset;
	       	m_dwLength = 0;
			m_fInited = TRUE;
	    }
	}
	else if (m_fdwFlags & STREAM_WRITE)
	{
		// create the file if it does not exist.
		// first, see if it exists by opening for reading.
//		m_hFile = _lopen(pAbsFilePath, OF_READ);

//		if (m_hFile == HFILE_ERROR) // file doesn't exist!
//		{
			m_hFile = _lcreat(pAbsFilePath,0);
//		}
		if (m_hFile != HFILE_ERROR)
		{
			_lclose(m_hFile);

		    // open the file writing
		    m_hFile = _lopen(pAbsFilePath, OF_WRITE|OF_SHARE_DENY_READ);

			if (m_hFile != HFILE_ERROR)
			{
				m_fInited = TRUE;
			}
		}
	}
	
	if (m_fInited)
	{
		hr = S_OK;
	}

	return(hr);
}




// ---------------------------------------------------
//  CFileStream::Read()
// ---------------------------------------------------
STDMETHODIMP CFileStream::Read(THIS_ void HUGEP * pv, ULONG cb, ULONG * pcbRead)
{ 
    long cbRead;
	HRESULT hr = E_POINTER;

    if (pv)
    {
        cbRead = _hread( m_hFile, pv, cb );

        if (cbRead != HFILE_ERROR)
		{
			if (pcbRead)
			{
				*pcbRead = (ULONG)cbRead;
			}

			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
    }

    return(hr);
} 

// ---------------------------------------------------
//  CFileStream::Seek()
// ---------------------------------------------------
STDMETHODIMP CFileStream::Seek(THIS_ LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * pliNewPosition)
{
	HRESULT hr = E_FAIL;
	long	lOffset = HFILE_ERROR;

    switch (dwOrigin)
    {
    case STREAM_SEEK_SET : 
        lOffset = _llseek(m_hFile, (long)(m_dwStartOffset + dlibMove.LowPart), FILE_BEGIN);
        break;

    case STREAM_SEEK_CUR : 
        lOffset = _llseek(m_hFile, (long)dlibMove.LowPart, FILE_CURRENT);
        break;

    case STREAM_SEEK_END :
        lOffset = _llseek(m_hFile, (long)dlibMove.LowPart, FILE_END);
        break;
    
    default:
    	break;
//        ASSERTSZ(FALSE,"Invalid Flag for Stream::Seek");
    }

	if (lOffset == HFILE_ERROR)
	{
		hr = E_FAIL;
	}
	else
	{ 
		hr = S_OK;

		if (pliNewPosition)
		{
			pliNewPosition->LowPart = _llseek(m_hFile, 0, FILE_CURRENT);
			pliNewPosition->HighPart = 0;
		}
	}

    return hr;
} 

// ---------------------------------------------------
//  CFileStream::Write()
// ---------------------------------------------------

STDMETHODIMP CFileStream::Write(THIS_ VOID const HUGEP * pv, ULONG cb, ULONG * pcbWritten)
{   
	 return ((*pcbWritten = _hwrite(m_hFile, (LPCSTR) pv, cb)) == -1)
               ? E_FAIL : S_OK; 
}

// NOT IMPLEMENTED CFileStream IStream methods

STDMETHODIMP CFileStream::Clone(THIS_ IStream ** ppFileStream)
{   return E_NOTIMPL; }

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


