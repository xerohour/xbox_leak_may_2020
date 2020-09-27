/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: TCPPacket.hpp                                                               *
* Description: this defines the TCP Packet class , extends the IP Packet                *
*               class                                                                   *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       balasha      3/20/2000    created                               *
*                       jbekmann      5/31/2000    made destructor virtual              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/


#ifndef __SPARTA_TCP_PACKET_H__
#define __SPARTA_TCP_PACKET_H__

#include "sparta.h"
#include "udp.h"
#include "IPHeader.hpp"
#include "Packet.hpp"
#include "TCPHeader.hpp"
#include "IPPacket.hpp"


class CTCPPacket : public CIPPacket
{
  
public:
   CTCPHeader & TCPHeader;

   CTCPPacket(PKT_PACKET Packet, CMacHeader &MacHeaderParam, CIPHeader &IPHeader, 
              CTCPHeader &TCPHeader, BOOL IsVersion6 = FALSE);
   CTCPPacket(MAC_MEDIA_TYPE MediaType, BOOL IsVersion6 = FALSE);
   SPARTA_STATUS PreparePacketForSend();
   USHORT GetHdrLength();
   USHORT CalcChecksum();
    
   BOOL VerifyChecksum();
   
   virtual ~CTCPPacket();

   virtual int GetPacketType();

   virtual void PrintPacket();

};

#endif //__SPARTA_UDP_PACKET_H__
