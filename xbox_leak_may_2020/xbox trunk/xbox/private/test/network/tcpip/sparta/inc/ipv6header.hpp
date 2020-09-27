/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: Ip6Header.hpp                                                                *
* Description: This is the implementation of the Ip6Header functions                     *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      4/18/2000    created                               *
*                       balasha      5/02/2000    Added AutoChecksum Calculation        *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_Ip6Header_H__
#define __SPARTA_Ip6Header_H__


#include "packets.h"
#include "media.h"
#include "Parser.hpp"
#include "Media.h"
#include "Pattern.hpp"

#include "ipinfo.h"
//#include "ip6.h"

#pragma pack(push,origpack)
#pragma pack(1)

//
// IPv6 Header Format.
// See RFC 1883, page 5 (and subsequent draft updates to same).
//

typedef struct IPv6Header {
    u_long VersClassFlow;   // 4 bits Version, 8 Traffic Class, 20 Flow Label.
    u_short PayloadLength;  // Zero indicates Jumbo Payload hop-by-hop option.
    u_char NextHeader;      // Values are superset of IPv4's Protocol field.
    u_char HopLimit;
    struct in6_addr Source;
    struct in6_addr Dest;
} tIPv6Header;

#pragma pack(pop,origpack)


#define IVP6BASEHEADERLENGTH sizeof(tIPv6Header)


class CIPv6Header
{
    friend class CIPv6Packet;

private:

//    VOID ParseBufferIntoVariables();
//    VOID ParseVariablesIntoBuffer();

protected:
   IPv6Header *m_pIPv6Header;

    PKT_BUFFER m_IPv6Header; // we want access to these members from the IPPacket class

public:

    
	CIPv6Header();
	
//	CIPv6Header(PVOID rawBuffer,DWORD actualSize,DWORD maxSize);
    
    CIPv6Header(PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead);
    
    ~CIPv6Header();

#if 0
	SPARTA_STATUS GetSrcAddr(TCHAR *ipBuff,DWORD * dwlen);

	SPARTA_STATUS GetDestAddr(TCHAR *ipBuff,DWORD * dwlen);

    ULONG GetSrcAddr();
    ULONG GetDestAddr();
   

	/**++
	Routine Description:

	  Returns the Actual IP Datagram length of the IP Packet(including options) buffers
	  This is not the IP Datagram length from the IP Header but the buffer length that is
	  used to store the ip header and the ip options and the IP data (datagram)

	  Returns: DWORD_ERROR_VALUE in case of Error

	--*/
	DWORD GetActualDatagramLength();

	/**++
	Routine Description:
	
	  Returns the Actual Header length of the IP Packet(including options) buffers
	  This is not the header length from the IP Header but the buffer length that is
	  used to store the ip header and the ip options

	  Returns: DWORD_ERROR_VALUE in case of Error

	--*/
	DWORD GetActualHeaderLength();

	

	// Methods for Pattern filtering
	static CPattern *CreateSrcIPv6AddressPattern(MAC_MEDIA_TYPE MediaType,TCHAR *sIpAdd);
	static CPattern *CreateDestIPv6AddressPattern(MAC_MEDIA_TYPE MediaType,TCHAR * dIpAdd);
	
    static CPattern *CreateSrcIPv6AddressPattern(MAC_MEDIA_TYPE MediaType,ULONG sIpAdd);
	static CPattern *CreateDestIPv6AddressPattern(MAC_MEDIA_TYPE MediaType,ULONG dIpAdd);

	
	PKT_BUFFER GetRawBuffer(); // not to be used from the script

	PVOID GetRawBufferData();

	SPARTA_STATUS SetRawBuffer(PKT_BUFFER buffer);


    DWORD GetRawBufferLength();

#endif //0

    void Print();

    SPARTA_STATUS PreparePacketForSend();


};

#endif // __SPARTA_IP6HEADER_H__

