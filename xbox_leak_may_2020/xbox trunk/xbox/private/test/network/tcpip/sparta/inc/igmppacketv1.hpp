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

#ifndef __SPARTA_IGMPV1_PACKET_H__
#define __SPARTA_IGMPV1_PACKET_H__

#include "sparta.h"
#include "IGMPHeaderV1.hpp"
#include "IPPacket.hpp"

class CIGMPPacketV1 : public CIPPacket
{
public:
    CIGMPHeaderV1 &IGMPHeaderV1;

    CIGMPPacketV1(MAC_MEDIA_TYPE MediaType,UCHAR igmpType);
    CIGMPPacketV1(PKT_PACKET Packet, CMacHeader &MacHeaderParam, CIPHeader &IPHeader,CIGMPHeaderV1 &IGMPHeaderV1);
    virtual ~CIGMPPacketV1();

    int GetPacketType();
	virtual SPARTA_STATUS PreparePacketForSend();

    virtual void PrintPacket();
};

#endif // __SPARTA_IGMPV1_PACKET_H__

