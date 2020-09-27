/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: ArpHeader.hpp                                                               *
* Description: this defines the IP ARP Header class for Media Packets,                  *
*              extends the Packet class                                                 *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       balasha       4/18/2000    created                              *
*                       jbekmann      5/17/2000    added CMacAddress methods            *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/
#ifndef __SPARTA_ARPHEADER_H__
#define __SPARTA_ARPHEADER_H__

#include "sparta.h"
#include "packets.h"
#include "arp.h"

#include "media.h"
#include "Parser.hpp"
#include "MacPacket.hpp"
#include "MacHeader.hpp"


class CArpHeader
{
   friend class CArpPacket;

protected:

   PKT_BUFFER m_ArpBuffer;
   pARP_PKT_TYPE pArpHeader;

public:

   CArpHeader();
   CArpHeader::CArpHeader(PVOID pvRawData, DWORD dwBytesRemaining, 
                          OUT PDWORD pdwBytesRead);
   
   SPARTA_STATUS SetHardType(const USHORT Hard);
   SPARTA_STATUS SetProtType(const USHORT Prot);
   SPARTA_STATUS SetHardSize(const UCHAR  HardSize);
   SPARTA_STATUS SetProtSize(const UCHAR ProtSize);

   SPARTA_STATUS SetOpCode(const USHORT OpCode);
   SPARTA_STATUS SetSrcIP(const TCHAR * IPAddress)
   {
      return SetSrcIP(inet_addr(IPAddress));
   }
   SPARTA_STATUS SetDestIP(const TCHAR * IPAddress)
   {
      return SetDestIP(inet_addr(IPAddress));
   }

   SPARTA_STATUS SetSrcIP(unsigned long IPAddress);
   SPARTA_STATUS SetDestIP(unsigned long IPAddress);

   SPARTA_STATUS SetSrcMac(const TCHAR * MacAddress);
   SPARTA_STATUS SetDestMac(const TCHAR * MacAddress);

   SPARTA_STATUS SetSrcMac(const CMacAddress &MacAddress);
   SPARTA_STATUS SetDestMac(const CMacAddress &MacAddress);

   USHORT GetHardType();
   USHORT GetProtType();
   UCHAR  GetHardSize();
   UCHAR  GetProtSize();
   USHORT GetOpCode();
        
   //
   // values returned are in NETWORK btye order
   //
   unsigned long GetSrcIP();
   unsigned long GetDestIP();

   SPARTA_STATUS GetSrcMac(TCHAR * MacAddress,  DWORD * AddressLength);
   SPARTA_STATUS GetDestMac(TCHAR * MacAddress, DWORD * AddressLength);

   SPARTA_STATUS GetSrcMac(CMacAddress &MacAddress);
   SPARTA_STATUS GetDestMac(CMacAddress &MacAddress);


   ~CArpHeader();

   void Print();
};


#endif //__SPARTA_ARPHEADER_H__


