/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tcp.h

Abstract:

    TCP/UDP protocol related declarations.

Notes:

    Please refer to RFC 793, 768, and 2581.

Revision History:

    05/02/2000 davidx
        Created it.

--*/

#ifndef _TCP_H
#define _TCP_H

//
// UDP datagram header format
//
typedef struct _UdpHeader {
    IPPORT srcport;     // source port
    IPPORT dstport;     // destination port
    WORD length;        // datagram length (incl. header)
    WORD checksum;      // datagram checksum
} UdpHeader;

// UDP header length
#define UDPHDRLEN sizeof(UdpHeader)

//
// TCP/UDP pseudo-header structure
//
typedef struct _PseudoHeader {
    IPADDR srcaddr;
    IPADDR dstaddr;
    BYTE zero;
    BYTE protocol;
    WORD length;
} PseudoHeader;

//
// TCP segment header format
//
typedef struct _TcpHeader {
    IPPORT srcport;     // source port
    IPPORT dstport;     // destination port
    DWORD seqnum;       // sequence number
    DWORD acknum;       // acknowledgement number
    BYTE hdrlen;        // header length (# of DWORDs)
    BYTE flags;         // flags
    WORD window;        // window
    WORD checksum;      // checksum
    WORD urgent;        // urgent pointer
} TcpHeader;

// Default TCP header length (without options)
#define TCPHDRLEN sizeof(TcpHeader)
#define MAXTCPHDRLEN (4*15)

// Extract the TCP header length (in bytes)
#define GETTCPHDRLEN(_tcphdr) (((_tcphdr)->hdrlen & 0xf0) >> 2)
#define SETTCPHDRLEN(_tcphdr, _hdrlen) ((_tcphdr)->hdrlen = (BYTE) (ROUNDUP4(_hdrlen) << 2))

// TCP segment flag bits
#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20

// Control flags
#define TCP_CONTROLS (TCP_SYN|TCP_FIN|TCP_RST) 

// convenience macro for checking segment flags
#define ISTCPSEG(_tcphdr, _flag) ((_tcphdr)->flags & TCP_##_flag)

//
// TCP option tags
//
#define TCPOPT_EOL              0x00
#define TCPOPT_NOP              0x01
#define TCPOPT_MAX_SEGSIZE      0x02
#define TCPOPT_WINDOW_SCALE     0x03
#define TCPOPT_SACK_PERMITTED   0x04
#define TCPOPT_SACK             0x05
#define TCPOPT_TIMESTAMP        0x08

// Default MSS (maximum segment size)
//  576 - 20 (IP header) - 20 (TCP header)
#define TCP_DEFAULT_MSS 536

#endif // !_TCP_H

