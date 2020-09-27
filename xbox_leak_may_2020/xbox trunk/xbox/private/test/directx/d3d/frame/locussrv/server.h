/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    server.h

Author:

    Matt Bronder

Description:

    Server code for remote reference verification.

*******************************************************************************/

#ifndef __SERVER_H__
#define __SERVER_H__

//***********************************************************************************
// Preprocessor definitions
//***********************************************************************************

#define countof(x)      (sizeof(x) / sizeof(*(x)))

#define REMAPIN(t, x)   g_pServer->RemapType(t, (DWORD)(x), (LPDWORD)&(x))
#define REMAPOUT(t, x)  g_pServer->RemapType(t, (LPDWORD)&(x), (DWORD)(x))
#define REMAP(in, t, x) if (in) \
                            REMAPIN(t, x); \
                        else \
                            REMAPOUT(t, x);

//***********************************************************************************
// Data types
//***********************************************************************************

typedef void (__cdecl *METHODPROC)(...);

typedef struct _METHODDESC {
   METHODPROC pfnCall;
   LPCSTR     szSig;
} METHODDESC, *PMETHODDESC;

//***********************************************************************************
// CServer
//***********************************************************************************
class CServer {

private:

    TCHAR                   m_szWndClassName[32];
    BOOL                    m_bRegistered;

    SOCKET                  m_sEnumerate;
    SOCKET                  m_sClient;
    SOCKET                  m_sListen;
    BOOL                    m_bWSAStartup;
    HANDLE                  m_hThread;
    HANDLE                  m_hExitEvent;
    PPACKET                 m_pPacket;
    PACKET_SC_FAULT         m_pkFault;

    static METHODDESC       m_MethodList[];

    CMap32                  m_pMap[TRANSLATION_TYPES];
    CMap32                  m_pTypeMap[NUM_STATIC_TRANSLATIONS];

    DWORD                   m_dwCRCTable[256];

public:

                            CServer();
                            ~CServer();
    BOOL                    Create();

    void                    Connect();
    void                    Disconnect();
    void                    DispatchSocket(SOCKET s);
    void                    CloseSocket(SOCKET s);

    BOOL                    IsConnected();

    ULONG                   ReleaseObject(IUnknown* punk);
    BOOL                    AddMapping(UINT uType, DWORD dwKey, DWORD dwValue);
    BOOL                    RemoveMapping(UINT uType, DWORD dwKey);
    BOOL                    FindMapping(UINT uType, LPDWORD pdwKey, DWORD dwValue);
    void                    RemapType(D3DTYPEINDEX d3dti, DWORD dwXboxType, LPDWORD pdwWinxType);
    void                    RemapType(D3DTYPEINDEX d3dti, LPDWORD pdwXboxType, DWORD dwWinxType);
    void                    RemapType(BOOL bIn, D3DTYPEINDEX d3dti, LPDWORD pdwXboxType, LPDWORD pdwWinxType);
    void                    RemapStruct(BOOL bIn, D3DSURFACE_DESC* pd3dsd);
    void                    RemapStruct(BOOL bIn, D3DVOLUME_DESC* pd3dvd);
    void                    RemapStruct(BOOL bIn, D3DPRESENT_PARAMETERS* pd3dpp);

    void                    SendState();

    int                     SendPacket(PPACKET ppk, SOCKET s = INVALID_SOCKET);
    BOOL                    RecvPacket(PPACKET ppk = NULL, SOCKET s = INVALID_SOCKET);
    BOOL                    ProcessPacket();

    int                     Send(LPVOID pv, int nSize, SOCKET s = INVALID_SOCKET);
    int                     Recv(LPVOID pv, int nSize, SOCKET s = INVALID_SOCKET);

    DWORD                   ThreadProc();
    friend DWORD WINAPI     ThreadProcFriend(LPVOID lpv);

    LPDIRECT3DSURFACE8      GetFrontBuffer(LPDIRECT3DDEVICE8 pd3dd);
    DWORD                   ComputeSurfaceCRC32(LPDIRECT3DSURFACE8 pd3ds);

private:

    BOOL                    HandleDeviceAffinity(PPACKET_CS_CHECKDEVICEAFFINITY ppk);
    BOOL                    HandleDirect3DCreate8(PPACKET_CS_DIRECT3DCREATE8 ppk);
    BOOL                    HandleMethodCall(PPACKET_CS_CALLMETHOD ppk);
    BOOL                    HandleRefFrameCRC32(PPACKET_CS_GETREFFRAMECRC32 ppk);
    BOOL                    HandleRefFrame(PPACKET_CS_GETREFFRAME ppk);
};

//***********************************************************************************
// Function prototypes
//***********************************************************************************

DWORD WINAPI                ThreadProcFriend(LPVOID lpv);

int                         gethostaddr(IN_ADDR* psin);
int                         gethostaddr(char* addr, int addrlen);

//***********************************************************************************
extern HWND                 g_hWndD3D;
extern CServer*             g_pServer;

#endif // __SERVER_H__
