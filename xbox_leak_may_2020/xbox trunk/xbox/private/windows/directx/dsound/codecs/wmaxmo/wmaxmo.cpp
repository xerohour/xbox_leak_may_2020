/***************************************************************************
*
*  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
*
*  File:       wmaxmo.c
*  Content:    WMA XMO en/decode API.
*  History:
*   Date       By        Reason
*   ====       ==        ======
*  10/03/00    jpalevic  Created.
*  01/12/2001  georgioc  overhaul to make it more user friendly and conform to xmo spec
****************************************************************************/

#pragma code_seg("WMADECXMO")
#pragma data_seg("WMADECXMO_RW")
#pragma const_seg("WMADECXMO_RD")

#pragma comment(linker, "/merge:WMADECXMO_RW=WMADECXMO")
#pragma comment(linker, "/merge:WMADECXMO_RD=WMADECXMO")
#pragma comment(linker, "/section:WMADECXMO,ERW")

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>
#include <ntos.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#include <xtl.h>
#include "..\..\dsound\dsndver.h"
#include <xdbg.h>
#include "wmaxmo.h"
#include "wmaxmop.h"
#include "wma_dec_win32_x86.h"

#if DBG

    ULONG g_WmaXmoDebugLevel = DEBUG_LEVEL_WARNING;

#endif



//
// this is the intermediate XMO instantiation function
//

HRESULT 
WmaCreateInMemoryDecoderEx(
    LPFNWMAXMODATACALLBACK  pCallback,
    PVOID                   pCallbackContext,
    DWORD                   dwYieldRate,
    LPWAVEFORMATEX          pwfxCompressed,
    XWmaFileMediaObject     **ppMediaObject)
{

    HRESULT hr = S_OK;

    hr = WmaCreateInMemoryDecoder(
        pCallback,
        pCallbackContext,
        dwYieldRate,
        pwfxCompressed,
        (XMediaObject **)ppMediaObject);

    return hr;

}

HRESULT 
WmaCreateInMemoryDecoder(
    LPFNWMAXMODATACALLBACK  pCallback,
    PVOID                   pCallbackContext,
    DWORD                   dwYieldRate,
    LPWAVEFORMATEX          pwfxCompressed,
    XMediaObject            **ppMediaObject)
{


    HRESULT                 hr;
    MYCHECK(ppMediaObject);
    
    CWMAXMediaObject* pXMO;
        
    if (dwYieldRate == 0) {

        //
        // never yield
        //

        dwYieldRate = 1000;

    }

    //
    // pcallbackBack
    //

    if (pCallback == NULL) {

        MYCHECK(pCallback != NULL);
        *ppMediaObject = NULL;
        return E_FAIL;

    }

    pXMO = new CWMAXMediaObject();
    if (pXMO == NULL) {
        return E_OUTOFMEMORY;
    }    

    //
    // initialize wma xmo
    //

    hr = pXMO->InitializeIntermediate(
        pCallback,
        pCallbackContext,
        dwYieldRate,
        pwfxCompressed);


    if (SUCCEEDED(hr)) {

        *ppMediaObject = (XMediaObject *) pXMO;

    } else {

        *ppMediaObject = NULL;
        pXMO->Release();

    }

    return hr;

}

HRESULT
WmaCreateDecoderEx(
    LPCSTR                  pszFileName, 
    HANDLE                  hFile,
    BOOL                    fAsyncMode,
    DWORD                   dwLookaheadBufferSize,
    DWORD                   dwMaxPackets,
    DWORD                   dwYieldRate,
    LPWAVEFORMATEX          pwfxCompressed,
    XWmaFileMediaObject        **ppMediaObject)
{
    HRESULT hr = S_OK;

    hr = WmaCreateDecoder(
        pszFileName, 
        hFile,
        fAsyncMode,
        dwLookaheadBufferSize,
        dwMaxPackets,
        dwYieldRate,
        pwfxCompressed,
        (XFileMediaObject **)ppMediaObject);

    return hr;

}


HRESULT
WmaCreateDecoder(
    LPCSTR                  pszFileName, 
    HANDLE                  hFile,
    BOOL                    fAsyncMode,
    DWORD                   dwLookaheadBufferSize,
    DWORD                   dwMaxPackets,
    DWORD                   dwYieldRate,
    LPWAVEFORMATEX          pwfxCompressed,
    XFileMediaObject        **ppMediaObject)
{
    HRESULT                 hr;
    MYCHECK(ppMediaObject);
    
    CWMAXMediaObject* pXMO;
        
    if (dwYieldRate == 0) {

        //
        // never yield
        //

        dwYieldRate = 1000;

    }

    pXMO = new CWMAXMediaObject();
    if (pXMO == NULL) {
        return E_OUTOFMEMORY;
    }    

    //
    // initialize wma xmo
    //

    hr = pXMO->Initialize(pszFileName,
                          hFile,
                          fAsyncMode,
                          dwLookaheadBufferSize,
                          dwMaxPackets,
                          dwYieldRate,
                          pwfxCompressed);
    
    if (SUCCEEDED(hr)) {

        *ppMediaObject = pXMO;

    } else {

        *ppMediaObject = NULL;
        pXMO->Release();

    }

    return hr;
}


CWMAXMediaObject::CWMAXMediaObject()
{
    ZeroMemory(&m_state, sizeof (m_state));
    ZeroMemory(&m_hdr, sizeof (m_hdr));
    m_dwYieldRate = 1000;
    m_pSrcBuffer = NULL;
    m_dwMaxContexts = 0;
    m_pContextPool = NULL;
    m_bDecodeMore = TRUE;
    m_bAsyncMode = FALSE;
    m_bExternalHandle = FALSE;
    m_cRef = 0;
    m_dwPendingPackets = 0;
    m_dwSrcBufferOffset = 0;
    m_dwCurrentWmaFileOffset = 0;
    m_pCallback = NULL;
    m_pCallbackContext = NULL;
    m_hThread = NULL;
    m_hDeleteThreadEvent = NULL;
    m_hFile = INVALID_HANDLE_VALUE;
    m_dwThreadId = 0;

    memset(&m_overlapped,0,sizeof(OVERLAPPED));
    InitializeCriticalSection(&m_CS);

}

CWMAXMediaObject::~CWMAXMediaObject()
{

    Reset();

    //
    // set the delete event
    //

    if (m_hDeleteThreadEvent) {

        SetEvent(m_hDeleteThreadEvent);

    }

    //
    // wait on thread exiting..
    //

    if (m_hThread) {
        WaitForSingleObject(m_hThread,INFINITE);
    }

    ASSERT(m_dwThreadId == 0);


    if ( m_state ) {
        WMAFileDecodeClose(&m_state);
    }

    if (m_pSrcBuffer) {
        delete [] m_pSrcBuffer;
    }

    if (m_pContextPool) {
        delete [] m_pContextPool;
    }


    if ((m_hFile != INVALID_HANDLE_VALUE) && (m_bExternalHandle == FALSE)) {
        CloseHandle(m_hFile);
    }

    if (m_hDeleteThreadEvent) {

        CloseHandle(m_hDeleteThreadEvent);

    }

    if (m_overlapped.hEvent) {
        CloseHandle(m_overlapped.hEvent);
    }
}


VOID CWMAXMediaObject::Reset()
{

    //
    // wait until out pending list is  empty
    //

    while (TRUE) {

        if (IsListEmpty(&m_PendingList)) {

            break;

        }

        Sleep(5);

    }

}


HRESULT CWMAXMediaObject::InitializeIntermediate(
    LPFNWMAXMODATACALLBACK     pCallback,
    PVOID                      pCallbackContext,
    DWORD                      dwYieldRate,
    LPWAVEFORMATEX             pwfxCompressed
    )
{
    HRESULT hr;
    m_bAsyncMode = FALSE;

    m_dwYieldRate = dwYieldRate;
    m_dwLookaheadSize = 0;

    m_pCallback = pCallback;
    m_pCallbackContext = pCallbackContext;

    InitializeListHead(&m_AvailableList);
    InitializeListHead(&m_PendingList);

    AddRef();

    hr = Flush();
    
    if (FAILED(hr)) {
        return hr;
    }

    if (pwfxCompressed) {
        GetFormat(pwfxCompressed);
    }

    return hr;

}


HRESULT CWMAXMediaObject::Initialize(
    LPCSTR pszFileName, 
    HANDLE hFile,
    BOOL fAsyncMode,
    DWORD dwLookaheadSize,
    DWORD dwMaxPackets,
    DWORD dwYieldRate,
    LPWAVEFORMATEX pwfxCompressed
    )
{
    HRESULT hr;
    PWMAXMO_ASYNC_CONTEXT pContext;

    AddRef();

    m_dwYieldRate = dwYieldRate;

    //
    // create a file media object to read the raw wma data from
    //

    if (hFile == NULL) {

        MYCHECK(pszFileName);
        m_bExternalHandle = FALSE;

        m_hFile = CreateFile(pszFileName,
                             GENERIC_READ,
                             FILE_SHARE_READ,
                             NULL,
                             OPEN_EXISTING,
                             (fAsyncMode ? (FILE_FLAG_OVERLAPPED) : 0),
                             NULL);

    } else {

        m_bExternalHandle = TRUE;
        m_hFile = hFile;

        NTSTATUS status;
        IO_STATUS_BLOCK IoStatusBlock;
        DWORD dwMode;

        status = NtQueryInformationFile(
                hFile,
                &IoStatusBlock,
                &dwMode,
                sizeof(dwMode),
                FileModeInformation
                );

        MYCHECK(!(dwMode &  FILE_NO_INTERMEDIATE_BUFFERING) && "File Handle passed was opened async. Not supported.");

    }


    if (m_hFile == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    //
    // allocate packet buffer.assume 16 bit resolution for all wma files
    //

    m_dwLookaheadSize = max(WMA_SRC_BUFFER_SIZE,dwLookaheadSize)*2;
    
    m_pSrcBuffer = new BYTE[m_dwLookaheadSize];

    if (m_pSrcBuffer == NULL) {
        return E_OUTOFMEMORY;
    }

    m_bAsyncMode = fAsyncMode;
    m_dwMaxContexts = max(2,dwMaxPackets);

    //
    // init state
    //

    InitializeListHead(&m_AvailableList);
    InitializeListHead(&m_PendingList);

    //
    // we are initialized ok. Now create a thread that will gives us
    // some context to run and check if packets have been submitted
    //

    if (fAsyncMode == TRUE) {

        //
        // alloc a buffer for contexts
        //
    
        m_pContextPool = (PUCHAR) new WMAXMO_ASYNC_CONTEXT[m_dwMaxContexts];
    
        if (m_pContextPool == NULL) {
            return E_OUTOFMEMORY;
        }
    
        //
        // insert contexts in list..
        //
    
        for (ULONG i=0;i<m_dwMaxContexts;i++) {
    
            pContext = (PWMAXMO_ASYNC_CONTEXT) (m_pContextPool + sizeof(WMAXMO_ASYNC_CONTEXT)*i);
            InitializeListHead(&pContext->ListEntry);
            InsertHeadList(&m_AvailableList,
                           &pContext->ListEntry);
    
        }

        m_hThread = CreateThread(NULL,
                                 8192,
                                 WmaXmoThreadProc,
                                 this,
                                 0,
                                 &m_dwThreadId);

        if (m_hThread == NULL) {
            return E_OUTOFMEMORY;
        }

        //
        // create event that will signal thread to terminate
        //

        m_hDeleteThreadEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

        if (m_hDeleteThreadEvent == NULL) {
            return E_OUTOFMEMORY;
        }

        m_overlapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);;

        if (m_overlapped.hEvent == NULL) {
            return E_OUTOFMEMORY;
        }

    }

    hr = Flush();
    
    if (FAILED(hr)) {
        return hr;
    }

    if (pwfxCompressed) {
        GetFormat(pwfxCompressed);
    }
    
    return hr;

}

HRESULT STDMETHODCALLTYPE CWMAXMediaObject::Flush()
{
    tWMAFileStatus rc;
    HRESULT hr;
    DWORD dwBytesRead;
    BOOL bResult;

    DBG_INFO(("WmaXmo (%x): Flush Called\n",this));

    Reset();

    ENTER_CS(&m_CS);

    m_dwSrcBufferOffset = 0;
    m_dwCurrentWmaFileOffset = 0;

    DBG_INFO(("WmaXmo (%x): Flush: SrcBufferOffset = 0x%x, WmaFileOffset 0x%x\n",
             this,
             m_dwSrcBufferOffset,
             m_dwCurrentWmaFileOffset));

    //
    // for the in-memory wma, we dont read any data from file
    //

    if (m_pCallback == NULL) {

        //
        // prime the src buffer with file data (wma header etc)
        //
    
        SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);

        m_overlapped.Offset = 0;
        m_overlapped.OffsetHigh = 0;

        bResult = ReadFile(m_hFile,
                     m_pSrcBuffer,
                     m_dwLookaheadSize,
                     &dwBytesRead,
                     (m_bAsyncMode ? &m_overlapped : 0)) ;

        if (!bResult && !m_bAsyncMode) {

            LEAVE_CS(&m_CS);
            return HRESULT_FROM_WIN32(GetLastError());

        }
     
        if (m_bAsyncMode) {
    
            bResult = GetOverlappedResult(m_hFile,&m_overlapped,&dwBytesRead,TRUE);
            if (!bResult) {
    
                LEAVE_CS(&m_CS);
                return HRESULT_FROM_WIN32(GetLastError());
    
            }
    
        }

    }

        
    rc = WMAFileDecodeCreate(&m_state, this);

    if (rc != cWMA_NoErr) {

        DBG_WARN(("WmaXmo (%x): Flush: WMA decoder failed creation. Most likely due to out-of-memory.\n",this));

        LEAVE_CS(&m_CS);
        return E_FAIL;

    }

    rc = WMAFileDecodeInit( m_state);
    
    DBG_INFO(("m_state = 0x%08x\n", m_state));
    
    if (rc != cWMA_NoErr) {

        DBG_WARN(("WmaXmo (%x): Flush: WMA decoder failed init. Most likely due to out-of-memory.\n",this));

        LEAVE_CS(&m_CS);
        return E_FAIL;

    }

    rc = WMAFileDecodeInfo( m_state, &m_hdr);
    
    if (rc != cWMA_NoErr) {

        DBG_WARN(("WmaXmo (%x): Flush: WMA decoder failed init 2. Most likely due to out-of-memory.\n",this));

        LEAVE_CS(&m_CS);
        return E_FAIL;

    }

    if (m_hdr.has_DRM) {
        
        DBG_ERROR(( "DRM FILES NOT SUPPORTED!\n")); // Cannot open DRM licensed files.

        LEAVE_CS(&m_CS);
        return E_FAIL;

    }

    m_bDecodeMore = TRUE;

    LEAVE_CS(&m_CS);
    return S_OK;
}

DWORD WINAPI WmaXmoThreadProc(
    LPVOID pParameter
    )
{
    CWMAXMediaObject *pThis = (CWMAXMediaObject *) pParameter;
    pThis->ThreadProc();

    return 0;
}

VOID CWMAXMediaObject::ThreadProc()
{
    HRESULT hr = S_OK;

    while (m_cRef > 0) {

        //
        // this function should have been called in response to an event being signalled
        //
    
        if (WaitForSingleObject(m_hDeleteThreadEvent,0) == WAIT_OBJECT_0) {

            //
            // thread needs to be deleted, bail...
            //

            DBG_INFO(("ThreadProc: Leaving thread\n"));
            goto exit;

        }

        ENTER_CS(&m_CS);

        if (!IsListEmpty(&m_PendingList)) {
    
            PWMAXMO_ASYNC_CONTEXT pCtx;

            pCtx = (PWMAXMO_ASYNC_CONTEXT) m_PendingList.Flink;
            hr =  ReadWmaDataFromFile((PUCHAR)m_pSrcBuffer,
                                      m_dwLookaheadSize,
                                      pCtx);

            LEAVE_CS(&m_CS);

            if (hr == E_ABORT) {

                goto exit;

            }

        } else {

            LEAVE_CS(&m_CS);

        }

        

    }

exit:

    m_dwThreadId = 0;
    ExitThread(0);

}


HRESULT STDMETHODCALLTYPE CWMAXMediaObject::Process( 
    LPCXMEDIAPACKET pSrc,
    LPCXMEDIAPACKET pDst
    )
{
    HRESULT hr = S_OK;
    
    MYCHECK(m_state);

    if ((m_bAsyncMode == FALSE) && (m_hFile == INVALID_HANDLE_VALUE)) {

        ASSERT(m_pCallback);
        ASSERT(m_pCallbackContext);

        //
        // we are in intermediate XMO mode. Dont read from file
        // since the wma codec will call the caller for data
        //

        return Decode(pDst);

    } else {

        MYCHECK(pSrc == NULL);

    }

    if (m_bAsyncMode) {

        PWMAXMO_ASYNC_CONTEXT   pContext;

        ENTER_CS(&m_CS);

        //
        // retrieve a context packet
        //

        if (IsListEmpty(&m_AvailableList)) {

            LEAVE_CS(&m_CS);
            ASSERT(FALSE && " Run out of available packets. Caller called Process too many times in a row");
            return E_OUTOFMEMORY;

        }

        pContext = (PWMAXMO_ASYNC_CONTEXT) RemoveTailList(&m_AvailableList);

        m_dwPendingPackets++;

        DBG_SPAM(("Process: Dequeued available context %x, pendingCount %x\n",
                 pContext, m_dwPendingPackets));


        pContext->pThis = this;

        //
        // save relevant variables into our context since media packets are off the stack
        //

        memcpy(&pContext->Xmp, pDst, sizeof(XMEDIAPACKET));

        if (!IsListEmpty(&m_PendingList)) {

            //
            // queue this packet since one is already being processed..
            //

            InsertTailList(&m_PendingList,
                           &pContext->ListEntry);


            DBG_SPAM(("Process: queuing pending context %x, pendingCount %x\n",
                     pContext, m_dwPendingPackets));


            LEAVE_CS(&m_CS);

            if (pDst->pdwStatus) {
                *pDst->pdwStatus = XMEDIAPACKET_STATUS_PENDING;
            }

            //
            // return pending.
            //

            return S_OK;

        }

        //
        // insert in pending list
        //

        InsertTailList(&m_PendingList,
                       &pContext->ListEntry);

        LEAVE_CS(&m_CS);


        return hr;

    } else {

        hr = ReadWmaDataFromFile((PUCHAR)m_pSrcBuffer,
                                 m_dwLookaheadSize,
                                 NULL);
     
        if (FAILED(hr)) {
     
            ASSERT(!FAILED(hr));
            return hr;
     
        }

        return Decode(pDst);
    }

}

HRESULT 
CWMAXMediaObject::Decode( 
    LPCXMEDIAPACKET pDst
    )
{
    PVOID pDstBuffer;
    HRESULT hr;
    DWORD count = 0;
    DWORD dwXmpStatus = XMEDIAPACKET_STATUS_SUCCESS;

    ASSERT((pDst->dwMaxSize % (2*m_hdr.num_channels)) == 0);
    DWORD num_bytes_requested = pDst->dwMaxSize;
    DWORD num_samples_requested = (num_bytes_requested) / (sizeof(short)* m_hdr.num_channels);
    DWORD num_samples_decoded = 0;
    
    pDstBuffer = pDst->pvBuffer;

    {
        while ( num_samples_decoded < num_samples_requested ) {


            if ( m_bDecodeMore ) {

                m_bDecodeMore = false;
                count++;

                //
                // yield execution so we dont hog the CPU
                //

                if (count%m_dwYieldRate == 0) {

                    DBG_SPAM(("Yielding execution\n"));
                    SwitchToThread();

                }

                tWMAFileStatus rc = WMAFileDecodeData(m_state);

                if (rc != cWMA_NoErr) {

                    if (rc == cWMA_NoMoreFrames)
                    {
                        DBG_ERROR(("  done playing!\n"));
                    }
                    else
                    {
                        DBG_ERROR(("  stopping due to error! (%d)\n", rc));
                        dwXmpStatus = XMEDIAPACKET_STATUS_FAILURE;
                    }
                    break; // End of WMA file, or some error.
                }


            }

            short* pSamples = ((short*) pDstBuffer) + num_samples_decoded * m_hdr.num_channels;
            DWORD numSamples = num_samples_requested - num_samples_decoded;
            
            if ( numSamples > MAX_SAMPLES_PER_REQUEST) {
                numSamples = MAX_SAMPLES_PER_REQUEST;
            }

            DBG_SPAM(("WMAFileGetPCM (this %x)(0x%08x, %d)\n", this, pSamples, numSamples));
            DWORD samples_decoded_this_round = WMAFileGetPCM( m_state,
                pSamples,
                NULL, numSamples);

            DBG_INFO((" ...got %d samples.\n", samples_decoded_this_round));

            if ( samples_decoded_this_round < numSamples ){
                m_bDecodeMore = true;
            }

            num_samples_decoded += samples_decoded_this_round;
        }

    }
    
    DBG_SPAM(("Process: Completing context, pdwStatus %x\n",
             pDst->pdwStatus));

    if (pDst->pdwCompletedSize ) {
        *pDst->pdwCompletedSize = num_samples_decoded * m_hdr.num_channels * sizeof(short);

        if (pDst->dwMaxSize > *pDst->pdwCompletedSize) {
    
            DBG_ERROR(("Wmaxmo (%x): Process: Completing packet %x with %x bytes, asked for %x\n",
                        this,
                        pDst,
                        *pDst->pdwCompletedSize,
                        pDst->dwMaxSize));
    
    
        }

    }
    
    if (pDst->hCompletionEvent) {
        SetEvent(pDst->hCompletionEvent);
    }

    if (pDst->pdwStatus) {
        *pDst->pdwStatus = dwXmpStatus;
    }
    
    return S_OK;
}

// Callback from WMA codec code.

extern "C" tWMA_U32 WMAFileCBGetData(tHWMAFileState state,
   PVOID Context,
   tWMA_U32 offset,
   tWMA_U32 num_bytes,
   unsigned char **ppData);

tWMA_U32 WMAFileCBGetData(tHWMAFileState state,
   PVOID pContext,
   tWMA_U32 offset,
   tWMA_U32 num_bytes,
   unsigned char **ppData)
{
   CWMAXMediaObject* pMO = (CWMAXMediaObject*) pContext;
   MYCHECK(pMO);

   return pMO->GetData(offset, num_bytes, (void**) ppData);
}

tWMA_U32 CWMAXMediaObject::GetData(DWORD offset,
   DWORD num_bytes,
   void** ppData
   )
{
   XMEDIAPACKET xmp;
   HRESULT                 hr      = DS_OK;
   DWORD                   dwRead;

   //
   // if the XMO is instanttiated as a in-memory decoder
   // dialup the caller supplied callback
   //

   if (m_pCallback) {

       ASSERT(m_dwLookaheadSize == 0);
       return (*m_pCallback)(m_pCallbackContext,
                             offset,
                             num_bytes,
                             ppData);

   }

   ENTER_CS(&m_CS);

   ULONG max_bytes = m_dwSrcBufferOffset+m_dwLookaheadSize - offset;

   DBG_SPAM((" CBGetData(%x) offset: 0x%08x-0x%08x size: %d\n", this, offset, offset + num_bytes - 1, num_bytes));
   MYCHECK(ppData);
   
   if (((m_dwSrcBufferOffset+m_dwLookaheadSize) <= offset) ||
       (num_bytes > max_bytes)){

       DBG_WARN(("Wma decoder requested data at an offset further than dwLookaheadSize.Offset %x, current %x ",offset,m_dwSrcBufferOffset));
       return 0;

   }

   *ppData = m_pSrcBuffer + offset - m_dwSrcBufferOffset;

#if DBG
   if (offset < m_dwCurrentWmaFileOffset) {

       DBG_WARN(("WMAXMO(%x): GetData: Wma ask for Offset %x, last offset %x\n",
                this,
                offset,
                m_dwCurrentWmaFileOffset));


   }
#endif

   m_dwCurrentWmaFileOffset = offset;

   LEAVE_CS(&m_CS);

   return num_bytes;
}

HRESULT CWMAXMediaObject::ReadWmaDataFromFile(
    PVOID pvBuffer,
    DWORD dwSize,
    PWMAXMO_ASYNC_CONTEXT   pContext
    )
{
    NTSTATUS                status = STATUS_SUCCESS;
    HRESULT hr = S_OK;
    ULONG Offset;
    BOOL bResult;
    DWORD dwBytesRead;

    Offset = m_dwSrcBufferOffset;

    if ((m_dwCurrentWmaFileOffset-m_dwSrcBufferOffset) < m_dwLookaheadSize/2) {

        //
        // no need to do any file i/o
        //

        DBG_INFO(("ReadFile: no file I/o done\n"));

    } else {

        m_dwSrcBufferOffset = m_dwCurrentWmaFileOffset;        
        if (Offset) {
            ASSERT(m_dwSrcBufferOffset);
        }

    }

    if (Offset == m_dwSrcBufferOffset) {

        //
        // if offset did not change dont bother reading
        //


    } else {

        DBG_INFO((" CBGetData Reading ahead, offset %x, wma offset %x\n",
                  m_dwSrcBufferOffset,
                  m_dwCurrentWmaFileOffset));
    
        SetFilePointer(m_hFile, m_dwSrcBufferOffset, NULL, FILE_BEGIN);

        DBG_SPAM(("ReadFile(%x): calling readfile with context %x, pendingCount %x, listempty %d\n",
                 this, pContext, m_dwPendingPackets, IsListEmpty(&m_PendingList) ));
    
        m_overlapped.Offset = m_dwSrcBufferOffset;

        bResult = ReadFile(m_hFile,
                     pvBuffer,
                     dwSize,
                     &dwBytesRead,
                     (pContext ? &m_overlapped : NULL));

        if (!pContext && !bResult) {

            hr = HRESULT_FROM_WIN32(GetLastError());

            DBG_ERROR(("Wmaxmo::readfile: Read failed with %x\n",
                      hr));
                      
            return hr;

        }
     
    }

    if (pContext) {

        //
        // wait for overlapped operation
        //

        bResult = GetOverlappedResult(m_hFile,&m_overlapped,&dwBytesRead,TRUE);
        if (!bResult) {

            return HRESULT_FROM_WIN32(GetLastError());

        }

        //
        // decode wma data
        //
    
        hr = Decode(&pContext->Xmp);
    
        //
        // remove from pending list
        //
    
        RemoveEntryList(&pContext->ListEntry);
    
        //
        // return to available list
        //
    
        InsertHeadList(&m_AvailableList,
                       &pContext->ListEntry);
    
        m_dwPendingPackets--;
    
        DBG_SPAM(("WmaXMO::ReadFile(%x): completing context %x, pendingCount %x, listempty %d\n",
                 this, pContext, m_dwPendingPackets, IsListEmpty(&m_PendingList) ));
    
    
    }

    return HRESULT_FROM_NT(status);

}

HRESULT STDMETHODCALLTYPE CWMAXMediaObject::GetInfo( LPXMEDIAINFO pInfo )
{

    ASSERT(pInfo);

    pInfo->dwMaxLookahead = 0;
    pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE | 
        XMO_STREAMF_FIXED_PACKET_ALIGNMENT;

    //
    // we are decoder: GetOutput gives the minimum buffer we need to decode a 
    // block of wma data.
    //
    
    // No real minimum, the codec has its own internal buffer. But it likes to
    // decode in MAX_SAMPLES_PER_REQUEST sample chunks

    pInfo->dwOutputSize = m_hdr.num_channels * sizeof(short) * MAX_SAMPLES_PER_REQUEST; 
    return S_OK;
}
                                                   
HRESULT STDMETHODCALLTYPE CWMAXMediaObject::Discontinuity(void){
   return S_OK;
}

HRESULT STDMETHODCALLTYPE CWMAXMediaObject::GetStatus( 
    LPDWORD pdwStatus
    )
{

    if (pdwStatus) {

        *pdwStatus = XMO_STATUSF_ACCEPT_OUTPUT_DATA;

        if (m_bAsyncMode) {
    
            //
            // if we have no available contexts to queue packets,
            // tell caller to not submit anymore...
            //

            ENTER_CS(&m_CS);
            if (IsListEmpty(&m_AvailableList)) {
    
                *pdwStatus = 0;

            }
            LEAVE_CS(&m_CS);

        }

    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CWMAXMediaObject::GetFileContentDescription(WMAXMOFileContDesc* pContentDesc)
{
   HRESULT hr = S_OK;
   tWMA_U32 WmaErr = cWMA_NoErr;

   WmaErr = WMAFileContentDesc(m_state, (tWMAFileContDesc *) pContentDesc);
   if (WmaErr != cWMA_NoErr) {

       DBG_ERROR(("WMAFileContentDesc failed with 0x%x\n",WmaErr));
       return E_FAIL;
   }

   return hr;
}

HRESULT STDMETHODCALLTYPE CWMAXMediaObject::GetFormat(WAVEFORMATEX* pWave)
{
   pWave->wFormatTag = WAVE_FORMAT_PCM;
   pWave->nSamplesPerSec = WMASampleRateToHz(m_hdr.sample_rate);
   pWave->nChannels = m_hdr.num_channels;
   pWave->wBitsPerSample = 16;
   pWave->nBlockAlign = ((pWave->wBitsPerSample + 7) / 8) * pWave->nChannels;
   pWave->nAvgBytesPerSec = pWave->nBlockAlign * pWave->nSamplesPerSec;
   pWave->cbSize = 0;
   
   return S_OK;
}

HRESULT CWMAXMediaObject::Seek(LONG lOffset, DWORD dwOrigin, LPDWORD pdwAbsolute)
{
    HRESULT                 hr          = DS_OK;
    NTSTATUS                status;

    ASSERT(m_pCallback == NULL);

    ASSERT((lOffset == 0) && (dwOrigin == FILE_CURRENT) && "WmaXmo only supports querying current position");
    
    if(pdwAbsolute)
    {
        *pdwAbsolute = m_dwCurrentWmaFileOffset;
    }

    return hr;
}

HRESULT CWMAXMediaObject::GetLength(LPDWORD pdwLength)
{
    HRESULT                 hr  = DS_OK;
    
    //
    // you can not query file pointer if we are in-memory decoder
    //


    ASSERT(m_pCallback == NULL);

    if((*pdwLength = GetFileSize(m_hFile, NULL)) == -1)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
}

__inline DWORD CWMAXMediaObject::WMASampleRateToHz(tWMAFileSampleRate sampleRate)
{

   UINT nSampleRate;
   switch (sampleRate)
   {
   case cWMA_SR_08kHz:
       nSampleRate = 8000;
       break;
       
   case cWMA_SR_11_025kHz:
       nSampleRate = 11025;
       break;
       
   case cWMA_SR_16kHz:
       nSampleRate = 16000;
       break;
       
   case cWMA_SR_22_05kHz:
       nSampleRate = 22050;
       break;
       
   case cWMA_SR_32kHz:
       nSampleRate = 32000;
       break;
       
   case cWMA_SR_44_1kHz:
       nSampleRate = 44100;
       break;
       
   case cWMA_SR_48kHz:
       nSampleRate = 48000;
       break;
       
   default:
       DBG_ERROR(("Unknown sample rate: %d\n", m_hdr.sample_rate));

       MYCHECK(0);
       nSampleRate = 0;
       break;
   }
   return nSampleRate;
}

