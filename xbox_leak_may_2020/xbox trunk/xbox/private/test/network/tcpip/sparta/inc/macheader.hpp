/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: MacHeader.hpp                                                               *
* Description: This is the implementation of the MediaHeader functions                  *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/17/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_MACHEADER_H__
#define __SPARTA_MACHEADER_H__


#include "packets.h"
#include "media.h"
#include "MacAddress.hpp"
#include "Parser.hpp"

class CPattern;


class CMacHeader
{

    friend class CMacPacket;
    friend class CArpPacket;

protected:

    PKT_BUFFER m_MacHeader; // we want access to these members from the MacPacket class

public:

    PVOID GetRawBuffer();
    DWORD GetRawBufferLength();
    DWORD GetMinimumMediaPacketLength();
    DWORD GetMaximumMediaPacketLength();
    

    CMacHeader(MAC_MEDIA_TYPE MediaType);

// create a MEDIA header buffer from raw data, part of the parsing API
// dwBytesRead will return the number of bytes used for the header
    CMacHeader(MAC_MEDIA_TYPE MediaType, PVOID pvRawData, DWORD dwBytesRemaining, OUT PDWORD pdwBytesRead);
    
    ~CMacHeader();

    SPARTA_STATUS SetDestAddress(CMacAddress &Address);

    SPARTA_STATUS SetSrcAddress(CMacAddress &Address);

    SPARTA_STATUS GetSrcAddress(CMacAddress &Address);
    // takes a previously created Address object and fills it with the address

    SPARTA_STATUS GetDestAddress(CMacAddress &Address);
    // takes a previously created Address object and fills it with the address

    SPARTA_STATUS SetProtocolType(WORD wProtocolType);

    WORD GetProtocolType();

    SPARTA_STATUS Print();

    static CPattern *CreateDestAddressPattern(MAC_MEDIA_TYPE MediaType, CMacAddress *Address);
    static CPattern *CreateSrcAddressPattern(MAC_MEDIA_TYPE MediaType, CMacAddress *Address);

    static CPattern *CreateDestAddressPattern(MAC_MEDIA_TYPE MediaType, CMacAddress &Address)
    {
        return CreateDestAddressPattern(MediaType,&Address);
    }

    static CPattern *CreateSrcAddressPattern(MAC_MEDIA_TYPE MediaType, CMacAddress &Address)
    {
        return CreateSrcAddressPattern(MediaType,&Address);
    }

    static CPattern *CreateProtocolTypePattern(MAC_MEDIA_TYPE MediaType, USHORT usProtocolType);

    static WORD LookupMediaHeaderLength(MAC_MEDIA_TYPE MediaType);
};

#endif // __SPARTA_MACHEADER_H__

