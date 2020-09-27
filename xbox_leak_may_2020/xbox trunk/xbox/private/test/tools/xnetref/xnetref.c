/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  xnetref.c

Abstract:

  This module is a common library for starting and stopping the xbox net stack

Author:

  Steven Kehrli (steveke) 23-May-2001

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef _DEBUG

extern int t_nicStats;
extern int t_sockWarn;
extern int t_Arp;
extern int t_pktPreAuth;
extern int t_pktXmit;
extern int t_pktRecv;
extern int t_nicStatsAll;
extern int t_pktBroadcast;
extern int t_sock;
extern int t_dhcpDump;
extern int t_pktWarn;
extern int t_poolWarn;
extern int t_tcpRetrans;
extern int t_udpWarn;
extern int t_LeakWarn;
extern int t_Warning;
extern int t_tcpStat;
extern int t_ArpWarn;
extern int t_Verbose;
extern int t_secStat;
extern int t_Timer;
extern int t_poolDump;
extern int t_tcpWarn;

#endif



namespace XNetRefNamespace {

// XNetRefStartupParams is the xnet start parameters
XNetStartupParams  XNetRefStartupParams;
// XNetRefConfigParams is the xnet config parameters
XNetConfigParams   XNetRefConfigParams;

// XNetRefCount is the net ref count
LONG               XNetRefCount = 0;
// hXNetRefMutex is a handle to the net ref mutex
HANDLE             hXNetRefMutex = NULL;

// hXNetRefLog is a handle to the net ref log file
#define XNETREF_LOGFILE_NAME  "T:\\xnetref.log"
HANDLE             hXNetRefLog = INVALID_HANDLE_VALUE;

} // namespace XNetRefNamespace



using namespace XNetRefNamespace;

ULONG
WINAPI
XNetAddRefLocal(
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Initializes the xbox net stack if necessary.  Increases the ref count for the xbox net stack.

Return Value:

  The new reference count.

------------------------------------------------------------------------------*/
{
    // XNetNewRefCount is the new reference count;
    ULONG  XNetNewRefCount = 0;
    // szIniString is the ini string
    char   szIniString[64];
    // iValue is the value of the flag
    INT    iValue;
    // iLastError is the error code returned from XNetStartup
    INT    iLastError = 0;

    // szLogString is the log string
    CHAR   szLogString[1024];
    // cb is the number of bytes written to the log string
    DWORD  cb;



    if (NULL == hXNetRefMutex) {
        hXNetRefMutex = CreateMutexA(NULL, FALSE, "XNetRefMutex");
    }

    if (INVALID_HANDLE_VALUE == hXNetRefLog) {
        hXNetRefLog = CreateFile(XNETREF_LOGFILE_NAME, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    WaitForSingleObject(hXNetRefMutex, INFINITE);

    if (0 == XNetRefCount) {
#ifdef _DEBUG
        iValue = GetProfileIntA("XNetRef", "t_nicStats", -1);
        if (-1 != iValue) {
            t_nicStats = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_sockWarn", -1);
        if (-1 != iValue) {
            t_sockWarn = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_Arp", -1);
        if (-1 != iValue) {
            t_Arp = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_pktPreAuth", -1);
        if (-1 != iValue) {
            t_pktPreAuth = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_pktXmit", -1);
        if (-1 != iValue) {
            t_pktXmit = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_pktRecv", -1);
        if (-1 != iValue) {
            t_pktRecv = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_nicStatsAll", -1);
        if (-1 != iValue) {
            t_nicStatsAll = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_pktBroadcast", -1);
        if (-1 != iValue) {
            t_pktBroadcast = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_sock", -1);
        if (-1 != iValue) {
            t_sock = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_dhcpDump", -1);
        if (-1 != iValue) {
            t_dhcpDump = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_pktWarn", -1);
        if (-1 != iValue) {
            t_pktWarn = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_poolWarn", -1);
        if (-1 != iValue) {
            t_poolWarn = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_tcpRetrans", -1);
        if (-1 != iValue) {
            t_tcpRetrans = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_udpWarn", -1);
        if (-1 != iValue) {
            t_udpWarn = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_LeakWarn", -1);
        if (-1 != iValue) {
            t_LeakWarn = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_Warning", -1);
        if (-1 != iValue) {
            t_Warning = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_tcpStat", -1);
        if (-1 != iValue) {
            t_tcpStat = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_ArpWarn", -1);
        if (-1 != iValue) {
            t_ArpWarn = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_Verbose", -1);
        if (-1 != iValue) {
            t_Verbose = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_secStat", -1);
        if (-1 != iValue) {
            t_secStat = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_Timer", -1);
        if (-1 != iValue) {
            t_Timer = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_poolDump", -1);
        if (-1 != iValue) {
            t_poolDump = iValue;
        }

        iValue = GetProfileIntA("XNetRef", "t_tcpWarn", -1);
        if (-1 != iValue) {
            t_tcpWarn = iValue;
        }
#endif
        ZeroMemory(&XNetRefConfigParams, sizeof(XNetRefConfigParams));

        // Get the config sector
        XNetLoadConfigParams(&XNetRefConfigParams);

        // Get the new ip address
        ZeroMemory(szIniString, sizeof(szIniString));
        if (0 != GetProfileStringA("XNetRef", "config_ina", NULL, szIniString, sizeof(szIniString))) {
            XNetRefConfigParams.ina.s_addr = inet_addr(szIniString);
        }

        // Get the new ip subnet mask address
        ZeroMemory(szIniString, sizeof(szIniString));
        if (0 != GetProfileStringA("XNetRef", "config_inaMask", NULL, szIniString, sizeof(szIniString))) {
            XNetRefConfigParams.inaMask.s_addr = inet_addr(szIniString);
        }

        // Get the new ip gateway address
        ZeroMemory(szIniString, sizeof(szIniString));
        if (0 != GetProfileStringA("XNetRef", "config_inaGateway", NULL, szIniString, sizeof(szIniString))) {
            XNetRefConfigParams.inaGateway.s_addr = inet_addr(szIniString);
        }

        // Get the new primary DNS address
        ZeroMemory(szIniString, sizeof(szIniString));
        if (0 != GetProfileStringA("XNetRef", "config_inaDnsPrimary", NULL, szIniString, sizeof(szIniString))) {
            XNetRefConfigParams.inaDnsPrimary.s_addr = inet_addr(szIniString);
        }

        // Get the new secondary DNS address
        ZeroMemory(szIniString, sizeof(szIniString));
        if (0 != GetProfileStringA("XNetRef", "config_inaDnsSecondary", NULL, szIniString, sizeof(szIniString))) {
            XNetRefConfigParams.inaDnsSecondary.s_addr = inet_addr(szIniString);
        }

        // Get the DHCP host name
        ZeroMemory(szIniString, sizeof(szIniString));
        if (0 != GetProfileStringA("XNetRef", "config_achDhcpHostName", NULL, szIniString, sizeof(szIniString))) {
            strncpy(XNetRefConfigParams.achDhcpHostName, szIniString, sizeof(XNetRefConfigParams.achDhcpHostName) - 1);
        }

        // Get the PPPoE user name
        ZeroMemory(szIniString, sizeof(szIniString));
        if (0 != GetProfileStringA("XNetRef", "config_achPppUserName", NULL, szIniString, sizeof(szIniString))) {
            strncpy(XNetRefConfigParams.achPppUserName, szIniString, sizeof(XNetRefConfigParams.achPppUserName) - 1);
        }

        // Get the PPPoE password
        ZeroMemory(szIniString, sizeof(szIniString));
        if (0 != GetProfileStringA("XNetRef", "config_achPppPassword", NULL, szIniString, sizeof(szIniString))) {
            strncpy(XNetRefConfigParams.achPppPassword, szIniString, sizeof(XNetRefConfigParams.achPppPassword) - 1);
        }

        // Get the PPPoE server name
        ZeroMemory(szIniString, sizeof(szIniString));
        if (0 != GetProfileStringA("XNetRef", "config_achPppServer", NULL, szIniString, sizeof(szIniString))) {
            strncpy(XNetRefConfigParams.achPppServer, szIniString, sizeof(XNetRefConfigParams.achPppServer) - 1);
        }

        // Set the start parameters
        ZeroMemory(&XNetRefStartupParams, sizeof(XNetRefStartupParams));

        XNetRefStartupParams.cfgSizeOfStruct                    = sizeof(XNetRefStartupParams);
        XNetRefStartupParams.cfgFlags                           = (BYTE) GetProfileIntA("XNetRef", "cfgFlags",                           0);
        XNetRefStartupParams.cfgFlags                           |= XNET_STARTUP_MANUAL_CONFIG;
#ifdef XNETREF_I
        XNetRefStartupParams.cfgFlags                           |= XNET_STARTUP_BYPASS_SECURITY;
#endif
        XNetRefStartupParams.cfgPrivatePoolSizeInPages          = (BYTE) GetProfileIntA("XNetRef", "cfgPrivatePoolSizeInPages",          0);
        XNetRefStartupParams.cfgEnetReceiveQueueLength          = (BYTE) GetProfileIntA("XNetRef", "cfgEnetReceiveQueueLength",          0);
        XNetRefStartupParams.cfgIpFragMaxSimultaneous           = (BYTE) GetProfileIntA("XNetRef", "cfgIpFragMaxSimultaneous",           0);
        XNetRefStartupParams.cfgIpFragMaxPacketDiv256           = (BYTE) GetProfileIntA("XNetRef", "cfgIpFragMaxPacketDiv256",           0);
        XNetRefStartupParams.cfgSockMaxSockets                  = (BYTE) GetProfileIntA("XNetRef", "cfgSockMaxSockets",                  0);
        XNetRefStartupParams.cfgSockDefaultRecvBufsizeInK       = (BYTE) GetProfileIntA("XNetRef", "cfgSockDefaultRecvBufsizeInK",       0);
        XNetRefStartupParams.cfgSockDefaultSendBufsizeInK       = (BYTE) GetProfileIntA("XNetRef", "cfgSockDefaultSendBufsizeInK",       0);
        XNetRefStartupParams.cfgKeyRegMax                       = (BYTE) GetProfileIntA("XNetRef", "cfgKeyRegMax",                       0);
        XNetRefStartupParams.cfgSecRegMax                       = (BYTE) GetProfileIntA("XNetRef", "cfgSecRegMax",                       0);

        iLastError = XNetStartup(&XNetRefStartupParams);

        // Set the xnet config parameters
        XNetConfig(&XNetRefConfigParams, XNET_CONFIG_NORMAL);
    }

    if (0 == iLastError) {
        XNetNewRefCount = (ULONG) ++XNetRefCount;
    }

    // Update the log file
    sprintf(szLogString, "XNetAddRef:\r\n");
    WriteFile(hXNetRefLog, szLogString, strlen(szLogString), &cb, NULL);

    sprintf(szLogString, "  File Name:   %s\r\n", lpszFile);
    WriteFile(hXNetRefLog, szLogString, strlen(szLogString), &cb, NULL);

    sprintf(szLogString, "  Line Number: %u\r\n", dwLine);
    WriteFile(hXNetRefLog, szLogString, strlen(szLogString), &cb, NULL);

    sprintf(szLogString, "  Ref Count:   %u\r\n", XNetNewRefCount);
    WriteFile(hXNetRefLog, szLogString, strlen(szLogString), &cb, NULL);

    sprintf(szLogString, "  Error Code:  %u\r\n", iLastError);
    WriteFile(hXNetRefLog, szLogString, strlen(szLogString), &cb, NULL);

    ReleaseMutex(hXNetRefMutex);

    if (0 != iLastError) {
        SetLastError(iLastError);
    }

    return XNetNewRefCount;
}



ULONG
WINAPI
XNetReleaseLocal(
    IN  LPSTR   lpszFile,
    IN  DWORD   dwLine
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Decreases the ref count for the xbox net stack.  Terminates the xbox net stack if necessary.

Return Value:

  The new reference count.

------------------------------------------------------------------------------*/
{
    // XNetNewRefCount is the new reference count;
    ULONG  XNetNewRefCount = 0;

    // szLogString is the log string
    CHAR   szLogString[1024];
    // cb is the number of bytes written to the log string
    DWORD  cb;



    // Synchronize access to this library

    if (NULL == hXNetRefMutex) {
        hXNetRefMutex = CreateMutexA(NULL, FALSE, "XNetRefMutex");
    }

    WaitForSingleObject(hXNetRefMutex, INFINITE);

    // Decrement the ref count
    ASSERT(0 < XNetRefCount);

    if (0 < XNetRefCount) {
        XNetNewRefCount = (ULONG) --XNetRefCount;

        if (0 == XNetRefCount) {
            // Terminate the net stack
            XNetCleanup();

            ZeroMemory(&XNetRefStartupParams, sizeof(XNetRefStartupParams));
            ZeroMemory(&XNetRefConfigParams, sizeof(XNetRefConfigParams));
        }
    }

    // Update the log file
    sprintf(szLogString, "XNetRelease:\r\n");
    WriteFile(hXNetRefLog, szLogString, strlen(szLogString), &cb, NULL);

    sprintf(szLogString, "  File Name:   %s\r\n", lpszFile);
    WriteFile(hXNetRefLog, szLogString, strlen(szLogString), &cb, NULL);

    sprintf(szLogString, "  Line Number: %u\r\n", dwLine);
    WriteFile(hXNetRefLog, szLogString, strlen(szLogString), &cb, NULL);

    sprintf(szLogString, "  Ref Count:   %u\r\n", XNetNewRefCount);
    WriteFile(hXNetRefLog, szLogString, strlen(szLogString), &cb, NULL);

    // Release access to this library
    ReleaseMutex(hXNetRefMutex);

    return XNetNewRefCount;
}



ULONG
WINAPI
XNetRefGetParams(
    XNetStartupParams  *pXNetRefStartupParams,
    XNetConfigParams   *pXNetRefConfigParams
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the xnet start parameters.

Arguments:

  pXNetRefStartupParams - Pointer to the buffer to receive the xnet start parameters
  pXNetRefConfigParams - Pointer to the buffer to receive the xnet config parameters

Return Value:

  The new reference count.

------------------------------------------------------------------------------*/
{
    // XNetNewRefCount is the new reference count;
    ULONG  XNetNewRefCount = 0;



    // Synchronize access to this library

    if (NULL == hXNetRefMutex) {
        hXNetRefMutex = CreateMutexA(NULL, FALSE, "XNetRefMutex");
    }

    WaitForSingleObject(hXNetRefMutex, INFINITE);

    if (0 != XNetRefCount) {
        // Copy the xnet start parameters
        if (NULL != pXNetRefStartupParams) {
            CopyMemory(pXNetRefStartupParams, &XNetRefStartupParams, sizeof(XNetRefStartupParams));
        }

        // Copy the xnet config parameters
        if (NULL != pXNetRefConfigParams) {
            CopyMemory(pXNetRefConfigParams, &XNetRefConfigParams, sizeof(XNetRefConfigParams));
        }
    }

    // Copy the xnet ref count
    XNetNewRefCount = XNetRefCount;

    // Release access to this library
    ReleaseMutex(hXNetRefMutex);

    return XNetNewRefCount;
}
