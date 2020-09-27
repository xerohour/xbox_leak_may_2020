// ----------------------------------------------------------------------------
// test.cpp
//
// Copyright (C) Microsoft Corporation
// ----------------------------------------------------------------------------

#include "xnp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vlan.h>
#include <sgadmin.h>

// ---------------------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------------------

#define TESTSG_START_EVENT_NAME     "$TESTSG$1$EVENT$START$"
#define TESTSG_TERM_EVENT_NAME      "$TESTSG$1$EVENT$TERM$"

// ---------------------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------------------

ULONG   g_ipInet = inet_addr("10.0.0.2");

char    g_szServerConfiguration[] =
"Service    { Id 0 Name TestSg }\n"
"Server     { Id 1 Ip 10.0.0.4 Port 127 Service TestSg }\n"
"Server     { Id 2 Ip 10.0.0.5 Port  80 Service TestSg }\n"
"Server     { Id 3 Ip 10.0.0.6 Port 123 Service TestSg }\n";

char    g_szVirtualNicConfiguration[] =
"NetworkInterface {\n"
"    Type                Internet        ; Interface is connected to the Internet\n"
"    Ip                  10.0.0.2        ; Static IP address of this interface\n"
"    IpMask              255.255.0.0     ; Subnet mask of this interface\n"
"    IpGateway           10.0.0.1        ; Gateway of this interface\n"
"    VLan                sg1@Nic/0       ; VLan this interface is connected to }\n"
"NetworkInterface { \n"
"    Type                Datacenter      ; Interface is connected to the Internet\n"
"    Ip                  10.0.0.3        ; Static IP address of this interface\n"
"    IpMask              255.255.0.0     ; Subnet mask of this interface\n"
"    IpGateway           10.0.0.1        ; Gateway of this interface\n"
"    IpAdmin             10.0.0.100      ; Admin IP address of this interface\n"
"    VLan                sg2@Nic/1       ; VLan this interface is connected to }\n";

char    g_szVirtualOtherConfiguration[] =
"ClientLimit                     64      ; Maximum simultanous clients\n"
"ClientScanFrequencyInSeconds    5       ; How often each client is checked for inactivity\n"
"ClientProbeFrequencyInSeconds   5       ; Inactivity time before probe is sent to client\n"
"ClientKeyExTimeoutInSeconds     10      ; KeyEx inactivity before client presumed dead\n"
"ClientActivityTimeoutInSeconds  20      ; Normal inactivity before client presumed dead\n"
"IpHdrTos                        0       ; IP header type-of-service\n"
"IpHdrTtl                        64      ; IP header time-to-live\n"
"EspReplayWindow                 1024    ; Number of packets sender can get ahead of receiver\n"
"KerberosApReqDeltaInMinutes     5       ; Allowable time-skew for AP request\n"
"HashTableOverheadInPercent      60      ; Hash table bucket overhead above expected maximum\n"
"ArpRequestRetries               4       ; Number of times to retry ARP request\n"
"ArpRexmitTimeoutInSeconds       2       ; Timeout for ARP request\n"
"ArpPositiveTimeoutInSeconds     10      ; Timeout before good ARP request is verified\n"
"ArpNegativeTimeoutInSeconds     10      ; Timeout before bad ARP request is retried\n"
"PacketPool                      64      ; Initial packet pool size (will grow if needed)\n";


char    g_szVirtualSgFormat[] = "sgvlan.exe -s \"%s %s %s\" -e " TESTSG_START_EVENT_NAME " " TESTSG_TERM_EVENT_NAME;

HANDLE  g_hTermVirtualSg;

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
        if (hEvent2) WaitForSingleObject(hEvent2, INFINITE);
        Test();
        if (hEvent1) SetEvent(hEvent1);
        if (hEvent2) WaitForSingleObject(hEvent2, INFINITE);
        Term();
    }

    void UdpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CUdpHdr * pUdpHdr, UINT cbLen) {};
    void TcpRecv(CPacket * ppkt, CIpHdr * pIpHdr, CTcpHdr * pTcpHdr, UINT cbHdrLen, UINT cbLen) {};

    virtual void Test() = 0;
};


// ---------------------------------------------------------------------------------------
// CTestXNet
// ---------------------------------------------------------------------------------------

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
        if (hEvent2) WaitForSingleObject(hEvent2, INFINITE);
        Test();
        if (hEvent1) SetEvent(hEvent1);
        if (hEvent2) WaitForSingleObject(hEvent2, INFINITE);
        Term();
    }

    virtual void Test() = 0;

    SOCKET  SockUdpCreate(CIpPort ipport = 0, BOOL fBroadcast = FALSE);
    void    SockUdpTransmit(SOCKET s, CIpAddr ipaDst, CIpPort ipport);
    void    SockUdpReflect(SOCKET s);
    SOCKET  SockTcpCreate(CIpPort ipport = 0);
    void    SockTcpTransmit(SOCKET s);
    void    SockTcpReflect(SOCKET s);
    void    DiscoveryHost(UINT cClient, XNKID * pxnkid);
    void    DiscoveryClient(XNKID * pxnkid, CIpAddr * pipa);

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

void CTestXNet::SockUdpTransmit(SOCKET s, CIpAddr ipaDst, CIpPort ipport)
{
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ipaDst;
    sin.sin_port = ipport;
    
    BYTE abXmit[1500], abRecv[1500];

    int cb;

    for (cb = 1; cb < UDP_MAXIMUM_MSS; ++cb)
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

void CTestXNet::SockTcpTransmit(SOCKET s)
{
    BYTE abXmit[1500], abRecv[1500];

    int cb;

    for (cb = 1; cb < TCP_MAXIMUM_MSS; ++cb)
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
        WaitForSingleObject(p1.hThread, INFINITE); \
        WaitForSingleObject(p2.hThread, INFINITE); \
        CloseHandle(p1.hEvent1); CloseHandle(p2.hEvent1); \
        CloseHandle(p1.hThread); CloseHandle(p2.hThread); \
    } \
    void Test_##_name() \

#define TWOTHREADRUN(_name) Test_##_name##_run()


// Test_SgUdpCrypt -----------------------------------------------------------------------

//XNETTEST_BEG(SgUdpCrypt_1, NULL, NULL, NULL, "Nic/0:FEFFDEADF00D", 0)
XNETTEST_BEG(SgUdpCrypt_1, "xbsg10@Nic/0", "10.0.0.10", "255.255.0.0", "10.0.0.1", 0)
{
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = g_ipInet;
    Verify(XNetServerToInAddr(inaVip, 0, &inaSec) == 0);
    SOCKET s = SockUdpCreate(0, FALSE);
    Verify(s != SOCKET_ERROR);
    SockUdpTransmit(s, inaSec.s_addr, CIpPort(NTOHS(1)));
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgUdpCrypt_1)

XNETTEST_BEG(SgUdpCrypt_2, "xbsg4@Nic/1", "10.0.0.4", "255.255.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s = SockUdpCreate(HTONS(127));
    SockUdpReflect(s);
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgUdpCrypt_2)

TWOTHREADTEST(SgUdpCrypt)
{
    printf("Testing SgUdpCrypt ");
    TWOTHREADRUN(SgUdpCrypt);
    printf(" [OK]\n");
}

// Test_SgUdpAuth -----------------------------------------------------------------------

//XNETTEST_BEG(SgUdpAuth_1, NULL, NULL, NULL, "Nic/0:FEFFDEADF00D", XNET_STARTUP_BYPASS_ENCRYPTION)
XNETTEST_BEG(SgUdpAuth_1, "xbsg10@Nic/0", "10.0.0.10", "255.255.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_ENCRYPTION)
{
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = g_ipInet;
    Verify(XNetServerToInAddr(inaVip, 0, &inaSec) == 0);
    SOCKET s = SockUdpCreate(0, FALSE);
    Verify(s != SOCKET_ERROR);
    SockUdpTransmit(s, inaSec.s_addr, CIpPort(NTOHS(1)));
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgUdpAuth_1)

XNETTEST_BEG(SgUdpAuth_2, "xbsg4@Nic/1", "10.0.0.4", "255.255.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s = SockUdpCreate(HTONS(127));
    SockUdpReflect(s);
    Verify(closesocket(s) == 0);
}
XNETTEST_END(SgUdpAuth_2)

TWOTHREADTEST(SgUdpAuth)
{
    printf("Testing SgUdpAuth  ");
    TWOTHREADRUN(SgUdpAuth);
    printf(" [OK]\n");
}

// Test_SgTcpCrypt -----------------------------------------------------------------------

//XNETTEST_BEG(SgTcpCrypt_1, NULL, NULL, NULL, "Nic/0:FEFFDEADF00D", 0)
XNETTEST_BEG(SgTcpCrypt_1, "xbsg11@Nic/0", "10.0.0.11", "255.255.0.0", "10.0.0.1", 0)
{
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = g_ipInet;
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
XNETTEST_END(SgTcpCrypt_1)

XNETTEST_BEG(SgTcpCrypt_2, "xbsg5@Nic/1", "10.0.0.5", "255.255.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
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
XNETTEST_END(SgTcpCrypt_2)

TWOTHREADTEST(SgTcpCrypt)
{
    printf("Testing SgTcpCrypt ");
    TWOTHREADRUN(SgTcpCrypt);
    printf(" [OK]\n");
}

// Test_SgTcpAuth -----------------------------------------------------------------------

//XNETTEST_BEG(SgTcpAuth_1, NULL, NULL, NULL, "Nic/0:FEFFDEADF00D", XNET_STARTUP_BYPASS_ENCRYPTION)
XNETTEST_BEG(SgTcpAuth_1, "xbsg11@Nic/0", "10.0.0.11", "255.255.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_ENCRYPTION)
{
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = g_ipInet;
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
XNETTEST_END(SgTcpAuth_1)

XNETTEST_BEG(SgTcpAuth_2, "xbsg5@Nic/1", "10.0.0.5", "255.255.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
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
XNETTEST_END(SgTcpAuth_2)

TWOTHREADTEST(SgTcpAuth)
{
    printf("Testing SgTcpAuth  ");
    TWOTHREADRUN(SgTcpAuth);
    printf(" [OK]\n");
}

// Test_SgMsg ----------------------------------------------------------------------------

//XNETTEST_BEG(SgMsg_1, NULL, NULL, NULL, "Nic/0:FEFFDEADF00D", 0)
XNETTEST_BEG(SgMsg_1, "xbsg10@Nic/0", "10.0.0.10", "255.255.0.0", "10.0.0.1", 0)
{
    Sleep(200);
    IN_ADDR inaVip, inaSec;
    inaVip.s_addr = g_ipInet;
    Verify(XNetServerToInAddr(inaVip, 0, &inaSec) == 0);
    printf("1");
    SOCKET s = SockUdpCreate(0, FALSE);
    Verify(s != SOCKET_ERROR);
    printf("1");
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr = inaSec;
    sin.sin_port = NTOHS(3);
    Verify(sendto(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(inaVip));
    printf("1");
    Verify(closesocket(s) == 0);
    printf("1");
}
XNETTEST_END(SgMsg_1)

XNETTEST_BEG(SgMsg_2, "xbsg6@Nic/1", "10.0.0.6", "255.255.0.0", "10.0.0.1", XNET_STARTUP_BYPASS_SECURITY)
{
    SOCKET s = SockUdpCreate(HTONS(123));

    IN_ADDR inaVip;
    sockaddr_in sin;
    int slen = sizeof(sin);
    
    Verify(recvfrom(s, (char *)&inaVip, sizeof(inaVip), 0, (struct sockaddr *)&sin, &slen) == sizeof(inaVip));
    printf("2");
    inaVip.s_addr = g_ipInet;

    CSgMsgAuthReq   sgar;

    sgar._wType   = SGMSG_TYPE_AUTHDATA_REQ;
    sgar._cbEnt   = sizeof(CSgMsgAuthReq);
    XNetRandom((BYTE *)&sgar._dwReqNum, sizeof(sgar._dwReqNum));
    sgar._ipaZ    = sin.sin_addr;
    sgar._ipportZ = sin.sin_port;
    
    sin.sin_port = NTOHS(0xFFFF);
    Verify(sendto(s, (char *)&sgar, sizeof(sgar), 0, (struct sockaddr *)&sin, sizeof(sin)) == sizeof(sgar));
    printf("2");

    BYTE            ab[CBSGAUTHREPMSG];
    CSgMsgAuthRep * psgap     = (CSgMsgAuthRep *)ab;
    CSgMsgHdr *     psgmh     = (CSgMsgHdr *)(psgap + 1);
    CAuthData *     pAuthData = (CAuthData *)(psgmh + 1);

    slen = sizeof(sin);
    Verify(recvfrom(s, (char *)ab, sizeof(ab), 0, (struct sockaddr *)&sin, &slen) == sizeof(ab));
    printf("2");
    Verify(psgap->_wType == SGMSG_TYPE_AUTHDATA_REP);
    Verify(psgap->_cbEnt == sizeof(CSgMsgAuthRep));
    Verify(psgap->_dwReqNum == sgar._dwReqNum);
    Verify(psgap->_ipaZ.s_addr == sgar._ipaZ.s_addr);
    Verify(psgap->_ipportZ == sgar._ipportZ);
    Verify(psgap->_fNotFound == FALSE);
    Verify(psgap->_bReserved == 0);
//    Verify(psgap->_ipaI.s_addr == inet_addr("10.0.0.10"));
    Verify(psgmh->_wType == SGMSG_TYPE_AUTHDATA);
    Verify(psgmh->_cbEnt == sizeof(CSgMsgHdr) + sizeof(CAuthData));
    Verify(pAuthData->GetCb() == sizeof(CAuthData));

    Verify(closesocket(s) == 0);
    printf("2");
}
XNETTEST_END(SgMsg_2)

TWOTHREADTEST(SgMsg)
{
    printf("Testing SgMsg ");
    TWOTHREADRUN(SgMsg);
    printf("[OK]\n");
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

        if (!fByNameOnly && g_argc == 0)
        {
            return(TRUE);
        }

        for (int i = 0; i < g_argc; ++i)
        {
            if (g_argv[i] && lstrcmpiA(g_argv[i], pszName) == 0)
                return(TRUE);
        }

        pszName = va_arg(va, char *);
    }

    return(FALSE);
}

BOOL RunVirtualSg()
{
    BOOL fReturn;
    STARTUPINFOA StartupInfo;
    PROCESS_INFORMATION ProcessInfo;
    char szCmdLine[sizeof(g_szVirtualSgFormat)
                   + sizeof(g_szVirtualNicConfiguration) 
                   + sizeof(g_szServerConfiguration) 
                   + sizeof(g_szVirtualOtherConfiguration)];

    HANDLE hStart       = CreateEventA(NULL, FALSE, FALSE, TESTSG_START_EVENT_NAME);
    g_hTermVirtualSg    = CreateEventA(NULL, FALSE, FALSE, TESTSG_TERM_EVENT_NAME);

    memset(&StartupInfo, 0, sizeof(StartupInfo));
    sprintf(szCmdLine, g_szVirtualSgFormat, g_szVirtualNicConfiguration, g_szServerConfiguration, g_szVirtualOtherConfiguration);

    fReturn = CreateProcessA(NULL,
                             szCmdLine,
                             NULL,                  // lpProcessAttributes
                             NULL,                  // lpThreadAttributes
                             FALSE,                 // bInheritHandles
                             CREATE_NEW_CONSOLE,    // dwCreationFlags
                             NULL,                  // lpEnvironment
                             NULL,                  // lpCurrentDirectory
                             &StartupInfo,
                             &ProcessInfo);
                             

    if (!fReturn)
    {
        printf("Error %d executing sgvlan.exe\n", GetLastError());
    }
    else
    {
        printf("Waiting for sgvlan to start... ");

        HANDLE ah[2];
        DWORD i;

        ah[0] = hStart;
        ah[1] = ProcessInfo.hProcess;

        i = WaitForMultipleObjects(2, ah, FALSE, INFINITE);

        if (i == WAIT_OBJECT_0)
        {
            printf("[OK]\n");
        }
        else
        {
            printf("[FAIL]\n");
        }

        CloseHandle(hStart);
        CloseHandle(ProcessInfo.hProcess);
        CloseHandle(ProcessInfo.hThread);
    }

    return fReturn;                             
}

void Usage()
{
    printf("Usage:\n"
           "    testsg client <sg-ip-inet> [testlist]\n"
           "    testsg server\n"
           "    testsg virtual\n"
           "\n");
}

void Run()
{
    Verify(VLanInit());

    if (RunTest("SgMsg", "Sg", NULL))                   Test_SgMsg();
    if (RunTest("SgUdpCrypt", "SgUdp", "Sg", NULL))     Test_SgUdpCrypt();
    if (RunTest("SgUdpAuth", "SgUdp", "Sg", NULL))      Test_SgUdpAuth();
    if (RunTest("SgTcpCrypt", "SgTcp", "Sg", NULL))     Test_SgTcpCrypt();
    if (RunTest("SgTcpAuth", "SgTcp", "Sg", NULL))      Test_SgTcpAuth();
    
    VLanTerm();
}

int __cdecl main(int argc, char * argv[])
{
    if (argc < 2)
    {
        Usage();
        return(0);
    }

    g_argc = argc - 2;
    g_argv = argv + 2;

    if (lstrcmpiA(argv[1], "client") == 0)
    {
        if (argc < 3 || inet_addr(argv[2]) == INADDR_NONE)
        {
            printf("Error, testsg client requires an IP address\n\n");
            Usage();
            return(0);
        }

        g_argc--;
        g_argv++;

        g_ipInet = inet_addr(argv[2]);
        Run();
    }
    else if (lstrcmpiA(argv[1], "server") == 0)
    {
        Verify(NT_SUCCESS(SgConfigure(g_szServerConfiguration)));
    }
    else if (lstrcmpiA(argv[1], "virtual") == 0)
    {
        if (!RunVirtualSg())
        {
            return(0);
        }

        Run();

        SetEvent(g_hTermVirtualSg);
        CloseHandle(g_hTermVirtualSg);
    }

    return(0);
}
