//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1993-1999. All rights reserved.
//
//  MODULE: nmtcpstructs.h
//
//  Definitions/declarations for parsing TCP frames.
//=============================================================================

#ifndef NMTCPSTRUCTS_H
#define NMTCPSTRUCTS_H

#ifdef __cplusplus
extern "C"
{
#endif

//
// TCP Packet Structure
//

typedef struct _TCP 
{
    WORD  SrcPort;
    WORD  DstPort;
    DWORD SeqNum;
    DWORD AckNum;
    BYTE  DataOff;
    BYTE  Flags;
    WORD  Window;
    WORD  Chksum;
    WORD  UrgPtr;
} TCP;

typedef TCP * LPTCP;
typedef TCP UNALIGNED * ULPTCP;

INLINE DWORD TCP_HdrLen(ULPTCP pTCP)
{
    return (pTCP->DataOff & 0xf0) >> 2;
}

INLINE DWORD TCP_SrcPort(ULPTCP pTCP)
{
    return XCHG(pTCP->SrcPort);
}

INLINE DWORD TCP_DstPort(ULPTCP pTCP)
{
    return XCHG(pTCP->DstPort);
}

//
// TCP Option Opcodes
//

#define TCP_OPTION_ENDOFOPTIONS   0
#define TCP_OPTION_NOP            1
#define TCP_OPTION_MAXSEGSIZE     2
#define TCP_OPTION_WSCALE         3
#define TCP_OPTION_SACK_PERMITTED 4
#define TCP_OPTION_SACK           5
#define TCP_OPTION_TIMESTAMPS     8

//
// TCP Flags
//

#define TCP_FLAG_URGENT     0x20
#define TCP_FLAG_ACK        0x10
#define TCP_FLAG_PUSH       0x08
#define TCP_FLAG_RESET      0x04
#define TCP_FLAG_SYN        0x02
#define TCP_FLAG_FIN        0x01

//
// TCP Field Masks
//

#define TCP_RESERVED_MASK   0x0FC0

#ifdef __cplusplus
}
#endif

#endif