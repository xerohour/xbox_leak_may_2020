/************************************************************************
*                                                                       *
*   Contains the implementation of CRIFFStream                          *
*                                                                       *
*   Copyright (c) 1995-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

#include "stdafx.h"
#include <ole2.h>
#include <initguid.h>
#include "RiffStrm.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// this takes care of a ton of compiler complaints
#ifndef WIN32
#define E_FAIL	(void __far *)MAKE_SCODE(SEVERITY_ERROR,   FACILITY_NULL, 8)
#endif


/////////////////////////////////////////////////////////////////////////////
// AllocRIFFStream

// Allocate an IDMUSProdRIFFStream from an IStream
STDAPI AllocRIFFStream( IStream* pIStream, IDMUSProdRIFFStream** ppIRiffStream )
{
	// Validate arguments
	if( (pIStream == NULL) || (ppIRiffStream == NULL) )
	{
		return E_POINTER;
	}

	// Create a new CRIFFStream that wraps the IStream
    if( ( *ppIRiffStream = (IDMUSProdRIFFStream*) new CRIFFStream( pIStream ) ) == NULL )
    {
		// Couldn't create the CRIFFStream object, probably out of memory
        return E_OUTOFMEMORY;
    }

	// Succeeded
    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// MyRead, MyWrite, MySeek
//
// These are functionally identical to mmioRead, mmioWrite, and mmioSeek,
// except for the absence of the HMMIO parameter.

/////////////////////////////////////////////////////////////////////////////
// CRIFFStream::MyRead

// Read a chunk of data from m_pStream.
long CRIFFStream::MyRead(void *pv, long cb)
{
    ULONG cbRead;
    if (FAILED(m_pStream->Read(pv, cb, &cbRead)))
        return -1;
    return cbRead;
}


/////////////////////////////////////////////////////////////////////////////
// CRIFFStream::MyWrite

// Write a chunk of data to m_pStream.
long CRIFFStream::MyWrite(const void *pv, long cb)
{
    ULONG cbWritten;
    if (FAILED(m_pStream->Write(pv, cb, &cbWritten)))
        return -1;
    return cbWritten;
}


/////////////////////////////////////////////////////////////////////////////
// CRIFFStream::MySeek

// Seek to a different position in m_pStream
long CRIFFStream::MySeek(long lOffset, int iOrigin)
{
    LARGE_INTEGER   dlibSeekTo;
    ULARGE_INTEGER  dlibNewPos;

    dlibSeekTo.HighPart = 0;
    dlibSeekTo.LowPart = lOffset;
    if (FAILED(m_pStream->Seek(dlibSeekTo, iOrigin, &dlibNewPos)))
        return -1;

    return dlibNewPos.LowPart;
}


/////////////////////////////////////////////////////////////////////////////
// CRIFFStream::Descend

// Descend into a RIFF chunk and return information about the chunk in the lpck parameter.
UINT CRIFFStream::Descend(LPMMCKINFO lpck, LPMMCKINFO lpckParent, UINT wFlags)
{
    FOURCC          ckidFind;       // chunk ID to find (or NULL)
    FOURCC          fccTypeFind;    // form/list type to find (or NULL)

    // figure out what chunk id and form/list type to search for
    if (wFlags & MMIO_FINDCHUNK)
        ckidFind = lpck->ckid, fccTypeFind = NULL;
    else
    if (wFlags & MMIO_FINDRIFF)
        ckidFind = FOURCC_RIFF, fccTypeFind = lpck->fccType;
    else
    if (wFlags & MMIO_FINDLIST)
        ckidFind = FOURCC_LIST, fccTypeFind = lpck->fccType;
    else
        ckidFind = fccTypeFind = NULL;

    lpck->dwFlags = 0L;

    for(;;)
    {
        UINT        w;

        // read the chunk header
        if (MyRead(lpck, 2 * sizeof(DWORD)) !=
            2 * sizeof(DWORD))
        return MMIOERR_CHUNKNOTFOUND;

        // store the offset of the data part of the chunk
        if ((lpck->dwDataOffset = MySeek(0L, SEEK_CUR)) == -1)
            return MMIOERR_CANNOTSEEK;

        // see if the chunk is within the parent chunk (if given)
        if ((lpckParent != NULL) &&
            (lpck->dwDataOffset - 8L >=
             lpckParent->dwDataOffset + lpckParent->cksize))
            return MMIOERR_CHUNKNOTFOUND;

        // if the chunk if a 'RIFF' or 'LIST' chunk, read the
        // form type or list type
        if ((lpck->ckid == FOURCC_RIFF) || (lpck->ckid == FOURCC_LIST))
        {
            if (MyRead(&lpck->fccType,
                     sizeof(DWORD)) != sizeof(DWORD))
                return MMIOERR_CHUNKNOTFOUND;
        }
        else
            lpck->fccType = NULL;

        // if this is the chunk we're looking for, stop looking
        if ( ((ckidFind == NULL) || (ckidFind == lpck->ckid)) &&
             ((fccTypeFind == NULL) || (fccTypeFind == lpck->fccType)) )
            break;

        // ascend out of the chunk and try again
        if ((w = Ascend(lpck, 0)) != 0)
            return w;
    }

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CRIFFStream::Ascend

// Ascend out of a RIFF chunk and update its size from the size in the lpck parameter.
UINT CRIFFStream::Ascend(LPMMCKINFO lpck, UINT /*wFlags*/)
{
    if (lpck->dwFlags & MMIO_DIRTY)
    {
        // <lpck> refers to a chunk created by CreateChunk();
        // check that the chunk size that was written when
        // CreateChunk() was called is the real chunk size;
        // if not, fix it
        LONG            lOffset;        // current offset in file
        LONG            lActualSize;    // actual size of chunk data

        if ((lOffset = MySeek(0L, SEEK_CUR)) == -1)
            return MMIOERR_CANNOTSEEK;
        if ((lActualSize = lOffset - lpck->dwDataOffset) < 0)
            return MMIOERR_CANNOTWRITE;

        if (LOWORD(lActualSize) & 1)
        {
            // chunk size is odd -- write a null pad byte
            if (MyWrite("\0", 1) != 1)
                return MMIOERR_CANNOTWRITE;

        }

        if (lpck->cksize == (DWORD)lActualSize)
            return 0;

        // fix the chunk header
        lpck->cksize = lActualSize;
        if (MySeek(lpck->dwDataOffset - sizeof(DWORD), SEEK_SET) == -1)
            return MMIOERR_CANNOTSEEK;
        if (MyWrite(&lpck->cksize, sizeof(DWORD)) != sizeof(DWORD))
            return MMIOERR_CANNOTWRITE;
    }

    // seek to the end of the chunk, past the null pad byte
    // (which is only there if chunk size is odd)
    if (MySeek(lpck->dwDataOffset + lpck->cksize + (lpck->cksize & 1L),
            SEEK_SET) == -1)
        return MMIOERR_CANNOTSEEK;

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CRIFFStream::CreateChunk

// Create a RIFF chunk, passed in as lpck.
UINT CRIFFStream::CreateChunk(LPMMCKINFO lpck, UINT wFlags)
{
    int             iBytes;         // bytes to write
    LONG            lOffset;        // current offset in file

    // store the offset of the data part of the chunk
    if ((lOffset = MySeek(0L, SEEK_CUR)) == -1)
        return MMIOERR_CANNOTSEEK;
    lpck->dwDataOffset = lOffset + 2 * sizeof(DWORD);

    // figure out if a form/list type needs to be written
    if (wFlags & MMIO_CREATERIFF)
        lpck->ckid = FOURCC_RIFF, iBytes = 3 * sizeof(DWORD);
    else
    if (wFlags & MMIO_CREATELIST)
        lpck->ckid = FOURCC_LIST, iBytes = 3 * sizeof(DWORD);
    else
        iBytes = 2 * sizeof(DWORD);

    // write the chunk header
    if (MyWrite(lpck, (LONG) iBytes) != (LONG) iBytes)
        return MMIOERR_CANNOTWRITE;

    lpck->dwFlags = MMIO_DIRTY;

    return 0;
}


//////////////////////////////////
// Save routines
//////////////////////////////////
HRESULT SaveMBStoWCS( IStream* pIStream, const CString* pstrText )
{
	HRESULT		hr = S_OK;
	wchar_t*	wstrText = NULL;
	DWORD		dwLength;
	DWORD		dwBytesWritten;
		
	if( pstrText == NULL )
	{
		hr = E_FAIL;
		goto ON_ERR;
	}

	dwLength = pstrText->GetLength() + 1;
	wstrText = new wchar_t[dwLength];
	if( wstrText == NULL )
	{
		hr = E_FAIL;
		goto ON_ERR;
	}

    MultiByteToWideChar( CP_ACP, 0, *pstrText, -1, wstrText, (size_t)dwLength );
	dwLength *= sizeof(wchar_t);

	hr = pIStream->Write( wstrText, dwLength, &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != dwLength )
	{
		goto ON_ERR;
	}

ON_ERR:
	if( wstrText )
		delete wstrText;
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// ReadMBSfromWCS

void ReadMBSfromWCS( IStream* pIStream, DWORD dwSize, CString* pstrText )
{
	HRESULT		hr = S_OK;
	wchar_t*	wstrText = NULL;
	DWORD		dwBytesRead;
	
	pstrText->Empty();
	
	wstrText = new wchar_t[dwSize];
	if( wstrText == NULL )
	{
		hr = E_FAIL;
		goto ON_ERR;
	}

	hr = pIStream->Read( wstrText, dwSize, &dwBytesRead );
	if( FAILED( hr )
	||  dwBytesRead != dwSize )
	{
		goto ON_ERR;
	}

	*pstrText = wstrText;
	
ON_ERR:
	if( wstrText )
		delete wstrText;
}


