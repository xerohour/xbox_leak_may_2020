/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: TCPHeader.hpp                                                               *
* Description: This is the implementation of the TCPHeader functions                    *
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
#ifndef __SPARTA_TCP_HEADER_H__
#define __SPARTA_TCP_HEADER_H__


#include "sparta.h"
#include "packets.h"
#include "tcp.h"

#include "media.h"
#include "Parser.hpp"
#include "TCPOption.hpp"

#include "IPConstants.h"


class CTCPHeader
{
  friend class CTCPPacket;
  //friend class CTCPOption;
//  friend class CICMPHeader;

protected:

   pTCP_HDR_TYPE pTCPHeader; 
   PKT_BUFFER    m_TCPBuffer;
 
   AUTO_CAL_TYPES m_AutoChecksum;
   AUTO_CAL_TYPES m_AutoLength;
   
   USHORT  TotalOptionsLength;
public:

   CTCPOption Options[MAX_NO_TCP_OPTIONS];
   
   CTCPHeader();
   
   CTCPHeader(PVOID RawBuffer, DWORD RemainingLength,OUT PDWORD pdwProcessed);
	/**
		This Contructor is only used by icmp class when creating the tcp header as only eight
		bytes can provided for the the construction.
		In This case the rest of the bytes reading will result in zero getting returned
		dwRemainingLength will never be greater than the max bytes required to make a tcp packet.
		This will be ensured by who ever is calling this function
		dwRemainingLength will usually be eight.
	*/
	CTCPHeader::CTCPHeader(PVOID RawBuffer, PDWORD pdwRemainingLength,OUT PDWORD pdwProcessed);
   
   USHORT GetActualHdrLength();
   SPARTA_STATUS PrepareHdrForSend();
   
   
   SPARTA_STATUS SetSrcPort(const USHORT Port);
   
   SPARTA_STATUS SetDestPort(const USHORT Port);
   
   SPARTA_STATUS SetSeqNumber(const int SeqNumber);
   
   SPARTA_STATUS SetAckNumber(const int AckNumber);

   
   SPARTA_STATUS SetHdrLength(const USHORT Length);
   
   SPARTA_STATUS SetReserved(const USHORT Reserved);   
   
   SPARTA_STATUS SetFlag(const USHORT Flag);
   
   SPARTA_STATUS SetWindowSize(const USHORT WindowSize);
   
   SPARTA_STATUS SetChecksum(const USHORT Checksum);
   
   SPARTA_STATUS SetUrgentPointer(const USHORT UrgentPointer);
   
   SPARTA_STATUS SetAutoCalcChecksum(const AUTO_CAL_TYPES Status);
   SPARTA_STATUS SetAutoCalcLength(const AUTO_CAL_TYPES Status);
   
/*********************************************************************************
*                               Get Functions                                   *
*********************************************************************************/

   USHORT GetSrcPort();
   USHORT GetDstPort();
   int    GetSeqNumber();
   int    GetAckNumber();
   
   USHORT GetHdrLength();
   USHORT GetReserved();
   USHORT GetFlag();
   
   USHORT GetWindowSize();
   USHORT GetChecksum();
   USHORT GetUrgentPointer();

   AUTO_CAL_TYPES GetAutoCalcChecksum();
   AUTO_CAL_TYPES GetAutoCalcLength();


   void          Print();
   SPARTA_STATUS ProcessOptions(UCHAR* OptionPointer, USHORT TotalOptionsLength);

   /**++
        Routine Description:
                Returns the TCP internal buffer
                This is required as other classes using the the header has to chain
                the buffer in the buffer chain.
        -*/
        PKT_BUFFER GetRawBuffer();

        /**++
        Routine Description:
        Copies the first bwBytes bytes of the tcp header and returns the number of bytes copied
        if bwBytes is more than 8

        -*/
        SPARTA_STATUS CopyTCPHdrFrom(PVOID copyFrom, DWORD *bwBytes);


  //SPARTA_STATUS PreparePacketForSend(const USHORT IPTotalLength, const USHORT IPHeaderLength);
   
   ~CTCPHeader();

};


#endif // __SPARTA_TCP_HEADER_H__
