/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    server.cpp

Author:

    Matt Bronder

Description:

    Server code for remote reference verification.

*******************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <winsock2.h>
#include <d3dx8.h>
#include <xboxdbg.h>
#include "transprt.h"
#include "util.h"
#include "typetran.h"
#include "server.h"
#include "resource.h"

// Function prototypes
#define SIG_MACRO(f,s) extern "C" void __cdecl M_##f();
#include "sigs.h"

#define SIG_MACRO(f,s) { (METHODPROC)M_##f, s },
METHODDESC CServer::m_MethodList[] = {
#include "sigs.h"
};

BOOL                    RefreshTaskbar();

extern HINSTANCE        g_hInstance;
extern HWND             g_hWnd;
HWND                    g_hWndD3D;

//***********************************************************************************
LRESULT CALLBACK WndProcD3D(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch (message) {

        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rect);
            EndPaint(hWnd, &ps);
            return 0;

        case WM_SETCURSOR:
            SetCursor(NULL);
            return TRUE;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_ESCAPE: // Exit
                    SendMessage(g_hWnd, WM_CLOSE, 0, 0);
                    return 0;
            }

            break;
   }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

//***********************************************************************************
CServer::CServer() {

    DWORD   dwCRC;
    UINT    i, j;

    m_bWSAStartup = FALSE;
    m_sEnumerate = INVALID_SOCKET;
    m_sClient = INVALID_SOCKET;
    m_pPacket = NULL;
    m_hThread = NULL;
    m_hExitEvent = NULL;
    m_bRegistered = FALSE;
    _tcscpy(m_szWndClassName, TEXT("D3DLocus Server"));

    // Initialize the CRC table
    for (i = 0; i < 256; i++) {
        dwCRC = i;
        for (j = 0; j < 8; j++) {
            dwCRC = (dwCRC & 1) ? ((dwCRC >> 1) ^ 0xEDB88320L) : (dwCRC >> 1);
        }
        m_dwCRCTable[i] = dwCRC;
    }
}

//***********************************************************************************
CServer::~CServer() {

    HANDLE hThread;

    // Close the enumeration socket
    if (m_sEnumerate != INVALID_SOCKET) {
        shutdown(m_sEnumerate, SD_BOTH);
        closesocket(m_sEnumerate);
        m_sEnumerate = INVALID_SOCKET;
    }

    // Close the client socket
    if (m_sClient != INVALID_SOCKET) {
        shutdown(m_sClient, SD_BOTH);
        closesocket(m_sClient);
        m_sClient = INVALID_SOCKET;
    }

    // Trigger our termination event to cause the worker thread to finish
    SetEvent(m_hExitEvent);

    // Wait for the worker thread to exit
    if (WaitForSingleObject(m_hThread, 3000) != WAIT_OBJECT_0) {
        TerminateThread(m_hThread, STILL_ACTIVE);
        DebugString(TEXT("Unable to close the rendering thread (0x%X)"), m_hThread);
    }
    CloseHandle(m_hThread);

    if (m_hExitEvent) {
        CloseHandle(m_hExitEvent);
    }

    // Delete the packet
    if (m_pPacket) {
        delete [] m_pPacket;
        m_pPacket = NULL;
    }
    
    if (m_bWSAStartup) {
        WSACleanup();
    }

    if (m_bRegistered) {
        UnregisterClass(m_szWndClassName, g_hInstance);
    }
}

//***********************************************************************************
BOOL CServer::Create() {

    WNDCLASS    wc;
    WSADATA     wsa;
    SOCKADDR_IN addr;
    DWORD       dwThreadID;

    // Register the window class
    memset(&wc, 0, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WndProcD3D;   // Window Procedure                       
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = g_hInstance;          // Owner of this class
    wc.hIcon         = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.hCursor       = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = 0;
    wc.lpszClassName = m_szWndClassName;

    RegisterClass(&wc);
    m_bRegistered = TRUE;

    // Map NULL to NULL in the address map
    if (!m_pMap[__ADDRESS].Add(0, 0)) {
        return FALSE;
    }

    // Initialize the static Direct3D type translation table
    InitTypeTranslationMaps(m_pTypeMap);

    // Perform socket initilization for version 2 of the socket API's
    if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
        DebugString(TEXT("WSAStartup() failed [%d]"), WSAGetLastError());
        return FALSE;
    }
    m_bWSAStartup = TRUE;

    // Create a datagram socket to listen for broadcasted enumeration requests
    m_sEnumerate = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sEnumerate == INVALID_SOCKET) {
        DebugString(TEXT("socket() failed [%d]"), WSAGetLastError());
        return FALSE;        
    }

    // Bind the socket to the broadcast port
    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_BROADCAST);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_sEnumerate, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
        DebugString(TEXT("bind() failed [%d]"), WSAGetLastError());
        return FALSE;
    }

    // Switch to asynchronous mode
//    if (WSAAsyncSelect(m_sEnumerate, g_hWnd, WM_SOCKET, 
//                       FD_READ | FD_CLOSE) == SOCKET_ERROR) 
//    {
//        DebugString(TEXT("WSAAsyncSelect() failed [%d]"), WSAGetLastError());
//        return FALSE;
//    }

    // Create an event to signal the transfer threads on exit
    m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!m_hExitEvent) {
        return FALSE;
    }

    // Create an enumeration thread to service the enumeration socket
    m_hThread = CreateThread(NULL, 0, ThreadProcFriend, this, 0, &dwThreadID);
    if (!m_hThread) {
        return FALSE;
    }

    return TRUE;
}

//***********************************************************************************
void CServer::Disconnect() {

    if (m_sClient != INVALID_SOCKET) {
        shutdown(m_sClient, SD_BOTH);
        closesocket(m_sClient);
        m_sClient = INVALID_SOCKET;
        RefreshTaskbar();
    }
}

//***********************************************************************************
BOOL CServer::IsConnected() {

    return (m_sClient != INVALID_SOCKET);
}

//***********************************************************************************
DWORD WINAPI ThreadProcFriend(LPVOID lpv) {
    
    return ((CServer*)lpv)->ThreadProc();
}

//***********************************************************************************
DWORD CServer::ThreadProc() {

    MSG             msg;
    SOCKADDR_IN     addr;
    PACKET_ENUM     pkEnum, pkRecv;
    int             nSize;
    DWORD           dwAddress;
    HRESULT         hr;
    BOOL            bKeepAlive = TRUE;

    while (TRUE) {

        // Wait for an enumeration request from the client and connect if a request is
        // received
        while (TRUE) {

            if (m_sEnumerate == INVALID_SOCKET) {
                return 0;
            }

            ZeroMemory(&pkRecv, sizeof(pkRecv));
            nSize = recvfrom(m_sEnumerate, (LPSTR)&pkRecv, sizeof(pkRecv), 0, 
                NULL, 0);

            if (nSize == sizeof(pkEnum) && 
                !memcmp(&(pkRecv.enumid), &(pkEnum.enumid), sizeof(ENUMERATION_ID))) 
            {
                // Verify the address belongs to the client specified through the debug
                // interface (i.e. "xbdir -x <client_name>")
                hr = DmGetAltAddress(&dwAddress);
                if (SUCCEEDED(hr) && pkRecv.s_addrClient.S_un.S_addr == ((dwAddress >> 24) | (dwAddress << 24) | ((dwAddress & 0xFF0000) >> 8) | ((dwAddress & 0xFF00) << 8))) {

                    // An enumeration request has been received from the client.  Attempt to connect
                    // using the address specified in the message
                    m_sClient = socket(AF_INET, SOCK_STREAM, 0);
                    if (m_sClient == INVALID_SOCKET) {
                        DebugString(TEXT("socket() failed [%d]"), WSAGetLastError());
                        continue;
                    }

                    // Bind the socket
                    ZeroMemory(&addr, sizeof(addr));
                    addr.sin_family = AF_INET;
                    addr.sin_port = 0;
                    addr.sin_addr.s_addr = htonl(INADDR_ANY);

                    if (bind(m_sClient, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
                        DebugString(TEXT("bind() failed [%d]"), WSAGetLastError());
                        closesocket(m_sClient);
                        continue;
                    }

                    // Connect to the client
                    ZeroMemory(&addr, sizeof(addr));
                    addr.sin_family = AF_INET;
                    addr.sin_port = htons((USHORT)pkRecv.uPort);
                    addr.sin_addr.S_un.S_addr = pkRecv.s_addrClient.S_un.S_addr;

                    if (connect(m_sClient, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
                        DebugString(TEXT("connect() failed [%d]"), WSAGetLastError());
                        closesocket(m_sClient);
                        continue;
                    }

                    // Enable keep-alive on the socket
                    if (setsockopt(m_sClient, SOL_SOCKET, SO_KEEPALIVE, (LPSTR)&bKeepAlive, 
                                                            sizeof(BOOL)) == SOCKET_ERROR) {
                        DebugString(TEXT("setsockopt() failed [%d]"), WSAGetLastError());
                    }

                    RefreshTaskbar();

                    break;
                }
            }
            else if (!nSize || (nSize == SOCKET_ERROR && WSAGetLastError() != WSAEMSGSIZE)) {
                return 0;
            }
        }

        // A connection with the client has been established, create a window for Direct3D
        // (the main application window cannot be used because a bug in DX8 causes CreateDevice
        // to hang if called using a window created in a different thread.
        g_hWndD3D = CreateWindow(m_szWndClassName, m_szWndClassName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
                                    640, 480, NULL, NULL, g_hInstance, NULL);
        ShowWindow(g_hWndD3D, SW_HIDE);
        UpdateWindow(g_hWndD3D);

        // Pump any messages
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //Begin communications
        while (TRUE) {

            // Wait for a transmission from the client
            if (!RecvPacket()) {
                // Return to listening for broadcasts if the connection has been dropped
                // TODO: Move this cleanup into recv code so that it only gets called if
                // the client dropped us rather than the socket being closed in the
                // destructor

                if (m_sClient != INVALID_SOCKET) {
                    shutdown(m_sClient, SD_BOTH);
                    closesocket(m_sClient);
                    m_sClient = INVALID_SOCKET;
                }

                // Destroy the window
                DestroyWindow(g_hWndD3D);

                RefreshTaskbar();

                break;
            }

            // Pump any messages received
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            // Process the packet from the client
            ProcessPacket();
        }
    }

    return 0;
}

//***********************************************************************************
void CServer::CloseSocket(SOCKET s) {

    if (s == m_sClient) {
        Disconnect();
    }
}

//***********************************************************************************
int CServer::Send(LPVOID pv, int nSize, SOCKET s) {

    int size, total = 0;

    if (s == INVALID_SOCKET) {
        s = m_sClient;
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
int CServer::SendPacket(PPACKET ppk, SOCKET s) {

    return Send(ppk, ppk->dwSize, s);
}

//***********************************************************************************
int CServer::Recv(LPVOID pv, int nSize, SOCKET s) {

    int size, total = 0;

    if (s == INVALID_SOCKET) {
        if (m_sClient == INVALID_SOCKET) {
            return 0;
        }
        s = m_sClient;
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
BOOL CServer::RecvPacket(PPACKET ppk, SOCKET s) {

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

//***********************************************************************************
BOOL CServer::ProcessPacket() {

    switch (m_pPacket->packetID) {

        case PACKETID_CS_CHECKDEVICEAFFINITY: {
            if (!HandleDeviceAffinity((PPACKET_CS_CHECKDEVICEAFFINITY)m_pPacket)) {
                SendPacket(&m_pkFault);
                return FALSE;
            }
            return TRUE;
        }

        case PACKETID_CS_DIRECT3DCREATE8: {

            if (!HandleDirect3DCreate8((PPACKET_CS_DIRECT3DCREATE8)m_pPacket)) {
                SendPacket(&m_pkFault);
                return FALSE;
            }

            return TRUE;
        }

        case PACKETID_CS_GETREFFRAMECRC32: {

            if (!HandleRefFrameCRC32((PPACKET_CS_GETREFFRAMECRC32)m_pPacket)) {
                SendPacket(&m_pkFault);
                return FALSE;
            }
            return TRUE;
        }

        case PACKETID_CS_GETREFFRAME: {

            if (!HandleRefFrame((PPACKET_CS_GETREFFRAME)m_pPacket)) {
                SendPacket(&m_pkFault);
                return FALSE;
            }
            return TRUE;
        }

        default: {

            if (!HandleMethodCall((PPACKET_CS_CALLMETHOD)m_pPacket)) {
                SendPacket(&m_pkFault);
                return FALSE;
            }

            return TRUE;
        }
    }

    return FALSE;
}

//******************************************************************************
ULONG CServer::ReleaseObject(IUnknown* punk) {

    ULONG uRef;
    DWORD dwKey;

    uRef = punk->Release();

    if (uRef == 0) {
        if (m_pMap[__ADDRESS].Find(&dwKey, (DWORD)punk)) {
            m_pMap[__ADDRESS].Remove(dwKey);
        }
    }

    return uRef;
}

//******************************************************************************
BOOL CServer::AddMapping(UINT uType, DWORD dwKey, DWORD dwValue) {

    return m_pMap[uType].Add(dwKey, dwValue);
}

//******************************************************************************
BOOL CServer::RemoveMapping(UINT uType, DWORD dwKey) {

    return m_pMap[uType].Remove(dwKey);
}

//***********************************************************************************
BOOL CServer::FindMapping(UINT uType, LPDWORD pdwKey, DWORD dwValue) {

    return m_pMap[uType].Find(pdwKey, dwValue);
}

//***********************************************************************************
void CServer::RemapType(D3DTYPEINDEX d3dti, DWORD dwXboxType, LPDWORD pdwWinxType) {

    *pdwWinxType = 0;
    m_pTypeMap[d3dti].Find(dwXboxType, pdwWinxType);
}

//***********************************************************************************
void CServer::RemapType(D3DTYPEINDEX d3dti, LPDWORD pdwXboxType, DWORD dwWinxType) {

    *pdwXboxType = 0;
    m_pTypeMap[d3dti].Find(pdwXboxType, dwWinxType);
}

//***********************************************************************************
void CServer::RemapType(BOOL bIn, D3DTYPEINDEX d3dti, LPDWORD pdwXboxType, LPDWORD pdwWinxType) {

    if (bIn) {
        *pdwWinxType = 0;
        m_pTypeMap[d3dti].Find(*pdwXboxType, pdwWinxType);
    }
    else {
        *pdwXboxType = 0;
        m_pTypeMap[d3dti].Find(pdwXboxType, *pdwWinxType);
    }
}

//***********************************************************************************
void CServer::RemapStruct(BOOL bIn, D3DSURFACE_DESC* pd3dsd) {

    REMAP(bIn, D3DTI_D3DFORMAT, pd3dsd->Format);
    REMAP(bIn, D3DTI_D3DRESOURCETYPE, pd3dsd->Type);
    REMAP(bIn, D3DTI_D3DPOOL, pd3dsd->Pool);
    REMAP(bIn, D3DTI_D3DMULTISAMPLE_TYPE, pd3dsd->MultiSampleType);
}

//***********************************************************************************
void CServer::RemapStruct(BOOL bIn, D3DVOLUME_DESC* pd3dvd) {

    REMAP(bIn, D3DTI_D3DFORMAT, pd3dvd->Format);
    REMAP(bIn, D3DTI_D3DRESOURCETYPE, pd3dvd->Type);
    REMAP(bIn, D3DTI_D3DPOOL, pd3dvd->Pool);
}

//***********************************************************************************
void CServer::RemapStruct(BOOL bIn, D3DPRESENT_PARAMETERS* pd3dpp) {

    REMAP(bIn, D3DTI_D3DFORMAT, pd3dpp->BackBufferFormat);
    REMAP(bIn, D3DTI_D3DMULTISAMPLE_TYPE, pd3dpp->MultiSampleType);
    REMAP(bIn, D3DTI_D3DSWAPEFFECT, pd3dpp->SwapEffect);
    REMAP(bIn, D3DTI_D3DFORMAT, pd3dpp->AutoDepthStencilFormat);
    pd3dpp->FullScreen_RefreshRateInHz = 0;
}

//***********************************************************************************
BOOL CServer::HandleDeviceAffinity(PPACKET_CS_CHECKDEVICEAFFINITY ppk) {

    PACKET_SC_DEVICEAFFINITY pkAffinity;
    LPDIRECT3D8              pd3d;

    if (!m_pMap[__ADDRESS].Find((DWORD)ppk->pvDirect3D, (LPDWORD)&pd3d) || !pd3d) {
        return FALSE;
    }

    pkAffinity.bEquivalent = TRUE;
    SendPacket(&pkAffinity);
    return TRUE;

    return FALSE;
}

//***********************************************************************************
BOOL CServer::HandleDirect3DCreate8(PPACKET_CS_DIRECT3DCREATE8 ppk) {

    LPDIRECT3D8 pd3d = Direct3DCreate8(D3D_SDK_VERSION);

    if (pd3d) {
        if (m_pMap[__ADDRESS].Add((DWORD)(ppk->pvDirect3D), (DWORD)pd3d)) {
            PACKET_SC_SUCCESS pk;
            SendPacket(&pk);
            return TRUE;
        }
    }

    return FALSE;
}

//******************************************************************************
BOOL CServer::HandleMethodCall(PPACKET_CS_CALLMETHOD ppk) {

    PPACKET_SC_METHODRESULT ppkResult;
    LPCSTR                  pszSigs;
    LPDWORD                 pdwArgSrc, pdwArgDst;
    DWORD                   dwObject;
    DWORD                   dwArg[16];
    PMETHODDESC             pmethod;
    DWORD                   dwArgSize, dwDataSize;
    LPBYTE                  pData;
    TRANSTYPE               ttype;
    UINT                    i, j;

    if (m_pPacket->packetID > countof(m_MethodList)) {
        DebugString(TEXT("HandleMethodCall was given an unknown method identifier"));
        return FALSE;
    }

    pmethod = &m_MethodList[m_pPacket->packetID];

    // Determine the size of the result packet
    pdwArgSrc = ppk->pdwArguments;//(LPDWORD)&(ppk->pvArguments);

    dwArgSize = 0;
    dwDataSize = 0;

    for (pszSigs = pmethod->szSig; *pszSigs; pszSigs += 2) {

        if (*pszSigs == __POUT || *pszSigs == __OUT || *pszSigs == __ADD || *pszSigs == __BI) {

            dwArgSize += 4;

            if (*pszSigs == __POUT) {
                dwDataSize += *pdwArgSrc;
                // ##TODO: Pad data so each data segment begins on a DWORD boundary
            }
        }

        if (*pszSigs != __OUT) {
            pdwArgSrc++;
        }
    }

    // Allocate memory for the result packet
    ppkResult = (PPACKET_SC_METHODRESULT)MemAlloc32(sizeof(PACKET_SC_METHODRESULT) + dwArgSize + dwDataSize - sizeof(DWORD));
    if (!ppkResult) {
        DebugString(TEXT("Unable to allocate memory for the result packet for method %d"), m_pPacket->packetID);
        return FALSE;
    }

    // Initialize the packet header
    ppkResult->dwSize = sizeof(PACKET_SC_METHODRESULT) + dwArgSize + dwDataSize - sizeof(DWORD);
    ppkResult->packetID = m_pPacket->packetID;

    // Initialize in parameters from the source packet.  Initialize out parameters as pointers into the result packet.
    pdwArgSrc = ppk->pdwArguments;//(LPDWORD)&(ppk->pvArguments);
    pdwArgDst = ppkResult->pdwResult;//(LPDWORD)&(ppkResult->pvResult);
    pData = (LPBYTE)pdwArgDst + dwArgSize;

    for (pszSigs = pmethod->szSig, i = 0; *pszSigs; pszSigs += 2, i++) {

        switch (*pszSigs) {
            case __POUT:
                if (*pdwArgSrc) {
                    dwArg[i] = (DWORD)pData;
                    *pdwArgDst = (DWORD)pData - (DWORD)pdwArgDst;
                    pData += *pdwArgSrc;
                }
                else {
                    dwArg[i] = 0;
                    *pdwArgDst = 0;
                }
                pdwArgDst++;
                break;
            case __OUT:
                dwArg[i] = (DWORD)pdwArgDst;
                pdwArgDst++;
                break;
            case __BI:
                *pdwArgDst = *pdwArgSrc;
                dwArg[i] = (DWORD)pdwArgDst;
                pdwArgDst++;
                break;
            case __ADD:
                ttype = GET_TRANSTYPE(*(pszSigs + 1));
                if (ttype != __ADDRESS || *pdwArgSrc) {
                    dwArg[i] = (DWORD)pdwArgDst;
                }
                else {
                    dwArg[i] = 0;
                }
                pdwArgDst++;
                break;
            case __PIN:
                if (*pdwArgSrc) {
                    dwArg[i] = (DWORD)((LPBYTE)pdwArgSrc + *pdwArgSrc);
                }
                else {
                    dwArg[i] = 0;
                }
                break;
            case __IN:
            case __DEL:
                ttype = GET_TRANSTYPE(*(pszSigs + 1));
                if (ttype == __DATA) {
                    dwArg[i] = *pdwArgSrc;
                }
                else {
                    if (!m_pMap[ttype].Find(*pdwArgSrc, (LPDWORD)&dwArg[i])) {
                        if (ttype == __VSHADER) {
                            dwArg[i] = *pdwArgSrc;
                        }
                        else {
                            MemFree32(ppkResult);
                            DebugString(TEXT("Unable to translate non-data argument for method %d"), m_pPacket->packetID);
                            return FALSE;
                        }
                    }
                }
                break;
        }

        if (*pszSigs != __OUT) {
            pdwArgSrc++;
        }
    }

    if (!m_pMap[__ADDRESS].Find((DWORD)ppk->pvObject, &dwObject)) {
        MemFree32(ppkResult);
        DebugString(TEXT("Unable to translate the object address for method %d"), m_pPacket->packetID);
        return FALSE;
    }

    // Call the method
    pmethod->pfnCall(dwObject,
                     dwArg[ 0], dwArg[ 1], dwArg[ 2], dwArg[ 3],
                     dwArg[ 4], dwArg[ 5], dwArg[ 6], dwArg[ 7],
                     dwArg[ 8], dwArg[ 9], dwArg[10], dwArg[11],
                     dwArg[12], dwArg[13], dwArg[14], dwArg[15]);


    // Translate out parameters and alter the translation maps as necessary
    pdwArgSrc = ppk->pdwArguments;//(LPDWORD)&(ppk->pvArguments);
    pdwArgDst = ppkResult->pdwResult;//(LPDWORD)&(ppkResult->pvResult);

    for (pszSigs = pmethod->szSig, i = 0; *pszSigs; pszSigs += 2, i++) {

        if (*pszSigs == __ADD) {

            ttype = GET_TRANSTYPE(*(pszSigs + 1));
            if (ttype != __ADDRESS || *pdwArgSrc) {
                if (ttype == __DATA || !m_pMap[ttype].Add((DWORD)*pdwArgSrc, (DWORD)*pdwArgDst)) {
                    MemFree32(ppkResult);
                    DebugString(TEXT("Unable to map a created output parameter between client and server for method %d"), m_pPacket->packetID);
                    return FALSE;
                }
            }
            *pdwArgDst = *pdwArgSrc;
        }
        else if (*pszSigs == __DEL) {

            ttype = GET_TRANSTYPE(*(pszSigs + 1));
            if (ttype != __ADDRESS || *pdwArgSrc) {
                if (ttype == __DATA || !m_pMap[ttype].Remove((DWORD)*pdwArgSrc)) {
                    MemFree32(ppkResult);
                    DebugString(TEXT("Unable to unmap a deleted parameter between client and server for method %d"), m_pPacket->packetID);
                    return FALSE;
                }
            }
        }
        else if (*pszSigs == __OUT) {
            ttype = GET_TRANSTYPE(*(pszSigs + 1));
            if (ttype != __DATA) {
                if (!m_pMap[ttype].Find(pdwArgDst, *pdwArgDst)) {
                    MemFree32(ppkResult);
                    DebugString(TEXT("Unable to translate an output parameter between client and server for method %d"), m_pPacket->packetID);
                    return FALSE;
                }
            }
        }

        if (*pszSigs == __POUT || *pszSigs == __OUT || *pszSigs == __ADD || *pszSigs == __BI) {
            pdwArgDst++;
        }

        if (*pszSigs != __OUT) {
            pdwArgSrc++;
        }
    }
/*
{
UINT i;
LPDWORD pdwArgs = (LPDWORD)&ppkResult->pvResult;
DebugString(TEXT("Args for packet %d:"), ppkResult->packetID);
for (i = 0; i < dwArgSize / 4; i++) {
DebugString(TEXT("0x%X"), *pdwArgs++);
}
}
*/
    // Send the result
    if (!SendPacket(ppkResult)) {
        MemFree32(ppkResult);
        DebugString(TEXT("Unable to send the result packet for method %d"), m_pPacket->packetID);
        return FALSE;
    }

    MemFree32(ppkResult);

    return TRUE;
}

//******************************************************************************
BOOL CServer::HandleRefFrameCRC32(PPACKET_CS_GETREFFRAMECRC32 ppk) {

    PACKET_SC_REFFRAMECRC32 pkRefCRC;
    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DSURFACE8      pd3ds;
    HRESULT                 hr;

    if (!m_pMap[__ADDRESS].Find((DWORD)ppk->pvDevice, (LPDWORD)&pd3dd) || !pd3dd) {
        return FALSE;
    }

    pd3ds = GetFrontBuffer(pd3dd);
    if (!pd3ds) {
        return FALSE;
    }

    pkRefCRC.dwRefCRC = ComputeSurfaceCRC32(pd3ds);
    SendPacket(&pkRefCRC);

    pd3ds->Release();

    return TRUE;
}

//******************************************************************************
BOOL CServer::HandleRefFrame(PPACKET_CS_GETREFFRAME ppk) {

    PACKET_SC_REFFRAME      pkRef;
    LPDIRECT3DDEVICE8       pd3dd;
    LPDIRECT3DSURFACE8      pd3ds;
    D3DSURFACE_DESC         d3dsd;
    D3DLOCKED_RECT          d3dlr;
    UINT                    i;
    HRESULT                 hr;

    if (!m_pMap[__ADDRESS].Find((DWORD)ppk->pvDevice, (LPDWORD)&pd3dd) || !pd3dd) {
        return FALSE;
    }

    pd3ds = GetFrontBuffer(pd3dd);
    if (!pd3ds) {
        return FALSE;
    }

    hr = pd3ds->GetDesc(&d3dsd);
    if (FAILED(hr)) {
        pd3ds->Release();
        return FALSE;
    }

//    ppkRef = (PPACKET_SC_REFFRAME)MemAlloc32(sizeof(PACKET_SC_REFFRAME) + d3dsd.Width * d3dsd.Height * 4 - 1);
//    if (!ppkRef) {
//        return FALSE;
//    }

    pkRef.dwSize = sizeof(PACKET) + d3dsd.Width * d3dsd.Height * 4;
//    ppkRef->packetID = PACKETID_SC_REFFRAME;

    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
    if (FAILED(hr)) {
//        MemFree32(ppkRef);
        return FALSE;
    }

    // Send the packet header
    Send(&pkRef, sizeof(PACKET));

    // Send the frame buffer contents in rows
    for (i = 0; i < d3dsd.Height; i++) {
        Send((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, d3dsd.Width * 4);
    }

//    for (i = 0; i < d3dsd.Height; i++) {
//        memcpy(ppkRef->pBits[i * d3dsd.Width * 4], (LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, d3dsd.Width * 4);
//    }

    pd3ds->UnlockRect();
    pd3ds->Release();

    return TRUE;
}

/*
//******************************************************************************
UINT CServer::TransTypeToEntry(char cType) {

    UINT        i;
    static char cTransType[] = {
        _OBJECT,
        _VSHADER,
        _PSHADER,
        _SBLOCK
    };

    for (i = 0; i < 4; i++) {
        if (cType == cTransType[i]) {
            return i;
        }
    }

    DebugString(TEXT("TransTypeToEntry was given an invalid translation type"));
    return -1;
}
*/

//******************************************************************************
LPDIRECT3DSURFACE8 CServer::GetFrontBuffer(LPDIRECT3DDEVICE8 pd3dd) {

    LPDIRECT3DSURFACE8 pd3ds;
    D3DDISPLAYMODE     d3ddm;
    D3DSURFACE_DESC    d3dsd;
    HRESULT            hr;

    hr = pd3dd->GetDisplayMode(&d3ddm);
    if (FAILED(hr)) {
        return NULL;
    }

    hr = pd3dd->CreateImageSurface(d3ddm.Width, d3ddm.Height, D3DFMT_A8R8G8B8, &pd3ds);
    if (FAILED(hr)) {
        return NULL;
    }

    hr = pd3dd->GetFrontBuffer(pd3ds);
    if (FAILED(hr)) {
        pd3ds->Release();
        return NULL;
    }

    return pd3ds;
}

//******************************************************************************
DWORD CServer::ComputeSurfaceCRC32(LPDIRECT3DSURFACE8 pd3ds) {

    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT  d3dlr;
    LPDWORD         pdwPixel;
    DWORD           dwCRC = 0;
    LPBYTE          pbData;
    UINT            i, j, k;
    HRESULT         hr;

    hr = pd3ds->GetDesc(&d3dsd);
    if (FAILED(hr)) {
        return 0;
    }

    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
    if (FAILED(hr)) {
        return 0;
    }

    pdwPixel = (LPDWORD)d3dlr.pBits;
    for (i = 0; i < d3dsd.Height; i++) {
        for (j = 0; j < d3dsd.Width; j++) {
            pbData = (LPBYTE)&pdwPixel[j];
            for (k = 0; k < 3; k++) {
                dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
            }
        }
        pdwPixel += d3dlr.Pitch / 4;
    }

    pd3ds->UnlockRect();

    return dwCRC;
}

//******************************************************************************
int gethostaddr(IN_ADDR* psin) {

    // Return the first IP address of the host
    char            hostname[256];
    PHOSTENT        phostent;

    // Parameter validation
    if (!psin) {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }

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

    return 0;
}

//******************************************************************************
int gethostaddr(char* addr, int addrlen) {

    // Return the first IP address of the host
    char*           address;
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

        address = inet_ntoa(sin);
        if (!address) {
            return SOCKET_ERROR;
        }

        // Verify the buffer is large enough for the address
        if (strlen(address)+1 > (UINT)addrlen) {
            DebugString(TEXT("The address buffer is %d bytes.  It needs to be %d ")
                     TEXT("bytes\n"), addrlen, strlen(address)+1);
            WSASetLastError(WSAEFAULT);
            return SOCKET_ERROR;
        }

        // Copy the address string into the buffer
        strcpy(addr, address);
    }

    return ret;
}
