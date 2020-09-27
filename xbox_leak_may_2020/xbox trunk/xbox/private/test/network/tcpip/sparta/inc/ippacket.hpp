/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: IPPacket.hpp                                                                *
* Description: this defines the Packet class for IP Packets, extends the MAC Packet     *
*               class                                                                   *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      2/22/2000    created                               *
*                       jbekmann      5/31/2000    made destructor virtual              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_IP_PACKET_H__
#define __SPARTA_IP_PACKET_H__

#include "sparta.h"
#include "MacPacket.hpp"
#include "IPHeader.hpp"
#include "IPOptions.hpp"
#include "Packet.hpp"

#define IP_MAXIMUM_PACKET_LENGTH  65535    // 
#define IP_MINIMUM_PACKET_LENGTH  66    // with padding

class CIPPacket : public CMacPacket
{

public:
   BOOL      m_IsVersion6;
   CIPHeader &IPHeader;


   CIPPacket(MAC_MEDIA_TYPE MediaType, BOOL fIsVersion6 = FALSE);
   CIPPacket(PKT_PACKET Packet, CMacHeader &MacHeaderParam, CIPHeader &IPHeader, BOOL fIsVersion6 = FALSE);
   virtual ~CIPPacket();

   virtual int GetPacketType();

   virtual DWORD GetMinimumPacketLength(); // override for subsequent packet types
                                            // primarily used for CIPPacket layer

   virtual DWORD GetMaximumPacketLength(); // override for subsequent packet types
        

   SPARTA_STATUS PreparePacketForSend();
   SPARTA_STATUS GetIPData(PVOID pIPData, DWORD * dwDataSize);

   virtual void PrintPacket();
};

#endif // __SPARTA_IP_PACKET_H__

