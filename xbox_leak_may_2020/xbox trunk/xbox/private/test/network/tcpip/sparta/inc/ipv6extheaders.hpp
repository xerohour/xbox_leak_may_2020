//////////////////////////////////////////////////////////////////////////
//                       SPARTA project
//
// (TCP/IP test team)
//
// Filename:      Ipv6OptionsHeader.hpp
// Description:   This is the implementation of the option Extention Header
//                type HopByHop and DestinationOptions
//
// Revision history:     name          date         modifications
//                       jbekmann      10/9/2000    created
//
//            (C) Copyright Microsoft Corporation 1999-2000
//////////////////////////////////////////////////////////////////////////

#ifndef __SPARTA_Ip6ExtHeaders_H__
#define __SPARTA_Ip6ExtHeaders_H__


#include "packets.h"
#include "media.h"
#include "Parser.hpp"
#include "Media.h"
#include "Pattern.hpp"
#include "ipinfo.h"
#include "ipheader.hpp"



/////////////////////////////////////////////////
// base header class -- virtual class that everything else is based off
/////////////////////////////////////////////////

//
// functions supported generically by all extension header types
// Deal with type of this header and type of the next header in the chain
//
class CIPv6ExtensionHeader
{
public:
   virtual UCHAR GetHeaderType();         // get type of this header
   virtual VOID  SetNextHeader(UCHAR ucNextHeader); 
   virtual UCHAR GetNextHeader();
   virtual USHORT GetLength();
};


/////////////////////////////////////////////////
// options header class.  Used for hop-by-hop and destination options
/////////////////////////////////////////////////

#define  MIN_OPTIONS_HEADER_SIZE    8
#define  MAX_OPTIONS_HEADER_SIZE    512
#define  MAX_OPTION_SIZE            256

struct OptionData
{
   UCHAR    ucType;                        // option type
   UCHAR    ucDataLength;                  // In bytes, not counting two for the header.
   UCHAR    pbOptionData[MAX_OPTION_SIZE]; // pointer to the option data
};

//
// from ip6.h
//
// typedef struct IPv6OptionsHeader 
// {
//    u_char NextHeader;
//    u_char HeaderExtLength;  // In 8-byte units, not counting first 8.
// } IPv6OptionsHeader;
// This is followed by 1 or more OptionsData structures
//

class CIPv6OptionsHeader : public CIPv6ExtensionHeader
{
private:
   //
   // internal variables
   //
   PKT_BUFFER  m_HeaderBuffer;
   UCHAR       m_OptionType;
   BOOL        m_AutoPadOptions;
   BOOL        m_AutoCalcLength;
   USHORT      m_WriteOffset;
   USHORT      m_ReadOffset;
   PVOID       m_pvBuffer;    // handy reference to actual header data in m_HeaderBuffer

public:
   //
   // support for virtual functions from base type
   //
   UCHAR    GetHeaderType()
   {                                
      return m_OptionType;
   }
   UCHAR    GetNextHeader();
   VOID     SetNextHeader(UCHAR ucNextHeader);
   USHORT   GetLength();

   //
   // class specific functions
   //
   CIPv6OptionsHeader(UCHAR OptionType);        // default constructor.
   CIPv6OptionsHeader(UCHAR OptionType, PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead); // constructor called by the parser
   ~CIPv6OptionsHeader();       // destructor

   BOOL     GetAutoPad()
   {
      return m_AutoPadOptions;
   }
   VOID  SetAutoPad(BOOL Flag)
   {
      m_AutoPadOptions = Flag;
   }
   BOOL     GetAutoCalcLength()
   {
      return m_AutoCalcLength;
   }
   VOID  SetAutoCalcLength(BOOL Flag)
   {
      m_AutoCalcLength = Flag;
   }

   SPARTA_STATUS  SetLength(USHORT usLength);
   
   SPARTA_STATUS AddOption(OptionData *pOptionData);  // appends an option to end
                                                      // of hop by hop options
   OptionData  *GetFirstOption();                     // returns NULL if there is no more option
   OptionData  *GetNextOption();                      // returns NULL if there is no more option
};

/////////////////////////////////////////////////
// routing header class                                  
/////////////////////////////////////////////////


#define  MIN_ROUTING_HEADER_SIZE    8
#define  MAX_ROUTE_ADDRESSES        32
#define  MAX_ROUTING_HEADER_SIZE    (MAX_ROUTE_ADDRESSES*16) + 8

//
// from ip6.h
//
// typedef struct IPv6RoutingHeader 
// {
//    u_char NextHeader;
//    u_char HeaderExtLength;  // In 8-byte units, not counting first 8.
//    u_char RoutingType;
//    u_char SegmentsLeft;     // Number of nodes still left to be visited.
//    u_int Reserved;
// } IPv6RoutingHeader;
// This is followed by 1 or more Ip6Addr structures


class CIPv6RoutingHeader : public CIPv6ExtensionHeader
{
private:
   //
   // internal variables
   //
   PKT_BUFFER  m_HeaderBuffer;
   BOOL        m_AutoCalcLength;
   USHORT      m_NumAddresses;   
   PVOID       m_pvBuffer;    // handy reference to actual header data in m_HeaderBuffer

public:
   //
   // support for virtual functions from base type
   //
   UCHAR    GetHeaderType()
   {                                
      return IP6_PROTOCOL_ROUTING;
   }
   UCHAR    GetNextHeader();
   VOID     SetNextHeader(UCHAR ucNextHeader);
   USHORT   GetLength();

   //
   // class specific functions
   //
   CIPv6RoutingHeader();        // default constructor.
   CIPv6RoutingHeader(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead); // constructor called by the parser
   ~CIPv6RoutingHeader();       // destructor


   BOOL     GetAutoCalcLength()
   {
      return m_AutoCalcLength;
   }
   VOID  SetAutoCalcLength(BOOL Flag)
   {
      m_AutoCalcLength = Flag;
   }

   SPARTA_STATUS  SetLength(USHORT usLength);
   UCHAR          GetRoutingType();
   SPARTA_STATUS  SetRoutingType(UCHAR ucType);
   UCHAR          GetSegmentsLeft();
   SPARTA_STATUS  SetSegmentsLeft(UCHAR ucSegments);
   ULONG          GetReserved();
   SPARTA_STATUS  SetReserved(ULONG Reserved);
   SPARTA_STATUS  GetAddress(ULONG Slot, IPv6Addr *pAddr);
   SPARTA_STATUS  SetAddress(ULONG Slot, IPv6Addr *pAddr);
   USHORT         GetNumAddresses()
   {
      return m_NumAddresses;
   }

};


/////////////////////////////////////////////////
// fragmentation header class
/////////////////////////////////////////////////

//
// from ip6.h
//
// typedef struct FragmentHeader 
// {
//    u_char NextHeader;
//    u_char Reserved;
//    u_short OffsetFlag;  // Offset is upper 13 bits, flag is lowest bit.
//    u_long Id;
//} FragmentHeader;
//


class CIPv6FragmentHeader : public CIPv6ExtensionHeader
{
private:
   //
   // internal variables
   //
   PKT_BUFFER  m_HeaderBuffer;
   PVOID       m_pvBuffer;    // handy reference to actual header data in m_HeaderBuffer

public:
   //
   // support for virtual functions from base type
   //
   UCHAR    GetHeaderType()
   {                                
      return IP6_PROTOCOL_FRAGMENT;
   }
   UCHAR    GetNextHeader();
   VOID     SetNextHeader(UCHAR ucNextHeader);
   USHORT   GetLength();

   //
   // class specific functions
   //
   CIPv6FragmentHeader();        // default constructor.
   CIPv6FragmentHeader(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead); // constructor called by the parser
   ~CIPv6FragmentHeader();       // destructor

   UCHAR          GetReserved();
   SPARTA_STATUS  SetReserved(UCHAR cVal);
   USHORT         GetFragmentOffset();
   SPARTA_STATUS  SetFragmentOffset(USHORT sVal);
   UCHAR          GetReservedFlags();
   SPARTA_STATUS  SetReservedFlags(UCHAR cVal);
   BOOL           GetMoreFragmentsFlag();
   SPARTA_STATUS  SetMoreFragmentsFlag(BOOL bVal);
   ULONG          GetId();
   SPARTA_STATUS  SetId(ULONG ulVal);
};

/////////////////////////////////////////////////
// authentication header class
/////////////////////////////////////////////////


#define  MIN_AUTHENT_HEADER_SIZE    16
#define  MAX_AUTHENT_HEADER_SIZE    512
//
// from ip6.h
//
// typedef struct AHHeader 
// {
//    u_char NextHeader;
//    u_char PayloadLen;  // In 4-byte units, not counting first 8 bytes.
//    u_short Reserved;   // Padding.  Must be zero on transmit.
//    u_long SPI;         // Security Parameters Index.
//    u_long Seq;         // Sequence number for anti-replay algorithms.
//} AHHeader;
// This is followed by 0 or more bytes of authentication data


class CIPv6AuthentHeader : public CIPv6ExtensionHeader
{
private:
   //
   // internal variables
   //
   PKT_BUFFER  m_HeaderBuffer;   // we want access to these members from the IPPacket class
   BOOL        m_AutoCalcLength;
   PVOID       m_pvBuffer;    // handy reference to actual header data in m_HeaderBuffer

public:
   //
   // support for virtual functions from base type
   //
   UCHAR    GetHeaderType()
   {                                
      return IP6_PROTOCOL_AH;
   }
   UCHAR    GetNextHeader();
   VOID     SetNextHeader(UCHAR ucNextHeader);
   USHORT   GetLength();

   //
   // class specific functions
   //
   CIPv6AuthentHeader();        // default constructor.
   CIPv6AuthentHeader(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead); // constructor called by the parser
   ~CIPv6AuthentHeader();       // destructor



   BOOL     GetAutoCalcLength()
   {
      return m_AutoCalcLength;
   }
   VOID  SetAutoCalcLength(BOOL Flag)
   {
      m_AutoCalcLength = Flag;
   }

   SPARTA_STATUS  SetLength(USHORT sVal);
   USHORT         GetReserved();
   SPARTA_STATUS  SetReserved(USHORT sVal);
   ULONG          GetSPI();
   SPARTA_STATUS  SetSPI(ULONG lVal);
   ULONG          GetSeq();
   SPARTA_STATUS  SetSeq(ULONG lVal);
   SPARTA_STATUS  GetAuthentData(PVOID pvBuffer, DWORD *dwLen);
   SPARTA_STATUS  SetAuthentData(PVOID pvBuffer, DWORD dwLen);

};


/////////////////////////////////////////////////
// ESP header class
/////////////////////////////////////////////////

//
// from ip6.h
//
// typedef struct ESPHeader
// {
//     u_long SPI;  // Security Parameters Index.
//     u_long Seq;  // Sequence number for anti-replay algorithms.
// } ESPHeader;
//
// Followed by variable amount of payload data then padding, then ESPTrailer
//
// typedef struct ESPTrailer
// {
//    u_char PadLength;   // Number of bytes in pad.
//    u_char NextHeader;
// } ESPTrailer;
// Followed by 0 or more bytes of authentication data
//

class CIPv6ESPHeader : public CIPv6ExtensionHeader
{
private:
   //
   // internal variables
   //
   PKT_BUFFER  m_HeaderBuffer;   // we want access to these members from the IPPacket class

public:
   //
   // support for virtual functions from base type
   //
   UCHAR    GetHeaderType()
   {                                
      return IP6_PROTOCOL_ESP;
   }
   UCHAR    GetNextHeader();
   VOID     SetNextHeader(UCHAR ucNextHeader);
   USHORT   GetLength()
   {
      return 0xFFFF;       // not really supported yet
   }

   //
   // class specific functions
   //
   CIPv6ESPHeader();        // default constructor.
   CIPv6ESPHeader(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead); // constructor called by the parser
   ~CIPv6ESPHeader();       // destructor
};


#endif // __SPARTA_Ip6ExtHeaders_H__

