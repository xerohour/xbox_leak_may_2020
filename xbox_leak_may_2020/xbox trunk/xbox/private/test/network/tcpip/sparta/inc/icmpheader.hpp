/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: ICMPHeader.hpp                                                              *
* Description: This is the description of the ICMPHeader functions                      *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      5/2/2000    created                                *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_ICMPHEADER_H__
#define __SPARTA_ICMPHEADER_H__


#include "packets.h"
#include "IPPacket.hpp"
#include "UDPPacket.hpp"
#include "Parser.hpp"
#include "IPConstants.h"
#include "IPHeader.hpp"
#include "UDPHeader.hpp"
#include "TCPHeader.hpp"
#include "TCPPacket.hpp"
#include "IPConstants.h"

//
// structures for data portion of icmp header
//
struct   tReqReply
{
   WORD  ID;
   WORD  SeqNo;
};
typedef  tReqReply   *ReqReply;

struct   tParmProb         // _ParameterProblemFields 
{
   BYTE  Pointer;          // pointer to the original ip header with parameter problem
   BYTE  junk[3];
};
typedef  tParmProb   *ParmProb;

struct   tPMTUDiscovery    // _PathMTUDiscoveryFields
{
   WORD  MTUVoid;
   WORD  NextMTU;
};
typedef  tPMTUDiscovery *PMTUDiscovery;


struct   tRouterAH         // _RouterAnnounceHeaderFields
{
   BYTE  NumAddrs;
   BYTE  AddrEntrySize;
   WORD  Lifetime;
};
typedef  tRouterAH   *RouterAH;


struct   tTS               // _TimestampFields
{
   DWORD tsOrig;
   DWORD tsRecv;
   DWORD tsXmit;
};
typedef  tTS   *TS;


struct   tRouterAE         // _RouterAnnounceEntry
{
   DWORD Address;
   DWORD PreferenceLevel;
};
typedef  tRouterAE   *RouterAE;


//
// main icmp header structure
//
struct      tICMP_HEADER
{
   UCHAR    icmp_type;
   UCHAR    icmp_code;
   USHORT   icmp_chksum;

   union 
   {
      tParmProb      PP;         // parameter problem fields
      tReqReply      ReqRep;     // Request reply fields
      tRouterAH      RAH;        // Router Announce fields
      tPMTUDiscovery PMTUD;      // PMTU discovery fields
      ULONG          gwaddr;     // gateway address
      DWORD          unused;     // unused junk 
   }  icmp_header_union;

#define icmp_pptr             icmp_header_union.PP.Pointer
#define icmp_pjunk            icmp_header_union.PP.junk

#define icmp_id               icmp_header_union.ReqRep.ID
#define icmp_seq              icmp_header_union.ReqRep.SeqNo

#define icmp_numAddr          icmp_header_union.RAH.NumAddrs
#define icmp_addrEntrySize    icmp_header_union.RAH.AddrEntrySize
#define icmp_lifetime         icmp_header_union.RAH.Lifetime

#define icmp_mtuvoid          icmp_header_union.PMTUD.MTUVoid
#define icmp_nextmtu          icmp_header_union.PMTUD.NextMTU

#define icmp_gwaddr           icmp_header_union.gwaddr
#define icmp_unused           icmp_header_union.unused



   union 
   {
      tTS      TS;
      ULONG    mask;
      CHAR     data[1];
   }  icmp_data_union;
                
#define icmp_otime            icmp_data_union.TS.tsOrig
#define icmp_rtime            icmp_data_union.TS.tsRecv
#define icmp_ttime            icmp_data_union.TS.tsXmit

#define icmp_mask             icmp_data_union.mask
#define icmp_data             icmp_data_union.data


};
typedef  tICMP_HEADER   *ICMP_HEADER;



class CICMPHeader
{
   friend class CICMPPacket;

   // IP header including options that generated the ERROR for which this icmp packet
   // will be sent
   AUTO_CAL_TYPES m_autoCalcICMPChecksum;

   BOOL  ParseICMP(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead);

protected:

   // This buffer will have the pvData memeber field pointed to the ip structure
   PKT_BUFFER  m_ICMPHeader;     // we want access to these members from the IPPacket class
   ICMP_HEADER m_ICMPptr;        // we want access to these members from the IPPacket class

   PKT_BUFFER  m_ICMPRouterAE;   // we want access to these members from the IPPacket class

   UCHAR       m_icmpType;

   BOOL  m_IsIPHeaderChained;
   BOOL  m_IsIPHeaderDataChained;
   BOOL  m_IsIPOptionsChained;
   BOOL  m_IsICMPErrHeaderChained;
   BOOL  m_IsTCPErrHeaderChained;
   BOOL  m_IsUDPErrHeaderChained;
   BOOL  m_IsRouterAEChained;

   // This points to the IP Datagram (minus header) , the 8 bytes from the error datagram
   PKT_BUFFER  m_ICMPIPHeaderData;

   SPARTA_STATUS ProcessIPErrorDatagram(CIPPacket *packet,DWORD bytesToCopy);
   CICMPHeader(PVOID pvRawData, DWORD dwBytesRemaining);

public:
   CIPHeader   *pICMPIPHeader;
   CUDPHeader  *pICMPUDPHeader;
   CTCPHeader  *pICMPTCPHeader;
   CICMPHeader *pICMPICMPHeader;

   PKT_BUFFER  GetRawBuffer();
   DWORD       GetRawBufferLength();
    
   // Default Constructor
   CICMPHeader(UCHAR icmpType);


   // create a MEDIA header buffer from raw data, part of the parsing API
   // dwBytesRead will return the number of bytes used for the header
   // TO DO: Need to implement this function for parsing
   CICMPHeader(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead);
    
   ~CICMPHeader();


   SPARTA_STATUS ChainICMPPacketLinks();

   // ICMP Set Functions - Common functions

   SPARTA_STATUS SetCode(UCHAR code);
   SPARTA_STATUS SetType(UCHAR type);
   SPARTA_STATUS SetChksum(USHORT chksum);

   UCHAR GetCode();
   UCHAR GetType();
   USHORT GetChksum();

   //////////////////////////////////
   // Request Reply functions
   //////////////////////////////////

   // ICMP Set Functions

   SPARTA_STATUS SetIcmpID(WORD id);
   SPARTA_STATUS SetIcmpSeq(WORD seq);
        
   // ICMP Get Functions

   WORD GetIcmpID();
   WORD GetIcmpSeq();

   // ICMP Timestamp Options

   SPARTA_STATUS SetTSOrig(DWORD tsOrig);
   SPARTA_STATUS SetTSRecv(DWORD tsRecv);
   SPARTA_STATUS SetTSXmit(DWORD tsXmit);

   DWORD GetTSOrig();
   DWORD GetTSRecv();
   DWORD GetTSXmit();

   // ICMP Address mask Options

   // mask is in NETWORK byte order
   SPARTA_STATUS SetAddrMask(DWORD mask);
   SPARTA_STATUS SetAddrMask(TCHAR * mask)
   {
      return SetAddrMask(inet_addr(mask));
   }

   // NETWORK byte order
   DWORD GetAddrMask();

   ////////////////////////////////////
   // Functions for handling the ICMP Error Messages
   ////////////////////////////////////

   // Copies the IP header from the IP Datagram that genereted the error
   SPARTA_STATUS AttachIPErrPacket(CIPPacket *IPPacket);

   // The following functions return NULL if the corresponding err header is
   // not attached. Otherwise they return pointer to the corresponding header.
   CIPHeader   *GetIPErrHeader();
   CICMPHeader *GetICMPErrHeader();
   CTCPHeader  *GetTCPErrHeader();
   CUDPHeader  *GetUDPErrHeader();

   //////////////////////////////////
   // ICMP Redirect functions
   //////////////////////////////////

   SPARTA_STATUS  SetGatewayAddr(TCHAR * addr)
   {
      return SetGatewayAddr(inet_addr(addr));
   }
   //
   // NETWORK byte order
   //
   SPARTA_STATUS  SetGatewayAddr(DWORD  addr);
   DWORD          GetGatewayAddr();

   // ICMP Fragmentation Required Functions
   SPARTA_STATUS  SetMTU(USHORT mtu);
   USHORT         GetMTU();

   SPARTA_STATUS  SetParamPtr(UCHAR ptr);
   UCHAR          GetParamPtr();

   // Source Quench, UnReachable, Time Exceeded Packets, Parameter Problem, MTU ICMP packets
   //               The following is to set a particular value to the unused field
   //              The user should set it only when the user wants to set a non-zero value 
   //              This field stays zero by default(Required by RFC)
   //              NOTE: The functions are although common for all packets but the passed value
   //              will be truncated depending upon the number of unused bytes in each type of icmp packet
   //
   SPARTA_STATUS  SetUnused(ULONG unused);
   ULONG          GetUnused();

   // Router Advertisement
   // Not Yet implemented
   //
   SPARTA_STATUS SetNumberOfRouter(UCHAR nRouter);
   SPARTA_STATUS SetAddressEntrySize(UCHAR AddESize);
   SPARTA_STATUS SetRouterLifeTime(USHORT lifetime);

   UCHAR    GetNumberOfRouter();
   UCHAR    GetAddressEntrySize();
   USHORT   GetRouterLifeTime();

   SPARTA_STATUS  SetRouterAddrAt(DWORD index,TCHAR *addr)
   {
      return SetRouterAddrAt(index, inet_addr(addr));
   }
   //
   // address in NETWORK byte order
   //
   SPARTA_STATUS  SetRouterAddrAt(DWORD index,DWORD addr);
   SPARTA_STATUS  SetPreferenceLevelAt(DWORD index,ULONG level);

   //
   // address in NETWORK byte order
   //
   DWORD          GetRouterAddrAt(DWORD index);
   ULONG          GetPreferenceLevelAt(DWORD index);


   USHORT         CalcChecksum();
   SPARTA_STATUS  SetAutoCalcChecksum(const AUTO_CAL_TYPES status);
   SPARTA_STATUS  PreparePacketForSend();

   void           Print();


};

#endif // __SPARTA_ICMPHEADER_H__

