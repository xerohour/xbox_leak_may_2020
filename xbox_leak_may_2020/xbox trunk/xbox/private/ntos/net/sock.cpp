// ---------------------------------------------------------------------------------------
// sock.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

#pragma warning(disable:4102)

// ---------------------------------------------------------------------------------------
// Trace tags
// ---------------------------------------------------------------------------------------

DefineTag(sock,     0);
DefineTag(sockWarn, TAG_ENABLE);

// ---------------------------------------------------------------------------------------
// socket
// ---------------------------------------------------------------------------------------

SOCKET CXnSock::socket(IN int af, IN int type, IN int protocol)
{
    WinsockApiProlog_(socket, INVALID_SOCKET);

    ICHECK(SOCK, USER);

    CSocket * pSocket;

    if (af != 0 && af != AF_INET)
        err = WSAEAFNOSUPPORT;
    else if (type != 0 && type != SOCK_STREAM && type != SOCK_DGRAM)
        err = WSAESOCKTNOSUPPORT;
    else if (protocol != 0 && protocol != IPPROTO_TCP && protocol != IPPROTO_UDP)
        err = WSAEPROTONOSUPPORT;
    else
    {
        if (type == 0)
            type = (protocol == IPPROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM;
        if (protocol == 0)
            protocol = (type == SOCK_DGRAM) ? IPPROTO_UDP : IPPROTO_TCP;
        if ((type == SOCK_STREAM) != (protocol == IPPROTO_TCP))
            err = WSAEPROTONOSUPPORT;
    }

    WinsockApiCheckError_(INVALID_SOCKET);

    pSocket = SockAlloc(type == SOCK_STREAM, FALSE);

    if (pSocket == NULL)
    {
        WinsockApiReturnError_(WSAENOBUFS, INVALID_SOCKET);
    }

    RaiseToDpc();
    pSocket->SetFlags(SOCKF_OWNED);
    pSocket->Enqueue(&_leSockets);

    return((SOCKET)pSocket);
}

CSocket * CXnSock::SockAlloc(BOOL fTcp, BOOL fPoolAlloc)
{
    ICHECK(SOCK, USER|UDPC|SDPC);

    CSocket *   pSocket;
    UINT        cb  = fTcp ? sizeof(CTcpSocket) : sizeof(CSocket);
    ULONG       tag = fTcp ? PTAG_CTcpSocket : PTAG_CSocket;

    if (_cSockets >= cfgSockMaxSockets)
    {
        TraceSz(Warning, "Too many sockets");
        return NULL;
    }

    if (fPoolAlloc)
        pSocket = (CSocket *)PoolAllocZ(cb, tag);
    else
        pSocket = (CSocket *)SysAllocZ(cb, tag);

    if (pSocket)
    {
        pSocket->Init(this, fTcp, fPoolAlloc);

        if (fTcp)
        {
            ((CTcpSocket *)pSocket)->TcpInit(this);
        }

        InterlockedIncrement((LONG *)&_cSockets);
    }

    return(pSocket);
}

// ---------------------------------------------------------------------------------------
// closesocket
// ---------------------------------------------------------------------------------------

int CXnSock::closesocket(IN SOCKET s)
{
    WinsockApiPrologSockLock_(closesocket, SOCKET_ERROR);

    RaiseToDpc();

    if (pSocket == NULL)
        err = NETERR_PARAM;
    else if (pSocket->TestFlags(SOCKF_CLOSED))
    {
        SockFree(pSocket);
        err = 0;
    }
    else
    {
        pSocket->ClearFlags(SOCKF_OWNED);
        err = SockClose(pSocket, FALSE);
    }

    MapNtStatusToWinsockError_(err);

    // Don't call SockUnlock here because the socket is most likely deallocated by now

    WinsockApiCheckError_(SOCKET_ERROR);
    return NO_ERROR;
}

NTSTATUS CXnSock::SockClose(CSocket * pSocket, BOOL fForce)
{
    RaiseToDpc();

    // If a TCP is being gracefully closed, then we'll
    // leave the the CSocket structure in the global list
    // until the connection is really gone.
    if (pSocket->IsTcp() && !TcpClose((CTcpSocket *)pSocket, fForce))
    {
        pSocket->SetFlags(SOCKF_NOMORE_XMIT|SOCKF_NOMORE_RECV|SOCKF_LINGERING);
        pSocket->SetClosed();
        return(NETERR_OK);
    }

    pSocket->Dequeue();

    // Clean up the information associated with the CSocket
    SockCleanup(pSocket);

    Assert(!pSocket->IsTcp() || !((CTcpSocket *)pSocket)->GetTimer()->IsActive());

    EvtTerm(pSocket->GetEvent());

    InterlockedDecrement((LONG *)&_cSockets);

    if (pSocket->TestFlags(SOCKF_OWNED))
        pSocket->SetFlags(SOCKF_CLOSED);
    else
        SockFree(pSocket);

    return(NETERR_OK);
}

void CXnSock::SockFree(CSocket * pSocket)
{
    // Mark socket as closed even though we are freeing the memory just in case the
    // user tries to access it anyways.

    pSocket->SetClosed();

    if (pSocket->TestFlags(SOCKF_POOLALLOC))
        PoolFree(pSocket);
    else
        SysFree(pSocket);
}

void CXnSock::SockCleanup(CSocket * pSocket)
{
    ICHECK(SOCK, UDPC|SDPC);

    NTSTATUS status;

    status = NT_SUCCESS(pSocket->GetStatus()) ? NETERR_CANCELLED : pSocket->GetStatus();

    SockReqComplete(pSocket, pSocket->GetRecvReq(), status);
    SockReqComplete(pSocket, pSocket->GetSendReq(), status);

    // Flush receive buffers

    SockFlushRecvBuffers(pSocket);

    // Flush send buffers

    while (pSocket->HasSendBuf())
    {
        CSendBuf * pSendBuf = pSocket->DequeueSendBuf();
        SockReleaseSendBuf(pSendBuf);
    }

    pSocket->SetCbSendBuf(0);

    if (pSocket->_prte)
    {
        RouteRelease(pSocket->_prte);
        pSocket->_prte = NULL;
    }
}

void CXnSock::SockReleaseSendBuf(CSendBuf * pSendBuf)
{
    if (pSendBuf->Release() == 0)
        PacketFree(pSendBuf);
    else
        pSendBuf->SetPfn((PFNPKTFREE)PacketFree);
}

//
// Flush a socket's receive buffers
//
void CXnSock::SockFlushRecvBuffers(CSocket * pSocket)
{
    while (pSocket->HasRecvBuf())
    {
        PoolFree(pSocket->DequeueRecvBuf());
    }

    pSocket->SetCbRecvBuf(0);
}

// ---------------------------------------------------------------------------------------
// shutdown
// ---------------------------------------------------------------------------------------

int CXnSock::shutdown(IN SOCKET s, IN int how)
{
    DWORD dwFlags;
    WinsockApiPrologSockLock_(shutdown, SOCKET_ERROR);

    WinsockApiParamCheck_(
        how == SD_SEND ||
        how == SD_RECEIVE ||
        how == SD_BOTH);

    switch (how) {
    case SD_SEND:
        dwFlags = SOCKF_NOMORE_XMIT;
        break;

    case SD_RECEIVE:
        dwFlags = SOCKF_NOMORE_RECV;
        break;

    default:
        dwFlags = SOCKF_NOMORE_XMIT|SOCKF_NOMORE_RECV;
        break;
    }

    if (pSocket->IsUdp())
    {
        err = UdpShutdown(pSocket, dwFlags);
    }
    else
    {
        if (!pSocket->TestFlags(SOCKF_CONNECTED))
        {
            WinsockApiGotoExit_(WSAENOTCONN);
        }
        err = TcpShutdown((CTcpSocket*) pSocket, dwFlags, TRUE);
    }

    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// ioctlsocket
// ---------------------------------------------------------------------------------------

int CXnSock::ioctlsocket(SOCKET s, long cmd, u_long* argp)
{
    KIRQL irql;
    UINT bytesReady;

    WinsockApiPrologSockLock_(ioctlsocket, SOCKET_ERROR);
    WinsockApiParamCheck_(argp != NULL);

    switch (cmd) {
    case FIONBIO:
        pSocket->SetFlags((*argp) ? SOCKF_OPT_NONBLOCKING : 0, SOCKF_OPT_NONBLOCKING);
        break;

    case FIONREAD:
    {
        RaiseToDpc();

        if (pSocket->IsUdp())
        {
            // For UDP sockets, return the size of the first buffered datagram
            // NOTE: For compatibility with win2k behavior, if the first datagram
            // is 0-sized, we'll return 1.
            if (pSocket->IsUdpRecvBufEmpty())
            {
                bytesReady = 0;
            }
            else
            {
                bytesReady = ((CUdpRecvBuf *)pSocket->GetRecvBufFirst())->GetCbBuf();
                if (bytesReady == 0)
                    bytesReady = 1;
            }
        }
        else
        {
            // For TCP sockets, return the total number of bytes
            // available for reading.
            bytesReady = pSocket->GetCbRecvBuf();
        }
        *argp = bytesReady;
        break;
    }
    
    // case SIOCATMARK:
    default:
        WinsockApiGotoExit_(WSAENOPROTOOPT);
        break;
    }

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// setsockopt
// ---------------------------------------------------------------------------------------

int CXnSock::setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen)
{
    INT val;
    WinsockApiPrologSockLock_(setsockopt, SOCKET_ERROR);
    WinsockApiParamCheck_(optval != NULL && optlen > 0);

    if (optlen < (INT) sizeof(INT))
        val = (UCHAR) *optval;
    else
        val = *((INT*) optval);

    switch (level)
    {
        case SOL_SOCKET:
        {
            RaiseToDpc();

            // NOTE: Some options may not be applicable to datagram sockets,
            // while others may not be applicable to stream sockets. But
            // we'll let apps set those options here anyway (to avoid extra
            // checking code). Downstream code just won't use those 

            switch (optname)
            {
                case SO_BROADCAST:
                    if (pSocket->IsTcp()) goto noopt;
                    pSocket->SetFlags(val ? SOCKF_OPT_BROADCAST : 0, SOCKF_OPT_BROADCAST);
                    break;

                case SO_DONTLINGER:
                    if (pSocket->IsUdp()) goto noopt;
                    ((CTcpSocket *)pSocket)->SetLingerOnOff(!val);
                    break;

                case SO_LINGER:
                    if (pSocket->IsUdp()) goto noopt;
                    WinsockApiParamCheck_(optlen >= sizeof(LINGER));
                    ((CTcpSocket *)pSocket)->SetLinger((LINGER *)optval);
                    break;

                case SO_REUSEADDR:
                    if (pSocket->TestFlags(SOCKF_OPT_REUSEADDR) && val != 0) goto inval;
                    pSocket->SetFlags(val? SOCKF_OPT_REUSEADDR : 0, SOCKF_OPT_REUSEADDR);
                    break;

                case SO_EXCLUSIVEADDRUSE:
                    if (pSocket->TestFlags(SOCKF_OPT_EXCLUSIVEADDR) && val != 0) goto inval;
                    pSocket->SetFlags(val ? SOCKF_OPT_EXCLUSIVEADDR : 0, SOCKF_OPT_EXCLUSIVEADDR);
                    break;

                case SO_RCVTIMEO:
                    // Timeout value is in milliseconds
                    WinsockApiParamCheck_(optlen >= sizeof(INT));
                    pSocket->_uiRecvTimeout = val;
                    break;

                case SO_SNDTIMEO:
                    // Timeout value is in milliseconds
                    WinsockApiParamCheck_(optlen >= sizeof(INT));
                    pSocket->_uiSendTimeout = val;
                    break;

                case SO_RCVBUF:
                    WinsockApiParamCheck_(optlen >= sizeof(INT));
                    err = SockUpdateBufferSize(pSocket, pSocket->_cbMaxSendBuf, val);
                    if (!NT_SUCCESS(err)) goto inval;
                    break;

                case SO_SNDBUF:
                    WinsockApiParamCheck_(optlen >= sizeof(INT));
                    err = SockUpdateBufferSize(pSocket, val, pSocket->_cbMaxRecvBuf);
                    if (!NT_SUCCESS(err)) goto inval;
                    break;

#ifdef XNET_FEATURE_ONLINE

                case SO_INSECURE:
                    if (optlen == sizeof(CXoBase *) && optval == (char *)IpGetXoBase())
                        pSocket->SetFlags(SOCKF_INSECURE);
                    break;

#endif

                default:
                    goto noopt;
            }
            break;
        }

        case IPPROTO_TCP:
        {
            if (pSocket->IsUdp()) goto noopt;

            switch (optname) {
            case TCP_NODELAY:
                pSocket->SetFlags(val ? SOCKF_OPT_NONAGLE : 0, SOCKF_OPT_NONAGLE);
                break;

            default:
                goto noopt;
            }
            break;
        }

        default:
            goto inval;
    }

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);

inval:
    err = WSAEINVAL;
    goto exit;

noopt:
    err = WSAENOPROTOOPT;
    goto exit;
}

NTSTATUS CXnSock::SockUpdateBufferSize(CSocket* pSocket, INT sendBufsize, INT recvBufsize)

/*++

Routine Description:

    Update the send and receive buffer sizes

Arguments:

    pSocket - Points to the CSocket structure
    sendBufsize, recvBufsize - 
        Specifies the new send and receive buffer sizes

Return Value:

    Status code

--*/

{
    ICHECK(SOCK, UDPC);

    NTSTATUS status;

    if (sendBufsize > (INT) cfgSockMaxSendBufsizeInK * 1024)
        sendBufsize = (INT) cfgSockMaxSendBufsizeInK * 1024;
    else if (sendBufsize <= 0) {
        // NOTE: we never set actual send buffer size to 0
        // because we don't support the no-buffering option.
        sendBufsize = 1;
    }

    if (recvBufsize > (INT) cfgSockMaxRecvBufsizeInK * 1024)
        recvBufsize = (INT) cfgSockMaxRecvBufsizeInK * 1024;
    else if (recvBufsize <= 0)
        recvBufsize = 0;

    status = NETERR_OK;

    if (pSocket->IsUdp())
    {
        // For datagram sockets, we'll just update the send and
        // receive buffers sizes. If the current buffers are bigger
        // than the specified limits, we'll leave the current data alone.
        pSocket->_cbMaxSendBuf = sendBufsize;
        pSocket->_cbMaxRecvBuf = recvBufsize;
    }
    else
    {
        CTcpSocket* pTcpSocket = (CTcpSocket*) pSocket;

        // Set the send buffer size. If the current send buffer size
        // is larger than the specified limit, leave the current data untouched.
        pTcpSocket->_cbMaxSendBuf = sendBufsize;
        
        // If the TCP socket is already connected,
        // don't allow the app to reduce the receive buffer size.
        if (!pTcpSocket->IsIdleState() && recvBufsize < (INT) pTcpSocket->_cbMaxRecvBuf)
        {
            status = NETERR_PARAM;
        }
        else
        {
            // NOTE: we don't update receive window to the connection peer
            // right away. The new window information will be sent
            // in the next outgoing ACK segment.
            pTcpSocket->_cbMaxRecvBuf = recvBufsize;
        }
    }

    // NOTE: If the send buffer has just opened up,
    // we don't check to see if there is any pending
    // overlapped send request that can be started.
    // Rather the overlapped send request will be started
    // by the normal process.

    return(status);
}

// ---------------------------------------------------------------------------------------
// getsockopt
// ---------------------------------------------------------------------------------------

int CXnSock::getsockopt(SOCKET s, int level, int optname, char * optval, int * optlen)
{
    INT val;
    WinsockApiPrologSockLock_(getsockopt, SOCKET_ERROR);

    WinsockApiParamCheck_(
        optval != NULL &&
        optlen != NULL &&
        *optlen > 0);

    memset(optval, 0, *optlen);

    switch (level) {
    case SOL_SOCKET:

        switch (optname) {
        case SO_BROADCAST:
            if (pSocket->IsTcp()) goto noopt;
            val = pSocket->TestFlags(SOCKF_OPT_BROADCAST);
            break;

        case SO_DONTLINGER:
            if (pSocket->IsUdp()) goto noopt;
            val = !((CTcpSocket *)pSocket)->GetLingerOnOff();
            break;

        case SO_LINGER:
            if (pSocket->IsUdp()) goto noopt;
            WinsockApiParamCheck_(*optlen >= sizeof(LINGER));
            ((CTcpSocket *)pSocket)->GetLinger((LINGER *)optval);
            *optlen = sizeof(LINGER);
            goto exit;

        case SO_REUSEADDR:
            val = pSocket->TestFlags(SOCKF_OPT_REUSEADDR);
            break;

        case SO_EXCLUSIVEADDRUSE:
            val = pSocket->TestFlags(SOCKF_OPT_EXCLUSIVEADDR);
            break;

        case SO_RCVTIMEO:
            WinsockApiParamCheck_(*optlen >= sizeof(INT));
            val = pSocket->_uiRecvTimeout;
            break;

        case SO_SNDTIMEO:
            WinsockApiParamCheck_(*optlen >= sizeof(INT));
            val = pSocket->_uiSendTimeout;
            break;

        case SO_RCVBUF:
            WinsockApiParamCheck_(*optlen >= sizeof(INT));
            val = pSocket->_cbMaxRecvBuf;
            break;

        case SO_SNDBUF:
            WinsockApiParamCheck_(*optlen >= sizeof(INT));
            val = pSocket->_cbMaxSendBuf;
            break;

        case SO_TYPE:
            val = pSocket->IsUdp() ? SOCK_DGRAM : SOCK_STREAM;
            break;

        case SO_ACCEPTCONN:
            if (pSocket->IsUdp()) goto noopt;
            val = ((CTcpSocket *)pSocket)->IsListenState();
            break;

        default:
            goto noopt;
        }
        break;

    case IPPROTO_TCP:
        if (pSocket->IsUdp())
            goto noopt;

        switch (optname) {
        case TCP_NODELAY:
            val = pSocket->TestFlags(SOCKF_OPT_NONAGLE);
            break;

        default:
            goto noopt;
        }
        break;

    default:
        err = WSAEINVAL;
        goto exit;
    }

    if (*optlen < sizeof(INT)) {
        *optval = (CHAR) val;
        *optlen = 1;
    } else {
        *((INT*) optval) = val;
        *optlen = sizeof(INT);
    }

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);

noopt:
    err = WSAENOPROTOOPT;
    goto exit;
}

// ---------------------------------------------------------------------------------------
// getsockname
// ---------------------------------------------------------------------------------------

int CXnSock::getsockname(SOCKET s, struct sockaddr* name, int* namelen)
{
    struct sockaddr_in* sin;
    WinsockApiPrologSockLock_(getsockname, SOCKET_ERROR);

    WinsockApiParamCheck_(
        name != NULL &&
        namelen != NULL &&
        *namelen >= SOCKADDRLEN);

    if (!pSocket->TestFlags(SOCKF_BOUND))
    {
        WinsockApiGotoExit_(WSAEINVAL);
    }

    sin = (struct sockaddr_in*) name;
    memset(sin, 0, SOCKADDRLEN);
    sin->sin_family = AF_INET;
    sin->sin_port = pSocket->_ipportSrc;
    sin->sin_addr.s_addr = 0;

    *namelen = SOCKADDRLEN;
    err = NO_ERROR;

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// getpeername
// ---------------------------------------------------------------------------------------

int CXnSock::getpeername(SOCKET s, struct sockaddr * name, int * namelen)
{
    struct sockaddr_in* sin;
    WinsockApiPrologSockLock_(getpeername, SOCKET_ERROR);

    WinsockApiParamCheck_(
        name != NULL &&
        namelen != NULL &&
        *namelen >= SOCKADDRLEN);

    if (!pSocket->TestFlags(SOCKF_CONNECTED)) {
        WinsockApiGotoExit_(WSAENOTCONN);
    }

    sin = (struct sockaddr_in*) name;
    sin->sin_family = AF_INET;
    sin->sin_port = pSocket->_ipportDst;
    sin->sin_addr.s_addr = pSocket->_ipaDst;
    memset(sin->sin_zero, 0, sizeof(sin->sin_zero));

    *namelen = SOCKADDRLEN;
    err = NO_ERROR;

    SecRegSetOwned(sin->sin_addr.s_addr);

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// bind
// ---------------------------------------------------------------------------------------

int CXnSock::bind(SOCKET s, const struct sockaddr * name, int namelen)
{
    const struct sockaddr_in* sin = (const struct sockaddr_in*) name;
    WinsockApiPrologSockLock_(bind, SOCKET_ERROR);

    WinsockApiParamCheck_(
        name != NULL &&
        namelen >= SOCKADDRLEN &&
        sin->sin_family == AF_INET &&
        sin->sin_addr.s_addr == 0);

    if (pSocket->TestFlags(SOCKF_BOUND))
    {
        WinsockApiGotoExit_(WSAEINVAL);
    }

    if (sin->sin_addr.s_addr != 0)
    {
        WinsockApiGotoExit_(WSAEADDRNOTAVAIL);
    }

    err = SockBind(pSocket, sin->sin_port);

    MapNtStatusToWinsockError_(err);

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

NTSTATUS CXnSock::SockBind(CSocket * pSocket, CIpPort ipportBind)

/*++

Routine Description:

    Bind a socket to the specified local address

Arguments:

    pSocket - Points to the protocol control block
    srcaddr, srcport - Specifies the local socket address

Return Value:

    Status code

--*/

{
    int cRetry = ipportBind ? 0 : min(cfgSockMaxSockets, TEMP_PORT_COUNT);
    CSocket * pSocketCur;

    RaiseToDpc();

    Assert(!pSocket->TestFlags(SOCKF_BOUND));

    while (1)
    {
        if (cRetry-- > 0)
        {
            while (1)
            {
                _ipportTempNext += 1;

                if (_ipportTempNext > MAX_TEMP_PORT)
                    _ipportTempNext = MIN_TEMP_PORT;

                if (_ipportTempNext != NTOHS(ESPUDP_CLIENT_PORT))
                    break;
            }

            ipportBind = HTONS((WORD)_ipportTempNext);
        }

        for (pSocketCur = GetFirstSocket(); pSocketCur; pSocketCur = GetNextSocket(pSocketCur))
        {
            if (pSocketCur->TestFlags(SOCKF_BOUND))
            {
                Assert(pSocketCur != pSocket);

                if (    (pSocketCur->IsUdp() == pSocket->IsUdp())
                    &&  (pSocketCur->_ipportSrc == ipportBind)
                    &&  (   pSocketCur->TestFlags(SOCKF_OPT_EXCLUSIVEADDR)
                        ||  pSocket->TestFlags(SOCKF_OPT_EXCLUSIVEADDR)
                        || !pSocket->TestFlags(SOCKF_OPT_REUSEADDR)))
                {
                    break;
                }
            }
        }

        if (pSocketCur == NULL)
        {
            pSocket->_ipportSrc = ipportBind;
            pSocket->SetFlags(SOCKF_BOUND);
            return(NETERR_OK);
        }

        if (cRetry <= 0)
        {
            return(NETERR_ADDRINUSE);
        }
    }
}

// ---------------------------------------------------------------------------------------
// connect
// ---------------------------------------------------------------------------------------

int CXnSock::connect(SOCKET s, const struct sockaddr * name, int namelen)
{
    const struct sockaddr_in* sin = (const struct sockaddr_in*) name;
    CIpAddr dstaddr;
    CIpPort dstport;
    WinsockApiPrologSockLock_(connect, SOCKET_ERROR);

    WinsockApiParamCheck_(
        name != NULL &&
        namelen >= SOCKADDRLEN &&
        sin->sin_family == AF_INET);

    if (pSocket->TestFlags(SOCKF_CONNECTED) && pSocket->IsTcp()) {
        WinsockApiGotoExit_(WSAEISCONN);
    }

    dstaddr = sin->sin_addr.s_addr;
    dstport = sin->sin_port;
    if (pSocket->IsUdp())
    {
        err = UdpConnect(pSocket, dstaddr, dstport);
    }
    else if (pSocket->TestFlags(SOCKF_OPT_NONBLOCKING))
    {
        err = TcpConnect((CTcpSocket*) pSocket, dstaddr, dstport, FALSE);

        // For nonblocking socket, we'll return WSAEWOULDBLOCK
        // error code but the operation proceeds.
        if (NT_SUCCESS(err)) err = NETERR_WOULDBLOCK;
    }
    else
    {
        err = TcpConnect((CTcpSocket*) pSocket, dstaddr, dstport, FALSE);
        if (NT_SUCCESS(err))
        {
            // For blocking socket, we need to wait here for
            // the operation to complete.
            err = SockWaitForEvent(pSocket, SOCKF_EVENT_CONNECT, 0);
            pSocket->SetFlags(SOCKF_CONNECT_SELECTED);
        }
    }

    MapNtStatusToWinsockError_(err);

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// listen
// ---------------------------------------------------------------------------------------

int CXnSock::listen(SOCKET s, int backlog)
{
    WinsockApiPrologSockLock_(listen, SOCKET_ERROR);

    WinsockApiParamCheck_(pSocket->IsTcp());

    if (pSocket->TestFlags(SOCKF_CONNECTED))
    {
        WinsockApiGotoExit_(WSAEISCONN);
    }

    if (!pSocket->TestFlags(SOCKF_BOUND))
    {
        WinsockApiGotoExit_(WSAEINVAL);
    }

    err = TcpListen((CTcpSocket*) pSocket, backlog);
    MapNtStatusToWinsockError_(err);

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// accept
// ---------------------------------------------------------------------------------------

SOCKET CXnSock::accept(SOCKET s, struct sockaddr * addr, int * addrlen)
{
    CTcpSocket * pTcpSocketChild = NULL;

    WinsockApiPrologSockLock_(accept, INVALID_SOCKET);

    WinsockApiParamCheck_(
        pSocket->IsTcp() &&
        (addr == NULL ||
         addrlen != NULL && *addrlen >= SOCKADDRLEN));

    CTcpSocket * pTcpSocket = (CTcpSocket *)pSocket;

    if (!pTcpSocket->IsTcp() || !pTcpSocket->IsListenState())
    {
        WinsockApiGotoExit_(WSAEINVAL);
    }

    while (1)
    {
        {
            RaiseToDpc();
            pTcpSocketChild = pTcpSocket->DequeueConnectedChild();
        }

        if (pTcpSocketChild)
            break;

        // For nonblocking socket, return an error code
        // if we don't have any pending connection requests.

        if (pTcpSocket->TestFlags(SOCKF_OPT_NONBLOCKING))
        {
            WinsockApiGotoExit_(WSAEWOULDBLOCK);
        }

        // For blocking sockets, wait until there
        // is a connection request.

        err = SockWaitForEvent(pSocket, SOCKF_EVENT_ACCEPT, 0);

        if (err != 0)
            break;
    }

    MapNtStatusToWinsockError_(err);

    if (pTcpSocketChild)
    {
        pTcpSocketChild->SetFlags(SOCKF_OWNED);

        if (addr && addrlen)
        {
            struct sockaddr_in* sin;

            sin = (struct sockaddr_in*) addr;
            sin->sin_family = AF_INET;
            sin->sin_port = pTcpSocketChild->_ipportDst;
            sin->sin_addr.s_addr = pTcpSocketChild->_ipaDst;
            memset(sin->sin_zero, 0, sizeof(sin->sin_zero));
            *addrlen = SOCKADDRLEN;

            SecRegSetOwned(pTcpSocketChild->_ipaDst);
        }
    }

    WinsockApiExitSockUnlock_((SOCKET) pTcpSocketChild, INVALID_SOCKET);
}

// ---------------------------------------------------------------------------------------
// select
// ---------------------------------------------------------------------------------------

//
// Count the total number of socket handles
//
#define SOCKETS_IN_SET(_set) ((_set) ? ((_set)->fd_count & 0xffff) : 0)

//
// Select event masks
//
#define SELECT_READ_EVENTS (SOCKF_EVENT_READ|SOCKF_EVENT_ACCEPT|SOCKF_EVENT_CLOSE|SOCKF_EVENT_RESET)
#define SELECT_WRITE_EVENTS (SOCKF_EVENT_WRITE|SOCKF_EVENT_CONNECT)
#define SELECT_EXCEPT_EVENTS (SOCKF_EVENT_RESET)

//
// Number of KWAIT_BLOCKs allocated on the stack for select() calls.
//
#define SELECT_STACK_KWAIT_BLOCKS 3

int CXnSock::select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout)
{
    SELECTINFO tempinfo;
    PRKEVENT tempevent;
    SELECTINFO* selinfo;
    PRKEVENT* events;
    INT index, rdcnt, rwcnt, selcnt = 0;
    CSocket* pSocket;
    LARGE_INTEGER waittime;
    LARGE_INTEGER* pwait;
#ifdef XNET_FEATURE_XBOX
    KWAIT_BLOCK tempWaitBlocks[SELECT_STACK_KWAIT_BLOCKS];
    PKWAIT_BLOCK waitBlockArray = tempWaitBlocks;
#endif

    WinsockApiProlog_(select, SOCKET_ERROR);

    // Count the total number of sockets
    // (ignore the input nfds parameter)
    rdcnt = SOCKETS_IN_SET(readfds);
    rwcnt = rdcnt + SOCKETS_IN_SET(writefds);
    nfds = rwcnt + SOCKETS_IN_SET(exceptfds);

    if (nfds == 0) {
        WinsockApiReturnError_(WSAEINVAL, SOCKET_ERROR);
    }

    if (nfds == 1) {
        // Use temporary stack buffers for the special case
        // where there is only one socket. This saves us from
        // two extra memory allocations.
        events = &tempevent;
        selinfo = &tempinfo;
        memset(selinfo, 0, sizeof(SELECTINFO));
    } else {
        selinfo = (SELECTINFO*) SysAllocZ(nfds*sizeof(SELECTINFO), PTAG_select);
        events = (PRKEVENT*) SysAlloc(nfds*sizeof(PRKEVENT), PTAG_select);
        if (!selinfo || !events) {
            nfds = 0;
            WinsockApiGotoExit_(WSAENOBUFS);
        }
    }

    // Lock all the socket handles
    if ((err = SockLockSelectSockets(readfds, selinfo, 0, SELECT_READ_EVENTS)) != 0 ||
        (err = SockLockSelectSockets(writefds, selinfo, rdcnt, SELECT_WRITE_EVENTS)) != 0 ||
        (err = SockLockSelectSockets(exceptfds, selinfo, rwcnt, SELECT_EXCEPT_EVENTS)) != 0) {
        goto exit;
    }

    // Compute the wait time in 100ns unit
    if (timeout) {
        pwait = &waittime;
#ifdef XNET_FEATURE_XBOX
        waittime.QuadPart = Int32x32To64(timeout->tv_sec, -10000000) +
                            Int32x32To64(timeout->tv_usec, -10);
#else
        waittime.QuadPart = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
#endif
    } else {
        pwait = NULL;
    }

    // Check if we to wait:
    // if we do, set up the socket event flags
    if (!pwait || pwait->QuadPart) {
        INT waitCount = 0;
        for (index=0; index < nfds; index++) {
            pSocket = selinfo[index].pSocket;
            if (selinfo[index].pSocketMasks) {
                if (SockCheckSelectEvents(pSocket, selinfo[index].pSocketMasks, -1)) break;
                events[waitCount++] = pSocket->GetEvent();
            }
        }

        if (index == nfds) {

#ifdef XNET_FEATURE_XBOX
            if (waitCount > SELECT_STACK_KWAIT_BLOCKS) {
                waitBlockArray = (PKWAIT_BLOCK) SysAllocZ(waitCount * sizeof(KWAIT_BLOCK), PTAG_select);
                if (!waitBlockArray) {
                    WinsockApiGotoExit_(WSAENOBUFS);
                }
            }

#ifdef XNET_FEATURE_XBDM_SERVER

            do {
                err = KeWaitForMultipleObjects(
                            waitCount,
                            (void **)events,
                            WaitAny,
                            UserRequest,
                            KernelMode,
                            TRUE,
                            pwait,
                            waitBlockArray);
            } while(err == STATUS_KERNEL_APC);

            if(err == STATUS_ALERTED)
                err = STATUS_TIMEOUT; 

#else

            err = KeWaitForMultipleObjects(
                        waitCount,
                        (void **)events,
                        WaitAny,
                        UserRequest,
                        UserMode,
                        FALSE,
                        pwait,
                        waitBlockArray);

#endif // XBDM

            if ((err < 0 || err >= waitCount) && err != STATUS_TIMEOUT) {
                WinsockApiGotoExit_(WSAEFAULT);
            }
#else
            err = WaitForMultipleObjects(waitCount, (HANDLE *)events, FALSE, (DWORD)waittime.QuadPart);
            if (err == WAIT_FAILED)
                WinsockApiGotoExit_(WSAEFAULT);
#endif
        }
    }

    // Determine which socket events are ready
    // and return appropriate information

    if (readfds) { FD_ZERO(readfds); }
    if (writefds) { FD_ZERO(writefds); }
    if (exceptfds) { FD_ZERO(exceptfds); }

    for (index=selcnt=0; index < nfds; index++)
    {
        if (SockCheckSelectEvents(selinfo[index].pSocket, selinfo[index].eventMasks, 0))
        {
            FD_SET(selinfo[index].s, selinfo[index].fdset);
            selcnt++;
        }
    }

    err = NO_ERROR;

exit:

    for (index=0; index < nfds; index++)
    {
        pSocket = selinfo[index].pSocket;

        if (pSocket && selinfo[index].pSocketMasks)
        {
            pSocket->ClearFlags(SOCKF_EVENT_MASK);
            pSocket->Unlock();
        }
    }

#ifdef XNET_FEATURE_XBOX
    if (waitBlockArray != tempWaitBlocks) { SysFree(waitBlockArray); }
#endif
    if (selinfo != &tempinfo) { SysFree(selinfo); }
    if (events != &tempevent) { SysFree(events); }

    WinsockApiCheckError_(SOCKET_ERROR);
    return selcnt;
}

INT CXnSock::SockLockSelectSockets(fd_set* fdset, SELECTINFO* selinfo, INT offset, INT eventMasks)

/*++

Routine Description:

    Lock the socket handles that was passed to the select API

Arguments:

    fdset - Points to the socket set
    selinfo - Points to an array of SELECTINFO structures
        for storing the locked socket information
    eventMasks - Specifies the interested events

Return Value:

    Winsock error code

--*/

{
    INT i, count;

    // Nothing to do if the set is empty
    count = SOCKETS_IN_SET(fdset);

    for (i=0; i < count; i++) {
        SOCKET s = fdset->fd_array[i];
        INT j, k = offset + i;

        selinfo[k].s = s;
        selinfo[k].fdset = fdset;
        selinfo[k].eventMasks = eventMasks;

        // Check to see if the socket is already used
        // in the same select call
        for (j=0; j < k && selinfo[j].s != s; j++)
            ;

        if (j == k) {
            //
            // The socket isn't seen already
            //
            selinfo[k].pSocketMasks = eventMasks;
            selinfo[k].pSocket = CSocket::Lock(s);
            if (!selinfo[k].pSocket)
                return GetLastError();
        } else {
            //
            // The socket is already seen
            //
            selinfo[j].pSocketMasks |= eventMasks;
            selinfo[k].pSocketMasks = 0;
            selinfo[k].pSocket = selinfo[j].pSocket;
        }
    }

    return NO_ERROR;
}

INT CXnSock::SockCheckSelectEvents(CSocket * pSocket, INT eventMasks, INT setwait)

/*++

Routine Description:

    Check if the specified socket events are available
    and optionally set up the socket to wait for them

Arguments:

    pSocket - Points to the protocol control block
    eventMasks - Specifies the socket events the caller is interested in
    setwait - Whether to set up the sockets to wait if 
        none of the specified events are avaiable

Return Value:

    Set of event flags that are already available

--*/

{
    NTSTATUS status;
    INT readyMasks;

    RaiseToDpc();

    // Check to see if the specified event is already available
    // Since our checks are trivial, it's faster to check
    // everything instead of trying to check selectively
    // based on the flags specified by the caller.
    
    if (pSocket->IsTcp())
    {
        CTcpSocket * pTcpSocket = (CTcpSocket *)pSocket;

        // If the connection was reset, return reset status
        status = pSocket->GetStatus();

        if (!NT_SUCCESS(status))
        {
            // Note: If we already told the app that the socket was connected
            // and then the socket got resetted, we don't need to set the socket
            // in the exceptfds again to tell the app the connection has failed.

            if (eventMasks != SOCKF_EVENT_RESET || !pSocket->TestFlags(SOCKF_CONNECT_SELECTED))
            {
                return(SOCKF_EVENT_RESET);
            }
        }

        readyMasks = (pTcpSocket->IsTcpRecvBufEmpty() ? 0 : SOCKF_EVENT_READ) |
                     (pTcpSocket->HasConnectedChild() ? SOCKF_EVENT_ACCEPT : 0) |
                     (pTcpSocket->IsFinReceived() ? SOCKF_EVENT_READ|SOCKF_EVENT_CLOSE : 0);

        if (pSocket->TestFlags(SOCKF_CONNECTED))
        {
            if (!pSocket->IsSendBufFull())
            {
                readyMasks |= SOCKF_EVENT_WRITE;
            }

            // NOTE: we only signal the connect event exactly once
            if ((eventMasks & SOCKF_EVENT_CONNECT) && !pSocket->TestFlags(SOCKF_CONNECT_SELECTED))
            {
                readyMasks |= SOCKF_EVENT_CONNECT;
                if (setwait >= 0)
                {
                    pSocket->SetFlags(SOCKF_CONNECT_SELECTED);
                }
            }
        }
    } else {
        readyMasks = (pSocket->IsUdpRecvBufEmpty() ? 0 : SOCKF_EVENT_READ) |
                     (pSocket->IsSendBufFull() ? 0 : SOCKF_EVENT_WRITE);
    }

    if ((readyMasks &= eventMasks) == 0 && setwait)
    {
        // Indicate that we're interested in the specified event
        // and prepare to wait
        pSocket->SetFlags(eventMasks, SOCKF_EVENT_MASK);
        EvtClear(pSocket->GetEvent());
    }

    return(readyMasks);
}

// ---------------------------------------------------------------------------------------
// WSAGetOverlappedResult
// ---------------------------------------------------------------------------------------

BOOL CXnSock::WSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED overlapped, LPDWORD byteCount, BOOL fWait, LPDWORD flags)
{
    WinsockApiPrologSockLock_(WSAGetOverlappedResult, FALSE);

    WinsockApiParamCheck_(
        overlapped != NULL &&
        overlapped->hEvent != NULL &&
        byteCount != NULL &&
        flags != NULL);

    //
    // Check if we need to wait for the I/O request to complete
    //
    if (overlapped->_iostatus == NETERR_PENDING && fWait)
    {
        WaitForSingleObject(overlapped->hEvent, INFINITE);

        RaiseToDpc();

        if (overlapped->_iostatus == NETERR_PENDING)
        {
            err = overlapped->_ioxfercnt ? NETERR_OK : NETERR_CANCELLED;
            SockReqComplete(pSocket, (CSockReq *)overlapped->_ioreq, err);
        }
    }

    //
    // If the I/O request was completed,
    // return the completion status information
    //
    if ((err = overlapped->_iostatus) != NETERR_PENDING && NT_SUCCESS(err))
    {
        *byteCount = overlapped->_ioxfercnt;
        *flags = overlapped->_ioflags;
    }

    if (err == NETERR_PENDING)
    {
        WinsockApiGotoExit_(WSA_IO_INCOMPLETE);
    }
    else
    {
        MapNtStatusToWinsockError_(err);
    }
    
    WinsockApiExitSockUnlock_(TRUE, FALSE);
}

// ---------------------------------------------------------------------------------------
// WSACancelOverlappedIO
// ---------------------------------------------------------------------------------------

INT CXnSock::WSACancelOverlappedIO(SOCKET s)
{
    WinsockApiPrologSockLock_(WSACancelOverlappedIO, SOCKET_ERROR);

    {
        RaiseToDpc();
        SockReqComplete(pSocket, pSocket->GetRecvReq(), NETERR_CANCELLED);
        SockReqComplete(pSocket, pSocket->GetSendReq(), NETERR_CANCELLED);
    }

    WinsockApiGotoExit_(NO_ERROR);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// recv
// ---------------------------------------------------------------------------------------

int CXnSock::recv(SOCKET s, char* buf, int len, int flags)
{
    CRecvReq pRecvReq;
    DWORD count;
    WinsockApiPrologSockLock_(recv, SOCKET_ERROR);

    WinsockApiParamCheck_(
        (len > 0 && buf != NULL || len == 0) &&
        flags == 0);

    pRecvReq.buf = (BYTE*) buf;
    pRecvReq.buflen = len;
    pRecvReq.flags = flags;
    pRecvReq.bytesRecv = &count;
    pRecvReq.fromaddr = NULL;
    pRecvReq._pWsaOverlapped = NULL;

    err = SockRead(pSocket, &pRecvReq);

    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(count, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// WSARecv
// ---------------------------------------------------------------------------------------

//
// Verify buffers passed to WSARecv API
// NOTE: we do not support more than 1 receive buffers.
//
INLINE
BOOL CheckRecvWsaBuf(WSABUF* bufs, UINT bufcnt)
{
    return (bufcnt == 1 && bufs != NULL && (bufs->len > 0 && bufs->buf != NULL || bufs->len == 0));
}

int CXnSock::WSARecv(SOCKET s, LPWSABUF bufs, DWORD bufcnt, LPDWORD bytesRecv, LPDWORD flags,
                     LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc)
{
    CRecvReq pRecvReq;
    WinsockApiPrologSockLock_(WSARecv, SOCKET_ERROR);

    WinsockApiParamCheck_(
        CheckRecvWsaBuf(bufs, bufcnt) &&
        bytesRecv != NULL &&
        flags != NULL && *flags == 0 &&
        completionproc == NULL);

    pRecvReq.buf = (BYTE*) bufs->buf;
    pRecvReq.buflen = bufs->len;
    pRecvReq.flags = *flags;
    pRecvReq.bytesRecv = bytesRecv;
    pRecvReq.fromaddr = NULL;
    pRecvReq._pWsaOverlapped = overlapped;

    err = SockRead(pSocket, &pRecvReq);

    *flags = pRecvReq.flags;

    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// recvfrom
// ---------------------------------------------------------------------------------------

int CXnSock::recvfrom(SOCKET s, char FAR * buf, int len, int flags, struct sockaddr * from, int * fromlen)
{
    DWORD count;
    INT err;
    WSABUF wsabuf;

    WinsockApiPrologLight_(recvfrom);
    WinsockApiParamCheck_(
        (len > 0 && buf != NULL || len == 0) &&
        (from == NULL ||
         fromlen != NULL && *fromlen >= SOCKADDRLEN));

    wsabuf.len = len;
    wsabuf.buf = buf;
    err = WSARecvFrom(s, &wsabuf, 1, &count, (DWORD*) &flags, from, fromlen, NULL, NULL);
    return (err == NO_ERROR) ? count : SOCKET_ERROR;
}

// ---------------------------------------------------------------------------------------
// WSARecvFrom
// ---------------------------------------------------------------------------------------

int CXnSock::WSARecvFrom(SOCKET s, LPWSABUF bufs, DWORD bufcnt, LPDWORD bytesRecv, LPDWORD flags,
                         struct sockaddr * fromaddr, LPINT fromlen, LPWSAOVERLAPPED overlapped,
                         LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc)
{
    CRecvReq pRecvReq;
    WinsockApiPrologSockLock_(WSARecvFrom, SOCKET_ERROR);

    WinsockApiParamCheck_(
        CheckRecvWsaBuf(bufs, bufcnt) &&
        bytesRecv != NULL &&
        (fromaddr == NULL ||
         fromlen != NULL && *fromlen >= SOCKADDRLEN) &&
        flags != NULL && *flags == 0 &&
        completionproc == NULL);

    // Winsock documentation on this call is extremely confusing
    // regarding the correct behavior for connection-oriented sockets
    // Here I've taken the liberty to treat WSARecvFrom the same way
    // as WSARecv for such cases.
    if (pSocket->IsTcp()) {
        TraceSz(Verbose, "WSARecvFrom called on stream socket!");
    }

    pRecvReq.buf = (BYTE*) bufs->buf;
    pRecvReq.buflen = bufs->len;
    pRecvReq.flags = *flags;
    pRecvReq.bytesRecv = bytesRecv;
    pRecvReq._pWsaOverlapped = overlapped;
    pRecvReq.fromaddr = (struct sockaddr_in*) fromaddr;

    if (fromaddr) 
    {
        memset(fromaddr, 0, SOCKADDRLEN);
        *fromlen = SOCKADDRLEN;
        pRecvReq.fromaddr->sin_family = AF_INET;
    }

    err = SockRead(pSocket, &pRecvReq);
    *flags = pRecvReq.flags;
    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// SockRead
// ---------------------------------------------------------------------------------------

NTSTATUS CXnSock::SockRead(CSocket * pSocket, CRecvReq * pRecvReq)
{
    WSAOVERLAPPED * pWsaOverlapped;
    WSAOVERLAPPED   WsaOverlappedTemp;
    NTSTATUS        status;

    if (pSocket->IsTcp())
    {
        if (!pSocket->TestFlags(SOCKF_CONNECTED))
        {
            return NETERR(WSAENOTCONN);
        }
    }
    else
    {
        if (!pSocket->TestFlags(SOCKF_BOUND))
        {
            return NETERR(WSAEINVAL);
        }
    }

    if (pSocket->TestFlags(SOCKF_NOMORE_RECV))
    {
        return(!NT_SUCCESS(pSocket->GetStatus()) ? pSocket->GetStatus() : NETERR(WSAESHUTDOWN));
    }

    // Since all our sockets are overlapped, we don't
    // enforce the Win32 behavior that the input socket
    // must be a non-overlapped socket.

    pWsaOverlapped = pRecvReq->_pWsaOverlapped;

    if (pWsaOverlapped != NULL)
    {
        pRecvReq->_pEvent = EvtFromHandle(pWsaOverlapped->hEvent);

        if (pRecvReq->_pEvent == NULL)
        {
            return(NETERR(WSASYSCALLFAILURE));
        }
    }
    else
    {
        pRecvReq->_pEvent = NULL;
    }

    // Check if we have any buffered data waiting to be read

    if (pSocket->IsRecvBufEmpty())
    {
        if (pWsaOverlapped)
        {
            // Overlapped call.  This used to clear the event here, but that breaks if the caller
            // is using the same event for multiple sockets and/or online tasks.  So now we
            // let the caller reset the event themselves before making the overlapped call.
            // Normally they would use an auto-reset event anyways, so there is nothing for them to do.
        }
        else if (pSocket->TestFlags(SOCKF_OPT_NONBLOCKING))
        {
            // Nonoverlapped call and socket is nonblocking:
            // just return WOULDBLOCK error code.
            //
            // Note: For TCP socket, if FIN has been received
            // we should return success with bytesRecv set to 0.
            if (pSocket->IsUdp() || !((CTcpSocket *)pSocket)->IsFinReceived())
            {
                return(NETERR(WSAEWOULDBLOCK));
            }
        }
        else
        {
            // Blocking call - prepare to wait
            pRecvReq->_pWsaOverlapped = &WsaOverlappedTemp;
            pRecvReq->_pEvent = pSocket->GetEvent();
            EvtClear(pRecvReq->_pEvent);
        }
    }

    status = pSocket->IsUdp() ?
                UdpRead(pSocket, pRecvReq) :
                TcpRead((CTcpSocket *) pSocket, pRecvReq);

    if (status == NETERR_PENDING)
    {
        if (pWsaOverlapped == NULL)
        {
            // A blocking call is still in progress
            EvtWait(pRecvReq->_pEvent, pSocket->_uiRecvTimeout);

            if (WsaOverlappedTemp._iostatus == NETERR_PENDING)
            {
                RaiseToDpc();

                if (WsaOverlappedTemp._iostatus == NETERR_PENDING)
                {
                    // We can use pRecvReq directly here because
                    // blocking recv call is treated as a special case
                    // in SockQueueOverlappedRecv 
                    SockReqComplete(pSocket, pRecvReq, NETERR_TIMEOUT);
                }
            }

            *pRecvReq->bytesRecv = WsaOverlappedTemp._ioxfercnt;
            pRecvReq->flags = WsaOverlappedTemp._ioflags;
            status = WsaOverlappedTemp._iostatus;
        }
    }
    else
    {
        if (pWsaOverlapped)
        {
            // An overlapped call was completed immediately
            pWsaOverlapped->_ioflags = pRecvReq->flags;
            pWsaOverlapped->_ioxfercnt = *pRecvReq->bytesRecv;
            pWsaOverlapped->_iostatus = status;

            // It would seem to be a waste to signal the event here.
            // But that's win2k behavior.
            EvtSet(pRecvReq->_pEvent, 0);
            EvtDereference(pRecvReq->_pEvent);
        }
    }

    return(status);
}

NTSTATUS CXnSock::RecvReqEnqueue(CSocket * pSocket, CRecvReq * pRecvReq)
{
    ICHECK(SOCK, UDPC|SDPC);

    CRecvReq * pRecvReqNew;

    if (pSocket->HasRecvReq())
    {
        TraceSz2(sockWarn, "[%08lX:%s] Can only have one overlapped read at a time",
                 pSocket, pSocket->TypeStr());
        return(NETERR_WOULDBLOCK);
    }
    else if (!pRecvReq->_pEvent)
    {
        TraceSz2(sockWarn, "[%08lX:%s] Overlapped read on nonblocking socket must supply an event",
                pSocket, pSocket->TypeStr());
        return(NETERR_WOULDBLOCK);
    }

    if (pRecvReq->_pEvent == pSocket->GetEvent())
    {
        // If the wait event is our internal per-CSocket event,
        // then this is a special case for implementing the
        // blocking recv call.
        //
        // In this case, we avoid an allocation by directly
        // queuing up the RECVREQ structure that was passed
        // in from the caller (winsock layer). This works because
        // the caller will wait for the recv to complete after
        // this function returns.

        pRecvReqNew = pRecvReq;
    }
    else
    {
        pRecvReqNew = (CRecvReq *)SysAlloc(sizeof(CRecvReq), PTAG_CRecvReq);

        if (pRecvReqNew == NULL)
        {
            TraceSz2(sockWarn, "[%08lX:%s] Out of memory allocating overlapped read control block",
                     pSocket, pSocket->TypeStr());
            return(NETERR_MEMORY);
        }

        *pRecvReqNew = *pRecvReq;
    }

    pSocket->SetRecvReq(pRecvReqNew);

    pRecvReqNew->_pWsaOverlapped->_ioxfercnt = 0;
    pRecvReqNew->_pWsaOverlapped->_ioflags = 0;
    pRecvReqNew->_pWsaOverlapped->_ioreq = (UINT_PTR)pRecvReqNew;
    pRecvReqNew->_pWsaOverlapped->_iostatus = (DWORD)NETERR_PENDING;

    TraceSz3(sock, "[%08lX:%s] Enqueued overlapped read of %ld bytes",
             pSocket, pSocket->TypeStr(), pRecvReq->GetCbBuf());

    return(NETERR_PENDING);
}

// ---------------------------------------------------------------------------------------
// Forward Declarations
// ---------------------------------------------------------------------------------------

UINT SockCountSendTotal(WSABUF * bufs, UINT bufcnt);
BOOL SockCheckSendWsaBuf(WSABUF * bufs, UINT bufcnt);

// ---------------------------------------------------------------------------------------
// send
// ---------------------------------------------------------------------------------------

int CXnSock::send(SOCKET s, const char* buf, int len, int flags)
{
    WSABUF wsabuf;
    CSendReq SendReq;
    WinsockApiPrologSockLock_(send, SOCKET_ERROR);

    WinsockApiParamCheck_(
        (len > 0 && buf != NULL || len == 0) &&
        (flags  == 0));

    wsabuf.len = len;
    wsabuf.buf = (char*) buf;
    SendReq._pWsaOverlapped = NULL;
    SendReq.bufs = &wsabuf;
    SendReq.bufcnt = 1;
    SendReq.sendtotal = len;
    SendReq.toaddr = NULL;

    err = SockSend(pSocket, &SendReq);
    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(SendReq.sendtotal, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// WSASend
// ---------------------------------------------------------------------------------------

int CXnSock::WSASend(SOCKET s, LPWSABUF bufs, DWORD bufcnt, LPDWORD bytesSent, DWORD flags,
                     LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc)
{
    CSendReq SendReq;
    WinsockApiPrologSockLock_(WSASend, SOCKET_ERROR);

    WinsockApiParamCheck_(
        SockCheckSendWsaBuf(bufs, bufcnt) &&
        bytesSent != NULL &&
        flags == 0 &&
        completionproc == NULL);

    SendReq._pWsaOverlapped = overlapped;
    SendReq.bufs = bufs;
    SendReq.bufcnt = (WORD)bufcnt;
    SendReq.sendtotal = SockCountSendTotal(bufs, bufcnt);
    SendReq.toaddr = NULL;

    err = SockSend(pSocket, &SendReq);
    if (NT_SUCCESS(err)) *bytesSent = SendReq.sendtotal;
    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

BOOL SockCheckSendWsaBuf(WSABUF* bufs, UINT bufcnt)
{
    if (bufs == NULL || bufcnt == 0 || bufcnt > 0xFFFF)
        return FALSE;

    while (bufcnt--) {
        if (bufs->len && bufs->buf == NULL)
            return FALSE;
        bufs++;
    }
    return TRUE;
}

UINT SockCountSendTotal(WSABUF* bufs, UINT bufcnt)
{
    UINT total = 0;

    while (bufcnt--)
    {
        total += bufs->len;
        bufs++;
    }

    return(total);
}

// ---------------------------------------------------------------------------------------
// sendto
// ---------------------------------------------------------------------------------------

int CXnSock::sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
{
    DWORD count;
    INT err;
    WSABUF wsabuf;

    WinsockApiPrologLight_(sendto);
    WinsockApiParamCheck_(len > 0 && buf != NULL || len == 0);

    wsabuf.len = len;
    wsabuf.buf = (char*) buf;
    err = WSASendTo(s, &wsabuf, 1, &count, flags, to, tolen, NULL, NULL);
    return (err == NO_ERROR) ? count : SOCKET_ERROR;
}

// ---------------------------------------------------------------------------------------
// WSASendTo
// ---------------------------------------------------------------------------------------

int CXnSock::WSASendTo(SOCKET s, LPWSABUF bufs, DWORD bufcnt, LPDWORD bytesSent, DWORD flags,
                       const struct sockaddr* toaddr, int tolen, LPWSAOVERLAPPED overlapped,        
                       LPWSAOVERLAPPED_COMPLETION_ROUTINE completionproc)
{
    struct sockaddr_in* sin = (struct sockaddr_in*) toaddr;
    CSendReq SendReq;
    WinsockApiPrologSockLock_(WSASendTo, SOCKET_ERROR);

    WinsockApiParamCheck_(
        SockCheckSendWsaBuf(bufs, bufcnt) &&
        bytesSent != NULL &&
        flags == 0 &&
        (toaddr == NULL ||
         tolen >= SOCKADDRLEN && sin->sin_family == AF_INET) &&
        completionproc == NULL);

    SendReq._pWsaOverlapped = overlapped;
    SendReq.bufs = bufs;
    SendReq.bufcnt = (WORD)bufcnt;
    SendReq.sendtotal = SockCountSendTotal(bufs, bufcnt);

    if (pSocket->IsTcp() || toaddr == NULL)
    {
        // For TCP sockets, WSASendTo is equivalent to WSASend.
        // We simply ignore lpTo and iToLen parameters.
        //
        // Also, if toaddr parameter is NULL, we treat WSASendTo
        // the same way as WSASend.

        SendReq.toaddr = NULL;
    }
    else
    {
        CIpAddr ipaDst = sin->sin_addr.s_addr;

        // Must do this check because downstream code
        // doesn't expect the destination address to be 0.
        // Also, we consider sending to UDP port 0 an error.
        if (    ipaDst == 0
            ||  sin->sin_port == 0
            ||  ipaDst.IsMulticast()
            || (ipaDst.IsLoopback() && ipaDst != IPADDR_LOOPBACK))
        {
            WinsockApiGotoExit_(WSAEADDRNOTAVAIL);
        }

        // Is this socket allowed to send broadcast
        // datagrams on this socket?
        if (ipaDst.IsBroadcast() && !pSocket->TestFlags(SOCKF_OPT_BROADCAST))
        {
            WinsockApiGotoExit_(WSAEACCES);
        }

        // If the socket is not bound, bind it here
        if (!pSocket->TestFlags(SOCKF_BOUND))
        {
            err = SockBind(pSocket, 0);

            if (!NT_SUCCESS(err))
            {
                MapNtStatusToWinsockError_(err);
                goto exit;
            }
        }

        // Send to the new destination
        SendReq.toaddr = sin;
    }
        
    err = SockSend(pSocket, &SendReq);

    if (NT_SUCCESS(err))
    {
        *bytesSent = SendReq.sendtotal;
    }

    MapNtStatusToWinsockError_(err);
    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

// ---------------------------------------------------------------------------------------
// SockSend
// ---------------------------------------------------------------------------------------

NTSTATUS CXnSock::SockSend(CSocket* pSocket, CSendReq* pSendReq)

/*++

Routine Description:

    Internal function for sending out data from a socket

Arguments:

    pSocket - Points to the protocol control block
    pSendReq - Points to send user request information

Return Value:

    Status code

--*/

{
    WSAOVERLAPPED * pWsaOverlapped;
    NTSTATUS        status;

    if (!pSocket->TestFlags(SOCKF_CONNECTED) && !pSendReq->toaddr)
        return NETERR(WSAENOTCONN);

    if (pSocket->TestFlags(SOCKF_NOMORE_XMIT))
    {
        return(!NT_SUCCESS(pSocket->GetStatus()) ? pSocket->GetStatus() : NETERR(WSAESHUTDOWN));
    }

    pWsaOverlapped = pSendReq->_pWsaOverlapped;

    if (pWsaOverlapped != NULL)
    {
        pSendReq->_pEvent = EvtFromHandle(pWsaOverlapped->hEvent);

        if (pSendReq->_pEvent == NULL)
        {
            return(NETERR(WSASYSCALLFAILURE));
        }
    }

    // Check if we have room in the send buffer

    if (pSocket->IsSendBufFull())
    {
        if (pWsaOverlapped)
        {
            // Overlapped call.  This used to clear the event here, but that breaks if the caller
            // is using the same event for multiple sockets and/or online tasks.  So now we
            // let the caller reset the event themselves before making the overlapped call.
            // Normally they would use an auto-reset event anyways, so there is nothing for them to do.

            status = SockQueueOverlappedSend(pSocket, pSendReq);

            // The overlapped request was successfully queued up
            if (status == NETERR_PENDING)
                return(status);

            // The overlapped send request wasn't queued
            // because of an error.
            if (!NT_SUCCESS(status))
                goto exit;

            // The send buffer has opened up and
            // the overlapped send request wasn't queued.
        }
        else if (pSocket->TestFlags(SOCKF_OPT_NONBLOCKING) || pSocket->HasSendReq())
        {
            return(NETERR_WOULDBLOCK);
        }
        else
        {
            status = SockWaitForEvent(pSocket, SOCKF_EVENT_WRITE, pSocket->_uiSendTimeout);

            if (!NT_SUCCESS(status))
                return(status);
        }
    }

    status = pSocket->IsUdp() ?
                UdpSend(pSocket, pSendReq, 0) :
                TcpSend((CTcpSocket *) pSocket, pSendReq, 0);

exit:

    if (pWsaOverlapped)
    {
        // An overlapped call was completed immediately
        pWsaOverlapped->_ioflags = 0;
        pWsaOverlapped->_ioxfercnt = pSendReq->sendtotal;
        pWsaOverlapped->_iostatus = status;

        // It would seem to be a waste to signal the event here.
        // But that's win2k behavior.

        EvtSet(pSendReq->_pEvent, 0);
        EvtDereference(pSendReq->_pEvent);
    }

    return(status);
}

NTSTATUS CXnSock::SockQueueOverlappedSend(CSocket * pSocket, CSendReq * pSendReq)

/*++

Routine Description:

    Queue up an overlapped send request

Arguments:

    pSocket - Points to the protocol control block
    pSendReq - Points to the overlapped send request

Return Value:

    Status code:
        NETERR_PENDING - the request was successfully queued up
        NETERR_OK - if the send buffer has opened up and
            there is no need to queue up the request
        otherwise - the request was not queued up due to an error

--*/

{
    NTSTATUS status;

    RaiseToDpc();

    if (pSocket->HasSendReq())
    {
        // We only support 1 outstanding overlapped send request.
        status = NETERR_WOULDBLOCK;
    }
    else if (!pSocket->IsSendBufFull())
    {
        // The send buffer opened up just as
        // we were raising to DPC level
        status = NETERR_OK;
    }
    else
    {
        CSendReq* newreq;
        UINT size = sizeof(CSendReq) +
                    (pSendReq->toaddr ? sizeof(*pSendReq->toaddr) : 0) +
                    sizeof(WSABUF) * pSendReq->bufcnt;

        newreq = (CSendReq*) SysAlloc(size, PTAG_CXmitReq);
        if (!newreq)
        {
            // Out of memory
            status = NETERR_MEMORY;
        }
        else
        {
            VOID* bufs;

            pSocket->SetSendReq(newreq);
            *newreq = *pSendReq;
            bufs = newreq+1;
            if (pSendReq->toaddr) {
                newreq->toaddr = (struct sockaddr_in*) bufs;
                *newreq->toaddr = *pSendReq->toaddr;
                bufs = newreq->toaddr + 1;
            }

            newreq->bufs = (WSABUF*) bufs;
            memcpy(bufs, pSendReq->bufs, sizeof(WSABUF) * pSendReq->bufcnt);

            newreq->_pWsaOverlapped->_ioxfercnt = 0;
            newreq->_pWsaOverlapped->_ioflags = 0;
            newreq->_pWsaOverlapped->_ioreq = (UINT_PTR) newreq;
            newreq->_pWsaOverlapped->_iostatus = status = NETERR_PENDING;
        }
    }

    return(status);
}

// ---------------------------------------------------------------------------------------
// inet_addr
// ---------------------------------------------------------------------------------------

ULONG _WSAAPI_ inet_addr(const char * pch)
{
    WinsockApiPrologTrivial_(inet_addr);
    WinsockApiParamCheck_(pch != NULL);

    // Convert the string representation of IP address to its binary form.
    // The following formats are recognized:
    //      a.b.c.d     8-8-8-8
    //      a.b.c       8-8-16
    //      a.b         8-24
    //      a           32
    // Each field can be in decimal, octal, or hex format.

    ULONG fields[4], addr;
    UINT fieldcnt = 0;
    const UCHAR* p = (const UCHAR *) pch;

    // NOTE: We don't handle overflow conditions.

    while (TRUE) {
        // skip leading spaces
        while (*p == ' ') p++;
        if (fieldcnt >= 4 || *p == 0) break;
    
        addr = 0;
        if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
            // hex number
            const UCHAR* q = (p += 2);

            while (TRUE) {
                if (*p >= '0' && *p <= '9')
                    addr = (addr << 4) + (*p - '0');
                else if (*p >= 'a' && *p <= 'f')
                    addr = (addr << 4) + 10 + (*p - 'a');
                else if (*p >= 'A' && *p <= 'F')
                    addr = (addr << 4) + 10 + (*p - 'A');
                else
                    break;
                p++;
            }

            if (q == p)
            {
                return(INADDR_NONE);
            }

        } else if (*p == '0') {
            // octal number
            do {
                addr = (addr << 3) + (*p - '0');
                p++;
            } while (*p >= '0' && *p <= '7');
        } else if (*p >= '1' && *p <= '9') {
            // decimal number
            do {
                addr = addr*10 + (*p - '0');
                p++;
            } while (*p >= '0' && *p <= '9');
        } else {
            // invalid character
            break;
        }

        // skip trailing spaces and . separator
        while (*p == ' ') p++;
        if (*p == '.') p++;

        fields[fieldcnt++] = addr;
    }

    if (*p)
    {
        return(INADDR_NONE);
    }

    switch (fieldcnt) {
    case 1:
        addr = fields[0];
        break;
    case 2:
        addr = ((fields[0] & 0xff) << 24) |
               (fields[1] & 0xffffff);
        break;
    case 3:
        addr = ((fields[0] & 0xff) << 24) |
               ((fields[1] & 0xff) << 16) |
               (fields[2] & 0xffff);
        break;
    case 4:
        addr = ((fields[0] & 0xff) << 24) |
               ((fields[1] & 0xff) << 16) |
               ((fields[2] & 0xff) <<  8) |
               (fields[3] & 0xff);
        break;

    default:
        addr = 0;
        break;
    }

    return(HTONL(addr));
}

// ---------------------------------------------------------------------------------------
// Miscellanous
// ---------------------------------------------------------------------------------------

u_long      _WSAAPI_ htonl(IN u_long hostlong)            { return HTONL(hostlong); }
u_short     _WSAAPI_ htons(IN u_short hostshort)          { return HTONS(hostshort); }
u_long      _WSAAPI_ ntohl(IN u_long netlong)             { return NTOHL(netlong); }
u_short     _WSAAPI_ ntohs(IN u_short netshort)           { return NTOHS(netshort); }
int         _WSAAPI_ WSAGetLastError()                    { return(GetLastError()); }
void        _WSAAPI_ WSASetLastError(IN int error)        { SetLastError(error); }
WSAEVENT    _WSAAPI_ WSACreateEvent()                     { return(CreateEvent(NULL, TRUE, FALSE, NULL)); }
BOOL        _WSAAPI_ WSACloseEvent(IN WSAEVENT hEvent)    { return(CloseHandle(hEvent)); }
BOOL        _WSAAPI_ WSASetEvent(IN WSAEVENT hEvent)      { return(SetEvent(hEvent)); }
BOOL        _WSAAPI_ WSAResetEvent(IN WSAEVENT hEvent)    { return(ResetEvent(hEvent)); }

DWORD _WSAAPI_ WSAWaitForMultipleEvents(IN DWORD cEvents, IN const WSAEVENT* lphEvents,
                                        IN BOOL fWaitAll, IN DWORD dwTimeout, IN BOOL fAlertable)
{
    return(WaitForMultipleObjectsEx(cEvents, lphEvents, fWaitAll, dwTimeout, fAlertable));
}

int _WSAAPI_ __WSAFDIsSet(SOCKET fd, fd_set * set)
{
    int i = (set->fd_count & 0xffff);

    while (i--)
    {
        if (set->fd_array[i] == fd)
            return 1;
    }

    return 0;
}

// ---------------------------------------------------------------------------------------
// SockLock
// ---------------------------------------------------------------------------------------

CSocket * CSocket::Lock(SOCKET s)
{
    CSocket *   pSocket;
    LONG        lLock;
    int         err;

    // NOTE: We don't allow multiple threads to access the same
    // socket simultaneously. If the app tries to do that, the first
    // thread will succeed and other threads will get an error return.

    if (s == 0 || s == INVALID_SOCKET)
    {
        err = WSAENOTSOCK;
    }
    else
    {
        pSocket = (CSocket *)s;
        lLock   = InterlockedCompareExchange(&pSocket->_lLock, SOCK_LOCK_BUSY, SOCK_LOCK_ACTIVE);

        if (lLock == SOCK_LOCK_ACTIVE)
            return(pSocket);

        err = (lLock == SOCK_LOCK_BUSY) ? WSAEINPROGRESS : WSAENOTSOCK;
    }

#if defined(XNET_FEATURE_TRACE) && !defined(XNET_FEATURE_XBDM_SERVER)
    if (err == WSAEINPROGRESS)
    {
        TraceSz1(sockWarn, "[%X] Socket thread contention", s);
    }
#endif

    SetLastError(err);

    return(NULL);
}

// ---------------------------------------------------------------------------------------
// CXnSock::SockInit
// ---------------------------------------------------------------------------------------

NTSTATUS CXnSock::SockInit(XNetInitParams * pxnip)
{
    TCHECK(USER);

    LARGE_INTEGER dueTime;
    UINT period;

    NTSTATUS status = IpInit(pxnip);
    if (!NT_SUCCESS(status))
        return(status);

    SetInitFlag(INITF_SOCK);

    InitializeListHead(&_leSockets);
    InitializeListHead(&_leDeadSockets);
    _ipportTempNext = MAX_TEMP_PORT;

    return(NETERR_OK);
}

void CXnSock::SockTerm()
{
    TCHECK(USER);

    RaiseToDpc();

    if (TestInitFlag(INITF_SOCK))
    {
        SockStop();
        SockShutdown();
    }

    SetInitFlag(INITF_SOCK_TERM);

    IpTerm();
}

void CXnSock::SockShutdown()
{
    if (TestInitFlag(INITF_SOCK))
    {
        RaiseToDpc();

        while (!IsListEmpty(&_leSockets))
        {
            CSocket * pSocket = (CSocket*)_leSockets.Flink;

            if (pSocket->TestFlags(SOCKF_OWNED))
            {
                TraceSz1(LeakWarn, "Socket %08lX not closed before WSACleanup", pSocket);
            }

            SockClose(pSocket, TRUE);
        }

        while (!IsListEmpty(&_leDeadSockets))
        {
            CSocket * pSocket = (CSocket*) _leDeadSockets.Flink;

            SockClose(pSocket, TRUE);
        }
    }
}

// ---------------------------------------------------------------------------------------
// SockWaitForEvent
// ---------------------------------------------------------------------------------------

NTSTATUS CXnSock::SockWaitForEvent(CSocket* pSocket, INT eventMask, UINT timeout)

/*++

Routine Description:

    Block the current thread until the specified CSocket event is signalled

Arguments:

    pSocket - Points to the protocol control block
    eventMask - Flag bit to indicate which event to block on
    timeout - Specifies the wait timeout (in milliseconds, 0 means forever)

Return Value:

    Status code

--*/

{
    INT readyMask;
    NTSTATUS status = NETERR_OK;

    //
    // Check if the specified event is already available
    // or if the socket connection has been reset.
    //
    readyMask = SockCheckSelectEvents(pSocket, eventMask, 1);
    if (readyMask & SOCKF_EVENT_RESET) goto exit;
    if (readyMask != 0) return NETERR_OK;

    status = EvtWait(pSocket->GetEvent(), timeout) ? NETERR_OK : NETERR_TIMEOUT;

    pSocket->ClearFlags(SOCKF_EVENT_MASK);

exit:
    return NT_SUCCESS(status) ? pSocket->GetStatus() : status;
}


void CXnSock::SockReqComplete(CSocket * pSocket, CSockReq * pSockReq, NTSTATUS status)
{
    if (pSockReq)
    {
        if (pSocket->GetSendReq() == pSockReq)
            pSocket->SetSendReq(NULL);
        else if (pSocket->GetRecvReq() == pSockReq)
            pSocket->SetRecvReq(NULL);

        pSockReq->_pWsaOverlapped->_iostatus = status;

        EvtSet(pSockReq->_pEvent, EVENT_INCREMENT);

        if (pSockReq->_pEvent != pSocket->GetEvent())
        {
            EvtDereference(pSockReq->_pEvent);
            SysFree(pSockReq);
        }
    }
}


CSocket * CXnSock::SockFindMatch(CIpPort toport, CIpAddr fromaddr, CIpPort fromport, BYTE type)
{
    TCHECK(SDPC);

    CSocket *   pSocket;
    CSocket *   pSocketBest = NULL;
    BOOL        fIsUdp      = (type == SOCK_DGRAM);
    UINT        cWildMin    = 3;
    UINT        cWild;

    for (pSocket = GetFirstSocket(); pSocket; pSocket = GetNextSocket(pSocket))
    {
        if (!!pSocket->IsUdp() != fIsUdp)
            continue;

        if (pSocket->_ipportSrc != toport)
            continue;

        cWild = 0;

        if (pSocket->_ipaDst != fromaddr)
        {
            if (pSocket->_ipaDst)
                continue;

            cWild++;
        }

        if (pSocket->_ipportDst != fromport)
        {
            if (pSocket->_ipportDst)
                continue;

            cWild++;
        }

        if (cWild == 0)
        {
            pSocketBest = pSocket;
            break;
        }

        if (cWild < cWildMin)
        {
            pSocketBest = pSocket;
            cWildMin = cWild;
        }
    }

    return(pSocketBest);
}

void CSocket::SetFlagsAndOr(DWORD dwAnd, DWORD dwOr)
{
    ICHECK_(GetXnBase(), SOCK, USER|UDPC|SDPC);

    DWORD dwFlagsOld, dwFlagsNew;

    while (1)
    {
        dwFlagsOld = _dwFlags;
        dwFlagsNew = (dwFlagsOld & dwAnd) | dwOr;

        if (InterlockedCompareExchange((LONG *)&_dwFlags, (LONG)dwFlagsNew, (LONG)dwFlagsOld) == (LONG)dwFlagsOld)
            break;

        TraceSz(Warning, "CSocket::SetFlagsAndOr: Thread/DPC contention detected.  Retrying.");
    }
}

// ---------------------------------------------------------------------------------------
// SockReset
// ---------------------------------------------------------------------------------------

void CXnSock::SockReset(CIpAddr ipa)
{
    TCHECK(UDPC|SDPC);

    if (ipa != 0 && TestInitFlag(INITF_SOCK) && !TestInitFlag(INITF_SOCK_TERM))
    {
        CSocket *       pSocket;
        CSocket *       pSocketNext;
        CTcpSocket *    pTcpSocket;

        for (pSocket = GetFirstSocket(); pSocket; pSocket = pSocketNext)
        {
            pSocketNext = GetNextSocket(pSocket);

            if (pSocket->IsTcp() && pSocket->_ipaDst == ipa)
            {
                pTcpSocket = (CTcpSocket *)pSocket;

                if (!pTcpSocket->IsIdleState())
                {
                    TcpReset(pTcpSocket, NETERR_CONNRESET);
                }
            }
        }
    }
}
