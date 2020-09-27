/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: ArpPacket.hpp                                                               *
* Description: this defines the IP ARP Packet class for Media Packets,                  *
*              extends the Packet class                                                 *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       balasha       4/03/2000    created                              *
*                       jbekmann      5/31/2000    made destructor virtual              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_ARPPACKET_H__
#define __SPARTA_ARPPACKET_H__

#include "sparta.h"
#include "arp.h"
#include "MacHeader.hpp"
#include "Packet.hpp"
#include "ArpHeader.hpp"
#include "MacPacket.hpp"


class CArpPacket : public CMacPacket
{

public:
	CArpHeader & ArpHeader;

	/**++
	C'tor Description:
		This ctor will be used by the parser
	--*/
	CArpPacket(PKT_PACKET Packet, CMacHeader &MacHeaderParam, CArpHeader &ArpHeaderParam);
	
	virtual int GetPacketType();

	CArpPacket(MAC_MEDIA_TYPE MediaType);
	virtual ~CArpPacket();
   
    virtual void PrintPacket();
};

#endif // __SPARTA_ARPPACKET_H__

