/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: AutoNeighbor.hpp                                                            *
* Description: This is the implementation of Automatic Neighbor discovery functionality.*
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                       timothyw      1/16/2001    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2001                          *
*****************************************************************************************/

#ifndef __SPARTA_AUTONEIGHBOR_HPP__
#define __SPARTA_AUTONEIGHBOR_HPP__

#include "Sparta.h"
#include "Media.h"
#include <ipexport.h>

class CInterface;
class CMacAddress;


class CAutoNeighbor
{
protected:
   CRITICAL_SECTION  m_csResolvingIpAddress;
   HANDLE            m_hResolutionEvent;
   HANDLE            m_hShutdownEvent;
   BOOL              m_fThreadHasQuit; // used to sync object destruction
   MAC_MEDIA_TYPE    m_MediaType;
   IPv6Addr          *m_pRequestedIpAddress;
   CMacAddress       *m_pLocalMacAddress;
   CMacAddress       *m_pResolvedMacAddress;
   CInterface        *m_pInterface;

   TCHAR             *m_szInterfaceDescriptor;
   TCHAR             *m_szLocalMacAddress;
   IPv6Addr          *m_pLocalIpAddress;

   VOID AutoNeighborListener();

   friend DWORD WINAPI LaunchAutoNeighborThread(LPVOID pvContext);
    // this function will call the AutoNeighborListener...

private:
   Init(TCHAR        *szInterfaceDescriptor,
        TCHAR        *szMacAddress,
        IPv6Addr     *pLocalIpAddress);

public:

   // this constructor will use the Interface's own MAC address for ND responses

   CAutoNeighbor(TCHAR    *szInterfaceDescriptor,
                 IPv6Addr *pLocalIpAddress);


   // this constructor will use the supplied MAC address for ND responses - this
   // HAS to be a multicast MAC address (since we add it to the mcast addr list and
   // then filter based on mcast addresses)

   CAutoNeighbor(TCHAR *szInterfaceDescriptor,
                 TCHAR *szMacAddress,
                 IPv6Addr *pLocalIpAddress);


   ~CAutoNeighbor();

   CMacAddress *ResolveIpAddress(IPv6Addr   *pIpAddr);
};

#endif // __SPARTA_AUTONEIGHBOR_HPP__

