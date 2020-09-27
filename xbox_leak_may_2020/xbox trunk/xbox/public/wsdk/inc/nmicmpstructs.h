//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1993-1999. All rights reserved.
//
//  MODULE: nmicmpstructs.h
//
//  Definitions/#defines for parsing ICMP frames.
//=============================================================================

#ifndef NMICMPSTRUCTS_H
#define NMICMPSTRUCTS_H

#include <nmipstructs.h>

#ifdef __cplusplus
extern "C"
{
#endif

//
// ICMP Frame Structure
//

typedef struct _RequestReplyFields 
{
   WORD ID;
   WORD SeqNo;
} ReqReply;

typedef struct _ParameterProblemFields 
{
   BYTE Pointer;
   BYTE junk[3];
} ParmProb;

typedef struct _TimestampFields 
{
   DWORD tsOrig;
   DWORD tsRecv;
   DWORD tsXmit;
} TS;

typedef struct _RouterAnnounceHeaderFields
{
    BYTE NumAddrs;
    BYTE AddrEntrySize;
    WORD Lifetime;
} RouterAH;

typedef struct _RouterAnnounceEntry
{
    DWORD Address;
    DWORD PreferenceLevel;
} RouterAE;

typedef struct _ICMP 
{
   BYTE Type;
   BYTE Code;
   WORD Checksum;
   union
   {
      DWORD    Unused;
      DWORD    Address;
      ReqReply RR;
      ParmProb PP;
      RouterAH RAH;     
   };

   union
   {
      TS       Time;
      IP       IP;
#pragma warning(disable:4200)
      RouterAE RAE[0];
#pragma warning(default:4200)
   };
} ICMP;

typedef ICMP * LPICMP;
typedef ICMP UNALIGNED * ULPICMP;

#define ICMP_HEADER_LENGTH (8)
#define ICMP_IP_DATA_LENGTH   (8)   // # of *BYTES* of IP data to attach to
                                    // datagram in addition to IP header
//
// ICMP Packet Types
//

#define ECHO_REPLY                     0
#define DESTINATION_UNREACHABLE        3
#define SOURCE_QUENCH                  4
#define REDIRECT                       5
#define ECHO                           8
#define ROUTER_ADVERTISEMENT           9
#define ROUTER_SOLICITATION           10
#define TIME_EXCEEDED                 11
#define PARAMETER_PROBLEM             12
#define TIMESTAMP                     13
#define TIMESTAMP_REPLY               14
#define INFORMATION_REQUEST           15
#define INFORMATION_REPLY             16
#define ADDRESS_MASK_REQUEST          17
#define ADDRESS_MASK_REPLY            18

#ifdef __cplusplus
}
#endif

#endif NMICMPSTRUCTS_H
