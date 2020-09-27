//
// Debug.CPP
//
// Copyright (c) 1997-1998 Microsoft Corporation
//
// @doc INTERNAL
//
// @module Debug | Debug services for DMusic.DLL
//
#include "pchime.h"

/*#ifdef XBOX
#include <xtl.h>
#include <xdbg.h>
#else  // XBOX
#include <windows.h>
#endif // XBOX
#include <stdio.h>
#include <stdarg.h>
#include <dmusicip.h>
#include "debug.h"

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
extern "C" void DMRegisterPMsgPerformanceCounters();
extern "C" void DMUnregisterPMsgPerformanceCounters();
#endif*/

// Heap objects
IDirectMusicHeap* gpDMHeap;
IDirectMusicHeap* gpDMPhysicalHeap;

// General Direct Music Initialization hook
// Required because we can't allocate any memory until after the heap is registered,
// but then there are various objects that depend upon static constructors being called.

extern LPDIRECTMUSICFACTORYFN gDirectMusicFactoryFn;

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS

HRESULT WrapPerformanceCountersAroundHeap(bool bPhysicalHeap, IDirectMusicHeap* pHeap, IDirectMusicHeap** ppWrappedHeap);

#endif


void InitializeDirectMusicDoWorkCriticalSection();
extern void CScriptTrackCallListInitialize(void);

void DirectMusicInitializedCheck(){
	ASSERTMSG("You must call either DirectMusicInitialize or DirectMusicInitializeEx before using DirectMusic.",
		gDirectMusicFactoryFn != NULL);
}

HRESULT WINAPI DirectMusicInitializeEx(IDirectMusicHeap* pNormalHeap, IDirectMusicHeap* pPhysicalHeap,
                                       LPDIRECTMUSICFACTORYFN pFactory){
    ASSERTMSG("Must not be NULL.",pNormalHeap);
    ASSERTMSG("Must not be NULL.",pPhysicalHeap);
    ASSERTMSG("Must not be NULL.",pFactory);

    ASSERTMSG("DirectMusicInitialize or DirectMusicInitializeEx has already been called.",
        gDirectMusicFactoryFn == NULL);

    HRESULT hr = S_OK;

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    IDirectMusicHeap* pWrappedHeap;
    hr = WrapPerformanceCountersAroundHeap(false, pNormalHeap, &pWrappedHeap);
    if(SUCCEEDED(hr)){
        pNormalHeap = pWrappedHeap;
    }
    if(SUCCEEDED(hr)){
        hr = WrapPerformanceCountersAroundHeap(true, pPhysicalHeap, &pWrappedHeap);
    }
    if(SUCCEEDED(hr)){
        pPhysicalHeap = pWrappedHeap;
    }
#else
    pNormalHeap->AddRef();
    pPhysicalHeap->AddRef();
#endif

    if(SUCCEEDED(hr)){
        gpDMHeap = pNormalHeap;

        gpDMPhysicalHeap = pPhysicalHeap;

        gDirectMusicFactoryFn = pFactory;

        CScriptTrackCallListInitialize();
        InitializeDirectMusicDoWorkCriticalSection();
    }
    return hr;
}

HRESULT WINAPI DirectMusicInitialize(){
    HRESULT hr = S_OK;
    IDirectMusicHeap* pHeap = NULL;
    IDirectMusicHeap* pPhysicalHeap = NULL;
    if(SUCCEEDED(hr)){
        hr = DirectMusicCreateDefaultHeap(&pHeap);
    }
    if(SUCCEEDED(hr)){
        hr = DirectMusicCreateDefaultPhysicalHeap(&pPhysicalHeap);
    }
    if(SUCCEEDED(hr)){
        hr = DirectMusicInitializeEx(pHeap, pPhysicalHeap, &DirectMusicDefaultFactory);
    }
    if(pHeap){
        pHeap->Release();
    }
    if(pPhysicalHeap){
        pPhysicalHeap->Release();
    }
    return hr;
}

void* DirectMusicAllocI(size_t cb)
{
    if(!gpDMHeap) {
    	DirectMusicInitializedCheck();
    }
    void* pData = NULL;
    HRESULT hr = gpDMHeap->Allocate(cb, &pData);
    if(FAILED(hr) || pData == NULL){
        Trace(1,"DirectMusic normal heap failed to allocate a block of size %d bytes.\n", cb);
        return NULL;
    }
    ASSERTMSG("Allocated memory must be DWORD aligned.", (((int) pData) & 3) == 0);
    return pData;
}

void DirectMusicFreeI(void *pv)
{
    if(!gpDMHeap) {
    	DirectMusicInitializedCheck();
    }
    if(pv){
        gpDMHeap->Free(pv);
    }
}

void* DirectMusicPhysicalAllocI(size_t dwSize)
{
    if(!gpDMPhysicalHeap) {
    	DirectMusicInitializedCheck();
    }
    void* pData = NULL;
    HRESULT hr = gpDMPhysicalHeap->Allocate(dwSize, &pData);
    ASSERTMSG("Allocated memory must be DWORD aligned.", (((int) pData) & 3) == 0);
    if(FAILED(hr) || pData == NULL){
        Trace(1,"DirectMusic physical heap failed to allocate a block of size %d bytes.\n", dwSize);
        return NULL;
    }
    return pData;
}

void DirectMusicPhysicalFreeI(void *pv)
{
    if(!gpDMPhysicalHeap) {
        DirectMusicInitializedCheck();
    }
    if(pv){
        gpDMPhysicalHeap->Free(pv);
    }
}

// Standard heaps
// Note that we allocate these objects using LocalAlloc and free rather than the DMusic heap

#define LOCALALLOC_NEWDELETE \
    public: __inline void *__cdecl operator new(size_t cbBuffer) { return LocalAlloc(0,cbBuffer); } \
    public: __inline void *__cdecl operator new[](size_t cbBuffer) { return LocalAlloc(0,cbBuffer); } \
    public: __inline void __cdecl operator delete(void *pvBuffer) { if(pvBuffer) LocalFree(pvBuffer); } \
    public: __inline void __cdecl operator delete[](void *pvBuffer) { if(pvBuffer) LocalFree(pvBuffer); }


class DirectMusicDefaultHeap : public IDirectMusicHeap
{
    LOCALALLOC_NEWDELETE
public:
    DirectMusicDefaultHeap(){
        m_cRef = 1;
    }

    HRESULT Initialize() {
        return S_OK;
    }

    ~DirectMusicDefaultHeap() {}

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj)
    {
        return E_NOTIMPL;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void) 
        
    {
        return InterlockedIncrement(&m_cRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release( void) 
    {
        ULONG cRef;
            
        cRef = InterlockedDecrement(&m_cRef);

        if (cRef == 0)
            delete this;

        return cRef;
    }

    LONG m_cRef;

    /*  IDirectMusicHeap */
    STDMETHOD(Allocate)   (THIS_ DWORD dwSize, PVOID* ppData){
        *ppData = HeapAlloc(GetProcessHeap(),0, dwSize); // LocalAlloc is threadsafe. Can't use malloc/free because it might not be threadsafe.
        if(!*ppData){
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }

    STDMETHOD(GetSize)(THIS_ PVOID pData, LPDWORD pcbSize){
        HRESULT hr = S_OK;
        if(pData){
            DWORD dwSize = HeapSize(GetProcessHeap(), 0, pData);
            if(dwSize ==0xFFFFFFFF){
                hr = E_FAIL;
            }
            else {
                *pcbSize = dwSize;
            }
        }
        else {
            hr = E_FAIL;
        }
        return hr;
    }

    STDMETHOD(Free)   (THIS_ PVOID pData){
        HeapFree(GetProcessHeap(),0,pData); // LocalFree is threadsafe. Can't use malloc/free because it might not be threadsafe.
        return S_OK;
    }
};

class DirectMusicDefaultPhysicalHeap : public IDirectMusicHeap
{
    LOCALALLOC_NEWDELETE
public:
    DirectMusicDefaultPhysicalHeap(){
        m_cRef = 1;
    }

    HRESULT Initialize() {
        return S_OK;
    }

    ~DirectMusicDefaultPhysicalHeap() {}

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj)
    {
        return E_NOTIMPL;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void) 
        
    {
        return InterlockedIncrement(&m_cRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release( void) 
    {
        ULONG cRef;
            
        cRef = InterlockedDecrement(&m_cRef);

        if (cRef == 0)
            delete this;

        return cRef;
    }

    LONG m_cRef;

    /*  IDirectMusicHeap */
    STDMETHOD(Allocate)   (THIS_ DWORD dwSize, PVOID* ppData){
        PVOID pData = XPhysicalAlloc(dwSize+4, MAXULONG_PTR, 0, PAGE_READWRITE);
        if(!pData){
            return E_OUTOFMEMORY;
        }
        ((DWORD*) pData)[0] = dwSize;
        *ppData = ((DWORD*) pData) + 1;
        return S_OK;
    }

    STDMETHOD(GetSize) (THIS_ PVOID pData, LPDWORD pcbSize){
        HRESULT hr = S_OK;
        if(pData){
            *pcbSize = ((DWORD*) pData)[-1];
        }
        else {
            hr = E_FAIL;
        }
        return hr;
    }

    STDMETHOD(Free)   (THIS_ PVOID pData){
        XPhysicalFree(((DWORD*) pData)-1);
        return S_OK;
    }
};

class DirectMusicFixedSizeHeap2 : public IDirectMusicHeap
{
    LOCALALLOC_NEWDELETE
public:
    DirectMusicFixedSizeHeap2(){
        m_cRef = 1;
        m_hHeap = GetProcessHeap();
        InitializeCriticalSection(&m_csPhysicalHeap);
    }

    HRESULT Initialize(DWORD dwNormalHeapSize) {
        m_dwLimit = dwNormalHeapSize;
        m_dwSize = 0;
        return S_OK;
    }

    ~DirectMusicFixedSizeHeap2() {
        // We don't delete m_hHeap, since it's the default process heap
        DeleteCriticalSection(&m_csPhysicalHeap);
    }

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj)
    {
        return E_NOTIMPL;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void) 
        
    {
        return InterlockedIncrement(&m_cRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release( void) 
    {
        ULONG cRef;
            
        cRef = InterlockedDecrement(&m_cRef);

        if (cRef == 0)
            delete this;

        return cRef;
    }

    LONG m_cRef;

    /*  IDirectMusicHeap */
    STDMETHOD(Allocate)   (THIS_ DWORD dwSize, PVOID* ppData){
        HRESULT hr = S_OK;
        EnterCriticalSection(&m_csPhysicalHeap);
        if(m_dwSize + dwSize <= m_dwLimit){
            PVOID pData = HeapAlloc(m_hHeap, 0, dwSize);
            if(!pData){
			    hr = E_OUTOFMEMORY;
            }
            else {
                m_dwSize += dwSize;
                *ppData = pData;
            }
        }
        else {
            Trace(1,"Normal fixed size heap is out of memory. Limit = %d, current size = %d, request = %d.\n",
                m_dwLimit, m_dwSize, dwSize);
		    hr = E_OUTOFMEMORY;
        }
        LeaveCriticalSection(&m_csPhysicalHeap);
        return hr;
    }

    STDMETHOD(GetSize) (THIS_ PVOID pData, LPDWORD pcbSize){
        HRESULT hr = S_OK;
        if(pData){
            DWORD dwSize = HeapSize(m_hHeap, 0, pData);
            if(dwSize ==0xFFFFFFFF){
                hr = E_FAIL;
            }
            else {
                *pcbSize = dwSize;
            }
        }
        else {
            hr = E_FAIL;
        }
        return hr;
    }

    STDMETHOD(Free)   (THIS_ PVOID pData){
        HRESULT hr = S_OK;
        if(pData){
            EnterCriticalSection(&m_csPhysicalHeap);
            DWORD dwSize = HeapSize(m_hHeap, 0, pData);
            if(m_dwSize >= dwSize){
                m_dwSize -= dwSize;
            }
            HeapFree(m_hHeap, 0, pData);
            LeaveCriticalSection(&m_csPhysicalHeap);
        }
        else {
            hr = E_FAIL;
        }
        return hr;
    }


private:
    CRITICAL_SECTION m_csPhysicalHeap;
    HANDLE m_hHeap; // An alias to GetProcessHeap();
    DWORD m_dwSize;
    DWORD m_dwLimit;

};

class DirectMusicFixedSizeHeap : public IDirectMusicHeap
{
    LOCALALLOC_NEWDELETE

    // Use standard HeapCreate / HeapAlloc heap for the normal heap.
    // 

    HANDLE m_hHeap; // Used for small blocks

    struct BlockHeader {
        DWORD dwSize;
    };

    // 1044480 is the threshold for using VirtualAlloc on the Xbox implementation of the Rtl Heap.
#define LARGEST_SMALL_HEAP_BLOCK_SIZE (1044480-sizeof(BlockHeader)) /* Blocks larger than this size are allocated using VirtualAlloc */

public:
    DirectMusicFixedSizeHeap(){
        m_cRef = 1;
        m_hHeap = 0;
    }

    HRESULT Initialize(DWORD dwNormalHeapSize) {
        if(dwNormalHeapSize){
            m_hHeap = HeapCreate(0, dwNormalHeapSize, dwNormalHeapSize);
            if(!m_hHeap){
                return E_OUTOFMEMORY;
            }
        }
        return S_OK;
    }

    ~DirectMusicFixedSizeHeap() {
        if(m_hHeap) {
            HeapDestroy(m_hHeap);
        }
    }

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj)
    {
        return E_NOTIMPL;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void) 
        
    {
        return InterlockedIncrement(&m_cRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release( void) 
    {
        ULONG cRef;
            
        cRef = InterlockedDecrement(&m_cRef);

        if (cRef == 0)
            delete this;

        return cRef;
    }

    LONG m_cRef;

    /*  IDirectMusicHeap */
    STDMETHOD(Allocate)   (THIS_ DWORD dwSize, PVOID* ppData){
        HRESULT hr = S_OK;
        if(m_hHeap){
            DWORD dwSize2 = dwSize + sizeof(BlockHeader);
            PVOID pData = 0;
            if (dwSize <= LARGEST_SMALL_HEAP_BLOCK_SIZE){
                pData = HeapAlloc(m_hHeap, 0, dwSize2);
            }
            else {
                Trace(2,"DirectMusicFixedSizeHeap allocating large block (%d bytes) using VirtualAlloc\n", dwSize);
                pData = VirtualAlloc(NULL, dwSize2, MEM_COMMIT, PAGE_READWRITE);
            }
            if(!pData){
			    hr = E_OUTOFMEMORY;
            }
            else {
                BlockHeader* pHeader = (BlockHeader*) pData;
                pHeader->dwSize = dwSize;
                *ppData = pHeader + 1;
            }
        }
        else {
		    hr = E_OUTOFMEMORY;
        }
        return hr;
    }

    STDMETHOD(GetSize) (THIS_ PVOID pData, LPDWORD pcbSize){
        HRESULT hr = S_OK;
        if(pData){
            BlockHeader* pHeader = ((BlockHeader*) pData) - 1;
            *pcbSize = pHeader->dwSize;
        }
        else {
            hr = E_FAIL;
        }
        return hr;
    }

    STDMETHOD(Free)   (THIS_ PVOID pData){
        HRESULT hr = S_OK;
        if(m_hHeap){
            if(pData){
                BlockHeader* pHeader = ((BlockHeader*) pData) - 1;
                if(pHeader->dwSize <= LARGEST_SMALL_HEAP_BLOCK_SIZE){ 
                    HeapFree(m_hHeap, 0, pHeader);
                }
                else {
                    VirtualFree(pHeader, 0, MEM_RELEASE);
                }
            }
        }
        else {
            hr = E_FAIL;
        }
        return hr;
    }


private:

};

// VerifyHeap is slow, so we only do it on debug builds.

#ifdef DBG
    #define VERIFYHEAP// VerifyHeap();
#else
    #define VERIFYHEAP
#endif

class DirectMusicFixedSizePhysicalHeap : public IDirectMusicHeap
{
    LOCALALLOC_NEWDELETE
public:
    DirectMusicFixedSizePhysicalHeap(){
        m_cRef = 1;
        m_pHead = 0;
        InitializeCriticalSection(&m_csPhysicalHeap);
    }

private:
    // Use a traditional first-fit heap for the physical memory heap

    struct BlockHead {
        BlockHead* pNext;
        unsigned int size;
        DWORD bUsed; // A DWORD to make the whole BlockHead struct DWORD aligned
    };

    CRITICAL_SECTION m_csPhysicalHeap;
    BlockHead* m_pHead;

public:

    HRESULT Initialize(DWORD dwPhysicalHeapSize) {

        if(dwPhysicalHeapSize < sizeof(BlockHead)){
            dwPhysicalHeapSize = 0;
        }
        else {
            m_pHead = (BlockHead*) XPhysicalAlloc(dwPhysicalHeapSize, MAXULONG_PTR, 0, PAGE_READWRITE);
            if(!m_pHead){
                return E_OUTOFMEMORY;
            }
            m_pHead->pNext = 0;
            m_pHead->size = dwPhysicalHeapSize-sizeof(BlockHead);
            m_pHead->bUsed = 0;
        }

        return S_OK;
    }

    ~DirectMusicFixedSizePhysicalHeap() {
        if(m_pHead){
            XPhysicalFree(m_pHead);
        }
        DeleteCriticalSection(&m_csPhysicalHeap);
    }

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj)
    {
        return E_NOTIMPL;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void) 
        
    {
        return InterlockedIncrement(&m_cRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release( void) 
    {
        ULONG cRef;
            
        cRef = InterlockedDecrement(&m_cRef);

        if (cRef == 0)
            delete this;

        return cRef;
    }

    LONG m_cRef;

    /*  IDirectMusicHeap methods */
    STDMETHOD(Allocate)   (THIS_ DWORD dwSize, PVOID* ppData){
        if(!m_pHead){
            return E_OUTOFMEMORY;
        }
		VERIFYHEAP
        PVOID pData = NULL;
        //Round up to the nearest DWORD.
        dwSize = (dwSize + 3) & ~3;

        EnterCriticalSection(&m_csPhysicalHeap);
        for(BlockHead* pB = m_pHead; pB; pB = pB->pNext){
            if((! pB->bUsed)){
                // Coalesce with next block, if possible
                while(pB->pNext && !pB->pNext->bUsed){
                    pB->size += pB->pNext->size + sizeof(BlockHead);
                    pB->pNext = pB->pNext->pNext;
                }
                // Is this block big enough to use?
                if(dwSize <= pB->size){
                    // Is it big enough to split?
                    if(dwSize + sizeof(BlockHead) < pB->size){
                        // split
                        BlockHead* pNew = (BlockHead*) (((char*) pB) + sizeof(BlockHead) + dwSize);
                        pNew->pNext = pB->pNext;
                        pNew->size = pB->size - (dwSize + sizeof(BlockHead));
                        pNew->bUsed = 0;
						pB->size = dwSize;
                        pB->pNext = pNew;
                    }
                    pB->bUsed = 'z';
                    pData = (void*) (((char*) pB) + sizeof(BlockHead));
                    break;
                }
            }
        }
        LeaveCriticalSection(&m_csPhysicalHeap);
		VERIFYHEAP
        if(!pData){
            return E_OUTOFMEMORY;
        }
        *ppData = pData;
		VERIFYHEAP
        return S_OK;
    }

    STDMETHOD(GetSize) (THIS_ PVOID pData, LPDWORD pcbSize){
        HRESULT hr = S_OK;
        if(pData){
            BlockHead* pB = (BlockHead*)(((char*) pData) - sizeof(BlockHead));
            *pcbSize = pB->size;
        }
        else {
            hr = E_FAIL;
        }
        return hr;
    }

    STDMETHOD(Free)   (THIS_ PVOID pData){
        if(!m_pHead){
            return E_FAIL;
        }
        if(pData){
			VERIFYHEAP
            EnterCriticalSection(&m_csPhysicalHeap);
            BlockHead* pB = (BlockHead*)(((char*) pData) - sizeof(BlockHead));
            ASSERTMSG("Bad Block", pB->bUsed == 'z');
            pB->bUsed = 0;
            LeaveCriticalSection(&m_csPhysicalHeap);
			VERIFYHEAP
        }
        return S_OK;
    }

#ifdef VERIFYHEAP

	STDMETHOD(VerifyHeap)(){
		DWORD dwCount = 0;
		BlockHead *pB = NULL;
		char *pByte;

		EnterCriticalSection(&m_csPhysicalHeap);

		for(pB = m_pHead; pB; pB = pB->pNext){
			dwCount++;
			pByte = (char *)pB;
        
			//Verify that the first and last byte in this block are 'X'.
			if(pB->bUsed == 0){
				if((pByte + sizeof(BlockHead))[0] != 'X'){
					Trace(1,"Heap Corruption at beginning of block");
					assert(FALSE);
				}

				if((pByte + sizeof(BlockHead))[pB->size - 1] != 'X'){
					Trace(1,"Heap Corruption at end of block");
					assert(FALSE);
				}
			}

			//Verify that the sizes match.
			if (pB->pNext) {
				if((char *)pB->pNext - (char *)pB != pB->size + sizeof(BlockHead)){
					Trace(1,"Difference between pB (%08X) and pB->pNext (%08X) is %08X (%d), but the size of pB + sizeof(BlockHead) is %08X (%d)\n",
						 pB,
						 pB->pNext,
						 (char *)pB->pNext - (char *)pB,
						 (char *)pB->pNext - (char *)pB,
						 pB->size,
						 pB->size
						 );
					assert(FALSE);
				}
			}
		}

		LeaveCriticalSection(&m_csPhysicalHeap);
		return S_OK;
	}
#endif
};

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS

class PerformanceCounterWrapper : public IDirectMusicHeap {
    LOCALALLOC_NEWDELETE
public:
    PerformanceCounterWrapper(){
        InitializeCriticalSection(&m_csHeap);
        m_cRef = 1;
        m_pHeap = 0;
        m_bPhysicalHeap = false;
    }

    ~PerformanceCounterWrapper(){
        if(m_pHeap){
            m_pHeap->Release();
        }
        DeleteCriticalSection(&m_csHeap);
    }

    HRESULT Initialize(bool bIsPhysicalHeap, IDirectMusicHeap* pHeap){
        m_bPhysicalHeap = bIsPhysicalHeap;
        m_pHeap = pHeap;
        m_pHeap->AddRef();
        m_dwHeapAllocation = 0;
        m_dwHeapAllocationPeak = 0;
        m_dwHeapAllocationBlocks = 0;
        m_dwHeapAllocationBlocksPeak = 0;
        m_dwHeapAllocationRate = 0;
        m_dwHeapAllocationBlocksRate = 0;
        return S_OK;
    }

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj)
    {
        return E_NOTIMPL;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void) 
        
    {
        return InterlockedIncrement(&m_cRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release( void) 
    {
        ULONG cRef;
            
        cRef = InterlockedDecrement(&m_cRef);

        if (cRef == 0)
            delete this;

        return cRef;
    }

    LONG m_cRef;

    /*  IDirectMusicHeap */
    STDMETHOD(Allocate)   (THIS_ DWORD dwSize, PVOID* ppData){
        PVOID pData = Alloc(dwSize);
        if(!pData){
            return E_OUTOFMEMORY;
        }
        *ppData = pData;
        return S_OK;
    }

    STDMETHOD(Free)   (THIS_ PVOID pData){
        return Free2(pData);
    }

    void* Alloc(size_t cb){
        DWORD* pBlock = 0;
        HRESULT hr = m_pHeap->Allocate(cb, (PVOID*) & pBlock);
        if(FAILED(hr)){
            return NULL;
        }
        if(!pBlock){
            return NULL;
        }
        ASSERTMSG("Allocated memory must be DWORD aligned.", (((int) pBlock) & 3) == 0);
        EnterCriticalSection(&m_csHeap);
        m_dwHeapAllocation += cb;
        if(m_dwHeapAllocation > m_dwHeapAllocationPeak){
            m_dwHeapAllocationPeak = m_dwHeapAllocation;
        }
        ++m_dwHeapAllocationBlocks;
        if(m_dwHeapAllocationBlocks > m_dwHeapAllocationBlocksPeak){
            m_dwHeapAllocationBlocksPeak = m_dwHeapAllocationBlocks;
        }
        m_dwHeapAllocationRate += cb;
        ++m_dwHeapAllocationBlocksRate;
        LeaveCriticalSection(&m_csHeap);

        return pBlock;
    }

    STDMETHOD(GetSize)(THIS_ PVOID pData, LPDWORD pcbSize){
        return m_pHeap->GetSize(pData, pcbSize);
    }

    HRESULT Free2(void *pv)
    {
        HRESULT hr = S_OK;
        if(pv){
            DWORD cb;
            hr = m_pHeap->GetSize(pv, &cb);
            if(SUCCEEDED(hr)){
                EnterCriticalSection(&m_csHeap);
                m_dwHeapAllocation -= cb;
                --m_dwHeapAllocationBlocks;
                LeaveCriticalSection(&m_csHeap);
                hr = m_pHeap->Free(pv);
            }
        }
        else {
            hr = E_FAIL;
        }
        return hr;
    }

    void RegisterPerformanceCounters(){
        DMusicRegisterPerformanceCounter( Name("bytes"),
                                      DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                      &m_dwHeapAllocation );
        DMusicRegisterPerformanceCounter( Name("bytes peak"),
                                      DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                      &m_dwHeapAllocationPeak );
        DMusicRegisterPerformanceCounter( Name("allocations"),
                                      DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                      &m_dwHeapAllocationBlocks );
        DMusicRegisterPerformanceCounter( Name("allocations peak"),
                                      DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                      &m_dwHeapAllocationBlocksPeak );
        DMusicRegisterPerformanceCounter( Name("allocation byte rate"),
                                      DMCOUNT_EVENT  | DMCOUNT_ASYNC32,
                                      &m_dwHeapAllocationRate );
        DMusicRegisterPerformanceCounter( Name("allocation rate"),
                                      DMCOUNT_EVENT  | DMCOUNT_ASYNC32,
                                      &m_dwHeapAllocationBlocksRate );

    }

    void UnregisterPerformanceCounters(){
        DMusicUnregisterPerformanceCounter(Name("bytes"));
        DMusicUnregisterPerformanceCounter(Name("bytes peak"));
        DMusicUnregisterPerformanceCounter(Name("allocations"));
        DMusicUnregisterPerformanceCounter(Name("allocations peak"));
        DMusicUnregisterPerformanceCounter(Name("allocation byte rate"));
        DMusicUnregisterPerformanceCounter(Name("allocation rate"));
    }

    char* Name(const char* sBase){
        _snprintf(m_sNameBuffer, sizeof(m_sNameBuffer), "DM heap %s %s", m_bPhysicalHeap ? "physical" : "normal" , sBase);
        return m_sNameBuffer;
    }

private:
    CRITICAL_SECTION m_csHeap;
    DWORD m_dwHeapAllocation;
    DWORD m_dwHeapAllocationPeak;
    DWORD m_dwHeapAllocationBlocks;
    DWORD m_dwHeapAllocationBlocksPeak;
    DWORD m_dwHeapAllocationRate;
    DWORD m_dwHeapAllocationBlocksRate;

    bool m_bPhysicalHeap;

    char m_sNameBuffer[100];

    IDirectMusicHeap* m_pHeap;
};

HRESULT WrapPerformanceCountersAroundHeap(bool bPhysicalHeap, IDirectMusicHeap* pInsideHeap, IDirectMusicHeap** ppHeap)
{
    HRESULT hr = S_OK;
    PerformanceCounterWrapper* pHeap = new PerformanceCounterWrapper();
    if(!pHeap){
        hr = E_OUTOFMEMORY;
    }
    if(SUCCEEDED(hr)){
        hr = pHeap->Initialize(bPhysicalHeap, pInsideHeap);
        if(SUCCEEDED(hr)){
            *ppHeap = pHeap;
        }
        else {
            pHeap->Release();
        }
    }
    return hr;
}


#endif // IMPLEMENT_PERFORMANCE_COUNTERS

void* WINAPI DirectMusicAlloc(size_t cb){
    return DirectMusicAllocI(cb);
}

void WINAPI DirectMusicFree(void *pv){
    DirectMusicFreeI(pv);
}

void* WINAPI DirectMusicPhysicalAlloc(size_t dwSize){
    return DirectMusicPhysicalAllocI(dwSize);
}

void WINAPI DirectMusicPhysicalFree(void* lpAddress){
    DirectMusicPhysicalFreeI(lpAddress);
}

HRESULT WINAPI DirectMusicCreateDefaultHeap(IDirectMusicHeap** ppHeap)
{
    HRESULT hr = S_OK;
    DirectMusicDefaultHeap* pHeap = new DirectMusicDefaultHeap();
    if(!pHeap){
        hr = E_OUTOFMEMORY;
    }
    if(SUCCEEDED(hr)){
        hr = pHeap->Initialize();
        if(SUCCEEDED(hr)){
            *ppHeap = pHeap;
        }
        else {
            pHeap->Release();
        }
    }
    return hr;
}

HRESULT WINAPI DirectMusicCreateDefaultPhysicalHeap(IDirectMusicHeap** ppHeap)
{
    HRESULT hr = S_OK;
    DirectMusicDefaultPhysicalHeap* pHeap = new DirectMusicDefaultPhysicalHeap();
    if(!pHeap){
        hr = E_OUTOFMEMORY;
    }
    if(SUCCEEDED(hr)){
        hr = pHeap->Initialize();
        if(SUCCEEDED(hr)){
            *ppHeap = pHeap;
        }
        else {
            pHeap->Release();
        }
    }
    return hr;
}

HRESULT WINAPI DirectMusicCreateFixedSizeHeap(DWORD dwHeapSize, IDirectMusicHeap** ppHeap)
{
    HRESULT hr = S_OK;
    DirectMusicFixedSizeHeap2* pHeap = new DirectMusicFixedSizeHeap2();
    if(!pHeap){
        hr = E_OUTOFMEMORY;
    }
    if(SUCCEEDED(hr)){
        hr = pHeap->Initialize(dwHeapSize);
        if(SUCCEEDED(hr)){
            *ppHeap = pHeap;
        }
        else {
            pHeap->Release();
        }
    }
    return hr;
}

HRESULT WINAPI DirectMusicCreateFixedSizePhysicalHeap(DWORD dwHeapSize, IDirectMusicHeap** ppHeap)
{
    HRESULT hr = S_OK;
    DirectMusicFixedSizePhysicalHeap* pHeap = new DirectMusicFixedSizePhysicalHeap();
    if(!pHeap){
        hr = E_OUTOFMEMORY;
    }
    if(SUCCEEDED(hr)){
        hr = pHeap->Initialize(dwHeapSize);
        if(SUCCEEDED(hr)){
            *ppHeap = pHeap;
        }
        else {
            pHeap->Release();
        }
    }
    return hr;
}

HRESULT WINAPI DirectMusicInitializeFixedSizeHeaps(DWORD dwNormalHeapSize, DWORD dwPhysicalHeapSize,
											  LPDIRECTMUSICFACTORYFN pFactory)
{
	IDirectMusicHeap* pPhysicalHeap = NULL;
	IDirectMusicHeap* pNormalHeap = NULL;
	HRESULT hr = S_OK;
	if(SUCCEEDED(hr)){
		hr = DirectMusicCreateFixedSizeHeap(dwNormalHeapSize, &pNormalHeap);
	}
	if(SUCCEEDED(hr)){
		hr = DirectMusicCreateFixedSizePhysicalHeap(dwPhysicalHeapSize, &pPhysicalHeap);
	}
	if(SUCCEEDED(hr)){
		hr = DirectMusicInitializeEx(pNormalHeap, pPhysicalHeap, pFactory);
	}
	if(pPhysicalHeap){
		pPhysicalHeap->Release();
	}
	if(pNormalHeap){
		pNormalHeap->Release();
	}
	return hr;
}

#ifdef DBG

#define MODULE "DMUSIC"

// @globalv Section in WIN.INI for all debug settings
const char szDebugSection[] = "debug";

// @globalv Key in WIN.INI for our debug level. All messages with
// a level of this number or lower will be displayed.
const char szDebugKey[] = MODULE;

// @globalv Key in WIN.INI [debug] section which determines if assert calls
// DebugBreak or not
//
const char szAssertBreak[] = "AssertBreak";

// @globalv Prefix for all debug outputs
//
const char szDebugPrefix[] = MODULE ": ";

// @globalv The current debug level. 
static int giDebugLevel = 1;

// The current RIP level.

static int giRIPLevel = 0;

// @globalv Do asserts break?
static BOOL gfAssertBreak = 1;

// @func Sets the debug level from WIN.INI
// 

#ifdef DXAPI
void DebugInit(
    void)
{
    giDebugLevel = GetProfileInt(szDebugSection, szDebugKey, 0);
    gfAssertBreak = GetProfileInt(szDebugSection, szAssertBreak, 0);
    giRIPLevel = 0;
    // Nepotism at its finest
    DebugTrace(-1, "Debug level is %d\n", giDebugLevel);
}
#endif // DXAPI

// @func Send a debug trace out.
//
// @comm Any message with a level less than or equal to the current debug
// level will be displayed using the OutputDebugString API. This means either
// the IDE Debug window if the app is running in that context or WDEB if
// it's running.
//
static BOOL fNeedPrefix = TRUE;
void DebugTrace(
    int iDebugLevel,        // @parm The debug level of this message
    LPSTR pstrFormat,       // @parm A printf style format string
    ...)                    // @parm | ... | Variable paramters based on <p pstrFormat>
{
    char sz[512];

    if (iDebugLevel != -1 && iDebugLevel > giDebugLevel)
    {
        return;
    }

    va_list va;

    va_start(va, pstrFormat);
    vsprintf(sz, pstrFormat, va);
    va_end(va);

    if (fNeedPrefix)
    {
        OutputDebugStringA(szDebugPrefix);
    }
    
    OutputDebugStringA(sz);

    // Let them construct multiple piece trace outs w/o
    // prefixing each one
    //
    fNeedPrefix = FALSE;
    for (;*pstrFormat && !fNeedPrefix; ++pstrFormat)
    {
        if (*pstrFormat == '\n')
        {
            fNeedPrefix = TRUE;
        }
    }
#ifndef DXAPI
	if ((iDebugLevel >= 0) && (iDebugLevel <= giRIPLevel)) 
	{
		DebugBreak();
	}
#endif
}

void DebugAssert(
    LPSTR szExp, 
    LPSTR szFile, 
    ULONG ulLine)
{
    DebugTrace(-1, "ASSERT: \"%s\" %s@%lu\n", szExp, szFile, ulLine);
    if (gfAssertBreak)
    {
        DebugBreak();
    }
}

void WINAPI DirectMusicSetDebugLevel(int iDebugLevel, int iRIPLevel)
{
    giDebugLevel = iDebugLevel;
    giRIPLevel = iRIPLevel;
}

CMemTrackList g_MemTrackList[DMTRACK_MAX];

CMemTrackList::CMemTrackList()
{
    InitializeCriticalSection(&m_CriticalSection);
}

CMemTrackList::~CMemTrackList()
{
    DeleteCriticalSection(&m_CriticalSection);
}

void CMemTrackList::AddHead(CMemTrack* pMemTrack) 
{ 
    EnterCriticalSection(&m_CriticalSection);
    AList::AddHead((AListItem*)pMemTrack);
    LeaveCriticalSection(&m_CriticalSection);
}

CMemTrack* CMemTrackList::GetHead()
{
    CMemTrack *pHead;
    EnterCriticalSection(&m_CriticalSection);
    pHead = (CMemTrack*)AList::GetHead();
    LeaveCriticalSection(&m_CriticalSection);
    return pHead;
}

CMemTrack* CMemTrackList::RemoveHead() 
{
    CMemTrack *pHead;
    EnterCriticalSection(&m_CriticalSection);
    pHead = (CMemTrack *) AList::RemoveHead();
    LeaveCriticalSection(&m_CriticalSection);
    return pHead;
}

void CMemTrackList::Remove(CMemTrack* pMemTrack)
{
    EnterCriticalSection(&m_CriticalSection);
    AList::Remove((AListItem*)pMemTrack);
    LeaveCriticalSection(&m_CriticalSection);
}

LONG CMemTrackList::GetCount()
{
    LONG lCount;
    EnterCriticalSection(&m_CriticalSection);
    lCount = AList::GetCount();
    LeaveCriticalSection(&m_CriticalSection);
    return lCount;
}

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
static LONG gDMusicClassInstanceCount[DMTRACK_MAX];
#endif

CMemTrack::CMemTrack(DWORD dwClass)
{
    m_dwClass = dwClass;
    if (dwClass < DMTRACK_MAX)
    {
        g_MemTrackList[dwClass].AddHead(this);
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
        InterlockedIncrement(&gDMusicClassInstanceCount[m_dwClass]);
#endif

    }
}

CMemTrack::~CMemTrack()
{
    if (m_dwClass < DMTRACK_MAX)
    {
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
        InterlockedDecrement(&gDMusicClassInstanceCount[m_dwClass]);
#endif
        g_MemTrackList[m_dwClass].Remove(this);
    }
}


//Don't forget to update enumClass in DEBUG.H when you update this gpNames array!!!
static char *gpNames[DMTRACK_MAX] = {
    "File Stream",
    "Memory Stream",
    "Stream Stream",
    "Segment",
    "Segment State",
    "Loader",
    "Performance",
    "Wave Track",
    "Wave",
    "Audio Path",
    "Audio Path Config",
    "Script",
    "Tool Graph",
    "Band",
    "Band Track",
    "DLS Collection",
    "Instrument",
    "Composer",
    "ChordMap",
    "Template",
    "SignPost Track",
    "ChordMap Track",
    "Lyrics Track",
    "Marker Track",
    "Parameter Control Track",
    "Segment Trigger Track",
    "Sequence Track",
    "Song",
    "System Exclusive Track",
    "Tempo Track",
    "Time Signature Track",
    "Container",
    "Synthesizer",
};

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
static void RegisterDMusicObjectCounters(){
    for (DWORD dwI = 0; dwI < DMTRACK_MAX;dwI++)
    {
        char buf[200];
        _snprintf(buf,sizeof(buf),"DM allocated %ss", gpNames[dwI]);
        buf[sizeof(buf)-1] = 0;
        DMusicRegisterPerformanceCounter( buf,
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &gDMusicClassInstanceCount[dwI] );
    }
}

static void UnregisterDMusicObjectCounters(){
    for (DWORD dwI = 0; dwI < DMTRACK_MAX;dwI++)
    {
        char buf[200];
        _snprintf(buf,sizeof(buf),"DM allocated %ss", gpNames[dwI]);
        buf[sizeof(buf)-1] = 0;
        DMusicUnregisterPerformanceCounter( buf );
    }
}

#endif // IMPLEMENT_PERFORMANCE_COUNTERS

long WINAPI DirectMusicMemCheck(DWORD dwMemType, char **ppName)

{
    if (dwMemType >= DMTRACK_MAX)
    {
        return -1;
    }
    if (ppName)
    {
        *ppName = gpNames[dwMemType];
    }
    return g_MemTrackList[dwMemType].GetCount();
}

void WINAPI DirectMusicDumpMemStats();

void WINAPI DirectMusicMemDump()
{
    DWORD dwI;
    DbgPrint("Objects Allocated:\n");
    for (dwI = 0; dwI < DMTRACK_MAX;dwI++)
    {
        if (g_MemTrackList[dwI].GetCount())
        {
            DbgPrint("%ld %s",g_MemTrackList[dwI].GetCount(),gpNames[dwI]);
            if (g_MemTrackList[dwI].GetCount() > 1)
            {
                DbgPrint("s\n");
            }
            else
            {
                DbgPrint("\n");
            }
        }
    }

    DirectMusicDumpMemStats();
}

#else
void WINAPI DirectMusicSetDebugLevel(int iDebugLevel, int iRIPLevel)
{
}
void WINAPI DirectMusicMemDump() 
{
}
long WINAPI DirectMusicMemCheck(DWORD dwMemType, char **pName) { return (long) 0; }

#endif

// Hooks to use an allocator from the application if there is one.
//

#include "..\shared\xalloc.h"

#ifdef DBG

// Define one of the following
// #define USE_SIZE_RECORDING_ALLOCATOR

// #define USE_LEAK_DETECTING_ALLOCATOR
// #define USE_XHEAP_WITH_LEAK_DETECTING_ALLOCATOR

// #define USE_XHEAP_ALLOCATOR

#define USE_C_ALLOCATOR


#else // non-debug

// #define USE_XHEAP_ALLOCATOR

#define USE_C_ALLOCATOR

#endif

interface IDirectMusicAlloc 
{
	virtual LPVOID STDMETHODCALLTYPE Alloc(THIS_ DWORD cb) = 0;
    
	virtual VOID STDMETHODCALLTYPE Free(THIS_ LPVOID pvFree) = 0;
};

#ifdef USE_SIZE_RECORDING_ALLOCATOR

class CDirectMusicAllocatorI : public IDirectMusicAlloc
{
public:
    CDirectMusicAllocatorI();

    STDMETHODIMP_(LPVOID) Alloc(THIS_ DWORD cb);
    STDMETHODIMP_(VOID)   Free(THIS_ LPVOID pvFree);

    bool EverCalled() const;

private:
    bool                    m_fEverCalled;
};

static CDirectMusicAllocatorI g_DefaultAllocator;
static IDirectMusicAlloc *g_pAllocI = &g_DefaultAllocator;

////////////////////////////////////////////////////////////////////////////////
//
// CDirectMusicAllocatorI::CDirectMusicAllocatorI
//
//
CDirectMusicAllocatorI::CDirectMusicAllocatorI()    
    : m_fEverCalled(false)
{
}

static DWORD dwAllocated[1000];

void WINAPI DirectMusicDumpMemStats()

{
    DWORD dwIndex;
    for (dwIndex = 0; dwIndex < 1000; dwIndex++)
    {
        if (dwAllocated[dwIndex])
        {
            Trace(1,"%ld: %ld\n",dwIndex,dwAllocated[dwIndex]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// CDirectMusicAllocatorI::Alloc
// CDirectMusicAllocatorI::Free
//
// Default allocator just uses malloc and free
//
// Also track if we've ever been called so we can disallow the user setting
// the allocator if there's already memory allocated.
//

STDMETHODIMP_(LPVOID) CDirectMusicAllocatorI::Alloc(THIS_ DWORD cb)
{
    if (!m_fEverCalled)
    {
        DWORD dwIndex;
        for (dwIndex = 0; dwIndex < 1000; dwIndex++)
        {
            dwAllocated[dwIndex] = 0;
        }
    }
    if (cb < 1000)
    {
        dwAllocated[cb]++;
    }
    m_fEverCalled = true;

    return malloc(cb);
}

STDMETHODIMP_(VOID) CDirectMusicAllocatorI::Free(THIS_ LPVOID pvAlloc)
{
    free(pvAlloc);
}

////////////////////////////////////////////////////////////////////////////////
//
// CDirectMusicAllocatorI::EverCalled
//
//
bool CDirectMusicAllocatorI::EverCalled() const
{
    return m_fEverCalled;
}

#endif // USE_SIZE_RECORDING_ALLOCATOR

#ifdef USE_LEAK_DETECTING_ALLOCATOR

#ifdef USE_XHEAP_WITH_LEAK_DETECTING_ALLOCATOR
  #define COMPILING_ROCKALL_LIBRARY
  #include "..\xheap\xheap\blendedheap.hpp"
  #include "..\xheap\xheap\smallheap.hpp"
  #include "..\xheap\xheap\fastheap.hpp"
  #include "..\xheap\xheap\debugheap.hpp"
  #include "..\xheap\xheap\dynamicdebugheap.hpp"
  #include "..\xheap\xheap\pageheap.hpp"
#endif

//////////////////////////////////////////////////////////////////////////////
//
// CAllocate
//
// Simple memory allocation class that can do diffs between two states
//
class CAllocate : public IDirectMusicAlloc
{
public:
    struct SnapBlock
    {
        void               *m_pBlock;                       // -> block as seen by app
        size_t              m_cb;                           // bytes asked for by app
        DWORD               m_RA;                           // Address of allocator
        DWORD               m_nSeq;                         // Allocation sequence number
        bool                m_fDiff;                        // After diff, true if this block
                                                            // is not present in the other
                                                            //  snapshot
        bool                m_fUser;                        // Used in 3-generation diff
    };

    struct SnapHeader
    {
        int                 m_nBlocks;                      // Number of blocks in snapshot
        DWORD               m_cbTotal;                      // Total allocated bytes (app)
        SnapBlock           m_rgBlocks[1];                  // The blocks
    };

    // Constructor
    //
    CAllocate();
    ~CAllocate();

    // Allocator implementation
    //
    STDMETHODIMP_(LPVOID) Alloc(DWORD cb);
    STDMETHODIMP_(VOID) Free(void *pv);

    // Utility functions
    //
    SnapHeader *Snapshot();
    static void FreeSnapshot(SnapHeader *ph);
    static void Diff(SnapHeader *ph1, SnapHeader *ph2);

    enum BlockSortKey
    {
        bsk_ReturnAddress,
        bsk_BlockAddress,
        bsk_Size
    };

    static void SortSnapshot(SnapHeader *ph, BlockSortKey bsk);

    bool EverCalled() const { return m_fEverCalled; }

    DWORD GetTotalAllocationCount() const { return m_nSeq; }

private:
    struct Block
    {
        Block              *m_pNext;                        // Next block in the chain
        size_t              m_cb;                           // Bytes (not including this header)
        DWORD               m_RA;                           // Address of requestor
        DWORD               m_nSeq;                         // Unique sequence number to id blocks
                                                            // with same address
    };

    CRITICAL_SECTION        m_cs;                           // Protect allocation list
    Block                  *m_pBlocks;                      // The list itself
    int                     m_nBlocks;                      // Number of outstanding allocations
    DWORD                   m_nSeq;                         // Next unique sequence number

    DWORD                   m_nBytes;
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    __int64                 m_llTotalBytes;                 // Total Bytes ever allocated
#endif

private:
    inline void EnterCS()       { EnterCriticalSection(&m_cs); }
    inline void LeaveCS()       { LeaveCriticalSection(&m_cs); }

    static int __cdecl SortRA(const void *p1, const void *p2);
    static int __cdecl SortBA(const void *p1, const void *p2);
    static int __cdecl SortSize(const void *p1, const void *p2);

    bool m_fEverCalled;

#ifdef USE_XHEAP_WITH_LEAK_DETECTING_ALLOCATOR
    // DEBUG_HEAP m_heap;
    // FAST_HEAP m_heap;
    // BLENDED_HEAP m_heap;
    // SMALL_HEAP m_heap;
#endif

};

static CAllocate g_DefaultAllocator;
static IDirectMusicAlloc *g_pAllocI = &g_DefaultAllocator;

//////////////////////////////////////////////////////////////////////////////
//
// GrovelRA
//
// This function is tailored to the way the allocator call chain is set up.
// It also will not work in retail.
//
#ifdef DBG
static DWORD __declspec(naked) GrovelRA()
{
    _asm
    {
        mov     eax, [ebp]
        mov     eax, [eax]
        mov     eax, [eax + 4]
        ret
    }
}

DWORD CalculatePagesSpanned(CAllocate::SnapHeader* pNew)
{
    g_DefaultAllocator.SortSnapshot(pNew, CAllocate::bsk_BlockAddress);
    DWORD dwNumPages = 0;
    DWORD dwCurrentPage = 0; // Assumes that blocks are never allocated out of the zero page.
    for(int i = 0; i < pNew->m_nBlocks; i++){
        CAllocate::SnapBlock* pBlock = & pNew->m_rgBlocks[i];
        DWORD dwBaseAddress = (DWORD) pBlock->m_pBlock;
        DWORD dwStartPage = (dwBaseAddress >> 12);
        DWORD dwEndPage = ((dwBaseAddress + pBlock->m_cb - 1) >> 12);
        dwNumPages += dwEndPage-dwStartPage+1;
        if(dwCurrentPage == dwStartPage){
            dwNumPages--;
        }
        dwCurrentPage = dwEndPage;
    }
    return dwNumPages;
}

#endif

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS

static HRESULT
__stdcall DMPagesSpannedCallback
(
    PLARGE_INTEGER pliCounter,
    PLARGE_INTEGER pUnused
)
{
    CAllocate::SnapHeader* pNew = g_DefaultAllocator.Snapshot();
    DWORD dwPagesSpanned = CalculatePagesSpanned(pNew);
    g_DefaultAllocator.FreeSnapshot(pNew);

    pliCounter->HighPart = 0;
    pliCounter->LowPart = dwPagesSpanned;
    return XBDM_NOERR;
}

static HRESULT
__stdcall DMHeapDensityCallback
(
    PLARGE_INTEGER pliCounter,
    PLARGE_INTEGER pUnused
)
{
    CAllocate::SnapHeader* pNew = g_DefaultAllocator.Snapshot();
    DWORD dwBytesSpanned = CalculatePagesSpanned(pNew) << 12;
    DWORD dwBytesAllocated = pNew->m_cbTotal;
    g_DefaultAllocator.FreeSnapshot(pNew);

    DWORD dwDensity = dwBytesAllocated ? (dwBytesAllocated * 100 / dwBytesSpanned) : 0;

    pliCounter->HighPart = 0;
    pliCounter->LowPart = dwDensity;
    return XBDM_NOERR;
}

static HRESULT
__stdcall DMHeapDumpCallback
(
    PLARGE_INTEGER pliCounter,
    PLARGE_INTEGER pUnused
)
{
    
    static DWORD gCallCount;

    if(gCallCount++ % 10 == 0){
        DirectMusicDumpMemStats();
    }

    pliCounter->HighPart = 0;
    pliCounter->LowPart = gCallCount;
    return XBDM_NOERR;
}

#endif

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS

extern "C" void DMusicRegisterPerformanceCounters(){
    RegisterDMusicObjectCounters();
    DMRegisterPMsgPerformanceCounters();
}

extern "C" void DMusicUnregisterPerformanceCounters(){
    UnregisterDMusicObjectCounters();
    DMUnregisterPMsgPerformanceCounters();
}

#endif


//////////////////////////////////////////////////////////////////////////////
//
// CAllocate::CAllocate
//
// 
CAllocate::CAllocate()
{
    InitializeCriticalSection(&m_cs);
    m_pBlocks = NULL;
    m_nBlocks = 0;
    m_nSeq = 0;
    m_fEverCalled = 0;
    m_nBytes = 0;

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    m_llTotalBytes = 0;
    DMusicRegisterPerformanceCounter( "DM heap bytes",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_nBytes );
    DMusicRegisterPerformanceCounter( "DM heap allocations",
                                  DMCOUNT_VALUE  | DMCOUNT_ASYNC32,
                                  &m_nBlocks );
    DMusicRegisterPerformanceCounter( "DM heap allocation byte rate",
                                  DMCOUNT_EVENT  | DMCOUNT_ASYNC64,
                                  &m_llTotalBytes );
    DMusicRegisterPerformanceCounter( "DM heap allocation rate",
                                  DMCOUNT_EVENT  | DMCOUNT_ASYNC32,
                                  &m_nSeq );
    DMusicRegisterPerformanceCounter("DM heap pages spanned",
        DMCOUNT_VALUE | DMCOUNT_SYNC,
        &DMPagesSpannedCallback);
    DMusicRegisterPerformanceCounter("DM heap density (0..100)",
        DMCOUNT_VALUE | DMCOUNT_SYNC,
        &DMHeapDensityCallback);
#if 0 // Seems to be called always, not just when visible in the UI -- perhaps an xbperfmon bug?
    DMusicRegisterPerformanceCounter("DM heap dump (look at debug output)",
        DMCOUNT_VALUE | DMCOUNT_SYNC,
        &DMHeapDumpCallback);
#endif
#endif
}

CAllocate::~CAllocate(){
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
    DMusicUnregisterPerformanceCounter("DM heap bytes");
    DMusicUnregisterPerformanceCounter("DM heap allocations");
    DMusicUnregisterPerformanceCounter("DM heap allocation byte rate");
    DMusicUnregisterPerformanceCounter("DM heap allocation rate");
    DMusicUnregisterPerformanceCounter("DM heap pages spanned");
    DMusicUnregisterPerformanceCounter("DM heap density (0..100)");
#if 0 // Seems to be called always, not just when visible in the UI -- perhaps an xbperfmon bug?
    DMusicUnregisterPerformanceCounter("DM heap dump (look at debug output)");
#endif
#endif
}

//////////////////////////////////////////////////////////////////////////////
//
// CAllocate::Alloc
//
// Allocate method exposed to the app.
//
// cb [IN] - Requested block size
// 
// Returns a pointer to the block or NULL on out of memory.
// 
STDMETHODIMP_(LPVOID) CAllocate::Alloc(DWORD cb)
{
    m_fEverCalled = true;

#ifdef USE_XHEAP_WITH_LEAK_DETECTING_ALLOCATOR

    Block *pb = (Block*)m_heap.New(sizeof(Block) + cb);

#else

    Block *pb = (Block*)malloc(sizeof(Block) + cb);

#endif

    if (pb == NULL)
        return NULL;
    
    pb->m_cb = cb;
    pb->m_nSeq = m_nSeq++;

#ifdef DBG
    pb->m_RA = GrovelRA();
#else
    pb->m_RA = 0;
#endif

    EnterCS();

        pb->m_pNext = m_pBlocks;
        m_pBlocks = pb;

        m_nBlocks++;

        m_nBytes += cb;

        m_llTotalBytes += cb;

    LeaveCS();

    return (LPVOID)(pb + 1);
}

//////////////////////////////////////////////////////////////////////////////
//
// CAllocate::Free
//
// Free method exposed to the app.
//
// pv [IN] - The pointer to free; must have been allocated by this allocator
//           instance.
// 
STDMETHODIMP_(VOID) CAllocate::Free(void *pv)
{
    Block *pb = (Block*)pv;

    if (pb == NULL)
        return;

    pb--;

    EnterCS();

        Block *prev = NULL;
        Block *curr = m_pBlocks;

        while (curr && curr != pb)
        {
            prev = curr;
            curr = curr->m_pNext;
        }

        // If this fires then the block did not come from us.
        // 
        assert( curr );

        if (curr) 
        {
            if (prev)
            {
                prev->m_pNext = curr->m_pNext;
            }
            else
            {
                m_pBlocks = curr->m_pNext;
            }

            m_nBytes -= curr->m_cb;

#ifdef USE_XHEAP_WITH_LEAK_DETECTING_ALLOCATOR

            m_heap.Delete(pb);

#else
            free(pb);
#endif

            m_nBlocks--;
        }

    LeaveCS();
}

//////////////////////////////////////////////////////////////////////////////
//
// CAllocate::Snapshot
//
// Returns a pointer to a block of memory describing all the outstanding 
// allocations. This pointer must eventually be free'd with FreeSnapshot.
//
CAllocate::SnapHeader *CAllocate::Snapshot()
{
    EnterCS();

        int n = max(0, m_nBlocks - 1);

        SnapHeader *ph = (SnapHeader*)malloc(sizeof(SnapHeader) + n * sizeof(SnapBlock));
        if (ph)
        {
            ph->m_nBlocks = m_nBlocks;

            n = 0;

            DWORD cbTotal = 0;

            for (Block *pb = m_pBlocks; pb; pb = pb->m_pNext, n++)
            {
                ph->m_rgBlocks[n].m_cb = pb->m_cb;
                ph->m_rgBlocks[n].m_RA = pb->m_RA;
                ph->m_rgBlocks[n].m_nSeq = pb->m_nSeq;
                ph->m_rgBlocks[n].m_pBlock = (LPVOID)(pb + 1);

                cbTotal += pb->m_cb;
            }

            ph->m_cbTotal = cbTotal;

            assert( n == m_nBlocks );
        }

    LeaveCS();

    return ph;
}

//////////////////////////////////////////////////////////////////////////////
//
// CAllocate::FreeSnapshot
//
// Free's a shapshot.
//
void CAllocate::FreeSnapshot(SnapHeader *psh)
{
    free(psh);
}

//////////////////////////////////////////////////////////////////////////////
//
// CAllocate::Diff
//
// Generates the diff of two snapshots. Each entry in both snapshots will have
// the m_fDiff flag set to false if the block exists in both snapshots; else
// it will be set to true. 
//
// If ph1 was taken before ph2, then m_fDiff set on a ph1 entry means that 
// entry has since been free'd; while if the flag is set on a ph2 entry it
// means that the entry has since been allocated.
//
// This will resort both snapshots.
//
void CAllocate::Diff(SnapHeader *ph1, SnapHeader *ph2)
{
    int n1 = 0;
    int n2 = 0;

    SortSnapshot(ph1, bsk_BlockAddress);
    SortSnapshot(ph2, bsk_BlockAddress);

    while (n1 < ph1->m_nBlocks && n2 < ph2->m_nBlocks)
    {
        while (
            ph1->m_rgBlocks[n1].m_pBlock == ph2->m_rgBlocks[n2].m_pBlock &&
            n1 < ph1->m_nBlocks && n2 < ph2->m_nBlocks)
        {
            // This catches the case where the underlying allocator has
            // returned a previously used address.
            //
            bool fDiff = ph1->m_rgBlocks[n1].m_nSeq != ph2->m_rgBlocks[n2].m_nSeq;

            ph1->m_rgBlocks[n1++].m_fDiff = fDiff;
            ph2->m_rgBlocks[n2++].m_fDiff = fDiff;
        }

        while (
            ph1->m_rgBlocks[n1].m_pBlock < ph2->m_rgBlocks[n2].m_pBlock &&
            n1 < ph1->m_nBlocks)
        {
            ph1->m_rgBlocks[n1++].m_fDiff = true;
        }
        
        while (
            ph1->m_rgBlocks[n1].m_pBlock > ph2->m_rgBlocks[n2].m_pBlock &&
            n2 < ph2->m_nBlocks)
        {
            ph2->m_rgBlocks[n2++].m_fDiff = true;
        }
    }

    while (n1 < ph1->m_nBlocks)
    {
        ph1->m_rgBlocks[n1++].m_fDiff = true;
    }

    while (n2 < ph2->m_nBlocks)
    {
        ph2->m_rgBlocks[n2++].m_fDiff = true;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// CAllocate::SortSnapshot
//
// Sort the snapshot by the given key.
//
void CAllocate::SortSnapshot(SnapHeader *ph, BlockSortKey bsk)
{
    int (__cdecl *comp)(const void *, const void *) = NULL;

    switch (bsk)
    {
    case bsk_ReturnAddress:
        comp = SortRA;
        break;

    case bsk_BlockAddress:
        comp = SortBA;
        break;

    case bsk_Size:
        comp = SortSize;
        break;

    default:
        assert( false );
    }

    if (comp)
    {
        qsort(ph->m_rgBlocks, ph->m_nBlocks, sizeof(SnapBlock), comp);
    }
}


//////////////////////////////////////////////////////////////////////////////
//
// CAllocate::SortXxx
//
// Key compare functions for qsort
//
int __cdecl CAllocate::SortRA(const void *pv1, const void *pv2)
{
    const SnapBlock *pb1 = (const SnapBlock *)pv1;
    const SnapBlock *pb2 = (const SnapBlock *)pv2;

    if (pb1->m_RA < pb2->m_RA)
        return -1;
    else if (pb1->m_RA > pb2->m_RA)
        return 1;

    return 0;
}

int __cdecl CAllocate::SortBA(const void *pv1, const void *pv2)
{
    const SnapBlock *pb1 = (const SnapBlock *)pv1;
    const SnapBlock *pb2 = (const SnapBlock *)pv2;

    if (pb1->m_pBlock < pb2->m_pBlock)
        return -1;
    else if (pb1->m_pBlock > pb2->m_pBlock)
        return 1;

    return 0;
}

int __cdecl CAllocate::SortSize(const void *pv1, const void *pv2)
{
    const SnapBlock *pb1 = (const SnapBlock *)pv1;
    const SnapBlock *pb2 = (const SnapBlock *)pv2;

    if (pb1->m_cb < pb2->m_cb)
        return -1;
    else if (pb1->m_cb > pb2->m_cb)
        return 1;

    return 0;
}

static CAllocate::SnapHeader* pMiddle;
static CAllocate::SnapHeader* pNew;

static void PrintGarbage(const CAllocate::SnapBlock* pGarbageBlock, DWORD totalNum, DWORD totalMem){
    DbgPrint(" Allocator: 0x%08x instance( addr 0x%08x, seq %d) %5d blocks, %7d bytes average %7d bytes total\n",
        pGarbageBlock->m_RA, pGarbageBlock->m_pBlock, pGarbageBlock->m_nSeq, totalNum, totalNum ? totalMem / totalNum : totalMem, totalMem);
}

void WINAPI DirectMusicDumpMemStats()
{
    if(g_pAllocI != &g_DefaultAllocator){
        return; // Allocator has been replaced.
    }

    CAllocate::SnapHeader* pAncient;
    pAncient = pMiddle;
    pMiddle = pNew;
    pNew = g_DefaultAllocator.Snapshot();

    DbgPrint("Total allocated memory: %d bytes, %d active blocks, %d allocations\n",
        pNew->m_cbTotal, pNew->m_nBlocks, g_DefaultAllocator.GetTotalAllocationCount());

    // Calculate how many heap blocks are covered (for trying to minimize lacy heaps)
    {
        DWORD dwNumPages = CalculatePagesSpanned(pNew);
        DbgPrint("DMusic heap allocations span %d pages.\n", dwNumPages);
    }

    // Find 20 largest groups of objects..

    {
        DbgPrint("DMusic heap most frequent allocations:\n");
        g_DefaultAllocator.SortSnapshot(pNew, CAllocate::bsk_ReturnAddress);
        // How many different allocators?
        DWORD dwRA = 0;
        DWORD dwNumRA = 0;
        for(int i = 0; i < pNew->m_nBlocks; i++){
            if(pNew->m_rgBlocks[i].m_RA == dwRA){
            }
            else{
                dwNumRA++;
                dwRA = pNew->m_rgBlocks[i].m_RA;
            }   
        }
        DbgPrint("number of distinct allocators: %d\n", dwNumRA);
        struct RACount {
            DWORD m_dwRA;
            DWORD m_dwCount;
            static int __cdecl SortByCount(const void* pA, const void* pB){
                return ((RACount*) pA)->m_dwCount - ((RACount*) pB)->m_dwCount;
            }
        };
        RACount* pRACount = new RACount[dwNumRA];
        dwRA = 0;
        DWORD dwCurrentRA = 0;
        for(int i = 0; i < pNew->m_nBlocks; i++){
            if(pNew->m_rgBlocks[i].m_RA == dwRA){
                pRACount[dwCurrentRA].m_dwCount++;
            }
            else{
                if(dwRA != 0){
                    dwCurrentRA++;
                }
                dwRA = pNew->m_rgBlocks[i].m_RA; 
                pRACount[dwCurrentRA].m_dwRA = dwRA;
                pRACount[dwCurrentRA].m_dwCount = 1;
            }   
        }
        qsort(pRACount,dwNumRA,sizeof(RACount),RACount::SortByCount);

        for(DWORD i = 0; i < dwNumRA && i < 20; i++){
            RACount* pRAC = &pRACount[dwNumRA-(i+1)];
            DbgPrint("%2d: allocator: %8x instances: %4d\n",
                i, pRAC->m_dwRA, pRAC->m_dwCount);
        }
        delete [] pRACount;
    }

#ifdef TRACK_LONG_TERM_MEMORY_GROWTH

    if(!pAncient){
        DbgPrint("Not enough snapshots to generate long-term memory growth report yet.\n");
        return; // Not enough snapshots yet.
    }

    // Anything that's not in pAncient, but is in Middle and New, is new garbage

    g_DefaultAllocator.Diff(pAncient, pNew);
    for(int i = 0; i < pNew->m_nBlocks; i++){
        pNew->m_rgBlocks[i].m_fUser = pNew->m_rgBlocks[i].m_fDiff;
    }

    g_DefaultAllocator.Diff(pMiddle, pNew);

    // If m_fUser is true, that means block was not in old.
    // If m_fDiff is false, that means the block was in middle
    // Therefore it's probably new garbage

    // Sort by allocator

    g_DefaultAllocator.SortSnapshot(pNew, CAllocate::bsk_ReturnAddress);

    bool bPrintHeader = true;

    size_t totalMem = 0;
    size_t totalNum = 0;
    size_t totalAllocators = 0;

    CAllocate::SnapBlock* pGarbageBlock = 0;

    DbgPrint("Recently created long-term data, possibly garbage:\n");
    for(int i = 0; i < pNew->m_nBlocks; i++){
        CAllocate::SnapBlock* pBlock = & pNew->m_rgBlocks[i];
        if( pBlock->m_fUser && ! pBlock->m_fDiff) {
            if(bPrintHeader){
                bPrintHeader = false;
            }

            if(!pGarbageBlock || pGarbageBlock->m_RA != pBlock->m_RA){
                if(pGarbageBlock){
                    totalAllocators++;
                    PrintGarbage(pGarbageBlock, totalNum, totalMem);
                }
                pGarbageBlock = pBlock;
                totalNum = 0;
                totalMem = 0;
            }
            totalNum++;
            totalMem += pBlock->m_cb;
        }
    }
    if(pGarbageBlock){
        totalAllocators++;
        PrintGarbage(pGarbageBlock, totalNum, totalMem);
    }
    if(totalAllocators){
        DbgPrint(" end of report.\n");
    }
    else {
        DbgPrint(" no new blocks found.\n");
    }

#endif // TRACK_LONG_TERM_MEMORY_GROWTH

    if(pAncient){
        g_DefaultAllocator.FreeSnapshot(pAncient);
    }
}

#endif // USE_LEAK_DETECTING_ALLOCATOR

#ifdef USE_XHEAP_ALLOCATOR

#define COMPILING_ROCKALL_LIBRARY
#include "..\xheap\xheap\blendedheap.hpp"
#include "..\xheap\xheap\smallheap.hpp"
#include "..\xheap\xheap\fastheap.hpp"
#include "..\xheap\xheap\debugheap.hpp"
#include "..\xheap\xheap\dynamicdebugheap.hpp"
#include "..\xheap\xheap\pageheap.hpp"

static FAST_HEAP gDMusicHeap;


void WINAPI DirectMusicDumpMemStats()

{
    Trace(1,"DirectMusicDumpMemStats: Using xheap.\n");
}

////////////////////////////////////////////////////////////////////////////////
//
// Internal alloc/free routines to be called from operator new/delete
//
//
void* DirectMusicAllocI(size_t cb)
{
    return gDMusicHeap.New(cb);
}

void DirectMusicFreeI(void *pv)
{
    gDMusicHeap.Delete(pv);
}


#endif // USE_XHEAP_ALLOCATOR

#ifdef USE_C_ALLOCATOR

void WINAPI DirectMusicDumpMemStats()

{
    Trace(1,"DirectMusicDumpMemStats: Using C allocator.\n");
}

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
extern "C" void DMusicRegisterPerformanceCounters(){
    if(gpDMHeap){
        // We know this is really a PerformanceCounterWrapper
        ((PerformanceCounterWrapper*)gpDMHeap)->RegisterPerformanceCounters();
    }
    if(gpDMPhysicalHeap){
        // We know this is really a PerformanceCounterWrapper
        ((PerformanceCounterWrapper*)gpDMPhysicalHeap)->RegisterPerformanceCounters();
    }

#ifdef DBG
    RegisterDMusicObjectCounters();
#endif
    DMRegisterPMsgPerformanceCounters();
}

extern "C" void DMusicUnregisterPerformanceCounters(){
    if(gpDMHeap){
        // We know this is really a PerformanceCounterWrapper
        ((PerformanceCounterWrapper*)gpDMHeap)->UnregisterPerformanceCounters();
    }
    if(gpDMPhysicalHeap){
        // We know this is really a PerformanceCounterWrapper
        ((PerformanceCounterWrapper*)gpDMPhysicalHeap)->UnregisterPerformanceCounters();
    }
#ifdef DBG
    UnregisterDMusicObjectCounters();
#endif
    DMUnregisterPMsgPerformanceCounters();
}

#endif // IMPLEMENT_PERFORMANCE_COUNTERS

#endif // USE_C_ALLOCATOR

#if ! ( defined(USE_XHEAP_ALLOCATOR) || defined(USE_C_ALLOCATOR) )

////////////////////////////////////////////////////////////////////////////////
//
// DirectMusicSetAllocator 
//
// Allow the app to specify an interface to manage memory allocations.
//
// This function must be called once, and it must be called before 
//    InitAudioX. This prevents allocations being made via malloc
//    that will attempt to be free'd through the interface and vice
//    versa.
//
//
HRESULT WINAPI DirectMusicSetAllocator(IDirectMusicAlloc *pAlloc)
{
    if (g_pAllocI != &g_DefaultAllocator)
    {
        return DMUS_E_NOT_INIT;         // Someone has already set us
    }
    
    if (g_DefaultAllocator.EverCalled())
    {
        return DMUS_E_NOT_INIT;         // Too late, there's already memory allocated
    }

    if (!pAlloc)
    {
        return E_INVALIDARG;
    }
    
    g_pAllocI = pAlloc;

    return S_OK;
}



////////////////////////////////////////////////////////////////////////////////
//
// Internal alloc/free routines to be called from operator new/delete
//
//
void* DirectMusicAllocI(size_t cb)
{
    return g_pAllocI->Alloc(cb);
}

void DirectMusicFreeI(void *pv)
{
    g_pAllocI->Free(pv);
}


#endif // ! USE_XHEAP_ALLOCATOR
