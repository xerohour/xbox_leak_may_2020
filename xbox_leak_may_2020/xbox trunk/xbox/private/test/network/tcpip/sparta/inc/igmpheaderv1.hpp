/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: IGMPHeaderV1.hpp                                                               *
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

#ifndef __SPARTA_IGMPHeaderV1_H__
#define __SPARTA_IGMPHeaderV1_H__

#include "sparta.h"
#include "packets.h"
#include "IPConstants.h"


typedef struct
{
   USHORT reserved;
   UCHAR  minorVer;
   UCHAR  majorVer;
}tDVMRP,*DVMRP;



typedef struct
{
   UCHAR igmp_ver_type;
   UCHAR igmp_code;
   USHORT igmp_chksum;

   union
   {
      ULONG igmp_group;
      tDVMRP dvmrp;
   }dvmrp_group_union;

#define igmp_group              dvmrp_group_union.igmp_group
#define igmp_reserved           dvmrp_group_union.dvmrp.reserved
#define igmp_minorVer           dvmrp_group_union.dvmrp.minorVer
#define igmp_majorVer           dvmrp_group_union.dvmrp.majorVer

} tIGMPHeaderV1, *IGMPHeaderV1;



class CIGMPHeaderV1
{
   friend class CIGMPPacketV1;

   AUTO_CAL_TYPES m_autoCalcIGMPChecksum;
   IGMPHeaderV1 m_pIGMPHeaderV1;

protected:

   PKT_BUFFER m_IGMPHeaderV1; // we want access to these members from the CIGMPPacketV1 class

public:

   PVOID GetRawBuffer();
   DWORD GetRawBufferLength();
    

   CIGMPHeaderV1(UCHAR type);

   CIGMPHeaderV1(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead);
    
   ~CIGMPHeaderV1();

   SPARTA_STATUS SetVersion(UCHAR ver);

   SPARTA_STATUS SetType(UCHAR type);

   SPARTA_STATUS SetCode(UCHAR code);

   SPARTA_STATUS SetChksum(USHORT chksum);

   SPARTA_STATUS SetGroupAddr(DWORD addr);      // NETWORK byte order

   SPARTA_STATUS SetGroupAddr(TCHAR * addr)
   {
      return SetGroupAddr(inet_addr(addr));
   }

   UCHAR GetVersion();

   UCHAR GetType();

   UCHAR GetCode();

   USHORT GetChksum();

   DWORD GetGroupAddr();      // NETWORK byte order

   SPARTA_STATUS SetAutoCalcChecksum(const AUTO_CAL_TYPES status);

   USHORT CalcChecksum();

   SPARTA_STATUS PreparePacketForSend();



// Methods for DVMRP, type 0x13



   SPARTA_STATUS SetDVMRPReserved(USHORT res);
   SPARTA_STATUS SetDVMRPMinorVer(UCHAR ver);
   SPARTA_STATUS SetDVMRPMajorVer(UCHAR ver);


   USHORT GetDVMRPReserved();
   UCHAR GetDVMRPMinorVer();
   UCHAR GetDVMRPMajorVer();

   void Print();
};

#endif // __SPARTA_IGMPHeaderV1_H__

