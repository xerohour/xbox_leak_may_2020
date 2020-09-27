/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    enumprot.c

Abstract:

    Implement the Winsock WSAEnumProtocols API

Revision History:

    06/01/2000 davidx
        Created it.

--*/

#include "precomp.h"


//
// Hard-coded TCP/IP protocol information
//
#define NUM_TCPIP_PROTOCOLS 3
#define MAX_UDP_MSGSIZE (MAXIPLEN-MAXIPHDRLEN-UDPHDRLEN)

const WSAPROTOCOL_INFOW TcpIpProtocols[NUM_TCPIP_PROTOCOLS] = {
    //
    // TCP
    //
    {
        XP1_GUARANTEED_DELIVERY     // dwServiceFlags1
            | XP1_GUARANTEED_ORDER
            | XP1_GRACEFUL_CLOSE,
        0,                          // dwServiceFlags2
        0,                          // dwServiceFlags3
        0,                          // dwServiceFlags4
        PFL_MATCHES_PROTOCOL_ZERO,  // dwProviderFlags
        {                           // ProviderId
            0xe70f1aa0,
            0xab8b,
            0x11cf,
            { 0x8c, 0xa3, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92 }
        },
        1001,                       // dwCatalogEntryId
        { BASE_PROTOCOL, },         // ProtocolChain
        2,                          // iVersion
        AF_INET,                    // iAddressFamily
        SOCKADDRLEN,                // iMaxSockAddr
        SOCKADDRLEN,                // iMinSockAddr
        SOCK_STREAM,                // iSocketType
        IPPROTO_TCP,                // iProtocol
        0,                          // iProtocolMaxOffset
        BIGENDIAN,                  // iNetworkByteOrder
        SECURITY_PROTOCOL_NONE,     // iSecurityScheme
        0,                          // dwMessageSize
        0,                          // dwProviderReserved
        L"MSAFD Tcpip [TCP/IP]"     // szProtocol
    },

    //
    // UDP
    //
    {
        XP1_CONNECTIONLESS          // dwServiceFlags1
            | XP1_MESSAGE_ORIENTED
            | XP1_SUPPORT_BROADCAST,
        0,                          // dwServiceFlags2
        0,                          // dwServiceFlags3
        0,                          // dwServiceFlags4
        PFL_MATCHES_PROTOCOL_ZERO,  // dwProviderFlags
        {                           // ProviderId
            0xe70f1aa0,
            0xab8b,
            0x11cf,
            { 0x8c, 0xa3, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92 }
        },
        1002,                       // dwCatalogEntryId
        { BASE_PROTOCOL, },         // ProtocolChain
        2,                          // iVersion
        AF_INET,                    // iAddressFamily
        SOCKADDRLEN,                // iMaxSockAddr
        SOCKADDRLEN,                // iMinSockAddr
        SOCK_DGRAM,                 // iSocketType
        IPPROTO_UDP,                // iProtocol
        0,                          // iProtocolMaxOffset
        BIGENDIAN,                  // iNetworkByteOrder
        SECURITY_PROTOCOL_NONE,     // iSecurityScheme
        MAX_UDP_MSGSIZE,            // dwMessageSize
        0,                          // dwProviderReserved
        L"MSAFD Tcpip [UDP/IP]"     // szProtocol
    },

    //
    // Raw IP
    //
    {
        XP1_CONNECTIONLESS          // dwServiceFlags1
            | XP1_MESSAGE_ORIENTED
            | XP1_SUPPORT_BROADCAST,
        0,                          // dwServiceFlags2
        0,                          // dwServiceFlags3
        0,                          // dwServiceFlags4
        PFL_MATCHES_PROTOCOL_ZERO,  // dwProviderFlags
        {                           // ProviderId
            0xe70f1aa0,
            0xab8b,
            0x11cf,
            { 0x8c, 0xa3, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92 }
        },
        1003,                       // dwCatalogEntryId
        { BASE_PROTOCOL, },         // ProtocolChain
        2,                          // iVersion
        AF_INET,                    // iAddressFamily
        SOCKADDRLEN,                // iMaxSockAddr
        SOCKADDRLEN,                // iMinSockAddr
        SOCK_RAW,                   // iSocketType
        0,                          // iProtocol
        255,                        // iProtocolMaxOffset
        BIGENDIAN,                  // iNetworkByteOrder
        SECURITY_PROTOCOL_NONE,     // iSecurityScheme
        MAX_UDP_MSGSIZE,            // dwMessageSize
        0,                          // dwProviderReserved
        L"MSAFD Tcpip [RAW/IP]"     // szProtocol
    }
};


INT
MatchTcpipProtocol(
    IN INT af,
    IN INT type,
    IN INT protocol,
    OUT const WSAPROTOCOL_INFOW** protoinfo
    )

/*++

Routine Description:

    Match the specified parameters to a TCP/IP protocol

Arguments:

    af - Supplies an address family specification
    type - Supplies a socket type specification
    protocol - Supplies an address family specific identification of a
        protocol to be used with a socket, or 0 if the caller does
        not wish to specify a protocol.
    protoinfo - Returns the matched protocol information

Return Value:

    Error code

--*/

#define MATCHED_NONE 0
#define MATCHED_TYPE 1
#define MATCHED_TYPE_FAMILY 2
#define MATCHED_TYPE_FAMILY_PROTOCOL 3

#define PROTOCOL_IN_RANGE(_info, _proto) \
        ((_proto) >= (_info)->iProtocol && \
         (_proto) <= (_info)->iProtocol + (_info)->iProtocolMaxOffset)

{
    const WSAPROTOCOL_INFOW* p = TcpIpProtocols;
    const WSAPROTOCOL_INFOW* matched_proto = NULL;
    INT match = MATCHED_NONE;
    INT count = NUM_TCPIP_PROTOCOLS;
    INT err;

    // Either address family or protocol must be specified
    *protoinfo = NULL;
    if (af == 0 && protocol == 0) return WSAEINVAL;

    while (count-- && match < MATCHED_TYPE_FAMILY_PROTOCOL) {
        // Does the socket type match?

        if ((p->iSocketType == type) || (type == 0)) {
            if (match < MATCHED_TYPE) {
                match = MATCHED_TYPE;
                matched_proto = p;
            }

            // Can it support the requested address family?
            // Or is the wildcard family specified?

            if ((p->iAddressFamily == af) || (af == 0)) {
                if (match < MATCHED_TYPE_FAMILY) {
                    match = MATCHED_TYPE_FAMILY;
                    matched_proto = p;
                }

                // Is the requested protcol in range? Or is the requested
                // protocol 0 and entry supports protocol 0?

                if (PROTOCOL_IN_RANGE(p, protocol) || protocol == 0) {
                    match = MATCHED_TYPE_FAMILY_PROTOCOL;
                    matched_proto = p;
                }
            }
        }

        p++;
    }

    // Select an appropriate error code for "no match" cases,
    // or success code to proceed.

    switch (match) {
    case MATCHED_NONE:
        err = WSAESOCKTNOSUPPORT;
        break;

    case MATCHED_TYPE:
        err = WSAEAFNOSUPPORT;
        break;

    case MATCHED_TYPE_FAMILY_PROTOCOL:
        // NOTE: we don't allow raw sockets with TCP or UDP protocol
        if (type != SOCK_RAW ||
            protocol != IPPROTO_TCP && protocol != IPPROTO_UDP) {
            err = NO_ERROR;
            break;
        }

        // fall through

    case MATCHED_TYPE_FAMILY:
        err = WSAEPROTONOSUPPORT;
        break;

    default:
        err = WSASYSCALLFAILURE;
        break;
    }

    *protoinfo = (LPWSAPROTOCOL_INFOW) matched_proto;
    return err;
}

