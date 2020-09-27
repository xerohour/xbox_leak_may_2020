/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: Ip6Packet.hpp                                                               *
* Description: this defines the Packet class for IPv6 Packets, extends the Media Packet *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann     8/24/2000    created                               *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_IP6_PACKET_H__
#define __SPARTA_IP6_PACKET_H__

#include "sparta.h"
#include "MacPacket.hpp"
#include "Ipv6Header.hpp"
#include "Packet.hpp"

#define IP6_MAXIMUM_PACKET_LENGTH  65535    // 
#define IP6_MINIMUM_PACKET_LENGTH  66	// with padding

class CIPv6Packet : public CMacPacket
{
public:
    CIPv6Header &IPv6Header;

    CIPv6Packet(MAC_MEDIA_TYPE MediaType);
    CIPv6Packet(PKT_PACKET Packet, CMacHeader &MacHeaderParam, CIPv6Header &IPv6Header);
    virtual ~CIPv6Packet();

    virtual int GetPacketType();

    virtual DWORD GetMinimumPacketLength(); // override for subsequent packet types
                                            // primarily used for CIPv6Packet layer

    virtual DWORD GetMaximumPacketLength(); // override for subsequent packet types
	


	SPARTA_STATUS PreparePacketForSend();

//	SPARTA_STATUS GetIPData(PVOID pIPData, DWORD * dwDataSize);

    virtual void PrintPacket();
};

#endif // __SPARTA_IP6_PACKET_H__

