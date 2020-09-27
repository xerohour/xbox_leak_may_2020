/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: ICMPv6Header.hpp                                                            *
* Description: This is the description of the ICMPv6Header functions                    *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      5/2/2000    created                                *
*                       timothyw     12/27/2000  copied from icmpheader.hpp             *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_ICMPv6HEADER_H__
#define __SPARTA_ICMPv6HEADER_H__


#include "packets.h"
#include "IPConstants.h"
#include <ipexport.h>
#include "media.h"

//
// structures for the second dword of the icmpv6 header
//
struct   WordWord
{
   WORD  WordOne;
   WORD  WordTwo;
};


struct   ByteByteWord 
{
   BYTE  ByteOne;
   BYTE  ByteTwo;
   WORD  WordOne;
};


//
// main icmp header structure
//
struct      ICMPv6_HEADER
{
   UCHAR    ucType;
   UCHAR    ucCode;
   USHORT   usChksum;

   union 
   {
      ULONG          ulData;
      WordWord       wwData;
      ByteByteWord   bbwData;
   }  icmp_header_data;
};
typedef  ICMPv6_HEADER  *PICMPv6_HEADER;


//
// defines to provide meaningful names for the union fields.
//

//
// used by ICMPv6_DESTINATION_UNREACHABLE
//         ICMPv6_TIME_EXCEEDED
//         ICMPv6_ROUTER_SOLICITATION
//         ICMPv6_NEIGHBOR_SOLICITATION
//         ICMPv6_NEIGHBOR_ADVERTISEMENT
//         ICMPv6_REDIRECT
//
#define icmp6_reserved         icmp_header_data.ulData

//
// used by ICMPv6_PACKET_TOO_BIG
//
#define icmp6_mtu              icmp_header_data.ulData

//
// used by ICMPv6_PAARAMETER_PROBLEM
//
#define icmp6_ptr              icmp_header_data.ulData


//
// used by ICMPv6_ECHO_REQUEST
//         ICMPv6_ECHO_REPLAY
#define icmp6_id               icmp_header_data.wwData.WordOne
#define icmp6_seq              icmp_header_data.wwData.WordTwo

//
// used by ICMPv6_MULTICAST_LISTENER_QUERY
//         ICMPv6_MULTICAST_LISTENER_REPORT
//         ICMPv6_MULTICAST_LISTENER_DONE
//
#define icmp6_maxdelay         icmp_header_data.wwData.WordOne
#define icmp6_wreserved        icmp_header_data.wwData.WordTwo

//
// used by ICMPv6_ROUTER_ADVERTISEMENT
//
#define icmp6_curlimit         icmp_header_data.bbwData.ByteOne
#define icmp6_reservedflags    icmp_header_data.bbwData.ByteTwo
#define icmp6_lifetime         icmp_header_data.bbwData.WordOne


//
// definition for structures before options but after 8-bytes of header
// used by ICMPv6_ROUTER_ADVERTISEMENT
//
struct   ROUTER_ADVERTISEMENT
{
   ULONG ReachableTimer;
   ULONG RetransmitTimer;
};
typedef ROUTER_ADVERTISEMENT *PROUTER_ADVERTISEMENT;


//
// ICMPv6_NEIGHBOR_SOLICITATION, ICMPv6_NEIGHBOR_ADVERTISEMENT,
// ICMPv6_MULTICAST_LISTENER_QUERY, ICMPv6_MULTICAST_LISTENER_REPORT,
// ICMPv6_MULTICAST_LISTENER_DONE  use a single IPv6Addr
// ICMPv6_REDIRECT uses two of them
// 

//
// option header structure
//
struct   ICMPv6_OPTION
{
   UCHAR    ucType;
   UCHAR    ucLength;
};
typedef  ICMPv6_OPTION *PICMPv6_OPTION;

//
// possible option types: link layer address (6 bytes for ethernet/tokenring)
//                        router prefix ???
//                        redir header  ???
//                        mtu  -- ushort
//

#define MAX_ICMPv6_HEADER_LENGTH    (sizeof(ICMPv6_HEADER) + 2 * sizeof(IPv6Addr))
#define MAX_ICMPv6_OPTIONS          8


class CIPHeader;
class CUDPHeader;
class CTCPHeader;
class CIPPacket;

class CICMPv6Header
{
   friend class CICMPv6Packet;

   BOOL  ParseICMP(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead);

protected:

   PKT_BUFFER     m_ICMPHeader;        // buffer storing the icmpv6 header (no user data or options?)
   PICMPv6_HEADER m_pIcmpHeader;       // ptr to the header itself
   PVOID          m_pvIcmpData;        // rest of header data (not used by all types)
   UCHAR          m_icmpType;          // local copy of icmp type
   PKT_BUFFER     m_Option[MAX_ICMPv6_OPTIONS];
   ULONG          m_NumOptions;

   BOOL  m_IsIPHeaderChained;
   BOOL  m_IsIPHeaderDataChained;
   BOOL  m_IsIPOptionsChained;
   BOOL  m_IsICMPErrHeaderChained;
   BOOL  m_IsTCPErrHeaderChained;
   BOOL  m_IsUDPErrHeaderChained;

   // This points to the IP Datagram (minus header) , the 8 bytes from the error datagram
   PKT_BUFFER  m_ICMPIPHeaderData;

   CICMPv6Header(PVOID pvRawData, DWORD dwBytesRemaining);
   SPARTA_STATUS ProcessIPErrorDatagram(CIPPacket *packet,DWORD bytesToCopy);

public:
   CIPHeader      *m_pICMPIPHeader;
   CUDPHeader     *m_pICMPUDPHeader;
   CTCPHeader     *m_pICMPTCPHeader;
   CICMPv6Header  *m_pICMPICMPHeader;

   PKT_BUFFER  GetRawBuffer();
   DWORD       GetRawBufferLength();
    
   // Default Constructor
   CICMPv6Header(UCHAR icmpType);


   // create a MEDIA header buffer from raw data, part of the parsing API
   // dwBytesRead will return the number of bytes used for the header
   // TO DO: Need to implement this function for parsing
   CICMPv6Header(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead);
    
   ~CICMPv6Header();

   SPARTA_STATUS ChainICMPPacketLinks();


   // ICMP Set Functions - Common functions

   SPARTA_STATUS SetCode(UCHAR code);
   SPARTA_STATUS SetType(UCHAR type);
   SPARTA_STATUS SetChksum(USHORT chksum);

   UCHAR GetCode();
   UCHAR GetType();
   USHORT GetChksum();

   //////////////////////////////////
   // Echo Request Reply functions
   //////////////////////////////////

   // ICMP Set Functions

   SPARTA_STATUS SetIcmpID(WORD id);
   SPARTA_STATUS SetIcmpSeq(WORD seq);
        
   // ICMP Get Functions

   WORD GetIcmpID();
   WORD GetIcmpSeq();

   //////////////////////////////////
   // functions dealing with reserved bytes
   //////////////////////////////////


   SPARTA_STATUS  SetReserved(ULONG Reserved);
   ULONG          GetReserved();

   //////////////////////////////////
   // other functions for error reports
   //////////////////////////////////

   SPARTA_STATUS  SetMTU(ULONG mtu);
   ULONG          GetMTU();

   SPARTA_STATUS  SetPointer(ULONG ptr);
   ULONG          GetPointer();

   //
   // Functions for handling the ICMP Error Messages
   //
   SPARTA_STATUS AttachIPErrPacket(CIPPacket *IPPacket);

   // The following functions return NULL if the corresponding err header is
   // not attached. Otherwise they return pointer to the corresponding header.
   CIPHeader   *GetIPErrHeader();
   CICMPv6Header *GetICMPErrHeader();
   CTCPHeader  *GetTCPErrHeader();
   CUDPHeader  *GetUDPErrHeader();

   ///////////////////////////////////////
   // neighbor discovery and family support
   ///////////////////////////////////////
   SPARTA_STATUS  SetCurrentHopLimit(UCHAR Limit);
   UCHAR          GetCurrentHopLimit();
   SPARTA_STATUS  SetManagedAddrFlag(BOOL Flag);
   BOOL           GetManagedAddrFlag();
   SPARTA_STATUS  SetStatefulConfigFlag(BOOL Flag);
   BOOL           GetStatefulConfigFlag();
   SPARTA_STATUS  SetRouterLifetime(USHORT Lifetime);
   USHORT         GetRouterLifetime();
   SPARTA_STATUS  SetReachableTimer(ULONG ReachTime);
   ULONG          GetReachableTimer();
   SPARTA_STATUS  SetRetransmitTimer(ULONG ReTime);
   ULONG          GetRetransmitTimer();

   SPARTA_STATUS  SetMaxDelay(USHORT Delay);
   USHORT         GetMaxDelay();

   SPARTA_STATUS  SetTargetAddress(IPv6Addr *pAddr);
   SPARTA_STATUS  GetTargetAddress(IPv6Addr *pAddr);

   SPARTA_STATUS  SetRouterFlag(BOOL Flag);
   BOOL           GetRouterFlag();
   SPARTA_STATUS  SetSolicitedFlag(BOOL Flag);
   BOOL           GetSolicitedFlag();
   SPARTA_STATUS  SetOverrideFlag(BOOL Flag);
   BOOL           GetOverrideFlag();
   SPARTA_STATUS  SetDestinationAddress(IPv6Addr *pAddr);
   SPARTA_STATUS  GetDestinationAddress(IPv6Addr *pAddr);

   ////////////////////////////////////////
   // options support
   ////////////////////////////////////////

   SPARTA_STATUS  AddSourceAddressOption(UCHAR Length, PCHAR Addr);
   SPARTA_STATUS  AddSourceAddressOption(MAC_MEDIA_TYPE Type, TCHAR *strAddr);
   SPARTA_STATUS  AddTargetAddressOption(UCHAR Length, PCHAR Addr);
   SPARTA_STATUS  AddTargetAddressOption(MAC_MEDIA_TYPE Type, TCHAR *strAddr);
   SPARTA_STATUS  AddPrefixOption(UCHAR Length, UCHAR Flags, 
                                  ULONG Valid, ULONG Preferred, 
                                  ULONG Reserved, IPv6Addr *pAddr);
  
   SPARTA_STATUS  AddHeaderOption(USHORT MaxLength, USHORT Reserved1, ULONG Reserved2, CIPPacket *IPPacket);
   SPARTA_STATUS  AddMTUOption(USHORT Reserved, ULONG MTU);
   SPARTA_STATUS  AddOption(UCHAR Type, USHORT Length, PCHAR Data);

   ULONG          GetNumOptions()
   {
      return m_NumOptions;
   }

   SPARTA_STATUS  GetOption(USHORT Slot, PUCHAR Type, PUSHORT Length, PCHAR Data);
   UCHAR          GetOptionType(USHORT Slot);
   SPARTA_STATUS  GetSourceAddressOption(USHORT Slot, PCHAR Addr, PUCHAR Length);
   SPARTA_STATUS  GetTargetAddressOption(USHORT Slot, PCHAR Addr, PUCHAR Length);
   SPARTA_STATUS  GetPrefixOption(USHORT Slot, PUCHAR Length, PUCHAR Flags, PULONG Valid,
                                  PULONG Preferred, PULONG Reserved, IPv6Addr *pAddr);
   SPARTA_STATUS  GetHeaderOption(USHORT Slot, PUSHORT MaxLength, PUSHORT Reserved1, PULONG Reserved2,
                                  PVOID pvBuffer, DWORD *dwLen);
   SPARTA_STATUS  GetMTUOption(USHORT Slot, PUSHORT Reserved, PULONG MTU);


   ////////////////////////////////////////
   // misc
   ////////////////////////////////////////

   void           Print();


};

#endif // __SPARTA_ICMPv6HEADER_H__

