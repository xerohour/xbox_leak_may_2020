#include "netxmo.h"
#include "netxmop.h"

ULONG g_NetXmoDebugLevel;

HRESULT
XnetCreateMediaObject(
    XMediaObject **ppMediaObject,
    SOCKET Socket,
    const char * pszAddress,
    DWORD dwPort,
    DWORD dwMaxPacketSize,
    DWORD dwFlags
    )
{   
    HRESULT hr;
    CNetXMO *pNetXmo;

    //
    // create net xmo
    //

    pNetXmo = new CNetXMO();
    ASSERT(pNetXmo);
    if (pNetXmo == NULL) {
        return E_OUTOFMEMORY;
    }

    hr = pNetXmo->Initialize(Socket,
                             (PUCHAR)pszAddress,
                             dwPort,
                             dwMaxPacketSize,
                             dwFlags);


    *ppMediaObject = NULL;

    if (SUCCEEDED(hr)) {
        *ppMediaObject = pNetXmo;
        pNetXmo->AddRef();
    }

    return hr;

}

HRESULT CNetXMO::Initialize(
    SOCKET ExternalSocket,
    PUCHAR pszAddress,
    DWORD dwPort,
    DWORD dwMaxPacketSize,
    DWORD dwFlags
    )
{
    SOCKET sock = NULL;
    INT err, size;
    DWORD i;
    PXMB_CONTEXT pCtx;

    if (ExternalSocket == INVALID_SOCKET) {

        //
        // use unreliable but with small latency UDP
        //
    
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == INVALID_SOCKET) {
            CALLFAILED(socket,GetLastError());
            return E_FAIL;
        }
    
        //
        // create the local socket
        //
    
        m_SocketName.sin_family = AF_INET;
        m_SocketName.sin_port = htons((USHORT)dwPort);    
        m_SocketName.sin_addr.s_addr  = INADDR_ANY;
    
        err = bind(sock,
                   (struct sockaddr*)&m_SocketName,
                   sizeof(m_SocketName));

        if (err != NO_ERROR) {
            DbgPrint("Net xmo %x failed bind with err = %x\n",
                     this,
                     err);

            goto exit;
        }
    
        //
        // create a remote address
        //
    
        m_RemoteSocketName.sin_family = AF_INET;
        m_RemoteSocketName.sin_port = htons((USHORT)dwPort);    
        m_RemoteSocketName.sin_addr.s_addr = inet_addr((const char *)pszAddress);
    
        //
        // connect local socket to remote address
        //
    
        err = connect(sock,
                      (struct sockaddr*)&m_RemoteSocketName,
                      sizeof(m_RemoteSocketName));
    
        if (err != NO_ERROR) {
            DbgPrint("Net xmo %x failed connect with err = %x\n",
                     this,
                     err);
            goto exit;
        }
        
        DbgPrint("Net xmo %x connecting to: %u.%u.%u.%u:%d\n",
                 this,
                 m_RemoteSocketName.sin_addr.S_un.S_un_b.s_b1, m_RemoteSocketName.sin_addr.S_un.S_un_b.s_b2, m_RemoteSocketName.sin_addr.S_un.S_un_b.s_b3, m_RemoteSocketName.sin_addr.S_un.S_un_b.s_b4,
                 ntohs(m_RemoteSocketName.sin_port));
        
        size = sizeof(m_SocketName);

        err = getsockname(sock,
                          (struct sockaddr*) &m_SocketName,
                          &size);

        if (err != NO_ERROR) {
            DbgPrint("Net xmo %x failed getsockname with err = %x\n",
                     this,
                     err);

            goto exit;
        }
    
        DbgPrint("Net xmo %x local address: %u.%u.%u.%u:%d\n",
                 this,
                 m_SocketName.sin_addr.S_un.S_un_b.s_b1, m_SocketName.sin_addr.S_un.S_un_b.s_b2, m_SocketName.sin_addr.S_un.S_un_b.s_b3, m_SocketName.sin_addr.S_un.S_un_b.s_b4,
                 ntohs(m_SocketName.sin_port));

        m_Socket = sock;

        err = ioctlsocket(m_Socket,
                          FIONBIO,
                          &m_cRef);

        if (err!=NO_ERROR) {

            DbgPrint("Net xmo %x failed ioctlsocket with err = %x\n",
                     this,
                     err);

            //
            // this is non fatal
            //

        }

    } else {

        DbgPrint("Net xmo %x using supplied socket: %x\n",
                 this,
                 ExternalSocket);

        m_Socket = ExternalSocket;

    }

    InitializeListHead(&m_AvailableList);
    InitializeListHead(&m_PendingList);

    //
    // prime our available list with contexts
    //

    for (i=0;i<MAX_NETXMO_BUFFER_COUNT;i++) {

        pCtx = new XMB_CONTEXT;
        ASSERT(pCtx);
        if (pCtx == NULL) {
            err = E_OUTOFMEMORY;
            goto exit;
        }

        InsertTailList(&m_AvailableList,&pCtx->ListEntry);

    }

    //
    // allocate double buffer for receive and send
    // we only need one because we are sync
    //

    m_pRecvBuffer = (PUCHAR) new BYTE[dwMaxPacketSize+sizeof(NETXMOPACKETHEADER)];

    ASSERT(m_pRecvBuffer);

    if (m_pRecvBuffer == NULL) {
        return E_OUTOFMEMORY;
    }

    m_pSendBuffer = (PUCHAR) new BYTE[dwMaxPacketSize+sizeof(NETXMOPACKETHEADER)];
    ASSERT(m_pSendBuffer);

    if (m_pSendBuffer == NULL) {
        return E_OUTOFMEMORY;
    }

    m_dwFlags = dwFlags;

    //
    // initialize synchronization object
    //

    m_hMutex = CreateMutex(NULL,
                FALSE,
                NULL);

    if (m_hMutex == NULL) {

        DbgPrint("Netxmo: Thread creation failed..");
        goto exit;


    }

    //
    // we are initialized ok. Now create a thread that will gives us
    // some context to run and check if the net has completed packets
    //

    m_hThread = CreateThread(NULL,
                             4096,
                             NetXmoThreadProc,
                             this,
                             0,
                             &m_dwThreadId);

    if (m_hThread == NULL) {

        DbgPrint("Netxmo: Thread creation failed..");
        goto exit;

    }

    //
    // create event that will signal thread to terminate
    //

    m_hDeleteThreadEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

    if (m_hDeleteThreadEvent == NULL) {

        DbgPrint("Netxmo: Thread creation failed..");
        goto exit;

    }

    //
    // single event to track the outstanding recv packet request
    //

    m_hRecvPacketEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

    if (m_hRecvPacketEvent == NULL) {

        DbgPrint("Netxmo: Thread creation failed..");
        goto exit;

    }

    return S_OK;

exit:
    if (sock) {
        closesocket(sock);
    }
    
    WSACleanup();

    return E_FAIL;


}


STDMETHODIMP_(ULONG) CNetXMO::Release()
{

   long l = InterlockedDecrement((long*)&m_cRef);

   if (l == 0)
      delete this;
   return l;
}

CNetXMO::~CNetXMO()
{

    PXMB_CONTEXT pCtx;

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

    //
    // cleanup
    //

    //
    // close the socket. This will cancel all pending requests
    // but it will NOT trigger notifications...
    //

    closesocket(m_Socket);

    //
    // if anything pending, complete it
    //

    if (!IsListEmpty(&m_PendingList)) {
    
        pCtx = (PXMB_CONTEXT) m_PendingList.Flink;

        while ((PLIST_ENTRY)pCtx != &m_PendingList) {

            if (pCtx->Xmb.pdwCompletedSize) {
                *pCtx->Xmb.pdwCompletedSize = 0;
            }
            
            if (pCtx->Xmb.pdwStatus) {
                *pCtx->Xmb.pdwStatus = XMEDIAPACKET_STATUS_FLUSHED;
            }
            
            if (pCtx->Xmb.hCompletionEvent) {
                SetEvent(pCtx->Xmb.hCompletionEvent);
            }

            NX_DBG_ERROR(("NetXMO::ThreadProc: Cleaning up pending context %x\n",pCtx));;

            ReturnContext(pCtx);

            pCtx = (PXMB_CONTEXT) m_PendingList.Flink;

        }

    }

    //
    // free all available list contexts
    //

    if (!IsListEmpty(&m_AvailableList)) {

        pCtx = (PXMB_CONTEXT) m_AvailableList.Flink;

        while ((PLIST_ENTRY)pCtx != &m_AvailableList) {

            RemoveEntryList(&pCtx->ListEntry);
            delete pCtx;
            pCtx = (PXMB_CONTEXT) m_AvailableList.Flink;

             if ((PLIST_ENTRY)pCtx == &m_AvailableList) {
                 break;
             }

        }

    }

    if (m_pRecvBuffer) {
        delete [] m_pRecvBuffer;
    }
    
    if (m_pSendBuffer) {
        delete [] m_pSendBuffer;
    }
    
    if (m_hMutex) {

        CloseHandle(m_hMutex);

    }
 
    if (m_hDeleteThreadEvent) {
        CloseHandle(m_hDeleteThreadEvent);
    }

    if (m_hRecvPacketEvent) {
        CloseHandle(m_hRecvPacketEvent);
    }
    

}


HRESULT STDMETHODCALLTYPE CNetXMO::GetInfo( 
    PXMEDIAINFO pInfo
    )
{
    ASSERT(pInfo);

    pInfo->dwFlags = XMO_STREAMF_INPUT_ASYNC;

    pInfo->dwInputSize = 1;
    pInfo->dwOutputSize = 1;

    return S_OK;

}


HRESULT STDMETHODCALLTYPE CNetXMO::Process( 
    const XMEDIAPACKET __RPC_FAR *pSrcBuffer,
    const XMEDIAPACKET __RPC_FAR *pDstBuffer
    )
{

    DWORD dwSize;
    PNETXMOPACKETHEADER pHeader;
    PXMB_CONTEXT pCtx;
    HRESULT hr = NOERROR;

    if (pSrcBuffer) {

        ASSERT ( pSrcBuffer->dwMaxSize <= MAX_NETXMO_TRANSFER_SIZE);

        //
        // they want to send data..
        //

        pHeader = (PNETXMOPACKETHEADER) m_pSendBuffer;

        memcpy(m_pSendBuffer+sizeof(NETXMOPACKETHEADER),
               pSrcBuffer->pvBuffer,
               pSrcBuffer->dwMaxSize);

        pHeader->bType = NETXMO_PACKET_TYPE_AUDIO;
        pHeader->bSeqNum = m_bSeqNum++;
        pHeader->wSize = (WORD)pSrcBuffer->dwMaxSize;

        dwSize = pSrcBuffer->dwMaxSize + sizeof(NETXMOPACKETHEADER);

        dwSize = send(m_Socket,
             (const char *)pHeader,
             dwSize,
             0);

        if (dwSize == SOCKET_ERROR) {

            hr = E_FAIL;
            CALLFAILED(send,hr);
            goto processRecv;

        } 

        if (pSrcBuffer->pdwCompletedSize) {

            *pSrcBuffer->pdwCompletedSize = SUCCEEDED(hr) ? pSrcBuffer->dwMaxSize : 0;

        }

        if (pSrcBuffer->pdwStatus) {

            *pSrcBuffer->pdwStatus = SUCCEEDED(hr) ? XMEDIAPACKET_STATUS_SUCCESS : XMEDIAPACKET_STATUS_FAILURE;

        }

        if (pSrcBuffer->hCompletionEvent) {

            SetEvent(pSrcBuffer->hCompletionEvent);

        }

    }

processRecv:

    if (pDstBuffer) {

        //
        // receive data..
        //

        ASSERT ( pDstBuffer->dwMaxSize <= MAX_NETXMO_TRANSFER_SIZE);

        //
        // Set initial packet status to pending
        //

        if (pDstBuffer->pdwStatus) {
            *pDstBuffer->pdwStatus = XMEDIAPACKET_STATUS_PENDING;
        }
        
        //
        // grab mutex to sync with thread
        //

        WaitForSingleObject(m_hMutex,INFINITE);

        pCtx = (PXMB_CONTEXT) RemoveTailList(&m_AvailableList);
        ASSERT(pCtx);

        memcpy(&pCtx->Xmb, pDstBuffer,sizeof(XMEDIAPACKET));

        InsertTailList(&m_PendingList,&pCtx->ListEntry);

        ReleaseMutex(m_hMutex);

        return S_OK;

    }

    return S_OK;

}


DWORD WINAPI NetXmoThreadProc(
    LPVOID pParameter
    )
{
    CNetXMO *pThis = (CNetXMO *) pParameter;
    pThis->ThreadProc();

    return 0;
}

VOID CNetXMO::ThreadProc()
{
    HRESULT hr = S_OK;
    HANDLE hArray[2];
    DWORD eventIndex;
    PXMB_CONTEXT pCtx;

    while (m_cRef > 0) {

        //
        // this function should have been called in response to an event being signalled
        // check if we have any pending recv buffers
        //
    
        //
        // grab a mutex
        //

        hArray[0] = m_hDeleteThreadEvent;
        hArray[1] = m_hMutex;

        eventIndex = WaitForMultipleObjects(2,
                                            hArray,
                                            FALSE,
                                            INFINITE);


        if ((eventIndex - WAIT_OBJECT_0) == 0) {

            //
            // thread needs to be deleted, bail...
            //

            NX_DBG_INFO(("ThreadProc: Leaving thread\n"));

            ReleaseMutex(m_hMutex);
            goto exit;

        }

        if (!IsListEmpty(&m_PendingList)) {
    
            pCtx = (PXMB_CONTEXT) m_PendingList.Flink;

            //
            // ask xnet to see if it has data for us
            //

            NX_DBG_INFO(("ThreadProc: Submiting context %x to XNET, overlapped\n",pCtx));
            hr = SubmitRecvRequest(&pCtx->Xmb,pCtx);

            ReleaseMutex(m_hMutex);

            if ((hr == E_PENDING) || (hr == NOERROR)) {

                if ((hr == E_PENDING)) {

                    //
                    // now wait on the deletion and packet event
                    //
    
                    hArray[0] = m_hDeleteThreadEvent;                
                    hArray[1] = m_hRecvPacketEvent;
    
                    eventIndex = WaitForMultipleObjects(2,
                                                        hArray,
                                                        FALSE,
                                                        INFINITE);
    
    
                    if ((eventIndex - WAIT_OBJECT_0) == 0) {
    
                        //
                        // thread needs to be deleted, bail...
                        // 
    
                        NX_DBG_INFO(("ThreadProc: Leaving thread\n"));
    
                        goto exit;
    
                    }

                }

                //
                // set caller parameters, extract header info
                //
                    
                NX_DBG_INFO(("ThreadProc: Handling completed context %x\n",pCtx));
                HandleRecvCompletion(&pCtx->Xmb);
    
                ReturnContext(pCtx);

            } else {

                //
                // error occured
                //

                if (pCtx->Xmb.pdwCompletedSize) {
                    *pCtx->Xmb.pdwCompletedSize = 0;
                }
                
                if (pCtx->Xmb.pdwStatus) {
                    *pCtx->Xmb.pdwStatus = XMEDIAPACKET_STATUS_FAILURE;
                }
                
                if (pCtx->Xmb.hCompletionEvent) {
                    SetEvent(pCtx->Xmb.hCompletionEvent);
                }

                NX_DBG_ERROR(("NetXMO::Process: SubmitRecvRequest failed with %x\n",hr));;

                //
                // return context to available list
                //
    
                ReturnContext(pCtx);

            }

        } else { // if listempty

            ReleaseMutex(m_hMutex);

        }

    }

exit:

    m_dwThreadId = 0;
    ExitThread(0);

}

HRESULT STDMETHODCALLTYPE CNetXMO::GetStatus( 
        /* [out] */ DWORD __RPC_FAR *pdwFlags
        )
{

    //
    // let the caller know if we can accept any more data
    //

    *pdwFlags = XMO_STATUSF_ACCEPT_INPUT_DATA;

    if(!IsListEmpty(&m_AvailableList))
    {
        *pdwFlags |= XMO_STATUSF_ACCEPT_OUTPUT_DATA;
    }

    return S_OK;

}

HRESULT
CNetXMO::ReturnContext(
    PXMB_CONTEXT pCtx
    )
{

    HANDLE hArray[2];
    DWORD eventIndex;

    //
    // grab a mutex (assume the thread does not own it already)
    //

    hArray[0] = m_hDeleteThreadEvent;
    hArray[1] = m_hMutex;

    eventIndex = WaitForMultipleObjects(2,
                                        hArray,
                                        FALSE,
                                        INFINITE);

    if ((eventIndex - WAIT_OBJECT_0) == 0) {

        //
        // thread needs to be deleted, bail...
        //

        NX_DBG_INFO(("ThreadProc: We have been deleted, Leaving thread\n"));

        ReleaseMutex(m_hMutex);
        return E_FAIL;

    }

    //
    // return context to available list
    //

    RemoveEntryList(&pCtx->ListEntry);
    InsertTailList(&m_AvailableList,&pCtx->ListEntry);

    ReleaseMutex(m_hMutex);
    return NOERROR;

}

HRESULT
CNetXMO::SubmitRecvRequest(
    const XMEDIAPACKET *pDstBuffer,
    PXMB_CONTEXT pCtx
    )

{
    INT err;
    HRESULT hr;
    WSABUF wsaBuf;
    
    DWORD dwFlags = 0, dwSize;

    dwSize = pDstBuffer->dwMaxSize + sizeof(NETXMOPACKETHEADER);

    wsaBuf.len = dwSize;
    wsaBuf.buf = (PCHAR) m_pRecvBuffer;

    //
    // NULL the header
    //

    *((PULONG)m_pRecvBuffer) = 0;

    m_overlapped.hEvent = m_hRecvPacketEvent;

    err = WSARecv(m_Socket,
            &wsaBuf,
            1,
            &dwSize,
            &dwFlags,
            &m_overlapped,
            NULL);

    if (err) {
        err = WSAGetLastError();
    }

    NX_DBG_SPAM(("SubmitRecvRequest: WSARecv status %x\n",err));

    //
    // convert to COM error
    //
    
    if (err == WSA_IO_PENDING) {
        hr = E_PENDING;
    } else if (err) {
        hr = E_FAIL;
    } else {
        // no error
        hr = S_OK;
    }

    NX_DBG_SPAM(("SubmitRecvRequest: WSARecv COM status %x\n",hr));

    return hr;
}

VOID
CNetXMO::HandleRecvCompletion(
    const XMEDIAPACKET *pDstBuffer
    )
{

    PNETXMOPACKETHEADER pHeader;
    DWORD dwSize;

    //
    // event if there was one
    // has been signalled by XNET.
    // Extract our header, set XMB fields
    //

    pHeader = (PNETXMOPACKETHEADER) m_pRecvBuffer;

    ASSERT(pDstBuffer->dwMaxSize >= pHeader->wSize);
    dwSize = min(pDstBuffer->dwMaxSize, pHeader->wSize);

    memcpy((PUCHAR)pDstBuffer->pvBuffer,
           m_pRecvBuffer+sizeof(NETXMOPACKETHEADER),
           dwSize);

    if(pHeader->bSeqNum != m_bSeqNum) {

        NX_DBG_SPAM(("Out of sequence packet. Seq %d, expected %d\n",
                 pHeader->bSeqNum,
                 m_bSeqNum));

        m_bSeqNum = pHeader->bSeqNum;

        //
        // in here, you could possibly implement re-ordering
        //

    }

    m_bSeqNum++;

    if (pDstBuffer->pdwCompletedSize) {

        *pDstBuffer->pdwCompletedSize = dwSize;

    }
    
    if (pDstBuffer->pdwStatus) {

        *pDstBuffer->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;

    }

    if (pDstBuffer->hCompletionEvent) {

        SetEvent(pDstBuffer->hCompletionEvent);

    }

}


