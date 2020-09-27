/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    client.cpp

Author:

    Matt Bronder

Description:

    Client code for remote reference verification.

*******************************************************************************/

#include "d3dlocus.h"
#include "transprt.h"

#define SIG_MACRO(f,s) s,
LPCSTR CClient::m_szSigs[] = {
#include "sigs.h"
};

//******************************************************************************
CClient::CClient() {

    m_sEnumerate = INVALID_SOCKET;
    m_sListen = INVALID_SOCKET;
    m_sServer = INVALID_SOCKET;
    m_pPacket = NULL;
#ifdef UNDER_XBOX
    m_bXnetStartup = FALSE;
#endif // UNDER_XBOX
    m_bWSAStartup = FALSE;
    m_bMCallsEnabled = TRUE;
}

//******************************************************************************
CClient::~CClient() {

    // Close the broadcast socket
    if (m_sEnumerate != INVALID_SOCKET) {
        shutdown(m_sEnumerate, SD_BOTH);
        closesocket(m_sEnumerate);
        m_sEnumerate = INVALID_SOCKET;
    }

    // Close the listen socket
    if (m_sListen != INVALID_SOCKET) {
        shutdown(m_sListen, SD_BOTH);
        closesocket(m_sListen);
        m_sListen = INVALID_SOCKET;
    }

    // Close the server socket
    if (m_sServer != INVALID_SOCKET) {
        shutdown(m_sServer, SD_BOTH);
        closesocket(m_sServer);
        m_sServer = INVALID_SOCKET;
    }

    // Delete the packet
    if (m_pPacket) {
        delete [] m_pPacket;
        m_pPacket = NULL;
    }

    if (m_bWSAStartup) {
        WSACleanup();
    }

#ifdef UNDER_XBOX
    if (m_bXnetStartup) {
        if (GetStartupContext() & TSTART_HARNESS) {
            XNetRelease();
        }
        else {
            XNetCleanup();
        }
    }
#endif // UNDER_XBOX
}

//******************************************************************************
BOOL CClient::Create() {

    WSADATA         wsa;
    SOCKADDR_IN     addr;
    int             nSize;
    DWORD           dwRet;
    BOOL            bBroadcast = TRUE;
    BOOL            bKeepAlive = TRUE;
#ifndef UNDER_XBOX
    ULONG           uNonBlocking = 0;
    WSAEVENT        hAcceptEvent;
#else
    fd_set          readfds;
    TIMEVAL         timeout;
#endif // UNDER_XBOX

#ifdef UNDER_XBOX
    if (GetStartupContext() & TSTART_HARNESS) {
        XNetAddRef();
    }
    else {
        XNetStartupParams xnsp;
        memset(&xnsp, 0, sizeof(XNetStartupParams));
        xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
        xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
        XNetStartup(&xnsp);
    }
    m_bXnetStartup = TRUE;
#endif // UNDER_XBOX

    // Perform socket initilization for version 2 of the socket API's
    if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
        DebugString(TEXT("WSAStartup() failed [%d]"), WSAGetLastError());
        return FALSE;
    }
    m_bWSAStartup = TRUE;

    // Get the client machine address and initialize the enumeration packet with
    // address and port information (we could simply get the necessary information
    // on the server side from the recvfrom call, but this gives us the option of
    // redirecting the server in the future)
    if (gethostaddr(&(m_pkEnum.s_addrClient))) {
        return FALSE;
    }
    m_pkEnum.uPort = PORT_SERVICE;

    // Create a datagram socket to broadcast enumeration requests
    m_sEnumerate = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sEnumerate == INVALID_SOCKET) {
        DebugString(TEXT("socket() failed [%d]"), WSAGetLastError());
        return FALSE;
    }

    // Enable broadcasting in the socket
    if (setsockopt(m_sEnumerate, SOL_SOCKET, SO_BROADCAST, (LPSTR)&bBroadcast, 
                                            sizeof(BOOL)) == SOCKET_ERROR) {
        DebugString(TEXT("setsockopt() failed [%d]"), WSAGetLastError());
        return FALSE;
    }

    // Bind the socket to the broadcast port
    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_ENUMERATE);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_sEnumerate, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
        DebugString(TEXT("bind() failed [%d]"), WSAGetLastError());
        return FALSE;
    }

    // Create a listen socket to handle enumeration responses
    m_sListen = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sListen == INVALID_SOCKET) {
        DebugString(TEXT("socket() failed [%d]"), WSAGetLastError());
        return FALSE;        
    }

    // Bind the socket to the service port
    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_SERVICE);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_sListen, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
        DebugString(TEXT("bind() failed [%d]"), WSAGetLastError());
        return FALSE;
    }

    // Allow the listen socket to handle only 1 pending connection
    if (listen(m_sListen, 1) == SOCKET_ERROR) {
        DebugString(TEXT("listen() failed [%d]"), WSAGetLastError());
        return FALSE;
    }

#ifndef UNDER_XBOX

    // Create an event to signal an accepted connection
    hAcceptEvent = WSACreateEvent();
    if (hAcceptEvent == WSA_INVALID_EVENT) {
        return FALSE;
    }

    // Switch to asynchronous mode
    if (WSAEventSelect(m_sListen, hAcceptEvent, FD_ACCEPT) == SOCKET_ERROR) {
        DebugString(TEXT("WSAEventSelect() failed [%d]"), WSAGetLastError());
        return FALSE;
    }

#endif // !UNDER_XBOX

    // Broadcast an enumeration request to the network
    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_BROADCAST);
    addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    if (sendto(m_sEnumerate, (LPCSTR)&m_pkEnum, sizeof(m_pkEnum), 0, 
        (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) 
    {
        DebugString(TEXT("sendto() failed [%d]"), WSAGetLastError());
        return FALSE;
    }

    ZeroMemory(&addr, sizeof(SOCKADDR_IN));
    nSize = sizeof(SOCKADDR_IN);

#ifndef UNDER_XBOX

    // Wait for the server to respond to our enumeration request and connect
    m_sServer = accept(m_sListen, (LPSOCKADDR)&addr, &nSize);
    if (m_sServer == INVALID_SOCKET) {

        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            WSACloseEvent(hAcceptEvent);
            return FALSE;
        }
        else {

            dwRet = WSAWaitForMultipleEvents(1, &hAcceptEvent, FALSE, 500, FALSE);
            if (dwRet != 0) {
                if (dwRet == WSA_WAIT_FAILED) {
                    DebugString(TEXT("WSAWaitForMultipleEvents() failed [%d]"), WSAGetLastError());
                }
                else if (dwRet == WSA_WAIT_TIMEOUT) {
                    Log(LOG_COMMENT, TEXT("Unable to connect to reference verification server.  Rendered images will not be verified."));
                }
                WSACloseEvent(hAcceptEvent);
                return FALSE;
            }

            m_sServer = accept(m_sListen, (LPSOCKADDR)&addr, &nSize);
            if (m_sServer == INVALID_SOCKET) {
                WSACloseEvent(hAcceptEvent);
                return FALSE;
            }
        }
    }

    // Switch back to synchronous mode
    if (WSAEventSelect(m_sServer, hAcceptEvent, 0) == SOCKET_ERROR) {
        DebugString(TEXT("WSAEventSelect() failed [%d]"), WSAGetLastError());
        WSACloseEvent(hAcceptEvent);
        return FALSE;
    }

    if (ioctlsocket(m_sServer, FIONBIO, &uNonBlocking) == SOCKET_ERROR) {
        DebugString(TEXT("ioctsocket() failed [%d]"), WSAGetLastError());
    }

    WSACloseEvent(hAcceptEvent);

    // Enable keep-alive on the socket
    if (setsockopt(m_sServer, SOL_SOCKET, SO_KEEPALIVE, (LPSTR)&bKeepAlive, 
                                            sizeof(BOOL)) == SOCKET_ERROR) {
        DebugString(TEXT("setsockopt() failed [%d]"), WSAGetLastError());
    }

#else

    FD_ZERO(&readfds);
    FD_SET(m_sListen, &readfds);

    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;

    select(0, &readfds, NULL, NULL, &timeout);

    if (!FD_ISSET(m_sListen, &readfds)) {
        Log(LOG_COMMENT, TEXT("Unable to connect to reference verification server.  Rendered images will not be verified."));
        return FALSE;
    }

    m_sServer = accept(m_sListen, (LPSOCKADDR)&addr, &nSize);
    if (m_sServer == INVALID_SOCKET) {
        return FALSE;
    }

#endif // UNDER_XBOX

    return TRUE;
}

//******************************************************************************
void CClient::Disconnect() {

    Log(LOG_WARN, TEXT("Disconnecting from the reference machine.  Subsequent frames will not be verified."));

    // Close the server socket
    if (m_sServer != INVALID_SOCKET) {
        shutdown(m_sServer, SD_BOTH);
        closesocket(m_sServer);
        m_sServer = INVALID_SOCKET;
    }
}

//******************************************************************************
BOOL CClient::IsConnected() {

    return (m_sServer != INVALID_SOCKET);
}

//******************************************************************************
void CClient::EnableMethodCalls(BOOL bEnable) {

    m_bMCallsEnabled = bEnable;
}

//******************************************************************************
BOOL CClient::MethodCallsEnabled() {

    return m_bMCallsEnabled;
}

//***********************************************************************************
BOOL CClient::CheckDeviceAffinity(CDirect3D8* pDirect3D, BOOL* bEquivalent) {

    PACKET_CS_CHECKDEVICEAFFINITY pkCheckAffinity;

    // Verify the connection
    if (m_sServer == INVALID_SOCKET) {
        return FALSE;
    }

    pkCheckAffinity.pvDirect3D = pDirect3D;

    if (!SendPacket(&pkCheckAffinity)) {
        Disconnect();
        return FALSE;
    }

    if (!RecvPacket()) {
        Disconnect();
        return FALSE;
    }

    *bEquivalent = (m_pPacket->packetID == PACKETID_SC_DEVICEAFFINITY) && ((PPACKET_SC_DEVICEAFFINITY)m_pPacket)->bEquivalent;

    return TRUE;
}

//***********************************************************************************
BOOL CClient::CallDirect3DCreate8(LPVOID pvDirect3D) {

    PACKET_CS_DIRECT3DCREATE8 pkCreate;

    // Verify the connection
    if (m_sServer == INVALID_SOCKET) {
        return FALSE;
    }

    pkCreate.pvDirect3D = pvDirect3D;
    if (!SendPacket(&pkCreate)) {
        Disconnect();
        return FALSE;
    }

    if (!RecvPacket()) {
        Disconnect();
        return FALSE;
    }

    if (m_pPacket->packetID != PACKETID_SC_SUCCESS) {
        Disconnect();
        return FALSE;
    }

    return TRUE;
}

//***********************************************************************************
BOOL CClient::CallMethod(METHODID methodID, LPVOID pvObject, ...) {

    PPACKET_CS_CALLMETHOD   ppkCallMethod;
    va_list                 vl;
    LPCSTR                  pszSigs;
    LPDWORD                 pdwArg;
    LPBYTE                  pData;
    LPVOID                  pvData;
    DWORD                   dwArg;
    DWORD                   dwArgSize, dwDataSize;
    LPDWORD                 pdwOut;

    // Return if method calls have been disabled
    if (!m_bMCallsEnabled) {
        return FALSE;
    }

    // Verify the connection
    if (m_sServer == INVALID_SOCKET) {
        return FALSE;
    }

    // Determine the size of the packet
    dwArgSize = 0;
    dwDataSize = 0;

    va_start(vl, pvObject);

    for (pszSigs = m_szSigs[methodID]; *pszSigs; pszSigs += 2) {

        va_arg(vl, DWORD);

        if (*pszSigs != __OUT) {

            dwArgSize += 4;

            if (*pszSigs == __PIN) {
                dwDataSize += va_arg(vl, DWORD);
                // ##TODO: Pad data so each data segment begins on a DWORD boundary
            }
            else if (*pszSigs == __POUT) {
                va_arg(vl, DWORD);
            }
        }
    }

    // Allocate memory for the packet
    ppkCallMethod = (PPACKET_CS_CALLMETHOD)MemAlloc32(sizeof(PACKET_CS_CALLMETHOD) + dwArgSize + dwDataSize - sizeof(DWORD));
    if (!ppkCallMethod) {
        DebugString(TEXT("Failed to allocate memory for PACKET_CS_CALLMETHOD for method %d"), methodID); 
        Disconnect();
        return FALSE;
    }

    // Initialize the packet header
    ppkCallMethod->dwSize = sizeof(PACKET_CS_CALLMETHOD) + dwArgSize + dwDataSize - sizeof(DWORD);
    ppkCallMethod->packetID = methodID;
    ppkCallMethod->pvObject = pvObject;

    // Fill in the method arguments
    pdwArg = ppkCallMethod->pdwArguments;//(LPDWORD)&(ppkCallMethod->pvArguments);
    pData = (LPBYTE)pdwArg + dwArgSize;

    va_start(vl, pvObject);

    for (pszSigs = m_szSigs[methodID]; *pszSigs; pszSigs += 2) {

        switch (*pszSigs) {
            case __OUT:
                va_arg(vl, DWORD);
                break;
            case __PIN:
                pvData = (LPVOID)va_arg(vl, DWORD);
                dwDataSize = va_arg(vl, DWORD);
                if (pvData) {
                    memcpy(pData, pvData, dwDataSize);
                    *pdwArg++ = (DWORD)pData - (DWORD)pdwArg;
                    pData += dwDataSize;
                }
                else {
                    *pdwArg++ = 0;
                }
                break;
            case __POUT:
                if (va_arg(vl, DWORD)) {
                    *pdwArg++ = va_arg(vl, DWORD);
                }
                else {
                    va_arg(vl, DWORD);
                    *pdwArg++ = 0;
                }
                break;
            case __BI:
            case __ADD:
                pdwOut = (LPDWORD)va_arg(vl, DWORD);
                if (pdwOut) {
                    *pdwArg++ = *pdwOut;
                }
                else {
                    *pdwArg++ = 0;
                }
                break;
            case __IN:
            case __DEL:
                *pdwArg++ = va_arg(vl, DWORD);
                break;
        }
    }

    va_end(vl);

/*
{
UINT i;
LPDWORD pdwArgs = (LPDWORD)&ppkCallMethod->pvArguments;
DebugString(TEXT("Args for packet %d:"), ppkCallMethod->packetID);
for (i = 0; i < dwArgSize / 4; i++) {
DebugString(TEXT("0x%X"), *pdwArgs++);
}
}
*/

    // Perform a remote call of the method
    if (!SendPacket(ppkCallMethod)) {
        DebugString(TEXT("Failed to send the call method packet for method %d"), methodID);
        MemFree32(ppkCallMethod);
        Disconnect();
        return FALSE;
    }

    MemFree32(ppkCallMethod);

    // Obtain the method results
    if (!RecvPacket()) {
        DebugString(TEXT("Failed to receive the result packet for method call %d"), methodID);
        Disconnect();
        return FALSE;
    }

    // Verify the method was actually executed (even if it failed)
    if (m_pPacket->packetID != (DWORD)methodID) {
        DebugString(TEXT("The server returned a fault when attempting to call method %d"), methodID);
        Disconnect();
        return FALSE;
    }

    // Copy the returned data to the locations passed in
    pdwArg = (LPDWORD)((LPBYTE)m_pPacket + sizeof(PACKET));

    va_start(vl, pvObject);

    for (pszSigs = m_szSigs[methodID]; *pszSigs; pszSigs += 2) {

        if (*pszSigs == __POUT || *pszSigs == __OUT || *pszSigs == __ADD || *pszSigs == __BI) {

            pdwOut = (LPDWORD)va_arg(vl, DWORD);

            if (*pszSigs == __POUT) {
                dwDataSize = va_arg(vl, DWORD);
                if (pdwOut) {
                    memcpy(pdwOut, (LPBYTE)pdwArg + *pdwArg, dwDataSize);
                }
            }
            else {
                if (pdwOut) {
                    *pdwOut = *pdwArg;
                }
            }

            pdwArg++;
        }
        else {
            va_arg(vl, DWORD);
            if (*pszSigs == __PIN) {
                va_arg(vl, DWORD);
            }
        }
    }

    va_end(vl);

    return TRUE;
}

//***********************************************************************************
BOOL CClient::GetRefFrameCRC32(CDevice8* pDevice, LPDWORD pdwCRC) {

    PACKET_CS_GETREFFRAMECRC32 pkGetCRC;

    // Verify the connection
    if (m_sServer == INVALID_SOCKET) {
        return FALSE;
    }

    pkGetCRC.pvDevice = pDevice;

    if (!SendPacket(&pkGetCRC)) {
        Disconnect();
        return FALSE;
    }

    if (!RecvPacket()) {
        Disconnect();
        return FALSE;
    }

    if (m_pPacket->packetID != PACKETID_SC_REFFRAMECRC32) {
        Disconnect();
        return FALSE;
    }

    *pdwCRC = ((PPACKET_SC_REFFRAMECRC32)m_pPacket)->dwRefCRC;

    return TRUE;
}

//***********************************************************************************
BOOL CClient::GetRefFrame(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, LPVOID pvBits) {

    PACKET_CS_GETREFFRAME   pkGetRef;
    PACKET_SC_REFFRAME      pkRef;
    UINT                    i;

    // Verify the connection
    if (m_sServer == INVALID_SOCKET) {
        return FALSE;
    }

    pkGetRef.pvDevice = pDevice;

    if (!SendPacket(&pkGetRef)) {
        Disconnect();
        return FALSE;
    }

    // Get the packet header
    if (!Recv(&pkRef, sizeof(PACKET)) || pkRef.packetID != PACKETID_SC_REFFRAME ||
        pkRef.dwSize != sizeof(PACKET) + dwWidth * dwHeight * 4)
    {
        Disconnect();
        return FALSE;
    }

    // Get the reference frame buffer contents
    for (i = 0; i < dwHeight; i++) {
        if (!Recv((LPBYTE)pvBits + i * dwPitch, dwWidth * 4)) {
            Disconnect();
            return FALSE;
        }
    }

    return TRUE;
}

//***********************************************************************************
int CClient::Send(LPVOID pv, int nSize, SOCKET s) {

    int size, total = 0;

    if (s == INVALID_SOCKET) {
        s = m_sServer;
    }

    do {
        size = send(s, (char*)pv + total, nSize - total, 0);
        if (size == SOCKET_ERROR) {
            return 0;
        }
        total += size;

    } while (total < nSize);

    return total;
}

//***********************************************************************************
int CClient::SendPacket(PPACKET ppk, SOCKET s) {

    return Send(ppk, ppk->dwSize, s);
}

//***********************************************************************************
int CClient::Recv(LPVOID pv, int nSize, SOCKET s) {

    int size, total = 0;

    if (s == INVALID_SOCKET) {
        s = m_sServer;
    }

    do {
        size = recv(s, (char*)pv + total, nSize - total, 0);
        if (!size || size == SOCKET_ERROR) {
            return 0;
        }
        total += size;

    } while (total < nSize);

    return total;
}

//***********************************************************************************
BOOL CClient::RecvPacket(PPACKET ppk, SOCKET s) {

    DWORD   dwSize;

    if (!ppk) {

        // If a packet has already been allocated, free it
        if (m_pPacket) {
            delete [] m_pPacket;
            m_pPacket = NULL;
        }

        // Get the size of the packet
        if (!Recv((LPVOID)&dwSize, sizeof(DWORD))) {
            return FALSE;
        }

        m_pPacket = (PPACKET)new BYTE[dwSize];
        if (!m_pPacket) {
            return FALSE;
        }
        m_pPacket->dwSize = dwSize;

        // Retrieve the rest of the packet
        if (!Recv((LPVOID)((LPBYTE)m_pPacket + sizeof(DWORD)), dwSize - sizeof(DWORD))) {
            return FALSE;
        }
    }
    else {
        // Retrieve the packet
        if (!Recv((LPVOID)ppk, ppk->dwSize, s)) {
            return FALSE;
        }
    }

    return TRUE;
}

//******************************************************************************
int gethostaddr(IN_ADDR* psin) {

#ifndef UNDER_XBOX
    // Return the first IP address of the host
    char            hostname[256];
    PHOSTENT        phostent;
#else
    XNADDR          XnAddr;
    DWORD           dwStart;
#endif

    // Parameter validation
    if (!psin) {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }

#ifndef UNDER_XBOX
    // Get the name of the host
    if (gethostname(hostname, sizeof(hostname))) {
        return SOCKET_ERROR;
    }

    // Use the name to get the host information
    phostent = gethostbyname(hostname);
    if (!phostent) {
        return SOCKET_ERROR;
    }

    // Get a string of the IP address from the host information
    psin->S_un.S_addr = *((ULONG*)phostent->h_addr_list[0]);
#else
    // Get the host address
    dwStart = GetTickCount();
    do {
        if (GetTickCount() > dwStart + 5000) {
            return SOCKET_ERROR;
        }
    } while (XNetGetTitleXnAddr(&XnAddr) == 0);

    memcpy(psin, &XnAddr.ina.s_addr, sizeof(*psin));
#endif

    return 0;
}

//******************************************************************************
int gethostaddr(char* addr, int addrlen) {

    // Return the first IP address of the host
    IN_ADDR         sin;
    int             ret = 0;

    // Parameter validation
    if (!addr || addrlen <= 0) {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }

    // Get the address of the host
    ret = gethostaddr(&sin);

    if (!ret) {

        // Verify the buffer is large enough for the address
        if (16 > (UINT)addrlen) {
            DebugString(TEXT("The address buffer is %d bytes.  It needs to be %d ")
                     TEXT("bytes\n"), addrlen, 16);
            WSASetLastError(WSAEFAULT);
            return SOCKET_ERROR;
        }

        // Copy the address string into the buffer
        sprintf(addr, "%u.%u.%u.%u", sin.S_un.S_un_b.s_b1, sin.S_un.S_un_b.s_b2, sin.S_un.S_un_b.s_b3, sin.S_un.S_un_b.s_b4);
    }

    return ret;
}
