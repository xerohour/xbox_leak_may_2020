/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    client.h

Author:

    Matt Bronder

Description:

    Client code for remote reference verification.

*******************************************************************************/

#ifndef __CLIENT_H__
#define __CLIENT_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define PORT_SERVICE        2202
#define PORT_ENUMERATE      2203

#define ARG_BUFFER_SIZE     512

//******************************************************************************
class CClient {

private:

    SOCKET                  m_sEnumerate;
    SOCKET                  m_sListen;
    SOCKET                  m_sServer;

    BOOL                    m_bWSAStartup;
#ifdef UNDER_XBOX
    BOOL                    m_bXnetStartup;
#endif // UNDER_XBOX

    BOOL                    m_bMCallsEnabled;

    PPACKET                 m_pPacket;

    PACKET_ENUM             m_pkEnum;

    static LPCSTR           m_szSigs[];

public:

                            CClient();
                            ~CClient();
    BOOL                    Create();

    void                    Disconnect();
    BOOL                    IsConnected();

    void                    EnableMethodCalls(BOOL bEnable);
    BOOL                    MethodCallsEnabled();

    BOOL                    CheckDeviceAffinity(CDirect3D8* pDirect3D, BOOL* bEquivalent);
    BOOL                    CallDirect3DCreate8(LPVOID pvDirect3D);
    BOOL                    CallMethod(METHODID methodID, LPVOID pvObject, ...);
    BOOL                    GetRefFrameCRC32(CDevice8* pDevice, LPDWORD pdwCRC);
    BOOL                    GetRefFrame(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, LPVOID pvBits);

    int                     SendPacket(PPACKET ppk, SOCKET s = INVALID_SOCKET);
    BOOL                    RecvPacket(PPACKET ppk = NULL, SOCKET s = INVALID_SOCKET);

    int                     Send(LPVOID pv, int nSize, SOCKET s = INVALID_SOCKET);
    int                     Recv(LPVOID pv, int nSize, SOCKET s = INVALID_SOCKET);
};

//******************************************************************************
typedef struct _SLOCKDESC {
    D3DLOCKED_RECT          d3dlr;
    RECT                    rect;
    DWORD                   dwFlags;
    D3DLOCKED_RECT          d3dlrSrv;
} SLOCKDESC, *PSLOCKDESC;

typedef struct _VLOCKDESC {
    D3DLOCKED_BOX           d3dlb;
    D3DBOX                  box;
    DWORD                   dwFlags;
    D3DLOCKED_BOX           d3dlbSrv;
} VLOCKDESC, *PVLOCKDESC;

typedef struct _RLOCKDESC {
    LPBYTE              pData;
    UINT                uSize;
//    DWORD               dwFlags;
    LPBYTE              pDataSrv;
} RLOCKDESC, *PRLOCKDESC;

typedef struct _ILOCKDESC {
    LPBYTE              pData;
    UINT                uSize;
//    DWORD               dwFlags;
    LPBYTE              pDataSrv;
} ILOCKDESC, *PILOCKDESC;

int                         gethostaddr(IN_ADDR* psin);
int                         gethostaddr(char* addr, int addrlen);

#endif // __CLIENT_H__
