/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: ICMPv6Packet.hpp                                                            *
* Description: this defines the Packet class for Icmp v6 Packets, extends the MAC Packet*
*               class                                                                   *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                       deepakp      5/2/2000    created                                *
*                       jbekmann      5/31/2000    made destructor virtual              *
*                       timothyw     12/27/2000  copied, modified for ipv6              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_ICMPv6_PACKET_H__
#define __SPARTA_ICMPv6_PACKET_H__

#include "sparta.h"
#include "IPHeader.hpp"
#include "IPPacket.hpp"
//#include "IPOptions.hpp"
#include "ICMPv6Header.hpp"
#include "Packet.hpp"


class CICMPv6Packet : public CIPPacket
{
   BOOL     m_AutoCalcChecksum;

public:
   CICMPv6Header &ICMPHeader;

   CICMPv6Packet(MAC_MEDIA_TYPE MediaType, UCHAR type);
   CICMPv6Packet(PKT_PACKET Packet, 
                 CMacHeader &MacHeaderParam, 
                 CIPHeader &IPHeader,
                 CICMPv6Header &ICMPHeader);

   virtual ~CICMPv6Packet();

   virtual int GetPacketType();

   virtual DWORD GetMinimumPacketLength(); // override for subsequent packet types
                                           // primarily used for CICMPPacket layer

   virtual SPARTA_STATUS PreparePacketForSend();

   virtual void PrintPacket();

   SPARTA_STATUS  SetAutoCalcChecksum(BOOL  status);

   USHORT   CalcChecksum();
};

#endif // __SPARTA_ICMPv6_PACKET_H__

