/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    ip.h

Abstract:

    IPv4 protocol related declarations

Notes:

    Please refer to RFC 791, 950, and 1519.

Revision History:

    05/02/2000 davidx
        Created it.

--*/

#ifndef _IP_H
#define _IP_H

//
// IPv4 datagram header
//
typedef struct _IpHeader {
    BYTE ver_hdrlen;    // version & header length
    BYTE tos;           // type of service
    WORD length;        // total length
    WORD id;            // identification
    WORD fragoffset;    // flags & fragment offset
    BYTE ttl;           // time to live
    BYTE protocol;      // protocol
    WORD hdrxsum;       // header checksum
    IPADDR srcaddr;     // source address
    IPADDR dstaddr;     // destination address
} IpHeader;

// IP datagram header length without options (20 bytes)
#define IPHDRLEN sizeof(IpHeader)

// Maximum IP header length (60 bytes)
#define MAXIPHDRLEN (15*4)

// Maximum IP datagram length
#define MAXIPLEN 0xffff

// Check IP version and get header length (in bytes)
#define IPVER4 0x40
#define VERIFY_IPVER_HDRLEN(_verhdrlen) \
        ((((_verhdrlen) & 0xf0) == IPVER4) ? (((_verhdrlen) & 0x0f) << 2) : 0)

#define GETIPHDRLEN(_iphdr) (((_iphdr)->ver_hdrlen & 0x0f) << 2)
#define GETIPLEN(_iphdr)    NTOHS((_iphdr)->length)

//
// Bit assignment for the flags & fragment offset field
//  bit 15: reserved
//  bit 14: 1 = don't fragment
//  bit 13: 1 = more fragments
//  bit 12-0: fragment offset (in units of 8 bytes)
//
#define DONT_FRAGMENT   BIT(14)
#define MORE_FRAGMENTS  BIT(13)
#define FRAGOFFSET_MASK 0x1fff

//
// IP option tags
//  bit 7: 1 = option copied into fragments
//  bit 6-5: option class
//  bit 4-0: option number
//
#define IPOPT_EOL               0x00    // no copy
#define IPOPT_NOP               0x01    // no copy
#define IPOPT_SECURITY          0x82    // copy, control
#define IPOPT_LOOSE_SRCROUTE    0x83    // copy, control
#define IPOPT_RECORD_ROUTE      0x07    // no copy, control
#define IPOPT_STREAM_ID         0x88    // copy, control
#define IPOPT_STRICT_SRCROUTE   0x89    // copy, control
#define IPOPT_TIMESTAMP         0x44    // no copy, debug

//
// IP protocol numbers
//
#define IPPROTOCOL_ICMP 1
#define IPPROTOCOL_IGMP 2
#define IPPROTOCOL_TCP  6
#define IPPROTOCOL_UDP  17

#endif // !_IP_H

