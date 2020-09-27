/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: MacAddress.hpp                                                              *
* Description: This is the implementation of the MacAddress functions                   *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      2/17/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_MACADDRESS_H__
#define __SPARTA_MACADDRESS_H__


#include "packets.h"
#include "media.h"

class CMacAddress
{
protected:
    WORD m_wAddressLength;
	BYTE *m_bpAddress;
    MAC_MEDIA_TYPE m_MediaType;
    TCHAR *m_szStringVersion;

public:
    CMacAddress(MAC_ADDRESS MacAddress); // constuctor taking a C-style struct equivalent
    CMacAddress(MAC_MEDIA_TYPE MediaType); // constructor
    CMacAddress(MAC_MEDIA_TYPE MediaType,const TCHAR *szAddressString); // constructor

    ~CMacAddress(); // destructor

    SPARTA_STATUS SetAddress(const TCHAR *szAddress);
    SPARTA_STATUS SetAddress(const PBYTE pbNewAddress, DWORD dwNewAddressLength);
    BYTE *GetBuffer() const;
    WORD GetLength() const;
    MAC_MEDIA_TYPE GetMediaType() const;
    TCHAR *GetString() const; // returns string format of Media Address

    VOID Print();

    bool operator==(const CMacAddress &other) const
    {
        if(m_MediaType == other.m_MediaType &&
           m_wAddressLength == other.m_wAddressLength)
        {
            if(memcmp(m_bpAddress,other.m_bpAddress,m_wAddressLength) == 0)
                return true;
        }
    
        return false;
    }

};

WORD GetMediaAddressLength(MAC_MEDIA_TYPE MediaType); // also needed by MacHeader

#endif
