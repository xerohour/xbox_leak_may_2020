// ----------------------------------------------------------------------------
// testxnet.cpp
//
// Copyright (C) Microsoft Corporation
// ----------------------------------------------------------------------------

#include "xnp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vlan.h>

// ---------------------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------------------

#define Verify(x)       do { if (!(x)) DbgBreak(); } while (0)

struct CDiscReq
{
    BYTE    abNonce[8];         // Nonce of the requester
};

struct CDiscRsp : public CDiscReq
{
    XNKID   xnkid;              // The host's key identifier
    XNKEY   xnkey;              // The host's key
    XNADDR  xnaddr;             // The host's address
};

ULONG WSAAPI inet_addr(const char * pch)
{
    return(((CXNet *)0)->inet_addr(pch));
}

// ---------------------------------------------------------------------------------------
// CTestXnIp
// ---------------------------------------------------------------------------------------

class CTestXnIp : public CXnIp
{

public:
    
    virtual void Startup(char * pszXbox, char * pszIp, char * pszMask, char * pszGateway, DWORD dwFlags)
    {
        XNetParams xnp = { sizeof(XNetStartupParams) };
        xnp.cfgFlags = (BYTE)(XNET_STARTUP_MANUAL_CONFIG|dwFlags);
        XNetInitParams xnip = { &xnp, pszXbox };
        Verify(IpInit(&xnip) == 0);

        XNetConfigParams xncp = { 0 };
        xncp.ina.s_addr = pszIp ? inet_addr(pszIp) : 0;
        xncp.inaMask.s_addr = pszMask ? inet_addr(pszMask) : 0;
        xncp.inaGateway.s_addr = pszGateway ? inet_addr(pszGateway) : 0;
        Verify(IpConfig(&xncp, XNET_CONFIG_NORMAL) == 0);

        IpStart();
    }

    virtual void Init()
    {
        Startup(NULL, NULL, NULL, NULL, 0);
    }

    virtual void Term()
    {
        RaiseToDpc();
        IpTerm();
    }

    void Go(HANDLE hEvent1, HANDLE hEvent2)
    {
        Init();
        if (hEvent1) SetEvent(hEvent1);
        if (hEvent2) Verify(WaitForSingleObject(hEvent2, INFINITE) == WAIT_OBJECT_0);
        Test();
        if (hEvent1) SetEvent(hEvent1);
        if (hEvent2) Verify(WaitForSingleObject(hEvent2, INFINITE) == WAIT_OBJECT_0);
        Term();
    }

    void UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen) {};
    void TcpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbHdrLen, UINT cbLen) {};
    void SockReset(CIpAddr ipa) {};

    virtual void Test() = 0;
};

#define IPTEST_BEG(_name, pszXbox, pszIp, pszMask, pszGateway, dwFlags) \
    class CTest##_name : public CTestXnIp { \
    virtual void Init() { Startup(pszXbox, pszIp, pszMask, pszGateway, dwFlags); } \
    virtual void Test()
#define IPTEST_END(_name) \
    }; void Test_##_name(HANDLE hEvent1 = 0, HANDLE hEvent2 = 0) \
    { CTest##_name * p = new CTest##_name; p->Go(hEvent1, hEvent2); delete p; }

// ---------------------------------------------------------------------------------------
// CTestXNet
// ---------------------------------------------------------------------------------------

HANDLE  g_hEventXb1ToXb2;
HANDLE  g_hEventXb2ToXb1;
XNADDR  g_xnaddrXb1;
XNADDR  g_xnaddrXb2;
XNKID   g_xnkidXb1ToXb2;
XNKEY   g_xnkeyXb1ToXb2;
BOOL    g_fXb1ToXb2Timeout;
UINT    g_cSecsXb1ToXb2Connect;

class CTestXNet : public CXNet
{
public:
    
    CTestXNet(char * pszXbox = NULL) : CXNet(pszXbox) {}

    virtual void Startup(char * pszIp, char * pszMask, char * pszGateway, DWORD dwFlags)
    {
        XNetStartupParams xnp = { sizeof(XNetStartupParams) };
        xnp.cfgFlags = (BYTE)(XNET_STARTUP_MANUAL_CONFIG|dwFlags);
        Verify(XNetStartup(&xnp) == 0);

        XNetConfigParams xncp = { 0 };
        xncp.ina.s_addr = pszIp ? inet_addr(pszIp) : 0;
        xncp.inaMask.s_addr = pszMask ? inet_addr(pszMask) : 0;
        xncp.inaGateway.s_addr = pszGateway ? inet_addr(pszGateway) : 0;
        Verify(XNetConfig(&xncp, XNET_CONFIG_NORMAL) == 0);

        WSADATA WSAData;
        Verify(WSAStartup(0x0200, &WSAData) == 0);
    }

    virtual void Init()
    {
        Startup(NULL, NULL, NULL, 0);
    }

    virtual void Term()
    {
        WSACleanup();
        XNetCleanup();
    }

    void Go(HANDLE hEvent1, HANDLE hEvent2)
    {
        Init();
        if (hEvent1) SetEvent(hEvent1);
        if (hEvent2) Verify(WaitForSingleObject(hEvent2, INFINITE) == WAIT_OBJECT_0);
        Test();
        if (hEvent1) SetEvent(hEvent1);
        if (hEvent2) Verify(WaitForSingleObject(hEvent2, INFINITE) == WAIT_OBJECT_0);
        Term();
    }

    virtual void Test() = 0;

    SOCKET  SockUdpCreate(CIpPort ipport = 0, BOOL fBroadcast = FALSE);
    void    SockUdpTransmit(SOCKET s, CIpAddr ipaDst, CIpPort ipport, UINT cbMax = UDP_MAXIMUM_MSS);
    void    SockUdpReflect(SOCKET s);
    SOCKET  SockTcpCreate(CIpPort ipport = 0);
    void    SockTcpTransmit(SOCKET s, UINT cbMax = TCP_MAXIMUM_MSS);
    void    SockTcpReflect(SOCKET s);
    void    DiscoveryHost(UINT cClient, XNKID * pxnkid);
    void    DiscoveryClient(XNKID * pxnkid, CIpAddr * pipa);
    void    SgTcpXbToXbClient();
    void    SgTcpXbToXbServer();

};

SOCKET CTestXNet::SockUdpCreate(CIpPort ipport, BOOL fBroadcast)
{
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    Verify(s != SOCKET_ERROR);

    if (ipport)
    {
        sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = 0;
        sin.sin_port = ipport;
        Verify(bind(s, (struct sockaddr *)&sin, sizeof(sin)) == 0);
    }

    if (fBroadcast)
    {
        char cBroadcast = 1;
        Verify(setsockopt(s, SOL_SOCKET, SO_BROADCAST, &cBroadcast, 1) == 0);
    }

    return(s);
}

void CTestXNet::SockUdpTransmit(SOCKET s, CIpAddr ipaDst, CIpPort ipport, UINT cbMax)
{
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ipaDst;
    sin.sin_port = ipport;
    
    BYTE abXmit[1500], abRecv[1500];

    if (cbMax > sizeof(abXmit))
        cbMax = sizeof(abXmit);

    int cb;

    for (cb = 1; cb < (int)cbMax; ++cb)
    {
        XNetRandom(abXmit, cb);
        Verify(sendto(s, (char *)abXmit, cb, 0, (struct sockaddr *)&sin, sizeof(sin)) == cb);
        Verify(recv(s, (char *)abRecv, sizeof(abRecv), 0) == cb);
        Verify(memcmp(abXmit, abRecv, cb) == 0);
        if ((cb % 200) == 0) printf(".");
    }

    for (; cb > 0; --cb)
    {
        XNetRandom(abXmit, cb);
        Verify(sendto(s, (char *)abXmit, cb, 0, (struct sockaddr *)&sin, sizeof(sin)) == cb);
        Verify(recv(s, (char *)abRecv, sizeof(abRecv), 0) == cb);
        Verify(memcmp(abXmit, abRecv, cb) == 0);
        if ((cb % 200) == 0) printf(".");
    }

    Verify(sendto(s, (char *)abXmit, 0, 0, (struct sockaddr *)&sin, sizeof(sin)) == 0);
    Verify(recv(s, (char *)abRecv, sizeof(abRecv), 0) == 0);
}

void CTestXNet::SockUdpReflect(SOCKET s)
{
    BYTE abRecv[1500];

    sockaddr_in sin;
    int cb;

    while (1)
    {
        int slen = sizeof(sin);
        cb = recvfrom(s, (char *)abRecv, sizeof(abRecv), 0, (struct sockaddr *)&sin, &slen);
        Verify(cb != SOCKET_ERROR);
        Verify(sendto(s, (char *)abRecv, cb, 0, (struct sockaddr *)&sin, sizeof(sin)) == cb);
        if (cb == 0)
            break;
    }
}

SOCKET CTestXNet::SockTcpCreate(CIpPort ipport)
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Verify(s != SOCKET_ERROR);

    if (ipport)
    {
        sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = 0;
        sin.sin_port = ipport;
        Verify(bind(s, (struct sockaddr *)&sin, sizeof(sin)) == 0);
    }

    return(s);
}

void CTestXNet::SockTcpTransmit(SOCKET s, UINT cbMax)
{
    BYTE abXmit[1500], abRecv[1500];

    if (cbMax > sizeof(abXmit))
        cbMax = sizeof(abXmit);

    int cb;

    for (cb = 1; cb < (int)cbMax; ++cb)
    {
        XNetRandom(abXmit, cb);
        Verify(send(s, (char *)abXmit, cb, 0) == cb);
        Verify(recv(s, (char *)abRecv, sizeof(abRecv), 0) == cb);
        Verify(memcmp(abXmit, abRecv, cb) == 0);
        if ((cb % 200) == 0) printf(".");
    }

    for (; cb > 0; --cb)
    {
        XNetRandom(abXmit, cb);
        Verify(send(s, (char *)abXmit, cb, 0) == cb);
        Verify(recv(s, (char *)abRecv, sizeof(abRecv), 0) == cb);
        Verify(memcmp(abXmit, abRecv, cb) == 0);
        if ((cb % 200) == 0) printf(".");
    }

    Verify(shutdown(s, SD_SEND) != SOCKET_ERROR);
}

void CTestXNet::SockTcpReflect(SOCKET s)
{
    BYTE abRecv[1500];
    int cb;

    while (1)
    {
        cb = recv(s, (char *)abRecv, sizeof(abRecv), 0);
        Verify(cb != SOCKET_ERROR);

        if (cb == 0)
        {
            Verify(shutdown(s, SD_SEND) != SOCKET_ERROR);
            break;
        }

        Verify(send(s, (char *)abRecv, cb, 0) == cb);
    }
}

void CTestXNet::DiscoveryHost(UINT cClient, XNKID * pxnkid)
{
    SOCKET s = SockUdpCreate(NTOHS(1234), TRUE);

    CDiscReq dreq;
    CDiscRsp drsp;
    UINT iClient = 0;
    sockaddr_in sin;
    BYTE * pbNonce = (BYTE *)LocalAlloc(LPTR, sizeof(dreq.abNonce) * cClient);
    Verify(pbNonce != NULL);
    XNADDR xnaddr;
    XNKID xnkid;
    XNKEY xnkey;

    Verify(XNetCreateKey(&drsp.xnkid, &drsp.xnkey) == 0);
    Verify(XNetRegisterKey(&drsp.xnkid, &drsp.xnkey) == 0);
    Verify(XNetGetTitleXnAddr(&drsp.xnaddr) != 0);

    XNetRandom(drsp.abNonce, sizeof(drsp.abNonce));

    while (iClient < cClient)
    {
        Verify(recv(s, (char *)&dreq, sizeof(dreq), 0) == sizeof(dreq));

        for (UINT i = 0; i < iClient; ++i)
        {
            if (memcmp(&pbNonce[i * sizeof(dreq.abNonce)], dreq.abNonce, sizeof(dreq.abNonce)) == 0)
                break;
        }

        if (i == iClient)
        {
            memcpy(&pbNonce[i * sizeof(dreq.abNonce)], dreq.abNonce, sizeof(dreq.abNonce));
            iClient += 1;
        }

        memcpy(drsp.abNonce, dreq.abNonce, sizeof(dreq.abNonce));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_BROADCAST;
        sin.sin_port = NTOHS(1234);
        Verify(sendto(s, (char *)&drsp, sizeof(drsp), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(drsp));
    }

    Verify(closesocket(s) == 0);
    LocalFree(pbNonce);
    *pxnkid = drsp.xnkid;
}

void CTestXNet::DiscoveryClient(XNKID * pxnkid, CIpAddr * pipa)
{
    SOCKET s = SockUdpCreate(NTOHS(1234));

    char cBroadcast = 1;
    sockaddr_in sin;
    CDiscReq dreq;
    CDiscRsp drsp;

    XNetRandom(dreq.abNonce, sizeof(dreq.abNonce));

    Verify(setsockopt(s, SOL_SOCKET, SO_BROADCAST, &cBroadcast, 1) == 0);

    while (1)
    {
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_BROADCAST;
        sin.sin_port = NTOHS(1234);
        Verify(sendto(s, (char *)&dreq, sizeof(dreq), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(dreq));

        fd_set  fds;
        TIMEVAL tv;
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        tv.tv_sec  = 0;
        tv.tv_usec = 200000;

        INT iResult = select(0, &fds, NULL, NULL, &tv);
        Verify(iResult != SOCKET_ERROR);

        while (1)
        {
            ULONG ul = 0;
            iResult = ioctlsocket(s, FIONREAD, &ul);
            Verify(iResult != SOCKET_ERROR);

            if (ul == 0)
                break;

            Verify(recv(s, (char *)&drsp, sizeof(drsp), 0) == sizeof(drsp));
            if (memcmp(drsp.abNonce, dreq.abNonce, sizeof(drsp.abNonce)) == 0)
                goto done;
        }
    }

done:

    Verify(XNetRegisterKey(&drsp.xnkid, &drsp.xnkey) == 0);
    Verify(XNetXnAddrToInAddr(&drsp.xnaddr, &drsp.xnkid, (IN_ADDR *)pipa) == 0);
    Verify(closesocket(s) == 0);
    *pxnkid = drsp.xnkid;
}

void CTestXNet::SgTcpXbToXbClient()
{
    while (XNetGetTitleXnAddr(&g_xnaddrXb1) == XNET_GET_XNADDR_PENDING)
        Sleep(100);

    IN_ADDR inaVip, inaSg, inaXb2;
    HANDLE hEventLogon;

    inaVip.s_addr = inet_addr("10.0.0.2");
    Verify(XNetServerToInAddr(inaVip, 0, &inaSg) == 0);
    Verify((hEventLogon = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);
    GetXn()->IpLogon(inaSg.s_addr, NULL, hEventLogon);
    Verify(WaitForSingleObject(hEventLogon, INFINITE) == WAIT_OBJECT_0);
    Verify(GetXn()->IpLogonGetStatus(NULL) == XN_LOGON_STATE_ONLINE);

    Verify(WaitForSingleObject(g_hEventXb1ToXb2, INFINITE) == WAIT_OBJECT_0);

    Verify(XNetRegisterKey(&g_xnkidXb1ToXb2, &g_xnkeyXb1ToXb2) == 0);
    Verify(XNetXnAddrToInAddr(&g_xnaddrXb2, &g_xnkidXb1ToXb2, &inaXb2) == 0);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Verify(s != SOCKET_ERROR);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inaXb2.s_addr;
    sin.sin_port = HTONS(80);
    DWORD dwTick = GetTickCount();
    INT err = connect(s, (struct sockaddr *)&sin, sizeof(sin));
    dwTick = GetTickCount() - dwTick;
    g_cSecsXb1ToXb2Connect = (dwTick + 500) / 1000;

    if (err == SOCKET_ERROR)
    {
        g_fXb1ToXb2Timeout = TRUE;
    }

    SetEvent(g_hEventXb2ToXb1);

    if (!g_fXb1ToXb2Timeout)
    {
        SockTcpTransmit(s, 10);
    }

    Verify(closesocket(s) == 0);

    // don't blow away the SA before the TCP connection has a chance to close gracefully
    Sleep(100);
    
    Verify(XNetUnregisterKey(&g_xnkidXb1ToXb2) == 0);

    GetXn()->IpLogoff();
    Verify(CloseHandle(hEventLogon));
}

void CTestXNet::SgTcpXbToXbServer()
{
    while (XNetGetTitleXnAddr(&g_xnaddrXb2) == XNET_GET_XNADDR_PENDING)
        Sleep(100);

    IN_ADDR inaVip, inaSg, inaXb2;
    HANDLE hEventLogon;

    inaVip.s_addr = inet_addr("10.0.0.2");
    Verify(XNetServerToInAddr(inaVip, 0, &inaSg) == 0);
    Verify((hEventLogon = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);
    GetXn()->IpLogon(inaSg.s_addr, NULL, hEventLogon);
    Verify(WaitForSingleObject(hEventLogon, INFINITE) == WAIT_OBJECT_0);
    Verify(GetXn()->IpLogonGetStatus(NULL) == XN_LOGON_STATE_ONLINE);

    Verify(XNetCreateKey(&g_xnkidXb1ToXb2, &g_xnkeyXb1ToXb2) == 0);
    g_xnkidXb1ToXb2.ab[0] &= ~XNET_XNKID_MASK;
    g_xnkidXb1ToXb2.ab[0] |=  XNET_XNKID_ONLINE_PEER;
    Verify(XNetRegisterKey(&g_xnkidXb1ToXb2, &g_xnkeyXb1ToXb2) == 0);
    DWORD dw = XNetGetTitleXnAddr(&g_xnaddrXb2);
    Verify(dw != XNET_GET_XNADDR_PENDING);
    Verify(dw & XNET_GET_XNADDR_ONLINE);

    SOCKET s1 = SockTcpCreate(HTONS(80));
    Verify(listen(s1, 1) == 0);

    SetEvent(g_hEventXb1ToXb2);
    Verify(WaitForSingleObject(g_hEventXb2ToXb1, INFINITE) == WAIT_OBJECT_0);

    if (!g_fXb1ToXb2Timeout)
    {
        sockaddr_in sin;
        int slen = sizeof(sin);
        SOCKET s2 = accept(s1, (struct sockaddr *)&sin, &slen);
        Verify(s2 != SOCKET_ERROR);
        Verify(slen == sizeof(sin));
        SockTcpReflect(s2);
        Verify(closesocket(s2) == 0);
    }

    Verify(closesocket(s1) == 0);
    Verify(XNetUnregisterKey(&g_xnkidXb1ToXb2) == 0);

    GetXn()->IpLogoff();
    Verify(CloseHandle(hEventLogon));
}

#define XNETTEST_BEG(_name, pszXbox, pszIp, pszMask, pszGateway, dwFlags) \
    class CTest##_name : public CTestXNet { public: \
    CTest##_name() : CTestXNet(pszXbox) {}; \
    virtual void Init() { Startup(pszIp, pszMask, pszGateway, dwFlags); } \
    virtual void Test()
#define XNETTEST_END(_name) \
    }; void Test_##_name(HANDLE hEvent1 = 0, HANDLE hEvent2 = 0) { CTest##_name * p = new CTest##_name; p->Go(hEvent1, hEvent2); delete p; }

// ---------------------------------------------------------------------------------------
// TWOTHREADTEST
// ---------------------------------------------------------------------------------------

typedef void (*PFNTEST)(HANDLE hEvent1, HANDLE hEvent2);

typedef struct {
    HANDLE      hThread;
    HANDLE      hEvent1;
    HANDLE      hEvent2;
    PFNTEST     pfn;
} TWOTHREADPARAM;

DWORD WINAPI TwoThreadProc(void * pv)
{
    TWOTHREADPARAM * p = (TWOTHREADPARAM *)pv;
    p->pfn(p->hEvent1, p->hEvent2);
    return(0);
}

#define TWOTHREADTEST(_name) \
    void Test_##_name##_run() \
    { \
        g_fXb1ToXb2Timeout = FALSE; \
        Verify((g_hEventXb1ToXb2 = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL); \
        Verify((g_hEventXb2ToXb1 = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL); \
        HANDLE  hThread1 = NULL, hEvent1 = NULL, hThread2 = NULL, hEvent2 = NULL; \
        TWOTHREADPARAM p1, p2; \
        Verify((p1.hEvent1 = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL); \
        Verify((p1.hEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL); \
        p1.pfn = &Test_##_name##_1; \
        p2.hEvent1 = p1.hEvent2; \
        p2.hEvent2 = p1.hEvent1; \
        p2.pfn = &Test_##_name##_2; \
        Verify((p1.hThread = CreateThread(NULL, 0, TwoThreadProc, &p1, 0, NULL)) != NULL); \
        Verify((p2.hThread = CreateThread(NULL, 0, TwoThreadProc, &p2, 0, NULL)) != NULL); \
        Verify(WaitForSingleObject(p1.hThread, INFINITE) == WAIT_OBJECT_0); \
        Verify(WaitForSingleObject(p2.hThread, INFINITE) == WAIT_OBJECT_0); \
        CloseHandle(p1.hEvent1); CloseHandle(p2.hEvent1); \
        CloseHandle(p1.hThread); CloseHandle(p2.hThread); \
        CloseHandle(g_hEventXb1ToXb2); g_hEventXb1ToXb2 = NULL; \
        CloseHandle(g_hEventXb2ToXb1); g_hEventXb2ToXb1 = NULL; \
    } \
    void Test_##_name() \

#define TWOTHREADRUN(_name) Test_##_name##_run()

// Test_inet_addr ------------------------------------------------------------------------

XNETTEST_BEG(inet_addr, "xb1@Lan", "10.0.0.1", "255.0.0.0", NULL, 0)
{
    printf("Testing inet_addr ");
    Verify(inet_addr("004.003.002.020") == 0x10020304);
    Verify(inet_addr("0377.0377.0377.0377") == 0xffffffff);
    Verify(inet_addr("004.003.002.0400") == 0x00020304);
    Verify(inet_addr("004.003.0400.020") == 0x10000304);
    Verify(inet_addr("004.0400.002.020") == 0x10020004);
    Verify(inet_addr("0400.003.002.020") == 0x10020300);
    Verify(inet_addr("0x4.0x3.0x2.0x10") == 0x10020304);
    Verify(inet_addr("0xFF.0xFF.0xFF.0xFF") == 0xffffffff);
    Verify(inet_addr("0x4.0x3.0x2.0x100") == 0x00020304);
    Verify(inet_addr("0x4.0x3.0x100.0x10") == 0x10000304);
    Verify(inet_addr("0x4.0x100.0x2.0x10") == 0x10020004);
    Verify(inet_addr("0x100.0x3.0x2.0x10") == 0x10020300);
    Verify(inet_addr("4.003.2.0x10") == 0x10020304);
    Verify(inet_addr("004.3.0x2.16") == 0x10020304);
    Verify(inet_addr("4.0x3.2.020") == 0x10020304);
    Verify(inet_addr("0x4.3.002.16") == 0x10020304);
    Verify(inet_addr("255.0377.255.0xFF") == 0xffffffff);
    Verify(inet_addr("0377.255.0xFF.255") == 0xffffffff);
    Verify(inet_addr("255.0xFF.255.0377") == 0xffffffff);
    Verify(inet_addr("0xFF.255.0377.255") == 0xffffffff);
    Verify(inet_addr("4.0400.2.0x10") == 0x10020004);
    Verify(inet_addr("004.3.0x100.16") == 0x10000304);
    Verify(inet_addr("256.0x3.2.020") == 0x10020300);
    Verify(inet_addr("0x4.3.002.256") == 0x00020304);
    Verify(inet_addr("4.3.2") == 0x02000304);
    Verify(inet_addr("255.255.255") == 0xff00ffff);
    Verify(inet_addr("4.3.256") == 0x00010304);
    Verify(inet_addr("4.3.65536") == 0x00000304);
    Verify(inet_addr("4.3") == 0x03000004);
    Verify(inet_addr("255.255") == 0xff0000ff);
    Verify(inet_addr("4.256") == 0x00010004);
    Verify(inet_addr("4.16777216") == 0x00000004);
    Verify(inet_addr("4") == 0x04000000);
    Verify(inet_addr("255") == 0xff000000);
    Verify(inet_addr("4294967296") == 0x00000000);
    Verify(inet_addr("4.3.2.16abcdef") == 0xffffffff);
    Verify(inet_addr("4.3.2abcdef.16") == 0xffffffff);
    Verify(inet_addr("4.3abcdef.2.16") == 0xffffffff);
    Verify(inet_addr("4abcdef.3.2.16") == 0xffffffff);
    Verify(inet_addr("4.3.2.abcdef16") == 0xffffffff);
    Verify(inet_addr("4.3.abcdef2.16") == 0xffffffff);
    Verify(inet_addr("4.abcdef3.2.16") == 0xffffffff);
    Verify(inet_addr("abcdef4.3.2.16") == 0xffffffff);
    Verify(inet_addr("4.3.2 .16") == 0x10020304);
    Verify(inet_addr("4.3 .2.16") == 0x10020304);
    Verify(inet_addr("4 .3.2.16") == 0x10020304);
    Verify(inet_addr("4.3.2. 16") == 0x10020304);
    Verify(inet_addr("4.3. 2.16") == 0x10020304);
    Verify(inet_addr("4. 3.2.16") == 0x10020304);
    Verify(inet_addr(" 4.3.2.16") == 0x10020304);
    printf(" [OK]\n");
}
XNETTEST_END(inet_addr);

// Test_XNetRandom -----------------------------------------------------------------------

XNETTEST_BEG(XNetRandom, "xb1@Lan", "10.0.0.1", "255.0.0.0", NULL, 0)
{
    printf("Testing XNetRandom ");

    DWORD dw1, dw2;
    Verify(XNetRandom((BYTE *)&dw1, sizeof(dw1)) == 0);
    Verify(XNetRandom((BYTE *)&dw2, sizeof(dw2)) == 0);
    Verify(dw1 != dw2);

    BYTE ab[13];
    UINT c = 200;
    for (; c > 0; --c)
    {
        Verify(XNetRandom(ab, sizeof(ab)) == 0);
    }

    printf("[OK]\n");
}
XNETTEST_END(XNetRandom);

// Test_XNetRegisterKey ------------------------------------------------------------------

XNETTEST_BEG(XNetRegisterKey, "xb1@Lan", "10.0.0.1", "255.0.0.0", NULL, 0)
{
    printf("Testing XNetRegisterKey ");

    XNKID xnkid1, xnkid2, xnkid3, xnkid4, xnkid5;
    XNKEY xnkey1, xnkey2, xnkey3, xnkey4, xnkey5;

    Verify(XNetCreateKey(&xnkid1, &xnkey1) == 0);
    Verify(XNetCreateKey(&xnkid2, &xnkey2) == 0);
    Verify(XNetCreateKey(&xnkid3, &xnkey3) == 0);
    Verify(XNetCreateKey(&xnkid4, &xnkey4) == 0);
    Verify(XNetCreateKey(&xnkid5, &xnkey5) == 0);

    Verify(XNetRegisterKey(&xnkid1, &xnkey1) == 0);
    Verify(XNetRegisterKey(&xnkid1, &xnkey1) == WSAEALREADY);
    Verify(XNetRegisterKey(&xnkid1, &xnkey2) == WSAEALREADY);
    Verify(XNetRegisterKey(&xnkid2, &xnkey2) == 0);
    Verify(XNetRegisterKey(&xnkid2, &xnkey1) == WSAEALREADY);
    Verify(XNetRegisterKey(&xnkid2, &xnkey2) == WSAEALREADY);
    Verify(XNetUnregisterKey(&xnkid1) == 0);
    Verify(XNetUnregisterKey(&xnkid1) == WSAEINVAL);
    Verify(XNetRegisterKey(&xnkid1, &xnkey1) == 0);
    Verify(XNetRegisterKey(&xnkid1, &xnkey1) == WSAEALREADY);
    Verify(XNetUnregisterKey(&xnkid2) == 0);
    Verify(XNetUnregisterKey(&xnkid2) == WSAEINVAL);
    Verify(XNetUnregisterKey(&xnkid1) == 0);
    Verify(XNetUnregisterKey(&xnkid1) == WSAEINVAL);
    Verify(XNetRegisterKey(&xnkid1, &xnkey1) == 0);
    Verify(XNetRegisterKey(&xnkid2, &xnkey2) == 0);
    Verify(XNetRegisterKey(&xnkid3, &xnkey3) == 0);
    Verify(XNetRegisterKey(&xnkid4, &xnkey4) == 0);
    Verify(XNetRegisterKey(&xnkid5, &xnkey5) == WSAENOMORE);
    Verify(XNetUnregisterKey(&xnkid3) == 0);
    Verify(XNetRegisterKey(&xnkid5, &xnkey5) == 0);
    Verify(XNetUnregisterKey(&xnkid3) == WSAEINVAL);
    Verify(XNetUnregisterKey(&xnkid2) == 0);
    Verify(XNetUnregisterKey(&xnkid4) == 0);
    Verify(XNetUnregisterKey(&xnkid1) == 0);
    Verify(XNetUnregisterKey(&xnkid5) == 0);

    printf("[OK]\n");
}
XNETTEST_END(XNetRegisterKey);

// Test_XNetXnAddrToInAddr ---------------------------------------------------------------

XNETTEST_BEG(XNetXnAddrToInAddr, "xb1@Lan", "10.0.0.1", "255.0.0.0", NULL, 0)
{
    printf("Testing XNetXnAddrToInAddr ");

    XNKID   xnkid1,     xnkid2,     xnkid3,     xnkidT,     xnkidZ;
    XNKEY   xnkey1,     xnkey2,     xnkey3,     xnkeyT;
    XNADDR  xnaddr1,    xnaddr2,    xnaddr3,    xnaddrT,    xnaddrZ;
    CIpAddr ipa1,       ipa2,       ipa3,       ipaT;

    Verify(XNetCreateKey(&xnkid1, &xnkey1) == 0);
    Verify(XNetCreateKey(&xnkid2, &xnkey2) == 0);
    Verify(XNetCreateKey(&xnkid3, &xnkey3) == 0);

    Verify(XNetRegisterKey(&xnkid1, &xnkey1) == 0);
    Verify(XNetRegisterKey(&xnkid2, &xnkey2) == 0);
    Verify(XNetRegisterKey(&xnkid3, &xnkey3) == 0);
    
    Verify(XNetGetTitleXnAddr(&xnaddr1) != 0);
    Verify(XNetXnAddrToInAddr(&xnaddr1, &xnkid1, (IN_ADDR *)&ipa1) == 0);
    Verify(ipa1 == IPADDR_LOOPBACK);

    memset(&xnkidZ, 0, sizeof(xnkidZ));
    Verify(XNetInAddrToXnAddr(*(IN_ADDR *)&ipa1, &xnaddrT, &xnkidT) == 0);
    Verify(memcmp(&xnaddr1, &xnaddrT, sizeof(XNADDR)) == 0);
    Verify(memcmp(&xnkidT, &xnkidZ, sizeof(xnkidZ)) == 0);

    XNADDR  rgxna[32];
    CIpAddr rgipa[32];
    int     i;

    for (i = 0; i < dimensionof(rgxna); ++i)
    {
        XNADDR * pxnaddr = &rgxna[i];
        memset(pxnaddr, 0, sizeof(XNADDR));
        Verify(XNetRandom(pxnaddr->abEnet, sizeof(pxnaddr->abEnet)) == 0);
        pxnaddr->ina.s_addr = 0;
        XNKID * pxnkid = ((i % 3) == 0 ? &xnkid1 : ((i % 3) == 1) ? &xnkid2 : &xnkid3);
        Verify(XNetXnAddrToInAddr(pxnaddr, pxnkid, (IN_ADDR *)&rgipa[i]) == 0);
    }

    memset(&xnaddrT, 0, sizeof(xnaddrT));
    Verify(XNetRandom(xnaddrT.abEnet, sizeof(xnaddrT.abEnet)) == 0);
    Verify(XNetXnAddrToInAddr(&xnaddrT, &xnkid1, (IN_ADDR *)&ipaT) == WSAENOMORE);

    for (i = 0; i < dimensionof(rgxna); ++i)
    {
        XNADDR * pxnaddr = &rgxna[i];
        XNKID * pxnkid = ((i % 3) == 0 ? &xnkid1 : ((i % 3) == 1) ? &xnkid2 : &xnkid3);
        Verify(XNetInAddrToXnAddr(*(IN_ADDR *)&rgipa[i], &xnaddrT, &xnkidT) == 0);
        Verify(memcmp(&xnaddrT, &rgxna[i], sizeof(XNADDR)) == 0);
        Verify(memcmp(&xnkidT, pxnkid, sizeof(XNKID)) == 0);
    }

    Verify(XNetUnregisterKey(&xnkid2) == 0);

    for (i = 0; i < dimensionof(rgxna); ++i)
    {
        XNKID * pxnkid = ((i % 3) == 0 ? &xnkid1 : ((i % 3) == 1) ? &xnkid2 : &xnkid3);
        if ((i % 3) == 1)
        {
            memset(&xnaddrZ, 0, sizeof(XNADDR));
            memcpy(&xnaddrT, &rgxna[i], sizeof(XNADDR));
            memset(&xnkidZ, 0, sizeof(XNKID));
            memcpy(&xnkidT, pxnkid, sizeof(XNKID));
            Verify(XNetInAddrToXnAddr(*(IN_ADDR *)&rgipa[i], &xnaddrT, &xnkidT) == WSAEINVAL);
            Verify(memcmp(&xnaddrT, &xnaddrZ, sizeof(XNADDR)) == 0);
            Verify(memcmp(&xnkidT, &xnkidZ, sizeof(XNKID)) == 0);
        }
        else
        {
            Verify(XNetInAddrToXnAddr(*(IN_ADDR *)&rgipa[i], &xnaddrT, &xnkidT) == 0);
            Verify(memcmp(&xnaddrT, &rgxna[i], sizeof(XNADDR)) == 0);
            Verify(memcmp(&xnkidT, pxnkid, sizeof(XNKID)) == 0);
        }
    }

    Verify(XNetUnregisterKey(&xnkid1) == 0);
    Verify(XNetUnregisterKey(&xnkid3) == 0);

    printf("[OK]\n");
}
XNETTEST_END(XNetXnAddrToInAddr)

// Test_Broadcast ------------------------------------------------------------------------

IPTEST_BEG(Broadcast_1, "xb1@Lan", "10.0.0.1", "255.0.0.0", NULL, XNET_STARTUP_BYPASS_SECURITY)
{
    Verify((_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);

    _uiFlags = PKTF_TYPE_UDP_ESP|PKTF_CRYPT;

    for (int i = 0; i < 2; ++i)
    {
        Rand(_ab, sizeof(_ab));

        CPacket *   ppkt    = PacketAlloc(0, _uiFlags, sizeof(_ab));
        CUdpHdr *   pUdpHdr = ppkt->GetUdpHdr();
        memcpy(pUdpHdr + 1, _ab, sizeof(_ab));
        pUdpHdr->_ipportDst = HTONS(666);
        pUdpHdr->_ipportSrc = HTONS(666);
        pUdpHdr->_wLen      = HTONS(sizeof(CUdpHdr) + sizeof(_ab));
        IpFillAndXmit(ppkt, IPADDR_BROADCAST, IPPROTOCOL_UDP, NULL);
        Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
        _uiFlags = PKTF_TYPE_UDP;
    }
}

virtual void UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    Verify(ppkt->IsUdp());
    Verify(pIpHdr->_ipaDst.IsBroadcast());
    Verify(!!ppkt->TestFlags(PKTF_TYPE_ESP) == !!(_uiFlags & PKTF_TYPE_ESP));
    Verify(!!ppkt->TestFlags(PKTF_CRYPT) == !!(_uiFlags & PKTF_CRYPT));
    Verify(pUdpHdr->_ipportDst == HTONS(666));
    Verify(pUdpHdr->_ipportSrc == HTONS(666));
    Verify(NTOHS(pUdpHdr->_wLen) == sizeof(CUdpHdr) + sizeof(_ab));
    Verify(memcmp(pUdpHdr + 1, _ab, sizeof(_ab)) == 0);
    SetEvent(_hEvent);
}

UINT _uiFlags;
HANDLE _hEvent;
BYTE _ab[211];

IPTEST_END(Broadcast_1);

IPTEST_BEG(Broadcast_2, "xb2@Lan", "10.0.0.2", "255.0.0.0", NULL, XNET_STARTUP_BYPASS_SECURITY)
{
    Verify((_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);

    for (int i = 0; i < 2; ++i)
    {
        Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
    }
}

virtual void UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    Verify(pIpHdr->_ipaDst.IsBroadcast());
    UINT uiFlags = (ppkt->GetFlags() & (PKTF_TYPE_UDP|PKTF_TYPE_ESP|PKTF_CRYPT));
    CPacket * ppktNew = PacketAlloc(PTAG_CPacket, PKTF_POOLALLOC|uiFlags, cbLen);
    memcpy(ppktNew->GetUdpHdr(), pUdpHdr, sizeof(CUdpHdr) + cbLen);
    IpFillAndXmit(ppktNew, IPADDR_BROADCAST, IPPROTOCOL_UDP, NULL);
    SetEvent(_hEvent);
}

HANDLE _hEvent;

IPTEST_END(Broadcast_2);

TWOTHREADTEST(Broadcast)
{
    printf("Testing Broadcast ");
    TWOTHREADRUN(Broadcast);
    printf("[OK]\n");
}

// Test_Discovery ------------------------------------------------------------------------

#define DISCOVERY_UNICAST       200

IPTEST_BEG(Discovery_1, "xb1@Lan", "10.0.0.1", "255.0.0.0", NULL, 0)
{
    // This is the host of the "game session".  We create a key and register it.

    Verify((_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);
    Verify(IpCreateKey(&_xnkid, &_xnkey) == 0);
    IpGetXnAddr(&_xnaddr);
    Verify(IpRegisterKey(&_xnkid, &_xnkey) == 0);
    _fListening = TRUE;

    // Now we wait for discovery requests and respond with a discovery response.
    // This event will be signalled as soon as we've responded.

    Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
    Verify(_cBroadcast > 0);

    // Now we wait for a stream of unicast UDP packets from the client

    while (_cUnicast < DISCOVERY_UNICAST)
    {
        Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
    }

    Verify(IpUnregisterKey(&_xnkid) == 0);
}

virtual void UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    if (pIpHdr->_ipaDst.IsBroadcast())
    {
        if (!_fListening)
        {
            TraceSz(Warning, "Dropping discovery request that arrived too early");
            return;
        }

        Verify(cbLen == sizeof(CDiscReq));
        Verify(pUdpHdr->_ipportDst == HTONS(666));
        Verify(pUdpHdr->_ipportSrc == HTONS(666));
        CDiscReq *  pDiscReq    = (CDiscReq *)(pUdpHdr + 1);
        CPacket *   ppktNew     = PacketAlloc(PTAG_CPacket, PKTF_POOLALLOC|PKTF_TYPE_UDP_ESP|PKTF_CRYPT, sizeof(CDiscRsp));
        CUdpHdr *   pUdpHdrNew  = ppktNew->GetUdpHdr();
        pUdpHdrNew->_ipportDst  = HTONS(666);
        pUdpHdrNew->_ipportSrc  = HTONS(666);
        pUdpHdrNew->_wLen       = HTONS(sizeof(CUdpHdr) + sizeof(CDiscRsp));
        CDiscRsp *  pDiscRsp    = (CDiscRsp *)(pUdpHdrNew + 1);
        memcpy(pDiscRsp->abNonce, pDiscReq->abNonce, sizeof(pDiscReq->abNonce));
        pDiscRsp->xnkid         = _xnkid;
        pDiscRsp->xnkey         = _xnkey;
        pDiscRsp->xnaddr        = _xnaddr;
        IpFillAndXmit(ppktNew, IPADDR_BROADCAST, IPPROTOCOL_UDP, NULL);
        _cBroadcast += 1;
        SetEvent(_hEvent);
        return;
    }

    CIpAddr ipaSrc = pIpHdr->_ipaSrc;
    XNKID   xnkid;
    XNADDR  xnaddr;
    Verify(IpInAddrToXnAddr(ipaSrc, &xnaddr, &xnkid) == 0);
    Verify(memcmp(&xnkid, &_xnkid, sizeof(XNKID)) == 0);
    Verify(ppkt->GetEnetHdr()->_eaSrc.IsEqual(xnaddr.abEnet));
    UINT uiFlags = ppkt->GetFlags() & (PKTF_TYPE_UDP|PKTF_TYPE_ESP|PKTF_CRYPT);
    CPacket * ppktNew = PacketAlloc(PTAG_CPacket, PKTF_POOLALLOC|uiFlags, cbLen);
    memcpy(ppktNew->GetUdpHdr(), pUdpHdr, sizeof(CUdpHdr) + cbLen);
    IpFillAndXmit(ppktNew, ipaSrc, IPPROTOCOL_UDP, NULL);
    _cUnicast += 1;
    SetEvent(_hEvent);
}

HANDLE _hEvent;
XNKID _xnkid;
XNKEY _xnkey;
XNADDR _xnaddr;
BOOL _fListening;
UINT _cBroadcast;
UINT _cUnicast;

IPTEST_END(Discovery_1);

IPTEST_BEG(Discovery_2, "xb2@Lan", "10.0.0.2", "255.0.0.0", NULL, 0)
{
    Verify((_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);

    // Until we get a response, keep broadcasting a CDiscReq packet

    Rand(_abNonce, sizeof(_abNonce));

    while (_cBroadcast == 0)
    {
        CPacket * ppkt = PacketAlloc(0, PKTF_TYPE_UDP_ESP|PKTF_CRYPT, sizeof(CDiscReq));
        CUdpHdr * pUdpHdr = ppkt->GetUdpHdr();
        CDiscReq * pDiscReq = (CDiscReq *)(pUdpHdr + 1);
        pUdpHdr->_ipportDst  = HTONS(666);
        pUdpHdr->_ipportSrc  = HTONS(666);
        pUdpHdr->_wLen       = HTONS(sizeof(CUdpHdr) + sizeof(CDiscReq));
        memcpy(pDiscReq->abNonce, _abNonce, sizeof(_abNonce));
        IpFillAndXmit(ppkt, IPADDR_BROADCAST, IPPROTOCOL_UDP, NULL);
        WaitForSingleObject(_hEvent, 200);
    }

    // Register the host's key

    Verify(IpRegisterKey(&_xnkid, &_xnkey) == 0);

    // Get a secure IP address for the host

    CIpAddr ipaHost;

    Verify(IpXnAddrToInAddr(&_xnaddr, &_xnkid, &ipaHost) == 0);

    // Send Unicast UDP packets

    Rand(_ab, sizeof(_ab));

    UINT i;

    for (i = 0; i < DISCOVERY_UNICAST; ++i)
    {
        UINT uiFlags = PKTF_TYPE_UDP_ESP|PKTF_CRYPT;
        CPacket * ppkt = PacketAlloc(0, uiFlags, sizeof(_ab));
        CUdpHdr * pUdpHdr = ppkt->GetUdpHdr();
        pUdpHdr->_ipportDst  = HTONS(666);
        pUdpHdr->_ipportSrc  = HTONS(666);
        pUdpHdr->_wLen       = HTONS(sizeof(CUdpHdr) + sizeof(_ab));
        memcpy(pUdpHdr + 1, _ab, sizeof(_ab));
        IpFillAndXmit(ppkt, ipaHost, IPPROTOCOL_UDP, NULL);

        // Don't get too far ahead of the other side

        while (_cUnicast + 6 < i)
        {
            Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
        }
    }

    // Wait for the remaining packets to be reflected

    while (_cUnicast < i)
    {
        Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
    }

    Verify(IpUnregisterKey(&_xnkid) == 0);
}

virtual void UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    if (pIpHdr->_ipaDst.IsBroadcast())
    {
        Verify(cbLen == sizeof(CDiscRsp));
        Verify(pUdpHdr->_ipportDst == HTONS(666));
        Verify(pUdpHdr->_ipportSrc == HTONS(666));
        CDiscRsp * pDiscRsp = (CDiscRsp *)(pUdpHdr + 1);
        Verify(memcmp(pDiscRsp->abNonce, _abNonce, sizeof(_abNonce)) == 0);
        _xnkid = pDiscRsp->xnkid;
        _xnkey = pDiscRsp->xnkey;
        _xnaddr = pDiscRsp->xnaddr;
        _cBroadcast += 1;
        SetEvent(_hEvent);
        return;
    }

    Verify(pUdpHdr->_ipportDst == HTONS(666));
    Verify(pUdpHdr->_ipportSrc == HTONS(666));
    Verify(cbLen == sizeof(_ab));
    Verify(memcmp(pUdpHdr + 1, _ab, sizeof(_ab)) == 0);
    _cUnicast += 1;
    SetEvent(_hEvent);
}

HANDLE _hEvent;
XNKID _xnkid;
XNKEY _xnkey;
XNADDR _xnaddr;
BYTE _abNonce[8];
BYTE _ab[592];
UINT _cBroadcast;
UINT _cUnicast;

IPTEST_END(Discovery_2);

TWOTHREADTEST(Discovery)
{
    printf("Testing Discovery ");
    TWOTHREADRUN(Discovery);
    printf("[OK]\n");
}

// Test_Frag -----------------------------------------------------------------------------

#define FRAGALG_FORWARD     0
#define FRAGALG_REVERSE     1
#define FRAGALG_RANDOM      2
#define FRAGALG_OVERLAP     3
#define FRAGALG_COUNT       4

DefineTag(fragTest, 0);

IPTEST_BEG(Frag, "xb1@Lan", "10.0.0.1", "255.0.0.0", NULL, 0)
{
    Verify((_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);

    for (int iAlg = 0; iAlg < FRAGALG_COUNT; ++iAlg)
    {
        printf("Testing Fragment Reassembler (%s) ",
               iAlg == FRAGALG_FORWARD ? "forward" :
               iAlg == FRAGALG_REVERSE ? "reverse" :
               iAlg == FRAGALG_RANDOM ? "random" : "overlap");

        if (iAlg == FRAGALG_RANDOM)
        {
            printf(" ");
        }

        for (int cbHdrOpt = 0; cbHdrOpt <= (MAXIPHDRLEN-sizeof(CIpHdr)); cbHdrOpt += 4)
        {
            printf(".");

            for (int cbData = 1; cbData < (int)(ENET_DATA_MAXSIZE - sizeof(CIpHdr) - cbHdrOpt - sizeof(CUdpHdr)); )
            {
                int cbChunk = 8;

                while (1)
                {
                    TestFrag(cbHdrOpt, cbData, iAlg, cbChunk);

                    if (cbChunk < 64)
                        cbChunk += 8;
                    else
                        cbChunk += 32;

                    if (cbChunk > (int)((cbData + sizeof(CUdpHdr)) / 2))
                        break;
                }

                if (cbData < 63)
                    cbData += 1;
                else if (cbData < 253)
                    cbData += 51;
                else
                    cbData += 127;
            }
        }

        printf(" [OK]\n");
    }

    CloseHandle(_hEvent);
}

void TestFrag(int cbHdrOpt, int cbData, int iAlg, int cbChunk)
{
    CIpHdr *        pIpHdr;
    CUdpHdr *       pUdpHdr;
    CPseudoHeader   ph;
    UINT            uiChecksum;
    int             iChunk;
    int             cChunk;

    TraceSz4(fragTest, "cbHdrOpt=%d,cbData=%d,iAlg=%d,cbChunk=%d", cbHdrOpt, cbData, iAlg, cbChunk);

    ResetEvent(_hEvent);

    // Create the original packet

    _ppktOrig = PacketAlloc(0, PKTF_TYPE_UDP, cbHdrOpt + cbData);
    Verify(_ppktOrig != NULL);

    pIpHdr              = _ppktOrig->GetIpHdr();
    pIpHdr->SetHdrLen(sizeof(CIpHdr) + cbHdrOpt);
    pIpHdr->_bTos       = 0;
    pIpHdr->_wLen       = HTONS(sizeof(CIpHdr) + cbHdrOpt + sizeof(CUdpHdr) + cbData);
    pIpHdr->_wId        = HTONS(GetNextDgramId());
    pIpHdr->_wFragOff   = 0;
    pIpHdr->_bTtl       = 64;
    pIpHdr->_bProtocol  = IPPROTOCOL_UDP;
    pIpHdr->_ipaSrc     = IPADDR_LOOPBACK;
    pIpHdr->_ipaDst     = IPADDR_LOOPBACK;
    Rand((BYTE *)(pIpHdr + 1), cbHdrOpt);
    pIpHdr->_wChecksum  = 0;
    pIpHdr->_wChecksum  = ~tcpipxsum(0, pIpHdr, sizeof(CIpHdr) + cbHdrOpt);

    _ppktOrig->SetHdrOptLen(cbHdrOpt);

    pUdpHdr             = _ppktOrig->GetUdpHdr();
    pUdpHdr->_ipportSrc = HTONS(666);
    pUdpHdr->_ipportDst = HTONS(666);
    pUdpHdr->_wLen      = HTONS(sizeof(CUdpHdr) + cbData);
    pUdpHdr->_wChecksum = 0;
    Rand((BYTE *)(pUdpHdr + 1), cbData);
    ph._ipaSrc          = pIpHdr->_ipaSrc;
    ph._ipaDst          = pIpHdr->_ipaDst;
    ph._bZero           = 0;
    ph._bProtocol       = IPPROTOCOL_UDP;
    ph._wLen            = HTONS(sizeof(CUdpHdr) + cbData);
    pUdpHdr->_wChecksum = 0;
    uiChecksum          = ~tcpipxsum(tcpipxsum(0, &ph, sizeof(ph)), pUdpHdr, sizeof(CUdpHdr) + cbData);
    pUdpHdr->_wChecksum = uiChecksum - (uiChecksum == 0);

    TraceUdpHdr(fragTest, 0, pUdpHdr, pUdpHdr->GetLen() - sizeof(CUdpHdr));
    TraceSz9(fragTest, "[IP %s %s (%d %d %04X %04X) %d]{%d}[%d]",
             pIpHdr->_ipaDst.Str(), pIpHdr->_ipaSrc.Str(), pIpHdr->_bTos, pIpHdr->_bTtl,
             NTOHS(pIpHdr->_wId), NTOHS(pIpHdr->_wFragOff), pIpHdr->_bProtocol,
             pIpHdr->GetOptLen(), _ppktOrig->GetCb() - pIpHdr->GetHdrLen());

    // Create and send packet fragments

    cbData += sizeof(CUdpHdr);
    cChunk  = (cbData + cbChunk - 1) / cbChunk;

    if (iAlg == FRAGALG_FORWARD)
    {
        for (iChunk = 0; iChunk < cChunk; ++iChunk)
        {
            SendChunk(iChunk, 1, cbChunk, iChunk == (cChunk - 1));
        }
    }
    else if (iAlg == FRAGALG_REVERSE)
    {
        for (iChunk = cChunk - 1; iChunk >= 0; --iChunk)
        {
            SendChunk(iChunk, 1, cbChunk, iChunk == (cChunk - 1));
        }
    }
    else if (iAlg == FRAGALG_RANDOM || iAlg == FRAGALG_OVERLAP)
    {
        BYTE abMap[ENET_DATA_MAXSIZE / 8];
        int cChunkSent = 0;

        memset(abMap, 0, sizeof(abMap));

        while (cChunkSent < cChunk)
        {
            // Randomly select a chunk to send.  If that chunk is already sent,
            // look for the next one that isn't sent yet.

            int iChunkRand = rand() % cChunk;

            while (abMap[iChunkRand])
            {
                iChunkRand += 1;
                if (iChunkRand == cChunk)
                    iChunkRand = 0;
            }

            if (iAlg == FRAGALG_OVERLAP)
            {
                int iChunkBeg = iChunkRand;
                int iChunkEnd = iChunkRand;

                if (iChunkBeg > 0)
                {
                    // Back up a random number of chunks (no more than 4)

                    iChunkBeg -= rand() % (min(4, iChunkBeg));
                    Verify(iChunkBeg >= 0);
                }

                if (iChunkEnd < (cChunk - 1))
                {
                    // Extend the range a random number of chunks (no more than 4)

                    iChunkEnd += rand() % (cChunk - iChunkEnd);
                    Verify(iChunkEnd < cChunk);
                }

                if (iChunkBeg == 0 && iChunkEnd == (cChunk - 1))
                {
                    // Whoops.  Can't send the whole thing in one chunk.

                    if (iChunkBeg != iChunkRand)
                        iChunkBeg += 1;
                    else
                    {
                        Verify(iChunkEnd != iChunkRand);
                        iChunkEnd -= 1;
                    }
                }

                Verify(iChunkBeg >= 0);
                Verify(iChunkEnd < cChunk);

                for (iChunkRand = iChunkBeg; iChunkRand <= iChunkEnd; ++iChunkRand)
                {
                    if (abMap[iChunkRand] == 0)
                    {
                        abMap[iChunkRand] = 1;
                        cChunkSent += 1;
                        Verify(cChunkSent <= cChunk);
                    }
                }

                SendChunk(iChunkBeg, iChunkEnd - iChunkBeg + 1, cbChunk, iChunkEnd == (cChunk - 1));
            }
            else
            {
                abMap[iChunkRand] = 1;
                cChunkSent += 1;
                Verify(cChunkSent <= cChunk);
                SendChunk(iChunkRand, 1, cbChunk, iChunkRand == (cChunk - 1));
            }
        }
    }

    // Wait for the packet to arrive

    Verify(WaitForSingleObject(_hEvent, 1000) == WAIT_OBJECT_0);

    PacketFree(_ppktOrig);
    _ppktOrig = NULL;
}

void SendChunk(int iChunk, int cChunk, int cbChunk, BOOL fLastChunk)
{
    CIpHdr *    pIpHdrOrig   = _ppktOrig->GetIpHdr();
    int         cbHdrOptOrig = pIpHdrOrig->GetOptLen();
    int         cbDataOrig   = _ppktOrig->GetCb() - sizeof(CIpHdr) - cbHdrOptOrig;
    int         cbHdrOpt     = iChunk == 0 ? cbHdrOptOrig : 0;
    int         ibData       = iChunk * cbChunk;
    int         cbData       = cChunk * cbChunk;

    Verify(ibData < cbDataOrig);

    if (ibData + cbData >= cbDataOrig)
    {
        cbData = cbDataOrig - ibData;
        Verify(fLastChunk);
    }

    Verify(cbData > 0);
    Verify(!fLastChunk || iChunk > 0);

    CPacket *   ppkt    = PacketAlloc(0, PKTF_TYPE_IP, cbHdrOpt + cbData);
    Verify(ppkt != NULL);

    CIpHdr *    pIpHdr  = ppkt->GetIpHdr();
    memcpy(pIpHdr, pIpHdrOrig, sizeof(CIpHdr) + cbHdrOpt);
    pIpHdr->SetHdrLen(sizeof(CIpHdr) + cbHdrOpt);
    pIpHdr->_wLen       = NTOHS((WORD)(sizeof(CIpHdr) + cbHdrOpt + cbData));
    pIpHdr->_wFragOff   = NTOHS((WORD)((iChunk * (cbChunk / 8)) | (fLastChunk ? 0 : MORE_FRAGMENTS)));
    pIpHdr->_wChecksum  = 0;
    pIpHdr->_wChecksum  = ~tcpipxsum(0, pIpHdr, pIpHdr->GetHdrLen());
    memcpy((BYTE *)(pIpHdr + 1) + cbHdrOpt, (BYTE *)(pIpHdrOrig + 1) + cbHdrOptOrig + ibData, cbData);

    RaiseToDpc();
    IpXmit(ppkt);
}

void UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen)
{
    Verify(ppkt->GetCb() == _ppktOrig->GetCb());
    Verify(memcmp(ppkt->GetPv(), _ppktOrig->GetPv(), ppkt->GetCb()) == 0);
    SetEvent(_hEvent);
}

CPacket *   _ppktOrig;
HANDLE      _hEvent;

IPTEST_END(Frag);

// Test_SockUdp --------------------------------------------------------------------------

XNETTEST_BEG(SockUdp_1, "xb1@Lan", "10.0.0.1", "255.0.0.0", NULL, 0)
{
    SOCKET s = SockUdpCreate(NTOHS(1567));
    XNKID xnkid;
    DiscoveryHost(1, &xnkid);
    SockUdpReflect(s);
    Verify(closesocket(s) == 0);
    Verify(XNetUnregisterKey(&xnkid) == 0);
}
XNETTEST_END(SockUdp_1);

XNETTEST_BEG(SockUdp_2, "xb2@Lan", "10.0.0.2", "255.0.0.0", NULL, 0)
{
    SOCKET s = SockUdpCreate(CIpPort(NTOHS(6543)));
    XNKID xnkid;
    CIpAddr ipa;
    DiscoveryClient(&xnkid, &ipa);
    SockUdpTransmit(s, ipa, CIpPort(NTOHS(1567)));
    Verify(closesocket(s) == 0);
    Verify(XNetUnregisterKey(&xnkid) == 0);
}
XNETTEST_END(SockUdp_2);

TWOTHREADTEST(SockUdp)
{
    printf("Testing SockUdp ");
    TWOTHREADRUN(SockUdp);
    printf(" [OK]\n");
}

// Test_SockTcp --------------------------------------------------------------------------

XNETTEST_BEG(SockTcp_1, "xb1@Lan", "10.0.0.1", "255.0.0.0", NULL, 0)
{
    SOCKET s1 = SockTcpCreate(NTOHS(1567));
    Verify(listen(s1, 1) == 0);
    XNKID xnkid;
    DiscoveryHost(1, &_xnkid);
    sockaddr_in sin;
    int slen = sizeof(sin);
    SOCKET s2 = accept(s1, (struct sockaddr *)&sin, &slen);
    Verify(s2 != SOCKET_ERROR);
    Verify(slen == sizeof(sin));
    SockTcpReflect(s2);
    Verify(closesocket(s2) == 0);
    Verify(closesocket(s1) == 0);
}

virtual void Term()
{
    Verify(XNetUnregisterKey(&_xnkid) == 0);
    CTestXNet::Term();
}

XNKID _xnkid;

XNETTEST_END(SockTcp_1);

XNETTEST_BEG(SockTcp_2, "xb2@Lan", "10.0.0.2", "255.0.0.0", NULL, 0)
{
    SOCKET s = SockTcpCreate();
    XNKID xnkid;
    CIpAddr ipa;
    DiscoveryClient(&_xnkid, &ipa);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ipa;
    sin.sin_port = NTOHS(1567);
    Verify(connect(s, (struct sockaddr *)&sin, sizeof(sin)) != SOCKET_ERROR);
    SockTcpTransmit(s);
    Verify(closesocket(s) == 0);
}

virtual void Term()
{
    Verify(XNetUnregisterKey(&_xnkid) == 0);
    CTestXNet::Term();
}

XNKID _xnkid;

XNETTEST_END(SockTcp_2);

TWOTHREADTEST(SockTcp)
{
    printf("Testing SockTcp ");
    TWOTHREADRUN(SockTcp);
    printf(" [OK]\n");
}

// Test_Dhcp -----------------------------------------------------------------------------

#include <pshpack1.h>

struct DhcpMessage
{
    CEnetHdr        _enethdr;           // [ENET]
    CIpHdr          _iphdr;             // [IP]
    CUdpHdr         _udphdr;            // [UDP]
    BYTE            _op;                // message type
    BYTE            _htype;             // hareware address type
    BYTE            _hlen;              // hardware address length
    BYTE            _hops;              // relay hops
    DWORD           _xid;               // transaction ID
    WORD            _secs;              // seconds since address acquisition process began
    WORD            _flags;             // flags
    CIpAddr         _ciaddr;            // client IP address
    CIpAddr         _yiaddr;            // "your" (client) IP address
    CIpAddr         _siaddr;            // server IP address
    CIpAddr         _giaddr;            // relay agent IP address
    BYTE            _chaddr[16];        // client hardware address
    BYTE            _sname[64];         // optional server hostname
    BYTE            _file[128];         // boot filename
    BYTE            _options[256];      // optional parameters (variable length)
};

#include <poppack.h>

BYTE abDhcpResp[] =
{
    0x00,0x50,0xF2,0x00,0x17,0xFC,0x00,0x90,0x27,0xCC,0x74,0x7C,0x08,0x00,0x45,0x10,
    0x01,0x4E,0x00,0x00,0x00,0x00,0x10,0x11,0x9F,0x47,0x0A,0x47,0x00,0x02,0xFF,0xFF,
    0xFF,0xFF,0x00,0x43,0x00,0x44,0x01,0x3A,0xB4,0xF7,0x02,0x01,0x06,0x00,0x39,0x73,
    0xDD,0x20,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x0A,0x47,0xFE,0x63,0x0A,0x47,
    0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x50,0xF2,0x00,0x17,0xFC,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x63,0x82,0x53,0x63,0x35,0x01,0x02,0x36,0x04,0x0A,
    0x47,0x00,0x02,0x33,0x04,0x00,0x00,0x46,0x50,0x01,0x04,0xFF,0xFF,0x00,0x00,0x03,
    0x04,0x0A,0x47,0x00,0x01,0x06,0x0C,0x0A,0x47,0x00,0x02,0x0A,0x49,0x00,0x02,0xD1,
    0xF9,0xE0,0x82,0x0F,0x16,0x73,0x61,0x6E,0x6A,0x6F,0x73,0x65,0x2E,0x69,0x6E,0x66,
    0x6F,0x67,0x72,0x61,0x6D,0x65,0x73,0x2E,0x6C,0x6F,0x63,0xFF
};

BYTE abDhcpAck[] =
{
    0x00,0x50,0xF2,0x00,0x17,0xFC,0x00,0x90,0x27,0xCC,0x74,0x7C,0x08,0x00,0x45,0x10,
    0x01,0x4E,0x00,0x00,0x00,0x00,0x10,0x11,0x9F,0x47,0x0A,0x47,0x00,0x02,0xFF,0xFF,
    0xFF,0xFF,0x00,0x43,0x00,0x44,0x01,0x3A,0xB1,0xF6,0x02,0x01,0x06,0x00,0x39,0x73,
    0xDD,0x21,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x0A,0x47,0xFE,0x63,0x0A,0x47,
    0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x50,0xF2,0x00,0x17,0xFC,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x63,0x82,0x53,0x63,0x35,0x01,0x05,0x36,0x04,0x0A,
    0x47,0x00,0x02,0x33,0x04,0x00,0x00,0x46,0x50,0x01,0x04,0xFF,0xFF,0x00,0x00,0x03,
    0x04,0x0A,0x47,0x00,0x01,0x06,0x0C,0x0A,0x47,0x00,0x02,0x0A,0x49,0x00,0x02,0xD1,
    0xF9,0xE0,0x82,0x0F,0x16,0x73,0x61,0x6E,0x6A,0x6F,0x73,0x65,0x2E,0x69,0x6E,0x66,
    0x6F,0x67,0x72,0x61,0x6D,0x65,0x73,0x2E,0x6C,0x6F,0x63,0xFF
};

IPTEST_BEG(Dhcp, "xb1@Lan", NULL, NULL, NULL, 0)
{
    printf("Testing Dhcp ");
    XNADDR xnaddr;
    while (IpGetXnAddr(&xnaddr) == XNET_GET_XNADDR_PENDING)
        Sleep(100);
	Verify(xnaddr.ina.s_addr == inet_addr("10.71.254.99"));
    printf("[OK]\n");
}

virtual void EnetXmit(CPacket * ppkt, CIpAddr ipaNext)
{
    DhcpMessage * pdhcpMsg = (DhcpMessage *)ppkt->GetEnetHdr();
    DhcpMessage   dhcpMsg;

    BYTE * pb = abDhcpResp;
    UINT   cb = sizeof(abDhcpResp);

    if (_fGotDiscover)
    {
        pb = abDhcpAck;
        cb = sizeof(abDhcpAck);
    }

    Assert(sizeof(dhcpMsg) >= cb);
    memcpy(&dhcpMsg, pb, cb);

    memcpy(dhcpMsg._chaddr, _ea._ab, sizeof(_ea._ab));
    dhcpMsg._xid = pdhcpMsg->_xid;
    dhcpMsg._enethdr._eaDst = _ea;

    CPseudoHeader ph;
    ph._ipaSrc      = dhcpMsg._iphdr._ipaSrc;
    ph._ipaDst      = dhcpMsg._iphdr._ipaDst;
    ph._bZero       = 0;
    ph._bProtocol   = IPPROTOCOL_UDP;
    ph._wLen        = dhcpMsg._udphdr._wLen;
    dhcpMsg._udphdr._wChecksum = 0;
    UINT uiChecksum = ~tcpipxsum(tcpipxsum(0, &ph, sizeof(ph)), &dhcpMsg._udphdr, NTOHS(ph._wLen));
    dhcpMsg._udphdr._wChecksum = uiChecksum - (uiChecksum == 0);

    NicRecvFrame(&dhcpMsg, cb);

    CXnEnet::EnetXmit(ppkt, 0);

    _fGotDiscover = TRUE;
}

BOOL _fGotDiscover;

IPTEST_END(Dhcp)

// Test_Dns ------------------------------------------------------------------------------

XNETTEST_BEG(Dns, "xb1@Nic/0:FEFFDEADF00D", NULL, NULL, NULL, 0)
{
    printf("Testing Dns ");

    if (!VLanDriver())
    {
        printf("[No VLan]\n");
        return;
    }

    XNADDR xnaddr;
    while (XNetGetTitleXnAddr(&xnaddr) == XNET_GET_XNADDR_PENDING)
        Sleep(100);
    Verify((_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);
    
    XNDNS * pxndns = NULL;

    Verify(XNetDnsLookup("nosuchhost.foo.bar.com", _hEvent, &pxndns) == 0);
    Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
    Verify(pxndns->iStatus == WSAHOST_NOT_FOUND);
    Verify(pxndns->cina == 0);
    Verify(XNetDnsRelease(pxndns) == 0);

    Verify(XNetDnsLookup("products.redmond.corp.microsoft.com", _hEvent, &pxndns) == 0);
    Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
    Verify(pxndns->iStatus == 0);
    Verify(pxndns->cina > 0);
    Verify(XNetDnsRelease(pxndns) == 0);

    Verify(XNetDnsLookup("products.redmond.corp.microsoft.com", NULL, &pxndns) == 0);
    while (pxndns->iStatus == WSAEINPROGRESS)
        Sleep(100);
    Verify(pxndns->iStatus == 0);
    Verify(pxndns->cina > 0);
    Verify(XNetDnsRelease(pxndns) == 0);

    CloseHandle(_hEvent);
    printf("[OK]\n");
}

HANDLE _hEvent;

XNETTEST_END(Dns)

// Test_SgUdp ----------------------------------------------------------------------------

XNETTEST_BEG(SgUdp_1, "xb1@Internet", "10.0.0.10", "255.0.0.0", "10.0.0.1", 0)
{
    GetXn()->IpLogon(0, NULL, NULL);
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = inet_addr("10.0.0.2");
    Verify(XNetServerToInAddr(inaVip, 0, &inaSec) == 0);
    SOCKET s = SockUdpCreate(0, FALSE);
    Verify(s != SOCKET_ERROR);
    SockUdpTransmit(s, inaSec.s_addr, CIpPort(NTOHS(1)));
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgUdp_1)

XNETTEST_BEG(SgUdp_2, "xb2@Datacenter", "10.0.0.4", "255.0.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s = SockUdpCreate(HTONS(127));
    SockUdpReflect(s);
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgUdp_2)

TWOTHREADTEST(SgUdp)
{
    printf("Testing SgUdp ");
    TWOTHREADRUN(SgUdp);
    printf(" [OK]\n");
}

// Test_SgTcp ----------------------------------------------------------------------------

XNETTEST_BEG(SgTcp_1, "xb1@Internet", "10.0.0.11", "255.0.0.0", "10.0.0.1", 0)
{
    GetXn()->IpLogon(0, NULL, NULL);
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = inet_addr("10.0.0.2");
    Verify(XNetServerToInAddr(inaVip, 0, &inaSec) == 0);
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Verify(s != SOCKET_ERROR);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = inaSec;
    sin.sin_port = HTONS(2);
    Verify(connect(s, (struct sockaddr *)&sin, sizeof(sin)) != SOCKET_ERROR);
    SockTcpTransmit(s);
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgTcp_1)

XNETTEST_BEG(SgTcp_2, "xb2@Datacenter", "10.0.0.5", "255.0.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s1 = SockTcpCreate(HTONS(80));
    Verify(listen(s1, 1) == 0);
    sockaddr_in sin;
    int slen = sizeof(sin);
    SOCKET s2 = accept(s1, (struct sockaddr *)&sin, &slen);
    Verify(s2 != SOCKET_ERROR);
    Verify(slen == sizeof(sin));
    SockTcpReflect(s2);
    Verify(closesocket(s2) == 0);
    Verify(closesocket(s1) == 0);
}
XNETTEST_END(SgTcp_2)

TWOTHREADTEST(SgTcp)
{
    printf("Testing SgTcp ");
    TWOTHREADRUN(SgTcp);
    printf(" [OK]\n");
}

// Test_SgRst ----------------------------------------------------------------------------

XNETTEST_BEG(SgRst_1, "xb1@Internet", "10.0.0.11", "255.0.0.0", "10.0.0.1", 0)
{
    GetXn()->IpLogon(0, NULL, NULL);
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = inet_addr("10.0.0.2");
    Verify(XNetServerToInAddr(inaVip, 0, &inaSec) == 0);
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Verify(s != SOCKET_ERROR);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = inaSec;
    sin.sin_port = HTONS(2);
    Verify(connect(s, (struct sockaddr *)&sin, sizeof(sin)) != SOCKET_ERROR);
    Verify(recv(s, (char *)&s, 1, 0) == 1);
    Verify(XNetUnregisterInAddr(inaSec) == 0);
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgRst_1)

XNETTEST_BEG(SgRst_2, "xb2@Datacenter", "10.0.0.5", "255.0.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s1 = SockTcpCreate(HTONS(80));
    Verify(listen(s1, 1) == 0);
    sockaddr_in sin;
    int slen = sizeof(sin);
    SOCKET s2 = accept(s1, (struct sockaddr *)&sin, &slen);
    Verify(s2 != SOCKET_ERROR);
    Verify(slen == sizeof(sin));

    while (1)
    {
        INT err = send(s2, (char *)&s2, 1, 0);

        if (err == SOCKET_ERROR)
        {
            Verify(WSAGetLastError() == WSAECONNRESET);
            break;
        }

        Sleep(100);
    }

    Verify(closesocket(s2) == 0);
    Verify(closesocket(s1) == 0);
}
XNETTEST_END(SgRst_2)

TWOTHREADTEST(SgRst)
{
    printf("Testing SgRst ");
    TWOTHREADRUN(SgRst);
    printf("[OK]\n");
}

// Test_SgAlive --------------------------------------------------------------------------

XNETTEST_BEG(SgAlive_1, "xb1@Internet", "10.0.0.10", "255.0.0.0", "10.0.0.1", 0)
{
    GetXn()->IpLogon(0, NULL, NULL);
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = inet_addr("10.0.0.2");
    Verify(XNetServerToInAddr(inaVip, 0, &inaSec) == 0);
    SOCKET s = SockUdpCreate(0, FALSE);
    Verify(s != SOCKET_ERROR);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = inaSec;
    sin.sin_port = NTOHS(3);
    Verify(sendto(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(inaVip));

    // Sleep for a while to allow keepalives to flow
    Sleep(60 * 1000);

    sin.sin_family = AF_INET;
    sin.sin_addr = inaSec;
    sin.sin_port = NTOHS(3);
    Verify(sendto(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(inaVip));
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgAlive_1)

XNETTEST_BEG(SgAlive_2, "xb2@Datacenter", "10.0.0.6", "255.0.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s = SockUdpCreate(HTONS(123));

    IN_ADDR inaVip;
    sockaddr_in sin;
    int slen = sizeof(sin);
    
    Verify(recvfrom(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, &slen) == sizeof(inaVip));
    Verify(inaVip.s_addr == inet_addr("10.0.0.2"));

    CSgMsgAuthReq   sgar;

    sgar._wType   = SGMSG_TYPE_AUTHDATA_REQ;
    sgar._cbEnt   = sizeof(CSgMsgAuthReq);
    XNetRandom((BYTE *)&sgar._dwReqNum, sizeof(sgar._dwReqNum));
    sgar._ipaZ    = sin.sin_addr;
    sgar._ipportZ = sin.sin_port;
    
    sin.sin_port = NTOHS(0xFFFF);
    Verify(sendto(s, (char *)&sgar, sizeof(sgar), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(sgar));

    BYTE            ab[CBSGAUTHREPMSG];
    CSgMsgAuthRep * psgap     = (CSgMsgAuthRep *)ab;
    CSgMsgHdr *     psgmh     = (CSgMsgHdr *)(psgap + 1);
    CAuthData *     pAuthData = (CAuthData *)(psgmh + 1);

    slen = sizeof(sin);
    Verify(recvfrom(s, (char *)ab, sizeof(ab), 0, (struct sockaddr *)&sin, &slen) == sizeof(ab));
    Verify(psgap->_wType == SGMSG_TYPE_AUTHDATA_REP);
    Verify(psgap->_cbEnt == sizeof(CSgMsgAuthRep));
    Verify(psgap->_dwReqNum == sgar._dwReqNum);
    Verify(psgap->_ipaZ.s_addr == sgar._ipaZ.s_addr);
    Verify(psgap->_ipaI.s_addr == inet_addr("10.0.0.10"));
    Verify(psgap->_ipportZ == sgar._ipportZ);
    Verify(psgap->_fNotFound == FALSE);
    Verify(psgmh->_wType == SGMSG_TYPE_AUTHDATA);
    Verify(psgmh->_cbEnt == sizeof(CSgMsgHdr) + sizeof(CAuthData));
    Verify(pAuthData->GetCb() == sizeof(CAuthData));

    slen = sizeof(sin);
    Verify(recvfrom(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, &slen) == sizeof(inaVip));
    Verify(inaVip.s_addr == inet_addr("10.0.0.2"));

    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgAlive_2)

TWOTHREADTEST(SgAlive)
{
    printf("Testing SgAlive ");
    TWOTHREADRUN(SgAlive);
    printf("[OK]\n");
}

// Test_SgMsg ----------------------------------------------------------------------------

XNETTEST_BEG(SgMsg_1, "xb1@Internet", "10.0.0.10", "255.0.0.0", "10.0.0.1", 0)
{
    GetXn()->IpLogon(0, NULL, NULL);
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = inet_addr("10.0.0.2");
    Verify(XNetServerToInAddr(inaVip, 0, &inaSec) == 0);
    SOCKET s = SockUdpCreate(0, FALSE);
    Verify(s != SOCKET_ERROR);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = inaSec;
    sin.sin_port = NTOHS(3);
    Verify(sendto(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(inaVip));
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgMsg_1)

XNETTEST_BEG(SgMsg_2, "xb2@Datacenter", "10.0.0.6", "255.0.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s = SockUdpCreate(HTONS(123));

    IN_ADDR inaVip;
    sockaddr_in sin;
    int slen = sizeof(sin);
    
    Verify(recvfrom(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, &slen) == sizeof(inaVip));
    Verify(inaVip.s_addr == inet_addr("10.0.0.2"));

    CSgMsgAuthReq   sgar;

    sgar._wType   = SGMSG_TYPE_AUTHDATA_REQ;
    sgar._cbEnt   = sizeof(CSgMsgAuthReq);
    XNetRandom((BYTE *)&sgar._dwReqNum, sizeof(sgar._dwReqNum));
    sgar._ipaZ    = sin.sin_addr;
    sgar._ipportZ = sin.sin_port;
    
    sin.sin_port = NTOHS(0xFFFF);
    Verify(sendto(s, (char *)&sgar, sizeof(sgar), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(sgar));

    BYTE            ab[CBSGAUTHREPMSG];
    CSgMsgAuthRep * psgap     = (CSgMsgAuthRep *)ab;
    CSgMsgHdr *     psgmh     = (CSgMsgHdr *)(psgap + 1);
    CAuthData *     pAuthData = (CAuthData *)(psgmh + 1);

    slen = sizeof(sin);
    Verify(recvfrom(s, (char *)ab, sizeof(ab), 0, (struct sockaddr *)&sin, &slen) == sizeof(ab));
    Verify(psgap->_wType == SGMSG_TYPE_AUTHDATA_REP);
    Verify(psgap->_cbEnt == sizeof(CSgMsgAuthRep));
    Verify(psgap->_dwReqNum == sgar._dwReqNum);
    Verify(psgap->_ipaZ.s_addr == sgar._ipaZ.s_addr);
    Verify(psgap->_ipaI.s_addr == inet_addr("10.0.0.10"));
    Verify(psgap->_ipportZ == sgar._ipportZ);
    Verify(psgap->_fNotFound == FALSE);
    Verify(psgmh->_wType == SGMSG_TYPE_AUTHDATA);
    Verify(psgmh->_cbEnt == sizeof(CSgMsgHdr) + sizeof(CAuthData));
    Verify(pAuthData->GetCb() == sizeof(CAuthData));
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgMsg_2)

TWOTHREADTEST(SgMsg)
{
    printf("Testing SgMsg ");
    TWOTHREADRUN(SgMsg);
    printf("[OK]\n");
}

// Test_SgLogon --------------------------------------------------------------------------

XNETTEST_BEG(SgLogon_1, "xb1@Internet", "10.0.0.10", "255.0.0.0", "10.0.0.1", 0)
{
    IN_ADDR inaVip, inaSec;
    HANDLE hEventLogon = NULL;
    ULONGLONG aqwUserId[4];
    XNKID axnkid[4];
    BYTE abData[32];
    UINT iUser;

    // Let the other thread start up so it doesn't miss our first UDP packet
    Sleep(500);

    XNetRandom((BYTE *)aqwUserId, sizeof(aqwUserId));
    XNetRandom((BYTE *)axnkid, sizeof(axnkid));
    XNetRandom(abData, sizeof(abData));

    inaVip.s_addr = inet_addr("10.0.0.2");
    Verify(XNetServerToInAddr(inaVip, 0, &inaSec) == 0);
    Verify((hEventLogon = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);
    GetXn()->IpLogon(inaSec.s_addr, aqwUserId, hEventLogon);
    Verify(WaitForSingleObject(hEventLogon, INFINITE) == WAIT_OBJECT_0);
    Verify(GetXn()->IpLogonGetStatus(NULL) == XN_LOGON_STATE_ONLINE);

    XNADDR xnaddr;
    Verify(XNetGetTitleXnAddr(&xnaddr) != XNET_GET_XNADDR_PENDING);

    SOCKET s = SockUdpCreate(0, FALSE);
    Verify(s != SOCKET_ERROR);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = inaSec;
    sin.sin_port = NTOHS(3);
    Verify(sendto(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(inaVip));

    printf(".");

    for (iUser = 0; iUser < 4; ++iUser)
    {
        Verify(GetXn()->IpLogonSetPState(0, ~iUser, &axnkid[0], iUser ? (iUser + 1) * 2 : 0, &abData[iUser * 8]));
        printf(".");
    }

    // Sleep for a while to allow keepalives to flow
    for (int i = 0; i < 5; i++)
    {
        Sleep(4 * 1000);
        printf(".");
    }

    sin.sin_family = AF_INET;
    sin.sin_addr = inaSec;
    sin.sin_port = NTOHS(3);
    Verify(sendto(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(inaVip));
    Verify(closesocket(s) == 0);
    printf(".");

    GetXn()->IpLogoff();
    printf(".");

    CloseHandle(hEventLogon);
}
XNETTEST_END(SgLogon_1)

XNETTEST_BEG(SgLogon_2, "xb2@Datacenter", "10.0.0.6", "255.0.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s = SockUdpCreate(HTONS(123));

    IN_ADDR inaVip;
    sockaddr_in sin;
    int slen = sizeof(sin);
    
    Verify(recvfrom(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, &slen) == sizeof(inaVip));
    Verify(inaVip.s_addr == inet_addr("10.0.0.2"));
    printf(".");

    CSgMsgAuthReq   sgar;

    sgar._wType   = SGMSG_TYPE_AUTHDATA_REQ;
    sgar._cbEnt   = sizeof(CSgMsgAuthReq);
    XNetRandom((BYTE *)&sgar._dwReqNum, sizeof(sgar._dwReqNum));
    sgar._ipaZ    = sin.sin_addr;
    sgar._ipportZ = sin.sin_port;
    
    sin.sin_port = NTOHS(0xFFFF);
    Verify(sendto(s, (char *)&sgar, sizeof(sgar), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(sgar));

    BYTE            ab[CBSGAUTHREPMSG];
    CSgMsgAuthRep * psgap     = (CSgMsgAuthRep *)ab;
    CSgMsgHdr *     psgmh     = (CSgMsgHdr *)(psgap + 1);
    CAuthData *     pAuthData = (CAuthData *)(psgmh + 1);

    slen = sizeof(sin);
    Verify(recvfrom(s, (char *)ab, sizeof(ab), 0, (struct sockaddr *)&sin, &slen) == sizeof(ab));
    Verify(psgap->_wType == SGMSG_TYPE_AUTHDATA_REP);
    Verify(psgap->_cbEnt == sizeof(CSgMsgAuthRep));
    Verify(psgap->_dwReqNum == sgar._dwReqNum);
    Verify(psgap->_ipaZ.s_addr == sgar._ipaZ.s_addr);
    Verify(psgap->_ipaI.s_addr == inet_addr("10.0.0.10"));
    Verify(psgap->_ipportZ == sgar._ipportZ);
    Verify(psgap->_fNotFound == FALSE);
    Verify(psgmh->_wType == SGMSG_TYPE_AUTHDATA);
    Verify(psgmh->_cbEnt == sizeof(CSgMsgHdr) + sizeof(CAuthData));
    Verify(pAuthData->GetCb() == sizeof(CAuthData));

    slen = sizeof(sin);
    Verify(recvfrom(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, &slen) == sizeof(inaVip));
    Verify(inaVip.s_addr == inet_addr("10.0.0.2"));

    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgLogon_2)

TWOTHREADTEST(SgLogon)
{
    printf("Testing SgLogon ");
    TWOTHREADRUN(SgLogon);
    printf(" [OK]\n");
}

// Test_XnSetEventTimer ------------------------------------------------------------------

IPTEST_BEG(XnSetEventTimer, "xb1@Internet", "10.0.0.1", "255.255.0.0", NULL, 0)
{
    printf("Testing XnSetEventTimer ");
    
    CEventTimer et;
    HANDLE hEvent;

    memset(&et, 0, sizeof(et));
    Verify((hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);
    DWORD dwTickBeg = GetTickCount();
    IpSetEventTimer(&et, hEvent, 1000);
    IpSetEventTimer(&et, hEvent, 0xFFFFFFFF);
    CloseHandle(hEvent);
    hEvent = NULL;

    for (DWORD dwTimeout = 50; dwTimeout < 1000; dwTimeout += 160)
    {
        for (int i = 0; i < 5; ++i)
        {
            memset(&et, 0, sizeof(et));
            Verify((hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);
            DWORD dwTickBeg = GetTickCount();
            IpSetEventTimer(&et, hEvent, dwTimeout);
            Verify(WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0);
            CloseHandle(hEvent);
            hEvent = NULL;
            DWORD dwTickEnd = GetTickCount();
            Verify(dwTickEnd - dwTickBeg > dwTimeout);
            printf(".");
            Sleep(17 * i);
        }
    }

    printf(" [OK]\n");
}
IPTEST_END(XnSetEventTimer);

// Test_NatDhcp --------------------------------------------------------------------------

XNETTEST_BEG(NatDhcp_1, "xb1@Nat1Inner", NULL, NULL, NULL, 0)
{
    XNADDR xnaddr;
    DWORD dw;

    while (1)
    {
        dw = XNetGetTitleXnAddr(&xnaddr);
        if (dw != XNET_GET_XNADDR_PENDING)
            break;
        Sleep(100);
    }

    Verify(dw & XNET_GET_XNADDR_DHCP);
}
XNETTEST_END(NatDhcp_1)

void Test_NatDhcp()
{
    for (UINT iAssign = NAT_ASSIGN_MINIMAL; iAssign <= NAT_ASSIGN_AGGRESSIVE; ++iAssign)
    {
        for (UINT iFilter = NAT_FILTER_NONE; iFilter <= NAT_FILTER_ADDRESS_PORT; ++iFilter)
        {
            printf("Testing NatDhcp %s/%s ", iAssign == NAT_ASSIGN_MINIMAL ? "min" : "agr",
                   iFilter == NAT_FILTER_NONE ? "none" : iFilter == NAT_FILTER_ADDRESS ? "addr" :
                   iFilter == NAT_FILTER_PORT ? "port" : "both");

            NATCREATE natcreate = { 0 };

            strcpy(natcreate._achXboxInner, "Nat@Nat1Inner");
            strcpy(natcreate._achXboxOuter, "Nat@Nat1Outer");

            natcreate._xncpInner.ina.s_addr         = inet_addr("10.0.0.1");
            natcreate._xncpInner.inaMask.s_addr     = inet_addr("255.255.0.0");
            natcreate._xncpOuter.ina.s_addr         = inet_addr("10.0.0.1");
            natcreate._xncpOuter.inaMask.s_addr     = inet_addr("255.255.0.0");
            natcreate._inaBase.s_addr               = inet_addr("10.0.1.0");
            natcreate._inaLast.s_addr               = inet_addr("10.0.254.254");
            natcreate._dwLeaseTime                  = 60 * 60 * 24;
            natcreate._iAssign                      = iAssign;
            natcreate._iFilter                      = iFilter;

            void * pvNat = XNetNatCreate(&natcreate);
            Verify(pvNat != NULL);

            Test_NatDhcp_1();

            XNetNatDelete(pvNat);

            printf("[OK]\n");
        }
    }
}

// Test_NatDns ---------------------------------------------------------------------------

XNETTEST_BEG(NatDns_1, "xb1@Nat1Inner", NULL, NULL, NULL, 0)
{
    XNADDR xnaddr;
    while (XNetGetTitleXnAddr(&xnaddr) == XNET_GET_XNADDR_PENDING)
        Sleep(100);
    Verify((_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL);
    
    XNDNS * pxndns = NULL;

    Verify(XNetDnsLookup("nosuchhost.foo.bar.com", _hEvent, &pxndns) == 0);
    Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
    Verify(pxndns->iStatus == WSAHOST_NOT_FOUND);
    Verify(pxndns->cina == 0);
    Verify(XNetDnsRelease(pxndns) == 0);

    Verify(XNetDnsLookup("products.redmond.corp.microsoft.com", _hEvent, &pxndns) == 0);
    Verify(WaitForSingleObject(_hEvent, INFINITE) == WAIT_OBJECT_0);
    Verify(pxndns->iStatus == 0);
    Verify(pxndns->cina > 0);
    Verify(XNetDnsRelease(pxndns) == 0);

    Verify(XNetDnsLookup("products.redmond.corp.microsoft.com", NULL, &pxndns) == 0);
    while (pxndns->iStatus == WSAEINPROGRESS)
        Sleep(100);
    Verify(pxndns->iStatus == 0);
    Verify(pxndns->cina > 0);
    Verify(XNetDnsRelease(pxndns) == 0);

    CloseHandle(_hEvent);
}

HANDLE _hEvent;

XNETTEST_END(NatDns_1)

void Test_NatDns()
{
    for (UINT iAssign = NAT_ASSIGN_MINIMAL; iAssign <= NAT_ASSIGN_AGGRESSIVE; ++iAssign)
    {
        for (UINT iFilter = NAT_FILTER_NONE; iFilter <= NAT_FILTER_ADDRESS_PORT; ++iFilter)
        {
            printf("Testing NatDns %s/%s ", iAssign == NAT_ASSIGN_MINIMAL ? "min" : "agr",
                   iFilter == NAT_FILTER_NONE ? "none" : iFilter == NAT_FILTER_ADDRESS ? "addr" :
                   iFilter == NAT_FILTER_PORT ? "port" : "both");

            if (!VLanDriver())
            {
                printf("[No VLan]\n");
                return;
            }

            NATCREATE natcreate = { 0 };

            strcpy(natcreate._achXboxInner, "Nat1@Nat1Inner");
            strcpy(natcreate._achXboxOuter, "Nat1@Nic/0");

            natcreate._xncpInner.ina.s_addr         = inet_addr("10.0.0.1");
            natcreate._xncpInner.inaMask.s_addr     = inet_addr("255.255.0.0");
            natcreate._inaBase.s_addr               = inet_addr("10.0.1.0");
            natcreate._inaLast.s_addr               = inet_addr("10.0.254.254");
            natcreate._dwLeaseTime                  = 60 * 60 * 24;
            natcreate._iAssign                      = iAssign;
            natcreate._iFilter                      = iFilter;

            void * pvNat = XNetNatCreate(&natcreate);
            Verify(pvNat != NULL);

            Test_NatDns_1();

            XNetNatDelete(pvNat);

            printf("[OK]\n");
        }
    }
}

// Test_NatUdp ---------------------------------------------------------------------------

XNETTEST_BEG(NatUdp_1, "xb1@Nat1Inner", NULL, NULL, NULL, XNET_STARTUP_BYPASS_SECURITY)
{
    XNADDR xnaddr;
    while (XNetGetTitleXnAddr(&xnaddr) == XNET_GET_XNADDR_PENDING)
        Sleep(100);

    SOCKET s = SockUdpCreate(CIpPort(NTOHS(6543)));
    SockUdpTransmit(s, inet_addr("192.168.0.2"), CIpPort(NTOHS(1567)));
    Verify(closesocket(s) == 0);
}
XNETTEST_END(NatUdp_1);

XNETTEST_BEG(NatUdp_2, "xb2@Nat1Outer", "192.168.0.2", "255.255.0.0", NULL, XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s = SockUdpCreate(NTOHS(1567));
    SockUdpReflect(s);
    Verify(closesocket(s) == 0);
}
XNETTEST_END(NatUdp_2);

TWOTHREADTEST(NatUdp)
{
    for (UINT iAssign = NAT_ASSIGN_MINIMAL; iAssign <= NAT_ASSIGN_AGGRESSIVE; ++iAssign)
    {
        for (UINT iFilter = NAT_FILTER_NONE; iFilter <= NAT_FILTER_ADDRESS_PORT; ++iFilter)
        {
            printf("Testing NatUdp %s/%s ", iAssign == NAT_ASSIGN_MINIMAL ? "min" : "agr",
                   iFilter == NAT_FILTER_NONE ? "none" : iFilter == NAT_FILTER_ADDRESS ? "addr" :
                   iFilter == NAT_FILTER_PORT ? "port" : "both");

            NATCREATE natcreate = { 0 };

            strcpy(natcreate._achXboxInner, "Nat1@Nat1Inner");
            strcpy(natcreate._achXboxOuter, "Nat1@Nat1Outer");

            natcreate._xncpInner.ina.s_addr         = inet_addr("10.0.0.1");
            natcreate._xncpInner.inaMask.s_addr     = inet_addr("255.255.0.0");
            natcreate._xncpOuter.ina.s_addr         = inet_addr("192.168.0.1");
            natcreate._xncpOuter.inaMask.s_addr     = inet_addr("255.255.0.0");
            natcreate._inaBase.s_addr               = inet_addr("10.0.1.0");
            natcreate._inaLast.s_addr               = inet_addr("10.0.254.254");
            natcreate._dwLeaseTime                  = 60 * 60 * 24;
            natcreate._iAssign                      = iAssign;
            natcreate._iFilter                      = iFilter;

            void * pvNat = XNetNatCreate(&natcreate);
            Verify(pvNat != NULL);

            TWOTHREADRUN(NatUdp);

            XNetNatDelete(pvNat);

            printf(" [OK]\n");
        }
    }
}

// Test_NatTcp ---------------------------------------------------------------------------

XNETTEST_BEG(NatTcp_1, "xb1@Nat1Inner", NULL, NULL, NULL, XNET_STARTUP_BYPASS_SECURITY)
{
    XNADDR xnaddr;
    while (XNetGetTitleXnAddr(&xnaddr) == XNET_GET_XNADDR_PENDING)
        Sleep(100);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Verify(s != SOCKET_ERROR);
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("192.168.0.2");
    sin.sin_port = HTONS(80);
    Verify(connect(s, (struct sockaddr *)&sin, sizeof(sin)) != SOCKET_ERROR);
    SockTcpTransmit(s);
    Verify(closesocket(s) == 0);
}
XNETTEST_END(NatTcp_1)

XNETTEST_BEG(NatTcp_2, "xb2@Nat1Outer", "192.168.0.2", "255.255.0.0", NULL, XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s1 = SockTcpCreate(HTONS(80));
    Verify(listen(s1, 1) == 0);
    sockaddr_in sin;
    int slen = sizeof(sin);
    SOCKET s2 = accept(s1, (struct sockaddr *)&sin, &slen);
    Verify(s2 != SOCKET_ERROR);
    Verify(slen == sizeof(sin));
    SockTcpReflect(s2);
    Verify(closesocket(s2) == 0);
    Verify(closesocket(s1) == 0);
}
XNETTEST_END(NatTcp_2)

TWOTHREADTEST(NatTcp)
{
    for (UINT iAssign = NAT_ASSIGN_MINIMAL; iAssign <= NAT_ASSIGN_AGGRESSIVE; ++iAssign)
    {
        for (UINT iFilter = NAT_FILTER_NONE; iFilter <= NAT_FILTER_ADDRESS_PORT; ++iFilter)
        {
            printf("Testing NatTcp %s/%s ", iAssign == NAT_ASSIGN_MINIMAL ? "min" : "agr",
                   iFilter == NAT_FILTER_NONE ? "none" : iFilter == NAT_FILTER_ADDRESS ? "addr" :
                   iFilter == NAT_FILTER_PORT ? "port" : "both");

            NATCREATE natcreate = { 0 };

            strcpy(natcreate._achXboxInner, "Nat1@Nat1Inner");
            strcpy(natcreate._achXboxOuter, "Nat1@Nat1Outer");

            natcreate._xncpInner.ina.s_addr         = inet_addr("10.0.0.1");
            natcreate._xncpInner.inaMask.s_addr     = inet_addr("255.255.0.0");
            natcreate._xncpOuter.ina.s_addr         = inet_addr("192.168.0.1");
            natcreate._xncpOuter.inaMask.s_addr     = inet_addr("255.255.0.0");
            natcreate._inaBase.s_addr               = inet_addr("10.0.1.0");
            natcreate._inaLast.s_addr               = inet_addr("10.0.254.254");
            natcreate._dwLeaseTime                  = 60 * 60 * 24;
            natcreate._iAssign                      = iAssign;
            natcreate._iFilter                      = iFilter;

            void * pvNat = XNetNatCreate(&natcreate);
            Verify(pvNat != NULL);

            TWOTHREADRUN(NatTcp);

            XNetNatDelete(pvNat);

            printf(" [OK]\n");
        }
    }
}

// Test_SgNat1Xb1ToNat2Xb2 ---------------------------------------------------------------

XNETTEST_BEG(SgXb1ToXb2_1, "xb1@Internet", "10.0.0.10", "255.0.0.0", "10.0.0.1", 0)
{
    SgTcpXbToXbClient();
}
XNETTEST_END(SgXb1ToXb2_1)

XNETTEST_BEG(SgXb1ToXb2_2, "xb2@Internet", "10.0.0.11", "255.0.0.0", "10.0.0.1", 0)
{
    SgTcpXbToXbServer();
}
XNETTEST_END(SgXb1ToXb2_2)

TWOTHREADTEST(SgXb1ToXb2)
{
    TWOTHREADRUN(SgXb1ToXb2);
}

XNETTEST_BEG(SgXb1ToNat2Xb2_1, "xb1@Internet", "10.0.0.10", "255.0.0.0", "10.0.0.1", 0)
{
    SgTcpXbToXbClient();
}
XNETTEST_END(SgXb1ToNat2Xb2_1)

XNETTEST_BEG(SgXb1ToNat2Xb2_2, "xb2@Nat2Inner", NULL, NULL, NULL, 0)
{
    SgTcpXbToXbServer();
}
XNETTEST_END(SgXb1ToNat2Xb2_2)

TWOTHREADTEST(SgXb1ToNat2Xb2)
{
    TWOTHREADRUN(SgXb1ToNat2Xb2);
}

XNETTEST_BEG(SgNat1Xb1ToXb2_1, "xb1@Nat1Inner", NULL, NULL, NULL, 0)
{
    SgTcpXbToXbClient();
}
XNETTEST_END(SgNat1Xb1ToXb2_1)

XNETTEST_BEG(SgNat1Xb1ToXb2_2, "xb2@Internet", "10.0.0.10", "255.0.0.0", NULL, 0)
{
    SgTcpXbToXbServer();
}
XNETTEST_END(SgNat1Xb1ToXb2_2)

TWOTHREADTEST(SgNat1Xb1ToXb2)
{
    TWOTHREADRUN(SgNat1Xb1ToXb2);
}

XNETTEST_BEG(SgNat1Xb1ToNat2Xb2_1, "xb1@Nat1Inner", NULL, NULL, NULL, 0)
{
    SgTcpXbToXbClient();
}
XNETTEST_END(SgNat1Xb1ToNat2Xb2_1)

XNETTEST_BEG(SgNat1Xb1ToNat2Xb2_2, "xb2@Nat2Inner", NULL, NULL, NULL, 0)
{
    SgTcpXbToXbServer();
}
XNETTEST_END(SgNat1Xb1ToNat2Xb2_2)

TWOTHREADTEST(SgNat1Xb1ToNat2Xb2)
{
    TWOTHREADRUN(SgNat1Xb1ToNat2Xb2);
}

XNETTEST_BEG(SgNat1Xb1ToNat1Xb2_1, "xb1@Nat1Inner", NULL, NULL, NULL, 0)
{
    SgTcpXbToXbClient();
}
XNETTEST_END(SgNat1Xb1ToNat1Xb2_1)

XNETTEST_BEG(SgNat1Xb1ToNat1Xb2_2, "xb2@Nat1Inner", NULL, NULL, NULL, 0)
{
    SgTcpXbToXbServer();
}
XNETTEST_END(SgNat1Xb1ToNat1Xb2_2)

TWOTHREADTEST(SgNat1Xb1ToNat1Xb2)
{
    TWOTHREADRUN(SgNat1Xb1ToNat1Xb2);
}

void Test_SgNat()
{
    printf("Testing SgNat\n\n");
    printf("         Direct Min/-- Min/A- Min/-P Min/AP Agr/-- Agr/A- Agr/-P Agr/AP\n");

    UINT    aiAssign[] = { NAT_ASSIGN_MINIMAL, NAT_ASSIGN_MINIMAL, NAT_ASSIGN_MINIMAL, NAT_ASSIGN_MINIMAL, NAT_ASSIGN_AGGRESSIVE, NAT_ASSIGN_AGGRESSIVE, NAT_ASSIGN_AGGRESSIVE, NAT_ASSIGN_AGGRESSIVE };
    UINT    aiFilter[] = { NAT_FILTER_NONE, NAT_FILTER_ADDRESS, NAT_FILTER_PORT, NAT_FILTER_ADDRESS_PORT, NAT_FILTER_NONE, NAT_FILTER_ADDRESS, NAT_FILTER_PORT, NAT_FILTER_ADDRESS_PORT };
    UINT    iAssign1, iAssign2, iFilter1, iFilter2;
    void *  pvNat1, * pvNat2;
    UINT    i, j;

    printf("  OneNat ");
    TWOTHREADRUN(SgXb1ToXb2);
    printf(g_fXb1ToXb2Timeout ? "[F:%2d] " : "%6d ", g_cSecsXb1ToXb2Connect);

    for (i = 0; i < dimensionof(aiAssign); ++i)
    {
        iAssign1 = aiAssign[i];
        iFilter1 = aiFilter[i];

        NATCREATE natcreate1 = { 0 };

        strcpy(natcreate1._achXboxOuter, "Nat1@Internet");
        natcreate1._xncpOuter.ina.s_addr         = inet_addr("10.0.0.253");
        natcreate1._xncpOuter.inaMask.s_addr     = inet_addr("255.255.0.0");

        strcpy(natcreate1._achXboxInner, "Nat1@Nat1Inner");
        natcreate1._xncpInner.ina.s_addr         = inet_addr("192.168.0.1");
        natcreate1._xncpInner.inaMask.s_addr     = inet_addr("255.255.0.0");
        natcreate1._inaBase.s_addr               = inet_addr("192.168.0.2");
        natcreate1._inaLast.s_addr               = inet_addr("192.168.254.254");

        natcreate1._dwLeaseTime                  = 60 * 60 * 24;
        natcreate1._iAssign                      = iAssign1;
        natcreate1._iFilter                      = iFilter1;

        pvNat1 = XNetNatCreate(&natcreate1);
        Verify(pvNat1 != NULL);
        TWOTHREADRUN(SgNat1Xb1ToNat1Xb2);
        XNetNatDelete(pvNat1);
        printf(g_fXb1ToXb2Timeout ? "[F:%2d] " : "%6d ", g_cSecsXb1ToXb2Connect);
    }

    printf("\n");
    printf("  Direct ");

    TWOTHREADRUN(SgXb1ToXb2);
    printf(g_fXb1ToXb2Timeout ? "[F:%2d] " : "%6d ", g_cSecsXb1ToXb2Connect);

    for (i = 0; i < dimensionof(aiAssign); ++i)
    {
        iAssign2 = aiAssign[i];
        iFilter2 = aiFilter[i];

        NATCREATE natcreate2 = { 0 };

        strcpy(natcreate2._achXboxOuter, "Nat2@Internet");
        natcreate2._xncpOuter.ina.s_addr         = inet_addr("10.0.0.254");
        natcreate2._xncpOuter.inaMask.s_addr     = inet_addr("255.255.0.0");

        strcpy(natcreate2._achXboxInner, "Nat2@Nat2Inner");
        natcreate2._xncpInner.ina.s_addr         = inet_addr("192.168.0.1");
        natcreate2._xncpInner.inaMask.s_addr     = inet_addr("255.255.0.0");
        natcreate2._inaBase.s_addr               = inet_addr("192.168.0.2");
        natcreate2._inaLast.s_addr               = inet_addr("192.168.254.254");

        natcreate2._dwLeaseTime                  = 60 * 60 * 24;
        natcreate2._iAssign                      = iAssign2;
        natcreate2._iFilter                      = iFilter2;
        natcreate2._iNatPortBase                 = 2000;

        pvNat2 = XNetNatCreate(&natcreate2);
        Verify(pvNat2 != NULL);
        TWOTHREADRUN(SgXb1ToNat2Xb2);
        XNetNatDelete(pvNat2);
        printf(g_fXb1ToXb2Timeout ? "[F:%2d] " : "%6d ", g_cSecsXb1ToXb2Connect);
    }

    printf("\n");

    for (i = 0; i < dimensionof(aiAssign); ++i)
    {
        iAssign1 = aiAssign[i];
        iFilter1 = aiFilter[i];

        printf("  %s/%c%c ", iAssign1 == NAT_ASSIGN_MINIMAL ? "Min" : "Agr",
               (iFilter1 & NAT_FILTER_ADDRESS) ? 'A' : '-', (iFilter1 & NAT_FILTER_PORT) ? 'P' : '-');

        NATCREATE natcreate1 = { 0 };

        strcpy(natcreate1._achXboxOuter, "Nat1@Internet");
        natcreate1._xncpOuter.ina.s_addr         = inet_addr("10.0.0.253");
        natcreate1._xncpOuter.inaMask.s_addr     = inet_addr("255.255.0.0");

        strcpy(natcreate1._achXboxInner, "Nat1@Nat1Inner");
        natcreate1._xncpInner.ina.s_addr         = inet_addr("192.168.0.1");
        natcreate1._xncpInner.inaMask.s_addr     = inet_addr("255.255.0.0");
        natcreate1._inaBase.s_addr               = inet_addr("192.168.0.2");
        natcreate1._inaLast.s_addr               = inet_addr("192.168.254.254");

        natcreate1._dwLeaseTime                  = 60 * 60 * 24;
        natcreate1._iAssign                      = iAssign1;
        natcreate1._iFilter                      = iFilter1;

        pvNat1 = XNetNatCreate(&natcreate1);
        Verify(pvNat1 != NULL);
        TWOTHREADRUN(SgNat1Xb1ToXb2);
        XNetNatDelete(pvNat1);
        printf(g_fXb1ToXb2Timeout ? "[F:%2d] " : "%6d ", g_cSecsXb1ToXb2Connect);

        for (j = 0; j < dimensionof(aiAssign); ++j)
        {
            iAssign2 = aiAssign[j];
            iFilter2 = aiFilter[j];

            NATCREATE natcreate1 = { 0 };

            strcpy(natcreate1._achXboxOuter, "Nat1@Internet");
            natcreate1._xncpOuter.ina.s_addr         = inet_addr("10.0.0.253");
            natcreate1._xncpOuter.inaMask.s_addr     = inet_addr("255.255.0.0");

            strcpy(natcreate1._achXboxInner, "Nat1@Nat1Inner");
            natcreate1._xncpInner.ina.s_addr         = inet_addr("192.168.0.1");
            natcreate1._xncpInner.inaMask.s_addr     = inet_addr("255.255.0.0");
            natcreate1._inaBase.s_addr               = inet_addr("192.168.0.2");
            natcreate1._inaLast.s_addr               = inet_addr("192.168.254.254");

            natcreate1._dwLeaseTime                  = 60 * 60 * 24;
            natcreate1._iAssign                      = iAssign1;
            natcreate1._iFilter                      = iFilter1;

            pvNat1 = XNetNatCreate(&natcreate1);
            Verify(pvNat1 != NULL);

            NATCREATE natcreate2 = { 0 };

            strcpy(natcreate2._achXboxOuter, "Nat2@Internet");
            natcreate2._xncpOuter.ina.s_addr         = inet_addr("10.0.0.254");
            natcreate2._xncpOuter.inaMask.s_addr     = inet_addr("255.255.0.0");

            strcpy(natcreate2._achXboxInner, "Nat2@Nat2Inner");
            natcreate2._xncpInner.ina.s_addr         = inet_addr("192.168.0.1");
            natcreate2._xncpInner.inaMask.s_addr     = inet_addr("255.255.0.0");
            natcreate2._inaBase.s_addr               = inet_addr("192.168.0.2");
            natcreate2._inaLast.s_addr               = inet_addr("192.168.254.254");

            natcreate2._dwLeaseTime                  = 60 * 60 * 24;
            natcreate2._iAssign                      = iAssign2;
            natcreate2._iFilter                      = iFilter2;
            natcreate2._iNatPortBase                 = 2000;

            pvNat2 = XNetNatCreate(&natcreate2);
            Verify(pvNat2 != NULL);
            TWOTHREADRUN(SgNat1Xb1ToNat2Xb2);
            XNetNatDelete(pvNat2);
            XNetNatDelete(pvNat1);

            printf(g_fXb1ToXb2Timeout ? "[F:%2d] " : "%6d ", g_cSecsXb1ToXb2Connect);
        }
            
        printf("\n");
    }

    printf("\n");
}

// ---------------------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------------------

int     g_argc;
char ** g_argv;

BOOL RunTest(char * pszName, ...)
{
    BOOL fByNameOnly = FALSE;

    va_list va;

    va_start(va, pszName);

    while (pszName)
    {
        if (*pszName == '*')
        {
            fByNameOnly = TRUE;
            pszName += 1;
        }
        else
        {
            fByNameOnly = FALSE;
        }

        if (!fByNameOnly && g_argc == 1)
        {
            return(TRUE);
        }

        for (int i = 1; i < g_argc; ++i)
        {
            if (g_argv[i] && lstrcmpiA(g_argv[i], pszName) == 0)
                return(TRUE);
        }

        pszName = va_arg(va, char *);
    }

    return(FALSE);
}

int __cdecl main(int argc, char * argv[])
{
    g_argc = argc;
    g_argv = argv;

    Verify(VLanInit());

    if (RunTest("inet_addr", NULL))                     Test_inet_addr();
    if (RunTest("XNetRandom", NULL))                    Test_XNetRandom();
    if (RunTest("XNetRegisterKey", NULL))               Test_XNetRegisterKey();
    if (RunTest("XNetXnAddrToInAddr", NULL))            Test_XNetXnAddrToInAddr();
    if (RunTest("Broadcast", NULL))                     Test_Broadcast();
    if (RunTest("Discovery", NULL))                     Test_Discovery();
    if (RunTest("Frag", NULL))                          Test_Frag();
    if (RunTest("XnSetEventTimer", NULL))               Test_XnSetEventTimer();
    if (RunTest("SockUdp", NULL))                       Test_SockUdp();
    if (RunTest("SockTcp", NULL))                       Test_SockTcp();
    if (RunTest("Dhcp", NULL))                          Test_Dhcp();
    if (RunTest("Dns", NULL))                           Test_Dns();
    if (RunTest("NatDhcp", "Nat", NULL))                Test_NatDhcp();
    if (RunTest("NatDns", "Nat", NULL))                 Test_NatDns();
    if (RunTest("NatUdp", "Nat", NULL))                 Test_NatUdp();
    if (RunTest("NatTcp", "Nat", NULL))                 Test_NatTcp();
    if (RunTest("*SgAlive", NULL))                      Test_SgAlive();
    if (RunTest("*SgMsg", "*Sg", NULL))                 Test_SgMsg();
    if (RunTest("*SgUdp", "*Sg", NULL))                 Test_SgUdp();
    if (RunTest("*SgTcp", "*Sg", NULL))                 Test_SgTcp();
    if (RunTest("*SgRst", "*Sg", NULL))                 Test_SgRst();
    if (RunTest("*SgNat", "*Sg", NULL))                 Test_SgNat();
    if (RunTest("*SgLogon", "*Sg", NULL))               Test_SgLogon();

    VLanTerm();

    return(0);
}
