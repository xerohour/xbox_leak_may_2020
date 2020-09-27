/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    ethernet.h

Abstract:

    Ethernet and ARP protocol related declarations

Notes:

    Please refer to RFC 894, 826, and 1042.
    Also see RFC 1122, section 2.3.2 on ARP.

Revision History:

    05/02/2000 davidx
        Created it.

--*/

#ifndef _ETHERNET_H
#define _ETHERNET_H

//
// Ethernet hardware address is 6 bytes (48 bits)
//
#define ENETADDRLEN 6

//
// Ethernet frame header
//
#include <pshpack1.h>

typedef struct _EnetFrameHeader {
    BYTE dstaddr[ENETADDRLEN];
    BYTE srcaddr[ENETADDRLEN];
    WORD etherType;
} EnetFrameHeader;

#include <poppack.h>

//
// Ethernet frame header is 14 bytes
//
#define ENETHDRLEN sizeof(EnetFrameHeader)

//
// Mininum and maximum data size in an Ethernet frame
//
#define ENET_MINDATASIZE 46
#define ENET_MAXDATASIZE 1500

//
// Ethernet frame types
//
#define ENETTYPE_IP         0x0800
#define ENETTYPE_ARP        0x0806
#define ENETTYPE_LOOPBACK   0x9000

//
// IEEE 802.LLC and SNAP headers
//
#include <pshpack1.h>

typedef struct _IEEE802Header {
    BYTE DSAP;          // 0xaa
    BYTE SSAP;          // 0xaa
    BYTE control;       // 0x03
    BYTE orgcode[3];    // 0x00 0x00 0x00
    WORD etherType;
} IEEE802Header;

#define IEEE802HDRLEN sizeof(IEEE802Header)

#include <poppack.h>

//
// Constants we expect in the IEEE header
//
#define SNAP_DSAP 0xaa
#define SNAP_SSAP 0xaa
#define LLC_DGRAM 3

//
// Check if an Ethernet frame is actually an 802.3 frame
//
INLINE BOOL IsIEEE802Frame(WORD etherType) {
    return etherType <= ENET_MAXDATASIZE;
}

//
// Determine if an Ethernet hardware address is
// a broadcast or a multicast address
//
extern const BYTE EnetBroadcastAddr[ENETADDRLEN];

INLINE BOOL IsEnetAddrMcast(const BYTE* enetAddr) {
    return (enetAddr[0] & 1) != 0;
}

//
// ARP packet format
//
#include <pshpack1.h>

typedef struct _ArpPacket {
    WORD hrd;               // hardware address space
    WORD pro;               // protocol address space: ENETTYPE_IP
    BYTE hln;               // hardware address length: 6
    BYTE pln;               // protocol address length: 4
    WORD op;                // opcode
    BYTE sha[ENETADDRLEN];  // sender's hardware address
    IPADDR spa;             // sender's protocol address
    BYTE tha[ENETADDRLEN];  // target's hardware address
    IPADDR tpa;             // target's protocol address
} ArpPacket;

#include <poppack.h>

//
// Size of an ARP packet
//
#define ARPPKTLEN sizeof(ArpPacket)

//
// ARP packet opcodes
//
#define ARP_REQUEST 1
#define ARP_REPLY   2

//
// ARP hardware address spaces
//
#define ARP_HWTYPE_ENET 1
#define ARP_HWTYPE_802  6

#endif // !_ETHERNET_H

