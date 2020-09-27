/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: Parser.hpp                                                                  *
* Description: this code will parse a raw buffer into a Packet object                   *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/22/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_PARSER_H__
#define __SPARTA_PARSER_H__

#include "sparta.h"
#include "media.h"
#include "Packet.hpp"


// this is for parsing the MAC header protocol field

#define PROTOCOL_TYPE_IP4        0x0800
#define PROTOCOL_TYPE_IP6        0x86DD
#define PROTOCOL_TYPE_ARP        0x0806

#define PROTOCOL_TYPE_IGMP       0x0002
#define PROTOCOL_TYPE_ICMP       0x0001
#define PROTOCOL_TYPE_UDP        0x0011
#define PROTOCOL_TYPE_TCP        0x0006
#define PROTOCOL_TYPE_ERROR      0xFFFF


// IGMP Messages Types used for parsing only (the type and version
//  in IGMPV1 are clubbed together to form one byte)
// for version 1 and version 2 igmp packets
// for actual type constants refer to ipconstants.h
#define IGMP_DVMRP_PARSE_TYPE 0x13
#define IGMP_MEMBERSHIP_QUERY_PARSE_TYPE 0x11
#define IGMP_V2_MEMBERSHIP_REPORT_PARSE_TYPE 0x16
#define IGMP_LEAVE_GROUP_PARSE_TYPE 0x17
#define IGMP_V1_MEMBERSHIP_REPORT_PARSE_TYPE 0x12



class CParser
{
public:
    static CPacket *ParseBuffer(MAC_MEDIA_TYPE MediaType, PVOID pvBuffer, DWORD dwLength);

private:
    static CPacket *ParseRawBuffer(MAC_MEDIA_TYPE MediaType, PVOID pvBuffer, DWORD dwLength);

};

#endif // __SPARTA_PARSER_H__

