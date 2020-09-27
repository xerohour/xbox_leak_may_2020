// =======================================================================
// UnkDLSChunk.cpp
// =======================================================================
#include "stdafx.h"

#include "UnknownChunk.h"
#include "alist.h"


// ========================================================================
// CUnknownChunk implementation
// ========================================================================

//constructor/desctructor

CUnknownChunk::CUnknownChunk()
{
    m_dwRef = 0;
    AddRef();
}

CUnknownChunk::~CUnknownChunk()
{
	while(!IsEmpty())
	{
        CUnknownChunkItem *pItem = RemoveHead();
        pItem->Release();
	}
}

// ===============================================================
// Alist implementation
// ===============================================================

void CUnknownChunk::AddTail(CUnknownChunkItem *pINode)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pINode);

	AList::AddTail((AListItem *) pINode);
}

CUnknownChunkItem *CUnknownChunk::GetHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return (CUnknownChunkItem *) AList::GetHead();
}

CUnknownChunkItem *CUnknownChunk::RemoveHead() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	return (CUnknownChunkItem *) AList::RemoveHead();
}

void CUnknownChunk::Remove(CUnknownChunkItem *pChunk) 
{ 
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT(pChunk);
	
	AList::Remove((AListItem *) pChunk);
}

// =============================================================================
// CUnknownChunk IUnknown implementation
// =============================================================================
HRESULT CUnknownChunk::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	if( ::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CUnknownChunk::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CUnknownChunk::Release()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}




// ===================================================================================
// CUnknownChunk::Load
// ===================================================================================
HRESULT CUnknownChunk::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, BOOL bIsListType)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    
    ASSERT(pIRiffStream);
    ASSERT(pckMain);

    HRESULT			hr = S_OK;
         
    CUnknownChunkItem * pItem = new CUnknownChunkItem;
    pItem->m_bIsListItem = bIsListType;
    hr = pItem->Load(pIRiffStream, pckMain);
    if (SUCCEEDED(hr))
    {
        AddTail(pItem);
    }

    return hr;
}


HRESULT CUnknownChunk::Save(IDMUSProdRIFFStream* pIRiffStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    
    ASSERT(pIRiffStream);

    HRESULT			hr = S_OK;

    // go through the items and call save on it.
    CUnknownChunkItem * pItem = GetHead();
    for (int i = 0; pItem && i < pItem->GetCount(); i++)
    {
        pItem = (CUnknownChunkItem *)pItem->GetItem(i);
        pItem->Save(pIRiffStream);
    }

    return hr;

}


// ========================================================================
// CUnknownChunkItem implementation
// ========================================================================
CUnknownChunkItem::CUnknownChunkItem()
{
    m_pData = NULL;

    m_bIsListItem = FALSE;

    m_dwRef = 0;
    AddRef();
}

CUnknownChunkItem::~CUnknownChunkItem()
{
    if (m_pData)
        delete [] m_pData;

    ASSERT(m_dwRef == 0);
}


HRESULT CUnknownChunkItem::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	if( ::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CUnknownChunkItem::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CUnknownChunkItem::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

// ========================================================================
// Additional methods
// ========================================================================
HRESULT CUnknownChunkItem::Load( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(pIRiffStream);
    ASSERT(pckMain);

    HRESULT	hr = S_OK;
    DWORD cb = 0;

    IStream* pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );
    if(pIStream == NULL)
    {
        return E_OUTOFMEMORY;
    }
        
	// If this is a LIST chunk, descend down this unknown chunk 
	// and find out the actual size of the chunk by adding the 
	// sizes of the subchunks. This is required as we might be 
	// dealing with corrupt RIFF files....
	DWORD dwRealChunkSize = pckMain->cksize;
	if(pckMain->ckid == FOURCC_LIST)
    {
        DWORD dwStreamPosition = StreamTell(pIStream);
		dwRealChunkSize = GetChunkSize(pIRiffStream, pckMain);
        StreamSeek(pIStream, dwStreamPosition, STREAM_SEEK_SET );
    }

    // remember what the chunk has.
    memcpy((void *)&m_ck, pckMain, sizeof(MMCKINFO));
	m_ck.cksize = dwRealChunkSize;

    // Read the data into m_pData.
    m_pData = new BYTE[m_ck.cksize];

    if (m_pData)
    {
	    hr = pIStream->Read( m_pData, m_ck.cksize, &cb );
        // even if this fails the code below will release the stream.
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }
    // if fails we do nothing & return;

	pIStream->Release();
    return hr;
}

DWORD CUnknownChunkItem::GetChunkSize(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	DWORD dwTallySize = pckMain->cksize;
	DWORD dwSubChunkSize = 0;
	
	MMCKINFO ck;
	ck.ckid = 0;
	ck.fccType = 0;
	ck.cksize = 0;

	while(pIRiffStream->Descend(&ck, pckMain, 0) == 0)
	{
		if(dwSubChunkSize + ck.cksize <= dwTallySize)
		{
			// Add the sizes for the header and the size fields
			dwSubChunkSize += 2 * sizeof(DWORD);
			dwSubChunkSize += ck.cksize;
			pIRiffStream->Ascend(&ck, 0);
		}
		else
			break;

		ck.ckid = 0;
		ck.fccType = 0;
	}

	LARGE_INTEGER   dlibSeekTo;
	dlibSeekTo.HighPart = 0;
	dlibSeekTo.LowPart = ck.dwDataOffset - (dwSubChunkSize + 2 * sizeof(DWORD));
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	pIStream->Seek(dlibSeekTo, SEEK_SET, NULL);
	pIStream->Release();

	return dwSubChunkSize;
}


HRESULT CUnknownChunkItem::Save(IDMUSProdRIFFStream* pIRiffStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    
    ASSERT(pIRiffStream);

    MMCKINFO ck;
    DWORD cb = 0;
    HRESULT hr = S_OK;
    IStream* pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream);

    // write out the chunk.
    ck.ckid = m_ck.ckid;
    ck.fccType = m_ck.fccType;   

    if (m_bIsListItem == FALSE)
    {
        hr = pIRiffStream->CreateChunk(&ck, 0);
    }
    else    // it's a list item
    {
        // Just create the list chunk and return. The createchunk sets
        // up the dwoffset in the ck that the chunk is to be written at.
        hr = pIRiffStream->CreateChunk(&ck, MMIO_CREATELIST);
    }

    if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }

    // write the data
    hr = pIStream->Write((LPSTR) m_pData,m_ck.cksize, &cb);	
	pIStream->Release();
    
	if (FAILED(hr) || cb != m_ck.cksize)
    {
		pIStream->Release();
        return hr;
    }

    if (FAILED(pIRiffStream->Ascend(&ck, 0)))
    {
		pIStream->Release();
	    return (E_FAIL);
    }
	return hr;
}

HRESULT CUnknownChunkItem::Write(void * pv, DWORD * pdwCurOffset, DWORD dwIndexNextExtChk)
{
	return S_OK;
}

DWORD CUnknownChunkItem::Size()
{
	return 0;//m_ck.cksize;
}
