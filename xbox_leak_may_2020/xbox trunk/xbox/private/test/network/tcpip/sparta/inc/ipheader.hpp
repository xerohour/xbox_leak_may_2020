/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename:    IPHeader.hpp                                                             *
* Description: This is the implementation of the IPHeader functions                     *
*              The IpHeader structure supports BOTH ipv4 and ipv6 headers               *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                       deepakp      4/18/2000    created                               *
*                       balasha      5/02/2000    Added AutoChecksum Calculation        *
*                       timothyw     12/1/2000    added ipv6                            *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_IPHEADER_H__
#define __SPARTA_IPHEADER_H__


#include "packets.h"
#include "media.h"
#include "Parser.hpp"
#include "IPOptions.hpp"         // ipv4 specific
#include "IPOptionsVector.hpp"   // ipv4 specific
#include "IPConstants.h"
#include "Media.h"
#include "Pattern.hpp"
#include "ipinfo.h"              // ipv6 specific


/////////////////////////////////////////////////
// defines, structures, etc for ipv4
/////////////////////////////////////////////////



//////////////////////////////////////////////////
// IPv4 Header format
//////////////////////////////////////////////////

#include <pshpack1.h>

struct IPv4Header 
{
   UCHAR           iph_verlen;                             // Version and length.
   UCHAR           iph_tos;                                // Type of service.
   USHORT          iph_length;                             // Total length of datagram.
   USHORT          iph_id;                                 // Identification.
   USHORT          iph_offset;                             // Flags and fragment offset.
   UCHAR           iph_ttl;                                // Time to live.
   UCHAR           iph_protocol;                           // Protocol.
   USHORT          iph_xsum;                               // Header Checksum.
   IPAddr          iph_src;                                // Source address.
   IPAddr          iph_dest;                               // Destination address.
};

#include <poppack.h>

/////////////////////////////////////////////////
// defines, structures, etc for ipv6
/////////////////////////////////////////////////

//////////////////////////////////////////////////
// IPv6 Header Format.
// See RFC 1883, page 5 (and subsequent draft updates to same).
// from net\tcpip\tpipv6\ip6.h
//
// #pragma pack(1)
//
// typedef struct IPv6Header {
//     u_long VersClassFlow;   // 4 bits Version, 8 Traffic Class, 20 Flow Label.
//     u_short PayloadLength;  // Zero indicates Jumbo Payload hop-by-hop option.
//     u_char NextHeader;      // Values are superset of IPv4's Protocol field.
//     u_char HopLimit;
//     struct IPv6Addr Source;
//     struct IPv6Addr Dest;
// } tIPv6Header;
// 
//////////////////////////////////////////////////


// for the benefit of users which won't include ip6.h

#define IP6_PROTOCOL_HOP_BY_HOP 0  // IPv6 Hop-by-Hop Options Header.
#define IP6_PROTOCOL_V6        41  // IPv6 Header.
#define IP6_PROTOCOL_ROUTING   43  // IPv6 Routing Header.
#define IP6_PROTOCOL_FRAGMENT  44  // IPv6 Fragment Header.
#define IP6_PROTOCOL_ESP       50  // IPSec Encapsulating Security Payload Hdr.
#define IP6_PROTOCOL_AH        51  // IPSec Authentication Hdr.
#define IP6_PROTOCOL_ICMPv6    58  // IPv6 Internet Control Message Protocol.
#define IP6_PROTOCOL_NONE      59  // No next header - ignore packet remainder.
#define IP6_PROTOCOL_DEST_OPTS 60  // IPv6 Destination Options Header.

#define MAX_EXTENSION_HEADERS  16
#define MAX_SIZE_IPV6_STRING   40

#ifndef   IPv6Addr
typedef struct in6_addr IPv6Addr;
#endif

typedef struct IPv6Header IPv6Header;

#define IVP6BASEHEADERLENGTH sizeof(IPv6Header)


struct UpperLayerHeader
{
   IPv6Addr    SrcAddr;
   IPv6Addr    DestAddr;
   USHORT      usPacketLength;
   USHORT      usProtocol;
};


//////////////////////////////////////////////////////////////////////////
// forward references
//////////////////////////////////////////////////////////////////////////

class CIPv6OptionsHeader;
class CIPv6RoutingHeader;
class CIPv6FragmentHeader;
class CIPv6ESPHeader;
class CIPv6AuthentHeader;

class CIPv6ExtensionHeader;


/////////////////////////////////////////////////
// definition of the class that combines the ipv4 header and the ipv6 header
/////////////////////////////////////////////////

class CIPHeader
{
   friend class CIPPacket;

   // Shared internal vars
   // TRUE for ipv6, false for ipv4
   BOOL        m_IsVersion6;
   BOOL        m_autoCalcDatagramLength;
   // This buffer will have the pvData memeber field pointed to the ip structure
   PKT_BUFFER  m_IPHeader;    // we want access to these members from the IPPacket class
   PVOID       m_pvBuffer;    // handy reference to actual header data in m_IPHeader


   // ipv4-specific internal vars
   // True if the ip header contains ip options
   BOOL  m_IsOptionDefined;
   BOOL  m_autoAlignOptions;
   BOOL  m_autoCalcHdrLength;
   BOOL  m_autoCalcChecksum;
   BOOL  m_checkAligned;

   // ipv6-specific
   BOOL  m_UsesDefaultOrder;
   BOOL  m_UsesRandomOrder;
   ULONG m_NextHeaderIndex;

protected:
   // ipv4-specific vars
   // list of ip options
   CIPOption *m_ipOptionList[100];
   // number of IP Options
   DWORD m_nIPOptions;

   VOID OrderIpv6ExtHeaders();

public:
   // ipv6 variables...
   CIPv6OptionsHeader   *m_HopByHopHeader;
   CIPv6RoutingHeader   *m_RoutingHeader;
   CIPv6OptionsHeader   *m_IntermedDestOptionsHeader;
   CIPv6FragmentHeader  *m_FragmentHeader;
   CIPv6AuthentHeader   *m_AuthentHeader;
   CIPv6ESPHeader       *m_ESPHeader;
   CIPv6OptionsHeader   *m_FinalDestOptionsHeader;
   CIPv6ExtensionHeader *m_ExtensionHeaders[MAX_EXTENSION_HEADERS];

   // Default Constructor
   CIPHeader(BOOL fIsVersion6 = FALSE);
        
   // used by ICMP header
   CIPHeader(PVOID rawBuffer, DWORD actualSize, DWORD maxSize, BOOL fIsVersion6 = FALSE);

   // create a MEDIA header buffer from raw data, part of the parsing API
   // dwBytesRead will return the number of bytes used for the header
   CIPHeader(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead, BOOL fIsVersion6 = FALSE);
    
   ~CIPHeader();

   //
   // Functions to get values from the header itself -- common to ipv4 and ipv6
   //
   UCHAR          GetVersion();
   UCHAR          GetProtocolType();

   //
   // get header values specific to ipv4
   //
   UCHAR    GetHdrLength();
   UCHAR    GetTOS();
   USHORT   GetDatagramLength();
   USHORT   GetID();
   USHORT   GetFlag();
   USHORT   GetFragOffset();
   UCHAR    GetTTL();
   USHORT   GetChecksum();
   ULONG    GetSrcAddr();     // NETWORK byte order
   ULONG    GetDestAddr();    // NETWORK byte order

   //
   // get header values specific to ipv6
   //
   UCHAR          GetTrafficClass();
   ULONG          GetFlowLabel();
   USHORT         GetLength();
   UCHAR          GetNextHeader();
   UCHAR          GetHopLimit();
   SPARTA_STATUS  GetSrcAddr(IPv6Addr *pIp6Addr);
   SPARTA_STATUS  GetDestAddr(IPv6Addr *pIp6Addr);

   //
   // Functions to set values in the header itself -- common to ipv4 and ipv6
   //
   SPARTA_STATUS  SetVersion(UCHAR version);

   //
   // set header values specific to ipv4
   //
   SPARTA_STATUS  SetHdrLength(UCHAR hdrLength);
   SPARTA_STATUS  SetTOS(UCHAR tos);
   SPARTA_STATUS  SetDatagramLength(USHORT datagramlen);
   SPARTA_STATUS  SetID(USHORT ID);
   SPARTA_STATUS  SetFlag(USHORT Flag);
   SPARTA_STATUS  SetFragOffset(USHORT FragOffset);
   SPARTA_STATUS  SetTTL(UCHAR ttl);
   SPARTA_STATUS  SetProtocolType(UCHAR type);
   SPARTA_STATUS  SetChecksum(USHORT chksum);
   SPARTA_STATUS  SetSrcAddr(ULONG ip);      // NETWORK byte order
   SPARTA_STATUS  SetDestAddr(ULONG ip);     // NETWORK byte order

   //
   // set header values specific to ipv6
   //
   SPARTA_STATUS  SetTrafficClass(UCHAR ucTrafficClass);
   SPARTA_STATUS  SetFlowLabel(ULONG ulFlowLabel);
   SPARTA_STATUS  SetLength(USHORT usLength);
   SPARTA_STATUS  SetNextHeader(UCHAR ucNextHeader);
   SPARTA_STATUS  SetHopLimit(UCHAR ucHopLimit);
   SPARTA_STATUS  SetSrcAddr(IPv6Addr *pIp6Addr);
   SPARTA_STATUS  SetDestAddr(IPv6Addr *pIp6Addr);
   SPARTA_STATUS  SetSrcAddr(TCHAR *pAddr)
   {
      return SetSrcAddr(inet_addr(pAddr));
   }
   SPARTA_STATUS  SetDestAddr(TCHAR *pAddr)
   {
      return SetDestAddr(inet_addr(pAddr));
   }

   //
   // other functions that are common to ipv6 and ipv6
   //
   DWORD          GetActualHeaderLength();
   DWORD          GetActualDatagramLength();
   ULONG          GetPseudoChecksum(const USHORT Length);
   SPARTA_STATUS  PreparePacketForSend();
   SPARTA_STATUS  SetAutoCalcDatagramLength(BOOL flag);
   PKT_BUFFER     GetRawBuffer();
   PVOID          GetRawBufferData();
   SPARTA_STATUS  SetRawBuffer(PKT_BUFFER buffer);
   void           Print();

   //
   // functions specific for ipv4
   //
   SPARTA_STATUS     ConstructIPOptions(PVOID pvRawData, 
                                        DWORD dwBytesRemaining, 
                                        OUT PDWORD pdwBytesRead);
   SPARTA_STATUS     ChainIPOptions();
   CIPOptionsVector  *GetIPOptions();
   SPARTA_STATUS     GetIPOptionLength(DWORD * length);
   SPARTA_STATUS     AddIPOptions(CIPOption *ipOption,DWORD *dwOptionBUsed);
   CIPOption         *CreateIPOption(UCHAR opType);
   BOOL              IsOptionPresent();
   DWORD             GetNumberOfOptions();
   SPARTA_STATUS     RemoveALLIPOptions();
   SPARTA_STATUS     AutoAlignOptions(BOOL alignOptions);
   SPARTA_STATUS     SetAutoCalcChecksum(BOOL flag);
   USHORT            CalcChecksum();
   SPARTA_STATUS     SetAutoCalcHeaderLength(BOOL flag);
   static CPattern *CreateSrcIPAddressPattern(MAC_MEDIA_TYPE MediaType,ULONG sIpAdd);  // NETWORK byte order
   static CPattern *CreateDestIPAddressPattern(MAC_MEDIA_TYPE MediaType,ULONG dIpAdd); // NETWORK byte order
   static CPattern *CreateSrcIPAddressPattern(MAC_MEDIA_TYPE MediaType, TCHAR *pAddr)
   {
      return CreateSrcIPAddressPattern(MediaType, inet_addr(pAddr));
   }
   static CPattern *CreateDestIPAddressPattern(MAC_MEDIA_TYPE MediaType, TCHAR *pAddr)
   {
      return CreateDestIPAddressPattern(MediaType, inet_addr(pAddr));
   }

   //
   // functions specific for ipv6
   //
   SPARTA_STATUS  CreateHopByHopHeader();
   SPARTA_STATUS  CreateRoutingHeader();
   SPARTA_STATUS  CreateIntermediateDestinationOptionsHeader();
   SPARTA_STATUS  CreateFragmentHeader();
   SPARTA_STATUS  CreateAuthenticationHeader();
   SPARTA_STATUS  CreateESPHeader();
   SPARTA_STATUS  CreateFinalDestinationOptionsHeader();
   SPARTA_STATUS  AddHopByHopHeader();
   SPARTA_STATUS  AddRoutingHeader();
   SPARTA_STATUS  AddDestinationOptionsHeader();
   SPARTA_STATUS  AddFragmentHeader();
   SPARTA_STATUS  AddAuthenticationHeader();
   SPARTA_STATUS  AddESPHeader();
   static CPattern *CreateSrcIPAddressPattern(MAC_MEDIA_TYPE MediaType, IPv6Addr *psIpAdd);
   static CPattern *CreateDestIPAddressPattern(MAC_MEDIA_TYPE MediaType, IPv6Addr *pdIpAdd);

   BOOL  IsVersion6()
   {
      return m_IsVersion6;
   }

   BOOL IsWellOrdered()
   {
      if (m_IsVersion6 && (m_UsesDefaultOrder || (m_NextHeaderIndex == 0)))
      {
         return TRUE;
      }
      return FALSE;
   }

   short NumExtHeaders();

   //
   // static conversion functions
   //
   static SPARTA_STATUS Ipv6AddressToString(IPv6Addr *pAddr, TCHAR *strAddr, PULONG BufLength);
   static SPARTA_STATUS StringToIpv6Address(TCHAR *strAddr, IPv6Addr *pAddr);
   static SPARTA_STATUS MulticastAddressForIpv6(IPv6Addr *pAddr, TCHAR *strAddr, PULONG BufLength);

   //
   // NOTE:  expects address in NETWORK byt order
   //
   static SPARTA_STATUS Ipv4AddressToString(ULONG Addr, TCHAR *strAddr, PULONG BufLength);

   //
   // NOTE:  returns NETWORK byte ordering!
   //
   static ULONG   StringToIpv4Address(TCHAR *pAddr)
   {
      return inet_addr(pAddr);
   }
};

#endif // __SPARTA_MACHEADER_H__

