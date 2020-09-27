/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: IPOptions.hpp                                                               *
* Description: This is the implementation of the IPHeader functions                  *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      4/25/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_IPOPTIONS_H__
#define __SPARTA_IPOPTIONS_H__

#include "IPConstants.h"
#include "Packets.h"

class CIPOption
{

   friend class CIPHeader;
   friend class CIPPacket;

   UCHAR m_optionType;
   UCHAR m_TSOptionFlag;
   UCHAR m_IsBufferLengthSet;

protected:

   // This buffer will have the pvData memeber field pointed to the ip options
   PKT_BUFFER m_IPOptions; // we want access to these members from the IPPacket class

public:
        

   PVOID GetRawBuffer();
   // Not implemented yet: not used
   DWORD GetRawBufferLength()
   {
      return 0;
   }
        
   // Not to be exposed through COM
   PKT_BUFFER GetPktBuffer();
    

   // The IP Header is created with the IP Options and the type of the IPOptions is specified
   // as a parameter to the constructor.
   CIPOption(UCHAR IPOptionType);

   // create a Options header buffer from raw data, part of the parsing API
   // dwBytesRead will return the number of bytes used for the header
   CIPOption(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead);

   // copy constructor
   CIPOption(CIPOption& ipoption);

   ~CIPOption();

   bool IsOptionUnknown();

   // IPOptions set functions

   SPARTA_STATUS SetOptionType(UCHAR type);

   SPARTA_STATUS SetOptionBufferLength(UCHAR length);

   SPARTA_STATUS SetOptionLength(UCHAR length);

   SPARTA_STATUS SetOptionOffset(UCHAR offset);

   SPARTA_STATUS SetTSOverFlowFlag(UCHAR  oflag);
        
   SPARTA_STATUS SetTSFlag(UCHAR flag);

   SPARTA_STATUS SetIPAddrAt(UCHAR offset,const TCHAR * IP)
   {
      return SetIPAddrAt(offset, inet_addr(IP));
   }

   SPARTA_STATUS SetIPAddrAt(UCHAR offset,DWORD IP);     // address in NETWORK byte order


   SPARTA_STATUS SetTimeStampAt(UCHAR offset,unsigned long tstamp);


   // get functions

   UCHAR GetOptionType();


   UCHAR GetOptionBufferLength();

   UCHAR GetOptionLength();

   UCHAR  GetOptionOffset();


   UCHAR  GetTSOverFlowFlag();
        
   UCHAR  GetTSFlag();


   DWORD  GetIPAddrAt(UCHAR offset);   // address in NETWORK byte order

   ULONG GetTimeStampAt(UCHAR offset);


   void    Print();

};

#endif //__SPARTA_IPOPTIONS_H__
