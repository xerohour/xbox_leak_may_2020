/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	MusicHeapFixed.cpp

Abstract:

	User-defined DirectMusic heap object 

Author:

	Dan Haffner(danhaff) 18-Jul-2001

Environment:

	Xbox only

Revision History:

	05-Jul-2001 danhaff
		Initial Version; copied out of dmusic/debug.cpp and modified.

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <xtl.h>
#include "macros.h"
#include "MusicHeap.h"
#include "helpers.h"
#include "assert.h"

using namespace WFVO;


namespace WFVO
{


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CMusicHeapFixed : public IMusicHeap
{

   //LOCALALLOC_NEWDELETE What the heck is this?
public:

    CMusicHeapFixed(void);
    ~CMusicHeapFixed(void);
    HRESULT Initialize(DWORD dwSize, bool bIsPhysicalHeap);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    /*  IDirectMusicHeap */
    STDMETHOD(Allocate) (DWORD dwSize, PVOID* ppData);
    STDMETHOD(GetSize) (THIS_ PVOID pData, LPDWORD pcbSize);
    STDMETHOD(Free)(void *pv);

    // IMusicHeap
    STDMETHOD(GetHeapAllocation          )(DWORD *pdwHeapAllocation);
    STDMETHOD(GetHeapAllocationPeak      )(DWORD *pdwHeapAllocationPeak);
    STDMETHOD(GetHeapAllocationBlocks    )(DWORD *pdwHeapAllocationBlocks);
    STDMETHOD(GetHeapAllocationBlocksPeak)(DWORD *pdwHeapAllocationBlocksPeak);

private:
    void* Alloc(size_t cb);

    CRITICAL_SECTION m_csHeap;
    DWORD m_dwHeapAllocation;
    DWORD m_dwHeapAllocationPeak;
    DWORD m_dwHeapAllocationBlocks;
    DWORD m_dwHeapAllocationBlocksPeak;
    DWORD m_dwHeapSize;
    bool m_bPhysicalHeap;
    LONG m_cRef;
    IDirectMusicHeap *m_pHeap;
};


CMusicHeapFixed::CMusicHeapFixed(void)
{
    InitializeCriticalSection(&m_csHeap);
    m_cRef = 1;
    m_bPhysicalHeap = false;
    m_dwHeapAllocation = 0;
    m_dwHeapAllocationPeak = 0;
    m_dwHeapAllocationBlocks = 0;
    m_dwHeapAllocationBlocksPeak = 0;
    m_dwHeapSize = 0;
    m_pHeap = NULL;
}

CMusicHeapFixed::~CMusicHeapFixed(void)
{
    DeleteCriticalSection(&m_csHeap);
}

HRESULT CMusicHeapFixed::Initialize(DWORD dwHeapSize, bool bIsPhysicalHeap)
{
    HRESULT hr = S_OK;
    m_bPhysicalHeap = bIsPhysicalHeap;
    m_dwHeapSize = dwHeapSize;

    if (m_bPhysicalHeap)
        hr = DirectMusicCreateFixedSizePhysicalHeap(dwHeapSize, &m_pHeap);        
    else
        hr = DirectMusicCreateFixedSizeHeap(dwHeapSize, &m_pHeap);        
        
    return hr;
}


ULONG STDMETHODCALLTYPE CMusicHeapFixed::AddRef( void)         
{
return InterlockedIncrement(&m_cRef);
}

ULONG STDMETHODCALLTYPE CMusicHeapFixed::Release(void) 
{
    ULONG cRef;            
    cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
        delete this;
    return cRef;
}

STDMETHODIMP CMusicHeapFixed::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
return E_NOTIMPL;
}



STDMETHODIMP CMusicHeapFixed::Allocate(DWORD dwSize, PVOID* ppData)
{
    PVOID pData = Alloc(dwSize);
    if(!pData)
    {
        return E_OUTOFMEMORY;
    }
    *ppData = pData;
    return S_OK;
}


void* CMusicHeapFixed::Alloc(size_t cb)
{
    DWORD* pBlock = 0;
    HRESULT hr = S_OK;
    
    hr = m_pHeap->Allocate(cb + sizeof(DWORD), (LPVOID *)&pBlock);
    EnterCriticalSection(&m_csHeap);
    
    if(!pBlock || FAILED(hr))
    {        
        Log("\n");
        Log("%s HEAP OUT OF MEMORY!!!\n", m_bPhysicalHeap ? "PHYSICAL" : "NON-PHYSICAL");
        Log("m_dwHeapAllocation             = %u\n", m_dwHeapAllocation);
        Log("m_dwHeapSize                   = %u\n", m_dwHeapSize);
        Log("m_dwHeapAllocationBlocks       = %u\n", m_dwHeapAllocationBlocks);
        Log("m_dwHeapAllocationBlocksPeak   = %u\n", m_dwHeapAllocationBlocksPeak);
        Log("\n");
        assert(FALSE);
        LeaveCriticalSection(&m_csHeap);
        return NULL;
    }

    m_dwHeapAllocation += (cb + sizeof(DWORD));
    if(m_dwHeapAllocation > m_dwHeapAllocationPeak){
        m_dwHeapAllocationPeak = m_dwHeapAllocation;
    }
    ++m_dwHeapAllocationBlocks;
    if(m_dwHeapAllocationBlocks > m_dwHeapAllocationBlocksPeak){
        m_dwHeapAllocationBlocksPeak = m_dwHeapAllocationBlocks;
    }

    LeaveCriticalSection(&m_csHeap);

    *pBlock = cb;
    return pBlock + 1;
}

STDMETHODIMP CMusicHeapFixed::GetSize(PVOID pData, LPDWORD pcbSize)
{
    assert(pData);
    DWORD* pBlock = ((DWORD*) pData) - 1;
    *pcbSize = *pBlock;

    return S_OK;
}

STDMETHODIMP CMusicHeapFixed::Free(void *pv)
{
    assert(pv);
    DWORD* pBlock = ((DWORD*) pv) - 1;
    DWORD cb = *pBlock;
    EnterCriticalSection(&m_csHeap);
    assert(cb+sizeof(DWORD) < 64 * 1024 * 1024);
    
    //Update our stats
    m_dwHeapAllocation -= (cb + sizeof(DWORD));
    --m_dwHeapAllocationBlocks;

    //Set all block memory to a certain value.
    memset((void *)pBlock, 0xFE, cb+sizeof(DWORD));

    LeaveCriticalSection(&m_csHeap);
    m_pHeap->Free(pBlock);
    return S_OK;
}


STDMETHODIMP CMusicHeapFixed::GetHeapAllocation          (DWORD *pdwHeapAllocation)
{
    *pdwHeapAllocation = m_dwHeapAllocation;
    return S_OK;
};

STDMETHODIMP CMusicHeapFixed::GetHeapAllocationPeak      (DWORD *pdwHeapAllocationPeak)
{
    *pdwHeapAllocationPeak = m_dwHeapAllocationPeak;
    return S_OK;
};

STDMETHODIMP CMusicHeapFixed::GetHeapAllocationBlocks    (DWORD *pdwHeapAllocationBlocks)
{
    *pdwHeapAllocationBlocks = m_dwHeapAllocationBlocks;
    return S_OK;
};

STDMETHODIMP CMusicHeapFixed::GetHeapAllocationBlocksPeak(DWORD *pdwHeapAllocationBlocksPeak)
{
    *pdwHeapAllocationBlocksPeak = m_dwHeapAllocationBlocksPeak;
    return S_OK;
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CreateMusicHeapFixed(IMusicHeap** ppHeap, bool bPhysical, DWORD dwSize)
{
    HRESULT hr = S_OK;
    CMusicHeapFixed* pHeap = NULL;

    pHeap = new CMusicHeapFixed();

    if(!pHeap)
    {
        hr = E_OUTOFMEMORY;
    }
    if(SUCCEEDED(hr))
    {
        hr = pHeap->Initialize(dwSize, bPhysical);
        if(SUCCEEDED(hr))
            *ppHeap = pHeap;
        else
            pHeap->Release();
    }
    return hr;
}




}//end "namespace WFVO"