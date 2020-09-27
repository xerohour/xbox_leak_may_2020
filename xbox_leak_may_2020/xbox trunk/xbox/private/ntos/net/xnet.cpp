// ---------------------------------------------------------------------------------------
// xnet.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xnp.h"
#include "xnver.h"

// ---------------------------------------------------------------------------------------
// Trace Tags
// ---------------------------------------------------------------------------------------

DefineTag(Verbose,      0);                 // Trace verbose messages
DefineTag(Warning,      TAG_ENABLE);        // Trace warnings
DefineTag(pktRecv,      0);                 // Trace packet receives
DefineTag(pktBroadcast, 0);                 // Trace broadcast packet receives
DefineTag(pktXmit,      0);                 // Trace packet transmits
DefineTag(pktWarn,      TAG_ENABLE);        // Trace packet Recv/Xmit uncommon events

// ---------------------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------------------

LONG g_lXnLock = 0;

// ---------------------------------------------------------------------------------------
// XnInit
// ---------------------------------------------------------------------------------------

INT _WSAAPI_ XnInit(void ** ppXn, char * pszXbox, const XNetStartupParams * pxnsp, BOOL fWsa, WORD wVersionRequired, LPWSADATA lpWSAData)
{
    while (InterlockedCompareExchange(&g_lXnLock, 1, 0))
    {
        Sleep(0);
    }

    CXn *   pXn = (CXn *)(*ppXn);
    int     err = 0;

    if (pXn == NULL)
    {
        pXn = new CXn;

        if (pXn == NULL)
        {
            err = RtlNtStatusToDosError(NETERR_MEMORY);
            goto err;
        }

        *ppXn = pXn;

        // Set up XNetInitParams and XNetConfigParams

        XNetInitParams      xnip;
        XNetConfigParams    xncp;

        xnip.pxnp    = (XNetParams *)pxnsp;
        xnip.pszXbox = pszXbox;
        HalQueryTsc(&xnip.liTime);

        memset(&xncp, 0, sizeof(xncp));
        xncp.dwSigEnd = XNET_CONFIG_PARAMS_SIGEND;

        HANDLE hVolume = XNetOpenConfigVolume();

        if (hVolume != INVALID_HANDLE_VALUE)
        {
            if (XNetLoadConfigSector(hVolume, 0, (BYTE *)&xncp, sizeof(xncp)))
            {
                // Extract the rand seed from the config sector and move it into
                // the init params.  If we couldn't read the config sector, just
                // leave the seed with whatever is on the stack.

                Assert(sizeof(xnip.abSeed) == sizeof(xncp.abSeed));
                memcpy(xnip.abSeed, xncp.abSeed, sizeof(xnip.abSeed));
            }
        }

        if (xncp.dwSigEnd != XNET_CONFIG_PARAMS_SIGEND)
        {
            memset(&xncp, 0, sizeof(xncp));
            xncp.dwSigEnd = XNET_CONFIG_PARAMS_SIGEND;
        }

        NTSTATUS status = pXn->SockInit(&xnip);

        if (!NT_SUCCESS(status))
        {
            err = RtlNtStatusToDosError(status);
        }

        if (hVolume != INVALID_HANDLE_VALUE)
        {
            if (err == 0)
            {
                // Write the updated rand seed back to the config sector.

                Assert(sizeof(xnip.abSeed) == sizeof(xncp.abSeed));
                memcpy(xncp.abSeed, xnip.abSeed, sizeof(xncp.abSeed));

                XNetSaveConfigSector(hVolume, 0, (BYTE *)&xncp, sizeof(xncp));
            }

            XNetCloseConfigVolume(hVolume);
        }

        if (err == 0 && (pxnsp == NULL || (pxnsp->cfgFlags & XNET_STARTUP_MANUAL_CONFIG) == 0))
        {
        #if defined(XNET_FEATURE_XBDM_SERVER) && defined(DEVKIT)
            xncp.ina.s_addr = (ULONG)DmTell(DMTELL_GETDBGIP, NULL);
        #endif

            err = pXn->XNetConfig(&xncp, XNET_CONFIG_NORMAL);
        }

        if (err != 0)
        {
            pXn->SockTerm();
            pXn = NULL;
            *ppXn = NULL;
            goto err;
        }

        pXn->SockStart();
    }

    if (fWsa)
    {
        pXn->SockAddRef();

        lpWSAData->wVersion          = wVersionRequired;
        lpWSAData->wHighVersion      = 0x0202;
        lpWSAData->lpVendorInfo      = NULL;
        lpWSAData->iMaxSockets       = 0;
        lpWSAData->iMaxUdpDg         = 0;
        lpWSAData->szDescription[0]  = 0;
        lpWSAData->szSystemStatus[0] = 0;
    }
    else
    {
        pXn->XNetAddRef();
    }

err:

    g_lXnLock = 0;

    if (err != 0)
    {
        TraceSz2(Warning, "%s failed: %d", fWsa ? "WSAStartup" : "XNetStartup", err);

        if (fWsa)
        {
            SetLastError(err);
            err = SOCKET_ERROR;
        }
    }

    return(err);
}

// ---------------------------------------------------------------------------------------
// XnTerm
// ---------------------------------------------------------------------------------------

INT _WSAAPI_ XnTerm(void ** ppXn, BOOL fWsa)
{
    int err = 0;

    while (InterlockedCompareExchange(&g_lXnLock, 1, 0))
    {
        Sleep(0);
    }

    CXn * pXn = (CXn *)*ppXn;

    if (    pXn == NULL
        ||  ( fWsa && pXn->SockGetRefs() == 0)
        ||  (!fWsa && pXn->XNetGetRefs() == 0))
    {
        err = WSANOTINITIALISED;
    }
    else
    {
        if (fWsa)
        {
            if (pXn->SockRelease() == 0)
                pXn->SockShutdown();
        }
        else
        {
            pXn->XNetRelease();
        }

        if (pXn->XNetGetRefs() + pXn->SockGetRefs() == 0)
        {
            pXn->SockTerm();
            delete pXn;
            *ppXn = NULL;
        }
    }

    g_lXnLock = 0;

    if (err != 0)
    {
        TraceSz2(Warning, "%s failed: %d", fWsa ? "WSACleanup" : "XNetCleanup", err);

        if (fWsa)
        {
            SetLastError(err);
            err = SOCKET_ERROR;
        }
    }

    return(err);
}

// ---------------------------------------------------------------------------------------
// Key API's
// ---------------------------------------------------------------------------------------

INT CXn::XNetCreateKey(XNKID * pxnkid, XNKEY * pxnkey)
{
    XNetEnter(XNetCreateKey, pxnkid != NULL && pxnkey != NULL);
    INT err = IpCreateKey(pxnkid, pxnkey);
    XNetLeave(err);
    return(err);
}

INT CXn::XNetRegisterKey(const XNKID * pxnkid, const XNKEY * pxnkey)
{
    XNetEnter(XNetRegisterKey, pxnkid != NULL && pxnkey != NULL);
    INT err = IpRegisterKey(pxnkid, pxnkey);
    XNetLeave(err);
    return(err);
}

INT CXn::XNetUnregisterKey(const XNKID * pxnkid)
{
    XNetEnter(XNetUnregisterKey, pxnkid != NULL);
    INT err = IpUnregisterKey(pxnkid);
    XNetLeave(err);
    return(err);
}

INT CXn::XNetRandom(BYTE * pb, UINT cb)
{
    XNetEnter(XNetRandom, pb != NULL || cb == 0);
    Rand(pb, cb);
    XNetLeave(0);
    return(0);
}

// ---------------------------------------------------------------------------------------
// XnAddr API's
// ---------------------------------------------------------------------------------------

INT CXn::XNetXnAddrToInAddr(const XNADDR * pxna, const XNKID * pxnkid, IN_ADDR * pina)
{
    XNetEnter(XNetXnAddrToInAddr, pxna != NULL && pxnkid != NULL && pina != NULL);
    INT err = IpXnAddrToInAddr(pxna, pxnkid, (CIpAddr *)pina);
    XNetLeave(err);
    return(err);
}

INT CXn::XNetServerToInAddr(const IN_ADDR ina, DWORD dwServiceId, IN_ADDR * pina)
{
    XNetEnter(XNetServerToInAddr, ina.s_addr != 0 && pina != NULL);
#ifdef XNET_FEATURE_SG
    INT err = IpServerToInAddr(CIpAddr(ina.s_addr), dwServiceId, (CIpAddr *)pina);
#else
    INT err = WSAEACCES;
#endif
    XNetLeave(err);
    return(err);
}

INT CXn::XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR * pxna, XNKID * pxnkid)
{
    XNetEnter(XNetInAddrToXnAddr, ina.s_addr != 0);
    INT err = IpInAddrToXnAddr(CIpAddr(ina.s_addr), pxna, pxnkid);
    XNetLeave(err);
    return(err);
}

INT CXn::XNetInAddrToString(const IN_ADDR ina, char * pchBuf, INT cchBuf)
{
    XNetParamCheck(XNetInAddrToString, pchBuf != NULL && cchBuf > 0, WSAEFAULT);
    XnInAddrToString(ina, pchBuf, cchBuf);
    return(0);
}

INT CXn::XNetUnregisterInAddr(const IN_ADDR ina)
{
    XNetEnter(XNetUnregisterInAddr, ina.s_addr != 0);
    INT err = IpUnregisterInAddr(CIpAddr(ina.s_addr));
    XNetLeave(err);
    return(err);
}

DWORD CXn::XNetGetTitleXnAddr(XNADDR * pxna)
{
    XNetInitCheck(XNetGetTitleXnAddr, XNET_GET_XNADDR_NONE);
    XNetParamCheck(XNetGetTitleXnAddr, pxna != NULL, XNET_GET_XNADDR_NONE);

#ifdef XNET_FEATURE_XBDM_SERVER
    return(NicGetOtherXnAddr(pxna));
#else
    return(IpGetXnAddr(pxna));
#endif
}

DWORD CXn::XNetGetDebugXnAddr(XNADDR * pxna)
{
    XNetInitCheck(XNetGetDebugXnAddr, XNET_GET_XNADDR_NONE);
    XNetParamCheck(XNetGetDebugXnAddr, pxna != NULL, XNET_GET_XNADDR_NONE);

#ifdef XNET_FEATURE_XBDM_CLIENT
    return(NicGetOtherXnAddr(pxna));
#elif defined(XNET_FEATURE_XBDM_SERVER)
    return(IpGetXnAddr(pxna));
#else
    return(XNET_GET_XNADDR_NONE);
#endif
}

// ---------------------------------------------------------------------------------------
// Domain Name Service APIs
// ---------------------------------------------------------------------------------------

INT CXn::XNetDnsLookup(const char * pszHost, WSAEVENT hEvent, XNDNS ** ppxndns)
{
    XNetEnter(XNetDnsLookup, pszHost != NULL && ppxndns != NULL);

    #ifdef XNET_FEATURE_DNS
    INT err = IpDnsLookup(pszHost, hEvent, ppxndns);
    #else
    INT err = WSAEACCES;
    #endif

    XNetLeave(err);
    return(err);
}

INT CXn::XNetDnsRelease(XNDNS * pxndns)
{
    XNetEnter(XNetDnsRelease, pxndns != NULL);

    #ifdef XNET_FEATURE_DNS
    INT err = IpDnsRelease(pxndns);
    #else
    INT err = WSAEACCES;
    #endif

    XNetLeave(err);
    return(err);
}

// ---------------------------------------------------------------------------------------
// Quality of Service APIs
// ---------------------------------------------------------------------------------------

INT CXn::XNetQosListen(const XNKID * pxnkid, const BYTE * pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags)
{
    XNetEnter(XNetQosListen, pxnkid != NULL && (!!pb == !!cb) && (dwFlags & ~(XNET_QOS_LISTEN_ENABLE|XNET_QOS_LISTEN_DISABLE|XNET_QOS_LISTEN_SET_DATA|XNET_QOS_LISTEN_SET_BITSPERSEC)) == 0);

    #ifdef XNET_FEATURE_QOS
    INT err = IpQosListen(pxnkid, pb, cb, dwBitsPerSec, dwFlags);
    #else
    INT err = WSAEACCES;
    #endif

    XNetLeave(err);
    return(err);
}

INT CXn::XNetQosXnAddr(UINT cxnqos, const XNADDR * apxna[], const XNKID * apxnkid[], const XNKEY * apxnkey[], DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos)
{
    XNetEnter(XNetQosXnAddr, cxnqos > 0 && apxna != NULL && apxnkid != NULL && apxnkey != NULL && dwFlags == 0 && ppxnqos != NULL);

    #ifdef XNET_FEATURE_QOS
    INT err = IpQosXnAddr(cxnqos, apxna, apxnkid, apxnkey, dwFlags, hEvent, ppxnqos);
    #else
    INT err = WSAEACCES;
    #endif

    XNetLeave(err);
    return(err);
}

INT CXn::XNetQosServer(UINT cxnqos, const IN_ADDR aina[], const DWORD adwServiceId[], DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos)
{
    XNetEnter(XNetQosServer, cxnqos > 0 && aina != NULL && adwServiceId != NULL && dwFlags == 0 && ppxnqos != NULL);

    #if defined(XNET_FEATURE_QOS) && defined(XNET_FEATURE_SG)
    INT err = IpQosServer(cxnqos, aina, adwServiceId, dwFlags, hEvent, ppxnqos);
    #else
    INT err = WSAEACCES;
    #endif

    XNetLeave(err);
    return(err);
}

INT CXn::XNetQosRelease(XNQOS * pxnqos)
{
    XNetEnter(XNetQosRelease, pxnqos != NULL);

    #ifdef XNET_FEATURE_QOS
    INT err = IpQosRelease(pxnqos);
    #else
    INT err = WSAEACCES;
    #endif

    XNetLeave(err);
    return(err);
}

// ---------------------------------------------------------------------------------------
// Config
// ---------------------------------------------------------------------------------------

INT CXn::XNetConfig(const XNetConfigParams * pxncp, DWORD dwFlags)
{
    XNetEnter(XNetConfig, pxncp != NULL);
    INT err = IpConfig(pxncp, dwFlags);
    XNetLeave(err);
    return(err);
}

INT CXn::XNetGetConfigStatus(XNetConfigStatus * pxncs)
{
    XNetEnter(XNetGetConfigStatus, pxncs != NULL);
    INT err = IpGetConfigStatus(pxncs);
    XNetLeave(err);
    return(err);
}

// ---------------------------------------------------------------------------------------
// Main entrypoints for XNet/Winsock APIs
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_XBOX
CXn * g_pXn = NULL;
#endif

#undef  XNETAPI
#define XNETAPI(ret, fname, arglist, paramlist) ret _WSAAPI_ fname arglist { return(GetXn()->fname paramlist); }
#undef  XNETAPI_
#define XNETAPI_(ret, fname, arglist, paramlist)
#undef  XNETAPIV
#define XNETAPIV(ret, fname, arglist, paramlist)

XNETAPILIST()
SOCKAPILIST()

INT _WSAAPI_ XNetStartup(const XNetStartupParams * pxnsp)
{
    XNetParamCheck(XNetStartup, pxnsp == NULL || pxnsp->cfgSizeOfStruct == sizeof(XNetStartupParams), WSAEFAULT);
    return(XnInit((void **)GetXnRef(), GetAchXbox(), pxnsp, FALSE, 0, NULL));
}

int _WSAAPI_ WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData)
{
    WinsockApiPrologTrivial_(WSAStartup);
    WinsockApiParamCheck_(lpWSAData != NULL);
    return(XnInit((void **)GetXnRef(), GetAchXbox(), NULL, TRUE, wVersionRequired, lpWSAData));
}

INT _WSAAPI_ XNetCleanup()
{
    return(XnTerm((void **)GetXnRef(), FALSE));
}

int _WSAAPI_ WSACleanup()
{
    return(XnTerm((void **)GetXnRef(), TRUE));
}

// ---------------------------------------------------------------------------------------
// XNetGetEthernetLinkStatus
//
// This function is placed in the default .text code section so that is can be called
// before the XNET code section is loaded.
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_XBOX
#pragma code_seg(".text")
#endif

DWORD _WSAAPI_ XNetGetEthernetLinkStatus()
{
#ifdef XNET_FEATURE_XBOX
    while (InterlockedCompareExchange(&g_lXnLock, 1, 0))
    {
        Sleep(0);
    }
    
    if (g_pXn == NULL)
    {
        PhyInitialize(FALSE, NULL);
    }

    g_lXnLock = 0;

    return(PhyGetLinkState(FALSE));
#else
    return(XNET_ETHERNET_LINK_ACTIVE|XNET_ETHERNET_LINK_100MBPS|XNET_ETHERNET_LINK_FULL_DUPLEX);
#endif
}
