/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: spartaAPI.h                                                                 *
* Description: includes all the API header files needed by users of the API             *
*              (not intended for use by files which implement the API)                  *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/18/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_SPARTAAPI_H__
#define __SPARTA_SPARTAAPI_H__

#include "sparta.h"     // includes windows etc...

#include "memory.h"     // the .h files are for using the C API functions
#include "packets.h"
#include "media.h"
#include "driverapi.h"

#include "Packet.hpp"   // the .hpp are for using the C++ API functions
#include "Interface.hpp"
#include "SpartaException.hpp"
#include "MacAddress.hpp"
#include "MacPacket.hpp"
#include "ArpPacket.hpp"
#include "IPPacket.hpp"
#include "IPHeader.hpp"
#include "UDPHeader.hpp"
#include "UDPPacket.hpp"
#include "TCPHeader.hpp"
#include "TCPOption.hpp"
#include "TCPPacket.hpp"
#include "ICMPPacket.hpp"
#include "ICMPHeader.hpp"
#include "Parser.hpp"
#include "IGMPPacketV2.hpp"
#include "IGMPPacketV1.hpp"
#include "icmpv6header.hpp"
#include "icmpv6packet.hpp"
#include "AutoArp.hpp"

//#include "Parser.hpp" I don't think this is needed

#endif // __SPARTA_SPARTAAPI_H__
