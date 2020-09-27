/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: IGMPHeaderV2.hpp                                                               *
* Description: This is the implementation of the MediaHeader functions                  *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      5/22/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_IGMPHeaderV2_H__
#define __SPARTA_IGMPHeaderV2_H__

#include "sparta.h"
#include "packets.h"
#include "IPConstants.h"


typedef struct
{
   UCHAR igmp_type;
   UCHAR igmp_code;
   USHORT igmp_chksum;
   ULONG igmp_group;
} tIGMPHeaderV2, *IGMPHeaderV2;



class CIGMPHeaderV2
{

   friend class CIGMPPacketV2;
        
   AUTO_CAL_TYPES m_autoCalcIGMPChecksum;
   IGMPHeaderV2 m_pIGMPHeaderV2;

protected:

   PKT_BUFFER m_IGMPHeaderV2; // we want access to these members from the CIGMPPacketV2 class

public:

   PVOID GetRawBuffer();
   DWORD GetRawBufferLength();
    

   CIGMPHeaderV2(UCHAR type);

   CIGMPHeaderV2(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead);
    
   ~CIGMPHeaderV2();

   SPARTA_STATUS SetType(UCHAR type);

   SPARTA_STATUS SetMaxResponseTime(UCHAR code);

   SPARTA_STATUS SetChksum(USHORT chksum);

   SPARTA_STATUS SetGroupAddr(DWORD addr);   //NETWORK byte order

   SPARTA_STATUS SetGroupAddr(TCHAR * addr)
   {
      return SetGroupAddr(inet_addr(addr));
   }

   UCHAR GetType();

   UCHAR GetMaxResponseTime();

   USHORT GetChksum();

   DWORD GetGroupAddr();      // NETWORK byte order

   SPARTA_STATUS SetAutoCalcChecksum(const AUTO_CAL_TYPES status);

   USHORT CalcChecksum();

   SPARTA_STATUS PreparePacketForSend();

   void Print();

};

#endif // __SPARTA_IGMPHeaderV2_H__

