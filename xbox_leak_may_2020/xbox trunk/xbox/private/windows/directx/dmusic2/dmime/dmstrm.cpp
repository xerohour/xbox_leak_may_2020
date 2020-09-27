//
// dmstrm.cpp
// 
// Copyright (c) 1995-2000 Microsoft Corporation
//
#include "pchime.h"
/*
#include "debug.h"
#include "dmusicc.h"
#include "..\shared\dmstrm.h"
#include "..\shared\validate.h"*/

/////////////////////////////////////////////////////////////////////////////
// AllocDIrectMusicStream

STDAPI AllocDirectMusicStream(IStream* pIStream, IDMStream** ppIDMStream)
{
	if(pIStream == NULL || ppIDMStream == NULL)
	{
		return E_INVALIDARG;            
	}

	if((*ppIDMStream = (IDMStream*) new CDirectMusicStream()) == NULL)
    {
		return E_OUTOFMEMORY;
    }
	
	((CDirectMusicStream*)*ppIDMStream)->Init(pIStream);
    
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::CDirectMusicStream

CDirectMusicStream::CDirectMusicStream() :
m_cRef(1),
m_pStream(NULL)
{
}

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::~CDirectMusicStream
   
CDirectMusicStream::~CDirectMusicStream()
{
	if(m_pStream != NULL)
    {
		m_pStream->Release();
    }
}

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::Init
   
STDMETHODIMP CDirectMusicStream::Init(IStream* pStream)
{
    SetStream(pStream);
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IUnknown

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::QueryInterface

STDMETHODIMP CDirectMusicStream::QueryInterface(const IID &iid, void **ppv)
{
	V_INAME(CDirectMusicStream::QueryInterface);
	V_PTRPTR_WRITE(ppv);
	V_REFGUID(iid);

	if(iid == IID_IUnknown || iid == IID_IDMStream)
	{
		*ppv = static_cast<IDMStream*>(this);
    } 
	else 
	{
		*ppv = NULL;
		return E_NOINTERFACE;
    }

    reinterpret_cast<IUnknown*>(this)->AddRef();
    
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::AddRef

STDMETHODIMP_(ULONG) CDirectMusicStream::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::Release

STDMETHODIMP_(ULONG) CDirectMusicStream::Release()
{
    if(!InterlockedDecrement(&m_cRef)) 
	{
		delete this;
		return 0;
    }

    return m_cRef;
}

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::SetStream

STDMETHODIMP CDirectMusicStream::SetStream(IStream* pStream)
{
    if(m_pStream != NULL)
    {
		m_pStream->Release();
    }

	m_pStream = pStream;
    
	if(m_pStream != NULL)
    {
		m_pStream->AddRef();
    }
    
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::GetStream

STDMETHODIMP_(IStream*) CDirectMusicStream::GetStream()
{
	if(m_pStream != NULL)
    {
		m_pStream->AddRef();
    }
    
	return m_pStream;
}

//////////////////////////////////////////////////////////////////////
// IDMStream

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::Descend

STDMETHODIMP CDirectMusicStream::Descend(LPMMCKINFO lpck, LPMMCKINFO lpckParent, UINT wFlags)
{
    assert(lpck);

	FOURCC ckidFind;           // Chunk ID to find (or NULL)
    FOURCC fccTypeFind;    // Form/list type to find (or NULL)

    // Figure out what chunk id and form/list type for which to search
    if(wFlags & MMIO_FINDCHUNK)
	{
		ckidFind = lpck->ckid;
		fccTypeFind = NULL;
	}
    else if(wFlags & MMIO_FINDRIFF)
    {
		ckidFind = FOURCC_RIFF;
		fccTypeFind = lpck->fccType;
    }
	else if(wFlags & MMIO_FINDLIST)
	{
		ckidFind = FOURCC_LIST;
		fccTypeFind = lpck->fccType;
	}
    else
    {
		ckidFind = fccTypeFind = NULL;
	}

    lpck->dwFlags = 0L;

    for(;;)
    {
		HRESULT hr;
		LARGE_INTEGER li;
		ULARGE_INTEGER uli;
		ULONG cbRead;

		// Read the chunk header
		hr = m_pStream->Read(lpck, 2 * sizeof(DWORD), &cbRead);

        if (FAILED(hr) || (cbRead != 2 * sizeof(DWORD)))
		{
            Trace(3,"Warning: Reached end of file.\n");
			return DMUS_E_DESCEND_CHUNK_FAIL;
		}

		// Store the offset of the data part of the chunk
		li.QuadPart = 0;
		hr = m_pStream->Seek(li, STREAM_SEEK_CUR, &uli);

		if(FAILED(hr))
		{
            Trace(1,"Error: Unable to read file.\n");
			return DMUS_E_CANNOTSEEK;
		}
		else
		{
			lpck->dwDataOffset = uli.LowPart;
		}

		// See if the chunk is within the parent chunk (if given)
		if((lpckParent != NULL) &&
		   (lpck->dwDataOffset - 8L >=
		   lpckParent->dwDataOffset + lpckParent->cksize))
		{
            // This is not really a failure, just indicating we've reached the end of the list.
			return DMUS_E_DESCEND_CHUNK_FAIL;
		}

		// If the chunk is a 'RIFF' or 'LIST' chunk, read the
		// form type or list type
		if((lpck->ckid == FOURCC_RIFF) || (lpck->ckid == FOURCC_LIST))
		{

			hr = m_pStream->Read(&lpck->fccType, sizeof(DWORD), &cbRead);

			if(FAILED(hr) || (cbRead != sizeof(DWORD)))
			{
                Trace(1,"Error: Unable to read file.\n");
				return DMUS_E_DESCEND_CHUNK_FAIL;
			}
		}
		else
		{
			lpck->fccType = NULL;
		}

		// If this is the chunk we're looking for, stop looking
		if(((ckidFind == NULL) || (ckidFind == lpck->ckid)) &&
		   ((fccTypeFind == NULL) || (fccTypeFind == lpck->fccType)))
		{
			break;
		}

		// Ascend out of the chunk and try again
		HRESULT w = Ascend(lpck, 0);
		if(FAILED(w))
		{
			return w;
		}
	}

    return S_OK;
}


//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::Ascend

STDMETHODIMP CDirectMusicStream::Ascend(LPMMCKINFO lpck, UINT /*wFlags*/)
{
	assert(lpck);

    HRESULT hr;
	LARGE_INTEGER li;
	ULARGE_INTEGER uli;
	
	if (lpck->dwFlags & MMIO_DIRTY)
    {
		// <lpck> refers to a chunk created by CreateChunk();
		// check that the chunk size that was written when
		// CreateChunk() was called is the real chunk size;
		// if not, fix it
		LONG lOffset;           // current offset in file
		LONG lActualSize;   // actual size of chunk data

		li.QuadPart = 0;
		hr = m_pStream->Seek(li, STREAM_SEEK_CUR, &uli);

		if(FAILED(hr))
		{
            Trace(1,"Error: Unable to write file.\n");
			return DMUS_E_CANNOTSEEK;
		}
		else
		{
			lOffset = uli.LowPart;
		}
		
		if((lActualSize = lOffset - lpck->dwDataOffset) < 0)
		{
            Trace(1,"Error: Unable to write file.\n");
			return DMUS_E_CANNOTWRITE;
		}

		if(LOWORD(lActualSize) & 1)
		{
			ULONG cbWritten;

			// Chunk size is odd -- write a null pad byte
			hr = m_pStream->Write("\0", 1, &cbWritten); 
			
			if(FAILED(hr) || cbWritten != 1)
			{
                Trace(1,"Error: Unable to write file.\n");
				return DMUS_E_CANNOTWRITE;
			}
		
		}
	
		if(lpck->cksize == (DWORD)lActualSize)
		{
			return S_OK;
		}

		// Fix the chunk header
		lpck->cksize = lActualSize;

		li.QuadPart = lpck->dwDataOffset - sizeof(DWORD);
		hr = m_pStream->Seek(li, STREAM_SEEK_SET, &uli);

		if(FAILED(hr))
		{
            Trace(1,"Error: Unable to write file.\n");
			return DMUS_E_CANNOTSEEK;
		}

		ULONG cbWritten;

		hr = m_pStream->Write(&lpck->cksize, sizeof(DWORD), &cbWritten); 
		
		if(FAILED(hr) || cbWritten != sizeof(DWORD))
		{
            Trace(1,"Error: Unable to write file.\n");
			return DMUS_E_CANNOTWRITE;
		}
    }

	// Seek to the end of the chunk, past the null pad byte
	// (which is only there if chunk size is odd)
	li.QuadPart = lpck->dwDataOffset + lpck->cksize + (lpck->cksize & 1L);
	hr = m_pStream->Seek(li, STREAM_SEEK_SET, &uli);

	if(FAILED(hr))
	{
        Trace(1,"Error: Unable to write file.\n");
		return DMUS_E_CANNOTSEEK;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// CDirectMusicStream::CreateChunk

STDMETHODIMP CDirectMusicStream::CreateChunk(LPMMCKINFO lpck, UINT wFlags)
{
	assert(lpck);

    UINT iBytes;    // Bytes to write
    LONG lOffset;   // Current offset in file

	// Store the offset of the data part of the chunk
	LARGE_INTEGER li;
	ULARGE_INTEGER uli;

	li.QuadPart = 0;
	HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_CUR, &uli);

	if(FAILED(hr))
	{
        Trace(1,"Error: Unable to write file.\n");
		return DMUS_E_CANNOTSEEK;
	}
    else
	{
		lOffset = uli.LowPart;
	}
    
    lpck->dwDataOffset = lOffset + 2 * sizeof(DWORD);

    // figure out if a form/list type needs to be written
    if(wFlags & MMIO_CREATERIFF)
	{
		lpck->ckid = FOURCC_RIFF, iBytes = 3 * sizeof(DWORD);
    }
	else if(wFlags & MMIO_CREATELIST)
	{
		lpck->ckid = FOURCC_LIST, iBytes = 3 * sizeof(DWORD);
	}
    else
    {
		iBytes = 2 * sizeof(DWORD);
	}

    // Write the chunk header
	ULONG cbWritten;

	hr = m_pStream->Write(lpck, iBytes, &cbWritten); 
		
	if(FAILED(hr) || cbWritten != iBytes)
	{
        Trace(1,"Error: Unable to write file.\n");
		return DMUS_E_CANNOTWRITE;
	}

	lpck->dwFlags = MMIO_DIRTY;

    return S_OK;
}

CRiffParser::CRiffParser(IStream *pStream)

{
    assert(pStream);
    m_fDebugOn = FALSE;
    m_pStream = pStream; 
    m_pParent = NULL;
    m_pChunk = NULL;
    m_lRead = 0;
    m_fFirstPass = TRUE;
    m_fComponentFailed = FALSE;
    m_fInComponent = FALSE;
}

void CRiffParser::EnterList(RIFFIO *pChunk)

{
    assert (pChunk);
    pChunk->lRead = 0;
    pChunk->pParent = m_pChunk; // Previous chunk (could be NULL.)
    m_pParent = m_pChunk;
    m_pChunk = pChunk;
    m_fFirstPass = TRUE;
}

void CRiffParser::LeaveList()

{
    assert (m_pChunk);
    if (m_pChunk)
    {
        m_pChunk = m_pChunk->pParent;
        if (m_pChunk)
        {
            m_pParent = m_pChunk->pParent;
        }
    }
}

BOOL CRiffParser::NextChunk(HRESULT * pHr)

{
    BOOL fMore = FALSE;
    if (SUCCEEDED(*pHr))
    {
        // If this is the first time we've entered this list, there is no previous chunk.
        if (m_fFirstPass)
        {
            // Clear the flag.
            m_fFirstPass = FALSE;
        }
        else
        {
            // Clean up the previous pass.
            *pHr = LeaveChunk();
        }
        // Find out if there are more chunks to read.
        fMore = MoreChunks();
        // If so, and we don't have any failure, go ahead and read the next chunk header.
        if (fMore && SUCCEEDED(*pHr))
        {
            *pHr = EnterChunk();
        }
    }
    else
    {
#ifdef DBG
        char szName[5];
        if (m_fDebugOn)
        {
            szName[4] = 0;
            strncpy(szName,(char *)&m_pChunk->ckid,4);
            Trace(-1,"Error parsing %s, Read %ld of %ld\n",szName,m_pChunk->lRead,RIFF_ALIGN(m_pChunk->cksize));
        }
#endif
        // If we were in a component, it's okay to fail. Mark that fact by setting
        // m_fComponentFailed then properly pull out of the chunk so we can
        // continue reading.
        if (m_fInComponent) 
        {
            m_fComponentFailed = TRUE;
            // We don't need to check for first pass, because we must have gotten
            // that far. Instead, we just clean up from the failed chunk.
            // Note that this sets the hresult to S_OK, which is what we want.
            // Later, the caller needs to call ComponentFailed() to find out if
            // this error occured.
            *pHr = LeaveChunk();
        }
        else
        {
            // Clean up but leave the error code.
            LeaveChunk();
        }
    }
    return fMore && SUCCEEDED(*pHr);
}

BOOL CRiffParser::MoreChunks()

{
    assert(m_pChunk);
    if (m_pChunk)
    {
        if (m_pParent)
        {
            // Return TRUE if there's enough room for another chunk.
            return (m_pParent->lRead < (m_pParent->cksize - 8));
        }
        else
        {
            // This must be a top level chunk, in which case there would only be one to read.
            return (m_pChunk->lRead == 0);
        }
    }
    // This should never happen unless CRiffParser is used incorrectly, in which 
    // case the assert will help debug. But, in the interest of making Prefix happy...
    return false;
}

HRESULT CRiffParser::EnterChunk()

{
    assert(m_pChunk);
    if (m_pChunk)
    {
        // Read the chunk header
	    HRESULT hr = m_pStream->Read(m_pChunk, 2 * sizeof(DWORD), NULL);
        if (SUCCEEDED(hr))
        {
#ifdef DBG
            char szName[5];
            if (m_fDebugOn)
            {
                szName[4] = 0;
                strncpy(szName,(char *)&m_pChunk->ckid,4);
                ULARGE_INTEGER ul;
                LARGE_INTEGER li;
                li.QuadPart = 0;
                HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_CUR, &ul);

                Trace(-1,"Entering %s, Length %ld, File position is %ld",szName,m_pChunk->cksize,(long)ul.QuadPart);
            }
#endif
            // Clear bytes read field.
            m_pChunk->lRead = 0;
            // Check to see if this is a container (LIST or RIFF.)
            if((m_pChunk->ckid == FOURCC_RIFF) || (m_pChunk->ckid == FOURCC_LIST))
		    {
			    hr = m_pStream->Read(&m_pChunk->fccType, sizeof(DWORD), NULL);
                if (SUCCEEDED(hr))
                {
                    m_pChunk->lRead += sizeof(DWORD);
#ifdef DBG
                    if (m_fDebugOn)
                    {
                        strncpy(szName,(char *)&m_pChunk->fccType,4);
                        Trace(-1," Type %s",szName);
                    }
#endif
                }
		    }
#ifdef DBG
            if (m_fDebugOn) Trace(-1,"\n");
#endif
        }
        return hr;
    }
    // This should never happen unless CRiffParser is used incorrectly, in which 
    // case the assert will help debug. But, in the interest of making Prefix happy...
    return E_FAIL;
}

HRESULT CRiffParser::LeaveChunk()

{
    HRESULT hr = S_OK;
    assert(m_pChunk);
    if (m_pChunk)
    {
        m_fInComponent = false;
        // Get the rounded up size of the chunk.
        long lSize = RIFF_ALIGN(m_pChunk->cksize);
        // Increment the parent's count of bytes read so far.
        if (m_pParent)
        {
            m_pParent->lRead += lSize + (2 * sizeof(DWORD));
            if (m_pParent->lRead > RIFF_ALIGN(m_pParent->cksize))
            {
                hr = DMUS_E_DESCEND_CHUNK_FAIL; // Goofy error name, but need to be consistent with previous versions.
            }
        }
#ifdef DBG
        char szName[5];
        if (m_fDebugOn)
        {
            szName[4] = 0;
            strncpy(szName,(char *)&m_pChunk->ckid,4);
            ULARGE_INTEGER ul;
            LARGE_INTEGER li;
            li.QuadPart = 0;
            HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_CUR, &ul);

            Trace(-1,"Leaving %s, Read %ld of %ld, File Position is %ld\n",szName,m_pChunk->lRead,lSize,(long)ul.QuadPart);
        }
#endif
        // If we haven't actually read this entire chunk, seek to the end of it.
        if (m_pChunk->lRead < lSize)
        {
            LARGE_INTEGER li;
            li.QuadPart = lSize - m_pChunk->lRead;
            hr = m_pStream->Seek(li,STREAM_SEEK_CUR,NULL);
            // There's a chance it could fail because we are at the end of file with an odd length chunk.
            if (FAILED(hr))
            {
                // If there's a parent, see if this is the last chunk.
                if (m_pParent)
                {
                    if (m_pParent->cksize >= (m_pParent->lRead - 1))
                    {
                        hr = S_OK;
                    }
                }
                // Else, see if we are an odd length.
                else if (m_pChunk->cksize & 1)
                {
                    hr = S_OK;
                }
            }
        }
        return hr;
    }
    // This should never happen unless CRiffParser is used incorrectly, in which 
    // case the assert will help debug. But, in the interest of making Prefix happy...
    return E_FAIL;
}

HRESULT CRiffParser::Read(void *pv,ULONG cb)

{
    assert(m_pChunk);
    if (m_pChunk)
    {
        // Make sure we don't read beyond the end of the chunk.
        if (((long)cb + m_pChunk->lRead) > m_pChunk->cksize)
        {
            cb -= (cb - (m_pChunk->cksize - m_pChunk->lRead));
        }
        HRESULT hr = m_pStream->Read(pv,cb,NULL);
        if (SUCCEEDED(hr))
        {
            m_pChunk->lRead += cb;
        }
        return hr;
    }
    // This should never happen unless CRiffParser is used incorrectly, in which 
    // case the assert will help debug. But, in the interest of making Prefix happy...
    return E_FAIL;
}

HRESULT CRiffParser::Skip(ULONG ulBytes)

{
    assert(m_pChunk);
    if (m_pChunk)
    {
        // Make sure we don't scan beyond the end of the chunk.
        if (((long)ulBytes + m_pChunk->lRead) > m_pChunk->cksize)
        {
            ulBytes -= (ulBytes - (m_pChunk->cksize - m_pChunk->lRead));
        }
        LARGE_INTEGER li;
        li.HighPart = 0;
		li.LowPart = ulBytes;
        HRESULT hr = m_pStream->Seek( li, STREAM_SEEK_CUR, NULL );
        if (SUCCEEDED(hr))
        {
            m_pChunk->lRead += ulBytes;
        }
        return hr;
    }
    // This should never happen unless CRiffParser is used incorrectly, in which 
    // case the assert will help debug. But, in the interest of making Prefix happy...
    return E_FAIL;
}


void CRiffParser::MarkPosition()

{
    assert(m_pChunk);
    if (m_pChunk)
    {
        LARGE_INTEGER li;
        ULARGE_INTEGER ul;
        li.HighPart = 0;
        li.LowPart = 0;
        m_pStream->Seek(li, STREAM_SEEK_CUR, &ul);
        m_pChunk->liPosition.QuadPart = (LONGLONG) ul.QuadPart;
    }
}

HRESULT CRiffParser::SeekBack()

{
    assert(m_pChunk);
    if (m_pChunk)
    {
        // Move back to the start of the current chunk. Also, store the
        // absolute position because that will be useful later when we need to seek to the
        // end of this chunk.
        ULARGE_INTEGER ul;
        LARGE_INTEGER li;
        li.QuadPart = 0;
        li.QuadPart -= (m_pChunk->lRead + (2 * sizeof(DWORD))); 
        HRESULT hr = m_pStream->Seek(li, STREAM_SEEK_CUR, &ul);
        // Now, save the absolute position for the end of this chunk.
        m_pChunk->liPosition.QuadPart = ul.QuadPart + 
            RIFF_ALIGN(m_pChunk->cksize) + (2 * sizeof(DWORD));
        m_pChunk->lRead = 0;
        return hr;
    }
    return E_FAIL;
}

HRESULT CRiffParser::SeekForward()

{
    assert(m_pChunk);
    if (m_pChunk)
    {
        m_pChunk->lRead = RIFF_ALIGN(m_pChunk->cksize);
        return m_pStream->Seek(m_pChunk->liPosition, STREAM_SEEK_SET, NULL);
    }
    return E_FAIL;
}


HRESULT CInfo::GetDescriptor(LPDMUS_OBJECTDESC pDesc,REFGUID rguidClassID)
{
    memset( pDesc, 0, sizeof(DMUS_OBJECTDESC));
    pDesc->dwSize = sizeof(DMUS_OBJECTDESC);
    pDesc->guidClass = rguidClassID;
    pDesc->guidObject = m_guidObject;
    pDesc->ftDate = m_ftDate;
    pDesc->vVersion = m_vVersion;
    memcpy( pDesc->wszName, m_wszName, sizeof(m_wszName) );
    memcpy( pDesc->wszCategory, m_wszCategory, sizeof(m_wszCategory) );
    memcpy( pDesc->wszFileName, m_wszFileName, sizeof(m_wszFileName) );
    pDesc->dwValidData = ( m_dwValidData | DMUS_OBJ_CLASS );
    return S_OK;
}

HRESULT CInfo::SetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
    HRESULT hr = E_INVALIDARG;
    DWORD dw = 0;
    if( pDesc->dwSize >= sizeof(DMUS_OBJECTDESC) )
    {
        if(pDesc->dwValidData & DMUS_OBJ_CLASS)
        {
            dw |= DMUS_OBJ_CLASS;
        }
        if(pDesc->dwValidData & DMUS_OBJ_LOADED)
        {
            dw |= DMUS_OBJ_LOADED;
        }
        if( pDesc->dwValidData & DMUS_OBJ_OBJECT )
        {
            m_guidObject = pDesc->guidObject;
            dw |= DMUS_OBJ_OBJECT;
        }
        if( pDesc->dwValidData & DMUS_OBJ_NAME )
        {
            memcpy( m_wszName, pDesc->wszName, sizeof(WCHAR)*DMUS_MAX_NAME );
            dw |= DMUS_OBJ_NAME;
        }
        if( pDesc->dwValidData & DMUS_OBJ_CATEGORY )
        {
            memcpy( m_wszCategory, pDesc->wszCategory, sizeof(WCHAR)*DMUS_MAX_CATEGORY );
            dw |= DMUS_OBJ_CATEGORY;
        }
        if( ( pDesc->dwValidData & DMUS_OBJ_FILENAME ) ||
            ( pDesc->dwValidData & DMUS_OBJ_FULLPATH ) )
        {
            memcpy( m_wszFileName, pDesc->wszFileName, sizeof(WCHAR)*DMUS_MAX_FILENAME );
            dw |= (pDesc->dwValidData & (DMUS_OBJ_FILENAME | DMUS_OBJ_FULLPATH));
        }
        if( pDesc->dwValidData & DMUS_OBJ_VERSION )
        {
            m_vVersion = pDesc->vVersion;
            dw |= DMUS_OBJ_VERSION;
        }
        if( pDesc->dwValidData & DMUS_OBJ_DATE )
        {
            m_ftDate = pDesc->ftDate;
            dw |= DMUS_OBJ_DATE;
        }
        m_dwValidData |= dw;
        if( pDesc->dwValidData & (~dw) )
        {
            Trace(2,"Warning: SetDescriptor was not able to handle all passed fields, dwValidData bits %lx.\n",pDesc->dwValidData & (~dw));
            hr = S_FALSE; // there were extra fields we didn't parse;
            pDesc->dwValidData = dw;
        }
        else
        {
            hr = S_OK;
        }
    }
    else
    {
        Trace(1,"Error: Size of descriptor too large to parse.\n");
    }
    return hr;
}


HRESULT CInfo::ParseDescriptor(LPSTREAM pIStream, LPDMUS_OBJECTDESC pDesc, 
                                     DWORD dwRIFFID, REFGUID guidClassID) 
{
    CRiffParser Parser(pIStream);
    RIFFIO ckMain;
    RIFFIO ckNext;
    RIFFIO ckUNFO;
    HRESULT hr = S_OK;
    DWORD dwValidData;

    Parser.EnterList(&ckMain);  
    if (Parser.NextChunk(&hr) && (ckMain.fccType == dwRIFFID))
    {
        dwValidData = DMUS_OBJ_CLASS;
        pDesc->guidClass = guidClassID;
        Parser.EnterList(&ckNext);
        while(Parser.NextChunk(&hr))
        {
            switch(ckNext.ckid)
            {
            case FOURCC_DLID:
            case DMUS_FOURCC_GUID_CHUNK:
                hr = Parser.Read( &pDesc->guidObject, sizeof(GUID) );
                dwValidData |= DMUS_OBJ_OBJECT;
                break;
            case FOURCC_VERS:
                hr = Parser.Read( &pDesc->vVersion, sizeof(DMUS_VERSION) );
                dwValidData |= DMUS_OBJ_VERSION;
                break;
            case DMUS_FOURCC_CATEGORY_CHUNK:
                hr = Parser.Read( &pDesc->wszCategory, sizeof(pDesc->wszCategory) );
                dwValidData |= DMUS_OBJ_CATEGORY;
                break;
            case DMUS_FOURCC_DATE_CHUNK:
                hr = Parser.Read( &pDesc->ftDate, sizeof(FILETIME) );
                dwValidData |= DMUS_OBJ_DATE;
                break;
            case FOURCC_LIST:
                switch(ckNext.fccType)
                {
                case DMUS_FOURCC_UNFO_LIST:
                    Parser.EnterList(&ckUNFO);
                    while (Parser.NextChunk(&hr))
                    {
                        if (ckUNFO.ckid == DMUS_FOURCC_UNAM_CHUNK)
                        {
                            hr = Parser.Read(&pDesc->wszName, sizeof(pDesc->wszName));
                            dwValidData |= DMUS_OBJ_NAME;
                        }
                    }
                    Parser.LeaveList();
                    break;            
                    // Waves and DLS files have ascii info chunks...
                case DMUS_FOURCC_INFO_LIST:
                    Parser.EnterList(&ckUNFO);
                    while (Parser.NextChunk(&hr))
                    {
                        if ( ckUNFO.ckid == mmioFOURCC('I','N','A','M'))
                        {
			                char szName[DMUS_MAX_NAME];
			                hr = Parser.Read(szName, DMUS_MAX_NAME);
			                if(SUCCEEDED(hr))
			                {
				                MultiByteToWideChar(CP_ACP, 0, szName, -1, pDesc->wszName, sizeof(pDesc->wszName));
				                dwValidData |= DMUS_OBJ_NAME;
			                }
                            break;
                        }
                    }
                    Parser.LeaveList();
                    break;
                }
                break;
            }
        }
        Parser.LeaveList();
    }
    else
    {
        Trace(1,"Error: Parsing file - invalid file format\n");
        hr = DMUS_E_CHUNKNOTFOUND;
    }

    if (SUCCEEDED(hr))
    {
        pDesc->dwValidData = dwValidData;
    }
    return hr;
}

HRESULT CInfo::ReadChunk(CRiffParser *pParser,DWORD dwID)

{
    HRESULT hr = S_OK;
    RIFFIO ckChild;
    switch (dwID)
    {
    case FOURCC_DLID:
    case DMUS_FOURCC_GUID_CHUNK:
        hr = pParser->Read( &m_guidObject, sizeof(GUID) );
        m_dwValidData |= DMUS_OBJ_OBJECT;
        break;
    case DMUS_FOURCC_VERSION_CHUNK:
        hr = pParser->Read( &m_vVersion, sizeof(DMUS_VERSION) );
        m_dwValidData |= DMUS_OBJ_VERSION;
        break;
    case DMUS_FOURCC_CATEGORY_CHUNK:
        hr = pParser->Read( &m_wszCategory, sizeof(WCHAR)*DMUS_MAX_CATEGORY );
        m_dwValidData |= DMUS_OBJ_CATEGORY;
        break;
    case DMUS_FOURCC_DATE_CHUNK:
        hr = pParser->Read( &m_ftDate, sizeof(FILETIME) );
        m_dwValidData |= DMUS_OBJ_DATE;
        break;
    case DMUS_FOURCC_UNFO_LIST:
        pParser->EnterList(&ckChild);
        while (pParser->NextChunk(&hr))
        {
            if ( ckChild.ckid == DMUS_FOURCC_UNAM_CHUNK)
            {
                hr = pParser->Read(&m_wszName, sizeof(m_wszName));
                m_dwValidData |= DMUS_OBJ_NAME;
            }
        }
        pParser->LeaveList();
        break;
        // Waves and DLS files have ascii info chunks...
    case DMUS_FOURCC_INFO_LIST:
        pParser->EnterList(&ckChild);
        while (pParser->NextChunk(&hr))
        {
            if ( ckChild.ckid == mmioFOURCC('I','N','A','M'))
            {
			    char szName[DMUS_MAX_NAME];
			    hr = pParser->Read(szName, DMUS_MAX_NAME);
			    if(SUCCEEDED(hr))
			    {
				    MultiByteToWideChar(CP_ACP, 0, szName, -1, m_wszName, DMUS_MAX_NAME);
				    m_dwValidData |= DMUS_OBJ_NAME;
			    }
                break;
            }
        }
        pParser->LeaveList();
        break;
    }
    return hr;
}


void CInfo::Init()

{
    memset(this,0,sizeof(CInfo));
    m_dwValidData = DMUS_OBJ_CLASS; // upon creation, only this data is valid
}


HRESULT CLinkParser::ReadReference(CRiffParser *pParser, LPDMUS_OBJECTDESC pDesc)

{
    HRESULT hr = S_OK;
    pDesc->dwValidData = 0;
    RIFFIO ckNext;
    pParser->EnterList(&ckNext);
    while(pParser->NextChunk(&hr))
    {
        switch(ckNext.ckid)
        {
        case  DMUS_FOURCC_REF_CHUNK:
            DMUS_IO_REFERENCE ioDMRef;
            hr = pParser->Read(&ioDMRef, sizeof(DMUS_IO_REFERENCE));
            pDesc->guidClass = ioDMRef.guidClassID;
            pDesc->dwValidData |= ioDMRef.dwValidData;
            pDesc->dwValidData |= DMUS_OBJ_CLASS;
            break;
        case FOURCC_DLID:
        case DMUS_FOURCC_GUID_CHUNK:
            hr = pParser->Read(&(pDesc->guidObject), sizeof(GUID));
            pDesc->dwValidData |=  DMUS_OBJ_OBJECT;
            break;
        case DMUS_FOURCC_NAME_CHUNK:
            hr = pParser->Read(pDesc->wszName, sizeof(pDesc->wszName));
            pDesc->dwValidData |=  DMUS_OBJ_NAME;
            break;
        case DMUS_FOURCC_FILE_CHUNK:
            hr = pParser->Read(pDesc->wszFileName, sizeof(pDesc->wszFileName));
            pDesc->dwValidData |=  DMUS_OBJ_FILENAME;
            break;
        case DMUS_FOURCC_DATE_CHUNK:
            hr = pParser->Read(&(pDesc->ftDate), sizeof(FILETIME));
            pDesc->dwValidData |=  DMUS_OBJ_DATE;
            break;
        case DMUS_FOURCC_CATEGORY_CHUNK:
            hr = pParser->Read(pDesc->wszCategory, sizeof(pDesc->wszCategory));
            pDesc->dwValidData |=  DMUS_OBJ_CATEGORY;
            break;
        case DMUS_FOURCC_VERSION_CHUNK:
            DMUS_IO_VERSION ioDMObjVer;
            hr = pParser->Read(&ioDMObjVer, sizeof(DMUS_IO_VERSION));
            pDesc->vVersion.dwVersionMS = ioDMObjVer.dwVersionMS;
            pDesc->vVersion.dwVersionLS = ioDMObjVer.dwVersionLS;
            pDesc->dwValidData |= DMUS_OBJ_VERSION;
            break;
        }
    }
    pParser->LeaveList();
    return hr;
}

HRESULT CLinkParser::LoadReferencedObject(CRiffParser *pParser,void **ppObject,REFGUID rguidIID,BOOL fDontCache)

{
    IDirectMusicGetLoader *pIGetLoader;
    HRESULT hr = pParser->GetStream()->QueryInterface( IID_IDirectMusicGetLoader,(void **) &pIGetLoader );
    if (SUCCEEDED(hr))
    {
        IDirectMusicLoader* pLoader = NULL;
        hr = pIGetLoader->GetLoader(&pLoader);
        pIGetLoader->Release();
        if (SUCCEEDED(hr))
        {
            DMUS_OBJECTDESC Desc;
            hr = ReadReference(pParser,&Desc);
            if(SUCCEEDED(hr))
            {
                Desc.dwSize = sizeof(DMUS_OBJECTDESC);
                hr = pLoader->GetObject(&Desc, rguidIID, ppObject);
                // Once we get the object, we need to ensure that the same object is never
                // connected up to any other songs (or this one, too.)
                // So, we ensure that the loader doesn't keep it around.
                if (SUCCEEDED(hr) && fDontCache)
                {
#ifdef DXAPI
                    IDirectMusicObject *pObject;
                    if (SUCCEEDED(((IUnknown *) (*ppObject))->QueryInterface(IID_IDirectMusicObject,(void **)&pObject)))
                    {
                        pLoader->ReleaseObject(pObject);
                        pObject->Release();
                    }
#else
                    pLoader->ReleaseObjectByUnknown((IUnknown *) (*ppObject));
#endif
                }
            }
            pLoader->Release();
        }
    }
    return hr;
}



