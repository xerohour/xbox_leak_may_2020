/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: IGMPPacket.hpp                                                               *
* Description: this defines the Packet class for IGMP Packets, extends the CIPPacket class*
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      5/22/2000    created                               *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_IGMPV2_PACKET_H__
#define __SPARTA_IGMPV2_PACKET_H__

#include "sparta.h"
#include "IGMPHeaderV2.hpp"
#include "IPPacket.hpp"

class CIGMPPacketV2 : public CIPPacket
{
public:
    CIGMPHeaderV2 &IGMPHeaderV2;

    CIGMPPacketV2(MAC_MEDIA_TYPE MediaType,UCHAR igmpType);
    CIGMPPacketV2(PKT_PACKET Packet, CMacHeader &MacHeaderParam, CIPHeader &IPHeader,CIGMPHeaderV2 &IGMPHeaderV2);
    virtual ~CIGMPPacketV2();

    int GetPacketType();
	virtual SPARTA_STATUS PreparePacketForSend();

    virtual void PrintPacket();
};

#endif // __SPARTA_IGMPV2_PACKET_H__

