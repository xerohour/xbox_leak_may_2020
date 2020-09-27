/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: AutoArp.hpp                                                                 *
* Description: This is the implementation of Automatic ARP handler functionality.       *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      5/23/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

#ifndef __SPARTA_AUTOARP_HPP__
#define __SPARTA_AUTOARP_HPP__

#include "Sparta.h"
#include "Interface.hpp"
#include "MacAddress.hpp"

class CAutoArp
{
protected:

    CRITICAL_SECTION    m_csResolvingIpAddress;
    HANDLE              m_hResolutionEvent;
    HANDLE              m_hShutdownEvent;
    BOOL                m_fThreadHasQuit; // used to sync object destruction
    CInterface          *m_pInterface;
    MAC_MEDIA_TYPE      m_MediaType;
    ULONG               m_ulRequestedIpAddress;
    CMacAddress         *m_pLocalMacAddress;
    CMacAddress         *m_pResolvedMacAddress;

    TCHAR               *m_szInterfaceDescriptor;
    TCHAR               *m_szLocalMacAddress;
    TCHAR               *m_szLocalIpAddress;

    VOID AutoArpListener();

    friend DWORD WINAPI LaunchAutoArpThread(LPVOID pvContext);
    // this function will call the AutoArpListener...

private:
    Init(TCHAR *szInterfaceDescriptor,
             TCHAR *szMacAddress,
             TCHAR *szLocalIpAddress); // object initialization

public:

    // this constructor will use the Interface's own MAC address for ARP responses

    CAutoArp(TCHAR *szInterfaceDescriptor,
             TCHAR *szLocalIpAddress);


    // this constructor will use the supplied MAC address for ARP responses - this
    // HAS to be a multicast MAC address (since we add it to the mcast addr list and
    // then filter based on mcast addresses)

    CAutoArp(TCHAR *szInterfaceDescriptor,
             TCHAR *szMacAddress,
             TCHAR *szLocalIpAddress);


    ~CAutoArp();

    CMacAddress *ResolveIpAddress(ULONG ulRequestedIpAddr);    // expects NETWORK byte ordering
    CMacAddress *ResolveIpAddress(TCHAR *szRequestedIpAddr)
    {
       return ResolveIpAddress(inet_addr(szRequestedIpAddr));
    }
};

#endif // __SPARTA_AUTOARP_HPP__

