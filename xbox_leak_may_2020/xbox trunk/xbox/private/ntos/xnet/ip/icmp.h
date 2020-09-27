/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    icmp.h

Abstract:

    ICMP protocol related declarations.

Notes:

    Please refer to RFC 792 and 1256.

Revision History:

    05/02/2000 davidx
        Created it.

--*/

#ifndef _ICMP_H
#define _ICMP_H

//
// ICMP message types and codes
//
#define ICMPTYPE_DESTINATION_UNREACHABLE    3
#define   ICMPCODE_NET_UNREACHABLE          0
#define   ICMPCODE_HOST_UNREACHABLE         1
#define   ICMPCODE_PROTOCOL_UNREACHABLE     2
#define   ICMPCODE_PORT_UNREACHABLE         3
#define   ICMPCODE_CANT_FRAGMENT_WITH_DF    4
#define   ICMPCODE_SRCROUTE_FAILED          5
#define   ICMPCODE_DEST_NET_UNKNOWN         6
#define   ICMPCODE_DEST_HOST_UNKNOWN        7
#define   ICMPCODE_SRC_HOST_ISOLATED        8
#define   ICMPCODE_DEST_NET_PROHIBITED      9
#define   ICMPCODE_DEST_HOST_PROHIBITED     10
#define   ICMPCODE_NET_UNREACHABLE_TOS      11
#define   ICMPCODE_HOST_UNREACHABLE_TOS     12
#define ICMPTYPE_TIME_EXECEEDED             11
#define   ICMPCODE_TTL_EXCEEDED             0
#define   ICMPCODE_REASSEMBLY_TIMEOUT       1
#define ICMPTYPE_PARAMETER_PROBLEM          12
#define   ICMPCODE_PTR_INDICATES_ERROR      0
#define   ICMPCODE_MISSING_REQD_OPTION      1
#define ICMPTYPE_SOURCE_QUENCH              4
#define ICMPTYPE_REDIRECT                   5
#define   ICMPCODE_REDIRECT_NET             0
#define   ICMPCODE_REDIRECT_HOST            1
#define   ICMPCODE_REDIRECT_NET_TOS         2
#define   ICMPCODE_REDIRECT_HOST_TOS        3
#define ICMPTYPE_ECHO_REQUEST               8
#define ICMPTYPE_ECHO_REPLY                 0
#define ICMPTYPE_TIMESTAMP_REQUEST          13
#define ICMPTYPE_TIMESTAMP_REPLY            14
#define ICMPTYPE_INFO_REQUEST               15
#define ICMPTYPE_INFO_REPLY                 16

//
// Generic ICMP message format
//
typedef struct _IcmpMessage {
    BYTE type;          // ICMP message type
    BYTE code;          // type specific code
    WORD checksum;      // ICMP checksum
    union {             // type/code specific parameters
        struct {        // - for request/reply messages:
            WORD id;    //   identifier
            WORD seqno; //   sequence number
        };
        IPADDR gwaddr;  // - redirect message: gateway address
        BYTE info[4];   // - for other messages
        DWORD unused;   //
    };
    BYTE origdata[1];   // original IP header + some data
} IcmpMessage;

// ICMP message header length
#define ICMPHDRLEN offsetof(IcmpMessage, origdata)

#endif // !_ICMP_H

