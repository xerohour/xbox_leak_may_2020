/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    sockopt.c

Abstract:

    Implement the following Winsock APIs
        setsockopt
        getsockopt
        ioctlsocket
        WSAIoctl

Revision History:

    06/01/2000 davidx
        Created it.

--*/

#include "precomp.h"


int WSAAPI
setsockopt(
    SOCKET s,                 
    int level,                
    int optname,              
    const char* optval,  
    int optlen                
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    INT val;
    WinsockApiPrologSockLock_(setsockopt, SOCKET_ERROR);
    WinsockApiParamCheck_(optval != NULL && optlen > 0);

    if (optlen < (INT) sizeof(INT))
        val = (UCHAR) *optval;
    else
        val = *((INT*) optval);

    switch (level) {
    case SOL_SOCKET:

        // NOTE: Some options may not be applicable to datagram sockets,
        // while others may not be applicable to stream sockets. But
        // we'll let apps set those options here anyway (to avoid extra
        // checking code). Downstream code just won't use those options.

        switch (optname) {
        case SO_BROADCAST:
            if (IsTcb(pcb)) goto noopt;
            pcb->broadcast = val;
            break;

        case SO_DONTLINGER:
            pcb->linger.l_onoff = (u_short) !val;
            break;

        case SO_LINGER:
            WinsockApiParamCheck_(optlen >= sizeof(LINGER));
            CopyMem(&pcb->linger, optval, sizeof(LINGER));
            break;

        case SO_REUSEADDR:
            if (pcb->exclusiveAddr && val != 0) goto inval;
            pcb->reuseAddr = val;
            break;

        case SO_EXCLUSIVEADDRUSE:
            if (pcb->reuseAddr && val != 0) goto inval;
            pcb->exclusiveAddr = val;
            break;
        
        case SO_RCVTIMEO:
            // Timeout value is in milliseconds
            WinsockApiParamCheck_(optlen >= sizeof(INT));
            pcb->recvTimeout = val;
            break;

        case SO_SNDTIMEO:
            // Timeout value is in milliseconds
            WinsockApiParamCheck_(optlen >= sizeof(INT));
            pcb->sendTimeout = val;
            break;

        case SO_RCVBUF:
            WinsockApiParamCheck_(optlen >= sizeof(INT));
            err = PcbUpdateBufferSize(pcb, pcb->maxSendBufsize, val);
            if (!NT_SUCCESS(err)) goto inval;
            break;

        case SO_SNDBUF:
            WinsockApiParamCheck_(optlen >= sizeof(INT));
            err = PcbUpdateBufferSize(pcb, val, pcb->maxRecvBufsize);
            if (!NT_SUCCESS(err)) goto inval;
            break;

        // case SO_OOBINLINE:
        // case SO_KEEPALIVE:
        // case SO_DONTROUTE:
        // case SO_RCVLOWAT:
        // case SO_SNDLOWAT:
        default:
            goto noopt;
        }
        break;

    case IPPROTO_TCP:
        if (IsDgramPcb(pcb)) goto noopt;

        switch (optname) {
        case TCP_NODELAY:
            pcb->noNagle = val;
            break;

        default:
            goto noopt;
        }
        break;

    case IPPROTO_IP:
        switch (optname) {
        case IP_DONTFRAGMENT:
            // NOTE: For TCP sockets, DF bit is controlled
            // by the path MTU discovery mechanism. So
            // we don't allow apps to muck with it.
            if (IsTcb(pcb)) goto inval;
            pcb->ipDontFrag = val;
            break;

        case IP_TTL:
            if (val < 0 || val > 255) goto inval;
            pcb->ipTtl = (BYTE) val;
            break;

        case IP_TOS:
            if (val < 0 || val > 255) goto inval;
            pcb->ipTos = (BYTE) val;
            break;

        case IP_OPTIONS:
            // NOTE:
            // - We allocate pool memory for holding the option data.
            // - We are not validating the IP option data here.
            if (optlen > MAXIPHDRLEN-IPHDRLEN) goto inval;
            PcbFreeIpOpts(pcb);
            if (optlen) {
                PcbSetIpOpts(pcb, optval, optlen);
            }
            break;

        case IP_HDRINCL:
            if (pcb->type != SOCK_RAW) goto noopt;
            pcb->ipHdrIncl = val;
            break;

        case IP_MULTICAST_TTL:
            if (IsTcb(pcb)) goto noopt;
            if (val < 0 || val > 255) goto inval;
            pcb->mcastTtl = (BYTE) val;
            break;

        case IP_MULTICAST_LOOP:
            if (IsTcb(pcb)) goto noopt;
            pcb->noMcastLoopback = !val;
            break;

        case IP_MULTICAST_IF:
            WinsockApiParamCheck_(optlen >= sizeof(struct in_addr));
            if (IsTcb(pcb)) goto noopt;

            err = PcbSetMcastIf(pcb, ((struct in_addr*) optval)->s_addr);
            MapNtStatusToWinsockError_(err);
            break;

        case IP_ADD_MEMBERSHIP:
        case IP_DROP_MEMBERSHIP: {
            struct ip_mreq* mreq;

            WinsockApiParamCheck_(optlen >= sizeof(struct ip_mreq));
            if (IsTcb(pcb)) goto noopt;

            mreq = (struct ip_mreq*) optval;
            err = PcbChangeMcastGroup(
                    pcb,
                    mreq->imr_multiaddr.s_addr,
                    mreq->imr_interface.s_addr,
                    optname == IP_ADD_MEMBERSHIP);

            MapNtStatusToWinsockError_(err);
            }
            break;

        default:
            goto noopt;
        }
        break;

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


int WSAAPI
getsockopt(
    SOCKET s,         
    int level,        
    int optname,      
    char* optval, 
    int* optlen  
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    INT val;
    WinsockApiPrologSockLock_(getsockopt, SOCKET_ERROR);

    WinsockApiParamCheck_(
        optval != NULL &&
        optlen != NULL &&
        *optlen > 0);

    ZeroMem(optval, *optlen);

    switch (level) {
    case SOL_SOCKET:

        switch (optname) {
        case SO_BROADCAST:
            if (IsTcb(pcb)) goto noopt;
            val = pcb->broadcast;
            break;

        case SO_DONTLINGER:
            val = !pcb->linger.l_onoff;
            break;

        case SO_LINGER:
            WinsockApiParamCheck_(*optlen >= sizeof(LINGER));
            CopyMem(optval, &pcb->linger, sizeof(LINGER));
            *optlen = sizeof(LINGER);
            goto exit;

        case SO_REUSEADDR:
            val = pcb->reuseAddr;
            break;

        case SO_EXCLUSIVEADDRUSE:
            val = pcb->exclusiveAddr;
            break;

        case SO_RCVTIMEO:
            WinsockApiParamCheck_(*optlen >= sizeof(INT));
            val = pcb->recvTimeout;
            break;

        case SO_SNDTIMEO:
            WinsockApiParamCheck_(*optlen >= sizeof(INT));
            val = pcb->sendTimeout;
            break;

        case SO_RCVBUF:
            WinsockApiParamCheck_(*optlen >= sizeof(INT));
            val = pcb->maxRecvBufsize;
            break;

        case SO_SNDBUF:
            WinsockApiParamCheck_(*optlen >= sizeof(INT));
            val = pcb->maxSendBufsize;
            break;

        case SO_TYPE:
            val = pcb->type;
            break;

        case SO_ACCEPTCONN:
            if (IsDgramPcb(pcb)) goto noopt;
            val = IsTcpListenState(pcb);
            break;

        // case SO_MAXDG:
        // case SO_MAXPATHDG:
        // case SO_OOBINLINE:
        // case SO_KEEPALIVE:
        // case SO_DONTROUTE:
        // case SO_RCVLOWAT:
        // case SO_SNDLOWAT:
        default:
            goto noopt;
        }
        break;

    case IPPROTO_TCP:
        if (IsDgramPcb(pcb)) goto noopt;

        switch (optname) {
        case TCP_NODELAY:
            val = pcb->noNagle;
            break;

        default:
            goto noopt;
        }
        break;

    case IPPROTO_IP:
        switch (optname) {
        case IP_DONTFRAGMENT:
            val = pcb->ipDontFrag;
            break;

        case IP_TTL:
            val = pcb->ipTtl;
            break;

        case IP_TOS:
            val = pcb->ipTos;
            break;

        case IP_OPTIONS:
            if (*optlen < (INT) pcb->ipoptlen)  {
                err = WSAEFAULT;
            } else if (pcb->ipopts) {
                CopyMem(optval, pcb->ipopts, pcb->ipoptlen);
            }
            *optlen = pcb->ipoptlen;
            goto exit;

        case IP_HDRINCL:
            if (pcb->type != SOCK_RAW) goto noopt;
            val = pcb->ipHdrIncl;
            break;

        case IP_MULTICAST_TTL:
            if (IsTcb(pcb)) goto noopt;
            val = pcb->mcastTtl;
            break;

        case IP_MULTICAST_LOOP:
            if (IsTcb(pcb)) goto noopt;
            val = !pcb->noMcastLoopback;
            break;

        case IP_MULTICAST_IF:
            WinsockApiParamCheck_(*optlen >= IPADDRLEN);
            if (IsTcb(pcb)) goto noopt;
            val = pcb->mcastData ? pcb->mcastData->mcastIfAddr : 0;
            break;

        // case IP_ADD_MEMBERSHIP:
        // case IP_DROP_MEMBERSHIP:
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


//
// NOTE: Special ioctlsocket command for enabling and disabling
// IP source address filtering on datagram sockets.
//
#define FIOFILTER _IOW('x', 1, u_long)


int WSAAPI
ioctlsocket(
    SOCKET s,         
    long cmd,         
    u_long* argp  
    )

/*++

Routine Description:

    Refer to XAPI SDK documentation.

--*/

{
    KIRQL irql;
    UINT bytesReady;

    WinsockApiPrologSockLock_(ioctlsocket, SOCKET_ERROR);
    WinsockApiParamCheck_(argp != NULL);

    switch (cmd) {
    case FIONBIO:
        pcb->nonblocking = *argp;
        break;

    case FIONREAD:
        irql = RaiseToDpc();
        if (IsDgramPcb(pcb)) {
            // For UDP/RAW sockets, return the size of the first buffered datagram
            // NOTE: For compatibility with win2k behavior, if the first datagram
            // is 0-sized, we'll return 1.
            if (IsDgramRecvBufEmpty(pcb)) {
                bytesReady = 0;
            } else {
                bytesReady = ((RECVBUF*) pcb->recvbuf.Flink)->datalen;
                if (bytesReady == 0) bytesReady = 1;
            }
        } else {
            // For TCP sockets, return the total number of bytes
            // available for reading.
            bytesReady = pcb->recvbufSize;
        }
        LowerFromDpc(irql);
        *argp = bytesReady;
        break;
    
    // case SIOCATMARK:
    default:
        WinsockApiGotoExit_(WSAENOPROTOOPT);
        break;
    }

    WinsockApiExitSockUnlock_(NO_ERROR, SOCKET_ERROR);
}

