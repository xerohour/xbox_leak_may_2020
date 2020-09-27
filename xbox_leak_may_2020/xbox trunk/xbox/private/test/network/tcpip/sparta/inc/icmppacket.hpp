/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: ICMPPacket.hpp                                                              *
* Description: this defines the Packet class for IP Packets, extends the MAC Packet     *
*               class                                                                   *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      5/2/2000    created                                *
*                       jbekmann      5/31/2000    made destructor virtual              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_ICMP_PACKET_H__
#define __SPARTA_ICMP_PACKET_H__

#include "sparta.h"
#include "IPHeader.hpp"
#include "IPOptions.hpp"
#include "ICMPHeader.hpp"
#include "Packet.hpp"


class CICMPPacket : public CIPPacket
{
public:
   CICMPHeader &ICMPHeader;

   CICMPPacket(MAC_MEDIA_TYPE MediaType,UCHAR type);
   CICMPPacket(PKT_PACKET Packet, CMacHeader &MacHeaderParam, CIPHeader &IPHeader,CICMPHeader &ICMPHeader);
   virtual ~CICMPPacket();

   virtual int GetPacketType();

   virtual DWORD GetMinimumPacketLength(); // override for subsequent packet types
                                           // primarily used for CICMPPacket layer

   virtual SPARTA_STATUS PreparePacketForSend();

   virtual void PrintPacket();
};

#endif // __SPARTA_ICMP_PACKET_H__

