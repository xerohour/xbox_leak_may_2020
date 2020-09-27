/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: MacPacket.hpp                                                               *
* Description: this defines the Packet class for Media Packets, extends the Packet class*
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/22/2000    created                              *
*                       jbekmann      5/31/2000    made destructor virtual              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_MAC_PACKET_H__
#define __SPARTA_MAC_PACKET_H__

#include "sparta.h"
#include "MacHeader.hpp"
#include "Packet.hpp"

class CMacPacket : public CPacket
{
public:
    CMacHeader &MacHeader;

    CMacPacket(MAC_MEDIA_TYPE MediaType);
    CMacPacket(PKT_PACKET Packet, CMacHeader &MacHeader);
    virtual ~CMacPacket();

    virtual int GetPacketType();

    virtual DWORD GetMinimumPacketLength(); // override for subsequent packet types
                                            // primarily used for CMacPacket layer

    virtual DWORD GetMaximumPacketLength(); // override for subsequent packet types

    virtual void PrintPacket();
};

#endif // __SPARTA_MAC_PACKET_H__

