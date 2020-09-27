/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: UDPHeader.hpp                                                               *
* Description: This is the implementation of the UDPHeader functions                    *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       balasha       4/20/2000    created                              *
*                       deepakp       4/20/2000    added routines to return raw buffers *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_UDP_HEADER_H__
#define __SPARTA_UDP_HEADER_H__



#include "sparta.h"
#include "packets.h"
#include "udp.h"

#include "media.h"
#include "Parser.hpp"

#include "IPConstants.h"


class CUDPHeader
{
  friend class CUDPPacket;
//  friend class CICMPHeader;

protected:

   pUDP_HDR_TYPE pUDPHeader; 
   PKT_BUFFER    m_UDPBuffer;
 
   AUTO_CAL_TYPES m_AutoChecksum;
   AUTO_CAL_TYPES m_AutoLength;

public:

   CUDPHeader();
   CUDPHeader(PVOID RawBufferdw, DWORD RemainingLength,OUT PDWORD pdwProcessed);
   SPARTA_STATUS SetSrcPort(const USHORT Port);
   SPARTA_STATUS SetDestPort(const USHORT Port);
   SPARTA_STATUS SetLength(const USHORT Length);
   SPARTA_STATUS SetChecksum(const USHORT Port);
   
   SPARTA_STATUS SetAutoCalcChecksum(const AUTO_CAL_TYPES Status);
   SPARTA_STATUS SetAutoCalcLength(const AUTO_CAL_TYPES Status);
   
   AUTO_CAL_TYPES GetAutoCalcChecksum();
   AUTO_CAL_TYPES GetAutoCalcLength();

   USHORT GetSrcPort();
   USHORT GetDstPort();
   USHORT GetLength();
   USHORT GetChecksum();

   void Print();

   /**++
	Routine Description:
		Returns the UDP internal buffer
		This is required as other classes using the the header has to chain
		the buffer in the buffer chain.
	-*/
	PKT_BUFFER GetRawBuffer();

	/**++
	Routine Description:
	Copies the first bwBytes bytes of the udp header and returns the number of bytes copied
	if bwBytes is more than 8

	-*/
	SPARTA_STATUS CopyUDPHdrFrom(PVOID copyFrom, DWORD *bwBytes);


  //SPARTA_STATUS PreparePacketForSend(const USHORT IPTotalLength, const USHORT IPHeaderLength);
   
   ~CUDPHeader();

};


#endif // __SPARTA_UDP_HEADER_H__
