/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    igmp.h

Abstract:

    IGMP protocol related declarations.

Notes:

    Please refer to RFC 1112 and 2236.

Revision History:

    05/02/2000 davidx
        Created it.

--*/

#ifndef _IGMP_H
#define _IGMP_H

//
// IGMP v2 message header format
//
typedef struct _IgmpMessage {
    BYTE type;              // message type
    BYTE maxresptime;       // max response time
    WORD checksum;          // checksum
    IPADDR groupaddr;       // multicast group address
} IgmpMessage;

#define IGMPHDRLEN sizeof(IgmpMessage)

//
// Message type constants
//
#define IGMPTYPE_MEMBER_QUERY    0x11   // membership query
#define IGMPTYPE_MEMBER_REPORT_1 0x12   // membership report (version 1)
#define IGMPTYPE_MEMBER_REPORT_2 0x16   // membership report (version 2)
#define IGMPTYPE_LEAVE_GROUP     0x17   // leave group

//
// When we join a new group, we'll send out two membership report messages.
// The second report is sent out 2 seconds after the first one.
//
#define IGMP_JOIN_RETRY_TIMER   2

//
// IGMPv1 query response timer (10 seconds)
//
#define IGMP_V1_QUERY_RESPONSE  10

//
// How long to wait after hearing an IGMPv1 query
// before we can send IGMPv2 messages again.
//
#define IGMP_V1_QUERIER_TIMER   400

#endif // !_IGMP_H

